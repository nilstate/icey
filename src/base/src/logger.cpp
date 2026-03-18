///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "scy/logger.h"
#include "scy/datetime.h"
#include "scy/platform.h"

#include "scy/filesystem.h"
#include "scy/time.h"
#include "scy/util.h"

#include <chrono>
#include <iterator>
#include <thread>


using std::endl;


namespace scy {


static Singleton<Logger> singleton;


Logger::Logger()
    : _defaultChannel(nullptr)
    , _writer(std::make_unique<LogWriter>())
{
}


Logger::~Logger()
{
    _defaultChannel = nullptr;
    _channels.clear();
}


Logger& Logger::instance()
{
    return *singleton.get();
}


void Logger::setInstance(Logger* logger, bool freeExisting)
{
    auto current = singleton.swap(logger);
    if (current && freeExisting)
        delete current;
}


void Logger::destroy()
{
    singleton.destroy();
}


void Logger::add(std::unique_ptr<LogChannel> channel)
{
    std::lock_guard<std::mutex> guard(_mutex);
    auto* raw = channel.get();
    // The first channel added will be the default channel.
    if (_defaultChannel == nullptr)
        _defaultChannel = raw;
    _channels[raw->name()] = std::move(channel);
}


void Logger::remove(std::string_view name)
{
    std::lock_guard<std::mutex> guard(_mutex);
    auto it = _channels.find(std::string(name));
    if (it == _channels.end())
        throw std::runtime_error("Logger: channel not found: " + std::string(name));
    {
        if (_defaultChannel == it->second.get())
            _defaultChannel = nullptr;
        _channels.erase(it);
    }
}


LogChannel* Logger::get(std::string_view name, bool whiny) const
{
    std::lock_guard<std::mutex> guard(_mutex);
    auto it = _channels.find(std::string(name));
    if (it != _channels.end())
        return it->second.get();
    if (whiny)
        throw std::runtime_error("Not found: No log channel named: " + std::string(name));
    return nullptr;
}


void Logger::setDefault(std::string_view name)
{
    std::lock_guard<std::mutex> guard(_mutex);
    _defaultChannel = get(name, true);
}


LogChannel* Logger::getDefault() const
{
    std::lock_guard<std::mutex> guard(_mutex);
    return _defaultChannel;
}


void Logger::setWriter(std::unique_ptr<LogWriter> writer)
{
    // NOTE: Cannot lock here as writer may
    // attempt to flush pending on destruction.
    _writer = std::move(writer);
}


void Logger::write(const LogStream& stream)
{
    // avoid if possible, requires extra copy
    write(std::make_unique<LogStream>(stream));
}


void Logger::write(std::unique_ptr<LogStream> stream)
{
#ifdef SCY_ENABLE_LOGGING
    std::lock_guard<std::mutex> guard(_mutex);
    if (stream->channel == nullptr)
        stream->channel = _defaultChannel;

    // Drop messages if there is no output channel
    if (stream->channel == nullptr)
        return;
    _writer->write(std::move(stream));
#endif
}


// LogStream& Logger::send(const char* level, const char* realm, const void* ptr,
//                         const char* channel) const
// {
//     return *new LogStream(getLevelFromString(level), realm, 0, ptr, channel);
// }


//
// Log Writer
//


LogWriter::LogWriter()
{
}


LogWriter::~LogWriter()
{
}


void LogWriter::write(std::unique_ptr<LogStream> stream)
{
#ifdef SCY_ENABLE_LOGGING
    stream->channel->write(*stream);
#endif
}


//
// Asynchronous Log Writer
//


AsyncLogWriter::AsyncLogWriter()
{
    _thread.start(std::bind(&AsyncLogWriter::run, this));
}


AsyncLogWriter::~AsyncLogWriter()
{
    // Cancel and wait for the thread
    cancel();

    // Note: Not using join here as it is causing a deadlock
    // when unloading shared libraries when the logger is not
    // explicitly shutdown().
    // while (_thread.running())
    //    scy::sleep(10);
    _thread.join();

    // Flush remaining items synchronously
    flush();
    if (!_pending.empty())
        LWarn("AsyncLogWriter: pending messages on close");
}


void AsyncLogWriter::write(std::unique_ptr<LogStream> stream)
{
    std::lock_guard<std::mutex> guard(_mutex);
    _pending.push_back(std::move(stream));
}


void AsyncLogWriter::clear()
{
    std::lock_guard<std::mutex> guard(_mutex);
    _pending.clear();
}


void AsyncLogWriter::flush()
{
    while (writeNext())
        std::this_thread::yield();
}


void AsyncLogWriter::run()
{
    using namespace std::chrono_literals;
    while (!cancelled()) {
        if (!writeNext())
            std::this_thread::sleep_for(50ms);
    }
}


bool AsyncLogWriter::writeNext()
{
#ifdef SCY_ENABLE_LOGGING
    std::unique_ptr<LogStream> next;
    {
        std::lock_guard<std::mutex> guard(_mutex);
        if (_pending.empty())
            return false;

        next = std::move(_pending.front());
        _pending.pop_front();
    }
    next->channel->write(*next);
    return true;
#else
    return false;
#endif
}


//
// Log Stream
//


#ifdef SCY_ENABLE_LOGGING

LogStream::LogStream(Level level, std::string realm, int line, const char* channel)
    : level(level)
    , line(line)
    , realm(std::move(realm))
    , ts(time::now())
    , channel(channel ? Logger::instance().get(channel, false) : nullptr)
    , flushed(false)
{
}


LogStream::LogStream(const LogStream& that)
    : level(that.level)
    , line(that.line)
    , realm(that.realm)
    , ts(that.ts)
    , channel(that.channel)
    , flushed(that.flushed)
{
    // NOTE: std::stringstream::swap is not implemented on gcc < 5
#if __GNUC__ < 5
    message.str(that.message.str());
#else
    // message.rdbuf()->swap(*that.message.rdbuf());
    message.swap(const_cast<std::ostringstream&>(that.message));
#endif
}


LogStream::~LogStream()
{
    flush();
}

#endif


//
// Log Channel
//


LogChannel::LogChannel(std::string name, Level level,
                       std::string timeFormat)
    : _name(std::move(name))
    , _level(level)
    , _timeFormat(std::move(timeFormat))
{
}


void LogChannel::write(std::string message, Level level,
                       std::string realm)
{
#ifdef SCY_ENABLE_LOGGING
    LogStream stream(level, std::move(realm), 0);
    stream << std::move(message);
    write(stream);
#endif
}


void LogChannel::write(const LogStream& stream)
{
    (void)stream;
}


void LogChannel::format(const LogStream& stream, std::ostream& ost)
{
#ifdef SCY_ENABLE_LOGGING
    if (!_timeFormat.empty())
        ost << time::print(time::toLocal(stream.ts), _timeFormat.c_str());
    ost << " [" << getStringFromLevel(stream.level) << "] ";
    if (!stream.realm.empty()) { // || !stream.address.empty()
        ost << "[";
        if (!stream.realm.empty())
            ost << stream.realm;
        if (stream.line > 0)
            ost << "(" << stream.line << ")";
        // if (!stream.address.empty())
        //     ost << ":" << stream.address;
        ost << "] ";
    }
    ost << stream.message.str();
    ost.flush();
#endif
}


//
// Console Channel
//


ConsoleChannel::ConsoleChannel(std::string name, Level level,
                               std::string timeFormat)
    : LogChannel(std::move(name), level, std::move(timeFormat))
{
}


void ConsoleChannel::write(const LogStream& stream)
{
#ifdef SCY_ENABLE_LOGGING
    if (_level > stream.level)
        return;

    if (!_filter.empty() && !util::matchNodes(stream.realm, _filter, "::"))
        return;

    std::ostringstream ss;
    format(stream, ss);
#if !defined(_WIN32) || defined(_CONSOLE) || defined(_DEBUG)
    std::cout << ss.str() << std::flush;
#endif
//#if defined(_MSC_VER) && defined(_DEBUG)
//    std::string s(ss.str());
//    std::wstring temp(s.length(), L' ');
//    std::copy(s.begin(), s.end(), temp.begin());
//    OutputSDebugtring(temp.c_str());
//#endif
#endif
}


//
// File Channel
//


FileChannel::FileChannel(std::string name, std::string path,
                         Level level, std::string timeFormat)
    : LogChannel(std::move(name), level, std::move(timeFormat))
    , _path(std::move(path))
{
}


FileChannel::~FileChannel()
{
    close();
}


void FileChannel::open()
{
    // Ensure a path was set
    if (_path.empty())
        throw std::runtime_error("Log file path must be set.");

    // Create directories if needed
    fs::mkdirr(fs::dirname(_path));

    // Open the file stream
    _fstream.close();
    _fstream.open(_path.c_str(), std::ios::out | std::ios::app);

    // Throw on failure
    if (!_fstream.is_open())
        throw std::runtime_error("Failed to open log file: " + _path);
}


void FileChannel::close()
{
    _fstream.close();
}


void FileChannel::write(const LogStream& stream)
{
#ifdef SCY_ENABLE_LOGGING
    if (this->level() > stream.level)
        return;

    if (!_fstream.is_open())
        open();

    std::ostringstream ss;
    format(stream, ss);
    _fstream << ss.str() << std::endl;
    _fstream.flush();

#if defined(_CONSOLE) || defined(_DEBUG)
    std::cout << ss.str() << std::flush;
#endif
//#if defined(_MSC_VER) && defined(_DEBUG)
//    std::string s(ss.str());
//    std::wstring temp(s.length(), L' ');
//    std::copy(s.begin(), s.end(), temp.begin());
//    OutputSDebugtring(temp.c_str());
//#endif
#endif
}


void FileChannel::setPath(const std::string& path)
{
    _path = path;
    open();
}


std::string FileChannel::path() const
{
    return _path;
}


//
// Rotating File Channel
//


RotatingFileChannel::RotatingFileChannel(std::string name,
                                         std::string dir,
                                         Level level,
                                         std::string extension,
                                         int rotationInterval,
                                         std::string timeFormat)
    : LogChannel(std::move(name), level, std::move(timeFormat))
    , _dir(std::move(dir))
    , _extension(std::move(extension))
    , _rotationInterval(rotationInterval)
    , _rotatedAt(0)
{
    // The initial log file will be opened on the first call to rotate()
}


RotatingFileChannel::~RotatingFileChannel()
{
    if (_fstream)
        _fstream->close();
}


void RotatingFileChannel::write(const LogStream& stream)
{
#ifdef SCY_ENABLE_LOGGING
    if (this->level() > stream.level)
        return;

    if (!_fstream || stream.ts - _rotatedAt > _rotationInterval)
        rotate();

    std::ostringstream ss;
    format(stream, ss);
    *_fstream << ss.str();
    _fstream->flush();

#if defined(_CONSOLE) && defined(_DEBUG)
    std::cout << ss.str() << std::flush;
#endif
//#if defined(_MSC_VER) && defined(_DEBUG)
//    std::string s(ss.str());
//    std::wstring temp(s.length(), L' ');
//    std::copy(s.begin(), s.end(), temp.begin());
//    OutputSDebugtring(temp.c_str());
//#endif
#endif
}


void RotatingFileChannel::rotate()
{
    if (_fstream)
        _fstream->close();

    // Always try to create the directory
    fs::mkdirr(_dir);

    // Open the next log file
    _filename = util::format("%s_%ld.%s", _name.c_str(),
                             static_cast<long>(Timestamp().epochTime()),
                             _extension.c_str());

    std::string path = fs::makePath(_dir, _filename);
    _fstream = std::make_unique<std::ofstream>(path);
    _rotatedAt = time::now();
}


} // namespace scy


/// @\}
