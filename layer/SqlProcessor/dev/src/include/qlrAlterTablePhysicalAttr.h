/*******************************************************************************
 * qlrAlterTablePhysicalAttr.h
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

#ifndef _QLR_ALTER_TABLE_PHYSICAL_ATTR_H_
#define _QLR_ALTER_TABLE_PHYSICAL_ATTR_H_ 1

/**
 * @file qlrAlterTablePhysicalAttr.h
 * @brief ALTER TABLE .. STORAGE .. 구문을 위한 헤더파일
 */

/**
 * @brief ALTER TABLE .. STORAGE .. 을 위한 Code Plan
 */
typedef struct qlrInitAlterPhysicalAttr
{
    qlvInitPlan      mCommonInit;         /**< Common Init Plan */
    
    ellDictHandle    mSchemaHandle;      /**< Schema Handle */
    ellDictHandle    mTableHandle;       /**< Table Handle */
    
    stlChar        * mTableName;         /**< Table Name */
    smlTableAttr     mPhysicalAttr;      /**< Table Physical Attribute */
} qlrInitAlterPhysicalAttr;


stlStatus qlrValidateAlterPhysicalAttr( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv );

stlStatus qlrOptCodeAlterPhysicalAttr( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qlrOptDataAlterPhysicalAttr( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qlrExecuteAlterPhysicalAttr( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

#endif /* _QLR_ALTER_TABLE_PHYSICAL_ATTR_H_ */

