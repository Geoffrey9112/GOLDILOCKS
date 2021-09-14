CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
# Avoid warnings in higher versions
IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.6)
 CMAKE_POLICY(VERSION 2.8)
ENDIF()

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

# Include the platform-specific file. To allow exceptions, this code
# looks for files in order of how specific they are. If there is, for
# example, a generic Linux.cmake and a version-specific
# Linux-2.6.28-11-generic, it will pick Linux-2.6.28-11-generic and
# include it. It is then up to the file writer to include the generic
# version if necessary.
FOREACH(_base
    ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_VERSION}-${CMAKE_SYSTEM_PROCESSOR}
    ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_VERSION}
    ${CMAKE_SYSTEM_NAME})
  SET(_file ${CMAKE_CURRENT_SOURCE_DIR}/${_base}.cmake)
  IF(EXISTS ${_file})
    INCLUDE(${_file})
    BREAK()
  ENDIF()
ENDFOREACH()

# Searches function in libraries
# if function is found, sets output parameter result to the name of the library
# if function is found in libc, result will be empty 
INCLUDE (CheckLibraryExists)
INCLUDE (CheckFunctionExists)

FUNCTION(STC_SEARCH_LIBS func libs result)
  FOREACH(lib  ${libs})
    CHECK_LIBRARY_EXISTS(${lib} ${func} "" STC_HAVE_${func}_IN_${lib}) 
    IF(STC_HAVE_${func}_IN_${lib})
      SET(${result} ${lib} PARENT_SCOPE)
      SET(STC_HAVE_${result} 1 PARENT_SCOPE)
      RETURN()
    ENDIF()
  ENDFOREACH()
ENDFUNCTION()

# Find out which libraries to use.
IF(UNIX)
  STC_SEARCH_LIBS(fmod m LIBM)
  IF(NOT LIBM)
    STC_SEARCH_LIBS(__infinity m LIBM)
  ENDIF()
  STC_SEARCH_LIBS(pthread_create  pthread LIBPTHREAD)
  STC_SEARCH_LIBS(gethostbyname_r  "nsl_r;nsl" LIBNSL)
  STC_SEARCH_LIBS(bind "bind;socket" LIBBIND)
  STC_SEARCH_LIBS(crypt crypt LIBCRYPT)
  STC_SEARCH_LIBS(setsockopt socket LIBSOCKET)
  STC_SEARCH_LIBS(dlopen dl LIBDL)
  STC_SEARCH_LIBS(_Unwind_Backtrace unwind LIBUNWIND)
  STC_SEARCH_LIBS(sched_yield rt LIBRT)
  STC_SEARCH_LIBS(ibv_create_qp ibverbs LIBIBVERBS)
  STC_SEARCH_LIBS(rdma_create_event_channel rdmacm LIBIBRDMACM)
  IF(NOT LIBRT)
    STC_SEARCH_LIBS(sem_open rt LIBRT)
  ENDIF()
  IF(NOT LIBRT)
    STC_SEARCH_LIBS(clock_gettime rt LIBRT)
  ENDIF()
  IF(NOT LIBRT)
    STC_SEARCH_LIBS(timer_create rt LIBRT)
  ENDIF()
  FIND_PACKAGE(Threads)

  SET(CMAKE_REQUIRED_LIBRARIES 
    ${LIBM} ${LIBNSL} ${LIBBIND} ${LIBCRYPT} ${LIBSOCKET} ${LIBDL} ${LIBPTHREAD} ${LIBRT} ${LIBUNWIND})


  LIST(REMOVE_DUPLICATES CMAKE_REQUIRED_LIBRARIES)
  LINK_LIBRARIES(${CMAKE_REQUIRED_LIBRARIES})
ENDIF()

IF(${CMAKE_CURRENT_SOURCE_DIR}/dev/src/include/stc.h.in  IS_NEWER_THAN ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/include/stc.h OR
   ${CMAKE_CURRENT_SOURCE_DIR}/dev/include/stlTypes.h.in IS_NEWER_THAN ${CMAKE_CURRENT_SOURCE_DIR}/dev/include/stlTypes.h OR
   ${CMAKE_NEED_RECOMPILE} MATCHES "TRUE")

  FILE(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/include/stc.h)
  FILE(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/dev/include/stlTypes.h)

  # Run platform tests
  INCLUDE(configure.cmake)

  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/dev/src/include/stc.h.in
                 ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/include/stc.h)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/dev/include/stlTypes.h.in
                 ${CMAKE_CURRENT_SOURCE_DIR}/dev/include/stlTypes.h)
