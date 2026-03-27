if(NOT TARGET av)
  message(STATUS "  Skipping speech (requires av)")
  return()
endif()

icy_add_module(speech
  DEPENDS base av json
  PRETTY_NAME Speech
)
