/*******************************************************************************
 * qlrDropTable.h
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

#ifndef _QLR_DROP_TABLE_H_
#define _QLR_DROP_TABLE_H_ 1

/**
 * @file qlrDropTable.h
 * @brief DROP TABLE
 */

typedef struct qlrInitDropTable           qlrInitDropTable;
typedef struct qlrDataDropTable           qlrDataDropTable;

/**
 * @brief DROP TABLE 을 위한 Init Plan
 */
struct qlrInitDropTable
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    stlBool          mIfExists;         /**< IF EXISTS 옵션 사용 여부 */
    stlBool          mTableExist;       /**< Table 존재 여부 */

    ellDictHandle    mSchemaHandle;     /**< Schema Handle */
    ellDictHandle    mTableHandle;      /**< Table Handle */

    stlBool          mIsCascade;        /**< CASCADE CONSTRAINTS */
};



stlStatus qlrValidateDropTable( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv );

stlStatus qlrOptCodeDropTable( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qlrOptDataDropTable( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qlrExecuteDropTable( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

/*
 * Execution 보조 함수
 */

stlStatus qlrDropPhyObject4TablePrimeElement( smlTransId      aTransID,
                                              smlStatement  * aStmt,
                                              qllDBCStmt    * aDBCStmt,
                                              ellDictHandle * aTableHandle,
                                              qllEnv        * aEnv );

stlStatus qlrDropPhyTablePrimeElement4TableList( smlTransId      aTransID,
                                                 smlStatement  * aStmt,
                                                 qllDBCStmt    * aDBCStmt,
                                                 ellObjectList * aTableList,
                                                 qllEnv        * aEnv );


#endif /* _QLR_DROP_TABLE_H_ */
