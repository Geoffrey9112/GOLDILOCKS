/*******************************************************************************
 * smdmisScan.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmisScan.c 6124 2012-10-29 10:31:50Z htkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smdmisScan.c
 * @brief Storage Manager Layer Memory Instant Sort Table Routines
 */

#include <dtl.h>
#include <sml.h>
#include <smDef.h>
#include <sma.h>
#include <smo.h>
#include <smdmifScan.h>
#include <smdmisScan.h>
#include <smdmifFixedPart.h>
#include <smdmifTable.h>
#include <smdmisTable.h>

/**
 * @addtogroup smdmisScan
 * @{
 */

smeIteratorModule gSmdmisIteratorModule =
{
    STL_SIZEOF(smdmisIterator),

    smdmisScanInitIterator,
    NULL,     /* finiIterator */
    smdmisScanResetIterator,
    smdmifScanMoveToRowRid,
    smdmifScanMoveToPosNum,
    {
        smdmisScanFetchNext,
        smdmisScanFetchPrev,
        (smiFetchAggrFunc)smdmisScanFetchNext,
        NULL    /* FetchSampling */
    }
};

static stlBool smdmisScanCheckRadixCompPossible( smlFetchInfo *aFetchInfo );

static stlStatus smdmisScanInitForRadixComp( smdmisTableHeader *aHeader,
                                             smdmisIterator    *aIterator,
                                             smlEnv            *aEnv );

static void smdmisScanTransformRange( smdmisTableHeader  *aHeader,
                                      smdmisIterator     *aIterator,
                                      knlCompareList     *aKeyRange,
                                      stlBool             aMinRange );

static dtlCompareResult smdmisCompareKeyByRadix( stlUInt16       *aRadixArray,
                                                 smdmisIterator  *aIterator,
                                                 stlChar         *aLeftRow,
                                                 stlChar         *aRightRow );

static stlStatus smdmisScanFindFirstByRadixComp( smdmisTableHeader  *aHeader,
                                                 smdmisIterator     *aIterator,
                                                 knlCompareList     *aMinRange );

static stlStatus smdmisScanFindFirstByPhysicalComp( smdmisTableHeader  *aHeader,
                                                    smdmisIterator     *aIterator,
                                                    knlCompareList     *aMaxRange );

static stlStatus smdmisScanTryFindLastByRadixComp( smdmisTableHeader  *aHeader,
                                                   smdmisIterator     *aIterator,
                                                   knlCompareList     *aMaxRange );

static stlStatus smdmisScanTryFindLastByPhysicalComp( smdmisTableHeader  *aHeader,
                                                      smdmisIterator     *aIterator,
                                                      knlCompareList     *aMaxRange );

