CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

#set the Standard Library
SET (ST_LIBRARY st)
LINK_DIRECTORIES (${PRODUCT_LAYER_STANDARD_DIR}/dev/lib)

#set the DataType Library
SET (DT_LIBRARY dt)
LINK_DIRECTORIES (${PRODUCT_LAYER_DATATYPE_DIR}/dev/lib)

#set the Communication Library
SET (CM_LIBRARY cm)
LINK_DIRECTORIES (${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/lib)

#set the Kernel Library
SET (KN_LIBRARY kn)
LINK_DIRECTORIES (${PRODUCT_LAYER_KERNEL_DIR}/dev/lib)

#set the Server Communication Library
SET (SC_LIBRARY sc)
LINK_DIRECTORIES (${PRODUCT_LAYER_SERVERCOMMUNICATION_DIR}/dev/lib)

#set the Storage Library
SET (SM_LIBRARY sm)
LINK_DIRECTORIES (${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/lib)

#set the Execution Library
SET (EX_LIBRARY el)
LINK_DIRECTORIES (${PRODUCT_LAYER_EXECUTIONLIBRARY_DIR}/dev/lib)

#set the SQL Processor Library
SET (QP_LIBRARY qp)
LINK_DIRECTORIES (${PRODUCT_LAYER_SQLPROCESSOR_DIR}/dev/lib)

#set the Session Library
SET (SS_LIBRARY ss)
LINK_DIRECTORIES (${PRODUCT_LAYER_SESSION_DIR}/dev/lib)

#set the Server Library
SET (SL_LIBRARY sl)
LINK_DIRECTORIES (${PRODUCT_LAYER_SERVERLIBRARY_DIR}/dev/lib)

#set the Gliese Library
SET (ZLA_LIBRARY zla)
LINK_DIRECTORIES (${PRODUCT_LAYER_ODBC_DIR}/dev/lib)

#set the gliesec Library 
SET (ZLC_LIBRARY zlc)
LINK_DIRECTORIES (${PRODUCT_LAYER_ODBC_DIR}/dev/lib)

# include directories
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STANDARD_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_ODBC_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_GLIESETOOL_DIR}/gloader/dev/src/include)

SET( ZTD_DIR ${PRODUCT_LAYER_GLIESETOOL_DIR}/gloader )

AUX_SOURCE_DIRECTORY( ${ZTD_DIR}/dev/src ZTD_SOURCE_FILES )

SET (EXECUTABLE_OUTPUT_PATH ${GLIESE_HOME_BIN})

STL_FLEX_TARGET(ZTD_CMD_SCANNER
                ${ZTD_DIR}/dev/src/ztdCmd.l
                ${ZTD_DIR}/dev/src/ztdCmd.yy.c
                COMPILE_FLAGS "--prefix=ztdCmd")
SET_SOURCE_FILES_PROPERTIES(${STL_FLEX_ZTD_CMD_SCANNER_OUTPUTS} PROPERTIES GENERATED 1)

STL_BISON_TARGET(ZTD_CMD_PARSER
                 ${ZTD_DIR}/dev/src/ztdCmd.y
                 ${ZTD_DIR}/dev/src/ztdCmd.tab.c
                 COMPILE_FLAGS "--name-prefix=ztdCmd --defines=${ZTD_DIR}/dev/src/include/ztdCmd.tab.h")
SET_SOURCE_FILES_PROPERTIES(${STL_BISON_ZTD_CMD_PARSER_OUTPUTS} PROPERTIES GENERATED 1)

ADD_DEFINITIONS( -DYY_NO_INPUT )

SET( GLOADER_SOURCES_FILES ${ZTD_SOURCE_FILES} 
                           ${STL_FLEX_ZTD_CMD_SCANNER_OUTPUTS}
                           ${STL_BISON_ZTD_CMD_PARSER_OUTPUTS} ) 

IF( ${CMAKE_CPP_COMPILER} )
    SET_SOURCE_FILES_PROPERTIES( ${GLOADER_LIBRARY_SOURCE_FILES} PROPERTIES LANGUAGE CXX )
ENDIF()

LIST(REMOVE_DUPLICATES GLOADER_SOURCES_FILES)

IF(${CMAKE_TARGET_HOST_TYPE} MATCHES "server")
ADD_EXECUTABLE( gloader ${GLOADER_SOURCES_FILES})

TARGET_LINK_LIBRARIES (gloader ${ZLA_LIBRARY}
                               ${SL_LIBRARY}
                               ${SS_LIBRARY}
                               ${QP_LIBRARY}
                               ${EX_LIBRARY} 
                               ${SM_LIBRARY} 
                               ${SC_LIBRARY} 
                               ${KN_LIBRARY} 
                               ${CM_LIBRARY} 
                               ${DT_LIBRARY} 
                               ${ST_LIBRARY})

SET_TARGET_PROPERTIES( gloader PROPERTIES
                               RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_BIN}
                               RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_BIN}
                               RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_BIN})
ENDIF()

ADD_EXECUTABLE( gloadernet ${GLOADER_SOURCES_FILES})

TARGET_LINK_LIBRARIES (gloadernet ${ZLC_LIBRARY}
                                  ${CM_LIBRARY} 
                                  ${DT_LIBRARY} 
                                  ${ST_LIBRARY})

SET_TARGET_PROPERTIES( gloadernet PROPERTIES
                                  RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_BIN}
                                  RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_BIN}
                                  RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_BIN})

############################################
# PACKAGE( make package )
############################################

IF(${CMAKE_TARGET_HOST_TYPE} MATCHES "server")
INSTALL( TARGETS gloader    DESTINATION ${INSTALL_BINDIR} )
ENDIF()

INSTALL( TARGETS gloadernet DESTINATION ${INSTALL_BINDIR} )
