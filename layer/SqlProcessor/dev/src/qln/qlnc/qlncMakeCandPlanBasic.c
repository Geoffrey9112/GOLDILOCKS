/*******************************************************************************
 * qlncMakeCandPlanBasic.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlncMakeCandPlanBasic.c 9777 2013-10-04 02:22:51Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file qlncMakeCandPlanBasic.c
 * @brief SQL Processor Layer Make Candidate Plan Basic Info
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Make Candidate Plan Basic Function List
 */
const qlncMakeCandPlanBasicFuncPtr qlncMakeCandPlanBasicFuncList[ QLNC_BEST_OPT_TYPE_MAX ] =
{
    NULL,                               /**< QLNC_BEST_OPT_TYPE_NA */

    qlncMakeCandPlanBasicQuerySet,      /**< QLNC_BEST_OPT_TYPE_QUERY_SET */
    qlncMakeCandPlanBasicSetOP,         /**< QLNC_BEST_OPT_TYPE_SET_OP */
    qlncMakeCandPlanBasicQuerySpec,     /**< QLNC_BEST_OPT_TYPE_QUERY_SPEC */

    qlncMakeCandPlanBasicTableScan,     /**< QLNC_BEST_OPT_TYPE_TABLE_SCAN */
    qlncMakeCandPlanBasicIndexScan,     /**< QLNC_BEST_OPT_TYPE_INDEX_SCAN */
    qlncMakeCandPlanBasicIndexCombine,  /**< QLNC_BEST_OPT_TYPE_INDEX_COMBINE */
    qlncMakeCandPlanBasicRowidScan,     /**< QLNC_BEST_OPT_TYPE_ROWID_SCAN */

    qlncMakeCandPlanBasicSubTable,      /**< QLNC_BEST_OPT_TYPE_FILTER */

    qlncMakeCandPlanBasicInnerJoin,     /**< QLNC_BEST_OPT_TYPE_INNER_JOIN */
    qlncMakeCandPlanBasicOuterJoin,     /**< QLNC_BEST_OPT_TYPE_OUTER_JOIN */
    qlncMakeCandPlanBasicSemiJoin,      /**< QLNC_BEST_OPT_TYPE_SEMI_JOIN */
    qlncMakeCandPlanBasicAntiSemiJoin,  /**< QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN */

    qlncMakeCandPlanBasicNestedLoops,   /**< QLNC_BEST_OPT_TYPE_NESTED_LOOPS */
    qlncMakeCandPlanBasicMerge,         /**< QLNC_BEST_OPT_TYPE_MERGE */
    qlncMakeCandPlanBasicHash,          /**< QLNC_BEST_OPT_TYPE_HASH */
    qlncMakeCandPlanBasicJoinCombine,   /**< QLNC_BEST_OPT_TYPE_JOIN_COMBINE */

    qlncMakeCandPlanBasicSortInstant,   /**< QLNC_BEST_OPT_TYPE_SORT_INSTANT */
    qlncMakeCandPlanBasicHashInstant,   /**< QLNC_BEST_OPT_TYPE_HASH_INSTANT */
    qlncMakeCandPlanBasicGroup          /**< QLNC_BEST_OPT_TYPE_GROUP */
};


