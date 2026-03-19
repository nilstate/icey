if(HAVE_OPENSSL)
  icy_add_module(turn
    DEPENDS base net stun crypto
    PRETTY_NAME TURN
  )
endif()
