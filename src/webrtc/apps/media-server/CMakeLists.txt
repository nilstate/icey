if(NOT TARGET webrtc_support)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../support
                   ${CMAKE_CURRENT_BINARY_DIR}/webrtc-support)
endif()

add_executable(media-server
  internal/app.cpp
  internal/config.cpp
  internal/httpfactory.cpp
  internal/media.cpp
  internal/turnserver.cpp
  media-server.cpp
)

target_link_libraries(media-server PRIVATE
  base crypto net http json symple av stun turn webrtc webrtc_support
)

target_include_directories(media-server PRIVATE ${CMAKE_BINARY_DIR})
target_compile_definitions(media-server PRIVATE
  ICY_DATA_DIR="${icey_SOURCE_DIR}/data"
)

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(media-server PROPERTIES FOLDER "applications")
endif()

# Copy config to build directory
configure_file(config.json ${CMAKE_CURRENT_BINARY_DIR}/config.json COPYONLY)

install(TARGETS media-server
  RUNTIME DESTINATION bin
  COMPONENT applications
)
