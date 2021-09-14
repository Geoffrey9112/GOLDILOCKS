/*******************************************************************************
 * ztpwPrecomp.h
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


#ifndef _ZTPWPRECOMP_H_
#define _ZTPWPRECOMP_H_ 1

#include <dtl.h>

/**
 * @file ztpwPrecomp.h
 * @brief Gliese Embedded SQL in C precompiler write C code functions
 */

/**
 * @defgroup ztpwPrecomp Gliese Embedded SQL in C precompiler write C code functions
 * @ingroup ztp
 * @{
 */

#define ZTP_C_HOSTVAR_ARRAY_NAME_BASIC           ( "sqlhv" )
#define ZTP_C_HOSTVAR_ARRAY_NAME_DYNAMIC_USING   ( "dynusing" )
#define ZTP_C_HOSTVAR_ARRAY_NAME_DYNAMIC_INTO    ( "dyninto" )
#define ZTP_C_HOSTVAR_ARRAY_NAME_FETCH_OFFSET    ( "offsethv" )

/************************************************************************
 * Write C code
 ************************************************************************/

stlStatus ztpwSendStringToOutFile(ztpCParseParam *aParam,
                                  stlChar        *aStr);
stlStatus ztpwBypassCCode(ztpCParseParam *aParam);
stlStatus ztpwBypassCCodeByLoc(ztpCParseParam *aParam,
                               stlInt32        aStartLoc,
                               stlInt32        aEndLoc);

stlStatus ztpwPrecompSourceLine(ztpCParseParam *aParam,
                                stlInt32        aLine,
                                stlChar        *aInFileName,
                                stlChar        *aOutFileName);
stlStatus ztpwPrecompBlockBegin(ztpCParseParam *aParam);
stlStatus ztpwPrecompBlockEnd(ztpCParseParam *aParam);
stlStatus ztpwPrecompException(ztpCParseParam *aParam,
                               stlBool         aApplyNotFound);

stlStatus ztpwDeclareSqlargs( ztpCParseParam  *aParam );

stlStatus ztpwSetSqlargs( ztpCParseParam    * aParam,
                          stlChar           * aSqlca,
                          stlChar           * aSqlState,
                          stlChar           * aSqlStmt,
                          zlplStatementType   aStatementType );

stlStatus ztpwDeclareHostVariables( ztpCParseParam  * aParam,
                                    stlChar         * aCHostArrayName,
                                    ztpCVariable    * aHostVarList );

stlStatus ztpwDeclareDynamicParameter( ztpCParseParam  * aParam,
                                       stlBool           aIsUsing,
                                       stlChar         * aCHostArrayName,
                                       ztpDynamicParam * aDynParam );

stlStatus ztpwPrecompHostVariable( ztpCParseParam   * aParam,
                                   stlChar         * aCHostArrayName,
                                   ztpCVariable     * aHostVarList );

stlStatus ztpwSetOpenCursor( ztpCParseParam          * aParam,
                             stlChar                 * aCursorName,
                             zlplCursorOriginType      aOriginType,
                             stlChar                 * aStmtName,
                             zlplCursorStandardType    aStandardType,
                             zlplCursorSensitivity     aSensitivity,
                             zlplCursorScrollability   aScrollability,
                             zlplCursorHoldability     aHoldability,
                             zlplCursorUpdatability    aUpdatability,
                             zlplCursorReturnability   aReturnability );

stlStatus ztpwSetFetchCursor( ztpCParseParam * aParam,
                              stlChar        * aCursorName,
                              ztpFetchOrient * aFetchOrient );

stlStatus ztpwSetCloseCursor( ztpCParseParam * aParam,
                              stlChar        * aCursorName );

stlStatus ztpwSetPositionedCursor( ztpCParseParam * aParam,
                                   stlChar        * aCursorName );

stlStatus ztpwCalcFileLineCount(ztpCParseParam *aParam,
                                stlFile        *aFile,
                                stlInt32       *aLineCount);

/** @} */

#endif /* _ZTPWPRECOMP_H_ */
