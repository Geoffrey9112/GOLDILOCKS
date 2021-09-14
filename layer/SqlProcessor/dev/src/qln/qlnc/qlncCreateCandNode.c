/*******************************************************************************
 * qlncCreateCandNode.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file qlncCreateCandNode.c
 * @brief SQL Processor Layer SELECT statement candidate optimization
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/*******************************************************
 * Candidate Plan
 *******************************************************/


/**
 * @brief Query Set Node 또는 Query Spec Node의 Candidate Plan을 생성한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCreateQueryCandNode( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                   qllEnv                   * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aCreateNodeParamInfo->mInitNode->mType )
    {
        case QLV_NODE_TYPE_QUERY_SET:
            STL_TRY( qlncCreateQuerySetCandNode( aCreateNodeParamInfo,
                                                 aEnv )
                     == STL_SUCCESS );
            break;
        case QLV_NODE_TYPE_QUERY_SPEC:
            STL_TRY( qlncCreateQuerySpecCandNode( aCreateNodeParamInfo,
                                                  aEnv )
                     == STL_SUCCESS );
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Set Node의 Candidate Plan을 생성한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCreateQuerySetCandNode( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                                      qllEnv                    * aEnv )
{
    stlInt32                  i;
    qlncTableMapArray       * sBackupTableMapArr    = NULL;
    qlvInitQuerySetNode     * sInitQuerySet         = NULL;
    qlncQuerySet            * sCandQuerySet         = NULL;
 

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode->mType == QLV_NODE_TYPE_QUERY_SET,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Query Set Validation 정보 획득
     */

    sInitQuerySet = (qlvInitQuerySetNode*)(aCreateNodeParamInfo->mInitNode);

    /* Table Map Array 백업 */
    sBackupTableMapArr = aCreateNodeParamInfo->mTableMapArr;


    /**
     * Query Set Candidate Plan Node 할당
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncQuerySet ),
                                (void**) &sCandQuerySet,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sCandQuerySet->mNodeCommon),
                           *(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID),
                           QLNC_NODE_TYPE_QUERY_SET );
    (*(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID))++;  /* Global ID 증가 */

    /* Root Query Set 설정 */
    sCandQuerySet->mIsRootSet = sInitQuerySet->mIsRootSet;

    /* Table Map Array 생성 */
    if( sBackupTableMapArr == NULL )
    {
        STL_TRY( qlncCreateTableMapArray( &(aCreateNodeParamInfo->mParamInfo),
                                          &(sCandQuerySet->mTableMapArr),
                                          aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sCandQuerySet->mTableMapArr = sBackupTableMapArr;
    }
    STL_DASSERT( sCandQuerySet->mTableMapArr != NULL );

    /* Query Block ID 설정 */
    sCandQuerySet->mQBID = sInitQuerySet->mQBID;

    /* Query Block Map Array에 현재 노드 삽입 */
    STL_TRY( qlncAddQBMapToQBMapArray( aCreateNodeParamInfo->mQBMapArr,
                                       sCandQuerySet->mQBID,
                                       (qlvInitNode*)sInitQuerySet,
                                       (qlncNodeCommon*)sCandQuerySet,
                                       aEnv )
             == STL_SUCCESS );

    /* Set OP Candidate Node 처리 */
    aCreateNodeParamInfo->mInitNode = (qlvInitNode*)sInitQuerySet;
    aCreateNodeParamInfo->mTableMapArr = sCandQuerySet->mTableMapArr;
    STL_TRY( qlncCreateDiffSetOPCandNode( aCreateNodeParamInfo,
                                          aEnv )
             == STL_SUCCESS );
    sCandQuerySet->mChildNode = aCreateNodeParamInfo->mCandNode;


    /**
     * Order By 절
     */

    if( sInitQuerySet->mOrderBy != NULL )
    {
        STL_TRY( qlncCreateOrderByCandNode( aCreateNodeParamInfo,
                                            sInitQuerySet->mOrderBy,
                                            sCandQuerySet->mChildNode,
                                            aEnv )
                 == STL_SUCCESS );
        sCandQuerySet->mChildNode = aCreateNodeParamInfo->mCandNode;
        aCreateNodeParamInfo->mCandNode = NULL;
    }


    /**
     * Target절
     */

    sCandQuerySet->mSetTargetCount = sInitQuerySet->mSetTargetCount;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncTarget ) * sCandQuerySet->mSetTargetCount,
                                (void**) &(sCandQuerySet->mSetTargetArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncTarget ) * sCandQuerySet->mSetTargetCount,
                                (void**) &(sCandQuerySet->mSetColumnArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < sCandQuerySet->mSetTargetCount; i++ )
    {
        sCandQuerySet->mSetTargetArr[i].mDisplayPos = sInitQuerySet->mSetTargets[i].mDisplayPos;

        QLNC_ALLOC_AND_COPY_STRING( sCandQuerySet->mSetTargetArr[i].mDisplayName,
                                    sInitQuerySet->mSetTargets[i].mDisplayName,
                                    &QLL_CANDIDATE_MEM( aEnv ),
                                    aEnv );

        QLNC_ALLOC_AND_COPY_STRING( sCandQuerySet->mSetTargetArr[i].mAliasName,
                                    sInitQuerySet->mSetTargets[i].mAliasName,
                                    &QLL_CANDIDATE_MEM( aEnv ),
                                    aEnv );

        STL_TRY( qlncConvertExpression( aCreateNodeParamInfo,
                                        sInitQuerySet->mSetTargets[i].mExpr,
                                        QLNC_EXPR_PHRASE_TARGET,
                                        NULL,
                                        &sCandQuerySet->mSetTargetArr[i].mExpr,
                                        aEnv )
                 == STL_SUCCESS );

        sCandQuerySet->mSetTargetArr[i].mDataTypeInfo = sInitQuerySet->mSetTargets[i].mDataTypeInfo;

        sCandQuerySet->mSetColumnArr[i].mDisplayPos = sInitQuerySet->mSetColumns[i].mDisplayPos;

        QLNC_ALLOC_AND_COPY_STRING( sCandQuerySet->mSetColumnArr[i].mDisplayName,
                                    sInitQuerySet->mSetColumns[i].mDisplayName,
                                    &QLL_CANDIDATE_MEM( aEnv ),
                                    aEnv );

        QLNC_ALLOC_AND_COPY_STRING( sCandQuerySet->mSetColumnArr[i].mAliasName,
                                    sInitQuerySet->mSetColumns[i].mAliasName,
                                    &QLL_CANDIDATE_MEM( aEnv ),
                                    aEnv );

        STL_TRY( qlncConvertExpression( aCreateNodeParamInfo,
                                        sInitQuerySet->mSetColumns[i].mExpr,
                                        QLNC_EXPR_PHRASE_TARGET,
                                        NULL,
                                        &sCandQuerySet->mSetColumnArr[i].mExpr,
                                        aEnv )
                 == STL_SUCCESS );

        sCandQuerySet->mSetColumnArr[i].mDataTypeInfo = sInitQuerySet->mSetColumns[i].mDataTypeInfo;
    }


    /**
     * Offset 절
     */

    if( sInitQuerySet->mResultSkip == NULL )
    {
        sCandQuerySet->mOffset = NULL;
    }
    else
    {
        STL_TRY( qlncConvertExpression( aCreateNodeParamInfo,
                                        sInitQuerySet->mResultSkip,
                                        QLNC_EXPR_PHRASE_OFFSET_LIMIT,
                                        NULL,
                                        &sCandQuerySet->mOffset,
                                        aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Limit 절
     */

    if( sInitQuerySet->mResultLimit == NULL )
    {
        sCandQuerySet->mLimit = NULL;
    }
    else
    {
        STL_TRY( qlncConvertExpression( aCreateNodeParamInfo,
                                        sInitQuerySet->mResultLimit,
                                        QLNC_EXPR_PHRASE_OFFSET_LIMIT,
                                        NULL,
                                        &sCandQuerySet->mLimit,
                                        aEnv )
                 == STL_SUCCESS );
    }

    /* Table Map Array 복구 */
    aCreateNodeParamInfo->mTableMapArr = sBackupTableMapArr;


    /**
     * Output 설정
     */

    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandQuerySet;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Diff Set Operation Node의 Candidate Plan을 생성한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCreateDiffSetOPCandNode( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                       qllEnv                   * aEnv )
{
    qlvInitQuerySetNode     * sInitQuerySet         = NULL;
    qlncSetOP               * sCandSetOP            = NULL;
    qlvInitQuerySetNode     * sInitChildQuerySet    = NULL;
    stlBool                   sIsSameSet;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode->mType == QLV_NODE_TYPE_QUERY_SET,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Query Set Validation 정보 획득
     */

    sInitQuerySet = (qlvInitQuerySetNode*)(aCreateNodeParamInfo->mInitNode);


    /**
     * Set Operation Candidate Plan Node 할당
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncSetOP ),
                                (void**) &sCandSetOP,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sCandSetOP->mNodeCommon),
                           *(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID),
                           QLNC_NODE_TYPE_SET_OP );
    (*(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID))++;  /* Global ID 증가 */


    /* Set Type 설정 */
    QLNC_CONVERT_INIT_SET_TYPE_TO_CAND_SET_TYPE( sInitQuerySet->mSetType, sCandSetOP->mSetType );

    /* Set Option 설정 */
    QLNC_CONVERT_INIT_SET_OPTION_TO_CAND_SET_OPTION( sInitQuerySet->mSetOption, sCandSetOP->mSetOption );

    /* Child Count 설정 */
    sCandSetOP->mChildCount = sInitQuerySet->mSameSetChildCnt;

    /* Current Child Index 설정 */
    sCandSetOP->mCurChildIdx = 0;

    /* Child Candidate Node Array 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncNodeCommon* ) * sCandSetOP->mChildCount,
                                (void**) &(sCandSetOP->mChildNodeArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Left Child Node에 대한 처리 */
    if( sInitQuerySet->mLeftQueryNode->mType == QLV_NODE_TYPE_QUERY_SPEC )
    {
        aCreateNodeParamInfo->mInitNode = sInitQuerySet->mLeftQueryNode;
        STL_TRY( qlncCreateQuerySpecCandNode( aCreateNodeParamInfo,
                                              aEnv )
                 == STL_SUCCESS );
        sCandSetOP->mChildNodeArr[ sCandSetOP->mCurChildIdx ] = aCreateNodeParamInfo->mCandNode;
        sCandSetOP->mCurChildIdx++;
    }
    else
    {
        sInitChildQuerySet = (qlvInitQuerySetNode*)(sInitQuerySet->mLeftQueryNode);
        if( sInitChildQuerySet->mIsPureSet == STL_FALSE )
        {
            /* Pure Query Set이 아닌 경우 */

            aCreateNodeParamInfo->mInitNode = (qlvInitNode*)sInitChildQuerySet;
            STL_TRY( qlncCreateQuerySetCandNode( aCreateNodeParamInfo,
                                                 aEnv )
                     == STL_SUCCESS );
            sCandSetOP->mChildNodeArr[ sCandSetOP->mCurChildIdx ] = aCreateNodeParamInfo->mCandNode;
            sCandSetOP->mCurChildIdx++;
        }
        else
        {
            qlncSetType       sChildSetType;
            qlncSetOption     sChildSetOption;

            /* Pure Query Set인 경우 */

            /* 동일한 Query Set인지 판단 */
            QLNC_CONVERT_INIT_SET_TYPE_TO_CAND_SET_TYPE( sInitChildQuerySet->mSetType, sChildSetType );
            QLNC_CONVERT_INIT_SET_OPTION_TO_CAND_SET_OPTION( sInitChildQuerySet->mSetOption, sChildSetOption );
            if( sCandSetOP->mSetType == sChildSetType )
            {
                if( sCandSetOP->mSetType == QLNC_SET_TYPE_EXCEPT )
                {
                    if( sCandSetOP->mSetOption == sChildSetOption )
                    {
                        sIsSameSet = STL_TRUE;
                    }
                    else
                    {
                        sIsSameSet = STL_FALSE;
                    }
                }
                else
                {
                    if( sCandSetOP->mSetOption == QLNC_SET_OPTION_DISTINCT )
                    {
                        sIsSameSet = STL_TRUE;
                    }
                    else
                    {
                        if( sChildSetOption == QLNC_SET_OPTION_ALL )
                        {
                            sIsSameSet = STL_TRUE;
                        }
                        else
                        {
                            sIsSameSet = STL_FALSE;
                        }
                    }
                }
            }
            else
            {
                sIsSameSet = STL_FALSE;
            }

            /* Child Candidate Node 처리 */
            if( sIsSameSet == STL_TRUE )
            {
                aCreateNodeParamInfo->mInitNode = (qlvInitNode*)sInitChildQuerySet;
                STL_TRY( qlncCreateSameSetOPCandNode( aCreateNodeParamInfo,
                                                      sCandSetOP,
                                                      sChildSetOption,
                                                      aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                aCreateNodeParamInfo->mInitNode = (qlvInitNode*)sInitChildQuerySet;
                STL_TRY( qlncCreateDiffSetOPCandNode( aCreateNodeParamInfo,
                                                      aEnv )
                         == STL_SUCCESS );
                sCandSetOP->mChildNodeArr[ sCandSetOP->mCurChildIdx ] = aCreateNodeParamInfo->mCandNode;
                sCandSetOP->mCurChildIdx++;
            }
        }
    }

    /* Right Child Node에 대한 처리 */
    if( sInitQuerySet->mRightQueryNode->mType == QLV_NODE_TYPE_QUERY_SPEC )
    {
        aCreateNodeParamInfo->mInitNode = sInitQuerySet->mRightQueryNode;
        STL_TRY( qlncCreateQuerySpecCandNode( aCreateNodeParamInfo,
                                              aEnv )
                 == STL_SUCCESS );
        sCandSetOP->mChildNodeArr[ sCandSetOP->mCurChildIdx ] = aCreateNodeParamInfo->mCandNode;
        sCandSetOP->mCurChildIdx++;
    }
    else
    {
        sInitChildQuerySet = (qlvInitQuerySetNode*)(sInitQuerySet->mRightQueryNode);
        if( sInitChildQuerySet->mIsPureSet == STL_FALSE )
        {
            /* Pure Query Set이 아닌 경우 */

            aCreateNodeParamInfo->mInitNode = (qlvInitNode*)sInitChildQuerySet;
            STL_TRY( qlncCreateQuerySetCandNode( aCreateNodeParamInfo,
                                                 aEnv )
                     == STL_SUCCESS );
            sCandSetOP->mChildNodeArr[ sCandSetOP->mCurChildIdx ] = aCreateNodeParamInfo->mCandNode;
            sCandSetOP->mCurChildIdx++;
        }
        else
        {
            qlncSetType       sChildSetType;
            qlncSetOption     sChildSetOption;

            /* Pure Query Set인 경우 */

            /* 동일한 Query Set인지 판단 */
            QLNC_CONVERT_INIT_SET_TYPE_TO_CAND_SET_TYPE( sInitChildQuerySet->mSetType, sChildSetType );
            QLNC_CONVERT_INIT_SET_OPTION_TO_CAND_SET_OPTION( sInitChildQuerySet->mSetOption, sChildSetOption );
            if( sCandSetOP->mSetType == sChildSetType )
            {
                if( sCandSetOP->mSetType == QLNC_SET_TYPE_EXCEPT )
                {
                    sIsSameSet = STL_FALSE;
                }
                else
                {
                    if( sCandSetOP->mSetOption == QLNC_SET_OPTION_DISTINCT )
                    {
                        sIsSameSet = STL_TRUE;
                    }
                    else
                    {
                        if( sChildSetOption == QLNC_SET_OPTION_ALL )
                        {
                            sIsSameSet = STL_TRUE;
                        }
                        else
                        {
                            sIsSameSet = STL_FALSE;
                        }
                    }
                }
            }
            else
            {
                sIsSameSet = STL_FALSE;
            }

            /* Child Candidate Node 처리 */
            if( sIsSameSet == STL_TRUE )
            {
                aCreateNodeParamInfo->mInitNode = (qlvInitNode*)sInitChildQuerySet;
                STL_TRY( qlncCreateSameSetOPCandNode( aCreateNodeParamInfo,
                                                      sCandSetOP,
                                                      sChildSetOption,
                                                      aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                aCreateNodeParamInfo->mInitNode = (qlvInitNode*)sInitChildQuerySet;
                STL_TRY( qlncCreateDiffSetOPCandNode( aCreateNodeParamInfo,
                                                      aEnv )
                         == STL_SUCCESS );
                sCandSetOP->mChildNodeArr[ sCandSetOP->mCurChildIdx ] = aCreateNodeParamInfo->mCandNode;
                sCandSetOP->mCurChildIdx++;
            }
        }
    }


    /**
     * Output 설정
     */

    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandSetOP;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Same Set Operation Node의 Candidate Plan을 생성한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aRootSetOP              Root Set Operator
 * @param[in]   aParentSetOption        Parent Set Operator의 Option
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCreateSameSetOPCandNode( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                       qlncSetOP                * aRootSetOP,
                                       qlncSetOption              aParentSetOption,
                                       qllEnv                   * aEnv )
{
    qlvInitQuerySetNode     * sInitQuerySet         = NULL;
    qlvInitQuerySetNode     * sInitChildQuerySet    = NULL;
    stlBool                   sIsSameSet;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode->mType == QLV_NODE_TYPE_QUERY_SET,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Query Set Validation 정보 획득
     */

    sInitQuerySet = (qlvInitQuerySetNode*)(aCreateNodeParamInfo->mInitNode);

    /* Left Child Node에 대한 처리 */
    if( sInitQuerySet->mLeftQueryNode->mType == QLV_NODE_TYPE_QUERY_SPEC )
    {
        aCreateNodeParamInfo->mInitNode = sInitQuerySet->mLeftQueryNode;
        STL_TRY( qlncCreateQuerySpecCandNode( aCreateNodeParamInfo,
                                              aEnv )
                 == STL_SUCCESS );
        aRootSetOP->mChildNodeArr[ aRootSetOP->mCurChildIdx ] = aCreateNodeParamInfo->mCandNode;
        aRootSetOP->mCurChildIdx++;
    }
    else
    {
        sInitChildQuerySet = (qlvInitQuerySetNode*)(sInitQuerySet->mLeftQueryNode);
        if( sInitChildQuerySet->mIsPureSet == STL_FALSE )
        {
            /* Pure Query Set이 아닌 경우 */
            aCreateNodeParamInfo->mInitNode = (qlvInitNode*)sInitChildQuerySet;
            STL_TRY( qlncCreateQuerySetCandNode( aCreateNodeParamInfo,
                                                 aEnv )
                     == STL_SUCCESS );
            aRootSetOP->mChildNodeArr[ aRootSetOP->mCurChildIdx ] = aCreateNodeParamInfo->mCandNode;
            aRootSetOP->mCurChildIdx++;
        }
        else
        {
            qlncSetType       sChildSetType;
            qlncSetOption     sChildSetOption;

            /* Pure Query Set인 경우 */

            /* 동일한 Query Set인지 판단 */
            QLNC_CONVERT_INIT_SET_TYPE_TO_CAND_SET_TYPE( sInitChildQuerySet->mSetType, sChildSetType );
            QLNC_CONVERT_INIT_SET_OPTION_TO_CAND_SET_OPTION( sInitChildQuerySet->mSetOption, sChildSetOption );
            if( aRootSetOP->mSetType == sChildSetType )
            {
                if( aRootSetOP->mSetType == QLNC_SET_TYPE_EXCEPT )
                {
                    if( aParentSetOption == sChildSetOption )
                    {
                        sIsSameSet = STL_TRUE;
                    }
                    else
                    {
                        sIsSameSet = STL_FALSE;
                    }
                }
                else
                {
                    if( aParentSetOption == QLNC_SET_OPTION_DISTINCT )
                    {
                        sIsSameSet = STL_TRUE;
                    }
                    else
                    {
                        if( sChildSetOption == QLNC_SET_OPTION_ALL )
                        {
                            sIsSameSet = STL_TRUE;
                        }
                        else
                        {
                            sIsSameSet = STL_FALSE;
                        }
                    }
                }
            }
            else
            {
                sIsSameSet = STL_FALSE;
            }

            /* Child Candidate Node 처리 */
            if( sIsSameSet == STL_TRUE )
            {
                aCreateNodeParamInfo->mInitNode = (qlvInitNode*)sInitChildQuerySet;
                STL_TRY( qlncCreateSameSetOPCandNode( aCreateNodeParamInfo,
                                                      aRootSetOP,
                                                      sChildSetOption,
                                                      aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                aCreateNodeParamInfo->mInitNode = (qlvInitNode*)sInitChildQuerySet;
                STL_TRY( qlncCreateDiffSetOPCandNode( aCreateNodeParamInfo,
                                                      aEnv )
                         == STL_SUCCESS );
                aRootSetOP->mChildNodeArr[ aRootSetOP->mCurChildIdx ] = aCreateNodeParamInfo->mCandNode;
                aRootSetOP->mCurChildIdx++;
            }
        }
    }

    /* Right Child Node에 대한 처리 */
    if( sInitQuerySet->mRightQueryNode->mType == QLV_NODE_TYPE_QUERY_SPEC )
    {
        aCreateNodeParamInfo->mInitNode = sInitQuerySet->mRightQueryNode;
        STL_TRY( qlncCreateQuerySpecCandNode( aCreateNodeParamInfo,
                                              aEnv )
                 == STL_SUCCESS );
        aRootSetOP->mChildNodeArr[ aRootSetOP->mCurChildIdx ] = aCreateNodeParamInfo->mCandNode;
        aRootSetOP->mCurChildIdx++;
    }
    else
    {
        sInitChildQuerySet = (qlvInitQuerySetNode*)(sInitQuerySet->mRightQueryNode);
        if( sInitChildQuerySet->mIsPureSet == STL_FALSE )
        {
            /* Pure Query Set이 아닌 경우 */
            aCreateNodeParamInfo->mInitNode = (qlvInitNode*)sInitChildQuerySet;
            STL_TRY( qlncCreateQuerySetCandNode( aCreateNodeParamInfo,
                                                 aEnv )
                     == STL_SUCCESS );
            aRootSetOP->mChildNodeArr[ aRootSetOP->mCurChildIdx ] = aCreateNodeParamInfo->mCandNode;
            aRootSetOP->mCurChildIdx++;
        }
        else
        {
            qlncSetType       sChildSetType;
            qlncSetOption     sChildSetOption;

            /* Pure Query Set인 경우 */

            /* 동일한 Query Set인지 판단 */
            QLNC_CONVERT_INIT_SET_TYPE_TO_CAND_SET_TYPE( sInitChildQuerySet->mSetType, sChildSetType );
            QLNC_CONVERT_INIT_SET_OPTION_TO_CAND_SET_OPTION( sInitChildQuerySet->mSetOption, sChildSetOption );
            if( aRootSetOP->mSetType == sChildSetType )
            {
                if( aRootSetOP->mSetType == QLNC_SET_TYPE_EXCEPT )
                {
                    sIsSameSet = STL_FALSE;
                }
                else
                {
                    if( aParentSetOption == QLNC_SET_OPTION_DISTINCT )
                    {
                        sIsSameSet = STL_TRUE;
                    }
                    else
                    {
                        if( sChildSetOption == QLNC_SET_OPTION_ALL )
                        {
                            sIsSameSet = STL_TRUE;
                        }
                        else
                        {
                            sIsSameSet = STL_FALSE;
                        }
                    }
                }
            }
            else
            {
                sIsSameSet = STL_FALSE;
            }

            /* Child Candidate Node 처리 */
            if( sIsSameSet == STL_TRUE )
            {
                aCreateNodeParamInfo->mInitNode = (qlvInitNode*)sInitChildQuerySet;
                STL_TRY( qlncCreateSameSetOPCandNode( aCreateNodeParamInfo,
                                                      aRootSetOP,
                                                      sChildSetOption,
                                                      aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                aCreateNodeParamInfo->mInitNode = (qlvInitNode*)sInitChildQuerySet;
                STL_TRY( qlncCreateDiffSetOPCandNode( aCreateNodeParamInfo,
                                                      aEnv )
                         == STL_SUCCESS );
                aRootSetOP->mChildNodeArr[ aRootSetOP->mCurChildIdx ] = aCreateNodeParamInfo->mCandNode;
                aRootSetOP->mCurChildIdx++;
            }
        }
    }


    /**
     * Output 설정
     */

    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)aRootSetOP;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Spec Node의 Candidate Plan을 생성한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCreateQuerySpecCandNode( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                       qllEnv                   * aEnv )
{
    stlInt32                  i;
    qlvInitQuerySpecNode    * sInitQuerySpec    = NULL;
    qlncQuerySpec           * sCandQuerySpec    = NULL;

    qlvHintInfo             * sBackupHintInfo       = NULL;
    qlncTableMapArray       * sBackupTableMapArr    = NULL;
    qlncRefExprList           sSubQueryExprList;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode->mType == QLV_NODE_TYPE_QUERY_SPEC,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Query Spec Validation 정보 획득
     */

    sInitQuerySpec = (qlvInitQuerySpecNode*)(aCreateNodeParamInfo->mInitNode);

    /* Hint Info 및 Table Map Array 백업 */
    sBackupHintInfo = aCreateNodeParamInfo->mHintInfo;
    sBackupTableMapArr = aCreateNodeParamInfo->mTableMapArr;


    /**
     * Query Spec Candidate Plan Node 할당
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncQuerySpec ),
                                (void**) &sCandQuerySpec,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sCandQuerySpec->mNodeCommon),
                           *(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID),
                           QLNC_NODE_TYPE_QUERY_SPEC );
    (*(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID))++;  /* Global ID 증가 */
    sCandQuerySpec->mNonFilterSubQueryExprList = NULL;

    /* Table Map Array 생성 */
    if( sBackupTableMapArr == NULL )
    {
        STL_TRY( qlncCreateTableMapArray( &(aCreateNodeParamInfo->mParamInfo),
                                          &(sCandQuerySpec->mTableMapArr),
                                          aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sCandQuerySpec->mTableMapArr = sBackupTableMapArr;
    }
    STL_DASSERT( sCandQuerySpec->mTableMapArr != NULL );

    /* Query Block ID 설정 */
    sCandQuerySpec->mQBID = sInitQuerySpec->mQBID;

    /* Query Block Map Array에 현재 노드 삽입 */
    STL_TRY( qlncAddQBMapToQBMapArray( aCreateNodeParamInfo->mQBMapArr,
                                       sCandQuerySpec->mQBID,
                                       (qlvInitNode*)sInitQuerySpec,
                                       (qlncNodeCommon*)sCandQuerySpec,
                                       aEnv )
             == STL_SUCCESS );

    /* Query Transformation Hint 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncQueryTransformHint ),
                                (void**) &(sCandQuerySpec->mQueryTransformHint),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_QUERY_TRANSFORM_HINT( sCandQuerySpec->mQueryTransformHint );

    /* Other Hint 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOtherHint ),
                                (void**) &(sCandQuerySpec->mOtherHint),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_OTHER_HINT( sCandQuerySpec->mOtherHint );


    /**
     * From절에 대한 Candidate Node 생성
     */

    aCreateNodeParamInfo->mInitNode = sInitQuerySpec->mTableNode;
    aCreateNodeParamInfo->mCandNode = NULL;
    aCreateNodeParamInfo->mHintInfo = sInitQuerySpec->mHintInfo;
    aCreateNodeParamInfo->mTableMapArr = sCandQuerySpec->mTableMapArr;
    STL_TRY( qlncCreateTableCandNode( aCreateNodeParamInfo,
                                      aEnv )
             == STL_SUCCESS );

    sCandQuerySpec->mChildNode = aCreateNodeParamInfo->mCandNode;
    aCreateNodeParamInfo->mCandNode = NULL;


    /**
     * Where절
     */

    if( sInitQuerySpec->mWhereExpr != NULL )
    {
        qlncAndFilter   * sWhere    = NULL;

        STL_TRY( qlncConvertCondition( aCreateNodeParamInfo,
                                       sInitQuerySpec->mWhereExpr,
                                       QLNC_EXPR_PHRASE_WHERE,
                                       &sWhere,
                                       aEnv )
                 == STL_SUCCESS );

        /* Where절을 Table Node로 Push */
        if( sCandQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
        {
            STL_DASSERT( ((qlncBaseTableNode*)(sCandQuerySpec->mChildNode))->mFilter == NULL );
            ((qlncBaseTableNode*)(sCandQuerySpec->mChildNode))->mFilter = sWhere;
        }
        else if( sCandQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
        {
            STL_DASSERT( ((qlncSubTableNode*)(sCandQuerySpec->mChildNode))->mFilter == NULL );
            ((qlncSubTableNode*)(sCandQuerySpec->mChildNode))->mFilter = sWhere;
        }
        else if( sCandQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE )
        {
            STL_DASSERT( ((qlncJoinTableNode*)(sCandQuerySpec->mChildNode))->mFilter == NULL );

            /* 하위 노드가 Join Node이면 Join Condition에 합친다. */
            if( ((qlncJoinTableNode*)(sCandQuerySpec->mChildNode))->mJoinType == QLNC_JOIN_TYPE_INNER )
            {
                if( ((qlncJoinTableNode*)(sCandQuerySpec->mChildNode))->mJoinCond == NULL )
                {
                    ((qlncJoinTableNode*)(sCandQuerySpec->mChildNode))->mJoinCond = sWhere;
                }
                else
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail(
                                 &(aCreateNodeParamInfo->mParamInfo),
                                 ((qlncJoinTableNode*)(sCandQuerySpec->mChildNode))->mJoinCond,
                                 sWhere->mOrCount,
                                 sWhere->mOrFilters,
                                 aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                ((qlncJoinTableNode*)(sCandQuerySpec->mChildNode))->mFilter = sWhere;
            }
        }
    }


    /**
     * Group By & Having 절
     */

    if( sInitQuerySpec->mGroupBy != NULL )
    {
        STL_TRY( qlncCreateGroupByCandNode( aCreateNodeParamInfo,
                                            sInitQuerySpec->mGroupBy,
                                            sCandQuerySpec->mChildNode,
                                            aEnv )
                 == STL_SUCCESS );

        sCandQuerySpec->mChildNode = aCreateNodeParamInfo->mCandNode;
        aCreateNodeParamInfo->mCandNode = NULL;
    }


    /**
     * Distinct절
     */

    if( (sInitQuerySpec->mIsDistinct == STL_TRUE) &&
        (sInitQuerySpec->mDistinct != NULL) )
    {
        STL_TRY( qlncCreateDistinctCandNode( aCreateNodeParamInfo,
                                             sInitQuerySpec->mDistinct,
                                             sCandQuerySpec->mChildNode,
                                             aEnv )
                 == STL_SUCCESS );
        sCandQuerySpec->mChildNode = aCreateNodeParamInfo->mCandNode;
        aCreateNodeParamInfo->mCandNode = NULL;
    }


    /**
     * Order By절
     */

    if( sInitQuerySpec->mOrderBy != NULL )
    {
        STL_TRY( qlncCreateOrderByCandNode( aCreateNodeParamInfo,
                                            sInitQuerySpec->mOrderBy,
                                            sCandQuerySpec->mChildNode,
                                            aEnv )
                 == STL_SUCCESS );
        sCandQuerySpec->mChildNode = aCreateNodeParamInfo->mCandNode;
        aCreateNodeParamInfo->mCandNode = NULL;
    }


    /**
     * Target절
     */

    sCandQuerySpec->mTargetCount = sInitQuerySpec->mTargetCount;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncTarget ) * sCandQuerySpec->mTargetCount,
                                (void**) &(sCandQuerySpec->mTargetArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < sCandQuerySpec->mTargetCount; i++ )
    {
        sCandQuerySpec->mTargetArr[i].mDisplayPos = sInitQuerySpec->mTargets[i].mDisplayPos;

        QLNC_ALLOC_AND_COPY_STRING( sCandQuerySpec->mTargetArr[i].mDisplayName,
                                    sInitQuerySpec->mTargets[i].mDisplayName,
                                    &QLL_CANDIDATE_MEM( aEnv ),
                                    aEnv );

        QLNC_ALLOC_AND_COPY_STRING( sCandQuerySpec->mTargetArr[i].mAliasName,
                                    sInitQuerySpec->mTargets[i].mAliasName,
                                    &QLL_CANDIDATE_MEM( aEnv ),
                                    aEnv );

        QLNC_INIT_LIST( &sSubQueryExprList );

        STL_TRY( qlncConvertExpression( aCreateNodeParamInfo,
                                        sInitQuerySpec->mTargets[i].mExpr,
                                        QLNC_EXPR_PHRASE_TARGET,
                                        &sSubQueryExprList,
                                        &sCandQuerySpec->mTargetArr[i].mExpr,
                                        aEnv )
                 == STL_SUCCESS );

        sCandQuerySpec->mTargetArr[i].mDataTypeInfo = sInitQuerySpec->mTargets[i].mDataTypeInfo;

        if( sSubQueryExprList.mCount > 0 )
        {
            if( sCandQuerySpec->mNonFilterSubQueryExprList == NULL )
            {
                STL_TRY( knlAllocRegionMem(
                             &QLL_CANDIDATE_MEM( aEnv ),
                             STL_SIZEOF( qlncRefExprList ),
                             (void**) &(sCandQuerySpec->mNonFilterSubQueryExprList),
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                QLNC_COPY_LIST_INFO( &sSubQueryExprList, sCandQuerySpec->mNonFilterSubQueryExprList );
            }
            else
            {
                sCandQuerySpec->mNonFilterSubQueryExprList->mTail->mNext = sSubQueryExprList.mHead;
                sCandQuerySpec->mNonFilterSubQueryExprList->mTail = sSubQueryExprList.mTail;
                sCandQuerySpec->mNonFilterSubQueryExprList->mCount += sSubQueryExprList.mCount;
            }
        }
    }


    /**
     * Distinct 여부
     */

    sCandQuerySpec->mIsDistinct = sInitQuerySpec->mIsDistinct;


    /**
     * Offset 절
     */

    if( sInitQuerySpec->mResultSkip == NULL )
    {
        sCandQuerySpec->mOffset = NULL;
    }
    else
    {
        STL_TRY( qlncConvertExpression( aCreateNodeParamInfo,
                                        sInitQuerySpec->mResultSkip,
                                        QLNC_EXPR_PHRASE_OFFSET_LIMIT,
                                        NULL,
                                        &sCandQuerySpec->mOffset,
                                        aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Limit 절
     */

    if( sInitQuerySpec->mResultLimit == NULL )
    {
        sCandQuerySpec->mLimit = NULL;
    }
    else
    {
        STL_TRY( qlncConvertExpression( aCreateNodeParamInfo,
                                        sInitQuerySpec->mResultLimit,
                                        QLNC_EXPR_PHRASE_OFFSET_LIMIT,
                                        NULL,
                                        &sCandQuerySpec->mLimit,
                                        aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Query Transformation Hint 분석
     */

    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandQuerySpec;
    STL_TRY( qlncAnalyzeQueryTransformHint( aCreateNodeParamInfo,
                                            aEnv )
             == STL_SUCCESS );


    /**
     * Other Hint 분석
     */

    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandQuerySpec;
    STL_TRY( qlncAnalyzeOtherHint( aCreateNodeParamInfo,
                                   aEnv )
             == STL_SUCCESS );


    /**
     * Hint Info 및 Table Map Array 복구
     */

    aCreateNodeParamInfo->mHintInfo = sBackupHintInfo;
    aCreateNodeParamInfo->mTableMapArr = sBackupTableMapArr;


    /**
     * Output 설정
     */

    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandQuerySpec;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Table Node의 Candidate Plan을 생성한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCreateTableCandNode( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                   qllEnv                   * aEnv )
{
    qlvInitNode * sInitJoinNode = NULL;

    
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aCreateNodeParamInfo->mInitNode->mType == QLV_NODE_TYPE_BASE_TABLE) ||
                        (aCreateNodeParamInfo->mInitNode->mType == QLV_NODE_TYPE_SUB_TABLE) ||
                        (aCreateNodeParamInfo->mInitNode->mType == QLV_NODE_TYPE_JOIN_TABLE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mTableMapArr != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aCreateNodeParamInfo->mInitNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
            STL_TRY( qlncCreateBaseCandNode( aCreateNodeParamInfo,
                                             aEnv )
                     == STL_SUCCESS );

            /* Access Path Hint 분석 */
            STL_TRY( qlncAnalyzeAccessPathHint( aCreateNodeParamInfo,
                                                aEnv )
                     == STL_SUCCESS );

            break;

        case QLV_NODE_TYPE_SUB_TABLE:
            STL_TRY( qlncCreateSubCandNode( aCreateNodeParamInfo,
                                            aEnv )
                     == STL_SUCCESS );

            break;

        case QLV_NODE_TYPE_JOIN_TABLE:

            sInitJoinNode = aCreateNodeParamInfo->mInitNode;

            switch( ((qlvInitJoinTableNode*)(aCreateNodeParamInfo->mInitNode))->mJoinType )
            {
                case QLV_JOIN_TYPE_NONE:
                case QLV_JOIN_TYPE_CROSS:
                case QLV_JOIN_TYPE_INNER:
                    STL_TRY( qlncCreateInnerJoinCandNode( aCreateNodeParamInfo,
                                                          aEnv )
                             == STL_SUCCESS );

                    aCreateNodeParamInfo->mInitNode = sInitJoinNode;

                    /* Join Order Hint 분석 */
                    STL_TRY( qlncAnalyzeJoinOrderHint( aCreateNodeParamInfo,
                                                       aEnv )
                             == STL_SUCCESS );

                    break;
                case QLV_JOIN_TYPE_LEFT_OUTER:
                case QLV_JOIN_TYPE_RIGHT_OUTER:
                case QLV_JOIN_TYPE_FULL_OUTER:
                    STL_TRY( qlncCreateOuterJoinCandNode( aCreateNodeParamInfo,
                                                          aEnv )
                             == STL_SUCCESS );

                    aCreateNodeParamInfo->mInitNode = sInitJoinNode;

                    /* Join Order Hint 분석 */
                    if( ((qlvInitJoinTableNode*)sInitJoinNode)->mJoinType
                        == QLV_JOIN_TYPE_FULL_OUTER )
                    {
                        STL_TRY( qlncAnalyzeJoinOrderHint( aCreateNodeParamInfo,
                                                           aEnv )
                                 == STL_SUCCESS );
                    }

                    break;
                case QLV_JOIN_TYPE_LEFT_SEMI:
                case QLV_JOIN_TYPE_RIGHT_SEMI:
                case QLV_JOIN_TYPE_INVERTED_LEFT_SEMI:
                case QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI:
                    STL_TRY( qlncCreateSemiJoinCandNode( aCreateNodeParamInfo,
                                                         aEnv )
                             == STL_SUCCESS );

                    aCreateNodeParamInfo->mInitNode = sInitJoinNode;

                    break;
                case QLV_JOIN_TYPE_LEFT_ANTI_SEMI:
                case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI:
                case QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA:
                case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA:
                    STL_TRY( qlncCreateAntiSemiJoinCandNode( aCreateNodeParamInfo,
                                                             aEnv )
                             == STL_SUCCESS );

                    aCreateNodeParamInfo->mInitNode = sInitJoinNode;

                    break;
                default:
                    STL_DASSERT( 0 );
                    break;
            }

            /* Join Operation Hint 분석 */
            STL_TRY( qlncAnalyzeJoinOperationHint( aCreateNodeParamInfo,
                                                   aEnv )
                     == STL_SUCCESS );

            break;

        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Base Table Node의 Candidate Plan을 생성한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCreateBaseCandNode( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                                  qllEnv                    * aEnv )
{
    qlvInitBaseTableNode    * sInitBaseTableNode    = NULL;
    qlncBaseTableNode       * sCandBaseTableNode    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode->mType == QLV_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Base Table Validation Node 정보 획득
     */

    sInitBaseTableNode = (qlvInitBaseTableNode*)(aCreateNodeParamInfo->mInitNode);


    /**
     * Base Table Candidate Node 할당
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncBaseTableNode ),
                                (void**) &sCandBaseTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sCandBaseTableNode->mNodeCommon),
                           *(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID),
                           QLNC_NODE_TYPE_BASE_TABLE );
    (*(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID))++;  /* Global ID 증가 */
    sCandBaseTableNode->mFilter = NULL;
    QLNC_INIT_LIST( &(sCandBaseTableNode->mReadColumnList) );

    /* Access Path Hint 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncAccPathHint ),
                                (void**) &(sCandBaseTableNode->mAccPathHint),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_ACC_PATH_HINT( sCandBaseTableNode->mAccPathHint );

    /* Join Hint 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinHint ),
                                (void**) &(sCandBaseTableNode->mJoinHint),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_JOIN_HINT( sCandBaseTableNode->mJoinHint );


    /**
     * 필요한 정보 복사
     */

    /* Table Information 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncBaseTableNode ),
                                (void**) &(sCandBaseTableNode->mTableInfo),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    sCandBaseTableNode->mTableInfo->mSchemaHandle = &(sInitBaseTableNode->mSchemaHandle);
    sCandBaseTableNode->mTableInfo->mTableHandle = &(sInitBaseTableNode->mTableHandle);
    sCandBaseTableNode->mTableInfo->mTablePhyHandle = sInitBaseTableNode->mTablePhyHandle;

    /* Dump Option */
    QLNC_ALLOC_AND_COPY_STRING( sCandBaseTableNode->mTableInfo->mDumpOption,
                                sInitBaseTableNode->mDumpOption,
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );

    /* Catalog Name */
    QLNC_ALLOC_AND_COPY_STRING( sCandBaseTableNode->mTableInfo->mRelationName.mCatalog,
                                sInitBaseTableNode->mRelationName->mCatalog,
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );

    /* Schema Name */
    QLNC_ALLOC_AND_COPY_STRING( sCandBaseTableNode->mTableInfo->mRelationName.mSchema,
                                sInitBaseTableNode->mRelationName->mSchema,
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );

    /* Table Name */
    QLNC_ALLOC_AND_COPY_STRING( sCandBaseTableNode->mTableInfo->mRelationName.mTable,
                                sInitBaseTableNode->mRelationName->mTable,
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );


    /**
     * Candidate Node 설정
     */

    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandBaseTableNode;


    /**
     * Table & Index Statistic 수집
     */

    STL_TRY( qlncGatherBaseStatistic( aCreateNodeParamInfo,
                                      aEnv )
             == STL_SUCCESS );


    /**
     * Table Map Array에 추가
     */

    STL_TRY( qlncAddTableMapToTableMapArray( aCreateNodeParamInfo->mTableMapArr,
                                             (qlvInitNode*)sInitBaseTableNode,
                                             (qlncNodeCommon*)sCandBaseTableNode,
                                             aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Table Node의 Candidate Plan을 생성한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCreateSubCandNode( qlncCreateNodeParamInfo    * aCreateNodeParamInfo,
                                 qllEnv                     * aEnv )
{
    qlvInitSubTableNode     * sInitSubTableNode     = NULL;
    qlncSubTableNode        * sCandSubTableNode     = NULL;
    qlvHintInfo             * sInitHintInfo         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode->mType == QLV_NODE_TYPE_SUB_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Sub Table Validation Node 정보 획득
     */

    sInitSubTableNode = (qlvInitSubTableNode*)(aCreateNodeParamInfo->mInitNode);


    /**
     * Sub Table Candidate Node 할당
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncSubTableNode ),
                                (void**) &sCandSubTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sCandSubTableNode->mNodeCommon),
                           *(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID),
                           QLNC_NODE_TYPE_SUB_TABLE );
    (*(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID))++;  /* Global ID 증가 */
    sCandSubTableNode->mFilter = NULL;
    QLNC_INIT_LIST( &(sCandSubTableNode->mReadColumnList) );

    /* Join Hint 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinHint ),
                                (void**) &(sCandSubTableNode->mJoinHint),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_JOIN_HINT( sCandSubTableNode->mJoinHint );

    /* Relation Name 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncRelationName ),
                                (void**) &(sCandSubTableNode->mRelationName),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* View Handle 설정 */
    sCandSubTableNode->mHasHandle = sInitSubTableNode->mHasHandle;
    stlMemcpy( &(sCandSubTableNode->mSchemaHandle),
               &(sInitSubTableNode->mSchemaHandle),
               STL_SIZEOF( ellDictHandle ) );
    stlMemcpy( &(sCandSubTableNode->mViewHandle),
               &(sInitSubTableNode->mViewHandle),
               STL_SIZEOF( ellDictHandle ) );


    /**
     * 필요한 정보 복사
     */

    STL_DASSERT( sInitSubTableNode->mRelationName != NULL );

    /* Catalog Name */
    QLNC_ALLOC_AND_COPY_STRING( sCandSubTableNode->mRelationName->mCatalog,
                                sInitSubTableNode->mRelationName->mCatalog,
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );

    /* Schema Name */
    QLNC_ALLOC_AND_COPY_STRING( sCandSubTableNode->mRelationName->mSchema,
                                sInitSubTableNode->mRelationName->mSchema,
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );

    /* Table Name */
    QLNC_ALLOC_AND_COPY_STRING( sCandSubTableNode->mRelationName->mTable,
                                sInitSubTableNode->mRelationName->mTable,
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );


    /**
     * 하위 노드 처리
     */

    aCreateNodeParamInfo->mInitNode = sInitSubTableNode->mQueryNode;
    aCreateNodeParamInfo->mCandNode = NULL;
    sInitHintInfo = aCreateNodeParamInfo->mHintInfo;
    aCreateNodeParamInfo->mHintInfo = NULL;
    STL_TRY( qlncCreateQueryCandNode( aCreateNodeParamInfo,
                                      aEnv )
             == STL_SUCCESS );
    sCandSubTableNode->mQueryNode = aCreateNodeParamInfo->mCandNode;
    aCreateNodeParamInfo->mCandNode = NULL;
    aCreateNodeParamInfo->mHintInfo = sInitHintInfo;


    /**
     * Table Map Array에 추가
     */

    STL_TRY( qlncAddTableMapToTableMapArray( aCreateNodeParamInfo->mTableMapArr,
                                             (qlvInitNode*)sInitSubTableNode,
                                             (qlncNodeCommon*)sCandSubTableNode,
                                             aEnv )
             == STL_SUCCESS );

    
    /**
     * Output 설정
     */

    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandSubTableNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Inner Join Table Node의 Candidate Plan을 생성한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCreateInnerJoinCandNode( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                       qllEnv                   * aEnv )
{
    qlvInitJoinTableNode    * sInitInnerJoinTableNode   = NULL;
    qlncJoinTableNode       * sCandInnerJoinTableNode   = NULL;
    qlncAndFilter           * sAndFilter                = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode->mType == QLV_NODE_TYPE_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Join Table Validation Node 정보 획득
     */

    sInitInnerJoinTableNode = (qlvInitJoinTableNode*)(aCreateNodeParamInfo->mInitNode);


    /**
     * Join Table Candidate Node
     */

    if( aCreateNodeParamInfo->mCandNode == NULL )
    {
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncJoinTableNode ),
                                    (void**) &sCandInnerJoinTableNode,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_INIT_NODE_COMMON( &(sCandInnerJoinTableNode->mNodeCommon),
                               *(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID),
                               QLNC_NODE_TYPE_JOIN_TABLE );
        (*(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID))++;  /* Global ID 증가 */
        STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                          &(sCandInnerJoinTableNode->mNodeArray),
                                          aEnv )
                 == STL_SUCCESS );
        sCandInnerJoinTableNode->mJoinCond = NULL;
        sCandInnerJoinTableNode->mFilter = NULL;

        sCandInnerJoinTableNode->mJoinType = QLNC_JOIN_TYPE_INNER;
        sCandInnerJoinTableNode->mJoinDirect = QLNC_JOIN_DIRECT_NONE;
        sCandInnerJoinTableNode->mNamedColumnList = NULL;

        /* Semi-Join Hint 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncJoinHint ),
                                    (void**) &(sCandInnerJoinTableNode->mSemiJoinHint),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_JOIN_HINT( sCandInnerJoinTableNode->mSemiJoinHint );
    }
    else
    {
        sCandInnerJoinTableNode = (qlncJoinTableNode*)(aCreateNodeParamInfo->mCandNode);
    }


    /**
     * 필요한 정보 복사
     */

    /* Left 노드 */
    aCreateNodeParamInfo->mInitNode = sInitInnerJoinTableNode->mLeftTableNode;
    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandInnerJoinTableNode;
    STL_TRY( qlncCreateChildCandNodeFromInnerJoin( aCreateNodeParamInfo,
                                                   ( ( (sInitInnerJoinTableNode->mJoinSpec != NULL) &&
                                                       (sInitInnerJoinTableNode->mJoinSpec->mNamedColumnList != NULL) )
                                                     ? STL_TRUE : STL_FALSE ),
                                                   aEnv )
             == STL_SUCCESS );
    sCandInnerJoinTableNode = (qlncJoinTableNode*)(aCreateNodeParamInfo->mCandNode);
    aCreateNodeParamInfo->mCandNode = NULL;

    /* Right 노드 */
    aCreateNodeParamInfo->mInitNode = sInitInnerJoinTableNode->mRightTableNode;
    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandInnerJoinTableNode;
    STL_TRY( qlncCreateChildCandNodeFromInnerJoin( aCreateNodeParamInfo,
                                                   ( ( (sInitInnerJoinTableNode->mJoinSpec != NULL) &&
                                                       (sInitInnerJoinTableNode->mJoinSpec->mNamedColumnList != NULL) )
                                                     ? STL_TRUE : STL_FALSE ),
                                                   aEnv )
             == STL_SUCCESS );
    sCandInnerJoinTableNode = (qlncJoinTableNode*)(aCreateNodeParamInfo->mCandNode);
    aCreateNodeParamInfo->mCandNode = NULL;


    /**
     * Join Condition 처리
     */

    if( sInitInnerJoinTableNode->mJoinSpec != NULL )
    {
        STL_DASSERT( sInitInnerJoinTableNode->mJoinSpec->mJoinCondition != NULL );
        STL_TRY( qlncConvertCondition( aCreateNodeParamInfo,
                                       sInitInnerJoinTableNode->mJoinSpec->mJoinCondition,
                                       QLNC_EXPR_PHRASE_WHERE,
                                       &sAndFilter,
                                       aEnv )
                 == STL_SUCCESS );

        if( sCandInnerJoinTableNode->mJoinCond == NULL )
        {
            sCandInnerJoinTableNode->mJoinCond = sAndFilter;
        }
        else
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCreateNodeParamInfo->mParamInfo),
                                                        sCandInnerJoinTableNode->mJoinCond,
                                                        sAndFilter->mOrCount,
                                                        sAndFilter->mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Named Column List 설정
     */

    if( (sInitInnerJoinTableNode->mJoinSpec != NULL) &&
        (sInitInnerJoinTableNode->mJoinSpec->mNamedColumnList != NULL) )
    {
        qlvNamedColumnItem  * sInitNamedColumnItem  = NULL;
        qlncNamedColumnItem * sCandNamedColumnItem  = NULL;

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncNamedColumnList ),
                                    (void**) &(sCandInnerJoinTableNode->mNamedColumnList),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_LIST(sCandInnerJoinTableNode->mNamedColumnList);

        sInitNamedColumnItem = sInitInnerJoinTableNode->mJoinSpec->mNamedColumnList->mHead;
        while( sInitNamedColumnItem != NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncNamedColumnItem ),
                                        (void**) &(sCandNamedColumnItem),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY( qlncConvertExpression( aCreateNodeParamInfo,
                                           sInitNamedColumnItem->mLeftExpr,
                                           QLNC_EXPR_PHRASE_TARGET,
                                           NULL,
                                           &sCandNamedColumnItem->mLeftExpr,
                                           aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncConvertExpression( aCreateNodeParamInfo,
                                           sInitNamedColumnItem->mRightExpr,
                                           QLNC_EXPR_PHRASE_TARGET,
                                           NULL,
                                           &sCandNamedColumnItem->mRightExpr,
                                           aEnv )
                     == STL_SUCCESS );

            sCandNamedColumnItem->mNext = NULL;

            if( sCandInnerJoinTableNode->mNamedColumnList->mCount == 0 )
            {
                sCandInnerJoinTableNode->mNamedColumnList->mHead = sCandNamedColumnItem;
                sCandInnerJoinTableNode->mNamedColumnList->mTail = sCandNamedColumnItem;
            }
            else
            {
                sCandInnerJoinTableNode->mNamedColumnList->mTail->mNext = sCandNamedColumnItem;
                sCandInnerJoinTableNode->mNamedColumnList->mTail = sCandNamedColumnItem;
            }
            sCandInnerJoinTableNode->mNamedColumnList->mCount++;

            sInitNamedColumnItem = sInitNamedColumnItem->mNext;
        }
    }
    else
    {
        sCandInnerJoinTableNode->mNamedColumnList = NULL;
    }


    /**
     * Output 설정
     */

    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandInnerJoinTableNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Outer Join Table Node의 Candidate Plan을 생성한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCreateOuterJoinCandNode( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                       qllEnv                   * aEnv )
{
    qlvInitJoinTableNode    * sInitOuterJoinTableNode   = NULL;
    qlncJoinTableNode       * sCandOuterJoinTableNode   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode->mType == QLV_NODE_TYPE_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Join Table Validation Node 정보 획득
     */

    sInitOuterJoinTableNode = (qlvInitJoinTableNode*)(aCreateNodeParamInfo->mInitNode);


    /**
     * Join Table Candidate Node
     */

    STL_DASSERT( aCreateNodeParamInfo->mCandNode == NULL );
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinTableNode ),
                                (void**) &sCandOuterJoinTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sCandOuterJoinTableNode->mNodeCommon),
                           *(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID),
                           QLNC_NODE_TYPE_JOIN_TABLE );
    (*(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID))++;  /* Global ID 증가 */
    STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                      &(sCandOuterJoinTableNode->mNodeArray),
                                      aEnv )
             == STL_SUCCESS );
    sCandOuterJoinTableNode->mFilter = NULL;
    sCandOuterJoinTableNode->mNamedColumnList = NULL;

    /* Semi-Join Hint 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinHint ),
                                (void**) &(sCandOuterJoinTableNode->mSemiJoinHint),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_JOIN_HINT( sCandOuterJoinTableNode->mSemiJoinHint );


    /**
     * 필요한 정보 복사
     */

    /* Join Type */
    sCandOuterJoinTableNode->mJoinType = QLNC_JOIN_TYPE_OUTER;

    /* Join Direct */
    switch( sInitOuterJoinTableNode->mJoinType )
    {
        case QLV_JOIN_TYPE_LEFT_OUTER:
            sCandOuterJoinTableNode->mJoinDirect = QLNC_JOIN_DIRECT_LEFT;
            break;
        case QLV_JOIN_TYPE_RIGHT_OUTER:
            sCandOuterJoinTableNode->mJoinDirect = QLNC_JOIN_DIRECT_RIGHT;
            break;
        case QLV_JOIN_TYPE_FULL_OUTER:
            sCandOuterJoinTableNode->mJoinDirect = QLNC_JOIN_DIRECT_FULL;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Left 노드 */
    aCreateNodeParamInfo->mInitNode = sInitOuterJoinTableNode->mLeftTableNode;
    aCreateNodeParamInfo->mCandNode = NULL;
    STL_TRY( qlncCreateChildCandNodeFromNonInnerJoin( aCreateNodeParamInfo,
                                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( qlncAddNodeToNodeArray( &(sCandOuterJoinTableNode->mNodeArray),
                                     aCreateNodeParamInfo->mCandNode,
                                     aEnv )
             == STL_SUCCESS );

    aCreateNodeParamInfo->mCandNode = NULL;

    /* Right 노드 */
    aCreateNodeParamInfo->mInitNode = sInitOuterJoinTableNode->mRightTableNode;
    aCreateNodeParamInfo->mCandNode = NULL;
    STL_TRY( qlncCreateChildCandNodeFromNonInnerJoin( aCreateNodeParamInfo,
                                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( qlncAddNodeToNodeArray( &(sCandOuterJoinTableNode->mNodeArray),
                                     aCreateNodeParamInfo->mCandNode,
                                     aEnv )
             == STL_SUCCESS );

    aCreateNodeParamInfo->mCandNode = NULL;


    /**
     * Join Condition 처리
     */

    STL_DASSERT( sInitOuterJoinTableNode->mJoinSpec != NULL );
    STL_DASSERT( sInitOuterJoinTableNode->mJoinSpec->mJoinCondition != NULL );
    STL_TRY( qlncConvertCondition( aCreateNodeParamInfo,
                                   sInitOuterJoinTableNode->mJoinSpec->mJoinCondition,
                                   QLNC_EXPR_PHRASE_WHERE,
                                   &(sCandOuterJoinTableNode->mJoinCond),
                                   aEnv )
             == STL_SUCCESS );


    /**
     * Output 설정
     */

    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandOuterJoinTableNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Semi Join Table Node의 Candidate Plan을 생성한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCreateSemiJoinCandNode( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                                      qllEnv                    * aEnv )
{
    qlvInitJoinTableNode    * sInitSemiJoinTableNode    = NULL;
    qlncJoinTableNode       * sCandSemiJoinTableNode    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode->mType == QLV_NODE_TYPE_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Join Table Validation Node 정보 획득
     */

    sInitSemiJoinTableNode = (qlvInitJoinTableNode*)(aCreateNodeParamInfo->mInitNode);


    /**
     * Join Table Candidate Node
     */

    STL_DASSERT( aCreateNodeParamInfo->mCandNode == NULL );
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinTableNode ),
                                (void**) &sCandSemiJoinTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sCandSemiJoinTableNode->mNodeCommon),
                           *(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID),
                           QLNC_NODE_TYPE_JOIN_TABLE );
    (*(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID))++;  /* Global ID 증가 */
    STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                      &(sCandSemiJoinTableNode->mNodeArray),
                                      aEnv )
             == STL_SUCCESS );
    sCandSemiJoinTableNode->mFilter = NULL;
    sCandSemiJoinTableNode->mNamedColumnList = NULL;

    /* Semi-Join Hint 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinHint ),
                                (void**) &(sCandSemiJoinTableNode->mSemiJoinHint),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_JOIN_HINT( sCandSemiJoinTableNode->mSemiJoinHint );


    /**
     * 필요한 정보 복사
     */

    /* Join Type */
    sCandSemiJoinTableNode->mJoinType = QLNC_JOIN_TYPE_SEMI;

    /* Join Direct */
    switch( sInitSemiJoinTableNode->mJoinType )
    {
        case QLV_JOIN_TYPE_LEFT_SEMI:
        case QLV_JOIN_TYPE_INVERTED_LEFT_SEMI:
            sCandSemiJoinTableNode->mJoinDirect = QLNC_JOIN_DIRECT_LEFT;
            break;
        case QLV_JOIN_TYPE_RIGHT_SEMI:
        case QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI:
            sCandSemiJoinTableNode->mJoinDirect = QLNC_JOIN_DIRECT_RIGHT;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Left 노드 */
    aCreateNodeParamInfo->mInitNode = sInitSemiJoinTableNode->mLeftTableNode;
    aCreateNodeParamInfo->mCandNode = NULL;
    STL_TRY( qlncCreateChildCandNodeFromNonInnerJoin( aCreateNodeParamInfo,
                                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( qlncAddNodeToNodeArray( &(sCandSemiJoinTableNode->mNodeArray),
                                     aCreateNodeParamInfo->mCandNode,
                                     aEnv )
             == STL_SUCCESS );

    aCreateNodeParamInfo->mCandNode = NULL;

    /* Right 노드 */
    aCreateNodeParamInfo->mInitNode = sInitSemiJoinTableNode->mRightTableNode;
    aCreateNodeParamInfo->mCandNode = NULL;
    STL_TRY( qlncCreateChildCandNodeFromNonInnerJoin( aCreateNodeParamInfo,
                                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( qlncAddNodeToNodeArray( &(sCandSemiJoinTableNode->mNodeArray),
                                     aCreateNodeParamInfo->mCandNode,
                                     aEnv )
             == STL_SUCCESS );

    aCreateNodeParamInfo->mCandNode = NULL;


    /**
     * Join Condition 처리
     */

    STL_DASSERT( sInitSemiJoinTableNode->mJoinSpec != NULL );
    STL_DASSERT( sInitSemiJoinTableNode->mJoinSpec->mJoinCondition != NULL );
    STL_TRY( qlncConvertCondition( aCreateNodeParamInfo,
                                   sInitSemiJoinTableNode->mJoinSpec->mJoinCondition,
                                   QLNC_EXPR_PHRASE_WHERE,
                                   &(sCandSemiJoinTableNode->mJoinCond),
                                   aEnv )
             == STL_SUCCESS );

    
    /**
     * Output 설정
     */

    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandSemiJoinTableNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Anti-Semi Join Table Node의 Candidate Plan을 생성한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCreateAntiSemiJoinCandNode( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                                          qllEnv                    * aEnv )
{
    qlvInitJoinTableNode    * sInitAntiSemiJoinTableNode    = NULL;
    qlncJoinTableNode       * sCandAntiSemiJoinTableNode    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode->mType == QLV_NODE_TYPE_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Join Table Validation Node 정보 획득
     */

    sInitAntiSemiJoinTableNode = (qlvInitJoinTableNode*)(aCreateNodeParamInfo->mInitNode);


    /**
     * Join Table Candidate Node
     */

    STL_DASSERT( aCreateNodeParamInfo->mCandNode == NULL );
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinTableNode ),
                                (void**) &sCandAntiSemiJoinTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sCandAntiSemiJoinTableNode->mNodeCommon),
                           *(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID),
                           QLNC_NODE_TYPE_JOIN_TABLE );
    (*(aCreateNodeParamInfo->mParamInfo.mGlobalNodeID))++;  /* Global ID 증가 */
    STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                      &(sCandAntiSemiJoinTableNode->mNodeArray),
                                      aEnv )
             == STL_SUCCESS );
    sCandAntiSemiJoinTableNode->mFilter = NULL;
    sCandAntiSemiJoinTableNode->mNamedColumnList = NULL;

    /* Semi-Join Hint 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinHint ),
                                (void**) &(sCandAntiSemiJoinTableNode->mSemiJoinHint),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_JOIN_HINT( sCandAntiSemiJoinTableNode->mSemiJoinHint );


    /**
     * 필요한 정보 복사
     */

    /* Join Type 및 Join Direct */
    switch( sInitAntiSemiJoinTableNode->mJoinType )
    {
        case QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA:
            sCandAntiSemiJoinTableNode->mJoinType = QLNC_JOIN_TYPE_ANTI_SEMI_NA;
            sCandAntiSemiJoinTableNode->mJoinDirect = QLNC_JOIN_DIRECT_LEFT;
            break;
        case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA:
            sCandAntiSemiJoinTableNode->mJoinType = QLNC_JOIN_TYPE_ANTI_SEMI_NA;
            sCandAntiSemiJoinTableNode->mJoinDirect = QLNC_JOIN_DIRECT_RIGHT;
            break;
        /* case QLV_JOIN_TYPE_LEFT_ANTI_SEMI: */
        /* case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI: */
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Left 노드 */
    aCreateNodeParamInfo->mInitNode = sInitAntiSemiJoinTableNode->mLeftTableNode;
    aCreateNodeParamInfo->mCandNode = NULL;
    STL_TRY( qlncCreateChildCandNodeFromNonInnerJoin( aCreateNodeParamInfo,
                                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( qlncAddNodeToNodeArray( &(sCandAntiSemiJoinTableNode->mNodeArray),
                                     aCreateNodeParamInfo->mCandNode,
                                     aEnv )
             == STL_SUCCESS );

    aCreateNodeParamInfo->mCandNode = NULL;

    /* Right 노드 */
    aCreateNodeParamInfo->mInitNode = sInitAntiSemiJoinTableNode->mRightTableNode;
    aCreateNodeParamInfo->mCandNode = NULL;
    STL_TRY( qlncCreateChildCandNodeFromNonInnerJoin( aCreateNodeParamInfo,
                                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( qlncAddNodeToNodeArray( &(sCandAntiSemiJoinTableNode->mNodeArray),
                                     aCreateNodeParamInfo->mCandNode,
                                     aEnv )
             == STL_SUCCESS );

    aCreateNodeParamInfo->mCandNode = NULL;


    /**
     * Join Condition 처리
     */

    STL_DASSERT( sInitAntiSemiJoinTableNode->mJoinSpec != NULL );
    STL_DASSERT( sInitAntiSemiJoinTableNode->mJoinSpec->mJoinCondition != NULL );
    STL_TRY( qlncConvertCondition( aCreateNodeParamInfo,
                                   sInitAntiSemiJoinTableNode->mJoinSpec->mJoinCondition,
                                   QLNC_EXPR_PHRASE_WHERE,
                                   &(sCandAntiSemiJoinTableNode->mJoinCond),
                                   aEnv )
             == STL_SUCCESS );

    
    /**
     * Output 설정
     */

    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandAntiSemiJoinTableNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Inner Join의 Child Node에 대한 Candidate Plan을 생성한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aHasNamedColumnList     Inner Join이 Named ColumnList를 가졌는지 여부
 * @param[in]   aEnv                    Environment
 *
 * @remark Child Node가 2개 이상으로 여러개 올 수 있는 경우
 */
stlStatus qlncCreateChildCandNodeFromInnerJoin( qlncCreateNodeParamInfo * aCreateNodeParamInfo,
                                                stlBool                   aHasNamedColumnList,
                                                qllEnv                  * aEnv )
{
    qlncJoinTableNode   * sCandInnerJoinTableNode   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aCreateNodeParamInfo->mInitNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
            sCandInnerJoinTableNode = (qlncJoinTableNode*)(aCreateNodeParamInfo->mCandNode);
            aCreateNodeParamInfo->mCandNode = NULL;

            STL_TRY( qlncCreateBaseCandNode( aCreateNodeParamInfo,
                                             aEnv )
                     == STL_SUCCESS );

            /* Access Path Hint 분석 */
            STL_TRY( qlncAnalyzeAccessPathHint( aCreateNodeParamInfo,
                                                aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncAddNodeToNodeArray( &(sCandInnerJoinTableNode->mNodeArray),
                                             aCreateNodeParamInfo->mCandNode,
                                             aEnv )
                     == STL_SUCCESS );

            aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandInnerJoinTableNode;

            break;

        case QLV_NODE_TYPE_SUB_TABLE:
            sCandInnerJoinTableNode = (qlncJoinTableNode*)(aCreateNodeParamInfo->mCandNode);
            aCreateNodeParamInfo->mCandNode = NULL;

            STL_TRY( qlncCreateSubCandNode( aCreateNodeParamInfo,
                                            aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncAddNodeToNodeArray( &(sCandInnerJoinTableNode->mNodeArray),
                                             aCreateNodeParamInfo->mCandNode,
                                             aEnv )
                     == STL_SUCCESS );

            aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandInnerJoinTableNode;

            break;

        case QLV_NODE_TYPE_JOIN_TABLE:
            if( (((qlvInitJoinTableNode*)(aCreateNodeParamInfo->mInitNode))->mJoinType == QLV_JOIN_TYPE_NONE) ||
                (((qlvInitJoinTableNode*)(aCreateNodeParamInfo->mInitNode))->mJoinType == QLV_JOIN_TYPE_CROSS) ||
                (((qlvInitJoinTableNode*)(aCreateNodeParamInfo->mInitNode))->mJoinType == QLV_JOIN_TYPE_INNER) )
            {
                if( ( aHasNamedColumnList == STL_TRUE ) ||
                    ( (((qlvInitJoinTableNode*)(aCreateNodeParamInfo->mInitNode))->mJoinSpec != NULL) &&
                      (((qlvInitJoinTableNode*)(aCreateNodeParamInfo->mInitNode))->mJoinSpec->mNamedColumnList != NULL) ) )
                {
                    sCandInnerJoinTableNode = (qlncJoinTableNode*)(aCreateNodeParamInfo->mCandNode);
                    aCreateNodeParamInfo->mCandNode = NULL;

                    STL_TRY( qlncCreateInnerJoinCandNode( aCreateNodeParamInfo,
                                                          aEnv )
                             == STL_SUCCESS );

                    /* Join Order Hint 분석 */
                    STL_TRY( qlncAnalyzeJoinOrderHint( aCreateNodeParamInfo,
                                                       aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlncAddNodeToNodeArray( &(sCandInnerJoinTableNode->mNodeArray),
                                                     aCreateNodeParamInfo->mCandNode,
                                                     aEnv )
                             == STL_SUCCESS );

                    /* Join Operation Hint 분석 */
                    STL_TRY( qlncAnalyzeJoinOperationHint( aCreateNodeParamInfo,
                                                           aEnv )
                             == STL_SUCCESS );

                    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandInnerJoinTableNode;
                }
                else
                {
                    STL_TRY( qlncCreateInnerJoinCandNode( aCreateNodeParamInfo,
                                                          aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                sCandInnerJoinTableNode = (qlncJoinTableNode*)(aCreateNodeParamInfo->mCandNode);
                aCreateNodeParamInfo->mCandNode = NULL;

                switch( ((qlvInitJoinTableNode*)(aCreateNodeParamInfo->mInitNode))->mJoinType )
                {
                    case QLV_JOIN_TYPE_LEFT_OUTER:
                    case QLV_JOIN_TYPE_RIGHT_OUTER:
                        STL_TRY( qlncCreateOuterJoinCandNode( aCreateNodeParamInfo,
                                                              aEnv )
                                 == STL_SUCCESS );
                        break;
                    case QLV_JOIN_TYPE_FULL_OUTER:
                        STL_TRY( qlncCreateOuterJoinCandNode( aCreateNodeParamInfo,
                                                              aEnv )
                                 == STL_SUCCESS );

                        /* Join Order Hint 분석 */
                        STL_TRY( qlncAnalyzeJoinOrderHint( aCreateNodeParamInfo,
                                                           aEnv )
                                 == STL_SUCCESS );

                        break;
                    case QLV_JOIN_TYPE_LEFT_SEMI:
                    case QLV_JOIN_TYPE_RIGHT_SEMI:
                    case QLV_JOIN_TYPE_INVERTED_LEFT_SEMI:
                    case QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI:
                        STL_TRY( qlncCreateSemiJoinCandNode( aCreateNodeParamInfo,
                                                             aEnv )
                                 == STL_SUCCESS );
                        break;
                    case QLV_JOIN_TYPE_LEFT_ANTI_SEMI:
                    case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI:
                    case QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA:
                    case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA:
                        STL_TRY( qlncCreateAntiSemiJoinCandNode( aCreateNodeParamInfo,
                                                                 aEnv )
                                 == STL_SUCCESS );
                        break;
                    default:
                        STL_DASSERT( 0 );
                        break;
                }

                STL_TRY( qlncAddNodeToNodeArray( &(sCandInnerJoinTableNode->mNodeArray),
                                                 aCreateNodeParamInfo->mCandNode,
                                                 aEnv )
                         == STL_SUCCESS );

                /* Join Operation Hint 분석 */
                STL_TRY( qlncAnalyzeJoinOperationHint( aCreateNodeParamInfo,
                                                       aEnv )
                         == STL_SUCCESS );

                aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sCandInnerJoinTableNode;
            }
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Non Inner Join의 Child Node에 대한 Candidate Plan을 생성한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 *
 * @remark Child Node가 2개만 올 수 있는 경우
 */
stlStatus qlncCreateChildCandNodeFromNonInnerJoin( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                                   qllEnv                   * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aCreateNodeParamInfo->mInitNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
            STL_TRY( qlncCreateBaseCandNode( aCreateNodeParamInfo,
                                             aEnv )
                     == STL_SUCCESS );

            /* Access Path Hint 분석 */
            STL_TRY( qlncAnalyzeAccessPathHint( aCreateNodeParamInfo,
                                                aEnv )
                     == STL_SUCCESS );

            break;

        case QLV_NODE_TYPE_SUB_TABLE:
            STL_TRY( qlncCreateSubCandNode( aCreateNodeParamInfo,
                                            aEnv )
                     == STL_SUCCESS );

            break;

        case QLV_NODE_TYPE_JOIN_TABLE:
            switch( ((qlvInitJoinTableNode*)(aCreateNodeParamInfo->mInitNode))->mJoinType )
            {
                case QLV_JOIN_TYPE_NONE:
                case QLV_JOIN_TYPE_CROSS:
                case QLV_JOIN_TYPE_INNER:
                    STL_TRY( qlncCreateInnerJoinCandNode( aCreateNodeParamInfo,
                                                          aEnv )
                             == STL_SUCCESS );

                    /* Join Order Hint 분석 */
                    STL_TRY( qlncAnalyzeJoinOrderHint( aCreateNodeParamInfo,
                                                       aEnv )
                             == STL_SUCCESS );

                    break;
                case QLV_JOIN_TYPE_LEFT_OUTER:
                case QLV_JOIN_TYPE_RIGHT_OUTER:
                    STL_TRY( qlncCreateOuterJoinCandNode( aCreateNodeParamInfo,
                                                          aEnv )
                             == STL_SUCCESS );
                    break;
                case QLV_JOIN_TYPE_FULL_OUTER:
                    STL_TRY( qlncCreateOuterJoinCandNode( aCreateNodeParamInfo,
                                                          aEnv )
                             == STL_SUCCESS );

                    /* Join Order Hint 분석 */
                    STL_TRY( qlncAnalyzeJoinOrderHint( aCreateNodeParamInfo,
                                                       aEnv )
                             == STL_SUCCESS );

                    break;
                case QLV_JOIN_TYPE_LEFT_SEMI:
                case QLV_JOIN_TYPE_RIGHT_SEMI:
                case QLV_JOIN_TYPE_INVERTED_LEFT_SEMI:
                case QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI:
                    STL_TRY( qlncCreateSemiJoinCandNode( aCreateNodeParamInfo,
                                                         aEnv )
                             == STL_SUCCESS );
                    break;
                case QLV_JOIN_TYPE_LEFT_ANTI_SEMI:
                case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI:
                case QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA:
                case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA:
                    STL_TRY( qlncCreateAntiSemiJoinCandNode( aCreateNodeParamInfo,
                                                             aEnv )
                             == STL_SUCCESS );
                    break;
                default:
                    STL_DASSERT( 0 );
                    break;
            }

            /* Join Operation Hint 분석 */
            STL_TRY( qlncAnalyzeJoinOperationHint( aCreateNodeParamInfo,
                                                   aEnv )
                     == STL_SUCCESS );

            break;

        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Merge Join을 위한 Sort Instant Node를 생성한다.
 * @param[in]       aParamInfo                  Parameter Info
 * @param[in]       aInstantType                Instant Type
 * @param[in]       aSortTableAttr              Sort Table Attribute
 * @param[in]       aScrollable                 Scroll 여부 설정
 * @param[in]       aSortKeyCount               Sort Key Count
 * @param[in,out]   aSortKeyArr                 Sort Key Array
 * @param[in]       aSortKeyFlags               Sort Key Flags
 * @param[in]       aChildNode                  Child Node
 * @param[out]      aSortInstant                Sort Instant Node
 * @param[in]       aEnv                        Environment
 */
stlStatus qlncCreateSortInstantForMergeJoin( qlncParamInfo              * aParamInfo,
                                             qlncInstantType              aInstantType,
                                             smlSortTableAttr           * aSortTableAttr,
                                             ellCursorScrollability       aScrollable,
                                             stlInt32                     aSortKeyCount,
                                             qlncExprCommon            ** aSortKeyArr,
                                             stlUInt8                   * aSortKeyFlags,
                                             qlncNodeCommon             * aChildNode,
                                             qlncNodeCommon            ** aSortInstant,
                                             qllEnv                     * aEnv )
{
    stlInt32                  i;
    stlInt32                  sIdx;
    qlncSortInstantNode     * sSortInstantNode  = NULL;
    qlncExprCommon          * sExpr             = NULL;
    qlncRefExprItem         * sRefExprItem      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSortKeyCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSortKeyArr != NULL, QLL_ERROR_STACK(aEnv) );


    /* Sort Instant Node 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncSortInstantNode ),
                                (void**) &sSortInstantNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sSortInstantNode->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_SORT_INSTANT );
    (*(aParamInfo->mGlobalNodeID))++;  /* Global ID 증가 */

    /* Semi-Join Hint 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinHint ),
                                (void**) &(sSortInstantNode->mSemiJoinHint),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_JOIN_HINT( sSortInstantNode->mSemiJoinHint );

    /* Child Node 설정 */
    sSortInstantNode->mChildNode = aChildNode;

    /* Instant Type 설정 */
    sSortInstantNode->mInstantType = aInstantType;

    /* Sort Table Attribute 설정 */
    sSortInstantNode->mSortTableAttr = *aSortTableAttr;

    /* Scrollable 설정 */
    sSortInstantNode->mScrollable = aScrollable;

    /* Key List 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncRefExprList ),
                                (void**) &(sSortInstantNode->mKeyColList),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_LIST( sSortInstantNode->mKeyColList );

    /* Key Flag 설정 */
    sSortInstantNode->mKeyFlags = aSortKeyFlags;

    /* Sort Key 등록 */
    sIdx = 0;
    for( i = 0; i < aSortKeyCount; i++ )
    {
        /* 이미 Key Column에 등록되었는지 판단 */
        sRefExprItem = sSortInstantNode->mKeyColList->mHead;
        while( sRefExprItem != NULL )
        {
            STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
            if( qlncIsSameExpr( aSortKeyArr[i],
                                ((qlncExprInnerColumn*)(sRefExprItem->mExpr))->mOrgExpr )
                == STL_TRUE )
            {
                break;
            }

            sRefExprItem = sRefExprItem->mNext;
        }

        if( sRefExprItem != NULL )
        {
            sExpr = sRefExprItem->mExpr;
        }
        else
        {
            /* Inner Column으로 감싸 등록 */
            STL_TRY( qlncWrapExprInInnerColumn( &QLL_CANDIDATE_MEM( aEnv ),
                                                aSortKeyArr[i],
                                                (qlncNodeCommon*)sSortInstantNode,
                                                sIdx,
                                                &sExpr,
                                                aEnv )
                     == STL_SUCCESS );

            sIdx++;

            /* Reference Expression Item 할당 */
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefExprItem ),
                                        (void**) &sRefExprItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sRefExprItem->mExpr = sExpr;
            sRefExprItem->mNext = NULL;

            QLNC_APPEND_ITEM_TO_LIST( sSortInstantNode->mKeyColList, sRefExprItem );
        }

        /* 상위 Sort Key를 Inner Column으로 변경 */
        aSortKeyArr[i] = sExpr;
    }

    sSortInstantNode->mRecColList = NULL;
    sSortInstantNode->mReadColList = NULL;


    /* Filter 설정 */
    sSortInstantNode->mKeyFilter = NULL;
    sSortInstantNode->mRecFilter = NULL;
    sSortInstantNode->mFilter = NULL;

    /* Non Filter SubQuery Expression List 설정 */
    sSortInstantNode->mNonFilterSubQueryExprList = NULL;


    /**
     * Output 설정
     */

    *aSortInstant = (qlncNodeCommon*)sSortInstantNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Join을 위한 Hash Instant Node를 생성한다.
 * @param[in]       aParamInfo                  Parameter Info
 * @param[in]       aInstantType                Instant Type
 * @param[in]       aHashTableAttr              Hash Table Attribute
 * @param[in]       aScrollable                 Scroll 여부 설정
 * @param[in]       aHashKeyCount               Hash Key Count
 * @param[in,out]   aHashKeyArr                 Hash Key Array
 * @param[in]       aChildNode                  Child Node
 * @param[out]      aHashInstant                Hash Instant Node
 * @param[in]       aEnv                        Environment
 */
stlStatus qlncCreateHashInstantForHashJoin( qlncParamInfo           * aParamInfo,
                                            qlncInstantType           aInstantType,
                                            smlIndexAttr            * aHashTableAttr,
                                            ellCursorScrollability    aScrollable,
                                            stlInt32                  aHashKeyCount,
                                            qlncExprCommon         ** aHashKeyArr,
                                            qlncNodeCommon          * aChildNode,
                                            qlncNodeCommon         ** aHashInstant,
                                            qllEnv                  * aEnv )
{
    stlInt32                  i;
    stlInt32                  sIdx;
    qlncHashInstantNode     * sHashInstantNode  = NULL;
    qlncExprCommon          * sExpr             = NULL;
    qlncRefExprItem         * sRefExprItem      = NULL;

    
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashKeyCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashKeyArr != NULL, QLL_ERROR_STACK(aEnv) );


    /* Hash Instant Node 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncHashInstantNode ),
                                (void**) &sHashInstantNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sHashInstantNode->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_HASH_INSTANT );
    (*(aParamInfo->mGlobalNodeID))++;  /* Global ID 증가 */

    /* Semi-Join Hint 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinHint ),
                                (void**) &(sHashInstantNode->mSemiJoinHint),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_JOIN_HINT( sHashInstantNode->mSemiJoinHint );

    /* Child Node 설정 */
    sHashInstantNode->mChildNode = aChildNode;

    /* Instant Type 설정 */
    sHashInstantNode->mInstantType = aInstantType;

    /* Hash Table Attribute 설정 */
    sHashInstantNode->mHashTableAttr = *aHashTableAttr;

    /* Scrollable 설정 */
    sHashInstantNode->mScrollable = aScrollable;

    /* Key List 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncRefExprList ),
                                (void**) &(sHashInstantNode->mKeyColList),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_LIST( sHashInstantNode->mKeyColList );

    /* Hash Key 등록 */
    sIdx = 0;
    for( i = 0; i < aHashKeyCount; i++, sIdx++ )
    {
        /* Inner Column으로 감싸 등록 */
        STL_TRY( qlncWrapExprInInnerColumn( &QLL_CANDIDATE_MEM( aEnv ),
                                            aHashKeyArr[i],
                                            (qlncNodeCommon*)sHashInstantNode,
                                            sIdx,
                                            &sExpr,
                                            aEnv )
                 == STL_SUCCESS );

        /* 상위 Hash Key를 Inner Column으로 변경 */
        aHashKeyArr[i] = sExpr;

        /* Reference Expression Item 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncRefExprItem ),
                                    (void**) &sRefExprItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRefExprItem->mExpr = sExpr;
        sRefExprItem->mNext = NULL;

        QLNC_APPEND_ITEM_TO_LIST( sHashInstantNode->mKeyColList, sRefExprItem );
    }

    sHashInstantNode->mRecColList = NULL;
    sHashInstantNode->mReadColList = NULL;

    /* Filter 설정 */
    sHashInstantNode->mKeyFilter = NULL;
    sHashInstantNode->mRecFilter = NULL;
    sHashInstantNode->mFilter = NULL;

    /* Non Filter SubQuery Expression List 설정 */
    sHashInstantNode->mNonFilterSubQueryExprList = NULL;


    /**
     * Output 설정
     */

    *aHashInstant = (qlncNodeCommon*)sHashInstantNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sort Instant Node를 생성한다.
 * @param[in]       aParamInfo                  Parameter Info
 * @param[in]       aInstantType                Instant Type
 * @param[in]       aSortTableAttr              Sort Table Attribute
 * @param[in]       aScrollable                 Scroll 여부 설정
 * @param[in]       aChildNode                  Child Node
 * @param[out]      aSortInstant                Sort Instant Node
 * @param[in]       aEnv                        Environment
 */
stlStatus qlncCreateSortInstant( qlncParamInfo              * aParamInfo,
                                 qlncInstantType              aInstantType,
                                 smlSortTableAttr           * aSortTableAttr,
                                 ellCursorScrollability       aScrollable,
                                 qlncNodeCommon             * aChildNode,
                                 qlncSortInstantNode       ** aSortInstant,
                                 qllEnv                     * aEnv )
{
    qlncSortInstantNode     * sSortInstantNode  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSortTableAttr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Sort Instant Node 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncSortInstantNode ),
                                (void**) &sSortInstantNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sSortInstantNode, 0x00, STL_SIZEOF( qlncSortInstantNode ) );

    QLNC_INIT_NODE_COMMON( &(sSortInstantNode->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_SORT_INSTANT );
    (*(aParamInfo->mGlobalNodeID))++;  /* Global ID 증가 */

    /* Semi-Join Hint 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinHint ),
                                (void**) &(sSortInstantNode->mSemiJoinHint),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_JOIN_HINT( sSortInstantNode->mSemiJoinHint );

    /* Child Node 설정 */
    sSortInstantNode->mChildNode = aChildNode;

    /* Instant Type 설정 */
    sSortInstantNode->mInstantType = aInstantType;

    /* Sort Table Attribute 설정 */
    sSortInstantNode->mSortTableAttr = *aSortTableAttr;

    /* Scrollable 설정 */
    sSortInstantNode->mScrollable = aScrollable;


    /**
     * Output 설정
     */

    *aSortInstant = sSortInstantNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sort Instant Node에 Column 정보를 추가한다.
 * @param[in]       aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]       aColumnList             Column List
 * @param[in]       aKeyFlags               Key Flags (for Key Column Only )
 * @param[in]       aColumnType             Column Type
 * @param[in]       aExprPhraseFlag         Expression Phrase Flag
 * @param[in,out]   aSortInstant            Sort Instant
 * @param[in]       aEnv                    Environment
 */
stlStatus qlncAddColumnOnSortInstant( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                                      qlvRefExprList            * aColumnList,
                                      stlUInt8                  * aKeyFlags,
                                      stlInt16                    aColumnType,
                                      stlInt32                    aExprPhraseFlag,
                                      qlncSortInstantNode       * aSortInstant,
                                      qllEnv                    * aEnv )
{
    stlInt32              sIdx;
    qlncExprCommon      * sSortColumn       = NULL;
    qlvRefExprItem      * sInitRefExprItem  = NULL;
    qlncRefExprItem     * sCandRefExprItem  = NULL;
    qlncRefExprList       sTmpSubQueryExprList;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSortInstant != NULL, QLL_ERROR_STACK(aEnv) );


    if( aColumnType == QLNC_INSTANT_COLUMN_TYPE_KEY )
    {
        if( aSortInstant->mKeyColList == NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefExprList ),
                                        (void**) &(aSortInstant->mKeyColList),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            QLNC_INIT_LIST( aSortInstant->mKeyColList );
        }

        /* Column 등록 */
        sIdx = 0;
        sInitRefExprItem = aColumnList->mHead;
        while( sInitRefExprItem != NULL )
        {
            STL_DASSERT( sInitRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

            QLNC_INIT_LIST( &sTmpSubQueryExprList );
            STL_TRY( qlncConvertExpression( aCreateNodeParamInfo,
                                            sInitRefExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                            aExprPhraseFlag,
                                            &sTmpSubQueryExprList,
                                            &sSortColumn,
                                            aEnv )
                     == STL_SUCCESS );

            /* Inner Column으로 감싸 등록 */
            STL_TRY( qlncWrapExprInInnerColumn(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         sSortColumn,
                         (qlncNodeCommon*)aSortInstant,
                         sIdx,
                         &sSortColumn,
                         aEnv )
                     == STL_SUCCESS );

            /* Reference Expression Item 할당 */
            STL_TRY( knlAllocRegionMem(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         STL_SIZEOF( qlncRefExprItem ),
                         (void**) &sCandRefExprItem,
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sCandRefExprItem->mExpr = sSortColumn;
            sCandRefExprItem->mNext = NULL;

            /* Sort Key를 Key Column List에 추가 */
            QLNC_APPEND_ITEM_TO_LIST( aSortInstant->mKeyColList, sCandRefExprItem );

            /* SubQuery Expression List 등록 */
            if( sTmpSubQueryExprList.mCount > 0 )
            {
                if( aSortInstant->mNonFilterSubQueryExprList == NULL )
                {
                    STL_TRY( knlAllocRegionMem(
                                 &QLL_CANDIDATE_MEM( aEnv ),
                                 STL_SIZEOF( qlncRefExprList ),
                                 (void**) &(aSortInstant->mNonFilterSubQueryExprList),
                                 KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    QLNC_COPY_LIST_INFO( &sTmpSubQueryExprList,
                                         aSortInstant->mNonFilterSubQueryExprList );
                }
                else
                {
                    aSortInstant->mNonFilterSubQueryExprList->mTail->mNext =
                        sTmpSubQueryExprList.mHead;
                    aSortInstant->mNonFilterSubQueryExprList->mTail =
                        sTmpSubQueryExprList.mTail;
                    aSortInstant->mNonFilterSubQueryExprList->mCount +=
                        sTmpSubQueryExprList.mCount;
                }
            }

            sIdx++;
            sInitRefExprItem = sInitRefExprItem->mNext;
        }

        /* Sort Key Flags 등록 */
        STL_DASSERT( aKeyFlags != NULL );
        aSortInstant->mKeyFlags = aKeyFlags;
    }
    else if( aColumnType == QLNC_INSTANT_COLUMN_TYPE_RECORD )
    {
        if( aSortInstant->mRecColList == NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefExprList ),
                                        (void**) &(aSortInstant->mRecColList),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            QLNC_INIT_LIST( aSortInstant->mRecColList );
        }

        /* Column 등록 */
        sIdx = aSortInstant->mKeyColList->mCount;
        sInitRefExprItem = aColumnList->mHead;
        while( sInitRefExprItem != NULL )
        {
            STL_DASSERT( sInitRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

            QLNC_INIT_LIST( &sTmpSubQueryExprList );
            STL_TRY( qlncConvertExpression( aCreateNodeParamInfo,
                                            sInitRefExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                            aExprPhraseFlag,
                                            &sTmpSubQueryExprList,
                                            &sSortColumn,
                                            aEnv )
                     == STL_SUCCESS );

            /* Inner Column으로 감싸 등록 */
            STL_TRY( qlncWrapExprInInnerColumn(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         sSortColumn,
                         (qlncNodeCommon*)aSortInstant,
                         sIdx,
                         &sSortColumn,
                         aEnv )
                     == STL_SUCCESS );

            /* Reference Expression Item 할당 */
            STL_TRY( knlAllocRegionMem(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         STL_SIZEOF( qlncRefExprItem ),
                         (void**) &sCandRefExprItem,
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sCandRefExprItem->mExpr = sSortColumn;
            sCandRefExprItem->mNext = NULL;

            /* Sort Record를 Record Column List에 추가 */
            QLNC_APPEND_ITEM_TO_LIST( aSortInstant->mRecColList, sCandRefExprItem );

            /* SubQuery Expression List 등록 */
            if( sTmpSubQueryExprList.mCount > 0 )
            {
                if( aSortInstant->mNonFilterSubQueryExprList == NULL )
                {
                    STL_TRY( knlAllocRegionMem(
                                 &QLL_CANDIDATE_MEM( aEnv ),
                                 STL_SIZEOF( qlncRefExprList ),
                                 (void**) &(aSortInstant->mNonFilterSubQueryExprList),
                                 KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    QLNC_COPY_LIST_INFO( &sTmpSubQueryExprList,
                                         aSortInstant->mNonFilterSubQueryExprList );
                }
                else
                {
                    aSortInstant->mNonFilterSubQueryExprList->mTail->mNext =
                        sTmpSubQueryExprList.mHead;
                    aSortInstant->mNonFilterSubQueryExprList->mTail =
                        sTmpSubQueryExprList.mTail;
                    aSortInstant->mNonFilterSubQueryExprList->mCount +=
                        sTmpSubQueryExprList.mCount;
                }
            }

            sIdx++;
            sInitRefExprItem = sInitRefExprItem->mNext;
        }

        aSortInstant->mSortTableAttr.mTopDown = STL_FALSE;
        aSortInstant->mSortTableAttr.mDistinct = STL_FALSE;
    }
    else
    {
        STL_DASSERT( aColumnType == QLNC_INSTANT_COLUMN_TYPE_READ );

        if( aSortInstant->mReadColList == NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefExprList ),
                                        (void**) &(aSortInstant->mReadColList),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            QLNC_INIT_LIST( aSortInstant->mReadColList );
        }

        sInitRefExprItem = aColumnList->mHead;
        while( sInitRefExprItem != NULL )
        {
            STL_DASSERT( sInitRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

            /* Index에 대응하는 Key/Record Column 찾기 */
            sIdx = *(sInitRefExprItem->mExprPtr->mExpr.mInnerColumn->mIdx);

            if( sIdx < aSortInstant->mKeyColList->mCount )
            {
                sCandRefExprItem = aSortInstant->mKeyColList->mHead;
            }
            else
            {
                sIdx -= aSortInstant->mKeyColList->mCount;
                STL_DASSERT( sIdx < aSortInstant->mRecColList->mCount );

                sCandRefExprItem = aSortInstant->mRecColList->mHead;

                aSortInstant->mSortTableAttr.mTopDown = STL_FALSE;
                aSortInstant->mSortTableAttr.mDistinct = STL_FALSE;
            }

            for( ; sIdx > 0; sIdx-- )
            {
                sCandRefExprItem = sCandRefExprItem->mNext;
            }

            STL_DASSERT( sCandRefExprItem != NULL );
            STL_DASSERT( sCandRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );

            sSortColumn = ((qlncExprInnerColumn*)(sCandRefExprItem->mExpr))->mOrgExpr;
            sIdx = ((qlncExprInnerColumn*)(sCandRefExprItem->mExpr))->mIdx;

            /* Inner Column으로 감싸 등록 */
            STL_TRY( qlncWrapExprInInnerColumn(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         sSortColumn,
                         (qlncNodeCommon*)aSortInstant,
                         sIdx,
                         &sSortColumn,
                         aEnv )
                     == STL_SUCCESS );

            /* Reference Expression Item 할당 */
            STL_TRY( knlAllocRegionMem(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         STL_SIZEOF( qlncRefExprItem ),
                         (void**) &sCandRefExprItem,
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sCandRefExprItem->mExpr = sSortColumn;
            sCandRefExprItem->mNext = NULL;

            /* Sort Read Column을 Read Column List에 추가 */
            QLNC_APPEND_ITEM_TO_LIST( aSortInstant->mReadColList, sCandRefExprItem );

            sInitRefExprItem = sInitRefExprItem->mNext;
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Instant Node를 생성한다.
 * @param[in]       aParamInfo                  Parameter Info
 * @param[in]       aInstantType                Instant Type
 * @param[in]       aHashTableAttr              Hash Table Attribute
 * @param[in]       aScrollable                 Scroll 여부 설정
 * @param[in]       aChildNode                  Child Node
 * @param[out]      aHashInstant                Hash Instant Node
 * @param[in]       aEnv                        Environment
 */
stlStatus qlncCreateHashInstant( qlncParamInfo              * aParamInfo,
                                 qlncInstantType              aInstantType,
                                 smlIndexAttr               * aHashTableAttr,
                                 ellCursorScrollability       aScrollable,
                                 qlncNodeCommon             * aChildNode,
                                 qlncHashInstantNode       ** aHashInstant,
                                 qllEnv                     * aEnv )
{
    qlncHashInstantNode     * sHashInstantNode  = NULL;

    
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashTableAttr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Hash Instant Node 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncHashInstantNode ),
                                (void**) &sHashInstantNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sHashInstantNode, 0x00, STL_SIZEOF( qlncHashInstantNode ) );

    QLNC_INIT_NODE_COMMON( &(sHashInstantNode->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_HASH_INSTANT );
    (*(aParamInfo->mGlobalNodeID))++;  /* Global ID 증가 */

    /* Semi-Join Hint 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinHint ),
                                (void**) &(sHashInstantNode->mSemiJoinHint),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_JOIN_HINT( sHashInstantNode->mSemiJoinHint );

    /* Child Node 설정 */
    sHashInstantNode->mChildNode = aChildNode;

    /* Instant Type 설정 */
    sHashInstantNode->mInstantType = aInstantType;

    /* Hash Table Attribute 설정 */
    sHashInstantNode->mHashTableAttr = *aHashTableAttr;

    /* Scrollable 설정 */
    sHashInstantNode->mScrollable = aScrollable;


    /**
     * Output 설정
     */

    *aHashInstant = sHashInstantNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Instant Node에 Column 정보를 추가한다.
 * @param[in]       aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]       aColumnList             Column List
 * @param[in]       aColumnType             Column Type
 * @param[in]       aExprPhraseFlag         Expression Phrase Flag
 * @param[in,out]   aHashInstant            Hash Instant
 * @param[in]       aEnv                    Environment
 */
stlStatus qlncAddColumnOnHashInstant( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                                      qlvRefExprList            * aColumnList,
                                      stlInt16                    aColumnType,
                                      stlInt32                    aExprPhraseFlag,
                                      qlncHashInstantNode       * aHashInstant,
                                      qllEnv                    * aEnv )
{
    stlInt32              sIdx;
    qlncExprCommon      * sHashColumn       = NULL;
    qlvRefExprItem      * sInitRefExprItem  = NULL;
    qlncRefExprItem     * sCandRefExprItem  = NULL;
    qlncRefExprList       sTmpSubQueryExprList;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashInstant != NULL, QLL_ERROR_STACK(aEnv) );


    if( aColumnType == QLNC_INSTANT_COLUMN_TYPE_KEY )
    {
        if( aHashInstant->mKeyColList == NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefExprList ),
                                        (void**) &(aHashInstant->mKeyColList),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            QLNC_INIT_LIST( aHashInstant->mKeyColList );
        }

        /* Column 등록 */
        sIdx = 0;
        sInitRefExprItem = aColumnList->mHead;
        while( sInitRefExprItem != NULL )
        {
            STL_DASSERT( sInitRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

            QLNC_INIT_LIST( &sTmpSubQueryExprList );
            STL_TRY( qlncConvertExpression( aCreateNodeParamInfo,
                                            sInitRefExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                            aExprPhraseFlag,
                                            &sTmpSubQueryExprList,
                                            &sHashColumn,
                                            aEnv )
                     == STL_SUCCESS );

            /* Inner Column으로 감싸 등록 */
            STL_TRY( qlncWrapExprInInnerColumn(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         sHashColumn,
                         (qlncNodeCommon*)aHashInstant,
                         sIdx,
                         &sHashColumn,
                         aEnv )
                     == STL_SUCCESS );

            /* Reference Expression Item 할당 */
            STL_TRY( knlAllocRegionMem(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         STL_SIZEOF( qlncRefExprItem ),
                         (void**) &sCandRefExprItem,
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sCandRefExprItem->mExpr = sHashColumn;
            sCandRefExprItem->mNext = NULL;

            /* Hash Key를 Key Column List에 추가 */
            QLNC_APPEND_ITEM_TO_LIST( aHashInstant->mKeyColList, sCandRefExprItem );

            /* SubQuery Expression List 등록 */
            if( sTmpSubQueryExprList.mCount > 0 )
            {
                if( aHashInstant->mNonFilterSubQueryExprList == NULL )
                {
                    STL_TRY( knlAllocRegionMem(
                                 &QLL_CANDIDATE_MEM( aEnv ),
                                 STL_SIZEOF( qlncRefExprList ),
                                 (void**) &(aHashInstant->mNonFilterSubQueryExprList),
                                 KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    QLNC_COPY_LIST_INFO( &sTmpSubQueryExprList,
                                         aHashInstant->mNonFilterSubQueryExprList );
                }
                else
                {
                    aHashInstant->mNonFilterSubQueryExprList->mTail->mNext =
                        sTmpSubQueryExprList.mHead;
                    aHashInstant->mNonFilterSubQueryExprList->mTail =
                        sTmpSubQueryExprList.mTail;
                    aHashInstant->mNonFilterSubQueryExprList->mCount +=
                        sTmpSubQueryExprList.mCount;
                }
            }

            sIdx++;
            sInitRefExprItem = sInitRefExprItem->mNext;
        }
    }
    else if( aColumnType == QLNC_INSTANT_COLUMN_TYPE_RECORD )
    {
        if( aHashInstant->mRecColList == NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefExprList ),
                                        (void**) &(aHashInstant->mRecColList),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            QLNC_INIT_LIST( aHashInstant->mRecColList );
        }

        /* Column 등록 */
        sIdx = aHashInstant->mKeyColList->mCount;
        sInitRefExprItem = aColumnList->mHead;
        while( sInitRefExprItem != NULL )
        {
            STL_DASSERT( sInitRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

            QLNC_INIT_LIST( &sTmpSubQueryExprList );
            STL_TRY( qlncConvertExpression( aCreateNodeParamInfo,
                                            sInitRefExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                            aExprPhraseFlag,
                                            &sTmpSubQueryExprList,
                                            &sHashColumn,
                                            aEnv )
                     == STL_SUCCESS );

            /* Inner Column으로 감싸 등록 */
            STL_TRY( qlncWrapExprInInnerColumn(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         sHashColumn,
                         (qlncNodeCommon*)aHashInstant,
                         sIdx,
                         &sHashColumn,
                         aEnv )
                     == STL_SUCCESS );

            /* Reference Expression Item 할당 */
            STL_TRY( knlAllocRegionMem(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         STL_SIZEOF( qlncRefExprItem ),
                         (void**) &sCandRefExprItem,
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sCandRefExprItem->mExpr = sHashColumn;
            sCandRefExprItem->mNext = NULL;

            /* Hash Record를 Record Column List에 추가 */
            QLNC_APPEND_ITEM_TO_LIST( aHashInstant->mRecColList, sCandRefExprItem );

            /* SubQuery Expression List 등록 */
            if( sTmpSubQueryExprList.mCount > 0 )
            {
                if( aHashInstant->mNonFilterSubQueryExprList == NULL )
                {
                    STL_TRY( knlAllocRegionMem(
                                 &QLL_CANDIDATE_MEM( aEnv ),
                                 STL_SIZEOF( qlncRefExprList ),
                                 (void**) &(aHashInstant->mNonFilterSubQueryExprList),
                                 KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    QLNC_COPY_LIST_INFO( &sTmpSubQueryExprList,
                                         aHashInstant->mNonFilterSubQueryExprList );
                }
                else
                {
                    aHashInstant->mNonFilterSubQueryExprList->mTail->mNext =
                        sTmpSubQueryExprList.mHead;
                    aHashInstant->mNonFilterSubQueryExprList->mTail =
                        sTmpSubQueryExprList.mTail;
                    aHashInstant->mNonFilterSubQueryExprList->mCount +=
                        sTmpSubQueryExprList.mCount;
                }
            }

            sIdx++;
            sInitRefExprItem = sInitRefExprItem->mNext;
        }

        aHashInstant->mHashTableAttr.mUniquenessFlag = STL_FALSE;
    }
    else
    {
        STL_DASSERT( aColumnType == QLNC_INSTANT_COLUMN_TYPE_READ );

        if( aHashInstant->mReadColList == NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefExprList ),
                                        (void**) &(aHashInstant->mReadColList),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            QLNC_INIT_LIST( aHashInstant->mReadColList );
        }

        sInitRefExprItem = aColumnList->mHead;
        while( sInitRefExprItem != NULL )
        {
            STL_DASSERT( sInitRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

            /* Index에 대응하는 Key/Record Column 찾기 */
            sIdx = *(sInitRefExprItem->mExprPtr->mExpr.mInnerColumn->mIdx);

            if( sIdx < aHashInstant->mKeyColList->mCount )
            {
                sCandRefExprItem = aHashInstant->mKeyColList->mHead;
            }
            else
            {
                sIdx -= aHashInstant->mKeyColList->mCount;
                STL_DASSERT( sIdx < aHashInstant->mRecColList->mCount );

                sCandRefExprItem = aHashInstant->mRecColList->mHead;

                aHashInstant->mHashTableAttr.mUniquenessFlag = STL_FALSE;
            }

            for( ; sIdx > 0; sIdx-- )
            {
                sCandRefExprItem = sCandRefExprItem->mNext;
            }

            STL_DASSERT( sCandRefExprItem != NULL );
            STL_DASSERT( sCandRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );

            sHashColumn = ((qlncExprInnerColumn*)(sCandRefExprItem->mExpr))->mOrgExpr;
            sIdx = ((qlncExprInnerColumn*)(sCandRefExprItem->mExpr))->mIdx;

            /* Inner Column으로 감싸 등록 */
            STL_TRY( qlncWrapExprInInnerColumn(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         sHashColumn,
                         (qlncNodeCommon*)aHashInstant,
                         sIdx,
                         &sHashColumn,
                         aEnv )
                     == STL_SUCCESS );

            /* Reference Expression Item 할당 */
            STL_TRY( knlAllocRegionMem(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         STL_SIZEOF( qlncRefExprItem ),
                         (void**) &sCandRefExprItem,
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sCandRefExprItem->mExpr = sHashColumn;
            sCandRefExprItem->mNext = NULL;

            /* Hash Read Column을 Read Column List에 추가 */
            QLNC_APPEND_ITEM_TO_LIST( aHashInstant->mReadColList, sCandRefExprItem );

            sInitRefExprItem = sInitRefExprItem->mNext;
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Instant에 Filter를 등록한다.
 * @param[in,out]   aHashInstant    Hash Instant
 * @param[in]       aFilter         And Filter
 */
void qlncSetFilterOnHashInstant( qlncHashInstantNode    * aHashInstant,
                                 qlncAndFilter          * aFilter )
{
    STL_DASSERT( aHashInstant != NULL );

    aHashInstant->mRecFilter = aFilter;
}


/**
 * @brief Group By를 위한 Node를 생성한다.
 * @param[in]   aCreateNodeParamInfo    Parameter Info
 * @param[in]   aGroupBy                Group By 정보
 * @param[in]   aChildNode              Child Candidate Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCreateGroupByCandNode( qlncCreateNodeParamInfo    * aCreateNodeParamInfo,
                                     qlvGroupBy                 * aGroupBy,
                                     qlncNodeCommon             * aChildNode,
                                     qllEnv                     * aEnv )
{
    smlIndexAttr              sHashTableAttr;

    qlncHashInstantNode     * sHashInstant      = NULL;
    qlncAndFilter           * sHaving           = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGroupBy != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Group By Type에 따라 Node 생성 */
    switch( aGroupBy->mGroupByType )
    {
        case QLP_GROUPING_TYPE_ORDINARY:
            STL_DASSERT( (aGroupBy->mInstant.mType == QLV_NODE_TYPE_GROUP_HASH_INSTANT) ||
                         (aGroupBy->mInstant.mType == QLV_NODE_TYPE_GROUP_SORT_INSTANT) );

            /* Hash Table Attribute 설정 */
            stlMemset( &sHashTableAttr, 0x00, STL_SIZEOF( smlIndexAttr ) );

            /* Hash Instant 생성 */
            STL_TRY( qlncCreateHashInstant( &(aCreateNodeParamInfo->mParamInfo),
                                            QLNC_INSTANT_TYPE_GROUP,
                                            &sHashTableAttr,
                                            ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                            aChildNode,
                                            &sHashInstant,
                                            aEnv )
                     == STL_SUCCESS );

            /**
             * Table Map Array에 추가
             */

            STL_TRY( qlncAddTableMapToTableMapArray( aCreateNodeParamInfo->mTableMapArr,
                                                     (qlvInitNode*)(&(aGroupBy->mInstant)),
                                                     (qlncNodeCommon*)sHashInstant,
                                                     aEnv )
                     == STL_SUCCESS );

            /* Hash Key 설정 */
            STL_DASSERT( aGroupBy->mInstant.mKeyColCnt > 0 );

            STL_TRY( qlncAddColumnOnHashInstant( aCreateNodeParamInfo,
                                                 aGroupBy->mInstant.mKeyColList,
                                                 QLNC_INSTANT_COLUMN_TYPE_KEY,
                                                 QLNC_EXPR_PHRASE_GROUPBY,
                                                 sHashInstant,
                                                 aEnv )
                     == STL_SUCCESS );

            /* Hash Record 설정 */
            if( aGroupBy->mInstant.mRecColCnt > 0 )
            {
                STL_TRY( qlncAddColumnOnHashInstant( aCreateNodeParamInfo,
                                                     aGroupBy->mInstant.mRecColList,
                                                     QLNC_INSTANT_COLUMN_TYPE_RECORD,
                                                     QLNC_EXPR_PHRASE_GROUPBY,
                                                     sHashInstant,
                                                     aEnv )
                         == STL_SUCCESS );
            }

            /* Hash Read Column 설정 */
            if( aGroupBy->mInstant.mReadColCnt > 0 )
            {
                STL_TRY( qlncAddColumnOnHashInstant( aCreateNodeParamInfo,
                                                     aGroupBy->mInstant.mReadColList,
                                                     QLNC_INSTANT_COLUMN_TYPE_READ,
                                                     QLNC_EXPR_PHRASE_GROUPBY,
                                                     sHashInstant,
                                                     aEnv )
                         == STL_SUCCESS );
            }

            /* Having 설정 */
            if( aGroupBy->mHaving != NULL )
            {
                STL_TRY( qlncConvertCondition( aCreateNodeParamInfo,
                                               aGroupBy->mHaving,
                                               QLNC_EXPR_PHRASE_HAVING,
                                               &sHaving,
                                               aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                sHaving = NULL;
            }

            /* Filter 설정 */
            (void)qlncSetFilterOnHashInstant( sHashInstant, sHaving );

            aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sHashInstant;

            break;

        case QLP_GROUPING_TYPE_GROUPING_SET:
        case QLP_GROUPING_TYPE_EMPTY:
        case QLP_GROUPING_TYPE_ROLL_UP:
        case QLP_GROUPING_TYPE_CUBE:
        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Distinct를 위한 Node를 생성한다.
 * @param[in]   aCreateNodeParamInfo    Parameter Info
 * @param[in]   aDistinct               Distinct 정보
 * @param[in]   aChildNode              Child Candidate Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCreateDistinctCandNode( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                                      qlvInitInstantNode        * aDistinct,
                                      qlncNodeCommon            * aChildNode,
                                      qllEnv                    * aEnv )
{
    smlIndexAttr              sHashTableAttr;
    qlncHashInstantNode     * sHashInstant      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDistinct != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildNode != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aDistinct->mType == QLV_NODE_TYPE_GROUP_HASH_INSTANT );

    /* Hash Table Attribute 설정 */
    stlMemset( &sHashTableAttr, 0x00, STL_SIZEOF( smlIndexAttr ) );

    /* Hash Instant 생성 */
    STL_TRY( qlncCreateHashInstant( &(aCreateNodeParamInfo->mParamInfo),
                                    QLNC_INSTANT_TYPE_DISTINCT,
                                    &sHashTableAttr,
                                    ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                    aChildNode,
                                    &sHashInstant,
                                    aEnv )
             == STL_SUCCESS );


    /**
     * Table Map Array에 추가
     */

    STL_TRY( qlncAddTableMapToTableMapArray( aCreateNodeParamInfo->mTableMapArr,
                                             (qlvInitNode*)aDistinct,
                                             (qlncNodeCommon*)sHashInstant,
                                             aEnv )
             == STL_SUCCESS );


    /* Hash Key 설정 */
    STL_DASSERT( aDistinct->mKeyColCnt > 0 );

    STL_TRY( qlncAddColumnOnHashInstant( aCreateNodeParamInfo,
                                         aDistinct->mKeyColList,
                                         QLNC_INSTANT_COLUMN_TYPE_KEY,
                                         QLNC_EXPR_PHRASE_TARGET,
                                         sHashInstant,
                                         aEnv )
             == STL_SUCCESS );

    /* Hash Record 설정 */
    /* Distinct는 모든 column이 key column으로 구성된다. */
    STL_DASSERT( aDistinct->mRecColCnt == 0 );

    /* Hash Read Column 설정 */
    if( aDistinct->mReadColCnt > 0 )
    {
        STL_TRY( qlncAddColumnOnHashInstant( aCreateNodeParamInfo,
                                             aDistinct->mReadColList,
                                             QLNC_INSTANT_COLUMN_TYPE_READ,
                                             QLNC_EXPR_PHRASE_TARGET,
                                             sHashInstant,
                                             aEnv )
                 == STL_SUCCESS );
    }

            
    /**
     * Output 설정
     */

    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sHashInstant;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Order By를 위한 Node를 생성한다.
 * @param[in]   aCreateNodeParamInfo    Parameter Info
 * @param[in]   aOrderBy                Order By 정보
 * @param[in]   aChildNode              Child Candidate Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCreateOrderByCandNode( qlncCreateNodeParamInfo    * aCreateNodeParamInfo,
                                     qlvOrderBy                 * aOrderBy,
                                     qlncNodeCommon             * aChildNode,
                                     qllEnv                     * aEnv )
{
    smlSortTableAttr          sSortTableAttr;
    qlncSortInstantNode     * sSortInstant      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrderBy != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildNode != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aOrderBy->mInstant.mType == QLV_NODE_TYPE_SORT_INSTANT );

    /* Sort Table Attribute 설정 */
    sSortTableAttr.mTopDown      = STL_FALSE;    /* bottom-up */
    sSortTableAttr.mVolatile     = STL_FALSE;    /* not volatile */
    sSortTableAttr.mLeafOnly     = STL_TRUE;     /* leaf-only */
    sSortTableAttr.mDistinct     = STL_FALSE;    /* non-distinct */
    sSortTableAttr.mNullExcluded = STL_FALSE;    /* null skip */

    /* Sort Instant 생성 */
    STL_TRY( qlncCreateSortInstant( &(aCreateNodeParamInfo->mParamInfo),
                                    QLNC_INSTANT_TYPE_ORDER,
                                    &sSortTableAttr,
                                    ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                    aChildNode,
                                    &sSortInstant,
                                    aEnv )
             == STL_SUCCESS );


    /**
     * Table Map Array에 추가
     */

    STL_TRY( qlncAddTableMapToTableMapArray( aCreateNodeParamInfo->mTableMapArr,
                                             (qlvInitNode*)(&(aOrderBy->mInstant)),
                                             (qlncNodeCommon*)sSortInstant,
                                             aEnv )
             == STL_SUCCESS );


    /* Sort Key 설정 */
    STL_DASSERT( aOrderBy->mInstant.mKeyColCnt > 0 );

    STL_TRY( qlncAddColumnOnSortInstant( aCreateNodeParamInfo,
                                         aOrderBy->mInstant.mKeyColList,
                                         aOrderBy->mInstant.mKeyFlags,
                                         QLNC_INSTANT_COLUMN_TYPE_KEY,
                                         QLNC_EXPR_PHRASE_ORDERBY,
                                         sSortInstant,
                                         aEnv )
             == STL_SUCCESS );

    /* Sort Record 설정 */
    if( aOrderBy->mInstant.mRecColCnt > 0 )
    {
        STL_TRY( qlncAddColumnOnSortInstant( aCreateNodeParamInfo,
                                             aOrderBy->mInstant.mRecColList,
                                             NULL,
                                             QLNC_INSTANT_COLUMN_TYPE_RECORD,
                                             QLNC_EXPR_PHRASE_ORDERBY,
                                             sSortInstant,
                                             aEnv )
                 == STL_SUCCESS );
    }

    /* Sort Read Column 설정 */
    if( aOrderBy->mInstant.mReadColCnt > 0 )
    {
        STL_TRY( qlncAddColumnOnSortInstant( aCreateNodeParamInfo,
                                             aOrderBy->mInstant.mReadColList,
                                             NULL,
                                             QLNC_INSTANT_COLUMN_TYPE_READ,
                                             QLNC_EXPR_PHRASE_ORDERBY,
                                             sSortInstant,
                                             aEnv )
                 == STL_SUCCESS );
    }

  
    /**
     * Output 설정
     */

    aCreateNodeParamInfo->mCandNode = (qlncNodeCommon*)sSortInstant;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 새로운 Base Table Node를 생성하고 Cost Base Optimizer 정보를 제외한 Node 정보를 Copy 한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aOrgNode        Original Node
 * @param[out]  aCopiedNode     Copied Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCopyBaseNodeWithoutCBOInfo( qlncParamInfo     * aParamInfo,
                                          qlncNodeCommon    * aOrgNode,
                                          qlncNodeCommon   ** aCopiedNode,
                                          qllEnv            * aEnv )
{
    qlncBaseTableNode   * sBaseTableNode    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrgNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Base Table Node 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncBaseTableNode ),
                                (void**) &sBaseTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Base Table Node 내용 복사 */
    QLNC_INIT_NODE_COMMON( &(sBaseTableNode->mNodeCommon),
                           aOrgNode->mNodeID,
                           QLNC_NODE_TYPE_BASE_TABLE );
    sBaseTableNode->mTableInfo = ((qlncBaseTableNode*)aOrgNode)->mTableInfo;

    sBaseTableNode->mTableStat = ((qlncBaseTableNode*)aOrgNode)->mTableStat;
    sBaseTableNode->mIndexCount = ((qlncBaseTableNode*)aOrgNode)->mIndexCount;
    sBaseTableNode->mIndexStatArr = ((qlncBaseTableNode*)aOrgNode)->mIndexStatArr;

    sBaseTableNode->mAccPathHint = ((qlncBaseTableNode*)aOrgNode)->mAccPathHint;
    sBaseTableNode->mJoinHint = ((qlncBaseTableNode*)aOrgNode)->mJoinHint;

    if( ((qlncBaseTableNode*)aOrgNode)->mFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( aParamInfo,
                                    ((qlncBaseTableNode*)aOrgNode)->mFilter,
                                    &(sBaseTableNode->mFilter),
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sBaseTableNode->mFilter = NULL;
    }
    QLNC_COPY_LIST_INFO( &(((qlncBaseTableNode*)aOrgNode)->mReadColumnList),
                         &(sBaseTableNode->mReadColumnList) );

    /* Cost 관련 정보 초기화 */
    sBaseTableNode->mTableScanCost = NULL;
    sBaseTableNode->mIndexScanCostCount = 0;
    sBaseTableNode->mIndexScanCostArr = NULL;
    sBaseTableNode->mIndexCombineCost = NULL;
    sBaseTableNode->mRowidScanCost = NULL;


    /**
     * Output 설정
     */

    *aCopiedNode = (qlncNodeCommon*)sBaseTableNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 새로운 Sub Table Node를 생성하고 Cost Base Optimizer 정보를 제외한 Node 정보를 Copy 한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aOrgNode        Original Node
 * @param[out]  aCopiedNode     Copied Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCopySubNodeWithoutCBOInfo( qlncParamInfo      * aParamInfo,
                                         qlncNodeCommon     * aOrgNode,
                                         qlncNodeCommon    ** aCopiedNode,
                                         qllEnv             * aEnv )
{
    qlncSubTableNode    * sSubTableNode     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrgNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrgNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE, QLL_ERROR_STACK(aEnv) );


    /* Sub Table Node 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncSubTableNode ),
                                (void**) &sSubTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Sub Table Node 내용 복사 */
    QLNC_INIT_NODE_COMMON( &(sSubTableNode->mNodeCommon),
                           aOrgNode->mNodeID,
                           QLNC_NODE_TYPE_SUB_TABLE );
    sSubTableNode->mRelationName = ((qlncSubTableNode*)aOrgNode)->mRelationName;
    sSubTableNode->mQueryNode = ((qlncSubTableNode*)aOrgNode)->mQueryNode;

    sSubTableNode->mJoinHint = ((qlncSubTableNode*)aOrgNode)->mJoinHint;

    sSubTableNode->mHasHandle = ((qlncSubTableNode*)aOrgNode)->mHasHandle;
    stlMemcpy( &(sSubTableNode->mSchemaHandle),
               &(((qlncSubTableNode*)aOrgNode)->mSchemaHandle),
               STL_SIZEOF( ellDictHandle ) );
    stlMemcpy( &(sSubTableNode->mViewHandle),
               &(((qlncSubTableNode*)aOrgNode)->mViewHandle),
               STL_SIZEOF( ellDictHandle ) );

    if( ((qlncSubTableNode*)aOrgNode)->mFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( aParamInfo,
                                    ((qlncSubTableNode*)aOrgNode)->mFilter,
                                    &(sSubTableNode->mFilter),
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sSubTableNode->mFilter = NULL;
    }

    QLNC_COPY_LIST_INFO( &(((qlncSubTableNode*)aOrgNode)->mReadColumnList),
                         &(sSubTableNode->mReadColumnList) );

    sSubTableNode->mFilterCost = NULL;


    /**
     * Output 설정
     */

    *aCopiedNode = (qlncNodeCommon*)sSubTableNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 새로운 Join Table Node를 생성하고 Cost Base Optimizer 정보를 제외한 Node 정보를 Copy 한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aOrgNode        Original Node
 * @param[out]  aCopiedNode     Copied Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCopyJoinNodeWithoutCBOInfo( qlncParamInfo     * aParamInfo,
                                          qlncNodeCommon    * aOrgNode,
                                          qlncNodeCommon   ** aCopiedNode,
                                          qllEnv            * aEnv )
{
    stlInt32              i;
    qlncJoinTableNode   * sJoinTableNode    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrgNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Join Table Node 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinTableNode ),
                                (void**) &sJoinTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Join Table Node 내용 복사 */
    QLNC_INIT_NODE_COMMON( &(sJoinTableNode->mNodeCommon),
                           aOrgNode->mNodeID,
                           QLNC_NODE_TYPE_JOIN_TABLE );

    STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                      &(sJoinTableNode->mNodeArray),
                                      aEnv )
             == STL_SUCCESS );

    for( i = 0; i < ((qlncJoinTableNode*)aOrgNode)->mNodeArray.mCurCnt; i++ )
    {
        STL_TRY( qlncAddNodeToNodeArray( &(sJoinTableNode->mNodeArray),
                                         ((qlncJoinTableNode*)aOrgNode)->mNodeArray.mNodeArr[i],
                                         aEnv )
                 == STL_SUCCESS );
    }

    if( ((qlncJoinTableNode*)aOrgNode)->mJoinCond != NULL )
    {
        STL_TRY( qlncCopyAndFilter( aParamInfo,
                                    ((qlncJoinTableNode*)aOrgNode)->mJoinCond,
                                    &(sJoinTableNode->mJoinCond),
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sJoinTableNode->mJoinCond = NULL;
    }

    if( ((qlncJoinTableNode*)aOrgNode)->mFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( aParamInfo,
                                    ((qlncJoinTableNode*)aOrgNode)->mFilter,
                                    &(sJoinTableNode->mFilter),
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sJoinTableNode->mFilter = NULL;
    }

    sJoinTableNode->mJoinType = ((qlncJoinTableNode*)aOrgNode)->mJoinType;
    sJoinTableNode->mJoinDirect = ((qlncJoinTableNode*)aOrgNode)->mJoinDirect;
    sJoinTableNode->mNamedColumnList = ((qlncJoinTableNode*)aOrgNode)->mNamedColumnList;
    sJoinTableNode->mSemiJoinHint = ((qlncJoinTableNode*)aOrgNode)->mSemiJoinHint;

    /* Cost 관련 정보 초기화 */
    sJoinTableNode->mBestInnerJoinTable = NULL;
    sJoinTableNode->mBestOuterJoinTable = NULL;
    sJoinTableNode->mBestSemiJoinTable = NULL;
    sJoinTableNode->mBestAntiSemiJoinTable = NULL;


    /**
     * Output 설정
     */

    *aCopiedNode = (qlncNodeCommon*)sJoinTableNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Org Node를 포함한 Child Node들에 대하여 새로운 Node를 생성하고 Node 정보를 Copy 한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aOrgNode        Original Node
 * @param[out]  aCopiedNode     Copied Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCopyAllNodeWithoutCBOInfo( qlncParamInfo      * aParamInfo,
                                         qlncNodeCommon     * aOrgNode,
                                         qlncNodeCommon    ** aCopiedNode,
                                         qllEnv             * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrgNode != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aOrgNode->mNodeType )
    {
        case QLNC_NODE_TYPE_QUERY_SET:
            {
                qlncQuerySet    * sQuerySet = NULL;

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncQuerySet ),
                                            (void**) &sQuerySet,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                QLNC_INIT_NODE_COMMON( &(sQuerySet->mNodeCommon),
                                       aOrgNode->mNodeID,
                                       QLNC_NODE_TYPE_QUERY_SET );

                sQuerySet->mTableMapArr = NULL;
                sQuerySet->mIsRootSet = ((qlncQuerySet*)aOrgNode)->mIsRootSet;
                sQuerySet->mQBID = ((qlncQuerySet*)aOrgNode)->mQBID;
                sQuerySet->mSetTargetCount = ((qlncQuerySet*)aOrgNode)->mSetTargetCount;
                sQuerySet->mSetTargetArr = ((qlncQuerySet*)aOrgNode)->mSetTargetArr;
                sQuerySet->mSetColumnArr = ((qlncQuerySet*)aOrgNode)->mSetColumnArr;

                STL_TRY( qlncCopyAllNodeWithoutCBOInfo( aParamInfo,
                                                        ((qlncQuerySet*)aOrgNode)->mChildNode,
                                                        &(sQuerySet->mChildNode),
                                                        aEnv )
                         == STL_SUCCESS );

                sQuerySet->mOffset = ((qlncQuerySet*)aOrgNode)->mOffset;
                sQuerySet->mLimit = ((qlncQuerySet*)aOrgNode)->mLimit;
                sQuerySet->mQuerySetCost = NULL;

                *aCopiedNode = (qlncNodeCommon*)sQuerySet;

                break;
            }
        case QLNC_NODE_TYPE_SET_OP:
            {
                stlInt32      i;
                qlncSetOP   * sSetOP    = NULL;

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncSetOP ),
                                            (void**) &sSetOP,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                QLNC_INIT_NODE_COMMON( &(sSetOP->mNodeCommon),
                                       aOrgNode->mNodeID,
                                       QLNC_NODE_TYPE_SET_OP );

                sSetOP->mSetType = ((qlncSetOP*)aOrgNode)->mSetType;
                sSetOP->mSetOption = ((qlncSetOP*)aOrgNode)->mSetOption;
                sSetOP->mChildCount = ((qlncSetOP*)aOrgNode)->mChildCount;
                sSetOP->mCurChildIdx = ((qlncSetOP*)aOrgNode)->mCurChildIdx;

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncNodeCommon* ) * sSetOP->mChildCount,
                                            (void**) &(sSetOP->mChildNodeArr),
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                for( i = 0; i < sSetOP->mChildCount; i++ )
                {
                    STL_TRY( qlncCopyAllNodeWithoutCBOInfo( aParamInfo,
                                                            ((qlncSetOP*)aOrgNode)->mChildNodeArr[i],
                                                            &(sSetOP->mChildNodeArr[i]),
                                                            aEnv )
                             == STL_SUCCESS );
                }

                sSetOP->mSetOPCost = NULL;

                *aCopiedNode = (qlncNodeCommon*)sSetOP;

                break;
            }
        case QLNC_NODE_TYPE_QUERY_SPEC:
            {
                stlInt32          i;
                qlncQuerySpec   * sQuerySpec    = NULL;

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncQuerySpec ),
                                            (void**) &sQuerySpec,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                QLNC_INIT_NODE_COMMON( &(sQuerySpec->mNodeCommon),
                                       aOrgNode->mNodeID,
                                       QLNC_NODE_TYPE_QUERY_SPEC );

                sQuerySpec->mTableMapArr = NULL;
                sQuerySpec->mQBID = ((qlncQuerySpec*)aOrgNode)->mQBID;

                sQuerySpec->mQueryTransformHint = ((qlncQuerySpec*)aOrgNode)->mQueryTransformHint;
                sQuerySpec->mOtherHint = ((qlncQuerySpec*)aOrgNode)->mOtherHint;

                sQuerySpec->mIsDistinct = ((qlncQuerySpec*)aOrgNode)->mIsDistinct;

                /* Target은 Heuristic Query Transformation에서 변경될 수 있으므로 반드시 복사한다. */
                sQuerySpec->mTargetCount = ((qlncQuerySpec*)aOrgNode)->mTargetCount;

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncTarget ) * sQuerySpec->mTargetCount,
                                            (void**) &(sQuerySpec->mTargetArr),
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sQuerySpec->mTargetArr,
                           ((qlncQuerySpec*)aOrgNode)->mTargetArr,
                           STL_SIZEOF( qlncTarget ) * sQuerySpec->mTargetCount );

                for( i = 0; i < sQuerySpec->mTargetCount; i++ )
                {
                    STL_TRY( qlncDuplicateExpr( aParamInfo,
                                                ((qlncQuerySpec*)aOrgNode)->mTargetArr[i].mExpr,
                                                &(sQuerySpec->mTargetArr[i].mExpr),
                                                aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncCopyAllNodeWithoutCBOInfo( aParamInfo,
                                                        ((qlncQuerySpec*)aOrgNode)->mChildNode,
                                                        &(sQuerySpec->mChildNode),
                                                        aEnv )
                         == STL_SUCCESS );

                sQuerySpec->mOffset = ((qlncQuerySpec*)aOrgNode)->mOffset;
                sQuerySpec->mLimit = ((qlncQuerySpec*)aOrgNode)->mLimit;
                sQuerySpec->mNonFilterSubQueryExprList =
                    ((qlncQuerySpec*)aOrgNode)->mNonFilterSubQueryExprList;
                sQuerySpec->mQuerySpecCost = NULL;

                *aCopiedNode = (qlncNodeCommon*)sQuerySpec;

                break;
            }
        case QLNC_NODE_TYPE_BASE_TABLE:
            {
                qlncBaseTableNode   * sBaseTableNode    = NULL;

                /* Base Table Node 할당 */
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncBaseTableNode ),
                                            (void**) &sBaseTableNode,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                /* Base Table Node 내용 복사 */
                QLNC_INIT_NODE_COMMON( &(sBaseTableNode->mNodeCommon),
                                       aOrgNode->mNodeID,
                                       QLNC_NODE_TYPE_BASE_TABLE );
                sBaseTableNode->mTableInfo = ((qlncBaseTableNode*)aOrgNode)->mTableInfo;

                sBaseTableNode->mTableStat = ((qlncBaseTableNode*)aOrgNode)->mTableStat;
                sBaseTableNode->mIndexCount = ((qlncBaseTableNode*)aOrgNode)->mIndexCount;
                sBaseTableNode->mIndexStatArr = ((qlncBaseTableNode*)aOrgNode)->mIndexStatArr;

                sBaseTableNode->mAccPathHint = ((qlncBaseTableNode*)aOrgNode)->mAccPathHint;
                sBaseTableNode->mJoinHint = ((qlncBaseTableNode*)aOrgNode)->mJoinHint;

                if( ((qlncBaseTableNode*)aOrgNode)->mFilter != NULL )
                {
                    STL_TRY( qlncCopyAndFilter( aParamInfo,
                                                ((qlncBaseTableNode*)aOrgNode)->mFilter,
                                                &(sBaseTableNode->mFilter),
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    sBaseTableNode->mFilter = NULL;
                }
                QLNC_COPY_LIST_INFO( &(((qlncBaseTableNode*)aOrgNode)->mReadColumnList),
                                     &(sBaseTableNode->mReadColumnList) );

                /* Cost 관련 정보 초기화 */
                sBaseTableNode->mTableScanCost = NULL;
                sBaseTableNode->mIndexScanCostCount = 0;
                sBaseTableNode->mIndexScanCostArr = NULL;
                sBaseTableNode->mIndexCombineCost = NULL;
                sBaseTableNode->mRowidScanCost = NULL;

                *aCopiedNode = (qlncNodeCommon*)sBaseTableNode;

                break;
            }
        case QLNC_NODE_TYPE_SUB_TABLE:
            {
                qlncSubTableNode    * sSubTableNode = NULL;

                /* Sub Table Node 할당 */
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncSubTableNode ),
                                            (void**) &sSubTableNode,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                /* Sub Table Node 내용 복사 */
                QLNC_INIT_NODE_COMMON( &(sSubTableNode->mNodeCommon),
                                       aOrgNode->mNodeID,
                                       QLNC_NODE_TYPE_SUB_TABLE );
                sSubTableNode->mRelationName = ((qlncSubTableNode*)aOrgNode)->mRelationName;

                STL_TRY( qlncCopyAllNodeWithoutCBOInfo( aParamInfo,
                                                        ((qlncSubTableNode*)aOrgNode)->mQueryNode,
                                                        &(sSubTableNode->mQueryNode),
                                                        aEnv )
                         == STL_SUCCESS );

                sSubTableNode->mJoinHint = ((qlncSubTableNode*)aOrgNode)->mJoinHint;

                sSubTableNode->mHasHandle = ((qlncSubTableNode*)aOrgNode)->mHasHandle;
                stlMemcpy( &(sSubTableNode->mSchemaHandle),
                           &(((qlncSubTableNode*)aOrgNode)->mSchemaHandle),
                           STL_SIZEOF( ellDictHandle ) );
                stlMemcpy( &(sSubTableNode->mViewHandle),
                           &(((qlncSubTableNode*)aOrgNode)->mViewHandle),
                           STL_SIZEOF( ellDictHandle ) );

                if( ((qlncSubTableNode*)aOrgNode)->mFilter != NULL )
                {
                    STL_TRY( qlncCopyAndFilter( aParamInfo,
                                                ((qlncSubTableNode*)aOrgNode)->mFilter,
                                                &(sSubTableNode->mFilter),
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    sSubTableNode->mFilter = NULL;
                }

                QLNC_COPY_LIST_INFO( &(((qlncSubTableNode*)aOrgNode)->mReadColumnList),
                                     &(sSubTableNode->mReadColumnList) );

                sSubTableNode->mFilterCost = NULL;

                *aCopiedNode = (qlncNodeCommon*)sSubTableNode;

                break;
            }
        case QLNC_NODE_TYPE_JOIN_TABLE:
            {
                stlInt32              i;
                qlncJoinTableNode   * sJoinTableNode    = NULL;
                qlncNodeCommon      * sTmpNode          = NULL;

                /* Join Table Node 할당 */
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncJoinTableNode ),
                                            (void**) &sJoinTableNode,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                /* Join Table Node 내용 복사 */
                QLNC_INIT_NODE_COMMON( &(sJoinTableNode->mNodeCommon),
                                       aOrgNode->mNodeID,
                                       QLNC_NODE_TYPE_JOIN_TABLE );

                STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                                  &(sJoinTableNode->mNodeArray),
                                                  aEnv )
                         == STL_SUCCESS );

                for( i = 0; i < ((qlncJoinTableNode*)aOrgNode)->mNodeArray.mCurCnt; i++ )
                {
                    STL_TRY( qlncCopyAllNodeWithoutCBOInfo(
                                 aParamInfo,
                                 ((qlncJoinTableNode*)aOrgNode)->mNodeArray.mNodeArr[i],
                                 &sTmpNode,
                                 aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlncAddNodeToNodeArray( &(sJoinTableNode->mNodeArray),
                                                     sTmpNode,
                                                     aEnv )
                             == STL_SUCCESS );
                }

                if( ((qlncJoinTableNode*)aOrgNode)->mJoinCond != NULL )
                {
                    STL_TRY( qlncCopyAndFilter( aParamInfo,
                                                ((qlncJoinTableNode*)aOrgNode)->mJoinCond,
                                                &(sJoinTableNode->mJoinCond),
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    sJoinTableNode->mJoinCond = NULL;
                }

                if( ((qlncJoinTableNode*)aOrgNode)->mFilter != NULL )
                {
                    STL_TRY( qlncCopyAndFilter( aParamInfo,
                                                ((qlncJoinTableNode*)aOrgNode)->mFilter,
                                                &(sJoinTableNode->mFilter),
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    sJoinTableNode->mFilter = NULL;
                }

                sJoinTableNode->mJoinType = ((qlncJoinTableNode*)aOrgNode)->mJoinType;
                sJoinTableNode->mJoinDirect = ((qlncJoinTableNode*)aOrgNode)->mJoinDirect;
                sJoinTableNode->mNamedColumnList = ((qlncJoinTableNode*)aOrgNode)->mNamedColumnList;
                sJoinTableNode->mSemiJoinHint = ((qlncJoinTableNode*)aOrgNode)->mSemiJoinHint;

                /* Cost 관련 정보 초기화 */
                sJoinTableNode->mBestInnerJoinTable = NULL;
                sJoinTableNode->mBestOuterJoinTable = NULL;
                sJoinTableNode->mBestSemiJoinTable = NULL;
                sJoinTableNode->mBestAntiSemiJoinTable = NULL;

                *aCopiedNode = (qlncNodeCommon*)sJoinTableNode;

                break;
            }
        case QLNC_NODE_TYPE_FLAT_INSTANT:
            {
                STL_DASSERT( 0 );
                break;
            }
        case QLNC_NODE_TYPE_SORT_INSTANT:
            {
                qlncSortInstantNode * sSortInstantNode  = NULL;

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncSortInstantNode ),
                                            (void**) &sSortInstantNode,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                /* Sort Instant Node 내용 복사 */
                QLNC_INIT_NODE_COMMON( &(sSortInstantNode->mNodeCommon),
                                       aOrgNode->mNodeID,
                                       QLNC_NODE_TYPE_SORT_INSTANT );

                sSortInstantNode->mInstantType = ((qlncSortInstantNode*)aOrgNode)->mInstantType;
                sSortInstantNode->mSortTableAttr = ((qlncSortInstantNode*)aOrgNode)->mSortTableAttr;
                sSortInstantNode->mScrollable = ((qlncSortInstantNode*)aOrgNode)->mScrollable;

                STL_TRY( qlncCopyAllNodeWithoutCBOInfo( aParamInfo,
                                                        ((qlncSortInstantNode*)aOrgNode)->mChildNode,
                                                        &(sSortInstantNode->mChildNode),
                                                        aEnv )
                         == STL_SUCCESS );

                sSortInstantNode->mKeyColList = ((qlncSortInstantNode*)aOrgNode)->mKeyColList;
                sSortInstantNode->mRecColList = ((qlncSortInstantNode*)aOrgNode)->mRecColList;
                sSortInstantNode->mReadColList = ((qlncSortInstantNode*)aOrgNode)->mReadColList;

                sSortInstantNode->mKeyFlags = ((qlncSortInstantNode*)aOrgNode)->mKeyFlags;

                if( ((qlncSortInstantNode*)aOrgNode)->mKeyFilter != NULL )
                {
                    /* @todo 현재 여기에 들어오는 테스트 케이스를 만들기 어려움 */

                    STL_TRY( qlncCopyAndFilter( aParamInfo,
                                                ((qlncSortInstantNode*)aOrgNode)->mKeyFilter,
                                                &(sSortInstantNode->mKeyFilter),
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    sSortInstantNode->mKeyFilter = NULL;
                }

                if( ((qlncSortInstantNode*)aOrgNode)->mRecFilter != NULL )
                {
                    /* @todo 현재 여기에 들어오는 테스트 케이스를 만들기 어려움 */

                    STL_TRY( qlncCopyAndFilter( aParamInfo,
                                                ((qlncSortInstantNode*)aOrgNode)->mRecFilter,
                                                &(sSortInstantNode->mRecFilter),
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    sSortInstantNode->mRecFilter = NULL;
                }

                if( ((qlncSortInstantNode*)aOrgNode)->mFilter != NULL )
                {
                    STL_TRY( qlncCopyAndFilter( aParamInfo,
                                                ((qlncSortInstantNode*)aOrgNode)->mFilter,
                                                &(sSortInstantNode->mFilter),
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    sSortInstantNode->mFilter = NULL;
                }

                sSortInstantNode->mNonFilterSubQueryExprList =
                    ((qlncSortInstantNode*)aOrgNode)->mNonFilterSubQueryExprList;

                sSortInstantNode->mSemiJoinHint = ((qlncSortInstantNode*)aOrgNode)->mSemiJoinHint;
                sSortInstantNode->mSortInstantCost = ((qlncSortInstantNode*)aOrgNode)->mSortInstantCost;

                *aCopiedNode = (qlncNodeCommon*)sSortInstantNode;

                break;
            }
        case QLNC_NODE_TYPE_HASH_INSTANT:
            {
                qlncHashInstantNode * sHashInstantNode  = NULL;

                /* Hash Instant Node 할당 */
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncHashInstantNode ),
                                            (void**) &sHashInstantNode,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                /* Hash Instant Node 내용 복사 */
                QLNC_INIT_NODE_COMMON( &(sHashInstantNode->mNodeCommon),
                                       aOrgNode->mNodeID,
                                       QLNC_NODE_TYPE_HASH_INSTANT );

                sHashInstantNode->mInstantType = ((qlncHashInstantNode*)aOrgNode)->mInstantType;
                sHashInstantNode->mHashTableAttr = ((qlncHashInstantNode*)aOrgNode)->mHashTableAttr;
                sHashInstantNode->mScrollable = ((qlncHashInstantNode*)aOrgNode)->mScrollable;

                STL_TRY( qlncCopyAllNodeWithoutCBOInfo( aParamInfo,
                                                        ((qlncHashInstantNode*)aOrgNode)->mChildNode,
                                                        &(sHashInstantNode->mChildNode),
                                                        aEnv )
                         == STL_SUCCESS );

                sHashInstantNode->mKeyColList = ((qlncHashInstantNode*)aOrgNode)->mKeyColList;
                sHashInstantNode->mRecColList = ((qlncHashInstantNode*)aOrgNode)->mRecColList;
                sHashInstantNode->mReadColList = ((qlncHashInstantNode*)aOrgNode)->mReadColList;

                if( ((qlncHashInstantNode*)aOrgNode)->mKeyFilter != NULL )
                {
                    /* @todo 현재 여기에 들어오는 테스트 케이스를 만들기 어려움 */

                    STL_TRY( qlncCopyAndFilter( aParamInfo,
                                                ((qlncHashInstantNode*)aOrgNode)->mKeyFilter,
                                                &(sHashInstantNode->mKeyFilter),
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    sHashInstantNode->mKeyFilter = NULL;
                }

                if( ((qlncHashInstantNode*)aOrgNode)->mRecFilter != NULL )
                {
                    STL_TRY( qlncCopyAndFilter( aParamInfo,
                                                ((qlncHashInstantNode*)aOrgNode)->mRecFilter,
                                                &(sHashInstantNode->mRecFilter),
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    sHashInstantNode->mRecFilter = NULL;
                }

                if( ((qlncHashInstantNode*)aOrgNode)->mFilter != NULL )
                {
                    STL_TRY( qlncCopyAndFilter( aParamInfo,
                                                ((qlncHashInstantNode*)aOrgNode)->mFilter,
                                                &(sHashInstantNode->mFilter),
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    sHashInstantNode->mFilter = NULL;
                }

                sHashInstantNode->mNonFilterSubQueryExprList =
                    ((qlncHashInstantNode*)aOrgNode)->mNonFilterSubQueryExprList;

                sHashInstantNode->mSemiJoinHint = ((qlncHashInstantNode*)aOrgNode)->mSemiJoinHint;
                sHashInstantNode->mHashInstantCost = ((qlncHashInstantNode*)aOrgNode)->mHashInstantCost;

                *aCopiedNode = (qlncNodeCommon*)sHashInstantNode;

                break;
            }
        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 새로운 Sort Instant Table Node를 생성하고 Node 정보를 Copy 한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aOrgNode        Original Node
 * @param[out]  aCopiedNode     Copied Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCopySortInstantNode( qlncParamInfo    * aParamInfo,
                                   qlncNodeCommon   * aOrgNode,
                                   qlncNodeCommon  ** aCopiedNode,
                                   qllEnv           * aEnv )
{
    qlncSortInstantNode * sSortInstantNode  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrgNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Sort Instant Node 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncSortInstantNode ),
                                (void**) &sSortInstantNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Sort Instant Node 내용 복사 */
    sSortInstantNode->mNodeCommon  = ((qlncSortInstantNode*)aOrgNode)->mNodeCommon;

    sSortInstantNode->mInstantType = ((qlncSortInstantNode*)aOrgNode)->mInstantType;
    sSortInstantNode->mSortTableAttr = ((qlncSortInstantNode*)aOrgNode)->mSortTableAttr;
    sSortInstantNode->mScrollable = ((qlncSortInstantNode*)aOrgNode)->mScrollable;

    sSortInstantNode->mChildNode = ((qlncSortInstantNode*)aOrgNode)->mChildNode;

    sSortInstantNode->mKeyColList = ((qlncSortInstantNode*)aOrgNode)->mKeyColList;
    sSortInstantNode->mRecColList = ((qlncSortInstantNode*)aOrgNode)->mRecColList;
    sSortInstantNode->mReadColList = ((qlncSortInstantNode*)aOrgNode)->mReadColList;

    sSortInstantNode->mKeyFlags = ((qlncSortInstantNode*)aOrgNode)->mKeyFlags;

    if( ((qlncSortInstantNode*)aOrgNode)->mKeyFilter != NULL )
    {
        /* @todo 현재 여기에 들어오는 테스트 케이스를 만들기 어려움 */

        STL_TRY( qlncCopyAndFilter( aParamInfo,
                                    ((qlncSortInstantNode*)aOrgNode)->mKeyFilter,
                                    &(sSortInstantNode->mKeyFilter),
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sSortInstantNode->mKeyFilter = NULL;
    }

    if( ((qlncSortInstantNode*)aOrgNode)->mRecFilter != NULL )
    {
        /* @todo 현재 여기에 들어오는 테스트 케이스를 만들기 어려움 */

        STL_TRY( qlncCopyAndFilter( aParamInfo,
                                    ((qlncSortInstantNode*)aOrgNode)->mRecFilter,
                                    &(sSortInstantNode->mRecFilter),
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sSortInstantNode->mRecFilter = NULL;
    }

    if( ((qlncSortInstantNode*)aOrgNode)->mFilter != NULL )
    {
        /* @todo 현재 여기에 들어오는 테스트 케이스를 만들기 어려움 */

        STL_TRY( qlncCopyAndFilter( aParamInfo,
                                    ((qlncSortInstantNode*)aOrgNode)->mFilter,
                                    &(sSortInstantNode->mFilter),
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sSortInstantNode->mFilter = NULL;
    }

    sSortInstantNode->mNonFilterSubQueryExprList =
        ((qlncSortInstantNode*)aOrgNode)->mNonFilterSubQueryExprList;

    sSortInstantNode->mSemiJoinHint = ((qlncSortInstantNode*)aOrgNode)->mSemiJoinHint;
    sSortInstantNode->mSortInstantCost = ((qlncSortInstantNode*)aOrgNode)->mSortInstantCost;


    /**
     * Output 설정
     */

    *aCopiedNode = (qlncNodeCommon*)sSortInstantNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 새로운 Hash Instant Table Node를 생성하고 Node 정보를 Copy 한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aOrgNode        Original Node
 * @param[out]  aCopiedNode     Copied Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCopyHashInstantNode( qlncParamInfo    * aParamInfo,
                                   qlncNodeCommon   * aOrgNode,
                                   qlncNodeCommon  ** aCopiedNode,
                                   qllEnv           * aEnv )
{
    qlncHashInstantNode * sHashInstantNode  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrgNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Hash Instant Node 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncHashInstantNode ),
                                (void**) &sHashInstantNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Hash Instant Node 내용 복사 */
    sHashInstantNode->mNodeCommon  = ((qlncHashInstantNode*)aOrgNode)->mNodeCommon;

    sHashInstantNode->mInstantType = ((qlncHashInstantNode*)aOrgNode)->mInstantType;
    sHashInstantNode->mHashTableAttr = ((qlncHashInstantNode*)aOrgNode)->mHashTableAttr;
    sHashInstantNode->mScrollable = ((qlncHashInstantNode*)aOrgNode)->mScrollable;

    sHashInstantNode->mChildNode = ((qlncHashInstantNode*)aOrgNode)->mChildNode;

    sHashInstantNode->mKeyColList = ((qlncHashInstantNode*)aOrgNode)->mKeyColList;
    sHashInstantNode->mRecColList = ((qlncHashInstantNode*)aOrgNode)->mRecColList;
    sHashInstantNode->mReadColList = ((qlncHashInstantNode*)aOrgNode)->mReadColList;

    if( ((qlncHashInstantNode*)aOrgNode)->mKeyFilter != NULL )
    {
        /* @todo 현재 여기에 들어오는 테스트 케이스를 만들기 어려움 */

        STL_TRY( qlncCopyAndFilter( aParamInfo,
                                    ((qlncHashInstantNode*)aOrgNode)->mKeyFilter,
                                    &(sHashInstantNode->mKeyFilter),
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sHashInstantNode->mKeyFilter = NULL;
    }

    if( ((qlncHashInstantNode*)aOrgNode)->mRecFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( aParamInfo,
                                    ((qlncHashInstantNode*)aOrgNode)->mRecFilter,
                                    &(sHashInstantNode->mRecFilter),
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sHashInstantNode->mRecFilter = NULL;
    }

    if( ((qlncHashInstantNode*)aOrgNode)->mFilter != NULL )
    {
        /* @todo 현재 여기에 들어오는 테스트 케이스를 만들기 어려움 */

        STL_TRY( qlncCopyAndFilter( aParamInfo,
                                    ((qlncHashInstantNode*)aOrgNode)->mFilter,
                                    &(sHashInstantNode->mFilter),
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sHashInstantNode->mFilter = NULL;
    }

    sHashInstantNode->mNonFilterSubQueryExprList =
        ((qlncHashInstantNode*)aOrgNode)->mNonFilterSubQueryExprList;

    sHashInstantNode->mSemiJoinHint = ((qlncHashInstantNode*)aOrgNode)->mSemiJoinHint;
    sHashInstantNode->mHashInstantCost = ((qlncHashInstantNode*)aOrgNode)->mHashInstantCost;


    /**
     * Output 설정
     */

    *aCopiedNode = (qlncNodeCommon*)sHashInstantNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubTable Candidate Node를 생성한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aQueryNode  Query Node
 * @param[out]  aOutNode    Output Node
 * @param[in]   aEnv        Environment
 */
stlStatus qlncCreateSubTable( qlncParamInfo     * aParamInfo,
                              qlncNodeCommon    * aQueryNode,
                              qlncNodeCommon   ** aOutNode,
                              qllEnv            * aEnv )
{
    qlncSubTableNode    * sCandSubTableNode = NULL;
    qlncExprInnerColumn * sCandInnerColumn  = NULL;
    qlncTarget          * sTargetArr        = NULL;
    stlInt32              sTargetCount;
    stlInt32              i;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Sub Table Candidate Node 할당
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncSubTableNode ),
                                (void**) &sCandSubTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_NODE_COMMON( &(sCandSubTableNode->mNodeCommon),
                           *(aParamInfo->mGlobalNodeID),
                           QLNC_NODE_TYPE_SUB_TABLE );
    (*(aParamInfo->mGlobalNodeID))++;  /* Global ID 증가 */
    sCandSubTableNode->mFilter = NULL;
    QLNC_INIT_LIST( &(sCandSubTableNode->mReadColumnList) );

    /* Join Hint 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncJoinHint ),
                                (void**) &(sCandSubTableNode->mJoinHint),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_JOIN_HINT( sCandSubTableNode->mJoinHint );

    /* Relation Name 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncRelationName ),
                                (void**) &(sCandSubTableNode->mRelationName),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* View Handle 설정 */
    sCandSubTableNode->mHasHandle = STL_FALSE;
    ellInitDictHandle( &(sCandSubTableNode->mSchemaHandle) );
    ellInitDictHandle( &(sCandSubTableNode->mViewHandle) );


    /**
     * 필요한 정보 복사
     */

    /* Catalog Name */
    sCandSubTableNode->mRelationName->mCatalog = NULL;

    /* Schema Name */
    sCandSubTableNode->mRelationName->mSchema = NULL;

    /* Table Name */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                16,
                                (void**) &(sCandSubTableNode->mRelationName->mTable),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlSnprintf( sCandSubTableNode->mRelationName->mTable,
                 16,
                 "TEMP_%03d",
                 sCandSubTableNode->mNodeCommon.mNodeID );

    /* Query Node */
    sCandSubTableNode->mQueryNode = aQueryNode;


    /**
     * Read Column List에 하위 노드의 Target Expr을 추가
     */

    /* 하위 노드의 Target Expr 가져오기 */
    if( aQueryNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC )
    {
        sTargetArr = ((qlncQuerySpec*)aQueryNode)->mTargetArr;
        sTargetCount = ((qlncQuerySpec*)aQueryNode)->mTargetCount;
    }
    else
    {
        /* @todo 현재 Query Set이 들어오는 경우는 없다. */

        STL_DASSERT( aQueryNode->mNodeType == QLNC_NODE_TYPE_QUERY_SET );
        sTargetArr = ((qlncQuerySet*)aQueryNode)->mSetTargetArr;
        sTargetCount = ((qlncQuerySet*)aQueryNode)->mSetTargetCount;
    }

    for( i = 0; i < sTargetCount; i++ )
    {
        /* Candidate Inner Column Expression 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprInnerColumn ),
                                    (void**) &sCandInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sCandInnerColumn->mNode = (qlncNodeCommon*)sCandSubTableNode;
        sCandInnerColumn->mOrgExpr = NULL;
        sCandInnerColumn->mIdx = sCandSubTableNode->mReadColumnList.mCount;

        /* Expression Common 정보 셋팅 */
        QLNC_SET_EXPR_COMMON( &sCandInnerColumn->mCommon,
                              &QLL_CANDIDATE_MEM( aEnv ),
                              aEnv,
                              QLNC_EXPR_TYPE_INNER_COLUMN,
                              QLNC_EXPR_PHRASE_TARGET,
                              DTL_ITERATION_TIME_FOR_EACH_EXPR,
                              sTargetArr[i].mExpr->mPosition,
                              sTargetArr[i].mExpr->mColumnName,
                              sTargetArr[i].mExpr->mDataType );

        /* Sub Table의 Read Column List에 등록 */
        STL_TRY( qlncAddColumnToRefColumnList(
                     aParamInfo,
                     (qlncExprCommon*)sCandInnerColumn,
                     &(sCandSubTableNode->mReadColumnList),
                     aEnv )
                 == STL_SUCCESS );
    }

    sCandSubTableNode->mFilter = NULL;
    sCandSubTableNode->mFilterCost = NULL;


    /**
     * Output 설정
     */

    *aOutNode = (qlncNodeCommon*)sCandSubTableNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */
