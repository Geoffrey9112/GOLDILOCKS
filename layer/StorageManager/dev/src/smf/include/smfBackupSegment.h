/*******************************************************************************
 * smfBackupSegment.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smfBackupSegment.h 
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _SMFBACKUPSEGMENT_H_
#define _SMFBACKUPSEGMENT_H_ 1

/**
 * @file smfBackupSegment.h
 * @brief Storage Manager Layer Backup Segment Manager Component Internal Routines
 */

/**
 * @defgroup smfBackupSegmentMgr Backup Segment Manager
 * @ingroup smfInternal
 * @{
 */

stlStatus smfAllocBackupChunk( smlEnv * aEnv );
stlStatus smfTruncateBackupSegment( stlInt64   aDeleteCnt,
                                    smlEnv   * aEnv );

smfBackupChunk * smfGetCurrBackupChunk( );
smfBackupChunk * smfGetFirstBackupChunk( smfBackupSegment * aBackupSegment );
smfBackupChunk * smfGetNextBackupChunk( smfBackupSegment * aBackupSegment,
                                        smfBackupChunk   * aCurrBackupChunk );
smfBackupChunk * smfGetPrevBackupChunk( smfBackupSegment * aBackupSegment,
                                        smfBackupChunk   * aCurrBackupChunk );

void smfAllocBackupSlot( smfBackupChunk   * aBackupChunk,
                         stlSize            aFileNameSize,
                         void            ** aBackupSlot );

void smfWriteBackupRecord( smfBackupRecord   * aBackupRecord,
                           smfBackupChunk    * aBackupChunk,
                           stlChar           * aBackupFileName,
                           stlSize             aFileNameSize,
                           stlTime             aBeginTime,
                           stlTime             aCompletionTime,
                           smrLsn              aBackupLsn,
                           smrLid              aBackupLid,
                           smlTbsId            aTbsId,
                           stlInt16            aLevel,
                           stlUInt8            aBackupObjectType,
                           stlUInt8            aBackupOption );

stlStatus smfFindLatestLevel0Backup( stlInt64  * aSegId,
                                     stlInt32  * aSlotId,
                                     smlEnv    * aEnv );

stlStatus smfDeleteObsoleteBackupList( stlInt64 * aEndSegId,
                                       stlInt32 * aEndSlotId,
                                       smlEnv   * aEnv );

stlStatus smfDeleteBackupFile( stlFile        * aFile,
                               smfSysPersData * aSysPersData,
                               stlInt32         aEndChunkId,
                               stlInt32         aEndRecordId,
                               smlEnv         * aEnv );

/** @} */
#endif /* _SMFBACKUPSEGMENT_H_ */
