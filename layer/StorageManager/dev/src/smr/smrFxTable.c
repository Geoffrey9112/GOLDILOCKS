/*******************************************************************************
 * smrFxTable.c
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
#include <smg.h>
#include <smr.h>
#include <smrLogStream.h>
#include <smrLogGroup.h>
#include <smrLogMember.h>
#include <smlSuppLogDef.h>

/**
 * @file smrFxTable.c
 * @brief Storage Manager Layer Datafile Fixed Table Internal Routines
 */

/**
 * @addtogroup smrFxTable
 * @{
 */

extern smrWarmupEntry * gSmrWarmupEntry;
extern smrRedoVector gSmfRedoVectors[];
extern smrRedoVector gSmpRedoVectors[];
extern smrRedoVector gSmtRedoVectors[];
extern smrRedoVector gSmrRedoVectors[];
extern smrRedoVector gSmsRedoVectors[];
extern smrRedoVector gSmxlRedoVectors[];
extern smrRedoVector gSmeRedoVectors[];
extern smrRedoVector gSmdRedoVectors[];
extern smrRedoVector gSmnRedoVectors[];
extern smrRedoVector gSmqRedoVectors[];
extern smrRedoVector gSmgRedoVectors[];


/*
 * X$LATCH에 추가
 */
stlStatus smrLogStreamOpenLatch()
{
    return STL_SUCCESS;
}

stlStatus smrLogStreamGetNextLatch( knlLatch ** aLatch,
                                    stlChar   * aBuf,
                                    void      * aPrevPosition,
                                    knlEnv    * aEnv )
{
    stlUInt32    * sCurSeq = (stlUInt32*)aPrevPosition;
    smrLogStream * sLogStream;

    if( *aLatch == NULL)
    {
        *sCurSeq = 0;
    }
    else
    {
        *sCurSeq += 1;
    }

    if( (*sCurSeq / 2) < 1 )
    {
        sLogStream = &(gSmrWarmupEntry->mLogStream);
        if( (*sCurSeq % 2) == 0 )
        {
            *aLatch = &(sLogStream->mLogBuffer->mBufferLatch);
            stlSnprintf(aBuf,
                        KNL_LATCH_MAX_DESC_BUF_SIZE,
                        "Log Stream #%d (BUFFER)",
                        (*sCurSeq/2));
        }
        else
        {
            *aLatch = &(sLogStream->mFileLatch);
            stlSnprintf(aBuf,
                        KNL_LATCH_MAX_DESC_BUF_SIZE,
                        "Log Stream #%d (FILE)",
                        (*sCurSeq/2));
        }
    }
    else
    {
        *aLatch = NULL;
        aBuf[0] = '\0';
    }

    return STL_SUCCESS;
}

stlStatus smrLogStreamCloseLatch()
{
    return STL_SUCCESS;
}

knlLatchScanCallback gSmrLogStreamLatchScanCallback =
{
    smrLogStreamOpenLatch,
    smrLogStreamGetNextLatch,
    smrLogStreamCloseLatch
};

/*
 * X$PENDING_LOG_BUFFER
 */
