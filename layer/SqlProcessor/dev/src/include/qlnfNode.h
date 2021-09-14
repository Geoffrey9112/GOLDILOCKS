/*******************************************************************************
 * qlnfNode.h
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

#ifndef _QLNFNODE_H_
#define _QLNFNODE_H_ 1

/**
 * @file qlnfNode.h
 * @brief SQL Processor Layer SQL Function Node
 */

#include <qlDef.h>

/**
 * @addtogroup qlnf
 * @{
 */

typedef stlStatus (*qlnfSetAggrDBTypeFuncPtr) ( qlvInitExpression  * aInitExpr,
                                                dtlDataType          aDBType,
                                                qloExprInfo        * aExprInfo,
                                                qllEnv             * aEnv );

typedef stlStatus (*qlnfGetAggrExecInfoFuncPtr) ( qllStatement          * aSQLStmt,
                                                  qlnfAggrOptInfo       * aAggrOptInfo,
                                                  qllDataArea           * aDataArea,
                                                  qlnfAggrExecInfo      * aAggrExecInfo,
                                                  knlRegionMem          * aRegionMem,
                                                  qllEnv                * aEnv );

typedef stlStatus (*qlnfGetAggrDistExecInfoFuncPtr) ( qllStatement          * aSQLStmt,
                                                      qlnfAggrOptInfo       * aAggrOptInfo,
                                                      qllDataArea           * aDataArea,
                                                      qlnfAggrDistExecInfo  * aAggrExecInfo,
                                                      knlRegionMem          * aRegionMem,
                                                      qllEnv                * aEnv );

typedef stlStatus (*qlnfAggrInitFuncPtr) ( qlnfAggrExecInfo   * aAggrExecInfo,
                                           stlInt32             aBlockIdx,
                                           qllEnv             * aEnv );

typedef stlStatus (*qlnfAggrBuildFuncPtr) ( qlnfAggrExecInfo   * aAggrExecInfo,
                                            stlInt32             aStartIdx,
                                            stlInt32             aBlockCnt,
                                            stlInt32             aResultBlockIdx,
                                            qllEnv             * aEnv );


/**
 * @brief aggregation function에 대한 정보 관리하는 구조체
 */
struct qlnfAggrFuncInfo
{
    knlBuiltInAggrFunc              mFuncID;                  /**< Built-In Aggregation Function ID */
    stlUInt16                       mArgumentCntMin;          /**< Function의 input Argument 최소 갯수 */
    stlUInt16                       mArgumentCntMax;          /**< Function의 input Argument 최대 갯수 */
    const stlChar                   mName[32];                /**< 함수의 대표명 */

    qlnfSetAggrDBTypeFuncPtr        mSetDBTypeFuncPtr;        /**< Function내 모든 Expression에 대한 DB type 설정 */
    qlnfGetAggrExecInfoFuncPtr      mGetExecInfoFuncPtr;      /**< Aggregation Execution 정보 얻기 */
    qlnfGetAggrDistExecInfoFuncPtr  mGetDistExecInfoFuncPtr;  /**< Aggregation Distinct Execution 정보 얻기 */
    qlnfAggrInitFuncPtr             mInitFuncPtr;             /**< Init Function Pointer 얻기 */
    qlnfAggrBuildFuncPtr            mBuildFuncPtr;            /**< Build Function Pointer 얻기 */
};


/**
 * @brief aggregation에 대한 Optimization 정보 관리하는 구조체
 */
struct qlnfAggrOptInfo
{
    qlvInitExpression    * mAggrExpr;         /**< Aggregation Expression */
    knlBuiltInAggrFunc     mAggrFuncId;       /**< Aggregation Function ID */
    knlExprStack        ** mArgCodeStack;     /**< Aggregation Argument Expression Stack
                                               *   : Function의 Argument Expression 마다 구성 (array) */
    knlExprStack         * mFilterCodeStack;  /**< Aggregation Function에 대한 Filter Expression Stack */
};

extern qlnfAggrFuncInfo gQlnfAggrFuncInfoArr[ KNL_BUILTIN_AGGREGATION_UNARY_MAX ];


