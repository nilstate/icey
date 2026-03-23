icy_add_sample(webcam-streamer DEPENDS base crypto net http json symple av webrtc)
target_sources(webcam-streamer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../symplesignaller.cpp)
target_include_directories(webcam-streamer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
