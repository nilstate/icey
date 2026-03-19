if(HAVE_OPENSSL)
  icy_add_module(stun
    DEPENDS base net crypto
    PACKAGES OpenSSL::SSL OpenSSL::Crypto
    PRETTY_NAME STUN
  )
endif()
