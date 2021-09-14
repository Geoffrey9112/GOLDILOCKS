/*******************************************************************************
 * smklFxTable.c
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
#include <smkl.h>
#include <smklDef.h>
#include <smklFxTable.h>

/**
 * @file smklFxTable.c
 * @brief Storage Manager Layer Local Lock Manager Fixed Table Internal Routines
 */

/**
 * @addtogroup smklFxTable
 * @{
 */

knlFxColumnDesc gLockWaitColumnDesc[] =
{
    {
        "GRANTED_TRANSACTION_SLOT_ID",
        "granted transaction slot identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smklFxLockWait, mGrantedTransSlotId ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "REQUEST_TRANSACTION_SLOT_ID",
        "request transaction slot identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smklFxLockWait, mRequestTransSlotId ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};

stlStatus smklOpenLockWaitCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv )
{
    smklFxPathCtrl * sPathCtrl;

    sPathCtrl = (smklFxPathCtrl*)aPathCtrl;
    sPathCtrl->mX = 0;
    sPathCtrl->mY = 0;
    
    return STL_SUCCESS;
}

stlStatus smklCloseLockWaitCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smklBuildRecordLockWaitCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aEnv )
{
    smklFxPathCtrl * sPathCtrl;
    stlUInt32        i;
    stlUInt32        j;
    smklFxLockWait   sFxLockWait;

    sPathCtrl = (smklFxPathCtrl*)aPathCtrl;
    *aTupleExist = STL_FALSE;
        
    for( i = sPathCtrl->mY; i < SMKL_LOCK_WAIT_TABLE_SIZE; i++ )
    {
        for( j = sPathCtrl->mX; j < SMKL_LOCK_WAIT_TABLE_SIZE; j++ )
        {
            if( SMKL_HAS_EDGE( j, i ) == STL_FALSE )
            {
                continue;
            }

            sFxLockWait.mGrantedTransSlotId = i;
            sFxLockWait.mRequestTransSlotId = j;
            
            STL_TRY( knlBuildFxRecord( gLockWaitColumnDesc,
                                       &sFxLockWait,
                                       aValueList,
                                       aBlockIdx,
                                       aEnv )
                     == STL_SUCCESS );
            
            sPathCtrl->mY = i;
            sPathCtrl->mX = j + 1;
            
            *aTupleExist = STL_TRUE;
            break;
        }

        if( *aTupleExist == STL_TRUE )
        {
            break;
        }
        
        sPathCtrl->mX = 0;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gLockWaitTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    NULL,
    smklOpenLockWaitCallback,
    smklCloseLockWaitCallback,
    smklBuildRecordLockWaitCallback,
    STL_SIZEOF( smklFxPathCtrl ),
    "X$LOCK_WAIT",
    "lock wait graph information",
    gLockWaitColumnDesc
};

/** @} */
