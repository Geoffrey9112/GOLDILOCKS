/*******************************************************************************
 * smnmihScan.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smnmihScan.c 6124 2012-10-29 10:31:50Z htkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smnmihScan.c
 * @brief Storage Manager Layer Memory Instant Hash Table Scan Routines
 */

#include <dtl.h>
#include <sml.h>
#include <smDef.h>
#include <sma.h>
#include <sme.h>
#include <smo.h>
#include <smdmifTable.h>
#include <smnmihDef.h>
#include <smnmih.h>
#include <smdmih.h>
#include <smdmifVarPart.h>

/**
 * @addtogroup smnmihScan
 * @{
 */

smeIteratorModule gSmnmihIteratorModule =
{
    STL_SIZEOF(smnmihIterator),

    smnmihInitIterator,
    NULL,   /* FiniIterator */
    smnmihResetIterator,
    NULL,   /* MoveToRowId */
    NULL,   /* MoveToPosNum */
    {
        smnmihFetchNext,
        NULL,   /* FetchPrev */
        NULL,   /* FetchAggr */
        NULL    /* FetchSampling */
    }
};

/**
 * @brief 주어진 Iterator를 초기화한다
 * @param[in,out] aIterator 초기화할 iterator
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmihInitIterator( void   * aIterator,
                              smlEnv * aEnv )
{
    smnmihIterator    * sIterator;
    smnmihIndexHeader * sIndexHeader;

    sIterator = (smnmihIterator*)aIterator;
    
    SMNMIH_INIT_ITERATOR( sIterator );

    sIterator->mSkipHitKey = sIterator->mCommon.mProperty.mSkipHitRecord;
    sIndexHeader = (smnmihIndexHeader*)(sIterator->mCommon.mRelationHandle);
    
    if( sIterator->mSkipHitKey == STL_TRUE )
    {
        sIterator->mHitKeyCount = 0;
        sIterator->mUnhitKeyCount = sIndexHeader->mTotalKeyCount;
    }
    
    return STL_SUCCESS;
}

/**
 * @brief 주어진 Iterator를 다시 초기화한다
 * @param[in,out] aIterator         초기화할 iterator
 * @param[in]     aEnv              Storage Manager Environment
 */
