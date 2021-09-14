/*******************************************************************************
 * ztcc.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZTCC_H_
#define _ZTCC_H_ 1

/**
 * @file ztcc.h
 * @brief GlieseTool Cyclone Capture Routines
 */

/**
 * @defgroup ztcc Cyclone Capture Routines
 * @{
 */

/**
 * Capture
 */

stlStatus ztccCaptureLogFile( stlChar          * aFilePath,
                              ztcCaptureInfo   * aCaptureInfo,
                              stlInt16           aWrapNo,
                              ztcCaptureStatus * aCaptureStatus,
                              stlBool            aRecovery,
                              stlErrorStack    * aErrorStack );

stlStatus ztccCapture( ztcLogFileInfo * aLogFileInfo,
                       stlErrorStack  * aErrorStack );

stlStatus ztccWakeUpCapture( stlErrorStack * aErrorStack );

stlStatus ztccSaveRestartInfo( ztcCaptureProgress   * aCurProgress,
                               stlErrorStack        * aErrorStack );

stlInt16  ztccIsValidLog( ztcLogCursor   * aLogCursor,
                          ztcLogBlockHdr * aLogBlockHdr );

stlStatus ztccCloseLogCursor( ztcLogCursor  * aLogCursor,
                              stlErrorStack * aErrorStack );

stlStatus ztccReadLogBlock( ztcLogCursor  * aLogCursor,
                            stlChar      ** aLogBlock,
                            stlInt32      * aRetType,
                            stlErrorStack * aErrorStack );

stlStatus ztccOpenLogCursor( ztcLogCursor  * aLogCursor,
                             stlInt32        aBlockSeq,
                             stlChar       * aFilePath,
                             stlInt32      * aRetType,
                             stlErrorStack * aErrorStack );

stlStatus ztccGetLogFileWrapNo( stlChar       * aFileName,
                                stlInt16      * aWrapNo,
                                stlErrorStack * aErrorStack );
                                
stlStatus ztccGetLogFileSeqNoNGroupId( stlChar       * aFileName,
                                       stlInt64      * aFileSeqNo,
                                       stlInt16      * aGroupId,
                                       stlErrorStack * aErrorStack );

stlStatus ztccValidateArchiveLogFile( stlChar        * aFileName,
                                      ztcLogFileInfo * aLogFileInfo,
                                      stlErrorStack  * aErrorStack );

stlStatus ztccCheckCaptureStatus( stlErrorStack * aErrorStack );

stlStatus ztccGetIntervalInfo( stlInt16        aGroupId,
                               stlInt64        aFileSeqNo,
                               stlInt32        aBlockSeq,
                               stlInt64      * aInterval,
                               stlErrorStack * aErrorStack );

stlStatus ztccGetLogFileBlockSeq( stlChar          * aFilePath,
                                  ztcCaptureInfo   * aCaptureInfo,
                                  stlInt64           aBeginLsn,
                                  stlInt16           aWrapNo,
                                  stlBool          * aFound,
                                  ztcCaptureStatus * aCaptureStatus,
                                  stlErrorStack    * aErrorStack );

void      ztccCheckPrimaryKeyUpdated( ztcCaptureTableInfo * aTableInfo,
                                      stlInt64              aTransId,
                                      stlChar             * aData );

/**
 * Transaction Management
 */

stlStatus ztccInitializeCapture( stlErrorStack * aErrorStack );

stlStatus ztccFinalizeCapture( stlErrorStack * aErrorStack );

stlStatus ztccRemoveListAtSlot( ztcTransSlotHdr * aTransSlotHdr,
                                stlErrorStack   * aErrorStack );

stlStatus ztccAddRestartInfo( ztcCaptureProgress * aProgress,
                              ztcRestartInfo     * aRestartInfo,
                              stlErrorStack      * aErrorStack );

stlStatus ztccRemoveRestartInfo( ztcRestartInfo * aRestartInfo,
                                 stlErrorStack  * aErrorStack );

