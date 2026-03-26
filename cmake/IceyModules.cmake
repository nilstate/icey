# Icey module system
#
# Target-based dependency propagation. Each module declares its own dependencies
# via DEPENDS (other icy modules) and PACKAGES (external find_package targets).
# Dependencies flow through targets automatically via PUBLIC linkage.
#
# Usage in module CMakeLists.txt:
#
#   icy_add_module(http
#     DEPENDS base net crypto
#     PACKAGES OpenSSL::SSL
#     PRETTY_NAME HTTP
#   )
#

include(GNUInstallDirs)

# Tracked lists for build summary and config header
set(Icey_BUILD_MODULES "" CACHE INTERNAL "Modules built")
set(Icey_BUILD_TESTS "" CACHE INTERNAL "Tests built")
set(Icey_BUILD_SAMPLES "" CACHE INTERNAL "Samples built")
set(Icey_BUILD_FUZZERS "" CACHE INTERNAL "Fuzz targets built")
set(Icey_BUILD_BENCHMARKS "" CACHE INTERNAL "Benchmark targets built")
set(Icey_BUILD_PERF "" CACHE INTERNAL "Comparative performance targets built")
set(Icey_REPORT_BENCHMARKS "" CACHE INTERNAL "Machine-readable benchmark targets built")

# ----------------------------------------------------------------------------
# Helper: filter platform-specific sources
# ----------------------------------------------------------------------------
function(icy_filter_platform_sources srcs_var hdrs_var)
  set(_srcs ${${srcs_var}})
  set(_hdrs ${${hdrs_var}})

  # Build exclusion regex based on platform
  set(_exclude_patterns "")
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND _exclude_patterns ".*win32.*")
  endif()
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND _exclude_patterns ".*apple.*")
  endif()
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND _exclude_patterns ".*linux.*")
  endif()

  if(_exclude_patterns)
    list(JOIN _exclude_patterns "|" _exclude_regex)
    list(FILTER _srcs EXCLUDE REGEX "${_exclude_regex}")
    list(FILTER _hdrs EXCLUDE REGEX "${_exclude_regex}")
  endif()

  # Include Objective-C on Apple
  if(APPLE)
    file(GLOB_RECURSE _objc_srcs "src/*.mm")
    list(APPEND _srcs ${_objc_srcs})
  endif()

  set(${srcs_var} ${_srcs} PARENT_SCOPE)
  set(${hdrs_var} ${_hdrs} PARENT_SCOPE)
endfunction()

# ----------------------------------------------------------------------------
# Helper: set export symbol name
# ----------------------------------------------------------------------------
function(icy_set_export_symbol target pretty_name)
  if(pretty_name)
    set(_name ${pretty_name})
  else()
    # Capitalise first letter of target name
    string(SUBSTRING ${target} 0 1 _first)
    string(TOUPPER ${_first} _first)
    string(SUBSTRING ${target} 1 -1 _rest)
    set(_name "${_first}${_rest}")
  endif()
  target_compile_definitions(${target} PRIVATE ${_name}_EXPORTS)
endfunction()

# ----------------------------------------------------------------------------
# Helper: set library output name (icy_<name>)
# ----------------------------------------------------------------------------
function(icy_set_output_properties target)
  string(TOLOWER "icy_${target}" _output_name)

  set_target_properties(${target} PROPERTIES
    OUTPUT_NAME ${_output_name}
    DEBUG_POSTFIX "${Icey_DEBUG_POSTFIX}"
    LINKER_LANGUAGE CXX
  )

  if(NOT ANDROID)
    set_target_properties(${target} PROPERTIES
      VERSION "${PROJECT_VERSION}"
      SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    )
  endif()

  if(ENABLE_SOLUTION_FOLDERS)
    set_target_properties(${target} PROPERTIES FOLDER "modules")
  endif()
endfunction()

