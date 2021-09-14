
# Copyright (C) 2009 Sun Microsystems,Inc
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

INCLUDE (CheckCSourceCompiles)
INCLUDE (CheckStructHasMember)
INCLUDE (CheckLibraryExists)
INCLUDE (CheckFunctionExists)
INCLUDE (CheckCCompilerFlag)
INCLUDE (CheckCSourceRuns)
INCLUDE (CheckSymbolExists)

# System type affects version_compile_os variable 
IF(NOT SYSTEM_TYPE)
  IF(PLATFORM)
    SET(STC_SYSTEM_TYPE ${PLATFORM})
  ELSE()
    SET(STC_SYSTEM_TYPE ${CMAKE_SYSTEM_NAME})
  ENDIF()
ENDIF()

# Large files, common flag
SET(_LARGEFILE_SOURCE  1)

# If finds the size of a type, set SIZEOF_<type> and HAVE_<type>
FUNCTION(STC_CHECK_TYPE_SIZE type defbase)
  IF( CMAKE_NEED_RECOMPILE MATCHES "TRUE" )
    UNSET( HAVE_STC_SIZEOF_${defbase} CACHE )
  ENDIF()
  CHECK_TYPE_SIZE("${type}" STC_SIZEOF_${defbase})
  IF(STC_SIZEOF_${defbase})
    SET(STC_HAVE_${defbase} 1 PARENT_SCOPE)
  ENDIF()
ENDFUNCTION()

# Same for structs, setting HAVE_STRUCT_<name> instead
FUNCTION(STC_CHECK_STRUCT_SIZE type defbase)
  IF( CMAKE_NEED_RECOMPILE MATCHES "TRUE" )
    UNSET( HAVE_STC_SIZEOF_${defbase} CACHE )
  ENDIF()
  CHECK_TYPE_SIZE("struct ${type}" STC_SIZEOF_${defbase})
  IF(STC_SIZEOF_${defbase})
    SET(STC_HAVE_STRUCT_${defbase} 1 PARENT_SCOPE)
  ENDIF()
ENDFUNCTION()

#
# Tests for header files
#
INCLUDE (CheckIncludeFiles)

CHECK_INCLUDE_FILES ("stdlib.h;stdarg.h;string.h;float.h" STDC_HEADERS)
CHECK_INCLUDE_FILES (stdio.h STC_HAVE_STDIO_H)
CHECK_INCLUDE_FILES (sys/resource.h STC_HAVE_SYS_RESOURCE_H)
CHECK_INCLUDE_FILES (sys/types.h STC_HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES (sys/ipc.h STC_HAVE_SYS_IPC_H)
CHECK_INCLUDE_FILES (sys/msg.h STC_HAVE_SYS_MSG_H)
CHECK_INCLUDE_FILES (alloca.h STC_HAVE_ALLOCA_H)
CHECK_INCLUDE_FILES (aio.h STC_HAVE_AIO_H)
CHECK_INCLUDE_FILES (arpa/inet.h STC_HAVE_ARPA_INET_H)
CHECK_INCLUDE_FILES (net/if.h STC_HAVE_NET_IF_H)
CHECK_INCLUDE_FILES (asm/msr.h STC_HAVE_ASM_MSR_H)
CHECK_INCLUDE_FILES (crypt.h STC_HAVE_CRYPT_H)
CHECK_INCLUDE_FILES (ctype.h STC_HAVE_CTYPE_H)
CHECK_INCLUDE_FILES (dirent.h STC_HAVE_DIRENT_H)
CHECK_INCLUDE_FILES (dl.h STC_HAVE_DL_H)
CHECK_INCLUDE_FILES (dlfcn.h STC_HAVE_DLFCN_H)
CHECK_INCLUDE_FILES (errno.h STC_HAVE_ERRNO_H)
CHECK_INCLUDE_FILES (execinfo.h STC_HAVE_EXECINFO_H)
CHECK_INCLUDE_FILES (fcntl.h STC_HAVE_FCNTL_H)
CHECK_INCLUDE_FILES (fenv.h STC_HAVE_FENV_H)
CHECK_INCLUDE_FILES (float.h STC_HAVE_FLOAT_H)
CHECK_INCLUDE_FILES (floatingpoint.h STC_HAVE_FLOATINGPOINT_H)
CHECK_INCLUDE_FILES (fpu_control.h STC_HAVE_FPU_CONTROL_H)
CHECK_INCLUDE_FILES (grp.h STC_HAVE_GRP_H)
CHECK_INCLUDE_FILES (ia64intrin.h STC_HAVE_IA64INTRIN_H)
CHECK_INCLUDE_FILES (ieeefp.h STC_HAVE_IEEEFP_H)
CHECK_INCLUDE_FILES (inttypes.h STC_HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES (langinfo.h STC_HAVE_LANGINFO_H)
CHECK_INCLUDE_FILES (limits.h STC_HAVE_LIMITS_H)
CHECK_INCLUDE_FILES (locale.h STC_HAVE_LOCALE_H)
CHECK_INCLUDE_FILES (malloc.h STC_HAVE_MALLOC_H)
CHECK_INCLUDE_FILES (memory.h STC_HAVE_MEMORY_H)
CHECK_INCLUDE_FILES (ndir.h STC_HAVE_NDIR_H)
CHECK_INCLUDE_FILES (netinet/in.h STC_HAVE_NETINET_IN_H)
CHECK_INCLUDE_FILES (paths.h STC_HAVE_PATHS_H)
CHECK_INCLUDE_FILES (port.h STC_HAVE_PORT_H)
CHECK_INCLUDE_FILES (poll.h STC_HAVE_POLL_H)
CHECK_INCLUDE_FILES (process.h STC_HAVE_PROCESS_H)
CHECK_INCLUDE_FILES (pthread.h STC_HAVE_PTHREAD_H)
CHECK_INCLUDE_FILES (pwd.h STC_HAVE_PWD_H)
CHECK_INCLUDE_FILES (sched.h STC_HAVE_SCHED_H)
CHECK_INCLUDE_FILES (select.h STC_HAVE_SELECT_H)
CHECK_INCLUDE_FILES (semaphore.h STC_HAVE_SEMAPHORE_H)
CHECK_INCLUDE_FILES ("sys/types.h;sys/dir.h" STC_HAVE_SYS_DIR_H)
CHECK_INCLUDE_FILES (sys/ndir.h STC_HAVE_SYS_NDIR_H)
CHECK_INCLUDE_FILES (sys/pte.h STC_HAVE_SYS_PTE_H)
CHECK_INCLUDE_FILES (stddef.h STC_HAVE_STDDEF_H)
CHECK_INCLUDE_FILES (stdint.h STC_HAVE_STDINT_H)
CHECK_INCLUDE_FILES (stdlib.h STC_HAVE_STDLIB_H)
CHECK_INCLUDE_FILES (strings.h STC_HAVE_STRINGS_H)
CHECK_INCLUDE_FILES (string.h STC_HAVE_STRING_H)
CHECK_INCLUDE_FILES (synch.h STC_HAVE_SYNCH_H)
CHECK_INCLUDE_FILES (sysent.h STC_HAVE_SYSENT_H)
CHECK_INCLUDE_FILES (sys/cdefs.h STC_HAVE_SYS_CDEFS_H)
CHECK_INCLUDE_FILES (sys/epoll.h STC_HAVE_SYS_EPOLL_H)
CHECK_INCLUDE_FILES (sys/file.h STC_HAVE_SYS_FILE_H)
CHECK_INCLUDE_FILES (sys/fpu.h STC_HAVE_SYS_FPU_H)
CHECK_INCLUDE_FILES (sys/ioctl.h STC_HAVE_SYS_IOCTL_H)
CHECK_INCLUDE_FILES (sys/ipc.h STC_HAVE_SYS_IPC_H)
CHECK_INCLUDE_FILES (sys/malloc.h STC_HAVE_SYS_MALLOC_H)
CHECK_INCLUDE_FILES (sys/mman.h STC_HAVE_SYS_MMAN_H)
CHECK_INCLUDE_FILES (sys/prctl.h STC_HAVE_SYS_PRCTL_H)
CHECK_INCLUDE_FILES (sys/resource.h STC_HAVE_SYS_RESOURCE_H)
CHECK_INCLUDE_FILES (sys/select.h STC_HAVE_SYS_SELECT_H)
CHECK_INCLUDE_FILES (sys/shm.h STC_HAVE_SYS_SHM_H)
CHECK_INCLUDE_FILES (sys/socket.h STC_HAVE_SYS_SOCKET_H)
CHECK_INCLUDE_FILES (sys/stat.h STC_HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES (sys/stream.h STC_HAVE_SYS_STREAM_H)
CHECK_INCLUDE_FILES (sys/syslimits.h STC_HAVE_SYS_SYSLIMITES_H)
CHECK_INCLUDE_FILES (sys/termcap.h STC_HAVE_SYS_TERMCAP_H)
CHECK_INCLUDE_FILES ("time.h;sys/timeb.h" STC_HAVE_SYS_TIMEB_H)
CHECK_INCLUDE_FILES (sys/time.h STC_HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILES (sys/times.h STC_HAVE_SYS_TIMES_H)
CHECK_INCLUDE_FILES ("curses.h;term.h" STC_HAVE_TERM_H)
CHECK_INCLUDE_FILES (asm/termbits.h STC_HAVE_ASM_TERMBITS_H)
CHECK_INCLUDE_FILES (termbits.h STC_HAVE_TERMBITS_H)
CHECK_INCLUDE_FILES (termios.h STC_HAVE_TERMIOS_H)
CHECK_INCLUDE_FILES (termio.h STC_HAVE_TERMIO_H)
CHECK_INCLUDE_FILES (termcap.h STC_HAVE_TERMCAP_H)
CHECK_INCLUDE_FILES (unistd.h STC_HAVE_UNISTD_H)
CHECK_INCLUDE_FILES (utime.h STC_HAVE_UTIME_H)
CHECK_INCLUDE_FILES (varargs.h STC_HAVE_VARARGS_H)
CHECK_INCLUDE_FILES (sys/time.h STC_HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILES (sys/utime.h STC_HAVE_SYS_UTIME_H)
CHECK_INCLUDE_FILES (sys/wait.h STC_HAVE_SYS_WAIT_H)
CHECK_INCLUDE_FILES (sys/param.h STC_HAVE_SYS_PARAM_H)
CHECK_INCLUDE_FILES (sys/vadvise.h STC_HAVE_SYS_VADVISE_H)
CHECK_INCLUDE_FILES (fnmatch.h STC_HAVE_FNMATCH_H)
CHECK_INCLUDE_FILES (stdarg.h  STC_HAVE_STDARG_H)
CHECK_INCLUDE_FILES ("stdlib.h;sys/un.h" STC_HAVE_SYS_UN_H)
CHECK_INCLUDE_FILES (time.h STC_HAVE_TIME_H)
CHECK_INCLUDE_FILES (vis.h STC_HAVE_VIS_H)
CHECK_INCLUDE_FILES (wchar.h STC_HAVE_WCHAR_H)
CHECK_INCLUDE_FILES (wctype.h STC_HAVE_WCTYPE_H)
CHECK_INCLUDE_FILES (windows.h STC_HAVE_WINDOWS_H)
CHECK_INCLUDE_FILES (winsock2.h STC_HAVE_WINSOCK2_H)
CHECK_INCLUDE_FILES (winsock.h STC_HAVE_WINSOCK_H)
CHECK_INCLUDE_FILES (mswsock.h STC_HAVE_MSWSOCK_H)
CHECK_INCLUDE_FILES (math.h STC_HAVE_MATH_H)
CHECK_INCLUDE_FILES (signal.h STC_HAVE_SIGNAL_H)
CHECK_INCLUDE_FILES (procfs.h STC_HAVE_PROCFS_H)
CHECK_INCLUDE_FILES (sys/procfs.h STC_HAVE_SYS_PROCFS_H)
CHECK_INCLUDE_FILES (sys/pstat.h STC_HAVE_SYS_PSTAT_H)
CHECK_INCLUDE_FILES (unwind.h STC_HAVE_UNWIND_H)
CHECK_INCLUDE_FILES (uwx.h STC_HAVE_UWX_H)
CHECK_INCLUDE_FILES (setjmp.h STC_HAVE_SETJMP_H)
CHECK_INCLUDE_FILES (infiniband/verbs.h STC_HAVE_INFINIBAND_VERBS_H)
CHECK_INCLUDE_FILES (rdma/rdma_cma.h STC_HAVE_INFINIBAND_RDMACM_H)
CHECK_INCLUDE_FILES (rdma/rdma_verbs.h STC_HAVE_INFINIBAND_RDMAVERBS_H)

IF(STC_HAVE_SYS_STREAM_H)
  # Needs sys/stream.h on Solaris
  CHECK_INCLUDE_FILES ("sys/stream.h;sys/ptem.h" STC_HAVE_SYS_PTEM_H)
ELSE()
  CHECK_INCLUDE_FILES (sys/ptem.h STC_HAVE_SYS_PTEM_H)
ENDIF()

IF(STC_HAVE_WINDOWS_H)
  SET(STC_INCLUDE_WINDOWS_H "#include <windows.h>")
ENDIF()

IF(STC_HAVE_WINSOCK2_H)
  SET(STC_INCLUDE_WINSOCK2_H "#include <winsock2.h>")
ENDIF()

IF(STC_HAVE_WINSOCK_H)
  SET(STC_INCLUDE_WINSOCK_H "#include <winsock.h>")
ENDIF()

IF(STC_HAVE_MSWOCK_H)
  SET(STC_INCLUDE_MSWSOCK_H "#include <mswsock.h>")
ENDIF()

IF(STC_HAVE_SYS_RESOURCE_H)
  SET(STC_INCLUDE_SYS_RESOURCE_H "#include <sys/resource.h>")
ENDIF()

IF(STC_HAVE_SYS_TYPES_H)
  SET(STC_INCLUDE_SYS_TYPES_H "#include <sys/types.h>")
ENDIF()

IF(STC_HAVE_SYS_IPC_H)
  SET(STC_INCLUDE_SYS_IPC_H "#include <sys/ipc.h>")
ENDIF()

IF(STC_HAVE_SYS_MSG_H)
  SET(STC_INCLUDE_SYS_MSG_H "#include <sys/msg.h>")
ENDIF()

IF(STC_HAVE_STDIO_H)
  SET(STC_INCLUDE_STDIO_H "#include <stdio.h>")
ENDIF()

IF(STC_HAVE_STDLIB_H)
  SET(STC_INCLUDE_STDLIB_H "#include <stdlib.h>")
ENDIF()

IF(STC_HAVE_STDARG_H)
  SET(STC_INCLUDE_STDARG_H "#include <stdarg.h>")
ENDIF()

IF(STC_HAVE_STDINT_H)
  SET(STC_INCLUDE_STDINT_H "#include <stdint.h>")
ENDIF()

IF(STC_HAVE_FCNTL_H)
  SET(STC_INCLUDE_FCNTL_H "#include <fcntl.h>")
ENDIF()

IF(STC_HAVE_STRING_H)
  SET(STC_INCLUDE_STRING_H "#include <string.h>")
ENDIF()

IF(STC_HAVE_CTYPE_H)
  SET(STC_INCLUDE_CTYPE_H "#include <ctype.h>")
ENDIF()

IF(STC_HAVE_SYS_SOCKET_H)
  SET(STC_INCLUDE_SYS_SOCKET_H "#include <sys/socket.h>")
ENDIF()

IF(STC_HAVE_NETINET_IN_H)
  SET(STC_INCLUDE_NETINET_IN_H "#include <netinet/in.h>")
ENDIF()

IF(STC_HAVE_ARPA_INET_H)
  SET(STC_INCLUDE_ARPA_INET_H "#include <arpa/inet.h>")
ENDIF()

IF(STC_HAVE_SYS_WAIT_H)
  SET(STC_INCLUDE_SYS_WAIT_H "#include <sys/wait.h>")
ENDIF()

IF(STC_HAVE_LIMITS_H)
  SET(STC_INCLUDE_LIMITS_H "#include <limits.h>")
ELSEIF(STC_HAVE_SYS_SYSLIMITS_H)
  SET(STC_INCLUDE_LIMITS_H "#include <sys/syslimits.h>")
ENDIF()

IF(STC_HAVE_DIRENT_H)
  SET(STC_INCLUDE_DIRENT_H "#include <dirent.h>")
ENDIF()

IF(STC_HAVE_DL_H)
  SET(STC_INCLUDE_DL_H "#include <dl.h>")
ENDIF()

IF(STC_HAVE_DLFCN_H)
  SET(STC_INCLUDE_DLFCN_H "#include <dlfcn.h>")
ENDIF()

IF(STC_HAVE_TIME_H)
  SET(STC_INCLUDE_TIME_H "#include <time.h>")
ENDIF()

IF(STC_HAVE_SEMAPHORE_H)
  SET(STC_INCLUDE_SEMAPHORE_H "#include <semaphore.h>")
ENDIF()

IF(STC_HAVE_PTHREAD_H)
  SET(STC_INCLUDE_PTHREAD_H "#include <pthread.h>")
ENDIF()

IF(STC_HAVE_ERRNO_H)
  SET(STC_INCLUDE_ERRNO_H "#include <errno.h>")
ENDIF()

IF(STC_HAVE_SYS_IPC_H)
  SET(STC_INCLUDE_SYS_IPC_H "#include <sys/ipc.h>")
ENDIF()

IF(STC_HAVE_SYS_SHM_H)
  SET(STC_INCLUDE_SYS_SHM_H "#include <sys/shm.h>")
ENDIF()

IF(STC_HAVE_PWD_H)
  SET(STC_INCLUDE_PWD_H "#include <pwd.h>")
ENDIF()

IF(STC_HAVE_GRP_H)
  SET(STC_INCLUDE_GRP_H "#include <grp.h>")
ENDIF()

IF(STC_HAVE_UNISTD_H)
  SET(STC_INCLUDE_UNISTD_H "#include <unistd.h>")
ENDIF()

IF(STC_HAVE_POLL_H)
  SET(STC_INCLUDE_POLL_H "#include <poll.h>")
ENDIF()

IF(STC_HAVE_SYS_EPOLL_H)
  SET(STC_INCLUDE_SYS_EPOLL_H "#include <sys/epoll.h>")
ENDIF()

IF(STC_HAVE_SYS_UN_H)
  SET(STC_INCLUDE_SYS_UN_H "#include <sys/un.h>")
ENDIF()

IF(STC_HAVE_NET_IF_H)
  SET(STC_INCLUDE_NET_IF_H "#include <net/if.h>")
ENDIF()

IF(STC_HAVE_MATH_H)
  SET(STC_INCLUDE_MATH_H "#include <math.h>")
ENDIF()

IF(STC_HAVE_SIGNAL_H)
  SET(STC_INCLUDE_SIGNAL_H "#include <signal.h>")
ENDIF()

IF(STC_HAVE_FLOAT_H)
  SET(STC_INCLUDE_FLOAT_H "#include <float.h>")
ENDIF()

IF(STC_HAVE_SETJMP_H)
  SET(STC_INCLUDE_SETJMP_H "#include <setjmp.h>")
ENDIF()

IF(STC_HAVE_INFINIBAND_VERBS_H)
  SET(STC_INCLUDE_INFINIBAND_VERBS_H "#include <infiniband/verbs.h>")
ENDIF()

IF(STC_HAVE_INFINIBAND_RDMACM_H)
  SET(STC_INCLUDE_INFINIBAND_RDMACM_H "#include <rdma/rdma_cma.h>")
  SET(STC_INCLUDE_INFINIBAND_RDMAVERBS_H "#include <rdma/rdma_verbs.h>")
ENDIF()

# Figure out threading library
#
FIND_PACKAGE (Threads)

FUNCTION(STC_CHECK_PTHREAD_ONCE_INIT)
  CHECK_C_COMPILER_FLAG("-Werror" STC_HAVE_WERROR_FLAG)
  IF(NOT STC_HAVE_WERROR_FLAG)
    RETURN()
  ENDIF()
  SET(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Werror")
  CHECK_C_SOURCE_COMPILES("
    #include <pthread.h>
    void foo(void) {}
    int main()
    {
      pthread_once_t once_control = PTHREAD_ONCE_INIT;
      pthread_once(&once_control, foo);
      return 0;
    }"
    STC_HAVE_PTHREAD_ONCE_INIT
  )
  # http://bugs.opensolaris.org/bugdatabase/printableBug.do?bug_id=6611808
  IF(NOT STC_HAVE_PTHREAD_ONCE_INIT)
    CHECK_C_SOURCE_COMPILES("
      #include <pthread.h>
      void foo(void) {}
      int main()
      {
        pthread_once_t once_control = { PTHREAD_ONCE_INIT };
        pthread_once(&once_control, foo);
        return 0;
      }"
      STC_HAVE_ARRAY_PTHREAD_ONCE_INIT
    )
  ENDIF()
  IF(STC_HAVE_PTHREAD_ONCE_INIT)
    SET(STC_PTHREAD_ONCE_INITIALIZER "PTHREAD_ONCE_INIT" PARENT_SCOPE)
  ENDIF()
  IF(STC_HAVE_ARRAY_PTHREAD_ONCE_INIT)
    SET(STC_PTHREAD_ONCE_INITIALIZER "{ PTHREAD_ONCE_INIT }" PARENT_SCOPE)
  ENDIF()
ENDFUNCTION()

IF(CMAKE_USE_PTHREADS_INIT)
  STC_CHECK_PTHREAD_ONCE_INIT()
ENDIF()

MESSAGE( "-- shared library prefix : ${CMAKE_SHARED_LIBRARY_PREFIX}" ) 
MESSAGE( "-- shared library suffix : ${CMAKE_SHARED_LIBRARY_SUFFIX}" ) 

#
# Tests for functions
#
#CHECK_FUNCTION_EXISTS (aiowait STC_HAVE_AIOWAIT)
CHECK_FUNCTION_EXISTS (aio_read STC_HAVE_AIO_READ)
CHECK_FUNCTION_EXISTS (alarm STC_HAVE_ALARM)
SET(STC_HAVE_ALLOCA 1)
CHECK_FUNCTION_EXISTS (backtrace STC_HAVE_BACKTRACE)
CHECK_FUNCTION_EXISTS (backtrace_symbols STC_HAVE_BACKTRACE_SYMBOLS)
CHECK_FUNCTION_EXISTS (backtrace_symbols_fd STC_HAVE_BACKTRACE_SYMBOLS_FD)
CHECK_FUNCTION_EXISTS (printstack STC_HAVE_PRINTSTACK)
CHECK_FUNCTION_EXISTS (bfill STC_HAVE_BFILL)
CHECK_FUNCTION_EXISTS (bmove STC_HAVE_BMOVE)
CHECK_FUNCTION_EXISTS (bsearch STC_HAVE_BSEARCH)
CHECK_FUNCTION_EXISTS (index STC_HAVE_INDEX)
CHECK_FUNCTION_EXISTS (bzero STC_HAVE_BZERO)
CHECK_FUNCTION_EXISTS (chown STC_HAVE_CHOWN)
CHECK_FUNCTION_EXISTS (clock_gettime STC_HAVE_CLOCK_GETTIME)
CHECK_FUNCTION_EXISTS (cuserid STC_HAVE_CUSERID)
CHECK_FUNCTION_EXISTS (directio STC_HAVE_DIRECTIO)
CHECK_FUNCTION_EXISTS (_doprnt STC_HAVE_DOPRNT)
CHECK_FUNCTION_EXISTS (flockfile STC_HAVE_FLOCKFILE)
CHECK_FUNCTION_EXISTS (ftruncate STC_HAVE_FTRUNCATE)
CHECK_FUNCTION_EXISTS (getline STC_HAVE_GETLINE)
CHECK_FUNCTION_EXISTS (compress STC_HAVE_COMPRESS)
CHECK_FUNCTION_EXISTS (crypt STC_HAVE_CRYPT)
CHECK_FUNCTION_EXISTS (dlerror STC_HAVE_DLERROR)
CHECK_FUNCTION_EXISTS (dlopen STC_HAVE_DLOPEN)
CHECK_FUNCTION_EXISTS (fchmod STC_HAVE_FCHMOD)
CHECK_FUNCTION_EXISTS (fcntl STC_HAVE_FCNTL)
CHECK_FUNCTION_EXISTS (fconvert STC_HAVE_FCONVERT)
CHECK_FUNCTION_EXISTS (fdatasync STC_HAVE_FDATASYNC)
CHECK_SYMBOL_EXISTS(fdatasync "unistd.h" STC_HAVE_DECL_FDATASYNC)
CHECK_FUNCTION_EXISTS (fesetround STC_HAVE_FESETROUND)
CHECK_FUNCTION_EXISTS (fpsetmask STC_HAVE_FPSETMASK)
CHECK_FUNCTION_EXISTS (fseeko STC_HAVE_FSEEKO)
CHECK_FUNCTION_EXISTS (fsync STC_HAVE_FSYNC)
CHECK_FUNCTION_EXISTS (getcwd STC_HAVE_GETCWD)
CHECK_FUNCTION_EXISTS (getenv STC_HAVE_GETENV)
CHECK_FUNCTION_EXISTS (getgrnam_r STC_HAVE_GETGRNAM_R)
CHECK_FUNCTION_EXISTS (getgrgid_r STC_HAVE_GETGRGID_R)
CHECK_FUNCTION_EXISTS (gethostbyaddr_r STC_HAVE_GETHOSTBYADDR_R)
CHECK_FUNCTION_EXISTS (gethostbyname_r STC_HAVE_GETHOSTBYNAME_R)
CHECK_FUNCTION_EXISTS (gethrtime STC_HAVE_GETHRTIME)
CHECK_FUNCTION_EXISTS (getnameinfo STC_HAVE_GETNAMEINFO)
CHECK_FUNCTION_EXISTS (getpass STC_HAVE_GETPASS)
CHECK_FUNCTION_EXISTS (getpassphrase STC_HAVE_GETPASSPHRASE)
CHECK_FUNCTION_EXISTS (getpwnam STC_HAVE_GETPWNAM)
CHECK_FUNCTION_EXISTS (getpwnam_r STC_HAVE_GETPWNAM_R)
CHECK_FUNCTION_EXISTS (getpwuid STC_HAVE_GETPWUID)
CHECK_FUNCTION_EXISTS (getpwuid_r STC_HAVE_GETPWUID_R)
CHECK_FUNCTION_EXISTS (getrlimit STC_HAVE_GETRLIMIT)
CHECK_FUNCTION_EXISTS (getrusage STC_HAVE_GETRUSAGE)
CHECK_FUNCTION_EXISTS (gettimeofday STC_HAVE_GETTIMEOFDAY)
CHECK_FUNCTION_EXISTS (getwd STC_HAVE_GETWD)
CHECK_FUNCTION_EXISTS (gmtime_r STC_HAVE_GMTIME_R)
CHECK_FUNCTION_EXISTS (inet_addr STC_HAVE_INET_ADDR)
CHECK_FUNCTION_EXISTS (inet_network STC_HAVE_INET_NETWORK)
CHECK_FUNCTION_EXISTS (initgroups STC_HAVE_INITGROUPS)
CHECK_FUNCTION_EXISTS (issetugid STC_HAVE_ISSETUGID)
CHECK_FUNCTION_EXISTS (iswlower STC_HAVE_ISWLOWER)
CHECK_FUNCTION_EXISTS (iswupper STC_HAVE_ISWUPPER)
CHECK_FUNCTION_EXISTS (iswctype STC_HAVE_ISWCTYPE)
CHECK_FUNCTION_EXISTS (kqueue STC_HAVE_KQUEUE)
CHECK_FUNCTION_EXISTS (ldiv STC_HAVE_LDIV)
CHECK_FUNCTION_EXISTS (llabs STC_HAVE_LLABS)
CHECK_FUNCTION_EXISTS (localtime_r STC_HAVE_LOCALTIME_R)
CHECK_FUNCTION_EXISTS (longjmp STC_HAVE_LONGJMP)
CHECK_FUNCTION_EXISTS (lstat STC_HAVE_LSTAT)
CHECK_FUNCTION_EXISTS (madvise STC_HAVE_MADVISE)
CHECK_FUNCTION_EXISTS (mallinfo STC_HAVE_MALLINFO)
CHECK_FUNCTION_EXISTS (mbrlen STC_HAVE_MBRLEN)
CHECK_FUNCTION_EXISTS (mbscmp STC_HAVE_MBSCMP)
CHECK_FUNCTION_EXISTS (mbsrtowcs STC_HAVE_MBSRTOWCS)
CHECK_FUNCTION_EXISTS (mbrtowc STC_HAVE_MBRTOWC)
CHECK_FUNCTION_EXISTS (memalign STC_HAVE_MEMALIGN)
CHECK_FUNCTION_EXISTS (memcpy STC_HAVE_MEMCPY)
CHECK_FUNCTION_EXISTS (memmove STC_HAVE_MEMMOVE)
CHECK_FUNCTION_EXISTS (mkstemp STC_HAVE_MKSTEMP)
CHECK_FUNCTION_EXISTS (mlock STC_HAVE_MLOCK)
CHECK_FUNCTION_EXISTS (mlockall STC_HAVE_MLOCKALL)
CHECK_FUNCTION_EXISTS (mmap STC_HAVE_MMAP)
CHECK_FUNCTION_EXISTS (mmap64 STC_HAVE_MMAP64)
CHECK_FUNCTION_EXISTS (nl_langinfo STC_HAVE_NL_LANGINFO)
CHECK_FUNCTION_EXISTS (perror STC_HAVE_PERROR)
CHECK_FUNCTION_EXISTS (poll STC_HAVE_POLL)
CHECK_FUNCTION_EXISTS (port_create STC_HAVE_PORT_CREATE)
CHECK_FUNCTION_EXISTS (posix_fallocate STC_HAVE_POSIX_FALLOCATE)
CHECK_FUNCTION_EXISTS (pread STC_HAVE_PREAD)
CHECK_FUNCTION_EXISTS (pthread_attr_create STC_HAVE_PTHREAD_ATTR_CREATE)
CHECK_FUNCTION_EXISTS (pthread_attr_getstacksize STC_HAVE_PTHREAD_ATTR_GETSTACKSIZE)
CHECK_FUNCTION_EXISTS (pthread_attr_setscope STC_HAVE_PTHREAD_ATTR_SETSCOPE)
CHECK_FUNCTION_EXISTS (pthread_attr_setstacksize STC_HAVE_PTHREAD_ATTR_SETSTACKSIZE)
CHECK_FUNCTION_EXISTS (pthread_condattr_create STC_HAVE_PTHREAD_CONDATTR_CREATE)
CHECK_FUNCTION_EXISTS (pthread_condattr_setclock STC_HAVE_PTHREAD_CONDATTR_SETCLOCK)
CHECK_FUNCTION_EXISTS (pthread_init STC_HAVE_PTHREAD_INIT)
CHECK_FUNCTION_EXISTS (pthread_key_delete STC_HAVE_PTHREAD_KEY_DELETE)
CHECK_FUNCTION_EXISTS (pthread_rwlock_rdlock STC_HAVE_PTHREAD_RWLOCK_RDLOCK)
CHECK_FUNCTION_EXISTS (pthread_sigmask STC_HAVE_PTHREAD_SIGMASK)
CHECK_FUNCTION_EXISTS (pthread_threadmask STC_HAVE_PTHREAD_THREADMASK)
CHECK_FUNCTION_EXISTS (pthread_yield_np STC_HAVE_PTHREAD_YIELD_NP)
CHECK_FUNCTION_EXISTS (pthread_yield STC_HAVE_PTHREAD_YIELD)
CHECK_FUNCTION_EXISTS (putenv STC_HAVE_PUTENV)
CHECK_FUNCTION_EXISTS (readdir_r STC_HAVE_READDIR_R)
CHECK_FUNCTION_EXISTS (readlink STC_HAVE_READLINK)
CHECK_FUNCTION_EXISTS (read_real_time STC_HAVE_READ_REAL_TIME)
CHECK_FUNCTION_EXISTS (realpath STC_HAVE_REALPATH)
CHECK_FUNCTION_EXISTS (regcomp STC_HAVE_REGCOMP)
CHECK_FUNCTION_EXISTS (rename STC_HAVE_RENAME)
CHECK_FUNCTION_EXISTS (re_comp STC_HAVE_RE_COMP)
CHECK_FUNCTION_EXISTS (rwlock_init STC_HAVE_RWLOCK_INIT)
CHECK_FUNCTION_EXISTS (sched_yield STC_HAVE_SCHED_YIELD)
CHECK_FUNCTION_EXISTS (setenv STC_HAVE_SETENV)
CHECK_FUNCTION_EXISTS (setlocale STC_HAVE_SETLOCALE)
CHECK_FUNCTION_EXISTS (setfd STC_HAVE_SETFD)
CHECK_FUNCTION_EXISTS (sigaction STC_HAVE_SIGACTION)
CHECK_FUNCTION_EXISTS (sigthreadmask STC_HAVE_SIGTHREADMASK)
CHECK_FUNCTION_EXISTS (sigwait STC_HAVE_SIGWAIT)
CHECK_FUNCTION_EXISTS (sigaddset STC_HAVE_SIGADDSET)
CHECK_FUNCTION_EXISTS (sigemptyset STC_HAVE_SIGEMPTYSET)
CHECK_FUNCTION_EXISTS (sighold STC_HAVE_SIGHOLD) 
CHECK_FUNCTION_EXISTS (sigset STC_HAVE_SIGSET)
CHECK_FUNCTION_EXISTS (sleep STC_HAVE_SLEEP)
CHECK_FUNCTION_EXISTS (snprintf STC_HAVE_SNPRINTF)
CHECK_FUNCTION_EXISTS (stpcpy STC_HAVE_STPCPY)
CHECK_FUNCTION_EXISTS (strcoll STC_HAVE_STRCOLL)
CHECK_FUNCTION_EXISTS (strerror STC_HAVE_STRERROR)
CHECK_FUNCTION_EXISTS (strlcpy STC_HAVE_STRLCPY)
CHECK_FUNCTION_EXISTS (strnlen STC_HAVE_STRNLEN)
CHECK_FUNCTION_EXISTS (strlcat STC_HAVE_STRLCAT)
CHECK_FUNCTION_EXISTS (strsignal STC_HAVE_STRSIGNAL)
CHECK_FUNCTION_EXISTS (fgetln STC_HAVE_FGETLN)
CHECK_FUNCTION_EXISTS (strpbrk STC_HAVE_STRPBRK)
CHECK_FUNCTION_EXISTS (strsep STC_HAVE_STRSEP)
CHECK_FUNCTION_EXISTS (strstr STC_HAVE_STRSTR)
CHECK_FUNCTION_EXISTS (strtok_r STC_HAVE_STRTOK_R)
CHECK_FUNCTION_EXISTS (strtol STC_HAVE_STRTOL)
CHECK_FUNCTION_EXISTS (strtoll STC_HAVE_STRTOLL)
CHECK_FUNCTION_EXISTS (strtoul STC_HAVE_STRTOUL)
CHECK_FUNCTION_EXISTS (strtoull STC_HAVE_STRTOULL)
CHECK_FUNCTION_EXISTS (strtod STC_HAVE_STRTOD)
CHECK_FUNCTION_EXISTS (strtof STC_HAVE_STRTOF)
CHECK_FUNCTION_EXISTS (strcasecmp STC_HAVE_STRCASECMP)
CHECK_FUNCTION_EXISTS (strncasecmp STC_HAVE_STRNCASECMP)
CHECK_FUNCTION_EXISTS (strdup STC_HAVE_STRDUP)
CHECK_FUNCTION_EXISTS (shmat STC_HAVE_SHMAT) 
CHECK_FUNCTION_EXISTS (shmctl STC_HAVE_SHMCTL)
CHECK_FUNCTION_EXISTS (shmdt STC_HAVE_SHMDT)
CHECK_FUNCTION_EXISTS (shmget STC_HAVE_SHMGET)
CHECK_FUNCTION_EXISTS (tell STC_HAVE_TELL)
CHECK_FUNCTION_EXISTS (tempnam STC_HAVE_TEMPNAM)
CHECK_FUNCTION_EXISTS (thr_setconcurrency STC_HAVE_THR_SETCONCURRENCY)
CHECK_FUNCTION_EXISTS (thr_yield STC_HAVE_THR_YIELD)
CHECK_FUNCTION_EXISTS (times STC_HAVE_TIMES)
CHECK_FUNCTION_EXISTS (towlower STC_HAVE_TOWLOWER)
CHECK_FUNCTION_EXISTS (towupper STC_HAVE_TOWUPPER)
CHECK_FUNCTION_EXISTS (unsetenv STC_HAVE_UNSETENV)
CHECK_FUNCTION_EXISTS (vasprintf STC_HAVE_VASPRINTF)
CHECK_FUNCTION_EXISTS (vsnprintf STC_HAVE_VSNPRINTF)
CHECK_FUNCTION_EXISTS (vprintf STC_HAVE_VPRINTF)
CHECK_FUNCTION_EXISTS (valloc STC_HAVE_VALLOC)
CHECK_FUNCTION_EXISTS (wcrtomb STC_HAVE_WCRTOMB)
CHECK_FUNCTION_EXISTS (wcscoll STC_HAVE_WCSCOLL)
CHECK_FUNCTION_EXISTS (wcsdup STC_HAVE_WCSDUP)
CHECK_FUNCTION_EXISTS (wcwidth STC_HAVE_WCWIDTH)
CHECK_FUNCTION_EXISTS (wctype STC_HAVE_WCTYPE)
CHECK_FUNCTION_EXISTS (pthread_setaffinity_np STC_HAVE_CPUSET)
CHECK_FUNCTION_EXISTS (sem_timedwait STC_HAVE_SEM_TIMEDWAIT)
CHECK_FUNCTION_EXISTS (pthread_condattr_setpshared STC_HAVE_PTHREAD_COND_PSHARED)

# This should work on AIX.

CHECK_FUNCTION_EXISTS(ftime STC_HAVE_FTIME)
# This is still a normal call for milliseconds.

CHECK_FUNCTION_EXISTS(time STC_HAVE_TIME)
# We can use time() on Macintosh if there is no ftime().

CHECK_FUNCTION_EXISTS(rdtscll STC_HAVE_RDTSCLL)
# I doubt that we'll ever reach the check for this.


#
# Tests for symbols
#

CHECK_SYMBOL_EXISTS(sys_errlist "stdio.h" STC_HAVE_SYS_ERRLIST)
CHECK_SYMBOL_EXISTS(madvise "sys/mman.h" STC_HAVE_DECL_MADVISE)
CHECK_SYMBOL_EXISTS(tzname "time.h" STC_HAVE_TZNAME)
CHECK_SYMBOL_EXISTS(lrand48 "stdlib.h" STC_HAVE_LRAND48)
CHECK_SYMBOL_EXISTS(getpagesize "unistd.h" STC_HAVE_GETPAGESIZE)
CHECK_SYMBOL_EXISTS(TIOCGWINSZ "sys/ioctl.h" GWINSZ_IN_SYS_IOCTL)
CHECK_SYMBOL_EXISTS(FIONREAD "sys/ioctl.h" FIONREAD_IN_SYS_IOCTL)
CHECK_SYMBOL_EXISTS(TIOCSTAT "sys/ioctl.h" TIOCSTAT_IN_SYS_IOCTL)
CHECK_SYMBOL_EXISTS(gettimeofday "sys/time.h" STC_HAVE_GETTIMEOFDAY)

CHECK_SYMBOL_EXISTS(finite  "math.h" STC_HAVE_FINITE_IN_MATH_H)
IF(STC_HAVE_FINITE_IN_MATH_H)
  SET(STC_HAVE_FINITE TRUE CACHE INTERNAL "")
ELSE()
  CHECK_SYMBOL_EXISTS(finite  "ieeefp.h" STC_HAVE_FINITE)
ENDIF()
CHECK_SYMBOL_EXISTS(log2  math.h STC_HAVE_LOG2)
CHECK_SYMBOL_EXISTS(isnan math.h STC_HAVE_ISNAN)
CHECK_SYMBOL_EXISTS(rint  math.h STC_HAVE_RINT)

# isinf() prototype not found on Solaris
CHECK_C_SOURCE_COMPILES(
"#include  <math.h>
int main() { 
  isinf(0.0); 
  return 0;
}" STC_HAVE_ISINF)


#
# Test for miss-ligned address
#
STRING(TOLOWER ${CMAKE_SYSTEM_PROCESSOR}  processor)
IF(processor MATCHES "86" OR processor MATCHES "amd64" OR processor MATCHES "x64")
  SET( STL_SUPPORT_MISSALIGNED_ADDRESS 1 )
ENDIF()

#
# Test for endianess
#
INCLUDE(TestBigEndian)
IF(APPLE)
  # Cannot run endian test on universal PPC/Intel binaries 
  # would return inconsistent result.
  # config.h.cmake includes a special #ifdef for Darwin
ELSE()
  TEST_BIG_ENDIAN(STL_IS_BIGENDIAN)
ENDIF()

#
# Tests for type sizes (and presence)
#
INCLUDE (CheckTypeSize)
SET(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS}
        -D_LARGEFILE_SOURCE -D_LARGE_FILES -D_FILE_OFFSET_BITS=64
        -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS)
SET(CMAKE_EXTRA_INCLUDE_FILES signal.h)
STC_CHECK_TYPE_SIZE(sigset_t SIGSET_T)
IF(NOT STC_SIZEOF_SIGSET_T)
 SET(sigset_t int)
ENDIF()
STC_CHECK_TYPE_SIZE(mode_t MODE_T)
IF(NOT STC_SIZEOF_MODE_T)
 SET(mode_t int)
ENDIF()

IF(STC_HAVE_CPUSET)
  SET(STL_HAVE_CPUSET 1)
ENDIF()

IF(STC_HAVE_STDINT_H)
  SET(CMAKE_EXTRA_INCLUDE_FILES stdint.h)
ENDIF(STC_HAVE_STDINT_H)

IF(STC_HAVE_INFINIBAND_RDMACM_H)
  SET(SUPPORT_INFINIBAND 1)
ENDIF(STC_HAVE_INFINIBAND_RDMACM_H)

STC_CHECK_TYPE_SIZE("void *" VOIDP)
IF(${CMAKE_BITS} EQUAL "64")
  IF(NOT ${STC_SIZEOF_VOIDP} EQUAL "8")
    MESSAGE( FATAL_ERROR "sizeof(void*) != 8")
  ENDIF()
ELSE()
  IF(NOT ${STC_SIZEOF_VOIDP} EQUAL "4")
    MESSAGE( FATAL_ERROR "sizeof(void*) != 4")
  ENDIF()
ENDIF()
STC_CHECK_TYPE_SIZE(int INT)
STC_CHECK_TYPE_SIZE(long LONG)
STC_CHECK_TYPE_SIZE("long long" LONG_LONG)
STC_CHECK_TYPE_SIZE(size_t SIZE_T)
STC_CHECK_TYPE_SIZE(off_t OFF_T)
CHECK_SYMBOL_EXISTS(O_LARGEFILE "unistd.h;fcntl.h" STC_HAVE_O_LARGEFILE)
IF(STC_HAVE_O_LARGEFILE AND STC_SIZEOF_OFF_T EQUAL "4")
  MESSAGE( FATAL_ERROR "sizeof(off_t) != 8")
ENDIF()
IF(_LARGEFILE64_SOURCE AND STC_SIZEOF_OFF_T EQUAL "4")
  MESSAGE( FATAL_ERROR "sizeof(off_t) != 8")
ENDIF()
STC_CHECK_TYPE_SIZE(ino_t INO_T)

IF(STC_SIZEOF_INT EQUAL "8")
  SET(STC_INT64_LITERAL "#define STL_INT64_C(val) (val)")
  SET(STC_UINT64_LITERAL "#define STL_UINT64_C(val) (val##U)")
  SET(STC_INT64_FMT "#define STL_INT64_FMT \"d\"")
  SET(STC_UINT64_FMT "#define STL_UINT64_FMT \"u\"")
  SET(STC_UINT64_HEX_FMT "#define STL_UINT64_HEX_FMT \"x\"")
  SET(STC_INT64_VALUE "int")
ELSEIF(STC_SIZEOF_LONG EQUAL "8")
  SET(STC_INT64_LITERAL "#define STL_INT64_C(val) (val##L)")
  SET(STC_UINT64_LITERAL "#define STL_UINT64_C(val) (val##UL)")
  SET(STC_INT64_FMT "#define STL_INT64_FMT \"ld\"")
  SET(STC_UINT64_FMT "#define STL_UINT64_FMT \"lu\"")
  SET(STC_UINT64_HEX_FMT "#define STL_UINT64_HEX_FMT \"lx\"")
  SET(STC_INT64_VALUE "long")
ELSEIF(STC_SIZEOF_LONG_LONG EQUAL "8")
  SET(STC_INT64_LITERAL "#define STL_INT64_C(val) (val##LL)")
  SET(STC_UINT64_LITERAL "#define STL_UINT64_C(val) (val##ULL)")
  SET(STC_INT64_FMT "#define STL_INT64_FMT \"lld\"")
  SET(STC_UINT64_FMT "#define STL_UINT64_FMT \"llu\"")
  SET(STC_UINT64_HEX_FMT "#define STL_UINT64_HEX_FMT \"llx\"")
  SET(STC_INT64_VALUE "long long")
ENDIF()

IF(STC_SIZEOF_SIZE_T EQUAL STC_SIZEOF_INT)
  SET(STC_SSIZE_FMT "#define STL_SSIZE_FMT \"d\"")
  SET(STC_SIZE_FMT "#define STL_SIZE_FMT \"u\"")
ELSE()
  SET(STC_SSIZE_FMT "#define STL_SSIZE_FMT \"ld\"")
  SET(STC_SIZE_FMT "#define STL_SIZE_FMT \"lu\"")
ENDIF()

IF(STC_SIZEOF_OFF_T EQUAL STC_SIZEOF_INT)
  SET(STC_OFF_FMT "#define STL_OFF_FMT \"d\"")
ELSE()
  SET(STC_OFF_FMT "#define STL_OFF_FMT \"ld\"")
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  SET(STC_TIMER_TYPE "typedef LONGLONG stlTimer;")
  SET(STC_FILE_TYPE "HANDLE")
  SET(STC_OS_DIR "typedef HANDLE stlDir;")
  SET(STC_OS_SOCKET "typedef SOCKET stlSocket;")
  SET(STC_SOCKET_TYPE "SOCKET")
  SET(STC_OS_TIMEVAL "typedef FILETIME sttTimeVal;")
  SET(STC_OS_EXP_TIME "typedef SYSTEMTIME stlExpTime;")
  SET(STC_OS_DSO_HANDLE "typedef HMODULE stlDsoHandle;")
  SET(STC_OS_DSO_SYM_HANDLE "typedef FARPROC stlDsoSymHandle;")
  SET(STC_SEM_HANDLE "typedef HANDLE stlSemHandle;")
  SET(STC_SHM_HANDLE "typedef HANDLE stlShmHandle;")
  SET(STC_NAMED_SEM_HANDLE "typedef HANDLE stlNamedSemHandle;")
  SET(STC_UNNAMED_SEM_HANDLE "typedef HANDLE stlUnNamedSemHandle;")
  SET(STC_UID "typedef PSID stlUserID;")
  SET(STC_GID "typedef PSID stlGroupID;")
  SET(STC_DEVICE "typedef stlUInt32 stlDeviceID;")
  SET(STC_OFFSET_TYPE "signed long long")
  SET(STC_PROC_HANDLE "DWORD mProcID;")
  SET(STC_SSIZE_TYPE "SSIZE_T")
  SET(STC_THREAD_HANDLE "HANDLE mHandle;")
  SET(STC_THREAD_ATTR "stlSize mStackSize;")
  SET(STC_THREAD_ONCE_INIT "0")
  SET(STC_THREAD_ONCE "LONG mOnce;")
  SET(STC_THREAD_FUNC "__stdcall")
  SET(STC_SOCKET_TIMEOUT_MS "stlInt32 mTimeoutMs;")
  IF(STC_HAVE_POLL)
    SET(STC_SOCKET_CONNECTED "stlInt32 mDisconnected;")
  ELSE()
    SET(STC_SOCKET_CONNECTED "")
  ENDIF()
  SET(STC_PATH_SEPARATOR "#define STL_PATH_SEPARATOR \"\\\\\"")
  SET(STC_PATH_SEPARATOR_CHARACTER "#define STL_PATH_SEPARATOR_CHARACTER '\\\\'")
ELSE()
  IF(STL_LINT)
    SET(STC_TIMER_TYPE "typedef __timer_t stlTimer;")
  ELSE()
    SET(STC_TIMER_TYPE "typedef timer_t stlTimer;")
  ENDIF()
  SET(STC_FILE_TYPE "int")
  SET(STC_OS_DIR "typedef DIR stlDir;")
  SET(STC_OS_SOCKET "typedef stlInt32 stlSocket;")
  SET(STC_SOCKET_TYPE "stlInt32")
  SET(STC_OS_TIMEVAL "typedef struct timeval sttTimeVal;")
  SET(STC_OS_EXP_TIME "typedef struct tm stlExpTime;")
  SET(STC_OFFSET_TYPE "off_t")
  SET(STC_PROC_HANDLE "pid_t mProcID;")
  SET(STC_SSIZE_TYPE "ssize_t")
  SET(STC_THREAD_HANDLE "pthread_t mHandle;")
  SET(STC_THREAD_ATTR "pthread_attr_t mAttribute;")
  SET(STC_THREAD_ONCE_INIT "PTHREAD_ONCE_INIT")
  SET(STC_THREAD_ONCE "pthread_once_t mOnce;")
  SET(STC_THREAD_FUNC "")
  IF(${CMAKE_SYSTEM_NAME} MATCHES "HP-UX")
    SET(STC_INCLUDE_DL_H "#include <dl.h>")
    SET(STC_OS_DSO_HANDLE "typedef shl_t stlDsoHandle;")
	SET(STC_OS_DSO_SYM_HANDLE "typedef void * stlDsoSymHandle;")
  ELSE()
    SET(STC_OS_DSO_HANDLE "typedef void * stlDsoHandle;")
	SET(STC_OS_DSO_SYM_HANDLE "typedef void * stlDsoSymHandle;")
  ENDIF()
  SET(STC_SHM_HANDLE "typedef int stlShmHandle;")
  IF(STC_TARGET_OS_SOLARIS)
    IF(STC_HAVE_PTHREAD_COND_PSHARED)
      SET(STC_SEM_HANDLE "typedef struct stlSemHandle { stlInt64 mCount; pthread_cond_t mCond; pthread_mutex_t mMutex;} stlSemHandle;")
      SET(STC_USE_SHARED_MUTEX 1)
    ELSEIF(STC_HAVE_SEM_TIMEDWAIT)
      SET(STC_SEM_HANDLE "typedef sem_t stlSemHandle;")
      SET(STC_USE_SEMAPHORE 1)
    ELSE()
      MESSAGE( FATAL_ERROR "does not support sem_timedwait or process shared mutext")
    ENDIF()
  ELSE()  
    IF(STC_HAVE_SEM_TIMEDWAIT)
      SET(STC_SEM_HANDLE "typedef sem_t stlSemHandle;")
      SET(STC_USE_SEMAPHORE 1)
    ELSEIF(STC_HAVE_PTHREAD_COND_PSHARED)
      SET(STC_SEM_HANDLE "typedef struct stlSemHandle { stlInt64 mCount; pthread_cond_t mCond; pthread_mutex_t mMutex;} stlSemHandle;")
      SET(STC_USE_SHARED_MUTEX 1)
    ELSE()
      MESSAGE( FATAL_ERROR "does not support sem_timedwait or process shared mutext")
    ENDIF()
  ENDIF()
  SET(STC_NAMED_SEM_HANDLE "typedef sem_t stlNamedSemHandle;")
  SET(STC_UNNAMED_SEM_HANDLE "typedef sem_t stlUnNamedSemHandle;")
  SET(STC_UID "typedef uid_t stlUserID;")
  SET(STC_GID "typedef gid_t stlGroupID;")
  SET(STC_DEVICE "typedef dev_t stlDeviceID;")
  SET(STC_SOCKET_TIMEOUT_MS "")
  IF(STC_HAVE_POLL)
    SET(STC_SOCKET_CONNECTED "stlInt32 mConnected;")
  ELSE()
    SET(STC_SOCKET_CONNECTED "")
  ENDIF()
  SET(STC_PATH_SEPARATOR "#define STL_PATH_SEPARATOR \"/\"")
  SET(STC_PATH_SEPARATOR_CHARACTER "#define STL_PATH_SEPARATOR_CHARACTER '/'")
ENDIF()

IF(STC_HAVE_SYS_EPOLL_H)
  SET(STC_POLLSET_USES_EPOLL 1)
  SET(STC_POLLSET_EPOLL_EVENT_TYPE "typedef struct epoll_event stlEpollEvent; /**< Query pollset */" )
  SET(STC_POLLSET_EPOLL_EVENT "stlEpollEvent * mEpollEvent; /**< Epoll Event */" )
ELSEIF(STC_HAVE_POLL_H)
  SET(STC_POLLSET_USES_POLL 1)
  SET(STC_POLLSET_FD_ARRAY "stlPollFd * mPollFds; /**< PollFd Array */" )
ELSE()
  SET(STC_POLLSET_USES_SELECT 1)
  SET(STC_POLLSET_MAX_FD "stlInt32 mMaxFd; /**< 가장 높은 지시자 */" )
  SET(STC_POLLSET_READ_FD_SET "fd_set mReadSet; /**< 수신을 검사할 소켓 */")
  SET(STC_POLLSET_WRITE_FD_SET "fd_set mWriteSet; /**< 송신을 검사할 소켓 */")
  SET(STC_POLLSET_ERROR_FD_SET "fd_set mExceptSet; /**< 에러를 검사할 소켓 */")
ENDIF()

IF(STC_HAVE_POLL_H)
  SET(STC_POLL_USES_POLL 1)
ELSE()
  SET(STC_POLL_USES_SELECT 1)
ENDIF()

SET(CMAKE_EXTRA_INCLUDE_FILES stdio.h sys/types.h)
STC_CHECK_TYPE_SIZE(off_t OFF_T)
SET(CMAKE_EXTRA_INCLUDE_FILES)
IF(STC_HAVE_SYS_SOCKET_H)
  SET(CMAKE_EXTRA_INCLUDE_FILES sys/socket.h)
ENDIF(STC_HAVE_SYS_SOCKET_H)
STC_CHECK_TYPE_SIZE(socklen_t SOCKLEN_T)
SET(CMAKE_EXTRA_INCLUDE_FILES)

IF(STC_HAVE_IEEEFP_H)
  SET(CMAKE_EXTRA_INCLUDE_FILES ieeefp.h)
  STC_CHECK_TYPE_SIZE(fp_except FP_EXCEPT)
ENDIF()


#
# Code tests
#

CHECK_C_SOURCE_COMPILES("
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif
int main()
{
  getaddrinfo( 0, 0, 0, 0);
  return 0;
}"
STC_HAVE_GETADDRINFO)

CHECK_C_SOURCE_COMPILES("
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif
int main()
{
  select(0,0,0,0,0);
  return 0;
}"
STC_HAVE_SELECT)

#
# Check if timespec has ts_sec and ts_nsec fields
#

CHECK_C_SOURCE_COMPILES("
#include <pthread.h>

int main(int ac, char **av)
{
  struct timespec abstime;
  abstime.ts_sec = time(NULL)+1;
  abstime.ts_nsec = 0;
}
" STC_HAVE_TIMESPEC_TS_SEC)


#
# Check return type of qsort()
#
CHECK_C_SOURCE_COMPILES("
#include <stdlib.h>
#ifdef __cplusplus
extern \"C\"
#endif
void qsort(void *base, size_t nel, size_t width,
  int (*compar) (const void *, const void *));
int main(int ac, char **av) {}
" QSORT_TYPE_IS_VOID)
IF(QSORT_TYPE_IS_VOID)
  SET(RETQSORTTYPE "void")
ELSE(QSORT_TYPE_IS_VOID)
  SET(RETQSORTTYPE "int")
ENDIF(QSORT_TYPE_IS_VOID)

IF(WIN32)
  SET(SOCKET_SIZE_TYPE int)
ELSE()
CHECK_C_SOURCE_COMPILES("
#include <sys/socket.h>
int main(int argc, char **argv)
{
  getsockname(0,0,(socklen_t *) 0);
  return 0; 
}"
STC_HAVE_SOCKET_SIZE_T_AS_socklen_t
FAIL_REGEX "warning: passing argument 3")
IF(STC_HAVE_SOCKET_SIZE_T_AS_socklen_t)
  SET(SOCKET_SIZE_TYPE socklen_t)
ELSE()
  CHECK_C_SOURCE_COMPILES("
  #include <sys/socket.h>
  int main(int argc, char **argv)
  {
    getsockname(0,0,(int *) 0);
    return 0; 
  }"
  STC_HAVE_SOCKET_SIZE_T_AS_int
  FAIL_REGEX "warning: passing argument 3")
  IF(STC_HAVE_SOCKET_SIZE_T_AS_int)
    SET(SOCKET_SIZE_TYPE int)
  ELSE()
    CHECK_C_SOURCE_COMPILES("
    #include <sys/socket.h>
    int main(int argc, char **argv)
    {
      getsockname(0,0,(size_t *) 0);
      return 0; 
    }"
    STC_HAVE_SOCKET_SIZE_T_AS_size_t
    FAIL_REGEX "warning: passing argument 3")
    IF(STC_HAVE_SOCKET_SIZE_T_AS_size_t)
      SET(SOCKET_SIZE_TYPE size_t)
    ELSE()
      SET(SOCKET_SIZE_TYPE int)
    ENDIF()
  ENDIF()
ENDIF()
ENDIF()

CHECK_C_SOURCE_COMPILES("
#include <pthread.h>
int main()
{
  pthread_yield();
  return 0;
}
" STC_HAVE_PTHREAD_YIELD_ZERO_ARG)

#
# Check return type of signal handlers
#
CHECK_C_SOURCE_COMPILES("
#include <signal.h>
#ifdef signal
# undef signal
#endif
#ifdef __cplusplus
extern \"C\" void (*signal (int, void (*)(int)))(int);
#else
void (*signal ()) ();
#endif
int main(int ac, char **av) {}
" SIGNAL_RETURN_TYPE_IS_VOID)
IF(SIGNAL_RETURN_TYPE_IS_VOID)
  SET(RETSIGTYPE void)
  SET(VOID_SIGHANDLER 1)
ELSE(SIGNAL_RETURN_TYPE_IS_VOID)
  SET(RETSIGTYPE int)
ENDIF(SIGNAL_RETURN_TYPE_IS_VOID)


CHECK_INCLUDE_FILES("time.h;sys/time.h" TIME_WITH_SYS_TIME)
CHECK_SYMBOL_EXISTS(O_NONBLOCK "unistd.h;fcntl.h" STC_HAVE_FCNTL_NONBLOCK)
IF(NOT STC_HAVE_FCNTL_NONBLOCK)
 SET(NO_FCNTL_NONBLOCK 1)
ENDIF()

#
# Test for how the C compiler does inline, if at all
#
CHECK_C_SOURCE_COMPILES("
static inline int foo(){return 0;}
int main(int argc, char *argv[]){return 0;}"
                            STC_C_HAS_INLINE)
IF(STC_C_HAS_INLINE)
  CHECK_C_SOURCE_COMPILES("
  static __inline int foo(){return 0;}
  int main(int argc, char *argv[]){return 0;}"
                            STC_C_HAS___INLINE)
  SET(C_INLINE __inline)
ENDIF()

IF(NOT CMAKE_CROSSCOMPILING AND NOT MSVC)
  STRING(TOLOWER ${CMAKE_SYSTEM_PROCESSOR}  processor)
  IF(processor MATCHES "86" OR processor MATCHES "amd64" OR processor MATCHES "x64")
  #Check for x86 PAUSE instruction
  # We have to actually try running the test program, because of a bug
  # in Solaris on x86_64, where it wrongly reports that PAUSE is not
  # supported when trying to run an application.  See
  # http://bugs.opensolaris.org/bugdatabase/printableBug.do?bug_id=6478684
  CHECK_C_SOURCE_RUNS("
  int main()
  { 
    __asm__ __volatile__ (\"pause\"); 
    return 0;
  }"  STC_HAVE_PAUSE_INSTRUCTION)
  ENDIF()
  IF (NOT STC_HAVE_PAUSE_INSTRUCTION)
    CHECK_C_SOURCE_COMPILES("
    int main()
    {
     __asm__ __volatile__ (\"rep; nop\");
     return 0;
    }
   " STC_HAVE_FAKE_PAUSE_INSTRUCTION)
  ENDIF()
ENDIF()
  
CHECK_SYMBOL_EXISTS(tcgetattr "termios.h" STC_HAVE_TCGETATTR 1)

#
# Check type of signal routines (posix, 4.2bsd, 4.1bsd or v7)
#
CHECK_C_SOURCE_COMPILES("
  #include <signal.h>
  int main(int ac, char **av)
  {
    sigset_t ss;
    struct sigaction sa;
    sigemptyset(&ss); sigsuspend(&ss);
    sigaction(SIGINT, &sa, (struct sigaction *) 0);
    sigprocmask(SIG_BLOCK, &ss, (sigset_t *) 0);
  }"
  STC_HAVE_POSIX_SIGNALS)

IF(NOT STC_HAVE_POSIX_SIGNALS)
 CHECK_C_SOURCE_COMPILES("
  #include <signal.h>
  int main(int ac, char **av)
  {
    int mask = sigmask(SIGINT);
    sigsetmask(mask); sigblock(mask); sigpause(mask);
  }"
  STC_HAVE_BSD_SIGNALS)
  IF (NOT STC_HAVE_BSD_SIGNALS)
    CHECK_C_SOURCE_COMPILES("
    #include <signal.h>
    void foo() { }
    int main(int ac, char **av)
    {
      int mask = sigmask(SIGINT);
      sigset(SIGINT, foo); sigrelse(SIGINT);
      sighold(SIGINT); sigpause(SIGINT);
    }"
   STC_HAVE_SVR3_SIGNALS)  
   IF (NOT STC_HAVE_SVR3_SIGNALS)
    SET(STC_HAVE_V7_SIGNALS 1)
   ENDIF(NOT STC_HAVE_SVR3_SIGNALS)
 ENDIF(NOT STC_HAVE_BSD_SIGNALS)
ENDIF(NOT STC_HAVE_POSIX_SIGNALS)

# Assume regular sprintf
SET(SPRINTFS_RETURNS_INT 1)

CHECK_C_SOURCE_COMPILES("
  int main(int argc, char **argv) 
  {
    extern char *__bss_start;
    return __bss_start ? 1 : 0;
  }"
STC_HAVE_BSS_START)

CHECK_C_SOURCE_COMPILES("
    int main()
    {
      extern void __attribute__((weak)) foo(void);
      return 0;
    }"
    STC_HAVE_WEAK_SYMBOL
)


CHECK_C_SOURCE_COMPILES("
    #undef inline
    #if !defined(SCO) && !defined(__osf__) && !defined(_REENTRANT)
    #define _REENTRANT
    #endif
    #include <pthread.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    int main()
    {

       struct hostent *foo =
       gethostbyaddr_r((const char *) 0,
          0, 0, (struct hostent *) 0, (char *) NULL,  0, (int *)0);
       return 0;
    }
  "
  STC_HAVE_SOLARIS_STYLE_GETHOST)

CHECK_C_SOURCE_COMPILES("
    #undef inline
    #if !defined(SCO) && !defined(__osf__) && !defined(_REENTRANT)
    #define _REENTRANT
    #endif
    #include <pthread.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    int main()
    {
       int ret = gethostbyname_r((const char *) 0,
	(struct hostent*) 0, (char*) 0, 0, (struct hostent **) 0, (int *) 0);
      return 0;
    }"
    STC_HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE)

CHECK_C_SOURCE_COMPILES("
    #undef inline
    #if !defined(SCO) && !defined(__osf__) && !defined(_REENTRANT)
    #define _REENTRANT
    #endif
    #include <pthread.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    int main()
    {
      int ret = gethostbyname_r((const char *) 0, (struct hostent*) 0, (struct hostent_data*) 0);
      return 0;
    }"
    STC_HAVE_GETHOSTBYNAME_R_RETURN_INT)


# Use of ALARMs to wakeup on timeout on sockets
#
# This feature makes use of a mutex and is a scalability hog we
# try to avoid using. However we need support for SO_SNDTIMEO and
# SO_RCVTIMEO socket options for this to work. So we will check
# if this feature is supported by a simple TRY_RUN macro. However
# on some OS's there is support for setting those variables but
# they are silently ignored. For those OS's we will not attempt
# to use SO_SNDTIMEO and SO_RCVTIMEO even if it is said to work.
# See Bug#29093 for the problem with SO_SND/RCVTIMEO on HP/UX.
# To use alarm is simple, simply avoid setting anything.

IF(WIN32)
  SET(STC_HAVE_SOCKET_TIMEOUT 1)
ELSEIF(CMAKE_SYSTEM MATCHES "HP-UX")
  SET(STC_HAVE_SOCKET_TIMEOUT 0)
ELSEIF(CMAKE_CROSSCOMPILING)
  SET(STC_HAVE_SOCKET_TIMEOUT 0)
ELSE()
SET(CMAKE_REQUIRED_LIBRARIES ${LIBNSL} ${LIBSOCKET}) 
CHECK_C_SOURCE_RUNS(
"
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <sys/time.h>
 
 int main()
 {    
   int fd = socket(AF_INET, SOCK_STREAM, 0);
   struct timeval tv;
   int ret= 0;
   tv.tv_sec= 2;
   tv.tv_usec= 0;
   ret|= setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
   ret|= setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
   return !!ret;
 }
" STC_HAVE_SOCKET_TIMEOUT)
ENDIF()

SET(NO_ALARM "${STC_HAVE_SOCKET_TIMEOUT}" CACHE BOOL 
   "No need to use alarm to implement socket timeout")
SET(SIGNAL_WITH_VIO_CLOSE "${STC_HAVE_SOCKET_TIMEOUT}")
MARK_AS_ADVANCED(NO_ALARM)


IF(WITH_ATOMIC_OPS STREQUAL "up")
  SET(STC_ATOMIC_MODE_DUMMY 1 CACHE BOOL "Assume single-CPU mode, no concurrency")
ELSEIF(WITH_ATOMIC_OPS STREQUAL "rwlocks")
  SET(STC_ATOMIC_MODE_RWLOCK 1 CACHE BOOL "Use pthread rwlocks for atomic ops")
ELSEIF(WITH_ATOMIC_OPS STREQUAL "smp")
ELSEIF(NOT WITH_ATOMIC_OPS)
  CHECK_C_SOURCE_COMPILES("
  int main()
  {
    int foo= -10; int bar= 10;
    long long int foo64= -10; long long int bar64= 10;
    if (!__sync_fetch_and_add(&foo, bar) || foo)
      return -1;
    bar= __sync_lock_test_and_set(&foo, bar);
    if (bar || foo != 10)
      return -1;
    bar= __sync_val_compare_and_swap(&bar, foo, 15);
    if (bar)
      return -1;
    if (!__sync_fetch_and_add(&foo64, bar64) || foo64)
      return -1;
    bar64= __sync_lock_test_and_set(&foo64, bar64);
    if (bar64 || foo64 != 10)
      return -1;
    bar64= __sync_val_compare_and_swap(&bar64, foo, 15);
    if (bar64)
      return -1;
    return 0;
  }"
  STC_HAVE_GCC_ATOMIC_BUILTINS)
ELSE()
  MESSAGE(FATAL_ERROR "${WITH_ATOMIC_OPS} is not a valid value for WITH_ATOMIC_OPS!")
ENDIF()

SET(WITH_ATOMIC_LOCKS "${WITH_ATOMIC_LOCKS}" CACHE STRING
"Implement atomic operations using pthread rwlocks or atomic CPU
instructions for multi-processor or uniprocessor
configuration. By default gcc built-in sync functions are used,
if available and 'smp' configuration otherwise.")
MARK_AS_ADVANCED(WITH_ATOMIC_LOCKS STC_ATOMIC_MODE_RWLOCK STC_ATOMIC_MODE_DUMMY)

# Check for the Linux epoll interface; epoll* may be available in libc
# but return ENOSYS on a pre-2.6 kernel, so do a run-time check.
CHECK_C_SOURCE_COMPILES("
    #include <sys/epoll.h>
    #include <unistd.h>

    int main()
    {
        return epoll_create(5) == -1;
    }"
    STC_HAVE_EPOLL)

# test for epoll_create1
CHECK_C_SOURCE_COMPILES("
    #include <sys/epoll.h>
    #include <unistd.h>

    int main()
    {
        return epoll_create1(0) == -1;
    }"
    STC_HAVE_EPOLL_CREATE1)

IF(WITH_VALGRIND)
  CHECK_INCLUDE_FILES("valgrind/memcheck.h;valgrind/valgrind.h" 
    STC_HAVE_VALGRIND_HEADERS)
  IF(STC_HAVE_VALGRIND_HEADERS)
    SET(STC_HAVE_VALGRIND 1)
  ENDIF()
ENDIF()

#--------------------------------------------------------------------
# Check for IPv6 support
#--------------------------------------------------------------------
CHECK_INCLUDE_FILE(netinet/in6.h STC_HAVE_NETINET_IN6_H)

IF(UNIX)
  SET(CMAKE_EXTRA_INCLUDE_FILES sys/types.h netinet/in.h sys/socket.h)
  IF(STC_HAVE_NETINET_IN6_H)
    SET(CMAKE_EXTRA_INCLUDE_FILES ${CMAKE_EXTRA_INCLUDE_FILES} netinet/in6.h)
  ENDIF()
  IF(STC_HAVE_SYS_UN_H)
    SET(CMAKE_EXTRA_INCLUDE_FILES ${CMAKE_EXTRA_INCLUDE_FILES} sys/socket.h sys/un.h)
  ENDIF()
ELSEIF(WIN32)
  SET(CMAKE_EXTRA_INCLUDE_FILES ${CMAKE_EXTRA_INCLUDE_FILES} winsock2.h ws2ipdef.h)
ENDIF()

STC_CHECK_STRUCT_SIZE("sockaddr_in6" SOCKADDR_IN6)
STC_CHECK_STRUCT_SIZE("in6_addr" IN6_ADDR)

IF(STC_HAVE_STRUCT_SOCKADDR_IN6 OR STC_HAVE_STRUCT_IN6_ADDR)
  SET(STC_HAVE_IPV6 TRUE CACHE INTERNAL "")
ENDIF()

#--------------------------------------------------------------------
# Check for Unix domain socket support
#--------------------------------------------------------------------
STC_CHECK_STRUCT_SIZE("sockaddr_un" SOCKADDR_UN)

IF(STC_HAVE_STRUCT_SOCKADDR_UN)
  SET(STC_HAVE_UNIXDOMAIN TRUE CACHE INTERNAL "")
ENDIF()

#--------------------------------------------------------------------
# Check for msghdr support
#--------------------------------------------------------------------
STC_CHECK_STRUCT_SIZE("msghdr" MSGHDR)
STC_CHECK_STRUCT_SIZE("cmsghdr" CMSGHDR)
STC_CHECK_STRUCT_SIZE("iovec" IOVEC)

# Check for sockaddr_storage.ss_family
# It is called differently under OS400 and older AIX

CHECK_STRUCT_HAS_MEMBER("struct sockaddr_storage"
 ss_family "${CMAKE_EXTRA_INCLUDE_FILES}" STC_HAVE_SOCKADDR_STORAGE_SS_FAMILY)
IF(NOT STC_HAVE_SOCKADDR_STORAGE_SS_FAMILY)
  CHECK_STRUCT_HAS_MEMBER("struct sockaddr_storage"
  __ss_family "${CMAKE_EXTRA_INCLUDE_FILES}" STC_HAVE_SOCKADDR_STORAGE___SS_FAMILY)
  IF(STC_HAVE_SOCKADDR_STORAGE___SS_FAMILY)
    SET(ss_family __ss_family)
  ENDIF()
ENDIF()

#
# Check if struct sockaddr_in::sin_len is available.
#

CHECK_STRUCT_HAS_MEMBER("struct sockaddr_in" sin_len
  "${CMAKE_EXTRA_INCLUDE_FILES}" STC_HAVE_SOCKADDR_IN_SIN_LEN)

#
# Check if struct sockaddr_in6::sin6_len is available.
#

CHECK_STRUCT_HAS_MEMBER("struct sockaddr_in6" sin6_len
  "${CMAKE_EXTRA_INCLUDE_FILES}" STC_HAVE_SOCKADDR_IN6_SIN6_LEN)

SET(CMAKE_EXTRA_INCLUDE_FILES) 

CHECK_STRUCT_HAS_MEMBER("struct dirent" d_ino "dirent.h"  STC_STRUCT_DIRENT_HAS_D_INO)
CHECK_STRUCT_HAS_MEMBER("struct dirent" d_namlen "dirent.h"  STC_STRUCT_DIRENT_HAS_D_NAMLEN)
CHECK_STRUCT_HAS_MEMBER("struct stat" st_blocks "sys/stat.h"  STC_HAVE_STRUCT_STAT_ST_BLOCKS)
SET(STC_SPRINTF_RETURNS_INT 1)


# We need to make sure we always have an in_addr type, so STL will just
# define it ourselves, if the platform doesn't provide it.
IF(STC_HAVE_IN_ADDR)
  SET(STC_SOCKET_IN_ADDR "")
ELSE()
  SET(STC_SOCKET_IN_ADDR "struct in_addr { stlUInt32 s_addr; };")
ENDIF()
SET(STC_SOCKET_DEFINE_INET_ADDR "typedef struct in_addr stlInAddr4;")


# The specific declaration of inet_addr's ... some platforms fall back
# inet_network (this is not good, but necessary)
IF(STC_HAVE_INET_ADDR)
  SET(STC_SOCKET_INET_ADDR "#define stlInetAddr inet_addr")
ELSEIF(STC_HAVE_INET_NETWORK)
# not generally safe... inet_network() and inet_addr() perform
# different functions */
  SET(STC_SOCKET_INET_ADDR "#define stlInetAddr inet_network")
ELSE()
  SET(STC_SOCKET_INET_ADDR "")
ENDIF()

IF(STC_HAVE_IPV6)
  SET(STC_SOCKADDR_IN6 "struct sockaddr_in6 mIn6;")
  SET(STC_SOCKET_DEFINE_INET6_ADDR "typedef struct in6_addr stlInAddr6;")
  IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET(STC_INCLUDE_WS2TCPIP_H "#include <ws2tcpip.h>")
  ENDIF()
ELSE()
  SET(STC_SOCKADDR_IN6 "")
ENDIF()

IF(STC_HAVE_UNIXDOMAIN)
  SET(STC_SOCKADDR_UN "struct sockaddr_un  mUn;")
ELSE()
  SET(STC_SOCKADDR_UN "")
ENDIF()

IF(STC_HAVE_STRUCT_IOVEC)
  SET(STC_SOCKET_DEFINE_IOVEC "typedef struct iovec stlIoVec;")
ELSE()
  SET(STC_SOCKET_DEFINE_IOVEC "typedef struct iovec
{
    ULONG   iov_len;
    char  * iov_base;
} iovec;

typedef struct iovec stlIoVec;")
ENDIF()

IF(STC_HAVE_STRUCT_MSGHDR)
  SET(STC_SOCKET_DEFINE_MSGHDR "typedef struct msghdr stlMsgHdr;")
ELSE()
  IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET(STC_SOCKET_DEFINE_MSGHDR "typedef struct msghdr
{
    struct sockaddr * msg_name;
    int               msg_namelen;
    iovec           * msg_iov;
    ULONG             msg_iovlen;
    int               msg_controllen;
    char            * msg_control;
    ULONG             msg_flags;
} msghdr;

typedef struct msghdr stlMsgHdr;")
  ELSE()
    SET(STC_SOCKET_DEFINE_MSGHDR "" )
  ENDIF()
ENDIF()

IF(STC_HAVE_STRUCT_CMSGHDR)
  SET(STC_SOCKET_DEFINE_CMSGHDR "typedef struct cmsghdr stlCMsgHdr;")
ELSE()
  SET(STC_SOCKET_DEFINE_CMSGHDR "")
ENDIF()

# ignore validation flag
#SET(STL_IGNORE_VALIDATION  1)

CHECK_C_SOURCE_COMPILES("
  #include <sys/types.h>
  #include <sys/socket.h>
int main()
{
    return socket(AF_INET, SOCK_STREAM|SOCK_CLOEXEC, 0) == -1;
}"
 STC_HAVE_SOCK_CLOEXEC)


CHECK_C_SOURCE_COMPILES("
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETINET_SCTP_H
#include <netinet/sctp.h>
#endif
#ifdef HAVE_NETINET_SCTP_UIO_H
#include <netinet/sctp_uio.h>
#endif
#include <stdlib.h>
int main(void) {
  int s, opt = 1;
  if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP)) < 0)
    exit(1);
  if (setsockopt(s, IPPROTO_SCTP, SCTP_NODELAY, &opt, sizeof(int)) < 0)
    exit(2);
  exit(0);
}"
 STC_HAVE_SCTP)

# GMTOFF
CHECK_STRUCT_HAS_MEMBER("struct tm" tm_gmtoff "time.h"  STC_STRUCT_TM_TM_GMTOFF)
CHECK_STRUCT_HAS_MEMBER("struct tm" __tm_gmtoff "time.h"  STC_STRUCT_TM___TM_GMTOFF)

