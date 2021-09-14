/*******************************************************************************
 * qliDropIndex.h
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

#ifndef _QLI_DROP_INDEX_H_
#define _QLI_DROP_INDEX_H_ 1

/**
 * @file qliDropIndex.h
 * @brief DROP INDEX
 */

/**
 * @brief DROP INDEX 를 위한 Init Plan
 */
typedef struct qliInitDropIndex
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    stlBool          mIfExists;            /**< IF EXISTS 옵션 사용 여부 */
    stlBool          mIndexExist;          /**< Index 존재 여부 */

    stlBool          mIsUnique;            /**< Unique Index 여부 */
    
    ellDictHandle    mSchemaHandle;        /**< Index 의 Schema Handle */
    ellDictHandle    mTableHandle;         /**< Index 가 참조하는 Table Handle */
    ellDictHandle    mIndexHandle;         /**< Index Handle */
} qliInitDropIndex;


stlStatus qliValidateDropIndex( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv );

stlStatus qliOptCodeDropIndex( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qliOptDataDropIndex( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qliExecuteDropIndex( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qliDropIndexes4IndexList( smlStatement  * aStmt,
                                    ellObjectList * aIndexList,
                                    qllEnv        * aEnv );

stlStatus qliRefineCache4DropIndexList( smlTransId         aTransID,
                                        smlStatement     * aStmt,
                                        ellObjectList    * aIndexList,
                                        ellObjectList    * aIndexTableList,
                                        qllEnv           * aEnv );

#endif /* _QLI_DROP_INDEX_H_ */
