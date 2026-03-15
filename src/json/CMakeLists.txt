scy_add_module(json
  DEPENDS base
  PRETTY_NAME JSON
)
if(TARGET json)
  # Link privately to avoid exporting nlohmann_json target, but propagate include path
  # for consumers who include scy/json/json.h (which includes <nlohmann/json.hpp>)
  get_target_property(_nlohmann_inc nlohmann_json::nlohmann_json INTERFACE_INCLUDE_DIRECTORIES)
  target_include_directories(json PUBLIC ${_nlohmann_inc})
endif()