stlStatus smnmihResetIterator( void   * aIterator,
                               smlEnv * aEnv )
{
    smnmihIterator * sIterator = (smnmihIterator *)aIterator;
    smnmihIndexHeader * sIndexHeader;
    
    sIndexHeader = (smnmihIndexHeader*)(sIterator->mCommon.mRelationHandle);

    if( sIterator->mSkipHitKey == STL_TRUE )
    {
        STL_TRY( smnmihResetHitKeys( sIterator,
                                     sIndexHeader,
                                     aEnv )
                 == STL_SUCCESS );
    }
    
    SMNMIH_RESET_ITERATOR( sIterator );

    if( sIterator->mSkipHitKey == STL_TRUE )
    {
        sIterator->mHitKeyCount = 0;
        sIterator->mUnhitKeyCount = sIndexHeader->mTotalKeyCount;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 일반 Scan을 위해 열린 Iterator에서 다음 row를 반환한다
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 */
stlStatus smnmihFetchNext( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv )
{
    STL_PARAM_VALIDATE( aFetchInfo->mBlockJoinFetchInfo != NULL, KNL_ERROR_STACK(aEnv) );
    
    switch ( aFetchInfo->mBlockJoinFetchInfo->mBlockJoinType )
    {
        case KNL_BLOCK_JOIN_INNER_JOIN:
            {
                STL_TRY( smnmihFetchNextInnerJoin( aIterator,
                                                   aFetchInfo,
                                                   aEnv )
                         == STL_SUCCESS);
                break;
            }
        case KNL_BLOCK_JOIN_LEFT_OUTER_JOIN:
            {
                STL_TRY( smnmihFetchNextLeftOuterJoin( aIterator,
                                                       aFetchInfo,
                                                       aEnv )
                         == STL_SUCCESS);
                break;
            }
        case KNL_BLOCK_JOIN_ANTI_OUTER_JOIN:
            {
                STL_TRY( smnmihFetchNextAntiOuterJoin( aIterator,
                                                       aFetchInfo,
                                                       aEnv )
                         == STL_SUCCESS);
                break;
            }
        case KNL_BLOCK_JOIN_SEMI_JOIN:
            {
                STL_TRY( smnmihFetchNextSemiJoin( aIterator,
                                                  aFetchInfo,
                                                  aEnv )
                         == STL_SUCCESS);
                break;
            }
        case KNL_BLOCK_JOIN_ANTI_SEMI_JOIN:
            {
                STL_TRY( smnmihFetchNextAntiSemiJoin( aIterator,
                                                      aFetchInfo,
                                                      aEnv )
                         == STL_SUCCESS);
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief INNER JOIN 의 Result Block 을 Fetch 한다.
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 * @remarks
 * - 각 left row 에 대해 join 조건을 만족하는 모든 right row 에 대해 result row 를 생성한다.
 */
stlStatus smnmihFetchNextInnerJoin( void          * aIterator,
                                    smlFetchInfo  * aFetchInfo,
                                    smlEnv        * aEnv )
{
    smnmihIterator        * sIterator = (smnmihIterator *)aIterator;
    smnmihIndexHeader     * sIndexHeader;
    smlBlockJoinFetchInfo * sJoinFetchInfo;
    stlInt64                sResultBlockSize;
    stlInt32                sLeftUsedSize;
    stlBool                 sHasMatchRow;
    stlChar               * sMatchKey;
    knlLogicalAddr          sMatchKeyLa = KNL_LOGICAL_ADDR_NULL;

    
    sIndexHeader = (smnmihIndexHeader*)sIterator->mCommon.mRelationHandle;
    sJoinFetchInfo = aFetchInfo->mBlockJoinFetchInfo;
    sResultBlockSize = SML_BLOCK_SIZE( aFetchInfo->mRowBlock );
    sLeftUsedSize = DTL_GET_BLOCK_USED_CNT( sJoinFetchInfo->mKeyBlockList );
    
    /**
     * Result Block 의 남아 있는 공간 검사
     */

    if ( sJoinFetchInfo->mResultBlockIdx == 0 )
    {
        SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );
    }
    else
    {
        /* result block 공간이 남아 있는 상태에서 EOF 가 된 경우임 */
    }

    aFetchInfo->mIsEndOfScan = STL_FALSE;
    sJoinFetchInfo->mIsEndOfJoin = STL_FALSE;

    /**
     * Left Block 구간에 대해 Hash Join 수행
     */
    
    while ( sJoinFetchInfo->mLeftBlockIdx < sLeftUsedSize )
    {
        /**
         * Outer Column 설정
         */

        smeSetJoinFilter4InstRelation( sJoinFetchInfo );
        
        /**
         * Hash Key에 대한 Constant Expression 평가
         */

        if( sJoinFetchInfo->mPreHashEvalInfo != NULL )
        {
            sJoinFetchInfo->mPreHashEvalInfo->mBlockIdx = sJoinFetchInfo->mLeftBlockIdx;
            
            STL_TRY( knlEvaluateOneExpression( sJoinFetchInfo->mPreHashEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        
        /**
         * 새로운 Left Row 에 대해 Hash Iterator 정보 설정
         */
        
        if( sIterator->mKeyFetched == STL_FALSE )
        {
            smnmihBeforeFirst( sIterator,
                               sIndexHeader,
                               sJoinFetchInfo,
                               NULL );  /* aLeftIncludeNull */
            
            if( sIterator->mNextKeyLa == KNL_LOGICAL_ADDR_NULL )
            {
                sJoinFetchInfo->mLeftBlockIdx++;
                continue;
            }
        
            sIterator->mKeyFetched = STL_TRUE;
            
            if ( (aFetchInfo->mPhysicalFilter != NULL) || (aFetchInfo->mLogicalFilter != NULL) )
            {
                /**
                 * Join Predicate에 대한 Constant Expression 평가
                 */

                if( sJoinFetchInfo->mPreJoinEvalInfo != NULL )
                {
                    sJoinFetchInfo->mPreJoinEvalInfo->mBlockIdx = sJoinFetchInfo->mLeftBlockIdx;
            
                    STL_TRY( knlEvaluateOneExpression( sJoinFetchInfo->mPreJoinEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                
                smeSetPhysicalFilter4InstRelation( aFetchInfo );
            }
        }

        /**
         * 동일한 Hash Value 를 갖는 Key 들에 대해 Hash Join 수행
         */

        while ( sJoinFetchInfo->mResultBlockIdx < sResultBlockSize )
        {
            /**
             * 동일한 Right Row 를 검색
             */

            sJoinFetchInfo->mReady4JoinFilter = STL_FALSE;

            STL_TRY( smnmihSetNextRightRow( sIterator,
                                            sIndexHeader,
                                            aFetchInfo,
                                            &sHasMatchRow,
                                            &sMatchKeyLa,
                                            &sMatchKey,
                                            aEnv )
                     == STL_SUCCESS );

            if ( sHasMatchRow == STL_FALSE )
            {
                break;
            }

            if( sIterator->mSkipHitKey == STL_TRUE )
            {
                if( SMNMIH_GET_FETCH_BIT( sMatchKey ) == STL_TRUE )
                {
                    if( ( aFetchInfo->mPhysicalFilter == NULL ) &&
                        ( aFetchInfo->mLogicalFilter == NULL ) )
                    {
                        /**
                         * Filter 가 없는 Join의 경우 hit bit가 체크되어 있다면,
                         * 이전 탐색에서 조건에 만족하는 모든 record를 찾았기 때문에
                         * 다시 탐색하지 않아도 된다.
                         * 
                         * 또한 현재 record에 대해서는 join result 를 구성하지 않는다.
                         *
                         * hash filter   : t1.col1 = t2.col1 and t1.col2 = t2.col2
                         * result filter : empty
                         *
                         * v 표시는 현재 단계에서 평가되는 대상 record를 의미.
                         * 
                         * 1. 초기 단계 (t1,t2)
                         *  col1 col2             col1 col2  hit
                         *  ---------             ---------  ---  
                         *  | 1 | 1 |      ->     | 1 | 1 |   F
                         *  | 1 | 1 |             | 1 | 1 |   F
                         *  ---------             | 1 | 2 |   F
                         *                        | 2 | 2 |   F
                         *                        ---------
                         *
                         * 2. 첫번째 left record에 대한 join 결과
                         *  col1 col2             col1 col2  hit
                         *  ---------             ---------  ---  
                         * v| 1 | 1 |      ->   v | 1 | 1 |   F -> T
                         *  | 1 | 1 |           v | 1 | 1 |   F -> T
                         *  ---------             | 1 | 2 |   F
                         *                        | 2 | 2 |   F
                         *                        ---------
                         *
                         * 3. 두번째 left record에 대한 join 수행
                         *  col1 col2             col1 col2  hit
                         *  ---------             ---------  ---  
                         *  | 1 | 1 |      ->   v | 1 | 1 |   T   
                         * v| 1 | 1 |             | 1 | 1 |   T
                         *  ---------             | 1 | 2 |   F
                         *                        | 2 | 2 |   F
                         *                        ---------
                         * 현재 탐색된 record가 hit된 경우,
                         * 현재 record 와 동일한 값을 가지는 record들은
                         * 이전 단계의 탐색에서 이미 탐색되어있음을 보장하기 때문에
                         * 현재 key 로의 탐색을 종료한다.
                         */
                        break;
                    }
                    else
                    {
                        /**
                         * Filter 가 있는 Join의 경우 hit bit가 체크되어 있다면,
                         * 현재 record에 대해서는 join result 를 구성하지 않는다.
                         * 
                         * 현재 hash key 값과 일치하는 hash instant 의 record 중에
                         * Filter 에 의해 hit 되지 않은 record 가 존재할 수 있기 때문에
                         * scan을 지속한다.
                         *
                         * hash filter   : t1.col1 = t2.col1 
                         * result filter : t1.col2 <= t2.col2
                         *
                         * v 표시는 현재 단계에서 평가되는 대상 record를 의미.
                         * 
                         * 1. 초기 단계 (t1,t2)
                         *  col1 col2             col1 col2  hit
                         *  ---------             ---------  ---  
                         *  | 1 | 1 |      ->     | 1 | 1 |   F
                         *  | 1 | 1 |             | 1 | 1 |   F
                         *  | 1 | 2 |             | 1 | 2 |   F
                         *  ---------             | 2 | 2 |   F
                         *                        ---------
                         *
                         * 2. 첫번째 left record에 대한 join 결과
                         *  col1 col2             col1 col2  hit
                         *  ---------             ---------  ---  
                         * v| 1 | 1 |      ->   v | 1 | 1 |   F -> T
                         *  | 1 | 1 |           v | 1 | 1 |   F -> T
                         *  | 1 | 1 |             | 1 | 2 |   F
                         *  ---------             | 2 | 2 |   F
                         *                        ---------
                         *
                         * 3. 두번째 left record에 대한 join 결과
                         *  col1 col2             col1 col2  hit
                         *  ---------             ---------  ---  
                         *  | 1 | 1 |      ->   v | 1 | 1 |   T
                         * v| 1 | 1 |           v | 1 | 1 |   T
                         *  | 1 | 1 |             | 1 | 2 |   F
                         *  ---------             | 2 | 2 |   F
                         *                        ---------
                         * a. 현재 탐색된 record가 hit된 경우 join result룰 구성하지 않는다.
                         * b. 현재 탐색된 record가 hit되지 않은 경우 join result룰 구성한다.
                         * 
                         * 조건에 맞는 모든 row를 탐색하여 a 와 b 의 경우를 각각 적용한다.
                         *                        
                         * 4. 두번째 left record에 대한 join 수행
                         *  col1 col2             col1 col2  hit
                         *  ---------             ---------  ---  
                         *  | 1 | 1 |      ->   v | 1 | 1 |   T
                         *  | 1 | 1 |           v | 1 | 1 |   T
                         * v| 1 | 2 |           v | 1 | 2 |   F -> T
                         *  ---------             | 2 | 2 |   F
                         *                        ---------
                         * t2 에서 ( 1, 2 )를 찾은 경우 hit되지 않았기 때문에
                         * join result를 구성하고 hit 를 체크한다.
                         * 
                         * 조건에 맞는 모든 row를 탐색하여 a 와 b 의 경우를 각각 적용한다.
                         */
                        continue;
                    }
                }
                else
                {
                    SMNMIH_SET_FETCH_BIT( sMatchKey, STL_TRUE );
                    
                    sIterator->mUnhitKeyCount--;
                    if( sIterator->mHitKeyCount < SMNMIH_MAX_HIT_KEY_HINT_COUNT )
                    {
                        sIterator->mHitKeyLa[sIterator->mHitKeyCount] = sMatchKeyLa;
                    }
                    sIterator->mHitKeyCount++;
                }
            }

            /**
             * Result Block 구성
             */

            STL_TRY( knlSetJoinResultBlock( sJoinFetchInfo->mResultRelationList,
                                            sJoinFetchInfo->mResultBlockIdx,
                                            sJoinFetchInfo->mLeftBlockIdx,
                                            sJoinFetchInfo->mResultBlockIdx, /* right block idx */
                                            KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * 남은 Result Join 조건에 부합하는지 검사
             */
            
            if ( sJoinFetchInfo->mResultFilter == NULL )
            {
                sHasMatchRow = STL_TRUE;

            }
            else
            {
                /**
                 * Result Filter 를 위한 Left 상수 구성
                 */

                if ( sJoinFetchInfo->mReady4JoinFilter == STL_FALSE )
                {
                    smeSetJoinFilter4InstRelation( sJoinFetchInfo );
                    smeSetPhysicalFilter4InstRelation( aFetchInfo );
                    sJoinFetchInfo->mReady4JoinFilter = STL_TRUE;
                }
                
                STL_TRY( knlEvaluateJoinFilter( sJoinFetchInfo->mResultFilter,
                                                sJoinFetchInfo->mResultBlockIdx,
                                                & sHasMatchRow,
                                                KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }

            /**
             * Result Block Idx 증가 여부 판단
             */
            
            if ( sHasMatchRow == STL_TRUE )
            {
                if( aFetchInfo->mSkipCnt > 0 )
                {
                    aFetchInfo->mSkipCnt--;
                    continue;
                }
                else
                {
                    sJoinFetchInfo->mResultRefLeftBlockIdx[sJoinFetchInfo->mResultBlockIdx]
                        = sJoinFetchInfo->mLeftBlockIdx;
                    sJoinFetchInfo->mResultBlockIdx++;
                    aFetchInfo->mFetchCnt--;

                    /**
                     * Hit key를 skip 하는 경우 Unhit Key가 없다면 더이상 스캔할 필요가 없다.
                     */
                    if( sIterator->mSkipHitKey == STL_TRUE )
                    {
                        if( sIterator->mUnhitKeyCount == 0 )
                        {
                            sJoinFetchInfo->mIsEndOfJoin = STL_TRUE;
                            aFetchInfo->mIsEndOfScan = STL_TRUE;
                            STL_THROW( RAMP_RESULT_FULL );
                        }
                    }
                
                    STL_TRY_THROW( aFetchInfo->mFetchCnt > 0, RAMP_RESULT_FULL );
                }
            }
            else
            {
                continue;
            }
        }

        /**
         * 하나의 Left Row 에 대한 처리가 완료되었는지 검사.
         */
        
        if ( sJoinFetchInfo->mResultBlockIdx == sResultBlockSize )
        {
            /* result block 이 모두 채워짐 */
            aFetchInfo->mIsEndOfScan = STL_FALSE;
            STL_THROW( RAMP_RESULT_FULL );
        }
        else
        {
            sJoinFetchInfo->mLeftBlockIdx++;
            sIterator->mKeyFetched = STL_FALSE;
        }
    }

    /* left block 에 대한 처리가 모두 끝남 */
    sJoinFetchInfo->mLeftBlockIdx = 0;
    aFetchInfo->mIsEndOfScan = STL_TRUE;

    STL_RAMP( RAMP_RESULT_FULL );

    SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sJoinFetchInfo->mResultBlockIdx );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief LEFT OUTER JOIN 의 Result Block 을 Fetch 한다.
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 * @remarks
 * - 하나의 left row 에 대해 outer join 조건을 만족하는 모든 right row 에 대해 result row 를 생성한다.
 * - 어떤 right row 도 만족하지 않는 경우, left row 에 대해 right 가 NULL 인 하나의 result row 를 생성한다.
 * 
 * ex) SELECT *
 *       FROM left LEFT OUTER JOIN right
 *                              ON left_column = right_column
 *                             AND outer_join_filter
 *      WHERE inner_join_filter
 */
stlStatus smnmihFetchNextLeftOuterJoin( void          * aIterator,
                                        smlFetchInfo  * aFetchInfo,
                                        smlEnv        * aEnv )
{
    smnmihIterator        * sIterator = (smnmihIterator *)aIterator;
    smnmihIndexHeader     * sIndexHeader = (smnmihIndexHeader*)sIterator->mCommon.mRelationHandle;
    smlBlockJoinFetchInfo * sJoinFetchInfo = aFetchInfo->mBlockJoinFetchInfo;
    stlInt64                sResultBlockSize = SML_BLOCK_SIZE( aFetchInfo->mRowBlock );
    stlInt32                sLeftUsedSize = DTL_GET_BLOCK_USED_CNT( sJoinFetchInfo->mKeyBlockList );
    stlBool                 sHasMatchRow;
    stlBool                 sLeftRowMatched;
    knlValueBlockList     * sRightBlock;
    dtlDataValue          * sRightValue;
    stlChar               * sMatchKey = NULL;
    knlLogicalAddr          sMatchKeyLa;

    
    /**
     * Result Block 의 남아 있는 공간 검사
     */
    
    if ( sJoinFetchInfo->mResultBlockIdx == 0 )
    {
        SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );
    }
    else
    {
        /* result block 공간이 남아 있는 상태에서 EOF 가 된 경우임 */
    }

    aFetchInfo->mIsEndOfScan = STL_FALSE;

    /**
     * Left Block 구간에 대해 Hash Join 수행
     */
    
    while ( sJoinFetchInfo->mLeftBlockIdx < sLeftUsedSize )
    {
        /**
         * Outer Column 설정
         */

        smeSetJoinFilter4InstRelation( sJoinFetchInfo );

        /**
         * Hash Key에 대한 Constant Expression 평가
         */

        if( sJoinFetchInfo->mPreHashEvalInfo != NULL )
        {
            sJoinFetchInfo->mPreHashEvalInfo->mBlockIdx = sJoinFetchInfo->mLeftBlockIdx;
            
            STL_TRY( knlEvaluateOneExpression( sJoinFetchInfo->mPreHashEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }

        /**
         * 새로운 Left Row 에 대해 Hash Iterator 정보 설정
         */
        
        if( sIterator->mKeyFetched == STL_FALSE )
        {
            smnmihBeforeFirst( sIterator,
                               sIndexHeader,
                               sJoinFetchInfo,
                               NULL );  /* aLeftIncludeNull */
            
            sIterator->mKeyFetched = STL_TRUE;

            if ( (aFetchInfo->mPhysicalFilter != NULL) || (aFetchInfo->mLogicalFilter != NULL) )
            {
                /**
                 * Join Predicate에 대한 Constant Expression 평가
                 */

                if( sJoinFetchInfo->mPreJoinEvalInfo != NULL )
                {
                    sJoinFetchInfo->mPreJoinEvalInfo->mBlockIdx = sJoinFetchInfo->mLeftBlockIdx;
            
                    STL_TRY( knlEvaluateOneExpression( sJoinFetchInfo->mPreJoinEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                
                smeSetPhysicalFilter4InstRelation( aFetchInfo );
            }
            
            sLeftRowMatched = STL_FALSE;
        }
        else
        {
            /* 이미 이전 result 에서 left outer join 을 만족하는 row 가 생김 */
            sLeftRowMatched = STL_TRUE;
        }

        /**
         * 동일한 Hash Value 를 갖는 Key 들에 대해 Hash Join 수행
         */

        while ( sJoinFetchInfo->mResultBlockIdx < sResultBlockSize )
        {
            /**
             * 동일한 Right Row 를 검색
             */
            
            sJoinFetchInfo->mReady4JoinFilter = STL_FALSE;

            STL_TRY( smnmihSetNextRightRow( sIterator,
                                            sIndexHeader,
                                            aFetchInfo,
                                            &sHasMatchRow,
                                            &sMatchKeyLa,
                                            &sMatchKey,
                                            aEnv )
                     == STL_SUCCESS );

            if ( sHasMatchRow == STL_FALSE )
            {
                break;
            }

            /**
             * Result Block 구성
             */

            STL_TRY( knlSetJoinResultBlock( sJoinFetchInfo->mResultRelationList,
                                            sJoinFetchInfo->mResultBlockIdx,
                                            sJoinFetchInfo->mLeftBlockIdx,
                                            sJoinFetchInfo->mResultBlockIdx, /* right block idx */
                                            KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * 남은 Outer Join 조건에 부합하는지 검사
             */

            if ( sJoinFetchInfo->mOuterJoinFilter == NULL )
            {
                sHasMatchRow = STL_TRUE;
                sLeftRowMatched = STL_TRUE;

                SMNMIH_SET_FETCH_BIT( sMatchKey, STL_TRUE );
            }
            else
            {
                /**
                 * Outer Join Filter 를 위한 Left 상수 구성
                 */

                if ( sJoinFetchInfo->mReady4JoinFilter == STL_FALSE )
                {
                    smeSetJoinFilter4InstRelation( sJoinFetchInfo );
                    smeSetPhysicalFilter4InstRelation( aFetchInfo );
                    sJoinFetchInfo->mReady4JoinFilter = STL_TRUE;
                }
                
                STL_TRY( knlEvaluateJoinFilter( sJoinFetchInfo->mOuterJoinFilter,
                                                sJoinFetchInfo->mResultBlockIdx,
                                                & sHasMatchRow,
                                                KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if ( sHasMatchRow == STL_TRUE )
                {
                    sLeftRowMatched = STL_TRUE;
                    SMNMIH_SET_FETCH_BIT( sMatchKey, STL_TRUE );
                }
                else
                {
                    break;
                }
            }
            
            /**
             * 남은 Result 조건에 부합하는지 검사
             */

            if ( sJoinFetchInfo->mResultFilter == NULL )
            {
                sHasMatchRow = STL_TRUE;
            }
            else
            {
                /**
                 * Result Filter 를 위한 Left 상수 구성
                 */

                if ( sJoinFetchInfo->mReady4JoinFilter == STL_FALSE )
                {
                    smeSetJoinFilter4InstRelation( sJoinFetchInfo );
                    smeSetPhysicalFilter4InstRelation( aFetchInfo );
                    sJoinFetchInfo->mReady4JoinFilter = STL_TRUE;
                }
                
                STL_TRY( knlEvaluateJoinFilter( sJoinFetchInfo->mResultFilter,
                                                sJoinFetchInfo->mResultBlockIdx,
                                                & sHasMatchRow,
                                                KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }

            /**
             * Result Block Idx 증가 여부 판단
             */
            
            if ( sHasMatchRow == STL_TRUE )
            {
                if( aFetchInfo->mSkipCnt > 0 )
                {
                    aFetchInfo->mSkipCnt--;
                    continue;
                }
                else
                {
                    sJoinFetchInfo->mResultRefLeftBlockIdx[sJoinFetchInfo->mResultBlockIdx]
                        = sJoinFetchInfo->mLeftBlockIdx;
                    sJoinFetchInfo->mResultBlockIdx++;
                    aFetchInfo->mFetchCnt--;

                    STL_TRY_THROW( aFetchInfo->mFetchCnt > 0, RAMP_RESULT_FULL );
                }
            }
            else
            {
                continue;
            }
        }

        /**
         * 하나의 Left Row 에 대한 처리가 완료되었는지 검사.
         */
        
        if ( sJoinFetchInfo->mResultBlockIdx == sResultBlockSize )
        {
            /* result block 이 모두 채워짐 */
            aFetchInfo->mIsEndOfScan = STL_FALSE;
            STL_THROW( RAMP_RESULT_FULL );
            break;
        }
        else
        {
            if ( sLeftRowMatched == STL_TRUE )
            {
                /* outer join 조건을 만족하는 left row 가 존재함 */
                sJoinFetchInfo->mLeftBlockIdx++;
                sIterator->mKeyFetched = STL_FALSE;
            }
            else
            {
                /* outer join 조건을 만족하는 left row 가 존재하지 않음 */
                
                /**
                 * Right Row 를 Null 로 채운 후 Result Block 을 구성함
                 */

                for ( sRightBlock = aFetchInfo->mColList;
                      sRightBlock != NULL;
                      sRightBlock = sRightBlock->mNext )
                {
                    sRightValue = KNL_GET_BLOCK_DATA_VALUE( sRightBlock, sJoinFetchInfo->mResultBlockIdx );
                    DTL_SET_NULL( sRightValue );
                }

                STL_TRY( knlSetJoinResultBlock( sJoinFetchInfo->mResultRelationList,
                                                sJoinFetchInfo->mResultBlockIdx,
                                                sJoinFetchInfo->mLeftBlockIdx,
                                                sJoinFetchInfo->mResultBlockIdx, /* right block idx */
                                                KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                /**
                 * 남은 Result 조건에 부합하는지 검사
                 */
                
                if ( sJoinFetchInfo->mResultFilter == NULL )
                {
                    sHasMatchRow = STL_TRUE;
                }
                else
                {
                    /**
                     * Result Filter 를 위한 Left 상수 구성
                     */
                    
                    if ( sJoinFetchInfo->mReady4JoinFilter == STL_FALSE )
                    {
                        smeSetJoinFilter4InstRelation( sJoinFetchInfo );
                        smeSetPhysicalFilter4InstRelation( aFetchInfo );
                        sJoinFetchInfo->mReady4JoinFilter = STL_TRUE;
                    }
                    
                    STL_TRY( knlEvaluateJoinFilter( sJoinFetchInfo->mResultFilter,
                                                    sJoinFetchInfo->mResultBlockIdx,
                                                    & sHasMatchRow,
                                                    KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                
                /**
                 * Result Block Idx 증가 여부 판단
                 */
                
                if ( sHasMatchRow == STL_TRUE )
                {
                    if( aFetchInfo->mSkipCnt > 0 )
                    {
                        aFetchInfo->mSkipCnt--;
                    }
                    else
                    {
                        sJoinFetchInfo->mResultRefLeftBlockIdx[sJoinFetchInfo->mResultBlockIdx]
                            = sJoinFetchInfo->mLeftBlockIdx;
                        sJoinFetchInfo->mResultBlockIdx++;
                        aFetchInfo->mFetchCnt--;

                        STL_TRY_THROW( aFetchInfo->mFetchCnt > 0, RAMP_RESULT_FULL );
                    }

                    if ( sJoinFetchInfo->mResultBlockIdx == sResultBlockSize )
                    {
                        /* result block 이 모두 채워짐 */
                        aFetchInfo->mIsEndOfScan = STL_FALSE;
                        STL_THROW( RAMP_RESULT_FULL );
                    }
                }
                else
                {
                    /* Result Filter 에 만족하지 않음 */
                }

                sJoinFetchInfo->mLeftBlockIdx++;
                sIterator->mKeyFetched = STL_FALSE;
            }
        }
    }

    /* left block 에 대한 처리가 모두 끝남 */
    sJoinFetchInfo->mLeftBlockIdx = 0;
    aFetchInfo->mIsEndOfScan = STL_TRUE;
    
    STL_RAMP( RAMP_RESULT_FULL );

    SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sJoinFetchInfo->mResultBlockIdx );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief ANTI OUTER JOIN 의 Result Block 을 Fetch 한다.
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 * @remarks
 * - Full Outer Join 의 right -> left 의 ANTI OUTER JOIN 을 수행한다.
 * 
 * ex) SELECT *
 *       FROM left FULL OUTER JOIN right
 *                              ON left_column = right_column
 *                             AND outer_join_filter
 *      WHERE inner_join_filter
 *
 * - phase 1) left LEFT OUTER JOIN right
 * - phase 2) right ANTI OUTER JOIN left
 */
stlStatus smnmihFetchNextAntiOuterJoin( void          * aIterator,
                                        smlFetchInfo  * aFetchInfo,
                                        smlEnv        * aEnv )
{
    smnmihIterator        * sIterator = (smnmihIterator *)aIterator;
    smnmihIndexHeader     * sIndexHeader = (smnmihIndexHeader*)sIterator->mCommon.mRelationHandle;
    smlBlockJoinFetchInfo * sJoinFetchInfo = aFetchInfo->mBlockJoinFetchInfo;
    stlInt64                sResultBlockSize = SML_BLOCK_SIZE( aFetchInfo->mRowBlock );
    stlBool                 sHasMatchRow;
    
    /**
     * Result Block 의 남아 있는 공간 검사
     */
    
    if ( sJoinFetchInfo->mResultBlockIdx == 0 )
    {
        SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );
    }
    else
    {
        /* result block 공간이 남아 있는 상태에서 EOF 가 된 경우임 */
    }

    aFetchInfo->mIsEndOfScan = STL_FALSE;

    /**
     * Result Block 을 채울 때까지 anti outer join 을 수행한다.
     */
    
    while( sJoinFetchInfo->mResultBlockIdx < sResultBlockSize )
    {
        /**
         * LEFT OUTER JOIN 에 의해 hit 되지 않은 row 를 fetch 함.
         */

        sJoinFetchInfo->mReady4JoinFilter = STL_FALSE;

        STL_TRY( smnmihFetchNextUnhitRow( aIterator,
                                          sIndexHeader,
                                          aFetchInfo,
                                          &sHasMatchRow,
                                          aEnv )
                 == STL_SUCCESS );

        /**
         * no more unhit row
         */
        
        if ( sHasMatchRow == STL_FALSE )
        {
            break;
        }
            
        /**
         * Left Row 를 Null 로 채운 Result Block 을 구성함
         */
        
        STL_TRY( knlSetLeftNullPaddJoinResultBlock( sJoinFetchInfo->mResultRelationList,
                                                    sJoinFetchInfo->mResultBlockIdx,
                                                    sJoinFetchInfo->mResultBlockIdx, /* right block idx */
                                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * 남은 Result 조건에 부합하는지 검사
         */
        
        if ( sJoinFetchInfo->mResultFilter != NULL )
        {
            /**
             * Result Filter 를 위한 Left 상수 구성
             */
            
            if ( sJoinFetchInfo->mReady4JoinFilter == STL_FALSE )
            {
                smeSetJoinFilter4InstRelation( sJoinFetchInfo );
                
                /**
                 * Join Predicate에 대한 Constant Expression 평가
                 */

                if( sJoinFetchInfo->mPreJoinEvalInfo != NULL )
                {
                    sJoinFetchInfo->mPreJoinEvalInfo->mBlockIdx = sJoinFetchInfo->mLeftBlockIdx;
            
                    STL_TRY( knlEvaluateOneExpression( sJoinFetchInfo->mPreJoinEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }

                smeSetPhysicalFilter4InstRelation( aFetchInfo );
                sJoinFetchInfo->mReady4JoinFilter = STL_TRUE;
            }
            
            STL_TRY( knlEvaluateJoinFilter( sJoinFetchInfo->mResultFilter,
                                            sJoinFetchInfo->mResultBlockIdx,
                                            & sHasMatchRow,
                                            KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        
        /**
         * Result Block Idx 증가 여부 판단
         */
        
        if ( sHasMatchRow == STL_TRUE )
        {
            if( aFetchInfo->mSkipCnt > 0 )
            {
                aFetchInfo->mSkipCnt--;
            }
            else
            {
                sJoinFetchInfo->mResultBlockIdx++;
                aFetchInfo->mFetchCnt--;

                STL_TRY_THROW( aFetchInfo->mFetchCnt > 0, RAMP_RESULT_FULL );
            }
        }
    }

    /**
     * anti outer join 의 수행이 모두 완료되었는지의 여부
     */
    
    if ( sJoinFetchInfo->mResultBlockIdx == sResultBlockSize )
    {
        aFetchInfo->mIsEndOfScan = STL_FALSE;
    }
    else
    {
        aFetchInfo->mIsEndOfScan = STL_TRUE;
    }

    STL_RAMP( RAMP_RESULT_FULL );
    
    SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sJoinFetchInfo->mResultBlockIdx );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SEMI JOIN 의 Result Block 을 Fetch 한다.
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 * @remarks
 * - 하나의 left row 에 대해 semi join 조건을 만족하는 right row 가 존재할 경우, 하나의 result row 를 생성한다.
 * 
 * ex) SELECT *
 *       FROM left
 *      WHERE left_column IN ( SELECT right_column
 *                               FROM right )
 */
stlStatus smnmihFetchNextSemiJoin( void          * aIterator,
                                   smlFetchInfo  * aFetchInfo,
                                   smlEnv        * aEnv )
{
    smnmihIterator        * sIterator = (smnmihIterator *)aIterator;
    smnmihIndexHeader     * sIndexHeader = (smnmihIndexHeader*)sIterator->mCommon.mRelationHandle;
    smlBlockJoinFetchInfo * sJoinFetchInfo = aFetchInfo->mBlockJoinFetchInfo;
    stlInt64                sResultBlockSize = SML_BLOCK_SIZE( aFetchInfo->mRowBlock );
    stlInt32                sLeftUsedSize = DTL_GET_BLOCK_USED_CNT( sJoinFetchInfo->mKeyBlockList );
    stlBool                 sHasMatchRow;
    stlChar               * sMatchKey;
    knlLogicalAddr          sMatchKeyLa;
    
    
    /**
     * Result Block 의 남아 있는 공간 검사
     */
    
    if ( sJoinFetchInfo->mResultBlockIdx == 0 )
    {
        SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );
    }
    else
    {
        /* result block 공간이 남아 있는 상태에서 EOF 가 된 경우임 */
    }

    aFetchInfo->mIsEndOfScan = STL_FALSE;
    sJoinFetchInfo->mIsEndOfJoin = STL_FALSE;

    /**
     * Left Block 구간에 대해 Hash Join 수행
     */
    
    while ( sJoinFetchInfo->mLeftBlockIdx < sLeftUsedSize )
    {
        /**
         * Outer Column 설정
         */

        smeSetJoinFilter4InstRelation( sJoinFetchInfo );

        /**
         * Hash Key에 대한 Constant Expression 평가
         */

        if( sJoinFetchInfo->mPreHashEvalInfo != NULL )
        {
            sJoinFetchInfo->mPreHashEvalInfo->mBlockIdx = sJoinFetchInfo->mLeftBlockIdx;
            
            STL_TRY( knlEvaluateOneExpression( sJoinFetchInfo->mPreHashEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }

        /**
         * 새로운 Left Row 에 대해 Hash Iterator 정보 설정
         */
        
        smnmihBeforeFirst( sIterator,
                           sIndexHeader,
                           sJoinFetchInfo,
                           NULL );  /* aLeftIncludeNull */

        if( sIterator->mNextKeyLa == KNL_LOGICAL_ADDR_NULL )
        {
            sJoinFetchInfo->mLeftBlockIdx++;
            continue;
        }

        if ( (aFetchInfo->mPhysicalFilter != NULL) || (aFetchInfo->mLogicalFilter != NULL) )
        {
            /**
             * Join Predicate에 대한 Constant Expression 평가
             */

            if( sJoinFetchInfo->mPreJoinEvalInfo != NULL )
            {
                sJoinFetchInfo->mPreJoinEvalInfo->mBlockIdx = sJoinFetchInfo->mLeftBlockIdx;

                STL_TRY( knlEvaluateOneExpression( sJoinFetchInfo->mPreJoinEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }

            smeSetPhysicalFilter4InstRelation( aFetchInfo );
        }

        /**
         * 동일한 Hash Value 를 갖는 Key 들에 대해 Hash Join 수행
         */

        while ( sJoinFetchInfo->mResultBlockIdx < sResultBlockSize )
        {
            /**
             * 동일한 Right Row 를 검색
             */
            
            sJoinFetchInfo->mReady4JoinFilter = STL_FALSE;

            STL_TRY( smnmihSetNextRightRow( sIterator,
                                            sIndexHeader,
                                            aFetchInfo,
                                            &sHasMatchRow,
                                            &sMatchKeyLa,
                                            &sMatchKey,
                                            aEnv )
                     == STL_SUCCESS );

            if ( sHasMatchRow == STL_FALSE )
            {
                break;
            }

            /**
             * Result Block 구성
             */

            STL_TRY( knlSetJoinResultBlock( sJoinFetchInfo->mResultRelationList,
                                            sJoinFetchInfo->mResultBlockIdx,
                                            sJoinFetchInfo->mLeftBlockIdx,
                                            sJoinFetchInfo->mResultBlockIdx, /* right block idx */
                                            KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * 남은 Result Join 조건에 부합하는지 검사
             */
            
            if ( sJoinFetchInfo->mResultFilter != NULL )
            {
                /**
                 * Result Filter 를 위한 Left 상수 구성
                 */

                if ( sJoinFetchInfo->mReady4JoinFilter == STL_FALSE )
                {
                    smeSetJoinFilter4InstRelation( sJoinFetchInfo );
                    smeSetPhysicalFilter4InstRelation( aFetchInfo );
                    sJoinFetchInfo->mReady4JoinFilter = STL_TRUE;
                }
                
                STL_TRY( knlEvaluateJoinFilter( sJoinFetchInfo->mResultFilter,
                                                sJoinFetchInfo->mResultBlockIdx,
                                                & sHasMatchRow,
                                                KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }

            /**
             * Result Block Idx 증가 여부 판단
             */
            
            /* semi join 조건을 만족하는 left row 임 */
            if ( sHasMatchRow == STL_TRUE )
            {
                if( aFetchInfo->mSkipCnt > 0 )
                {
                    aFetchInfo->mSkipCnt--;
                }
                else
                {
                    sJoinFetchInfo->mResultRefLeftBlockIdx[sJoinFetchInfo->mResultBlockIdx]
                        = sJoinFetchInfo->mLeftBlockIdx;
                    sJoinFetchInfo->mResultBlockIdx++;
                    aFetchInfo->mFetchCnt--;

                    if( aFetchInfo->mFetchCnt == 0 )
                    {
                        aFetchInfo->mIsEndOfScan = STL_TRUE;
                        STL_THROW( RAMP_RESULT_FULL );
                    }
                }
                /* semi join 조건을 만족하므로 더 이상 진행하지 않는다 */
                break;
            }
        }

        sJoinFetchInfo->mLeftBlockIdx++;

        /**
         * Result Block 이 모두 채워졌는지 검사 
         */
        
        if ( sJoinFetchInfo->mResultBlockIdx == sResultBlockSize )
        {
            /* result block 이 모두 채워짐 */
            aFetchInfo->mIsEndOfScan = STL_FALSE;
            STL_THROW( RAMP_RESULT_FULL );
            break;
        }
        else
        {
            continue;
        }
    }

    /* left block 에 대한 처리가 모두 끝남 */
    sJoinFetchInfo->mLeftBlockIdx = 0;
    aFetchInfo->mIsEndOfScan = STL_TRUE;
    
    STL_RAMP( RAMP_RESULT_FULL );

    SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sJoinFetchInfo->mResultBlockIdx );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ANTI SEMI JOIN 의 Result Block 을 Fetch 한다.
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 * @remarks
 * - 하나의 left row 에 대해 semi join 조건을 만족하는 right row 가 없을 경우, 하나의 result row 를 생성한다.
 * ex) SELECT *
 *       FROM left
 *      WHERE left_column NOT IN ( SELECT right_column
 *                                   FROM right )
 */
stlStatus smnmihFetchNextAntiSemiJoin( void          * aIterator,
                                       smlFetchInfo  * aFetchInfo,
                                       smlEnv        * aEnv )
{
    smnmihIterator        * sIterator = (smnmihIterator *)aIterator;
    smnmihIndexHeader     * sIndexHeader = (smnmihIndexHeader*)sIterator->mCommon.mRelationHandle;
    smlBlockJoinFetchInfo * sJoinFetchInfo = aFetchInfo->mBlockJoinFetchInfo;
    stlInt64                sResultBlockSize = SML_BLOCK_SIZE( aFetchInfo->mRowBlock );
    stlInt32                sLeftUsedSize = DTL_GET_BLOCK_USED_CNT( sJoinFetchInfo->mKeyBlockList );
    stlBool                 sHasMatchRow;
    stlChar               * sMatchKey;
    knlLogicalAddr          sMatchKeyLa;
    stlBool                 sLeftIncludeNull;
    
    
    /**
     * Result Block 의 남아 있는 공간 검사
     */
    
    if ( sJoinFetchInfo->mResultBlockIdx == 0 )
    {
        SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );
    }
    else
    {
        /* result block 공간이 남아 있는 상태에서 EOF 가 된 경우임 */
    }

    aFetchInfo->mIsEndOfScan = STL_FALSE;
    sJoinFetchInfo->mIsEndOfJoin = STL_FALSE;
    
    /**
     * Left Block 구간에 대해 Hash Join 수행
     */
    
    while ( sJoinFetchInfo->mLeftBlockIdx < sLeftUsedSize )
    {
        /**
         * Outer Column 설정
         */

        smeSetJoinFilter4InstRelation( sJoinFetchInfo );

        /**
         * Hash Key에 대한 Constant Expression 평가
         */

        if( sJoinFetchInfo->mPreHashEvalInfo != NULL )
        {
            sJoinFetchInfo->mPreHashEvalInfo->mBlockIdx = sJoinFetchInfo->mLeftBlockIdx;
            
            STL_TRY( knlEvaluateOneExpression( sJoinFetchInfo->mPreHashEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        
        /**
         * 새로운 Left Row 에 대해 Hash Iterator 정보 설정
         */
        
        sLeftIncludeNull = STL_FALSE;

        smnmihBeforeFirst( sIterator,
                           sIndexHeader,
                           sJoinFetchInfo,
                           &sLeftIncludeNull );

        if ( (aFetchInfo->mPhysicalFilter != NULL) || (aFetchInfo->mLogicalFilter != NULL) )
        {
            /**
             * Join Predicate에 대한 Constant Expression 평가
             */

            if( sJoinFetchInfo->mPreJoinEvalInfo != NULL )
            {
                sJoinFetchInfo->mPreJoinEvalInfo->mBlockIdx = sJoinFetchInfo->mLeftBlockIdx;

                STL_TRY( knlEvaluateOneExpression( sJoinFetchInfo->mPreJoinEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }

            smeSetPhysicalFilter4InstRelation( aFetchInfo );
        }


        /**
         * Anti-Semi Join의 결과 처리
         *
         * 1. Right Node(Hash Instant)에 Filter가 존재하는 경우
         *    1) Filter처리 후 row 개수가 0건인 경우
         *       => Key 비교 결과와 상관없이 결과로 반환
         *    2) Filter처리 후 row 개수가 1건 이상인 경우
         *       => Key 비교 결과에 의해
         *          * 하나라도 TRUE가 존재하면 결과로 반환하지 않음
         *          * TRUE가 하나도 없고 하나라도 UNKNOWN이 존재하면 결과로 반환하지 않음
         *          * 모두 FALSE인 경우 결과로 반환
         * 2. Right Node(Hash Instant)에 Filter가 존재하지 않는 경우
         *    1) row 개수가 0건인 경우
         *       => Key 비교 결과와 상관없이 결과로 반환
         *    2) row 개수가 1건 이상인 경우
         *       => Key 비교 결과에 의해
         *          * 하나라도 TRUE가 존재하면 결과로 반환하지 않음
         *          * TRUE가 하나도 없고 하나라도 UNKNOWN이 존재하면 결과로 반환하지 않음
         *          * 모두 FALSE인 경우 결과로 반환
         *
         * ## 위의 조건에 의해 Join 결과를 결정해야 한다.
         *    그러나 Hash Join의 특성상 Key의 비교가 먼저 이루어지고
         *    Filter를 처리하게 되므로 아래와 같은 처리가 이루어진다.
         *
         * A. Left Node의 Key에 NULL이 존재하는 경우
         *    1) Physical Filter나 Logical Filter가 존재하는 경우
         *       => Filter를 이용하여 Right Node의 모든 row를 탐색하고
         *          일치하는 데이터가 없는 경우 결과를 반환
         *    2) Physical Filter나 Logical Filter가 존재하지 않는 경우
         *       => Right Node에 row가 하나도 존재하지 않는 경우 결과 반환
         * B. Left Node의 Key에 NULL이 존재하지 않는 경우
         *    1) Physical Filter나 Logical Filter가 존재하는 경우
         *       => Filter의 결과가 TRUE이고 Key의 비교 결과가
         *          TRUE나 UNKNOWN이 하나도 없는 경우 결과 반환
         *    2) Physical Filter나 Logical Filter가 존재하지 않는 경우
         *       => Key의 비교 결과가 TRUE나 UNKNOWN이 하나도 없는 경우 결과 반환
         */


        /**
         * 동일한 Hash Value 를 갖는 Key 들에 대해 Hash Join 수행
         */

        sHasMatchRow = STL_FALSE;
        sJoinFetchInfo->mReady4JoinFilter = STL_FALSE;

        if( (sLeftIncludeNull == STL_FALSE) ||
            ( (aFetchInfo->mPhysicalFilter == NULL) &&
              (aFetchInfo->mLogicalFilter == NULL) ) )
        {
            /**
             * A-2, B-1, B-2 case
             */

            STL_TRY( smnmihSetNextRightRow( sIterator,
                                            sIndexHeader,
                                            aFetchInfo,
                                            &sHasMatchRow,
                                            &sMatchKeyLa,
                                            &sMatchKey,
                                            aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * A-1 case
             */

            STL_TRY( smnmihFullScanWithRowFilter( sIterator,
                                                  sIndexHeader,
                                                  aFetchInfo,
                                                  &sHasMatchRow,
                                                  aEnv )
                     == STL_SUCCESS );
        }

        /**
         * 만족하는 right row 가 없다면 result 를 구성함.
         */

        if( sHasMatchRow == STL_FALSE )
        {
            /**
             * Result Block 구성
             */

            STL_TRY( knlSetJoinResultBlock( sJoinFetchInfo->mResultRelationList,
                                            sJoinFetchInfo->mResultBlockIdx,
                                            sJoinFetchInfo->mLeftBlockIdx,
                                            sJoinFetchInfo->mResultBlockIdx, /* right block idx */
                                            KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * 남은 Result Join 조건에 부합하는지 검사
             */

            sHasMatchRow = STL_TRUE;
            if ( sJoinFetchInfo->mResultFilter != NULL )
            {
                /**
                 * Result Filter 를 위한 Left 상수 구성
                 */

                if ( sJoinFetchInfo->mReady4JoinFilter == STL_FALSE )
                {
                    smeSetJoinFilter4InstRelation( sJoinFetchInfo );
                    smeSetPhysicalFilter4InstRelation( aFetchInfo );
                    sJoinFetchInfo->mReady4JoinFilter = STL_TRUE;
                }
                
                STL_TRY( knlEvaluateJoinFilter( sJoinFetchInfo->mResultFilter,
                                                sJoinFetchInfo->mResultBlockIdx,
                                                & sHasMatchRow,
                                                KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }

            /* anti semi join 조건을 만족하는 left row 임 */
            if ( sHasMatchRow == STL_TRUE )
            {
                if( aFetchInfo->mSkipCnt > 0 )
                {
                    aFetchInfo->mSkipCnt--;
                }
                else
                {
                    sJoinFetchInfo->mResultRefLeftBlockIdx[sJoinFetchInfo->mResultBlockIdx]
                        = sJoinFetchInfo->mLeftBlockIdx;
                    sJoinFetchInfo->mResultBlockIdx++;
                    aFetchInfo->mFetchCnt--;

                    if( aFetchInfo->mFetchCnt == 0 )
                    {
                        aFetchInfo->mIsEndOfScan = STL_TRUE;
                        STL_THROW( RAMP_RESULT_FULL );
                    }
                }
            }
        }
        
        sJoinFetchInfo->mLeftBlockIdx++;
        
        /**
         * Result Block 이 모두 채워졌는지 검사 
         */
        
        if ( sJoinFetchInfo->mResultBlockIdx == sResultBlockSize )
        {
            /* result block 이 모두 채워짐 */
            aFetchInfo->mIsEndOfScan = STL_FALSE;
            STL_THROW( RAMP_RESULT_FULL );
            break;
        }
        else
        {
            continue;
        }
    }

    /* left block 에 대한 처리가 모두 끝남 */
    sJoinFetchInfo->mLeftBlockIdx = 0;
    aFetchInfo->mIsEndOfScan = STL_TRUE;
    
    STL_RAMP( RAMP_RESULT_FULL );

    SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sJoinFetchInfo->mResultBlockIdx );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 다음 Right Row 를 설정
 * @param[in]  aIterator       Hash Iterator
 * @param[in]  aIndexHeader    Hash Instance Table Header
 * @param[in]  aFetchInfo      Fetch Information
 * @param[out] aHasMatchRow    Match Row 의 존재 여부
 * @param[out] aMatchKeyLa     Match Key의 Logical Address
 * @param[out] aMatchKey       Match Key Pointer
 * @param[in]  aEnv            Environment
 */
stlStatus smnmihSetNextRightRow( smnmihIterator    * aIterator,
                                 smnmihIndexHeader * aIndexHeader,
                                 smlFetchInfo      * aFetchInfo,
                                 stlBool           * aHasMatchRow,
                                 knlLogicalAddr    * aMatchKeyLa,
                                 stlChar          ** aMatchKey,
                                 smlEnv            * aEnv )
{
    knlLogicalAddr          sKeyLa = aIterator->mNextKeyLa;
    knlLogicalAddr          sNextKeyLa;
    stlUInt32               sFilterHashKey = aIterator->mFilterHashKey;
    stlChar               * sKey;
    stlChar               * sRow;
    stlUInt32               sHashKey;
    stlBool                 sMatched;
    knlLogicalAddr          sRowLa;
    smlBlockJoinFetchInfo * sJoinFetchInfo;

    sJoinFetchInfo = aFetchInfo->mBlockJoinFetchInfo;
    *aHasMatchRow = STL_FALSE;
    
    while( sKeyLa != KNL_LOGICAL_ADDR_NULL )
    {
        sKey = KNL_TO_PHYSICAL_ADDR( sKeyLa );
        SMNMIH_GET_HASH_KEY( sKey, sHashKey );
        SMNMIH_GET_OVERFLOW_KEY( sKey, sNextKeyLa );
            
        if( sHashKey != sFilterHashKey )
        {
            sKeyLa = sNextKeyLa;
            continue;
        }

        SMNMIH_GET_ROWID( sKey, sRowLa );
        sRow = KNL_TO_PHYSICAL_ADDR( sRowLa );
        
        sMatched = smnmihCompareKey( aIndexHeader,
                                     sRow,
                                     STL_FALSE, /* NULL AWARE */
                                     sJoinFetchInfo->mKeyConstList );

        if( sMatched == STL_FALSE )
        {
            sKeyLa = sNextKeyLa;
            continue;
        }
        
        STL_TRY( smdmifTableFetchOneRowWithRowFilter( aIndexHeader->mBaseHeader,
                                                      sRow,
                                                      aFetchInfo->mPhysicalFilter,
                                                      aFetchInfo->mFilterEvalInfo,
                                                      aFetchInfo->mColList,
                                                      sJoinFetchInfo->mResultBlockIdx,
                                                      &sMatched,
                                                      aEnv ) == STL_SUCCESS );
        
        if( sMatched == STL_FALSE )
        {
            sKeyLa = sNextKeyLa;
            continue;
        }

        *aHasMatchRow = STL_TRUE;
        *aMatchKey = sKey;
        *aMatchKeyLa = sKeyLa;
        break;
    }

    if ( *aHasMatchRow == STL_TRUE )
    {
        aIterator->mNextKeyLa = sNextKeyLa;
    }
    else
    {
        aIterator->mNextKeyLa = KNL_LOGICAL_ADDR_NULL;

        if( sJoinFetchInfo->mNullAwareComparison == STL_TRUE )
        {
            /**
             * Null aware comparision이 필요한 경우
             */
        
            STL_TRY( smnmihNullAwareCompare( aIndexHeader,
                                             aFetchInfo,
                                             sJoinFetchInfo->mKeyConstList,
                                             STL_TRUE,    /* aFirstCompare */
                                             aHasMatchRow,
                                             aMatchKey,
                                             aEnv )
                     == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smnmihNullAwareCompare( smnmihIndexHeader  * aIndexHeader,
                                  smlFetchInfo       * aFetchInfo,
                                  knlDataValueList   * aHashConstList,
                                  stlBool              aFirstCompare,
                                  stlBool            * aMatchRow,
                                  stlChar           ** aMatchKey,
                                  smlEnv             * aEnv )
{
    stlInt64                sLength;
    knlValueBlockList     * sHashBlockList;
    knlDataValueList      * sHashConstList;
    stlUInt32               sFilterHashKey = 0;
    stlChar               * sKey;
    stlUInt32               sHashKey;
    knlLogicalAddr          sKeyLa = KNL_LOGICAL_ADDR_NULL;
    knlLogicalAddr          sNextKeyLa = KNL_LOGICAL_ADDR_NULL;
    smnmihDirSlot         * sDirSlot;
    stlInt32                sBucketIdx;
    knlLogicalAddr          sRowLa;
    stlChar               * sRow;
    stlBool                 sMatched;
    smlBlockJoinFetchInfo * sJoinFetchInfo;

    if( aHashConstList == NULL )
    {
        STL_TRY_THROW( aFirstCompare == STL_FALSE, RAMP_FINISH );
        
        sJoinFetchInfo = aFetchInfo->mBlockJoinFetchInfo;
        sHashBlockList = sJoinFetchInfo->mKeyBlockList;
        sHashConstList = sJoinFetchInfo->mKeyConstList;
        
        while( sHashBlockList != NULL )
        {
            sHashConstList->mConstVal = KNL_GET_BLOCK_DATA_VALUE( sHashBlockList, sJoinFetchInfo->mLeftBlockIdx );
            sFilterHashKey += gDtlHash32[sHashConstList->mConstVal->mType]( sHashConstList->mConstVal->mValue,
                                                                            sHashConstList->mConstVal->mLength );
        
            sHashBlockList = sHashBlockList->mNext;
            sHashConstList = sHashConstList->mNext;
        }

        SMNMIH_DIR_SLOT( aIndexHeader, sFilterHashKey, sDirSlot, sBucketIdx );
        sKeyLa = sDirSlot->mKeyLa;

        while( sKeyLa != KNL_LOGICAL_ADDR_NULL )
        {
            sKey = KNL_TO_PHYSICAL_ADDR( sKeyLa );
            SMNMIH_GET_HASH_KEY( sKey, sHashKey );
            SMNMIH_GET_OVERFLOW_KEY( sKey, sNextKeyLa );
        
            if( sHashKey != sFilterHashKey )
            {
                sKeyLa = sNextKeyLa;
                continue;
            }

            SMNMIH_GET_ROWID( sKey, sRowLa );
            sRow = KNL_TO_PHYSICAL_ADDR( sRowLa );
        
            sMatched = smnmihCompareKey( aIndexHeader,
                                         sRow,
                                         STL_TRUE, /* NULL AWARE */
                                         sJoinFetchInfo->mKeyConstList );

            if( sMatched == STL_FALSE )
            {
                sKeyLa = sNextKeyLa;
                continue;
            }
        
            STL_TRY( smdmifTableFetchOneRowWithRowFilter( aIndexHeader->mBaseHeader,
                                                          sRow,
                                                          aFetchInfo->mPhysicalFilter,
                                                          aFetchInfo->mFilterEvalInfo,
                                                          aFetchInfo->mColList,
                                                          sJoinFetchInfo->mResultBlockIdx,
                                                          &sMatched,
                                                          aEnv ) == STL_SUCCESS );
        
            if( sMatched == STL_FALSE )
            {
                sKeyLa = sNextKeyLa;
                continue;
            }

            *aMatchRow = STL_TRUE;
            *aMatchKey = sKey;
            break;
        }

        if( sKeyLa != KNL_LOGICAL_ADDR_NULL )
        {
            *aMatchRow = STL_TRUE;
        }
    }
    else
    {
        STL_TRY( smnmihNullAwareCompare( aIndexHeader,
                                         aFetchInfo,
                                         aHashConstList->mNext,
                                         aFirstCompare,
                                         aMatchRow,
                                         aMatchKey,
                                         aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( *aMatchRow == STL_FALSE, RAMP_FINISH );

        sLength = aHashConstList->mConstVal->mLength;
        aHashConstList->mConstVal->mLength = 0;
        
        STL_TRY( smnmihNullAwareCompare( aIndexHeader,
                                         aFetchInfo,
                                         aHashConstList->mNext,
                                         STL_FALSE,
                                         aMatchRow,
                                         aMatchKey,
                                         aEnv )
                 == STL_SUCCESS );
        
        aHashConstList->mConstVal->mLength = sLength;
        
        STL_TRY_THROW( *aMatchRow == STL_FALSE, RAMP_FINISH );
    }
    
    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlBool smnmihCompareKey( smnmihIndexHeader  * aIndexHeader,
                          stlChar            * aRow,
                          stlBool              aAwareNull,
                          knlDataValueList   * aDataValueList )
{
    knlDataValueList  * sDataValueList;
    stlInt64            sLength1;
    stlInt64            sLength2;
    stlInt32            i;
    stlInt32            j;
    stlChar           * sValue1;
    stlChar           * sValue2;
    smdmifDefColumn   * sColumn;
    smdmifFixedRow      sFixedRow = (smdmifFixedRow)aRow;
    
    sDataValueList = aDataValueList;
    
    for( i = 0; i < aIndexHeader->mKeyColCount; i++ )
    {
        sColumn = aIndexHeader->mRowColumn[i];
        
        smdmifTableGetColValueAndLen( sFixedRow,
                                      sColumn,
                                      aIndexHeader->mColCombineMem,
                                      (stlChar**)&sValue1,
                                      &sLength1 );

        sLength2 = sDataValueList->mConstVal->mLength;

        if( ( sDataValueList->mConstVal->mType == DTL_TYPE_CHAR ) &&
            ( sLength1 > 0 ) &&
            ( sLength2 > 0 ) )
        {
            /**
             * CHAR type은 space padding을 제거한 상태로 값을 비교한다.
             */

            if( sValue1[ sLength1 - 1 ] == ' ' )
            {
                while( sLength1 > 0 )
                {
                    sLength1--;
                    if( sValue1[ sLength1 ] != ' ' )
                    {
                        sLength1++;
                        break;
                    }
                }
            }

            sValue2 = (stlChar*) sDataValueList->mConstVal->mValue;
            if( sValue2[ sLength2 - 1 ] == ' ' )
            {
                while( sLength2 > 0 )
                {
                    sLength2--;
                    if( sValue2[ sLength2 ] != ' ' )
                    {
                        sLength2++;
                        break;
                    }
                }
            }
        }
        else if( ( sDataValueList->mConstVal->mType == DTL_TYPE_BINARY ) &&
                 ( sLength1 > 0 ) &&
                 ( sLength2 > 0 ) )
        {
            /**
             * BINARY type은 space padding을 제거한 상태로 값을 비교한다.
             */

            if( sValue1[ sLength1 - 1 ] == 0x00 )
            {
                while( sLength1 > 0 )
                {
                    sLength1--;
                    if( sValue1[ sLength1 ] != 0x00 )
                    {
                        sLength1++;
                        break;
                    }
                }
            }

            sValue2 = (stlChar*) sDataValueList->mConstVal->mValue;
            if( sValue2[ sLength2 - 1 ] == 0x00 )
            {
                while( sLength2 > 0 )
                {
                    sLength2--;
                    if( sValue2[ sLength2 ] != 0x00 )
                    {
                        sLength2++;
                        break;
                    }
                }
            }
        }

        if( sLength1 == sLength2 )
        {
            if( sLength1 != 0 )
            {
                sValue2 = sDataValueList->mConstVal->mValue;

                sLength1 -= gDtlCompValueIgnoreSize[ sDataValueList->mConstVal->mType ];
                
                for( j = 0 ; j < sLength1; j++ )
                {
                    if( *sValue2 != *sValue1 )
                    {
                        return STL_FALSE;
                    }
                    sValue2++;
                    sValue1++;
                }
            }
            else
            {
                /* Null Aware Anti Semi Join인 경우에는 length가 0이면
                 * 같은 데이터로 취급하며, 그 이외에는 FALSE로 취급한다. */
                if( aAwareNull == STL_FALSE )
                {
                    return STL_FALSE;
                }
            }
        }
        else
        {
            return STL_FALSE;
        }

        sDataValueList = sDataValueList->mNext;
    }

    return STL_TRUE;
}


/**
 * @brief 하나의 Left Row 에 대해 Hash Iterator의 Before First 를 수행한다.
 * @param[in]  aIterator         Hash Iterator
 * @param[in]  aIndexHeader      Hash Instance Table Header
 * @param[in]  aJoinFetchInfo    Join Fetch Information
 * @param[out] aLeftIncludeNull  Hash Const List에 Null이 포함되어 있는지 여부
 */
void smnmihBeforeFirst( smnmihIterator        * aIterator,
                        smnmihIndexHeader     * aIndexHeader,
                        smlBlockJoinFetchInfo * aJoinFetchInfo,
                        stlBool               * aLeftIncludeNull )
{
    stlUInt32             sFilterHashKey = 0;
    knlValueBlockList   * sHashBlockList = aJoinFetchInfo->mKeyBlockList;
    knlDataValueList    * sHashConstList = aJoinFetchInfo->mKeyConstList;
    stlUInt64             sBucketIdx;
    smnmihDirSlot       * sDirSlot;

    if( aLeftIncludeNull != NULL )
    {
        *aLeftIncludeNull = STL_FALSE;
    }
    
    while( sHashBlockList != NULL )
    {
        sHashConstList->mConstVal = KNL_GET_BLOCK_DATA_VALUE( sHashBlockList, aJoinFetchInfo->mLeftBlockIdx );

        if( sHashConstList->mConstVal->mLength == 0 )
        {
            aIterator->mNextKeyLa = KNL_LOGICAL_ADDR_NULL;

            if( aLeftIncludeNull != NULL )
            {
                *aLeftIncludeNull = STL_TRUE;
            }
            return;
        }
        
        sFilterHashKey += gDtlHash32[sHashConstList->mConstVal->mType]( sHashConstList->mConstVal->mValue,
                                                                        sHashConstList->mConstVal->mLength );
        
        sHashBlockList = sHashBlockList->mNext;
        sHashConstList = sHashConstList->mNext;
    }

    aIterator->mFilterHashKey = sFilterHashKey;

    SMNMIH_DIR_SLOT( aIndexHeader, sFilterHashKey, sDirSlot, sBucketIdx );
    aIterator->mNextKeyLa = sDirSlot->mKeyLa;

    if( sDirSlot->mOverflowCount == 1 )
    {
        if( sDirSlot->mLastHashKey != sFilterHashKey )
        {
            aIterator->mNextKeyLa = KNL_LOGICAL_ADDR_NULL;
        }
    }
}

stlStatus smnmihFetchNextUnhitRow( smnmihIterator    * aIterator,
                                   smnmihIndexHeader * aIndexHeader,
                                   smlFetchInfo      * aFetchInfo,
                                   stlBool           * aHasMatchRow,
                                   smlEnv            * aEnv )
{
    stlChar         * sKey;
    knlLogicalAddr    sRowLa;
    stlChar         * sRow;
    stlBool           sMatched;
    smnmihDirSlot   * sDirSlot;
    
    
    *aHasMatchRow = STL_FALSE;
    
    while( 1 )
    {
        while( aIterator->mNextKeyLa != KNL_LOGICAL_ADDR_NULL )
        {
            sKey = KNL_TO_PHYSICAL_ADDR( aIterator->mNextKeyLa );
            SMNMIH_GET_OVERFLOW_KEY( sKey, aIterator->mNextKeyLa );

            if( SMNMIH_GET_FETCH_BIT( sKey ) == STL_TRUE )
            {
                continue;
            }
            
            SMNMIH_GET_ROWID( sKey, sRowLa );
            sRow = KNL_TO_PHYSICAL_ADDR( sRowLa );

            STL_TRY( smdmifTableFetchOneRowWithRowFilter( aIndexHeader->mBaseHeader,
                                                          sRow,
                                                          NULL,   /* aRowPhysicalFilter */
                                                          NULL,   /* aRowLogicalFilterInfo */
                                                          aFetchInfo->mColList,
                                                          aFetchInfo->mBlockJoinFetchInfo->mResultBlockIdx,
                                                          &sMatched,
                                                          aEnv ) == STL_SUCCESS );
            
            *aHasMatchRow = STL_TRUE;
            
            STL_THROW( RAMP_FINISH );
        }

        aIterator->mFetchDirIdx++;

        if( aIterator->mFetchDirIdx >= aIndexHeader->mBucketCount )
        {
            break;
        }
            
        sDirSlot = &(aIndexHeader->mDirectory[aIterator->mFetchDirIdx]);
        aIterator->mNextKeyLa = sDirSlot->mKeyLa;
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smnmihResetHitKeys( smnmihIterator    * aIterator,
                              smnmihIndexHeader * aIndexHeader,
                              smlEnv            * aEnv )
{
    stlInt64          i;
    knlLogicalAddr    sKeyLa;
    stlChar         * sKey;
    smnmihDirSlot   * sDirSlot;
    
    if( aIterator->mHitKeyCount > SMNMIH_MAX_HIT_KEY_HINT_COUNT )
    {
        for( i = 0; i < aIndexHeader->mBucketCount; i++ )
        {
            sDirSlot = &(aIndexHeader->mDirectory[i]);
            sKeyLa = sDirSlot->mKeyLa;

            while( sKeyLa != KNL_LOGICAL_ADDR_NULL )
            {
                sKey = KNL_TO_PHYSICAL_ADDR( sKeyLa );
                
                if( SMNMIH_GET_FETCH_BIT( sKey ) == STL_TRUE )
                {
                    SMNMIH_SET_FETCH_BIT( sKey, STL_FALSE );
                }

                SMNMIH_GET_OVERFLOW_KEY( sKey, sKeyLa );
            }
        }
    }
    else
    {
        for( i = 0; i < aIterator->mHitKeyCount; i++ )
        {
            sKeyLa = aIterator->mHitKeyLa[i];

            sKey = KNL_TO_PHYSICAL_ADDR( sKeyLa );
            SMNMIH_SET_FETCH_BIT( sKey, STL_FALSE );
        }
    }

    return STL_SUCCESS;
}


stlStatus smnmihFullScanWithRowFilter( smnmihIterator    * aIterator,
                                       smnmihIndexHeader * aIndexHeader,
                                       smlFetchInfo      * aFetchInfo,
                                       stlBool           * aIsMatched,
                                       smlEnv            * aEnv )
{
    stlInt64                i;
    knlLogicalAddr          sKeyLa;
    stlChar               * sKey;
    smnmihDirSlot         * sDirSlot;
    stlChar               * sRow;
    knlLogicalAddr          sRowLa;
    smlBlockJoinFetchInfo * sJoinFetchInfo;
    
    sJoinFetchInfo = aFetchInfo->mBlockJoinFetchInfo;
    
    for( i = 0; i < aIndexHeader->mBucketCount; i++ )
    {
        sDirSlot = &(aIndexHeader->mDirectory[i]);
        sKeyLa = sDirSlot->mKeyLa;

        while( sKeyLa != KNL_LOGICAL_ADDR_NULL )
        {
            sKey = KNL_TO_PHYSICAL_ADDR( sKeyLa );
            SMNMIH_GET_ROWID( sKey, sRowLa );
            sRow = KNL_TO_PHYSICAL_ADDR( sRowLa );
                
            STL_TRY( smdmifTableFetchOneRowWithRowFilter( aIndexHeader->mBaseHeader,
                                                          sRow,
                                                          aFetchInfo->mPhysicalFilter,
                                                          aFetchInfo->mFilterEvalInfo,
                                                          aFetchInfo->mColList,
                                                          sJoinFetchInfo->mResultBlockIdx,
                                                          aIsMatched,
                                                          aEnv ) == STL_SUCCESS );
            
            if( *aIsMatched == STL_TRUE )
            {
                break;
            }
            
            SMNMIH_GET_OVERFLOW_KEY( sKey, sKeyLa );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */

