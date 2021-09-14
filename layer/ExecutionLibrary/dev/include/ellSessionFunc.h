/*******************************************************************************
 * ellSessionFunc.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _ELLSESSIONFUNC_H_
#define _ELLSESSIONFUNC_H_ 1

/**
 * @file ellSessionFunc.h
 * @brief ellSessionFunc Execution Layer Session Function
 */
 

/**
 * @addtogroup ellSessionFunc 
 * @{
 */


/****************************
 * SYSTEM
 ***************************/

/* Catalog Name */
stlStatus ellGetFuncInfoCatalogName( stlUInt16               aDataTypeArrayCount,
                                     stlBool               * aIsConstantData,
                                     dtlDataType           * aDataTypeArray,
                                     dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                     stlUInt16               aFuncArgDataTypeArrayCount,
                                     dtlDataType           * aFuncArgDataTypeArray,
                                     dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                     dtlDataType           * aFuncResultDataType,
                                     dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                     stlUInt32             * aFuncPtrIdx,
                                     dtlFuncVector         * aVectorFunc,
                                     void                  * aVectorArg,                                      
                                     stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrCatalogName( stlUInt32             aFuncPtrIdx,
                                    dtlBuiltInFuncPtr   * aFuncPtr,
                                    stlErrorStack       * aErrorStack );

stlStatus ellFuncCatalogName( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv );


/* Version */
stlStatus ellGetFuncInfoVersion( stlUInt16               aDataTypeArrayCount,
                                 stlBool               * aIsConstantData,
                                 dtlDataType           * aDataTypeArray,
                                 dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                 stlUInt16               aFuncArgDataTypeArrayCount,
                                 dtlDataType           * aFuncArgDataTypeArray,
                                 dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                 dtlDataType           * aFuncResultDataType,
                                 dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                 stlUInt32             * aFuncPtrIdx,
                                 dtlFuncVector         * aVectorFunc,
                                 void                  * aVectorArg,                                  
                                 stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrVersion( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack );

stlStatus ellFuncVersion( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );


/****************************
 * SESSION
 ***************************/

/* SessionID */
stlStatus ellGetFuncInfoSessionID( stlUInt16               aDataTypeArrayCount,
                                   stlBool               * aIsConstantData,
                                   dtlDataType           * aDataTypeArray,
                                   dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                   stlUInt16               aFuncArgDataTypeArrayCount,
                                   dtlDataType           * aFuncArgDataTypeArray,
                                   dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                   dtlDataType           * aFuncResultDataType,
                                   dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                   stlUInt32             * aFuncPtrIdx,
                                   dtlFuncVector         * aVectorFunc,
                                   void                  * aVectorArg,                                    
                                   stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrSessionID( stlUInt32             aFuncPtrIdx,
                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                  stlErrorStack       * aErrorStack );

stlStatus ellFuncSessionID( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv );


/* SessionSerial */
stlStatus ellGetFuncInfoSessionSerial( stlUInt16               aDataTypeArrayCount,
                                       stlBool               * aIsConstantData,
                                       dtlDataType           * aDataTypeArray,
                                       dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                       stlUInt16               aFuncArgDataTypeArrayCount,
                                       dtlDataType           * aFuncArgDataTypeArray,
                                       dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                       dtlDataType           * aFuncResultDataType,
                                       dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                       stlUInt32             * aFuncPtrIdx,
                                       dtlFuncVector         * aVectorFunc,
                                       void                  * aVectorArg,                                        
                                       stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrSessionSerial( stlUInt32             aFuncPtrIdx,
                                      dtlBuiltInFuncPtr   * aFuncPtr,
                                      stlErrorStack       * aErrorStack );

stlStatus ellFuncSessionSerial( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );


/* UserID */
stlStatus ellGetFuncInfoUserID( stlUInt16               aDataTypeArrayCount,
                                stlBool               * aIsConstantData,
                                dtlDataType           * aDataTypeArray,
                                dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                stlUInt16               aFuncArgDataTypeArrayCount,
                                dtlDataType           * aFuncArgDataTypeArray,
                                dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                dtlDataType           * aFuncResultDataType,
                                dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                stlUInt32             * aFuncPtrIdx,
                                dtlFuncVector         * aVectorFunc,
                                void                  * aVectorArg,                                 
                                stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrUserID( stlUInt32             aFuncPtrIdx,
                               dtlBuiltInFuncPtr   * aFuncPtr,
                               stlErrorStack       * aErrorStack );

stlStatus ellFuncUserID( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );


/* CurrentUser */
stlStatus ellGetFuncInfoCurrentUser( stlUInt16               aDataTypeArrayCount,
                                     stlBool               * aIsConstantData,
                                     dtlDataType           * aDataTypeArray,
                                     dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                     stlUInt16               aFuncArgDataTypeArrayCount,
                                     dtlDataType           * aFuncArgDataTypeArray,
                                     dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                     dtlDataType           * aFuncResultDataType,
                                     dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                     stlUInt32             * aFuncPtrIdx,
                                     dtlFuncVector         * aVectorFunc,
                                     void                  * aVectorArg,                                      
                                     stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrCurrentUser( stlUInt32             aFuncPtrIdx,
                                    dtlBuiltInFuncPtr   * aFuncPtr,
                                    stlErrorStack       * aErrorStack );

stlStatus ellFuncCurrentUser( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv );

/* SessionUser */
stlStatus ellGetFuncInfoSessionUser( stlUInt16               aDataTypeArrayCount,
                                     stlBool               * aIsConstantData,
                                     dtlDataType           * aDataTypeArray,
                                     dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                     stlUInt16               aFuncArgDataTypeArrayCount,
                                     dtlDataType           * aFuncArgDataTypeArray,
                                     dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                     dtlDataType           * aFuncResultDataType,
                                     dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                     stlUInt32             * aFuncPtrIdx,
                                     dtlFuncVector         * aVectorFunc,
                                     void                  * aVectorArg,                                      
                                     stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrSessionUser( stlUInt32             aFuncPtrIdx,
                                    dtlBuiltInFuncPtr   * aFuncPtr,
                                    stlErrorStack       * aErrorStack );

stlStatus ellFuncSessionUser( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv );

/* LogonUser */
stlStatus ellGetFuncInfoLogonUser( stlUInt16               aDataTypeArrayCount,
                                   stlBool               * aIsConstantData,
                                   dtlDataType           * aDataTypeArray,
                                   dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                   stlUInt16               aFuncArgDataTypeArrayCount,
                                   dtlDataType           * aFuncArgDataTypeArray,
                                   dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                   dtlDataType           * aFuncResultDataType,
                                   dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                   stlUInt32             * aFuncPtrIdx,
                                   dtlFuncVector         * aVectorFunc,
                                   void                  * aVectorArg,                                    
                                   stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrLogonUser( stlUInt32             aFuncPtrIdx,
                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                  stlErrorStack       * aErrorStack );

stlStatus ellFuncLogonUser( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv );


/* CurrentSchema */
stlStatus ellGetFuncInfoCurrentSchema( stlUInt16               aDataTypeArrayCount,
                                       stlBool               * aIsConstantData,
                                       dtlDataType           * aDataTypeArray,
                                       dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                       stlUInt16               aFuncArgDataTypeArrayCount,
                                       dtlDataType           * aFuncArgDataTypeArray,
                                       dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                       dtlDataType           * aFuncResultDataType,
                                       dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                       stlUInt32             * aFuncPtrIdx,
                                       dtlFuncVector         * aVectorFunc,
                                       void                  * aVectorArg,                                        
                                       stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrCurrentSchema( stlUInt32             aFuncPtrIdx,
                                      dtlBuiltInFuncPtr   * aFuncPtr,
                                      stlErrorStack       * aErrorStack );

stlStatus ellFuncCurrentSchema( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );


/****************************
 * DATE & TIME
 ***************************/

/* ClockDate */
stlStatus ellGetFuncInfoClockDate( stlUInt16               aDataTypeArrayCount,
                                   stlBool               * aIsConstantData,
                                   dtlDataType           * aDataTypeArray,
                                   dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                   stlUInt16               aFuncArgDataTypeArrayCount,
                                   dtlDataType           * aFuncArgDataTypeArray,
                                   dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                   dtlDataType           * aFuncResultDataType,
                                   dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                   stlUInt32             * aFuncPtrIdx,
                                   dtlFuncVector         * aVectorFunc,
                                   void                  * aVectorArg,                                    
                                   stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrClockDate( stlUInt32             aFuncPtrIdx,
                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                  stlErrorStack       * aErrorStack );

stlStatus ellFuncClockDate( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv );


/* StatementDate */
stlStatus ellGetFuncInfoStatementDate( stlUInt16               aDataTypeArrayCount,
                                       stlBool               * aIsConstantData,
                                       dtlDataType           * aDataTypeArray,
                                       dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                       stlUInt16               aFuncArgDataTypeArrayCount,
                                       dtlDataType           * aFuncArgDataTypeArray,
                                       dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                       dtlDataType           * aFuncResultDataType,
                                       dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                       stlUInt32             * aFuncPtrIdx,
                                       dtlFuncVector         * aVectorFunc,
                                       void                  * aVectorArg,                                        
                                       stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrStatementDate( stlUInt32             aFuncPtrIdx,
                                      dtlBuiltInFuncPtr   * aFuncPtr,
                                      stlErrorStack       * aErrorStack );

stlStatus ellFuncStatementDate( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );


/* TransactionDate */
stlStatus ellGetFuncInfoTransactionDate( stlUInt16               aDataTypeArrayCount,
                                         stlBool               * aIsConstantData,
                                         dtlDataType           * aDataTypeArray,
                                         dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                         stlUInt16               aFuncArgDataTypeArrayCount,
                                         dtlDataType           * aFuncArgDataTypeArray,
                                         dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                         dtlDataType           * aFuncResultDataType,
                                         dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                         stlUInt32             * aFuncPtrIdx,
                                         dtlFuncVector         * aVectorFunc,
                                         void                  * aVectorArg,
                                         stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrTransactionDate( stlUInt32             aFuncPtrIdx,
                                        dtlBuiltInFuncPtr   * aFuncPtr,
                                        stlErrorStack       * aErrorStack );

stlStatus ellFuncTransactionDate( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv );


/* ClockTime */
stlStatus ellGetFuncInfoClockTime( stlUInt16               aDataTypeArrayCount,
                                   stlBool               * aIsConstantData,
                                   dtlDataType           * aDataTypeArray,
                                   dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                   stlUInt16               aFuncArgDataTypeArrayCount,
                                   dtlDataType           * aFuncArgDataTypeArray,
                                   dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                   dtlDataType           * aFuncResultDataType,
                                   dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                   stlUInt32             * aFuncPtrIdx,
                                   dtlFuncVector         * aVectorFunc,
                                   void                  * aVectorArg,                                    
                                   stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrClockTime( stlUInt32             aFuncPtrIdx,
                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                  stlErrorStack       * aErrorStack );

stlStatus ellFuncClockTime( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv );


/* StatementTime */
stlStatus ellGetFuncInfoStatementTime( stlUInt16               aDataTypeArrayCount,
                                       stlBool               * aIsConstantData,
                                       dtlDataType           * aDataTypeArray,
                                       dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                       stlUInt16               aFuncArgDataTypeArrayCount,
                                       dtlDataType           * aFuncArgDataTypeArray,
                                       dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                       dtlDataType           * aFuncResultDataType,
                                       dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                       stlUInt32             * aFuncPtrIdx,
                                       dtlFuncVector         * aVectorFunc,
                                       void                  * aVectorArg,                                        
                                       stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrStatementTime( stlUInt32             aFuncPtrIdx,
                                      dtlBuiltInFuncPtr   * aFuncPtr,
                                      stlErrorStack       * aErrorStack );

stlStatus ellFuncStatementTime( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );


/* TransactionTime */
stlStatus ellGetFuncInfoTransactionTime( stlUInt16               aDataTypeArrayCount,
                                         stlBool               * aIsConstantData,
                                         dtlDataType           * aDataTypeArray,
                                         dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                         stlUInt16               aFuncArgDataTypeArrayCount,
                                         dtlDataType           * aFuncArgDataTypeArray,
                                         dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                         dtlDataType           * aFuncResultDataType,
                                         dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                         stlUInt32             * aFuncPtrIdx,
                                         dtlFuncVector         * aVectorFunc,
                                         void                  * aVectorArg,
                                         stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrTransactionTime( stlUInt32             aFuncPtrIdx,
                                        dtlBuiltInFuncPtr   * aFuncPtr,
                                        stlErrorStack       * aErrorStack );

stlStatus ellFuncTransactionTime( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv );


/* ClockTimestamp */
stlStatus ellGetFuncInfoClockTimestamp( stlUInt16               aDataTypeArrayCount,
                                        stlBool               * aIsConstantData,
                                        dtlDataType           * aDataTypeArray,
                                        dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                        stlUInt16               aFuncArgDataTypeArrayCount,
                                        dtlDataType           * aFuncArgDataTypeArray,
                                        dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                        dtlDataType           * aFuncResultDataType,
                                        dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                        stlUInt32             * aFuncPtrIdx,
                                        dtlFuncVector         * aVectorFunc,
                                        void                  * aVectorArg,
                                        stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrClockTimestamp( stlUInt32             aFuncPtrIdx,
                                       dtlBuiltInFuncPtr   * aFuncPtr,
                                       stlErrorStack       * aErrorStack );

stlStatus ellFuncClockTimestamp( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );


/* StatementTimestamp */
stlStatus ellGetFuncInfoStatementTimestamp( stlUInt16               aDataTypeArrayCount,
                                            stlBool               * aIsConstantData,
                                            dtlDataType           * aDataTypeArray,
                                            dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                            stlUInt16               aFuncArgDataTypeArrayCount,
                                            dtlDataType           * aFuncArgDataTypeArray,
                                            dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                            dtlDataType           * aFuncResultDataType,
                                            dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                            stlUInt32             * aFuncPtrIdx,
                                            dtlFuncVector         * aVectorFunc,
                                            void                  * aVectorArg,
                                            stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrStatementTimestamp( stlUInt32             aFuncPtrIdx,
                                           dtlBuiltInFuncPtr   * aFuncPtr,
                                           stlErrorStack       * aErrorStack );

stlStatus ellFuncStatementTimestamp( stlUInt16        aInputArgumentCnt,
                                     dtlValueEntry  * aInputArgument,
                                     void           * aResultValue,
                                     void           * aInfo,
                                     dtlFuncVector  * aVectorFunc,
                                     void           * aVectorArg,
                                     void           * aEnv );


/* TransactionTimestamp */
stlStatus ellGetFuncInfoTransactionTimestamp( stlUInt16               aDataTypeArrayCount,
                                              stlBool               * aIsConstantData,
                                              dtlDataType           * aDataTypeArray,
                                              dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                              stlUInt16               aFuncArgDataTypeArrayCount,
                                              dtlDataType           * aFuncArgDataTypeArray,
                                              dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                              dtlDataType           * aFuncResultDataType,
                                              dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                              stlUInt32             * aFuncPtrIdx,
                                              dtlFuncVector         * aVectorFunc,
                                              void                  * aVectorArg,
                                              stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrTransactionTimestamp( stlUInt32             aFuncPtrIdx,
                                             dtlBuiltInFuncPtr   * aFuncPtr,
                                             stlErrorStack       * aErrorStack );

stlStatus ellFuncTransactionTimestamp( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );


/* ClockLocalTime */
stlStatus ellGetFuncInfoClockLocalTime( stlUInt16               aDataTypeArrayCount,
                                        stlBool               * aIsConstantData,
                                        dtlDataType           * aDataTypeArray,
                                        dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                        stlUInt16               aFuncArgDataTypeArrayCount,
                                        dtlDataType           * aFuncArgDataTypeArray,
                                        dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                        dtlDataType           * aFuncResultDataType,
                                        dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                        stlUInt32             * aFuncPtrIdx,
                                        dtlFuncVector         * aVectorFunc,
                                        void                  * aVectorArg,
                                        stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrClockLocalTime( stlUInt32             aFuncPtrIdx,
                                       dtlBuiltInFuncPtr   * aFuncPtr,
                                       stlErrorStack       * aErrorStack );

stlStatus ellFuncClockLocalTime( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );


/* StatementLocalTime */
stlStatus ellGetFuncInfoStatementLocalTime( stlUInt16               aDataTypeArrayCount,
                                            stlBool               * aIsConstantData,
                                            dtlDataType           * aDataTypeArray,
                                            dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                            stlUInt16               aFuncArgDataTypeArrayCount,
                                            dtlDataType           * aFuncArgDataTypeArray,
                                            dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                            dtlDataType           * aFuncResultDataType,
                                            dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                            stlUInt32             * aFuncPtrIdx,
                                            dtlFuncVector         * aVectorFunc,
                                            void                  * aVectorArg,
                                            stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrStatementLocalTime( stlUInt32             aFuncPtrIdx,
                                           dtlBuiltInFuncPtr   * aFuncPtr,
                                           stlErrorStack       * aErrorStack );

stlStatus ellFuncStatementLocalTime( stlUInt16        aInputArgumentCnt,
                                     dtlValueEntry  * aInputArgument,
                                     void           * aResultValue,
                                     void           * aInfo,
                                     dtlFuncVector  * aVectorFunc,
                                     void           * aVectorArg,
                                     void           * aEnv );


/* TransactionLocalTime */
stlStatus ellGetFuncInfoTransactionLocalTime( stlUInt16               aDataTypeArrayCount,
                                              stlBool               * aIsConstantData,
                                              dtlDataType           * aDataTypeArray,
                                              dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                              stlUInt16               aFuncArgDataTypeArrayCount,
                                              dtlDataType           * aFuncArgDataTypeArray,
                                              dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                              dtlDataType           * aFuncResultDataType,
                                              dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                              stlUInt32             * aFuncPtrIdx,
                                              dtlFuncVector         * aVectorFunc,
                                              void                  * aVectorArg,
                                              stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrTransactionLocalTime( stlUInt32             aFuncPtrIdx,
                                             dtlBuiltInFuncPtr   * aFuncPtr,
                                             stlErrorStack       * aErrorStack );

stlStatus ellFuncTransactionLocalTime( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );


/* ClockLocalTimestamp */
stlStatus ellGetFuncInfoClockLocalTimestamp( stlUInt16               aDataTypeArrayCount,
                                             stlBool               * aIsConstantData,
                                             dtlDataType           * aDataTypeArray,
                                             dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                             stlUInt16               aFuncArgDataTypeArrayCount,
                                             dtlDataType           * aFuncArgDataTypeArray,
                                             dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                             dtlDataType           * aFuncResultDataType,
                                             dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                             stlUInt32             * aFuncPtrIdx,
                                             dtlFuncVector         * aVectorFunc,
                                             void                  * aVectorArg,
                                             stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrClockLocalTimestamp( stlUInt32             aFuncPtrIdx,
                                            dtlBuiltInFuncPtr   * aFuncPtr,
                                            stlErrorStack       * aErrorStack );

stlStatus ellFuncClockLocalTimestamp( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv );


/* StatementLocalTimestamp */
stlStatus ellGetFuncInfoStatementLocalTimestamp( stlUInt16               aDataTypeArrayCount,
                                                 stlBool               * aIsConstantData,
                                                 dtlDataType           * aDataTypeArray,
                                                 dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                                 stlUInt16               aFuncArgDataTypeArrayCount,
                                                 dtlDataType           * aFuncArgDataTypeArray,
                                                 dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                                 dtlDataType           * aFuncResultDataType,
                                                 dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                                 stlUInt32             * aFuncPtrIdx,
                                                 dtlFuncVector         * aVectorFunc,
                                                 void                  * aVectorArg,
                                                 stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrStatementLocalTimestamp( stlUInt32             aFuncPtrIdx,
                                                dtlBuiltInFuncPtr   * aFuncPtr,
                                                stlErrorStack       * aErrorStack );

stlStatus ellFuncStatementLocalTimestamp( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );


/* TransactionLocalTimestamp */
stlStatus ellGetFuncInfoTransactionLocalTimestamp( stlUInt16               aDataTypeArrayCount,
                                                   stlBool               * aIsConstantData,
                                                   dtlDataType           * aDataTypeArray,
                                                   dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                                   stlUInt16               aFuncArgDataTypeArrayCount,
                                                   dtlDataType           * aFuncArgDataTypeArray,
                                                   dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                                   dtlDataType           * aFuncResultDataType,
                                                   dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                                   stlUInt32             * aFuncPtrIdx,
                                                   dtlFuncVector         * aVectorFunc,
                                                   void                  * aVectorArg,
                                                   stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrTransactionLocalTimestamp( stlUInt32             aFuncPtrIdx,
                                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                                  stlErrorStack       * aErrorStack );

stlStatus ellFuncTransactionLocalTimestamp( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

/* SysDate */
stlStatus ellGetFuncInfoSysDate( stlUInt16               aDataTypeArrayCount,
                                 stlBool               * aIsConstantData,
                                 dtlDataType           * aDataTypeArray,
                                 dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                 stlUInt16               aFuncArgDataTypeArrayCount,
                                 dtlDataType           * aFuncArgDataTypeArray,
                                 dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                 dtlDataType           * aFuncResultDataType,
                                 dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                 stlUInt32             * aFuncPtrIdx,
                                 dtlFuncVector         * aVectorFunc,
                                 void                  * aVectorArg,                                       
                                 stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrSysDate( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack );

stlStatus ellFuncSysDate( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

/* SysTime */
stlStatus ellGetFuncInfoSysTime( stlUInt16               aDataTypeArrayCount,
                                 stlBool               * aIsConstantData,
                                 dtlDataType           * aDataTypeArray,
                                 dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                 stlUInt16               aFuncArgDataTypeArrayCount,
                                 dtlDataType           * aFuncArgDataTypeArray,
                                 dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                 dtlDataType           * aFuncResultDataType,
                                 dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                 stlUInt32             * aFuncPtrIdx,
                                 dtlFuncVector         * aVectorFunc,
                                 void                  * aVectorArg,                                       
                                 stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrSysTime( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack );

stlStatus ellFuncSysTime( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

/* SysTimestamp */
stlStatus ellGetFuncInfoSysTimestamp( stlUInt16               aDataTypeArrayCount,
                                      stlBool               * aIsConstantData,
                                      dtlDataType           * aDataTypeArray,
                                      dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                      stlUInt16               aFuncArgDataTypeArrayCount,
                                      dtlDataType           * aFuncArgDataTypeArray,
                                      dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                      dtlDataType           * aFuncResultDataType,
                                      dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                      stlUInt32             * aFuncPtrIdx,
                                      dtlFuncVector         * aVectorFunc,
                                      void                  * aVectorArg,                                       
                                      stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrSysTimestamp( stlUInt32             aFuncPtrIdx,
                                     dtlBuiltInFuncPtr   * aFuncPtr,
                                     stlErrorStack       * aErrorStack );

stlStatus ellFuncSysTimestamp( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

/* StatementViewScn */
stlStatus ellGetFuncInfoStatementViewScn( stlUInt16               aDataTypeArrayCount,
                                          stlBool               * aIsConstantData,
                                          dtlDataType           * aDataTypeArray,
                                          dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                          stlUInt16               aFuncArgDataTypeArrayCount,
                                          dtlDataType           * aFuncArgDataTypeArray,
                                          dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                          dtlDataType           * aFuncResultDataType,
                                          dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                          stlUInt32             * aFuncPtrIdx,
                                          dtlFuncVector         * aVectorFunc,
                                          void                  * aVectorArg,
                                          stlErrorStack         * aErrorStack );

stlStatus ellGetFuncPtrStatementViewScn( stlUInt32             aFuncPtrIdx,
                                         dtlBuiltInFuncPtr   * aFuncPtr,
                                         stlErrorStack       * aErrorStack );

stlStatus ellFuncStatementViewScn( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   void           * aInfo,
                                   dtlFuncVector  * aVectorFunc,
                                   void           * aVectorArg,
                                   void           * aEnv );

/** @} ellFunc */

#endif /* _ELLSESSIONFUNC_H_ */
