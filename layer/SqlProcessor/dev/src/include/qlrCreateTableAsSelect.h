/*******************************************************************************
 * qlrCreateTableAsSelect.h
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

#ifndef _QLR_CREATE_TABLE_AS_SELECT_H_
#define _QLR_CREATE_TABLE_AS_SELECT_H_ 1

/**
 * @file qlrCreateTableAsSelect.h
 * @brief CREATE TABLE AS SELECT
 */

/**
 * @brief CREATE TABLE AS SELECT 를 위한 Code Plan
 */

struct qlrCodeCreateTable
{
    
    /*
     * For CREATE TABLE AS SELECT 
     * INSERT SELECT 수행을 위한 Code Plan 정보  
     */

    qllOptNodeList       * mOptNodeList;
    qloInitStmtExprList  * mStmtExprList;
    qllOptimizationNode  * mSubQueryOptNode;
};


stlStatus qlrValidateCreateTableAsSelect( smlTransId      aTransID,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          stlChar       * aSQLString,
                                          qllNode       * aParseTree,
                                          qllEnv        * aEnv );

stlStatus qlrOptCodeCreateTableAsSelect( smlTransId      aTransID,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv );

stlStatus qlrOptDataCreateTableAsSelect( smlTransId      aTransID,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv );

stlStatus qlrExecuteCreateTableAsSelect( smlTransId      aTransID,
                                         smlStatement  * aStmt,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv );


/*
 * Validation 보조 함수
 */

stlStatus qlrMakeColumnDefinition( qlvStmtInfo             * aStmtInfo,
                                   ellDictHandle           * aSchemaHandle,
                                   smlRelationType           aRelationType,
                                   stlChar                 * aTableName,
                                   qlpList                 * aColumnNameList,
                                   qllTarget               * aTargetInfo,
                                   qlvInitNode             * aSubquery,
                                   stlInt32                * aColumnCount,
                                   qlrInitColumn          ** aColumnList,
                                   qlrInitTableConstraint  * aAllConstraints,
                                   qllEnv                  * aEnv );

stlStatus qlrExistNotNullConst( qlvStmtInfo          * aStmtInfo,
                                qlvInitExpression    * aTargetExpr,
                                qllTarget            * aTargetInfo,
                                stlBool              * aExistNotNullConst,
                                qllEnv               * aEnv );

stlStatus qlrMakeNotNullConstraint( qlvStmtInfo            * aStmtInfo,
                                    ellDictHandle          * aSchemaHandle,
                                    stlChar                * aTableName,
                                    stlChar                * aColumnName,
                                    qlvInitTarget          * aTarget,
                                    stlInt32                 aColumnIdx,
                                    qlrInitTableConstraint * aAllConstraint,
                                    qllEnv                 * aEnv );

#endif /* _QLR_CREATE_TABLE_AS_SELECT_H_ */
