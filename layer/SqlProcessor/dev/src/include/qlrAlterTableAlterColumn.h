/*******************************************************************************
 * qlrAlterTableAlterColumn.h
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

#ifndef _QLR_ALTER_TABLE_ALTER_COLUMN_H_
#define _QLR_ALTER_TABLE_ALTER_COLUMN_H_ 1

/**
 * @file qlrAlterTableAlterColumn.h
 * @brief ALTER TABLE .. ALTER COLUMN
 */

extern const stlBool qlrSetDataTypeCompatMatrix[DTL_TYPE_MAX][DTL_TYPE_MAX];

/**
 * @brief ALTER COLUMN 을 위한 Code Plan
 */
typedef struct qlrInitAlterColumn
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle         mSchemaHandle;     /**< Schema Handle */
    ellDictHandle         mTableHandle;      /**< Table Handle */
    ellDictHandle         mColumnHandle;     /**< Column Handle */

    qlpAlterColumnAction  mAlterAction;      /**< Alter Column Action */

    stlBool               mIsNullable;       /**< Column 의 Nullable 여부 */

    /*
     * for SET DEFAULT
     */
    
    stlChar             * mColumnDefault;     /**< DEFAULT clause string */
    
    /*
     * for SET NOT NULL
     */

    ellDictHandle         mNotNullSchemaHandle;     /**< NOT NULL Schema Handle */
    stlChar             * mNotNullConstName;        /**< NOT NULL Constraint Name */
    stlBool               mNotNullDeferrable;       /**< NOT NULL Deferrable */
    stlBool               mNotNullInitDeferred;     /**< NOT NULL Initially Deferred */
    stlBool               mNotNullEnforced;         /**< NOT NULL Enforced */
    
    /*
     * for DROP NOT NULL
     */
    
    ellDictHandle         mNotNullHandle;    /**< Not Null Handle */

    /*
     * for SET DATA TYPE
     */

    qlvInitTypeDef        mNewDataType;      /**< New Data Type Definition */

    /*
     * for Alter Identity
     */

    ellIdentityGenOption mIdentityGenOption;    /**< Identity Generation Option */
    stlBool              mModifySeqAttr;        /**< Modify Sequence Attribute or not */
    smlSequenceAttr      mIdentitySeqAttr;      /**< Idenitty Sequence Attribute */
    
} qlrInitAlterColumn;




stlStatus qlrValidateAlterColumn( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv );

stlStatus qlrValidateSetDefault( smlTransId                 aTransID,
                                 qllDBCStmt               * aDBCStmt,
                                 qllStatement             * aSQLStmt,
                                 stlChar                  * aSQLString,
                                 qlpAlterTableAlterColumn * aParseTree,
                                 qlrInitAlterColumn       * aInitPlan,
                                 qllEnv                   * aEnv );

stlStatus qlrValidateDropDefault( smlTransId                 aTransID,
                                  stlChar                  * aSQLString,
                                  qlpAlterTableAlterColumn * aParseTree,
                                  qlrInitAlterColumn       * aInitPlan,
                                  qllEnv                   * aEnv );

stlStatus qlrValidateSetNotNull( smlTransId                 aTransID,
                                 qllDBCStmt               * aDBCStmt,
                                 qllStatement             * aSQLStmt,
                                 stlChar                  * aSQLString,
                                 qlrInitAlterColumn       * aInitPlan,
                                 qlpObjectName            * aNotNullName,
                                 qlpConstraintAttr        * aNotNullConstAttr,
                                 qllEnv                   * aEnv );

stlStatus qlrValidateDropNotNull( smlTransId                 aTransID,
                                  qllDBCStmt               * aDBCStmt,
                                  qllStatement             * aSQLStmt,
                                  qlrInitAlterColumn       * aInitPlan,
                                  qllEnv                   * aEnv );

stlStatus qlrValidateAlterIdentity( smlTransId                 aTransID,
                                    qllDBCStmt               * aDBCStmt,
                                    qllStatement             * aSQLStmt,
                                    stlChar                  * aSQLString,
                                    qlpAlterTableAlterColumn * aParseTree,
                                    qlrInitAlterColumn       * aInitPlan,
                                    qllEnv                   * aEnv );

stlStatus qlrValidateDropIdentity( smlTransId                 aTransID,
                                   qllDBCStmt               * aDBCStmt,
                                   qllStatement             * aSQLStmt,
                                   qlrInitAlterColumn       * aInitPlan,
                                   qllEnv                   * aEnv );

stlStatus qlrValidateSetDataType( smlTransId                 aTransID,
                                  qllDBCStmt               * aDBCStmt,
                                  qllStatement             * aSQLStmt,
                                  stlChar                  * aSQLString,
                                  qlpAlterTableAlterColumn * aParseTree,
                                  qlrInitAlterColumn       * aInitPlan,
                                  qllEnv                   * aEnv );

stlStatus qlrOptCodeAlterColumn( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlrOptDataAlterColumn( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlrExecuteAlterColumn( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlrExecuteSetDefault( smlTransId                 aTransID,
                                smlStatement             * aStmt,
                                qlrInitAlterColumn       * aInitPlan,
                                qllEnv                   * aEnv );

stlStatus qlrExecuteDropDefault( smlTransId                 aTransID,
                                 smlStatement             * aStmt,
                                 qlrInitAlterColumn       * aInitPlan,
                                 qllEnv                   * aEnv );

stlStatus qlrExecuteSetNotNull( smlTransId                 aTransID,
                                smlStatement             * aStmt,
                                qllDBCStmt               * aDBCStmt,
                                qllStatement             * aSQLStmt,
                                qlrInitAlterColumn       * aInitPlan,
                                qllEnv                   * aEnv );

stlStatus qlrExecuteDropNotNull( smlTransId                 aTransID,
                                 smlStatement             * aStmt,
                                 qlrInitAlterColumn       * aInitPlan,
                                 qllEnv                   * aEnv );

stlStatus qlrExecuteSetDataType( smlTransId                 aTransID,
                                 smlStatement             * aStmt,
                                 qllDBCStmt               * aDBCStmt,
                                 qlrInitAlterColumn       * aInitPlan,
                                 ellObjectList            * aAffectedViewList,
                                 qllEnv                   * aEnv );

stlStatus qlrExecuteAlterIdentity( smlTransId                 aTransID,
                                   smlStatement             * aStmt,
                                   qlrInitAlterColumn       * aInitPlan,
                                   qllEnv                   * aEnv );

stlStatus qlrExecuteDropIdentity( smlTransId                 aTransID,
                                  smlStatement             * aStmt,
                                  qlrInitAlterColumn       * aInitPlan,
                                  qllEnv                   * aEnv );

#endif /* _QLR_ALTER_TABLE_ALTER_COLUMN_H_ */

