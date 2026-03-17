scy_add_module(av
  DEPENDS base
  PRETTY_NAME AV
)

# FFmpeg is optional for this module
if(HAVE_FFMPEG AND TARGET av)
  target_link_libraries(av PUBLIC ${FFMPEG_LIBRARIES})
  target_include_directories(av PUBLIC ${FFMPEG_INCLUDE_DIRS})
endif()
