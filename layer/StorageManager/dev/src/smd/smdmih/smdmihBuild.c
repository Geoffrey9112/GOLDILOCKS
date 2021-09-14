/*******************************************************************************
 * smdmihBuild.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmihBuild.c 7363 2013-02-22 07:19:16Z mkkim $
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
#include <smdmih.h>

/**
 * @file smdmihBuild.c
 * @brief Storage Manager Layer Memory Instant Hash Table Build Routines
 */

extern knlHashModulus gSmdmihHashModulus[];

/**
 * @addtogroup smdmih
 * @{
 */

stlStatus smdmihExtending( smdmihTableHeader  * aTableHeader,
                           smlEnv             * aEnv )
{
    knlLogicalAddr   sKeyLa;
    knlLogicalAddr   sNextKeyLa;
    stlChar        * sKey;
    stlUInt32        sHashKey;
    smdmihDirSlot  * sDirectory;
    smdmihDirSlot  * sOldDirectory;
    stlInt32         i;
    stlInt32         sBucketCount;
    stlInt32         sStartBucketIdx;
    stlInt32         sEndBucketIdx;
    stlInt32         sBucketIdx;
    smdmihDirSlot  * sDirSlot;

    STL_TRY_THROW( gSmdmihHashModulus[aTableHeader->mBucketModulusIdx + 1].mPrimeNumber != 0, RAMP_FINISH );
    
    aTableHeader->mBucketModulusIdx += 1;
    sBucketCount = gSmdmihHashModulus[aTableHeader->mBucketModulusIdx].mPrimeNumber;
    
    STL_TRY( smgAllocSessShmMem( sBucketCount * STL_SIZEOF( smdmihDirSlot ),
                                 (void**)&sDirectory,
                                 aEnv ) == STL_SUCCESS );
    
    for( i = 0; i < sBucketCount; i++ )
    {
        sDirectory[i].mCollisionCount = 0;
        sDirectory[i].mKeyLa = KNL_LOGICAL_ADDR_NULL;
    }
    
    sOldDirectory   = aTableHeader->mDirectory;
    sStartBucketIdx = aTableHeader->mStartBucketIdx;
    sEndBucketIdx   = aTableHeader->mEndBucketIdx;

    aTableHeader->mDirectory = (smdmihDirSlot*)sDirectory;
    aTableHeader->mStartBucketIdx = STL_INT32_MAX;
    aTableHeader->mEndBucketIdx = -1;
    aTableHeader->mBucketCount = sBucketCount;
    aTableHeader->mModularConstant = gSmdmihHashModulus[aTableHeader->mBucketModulusIdx].mConstant;
    aTableHeader->mModularShiftBit = gSmdmihHashModulus[aTableHeader->mBucketModulusIdx].mShiftBit;
    aTableHeader->mAdjustConstant  = gSmdmihHashModulus[aTableHeader->mBucketModulusIdx].mAdjustConstant;

    for( i = sStartBucketIdx; i <= sEndBucketIdx; i++ )
    {
        sKeyLa = sOldDirectory[i].mKeyLa;
        
        while( sKeyLa != KNL_LOGICAL_ADDR_NULL )
        {
            sKey = KNL_TO_PHYSICAL_ADDR( sKeyLa );

            STL_DASSERT( sOldDirectory[i].mCollisionCount > 0 );
            
            if( sOldDirectory[i].mCollisionCount == 1 )
            {
                sHashKey = sOldDirectory[i].mHashKey;

                SMDMIH_DIR_SLOT( aTableHeader, sHashKey, sDirSlot, sBucketIdx );
                SMDMIH_ADJUST_SCAN_HINT( aTableHeader, sBucketIdx );
                SMDMIH_SET_COLLIDE_HASH_KEY( sDirSlot, sHashKey );
                
                if( sDirSlot->mKeyLa != KNL_LOGICAL_ADDR_NULL )
                {
                    sNextKeyLa = sDirSlot->mKeyLa;

                    while( sNextKeyLa != KNL_LOGICAL_ADDR_NULL )
                    {
                        sKey = KNL_TO_PHYSICAL_ADDR( sNextKeyLa );
                        SMDMIH_GET_OVERFLOW_KEY( sKey, sNextKeyLa );
                    }

                    SMDMIH_SET_OVERFLOW_KEY( sKey, sKeyLa );
                }
                else
                {
                    sDirSlot->mKeyLa = sKeyLa;
                }

                break;
            }
            else
            {
                SMDMIH_GET_HASH_KEY( sKey, sHashKey );
                SMDMIH_GET_OVERFLOW_KEY( sKey, sNextKeyLa );
                
                SMDMIH_DIR_SLOT( aTableHeader, sHashKey, sDirSlot, sBucketIdx );
                SMDMIH_ADJUST_SCAN_HINT( aTableHeader, sBucketIdx );
                SMDMIH_SET_OVERFLOW_KEY( sKey, sDirSlot->mKeyLa );
                SMDMIH_SET_COLLIDE_HASH_KEY( sDirSlot, sHashKey );
                
                sDirSlot->mKeyLa = sKeyLa;
            }

            sKeyLa = sNextKeyLa;
        }
    }

    STL_TRY( smgFreeSessShmMem( (void*)sOldDirectory, aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihInsertKey( smdmihTableHeader  * aTableHeader,
                           smdmihDirSlot      * aDirSlot,
                           stlInt32             aBucketIdx,
                           stlUInt32            aHashKey,
                           knlValueBlockList  * aInsertCols,
                           knlValueBlockList  * aUniqueViolation,
                           stlInt32             aBlockIdx,
                           smlMergeRecordInfo * aMergeRecordInfo,
                           knlLogicalAddr     * aRowLa,
                           smlEnv             * aEnv )
{
    stlChar             * sNode;
    knlLogicalAddr        sNodeLa;
    knlLogicalAddr        sRowLa;
    stlChar             * sRow;
    stlChar             * sDupRow;
    knlValueBlockList   * sInsertCol;
    stlChar             * sColPos;
    stlInt32              sColLen;
    stlInt32              sColumnIdx = 0;
    knlLogicalAddr        sVarColLa;
    smdmifDefColumn     * sColumn;
    dtlDataValue        * sDataValue = NULL;
    stlBool               sUniqueViolation;
    stlInt32              i;

    if( aTableHeader->mUniqueness == STL_TRUE )
    {
        STL_TRY( smdmihCheckUniqueness( aTableHeader,
                                        aDirSlot,
                                        aHashKey,
                                        aInsertCols,
                                        aBlockIdx,
                                        &sUniqueViolation,
                                        &sDupRow,
                                        aEnv )
                 == STL_SUCCESS );

        if( sUniqueViolation == STL_TRUE )
        {
            if( aMergeRecordInfo == NULL )
            {
                if( aUniqueViolation != NULL )
                {
                    sDataValue = KNL_GET_BLOCK_DATA_VALUE( aUniqueViolation, aBlockIdx );
                    DTL_BOOLEAN( sDataValue ) = sUniqueViolation;
                    DTL_SET_BOOLEAN_LENGTH( sDataValue );
                }
                
                STL_TRY_THROW( sUniqueViolation == STL_FALSE, RAMP_FINISH );
            }
            else
            {
                STL_TRY( smdmihMergeRecordInternal( aTableHeader,
                                                    sDupRow,
                                                    aMergeRecordInfo,
                                                    aBlockIdx,
                                                    aBlockIdx + 1,
                                                    aEnv )
                         == STL_SUCCESS );

                STL_THROW( RAMP_FINISH );
            }
        }
        else
        {
            if( aUniqueViolation != NULL )
            {
                sDataValue = KNL_GET_BLOCK_DATA_VALUE( aUniqueViolation, aBlockIdx );
                DTL_BOOLEAN( sDataValue ) = sUniqueViolation;
                DTL_SET_BOOLEAN_LENGTH( sDataValue );
            }
        }
    }

    if( aMergeRecordInfo != NULL )
    {
        STL_TRY( smoAssignAggrFunc( aMergeRecordInfo->mAggrFuncInfoList,
                                    aBlockIdx,
                                    aEnv )
                 == STL_SUCCESS );
    }

    if( aTableHeader->mSkipNull == STL_TRUE )
    {
        sInsertCol = aInsertCols;
    
        for( i = 0; i < aTableHeader->mKeyColCount; i++ )
        {
            sColLen = KNL_GET_BLOCK_DATA_LENGTH( sInsertCol, aBlockIdx );
            STL_TRY_THROW( sColLen != 0, RAMP_FINISH );
            
            sInsertCol = sInsertCol->mNext;
        }
    }
    
    STL_TRY( smdmihAllocSlot( aTableHeader,
                              &sRow,
                              aTableHeader->mRowLen,
                              aEnv )
             == STL_SUCCESS );
    
    sInsertCol = aInsertCols;
    sColumnIdx = 0;
    
    while( sInsertCol != NULL )
    {
        sColLen = KNL_GET_BLOCK_DATA_LENGTH( sInsertCol, aBlockIdx );
        sColPos = KNL_GET_BLOCK_DATA_PTR( sInsertCol, aBlockIdx );

        sColumn = &aTableHeader->mColumns[sColumnIdx];
        
        if( sColumn->mIsFixedPart == STL_TRUE )
        {
            smdmihFixedRowWriteCol( sRow + sColumn->mOffset,
                                    aTableHeader->mColumnInfo[sColumnIdx].mColLenSize,
                                    sColPos,
                                    sColLen );
        }
        else
        {
            if( sColLen == 0 )
            {
                sVarColLa = KNL_LOGICAL_ADDR_NULL;
            }
            else
            {
                STL_TRY( smdmifVarPartWriteCol( &aTableHeader->mBaseHeader,
                                                SMDMIF_TABLE_GET_CUR_VAR_PART( &aTableHeader->mBaseHeader ),
                                                &sVarColLa,
                                                sColPos,
                                                sColLen,
                                                aTableHeader->mBaseHeader.mVarColPieceMinLen,
                                                smdmifTablePrepareNextVarBlock,
                                                aEnv ) == STL_SUCCESS );
            }

            STL_WRITE_INT64( sRow + sColumn->mOffset, &sVarColLa );
        }

        sInsertCol = sInsertCol->mNext;
        sColumnIdx++;
    }

    sNodeLa = aTableHeader->mDataNodeLa;
    sNode = KNL_TO_PHYSICAL_ADDR( aTableHeader->mDataNodeLa );
    
    SMDMIH_SET_OVERFLOW_KEY( sRow, aDirSlot->mKeyLa );
    SMDMIH_SET_HASH_KEY( sRow, aHashKey );
    SMDMIH_SET_FETCH_BIT( sRow, STL_FALSE );

    sRowLa = sNodeLa + ( sRow - sNode );
    aDirSlot->mKeyLa = sRowLa;

    SMDMIH_ADJUST_SCAN_HINT( aTableHeader, aBucketIdx );
    
    if( aRowLa != NULL )
    {
        stlMemcpy( aRowLa, &sRowLa, STL_SIZEOF(knlLogicalAddr) );
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihAllocSlot( smdmihTableHeader  * aTableHeader,
                           stlChar           ** aKey,
                           stlInt32             aKeyLen,
                           smlEnv             * aEnv )
{
    smdmihNodeHeader * sNodeHeader;
    stlChar          * sNode;
    stlChar          * sKey;

    if( aTableHeader->mFreeDataBlockLa == KNL_LOGICAL_ADDR_NULL )
    {
        STL_TRY( smdmihAllocDataNode( aTableHeader,
                                      &sNode,
                                      aEnv )
                 == STL_SUCCESS );

        sNodeHeader = (smdmihNodeHeader*)sNode;
        smdmihInitDataNode( sNodeHeader );
    }
    else
    {
        sNode = KNL_TO_PHYSICAL_ADDR( aTableHeader->mDataNodeLa );
        sNodeHeader = (smdmihNodeHeader*)sNode;

        if( (sNodeHeader->mUsedSize + aKeyLen) > aTableHeader->mDataNodeSize )
        {
            STL_TRY( smdmihAllocDataNode( aTableHeader,
                                          &sNode,
                                          aEnv )
                     == STL_SUCCESS );

            sNodeHeader = (smdmihNodeHeader*)sNode;
            smdmihInitDataNode( sNodeHeader );
        }
    }

    sKey = sNode + sNodeHeader->mUsedSize;
    sNodeHeader->mUsedSize += aKeyLen;
    aTableHeader->mRowCount++;

    *aKey = sKey;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihAllocDataNode( smdmihTableHeader  * aTableHeader,
                               stlChar           ** aDataNode,
                               smlEnv             * aEnv )
{
    smdmifDefBlockHeader * sFreeBlock;

    if( (aTableHeader->mFreeDataBlockLa == KNL_LOGICAL_ADDR_NULL) ||
        ((aTableHeader->mUsedDataBlockSize + aTableHeader->mDataNodeSize) >
         aTableHeader->mBlockSize) )
    {
        STL_TRY( smdmifTableAllocBlock( &aTableHeader->mBaseHeader,
                                        SMDMIF_DEF_BLOCK_INDEX_NODE,
                                        (void **)&sFreeBlock,
                                        aEnv ) == STL_SUCCESS );
        
        aTableHeader->mFreeDataBlockLa = ( SMDMIF_DEF_BLOCK_ADDR( sFreeBlock ) +
                                           STL_SIZEOF( smdmifDefBlockHeader ) );
        aTableHeader->mBlockSize = SMDMIH_USABLE_BLOCK_SIZE( aTableHeader );
        aTableHeader->mUsedDataBlockSize = 0;
    }

    *aDataNode = KNL_TO_PHYSICAL_ADDR( (aTableHeader->mFreeDataBlockLa +
                                        aTableHeader->mUsedDataBlockSize) );

    aTableHeader->mDataNodeLa = ( aTableHeader->mFreeDataBlockLa +
                                  aTableHeader->mUsedDataBlockSize );
    aTableHeader->mUsedDataBlockSize += aTableHeader->mDataNodeSize;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihCreateDir( smdmihTableHeader * aTableHeader,
                           smlEnv            * aEnv )
{
    smdmihDirSlot * sDirectory;
    stlInt32        i;

    STL_TRY( smgAllocSessShmMem( aTableHeader->mBucketCount * STL_SIZEOF( smdmihDirSlot ),
                                 (void**)&sDirectory,
                                 aEnv ) == STL_SUCCESS );

    aTableHeader->mDirectory = (smdmihDirSlot*)sDirectory;
    
    for( i = 0; i < aTableHeader->mBucketCount; i++ )
    {
        sDirectory[i].mCollisionCount = 0;
        sDirectory[i].mKeyLa = KNL_LOGICAL_ADDR_NULL;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void smdmihInitDataNode( smdmihNodeHeader  * aNodeHeader )
{
    aNodeHeader->mUsedSize = STL_SIZEOF(smdmihNodeHeader);
}
                            
stlStatus smdmihValidate( smdmihTableHeader  * aTableHeader,
                          smlEnv             * aEnv )
{
    stlInt32         sRowCount = 0;
    knlLogicalAddr   sKeyLa;
    smdmihDirSlot  * sDirSlot;
    stlInt32         sBucketIdx;
    stlChar        * sRow;
    
    for( sBucketIdx = 0; sBucketIdx < aTableHeader->mBucketCount; sBucketIdx++ )
    {
        sDirSlot = &aTableHeader->mDirectory[sBucketIdx];
        sKeyLa = sDirSlot->mKeyLa;

        while( sKeyLa != KNL_LOGICAL_ADDR_NULL )
        {
            sRowCount++;
            
            sRow = KNL_TO_PHYSICAL_ADDR( sKeyLa );
            SMDMIH_GET_OVERFLOW_KEY( sRow, sKeyLa );
        }
    }

    STL_DASSERT( sRowCount == aTableHeader->mRowCount );
    
    return STL_SUCCESS;
}

/** @} */
