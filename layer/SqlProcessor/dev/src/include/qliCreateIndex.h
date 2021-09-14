/*******************************************************************************
 * qliCreateIndex.h
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

#ifndef _QLI_CREATE_INDEX_H_
#define _QLI_CREATE_INDEX_H_ 1

/**
 * @file qliCreateIndex.h
 * @brief CREATE INDEX
 */

/**
 * @brief CREATE INDEX 을 위한 Init Plan
 */
typedef struct qliInitCreateIndex
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle      mSpaceHandle;       /**< Tablespace Handle */
    ellDictHandle      mSchemaHandle;      /**< Schema Handle */
    stlChar          * mIndexName;         /**< Index Name */

    ellDictHandle      mTableSchemaHandle; /**< Table Schema Handle */
    ellDictHandle      mTableHandle;       /**< Table Handle */
    
    stlBool            mIsUnique;          /**< Unique Index 여부 */

    smlIndexAttr       mPhysicalAttr;      /**< Index Physical Attribute */
    stlUInt16          mParallelFactor;    /**< Parallel Build Factor */
    
    stlInt32                    mKeyCnt;         /**< Key Column 개수 */
    ellDictHandle               mKeyHandle[DTL_INDEX_COLUMN_MAX_COUNT];
    ellIndexColumnOrdering      mKeyOrder[DTL_INDEX_COLUMN_MAX_COUNT];
    ellIndexColumnNullsOrdering mKeyNullOrder[DTL_INDEX_COLUMN_MAX_COUNT];
    stlUInt8                    mKeyFlag[DTL_INDEX_COLUMN_MAX_COUNT];  
    
} qliInitCreateIndex;
    
/**
 * @brief CREATE INDEX 을 위한 Data Plan
 */
typedef struct qliDataCreateIndex
{
    knlValueBlockList  * mValueList;       /**< Index Build 를 위한 공간 */
} qliDataCreateIndex;



stlStatus qliValidateCreateIndex( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv );

stlStatus qliOptCodeCreateIndex( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qliOptDataCreateIndex( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qliExecuteCreateIndex( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

/*
 * Validation 보조 함수 
 */

stlStatus qliValidateKeyDefinition( smlTransId           aTransID,
                                    qllDBCStmt         * aDBCStmt,
                                    qllStatement       * aSQLStmt,
                                    stlChar            * aSQLString,
                                    qliInitCreateIndex * aInitPlan,
                                    qlpList            * aKeyElement,
                                    qllEnv             * aEnv );

void qliGetIndexLoggingFlag( stlChar       * aSQLString,
                             qlpIndexAttr  * aParseIndexAttr,
                             smlIndexAttr  * aPhysicalAttr,
                             stlBool       * aLoggingFlag,
                             qllEnv        * aEnv );

stlStatus qliValidateIndexAttr( stlChar       * aSQLString,
                                smlTbsId        aSpacePhyID,
                                qlpIndexAttr  * aParseIndexAttr,
                                smlIndexAttr  * aPhysicalAttr,
                                qllEnv        * aEnv );


#endif /* _QLI_CREATE_INDEX_H_ */
