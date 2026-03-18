add_executable(data-echo
  data-echo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../symplesignaller.cpp
)
target_include_directories(data-echo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(data-echo PRIVATE base crypto net http json socketio symple webrtc)
