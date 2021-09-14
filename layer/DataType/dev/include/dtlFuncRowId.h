/*******************************************************************************
 * dtlFuncRowId.h
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

#ifndef _DTL_FUNC_ROWID_H_
#define _DTL_FUNC_ROWID_H_ 1


/**
 * @file dtlFuncRowId.h
 * @brief DataType Layer Function 함수 
 */

/*
 * ROWID_OBJECT_ID
 */

stlStatus dtlRowIdObjectId( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv );

stlStatus dtlGetFuncInfoRowIdObjectId( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrRowIdObjectId( stlUInt32             aFuncPtrIdx,
                                      dtlBuiltInFuncPtr   * aFuncPtr,
                                      stlErrorStack       * aErrorStack );


/*
 * ROWID_PAGE_ID
 */ 

stlStatus dtlRowIdPageId( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlGetFuncInfoRowIdPageId( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrRowIdPageId( stlUInt32             aFuncPtrIdx,
                                    dtlBuiltInFuncPtr   * aFuncPtr,
                                    stlErrorStack       * aErrorStack );

/*
 * ROWID_ROW_NUMBER
 */

stlStatus dtlRowIdRowNumber( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv );

stlStatus dtlGetFuncInfoRowIdRowNumber( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrRowIdRowNumber( stlUInt32             aFuncPtrIdx,
                                       dtlBuiltInFuncPtr   * aFuncPtr,
                                       stlErrorStack       * aErrorStack );

/*
 * ROWID_TABLESPACE_ID
 */ 

stlStatus dtlRowIdTablespaceId( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );

stlStatus dtlGetFuncInfoRowIdTablespaceId( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrRowIdTablespaceId( stlUInt32             aFuncPtrIdx,
                                          dtlBuiltInFuncPtr   * aFuncPtr,
                                          stlErrorStack       * aErrorStack );

#endif /* _DTL_FUNC_ROWID_H_ */
