/*******************************************************************************
 * smdmihTable.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmihTable.c 6322 2012-11-13 05:16:47Z egon $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smdmihTable.c
 * @brief Storage Manager Layer Memory Instant Hash Table Routines
 */

#include <knl.h>
#include <scl.h>
#include <sml.h>
#include <smDef.h>
#include <smg.h>
#include <sma.h>
#include <smd.h>
#include <smo.h>
#include <sms.h>
#include <smsDef.h>
#include <smsManager.h>
#include <smdDef.h>
#include <smdmifTable.h>
#include <smdmih.h>
#include <smdmifVarPart.h>

/**
 * Gutenberg EBook 프로젝트의 솟수 찾기 프로젝트에서 임의의 솟수를 발췌함
 * 참조 ) http://www.gutenberg.org/cache/epub/65/pg65.html.utf8
 *
 * 16 bit Modular Constant 구하는 방법
 *   1.  Convert 1 / K into binary.
 *   2.  Take all the bits to the right of the binary point, and left shift them until
 *       the bit to the right of the binary point is 1. Record the required number of shifts S.
 *   3.  Take the most significant 18 bits and add 1 and then truncate to 17 bits.
 *       This effectively rounds the result.
 *   4.  Express the 17 bit result as 1hhhh. Denote the hhhh portion as M
 *   5.  Q = ((((uint32_t)A * (uint32_t)M) >> 16) + A) >> 1) >> S
 *   6.  Perform an exhaustive check for all A & Q. If necessary adjust M.
 *
 *  Quotent = ( Prime Number * constant ) >> shift bit
 *  Remain  = HashKey - ( Quotent * Prime Number )
 *  
 *  참조 ) http://embeddedgurus.com/stack-overflow/tag/division/
 */
    
knlHashModulus gSmdmihHashModulus[] =
{
    { 127,      0x81020408,   6, 1 },  /* 최소값 */
    { 563,      0xE8CF58AB,   9, 1 },
    { 1031,     0xFE430AAD,  10, 1 },
    { 3181,     0xA4D19075,  11, 1 },
    { 7673,     0x88A86B91,  12, 0 },
    { 10979,    0xBF03CCFB,  13, 1 },
    { 43591,    0xC07061A1,  15, 1 },
    { 199999,   0xA7C5E341,  17, 1 },
    { 1099997,  0xF4086C05,  20, 1 },
    { 5000011,  0xD6BF75E0,  22, 0 },
    { 0, 0, 0, 0 }   /* 종료조건 */
};

/**
 * @addtogroup smdmihTable
 * @{
 */

/* Memory Instant Hash Table Module */
smdTableModule gSmdmihTableModule =
{
    NULL,
    smdmihCreateIot,
    NULL, /* CreateUndoFunc */
    smdmihDrop,
    NULL, /* CopyFunc */
    NULL, /* AddColumnFunc */
    NULL, /* DropColumnFunc */
    smdmihAlterTableAttr,
    NULL, /* MergeTableFunc */
    NULL, /* CheckExistRowFunc */
    NULL, /* CreateForTruncateFunc */
    NULL, /* DropAgingFunc */
    NULL, /* InitializeFunc */
    NULL, /* FinalizeFunc */
    smdmihInsert,
    smdmihBlockInsert,
    smdmihUpdate,
    smdmihDelete,
    smdmihMerge,
    smdmihRowCount,
    NULL, /* FetchFunc */
    NULL, /* FetchWithRowidFunc */
    NULL, /* LockRowFunc */
    NULL, /* CompareKeyValueFunc */
    NULL, /* ExtractKeyValueFunc */
    NULL, /* ExtractValidKeyValueFunc */
    NULL, /* SortFunc */
    NULL, /* GetSlotBodyFunc */
    smdmihTruncate,
    NULL,  /* SetFlagFunc */
    smdmihCleanup,
    NULL,  /* BuildCacheFunc */
    NULL   /* Fetch4Index */
};

