{#graftmodule}

# graft

Native plugin ABI and shared-library loading helpers.

### Namespaces

| Name | Description |
|------|-------------|
| [`graft`](#graft) | Manifest, runtime-kind, and loader helpers for native plugins. |

### Macros

| Name | Description |
|------|-------------|
| [`ICY_GRAFT_PLUGIN`](#icy_graft_plugin) | Exports the standard graft manifest for a native plugin library. |

---

{#icy_graft_plugin}

#### ICY_GRAFT_PLUGIN

```cpp
ICY_GRAFT_PLUGIN()
```

Exports the standard `icy_graft_manifest` symbol with the current graft ABI version.

#### Parameters
* `pluginId` Stable package/plugin identifier.
* `pluginName` Human-readable plugin name.
* `pluginVersion` Plugin version string.
* `runtimeKind` Runtime string, usually `icy::graft::RUNTIME_NATIVE`.
* `entrypointName` Exported symbol the host should resolve after validation.

---

{#graft}

# graft

Manifest, runtime-kind, and loader helpers for native plugins.

### Classes

| Name | Description |
|------|-------------|
| [`Library`](#library) | Opens a shared library, validates the graft manifest, and resolves symbols. |
| [`Manifest`](#manifest) | Exported metadata contract for a graft plugin library. |

### Enums

| Name | Description |
|------|-------------|
| [`RuntimeKind`](#runtimekind) | Normalized runtime kinds recognized by graft. |

### Constants

| Name | Description |
|------|-------------|
| `ABI_VERSION` | Current graft ABI version. |
| `MANIFEST_SYMBOL` | Required manifest export symbol name. |
| `RUNTIME_NATIVE` | Runtime string for in-process native libraries. |
| `RUNTIME_WORKER` | Runtime string for out-of-process worker packages. |

### Free Functions

| Return | Name | Description |
|--------|------|-------------|
| `RuntimeKind` | `parseRuntimeKind` | Parses a runtime string into a normalized enum value. |
| `const char *` | `runtimeKindName` | Returns the canonical runtime string for a `RuntimeKind`. |
| `void` | `validateManifest` | Validates the manifest fields and throws on contract violations. |

---

{#manifest}

## Manifest

```cpp
#include <icy/graft/graft.h>
```

Exported metadata contract for a graft plugin library.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::uint32_t` | `abiVersion` | ABI version reported by the plugin. |
| `const char *` | `fileName` | Source file that emitted the manifest. |
| `const char *` | `id` | Stable plugin identifier. |
| `const char *` | `name` | Human-readable plugin name. |
| `const char *` | `version` | Plugin version string. |
| `const char *` | `runtime` | Runtime kind string. |
| `const char *` | `entrypoint` | Symbol name the host should resolve after validation. |

---

{#library}

## Library

```cpp
#include <icy/graft/graft.h>
```

Opens a shared library, validates the graft manifest, and resolves symbols.

### Key Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | `open` | Opens the library and validates the manifest immediately. |
| `void` | `close` | Closes the library and clears the cached manifest. |
| `bool` | `isOpen` | Returns whether the library is currently open. |
| `const Manifest &` | `manifest` | Returns the validated manifest. |
| `void *` | `requireSymbolAddress` | Resolves a required symbol or throws. |

`entrypoint<T>()` is the convenience path for loading the manifest-declared entrypoint symbol as a typed function pointer.

---

{#runtimekind}

## RuntimeKind

Normalized runtime kinds recognized by graft.

- `Unknown`
- `Native`
- `Worker`
