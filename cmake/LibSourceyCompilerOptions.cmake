# Initialize flags
set(LibSourcey_EXTRA_C_FLAGS "")
set(LibSourcey_EXTRA_C_FLAGS_RELEASE "")
set(LibSourcey_EXTRA_C_FLAGS_DEBUG "")
set(LibSourcey_EXTRA_EXE_LINKER_FLAGS "")
set(LibSourcey_EXTRA_EXE_LINKER_FLAGS_RELEASE "")
set(LibSourcey_EXTRA_EXE_LINKER_FLAGS_DEBUG "")

# Set processor flags
if(CMAKE_SYSTEM_PROCESSOR MATCHES amd64.*|x86_64.*)
  set(X86_64 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES i686.*|i386.*|x86.*)
  set(X86 1)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # message(STATUS "Detected version of GNU GCC: ${CMAKE_CXX_COMPILER_VERSION}")

  # Require at least gcc 4.9
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
    message(FATAL_ERROR "GCC version must be at least 4.9!")
  endif()

  # Using c++14 (CMAKE_CXX_FLAGS only, not for CMAKE_C_FLAGS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

  # High level of warnings.
  set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -Wall")

  # The -Wno-long-long is required in 64bit systems when including sytem headers.
  if(X86_64)
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -Wno-long-long")
  endif()

  # We need pthread's
  if(UNIX AND NOT ANDROID AND NOT APPLE)
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -pthread")
  endif()

  # if(BUILD_SHARED_LIBS)
  #  set(LibSourcey_EXTRA_C_FLAGS "-shared ${LibSourcey_EXTRA_C_FLAGS}")
  # else()
  #  set(LibSourcey_EXTRA_C_FLAGS "-static ${LibSourcey_EXTRA_C_FLAGS}")
  # endif()

  if(ENABLE_WARNINGS_ARE_ERRORS)
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -Werror")
  endif()

  if(X86 AND NOT MINGW64 AND NOT X86_64 AND NOT APPLE)
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -march=i686")
  endif()

  # Other optimizations
  if(ENABLE_OMIT_FRAME_POINTER)
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -fomit-frame-pointer")
  else()
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -fno-omit-frame-pointer")
  endif()
  if(ENABLE_FAST_MATH)
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -ffast-math")
  endif()
  if(ENABLE_POWERPC)
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -mcpu=G3 -mtune=G5")
  endif()
  if(ENABLE_SSE)
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -msse")
  endif()
  if(ENABLE_SSE2)
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -msse2")
  endif()

  # SSE3 and further should be disabled under MingW because it generates compiler errors
  if(NOT MINGW)
    if(ENABLE_SSE3)
      set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -msse3")
    endif()

    if(APPLE)
      if(ENABLE_SSSE3)
        set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -mssse3")
      endif()
      if(ENABLE_SSE41)
         set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -msse4.1")
      endif()
      if(ENABLE_SSE42)
         set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -msse4.2")
      endif()
    endif()
  endif(NOT MINGW)

  if (CMAKE_GENERATOR MATCHES "(MinGW)|(MSYS)")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "")
  endif()

  if(X86 OR X86_64)
    if(NOT APPLE AND CMAKE_SIZEOF_VOID_P EQUAL 4)
       if(ENABLE_SSE2)
         set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -mfpmath=sse")# !! important - be on the same wave with x64 compilers
       else()
         set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -mfpmath=387")
       endif()
    endif()
  endif()

  # Profiling?
  if(ENABLE_PROFILING)
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -pg -g")
    # turn off incompatible options
    foreach(flags CMAKE_CXX_FLAGS CMAKE_C_FLAGS CMAKE_CXX_FLAGS_RELEASE CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS_DEBUG CMAKE_C_FLAGS_DEBUG LibSourcey_EXTRA_C_FLAGS_RELEASE)
      string(REPLACE "-fomit-frame-pointer" "" ${flags} "${${flags}}")
      string(REPLACE "-ffunction-sections" "" ${flags} "${${flags}}")
    endforeach()
  elseif(NOT APPLE AND NOT ANDROID)
    # Remove unreferenced functions: function level linking
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} -ffunction-sections")
  endif()

  set(LibSourcey_EXTRA_C_FLAGS_RELEASE "${LibSourcey_EXTRA_C_FLAGS_RELEASE} -DNDEBUG")
  set(LibSourcey_EXTRA_C_FLAGS_DEBUG "${LibSourcey_EXTRA_C_FLAGS_DEBUG} -O0 -DDEBUG -D_DEBUG")
  if(BUILD_WITH_DEBUG_INFO)
    set(LibSourcey_EXTRA_C_FLAGS_DEBUG "${LibSourcey_EXTRA_C_FLAGS_DEBUG} -ggdb3")
  endif()

  if(NOT BUILD_SHARED_LIBS AND NOT ANDROID)
    # Android does not need these settings because they are already set by toolchain file
    set(LibSourcey_LINKER_LIBS ${LibSourcey_LINKER_LIBS} stdc++)
    set(LibSourcey_EXTRA_C_FLAGS "-fPIC ${LibSourcey_EXTRA_C_FLAGS}")
  endif()
