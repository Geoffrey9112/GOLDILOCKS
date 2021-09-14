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

  OPTION(DOXYGEN_SHORT_NAMES "Build DataType Layer documentations with short files name" OFF)
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
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile_DataType_API.in
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile_DataType_API)

  ADD_CUSTOM_TARGET(Doxygen_DataType_API ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile_DataType_API
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile_DataType_API
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
ENDIF(BUILD_DOCUMENTATION_API)

INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STANDARD_DIR}/dev/include )
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/src/dts/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/include)

# common source files
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dtl DT_LIBRARY_SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dtd DT_TYPEDEF_SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dtc DT_TYPECAST_SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dts DT_CHARACTERSET_SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dtt DT_CHARACTERCONV_SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dtf DT_FUNCTION_SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dtm DT_COMPARISON_SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dtk DT_KEYRANGE_SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dtb DT_BIND_SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dtz DT_TIMEZONE_SOURCE_FILES)

# character set

# UHC <-> UTF8
SET(DT_CHARACTERSET_UHC_HASH_SOURCES_FILES ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dts/dtsh/dtsUhcToUtf8Hash.c
                                           ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dts/dtsh/dtsUtf8ToUhcHash.c)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  SET(CMAKE_RC_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE}")
  SET(DT_CHARACTERSET_UHC_HASH_SOURCES_FILES ${DT_CHARACTERSET_UHC_HASH_SOURCES_FILES}
                                             ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dts/dtsh/dtsUhcModule.def
                                             ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dts/dtsh/dtsUhcModule.rc)
ENDIF()

# GB18030 <-> UTF8
SET(DT_CHARACTERSET_GB18030_HASH_SOURCES_FILES ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dts/dtsh/dtsGb18030ToUtf8Hash.c
                                               ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dts/dtsh/dtsUtf8ToGb18030Hash.c)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  SET(CMAKE_RC_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE}")
  SET(DT_CHARACTERSET_GB18030_HASH_SOURCES_FILES ${DT_CHARACTERSET_GB18030_HASH_SOURCES_FILES}
                                                 ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dts/dtsh/dtsGb18030Module.def
                                                 ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/dts/dtsh/dtsGb18030Module.rc)
ENDIF()

# LIBRARY
SET(DT_SOURCES_FILES ${DT_LIBRARY_SOURCE_FILES}
                     ${DT_TYPEDEF_SOURCE_FILES}
                     ${DT_TYPECAST_SOURCE_FILES}
                     ${DT_CHARACTERSET_SOURCE_FILES}
                     ${DT_CHARACTERCONV_SOURCE_FILES}
                     ${DT_FUNCTION_SOURCE_FILES}
                     ${DT_COMPARISON_SOURCE_FILES}
                     ${DT_KEYRANGE_SOURCE_FILES} 
                     ${DT_BIND_SOURCE_FILES}
                     ${DT_TIMEZONE_SOURCE_FILES} )

IF( ${CMAKE_CPP_COMPILER} )
    SET_SOURCE_FILES_PROPERTIES( ${DT_SOURCES_FILES}                           PROPERTIES LANGUAGE CXX )
    SET_SOURCE_FILES_PROPERTIES( ${DT_CHARACTERSET_UHC_HASH_SOURCES_FILES}     PROPERTIES LANGUAGE CXX )
    SET_SOURCE_FILES_PROPERTIES( ${DT_CHARACTERSET_GB18030_HASH_SOURCES_FILES} PROPERTIES LANGUAGE CXX )
ENDIF()

