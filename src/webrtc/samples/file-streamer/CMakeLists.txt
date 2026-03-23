icy_add_sample(file-streamer DEPENDS base crypto net http json symple av webrtc)
target_sources(file-streamer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../symplesignaller.cpp)
target_include_directories(file-streamer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
