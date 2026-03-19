if(HAVE_OPENSSL)
  icy_add_module(crypto
    DEPENDS base
    PACKAGES OpenSSL::SSL OpenSSL::Crypto
  )
endif()