/**
 * @brief aggregation에 대한 Execution 정보 관리하는 구조체
 */
struct qlnfAggrExecInfo
{
    /* Input Argument 관련 */
    stlInt32                 mInputArgCnt;        /**< input argument 개수 */
    knlExprEvalInfo       ** mEvalInfo;           /**< input argument value 구성을 위한 평가 정보 */

    /* Value Block 관련 */
    knlValueBlockList      * mInputArgList;       /**< input argument value block list */
    knlValueBlockList      * mAggrValueList;      /**< aggregation result value block list */

    /* Aggregation 정보 */
    qlnfAggrOptInfo        * mAggrOptInfo;        /**< aggregation optimization 정보 */
};


/**
 * @brief distinct 조건이 포함된 aggregation 에 대한 Execution 정보 관리하는 구조체
 * @remark qlnfAggrExecInfo로 type casting하여 사용할 수 있다.
 */
struct qlnfAggrDistExecInfo
{
    /* qlnfAggrExecInfo와 구조 통일 */
    
    /* Input Argument 관련 */
    stlInt32                 mInputArgCnt;        /**< input argument 개수 */
    knlExprEvalInfo       ** mEvalInfo;           /**< input argument value 구성을 위한 평가 정보 */

    /* Value Block 관련 */
    knlValueBlockList      * mInputArgList;       /**< input argument value block list */
    knlValueBlockList      * mAggrValueList;      /**< aggregation result value block list */

    /* Aggregation 정보 */
    qlnfAggrOptInfo        * mAggrOptInfo;        /**< aggregation optimization 정보 */


    /* Value Block 관련 */
    knlValueBlockList      * mInsertColList;      /**< insert column value block list for instant table */
    
    /* Instance Table 관련 */
    qlnInstantTable          mInstantTable;       /**< Distinct를 위한 Hash Instant Table */ 
    smlFetchInfo             mFetchInfo;          /**< filter 없음, Aggregation Info와 read col list만 존재 */
};


/*****************************************************
 * COMMON FUNCTIONS
 ****************************************************/

stlStatus qlnfGetAggrFuncExecInfo( qllStatement          * aSQLStmt,
                                   stlInt32                aAggrFuncCnt,
                                   qlnfAggrOptInfo       * aAggrOptInfo,
                                   qllDataArea           * aDataArea,
                                   qlnfAggrExecInfo     ** aAggrExecInfo,
                                   knlRegionMem          * aRegionMem,
                                   qllEnv                * aEnv );

stlStatus qlnfGetAggrDistFuncExecInfo( qllStatement           * aSQLStmt,
                                       stlInt32                 aAggrFuncCnt,
                                       qlnfAggrOptInfo        * aAggrOptInfo,
                                       qllDataArea            * aDataArea,
                                       qlnfAggrDistExecInfo  ** aAggrExecInfo,
                                       knlRegionMem           * aRegionMem,
                                       qllEnv                 * aEnv );

stlStatus qlnfGetAggrFuncAggrFetchInfo( qllStatement           * aSQLStmt,
                                        stlInt32                 aAggrFuncCnt,
                                        qlnfAggrOptInfo        * aAggrOptInfo,
                                        qllDataArea            * aDataArea,
                                        smlAggrFuncInfo       ** aAggrFetchInfo,
                                        knlRegionMem           * aRegionMem,
                                        qllEnv                 * aEnv );

stlStatus qlnfClassifyAggrFunc( qlvRefExprList   * aSrcAggrFuncList,
                                qlvRefExprList  ** aAggrFuncList,
                                qlvRefExprList  ** aAggrDistFuncList,
                                knlRegionMem     * aRegionMem,
                                qllEnv           * aEnv );

stlStatus qlnfSetNonDistAggrFunc( qlvRefExprList   * aSrcAggrFuncList,
                                  qlvRefExprList  ** aAggrFuncList,
                                  knlRegionMem     * aRegionMem,
                                  qllEnv           * aEnv );

