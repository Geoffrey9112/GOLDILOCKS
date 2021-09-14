/*******************************************************************************
 * smfFxTable.c
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
#include <smg.h>
#include <smf.h>
#include <smr.h>
#include <smfDef.h>
#include <smrDef.h>
#include <smfTbsMgr.h>

/**
 * @file smfFxTable.c
 * @brief Storage Manager Layer Datafile Fixed Table Internal Routines
 */

/**
 * @addtogroup smfFxTable
 * @{
 */

knlFxColumnDesc gTablespaceColumnDesc[] =
{
    {
        "NAME",
        "tablespace name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxTbs, mTbsName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ID",
        "tablespace id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxTbs, mTbsId ),
        STL_SIZEOF( smlTbsId ),
        STL_FALSE  /* nullable */
    },
    {
        "ATTR",
        "tablespace attribute",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxTbs, mAttr ),
        64,
        STL_FALSE  /* nullable */
    },
    {
        "LOGGING",
        "logging",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smfFxTbs, mIsLogging ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "ONLINE",
        "online",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smfFxTbs, mIsOnline ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "SCAN_ONLINE",
        "online flag for scan",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smfFxTbs, mIsOnline4Scan ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "STATE",
        "tablespace state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxTbs, mState ),
        16,
        STL_FALSE  /* nullable */
    },
    {
        "OFFLINE_STATE",
        "tablespace offline state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxTbs, mOfflineState ),
        16,
        STL_FALSE  /* nullable */
    },
    {
        "PENDING_RELATION_ID",
        "pending relation identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxTbs, mRelationId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "EXTSIZE",
        "extent size (bytes)",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxTbs, mExtSize ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "NEW_DATAFILE_ID",
        "new datafile id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxTbs, mNewDatafileId ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "DATAFILE_COUNT",
        "count of datafile",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smfFxTbs, mDatafileCount ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "PAGE_SIZE",
        "tablespace page size",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smfFxTbs, mPageSize ),
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

stlStatus smfOpenTbsFxTableCallback( void   * aStmt,
                                     void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aKnlEnv )
{
    smlTbsId sTbsId = 0;
    
    stlMemcpy( aPathCtrl, &sTbsId, STL_SIZEOF(smlTbsId) );
    
    return STL_SUCCESS;
}

