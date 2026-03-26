cmake_minimum_required(VERSION 3.21)

# Read version from file
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" icey_VERSION)
string(STRIP "${icey_VERSION}" icey_VERSION)

project(icey
  VERSION ${icey_VERSION}
  LANGUAGES C CXX
  DESCRIPTION "C++ Networking Evolved"
  HOMEPAGE_URL "https://github.com/sourcey/icey"
)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to Release if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

# Unicode on Windows
if(WIN32)
  add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX -DWIN32_LEAN_AND_MEAN)
endif()

# Include the main build orchestrator
include(icey.cmake)

# Package configuration (CPack)
include(cmake/icey_package.cmake)

# CTest support
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
  set(CMAKE_CTEST_COMMAND ctest -V)
  add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
endif()

if(BUILD_BENCHMARKS)
  find_package(Python3 COMPONENTS Interpreter QUIET)
  if(Python3_Interpreter_FOUND AND icey_REPORT_BENCHMARKS)
    set(_icey_bench_targets "")
    foreach(_bench ${icey_REPORT_BENCHMARKS})
      list(APPEND _icey_bench_targets "$<TARGET_FILE:${_bench}>")
    endforeach()

    add_custom_target(benchreport
      COMMAND "${Python3_EXECUTABLE}"
              "${CMAKE_CURRENT_SOURCE_DIR}/bench/run_benchmarks.py"
              --output "${CMAKE_BINARY_DIR}/bench/benchmarks.json"
              --samples 3
              --warmups 1
              ${_icey_bench_targets}
      DEPENDS ${icey_REPORT_BENCHMARKS}
      USES_TERMINAL
      COMMENT "Running icey microbenchmarks and writing bench/benchmarks.json"
    )
  endif()
endif()

# Doxygen documentation target
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif()

# Uninstall target
if(UNIX)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
  add_custom_target(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
endif()

# Build summary
message(STATUS "")
message(STATUS "  icey ${PROJECT_VERSION}")
message(STATUS "")
message(STATUS "  Platform:      ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "  CMake:         ${CMAKE_VERSION}")
message(STATUS "  Generator:     ${CMAKE_GENERATOR}")
message(STATUS "  Build type:    ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ compiler:  ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  C++ standard:  ${CMAKE_CXX_STANDARD}")
message(STATUS "  Shared libs:   ${BUILD_SHARED_LIBS}")
message(STATUS "")
message(STATUS "  Modules:       ${icey_BUILD_MODULES}")
message(STATUS "  Tests:         ${icey_BUILD_TESTS}")
message(STATUS "  Samples:       ${icey_BUILD_SAMPLES}")
message(STATUS "  Fuzzers:       ${icey_BUILD_FUZZERS}")
message(STATUS "  Benchmarks:    ${icey_BUILD_BENCHMARKS}")
message(STATUS "  Perf:          ${icey_BUILD_PERF}")
message(STATUS "")
message(STATUS "  OpenSSL:       ${HAVE_OPENSSL}")
message(STATUS "  FFmpeg:        ${HAVE_FFMPEG}")
message(STATUS "  OpenCV:        ${HAVE_OPENCV}")

message(STATUS "")
message(STATUS "  Install path:  ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")

# Warn on in-source builds
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(WARNING "In-source builds are not recommended. Use a separate build directory.")
endif()
