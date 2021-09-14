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
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_GLIESETOOL_DIR}/gsql/dev/src/include)

SET( ZTQ_DIR ${PRODUCT_LAYER_GLIESETOOL_DIR}/gsql )

AUX_SOURCE_DIRECTORY( ${ZTQ_DIR}/dev/src ZTQ_SOURCE_FILES )

SET (EXECUTABLE_OUTPUT_PATH ${GLIESE_HOME_BIN})

STL_FLEX_TARGET(ZTQ_CMD_SCANNER
                ${ZTQ_DIR}/dev/src/ztqCmd.l
                ${ZTQ_DIR}/dev/src/ztqCmd.yy.c
                COMPILE_FLAGS "--prefix=ztqCmd")
SET_SOURCE_FILES_PROPERTIES(${STL_FLEX_ZTQ_CMD_SCANNER_OUTPUTS} PROPERTIES GENERATED 1)

STL_BISON_TARGET(ZTQ_CMD_PARSER
                 ${ZTQ_DIR}/dev/src/ztqCmd.y
                 ${ZTQ_DIR}/dev/src/ztqCmd.tab.c
                 COMPILE_FLAGS "--name-prefix=ztqCmd --defines=${ZTQ_DIR}/dev/src/include/ztqCmd.tab.h")
SET_SOURCE_FILES_PROPERTIES(${STL_BISON_ZTQ_CMD_PARSER_OUTPUTS} PROPERTIES GENERATED 1)

# -----------------------------------------------------
# Parser의 Prefix
# -----------------------------------------------------
SET( STL_PARSER_PREFIX  "ztqSql" )
# -----------------------------------------------------
# Parser에서 사용될 Macro Body가 구현되어 있는 파일명 
# -----------------------------------------------------
SET( STL_PARSER_MACRO   "ztqSqlMacro" )
# -----------------------------------------------------
# stlParseLex.h.in을 변환시킬 파일명
# -----------------------------------------------------
SET( STL_PARSER_LEX      "${STL_PARSER_PREFIX}Lex" )

# -----------------------------------------------------
# Lexer 분석 결과 출력 (출력시 "1"로 설정)
# -----------------------------------------------------
SET( STL_PARSER_PRINT_FLAG "0" )

# -----------------------------------------------------
# Grammer 파일에 설정될 헤더 지정
# -----------------------------------------------------
SET( STL_INCLUDE_GRAMMER_H "#include <${STL_PARSER_LEX}.h>\n#include <${STL_PARSER_MACRO}.h>" )
# -----------------------------------------------------
# Lexer 파일에 설정될 헤더 지정
# -----------------------------------------------------
SET( STL_INCLUDE_LEXER_H "#include <${STL_PARSER_LEX}.h>\n#include <${STL_PARSER_PREFIX}.tab.h>\n#include <${STL_PARSER_MACRO}.h>" )

CONFIGURE_FILE(${PRODUCT_LAYER_STANDARD_DIR}/dev/src/stl/stlGram.y.in
               ${ZTQ_DIR}/dev/src/${STL_PARSER_PREFIX}.y)
CONFIGURE_FILE(${PRODUCT_LAYER_STANDARD_DIR}/dev/src/stl/stlLex.l.in
               ${ZTQ_DIR}/dev/src/${STL_PARSER_PREFIX}.l)
CONFIGURE_FILE(${PRODUCT_LAYER_STANDARD_DIR}/dev/include/stlParseLex.h.in
               ${ZTQ_DIR}/dev/src/include/${STL_PARSER_LEX}.h)

STL_FLEX_TARGET(ZTQ_SQL_SCANNER
                ${ZTQ_DIR}/dev/src/${STL_PARSER_PREFIX}.l
                ${ZTQ_DIR}/dev/src/${STL_PARSER_PREFIX}.yy.c
                COMPILE_FLAGS "-P${STL_PARSER_PREFIX}")
SET_SOURCE_FILES_PROPERTIES(${STL_FLEX_ZTQ_SQL_SCANNER_OUTPUTS} PROPERTIES GENERATED 1)

STL_BISON_TARGET(ZTQ_SQL_PARSER
                 ${ZTQ_DIR}/dev/src/${STL_PARSER_PREFIX}.y
                 ${ZTQ_DIR}/dev/src/${STL_PARSER_PREFIX}.tab.c
                 COMPILE_FLAGS "--name-prefix=${STL_PARSER_PREFIX} --defines=${ZTQ_DIR}/dev/src/include/${STL_PARSER_PREFIX}.tab.h")
SET_SOURCE_FILES_PROPERTIES(${STL_BISON_ZTQ_SQL_PARSER_OUTPUTS} PROPERTIES GENERATED 1)

ADD_DEFINITIONS( -DYY_NO_INPUT )
IF( ${ODBC_LEGACY_64BIT} )
    ADD_DEFINITIONS( -DBUILD_LEGACY_64_BIT_MODE )
ENDIF()

SET( GSQL_SOURCES_FILES ${ZTQ_SOURCE_FILES}
                        ${STL_FLEX_ZTQ_CMD_SCANNER_OUTPUTS}
                        ${STL_BISON_ZTQ_CMD_PARSER_OUTPUTS} 
                        ${STL_FLEX_ZTQ_SQL_SCANNER_OUTPUTS}
                        ${STL_BISON_ZTQ_SQL_PARSER_OUTPUTS} ) 

IF( ${CMAKE_SUPPORT_CPP_ODBC} )
    SET_SOURCE_FILES_PROPERTIES( ${GSQL_SOURCES_FILES} PROPERTIES LANGUAGE CXX )
    SET( CPP_LIBRARY stdc++ )
ENDIF()
IF( ${CMAKE_CPP_COMPILER} )
    SET_SOURCE_FILES_PROPERTIES( ${GSQL_SOURCES_FILES} PROPERTIES LANGUAGE CXX )
ENDIF()

LIST(REMOVE_DUPLICATES GSQL_SOURCES_FILES)

IF(${CMAKE_TARGET_HOST_TYPE} MATCHES "server")
ADD_EXECUTABLE( gsql ${GSQL_SOURCES_FILES})

TARGET_LINK_LIBRARIES (gsql ${CPP_LIBRARY}
                            ${ZLA_LIBRARY}
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

SET_TARGET_PROPERTIES( gsql PROPERTIES
                            RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_BIN}
                            RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_BIN}
                            RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_BIN})
ENDIF()

ADD_EXECUTABLE( gsqlnet ${GSQL_SOURCES_FILES})

TARGET_LINK_LIBRARIES (gsqlnet ${CPP_LIBRARY}
                               ${ZLC_LIBRARY}
                               ${CM_LIBRARY}
                               ${DT_LIBRARY} 
                               ${ST_LIBRARY})

SET_TARGET_PROPERTIES( gsqlnet PROPERTIES
                               COMPILE_FLAGS -DGSQLNET
                               RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_BIN}
                               RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_BIN}
                               RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_BIN})

############################################
# PACKAGE( make package )
############################################

IF(${CMAKE_TARGET_HOST_TYPE} MATCHES "server")
INSTALL( TARGETS gsql    DESTINATION ${INSTALL_BINDIR} )
ENDIF()

INSTALL( TARGETS gsqlnet DESTINATION ${INSTALL_BINDIR} )

