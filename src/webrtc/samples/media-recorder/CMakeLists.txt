icy_add_sample(media-recorder DEPENDS base crypto net http json symple av webrtc)
target_sources(media-recorder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../symplesignaller.cpp)
target_include_directories(media-recorder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
