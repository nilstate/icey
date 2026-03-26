# JSON Module

JSON serialization, file I/O, and configuration helpers built on `nlohmann/json`.

- Namespace: `icy::json`
- CMake target: `icey::json`
- Primary headers: `include/icy/json/json.h`, `iserializable.h`, `configuration.h`
- Directory layout: `include/` for the public API, `src/` for serialization/configuration helpers, `tests/` for round-trip and file coverage
- Main downstream users inside icey: configuration-heavy modules such as `pacm`, `symple`, and `sched`

Read next:
- [JSON module guide](../../doc/modules/json.md)
- [Pacm module guide](../../doc/modules/pacm.md)
