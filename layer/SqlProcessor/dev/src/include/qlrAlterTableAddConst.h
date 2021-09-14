/*******************************************************************************
 * qlrAlterTableAddConst.h
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

#ifndef _QLR_ALTER_TABLE_ADD_CONST_H_
#define _QLR_ALTER_TABLE_ADD_CONST_H_ 1

/**
 * @file qlrAlterTableAddConst.h
 * @brief ALTER TABLE .. ADD CONSTRAINT
 */

/**
 * @brief ADD CONSTRAINT 를 위한 Init Plan
 */
typedef struct qlrInitAddConst
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle   mSchemaHandle;     /**< Schema Handle */
    ellDictHandle   mTableHandle;      /**< Table Handle */

    qlrInitConst    mConstDef;         /**< Constraint Definintion */
    
} qlrInitAddConst;




stlStatus qlrValidateAddConst( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               stlChar       * aSQLString,
                               qllNode       * aParseTree,
                               qllEnv        * aEnv );

stlStatus qlrOptCodeAddConst( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );

stlStatus qlrOptDataAddConst( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );

stlStatus qlrExecuteAddConst( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );


#endif /* _QLR_ALTER_TABLE_ADD_CONST_H_ */

