CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
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
    ${PRODUCT_LAYER_GOLDILOCKSESQLLIB_DIR}/Doxyfile_GOLDILOCKSESQL_API.in
    ${PRODUCT_LAYER_GOLDILOCKSESQLLIB_DIR}/Doxyfile_GOLDILOCKSESQL_API)

  ADD_CUSTOM_TARGET(Doxygen_GOLDILOCKSESQL_API ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${PRODUCT_LAYER_GOLDILOCKSESQLLIB_DIR}/Doxyfile_GOLDILOCKSESQL_API
    MAIN_DEPENDENCY ${PRODUCT_LAYER_GOLDILOCKSESQLLIB_DIR}/Doxyfile_GOLDILOCKSESQL_API
    WORKING_DIRECTORY ${PRODUCT_LAYER_GOLDILOCKSESQLLIB_DIR}
    )
ENDIF(BUILD_DOCUMENTATION_API)

# include directories
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STANDARD_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_ODBC_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_GOLDILOCKSESQLLIB_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_GOLDILOCKSESQLLIB_DIR}/dev/src/include)

SET( ZLP_DIR ${PRODUCT_LAYER_GOLDILOCKSESQLLIB_DIR} )

# common source files
AUX_SOURCE_DIRECTORY( ${ZLP_DIR}/dev/src/zlpb ZLPB_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZLP_DIR}/dev/src/zlpc ZLPC_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZLP_DIR}/dev/src/zlpd ZLPD_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZLP_DIR}/dev/src/zlpe ZLPE_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZLP_DIR}/dev/src/zlpl ZLPL_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZLP_DIR}/dev/src/zlpn ZLPN_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZLP_DIR}/dev/src/zlpu ZLPU_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZLP_DIR}/dev/src/zlpx ZLPX_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZLP_DIR}/dev/src/zlpy ZLPY_SOURCE_FILES )

IF( ${CMAKE_CPP_COMPILER} )
    SET_SOURCE_FILES_PROPERTIES( ${ZLPB_SOURCE_FILES} PROPERTIES LANGUAGE CXX )
    SET_SOURCE_FILES_PROPERTIES( ${ZLPC_SOURCE_FILES} PROPERTIES LANGUAGE CXX )
    SET_SOURCE_FILES_PROPERTIES( ${ZLPD_SOURCE_FILES} PROPERTIES LANGUAGE CXX )
    SET_SOURCE_FILES_PROPERTIES( ${ZLPE_SOURCE_FILES} PROPERTIES LANGUAGE CXX )
    SET_SOURCE_FILES_PROPERTIES( ${ZLPL_SOURCE_FILES} PROPERTIES LANGUAGE CXX )
    SET_SOURCE_FILES_PROPERTIES( ${ZLPN_SOURCE_FILES} PROPERTIES LANGUAGE CXX )
    SET_SOURCE_FILES_PROPERTIES( ${ZLPU_SOURCE_FILES} PROPERTIES LANGUAGE CXX )
    SET_SOURCE_FILES_PROPERTIES( ${ZLPX_SOURCE_FILES} PROPERTIES LANGUAGE CXX )
    SET_SOURCE_FILES_PROPERTIES( ${ZLPY_SOURCE_FILES} PROPERTIES LANGUAGE CXX )
ENDIF()

SET( LIBRARY_OUTPUT_PATH ${PRODUCT_LAYER_GOLDILOCKSESQLLIB_DIR}/dev/lib )

SET( GOLDILOCKSESQLLIB_SOURCE_FILES ${ZLPB_SOURCE_FILES}
                               ${ZLPC_SOURCE_FILES}
                               ${ZLPD_SOURCE_FILES}
                               ${ZLPE_SOURCE_FILES}
                               ${ZLPL_SOURCE_FILES}
                               ${ZLPN_SOURCE_FILES}
                               ${ZLPU_SOURCE_FILES}
                               ${ZLPX_SOURCE_FILES}
                               ${ZLPY_SOURCE_FILES} )

##################################################
# libgoldilocksesql.a
##################################################
ADD_LIBRARY( goldilocksesql ${GOLDILOCKSESQLLIB_SOURCE_FILES} )

