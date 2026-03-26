if(HAVE_LIBDATACHANNEL AND HAVE_OPENSSL AND HAVE_FFMPEG)
  set(_webrtc_export_args)
  if(NOT USE_SYSTEM_DEPS)
    # Vendored libdatachannel exports its own dependency graph, which clashes
    # with the unified icey export set. System packages are safe to export.
    list(APPEND _webrtc_export_args SKIP_EXPORT)
  endif()

  icy_add_module(webrtc
    DEPENDS base net crypto av json
    PACKAGES OpenSSL::SSL OpenSSL::Crypto
    PRETTY_NAME WebRTC
    ${_webrtc_export_args}
  )

  if(TARGET webrtc)
    # libdatachannel: headers PUBLIC (API exposes rtc:: types), link PRIVATE
    if(DEFINED libdatachannel_SOURCE_DIR)
      target_include_directories(webrtc
        PUBLIC $<BUILD_INTERFACE:${libdatachannel_SOURCE_DIR}/include>)
    endif()
    target_link_libraries(webrtc PRIVATE ${ICEY_LIBDATACHANNEL_TARGET})

    target_link_libraries(webrtc PUBLIC ${FFMPEG_LIBRARIES})
    target_include_directories(webrtc PUBLIC ${FFMPEG_INCLUDE_DIRS})
  endif()

  if(BUILD_APPLICATIONS AND TARGET webrtc AND TARGET symple)
    add_subdirectory(apps/media-server)
  endif()
endif()
