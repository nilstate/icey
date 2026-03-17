if(HAVE_OPENSSL)
  scy_add_module(crypto
    DEPENDS base
    PACKAGES OpenSSL::SSL OpenSSL::Crypto
  )
endif()
