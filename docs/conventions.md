# Repository Conventions

This page is for contributors changing icey itself.

The architecture and module guides explain how the runtime works. The [contributing guide](contributing.md) explains workflow and release mechanics. This page covers the repository-specific rules that keep new code aligned with the existing structure.

## Module Shape

Each module lives under `src/<module>/`.

The normal layout is:

```text
src/<module>/
├── CMakeLists.txt
├── README.md
├── include/icy/...     # public headers
├── src/                # implementation
├── tests/              # behavioural and contract coverage
├── samples/            # runnable examples
├── bench/              # focused microbenchmarks
├── perf/               # comparative or reportable performance harnesses
├── fuzz/               # fuzz targets for parser or protocol surfaces
└── apps/               # product-style binaries layered on the module
```

Only the first four are expected everywhere. The rest are added when the module actually needs them.

Use the module README as the short local map: namespace, CMake target, primary headers, directory layout, and the next docs to read. Keep that README brief and contributor-oriented.

## Public API Placement

- Public headers live under `src/<module>/include/icy/...`.
- Implementation files live under `src/<module>/src/`.
- Public CMake targets use the `icey::<module>` namespace.
- Public package, docs, and target naming stays lowercase `icey`.

Do not add new public headers outside `include/icy`. If a type is part of the supported API surface, it should be discoverable there.

## CMake Conventions

New modules should follow the existing target pattern and be declared with `icy_add_module(...)`.

The expectations are:

- declare module-to-module dependencies explicitly with `DEPENDS`
- declare third-party packages explicitly with `PACKAGES`
- gate optional modules on real prerequisites such as `HAVE_OPENSSL`, `HAVE_FFMPEG`, or `HAVE_LIBDATACHANNEL`
- expose dependencies as `PUBLIC` only when the public headers require them
- keep implementation-only links and include directories `PRIVATE`

The repo is intentionally target-based. Avoid ad-hoc global include paths, raw compiler flag mutation, or implicit transitive dependency assumptions when a target declaration can state the contract directly.

## Runtime Contracts Are Design Constraints

Most behavioural bugs in icey come from violating the runtime model, not from syntax or formatting drift.

Before changing `base`, `net`, `http`, `av`, or `webrtc`, keep these rules in view:

- most runtime objects are loop-affine
- receive buffers are borrowed
- `send()` is the borrowed fast path and `sendOwned()` is the retained path
- `PacketStream` stays zero-copy until an explicit retention boundary
- `close()` is asynchronous

Use [runtime-contracts.md](concepts/runtime-contracts.md) as the source of truth. Do not widen contracts "just in case" by making a signal cross-thread, adding retention, or inserting queues unless the code path actually needs that broader contract.

## Tests, Samples, And Performance

- Behavioural changes should come with test coverage in the owning module's `tests/` directory or the top-level `tests/` tree.
- User-facing workflows should usually have a runnable sample or app when the feature is not obvious from unit tests alone.
- Benchmark and perf harnesses should be updated only when the changed code affects measured hot paths or published performance claims.

Tests are not an afterthought here. If a change touches lifetime, shutdown, ownership, protocol parsing, queueing, or async state flow, assume it needs a test unless there is a clear reason it does not.

## Documentation Expectations

When the public surface changes, update the docs that own that surface.

That usually means some combination of:

- the module guide in `docs/modules/`
- a concept page in `docs/concepts/`
- a runnable recipe in `docs/recipes/`
- the module README in `src/<module>/README.md`
- root-level docs such as `README.md`, `CHANGELOG.md`, or release notes when the change is externally visible

Keep generated API reference concerns separate from prose docs. The prose explains the model and workflows; the generated API tab explains the exact types and signatures.

## Naming And Rename Hygiene

The repo has already been renamed to `icey`. Do not introduce new references to the older `LibSourcey`, `Sourcey`, or `scy` project naming for library code, namespaces, include paths, or contributor docs unless you are referring to an external tool or historical changelog entry on purpose.

## Related Pages

- [Contributing](contributing.md)
- [Modules](modules.md)
- [Architecture](concepts/architecture.md)
- [Runtime Contracts](concepts/runtime-contracts.md)
- [Releasing](releasing.md)
