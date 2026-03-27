if(NOT TARGET av)
  message(STATUS "  Skipping vision (requires av)")
  return()
endif()

icy_add_module(vision
  DEPENDS base av json
  PRETTY_NAME Vision
)