/**
 * @brief Query Set Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicQuerySet( qlncPlanBasicParamInfo     * aPlanBasicParamInfo,
                                         qlncNodeCommon             * aCandNode,
                                         qlncPlanCommon            ** aPlanNode,
                                         qllEnv                     * aEnv )
{
    qlncPlanQuerySet    * sBackupRootQuerySetPlanNode   = NULL;
    qlvInitNode         * sBackupInitQuerySetNode       = NULL;
    qlncPlanQuerySet    * sPlanQuerySet                 = NULL;
    qlvInitQuerySetNode * sValidateQuerySet             = NULL;
    qlncNodeArray         sNodeArray;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( aCandNode,
                                             QLNC_NODE_TYPE_QUERY_SET,
                                             QLNC_BEST_OPT_TYPE_QUERY_SET,
                                             aEnv );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    sBackupRootQuerySetPlanNode = aPlanBasicParamInfo->mRootQuerySetPlanNode;
    sBackupInitQuerySetNode = aPlanBasicParamInfo->mInitQuerySetNode;

    /* Query Set Candidate Plan 생성 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mCandidateMem,
                                sPlanQuerySet,
                                STL_SIZEOF( qlncPlanQuerySet ),
                                aEnv );

    /* Query Set Validation Node 생성 및 설정 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mRegionMem,
                                sValidateQuerySet,
                                STL_SIZEOF( qlvInitQuerySetNode ),
                                aEnv );

    sValidateQuerySet->mType = QLV_NODE_TYPE_QUERY_SET;

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanQuerySet->mCommon),
                          QLNC_PLAN_TYPE_QUERY_SET,
                          aCandNode,
                          (qlvInitNode*)sValidateQuerySet,
                          aPlanBasicParamInfo->mNeedRowBlock );

    /* Root Set 여부 설정 */
    sPlanQuerySet->mIsRootSet = ((qlncQuerySet*)aCandNode)->mIsRootSet;

    if( sPlanQuerySet->mIsRootSet == STL_TRUE )
    {
        aPlanBasicParamInfo->mRootQuerySetPlanNode = sPlanQuerySet;
    }

    /* Root Query Set Plan Node 설정 */
    sPlanQuerySet->mRootQuerySet = aPlanBasicParamInfo->mRootQuerySetPlanNode;

    /* Child Node 정보 설정 */
    aPlanBasicParamInfo->mInitQuerySetNode = sPlanQuerySet->mCommon.mInitNode;

    STL_TRY( qlncMakeCandPlanBasicFuncList[ ((qlncQuerySet*)aCandNode)->mChildNode->mBestOptType ](
                 aPlanBasicParamInfo,
                 ((qlncQuerySet*)aCandNode)->mChildNode,
                 &(sPlanQuerySet->mChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    /* Node Array 초기화 */
    STL_TRY( qlncInitializeNodeArray( aPlanBasicParamInfo->mCandidateMem,
                                      &sNodeArray,
                                      aEnv )
             == STL_SUCCESS );

    /* Node Array 생성 */
    STL_TRY( qlncMakeNodeArrayOnCurrNode( &sNodeArray,
                                          (qlncNodeCommon*)aCandNode,
                                          aEnv )
             == STL_SUCCESS );

    /* Need Build 여부 설정 */
    sPlanQuerySet->mNeedRebuild = qlncIsExistOuterColumnOnCandNode( aCandNode,
                                                                    &sNodeArray );

    aPlanBasicParamInfo->mRootQuerySetPlanNode = sBackupRootQuerySetPlanNode;
    aPlanBasicParamInfo->mInitQuerySetNode = sBackupInitQuerySetNode;

    /* Out Plan 설정 */
    *aPlanNode = (qlncPlanCommon*)sPlanQuerySet;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Set OP Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicSetOP( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                      qlncNodeCommon            * aCandNode,
                                      qlncPlanCommon           ** aPlanNode,
                                      qllEnv                    * aEnv )
{
    stlInt32          i;
    qlncSetOP       * sNodeSetOP    = NULL;
    qlncPlanSetOP   * sPlanSetOP    = NULL;
    qlncNodeArray     sNodeArray;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( aCandNode,
                                             QLNC_NODE_TYPE_SET_OP,
                                             QLNC_BEST_OPT_TYPE_SET_OP,
                                             aEnv );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* Set OP Node 설정 */
    sNodeSetOP = (qlncSetOP*)aCandNode;

    /* Set OP Candidate Plan 생성 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mCandidateMem,
                                sPlanSetOP,
                                STL_SIZEOF( qlncPlanSetOP ),
                                aEnv );

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanSetOP->mCommon),
                          QLNC_PLAN_TYPE_SET_OP,
                          (qlncNodeCommon*)sNodeSetOP,
                          aPlanBasicParamInfo->mInitQuerySetNode,
                          aPlanBasicParamInfo->mNeedRowBlock );

    /* Root Query Set Plan Node 설정 */
    sPlanSetOP->mRootQuerySet = aPlanBasicParamInfo->mRootQuerySetPlanNode;

    /* Set Type 설정 */
    if( sNodeSetOP->mSetType == QLNC_SET_TYPE_UNION )
    {
        sPlanSetOP->mSetType = QLV_SET_TYPE_UNION;
    }
    else if( sNodeSetOP->mSetType == QLNC_SET_TYPE_EXCEPT )
    {
        sPlanSetOP->mSetType = QLV_SET_TYPE_EXCEPT;
    }
    else
    {
        STL_DASSERT( sNodeSetOP->mSetType == QLNC_SET_TYPE_INTERSECT );
        sPlanSetOP->mSetType = QLV_SET_TYPE_INTERSECT;
    }

    /* Set Option 설정 */
    if( sNodeSetOP->mSetOption == QLNC_SET_OPTION_ALL )
    {
        sPlanSetOP->mSetOption = QLV_SET_OPTION_ALL;
    }
    else
    {
        STL_DASSERT( sNodeSetOP->mSetOption == QLNC_SET_OPTION_DISTINCT );
        sPlanSetOP->mSetOption = QLV_SET_OPTION_DISTINCT;
    }

    /* Child Count 설정 */
    sPlanSetOP->mChildCount = sNodeSetOP->mChildCount;

    /* Child Node Array 할당 */
    STL_TRY( knlAllocRegionMem( aPlanBasicParamInfo->mCandidateMem,
                                STL_SIZEOF( qlncPlanCommon* ) * sPlanSetOP->mChildCount,
                                (void**) &(sPlanSetOP->mChildPlanNodeArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Child Node Array 설정 */
    for( i = 0; i < sPlanSetOP->mChildCount; i++ )
    {
        STL_TRY( qlncMakeCandPlanBasicFuncList[ sNodeSetOP->mChildNodeArr[i]->mBestOptType ](
                     aPlanBasicParamInfo,
                     sNodeSetOP->mChildNodeArr[i],
                     &(sPlanSetOP->mChildPlanNodeArr[i]),
                     aEnv )
                 == STL_SUCCESS );
    }
        
    /* Node Array 초기화 */
    STL_TRY( qlncInitializeNodeArray( aPlanBasicParamInfo->mCandidateMem,
                                      &sNodeArray,
                                      aEnv )
             == STL_SUCCESS );

    /* Node Array 생성 */
    STL_TRY( qlncMakeNodeArrayOnCurrNode( &sNodeArray,
                                          (qlncNodeCommon*)aCandNode,
                                          aEnv )
             == STL_SUCCESS );

    /* Need Build 여부 설정 */
    sPlanSetOP->mNeedRebuild = qlncIsExistOuterColumnOnCandNode( aCandNode,
                                                                 &sNodeArray );


    /* Out Plan 설정 */
    *aPlanNode = (qlncPlanCommon*)sPlanSetOP;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Spec Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicQuerySpec( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                          qlncNodeCommon            * aCandNode,
                                          qlncPlanCommon           ** aPlanNode,
                                          qllEnv                    * aEnv )
{
    qlncQuerySpec           * sNodeQuerySpec        = NULL;
    qlncPlanQuerySpec       * sPlanQuerySpec        = NULL;
    qlvInitQuerySpecNode    * sValidateQuerySpec    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( aCandNode,
                                             QLNC_NODE_TYPE_QUERY_SPEC,
                                             QLNC_BEST_OPT_TYPE_QUERY_SPEC,
                                             aEnv );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* Query Spec Node 설정 */
    sNodeQuerySpec = (qlncQuerySpec*)aCandNode;

    /* Query Spec Candidate Plan 생성 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mCandidateMem,
                                sPlanQuerySpec,
                                STL_SIZEOF( qlncPlanQuerySpec ),
                                aEnv );

    /* Query Spec Validation Node 생성 및 설정 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mRegionMem,
                                sValidateQuerySpec,
                                STL_SIZEOF( qlvInitQuerySpecNode ),
                                aEnv );

    sValidateQuerySpec->mType = QLV_NODE_TYPE_QUERY_SPEC;

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanQuerySpec->mCommon),
                          QLNC_PLAN_TYPE_QUERY_SPEC,
                          (qlncNodeCommon*)sNodeQuerySpec,
                          (qlvInitNode*)sValidateQuerySpec,
                          aPlanBasicParamInfo->mNeedRowBlock );

    /* Distinct 여부 */
    sPlanQuerySpec->mIsDistinct = sNodeQuerySpec->mIsDistinct;

    /* Child Node 정보 설정 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ sNodeQuerySpec->mQuerySpecCost->mChildNode->mBestOptType ](
                 aPlanBasicParamInfo,
                 sNodeQuerySpec->mQuerySpecCost->mChildNode,
                 &(sPlanQuerySpec->mChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    /* Out Plan 설정 */
    *aPlanNode = (qlncPlanCommon*)sPlanQuerySpec;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Table Scan Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicTableScan( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                          qlncNodeCommon            * aCandNode,
                                          qlncPlanCommon           ** aPlanNode,
                                          qllEnv                    * aEnv )
{
    qlncBaseTableNode       * sNodeBaseTable        = NULL;
    qlncPlanTableScan       * sPlanTableScan        = NULL;
    qlvInitBaseTableNode    * sValidateBaseTable    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( aCandNode,
                                             QLNC_NODE_TYPE_BASE_TABLE,
                                             QLNC_BEST_OPT_TYPE_TABLE_SCAN,
                                             aEnv );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* Base Table Node 설정 */
    sNodeBaseTable = (qlncBaseTableNode*)aCandNode;

    /* Table Scan Candidate Plan 생성 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mCandidateMem,
                                sPlanTableScan,
                                STL_SIZEOF( qlncPlanTableScan ),
                                aEnv );

    /* Base Table Validation Node 생성 및 설정 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mRegionMem,
                                sValidateBaseTable,
                                STL_SIZEOF( qlvInitBaseTableNode ),
                                aEnv );

    sValidateBaseTable->mType = QLV_NODE_TYPE_BASE_TABLE;
    sValidateBaseTable->mIsIndexScanAsc = STL_FALSE;
    
    QLNC_ALLOC_AND_COPY_RELATION_NAME( aPlanBasicParamInfo->mRegionMem,
                                       &(sNodeBaseTable->mTableInfo->mRelationName),
                                       sValidateBaseTable->mRelationName,
                                       aEnv );

    /* Base Table Validation Node 기본정보 설정 */
    stlMemcpy( &(sValidateBaseTable->mSchemaHandle),
               sNodeBaseTable->mTableInfo->mSchemaHandle,
               STL_SIZEOF( ellDictHandle ) );
    stlMemcpy( &(sValidateBaseTable->mTableHandle),
               sNodeBaseTable->mTableInfo->mTableHandle,
               STL_SIZEOF( ellDictHandle ) );
    sValidateBaseTable->mTablePhyHandle = sNodeBaseTable->mTableInfo->mTablePhyHandle;
    QLNC_ALLOC_AND_COPY_STRING( sValidateBaseTable->mDumpOption,
                                sNodeBaseTable->mTableInfo->mDumpOption,
                                aPlanBasicParamInfo->mRegionMem,
                                aEnv );

    sValidateBaseTable->mIndexHandle = NULL;
    sValidateBaseTable->mIndexPhyHandle = NULL;

    STL_TRY( qlvCreateRefExprList( &(sValidateBaseTable->mOuterColumnList),
                                   aPlanBasicParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanTableScan->mCommon),
                          QLNC_PLAN_TYPE_TABLE_SCAN,
                          (qlncNodeCommon*)sNodeBaseTable,
                          (qlvInitNode*)sValidateBaseTable,
                          aPlanBasicParamInfo->mNeedRowBlock );

    /* Table Handle 설정 */
    QLNC_ALLOC_AND_COPY_DICT_HANDLE( aPlanBasicParamInfo->mRegionMem,
                                     sNodeBaseTable->mTableInfo->mTableHandle,
                                     sPlanTableScan->mTableHandle,
                                     aEnv );

    /* Table Physical Handle 설정 */
    sPlanTableScan->mPhysicalHandle = sNodeBaseTable->mTableInfo->mTablePhyHandle;

    /* Dump Option 설정 */
    QLNC_ALLOC_AND_COPY_STRING( sPlanTableScan->mDumpOption,
                                sNodeBaseTable->mTableInfo->mDumpOption,
                                aPlanBasicParamInfo->mRegionMem,
                                aEnv );

    /* Outer Column List 설정 */
    sPlanTableScan->mOuterColumnList = sValidateBaseTable->mOuterColumnList;

    /* Out Plan 설정 */
    *aPlanNode = (qlncPlanCommon*)sPlanTableScan;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index Scan Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicIndexScan( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                          qlncNodeCommon            * aCandNode,
                                          qlncPlanCommon           ** aPlanNode,
                                          qllEnv                    * aEnv )
{
    qlncBaseTableNode       * sNodeBaseTable        = NULL;
    qlncPlanIndexScan       * sPlanIndexScan        = NULL;
    qlvInitBaseTableNode    * sValidateBaseTable    = NULL;
    qlncIndexScanCost       * sIndexScanCost        = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( aCandNode,
                                             QLNC_NODE_TYPE_BASE_TABLE,
                                             QLNC_BEST_OPT_TYPE_INDEX_SCAN,
                                             aEnv );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* Base Table Node 설정 */
    sNodeBaseTable = (qlncBaseTableNode*)aCandNode;

    /* Index Scan Cost 설정 */
    sIndexScanCost = (qlncIndexScanCost*)(sNodeBaseTable->mNodeCommon.mBestCost);

    /* Index Scan Candidate Plan 생성 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mCandidateMem,
                                sPlanIndexScan,
                                STL_SIZEOF( qlncPlanIndexScan ),
                                aEnv );

    /* Base Table Validation Node 생성 및 설정 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mRegionMem,
                                sValidateBaseTable,
                                STL_SIZEOF( qlvInitBaseTableNode ),
                                aEnv );

    sValidateBaseTable->mType = QLV_NODE_TYPE_BASE_TABLE;
    sValidateBaseTable->mIsIndexScanAsc = sIndexScanCost->mIsAsc;
    
    QLNC_ALLOC_AND_COPY_RELATION_NAME( aPlanBasicParamInfo->mRegionMem,
                                       &(sNodeBaseTable->mTableInfo->mRelationName),
                                       sValidateBaseTable->mRelationName,
                                       aEnv );

    STL_TRY( qlvCreateRefExprList( &(sValidateBaseTable->mOuterColumnList),
                                   aPlanBasicParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Base Schema Validation Node 기본정보 설정 */
    stlMemcpy( &(sValidateBaseTable->mSchemaHandle),
               sNodeBaseTable->mTableInfo->mSchemaHandle,
               STL_SIZEOF( ellDictHandle ) );
    
    /* Base Table Validation Node 기본정보 설정 */
    stlMemcpy( &(sValidateBaseTable->mTableHandle),
               sNodeBaseTable->mTableInfo->mTableHandle,
               STL_SIZEOF( ellDictHandle ) );
    sValidateBaseTable->mTablePhyHandle = sNodeBaseTable->mTableInfo->mTablePhyHandle;

    QLNC_ALLOC_AND_COPY_DICT_HANDLE( aPlanBasicParamInfo->mRegionMem,
                                     sIndexScanCost->mIndexHandle,
                                     sValidateBaseTable->mIndexHandle,
                                     aEnv );

    sValidateBaseTable->mIndexPhyHandle = sIndexScanCost->mIndexPhyHandle;

    QLNC_ALLOC_AND_COPY_STRING( sValidateBaseTable->mDumpOption,
                                sNodeBaseTable->mTableInfo->mDumpOption,
                                aPlanBasicParamInfo->mRegionMem,
                                aEnv );

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanIndexScan->mCommon),
                          QLNC_PLAN_TYPE_INDEX_SCAN,
                          (qlncNodeCommon*)sNodeBaseTable,
                          (qlvInitNode*)sValidateBaseTable,
                          aPlanBasicParamInfo->mNeedRowBlock );

    /* Table Handle 설정 */
    QLNC_ALLOC_AND_COPY_DICT_HANDLE( aPlanBasicParamInfo->mRegionMem,
                                     sNodeBaseTable->mTableInfo->mTableHandle,
                                     sPlanIndexScan->mTableHandle,
                                     aEnv );

    /* Table Physical Handle 설정 */
    sPlanIndexScan->mTablePhysicalHandle = sNodeBaseTable->mTableInfo->mTablePhyHandle;

    /* Index Handle 설정 */
    QLNC_ALLOC_AND_COPY_DICT_HANDLE( aPlanBasicParamInfo->mRegionMem,
                                     sIndexScanCost->mIndexHandle,
                                     sPlanIndexScan->mIndexHandle,
                                     aEnv );

    /* Index Physical Handle 설정 */
    sPlanIndexScan->mIndexPhysicalHandle = sIndexScanCost->mIndexPhyHandle;

    /* Index Scan Asc 설정 */
    sPlanIndexScan->mIsIndexScanAsc = sIndexScanCost->mIsAsc;

    /* Outer Column List 설정 */
    sPlanIndexScan->mOuterColumnList = sValidateBaseTable->mOuterColumnList;

    /* Out Plan 설정 */
    *aPlanNode = (qlncPlanCommon*)sPlanIndexScan;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index Combine Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicIndexCombine( qlncPlanBasicParamInfo     * aPlanBasicParamInfo,
                                             qlncNodeCommon             * aCandNode,
                                             qlncPlanCommon            ** aPlanNode,
                                             qllEnv                     * aEnv )
{
    stlBool                   sBackupNeedRowBlock;
    stlInt32                  i;
    qlncBaseTableNode       * sNodeBaseTable        = NULL;
    qlncPlanIndexCombine    * sPlanIndexCombine     = NULL;
    qlvInitBaseTableNode    * sValidateBaseTable    = NULL;
    qlncIndexCombineCost    * sIndexCombineCost     = NULL;

    qlncBestOptType           sBackupBestOptType;
    qlncCostCommon          * sBackupBestCost       = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( aCandNode,
                                             QLNC_NODE_TYPE_BASE_TABLE,
                                             QLNC_BEST_OPT_TYPE_INDEX_COMBINE,
                                             aEnv );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* Base Table Node 설정 */
    sNodeBaseTable = (qlncBaseTableNode*)aCandNode;

    /* Index Combine Cost 설정 */
    sIndexCombineCost = (qlncIndexCombineCost*)(sNodeBaseTable->mNodeCommon.mBestCost);

    /* Index Combine Candidate Plan 생성 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mCandidateMem,
                                sPlanIndexCombine,
                                STL_SIZEOF( qlncPlanIndexCombine ),
                                aEnv );

    /* Base Table Validation Node 생성 및 설정 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mRegionMem,
                                sValidateBaseTable,
                                STL_SIZEOF( qlvInitBaseTableNode ),
                                aEnv );

    sValidateBaseTable->mType = QLV_NODE_TYPE_BASE_TABLE;
    sValidateBaseTable->mIsIndexScanAsc = STL_FALSE;

    QLNC_ALLOC_AND_COPY_RELATION_NAME( aPlanBasicParamInfo->mRegionMem,
                                       &(sNodeBaseTable->mTableInfo->mRelationName),
                                       sValidateBaseTable->mRelationName,
                                       aEnv );

    STL_TRY( qlvCreateRefExprList( &(sValidateBaseTable->mOuterColumnList),
                                   aPlanBasicParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Base Table Validation Node 기본정보 설정 */
    stlMemcpy( &(sValidateBaseTable->mSchemaHandle),
               sNodeBaseTable->mTableInfo->mSchemaHandle,
               STL_SIZEOF( ellDictHandle ) );
    
    /* Base Table Validation Node 기본정보 설정 */
    stlMemcpy( &(sValidateBaseTable->mTableHandle),
               sNodeBaseTable->mTableInfo->mTableHandle,
               STL_SIZEOF( ellDictHandle ) );
    sValidateBaseTable->mTablePhyHandle = sNodeBaseTable->mTableInfo->mTablePhyHandle;
    QLNC_ALLOC_AND_COPY_STRING( sValidateBaseTable->mDumpOption,
                                sNodeBaseTable->mTableInfo->mDumpOption,
                                aPlanBasicParamInfo->mRegionMem,
                                aEnv );

    sValidateBaseTable->mIndexHandle = NULL;
    sValidateBaseTable->mIndexPhyHandle = NULL;
    
    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanIndexCombine->mCommon),
                          QLNC_PLAN_TYPE_INDEX_COMBINE,
                          (qlncNodeCommon*)sNodeBaseTable,
                          (qlvInitNode*)sValidateBaseTable,
                          aPlanBasicParamInfo->mNeedRowBlock );

    /* Table Handle 설정 */
    QLNC_ALLOC_AND_COPY_DICT_HANDLE( aPlanBasicParamInfo->mRegionMem,
                                     sNodeBaseTable->mTableInfo->mTableHandle,
                                     sPlanIndexCombine->mTableHandle,
                                     aEnv );

    /* Outer Column List 설정 */
    sPlanIndexCombine->mOuterColumnList = sValidateBaseTable->mOuterColumnList;

    /* Child Count 설정 */
    sPlanIndexCombine->mChildCount = sIndexCombineCost->mDnfAndFilterCount;

    /* Child Plan Pointer Array 할당 */
    STL_TRY( knlAllocRegionMem( aPlanBasicParamInfo->mCandidateMem,
                                STL_SIZEOF( qlncPlanCommon* ) * sIndexCombineCost->mDnfAndFilterCount,
                                (void**) &(sPlanIndexCombine->mChildPlanArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Child Node 정보 설정 */
    sBackupNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
    sBackupBestOptType = sNodeBaseTable->mNodeCommon.mBestOptType;
    sBackupBestCost = sNodeBaseTable->mNodeCommon.mBestCost;

    aPlanBasicParamInfo->mNeedRowBlock = STL_TRUE;
    for( i = 0; i < sIndexCombineCost->mDnfAndFilterCount; i++ )
    {
        sNodeBaseTable->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_INDEX_SCAN;
        sNodeBaseTable->mNodeCommon.mBestCost = (qlncCostCommon*)(&(sIndexCombineCost->mIndexScanCostArr[i]));
        STL_TRY( qlncMakeCandPlanBasicIndexScan( aPlanBasicParamInfo,
                                                 aCandNode,
                                                 &(sPlanIndexCombine->mChildPlanArr[i]),
                                                 aEnv )
                 == STL_SUCCESS );
    }

    aPlanBasicParamInfo->mNeedRowBlock = sBackupNeedRowBlock;
    sNodeBaseTable->mNodeCommon.mBestOptType = sBackupBestOptType;
    sNodeBaseTable->mNodeCommon.mBestCost = sBackupBestCost;

    /* Out Plan 설정 */
    *aPlanNode = (qlncPlanCommon*)sPlanIndexCombine;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Rowid Scan Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicRowidScan( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                          qlncNodeCommon            * aCandNode,
                                          qlncPlanCommon           ** aPlanNode,
                                          qllEnv                    * aEnv )
{
    qlncBaseTableNode       * sNodeBaseTable        = NULL;
    qlncPlanRowidScan       * sPlanRowidScan        = NULL;
    qlvInitBaseTableNode    * sValidateBaseTable    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( aCandNode,
                                             QLNC_NODE_TYPE_BASE_TABLE,
                                             QLNC_BEST_OPT_TYPE_ROWID_SCAN,
                                             aEnv );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* Base Table Node 설정 */
    sNodeBaseTable = (qlncBaseTableNode*)aCandNode;

    /* Rowid Scan Candidate Plan 생성 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mCandidateMem,
                                sPlanRowidScan,
                                STL_SIZEOF( qlncPlanRowidScan ),
                                aEnv );
   
    /* Base Table Validation Node 생성 및 설정 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mRegionMem,
                                sValidateBaseTable,
                                STL_SIZEOF( qlvInitBaseTableNode ),
                                aEnv );

    sValidateBaseTable->mType = QLV_NODE_TYPE_BASE_TABLE;
    sValidateBaseTable->mIsIndexScanAsc = STL_FALSE;
    
    QLNC_ALLOC_AND_COPY_RELATION_NAME( aPlanBasicParamInfo->mRegionMem,
                                       &(sNodeBaseTable->mTableInfo->mRelationName),
                                       sValidateBaseTable->mRelationName,
                                       aEnv );

    STL_TRY( qlvCreateRefExprList( &(sValidateBaseTable->mOuterColumnList),
                                   aPlanBasicParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Base Table Validation Node 기본정보 설정 */
    stlMemcpy( &(sValidateBaseTable->mSchemaHandle),
               sNodeBaseTable->mTableInfo->mSchemaHandle,
               STL_SIZEOF( ellDictHandle ) );

    /* Base Table Validation Node 기본정보 설정 */
    stlMemcpy( &(sValidateBaseTable->mTableHandle),
               sNodeBaseTable->mTableInfo->mTableHandle,
               STL_SIZEOF( ellDictHandle ) );
    sValidateBaseTable->mTablePhyHandle = sNodeBaseTable->mTableInfo->mTablePhyHandle;
    QLNC_ALLOC_AND_COPY_STRING( sValidateBaseTable->mDumpOption,
                                sNodeBaseTable->mTableInfo->mDumpOption,
                                aPlanBasicParamInfo->mRegionMem,
                                aEnv );

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanRowidScan->mCommon),
                          QLNC_PLAN_TYPE_ROWID_SCAN,
                          (qlncNodeCommon*)sNodeBaseTable,
                          (qlvInitNode*)sValidateBaseTable,
                          aPlanBasicParamInfo->mNeedRowBlock );

    /* Table Handle 설정 */
    QLNC_ALLOC_AND_COPY_DICT_HANDLE( aPlanBasicParamInfo->mRegionMem,
                                     sNodeBaseTable->mTableInfo->mTableHandle,
                                     sPlanRowidScan->mTableHandle,
                                     aEnv );

    /* Table Physical Handle 설정 */
    sPlanRowidScan->mPhysicalHandle = sNodeBaseTable->mTableInfo->mTablePhyHandle;

    /* Outer Column List 설정 */
    sPlanRowidScan->mOuterColumnList = sValidateBaseTable->mOuterColumnList;

    /* Out Plan 설정 */
    *aPlanNode = (qlncPlanCommon*)sPlanRowidScan;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Table Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicSubTable( qlncPlanBasicParamInfo     * aPlanBasicParamInfo,
                                         qlncNodeCommon             * aCandNode,
                                         qlncPlanCommon            ** aPlanNode,
                                         qllEnv                     * aEnv )
{
    qlncSubTableNode    * sNodeSubTable     = NULL;
    qlncPlanSubTable    * sPlanSubTable     = NULL;
    qlvInitSubTableNode * sValidateSubTable = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( aCandNode,
                                             QLNC_NODE_TYPE_SUB_TABLE,
                                             QLNC_BEST_OPT_TYPE_FILTER,
                                             aEnv );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* Sub Table Node 설정 */
    sNodeSubTable = (qlncSubTableNode*)aCandNode;

    if( sNodeSubTable->mFilterCost->mMaterializedNode != NULL )
    {
        return qlncMakeCandPlanBasicFuncList[ sNodeSubTable->mFilterCost->mMaterializedNode->mBestOptType ](
                 aPlanBasicParamInfo,
                 sNodeSubTable->mFilterCost->mMaterializedNode,
                 aPlanNode,
                 aEnv );
    }

    /* Sub Table Candidate Plan 생성 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mCandidateMem,
                                sPlanSubTable,
                                STL_SIZEOF( qlncPlanSubTable ),
                                aEnv );

    /* Sub Table Validation Node 생성 및 설정 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mRegionMem,
                                sValidateSubTable,
                                STL_SIZEOF( qlvInitSubTableNode ),
                                aEnv );

    sValidateSubTable->mType = QLV_NODE_TYPE_SUB_TABLE;

    STL_DASSERT( sNodeSubTable->mRelationName != NULL );
    QLNC_ALLOC_AND_COPY_RELATION_NAME( aPlanBasicParamInfo->mRegionMem,
                                       sNodeSubTable->mRelationName,
                                       sValidateSubTable->mRelationName,
                                       aEnv );

    sValidateSubTable->mIsView = STL_TRUE;
    sValidateSubTable->mHasHandle = sNodeSubTable->mHasHandle;
    stlMemcpy( &(sValidateSubTable->mSchemaHandle),
               &(sNodeSubTable->mSchemaHandle),
               STL_SIZEOF( ellDictHandle ) );
    stlMemcpy( &(sValidateSubTable->mViewHandle),
               &(sNodeSubTable->mViewHandle),
               STL_SIZEOF( ellDictHandle ) );
    
    STL_TRY( qlvCreateRefExprList( &(sValidateSubTable->mOuterColumnList),
                                   aPlanBasicParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanSubTable->mCommon),
                          QLNC_PLAN_TYPE_SUB_TABLE,
                          (qlncNodeCommon*)sNodeSubTable,
                          (qlvInitNode*)sValidateSubTable,
                          aPlanBasicParamInfo->mNeedRowBlock );

    /* Child Plan 설정 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ sNodeSubTable->mQueryNode->mBestOptType ](
                 aPlanBasicParamInfo,
                 sNodeSubTable->mQueryNode,
                 &(sPlanSubTable->mChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    /* Outer Column List 설정 */
    sPlanSubTable->mOuterColumnList = sValidateSubTable->mOuterColumnList;

    /* Out Plan 설정 */
    *aPlanNode = (qlncPlanCommon*)sPlanSubTable;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Inner Join Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicInnerJoin( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                          qlncNodeCommon            * aCandNode,
                                          qlncPlanCommon           ** aPlanNode,
                                          qllEnv                    * aEnv )
{
    qlncInnerJoinTableNode  * sInnerJoinTable   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( aCandNode,
                                             QLNC_NODE_TYPE_JOIN_TABLE,
                                             QLNC_BEST_OPT_TYPE_INNER_JOIN,
                                             aEnv );


    /* Inner Join Table 설정 */
    sInnerJoinTable = ((qlncJoinTableNode*)aCandNode)->mBestInnerJoinTable;
    STL_DASSERT( sInnerJoinTable != NULL );

    /* Inner Join Table 수행 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ sInnerJoinTable->mNodeCommon.mBestOptType ](
                 aPlanBasicParamInfo,
                 (qlncNodeCommon*)sInnerJoinTable,
                 aPlanNode,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Outer Join Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicOuterJoin( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                          qlncNodeCommon            * aCandNode,
                                          qlncPlanCommon           ** aPlanNode,
                                          qllEnv                    * aEnv )
{
    qlncOuterJoinTableNode  * sOuterJoinTable   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( aCandNode,
                                             QLNC_NODE_TYPE_JOIN_TABLE,
                                             QLNC_BEST_OPT_TYPE_OUTER_JOIN,
                                             aEnv );


    /* Outer Join Table 설정 */
    sOuterJoinTable = ((qlncJoinTableNode*)aCandNode)->mBestOuterJoinTable;
    STL_DASSERT( sOuterJoinTable != NULL );

    /* Outer Join Table 수행 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ sOuterJoinTable->mNodeCommon.mBestOptType ](
                 aPlanBasicParamInfo,
                 (qlncNodeCommon*)sOuterJoinTable,
                 aPlanNode,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Semi Join Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicSemiJoin( qlncPlanBasicParamInfo     * aPlanBasicParamInfo,
                                         qlncNodeCommon             * aCandNode,
                                         qlncPlanCommon            ** aPlanNode,
                                         qllEnv                     * aEnv )
{
    qlncSemiJoinTableNode   * sSemiJoinTable    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( aCandNode,
                                             QLNC_NODE_TYPE_JOIN_TABLE,
                                             QLNC_BEST_OPT_TYPE_SEMI_JOIN,
                                             aEnv );


    /* Semi Join Table 설정 */
    sSemiJoinTable = ((qlncJoinTableNode*)aCandNode)->mBestSemiJoinTable;
    STL_DASSERT( sSemiJoinTable != NULL );

    /* Semi Join Table 수행 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ sSemiJoinTable->mNodeCommon.mBestOptType ](
                 aPlanBasicParamInfo,
                 (qlncNodeCommon*)sSemiJoinTable,
                 aPlanNode,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Anti Semi Join Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicAntiSemiJoin( qlncPlanBasicParamInfo     * aPlanBasicParamInfo,
                                             qlncNodeCommon             * aCandNode,
                                             qlncPlanCommon            ** aPlanNode,
                                             qllEnv                     * aEnv )
{
    qlncAntiSemiJoinTableNode   * sAntiSemiJoinTable    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( aCandNode,
                                             QLNC_NODE_TYPE_JOIN_TABLE,
                                             QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN,
                                             aEnv );


    /* Anti Semi Join Table 설정 */
    sAntiSemiJoinTable = ((qlncJoinTableNode*)aCandNode)->mBestAntiSemiJoinTable;
    STL_DASSERT( sAntiSemiJoinTable != NULL );

    /* Anti Semi Join Table 수행 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ sAntiSemiJoinTable->mNodeCommon.mBestOptType ](
                 aPlanBasicParamInfo,
                 (qlncNodeCommon*)sAntiSemiJoinTable,
                 aPlanNode,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Nested Loops Join Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicNestedLoops( qlncPlanBasicParamInfo  * aPlanBasicParamInfo,
                                            qlncNodeCommon          * aCandNode,
                                            qlncPlanCommon         ** aPlanNode,
                                            qllEnv                  * aEnv )
{
    stlBool                   sLeftNeedRowBlock;
    stlBool                   sRightNeedRowBlock;
    stlBool                   sBackupNeedRowBlock;
    stlBool                   sNeedRowBlock;
    qlncPlanNestedJoin      * sPlanNestedJoin       = NULL;
    qlvInitJoinTableNode    * sValidateJoinTable    = NULL;
    qlncNestedJoinCost      * sNestedJoinCost       = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    STL_PARAM_VALIDATE( (aCandNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE) ||
                        (aCandNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE) ||
                        (aCandNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE) ||
                        (aCandNode->mNodeType == QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mIsAnalyzedCBO == STL_TRUE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mBestOptType == QLNC_BEST_OPT_TYPE_NESTED_LOOPS,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mBestCost != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* Nested Join Cost 설정 */
    sNestedJoinCost = (qlncNestedJoinCost*)(aCandNode->mBestCost);

    /* Nested Join Candidate Plan 생성 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mCandidateMem,
                                sPlanNestedJoin,
                                STL_SIZEOF( qlncPlanNestedJoin ),
                                aEnv );

    /* Join Table Validation Node 생성 및 설정 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mRegionMem,
                                sValidateJoinTable,
                                STL_SIZEOF( qlvInitJoinTableNode ),
                                aEnv );

    sValidateJoinTable->mType = QLV_NODE_TYPE_JOIN_TABLE;

    STL_TRY( qlvCreateRefExprList( &(sValidateJoinTable->mOuterColumnList),
                                   aPlanBasicParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Join Type 설정 */
    sNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
    if( aCandNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE )
    {
        STL_DASSERT( sNestedJoinCost->mIsInverted == STL_FALSE );
        sPlanNestedJoin->mJoinType = QLV_JOIN_TYPE_INNER;
        sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
        sRightNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
    }
    else if( aCandNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE )
    {
        STL_DASSERT( sNestedJoinCost->mIsInverted == STL_FALSE );
        switch( ((qlncOuterJoinTableNode*)(aCandNode))->mJoinDirect )
        {
            case QLNC_JOIN_DIRECT_LEFT:
                sPlanNestedJoin->mJoinType = QLV_JOIN_TYPE_LEFT_OUTER;
                break;

            case QLNC_JOIN_DIRECT_FULL:
                sPlanNestedJoin->mJoinType = QLV_JOIN_TYPE_FULL_OUTER;
                break;

            /* case QLNC_JOIN_DIRECT_RIGHT: */
            default:
                STL_DASSERT( 0 );
                break;
        }

        sNeedRowBlock = STL_FALSE;
        sLeftNeedRowBlock = STL_FALSE;
        sRightNeedRowBlock = STL_FALSE;
    }
    else if( aCandNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE )
    {
        switch( ((qlncSemiJoinTableNode*)(aCandNode))->mJoinDirect )
        {
            case QLNC_JOIN_DIRECT_LEFT:
                if( sNestedJoinCost->mIsInverted == STL_FALSE )
                {
                    sPlanNestedJoin->mJoinType = QLV_JOIN_TYPE_LEFT_SEMI;
                    sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                    sRightNeedRowBlock = STL_FALSE;
                }
                else
                {
                    sPlanNestedJoin->mJoinType = QLV_JOIN_TYPE_INVERTED_LEFT_SEMI;
                    sLeftNeedRowBlock = STL_FALSE;
                    sRightNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                }
                break;

            /* case QLNC_JOIN_DIRECT_RIGHT: */
            default:
                STL_DASSERT( 0 );
                sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                sRightNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                break;
        }
    }
    else
    {
        STL_DASSERT( aCandNode->mNodeType == QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE );
        STL_DASSERT( sNestedJoinCost->mIsInverted == STL_FALSE );
        switch( ((qlncAntiSemiJoinTableNode*)(aCandNode))->mJoinDirect )
        {
            case QLNC_JOIN_DIRECT_LEFT:
                sPlanNestedJoin->mJoinType = QLV_JOIN_TYPE_LEFT_ANTI_SEMI;
                sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                sRightNeedRowBlock = STL_FALSE;
                break;

            /* case QLNC_JOIN_DIRECT_RIGHT: */
            default:
                STL_DASSERT( 0 );
                sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                sRightNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                break;
        }
    }

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanNestedJoin->mCommon),
                          QLNC_PLAN_TYPE_NESTED_JOIN,
                          aCandNode,
                          (qlvInitNode*)sValidateJoinTable,
                          sNeedRowBlock );

    sBackupNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;

    aPlanBasicParamInfo->mNeedRowBlock = sLeftNeedRowBlock;

    /* Left Plan 설정 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ sNestedJoinCost->mLeftNode->mBestOptType ](
                 aPlanBasicParamInfo,
                 sNestedJoinCost->mLeftNode,
                 &(sPlanNestedJoin->mLeftChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    aPlanBasicParamInfo->mNeedRowBlock = sRightNeedRowBlock;

    /* Right Plan 설정 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ sNestedJoinCost->mRightNode->mBestOptType ](
                 aPlanBasicParamInfo,
                 sNestedJoinCost->mRightNode,
                 &(sPlanNestedJoin->mRightChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    aPlanBasicParamInfo->mNeedRowBlock = sBackupNeedRowBlock;

    /* Outer Column List 설정 */
    sPlanNestedJoin->mOuterColumnList = sValidateJoinTable->mOuterColumnList;

    /* Out Plan 설정 */
    *aPlanNode = (qlncPlanCommon*)sPlanNestedJoin;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Merge Join Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicMerge( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                      qlncNodeCommon            * aCandNode,
                                      qlncPlanCommon           ** aPlanNode,
                                      qllEnv                    * aEnv )
{
    stlBool                   sLeftNeedRowBlock;
    stlBool                   sRightNeedRowBlock;
    stlBool                   sBackupNeedRowBlock;
    stlBool                   sNeedRowBlock;
    qlncPlanMergeJoin       * sPlanMergeJoin        = NULL;
    qlvInitJoinTableNode    * sValidateJoinTable    = NULL;
    qlncMergeJoinCost       * sMergeJoinCost        = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    STL_PARAM_VALIDATE( (aCandNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE) ||
                        (aCandNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE) ||
                        (aCandNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE) ||
                        (aCandNode->mNodeType == QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mIsAnalyzedCBO == STL_TRUE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mBestOptType == QLNC_BEST_OPT_TYPE_MERGE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mBestCost != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* Merge Join Cost 설정 */
    sMergeJoinCost = (qlncMergeJoinCost*)(aCandNode->mBestCost);

    /* Merge Join Candidate Plan 생성 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mCandidateMem,
                                sPlanMergeJoin,
                                STL_SIZEOF( qlncPlanMergeJoin ),
                                aEnv );

    /* Join Table Validation Node 생성 및 설정 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mRegionMem,
                                sValidateJoinTable,
                                STL_SIZEOF( qlvInitJoinTableNode ),
                                aEnv );

    sValidateJoinTable->mType = QLV_NODE_TYPE_JOIN_TABLE;

    STL_TRY( qlvCreateRefExprList( &(sValidateJoinTable->mOuterColumnList),
                                   aPlanBasicParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Join Type 설정 */
    sNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
    if( aCandNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE )
    {
        sPlanMergeJoin->mJoinType = QLV_JOIN_TYPE_INNER;
        sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
        sRightNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
    }
    else if( aCandNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE )
    {
        switch( ((qlncOuterJoinTableNode*)(aCandNode))->mJoinDirect )
        {
            case QLNC_JOIN_DIRECT_LEFT:
                sPlanMergeJoin->mJoinType = QLV_JOIN_TYPE_LEFT_OUTER;
                break;

            case QLNC_JOIN_DIRECT_FULL:
                sPlanMergeJoin->mJoinType = QLV_JOIN_TYPE_FULL_OUTER;
                break;

            /* case QLNC_JOIN_DIRECT_RIGHT: */
            default:
                STL_DASSERT( 0 );
                break;
        }

        sNeedRowBlock = STL_FALSE;
        sLeftNeedRowBlock = STL_FALSE;
        sRightNeedRowBlock = STL_FALSE;
    }
    else if( aCandNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE )
    {
        switch( ((qlncSemiJoinTableNode*)(aCandNode))->mJoinDirect )
        {
            case QLNC_JOIN_DIRECT_LEFT:
                sPlanMergeJoin->mJoinType = QLV_JOIN_TYPE_LEFT_SEMI;
                sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                sRightNeedRowBlock = STL_FALSE;
                break;

            /* case QLNC_JOIN_DIRECT_RIGHT: */
            default:
                STL_DASSERT( 0 );
                sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                sRightNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                break;
        }
    }
    else
    {
        STL_DASSERT( aCandNode->mNodeType == QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE );
        switch( ((qlncAntiSemiJoinTableNode*)(aCandNode))->mJoinDirect )
        {
            case QLNC_JOIN_DIRECT_LEFT:
                sPlanMergeJoin->mJoinType = QLV_JOIN_TYPE_LEFT_ANTI_SEMI;
                sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                sRightNeedRowBlock = STL_FALSE;
                break;

            /* case QLNC_JOIN_DIRECT_RIGHT: */
            default:
                STL_DASSERT( 0 );
                sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                sRightNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                break;
        }
    }

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanMergeJoin->mCommon),
                          QLNC_PLAN_TYPE_MERGE_JOIN,
                          aCandNode,
                          (qlvInitNode*)sValidateJoinTable,
                          sNeedRowBlock );

    sBackupNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;

    aPlanBasicParamInfo->mNeedRowBlock = sLeftNeedRowBlock;

    /* Left Plan 설정 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ sMergeJoinCost->mLeftNode->mBestOptType ](
                 aPlanBasicParamInfo,
                 sMergeJoinCost->mLeftNode,
                 &(sPlanMergeJoin->mLeftChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    aPlanBasicParamInfo->mNeedRowBlock = sRightNeedRowBlock;

    /* Right Plan 설정 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ sMergeJoinCost->mRightNode->mBestOptType ](
                 aPlanBasicParamInfo,
                 sMergeJoinCost->mRightNode,
                 &(sPlanMergeJoin->mRightChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    aPlanBasicParamInfo->mNeedRowBlock = sBackupNeedRowBlock;

    /* Outer Column List 설정 */
    sPlanMergeJoin->mOuterColumnList = sValidateJoinTable->mOuterColumnList;

    /* Out Plan 설정 */
    *aPlanNode = (qlncPlanCommon*)sPlanMergeJoin;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Join Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicHash( qlncPlanBasicParamInfo     * aPlanBasicParamInfo,
                                     qlncNodeCommon             * aCandNode,
                                     qlncPlanCommon            ** aPlanNode,
                                     qllEnv                     * aEnv )
{
    stlBool                   sLeftNeedRowBlock;
    stlBool                   sRightNeedRowBlock;
    stlBool                   sBackupNeedRowBlock;
    stlBool                   sNeedRowBlock;
    qlncPlanHashJoin        * sPlanHashJoin         = NULL;
    qlvInitJoinTableNode    * sValidateJoinTable    = NULL;
    qlncHashJoinCost        * sHashJoinCost         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    STL_PARAM_VALIDATE( (aCandNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE) ||
                        (aCandNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE) ||
                        (aCandNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE) ||
                        (aCandNode->mNodeType == QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mIsAnalyzedCBO == STL_TRUE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mBestOptType == QLNC_BEST_OPT_TYPE_HASH,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mBestCost != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* Hash Join Cost 설정 */
    sHashJoinCost = (qlncHashJoinCost*)(aCandNode->mBestCost);

    /* Hash Join Candidate Plan 생성 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mCandidateMem,
                                sPlanHashJoin,
                                STL_SIZEOF( qlncPlanHashJoin ),
                                aEnv );

    /* Join Table Validation Node 생성 및 설정 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mRegionMem,
                                sValidateJoinTable,
                                STL_SIZEOF( qlvInitJoinTableNode ),
                                aEnv );

    sValidateJoinTable->mType = QLV_NODE_TYPE_JOIN_TABLE;

    STL_TRY( qlvCreateRefExprList( &(sValidateJoinTable->mOuterColumnList),
                                   aPlanBasicParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Join Type 설정 */
    sNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
    if( aCandNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE )
    {
        sPlanHashJoin->mJoinType = QLV_JOIN_TYPE_INNER;
        sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
        sRightNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
    }
    else if( aCandNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE )
    {
        switch( ((qlncOuterJoinTableNode*)(aCandNode))->mJoinDirect )
        {
            case QLNC_JOIN_DIRECT_LEFT:
                sPlanHashJoin->mJoinType = QLV_JOIN_TYPE_LEFT_OUTER;
                break;

            case QLNC_JOIN_DIRECT_FULL:
                sPlanHashJoin->mJoinType = QLV_JOIN_TYPE_FULL_OUTER;
                break;

            /* case QLNC_JOIN_DIRECT_RIGHT: */
            default:
                STL_DASSERT( 0 );
                break;
        }

        sNeedRowBlock = STL_FALSE;
        sLeftNeedRowBlock = STL_FALSE;
        sRightNeedRowBlock = STL_FALSE;
    }
    else if( aCandNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE )
    {
        switch( ((qlncSemiJoinTableNode*)(aCandNode))->mJoinDirect )
        {
            case QLNC_JOIN_DIRECT_LEFT:
                if( sHashJoinCost->mIsInverted == STL_FALSE )
                {
                    sPlanHashJoin->mJoinType = QLV_JOIN_TYPE_LEFT_SEMI;
                    sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                    sRightNeedRowBlock = STL_FALSE;
                }
                else
                {
                    sPlanHashJoin->mJoinType = QLV_JOIN_TYPE_INVERTED_LEFT_SEMI;
                    sLeftNeedRowBlock = STL_FALSE;
                    sRightNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                }
                break;

            /* case QLNC_JOIN_DIRECT_RIGHT: */
            default:
                STL_DASSERT( 0 );
                sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                sRightNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                break;
        }
    }
    else
    {
        STL_DASSERT( aCandNode->mNodeType == QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE );
        switch( ((qlncAntiSemiJoinTableNode*)(aCandNode))->mJoinDirect )
        {
            case QLNC_JOIN_DIRECT_LEFT:
                if( ((qlncAntiSemiJoinTableNode*)(aCandNode))->mIsNullAware == STL_TRUE )
                {
                    sPlanHashJoin->mJoinType = QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA;
                    sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                    sRightNeedRowBlock = STL_FALSE;
                }
                else
                {
                    sPlanHashJoin->mJoinType = QLV_JOIN_TYPE_LEFT_ANTI_SEMI;
                    sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                    sRightNeedRowBlock = STL_FALSE;
                }
                break;

            /* case QLNC_JOIN_DIRECT_RIGHT: */
            default:
                STL_DASSERT( 0 );
                sLeftNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                sRightNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;
                break;
        }
    }

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanHashJoin->mCommon),
                          QLNC_PLAN_TYPE_HASH_JOIN,
                          aCandNode,
                          (qlvInitNode*)sValidateJoinTable,
                          sNeedRowBlock );

    sBackupNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;

    aPlanBasicParamInfo->mNeedRowBlock = sLeftNeedRowBlock;

    /* Left Plan 설정 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ sHashJoinCost->mLeftNode->mBestOptType ](
                 aPlanBasicParamInfo,
                 sHashJoinCost->mLeftNode,
                 &(sPlanHashJoin->mLeftChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    aPlanBasicParamInfo->mNeedRowBlock = sRightNeedRowBlock;

    /* Right Plan 설정 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ sHashJoinCost->mRightNode->mBestOptType ](
                 aPlanBasicParamInfo,
                 sHashJoinCost->mRightNode,
                 &(sPlanHashJoin->mRightChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    aPlanBasicParamInfo->mNeedRowBlock = sBackupNeedRowBlock;

    /* Outer Column List 설정 */
    sPlanHashJoin->mOuterColumnList = sValidateJoinTable->mOuterColumnList;

    /* Out Plan 설정 */
    *aPlanNode = (qlncPlanCommon*)sPlanHashJoin;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Combine Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicJoinCombine( qlncPlanBasicParamInfo  * aPlanBasicParamInfo,
                                            qlncNodeCommon          * aCandNode,
                                            qlncPlanCommon         ** aPlanNode,
                                            qllEnv                  * aEnv )
{
    stlBool                   sBackupNeedRowBlock;
    stlInt32                  i;
    qlncPlanJoinCombine     * sPlanJoinCombine      = NULL;
    qlvInitJoinTableNode    * sValidateJoinTable    = NULL;
    qlncJoinCombineCost     * sJoinCombineCost      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mIsAnalyzedCBO == STL_TRUE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mBestOptType == QLNC_BEST_OPT_TYPE_JOIN_COMBINE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mBestCost != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* Join Combine Cost 설정 */
    sJoinCombineCost = (qlncJoinCombineCost*)(aCandNode->mBestCost);

    /* Join Combine Candidate Plan 생성 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mCandidateMem,
                                sPlanJoinCombine,
                                STL_SIZEOF( qlncPlanJoinCombine ),
                                aEnv );

    /* Join Table Validation Node 생성 및 설정 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mRegionMem,
                                sValidateJoinTable,
                                STL_SIZEOF( qlvInitJoinTableNode ),
                                aEnv );

    sValidateJoinTable->mType = QLV_NODE_TYPE_JOIN_TABLE;

    STL_TRY( qlvCreateRefExprList( &(sValidateJoinTable->mOuterColumnList),
                                   aPlanBasicParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanJoinCombine->mCommon),
                          QLNC_PLAN_TYPE_JOIN_COMBINE,
                          aCandNode,
                          (qlvInitNode*)sValidateJoinTable,
                          aPlanBasicParamInfo->mNeedRowBlock );

    /* Outer Column List 설정 */
    sPlanJoinCombine->mOuterColumnList = sValidateJoinTable->mOuterColumnList;

    /* Child Count 설정 */
    sPlanJoinCombine->mChildCount = sJoinCombineCost->mDnfAndFilterCount;

    /* Child Plan Pointer Array 할당 */
    STL_TRY( knlAllocRegionMem( aPlanBasicParamInfo->mCandidateMem,
                                STL_SIZEOF( qlncPlanCommon* ) * sJoinCombineCost->mDnfAndFilterCount,
                                (void**) &(sPlanJoinCombine->mChildPlanArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Child Node 정보 설정 */
    sBackupNeedRowBlock = aPlanBasicParamInfo->mNeedRowBlock;

    aPlanBasicParamInfo->mNeedRowBlock = STL_TRUE;
    for( i = 0; i < sJoinCombineCost->mDnfAndFilterCount; i++ )
    {
        STL_TRY( qlncMakeCandPlanBasicFuncList[ sJoinCombineCost->mJoinNodePtrArr[i]->mBestOptType ](
                     aPlanBasicParamInfo,
                     sJoinCombineCost->mJoinNodePtrArr[i],
                     &(sPlanJoinCombine->mChildPlanArr[i]),
                     aEnv )
                 == STL_SUCCESS );
    }

    aPlanBasicParamInfo->mNeedRowBlock = sBackupNeedRowBlock;

    /* Out Plan 설정 */
    *aPlanNode = (qlncPlanCommon*)sPlanJoinCombine;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sort Instant Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicSortInstant( qlncPlanBasicParamInfo  * aPlanBasicParamInfo,
                                            qlncNodeCommon          * aCandNode,
                                            qlncPlanCommon         ** aPlanNode,
                                            qllEnv                  * aEnv )
{
    qlncPlanType              sPlanType;
    qlncSortInstantNode     * sNodeSortInstant      = NULL;
    qlncPlanSortInstant     * sPlanSortInstant      = NULL;
    qlvInitInstantNode      * sValidateSortInstant  = NULL;
    qlncNodeArray             sNodeArray;
    qlncRefExprItem         * sRefExprItem          = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( aCandNode,
                                             QLNC_NODE_TYPE_SORT_INSTANT,
                                             QLNC_BEST_OPT_TYPE_SORT_INSTANT,
                                             aEnv );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* Sort Instant Node 설정 */
    sNodeSortInstant = (qlncSortInstantNode*)aCandNode;

    /* Sort Instant Candidate Plan 생성 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mCandidateMem,
                                sPlanSortInstant,
                                STL_SIZEOF( qlncPlanSortInstant ),
                                aEnv );

   
    /* Sort Instant Validation Node 생성 및 설정 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mRegionMem,
                                sValidateSortInstant,
                                STL_SIZEOF( qlvInitInstantNode ),
                                aEnv );

    switch( sNodeSortInstant->mInstantType )
    {
        case QLNC_INSTANT_TYPE_NORMAL:
        case QLNC_INSTANT_TYPE_ORDER:
            sValidateSortInstant->mType = QLV_NODE_TYPE_SORT_INSTANT;
            sPlanType = QLNC_PLAN_TYPE_SORT_INSTANT;
            break;
        case QLNC_INSTANT_TYPE_GROUP:
        case QLNC_INSTANT_TYPE_DISTINCT:
            /* @todo 현재 Sort Instant는 Group이나 Distinct가 올 수 없다. */

            sValidateSortInstant->mType = QLV_NODE_TYPE_GROUP_SORT_INSTANT;
            sPlanType = QLNC_PLAN_TYPE_SORT_GROUP_INSTANT;
            break;
        case QLNC_INSTANT_TYPE_JOIN:
            sValidateSortInstant->mType = QLV_NODE_TYPE_SORT_JOIN_INSTANT;
            sPlanType = QLNC_PLAN_TYPE_SORT_JOIN_INSTANT;
            break;
        default:
            STL_DASSERT( 0 );
            sPlanType = QLNC_PLAN_TYPE_SORT_INSTANT;
            break;
    }

    STL_TRY( qlvCreateRefExprList( &(sValidateSortInstant->mOuterColumnList),
                                   aPlanBasicParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanSortInstant->mCommon),
                          sPlanType,
                          aCandNode,
                          (qlvInitNode*)sValidateSortInstant,
                          aPlanBasicParamInfo->mNeedRowBlock );

    /* Child Plan 설정 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ sNodeSortInstant->mChildNode->mBestOptType ](
                 aPlanBasicParamInfo,
                 sNodeSortInstant->mChildNode,
                 &(sPlanSortInstant->mChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    sValidateSortInstant->mTableNode = sPlanSortInstant->mChildPlanNode->mInitNode;

    /* Node Array 초기화 */
    STL_TRY( qlncInitializeNodeArray( aPlanBasicParamInfo->mCandidateMem,
                                      &sNodeArray,
                                      aEnv )
             == STL_SUCCESS );

    /* Node Array 생성 */
    STL_TRY( qlncMakeNodeArrayOnCurrNode( &sNodeArray,
                                          (qlncNodeCommon*)sNodeSortInstant,
                                          aEnv )
             == STL_SUCCESS );

    /* Need Build 여부 설정 */
    if( qlncIsExistOuterColumnOnCandNode( sNodeSortInstant->mChildNode,
                                          &sNodeArray )
        == STL_TRUE )
    {
        sPlanSortInstant->mNeedRebuild = STL_TRUE;
    }
    else
    {
        /* Key Column과 Record Column에 대해서도 Outer Column 체크해야 함 */
        STL_DASSERT( sNodeSortInstant->mKeyColList != NULL );
        sRefExprItem = sNodeSortInstant->mKeyColList->mHead;
        while( sRefExprItem != NULL )
        {
            sPlanSortInstant->mNeedRebuild =
                qlncIsExistOuterColumnOnExpr( sRefExprItem->mExpr, &sNodeArray );

            if( sPlanSortInstant->mNeedRebuild == STL_TRUE )
            {
                break;
            }

            sRefExprItem = sRefExprItem->mNext;
        }

        if( (sPlanSortInstant->mNeedRebuild == STL_FALSE) &&
            (sNodeSortInstant->mRecColList != NULL) )
        {
            sRefExprItem = sNodeSortInstant->mRecColList->mHead;
            while( sRefExprItem != NULL )
            {
                sPlanSortInstant->mNeedRebuild =
                    qlncIsExistOuterColumnOnExpr( sRefExprItem->mExpr, &sNodeArray );

                if( sPlanSortInstant->mNeedRebuild == STL_TRUE )
                {
                    break;
                }

                sRefExprItem = sRefExprItem->mNext;
            }
        }
    }

    /* Sort Table Attribute 설정 */
    sPlanSortInstant->mSortTableAttr = sNodeSortInstant->mSortTableAttr;

    if( sNodeSortInstant->mFilter != NULL )
    {
        sPlanSortInstant->mSortTableAttr.mLeafOnly = STL_FALSE;
    }

    /* Scrollable 설정 */
    sPlanSortInstant->mScrollable = sNodeSortInstant->mScrollable;

    /* Outer Column List 설정 */
    sPlanSortInstant->mOuterColumnList = sValidateSortInstant->mOuterColumnList;

    /* Out Plan 설정 */
    *aPlanNode = (qlncPlanCommon*)sPlanSortInstant;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Instant Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicHashInstant( qlncPlanBasicParamInfo  * aPlanBasicParamInfo,
                                            qlncNodeCommon          * aCandNode,
                                            qlncPlanCommon         ** aPlanNode,
                                            qllEnv                  * aEnv )
{
    qlncPlanType              sPlanType;
    qlncHashInstantNode     * sNodeHashInstant      = NULL;
    qlncPlanHashInstant     * sPlanHashInstant      = NULL;
    qlvInitInstantNode      * sValidateHashInstant  = NULL;
    qlncNodeArray             sNodeArray;
    qlncRefExprItem         * sRefExprItem          = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( aCandNode,
                                             QLNC_NODE_TYPE_HASH_INSTANT,
                                             QLNC_BEST_OPT_TYPE_HASH_INSTANT,
                                             aEnv );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* Hash Instant Node 설정 */
    sNodeHashInstant = (qlncHashInstantNode*)aCandNode;

    /* Hash Instant Candidate Plan 생성 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mCandidateMem,
                                sPlanHashInstant,
                                STL_SIZEOF( qlncPlanHashInstant ),
                                aEnv );

    /* Hash Instant Validation Node 생성 및 설정 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mRegionMem,
                                sValidateHashInstant,
                                STL_SIZEOF( qlvInitInstantNode ),
                                aEnv );

    switch( sNodeHashInstant->mInstantType )
    {
        case QLNC_INSTANT_TYPE_NORMAL:
        case QLNC_INSTANT_TYPE_ORDER:
            /* @todo 현재 Hash Instant에서는 Normal이나 Order인 경우가 올 수 없다. */

            sValidateHashInstant->mType = QLV_NODE_TYPE_HASH_INSTANT;
            sPlanType = QLNC_PLAN_TYPE_HASH_INSTANT;
            break;
        case QLNC_INSTANT_TYPE_GROUP:
        case QLNC_INSTANT_TYPE_DISTINCT:
            sValidateHashInstant->mType = QLV_NODE_TYPE_GROUP_HASH_INSTANT;
            sPlanType = QLNC_PLAN_TYPE_HASH_GROUP_INSTANT;
            break;
        case QLNC_INSTANT_TYPE_JOIN:
            sValidateHashInstant->mType = QLV_NODE_TYPE_HASH_JOIN_INSTANT;
            sPlanType = QLNC_PLAN_TYPE_HASH_JOIN_INSTANT;
            break;
        default:
            STL_DASSERT( 0 );
            sPlanType = QLNC_PLAN_TYPE_HASH_INSTANT;
            break;
    }

    STL_TRY( qlvCreateRefExprList( &(sValidateHashInstant->mOuterColumnList),
                                   aPlanBasicParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanHashInstant->mCommon),
                          sPlanType,
                          aCandNode,
                          (qlvInitNode*)sValidateHashInstant,
                          aPlanBasicParamInfo->mNeedRowBlock );

    /* Child Plan 설정 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ sNodeHashInstant->mChildNode->mBestOptType ](
                 aPlanBasicParamInfo,
                 sNodeHashInstant->mChildNode,
                 &(sPlanHashInstant->mChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    sValidateHashInstant->mTableNode = sPlanHashInstant->mChildPlanNode->mInitNode;

    /* Node Array 초기화 */
    STL_TRY( qlncInitializeNodeArray( aPlanBasicParamInfo->mCandidateMem,
                                      &sNodeArray,
                                      aEnv )
             == STL_SUCCESS );

    /* Node Array 생성 */
    STL_TRY( qlncMakeNodeArrayOnCurrNode( &sNodeArray,
                                          (qlncNodeCommon*)sNodeHashInstant,
                                          aEnv )
             == STL_SUCCESS );

    /* Need Build 여부 설정 */
    if( qlncIsExistOuterColumnOnCandNode( sNodeHashInstant->mChildNode,
                                          &sNodeArray )
        == STL_TRUE )
    {
        sPlanHashInstant->mNeedRebuild = STL_TRUE;
    }
    else
    {
        /* Key Column과 Record Column에 대해서도 Outer Column 체크해야 함 */
        STL_DASSERT( sNodeHashInstant->mKeyColList != NULL );
        sRefExprItem = sNodeHashInstant->mKeyColList->mHead;
        while( sRefExprItem != NULL )
        {
            sPlanHashInstant->mNeedRebuild =
                qlncIsExistOuterColumnOnExpr( sRefExprItem->mExpr, &sNodeArray );

            if( sPlanHashInstant->mNeedRebuild == STL_TRUE )
            {
                break;
            }

            sRefExprItem = sRefExprItem->mNext;
        }

        if( (sPlanHashInstant->mNeedRebuild == STL_FALSE) &&
            (sNodeHashInstant->mRecColList != NULL) )
        {
            sRefExprItem = sNodeHashInstant->mRecColList->mHead;
            while( sRefExprItem != NULL )
            {
                sPlanHashInstant->mNeedRebuild =
                    qlncIsExistOuterColumnOnExpr( sRefExprItem->mExpr, &sNodeArray );

                if( sPlanHashInstant->mNeedRebuild == STL_TRUE )
                {
                    break;
                }

                sRefExprItem = sRefExprItem->mNext;
            }
        }
    }

    /* Hash Table Attribute 설정 */
    stlMemcpy( &(sPlanHashInstant->mHashTableAttr),
               &(sNodeHashInstant->mHashTableAttr),
               STL_SIZEOF( smlIndexAttr ) );

    /* Scrollable 설정 */
    sPlanHashInstant->mScrollable = sNodeHashInstant->mScrollable;

    /* Outer Column List 설정 */
    sPlanHashInstant->mOuterColumnList = sValidateHashInstant->mOuterColumnList;

    /* Out Plan 설정 */
    *aPlanNode = (qlncPlanCommon*)sPlanHashInstant;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Group Candidate Plan을 생성한다.
 * @param[in]   aPlanBasicParamInfo Candidate Plan Basic Parameter Info
 * @param[in]   aCandNode           Candidate Node
 * @param[out]  aPlanNode           Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanBasicGroup( qlncPlanBasicParamInfo    * aPlanBasicParamInfo,
                                      qlncNodeCommon            * aCandNode,
                                      qlncPlanCommon           ** aPlanNode,
                                      qllEnv                    * aEnv )
{
    qlncPlanType              sPlanType;
    qlncHashInstantNode     * sNodeHashInstant  = NULL;
    qlncPlanGroup           * sPlanGroup        = NULL;
    qlvInitInstantNode      * sValidateGroup    = NULL;
    qlncNodeArray             sNodeArray;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_BASIC_PARAM_VALIDATE( aPlanBasicParamInfo, aEnv );
    QLNC_MAKE_CAND_PLAN_NODE_PARAM_VALIDATE( aCandNode,
                                             QLNC_NODE_TYPE_HASH_INSTANT,
                                             QLNC_BEST_OPT_TYPE_GROUP,
                                             aEnv );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* Hash Instant Node 설정 */
    sNodeHashInstant = (qlncHashInstantNode*)aCandNode;

    /* Group Candidate Plan 생성 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mCandidateMem,
                                sPlanGroup,
                                STL_SIZEOF( qlncPlanGroup ),
                                aEnv );

    /* Group Validation Node 생성 및 설정 */
    QLNC_ALLOC_AND_INIT_MEMORY( aPlanBasicParamInfo->mRegionMem,
                                sValidateGroup,
                                STL_SIZEOF( qlvInitInstantNode ),
                                aEnv );

    STL_DASSERT( (sNodeHashInstant->mInstantType == QLNC_INSTANT_TYPE_GROUP) ||
                 (sNodeHashInstant->mInstantType == QLNC_INSTANT_TYPE_DISTINCT) );
    sValidateGroup->mType = QLV_NODE_TYPE_GROUP_HASH_INSTANT;
    sPlanType = QLNC_PLAN_TYPE_GROUP;

    STL_TRY( qlvCreateRefExprList( &(sValidateGroup->mOuterColumnList),
                                   aPlanBasicParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanGroup->mCommon),
                          sPlanType,
                          aCandNode,
                          (qlvInitNode*)sValidateGroup,
                          aPlanBasicParamInfo->mNeedRowBlock );

    /* Child Plan 설정 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ sNodeHashInstant->mChildNode->mBestOptType ](
                 aPlanBasicParamInfo,
                 sNodeHashInstant->mChildNode,
                 &(sPlanGroup->mChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    sValidateGroup->mTableNode = sPlanGroup->mChildPlanNode->mInitNode;

    /* Node Array 초기화 */
    STL_TRY( qlncInitializeNodeArray( aPlanBasicParamInfo->mCandidateMem,
                                      &sNodeArray,
                                      aEnv )
             == STL_SUCCESS );

    /* Node Array 생성 */
    STL_TRY( qlncMakeNodeArrayOnCurrNode( &sNodeArray,
                                          (qlncNodeCommon*)sNodeHashInstant,
                                          aEnv )
             == STL_SUCCESS );

    /* Outer Column List 설정 */
    sPlanGroup->mOuterColumnList = sValidateGroup->mOuterColumnList;

    /* Out Plan 설정 */
    *aPlanNode = (qlncPlanCommon*)sPlanGroup;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */

