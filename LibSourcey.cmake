# ============================================================================
# CMake file for LibSourcey
# ============================================================================

# ----------------------------------------------------------------------------
# LibSourcey build paths
# ----------------------------------------------------------------------------
set(LibSourcey_NAME LibSourcey)
set(LibSourcey_DIR ${CMAKE_CURRENT_LIST_DIR})
set(LibSourcey_SOURCE_DIR ${LibSourcey_DIR}/src)
set(LibSourcey_VENDOR_SOURCE_DIR ${LibSourcey_DIR}/vendor)
set(LibSourcey_BUILD_DIR ${CMAKE_BINARY_DIR})
set(LibSourcey_VENDOR_BUILD_DIR ${CMAKE_BINARY_DIR}/vendor)

set(LibSourcey_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
set(LibSourcey_SHARED_INSTALL_DIR ${LibSourcey_INSTALL_DIR}/share/libsourcey)
set(LibSourcey_VENDOR_INSTALL_DIR ${LibSourcey_SHARED_INSTALL_DIR}/vendor)
set(LibSourcey_PKGCONFIG_DIR ${LibSourcey_INSTALL_DIR}/lib/pkgconfig)

# Set CMake defaults
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LibSourcey_DIR}/cmake)
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${LibSourcey_DIR}/cmake)
set(CMAKE_LIBRARY_PATH ${LibSourcey_VENDOR_INSTALL_DIR}/lib ${CMAKE_LIBRARY_PATH})
set(CMAKE_SYSTEM_PREFIX_PATH ${LibSourcey_VENDOR_INSTALL_DIR}/include ${CMAKE_SYSTEM_PREFIX_PATH})

# Include CMake extensions
include(CMakeHelpers REQUIRED)
include(CMakeFindExtensions REQUIRED)
include(LibSourceyUtilities REQUIRED)
include(LibSourceyIncludes REQUIRED)
include(LibSourceyModules REQUIRED)
include(LibSourceyVersion REQUIRED)

# ----------------------------------------------------------------------------
# LibSourcey build components
# ----------------------------------------------------------------------------
set_option(BUILD_SHARED_LIBS          "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" NOT (WIN32 OR ANDROID OR IOS))
set_option(BUILD_MODULES              "Build LibSourcey modules"                                 ON)
set_option(BUILD_APPLICATIONS         "Build LibSourcey applications"                            ON   IF CMAKE_COMPILER_IS_GNUCXX)
set_option(BUILD_DEPENDENCIES         "Build third party dependencies"                           ON)
set_option(BUILD_TESTS                "Build module test applications?"                          ON   IF CMAKE_COMPILER_IS_GNUCXX)
set_option(BUILD_SAMPLES              "Build module sample applications?"                        ON   IF CMAKE_COMPILER_IS_GNUCXX)
set_option(BUILD_WITH_DEBUG_INFO      "Include debug info into debug libs"                       ON)
set_option(BUILD_WITH_STATIC_CRT      "Enables statically linked CRT for statically linked libraries" OFF)
set_option(BUILD_ALPHA                "Build alpha development modules"                          OFF)

