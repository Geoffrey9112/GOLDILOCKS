/*******************************************************************************
 * qlrTruncateTable.h
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

#ifndef _QLR_TRUNCATE_TABLE_H_
#define _QLR_TRUNCATE_TABLE_H_ 1

/**
 * @file qlrTruncateTable.h
 * @brief TRUNCATE TABLE 
 */

/**
 * @brief TRUNCATE TABLE 을 위한 Code Plan
 */
typedef struct qlrInitTruncateTable
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle    mSchemaHandle;     /**< Schema Handle */
    ellDictHandle    mTableHandle;      /**< Table Handle */

    qlpIdentityRestart    mIdentityRestart;  /**< IDENTITY RESTART option */
    qlpDropStorageOption  mDropStorage;      /**< Drop Storage option */

    /*
     * TRUNCATE table 은 drop behavior 가 존재하지 않는다.
     * foreign key 가 걸린 child table 들을 truncate 할 경우 dead-lock 이 발생할 수 있다.
     */ 
    
} qlrInitTruncateTable;




stlStatus qlrValidateTruncateTable( smlTransId      aTransID,
                                    qllDBCStmt    * aDBCStmt,
                                    qllStatement  * aSQLStmt,
                                    stlChar       * aSQLString,
                                    qllNode       * aParseTree,
                                    qllEnv        * aEnv );

stlStatus qlrOptCodeTruncateTable( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv );

stlStatus qlrOptDataTruncateTable( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv );

stlStatus qlrExecuteTruncateTable( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv );

stlStatus qlrTruncateOneTable( smlTransId             aTransID,
                               smlStatement         * aStmt,
                               qllDBCStmt           * aDBCStmt,
                               ellDictHandle        * aTableHandle,
                               smlDropStorageOption   aPhyBehavior,
                               qlpIdentityRestart     aIdentityRestart,
                               qllEnv               * aEnv );


#endif /* _QLR_TRUNCATE_TABLE_H_ */

