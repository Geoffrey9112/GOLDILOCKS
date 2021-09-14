/*******************************************************************************
 * smtRedo.c
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

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smf.h>
#include <smp.h>
#include <smtDef.h>
#include <smt.h>
#include <smg.h>
#include <smxm.h>
#include <smtTbs.h>
#include <smtExtMgr.h>
#include <smtRedo.h>
#include <smtMtxUndo.h>

/**
 * @file smtRedo.c
 * @brief Storage Manager Layer Tablespace Redo Internal Routines
 */

/**
 * @addtogroup smtRedo
 * @{
 */

smrRedoVector gSmtRedoVectors[SMR_TABLESPACE_LOG_MAX_COUNT] =
{
    { /* SMR_LOG_ALLOC_EXTENT */
        smtRedoAllocExtent,
        smtMtxUndoAllocExtent,
        "ALLOC_EXTENT"
    },
    { /* SMR_LOG_FREE_EXTENT */
        smtRedoFreeExtent,
        NULL,
        "FREE_EXTENT"
    },
    { /* SMR_LOG_ADD_EXTENT_BLOCK */
        smtRedoAddExtentBlock,
        smtMtxUndoAddExtentBlock,
        "ADD_EXTENT_BLOCK"
    },
    { /* SMR_LOG_CREATE_EXTENT_BLOCK */
        smtRedoCreateExtentBlock,
        NULL,
        "CREATE_EXTENT_BLOCK"
    },
    { /* SMR_LOG_CREATE_EXTENT_BLOCK_MAP */
        smtRedoCreateExtentBlockMap,
        NULL,
        "CREATE_EXTENT_BLOCK_MAP"
    },
    { /* SMR_LOG_UPDATE_EXT_BLOCK_DESC_STATE */
        smtRedoUpdateExtBlockDescState,
        smtMtxUndoUpdateExtBlockDescState,
        "UPDATE_EXTENT_BLOCK_DESC_STATE"
    },
    { /* SMR_LOG_UPDATE_EXT_BLOCK_MAP_HDR_STATE */
        smtRedoUpdateExtBlockMapHdrState,
        NULL,
        "UPDATE_EXTENT_BLOCK_MAP_HDR_STATE"
    }
};

