if(HAVE_OPENSSL)
  scy_add_module(symple
    DEPENDS base crypto net http json
    PACKAGES OpenSSL::SSL OpenSSL::Crypto
  )
endif()
