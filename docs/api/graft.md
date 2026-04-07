{#graftmodule}

# graft

Native plugin ABI contract and runtime loading helpers.

### Namespaces

| Name | Description |
|------|-------------|
| [`graft`](#graft) | Shared-library plugin contracts and runtime loading helpers. |

{#graft}

# graft

Shared-library plugin contracts and runtime loading helpers.

### Classes

| Name | Description |
|------|-------------|
| [`Library`](#library) |  |
| [`Manifest`](#manifest-4) |  |

### Enumerations

| Name | Description |
|------|-------------|
| [`RuntimeKind`](#runtimekind)  |  |

---

{#runtimekind}

#### RuntimeKind

```cpp
enum RuntimeKind
```

| Value | Description |
|-------|-------------|
| `Unknown` |  |
| `Native` |  |
| `Worker` |  |

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `Graft_APIRuntimeKind` | [`parseRuntimeKind`](#parseruntimekind)  |  |
| `Graft_API const char *` | [`runtimeKindName`](#runtimekindname)  |  |
| `Graft_API void` | [`validateManifest`](#validatemanifest)  |  |

---

{#parseruntimekind}

#### parseRuntimeKind

```cpp
Graft_APIRuntimeKind parseRuntimeKind(std::string_view runtime) noexcept
```

---

{#runtimekindname}

#### runtimeKindName

```cpp
Graft_API const char * runtimeKindName(RuntimeKind runtime) noexcept
```

---

{#validatemanifest}

#### validateManifest

```cpp
Graft_API void validateManifest(const Manifest & manifest, std::string_view path)
```

### Variables

| Return | Name | Description |
|--------|------|-------------|
| `constexpr std::uint32_t` | [`ABI_VERSION`](#abi_version)  |  |
| `constexpr const char *` | [`MANIFEST_SYMBOL`](#manifest_symbol)  |  |
| `constexpr const char *` | [`RUNTIME_NATIVE`](#runtime_native)  |  |
| `constexpr const char *` | [`RUNTIME_WORKER`](#runtime_worker)  |  |

---

{#abi_version}

#### ABI_VERSION

```cpp
constexpr std::uint32_t ABI_VERSION = 1
```

---

{#manifest_symbol}

#### MANIFEST_SYMBOL

```cpp
constexpr const char * MANIFEST_SYMBOL = "icy_graft_manifest"
```

---

{#runtime_native}

#### RUNTIME_NATIVE

```cpp
constexpr const char * RUNTIME_NATIVE = "native"
```

---

{#runtime_worker}

#### RUNTIME_WORKER

```cpp
constexpr const char * RUNTIME_WORKER = "worker"
```

{#library}

## Library

```cpp
#include <icy/graft/graft.h>
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Library`](#library-1)  | Defaulted constructor. |
|  | [`Library`](#library-2)  | Deleted constructor. |
|  | [`Library`](#library-3)  | Deleted constructor. |
| `void` | [`open`](#open-6)  |  |
| `void` | [`close`](#close-24)  |  |
| `bool` | [`isOpen`](#isopen) `const` |  |
| `const std::string &` | [`path`](#path-3) `const` |  |
| `const Manifest &` | [`manifest`](#manifest-3) `const` |  |
| `void *` | [`requireSymbolAddress`](#requiresymboladdress) `const` |  |
| `T` | [`requireSymbol`](#requiresymbol) `const` `inline` |  |
| `T` | [`entrypoint`](#entrypoint-1) `const` `inline` |  |

---

{#library-1}

#### Library

```cpp
Library() = default
```

Defaulted constructor.

---

{#library-2}

#### Library

```cpp
Library(const Library &) = delete
```

Deleted constructor.

---

{#library-3}

#### Library

```cpp
Library(Library &&) = delete
```

Deleted constructor.

---

{#open-6}

#### open

```cpp
void open(const std::string & path)
```

---

{#close-24}

#### close

```cpp
void close() noexcept
```

---

{#isopen}

#### isOpen

`const`

```cpp
bool isOpen() const noexcept
```

---

{#path-3}

#### path

`const`

```cpp
const std::string & path() const noexcept
```

---

{#manifest-3}

#### manifest

`const`

```cpp
const Manifest & manifest() const
```

---

{#requiresymboladdress}

#### requireSymbolAddress

`const`

```cpp
void * requireSymbolAddress(const char * name) const
```

---

{#requiresymbol}

#### requireSymbol

`const` `inline`

```cpp
template<typename T> inline T requireSymbol(const char * name) const
```

---

{#entrypoint-1}

#### entrypoint

`const` `inline`

```cpp
template<typename T> inline T entrypoint() const
```

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `SharedLibrary` | [`_library`](#_library)  |  |
| `std::string` | [`_path`](#_path-4)  |  |
| `const Manifest *` | [`_manifest`](#_manifest)  |  |
| `bool` | [`_open`](#_open)  |  |

---

{#_library}

#### _library

```cpp
SharedLibrary _library
```

---

{#_path-4}

#### _path

```cpp
std::string _path
```

---

{#_manifest}

#### _manifest

```cpp
const Manifest * _manifest = nullptr
```

---

{#_open}

#### _open

```cpp
bool _open = false
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`loadManifest`](#loadmanifest)  |  |

---

{#loadmanifest}

#### loadManifest

```cpp
void loadManifest()
```

{#manifest-4}

## Manifest

```cpp
#include <icy/graft/graft.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::uint32_t` | [`abiVersion`](#abiversion-1)  |  |
| `const char *` | [`fileName`](#filename-3)  |  |
| `const char *` | [`id`](#id-4)  |  |
| `const char *` | [`name`](#name-8)  |  |
| `const char *` | [`version`](#version-6)  |  |
| `const char *` | [`runtime`](#runtime-1)  |  |
| `const char *` | [`entrypoint`](#entrypoint-2)  |  |

---

{#abiversion-1}

#### abiVersion

```cpp
std::uint32_t abiVersion
```

---

{#filename-3}

#### fileName

```cpp
const char * fileName
```

---

{#id-4}

#### id

```cpp
const char * id
```

---

{#name-8}

#### name

```cpp
const char * name
```

---

{#version-6}

#### version

```cpp
const char * version
```

---

{#runtime-1}

#### runtime

```cpp
const char * runtime
```

---

{#entrypoint-2}

#### entrypoint

```cpp
const char * entrypoint
```

