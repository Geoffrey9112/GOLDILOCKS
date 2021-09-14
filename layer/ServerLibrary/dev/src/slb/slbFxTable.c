/*******************************************************************************
 * slbFxTable.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: slbFxTable.c 13560 2014-09-12 06:47:23Z kyungoh $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <cml.h>
#include <knl.h>
#include <scl.h>
#include <sml.h>
#include <ell.h>
#include <qll.h>
#include <ssl.h>
#include <sll.h>
#include <slDef.h>
#include <slb.h>

/**
 * @file slbFxTable.c
 * @brief Server Libarary General Fixed Table Internal Routines
 */

/**
 * @addtogroup slbFxTable
 * @{
 */

/**
 * X$SL_SYSTEM_INFO
 */
knlFxSystemInfo gSlbSystemInfoRows[SLB_SYSTEM_INFO_ROW_COUNT] =
{
    {
        "SHARED_SERVER_ACTIVITY",
        0,
        KNL_STAT_CATEGORY_NONE,
        "shared server activity( 0: IN-ACTIVE, 1: ACTIVE )"
    },
    {
        "CM_UNIT_SIZE",
        0,
        KNL_STAT_CATEGORY_NONE,
        "communication unit size (bytes)"
    },
    {
        "TOTAL_CM_UNIT_COUNT",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        "total number of communication unit"
    },
    {
        "USED_CM_UNIT_COUNT",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        "communication unit number in use"
    },
    {
        "REQUEST_SHARED_JOB_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        "request shared job count"
    },
    {
        "REQUEST_QUEUE_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        "request queue count"
    }
};

stlStatus slbOpenSystemInfoFxTableCallback( void   * aStmt,
                                            void   * aDumpObjHandle,
                                            void   * aPathCtrl,
                                            knlEnv * aKnlEnv )
{
    slbFxSystemInfoPathCtrl * sPathCtrl;
    stlInt64                * sValues;
    stlInt32                  sRowIdx = 0;
    stlArrayAllocator       * sPacketAllocator;
    sclQueue               ** sRequestEvent;
    stlInt32                  i;

    sPathCtrl  = (slbFxSystemInfoPathCtrl*)aPathCtrl;
    sPathCtrl->mIterator = 0;

    sValues = sPathCtrl->mValues;

    /**
     * Warmup Entry
     */

    sValues[sRowIdx++] = knlGetPropertyBoolValueByID( KNL_PROPERTY_SHARED_SESSION,
                                                      aKnlEnv );

    /**
     * Packet Allocator
     */

    sPacketAllocator = gSlSharedMemory->mPacketAllocator;

    if( sPacketAllocator != NULL )
    {
        sValues[sRowIdx++] = sPacketAllocator->mBodySize - STL_SIZEOF(sclIpcPacket);
        sValues[sRowIdx++] = sPacketAllocator->mTotalElementCount;
        sValues[sRowIdx++] = sPacketAllocator->mUsedElementCount;
    }
    else
    {
        sValues[sRowIdx++] = 0;
        sValues[sRowIdx++] = 0;
        sValues[sRowIdx++] = 0;
    }

    sRequestEvent = gSlSharedMemory->mRequestEvent;
    
    if( sRequestEvent != NULL )
    {
        sValues[sRowIdx] = 0;
        
        for( i = 0; i < gSlSharedMemory->mRequestQueueCount; i++ )
        {
            sValues[sRowIdx] += ( sRequestEvent[i]->mFront -
                                  sRequestEvent[i]->mRear );
        }
        
        sRowIdx++;
    }
    else
    {
        sValues[sRowIdx++] = 0;
    }
    
    sValues[sRowIdx++] = gSlSharedMemory-> mRequestQueueCount;
    
    STL_DASSERT( sRowIdx == SLB_SYSTEM_INFO_ROW_COUNT );

    return STL_SUCCESS;
}

