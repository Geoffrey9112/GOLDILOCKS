CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

#set the Standard Library
SET (ST_LIBRARY st)
LINK_DIRECTORIES (${PRODUCT_LAYER_STANDARD_DIR}/dev/lib)

#set the DataType Library
SET (DT_LIBRARY dt)
LINK_DIRECTORIES (${PRODUCT_LAYER_DATATYPE_DIR}/dev/lib)

# include directories
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STANDARD_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_ODBC_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_GOLDILOCKSESQLLIB_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_GLIESETOOL_DIR}/gpec/dev/src/include)

SET( ZTP_DIR ${PRODUCT_LAYER_GLIESETOOL_DIR}/gpec )

AUX_SOURCE_DIRECTORY( ${ZTP_DIR}/dev/src/ztp   ZTP_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZTP_DIR}/dev/src/ztpc  ZTPC_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZTP_DIR}/dev/src/ztpd  ZTPD_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZTP_DIR}/dev/src/ztpe  ZTPE_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZTP_DIR}/dev/src/ztpf  ZTPF_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZTP_DIR}/dev/src/ztph  ZTPH_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZTP_DIR}/dev/src/ztpp  ZTPP_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZTP_DIR}/dev/src/ztpu  ZTPU_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZTP_DIR}/dev/src/ztpv  ZTPV_SOURCE_FILES )
AUX_SOURCE_DIRECTORY( ${ZTP_DIR}/dev/src/ztpw  ZTPW_SOURCE_FILES )

SET (EXECUTABLE_OUTPUT_PATH ${GLIESE_HOME_BIN})

STL_FLEX_TARGET(ZTP_C_MIN_SCANNER
                ${ZTP_DIR}/dev/src/ztp/ztpCMin.l
                ${ZTP_DIR}/dev/src/ztp/ztpCMin.yy.c
                COMPILE_FLAGS "--prefix=ztpCMin")
SET_SOURCE_FILES_PROPERTIES(${STL_FLEX_ZTP_C_MIN_SCANNER_OUTPUTS} PROPERTIES GENERATED 1)

STL_BISON_TARGET(ZTP_C_MIN_PARSER
                 ${ZTP_DIR}/dev/src/ztp/ztpCMin.y
                 ${ZTP_DIR}/dev/src/ztp/ztpCMin.tab.c
                 COMPILE_FLAGS "--name-prefix=ztpCMin --defines=${ZTP_DIR}/dev/src/include/ztpCMin.tab.h")
SET_SOURCE_FILES_PROPERTIES(${STL_BISON_ZTP_C_MIN_PARSER_OUTPUTS} PROPERTIES GENERATED 1)

#STL_FLEX_TARGET(ZTP_PP_SCANNER
#                ${ZTP_DIR}/dev/src/ztpp/ztppLex.l
#                ${ZTP_DIR}/dev/src/ztpp/ztppLex.yy.c
#                COMPILE_FLAGS "--prefix=ztpPP")
#SET_SOURCE_FILES_PROPERTIES(${STL_FLEX_ZTP_PP_SCANNER_OUTPUTS} PROPERTIES GENERATED 1)

#STL_BISON_TARGET(ZTP_PP_PARSER
#                 ${ZTP_DIR}/dev/src/ztpp/ztppGram.y
#                 ${ZTP_DIR}/dev/src/ztpp/ztppGram.tab.c
#                 COMPILE_FLAGS "--name-prefix=ztpPP --defines=${ZTP_DIR}/dev/src/include/ztpPP.tab.h")
#SET_SOURCE_FILES_PROPERTIES(${STL_BISON_ZTP_PP_PARSER_OUTPUTS} PROPERTIES GENERATED 1)

#STL_FLEX_TARGET(ZTP_PP_CALC_SCANNER
#                ${ZTP_DIR}/dev/src/ztpp/ztppCalc.l
#                ${ZTP_DIR}/dev/src/ztpp/ztppCalc.yy.c
#                COMPILE_FLAGS "--prefix=ztppCalc")
#SET_SOURCE_FILES_PROPERTIES(${STL_FLEX_ZTP_PP_CALC_SCANNER_OUTPUTS} PROPERTIES GENERATED 1)

#STL_BISON_TARGET(ZTP_PP_CALC_PARSER
#                 ${ZTP_DIR}/dev/src/ztpp/ztppCalc.y
#                 ${ZTP_DIR}/dev/src/ztpp/ztppCalc.tab.c
#                 COMPILE_FLAGS "--name-prefix=ztppCalc --defines=${ZTP_DIR}/dev/src/include/ztppCalc.tab.h")
#SET_SOURCE_FILES_PROPERTIES(${STL_BISON_ZTP_PP_CALC_PARSER_OUTPUTS} PROPERTIES GENERATED 1)

