if(NOT TARGET webrtc_support)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../support
                   ${CMAKE_CURRENT_BINARY_DIR}/webrtc-support)
endif()

add_subdirectory(data-echo)
if(HAVE_FFMPEG)
  add_subdirectory(webcam-streamer)
  add_subdirectory(file-streamer)
  add_subdirectory(media-recorder)
endif()
