CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
# Avoid warnings in higher versions
IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.6)
 CMAKE_POLICY(VERSION 2.8)
ENDIF()

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
    ${PRODUCT_LAYER_ODBC_DIR}/Doxyfile_ODBC_API.in
    ${PRODUCT_LAYER_ODBC_DIR}/Doxyfile_ODBC_API)

  ADD_CUSTOM_TARGET(Doxygen_ODBC_API ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${PRODUCT_LAYER_ODBC_DIR}/Doxyfile_ODBC_API
    MAIN_DEPENDENCY ${PRODUCT_LAYER_ODBC_DIR}/Doxyfile_ODBC_API
    WORKING_DIRECTORY ${PRODUCT_LAYER_ODBC_DIR}
    )
ENDIF(BUILD_DOCUMENTATION_API)

##################################################
# unix odbc headers
##################################################

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
ELSE()
  FOREACH(_header sql.h sqlext.h sqltypes.h sqlucode.h xa.h )
    IF(NOT EXISTS ${PRODUCT_LAYER_ODBC_DIR}/dev/include/${_header})
      MESSAGE( "-- Copy unix header file : " ${_header})
      CONFIGURE_FILE( ${PRODUCT_LAYER_ODBC_DIR}/dev/include/unixodbc/${_header}
                      ${PRODUCT_LAYER_ODBC_DIR}/dev/include/${_header}
                      @ONLY )
      MESSAGE( "-- Copy unix header file : " ${_header} " - done")
    ENDIF()
  ENDFOREACH()
ENDIF()

##################################################
# Configure
##################################################

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  SET(INCLUDE_WINSOCK2_H "#include <winsock2.h>")
  SET(INCLUDE_WINDOWS_H "#include <windows.h>")
ENDIF()

IF( CMAKE_NEED_RECOMPILE MATCHES "TRUE" )
  UNSET( HAVE_SIZEOF_LONG_INT CACHE )
ENDIF()

CHECK_TYPE_SIZE( long SIZEOF_LONG_INT )

CHECK_TYPE_SIZE( "long long" LONG_LONG )
IF( HAVE_LONG_LONG )
  SET( HAVE_LONG_LONG 1 )
ELSE()
  SET( HAVE_LONG_LONG 0 )
ENDIF()

CONFIGURE_FILE( ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilocks.h.in
                ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilocks.h )

SET( CML_MARSHALL_SMALL_PREFIX "zll" )
SET( CML_MARSHALL_LARGE_PREFIX "ZLL" )

SET( ELL_MARSHALL_SMALL_PREFIX "zll" )
SET( ELL_MARSHALL_LARGE_PREFIX "ZLL" )

SET( SSL_MARSHALL_SMALL_PREFIX "zll" )
SET( SSL_MARSHALL_LARGE_PREFIX "ZLL" )

CONFIGURE_FILE(${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/include/cmlRowStatusDef.h.in
               ${PRODUCT_LAYER_ODBC_DIR}/dev/include/${CML_MARSHALL_SMALL_PREFIX}RowStatusDef.h)

CONFIGURE_FILE(${PRODUCT_LAYER_EXECUTIONLIBRARY_DIR}/dev/include/ellCursorDef.h.in
               ${PRODUCT_LAYER_ODBC_DIR}/dev/include/${ELL_MARSHALL_SMALL_PREFIX}CursorDef.h)

CONFIGURE_FILE(${PRODUCT_LAYER_SESSION_DIR}/dev/include/sslConstDef.h.in
               ${PRODUCT_LAYER_ODBC_DIR}/dev/include/${SSL_MARSHALL_SMALL_PREFIX}ConstDef.h)

##################################################
# Include
##################################################

INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STANDARD_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/include)

IF(${CMAKE_TARGET_HOST_TYPE} MATCHES "server")
  INCLUDE_DIRECTORIES(${PRODUCT_LAYER_KERNEL_DIR}/dev/include)
  INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SERVERCOMMUNICATION_DIR}/dev/include)
  INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/include)
  INCLUDE_DIRECTORIES(${PRODUCT_LAYER_EXECUTIONLIBRARY_DIR}/dev/include)
  INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SQLPROCESSOR_DIR}/dev/include)
  INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SESSION_DIR}/dev/include)
  INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SERVERLIBRARY_DIR}/dev/include)
ENDIF()

INCLUDE_DIRECTORIES(${PRODUCT_LAYER_ODBC_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_ODBC_DIR}/dev/src/include)

##################################################
# Source
##################################################

# source files
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlc  ODBC_DBC_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zld  ODBC_DESC_DIAG_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zle  ODBC_ERROR_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlf  ODBC_INFO_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zli  ODBC_BIND_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zll  ODBC_INTERFACE_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zln  ODBC_ENV_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlo  ODBC_XA_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlp  ODBC_PREPARE_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlr  ODBC_RESULT_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zls  ODBC_STMT_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlt  ODBC_TRANSACTION_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlu  ODBC_SETUP_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlv  ODBC_CONVERT_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlx  ODBC_EXECUTE_FILES)


AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlab ODBC_DA_BOOT_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlac ODBC_DA_DBC_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlad ODBC_DA_DESC_DIAG_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlai ODBC_DA_BIND_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlao ODBC_DA_XA_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlap ODBC_DA_PREPARE_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlar ODBC_DA_RESULT_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlas ODBC_DA_STMT_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlat ODBC_DA_TRANSACTION_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlax ODBC_DA_EXECUTE_FILES)


AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlcc ODBC_CS_DBC_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlcd ODBC_CS_DESC_DIAG_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlci ODBC_CS_BIND_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlco ODBC_CS_XA_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlcp ODBC_CS_PREPARE_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlcr ODBC_CS_RESULT_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlcs ODBC_CS_STMT_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlct ODBC_CS_TRANSACTION_FILES)
AUX_SOURCE_DIRECTORY(${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlcx ODBC_CS_EXECUTE_FILES)

SET(ODBC_SOURCES_FILES ${ODBC_DBC_FILES}
                       ${ODBC_DESC_DIAG_FILES}
                       ${ODBC_ERROR_FILES}
                       ${ODBC_INFO_FILES}
                       ${ODBC_BIND_FILES}
                       ${ODBC_INTERFACE_FILES}
                       ${ODBC_ENV_FILES}
                       ${ODBC_XA_FILES}
                       ${ODBC_PREPARE_FILES}
                       ${ODBC_RESULT_FILES}
                       ${ODBC_STMT_FILES}
                       ${ODBC_CONVERT_FILES}
                       ${ODBC_TRANSACTION_FILES}
                       ${ODBC_EXECUTE_FILES} )

IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  SET(CMAKE_RC_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE}")
  SET(ODBC_SOURCES_FILES ${ODBC_SOURCES_FILES}
                         ${PRODUCT_LAYER_ODBC_DIR}/dev/src/zll/zllModule.def
                         ${PRODUCT_LAYER_ODBC_DIR}/dev/src/zll/zllModule.rc)
ENDIF()
                       
SET(ODBCA_SOURCES_FILES ${ODBC_DA_BOOT_FILES}
                        ${ODBC_DA_DBC_FILES}
                        ${ODBC_DA_DESC_DIAG_FILES}
                        ${ODBC_DA_BIND_FILES}
                        ${ODBC_DA_XA_FILES}
                        ${ODBC_DA_PREPARE_FILES}
                        ${ODBC_DA_RESULT_FILES}
                        ${ODBC_DA_STMT_FILES}
                        ${ODBC_DA_TRANSACTION_FILES}
                        ${ODBC_DA_EXECUTE_FILES} )

SET(ODBCC_SOURCES_FILES ${ODBC_CS_DBC_FILES}
                        ${ODBC_CS_DESC_DIAG_FILES}
                        ${ODBC_CS_BIND_FILES}
                        ${ODBC_CS_XA_FILES}
                        ${ODBC_CS_PREPARE_FILES}
                        ${ODBC_CS_RESULT_FILES}
                        ${ODBC_CS_STMT_FILES}
                        ${ODBC_CS_TRANSACTION_FILES}
                        ${ODBC_CS_EXECUTE_FILES} )

IF( ${CMAKE_CPP_COMPILER} )
    SET_SOURCE_FILES_PROPERTIES( ${ODBC_SOURCES_FILES}  PROPERTIES LANGUAGE CXX )
    SET_SOURCE_FILES_PROPERTIES( ${ODBCA_SOURCES_FILES} PROPERTIES LANGUAGE CXX )
    SET_SOURCE_FILES_PROPERTIES( ${ODBCC_SOURCES_FILES} PROPERTIES LANGUAGE CXX )
ENDIF()

SET(LIBRARY_OUTPUT_PATH ${PRODUCT_LAYER_ODBC_DIR}/dev/lib)

#######################################################
# libzl.a
#######################################################

IF(${CMAKE_TARGET_HOST_TYPE} MATCHES "server")
  ADD_LIBRARY(zl ${ODBC_SOURCES_FILES} ${ODBCA_SOURCES_FILES} ${ODBCC_SOURCES_FILES})

  SET_TARGET_PROPERTIES( zl PROPERTIES
                            COMPILE_FLAGS "-DODBC_ALL" )
ENDIF()

#######################################################
# libgoldilocks.a
# note: 윈도우즈 플랫폼에 호환성이 없을수 있다.
#######################################################

IF(${CMAKE_TARGET_HOST_TYPE} MATCHES "server")
  IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "AIX")
    ADD_CUSTOM_TARGET( libgoldilocks ALL
                       COMMAND ${CMAKE_COMMAND} -E remove ${GLIESE_HOME_LIB}/libgoldilocks.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_STANDARD_DIR}/dev/lib/libst.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_DATATYPE_DIR}/dev/lib/libdt.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/lib/libcm.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_KERNEL_DIR}/dev/lib/libkn.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_SERVERCOMMUNICATION_DIR}/dev/lib/libsc.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/lib/libsm.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_EXECUTIONLIBRARY_DIR}/dev/lib/libel.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_SQLPROCESSOR_DIR}/dev/lib/libqp.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_SESSION_DIR}/dev/lib/libss.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_SERVERLIBRARY_DIR}/dev/lib/libsl.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_ODBC_DIR}/dev/lib/libzl.a
                       COMMAND ${CMAKE_AR} -X64 r ${GLIESE_HOME_LIB}/libgoldilocks.a *.o
                       COMMAND ${CMAKE_COMMAND} -E remove *.o
                       COMMAND ${CMAKE_RANLIB} ${GLIESE_HOME_LIB}/libgoldilocks.a
                       DEPENDS st dt cm kn sc sm el qp ss sl zl
                       COMMENT "generate libgoldilocks" )

    INSTALL( FILES       ${GLIESE_HOME_LIB}/libgoldilocks.a
             DESTINATION ${INSTALL_LIBDIR} )
  ELSE()
    ADD_CUSTOM_TARGET( libgoldilocks ALL
                       COMMAND ${CMAKE_COMMAND} -E remove ${GLIESE_HOME_LIB}/libgoldilocks.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_STANDARD_DIR}/dev/lib/libst.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_DATATYPE_DIR}/dev/lib/libdt.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/lib/libcm.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_KERNEL_DIR}/dev/lib/libkn.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_SERVERCOMMUNICATION_DIR}/dev/lib/libsc.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/lib/libsm.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_EXECUTIONLIBRARY_DIR}/dev/lib/libel.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_SQLPROCESSOR_DIR}/dev/lib/libqp.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_SESSION_DIR}/dev/lib/libss.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_SERVERLIBRARY_DIR}/dev/lib/libsl.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_ODBC_DIR}/dev/lib/libzl.a
                       COMMAND ${CMAKE_AR} r ${GLIESE_HOME_LIB}/libgoldilocks.a *.o
                       COMMAND ${CMAKE_COMMAND} -E remove *.o
                       COMMAND ${CMAKE_RANLIB} ${GLIESE_HOME_LIB}/libgoldilocks.a
                       DEPENDS st dt cm kn sc sm el qp ss sl zl
                       COMMENT "generate libgoldilocks" )

    INSTALL( FILES       ${GLIESE_HOME_LIB}/libgoldilocks.a
             DESTINATION ${INSTALL_LIBDIR} )
  ENDIF()