stlStatus slbCloseSystemInfoFxTableCallback( void   * aDumpObjHandle,
                                             void   * aPathCtrl,
                                             knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus slbBuildRecordSystemInfoFxTableCallback( void              * aDumpObjHandle,
                                                   void              * aPathCtrl,
                                                   knlValueBlockList * aValueList,
                                                   stlInt32            aBlockIdx,
                                                   stlBool           * aTupleExist,
                                                   knlEnv            * aKnlEnv )
{
    slbFxSystemInfoPathCtrl * sPathCtrl;
    knlFxSystemInfo           sFxSystemInfo;
    knlFxSystemInfo         * sRowDesc;
    stlInt64                * sValues;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl = (slbFxSystemInfoPathCtrl*)aPathCtrl;
    sValues = sPathCtrl->mValues;

    sRowDesc = &gSlbSystemInfoRows[sPathCtrl->mIterator];

    if( sPathCtrl->mIterator < SLB_SYSTEM_INFO_ROW_COUNT )
    {
        sFxSystemInfo.mName     = sRowDesc->mName;
        sFxSystemInfo.mValue    = sValues[sPathCtrl->mIterator];
        sFxSystemInfo.mComment  = sRowDesc->mComment;
        sFxSystemInfo.mCategory = sRowDesc->mCategory;

        STL_TRY( knlBuildFxRecord( gKnlSystemInfoColumnDesc,
                                   &sFxSystemInfo,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        *aTupleExist = STL_TRUE;
        sPathCtrl->mIterator += 1;
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


knlFxTableDesc gSlbSystemInfoTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    NULL,
    slbOpenSystemInfoFxTableCallback,
    slbCloseSystemInfoFxTableCallback,
    slbBuildRecordSystemInfoFxTableCallback,
    STL_SIZEOF( slbFxSystemInfoPathCtrl ),
    "X$SL_SYSTEM_INFO",
    "system information of server library",
    gKnlSystemInfoColumnDesc
};

/**
 * X$DISPATCHER
 */
knlFxColumnDesc gSlbDispatcherColumnDesc[] =
{
    {
        "PROCESS_ID",
        "dispatcher process identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxDispatcher, mProcId ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "RESPONSE_JOB_COUNT",
        "response job count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxDispatcher, mResponseJobCount ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "ACCEPT",
        "indicates whether this dispatcher is accepting new connections",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( slbFxDispatcher, mAccept ),
        STL_SIZEOF(stlBool),
        STL_FALSE  /* nullable */
    },
    {
        "START_TIME",
        "process start time",
        KNL_FXTABLE_DATATYPE_TIMESTAMP,
        STL_OFFSETOF( slbFxDispatcher, mStartTime ),
        STL_SIZEOF(stlTime),
        STL_FALSE  /* nullable */
    },
    {
        "CUR_CONNECTIONS",
        "current number of connections",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxDispatcher, mCurrentConnections ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "CONNECTIONS",
        "total number of connections",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxDispatcher, mConnections ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "CONNECTIONS_HIGHWATER",
        "highest number of connections",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxDispatcher, mConnectionsHighwater ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "MAX_CONNECTIONS",
        "maximum connections",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxDispatcher, mMaxConnections ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "RECV_STATUS",
        "receive status",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( slbFxDispatcher, mRecvStatus ),
        SLB_STATUS_LEN,
        STL_FALSE  /* nullable */
    },
    {
        "RECV_BYTES",
        "total bytes of received",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxDispatcher, mTotalRecvBytes ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "RECV_UNITS",
        "total units of received",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxDispatcher, mTotalRecvUnits ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "RECV_IDLE",
        "total idle time of receive (1/100 second)",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxDispatcher, mRecvIdle ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "RECV_BUSY",
        "total busy time of receive (1/100 second)",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxDispatcher, mRecvBusy ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "SEND_STATUS",
        "send status",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( slbFxDispatcher, mSendStatus ),
        SLB_STATUS_LEN,
        STL_FALSE  /* nullable */
    },
    {
        "SEND_BYTES",
        "total bytes of sent",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxDispatcher, mTotalSendBytes ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "SEND_UNITS",
        "total units of sent",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxDispatcher, mTotalSendUnits ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "SEND_IDLE",
        "total idle time of send (1/100 second)",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxDispatcher, mSendIdle ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "SEND_BUSY",
        "total busy time of send (1/100 second)",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxDispatcher, mSendBusy ),
        STL_SIZEOF(stlInt64),
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

stlStatus slbOpenDispatcherCallback( void   * aStmt,
                                     void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    *(stlInt64*)aPathCtrl = 0;
    
    return STL_SUCCESS;
}

stlStatus slbCloseDispatcherCallback( void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus slbBuildRecordDispatcherCallback( void              * aDumpObjHandle,
                                            void              * aPathCtrl,
                                            knlValueBlockList * aValueList,
                                            stlInt32            aBlockIdx,
                                            stlBool           * aTupleExist,
                                            knlEnv            * aEnv )
{
    stlInt64           sDispatcherId;
    sllDispatcher    * sDispatcher;
    slbFxDispatcher    sFxDispatcher;
    sclQueue         * sResponseEvent;
    
    *aTupleExist = STL_FALSE;

    sDispatcherId = *(stlInt64*)aPathCtrl;
    
    STL_TRY_THROW( sDispatcherId < gSlSharedMemory->mDispatchers,
                   RAMP_FINISH );

    sDispatcher = &(gSlSharedMemory->mDispatcher[sDispatcherId]);

    sFxDispatcher.mProcId = sDispatcher->mThread.mProc.mProcID;
    sFxDispatcher.mCurrentConnections = sDispatcher->mSocketCount;
    sFxDispatcher.mResponseJobCount = 0;
    sFxDispatcher.mAccept = sDispatcher->mAccept;
    sFxDispatcher.mStartTime = sDispatcher->mStartTime;

    sResponseEvent = sDispatcher->mResponseEvent;
    
    if( sResponseEvent != NULL )
    {
        sFxDispatcher.mResponseJobCount = ( sResponseEvent->mFront -
                                            sResponseEvent->mRear );
    }
    
    sFxDispatcher.mConnections = sDispatcher->mConnections;
    sFxDispatcher.mConnectionsHighwater = sDispatcher->mConnectionsHighwater;
    sFxDispatcher.mMaxConnections = sDispatcher->mMaxConnections;
    slbStatusString( sDispatcher->mRecvStatus, sFxDispatcher.mRecvStatus );
    sFxDispatcher.mTotalRecvBytes = sDispatcher->mTotalRecvBytes;
    sFxDispatcher.mTotalRecvUnits = sDispatcher->mTotalRecvUnits;
    /**
     * micro second를 1/100 second로 변경을 위해 [/ 10000] 한다.
     */
    sFxDispatcher.mRecvIdle = sDispatcher->mRecvIdle / 10000;
    sFxDispatcher.mRecvBusy = sDispatcher->mRecvBusy / 10000;
    slbStatusString( sDispatcher->mSendStatus, sFxDispatcher.mSendStatus );
    sFxDispatcher.mTotalSendBytes = sDispatcher->mTotalSendBytes;
    sFxDispatcher.mTotalSendUnits = sDispatcher->mTotalSendUnits;
    /**
     * micro second를 1/100 second로 변경을 위해 [/ 10000] 한다.
     */
    sFxDispatcher.mSendIdle = sDispatcher->mSendIdle / 10000;
    sFxDispatcher.mSendBusy = sDispatcher->mSendBusy / 10000;

    STL_TRY( knlBuildFxRecord( gSlbDispatcherColumnDesc,
                               &sFxDispatcher,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );
    
    *aTupleExist = STL_TRUE;
    *(stlInt64*)aPathCtrl += 1;

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSlbDispatcherTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    NULL,
    slbOpenDispatcherCallback,
    slbCloseDispatcherCallback,
    slbBuildRecordDispatcherCallback,
    STL_SIZEOF( stlInt64 ),
    "X$DISPATCHER",
    "information of dispatchers",
    gSlbDispatcherColumnDesc
};


/**
 * X$SHARED_SERVER
 */
knlFxColumnDesc gSlbSharedServerColumnDesc[] =
{
    {
        "PROCESS_ID",
        "shared server process identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxSharedServer, mProcId ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "PROCESSED_JOB_COUNT",
        "processed job count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxSharedServer, mProcessedJobCount ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "STATUS",
        "status",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( slbFxSharedServer, mStatus ),
        SLB_STATUS_LEN,
        STL_FALSE  /* nullable */
    },
    {
        "IDLE",
        "total idle time (1/100 second)",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxSharedServer, mIdle ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "BUSY",
        "total busy time (1/100 second)",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxSharedServer, mBusy ),
        STL_SIZEOF(stlInt64),
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

stlStatus slbOpenSharedServerCallback( void   * aStmt,
                                       void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aEnv )
{
    *(stlInt64*)aPathCtrl = 0;
    
    return STL_SUCCESS;
}

stlStatus slbCloseSharedServerCallback( void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus slbBuildRecordSharedServerCallback( void              * aDumpObjHandle,
                                              void              * aPathCtrl,
                                              knlValueBlockList * aValueList,
                                              stlInt32            aBlockIdx,
                                              stlBool           * aTupleExist,
                                              knlEnv            * aEnv )
{
    stlInt64             sSharedServerId;
    sllSharedServer    * sSharedServer;
    slbFxSharedServer    sFxSharedServer;
    
    *aTupleExist = STL_FALSE;

    sSharedServerId = *(stlInt64*)aPathCtrl;
    
    STL_TRY_THROW( sSharedServerId < gSlSharedMemory->mSharedServers,
                   RAMP_FINISH );

    sSharedServer = &(gSlSharedMemory->mSharedServer[sSharedServerId]);

    sFxSharedServer.mProcId = sSharedServer->mThread.mProc.mProcID;
    sFxSharedServer.mProcessedJobCount = sSharedServer->mJobCount;

    slbStatusString( sSharedServer->mStatus, sFxSharedServer.mStatus );

    /**
     * micro second를 1/100 second로 변경을 위해 [/ 10000] 한다.
     */
    sFxSharedServer.mIdle = sSharedServer->mIdle / 10000;
    sFxSharedServer.mBusy = sSharedServer->mBusy / 10000;

    STL_TRY( knlBuildFxRecord( gSlbSharedServerColumnDesc,
                               &sFxSharedServer,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );
    
    *aTupleExist = STL_TRUE;
    *(stlInt64*)aPathCtrl += 1;

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSlbSharedServerTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    NULL,
    slbOpenSharedServerCallback,
    slbCloseSharedServerCallback,
    slbBuildRecordSharedServerCallback,
    STL_SIZEOF( stlInt64 ),
    "X$SHARED_SERVER",
    "information of shared servers",
    gSlbSharedServerColumnDesc
};


/**
 * X$BALANCER
 */
knlFxColumnDesc gSlbBalancerColumnDesc[] =
{
    {
        "PROCESS_ID",
        "balancer process identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxBalancer, mProcId ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "CUR_CONNECTIONS",
        "current number of connections",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxBalancer, mCurrentConnections ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "CONNECTIONS",
        "total number of connections",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxBalancer, mConnections ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "CONNECTIONS_HIGHWATER",
        "highest number of connections",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxBalancer, mConnectionsHighwater ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "MAX_CONNECTIONS",
        "maximum connections",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxBalancer, mMaxConnections ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "STATUS",
        "status",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( slbFxBalancer, mStatus ),
        SLB_STATUS_LEN,
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

stlStatus slbOpenBalancerCallback( void   * aStmt,
                                   void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv )
{
    *(stlBool*)aPathCtrl = STL_FALSE;

    return STL_SUCCESS;
}

stlStatus slbCloseBalancerCallback( void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus slbBuildRecordBalancerCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aTupleExist,
                                          knlEnv            * aEnv )
{
    sllBalancer    * sBalancer;
    slbFxBalancer    sFxBalancer;

    *aTupleExist = STL_FALSE;

    STL_TRY_THROW( *(stlBool*)aPathCtrl == STL_FALSE, RAMP_FINISH );

    sBalancer = &(gSlSharedMemory->mBalancer);

    sFxBalancer.mProcId = sBalancer->mThread.mProc.mProcID;
    sFxBalancer.mCurrentConnections = sBalancer->mCurrentConnections;
    sFxBalancer.mConnections = sBalancer->mConnections;
    sFxBalancer.mConnectionsHighwater = sBalancer->mConnectionsHighwater;
    sFxBalancer.mMaxConnections = sBalancer->mMaxConnections;
    slbStatusString( sBalancer->mStatus, sFxBalancer.mStatus );

    STL_TRY( knlBuildFxRecord( gSlbBalancerColumnDesc,
                               &sFxBalancer,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;
    *(stlBool*)aPathCtrl = STL_TRUE;

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSlbBalancerTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    NULL,
    slbOpenBalancerCallback,
    slbCloseBalancerCallback,
    slbBuildRecordBalancerCallback,
    STL_SIZEOF( stlBool ),
    "X$BALANCER",
    "information of balancer",
    gSlbBalancerColumnDesc
};



/**
 * X$QUEUE
 */
knlFxColumnDesc gSlbQueueColumnDesc[] =
{
    {
        "TYPE",
        "queue type ( COMMON or DISPATCHER )",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( slbFxQueue, mType ),
        SLB_TYPE_LEN,
        STL_FALSE  /* nullable */
    },
    {
        "INDEX",
        "index",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxQueue, mIndex ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },

    {
        "QUEUED",
        "number of items in the queue",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxQueue, mQueued ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "WAIT",
        "total time that all items in this queue have waited (1/100 second)",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxQueue, mWait ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "TOTALQ",
        "total number of items that have ever been in the queue",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( slbFxQueue, mTotalQueue ),
        STL_SIZEOF(stlInt64),
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


stlStatus slbOpenQueueCallback( void   * aStmt,
                                void   * aDumpObjHandle,
                                void   * aPathCtrl,
                                knlEnv * aEnv )
{
    *(stlInt64*)aPathCtrl = 0;

    return STL_SUCCESS;
}

stlStatus slbCloseQueueCallback( void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus slbBuildRecordQueueCallback( void              * aDumpObjHandle,
                                       void              * aPathCtrl,
                                       knlValueBlockList * aValueList,
                                       stlInt32            aBlockIdx,
                                       stlBool           * aTupleExist,
                                       knlEnv            * aEnv )
{
    stlInt64      sQueueId;
    sclQueue    * sQueue;
    slbFxQueue    sFxQueue;

    *aTupleExist = STL_FALSE;

    sQueueId = *(stlInt64*)aPathCtrl;

    /**
     * Queue의 개수는 SLL_MAX_REQUEST_QUEUE_COUNT + dispatcher 개수
     */
    STL_TRY_THROW( sQueueId < SLL_MAX_REQUEST_QUEUE_COUNT + gSlSharedMemory->mDispatchers,
                   RAMP_FINISH );

    if( sQueueId < gSlSharedMemory->mDispatchers )
    {
        sQueue = gSlSharedMemory->mDispatcher[sQueueId].mResponseEvent;
        sFxQueue.mIndex = sQueueId;
        stlStrcpy( sFxQueue.mType, "DISPATCHER" );
    }
    else
    {
        sQueue = gSlSharedMemory->mRequestEvent[sQueueId - gSlSharedMemory->mDispatchers];

        STL_TRY_THROW( sQueue != NULL, RAMP_FINISH );

        sFxQueue.mIndex = sQueueId - gSlSharedMemory->mDispatchers;
        stlStrcpy( sFxQueue.mType, "COMMON" );
    }


    sFxQueue.mQueued = sQueue->mQueued;
    /**
     * Queue에 대기시간
     * micro second를 1/100 second로 변경을 위해 [/ 10000] 한다.
     */
    sFxQueue.mWait = sQueue->mWait / 10000;
    sFxQueue.mTotalQueue = sQueue->mTotalQueue;

    STL_TRY( knlBuildFxRecord( gSlbQueueColumnDesc,
                               &sFxQueue,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;
    *(stlInt64*)aPathCtrl += 1;

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSlbQueueTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    NULL,
    slbOpenQueueCallback,
    slbCloseQueueCallback,
    slbBuildRecordQueueCallback,
    STL_SIZEOF( stlInt64 ),
    "X$QUEUE",
    "information of queue",
    gSlbQueueColumnDesc
};


void slbStatusString( sllStatus       aStatus,
                      stlChar       * aString )
{
    switch( aStatus )
    {
        case SLL_STATUS_WAIT:
            stlStrcpy( aString, "WAIT" );
            break;
        case SLL_STATUS_SEND:
            stlStrcpy( aString, "SEND" );
            break;
        case SLL_STATUS_RECEIVE:
            stlStrcpy( aString, "RECEIVE" );
            break;
        case SLL_STATUS_RUN:
            stlStrcpy( aString, "RUN" );
            break;
        default:
            stlStrcpy( aString, "UNKNOWN" );
            break;
    }

    STL_DASSERT( stlStrlen( aString ) < SLB_STATUS_LEN );
}

/** @} */
