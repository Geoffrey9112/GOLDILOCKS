/*******************************************************************************
 * smsmbRedo.c
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
#include <smt.h>
#include <smxm.h>
#include <smxl.h>
#include <smp.h>
#include <smg.h>
#include <smsDef.h>
#include <smsmcDef.h>
#include <smsmcRedo.h>

/**
 * @file smsmbRedo.c
 * @brief Storage Manager Layer Bitmap Segment Redo Internal Routines
 */

/**
 * @addtogroup smsmbRedo
 * @{
 */

stlStatus smsmcRedoUpdateSegHdr( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv )
{
    smsmcSegHdr   sSegHdrLog;
    smsmcSegHdr * sSegHdr;

    STL_WRITE_BYTES( &sSegHdrLog, aData, STL_SIZEOF( smsmcSegHdr ) );

    sSegHdr = (smsmcSegHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    stlMemcpy( sSegHdr, &sSegHdrLog, STL_SIZEOF( smsmcSegHdr ) );
    
    return STL_SUCCESS;
}

stlStatus smsmcRedoUpdateExtHdr( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv )
{
    smsmcExtHdr   sExtHdrLog;
    smsmcExtHdr * sExtHdr;

    STL_WRITE_BYTES( &sExtHdrLog, aData, STL_SIZEOF( smsmcExtHdr ) );

    sExtHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    stlMemcpy( sExtHdr, &sExtHdrLog, STL_SIZEOF( smsmcExtHdr ) );
    
    return STL_SUCCESS;
}

/** @} */
