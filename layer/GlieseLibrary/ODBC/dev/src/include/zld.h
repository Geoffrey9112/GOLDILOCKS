/*******************************************************************************
 * zld.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zld.h 12690 2014-06-02 04:00:21Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLD_H_
#define _ZLD_H_ 1

/**
 * @file zld.h
 * @brief Gliese API Internal Diagnostic/Descriptor Routines.
 */

/**
 * @defgroup zld Internal Diagnostic/Descriptor Routines
 * @{
 */

stlStatus zldDiagInit( stlInt16        aHandleType,
                       void          * aHandle,
                       stlErrorStack * aErrorStack );

stlStatus zldDiagFini( stlInt16        aHandleType,
                       void          * aHandle,
                       stlErrorStack * aErrorStack );

stlStatus zldDiagAdds( stlInt16        aHandleType,
                       void          * aHandle,
                       stlInt64        aRowNumber,
                       stlInt32        aColumnNumber,
                       stlErrorStack * aErrorStack );

stlStatus zldDiagClear( stlInt16        aHandleType,
                        void          * aHandle,
                        stlErrorStack * aErrorStack );

void zldDiagSetRetCode( stlInt16   aHandleType,
                        void     * aHandle,
                        stlInt16   aRetCode );

void zldDiagSetRowCount( zldDiag  * aDiag,
                         stlInt64   aRowCount );

void zldDiagSetCursorRowCount( zldDiag  * aDiag,
                               stlInt64   aFetchedRowCount );

void zldDiagSetDynamicFunctionCode( zldDiag  * aDiag,
                                    stlInt32   aStatementType );

stlStatus zldDiagGetRec( zldDiag  * aDiag,
                         stlInt16   aRecNumber,
                         stlChar  * aSQLState,
                         stlInt32 * aNativeError,
                         stlChar  * aMessage,
                         stlInt16   aBufferLen,
                         stlInt16 * aTextLen );

stlStatus zldDiagGetRecW( zldDiag         * aDiag,
                          stlInt16          aRecNumber,
                          dtlCharacterSet   aCharacterSet,
                          SQLWCHAR        * aSQLState,
                          stlInt32        * aNativeError,
                          SQLWCHAR        * aMessage,
                          stlInt16          aBufferLen,
                          stlInt16        * aTextLen,
                          stlErrorStack   * aErrorStack );

stlStatus zldDiagGetField( zldDiag  * aDiag,
                           stlInt16   aRecNumber,
                           stlInt16   aDiagId,
                           void     * aDiagInfo,
                           stlInt16   aBufferLen,
                           stlInt16 * aStringLen );

stlStatus zldDiagGetFieldW( zldDiag         * aDiag,
                            stlInt16          aRecNumber,
                            dtlCharacterSet   aCharacterSet,
                            stlInt16          aDiagId,
                            void            * aDiagInfo,
                            stlInt16          aBufferLen,
                            stlInt16        * aStringLen,
                            stlErrorStack   * aErrorStack );

/*
 * Descriptor
 */

stlStatus zldDescAllocRec( zldDesc         * aDesc,
                           stlUInt16         aRecNumber,
                           zldDescElement ** aDescRec,
                           stlErrorStack   * aErrorStack );

stlStatus zldDescReallocRec(zldDesc       * aDesc,
                            stlUInt16       aRecNumber,
                            stlErrorStack * aErrorStack );

stlStatus zldDescFreeRecs( zldDesc       * aDesc,
                           stlErrorStack * aErrorStack );

stlStatus zldDescFindRec( zldDesc         * aDesc,
                          stlUInt16         aRecNumber,
                          zldDescElement ** aDescRec );

stlStatus zldDescSetTargetType( zlsStmt        * aStmt,
                                zldDescElement * aDescRec,
                                stlInt16         aTargetType,
                                stlInt64         aBufferLength );

stlStatus zldDescSetParameterType( zldDescElement * aDescRec,
                                   stlInt16         aParameterType,
                                   stlInt64         aColumnSize,
                                   stlInt16         aDecimalDigits,
                                   stlErrorStack  * aErrorStack );

stlStatus zldDescSetRec( zldDesc       * aDesc,
                         stlInt16        aRecNumber,
                         stlInt16        aType,
                         stlInt16        aSubType,
                         stlInt64        aLength,
                         stlInt16        aPrecision,
                         stlInt16        aScale,
                         void          * aDataPtr,
                         SQLLEN        * aStringLength,
                         SQLLEN        * aIndicator,
                         stlErrorStack * aErrorStack );

stlStatus zldDescGetRec( zldDesc       * aDesc,
                         stlInt16        aRecNumber,
                         stlChar       * aName,
                         stlInt16        aBufferLen,
                         stlInt16      * aStringLen,
                         stlInt16      * aType,
                         stlInt16      * aSubType,
                         stlInt64      * aLength,
                         stlInt16      * aPrecision,
                         stlInt16      * aScale,
                         stlInt16      * aNullable,
                         stlErrorStack * aErrorStack );

stlStatus zldDescSetField( zldDesc       * aDesc,
                           stlInt16        aRecNumber,
                           stlInt16        aFieldId,
                           void          * aValue,
                           stlInt32        aBufferLen,
                           stlErrorStack * aErrorStack );

stlStatus zldDescGetField( zldDesc       * aDesc,
                           stlInt16        aRecNumber,
                           stlInt16        aFieldId,
                           void          * aValue,
                           stlInt32        aBufferLen,
                           stlInt32      * aStringLen,
                           stlErrorStack * aErrorStack );

stlStatus zldDescGetSqlType( dtlDataType            aDBType,
                             stlInt64               aScale,
                             dtlIntervalIndicator   aIntervalCode,
                             stlInt16             * aSqlType,
                             stlInt16             * aSqlConciseType,
                             stlInt16             * aSqlIntervalCode,
                             stlErrorStack        * aErrorStack );

stlStatus zldDescBuildResult( zlsStmt       * aStmt,
                              stlBool         aNeedResultCols,
                              stlErrorStack * aErrorStack );

stlStatus zldDescCheckConsistency( zldDesc        * aDesc,
                                   zldDescElement * aDescRec,
                                   stlErrorStack  * aErrorStack );

/** @} */

#endif /* _ZLD_H_ */
