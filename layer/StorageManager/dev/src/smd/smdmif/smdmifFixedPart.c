/*******************************************************************************
 * smdmifFixedPart.c
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
 * @file smdmifFixedPart.c
 * @brief Storage Manager Layer Memory Instant Flat Table Variable Part Routines
 */

#include <sml.h>
#include <smDef.h>
#include <smdmifFixedPart.h>

/**
 * @addtogroup smdmifFixedPart
 * @{
 */


/**
 * @brief fixed part header를 초기화한다.
 * @param[in] aFixedPart fixed part header가 담긴 메모리 포인터
 * @param[in] aPrevFixedPart aFixedPart 앞에 올 fixed part. aPrevFixedPart의 next에 fixed part가 달림
 * @param[in] aBlockSize aFixedPart로 사용되는 메모리의 크기
 * @param[in] aFixedRowLen fixed part에 저장될 row의 크기
 * @param[in] aFixedRowAlign fixed part에 저장될 row의 align
 */
void smdmifFixedPartInit( void          *aFixedPart,
                          void          *aPrevFixedPart,
                          stlInt32       aBlockSize,
                          stlInt32       aFixedRowLen,
                          stlInt16       aFixedRowAlign )
{
    smdmifFixedPartHeader *sFixedPart = (smdmifFixedPartHeader *)aFixedPart;

    sFixedPart->mNextFixedPart   = KNL_LOGICAL_ADDR_NULL;
    sFixedPart->mRowPosNumBase   = 0;
    sFixedPart->mRowLen          = aFixedRowLen;

    /* 첫번째 row의 위치: row header 바로 다음 바이트가 aFixedRowAlign에 맞아야 한다. */
    sFixedPart->mFirstRowOffset  = SMDMIF_FIRST_FIXED_ROW_OFFSET( aFixedRowAlign );
    sFixedPart->mInsertRowOffset = sFixedPart->mFirstRowOffset;
    sFixedPart->mMaxRowCount     = ( aBlockSize - sFixedPart->mFirstRowOffset ) / aFixedRowLen;

    if( aPrevFixedPart != NULL )
    {
        sFixedPart->mRowPosNumBase = SMDMIF_FIXED_PART_GET_TOTAL_ROW_COUNT_SO_FAR(aPrevFixedPart);

        SMDMIF_FIXED_PART_SET_NEXT( (smdmifFixedPartHeader *)aPrevFixedPart, SMDMIF_DEF_BLOCK_ADDR( sFixedPart ) );
    }
}

void smdmifFixedPartUnsetDeletion( void *aFixedPart )
{
    smdmifFixedPartHeader *sFixedPart = (smdmifFixedPartHeader *)aFixedPart;
    smdmifFixedRow         sRow;
    stlInt32               i;

    for( i = 0; i < SMDMIF_DEF_BLOCK_ITEM_COUNT(sFixedPart); i++ )
    {
        sRow = SMDMIF_FIXED_PART_GET_ROW( sFixedPart, i );
        SMDMIF_FIXED_ROW_UNDELETE( sRow );
    }
}

/**
 * @brief 주어진 row 위치에 해당 컬럼의 내용을 기록한다.
 * @param[in] aRow 컬럼을 기록할 row
 * @param[in] aColInfo 기록할 컬럼의 정보
 * @param[in] aSourceCol 기록할 컬럼의 내용
 * @param[in] aSourceColLen 기록할 컬럼의 내용의 길이
 */
void smdmifFixedRowWriteCol( smdmifFixedRow   aRow,
                             smdmifDefColumn *aColInfo,
                             stlChar         *aSourceCol,
                             stlInt32         aSourceColLen )
{
    stlChar *sColPos = aRow + aColInfo->mOffset;

    *(smdmifFixedColLength*)sColPos = aSourceColLen;
    sColPos += STL_SIZEOF( smdmifFixedColLength );
    stlMemcpy( sColPos, aSourceCol, aSourceColLen );
}

/**
 * @brief 주어진 row 위치에 해당 컬럼의 내용을 overwrite한다.
 * @param[in] aRow 컬럼을 기록할 row
 * @param[in] aColInfo 기록할 컬럼의 정보
 * @param[in] aSourceCol 기록할 컬럼의 내용
 * @param[in] aSourceColLen 기록할 컬럼의 내용의 길이 
 */
void smdmifFixedRowUpdateCol( smdmifFixedRow   aRow,
                              smdmifDefColumn *aColInfo,
                              stlChar         *aSourceCol,
                              stlInt32         aSourceColLen )
{
    /* update나 insert나 똑같다. */
    smdmifFixedRowWriteCol( aRow,
                            aColInfo,
                            aSourceCol,
                            aSourceColLen );
}

