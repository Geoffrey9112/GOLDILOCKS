CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

#set the Standard Library
SET (ST_LIBRARY st)
LINK_DIRECTORIES (${PRODUCT_LAYER_STANDARD_DIR}/dev/lib)

#set the Data Type Library
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

#set the Storage Manager Library
SET (SM_LIBRARY sm)
LINK_DIRECTORIES (${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/lib)

#set the Replication Library
SET (RM_LIBRARY rp)
LINK_DIRECTORIES (${PRODUCT_LAYER_REPLICATION_DIR}/dev/lib)

# include directories
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STANDARD_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_DATATYPE_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_COMMUNICATION_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_KERNEL_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_SERVERCOMMUNICATION_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_STORAGEMANAGER_DIR}/dev/include)
INCLUDE_DIRECTORIES(${PRODUCT_LAYER_REPLICATION_DIR}/dev/include)

#define a macro to simplify adding tests, then use it
MACRO (DO_TEST ARG)
  ADD_EXECUTABLE( test${ARG} test${ARG}.c )
  TARGET_LINK_LIBRARIES (test${ARG} ${SM_LIBRARY} 
                                    ${SC_LIBRARY} 
                                    ${KN_LIBRARY} 
                                    ${CM_LIBRARY} 
                                    ${DT_LIBRARY} 
                                    ${ST_LIBRARY})
  ADD_TEST (${ARG} test${ARG})
  SET_TESTS_PROPERTIES( ${ARG}
    PROPERTIES PASS_REGULAR_EXPRESSION
    "PASS")
ENDMACRO (DO_TEST)

MACRO (DO_DIFF ARG)
  ADD_EXECUTABLE( test${ARG} test${ARG}.c )
  TARGET_LINK_LIBRARIES (test${ARG} ${SM_LIBRARY} 
                                    ${SC_LIBRARY} 
                                    ${KN_LIBRARY} 
                                    ${CM_LIBRARY} 
                                    ${DT_LIBRARY} 
                                    ${ST_LIBRARY})
  ADD_TEST ( ${ARG} udiff test${ARG} test${ARG} )
  SET_TESTS_PROPERTIES( ${ARG}
    PROPERTIES PASS_REGULAR_EXPRESSION
    "PASS")
ENDMACRO (DO_DIFF)

MACRO( DO_DIFF2 ARG )
  SET( SM_${ARG}_OUTPUT test${ARG}.c )
  ADD_CUSTOM_COMMAND( OUTPUT ${SM_${ARG}_OUTPUT}
                      COMMAND codegen 
                      ARGS -i test${ARG}.sc -o test${ARG}.c 
                      DEPENDS test${ARG}.sc )

  ADD_EXECUTABLE( test${ARG} ${SM_${ARG}_OUTPUT} )
  TARGET_LINK_LIBRARIES (test${ARG} ${SM_LIBRARY} 
                                    ${SC_LIBRARY} 
                                    ${KN_LIBRARY} 
                                    ${CM_LIBRARY} 
                                    ${DT_LIBRARY} 
                                    ${ST_LIBRARY})
  ADD_TEST ( ${ARG} udiff test${ARG} test${ARG} )
  SET_TESTS_PROPERTIES( ${ARG}
                        PROPERTIES PASS_REGULAR_EXPRESSION
                        "PASS")
ENDMACRO( DO_DIFF2 )

ADD_EXECUTABLE( testCheckPoint testCheckPoint.c )
TARGET_LINK_LIBRARIES (testCheckPoint ${SM_LIBRARY} 
                                      ${SC_LIBRARY} 
                                      ${KN_LIBRARY} 
                                      ${CM_LIBRARY} 
                                      ${DT_LIBRARY} 
                                      ${ST_LIBRARY})
