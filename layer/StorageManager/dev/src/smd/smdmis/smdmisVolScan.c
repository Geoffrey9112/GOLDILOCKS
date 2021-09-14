/*******************************************************************************
 * smdmisVolScan.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmisVolScan.c 6124 2012-10-29 10:31:50Z htkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smdmisVolScan.c
 * @brief Storage Manager Layer Memory Instant Sort Table Routines
 */

#include <dtl.h>
#include <sml.h>
#include <smDef.h>
#include <smo.h>
#include <smdmifScan.h>
#include <smdmisVolScan.h>
#include <smdmifFixedPart.h>
#include <smdmifTable.h>
#include <smdmisTable.h>

/**
 * @addtogroup smdmisVolScan
 * @{
 */

smeIteratorModule gSmdmisVolIteratorModule =
{
    STL_SIZEOF(smdmisVolIterator),

    smdmisVolScanInitIterator,
    NULL,     /* finiIterator */
    smdmisVolScanResetIterator,
    smdmifScanMoveToRowRid,
    smdmisVolScanMoveToPosNum,
    {
        smdmisVolScanFetchNext,
        smdmisVolScanFetchPrev,
        (smiFetchAggrFunc)smdmisVolScanFetchNext,
        NULL    /* FetchSampling */
    }
};

/**
 * @brief 주어진 Iterator를 초기화한다
 * @param[in,out] aIterator 초기화할 iterator
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmisVolScanInitIterator( void   *aIterator,
                                     smlEnv *aEnv )
{
    smdmisVolIterator *sIterator = (smdmisVolIterator *)aIterator;
    smdmisTableHeader *sHeader = SMDMIS_VOL_TABLE_ITERATOR_GET_TABLE_HEADER( sIterator );

    STL_DASSERT( SMDMIS_TABLE_IS_VOLATILE( sHeader ) == STL_TRUE );

    STL_TRY( smdmisTableBuildMergeEnv( sIterator->mCommon.mStatement,
                                       sHeader,
                                       &sIterator->mMergeEnv,
                                       aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 Iterator를 다시 초기화한다
 * @param[in,out] aIterator         초기화할 iterator
 * @param[in]     aEnv              Storage Manager Environment
 */
stlStatus smdmisVolScanResetIterator( void   *aIterator,
                                      smlEnv *aEnv )
{
    smdmisVolIterator *sIterator = (smdmisVolIterator *)aIterator;
    smdmisTableHeader *sHeader = SMDMIS_VOL_TABLE_ITERATOR_GET_TABLE_HEADER( sIterator );

    smdmisTableReadyToMerge( sHeader,
                             sIterator->mMergeEnv );

    return STL_SUCCESS;
}

/**
 * @brief 일반 Scan을 위해 열린 Iterator에서 다음 row를 반환한다
 * @param[in]     aIterator 이전에 읽은 위치를 가진 iterator
 * @param[in,out] aFetchInfo Fetch Info
 * @param[in]     aEnv Storage Manager Environment
 */
stlStatus smdmisVolScanFetchNext( void              *aIterator,
                                  smlFetchInfo      *aFetchInfo,
                                  smlEnv            *aEnv )
{
    smdmisVolIterator   *sIterator = (smdmisVolIterator *)aIterator;
    smdmisTableHeader   *sHeader = SMDMIS_VOL_TABLE_ITERATOR_GET_TABLE_HEADER( sIterator );
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
    stlInt32             sAggrRowCount = 0;
    
    SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );

    aFetchInfo->mIsEndOfScan = STL_FALSE;
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
        sKeyRow = smdmisTableGetNext( sHeader, sIterator->mMergeEnv );

        if( sKeyRow == NULL )
        {
            aFetchInfo->mIsEndOfScan = STL_TRUE;
            break;
        }

        sFixedRow = SMDMIS_KEY_ROW_GET_FIXED_ROW( sKeyRow );

        /* 현재 row가 delete되었는지 검사한다. */
        if( SMDMIF_FIXED_ROW_IS_DELETED( sFixedRow ) == STL_TRUE )
        {
            continue;
        }

        /* 성능을 위해 aFetchInfo->mRowFlag를 체크하지 않는다. */

        /* 실제 row를 읽어들인다. */
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

    if( aFetchInfo->mAggrFetchInfo == NULL )
    {
        SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sBlockIdx );
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
 * @brief 일반 Scan을 위해 열린 Iterator에서 이전 row를 반환한다
 * @param[in]     aIterator 이전에 읽은 위치를 가진 iterator
 * @param[in,out] aFetchInfo Fetch Info
 * @param[in]     aEnv Storage Manager Environment
 */
stlStatus smdmisVolScanFetchPrev( void              *aIterator,
                                  smlFetchInfo      *aFetchInfo,
                                  smlEnv            *aEnv )
{
    return STL_SUCCESS;
}

/**                           
 * @brief 주어진 Row position number로 Iterator의 위치를 이동시킨다
 * @param[in] aIterator 조사할 iterator
 * @param[in] aPosNum 이동시킬 position number
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmisVolScanMoveToPosNum( void     *aIterator,
                                     stlInt64  aPosNum,
                                     smlEnv   *aEnv )
{
    /* volatile sort table은 position number로의 row 이동을 지원하지 않는다. */
    return STL_SUCCESS;
}

/** @} */