# ----------------------------------------------------------------------------
# LibSourcey build options
# ----------------------------------------------------------------------------
set_option(ENABLE_SOLUTION_FOLDERS    "Solution folder in Visual Studio or in other IDEs"        MSVC_IDE IF (CMAKE_VERSION VERSION_GREATER "2.8.0") )
set_option(ENABLE_PROFILING           "Enable profiling in the GCC compiler (Add flags: -g -pg)" OFF  IF CMAKE_COMPILER_IS_GNUCXX )
set_option(ENABLE_OMIT_FRAME_POINTER  "Enable -fomit-frame-pointer for GCC"                      ON   IF CMAKE_COMPILER_IS_GNUCXX )
set_option(ENABLE_POWERPC             "Enable PowerPC for GCC"                                   ON   IF (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SYSTEM_PROCESSOR MATCHES powerpc.*) )
set_option(ENABLE_FAST_MATH           "Enable -ffast-math (not recommended for GCC 4.6.x)"       OFF  IF (CMAKE_COMPILER_IS_GNUCXX AND (X86 OR X86_64)) )
set_option(ENABLE_SSE                 "Enable SSE instructions"                                  ON   IF (MSVC OR CMAKE_COMPILER_IS_GNUCXX AND (X86 OR X86_64)) )
set_option(ENABLE_SSE2                "Enable SSE2 instructions"                                 ON   IF (MSVC OR CMAKE_COMPILER_IS_GNUCXX AND (X86 OR X86_64)) )
set_option(ENABLE_SSE3                "Enable SSE3 instructions"                                 OFF  IF (CMAKE_COMPILER_IS_GNUCXX AND (X86 OR X86_64)) )
set_option(ENABLE_SSSE3               "Enable SSSE3 instructions"                                OFF  IF (CMAKE_COMPILER_IS_GNUCXX AND (X86 OR X86_64)) )
set_option(ENABLE_SSE41               "Enable SSE4.1 instructions"                               OFF  IF (CMAKE_COMPILER_IS_GNUCXX AND (X86 OR X86_64)) )
set_option(ENABLE_SSE42               "Enable SSE4.2 instructions"                               OFF  IF (CMAKE_COMPILER_IS_GNUCXX AND (X86 OR X86_64)) )
set_option(ENABLE_NOISY_WARNINGS      "Show all warnings even if they are too noisy"             OFF )
set_option(ENABLE_WARNINGS_ARE_ERRORS "Treat warnings as errors"                                 OFF )
set_option(ENABLE_LOGGING             "Enable internal debug logging"                            ON   IF (CMAKE_BUILD_TYPE MATCHES DEBUG) )
set_option(EXCEPTION_RECOVERY         "Attempt to recover from internal exceptions"              ON   IF (CMAKE_BUILD_TYPE MATCHES DEBUG) )
set_option(MSG_VERBOSE                "Print verbose debug status messages"                      OFF )


# ----------------------------------------------------------------------------
# LibSourcey internal options
# ----------------------------------------------------------------------------
set(LibSourcey_INCLUDE_DIRS           "") # CACHE INTERNAL "Global include dirs" FORCE)
set(LibSourcey_LIBRARY_DIRS           "") # CACHE INTERNAL "Global include library dirs" FORCE)
set(LibSourcey_INCLUDE_LIBRARIES      "") # CACHE INTERNAL "Global include libraries" FORCE)

set(LibSourcey_BUILD_DEPENDENCIES     "" CACHE INTERNAL "Dependencies to build" FORCE)
set(LibSourcey_BUILD_MODULES          "" CACHE INTERNAL "Modules to build" FORCE)
set(LibSourcey_BUILD_SAMPLES          "" CACHE INTERNAL "Samples to build" FORCE)
set(LibSourcey_BUILD_TESTS            "" CACHE INTERNAL "Tests to build" FORCE)
set(LibSourcey_BUILD_APPLICATIONS     "" CACHE INTERNAL "Applications to build" FORCE)

# ----------------------------------------------------------------------------
# Solution folders:
# ----------------------------------------------------------------------------
if(ENABLE_SOLUTION_FOLDERS)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")
endif()

# ----------------------------------------------------------------------------
# C++ standard:
# ----------------------------------------------------------------------------
# C++ standard 14 minimum is required
# Wait until CMake >= 3.1 is widely adopted to use CMAKE_CXX_STANDARD
# set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_STANDARD_REQUIRED on)

# ----------------------------------------------------------------------------
# Use statically or dynamically linked CRT?
# Default: dynamic
# ----------------------------------------------------------------------------
if(MSVC)
  include(LibSourceyCRTLinkage REQUIRED)
endif()

# ----------------------------------------------------------------------------
# Apple and iOS build options
# ----------------------------------------------------------------------------
if(APPLE)
  # Remove the “MACOSX_RPATH is not specified” warning
  # set(CMAKE_MACOSX_RPATH 1)

  # Silence CMake warnings by adopting modern behavior for MACOSX_RPATH on newer
  # versions of CMake
  if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
  endif()
  if(IOS)
    find_package(Threads REQUIRED)
  endif()
  if(!IOS)
    set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
  endif()
endif()