ENDIF()

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/dev/include/stlProduct.h.in
                ${CMAKE_CURRENT_SOURCE_DIR}/dev/include/stlProduct.h )

# If choose to build documentation, then search for Doxygen executables.
INCLUDE (${CMAKE_ROOT}/Modules/CMakeDependentOption.cmake)
CMAKE_DEPENDENT_OPTION(BUILD_DOCUMENTATION_API 
                      "Build API Documentation with Doxygen" ON
                       BUILD_DOCUMENTATION OFF)

# If choose to build documentation, then search for Doxygen executables.
IF(BUILD_DOCUMENTATION_API)
  FIND_PACKAGE(Doxygen REQUIRED)

  OPTION(DOXYGEN_SHORT_NAMES "Build Standard Layer documentations with short files name" OFF)
  MARK_AS_ADVANCED(DOXYGEN_SHORT_NAMES)
  IF(DOXYGEN_SHORT_NAMES)
    SET(DOXYGEN_SHORT_NAMES YES)
  ELSE(DOXYGEN_SHORT_NAMES)
    SET(DOXYGEN_SHORT_NAMES NO)
  ENDIF(DOXYGEN_SHORT_NAMES)

  #
  # Configure the script and the doxyfile, then add target
  #
  SET(HAVE_DOT_YESNO NO)
  IF(DOT)
    SET(HAVE_DOT_YESNO YES)
    IF(NOT DOT_PATH)
      GET_FILENAME_COMPONENT(DOT_PATH ${DOT} PATH)
    ENDIF(NOT DOT_PATH)
  ENDIF(DOT)

  # API target
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile_Standard_API.in
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile_Standard_API)

  ADD_CUSTOM_TARGET(Doxygen_Standard_API ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile_Standard_API
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile_Standard_API
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
ENDIF(BUILD_DOCUMENTATION_API)

INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STANDARD_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STANDARD_DIR}/dev/include)

# common source files
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stl ST_COMMON_SOURCE_FILES)

# OS specific source files
IF (${CMAKE_SYSTEM_NAME} MATCHES "AIX")
  SET (ST_OS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sta/staBuiltin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stb/stbBacktraceUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stt/sttUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/ste/ste.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sts/stsUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sti/stiEnvUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stx/stxSignalUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stx/stxThreadUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stx/stxProcUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfAccUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfStatUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfSeekUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfCopyUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfReadWriteUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfOpenUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfDupUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfPipeUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfLockUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sth/sthUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sto/stoUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stp/stpUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strSnprintf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strVprintf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strStringsUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strDigest.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strDes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strLocaleUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stu/stuUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnInetNtopUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnInetPtonUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSendRecvUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSockAddrUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSockOptUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSocketsUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stm/stmCommMgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stm/stmCommMgrUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stm/stmPassingFdUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stz/stz.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sty/sty.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stg/stg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stq/stqUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stw/stwUnix.c)
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "HP-UX")
  SET (ST_OS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sta/staBuiltin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stb/stbBacktraceUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/ste/ste.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sts/stsUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sti/stiEnvUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stx/stxSignalUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stx/stxThreadUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stx/stxProcUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stt/sttUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfAccUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfSeekUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfStatUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfCopyUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfReadWriteUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfOpenUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfDupUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfPipeUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfLockUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sth/sthUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sto/stoUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stp/stpUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strSnprintf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strStringsUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strVprintf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strDigest.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strDes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strLocaleUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stu/stuUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnInetNtopUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnInetPtonUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSendRecvUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSockAddrUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSockOptUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSocketsUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stm/stmCommMgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stm/stmCommMgrUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stm/stmPassingFdUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stz/stz.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sty/sty.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stg/stg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stq/stqUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stw/stwUnix.c)
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  SET (ST_OS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sta/staBuiltin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stb/stbBacktraceUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/ste/ste.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sts/stsUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sti/stiEnvUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stx/stxSignalUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stx/stxThreadUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stx/stxProcUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stt/sttUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfAccUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfSeekUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfStatUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfCopyUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfReadWriteUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfOpenUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfDupUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfPipeUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfLockUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sth/sthUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sto/stoUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stp/stpUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strSnprintf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strStringsUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strVprintf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strDigest.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strDes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strLocaleUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stu/stuUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnInetNtopUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnInetPtonUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSendRecvUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSockAddrUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSockOptUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSocketsUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stm/stmCommMgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stm/stmCommMgrUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stm/stmPassingFdUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stz/stz.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sty/sty.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stg/stg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stq/stqUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stw/stwUnix.c)
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
  SET (ST_OS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sta/staBuiltin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stb/stbBacktraceUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/ste/ste.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sts/stsUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sti/stiEnvUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stx/stxSignalUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stx/stxThreadUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stx/stxProcUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stt/sttUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfAccUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfSeekUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfStatUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfCopyUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfReadWriteUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfOpenUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfDupUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfPipeUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfLockUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sth/sthUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sto/stoUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stp/stpUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strSnprintf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strStringsUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strVprintf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strDigest.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strDes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strLocaleUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stu/stuUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnInetNtopUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnInetPtonUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSendRecvUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSockAddrUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSockOptUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSocketsUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stm/stmCommMgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stm/stmCommMgrUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stm/stmPassingFdUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stz/stz.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sty/sty.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stg/stg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stq/stqUnix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stw/stwUnix.c)
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  SET (ST_OS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sta/staBuiltin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stb/stbBacktraceWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/ste/ste.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sts/stsWin.c    
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sti/stiEnvWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stx/stxSignalWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stx/stxProcWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stx/stxThreadWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stt/sttWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfSeekWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfStatWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfCopyWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfReadWriteWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfOpenWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfDupWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfPipeWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stf/stfLockWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sth/sthWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sto/stoWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stp/stpWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strSnprintf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strStringsWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strVprintf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strDigest.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strDes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/str/strLocaleWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stu/stuWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnInetNtopWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnInetPtonWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSendRecvWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSockAddrWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSockOptWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnSocketsWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stm/stmCommMgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stm/stmCommMgrWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stm/stmPassingFdWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stz/stz.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sty/sty.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stg/stg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stq/stqWin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stw/stwWin.c)
ENDIF()

