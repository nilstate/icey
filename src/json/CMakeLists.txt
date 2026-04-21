set(_icey_json_uses_vendored_headers FALSE)

if(USE_SYSTEM_DEPS AND TARGET nlohmann_json::nlohmann_json)
  icy_add_module(json
    DEPENDS base
    PACKAGES nlohmann_json::nlohmann_json
    PRETTY_NAME JSON
  )
else()
  set(_icey_json_uses_vendored_headers TRUE)
  icy_add_module(json
    DEPENDS base
    PRETTY_NAME JSON
  )
endif()

if(_icey_json_uses_vendored_headers AND TARGET json)
  target_include_directories(json
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor>
  )
  install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vendor/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT dev
  )
endif()
