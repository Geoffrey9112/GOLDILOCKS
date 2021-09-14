/*******************************************************************************
 * zlvCharacterset.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlvCharacterset.h 11187 2014-02-10 07:27:24Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLV_CHARACTERSET_H_
#define _ZLV_CHARACTERSET_H_ 1

/**
 * @file zlvCharacterset.h
 * @brief Gliese API Internal Character Set Routines.
 */

/**
 * @defgroup zlv Internal Convert Routines
 * @{
 */

/*
 * WCHAR
 */
stlSize zlvStrlenW( SQLWCHAR * aString );

stlStatus zlvAllocCharBuffer( SQLWCHAR         * aSrc,
                              stlInt64           aSrcLength,
                              dtlCharacterSet    aCharacterset,
                              SQLCHAR         ** aBuffer,
                              stlInt64         * aBufferLength,
                              stlAllocator     * aAllocator,
                              stlErrorStack    * aErrorStack );

stlStatus zlvWcharToChar( SQLWCHAR         * aSrc,
                          stlInt64           aSrcLength,
                          dtlCharacterSet    aCharacterset,
                          SQLCHAR          * aDest,
                          stlInt64           aDestLength,
                          stlInt64         * aTotalLength,
                          stlErrorStack    * aErrorStack );

stlStatus zlvCharToWchar( dtlCharacterSet    aCharacterset,
                          SQLCHAR          * aSrc,
                          stlInt64           aSrcLength,
                          SQLWCHAR         * aDest,
                          stlInt64           aDestLength,
                          stlInt64         * aTotalLength,
                          stlErrorStack    * aErrorStack );

/*
 * SQL 
 */
stlStatus zlvNonConvertSQL( zlsStmt        * aStmt,
                            stlChar        * aStatementText,
                            stlInt32         aTextLength,
                            stlChar       ** aSQL,
                            stlErrorStack  * aErrorStack );

stlStatus zlvConvertSQL( zlsStmt        * aStmt,
                         stlChar        * aStatementText,
                         stlInt32         aTextLength,
                         stlChar       ** aSQL,
                         stlErrorStack  * aErrorStack );

/*
 * Result Set
 */
stlStatus zlvNonConvertResultSet( zlsStmt        * aStmt,
                                  SQLLEN         * aOffset,
                                  dtlDataValue   * aDataValue,
                                  void           * aTargetValuePtr,
                                  stlInt64         aTargetLength,
                                  SQLLEN         * aIndicator,
                                  stlErrorStack  * aErrorStack );

stlStatus zlvConvertResultSet( zlsStmt        * aStmt,
                               SQLLEN         * aOffset,
                               dtlDataValue   * aDataValue,
                               void           * aTargetValuePtr,
                               stlInt64         aTargetLength,
                               SQLLEN         * aIndicator,
                               stlErrorStack  * aErrorStack );

/*
 * Error Message
 */
stlStatus zlvNonConvertError( zlsStmt        * aStmt,
                              stlErrorData   * aErrorData,
                              stlChar        * aExtraMessage,
                              zldDiag        * aDiag,
                              zldDiagElement * aDiagRec,
                              stlErrorStack  * aErrorStack );


stlStatus zlvConvertError( zlsStmt        * aStmt,
                           stlErrorData   * aErrorData,
                           stlChar        * aExtraMessage,
                           zldDiag        * aDiag,
                           zldDiagElement * aDiagRec,
                           stlErrorStack  * aErrorStack );
/** @} */

#endif /* _ZLV_CHARACTERSET_H_ */
