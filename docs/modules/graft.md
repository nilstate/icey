# Graft

> Native plugin ABI contract and runtime loading helpers

**[API Reference →](../api/graft.md)** · **[Source →](https://github.com/nilstate/icey/tree/main/src/graft/)** · **CMake target:** `icey::graft`

## Overview

The `graft` module is icey's native plugin boundary. It defines the manifest a shared library must export, the runtime kinds the host understands, and the small loader that opens a library, validates its manifest, and resolves the declared entrypoint.

`graft` is intentionally narrow. It does not prescribe a C++ object model across the ABI boundary. Instead, the host and the plugin agree on a manifest plus one typed C entrypoint. That keeps the boundary explicit, versioned, and easier to validate at load time.

Combined with `pacm`, the two modules cover the full lifecycle: `pacm` downloads and installs plugin archives, and `graft` loads those installed libraries into the process and hands the host the plugin entrypoint it asked for.

**Dependencies:** `base`

## Architecture

The runtime contract looks like this:

```text
Host process                         Plugin shared library
──────────────────────────────────   ───────────────────────────────────
graft::Library::open(path)       →   shared library loaded
load manifest symbol             →   icy_graft_manifest exported
validateManifest(...)            →   ABI, id, name, version, runtime checked
entrypoint<GetApiFunc>()         →   named exported function resolved
api = getApi()                   →   host uses the plugin's typed API
```

Four pieces make up that contract:

- `graft::ABI_VERSION` — incremented when the binary contract changes.
- `graft::Manifest` — metadata exported from the plugin under `icy_graft_manifest`.
- `graft::RuntimeKind` — currently `native` or `worker`.
- A typed entrypoint function name stored in the manifest and resolved by `graft::Library`.

## Usage

### Define the shared API header

The host and plugin both include the same API header. That header defines the function table and the entrypoint name the manifest will advertise.

```cpp
#pragma once

#include <icy/graft/graft.h>

namespace myapp::plugin {

struct Api
{
    void (*setValue)(const char* value);
    const char* (*value)();
};

using GetApiFunc = const Api* (*)();

inline constexpr const char* ENTRYPOINT = "myapp_plugin_api";

} // namespace myapp::plugin
```

### Export the manifest from the plugin

The plugin implements its API and exports both the manifest and the entrypoint.

```cpp
#include "mypluginapi.h"

namespace {

void setValue(const char* value) { /* ... */ }
const char* value() { /* ... */ return ""; }

const myapp::plugin::Api API = {
    setValue,
    value,
};

} // namespace

ICY_GRAFT_PLUGIN("my-plugin",
                 "My Plugin",
                 "1.0.0",
                 icy::graft::RUNTIME_NATIVE,
                 myapp::plugin::ENTRYPOINT)

extern "C" ICY_GRAFT_EXPORT const myapp::plugin::Api* myapp_plugin_api()
{
    return &API;
}
```

The manifest is exported as `icy_graft_manifest`. `graft::Library` reads that symbol first, validates it, then resolves the entrypoint named by `manifest().entrypoint`.

### Load the plugin in the host

```cpp
#include <icy/graft/graft.h>
#include "mypluginapi.h"

icy::graft::Library library;
library.open("./libmyplugin.so");

if (icy::graft::parseRuntimeKind(library.manifest().runtime) !=
    icy::graft::RuntimeKind::Native) {
    throw std::runtime_error("unexpected runtime kind");
}

auto getApi = library.entrypoint<myapp::plugin::GetApiFunc>();
const myapp::plugin::Api* api = getApi();
api->setValue("ready");
```

If the manifest is missing required fields, the ABI version does not match, or the runtime string is unknown, `library.open()` throws and closes the library again before returning control to the caller.

### Runtime kinds

`graft` currently defines two runtime strings:

- `graft::RUNTIME_NATIVE` for code loaded directly into the current process.
- `graft::RUNTIME_WORKER` for plugin contracts intended to execute in a worker runtime.

Hosts can use `parseRuntimeKind()` and `runtimeKindName()` to normalize and report that value instead of string-comparing ad hoc.

## Configuration

There is no runtime configuration surface for `graft`. The compatibility boundary is the manifest and the exported entrypoint:

- keep `graft::ABI_VERSION` in sync between host and plugin
- choose a stable entrypoint name and publish it in the shared API header
- reject manifests whose runtime kind does not match the host's execution model

## See Also

- [Pacm](pacm.md) — package download and installation flow for plugin bundles
- [Base](base.md) — `SharedLibrary`, platform macros, and runtime utilities underneath `graft`
