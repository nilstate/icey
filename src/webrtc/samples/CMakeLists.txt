# Shared Symple signaller used by sample apps that connect via Symple.
# Not part of the webrtc library itself - webrtc is signalling-agnostic.
set(SYMPLE_SIGNALLER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/symplesignaller.cpp)

add_subdirectory(data-echo)
if(HAVE_FFMPEG)
  add_subdirectory(webcam-streamer)
  add_subdirectory(file-streamer)
  add_subdirectory(media-recorder)
endif()
