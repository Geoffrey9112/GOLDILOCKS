/*******************************************************************************
 * smlDatabase.c
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
#include <smb.h>
#include <smf.h>
#include <sma.h>
#include <smd.h>
#include <smxl.h>
#include <smr.h>
#include <smlGeneral.h>

/**
 * @file smlDatabase.c
 * @brief Storage Manager Layer Database Routines
 */

/**
 * @addtogroup smlDatabase
 * @{
 */

/**
 * @brief 단계별로 데이터베이스 생성 관련 Action을 수행한다.
 * @param[in] aPhase Startup Phase
 * @param[in,out] aEnv Environment
 */
stlStatus smlCreateDatabase( knlStartupPhase   aPhase,
                             smlEnv          * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    switch( aPhase )
    {
        case KNL_STARTUP_PHASE_NO_MOUNT:
            STL_TRY( smbCreateDatabaseAtNoMount( aEnv ) == STL_SUCCESS );
            break;
        case KNL_STARTUP_PHASE_MOUNT:
            STL_TRY( smbCreateDatabaseAtMount( aEnv ) == STL_SUCCESS );
            break;
        case KNL_STARTUP_PHASE_OPEN:
            STL_TRY( smbCreateDatabaseAtOpen( aEnv ) == STL_SUCCESS );
            break;
        default:
            STL_ASSERT( 0 );
            break;
    }
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 데이터베이스를 제거한다.
 * @param[in,out] aEnv Environment
 */
stlStatus smlDropDatabase( smlEnv * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smbDropDatabase( aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Create Database 의 마무리 작업을 수행한다.
 * @param[in,out] aEnv Environment
 */
stlStatus smlCompleteCreateDatabase( smlEnv * aEnv )
{
    smxlTransId    sTransId;
    stlUInt64      sTransSeq;
    stlInt32       sState = 0;
    smlStatement * sStatement;

    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

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

    /**
     * Pending relation을 생성한다.
     */
    
    STL_TRY( smdCreatePendingRelation( sStatement,
                                       SML_MEMORY_DATA_SYSTEM_TBS_ID,
                                       aEnv )
             == STL_SUCCESS );
    
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
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
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

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 첫번째 Dictionary Table Identifier를 얻는다.
 */
stlInt64 smlGetFirstDicTableId()
{
    smlRid   sFirstDictTableRid;
    stlInt64 sFirstDictTableId;
    
    sFirstDictTableRid = SMB_FIRST_DICTIONARY_TABLE_ID;

    stlMemcpy( (void*)&sFirstDictTableId,
               (void*)&sFirstDictTableRid,
               STL_SIZEOF( smlRid ) );

    return sFirstDictTableId;
}

/**
 * @brief Archivelog Mode 여부를 구한다.
 * @return Archivelog Mode
 */
inline stlBool smlIsArchivelogMode()
{
    return (smrGetArchivelogMode() == SML_ARCHIVELOG_MODE);
}

/**
 * @brief Database Backup을 수행한다.
 * @param[in] aBackupMsg Backup 정보
 * @param[in,out] aEnv Environment
 */
stlStatus smlBackup( smlBackupMessage * aBackupMsg,
                     smlEnv           * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    STL_TRY( smfBackup( aBackupMsg, aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Obsolete Increment Backup List를 삭제한다.
 * @param[in] aTarget 삭제할 Backup List 범위(ALL or OBSOLETE)
 * @param[in] aDeleteFile Backup List와 함께 Backup File을 삭제할지 여부
 * @param[in,out] aEnv Environment
 */
stlStatus smlDeleteBackupList( stlChar    aTarget,
                               stlBool    aDeleteFile,
                               smlEnv   * aEnv )
{
    stlBool   sDeleteFile = aDeleteFile;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    if( sDeleteFile == STL_FALSE )
    {
        /**
         * Backup list 삭제 시 backup file 삭제를 지정하지 않으면
         * DEFAULT_REMOVAL_BACKUP_FILE property를 적용한다.
         */
        sDeleteFile = knlGetPropertyBoolValueByID(
            KNL_PROPERTY_DEFAULT_REMOVAL_BACKUP_FILE,
            KNL_ENV( aEnv ) );
    }

    STL_TRY( smfDeleteBackupList( aTarget,
                                  sDeleteFile,
                                  aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/** @} */
