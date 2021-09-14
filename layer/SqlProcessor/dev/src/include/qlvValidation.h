/*******************************************************************************
 * qlvValidation.h
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

#ifndef _QLVVALIDATION_H_
#define _QLVVALIDATION_H_ 1

/**
 * @file qlvValidation.h
 * @brief SQL Processor Layer Validation 
 */

#include <qlDef.h>

/**
 * @brief WAIT interval 범위
 */

#define QLV_MIN_WAIT_INTERVAL    (0)
#define QLV_MAX_WAIT_INTERVAL    (1000000000)


/*******************************************************************************
 * Rewrite Functions
 ******************************************************************************/

stlStatus qlvRewriteExpr( qlvInitExpression   * aInitExpr,
                          dtlIterationTime      aParentIterTime,
                          qlvInitExprList     * aInitExprList,
                          qlvInitExpression  ** aInitNewExpr,
                          knlRegionMem        * aRegionMem,
                          qllEnv              * aEnv );


typedef stlStatus (*qlvRewriteFuncPtr) ( qlvInitExpression   * aInitExpr,
                                         dtlIterationTime      aParentIterTime,
                                         qlvInitExprList     * aInitExprList,
                                         qlvInitExpression  ** aInitNewExpr,
                                         knlRegionMem        * aRegionMem,
                                         qllEnv              * aEnv );


stlStatus qlvRewriteValue( qlvInitExpression   * aInitExpr,
                           dtlIterationTime      aParentIterTime,
                           qlvInitExprList     * aInitExprList,
                           qlvInitExpression  ** aInitNewExpr,
                           knlRegionMem        * aRegionMem,
                           qllEnv              * aEnv );


stlStatus qlvRewriteBindParam( qlvInitExpression   * aInitExpr,
                               dtlIterationTime      aParentIterTime,
                               qlvInitExprList     * aInitExprList,
                               qlvInitExpression  ** aInitNewExpr,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv );

stlStatus qlvRewriteColumn( qlvInitExpression   * aInitExpr,
                            dtlIterationTime      aParentIterTime,
                            qlvInitExprList     * aInitExprList,
                            qlvInitExpression  ** aInitNewExpr,
                            knlRegionMem        * aRegionMem,
                            qllEnv              * aEnv );

stlStatus qlvRewriteFunction( qlvInitExpression   * aInitExpr,
                              dtlIterationTime      aParentIterTime,
                              qlvInitExprList     * aInitExprList,
                              qlvInitExpression  ** aInitNewExpr,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv );

stlStatus qlvRewriteCast( qlvInitExpression   * aInitExpr,
                          dtlIterationTime      aParentIterTime,
                          qlvInitExprList     * aInitExprList,
                          qlvInitExpression  ** aInitNewExpr,
                          knlRegionMem        * aRegionMem,
                          qllEnv              * aEnv );

stlStatus qlvRewritePseudoColumn( qlvInitExpression   * aInitExpr,
                                  dtlIterationTime      aParentIterTime,
                                  qlvInitExprList     * aInitExprList,
                                  qlvInitExpression  ** aInitNewExpr,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv );

stlStatus qlvRewritePseudoArgs( qlvInitExpression   * aInitExpr,
                                dtlIterationTime      aParentIterTime,
                                qlvInitExprList     * aInitExprList,
                                qlvInitExpression  ** aInitNewExpr,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv );

stlStatus qlvRewriteConstExpr( qlvInitExpression   * aInitExpr,
                               dtlIterationTime      aParentIterTime,
                               qlvInitExprList     * aInitExprList,
                               qlvInitExpression  ** aInitNewExpr,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv );

stlStatus qlvRewriteReference( qlvInitExpression   * aInitExpr,
                               dtlIterationTime      aParentIterTime,
                               qlvInitExprList     * aInitExprList,
                               qlvInitExpression  ** aInitNewExpr,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv );


stlStatus qlvRewriteSubQuery( qlvInitExpression   * aInitExpr,
                              dtlIterationTime      aParentIterTime,
                              qlvInitExprList     * aInitExprList,
                              qlvInitExpression  ** aInitNewExpr,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv );

stlStatus qlvRewriteInnerColumn( qlvInitExpression   * aInitExpr,
                                 dtlIterationTime      aParentIterTime,
                                 qlvInitExprList     * aInitExprList,
                                 qlvInitExpression  ** aInitNewExpr,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv );

stlStatus qlvRewriteOuterColumn( qlvInitExpression   * aInitExpr,
                                 dtlIterationTime      aParentIterTime,
                                 qlvInitExprList     * aInitExprList,
                                 qlvInitExpression  ** aInitNewExpr,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv );

stlStatus qlvRewriteRowIdColumn( qlvInitExpression   * aInitExpr,
                                 dtlIterationTime      aParentIterTime,
                                 qlvInitExprList     * aInitExprList,
                                 qlvInitExpression  ** aInitNewExpr,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv );

