if(HAVE_OPENSSL)
  icy_add_module(net
    DEPENDS base crypto
    PACKAGES OpenSSL::SSL OpenSSL::Crypto
  )
endif()
