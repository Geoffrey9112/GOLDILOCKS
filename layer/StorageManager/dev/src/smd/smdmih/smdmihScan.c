/*******************************************************************************
 * smdmihScan.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmihScan.c 6124 2012-10-29 10:31:50Z htkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smdmihScan.c
 * @brief Storage Manager Layer Memory Instant Hash Table Scan Routines
 */

#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <sml.h>
#include <smDef.h>
#include <sma.h>
#include <sme.h>
#include <smo.h>
#include <smdmifTable.h>
#include <smdmihDef.h>
#include <smdmihScan.h>
#include <smdmih.h>
#include <smdmifVarPart.h>

/**
 * @addtogroup smdmihScan
 * @{
 */

smeIteratorModule gSmdmihIteratorModule =
{
    STL_SIZEOF(smdmihIterator),

    smdmihInitIterator,
    NULL,   /* FiniIterator */
    smdmihResetIterator,
    NULL,   /* MoveToRowId */
    NULL,   /* MoveToPosNum */
    {

        smdmihFetchNext,
        NULL,   /* FetchPrev */
        (smiFetchAggrFunc)smdmihFetchNext,
        NULL    /* FetchSampling */
    }
};

/**
 * @brief 주어진 Iterator를 초기화한다
 * @param[in,out] aIterator 초기화할 iterator
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmihInitIterator( void   * aIterator,
                              smlEnv * aEnv )
{
    smdmihIterator    * sIterator;

    sIterator = (smdmihIterator*)aIterator;
    
    SMDMIH_INIT_ITERATOR( sIterator );

    STL_DASSERT( smdmihValidate( (smdmihTableHeader*)sIterator->mCommon.mRelationHandle,
                                 aEnv )
                 == STL_SUCCESS );

    return STL_SUCCESS;
}

/**
 * @brief 주어진 Iterator를 다시 초기화한다
 * @param[in,out] aIterator         초기화할 iterator
 * @param[in]     aEnv              Storage Manager Environment
 */
stlStatus smdmihResetIterator( void   * aIterator,
                               smlEnv * aEnv )
{
    smdmihIterator * sIterator = (smdmihIterator *)aIterator;

    SMDMIH_RESET_ITERATOR( sIterator );

    return STL_SUCCESS;
}