stlStatus qlvRewriteAggregation( qlvInitExpression   * aInitExpr,
                                 dtlIterationTime      aParentIterTime,
                                 qlvInitExprList     * aInitExprList,
                                 qlvInitExpression  ** aInitNewExpr,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv );

stlStatus qlvRewriteCaseExpr( qlvInitExpression   * aInitExpr,
                              dtlIterationTime      aParentIterTime,
                              qlvInitExprList     * aInitExprList,
                              qlvInitExpression  ** aInitNewExpr,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv );

stlStatus qlvRewriteListFunction( qlvInitExpression   * aInitExpr,
                                  dtlIterationTime      aParentIterTime,
                                  qlvInitExprList     * aInitExprList,
                                  qlvInitExpression  ** aInitNewExpr,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv );

stlStatus qlvRewriteListColumn( qlvInitExpression   * aInitExpr,
                                dtlIterationTime      aParentIterTime,
                                qlvInitExprList     * aInitExprList,
                                qlvInitExpression  ** aInitNewExpr,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv );

stlStatus qlvRewriteRowExpr( qlvInitExpression   * aInitExpr,
                             dtlIterationTime      aParentIterTime,
                             qlvInitExprList     * aInitExprList,
                             qlvInitExpression  ** aInitNewExpr,
                             knlRegionMem        * aRegionMem,
                             qllEnv              * aEnv );

/*******************************************************************************
 * SELECT
 ******************************************************************************/

/**
 * @defgroup qlvSelect SELECT Validation
 * @ingroup qlv
 * @{
 */


stlStatus qlvValidateSelect( smlTransId      aTransID,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             stlChar       * aSQLString,
                             qllNode       * aParseTree,
                             qllEnv        * aEnv );

stlStatus qlvValidateTarget( qlvStmtInfo       * aStmtInfo,
                             ellObjectList     * aValidationObjList,
                             qlvRefTableList   * aRefTableList,
                             stlInt32            aAllowedAggrDepth,
                             qlpTarget         * aTargetSource,
                             qlvInitTarget     * aTargetCode,
                             dtlIterationTime    aIterationTimeScope,
                             dtlIterationTime    aIterationTime,
                             qllEnv            * aEnv );

/** @} qlvSelect */




/**
 * @defgroup qlvSelectInto SELECT INTO Validation
 * @ingroup qlv
 * @{
 */


stlStatus qlvValidateSelectInto( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv );

/** @} qlvSelectInto */



/*******************************************************************************
 * INSERT 
 ******************************************************************************/

/**
 * @defgroup qlvInsert INSERT VALUES Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateInsertValues( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv );


/** @} qlvInsert */

/**
 * @defgroup qlvInsertSelect INSERT SELECT Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateInsertSelect( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv );


/** @} qlvInsertSelect */


/**
 * @defgroup qlvInsertReturnInto INSERT RETURNING INTO Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateInsertReturnInto( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       stlChar       * aSQLString,
                                       qllNode       * aParseTree,
                                       qllEnv        * aEnv );


/** @} qlvInsertReturnInto */


/**
 * @defgroup qlvInsertReturnQuery INSERT RETURNING Query Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateInsertReturnQuery( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv );


/** @} qlvInsertReturnInto */



/*******************************************************************************
 * UPDATE 
 ******************************************************************************/

/**
 * @defgroup qlvUpdate UPDATE Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateUpdate( smlTransId      aTransID,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             stlChar       * aSQLString,
                             qllNode       * aParseTree,
                             qllEnv        * aEnv );

/** @} qlvUpdate */


/**
 * @defgroup qlvUpdateReturnInto UPDATE RETURNING INTO Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateUpdateReturnInto( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       stlChar       * aSQLString,
                                       qllNode       * aParseTree,
                                       qllEnv        * aEnv );


/** @} qlvUpdateReturnInto */


/**
 * @defgroup qlvUpdateReturnQuery UPDATE RETURNING Query Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateUpdateReturnQuery( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv );


/** @} qlvUpdateReturnQuery */


/**
 * @defgroup qlvUpdatePositioned UPDATE CURRENT OF Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateUpdatePositioned( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       stlChar       * aSQLString,
                                       qllNode       * aParseTree,
                                       qllEnv        * aEnv );

/** @} qlvUpdatePositioned */


/*******************************************************************************
 * DELETE 
 ******************************************************************************/

/**
 * @defgroup qlvDelete DELETE Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateDelete( smlTransId      aTransID,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             stlChar       * aSQLString,
                             qllNode       * aParseTree,
                             qllEnv        * aEnv );

/** @} qlvDelete */



/**
 * @defgroup qlvDeleteReturnInto DELETE RETURNING INTO Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateDeleteReturnInto( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       stlChar       * aSQLString,
                                       qllNode       * aParseTree,
                                       qllEnv        * aEnv );

/** @} qlvDeleteReturnInto */

