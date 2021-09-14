/*******************************************************************************
 * qlsDropSchema.h
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

#ifndef _QLS_DROP_SCHEMA_H_
#define _QLS_DROP_SCHEMA_H_ 1

/**
 * @file qlsDropSchema.h
 * @brief DROP SCHEMA
 */

/**
 * @brief DROP SCHEMA 를 위한 Init Plan
 */
typedef struct qlsInitDropSchema
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    stlBool          mIfExists;         /**< IF EXISTS 옵션 사용 여부 */
    stlBool          mSchemaExist;      /**< Schema 존재 여부 */
    
    ellDictHandle    mSchemaHandle;     /**< Schema Handle */
    
    stlBool          mIsCascade;        /**< CASCADE 여부  */
    
} qlsInitDropSchema;



stlStatus qlsValidateDropSchema( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv );

stlStatus qlsOptCodeDropSchema( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlsOptDataDropSchema( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlsExecuteDropSchema( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

/*
 * Execution 보조 함수
 */

stlStatus qlsSetOuterTableColumnNullable4DropNonSchemaObject( smlTransId      aTransID,
                                                              smlStatement  * aStmt,
                                                              knlRegionMem  * aRegionMem,
                                                              ellObjectType   aObjectType,
                                                              ellDictHandle * aObjectHandle,
                                                              ellObjectList * aPrimaryKeyOuterTableList,
                                                              ellObjectList * aNotNullOuterTableList,
                                                              qllEnv        * aEnv );

stlStatus qlsSetTableColumnNullable4DropNonSchemaObject( smlTransId      aTransID,
                                                         smlStatement  * aStmt,
                                                         knlRegionMem  * aRegionMem,
                                                         ellObjectType   aObjectType,
                                                         ellDictHandle * aObjectHandle,
                                                         ellDictHandle * aOuterTableHandle,
                                                         qllEnv        * aEnv );

#endif /* _QLS_DROP_SCHEMA_H_ */
