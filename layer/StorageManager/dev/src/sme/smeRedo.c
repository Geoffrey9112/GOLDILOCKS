/*******************************************************************************
 * smeRedo.c
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
#include <smeRedo.h>
#include <smeDef.h>

/**
 * @file smdRedo.c
 * @brief Storage Manager Table Redo Internal Routines
 */

/**
 * @addtogroup smdRedo
 * @{
 */

smrRedoVector gSmeRedoVectors[SMR_RELATION_LOG_MAX_COUNT] =
{
    { /* SMR_LOG_INIT_REL_HEADER */
        smeRedoInitRelationHeader,
        NULL,
        "INIT_RELATION_HEADER"
    },
    { /* SMR_LOG_SAVE_COLUMN_LENGTH */
        smeRedoColumnLength,
        NULL,
        "SAVE_COLUMN_LENGTH"
    }
};

stlStatus smeRedoInitRelationHeader( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv )
{
    stlChar * sPtr = (stlChar*)SMP_FRAME(aPageHandle) + aDataRid->mOffset;

    STL_WRITE_BYTES( sPtr, aData, aDataSize );

    return STL_SUCCESS;
}

stlStatus smeRedoColumnLength( smlRid    * aDataRid,
                               void      * aData,
                               stlUInt16   aDataSize,
                               smpHandle * aPageHandle,
                               smlEnv    * aEnv )
{
    stlChar * sPtr = (stlChar *)SMP_FRAME(aPageHandle) + aDataRid->mOffset;

    STL_WRITE_BYTES( sPtr, aData, aDataSize );
    
    return STL_SUCCESS;
}


/** @} */
