{#plugamodule}

# pluga

Plugin system for shared library loading.

### Namespaces

| Name | Description |
|------|-------------|
| [`pluga`](#pluga) | Shared-library plugin discovery helpers and export metadata. |

### Macros

| Name | Description |
|------|-------------|
| [`ICY_PLUGIN_API_VERSION`](#icy_plugin_api_version)  | ABI version expected by the host when loading plugins. Increment this whenever plugin-facing binary compatibility changes. |
| [`ICY_STANDARD_PLUGIN_STUFF`](#icy_standard_plugin_stuff)  | Default metadata prefix used by the `ICY_PLUGIN` export helper. |
| [`ICY_PLUGIN`](#icy_plugin)  | Declares the standard exported entry points and metadata for a plugin class. |

---

{#icy_plugin_api_version}

#### ICY_PLUGIN_API_VERSION

```cpp
ICY_PLUGIN_API_VERSION()
```

ABI version expected by the host when loading plugins. Increment this whenever plugin-facing binary compatibility changes.

---

{#icy_standard_plugin_stuff}

#### ICY_STANDARD_PLUGIN_STUFF

```cpp
ICY_STANDARD_PLUGIN_STUFF()
```

Default metadata prefix used by the `ICY_PLUGIN` export helper.

---

{#icy_plugin}

#### ICY_PLUGIN

```cpp
ICY_PLUGIN()
```

Declares the standard exported entry points and metadata for a plugin class. 
#### Parameters
* `classType` Concrete plugin implementation type. 

* `pluginName` Human-readable plugin name string. 

* `pluginVersion` Plugin version string.

{#pluga}

# pluga

Shared-library plugin discovery helpers and export metadata.

### Classes

| Name | Description |
|------|-------------|
| [`PluginDetails`](#plugindetails) | Metadata descriptor exported alongside a dynamically loaded plugin. |

### Typedefs

| Return | Name | Description |
|--------|------|-------------|
| `IPlugin *(*)()` | [`GetPluginFunc`](#getpluginfunc)  | Function pointer type for the exported plugin entry point. |

---

{#getpluginfunc}

#### GetPluginFunc

```cpp
IPlugin *(*)() GetPluginFunc()
```

Function pointer type for the exported plugin entry point.

{#plugindetails}

## PluginDetails

```cpp
#include <icy/pluga/pluga.h>
```

Metadata descriptor exported alongside a dynamically loaded plugin.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int` | [`apiVersion`](#apiversion)  | ABI version reported by the plugin. |
| `const char *` | [`fileName`](#filename-3)  | Source file used when the plugin export block was compiled. |
| `const char *` | [`className`](#classname-8)  | Concrete plugin class name. |
| `const char *` | [`pluginName`](#pluginname)  | Human-readable plugin display name. |
| `const char *` | [`pluginVersion`](#pluginversion)  | Plugin version string. |
| `GetPluginFunc` | [`initializeFunc`](#initializefunc)  | Entry point that returns the plugin singleton. |

---

{#apiversion}

#### apiVersion

```cpp
int apiVersion
```

ABI version reported by the plugin.

---

{#filename-3}

#### fileName

```cpp
const char * fileName
```

Source file used when the plugin export block was compiled.

---

{#classname-8}

#### className

```cpp
const char * className
```

Concrete plugin class name.

---

{#pluginname}

#### pluginName

```cpp
const char * pluginName
```

Human-readable plugin display name.

---

{#pluginversion}

#### pluginVersion

```cpp
const char * pluginVersion
```

Plugin version string.

---

{#initializefunc}

#### initializeFunc

```cpp
GetPluginFunc initializeFunc
```

Entry point that returns the plugin singleton.

