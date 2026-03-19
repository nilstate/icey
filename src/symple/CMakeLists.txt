if(HAVE_OPENSSL)
  icy_add_module(symple
    DEPENDS base crypto net http json
    PACKAGES OpenSSL::SSL OpenSSL::Crypto
  )
endif()
