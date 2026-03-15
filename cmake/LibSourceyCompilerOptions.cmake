# LibSourcey compiler options
# Modern compilers: GCC 12+, Clang 15+, MSVC 2022+

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(
    -Wall
    -Wextra
    -Wno-unused-parameter
  )

  if(ENABLE_WARNINGS_ARE_ERRORS)
    add_compile_options(-Werror)
  endif()

  if(NOT BUILD_SHARED_LIBS AND NOT ANDROID)
    add_compile_options(-fPIC)
  endif()

  if(UNIX AND NOT ANDROID AND NOT APPLE)
    add_compile_options(-pthread)
    link_libraries(-pthread)
  endif()

  # Debug/Release flags
  add_compile_options(
    $<$<CONFIG:Debug>:-O0>
    $<$<CONFIG:Debug>:-g>
  )

  if(ENABLE_PROFILING)
    add_compile_options(-pg -g)
  endif()
endif()

if(MSVC)
  add_compile_options(
    /W4
    /wd4251  # needs dll-interface
    /wd4100  # unreferenced formal parameter
    /bigobj
    /EHa     # async exception handling
  )
  add_compile_definitions(
    _CRT_SECURE_NO_DEPRECATE
    _CRT_NONSTDC_NO_DEPRECATE
    _SCL_SECURE_NO_WARNINGS
  )

  if(ENABLE_WARNINGS_ARE_ERRORS)
    add_compile_options(/WX)
  endif()
endif()
