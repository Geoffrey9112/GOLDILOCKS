/*******************************************************************************
 * zlciCol.c
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

#include <cml.h>
#include <goldilocks.h>

#include <zlDef.h>
#include <zld.h>

/**
 * @file zlciCol.c
 * @brief ODBC API Internal Column Routines.
 */

/**
 * @addtogroup zlci
 * @{
 */

stlStatus zlciBuildTargetType( zlsStmt       * aStmt,
                               stlInt32        aTargetCount,
                               cmlTargetType * aTargetType,
                               stlErrorStack * aErrorStack )
{
    zldDesc           * sIrd;

    stlAllocator      * sAllocator;
    
    dtlValueBlockList * sBlockList  = NULL;
    dtlValueBlock     * sValueBlock = NULL;
    dtlDataValue      * sDataValue  = NULL;
    void              * sDataBuffer = NULL;

    stlInt64            sBufferSize = 0;
    stlInt64            sAlignSize = 0;
    stlInt32            sSize = 0;

    stlInt32            sIdx;
    stlInt32            i;
    
    sIrd = &aStmt->mIrd;
    
    STL_TRY( zldDescReallocRec( sIrd,
                                aTargetCount,
                                aErrorStack ) == STL_SUCCESS );

    if( aTargetCount > 0 )
    {
        sAllocator = &aStmt->mResult.mAllocator;
        
        STL_TRY( stlClearRegionMem( sAllocator,
                                    aErrorStack ) == STL_SUCCESS );
        
        sSize = STL_SIZEOF(dtlValueBlockList) * aTargetCount;

        STL_TRY( stlAllocRegionMem( sAllocator,
                                    sSize,
                                    (void **)&sBlockList,
                                    aErrorStack )
                 == STL_SUCCESS );
        stlMemset( sBlockList, 0x00, sSize );

        for( sIdx = 0; sIdx < aTargetCount - 1; sIdx++ )
        {
            sBlockList[sIdx].mNext = &sBlockList[sIdx + 1];
        }
        
        sBlockList[sIdx].mNext = NULL;

        for( sIdx = 0; sIdx < aTargetCount; sIdx++ )
        {
            /*
             * PreFetchSize 만큼 Data Value 정보 설정 
             */
            sSize = STL_SIZEOF(dtlDataValue) * aStmt->mPreFetchSize;
    
            STL_TRY( stlAllocRegionMem( sAllocator,
                                        sSize,
                                        (void **)&sDataValue,
                                        aErrorStack ) == STL_SUCCESS );
            stlMemset( sDataValue, 0x00, sSize );

            for ( i = 0; i < aStmt->mPreFetchSize; i++ )
            {
                sDataValue[i].mType   = aTargetType[sIdx].mType;
                sDataValue[i].mLength = 0;
                sDataValue[i].mBufferSize = 0;
                sDataValue[i].mValue  = NULL;
            }

            /*
             * Data Buffer 공간 할당
             */

            STL_TRY( dtlGetDataValueBufferSize( aTargetType[sIdx].mType,
                                                aTargetType[sIdx].mArgNum1,
                                                aTargetType[sIdx].mStringLengthUnit,
                                                &sBufferSize,
                                                ZLS_STMT_NLS_DT_VECTOR( aStmt ),
                                                (void*)aStmt,
                                                aErrorStack )
                     == STL_SUCCESS );

            sAlignSize = STL_ALIGN_DEFAULT( sBufferSize );

            if( (aTargetType[sIdx].mType == DTL_TYPE_LONGVARCHAR) ||
                (aTargetType[sIdx].mType == DTL_TYPE_LONGVARBINARY) )
            {
                /**
                 * Long varying type은 동적 할당을 요구하기 때문에
                 * dynamic allocator를 사용한다.
                 */
                
                for ( i = 0; i < aStmt->mPreFetchSize; i++ )
                {
                    STL_TRY( stlAllocDynamicMem( &aStmt->mLongVaryingMem,
                                                 sAlignSize,
                                                 &sDataBuffer,
                                                 aErrorStack )
                             == STL_SUCCESS );
                    
                    sDataValue[i].mBufferSize = sAlignSize;
                    sDataValue[i].mValue = sDataBuffer;
                }
            }
            else
            {
                STL_TRY( stlAllocRegionMem( sAllocator,
                                            sAlignSize * aStmt->mPreFetchSize,
                                            &sDataBuffer,
                                            aErrorStack )
                         == STL_SUCCESS );

                for ( i = 0; i < aStmt->mPreFetchSize; i++ )
                {
                    sDataValue[i].mBufferSize = sAlignSize;
                    sDataValue[i].mValue = sDataBuffer;
                    sDataBuffer = (stlChar*)sDataBuffer + sAlignSize;
                }
            }
    
            /*
             * Value Block 정보 설정
             */

            sSize = STL_SIZEOF(dtlValueBlock);
    
            STL_TRY( stlAllocRegionMem( sAllocator,
                                        sSize,
                                        (void **)&sValueBlock,
                                        aErrorStack )
                     == STL_SUCCESS );
            stlMemset( sValueBlock, 0x00, sSize );

            sValueBlock->mIsSepBuff         = STL_TRUE;
            sValueBlock->mAllocLayer        = STL_LAYER_GLIESE_LIBRARY;
            sValueBlock->mAllocBlockCnt     = aStmt->mPreFetchSize;
            sValueBlock->mUsedBlockCnt      = 0;
            sValueBlock->mSkipBlockCnt      = 0;
            sValueBlock->mArgNum1           = aTargetType[sIdx].mArgNum1;
            sValueBlock->mArgNum2           = aTargetType[sIdx].mArgNum2;
            sValueBlock->mStringLengthUnit  = aTargetType[sIdx].mStringLengthUnit;
            sValueBlock->mIntervalIndicator = aTargetType[sIdx].mIntervalIndicator;
            sValueBlock->mAllocBufferSize   = sBufferSize;
            sValueBlock->mDataValue         = sDataValue;

            /*
             * Block List 정보 설정
             */
            sBlockList[sIdx].mTableID     = 0;
            sBlockList[sIdx].mColumnOrder = 0;
            sBlockList[sIdx].mValueBlock  = sValueBlock;
        }

        aStmt->mResult.mTargetBuffer = sBlockList;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
