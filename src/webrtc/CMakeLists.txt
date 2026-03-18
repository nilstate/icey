if(HAVE_LIBDATACHANNEL AND HAVE_OPENSSL AND HAVE_FFMPEG)
  scy_add_module(webrtc
    DEPENDS base net crypto av json
    PACKAGES OpenSSL::SSL OpenSSL::Crypto
    PRETTY_NAME WebRTC
    SKIP_EXPORT
  )

  if(TARGET webrtc)
    # libdatachannel: headers PUBLIC (API exposes rtc:: types), link PRIVATE
    target_include_directories(webrtc
      PUBLIC $<BUILD_INTERFACE:${libdatachannel_SOURCE_DIR}/include>)
    target_link_libraries(webrtc PRIVATE LibDataChannel::LibDataChannelStatic)

    target_link_libraries(webrtc PUBLIC ${FFMPEG_LIBRARIES})
    target_include_directories(webrtc PUBLIC ${FFMPEG_INCLUDE_DIRS})
  endif()
endif()