STL_FLEX_TARGET(ZTP_C_HOST_VAR_SCANNER
                ${ZTP_DIR}/dev/src/ztpv/ztpvCVar.l
                ${ZTP_DIR}/dev/src/ztpv/ztpvCVar.yy.c
                COMPILE_FLAGS "--prefix=ztpvCVar")
SET_SOURCE_FILES_PROPERTIES(${STL_FLEX_ZTP_C_HOST_VAR_SCANNER_OUTPUTS} PROPERTIES GENERATED 1)

STL_BISON_TARGET(ZTP_C_HOST_VAR_PARSER
                 ${ZTP_DIR}/dev/src/ztpv/ztpvCVar.y
                 ${ZTP_DIR}/dev/src/ztpv/ztpvCVar.tab.c
                 COMPILE_FLAGS "--name-prefix=ztpvCVar --defines=${ZTP_DIR}/dev/src/include/ztpvCVar.tab.h")
SET_SOURCE_FILES_PROPERTIES(${STL_BISON_ZTP_C_HOST_VAR_PARSER_OUTPUTS} PROPERTIES GENERATED 1)

STL_FLEX_TARGET(ZTP_PATH_SCANNER
                ${ZTP_DIR}/dev/src/ztpf/ztpfPath.l
                ${ZTP_DIR}/dev/src/ztpf/ztpfPath.yy.c
                COMPILE_FLAGS "--prefix=ztpfPath")
SET_SOURCE_FILES_PROPERTIES(${STL_FLEX_ZTP_PATH_SCANNER_OUTPUTS} PROPERTIES GENERATED 1)

STL_BISON_TARGET(ZTP_PATH_PARSER
                 ${ZTP_DIR}/dev/src/ztpf/ztpfPath.y
                 ${ZTP_DIR}/dev/src/ztpf/ztpfPath.tab.c
                 COMPILE_FLAGS "--name-prefix=ztpfPath --defines=${ZTP_DIR}/dev/src/include/ztpfPath.tab.h")
SET_SOURCE_FILES_PROPERTIES(${STL_BISON_ZTP_PATH_PARSER_OUTPUTS} PROPERTIES GENERATED 1)

STL_FLEX_TARGET(ZTP_INCLUDE_FILE_SCANNER
                ${ZTP_DIR}/dev/src/ztpf/ztpfIncludeFile.l
                ${ZTP_DIR}/dev/src/ztpf/ztpfIncludeFile.yy.c
                COMPILE_FLAGS "--prefix=ztpfIncludeFile")
SET_SOURCE_FILES_PROPERTIES(${STL_FLEX_ZTP_INCLUDE_FILE_SCANNER_OUTPUTS} PROPERTIES GENERATED 1)

STL_BISON_TARGET(ZTP_INCLUDE_FILE_PARSER
                 ${ZTP_DIR}/dev/src/ztpf/ztpfIncludeFile.y
                 ${ZTP_DIR}/dev/src/ztpf/ztpfIncludeFile.tab.c
                 COMPILE_FLAGS "--name-prefix=ztpfIncludeFile --defines=${ZTP_DIR}/dev/src/include/ztpfIncludeFile.tab.h")
SET_SOURCE_FILES_PROPERTIES(${STL_BISON_ZTP_INCLUDE_FILE_PARSER_OUTPUTS} PROPERTIES GENERATED 1)

# -----------------------------------------------------
# Parser의 Prefix
# -----------------------------------------------------
SET( STL_PARSER_PREFIX  "ztpSql" )
# -----------------------------------------------------
# Parser에서 사용될 Macro Body가 구현되어 있는 파일명 
# -----------------------------------------------------
SET( STL_PARSER_MACRO   "ztpSqlMacro" )
# -----------------------------------------------------
# stlParseLex.h.in을 변환시킬 파일명
# -----------------------------------------------------
SET( STL_PARSER_LEX     "${STL_PARSER_PREFIX}Lex" )

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
               ${ZTP_DIR}/dev/src/ztp/${STL_PARSER_PREFIX}.y)
CONFIGURE_FILE(${PRODUCT_LAYER_STANDARD_DIR}/dev/src/stl/stlLex.l.in
               ${ZTP_DIR}/dev/src/ztp/${STL_PARSER_PREFIX}.l)
CONFIGURE_FILE(${PRODUCT_LAYER_STANDARD_DIR}/dev/include/stlParseLex.h.in
               ${ZTP_DIR}/dev/src/include/${STL_PARSER_LEX}.h)

