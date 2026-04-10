if(USE_SYSTEM_DEPS)
  icy_add_module(json
    DEPENDS base
    PACKAGES nlohmann_json::nlohmann_json
    PRETTY_NAME JSON
  )
else()
  icy_add_module(json
    DEPENDS base
    PRETTY_NAME JSON
  )
  target_include_directories(json
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor>
  )
  install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vendor/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT dev
  )
endif()
