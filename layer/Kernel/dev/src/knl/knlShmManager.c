/*******************************************************************************
 * knlShmManager.c
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

/**
 * @file knlShmManager.c
 * @brief Kernel Layer Shared Memory Segment Management Table Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knlRing.h>
#include <knlShmManager.h>
#include <knlEvent.h>
#include <knDef.h>
#include <knsSegment.h>
#include <knlDynamicMem.h>
#include <knmDynamic.h>

extern knsEntryPoint * gKnEntryPoint;

/**
 * @brief 공유 메모리에서 각 layer의 entry point를 반환한다.
 * @param[in] aLayer entry point를 획득할 레이어
 * <PRE>
 *     STL_LAYER_STANDARD : Standard Layer
 *     STL_LAYER_KERNEL : Kernel Layer
 *     STL_LAYER_DATATYPE : DataType Layer
 *     STL_LAYER_COMMUNICATION : Communication Layer
 *     STL_LAYER_STORAGE_MANAGER : Storage Manager Layer
 *     STL_LAYER_QUERY_PROCESSOR : Query Processor Layer
 *     STL_LAYER_SESSION_MANAGER : Session Manager Layer
 *     STL_LAYER_INSTANCE_MANAGER : Instance Manager Layer
 *     STL_LAYER_GLIESE_LIBRARY : Glise Library
 *     STL_LAYER_GLIESE_TOOL : Gliese Tool
 *     STL_LAYER_GOLDILOCKS_LIBRARY : Goldilocks Library
 *     STL_LAYER_GOLDILOCKS_TOOL : Goldilocks Tool
 * </PRE>
 * @param[out] aAddr entry point의 주소
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlGetEntryPoint( stlLayerClass aLayer,
                            void       ** aAddr,
                            knlEnv      * aEnv )
{
    void * sBaseAddr;

    /* static shared memory는 할당 할 수 없다. */
    STL_PARAM_VALIDATE( ((aLayer > STL_LAYER_NONE) &&
                         (aLayer < STL_LAYER_MAX)), KNL_ERROR_STACK( aEnv ) );

    sBaseAddr = gShmAddrTable.mStaticArea.mAddr;
    STL_ASSERT( sBaseAddr  != NULL);

    *aAddr = sBaseAddr + (STL_SIZEOF(void*) * aLayer);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Address에서 Layer entry의 주소를 반환한다.
 * @param[in] aBaseAddr  Share memory address
 * @param[in] aLayer  Layer
 * @return Layer entry의 주소
 */ 
void *  knlGetLayerEntryAddr( void          * aBaseAddr,
                              stlLayerClass   aLayer )
{
    return aBaseAddr + ( STL_SIZEOF(void*) * aLayer );
}

/**
 * @brief database area를 생성한다.
 * @param[in]     aName      공유 메모리 이름
 * @param[out]    aIdx       생성된 공유 메모리 세그먼트의 ID
 * @param[in]     aReqSize   생성할 공유메모리 세그먼트 크기
 * @param[in,out] aEnv       커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * [KNL_ERRCODE_EXCEEDED_SHM_SEGMENT_COUNT]
 *     생성할 수 있는 공유 메모리 세그먼트의 최대값을 넘었다.
 * @endcode
 * @remark 생성되는 공유 메모리 세그먼트 크기는 KNS_SHM_GRANULE 크기에 align 되어 생성된다.
 */
