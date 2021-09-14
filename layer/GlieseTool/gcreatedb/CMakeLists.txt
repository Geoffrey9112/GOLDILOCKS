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
SET (EL_LIBRARY el)
LINK_DIRECTORIES (${PRODUCT_LAYER_EXECUTIONLIBRARY_DIR}/dev/lib)

#set the Sql Processor
SET (QP_LIBRARY qp)
LINK_DIRECTORIES (${PRODUCT_LAYER_SQLPROCESSOR_DIR}/dev/lib)

# include directories
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STANDARD_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_KERNEL_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SERVERCOMMUNICATION_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_EXECUTIONLIBRARY_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SQLPROCESSOR_DIR}/dev/include)

SET (EXECUTABLE_OUTPUT_PATH ${GLIESE_HOME_BIN})

ADD_EXECUTABLE( gcreatedb dev/src/gcreatedb.c )
TARGET_LINK_LIBRARIES (gcreatedb ${QP_LIBRARY} 
                                 ${EL_LIBRARY} 
                                 ${SM_LIBRARY} 
                                 ${SC_LIBRARY} 
                                 ${KN_LIBRARY} 
                                 ${CM_LIBRARY} 
                                 ${DT_LIBRARY} 
                                 ${ST_LIBRARY})

############################################
# PACKAGE( make package )
############################################

INSTALL( PROGRAMS ${GLIESE_HOME_BIN}/gcreatedb DESTINATION ${INSTALL_BINDIR} )
