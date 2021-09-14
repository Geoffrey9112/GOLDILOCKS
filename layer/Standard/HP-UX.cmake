###############################################################################
# HP-UX.cmake
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
SET(STC_TARGET_OS_HPUX 1)

# Enable 64 bit file offsets
SET(_LARGEFILE64_SOURCE 1)
SET(_FILE_OFFSET_BITS 64)

# If Itanium make shared library suffix .so
# OS understands both .sl and .so. CMake would
# use .sl, however Gliese prefers .so

IF(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "9000")
  SET(CMAKE_SHARED_LIBRARY_SUFFIX ".so" )
  SET(CMAKE_SHARED_LIBRARY_SUFFIX ".so" PARENT_SCOPE )
  SET(CMAKE_SHARED_MODULE_SUFFIX ".so" )
  SET(CMAKE_SHARED_MODULE_SUFFIX ".so" PARENT_SCOPE )
ENDIF()

IF(CMAKE_SYSTEM MATCHES "11")
  ADD_DEFINITIONS(-DHPUX11)
ENDIF()
