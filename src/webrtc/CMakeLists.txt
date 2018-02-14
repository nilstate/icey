ask_build_sourcey_module(webrtc)
if(HAVE_WEBRTC AND (BUILD_MODULES OR (NOT BUILD_MODULES AND BUILD_MODULE_webrtc)))
  include_dependency(WebRTC REQUIRED)

  if(BUILD_SHARED_LIBS)
    message(WARNING "Compiling WebRTC libraries with `BUILD_SHARED_LIBS=OFF` is recommended")
  endif()

  set(webrtc_PRETTY_NAME WebRTC)
  define_sourcey_module(webrtc base av crypto util)
endif()