/**
 * @defgroup qlvDeleteReturnQuery DELETE RETURNING Query Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateDeleteReturnQuery( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv );

/** @} qlvDeleteReturnQuery */


/**
 * @defgroup qlvDeletePositioned DELETE CURRENT OF Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateDeletePositioned( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       stlChar       * aSQLString,
                                       qllNode       * aParseTree,
                                       qllEnv        * aEnv );

/** @} qlvDeletePositioned */


/*******************************************************************************
 * COLUMN 
 ******************************************************************************/

/**
 * @defgroup qlvColumn Column Validation
 * @ingroup qlv
 * @{
 */


stlStatus qlvValidateColumnRef( qlvStmtInfo         * aStmtInfo,
                                qlvExprPhraseType     aPhraseType,
                                ellObjectList       * aValidationObjList,
                                qlvRefTableList     * aRefTableList,
                                stlBool               aIsAtomic,
                                stlInt32              aAllowedAggrDepth,
                                qlpColumnRef        * aColumn,
                                qlvInitExpression  ** aExprCode,
                                stlBool             * aHasOuterJoinOperator,
                                stlBool             * aIsUpdatable,
                                knlBuiltInFunc        aRelatedFuncId,
                                dtlIterationTime      aIterationTimeScope,
                                dtlIterationTime      aIterationTime,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv );

stlStatus qlvValidateColumnName( qlvStmtInfo        * aStmtInfo,
                                 qlvExprPhraseType    aPhraseType,        
                                 qlvRefTableList    * aRefTableList,
                                 qlpColumnName      * aColumnNameSource,
                                 stlBool              aIsSetOuterMark,
                                 qlpPseudoCol      ** aMaySequence,
                                 qlvInitExpression ** aColumnExpr,
                                 stlBool            * aIsUpdatable,
                                 knlBuiltInFunc       aRelatedFuncId,
                                 dtlIterationTime     aIterationTime,
                                 knlRegionMem       * aRegionMem,
                                 qllEnv             * aEnv );

stlStatus qlvSetColumnType( qlvStmtInfo      * aStmtInfo,
                            qlvInitTypeDef   * aCodeTypeDef,
                            qlpTypeName      * aColumnType,
                            qllEnv           * aEnv );

stlStatus qlvSetColumnTypeForCast( qlvStmtInfo      * aStmtInfo,
                                   qlvInitTypeDef   * aCodeTypeDef,
                                   qlpTypeName      * aColumnType,
                                   qllEnv           * aEnv );
    
// stlStatus qlvValidateColumnDef( smlTransId      aTransID,
//                                 qllStatement  * aSQLStmt,
//                                 qllNode       * aParseTree,
//                                 qllEnv        * aEnv );

// stlStatus qlvValidateCharLength( smlTransId      aTransID,
//                                  qllStatement  * aSQLStmt,
//                                  qllNode       * aParseTree,
//                                  qllEnv        * aEnv );

/** @} qlvColumn */


/*******************************************************************************
 * OBJECT
 ******************************************************************************/

/**
 * @defgroup qlvObject SQL-Object Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateSequenceRef( qlvStmtInfo         * aStmtInfo,
                                  ellObjectList       * aValidationObjList,
                                  qlpObjectName       * aSeqName,
                                  qlvInitExpression  ** aSeqExprCode,
                                  knlBuiltInFunc        aRelatedFuncId,
                                  dtlIterationTime      aIterationTimeScope,
                                  dtlIterationTime      aIterationTime,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv );

/** @} qlvObject */

/*******************************************************************************
 * FUNCTION
 ******************************************************************************/

/**
 * @defgroup qlvFunction Function Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateFunction( qlvStmtInfo         * aStmtInfo,
                               qlvExprPhraseType     aPhraseType,
                               ellObjectList       * aValidationObjList,
                               qlvRefTableList     * aRefTableList,
                               stlBool               aIsAtomic,
                               stlInt32              aAllowedAggrDepth,
                               qlpFunction         * aFunction,
                               qlvInitExpression  ** aExprCode,
                               stlBool             * aHasOuterJoinOperator,
                               knlBuiltInFunc        aRelatedFuncId,
                               dtlIterationTime      aIterationTimeScope,
                               dtlIterationTime      aIterationTime,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv );

stlStatus qlvValidateCast( qlvStmtInfo         * aStmtInfo,
                           qlvExprPhraseType     aPhraseType,
                           ellObjectList       * aValidationObjList,
                           qlvRefTableList     * aRefTableList,
                           stlBool               aIsAtomic,
                           stlInt32              aAllowedAggrDepth,
                           qlpTypeCast         * aFunction,
                           qlvInitExpression  ** aExprCode,
                           stlBool             * aHasOuterJoinOperator,
                           knlBuiltInFunc        aRelatedFuncId,
                           dtlIterationTime      aIterationTimeScope,
                           dtlIterationTime      aIterationTime,
                           knlRegionMem        * aRegionMem,
                           qllEnv              * aEnv );

// stlStatus qlvValidateTypeCast( smlTransId      aTransID,
//                                qllStatement  * aSQLStmt,
//                                qllNode       * aParseTree,
//                                qllEnv        * aEnv );

stlStatus qlvValidateCaseExpr( qlvStmtInfo         * aStmtInfo,
                               qlvExprPhraseType     aPhraseType,
                               ellObjectList       * aValidationObjList,
                               qlvRefTableList     * aRefTableList,
                               stlBool               aIsAtomic,
                               stlInt32              aAllowedAggrDepth,
                               qlpCaseExpr         * aCaseSource,
                               qlvInitExpression  ** aExprCode,
                               stlBool             * aHasOuterJoinOperator,
                               knlBuiltInFunc        aRelatedFuncId,
                               dtlIterationTime      aIterationTimeScope,
                               dtlIterationTime      aIterationTime,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv );

/** @} qlvFunction */


