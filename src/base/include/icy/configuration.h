///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include "icy/signal.h"
#include <mutex>
#include <string>
#include <string_view>


namespace icy {


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
    ThreadSignal<void(const std::string&, const std::string&)> PropertyChanged;

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

    /// Parses a decimal or `0x`-prefixed hexadecimal string to int.
    /// @param value String representation of the integer.
    /// @return Parsed integer value.
    static int parseInt(std::string_view value);

    /// Parses a decimal or `0x`-prefixed hexadecimal string to int64.
    /// @param value String representation of the integer.
    /// @return Parsed 64-bit integer value.
    static std::int64_t parseLargeInt(std::string_view value);

    /// Parses a boolean string (true/false/yes/no/on/off/0/1, case-insensitive).
    /// @param value String representation of the boolean.
    /// @return Parsed boolean.
    /// @throws std::runtime_error on unrecognised input.
    static bool parseBool(std::string_view value);

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
    /// @param config       Backing configuration store.
    /// @param currentScope Key prefix for the module-level scope.
    /// @param defaultScope Key prefix for the default/fallback scope.
    ScopedConfiguration(Configuration& config, const std::string& currentScope,
                        const std::string& defaultScope);
    ScopedConfiguration(const ScopedConfiguration& that);

    /// Returns the string value, trying `currentScope` first then `defaultScope`.
    /// @param key              Property key (without scope prefix).
    /// @param defaultValue     Fallback when neither scope has the key.
    /// @param forceDefaultScope If true, skips `currentScope` and reads from `defaultScope` only.
    /// @return Property value or `defaultValue`.
    std::string getString(const std::string& key,
                          const std::string& defaultValue,
                          bool forceDefaultScope = false) const;

    /// Returns the int value, trying `currentScope` first then `defaultScope`.
    /// @param key              Property key (without scope prefix).
    /// @param defaultValue     Fallback when neither scope has the key.
    /// @param forceDefaultScope If true, reads from `defaultScope` only.
    /// @return Property value or `defaultValue`.
    int getInt(const std::string& key, int defaultValue,
               bool forceDefaultScope = false) const;

    /// Returns the double value, trying `currentScope` first then `defaultScope`.
    /// @param key              Property key (without scope prefix).
    /// @param defaultValue     Fallback when neither scope has the key.
    /// @param forceDefaultScope If true, reads from `defaultScope` only.
    /// @return Property value or `defaultValue`.
    double getDouble(const std::string& key, double defaultValue,
                     bool forceDefaultScope = false) const;

    /// Returns the bool value, trying `currentScope` first then `defaultScope`.
    /// @param key              Property key (without scope prefix).
    /// @param defaultValue     Fallback when neither scope has the key.
    /// @param forceDefaultScope If true, reads from `defaultScope` only.
    /// @return Property value or `defaultValue`.
    bool getBool(const std::string& key, bool defaultValue,
                 bool forceDefaultScope = false) const;

    /// Writes a string value under the scoped key.
    /// @param key          Property key (without scope prefix).
    /// @param value        Value to store.
    /// @param defaultScope If true, writes to `defaultScope`; otherwise to `currentScope`.
    void setString(const std::string& key, const std::string& value,
                   bool defaultScope = false);

    /// Writes an int value under the scoped key.
    /// @param key          Property key (without scope prefix).
    /// @param value        Value to store.
    /// @param defaultScope If true, writes to `defaultScope`; otherwise to `currentScope`.
    void setInt(const std::string& key, int value, bool defaultScope = false);

    /// Writes a double value under the scoped key.
    /// @param key          Property key (without scope prefix).
    /// @param value        Value to store.
    /// @param defaultScope If true, writes to `defaultScope`; otherwise to `currentScope`.
    void setDouble(const std::string& key, double value,
                   bool defaultScope = false);

    /// Writes a bool value under the scoped key.
    /// @param key          Property key (without scope prefix).
    /// @param value        Value to store.
    /// @param defaultScope If true, writes to `defaultScope`; otherwise to `currentScope`.
    void setBool(const std::string& key, bool value, bool defaultScope = false);

    /// @param key Property key (without scope prefix).
    /// @return Fully qualified key in `currentScope`.
    std::string getCurrentScope(const std::string& key) const;

    /// @param key Property key (without scope prefix).
    /// @return Fully qualified key in `defaultScope`.
    std::string getDafaultKey(const std::string& key) const;

    /// @param key          Property key (without scope prefix).
    /// @param defaultScope If true, returns the `defaultScope` key; otherwise `currentScope` key.
    /// @return Fully qualified scoped key string.
    std::string getScopedKey(const std::string& key,
                             bool defaultScope = false) const;

    Configuration& config;
    std::string currentScope;
    std::string defaultScope;

private:
    ScopedConfiguration& operator=(const ScopedConfiguration&);
};


} // namespace icy


/// @}