stlStatus knlCreateDatabaseArea( stlChar   * aName,
                                 stlUInt16 * aIdx,
                                 stlSize     aReqSize,
                                 knlEnv    * aEnv )
{
    STL_TRY( knsCreateSegment( KNS_SHM_SEGMENT_TYPE_DATABASE,
                               aName,
                               aIdx,
                               aReqSize,
                               aEnv ) == STL_SUCCESS );

    /**
     * @todo 공유 메모리 세그먼트들간 링크 연결이 필요?
     */
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief database area를 삭제한다.
 * @param[in] aIdx 삭제할 공유 메모리 세그먼트의 ID
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * @endcode
 */
stlStatus knlDestroyDatabaseArea(stlUInt16   aIdx,
                                 knlEnv    * aEnv )
{
    STL_PARAM_VALIDATE( aIdx > 0,
                        KNL_ERROR_STACK( aEnv ));

    STL_TRY( knsDestroySegment( aIdx,
                                aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief static area의 상태를 조사한다.
 */
stlBool knlValidateStaticArea()
{
    return knsValidateStaticArea();
}

/**
 * @brief static area의 fixed 영역에서 메모리를 할당 받는다.
 * @param[in] aSize 메모리 요청 크기
 * @param[out] aAddr 할당된 메모리 주소
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     attach 된 static shared memory segment가 invalid 하다.
 * @endcode
 * @remark 실제 할당되는 메모리는 8바이트 align된 크기이다.
 *         동시성 제어를 하지 않는다.
 * @todo static area의 최대 크기를 알아야 한다.
 */
stlStatus knlAllocFixedStaticArea( stlSize   aSize,
                                   void   ** aAddr,
                                   knlEnv  * aEnv )
{
    stlUInt64      * sFixedOffset;
    stlSize          sAllocSize;
    void           * sBaseAddr;
    knlDynamicMem  * sVarArea;
    knmChunkHeader * sChunk;
    knmBlockHeader * sBlock;
    
    STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE,
                   RAMP_ERR_INVALID_STATIC_SHM );

    sAllocSize = STL_ALIGN_DEFAULT(aSize);

    sVarArea = gKnEntryPoint->mDynamicMem;
    sChunk = (knmChunkHeader*)STL_RING_GET_FIRST_DATA( &(sVarArea->mChunkRing) );
    sBlock = (knmBlockHeader*)STL_RING_GET_LAST_DATA( &(sChunk->mAllocRing) );

    sBaseAddr = gShmAddrTable.mStaticArea.mAddr;
    STL_ASSERT( sBaseAddr != NULL );
    
    STL_TRY_THROW( (stlUInt64)((stlUInt64)sBaseAddr +
                               gKnEntryPoint->mFixedOffset +
                               sAllocSize) <= (stlUInt64)sBlock,
                   RAMP_ERR_INSUFFICIENT_STATIC_AREA );

    sFixedOffset = &(gKnEntryPoint->mFixedOffset);

    *aAddr = sBaseAddr + (*sFixedOffset);
    *sFixedOffset = (*sFixedOffset) + sAllocSize;

    sChunk->mSize = sChunk->mSize - sAllocSize;
    sVarArea->mInitSize = sVarArea->mInitSize - sAllocSize; 

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STATIC_SHM )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_INSUFFICIENT_STATIC_AREA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INSUFFICIENT_STATIC_AREA,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief static area의 fixed 영역에서 aligned addr을 갖는 메모리를 할당 받는다.
 * @param[in] aSize 메모리 요청 크기
 * @param[out] aAddr 할당된 메모리 주소
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     attach 된 static shared memory segment가 invalid 하다.
 * @endcode
 * @remark 실제 할당되는 메모리는 8바이트 align된 크기이다.
 *         실제 할당되는 메모리의 시작주소는 property에 설정된 크기만큼에 align된 주소이다.
 *         동시성 제어를 하지 않는다.
 */
stlStatus knlCacheAlignedAllocFixedStaticArea( stlSize   aSize,
                                               void   ** aAddr,
                                               knlEnv  * aEnv )
{
    stlSize       sSize;
    void        * sAddr;

    sSize = aSize + gKnEntryPoint->mCacheAlignedSize;

    STL_TRY( knlAllocFixedStaticArea( sSize, &sAddr, aEnv )
             == STL_SUCCESS );

    *aAddr = (void*)STL_ALIGN( (stlUInt64)sAddr, gKnEntryPoint->mCacheAlignedSize );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief 현재까지 사용한 Static Area 정보를 저장한다.
 * @param[out] aMark 이전 Static Area 메모리의 정보
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlMarkStaticArea( knlStaticAreaMark * aMark,
                             knlEnv            * aEnv )
{
    knmChunkHeader * sChunkHeader;

    sChunkHeader = (knmChunkHeader*)STL_RING_GET_FIRST_DATA( &gKnEntryPoint->mDynamicMem->mChunkRing );

    aMark->mFixedOffset = gKnEntryPoint->mFixedOffset;
    aMark->mInitSize    = gKnEntryPoint->mDynamicMem->mInitSize;
    aMark->mChunkSize   = sChunkHeader->mSize;
    
    return STL_SUCCESS;
}

/**
 * @brief Static Area 정보를 이전 상태로 되돌린다.
 * @param[out] aMark 현재까지 사용한 Static Area 메모리의 정보
 * @param[in,out] aEnv 커널 Environment
 * @remark @a knlMarkStaticArea() 이후에 반환된 메모리가 없어야 한다.
 */
stlStatus knlRestoreStaticArea( knlStaticAreaMark * aMark,
                                knlEnv            * aEnv )
{
    knmChunkHeader * sChunkHeader;
    stlUInt64        sDiff;

    sChunkHeader = (knmChunkHeader*)STL_RING_GET_FIRST_DATA( &gKnEntryPoint->mDynamicMem->mChunkRing );

    sDiff = gKnEntryPoint->mFixedOffset - aMark->mFixedOffset;

    gKnEntryPoint->mFixedOffset = aMark->mFixedOffset;
    gKnEntryPoint->mDynamicMem->mInitSize += sDiff;
    sChunkHeader->mSize += sDiff;
    
    return STL_SUCCESS;
}

/**
 * @brief 프로세스에서 유효한 공유 메모리 세그먼트 주소를 반환한다.
 * @param[in] aIdx 공유 메모리 세그먼트의 ID
 * @param[out] aAddr 프로세스에서 유효한 공유 메모리 세그먼트 주소
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 공유 메모리 세그먼트의 ID가 올바르지 않음.
 * @endcode
 */
stlStatus knlGetShmSegmentAddrUnsafe( stlUInt16   aIdx,
                                      void     ** aAddr,
                                      knlEnv    * aEnv )
{
    if( gShmAddrTable.mAddr[aIdx] == NULL )
    {
        /**
         * startup 시에만 사용함으로 동시성 제어가 필요없다.
         */
        STL_TRY( knsAttachSegment( aIdx,
                                   STL_FALSE, /* aDoLatch */
                                   aEnv ) == STL_SUCCESS );
    }

    *aAddr = gShmAddrTable.mAddr[aIdx];

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 pointer가 유효한지 검사한다.
 * @param[in] aMemAddr memory pointer
 * @param[in] aMemSize memory size
 * @return 유효하면 STL_TRUE, 그렇지 않으면 STL_FALSE
 */
stlBool knlIsValidStaticAddress( void    * aMemAddr,
                                 stlSize   aMemSize )
{
    stlBool sResult = STL_FALSE;
    
    if( gShmAddrTable.mStaticArea.mAddr != NULL)
    {
        if( (aMemAddr > gShmAddrTable.mStaticArea.mAddr) &&
            (aMemAddr < (gShmAddrTable.mStaticArea.mAddr + gShmAddrTable.mStaticArea.mSize)) )
        {
            if( (aMemAddr + aMemSize) <
                (gShmAddrTable.mStaticArea.mAddr + gShmAddrTable.mStaticArea.mSize) )
            {
                sResult = STL_TRUE;
            }
        }
    }

    return sResult;
}

/**
 * @brief 논리적 주소를 물리적 주소로 변환한다.
 * @param[in] aLogicalAddr 논리적 주소
 * @param[out] aPhysicalAddr 변환된 물리적 주소
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 주소가 올바르지 않음.
 * @endcode
 */
stlStatus knlGetPhysicalAddr( knlLogicalAddr   aLogicalAddr,
                              void          ** aPhysicalAddr,
                              knlEnv         * aEnv )
{
    stlUInt16            sIdx;
    stlUInt64            sOffset;
    

    if( aLogicalAddr == KNL_LOGICAL_ADDR_NULL )
    {
        *aPhysicalAddr = NULL;
    }
    else
    {
        sIdx = KNL_GET_SHM_INDEX( aLogicalAddr );
        sOffset = KNL_GET_OFFSET( aLogicalAddr );

        if( gShmAddrTable.mAddr[sIdx] == NULL )
        {
            STL_TRY( knsAttachSegment( sIdx,
                                       STL_FALSE,  /* aDoLatch */
                                       aEnv ) == STL_SUCCESS );

            /**
             * Database Segment의 경우에는 유효하지 않은 세그먼트에
             * 접근하는 경우는 없어야 한다.
             */
            STL_DASSERT( gShmAddrTable.mSeq[sIdx] == gShmAddrTable.mShmManager->mSegment[sIdx].mSeq );
        }
    
        *aPhysicalAddr = (void*)(gShmAddrTable.mAddr[sIdx] + sOffset);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 해당 프로세스가 Attach한 segment가 유효한지 검사한다.
 * @param[in] aShmIndex Shared Memory Segment Identifier
 * @param[out] aIsValid 유효성 여부
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlIsValidShmSegment( stlUInt16   aShmIndex,
                                stlBool   * aIsValid,
                                knlEnv    * aEnv )
{
    *aIsValid = STL_TRUE;
    
    if( gShmAddrTable.mAddr[aShmIndex] == NULL )
    {
        STL_TRY( knsAttachSegment( aShmIndex,
                                   STL_FALSE,  /* aDoLatch */
                                   aEnv ) == STL_SUCCESS );
    }

    if( gShmAddrTable.mSeq[aShmIndex] != gShmAddrTable.mShmManager->mSegment[aShmIndex].mSeq )
    {
        *aIsValid = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Shared memory segment를 다시 attach 한다.
 * @param[in] aShmIndex Shared Memory Segment Identifier
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlReattachShmSegment( stlUInt16   aShmIndex,
                                 knlEnv    * aEnv )
{
    STL_TRY( knsReattachSegment( aShmIndex,
                                 aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 논리적 주소를 물리적 주소로 변환한다. attach되어 있지 않으면 NULL을 리턴한다.
 * @param[in] aLogicalAddr 논리적 주소
 * @par Error Codes :
 */
void * knlToPhysicalAddr( knlLogicalAddr aLogicalAddr )
{
    stlUInt16  sIdx;
    stlUInt64  sOffset;

    if( aLogicalAddr == KNL_LOGICAL_ADDR_NULL )
    {
        return NULL;
    }

    sIdx = KNL_GET_SHM_INDEX( aLogicalAddr );
    sOffset = KNL_GET_OFFSET( aLogicalAddr );

    if( gShmAddrTable.mAddr[sIdx] == NULL)
    {
        return NULL;
    }
   
    return (void*)(gShmAddrTable.mAddr[sIdx] + sOffset);
}

/**
 * @brief 논리적 주소를 물리적 주소로 변환한다.
 * @param[in] aLogicalAddr 논리적 주소
 * @note  NULL logical address는 지원하지 않는다.
 * @par Error Codes :
 */
void * knlToPhysicalUnsafeAddr( knlLogicalAddr aLogicalAddr )
{
    STL_DASSERT( aLogicalAddr != KNL_LOGICAL_ADDR_NULL );
    STL_DASSERT( gShmAddrTable.mAddr[KNL_GET_SHM_INDEX( aLogicalAddr )] != NULL );

    return (gShmAddrTable.mAddr[KNL_GET_SHM_INDEX( aLogicalAddr )] + KNL_GET_OFFSET( aLogicalAddr ));
}

/**
 * @brief 물리적 주소를 논리적 주소로 변환한다.
 * @param[in] aPhysicalAddr 물리적 주소
 * @param[out] aLogicalAddr 변환된 논리적 주소
 * @param[in,out] aEnv 커널 Environment
 * @par Error Codes :
 * @code
 * [KNL_ERRCODE_INVALID_STATIC_SHM]
 *     static shared memory segment가 invalid 하다.
 * [KNL_ERRCODE_INVALID_ARGUMENT]
 *     입력받은 논리적 주소가 올바르지 않음.
 * @endcode
 */
stlStatus knlGetLogicalAddr( void           * aPhysicalAddr,
                             knlLogicalAddr * aLogicalAddr,
                             knlEnv         * aEnv )
{
    STL_PARAM_VALIDATE( aLogicalAddr != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    if( aPhysicalAddr == NULL )
    {
        *aLogicalAddr = KNL_LOGICAL_ADDR_NULL;
    }
    else
    {
        STL_TRY( knsGetLogicalAddr( aPhysicalAddr,
                                    aLogicalAddr,
                                    aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
    
}

/**
 * @brief 이름을 사용하여 공유 메모리 세그먼트 Idx를 얻는다.
 * @param[in] aName Segment 이름
 * @param[out] aIdx 세그먼트 ID
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knlGetSegmentIdxByName( stlChar    * aName,
                                  stlUInt16  * aIdx,
                                  knlEnv     * aEnv )
{
    STL_TRY( knsGetSegmentIdxByName( aName,
                                     aIdx,
                                     aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief variable static area에 생성된 메모리 정보를 출력한다.
 * @internal
 */
void knlTestPrintShmManager()
{
    knlShmManager      * sShmManager;
    stlUInt32            sIdx;
    knlShmTableElement * sElement;

    sShmManager =  gShmAddrTable.mShmManager;

    stlPrintf("===================================================================\n");
    stlPrintf("SHM MANAGER : SEGMENT COUNT[%d]\n", sShmManager->mSegmentCount);
    stlPrintf("===================================================================\n");

    for( sIdx = 0; sIdx < KNL_SHM_TABLE_ELEMENT_COUNT; sIdx++ )
    {
        sElement = &(sShmManager->mSegment[sIdx]);

        if( sElement->mKey == KNS_INVALID_SHM_KEY )
        {
            continue;
        }

        stlPrintf("    ===================================================================\n");
        stlPrintf("    SEG #%d : NAME[%s] SIZE[%ld] KEY[%d]\n", sIdx,
                                                                sElement->mName,
                                                                sElement->mSize,
                                                                sElement->mKey);
        stlPrintf("    ===================================================================\n");
    }
}

/**
 * @brief variable static area에 생성된 메모리 정보를 출력한다.
 * @internal
 */
void knlTestPrintAllocVariableStaticArea()
{
    knlTestPrintAllocDynamicMem( gKnEntryPoint->mDynamicMem );
}

/**
 * @brief variable static area에 반환된 메모리 정보를 출력한다.
 * @internal
 */
void knlTestPrintFreeVariableStaticArea()
{
    knlTestPrintFreeDynamicMem( gKnEntryPoint->mDynamicMem );
}


/** @} */
