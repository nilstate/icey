if(TARGET webrtc_support)
  return()
endif()

if(NOT TARGET webrtc)
  message(FATAL_ERROR "webrtc_support requires the webrtc target")
endif()

if(NOT TARGET symple)
  message(FATAL_ERROR "webrtc_support requires the symple target")
endif()

file(GLOB_RECURSE _srcs "src/*.cpp")
file(GLOB_RECURSE _hdrs "include/*.h")

add_library(webrtc_support STATIC ${_srcs} ${_hdrs})

target_include_directories(webrtc_support
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(webrtc_support
  PUBLIC base json symple webrtc
)

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(webrtc_support PROPERTIES FOLDER "support")
endif()
