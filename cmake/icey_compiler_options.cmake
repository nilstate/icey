# icey compiler options
# Modern compilers: GCC 12+, Clang 15+, MSVC 2022+

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(
    -Wall
    -Wextra
    -Wno-unused-parameter
  )

  # -Werror is applied per-target in icy_add_module() to avoid
  # breaking FetchContent dependencies (libuv, zlib, llhttp, etc.)

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

  # Release optimizations: native arch tuning + link-time optimization
  option(ENABLE_NATIVE_ARCH "Tune for the build machine's CPU (-march=native)" ON)
  if(ENABLE_NATIVE_ARCH)
    add_compile_options($<$<CONFIG:Release>:-march=native>)
    add_compile_options($<$<CONFIG:RelWithDebInfo>:-march=native>)
  endif()

  option(ENABLE_LTO "Enable link-time optimization (-flto)" ON)
  if(ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT _lto_supported OUTPUT _lto_error)
    if(_lto_supported)
      set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
      set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
    else()
      message(STATUS "LTO not supported: ${_lto_error}")
    endif()
  endif()

  # Hide internal symbols by default (smaller binary, faster loads)
  add_compile_options(-fvisibility=hidden)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden>)

  if(ENABLE_PROFILING)
    add_compile_options(-pg -g)
  endif()

  # AddressSanitizer. Applied via CMAKE_*_FLAGS so FetchContent
  # dependencies (libuv, zlib, llhttp) inherit the flags.
  option(ASAN "Enable AddressSanitizer" OFF)
  if(ASAN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address" CACHE STRING "" FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address" CACHE STRING "" FORCE)
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

  # /WX is applied per-target in icy_add_module() to avoid
  # breaking FetchContent dependencies
endif()
