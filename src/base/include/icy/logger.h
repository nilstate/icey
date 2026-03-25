///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include "icy/error.h"
#include "icy/interface.h"
#include "icy/singleton.h"
#include "icy/thread.h"

#include <cstring>
#include <ctime>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string_view>


namespace icy {


/// Logging severity levels used by Logger and LogChannel.
enum class Level
{
    Trace = 0, ///< Most verbose diagnostic output.
    Debug = 1, ///< Debug-only diagnostic output.
    Info = 2,  ///< Normal informational output.
    Warn = 3,  ///< Warning output for recoverable problems.
    Error = 4, ///< Error output for failed operations.
    Fatal = 5, ///< Fatal output immediately before termination.
};


/// Converts a log level string to its corresponding `Level` enum value.
/// Unrecognized strings default to `Level::Trace`.
/// @param level Lowercase level string: "trace", "debug", "info", "warn", "error", or "fatal".
/// @return The matching `Level` enum value.
inline Level getLevelFromString(const char* level)
{
    if (strcmp(level, "trace") == 0)
        return Level::Trace;
    if (strcmp(level, "debug") == 0)
        return Level::Debug;
    if (strcmp(level, "info") == 0)
        return Level::Info;
    if (strcmp(level, "warn") == 0)
        return Level::Warn;
    if (strcmp(level, "error") == 0)
        return Level::Error;
    if (strcmp(level, "fatal") == 0)
        return Level::Fatal;
    return Level::Trace;
}


/// Converts a `Level` enum value to its lowercase string representation.
/// @param level The log level to convert.
/// @return Lowercase C string: "trace", "debug", "info", "warn", "error", or "fatal".
inline const char* getStringFromLevel(Level level)
{
    switch (level) {
        case Level::Trace:
            return "trace";
        case Level::Debug:
            return "debug";
        case Level::Info:
            return "info";
        case Level::Warn:
            return "warn";
        case Level::Error:
            return "error";
        case Level::Fatal:
            return "fatal";
    }
    return "debug";
}


/// Mutable log record accumulated before it is dispatched to a LogChannel.
struct Base_API LogStream;
class Base_API LogChannel;


//
// Default Log Writer
//


/// Log output stream writer.
class Base_API LogWriter
{
public:
    LogWriter();
    virtual ~LogWriter();

    /// Writes the given log message stream.
    virtual void write(std::unique_ptr<LogStream> stream);
};


//
// Asynchronous Log Writer
//


/// Thread based log output stream writer.
class Base_API AsyncLogWriter : public LogWriter
    , public basic::Runnable
{
public:
    AsyncLogWriter();
    virtual ~AsyncLogWriter();

    /// Queues the given log message stream.
    virtual void write(std::unique_ptr<LogStream> stream) override;

    /// Flushes queued messages.
    void flush();

    /// Writes queued messages asynchronously.
    void run() override;

    /// Clears all queued messages.
    void clear();

protected:
    bool writeNext();

    Thread _thread;
    std::deque<std::unique_ptr<LogStream>> _pending;
    mutable std::mutex _mutex;
};


//
// Logger
//


/// Logger class.
class Base_API Logger
{
public:
    Logger();
    ~Logger();

    /// Returns the default logger singleton.
    /// Logger instances may be created separately as needed.
    static Logger& instance();

    /// Sets the default logger singleton instance.
    static void setInstance(Logger* logger, bool freeExisting = true);

    /// Destroys the default logger singleton instance.
    static void destroy();

    /// Adds the given log channel. Takes ownership.
    void add(std::unique_ptr<LogChannel> channel);

    /// Removes the given log channel by name.
    void remove(std::string_view name);

    /// Returns the specified log channel.
    /// Throws an exception if the channel doesn't exist.
    LogChannel* get(std::string_view name, bool whiny = true) const;

    /// Sets the default log to the specified log channel.
    void setDefault(std::string_view name);

    /// Sets the log writer instance. Takes ownership.
    void setWriter(std::unique_ptr<LogWriter> writer);

    /// Returns the default log channel, or the nullptr channel
    /// if no default channel has been set.
    LogChannel* getDefault() const;

