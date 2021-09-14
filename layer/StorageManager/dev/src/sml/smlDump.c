/*******************************************************************************
 * smlDump.c
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
#include <smf.h>
#include <smg.h>
#include <smr.h>
#include <smlDump.h>
#include <smlGeneral.h>

/**
 * @file smlDump.c
 * @brief Storage Manager Layer Dump Routines
 */

/**
 * @addtogroup smlDump
 * @{
 */

/**
 * @brief Control File을 Dump한다.
 * @param[in] aFilePath Control File의 전체 경로
 * @param[in] aSection  Dump Section을 print 여부 flag
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlDumpControlFile( stlChar * aFilePath,
                              stlInt8   aSection,
                              smlEnv  * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    if( (aSection & SML_DUMP_CTRLFILE_SECTION_SYS) ==
        SML_DUMP_CTRLFILE_SECTION_SYS )
    {
        STL_TRY( smfDumpSysCtrlSection( aFilePath,
                                        aEnv ) == STL_SUCCESS );
    }

    if( (aSection & SML_DUMP_CTRLFILE_SECTION_LOG) ==
        SML_DUMP_CTRLFILE_SECTION_LOG )
    {
        STL_TRY( smrDumpLogCtrlSection( aFilePath,
                                        aEnv ) == STL_SUCCESS );
    }
    
    if( (aSection & SML_DUMP_CTRLFILE_SECTION_DB) ==
        SML_DUMP_CTRLFILE_SECTION_DB )
    {
        STL_TRY( smfDumpDbCtrlSection( aFilePath,
                                       aEnv ) == STL_SUCCESS );
    }

    if( (aSection & SML_DUMP_CTRLFILE_SECTION_BACKUP) ==
        SML_DUMP_CTRLFILE_SECTION_BACKUP )
    {
        STL_TRY( smfDumpIncBackupCtrlSection( aFilePath,
                                              aEnv ) == STL_SUCCESS );
    }
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Log File을 Dump한다.
 * @param[in] aFilePath    Log File의 전체 경로
 * @param[in] aTargetLsn   Target Lsn
 * @param[in] aLsnOffset   Offset of Target Lsn
 * @param[in] aFetchCnt    Fetch Count of Log
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlDumpLogFile( stlChar * aFilePath,
                          stlInt64  aTargetLsn,
                          stlInt64  aLsnOffset,
                          stlInt64  aFetchCnt,
                          smlEnv  * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smrDumpLogFile( aFilePath,
                             aTargetLsn,
                             aLsnOffset,
                             aFetchCnt,
                             aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Datafile Header를 Dump한다.
 * @param[in] aFilePath Datafile의 전체 경로
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlDumpDatafileHeader( stlChar * aFilePath,
                                 smlEnv  * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smfDumpDatafileHeader( aFilePath,
                                    aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Datafile의 특정 Page를 Dump한다.
 * @param[in]     aFilePath  Datafile 경로
 * @param[in]     aPageSeqId Page Sequence Id
 * @param[in]     aFetchCnt  Page Fetch Count
 * @param[in,out] aEnv       Envrionment 정보
 */ 
stlStatus smlDumpDatafilePage( stlChar  * aFilePath,
                               stlInt64   aPageSeqId,
                               stlInt64   aFetchCnt,
                               smlEnv   * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smfDumpDatafilePage( aFilePath,
                                  aPageSeqId,
                                  aFetchCnt,
                                  aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Incremental Backup의 Header와 Tail을 Dump한다.
 * @param[in]     aFilePath  Backup File 경로
 * @param[in]     aDumpBody  Dump Body flag
 * @param[in]     aTbsId     Tablespace Id
 * @param[in]     aPageSeqId Page Sequence Id
 * @param[in]     aFetchCnt  Dump Body Fetch Count
 * @param[in,out] aEnv       Envrionment 정보
 */ 
stlStatus smlDumpIncBackupFile( stlChar  * aFilePath,
                                stlInt8    aDumpBody,
                                stlUInt16  aTbsId,
                                stlInt64   aPageSeqId,
                                stlInt64   aFetchCnt,
                                smlEnv   * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smfDumpIncBackupFile( aFilePath,
                                   aDumpBody,
                                   aTbsId,
                                   aPageSeqId,
                                   aFetchCnt,
                                   aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/** @} */