stlStatus smdmihCreateIot( smlStatement      * aStatement,
                           smlTbsId            aTbsId,
                           smlTableAttr      * aTableAttr,
                           smlIndexAttr      * aIndexAttr,
                           knlValueBlockList * aRowColList,
                           stlUInt16           aKeyColCount,
                           stlUInt8          * aKeyColFlags,
                           stlInt64          * aSegmentId,
                           void             ** aRelationHandle,
                           smlEnv            * aEnv )
{
    smdmihTableHeader * sHashTableHeader = NULL;
    stlInt32            sState = 0;
    
    STL_TRY( smgAllocSessShmMem( STL_SIZEOF( smdmihTableHeader ),
                                 (void**)&sHashTableHeader,
                                 aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smdmihInitTableHeader( sHashTableHeader,
                                    aTbsId,
                                    aIndexAttr,
                                    aKeyColCount,
                                    aRowColList,
                                    aKeyColFlags,
                                    aEnv )
             == STL_SUCCESS );

    STL_TRY( smdmifTableInitHeader( &sHashTableHeader->mBaseHeader,
                                    aTbsId,
                                    SML_MEMORY_INSTANT_FLAT_TABLE,
                                    SMDMIF_TABLE_TYPE_FLAT,
                                    STL_FALSE, /* aScrollale */
                                    STL_SIZEOF( smdmifTableHeader ),
                                    aEnv ) == STL_SUCCESS );

    STL_TRY( smdAddHandleToSession( aStatement,
                                    (smeRelationHeader*)sHashTableHeader,
                                    aEnv ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( smdmifTableInitInstantMode( &sHashTableHeader->mBaseHeader,
                                         aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smdmihInitColumns( sHashTableHeader,
                                aRowColList,
                                aEnv ) == STL_SUCCESS );

    STL_TRY_THROW( sHashTableHeader->mRowLen < SMDMIH_USABLE_DATA_BLOCK_SIZE(sHashTableHeader),
                   RAMP_ERR_TOO_LARGE_ROW );
                   
    
    STL_TRY( smdmihCreateDir( sHashTableHeader,
                              aEnv ) == STL_SUCCESS );
    
    *aRelationHandle = (void*)sHashTableHeader;

    return STL_SUCCESS;
                   
    STL_CATCH( RAMP_ERR_TOO_LARGE_ROW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXCEEDED_MAX_INSTANT_ROW_SIZE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      SMDMIH_USABLE_DATA_BLOCK_SIZE(sHashTableHeader) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smdmihDrop( aStatement, (void*)sHashTableHeader, aEnv );
            break;
        case 1:
            (void) smgFreeSessShmMem( sHashTableHeader, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smdmihDrop( smlStatement * aStatement,
                      void         * aRelationHandle,
                      smlEnv       * aEnv )
{
    smdmihTableHeader * sHashTableHeader = (smdmihTableHeader *)aRelationHandle;

    STL_TRY( smdmifTableFiniHeader( &sHashTableHeader->mBaseHeader,
                                    aEnv ) == STL_SUCCESS );

    if( sHashTableHeader->mColCombineMem1 != NULL )
    {
        STL_TRY( knlFreeLongVaryingMem( sHashTableHeader->mColCombineMem1,
                                        KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sHashTableHeader->mColCombineMem1 = NULL;
    }
        
    if( sHashTableHeader->mColCombineMem2 != NULL )
    {
        STL_TRY( knlFreeLongVaryingMem( sHashTableHeader->mColCombineMem2,
                                        KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sHashTableHeader->mColCombineMem2 = NULL;
    }

    if( sHashTableHeader->mColumns != NULL )
    {
        STL_TRY( smgFreeSessShmMem( sHashTableHeader->mColumns,
                                    aEnv ) == STL_SUCCESS );
        sHashTableHeader->mColumns = NULL;
    }
    if( sHashTableHeader->mColumnInfo != NULL )
    {
        STL_TRY( smgFreeSessShmMem( sHashTableHeader->mColumnInfo,
                                    aEnv ) == STL_SUCCESS );
        sHashTableHeader->mColumnInfo = NULL;
    }

    if( sHashTableHeader->mDirectory != NULL )
    {
        STL_TRY( smgFreeSessShmMem( sHashTableHeader->mDirectory,
                                    aEnv ) == STL_SUCCESS );
        sHashTableHeader->mDirectory = NULL;
    }
    
    STL_TRY( smdRemoveHandleFromSession( aStatement,
                                         (smeRelationHeader*)sHashTableHeader,
                                         aEnv ) == STL_SUCCESS );

    STL_TRY( smgFreeSessShmMem( sHashTableHeader,
                                aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihCleanup( void    * aRelationHandle,
                         smlEnv  * aEnv )
{
    smdmihTableHeader * sHashTableHeader = (smdmihTableHeader *)aRelationHandle;

    if( sHashTableHeader->mBaseHeader.mSegHandle != NULL )
    {
        STL_TRY( smsDrop( SML_INVALID_TRANSID,
                          sHashTableHeader->mBaseHeader.mSegHandle,
                          STL_FALSE, /*on start up*/
                          aEnv ) == STL_SUCCESS );

        sHashTableHeader->mBaseHeader.mSegHandle = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihAlterTableAttr( smlStatement  * aStatement,
                                void          * aRelationHandle,
                                smlTableAttr  * aTableAttr,
                                smlIndexAttr  * aIndexAttr,
                                smlEnv        * aEnv )
{
    smdmihTableHeader * sTableHeader;

    sTableHeader = (smdmihTableHeader*)aRelationHandle;
    
    if( aIndexAttr != NULL )
    {
        if( (aIndexAttr->mValidFlags & SML_INDEX_UNIQUENESS_MASK) ==
            SML_INDEX_UNIQUENESS_YES )
        {
            sTableHeader->mUniqueness = aIndexAttr->mUniquenessFlag;
        }
        if( (aIndexAttr->mValidFlags & SML_INDEX_SKIP_NULL_MASK) ==
            SML_INDEX_SKIP_NULL_YES )
        {
            sTableHeader->mSkipNull = aIndexAttr->mSkipNullFlag;
        }
    }
    
    return STL_SUCCESS;
}

stlStatus smdmihTruncate( smlStatement * aStatement,
                          void         * aRelationHandle,
                          smlEnv       * aEnv )
{
    smdmihTableHeader * sTableHeader;
    smdmihDirSlot     * sDirectory;
    stlInt32            i;

    sTableHeader = (smdmihTableHeader*)aRelationHandle;

    sDirectory = sTableHeader->mDirectory;
    
    for( i = 0; i < sTableHeader->mBucketCount; i++ )
    {
        sDirectory[i].mCollisionCount = 0;
        sDirectory[i].mKeyLa = KNL_LOGICAL_ADDR_NULL;
    }
    
    STL_TRY( smdmifTableTruncate( aStatement,
                                  &sTableHeader->mBaseHeader,
                                  aEnv )
             == STL_SUCCESS);

    sTableHeader->mRowCount = 0;
    sTableHeader->mDataNodeLa = KNL_LOGICAL_ADDR_NULL;
    sTableHeader->mFreeDataBlockLa = KNL_LOGICAL_ADDR_NULL;
    sTableHeader->mUsedDataBlockSize = 0;
    sTableHeader->mStartBucketIdx = STL_INT32_MAX;
    sTableHeader->mEndBucketIdx = -1;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihInsert( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aInsertCols,
                        knlValueBlockList * aUniqueViolation,
                        smlRid            * aRowRid,
                        smlEnv            * aEnv )
{
    smdmihTableHeader * sTableHeader = (smdmihTableHeader *)aRelationHandle;

    STL_TRY( smdmihInsertInternal( sTableHeader,
                                   aInsertCols,
                                   aUniqueViolation,
                                   aValueIdx,
                                   NULL, /* aMergeRecordInfo */
                                   aRowRid,
                                   aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihBlockInsert( smlStatement      * aStatement,
                             void              * aRelationHandle,
                             knlValueBlockList * aInsertCols,
                             knlValueBlockList * aUniqueViolation,
                             smlRowBlock       * aRowBlock,
                             smlEnv            * aEnv )
{
    smdmihTableHeader * sTableHeader = (smdmihTableHeader *)aRelationHandle;
    stlInt32            sBlockIdx;

    for( sBlockIdx = KNL_GET_BLOCK_SKIP_CNT( aInsertCols );
         sBlockIdx < KNL_GET_BLOCK_USED_CNT( aInsertCols );
         sBlockIdx++ )
    {
        STL_TRY( smdmihInsertInternal( sTableHeader,
                                       aInsertCols,
                                       aUniqueViolation,
                                       sBlockIdx,
                                       NULL, /* aMergeRecordInfo */
                                       &aRowBlock->mRidBlock[sBlockIdx],
                                       aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihMerge( smlStatement       * aStatement,
                       void               * aRelationHandle,
                       knlValueBlockList  * aInsertCols,
                       smlMergeRecordInfo * aMergeRecordInfo,
                       smlEnv             * aEnv )
{
    smdmihTableHeader * sTableHeader = (smdmihTableHeader *)aRelationHandle;
    stlInt32            sBlockIdx;
    stlInt64            i;
    knlValueBlockList * sInsertCol;
    dtlDataValue      * sDataValue;
    stlUInt32           sHashKey = 0;
    smdmihDirSlot     * sDirSlot;
    knlLogicalAddr      sRowLa;
    stlInt32            sBlockUsedCnt = KNL_GET_BLOCK_USED_CNT( aInsertCols );
    stlInt32            sBlockSkipCnt = KNL_GET_BLOCK_SKIP_CNT( aInsertCols );
    stlUInt32           sPrevHashKey = 0;
    stlInt32            sStartIdx = sBlockSkipCnt;
    stlInt32            sEndIdx = sBlockSkipCnt;
    stlInt32            sBucketIdx;

    for( sBlockIdx = sBlockSkipCnt; sBlockIdx < sBlockUsedCnt; sBlockIdx++ )
    {
        /**
         * extract hash key
         */
    
        sInsertCol = aInsertCols;
        sHashKey = 0;
        
        for( i = 0; i < sTableHeader->mKeyColCount; i++ )
        {
            sDataValue = KNL_GET_BLOCK_DATA_VALUE( sInsertCol, sBlockIdx );
            sHashKey += gDtlHash32[sDataValue->mType]( sDataValue->mValue,
                                                       sDataValue->mLength );
            sInsertCol = sInsertCol->mNext;
        }
        
        if( sBlockIdx == sBlockSkipCnt )
        {
            sPrevHashKey = sHashKey;
            continue;
        }
        
        if( sPrevHashKey == sHashKey )
        {
            sEndIdx = sBlockIdx;
            continue;
        }

        if( (sEndIdx - sStartIdx) >= 1 )
        {
            STL_TRY( smdmihBlockInsertKey( sTableHeader,
                                           sPrevHashKey,
                                           aInsertCols,
                                           sStartIdx,
                                           sEndIdx + 1,
                                           aMergeRecordInfo,
                                           aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            SMDMIH_DIR_SLOT( sTableHeader, sPrevHashKey, sDirSlot, sBucketIdx );

            if( SMDMIH_CHECK_COLLISION( sDirSlot, sPrevHashKey ) == STL_TRUE )
            {
                if( smdmihNeedExtending( sTableHeader, sDirSlot ) == STL_TRUE )
                {
                    STL_TRY( smdmihExtending( sTableHeader,
                                              aEnv )
                             == STL_SUCCESS );
                    
                    SMDMIH_DIR_SLOT( sTableHeader, sPrevHashKey, sDirSlot, sBucketIdx );
                }

                SMDMIH_SET_COLLIDE_HASH_KEY( sDirSlot, sPrevHashKey );
            }
            
            STL_TRY( smdmihInsertKey( sTableHeader,
                                      sDirSlot,
                                      sBucketIdx,
                                      sPrevHashKey,
                                      aInsertCols,
                                      NULL,  /* aUniqueViolation */
                                      sStartIdx,
                                      aMergeRecordInfo,
                                      &sRowLa,
                                      aEnv )
                     == STL_SUCCESS );
        }

        sPrevHashKey = sHashKey;
        sStartIdx = sBlockIdx;
        sEndIdx = sBlockIdx;
    }

    if( (sEndIdx - sStartIdx) >= 1 )
    {
        STL_TRY( smdmihBlockInsertKey( sTableHeader,
                                       sPrevHashKey,
                                       aInsertCols,
                                       sStartIdx,
                                       sEndIdx + 1,
                                       aMergeRecordInfo,
                                       aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        SMDMIH_DIR_SLOT( sTableHeader, sPrevHashKey, sDirSlot, sBucketIdx );
        
        if( SMDMIH_CHECK_COLLISION( sDirSlot, sPrevHashKey ) == STL_TRUE )
        {
            if( smdmihNeedExtending( sTableHeader, sDirSlot ) == STL_TRUE )
            {
                STL_TRY( smdmihExtending( sTableHeader,
                                          aEnv )
                         == STL_SUCCESS );
                    
                SMDMIH_DIR_SLOT( sTableHeader, sPrevHashKey, sDirSlot, sBucketIdx );
            }

            SMDMIH_SET_COLLIDE_HASH_KEY( sDirSlot, sPrevHashKey );
        }
            
        STL_TRY( smdmihInsertKey( sTableHeader,
                                  sDirSlot,
                                  sBucketIdx,
                                  sPrevHashKey,
                                  aInsertCols,
                                  NULL,  /* aUniqueViolation */
                                  sStartIdx,
                                  aMergeRecordInfo,
                                  &sRowLa,
                                  aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlBool smdmihNeedExtending( smdmihTableHeader * aTableHeader,
                             smdmihDirSlot     * aDirSlot )
{
    if( aDirSlot->mCollisionCount > 1 )
    {
        /**
         * Bucket의 1/2배 이상의 레코드가 삽입되어 있다면 extending을 허용한다.
         */
        if( (aTableHeader->mRowCount >> 1) > aTableHeader->mBucketCount )
        {
            return STL_TRUE;
        }
    }
    
    return STL_FALSE;
}

stlStatus smdmihRowCount( void      * aRelationHandle,
                          stlInt64  * aRowCount,
                          smlEnv    * aEnv )
{
    smdmihTableHeader * sTableHeader = (smdmihTableHeader *)aRelationHandle;

    *aRowCount = sTableHeader->mRowCount;

    return STL_SUCCESS;
}

stlStatus smdmihInsertInternal( smdmihTableHeader  * aTableHeader,
                                knlValueBlockList  * aInsertCols,
                                knlValueBlockList  * aUniqueViolation,
                                stlInt32             aBlockIdx,
                                smlMergeRecordInfo * aMergeRecordInfo,
                                smlRid             * aRowId,
                                smlEnv             * aEnv )
{
    stlInt64              i;
    knlValueBlockList   * sInsertCol;
    dtlDataValue        * sDataValue;
    stlUInt32             sHashKey = 0;
    smdmihDirSlot       * sDirSlot;
    knlLogicalAddr        sRowLa;
    stlInt32              sBucketIdx;
    
    /**
     * extract hash key
     */
    
    sInsertCol = aInsertCols;

    for( i = 0; i < aTableHeader->mKeyColCount; i++ )
    {
        sDataValue = KNL_GET_BLOCK_DATA_VALUE( sInsertCol, aBlockIdx );
        sHashKey += gDtlHash32[sDataValue->mType]( sDataValue->mValue,
                                                   sDataValue->mLength );
        sInsertCol = sInsertCol->mNext;
    }

    /**
     * insert into hash index
     */
    
    SMDMIH_DIR_SLOT( aTableHeader, sHashKey, sDirSlot, sBucketIdx );
        
    if( SMDMIH_CHECK_COLLISION( sDirSlot, sHashKey ) == STL_TRUE )
    {
        if( smdmihNeedExtending( aTableHeader, sDirSlot ) == STL_TRUE )
        {
            STL_TRY( smdmihExtending( aTableHeader,
                                      aEnv )
                     == STL_SUCCESS );
                    
            SMDMIH_DIR_SLOT( aTableHeader, sHashKey, sDirSlot, sBucketIdx );
        }

        SMDMIH_SET_COLLIDE_HASH_KEY( sDirSlot, sHashKey );
    }
            
    STL_TRY( smdmihInsertKey( aTableHeader,
                              sDirSlot,
                              sBucketIdx,
                              sHashKey,
                              aInsertCols,
                              aUniqueViolation,
                              aBlockIdx,
                              aMergeRecordInfo,
                              &sRowLa,
                              aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmihBlockInsertKey( smdmihTableHeader  * aTableHeader,
                                stlUInt32            aHashKey,
                                knlValueBlockList  * aInsertCols,
                                stlInt32             aStartIdx,
                                stlInt32             aEndIdx,
                                smlMergeRecordInfo * aMergeRecordInfo,
                                smlEnv             * aEnv )
{
    stlChar             * sNode;
    knlLogicalAddr        sNodeLa;
    knlLogicalAddr        sRowLa;
    stlChar             * sRow;
    stlChar             * sDupRow = NULL;
    knlValueBlockList   * sInsertCol;
    stlChar             * sColPos;
    stlInt32              sColLen;
    stlInt32              sColumnIdx = 0;
    knlLogicalAddr        sVarColLa;
    smdmifDefColumn     * sColumn;
    stlBool               sUniqueViolation;
    stlChar             * sPrevDupRow = NULL;
    stlInt32              sMergeStartBlockIdx = aStartIdx;
    stlInt32              sMergeEndBlockIdx = aStartIdx;
    smdmihDirSlot       * sDirSlot;
    stlInt32              sBlockIdx;
    stlInt32              sBucketIdx;
    stlInt32              i;

    SMDMIH_DIR_SLOT( aTableHeader, aHashKey, sDirSlot, sBucketIdx );

    if( SMDMIH_CHECK_COLLISION( sDirSlot, aHashKey ) == STL_TRUE )
    {
        if( smdmihNeedExtending( aTableHeader, sDirSlot ) == STL_TRUE )
        {
            STL_TRY( smdmihExtending( aTableHeader,
                                      aEnv )
                     == STL_SUCCESS );
                    
            SMDMIH_DIR_SLOT( aTableHeader, aHashKey, sDirSlot, sBucketIdx );
        }

        SMDMIH_SET_COLLIDE_HASH_KEY( sDirSlot, aHashKey );
    }

    for( sBlockIdx = aStartIdx; sBlockIdx < aEndIdx; sBlockIdx++ )
    {
        STL_TRY( smdmihCheckUniqueness( aTableHeader,
                                        sDirSlot,
                                        aHashKey,
                                        aInsertCols,
                                        sBlockIdx,
                                        &sUniqueViolation,
                                        &sDupRow,
                                        aEnv )
                 == STL_SUCCESS );

        if( sUniqueViolation == STL_TRUE )
        {
            if( sPrevDupRow == sDupRow )
            {
                sMergeEndBlockIdx = sBlockIdx;
                continue;
            }
            
            if( sPrevDupRow != NULL )
            {
                STL_TRY( smdmihMergeRecordInternal( aTableHeader,
                                                    sPrevDupRow,
                                                    aMergeRecordInfo,
                                                    sMergeStartBlockIdx,
                                                    sMergeEndBlockIdx + 1,
                                                    aEnv )
                         == STL_SUCCESS );
            }
            
            sPrevDupRow = sDupRow;

            sMergeStartBlockIdx = sBlockIdx;
            sMergeEndBlockIdx = sBlockIdx;
        }
        else
        {
            if( sPrevDupRow != NULL )
            {
                STL_TRY( smdmihMergeRecordInternal( aTableHeader,
                                                    sPrevDupRow,
                                                    aMergeRecordInfo,
                                                    sMergeStartBlockIdx,
                                                    sMergeEndBlockIdx + 1,
                                                    aEnv )
                         == STL_SUCCESS );
            }
            
            sMergeStartBlockIdx = sBlockIdx + 1;
            sMergeEndBlockIdx = sMergeStartBlockIdx;
            sPrevDupRow = NULL;
            
            STL_TRY( smoAssignAggrFunc( aMergeRecordInfo->mAggrFuncInfoList,
                                        sBlockIdx,
                                        aEnv )
                     == STL_SUCCESS );
            
            if( aTableHeader->mSkipNull == STL_TRUE )
            {
                sInsertCol = aInsertCols;
    
                for( i = 0; i < aTableHeader->mKeyColCount; i++ )
                {
                    sColLen = KNL_GET_BLOCK_DATA_LENGTH( sInsertCol, sBlockIdx );
                    STL_TRY_THROW( sColLen != 0, RAMP_NEXT_ROW );
                    
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
                sColLen = KNL_GET_BLOCK_DATA_LENGTH( sInsertCol, sBlockIdx );
                sColPos = KNL_GET_BLOCK_DATA_PTR( sInsertCol, sBlockIdx );

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
    
            SMDMIH_SET_OVERFLOW_KEY( sRow, sDirSlot->mKeyLa );
            SMDMIH_SET_HASH_KEY( sRow, aHashKey );
            SMDMIH_SET_FETCH_BIT( sRow, STL_FALSE );

            sRowLa = sNodeLa + ( sRow - sNode );
            sDirSlot->mKeyLa = sRowLa;

            SMDMIH_ADJUST_SCAN_HINT( aTableHeader, sBucketIdx );

            STL_RAMP( RAMP_NEXT_ROW );
        }
    }

    if( (sBlockIdx - sMergeStartBlockIdx) > 0 )
    {
        STL_TRY( smdmihMergeRecordInternal( aTableHeader,
                                            sDupRow,
                                            aMergeRecordInfo,
                                            sMergeStartBlockIdx,
                                            sBlockIdx,
                                            aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmihUpdate( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aAfterCols,
                        knlValueBlockList * aBeforeCols,
                        knlValueBlockList * aPrimaryKey,
                        stlBool           * aVerionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv )
{
    return STL_FAILURE;
}


stlStatus smdmihDelete( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aKeyColumns,
                        stlBool           * aVerionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv )
{
    knlValueBlockList * sKeyColumn;
    stlUInt32           sHashKey;
    stlUInt32           sFilterHashKey = 0;
    dtlDataValue      * sDataValue;
    smdmihTableHeader * sTableHeader = (smdmihTableHeader*)aRelationHandle;
    stlChar           * sKey;
    stlChar           * sPrevKey;
    knlLogicalAddr      sKeyLa = KNL_LOGICAL_ADDR_NULL;
    knlLogicalAddr      sNextKeyLa;
    knlLogicalAddr      sPrevKeyLa;
    smdmihDirSlot     * sDirSlot;
    stlBool             sMatched;
    stlInt32            sBucketIdx;

    *aVerionConflict = STL_FALSE;
    *aSkipped = STL_TRUE;
    
    sKeyColumn = aKeyColumns;
    
    while( sKeyColumn != NULL )
    {
        sDataValue = KNL_GET_BLOCK_DATA_VALUE( sKeyColumn, aValueIdx );
        sFilterHashKey += gDtlHash32[sDataValue->mType]( sDataValue->mValue,
                                                         sDataValue->mLength );
        
        sKeyColumn = sKeyColumn->mNext;
    }

    SMDMIH_DIR_SLOT( sTableHeader, sFilterHashKey, sDirSlot, sBucketIdx );

    sKeyLa = sDirSlot->mKeyLa;
    sPrevKeyLa = KNL_LOGICAL_ADDR_NULL;

    while( sKeyLa != KNL_LOGICAL_ADDR_NULL )
    {
        sKey = KNL_TO_PHYSICAL_ADDR( sKeyLa );
        SMDMIH_GET_HASH_KEY( sKey, sHashKey );
        SMDMIH_GET_OVERFLOW_KEY( sKey, sNextKeyLa );

        if( sHashKey != sFilterHashKey )
        {
            sPrevKeyLa = sKeyLa;
            sKeyLa = sNextKeyLa;
            continue;
        }

        sMatched = smdmihCompareKeyAndBlock( sTableHeader,
                                             aKeyColumns,
                                             aValueIdx,
                                             sKey );

        if( sMatched == STL_FALSE )
        {
            sPrevKeyLa = sKeyLa;
            sKeyLa = sNextKeyLa;
            continue;
        }

        /**
         * 지울 대상을 찾음
         * - 물리적으로 키를 삭제하지 않고 링크만 삭제한다.
         */

        /*
         * ( directory node )
         *      |
         *      ----> ( key1 ) ----> (key2) --> ... --> (keyn)
         */
        
        if( sPrevKeyLa == KNL_LOGICAL_ADDR_NULL )
        {
            /*
             * key1을 삭제하는 경우
             */
            
            sDirSlot->mKeyLa = sNextKeyLa;
        }
        else
        {
            /*
             * key2 ~ keyn을  삭제하는 경우
             */
            
            sPrevKey = KNL_TO_PHYSICAL_ADDR( sPrevKeyLa );
            SMDMIH_SET_OVERFLOW_KEY( sPrevKey, sNextKeyLa );
        }

        sTableHeader->mRowCount--;
        
        *aSkipped = STL_FALSE;
        break;
    }

    return STL_SUCCESS;
}


stlStatus smdmihInitTableHeader( smdmihTableHeader * aHashTableHeader,
                                 smlTbsId            aTbsId,
                                 smlIndexAttr      * aAttr,
                                 stlUInt16           aKeyColCount,
                                 knlValueBlockList * aRowColList,
                                 stlUInt8          * aKeyColFlags,
                                 smlEnv            * aEnv )
{
    knlValueBlockList * sColumn;
    stlInt64            sUsableBlockSize;
    
    aHashTableHeader->mColumns     = NULL;
    aHashTableHeader->mFreeDataBlockLa = KNL_LOGICAL_ADDR_NULL;
    aHashTableHeader->mDataNodeLa  = KNL_LOGICAL_ADDR_NULL;

    /**
     * hash key 개수 제약 풀기
     */
    sUsableBlockSize = smdmifGetBlockSize( aTbsId ) - STL_SIZEOF( smdmifDefBlockHeader );
    
    aHashTableHeader->mBlockSize      = sUsableBlockSize;
    aHashTableHeader->mDataNodeSize   = sUsableBlockSize;
    aHashTableHeader->mUsedDataBlockSize = 0;
    aHashTableHeader->mKeyColCount    = aKeyColCount;
    aHashTableHeader->mRowColCount    = 0;
    aHashTableHeader->mRowCount       = 0;
    aHashTableHeader->mUniqueness     = STL_FALSE;
    aHashTableHeader->mSkipNull       = STL_FALSE;
    aHashTableHeader->mColumns        = NULL;
    aHashTableHeader->mColumnInfo     = NULL;
    aHashTableHeader->mColCombineMem1 = NULL;
    aHashTableHeader->mColCombineMem2 = NULL;

    sColumn = aRowColList;

    while( sColumn != NULL )
    {
        aHashTableHeader->mRowColCount++;
        sColumn = sColumn->mNext;
    }

    if( aAttr != NULL )
    {
        if( (aAttr->mValidFlags & SML_INDEX_UNIQUENESS_MASK) == SML_INDEX_UNIQUENESS_YES )
        {
            aHashTableHeader->mUniqueness = aAttr->mUniquenessFlag;
        }
        if( (aAttr->mValidFlags & SML_INDEX_SKIP_NULL_MASK) == SML_INDEX_SKIP_NULL_YES )
        {
            aHashTableHeader->mSkipNull = aAttr->mSkipNullFlag;
        }
    }

    aHashTableHeader->mRelHeader.mRelationType = SML_MEMORY_INSTANT_HASH_TABLE;
    aHashTableHeader->mRelHeader.mRelHeaderSize = STL_SIZEOF( smdmihTableHeader);
    aHashTableHeader->mRelHeader.mCreateTransId = SML_INVALID_TRANSID;
    aHashTableHeader->mRelHeader.mDropTransId = SML_INVALID_TRANSID;
    aHashTableHeader->mRelHeader.mCreateScn = 0;
    aHashTableHeader->mRelHeader.mDropScn = 0;

    SME_SET_RELATION_STATE( (void*)&aHashTableHeader->mRelHeader, SME_RELATION_STATE_ACTIVE );
    SME_SET_OBJECT_SCN( (void*)&aHashTableHeader->mRelHeader, SML_INFINITE_SCN );
    SME_SET_SEGMENT_HANDLE( (void*)&aHashTableHeader->mRelHeader, NULL );

    aHashTableHeader->mDirectory = NULL;
    aHashTableHeader->mStartBucketIdx = STL_INT32_MAX;
    aHashTableHeader->mEndBucketIdx = -1;
    aHashTableHeader->mBucketModulusIdx = SMDMIH_INITIAL_MODULUS_ID;
    aHashTableHeader->mBucketCount = gSmdmihHashModulus[SMDMIH_INITIAL_MODULUS_ID].mPrimeNumber;
    aHashTableHeader->mModularConstant = gSmdmihHashModulus[SMDMIH_INITIAL_MODULUS_ID].mConstant;
    aHashTableHeader->mModularShiftBit = gSmdmihHashModulus[SMDMIH_INITIAL_MODULUS_ID].mShiftBit;
    aHashTableHeader->mAdjustConstant  = gSmdmihHashModulus[SMDMIH_INITIAL_MODULUS_ID].mAdjustConstant;
    
    return STL_SUCCESS;
}

stlStatus smdmihInitColumns( smdmihTableHeader * aHashTableHeader,
                             knlValueBlockList * aRowColList,
                             smlEnv            * aEnv )
{
    stlInt32            i;
    stlInt32            sRowLen;
    knlValueBlockList * sColumn;
    smdmifDefColumn   * sColumns = NULL;
    stlBool             sHasVarCol = STL_FALSE;
    stlInt32            sFixedKeyMaxLen;
    stlInt32            sLargestColSize = 0;
    smdmihColumnInfo  * sColumnInfo = NULL;
    
    /**
     * construct key columns
     */
    
    STL_TRY( smgAllocSessShmMem( STL_SIZEOF( smdmifDefColumn ) * aHashTableHeader->mRowColCount,
                                 (void**)&aHashTableHeader->mColumns,
                                 aEnv ) == STL_SUCCESS );

    STL_TRY( smgAllocSessShmMem( STL_SIZEOF( smdmihColumnInfo ) * aHashTableHeader->mRowColCount,
                                 (void**)&aHashTableHeader->mColumnInfo,
                                 aEnv ) == STL_SUCCESS );

    sColumns = aHashTableHeader->mColumns;
    sColumnInfo = aHashTableHeader->mColumnInfo;

    /**
     * key column의 fixed/var type 결정하고, offset을 설정한다.
     */
    
    sFixedKeyMaxLen = (stlInt32)knlGetPropertyBigIntValueByID( KNL_PROPERTY_INST_TABLE_FIXED_COL_MAX_LEN,
                                                               KNL_ENV( aEnv ) );

    sRowLen = SMDMIH_KEY_HEADER_SIZE;
    aHashTableHeader->mVarRowLen = 0;

    sColumn = aRowColList;
    
    for( i = 0; i < aHashTableHeader->mRowColCount; i++ )
    {
        sColumns[i].mDataType = KNL_GET_BLOCK_DB_TYPE( sColumn );
        sColumns[i].mMaxActualLen = 0;
        sColumns[i].mIsFixedPart = STL_FALSE;
        sColumns[i].mAllocSize = KNL_GET_BLOCK_DATA_BUFFER_SIZE( sColumn );
        sColumnInfo[i].mColOrder = sColumn->mColumnOrder;

        sLargestColSize = STL_MAX( sLargestColSize, sColumns[i].mAllocSize );

        if( sColumns[i].mAllocSize <= sFixedKeyMaxLen )
        {
            sColumns[i].mIsFixedPart = STL_TRUE;
        }

        STL_DASSERT( (sColumns[i].mDataType != DTL_TYPE_LONGVARCHAR) &&
                     (sColumns[i].mDataType != DTL_TYPE_LONGVARBINARY) );

        if( sColumns[i].mIsFixedPart == STL_FALSE )
        {
            sColumns[i].mOffset = sRowLen;
            sRowLen += STL_SIZEOF( knlLogicalAddr );
            sHasVarCol = STL_TRUE;
            aHashTableHeader->mVarRowLen += STL_SIZEOF( knlLogicalAddr );
        }

        sColumn = sColumn->mNext;
    }

    /**
     * key column 중 fixed part 컬럼들에 대해 offset을 설정한다.
     */

    sColumn = aRowColList;
    aHashTableHeader->mIsAllColumnLen1 = STL_TRUE;
    
    for( i = 0; i < aHashTableHeader->mRowColCount; i++ )
    {
        if( sColumns[i].mIsFixedPart == STL_TRUE )
        {
            sColumns[i].mOffset = sRowLen;
            sColumnInfo[i].mColLenSize = SMDMIH_COL_LEN_SIZE(sColumns[i].mAllocSize);
            sRowLen += ( sColumnInfo[i].mColLenSize + sColumns[i].mAllocSize );

            if( sColumnInfo[i].mColLenSize > 1 )
            {
                aHashTableHeader->mIsAllColumnLen1 = STL_FALSE;
            }
        }
        else
        {
            aHashTableHeader->mIsAllColumnLen1 = STL_FALSE;
        }
        
        sColumn = sColumn->mNext;
    }

    aHashTableHeader->mRowLen = sRowLen;
    
    /**
     * allocate combine memory
     */
    
    if( sHasVarCol == STL_TRUE )
    {
        STL_TRY( knlAllocLongVaryingMem( sLargestColSize,
                                         (void**)&aHashTableHeader->mColCombineMem1,
                                         KNL_ENV(aEnv) ) == STL_SUCCESS );

        STL_TRY( knlAllocLongVaryingMem( sLargestColSize,
                                         (void**)&aHashTableHeader->mColCombineMem2,
                                         KNL_ENV(aEnv) ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void smdmihFixedRowWriteCol( stlChar   * aDestColumn,
                             stlUInt16   aColLenSize,
                             stlChar   * aDataValue,
                             stlUInt16   aDataSize )
{
    if( aColLenSize == 1 )
    {
        *aDestColumn = aDataSize;
        stlMemcpy( aDestColumn + 1, aDataValue, aDataSize );
    }
    else
    {
        STL_WRITE_INT16( aDestColumn, &aDataSize );
        stlMemcpy( aDestColumn + 2, aDataValue, aDataSize );
    }
}

stlStatus smdmihCheckUniqueness( smdmihTableHeader  * aTableHeader,
                                 smdmihDirSlot      * aDirSlot,
                                 stlUInt32            aHashKey,
                                 knlValueBlockList  * aInsertCols,
                                 stlInt32             aBlockIdx,
                                 stlBool            * aUniqueViolation,
                                 stlChar           ** aDupRow,
                                 smlEnv             * aEnv )
{
    knlLogicalAddr   sKeyLa;
    stlChar        * sKey;
    stlUInt32        sHashKey;
    stlBool          sMatched;
    stlBool          sUniqueViolation = STL_FALSE;
    
    sKeyLa = aDirSlot->mKeyLa;
    
    while( sKeyLa != KNL_LOGICAL_ADDR_NULL )
    {
        sKey = KNL_TO_PHYSICAL_ADDR( sKeyLa );
        SMDMIH_GET_HASH_KEY( sKey, sHashKey );
        SMDMIH_GET_OVERFLOW_KEY( sKey, sKeyLa );
        
        if( sHashKey != aHashKey )
        {
            continue;
        }

        sMatched = smdmihCompareKeyAndBlock( aTableHeader,
                                             aInsertCols,
                                             aBlockIdx,
                                             sKey );

        if( sMatched == STL_FALSE )
        {
            continue;
        }

        sUniqueViolation = STL_TRUE;
        *aDupRow = sKey;
        break;
    }

    *aUniqueViolation = sUniqueViolation;
    
    return STL_SUCCESS;
}

stlStatus smdmihMergeRecordInternal( smdmihTableHeader  * aTableHeader,
                                     stlChar            * aRow,
                                     smlMergeRecordInfo * aMergeRecordInfo,
                                     stlInt32             aStartIdx,
                                     stlInt32             aEndIdx,
                                     smlEnv             * aEnv )
{
    smlAggrFuncInfo   * sAggrFuncInfo;
    knlValueBlockList * sReadBlock;
    knlValueBlockList * sUpdateBlock;
    stlUInt8            sHasExpr;

    sAggrFuncInfo = aMergeRecordInfo->mAggrFuncInfoList;
    sReadBlock = aMergeRecordInfo->mReadColList;
    sUpdateBlock = aMergeRecordInfo->mUpdateColList;
    
    while( sAggrFuncInfo != NULL )
    {
        sHasExpr = (sAggrFuncInfo->mEvalInfo[0] == NULL) ? STL_FALSE : STL_TRUE;
        
        STL_TRY( (sAggrFuncInfo->mAggrMergeFunc[sHasExpr])( aTableHeader,
                                                            aRow,
                                                            sAggrFuncInfo,
                                                            aStartIdx,
                                                            aEndIdx,
                                                            sReadBlock,
                                                            sUpdateBlock,
                                                            aEnv )
                 == STL_SUCCESS );
        
        sAggrFuncInfo = sAggrFuncInfo->mNext;
        sReadBlock = sReadBlock->mNext;
        sUpdateBlock = sUpdateBlock->mNext;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */

