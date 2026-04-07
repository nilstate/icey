icy_add_module(av
  DEPENDS base
  PRETTY_NAME AV
)

# FFmpeg is optional for this module
if(HAVE_FFMPEG AND TARGET av)
  if(FFMPEG_LIBRARY_DIRS)
    target_link_directories(av PUBLIC ${FFMPEG_LIBRARY_DIRS})
  endif()
  target_link_libraries(av PUBLIC ${FFMPEG_LIBRARIES})
  target_include_directories(av PUBLIC ${FFMPEG_INCLUDE_DIRS})
endif()

# Platform-specific device libraries
if(TARGET av)
  if(WIN32)
    target_link_libraries(av PUBLIC
      mf mfplat mfreadwrite mfuuid ole32 strmiids)
  elseif(APPLE)
    find_library(COREAUDIO_LIBRARY CoreAudio)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    find_library(FOUNDATION_LIBRARY Foundation)
    find_library(AVFOUNDATION_LIBRARY AVFoundation)
    find_library(COREMEDIA_LIBRARY CoreMedia)
    target_link_libraries(av PUBLIC
      ${COREAUDIO_LIBRARY}
      ${COREFOUNDATION_LIBRARY}
      ${FOUNDATION_LIBRARY}
      ${AVFOUNDATION_LIBRARY}
      ${COREMEDIA_LIBRARY})
  endif()
  # Linux: no extra libs needed (v4l2 ioctl is in libc, inotify in kernel)
endif()
