# ============================================================================
# Icey build orchestrator
#
# Sets options, finds dependencies, discovers modules, generates config files.
# Module targets are created by icy_add_module() in IceyModules.cmake.
# ============================================================================

# Paths
set(Icey_DIR ${CMAKE_CURRENT_LIST_DIR})
set(Icey_SOURCE_DIR ${Icey_DIR}/src)
set(Icey_VENDOR_SOURCE_DIR ${Icey_DIR}/vendor)
set(Icey_VENDOR_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/icey/vendor)

# CMake module path
list(APPEND CMAKE_MODULE_PATH ${Icey_DIR}/cmake)

# Include build system components
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(IceyModules)
include(IceyCompilerOptions)

# Debug postfix
if(WIN32)
  set(Icey_DEBUG_POSTFIX "d")
else()
  set(Icey_DEBUG_POSTFIX "")
endif()

# ----------------------------------------------------------------------------
# Build options
# ----------------------------------------------------------------------------
option(BUILD_SHARED_LIBS          "Build shared libraries"                    OFF)
option(BUILD_MODULES              "Build Icey modules"                  ON)
option(BUILD_APPLICATIONS         "Build Icey applications"             ON)
option(BUILD_TESTS                "Build module tests"                        OFF)
option(BUILD_SAMPLES              "Build module samples"                      OFF)
option(BUILD_ALPHA                "Build alpha development modules"           OFF)
option(ENABLE_SOLUTION_FOLDERS    "IDE solution folders"                      ON)
option(ENABLE_LOGGING             "Enable internal debug logging"             ON)
option(EXCEPTION_RECOVERY         "Attempt to recover from internal exceptions" OFF)
option(ENABLE_WARNINGS_ARE_ERRORS "Treat compiler warnings as errors"           OFF)
option(USE_SYSTEM_DEPS            "Use find_package instead of FetchContent for core deps" OFF)

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
set(Icey_LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
  set(Icey_LIB_TYPE SHARED)
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
    EXPORT IceyTargets
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
    EXPORT IceyTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT dev)
  install(FILES ${minizip_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT dev)

else()
  # System deps: use find_package (for vcpkg or system-installed packages)
  find_package(libuv CONFIG REQUIRED)
  find_package(llhttp CONFIG REQUIRED)
  find_package(ZLIB REQUIRED)

  # Alias system zlib to match the target name used by modules
  if(NOT TARGET zlibstatic)
    add_library(zlibstatic ALIAS ZLIB::ZLIB)
  endif()

  # minizip: try vcpkg's unofficial-minizip first, fall back to building from source
  find_package(unofficial-minizip CONFIG QUIET)
  if(TARGET unofficial-minizip::minizip)
    if(NOT TARGET minizip)
      add_library(minizip ALIAS unofficial-minizip::minizip)
    endif()
  else()
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
  set(HAVE_LIBDATACHANNEL ON)
  message(STATUS "  Found libdatachannel (FetchContent)")

  # The webrtc module uses SKIP_EXPORT in icy_add_module() because
  # libdatachannel's transitive deps (juice, usrsctp, srtp2) have their
  # own export sets that conflict with IceyTargets.
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

  subdirlist(_module_dirs ${Icey_SOURCE_DIR})
  foreach(_name ${_module_dirs})
    set(_dir "${Icey_SOURCE_DIR}/${_name}")
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
configure_file(
  ${Icey_DIR}/cmake/icey.h.cmake.in
  ${CMAKE_BINARY_DIR}/icey.h)
install(FILES ${CMAKE_BINARY_DIR}/icey.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT dev)

# ----------------------------------------------------------------------------
# Generate pkg-config file
# ----------------------------------------------------------------------------
set(PKG_CONFIG_LIBS)
foreach(_mod ${Icey_BUILD_MODULES})
  string(APPEND PKG_CONFIG_LIBS " -licy_${_mod}")
endforeach()

message(STATUS "  Generating icey.pc")
configure_file(
  ${Icey_DIR}/cmake/icey.pc.cmake.in
  ${CMAKE_BINARY_DIR}/icey.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/icey.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig COMPONENT dev)

# ----------------------------------------------------------------------------
# Install export set and config package
# ----------------------------------------------------------------------------
install(EXPORT IceyTargets
  NAMESPACE Icey::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Icey
  COMPONENT dev)

configure_package_config_file(
  ${Icey_DIR}/cmake/IceyConfig.cmake.in
  ${CMAKE_BINARY_DIR}/IceyConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Icey)

write_basic_package_version_file(
  ${CMAKE_BINARY_DIR}/IceyConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(FILES
  ${CMAKE_BINARY_DIR}/IceyConfig.cmake
  ${CMAKE_BINARY_DIR}/IceyConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Icey
  COMPONENT dev)