# ----------------------------------------------------------------------------
# icy_add_module(<name>
#   [DEPENDS <module1> <module2> ...]
#   [PACKAGES <pkg1> <pkg2> ...]
#   [PRETTY_NAME <name>]
#   [SKIP_EXPORT]
# )
# ----------------------------------------------------------------------------
function(icy_add_module name)
  cmake_parse_arguments(MOD "SKIP_EXPORT" "PRETTY_NAME" "DEPENDS;PACKAGES" ${ARGN})

  # Check if module is enabled
  if(DEFINED BUILD_MODULE_${name} AND NOT BUILD_MODULE_${name})
    return()
  endif()

  # Collect sources
  file(GLOB_RECURSE _srcs "src/*.cpp")
  file(GLOB_RECURSE _hdrs "include/*.h")
  icy_filter_platform_sources(_srcs _hdrs)

  source_group("Src" FILES ${_srcs})
  source_group("Include" FILES ${_hdrs})

  # Create library target
  add_library(${name} ${_srcs} ${_hdrs})
  add_library(Icey::${name} ALIAS ${name})

  # Include directories with generator expressions for build vs install
  target_include_directories(${name}
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  # Link module dependencies (propagate transitively)
  if(MOD_DEPENDS)
    target_link_libraries(${name} PUBLIC ${MOD_DEPENDS})
  endif()

  # Link external package dependencies
  if(MOD_PACKAGES)
    target_link_libraries(${name} PUBLIC ${MOD_PACKAGES})
  endif()

  # Warnings as errors (per-target, avoids breaking FetchContent deps)
  if(ENABLE_WARNINGS_ARE_ERRORS)
    if(MSVC)
      target_compile_options(${name} PRIVATE /WX)
    else()
      target_compile_options(${name} PRIVATE -Werror)
    endif()
  endif()

  # Export symbol for shared libraries
  icy_set_export_symbol(${name} "${MOD_PRETTY_NAME}")

  # Output properties
  icy_set_output_properties(${name})

  # Install library
  if(_srcs)
    if(MOD_SKIP_EXPORT)
      # Install without adding to the shared export set (for modules
      # with FetchContent deps that have conflicting export targets).
      install(TARGETS ${name}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT libs
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libs
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT dev
      )
    else()
      install(TARGETS ${name}
        EXPORT IceyTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT libs
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libs
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT dev
      )
    endif()
  endif()

  # Install headers
  install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT dev
  )

  # Track for build summary and config header
  set(HAVE_ICY_${name} ON CACHE INTERNAL "")
  set(Icey_BUILD_MODULES ${Icey_BUILD_MODULES} ${name} CACHE INTERNAL "")

  message(STATUS "  Module: ${name}")

  # Build tests if requested
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests AND (BUILD_TESTS OR BUILD_TESTS_${name}))
    add_subdirectory(tests)
  endif()

  # Build samples if requested
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/samples AND (BUILD_SAMPLES OR BUILD_SAMPLES_${name}))
    add_subdirectory(samples)
  endif()

  # Build fuzz targets if requested
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/fuzz AND BUILD_FUZZERS)
    add_subdirectory(fuzz)
  endif()

  # Build benchmark targets if requested
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bench AND BUILD_BENCHMARKS)
    add_subdirectory(bench)
  endif()

  # Build comparative performance targets if requested
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/perf AND BUILD_PERF)
    add_subdirectory(perf)
  endif()
endfunction()

# ----------------------------------------------------------------------------
# icy_add_test(<name> DEPENDS <module1> <module2> ...
#   [EXCLUDE_CTEST]
# )
# ----------------------------------------------------------------------------
function(icy_add_test name)
  cmake_parse_arguments(TEST "EXCLUDE_CTEST" "" "DEPENDS" ${ARGN})

  # Use per-target source file if it exists, otherwise glob all
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp")
    set(_srcs "${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.h")
      set(_hdrs "${CMAKE_CURRENT_SOURCE_DIR}/${name}.h")
    else()
      set(_hdrs "")
    endif()
  else()
    file(GLOB _srcs "*.cpp")
    file(GLOB _hdrs "*.h*")
  endif()

  source_group("Src" FILES ${_srcs})
  source_group("Include" FILES ${_hdrs})

  add_executable(${name} ${_srcs} ${_hdrs})

  if(TEST_DEPENDS)
    target_link_libraries(${name} PRIVATE ${TEST_DEPENDS})
  endif()

  # Config header access
  target_include_directories(${name} PRIVATE ${CMAKE_BINARY_DIR})
  target_compile_definitions(${name} PRIVATE ICY_DATA_DIR="${Icey_SOURCE_DIR}/data")

  if(NOT TEST_EXCLUDE_CTEST)
    add_test(NAME ${name} COMMAND ${name})
  endif()

  if(ENABLE_SOLUTION_FOLDERS)
    set_target_properties(${name} PROPERTIES FOLDER "tests")
  endif()

  set(Icey_BUILD_TESTS ${Icey_BUILD_TESTS} ${name} CACHE INTERNAL "")

  install(TARGETS ${name}
    RUNTIME DESTINATION "${CMAKE_INSTALL_DATADIR}/icey/tests/${name}"
    COMPONENT tests
  )
