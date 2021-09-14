/*******************************************************************************
 * ztpSqlParser.h
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


#ifndef _ZTPSQLPARSER_H_
#define _ZTPSQLPARSER_H_ 1

#include <zlplDef.h>
#include <ztpDef.h>
#include <ztpSqlLex.h>

/**
 * @file ztpSqlParser.h
 * @brief SQL Parser for Gliese SQL
 */

/**
 * @defgroup ztpSqlParser SQL Parser
 * @ingroup ztp
 * @{
 */

YYSTYPE ztpMakeIgnoreStmt( stlParseParam *aParam );

YYSTYPE ztpMakeTransactionTree( stlParseParam     *aParam,
                                zlplStatementType  aStatementType,
                                stlBool            aIsCommit,
                                stlBool            aIsRelease );

YYSTYPE ztpMakeBypassSqlTree( stlParseParam     *aParam,
                              zlplStatementType  aStmtType,
                              ztpObjectType aObjType );

ztpDeclCursor *ztpMakeDeclareCursorISO(stlParseParam           * aParam,
                                       ztpIdentifier           * aCursorName,
                                       zlplCursorSensitivity     aSensitivity,
                                       zlplCursorScrollability   aScrollability,
                                       zlplCursorHoldability     aHoldability,
                                       stlChar                 * aCursorQuery );

ztpDeclCursor *ztpMakeDynamicCursorISO(stlParseParam           * aParam,
                                       ztpIdentifier           * aCursorName,
                                       zlplCursorSensitivity     aSensitivity,
                                       zlplCursorScrollability   aScrollability,
                                       zlplCursorHoldability     aHoldability,
                                       ztpIdentifier           * aStmtName );

ztpDeclCursor *ztpMakeDeclareCursorODBC(stlParseParam          * aParam,
                                        ztpIdentifier          * aCursorName,
                                        zlplCursorStandardType   aStandardType,
                                        zlplCursorHoldability    aHoldability,
                                        stlChar                * aCursorQuery );

ztpDeclCursor *ztpMakeDynamicCursorODBC(stlParseParam          * aParam,
                                        ztpIdentifier          * aCursorName,
                                        zlplCursorStandardType   aStandardType,
                                        zlplCursorHoldability    aHoldability,
                                        ztpIdentifier          * aStmtName );

ztpOpenCursor * ztpMakeOpenCursor( stlParseParam   * aParam,
                                   ztpIdentifier   * aName,
                                   ztpDynamicParam * aUsingParam );

ztpFetchOrient * ztpMakeFetchOrientation( stlParseParam        * aParam,
                                          zlplFetchOrientation   aFetchOrient,
                                          stlChar              * aOffsetStr,
                                          stlInt32               aOffsetPos,
                                          ztpCVariable         * aOffsetHostVar );

ztpFetchCursor *ztpMakeFetchCursor(stlParseParam  *aParam,
                                   ztpIdentifier  *aName,
                                   ztpFetchOrient *aFetchOrient);

ztpCloseCursor *ztpMakeCloseCursor(stlParseParam *aParam,
                                  ztpIdentifier *aName);

ztpPositionedCursorDML * ztpMakePositionedCursorDML( stlParseParam * aParam,
                                                     ztpIdentifier * aName,
                                                     stlChar       * aCursorDML );

ztpIdentifier *ztpMakeIdentifier(stlParseParam *aParam,
                                 stlChar       *aName,
                                 stlInt32       aNamePos);

/***********************************************************************
 * for Dynamic SQL
 ***********************************************************************/

/*
 * EXECUTE IMMEDIATE
 */
ztpExecuteImmediate * ztpMakeExecuteImmediate( stlParseParam  * aParam,
                                               stlBool          aIsQuoted,
                                               ztpCVariable   * aHostVariable,
                                               stlChar        * aStringSQL );


/*
 * PREPARE
 */
ztpPrepare * ztpMakePrepare( stlParseParam  * aParam,
                             ztpIdentifier  * aStmtName,
                             stlBool          aIsQuoted,
                             ztpCVariable   * aHostVariable,
                             stlChar        * aStringSQL );


/*
 * EXECUTE
 */

ztpDynamicParam * ztpMakeDynamicParam( stlParseParam  * aParam,
                                       stlBool          aIsDesc,
                                       ztpCVariable   * aHostVariable,
                                       stlChar        * aDescName );

ztpExecute * ztpMakeExecute( stlParseParam    * aParam,
                             ztpIdentifier    * aStmtName,
                             ztpDynamicParam  * aUsingHost,
                             ztpDynamicParam  * aIntoHost );

/***********************************************************************
 * for Host Variable
 ***********************************************************************/

stlChar * ztpMakeSignedNumericString( stlParseParam * aParam,
                                      stlInt32        aStartStrPos,
                                      stlInt32        aEndStrPos );

ztpHostVariable *ztpMakeHostVariableInSql( stlParseParam *aParam,
                                           stlInt32       aStartPos,
                                           stlInt32      *aEndPos );

ztpCVariable *ztpAddHostVariable( stlParseParam   *aParam,
                                  ztpHostVariable *aHostVariableDesc,
                                  ztpCVariable    *aHostIndicator,
                                  stlInt32         aStartPos,
                                  stlBool          aIn );
void    ztpAddHostVariableList( ztpCVariable * aHostVariableList, ztpCVariable * aHostVariable );
ztpCVariable *ztpMakeHostIndicator( stlParseParam   *aParam,
                                    ztpHostVariable *aHostIndicator,
                                    stlInt32         aIndStartPos,
                                    stlInt32         aHostVarStartPos );

ztpNameTag *ztpMakeNameTag( stlParseParam    *aParam,
                            ztpNameType       aType,
                            void             *aHostVarDesc,
                            stlInt32          aHostVarDescPos );

stlChar *ztpMakeStaticConnStringInSql( stlParseParam   *aParam,
                                       stlChar         *aConnName );

ztpConnectParam *ztpMakeConnectParam(stlParseParam   *aParam,
                                     ztpNameTag      *aUserName,
                                     ztpNameTag      *aPassword);
ztpConnectParam *ztpSetConnectParam(stlParseParam   *aParam,
                                    ztpConnectParam *aConnectParam,
                                    ztpNameTag      *aConnName,
                                    ztpNameTag      *aConnString);
ztpDisconnectParam *ztpMakeDisconnectParam(stlParseParam   *aParam,
                                           ztpNameTag      *aConnectionObject,
                                           stlBool          aIsAll);

stlStatus ztpSqlAllocator( void          * aContext,
                           stlInt32        aAllocSize,
                           void         ** aAddr,
                           stlErrorStack * aErrorStack );

stlStatus ztpSqlParseIt( stlChar         * aSqlString,
                         stlInt32        * aEndPos,
                         stlAllocator    * aAllocator,
                         ztpParseTree   ** aParseTree,
                         stlErrorStack   * aErrorStack );

/** @} */

#endif /* _ZTPSQLPARSER_H_ */
