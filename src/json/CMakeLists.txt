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
endif()
