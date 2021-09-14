/*******************************************************************************
 * smdRedo.c
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
#include <smp.h>
#include <smd.h>
#include <smdmphDef.h>
#include <smdmph.h>
#include <smdmpfDef.h>
#include <smdmpf.h>
#include <smdDef.h>
#include <smdRedo.h>
#include <smdMtxUndo.h>

/**
 * @file smdRedo.c
 * @brief Storage Manager Table Redo Internal Routines
 */

/**
 * @addtogroup smdRedo
 * @{
 */

extern smdTableRedoModule gSmdmphTableRedoModule;
extern smdTableRedoModule gSmdmpfTableRedoModule;

smdTableRedoModule * gSmdTableRedoModule[SMP_PAGE_TYPE_MAX] =
{
    NULL,        /* INIT */
    NULL,        /* UNFORMAT */
    NULL,        /* EXT_BLOCK_MAP */
    NULL,        /* EXT_MAP */
    NULL,        /* BITMAP_HEADER */
    NULL,        /* CIRCULAR_HEADER */
    NULL,        /* FLAT_HEADER */
    NULL,        /* PENDING_HEADER */
    NULL,        /* UNDO */
    NULL,        /* TRANS */
    NULL,        /* BITMAP_INTERNAL */
    NULL,        /* BITMAP_LEAF */
    NULL,        /* BITMAP_EXTENT_MAP */
    &gSmdmphTableRedoModule,        /* TABLE_DATA */
    &gSmdmpfTableRedoModule,        /* COLUMNAR_TABLE_DATA */
    NULL         /* INDEX_DATA*/
};

smrRedoVector gSmdRedoVectors[SMR_TABLE_LOG_MAX_COUNT] =
{
    { /* SMR_LOG_MEMORY_HEAP_INSERT */
        smdRedoMemoryHeapInsert,
        smdMtxUndoMemoryHeapInsert,
        "MEMORY_HEAP_INSERT"
    },
    { /* SMR_LOG_MEMORY_HEAP_UNINSERT */
        smdRedoMemoryHeapUninsert,
        NULL,
        "MEMORY_HEAP_UNINSERT"
    },
    { /* SMR_LOG_MEMORY_HEAP_UPDATE */
        smdRedoMemoryHeapUpdate,
        NULL,
        "MEMORY_HEAP_UPDATE"
    },
    { /* SMR_LOG_MEMORY_HEAP_UNUPDATE */
        smdRedoMemoryHeapUnupdate,
        NULL,
        "MEMORY_HEAP_UNUPDATE"
    },
    { /* SMR_LOG_MEMORY_HEAP_MIGRATE */
        smdRedoMemoryHeapMigrate,
        NULL,
        "MEMORY_HEAP_MIGRATE"
    },
    { /* SMR_LOG_MEMORY_HEAP_UNMIGRATE */
        smdRedoMemoryHeapUnmigrate,
        NULL,
        "MEMORY_HEAP_UNMIGRATE"
    },
    { /* SMR_LOG_MEMORY_HEAP_DELETE */
        smdRedoMemoryHeapDelete,
        NULL,
        "MEMORY_HEAP_DELETE"
    },
    { /* SMR_LOG_MEMORY_HEAP_UNDELETE */
        smdRedoMemoryHeapUndelete,
        NULL,
        "MEMORY_HEAP_UNDELETE"
    },
    { /* SMR_LOG_MEMORY_HEAP_UPDATE_LINK */
        smdRedoMemoryHeapUpdateLink,
        NULL,
        "MEMORY_HEAP_UPDATE_LINK"
    },
    { /* SMR_LOG_MEMORY_HEAP_INSERT_FOR_UPDATE */
        smdRedoMemoryHeapInsertForUpdate,
        NULL,
        "MEMORY_HEAP_INSERT_FOR_UPDATE"
    },
    { /* SMR_LOG_MEMORY_HEAP_DELETE_FOR_UPDATE */
        smdRedoMemoryHeapDeleteForUpdate,
        NULL,
        "MEMORY_HEAP_DELETE_FOR_UPDATE"
    },
    { /* SMR_LOG_MEMORY_HEAP_UPDATE_LOGICAL_HDR */
        smdRedoMemoryHeapUpdateLogicalHdr,
        NULL,
        "MEMORY_HEAP_UPDATE_LOGICAL_HEADER"
    },
    { /* SMR_LOG_MEMORY_HEAP_APPEND */
        smdRedoMemoryHeapAppend,
        NULL,
        "MEMORY_HEAP_APPEND"
    },
    { /* SMR_LOG_MEMORY_HEAP_UNAPPEND */
        smdRedoMemoryHeapUnappend,
        NULL,
        "MEMORY_HEAP_UNAPPEND"
    },
    { /* SMR_LOG_MEMORY_PENDING_INSERT */
        smdRedoMemoryPendingInsert,
        NULL,
        "MEMORY_PENDING_INSERT"
    },
    { /* SMR_LOG_MEMORY_HEAP_COMPACTION */
        smdRedoMemoryHeapCompaction,
        NULL,
        "MEMORY_HEAP_COMPACTION"
    }
};

