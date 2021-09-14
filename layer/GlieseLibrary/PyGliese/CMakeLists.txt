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
    ${PRODUCT_LAYER_PYGLIESELIB_DIR}/Doxyfile_PYGLIESE_API.in
    ${PRODUCT_LAYER_PYGLIESELIB_DIR}/Doxyfile_PYGLIESE_API)

  ADD_CUSTOM_TARGET(Doxygen_PYGLIESE_API ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${PRODUCT_LAYER_PYGLIESELIB_DIR}/Doxyfile_PYGLIESE_API
    MAIN_DEPENDENCY ${PRODUCT_LAYER_PYGLIESELIB_DIR}/Doxyfile_PYGLIESE_API
    WORKING_DIRECTORY ${PRODUCT_LAYER_PYGLIESELIB_DIR}
    )
ENDIF(BUILD_DOCUMENTATION_API)

# check python library
INCLUDE(../FindPythonLibs.cmake)

#set the Gliese Library
SET (GLIESEA_SHARED_LIBRARY goldilocksas)
SET (GLIESECS_SHARED_LIBRARY goldilockscs)
LINK_DIRECTORIES (${GLIESE_HOME_LIB})

# include directories
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STANDARD_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_ODBC_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_PYGLIESELIB_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

# PyGliese source files
AUX_SOURCE_DIRECTORY( dev/src/zlym
                      PYGLIESE_LIBRARY_SOURCE_FILES )


IF( ${CMAKE_CPP_COMPILER} )
    SET_SOURCE_FILES_PROPERTIES( ${PYGLIESE_LIBRARY_SOURCE_FILES} PROPERTIES LANGUAGE CXX )
ENDIF()

SET (LIBRARY_OUTPUT_PATH ${PRODUCT_LAYER_PYGLIESELIB_DIR}/dev/lib)

#######################################################
# compile option 변경
# -D_XOPEN_SOURCE=700 옵션 제거
# Python 자체 header file에서 위의 매크로를 따로 정의하고 있음
#######################################################
STRING(REGEX REPLACE "-D_XOPEN_SOURCE=700" "" TMP_CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
SET(CMAKE_C_FLAGS ${TMP_CMAKE_C_FLAGS})
STRING(REGEX REPLACE "-D_XOPEN_SOURCE=700" "" TMP_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
SET(CMAKE_CXX_FLAGS ${TMP_CMAKE_CXX_FLAGS})

##################################################
# LIBRARY
# pygliese.so
##################################################
ADD_LIBRARY( pygliese SHARED ${PYGLIESE_LIBRARY_SOURCE_FILES} )

TARGET_LINK_LIBRARIES (pygliese ${GLIESEA_SHARED_LIBRARY})

SET_TARGET_PROPERTIES( pygliese  PROPERTIES
                       PREFIX    ""
#                       COMPILE_FLAGS  "-fPIC -Wl,-Bsymbolic-functions"
#                       VERSION        ${PRODUCT_VERSION}.${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
                       LIBRARY_OUTPUT_DIRECTORY ${GLIESE_HOME_LIB} )


##################################################
# PACKAGE( make package )
##################################################

INSTALL( FILES     ${GLIESE_HOME_LIB}/pygliese.so              DESTINATION ${INSTALL_LIBDIR} )
