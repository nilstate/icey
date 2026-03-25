{#jsonmodule}

# json

JSON serialization using nlohmann/json.

### Namespaces

| Name | Description |
|------|-------------|
| [`json`](#json) | JSON value aliases, serialization helpers, and file utilities built on nlohmann/json. |

{#json}

# json

JSON value aliases, serialization helpers, and file utilities built on nlohmann/json.

### Classes

| Name | Description |
|------|-------------|
| [`Configuration`](#configuration-1) | JSON configuration file |
| [`ISerializable`](#iserializable) | Abstract interface for JSON-serializable objects. |

### Typedefs

| Return | Name | Description |
|--------|------|-------------|
| `nlohmann::json` | [`Value`](#value)  |  |

---

{#value}

#### Value

```cpp
nlohmann::json Value()
```

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`serialize`](#serialize) `inline` | Serializes `pObj` to a pretty-printed JSON string.  |
| `bool` | [`deserialize`](#deserialize) `inline` | Deserializes `pObj` from a JSON string.  |
| `void` | [`loadFile`](#loadfile) `inline` | Load a JSON file into a value. Throws on missing file or parse error. |
| `void` | [`saveFile`](#savefile-1) `inline` | Save a JSON value to a file. Throws on write error. |
| `void` | [`assertMember`](#assertmember) `inline` | Assert that a required member exists. Throws if missing. |
| `void` | [`countNestedKeys`](#countnestedkeys) `inline` | Count how many nested objects contain the given key. |
| `bool` | [`hasNestedKey`](#hasnestedkey) `inline` | Return true if any nested object contains the given key. |
| `bool` | [`findNestedObjectWithProperty`](#findnestedobjectwithproperty) `inline` | Find a nested object whose property matches the given key/value. |

---

{#serialize}

#### serialize

`inline`

```cpp
inline bool serialize(ISerializable * pObj, std::string & output)
```

Serializes `pObj` to a pretty-printed JSON string. 
#### Parameters
* `pObj` Object to serialize; must not be null. 

* `output` Receives the 4-space indented JSON string. 

#### Returns
true on success, false if `pObj` is null.

| Parameter | Type | Description |
|-----------|------|-------------|
| `pObj` | `[ISerializable](#iserializable) *` |  |
| `output` | `std::string &` |  |

---

{#deserialize}

#### deserialize

`inline`

```cpp
inline bool deserialize(ISerializable * pObj, std::string & input)
```

Deserializes `pObj` from a JSON string. 
#### Parameters
* `pObj` Object to populate; must not be null. 

* `input` JSON string to parse. 

#### Returns
true on success, false if `pObj` is null or parsing fails.

| Parameter | Type | Description |
|-----------|------|-------------|
| `pObj` | `[ISerializable](#iserializable) *` |  |
| `input` | `std::string &` |  |

---

{#loadfile}

#### loadFile

`inline`

```cpp
inline void loadFile(const std::string & path, json::Value & root)
```

Load a JSON file into a value. Throws on missing file or parse error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |
| `root` | `[json::Value](#value) &` |  |

---

{#savefile-1}

#### saveFile

`inline`

```cpp
inline void saveFile(const std::string & path, const json::Value & root, int indent)
```

Save a JSON value to a file. Throws on write error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |
| `root` | `const [json::Value](#value) &` |  |
| `indent` | `int` |  |

---

{#assertmember}

#### assertMember

`inline`

```cpp
inline void assertMember(const json::Value & root, const std::string & name)
```

Assert that a required member exists. Throws if missing.

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `const [json::Value](#value) &` |  |
| `name` | `const std::string &` |  |

---

{#countnestedkeys}

#### countNestedKeys

`inline`

```cpp
inline void countNestedKeys(const json::Value & root, const std::string & key, int & count)
```

Count how many nested objects contain the given key.

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `const [json::Value](#value) &` |  |
| `key` | `const std::string &` |  |
| `count` | `int &` |  |

---

{#hasnestedkey}

#### hasNestedKey

`inline`

```cpp
inline bool hasNestedKey(const json::Value & root, const std::string & key)
```

Return true if any nested object contains the given key.

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `const [json::Value](#value) &` |  |
| `key` | `const std::string &` |  |

---

{#findnestedobjectwithproperty}

#### findNestedObjectWithProperty

`inline`

```cpp
inline bool findNestedObjectWithProperty(json::Value & root, json::Value *& result, std::string_view key, std::string_view value, bool partial, int index)
```

Find a nested object whose property matches the given key/value.

Key or value may be empty for wildcard matching. If partial is true, substring matches are accepted for string values. The index parameter selects the Nth match (0 = first).

Returns true if found, with result pointing to the matching object.

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `[json::Value](#value) &` |  |
| `result` | `[json::Value](#value) *&` |  |
| `key` | `std::string_view` |  |
| `value` | `std::string_view` |  |
| `partial` | `bool` |  |
| `index` | `int` |  |

{#configuration-1}

## Configuration

```cpp
#include <icy/json/configuration.h>
```

> **Inherits:** [`Configuration`](base.md#configuration)

JSON configuration file

See base [Configuration](#configuration-1) for all accessors

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `json::Value` | [`root`](#root)  |  |

---

{#root}

#### root

```cpp
json::Value root
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Configuration`](#configuration-2)  |  |
|  | [`~Configuration`](#configuration-3) `virtual` | Destroys the [Configuration](#configuration-1). |
| `void` | [`load`](#load) `virtual` | Sets the file path and loads the configuration.  |
| `void` | [`load`](#load-1) `virtual` | Reloads the configuration from the previously set path. Silently ignores parse errors (e.g. empty file).  |
| `void` | [`save`](#save) `virtual` | Writes the current JSON root to the file at the stored path.  |
| `bool` | [`remove`](#remove) `virtual` | Removes the top-level key `key` from the JSON root.  |
| `void` | [`removeAll`](#removeall) `virtual` | Removes all top-level keys whose names contain `baseKey` as a substring.  |
| `void` | [`replace`](#replace-2) `virtual` | Performs a global string substitution on the serialized JSON, replacing all occurrences of `from` with `to`, then re-parses.  |
| `void` | [`keys`](#keys) `virtual` | Populates `keys` with all top-level key names containing `baseKey` as a substring.  |
| `void` | [`print`](#print-8) `virtual` | Writes the pretty-printed JSON to `ost` with 4-space indentation.  |
| `std::string` | [`path`](#path-1) `virtual` | Returns the file path that was passed to [load()](#load). |
| `bool` | [`loaded`](#loaded) `virtual` | Returns true if [load()](#load) has been called at least once. |

---

{#configuration-2}

#### Configuration

```cpp
Configuration()
```

---

{#configuration-3}

#### ~Configuration

`virtual`

```cpp
virtual ~Configuration()
```

Destroys the [Configuration](#configuration-1).

---

{#load}

#### load

`virtual`

```cpp
virtual void load(const std::string & path, bool create)
```

Sets the file path and loads the configuration. 
#### Parameters
* `path` Absolute or relative path to the JSON file. 

* `create` Reserved for future use (currently unused). 

#### Exceptions
* `std::runtime_error` if the path is empty.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |
| `create` | `bool` |  |

---

{#load-1}

#### load

`virtual`

```cpp
virtual void load(bool create)
```

Reloads the configuration from the previously set path. Silently ignores parse errors (e.g. empty file). 
#### Parameters
* `create` Reserved for future use (currently unused). 

#### Exceptions
* `std::runtime_error` if the path has not been set.

| Parameter | Type | Description |
|-----------|------|-------------|
| `create` | `bool` |  |

---

{#save}

#### save

`virtual`

```cpp
virtual void save()
```

Writes the current JSON root to the file at the stored path. 
#### Exceptions
* `std::runtime_error` if the path is empty or the file cannot be written.

---

{#remove}

#### remove

`virtual`

```cpp
virtual bool remove(const std::string & key)
```

Removes the top-level key `key` from the JSON root. 
#### Parameters
* `key` Key to remove. 

#### Returns
true if the key existed and was removed.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |

---

{#removeall}

#### removeAll

`virtual`

```cpp
virtual void removeAll(const std::string & baseKey)
```

Removes all top-level keys whose names contain `baseKey` as a substring. 
#### Parameters
* `baseKey` Substring to match against key names.

| Parameter | Type | Description |
|-----------|------|-------------|
| `baseKey` | `const std::string &` |  |

---

{#replace-2}

#### replace

`virtual`

```cpp
virtual void replace(const std::string & from, const std::string & to)
```

Performs a global string substitution on the serialized JSON, replacing all occurrences of `from` with `to`, then re-parses. 
#### Parameters
* `from` Substring to find. 

* `to` Replacement string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `from` | `const std::string &` |  |
| `to` | `const std::string &` |  |

---

{#keys}

#### keys

`virtual`

```cpp
virtual void keys(std::vector< std::string > & keys, const std::string & baseKey)
```

Populates `keys` with all top-level key names containing `baseKey` as a substring. 
#### Parameters
* `keys` Output vector to append matching key names to. 

* `baseKey` Filter substring; empty string matches all keys.

| Parameter | Type | Description |
|-----------|------|-------------|
| `keys` | `std::vector< std::string > &` |  |
| `baseKey` | `const std::string &` |  |

---

{#print-8}

#### print

`virtual`

```cpp
virtual void print(std::ostream & ost)
```

Writes the pretty-printed JSON to `ost` with 4-space indentation. 
#### Parameters
* `ost` Output stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ost` | `std::ostream &` |  |

---

{#path-1}

#### path

`virtual`

```cpp
virtual std::string path()
```

Returns the file path that was passed to [load()](#load).

---

{#loaded}

#### loaded

`virtual`

```cpp
virtual bool loaded()
```

Returns true if [load()](#load) has been called at least once.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`_loaded`](#_loaded)  |  |
| `std::string` | [`_path`](#_path-3)  |  |
| `std::mutex` | [`_mutex`](#_mutex-9)  |  |

---

{#_loaded}

#### _loaded

```cpp
bool _loaded
```

---

{#_path-3}

#### _path

```cpp
std::string _path
```

---

{#_mutex-9}

#### _mutex

```cpp
std::mutex _mutex
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`getRaw`](#getraw) `virtual` `const` | Retrieves the string value for `key` from the JSON root.  |
| `void` | [`setRaw`](#setraw) `virtual` | Stores `value` under `key` in the JSON root and emits PropertyChanged.  |

---

{#getraw}

#### getRaw

`virtual` `const`

```cpp
virtual bool getRaw(const std::string & key, std::string & value) const
```

Retrieves the string value for `key` from the JSON root. 
#### Parameters
* `key` Top-level JSON key. 

* `value` Set to the string value if the key exists. 

#### Returns
true if the key was found, false otherwise.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `std::string &` |  |

---

{#setraw}

#### setRaw

`virtual`

```cpp
virtual void setRaw(const std::string & key, const std::string & value)
```

Stores `value` under `key` in the JSON root and emits PropertyChanged. 
#### Parameters
* `key` Top-level JSON key. 

* `value` String value to store.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `const std::string &` |  |

{#iserializable}

## ISerializable

```cpp
#include <icy/json/iserializable.h>
```

> **Subclassed by:** [`Scheduler`](sched.md#scheduler), [`Task`](sched.md#task-1), [`Trigger`](sched.md#trigger-1)

Abstract interface for JSON-serializable objects.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`serialize`](#serialize-1)  | Serializes this object's state into `root`.  |
| `void` | [`deserialize`](#deserialize-1)  | Populates this object's state from `root`.  |

---

{#serialize-1}

#### serialize

```cpp
void serialize(json::Value & root)
```

Serializes this object's state into `root`. 
#### Parameters
* `root` JSON object to populate.

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `[json::Value](#value) &` |  |

---

{#deserialize-1}

#### deserialize

```cpp
void deserialize(json::Value & root)
```

Populates this object's state from `root`. 
#### Parameters
* `root` JSON object previously produced by [serialize()](#serialize-1).

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `[json::Value](#value) &` |  |

