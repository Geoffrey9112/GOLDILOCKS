/*******************************************************************************
 * zli.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zli.h 12354 2014-05-13 06:40:52Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLI_H_
#define _ZLI_H_ 1

/**
 * @file zli.h
 * @brief Gliese API Internal Bind Routines.
 */

/**
 * @defgroup zli Internal Bind Routines
 * @{
 */

void * zliGetDataBuffer( zldDesc        * aDesc,
                         zldDescElement * aDescRec,
                         stlInt64         aNumber );

SQLLEN * zliGetIndicatorBuffer( zldDesc        * aDesc,
                                zldDescElement * aDescRec,
                                stlInt64          aNumber );

stlStatus zliBindCol( zlsStmt       * aStmt,
                      stlUInt16       aColIdx,
                      stlInt16        aCType,
                      void          * aCBuffer,
                      SQLLEN          aBufferLen,
                      SQLLEN        * aInd,
                      stlErrorStack * aErrorStack );

stlStatus zliGetDtlDataType( stlInt16        aSqlType,
                             dtlDataType   * aDtlDataType,
                             stlErrorStack * aErrorStack );

void zliGetDtlDataArgs( zldDescElement * aRec,
                        stlInt64       * aArg1,
                        stlInt64       * aArg2 );

dtlStringLengthUnit zliGetDtlDataUnit( zlsStmt        * aStmt,
                                       zldDescElement * aRec,
                                       stlInt16         aCType,
                                       stlInt16         aSqlType );

dtlIntervalIndicator zliGetDtlDataIndicator( stlInt16 aSqlType );

stlStatus zliBindParameter( zlsStmt       * aStmt,
                            stlUInt16       aParamIdx,
                            stlInt16        aInOutType,
                            stlInt16        aCType,
                            stlInt16        aSQLType,
                            stlUInt64       aColSize,
                            stlInt16        aDigits,
                            void          * aBuffer,
                            SQLLEN          aBufferLen,
                            SQLLEN        * aInd,
                            stlErrorStack * aErrorStack );

stlStatus zliInitParamBlock( zlsStmt              * aStmt,
                             zldDesc              * aIpd,
                             zldDescElement       * aApdRec,
                             zldDescElement       * aIpdRec,
                             dtlDataType          * aDtlDataType,
                             stlInt64             * aArg1,
                             stlInt64             * aArg2,
                             dtlStringLengthUnit  * aStringLengthUnit,
                             dtlIntervalIndicator * aIntervalIndicator,
                             stlErrorStack        * aErrorStack );

stlStatus zliHasDataAtExecIndicator( zlsStmt       * aStmt,
                                     zldDesc       * aApd,
                                     zldDesc       * aIpd,
                                     stlBool       * aHasDataAtExec,
                                     stlErrorStack * aErrorStack );

stlStatus zliReadyDataAtExec( zlsStmt       * aStmt,
                              zldDesc       * aApd,
                              zldDesc       * aIpd,
                              stlBool       * aHasDataAtExec,
                              stlErrorStack * aErrorStack );

stlStatus zliProecessDataAtExec( zlsStmt       * aStmt,
                                 zldDesc       * aApd,
                                 zldDesc       * aIpd,
                                 stlBool       * aHasDataAtExec,
                                 stlErrorStack * aErrorStack );

stlStatus zliPutData( zlsStmt       * aStmt,
                      void          * aDataPtr,
                      SQLLEN          aStrLen_or_Ind,
                      stlErrorStack * aErrorStack );

stlStatus zliSetParamValueIN( zlsStmt       * aStmt,
                              zldDesc       * aApd,
                              zldDesc       * aIpd,
                              stlUInt64       aApdArrayIdx,
                              stlUInt64       aIpdArrayIdx,
                              stlBool       * aParamWithInfo,
                              stlBool       * aParamError,
                              stlErrorStack * aErrorStack );

stlStatus zliSetParamValueOUT( zlsStmt       * aStmt,
                               zldDesc       * aApd,
                               zldDesc       * aIpd,
                               stlUInt64       aApdArrayIdx,
                               stlUInt64       aIpdArrayIdx,
                               stlBool       * aParamWithInfo,
                               stlBool       * aParamError,
                               stlErrorStack * aErrorStack );

stlStatus zliDescribeCol( zlsStmt       * aStmt,
                          stlUInt16       aColumnIdx,
                          stlChar       * aColumnName,
                          stlInt16        aBufferLen,
                          stlInt16      * aNameLen,
                          stlInt16      * aDataType,
                          stlUInt64     * aColumnSize,
                          stlInt16      *  aDecimnalDigits,
                          stlInt16      * aNullable,
                          stlErrorStack * aErrorStack );

stlStatus zliDescribeParam( zlsStmt       * aStmt,
                            stlUInt16       aParamIdx,
                            stlInt16      * aDataType,
                            stlUInt64     * aParamSize,
                            stlInt16      * aDecimnalDigits,
                            stlInt16      * aNullable,
                            stlErrorStack * aErrorStack );

stlStatus zliNumParams( zlsStmt       * aStmt,
                        stlInt16      * aParameterCount,
                        stlErrorStack * aErrorStack );

stlStatus zliExtendedDescribeParam( zlsStmt       * aStmt,
                                    stlUInt16       aParamIdx,
                                    stlInt16      * aInputOutputType,
                                    stlInt16      * aDataType,
                                    stlUInt64     * aParamSize,
                                    stlInt16      * aDecimnalDigits,
                                    stlInt16      * aNullable,
                                    stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLI_H_ */
