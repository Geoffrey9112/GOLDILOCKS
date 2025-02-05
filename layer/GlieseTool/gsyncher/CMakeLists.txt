CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

SET( SML_MARSHALL_SMALL_PREFIX "zty" )
SET( SML_MARSHALL_LARGE_PREFIX "ZTY" )

CONFIGURE_FILE(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/include/smlClassDef.h.in
               ${CMAKE_CURRENT_SOURCE_DIR}/dev/include/${SML_MARSHALL_SMALL_PREFIX}ClassDef.h)

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

# include directories
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STANDARD_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_KERNEL_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SERVERCOMMUNICATION_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_GLIESETOOL_DIR}/gsyncher/dev/include)

SET( ZTY_DIR ${PRODUCT_LAYER_GLIESETOOL_DIR}/gsyncher )

# file directories
AUX_SOURCE_DIRECTORY( ${ZTY_DIR}/dev/src/ztym ZTY_MAIN_SOURCE_FILES )
#AUX_SOURCE_DIRECTORY( ${ZTY_DIR}/dev/src/ztys ZTY_SM_SOURCE_FILES )

SET( GSYNCHER_SOURCES_FILES ${ZTY_MAIN_SOURCE_FILES} )
#                            ${ZTY_SM_SOURCE_FILES} )

# output path 
SET (EXECUTABLE_OUTPUT_PATH ${GLIESE_HOME_BIN})

IF( ${CMAKE_CPP_COMPILER} )
    SET_SOURCE_FILES_PROPERTIES( ${GSYNCHER_LIBRARY_SOURCE_FILES} PROPERTIES LANGUAGE CXX )
ENDIF()

LIST(REMOVE_DUPLICATES GSYNCHER_SOURCES_FILES)

ADD_EXECUTABLE( gsyncher ${GSYNCHER_SOURCES_FILES})
TARGET_LINK_LIBRARIES (gsyncher ${SM_LIBRARY} 
                                ${SC_LIBRARY} 
                                ${KN_LIBRARY} 
                                ${CM_LIBRARY} 
                                ${DT_LIBRARY} 
                                ${ST_LIBRARY})

############################################
# PACKAGE( make package )
############################################

INSTALL( PROGRAMS ${GLIESE_HOME_BIN}/gsyncher     DESTINATION ${INSTALL_BINDIR} )