/*******************************************************************************
 * LIST FUNCTION
 ******************************************************************************/

/**
 * @defgroup qlvFunction Function Validation
 * @ingroup qlv
 * @{
 */
stlStatus qlvValidateListFunction( qlvStmtInfo         * aStmtInfo,
                                   qlvExprPhraseType     aPhraseType,
                                   ellObjectList       * aValidationObjList,
                                   qlvRefTableList     * aRefTableList,
                                   stlBool               aIsAtomic,
                                   stlInt32              aAllowedAggrDepth,
                                   qlpListFunc         * aListSource,
                                   qlvInitExpression  ** aExprCode,
                                   stlBool             * aHasOuterJoinOperator,
                                   knlBuiltInFunc        aRelatedFuncId,
                                   dtlIterationTime      aIterationTimeScope,
                                   dtlIterationTime      aIterationTime,
                                   knlRegionMem        * aRegionMem,
                                   qllEnv              * aEnv );


stlStatus qlvValidateListColumn( qlvStmtInfo         * aStmtInfo,
                                 qlvExprPhraseType     aPhraseType,
                                 ellObjectList       * aValidationObjList,
                                 qlvRefTableList     * aRefTableList,
                                 stlBool               aIsAtomic,
                                 stlInt32              aAllowedAggrDepth,
                                 qlpListCol          * aListSource,
                                 qlvInitExpression  ** aExprCode,
                                 stlBool             * aHasOuterJoinOperator,
                                 knlBuiltInFunc        aRelatedFuncId,
                                 dtlIterationTime      aIterationTimeScope,
                                 dtlIterationTime      aIterationTime,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv );

stlStatus qlvValidateRowExpr( qlvStmtInfo         * aStmtInfo,
                              qlvExprPhraseType     aPhraseType,
                              ellObjectList       * aValidationObjList,
                              qlvRefTableList     * aRefTableList,
                              stlBool               aIsAtomic,
                              stlInt32              aAllowedAggrDepth,
                              qlpRowExpr          * aRowSource,
                              qlvInitExpression  ** aExprCode,
                              stlBool             * aHasOuterJoinOperator,
                              knlBuiltInFunc        aRelatedFuncId,
                              dtlIterationTime      aIterationTimeScope,
                              dtlIterationTime      aIterationTime,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv );
/** @} qlvFunction */


/*******************************************************************************
 * FUNCTION
 ******************************************************************************/

/**
 * @defgroup qlvPseudoCol Pseudo Column Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidatePseudoCol( qlvStmtInfo         * aStmtInfo,
                                qlvExprPhraseType     aPhraseType,
                                ellObjectList       * aValidatioinObjList,
                                qlvRefTableList     * aRefTableList,
                                stlBool               aIsAtomic,
                                stlInt32              aAllowedAggrDepth,
                                qlpPseudoCol        * aPseudoCol,
                                qlvInitExpression  ** aExprCode,
                                stlBool             * aHasOuterJoinOperator,
                                knlBuiltInFunc        aRelatedFuncId,
                                dtlIterationTime      aIterationTimeScope,
                                dtlIterationTime      aIterationTime,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv );


/** @} qlvPseudoCol */


/*******************************************************************************
 * AGGREGATION
 ******************************************************************************/

/**
 * @defgroup qlvAggregation Aggregation Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateAggregation( qlvStmtInfo         * aStmtInfo,
                                  qlvExprPhraseType     aPhraseType,
                                  ellObjectList       * aValidationObjList,
                                  qlvRefTableList     * aRefTableList,
                                  stlBool               aIsAtomic,
                                  stlInt32              aAllowedAggrDepth,
                                  qlpAggregation      * aAggrSource,
                                  qlvInitExpression  ** aExprCode,
                                  stlBool             * aHasOuterJoinOperator,
                                  knlBuiltInFunc        aRelatedFuncId,
                                  dtlIterationTime      aIterationTimeScope,
                                  dtlIterationTime      aIterationTime,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv );


/** @} qlvAggregation */


