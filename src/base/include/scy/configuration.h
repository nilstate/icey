///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "scy/base.h"
#include "scy/signal.h"
#include <mutex>
#include <string>


namespace scy {


/// Configuration is an abstract base class for managing different kinds of
/// configuration storage back ends such as JSON, XML, or database.
///
/// Subclasses must override the getRaw() and setRaw() and methods.
///
/// This class is safe for multithreaded use.
class Base_API Configuration
{
public:
    /// Creates the Configuration.
    Configuration();

    /// Destroys the Configuration.
    virtual ~Configuration();

    /// Returns true if the property with the given key exists.
    bool exists(const std::string& key) const;

    /// Returns the string value of the property with the given name.
    /// Throws a NotFoundException if the key does not exist.
    std::string getString(const std::string& key) const;

    /// If a property with the given key exists, returns the property's string
    /// value, otherwise returns the given default value.
    std::string getString(const std::string& key, const std::string& defaultValue) const;

    /// Returns the raw string value of the property with the given name.
    /// Throws a NotFoundException if the key does not exist.
    /// References to other properties are not expanded.
    std::string getRawString(const std::string& key) const;

    /// If a property with the given key exists, returns the property's raw
    /// string value, otherwise returns the given default value.
    /// References to other properties are not expanded.
    std::string getRawString(const std::string& key, const std::string& defaultValue) const;

    /// Returns the int value of the property with the given name.
    /// Throws a NotFoundException if the key does not exist.
    /// Throws a SyntaxException if the property can not be converted
    /// to an int.
    /// Numbers starting with 0x are treated as hexadecimal.
    int getInt(const std::string& key) const;

    /// If a property with the given key exists, returns the property's int
    /// value, otherwise returns the given default value.
    /// Throws a SyntaxException if the property can not be converted
    /// to an int.
    /// Numbers starting with 0x are treated as hexadecimal.
    int getInt(const std::string& key, int defaultValue) const;

    /// Returns the int value of the property with the given name.
    /// Throws a NotFoundException if the key does not exist.
    /// Throws a SyntaxException if the property can not be converted
    /// to an int.
    /// Numbers starting with 0x are treated as hexadecimal.
    std::int64_t getLargeInt(const std::string& key) const;

    /// If a property with the given key exists, returns the property's int
    /// value,
    /// otherwise returns the given default value.
    /// Throws a SyntaxException if the property can not be converted
    /// to an int.
    /// Numbers starting with 0x are treated as hexadecimal.
    std::int64_t getLargeInt(const std::string& key, std::int64_t defaultValue) const;

    /// Returns the double value of the property with the given name.
    /// Throws a NotFoundException if the key does not exist.
    /// Throws a SyntaxException if the property can not be converted
    /// to a double.
    double getDouble(const std::string& key) const;

    /// If a property with the given key exists, returns the property's double
    /// value, otherwise returns the given default value.
    /// Throws a SyntaxException if the property can not be converted
    /// to an double.
    double getDouble(const std::string& key, double defaultValue) const;

    /// Returns the double value of the property with the given name.
    /// Throws a NotFoundException if the key does not exist.
    /// Throws a SyntaxException if the property can not be converted
    /// to a double.
    bool getBool(const std::string& key) const;

    /// If a property with the given key exists, returns the property's bool
    /// value, otherwise returns the given default value.
    /// Throws a SyntaxException if the property can not be converted to a boolean.
    /// The following string values can be converted into a boolean:
    ///   - numerical values: non zero becomes true, zero becomes false
    ///   - strings: true, yes, on become true, false, no, off become false
    /// Case does not matter.
    bool getBool(const std::string& key, bool defaultValue) const;

    /// Sets the property with the given key to the given value.
    /// An already existing value for the key is overwritten.
    void setString(const std::string& key, const std::string& value);

    /// Sets the property with the given key to the given value.
    /// An already existing value for the key is overwritten.
    void setInt(const std::string& key, int value);

    /// Sets the property with the given key to the given value.
    /// An already existing value for the key is overwritten.
    void setLargeInt(const std::string& key, std::int64_t value);

    /// Sets the property with the given key to the given value.
    /// An already existing value for the key is overwritten.
    void setDouble(const std::string& key, double value);

    /// Sets the property with the given key to the given value.
    /// An already existing value for the key is overwritten.
    void setBool(const std::string& key, bool value);

    /// The Key and Value of the changed configuration property.
    Signal<void(const std::string&, const std::string&)> PropertyChanged;

protected:
    /// If the property with the given key exists, stores the property's value
    /// in value and returns true. Otherwise, returns false.
    ///
    /// Must be overridden by subclasses.
    virtual bool getRaw(const std::string& key, std::string& value) const = 0;

    /// Sets the property with the given key to the given value.
    /// An already existing value for the key is overwritten.
    ///
    /// The implementation is responsible for emitting the PropertyChanged signal.
    virtual void setRaw(const std::string& key, const std::string& value) = 0;

    static int parseInt(const std::string& value);
    static std::int64_t parseLargeInt(const std::string& value);
    static bool parseBool(const std::string& value);

protected:
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;
    Configuration(Configuration&&) = delete;
    Configuration& operator=(Configuration&&) = delete;

private:
    mutable std::mutex _mutex;
};


//
// Scoped Configuration
//


/// ScopedConfiguration provides multiple levels of configuration for a module.
/// Multiple levels means that there is a module level scope, and a default
/// scope.
/// When a property is accessed, the module scope value will be used if
/// available,
/// otherwise the default scope value will be used.
///
/// Example scoping:
///        Module: channels.[name].modes.[name].[value]
///        Default: modes.[name].[value]
class Base_API ScopedConfiguration
{
public:
    ScopedConfiguration(Configuration& config, const std::string& currentScope,
                        const std::string& defaultScope);
    ScopedConfiguration(const ScopedConfiguration& that);

    std::string getString(const std::string& key,
                          const std::string& defaultValue,
                          bool forceDefaultScope = false) const;
    int getInt(const std::string& key, int defaultValue,
               bool forceDefaultScope = false) const;
    double getDouble(const std::string& key, double defaultValue,
                     bool forceDefaultScope = false) const;
    bool getBool(const std::string& key, bool defaultValue,
                 bool forceDefaultScope = false) const;

    void setString(const std::string& key, const std::string& value,
                   bool defaultScope = false);
    void setInt(const std::string& key, int value, bool defaultScope = false);
    void setDouble(const std::string& key, double value,
                   bool defaultScope = false);
    void setBool(const std::string& key, bool value, bool defaultScope = false);

    std::string getCurrentScope(const std::string& key) const;
    std::string getDafaultKey(const std::string& key) const;
    std::string getScopedKey(const std::string& key,
                             bool defaultScope = false) const;

    Configuration& config;
    std::string currentScope;
    std::string defaultScope;

private:
    ScopedConfiguration& operator=(const ScopedConfiguration&);
};


} // namespace scy


/// @\}


// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or subclass
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
