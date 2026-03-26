if(NOT TARGET webrtc_support)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../support
                   ${CMAKE_CURRENT_BINARY_DIR}/webrtc-support)
endif()

icy_add_test(webrtctests DEPENDS base av webrtc webrtc_support)