# ----------------------------------------------------------------------------
# LibSourcey compiler and linker options
# ----------------------------------------------------------------------------
include(LibSourceyCompilerOptions REQUIRED)

# ----------------------------------------------------------------------------
# Set the LibSourcey_LIB_TYPE variable for add_library
# ----------------------------------------------------------------------------
set(LibSourcey_LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
  set(LibSourcey_LIB_TYPE SHARED)
endif()

# ============================================================================
# Include Dependencies, Modules and Applications
#
# LibSourcey automatically includes all directories inside the ./src folder.
# Libraries in the LibSourcey source tree are broken up into three types:
#
# Dependency:
#     A third party library required by a LibSourcey Modue or Application.
#     Dependencies may be external or internal. External dependencies reside
#     outside on the source tree, while internal dependencies generally reside
#     in the ./deps folder. All dependencies must be built, installed, found
#     and included by the build system before Modules and Application can be
#     built.
#
# Module:
#     A static or dynamic library which extends the LibSourcey core, and is
#     included by Applications based on LibSourcey architecture. Modules must
#     be built and installed before Applications.
#
# Application:
#     A standalone application. These can be built once all Dependencies and
#     Modules have been built and installed.
#
# ============================================================================

# ----------------------------------------------------------------------------
# Include standard and defult libraries
# ----------------------------------------------------------------------------
if(MSVC)
  # CMAKE_CXX_STANDARD_LIBRARIES must be a string for MSVC
  set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} advapi32.lib iphlpapi.lib psapi.lib shell32.lib ws2_32.lib dsound.lib winmm.lib strmiids.lib")
elseif(MSYS)
  set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -lws2_32 -liphlpapi") #${LibSourcey_INCLUDE_LIBRARIES}
elseif(APPLE)
  set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -ldl") # -lm -lz -llibc -lglibc #${LibSourcey_INCLUDE_LIBRARIES}
elseif(UNIX)
  set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -lm -ldl -lrt") # -lz -lrt -lpulse-simple -lpulse -ljack -llibc -lglibc #${LibSourcey_INCLUDE_LIBRARIES}
endif()
#if (CMAKE_COMPILER_IS_GNUCXX)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
#endif()

if(MSVC)
  # Temporary workaround for "error LNK2026: module unsafe for SAFESEH image"
  # when compiling with certain externally compiled libraries with VS2012,
  # such as http://ffmpeg.zeranoe.com/builds/
  # This disables safe exception handling by default.
  if(${_MACHINE_ARCH_FLAG} MATCHES X86)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")
  endif()

  # No pesky windows min/max macros
  add_definitions(-DNOMINMAX)
endif()


# ----------------------------------------------------------------------------
# Include third party dependencies
# ----------------------------------------------------------------------------

# Set some required vendor variables for all modules
link_directories(${LibSourcey_VENDOR_INSTALL_DIR}/lib)

list(APPEND LibSourcey_INCLUDE_DIRS ${LibSourcey_VENDOR_INSTALL_DIR}/include)
if (EXISTS ${LibSourcey_VENDOR_INSTALL_DIR}/lib)
  list(APPEND LibSourcey_LIBRARY_DIRS ${LibSourcey_VENDOR_INSTALL_DIR}/lib)
endif()

# Prefind external dependencies so we can set defaults
# find_package(OpenSSL)
find_package(Threads)
find_package(OpenCV)
find_package(FFmpeg)

# set_option(WITH_LIBUV           "Include LibUV support"                ON)
# set_option(WITH_RTAUDIO         "Include RtAudio support"              ON)
# set_option(WITH_HTTPPARSER      "Include HttpParser support"           ON)
# set_option(WITH_JSONCPP         "Include JsonCpp support"              ON)
set_option(WITH_ZLIB            "Include zlib support"                 ON)
set_option(WITH_OPENSSL         "Include OpenSSL support"              ON) #IF (OPENSSL_FOUND)
set_option(WITH_FFMPEG          "Include FFmpeg support"               OFF) #ON IF (FFmpeg_FOUND)
set_option(WITH_OPENCV          "Include OpenCV support"               OFF) #ON IF (OpenCV_FOUND)
set_option(WITH_WEBRTC          "Include WebRTC support"               OFF)
set_option(WITH_POCO            "Include Poco support"                 OFF)
set_option(WITH_WXWIDGETS       "Include wxWidgets support"            OFF)

