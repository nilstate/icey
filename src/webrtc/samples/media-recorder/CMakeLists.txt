add_executable(media-recorder
  media-recorder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../symplesignaller.cpp
)
target_include_directories(media-recorder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(media-recorder PRIVATE base crypto net http json symple av webrtc)
