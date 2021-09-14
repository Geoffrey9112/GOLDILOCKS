/*******************************************************************************
 * smxlRedo.c
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
#include <smxlDef.h>
#include <smrDef.h>
#include <smr.h>
#include <smp.h>
#include <smxl.h>
#include <smxlRedo.h>

/**
 * @file smxlRedo.c
 * @brief Storage Manager Layer Local Transactioanl Redo Internal Routines
 */

/**
 * @addtogroup smxlRedo
 * @{
 */

smrRedoVector gSmxlRedoVectors[SMR_TRANSACTION_LOG_MAX_COUNT] =
{
    { /* SMR_LOG_INSERT_UNDO_RECORD */
        smxlRedoInsertUndoRecord,
        NULL,
        "INSERT_UNDO_RECORD"
    },
    { /* SMR_LOG_INSERT_TRANSACTION_RECORD */
        smxlRedoInsertTransRecord,
        NULL,
        "INSERT_TRANSACTION_RECORD"
    },
    { /* SMR_LOG_UPDATE_UNDO_RECORD_FLAG */
        smxlRedoUpdateUndoRecordFlag,
        NULL,
        "UPDATE_UNDO_RECORD_FLAG"
    },
    { /* SMR_LOG_COMMIT */
        smxlRedoCommit,
        NULL,
        "COMMIT"
    },
    { /* SMR_LOG_ROLLBACK */
        smxlRedoRollback,
        NULL,
        "ROLLBACK"
    },
    { /* SMR_LOG_PREPARE */
        smxlRedoPrepare,
        NULL,
        "PREPARE"
    }
};

stlStatus smxlRedoInsertUndoRecord( smlRid    * aDataRid,
                                    void      * aData,
                                    stlUInt16   aDataSize,
                                    smpHandle * aPageHandle,
                                    smlEnv    * aEnv )
{
    stlChar * sUndoRecord;

    STL_TRY( smpAllocNthSlot( aPageHandle,
                              aDataRid->mOffset,
                              aDataSize,
                              STL_FALSE, /* aIsForTest */
                              &sUndoRecord )
             == STL_SUCCESS );

    STL_WRITE_BYTES( sUndoRecord, aData, aDataSize );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smxlRedoInsertTransRecord( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv )
{
    stlChar   * sUndoRecord;
    smlScn      sMinTransViewScn;

    STL_TRY( smpAllocNthSlot( aPageHandle,
                              aDataRid->mOffset,
                              aDataSize,
                              STL_FALSE, /* aIsForTest */
                              &sUndoRecord )
             == STL_SUCCESS );

    STL_WRITE_BYTES( sUndoRecord, aData, aDataSize );

    sMinTransViewScn = smxlGetTransViewScnFromUndoLog( aData );

    if( sMinTransViewScn < smpGetAgableScn( aPageHandle ) )
    {
        STL_TRY( smpUpdateAgableScn( NULL,
                                     aPageHandle,
                                     sMinTransViewScn,
                                     aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smxlRedoUpdateUndoRecordFlag( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv )
{
    stlChar        * sUndoRecord;
    smxlUndoRecHdr   sUndoRecHdr;

    STL_ASSERT( aDataSize == 1 );
    
    sUndoRecord = smpGetNthRowItem( aPageHandle,
                                    aDataRid->mOffset );

    SMXL_READ_UNDO_REC_HDR( &sUndoRecHdr, sUndoRecord );
    sUndoRecHdr.mFlag = *((stlChar*)aData);
    SMXL_WRITE_UNDO_REC_HDR( sUndoRecord, &sUndoRecHdr );

    return STL_SUCCESS;
}

stlStatus smxlRedoCommit( smlRid    * aDataRid,
                          void      * aData,
                          stlUInt16   aDataSize,
                          smpHandle * aPageHandle,
                          smlEnv    * aEnv )
{
    stlChar * sUndoRecord;

    sUndoRecord = smpGetNthRowItem( aPageHandle,
                                    aDataRid->mOffset );

    /**
     * transaction comment는 반영하지 않는다.
     */
    STL_WRITE_BYTES( sUndoRecord + SMXL_UNDO_REC_HDR_SIZE,
                     aData,
                     STL_SIZEOF(smxlTransRecord) );

    return STL_SUCCESS;
}

stlStatus smxlRedoPrepare( smlRid    * aDataRid,
                           void      * aData,
                           stlUInt16   aDataSize,
                           smpHandle * aPageHandle,
                           smlEnv    * aEnv )
{
    /**
     * Prepare는 Page연산을 수행하지 않는다.
     */
    
    STL_ASSERT( STL_FALSE );
    
    return STL_SUCCESS;
}

stlStatus smxlRedoRollback( smlRid    * aDataRid,
                            void      * aData,
                            stlUInt16   aDataSize,
                            smpHandle * aPageHandle,
                            smlEnv    * aEnv )
{
    stlChar * sUndoRecord;

    STL_ASSERT( aDataSize == STL_SIZEOF( smxlTransRecord ) );

    sUndoRecord = smpGetNthRowItem( aPageHandle,
                                    aDataRid->mOffset );

    STL_WRITE_BYTES( sUndoRecord + SMXL_UNDO_REC_HDR_SIZE,
                     aData,
                     aDataSize );

    return STL_SUCCESS;
}

/** @} */