/*******************************************************************************
 * VALUE 
 ******************************************************************************/

/**
 * @defgroup qlvValue Value Validation
 * @ingroup qlv
 * @{
 */


stlStatus qlvValidateNullBNF( qlvStmtInfo         * aStmtInfo,
                              qlpValue            * aExprSource,
                              qlvInitExpression  ** aExprCode,
                              knlBuiltInFunc        aRelatedFuncId,
                              dtlIterationTime      aIterationTimeScope,
                              dtlIterationTime      aIterationTime,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv );

stlStatus qlvValidateStringBNF( qlvStmtInfo         * aStmtInfo,
                                qlpValue            * aExprSource,
                                qlvInitExpression  ** aExprCode,
                                knlBuiltInFunc        aRelatedFuncId,
                                dtlIterationTime      aIterationTimeScope,
                                dtlIterationTime      aIterationTime,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv );

stlStatus qlvValidateBitStringBNF( qlvStmtInfo         * aStmtInfo,
                                   qlpValue            * aExprSource,
                                   qlvInitExpression  ** aExprCode,
                                   knlBuiltInFunc        aRelatedFuncId,
                                   dtlIterationTime      aIterationTimeScope,
                                   dtlIterationTime      aIterationTime,
                                   knlRegionMem        * aRegionMem,
                                   qllEnv              * aEnv );

stlStatus qlvValidateNumberBNF( qlvStmtInfo         * aStmtInfo,
                                qlpValue            * aExprSource,
                                qlvInitExpression  ** aExprCode,
                                knlBuiltInFunc        aRelatedFuncId,
                                dtlIterationTime      aIterationTimeScope,
                                dtlIterationTime      aIterationTime,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv );

stlStatus qlvValidateDateTimeBNF( qlvStmtInfo         * aStmtInfo,
                                  qlpValue            * aExprSource,
                                  qlvInitExpression  ** aExprCode,
                                  knlBuiltInFunc        aRelatedFuncId,
                                  dtlIterationTime      aIterationTimeScope,
                                  dtlIterationTime      aIterationTime,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv );

stlStatus qlvValidateIntParamBNF( qlvStmtInfo         * aStmtInfo,
                                  qlpValue            * aExprSource,
                                  qlvInitExpression  ** aExprCode,
                                  knlBuiltInFunc        aRelatedFuncId,
                                  dtlIterationTime      aIterationTimeScope,
                                  dtlIterationTime      aIterationTime,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv );

stlStatus qlvValidateDefaultBNF( qlvStmtInfo         * aStmtInfo,
                                 ellObjectList       * aValidationObjList,
                                 qllNode             * aExprSource,
                                 ellDictHandle       * aColumnHandle,
                                 qlvInitExpression  ** aExprCode,
                                 knlBuiltInFunc        aRelatedFuncId,
                                 dtlIterationTime      aIterationTime,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv );

stlStatus qlvAddIntegerValueExpr( qlvStmtInfo         * aStmtInfo,
                                  stlInt64              aInteger,
                                  qlvInitExpression  ** aExprCode,
                                  knlBuiltInFunc        aRelatedFuncId,
                                  dtlIterationTime      aIterationTime,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv );

stlBool qlvIsValidSkipLimit( qlvInitExpression * aSkipLimit, qllEnv * aEnv );
                             
stlBool qlvIsSameExpression( qlvInitExpression  * aExpr1,
                             qlvInitExpression  * aExpr2 );

stlBool qlvIsExistInExpression( qlvInitExpression  * aExpr,
                                qlvInitExpression  * aSearchExpr );

qlvInitExpression *  qlvFindInternalBindValExprInExpr( qlvInitExpression   * aExpr,
                                                       stlInt32              aIdx );

/** @} qlvValue */


/*******************************************************************************
 * PARAMETER  
 ******************************************************************************/

/**
 * @defgroup qlvParameter Parameter Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateBindParam( qlvStmtInfo         * aStmtInfo,
                                qlvExprPhraseType     aPhraseType,
                                stlBool               aIsAtomic,
                                qlpParameter        * aExprSource,
                                qlvInitExpression  ** aExprCode,
                                knlBuiltInFunc        aRelatedFuncId,
                                dtlIterationTime      aIterationTimeScope,
                                dtlIterationTime      aIterationTime,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv );


stlStatus qlvValidateReturningPhrase( qlvStmtInfo         * aStmtInfo,
                                      ellObjectList       * aValidationObjList,
                                      qlvInitNode         * aTableNode,
                                      qlpList             * aParseReturnTarget,
                                      stlInt32            * aReturnTargetCnt,
                                      qlvInitTarget      ** aInitReturnTarget,
                                      qllEnv              * aEnv );

stlStatus qlvValidateIntoPhrase( qlvStmtInfo         * aStmtInfo,
                                 stlInt32              aTargetCount,
                                 qlpIntoClause       * aIntoClause,
                                 qlvInitExpression   * aInitIntoArray,
                                 qllEnv              * aEnv );
                                 
                                 
/** @} qlvParameter */


