/*******************************************************************************
 * ztrs.h
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


#ifndef _ZTRS_H_
#define _ZTRS_H_ 1

/**
 * @file ztrs.h
 * @brief GlieseTool LogMirror Sender Routines
 */


/**
 * @defgroup ztrs LogMirror Sender Routines
 * @{
 */

stlStatus ztrsInitializeSender( stlErrorStack * aErrorStack );

stlStatus ztrsFinalizeSender( stlErrorStack * aErrorStack );

stlStatus ztrsSenderMain( stlErrorStack * aErrorStack );

stlStatus ztrsCreateListener( stlErrorStack * aErrorStack );

stlStatus ztrsCreateListener4IB( stlErrorStack * aErrorStack );

/**
 * Communicate
 */
stlStatus ztrsDoProtocol( stlErrorStack * aErrorStack );

stlStatus ztrsInitProtocol( stlErrorStack * aErrorStack );

stlStatus ztrsHandShakeProtocol( ztrRecoveryinfo * aRecoveryInfo,
                                 stlBool         * aDoRecovery,
                                 stlErrorStack   * aErrorStack );

stlStatus ztrsResponseBufferInfo( stlErrorStack * aErrorStack );

stlStatus ztrsRequestRecoveryInfoProtocol( ztrRecoveryinfo * aRecoveryInfo,
                                           stlBool         * aDoRecovery,
                                           stlErrorStack   * aErrorStack );

stlStatus ztrsRecoveryProtocol( stlBool         aIsLogFileHdr,
                                stlErrorStack * aErrorStack );

stlStatus ztrsSendRedoLogProtocol( stlErrorStack * aErrorStack );

stlStatus ztrsCheckSlaveState( void          * aHandle,
                               stlErrorStack * aErrorStack );

stlStatus ztrsCheckSlaveState4IB( void          * aHandle,
                                  stlErrorStack * aErrorStack );

stlStatus ztrsEndProtocol( void          * aHandle,
                           stlErrorStack * aErrorStack );

/**
 * Recovery
 */

stlStatus ztrsInitializeRecovery( ztrRecoveryinfo * aRecoveryInfo,
                                  stlErrorStack   * aErrorStack );

stlStatus ztrsFinalizeRecovery( stlErrorStack * aErrorStack );

stlStatus ztrsDoRecovery( ztrRecoveryinfo * aRecoveryInfo,
                          stlErrorStack   * aErrorStack );

stlStatus ztrsBuildLogFileInfo( ztrRecoveryinfo * aRecoveryInfo,
                                stlErrorStack   * aErrorStack );

stlStatus ztrsBuildArchiveInfo( stlErrorStack * aErrorStack );

stlStatus ztrsGetCurrentLogFileInfo( ztrLogFileInfo ** aLogFileInfo,
                                     stlErrorStack   * aErrorStack );

stlStatus ztrsGetNextLogFileInfo( ztrLogFileInfo ** aLogFileInfo,
                                  stlErrorStack   * aErrorStack );

stlStatus ztrsGetLogFileSeqNoNGroupId( stlChar        * aFileName,
                                       stlInt64       * aFileSeqNo,
                                       stlInt16       * aGroupId,
                                       stlErrorStack  * aErrorStack );

stlStatus ztrsGetArchiveLogFile( stlChar        * aFileName,
                                 ztrLogFileInfo * aLogFileInfo,
                                 ztrRecoveryMgr * aRecoveryMgr,
                                 stlErrorStack  * aErrorStack );

stlStatus ztrsValidateArchiveLogFile( stlChar        * aFileName,
                                      ztrLogFileInfo * aLogFileInfo,
                                      stlErrorStack  * aErrorStack );

stlStatus ztrsReadLogFile( stlChar        * aLogFileName,
                           ztrLogFileInfo * aLogFileInfo,
                           ztrRecoveryMgr * aRecoveryMgr,
                           stlInt32       * aResult,
                           stlErrorStack  * aErrorStack );

stlStatus ztrsOpenLogCursor( ztrLogCursor  * aLogCursor,
                             stlInt64        aBlockSeq,
                             stlChar       * aFileName,
                             stlErrorStack * aErrorStack );

stlStatus ztrsCloseLogCursor( ztrLogCursor  * aLogCursor,
                              stlErrorStack * aErrorStack );

stlStatus ztrsReadLogBlock( ztrLogCursor  * aLogCursor,
                            stlChar      ** aLogBlock,
                            stlBool       * aEndOfFile,
                            stlErrorStack * aErrorStack );
                            
stlInt16 ztrsIsValidLog( ztrLogCursor    * aLogCursor,
                         ztrLogBlockHdr  * aLogBlockHdr );

stlStatus ztrsWriteLogBlockToBuffer( stlChar       * aLogBlock,
                                     stlErrorStack * aErrorStack );

stlStatus ztrsGetLogFileHdr( stlChar       * aFileName,
                             ztrLogFileHdr * aLogFileHdr,
                             stlErrorStack * aErrorStack );

stlStatus ztrsControlLogFlush( stlBool         aIsLock,
                               stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZTRS_H_ */
