if(HAVE_OPENSSL)
  scy_add_module(net
    DEPENDS base crypto
    PACKAGES OpenSSL::SSL OpenSSL::Crypto
  )
endif()