stlStatus qlnfGetAggrOptInfo( qllStatement         * aSQLStmt,
                              qloInitExprList      * aQueryExprList,
                              stlInt32               aAggrFuncCnt,
                              qlvRefExprList       * aAggrFuncList,
                              qlnfAggrOptInfo     ** aAggrOptInfo,
                              knlRegionMem         * aRegionMem,
                              qllEnv               * aEnv );

/*****************************************************
 * AGGREGATION FUNCTIONS
 ****************************************************/

/* invalid */
stlStatus qlnfInvalidSetAggrDBType( qlvInitExpression  * aInitExpr,
                                    dtlDataType          aDBType,
                                    qloExprInfo        * aExprInfo,
                                    qllEnv             * aEnv );

stlStatus qlnfInvalidGetAggrExecInfo( qllStatement          * aSQLStmt,
                                      qlnfAggrOptInfo       * aAggrOptInfo,
                                      qllDataArea           * aDataArea,
                                      qlnfAggrExecInfo      * aAggrExecInfo,
                                      knlRegionMem          * aRegionMem,
                                      qllEnv                * aEnv );

stlStatus qlnfInvalidGetAggrDistExecInfo( qllStatement          * aSQLStmt,
                                          qlnfAggrOptInfo       * aAggrOptInfo,
                                          qllDataArea           * aDataArea,
                                          qlnfAggrDistExecInfo  * aAggrExecInfo,
                                          knlRegionMem          * aRegionMem,
                                          qllEnv                * aEnv );

stlStatus qlnfInvalidInit( qlnfAggrExecInfo   * aAggrExecInfo,
                           stlInt32             aBlockIdx,
                           qllEnv             * aEnv );

stlStatus qlnfInvalidBuild( qlnfAggrExecInfo   * aAggrExecInfo,
                            stlInt32             aStartIdx,
                            stlInt32             aBlockCnt,
                            stlInt32             aResultBlockIdx,
                            qllEnv             * aEnv );


/* sum */
stlStatus qlnfSumSetAggrDBType( qlvInitExpression  * aInitExpr,
                                dtlDataType          aDBType,
                                qloExprInfo        * aExprInfo,
                                qllEnv             * aEnv );

stlStatus qlnfSumGetAggrExecInfo( qllStatement          * aSQLStmt,
                                  qlnfAggrOptInfo       * aAggrOptInfo,
                                  qllDataArea           * aDataArea,
                                  qlnfAggrExecInfo      * aAggrExecInfo,
                                  knlRegionMem          * aRegionMem,
                                  qllEnv                * aEnv );

stlStatus qlnfSumGetAggrDistExecInfo( qllStatement          * aSQLStmt,
                                      qlnfAggrOptInfo       * aAggrOptInfo,
                                      qllDataArea           * aDataArea,
                                      qlnfAggrDistExecInfo  * aAggrExecInfo,
                                      knlRegionMem          * aRegionMem,
                                      qllEnv                * aEnv );

stlStatus qlnfSumInit( qlnfAggrExecInfo   * aAggrExecInfo,
                       stlInt32             aBlockIdx,
                       qllEnv             * aEnv );

stlStatus qlnfSumBuild( qlnfAggrExecInfo   * aAggrExecInfo,
                        stlInt32             aStartIdx,
                        stlInt32             aBlockCnt,
                        stlInt32             aResultBlockIdx,
                        qllEnv             * aEnv );


/* count */
stlStatus qlnfCountSetAggrDBType( qlvInitExpression  * aInitExpr,
                                  dtlDataType          aDBType,
                                  qloExprInfo        * aExprInfo,
                                  qllEnv             * aEnv );

stlStatus qlnfCountGetAggrExecInfo( qllStatement          * aSQLStmt,
                                    qlnfAggrOptInfo       * aAggrOptInfo,
                                    qllDataArea           * aDataArea,
                                    qlnfAggrExecInfo      * aAggrExecInfo,
                                    knlRegionMem          * aRegionMem,
                                    qllEnv                * aEnv );