endfunction()

# ----------------------------------------------------------------------------
# icy_add_sample(<name> DEPENDS <module1> <module2> ...)
# ----------------------------------------------------------------------------
function(icy_add_sample name)
  cmake_parse_arguments(SAMPLE "" "" "DEPENDS" ${ARGN})

  file(GLOB _srcs "*.cpp")
  file(GLOB _hdrs "*.h*")

  source_group("Src" FILES ${_srcs})
  source_group("Include" FILES ${_hdrs})

  add_executable(${name} ${_srcs} ${_hdrs})

  if(SAMPLE_DEPENDS)
    target_link_libraries(${name} PRIVATE ${SAMPLE_DEPENDS})
  endif()

  target_include_directories(${name} PRIVATE ${CMAKE_BINARY_DIR})
  target_compile_definitions(${name} PRIVATE ICY_DATA_DIR="${Icey_SOURCE_DIR}/data")

  if(ENABLE_SOLUTION_FOLDERS)
    set_target_properties(${name} PROPERTIES FOLDER "samples")
  endif()

  set(Icey_BUILD_SAMPLES ${Icey_BUILD_SAMPLES} ${name} CACHE INTERNAL "")

  install(TARGETS ${name}
    RUNTIME DESTINATION "${CMAKE_INSTALL_DATADIR}/icey/samples/${name}"
    COMPONENT samples
  )
endfunction()

# ----------------------------------------------------------------------------
# icy_add_application(<name> DEPENDS <module1> <module2> ...)
# ----------------------------------------------------------------------------
function(icy_add_application name)
  cmake_parse_arguments(APP "" "" "DEPENDS" ${ARGN})

  file(GLOB_RECURSE _srcs "src/*.c*")
  file(GLOB_RECURSE _hdrs "src/*.h*")

  # Windows resource file
  if(WIN32 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/${name}.rc")
    list(APPEND _srcs "src/${name}.rc")
  endif()

  source_group("Source" FILES ${_srcs})
  source_group("Include" FILES ${_hdrs})

  add_executable(${name} ${_srcs} ${_hdrs})

  if(APP_DEPENDS)
    target_link_libraries(${name} PRIVATE ${APP_DEPENDS})
  endif()

  target_include_directories(${name} PRIVATE ${CMAKE_BINARY_DIR})

  if(ENABLE_SOLUTION_FOLDERS)
    set_target_properties(${name} PROPERTIES FOLDER "applications")
  endif()

  install(TARGETS ${name}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT apps
  )
endfunction()

# ----------------------------------------------------------------------------
# icy_add_fuzzer(<name> DEPENDS <module1> <module2> ...)
# ----------------------------------------------------------------------------
function(icy_add_fuzzer name)
  cmake_parse_arguments(FUZZ "" "" "DEPENDS" ${ARGN})

  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp")
    set(_srcs "${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.h")
      set(_hdrs "${CMAKE_CURRENT_SOURCE_DIR}/${name}.h")
    else()
      set(_hdrs "")
    endif()
  else()
    file(GLOB _srcs "*.cpp")
    file(GLOB _hdrs "*.h*")
  endif()

  source_group("Src" FILES ${_srcs})
  source_group("Include" FILES ${_hdrs})

  add_executable(${name} ${_srcs} ${_hdrs})

  if(FUZZ_DEPENDS)
    target_link_libraries(${name} PRIVATE ${FUZZ_DEPENDS})
  endif()

  target_include_directories(${name} PRIVATE ${CMAKE_BINARY_DIR})
  target_compile_definitions(${name} PRIVATE ICY_DATA_DIR="${Icey_SOURCE_DIR}/data")

  if(FUZZING_ENGINE)
    separate_arguments(_fuzz_engine_flags NATIVE_COMMAND "${FUZZING_ENGINE}")
    target_compile_options(${name} PRIVATE ${_fuzz_engine_flags})
    target_link_options(${name} PRIVATE ${_fuzz_engine_flags})
  else()
    target_compile_definitions(${name} PRIVATE ICY_STANDALONE_FUZZER=1)
  endif()

  if(ENABLE_SOLUTION_FOLDERS)
    set_target_properties(${name} PROPERTIES FOLDER "fuzz")
  endif()

  set(Icey_BUILD_FUZZERS ${Icey_BUILD_FUZZERS} ${name} CACHE INTERNAL "")
endfunction()

# ----------------------------------------------------------------------------
# icy_add_benchmark(<name> DEPENDS <module1> <module2> ...)
# ----------------------------------------------------------------------------
function(icy_add_benchmark name)
  cmake_parse_arguments(BENCH "" "" "DEPENDS" ${ARGN})

  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp")
    set(_srcs "${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.h")
      set(_hdrs "${CMAKE_CURRENT_SOURCE_DIR}/${name}.h")
    else()
      set(_hdrs "")
    endif()
  else()
    file(GLOB _srcs "*.cpp")
    file(GLOB _hdrs "*.h*")
  endif()

  source_group("Src" FILES ${_srcs})
  source_group("Include" FILES ${_hdrs})

  add_executable(${name} ${_srcs} ${_hdrs})

  if(BENCH_DEPENDS)
    target_link_libraries(${name} PRIVATE ${BENCH_DEPENDS})
  endif()

  target_include_directories(${name} PRIVATE
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/bench
  )
  target_compile_definitions(${name} PRIVATE ICY_DATA_DIR="${Icey_SOURCE_DIR}/data")

  if(ENABLE_SOLUTION_FOLDERS)
    set_target_properties(${name} PROPERTIES FOLDER "benchmarks")
  endif()

  set(Icey_BUILD_BENCHMARKS ${Icey_BUILD_BENCHMARKS} ${name} CACHE INTERNAL "")
  set(Icey_REPORT_BENCHMARKS ${Icey_REPORT_BENCHMARKS} ${name} CACHE INTERNAL "")
endfunction()

# ----------------------------------------------------------------------------
# define_icey_dependency(<name>)
#
# For vendor/third-party libraries. Preserved for compatibility with existing
# vendor CMakeLists.txt files (libuv, http_parser, zlib, minizip).
# ----------------------------------------------------------------------------
macro(define_icey_dependency name)
  project(${name})

  if(NOT ${name}_SOURCE_FILES)
    if(NOT ${name}_SOURCE_PATH)
      set(${name}_SOURCE_PATH "src/*.c*")
    endif()
    file(GLOB_RECURSE ${name}_SOURCE_FILES ${${name}_SOURCE_PATH})
  endif()

  if(NOT ${name}_HEADER_FILES)
    if(NOT ${name}_HEADER_PATH)
      set(${name}_HEADER_PATH "src/*.h*")
    endif()
    file(GLOB_RECURSE ${name}_HEADER_FILES ${${name}_HEADER_PATH})
  endif()

  source_group("Source" FILES ${${name}_SOURCE_FILES})
  source_group("Include" FILES ${${name}_HEADER_FILES})

  add_library(${name} ${Icey_LIB_TYPE} ${${name}_SOURCE_FILES})

  if(${name}_DEFINITIONS)
    target_compile_definitions(${name} PRIVATE ${${name}_DEFINITIONS})
  endif()
  if(${name}_FLAGS)
    target_compile_options(${name} PRIVATE ${${name}_FLAGS})
  endif()
  if(${name}_INCLUDE_DIRS)
    foreach(_inc_dir ${${name}_INCLUDE_DIRS})
      if(IS_ABSOLUTE "${_inc_dir}")
        target_include_directories(${name} PUBLIC
          $<BUILD_INTERFACE:${_inc_dir}>)
      else()
        target_include_directories(${name} PUBLIC
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${_inc_dir}>)
      endif()
    endforeach()
    target_include_directories(${name} PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  endif()
  if(${name}_DEPENDENCIES)
    target_link_libraries(${name} PRIVATE ${${name}_DEPENDENCIES} ${CMAKE_DL_LIBS})
  endif()

  if(${name}_OUTPUT_NAME)
    set_target_properties(${name} PROPERTIES OUTPUT_NAME ${${name}_OUTPUT_NAME})
  endif()
  if(Icey_DEBUG_POSTFIX)
    set_target_properties(${name} PROPERTIES DEBUG_POSTFIX ${Icey_DEBUG_POSTFIX})
  endif()
  if(ENABLE_SOLUTION_FOLDERS)
    set_target_properties(${name} PROPERTIES FOLDER "dependencies")
  endif()

  install(TARGETS ${name}
    EXPORT IceyTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libs
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libs
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT dev
  )

  if(${name}_HEADER_FILES)
    install(FILES ${${name}_HEADER_FILES}
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
      COMPONENT dev
    )
  endif()
endmacro()

# ----------------------------------------------------------------------------
# Utility: list subdirectories
# ----------------------------------------------------------------------------
function(subdirlist result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
      list(APPEND dirlist ${child})
    endif()
  endforeach()
  set(${result} ${dirlist} PARENT_SCOPE)
endfunction()
