# Pluga

> Plugin system for loading shared libraries at runtime

**[API Reference →](../api/pluga.md)** · **[Source →](../../src/pluga/)**

## Overview

The `pluga` module provides the ABI contract and the macro scaffolding for a cross-platform C++ plugin system. We use it when we want to extend an application with user-supplied or third-party code that is compiled separately and loaded without restarting the host process.

The module is intentionally minimal. It defines the structures and macros that both sides of the plugin boundary must agree on; it does not implement the actual `dlopen`/`LoadLibrary` calls. Loading the shared library is the host application's responsibility. Pluga provides the known symbol names, the version guard, and the factory function convention that make the host and plugin interoperable.

Combined with `pacm`, the two modules cover the full lifecycle: `pacm` fetches and installs plugin archives from a remote server; `pluga` defines the interface the host uses to load and call into them.

**Dependencies:** `base`

## Architecture

The plugin boundary works as follows:

```text
Host application                    Plugin shared library (.so / .dll)
─────────────────────────────────   ──────────────────────────────────────
dlopen("libmyplugin.so")        →   [library loaded into process]
dlsym(handle, "exports")        →   PluginDetails exports { ... }
  check exports.apiVersion           apiVersion == ICY_PLUGIN_API_VERSION
exports.initializeFunc()        →   getPlugin() { static MyPlugin s; return &s; }
static_cast<IPlugin*>(result)   →   MyPlugin : public IPlugin { ... }
plugin->onLoad()                →   [plugin initialises itself]
```

Three elements make up the contract:

- `ICY_PLUGIN_API_VERSION` — an integer constant incremented on every ABI-breaking change. Both sides must agree on this value.
- `PluginDetails` — a POD struct exported by the plugin under the symbol name `exports`. The host reads it to get metadata and the factory function pointer.
- `IPlugin` — the abstract interface the host defines. `pluga.h` only forward-declares it; the actual interface is part of the application's SDK, shared between host and plugin as a common header.

## Usage

### Defining the shared interface (application SDK header)

The application defines `IPlugin` in a header that both the host and the plugin author include. Pluga forward-declares the class; the application fills in the virtual methods.

```cpp
// myapp/iplugin.h
#pragma once
#include <icy/pluga/pluga.h>

class IPlugin : public icy::pluga::IPlugin
{
public:
    virtual void onLoad()   = 0;
    virtual void onUnload() = 0;
    virtual const char* name() const = 0;
};
```

### Implementing a plugin (plugin side)

The plugin author includes the application's SDK header, implements `IPlugin`, and uses the `ICY_PLUGIN` macro to export the required symbols in a single line.

```cpp
// myplugin.cpp  — compiled as a shared library
#include "myapp/iplugin.h"

class MyPlugin : public IPlugin
{
public:
    void onLoad()   override { /* initialise resources */ }
    void onUnload() override { /* release resources */   }
    const char* name() const override { return "My Plugin"; }
};

ICY_PLUGIN(MyPlugin, "My Plugin", "1.0.0")
```

The macro generates two `extern "C"` symbols:

- `getPlugin()` — returns a pointer to a static `MyPlugin` singleton.
- `exports` — a `PluginDetails` struct containing the API version, file name, class name, plugin name, version string, and a pointer to `getPlugin`.

Compile as a shared library:

```bash
g++ -shared -fPIC -o libmyplugin.so myplugin.cpp -I/path/to/myapp/include
```

### Loading a plugin (host side)

The host loads the library, reads the `exports` symbol, validates the API version, and calls `initializeFunc()` to obtain the `IPlugin*`.

```cpp
#include <icy/pluga/pluga.h>
#include "myapp/iplugin.h"
#include <dlfcn.h>    // POSIX; use LoadLibrary/GetProcAddress on Windows
#include <stdexcept>

void* handle = dlopen("./libmyplugin.so", RTLD_NOW | RTLD_LOCAL);
if (!handle)
    throw std::runtime_error(std::string("dlopen failed: ") + dlerror());

auto* details = static_cast<icy::pluga::PluginDetails*>(
    dlsym(handle, "exports"));
if (!details)
    throw std::runtime_error("Not a pluga plugin: missing 'exports' symbol");

if (details->apiVersion != ICY_PLUGIN_API_VERSION)
    throw std::runtime_error("Plugin API version mismatch");

IPlugin* plugin = static_cast<IPlugin*>(details->initializeFunc());
plugin->onLoad();

// Use the plugin...
std::cout << "Loaded: " << plugin->name() << "\n";
```

### Unloading a plugin

Call `onUnload()` before closing the library handle. The `IPlugin*` is invalid after `dlclose()`.

```cpp
plugin->onUnload();
dlclose(handle);
handle  = nullptr;
plugin  = nullptr;
```

Do not `delete` the `IPlugin*`; it points to a static singleton inside the shared library.

### Managing multiple plugins

A typical host maintains a registry keyed by library path or plugin name:

```cpp
#include <icy/pluga/pluga.h>
#include "myapp/iplugin.h"
#include <dlfcn.h>
#include <unordered_map>
#include <string>

struct LoadedPlugin {
    void* handle;
    IPlugin* plugin;
};

std::unordered_map<std::string, LoadedPlugin> plugins;

void loadPlugin(const std::string& path)
{
    void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!handle)
        throw std::runtime_error(dlerror());

    auto* details = static_cast<icy::pluga::PluginDetails*>(
        dlsym(handle, "exports"));
    if (!details || details->apiVersion != ICY_PLUGIN_API_VERSION) {
        dlclose(handle);
        throw std::runtime_error("Invalid or incompatible plugin: " + path);
    }

    IPlugin* plugin = static_cast<IPlugin*>(details->initializeFunc());
    plugin->onLoad();
    plugins[path] = { handle, plugin };
}

void unloadAll()
{
    for (auto& [path, entry] : plugins) {
        entry.plugin->onUnload();
        dlclose(entry.handle);
    }
    plugins.clear();
}
```

### Integrating with pacm

`pacm` downloads and extracts plugin archives to disk. After a successful install, the host loads each installed `.so`/`.dll` using pluga's convention:

```cpp
#include <icy/pacm/packagemanager.h>
#include <icy/pluga/pluga.h>
#include "myapp/iplugin.h"

// After pm.installPackage("myplugin") completes:
for (auto& [id, pkg] : pm.localPackages().map()) {
    if (!pkg->isInstalled()) continue;
    std::string soPath = pkg->getInstalledFilePath("libmyplugin.so");
    loadPlugin(soPath); // uses the loadPlugin() from above
}
```

## Configuration

There is no runtime configuration for `pluga` itself. The only value that must be kept consistent across the codebase is `ICY_PLUGIN_API_VERSION`. Increment it in `pluga.h` whenever the `IPlugin` interface changes in a way that breaks binary compatibility. Plugins compiled against an older version will be rejected at load time by the API version check.

The `ICY_PLUGIN_EXPORT` macro expands to `__declspec(dllexport)` on Windows and to nothing on all other platforms.

## See Also

- [Pacm](pacm.md) — downloads and installs plugin archives; use together for full remote plugin management
- [Base](base.md) — shared library export macros and platform detection