stlStatus smtRedoAllocExtent( smlRid    * aDataRid,
                              void      * aData,
                              stlUInt16   aDataSize,
                              smpHandle * aPageHandle,
                              smlEnv    * aEnv )
{
    stlInt16       sOffset;
    stlChar      * sExtBitArray;
    smtExtMapHdr * sExtMapHdr;
    
    sOffset = aDataRid->mOffset;
    
    sExtMapHdr = (smtExtMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sExtBitArray = SMT_EXT_BIT_ARRAY( sExtMapHdr );

    STL_DASSERT( smtIsFreeExt( sExtBitArray, sOffset ) == STL_TRUE );
    smtOnExtBit( sExtBitArray, sOffset );
    
    sExtMapHdr->mOnExtCount += 1;
    sExtMapHdr->mOffExtCount -= 1;
    
    STL_DASSERT( sExtMapHdr->mOffExtCount <= sExtMapHdr->mExtCount );
    STL_DASSERT( sExtMapHdr->mOnExtCount <= sExtMapHdr->mExtCount );
                
    return STL_SUCCESS;
}

stlStatus smtRedoFreeExtent( smlRid    * aDataRid,
                             void      * aData,
                             stlUInt16   aDataSize,
                             smpHandle * aPageHandle,
                             smlEnv    * aEnv )
{
    stlInt16       sOffset;
    stlChar      * sExtBitArray;
    smtExtMapHdr * sExtMapHdr;
    
    sOffset = aDataRid->mOffset;
    
    sExtMapHdr = (smtExtMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sExtBitArray = SMT_EXT_BIT_ARRAY( sExtMapHdr );

    STL_DASSERT( smtIsFreeExt( sExtBitArray, sOffset ) == STL_FALSE );
    smtOffExtBit( sExtBitArray, sOffset );
    
    sExtMapHdr->mOnExtCount -= 1;
    sExtMapHdr->mOffExtCount += 1;

    STL_DASSERT( sExtMapHdr->mOffExtCount <= sExtMapHdr->mExtCount );
    STL_DASSERT( sExtMapHdr->mOnExtCount <= sExtMapHdr->mExtCount );
                
    return STL_SUCCESS;
}
    
stlStatus smtRedoAddExtentBlock( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv )
{
    stlInt16            sOffset;
    smtExtBlockMapHdr * sExtBlockMapHdr;
    smtExtBlockMap    * sExtBlockMap;
    smlPid              sExtBlockPid;
    smtExtBlockDesc   * sExtBlockDesc;

    sOffset = aDataRid->mOffset;
    STL_WRITE_INT32( &sExtBlockPid, aData );
    
    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sExtBlockMap = (smtExtBlockMap*)((stlChar*)sExtBlockMapHdr +
                                     STL_SIZEOF( smtExtBlockMapHdr ));
    
    sExtBlockMapHdr->mExtBlockDescCount++;
    sExtBlockDesc = &sExtBlockMap->mExtBlockDesc[sOffset];

    sExtBlockDesc->mExtBlock = sExtBlockPid;
    sExtBlockDesc->mState = SMT_EBS_FREE;
        
    return STL_SUCCESS;
}

stlStatus smtRedoCreateExtentBlock( smlRid    * aDataRid,
                                    void      * aData,
                                    stlUInt16   aDataSize,
                                    smpHandle * aPageHandle,
                                    smlEnv    * aEnv )
{
    smtExtMapHdr * sDestExtMapHdr;
    smtExtMapHdr   sSrcExtMapHdr;
    stlChar      * sExtBitArray;
    stlUInt32      i;
    
    STL_WRITE_BYTES( &sSrcExtMapHdr, aData, STL_SIZEOF( smtExtMapHdr ) );
    
    sDestExtMapHdr = (smtExtMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    stlMemcpy( sDestExtMapHdr, &sSrcExtMapHdr, STL_SIZEOF( smtExtMapHdr ) );
    
    sExtBitArray = SMT_EXT_BIT_ARRAY( sDestExtMapHdr );

    for( i = 0; i < sDestExtMapHdr->mExtCount; i++ )
    {
        smtOffExtBit( sExtBitArray, i );
    }

    return STL_SUCCESS;
}

stlStatus smtRedoCreateExtentBlockMap( smlRid    * aDataRid,
                                       void      * aData,
                                       stlUInt16   aDataSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv )
{
    smtExtBlockMapHdr * sExtBlockMapHdr;
    smtExtBlockMap    * sExtBlockMap;
    stlUInt32           i;
    
    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sExtBlockMap = (smtExtBlockMap*)((stlChar*)sExtBlockMapHdr +
                                     STL_SIZEOF( smtExtBlockMapHdr ));

    for( i = 0; i < SMT_MAX_EXT_BLOCK_DESC_COUNT; i++ )
    {
        sExtBlockMap->mExtBlockDesc[i].mExtBlock = SMP_NULL_PID;
        sExtBlockMap->mExtBlockDesc[i].mState = SMT_EBS_FREE;
    }

    sExtBlockMapHdr->mExtBlockDescCount = 0;
    sExtBlockMapHdr->mState = SMT_EBMS_FREE;

    return STL_SUCCESS;
}

stlStatus smtRedoUpdateExtBlockDescState( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv )
{
    smtExtBlockMapHdr * sExtBlockMapHdr;
    smtExtBlockMap    * sExtBlockMap;
    stlUInt32           sBeforeState;
    stlUInt32           sAfterState;
    stlInt16            sOffset = 0;
    
    STL_READ_MOVE_INT32( &sBeforeState, aData, sOffset );
    STL_READ_MOVE_INT32( &sAfterState, aData, sOffset );
    
    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sExtBlockMap = (smtExtBlockMap*)((stlChar*)sExtBlockMapHdr +
                                     STL_SIZEOF( smtExtBlockMapHdr ));

    sExtBlockMap->mExtBlockDesc[aDataRid->mOffset].mState = sAfterState;

    return STL_SUCCESS;
}

stlStatus smtRedoUpdateExtBlockMapHdrState( smlRid    * aDataRid,
                                            void      * aData,
                                            stlUInt16   aDataSize,
                                            smpHandle * aPageHandle,
                                            smlEnv    * aEnv )
{
    smtExtBlockMapHdr * sExtBlockMapHdr;
    stlUInt32           sState;
    stlInt16            sOffset = 0;
    
    STL_READ_MOVE_INT32( &sState, aData, sOffset );
    
    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );

    sExtBlockMapHdr->mState = sState;

    return STL_SUCCESS;
}

/** @} */
