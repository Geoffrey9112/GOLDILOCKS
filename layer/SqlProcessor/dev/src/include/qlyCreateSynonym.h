/*******************************************************************************
 * qlyCreateSynonym.h
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

#ifndef _QLY_CREATE_SYNONYM_H_
#define _QLY_CREATE_SYNONYM_H_ 1

/**
 * @file qlyCreateSynonym.h
 * @brief CREATE SYNONYM
 */


/**
 * @brief CREATE SYNONYM 을 위한 Init Plan
 */
typedef struct qlyInitCreateSynonym
{
    qlvInitPlan        mCommonInit;        /**< Common Init Plan */
    
    ellDictHandle      mSchemaHandle;      /**< Schema Handle */
    stlChar          * mSynonymName;       /**< Synonym Name */

    stlBool            mOrReplace;         /**< OR REPLACE option */
    stlBool            mIsPublic;          /**< PUBLIC option */
    
    stlChar          * mObjectSchemaName;  /**< Object Schema Name */
    stlChar          * mObjectName;        /**< Object Name */

    stlBool            mOldSynonymExist;   /**< Old Synonym 존재 여부 */
    ellDictHandle      mOldSynonymHandle;  /**< Old Synonym Handle */
    
} qlyInitCreateSynonym;
    

stlStatus qlyValidateCreateSynonym( smlTransId      aTransID,
                                    qllDBCStmt    * aDBCStmt,
                                    qllStatement  * aSQLStmt,
                                    stlChar       * aSQLString,
                                    qllNode       * aParseTree,
                                    qllEnv        * aEnv );


stlStatus qlyOptCodeCreateSynonym( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv );

stlStatus qlyOptDataCreateSynonym( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv );

stlStatus qlyExecuteCreateSynonym( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv );

/*
 * Validation 보조 함수 
 */

stlStatus  qlyExistSameNameObject( smlTransId      aTransID,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   ellDictHandle * aSchemaHandle,
                                   qlpObjectName * aObjectName,
                                   ellDictHandle * aObjectDictHandle,
                                   stlBool       * aObjectExist,
                                   ellTableType  * aObjectType,  
                                   qllEnv        * aEnv );


#endif /* _QLY_CREATE_SYNONYM_H_ */