stlStatus smfCloseTbsFxTableCallback( void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus smfBuildRecordTbsFxTableCallback( void              * aDumpObjHandle,
                                            void              * aPathCtrl,
                                            knlValueBlockList * aValueList,
                                            stlInt32            aBlockIdx,
                                            stlBool           * aTupleExist,
                                            knlEnv            * aKnlEnv )
{
    smlTbsId          sTbsId;
    smfTbsInfo      * sTbsInfo;
    stlUInt32         i;
    smfFxTbs          sFxTbs;
    stlUInt32         sDataIdx;
    stlUInt32         sDatafileCount;
    smfDatafileInfo * sDatafileInfo;

    stlMemcpy( &sTbsId, aPathCtrl, STL_SIZEOF(smlTbsId) );

    for( i = sTbsId; i < SML_TBS_MAX_COUNT; i++ )
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[i];
        
        if( sTbsInfo == NULL )
        {
            continue;
        }

        sDatafileCount = 0;

        for( sDataIdx = 0; sDataIdx < SML_DATAFILE_MAX_COUNT; sDataIdx++ )
        {
            sDatafileInfo = sTbsInfo->mDatafileInfoArray[sDataIdx];

            if( sDatafileInfo == NULL )
            {
                continue;
            }

            if( (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_INIT) ||
                (sDatafileInfo->mDatafilePersData.mState == SMF_DATAFILE_STATE_DROPPED) )
            {
                continue;
            }

            sDatafileCount++;
        }

        stlStrncpy( sFxTbs.mTbsName,
                    sTbsInfo->mTbsPersData.mName,
                    STL_MAX_SQL_IDENTIFIER_LENGTH );
        sFxTbs.mTbsId = sTbsInfo->mTbsPersData.mTbsId;
        sFxTbs.mExtSize = ((stlInt64)sTbsInfo->mTbsPersData.mPageCountInExt) * SMP_PAGE_SIZE;
        sFxTbs.mNewDatafileId = sTbsInfo->mTbsPersData.mNewDatafileId;
        sFxTbs.mDatafileCount = sDatafileCount;
        sFxTbs.mRelationId = sTbsInfo->mTbsPersData.mRelationId;

        smfTbsAttrString( sTbsInfo->mTbsPersData.mAttr, sFxTbs.mAttr );

        sFxTbs.mIsLogging = SMF_IS_LOGGING_TBS( sFxTbs.mTbsId );
        sFxTbs.mIsOnline = SMF_IS_ONLINE_TBS( sFxTbs.mTbsId );
        sFxTbs.mIsOnline4Scan = sTbsInfo->mIsOnline4Scan;
        sFxTbs.mPageSize = SMP_PAGE_SIZE;

        smfTbsStateString( sTbsInfo->mTbsPersData.mState, sFxTbs.mState );

        smfTbsOfflineStateString( sTbsInfo->mTbsPersData.mOfflineState,
                                  sFxTbs.mOfflineState );

        STL_TRY( knlBuildFxRecord( gTablespaceColumnDesc,
                                   &sFxTbs,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );
        
        *aTupleExist = STL_TRUE;
        break;
    }

    if ( i < SML_TBS_MAX_COUNT )
    {
        sTbsId = i + 1;
        
        stlMemcpy( aPathCtrl, &sTbsId, STL_SIZEOF(smlTbsId) );
        *aTupleExist = STL_TRUE;
    }
    else
    {
        *aTupleExist = STL_FALSE;
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gTablespaceTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    smfOpenTbsFxTableCallback,
    smfCloseTbsFxTableCallback,
    smfBuildRecordTbsFxTableCallback,
    STL_SIZEOF( smlTbsId ),
    "X$TABLESPACE",
    "tablespace information",
    gTablespaceColumnDesc
};

knlFxColumnDesc gDatafileColumnDesc[] =
{
    {
        "TABLESPACE_ID",
        "tablespace id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxDatafile, mTbsId ),
        STL_SIZEOF(smlTbsId),
        STL_FALSE  /* nullable */
    },
    {
        "PATH",
        "datafile path",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxDatafile, mName ),
        1024,
        STL_FALSE  /* nullable */
    },
    {
        "STATE",
        "datafile state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxDatafile, mState ),
        16,
        STL_FALSE  /* nullable */
    },
    {
        "DATAFILE_ID",
        "datafile id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxDatafile, mId ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "SIZE",
        "size of datafile (bytes)",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxDatafile, mSize ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "USABLE_SIZE",
        "usable size of datafile (bytes)",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxDatafile, mUsableSize ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "CREATION_TIME",
        "creation timestamp",
        KNL_FXTABLE_DATATYPE_TIMESTAMP,
        STL_OFFSETOF( smfFxDatafile, mTimestamp ),
        STL_SIZEOF( stlTime ),
        STL_FALSE  /* nullable */
    },
    {
        "CHECKPOINT_LSN",
        "last checkpoint lsn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxDatafile, mCheckpointLsn ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "SHM_SEGMENT_ID",
        "shared memory segment identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxDatafile, mShmSegId ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "SHM_STATE",
        "shared memory state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxDatafile, mShmState ),
        16,
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

stlStatus smfOpenDatafileFxTableCallback( void   * aStmt,
                                          void   * aDumpObjHandle,
                                          void   * aPathCtrl,
                                          knlEnv * aKnlEnv )
{
    smfFxDatafileCtrlPath sCtrlPath;

    sCtrlPath.mTbsId = 0;
    sCtrlPath.mDatafileId = 0;
    
    stlMemcpy( aPathCtrl, &sCtrlPath, STL_SIZEOF(smfFxDatafileCtrlPath) );
    
    return STL_SUCCESS;
}

stlStatus smfCloseDatafileFxTableCallback( void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus smfBuildRecordDatafileFxTableCallback( void              * aDumpObjHandle,
                                                 void              * aPathCtrl,
                                                 knlValueBlockList * aValueList,
                                                 stlInt32            aBlockIdx,
                                                 stlBool           * aTupleExist,
                                                 knlEnv            * aKnlEnv )
{
    smlTbsId               sTbsId;
    smfTbsInfo           * sTbsInfo;
    smfDatafileInfo      * sDatafileInfo;
    stlUInt32              i;
    stlUInt32              j;
    smfFxDatafileCtrlPath  sCtrlPath;
    smfFxDatafile          sFxDatafile;
    smlDatafileId          sDatafileId;

    stlMemcpy( &sCtrlPath, aPathCtrl, STL_SIZEOF(smfFxDatafileCtrlPath) );

    sTbsId = sCtrlPath.mTbsId;
    sDatafileId = sCtrlPath.mDatafileId;

    for( i = sTbsId; i < SML_TBS_MAX_COUNT; i++ )
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[i];
        
        if( sTbsInfo == NULL )
        {
            continue;
        }

        for( j = sDatafileId; j < SML_DATAFILE_MAX_COUNT; j++ )
        {
            sDatafileInfo = sTbsInfo->mDatafileInfoArray[j];
            
            if( sDatafileInfo == NULL )
            {
                continue;
            }

            break;
        }

        if( j >= SML_DATAFILE_MAX_COUNT )
        {
            sDatafileId = 0;
            continue;
        }
        
        sFxDatafile.mTbsId = sTbsInfo->mTbsPersData.mTbsId;
        sFxDatafile.mId = sDatafileInfo->mDatafilePersData.mID;
        sFxDatafile.mShmSegId = sDatafileInfo->mShmIndex;
        sFxDatafile.mSize = sDatafileInfo->mDatafilePersData.mSize + SMP_PAGE_SIZE /* header */;
        sFxDatafile.mUsableSize = sDatafileInfo->mDatafilePersData.mSize;
        sFxDatafile.mTimestamp = sDatafileInfo->mDatafilePersData.mTimestamp;
        sFxDatafile.mCheckpointLsn = sDatafileInfo->mDatafilePersData.mCheckpointLsn;
        stlStrncpy( sFxDatafile.mName, sDatafileInfo->mDatafilePersData.mName, 1024 );

        smfDatafileStateString( sDatafileInfo->mDatafilePersData.mState,
                                sFxDatafile.mState );
        
        switch( sDatafileInfo->mShmState )
        {
            case SMF_DATAFILE_SHM_STATE_NULL:
                stlStrncpy( sFxDatafile.mShmState, "", 16 );
                break;
            case SMF_DATAFILE_SHM_STATE_ALLOCATED:
                stlStrncpy( sFxDatafile.mShmState, "ALLOCATED", 16 );
                break;
            case SMF_DATAFILE_SHM_STATE_LOADED:
                stlStrncpy( sFxDatafile.mShmState, "LOADED", 16 );
                break;
            case SMF_DATAFILE_SHM_STATE_PCH_INIT:
                stlStrncpy( sFxDatafile.mShmState, "PCH_INIT", 16 );
                break;
            default:
                stlStrncpy( sFxDatafile.mShmState, "UNKNOWN", 16 );
                break;
        }
        
        STL_TRY( knlBuildFxRecord( gDatafileColumnDesc,
                                   &sFxDatafile,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );
        
        *aTupleExist = STL_TRUE;
        break;
    }

    if ( i < SML_TBS_MAX_COUNT )
    {
        sCtrlPath.mTbsId = i;
        sCtrlPath.mDatafileId = j + 1;
        
        stlMemcpy( aPathCtrl, &sCtrlPath, STL_SIZEOF(smfFxDatafileCtrlPath) );
        *aTupleExist = STL_TRUE;
    }
    else
    {
        *aTupleExist = STL_FALSE;
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gDatafileTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    smfOpenDatafileFxTableCallback,
    smfCloseDatafileFxTableCallback,
    smfBuildRecordDatafileFxTableCallback,
    STL_SIZEOF( smfFxDatafileCtrlPath ),
    "X$DATAFILE",
    "datafile information",
    gDatafileColumnDesc
};

knlFxColumnDesc gDatafileHeaderColumnDesc[] =
{
    {
        "PATH",
        "datafile path",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxDatafileHeader, mName ),
        1024,
        STL_FALSE  /* nullable */
    },
    {
        "TABLESPACE_ID",
        "tablespace id",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smfFxDatafileHeader, mTbsId ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "DATAFILE_ID",
        "datafile id",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smfFxDatafileHeader, mDatafileId ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "CHECKPOINT_LSN",
        "checkpoint lsn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxDatafileHeader, mCheckpointLsn ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "CREATION_TIME",
        "creation timestamp",
        KNL_FXTABLE_DATATYPE_TIMESTAMP,
        STL_OFFSETOF( smfFxDatafileHeader, mCreationTime ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "TIMESTAMP",
        "creation timestamp",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxDatafileHeader, mCreationTime ),
        STL_SIZEOF( stlUInt64 ),
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

stlStatus smfOpenDatafileHeaderFxTableCallback( void   * aStmt,
                                                void   * aDumpObjHandle,
                                                void   * aPathCtrl,
                                                knlEnv * aKnlEnv )
{
    smfFxDatafileCtrlPath sCtrlPath;

    sCtrlPath.mTbsId = 0;
    sCtrlPath.mDatafileId = 0;
    
    stlMemcpy( aPathCtrl, &sCtrlPath, STL_SIZEOF(smfFxDatafileCtrlPath) );
    
    return STL_SUCCESS;
}

stlStatus smfCloseDatafileHeaderFxTableCallback( void   * aDumpObjHandle,
                                                 void   * aPathCtrl,
                                                 knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus smfBuildRecordDatafileHeaderFxTableCallback( void              * aDumpObjHandle,
                                                       void              * aPathCtrl,
                                                       knlValueBlockList * aValueList,
                                                       stlInt32            aBlockIdx,
                                                       stlBool           * aTupleExist,
                                                       knlEnv            * aKnlEnv )
{
    smlTbsId               sTbsId;
    smfTbsInfo           * sTbsInfo;
    smfDatafileInfo      * sDatafileInfo;
    stlUInt32              i;
    stlUInt32              j;
    smfFxDatafileCtrlPath  sCtrlPath;
    smfFxDatafileHeader    sFxDatafileHeader;
    smlDatafileId          sDatafileId;
    stlFile                sFile;
    smfDatafileHeader      sDatafileHeader;
    stlInt32               sFlag;
    stlInt64               sDatabaseIo;
    stlInt32               sState = 0;
    stlBool                sIsExist;

    stlMemcpy( &sCtrlPath, aPathCtrl, STL_SIZEOF(smfFxDatafileCtrlPath) );

    sTbsId = sCtrlPath.mTbsId;
    sDatafileId = sCtrlPath.mDatafileId;

    sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_READ;

    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 aKnlEnv );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    for( i = sTbsId; i < SML_TBS_MAX_COUNT; i++ )
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[i];
        
        if( sTbsInfo == NULL )
        {
            continue;
        }

        if( SML_TBS_IS_MEMORY_TEMPORARY( sTbsInfo->mTbsPersData.mAttr ) == STL_TRUE )
        {
            continue;
        }

        if( (sTbsInfo->mTbsPersData.mState != SMF_TBS_STATE_CREATED) &&
            (sTbsInfo->mTbsPersData.mState != SMF_TBS_STATE_CREATING) )
        {
            continue;
        }

        for( j = sDatafileId; j < SML_DATAFILE_MAX_COUNT; j++ )
        {
            sDatafileInfo = sTbsInfo->mDatafileInfoArray[j];
            
            if( sDatafileInfo == NULL )
            {
                continue;
            }

            /* Datafile Header는 Datafile이 존재하는 상태에서만 보여준다. */
            if( (sDatafileInfo->mDatafilePersData.mState != SMF_DATAFILE_STATE_CREATED) &&
                (sDatafileInfo->mDatafilePersData.mState != SMF_DATAFILE_STATE_CREATING) )
            {
                continue;
            }

            STL_TRY( stlExistFile( sDatafileInfo->mDatafilePersData.mName,
                                   &sIsExist,
                                   KNL_ERROR_STACK( aKnlEnv ) )
                     == STL_SUCCESS );

            if( sIsExist == STL_TRUE )
            {
                break;
            }
        }

        if( j >= SML_DATAFILE_MAX_COUNT )
        {
            sDatafileId = 0;
            continue;
        }

        STL_TRY( smgOpenFile( &sFile,
                              sDatafileInfo->mDatafilePersData.mName,
                              sFlag,
                              STL_FPERM_OS_DEFAULT,
                              SML_ENV( aKnlEnv ) )
                 == STL_SUCCESS );
        sState = 1;
        
        STL_TRY( smfReadDatafileHeader( &sFile,
                                        sDatafileInfo->mDatafilePersData.mName,
                                        &sDatafileHeader,
                                        SML_ENV(aKnlEnv) )
                 == STL_SUCCESS );
    
        sState = 0;
        STL_TRY( smgCloseFile( &sFile,
                               SML_ENV( aKnlEnv ) )
                 == STL_SUCCESS );

        
        sFxDatafileHeader.mTbsId = sDatafileHeader.mTbsPhysicalId;
        sFxDatafileHeader.mDatafileId = sDatafileHeader.mDatafileId;
        sFxDatafileHeader.mCheckpointLsn = sDatafileHeader.mCheckpointLsn;
        sFxDatafileHeader.mCreationTime = sDatafileHeader.mCreationTime;
        stlStrncpy( sFxDatafileHeader.mName, sDatafileInfo->mDatafilePersData.mName, 1024 );
        
        STL_TRY( knlBuildFxRecord( gDatafileHeaderColumnDesc,
                                   &sFxDatafileHeader,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );
        
        *aTupleExist = STL_TRUE;
        break;
    }

    if ( i < SML_TBS_MAX_COUNT )
    {
        sCtrlPath.mTbsId = i;
        sCtrlPath.mDatafileId = j + 1;
        
        stlMemcpy( aPathCtrl, &sCtrlPath, STL_SIZEOF(smfFxDatafileCtrlPath) );
        *aTupleExist = STL_TRUE;
    }
    else
    {
        *aTupleExist = STL_FALSE;
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void) smgCloseFile( &sFile, SML_ENV( aKnlEnv ) );
    }

    return STL_FAILURE;
}

knlFxTableDesc gDatafileHeaderTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    smfOpenDatafileHeaderFxTableCallback,
    smfCloseDatafileHeaderFxTableCallback,
    smfBuildRecordDatafileHeaderFxTableCallback,
    STL_SIZEOF( smfFxDatafileCtrlPath ),
    "X$DATAFILE_HEADER",
    "datafile header information",
    gDatafileHeaderColumnDesc
};

knlFxColumnDesc gDiskPageDumpColumnDesc[] =
{
    {
        "ADDRESS",
        "page offset",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxDiskPage, mOffSetStr ),
        SMF_PAGE_DUMP_DATA_OFFSET_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "DATA",
        "dumped disk page data",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxDiskPage, mData ),
        SMF_PAGE_DUMP_DATA_LENGTH * 2,
        STL_TRUE  /* nullable */
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

stlStatus smfGetDiskPageDumpObjectCallback( stlInt64   aTransID,
                                            void     * aStmt,
                                            stlChar  * aDumpObjName,
                                            void    ** aDumpObjHandle,
                                            knlEnv   * aEnv )
{
    /* Open Callback에서 Parsing */
    *aDumpObjHandle = (void*) aDumpObjName;
    
    return STL_SUCCESS;
}

stlStatus smfOpenDiskPageDumpObjectCallback( void   * aStmt,
                                             void   * aDumpObjHandle,
                                             void   * aPathCtrl,
                                             knlEnv * aEnv )
{
    stlChar  * sCurPtr;
    stlChar  * sToken1;
    stlChar  * sToken2;
    stlChar  * sToken3;
    stlChar  * sStr;
    stlChar  * sEndPtr;
    stlChar    sFileName[STL_MAX_FILE_PATH_LENGTH * 2];
    stlChar    sFilePath[STL_MAX_FILE_PATH_LENGTH];
    stlChar    sBuffer[SMP_PAGE_SIZE * 2];
    stlChar  * sPage;
    stlInt32   sTokenCount;
    stlInt32   sStrLen;
    stlInt32   sDatafileId;
    stlInt64   sPageSeqId;
    stlUInt64  sNum64Bit;
    smlPid     sPageId;
    stlFile    sFileDesc;
    stlOffset  sOffset;
    stlInt32   sFlag;
    stlInt64   sDatabaseIo;
    stlInt32   sState = 0;
    
    smfFxDiskPageIterator * sIterator;
    smfDatafileHeader       sDatafileHeader;
    
    sIterator = (smfFxDiskPageIterator *) aPathCtrl;
    sCurPtr   = (stlChar*) aDumpObjHandle;
    
    /* get file name */
    STL_TRY( knlTokenizeNextDumpOption( &sCurPtr,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );

    STL_TRY_THROW( sTokenCount == 2, RAMP_ERR_FINISH );

    /* 파일 경로의 명시 유무 체크  */
    sStrLen = stlSnprintf( sFileName,
                           STL_MAX_FILE_PATH_LENGTH * 2,
                           "%s.%s",
                           sToken1,
                           sToken2 );
    sFileName[sStrLen] = '\0';    
    sStr               = stlStrrchr( sFileName, STL_PATH_SEPARATOR_CHARACTER );

    
    if( sStr == NULL ) /* File 이름만 주어진 경우: Gliese DB folder에서 얻는다. */
    {
        STL_TRY_THROW( sStrLen <= STL_MAX_FILE_NAME_LENGTH, RAMP_ERR_NAME_LEN );      
        STL_TRY( knlGetPropertyValueByName( "SYSTEM_TABLESPACE_DIR",
                                            sFilePath,
                                            aEnv ) == STL_SUCCESS );

        STL_TRY( stlMergeAbsFilePath( sFilePath,
                                      sFileName,
                                      sFileName,
                                      STL_MAX_FILE_PATH_LENGTH,
                                      &sStrLen,
                                      KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    }
    else
    {
        /* do nothing*/
    }    
    STL_TRY_THROW( sStrLen <= STL_MAX_FILE_PATH_LENGTH, RAMP_ERR_NAME_LEN );

    /* get page sequence number */
    STL_TRY( knlTokenizeNextDumpOption( &sCurPtr,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );    

    STL_TRY_THROW( sTokenCount == 1, RAMP_ERR_FINISH );

    STL_TRY( stlStrToUInt64( sToken1,
                             STL_NTS,
                             &sEndPtr,
                             10,
                             &sNum64Bit,
                             KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    sPageId = (smlPid) sNum64Bit;
    sDatafileId = SMP_DATAFILE_ID( sPageId );
    sPageSeqId  = SMP_PAGE_SEQ_ID( sPageId );
    
    /* Open file */
    sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_READ;

    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( smgOpenFile( &sFileDesc,
                          sFileName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          SML_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smfReadDatafileHeader( &sFileDesc,
                                    sFileName,
                                    &sDatafileHeader,
                                    (smlEnv*) aEnv ) == STL_SUCCESS );

    /* check validation Data_File_Id */
    STL_TRY_THROW( sDatafileId == sDatafileHeader.mDatafileId, RAMP_ERR_FINISH );
    
    sOffset = SMP_PAGE_SIZE * sPageSeqId;

    STL_TRY( stlSeekFile( &sFileDesc,
                          STL_FSEEK_CUR,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    sPage = (stlChar*)( STL_ALIGN( (stlInt64)sBuffer, SMP_PAGE_SIZE ) );
    STL_TRY( stlReadFile( &sFileDesc,
                          sPage,
                          SMP_PAGE_SIZE,
                          NULL,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    sIterator->mSize       = SMP_PAGE_SIZE;
    sIterator->mRemainSize = SMP_PAGE_SIZE;
    sIterator->mCurAddr    = sIterator->mPageSnapshot;
    
    stlMemcpy( sIterator->mPageSnapshot, sPage, SMP_PAGE_SIZE );

    sState = 0;
    STL_TRY( smgCloseFile( &sFileDesc, SML_ENV( aEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FINISH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_DUMP_OPTION_STRING,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sFileName );
    }
    
    STL_CATCH( RAMP_ERR_NAME_LEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sFileName );
    }
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void) smgCloseFile( &sFileDesc, SML_ENV( aEnv ) );
    }

    return STL_FAILURE;
}

stlStatus smfCloseDiskPageDumpObjectCallback( void   * aDumpObjHandle,
                                              void   * aPathCtrl,
                                              knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus smfBuildRecordDiskPageDumpObjectCallback( void              * aDumpObjHandle,
                                                    void              * aPathCtrl,
                                                    knlValueBlockList * aValueList,
                                                    stlInt32            aBlockIdx,
                                                    stlBool           * aTupleExist,
                                                    knlEnv            * aEnv )
{
    stlInt32  sPrintSize;
    stlUInt32 sOffSet;
    stlInt32  sStrSize;
    
    smfFxDiskPage           sPageDump;
    smfFxDiskPageIterator * sIterator;

    sIterator = (smfFxDiskPageIterator*)aPathCtrl;    

    *aTupleExist = STL_FALSE;
    
    if( sIterator->mRemainSize > 0 )
    {
        if( sIterator->mRemainSize >= SMF_PAGE_DUMP_DATA_LENGTH )
        {
            sPrintSize = SMF_PAGE_DUMP_DATA_LENGTH;
        }
        else
        {
            sPrintSize = sIterator->mRemainSize;
        }
        /* PrintSize(Default 32) byte를 HexaDecimal로 변환과 OffSet 설정. */
        sOffSet = SMP_PAGE_SIZE - sIterator->mRemainSize;
        sStrSize = stlSnprintf( sPageDump.mOffSetStr,
                                SMF_PAGE_DUMP_DATA_OFFSET_LENGTH,
                                "%04X",
                                sOffSet );
        sPageDump.mOffSetStr[sStrSize] = '\0';
        
        STL_TRY( stlBinary2HexString( (stlUInt8*)(sIterator->mCurAddr),
                                      sPrintSize,
                                      sPageDump.mData,
                                      (SMF_PAGE_DUMP_DATA_LENGTH * 2) + 1,
                                      KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );        


        STL_TRY( knlBuildFxRecord( gDiskPageDumpColumnDesc,
                                   &sPageDump,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv ) == STL_SUCCESS );
        *aTupleExist = STL_TRUE;
        sIterator->mCurAddr += sPrintSize;
        sIterator->mRemainSize -= sPrintSize;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gDiskPageDumpTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    smfGetDiskPageDumpObjectCallback,
    smfOpenDiskPageDumpObjectCallback,
    smfCloseDiskPageDumpObjectCallback,
    smfBuildRecordDiskPageDumpObjectCallback,
    STL_SIZEOF( smfFxDiskPageIterator ),
    "D$DISK_PAGE",
    "disk page dump table - usage 'datafile_path,page_id'",
    gDiskPageDumpColumnDesc
};

/*
 * X$CONTROLFILE
 */
knlFxColumnDesc gCtrlSysSectionColumnDesc[] =
{
    {
        "SERVER_STATE",
        "state of the server",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlSysSection, mServerState ),
        SMF_CTRL_STATE_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "DATA_STORE_MODE",
        "data store mode",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlSysSection, mDataStoreMode ),
        SMF_CTRL_STATE_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "ARCHIVELOG_MODE",
        "archivelog mode",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlSysSection, mArchivelogMode ),
        SMF_CTRL_STATE_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "CHECKPOINT_LID",
        "checkpoint log identifier",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlSysSection, mChkptLid ),
        SMF_CTRL_CHKPTLID_STRING_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "CHECKPOINT_LSN",
        "checkpoint lsn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxCtrlSysSection, mCheckpointLsn ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "LAST_ARCHIVED_LOGFILE_SEQUNECE",
        "the sequence number of the last logfile archived",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxCtrlSysSection, mLastInactiveLfsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "LOG_STREAM_STATE",
        "state of log stream",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlSysSection, mLogStreamState ),
        SMF_CTRL_STATE_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "LOG_GROUP_COUNT",
        "the number of log groups",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smfFxCtrlSysSection, mLogGroupCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "BLOCK_SIZE",
        "log block size",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxCtrlSysSection, mBlockSize ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "CURRENT_FILE_SEQ_NO",
        "current log file sequence no",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxCtrlSysSection, mCurFileSeqNo ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "TRANSACTION_TABLE_SIZE",
        "transaction table size",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxCtrlSysSection, mTransTableSize ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "UNDO_REL_COUNT",
        "undo relation count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxCtrlSysSection, mUndoRelCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "TABLESPACE_COUNT",
        "tablespace count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxCtrlSysSection, mTbsCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "NEW_TABLESPACE_ID",
        "new tablespace id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxCtrlSysSection, mNewTbsId ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "CONTROLFILE_NAME",
        "control file name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlSysSection, mCtrlFileName ),
        STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "CONTROLFILE_STATE",
        "controlfile state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlSysSection, mCtrlFileState ),
        SMF_CTRL_STATE_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "IS_PRIMARY",
        "whether a controlfile is primary",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smfFxCtrlSysSection, mIsPrimary ),
        STL_SIZEOF( stlBool ),
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

stlStatus smfOpenCtrlCallback( void   * aStmt,
                               void   * aDumpObjHandle,
                               void   * aPathCtrl,
                               knlEnv * aEnv )
{
    smfFxCtrlPathCtrl * sPathCtrl;

    sPathCtrl = (smfFxCtrlPathCtrl *) aPathCtrl;

    sPathCtrl->mCtrlFileCount = smfGetCtrlFileCount();
    sPathCtrl->mCurCtrlFileId = -1;

    sPathCtrl->mLogGroupCount = 0;
    sPathCtrl->mCurLogGroupId = -1;
    sPathCtrl->mTbsCount = 0;
    sPathCtrl->mCurTbsId = -1;
    sPathCtrl->mFileCount = 0;
    sPathCtrl->mCurFileId = -1;
    sPathCtrl->mCurBackupSeq = -1;
    sPathCtrl->mIsFileOpen = STL_FALSE;
    
    stlMemset( &sPathCtrl->mSysPersData, 0x00, STL_SIZEOF( smfSysPersData ) );
    stlMemset( &sPathCtrl->mBackupIterator, 0x00, STL_SIZEOF( smfBackupIterator ) );
    stlMemset( (stlChar *)sPathCtrl->mBackupChunk, 0x00, SMF_BACKUP_CHUNK_SIZE );

    return STL_SUCCESS;
}

stlStatus smfCloseCtrlCallback( void   * aDumpObjHandle,
                                void   * aPathCtrl,
                                knlEnv * aEnv )
{
    smfFxCtrlPathCtrl * sPathCtrl;

    sPathCtrl = (smfFxCtrlPathCtrl *)aPathCtrl;

    if( sPathCtrl->mIsFileOpen == STL_TRUE )
    {
        STL_TRY( smfCloseCtrlSection( &(sPathCtrl->mFile),
                                      SML_ENV( aEnv ) )
                 == STL_SUCCESS );

        sPathCtrl->mIsFileOpen = STL_FALSE;
        sPathCtrl->mCurCtrlFileId = -1;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfOpenCtrlFile( smfFxCtrlPathCtrl * aPathCtrl,
                           smfCtrlSection      aSection,
                           stlBool           * aIsExist,
                           stlBool           * aIsValid,
                           knlEnv            * aEnv )
{
    stlInt32 sIdx;
    
    if( aPathCtrl->mCurCtrlFileId < (aPathCtrl->mCtrlFileCount - 1) )
    {
        /**
         * Control file이 여러 개일 경우 이전에 open된 control file을 close한다.
         */
        if( aPathCtrl->mCurCtrlFileId > -1 )
        {
            STL_TRY( smfCloseCtrlSection( &(aPathCtrl->mFile),
                                          SML_ENV( aEnv ) )
                     == STL_SUCCESS );
            
            aPathCtrl->mIsFileOpen = STL_FALSE;
        }

        sIdx = aPathCtrl->mCurCtrlFileId + 1;
        
        STL_TRY( smfOpenCtrlSectionWithPath( &(aPathCtrl->mFile),
                                             gSmfWarmupEntry->mCtrlFileName[sIdx],
                                             aSection,
                                             aIsValid, /* aIsValid */
                                             SML_ENV( aEnv ) ) == STL_SUCCESS );

        aPathCtrl->mIsFileOpen = STL_TRUE;
        
        SMF_INIT_CTRL_BUFFER( &aPathCtrl->mBuffer );
            
        stlStrcpy( aPathCtrl->mCtrlFileName, gSmfWarmupEntry->mCtrlFileName[sIdx] );
        aPathCtrl->mCurCtrlFileId++;

        *aIsExist = STL_TRUE;
    }
    else
    {
        STL_TRY( smfCloseCtrlSection( &(aPathCtrl->mFile),
                                      SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        aPathCtrl->mIsFileOpen = STL_FALSE;
        aPathCtrl->mCurCtrlFileId = -1;
        *aIsExist = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfBuildRecordCtrlSysSectionCallback( void              * aDumpObjHandle,
                                                void              * aPathCtrl,
                                                knlValueBlockList * aValueList,
                                                stlInt32            aBlockIdx,
                                                stlBool           * aTupleExist,
                                                knlEnv            * aEnv )
{
    smfFxCtrlPathCtrl    * sPathCtrl;
    smfFxCtrlSysSection    sFxRecord;
    smfSysPersData         sSysPersData;
    smrLogPersData         sLogPersData;
    smrLogStreamPersData   sLogStreamPersData;
    smfDbPersData          sDbPersData;
    stlBool                sIsExist;
    stlBool                sIsValid = STL_TRUE;

    sPathCtrl = (smfFxCtrlPathCtrl *) aPathCtrl;

    STL_TRY( smfOpenCtrlFile( sPathCtrl,
                              SMF_CTRL_SECTION_SYSTEM,
                              &sIsExist,
                              &sIsValid,
                              aEnv ) == STL_SUCCESS );

    if( sIsExist == STL_FALSE )
    {
        *aTupleExist = STL_FALSE;
        STL_THROW( RAMP_RETURN );
    }

    stlMemset( &sFxRecord, 0x00, STL_SIZEOF(smfFxCtrlSysSection) );
    stlStrcpy( sFxRecord.mCtrlFileName, sPathCtrl->mCtrlFileName );

    if( sIsValid == STL_FALSE )
    {        
        stlStrcpy( sFxRecord.mCtrlFileState, "CORRUPTED" );   
        STL_THROW( RAMP_BUILD_SUCCESS );
    }

    /**
     * Read Sys Pers Data
     */
    STL_TRY( smfReadCtrlFile( &(sPathCtrl->mFile),
                              &(sPathCtrl->mBuffer),
                              (stlChar *)&sSysPersData,
                              STL_SIZEOF( smfSysPersData ),
                              SML_ENV( aEnv ) ) == STL_SUCCESS );

    /**
     * Read Log Pers Data
     */
    STL_TRY( smfMoveCtrlFilePosition( &(sPathCtrl->mFile),
                                      &(sPathCtrl->mBuffer),
                                      STL_FSEEK_SET,
                                      sSysPersData.mLogSectionOffset,
                                      SML_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( smfReadCtrlFile( &(sPathCtrl->mFile),
                              &(sPathCtrl->mBuffer),
                              (stlChar *)&sLogPersData,
                              STL_SIZEOF( smrLogPersData ),
                              SML_ENV( aEnv ) ) == STL_SUCCESS );

    /**
     * Read Log Stream Pers Data
     */
    STL_TRY( smfReadCtrlFile( &(sPathCtrl->mFile),
                              &(sPathCtrl->mBuffer),
                              (stlChar *)&sLogStreamPersData,
                              STL_SIZEOF( smrLogStreamPersData ),
                              SML_ENV( aEnv ) ) == STL_SUCCESS );

    /**
     * Read DB Pers Data
     */
    STL_TRY( smfMoveCtrlFilePosition( &(sPathCtrl->mFile),
                                      &(sPathCtrl->mBuffer),
                                      STL_FSEEK_SET,
                                      sSysPersData.mDbSectionOffset,
                                      SML_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( smfReadCtrlFile( &(sPathCtrl->mFile),
                              &(sPathCtrl->mBuffer),
                              (stlChar *)&sDbPersData,
                              STL_SIZEOF( smfDbPersData ),
                              SML_ENV( aEnv ) ) == STL_SUCCESS );

    smfServerStateString( sSysPersData.mServerState,
                          sFxRecord.mServerState );
                          
    smfDataStoreModeString( sSysPersData.mDataStoreMode,
                            sFxRecord.mDataStoreMode );

    stlStrcpy( sFxRecord.mArchivelogMode,
               (sLogPersData.mArchivelogMode == 0)? "NOARCHIVELOG" : "ARCHIVELOG" );

    stlMemset( sFxRecord.mChkptLid, 0x00, SMF_CTRL_CHKPTLID_STRING_LENGTH );
    stlSnprintf( sFxRecord.mChkptLid,
                 SMF_CTRL_CHKPTLID_STRING_LENGTH,
                 "( %ld, %d, %d )",
                 SMR_FILE_SEQ_NO( &sLogPersData.mChkptLid ),
                 SMR_BLOCK_SEQ_NO( &sLogPersData.mChkptLid ),
                 SMR_BLOCK_OFFSET( &sLogPersData.mChkptLid ) );

    sFxRecord.mCheckpointLsn = sLogPersData.mChkptLsn;
    sFxRecord.mLastInactiveLfsn = sLogPersData.mLastInactiveLfsn;

    smrLogStreamStateString( sLogStreamPersData.mState,
                             SMF_CTRL_STATE_LENGTH,
                             sFxRecord.mLogStreamState );

    sFxRecord.mLogGroupCount = sLogStreamPersData.mLogGroupCount;
    sFxRecord.mBlockSize = sLogStreamPersData.mBlockSize;
    sFxRecord.mCurFileSeqNo = sLogStreamPersData.mCurFileSeqNo;

    sFxRecord.mTransTableSize = sDbPersData.mTransTableSize;
    sFxRecord.mUndoRelCount = sDbPersData.mUndoRelCount;
    sFxRecord.mTbsCount = sDbPersData.mTbsCount;
    sFxRecord.mNewTbsId = sDbPersData.mNewTbsId;

    stlStrcpy( sFxRecord.mCtrlFileState, "VALID" );

    /* 0번 Controlfile(CONTROL_FILE_0)은 PRIMARY controlfile,
     * 나머지는 SECONDARY Controlfile */
    sFxRecord.mIsPrimary = (sPathCtrl->mCurCtrlFileId == 0);

    STL_RAMP( RAMP_BUILD_SUCCESS );
        
    STL_TRY( knlBuildFxRecord( gCtrlSysSectionColumnDesc,
                               &sFxRecord,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;

    STL_RAMP( RAMP_RETURN );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gCtrlSysSectionTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    smfOpenCtrlCallback,
    smfCloseCtrlCallback,
    smfBuildRecordCtrlSysSectionCallback,
    STL_SIZEOF( smfFxCtrlPathCtrl ),
    "X$CONTROLFILE",
    "control file",
    gCtrlSysSectionColumnDesc
};

/*
 * X$CONTROLFILE_LOG_SECTION
 */
knlFxColumnDesc gCtrlLogSectionColumnDesc[] =
{
    {
        "GROUP_ID",
        "log group identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxCtrlLogSection, mLogGroupId ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "STATE",
        "state of log group",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlLogSection, mLogGroupState ),
        SMF_CTRL_STATE_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "FILE_SEQ_NO",
        "log file sequence no",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxCtrlLogSection, mFileSeqNo ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "FILE_SIZE",
        "log file size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxCtrlLogSection, mFileSize ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "PREV_LAST_LSN",
        "the last lsn of previous log file",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxCtrlLogSection, mPrevLastLsn ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "MEMBER_COUNT",
        "log member count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smfFxCtrlLogSection, mLogMemberCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "MEMBER_PATH_1",
        "the path of log member 1",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlLogSection, mMemberName[0] ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "MEMBER_PATH_2",
        "the path of log member 2",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlLogSection, mMemberName[1] ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "MEMBER_PATH_3",
        "the path of log member 3",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlLogSection, mMemberName[2] ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "MEMBER_PATH_4",
        "the path of log member 4",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlLogSection, mMemberName[3] ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "MEMBER_PATH_5",
        "the path of log member 5",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlLogSection, mMemberName[4] ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "CONTROLFILE_NAME",
        "control file name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlLogSection, mCtrlFileName ),
        STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "CONTROLFILE_STATE",
        "controlfile state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlLogSection, mCtrlFileState ),
        SMF_CTRL_STATE_LENGTH,
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

stlStatus smfGetLogGroup( void                * aDumpObjHandle,
                          void                * aPathCtrl,
                          smrLogGroupPersData * aLogGroup,
                          stlBool             * aIsExist,
                          stlBool             * aIsValid,
                          knlEnv              * aEnv )
{
    smfFxCtrlPathCtrl    * sPathCtrl;
    smrLogPersData         sLogPersData;
    smrLogStreamPersData   sLogStreamPersData;
    stlBool                sIsExist;

    sPathCtrl = (smfFxCtrlPathCtrl *) aPathCtrl;

    if( (sPathCtrl->mLogGroupCount - 1) == sPathCtrl->mCurLogGroupId )
    {
        STL_TRY( smfOpenCtrlFile( sPathCtrl,
                                  SMF_CTRL_SECTION_LOG,
                                  &sIsExist,
                                  aIsValid,
                                  aEnv ) == STL_SUCCESS );

        if( sIsExist == STL_FALSE )
        {
            *aIsExist = STL_FALSE;
            STL_THROW( RAMP_RETURN );
        }

        if( *aIsValid == STL_FALSE )
        {
            *aIsExist = STL_TRUE;
            sPathCtrl->mLogGroupCount = 0;
            sPathCtrl->mCurLogGroupId = -1;
            STL_THROW( RAMP_RETURN );
        }

        /**
         * Read Log Pers Data
         */
        STL_TRY( smfReadCtrlFile( &(sPathCtrl->mFile),
                                  &(sPathCtrl->mBuffer),
                                  (stlChar *)&sLogPersData,
                                  STL_SIZEOF( smrLogPersData ),
                                  SML_ENV( aEnv ) ) == STL_SUCCESS );

        /**
         * Read Log Stream Pers Data
         */
        STL_TRY( smfReadCtrlFile( &(sPathCtrl->mFile),
                                  &(sPathCtrl->mBuffer),
                                  (stlChar *)&sLogStreamPersData,
                                  STL_SIZEOF( smrLogStreamPersData ),
                                  SML_ENV( aEnv ) ) == STL_SUCCESS );

        sPathCtrl->mLogGroupCount = sLogStreamPersData.mLogGroupCount;
        sPathCtrl->mCurLogGroupId = -1;
    }

    STL_TRY( smfReadCtrlFile( &(sPathCtrl->mFile),
                              &(sPathCtrl->mBuffer),
                              (stlChar *)aLogGroup,
                              STL_SIZEOF( smrLogGroupPersData ),
                              SML_ENV( aEnv ) ) == STL_SUCCESS );

    sPathCtrl->mCurLogGroupId++;
    *aIsExist = STL_TRUE;

    STL_RAMP( RAMP_RETURN );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfBuildRecordCtrlLogSectionCallback( void              * aDumpObjHandle,
                                                void              * aPathCtrl,
                                                knlValueBlockList * aValueList,
                                                stlInt32            aBlockIdx,
                                                stlBool           * aTupleExist,
                                                knlEnv            * aEnv )
{
    smfFxCtrlPathCtrl    * sPathCtrl;
    smfFxCtrlLogSection    sFxRecord;
    smrLogGroupPersData    sLogGroupPersData;
    smrLogMemberPersData   sLogMemberPersData;
    stlBool                sIsExist;
    stlBool                sIsValid = STL_TRUE;
    stlInt32               i;

    sPathCtrl = (smfFxCtrlPathCtrl *) aPathCtrl;

    STL_TRY( smfGetLogGroup( aDumpObjHandle,
                             aPathCtrl,
                             &sLogGroupPersData,
                             &sIsExist,
                             &sIsValid,
                             aEnv ) == STL_SUCCESS );

    if( sIsExist == STL_FALSE )
    {
        *aTupleExist = STL_FALSE;
        STL_THROW( RAMP_RETURN );
    }

    stlMemset( &sFxRecord, 0x00, STL_SIZEOF(smfFxCtrlLogSection) );
    stlStrcpy( sFxRecord.mCtrlFileName, sPathCtrl->mCtrlFileName );

    if( sIsValid == STL_FALSE )
    {
        stlStrcpy( sFxRecord.mCtrlFileState, "CORRUPTED" );
        STL_THROW( RAMP_BUILD_SUCCESS );
    }
    
    smrLogGroupStateString( sLogGroupPersData.mState,
                            SMF_CTRL_STATE_LENGTH,
                            sFxRecord.mLogGroupState );

    sFxRecord.mLogMemberCount = sLogGroupPersData.mLogMemberCount;
    sFxRecord.mLogGroupId     = sLogGroupPersData.mLogGroupId;
    sFxRecord.mFileSeqNo      = sLogGroupPersData.mFileSeqNo;
    sFxRecord.mFileSize       = sLogGroupPersData.mFileSize;
    sFxRecord.mPrevLastLsn    = sLogGroupPersData.mPrevLastLsn;

    for( i = 0; i < sLogGroupPersData.mLogMemberCount; i++ )
    {
        STL_TRY( smfReadCtrlFile( &(sPathCtrl->mFile),
                                  &(sPathCtrl->mBuffer),
                                  (stlChar *)&sLogMemberPersData,
                                  STL_SIZEOF( smrLogMemberPersData ),
                                  SML_ENV( aEnv ) ) == STL_SUCCESS );

        stlStrcpy( sFxRecord.mMemberName[i], sLogMemberPersData.mName );
    }

    for( ; i < SMR_MAX_LOGMEMBER_COUNT; i++ )
    {
        stlMemset( sFxRecord.mMemberName[i], 0x00, STL_MAX_FILE_PATH_LENGTH );
    }

    stlStrcpy( sFxRecord.mCtrlFileState, "VALID" );

    STL_RAMP( RAMP_BUILD_SUCCESS );

    STL_TRY( knlBuildFxRecord( gCtrlLogSectionColumnDesc,
                               &sFxRecord,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;

    STL_RAMP( RAMP_RETURN );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gCtrlLogSectionTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    smfOpenCtrlCallback,
    smfCloseCtrlCallback,
    smfBuildRecordCtrlLogSectionCallback,
    STL_SIZEOF( smfFxCtrlPathCtrl ),
    "X$CONTROLFILE_LOG_SECTION",
    "control file log section",
    gCtrlLogSectionColumnDesc
};

/**
 * X$CONTROLFILE_DB_SECTION : tablespace info
 */
knlFxColumnDesc gCtrlTbsColumnDesc[] =
{
    {
        "TABLESPACE_NAME",
        "tablespace name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlTbs, mTbsName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "TABLESPACE_ID",
        "tablespace identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxCtrlTbs, mTbsId ),
        STL_SIZEOF( smlTbsId ),
        STL_FALSE  /* nullable */
    },
    {
        "ATTR",
        "tablespace attribute",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlTbs, mAttr ),
        SMF_CTRL_TBS_ATTR_STRING_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "STATE",
        "tablespace state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlTbs, mState ),
        SMF_CTRL_STATE_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "LOGGING",
        "logging",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smfFxCtrlTbs, mIsLogging ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "ONLINE",
        "online",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smfFxCtrlTbs, mIsOnline ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "OFFLINE_STATE",
        "tablespace offline state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlTbs, mOfflineState ),
        SMF_CTRL_STATE_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "EXTSIZE",
        "extent size (bytes)",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxCtrlTbs, mExtSize ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "NEW_DATAFILE_ID",
        "new datafile id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxCtrlTbs, mNewDatafileId ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "DATAFILE_COUNT",
        "count of datafile",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smfFxCtrlTbs, mDatafileCount ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "PENDING_RELATION_ID",
        "pending relation identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxCtrlTbs, mRelationId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "CONTROLFILE_NAME",
        "control file name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlTbs, mCtrlFileName ),
        STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "CONTROLFILE_STATE",
        "controlfile state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlTbs, mCtrlFileState ),
        SMF_CTRL_STATE_LENGTH,
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

stlStatus smfGetTbs( void              * aDumpObjHandle,
                     void              * aPathCtrl,
                     smfTbsPersData    * aTbsPersData,
                     stlBool           * aIsExist,
                     stlBool           * aIsValid,
                     knlEnv            * aEnv )
{
    smfFxCtrlPathCtrl    * sPathCtrl;
    smfDbPersData          sDbPersData;
    stlBool                sIsExist;

    sPathCtrl = (smfFxCtrlPathCtrl *) aPathCtrl;

    if( (sPathCtrl->mTbsCount - 1) == sPathCtrl->mCurTbsId )
    {
        STL_TRY( smfOpenCtrlFile( sPathCtrl,
                                  SMF_CTRL_SECTION_DB,
                                  &sIsExist,
                                  aIsValid,
                                  aEnv ) == STL_SUCCESS );

        if( sIsExist == STL_FALSE )
        {
            *aIsExist = STL_FALSE;
            STL_THROW( RAMP_RETURN );
        }

        if( *aIsValid == STL_FALSE )
        {
            *aIsExist = STL_TRUE;
            sPathCtrl->mTbsCount = 0;
            sPathCtrl->mCurTbsId = -1;
            STL_THROW( RAMP_RETURN );
        }

        /**
         * Read DB Pers Data
         */
        STL_TRY( smfReadCtrlFile( &(sPathCtrl->mFile),
                                  &(sPathCtrl->mBuffer),
                                  (stlChar *)&sDbPersData,
                                  STL_SIZEOF( smfDbPersData ),
                                  SML_ENV( aEnv ) ) == STL_SUCCESS );

        sPathCtrl->mTbsCount = sDbPersData.mTbsCount;
        sPathCtrl->mCurTbsId = -1;
    }

    STL_TRY( smfReadCtrlFile( &(sPathCtrl->mFile),
                              &(sPathCtrl->mBuffer),
                              (stlChar *)aTbsPersData,
                              STL_SIZEOF( smfTbsPersData ),
                              SML_ENV( aEnv ) ) == STL_SUCCESS );

    sPathCtrl->mCurTbsId++;
    *aIsExist = STL_TRUE;

    STL_RAMP( RAMP_RETURN );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfBuildRecordCtrlDBSectionCallback( void              * aDumpObjHandle,
                                               void              * aPathCtrl,
                                               knlValueBlockList * aValueList,
                                               stlInt32            aBlockIdx,
                                               stlBool           * aTupleExist,
                                               knlEnv            * aEnv )
{
    smfFxCtrlPathCtrl    * sPathCtrl;
    smfFxCtrlTbs           sFxRecord;
    smfTbsPersData         sTbsPersData;
    stlOffset              sOffset;
    stlBool                sIsExist;
    stlBool                sIsValid = STL_TRUE;

    sPathCtrl = (smfFxCtrlPathCtrl *) aPathCtrl;

    STL_TRY( smfGetTbs( aDumpObjHandle,
                        aPathCtrl,
                        &sTbsPersData,
                        &sIsExist,
                        &sIsValid,
                        aEnv ) == STL_SUCCESS );

    if( sIsExist == STL_FALSE )
    {
        *aTupleExist = STL_FALSE;
        STL_THROW( RAMP_RETURN );
    }

    stlMemset( &sFxRecord, 0x00, STL_SIZEOF( smfFxCtrlTbs ) );
    stlStrcpy( sFxRecord.mCtrlFileName, sPathCtrl->mCtrlFileName );

    if( sIsValid == STL_FALSE )
    {
        stlStrcpy( sFxRecord.mCtrlFileState, "CORRUPTED" );
        STL_THROW( RAMP_BUILD_SUCCESS );
    }

    stlStrcpy( sFxRecord.mTbsName, sTbsPersData.mName );

    sFxRecord.mTbsId = sTbsPersData.mTbsId;
    sFxRecord.mExtSize = ((stlInt64)sTbsPersData.mPageCountInExt) * SMP_PAGE_SIZE;
    sFxRecord.mNewDatafileId = sTbsPersData.mNewDatafileId;
    sFxRecord.mDatafileCount = sTbsPersData.mDatafileCount;
    sFxRecord.mRelationId = sTbsPersData.mRelationId;

    smfTbsAttrString( sTbsPersData.mAttr, sFxRecord.mAttr );

    sFxRecord.mIsLogging = sTbsPersData.mIsLogging;
    sFxRecord.mIsOnline = sTbsPersData.mIsOnline;

    smfTbsStateString( sTbsPersData.mState, sFxRecord.mState );
    smfTbsOfflineStateString( sTbsPersData.mOfflineState,
                              sFxRecord.mOfflineState );

    stlStrcpy( sFxRecord.mCtrlFileState, "VALID" );

    /**
     * Datafile Pers Data skip
     */ 
    sOffset = sTbsPersData.mDatafileCount * STL_SIZEOF( smfDatafilePersData );
    STL_TRY( smfMoveCtrlFilePosition( &(sPathCtrl->mFile),
                                      &(sPathCtrl->mBuffer),
                                      STL_FSEEK_CUR,
                                      sOffset,
                                      SML_ENV( aEnv ) ) == STL_SUCCESS );

    STL_RAMP( RAMP_BUILD_SUCCESS );
    
    STL_TRY( knlBuildFxRecord( gCtrlTbsColumnDesc,
                               &sFxRecord,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;

    STL_RAMP( RAMP_RETURN );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gCtrlDBSectionTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    smfOpenCtrlCallback,
    smfCloseCtrlCallback,
    smfBuildRecordCtrlDBSectionCallback,
    STL_SIZEOF( smfFxCtrlPathCtrl ),
    "X$CONTROLFILE_DB_SECTION",
    "control file database section",
    gCtrlTbsColumnDesc
};

/**
 * X$CONTROLFILE_DATAFILE : datafile info
 */
knlFxColumnDesc gCtrlDatafileColumnDesc[] =
{
    {
        "TABLESPACE_ID",
        "tablespace identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxCtrlDatafile, mTbsId ),
        STL_SIZEOF( smlTbsId ),
        STL_FALSE  /* nullable */
    },
    {
        "DATAFILE_ID",
        "datafile identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxCtrlDatafile, mID ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "STATE",
        "the state of datafile",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlDatafile, mState ),
        SMF_CTRL_STATE_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "AUTO_EXTEND",
        "auto extend",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smfFxCtrlDatafile, mAutoExtend ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "SIZE",
        "datafile size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxCtrlDatafile, mSize ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "NEXT_SIZE",
        "datafile next size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxCtrlDatafile, mNextSize ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "MAX_SIZE",
        "datafile max size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxCtrlDatafile, mMaxSize ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "PATH",
        "datafile path",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlDatafile, mName ),
        STL_MAX_FILE_PATH_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "CONTROLFILE_NAME",
        "control file name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlDatafile, mCtrlFileName ),
        STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "CONTROLFILE_STATE",
        "controlfile state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxCtrlDatafile, mCtrlFileState ),
        SMF_CTRL_STATE_LENGTH,
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

stlStatus smfGetDatafile( void                * aDumpObjHandle,
                          void                * aPathCtrl,
                          smfDatafilePersData * aDatafile,
                          stlBool             * aIsExist,
                          stlBool             * aIsValid,
                          knlEnv              * aEnv )
{
    smfFxCtrlPathCtrl  * sPathCtrl;
    smfTbsPersData       sTbsPersData;
    stlBool              sIsExist;

    sPathCtrl = (smfFxCtrlPathCtrl *) aPathCtrl;

    if( (sPathCtrl->mFileCount - 1) == sPathCtrl->mCurFileId )
    {
        STL_TRY( smfGetTbs( aDumpObjHandle,
                            aPathCtrl,
                            &sTbsPersData,
                            &sIsExist,
                            aIsValid,
                            aEnv ) == STL_SUCCESS );

        if( sIsExist == STL_FALSE )
        {
            *aIsExist = STL_FALSE;
            STL_THROW( RAMP_RETURN );
        }

        if( *aIsValid == STL_FALSE )
        {
            *aIsExist = STL_TRUE;
            sPathCtrl->mFileCount = 0;
            sPathCtrl->mCurFileId = -1;
            STL_THROW( RAMP_RETURN );
        }
        
        sPathCtrl->mFileCount = sTbsPersData.mDatafileCount;
        sPathCtrl->mCurFileId = -1;
    }

    STL_TRY( smfReadCtrlFile( &(sPathCtrl->mFile),
                              &(sPathCtrl->mBuffer),
                              (stlChar *)aDatafile,
                              STL_SIZEOF( smfDatafilePersData ),
                              SML_ENV( aEnv ) ) == STL_SUCCESS );

    sPathCtrl->mCurFileId++;
    *aIsExist = STL_TRUE;

    STL_RAMP( RAMP_RETURN );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfBuildRecordCtrlDatafileCallback( void              * aDumpObjHandle,
                                              void              * aPathCtrl,
                                              knlValueBlockList * aValueList,
                                              stlInt32            aBlockIdx,
                                              stlBool           * aTupleExist,
                                              knlEnv            * aEnv )
{
    smfFxCtrlPathCtrl    * sPathCtrl;
    smfFxCtrlDatafile      sFxRecord;
    smfDatafilePersData    sDatafilePersData;
    stlBool                sIsExist;
    stlBool                sIsValid = STL_TRUE;

    sPathCtrl = (smfFxCtrlPathCtrl *) aPathCtrl;

    STL_TRY( smfGetDatafile( aDumpObjHandle,
                             aPathCtrl,
                             &sDatafilePersData,
                             &sIsExist,
                             &sIsValid,
                             aEnv ) == STL_SUCCESS );

    if( sIsExist == STL_FALSE )
    {
        *aTupleExist = STL_FALSE;
        STL_THROW( RAMP_RETURN );
    }

    stlMemset( &sFxRecord, 0x00, STL_SIZEOF( smfFxCtrlDatafile ) );
    stlStrcpy( sFxRecord.mCtrlFileName, sPathCtrl->mCtrlFileName );

    if( sIsValid == STL_FALSE )
    {
        stlStrcpy( sFxRecord.mCtrlFileState, "CORRUPTED" );
        STL_THROW( RAMP_BUILD_SUCCESS );
    }

    sFxRecord.mTbsId = sPathCtrl->mCurTbsId;
    sFxRecord.mID = sDatafilePersData.mID;
    sFxRecord.mAutoExtend = sDatafilePersData.mAutoExtend;
    sFxRecord.mSize = sDatafilePersData.mSize;
    sFxRecord.mNextSize = sDatafilePersData.mNextSize;
    sFxRecord.mMaxSize = sDatafilePersData.mMaxSize;

    stlStrcpy( sFxRecord.mName, sDatafilePersData.mName );

    smfDatafileStateString( sDatafilePersData.mState, sFxRecord.mState );

    stlStrcpy( sFxRecord.mCtrlFileState, "VALID" );

    STL_RAMP( RAMP_BUILD_SUCCESS );

    STL_TRY( knlBuildFxRecord( gCtrlDatafileColumnDesc,
                               &sFxRecord,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;

    STL_RAMP( RAMP_RETURN );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gCtrlDatafileTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    smfOpenCtrlCallback,
    smfCloseCtrlCallback,
    smfBuildRecordCtrlDatafileCallback,
    STL_SIZEOF( smfFxCtrlPathCtrl ),
    "X$CONTROLFILE_DATAFILE",
    "control file datafile",
    gCtrlDatafileColumnDesc
};

/*
 * X$BACKUP
 */
knlFxColumnDesc gBackupColumnDesc[] =
{
    {
        "NAME",
        "tablespace name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxBackup, mTbsName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ID",
        "tablespace id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxBackup, mTbsId ),
        STL_SIZEOF( smlTbsId ),
        STL_FALSE  /* nullable */
    },
    {
        "STATUS",
        "backup status",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxBackup, mStatus ),
        10,
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

stlStatus smfOpenBackupFxTableCallback( void   * aStmt,
                                        void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aKnlEnv )
{
    smfFxBackupCtrlPath    sCtrlPath;

    STL_TRY( smfFirstTbs( &sCtrlPath.mTbsId,
                          &sCtrlPath.mTbsExist,
                          SML_ENV( aKnlEnv ) ) == STL_SUCCESS );

    stlMemcpy( aPathCtrl, &sCtrlPath, STL_SIZEOF(smfFxBackupCtrlPath) );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfCloseBackupFxTableCallback( void   * aDumpObjHandle,
                                         void   * aPathCtrl,
                                         knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus smfBuildRecordBackupFxTableCallback( void              * aDumpObjHandle,
                                               void              * aPathCtrl,
                                               knlValueBlockList * aValueList,
                                               stlInt32            aBlockIdx,
                                               stlBool           * aTupleExist,
                                               knlEnv            * aKnlEnv )
{
    smfFxBackupCtrlPath    sCtrlPath;
    smfTbsInfo           * sTbsInfo;
    stlUInt32              sBackupState;
    smfFxBackup            sFxBackup;

    stlMemcpy( &sCtrlPath, aPathCtrl, STL_SIZEOF(smfFxBackupCtrlPath) );

    while( sCtrlPath.mTbsExist == STL_TRUE )
    {
        sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[sCtrlPath.mTbsId];

        if( SMF_TBS_AVAILABLE_BACKUP( sCtrlPath.mTbsId ) == STL_TRUE )
        {
            break;
        }
        else
        {
            STL_TRY( smfNextTbs( &sCtrlPath.mTbsId,
                                 &sCtrlPath.mTbsExist,
                                 SML_ENV( aKnlEnv ) ) == STL_SUCCESS );
        }
    }

    if( sCtrlPath.mTbsExist == STL_TRUE )
    {
        stlStrncpy( sFxBackup.mTbsName,
                    sTbsInfo->mTbsPersData.mName,
                    STL_MAX_SQL_IDENTIFIER_LENGTH );
        sFxBackup.mTbsId = sTbsInfo->mTbsPersData.mTbsId;

        sBackupState = sTbsInfo->mBackupInfo.mBackupState;

        stlStrcpy( sFxBackup.mStatus,
                   ((sBackupState & SMF_BACKUP_STATE_FULL_BACKUP) ==
                    SMF_BACKUP_STATE_FULL_BACKUP)? "ACTIVE" : "INACTIVE" );

        STL_TRY( knlBuildFxRecord( gBackupColumnDesc,
                                   &sFxBackup,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        STL_TRY( smfNextTbs( &sCtrlPath.mTbsId,
                             &sCtrlPath.mTbsExist,
                             SML_ENV( aKnlEnv ) ) == STL_SUCCESS );

        stlMemcpy( aPathCtrl, &sCtrlPath, STL_SIZEOF(smfFxBackupCtrlPath) );
        *aTupleExist = STL_TRUE;
    }
    else
    {
        stlMemcpy( aPathCtrl, &sCtrlPath, STL_SIZEOF(smfFxBackupCtrlPath) );
        *aTupleExist = STL_FALSE;
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gBackupTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    smfOpenBackupFxTableCallback,
    smfCloseBackupFxTableCallback,
    smfBuildRecordBackupFxTableCallback,
    STL_SIZEOF( smlTbsId ),
    "X$BACKUP",
    "backup information",
    gBackupColumnDesc
};

/**
 * D$INCREMENTAL_BACKUP_LIST
 */
knlFxColumnDesc gIncBackupColumnDesc[] =
{
    {
        "BACKUP_SEQ",
        "backup sequence no",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxIncBackup, mBackupSeq ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "BACKUP_LEVEL",
        "incremental backup level",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxIncBackup, mLevel ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "BACKUP_OBJECT",
        "backup object",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxIncBackup, mObjectType ),
        STL_MAX_FILE_NAME_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "BACKUP_TRUE",
        "backup option",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxIncBackup, mBackupOption ),
        12,
        STL_FALSE  /* nullable */
    },
    {
        "BACKUP_LSN",
        "backup Lsn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxIncBackup, mBackupLsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "BACKUP_FILE",
        "backup file name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxIncBackup, mFilePath ),
        STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH + 1,
        STL_FALSE  /* nullable */
    },
    {
        "CONTROLFILE_NAME",
        "control file name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxIncBackup, mCtrlFileName ),
        STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH + 1,
        STL_FALSE  /* nullable */
    },
    {
        "DELETED",
        "deleted backup file",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smfFxIncBackup, mIsDeleted ),
        STL_SIZEOF( stlBool ),
        STL_FALSE
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

stlStatus smfGetIncBackupListCallback( stlInt64   aTransID,
                                       void     * aStmt,
                                       stlChar  * aDumpObjName,
                                       void    ** aDumpObjHandle,
                                       knlEnv   * aEnv )
{
    /* Open Callback에서 Parsing */
    *aDumpObjHandle = (void*) aDumpObjName;
    
    return STL_SUCCESS;
}

stlStatus smfOpenIncBackupListCallback( void   * aStmt,
                                        void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aEnv )
{
    stlChar                  sFilePath[STL_MAX_FILE_PATH_LENGTH];
    stlInt32                 sFileNameLen;
    stlBool                  sIsExist;
    smfFxIncBackupIterator * sIterator;
    smfCtrlBuffer            sBuffer;

    sIterator = (smfFxIncBackupIterator *) aPathCtrl;

    sIterator->mFileOpen = STL_FALSE;
    sIterator->mBackupSeq = 0;

    SMF_INIT_BACKUP_ITERATOR( &(sIterator->mBackupIterator), STL_FALSE );

    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_LOG_DIR,
                                      sFilePath,
                                      aEnv ) == STL_SUCCESS );

    STL_TRY( stlMergeAbsFilePath( sFilePath,
                                  aDumpObjHandle,
                                  sIterator->mCtrlFileName,
                                  STL_MAX_FILE_PATH_LENGTH,
                                  &sFileNameLen,
                                  KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( stlExistFile( sIterator->mCtrlFileName,
                           &sIsExist,
                           KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_TRY_THROW( sIsExist == STL_TRUE, RAMP_ERR_NOT_EXIST_FILE );

    STL_TRY( smfOpenCtrlSectionWithPath( &(sIterator->mFile),
                                         sIterator->mCtrlFileName,
                                         SMF_CTRL_SECTION_SYSTEM,
                                         NULL, /* aIsValid */
                                         SML_ENV( aEnv ) ) == STL_SUCCESS );

    sIterator->mFileOpen = STL_TRUE;

    SMF_INIT_CTRL_BUFFER( &sBuffer );

    STL_TRY( smfReadCtrlFile( &(sIterator->mFile),
                              &sBuffer,
                              (stlChar*)&(sIterator->mSysPersData),
                              STL_SIZEOF(smfSysPersData),
                              SML_ENV(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sIterator->mCtrlFileName );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfCloseIncBackupListCallback( void   * aDumpObjHandle,
                                         void   * aPathCtrl,
                                         knlEnv * aKnlEnv )
{
    smfFxIncBackupIterator * sIterator;
    
    sIterator = (smfFxIncBackupIterator *) aPathCtrl;

    if( sIterator->mFileOpen == STL_TRUE )
    {
        STL_TRY( smfCloseCtrlSection( &(sIterator->mFile),
                                      SML_ENV( aKnlEnv ) )
                 == STL_SUCCESS );

        sIterator->mFileOpen = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfBuildRecordIncBackupListCallback( void              * aDumpObjHandle,
                                               void              * aPathCtrl,
                                               knlValueBlockList * aValueList,
                                               stlInt32            aBlockIdx,
                                               stlBool           * aTupleExist,
                                               knlEnv            * aEnv )
{
    smfFxIncBackup            sBackup;
    smfFxIncBackupIterator  * sIterator;

    sIterator = (smfFxIncBackupIterator*)aPathCtrl;    

    *aTupleExist = STL_FALSE;

    if( sIterator->mBackupIterator.mBackupChunkSeq < 0 )
    {
        STL_TRY( smfGetFirstIncBackup4Disk( &sIterator->mFile,
                                            &(sIterator->mSysPersData),
                                            &(sIterator->mBackupIterator),
                                            (stlChar *)sIterator->mBackupChunk,
                                            aTupleExist,
                                            SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smfGetNextIncBackup4Disk( &sIterator->mFile,
                                           &(sIterator->mSysPersData),
                                           &(sIterator->mBackupIterator),
                                           (stlChar *)sIterator->mBackupChunk,
                                           aTupleExist,
                                           SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    if( *aTupleExist == STL_TRUE )
    {
        sBackup.mBackupSeq = sIterator->mBackupSeq;
        sBackup.mLevel     = sIterator->mBackupIterator.mBackupRecord.mLevel;
        sBackup.mBackupLsn = sIterator->mBackupIterator.mBackupRecord.mBackupLsn;
        sBackup.mIsDeleted = sIterator->mBackupIterator.mBackupRecord.mIsDeleted;

        sIterator->mBackupSeq++;

        switch( sIterator->mBackupIterator.mBackupRecord.mObjectType )
        {
            case SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_DATABASE:
                stlStrcpy( sBackup.mObjectType, "database" );
                break;
            case SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_TABLESPACE:
                stlStrcpy( sBackup.mObjectType, "tablespace" );
                break;
            case SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_CONTROLFILE:
                stlStrcpy( sBackup.mObjectType, "control" );
                break;
            default:
                stlStrcpy( sBackup.mObjectType, "unknown" );
                break;
        }

        stlStrcpy( sBackup.mBackupOption,
                   (sBackup.mLevel == 0)? "N/A" :
                   ((sIterator->mBackupIterator.mBackupRecord.mBackupOption ==
                     SML_INCREMENTAL_BACKUP_OPTION_DIFFERENTIAL)?
                    "DIFFERENTIAL" : "CUMULATIVE") );

        stlStrcpy( sBackup.mFilePath, sIterator->mBackupIterator.mFileName );
        stlStrcpy( sBackup.mCtrlFileName, sIterator->mCtrlFileName );
        
        STL_TRY( knlBuildFxRecord( gIncBackupColumnDesc,
                                   &sBackup,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gDumpIncBackupTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    smfGetIncBackupListCallback,
    smfOpenIncBackupListCallback,
    smfCloseIncBackupListCallback,
    smfBuildRecordIncBackupListCallback,
    STL_SIZEOF( smfFxIncBackupIterator ),
    "D$INCREMENTAL_BACKUP_LIST",
    "incremental backup list",
    gIncBackupColumnDesc
};


/**
 * X$CONTROLFILE_BACKUP_SECTION
 */
knlFxColumnDesc gCtrlIncBackupSectionColumnDesc[] =
{
    {
        "BACKUP_SEQ",
        "backup sequence no",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxIncBackup, mBackupSeq ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "BACKUP_LEVEL",
        "incremental backup level",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smfFxIncBackup, mLevel ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "BACKUP_OBJECT",
        "backup object",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxIncBackup, mObjectType ),
        STL_MAX_FILE_NAME_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "BACKUP_OPTION",
        "backup option",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxIncBackup, mBackupOption ),
        12,
        STL_TRUE  /* nullable */
    },
    {
        "BACKUP_LSN",
        "backup Lsn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smfFxIncBackup, mBackupLsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "BACKUP_FILE",
        "backup file name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxIncBackup, mFilePath ),
        STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH + 1,
        STL_TRUE  /* nullable */
    },
    {
        "BEGIN_TIME",
        "backup beginning timestamp",
        KNL_FXTABLE_DATATYPE_TIMESTAMP,
        STL_OFFSETOF( smfFxIncBackup, mBeginTime ),
        STL_SIZEOF( stlTime ),
        STL_FALSE  /* nullable */
    },
    {
        "COMPLETION_TIME",
        "backup completion timestamp",
        KNL_FXTABLE_DATATYPE_TIMESTAMP,
        STL_OFFSETOF( smfFxIncBackup, mCompletionTime ),
        STL_SIZEOF( stlTime ),
        STL_FALSE  /* nullable */
    },
    {
        "CONTROLFILE_NAME",
        "control file name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxIncBackup, mCtrlFileName ),
        STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH + 1,
        STL_FALSE  /* nullable */
    },
    {
        "CONTROLFILE_STATE",
        "controlfile state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smfFxIncBackup, mCtrlFileState ),
        SMF_CTRL_STATE_LENGTH,
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

stlStatus smfGetCtrlIncBackupSection( void     * aDumpObjHandle,
                                      void     * aPathCtrl,
                                      stlBool  * aIsExist,
                                      stlBool  * aIsValid,
                                      knlEnv   * aEnv )
{
    smfFxCtrlPathCtrl  * sPathCtrl;
    stlBool              sIsExistFile;
    stlBool              sIsExistBackup = STL_FALSE;
    smfBackupChunkHdr  * sBackupChunkHdr;
    
    sPathCtrl = (smfFxCtrlPathCtrl *) aPathCtrl;

    sBackupChunkHdr = (smfBackupChunkHdr*) sPathCtrl->mBackupChunk;

    while( STL_TRUE )
    {
        /**
         * SysPersData의 Backup Chunk 개수와 Backup Chunk의 Backup Record 개수를 기준으로
         * 새로운 파일을 열어야 할지 판단한다.
         */ 
        if( (sBackupChunkHdr->mBackupCnt <= sPathCtrl->mBackupIterator.mBackupSlotSeq) &&
            ( (sPathCtrl->mSysPersData.mBackupChunkCnt - 1) <= sPathCtrl->mBackupIterator.mBackupChunkSeq ) )
        {
            
            STL_TRY( smfOpenCtrlFile( sPathCtrl,
                                      SMF_CTRL_SECTION_SYSTEM,
                                      &sIsExistFile,
                                      aIsValid,
                                      aEnv ) == STL_SUCCESS );

            if( sIsExistFile == STL_FALSE )
            {
                STL_THROW( RAMP_RETURN );
            }

            if( *aIsValid == STL_FALSE )
            {
                sIsExistBackup = STL_TRUE;
                sPathCtrl->mCurBackupSeq = -1;
                STL_THROW( RAMP_RETURN );
            }

            /**
             * Read system persistent data
             */
            STL_TRY( smfReadCtrlFile( &(sPathCtrl->mFile),
                                      &(sPathCtrl->mBuffer),
                                      (stlChar *) &(sPathCtrl->mSysPersData),
                                      STL_SIZEOF( smfSysPersData ),
                                      SML_ENV( aEnv ) ) == STL_SUCCESS );
            
            sPathCtrl->mCurBackupSeq = 0;

            SMF_INIT_BACKUP_ITERATOR( &(sPathCtrl->mBackupIterator), STL_FALSE );
        }

        if( sPathCtrl->mBackupIterator.mBackupChunkSeq < 0 )
        {
            STL_TRY( smfGetFirstIncBackup4Disk( &sPathCtrl->mFile,
                                                &(sPathCtrl->mSysPersData),
                                                &(sPathCtrl->mBackupIterator),
                                                (stlChar *)sPathCtrl->mBackupChunk,
                                                &sIsExistBackup,
                                                SML_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smfGetNextIncBackup4Disk( &sPathCtrl->mFile,
                                               &(sPathCtrl->mSysPersData),
                                               &(sPathCtrl->mBackupIterator),
                                               (stlChar *)sPathCtrl->mBackupChunk,
                                               &sIsExistBackup,
                                               SML_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

        /**
         * Delete Obsole Backup 이후 삭제돠니 Backup은 dump하지 않는다.
         */ 
        while( sPathCtrl->mBackupIterator.mBackupRecord.mIsDeleted == STL_TRUE )
        {
            STL_TRY( smfGetNextIncBackup4Disk( &sPathCtrl->mFile,
                                               &(sPathCtrl->mSysPersData),
                                               &(sPathCtrl->mBackupIterator),
                                               (stlChar *)sPathCtrl->mBackupChunk,
                                               &sIsExistBackup,
                                               SML_ENV( aEnv ) )
                     == STL_SUCCESS );

            sPathCtrl->mCurBackupSeq++;
        }
        
        if( sIsExistBackup == STL_TRUE ) 
        {
            break; 
        } 
        else 
        {
            STL_TRY_THROW( sPathCtrl->mCurCtrlFileId != (sPathCtrl->mCtrlFileCount - 1),
                           RAMP_RETURN );

            sPathCtrl->mBackupIterator.mBackupSlotSeq  = 0;
            sPathCtrl->mBackupIterator.mBackupChunkSeq = 0;
        } 

    }
    
    STL_RAMP( RAMP_RETURN );

    *aIsExist = sIsExistBackup;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfBuildRecordCtrlIncBackupListCallback( void              * aDumpObjHandle,
                                                   void              * aPathCtrl,
                                                   knlValueBlockList * aValueList,
                                                   stlInt32            aBlockIdx,
                                                   stlBool           * aTupleExist,
                                                   knlEnv            * aEnv )
{
    smfFxCtrlPathCtrl     * sPathCtrl;
    smfFxIncBackup          sBackupSection;
    stlBool                 sIsExist;
    stlBool                 sIsValid = STL_TRUE;
    
    sPathCtrl = (smfFxCtrlPathCtrl *) aPathCtrl;

    *aTupleExist = STL_FALSE;
    
    STL_TRY( smfGetCtrlIncBackupSection( aDumpObjHandle,
                                         aPathCtrl,
                                         &sIsExist,
                                         &sIsValid,
                                         aEnv ) == STL_SUCCESS );
    
    if( sIsExist == STL_FALSE )
    {
        *aTupleExist = STL_FALSE;
        STL_THROW( RAMP_RETURN );
    }

    stlMemset( (void *)&sBackupSection, 0x00, STL_SIZEOF( smfFxIncBackup ) );

    stlStrcpy( sBackupSection.mCtrlFileName, sPathCtrl->mCtrlFileName );
    
    if( sIsValid == STL_FALSE )
    {
        stlStrcpy( sBackupSection.mCtrlFileState, "CORRUPTED" );   
        STL_THROW( RAMP_BUILD_SUCCESS );
    }
    
    
    sBackupSection.mBackupSeq = sPathCtrl->mCurBackupSeq;
    sBackupSection.mLevel     = sPathCtrl->mBackupIterator.mBackupRecord.mLevel;
    sBackupSection.mBackupLsn = sPathCtrl->mBackupIterator.mBackupRecord.mBackupLsn;
    sBackupSection.mBeginTime = sPathCtrl->mBackupIterator.mBackupRecord.mBeginTime;
    sBackupSection.mCompletionTime = sPathCtrl->mBackupIterator.mBackupRecord.mCompletionTime;

    sPathCtrl->mCurBackupSeq++;

    switch( sPathCtrl->mBackupIterator.mBackupRecord.mObjectType )
    {
        case SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_DATABASE:
            stlStrcpy( sBackupSection.mObjectType, "database" );
            break;
        case SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_TABLESPACE:
            stlStrcpy( sBackupSection.mObjectType, "tablespace" );
            break;
        case SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_CONTROLFILE:
            stlStrcpy( sBackupSection.mObjectType, "control" );
            break;
        default:
            stlStrcpy( sBackupSection.mObjectType, "unknown" );
            break;
    }

    stlStrcpy( sBackupSection.mBackupOption,
               (sBackupSection.mLevel == 0)? "N/A" :
               ((sPathCtrl->mBackupIterator.mBackupRecord.mBackupOption ==
                 SML_INCREMENTAL_BACKUP_OPTION_DIFFERENTIAL)?
                "DIFFERENTIAL" : "CUMULATIVE") );

    stlStrcpy( sBackupSection.mFilePath, sPathCtrl->mBackupIterator.mFileName );
    stlStrcpy( sBackupSection.mCtrlFileState, "VALID" );   
    
    STL_RAMP( RAMP_BUILD_SUCCESS );

    STL_TRY( knlBuildFxRecord( gCtrlIncBackupSectionColumnDesc,
                               &sBackupSection,
                               aValueList,
                               aBlockIdx,
                               aEnv ) == STL_SUCCESS );

    *aTupleExist = STL_TRUE;
    
    STL_RAMP( RAMP_RETURN );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gCtrlIncBackupSectionTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    NULL,
    smfOpenCtrlCallback,
    smfCloseCtrlCallback,
    smfBuildRecordCtrlIncBackupListCallback,
    STL_SIZEOF( smfFxCtrlPathCtrl ),
    "X$CONTROLFILE_BACKUP_SECTION",
    "incremental backup list in current control file",
    gCtrlIncBackupSectionColumnDesc
};
   
/** @} */
