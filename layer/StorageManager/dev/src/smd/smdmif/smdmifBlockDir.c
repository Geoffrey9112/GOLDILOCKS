/*******************************************************************************
 * smdmifBlockDir.c
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
 * @file smdmifBlockDir.c
 * @brief Storage Manager Layer Memory Instant Flat Table Variable Part Routines
 */

#include <sml.h>
#include <smDef.h>
#include <smdmifDef.h>
#include <smdmifBlockDir.h>
#include <smdmifFixedPart.h>

/**
 * @addtogroup smdmifBlockDir
 * @{
 */

/* block dir이 몇 level까지 갈 수 있느냐를 정의한다.
   block이 256K라면, 한 block에 item이 32K개 들어가며,
   이 경우 level-4로 커버가능한 row의 개수는 32K * 32K * 32K * 32K이다.
   block이 8K라 해도, 1K * 1K * 1K * 1K = 약 1조개이므로 level 4면 충분하다. */
#define SMDMIF_BLOCK_DIR_MAX_LEVEL  4

static stlStatus smdmifBlockDirCheckParent( void                        *aTableHandle,
                                            smdmifBlockDirHeader        *aBlockDir,
                                            smdmifBlockDirCallbackAlloc  aAllocBlockDir,
                                            smlEnv                      *aEnv )
{
    void          *sNewBlockDir;

    if( aBlockDir->mParent == KNL_LOGICAL_ADDR_NULL )
    {
        STL_TRY( aAllocBlockDir( aTableHandle,
                                 &sNewBlockDir,
                                 KNL_LOGICAL_ADDR_NULL,  /* parent block dir addr */
                                 aBlockDir->mLevel + 1,
                                 aEnv ) == STL_SUCCESS );

        /* sBlockDir의 parent를 설정한다. */
        aBlockDir->mParent = SMDMIF_DEF_BLOCK_ADDR(sNewBlockDir);

        SMDMIF_BLOCK_DIR_ADD_DIR_KEY( (smdmifBlockDirHeader *)sNewBlockDir, SMDMIF_DEF_BLOCK_ADDR(aBlockDir) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief block directory header를 초기화한다.
 * @param[in] aBlockDir 초기화할 block directory memory 주소
 * @param[in] aParentDirAddr 초기화할 block dir의 부모 dir의 주소
 * @param[in] aLevel 초기화할 block dir의 레벨. leaf가 1이고 위로 갈 수록 1씩 증가한다.
 * @param[in] aBlockSize 주어진 block memory의 크기
 */
stlStatus smdmifBlockDirInit( void           *aBlockDir,
                              knlLogicalAddr  aParentDirAddr,
                              stlInt16        aLevel,
                              stlInt32        aBlockSize )
{
    smdmifBlockDirHeader *sBlockDir = (smdmifBlockDirHeader *)aBlockDir;

    sBlockDir->mMaxDirCount = ( aBlockSize - STL_OFFSETOF( smdmifBlockDirHeader, mDir ) ) /
                              STL_SIZEOF( knlLogicalAddr );

    sBlockDir->mLevel = aLevel;
    sBlockDir->mParent = aParentDirAddr;

    return STL_SUCCESS;
}

/**
 * @brief block directory에 item(block의 논리 주소)를 추가한다.
 * @param[in] aTableHandle block directory를 새로 할당받을 때 callback에 사용되는 인자
 * @param[in] aBlockDir 대상 block directory
 * @param[in] aChildBlockAddr insert할 item(block의 논리 주소)
 * @param[in] aAllocBlockDir block directory를 새로 할당받을 때 사용할 callback 함수
 * @param[in] aEnv block directory를 새로 할당받을 때 callback에 사용되는 인자
 */
stlStatus smdmifBlockDirInsertKey( void                        *aTableHandle,
                                   void                        *aBlockDir,
                                   knlLogicalAddr               aChildBlockAddr,
                                   smdmifBlockDirCallbackAlloc  aAllocBlockDir,
                                   smlEnv                      *aEnv )
{
    smdmifBlockDirHeader *sNewBlockDir;
    smdmifBlockDirHeader *sBlockDir = (smdmifBlockDirHeader *)aBlockDir;

    if( SMDMIF_DEF_BLOCK_ITEM_COUNT(sBlockDir) == sBlockDir->mMaxDirCount )
    {
        /* 현재 directory가 가득찼다. */

        /* 현재 directory의 parent를 확보한다. */
        STL_TRY( smdmifBlockDirCheckParent( aTableHandle,
                                            sBlockDir,
                                            aAllocBlockDir,
                                            aEnv ) == STL_SUCCESS );

        /* 새로운 directory를 할당받는다. */
        STL_TRY( aAllocBlockDir( aTableHandle,
                                 (void**)&sNewBlockDir,
                                 sBlockDir->mParent,
                                 sBlockDir->mLevel,
                                 aEnv ) == STL_SUCCESS );

        /* 새로 할당 받은 directory를 parent에 넣는다. */
        STL_TRY( smdmifBlockDirInsertKey( aTableHandle,
                                          KNL_TO_PHYSICAL_ADDR(sNewBlockDir->mParent),
                                          SMDMIF_DEF_BLOCK_ADDR(sNewBlockDir),
                                          aAllocBlockDir,
                                          aEnv ) == STL_SUCCESS );

        sBlockDir = sNewBlockDir;
    }

    SMDMIF_BLOCK_DIR_ADD_DIR_KEY( sBlockDir, aChildBlockAddr );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 block directory로부터 해당 position number의 item을 찾는다.
 * @param[in] aBlockDirAddr root block directory
 * @param[in] aFixedPartMaxRowCount fixed part에 들어갈 수 있는 최대 row 개수
 * @param[in,out] aRowNum 찾을 position number. 찾은 item(block directory)에
 * @param[in] aEnv Environment
 */
void* smdmifBlockDirFindItem( knlLogicalAddr   aBlockDirAddr,
                              stlInt32         aFixedPartMaxRowCount,
                              stlInt64        *aRowNum,
                              smlEnv          *aEnv )
{
    smdmifBlockDirHeader  *sHeader;
    stlInt64               sRowNum = *aRowNum;
    stlFloat64             sCount;
    stlInt32               sIndex;
    knlLogicalAddr         sAddr;

    /*
     * aRowNum은 0-base이다. 즉 첫번째 row를 가리키기 위해 0으로 들어온다.
     */
    sHeader = (smdmifBlockDirHeader *)KNL_TO_PHYSICAL_ADDR( aBlockDirAddr );

    while( STL_TRUE )
    {
        stlPow( sHeader->mMaxDirCount,
                sHeader->mLevel - 1,
                &sCount,
                KNL_ERROR_STACK( aEnv ) );

        sCount *= aFixedPartMaxRowCount;

        sIndex = sRowNum / ((stlInt64)sCount);
        sRowNum = sRowNum % ((stlInt64)sCount);

        if( sIndex > SMDMIF_DEF_BLOCK_ITEM_COUNT(sHeader) )
        {
            /* 찾고자 하는 dir이 없다. */
            return NULL;
        }

        sAddr = SMDMIF_BLOCK_DIR_GET_DIR( sHeader, sIndex );

        if( sHeader->mLevel == 1 )
        {
            break;
        }

        sHeader = (smdmifBlockDirHeader *)KNL_TO_PHYSICAL_ADDR( sAddr );
    }

    /* 남은 row 개수를 리턴한다. */
    *aRowNum = sRowNum;

    return KNL_TO_PHYSICAL_ADDR( sAddr );
}

stlStatus smdmifBlockDirVerify( void *aBlockDir )
{
    smdmifBlockDirHeader  *sBlockDir;
    smdmifBlockDirHeader  *sChildDir;
    smdmifFixedPartHeader *sFixedPart;
    stlInt32               i;

    sBlockDir = (smdmifBlockDirHeader *)aBlockDir;

    STL_ASSERT( SMDMIF_DEF_BLOCK_ITEM_COUNT( sBlockDir ) > 0 );

    if( sBlockDir->mLevel > 1 )
    {
        for( i = 0; i < SMDMIF_DEF_BLOCK_ITEM_COUNT( sBlockDir ); i++ )
        {
            sChildDir = (smdmifBlockDirHeader *)
                            KNL_TO_PHYSICAL_ADDR(
                                SMDMIF_BLOCK_DIR_GET_DIR( sBlockDir, i ) );

            STL_ASSERT( sChildDir->mLevel == sBlockDir->mLevel - 1 );
            STL_ASSERT( sChildDir->mMaxDirCount == sBlockDir->mMaxDirCount );
            STL_ASSERT( sChildDir->mParent == SMDMIF_DEF_BLOCK_ADDR( sBlockDir ) );
            STL_ASSERT( smdmifBlockDirVerify( sChildDir ) == STL_SUCCESS );
        }
    }
    else
    {
        sFixedPart = (smdmifFixedPartHeader *)
                         KNL_TO_PHYSICAL_ADDR(
                             SMDMIF_BLOCK_DIR_GET_DIR( sBlockDir, 0 ) );

        for( i = 1; i < SMDMIF_DEF_BLOCK_ITEM_COUNT( sBlockDir ); i++ )
        {
            STL_ASSERT( sFixedPart->mNextFixedPart ==
                        SMDMIF_BLOCK_DIR_GET_DIR( sBlockDir, i ) );

            sFixedPart = (smdmifFixedPartHeader *)
                             KNL_TO_PHYSICAL_ADDR(sFixedPart->mNextFixedPart);
        }
    }

    return STL_SUCCESS;
}

/** @} */