stlStatus smdmifFixedRowVerify( smdmifFixedRow     aRow,
                                smdmifDefColumn   *aColList,
                                stlInt32           aColCount,
                                smdmifVerifyVarCol aVerifyVarCol )
{
    smdmifFixedCol sCol, sCol2, sColFence;
    stlInt32       i, j;

    /**
     * row header 체크
     */
    STL_DASSERT( *(smdmifFixedRowHeader*)aRow <= (SMDMIF_FIXED_ROW_DELETE_YES | SMDMIF_FIXED_ROW_DUP_YES) );

    /**
     * 컬럼 체크
     */
    for( i = 0; i < aColCount; i++ )
    {
        if( aColList[i].mIsFixedPart == STL_TRUE )
        {
            sCol = SMDMIF_FIXED_ROW_GET_COL( aRow, &aColList[i] );
            sColFence = sCol + SMDMIF_FIXED_COL_GET_LEN( sCol );

            for( j = 0; j < aColCount; j++ )
            {
                if( aColList[j].mIsFixedPart == STL_TRUE && j != i )
                {
                    sCol2 = SMDMIF_FIXED_ROW_GET_COL( aRow, &aColList[j] );

                    /* 컬럼이 서로 겹치는 부분이 있는지 검사한다. */
                    /* release mode 컴파일시 warning 때문에 DASSERT가 아닌 ASSERT로 한다. */
                    STL_ASSERT( sCol2 < sCol || sCol2 >= sColFence );
                }
            }
        }
        else
        {
            STL_ASSERT( aVerifyVarCol( SMDMIF_FIXED_ROW_GET_VAR_COL( aRow, &aColList[i] ) ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;
}

stlStatus smdmifFixedPartVerify( void               *aFixedPart,
                                 smdmifDefColumn    *aColList,
                                 stlInt32            aColCount,
                                 smdmifVerifyVarCol  aVerifyVarCol )
{
    smdmifFixedPartHeader *sFixedPart = (smdmifFixedPartHeader *)aFixedPart;
    smdmifFixedRow         sFixedRow;
    stlInt32               sRowCount = 0;
    stlInt32               sRowLen = 0;
    stlInt32               i;

    while( sFixedPart != NULL )
    {
        /**
         * mRowPosNumBase 체크
         */
        STL_DASSERT( sFixedPart->mRowPosNumBase == sRowCount );

        /**
         * mItemCount 체크
         */
        if( sFixedPart->mNextFixedPart == KNL_LOGICAL_ADDR_NULL )
        {
            STL_DASSERT( SMDMIF_DEF_BLOCK_ITEM_COUNT(sFixedPart) > 0 &&
                         SMDMIF_DEF_BLOCK_ITEM_COUNT(sFixedPart) <= sFixedPart->mMaxRowCount );
        }
        else
        {
            /* 마지막 fixed part가 아니면 꽉차야한다. */
            STL_DASSERT( SMDMIF_DEF_BLOCK_ITEM_COUNT(sFixedPart) == sFixedPart->mMaxRowCount );
        }

        /**
         * mMaxRowCount 체크
         */
        STL_DASSERT( sFixedPart->mMaxRowCount > 0 );

        /**
         * mRowLen 체크
         */
        if( sRowLen == 0 )
        {
            sRowLen = sFixedPart->mRowLen;
            STL_DASSERT( sRowLen > 0 );
        }
        else
        {
            /* 모든 fixed part의 mRowLen은 같은 값이어야 한다. */
            STL_DASSERT( sFixedPart->mRowLen == sRowLen );
        }

        /**
         * mFirstRowOffset 체크
         */
        STL_DASSERT( sFixedPart->mFirstRowOffset > 0 &&
                 sFixedPart->mFirstRowOffset < sFixedPart->mMaxRowCount * sFixedPart->mRowLen );

        /**
         * mInsertRowOffset 체크
         */
        STL_DASSERT( sFixedPart->mInsertRowOffset == sFixedPart->mFirstRowOffset + 
                                                     SMDMIF_DEF_BLOCK_ITEM_COUNT(sFixedPart) * sFixedPart->mRowLen );

        /**
         * 모든 fixed row 체크
         */
        for( i = 0; i < SMDMIF_DEF_BLOCK_ITEM_COUNT(sFixedPart); i++ )
        {
            sFixedRow = SMDMIF_FIXED_PART_GET_ROW( sFixedPart, i );

            smdmifFixedRowVerify( sFixedRow,
                                  aColList,
                                  aColCount,
                                  aVerifyVarCol );
        }
        sRowCount += SMDMIF_DEF_BLOCK_ITEM_COUNT( sFixedPart );
        sFixedPart = SMDMIF_FIXED_PART_GET_NEXT_FIXED_PART( sFixedPart );
    }

    return STL_SUCCESS;
}

/** @} */