    /// Writes the given message to the default log channel.
    /// The message will be copied.
    void write(const LogStream& stream);

    /// Writes the given message to the default log channel.
    void write(std::unique_ptr<LogStream> stream);

protected:
    /// NonCopyable and NonMovable
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    using LogChannelMap = std::map<std::string, std::unique_ptr<LogChannel>>;

    friend class Singleton<Logger>;
    friend class Thread;

    mutable std::mutex _mutex;
    LogChannelMap _channels;
    LogChannel* _defaultChannel; // non-owning view into _channels
    std::unique_ptr<LogWriter> _writer;
};


//
// Recursive logging for veradic arguments
//


template <typename T>
void logArgs(std::ostream& o, T&& t)
{
    o << t; // << ' '; // << '\n';
}

template <typename T, typename... Args>
void logArgs(std::ostream& o, T&& t, Args&&... args) // recursive variadic function
{
    logArgs(o, std::forward<T>(t));
    logArgs(o, std::forward<Args>(args)...);
}


//
// Log Stream
//


#ifdef ICY_ENABLE_LOGGING

/// Mutable log record accumulated before it is dispatched to a LogChannel.
struct LogStream
{
    Level level;
    int line;
    std::string realm;
    std::ostringstream message;
    std::time_t ts;
    LogChannel* channel;
    bool flushed;

    LogStream(Level level, std::string realm, int line, const char* channel = nullptr);
    LogStream(const LogStream& that);
    ~LogStream();

    /// Recursively log veradic arguments.
    template <typename... Args>
    void write(Args... args)
    {
        logArgs(message, args...);
        flush();
    }

    /// Write data to the log message.
    template <typename T>
    LogStream& operator<<(const T& data)
    {
        message << data;
        return *this;
    }

    LogStream& operator<<(const Level data)
    {
        level = data;
        return *this;
    }

    LogStream& operator<<(LogChannel* data)
    {
        channel = data;
        return *this;
    }

    /// Handle std::endl manipulators.
    /// This method flushes the log message and queues it for write.
    LogStream& operator<<(std::ostream& (*f)(std::ostream&))
    {
        flush();
        return *this;
    }

    void flush()
    {
        if (!flushed) {
            flushed = true;
            message << '\n';
            Logger::instance().write(std::make_unique<LogStream>(*this));
        }
    }
};

#else

/// No-op log record used when logging is compiled out.
struct LogStream
{
    LogStream(Level level, std::string realm, int line, const char* channel = nullptr) {}
    LogStream(const LogStream& that) {}
    ~LogStream() = default;

    template <typename... Args>
    void write(Args... args)
    {
    }

    template <typename T>
    LogStream& operator<<(const T&)
    {
        return *this;
    }

    LogStream& operator<<(std::ostream& (*f)(std::ostream&))
    {
        return *this;
    }
};

#endif


//
// Log Channel
//


/// Named log output channel with configurable severity level and formatting
class Base_API LogChannel
{
public:
    /// @param name Unique channel name.
    /// @param level Minimum severity level; messages below this level are dropped.
    /// @param timeFormat strftime-compatible format string for timestamps.
    LogChannel(std::string name, Level level = Level::Debug,
               std::string timeFormat = "%H:%M:%S");
    virtual ~LogChannel() = default;

    /// Writes a log stream entry to this channel.
    /// @param stream The log stream to write.
    virtual void write(const LogStream& stream);

    /// Writes a plain message to this channel.
    /// @param message Log message text.
    /// @param level Severity level for the message.
    /// @param realm Optional source realm (e.g. file or class name).
    virtual void write(std::string message, Level level = Level::Debug,
                       std::string realm = "");

    /// Formats a log stream entry into the given output stream.
    /// @param stream The log stream to format.
    /// @param ost The output stream to write the formatted message into.
    virtual void format(const LogStream& stream, std::ostream& ost);

    /// Returns the channel name.
    std::string name() const { return _name; };

    /// Returns the minimum severity level.
    Level level() const { return _level; };

    /// Returns the timestamp format string.
    std::string timeFormat() const { return _timeFormat; };

