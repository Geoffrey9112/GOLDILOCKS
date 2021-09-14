/*******************************************************************************
 * smdmifVarPart.c
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
 * @file smdmifVarPart.c
 * @brief Storage Manager Layer Memory Instant Flat Table Variable Part Routines
 */

#include <sml.h>
#include <smDef.h>
#include <smdmifVarPart.h>

/**
 * @addtogroup smdmifVarPart
 * @{
 */

/**
 * @brief 주어진 var row에 대상 컬럼의 내용을 기록한다.
 * @param[in] aTableHandle 해당 인스턴트 테이블의 핸들. callback 호출시 필요
 * @param[in] aVarPart var part header
 * @param[out] aVarColAddr 맨 처음 var column piece의 주소
 * @param[in] aSourceColPos 컬럼 값
 * @param[in] aSourceColLen 컬럼 길이
 * @param[in] aVarColPieceMinLen variable column piece의 최소 크기. 이보다 작은 공간이 남아 있으면 버린다.
 * @param[in] aAllocVarPart var part를 alloc해주는 callback 함수
 * @param[in] aEnv Environment 구조체
 */
stlStatus smdmifVarPartWriteCol( void                            *aTableHandle,
                                 void                            *aVarPart,
                                 knlLogicalAddr                  *aVarColAddr,
                                 stlChar                         *aSourceColPos,
                                 stlInt32                         aSourceColLen,
                                 stlInt32                         aVarColPieceMinLen,
                                 smdmifVarPartCallbackNextVarPart aAllocVarPart,
                                 smlEnv                          *aEnv )
{
    smdmifVarPartHeader  *sVarPart = (smdmifVarPartHeader *)aVarPart;
    smdmifVarPartHeader  *sNewVarPart;
    knlLogicalAddr        sVarColPieceAddr;
    stlInt32              sFreeSize;
    stlInt32              sWritingSize;
    stlBool               sHasNext;

    *aVarColAddr = KNL_LOGICAL_ADDR_NULL;

    if( sVarPart == NULL )
    {
        /* alloc new var part extent */
        STL_TRY( aAllocVarPart( aTableHandle,
                                (void **)&sVarPart,
                                aEnv ) == STL_SUCCESS );
    }

    /* variable part column size check */
    while( STL_TRUE )
    {
        sHasNext = STL_FALSE;
        sFreeSize = SMDMIF_VAR_PART_FREE_SIZE( sVarPart ) - SMDMIF_VAR_COL_HEADER_SIZE;

        if( aSourceColLen > sFreeSize )
        {
            if( aVarColPieceMinLen > sFreeSize )
            {
                /* aVarColPieceMinLen보다 작게 남았으면 그 공간은 버린다. */
                sWritingSize = -1;
            }
            else
            {
                /* 컬럼이 쪼개져 저장되어야 하므로 next col ptr 공간을 뺀다. */
                sWritingSize = sFreeSize - SMDMIF_VAR_COL_NEXT_PTR_SIZE;
                sHasNext = STL_TRUE;
            }
        }
        else
        {
            sWritingSize = aSourceColLen;
        }

        if( sWritingSize >= 0 )
        {
            SMDMIF_VAR_PART_WRITE_COL( sVarPart,
                                       sVarColPieceAddr,
                                       sHasNext,
                                       aSourceColPos,
                                       sWritingSize );

            (SMDMIF_DEF_BLOCK_ITEM_COUNT( sVarPart ))++;

            aSourceColPos += sWritingSize;
            aSourceColLen -= sWritingSize;

            if( *aVarColAddr == KNL_LOGICAL_ADDR_NULL )
            {
                /* 맨 앞 column piece의 주소를 반환한다. */
                *aVarColAddr = sVarColPieceAddr;
            }

            if( aSourceColLen == 0 )
            {
                break;
            }
        }

        /* alloc new var part extent */
        STL_TRY( aAllocVarPart( aTableHandle,
                                (void **)&sNewVarPart,
                                aEnv ) == STL_SUCCESS );

        if( sHasNext == STL_TRUE )
        {
            /* 앞서 기록한 col piece가 있으면, next col ptr을 달아준다. */
            SMDMIF_VAR_PART_WRITE_NEXT_COL_PTR( sVarPart,
                                                SMDMIF_VAR_PART_INSERT_POS_ADDR( sNewVarPart ) );
        }

        sVarPart = sNewVarPart;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 var row에 대상 컬럼의 내용을 update한다. in-place가 될 수도 있고, out-place가 될 수도 있다.
 * @param[in] aTableHandle 해당 인스턴트 테이블의 핸들. callback 호출시 필요
 * @param[in] aVarPart var part header.
 * @param[in,out] aVarColAddr aVarCol의 로지컬 주소(input)이자 outplace-update됐을 때, 새로 insert된 컬럼의 로지컬 주소(output)
 * @param[in] aSourceColPos 컬럼 값
 * @param[in] aSourceColLen 컬럼 길이
 * @param[in] aVarColPieceMinLen variable column piece의 최소 크기. 이보다 작은 공간이 남아 있으면 버린다.
 * @param[in] aAllocVarPart var part를 alloc해주는 callback 함수
 * @param[in] aEnv Environment 구조체
 */
stlStatus smdmifVarPartUpdateCol( void                            *aTableHandle,
                                  void                            *aVarPart,
                                  knlLogicalAddr                  *aVarColAddr,
                                  stlChar                         *aSourceColPos,
                                  stlInt32                         aSourceColLen,
                                  stlInt32                         aVarColPieceMinLen,
                                  smdmifVarPartCallbackNextVarPart aAllocVarPart,
                                  smlEnv                          *aEnv )
{
    smdmifVarCol sVarCol = (smdmifVarCol)KNL_TO_PHYSICAL_ADDR( *aVarColAddr );

    if( SMDMIF_VAR_COL_GET_LEN( sVarCol ) >= aSourceColLen )
    {
        /* in-place update가 가능한 경우이다. 그냥 over-write한다. */
        SMDMIF_VAR_COL_WRITE_VALUE( sVarCol,
                                    STL_FALSE, /* no next */
                                    aSourceColPos,
                                    aSourceColLen );
    }
    else
    {
        STL_TRY( smdmifVarPartWriteCol( aTableHandle,
                                        aVarPart,
                                        aVarColAddr,
                                        aSourceColPos,
                                        aSourceColLen,
                                        aVarColPieceMinLen,
                                        aAllocVarPart,
                                        aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}   

/**
 * @brief 해당 컬럼을 주어진 위치로 읽어들인다.
 * @param[in] aSourceCol source var column
 * @param[in] aTargetPos 복사할 위치
 * @param[out] aReadLen 읽은 컬럼 값의 길이
 */
void smdmifVarPartReadCol( smdmifVarCol  aSourceCol,
                           stlChar      *aTargetPos,
                           stlInt64     *aReadLen )
{
    stlInt32 sColLen = 0;

    *aReadLen = 0;
    while( STL_TRUE )
    {
        sColLen = SMDMIF_VAR_COL_GET_LEN( aSourceCol );

        stlMemcpy( aTargetPos,
                   SMDMIF_VAR_COL_VALUE_POS( aSourceCol ),
                   sColLen );

        aTargetPos += sColLen;
        *aReadLen += sColLen;

        if( !SMDMIF_VAR_COL_HAS_NEXT( aSourceCol ) )
        {
            break;
        }

        aSourceCol = SMDMIF_VAR_COL_NEXT_PIECE( aSourceCol );
    }
}

/**
 * @brief 해당 컬럼을 주어진 위치로 읽어들인다.
 * @param[in] aSourceCol  source var column
 * @param[out] aDataValue 복사할 위치
 * @param[out] aEnv       Environment pointer
 */
stlStatus smdmifVarPartReadDataValue( smdmifVarCol   aSourceCol,
                                      dtlDataValue * aDataValue,
                                      smlEnv       * aEnv )
{
    stlInt32 sColLen = 0;
    stlInt64 sAllocSize;

    aDataValue->mLength = 0;
    
    while( STL_TRUE )
    {
        sColLen = SMDMIF_VAR_COL_GET_LEN( aSourceCol );

        if( (aDataValue->mLength + sColLen) > aDataValue->mBufferSize )
        {
            if( aDataValue->mType == DTL_TYPE_LONGVARCHAR )
            {
                sAllocSize = STL_MIN( (aDataValue->mLength + sColLen) * 2,
                                      DTL_LONGVARCHAR_MAX_PRECISION );
            }
            else
            {
                sAllocSize = STL_MIN( (aDataValue->mLength + sColLen) * 2,
                                      DTL_LONGVARBINARY_MAX_PRECISION );
            }
                            
            STL_TRY( knlReallocAndMoveLongVaryingMem( aDataValue,
                                                      sAllocSize,
                                                      KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }

        stlMemcpy( aDataValue->mValue + aDataValue->mLength,
                   SMDMIF_VAR_COL_VALUE_POS( aSourceCol ),
                   sColLen );

        aDataValue->mLength += sColLen;

        if( !SMDMIF_VAR_COL_HAS_NEXT( aSourceCol ) )
        {
            break;
        }

        aSourceCol = SMDMIF_VAR_COL_NEXT_PIECE( aSourceCol );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmifVarPartVerifyCol( void *aVarCol )
{
    smdmifVarCol sVarCol = (smdmifVarCol)aVarCol;

    if( SMDMIF_VAR_COL_HAS_NEXT( sVarCol ) == STL_TRUE )
    {
        STL_ASSERT( smdmifVarPartVerifyCol( SMDMIF_VAR_COL_NEXT_PIECE( sVarCol ) ) == STL_SUCCESS );
    }

    return STL_SUCCESS;
}

/** @} */