ENDIF()

#######################################################
# libzla.a
#######################################################

IF(${CMAKE_TARGET_HOST_TYPE} MATCHES "server")
  ADD_LIBRARY(zla ${ODBC_SOURCES_FILES} ${ODBCA_SOURCES_FILES})

  SET_TARGET_PROPERTIES( zla PROPERTIES
                             COMPILE_FLAGS "-DODBC_DA" )
ENDIF()

#######################################################
# libgoldilocksa.a
# note: 윈도우즈 플랫폼에 호환성이 없을수 있다.
#######################################################

IF(${CMAKE_TARGET_HOST_TYPE} MATCHES "server")
  IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "AIX")
    ADD_CUSTOM_TARGET( libgoldilocksa ALL
                       COMMAND ${CMAKE_COMMAND} -E remove ${GLIESE_HOME_LIB}/libgoldilocksa.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_STANDARD_DIR}/dev/lib/libst.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_DATATYPE_DIR}/dev/lib/libdt.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/lib/libcm.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_KERNEL_DIR}/dev/lib/libkn.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_SERVERCOMMUNICATION_DIR}/dev/lib/libsc.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/lib/libsm.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_EXECUTIONLIBRARY_DIR}/dev/lib/libel.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_SQLPROCESSOR_DIR}/dev/lib/libqp.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_SESSION_DIR}/dev/lib/libss.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_SERVERLIBRARY_DIR}/dev/lib/libsl.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_ODBC_DIR}/dev/lib/libzla.a
                       COMMAND ${CMAKE_AR} -X64 r ${GLIESE_HOME_LIB}/libgoldilocksa.a *.o
                       COMMAND ${CMAKE_COMMAND} -E remove *.o
                       COMMAND ${CMAKE_RANLIB} ${GLIESE_HOME_LIB}/libgoldilocksa.a
                       DEPENDS st dt cm kn sc sm el qp ss sl zla
                       WORKING_DIRECTORY ${PRODUCT_LAYER_ODBC_DIR}/dev/lib
                       COMMENT "generate libgoldilocksa" )

    INSTALL( FILES       ${GLIESE_HOME_LIB}/libgoldilocksa.a
             DESTINATION ${INSTALL_LIBDIR} )
  ELSE()
    ADD_CUSTOM_TARGET( libgoldilocksa ALL
                       COMMAND ${CMAKE_COMMAND} -E remove ${GLIESE_HOME_LIB}/libgoldilocksa.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_STANDARD_DIR}/dev/lib/libst.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_DATATYPE_DIR}/dev/lib/libdt.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/lib/libcm.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_KERNEL_DIR}/dev/lib/libkn.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_SERVERCOMMUNICATION_DIR}/dev/lib/libsc.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/lib/libsm.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_EXECUTIONLIBRARY_DIR}/dev/lib/libel.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_SQLPROCESSOR_DIR}/dev/lib/libqp.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_SESSION_DIR}/dev/lib/libss.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_SERVERLIBRARY_DIR}/dev/lib/libsl.a
                       COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_ODBC_DIR}/dev/lib/libzla.a
                       COMMAND ${CMAKE_AR} r ${GLIESE_HOME_LIB}/libgoldilocksa.a *.o
                       COMMAND ${CMAKE_COMMAND} -E remove *.o
                       COMMAND ${CMAKE_RANLIB} ${GLIESE_HOME_LIB}/libgoldilocksa.a
                       DEPENDS st dt cm kn sc sm el qp ss sl zla
                       WORKING_DIRECTORY ${PRODUCT_LAYER_ODBC_DIR}/dev/lib
                       COMMENT "generate libgoldilocksa" )

    INSTALL( FILES       ${GLIESE_HOME_LIB}/libgoldilocksa.a
             DESTINATION ${INSTALL_LIBDIR} )
  ENDIF()
ENDIF()

#######################################################
# libzlc.a
#######################################################

ADD_LIBRARY(zlc ${ODBC_SOURCES_FILES} ${ODBCC_SOURCES_FILES})

SET_TARGET_PROPERTIES( zlc PROPERTIES
                           COMPILE_FLAGS "-DODBC_CS" )

#######################################################
# libgoldilocksc.a
# note: 윈도우즈 플랫폼에 호환성이 없을수 있다.
#######################################################

IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "AIX")
  IF(${CMAKE_BITS} MATCHES "32")
  ELSE()
    ADD_CUSTOM_TARGET( libgoldilocksc ALL
                       COMMAND ${CMAKE_COMMAND} -E remove ${GLIESE_HOME_LIB}/libgoldilocksc.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_STANDARD_DIR}/dev/lib/libst.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_DATATYPE_DIR}/dev/lib/libdt.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/lib/libcm.a
                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_ODBC_DIR}/dev/lib/libzlc.a
                       COMMAND ${CMAKE_AR} -X64 r ${GLIESE_HOME_LIB}/libgoldilocksc.a *.o
                       COMMAND ${CMAKE_COMMAND} -E remove *.o
                       COMMAND ${CMAKE_RANLIB} ${GLIESE_HOME_LIB}/libgoldilocksc.a
                       DEPENDS st dt cm zlc
                       COMMENT "generate libgoldilocksc" )

    INSTALL( FILES       ${GLIESE_HOME_LIB}/libgoldilocksc.a
             DESTINATION ${INSTALL_LIBDIR} )
  ENDIF()

#  IF(${CMAKE_BITS} MATCHES "32")
#    ADD_CUSTOM_TARGET( libgoldilocksc ALL
#                       COMMAND ${CMAKE_COMMAND} -E remove ${GLIESE_HOME_LIB}/libgoldilocksc.a
#                       COMMAND ${CMAKE_AR} -X32 x ${PRODUCT_LAYER_STANDARD_DIR}/dev/lib/libst.a
#                       COMMAND ${CMAKE_AR} -X32 x ${PRODUCT_LAYER_DATATYPE_DIR}/dev/lib/libdt.a
#                       COMMAND ${CMAKE_AR} -X32 x ${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/lib/libcm.a
#                       COMMAND ${CMAKE_AR} -X32 x ${PRODUCT_LAYER_ODBC_DIR}/dev/lib/libzlc.a
#                       COMMAND ${CMAKE_AR} -X32 r ${GLIESE_HOME_LIB}/libgoldilocksc.a *.o
#                       COMMAND ${CMAKE_COMMAND} -E remove *.o
#                       COMMAND ${CMAKE_RANLIB} ${GLIESE_HOME_LIB}/libgoldilocksc.a
#                       DEPENDS st dt cm zlc
#                       COMMENT "generate libgoldilocksc" )
#  ELSE()
#    ADD_CUSTOM_TARGET( libgoldilocksc ALL
#                       COMMAND ${CMAKE_COMMAND} -E remove ${GLIESE_HOME_LIB}/libgoldilocksc.a
#                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_STANDARD_DIR}/dev/lib/libst.a
#                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_DATATYPE_DIR}/dev/lib/libdt.a
#                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/lib/libcm.a
#                       COMMAND ${CMAKE_AR} -X64 x ${PRODUCT_LAYER_ODBC_DIR}/dev/lib/libzlc.a
#                       COMMAND ${CMAKE_AR} -X64 r ${GLIESE_HOME_LIB}/libgoldilocksc.a *.o
#                       COMMAND ${CMAKE_COMMAND} -E remove *.o
#                       COMMAND ${CMAKE_RANLIB} ${GLIESE_HOME_LIB}/libgoldilocksc.a
#                       DEPENDS st dt cm zlc
#                       COMMENT "generate libgoldilocksc" )
#  ENDIF()
ELSE()
  ADD_CUSTOM_TARGET( libgoldilocksc ALL
                     COMMAND ${CMAKE_COMMAND} -E remove ${GLIESE_HOME_LIB}/libgoldilocksc.a
                     COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_STANDARD_DIR}/dev/lib/libst.a
                     COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_DATATYPE_DIR}/dev/lib/libdt.a
                     COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/lib/libcm.a
                     COMMAND ${CMAKE_AR} x ${PRODUCT_LAYER_ODBC_DIR}/dev/lib/libzlc.a
                     COMMAND ${CMAKE_AR} r ${GLIESE_HOME_LIB}/libgoldilocksc.a *.o
                     COMMAND ${CMAKE_COMMAND} -E remove *.o
                     COMMAND ${CMAKE_RANLIB} ${GLIESE_HOME_LIB}/libgoldilocksc.a
                     DEPENDS st dt cm zlc
                     COMMENT "generate libgoldilocksc" )

  INSTALL( FILES       ${GLIESE_HOME_LIB}/libgoldilocksc.a
           DESTINATION ${INSTALL_LIBDIR} )
