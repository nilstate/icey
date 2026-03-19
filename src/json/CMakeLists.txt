icy_add_module(json
  DEPENDS base
  PRETTY_NAME JSON
)
# nlohmann/json.hpp is vendored in include/nlohmann/ (MIT licensed, single-header)
# so no external dependency is needed - icy_add_module already adds our include dir.