# Include dependencies
if(APPLE)
  status("Including APPLE's foundation and AVFoundation frameworks")

  # Don't use RPATH's. The resulting binary could fail a security audit.
  # if (NOT CMAKE_VERSION VERSION_LESS 2.8.12)
  #   set(CMAKE_MACOSX_RPATH OFF)
  # endif()
  set(CMAKE_MACOSX_RPATH ON)

  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(FOUNDATION Foundation)
    find_library(AVFOUNDATION AVFoundation)

    list(APPEND LibSourcey_BUILD_DEPENDENCIES ${FOUNDATION} ${AVFOUNDATION})
  endif()

  #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Foundation -framework AVFoundation")
  #set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
  #set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")
endif()

# Build dependencies
add_vendor_dependency(LIBUV libuv)
if(WITH_ZLIB)
  add_vendor_dependency(ZLIB zlib)
  add_vendor_dependency(MINIZIP minizip)
endif()
# if(WITH_JSONCPP)
#   add_vendor_dependency(JSONCPP jsoncpp)
# endif()
# add_vendor_dependency(JSON json)
add_vendor_dependency(HTTPPARSER http_parser)

# External dependencies
if(WITH_WEBRTC)
  find_dependency(WebRTC REQUIRED)

  # We will be building with BoringSSL instead of OpenSSL
  message(STATUS "Using BoringSSL")
  set(WITH_OPENSSL OFF)
  set(HAVE_OPENSSL ON)
  set(OPENSSL_IS_BORINGSSL ON)
  unset(OPENSSL_INCLUDE_DIR CACHE)
  unset(LIB_EAY_DEBUG CACHE)
  unset(LIB_EAY_RELEASE CACHE)
  unset(SSL_EAY_DEBUG CACHE)
  unset(SSL_EAY_RELEASE CACHE)
  find_path(OPENSSL_INCLUDE_DIR
    NAMES openssl/ssl.h
    PATHS
      ${WEBRTC_ROOT_DIR}/third_party/boringssl/src/include
      ${WEBRTC_ROOT_DIR}/include/third_party/boringssl/src/include
    NO_DEFAULT_PATH)
  list(APPEND LibSourcey_VENDOR_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR})
endif()
if(WITH_OPENSSL)
  find_dependency(OpenSSL REQUIRED)
endif()
if(WITH_FFMPEG)
  find_dependency(FFmpeg REQUIRED)
endif()
if(WITH_OPENCV)
  find_dependency(OpenCV REQUIRED)
endif()
if(WITH_POCO)
  find_dependency(Poco REQUIRED Util XML CppParser Foundation)
endif()
if(WITH_WXWIDGETS)
  # TODO: specify required library options
  find_dependency(wxWidgets REQUIRED core base adv)
endif()

# ----------------------------------------------------------------------------
# Include the LibSourcey source tree
# ----------------------------------------------------------------------------

# Include vendor source dirs
list(APPEND LibSourcey_VENDOR_INCLUDE_DIRS
  ${LibSourcey_VENDOR_SOURCE_DIR}/zlib
  ${LibSourcey_VENDOR_BUILD_DIR}/zlib
  ${LibSourcey_VENDOR_SOURCE_DIR}/minizip
  ${LibSourcey_VENDOR_SOURCE_DIR}/libuv/include
  ${LibSourcey_VENDOR_SOURCE_DIR}/http_parser
  ${LibSourcey_VENDOR_SOURCE_DIR}/json/src)
  # ${LibSourcey_VENDOR_SOURCE_DIR}/rtaudio
  # ${LibSourcey_VENDOR_SOURCE_DIR}/rtaudio/include
  # ${LibSourcey_VENDOR_SOURCE_DIR}/jsoncpp

# Include inttypes.h for windows
# if (MSVC)
#   include_directories(${LibSourcey_VENDOR_SOURCE_DIR}/msvc)
# endif()

# Enable testing if requested
if (BUILD_TESTS)
  enable_testing()
endif()

