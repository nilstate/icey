# Contributing

Pull requests are always welcome. If you fix or improve anything, feel free to float a PR back upstream.

## Getting started

1. [Fork Icey on GitHub](https://github.com/sourcey/icey)
2. Create a feature branch (`git checkout -b my-feature`)
3. Build and run the tests to make sure everything passes before you start:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build build --parallel $(nproc)
ctest --test-dir build --output-on-failure
```

## Code style

The project uses `clang-format` with a custom config (`.clang-format` in the repo root). The highlights:

- **C++20**, LLVM base style
- 4-space indent, no tabs
- Allman braces for definitions, K&R for control flow
- Pointer/reference left-aligned (`int* p`, not `int *p`)
- No column limit; manual formatting is preserved
- Constructor initialiser lists: comma-first, one per line

Format your changes before committing:

```bash
clang-format -i src/path/to/file.cpp
```

## Commit messages

Use [conventional commits](https://www.conventionalcommits.org/): `feat:`, `fix:`, `refactor:`, `test:`, `docs:`, `chore:`, etc. Keep the subject line short, lowercase, imperative mood.

```
fix: resolve null pointer in WebRTC peer connection
refactor: replace raw pointers with unique_ptr in http client
```

## Tests

If your change touches runtime behaviour, add or update a test. Tests live in `test/` and are built when `-DBUILD_TESTS=ON` is set. Run them with:

```bash
ctest --test-dir build --output-on-failure
```

## Regenerating API docs

The API reference in `doc/` is generated from source using [Doxygen](https://www.doxygen.nl/) and [Moxygen](https://github.com/icey/moxygen). Prerequisites: `doxygen` and `npx` (Node.js).

```bash
doxygen Doxyfile
npx moxygen --groups --output=./doc/api-%s.md ./build/doxygen/xml
```

This parses C++ source into XML, then converts it into the `doc/api-*.md` markdown files.

## Licence

By contributing you agree that your work will be licensed under the [LGPL-2.1+](../LICENSE.md) licence.