    /// Sets the minimum severity level.
    /// @param level Messages below this level are dropped.
    void setLevel(Level level) { _level = level; };

    /// Sets the timestamp format string.
    /// @param format strftime-compatible format string.
    void setTimeFormat(std::string format) { _timeFormat = std::move(format); };

    /// Sets a realm filter; only messages whose realm matches are written.
    /// @param filter Realm substring or pattern to match against.
    void setFilter(std::string filter) { _filter = std::move(filter); }

protected:
    std::string _name;
    Level _level;
    std::string _timeFormat;
    std::string _filter;
};


/// Null log channel.
///
/// Redifine the base `LogChannel` as `NullChannel` so it can be logically used
/// as a disabled log channel.
using NullChannel = LogChannel;


//
// Console Channel
//


/// Log channel that writes formatted messages to standard output
class Base_API ConsoleChannel : public LogChannel
{
public:
    /// @param name Unique channel name.
    /// @param level Minimum severity level; messages below this level are dropped.
    /// @param timeFormat strftime-compatible format string for timestamps.
    ConsoleChannel(std::string name, Level level = Level::Debug,
                   std::string timeFormat = "%H:%M:%S");
    virtual ~ConsoleChannel() = default;

    /// Formats and writes the log stream entry to stdout.
    /// Messages below the channel level or filtered by realm are silently dropped.
    /// @param stream The log stream to write.
    void write(const LogStream& stream) override;
};


//
// File Channel
//


/// Log channel that writes formatted messages to a file
class Base_API FileChannel : public LogChannel
{
public:
    /// @param name Unique channel name.
    /// @param path Path to the output log file. Directories are created if needed.
    /// @param level Minimum severity level; messages below this level are dropped.
    /// @param timeFormat strftime-compatible format string for timestamps.
    FileChannel(std::string name, std::string path,
                Level level = Level::Debug,
                std::string timeFormat = "%H:%M:%S");
    virtual ~FileChannel();

    /// Formats and writes the log stream entry to the file.
    /// Opens the file on first write if not already open.
    /// @param stream The log stream to write.
    void write(const LogStream& stream) override;

    /// Sets the file path and reopens the file stream.
    /// @param path Path to the new log file.
    void setPath(const std::string& path);

    /// Returns the current log file path.
    /// @return Absolute or relative path to the log file.
    std::string path() const;

protected:
    virtual void open();
    virtual void close();

protected:
    std::ofstream _fstream;
    std::string _path;
};


//
// Rotating File Channel
//


/// Log channel that writes to time-rotated log files
class Base_API RotatingFileChannel : public LogChannel
{
public:
    /// @param name Unique channel name.
    /// @param dir Directory in which rotated log files are written.
    /// @param level Minimum severity level; messages below this level are dropped.
    /// @param extension File extension for log files (without leading dot).
    /// @param rotationInterval Seconds between log rotations (default: 12 hours).
    /// @param timeFormat strftime-compatible format string for timestamps.
    RotatingFileChannel(std::string name,
                        std::string dir,
                        Level level = Level::Debug,
                        std::string extension = "log",
                        int rotationInterval = 12 * 3600, // 12 hours
                        std::string timeFormat = "%H:%M:%S");
    virtual ~RotatingFileChannel();

    /// Formats and writes the log stream entry to the current log file.
    /// Rotates the file if the rotation interval has elapsed.
    /// @param stream The log stream to write.
    void write(const LogStream& stream) override;

    /// Closes the current log file and opens a new one with a timestamped filename.
    virtual void rotate();

    /// Returns the directory where log files are written.
    std::string dir() const { return _dir; };

    /// Returns the filename of the currently open log file.
    std::string filename() const { return _filename; };

    /// Returns the rotation interval in seconds.
    int rotationInterval() const { return _rotationInterval; };

    /// Sets the output directory for rotated log files.
    /// @param dir Target directory path.
    void setDir(std::string dir) { _dir = std::move(dir); };

    /// Sets the file extension for rotated log files.
    /// @param ext Extension without leading dot (e.g. "log").
    void setExtension(std::string ext) { _extension = std::move(ext); };

