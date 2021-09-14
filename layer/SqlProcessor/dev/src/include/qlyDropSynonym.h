/*******************************************************************************
 * qlyDropSynonym.h
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

#ifndef _QLY_DROP_SYNONYM_H_
#define _QLY_DROP_SYNONYM_H_ 1

/**
 * @file qlyDropSynonym.h
 * @brief DROP SYNONYM
 */

/**
 * @brief DROP SYNONYM 을 위한 Init Plan
 */
typedef struct qlyInitDropSynonym
{
    qlvInitPlan        mCommonInit;        /**< Common Init Plan */

    stlChar          * mSynonymName;       /**< Synonym Name */
    stlBool            mIsPublic;          /**< PUBLIC option */
    stlBool            mIfExists;          /**< IF EXISTS option */
    stlBool            mSynonymExist;    /* < Synonym 존재 여부 */
    
    ellDictHandle      mSchemaHandle;      /**< Schema Handle */
    ellDictHandle      mSynonymHandle;     /**< Synonym Handle */
    
    stlChar          * mObjectSchemaName;  /**< Object Schema Name */
    stlChar          * mObjectName;        /**< Object Name */
    
} qlyInitDropSynonym;

stlStatus qlyValidateDropSynonym( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv );

stlStatus qlyOptCodeDropSynonym( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlyOptDataDropSynonym( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlyExecuteDropSynonym( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );


#endif /* _QLY_DROP_SYNONYM_H_ */
