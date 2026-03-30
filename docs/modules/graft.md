# Graft

> Native plugin ABI and runtime loading for trusted shared-library extensions

**[API Reference →](../api/graft.md)** · **[Source →](https://github.com/nilstate/icey/tree/main/src/graft/)**

## Overview

The `graft` module is the native plugin boundary for icey. We use it when a host process needs to load a trusted shared library, validate its ABI contract, and bind a typed entrypoint without inventing a custom `dlopen` protocol each time.

`graft` is intentionally narrower than a full extension platform:

- it defines a manifest exported from the library
- it validates ABI version, runtime kind, and declared entrypoint
- it loads symbols through `SharedLibrary`
- it does not download, install, or update packages

That division is deliberate. `pacm` owns package delivery and install layout. `graft` owns native shared-library binding once the package is already on disk.

**Dependencies:** `base`

## Contract

Every graft-native library exports a manifest under the symbol `icy_graft_manifest`.

The manifest declares:

- plugin id
- display name
- version
- runtime kind (`native` or `worker`)
- entrypoint symbol name
- ABI version

The host opens the shared library through `graft::Library`, which:

1. opens the shared object
2. resolves `icy_graft_manifest`
3. validates the ABI and runtime fields
4. resolves the declared entrypoint when asked

The important difference from the old `pluga` shape is that `graft` does not force a shared C++ class ABI. The plugin-specific API can be a function table or any other C-compatible symbol contract.

## Usage

### Exporting a native plugin

```cpp
#include <icy/graft/graft.h>

namespace myplugin {
inline constexpr const char* ENTRYPOINT = "my_plugin_api";
}

ICY_GRAFT_PLUGIN("motion-detector",
                 "Motion Detector",
                 "1.0.0",
                 icy::graft::RUNTIME_NATIVE,
                 myplugin::ENTRYPOINT)
```

That exports the standard manifest symbol with the current graft ABI version.

### Loading a native plugin

```cpp
#include <icy/graft/graft.h>

using GetApiFunc = const MyPluginApi* (*)();

icy::graft::Library lib;
lib.open("./libmotion_detector.so");

const auto& manifest = lib.manifest();
auto getApi = lib.entrypoint<GetApiFunc>();
const MyPluginApi* api = getApi();
```

`Library::open()` fails fast if the manifest is missing, the ABI is wrong, or the runtime kind is not recognized.

### Runtime kinds

`graft` currently recognizes two runtime strings:

- `native`
- `worker`

Today the loader is for native shared libraries. The worker kind exists so package metadata and future host policy can distinguish in-process extensions from out-of-process ones using the same vocabulary.

## Using Graft With Pacm

`pacm` should describe the install-time extension payload. `graft` should load the actual library.

A native package manifest in `pacm` looks like this:

```json
{
  "id": "motion-detector",
  "type": "Plugin",
  "extension": {
    "loader": "graft",
    "runtime": "native",
    "entrypoint": "lib/libmotion_detector.so",
    "abi-version": 1,
    "capabilities": ["processor.video", "detector"]
  }
}
```

After `pacm` installs the package, the host resolves `extension.entrypoint` relative to the install directory, then hands that file path to `graft::Library`.

## Configuration

There is no runtime configuration surface inside `graft`.

The contract values that matter are:

- `icy::graft::ABI_VERSION`
- `icy::graft::MANIFEST_SYMBOL`
- the manifest `runtime` string
- the manifest `entrypoint` string

Change the ABI version only when the manifest contract or required entrypoint semantics break compatibility.

## See Also

- [Pacm](pacm.md) — package delivery and install layout for native or worker extensions
- [Base](base.md) — `SharedLibrary` and platform/runtime fundamentals
