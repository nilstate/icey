# Archo Module

ZIP archive reading and extraction with path traversal protection.

- Namespace: `icy::archo`
- CMake target: `icey::archo`
- Primary header: `include/icy/archo/zipfile.h`
- Directory layout: `include/` for the public API, `src/` for the minizip-backed implementation, `tests/` for extraction and path-safety coverage
- Main use inside icey: archive extraction for `pacm`

Read next:
- [Archo module guide](../../doc/modules/archo.md)
- [Pacm module guide](../../doc/modules/pacm.md)
