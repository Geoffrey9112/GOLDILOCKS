/*******************************************************************************
 * smnmihIndex.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smnmihIndex.c 6322 2012-11-13 05:16:47Z egon $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smnmihIndex.c
 * @brief Storage Manager Layer Memory Instant Hash IndexTable Routines
 */

#include <sml.h>
#include <smDef.h>
#include <smg.h>
#include <sma.h>
#include <smd.h>
#include <smo.h>
#include <smsDef.h>
#include <smsManager.h>
#include <smdDef.h>
#include <smdmifTable.h>
#include <smnmih.h>
#include <smdmifVarPart.h>

/**
 * @addtogroup smnmihIndex
 * @{
 */

/* Memory Instant Hash Index Module */
smnIndexModule gSmnmihModule =
{
    smnmihCreate,
    NULL, /* CreateUndo */
    smnmihDrop,
    NULL, /* AlterDataType */
    NULL, /* AlterIndexAttr */
    NULL, /* CreateForTruncate */
    NULL, /* DropAging */
    NULL, /* Initialize */
    NULL, /* Finalize */
    smnmihBuild,
    NULL, /* BuildUnusable */
    NULL, /* BuildForTruncate */
    smnmihInsert,
    NULL, /* Delete */
    smnmihBlockInsert,
    NULL, /* BlockDelete */
    NULL, /* GetSlotBody */
    NULL, /* InitModuleSpecificData */
    NULL, /* DestModuleSpecificData */
    smnmihTruncate
};