/**
 * @brief 일반 Scan을 위해 열린 Iterator에서 다음 row를 반환한다
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 */
stlStatus smdmihFetchNext( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv )
{
    if( aFetchInfo->mBlockJoinFetchInfo != NULL )
    {
        if( aFetchInfo->mBlockJoinFetchInfo->mBlockJoinType ==
            KNL_BLOCK_JOIN_SEMI_JOIN )
        {
            STL_TRY( smdmihFetchNext4SemiJoin( aIterator,
                                               aFetchInfo,
                                               aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smdmihFetchNext4AntiSemiJoin( aIterator,
                                                   aFetchInfo,
                                                   aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        STL_TRY( smdmihFetchNext4FullScan( aIterator,
                                           aFetchInfo,
                                           aEnv )
                 == STL_SUCCESS );
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
stlStatus smdmihFetchNext4FullScan( void          * aIterator,
                                    smlFetchInfo  * aFetchInfo,
                                    smlEnv        * aEnv )
{
    smdmihIterator      * sIterator = (smdmihIterator *)aIterator;
    smdmihTableHeader   * sTableHeader = (smdmihTableHeader*)sIterator->mCommon.mRelationHandle;
    stlInt64              sBlockSize = SML_BLOCK_SIZE( aFetchInfo->mRowBlock );
    stlInt32              sBlockIdx  = 0;
    knlLogicalAddr        sKeyLa;
    stlChar             * sKey;
    smlRid              * sRid;
    stlBool               sFetched;
    stlInt32              sAggrRowCount = 0;
    smdmihDirSlot       * sDirSlot;


    STL_PARAM_VALIDATE( aFetchInfo->mBlockJoinFetchInfo == NULL, KNL_ERROR_STACK( aEnv ) );
    
    SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );

    aFetchInfo->mIsEndOfScan = STL_FALSE;
    
    if( sIterator->mDirectory == NULL )
    {
        sIterator->mDirectory = sTableHeader->mDirectory;
        sIterator->mDirIdx = sTableHeader->mStartBucketIdx - 1;
    }

    while( 1 )
    {
        sKeyLa = sIterator->mNextKeyLa;

        if( (aFetchInfo->mSkipCnt == 0) &&
            (aFetchInfo->mFetchCnt == 0 || sBlockIdx >= sBlockSize ) )
        {
            break;
        }

        while( sKeyLa == KNL_LOGICAL_ADDR_NULL )
        {
            sIterator->mDirIdx += 1; 
            
            if( sIterator->mDirIdx > sTableHeader->mEndBucketIdx )
            {
                aFetchInfo->mIsEndOfScan = STL_TRUE;
                STL_THROW( RAMP_FINISH );
            }

            sDirSlot = &(sIterator->mDirectory[sIterator->mDirIdx]);
            sKeyLa = sDirSlot->mKeyLa;
        }

        sKey = KNL_TO_PHYSICAL_ADDR( sKeyLa );
        SMDMIH_GET_OVERFLOW_KEY( sKey, sIterator->mNextKeyLa );
        
        STL_TRY( smdmihFetchOneRow( sTableHeader,
                                    sKey,
                                    aFetchInfo,
                                    sBlockIdx,
                                    &sFetched,
                                    aEnv ) == STL_SUCCESS );

        if( sFetched == STL_FALSE )
        {
            continue;
        }

        if( aFetchInfo->mAggrFetchInfo != NULL )
        {
            sBlockIdx++;
            
            if( sBlockIdx == sBlockSize )
            {
                STL_TRY( smoBlockBuildAggrFunc( aFetchInfo->mAggrFetchInfo,
                                                0, /* aStartBlockIdx */
                                                sBlockSize,
                                                aEnv )
                         == STL_SUCCESS );
                sBlockIdx = 0;
                sAggrRowCount = 1;
            }
        }
        else
        {
            if( aFetchInfo->mSkipCnt > 0 )
            {
                aFetchInfo->mSkipCnt--;
                continue;
            }

            sRid = (smlRid *)(&sKeyLa);
            SML_SET_RID_VALUE( aFetchInfo->mRowBlock, sBlockIdx, *sRid );
        
            sBlockIdx++;
            aFetchInfo->mFetchCnt--;
        }
    }

    STL_RAMP( RAMP_FINISH );

    if( aFetchInfo->mAggrFetchInfo == NULL )
    {
        SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sBlockIdx );

        if( aFetchInfo->mColList != NULL )
        {
            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aFetchInfo->mColList, 0, sBlockIdx );
        }
    }
    else
    {
        if( sBlockIdx > 0 )
        {
            STL_TRY( smoBlockBuildAggrFunc( aFetchInfo->mAggrFetchInfo,
                                            0, /* aStartBlockIdx */
                                            sBlockIdx,
                                            aEnv )
                     == STL_SUCCESS );
            sAggrRowCount = 1;
        }
        
        SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sAggrRowCount );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 하나의 Left Row 에 부합하는 레코드를 얻는다.
 * @param[in]  aIterator       Hash Iterator
 * @param[in]  aTableHeader    Hash Instance Table Header
 * @param[in]  aFetchInfo      Fetch Information
 * @param[in]  aMatchRow       Match Row 여부
 * @param[in]  aEnv            Storage Manager Environment
 */
stlStatus smdmihFetchOneRow4SemiJoin( smdmihIterator    * aIterator,
                                      smdmihTableHeader * aTableHeader,
                                      smlFetchInfo      * aFetchInfo,
                                      stlBool           * aMatchRow,
                                      smlEnv            * aEnv )
{
    stlUInt32               sFilterHashKey = 0;
    knlValueBlockList     * sHashBlockList;
    knlDataValueList      * sHashConstList;
    smdmihDirSlot         * sDirSlot;
    knlLogicalAddr          sKeyLa = KNL_LOGICAL_ADDR_NULL;
    knlLogicalAddr          sNextKeyLa = KNL_LOGICAL_ADDR_NULL;
    stlChar               * sKey;
    stlUInt32               sHashKey;
    smlBlockJoinFetchInfo * sJoinFetchInfo;
    stlInt32                sBucketIdx;

    *aMatchRow = STL_FALSE;

    sJoinFetchInfo = aFetchInfo->mBlockJoinFetchInfo;
    sHashBlockList = sJoinFetchInfo->mKeyBlockList;
    sHashConstList = sJoinFetchInfo->mKeyConstList;
    
    while( sHashBlockList != NULL )
    {
        sHashConstList->mConstVal = KNL_GET_BLOCK_DATA_VALUE( sHashBlockList, sJoinFetchInfo->mLeftBlockIdx );

        STL_TRY_THROW( sHashConstList->mConstVal->mLength > 0, RAMP_FINISH );

        sFilterHashKey += gDtlHash32[sHashConstList->mConstVal->mType]( sHashConstList->mConstVal->mValue,
                                                                        sHashConstList->mConstVal->mLength );
        
        sHashBlockList = sHashBlockList->mNext;
        sHashConstList = sHashConstList->mNext;
    }

    SMDMIH_DIR_SLOT( aTableHeader, sFilterHashKey, sDirSlot, sBucketIdx );
    sKeyLa = sDirSlot->mKeyLa;
    
    while( sKeyLa != KNL_LOGICAL_ADDR_NULL )
    {
        sKey = KNL_TO_PHYSICAL_ADDR( sKeyLa );
        SMDMIH_GET_HASH_KEY( sKey, sHashKey );
        SMDMIH_GET_OVERFLOW_KEY( sKey, sNextKeyLa );
        
        if( sHashKey != sFilterHashKey )
        {
            sKeyLa = sNextKeyLa;
            continue;
        }

        if( smdmihCompareKeyAndBlock( aTableHeader,
                                      sJoinFetchInfo->mKeyBlockList,
                                      sJoinFetchInfo->mLeftBlockIdx,
                                      sKey )
            == STL_FALSE )
        {
            sKeyLa = sNextKeyLa;
            continue;
        }

        break;
    }

    *aMatchRow = ( sKeyLa != KNL_LOGICAL_ADDR_NULL );
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;
}


/**
 * @brief 하나의 Left Row 에 부합하지 않는 레코드를 얻는다.
 * @param[in]  aIterator       Hash Iterator
 * @param[in]  aTableHeader    Hash Instance Table Header
 * @param[in]  aFetchInfo      Fetch Information
 * @param[in]  aMatchRow       Match Row 여부
 * @param[in]  aEnv            Storage Manager Environment
 */
stlStatus smdmihFetchOneRow4AntiSemiJoin( smdmihIterator      * aIterator,
                                          smdmihTableHeader   * aTableHeader,
                                          smlFetchInfo        * aFetchInfo,
                                          stlBool             * aMatchRow,
                                          smlEnv              * aEnv )
{
    stlUInt32               sFilterHashKey = 0;
    knlValueBlockList     * sHashBlockList;
    knlDataValueList      * sHashConstList;
    smdmihDirSlot         * sDirSlot;
    knlLogicalAddr          sKeyLa = KNL_LOGICAL_ADDR_NULL;
    knlLogicalAddr          sNextKeyLa = KNL_LOGICAL_ADDR_NULL;
    stlChar               * sKey;
    stlUInt32               sHashKey;
    smlBlockJoinFetchInfo * sJoinFetchInfo;
    stlInt32                sBucketIdx;

    *aMatchRow = STL_TRUE;

    sJoinFetchInfo = aFetchInfo->mBlockJoinFetchInfo;
    sHashBlockList = sJoinFetchInfo->mKeyBlockList;
    sHashConstList = sJoinFetchInfo->mKeyConstList;
    
    /*
     * Anti Semi Join은 모든 Record 가 주어진 Key 값과 일치하지 않는 경우 Join을 수행한다.
     * 또한, no rows인 경우 무조건 Join을 수행한다.
     *
     * Join 조건이 성립되지 않는 경우부터 찾는 것이 효율적이기 때문에,
     * 먼저 조건과 일치(equal)하는 record를 찾는다. => "<> ALL" 조건을 만족하지 않는 경우 (aMatchRow = TRUE)
     *
     * 예제) ( 1, 2, 3 ) <> ALL ( SELECT I1, I2, I3 FROM T1 )
     *       => ( 1, 2, 3 ) 을 key 값으로 하는 record를 찾는다.
     */

    if( aTableHeader->mRowCount == 0 )
    {
        /* no rows이므로 aMatchRow를 FALSE로 하고 종료한다. */
        *aMatchRow = STL_FALSE;
        STL_THROW( RAMP_FINISH );
    }

    while( sHashBlockList != NULL )
    {
        sHashConstList->mConstVal = KNL_GET_BLOCK_DATA_VALUE( sHashBlockList, sJoinFetchInfo->mLeftBlockIdx );

        STL_TRY_THROW( sHashConstList->mConstVal->mLength > 0, RAMP_FINISH );
                
        sFilterHashKey += gDtlHash32[sHashConstList->mConstVal->mType]( sHashConstList->mConstVal->mValue,
                                                                        sHashConstList->mConstVal->mLength );
        
        sHashBlockList = sHashBlockList->mNext;
        sHashConstList = sHashConstList->mNext;
    }

    SMDMIH_DIR_SLOT( aTableHeader, sFilterHashKey, sDirSlot, sBucketIdx );
    sKeyLa = sDirSlot->mKeyLa;
    
    while( sKeyLa != KNL_LOGICAL_ADDR_NULL )
    {
        sKey = KNL_TO_PHYSICAL_ADDR( sKeyLa );
        SMDMIH_GET_HASH_KEY( sKey, sHashKey );
        SMDMIH_GET_OVERFLOW_KEY( sKey, sNextKeyLa );
        
        if( sHashKey != sFilterHashKey )
        {
            sKeyLa = sNextKeyLa;
            continue;
        }

        if( smdmihCompareKeyAndBlock( aTableHeader,
                                      sJoinFetchInfo->mKeyBlockList,
                                      sJoinFetchInfo->mLeftBlockIdx,
                                      sKey )
            == STL_FALSE )
        {
            sKeyLa = sNextKeyLa;
            continue;
        }

        break;
    }

    /*
     * 조건에 부합하는 record를 찾았으면 scan을 종료하고,
     * 못찾은 경우는 NULL과의 비교가 있는지 확인한다.
     *
     * NULL값 과의 비교는 항상 비교결과가 unknown인 상황으로
     * "<> ALL" 조건을 만족하지 않는 경우이다. (aMatchRow = TRUE)
     *
     * 예제) ( 1, 2, 3 ) <> ALL ( SELECT I1, I2, I3 FROM T1 )
     *
     * 다음을 key 값에 부합하는 레코드를 찾으면 "<> ALL" 조건을 만족하지 않는 경우로 판단된다.
     *       => ( 1, 2, 3 )
     *       => ( 1, 2, null )
     *       => ( 1, null, 3 )
     *       => ( 1, null, null )
     *       => ( null, 2, 3 )
     *       => ( null, 2, null )
     *       => ( null, null, 3 )
     *       => ( null, null, null )
     *
     * 총 비교회수는 2^3 이다.
     */
    if( sKeyLa == KNL_LOGICAL_ADDR_NULL )
    {
        *aMatchRow = STL_FALSE;

        if( sJoinFetchInfo->mNullAwareComparison == STL_TRUE )
        {
            /**
             * Null aware comparision이 필요한 경우
             */
        
            STL_TRY( smdmihNullAwareCompare( aTableHeader,
                                             sJoinFetchInfo,
                                             sJoinFetchInfo->mKeyConstList,
                                             STL_TRUE,    /* aFirstCompare */
                                             aMatchRow,
                                             aEnv )
                     == STL_SUCCESS );
        }
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihNullAwareCompare( smdmihTableHeader     * aTableHeader,
                                  smlBlockJoinFetchInfo * aJoinFetchInfo,
                                  knlDataValueList      * aHashConstList,
                                  stlBool                 aFirstCompare,
                                  stlBool               * aMatchRow,
                                  smlEnv                * aEnv )
{
    stlInt64                sLength;
    knlValueBlockList     * sHashBlockList;
    knlDataValueList      * sHashConstList;
    stlUInt32               sFilterHashKey = 0;
    stlChar               * sKey;
    stlUInt32               sHashKey;
    knlLogicalAddr          sKeyLa = KNL_LOGICAL_ADDR_NULL;
    knlLogicalAddr          sNextKeyLa = KNL_LOGICAL_ADDR_NULL;
    smdmihDirSlot         * sDirSlot;
    stlInt32                sBucketIdx;

    if( aHashConstList == NULL )
    {
        STL_TRY_THROW( aFirstCompare == STL_FALSE, RAMP_FINISH );
        
        sHashBlockList = aJoinFetchInfo->mKeyBlockList;
        sHashConstList = aJoinFetchInfo->mKeyConstList;
        
        while( sHashBlockList != NULL )
        {
            sHashConstList->mConstVal = KNL_GET_BLOCK_DATA_VALUE( sHashBlockList, aJoinFetchInfo->mLeftBlockIdx );
            sFilterHashKey += gDtlHash32[sHashConstList->mConstVal->mType]( sHashConstList->mConstVal->mValue,
                                                                            sHashConstList->mConstVal->mLength );
        
            sHashBlockList = sHashBlockList->mNext;
            sHashConstList = sHashConstList->mNext;
        }

        SMDMIH_DIR_SLOT( aTableHeader, sFilterHashKey, sDirSlot, sBucketIdx );
        sKeyLa = sDirSlot->mKeyLa;

        while( sKeyLa != KNL_LOGICAL_ADDR_NULL )
        {
            sKey = KNL_TO_PHYSICAL_ADDR( sKeyLa );
            SMDMIH_GET_HASH_KEY( sKey, sHashKey );
            SMDMIH_GET_OVERFLOW_KEY( sKey, sNextKeyLa );
        
            if( sHashKey != sFilterHashKey )
            {
                sKeyLa = sNextKeyLa;
                continue;
            }

            if( smdmihCompareKeyAndBlock( aTableHeader,
                                          aJoinFetchInfo->mKeyBlockList,
                                          aJoinFetchInfo->mLeftBlockIdx,
                                          sKey )
                == STL_FALSE )
            {
                sKeyLa = sNextKeyLa;
                continue;
            }

            break;
        }

        if( sKeyLa != KNL_LOGICAL_ADDR_NULL )
        {
            *aMatchRow = STL_TRUE;
        }
    }
    else
    {
        STL_TRY( smdmihNullAwareCompare( aTableHeader,
                                         aJoinFetchInfo,
                                         aHashConstList->mNext,
                                         aFirstCompare,
                                         aMatchRow,
                                         aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( *aMatchRow == STL_FALSE, RAMP_FINISH );

        sLength = aHashConstList->mConstVal->mLength;
        aHashConstList->mConstVal->mLength = 0;
        
        STL_TRY( smdmihNullAwareCompare( aTableHeader,
                                         aJoinFetchInfo,
                                         aHashConstList->mNext,
                                         STL_FALSE,
                                         aMatchRow,
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


/**
 * @brief SEMI JOIN 의 Result Block 을 Fetch 한다.
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 * @remarks
 * - 하나의 left row 에 대해 semi join 조건을 만족하는 right row 가 존재할 경우,
 *   하나의 result row 를 생성한다.
 */
stlStatus smdmihFetchNext4SemiJoin( void          * aIterator,
                                    smlFetchInfo  * aFetchInfo,
                                    smlEnv        * aEnv )
{
    smdmihIterator        * sIterator = (smdmihIterator *)aIterator;
    smdmihTableHeader     * sTableHeader = (smdmihTableHeader*)sIterator->mCommon.mRelationHandle;
    smlBlockJoinFetchInfo * sJoinFetchInfo = aFetchInfo->mBlockJoinFetchInfo;
    stlInt64                sResultBlockSize = SML_BLOCK_SIZE( aFetchInfo->mRowBlock );
    stlInt32                sLeftUsedSize = DTL_GET_BLOCK_USED_CNT( sJoinFetchInfo->mKeyBlockList );
    stlBool                 sMatchRow;

    
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
        sJoinFetchInfo->mReady4JoinFilter = STL_FALSE;
        smeSetJoinFilter4InstRelation( sJoinFetchInfo );

        /**
         * Hash Key에 대한 Constant Expression 평가
         */

        /*
         * ex) SELECT 1
         *     FROM  T1 LEFT SEMI JOIN T2 ON T1.I1 = T2.I1
         *     WHERE T1.I2 = 100;
         *
         * => FetchInfo     : Join Condition (ON Clause) 처리 ( T1.I1 = T2.I1 )
         * => JoinFetchInfo : Join Filter (WHERE Clause) 처리 ( T1.I2 = 100 )
         */

        if( sJoinFetchInfo->mPreHashEvalInfo != NULL )
        {
            sJoinFetchInfo->mPreHashEvalInfo->mBlockIdx = sJoinFetchInfo->mLeftBlockIdx;
            
            STL_TRY( knlEvaluateOneExpression( sJoinFetchInfo->mPreHashEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        
        if ( (aFetchInfo->mPhysicalFilter != NULL) || (aFetchInfo->mLogicalFilter != NULL) )
        {
            if( sJoinFetchInfo->mPreJoinEvalInfo != NULL )
            {
                sJoinFetchInfo->mPreJoinEvalInfo->mBlockIdx = sJoinFetchInfo->mLeftBlockIdx;
            
                STL_TRY( knlEvaluateOneExpression( sJoinFetchInfo->mPreJoinEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }

            if( aFetchInfo->mFilterEvalInfo != NULL )
            {
                aFetchInfo->mFilterEvalInfo->mBlockIdx = sJoinFetchInfo->mLeftBlockIdx;
        
                STL_TRY( knlEvaluateOneExpression( aFetchInfo->mFilterEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF(
                                 aFetchInfo->mFilterEvalInfo->mResultBlock )
                             == STL_TRUE );
            
                if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_SEP_DATA_VALUE(
                                              aFetchInfo->mFilterEvalInfo->mResultBlock,
                                              sJoinFetchInfo->mLeftBlockIdx ) ) )
                {
                    sJoinFetchInfo->mLeftBlockIdx++;
                    continue;
                }
            }
            
            smeSetPhysicalFilter4InstRelation( aFetchInfo );
        }

        /* Join Condition (ON Clause) 처리 */
        STL_TRY( smdmihFetchOneRow4SemiJoin( sIterator,
                                             sTableHeader,
                                             aFetchInfo,
                                             &sMatchRow,
                                             aEnv )
                 == STL_SUCCESS );
        
        /* 
         * SEMI JOIN 인 경우
         *  : Key와 일치하는 Record가 있으면 Result Filter 검사
         */
        if( sMatchRow == STL_TRUE )
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
                                            
            /* semi join 조건을 만족하는 left row 임 */
            if ( sJoinFetchInfo->mResultFilter == NULL )
            {
                sMatchRow = STL_TRUE;
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

                /* Join Filter (WHERE Clause) 처리 */
                STL_TRY( knlEvaluateJoinFilter( sJoinFetchInfo->mResultFilter,
                                                sJoinFetchInfo->mResultBlockIdx,
                                                & sMatchRow,
                                                KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }

            /* Result Filter 조건을 만족하는 경우 결과로 내보냄 */
            if( sMatchRow == STL_TRUE )
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
                        sJoinFetchInfo->mLeftBlockIdx++;
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
 * - 하나의 left row 에 대해 anti semi join 조건을 만족하는 right row 가 존재할 경우,
 *   하나의 result row 를 생성한다.
 */
stlStatus smdmihFetchNext4AntiSemiJoin( void          * aIterator,
                                        smlFetchInfo  * aFetchInfo,
                                        smlEnv        * aEnv )
{
    smdmihIterator        * sIterator = (smdmihIterator *)aIterator;
    smdmihTableHeader     * sTableHeader = (smdmihTableHeader*)sIterator->mCommon.mRelationHandle;
    smlBlockJoinFetchInfo * sJoinFetchInfo = aFetchInfo->mBlockJoinFetchInfo;
    stlInt64                sResultBlockSize = SML_BLOCK_SIZE( aFetchInfo->mRowBlock );
    stlInt32                sLeftUsedSize = DTL_GET_BLOCK_USED_CNT( sJoinFetchInfo->mKeyBlockList );
    stlBool                 sMatchRow;
    
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
        sJoinFetchInfo->mReady4JoinFilter = STL_FALSE;
        smeSetJoinFilter4InstRelation( sJoinFetchInfo );

        /**
         * Hash Key에 대한 Constant Expression 평가
         */

        /*
         * ex) SELECT 1
         *     FROM  T1 LEFT ANTI SEMI JOIN T2 ON T1.I1 = T2.I1
         *     WHERE T1.I2 = 100;
         *     
         * => FetchInfo     : Join Condition (ON Clause) 처리 ( T1.I1 = T2.I1 )
         * => JoinFetchInfo : Join Filter (WHERE Clause) 처리 ( T1.I2 = 100 )
         */

        if( sJoinFetchInfo->mPreHashEvalInfo != NULL )
        {
            sJoinFetchInfo->mPreHashEvalInfo->mBlockIdx = sJoinFetchInfo->mLeftBlockIdx;
            
            STL_TRY( knlEvaluateOneExpression( sJoinFetchInfo->mPreHashEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        
        if ( (aFetchInfo->mPhysicalFilter != NULL) || (aFetchInfo->mLogicalFilter != NULL) )
        {
            if( sJoinFetchInfo->mPreJoinEvalInfo != NULL )
            {
                sJoinFetchInfo->mPreJoinEvalInfo->mBlockIdx = sJoinFetchInfo->mLeftBlockIdx;
            
                STL_TRY( knlEvaluateOneExpression( sJoinFetchInfo->mPreJoinEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }
            
            if( aFetchInfo->mFilterEvalInfo != NULL )
            {
                aFetchInfo->mFilterEvalInfo->mBlockIdx = sJoinFetchInfo->mLeftBlockIdx;
        
                STL_TRY( knlEvaluateOneExpression( aFetchInfo->mFilterEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF(
                                 aFetchInfo->mFilterEvalInfo->mResultBlock )
                             == STL_TRUE );
            
                if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_SEP_DATA_VALUE(
                                              aFetchInfo->mFilterEvalInfo->mResultBlock,
                                              sJoinFetchInfo->mLeftBlockIdx ) ) )
                {
                    sJoinFetchInfo->mLeftBlockIdx++;
                    continue;
                }
            }

            smeSetPhysicalFilter4InstRelation( aFetchInfo );
        }

        /**
         * Join Condition (ON Clause) 처리
         */
        STL_TRY( smdmihFetchOneRow4AntiSemiJoin( sIterator,
                                                 sTableHeader,
                                                 aFetchInfo,
                                                 &sMatchRow,
                                                 aEnv )
                 == STL_SUCCESS );
        
        /** 
         * ANTI SEMI JOIN 인 경우
         *  : Key와 일치하는 Record가 없으면 Result Filter 검사
         */
        if( sMatchRow == STL_FALSE )
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
                                            
            /* anti semi join 조건을 만족하는 left row 임 */
            if ( sJoinFetchInfo->mResultFilter == NULL )
            {
                sMatchRow = STL_TRUE;
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

                /* Join Filter (WHERE Clause) 처리 */
                STL_TRY( knlEvaluateJoinFilter( sJoinFetchInfo->mResultFilter,
                                                sJoinFetchInfo->mResultBlockIdx,
                                                & sMatchRow,
                                                KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }

            /* Result Filter 조건을 만족하는 경우 결과로 내보냄 */
            if( sMatchRow == STL_TRUE )
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
                        sJoinFetchInfo->mLeftBlockIdx++;
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


stlBool smdmihCompareKeyAndBlock( smdmihTableHeader  * aTableHeader,
                                  knlValueBlockList  * aDataBlockList,
                                  stlInt64             aBlockIdx,
                                  stlChar            * aKey )
{
    knlValueBlockList * sDataBlockList;
    dtlDataValue      * sDataValue;
    stlUInt16           sColLen16;
    stlInt64            sLength1;
    stlInt64            sLength2;
    stlInt32            i;
    stlInt32            j;
    stlChar           * sKeyColumn = NULL;
    stlChar           * sValue1;
    stlChar           * sValue2;
    stlInt32            sColLenSize;
    smdmifDefColumn   * sColumn;
    stlInt64            sColLen64;
    knlLogicalAddr      sVarColLa;
    stlChar           * sVarCol = NULL;
    
    sDataBlockList = aDataBlockList;

    if( aTableHeader->mIsAllColumnLen1 == STL_TRUE )
    {
        for( i = 0; i < aTableHeader->mKeyColCount; i++ )
        {
            sColumn = &aTableHeader->mColumns[i];
            sDataValue = KNL_GET_BLOCK_DATA_VALUE( sDataBlockList, aBlockIdx );
            sKeyColumn = aKey + sColumn->mOffset;
            sLength1 = *(sKeyColumn);
            sLength2 = sDataValue->mLength;
        
            if( ( sDataValue->mType == DTL_TYPE_CHAR ) &&
                ( sLength1 > 0 ) &&
                ( sLength2 > 0 ) )
            {
                /**
                 * CHAR type은 space padding을 제거한 상태로 값을 비교한다.
                 */
                
                sValue1 = sKeyColumn + 1;
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

                sValue2 = (stlChar*) sDataValue->mValue;
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
            else if( ( sDataValue->mType == DTL_TYPE_BINARY ) &&
                     ( sLength1 > 0 ) &&
                     ( sLength2 > 0 ) )
            {
                /**
                 * BINARY type은 space padding을 제거한 상태로 값을 비교한다.
                 */
                
                sValue1 = sKeyColumn + 1;
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

                sValue2 = (stlChar*) sDataValue->mValue;
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
                    sValue1 = sKeyColumn + 1;
                    sValue2 = sDataValue->mValue;
                
                    sLength1 -= gDtlCompValueIgnoreSize[ sDataValue->mType ];
                    
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
            }
            else
            {
                return STL_FALSE;
            }

            sDataBlockList = sDataBlockList->mNext;
        }
    }
    else
    {
        for( i = 0; i < aTableHeader->mKeyColCount; i++ )
        {
            sColLenSize = aTableHeader->mColumnInfo[i].mColLenSize;
            sColumn = &aTableHeader->mColumns[i];
        
            sDataValue = KNL_GET_BLOCK_DATA_VALUE( sDataBlockList, aBlockIdx );
        
            if( sColumn->mIsFixedPart == STL_TRUE )
            {
                sKeyColumn = aKey + sColumn->mOffset;
                if( sColLenSize == 1 )
                {
                    sColLen16 = *(sKeyColumn);
                }
                else
                {
                    STL_WRITE_INT16( &sColLen16, sKeyColumn );
                }

                sLength1 = sColLen16;
            }
            else
            {
                STL_WRITE_INT64( &sVarColLa, (aKey + sColumn->mOffset) );

                if( sVarColLa == KNL_LOGICAL_ADDR_NULL )
                {
                    sLength1 = 0;
                }
                else
                {
                    sVarCol = (stlChar*)KNL_TO_PHYSICAL_ADDR( sVarColLa );
            
                    if( SMDMIF_VAR_COL_HAS_NEXT( sVarCol ) == STL_TRUE )
                    {
                        smdmifVarPartReadCol( sVarCol,
                                              aTableHeader->mColCombineMem1,
                                              &sColLen64 );
                        sKeyColumn = aTableHeader->mColCombineMem1;
                        sLength1 = sColLen64;
                    }
                    else
                    {
                        sKeyColumn = SMDMIF_VAR_COL_VALUE_POS( sVarCol );
                        sLength1 = SMDMIF_VAR_COL_GET_LEN( sVarCol );
                    }
                    sColLenSize = 0;
                }
            }
        
            if( sDataValue->mLength == sLength1 )
            {
                if( sLength1 != 0 )
                {
                    sValue1 = sKeyColumn + sColLenSize;
                    sValue2 = sDataValue->mValue;
                
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
            }
            else
            {
                return STL_FALSE;
            }

            sDataBlockList = sDataBlockList->mNext;
        }
    }

    return STL_TRUE;
}

stlStatus smdmihFetchOneRow( smdmihTableHeader * aTableHeader,
                             void              * aFixedRow,
                             smlFetchInfo      * aFetchInfo,
                             stlInt32            aBlockIdx,
                             stlBool           * aFetched,
                             smlEnv            * aEnv )
{
    knlValueBlockList   * sFetchCol;
    stlChar             * sFixedRow = (stlChar*)aFixedRow;
    stlChar             * sVarCol = NULL;
    smdmifDefColumn     * sColumn;
    dtlDataValue        * sValueCol;
    stlInt64              sColLen64;
    stlInt32              sColLenSize;
    knlLogicalAddr        sVarColLa;
    stlInt32              sKeyRowPos;
    stlChar             * sKeyColumn = NULL;
    stlUInt16             sColLen16;
    knlPhysicalFilter   * sPhysicalFilter = aFetchInfo->mPhysicalFilter;
    knlExprEvalInfo     * sLogicalFilterInfo = aFetchInfo->mFilterEvalInfo;
    knlColumnInReadRow  * sCurInReadRow;

    *aFetched = STL_FALSE;

    /**
     * fetch & physical filtering
     */

    sFetchCol = aFetchInfo->mColList;
    
    while( sFetchCol != NULL )
    {
        sColLenSize = aTableHeader->mColumnInfo[sFetchCol->mColumnOrder].mColLenSize;
        sColumn = &aTableHeader->mColumns[sFetchCol->mColumnOrder];
        sValueCol = KNL_GET_BLOCK_DATA_VALUE( sFetchCol, aBlockIdx );
        sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( sFetchCol );
        
        if( sColumn->mIsFixedPart == STL_TRUE )
        {
            sKeyColumn = sFixedRow + sColumn->mOffset;
            if( sColLenSize == 1 )
            {
                sColLen16 = *(sKeyColumn);
            }
            else
            {
                STL_WRITE_INT16( &sColLen16, sKeyColumn );
            }
            
            sKeyRowPos += ( sColLenSize + sColLen16 );
            sValueCol->mLength = sColLen16;
        }
        else
        {
            STL_WRITE_INT64( &sVarColLa, (sFixedRow + sColumn->mOffset) );
                    
            if( sVarColLa == KNL_LOGICAL_ADDR_NULL )
            {
                sColLen64 = 0;
            }
            else
            {
                sVarCol = (stlChar*)KNL_TO_PHYSICAL_ADDR( sVarColLa );

                if( SMDMIF_VAR_COL_HAS_NEXT( sVarCol ) == STL_TRUE )
                {
                    smdmifVarPartReadCol( sVarCol,
                                          aTableHeader->mColCombineMem1,
                                          &sColLen64 );
                    sKeyColumn = aTableHeader->mColCombineMem1;
                }
                else
                {
                    sKeyColumn = SMDMIF_VAR_COL_VALUE_POS( sVarCol );
                    sColLen64 = SMDMIF_VAR_COL_GET_LEN( sVarCol );
                }
                sColLenSize = 0;
            }

            sValueCol->mLength = sColLen64;
        }

        sCurInReadRow->mValue  = sKeyColumn + sColLenSize;
        sCurInReadRow->mLength = sValueCol->mLength;
                
        while( sPhysicalFilter != NULL )
        {
            if( sPhysicalFilter->mColIdx1 <= sFetchCol->mColumnOrder )
            {
                if( sPhysicalFilter->mFunc( sPhysicalFilter->mColVal1->mValue,
                                            sPhysicalFilter->mColVal1->mLength,
                                            sPhysicalFilter->mColVal2->mValue,
                                            sPhysicalFilter->mColVal2->mLength ) == STL_FALSE )
                {
                    STL_THROW( RAMP_FINISH );
                }
            }
            else
            {
                break;
            }

            sPhysicalFilter = sPhysicalFilter->mNext;
        }
        
        DTL_COPY_VALUE( sValueCol,
                        sKeyColumn + sColLenSize,
                        sValueCol->mLength );
        
        sFetchCol = sFetchCol->mNext;
            
        if( sFetchCol == NULL )
        {
            break;
        }
    }

    
    /**
     * evaluate logical filter
     */
    
    if( sLogicalFilterInfo != NULL )
    {
        STL_DASSERT( sLogicalFilterInfo != NULL );

        sLogicalFilterInfo->mBlockIdx = aBlockIdx;
        
        STL_TRY( knlEvaluateOneExpression( sLogicalFilterInfo,
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF(
                         sLogicalFilterInfo->mResultBlock )
                     == STL_TRUE );
            
        if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_SEP_DATA_VALUE(
                                      sLogicalFilterInfo->mResultBlock,
                                      aBlockIdx ) ) )
        {
            STL_THROW( RAMP_FINISH );
        }
    }

    *aFetched = STL_TRUE;

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
/** @} */