SET_TARGET_PROPERTIES( goldilocksesql PROPERTIES
                                 LIBRARY_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                 LIBRARY_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                 LIBRARY_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                 ARCHIVE_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                 ARCHIVE_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                 ARCHIVE_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                 RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                 RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                 RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB} )

INSTALL( TARGETS             goldilocksesql
         ARCHIVE DESTINATION ${INSTALL_LIBDIR}
         LIBRARY DESTINATION ${INSTALL_LIBDIR}
         RUNTIME DESTINATION ${INSTALL_LIBDIR}
         PERMISSIONS         OWNER_READ OWNER_EXECUTE OWNER_WRITE
                             GROUP_READ GROUP_EXECUTE
                             WORLD_READ WORLD_EXECUTE )

#######################################################
# libgoldilocksesqls.so
#######################################################

ADD_LIBRARY( goldilocksesqls SHARED ${GOLDILOCKSESQLLIB_SOURCE_FILES} )

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  SET (ST_LIBRARY st)
  SET (DT_LIBRARY dt)
  SET (CM_LIBRARY cm)
  SET (ZLC_LIBRARY zlc)

  LINK_DIRECTORIES( ${PRODUCT_LAYER_STANDARD_DIR}/dev/lib
                    ${PRODUCT_LAYER_DATATYPE_DIR}/dev/lib
                    ${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/lib
                    ${PRODUCT_LAYER_ODBC_DIR}/dev/lib )

  TARGET_LINK_LIBRARIES(goldilocksesqls ${SHARED_TARGET_LINK_LIBRARIES}
                                   ${ZLC_LIBRARY}
                                   ${CM_LIBRARY}
                                   ${DT_LIBRARY}
                                   ${ST_LIBRARY})
ELSE()
  TARGET_LINK_LIBRARIES(goldilocksesqls ${SHARED_TARGET_LINK_LIBRARIES})
ENDIF()

SET_TARGET_PROPERTIES( goldilocksesqls PROPERTIES
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
            TARGET goldilocksesqls
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove libgoldilocksesqls.so libgoldilocksesqls.so.${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
            COMMAND WORKING_DIRECTORY ${GLIESE_HOME_LIB} )
  ADD_CUSTOM_COMMAND (
            TARGET goldilocksesqls
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E rename libgoldilocksesqls.so libgoldilocksesqls.so.${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
            COMMAND ${CMAKE_COMMAND} -E create_symlink libgoldilocksesqls.so.${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION} libgoldilocksesqls.so
            COMMAND WORKING_DIRECTORY ${GLIESE_HOME_LIB} )
  INSTALL( DIRECTORY ${GLIESE_HOME_LIB}/  DESTINATION ${INSTALL_LIBDIR} USE_SOURCE_PERMISSIONS PATTERN "libgoldilocksesqls.so.*" )
ELSE()
  INSTALL( TARGETS             goldilocksesqls
           LIBRARY DESTINATION ${INSTALL_LIBDIR}
           RUNTIME DESTINATION ${INSTALL_LIBDIR}
           PERMISSIONS         OWNER_READ OWNER_EXECUTE OWNER_WRITE
                               GROUP_READ GROUP_EXECUTE
                               WORLD_READ WORLD_EXECUTE )
ENDIF()

#######################################################
# Header file copy
#######################################################

ADD_CUSTOM_COMMAND (
        TARGET goldilocksesql
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_GOLDILOCKSESQLLIB_DIR}/dev/include/goldilocksesql.h      ${GLIESE_HOME_INCLUDE}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRODUCT_LAYER_GOLDILOCKSESQLLIB_DIR}/dev/include/sqlca.h          ${GLIESE_HOME_INCLUDE}
        COMMENT "copy goldilocksesql header files" )

##################################################
# PACKAGE( make package )
##################################################

INSTALL( DIRECTORY ${PRODUCT_LAYER_GOLDILOCKSESQLLIB_DIR}/dev/include/ DESTINATION ${INSTALL_INCDIR} FILES_MATCHING PATTERN "goldilocksesql.h" )
INSTALL( DIRECTORY ${PRODUCT_LAYER_GOLDILOCKSESQLLIB_DIR}/dev/include/ DESTINATION ${INSTALL_INCDIR} FILES_MATCHING PATTERN "sqlca.h" )
