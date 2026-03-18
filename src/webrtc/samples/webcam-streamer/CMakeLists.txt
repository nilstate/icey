add_executable(webcam-streamer
  webcam-streamer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../symplesignaller.cpp
)
target_include_directories(webcam-streamer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(webcam-streamer PRIVATE base crypto net http json socketio symple av webrtc)
