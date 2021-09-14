/*******************************************************************************
 * qlrAlterTableAddColumn.h
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

#ifndef _QLR_ALTER_TABLE_ADD_COLUMN_H_
#define _QLR_ALTER_TABLE_ADD_COLUMN_H_ 1

/**
 * @file qlrAlterTableAddColumn.h
 * @brief ALTER TABLE .. ADD COLUMN
 */

/**
 * @brief ADD COLUMN 을 위한 Init Plan
 */
typedef struct qlrInitAddColumn
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle      mSchemaHandle;     /**< Schema Handle */
    ellDictHandle      mTableHandle;      /**< Table Handle */

    qlrInitColumn          * mColumnList;        /**< Column Definition List */

    qlrInitTableConstraint   mAllConstraint;    /**< All Table Constraints */

    qlvInitStmtExprList  * mStmtExprList;       /**< Statement 단위 Expression 정보 */
    qlvInitExprList      * mQueryExprList;      /**< Query 단위 Expression 정보 */

    /**
     * Refine Column Type
     */
    
    stlInt32                 mColumnCount;
    qlvInitTypeDef         * mRefineColumnType;  /**< 정의되지 않은 정보가 설정된 Column 의 DB Type */
    
    /*
     * 물리적으로 Column 을 만들지의 기준
     */

    stlBool            mHasDefault;        /**< Default Column 존재 여부 */
    stlBool            mHasIdentity;       /**< Identity Column 존재 여부 */
} qlrInitAddColumn;



/**
 * @brief ADD COLUMN 을 위한 Code Plan
 */
typedef struct qlrCodeAddColumn
{
    qloInitStmtExprList  * mStmtExprList;       /**< Statement 단위 Expression 정보 */
    qloInitExprList      * mQueryExprList;      /**< Query 단위 Expression 정보 */
    
    qlvInitExpression     ** mDefaultExprArray;      /**< DEFAULT Value Expression Array */
    knlExprStack           * mDefaultCodeStack;      /**< DEFAULT 절 Expression Stack: array */
    
} qlrCodeAddColumn;
    
/**
 * @brief ADD COLUMN 을 위한 Data Plan
 */
typedef struct qlrDataAddColumn
{
    /*
     * Table 객체 정보 
     */
    
    stlInt64              mTableOwnerID;
    stlInt64              mTableSchemaID;
    stlInt64              mTableID;
    void                * mTablePhyHandle;

    /*
     * Table Element 객체 
     */
    
    stlInt64            * mColumnID;
    ellAddColumnDesc    * mAddColumnDesc;

    /*
     * DEFAULT expression
     */

    qllDataArea            * mDataArea;
    
    knlValueBlockList      * mTotalPseudoColBlock;   /**< STATEMENT level Pseudo Column List */
    knlValueBlockList      * mQueryPseudoColBlock;   /**< ROW level Pseudo Column List */
    
    smlRowBlock            * mRowBlock;              /**< Row 관련 정보를 저장할 Block */
    knlValueBlockList      * mDefaultBlockList;      /**< Update Table에 대한 Write Column List */
    knlExprEvalInfo        * mDefaultEvalInfo;
    
} qlrDataAddColumn;

stlStatus qlrValidateAddColumn( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv );

stlStatus qlrOptCodeAddColumn( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qlrOptDataAddColumn( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qlrExecuteAddColumn( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qlrCheckInlineEmptyTable( smlTransId         aTransID,
                                    smlStatement     * aStmt,
                                    qllDBCStmt       * aDBCStmt,
                                    qlrInitAddColumn * aInitPlan,
                                    stlBool          * aIsEmptyTable,
                                    qllEnv           * aEnv );

stlStatus qlrAppendData4AddColumn( smlTransId         aTransID,
                                   smlStatement     * aStmt,
                                   qllDBCStmt       * aDBCStmt,
                                   qllStatement     * aSQLStmt,
                                   qllEnv           * aEnv );

#endif /* _QLR_ALTER_TABLE_ADD_COLUMN_H_ */

