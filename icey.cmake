# ============================================================================
# icey build orchestrator
#
# Sets options, finds dependencies, discovers modules, generates config files.
# Module targets are created by icy_add_module() in icey_modules.cmake.
# ============================================================================

# Paths
set(icey_DIR ${CMAKE_CURRENT_LIST_DIR})
set(icey_SOURCE_DIR ${icey_DIR}/src)
set(icey_VENDOR_SOURCE_DIR ${icey_DIR}/vendor)
set(icey_VENDOR_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/icey/vendor)

# CMake module path
list(APPEND CMAKE_MODULE_PATH ${icey_DIR}/cmake)

# Include build system components
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(${icey_DIR}/cmake/icey_modules.cmake)
include(${icey_DIR}/cmake/icey_compiler_options.cmake)

function(icy_add_compat_target compat_target actual_target)
  if(TARGET "${actual_target}" AND NOT TARGET "${compat_target}")
    add_library("${compat_target}" INTERFACE IMPORTED)
    set_property(TARGET "${compat_target}" PROPERTY INTERFACE_LINK_LIBRARIES "${actual_target}")
  endif()
endfunction()

# Debug postfix
if(WIN32)
  set(icey_DEBUG_POSTFIX "d")
else()
  set(icey_DEBUG_POSTFIX "")
endif()

# ----------------------------------------------------------------------------
# Build options
# ----------------------------------------------------------------------------
option(BUILD_SHARED_LIBS          "Build shared libraries"                    OFF)
option(BUILD_MODULES              "Build icey modules"                  ON)
option(BUILD_APPLICATIONS         "Build icey applications"             ON)
option(BUILD_TESTS                "Build module tests"                        OFF)
option(BUILD_SAMPLES              "Build module samples"                      OFF)
option(BUILD_FUZZERS              "Build module fuzz targets"                 OFF)
option(BUILD_BENCHMARKS          "Build module benchmark targets"            OFF)
option(BUILD_PERF                "Build module comparative performance targets" OFF)
option(BUILD_ALPHA                "Build alpha development modules"           OFF)
option(ENABLE_SOLUTION_FOLDERS    "IDE solution folders"                      ON)
option(ENABLE_LOGGING             "Enable internal debug logging"             ON)
option(EXCEPTION_RECOVERY         "Attempt to recover from internal exceptions" OFF)
option(ENABLE_WARNINGS_ARE_ERRORS "Treat compiler warnings as errors"           OFF)
option(USE_SYSTEM_DEPS            "Use find_package instead of FetchContent for core deps" OFF)
set(FUZZING_ENGINE "" CACHE STRING "Optional fuzzer engine/link flags for BUILD_FUZZERS targets")

# Platform-specific shared lib default
if(NOT DEFINED BUILD_SHARED_LIBS)
  if(WIN32 OR ANDROID OR IOS)
    set(BUILD_SHARED_LIBS OFF)
  else()
    set(BUILD_SHARED_LIBS ON)
  endif()
endif()

# Solution folders
if(ENABLE_SOLUTION_FOLDERS)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")
endif()

