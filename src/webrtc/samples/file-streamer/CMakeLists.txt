add_executable(file-streamer
  file-streamer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../symplesignaller.cpp
)
target_include_directories(file-streamer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(file-streamer PRIVATE base crypto net http json symple av webrtc)
