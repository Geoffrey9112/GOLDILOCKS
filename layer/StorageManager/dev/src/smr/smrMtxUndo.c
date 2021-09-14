/*******************************************************************************
 * smrMtxUndo.c
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
#include <smrDef.h>
#include <smr.h>
#include <smp.h>
#include <smrLogFile.h>
#include <smrLogBuffer.h>
#include <smrMtxUndo.h>

/**
 * @file smrMtxUndo.c
 * @brief Storage Manager Layer Recovery Mini-Transaction Undo Internal Routines
 */

extern smrRedoVector * gSmrRedoVectorsTable[];

/**
 * @addtogroup smr
 * @{
 */

stlStatus smrMtxUndo( smrLogPieceHdr * aLogPieceHdr,
                      stlChar        * aLogPieceBody,
                      smlEnv         * aEnv )
{
    smlRid           sDataRid;
    smpHandle        sPageHandle;
    stlUInt32        sState = 0;
    smrRedoVector  * sRedoVectors;
    stlInt16         sComponentClass;

    SMG_WRITE_RID( &sDataRid, &aLogPieceHdr->mDataRid );

    STL_TRY( smpFix( sDataRid.mTbsId,
                     sDataRid.mPageId,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 0;

    sComponentClass = (stlInt16)aLogPieceHdr->mComponentClass;
    sRedoVectors = gSmrRedoVectorsTable[sComponentClass];
    STL_ASSERT( sRedoVectors != NULL );
        
    STL_TRY( sRedoVectors[aLogPieceHdr->mType].mMtxUndoFunc( &sDataRid,
                                                             aLogPieceBody,
                                                             aLogPieceHdr->mSize,
                                                             &sPageHandle,
                                                             aEnv )
             == STL_SUCCESS );

    sState = 1;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smrMtxUndoBytes( smlRid    * aDataRid,
                           void      * aData,
                           stlUInt16   aDataSize,
                           smpHandle * aPageHandle,
                           smlEnv    * aEnv )
{
    stlChar * sBeforeImage;

    sBeforeImage = (stlChar*)aData;
    stlMemcpy( ((stlChar*)SMP_FRAME(aPageHandle)) + aDataRid->mOffset,
               sBeforeImage,
               (aDataSize / 2) );
    
    return STL_SUCCESS;
}

/** @} */