stlStatus ztccOperateTransBegin( stlInt64              aTransId,
                                 ztcCaptureProgress  * aProgress,
                                 stlBool               aPropagate,
                                 stlErrorStack       * aErrorStack );

stlStatus ztccOperateTransCommit( stlChar            * aData,
                                  stlInt64             aTransId,
                                  stlUInt64            aCommitSCN,
                                  ztcCaptureProgress * aProgress,
                                  stlErrorStack      * aErrorStack );

stlStatus ztccOperateTransRollback( stlChar            * aData,
                                    stlInt64             aTransId,
                                    stlUInt64            aCommitSCN,
                                    ztcCaptureProgress * aProgress,
                                    stlErrorStack      * aErrorStack );

stlStatus ztccOperateTransaction( stlChar             * aData,
                                  stlInt64              aLSN,
                                  stlInt64              aRecordId,
                                  stlInt64              aTransId,
                                  ztcCaptureTableInfo * aTableInfo,
                                  stlUInt16             aLogType,
                                  stlUInt16             aSize,
                                  stlErrorStack       * aErrorStack );

stlStatus ztccOperateTransUndo( stlInt64        aRecordId,
                                stlInt64        aTransId,
                                stlUInt16       aLogType,
                                stlErrorStack * aErrorStack );

stlStatus ztccAddTransDataToSlot( ztcTransSlotHdr * aTransSlotHdr,
                                  ztcTransHdr     * aTransHdr,
                                  stlChar         * aData,
                                  stlUInt16         aSize,
                                  stlErrorStack   * aErrorStack );
                                   
stlStatus ztccRemoveTransDataAtSlot( ztcTransSlotHdr * aTransSlotHdr,
                                     stlInt64          aRecordId,
                                     stlUInt16         aLogType,
                                     stlErrorStack   * aErrorStack );

stlStatus ztccOperateDDL( stlChar             * aData,
                          stlInt64              aLSN,
                          stlInt64              aRecordId,
                          stlInt64              aTransId,
                          ztcCaptureTableInfo * aTableInfo,
                          stlUInt16             aSize,
                          stlErrorStack       * aErrorStack );
                                   
stlStatus ztccOperateDDLCLR( stlChar             * aData,
                             stlInt64              aTransId,
                             ztcCaptureTableInfo * aTableInfo,
                             stlUInt16             aSize,
                             stlErrorStack       * aErrorStack );

stlStatus ztccUpdateTableCommitScn4DDL( stlInt64        aTransId,
                                        stlErrorStack * aErrorStack );

void      ztccInitializeSlotHdr( ztcTransSlotHdr * aTransSlotHdr );

stlStatus ztccRebuildTransData( ztcTransSlotHdr * aTransSlotHdr,
                                stlErrorStack   * aErrorStack );

/**
 * Chunk
 */

stlStatus ztccWriteCommittedTransToChunk( ztcTransSlotHdr * aTransSlotHdr,
                                          stlUInt64         aCommitSCN,
                                          stlInt32          aCommentLength,
                                          stlChar         * aComment,
                                          stlErrorStack   * aErrorStack );

                           
                           
/**
 * Capture Recovery...
 */

stlStatus ztccDummyFunc( stlInt64   aLSN,
                         stlBool  * aStatus );

/**
 * Transaction Chunk...
 */

stlStatus ztccWriteTransDataToChunk( ztcTransSlotHdr * aTransSlotHdr,
                                     ztcTransHdr     * aTransHdr,
                                     stlChar         * aData,
                                     stlUInt16         aSize,
                                     stlErrorStack   * aErrorStack );
                                     
stlStatus ztccRemoveTransDataFromChunk( ztcTransSlotHdr * aTransSlotHdr,
                                        ztcTransData    * aTransData,
                                        stlErrorStack   * aErrorStack );

stlStatus ztccWriteTransDataToFile( stlErrorStack * aErrorStack );

/** @} */


/** @} */

#endif /* _ZTCC_H_ */