IF(${CMAKE_BITS} MATCHES "32")
  ADD_LIBRARY( goldilockscvtUHC_32 SHARED ${DT_CHARACTERSET_UHC_HASH_SOURCES_FILES} )
  SET_TARGET_PROPERTIES( goldilockscvtUHC_32 PROPERTIES
                                        LIBRARY_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        LIBRARY_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        LIBRARY_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB} )

  INSTALL( TARGETS             goldilockscvtUHC_32
           LIBRARY DESTINATION ${INSTALL_LIBDIR}
           RUNTIME DESTINATION ${INSTALL_LIBDIR}
           PERMISSIONS         OWNER_READ OWNER_EXECUTE OWNER_WRITE
                               GROUP_READ GROUP_EXECUTE
                               WORLD_READ WORLD_EXECUTE )

  ADD_LIBRARY( goldilockscvtGB18030_32 SHARED ${DT_CHARACTERSET_GB18030_HASH_SOURCES_FILES} )
  SET_TARGET_PROPERTIES( goldilockscvtGB18030_32 PROPERTIES
                                        LIBRARY_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        LIBRARY_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        LIBRARY_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB} )
  INSTALL( TARGETS             goldilockscvtGB18030_32
           LIBRARY DESTINATION ${INSTALL_LIBDIR}
           RUNTIME DESTINATION ${INSTALL_LIBDIR}
           PERMISSIONS         OWNER_READ OWNER_EXECUTE OWNER_WRITE
                               GROUP_READ GROUP_EXECUTE
                               WORLD_READ WORLD_EXECUTE )
ELSE()
  ADD_LIBRARY( goldilockscvtUHC_64 SHARED ${DT_CHARACTERSET_UHC_HASH_SOURCES_FILES} )
  SET_TARGET_PROPERTIES( goldilockscvtUHC_64 PROPERTIES
                                        LIBRARY_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        LIBRARY_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        LIBRARY_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB} )
  INSTALL( TARGETS             goldilockscvtUHC_64
           LIBRARY DESTINATION ${INSTALL_LIBDIR}
           RUNTIME DESTINATION ${INSTALL_LIBDIR}
           PERMISSIONS         OWNER_READ OWNER_EXECUTE OWNER_WRITE
                               GROUP_READ GROUP_EXECUTE
                               WORLD_READ WORLD_EXECUTE )

  ADD_LIBRARY( goldilockscvtGB18030_64 SHARED ${DT_CHARACTERSET_GB18030_HASH_SOURCES_FILES} )
  SET_TARGET_PROPERTIES( goldilockscvtGB18030_64 PROPERTIES
                                        LIBRARY_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        LIBRARY_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        LIBRARY_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        ARCHIVE_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_LIB}
                                        RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_LIB} )
  INSTALL( TARGETS             goldilockscvtGB18030_64
           LIBRARY DESTINATION ${INSTALL_LIBDIR}
           RUNTIME DESTINATION ${INSTALL_LIBDIR}
           PERMISSIONS         OWNER_READ OWNER_EXECUTE OWNER_WRITE
                               GROUP_READ GROUP_EXECUTE
                               WORLD_READ WORLD_EXECUTE )
ENDIF()

SET (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/dev/lib)
ADD_LIBRARY(dt ${DT_SOURCES_FILES})

SET_TARGET_PROPERTIES( dt PROPERTIES
                       LIBRARY_OUTPUT_DIRECTORY           ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       LIBRARY_OUTPUT_DIRECTORY_DEBUG     ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       LIBRARY_OUTPUT_DIRECTORY_RELEASE   ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       ARCHIVE_OUTPUT_DIRECTORY           ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       ARCHIVE_OUTPUT_DIRECTORY_DEBUG     ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       ARCHIVE_OUTPUT_DIRECTORY_RELEASE   ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       RUNTIME_OUTPUT_DIRECTORY           ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${CMAKE_CURRENT_BINARY_DIR}/dev/lib
                       RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${CMAKE_CURRENT_BINARY_DIR}/dev/lib )

IF(${CMAKE_BITS} MATCHES "32")
  ADD_DEPENDENCIES(dt goldilockscvtUHC_32 goldilockscvtGB18030_32)
ELSE()
  ADD_DEPENDENCIES(dt goldilockscvtUHC_64 goldilockscvtGB18030_64)
ENDIF()

SET(PRODUCT_LAYER_GLIESEA_SHARED_SOURCES ${PRODUCT_LAYER_GLIESEA_SHARED_SOURCES}
                                         ${DT_SOURCES_FILES}
    PARENT_SCOPE)

SET(PRODUCT_LAYER_CLIENT_SOURCES ${PRODUCT_LAYER_CLIENT_SOURCES}
                                 ${DT_SOURCES_FILES}
    PARENT_SCOPE)

