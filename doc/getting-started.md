# Getting Started

Welcome to LibSourcey! If something's missing or outdated, please submit a [pull request](../README.md#contributing) with your updates.

Before going any further please follow the [installation guide](installation.md) to compile and install LibSourcey on your system.

Once LibSourcey is installed you have a number of choices for how to include the libraries in your own project:

### 1) Using `pkg-config` (Linux only)

After running `make install` the `pkg-config` script is located in `/usr/local/lib/pkgconfig/libsourcey.pc` by default.

```bash
g++ -o test test.cpp $(pkg-config --libs --cflags libsourcey)
```

### 2) Using CMake

If you're already using CMake then including LibSourcey with CMake is recommended. Once you have built and installed LibSourcey you can include all the libraries and headers by adding the following lines to your `CMakeLists.txt`:

```cmake
# The LibSourcey root directory must be set
set(LibSourcey_ROOT_DIR "" CACHE STRING "Where is the LibSourcey root directory located?")

# Tell CMake where to locate the LibSourcey .cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LibSourcey_ROOT_DIR}/cmake)
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${LibSourcey_ROOT_DIR}/cmake)

# Include LibSourcey with specified modules (example: base net crypto http)
find_package(LibSourcey COMPONENTS base net crypto http REQUIRED)
```

### 3) Extending the build system

The third option is to include the LibSourcey CMake build system in your own project. This method builds LibSourcey modules alongside your own project as dependencies. [See here](installation.md#cmake-build-options) for a complete list of build options.

This method is only recommended for advanced users who want their project tightly coupled with LibSourcey.

Below is an example `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.14)
project(exampleapp)

# Set the source directory
set(sourcedir src)

# Include LibSourcey
include("../libsourcey/LibSourcey.cmake")

include_directories(${sourcedir} ${LibSourcey_INCLUDE_DIRS})
link_libraries(${LibSourcey_LIBRARIES})

file(GLOB_RECURSE sources ${sourcedir}/*.cpp)
file(GLOB_RECURSE headers ${sourcedir}/*.h)

add_executable(exampleapp ${sources} ${headers})
install(TARGETS exampleapp)
```

## Next Steps

Once LibSourcey is included in your project you're ready to start using the API. Check out the [code examples](examples.md) and [API reference](api-base.md) to start cutting code.