ENDIF()



#######################################################
# libgoldilockscs.so
#######################################################

IF(${CMAKE_BITS} MATCHES "32")
  IF(${CMAKE_SYSTEM_NAME} MATCHES "AIX")
  ELSE()
    SET(ODBCC_SHARED_SOURCES_FILES ${PRODUCT_LAYER_CLIENT_SOURCES} ${ODBC_SOURCES_FILES} ${ODBCC_SOURCES_FILES})
    ADD_LIBRARY(goldilockscs SHARED ${ODBCC_SHARED_SOURCES_FILES})
    ADD_DEPENDENCIES(goldilockscs goldilockscvtUHC_32 goldilockscvtGB18030_32)
    TARGET_LINK_LIBRARIES(goldilockscs ${SHARED_TARGET_LINK_LIBRARIES})
    SET_TARGET_PROPERTIES( goldilockscs PROPERTIES
                                   COMPILE_FLAGS                      "-DODBC_CS -DODBC_SHARED -D_CRT_SECURE_NO_DEPRECATE"
                                   VERSION                            ${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
                                   LIBRARY_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                   LIBRARY_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                   LIBRARY_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                   ARCHIVE_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                   ARCHIVE_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                   ARCHIVE_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                   RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                   RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                   RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB} )

    IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
      ADD_CUSTOM_COMMAND (
              TARGET goldilockscs
              POST_BUILD
              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilocks.h       ${GLIESE_HOME_INCLUDE}
              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilocksxa.h     ${GLIESE_HOME_INCLUDE}
              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilockstypes.h  ${GLIESE_HOME_INCLUDE}
              COMMENT "copy library header files" )

      INSTALL( TARGETS             goldilockscs
               LIBRARY DESTINATION ${INSTALL_LIBDIR}
               RUNTIME DESTINATION ${INSTALL_LIBDIR}
               PERMISSIONS         OWNER_READ OWNER_EXECUTE OWNER_WRITE
                                   GROUP_READ GROUP_EXECUTE
                                   WORLD_READ WORLD_EXECUTE )
    ELSE()
      ADD_CUSTOM_COMMAND (
              TARGET goldilockscs
              POST_BUILD
              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/sql.h         ${GLIESE_HOME_INCLUDE}
              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/sqlext.h      ${GLIESE_HOME_INCLUDE}
              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/sqltypes.h    ${GLIESE_HOME_INCLUDE}
              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/sqlucode.h    ${GLIESE_HOME_INCLUDE}
              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilocks.h       ${GLIESE_HOME_INCLUDE}
              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilocksxa.h     ${GLIESE_HOME_INCLUDE}
              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/xa.h          ${GLIESE_HOME_INCLUDE}
              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilockstypes.h  ${GLIESE_HOME_INCLUDE}
              COMMENT "copy library header files" )

      INSTALL( TARGETS             goldilockscs
               LIBRARY DESTINATION ${INSTALL_LIBDIR}
               RUNTIME DESTINATION ${INSTALL_LIBDIR}
               PERMISSIONS         OWNER_READ OWNER_EXECUTE OWNER_WRITE
                                   GROUP_READ GROUP_EXECUTE
                                   WORLD_READ WORLD_EXECUTE )
    ENDIF()
  ENDIF()
