/*******************************************************************************
 * smdmifScan.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
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
 * @file smdmifScan.c
 * @brief Storage Manager Layer Memory Instant Flat Table Routines
 */

#include <dtl.h>
#include <sml.h>
#include <smDef.h>
#include <smo.h>
#include <smdmifScan.h>
#include <smdmifFixedPart.h>
#include <smdmifTable.h>

/**
 * @addtogroup smdmifScan
 * @{
 */

smeIteratorModule gSmdmifIteratorModule =
{
    STL_SIZEOF(smdmifIterator),

    smdmifScanInitIterator,
    NULL,     /* finiIterator */
    smdmifScanResetIterator,
    smdmifScanMoveToRowRid,
    smdmifScanMoveToPosNum,
    {
        smdmifScanFetchNext,
        smdmifScanFetchPrev,
        (smiFetchAggrFunc)smdmifScanFetchNext,
        NULL    /* FetchSampling */
    }
};

static void smdmifScanSetCurrent( smdmifIterator *aIterator,
                                  knlLogicalAddr  aFixedPartAddr,
                                  stlInt32        aIndex )
{
    aIterator->mCurFixedPartAddr = aFixedPartAddr;
    aIterator->mCurFixedRowIndex = aIndex;
}

/**
 * @brief 주어진 Iterator를 초기화한다
 * @param[in,out] aIterator 초기화할 iterator
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmifScanInitIterator( void   *aIterator,
                                  smlEnv *aEnv )
{
    smdmifIterator    *sIterator = (smdmifIterator *)aIterator;
    smdmifTableHeader *sTableHeader = SMDMIF_SCAN_GET_TABLE_HEADER( sIterator );
    knlLogicalAddr     sFixedPartAddr;

    if( SMDMIF_TABLE_FIRST_INSERTED( sTableHeader ) == STL_TRUE )
    {
        STL_DASSERT( smdmifTableVerify( sTableHeader ) == STL_SUCCESS );

        sFixedPartAddr = SMDMIF_TABLE_GET_FIRST_FIXED_PART_ADDR( sTableHeader );
    }
    else
    {
        /* record가 한건도 insert되지 않은 상태에서 iterator를 생성한 경우이다. */
        sFixedPartAddr = KNL_LOGICAL_ADDR_NULL;
    }

    smdmifScanSetCurrent( sIterator,
                          sFixedPartAddr,
                          SMDMIF_BEFORE_FIRST_ROW_INDEX );

    return STL_SUCCESS;
}

/**
 * @brief 주어진 Iterator를 다시 초기화한다
 * @param[in,out] aIterator         초기화할 iterator
 * @param[in]     aEnv              Storage Manager Environment
 */
