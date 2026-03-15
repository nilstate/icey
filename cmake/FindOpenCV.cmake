# Find OpenCV library (4.x+)
#
# Uses OpenCV's own CMake config (OpenCVConfig.cmake) which ships with
# all modern OpenCV installations. Falls back to pkg-config.
#
# Sets:
#   OPENCV_FOUND
#   OPENCV_INCLUDE_DIRS  (alias for OpenCV_INCLUDE_DIRS)
#   OPENCV_LIBRARIES     (alias for OpenCV_LIBS)
#   OPENCV_VERSION       (alias for OpenCV_VERSION)

# Try OpenCV's own config-based find first (preferred)
find_package(OpenCV 4.0 QUIET CONFIG)

if(OpenCV_FOUND)
  set(OPENCV_FOUND TRUE)
  set(OPENCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS})
  set(OPENCV_LIBRARIES ${OpenCV_LIBS})
  set(OPENCV_VERSION ${OpenCV_VERSION})
  message(STATUS "Found OpenCV ${OPENCV_VERSION} (config mode)")
  return()
endif()

# Fallback: pkg-config
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(OPENCV QUIET opencv4)
  if(NOT OPENCV_FOUND)
    pkg_check_modules(OPENCV QUIET opencv)
  endif()
  if(OPENCV_FOUND)
    message(STATUS "Found OpenCV ${OPENCV_VERSION} (pkg-config)")
    return()
  endif()
endif()

# Manual fallback for non-standard installations
find_path(OPENCV_INCLUDE_DIRS
  NAMES opencv2/core.hpp
  PATHS
    /usr/local/include
    /usr/include
    $ENV{OPENCV_DIR}/include
)

find_library(OPENCV_CORE_LIB
  NAMES opencv_core
  PATHS
    /usr/local/lib
    /usr/lib
    /usr/lib/x86_64-linux-gnu
    /usr/lib64
    $ENV{OPENCV_DIR}/lib
)

if(OPENCV_INCLUDE_DIRS AND OPENCV_CORE_LIB)
  set(OPENCV_FOUND TRUE)
  get_filename_component(OPENCV_LIBRARY_DIR ${OPENCV_CORE_LIB} DIRECTORY)

  # Find all opencv_ libraries in the directory
  file(GLOB _opencv_libs "${OPENCV_LIBRARY_DIR}/libopencv_*.so" "${OPENCV_LIBRARY_DIR}/libopencv_*.a")
  set(OPENCV_LIBRARIES "")
  foreach(lib ${_opencv_libs})
    list(APPEND OPENCV_LIBRARIES ${lib})
  endforeach()

  message(STATUS "Found OpenCV (manual search) in ${OPENCV_LIBRARY_DIR}")
else()
  set(OPENCV_FOUND FALSE)
  if(OPENCV_FIND_REQUIRED)
    message(FATAL_ERROR "OpenCV >= 4.0 not found. Set OpenCV_DIR or OPENCV_DIR.")
  endif()
endif()

mark_as_advanced(OPENCV_INCLUDE_DIRS OPENCV_LIBRARIES OPENCV_CORE_LIB)