ELSE()
  SET(ODBCC_SHARED_SOURCES_FILES ${PRODUCT_LAYER_CLIENT_SOURCES} ${ODBC_SOURCES_FILES} ${ODBCC_SOURCES_FILES})
  ADD_LIBRARY(goldilockscs-ul64 SHARED ${ODBCC_SHARED_SOURCES_FILES})
  ADD_DEPENDENCIES(goldilockscs-ul64 goldilockscvtUHC_64 goldilockscvtGB18030_64)
  TARGET_LINK_LIBRARIES(goldilockscs-ul64 ${SHARED_TARGET_LINK_LIBRARIES})
  SET_TARGET_PROPERTIES( goldilockscs-ul64 PROPERTIES
                                      COMPILE_FLAGS                      "-DODBC_CS -DODBC_SHARED -D_CRT_SECURE_NO_DEPRECATE"
                                      VERSION                            ${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
                                      LIBRARY_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                      LIBRARY_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                      LIBRARY_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                      ARCHIVE_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                      RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                      RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                      RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB} )

  IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    ADD_CUSTOM_COMMAND (
            TARGET goldilockscs-ul64
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilocks.h       ${GLIESE_HOME_INCLUDE}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilocksxa.h     ${GLIESE_HOME_INCLUDE}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilockstypes.h  ${GLIESE_HOME_INCLUDE}
            COMMENT "copy library header files" )
  ELSE()
    ADD_CUSTOM_COMMAND (
            TARGET goldilockscs-ul64
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/sql.h         ${GLIESE_HOME_INCLUDE}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/sqlext.h      ${GLIESE_HOME_INCLUDE}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/sqltypes.h    ${GLIESE_HOME_INCLUDE}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/sqlucode.h    ${GLIESE_HOME_INCLUDE}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilocks.h       ${GLIESE_HOME_INCLUDE}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilocksxa.h     ${GLIESE_HOME_INCLUDE}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/xa.h          ${GLIESE_HOME_INCLUDE}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilockstypes.h  ${GLIESE_HOME_INCLUDE}
            COMMENT "copy library header files" )
  ENDIF()

  IF(${CMAKE_SYSTEM_NAME} MATCHES "AIX")
    ADD_CUSTOM_COMMAND (
              TARGET goldilockscs-ul64
              PRE_BUILD
              COMMAND ${CMAKE_COMMAND} -E remove libgoldilockscs-ul64.so libgoldilockscs-ul64.so.${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
              COMMAND WORKING_DIRECTORY ${GLIESE_HOME_LIB} )
    ADD_CUSTOM_COMMAND (
              TARGET goldilockscs-ul64
              POST_BUILD
              COMMAND ${CMAKE_COMMAND} -E rename libgoldilockscs-ul64.so libgoldilockscs-ul64.so.${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
              COMMAND ${CMAKE_COMMAND} -E create_symlink libgoldilockscs-ul64.so.${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION} libgoldilockscs-ul64.so
              COMMAND WORKING_DIRECTORY ${GLIESE_HOME_LIB} )
    INSTALL( DIRECTORY ${GLIESE_HOME_LIB}/  DESTINATION ${INSTALL_LIBDIR} USE_SOURCE_PERMISSIONS PATTERN "libgoldilockscs-ul64.so.*" )
  ELSE()
    INSTALL( TARGETS             goldilockscs-ul64
             LIBRARY DESTINATION ${INSTALL_LIBDIR}
             RUNTIME DESTINATION ${INSTALL_LIBDIR}
             PERMISSIONS         OWNER_READ OWNER_EXECUTE OWNER_WRITE
                                 GROUP_READ GROUP_EXECUTE
                                 WORLD_READ WORLD_EXECUTE )
  ENDIF()

  IF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    ADD_LIBRARY(goldilockscs-ul32 SHARED ${ODBCC_SHARED_SOURCES_FILES})
    ADD_DEPENDENCIES(goldilockscs-ul32 goldilockscvtUHC_32 goldilockscvtGB18030_32)
    TARGET_LINK_LIBRARIES(goldilockscs-ul32 ${SHARED_TARGET_LINK_LIBRARIES})
    SET_TARGET_PROPERTIES( goldilockscs-ul32 PROPERTIES
                                        COMPILE_FLAGS                      "-DODBC_CS -DODBC_SHARED -D_CRT_SECURE_NO_DEPRECATE -DBUILD_LEGACY_64_BIT_MODE"
                                        VERSION                            ${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
                                        LIBRARY_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        LIBRARY_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        LIBRARY_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB} )

    ADD_CUSTOM_COMMAND ( TARGET goldilockscs-ul32
                         POST_BUILD
                         COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/sql.h         ${GLIESE_HOME_INCLUDE}
                         COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/sqlext.h      ${GLIESE_HOME_INCLUDE}
                         COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/sqltypes.h    ${GLIESE_HOME_INCLUDE}
                         COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/sqlucode.h    ${GLIESE_HOME_INCLUDE}
                         COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilocks.h       ${GLIESE_HOME_INCLUDE}
                         COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilocksxa.h     ${GLIESE_HOME_INCLUDE}
                         COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/xa.h          ${GLIESE_HOME_INCLUDE}
                         COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_ODBC_DIR}/dev/include/goldilockstypes.h  ${GLIESE_HOME_INCLUDE}
                         COMMENT "copy library header files" )

    INSTALL( TARGETS             goldilockscs-ul32
             LIBRARY DESTINATION ${INSTALL_LIBDIR}
             RUNTIME DESTINATION ${INSTALL_LIBDIR}
             PERMISSIONS         OWNER_READ OWNER_EXECUTE OWNER_WRITE
                                 GROUP_READ GROUP_EXECUTE
                                 WORLD_READ WORLD_EXECUTE )
  ENDIF()
ENDIF()

#######################################################
# libgoldilockss.so
#######################################################

FOREACH(GENERATED_FLEX_FILE ${GENERATED_FLEX_FILES})
    SET_SOURCE_FILES_PROPERTIES( ${GENERATED_FLEX_FILE} PROPERTIES GENERATED TRUE)
ENDFOREACH(GENERATED_FLEX_FILE)

FOREACH(GENERATED_BISION_FILE ${GENERATED_BISON_FILES})
    SET_SOURCE_FILES_PROPERTIES( ${GENERATED_BISION_FILE} PROPERTIES GENERATED TRUE)
ENDFOREACH(GENERATED_BISION_FILE)

INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STANDARD_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STANDARD_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/src/dts/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_KERNEL_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_KERNEL_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SERVERCOMMUNICATION_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SERVERCOMMUNICATION_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/smf/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/smo/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/smp/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/smt/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/smg/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/smr/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/smx/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/sms/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/sma/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/smd/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/smn/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/smk/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/sme/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/smq/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/smb/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/smf/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_EXECUTIONLIBRARY_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_EXECUTIONLIBRARY_DIR}/dev/src/eld/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_EXECUTIONLIBRARY_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SQLPROCESSOR_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SQLPROCESSOR_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SESSION_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SESSION_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SERVERLIBRARY_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SERVERLIBRARY_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_ODBC_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_ODBC_DIR}/dev/src/include)

