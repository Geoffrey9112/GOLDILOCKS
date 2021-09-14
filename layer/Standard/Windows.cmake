###############################################################################
# Windows.cmake
#
# Copyright (c) 2011, SUNJESOFT Inc.
#
#
# IDENTIFICATION & REVISION
#        $Id$
#
# NOTES
#    
#
##############################################################################

# Something that needs to be set on legacy reasons
SET(STC_TARGET_OS_WINDOWS 1)

# This file includes Windows specific hacks, mostly around compiler flags

INCLUDE (CheckCSourceCompiles)
INCLUDE (CheckStructHasMember)
INCLUDE (CheckLibraryExists)
INCLUDE (CheckFunctionExists)
INCLUDE (CheckCCompilerFlag)
INCLUDE (CheckCSourceRuns)
INCLUDE (CheckSymbolExists)
INCLUDE (CheckTypeSize)

# OS display name (version_compile_os etc).
# Used by the test suite to ignore bugs on some platforms, 
IF(CMAKE_SIZEOF_VOID_P MATCHES 8)
  SET(SYSTEM_TYPE "Win64")
ELSE()
  SET(SYSTEM_TYPE "Win32")
ENDIF()

# Intel compiler is almost Visual C++
# (same compile flags etc). Set MSVC flag
IF(CMAKE_C_COMPILER MATCHES "icl")
 SET(MSVC TRUE)
ENDIF()

ADD_DEFINITIONS("-D_WINDOWS -D__WIN__ -D_CRT_SECURE_NO_DEPRECATE")
ADD_DEFINITIONS("-D_WIN32_WINNT=0x0601")
# Speed up build process excluding unused header files
ADD_DEFINITIONS("-DWIN32_LEAN_AND_MEAN")
  
# Adjust compiler and linker flags
IF(MINGW AND CMAKE_SIZEOF_VOID_P EQUAL 4)
   # mininal architecture flags, i486 enables GCC atomics
  ADD_DEFINITIONS(-march=i486)
ENDIF()

IF(MSVC)
  # Enable debug info also in Release build, and create PDB to be able to analyze 
  # crashes
  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Zi")
  FOREACH(type EXE SHARED MODULE)
   SET(CMAKE_{type}_LINKER_FLAGS_RELEASE "${CMAKE_${type}_LINKER_FLAGS_RELEASE} /debug")
  ENDFOREACH()
  
  # Fix CMake's predefined huge stack size
  FOREACH(type EXE SHARED MODULE)
   STRING(REGEX REPLACE "/STACK:([^ ]+)" "" CMAKE_${type}_LINKER_FLAGS "${CMAKE_${type}_LINKER_FLAGS}")
   STRING(REGEX REPLACE "/INCREMENTAL:([^ ]+)" "" CMAKE_${type}_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_${type}_LINKER_FLAGS_RELWITHDEBINFO}")
  ENDFOREACH()
  
  # Mark 32 bit executables large address aware so they can 
  # use > 2GB address space
  IF(CMAKE_SIZEOF_VOID_P MATCHES 4)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
  ENDIF()
  
  # Speed up multiprocessor build
  IF (MSVC_VERSION GREATER 1400)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
  ENDIF()
  
ENDIF()

# Always link with socket library
LINK_LIBRARIES(ws2_32 Userenv Dbghelp)
# ..also for tests
SET(CMAKE_REQUIRED_LIBRARIES ws2_32)

# System checks
SET(STC_SIGNAL_WITH_VIO_CLOSE 1) # Something that runtime team needs

# IPv6 constants appeared in Vista SDK first. We need to define them in any case if they are 
# not in headers, to handle dual mode sockets correctly.
CHECK_SYMBOL_EXISTS(IPPROTO_IPV6 "winsock2.h" STC_HAVE_IPPROTO_IPV6)
IF(NOT STC_HAVE_IPPROTO_IPV6)
  SET(STC_HAVE_IPPROTO_IPV6 41)
ENDIF()
CHECK_SYMBOL_EXISTS(IPV6_V6ONLY  "winsock2.h;ws2ipdef.h" STC_HAVE_IPV6_V6ONLY)
IF(NOT STC_HAVE_IPV6_V6ONLY)
  SET(STC_HAVE_IPV6_V6ONLY 27)
ENDIF()

# Some standard functions exist there under different
# names (e.g popen is _popen or strok_r is _strtok_s)
# If a replacement function exists, HAVE_FUNCTION is
# defined to 1. CMake variable <function_name> will also
# be defined to the replacement name.
# So for example, CHECK_FUNCTION_REPLACEMENT(popen _popen)
# will define HAVE_POPEN to 1 and set variable named popen
# to _popen. If the header template, one needs to have
# cmakedefine popen @popen@ which will expand to 
# define popen _popen after CONFIGURE_FILE

MACRO(CHECK_FUNCTION_REPLACEMENT function replacement)
  STRING(TOUPPER ${function} function_upper)
  CHECK_FUNCTION_EXISTS(${function} STC_HAVE_${function_upper})
  IF(NOT STC_HAVE_${function_upper})
    CHECK_FUNCTION_EXISTS(${replacement}  STC_HAVE_${replacement})
    IF(STC_HAVE_${replacement})
      SET(STC_HAVE_${function_upper} 1 )
      SET(${function} ${replacement})
    ENDIF()
  ENDIF()
ENDMACRO()
MACRO(CHECK_SYMBOL_REPLACEMENT symbol replacement header)
  STRING(TOUPPER ${symbol} symbol_upper)
  CHECK_SYMBOL_EXISTS(${symbol} ${header} STC_HAVE_${symbol_upper})
  IF(NOT STC_HAVE_${symbol_upper})
    CHECK_SYMBOL_EXISTS(${replacement} ${header} STC_HAVE_${replacement})
    IF(STC_HAVE_${replacement})
      SET(STC_HAVE_${symbol_upper} 1)
      SET(${symbol} ${replacement})
    ENDIF()
  ENDIF()
ENDMACRO()

CHECK_SYMBOL_REPLACEMENT(S_IROTH _S_IREAD sys/stat.h)
CHECK_SYMBOL_REPLACEMENT(S_IFIFO _S_IFIFO sys/stat.h)
CHECK_SYMBOL_REPLACEMENT(SIGQUIT SIGTERM signal.h)
CHECK_SYMBOL_REPLACEMENT(SIGPIPE SIGINT signal.h)
CHECK_SYMBOL_REPLACEMENT(isnan _isnan float.h)
CHECK_SYMBOL_REPLACEMENT(finite _finite float.h)
CHECK_FUNCTION_REPLACEMENT(popen _popen)
CHECK_FUNCTION_REPLACEMENT(pclose _pclose)
CHECK_FUNCTION_REPLACEMENT(access _access)
CHECK_FUNCTION_REPLACEMENT(strcasecmp _stricmp)
CHECK_FUNCTION_REPLACEMENT(strncasecmp _strnicmp)
CHECK_FUNCTION_REPLACEMENT(snprintf _snprintf)
CHECK_FUNCTION_REPLACEMENT(strtok_r strtok_s)
CHECK_FUNCTION_REPLACEMENT(strtoll _strtoi64)
CHECK_FUNCTION_REPLACEMENT(strtoull _strtoui64)
CHECK_FUNCTION_REPLACEMENT(vsnprintf _vsnprintf)
CHECK_TYPE_SIZE(ssize_t SIZE_OF_SSIZE_T)
IF(NOT HAVE_SIZE_OF_SSIZE_T)
 SET(ssize_t SSIZE_T)
ENDIF()

SET(FN_NO_CASE_SENSE 1)