endif()

# Visual Studio
if(MSVC)

  # Ensure >= 2012 Update 4 for C++11 and Windows XP build support
  if(MSVC_VERSION LESS 1700)         # VC10-/VS2010-
    message(FATAL_ERROR "The project requires C++11 features. "
      "You need at least Visual Studio 11 (Microsoft Visual Studio 2012), "
      "with Microsoft Visual C++ Compiler 2012 CTP (v110_xp).")
  elseif( MSVC_VERSION EQUAL 1700 )  # VC11/VS2012
    #message( "VC11: use Microsoft Visual Studio 2012 "
    #  "with Microsoft Visual C++ Compiler 2012 CTP (v110_xp)" )
    set(CMAKE_GENERATOR_TOOLSET "v110_xp" CACHE STRING "Platform Toolset" FORCE)
  else() # VC12+, assuming C++11 supported.
  endif()

  if(CMAKE_CXX_FLAGS STREQUAL CMAKE_CXX_FLAGS_INIT)
    # override cmake default exception handling option
    string(REPLACE "/EHsc" "/EHa" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}"  CACHE STRING "Flags used by the compiler during all build types." FORCE)
  endif()

  set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} /D _CRT_SECURE_NO_DEPRECATE /D _CRT_NONSTDC_NO_DEPRECATE /D _SCL_SECURE_NO_WARNINGS")
  # 64-bit portability warnings, in MSVC80
  if(MSVC80)
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} /Wp64")
  endif()

  if(BUILD_WITH_DEBUG_INFO)
    set(LibSourcey_EXTRA_EXE_LINKER_FLAGS_RELEASE "${LibSourcey_EXTRA_EXE_LINKER_FLAGS_RELEASE} /debug")
  endif()

  # Remove unreferenced functions: function level linking
  set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} /Gy")
  if(NOT MSVC_VERSION LESS 1400)
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} /bigobj")
  endif()
  if(BUILD_WITH_DEBUG_INFO)
    set(LibSourcey_EXTRA_C_FLAGS_RELEASE "${LibSourcey_EXTRA_C_FLAGS_RELEASE} /Zi")
  endif()

  # Getting bitten by this bug: https://public.kitware.com/Bug/view.php?id=15496
  if(${CMAKE_GENERATOR} MATCHES "Win64")
    #set(CMAKE_GENERATOR_PLATFORM x64)
    set(CMAKE_MODULE_LINKER_FLAGS "/machine:x64")
    set(CMAKE_SHARED_LINKER_FLAGS "/machine:x64")
    set(CMAKE_STATIC_LINKER_FLAGS "/machine:x64")
    set(CMAKE_EXE_LINKER_FLAGS "/machine:x64")
  endif()

  if(NOT MSVC64)
    # 64-bit MSVC compiler uses SSE/SSE2 by default
    if(ENABLE_SSE)
      set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} /arch:SSE")
    endif()
    if(ENABLE_SSE2)
      set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} /arch:SSE2")
    endif()
  endif()

  if(ENABLE_SSE3)
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} /arch:SSE3")
  endif()
  if(ENABLE_SSE4_1)
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} /arch:SSE4.1")
  endif()

  if(ENABLE_SSE OR ENABLE_SSE2 OR ENABLE_SSE3 OR ENABLE_SSE4_1)
    set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} /Oi")
  endif()

  if(X86 OR X86_64)
    if(CMAKE_SIZEOF_VOID_P EQUAL 4 AND ENABLE_SSE2)
      set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS} /fp:fast")# !! important - be on the same wave with x64 compilers
    endif()
  endif()
