CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

#set the Gliese Library
SET (ST_LIBRARY st)
LINK_DIRECTORIES (${PRODUCT_LAYER_STANDARD_DIR}/dev/lib)

#set the DataType Library
SET (DT_LIBRARY dt)
LINK_DIRECTORIES (${PRODUCT_LAYER_DATATYPE_DIR}/dev/lib)

#set the Communication Library
SET (CM_LIBRARY cm)
LINK_DIRECTORIES (${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/lib)

# include directories
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STANDARD_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STANDARD_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_KERNEL_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_KERNEL_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SERVERCOMMUNICATION_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SERVERCOMMUNICATION_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_EXECUTIONLIBRARY_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_EXECUTIONLIBRARY_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SQLPROCESSOR_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SQLPROCESSOR_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SESSION_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SESSION_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SERVERLIBRARY_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SERVERLIBRARY_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_ODBC_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_ODBC_DIR}/dev/src/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_GLIESETOOL_DIR}/gerrormsg/dev/src/include)

SET( ZTE_DIR ${PRODUCT_LAYER_GLIESETOOL_DIR}/gerrormsg )

AUX_SOURCE_DIRECTORY( ${ZTE_DIR}/dev/src ZTE_SOURCE_FILES )

SET (EXECUTABLE_OUTPUT_PATH ${GLIESE_HOME_BIN})

ADD_EXECUTABLE( gerrormsg ${ZTE_SOURCE_FILES}
                          ${PRODUCT_LAYER_GOLDILOCKSESQLLIB_DIR}/dev/src/zlpe/zlpeError.c
                          ${PRODUCT_LAYER_GLIESETOOL_DIR}/gsql/dev/src/ztqError.c
                          ${PRODUCT_LAYER_GLIESETOOL_DIR}/gpec/dev/src/ztpe/ztpeError.c
                          ${PRODUCT_LAYER_GLIESETOOL_DIR}/gloader/dev/src/ztdError.c
                          ${PRODUCT_LAYER_GLIESETOOL_DIR}/glsnr/dev/src/ztle/ztleError.c
                          ${PRODUCT_LAYER_GLIESETOOL_DIR}/cyclone/dev/src/ztcm/ztcmError.c
                          ${PRODUCT_LAYER_GLIESETOOL_DIR}/LogMirror/dev/src/ztrm/ztrmError.c
                          ${PRODUCT_LAYER_GLIESETOOL_DIR}/cymon/dev/src/ztnm/ztnmError.c
                          ${PRODUCT_LAYER_GLIESETOOL_DIR}/gserver/dev/src/ztse/ztseError.c
                          ${PRODUCT_LAYER_GLIESETOOL_DIR}/gdispatcher/dev/src/ztie/ztieError.c
                          ${PRODUCT_LAYER_GLIESETOOL_DIR}/gbalancer/dev/src/ztbe/ztbeError.c
                          ${PRODUCT_LAYER_STANDARD_DIR}/dev/src/stl/stlError.c
                          ${PRODUCT_LAYER_DATATYPE_DIR}/dev/src/dtl/dtlError.c
                          ${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/src/cml/cmlError.c
                          ${PRODUCT_LAYER_KERNEL_DIR}/dev/src/knl/knlError.c
                          ${PRODUCT_LAYER_SERVERCOMMUNICATION_DIR}/dev/src/scl/sclError.c
                          ${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/src/sml/smlError.c
                          ${PRODUCT_LAYER_EXECUTIONLIBRARY_DIR}/dev/src/ell/ellError.c
                          ${PRODUCT_LAYER_SQLPROCESSOR_DIR}/dev/src/qll/qllError.c
                          ${PRODUCT_LAYER_SESSION_DIR}/dev/src/ssl/sslError.c 
                          ${PRODUCT_LAYER_SERVERLIBRARY_DIR}/dev/src/sll/sllError.c 
                          ${PRODUCT_LAYER_ODBC_DIR}/dev/src/zle/zleError.c )

SET_TARGET_PROPERTIES( gerrormsg PROPERTIES
                                 RUNTIME_OUTPUT_DIRECTORY           ${GLIESE_HOME_BIN}
                                 RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${GLIESE_HOME_BIN}
                                 RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${GLIESE_HOME_BIN})
							   
TARGET_LINK_LIBRARIES (gerrormsg ${CM_LIBRARY} 
                                 ${DT_LIBRARY} 
                                 ${ST_LIBRARY})