stlStatus smnmihCreate( smlStatement      * aStatement,
                        smlTbsId            aTbsId,
                        smlIndexAttr      * aAttr,
                        void              * aBaseTableHandle,
                        stlUInt16           aKeyColCount,
                        knlValueBlockList * aKeyColList,
                        stlUInt8          * aKeyColFlags,
                        stlInt64          * aSegmentId,
                        void             ** aRelationHandle,
                        smlEnv            * aEnv)
{
    smnmihIndexHeader * sHashIndexHeader = NULL;
    stlInt32            sState = 0;
    
    STL_TRY( smgAllocSessShmMem( STL_SIZEOF( smnmihIndexHeader ),
                                 (void**)&sHashIndexHeader,
                                 aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smnmihInitIndexHeader( sHashIndexHeader,
                                    aBaseTableHandle,
                                    aTbsId,
                                    aAttr,
                                    aKeyColCount,
                                    aKeyColList,
                                    aKeyColFlags,
                                    aEnv )
             == STL_SUCCESS );

    STL_TRY( smdAddHandleToSession( aStatement,
                                    (smeRelationHeader*)sHashIndexHeader,
                                    aEnv ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( smnmihInitColumns( sHashIndexHeader,
                                aKeyColList,
                                aEnv ) == STL_SUCCESS );
    
    *aRelationHandle = (void*)sHashIndexHeader;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smnmihDrop( aStatement, (void*)sHashIndexHeader, aEnv );
            break;
        case 1:
            (void) smgFreeSessShmMem( sHashIndexHeader, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smnmihInitIndexHeader( smnmihIndexHeader * aHashIndexHeader,
                                 void              * aTableHandle,
                                 smlTbsId            aTbsId,
                                 smlIndexAttr      * aAttr,
                                 stlUInt16           aKeyColCount,
                                 knlValueBlockList * aKeyColList,
                                 stlUInt8          * aKeyColFlags,
                                 smlEnv            * aEnv )
{
    aHashIndexHeader->mFreeBlockLa = KNL_LOGICAL_ADDR_NULL;
    aHashIndexHeader->mDataNodeLa  = KNL_LOGICAL_ADDR_NULL;
    
    aHashIndexHeader->mBlockSize      = smdmifGetBlockSize( aTbsId );
    aHashIndexHeader->mDataNodeSize   = SMNMIH_DATA_NODE_SIZE;
    aHashIndexHeader->mUsedBlockSize  = 0;
    aHashIndexHeader->mKeyColCount    = aKeyColCount;
    aHashIndexHeader->mDirectory      = NULL;
    aHashIndexHeader->mColumnInfo     = NULL;
    aHashIndexHeader->mColCombineMem  = NULL;
    aHashIndexHeader->mRowColumn      = NULL;
    aHashIndexHeader->mSkipNull       = STL_FALSE;
    aHashIndexHeader->mBaseHeader     = (smdmifTableHeader*)aTableHandle;
    aHashIndexHeader->mTotalKeyCount  = 0;

    if( aAttr != NULL )
    {
        if( (aAttr->mValidFlags & SML_INDEX_SKIP_NULL_MASK) == SML_INDEX_SKIP_NULL_YES )
        {
            aHashIndexHeader->mSkipNull = aAttr->mSkipNullFlag;
        }
    }

    aHashIndexHeader->mRelHeader.mRelationType = SML_MEMORY_INSTANT_HASH_INDEX;
    aHashIndexHeader->mRelHeader.mRelHeaderSize = STL_SIZEOF( smnmihIndexHeader);
    aHashIndexHeader->mRelHeader.mCreateTransId = SML_INVALID_TRANSID;
    aHashIndexHeader->mRelHeader.mDropTransId = SML_INVALID_TRANSID;
    aHashIndexHeader->mRelHeader.mCreateScn = 0;
    aHashIndexHeader->mRelHeader.mDropScn = 0;

    SME_SET_RELATION_STATE( (void*)&aHashIndexHeader->mRelHeader, SME_RELATION_STATE_ACTIVE );
    SME_SET_OBJECT_SCN( (void*)&aHashIndexHeader->mRelHeader, SML_INFINITE_SCN );
    SME_SET_SEGMENT_HANDLE( (void*)&aHashIndexHeader->mRelHeader, NULL );

    return STL_SUCCESS;
}

stlStatus smnmihInitColumns( smnmihIndexHeader * aHashIndexHeader,
                             knlValueBlockList * aKeyColList,
                             smlEnv            * aEnv )
{
    stlInt32             i;
    knlValueBlockList  * sColumn;
    smnmihColumnInfo   * sColumnInfo = NULL;
    
    /**
     * construct key columns
     */
    
    STL_TRY( smgAllocSessShmMem( STL_SIZEOF( smnmihColumnInfo ) * aHashIndexHeader->mKeyColCount,
                                 (void**)&sColumnInfo,
                                 aEnv ) == STL_SUCCESS );

    aHashIndexHeader->mColumnInfo = sColumnInfo;

    sColumn = aKeyColList;
    
    for( i = 0; i < aHashIndexHeader->mKeyColCount; i++ )
    {
        /**
         * Long varying type은 key column으로 사용할수 없다.
         */
        STL_DASSERT( (KNL_GET_BLOCK_DB_TYPE(sColumn) != DTL_TYPE_LONGVARCHAR) &&
                     (KNL_GET_BLOCK_DB_TYPE(sColumn) != DTL_TYPE_LONGVARBINARY) );
        
        sColumnInfo[i].mColOrder = sColumn->mColumnOrder;
        sColumn = sColumn->mNext;

    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmihDrop( smlStatement * aStatement,
                      void         * aRelationHandle,
                      smlEnv       * aEnv )
{
    smnmihIndexHeader * sHashIndexHeader = (smnmihIndexHeader *)aRelationHandle;

    if( sHashIndexHeader->mColCombineMem != NULL )
    {
        STL_TRY( smgFreeSessShmMem( sHashIndexHeader->mColCombineMem,
                                    aEnv ) == STL_SUCCESS );
        sHashIndexHeader->mColCombineMem = NULL;
    }

    if( sHashIndexHeader->mRowColumn != NULL )
    {
        STL_TRY( smgFreeSessShmMem( sHashIndexHeader->mRowColumn,
                                    aEnv ) == STL_SUCCESS );
        sHashIndexHeader->mRowColumn = NULL;
    }
    
    if( sHashIndexHeader->mColumnInfo != NULL )
    {
        STL_TRY( smgFreeSessShmMem( sHashIndexHeader->mColumnInfo,
                                    aEnv ) == STL_SUCCESS );
        sHashIndexHeader->mColumnInfo = NULL;
    }

    if( sHashIndexHeader->mDirectory != NULL )
    {
        STL_TRY( smgFreeSessShmMem( sHashIndexHeader->mDirectory,
                                    aEnv ) == STL_SUCCESS );
        sHashIndexHeader->mDirectory = NULL;
    }
    
    STL_TRY( smdRemoveHandleFromSession( aStatement,
                                         (smeRelationHeader*)sHashIndexHeader,
                                         aEnv ) == STL_SUCCESS );

    STL_TRY( smgFreeSessShmMem( sHashIndexHeader,
                                aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmihBuild( smlStatement      * aStatement,
                       void              * aIndexHandle,
                       void              * aBaseTableHandle,
                       stlUInt16           aKeyColCount,
                       knlValueBlockList * aIndexColList,
                       stlUInt8          * aKeyColFlags,
                       stlUInt16           aParallelFactor,
                       smlEnv            * aEnv )
{
    smnmihColumnInfo  * sColumnInfo;
    smnmihIndexHeader * sIndexHeader;
    smdmifTableHeader * sTableHeader;
    smnmihNodeHeader  * sNodeHeader;
    stlInt32            i;
    smnmihDirSlot     * sDirSlot;
    

    sIndexHeader = (smnmihIndexHeader*)aIndexHandle;
    sTableHeader = (smdmifTableHeader*)aBaseTableHandle;

    /**
     * allocate row column anchor
     */
    
    STL_TRY( smgAllocSessShmMem( STL_SIZEOF( smdmifDefColumn* ) * sIndexHeader->mKeyColCount,
                                 (void**)&sIndexHeader->mRowColumn,
                                 aEnv ) == STL_SUCCESS );

    sColumnInfo = sIndexHeader->mColumnInfo;
    
    for( i = 0; i < sIndexHeader->mKeyColCount; i++ )
    {
        sIndexHeader->mRowColumn[i] = &(sTableHeader->mCols[sColumnInfo[i].mColOrder]);
    }

    /**
     * allocate combine memory
     */

    if( sTableHeader->mVarPartColCount > 0 )
    {
        STL_TRY( smgAllocSessShmMem( sTableHeader->mLargestColSize,
                                     (void**)&sIndexHeader->mColCombineMem,
                                     aEnv ) == STL_SUCCESS );
    }

    /**
     * allocate root directory
     */
    
    sIndexHeader->mTotalKeyCount = SMDMIF_TABLE_GET_TOTAL_ROW_COUNT( sTableHeader );

    smnmihCalcBucketCount( sIndexHeader, sIndexHeader->mTotalKeyCount );

    STL_TRY( smgAllocSessShmMem( sIndexHeader->mBucketCount * STL_SIZEOF(smnmihDirSlot),
                                 (void**)&(sIndexHeader->mDirectory),
                                 aEnv ) == STL_SUCCESS );

    sDirSlot = sIndexHeader->mDirectory;

    for( i = 0; i < sIndexHeader->mBucketCount; i++ )
    {
        sDirSlot->mKeyLa = KNL_LOGICAL_ADDR_NULL;
        sDirSlot->mLastHashKey = 0;
        sDirSlot->mOverflowCount = 0;
        
        sDirSlot++;
    }

    /**
     * allocate data node
     */
    
    if( sIndexHeader->mTotalKeyCount > 0 )
    {
        STL_TRY( smnmihAllocDataNode( sIndexHeader,
                                      (stlChar**)&sNodeHeader,
                                      aEnv )
                 == STL_SUCCESS );
    
        sNodeHeader->mUsedSize = STL_SIZEOF(smnmihNodeHeader);
    
        /**
         * build hash index
         */

        STL_TRY( smnmihBuildNodes( sIndexHeader,
                                   sTableHeader,
                                   aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmihTruncate( smlStatement * aStatement,
                          void         * aRelationHandle,
                          smlEnv       * aEnv )
{
    smnmihIndexHeader * sIndexHeader;
    
    sIndexHeader = (smnmihIndexHeader*)aRelationHandle;

    if( sIndexHeader->mColCombineMem != NULL )
    {
        STL_TRY( smgFreeSessShmMem( sIndexHeader->mColCombineMem,
                                    aEnv ) == STL_SUCCESS );
        sIndexHeader->mColCombineMem = NULL;
    }

    if( sIndexHeader->mRowColumn != NULL )
    {
        STL_TRY( smgFreeSessShmMem( sIndexHeader->mRowColumn,
                                    aEnv ) == STL_SUCCESS );
        sIndexHeader->mRowColumn = NULL;
    }
    
    if( sIndexHeader->mDirectory != NULL )
    {
        STL_TRY( smgFreeSessShmMem( sIndexHeader->mDirectory,
                                    aEnv ) == STL_SUCCESS );
        sIndexHeader->mDirectory = NULL;
    }
    
    sIndexHeader->mFreeBlockLa   = KNL_LOGICAL_ADDR_NULL;
    sIndexHeader->mDataNodeLa    = KNL_LOGICAL_ADDR_NULL;
    sIndexHeader->mUsedBlockSize = 0;
    sIndexHeader->mTotalKeyCount = 0;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmihInsert( smlStatement            * aStatement,
                        void                    * aRelationHandle,
                        knlValueBlockList       * aColumns,
                        stlInt32                  aValueIdx,
                        smlRid                  * aRowRid,
                        smlRid                  * aUndoRid,
                        stlBool                   aIsIndexBuilding,
                        stlBool                   aIsDeferred,
                        smlUniqueViolationScope * aUniqueViolationScope,
                        smlEnv                  * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smnmihBlockInsert( smlStatement            * aStatement,
                             void                    * aRelationHandle,
                             knlValueBlockList       * aColumns,
                             smlRowBlock             * aRowBlock,
                             stlBool                 * aBlockFilter,
                             stlBool                   aIsDeferred,
                             stlInt32                * aViolationCnt,
                             smlUniqueViolationScope * aUniqueViolationScope,
                             smlEnv                  * aEnv )
{
    return STL_SUCCESS;
}

/** @} */