knlFxColumnDesc gSmrPendingLogBufferColumnDesc[] =
{
    {
        "BUFFER_SIZE",
        "buffer size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxPendingLogBuffer, mBufferSize ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "BUFFER_BLOCK_COUNT",
        "buffer block count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxPendingLogBuffer, mBufferBlockCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "REAR_PBSN",
        "rear pbsn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxPendingLogBuffer, mRearPbsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "REAR_LSN",
        "rear lsn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxPendingLogBuffer, mRearLsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "FRONT_PBSN",
        "front pbsn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxPendingLogBuffer, mFrontPbsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "FRONT_LSN",
        "front lsn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxPendingLogBuffer, mFrontLsn ),
        STL_SIZEOF( stlInt64 ),
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

stlStatus smrOpenPendingLogBufferFxTableCallback( void   * aStmt,
                                                  void   * aDumpObjHandle,
                                                  void   * aPathCtrl,
                                                  knlEnv * aKnlEnv )
{
    stlInt64 * sBufferIdx;

    sBufferIdx = (stlInt64*)aPathCtrl;
    *sBufferIdx = 0;
    
    return STL_SUCCESS;
}

stlStatus smrClosePendingLogBufferFxTableCallback( void   * aDumpObjHandle,
                                                   void   * aPathCtrl,
                                                   knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus smrBuildRecordPendingLogBufferFxTableCallback( void              * aDumpObjHandle,
                                                         void              * aPathCtrl,
                                                         knlValueBlockList * aValueList,
                                                         stlInt32            aBlockIdx,
                                                         stlBool           * aTupleExist,
                                                         knlEnv            * aKnlEnv )
{
    smrFxPendingLogBuffer   sFxPendingLogBuffer;
    smrLogStream          * sLogStream;
    smrPendLogBuffer      * sPendingLogBuffer;
    stlInt64              * sBufferIdx;

    *aTupleExist = STL_FALSE;
    sBufferIdx = (stlInt64*)aPathCtrl;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    
    if( sLogStream->mPendBufferCount > *sBufferIdx )
    {
        sPendingLogBuffer = &(sLogStream->mPendLogBuffer[*sBufferIdx]);

        sFxPendingLogBuffer.mBufferSize = sPendingLogBuffer->mBufferSize;
        sFxPendingLogBuffer.mBufferBlockCount = sPendingLogBuffer->mBufferBlockCount;
        sFxPendingLogBuffer.mRearPbsn = sPendingLogBuffer->mRearPbsn;
        sFxPendingLogBuffer.mFrontPbsn = sPendingLogBuffer->mFrontPbsn;
        sFxPendingLogBuffer.mRearLsn = sPendingLogBuffer->mRearLsn;
        sFxPendingLogBuffer.mFrontLsn = sPendingLogBuffer->mFrontLsn;

        STL_TRY( knlBuildFxRecord( gSmrPendingLogBufferColumnDesc,
                                   &sFxPendingLogBuffer,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        *sBufferIdx += 1;
        *aTupleExist = STL_TRUE;
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmrPendingLogBufferTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    smrOpenPendingLogBufferFxTableCallback,
    smrClosePendingLogBufferFxTableCallback,
    smrBuildRecordPendingLogBufferFxTableCallback,
    STL_SIZEOF( stlInt64 ),
    "X$PENDING_LOG_BUFFER",
    "pending log buffer information",
    gSmrPendingLogBufferColumnDesc
};

/*
 * X$LOG_BUFFER
 */
knlFxColumnDesc gSmrLogBufferColumnDesc[] =
{
    {
        "BUFFER_SIZE",
        "buffer size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogBuffer, mBufferSize ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "BUFFER_BLOCK_COUNT",
        "buffer block count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogBuffer, mBufferBlockCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "BLOCK_OFFSET",
        "current block offset",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smrFxLogBuffer, mBlockOffset ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "FILE_SEQ_NO",
        "current file sequence number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogBuffer, mFileSeqNo ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "REAR_FILE_BLOCK_SEQ_NO",
        "rear file block sequence number",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smrFxLogBuffer, mRearFileBlockSeqNo ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "FRONT_FILE_BLOCK_SEQ_NO",
        "front file block sequence number",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smrFxLogBuffer, mFrontFileBlockSeqNo ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "LEFT_LOG_COUNT_IN_BLOCK",
        "left log coun in block",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smrFxLogBuffer, mLeftLogCountInBlock ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "REAR_SBSN",
        "rear sbsn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogBuffer, mRearSbsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "REAR_LSN",
        "rear lsn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogBuffer, mRearLsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "FRONT_SBSN",
        "front sbsn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogBuffer, mFrontSbsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "FRONT_LSN",
        "front lsn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogBuffer, mFrontLsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "REAR_LID_FILE_SEQ_NO",
        "rear log identifier(file sequence no)",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogBuffer, mRearLidFileSeqNo ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "REAR_LID_BLOCK_SEQ_NO",
        "rear log identifier(block sequence no)",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smrFxLogBuffer, mRearLidBlockSeqNo ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "REAR_LID_BLOCK_OFFSET",
        "rear log identifier(block offset)",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smrFxLogBuffer, mRearLidBlockOffset ),
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

stlStatus smrOpenLogBufferFxTableCallback( void   * aStmt,
                                           void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aKnlEnv )
{
    stlInt64 * sStarted;

    sStarted = (stlInt64*)aPathCtrl;
    *sStarted = STL_FALSE;
    
    return STL_SUCCESS;
}

stlStatus smrCloseLogBufferFxTableCallback( void   * aDumpObjHandle,
                                            void   * aPathCtrl,
                                            knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus smrBuildRecordLogBufferFxTableCallback( void              * aDumpObjHandle,
                                                  void              * aPathCtrl,
                                                  knlValueBlockList * aValueList,
                                                  stlInt32            aBlockIdx,
                                                  stlBool           * aTupleExist,
                                                  knlEnv            * aKnlEnv )
{
    smrFxLogBuffer   sFxLogBuffer;
    smrLogStream   * sLogStream;
    smrLogBuffer   * sLogBuffer;
    stlInt64       * sStarted;

    *aTupleExist = STL_FALSE;
    sStarted = (stlInt64*)aPathCtrl;

    if( *sStarted == STL_FALSE )
    {
        sLogStream = &gSmrWarmupEntry->mLogStream;
        sLogBuffer = sLogStream->mLogBuffer;

        sFxLogBuffer.mBufferSize = sLogBuffer->mBufferSize;
        sFxLogBuffer.mBufferBlockCount = sLogBuffer->mBufferBlockCount;
        sFxLogBuffer.mBlockOffset = sLogBuffer->mBlockOffset;
        sFxLogBuffer.mFileSeqNo = sLogBuffer->mFileSeqNo;
        sFxLogBuffer.mRearFileBlockSeqNo = sLogBuffer->mRearFileBlockSeqNo;
        sFxLogBuffer.mFrontFileBlockSeqNo = sLogBuffer->mFrontFileBlockSeqNo;
        sFxLogBuffer.mLeftLogCountInBlock = sLogBuffer->mLeftLogCountInBlock;
        sFxLogBuffer.mRearSbsn = sLogBuffer->mRearSbsn;
        sFxLogBuffer.mFrontSbsn = sLogBuffer->mFrontSbsn;
        sFxLogBuffer.mRearLsn = sLogBuffer->mRearLsn;
        sFxLogBuffer.mFrontLsn = sLogBuffer->mFrontLsn;
        sFxLogBuffer.mRearLidFileSeqNo = SMR_FILE_SEQ_NO(&sLogBuffer->mRearLid);
        sFxLogBuffer.mRearLidBlockSeqNo = SMR_BLOCK_SEQ_NO(&sLogBuffer->mRearLid);
        sFxLogBuffer.mRearLidBlockOffset = SMR_BLOCK_OFFSET(&sLogBuffer->mRearLid);

        STL_TRY( knlBuildFxRecord( gSmrLogBufferColumnDesc,
                                   &sFxLogBuffer,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        *sStarted = STL_TRUE;
        *aTupleExist = STL_TRUE;
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmrLogBufferTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    smrOpenLogBufferFxTableCallback,
    smrCloseLogBufferFxTableCallback,
    smrBuildRecordLogBufferFxTableCallback,
    STL_SIZEOF( stlInt64 ),
    "X$LOG_BUFFER",
    "log buffer information",
    gSmrLogBufferColumnDesc
};

/*
 * X$LOG_STREAM
 */
knlFxColumnDesc gSmrLogStreamColumnDesc[] =
{
    {
        "STATE",
        "log stream state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxLogStream, mState ),
        16,
        STL_FALSE  /* nullable */
    },
    {
        "BLOCK_SIZE",
        "log stream block size",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smrFxLogStream, mBlockSize ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "FILE_SEQ_NO",
        "log stream file sequence number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogStream, mCurFileSeqNo ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "GROUP_COUNT",
        "log stream group count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smrFxLogStream, mLogGroupCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "FLUSH_COUNT",
        "flush operation count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogStream, mFlushCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "FLUSH_TRY_COUNT",
        "flush try count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogStream, mFlushTryCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "WAIT_COUNT_BY_BUFFER_FULL",
        "wait count by buffer full",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogStream, mWaitCountByBufferFull ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "WAIT_COUNT_FOR_SYNC",
        "wait count for sync operation",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogStream, mWaitCount4Sync ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "BLOCKED_LOGGING_COUNT",
        "blocked loggnig count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogStream, mBlockedLoggingCount ),
        STL_SIZEOF( stlInt64 ),
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

stlStatus smrOpenLogStreamFxTableCallback( void   * aStmt,
                                           void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aKnlEnv )
{
    stlInt64 * sStarted;

    sStarted = (stlInt64*)aPathCtrl;
    *sStarted = STL_FALSE;
    
    return STL_SUCCESS;
}

stlStatus smrCloseLogStreamFxTableCallback( void   * aDumpObjHandle,
                                            void   * aPathCtrl,
                                            knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus smrBuildRecordLogStreamFxTableCallback( void              * aDumpObjHandle,
                                                  void              * aPathCtrl,
                                                  knlValueBlockList * aValueList,
                                                  stlInt32            aBlockIdx,
                                                  stlBool           * aTupleExist,
                                                  knlEnv            * aKnlEnv )
{
    smrFxLogStream    sFxLogStream;
    smrLogStream    * sLogStream;
    stlInt64        * sStarted;

    *aTupleExist = STL_FALSE;
    sStarted = (stlInt64*)aPathCtrl;

    if( *sStarted == STL_FALSE )
    {
        sLogStream = &gSmrWarmupEntry->mLogStream;
        sFxLogStream.mBlockSize     = sLogStream->mLogStreamPersData.mBlockSize;
        sFxLogStream.mCurFileSeqNo  = sLogStream->mLogStreamPersData.mCurFileSeqNo;
        sFxLogStream.mLogGroupCount = sLogStream->mLogStreamPersData.mLogGroupCount;
        sFxLogStream.mFlushCount = sLogStream->mStatistics.mFlushCount;
        sFxLogStream.mFlushTryCount = sLogStream->mStatistics.mFlushTryCount;
        sFxLogStream.mWaitCountByBufferFull = sLogStream->mStatistics.mWaitCountByBufferFull;
        sFxLogStream.mWaitCount4Sync = sLogStream->mStatistics.mWaitCount4Sync;
        sFxLogStream.mBlockedLoggingCount = sLogStream->mStatistics.mBlockedLoggingCount;

        switch( sLogStream->mLogStreamPersData.mState )
        {
            case SMR_LOG_STREAM_STATE_CREATING:
                stlStrncpy( sFxLogStream.mState, "CREATING", 16 );
                break;
            case SMR_LOG_STREAM_STATE_DROPPING:
                stlStrncpy( sFxLogStream.mState, "DROPPING", 16 );
                break;
            case SMR_LOG_STREAM_STATE_UNUSED:
                stlStrncpy( sFxLogStream.mState, "UNUSED", 16 );
                break;
            case SMR_LOG_STREAM_STATE_ACTIVE:
                stlStrncpy( sFxLogStream.mState, "ACTIVE", 16 );
                break;
            default:
                stlStrncpy( sFxLogStream.mState, "UNKNOWN", 16 );
                break;
        }
        
        STL_TRY( knlBuildFxRecord( gSmrLogStreamColumnDesc,
                                   &sFxLogStream,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );
        
        *aTupleExist = STL_TRUE;
        *sStarted = STL_TRUE;
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmrLogStreamTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    smrOpenLogStreamFxTableCallback,
    smrCloseLogStreamFxTableCallback,
    smrBuildRecordLogStreamFxTableCallback,
    STL_SIZEOF( stlInt64 ),
    "X$LOG_STREAM",
    "log stream information",
    gSmrLogStreamColumnDesc
};

/*
 * X$LOG_GROUP
 */
knlFxColumnDesc gSmrLogGroupColumnDesc[] =
{
    {
        "GROUP_ID",
        "log group id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smrFxLogGroup, mLogGroupId ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "GROUP_ORD_ID",
        "ordinal group id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smrFxLogGroup, mOrdLogGroupId ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "STATE",
        "log group state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxLogGroup, mState ),
        16,
        STL_FALSE  /* nullable */
    },
    {
        "FILE_SEQ_NO",
        "log group file sequence number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogGroup, mFileSeqNo ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "FILE_SIZE",
        "log group file size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogGroup, mFileSize ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "PREV_LSN",
        "log group file size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogGroup, mPrevLastLsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "SWITCHING_SBSN",
        "switching SBSN",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogGroup, mLogSwitchingSbsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "MEMBER_COUNT",
        "log group member count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smrFxLogGroup, mLogMemberCount ),
        STL_SIZEOF( stlInt32 ),
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

stlStatus smrOpenLogGroupFxTableCallback( void   * aStmt,
                                          void   * aDumpObjHandle,
                                          void   * aPathCtrl,
                                          knlEnv * aKnlEnv )
{
    smrFxLogGroupPathCtrl * sPathCtrl;

    sPathCtrl = (smrFxLogGroupPathCtrl*)aPathCtrl;
    sPathCtrl->mLogGroupIterator = NULL;
    sPathCtrl->mOrdLogGroupId = 0;
    
    return STL_SUCCESS;
}

stlStatus smrCloseLogGroupFxTableCallback( void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus smrBuildRecordLogGroupFxTableCallback( void              * aDumpObjHandle,
                                                 void              * aPathCtrl,
                                                 knlValueBlockList * aValueList,
                                                 stlInt32            aBlockIdx,
                                                 stlBool           * aTupleExist,
                                                 knlEnv            * aKnlEnv )
{
    smrFxLogGroupPathCtrl * sPathCtrl;
    smrFxLogGroup           sFxLogGroup;

    sPathCtrl = (smrFxLogGroupPathCtrl*)aPathCtrl;
    *aTupleExist = STL_FALSE;

    if( sPathCtrl->mLogGroupIterator == NULL )
    {
        STL_TRY( smrFirstLogGroupPersData( &gSmrWarmupEntry->mLogStream,
                                           &sPathCtrl->mLogGroupPersData,
                                           &sPathCtrl->mLogGroupIterator,
                                           SML_ENV( aKnlEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smrNextLogGroupPersData( &gSmrWarmupEntry->mLogStream,
                                          &sPathCtrl->mLogGroupPersData,
                                          &sPathCtrl->mLogGroupIterator,
                                          SML_ENV( aKnlEnv ) )
                 == STL_SUCCESS );
    }
    
    if( sPathCtrl->mLogGroupIterator != NULL )
    {
        sFxLogGroup.mLogGroupId     = sPathCtrl->mLogGroupPersData.mLogGroupId;
        sFxLogGroup.mOrdLogGroupId  = sPathCtrl->mOrdLogGroupId;
        sFxLogGroup.mFileSeqNo      = sPathCtrl->mLogGroupPersData.mFileSeqNo;
        sFxLogGroup.mFileSize       = sPathCtrl->mLogGroupPersData.mFileSize;
        sFxLogGroup.mPrevLastLsn    = sPathCtrl->mLogGroupPersData.mPrevLastLsn;
        sFxLogGroup.mLogMemberCount = sPathCtrl->mLogGroupPersData.mLogMemberCount;
        sFxLogGroup.mLogSwitchingSbsn = ((smrLogGroup*)sPathCtrl->mLogGroupIterator)->mLogSwitchingSbsn;

        switch( sPathCtrl->mLogGroupPersData.mState )
        {
            case SMR_LOG_GROUP_STATE_CREATING:
                stlStrncpy( sFxLogGroup.mState, "CREATING", 16 );
                break;
            case SMR_LOG_GROUP_STATE_DROPPING:
                stlStrncpy( sFxLogGroup.mState, "DROPPING", 16 );
                break;
            case SMR_LOG_GROUP_STATE_UNUSED:
                stlStrncpy( sFxLogGroup.mState, "UNUSED", 16 );
                break;
            case SMR_LOG_GROUP_STATE_ACTIVE:
                stlStrncpy( sFxLogGroup.mState, "ACTIVE", 16 );
                break;
            case SMR_LOG_GROUP_STATE_CURRENT:
                stlStrncpy( sFxLogGroup.mState, "CURRENT", 16 );
                break;
            case SMR_LOG_GROUP_STATE_INACTIVE:
                stlStrncpy( sFxLogGroup.mState, "INACTIVE", 16 );
                break;
            default:
                stlStrncpy( sFxLogGroup.mState, "UNKNOWN", 16 );
                break;
        }
        
        STL_TRY( knlBuildFxRecord( gSmrLogGroupColumnDesc,
                                   &sFxLogGroup,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );
        
        sPathCtrl->mOrdLogGroupId++;
        *aTupleExist = STL_TRUE;
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmrLogGroupTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    smrOpenLogGroupFxTableCallback,
    smrCloseLogGroupFxTableCallback,
    smrBuildRecordLogGroupFxTableCallback,
    STL_SIZEOF( smrFxLogGroupPathCtrl ),
    "X$LOG_GROUP",
    "log group information",
    gSmrLogGroupColumnDesc
};

/*
 * X$LOG_MEMBER
 */
knlFxColumnDesc gSmrLogMemberColumnDesc[] =
{
    {
        "GROUP_ID",
        "log group id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smrFxLogMember, mLogGroupId ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "NAME",
        "log member",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxLogMember, mName ),
        STL_MAX_FILE_PATH_LENGTH,
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

stlStatus smrOpenLogMemberFxTableCallback( void   * aStmt,
                                           void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aKnlEnv )
{
    smrFxLogMemberPathCtrl * sPathCtrl;

    sPathCtrl = (smrFxLogMemberPathCtrl*)aPathCtrl;

    sPathCtrl->mLogGroupIterator = NULL;
    sPathCtrl->mLogMemberIterator = NULL;
    
    return STL_SUCCESS;
}

stlStatus smrCloseLogMemberFxTableCallback( void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus smrBuildRecordLogMemberFxTableCallback( void              * aDumpObjHandle,
                                                  void              * aPathCtrl,
                                                  knlValueBlockList * aValueList,
                                                  stlInt32            aBlockIdx,
                                                  stlBool           * aTupleExist,
                                                  knlEnv            * aKnlEnv )
{
    smrFxLogMemberPathCtrl * sPathCtrl;
    smrFxLogMember           sFxLogMember;

    sPathCtrl = (smrFxLogMemberPathCtrl*)aPathCtrl;

    STL_RAMP( RAMP_RETRY );

    *aTupleExist = STL_FALSE;
    
    if( sPathCtrl->mLogGroupIterator == NULL )
    {
        STL_TRY( smrFirstLogGroupPersData( &gSmrWarmupEntry->mLogStream,
                                           &sPathCtrl->mLogGroupPersData,
                                           &sPathCtrl->mLogGroupIterator,
                                           SML_ENV( aKnlEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        if( sPathCtrl->mLogMemberIterator == NULL )
        {
            STL_TRY( smrNextLogGroupPersData( &gSmrWarmupEntry->mLogStream,
                                              &sPathCtrl->mLogGroupPersData,
                                              &sPathCtrl->mLogGroupIterator,
                                              SML_ENV( aKnlEnv ) )
                     == STL_SUCCESS );
        }
    }
    
    STL_TRY_THROW( sPathCtrl->mLogGroupIterator != NULL, RAMP_SUCCESS );

    if( sPathCtrl->mLogMemberIterator == NULL )
    {
        STL_TRY( smrFirstLogMemberPersData( sPathCtrl->mLogGroupIterator,
                                            &sPathCtrl->mLogMemberPersData,
                                            &sPathCtrl->mLogMemberIterator,
                                            SML_ENV( aKnlEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smrNextLogMemberPersData( sPathCtrl->mLogGroupIterator,
                                           &sPathCtrl->mLogMemberPersData,
                                           &sPathCtrl->mLogMemberIterator,
                                           SML_ENV( aKnlEnv ) )
                 == STL_SUCCESS );
    }

    STL_TRY_THROW( sPathCtrl->mLogMemberIterator != NULL, RAMP_RETRY );
    
    sFxLogMember.mLogGroupId = sPathCtrl->mLogGroupPersData.mLogGroupId;
    stlStrncpy( sFxLogMember.mName,
                sPathCtrl->mLogMemberPersData.mName,
                STL_MAX_FILE_PATH_LENGTH );

    STL_TRY( knlBuildFxRecord( gSmrLogMemberColumnDesc,
                               &sFxLogMember,
                               aValueList,
                               aBlockIdx,
                               aKnlEnv )
             == STL_SUCCESS );
    *aTupleExist = STL_TRUE;

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmrLogMemberTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    smrOpenLogMemberFxTableCallback,
    smrCloseLogMemberFxTableCallback,
    smrBuildRecordLogMemberFxTableCallback,
    STL_SIZEOF( smrFxLogMemberPathCtrl ),
    "X$LOG_MEMBER",
    "log member information",
    gSmrLogMemberColumnDesc
};

/*
 * D$LOG_BLOCK
 */
knlFxColumnDesc gSmrLogBlockColumnDesc[] =
{
    {
        "BLOCK_SEQ",
        "block sequence number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogBlock, mBlockSeq ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "LSN",
        "log sequence number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLogBlock, mLsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "LOG_COUNT",
        "log count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smrFxLogBlock, mLogCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "WRAP_NO",
        "wrap number",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smrFxLogBlock, mWrapNo ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "CHAINED",
        "chained block",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smrFxLogBlock, mChained ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "FILE_END",
        "file end",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smrFxLogBlock, mFileEnd ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "FIRST_OFFSET",
        "first log piece offset",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smrFxLogBlock, mFirstLogOffset ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "UNUSED_SPACE",
        "unused space in block",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smrFxLogBlock, mUnusedSpace ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "HEX_BODY",
        "log block body",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxLogBlock, mHexBody ),
        SMR_DUMP_LOG_BODY_STRING_SIZE,
        STL_FALSE  /* nullable */
    },
    {
        "STR_BODY",
        "log block body",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxLogBlock, mStrBody ),
        SMR_DUMP_LOG_BODY_STRING_SIZE,
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

stlStatus smrGetDumpObjectCallback( stlInt64   aTransID,
                                    void     * aStmt,
                                    stlChar  * aDumpObjName,
                                    void    ** aDumpObjHandle,
                                    knlEnv   * aEnv )
{
    /**
     * aDumpObjName은 Statement 에서 할당된 메모리여야 한다.
     */
    
    *aDumpObjHandle = aDumpObjName;
    
    return STL_SUCCESS;
}

stlStatus smrOpenLogCallback( void   * aStmt,
                              void   * aDumpObjHandle,
                              void   * aPathCtrl,
                              knlEnv * aEnv )
{
    smrFxLogPathCtrl * sPathCtrl;
    stlInt32           sNameLen;
    smrLogFileHdr      sLogFileHdr;
    stlOffset          sOffset;
    stlChar            sFilePath[STL_MAX_FILE_PATH_LENGTH];
    stlChar            sLogDir[STL_MAX_FILE_PATH_LENGTH];
    smrLogBlockHdr     sLogBlockHdr;

    sPathCtrl = (smrFxLogPathCtrl*) aPathCtrl;
    
    sPathCtrl->mFileBlockSeq = 0;
    sPathCtrl->mCurBlockSeq = -1;
    sPathCtrl->mBufferBlockSeq = -1;
    sPathCtrl->mBufferBlockCount = 0;
    sPathCtrl->mBlockOffset = 0;
    sPathCtrl->mLeftLogCount = 0;
    sPathCtrl->mLeftPieceCount = 0;
    sPathCtrl->mCurLogSeq = -1;
    sPathCtrl->mCurPieceSeq = 0;
    sPathCtrl->mState = 0;

    STL_TRY( knlGetPropertyValueByName( "LOG_DIR",
                                        sLogDir,
                                        KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlMergeAbsFilePath( sLogDir,
                                  aDumpObjHandle,
                                  sFilePath,
                                  STL_MAX_FILE_PATH_LENGTH,
                                  &sNameLen,
                                  KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlOpenFile( &sPathCtrl->mFile,
                          sFilePath,
                          STL_FOPEN_READ  |
                          STL_FOPEN_LARGEFILE,
                          STL_FPERM_OS_DEFAULT,
                          KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    sPathCtrl->mState = 1;

    /**
     * Reading File Header
     */
    STL_TRY( stlReadFile( &sPathCtrl->mFile,
                          &sLogFileHdr,
                          STL_SIZEOF( smrLogFileHdr ),
                          NULL,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( 0, sLogFileHdr.mBlockSize );
    
    STL_TRY( stlSeekFile( &sPathCtrl->mFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    sPathCtrl->mBlockSize = sLogFileHdr.mBlockSize;
    sPathCtrl->mMaxBlockCount = sLogFileHdr.mMaxBlockCount - 1;

    STL_TRY( smgAllocSessShmMem( SMR_BLOCK_SIZE * (SMR_DUMP_LOG_BUFFER_BLOCK_COUNT + 1),
                                 (void*)&sPathCtrl->mBuffer,
                                 SML_ENV(aEnv) )
             == STL_SUCCESS );
    sPathCtrl->mState = 2;

    /**
     * Setting WrapNo
     */
    STL_TRY( stlReadFile( &sPathCtrl->mFile,
                          (void*)sPathCtrl->mBuffer,
                          SMR_BLOCK_SIZE,
                          NULL,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, sPathCtrl->mBuffer );
    
    sPathCtrl->mWrapNo = SMR_BLOCK_WRAP_NO( &sLogBlockHdr );

    STL_ASSERT( sPathCtrl->mBlockSize == SMR_BLOCK_SIZE );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrCloseLogCallback( void   * aDumpObjHandle,
                               void   * aPathCtrl,
                               knlEnv * aEnv )
{
    smrFxLogPathCtrl * sPathCtrl;

    sPathCtrl = (smrFxLogPathCtrl*) aPathCtrl;

    switch( sPathCtrl->mState )
    {
        case 2:
            sPathCtrl->mState = 1;
            STL_TRY( smgFreeSessShmMem( sPathCtrl->mBuffer,
                                        SML_ENV(aEnv) )
                     == STL_SUCCESS );
        case 1:
            sPathCtrl->mState = 0;
            STL_TRY( stlCloseFile( &sPathCtrl->mFile,
                                   KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
        default:
            break;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sPathCtrl->mState )
    {
        case 2:
            (void)smgFreeSessShmMem( sPathCtrl->mBuffer,
                                     SML_ENV(aEnv) );
        case 1:
            (void)stlCloseFile( &sPathCtrl->mFile, KNL_ERROR_STACK(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

stlBool smrIsValidBlock( smrFxLogPathCtrl * aPathCtrl,
                         smrLogBlockHdr   * aLogBlockHdr )
{
    stlBool sValid = STL_TRUE;
    
    if( aLogBlockHdr->mLsn < 0 )
    {
        sValid = STL_FALSE;
        STL_THROW( RAMP_RETURN );
    }

    if( SMR_BLOCK_WRAP_NO( aLogBlockHdr ) != (aPathCtrl->mWrapNo) )
    {
        sValid = STL_FALSE;
        STL_THROW( RAMP_RETURN );
    }

    STL_RAMP( RAMP_RETURN );
    
    return sValid;
}

stlStatus smrReadBlockForDump( smrFxLogPathCtrl  * aPathCtrl,
                               stlChar          ** aLogBlock,
                               stlBool           * aEndOfFile,
                               smlEnv            * aEnv )
{
    stlOffset       sOffset;
    stlInt32        sBlockCount = 0;
    smrLogBlockHdr  sLogBlockHdr;
    
    *aEndOfFile = STL_TRUE;
    
    aPathCtrl->mBufferBlockSeq++;
    aPathCtrl->mCurBlockSeq++;
    
    if( aPathCtrl->mBufferBlockSeq >= aPathCtrl->mBufferBlockCount )
    {
        STL_TRY_THROW( aPathCtrl->mFileBlockSeq < aPathCtrl->mMaxBlockCount, RAMP_FINISH );
        
        sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( aPathCtrl->mFileBlockSeq,
                                             aPathCtrl->mBlockSize );
        
        STL_TRY( stlSeekFile( &aPathCtrl->mFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
        
        sBlockCount = STL_MIN( aPathCtrl->mMaxBlockCount -
                               aPathCtrl->mFileBlockSeq,
                               SMR_DUMP_LOG_BUFFER_BLOCK_COUNT );

        STL_TRY_THROW( sBlockCount > 0, RAMP_FINISH );
            
        STL_TRY( stlReadFile( &aPathCtrl->mFile,
                              (void*)aPathCtrl->mBuffer,
                              aPathCtrl->mBlockSize * sBlockCount,
                              NULL,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        aPathCtrl->mFileBlockSeq += sBlockCount;
        aPathCtrl->mBufferBlockCount = sBlockCount;
        aPathCtrl->mBufferBlockSeq = 0;
    }
    
    *aLogBlock = (aPathCtrl->mBuffer + (aPathCtrl->mBufferBlockSeq *
                                        aPathCtrl->mBlockSize));

    SMR_READ_LOG_BLOCK_HDR( &sLogBlockHdr, *aLogBlock );

    /**
     * Last file block일 경우 dummy log block일 수 있으므로
     * valid check를 skip한다.
     */
    if( SMR_IS_LAST_FILE_BLOCK( &sLogBlockHdr ) != STL_TRUE )
    {
        STL_TRY_THROW( smrIsValidBlock( aPathCtrl,
                                        &sLogBlockHdr )
                       == STL_TRUE, RAMP_FINISH );
    }

    *aEndOfFile = STL_FALSE;
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrBuildRecordLogBlockCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aTupleExist,
                                          knlEnv            * aEnv )
{
    stlChar          * sLogBlock;
    stlBool            sEndOfFile;
    smrFxLogPathCtrl * sPathCtrl;
    smrFxLogBlock      sFxRecord;
    smrLogBlockHdr     sLogBlockHdr;
    stlInt32           sOffset = 0;
    stlInt32           sUsedSpace = 0;
    smrLogHdr          sLogHdr;
    stlInt16           sLogCount;

    *aTupleExist = STL_FALSE;

    sPathCtrl = (smrFxLogPathCtrl*) aPathCtrl;
    
    STL_TRY( smrReadBlockForDump( sPathCtrl,
                                  &sLogBlock,
                                  &sEndOfFile,
                                  SML_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY_THROW( sEndOfFile == STL_FALSE, RAMP_FINISH );
    
    SMR_READ_MOVE_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock, sOffset );

    sFxRecord.mBlockSeq = sPathCtrl->mCurBlockSeq;
    sFxRecord.mLsn = sLogBlockHdr.mLsn;
    sFxRecord.mLogCount = sLogBlockHdr.mLogCount;
    sFxRecord.mWrapNo = SMR_BLOCK_WRAP_NO( &sLogBlockHdr );
    sFxRecord.mFileEnd = SMR_IS_LAST_FILE_BLOCK( &sLogBlockHdr );
    sFxRecord.mFirstLogOffset = sLogBlockHdr.mFirstLogOffset;
    sFxRecord.mChained = SMR_IS_CHAINED_BLOCK( &sLogBlockHdr );

    sUsedSpace = sLogBlockHdr.mFirstLogOffset;
    sLogCount = sFxRecord.mLogCount;
    
    while( sLogCount > 0 )
    {
        SMR_READ_LOG_HDR( &sLogHdr, ((stlChar*)sLogBlock) + sUsedSpace );
        sUsedSpace += ( sLogHdr.mSize + SMR_LOG_HDR_SIZE );
        sLogCount--;
    }

    if( sUsedSpace >= sPathCtrl->mBlockSize )
    {
        sFxRecord.mUnusedSpace = 0;
    }
    else
    {
        sFxRecord.mUnusedSpace = sPathCtrl->mBlockSize - sUsedSpace;
    }

    STL_TRY( stlBinary2HexString( (stlUInt8*)(sLogBlock + sOffset),
                                   sPathCtrl->mBlockSize - sOffset,
                                   sFxRecord.mHexBody,
                                   SMR_DUMP_LOG_BODY_STRING_SIZE,
                                   KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    STL_TRY( stlBinary2CharString( (stlUInt8*)(sLogBlock + sOffset),
                                   sPathCtrl->mBlockSize - sOffset,
                                   sFxRecord.mStrBody,
                                   SMR_DUMP_LOG_BODY_STRING_SIZE,
                                   KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    STL_TRY( knlBuildFxRecord( gSmrLogBlockColumnDesc,
                               &sFxRecord,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;

    STL_RAMP( RAMP_FINISH );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmrLogBlockTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    smrGetDumpObjectCallback,
    smrOpenLogCallback,
    smrCloseLogCallback,
    smrBuildRecordLogBlockCallback,
    STL_SIZEOF( smrFxLogPathCtrl ),
    "D$LOG_BLOCK",
    "log block",
    gSmrLogBlockColumnDesc
};

static stlChar * smrLogTypeString( smgComponentClass aClass, stlUInt16 aType )
{
    stlChar * sString = "UNKNOWN";

    switch( aClass )
    {
        case SMG_COMPONENT_DATAFILE :
            if( aType < SMR_DATAFILE_LOG_MAX_COUNT )
            {
                sString = gSmfRedoVectors[aType].mLogName;
            }
            break;
        case SMG_COMPONENT_PAGE_ACCESS :
            if( aType < SMR_PAGE_ACCESS_LOG_MAX_COUNT )
            {
                sString = gSmpRedoVectors[aType].mLogName;
            }
            break;
        case SMG_COMPONENT_TABLESPACE :
            if( aType < SMR_TABLESPACE_LOG_MAX_COUNT )
            {
                sString = gSmtRedoVectors[aType].mLogName;
            }
            break;
        case SMG_COMPONENT_RECOVERY :
            if( aType < SMR_RECOVERY_LOG_MAX_COUNT )
            {
                sString = gSmrRedoVectors[aType].mLogName;
            }
            break;
        case SMG_COMPONENT_SEGMENT :
            if( aType < SMR_SEGMENT_LOG_MAX_COUNT )
            {
                sString = gSmsRedoVectors[aType].mLogName;
            }
            break;
        case SMG_COMPONENT_TRANSACTION :
            if( aType < SMR_TRANSACTION_LOG_MAX_COUNT )
            {
                sString = gSmxlRedoVectors[aType].mLogName;
            }
            break;
        case SMG_COMPONENT_RELATION :
            if( aType < SMR_RELATION_LOG_MAX_COUNT )
            {
                sString = gSmeRedoVectors[aType].mLogName;
            }
            break;
        case SMG_COMPONENT_TABLE :
            if( aType < SMR_TABLE_LOG_MAX_COUNT )
            {
                sString = gSmdRedoVectors[aType].mLogName;
            }
            break;
        case SMG_COMPONENT_INDEX :
            if( aType < SMR_INDEX_LOG_MAX_COUNT )
            {
                sString = gSmnRedoVectors[aType].mLogName;
            }
            break;
        case SMG_COMPONENT_SEQUENCE :
            if( aType < SMR_SEQUENCE_LOG_MAX_COUNT )
            {
                sString = gSmqRedoVectors[aType].mLogName;
            }
            break;
        case SMG_COMPONENT_EXTERNAL :
            if( aType < SML_SUPPL_LOG_MAX )
            {
                sString = gSmgRedoVectors[aType].mLogName;
            }
            break;
        case SMG_COMPONENT_TEST :
        case SMG_COMPONENT_GENERAL :
        default :
            break;
    }

    return sString;
}

static stlChar * smrGetComponentClassString( smgComponentClass aClass )
{
    stlChar * sString = "UNKNOWN";

    switch( aClass )
    {
        case SMG_COMPONENT_DATAFILE :
            sString = "DATAFILE";
            break;
        case SMG_COMPONENT_PAGE_ACCESS :
            sString = "PAGE_ACCESS";
            break;
        case SMG_COMPONENT_TABLESPACE :
            sString = "TABLESPACE";
            break;
        case SMG_COMPONENT_GENERAL :
            sString = "GENERAL";
            break;
        case SMG_COMPONENT_RECOVERY :
            sString = "RECOVERY";
            break;
        case SMG_COMPONENT_SEGMENT :
            sString = "SEGMENT";
            break;
        case SMG_COMPONENT_TRANSACTION :
            sString = "TRANSACTION";
            break;
        case SMG_COMPONENT_RELATION :
            sString = "ACCESS";
            break;
        case SMG_COMPONENT_TABLE :
            sString = "TABLE";
            break;
        case SMG_COMPONENT_INDEX :
            sString = "INDEX";
            break;
        case SMG_COMPONENT_SEQUENCE :
            sString = "SEQUENCE";
            break;
        case SMG_COMPONENT_EXTERNAL :
            sString = "EXTERNAL";
            break;
        case SMG_COMPONENT_TEST :
            sString = "TEST";
            break;
        default :
            break;
    }

    return sString;
}

static stlChar * smrGetRedoTargetTypeString( stlChar aRedoType )
{
    stlChar * sString = "UNKNOWN";

    switch( aRedoType )
    {
        case SMR_REDO_TARGET_PAGE :
            sString = "PAGE";
            break;
        case SMR_REDO_TARGET_UNDO_SEGMENT :
            sString = "UNDO";
            break;
        case SMR_REDO_TARGET_TRANSACTION :
            sString = "TRANSACTION";
            break;
        case SMR_REDO_TARGET_CTRL :
            sString = "CONTROL_FILE";
            break;
        case SMR_REDO_TARGET_NONE :
            sString = "NONE";
            break;
        case SMR_REDO_TARGET_TEST :
            sString = "TEST";
            break;
        default :
            break;
    }

    return sString;
}

/*
 * D$LOG
 */
knlFxColumnDesc gSmrLogColumnDesc[] =
{
    {
        "LOG_SEQ",
        "log sequence number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLog, mLogSeq ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "PIECE_SEQ",
        "piece sequence number",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smrFxLog, mPieceSeq ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "LSN",
        "log sequence number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLog, mLsn ),
        STL_SIZEOF( smrLsn ),
        STL_FALSE  /* nullable */
    },
    {
        "TRANS_ID",
        "transaction identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLog, mTransId ),
        STL_SIZEOF( smxlTransId ),
        STL_FALSE  /* nullable */
    },
    {
        "BLOCK_SEQ",
        "block sequece belong to log header",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxLog, mBlockSeq ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "ROW_TBS_ID",
        "tablespace ideintifer of target record",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smrFxLog, mRowId.mTbsId ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "ROW_PAGE_ID",
        "page ideintifer of target record",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smrFxLog, mRowId.mPageId ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "ROW_OFFSET",
        "page offset of target record",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smrFxLog, mRowId.mOffset ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "TYPE",
        "log type",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxLog, mType ),
        64,
        STL_FALSE  /* nullable */
    },
    {
        "SIZE",
        "log piece size",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smrFxLog, mSize ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "TARGET",
        "redo target type",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxLog, mTarget ),
        64,
        STL_FALSE  /* nullable */
    },
    {
        "CLASS",
        "component class",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxLog, mClass ),
        64,
        STL_FALSE  /* nullable */
    },
    {
        "PROPAGATE_LOG",
        "propagate log",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smrFxLog, mPropagateLog ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "HEX_BODY",
        "log block body",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxLog, mHexBody ),
        SMR_DUMP_LOG_BODY_STRING_SIZE,
        STL_FALSE  /* nullable */
    },
    {
        "STR_BODY",
        "log block body",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxLog, mStrBody ),
        SMR_DUMP_LOG_BODY_STRING_SIZE,
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

stlStatus smrBuildRecordLogCallback( void              * aDumpObjHandle,
                                     void              * aPathCtrl,
                                     knlValueBlockList * aValueList,
                                     stlInt32            aBlockIdx,
                                     stlBool           * aTupleExist,
                                     knlEnv            * aEnv )
{
    stlChar          * sLogBlock;
    stlBool            sEndOfFile;
    smrFxLogPathCtrl * sPathCtrl;
    smrFxLog           sFxRecord;
    smrLogBlockHdr     sLogBlockHdr;
    smrLogPieceHdr     sLogPieceHdr;
    stlUInt32          sOffset = 0;
    stlUInt32          sTotalReadBytes;
    stlUInt32          sReadBytes;
    stlInt32           sBlockOffset = 0;

    *aTupleExist = STL_FALSE;

    sPathCtrl = (smrFxLogPathCtrl*) aPathCtrl;

    if( sPathCtrl->mLeftPieceCount == 0 )
    {
        if( sPathCtrl->mLeftLogCount == 0 )
        {
            STL_TRY( smrReadBlockForDump( sPathCtrl,
                                          &sLogBlock,
                                          &sEndOfFile,
                                          SML_ENV(aEnv) ) == STL_SUCCESS );

            STL_TRY_THROW( sEndOfFile == STL_FALSE, RAMP_FINISH );
    
            SMR_READ_MOVE_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock, sOffset );

            sBlockOffset = sLogBlockHdr.mFirstLogOffset;
            sPathCtrl->mLeftLogCount = sLogBlockHdr.mLogCount;
        }
        else
        {
            sLogBlock = (sPathCtrl->mBuffer + (sPathCtrl->mBufferBlockSeq *
                                               sPathCtrl->mBlockSize));
            sBlockOffset = sPathCtrl->mBlockOffset;
        }
        
        SMR_READ_LOG_HDR( &sPathCtrl->mLogHdr, sLogBlock + sBlockOffset );
        sBlockOffset += SMR_LOG_HDR_SIZE;
        sPathCtrl->mLeftPieceCount = sPathCtrl->mLogHdr.mLogPieceCount;
        sPathCtrl->mLeftLogCount--;
        sPathCtrl->mLogHdrBlockSeq = sPathCtrl->mCurBlockSeq;
        
        /**
         * Log Body 구성
         */

        if( sPathCtrl->mLogHdr.mSize > (sPathCtrl->mBlockSize - sBlockOffset) )
        {
            sReadBytes = sPathCtrl->mBlockSize - sBlockOffset;
        }
        else
        {
            sReadBytes = sPathCtrl->mLogHdr.mSize;
        }

        /**
         * 동시성 문제로 partial wirte된 Log Hdr를 읽을수 있다.
         */
        STL_TRY_THROW( sReadBytes <= SMR_MAX_LOGBODY_SIZE, RAMP_ERR_INTERNAL );
            
        STL_READ_MOVE_BYTES( sPathCtrl->mLogBody,
                             sLogBlock,
                             sReadBytes,
                             sBlockOffset );
        
        sTotalReadBytes = sReadBytes;
        
        while( sTotalReadBytes < sPathCtrl->mLogHdr.mSize )
        {
            sBlockOffset = 0;
            STL_TRY( smrReadBlockForDump( sPathCtrl,
                                          &sLogBlock,
                                          &sEndOfFile,
                                          SML_ENV(aEnv) ) == STL_SUCCESS );

            STL_TRY_THROW( sEndOfFile == STL_FALSE, RAMP_FINISH );
    
            SMR_READ_MOVE_LOG_BLOCK_HDR( &sLogBlockHdr, sLogBlock, sBlockOffset );

            if( (sPathCtrl->mLogHdr.mSize - sTotalReadBytes) >
                (sPathCtrl->mBlockSize - sBlockOffset) )
            {
                sReadBytes = sPathCtrl->mBlockSize - sBlockOffset;
            }
            else
            {
                sReadBytes = (sPathCtrl->mLogHdr.mSize - sTotalReadBytes);
            }

            /**
             * 동시성 문제로 partial wirte된 Log Hdr를 읽을수 있다.
             */
            STL_TRY_THROW( ((stlUInt64)sTotalReadBytes + sReadBytes) <= SMR_MAX_LOGBODY_SIZE,
                           RAMP_ERR_INTERNAL );
            
            STL_READ_MOVE_BYTES( sPathCtrl->mLogBody + sTotalReadBytes,
                                 sLogBlock,
                                 sReadBytes,
                                 sBlockOffset );
            
            sTotalReadBytes += sReadBytes;
            sPathCtrl->mLeftLogCount = sLogBlockHdr.mLogCount;
        }

        sPathCtrl->mLogBodyOffset = 0;
        sPathCtrl->mBlockOffset = sBlockOffset;
        sPathCtrl->mCurLogSeq++;
        sPathCtrl->mCurPieceSeq = 0;
    }

    SMR_READ_MOVE_LOGPIECE_HDR( &sLogPieceHdr,
                                sPathCtrl->mLogBody,
                                sPathCtrl->mLogBodyOffset );

    sFxRecord.mLogSeq = sPathCtrl->mCurLogSeq;
    sFxRecord.mPieceSeq = sPathCtrl->mCurPieceSeq;
    sFxRecord.mLsn = sPathCtrl->mLogHdr.mLsn;
    sFxRecord.mTransId = sPathCtrl->mLogHdr.mTransId;
    sFxRecord.mSize = sLogPieceHdr.mSize;
    sFxRecord.mBlockSeq = sPathCtrl->mLogHdrBlockSeq;
    stlMemcpy( &sFxRecord.mRowId, &sLogPieceHdr.mDataRid, STL_SIZEOF(smlRid) );

    stlStrncpy( sFxRecord.mType,
                smrLogTypeString(sLogPieceHdr.mComponentClass,
                                 sLogPieceHdr.mType),
                64 );
    stlStrncpy( sFxRecord.mTarget,
                smrGetRedoTargetTypeString(sLogPieceHdr.mRedoTargetType),
                64 );
    stlStrncpy( sFxRecord.mClass,
                smrGetComponentClassString(sLogPieceHdr.mComponentClass),
                64 );

    sFxRecord.mPropagateLog = sLogPieceHdr.mPropagateLog;

    STL_TRY( stlBinary2HexString( (stlUInt8*)(sPathCtrl->mLogBody + sPathCtrl->mLogBodyOffset),
                                   sLogPieceHdr.mSize,
                                   sFxRecord.mHexBody,
                                   SMR_DUMP_LOG_BODY_STRING_SIZE,
                                   KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    STL_TRY( stlBinary2CharString( (stlUInt8*)(sPathCtrl->mLogBody + sPathCtrl->mLogBodyOffset),
                                   sLogPieceHdr.mSize,
                                   sFxRecord.mStrBody,
                                   SMR_DUMP_LOG_BODY_STRING_SIZE,
                                   KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    sPathCtrl->mLogBodyOffset += sLogPieceHdr.mSize;
    sPathCtrl->mLeftPieceCount--;
    sPathCtrl->mCurPieceSeq++;
    
    STL_TRY( knlBuildFxRecord( gSmrLogColumnDesc,
                               &sFxRecord,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;

    STL_RAMP( RAMP_FINISH );
        
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INTERNAL,
                      "- smrBuildRecordLogCallback()",
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmrLogTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    smrGetDumpObjectCallback,
    smrOpenLogCallback,
    smrCloseLogCallback,
    smrBuildRecordLogCallback,
    STL_SIZEOF( smrFxLogPathCtrl ),
    "D$LOG",
    "log",
    gSmrLogColumnDesc
};

/*
 * X$ARCHIVELOG
 */
knlFxColumnDesc gSmrArchivelogColumnDesc[] =
{
    {
        "ARCHIVELOG_MODE",
        "archive log mode",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxArchivelog, mMode ),
        SMR_ARCHIVELOG_MODE_STRING_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "LAST_INACTIVATED_LOGFILE_SEQ_NO",
        "the sequence number of the last logfile inactivated",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smrFxArchivelog, mLastArchivedLfsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "READABLE_ARCHIVELOG_DIR_COUNT",
        "readable archive log directory count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smrFxArchivelog, mDirCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "ARCHIVELOG_DIR_1",
        "archive log directory #1",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxArchivelog, mDir1 ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ARCHIVELOG_DIR_2",
        "archive log directory #2",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxArchivelog, mDir2 ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ARCHIVELOG_DIR_3",
        "archive log directory #3",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxArchivelog, mDir3 ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ARCHIVELOG_DIR_4",
        "archive log directory #4",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxArchivelog, mDir4 ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ARCHIVELOG_DIR_5",
        "archive log directory #5",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxArchivelog, mDir5 ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ARCHIVELOG_DIR_6",
        "archive log directory #6",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxArchivelog, mDir6 ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ARCHIVELOG_DIR_7",
        "archive log directory #7",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxArchivelog, mDir7 ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ARCHIVELOG_DIR_8",
        "archive log directory #8",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxArchivelog, mDir8 ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ARCHIVELOG_DIR_9",
        "archive log directory #9",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxArchivelog, mDir9 ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ARCHIVELOG_DIR_10",
        "archive log directory #10",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxArchivelog, mDir10 ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ARCHIVELOG_FILE",
        "archive log file format",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smrFxArchivelog, mFormat ),
        STL_MAX_FILE_NAME_LENGTH,
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

stlStatus smrOpenArchivelogFxTableCallback( void   * aStmt,
                                            void   * aDumpObjHandle,
                                            void   * aPathCtrl,
                                            knlEnv * aKnlEnv )
{
    stlInt64 * sStarted;

    sStarted = (stlInt64*)aPathCtrl;
    *sStarted = STL_FALSE;
    
    return STL_SUCCESS;
}

stlStatus smrCloseArchivelogFxTableCallback( void   * aDumpObjHandle,
                                             void   * aPathCtrl,
                                             knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus smrBuildRecordArchivelogFxTableCallback( void              * aDumpObjHandle,
                                                   void              * aPathCtrl,
                                                   knlValueBlockList * aValueList,
                                                   stlInt32            aBlockIdx,
                                                   stlBool           * aTupleExist,
                                                   knlEnv            * aKnlEnv )
{
    smrFxArchivelog    sFxRecord;
    stlInt64         * sStarted;

    *aTupleExist = STL_FALSE;
    sStarted = (stlInt64*)aPathCtrl;

    if( *sStarted == STL_FALSE )
    {
        stlStrcpy( sFxRecord.mMode,
                   (smrGetArchivelogMode() == SML_ARCHIVELOG_MODE)?
                   "ARCHIVELOG" : "NOARCHIVELOG" );

        STL_TRY( knlGetPropertyValueByName( "ARCHIVELOG_DIR_1",
                                            sFxRecord.mDir1,
                                            aKnlEnv ) == STL_SUCCESS );

        STL_TRY( knlGetPropertyValueByName( "ARCHIVELOG_DIR_2",
                                            sFxRecord.mDir2,
                                            aKnlEnv ) == STL_SUCCESS );

        STL_TRY( knlGetPropertyValueByName( "ARCHIVELOG_DIR_3",
                                            sFxRecord.mDir3,
                                            aKnlEnv ) == STL_SUCCESS );

        STL_TRY( knlGetPropertyValueByName( "ARCHIVELOG_DIR_4",
                                            sFxRecord.mDir4,
                                            aKnlEnv ) == STL_SUCCESS );

        STL_TRY( knlGetPropertyValueByName( "ARCHIVELOG_DIR_5",
                                            sFxRecord.mDir5,
                                            aKnlEnv ) == STL_SUCCESS );

        STL_TRY( knlGetPropertyValueByName( "ARCHIVELOG_DIR_6",
                                            sFxRecord.mDir6,
                                            aKnlEnv ) == STL_SUCCESS );

        STL_TRY( knlGetPropertyValueByName( "ARCHIVELOG_DIR_7",
                                            sFxRecord.mDir7,
                                            aKnlEnv ) == STL_SUCCESS );

        STL_TRY( knlGetPropertyValueByName( "ARCHIVELOG_DIR_8",
                                            sFxRecord.mDir8,
                                            aKnlEnv ) == STL_SUCCESS );

        STL_TRY( knlGetPropertyValueByName( "ARCHIVELOG_DIR_9",
                                            sFxRecord.mDir9,
                                            aKnlEnv ) == STL_SUCCESS );

        STL_TRY( knlGetPropertyValueByName( "ARCHIVELOG_DIR_10",
                                            sFxRecord.mDir10,
                                            aKnlEnv ) == STL_SUCCESS );

        STL_TRY( knlGetPropertyValueByName( "ARCHIVELOG_FILE",
                                            sFxRecord.mFormat,
                                            aKnlEnv ) == STL_SUCCESS );

        if( smrGetArchivelogMode() == SML_ARCHIVELOG_MODE )
        {
            sFxRecord.mLastArchivedLfsn =
                gSmrWarmupEntry->mLogPersData.mLastInactiveLfsn;
        }
        else
        {
            sFxRecord.mLastArchivedLfsn = SMR_INVALID_LOG_FILE_SEQ_NO;
        }

        sFxRecord.mDirCount = knlGetPropertyBigIntValueByID(
            KNL_PROPERTY_READABLE_ARCHIVELOG_DIR_COUNT,
            aKnlEnv );

        STL_TRY( knlBuildFxRecord( gSmrArchivelogColumnDesc,
                                   &sFxRecord,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );
        
        *aTupleExist = STL_TRUE;
        *sStarted = STL_TRUE;
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmrArchivelogTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    smrOpenArchivelogFxTableCallback,
    smrCloseArchivelogFxTableCallback,
    smrBuildRecordArchivelogFxTableCallback,
    STL_SIZEOF( stlInt64 ),
    "X$ARCHIVELOG",
    "archive log information",
    gSmrArchivelogColumnDesc
};

/** @} */