stlStatus smdRedoMemoryHeapInsert( smlRid    * aDataRid,
                                   void      * aData,
                                   stlUInt16   aDataSize,
                                   smpHandle * aPageHandle,
                                   smlEnv    * aEnv )
{
    smpPageType     sPageType;

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    STL_TRY( gSmdTableRedoModule[sPageType]->mRedoInsert( aDataRid,
                                                          aData,
                                                          aDataSize,
                                                          aPageHandle,
                                                          aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdRedoMemoryHeapUninsert( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv )
{
    smpPageType     sPageType;

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    STL_TRY( gSmdTableRedoModule[sPageType]->mRedoUninsert( aDataRid,
                                                            aData,
                                                            aDataSize,
                                                            aPageHandle,
                                                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdRedoMemoryHeapUpdate( smlRid    * aDataRid,
                                   void      * aData,
                                   stlUInt16   aDataSize,
                                   smpHandle * aPageHandle,
                                   smlEnv    * aEnv )
{
    smpPageType     sPageType;

    STL_DASSERT( (aDataRid->mTbsId  != 0) ||
                 (aDataRid->mPageId != 0) ||
                 (aDataRid->mOffset != 0) );

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    STL_TRY( gSmdTableRedoModule[sPageType]->mRedoUpdate( aDataRid,
                                                          aData,
                                                          aDataSize,
                                                          aPageHandle,
                                                          aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdRedoMemoryHeapUnupdate( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv )
{
    smpPageType     sPageType;

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    STL_TRY( gSmdTableRedoModule[sPageType]->mRedoUnupdate( aDataRid,
                                                            aData,
                                                            aDataSize,
                                                            aPageHandle,
                                                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



stlStatus smdRedoMemoryHeapMigrate( smlRid    * aDataRid,
                                    void      * aData,
                                    stlUInt16   aDataSize,
                                    smpHandle * aPageHandle,
                                    smlEnv    * aEnv )
{
    smpPageType     sPageType;

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    STL_TRY( gSmdTableRedoModule[sPageType]->mRedoMigrate( aDataRid,
                                                           aData,
                                                           aDataSize,
                                                           aPageHandle,
                                                           aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdRedoMemoryHeapUpdateLink( smlRid    * aDataRid,
                                       void      * aData,
                                       stlUInt16   aDataSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv )
{
    smpPageType     sPageType;

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    STL_TRY( gSmdTableRedoModule[sPageType]->mRedoUpdateLink( aDataRid,
                                                              aData,
                                                              aDataSize,
                                                              aPageHandle,
                                                              aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdRedoMemoryHeapUnmigrate( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    smpPageType     sPageType;

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    STL_TRY( gSmdTableRedoModule[sPageType]->mRedoUnmigrate( aDataRid,
                                                             aData,
                                                             aDataSize,
                                                             aPageHandle,
                                                             aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdRedoMemoryHeapDelete( smlRid    * aDataRid,
                                   void      * aData,
                                   stlUInt16   aDataSize,
                                   smpHandle * aPageHandle,
                                   smlEnv    * aEnv )
{
    smpPageType     sPageType;

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    STL_TRY( gSmdTableRedoModule[sPageType]->mRedoDelete( aDataRid,
                                                          aData,
                                                          aDataSize,
                                                          aPageHandle,
                                                          aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdRedoMemoryHeapUndelete( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv )
{
    smpPageType     sPageType;

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    STL_TRY( gSmdTableRedoModule[sPageType]->mRedoUndelete( aDataRid,
                                                            aData,
                                                            aDataSize,
                                                            aPageHandle,
                                                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdRedoMemoryHeapInsertForUpdate( smlRid    * aDataRid,
                                            void      * aData,
                                            stlUInt16   aDataSize,
                                            smpHandle * aPageHandle,
                                            smlEnv    * aEnv )
{
    /* CDC를 위해 REDO TYPE만 따로 만들고 실제 redo는 일반 insert 와 동일하다 */
    return smdRedoMemoryHeapInsert( aDataRid, aData, aDataSize, aPageHandle, aEnv );
}


stlStatus smdRedoMemoryHeapDeleteForUpdate( smlRid    * aDataRid,
                                            void      * aData,
                                            stlUInt16   aDataSize,
                                            smpHandle * aPageHandle,
                                            smlEnv    * aEnv )
{
    /* CDC를 위해 REDO TYPE만 따로 만들고 실제 redo는 일반 delete 와 동일하다 */
    return smdRedoMemoryHeapDelete( aDataRid, aData, aDataSize, aPageHandle, aEnv );
}

stlStatus smdRedoMemoryHeapUpdateLogicalHdr( smlRid    * aDataRid,
                                             void      * aData,
                                             stlUInt16   aDataSize,
                                             smpHandle * aPageHandle,
                                             smlEnv    * aEnv )
{
    smpPageType     sPageType;

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    STL_TRY( gSmdTableRedoModule[sPageType]->mRedoUpdateLogicalHdr( aDataRid,
                                                                    aData,
                                                                    aDataSize,
                                                                    aPageHandle,
                                                                    aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdRedoMemoryHeapAppend( smlRid    * aDataRid,
                                   void      * aData,
                                   stlUInt16   aDataSize,
                                   smpHandle * aPageHandle,
                                   smlEnv    * aEnv )
{
    smpPageType     sPageType;

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    STL_TRY( gSmdTableRedoModule[sPageType]->mRedoAppend( aDataRid,
                                                          aData,
                                                          aDataSize,
                                                          aPageHandle,
                                                          aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdRedoMemoryHeapUnappend( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv )
{
    smpPageType     sPageType;

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    STL_TRY( gSmdTableRedoModule[sPageType]->mRedoUnappend( aDataRid,
                                                            aData,
                                                            aDataSize,
                                                            aPageHandle,
                                                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdRedoMemoryPendingInsert( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    stlInt16    sSlotSeq = aDataRid->mOffset;
    stlChar   * sSlot;

    STL_TRY( smpAllocSlot( aPageHandle,
                           aDataSize,
                           STL_FALSE,  /* aIsForTest */
                           &sSlot,
                           &sSlotSeq )
             == STL_SUCCESS );
    
    stlMemcpy( sSlot, aData, aDataSize );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdRedoMemoryHeapCompaction( smlRid    * aDataRid,
                                       void      * aData,
                                       stlUInt16   aDataSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv )
{
    smpPageType     sPageType;

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(aPageHandle) );

    STL_TRY( gSmdTableRedoModule[sPageType]->mRedoCompaction( aDataRid,
                                                              aData,
                                                              aDataSize,
                                                              aPageHandle,
                                                              aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
