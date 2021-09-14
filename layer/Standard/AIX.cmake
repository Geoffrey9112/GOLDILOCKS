###############################################################################
# AIX.cmake
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
SET(STC_TARGET_OS_AIX 1)

#Enable 64 bit file offsets
SET(_LARGE_FILES 1)

# The following is required to export all symbols 
# (also with leading underscore)
STRING(REPLACE  "-bexpall" "-bexpfull" CMAKE_SHARED_LIBRARY_LINK_C_FLAGS
  "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS}")
