icy_add_benchmark(signalbench DEPENDS base)
icy_add_benchmark(packetstreambench DEPENDS base)

# signalbench's hot loop (1M virtual `Signal::emit` calls into a single
# `ClassDelegate`) hit SIGILL on the GitHub-hosted ubuntu-24.04 + gcc-14
# runner under the default CMake Release flags (`-O3 -DNDEBUG`). The same
# binary runs cleanly on macOS clang and in a local Linux gcc-14
# container. Drop signalbench to `-O2` and disable tree vectorization on
# GCC, which avoids the offending optimization without affecting the
# meaningfulness of the timing report (the inner loop's hot path is
# member-function-pointer indirection, not arithmetic).
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(signalbench PRIVATE -O2 -fno-tree-vectorize)
endif()
