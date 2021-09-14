/*******************************************************************************
 * smbDatabase.c
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
#include <smt.h>
#include <smr.h>
#include <sms.h>
#include <smxl.h>
#include <smq.h>
#include <smb.h>
#include <sma.h>
#include <smd.h>
#include <smlDatabase.h>
#include <smbDatabase.h>
#include <smbDef.h>

/**
 * @file smbDatabase.c
 * @brief Storage Manager Layer Database Routines
 */

/**
 * @addtogroup smb
 * @{
 */

/**
 * @brief No Mount 단계에서 데이터베이스 생성 관련 Action을 수행한다.
 * @param[in,out] aEnv Environment
 */
stlStatus smbCreateDatabaseAtNoMount( smlEnv * aEnv )
{
    STL_TRY( smfCreateCtrlFile( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Mount 단계에서 데이터베이스 생성 관련 Action을 수행한다.
 * @param[in,out] aEnv Environment
 */
stlStatus smbCreateDatabaseAtMount( smlEnv * aEnv )
{
    stlInt64           sLogBlockSize;
    stlInt64           sLogFileSize;
    stlInt32           i;
    smlLogStreamAttr   sLogStreamAttr;
    smlLogGroupAttr  * sLogGroupAttr;
    smlLogMemberAttr * sLogMemberAttr;
    stlChar            sLogDir[KNL_PROPERTY_STRING_MAX_LENGTH];
    stlInt64           sLogGroupCount;
    knlRegionMark      sMemMark;
    stlInt32           sState = 0;

    sLogBlockSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_BLOCK_SIZE,
                                                   KNL_ENV( aEnv ) );
    sLogFileSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_SIZE,
                                                  KNL_ENV( aEnv ) );
    sLogGroupCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_GROUP_COUNT,
                                                    KNL_ENV( aEnv ) );

    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_LOG_DIR,
                                      sLogDir,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /* LOG_BLOCK_SIZE가 suitable한지 체크 */
    STL_TRY_THROW( smrIsSuitableLogBlockSize( sLogBlockSize ) == STL_TRUE,
                   RAMP_ERR_UNSUITABLE_LOG_BLOCK_SIZE );

    STL_TRY( smrAdjustGlobalVariables( &sLogBlockSize, aEnv ) == STL_SUCCESS );

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sLogGroupCount * STL_SIZEOF( smlLogGroupAttr ),
                                (void*)&sLogGroupAttr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sLogGroupCount * STL_SIZEOF( smlLogMemberAttr ),
                                (void*)&sLogMemberAttr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    for( i = 0; i < sLogGroupCount; i++ )
    {
        if( i > 0 )
        {
            sLogGroupAttr[i-1].mNext = &sLogGroupAttr[i];
        }
            
        sLogGroupAttr[i].mValidFlags = ( SML_LOG_GROUP_GROUPID_YES |
                                         SML_LOG_GROUP_FILESIZE_YES );
        sLogGroupAttr[i].mLogGroupId = i;
        sLogGroupAttr[i].mFileSize = sLogFileSize;
        sLogGroupAttr[i].mNext = NULL;
        sLogGroupAttr[i].mLogMemberAttr = &sLogMemberAttr[i];
            
        sLogMemberAttr[i].mValidFlags = ( SML_LOG_MEMBER_NAME_YES );
        stlSnprintf( sLogMemberAttr[i].mName,
                     STL_MAX_FILE_PATH_LENGTH,
                     "%s" STL_PATH_SEPARATOR "redo_%d_0.log",
                     sLogDir,
                     i );
        sLogMemberAttr[i].mNext = NULL;
    }

    sLogStreamAttr.mValidFlags = ( SML_LOG_STREAM_STREAMID_YES |
                                   SML_LOG_STREAM_BLOCKSIZE_YES );
    sLogStreamAttr.mBlockSize = sLogBlockSize;
    sLogStreamAttr.mLogGroupAttr = sLogGroupAttr;
        
    STL_TRY( smrCreateLogStream( &sLogStreamAttr, aEnv ) == STL_SUCCESS );

    STL_TRY( smgExecutePendOp( SML_PEND_ACTION_COMMIT,
                               0, 
                               aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_RAMP( RAMP_ERR_UNSUITABLE_LOG_BLOCK_SIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_UNSUITABLE_LOG_BLOCK_SIZE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sLogBlockSize );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                        &sMemMark,
                                        STL_FALSE, /* aFreeChunk */
                                        (knlEnv*)aEnv );
    }
    
    (void) smgExecutePendOp( SML_PEND_ACTION_ROLLBACK, 0, aEnv );

    return STL_FAILURE;
}