    /// Sets the rotation interval.
    /// @param interval Number of seconds between rotations.
    void setRotationInterval(int interval) { _rotationInterval = interval; };

protected:
    std::unique_ptr<std::ofstream> _fstream;
    std::string _dir;
    std::string _filename;
    std::string _extension;
    int _rotationInterval; ///< The log rotation interval in seconds
    time_t _rotatedAt;     ///< The time the log was last rotated
};


//
// Compile time helpers for replacing file paths with filenames
//


constexpr const char* str_end(const char* str)
{
    return *str ? str_end(str + 1) : str;
}

constexpr bool str_slant(const char* str)
{
    return *str == '/' || *str == '\\' ? true : (*str ? str_slant(str + 1) : false);
}

constexpr const char* r_slant(const char* str)
{
    return *str == '/' || *str == '\\' ? (str + 1) : r_slant(str - 1);
}

constexpr const char* _fileName(const char* str)
{
    return str_slant(str) ? r_slant(str_end(str)) : str;
}


//
// Logging macros
//


// Macro for logging class functions
#ifndef __CLASS_FUNCTION__
#if _MSC_VER
#define __CLASS_FUNCTION__ __FUNCTION__
#else
inline std::string _methodName(std::string_view fsig)
{
    size_t colons = fsig.find("::");
    size_t sbeg = fsig.substr(0, colons).rfind(" ") + 1;
    size_t send = fsig.rfind("(") - sbeg;
    return std::string(fsig.substr(sbeg, send)) + "()";
}
#define __CLASS_FUNCTION__ _methodName(__PRETTY_FUNCTION__)
#endif
#endif


#define STrace LogStream(Level::Trace, _fileName(__FILE__), __LINE__)
#define SDebug LogStream(Level::Debug, _fileName(__FILE__), __LINE__)
#define SInfo LogStream(Level::Info, _fileName(__FILE__), __LINE__)
#define SWarn LogStream(Level::Warn, _fileName(__FILE__), __LINE__)
#define SError LogStream(Level::Error, _fileName(__FILE__), __LINE__)

#define LTrace(...)                                                                \
    do {                                                                           \
        LogStream(Level::Trace, _fileName(__FILE__), __LINE__).write(__VA_ARGS__); \
    } while (0)
#define LDebug(...)                                                                \
    do {                                                                           \
        LogStream(Level::Debug, _fileName(__FILE__), __LINE__).write(__VA_ARGS__); \
    } while (0)
#define LInfo(...)                                                                \
    do {                                                                          \
        LogStream(Level::Info, _fileName(__FILE__), __LINE__).write(__VA_ARGS__); \
    } while (0)
#define LWarn(...)                                                                \
    do {                                                                          \
        LogStream(Level::Warn, _fileName(__FILE__), __LINE__).write(__VA_ARGS__); \
    } while (0)
#define LError(...)                                                                \
    do {                                                                           \
        LogStream(Level::Error, _fileName(__FILE__), __LINE__).write(__VA_ARGS__); \
    } while (0)

// #define TraceS(self) LogStream(Level::Trace, _fileName(__FILE__), __LINE__, self)
// #define DebugS(self) LogStream(Level::Debug, _fileName(__FILE__), __LINE__, self)
// #define InfoS(self)  LogStream(Level::Info, _fileName(__FILE__), __LINE__, self)
// #define WarnS(self)  LogStream(Level::Warn, _fileName(__FILE__), __LINE__, self)
// #define ErrorS(self) LogStream(Level::Error, _fileName(__FILE__), __LINE__, self)

// #define TraceN(self) LogStream(Level::Trace, self->className(), __LINE__, self)
// #define DebugN(self) LogStream(Level::Debug, self->className(), __LINE__, self)
// #define InfoN(self)  LogStream(Level::Info, self->className(), __LINE__, self)
// #define WarnN(self)  LogStream(Level::Warn, self->className(), __LINE__, self)
// #define ErrorN(self) LogStream(Level::Error, self->className(), __LINE__, self)


} // namespace icy


/// @}
