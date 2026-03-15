if(HAVE_OPENSSL)
  scy_add_module(turn
    DEPENDS base net stun crypto
    PRETTY_NAME TURN
  )
endif()