STL_FLEX_TARGET(ZTP_SQL_SCANNER
                ${ZTP_DIR}/dev/src/ztp/${STL_PARSER_PREFIX}.l
                ${ZTP_DIR}/dev/src/ztp/${STL_PARSER_PREFIX}.yy.c
                COMPILE_FLAGS "-P${STL_PARSER_PREFIX}")
SET_SOURCE_FILES_PROPERTIES(${STL_FLEX_ZTP_SQL_SCANNER_OUTPUTS} PROPERTIES GENERATED 1)

STL_BISON_TARGET(ZTP_SQL_PARSER
                 ${ZTP_DIR}/dev/src/ztp/${STL_PARSER_PREFIX}.y
                 ${ZTP_DIR}/dev/src/ztp/${STL_PARSER_PREFIX}.tab.c
                 COMPILE_FLAGS "--name-prefix=${STL_PARSER_PREFIX} --defines=${ZTP_DIR}/dev/src/include/${STL_PARSER_PREFIX}.tab.h")
SET_SOURCE_FILES_PROPERTIES(${STL_BISON_ZTP_SQL_PARSER_OUTPUTS} PROPERTIES GENERATED 1)

ADD_DEFINITIONS( -DYY_NO_INPUT )
IF( STL_PARSER_PRINT_FLAG MATCHES "1" )
  ADD_DEFINITIONS( -DZTP_PARSER_PRINT )
ENDIF()

SET( GPEC_SOURCES_FILES ${ZTP_SOURCE_FILES}
                        ${ZTPC_SOURCE_FILES}
                        ${ZTPD_SOURCE_FILES}
                        ${ZTPE_SOURCE_FILES}
                        ${ZTPF_SOURCE_FILES}
#                        ${ZTPH_SOURCE_FILES}
#                        ${ZTPP_SOURCE_FILES}
                        ${ZTPU_SOURCE_FILES}
                        ${ZTPV_SOURCE_FILES}
                        ${ZTPW_SOURCE_FILES}
                        ${STL_FLEX_ZTP_C_MIN_SCANNER_OUTPUTS}
                        ${STL_BISON_ZTP_C_MIN_PARSER_OUTPUTS} 
#                        ${STL_FLEX_ZTP_PP_SCANNER_OUTPUTS}
#                        ${STL_BISON_ZTP_PP_PARSER_OUTPUTS} 
#                        ${STL_FLEX_ZTP_PP_CALC_SCANNER_OUTPUTS}
#                        ${STL_BISON_ZTP_PP_CALC_PARSER_OUTPUTS} 
                        ${STL_FLEX_ZTP_C_HOST_VAR_SCANNER_OUTPUTS}
                        ${STL_BISON_ZTP_C_HOST_VAR_PARSER_OUTPUTS} 
                        ${STL_FLEX_ZTP_PATH_SCANNER_OUTPUTS}
                        ${STL_BISON_ZTP_PATH_PARSER_OUTPUTS} 
                        ${STL_FLEX_ZTP_INCLUDE_FILE_SCANNER_OUTPUTS}
                        ${STL_BISON_ZTP_INCLUDE_FILE_PARSER_OUTPUTS} 
                        ${STL_FLEX_ZTP_SQL_SCANNER_OUTPUTS}
                        ${STL_BISON_ZTP_SQL_PARSER_OUTPUTS} )

IF( ${CMAKE_CPP_COMPILER} )
    SET_SOURCE_FILES_PROPERTIES( ${GPEC_SOURCE_FILES} PROPERTIES LANGUAGE CXX )
ENDIF()

LIST(REMOVE_DUPLICATES GPEC_SOURCES_FILES)
ADD_EXECUTABLE( gpec ${GPEC_SOURCES_FILES})

SET_TARGET_PROPERTIES( gpec PROPERTIES
                            RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_BIN}
                            RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_BIN}
                            RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_BIN})

#SET(REMOVE_OLD_FILES remove_files)
#ADD_CUSTOM_TARGET( remove_files
#	${CMAKE_COMMAND} -E remove -f "${ZTP_DIR}/dev/src/ztp/ztpC.yy.c" "${ZTP_DIR}/dev/src/ztp/ztpC.tab.c" "${ZTP_DIR}/dev/src/include/ztpC.tab.h"
#	COMMENT "delete old files"
#)
#ADD_DEPENDENCIES(gpec ${REMOVE_OLD_FILES})

TARGET_LINK_LIBRARIES (gpec ${DT_LIBRARY} 
                            ${ST_LIBRARY})

############################################
# PACKAGE( make package )
############################################

INSTALL( TARGETS gpec DESTINATION ${INSTALL_BINDIR} )