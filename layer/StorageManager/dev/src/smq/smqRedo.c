/*******************************************************************************
 * smqRedo.c
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
#include <smqDef.h>
#include <smqRedo.h>

/**
 * @file smqRedo.c
 * @brief Storage Manager Layer Sequence Redo Internal Routines
 */

/**
 * @addtogroup smqRedo
 * @{
 */

smrRedoVector gSmqRedoVectors[SMR_SEQUENCE_LOG_MAX_COUNT] =
{
    { /* SMR_LOG_UPDATE_SEQUENCE_VALUE */
        smqRedoUpdateSeqValue,
        NULL,
        "UPDATE_SEQUENCE_VALUE"
    }
};

stlStatus smqRedoUpdateSeqValue( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv )
{
    stlChar   * sSeqSlot;
    stlChar   * sSeqBody;
    smqRecord   sSeqRecord;
    stlInt64    sRecordLen;
    stlInt64    sSequenceValue;
    stlInt16    sSlotBodyOffset;
    stlInt16    sLogOffset = 0;

    STL_READ_MOVE_INT16( &sSlotBodyOffset, aData, sLogOffset );
    STL_READ_MOVE_INT64( &sSequenceValue, aData, sLogOffset );
    
    sSeqSlot = smpGetNthRowItem( aPageHandle,
                                 aDataRid->mOffset );
    sSeqBody = sSeqSlot + sSlotBodyOffset;
    
    SMP_READ_COLUMN_ZERO_INSENS( sSeqBody, (void*)&sSeqRecord, &sRecordLen );
    STL_ASSERT( sRecordLen == STL_SIZEOF( smqRecord ) );

    sSeqRecord.mRestartValue = sSequenceValue;

    SMP_WRITE_COLUMN_ZERO_INSENS( sSeqBody, (void*)&sSeqRecord, sRecordLen );
        
    return STL_SUCCESS;
}

/** @} */