/*******************************************************************************
 * SUB-QUERY
 ******************************************************************************/

/**
 * @defgroup qlvSubQuery Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvValidateSubQuery( qlvStmtInfo         * aStmtInfo,
                               qlvExprPhraseType     aPhraseType,
                               ellObjectList       * aValidationObjList,
                               qlvRefTableList     * aRefTableList,
                               stlBool               aIsAtomic,
                               stlBool               aIsRowExpr,
                               stlInt32              aAllowedAggrDepth,
                               qlpSubTableNode     * aSubQuery,
                               qlvInitExpression  ** aExprCode,
                               knlBuiltInFunc        aRelatedFuncId,
                               dtlIterationTime      aIterationTimeScope,
                               dtlIterationTime      aIterationTime,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv );

/** @} qlvSubQuery */


/*******************************************************************************
 * HINT    
 ******************************************************************************/

/**
 * @defgroup qlvHint Hint Validation
 * @ingroup qlv
 * @{
 */

stlStatus qlvCreateHintInfo( qlvHintInfo   ** aHintInfo,
                             stlInt32         aTableCount,
                             knlRegionMem   * aRegionMem,
                             qllEnv         * aEnv );

stlStatus qlvValidateHintInfo( qlvStmtInfo  * aStmtInfo,
                               stlBool        aCalledFromSubQuery,
                               stlBool        aHasGroupBy,
                               stlBool        aHasDistinct,
                               qlvInitNode  * aTableNode,
                               stlInt32       aTableCount,
                               qlpList      * aHintSource,
                               qlvHintInfo  * aHintInfo,
                               qllEnv       * aEnv );

stlStatus qlvValidateHintIndex( qlvStmtInfo         * aStmtInfo,
                                qlvInitNode        ** aTableNodePtrArr,
                                stlInt32              aTableCount,
                                stlBool               aEnabledHintError,
                                qlpHintIndex        * aHintIndex,
                                qlvAccessPathHint   * aAccPathHintArr,
                                qllEnv              * aEnv );

stlStatus qlvValidateHintTable( qlvStmtInfo         * aStmtInfo,
                                qlvInitNode        ** aTableNodePtrArr,
                                stlInt32              aTableCount,
                                stlBool               aEnabledHintError,
                                qlpHintTable        * aHintTable,
                                qlvAccessPathHint   * aAccPathHintArr,
                                qllEnv              * aEnv );

stlStatus qlvValidateHintJoinOrder( qlvStmtInfo         * aStmtInfo,
                                    qlvInitNode        ** aTableNodePtrArr,
                                    stlInt32              aTableCount,
                                    stlBool               aEnabledHintError,
                                    qlpHintJoinOrder    * aHintJoinOrder,
                                    qlvJoinOrderHint    * aJoinOrderArr,
                                    stlInt32            * aJoinOrderCount,
                                    qllEnv              * aEnv );

stlStatus qlvValidateHintJoinOper( qlvStmtInfo      * aStmtInfo,
                                   qlvInitNode     ** aTableNodePtrArr,
                                   stlInt32           aTableCount,
                                   stlBool            aEnabledHintError,
                                   qlpHintJoinOper  * aHintJoinOper,
                                   qlvJoinOperList  * aJoinOperList,
                                   qllEnv           * aEnv );

stlStatus qlvValidateHintQueryTransform( qlvStmtInfo            * aStmtInfo,
                                         stlBool                  aEnabledHintError,
                                         stlBool                  aCalledFromSubQuery,
                                         qlpHintQueryTransform  * aHintQueryTransform,
                                         qlvQueryTransformHint  * aValidateQTHint,
                                         qllEnv                 * aEnv );

stlStatus qlvValidateHintOther( qlvStmtInfo     * aStmtInfo,
                                stlBool           aEnabledHintError,
                                stlBool           aCalledFromSubQuery,
                                stlBool           aHasGroupBy,
                                stlBool           aHasDistinct,
                                qlpHintOther    * aHintOther,
                                qlvOtherHint    * aValidateOtherHint,
                                qllEnv          * aEnv );

/** @} qlvHint */


/*******************************************************************************
 * MISC    
 ******************************************************************************/

/**
 * @defgroup qlvMisc Misc Validation
 * @ingroup qlv
 * @{
 */


// stlStatus qlvValidateObjectName( smlTransId      aTransID,
//                                  qllStatement  * aSQLStmt,
//                                  qllNode       * aParseTree,
//                                  qllEnv        * aEnv );

// stlStatus qlvValidateList( smlTransId      aTransID,
//                            qllStatement  * aSQLStmt,
//                            qllNode       * aParseTree,
//                            qllEnv        * aEnv );

// stlStatus qlvValidateDefElem( smlTransId      aTransID,
//                               qllStatement  * aSQLStmt,
//                               qllNode       * aParseTree,
//                               qllEnv        * aEnv );

