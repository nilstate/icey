if(HAVE_OPENSSL)
  icy_add_module(http
    DEPENDS base net crypto
    PACKAGES OpenSSL::SSL OpenSSL::Crypto llhttp_static
    PRETTY_NAME HTTP
  )
endif()
