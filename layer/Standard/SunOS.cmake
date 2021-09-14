###############################################################################
# SunOS.cmake
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
SET(STC_TARGET_OS_SOLARIS 1)

INCLUDE(CheckSymbolExists)
INCLUDE(CheckCSourceRuns)
INCLUDE(CheckCSourceCompiles) 

# Enable 64 bit file offsets
SET(_FILE_OFFSET_BITS 64)

# On  Solaris, use of intrinsics will screw the lib search logic
# Force using -lm, so rint etc are found.
SET(LIBM m)

# CMake defined -lthread as thread flag. This crashes in dlopen 
# when trying to load plugins workaround with -lpthread
SET(CMAKE_THREADS_LIBS_INIT -lpthread CACHE INTERNAL "" FORCE)

# Solaris specific large page support
CHECK_SYMBOL_EXISTS(MHA_MAPSIZE_VA sys/mman.h  STC_HAVE_DECL_MHA_MAPSIZE_VA)
IF(STC_HAVE_DECL_MHA_MAPSIZE_VA)
 SET(STC_HAVE_SOLARIS_LARGE_PAGES 1)
 SET(STC_HAVE_LARGE_PAGE_OPTION 1)
ENDIF()


# Solaris atomics
CHECK_C_SOURCE_RUNS(
 "
 #include  <atomic.h>
  int main()
  {
    int foo = -10; int bar = 10;
    int64_t foo64 = -10; int64_t bar64 = 10;
    if (atomic_add_int_nv((uint_t *)&foo, bar) || foo)
      return -1;
    bar = atomic_swap_uint((uint_t *)&foo, (uint_t)bar);
    if (bar || foo != 10)
     return -1;
    bar = atomic_cas_uint((uint_t *)&bar, (uint_t)foo, 15);
    if (bar)
      return -1;
    if (atomic_add_64_nv((volatile uint64_t *)&foo64, bar64) || foo64)
      return -1;
    bar64 = atomic_swap_64((volatile uint64_t *)&foo64, (uint64_t)bar64);
    if (bar64 || foo64 != 10)
      return -1;
    bar64 = atomic_cas_64((volatile uint64_t *)&bar64, (uint_t)foo64, 15);
    if (bar64)
      return -1;
    atomic_or_64((volatile uint64_t *)&bar64, 0);
    return 0;
  }
"  HAVE_SOLARIS_ATOMIC)


# Check is special processor flag needs to be set on older GCC
#that defaults to v8 sparc . Code here is taken from my_rdtsc.c 
IF(CMAKE_COMPILER_IS_GNUCC AND CMAKE_SIZEOF_VOID_P EQUAL 4
  AND CMAKE_SYSTEM_PROCESSOR MATCHES "sparc")
  SET(SOURCE
  "
  int main()
  {
     long high\;
     long low\;
    __asm __volatile__ (\"rd %%tick,%1\; srlx %1,32,%0\" : \"=r\" ( high), \"=r\" (low))\;
    return 0\;
  } ")
  CHECK_C_SOURCE_COMPILES(${SOURCE}  STC_HAVE_SPARC32_TICK)
  IF(NOT STC_HAVE_SPARC32_TICK)
    SET(CMAKE_REQUIRED_FLAGS "-mcpu=v9")
    CHECK_C_SOURCE_COMPILES(${SOURCE}  STC_HAVE_SPARC32_TICK_WITH_V9)
    SET(CMAKE_REQUIRED_FLAGS)
    IF(STC_HAVE_SPARC32_TICK_WITH_V9)
      SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=v9")
    ENDIF()
  ENDIF()
ENDIF()
