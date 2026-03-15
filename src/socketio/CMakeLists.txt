if(HAVE_OPENSSL)
  scy_add_module(socketio
    DEPENDS base crypto net http json
    PACKAGES OpenSSL::SSL OpenSSL::Crypto
    PRETTY_NAME SocketIO
  )
endif()