IF(${CMAKE_TARGET_HOST_TYPE} MATCHES "server")
  ADD_LIBRARY( goldilockss SHARED ${PRODUCT_LAYER_GLIESEA_SHARED_SOURCES}
                             ${ODBC_SOURCES_FILES}
                             ${ODBCA_SOURCES_FILES}
                             ${ODBCC_SOURCES_FILES} )
  ADD_DEPENDENCIES(goldilockss goldilockscvtUHC_64 goldilockscvtGB18030_64)

  TARGET_LINK_LIBRARIES(goldilockss ${SHARED_TARGET_LINK_LIBRARIES})

  SET_TARGET_PROPERTIES( goldilockss PROPERTIES
                                COMPILE_FLAGS                      "-DODBC_ALL -DODBC_SHARED"
                                VERSION                            ${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
                                LIBRARY_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                LIBRARY_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                LIBRARY_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                ARCHIVE_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                ARCHIVE_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                ARCHIVE_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB} )

  IF(${CMAKE_SYSTEM_NAME} MATCHES "AIX")
    ADD_CUSTOM_COMMAND (
              TARGET goldilockss
              PRE_BUILD
              COMMAND ${CMAKE_COMMAND} -E remove libgoldilockss.so libgoldilockss.so.${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
              COMMAND WORKING_DIRECTORY ${GLIESE_HOME_LIB} )
    ADD_CUSTOM_COMMAND (
              TARGET goldilockss
              POST_BUILD
              COMMAND ${CMAKE_COMMAND} -E rename libgoldilockss.so libgoldilockss.so.${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
              COMMAND ${CMAKE_COMMAND} -E create_symlink libgoldilockss.so.${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION} libgoldilockss.so
              COMMAND WORKING_DIRECTORY ${GLIESE_HOME_LIB} )
    INSTALL( DIRECTORY ${GLIESE_HOME_LIB}/  DESTINATION ${INSTALL_LIBDIR} USE_SOURCE_PERMISSIONS PATTERN "libgoldilockss.so.*" )
  ELSE()
    INSTALL( TARGETS             goldilockss
             LIBRARY DESTINATION ${INSTALL_LIBDIR}
             RUNTIME DESTINATION ${INSTALL_LIBDIR}
             PERMISSIONS         OWNER_READ OWNER_EXECUTE OWNER_WRITE
                                 GROUP_READ GROUP_EXECUTE
                                 WORLD_READ WORLD_EXECUTE )
  ENDIF()
ENDIF()

#######################################################
# libgoldilocksas.so
#######################################################

IF(${CMAKE_TARGET_HOST_TYPE} MATCHES "server")
  ADD_LIBRARY( goldilocksas SHARED ${PRODUCT_LAYER_GLIESEA_SHARED_SOURCES}
                              ${ODBC_SOURCES_FILES}
                              ${ODBCA_SOURCES_FILES} )
  ADD_DEPENDENCIES(goldilocksas goldilockscvtUHC_64 goldilockscvtGB18030_64)

  TARGET_LINK_LIBRARIES(goldilocksas ${SHARED_TARGET_LINK_LIBRARIES})

  SET_TARGET_PROPERTIES( goldilocksas PROPERTIES
                                 COMPILE_FLAGS                      "-DODBC_DA -DODBC_SHARED"
                                 VERSION                            ${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
                                 LIBRARY_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                 LIBRARY_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                 LIBRARY_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                 ARCHIVE_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                 ARCHIVE_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                 ARCHIVE_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                 RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                 RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                 RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB} )

  IF(${CMAKE_SYSTEM_NAME} MATCHES "AIX")
    ADD_CUSTOM_COMMAND (
              TARGET goldilocksas
              PRE_BUILD
              COMMAND ${CMAKE_COMMAND} -E remove libgoldilocksas.so libgoldilocksas.so.${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
              COMMAND WORKING_DIRECTORY ${GLIESE_HOME_LIB} )
    ADD_CUSTOM_COMMAND (
              TARGET goldilocksas
              POST_BUILD
              COMMAND ${CMAKE_COMMAND} -E rename libgoldilocksas.so libgoldilocksas.so.${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
              COMMAND ${CMAKE_COMMAND} -E create_symlink libgoldilocksas.so.${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION} libgoldilocksas.so
              COMMAND WORKING_DIRECTORY ${GLIESE_HOME_LIB} )
    INSTALL( DIRECTORY ${GLIESE_HOME_LIB}/  DESTINATION ${INSTALL_LIBDIR} USE_SOURCE_PERMISSIONS PATTERN "libgoldilocksas.so.*" )
  ELSE()
    INSTALL( TARGETS             goldilocksas
             LIBRARY DESTINATION ${INSTALL_LIBDIR}
             RUNTIME DESTINATION ${INSTALL_LIBDIR}
             PERMISSIONS         OWNER_READ OWNER_EXECUTE OWNER_WRITE
                                 GROUP_READ GROUP_EXECUTE
                                 WORLD_READ WORLD_EXECUTE )
  ENDIF()
ENDIF()

#######################################################
# goldilockssetup32.dll / goldilockssetup64.dll
#######################################################

IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  SET(CMAKE_RC_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE}")
  SET(ODBC_SETUP_SOURCES_FILES ${ODBC_SETUP_FILES}
                               ${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlu/zluModule.def
                               ${PRODUCT_LAYER_ODBC_DIR}/dev/src/zlu/zluModule.rc)
							   
  IF( ${CMAKE_CPP_COMPILER} )
    SET_SOURCE_FILES_PROPERTIES( ${ODBC_SETUP_SOURCES_FILES} PROPERTIES LANGUAGE CXX )
  ENDIF()

  IF(${CMAKE_BITS} MATCHES "32")
    ADD_LIBRARY(goldilockssetup32 SHARED ${ODBC_SETUP_SOURCES_FILES})
    TARGET_LINK_LIBRARIES(goldilockssetup32 ${SHARED_TARGET_LINK_LIBRARIES} odbccp32 st)
    SET_TARGET_PROPERTIES( goldilockssetup32 PROPERTIES
                                        COMPILE_FLAGS                      "-D_CRT_SECURE_NO_DEPRECATE"
                                        LINKER_LANGUAGE                    "CXX"
                                        VERSION                            ${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
                                        LIBRARY_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        LIBRARY_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        LIBRARY_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB} )
									  
     INSTALL( TARGETS             goldilockssetup32
              LIBRARY DESTINATION ${INSTALL_LIBDIR}
              RUNTIME DESTINATION ${INSTALL_LIBDIR}
              PERMISSIONS         OWNER_READ OWNER_EXECUTE OWNER_WRITE
                                  GROUP_READ GROUP_EXECUTE
                                  WORLD_READ WORLD_EXECUTE )
  ELSE()
    ADD_LIBRARY(goldilockssetup64 SHARED ${ODBC_SETUP_SOURCES_FILES})
    TARGET_LINK_LIBRARIES(goldilockssetup64 ${SHARED_TARGET_LINK_LIBRARIES} odbccp32 st)
    SET_TARGET_PROPERTIES( goldilockssetup64 PROPERTIES
                                        COMPILE_FLAGS                      "-D_CRT_SECURE_NO_DEPRECATE"
                                        LINKER_LANGUAGE                    "CXX"
                                        VERSION                            ${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
                                        LIBRARY_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        LIBRARY_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        LIBRARY_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB} )
									  
     INSTALL( TARGETS             goldilockssetup64
              LIBRARY DESTINATION ${INSTALL_LIBDIR}
              RUNTIME DESTINATION ${INSTALL_LIBDIR}
              PERMISSIONS         OWNER_READ OWNER_EXECUTE OWNER_WRITE
                                  GROUP_READ GROUP_EXECUTE
                                  WORLD_READ WORLD_EXECUTE )
  ENDIF()
ENDIF()

##################################################
# PACKAGE( make package )
##################################################

IF (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  INSTALL( DIRECTORY ${GLIESE_HOME_INCLUDE}/  DESTINATION ${INSTALL_INCDIR} FILES_MATCHING PATTERN "sql*.h" )
  INSTALL( FILES ${GLIESE_HOME_INCLUDE}/xa.h  DESTINATION ${INSTALL_INCDIR}  )
ENDIF()
INSTALL( FILES ${GLIESE_HOME_INCLUDE}/goldilocks.h      DESTINATION ${INSTALL_INCDIR} )
INSTALL( FILES ${GLIESE_HOME_INCLUDE}/goldilockstypes.h DESTINATION ${INSTALL_INCDIR} )
INSTALL( FILES ${GLIESE_HOME_INCLUDE}/goldilocksxa.h    DESTINATION ${INSTALL_INCDIR} )
