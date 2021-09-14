/*******************************************************************************
 * ztpConvertExecSql.h
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


#ifndef _ZTPCONVERTEXECSQL_H_
#define _ZTPCONVERTEXECSQL_H_ 1

/**
 * @file ztpConvertExecSql.h
 * @brief Command Parser for Gliese SQL 
 */

/**
 * @defgroup ztpConvertExecSql Translate Embedded SQL to C function
 * @ingroup ztp
 * @{
 */

typedef stlStatus (*ztpCvtSQLFunc)(ztpCParseParam *aParam);
extern  ztpCvtSQLFunc gCvtSQLFunc[];

stlStatus ztpConvertExecSqlIgnore( ztpCParseParam * aParam );

stlStatus ztpConvertExecSqlTransaction(ztpCParseParam *aParam);
stlStatus ztpConvertExecSqlBypass(ztpCParseParam *aParam);
stlStatus ztpConvertExecSqlSetAutocommit(ztpCParseParam *aParam);
stlStatus ztpConvertExecSqlDeclareCursor(ztpCParseParam *aParam);
stlStatus ztpConvertExecSqlOpenCursor(ztpCParseParam *aParam);
stlStatus ztpConvertExecSqlFetchCursor(ztpCParseParam *aParam);
stlStatus ztpConvertExecSqlCloseCursor(ztpCParseParam *aParam);
stlStatus ztpConvertExecSqlPositionedCursorDML( ztpCParseParam *aParam );
stlStatus ztpConvertExecSqlIncludeToC(ztpCParseParam *aParam);
stlStatus ztpConvertExecSqlException(ztpCParseParam *aParam);
stlStatus ztpConvertExecSqlConnect(ztpCParseParam *aParam);
stlStatus ztpConvertExecSqlDisconnect(ztpCParseParam *aParam);
stlStatus ztpConvertExecSqlContextAllocate(ztpCParseParam *aParam);
stlStatus ztpConvertExecSqlContextFree(ztpCParseParam *aParam);
stlStatus ztpConvertExecSqlContextUse(ztpCParseParam *aParam);
stlStatus ztpConvertExecSqlContextUseDefault(ztpCParseParam *aParam);
stlStatus ztpConvertExecDynSqlExecuteImmediate( ztpCParseParam * aParam );
stlStatus ztpConvertExecDynSqlPrepare( ztpCParseParam * aParam );
stlStatus ztpConvertExecDynSqlExecute( ztpCParseParam * aParam );

/** @} */

#endif /* _ZTPCONVERTEXECSQL_H_ */
