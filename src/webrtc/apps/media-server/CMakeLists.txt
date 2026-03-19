add_executable(media-server
  media-server.cpp
)

target_link_libraries(media-server PRIVATE
  base crypto net http json symple av webrtc
)

target_include_directories(media-server PRIVATE ${CMAKE_BINARY_DIR})
target_compile_definitions(media-server PRIVATE
  ICY_DATA_DIR="${Icey_SOURCE_DIR}/data"
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
