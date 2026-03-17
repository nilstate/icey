if(HAVE_OPENSSL)
  scy_add_module(stun
    DEPENDS base net crypto
    PACKAGES OpenSSL::SSL OpenSSL::Crypto
    PRETTY_NAME STUN
  )
endif()
