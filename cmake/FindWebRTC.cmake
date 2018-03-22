########################################################################
# CMake module for finding WEBRTC
#
# The best way to build WebRTC is with the forked `webrtcbuilds` project here:
# https://github.com/sourcey/webrtcbuilds
#
# The following variables will be defined:
#
#  WEBRTC_FOUND
#  WEBRTC_INCLUDE_DIR
#  WEBRTC_LIBRARIES
#  WEBRTC_DEPENDENCIES
#

# unset(WEBRTC_INCLUDE_DIR)
# unset(WEBRTC_INCLUDE_DIR CACHE)
# unset(WEBRTC_LIBRARY)
# unset(WEBRTC_LIBRARY CACHE)
# unset(WEBRTC_LIBRARIES)
# unset(WEBRTC_LIBRARIES CACHE)
# unset(WEBRTC_LIBRARIES_DEBUG)
# unset(WEBRTC_LIBRARIES_DEBUG CACHE)
# unset(WEBRTC_LIBRARIES_RELEASE)
# unset(WEBRTC_LIBRARIES_RELEASE CACHE)

# Set required variables
set(WEBRTC_ROOT_DIR "" CACHE STRING "Where is the WebRTC root directory located?")
# set(WEBRTC_BUILD_DIR_SUFFIX_DEBUG "out/Debug" CACHE STRING "What is the WebRTC debug build directory suffix?")
# set(WEBRTC_BUILD_DIR_SUFFIX_RELEASE "out/Release" CACHE STRING "What is the WebRTC release build directory suffix?")

# ----------------------------------------------------------------------
# Find WEBRTC include path
# ----------------------------------------------------------------------
find_path(WEBRTC_INCLUDE_DIR
  NAMES
    # webrtc/config.h
    call/rtp_config.h
  PATHS
    ${WEBRTC_ROOT_DIR}
    ${WEBRTC_ROOT_DIR}/include
    $ENV{HOME}/tmp/webrtcbuilds/out/src
    $ENV{HOME}/sourcey/webrtcbuilds/out/src
)

