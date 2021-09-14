/*******************************************************************************
 * smdmisGroupScan.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: 
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smdmisGroupScan.c
 * @brief Storage Manager Layer Memory Instant Sort Table Routines
 */

#include <dtl.h>
#include <sml.h>
#include <smDef.h>
#include <sma.h>
#include <smdmifScan.h>
#include <smdmisGroupScan.h>
#include <smdmifFixedPart.h>
#include <smdmifTable.h>
#include <smdmisTable.h>

/**
 * @addtogroup smdmisGroupScan
 * @{
 */

smeGroupIteratorModule gSmdmisGroupIteratorModule =
{
    STL_SIZEOF(smdmisGroupIterator),

    smdmisGroupScanInitIterator,
    NULL, /* FiniIterator */
    smdmisGroupScanResetIterator,
    smdmifScanMoveToRowRid,
    smdmifScanMoveToPosNum,
    {
        smdmisGroupScanFetchNext,
        smdmisGroupScanFetchPrev,
        NULL,
        NULL
    },
    smdmisGroupScanNextGroup,
    smdmisGroupScanResetGroup
};

/**
 * @brief 주어진 Iterator를 초기화한다
 * @param[in,out] aIterator 초기화할 iterator
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmisGroupScanInitIterator( void   *aIterator,
                                       smlEnv *aEnv )
{
    smdmisGroupIterator  *sIterator = (smdmisGroupIterator *)aIterator;
    smdmisTableHeader    *sHeader = SMDMIS_GROUP_ITERATOR_GET_TABLE_HEADER( sIterator );

    SMDMIS_GROUP_ITERATOR_INIT( sIterator );

    if( SMDMIS_TABLE_INDEX_BUILT( sHeader ) == STL_FALSE )
    {
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
stlStatus smdmisGroupScanResetIterator( void   *aIterator,
                                        smlEnv *aEnv )
{
    smdmisGroupIterator *sIterator = (smdmisGroupIterator *)aIterator;

    SMDMIS_GROUP_ITERATOR_RESET( sIterator );

    return STL_SUCCESS;
}

/**
 * @brief 일반 Scan을 위해 열린 Iterator에서 다음 row를 반환한다
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 */
stlStatus smdmisGroupScanFetchNext( void          * aIterator,
                                    smlFetchInfo  * aFetchInfo,
                                    smlEnv        * aEnv )
{
    smdmisGroupIterator *sIterator = (smdmisGroupIterator *)aIterator;
    smdmisTableHeader   *sHeader = SMDMIS_GROUP_ITERATOR_GET_TABLE_HEADER( sIterator );
    smdmisIndexNode     *sNode;
    smdmisKeyRow         sKeyRow;
    smdmifFixedRow       sFixedRow;
    stlInt64             sBlockSize = SML_BLOCK_SIZE( aFetchInfo->mRowBlock );
    stlInt32             sBlockIdx  = 0;
    knlLogicalAddr       sRowAddr;
    smlRid              *sRid;
    stlBool              sFetched;
    knlPhysicalFilter   *sRowPhysicalFilter;
    knlExprEvalInfo     *sRowLogicalFilterInfo;
    knlValueBlockList   *sRowFetchCol;

    SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );

    if( SMDMIS_GROUP_ITERATOR_IS_BEFORE_FIRST( sIterator ) == STL_TRUE )
    {
        aFetchInfo->mIsEndOfScan = STL_TRUE;
    }
    else
    {
        aFetchInfo->mIsEndOfScan = STL_FALSE;
        sNode = SMDMIS_TO_NODE_PTR( sIterator->mCurNode );

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

        while( STL_TRUE )
        {
            if( (aFetchInfo->mSkipCnt == 0) && (aFetchInfo->mFetchCnt == 0 || sBlockIdx >= sBlockSize ) )
            {
                break;
            }

            /* 다음 key를 얻는다. */
            if( SMDMIS_GROUP_ITERATOR_IS_END( sIterator ) == STL_TRUE )
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
                                                      (smdmisIterator *)sIterator,
                                                      aFetchInfo->mRange->mMaxRange ) == STL_SUCCESS );

                    /* iteration end 조건을 다시 검사하기 위해 continue를 한다. */
                    continue;
                }
            }

            sKeyRow = SMDMIS_NODE_GET_KEY( sNode, sIterator->mCurIndex );
            sIterator->mCurIndex++;

            if( SMDMIS_KEY_ROW_IS_DUP( sKeyRow ) == STL_FALSE )
            {
                /* key의 dup가 0이면 새로운 key group이 시작되었다는 의미이다. */
                if( sIterator->mGroupStart == STL_FALSE )
                {
                    /* 다음 group의 key이다. 이 key는 다음 group에서 읽어야 하므로 curIndex를 뒤로 옮긴다. */
                    sIterator->mCurIndex--;
                    aFetchInfo->mIsEndOfScan = STL_TRUE;
                    break;
                }
                else
                {
                    /* 현재 group의 첫번째 key이므로 읽을 수 있다. */
                    sIterator->mGroupStart = STL_FALSE;
                }
            }

            sFixedRow = SMDMIS_KEY_ROW_GET_FIXED_ROW( sKeyRow );
                             
            /* 현재 row가 delete되었는지 검사한다. */
            if( SMDMIF_FIXED_ROW_IS_DELETED( sFixedRow ) == STL_TRUE )
            {
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
                    continue;
                }
            }

            /* key column을 제외한 컬럼을 읽어들인다. */
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

            if( aFetchInfo->mSkipCnt > 0 )
            {
                aFetchInfo->mSkipCnt--;
                continue;
            }

            sRowAddr = SMDMIS_KEY_ROW_GET_FIXED_ROW_ADDR( sKeyRow );

            sRid = (smlRid *)(&sRowAddr);

            SML_SET_RID_VALUE( aFetchInfo->mRowBlock, sBlockIdx, *sRid );

            /* row를 읽었으니 blockIdx와 fetchCnt를 갱신한다. */
            sBlockIdx++;
            aFetchInfo->mFetchCnt--;
        }

        SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sBlockIdx );

        if( aFetchInfo->mColList != NULL )
        {
            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aFetchInfo->mColList, 0, sBlockIdx );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 일반 Scan을 위해 열린 Iterator에서 이전 row를 반환한다
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 */
stlStatus smdmisGroupScanFetchPrev( void          * aIterator,
                                    smlFetchInfo  * aFetchInfo,
                                    smlEnv        * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief 다음 key group으로 이동한다.
 * @param[in] aIterator 대상 iterator
 * @param[out] aFetchInfo Fetch Info
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmisGroupScanNextGroup( void          *aIterator,
                                    smlFetchInfo  *aFetchInfo,
                                    smlEnv        *aEnv )
{
    smdmisGroupIterator *sIterator = (smdmisGroupIterator *)aIterator;
    smdmisTableHeader   *sHeader = SMDMIS_GROUP_ITERATOR_GET_TABLE_HEADER( sIterator );
    smdmisIndexNode     *sNode;
    smdmisKeyRow         sKeyRow;
    smdmifFixedRow       sFixedRow;
    stlBool              sFetched;

    aFetchInfo->mIsEndOfScan = STL_FALSE;

    if( SMDMIS_GROUP_ITERATOR_IS_BEFORE_FIRST( sIterator ) == STL_TRUE )
    {
        if( SMDMIF_TABLE_FIRST_INSERTED( &sHeader->mBaseHeader ) == STL_FALSE )
        {
            /*
             * 한건도 insert되지 않았으면 바로 EOS이다.
             */
            aFetchInfo->mIsEndOfScan = STL_TRUE;
            STL_THROW( RAMP_FINISH );
        }

        STL_TRY( smdmisScanFindFirstAndLast( sHeader,
                                             (smdmisIterator *)sIterator,
                                             aFetchInfo,
                                             aEnv ) == STL_SUCCESS );
    }

    sNode = SMDMIS_TO_NODE_PTR( sIterator->mCurNode );
    while( STL_TRUE )
    {
        /* 다음 key를 얻는다. */
        if( SMDMIS_GROUP_ITERATOR_IS_END( sIterator ) == STL_TRUE )
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
                                                  (smdmisIterator *)sIterator,
                                                  aFetchInfo->mRange->mMaxRange ) == STL_SUCCESS );

                continue;
            }
        }

        sKeyRow = SMDMIS_NODE_GET_KEY( sNode, sIterator->mCurIndex );
        sIterator->mCurIndex++;

        if( SMDMIS_KEY_ROW_IS_DUP( sKeyRow ) == STL_FALSE )
        {
            /* key의 dup가 0이면 새로운 key group이 시작되었다는 의미이다. */
            if( sIterator->mGroupStart == STL_FALSE )
            {
                sFixedRow = SMDMIS_KEY_ROW_GET_FIXED_ROW( sKeyRow );

                /* 현재 row가 delete되었는지 검사한다. */
                if( SMDMIF_FIXED_ROW_IS_DELETED( sFixedRow ) == STL_TRUE )
                {
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
                        continue;
                    }
                }

                /* key 컬럼을 읽어들인다. */
                if( aFetchInfo->mGroupKeyFetch == STL_TRUE )
                {
                    STL_TRY( smdmifTableFetchOneRow( (smdmifTableHeader *)sHeader,
                                                     sFixedRow,
                                                     aFetchInfo->mPhysicalFilter,
                                                     aFetchInfo->mFilterEvalInfo,
                                                     aFetchInfo->mColList,
                                                     NULL, /* row physical filter */
                                                     NULL, /* row logical filter */
                                                     NULL, /* row fetch columns */
                                                     0, /* block idx */
                                                     &sFetched,
                                                     aEnv ) == STL_SUCCESS );

                    if( sFetched == STL_FALSE )
                    {
                        continue;
                    }

                    SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, 1 );
                }

                /* 그룹, 현재 key 정보를 세팅하여 fetchNext할 수 있도록 한다. */
                sIterator->mGroupStart = STL_TRUE;
                sIterator->mCurIndex--;

                SMDMIS_GROUP_ITERATOR_MARK_CURRENT( sIterator );

                break;
            }
            else
            {
                /* 현재 group의 첫번째 key이므로 skip한다. */
                sIterator->mGroupStart = STL_FALSE;
            }
        }
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmisGroupScanResetGroup( void          *aIterator,
                                     smlEnv        *aEnv )
{
    smdmisGroupIterator *sIterator = (smdmisGroupIterator *)aIterator;

    SMDMIS_GROUP_ITERATOR_RETURN_TO_MARK( sIterator );
    sIterator->mGroupStart = STL_TRUE;

    return STL_SUCCESS;
}

/** @} */