endif()

# Extra link libs if the user selects building static libs:
#if(NOT BUILD_SHARED_LIBS AND CMAKE_COMPILER_IS_GNUCXX AND NOT ANDROID)
  # Android does not need these settings because they are already set by toolchain file
  #set(LibSourcey_LINKER_LIBS ${LibSourcey_LINKER_LIBS} stdc++)
  #set(LibSourcey_EXTRA_C_FLAGS "-fPIC ${LibSourcey_EXTRA_C_FLAGS}")
#endif()

# Add user supplied extra options (optimization, etc...)
# ==========================================================
set(LibSourcey_EXTRA_C_FLAGS "${LibSourcey_EXTRA_C_FLAGS}" CACHE INTERNAL "Extra compiler options")
set(LibSourcey_EXTRA_C_FLAGS_RELEASE "${LibSourcey_EXTRA_C_FLAGS_RELEASE}" CACHE INTERNAL "Extra compiler options for Release build")
set(LibSourcey_EXTRA_C_FLAGS_DEBUG "${LibSourcey_EXTRA_C_FLAGS_DEBUG}" CACHE INTERNAL "Extra compiler options for Debug build")
set(LibSourcey_EXTRA_EXE_LINKER_FLAGS "${LibSourcey_EXTRA_EXE_LINKER_FLAGS}" CACHE INTERNAL "Extra linker flags")
set(LibSourcey_EXTRA_EXE_LINKER_FLAGS_RELEASE "${LibSourcey_EXTRA_EXE_LINKER_FLAGS_RELEASE}" CACHE INTERNAL "Extra linker flags for Release build")
set(LibSourcey_EXTRA_EXE_LINKER_FLAGS_DEBUG "${LibSourcey_EXTRA_EXE_LINKER_FLAGS_DEBUG}" CACHE INTERNAL "Extra linker flags for Debug build")

# Combine all "extra" options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LibSourcey_EXTRA_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LibSourcey_EXTRA_C_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  ${LibSourcey_EXTRA_C_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${LibSourcey_EXTRA_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${LibSourcey_EXTRA_C_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${LibSourcey_EXTRA_C_FLAGS_DEBUG}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LibSourcey_EXTRA_EXE_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${LibSourcey_EXTRA_EXE_LINKER_FLAGS_RELEASE}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${LibSourcey_EXTRA_EXE_LINKER_FLAGS_DEBUG}")

if(MSVC)
  # Avoid warnings from MSVC about overriding the /W* option
  # We replace /W3 with /W4 only for C++ files,
  # since all the 3rd-party libraries LibSourcey uses are in C,
  # and we do not care about their warnings.
  string(REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
  string(REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

  # Temporary workaround for "error LNK2026: module unsafe for SAFESEH image"
  # when compiling with certain externally compiled libraries with VS2012,
  # such as http://ffmpeg.zeranoe.com/builds/
  # This disables safe exception handling by default.
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")

  # Use x64 by default. This prevents conflicts with dependencies such as OpenSSL
  # and FFmpeg on Windows.
  # set(CMAKE_GENERATOR_PLATFORM x64)

  # allow extern "C" functions throw exceptions
  foreach(flags CMAKE_C_FLAGS CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_DEBUG)
    string(REPLACE "/EHsc-" "/EHs" ${flags} "${${flags}}")
    string(REPLACE "/EHsc" "/EHs" ${flags} "${${flags}}")

    string(REPLACE "/Zm1000" "" ${flags} "${${flags}}")
  endforeach()

  if(NOT ENABLE_NOISY_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251") #class 'std::XXX' needs to have dll-interface to be used by clients of YYY
  endif()
endif()
