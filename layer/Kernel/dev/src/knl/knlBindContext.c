/*******************************************************************************
 * knlBindContext.c
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
 * @file knlBindContext.c
 * @brief Kernel Layer bind context routines
 */

#include <stl.h>
#include <dtl.h>
#include <knl.h>

#define KNL_PARAMETER_MAP_DEFAULT_SIZE 16

/**
 * @brief Bind Context를 생성한다.
 * @param[out]    aBindContext    생성된 Bind Context
 * @param[in]     aRegionMem      영역 기반 메모리 할당자
 * @param[in,out] aEnv            커널 Environment
 * @remark Bind Context의 Free는 존재하지 않는다.
 * <BR>    @a aRegionMem의 Free가 Bind Context의 Free를 의미한다.
 */
stlStatus knlCreateBindContext( knlBindContext ** aBindContext,
                                knlRegionMem    * aRegionMem,
                                knlEnv          * aEnv )
{
    STL_PARAM_VALIDATE( aBindContext != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, KNL_ERROR_STACK(aEnv) );

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(knlBindContext),
                                (void**)aBindContext,
                                aEnv )
             == STL_SUCCESS );

    (*aBindContext)->mSize               = 0;
    (*aBindContext)->mInitSize           = 0;
    (*aBindContext)->mExecuteSize        = 0;
    (*aBindContext)->mHasServerParameter = STL_FALSE;
    (*aBindContext)->mMapSize            = KNL_PARAMETER_MAP_DEFAULT_SIZE;

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(knlBindParam*) * KNL_PARAMETER_MAP_DEFAULT_SIZE,
                                (void**)&(*aBindContext)->mMap,
                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Bind Context의 배열 크기를 반환한다.
 * @param[in] aBindContext 대상 Bind Context
 * @return Bind Context의 배열 크기
 * @note 해당 함수는 Bind된 Parameter의 총 개수를 반환하는 함수가 아님.
 */
stlInt32 knlGetBindContextSize( knlBindContext * aBindContext )
{
    return aBindContext->mSize;
}

/**
 * @brief Bound Value Block의 크기를 반환한다.
 * @param[in] aBindContext 대상 Bind Context
 * @return Bound Value Block 의 크기
 */
stlInt32 knlGetExecuteValueBlockSize( knlBindContext * aBindContext )
{
    return aBindContext->mExecuteSize;
}

/**
 * @brief 새로운 Bind Parameter 정보를 저장하거나 기존 정보를 치환한다.
 * @param[in]     aBindContext   Bind Context
 * @param[in]     aRegionMem     영역 기반 메모리 할당자
 * @param[in]     aParamId       Parameter identifier
 * @param[in]     aBindType      Binding Type
 * @param[in]     aINValueBlock  SQL type에 대응되는 Default DB Type 정보
 * @param[in]     aOUTValueBlock SQL type에 대응되는 Default DB Type 정보
 * @param[in,out] aEnv           커널 Environment
 */