# For vendor define_icey_dependency compatibility
set(icey_LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
  set(icey_LIB_TYPE SHARED)
endif()

# ----------------------------------------------------------------------------
# External dependency options
# ----------------------------------------------------------------------------
option(WITH_OPENSSL         "Include OpenSSL support"          ON)
option(WITH_FFMPEG          "Include FFmpeg support"           OFF)
option(WITH_OPENCV          "Include OpenCV support"           OFF)
option(WITH_LIBDATACHANNEL  "Include libdatachannel for WebRTC" OFF)


# ----------------------------------------------------------------------------
# Platform libraries
# ----------------------------------------------------------------------------
if(MSVC)
  set(CMAKE_CXX_STANDARD_LIBRARIES
    "${CMAKE_CXX_STANDARD_LIBRARIES} advapi32.lib iphlpapi.lib psapi.lib shell32.lib ws2_32.lib dsound.lib winmm.lib strmiids.lib")
  add_definitions(-DNOMINMAX)
elseif(MSYS)
  set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -lws2_32 -liphlpapi")
elseif(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    find_library(FOUNDATION Foundation)
    find_library(AVFOUNDATION AVFoundation)
  endif()
elseif(UNIX)
  set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -lm -ldl -lrt")
endif()

# ----------------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------------
if(NOT USE_SYSTEM_DEPS)
  # FetchContent: download and build deps from source (default)
  include(FetchContent)

  # libuv - event loop
  FetchContent_Declare(libuv
    GIT_REPOSITORY https://github.com/libuv/libuv.git
    GIT_TAG        v1.50.0
    GIT_SHALLOW    TRUE)
  set(LIBUV_BUILD_SHARED OFF CACHE BOOL "" FORCE)
  set(LIBUV_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(LIBUV_BUILD_BENCH OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(libuv)

  # llhttp - HTTP parser (replaces http_parser)
  FetchContent_Declare(llhttp
    URL      https://github.com/nodejs/llhttp/archive/refs/tags/release/v9.2.1.tar.gz
    URL_HASH SHA256=3c163891446e529604b590f9ad097b2e98b5ef7e4d3ddcf1cf98b62ca668f23e)
  set(BUILD_SHARED_LIBS_SAVED ${BUILD_SHARED_LIBS})
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(llhttp)
  set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_SAVED} CACHE BOOL "" FORCE)

  # zlib
  FetchContent_Declare(zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG        v1.3.1
    GIT_SHALLOW    TRUE)
  set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(zlib)
  set(SKIP_INSTALL_ALL OFF CACHE BOOL "" FORCE)

  # Fix zlib's include_directories (uses raw paths, breaks install/export)
  # Replace with generator expressions so it can be exported cleanly
  FetchContent_GetProperties(zlib SOURCE_DIR zlib_SOURCE_DIR BINARY_DIR zlib_BINARY_DIR)
  set_target_properties(zlibstatic PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "")
  target_include_directories(zlibstatic PUBLIC
    $<BUILD_INTERFACE:${zlib_SOURCE_DIR}>
    $<BUILD_INTERFACE:${zlib_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

  install(TARGETS zlibstatic
    EXPORT iceyTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT dev)

  # minizip - built from zlib's contrib/minizip
  set(MINIZIP_SOURCE_DIR ${zlib_SOURCE_DIR}/contrib/minizip)
  set(minizip_SOURCES
    ${MINIZIP_SOURCE_DIR}/ioapi.c
    ${MINIZIP_SOURCE_DIR}/mztools.c
    ${MINIZIP_SOURCE_DIR}/unzip.c
    ${MINIZIP_SOURCE_DIR}/zip.c)
  set(minizip_HEADERS
    ${MINIZIP_SOURCE_DIR}/crypt.h
    ${MINIZIP_SOURCE_DIR}/ioapi.h
    ${MINIZIP_SOURCE_DIR}/mztools.h
    ${MINIZIP_SOURCE_DIR}/unzip.h
    ${MINIZIP_SOURCE_DIR}/zip.h)
  if(WIN32)
    list(APPEND minizip_SOURCES ${MINIZIP_SOURCE_DIR}/iowin32.c)
  endif()
  add_library(minizip STATIC ${minizip_SOURCES} ${minizip_HEADERS})
  target_link_libraries(minizip PUBLIC zlibstatic)
  target_include_directories(minizip PUBLIC
    $<BUILD_INTERFACE:${MINIZIP_SOURCE_DIR}>
    $<BUILD_INTERFACE:${zlib_SOURCE_DIR}>
    $<BUILD_INTERFACE:${zlib_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  if(ENABLE_SOLUTION_FOLDERS)
    set_target_properties(minizip PROPERTIES FOLDER "dependencies")
  endif()
  install(TARGETS minizip
    EXPORT iceyTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT dev)
  install(FILES ${minizip_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT dev)

else()
  # System deps: use find_package (for vcpkg or system-installed packages)
  find_package(PkgConfig QUIET)
  find_package(libuv CONFIG QUIET)
  if(NOT TARGET libuv::uv_a AND NOT TARGET libuv::libuv AND PkgConfig_FOUND)
    pkg_check_modules(LIBUV QUIET IMPORTED_TARGET GLOBAL libuv)
  endif()
  find_package(llhttp CONFIG QUIET)
  find_package(ZLIB REQUIRED)
  if(NOT TARGET llhttp_static
     AND NOT TARGET llhttp::llhttp_static
     AND NOT TARGET llhttp::llhttp
     AND NOT TARGET llhttp
     AND PkgConfig_FOUND)
    pkg_check_modules(LLHTTP QUIET IMPORTED_TARGET GLOBAL llhttp)
  endif()

  # Package managers do not agree on canonical target names.
  icy_add_compat_target(uv_a libuv::uv_a)
  icy_add_compat_target(uv_a libuv::libuv)
  icy_add_compat_target(uv_a PkgConfig::LIBUV)
  icy_add_compat_target(llhttp_static llhttp::llhttp_static)
  icy_add_compat_target(llhttp_static llhttp::llhttp)
  icy_add_compat_target(llhttp_static llhttp)
  icy_add_compat_target(llhttp_static PkgConfig::LLHTTP)
  if(NOT TARGET uv_a)
    message(FATAL_ERROR "libuv not found; install libuv with a CMake package config or pkg-config metadata")
  endif()
  if(NOT TARGET llhttp_static)
    message(FATAL_ERROR "llhttp not found; install llhttp with a CMake package config or pkg-config metadata")
  endif()

  # Alias system zlib to match the target name used by modules
  if(NOT TARGET zlibstatic)
    add_library(zlibstatic ALIAS ZLIB::ZLIB)
  endif()

  # minizip: try packaged targets first, fall back to building from source
  find_package(unofficial-minizip CONFIG QUIET)
  find_package(minizip CONFIG QUIET)
  if(TARGET unofficial-minizip::minizip)
    icy_add_compat_target(minizip unofficial-minizip::minizip)
  elseif(TARGET minizip::minizip)
    icy_add_compat_target(minizip minizip::minizip)
  elseif(PkgConfig_FOUND)
    pkg_check_modules(MINIZIP QUIET IMPORTED_TARGET GLOBAL minizip)
    icy_add_compat_target(minizip PkgConfig::MINIZIP)
  endif()

  if(NOT TARGET minizip)
    # Build minizip from zlib's contrib (requires zlib headers on system)
    find_path(ZLIB_SOURCE_DIR NAMES contrib/minizip/unzip.h
      HINTS ${ZLIB_INCLUDE_DIRS} ${ZLIB_ROOT}
      PATH_SUFFIXES .. src)
    if(ZLIB_SOURCE_DIR)
      set(MINIZIP_SOURCE_DIR ${ZLIB_SOURCE_DIR}/contrib/minizip)
      set(minizip_SOURCES
        ${MINIZIP_SOURCE_DIR}/ioapi.c
        ${MINIZIP_SOURCE_DIR}/mztools.c
        ${MINIZIP_SOURCE_DIR}/unzip.c
        ${MINIZIP_SOURCE_DIR}/zip.c)
      if(WIN32)
        list(APPEND minizip_SOURCES ${MINIZIP_SOURCE_DIR}/iowin32.c)
      endif()
      add_library(minizip STATIC ${minizip_SOURCES})
      target_link_libraries(minizip PUBLIC ZLIB::ZLIB)
      target_include_directories(minizip PUBLIC
        $<BUILD_INTERFACE:${MINIZIP_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
      install(TARGETS minizip
        EXPORT iceyTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT dev)
      install(FILES
        ${MINIZIP_SOURCE_DIR}/crypt.h
        ${MINIZIP_SOURCE_DIR}/ioapi.h
        ${MINIZIP_SOURCE_DIR}/mztools.h
        ${MINIZIP_SOURCE_DIR}/unzip.h
        ${MINIZIP_SOURCE_DIR}/zip.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT dev)
    else()
      message(WARNING "minizip sources not found; archo module will not be available")
    endif()
  endif()
endif()

# ----------------------------------------------------------------------------
# Find external dependencies
# ----------------------------------------------------------------------------
find_package(Threads REQUIRED)

if(WITH_OPENSSL)
  find_package(OpenSSL QUIET)
  if(OPENSSL_FOUND)
    set(HAVE_OPENSSL ON)
    message(STATUS "  Found OpenSSL: ${OPENSSL_VERSION}")
  else()
    message(STATUS "  OpenSSL not found, disabling SSL-dependent modules")
  endif()
endif()

if(WITH_FFMPEG)
  include(FindFFmpeg)
  if(FFMPEG_FOUND)
    set(HAVE_FFMPEG ON)
    # Set per-component flags for config header (#cmakedefine HAVE_FFMPEG_AVDEVICE etc.)
    foreach(_comp AVCODEC AVFORMAT AVUTIL AVFILTER AVDEVICE SWSCALE SWRESAMPLE POSTPROC)
      if(FFmpeg_${_comp}_FOUND)
        set(HAVE_FFMPEG_${_comp} ON)
      endif()
    endforeach()
  endif()
endif()

if(WITH_OPENCV)
  find_package(OpenCV QUIET)
  if(OpenCV_FOUND)
    set(HAVE_OPENCV ON)
  endif()
endif()

# ----------------------------------------------------------------------------
# libdatachannel (WebRTC transport: ICE, DTLS, SRTP, data channels)
# ----------------------------------------------------------------------------
if(WITH_LIBDATACHANNEL AND HAVE_OPENSSL AND HAVE_FFMPEG)
  if(USE_SYSTEM_DEPS)
    find_package(LibDataChannel CONFIG QUIET)
    if(TARGET LibDataChannel::LibDataChannelStatic)
      set(ICEY_LIBDATACHANNEL_TARGET LibDataChannel::LibDataChannelStatic)
    elseif(TARGET LibDataChannel::LibDataChannel)
      set(ICEY_LIBDATACHANNEL_TARGET LibDataChannel::LibDataChannel)
    endif()
    if(DEFINED ICEY_LIBDATACHANNEL_TARGET)
      set(HAVE_LIBDATACHANNEL ON)
      message(STATUS "  Found libdatachannel (system)")
    else()
      message(STATUS "  libdatachannel not found, disabling WebRTC")
    endif()
  else()
    include(FetchContent)
    FetchContent_Declare(libdatachannel
      GIT_REPOSITORY https://github.com/paullouisageneau/libdatachannel.git
      GIT_TAG        v0.24.1
      GIT_SHALLOW    TRUE)
    set(NO_MEDIA OFF CACHE BOOL "" FORCE)
    set(NO_WEBSOCKET ON CACHE BOOL "" FORCE)
    set(NO_EXAMPLES ON CACHE BOOL "" FORCE)
    set(NO_TESTS ON CACHE BOOL "" FORCE)
    # Disable libdatachannel's -Werror to avoid breaking our build
    set(WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(libdatachannel)
    if(TARGET LibDataChannel::LibDataChannelStatic)
      set(ICEY_LIBDATACHANNEL_TARGET LibDataChannel::LibDataChannelStatic)
    elseif(TARGET LibDataChannel::LibDataChannel)
      set(ICEY_LIBDATACHANNEL_TARGET LibDataChannel::LibDataChannel)
    endif()
    if(DEFINED ICEY_LIBDATACHANNEL_TARGET)
      set(HAVE_LIBDATACHANNEL ON)
      message(STATUS "  Found libdatachannel (FetchContent)")
    else()
      message(STATUS "  libdatachannel target not found after FetchContent, disabling WebRTC")
    endif()
  endif()
else()
  if(WITH_LIBDATACHANNEL)
    message(STATUS "  libdatachannel requires OpenSSL and FFmpeg (WITH_OPENSSL=ON WITH_FFMPEG=ON)")
  endif()
endif()

# ----------------------------------------------------------------------------
# Auto-discover and build modules
# ----------------------------------------------------------------------------
if(BUILD_MODULES)
  message(STATUS "")
  message(STATUS "  Modules:")

  subdirlist(_module_dirs ${icey_SOURCE_DIR})
  foreach(_name ${_module_dirs})
    set(_dir "${icey_SOURCE_DIR}/${_name}")
    if(EXISTS "${_dir}/CMakeLists.txt")
      add_subdirectory(${_dir} ${CMAKE_BINARY_DIR}/${_name})
    endif()
  endforeach()
endif()

# ----------------------------------------------------------------------------
# Generate config header (icey.h)
# ----------------------------------------------------------------------------
set(ICY_ENABLE_LOGGING ${ENABLE_LOGGING})
set(ICY_EXCEPTION_RECOVERY ${EXCEPTION_RECOVERY})
set(ICY_SHARED_LIBRARY ${BUILD_SHARED_LIBS})

message(STATUS "")
message(STATUS "  Generating icey.h")
set(ICY_SOURCE_DIR_VALUE "${icey_SOURCE_DIR}")
set(ICY_BUILD_DIR_VALUE "${CMAKE_BINARY_DIR}")
set(ICY_INSTALL_DIR_VALUE "${CMAKE_INSTALL_PREFIX}")
configure_file(
  ${icey_DIR}/cmake/icey.h.cmake.in
  ${CMAKE_BINARY_DIR}/icey.h)

set(ICY_SOURCE_DIR_VALUE "")
set(ICY_BUILD_DIR_VALUE "")
set(ICY_INSTALL_DIR_VALUE "")
configure_file(
  ${icey_DIR}/cmake/icey.h.cmake.in
  ${CMAKE_BINARY_DIR}/iceyInstall.h)

install(FILES ${CMAKE_BINARY_DIR}/iceyInstall.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  RENAME icey.h
  COMPONENT dev)

# ----------------------------------------------------------------------------
# Generate pkg-config file
# ----------------------------------------------------------------------------
set(PKG_CONFIG_LIBS)
foreach(_mod ${icey_BUILD_MODULES})
  string(APPEND PKG_CONFIG_LIBS " -licy_${_mod}")
endforeach()

message(STATUS "  Generating icey.pc")
configure_file(
  ${icey_DIR}/cmake/icey.pc.cmake.in
  ${CMAKE_BINARY_DIR}/icey.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/icey.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig COMPONENT dev)

# ----------------------------------------------------------------------------
# Install export set and config package
# ----------------------------------------------------------------------------
export(EXPORT iceyTargets
  FILE ${CMAKE_BINARY_DIR}/iceyTargets.cmake
  NAMESPACE icey::
)

if(NOT USE_SYSTEM_DEPS)
  set(_icey_vendored_targets)
  foreach(_target uv_a llhttp_static)
    if(TARGET ${_target})
      list(APPEND _icey_vendored_targets ${_target})
    endif()
  endforeach()
  if(_icey_vendored_targets)
    export(TARGETS ${_icey_vendored_targets}
      FILE ${CMAKE_BINARY_DIR}/iceyVendoredTargets.cmake
    )
  endif()
endif()

install(EXPORT iceyTargets
  NAMESPACE icey::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/icey
  COMPONENT dev)

configure_package_config_file(
  ${icey_DIR}/cmake/icey_build_config.cmake.in
  ${CMAKE_BINARY_DIR}/iceyConfig.cmake
  INSTALL_DESTINATION ${CMAKE_BINARY_DIR}
  INSTALL_PREFIX ${CMAKE_BINARY_DIR})

configure_package_config_file(
  ${icey_DIR}/cmake/icey_config.cmake.in
  ${CMAKE_BINARY_DIR}/iceyInstallConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/icey)

write_basic_package_version_file(
  ${CMAKE_BINARY_DIR}/iceyConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(FILES
  ${CMAKE_BINARY_DIR}/iceyConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/icey
  COMPONENT dev)
install(FILES ${CMAKE_BINARY_DIR}/iceyInstallConfig.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/icey
  RENAME iceyConfig.cmake
  COMPONENT dev)
