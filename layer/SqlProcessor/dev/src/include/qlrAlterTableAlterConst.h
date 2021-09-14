/*******************************************************************************
 * qlrAlterTableAlterConst.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlrAlterTableAlterConst.h 5803 2012-09-27 06:40:12Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLR_ALTER_TABLE_ALTER_CONST_H_
#define _QLR_ALTER_TABLE_ALTER_CONST_H_ 1

/**
 * @file qlrAlterTableAlterConst.h
 * @brief ALTER TABLE .. DROP CONSTRAINT
 */

/**
 * @brief ALTER CONSTRAINT 를 위한 Init Plan
 */
typedef struct qlrInitAlterConst
{
    qlvInitPlan        mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle      mSchemaHandle;     /**< Schema Handle */
    ellDictHandle      mTableHandle;      /**< Table Handle */
    ellDictHandle      mConstHandle;      /**< Constraint Handle */

    /**
     * Constraint Characteristics
     */

    stlBool            mSetDeferrable;    /**< Deferrable 설정 여부 */
    stlBool            mDeferrable;         /**< Deferrable */
    
    stlBool            mSetInitDeferred;  /**< Initially Deferred 설정 여부 */
    stlBool            mInitDeferred;       /**< Initially Deferred */

} qlrInitAlterConst;




stlStatus qlrValidateAlterConst( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv );

stlStatus qlrOptCodeAlterConst( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlrOptDataAlterConst( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlrExecuteAlterConst( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );


#endif /* _QLR_ALTER_TABLE_ALTER_CONST_H_ */