stlStatus knlCreateOrReplaceBindParam( knlBindContext    * aBindContext,
                                       knlRegionMem      * aRegionMem,
                                       stlInt32            aParamId,
                                       knlBindType         aBindType,
                                       knlValueBlockList * aINValueBlock,
                                       knlValueBlockList * aOUTValueBlock,
                                       knlEnv            * aEnv )
{
    knlBindParam  * sBindParam;
    knlBindParam ** sMap;
    stlInt32        i;
    
    STL_PARAM_VALIDATE( aBindContext != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParamId > 0, KNL_ERROR_STACK(aEnv) );

    if( aBindContext->mSize < aParamId )
    {
        if( aBindContext->mMapSize < aParamId )
        {
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF(knlBindParam*) * aBindContext->mMapSize * 2,
                                        (void**)&sMap,
                                        aEnv )
                     == STL_SUCCESS );

            stlMemcpy( (void*)sMap,
                       (void*)aBindContext->mMap,
                       STL_SIZEOF(knlBindParam*) * aBindContext->mMapSize );

            aBindContext->mMap      = sMap;
            aBindContext->mMapSize *= 2;
        }

        for( i = aBindContext->mSize; i < aParamId; i++ )
        {
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( knlBindParam ),
                                        (void**)&sBindParam,
                                        aEnv )
                     == STL_SUCCESS );

            sBindParam->mBindType      = KNL_BIND_TYPE_INVALID;
            sBindParam->mINValueBlock  = NULL;
            sBindParam->mOUTValueBlock = NULL;

            aBindContext->mMap[i] = sBindParam;

            aBindContext->mSize++;
        }
    }

    sBindParam = aBindContext->mMap[aParamId - 1];
    
    sBindParam->mBindType      = aBindType;
    sBindParam->mINValueBlock  = aINValueBlock;
    sBindParam->mOUTValueBlock = aOUTValueBlock;
        
    if( aINValueBlock != NULL )
    {
        if( (KNL_GET_BLOCK_DB_TYPE( aINValueBlock ) == DTL_TYPE_LONGVARCHAR)   ||
            (KNL_GET_BLOCK_DB_TYPE( aINValueBlock ) == DTL_TYPE_LONGVARBINARY) )        
        {
            aBindContext->mHasServerParameter = STL_TRUE;
        }
        
        sBindParam->mExecuteValueBlockList = *aINValueBlock;
        sBindParam->mExecuteValueBlockList.mValueBlock = &(sBindParam->mExecuteValueBlock);
        sBindParam->mExecuteValueBlock = *(aINValueBlock->mValueBlock);
    }

    if( aOUTValueBlock != NULL )
    {
        if( (KNL_GET_BLOCK_DB_TYPE( aOUTValueBlock ) == DTL_TYPE_LONGVARCHAR)   ||
            (KNL_GET_BLOCK_DB_TYPE( aOUTValueBlock ) == DTL_TYPE_LONGVARBINARY) )        
        {
            aBindContext->mHasServerParameter = STL_TRUE;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 @a aParamId에 해당하는 Parameter의 knlValueBlockList 를 얻는다.
 * @param[in]     aBindContext Bind Context
 * @param[in]     aParamId     Parameter identifier
 * @param[out]    aINValueBlock  Bind된 Parameter의 knlValueBlockList
 * @param[out]    aOUTValueBlock  Bind된 Parameter의 knlValueBlockList
 * @param[in,out] aErrorStack  Error Stack
 * @remark @a aParamId에 사용자가 Bind 정보를 설정하지 않았다면 aValueBlock 에는
 * NULL이 설정된다.
 */
stlStatus knlGetParamValueBlock( knlBindContext     * aBindContext,
                                 stlInt32             aParamId,
                                 knlValueBlockList ** aINValueBlock,
                                 knlValueBlockList ** aOUTValueBlock,
                                 stlErrorStack      * aErrorStack )
{
    STL_PARAM_VALIDATE( aBindContext != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aParamId > 0, aErrorStack );
    STL_PARAM_VALIDATE( aBindContext->mSize >= aParamId, aErrorStack );

    if( aINValueBlock != NULL )
    {
        *aINValueBlock  = aBindContext->mMap[aParamId - 1]->mINValueBlock;
    }

    if( aOUTValueBlock != NULL )
    {
        *aOUTValueBlock = aBindContext->mMap[aParamId - 1]->mOUTValueBlock;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    
/**
 * @brief 처리되지 않은 마지막 Value Block의 포인터를 반환한다.
 * @param[in]     aBindContext Bind Context
 * @param[in]     aParamId     Parameter identifier
 * @param[out]    aValueBlock  Bind된 Parameter의 knlValueBlockList
 * @param[in,out] aErrorStack  Error Stack
 */
stlStatus knlGetExecuteParamValueBlock( knlBindContext     * aBindContext,
                                        stlInt32             aParamId,
                                        knlValueBlockList ** aValueBlock,
                                        stlErrorStack      * aErrorStack )
{
    STL_PARAM_VALIDATE( aBindContext != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aValueBlock != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aParamId > 0, aErrorStack );
    STL_PARAM_VALIDATE( aBindContext->mSize >= aParamId, aErrorStack );

    *aValueBlock = NULL;
    
    if( aBindContext->mMap[aParamId - 1]->mINValueBlock != NULL )
    {
        *aValueBlock = &(aBindContext->mMap[aParamId - 1]->mExecuteValueBlockList);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    
/**
 * @brief Execute Param Value Block들의 위치를 aMoveSize만큼 이동시킨다.
 * @param[in]     aBindContext Bind Context
 * @param[in]     aMoveSize    이동할 개수
 * @param[in,out] aErrorStack  Error Stack
 */
stlStatus knlInitExecuteParamValueBlocks( knlBindContext * aBindContext,
                                          stlUInt64        aMoveSize,
                                          stlErrorStack  * aErrorStack )
{
    knlBindParam * sBindParam;
    stlInt32       i;
    
    STL_PARAM_VALIDATE( aBindContext != NULL, aErrorStack );

    aBindContext->mInitSize = aMoveSize;
    aBindContext->mExecuteSize = aMoveSize;
    aBindContext->mExecutePos = 0;

    for( i = 0; i < aBindContext->mSize; i++ )
    {
        sBindParam = aBindContext->mMap[i];

        if( sBindParam->mINValueBlock != NULL )
        {
            sBindParam->mExecuteValueBlock = *(sBindParam->mINValueBlock->mValueBlock);

            /**
             * Non-Atomic은 Constant처럼 처리해야 한다.
             */
            if( aMoveSize == 1 )
            {
                sBindParam->mExecuteValueBlock.mIsSepBuff = STL_FALSE;
            }
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    
/**
 * @brief Execute Block Block의 크기를 설정한다.
 * @param[in]     aBindContext Bind Context
 * @param[in]     aExecuteSize 설정할 execute 크기
 * @param[in,out] aErrorStack  Error Stack
 */
stlStatus knlSetExecuteValueBlockSize( knlBindContext * aBindContext,
                                       stlUInt64        aExecuteSize,
                                       stlErrorStack  * aErrorStack )
{
    STL_PARAM_VALIDATE( aBindContext != NULL, aErrorStack );

    aBindContext->mExecuteSize = aExecuteSize;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    
/**
 * @brief Execute Param Value Block들의 위치를 초기화한다.
 * @param[in]     aBindContext Bind Context
 * @param[in,out] aErrorStack  Error Stack
 */
stlStatus knlResetExecuteParamValueBlocks( knlBindContext * aBindContext,
                                           stlErrorStack  * aErrorStack )
{
    return knlInitExecuteParamValueBlocks( aBindContext,
                                           aBindContext->mInitSize,
                                           aErrorStack );
}
    
stlStatus knlMoveExecuteParamValueBlocks( knlBindContext * aBindContext,
                                          stlUInt64        aMoveSize,
                                          stlErrorStack  * aErrorStack )
{
    knlBindParam * sBindParam;
    stlInt32       i;
    
    STL_PARAM_VALIDATE( aBindContext != NULL, aErrorStack );

    aBindContext->mExecutePos += aMoveSize;
    
    for( i = 0; i < aBindContext->mSize; i++ )
    {
        sBindParam = aBindContext->mMap[i];
        
        if( sBindParam->mINValueBlock != NULL )
        {
            sBindParam->mExecuteValueBlock.mDataValue += aMoveSize;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 @a aParamId에 해당하는 Parameter의 Bind Type 정보를 얻는다.
 * @param[in]     aBindContext Bind Context
 * @param[in]     aParamId     Parameter identifier
 * @param[out]    aBindType    Bind된 Parameter의 Bind Type
 * @param[in,out] aErrorStack  Error Stack
 * @remark @a aParamId에 사용자가 Bind 정보를 설정하지 않았다면 aBindType에는
 * KNL_BIND_TYPE_INVALID가 설정된다.
 */
stlStatus knlGetBindParamType( knlBindContext  * aBindContext,
                               stlInt32          aParamId,
                               knlBindType     * aBindType,
                               stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( aBindContext != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aBindType != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aParamId > 0, aErrorStack );
    STL_PARAM_VALIDATE( aBindContext->mSize >= aParamId, aErrorStack );

    *aBindType = aBindContext->mMap[aParamId - 1]->mBindType;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 서버에서 할당된 파라미터 Memory를 해제 한다.
 * @param[in]      aBindContext  Bind Context
 * @param[in,out]  aEnv          Environment Pointer
 */
stlStatus knlFreeServerBindContext( knlBindContext * aBindContext,
                                    knlEnv         * aEnv )
{
    stlInt32            i;
    knlBindParam      * sBindParam;
    knlValueBlockList * sINValueBlock;
    knlValueBlockList * sOUTValueBlock;

    for( i = 0; i < aBindContext->mSize; i++ )
    {
        sBindParam = aBindContext->mMap[i];
        
        sINValueBlock = sBindParam->mINValueBlock;

        if( sINValueBlock != NULL )
        {
            /**
             * ValueBlock을 할당한 Layer가 Session Layer가 아니라면 삭제할수 없다.
             */
        
            if( KNL_GET_BLOCK_ALLOC_LAYER( sINValueBlock ) == STL_LAYER_SESSION )
            {
                STL_TRY( knlFreeParamValues( sINValueBlock, aEnv ) == STL_SUCCESS );
            }
        }

        sOUTValueBlock = sBindParam->mOUTValueBlock;

        if( sOUTValueBlock != NULL )
        {
            STL_TRY( knlFreeParamValues( sOUTValueBlock, aEnv ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
