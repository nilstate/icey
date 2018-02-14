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
#include "scy/filesystem.h"
#include "scy/platform.h"
#include "scy/time.h"
#include "scy/util.h"

#include <assert.h>
#include <iterator>


using std::endl;


namespace scy {


static Singleton<Logger> singleton;


Logger::Logger()
    : _defaultChannel(nullptr)
    , _writer(new LogWriter)
{
    // Decouple C and C++ streams for performance increase.
    // std::cout.sync_with_stdio(false);
}


Logger::~Logger()
  {
  if (_writer)
      delete _writer;
    util::clearMap(_channels);
    _defaultChannel = nullptr;
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


void Logger::add(LogChannel* channel)
{
    std::lock_guard<std::mutex> guard(_mutex);
    // The first channel added will be the default channel.
    if (_defaultChannel == nullptr)
        _defaultChannel = channel;
    _channels[channel->name()] = channel;
}


void Logger::remove(const std::string& name, bool freePointer)
{
    std::lock_guard<std::mutex> guard(_mutex);
    LogChannelMap::iterator it = _channels.find(name);
    assert(it != _channels.end());
    if (it != _channels.end()) {
        if (_defaultChannel == it->second)
            _defaultChannel = nullptr;
        if (freePointer)
            delete it->second;
        _channels.erase(it);
    }
}


LogChannel* Logger::get(const std::string& name, bool whiny) const
{
    std::lock_guard<std::mutex> guard(_mutex);
    LogChannelMap::const_iterator it = _channels.find(name);
    if (it != _channels.end())
        return it->second;
    if (whiny)
        throw std::runtime_error("Not found: No log channel named: " + name);
    return nullptr;
}


void Logger::setDefault(const std::string& name)
{
    std::lock_guard<std::mutex> guard(_mutex);
    _defaultChannel = get(name, true);
}


LogChannel* Logger::getDefault() const
{
    std::lock_guard<std::mutex> guard(_mutex);
    return _defaultChannel;
}


void Logger::setWriter(LogWriter* writer)
{
    // NOTE: Cannot lock here as writer may
    // attempt to flush pending on destruction.
    // std::lock_guard<std::mutex> guard(_mutex);
    if (_writer) {
        delete _writer;
    }
    _writer = writer;
}


void Logger::write(const LogStream& stream)
{
    // avoid if possible, requires extra copy
    write(new LogStream(stream));
}


void Logger::write(LogStream* stream)
{
#ifdef SCY_ENABLE_LOGGING
    std::lock_guard<std::mutex> guard(_mutex);
    if (stream->channel == nullptr)
        stream->channel = _defaultChannel;

    // Drop messages if there is no output channel
    if (stream->channel == nullptr) {
        delete stream;
        return;
    }
    _writer->write(stream);
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


void LogWriter::write(LogStream* stream)
{
#ifdef SCY_ENABLE_LOGGING
    // TODO: Make safer; if the app exists and async stuff
    // is still logging we can end up with a crash here.
    stream->channel->write(*stream);
    delete stream;
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
    assert(_pending.empty());
}


void AsyncLogWriter::write(LogStream* stream)
{
    std::lock_guard<std::mutex> guard(_mutex);
    _pending.push_back(stream);
}


void AsyncLogWriter::clear()
{
    std::lock_guard<std::mutex> guard(_mutex);
    LogStream* next = nullptr;
    while (!_pending.empty()) {
        next = _pending.front();
        delete next;
        _pending.pop_front();
    }
}


void AsyncLogWriter::flush()
{
    while (writeNext())
        scy::sleep(1);
}


void AsyncLogWriter::run()
{
    while (!cancelled()) {
        scy::sleep(writeNext() ? 1 : 50);
    }
}


bool AsyncLogWriter::writeNext()
{
#ifdef SCY_ENABLE_LOGGING
    LogStream* next;
    {
        std::lock_guard<std::mutex> guard(_mutex);
        if (_pending.empty())
            return false;

        next = _pending.front();
        _pending.pop_front();
    }
    next->channel->write(*next);
    delete next;
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
#if !defined(WIN32) || defined(_CONSOLE) || defined(_DEBUG)
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
    , _fstream(nullptr)
    , _dir(std::move(dir))
    , _extension(std::move(extension))
    , _rotationInterval(rotationInterval)
    , _rotatedAt(0)
{
    // The initial log file will be opened on the first call to rotate()
}


RotatingFileChannel::~RotatingFileChannel()
{
    if (_fstream) {
        _fstream->close();
        delete _fstream;
    }
}


void RotatingFileChannel::write(const LogStream& stream)
{
#ifdef SCY_ENABLE_LOGGING
    if (this->level() > stream.level)
        return;

    if (_fstream == nullptr || stream.ts - _rotatedAt > _rotationInterval)
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
    if (_fstream) {
        _fstream->close();
        delete _fstream;
    }

    // Always try to create the directory
    fs::mkdirr(_dir);

    // Open the next log file
    _filename = util::format("%s_%ld.%s", _name.c_str(),
                             static_cast<long>(Timestamp().epochTime()),
                             _extension.c_str());

    std::string path(_dir);
    fs::addnode(path, _filename);
    _fstream = new std::ofstream(path);
    _rotatedAt = time::now();
}


#if 0
// ---------------------------------------------------------------------
// Evented File Channel
//
EventedFileChannel::EventedFileChannel(std::string name,
                         const std::string& dir,
                         Level level,
                         const std::string& extension,
                         int rotationInterval,
                         const char* timeFormat) :
    FileChannel(name, dir, level, extension, rotationInterval, timeFormat)
{
}


EventedFileChannel::~EventedFileChannel()
{
}


void EventedFileChannel::write(const LogStream& stream, Level level, const char* realm, const void* ptr)
{
    if (this->level() > level)
        return;

    FileChannel::write(message, level, ptr);
    OnLogStream.emit(message, level, ptr);
}
#endif


} // namespace scy


/// @\}
