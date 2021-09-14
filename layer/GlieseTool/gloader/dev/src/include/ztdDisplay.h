/*******************************************************************************
 * ztdDisplay.h
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ZTDDISPLAY_H_
#define _ZTDDISPLAY_H_ 1


/**
 * @file ztdDisplay.h
 * @brief Display  Definition
 */

/**
 * @defgroup ztdDisplay Display
 * @ingroup ztd
 * 
 * @{
 */

stlStatus ztdEnableColoredText( stlErrorStack * aErrorStack );

stlInt32 ztdPaintf( stlPaintAttr * aAttr, const stlChar *aFormat, ... );

void ztdPrintErrorStack( stlErrorStack * aErrorStack );

stlStatus ztdPrintDiagnostic(SQLSMALLINT        aHandleType,
                             SQLHANDLE          aHandle,
                             stlErrorStack    * aErrorStack );

stlStatus ztdWriteDiagnostic(SQLSMALLINT        aHandleType,
                             SQLHANDLE          aHandle,
                             stlBool            aIsPrintRecordNumber,
                             stlInt64           aRecCount,
                             ztdFileAndBuffer * aFileAndBuffer,
                             stlErrorStack    * aErrorStack );

stlStatus ztdCheckError( SQLHENV            aEnvHandle,
                         SQLHDBC            aDbcHandle,
                         SQLHSTMT           aStmHandle,
                         stlInt64           aRecCount,
                         ztdFileAndBuffer * aFileAndBuffer,
                         stlSemaphore     * aSemaphore,
                         stlErrorStack    * aErrorStack );
/** @} */

#endif /* _ZTDDISPLAY_H_ */