// stlStatus qlvValidateParseContext( smlTransId      aTransID,
//                                    qllStatement  * aSQLStmt,
//                                    qllNode       * aParseTree,
//                                    qllEnv        * aEnv );

/*
 * Init Plan of Query or DML
 */

stlStatus qlvSetInitPlan( qllDBCStmt   * aDBCStmt,
                          qllStatement * aSQLStmt,
                          qlvInitPlan  * aInitPlan,
                          qllEnv       * aEnv );

void qlvSetInitPlanByInitPlan( qlvInitPlan  * aDesPlan,
                               qlvInitPlan  * aSrcPlan );

qlvInitPlan * qlvGetQueryInitPlan( qllStatement * aSQLStmt );

/*
 * Expression 
 */

stlStatus qlvValidateValueExpr( qlvStmtInfo         * aStmtInfo,
                                qlvExprPhraseType     aPhraseType,
                                ellObjectList       * aValidationObjList,
                                qlvRefTableList     * aRefTableList,
                                stlBool               aIsAtomic,
                                stlBool               aIsRowExpr,
                                stlInt32              aAllowedAggrDepth,
                                qllNode             * aExprSource,
                                qlvInitExpression  ** aExprCode,
                                stlBool             * aHasOuterJoinOperator,
                                knlBuiltInFunc        aRelatedFuncId,
                                dtlIterationTime      aIterationTimeScope,
                                dtlIterationTime      aIterationTime,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv );

stlStatus qlvCopyExpr( qlvInitExpression   * aSourceExpr,
                       qlvInitExpression  ** aDestExpr,
                       knlRegionMem        * aRegionMem,
                       qllEnv              * aEnv );

stlStatus qlvDuplicateExpr( qlvInitExpression   * aSourceExpr,
                            qlvInitExpression  ** aDestExpr,
                            knlRegionMem        * aRegionMem,
                            qllEnv              * aEnv );


/*
 * Expression List
 */

stlStatus qlvCreateRefExprList( qlvRefExprList  ** aRefExprList,
                                knlRegionMem     * aRegionMem,
                                qllEnv           * aEnv );

stlStatus qlvAddExprToRefExprList( qlvRefExprList     * aRefExprList,
                                   qlvInitExpression  * aInitExpr,
                                   stlBool              aIsSetOffset,
                                   knlRegionMem       * aRegionMem,
                                   qllEnv             * aEnv );

stlStatus qlvAddExprToRefColExprList( qlvRefExprList     * aRefColExprList,
                                      qlvInitExpression  * aInitExpr,
                                      knlRegionMem       * aRegionMem,
                                      qllEnv             * aEnv );

stlStatus qlvAddExprListToRefExprList( qlvRefExprList     * aDestExprList,
                                       qlvRefExprList     * aSrcExprList,
                                       stlBool              aIsSetOffset,
                                       knlRegionMem       * aRegionMem,
                                       qllEnv             * aEnv );

stlStatus qlvRemoveExprToRefExprList( qlvRefExprList     * aRefExprList,
                                      qlvInitExpression  * aInitExpr,
                                      qllEnv             * aEnv );

stlStatus qlvAddPseudoColExprList( qlvRefExprList     * aRefExprList,
                                   qlvInitExpression  * aExpr,
                                   knlRegionMem       * aRegionMem,
                                   qllEnv             * aEnv );

stlStatus qlvAddAggrExprList( qlvInitExprList     * aInitExprList,
                              qlvInitExpression   * aInitExpr,
                              stlBool               aIsSetOffset,
                              qlvInitExpression  ** aInitNewExpr,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv );

stlStatus qlvAddConstExpr( qlvInitExpression   * aInitExpr,
                           dtlIterationTime      aParentIterTime,
                           qlvInitExprList     * aInitExprList,
                           qlvInitExpression  ** aOrgExpr,
                           knlRegionMem        * aRegionMem,
                           qllEnv              * aEnv );


/*
 * Index 관련
 */

stlStatus qlvIsAffectedIndexByColumnList( ellDictHandle   * aIndexDictHandle,
                                          qlvRefExprList  * aWriteColumnList,
                                          stlBool         * aIsAffected,
                                          qllEnv          * aEnv );

stlStatus qlvAddRefColumnOnBaseTableByIndex( qlvInitBaseTableNode  * aBaseTableNode,
                                             ellDictHandle         * aIndexDictHandle,
                                             knlRegionMem          * aRegionMem,
                                             qllEnv                * aEnv );

stlStatus qlvAddRefColumnOnBaseTableByConstraintColumn( qlvInitBaseTableNode  * aBaseTableNode,
                                                        ellDictHandle         * aColumnHandle,
                                                        knlRegionMem          * aRegionMem,
                                                        qllEnv                * aEnv );

