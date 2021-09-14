/*******************************************************************************
 * smnmihBuild.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smnmihBuild.c 7363 2013-02-22 07:19:16Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <sml.h>
#include <smDef.h>
#include <sms.h>
#include <sme.h>
#include <smp.h>
#include <smn.h>
#include <smg.h>
#include <sma.h>
#include <smo.h>
#include <smd.h>
#include <smxm.h>
#include <smxl.h>
#include <smnDef.h>
#include <smnmih.h>
#include <smdmih.h>

/**
 * @file smnmihBuild.c
 * @brief Storage Manager Layer Memory Instant Hash Table Build Routines
 */

/**
 * @addtogroup smnmih
 * @{
 */

stlStatus smnmihBuildNodes( smnmihIndexHeader * aIndexHeader,
                            smdmifTableHeader * aTableHeader,
                            smlEnv            * aEnv )
{
    void              * sScanFixedPart;
    knlLogicalAddr      sScanRowAddr;
    stlInt32            sScanRowIndex;
    smdmifDefColumn   * sRowCol;
    stlChar           * sColValue;
    stlInt64            sColLen;
    stlUInt32           sHashKey;
    stlUInt64           sBucketIdx;
    stlInt64            i;
    stlChar           * sKeySlot;
    knlLogicalAddr      sKeyLa;
    smdmifFixedRow      sScanRow;
    smnmihDirSlot     * sDirSlot;
    
    
    sScanFixedPart = SMDMIF_TABLE_GET_FIRST_FIXED_PART( aTableHeader );
    sScanRowIndex = 0;

    while( STL_TRUE )
    {
        sScanRowAddr = SMDMIF_FIXED_PART_GET_ROW_ADDR( sScanFixedPart, sScanRowIndex );
        
        while( sScanRowAddr == KNL_LOGICAL_ADDR_NULL )
        {
            if( sScanFixedPart == NULL )
            {
                break;
            }
           
            sScanFixedPart = SMDMIF_FIXED_PART_GET_NEXT_FIXED_PART( sScanFixedPart );
            sScanRowIndex = 0;
            sScanRowAddr = SMDMIF_FIXED_PART_GET_ROW_ADDR( sScanFixedPart, sScanRowIndex );
        }
      
        if( sScanRowAddr == KNL_LOGICAL_ADDR_NULL )
        {
            /**
             * no more rows
             */
            break;
        }

        sScanRow = KNL_TO_PHYSICAL_ADDR( sScanRowAddr );
        sScanRowIndex++;
        sHashKey = 0;
      
        for( i = 0; i < aIndexHeader->mKeyColCount; i++ )
        {
            sRowCol = &(aTableHeader->mCols[aIndexHeader->mColumnInfo[i].mColOrder]);
            
            smdmifTableGetColValueAndLen( sScanRow,
                                          sRowCol,
                                          aIndexHeader->mColCombineMem,
                                          &sColValue,
                                          &sColLen );

            /**
             * NULL을 키로 갖는 레코드는 무시한다.
             */
        
            if( (aIndexHeader->mSkipNull == STL_TRUE) && (sColLen == 0) )
            {
                STL_THROW( RAMP_READ_NEXT_ROW );
            }
            
            sHashKey += gDtlHash32[sRowCol->mDataType]( (stlUInt8*)sColValue,
                                                        sColLen );
        }

        STL_TRY( smnmihAllocSlot( aIndexHeader,
                                  &sKeySlot,
                                  &sKeyLa,
                                  aEnv )
                 == STL_SUCCESS );
      
        sBucketIdx = KNL_HASH_BUCKET_IDX( aIndexHeader->mBucketCount,
                                          sHashKey,
                                          aIndexHeader->mModularConstant,
                                          aIndexHeader->mModularShiftBit,
                                          aIndexHeader->mAdjustConstant );
        STL_DASSERT( sBucketIdx < aIndexHeader->mBucketCount );
        
        sDirSlot = &(aIndexHeader->mDirectory[sBucketIdx]);
        
        SMNMIH_SET_HASH_KEY( sKeySlot, sHashKey );
        SMNMIH_SET_ROWID( sKeySlot, sScanRowAddr );
        SMNMIH_SET_OVERFLOW_KEY( sKeySlot, sDirSlot->mKeyLa );
        SMNMIH_SET_FETCH_BIT( sKeySlot, STL_FALSE );

        if( sDirSlot->mOverflowCount < STL_INT32_MAX )
        {
            sDirSlot->mOverflowCount++;
            sDirSlot->mLastHashKey = sHashKey;
        }
        
        sDirSlot->mKeyLa = sKeyLa;

        STL_RAMP( RAMP_READ_NEXT_ROW );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmihAllocSlot( smnmihIndexHeader  * aIndexHeader,
                           stlChar           ** aKey,
                           knlLogicalAddr     * aKeyLa,
                           smlEnv             * aEnv )
{
    smnmihNodeHeader * sNodeHeader;
    stlChar          * sNode;

    sNode = KNL_TO_PHYSICAL_ADDR( aIndexHeader->mDataNodeLa );

    sNodeHeader = (smnmihNodeHeader*)sNode;

    if( (sNodeHeader->mUsedSize + SMNMIH_KEY_HEADER_SIZE) > aIndexHeader->mDataNodeSize )
    {
        STL_TRY( smnmihAllocDataNode( aIndexHeader,
                                      &sNode,
                                      aEnv )
                 == STL_SUCCESS );

        sNodeHeader = (smnmihNodeHeader*)sNode;
        sNodeHeader->mUsedSize = STL_SIZEOF(smnmihNodeHeader);
    }

    *aKeyLa = aIndexHeader->mDataNodeLa + sNodeHeader->mUsedSize; 
    *aKey = sNode + sNodeHeader->mUsedSize;
    
    sNodeHeader->mUsedSize += SMNMIH_KEY_HEADER_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmihAllocDataNode( smnmihIndexHeader  * aIndexHeader,
                               stlChar           ** aDataNode,
                               smlEnv             * aEnv )
{
    smdmifDefBlockHeader * sFreeBlock;

    if( (aIndexHeader->mFreeBlockLa == KNL_LOGICAL_ADDR_NULL) ||
        ((aIndexHeader->mUsedBlockSize + aIndexHeader->mDataNodeSize) >
         aIndexHeader->mBlockSize) )
    {
        STL_TRY( smdmifTableAllocBlock( aIndexHeader->mBaseHeader,
                                        SMDMIF_DEF_BLOCK_INDEX_NODE,
                                        (void **)&sFreeBlock,
                                        aEnv ) == STL_SUCCESS );
        
        aIndexHeader->mFreeBlockLa = ( SMDMIF_DEF_BLOCK_ADDR( sFreeBlock ) +
                                       STL_SIZEOF( smdmifDefBlockHeader ) );
        aIndexHeader->mBlockSize = SMNMIH_USABLE_BLOCK_SIZE( aIndexHeader );
        aIndexHeader->mUsedBlockSize = 0;
    }

    *aDataNode = KNL_TO_PHYSICAL_ADDR( (aIndexHeader->mFreeBlockLa +
                                        aIndexHeader->mUsedBlockSize) );

    aIndexHeader->mDataNodeLa = ( aIndexHeader->mFreeBlockLa +
                                  aIndexHeader->mUsedBlockSize );
    aIndexHeader->mUsedBlockSize += aIndexHeader->mDataNodeSize;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


void smnmihCalcBucketCount( smnmihIndexHeader * aIndexHeader,
                            stlInt64            aRowCount )
{
    stlUInt64 i;

    /**
     * Record 개수보다 큰 소수값을 선택함.
     * 최대값보다 Record 개수가 크다면 최대값을 선택함.
     */
    for ( i = 0; gKnlHashModulus[i].mPrimeNumber != 0; i++ )
    {
        aIndexHeader->mBucketCount = gKnlHashModulus[i].mPrimeNumber;
        aIndexHeader->mModularConstant = gKnlHashModulus[i].mConstant;
        aIndexHeader->mModularShiftBit = gKnlHashModulus[i].mShiftBit;
        aIndexHeader->mAdjustConstant  = gKnlHashModulus[i].mAdjustConstant;
        
        if ( gKnlHashModulus[i].mPrimeNumber > aRowCount )
        {
            break;
        }
    }
}


/** @} */