/**
 * @brief Open 단계에서 데이터베이스 생성 관련 Action을 수행한다.
 * @param[in,out] aEnv Environment
 */
stlStatus smbCreateDatabaseAtOpen( smlEnv * aEnv )
{
    smxlTransId    sTransId;
    stlUInt64      sTransSeq;
    stlInt32       sState = 0;
    smlStatement * sStatement;
    stlInt64       sArchivelogMode;

    /**
     * DB Create시 Archivelog Mode Property를 읽어서 database persisten data에 저장한다.
     */
    sArchivelogMode = knlGetPropertyBigIntValueByID( KNL_PROPERTY_ARCHIVELOG_MODE,
                                                     KNL_ENV( aEnv ) );

    smrSetArchivelogMode( sArchivelogMode );

    STL_TRY( smxlAllocTrans( SML_TIL_READ_COMMITTED,
                             STL_FALSE, /* aIsGlobalTrans */
                             &sTransId,
                             &sTransSeq,
                             aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smaAllocStatement( sTransId,
                                SMXL_TO_SML_TRANS_ID( sTransId, sTransSeq ),
                                NULL, /* aRelationHandle */
                                ( SML_STMT_ATTR_UPDATABLE |
                                  SML_STMT_ATTR_LOCKABLE ),
                                SML_LOCK_TIMEOUT_INFINITE,
                                STL_FALSE, /* aNeedSnapshotIterator */
                                &sStatement,
                                aEnv )
             == STL_SUCCESS );
    sState = 2;
    
    /*
     * CREATE SYSTEM TABLESPACES
     */
    STL_TRY( smbCreateSystemTablespace( sStatement,
                                        KNL_PROPERTY_SYSTEM_MEMORY_DICT_TABLESPACE_SIZE,
                                        SML_MEMORY_DICT_SYSTEM_TBS_NAME,
                                        ( SML_TBS_DEVICE_MEMORY |
                                          SML_TBS_TEMPORARY_NO  |
                                          SML_TBS_USAGE_DICT ),
                                        SMB_DEFAULT_SYSTEM_MEMORY_DICT_TBS_EXTSIZE,
                                        SMB_DEFAULT_SYSTEM_MEMORY_DICT_TBS_DATAFILE_NAME,
                                        aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smbCreateSystemTablespace( sStatement,
                                        KNL_PROPERTY_SYSTEM_MEMORY_UNDO_TABLESPACE_SIZE,
                                        SML_MEMORY_UNDO_SYSTEM_TBS_NAME,
                                        ( SML_TBS_DEVICE_MEMORY |
                                          SML_TBS_TEMPORARY_NO  |
                                          SML_TBS_USAGE_UNDO ),
                                        SMB_DEFAULT_SYSTEM_MEMORY_UNDO_TBS_EXTSIZE,
                                        SMB_DEFAULT_SYSTEM_MEMORY_UNDO_TBS_DATAFILE_NAME,
                                        aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smbCreateSystemTablespace( sStatement,
                                        KNL_PROPERTY_SYSTEM_MEMORY_DATA_TABLESPACE_SIZE,
                                        SML_MEMORY_DATA_SYSTEM_TBS_NAME,
                                        ( SML_TBS_DEVICE_MEMORY |
                                          SML_TBS_TEMPORARY_NO  |
                                          SML_TBS_USAGE_DATA ),
                                        SMB_DEFAULT_SYSTEM_MEMORY_DATA_TBS_EXTSIZE,
                                        SMB_DEFAULT_SYSTEM_MEMORY_DATA_TBS_DATAFILE_NAME,
                                        aEnv )
             == STL_SUCCESS );

    STL_TRY( smbCreateSystemTablespace( sStatement,
                                        KNL_PROPERTY_SYSTEM_MEMORY_TEMP_TABLESPACE_SIZE,
                                        SML_MEMORY_TEMP_SYSTEM_TBS_NAME,
                                        ( SML_TBS_DEVICE_MEMORY |
                                          SML_TBS_TEMPORARY_YES | 
                                          SML_TBS_USAGE_TEMPORARY ),
                                        SMB_DEFAULT_SYSTEM_MEMORY_TEMP_TBS_EXTSIZE,
                                        SMB_DEFAULT_SYSTEM_MEMORY_TEMP_TBS_CHUNK_NAME,
                                        aEnv )
             == STL_SUCCESS );

    /*
     * EXECUTE PENDING OPERATION
     */
    STL_TRY( smgExecutePendOp( SML_PEND_ACTION_COMMIT,
                               sStatement->mTimestamp, 
                               aEnv ) == STL_SUCCESS );

    /*
     * CREATE CATALOG TABLE
     */
    STL_TRY( smsMapCreate( sTransId, aEnv ) == STL_SUCCESS );

    STL_TRY( smqMapCreate( sStatement, aEnv ) == STL_SUCCESS );

    STL_TRY( smbCreateFirstDictRelation( sStatement, aEnv ) == STL_SUCCESS );

    sState = 1;
    STL_TRY( smaFreeStatement( sStatement, aEnv ) == STL_SUCCESS );

    STL_TRY( smxlCreateUndoRelations( sTransId,
                                      aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smxlCommit( sTransId,
                         NULL, /* aComment */
                         SML_TRANSACTION_CWM_WAIT,
                         aEnv ) == STL_SUCCESS );
    STL_TRY( smxlFreeTrans( sTransId,
                            SML_PEND_ACTION_COMMIT,
                            STL_FALSE, /* aCleanup */
                            SML_TRANSACTION_CWM_WAIT,
                            aEnv ) == STL_SUCCESS );
    
    smfSetServerState( SML_SERVER_STATE_SERVICE );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smaRollbackStatement( sStatement, STL_TRUE, aEnv );
            (void) smaFreeStatement( sStatement, aEnv );
        case 1:
            (void) smxlRollback( sTransId,
                                 SML_TRANSACTION_CWM_NO_WAIT,
                                 STL_FALSE, /* aCleanup */
                                 aEnv );
            (void) smxlFreeTrans( sTransId,
                                  SML_PEND_ACTION_ROLLBACK,
                                  STL_FALSE, /* aCleanup */
                                  SML_TRANSACTION_CWM_NO_WAIT,
                                  aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief 데이터베이스를 삭제한다.
 * @param[in,out] aEnv Environment
 * @note 데이터베이스 삭제는 Service 단계에서 이루어져야 한다.
 */
stlStatus smbDropDatabase( smlEnv * aEnv )
{
    smxlTransId    sTransId;
    stlUInt64      sTransSeq;
    stlInt32       sState = 0;
    smlTbsId       sTbsId;
    stlBool        sExist;
    smlStatement * sStatement;
    
    STL_TRY( smxlAllocTrans( SML_TIL_READ_COMMITTED,
                             STL_FALSE, /* aIsGlobalTrans */
                             &sTransId,
                             &sTransSeq,
                             aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smaAllocStatement( sTransId,
                                SMXL_TO_SML_TRANS_ID( sTransId, sTransSeq ),
                                NULL, /* aRelationHandle */
                                ( SML_STMT_ATTR_UPDATABLE |
                                  SML_STMT_ATTR_LOCKABLE ),
                                SML_LOCK_TIMEOUT_INFINITE,
                                STL_FALSE, /* aNeedSnapshotIterator */
                                &sStatement,
                                aEnv )
             == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( smfFirstTbs( &sTbsId,
                          &sExist,
                          aEnv ) == STL_SUCCESS );
                 
    while( sExist == STL_TRUE )
    {
        /**
         * AGING 상태의 Tablespace는 삭제할 필요가 없다.
         * - Refine 단계에서 삭제된다.
         */
        if( (smfGetTbsState(sTbsId) & SMF_TBS_STATE_AGING) !=
            SMF_TBS_STATE_AGING )
        {
            STL_TRY( smtDropTbs( sStatement,
                                 sTbsId,
                                 STL_TRUE,  /* aAndDatafiles */
                                 aEnv ) == STL_SUCCESS );
        }
    
        STL_TRY( smfNextTbs( &sTbsId,
                             &sExist,
                             aEnv ) == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( smaFreeStatement( sStatement, aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smxlCommit( sTransId,
                         NULL, /* aComment */
                         SML_TRANSACTION_CWM_WAIT,
                         aEnv ) == STL_SUCCESS );
    STL_TRY( smxlFreeTrans( sTransId,
                            SML_PEND_ACTION_COMMIT,
                            STL_FALSE, /* aCleanup */
                            SML_TRANSACTION_CWM_WAIT,
                            aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smaRollbackStatement( sStatement, STL_TRUE, aEnv );
            (void) smaFreeStatement( sStatement, aEnv );
        case 1:
            (void) smxlRollback( sTransId,
                                 SML_TRANSACTION_CWM_NO_WAIT,
                                 STL_FALSE, /* aCleanup */
                                 aEnv );
            (void) smxlFreeTrans( sTransId,
                                  SML_PEND_ACTION_ROLLBACK,
                                  STL_FALSE, /* aCleanup */
                                  SML_TRANSACTION_CWM_NO_WAIT,
                                  aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/** @} */

/**
 * @addtogroup smbDatabase
 * @{
 */

stlStatus smbCreateFirstDictRelation( smlStatement * aStatement,
                                      smlEnv       * aEnv )
{
    smlTableAttr        sAttr;
    stlInt64            sSegmentId;
    void              * sRelationHandle;
    smdTableModule    * sModule;

    sModule = smdGetTableModule( SML_GET_TABLE_TYPE(SML_MEMORY_PERSISTENT_HEAP_ROW_TABLE) );
    STL_ASSERT( sModule != NULL );

    sAttr.mValidFlags = ( SML_TABLE_LOGGING_YES |
                          SML_TABLE_LOCKING_YES |
                          SML_TABLE_TYPE_MEMORY_PERSISTENT );
    sAttr.mLoggingFlag = STL_TRUE;
    sAttr.mLockingFlag = STL_FALSE;
    sAttr.mSegAttr.mValidFlags = 0;

    STL_TRY( sModule->mCreate( aStatement,
                               SML_MEMORY_DICT_SYSTEM_TBS_ID,
                               &sAttr,
                               STL_FALSE,   /* aLoggingUndo */
                               &sSegmentId,
                               &sRelationHandle,
                               aEnv) == STL_SUCCESS );

    STL_ASSERT( sSegmentId == smlGetFirstDicTableId() );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smbCreateSystemTablespace( smlStatement  * aStatement,
                                     knlPropertyID   aPropertyId,
                                     stlChar       * aName,
                                     stlInt32        aAttr,
                                     stlInt64        aExtSize,
                                     stlChar       * aDatafileName,
                                     smlEnv        * aEnv )
{
    smlTbsAttr       sTbsAttr;
    smlDatafileAttr  sDatafileAttr;
    stlInt64         sSysTbsSize;
    smlTbsId         sTbsId;
    stlInt32         sReservedPageCount = 0;
    stlChar          sSystemTbsDir[KNL_PROPERTY_STRING_MAX_LENGTH];
    stlChar          sSystemTbsPath[STL_MAX_FILE_PATH_LENGTH];
    stlInt32         sNewPathLen;
    
    sSysTbsSize = knlGetPropertyBigIntValueByID( aPropertyId,
                                                 KNL_ENV( aEnv ) );

    stlStrncpy( sTbsAttr.mName, aName, STL_MAX_SQL_IDENTIFIER_LENGTH );

    if( (aAttr & SML_TBS_TEMPORARY_MASK) == SML_TBS_TEMPORARY_NO )
    {
        STL_TRY( knlGetPropertyValueByName( "SYSTEM_TABLESPACE_DIR",
                                            sSystemTbsDir,
                                            KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( stlMergeAbsFilePath( sSystemTbsDir,
                                      aDatafileName,
                                      sSystemTbsPath,
                                      STL_MAX_FILE_PATH_LENGTH,
                                      &sNewPathLen,
                                      KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
                               
        sTbsAttr.mValidFlags = (SML_TBS_EXTSIZE_YES | SML_TBS_ATTR_YES);
        sTbsAttr.mAttr = aAttr;
        sTbsAttr.mExtSize = aExtSize;
        sTbsAttr.mDatafileAttr = &sDatafileAttr;

        sDatafileAttr.mValidFlags = (SML_DATAFILE_NAME_YES | SML_DATAFILE_SIZE_YES);
        stlStrcpy( sDatafileAttr.mName, aDatafileName );
        sDatafileAttr.mSize = sSysTbsSize;
        sDatafileAttr.mNext = NULL;
    }
    else
    {
        sTbsAttr.mValidFlags = ( SML_TBS_EXTSIZE_YES |
                                 SML_TBS_ATTR_YES    |
                                 SML_TBS_LOGGING_YES );
        sTbsAttr.mIsLogging = STL_FALSE;
        sTbsAttr.mAttr = aAttr;
        sTbsAttr.mExtSize = aExtSize;
        sTbsAttr.mDatafileAttr = &sDatafileAttr;
        
        stlStrcpy( sDatafileAttr.mName, aDatafileName );
        sDatafileAttr.mValidFlags = SML_DATAFILE_SIZE_YES;
        sDatafileAttr.mSize = sSysTbsSize;
        sDatafileAttr.mNext = NULL;
    }

    if( aPropertyId == KNL_PROPERTY_SYSTEM_MEMORY_DICT_TABLESPACE_SIZE )
    {
        sReservedPageCount = 1;
    }
    
    STL_TRY( smtCreateTbs( aStatement,
                           &sTbsAttr,
                           sReservedPageCount,
                           STL_FALSE,   /* aUndoLogging */
                           &sTbsId,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
