# JSON

> JSON serialization, file I/O, and configuration

**[API Reference →](../api/json.md)** · **[Source →](../../src/json/)**

## Overview

The `json` module is a thin layer over [nlohmann/json](https://github.com/nlohmann/json) that adds file I/O helpers, a serializable interface, deep traversal utilities, and a JSON-backed configuration class. We use it throughout Icey; `sched`, `pacm`, and several other modules depend on it directly.

The module does not wrap or hide nlohmann/json. `json::Value` is a plain `using` alias for `nlohmann::json`, so the complete nlohmann API is always available without any adapter layer.

**Dependencies:** `base`, `nlohmann/json`

## Architecture

Three components make up the module:

- `json::Value` — the canonical JSON type alias used everywhere in Icey.
- `json::ISerializable` — an abstract interface for objects that round-trip through JSON, with free-function helpers for string-based serialization.
- `json::Configuration` — a JSON-backed implementation of `icy::Configuration` for loading, querying, and saving application settings files.
- Free functions for file I/O (`loadFile`, `saveFile`) and deep-tree traversal (`assertMember`, `countNestedKeys`, `hasNestedKey`, `findNestedObjectWithProperty`).

## Usage

### Reading and writing JSON files

```cpp
#include <icy/json/json.h>

json::Value doc;
icy::json::loadFile("/etc/myapp/config.json", doc);

std::string name = doc["name"].get<std::string>();
doc["version"]   = "2.0.0";

icy::json::saveFile("/etc/myapp/config.json", doc);
```

`loadFile` throws `std::runtime_error` on a missing file or a parse error. `saveFile` throws on a write error. By default `saveFile` writes with 4-space indentation; pass `indent = 0` for compact output.

```cpp
// Compact output.
icy::json::saveFile("/tmp/data.json", doc, 0);
```

### Implementing ISerializable

Implement `serialize()` and `deserialize()` to give a class a standard JSON round-trip.

```cpp
#include <icy/json/iserializable.h>

class ServerConfig : public icy::json::ISerializable
{
public:
    std::string host = "localhost";
    int         port = 8080;

    void serialize(json::Value& root) override
    {
        root["host"] = host;
        root["port"] = port;
    }

    void deserialize(json::Value& root) override
    {
        host = root.value("host", "localhost");
        port = root.value("port", 8080);
    }
};
```

Use the free-function helpers for string-based round-trips:

```cpp
ServerConfig cfg;
cfg.host = "api.example.com";

// Serialize to a pretty-printed string.
std::string json_str;
icy::json::serialize(&cfg, json_str);

// Restore from the string.
ServerConfig cfg2;
icy::json::deserialize(&cfg2, json_str);
```

`serialize()` and `deserialize()` return `false` if the object pointer is null; `deserialize()` also returns `false` if parsing fails.

For file-based round-trips, use `loadFile`/`saveFile` directly with the `json::Value`:

```cpp
json::Value root;
cfg.serialize(root);
icy::json::saveFile("server.json", root);

json::Value loaded;
icy::json::loadFile("server.json", loaded);
cfg2.deserialize(loaded);
```

### Using Configuration

`json::Configuration` extends `icy::Configuration` with typed accessors (`get<T>()`, `set()`), change notifications via `PropertyChanged`, and direct access to the raw `root` value for structured reads.

```cpp
#include <icy/json/configuration.h>

icy::json::Configuration config;
config.load("/etc/myapp/settings.json");

// Typed access through the base Configuration interface.
std::string level = config.get<std::string>("log-level", "info");
config.set("log-level", std::string("debug"));
config.save();

// Direct JSON access for structured or nested data.
json::Value& plugins = config.root["plugins"];
for (auto& p : plugins)
    std::cout << p["id"].get<std::string>() << "\n";
```

`load(path)` sets the file path and parses the file immediately. `load()` (no arguments) reloads from the already-set path; parse errors are silently ignored so the object retains its previous state. `save()` writes `root` back to disk with 4-space indentation.

Manipulating keys:

```cpp
// Remove a single key.
config.remove("deprecated-key");

// Remove all keys whose names contain a substring.
config.removeAll("legacy-");

// Global string substitution on the serialized JSON, then re-parse.
config.replace("http://", "https://");

// List all top-level keys containing a substring.
std::vector<std::string> keys;
config.keys(keys, "plugin-");

// Check whether the file has been loaded at least once.
if (!config.loaded())
    config.load("/etc/myapp/settings.json");
```

### Asserting required members

`assertMember` throws a descriptive `std::runtime_error` if a key is absent, which gives a clear error message at the validation site rather than a cryptic access failure later.

```cpp
#include <icy/json/json.h>

// Throws: "A 'endpoint' member is required."
icy::json::assertMember(doc, "endpoint");
std::string endpoint = doc["endpoint"].get<std::string>();
```

### Traversal helpers

Count how many nested objects contain a key:

```cpp
int count = 0;
icy::json::countNestedKeys(doc, "url", count);
std::cout << "found " << count << " url entries\n";
```

Check whether any nested object contains a key:

```cpp
if (icy::json::hasNestedKey(doc, "deprecated")) {
    // warn
}
```

Find a nested object by a key/value property:

```cpp
json::Value* result = nullptr;
if (icy::json::findNestedObjectWithProperty(doc, result, "id", "myplugin")) {
    std::cout << (*result)["version"].get<std::string>() << "\n";
}
```

`findNestedObjectWithProperty` options:

- Pass an empty `key` to match any key name; pass an empty `value` to match any value.
- `partial = true` (default) uses substring matching on string values.
- `index` selects the Nth match (0 = first).

```cpp
// Find the second object whose "type" field contains "Plugin".
json::Value* second = nullptr;
icy::json::findNestedObjectWithProperty(doc, second, "type", "Plugin",
                                        /*partial=*/true, /*index=*/1);
```

## Configuration class reference

`json::Configuration` key/value access goes through `icy::Configuration::get<T>()` and `set()`. These read and write top-level string keys only. Use `config.root` directly for anything nested or non-string.

The `PropertyChanged` signal (from `icy::Configuration`) fires whenever `set()` changes a value:

```cpp
config.PropertyChanged += [](void*, const std::string& key, const std::string& val) {
    std::cout << key << " changed to " << val << "\n";
};
```

## See Also

- [Sched](sched.md) — `ISerializable` is the persistence interface for scheduled tasks
- [Pacm](pacm.md) — uses `json::Value` as the backing store for package manifests
- [Base](base.md) — `icy::Configuration` base class