stlStatus qlvGetRefColList( qlvInitExpression   * aOrgExpr,
                            qlvInitExpression   * aCurExpr,
                            qlvRefColExprList   * aColExprList,
                            knlRegionMem        * aRegionMem,
                            qllEnv              * aEnv );


/*
 * RETURN 절 관련
 */
stlStatus qlvValidateReturn( qlvStmtInfo       * aStmtInfo,
                             ellObjectList     * aValidationObjList,
                             qlvRefTableList   * aRefTableList,
                             stlInt32            aAllowedAggrDepth,
                             qlpTarget         * aTargetSource,
                             qlvInitTarget     * aTargetCode,
                             dtlIterationTime    aIterationTimeScope,
                             dtlIterationTime    aIterationTime,
                             qllEnv            * aEnv );

stlStatus qlvValidateSequenceInTarget( stlChar            * aSQLString,
                                       qlvInitExpression  * aTargetExpr,
                                       qllEnv             * aEnv );

/*
 * SYNONYM 관련 
 */

stlStatus qlvGetTblDictHandleBySchNameNObjNameOfSynonym( smlTransId          aTransID,
                                                         qllDBCStmt        * aDBCStmt,
                                                         qllStatement      * aSQLStmt,
                                                         stlChar           * aSQLString,
                                                         stlInt32            aTableNamePos,
                                                         stlChar           * aSchemaName,
                                                         stlChar           * aObjectName,
                                                         ellDictHandle     * aTableDictHandle,
                                                         stlBool           * aExist,
                                                         ellObjectList     * aSynonymList,
                                                         ellObjectList     * aPublicSynonymList,
                                                         qllEnv            * aEnv );

stlStatus qlvGetTableDictHandleBySchHandleNTblName( smlTransId          aTransID,
                                                    qllDBCStmt        * aDBCStmt,
                                                    qllStatement      * aSQLStmt,
                                                    stlChar           * aSQLString,
                                                    ellDictHandle     * aSchemaDictHandle,
                                                    stlChar           * aTableName,
                                                    stlInt32            aTableNamePos,
                                                    ellDictHandle     * aTableDictHandle,
                                                    stlBool           * aExist,
                                                    qllEnv            * aEnv );

stlStatus qlvGetTableDictHandleByAuthHandleNTblName( smlTransId          aTransID,
                                                     qllDBCStmt        * aDBCStmt,
                                                     qllStatement      * aSQLStmt,
                                                     stlChar           * aSQLString,
                                                     ellDictHandle     * aAuthDictHandle,
                                                     stlChar           * aTableName,
                                                     stlInt32            aTableNamePos,
                                                     ellDictHandle     * aTableDictHandle,
                                                     stlBool           * aExist,
                                                     qllEnv            * aEnv );

stlStatus qlvGetSequenceDictHandleBySchNameNObjName( smlTransId          aTransID,
                                                     qllDBCStmt        * aDBCStmt,
                                                     qllStatement      * aSQLStmt,
                                                     stlChar           * aSQLString,
                                                     stlInt32            aSeqNamePos,
                                                     stlChar           * aSchemaName,
                                                     stlChar           * aObjectName,
                                                     ellDictHandle     * aSeqDictHandle,
                                                     stlBool           * aExist,
                                                     ellObjectList     * aSynonymList,
                                                     ellObjectList     * aPublicSynonymList,
                                                     qllEnv            * aEnv );

stlStatus qlvGetSequenceDictHandleBySchHandleNSeqName( smlTransId          aTransID,
                                                       qllDBCStmt        * aDBCStmt,
                                                       qllStatement      * aSQLStmt,
                                                       stlChar           * aSQLString,
                                                       ellDictHandle     * aSchemaDictHandle,
                                                       stlChar           * aSeqName,
                                                       stlInt32            aSeqNamePos,
                                                       ellDictHandle     * aSeqDictHandle,
                                                       stlBool           * aExist,
                                                       qllEnv            * aEnv );

stlStatus qlvGetSequenceDictHandleByAuthHandleNSeqName( smlTransId          aTransID,
                                                        qllDBCStmt        * aDBCStmt,
                                                        qllStatement      * aSQLStmt,
                                                        stlChar           * aSQLString,
                                                        ellDictHandle     * aAuthDictHandle,
                                                        stlChar           * aSeqName,
                                                        stlInt32            aSeqNamePos,
                                                        ellDictHandle     * aSeqDictHandle,
                                                        stlBool           * aExist,
                                                        qllEnv            * aEnv );


/*
 * CONSTANT EXPRESSION 관련 
 */

stlStatus qlvGetConstDataValueFromParseNode( qllStatement     * aSQLStmt,
                                             stlChar          * aSQLString,
                                             qllNode          * aExprSource,
                                             qlvInitTypeDef   * aDataTypeDef,
                                             dtlDataValue    ** aDataValue,
                                             knlRegionMem     * aRegionMem,
                                             qllEnv           * aEnv );

/** @} qlvMisc */




#endif /* _QLVVALIDATION_H_ */