# ----------------------------------------------------------------------
# Find WEBRTC libraries
# ----------------------------------------------------------------------
if(WEBRTC_INCLUDE_DIR)
  find_existing_directory(debug_dir
      ${WEBRTC_ROOT_DIR}/lib/x64/Debug
      ${WEBRTC_ROOT_DIR}/out/x64/Debug
      ${WEBRTC_ROOT_DIR}/out/Debug_x64
      ${WEBRTC_ROOT_DIR}/out/Debug-x64
      ${WEBRTC_ROOT_DIR}/out/Debug)

  find_existing_directory(release_dir
      ${WEBRTC_ROOT_DIR}/lib/x64/Release
      ${WEBRTC_ROOT_DIR}/out/x64/Release
      ${WEBRTC_ROOT_DIR}/out/Release_x64
      ${WEBRTC_ROOT_DIR}/out/Release-x64
      ${WEBRTC_ROOT_DIR}/out/Release)

  # Attempt to find the monolithic library built with `webrtcbuilds`
  find_library_extended(WEBRTC
    NAMES webrtc webrtc_full libwebrtc_full
    PATHS_DEBUG ${debug_dir}
    PATHS_RELEASE ${release_dir}
  )

  # # Otherwise generate a library from available .o objects
  # if(NOT WEBRTC_LIBRARY)
  #   if(EXISTS ${_WEBRTC_BUILD_DIR_DEBUG})
  #     message("Generating WebRTC Debug library")
  #     set(lib_cmd "${LibSourcey_DIR}/cmake/webrtc_lib_generator.sh" -o ${_WEBRTC_BUILD_DIR_DEBUG} -x ${_WEBRTC_GENERATOR_EXCLUDES})
  #     execute_process(COMMAND ${lib_cmd})
  #   endif()
  #
  #   if(EXISTS ${_WEBRTC_BUILD_DIR_RELEASE})
  #     message("Generating WebRTC Release library")
  #     set(lib_cmd "${LibSourcey_DIR}/cmake/webrtc_lib_generator.sh" -o ${_WEBRTC_BUILD_DIR_RELEASE} -x ${_WEBRTC_GENERATOR_EXCLUDES})
  #     execute_process(COMMAND ${lib_cmd})
  #   endif()
  #
  #   # Attempt to find our library again...
  #   find_library_extended(WEBRTC
  #     NAMES webrtc_scy
  #     PATHS_DEBUG ${_WEBRTC_BUILD_DIR_DEBUG}
  #     PATHS_RELEASE ${_WEBRTC_BUILD_DIR_RELEASE}
  #   )
  # endif()

  # Otherwise fallback to library objects (slower and unreliable)
  if(NOT WEBRTC_LIBRARIES)
    #unset(WEBRTC_LIBRARY CACHE)
    #unset(WEBRTC_LIBRARIES CACHE)
    #set(WEBRTC_LIBRARIES "")

    if(MSVC)
      set(lib_suffix "lib")
    else()
      set(lib_suffix "a")
    endif()

    # obj/third_party/boringssl/libboringssl.a
    # obj/third_party/protobuf/libprotobuf_full.a
    # obj/webrtc/system_wrappers/libfield_trial_default.a
    # obj/webrtc/system_wrappers/libmetrics_default.a
    # obj/webrtc/libwebrtc.a

    set(_WEBRTC_DEPENDENCY_INCLUDES "webrtc\\.a|boringssl|protobuf_full|field_trial_default|metrics_default") #|common|video|media

    # Debug
    if (EXISTS ${debug_dir})
      file(GLOB_RECURSE debug_libs "${debug_dir}/*.${lib_suffix}")
      foreach(lib ${debug_libs})
        # if(${lib} NOT MATCHES ${_WEBRTC_DEPENDENCY_EXCLUDES})
        if(${lib} MATCHES ${_WEBRTC_DEPENDENCY_INCLUDES})
          list(APPEND WEBRTC_LIBRARIES_DEBUG ${lib})
        endif()
        # endif()
      endforeach()
      foreach(lib ${WEBRTC_LIBRARIES_DEBUG})
        if(WIN32 AND (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE))
          list(APPEND WEBRTC_LIBRARIES "debug" ${lib})
        else()
          list(APPEND WEBRTC_LIBRARIES ${lib})
        endif()
      endforeach()
    endif()

    # Release
    if (EXISTS ${release_dir})
      file(GLOB_RECURSE release_libs "${release_dir}/*.${lib_suffix}")
      foreach(lib ${release_libs})
        # if(${lib} NOT MATCHES ${_WEBRTC_DEPENDENCY_EXCLUDES})
        if(${lib} MATCHES ${_WEBRTC_DEPENDENCY_INCLUDES})
          list(APPEND WEBRTC_LIBRARIES_RELEASE ${lib})
        endif()
      endforeach()
      foreach(lib ${WEBRTC_LIBRARIES_RELEASE})
        if(WIN32 AND (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE))
          list(APPEND WEBRTC_LIBRARIES "optimized" ${lib})
        else()
          list(APPEND WEBRTC_LIBRARIES ${lib})
        endif()
      endforeach()
    endif()
  endif()

  # Add required system libraries
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND MSVC)
    add_definitions(-DWEBRTC_WIN)
    set(WEBRTC_DEPENDENCIES Secur32.lib Winmm.lib msdmo.lib dmoguids.lib wmcodecdspuuid.lib) # strmbase.lib
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DWEBRTC_POSIX)

    # For ABI compatability between precompiled WebRTC libraries using clang and new GCC versions
    add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
    set(WEBRTC_DEPENDENCIES -lrt -lX11 -lGLU) # -lGL

    # Enable libstdc++ debugging if you build WebRTC with `enable_iterator_debugging=true`
    # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG=1")
  endif()

  # Add vendor include directories
  if(WEBRTC_INCLUDE_DIR AND NOT WEBRTC_INCLUDE_DIRS)
    list(APPEND WEBRTC_INCLUDE_DIRS ${WEBRTC_INCLUDE_DIR} ${WEBRTC_INCLUDE_DIR}/third_party/boringssl/src/include)
  endif()

  # Workaround for fixing error WEBRTC_LIBRARY-NOTFOUND
  set(WEBRTC_LIBRARY_RELEASE ${WEBRTC_LIBRARIES_RELEASE})
  set(WEBRTC_LIBRARY_DEBUG ${WEBRTC_LIBRARIES_DEBUG})
  include(${CMAKE_ROOT}/Modules/SelectLibraryConfigurations.cmake)
  select_library_configurations(WEBRTC)

  # message("WEBRTC_LIBRARIES_DEBUG: ${WEBRTC_LIBRARIES_DEBUG}")
  # message("WEBRTC_LIBRARIES_RELEASE: ${WEBRTC_LIBRARIES_RELEASE}")
  # message("WEBRTC_LIBRARIES: ${WEBRTC_LIBRARIES}")
endif()

# ----------------------------------------------------------------------
# Display status
# ----------------------------------------------------------------------
include(FindPackageHandleStandardArgs)
message("WEBRTC_INCLUDE_DIR: ${WEBRTC_INCLUDE_DIR}")
message("WEBRTC_INCLUDE_DIRS: ${WEBRTC_INCLUDE_DIRS}")

set(WEBRTC_FOUND TRUE)
find_package_handle_standard_args(WEBRTC DEFAULT_MSG WEBRTC_LIBRARIES WEBRTC_INCLUDE_DIRS)

print_module_variables(WEBRTC)

# HACK: WEBRTC_LIBRARIES and WEBRTC_DEPENDENCIES not propagating to parent scope
# while the WEBRTC_DEBUG_LIBRARY and WEBRTC_RELEASE_LIBRARY vars are.
# Setting PARENT_SCOPE fixes this solves theis issue for now.
set(WEBRTC_LIBRARIES ${WEBRTC_LIBRARIES} CACHE INTERNAL "")
set(WEBRTC_DEPENDENCIES ${WEBRTC_DEPENDENCIES} CACHE INTERNAL "")
set(WEBRTC_INCLUDE_DIRS ${WEBRTC_INCLUDE_DIRS} CACHE INTERNAL "")
set(WEBRTC_FOUND ${WEBRTC_FOUND} CACHE INTERNAL "")

mark_as_advanced(WEBRTC_LIBRARIES WEBRTC_INCLUDE_DIR
                 WEBRTC_LIBRARIES_DEBUG WEBRTC_LIBRARIES_RELEASE
                 WEBRTC_BUILD_DIR_SUFFIX_DEBUG WEBRTC_BUILD_DIR_SUFFIX_RELEASE
                 WEBRTC_DEPENDENCIES)
