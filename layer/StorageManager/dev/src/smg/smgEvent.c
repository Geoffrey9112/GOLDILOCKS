/*******************************************************************************
 * smgEvent.c
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

/**
 * @file smgEvent.c
 * @brief Storage Manager Layer Event Internal Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smr.h>
#include <smp.h>
#include <smd.h>
#include <smn.h>
#include <smt.h>
#include <smq.h>
#include <smf.h>
#include <smg.h>
#include <smxl.h>
#include <smgDef.h>

extern smgWarmupEntry  * gSmgWarmupEntry;

/**
 * @addtogroup smg
 * @{
 */

knlEventTable gSmgEventTable[SML_MAX_EVENT_COUNT] =
{
    {/* CANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "checkpoint",
        smrCheckpointEventHandler
    },
    {/* NONCANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "drop table",
        smdDropAging
    },
    {/* NONCANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "drop Index",
        smnDropAging
    },
    {/* NONCANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "archive log",
        smrArchivelogEventHandler
    },
    {/* NONCANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "deactivate ager thread",
        smgDeactivateAgerEventHandler
    },
    {/* NONCANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "drop tablespace",
        smfDropTbsAging
    },
    {/* NONCANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "drop datafile",
        smfDropDatafileAging
    },
    {/* NONCANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "page flushing",
        smpPageFlushEventHandler
    },
    {/* NONCANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "wakeup log flusher",
        smgWakeupLogFlusherEventHandler
    },
    {/* NONCANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "page flushing for tbs",
        smpPageFlushTbsEventHandler
    },
    {/* CANCELABLE EVENT, CANCELABLE EXECUTION */
        "loop-back event",
        smgLoopbackEventHandler
    },
    {/* NONCANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "datafile load",
        smfLoadEventHandler
    },
    {/* NONCANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "flush datafile for io slaves",
        smpFlushPagesIoSlaveEventHandler
    },
    {/* CANCELABLE EVENT, CANCELABLE EXECUTION */
        "add logfile group",
        smrAddLogGroupEventHandler
    },
    {/* CANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "drop logfile group",
        smrDropLogGroupEventHandler
    },
    {/* CANCELABLE EVENT, CANCELABLE EXECUTION */
        "rename logfile",
        smrRenameLogFileEventHandler
    },
    {/* CANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "switch logfile group",
        smrSwitchLogGroupEventHandler
    },
    {/* CANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "alter database archivelog",
        smrAlterArchivelogEventHandler
    },
    {/* CANCELABLE EVENT, CANCELABLE EXECUTION */
        "alter database recover",
        smgMediaRecoverEventHandler
    },
    {/* PROCESS EVENT */
        "escalate warmup phase",
        smgEscalateWarmupPhase
    },
    {/* CANCELABLE EVENT, CANCELABLE EXECUTION */
        "alter database restore",
        smgRestoreEventHandler
    },
    {/* NONCANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "drop sequence",
        smqDropAging
    },
    {/* NONCANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "free lock item",
        smdLockItemAging
    },
    {/* CANCELABLE EVENT, CANCELABLE EXECUTION */
        "add datafile",
        smtAddDatafileEventHandler
    },
    {/* CANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "rename datafile",
        smtRenameDatafileEventHandler
    },
    {/* CANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "offline tablespace",
        smtOfflineTablespaceEventHandler
    },
    {/* CANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "add logfile member",
        smrAddLogMemberEventHandler
    },
    {/* CANCELABLE EVENT, NONCANCELABLE EXECUTION */
        "drop logfile member",
        smrDropLogMemberEventHandler

    }
};

stlStatus smgDeactivateAgerEventHandler( void      * aData,
                                         stlUInt32   aDataSize,
                                         stlBool   * aDone,
                                         void      * aEnv )
{
    gSmgWarmupEntry->mAgerState = SML_AGER_STATE_INACTIVE;

    *aDone = STL_TRUE;

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[AGER] deactivated \n" )
             == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smgWakeupLogFlusherEventHandler( void      * aData,
                                           stlUInt32   aDataSize,
                                           stlBool   * aDone,
                                           void      * aEnv )
{
    *aDone = STL_TRUE;
        
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[LOG FLUSHER] got up \n" )
             == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smgLoopbackEventHandler( void      * aData,
                                   stlUInt32   aDataSize,
                                   stlBool   * aDone,
                                   void      * aEnv )
{
    stlBool    sBuildAgableScn;

    *aDone = STL_TRUE;

    STL_WRITE_BYTES( &sBuildAgableScn, aData, STL_SIZEOF(stlBool) );

    if( sBuildAgableScn == STL_TRUE )
    {
        STL_TRY( smxlBuildAgableScn( SML_ENV( aEnv ) ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