stlStatus qlnfCountGetAggrDistExecInfo( qllStatement          * aSQLStmt,
                                        qlnfAggrOptInfo       * aAggrOptInfo,
                                        qllDataArea           * aDataArea,
                                        qlnfAggrDistExecInfo  * aAggrExecInfo,
                                        knlRegionMem          * aRegionMem,
                                        qllEnv                * aEnv );

stlStatus qlnfCountInit( qlnfAggrExecInfo   * aAggrExecInfo,
                         stlInt32             aBlockIdx,
                         qllEnv             * aEnv );

stlStatus qlnfCountBuild( qlnfAggrExecInfo   * aAggrExecInfo,
                          stlInt32             aStartIdx,
                          stlInt32             aBlockCnt,
                          stlInt32             aResultBlockIdx,
                          qllEnv             * aEnv );


/* count asterisk */
stlStatus qlnfCountAsteriskSetAggrDBType( qlvInitExpression  * aInitExpr,
                                          dtlDataType          aDBType,
                                          qloExprInfo        * aExprInfo,
                                          qllEnv             * aEnv );

stlStatus qlnfCountAsteriskGetAggrExecInfo( qllStatement          * aSQLStmt,
                                            qlnfAggrOptInfo       * aAggrOptInfo,
                                            qllDataArea           * aDataArea,
                                            qlnfAggrExecInfo      * aAggrExecInfo,
                                            knlRegionMem          * aRegionMem,
                                            qllEnv                * aEnv );

stlStatus qlnfCountAsteriskInit( qlnfAggrExecInfo   * aAggrExecInfo,
                                 stlInt32             aBlockIdx,
                                 qllEnv             * aEnv );

stlStatus qlnfCountAsteriskBuild( qlnfAggrExecInfo   * aAggrExecInfo,
                                  stlInt32             aStartIdx,
                                  stlInt32             aBlockCnt,
                                  stlInt32             aResultBlockIdx,
                                  qllEnv             * aEnv );


/* min */
stlStatus qlnfMinSetAggrDBType( qlvInitExpression  * aInitExpr,
                                dtlDataType          aDBType,
                                qloExprInfo        * aExprInfo,
                                qllEnv             * aEnv );

stlStatus qlnfMinGetAggrExecInfo( qllStatement          * aSQLStmt,
                                  qlnfAggrOptInfo       * aAggrOptInfo,
                                  qllDataArea           * aDataArea,
                                  qlnfAggrExecInfo      * aAggrExecInfo,
                                  knlRegionMem          * aRegionMem,
                                  qllEnv                * aEnv );

stlStatus qlnfMinInit( qlnfAggrExecInfo   * aAggrExecInfo,
                       stlInt32             aBlockIdx,
                       qllEnv             * aEnv );

stlStatus qlnfMinBuild( qlnfAggrExecInfo   * aAggrExecInfo,
                        stlInt32             aStartIdx,
                        stlInt32             aBlockCnt,
                        stlInt32             aResultBlockIdx,
                        qllEnv             * aEnv );


/* max */
stlStatus qlnfMaxSetAggrDBType( qlvInitExpression  * aInitExpr,
                                dtlDataType          aDBType,
                                qloExprInfo        * aExprInfo,
                                qllEnv             * aEnv );

stlStatus qlnfMaxGetAggrExecInfo( qllStatement          * aSQLStmt,
                                  qlnfAggrOptInfo       * aAggrOptInfo,
                                  qllDataArea           * aDataArea,
                                  qlnfAggrExecInfo      * aAggrExecInfo,
                                  knlRegionMem          * aRegionMem,
                                  qllEnv                * aEnv );

stlStatus qlnfMaxInit( qlnfAggrExecInfo   * aAggrExecInfo,
                       stlInt32             aBlockIdx,
                       qllEnv             * aEnv );

stlStatus qlnfMaxBuild( qlnfAggrExecInfo   * aAggrExecInfo,
                        stlInt32             aStartIdx,
                        stlInt32             aBlockCnt,
                        stlInt32             aResultBlockIdx,
                        qllEnv             * aEnv );


/** @} qlnf */



/**
 * @addtogroup qlnfAggr
 * @{
 */






/** @} qlnfAggr */


#endif /* _QLNFNODE_H_ */
