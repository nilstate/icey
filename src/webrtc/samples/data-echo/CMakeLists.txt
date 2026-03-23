icy_add_sample(data-echo DEPENDS base crypto net http json symple webrtc)
target_sources(data-echo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../symplesignaller.cpp)
target_include_directories(data-echo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
