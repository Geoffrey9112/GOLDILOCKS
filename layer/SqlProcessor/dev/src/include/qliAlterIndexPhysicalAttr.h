/*******************************************************************************
 * qliAlterIndexPhysicalAttr.h
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

#ifndef _QLI_ALTER_INDEX_PHYSICAL_ATTR_H_
#define _QLI_ALTER_INDEX_PHYSICAL_ATTR_H_ 1

/**
 * @file qliAlterIndexPhysicalAttr.h
 * @brief ALTER INDEX .. STORAGE .. 구문을 위한 헤더파일
 */

/**
 * @brief ALTER INDEX .. STORAGE .. 을 위한 Code Plan
 */
typedef struct qliInitAlterPhysicalAttr
{
    qlvInitPlan      mCommonInit;         /**< Common Init Plan */
    
    ellDictHandle    mSchemaHandle;      /**< Schema Handle */
    ellDictHandle    mTableHandle;       /**< Table Handle */
    ellDictHandle    mIndexHandle;       /**< Index Handle */
    
    stlChar        * mIndexName;         /**< Index Name */
    smlIndexAttr     mPhysicalAttr;      /**< Index Physical Attribute */
} qliInitAlterPhysicalAttr;


stlStatus qliValidateAlterPhysicalAttr( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv );

stlStatus qliOptCodeAlterPhysicalAttr( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qliOptDataAlterPhysicalAttr( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qliExecuteAlterPhysicalAttr( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

#endif /* _QLI_ALTER_INDEX_PHYSICAL_ATTR_H_ */

