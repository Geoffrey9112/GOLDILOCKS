/*******************************************************************************
 * smsmpRedo.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smsmpRedo.c 1548 2011-07-28 02:48:10Z mycomman $
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
#include <smt.h>
#include <smxm.h>
#include <smxl.h>
#include <smp.h>
#include <smg.h>
#include <smsDef.h>
#include <smsmpDef.h>
#include <smsmpRedo.h>

/**
 * @file smsmpRedo.c
 * @brief Storage Manager Layer Pending Segment Redo Internal Routines
 */

/**
 * @addtogroup smsmpRedo
 * @{
 */

stlStatus smsmpRedoUpdateSegHdr( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv )
{
    smsmpSegHdr   sSegHdrLog;
    smsmpSegHdr * sSegHdr;

    STL_WRITE_BYTES( &sSegHdrLog, aData, STL_SIZEOF( smsmpSegHdr ) );

    sSegHdr = (smsmpSegHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    stlMemcpy( sSegHdr, &sSegHdrLog, STL_SIZEOF( smsmpSegHdr ) );
    
    return STL_SUCCESS;
}

stlStatus smsmpRedoUpdateExtHdr( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv )
{
    smsmpExtHdr   sExtHdrLog;
    smsmpExtHdr * sExtHdr;

    STL_WRITE_BYTES( &sExtHdrLog, aData, STL_SIZEOF( smsmpExtHdr ) );

    sExtHdr = (smsmpExtHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    stlMemcpy( sExtHdr, &sExtHdrLog, STL_SIZEOF( smsmpExtHdr ) );
    
    return STL_SUCCESS;
}

/** @} */