/**
 * @brief 주어진 Iterator를 초기화한다
 * @param[in,out] aIterator 초기화할 iterator
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmisScanInitIterator( void   *aIterator,
                                  smlEnv *aEnv )
{
    smdmisIterator    *sIterator = (smdmisIterator *)aIterator;
    smdmisTableHeader *sHeader = SMDMIS_TABLE_ITERATOR_GET_TABLE_HEADER( sIterator );

    SMDMIS_TABLE_ITERATOR_INIT( sIterator );

    /*
     * 한건도 insert가 되지 않은 경우 index를 빌드하지 않는다.
     * insert시 NULL 컬럼이 있으면 insert가 안되게 함으로써,
     * insert가 한건도 안되었는데도 iterator를 alloc할 경우가 생겨서 이런 조취를 취했다.
     */
    if( SMDMIS_TABLE_INDEX_BUILT( sHeader ) == STL_FALSE )
    {
        STL_DASSERT( STL_RING_GET_LAST_DATA( &sIterator->mCommon.mStatement->mStatementMem.mMarkList ) ==
                     ((smiIterator*)sIterator)->mRegionMarkEx );
    
        STL_TRY( smdmisTableBuildIndex( sIterator->mCommon.mStatement,
                                        sHeader,
                                        &sHeader->mBaseHeader,
                                        aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 Iterator를 다시 초기화한다
 * @param[in,out] aIterator         초기화할 iterator
 * @param[in]     aEnv              Storage Manager Environment
 */
stlStatus smdmisScanResetIterator( void   *aIterator,
                                   smlEnv *aEnv )
{
    smdmisIterator *sIterator = (smdmisIterator *)aIterator;

    SMDMIS_TABLE_ITERATOR_RESET( sIterator );

    return STL_SUCCESS;
}

stlStatus smdmisScanFindFirstAndLast( smdmisTableHeader *aHeader,
                                      smdmisIterator    *aIterator,
                                      smlFetchInfo      *aFetchInfo,
                                      smlEnv            *aEnv )
{
    if( aIterator->mCompMethodDecided == STL_FALSE )
    {
        /* radix compare할지 일반 physical compare할지 결정한다.
         * 한번 결정되면 Iterator가 reset되어도 바뀌지 않는데,
         * QP에서는 iterator를 재사용할 때, 타입이 바뀌지 않는다는 가정을
         * SM에 제공했기 때문에 compare 방식을 바꾸는 일이 없다고 가정할 수 있는 것이다. */
        if( SMDMIS_TABLE_IS_RADIX_SORTED( aHeader ) == STL_TRUE )
        {
            if( smdmisScanCheckRadixCompPossible( aFetchInfo ) == STL_TRUE )
            {
                aIterator->mFindFirst = smdmisScanFindFirstByRadixComp;
                aIterator->mTryFindLast = smdmisScanTryFindLastByRadixComp;

                STL_TRY( smdmisScanInitForRadixComp( aHeader,
                                                     aIterator,
                                                     aEnv ) == STL_SUCCESS );
            }
            else
            {
                /* table은 radix sort되어 있지만, key range와 타입이 다른 이유로
                   radix compare를 할 수 없는 경우이다.
                   radix sort를 무효화하고 일반 physical compare를 사용한다. */
                STL_TRY( smdmisTableInvalidateRadix( aHeader,
                                                     aEnv ) == STL_SUCCESS );

                STL_DASSERT( SMDMIS_TABLE_IS_RADIX_SORTED( aHeader ) == STL_FALSE );

                aIterator->mFindFirst = smdmisScanFindFirstByPhysicalComp;
                aIterator->mTryFindLast = smdmisScanTryFindLastByPhysicalComp;
            }
        }
        else
        {
            aIterator->mFindFirst = smdmisScanFindFirstByPhysicalComp;
            aIterator->mTryFindLast = smdmisScanTryFindLastByPhysicalComp;
        }

        aIterator->mCompMethodDecided = STL_TRUE;
    }

    if( aFetchInfo->mRange->mMinRange != NULL )
    {
        STL_TRY( aIterator->mFindFirst( aHeader,
                                        aIterator,
                                        aFetchInfo->mRange->mMinRange ) == STL_SUCCESS );
    }
    else
    {
        aIterator->mCurNode = aHeader->mFirstLeafNode;
        aIterator->mCurIndex = 0;
    }

    if( aFetchInfo->mRange->mMaxRange != NULL )
    {
        STL_TRY( aIterator->mTryFindLast( aHeader,
                                          aIterator,
                                          aFetchInfo->mRange->mMaxRange ) == STL_SUCCESS );
    }
    else
    {
        aIterator->mLastNode = KNL_LOGICAL_ADDR_NULL;
        aIterator->mLastIndex = SMDMIS_INDEX_BEFORE_FIRST;
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
stlStatus smdmisScanFetchNext( void          * aIterator,
                               smlFetchInfo  * aFetchInfo,
                               smlEnv        * aEnv )
{
    smdmisIterator      *sIterator = (smdmisIterator *)aIterator;
    smdmisTableHeader   *sHeader = SMDMIS_TABLE_ITERATOR_GET_TABLE_HEADER( sIterator );
    smdmisIndexNode     *sNode;
    smdmisKeyRow         sKeyRow;
    smdmifFixedRow       sFixedRow;
    stlInt64             sBlockSize = SML_BLOCK_SIZE( aFetchInfo->mRowBlock );
    stlInt32             sBlockIdx  = 0;
    knlLogicalAddr       sRowAddr;
    smlRid              *sRid;
    stlBool              sFetched;
    stlInt32             sAggrRowCount = 0;
    knlPhysicalFilter   *sRowPhysicalFilter;
    knlExprEvalInfo     *sRowLogicalFilterInfo;
    knlValueBlockList   *sRowFetchCol;
    
    SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );

    aFetchInfo->mIsEndOfScan = STL_FALSE;

    if( SMDMIS_TABLE_ITERATOR_IS_BEFORE_FIRST( sIterator ) == STL_TRUE )
    {
        if( SMDMIF_TABLE_FIRST_INSERTED( &sHeader->mBaseHeader ) == STL_FALSE )
        {
            /*
             * 한건도 insert되지 않았으면 바로 EOS이다.
             * [coverage]
             * 현재(2014-5-26) QP에서 record가 없을 경우 iterator를
             * 아예 만들지 않기 때문에 이 코드를 타는 경우는 없다.
             * 따라서 coverage 측정시 이 코드는 dead-code로 나오는데,
             * 그렇다고 빼자니 assert로는 실 환경에서 불필요한 장애를
             * 불러 일으킬 수 있기 때문에 그냥 두기로 한다.
             */
            aFetchInfo->mIsEndOfScan = STL_TRUE;
            STL_THROW( RAMP_FINISH );
        }

        STL_TRY( smdmisScanFindFirstAndLast( sHeader,
                                             sIterator,
                                             aFetchInfo,
                                             aEnv ) == STL_SUCCESS );
    }

    if( aFetchInfo->mFetchRecordInfo == NULL )
    {
        sRowPhysicalFilter    = NULL;
        sRowLogicalFilterInfo = NULL;
        sRowFetchCol          = NULL;
    }
    else
    {
        sRowPhysicalFilter    = aFetchInfo->mFetchRecordInfo->mPhysicalFilter;
        sRowLogicalFilterInfo = aFetchInfo->mFetchRecordInfo->mLogicalFilterEvalInfo;
        sRowFetchCol          = aFetchInfo->mFetchRecordInfo->mColList;
    }

    sNode = SMDMIS_TO_NODE_PTR( sIterator->mCurNode );
    while( STL_TRUE )
    {
        if( (aFetchInfo->mSkipCnt == 0) && (aFetchInfo->mFetchCnt == 0 || sBlockIdx >= sBlockSize ) )
        {
            break;
        }

        if( SMDMIS_TABLE_ITERATOR_IS_END( sIterator ) == STL_TRUE )
        {
            aFetchInfo->mIsEndOfScan = STL_TRUE;
            break;
        }

        if( sIterator->mCurIndex >= sNode->mKeyCount )
        {
            sIterator->mCurNode = sNode->mNext;
            sIterator->mCurIndex = 0;
            sNode = SMDMIS_TO_NODE_PTR( sIterator->mCurNode );

            if( sNode == NULL )
            {
                /* 더이상 탐색할 node가 없는 경우이다. */
                continue;
            }
            /* 다음 노드의 키 개수가 0개일 수 없다. */
            STL_DASSERT( sNode->mKeyCount > 0 );

            if( sIterator->mTryFindLast != NULL )
            {
                STL_TRY( sIterator->mTryFindLast( sHeader,
                                                  sIterator,
                                                  aFetchInfo->mRange->mMaxRange ) == STL_SUCCESS );

                continue;
            }
        }

        sKeyRow = SMDMIS_NODE_GET_KEY( sNode, sIterator->mCurIndex );
        sIterator->mCurIndex++;

        sFixedRow = SMDMIS_KEY_ROW_GET_FIXED_ROW( sKeyRow );
                             
        /* 현재 row가 delete되었는지 검사한다. */
        if( SMDMIF_FIXED_ROW_IS_DELETED( sFixedRow ) == STL_TRUE )
        {
            /* [coverage]
             * 현재 sort instant table에 대해 delete하는 경우가 없으므로
             * 이 코드는 dead-code이다. 하지만 그대로 둔다.
             */
            continue;   
        }

        /* 현재 row의 flag를 검사한다. */
        if( aFetchInfo->mRowFlag == SML_INSTANT_TABLE_ROW_FLAG_DONTCARE )
        {
            /* flag에 상관하지 않음 */
        }
        else
        {
            /* flag에 따라 skip 여부를 판단함 */
            if( ( ( aFetchInfo->mRowFlag == SML_INSTANT_TABLE_ROW_FLAG_SET ) &&
                  ( SMDMIF_FIXED_ROW_IS_FLAG_SET( sFixedRow ) == STL_FALSE ) ) ||
                ( ( aFetchInfo->mRowFlag == SML_INSTANT_TABLE_ROW_FLAG_UNSET ) &&
                  ( SMDMIF_FIXED_ROW_IS_FLAG_SET( sFixedRow ) == STL_TRUE ) ) )
            {
                /* [coverage]
                 * 현재 이 코드를 타는 경우는 없어 dead-code이지만 그대로 둔다.
                 */
                continue;
            }
        }

        /* 컬럼을 읽어들인다. */
        STL_TRY( smdmifTableFetchOneRow( (smdmifTableHeader *)sHeader,
                                         sFixedRow,
                                         aFetchInfo->mPhysicalFilter,
                                         aFetchInfo->mFilterEvalInfo,
                                         aFetchInfo->mColList,
                                         sRowPhysicalFilter,
                                         sRowLogicalFilterInfo,
                                         sRowFetchCol,
                                         sBlockIdx,
                                         &sFetched,
                                         aEnv ) == STL_SUCCESS );

        if( sFetched == STL_FALSE )
        {
            continue;
        }

        if( aFetchInfo->mAggrFetchInfo != NULL )
        {
            /* [coverage]
             * 위에서 설명한대로 sort instant table에 대해
             * aggregation function이 들어오는 경우는 현재로서는 없다.
             */
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

            sRowAddr = SMDMIS_KEY_ROW_GET_FIXED_ROW_ADDR( sKeyRow );

            sRid = (smlRid *)(&sRowAddr);

            SML_SET_RID_VALUE( aFetchInfo->mRowBlock, sBlockIdx, *sRid );

            /* row를 읽었으니 blockIdx와 fetchCnt를 갱신하고 종료조건 검사를 한다. */
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
        /* [coverage]
         * 위에서 설명한대로 sort instant table에 대해
         * aggregation function이 주어지는 예를 현재로서는 만들 수 없다.
         */
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

static stlBool smdmisScanCheckRadixCompPossible( smlFetchInfo *aFetchInfo )
{
    knlCompareList *sKeyRange = aFetchInfo->mRange->mMinRange;

    while( sKeyRange != NULL )
    {
        if( sKeyRange->mIsDiffTypeCmp == STL_TRUE )
        {
            return STL_FALSE;
        }
        sKeyRange = sKeyRange->mNext;
    }

    sKeyRange = aFetchInfo->mRange->mMaxRange;
    while( sKeyRange != NULL )
    {
        if( sKeyRange->mIsDiffTypeCmp == STL_TRUE )
        {
            return STL_FALSE;
        }
        sKeyRange = sKeyRange->mNext;
    }

    return STL_TRUE;
}

static stlStatus smdmisScanInitForRadixComp( smdmisTableHeader *aHeader,
                                             smdmisIterator    *aIterator,
                                             smlEnv            *aEnv )
{
    /* 인덱스 bottom-up build시 keyrow하나를 alloc한다.
       이 메모리는 index search시 key compare할 때 range의 키를 transform할 때 사용된다.
       사실 heap 메모리를 써도 되지만, iterator가 해제될 때 불리는 callback이 없으므로
       iterator가 생성되는 같은 메모리(shared region mem)를 부득이하게 사용한다.
       이 메모리는 iterator가 해제될 때 자동으로 해제된다. */

    STL_DASSERT( STL_RING_GET_LAST_DATA( &aIterator->mCommon.mStatement->mStatementMem.mMarkList ) ==
                 ((smiIterator*)aIterator)->mRegionMarkEx );
    
    STL_TRY( smaAllocRegionMem( aIterator->mCommon.mStatement,
                                aHeader->mKeyRowLen + SMDMIS_KEY_COL_ALIGN,
                                (void**)&aIterator->mTransKey,
                                aEnv ) == STL_SUCCESS );

    /* key row header를 뺀 위치가 align 맞아야 한다. */
    aIterator->mTransKey = (smdmisKeyRow)( STL_ALIGN( ((stlInt64)aIterator->mTransKey + SMDMIS_KEY_ROW_HEADER_SIZE),
                                                      SMDMIS_KEY_COL_ALIGN ) -
                                           SMDMIS_KEY_ROW_HEADER_SIZE );

    STL_TRY( smaAllocRegionMem( aIterator->mCommon.mStatement,
                                STL_SIZEOF( dtlCompareResult ) * aHeader->mRadixIndexFence,
                                (void**)&aIterator->mCompWhenRightRemains,
                                aEnv ) == STL_SUCCESS );

    STL_TRY( smaAllocRegionMem( aIterator->mCommon.mStatement,
                                STL_SIZEOF( stlBool ) * aHeader->mRadixIndexFence,
                                (void**)&aIterator->mRadixEqualIncluded,
                                aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static void smdmisScanTransformRange( smdmisTableHeader  *aHeader,
                                      smdmisIterator     *aIterator,
                                      knlCompareList     *aKeyRange,
                                      stlBool             aMinRange )
{
    stlInt32         sKeyColIndex;
    smdmifDefColumn *sKeyCol;
    knlCompareList  *sOuterRange;
    knlCompareList  *sInnerRange;
    knlCompareList  *sTargetRange;
    stlUInt16        sRadixIndex = 0;
    stlInt32         sColLen;
    dtlCompareResult sCompResult;
    stlUInt8        *sPos;

    /*
       radix sort된 key들과 compare하기 위해서는 key range에 변형을 가해야 한다.

       1. 중복 컬럼의 경우 한 조건만 만족하면 다른 조건들은 자동으로 만족하게 되는데,
          그 다른 조건들을 찾아 제거한다.
       2. key column보다 적은 컬럼이 올 경우, 즉 a,b,c 키 컬럼이 있는데,
          조건으로 a,b가 올 경우 c 컬럼은 radix compare에서 제외해야 한다.
          이를 위해 radixIndexFence를 b 컬럼까지만으로 설정한다.
       3. range에 있는 값들을 가지고 가상 key row를 구성한다.
          이때 null order, asc/desc를 위해 key 변형이 일어난다.
          이 가상 key row는 aIterator->mTransKey에 저장된다.

          aMinRange 변수는 이 range가 minRange인지 maxRange를 알려주는데,
          min이냐 max냐에 따라 1번 작업에서 어떤 조건을 제거할지가 달라지기 때문이다.
     */

    SMDMIS_KEY_ROW_INIT( aIterator->mTransKey, aHeader->mKeyRowLen );

    sKeyColIndex = 0;
    for( sOuterRange = aKeyRange; sOuterRange != NULL; )
    {
        sTargetRange = sOuterRange;
        for( sInnerRange = sOuterRange->mNext; sInnerRange != NULL; sInnerRange = sInnerRange->mNext )
        {
            if( sInnerRange->mColOrder != sOuterRange->mColOrder )
            {
                break;
            }

            /*
             * sTargetRange과 sInnerRange는 같은 컬럼에 대한 다른 range이다.
             * 두 range 중에 하나라도 null 관련 조건일 수가 없다.
             * 만약 null 관련 조건과 그렇지 않은 조건이 함께 있을 경우,
             * 하나는 불필요한 조건이기 때문에 QP에서 제거하는 것이 좋다.
             *
             * 그러나 함께 들어오는 경우도 있을 수 있기 때문에
             * 아래 주석처리했던 내용을 복원한다.
             */
            /**
             * 다음과 같은 경우 null 관련 조건과 그렇지 않은 조건이 함께 들어오게 된다.
             *
             * create table t1( i1 varchar(10) );
             * create index idx1 on t1(i1);
             * commit;
             *
             * insert into t1 values(1);
             * commit;
             *
             * \explain plan
             * select / *+ use_inl(a, b) * / * from t1 a, t1 b
             * where b.i1 >= a.i1 and b.i1 <= a.i1;
             */

            if( sTargetRange->mRangeLen == 0 )
            {
                if( sInnerRange->mRangeLen == 0 )
                {
                    // 둘다 null 관련 조건인데, 하나는 is null이고 다른 하나는 is not null일 수는 없다.
                    STL_DASSERT( sTargetRange->mIsIncludeEqual == sInnerRange->mIsIncludeEqual );
                    // 같은 조건이므로 하나는 무시할 수 있다. inner range를 무시한다. 
                }
                else
                {
                    if( sTargetRange->mIsIncludeEqual == STL_TRUE )
                    {
                        // target이 is null 조건일 경우 
                        if( sTargetRange->mIsNullFirst == STL_TRUE )
                        {
                            sTargetRange = sInnerRange;
                        }
                    }
                    else
                    {
                        // target이 is not null 조건일 경우 
                        if( sTargetRange->mIsNullFirst == STL_FALSE )
                        {
                            sTargetRange = sInnerRange;
                        }
                    }
                }
            }
            else if( sInnerRange->mRangeLen == 0 )
            {
                if( sInnerRange->mIsIncludeEqual == STL_TRUE )
                {
                    // inner가 is null 조건일 경우 
                    if( sInnerRange->mIsNullFirst == STL_FALSE )
                    {
                        sTargetRange = sInnerRange;
                    }
                }
                else
                {
                    // target이 is not null 조건일 경우 
                    if( sInnerRange->mIsNullFirst == STL_TRUE )
                    {
                        sTargetRange = sInnerRange;
                    }
                }
            }
            else
            {
                sCompResult = sTargetRange->mCompFunc( sTargetRange->mRangeVal,
                                                       sTargetRange->mRangeLen,
                                                       sInnerRange->mRangeVal,
                                                       sInnerRange->mRangeLen );

                if( (sCompResult == DTL_COMPARISON_EQUAL) && (sInnerRange->mIsIncludeEqual == STL_FALSE) )
                {
                    /* 똑같은 조건이고, equal 조건을 포함하지 않는게 더 최소 조건이 된다. */
                    sTargetRange = sInnerRange;
                }
                else if( (aMinRange == STL_TRUE) && (sCompResult == DTL_COMPARISON_LESS) )
                {
                    /* minRange일 경우 sTargetRange는 최대값을 가진 range로 선택된다. */
                    sTargetRange = sInnerRange;
                }
                else if( (aMinRange == STL_FALSE) && (sCompResult == DTL_COMPARISON_GREATER) )
                {
                    /* maxRange일 경우 sTargetRange는 최소값을 가진 range로 선택된다. */
                    sTargetRange = sInnerRange;
                }
            }
        }

        /* 아래 조건을 만족하지 못한다면 range의 키 구성이 잘못된 경우이다. */
        STL_DASSERT( sTargetRange->mColOrder == sKeyColIndex );

        sKeyCol = &aHeader->mKeyCols[sKeyColIndex];

        /* range의 키값의 길이가 key에 할당된 크기보다 더 클 경우 mAllocSize 만큼만 radix compare 한다.
           남은 바이트를 조사해, 0이 아닌게 하나라도 있으면,
           compare시 같다고 판단했을 때 range쪽이 더 크다고 판단을 바꾼다. */
        sColLen = STL_MIN( sTargetRange->mRangeLen, sKeyCol->mAllocSize );

        SMDMIS_KEY_ROW_WRITE_COL( aIterator->mTransKey,
                                  sKeyCol,
                                  sTargetRange->mRangeVal,
                                  sColLen );

        if( aHeader->mKeyColInfo[sKeyColIndex].mModifyKeyVal != NULL )
        {
            /* radix compare를 위해 key 변형이 필요한 경우 수행한다. */
            aHeader->mKeyColInfo[sKeyColIndex].mModifyKeyVal( aIterator->mTransKey,
                                                              sTargetRange->mRangeLen,
                                                              sKeyCol );
        }

        while( (sRadixIndex < aHeader->mRadixIndexFence) &&
               (aHeader->mRadixArray[sRadixIndex] <
                SMDMIS_KEY_COL_OFFSET_VALUE(sKeyCol) + sKeyCol->mAllocSize) )
        {
            aIterator->mCompWhenRightRemains[sRadixIndex] = DTL_COMPARISON_EQUAL;
            aIterator->mRadixEqualIncluded[sRadixIndex] = STL_TRUE;
            sRadixIndex++;
        }

        if( sColLen < sTargetRange->mRangeLen )
        {
            /* 현재 key보다 range의 키가 더 길어서 더이상 비교 불가능할 때,
               어떤게 더 큰 값인지를 판단하게 하는 변수이다.
               두 키의 길이가 다를 수 있는 건 다음 다섯 타입 뿐이다. 
                  (처음엔 numeric, varchar, varbinary만 고려되었는데 왜 char, binary에 대한 고려가 없었는지 의문이다. - 2015/8/21)
               - numeric: 양수일 경우엔 긴 쪽이 더 크고, 음수일 경우엔 긴 쪽이 작다.
               - varchar, varbinary: 긴 쪽이 무조건 더 크다.
               - binary: 남은 데이터가 모두 0이면 같고 아니면 긴 쪽이 더 크다.
               - char: 남은 데이터가 모두 ' '이면 같고 그보다 작으면 키가 더 크고, ' '보다 크면 키가 더 작다. --2015/8/21 reviewed by jhkim
             */
            if( ( sKeyCol->mDataType == DTL_TYPE_FLOAT ) || ( sKeyCol->mDataType == DTL_TYPE_NUMBER ) )
            {
                if( *((stlUInt8 *)aIterator->mTransKey + SMDMIS_KEY_COL_OFFSET_VALUE(sKeyCol)) < SMDMIS_NUMERIC_MINUS_SIGN )
                {
                    /* 음수의 경우이다. */
                    aIterator->mCompWhenRightRemains[sRadixIndex - 1] = DTL_COMPARISON_GREATER;
                }
                else
                {
                    aIterator->mCompWhenRightRemains[sRadixIndex - 1] = DTL_COMPARISON_LESS;
                }
            }
            else if( sKeyCol->mDataType == DTL_TYPE_VARCHAR || sKeyCol->mDataType == DTL_TYPE_VARBINARY )
            {
                aIterator->mCompWhenRightRemains[sRadixIndex - 1] = DTL_COMPARISON_LESS;
            }
            else if( sKeyCol->mDataType == DTL_TYPE_BINARY )
            {
                aIterator->mCompWhenRightRemains[sRadixIndex - 1] = DTL_COMPARISON_EQUAL;

                for( sPos = (stlUInt8*)sTargetRange->mRangeVal + sColLen;
                     sPos < (stlUInt8*)sTargetRange->mRangeVal + sTargetRange->mRangeLen;
                     sPos++ )
                {
                    if( *sPos > 0 )
                    {
                        aIterator->mCompWhenRightRemains[sRadixIndex - 1] = DTL_COMPARISON_LESS;
                        break;
                    }
                }
            }
            else if( sKeyCol->mDataType == DTL_TYPE_CHAR )
            {
                aIterator->mCompWhenRightRemains[sRadixIndex - 1] = DTL_COMPARISON_EQUAL;

                for( sPos = (stlUInt8*)sTargetRange->mRangeVal + sColLen;
                     sPos < (stlUInt8*)sTargetRange->mRangeVal + sTargetRange->mRangeLen;
                     sPos++ )
                {
                    if( *sPos < ' ' )
                    {
                        aIterator->mCompWhenRightRemains[sRadixIndex - 1] = DTL_COMPARISON_GREATER;
                        break;
                    }
                    else if( *sPos > ' ' )
                    {
                        aIterator->mCompWhenRightRemains[sRadixIndex - 1] = DTL_COMPARISON_LESS;
                        break;
                    }
                }
            }
            else
            {
                STL_ASSERT( 0 );
            }
        }

        if( sTargetRange->mIsIncludeEqual == STL_FALSE )
        {
            aIterator->mRadixEqualIncluded[sRadixIndex - 1] = STL_FALSE;
        }

        sKeyColIndex++;
        sOuterRange = sInnerRange;
    }

    aIterator->mRadixIndexFence = sRadixIndex;
    aIterator->mForMinRange = aMinRange;
}

static dtlCompareResult smdmisCompareKeyByRadix( stlUInt16       *aRadixArray,
                                                 smdmisIterator  *aIterator,
                                                 stlChar         *aLeftRow,
                                                 stlChar         *aRightRow )
{
    stlUInt16        i;
    stlUInt16        sRadix;
    stlUInt8         sLeftRadixValue;
    stlUInt8         sRightRadixValue;
    dtlCompareResult sResult = DTL_COMPARISON_EQUAL;

    for( i = 0; i < aIterator->mRadixIndexFence; i++ )
    {
        sRadix = aRadixArray[i];

        sLeftRadixValue = SMDMIS_KEY_ROW_GET_RADIX( aLeftRow, sRadix );
        sRightRadixValue = SMDMIS_KEY_ROW_GET_RADIX( aRightRow, sRadix );

        if( sLeftRadixValue < sRightRadixValue )
        {
            sResult = DTL_COMPARISON_LESS;
            break;
        }
        else if( sLeftRadixValue > sRightRadixValue )
        {
            sResult = DTL_COMPARISON_GREATER;
            break;
        }
        else if( aIterator->mCompWhenRightRemains[i] != DTL_COMPARISON_EQUAL )
        {
            /* 한 컬럼에 대해 equal이지만, 오른쪽 키 값이 더 남아 있기 때문에 less로 판단한다.
               이에 관해 좀더 자세한 설명이 필요할 것 같아 남겨둔다.
               radix sort를 위해 key들을 모두 고정길이로 저장했다.
               그런데 문제는 성능을 위해 radix sort시 key를 저장할 때 최대길이(alloc size) 만큼 저장하지 않고
               table에 저장할 때 최대 길이를 구해 이만큼(mMaxActualLen)만 저장한다.
               range로 들어오는 키의 길이는 mMaxActualLen보다 클 수 있다.
               이 경우 어쨌든 mMaxActualLen까지만 radix compare를 수행할 수 있고,
               오른쪽 key(range로부터 온 키)는 남아 있게 되는데,
               이때 오른쪽 key가 무조건 크다고 판단한다.
               이게 가능한 이유는, key가 가변 길이로 insert될 수 있는 타입은
               varchar, numeric 이 두가지 뿐인데, 두가지 모두 길이가 긴쪽이 크다고 판단할 수 있는 것이다.
               예를 들면, 'abc'와 'abc '를 비교하면 'abc'까진 같고, 오른쪽 키에 바이트가 더 남는데,
               남은 바이트들이 모두 0이 아닌 이상 오른쪽이 크다고 할 수 있는 것이다. */
            sResult = aIterator->mCompWhenRightRemains[i];
            break;
        }
        else if( aIterator->mRadixEqualIncluded[i] == STL_FALSE )
        {
            sResult = (aIterator->mForMinRange == STL_TRUE ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER);
            break;
        }
    }

    return sResult;
}

static stlStatus smdmisScanFindFirstByRadixComp( smdmisTableHeader  *aHeader,
                                                 smdmisIterator     *aIterator,
                                                 knlCompareList     *aMinRange )
{
    smdmisIndexNode *sNode = NULL;
    smdmisKeyRow     sKeyRow;
    stlInt32         sMin;
    stlInt32         sMax;
    stlInt32         sMid;
    dtlCompareResult sCompResult;
    stlBool          sMoveToLeft;

    /* radix compare를 위해 minRange를 위한 key row를 aIterator->mTransKey에 구성한다. */
    smdmisScanTransformRange( aHeader,
                              aIterator,
                              aMinRange,
                              STL_TRUE /* minRange */ );

    sNode = SMDMIS_TO_NODE_PTR( aHeader->mRootNode );

    while( STL_TRUE )
    {
        sMin = 0;
        sMax = sNode->mKeyCount - 1;
        sMid = -1;
        sMoveToLeft = STL_FALSE;

        while( sMin <= sMax )
        {
            sMid = (sMin + sMax) >> 1;
            sKeyRow = SMDMIS_NODE_GET_KEY( sNode, sMid );

            STL_DASSERT( sKeyRow != NULL );

            sCompResult = smdmisCompareKeyByRadix( aHeader->mRadixArray,
                                                   aIterator,
                                                   sKeyRow,
                                                   aIterator->mTransKey );

            sMoveToLeft = ( sCompResult == DTL_COMPARISON_LESS ? STL_FALSE : STL_TRUE );

            if( sMoveToLeft == STL_TRUE )
            {
                sMax = sMid - 1;
            }
            else
            {
                sMin = sMid + 1;
            }
        } /* while( sMin <= sMax ) */

        if( sMoveToLeft == STL_TRUE )
        {
            sMid--;
        }

        STL_DASSERT( sMid >= -1 );

        if( sNode->mLevel == 0 )
        {
            break;
        }

        if( sMid < 0 )
        {
            sNode = SMDMIS_INDEX_NODE_LEFTMOST_NODE( sNode );
        }
        else
        {
            sNode = SMDMIS_TO_NODE_PTR( SMDMIS_KEY_ROW_GET_CHILD( SMDMIS_NODE_GET_KEY(sNode, sMid) ) );
        }
    } /* while( STL_TRUE ) */

    /* sMid는 조건을 만족하지 않는 최대값에 머물러 있기 때문에 하나 증가 시킨다. */
    aIterator->mCurIndex = sMid + 1;
    aIterator->mCurNode = sNode->mMyAddr;

    return STL_SUCCESS;
}

static stlStatus smdmisScanFindFirstByPhysicalComp( smdmisTableHeader  *aHeader,
                                                    smdmisIterator     *aIterator,
                                                    knlCompareList     *aMinRange )
{
    knlCompareList  *sCompList = aMinRange;
    smdmisIndexNode *sNode = NULL;
    smdmisKeyRow     sKeyRow;
    smdmisKeyCol     sKeyCol;
    smdmifVarCol     sVarCol;
    stlInt32         sMin;
    stlInt32         sMax;
    stlInt32         sMid = -1;
    stlChar         *sColValue = NULL;
    stlInt64         sColLen;
    stlInt32         sColIndex;
    dtlCompareResult sCompResult = DTL_COMPARISON_EQUAL;
    stlBool          sMoveToLeft = STL_TRUE;
    stlBool          sSameKey = STL_FALSE;

    sNode = SMDMIS_TO_NODE_PTR( aHeader->mRootNode );

    while( STL_TRUE )
    {
        sMin = 0;
        sMax = sNode->mKeyCount - 1;

        while( sMin <= sMax )
        {
            sMid = (sMin + sMax) >> 1;
            sKeyRow = SMDMIS_NODE_GET_KEY( sNode, sMid );

            STL_DASSERT( sKeyRow != NULL );

            sCompList = aMinRange;
            while( sCompList != NULL )
            {
                sColIndex = sCompList->mColOrder;

                if( sSameKey == STL_FALSE )
                {
                    if( aHeader->mKeyCols[sColIndex].mIsFixedPart == STL_TRUE )
                    {
                        sKeyCol   = SMDMIS_KEY_ROW_GET_COL( sKeyRow, &aHeader->mKeyCols[sColIndex] );
                        sColValue = SMDMIS_KEY_COL_GET_VALUE( sKeyCol );
                        sColLen   = SMDMIS_KEY_COL_GET_LEN( sKeyCol );
                    }
                    else
                    {
                        sVarCol = SMDMIS_KEY_ROW_GET_VAR_COL( sKeyRow, &aHeader->mKeyCols[sColIndex] );
                        if( SMDMIF_VAR_COL_HAS_NEXT( sVarCol ) == STL_TRUE )
                        {
                            /* var col이 쪼개져 있는 경우 aTempMem에 복사한다. */
                            smdmifVarPartReadCol( sVarCol,
                                    aHeader->mColCombineMem1,
                                    &sColLen );
                            sColValue = aHeader->mColCombineMem1;
                        }
                        else
                        {
                            sColValue = SMDMIF_VAR_COL_VALUE_POS( sVarCol );
                            sColLen   = SMDMIF_VAR_COL_GET_LEN( sVarCol );
                        }
                    }
                }

                sSameKey = STL_FALSE;

                if( sCompList->mRangeLen == 0 )
                {
                    /* is null 또는 is not null 조건일 경우
                       key가 null이냐 아니냐에 따라 left, right가 바뀐다.*/
                    if( sColLen == 0 )
                    {
                        sMoveToLeft = (!sCompList->mIsNullFirst) || (sCompList->mIsIncludeEqual);
                    }
                    else
                    {
                        sMoveToLeft = (sCompList->mIsNullFirst) || (!sCompList->mIsIncludeEqual);
                    }
                }
                else
                {
                    if( sColLen == 0 )
                    {
                        sMoveToLeft = sCompList->mIsNullFirst ? STL_FALSE : STL_TRUE;
                    }
                    else
                    {
                        sCompResult = sCompList->mCompFunc( sColValue,
                                                            sColLen,
                                                            sCompList->mRangeVal,
                                                            sCompList->mRangeLen );

                        if( sCompResult == DTL_COMPARISON_EQUAL )
                        {
                            sMoveToLeft = sCompList->mIsIncludeEqual;
                        }
                        else
                        {
                            if( sCompList->mIsAsc == STL_FALSE )
                            {
                                sCompResult = -sCompResult;
                            }

                            sMoveToLeft = ( sCompResult == DTL_COMPARISON_GREATER ? STL_TRUE : STL_FALSE );
                        }
                    }
                }

                if( sMoveToLeft == STL_FALSE )
                {
                    break;
                }
                else
                {
                    /* 다만 컬럼이 더 클 경우, 다음 비교할 컬럼이 같은 컬럼이면 조건 검사를 더 해봐야 안다.
                       예) a > 4 and a > 7 이런 조건이 오면 앞의 조건이 greater를 만족하더라도,
                       뒤에 조건을 봐야 하기 때문이다. */
                    if( sCompList->mNext != NULL )
                    {
                        if( sCompList->mColOrder == sCompList->mNext->mColOrder )
                        {
                            sSameKey = STL_TRUE;
                        }
                        else
                        {
                            if( sCompResult != DTL_COMPARISON_EQUAL )
                            {
                                break;
                            }
                        }
                    }
                }

                sCompList = sCompList->mNext;
            } /* while( sCompList != NULL ) */

            if( sMoveToLeft == STL_TRUE )
            {
                sMax = sMid - 1;
            }
            else
            {
                sMin = sMid + 1;
            }
        } /* while( sMin <= sMax ) */

        if( sMoveToLeft == STL_TRUE )
        {
            sMid--;
        }

        STL_DASSERT( sMid >= -1 );

        if( sNode->mLevel == 0 )
        {
            break;
        }

        if( sMid < 0 )
        {
            sNode = SMDMIS_INDEX_NODE_LEFTMOST_NODE( sNode );
        }
        else
        {
            sNode = SMDMIS_TO_NODE_PTR( SMDMIS_KEY_ROW_GET_CHILD( SMDMIS_NODE_GET_KEY(sNode, sMid) ) );
        }
    } /* while( STL_TRUE ) */

    /* sMid는 조건을 만족하지 않는 최대값에 머물러 있기 때문에 하나 증가 시킨다. */
    aIterator->mCurIndex = sMid + 1;
    aIterator->mCurNode = sNode->mMyAddr;

    return STL_SUCCESS;
}

static stlStatus smdmisScanTryFindLastByRadixComp( smdmisTableHeader  *aHeader,
                                                   smdmisIterator     *aIterator,
                                                   knlCompareList     *aMaxRange )
{
    smdmisIndexNode *sNode;
    smdmisKeyRow     sKeyRow;
    stlInt32         sMin;
    stlInt32         sMax;
    stlInt32         sMid = -1;
    dtlCompareResult sCompResult;
    stlBool          sMoveToRight = STL_TRUE;

    if( aIterator->mForMinRange == STL_TRUE )
    {
        smdmisScanTransformRange( aHeader,
                                  aIterator,
                                  aMaxRange,
                                  STL_FALSE /* maxRange */ );
    }

    sNode = SMDMIS_TO_NODE_PTR( aIterator->mCurNode );

    STL_DASSERT( sNode != NULL );

    /* node의 마지막 키에 대해 조건 검사를 한다. */
    sKeyRow = SMDMIS_NODE_GET_KEY( sNode, sNode->mKeyCount - 1 );

    sCompResult = smdmisCompareKeyByRadix( aHeader->mRadixArray,
                                           aIterator,
                                           sKeyRow,
                                           aIterator->mTransKey );

    /* 마지막 키가 max range 조건에 만족하면 현재 노드에서 max key를 찾을 순 없다. skip한다. */
    STL_TRY_THROW( sCompResult == DTL_COMPARISON_GREATER, RAMP_FINISH );

    /* 마지막 키가 max range 조건에 맞지 않으므로 binary search를 통해 max key를 찾는다. */
    sMin = aIterator->mCurIndex;
    sMax = sNode->mKeyCount - 2;
    sMoveToRight = STL_TRUE;

    while( sMin <= sMax )
    {
        sMid = (sMin + sMax) >> 1;
        sKeyRow = SMDMIS_NODE_GET_KEY( sNode, sMid );

        STL_DASSERT( sKeyRow != NULL );

        sCompResult = smdmisCompareKeyByRadix( aHeader->mRadixArray,
                                               aIterator,
                                               sKeyRow,
                                               aIterator->mTransKey );

        sMoveToRight = ( sCompResult == DTL_COMPARISON_GREATER ? STL_FALSE : STL_TRUE );

        if( sMoveToRight == STL_TRUE )
        {
            sMin = sMid + 1;
        }
        else
        {
            sMax = sMid - 1;
        }
    } /* while( sMin <= sMax ) */

    if( sMoveToRight == STL_FALSE )
    {
        sMid--;
    }

    STL_DASSERT( sMid >= -1 );

    aIterator->mLastNode = sNode->mMyAddr;
    aIterator->mLastIndex = sMid;

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;
}

static stlStatus smdmisScanTryFindLastByPhysicalComp( smdmisTableHeader  *aHeader,
                                                      smdmisIterator     *aIterator,
                                                      knlCompareList     *aMaxRange )
{
    knlCompareList  *sCompList = aMaxRange;
    smdmisIndexNode *sNode;
    smdmisKeyRow     sKeyRow;
    smdmisKeyCol     sKeyCol;
    smdmifVarCol     sVarCol;
    stlInt32         sMin;
    stlInt32         sMax;
    stlInt32         sMid = -1;
    stlChar         *sColValue = NULL;
    stlInt64         sColLen;
    stlInt32         sColIndex;
    dtlCompareResult sCompResult = DTL_COMPARISON_EQUAL;
    stlBool          sMoveToRight = STL_TRUE;
    stlBool          sSameKey = STL_FALSE;

    sNode = SMDMIS_TO_NODE_PTR( aIterator->mCurNode );

    STL_DASSERT( sNode != NULL );

    /* node의 마지막 키에 대해 조건 검사를 한다. */
    sKeyRow = SMDMIS_NODE_GET_KEY( sNode, sNode->mKeyCount - 1 );
    while( sCompList != NULL )
    {
        sColIndex = sCompList->mColOrder;

        if( sSameKey == STL_FALSE )
        {
            if( aHeader->mKeyCols[sColIndex].mIsFixedPart == STL_TRUE )
            {
                sKeyCol   = SMDMIS_KEY_ROW_GET_COL( sKeyRow, &aHeader->mKeyCols[sColIndex] );
                sColValue = SMDMIS_KEY_COL_GET_VALUE( sKeyCol );
                sColLen   = SMDMIS_KEY_COL_GET_LEN( sKeyCol );
            }
            else
            {
                sVarCol = SMDMIS_KEY_ROW_GET_VAR_COL( sKeyRow, &aHeader->mKeyCols[sColIndex] );
                if( SMDMIF_VAR_COL_HAS_NEXT( sVarCol ) == STL_TRUE )
                {
                    /* var col이 쪼개져 있는 경우 aTempMem에 복사한다. */
                    smdmifVarPartReadCol( sVarCol,
                            aHeader->mColCombineMem1,
                            &sColLen );
                    sColValue = aHeader->mColCombineMem1;
                }
                else
                {
                    sColValue = SMDMIF_VAR_COL_VALUE_POS( sVarCol );
                    sColLen   = SMDMIF_VAR_COL_GET_LEN( sVarCol );
                }
            }
        }

        sSameKey = STL_FALSE;

        if( sCompList->mRangeLen == 0 )
        {
            /* is null 또는 is not null 조건일 경우
               key가 null이냐 아니냐에 따라 left, right가 바뀐다.*/
            if( sColLen == 0 )
            {
                sMoveToRight = (sCompList->mIsNullFirst) || (sCompList->mIsIncludeEqual);
            }
            else
            {
                sMoveToRight = (!sCompList->mIsNullFirst) || (!sCompList->mIsIncludeEqual);
            }
        }
        else
        {
            if( sColLen == 0 )
            {
                sMoveToRight = sCompList->mIsNullFirst ? STL_TRUE : STL_FALSE;
            }
            else
            {
                sCompResult = sCompList->mCompFunc( sColValue,
                                                    sColLen,
                                                    sCompList->mRangeVal,
                                                    sCompList->mRangeLen );

                if( sCompResult == DTL_COMPARISON_EQUAL )
                {
                    sMoveToRight = sCompList->mIsIncludeEqual;
                }
                else
                {
                    if( sCompList->mIsAsc == STL_FALSE )
                    {
                        sCompResult = -sCompResult;
                    }
                    sMoveToRight = ( sCompResult == DTL_COMPARISON_LESS ? STL_TRUE : STL_FALSE );
                }
            }
        }

        if( sMoveToRight == STL_FALSE )
        {
            break;
        }
        else
        {
            /* 다만 컬럼이 더 클 경우, 다음 비교할 컬럼이 같은 컬럼이면 조건 검사를 더 해봐야 안다.
               예) a < 7 and a < 4 이런 조건이 오면 앞의 조건이 less를 만족하더라도,
               뒤에 조건을 봐야 하기 때문이다. */
            if( sCompList->mNext != NULL )
            {
                if( sCompList->mColOrder == sCompList->mNext->mColOrder )
                {
                    sSameKey = STL_TRUE;
                }
                else
                {
                    if( sCompResult != DTL_COMPARISON_EQUAL )
                    {
                        break;
                    }
                }
            }
        }

        sCompList = sCompList->mNext;
    } /* while( sCompList != NULL ) */

    /* 마지막 키가 max range 조건에 만족하면 현재 노드에서 max key를 찾을 순 없다. skip한다. */
    STL_TRY_THROW( sMoveToRight == STL_FALSE, RAMP_FINISH );

    /* 마지막 키가 max range 조건에 맞지 않으므로 binary search를 통해 max key를 찾는다. */
    sMin = aIterator->mCurIndex;
    sMax = sNode->mKeyCount - 2;
    sMoveToRight = STL_TRUE;

    while( sMin <= sMax )
    {
        sMid = (sMin + sMax) >> 1;
        sKeyRow = SMDMIS_NODE_GET_KEY( sNode, sMid );

        STL_DASSERT( sKeyRow != NULL );

        sCompList = aMaxRange;
        while( sCompList != NULL )
        {
            sColIndex = sCompList->mColOrder;

            if( sSameKey == STL_FALSE )
            {
                if( aHeader->mKeyCols[sColIndex].mIsFixedPart == STL_TRUE )
                {
                    sKeyCol   = SMDMIS_KEY_ROW_GET_COL( sKeyRow, &aHeader->mKeyCols[sColIndex] );
                    sColValue = SMDMIS_KEY_COL_GET_VALUE( sKeyCol );
                    sColLen   = SMDMIS_KEY_COL_GET_LEN( sKeyCol );
                }
                else
                {
                    sVarCol = SMDMIS_KEY_ROW_GET_VAR_COL( sKeyRow, &aHeader->mKeyCols[sColIndex] );
                    if( SMDMIF_VAR_COL_HAS_NEXT( sVarCol ) == STL_TRUE )
                    {
                        /* var col이 쪼개져 있는 경우 aTempMem에 복사한다. */
                        smdmifVarPartReadCol( sVarCol,
                                aHeader->mColCombineMem1,
                                &sColLen );
                        sColValue = aHeader->mColCombineMem1;
                    }
                    else
                    {
                        sColValue = SMDMIF_VAR_COL_VALUE_POS( sVarCol );
                        sColLen   = SMDMIF_VAR_COL_GET_LEN( sVarCol );
                    }
                }
            }

            sSameKey = STL_FALSE;

            if( sCompList->mRangeLen == 0 )
            {
                /* is null 또는 is not null 조건일 경우
                   key가 null이냐 아니냐에 따라 left, right가 바뀐다.*/
                if( sColLen == 0 )
                {
                    sMoveToRight = (sCompList->mIsNullFirst) || (sCompList->mIsIncludeEqual);
                }
                else
                {
                    sMoveToRight = (!sCompList->mIsNullFirst) || (!sCompList->mIsIncludeEqual);
                }
            }
            else
            {
                if( sColLen == 0 )
                {
                    sMoveToRight = sCompList->mIsNullFirst ? STL_TRUE : STL_FALSE;
                }
                else
                {
                    sCompResult = sCompList->mCompFunc( sColValue,
                                                        sColLen,
                                                        sCompList->mRangeVal,
                                                        sCompList->mRangeLen );

                    if( sCompResult == DTL_COMPARISON_EQUAL )
                    {
                        sMoveToRight = sCompList->mIsIncludeEqual;
                    }
                    else
                    {
                        if( sCompList->mIsAsc == STL_FALSE )
                        {
                            sCompResult = -sCompResult;
                        }
                        sMoveToRight = ( sCompResult == DTL_COMPARISON_LESS ? STL_TRUE : STL_FALSE );
                    }
                }
            }

            if( sMoveToRight == STL_FALSE )
            {
                break;
            }
            else
            {
                /* 다만 컬럼이 더 클 경우, 다음 비교할 컬럼이 같은 컬럼이면 조건 검사를 더 해봐야 안다.
                   예) a < 7 and a < 4 이런 조건이 오면 앞의 조건이 less를 만족하더라도,
                   뒤에 조건을 봐야 하기 때문이다. */
                if( sCompList->mNext != NULL )
                {
                    if( sCompList->mColOrder == sCompList->mNext->mColOrder )
                    {
                        sSameKey = STL_TRUE;
                    }
                    else
                    {
                        if( sCompResult != DTL_COMPARISON_EQUAL )
                        {
                            break;
                        }
                    }
                }
            }

            sCompList = sCompList->mNext;
        } /* while( sCompList != NULL ) */

        if( sMoveToRight == STL_TRUE )
        {
            sMin = sMid + 1;
        }
        else
        {
            sMax = sMid - 1;
        }
    } /* while( sMin <= sMax ) */

    if( sMoveToRight == STL_FALSE )
    {
        sMid--;
    }

    STL_DASSERT( sMid >= -1 );

    aIterator->mLastNode = sNode->mMyAddr;
    aIterator->mLastIndex = sMid;

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;
}

stlBool smdmisScanFilt( smdmisTableHeader  *aHeader,
                        smdmisKeyRow        aKeyRow,
                        smlFetchInfo       *aFetchInfo )
{
    void              *sValue;
    stlInt64           sLength;
    smdmifDefColumn   *sColInfo;
    smdmisKeyCol       sCol;
    smdmifVarCol       sVarCol;
    knlValueBlockList *sColList;
    knlPhysicalFilter *sPhysicalFilter;

    sColList = aFetchInfo->mColList;
    sPhysicalFilter = aFetchInfo->mPhysicalFilter;
    
    while( sColList != NULL )
    {
        sColInfo = &aHeader->mKeyCols[sColList->mColumnOrder];
        if( sColInfo->mIsFixedPart == STL_TRUE )
        {
            sCol = SMDMIS_KEY_ROW_GET_COL( aKeyRow, sColInfo );
            sValue = SMDMIS_KEY_COL_GET_VALUE( sCol );
            sLength = SMDMIS_KEY_COL_GET_LEN( sCol );
        }
        else
        {
            sVarCol = SMDMIS_KEY_ROW_GET_VAR_COL( aKeyRow, sColInfo );
            if( SMDMIF_VAR_COL_HAS_NEXT( sVarCol ) == STL_TRUE )
            {
                /* var col이 쪼개져 있는 경우 aTempMem에 복사한다. */
                smdmifVarPartReadCol( sVarCol,
                                      aHeader->mColCombineMem1,
                                      &sLength );
                sValue = aHeader->mColCombineMem1;
            }
            else
            {
                sValue = SMDMIF_VAR_COL_VALUE_POS( sVarCol );
                sLength = SMDMIF_VAR_COL_GET_LEN( sVarCol );
            }
        }

        while( sPhysicalFilter != NULL )
        {
            if( sPhysicalFilter->mColIdx1 <= sColList->mColumnOrder )
            {
                if( sPhysicalFilter->mFunc( sValue,
                                            sLength,
                                            sPhysicalFilter->mColVal2->mValue,
                                            sPhysicalFilter->mColVal2->mLength ) == STL_FALSE )
                {
                    STL_THROW( RAMP_KEY_SKIP );
                }
            }
            else
            {
                break;
            }

            sPhysicalFilter = sPhysicalFilter->mNext;
        }

        sColList = sColList->mNext;
    }

    return STL_TRUE;

    STL_RAMP( RAMP_KEY_SKIP );

    return STL_FALSE;
}

/**
 * @brief 일반 Scan을 위해 열린 Iterator에서 이전 row를 반환한다
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 */
stlStatus smdmisScanFetchPrev( void          * aIterator,
                               smlFetchInfo  * aFetchInfo,
                               smlEnv        * aEnv )
{
    return STL_SUCCESS;
}

/** @} */