IF( LIBIBRDMACM )
  SET (ST_OS_SOURCE_FILES ${ST_OS_SOURCE_FILES}    
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnIbImpl.c )
  SET( ST_IB_SOURCE_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/stn/stnIbIntf.c )
ENDIF()    

SET (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/dev/lib)

# LIBRARY
SET(ST_SOURCES_FILES ${ST_COMMON_SOURCE_FILES}
                     ${ST_OS_SOURCE_FILES})

IF( ${CMAKE_CPP_COMPILER} )
    SET_SOURCE_FILES_PROPERTIES( ${ST_SOURCES_FILES} PROPERTIES LANGUAGE CXX )
    SET_SOURCE_FILES_PROPERTIES( ${ST_IB_SOURCE_FILES} PROPERTIES LANGUAGE CXX )
ENDIF()

ADD_LIBRARY (st ${ST_SOURCES_FILES})

SET_TARGET_PROPERTIES( st PROPERTIES
                       LIBRARY_OUTPUT_DIRECTORY           ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       LIBRARY_OUTPUT_DIRECTORY_DEBUG     ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       LIBRARY_OUTPUT_DIRECTORY_RELEASE   ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       ARCHIVE_OUTPUT_DIRECTORY           ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       ARCHIVE_OUTPUT_DIRECTORY_DEBUG     ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       ARCHIVE_OUTPUT_DIRECTORY_RELEASE   ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       RUNTIME_OUTPUT_DIRECTORY           ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${CMAKE_CURRENT_BINARY_DIR}/dev/lib )

IF(${CMAKE_TARGET_HOST_TYPE} MATCHES "server")
    IF( LIBIBRDMACM )
      ADD_LIBRARY (stib SHARED ${ST_IB_SOURCE_FILES})
      TARGET_LINK_LIBRARIES(stib ${CMAKE_REQUIRED_LIBRARIES} ${LIBIBVERBS} ${LIBIBRDMACM})
  
      SET_TARGET_PROPERTIES( stib PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${GLIESE_HOME_LIB} )
    ENDIF()
ENDIF()

SET(PRODUCT_LAYER_GLIESEA_SHARED_SOURCES ${PRODUCT_LAYER_GLIESEA_SHARED_SOURCES}
                                         ${ST_SOURCES_FILES}
    PARENT_SCOPE)

SET(PRODUCT_LAYER_CLIENT_SOURCES ${PRODUCT_LAYER_CLIENT_SOURCES}
                                 ${ST_SOURCES_FILES}
    PARENT_SCOPE)

############################################
# UNIT TEST
############################################

# ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/test)

############################################
# PACKAGE( make package )
############################################

IF(${CMAKE_TARGET_HOST_TYPE} MATCHES "server")
    IF( LIBIBRDMACM )
        INSTALL( TARGETS             stib
                 LIBRARY DESTINATION ${INSTALL_LIBDIR}
                 PERMISSIONS         OWNER_READ OWNER_EXECUTE OWNER_WRITE
                 GROUP_READ GROUP_EXECUTE
                 WORLD_READ WORLD_EXECUTE )
    ENDIF()
ENDIF()