stlStatus smdmifScanResetIterator( void   *aIterator,
                                   smlEnv *aEnv )
{
    STL_TRY( smdmifScanInitIterator( aIterator,
                                     aEnv ) == STL_SUCCESS );

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
stlStatus smdmifScanFetchNext( void          * aIterator,
                               smlFetchInfo  * aFetchInfo,
                               smlEnv        * aEnv )
{
    smdmifIterator    *sIterator = (smdmifIterator *)aIterator;
    void              *sFixedPart;
    smdmifFixedRow     sRow;
    stlInt64           sBlockSize = SML_BLOCK_SIZE( aFetchInfo->mRowBlock );
    stlInt32           sBlockIdx  = 0;
    knlLogicalAddr     sRowAddr;
    knlLogicalAddr     sFixedPartAddr;
    smlRid            *sRid;
    stlInt32           sFixedRowCount;
    stlBool            sFetched;
    stlInt32           sAggrRowCount = 0;

    SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );

    aFetchInfo->mIsEndOfScan = STL_FALSE;

    if( sIterator->mCurFixedPartAddr == KNL_LOGICAL_ADDR_NULL )
    {
        if( SMDMIF_TABLE_FIRST_INSERTED( SMDMIF_SCAN_GET_TABLE_HEADER(sIterator) ) == STL_TRUE )
        {
            smdmifScanSetCurrent( sIterator,
                                  SMDMIF_TABLE_GET_FIRST_FIXED_PART_ADDR( SMDMIF_SCAN_GET_TABLE_HEADER(sIterator) ),
                                  SMDMIF_BEFORE_FIRST_ROW_INDEX );

            /* 한건이라도 insert가 되었다면 fixed part가 NULL이 아니어야 한다. */
            STL_DASSERT( sIterator->mCurFixedPartAddr != KNL_LOGICAL_ADDR_NULL );
        }
        else
        {
            aFetchInfo->mIsEndOfScan = STL_TRUE;
            STL_THROW( RAMP_FINISH );
        }
    }

    sFixedPart = KNL_TO_PHYSICAL_ADDR( sIterator->mCurFixedPartAddr );
    STL_DASSERT( sFixedPart != NULL );

    sFixedRowCount = SMDMIF_FIXED_PART_GET_ROW_COUNT( sFixedPart );

    while( STL_TRUE )
    {
        if( sIterator->mCurFixedRowIndex + 1 >= sFixedRowCount )
        {
            /* 현재 block에서 더이상 읽을 row가 없다. 이땐 다음 block으로 가야 한다. */

            sFixedPartAddr = SMDMIF_FIXED_PART_GET_NEXT_FIXED_PART_ADDR( sFixedPart );

            if( sFixedPartAddr == KNL_LOGICAL_ADDR_NULL )
            {
                /* 다음 fixed part가 없다. 이때엔 현재 fixed part에 머무른다. */
                aFetchInfo->mIsEndOfScan = STL_TRUE;
                break;
            }
            else
            {
                smdmifScanSetCurrent( sIterator,
                                      sFixedPartAddr,
                                      SMDMIF_BEFORE_FIRST_ROW_INDEX );

                sFixedPart = KNL_TO_PHYSICAL_ADDR( sIterator->mCurFixedPartAddr );
                sFixedRowCount = SMDMIF_FIXED_PART_GET_ROW_COUNT( sFixedPart );

                continue;
            }
        }

        sIterator->mCurFixedRowIndex++;

        /* 현재 위치의 row를 읽을 수 있다. */
        sRow = SMDMIF_FIXED_PART_GET_ROW( sFixedPart,
                                          sIterator->mCurFixedRowIndex );

        /* 현재 row가 delete되었는지 검사한다. */
        if( SMDMIF_FIXED_ROW_IS_DELETED( sRow ) == STL_TRUE )
        {
            continue;
        }

        if( aFetchInfo->mFetchCnt <= 0 || sBlockIdx >= sBlockSize )
        {
            /* 애시당초 FetchNext를 호출할 때부터 Fetch count를 0으로 주었거나,
               blockIdx가 blockSize보다 큰 경우이다. */
            break;
        }

        /* 실제 row를 읽어들인다. */
        STL_TRY( smdmifTableFetchOneRow( SMDMIF_SCAN_GET_TABLE_HEADER(sIterator),
                                         sRow,
                                         NULL, /* key physical filter */
                                         NULL, /* key logical filter */
                                         NULL, /* key fetch columns */
                                         aFetchInfo->mPhysicalFilter,
                                         aFetchInfo->mFilterEvalInfo,
                                         aFetchInfo->mColList,
                                         sBlockIdx,
                                         &sFetched,
                                         aEnv ) == STL_SUCCESS );

        if( sFetched == STL_FALSE )
        {
            continue;
        }

        /* evaluate aggregation */
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

            /* 읽은 row의 RID를 세팅한다. */
            sRowAddr = SMDMIF_FIXED_PART_GET_ROW_ADDR( sFixedPart,
                                                       sIterator->mCurFixedRowIndex );

            sRid = (smlRid *)(&sRowAddr);

            SML_SET_RID_VALUE( aFetchInfo->mRowBlock, sBlockIdx, *sRid );

            /* row를 읽었으니 blockIdx와 fetchCnt를 갱신하고 종료조건 검사를 한다. */
            sBlockIdx++;
            aFetchInfo->mFetchCnt--;

            if( aFetchInfo->mFetchCnt == 0 || sBlockIdx == sBlockSize )
            {
                /* row가 있어도 더이상 fetch할 수 없다.
                   row가 더 남았는지 여부를 검사해서 aIsEndOfScan을 설정해야 한다. */
                if( (sIterator->mCurFixedRowIndex == sFixedRowCount - 1) &&
                    (SMDMIF_FIXED_PART_GET_NEXT_FIXED_PART_ADDR( sFixedPart ) == KNL_LOGICAL_ADDR_NULL) )
                {
                    aFetchInfo->mIsEndOfScan = STL_TRUE;
                }
                break;
            }
        }
    }

    STL_RAMP( RAMP_FINISH );

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
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 */
stlStatus smdmifScanFetchPrev( void          * aIterator,
                               smlFetchInfo  * aFetchInfo,
                               smlEnv        * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief 주어진 Row Rid로 Iterator의 위치를 이동시킨다
 * @param[in] aIterator 조사할 iterator
 * @param[in] aRowRid 이동시킬 위치
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmifScanMoveToRowRid( void   *aIterator,
                                  smlRid *aRowRid,
                                  smlEnv *aEnv )
{
    /* instant table은 row rid로 이동시키는 것을 지원하지 않는다. */
    return STL_SUCCESS;
}

/**
 * @brief 주어진 Row position number로 Iterator의 위치를 이동시킨다
 * @param[in] aIterator 조사할 iterator
 * @param[in] aPosNum 이동시킬 position number
 * @param[in] aEnv Storage Manager Environment
 */ 
stlStatus smdmifScanMoveToPosNum( void     *aIterator,
                                  stlInt64  aPosNum,
                                  smlEnv   *aEnv )
{
    smdmifIterator    *sIterator = (smdmifIterator *)aIterator;
    stlInt64           sRowNum = aPosNum;
    void              *sFixedPart;

    STL_DASSERT( smdmifTableVerify( SMDMIF_SCAN_GET_TABLE_HEADER( sIterator ) )
                 == STL_SUCCESS );

    smdmifTableFindFixedPart( SMDMIF_SCAN_GET_TABLE_HEADER(sIterator),
                              &sRowNum,
                              &sFixedPart,
                              aEnv );

    STL_TRY_THROW( sFixedPart != NULL, RAMP_ERR_NO_ROW );

    smdmifScanSetCurrent( sIterator,
                          SMDMIF_FIXED_PART_GET_MY_ADDR( sFixedPart ),
                          sRowNum - 1 ); /* 찾은 row의 이전에 위치시키기 위해 -1을 한다. */

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NO_ROW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      "- invalid row position number(%ld)",
                      KNL_ERROR_STACK( aEnv ),
                      aPosNum );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */

