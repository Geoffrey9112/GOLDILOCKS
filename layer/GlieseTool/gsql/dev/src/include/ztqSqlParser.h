/*******************************************************************************
 * ztqSqlParser.h
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


#ifndef _ZTQSQLPARSER_H_
#define _ZTQSQLPARSER_H_ 1

/**
 * @file ztqSqlParser.h
 * @brief SQL Parser for Gliese SQL 
 */

/**
 * @defgroup ztqSqlParser SQL Parser
 * @ingroup ztq
 * @{
 */

YYSTYPE ztqMakeTransactionTree( stlParseParam * aParam, stlBool aIsCommit );
YYSTYPE ztqMakeBypassSqlTree( stlParseParam    * aParam,
                              ztqStatementType   aStatementType,
                              ztqObjectType      aObjectType,
                              stlChar          * aObjectName );
YYSTYPE ztqMakeBypassPsmTree( stlParseParam * aParam, ztqStatementType aStmtType, ztqObjectType aObjType );

YYSTYPE ztqAddInHostVariable( stlParseParam * aParam, YYSTYPE aHostVariable, YYSTYPE aHostIndicator );
YYSTYPE ztqAddOutHostVariable( stlParseParam * aParam, YYSTYPE aHostVariable, YYSTYPE aHostIndicator );
void    ztqAddHostVariable( ztqHostVariable * aHostVariableList, ztqHostVariable * aHostVariable );
YYSTYPE ztqMakeHostIndicator( stlParseParam * aParam, YYSTYPE aHostIndicator );

void ztqSetHostVar4OpenCursor( ztqBypassTree * aParseTree, YYSTYPE aCursorName );

YYSTYPE ztqMakeHostVariable( stlParseParam * aParam );

stlStatus ztqSqlAllocator( void          * aContext,
                           stlInt32        aAllocSize,
                           void         ** aAddr,
                           stlErrorStack * aErrorStack );

stlStatus ztqSqlParseIt( stlChar         * aSqlString,
                         stlAllocator    * aAllocator,
                         ztqParseTree   ** aParseTree,
                         stlErrorStack   * aErrorStack );

void ztqRaiseParseError( stlParseParam  *aParam,
                         stlInt32        aPosition );

stlChar * ztqMakeErrPosString( stlChar * aSQLString,
                               stlInt32  aPosition );

/** @} */

#endif /* _ZTQSQLPARSER_H_ */
