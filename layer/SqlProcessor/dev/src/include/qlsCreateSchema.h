/*******************************************************************************
 * qlsCreateSchema.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlsCreateSchema.h 5803 2012-09-27 06:40:12Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLS_CREATE_SCHEMA_H_
#define _QLS_CREATE_SCHEMA_H_ 1

/**
 * @file qlsCreateSchema.h
 * @brief CREATE SCHEMA
 */


/**
 * @brief CREATE SCHEMA 를 위한 Init Plan
 */
typedef struct qlsInitCreateSchema
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    stlChar                * mSchemaName;      /**< Schema Name */
    ellDictHandle            mOwnerHandle;     /**< Owner Handle */

    qlpList                * mSchemaElemList;  /**< Schema Element List */
} qlsInitCreateSchema;


stlStatus qlsValidateCreateSchema( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv );

stlStatus qlsOptCodeCreateSchema( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

stlStatus qlsOptDataCreateSchema( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

stlStatus qlsExecuteCreateSchema( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

/*
 * Validation 보조 함수
 */

stlStatus qlsMatchSchemaName( stlChar      * aSchemaName,
                              stlChar      * aSQLString,
                              qlpList      * aSchemaObjectList,
                              qllEnv       * aEnv );

stlStatus qlsMatchSchemaNameGrantPriv( stlChar      * aSchemaName,
                                       stlChar      * aSQLString,
                                       qlpGrantPriv * aParseTree,
                                       qllEnv       * aEnv );

stlStatus qlsMatchSchemaNameCommentOn( stlChar          * aSchemaName,
                                       stlChar          * aSQLString,
                                       qlpCommentOnStmt * aParseTree,
                                       qllEnv           * aEnv );

/*
 * Execution 보조 함수
 */


#endif /* _QLS_CREATE_SCHEMA_H_ */
