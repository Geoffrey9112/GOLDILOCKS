###############################################################################
# Linux.cmake
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

# This file includes Linux specific options and quirks, related to system checks

SET(STC_TARGET_OS_LINUX 1)

INCLUDE(CheckSymbolExists)

# Something that needs to be set on legacy reasons
SET(OS_LINUX 1)
SET(STC_HAVE_NPTL 1)
SET(_GNU_SOURCE 1)

# Fix CMake (< 2.8) flags. -rdynamic exports too many symbols.

# Ensure we have clean build for shared libraries
# without unresolved symbols
SET(STC_LINK_FLAG_NO_UNDEFINED "-Wl,--no-undefined")

# 64 bit file offset support flag
SET(_FILE_OFFSET_BITS 64)

# Linux specific HUGETLB /large page support
CHECK_SYMBOL_EXISTS(SHM_HUGETLB sys/shm.h  STC_HAVE_DECL_SHM_HUGETLB)
IF(STC_HAVE_DECL_SHM_HUGETLB)
  SET(STC_HAVE_LARGE_PAGES 1)
  SET(STC_HUGETLB_USE_PROC_MEMINFO 1)
  SET(STC_HAVE_LARGE_PAGE_OPTION 1)
ENDIF()