# Include source tree modules
subdirlist(subdirs ${LibSourcey_SOURCE_DIR})
foreach(name ${subdirs})
  # This variable is so modules can set HAVE_LibSourcey_XXX for our
  # libsourcey.h inside the child scope. See include_sourcey_modules()
  # TODO: Need to refactor since modules may be nested.
  set(HAVE_LibSourcey_${name} 0)
  set(dir "${LibSourcey_SOURCE_DIR}/${name}")
  if (EXISTS "${dir}/CMakeLists.txt")
    add_subdirectory(${dir} ${LibSourcey_BUILD_DIR}/${name})
  endif()
endforeach()

# Condense related sublists into main variables
list(APPEND LibSourcey_INCLUDE_DIRS ${LibSourcey_MODULE_INCLUDE_DIRS} ${LibSourcey_VENDOR_INCLUDE_DIRS})
list(APPEND LibSourcey_LIBRARY_DIRS ${LibSourcey_MODULE_LIBRARY_DIRS} ${LibSourcey_BUILD_DIR})
list(APPEND LibSourcey_INCLUDE_LIBRARIES ${LibSourcey_MODULE_INCLUDE_LIBRARIES})

# Remove any duplicates from our lists
if(LibSourcey_LINK_LIBRARIES)
  list(REMOVE_DUPLICATES LibSourcey_LINK_LIBRARIES)
endif()
if(LibSourcey_BUILD_DEPENDENCIES)
  list(REMOVE_DUPLICATES LibSourcey_BUILD_DEPENDENCIES)
endif()

# ----------------------------------------------------------------------------
# Install targets
# ----------------------------------------------------------------------------

# Install specific vendor headers
install(FILES ${LibSourcey_VENDOR_SOURCE_DIR}/json/src/json.hpp
  DESTINATION ${LibSourcey_VENDOR_INSTALL_DIR}/include
  COMPONENT dev)

# Copy CMake files to shared install directory
install(DIRECTORY ${LibSourcey_DIR}/cmake
  DESTINATION ${LibSourcey_SHARED_INSTALL_DIR}
  COMPONENT dev
  FILES_MATCHING PATTERN "CMake*"  PATTERN "Find*")

# ----------------------------------------------------------------------------
#  Install PkgConfig file
# ----------------------------------------------------------------------------

set(PKG_CONFIG_LIBS)
foreach(module ${LibSourcey_BUILD_MODULES})
  set(PKG_CONFIG_LIBS "${PKG_CONFIG_LIBS} -lscy_${module}")
endforeach()
foreach(dep ${LibSourcey_BUILD_DEPENDENCIES})
  set(PKG_CONFIG_LIBS "${PKG_CONFIG_LIBS} -l${dep}")
endforeach()

status("Creating 'libsourcey.pc'")
set(LibSourcey_PC ${LibSourcey_BUILD_DIR}/libsourcey.pc)
configure_file(
  ${LibSourcey_DIR}/cmake/libsourcey.pc.cmake.in
	${LibSourcey_PC} @ONLY)
install(FILES ${LibSourcey_PC} DESTINATION ${LibSourcey_PKGCONFIG_DIR} COMPONENT dev)

# ----------------------------------------------------------------------------
# Build our libsourcey.h file
#
# A directory will be created for each platform so the "libsourcey.h" file is
# not overwritten if CMake generates code in the same path.
# ----------------------------------------------------------------------------

# Variables for libsourcey.h
set(SCY_ENABLE_LOGGING ${ENABLE_LOGGING})
set(SCY_EXCEPTION_RECOVERY ${EXCEPTION_RECOVERY})
set(SCY_SHARED_LIBRARY ${BUILD_SHARED_LIBS})

status("Creating 'libsourcey.h'")
set(LibSourcey_CONFIG_FILE ${LibSourcey_BUILD_DIR}/libsourcey.h)
configure_file(
  ${LibSourcey_DIR}/cmake/libsourcey.h.cmake.in
  ${LibSourcey_CONFIG_FILE})
install(FILES ${LibSourcey_CONFIG_FILE} DESTINATION ${LibSourcey_INSTALL_DIR}/include COMPONENT dev)
