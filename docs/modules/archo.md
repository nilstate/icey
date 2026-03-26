# Archo

> ZIP archive reading and extraction

**[API Reference →](../api/archo.md)** · **[Source →](../../src/archo/)**

## Overview

The `archo` module provides ZIP archive handling built on zlib's minizip API. We use it wherever we need to unpack downloaded archives to disk; it is a direct dependency of `pacm`, which calls it to extract package files after each download completes.

The module exposes a single class, `icy::archo::ZipFile`, which wraps a `unzFile` handle and offers two levels of access: a high-level `extract()` call that unpacks everything to a directory, and a lower-level cursor API for iterating entries selectively. Every extraction path validates entry names against path-traversal attacks before any file is written.

**Dependencies:** `base`, `zlib` (minizip)

## Usage

### Extract all entries

The common case: open an archive and write all of its contents to a destination directory. The directory is created automatically if it does not exist.

```cpp
#include <icy/archo/zipfile.h>

icy::archo::ZipFile zip("/path/to/package.zip");
zip.extract("/opt/myapp/plugins");
// All entries are now on disk under /opt/myapp/plugins/.
```

The two-step form is identical; construct first, then open:

```cpp
icy::archo::ZipFile zip;
zip.open("/path/to/package.zip");
zip.extract("/opt/myapp/plugins");
```

### Inspect the entry list before extracting

After `open()` (or construction with a path), the `info` vector is populated with a `FileInfo` record for every entry in the archive. We can inspect sizes or filter the list before touching disk.

```cpp
icy::archo::ZipFile zip;
zip.open("/path/to/package.zip");

for (const auto& entry : zip.info) {
    std::cout << entry.path
              << "  compressed="   << entry.compressedSize
              << "  uncompressed=" << entry.uncompressedSize << "\n";
}

zip.extract("/opt/myapp/plugins");
```

### Selective extraction with the cursor API

Use `goToFirstFile()` and `goToNextFile()` to walk the archive and call `extractCurrentFile()` only for the entries we want.

```cpp
icy::archo::ZipFile zip("/path/to/archive.zip");

if (zip.goToFirstFile()) {
    do {
        std::string name = zip.currentFileName();
        if (name.ends_with(".so") || name.ends_with(".dll")) {
            zip.extractCurrentFile("/opt/myapp/plugins");
        }
    } while (zip.goToNextFile());
}
```

`extractCurrentFile()` handles `openCurrentFile()` / `closeCurrentFile()` internally; we only need to call those directly if we are implementing custom read logic at a lower level.

### Error handling

`open()` and `extract()` throw `std::runtime_error` on failure. `extractCurrentFile()` accepts an optional `whiny` parameter; when `false` it returns a boolean instead of throwing, which is useful when partial failures are acceptable.

```cpp
try {
    icy::archo::ZipFile zip("/path/to/archive.zip");
    zip.extract("/destination");
} catch (const std::runtime_error& e) {
    std::cerr << "extraction failed: " << e.what() << "\n";
}

// Or, tolerate per-entry failures:
if (!zip.extractCurrentFile("/destination", /*whiny=*/false)) {
    // log and continue
}
```

### Checking whether an archive is open

```cpp
icy::archo::ZipFile zip;
// zip.opened() == false

zip.open("/path/to/archive.zip");
// zip.opened() == true

zip.close();
// zip.opened() == false
```

The destructor closes the handle automatically, so explicit `close()` calls are only needed when reusing the object for a second archive.

### Security

`extractCurrentFile()` validates every entry name before writing. It rejects:

- path-traversal sequences (`..`)
- directory separators (`/` and `\`) in unexpected positions
- null bytes

Any entry that fails validation throws `std::runtime_error`. The high-level `extract()` routes every entry through `extractCurrentFile()`, so the validation is always applied. We never write a file to a location outside the destination directory.

## Configuration

The archo module has no configuration options. It is built automatically as a dependency of the pacm module.

## See Also

- [Pacm](pacm.md) — uses `archo` to extract downloaded package archives
- [Base](base.md) — foundation types and filesystem utilities
