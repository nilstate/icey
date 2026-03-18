# FindFFmpeg.cmake - Find FFmpeg libraries via pkg-config
#
# Once done this will define:
#  FFmpeg_FOUND         - System has all required components
#  FFmpeg_INCLUDE_DIRS  - Include directories
#  FFmpeg_LIBRARIES     - Libraries to link
#  HAVE_FFMPEG          - Set if found (for config header)
#
# Per-component variables:
#  FFmpeg_<COMPONENT>_FOUND
#  FFmpeg_<COMPONENT>_INCLUDE_DIRS
#  FFmpeg_<COMPONENT>_LIBRARIES
#  FFmpeg_<COMPONENT>_VERSION

if (NOT FFmpeg_FIND_COMPONENTS)
  set(FFmpeg_FIND_COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE SWRESAMPLE)
endif()

# Optional components that don't fail the overall find if missing
set(_FFmpeg_OPTIONAL_COMPONENTS AVDEVICE AVFILTER)

find_package(PkgConfig QUIET)

set(FFmpeg_FOUND TRUE)
set(FFmpeg_INCLUDE_DIRS "")
set(FFmpeg_LIBRARIES "")

# Map component names to pkg-config module names
set(_FFmpeg_PKG_AVCODEC    libavcodec)
set(_FFmpeg_PKG_AVFORMAT   libavformat)
set(_FFmpeg_PKG_AVUTIL     libavutil)
set(_FFmpeg_PKG_AVFILTER   libavfilter)
set(_FFmpeg_PKG_AVDEVICE   libavdevice)
set(_FFmpeg_PKG_AVRESAMPLE libavresample)
set(_FFmpeg_PKG_SWSCALE    libswscale)
set(_FFmpeg_PKG_SWRESAMPLE libswresample)
set(_FFmpeg_PKG_POSTPROC   libpostproc)

# Also probe optional components (avdevice, avfilter) if not already requested
set(_FFmpeg_ALL_COMPONENTS ${FFmpeg_FIND_COMPONENTS})
foreach(_opt ${_FFmpeg_OPTIONAL_COMPONENTS})
  list(FIND _FFmpeg_ALL_COMPONENTS ${_opt} _idx)
  if (_idx EQUAL -1)
    list(APPEND _FFmpeg_ALL_COMPONENTS ${_opt})
  endif()
endforeach()

foreach(_component ${_FFmpeg_ALL_COMPONENTS})
  set(_pkg ${_FFmpeg_PKG_${_component}})
  if (NOT _pkg)
    message(WARNING "FindFFmpeg: unknown component ${_component}")
    set(FFmpeg_${_component}_FOUND FALSE)
    set(FFmpeg_FOUND FALSE)
    continue()
  endif()

  pkg_check_modules(_FF_${_component} QUIET ${_pkg})

  if (_FF_${_component}_FOUND)
    set(FFmpeg_${_component}_FOUND TRUE)
    set(FFmpeg_${_component}_INCLUDE_DIRS ${_FF_${_component}_INCLUDE_DIRS})
    set(FFmpeg_${_component}_LIBRARIES ${_FF_${_component}_LIBRARIES})
    set(FFmpeg_${_component}_VERSION ${_FF_${_component}_VERSION})

    list(APPEND FFmpeg_INCLUDE_DIRS ${_FF_${_component}_INCLUDE_DIRS})
    list(APPEND FFmpeg_LIBRARIES ${_FF_${_component}_LIBRARIES})
  else()
    # Fallback: try find_path + find_library
    string(TOLOWER ${_component} _comp_lower)
    find_path(_FF_${_component}_INCDIR
      NAMES lib${_comp_lower}/${_comp_lower}.h
      PATH_SUFFIXES ffmpeg
    )
    find_library(_FF_${_component}_LIB
      NAMES ${_comp_lower}
    )
    if (_FF_${_component}_INCDIR AND _FF_${_component}_LIB)
      set(FFmpeg_${_component}_FOUND TRUE)
      set(FFmpeg_${_component}_INCLUDE_DIRS ${_FF_${_component}_INCDIR})
      set(FFmpeg_${_component}_LIBRARIES ${_FF_${_component}_LIB})
      list(APPEND FFmpeg_INCLUDE_DIRS ${_FF_${_component}_INCDIR})
      list(APPEND FFmpeg_LIBRARIES ${_FF_${_component}_LIB})
    else()
      set(FFmpeg_${_component}_FOUND FALSE)
      # Only fail overall find for required (non-optional) components
      list(FIND _FFmpeg_OPTIONAL_COMPONENTS ${_component} _opt_idx)
      if (_opt_idx EQUAL -1)
        set(FFmpeg_FOUND FALSE)
      endif()
    endif()
  endif()
endforeach()

if (FFmpeg_INCLUDE_DIRS)
  list(REMOVE_DUPLICATES FFmpeg_INCLUDE_DIRS)
endif()
if (FFmpeg_LIBRARIES)
  list(REMOVE_DUPLICATES FFmpeg_LIBRARIES)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFmpeg
  DEFAULT_MSG
  FFmpeg_LIBRARIES
  FFmpeg_INCLUDE_DIRS
)

# Set uppercase aliases for backward compatibility
set(FFMPEG_FOUND ${FFmpeg_FOUND})
set(FFMPEG_INCLUDE_DIRS ${FFmpeg_INCLUDE_DIRS})
set(FFMPEG_LIBRARIES ${FFmpeg_LIBRARIES})

mark_as_advanced(
  FFmpeg_INCLUDE_DIRS
  FFmpeg_LIBRARIES
  FFmpeg_FOUND
)
