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
add_library(icey::webrtc_support ALIAS webrtc_support)

target_include_directories(webrtc_support
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(webrtc_support
  PUBLIC base json symple webrtc
)

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(webrtc_support PROPERTIES FOLDER "support")
endif()

# Only export when webrtc itself is exported (system deps mode).
# Vendored libdatachannel creates export set conflicts.
if(USE_SYSTEM_DEPS)
  install(TARGETS webrtc_support
    EXPORT iceyTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT dev
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libs
  )
endif()

install(DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT dev
  FILES_MATCHING PATTERN "*.h"
)
