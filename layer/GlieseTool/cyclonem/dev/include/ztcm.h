/*******************************************************************************
 * ztcm.h
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


#ifndef _ZTCM_H_
#define _ZTCM_H_ 1

/**
 * @file ztcm.h
 * @brief GlieseTool Cyclone Main Routines
 */
extern ztcLibFunc           gZtcLibFunc;
extern stlBool              gRunState;
extern ztcMasterMgr       * gMasterMgr;

/**
 * @defgroup ztcm Cyclone Main Routines
 * @{
 */
void ztcmLogMsgCallback( const char * aLog );

stlStatus ztcmOpenLibrary( stlDsoHandle   * aZlmlLib,
                           stlErrorStack  * aErrorStack );

stlStatus ztcmCloseLibrary( stlDsoHandle    aZlmlLib,
                            stlErrorStack * aErrorStack );

void      ztcmPrintHelp();

void      ztcmMasterSignalFunc( stlInt32 aSignalNo );

void      ztcmSlaveSignalFunc( stlInt32 aSignalNo );

void      ztcmPrintErrorstack( stlErrorStack * aErrorStack );

stlStatus ztcmLogMsg( stlErrorStack  * aErrorStack,
                      const stlChar  * aFormat,
                      ... );
                      
void ztcmWriteLastError( stlErrorStack * aErrorStack );

stlStatus ztcmWriteTimeStamp( stlFile        * aFile,
                              stlErrorStack  * aErrorStack );

stlStatus ztcmAddLogMsg( stlChar        * aMsg,
                         stlInt32         aLength,
                         stlErrorStack  * aErrorStack );

stlStatus ztcmProcessAliveCheck( stlChar       * aFileName,
                                 stlErrorStack * aErrorStack );

stlStatus ztcmStart( stlBool         aIsMaster,
                     stlBool         aDoReset,
                     stlErrorStack * aErrorStack );
                    
stlStatus ztcmStop( stlBool         aIsMaster,
                    stlErrorStack * aErrorStack );

stlStatus ztcmInitializeEnv4Master( stlErrorStack * aErrorStack );

stlStatus ztcmFinalizeEnv4Master( stlErrorStack * aErrorStack );

stlStatus ztcmInitializeEnv4Slave( stlErrorStack * aErrorStack );

stlStatus ztcmFinalizeEnv4Slave( stlErrorStack * aErrorStack );
        
stlUInt32 ztcmCaptureTableHashFunc( void      * aKey, 
                                    stlUInt32   aBucketCount );
                            
stlInt32  ztcmCaptureTableHashCompareFunc( void * aKeyA, 
                                           void * aKeyB );

stlUInt32 ztcmSlaveTableHashFunc( void      * aKey, 
                                  stlUInt32   aBucketCount );

stlInt32  ztcmSlaveTableHashCompareFunc( void * aKeyA, 
                                         void * aKeyB );
                                         
stlUInt32 ztcmPrimaryKeyHashFunc( void      * aKey, 
                                  stlUInt32   aBucketCount );
                                  
stlInt32  ztcmPrimaryKeyHashCompareFunc( void * aKeyA, 
                                         void * aKeyB );

stlStatus ztcmAddGroupItem( stlChar       * aGroupName,
                            stlErrorStack * aErrorStack );

stlStatus ztcmAddTableInfo( stlChar       * aSchemaName,
                            stlChar       * aTableName,
                            stlErrorStack * aErrorStack );
                           
stlStatus ztcmAddApplyInfo( stlChar       * aMasterSchema,
                            stlChar       * aMasterTable,
                            stlChar       * aSlaveSchema,
                            stlChar       * aSlaveTable,
                            stlErrorStack * aErrorStack );

stlStatus ztcmAddLogFileInfo( stlChar       * aLogFileName,
                              stlInt16        aGroupId,
                              stlInt64        aFileSize,
                              stlErrorStack * aErrorStack );

stlStatus ztcmSetLogFileInfo( ztcCaptureInfo * aCaptureInfo,
                              stlErrorStack  * aErrorStack );

stlStatus ztcmCreateThreads( stlErrorStack  * aErrorStack );
                            
stlStatus ztcmJoinThreads( stlBool          aIsMaster,
                           stlErrorStack  * aErrorStack );

stlStatus ztcmGetCurrentLogFileInfo( ztcLogFileInfo ** aLogFileInfo,
                                     stlErrorStack   * aErrorStack );
                                    
stlStatus ztcmSetCurrentLogFileInfo( stlErrorStack * aErrorStack );
                                    
stlStatus ztcmGetNextLogFileInfo( ztcLogFileInfo ** aLogFileInfo,
                                  stlErrorStack   * aErrorStack );
                                    
stlStatus ztcmGetPidFileName( stlChar       * aFileName,
                              stlBool         aIsMaster,
                              stlErrorStack * aErrorStack );

stlStatus ztcmSetConfigureFile( const stlChar * aConfigureFile,
                                stlErrorStack * aErrorStack );

stlStatus ztcmGetConfigureFileName( stlChar       * aFileName,
                                    stlErrorStack * aErrorStack );

stlStatus ztcmSetGroupName( const stlChar * aGroupName,
                            stlErrorStack * aErrorStack );

stlStatus ztcmFindCaptureTableInfo( ztcCaptureTableInfo  ** aTableInfo,
                                    stlInt64                aId,
                                    stlErrorStack         * aErrorStack );
                            
stlStatus ztcmFindSlaveTableInfo( ztcSlaveTableInfo  ** aTableInfo,
                                  stlInt32              aTableId,
                                  void                * aErrorStack );
                                  
stlStatus ztcmSetUserIdToGlobalArea( stlChar       * aUserId,
                                     stlErrorStack * aErrorStack );

stlStatus ztcmSetUserIdToGroupArea( stlChar       * aUserId,
                                    stlErrorStack * aErrorStack );

stlStatus ztcmGetUserId( stlChar       * aUserId,
                         stlErrorStack * aErrorStack );
                        
stlStatus ztcmSetUserPwToGlobalArea( stlChar       * aUserPw,
                                     stlErrorStack * aErrorStack );

stlStatus ztcmSetUserPwToGroupArea( stlChar       * aUserPw,
                                    stlErrorStack * aErrorStack );
                         
stlStatus ztcmSetUserEncryptPwToGlobalArea( stlChar       * aUserEncryptPw,
                                            stlErrorStack * aErrorStack );

stlStatus ztcmSetUserEncryptPwToGroupArea( stlChar       * aUserEncryptPw,
                                           stlErrorStack * aErrorStack );

stlStatus ztcmGetUserPw( stlChar       * aUserPw,
                         stlErrorStack * aErrorStack );
                        
stlStatus ztcmSetPort( stlChar       * aPort,
                       stlErrorStack * aErrorStack );
                       
stlStatus ztcmGetPort( stlInt32      * aPort,
                       stlErrorStack * aErrorStack );

stlStatus ztcmSetHostPortToGlobalArea( stlChar       * aPort,
                                       stlErrorStack * aErrorStack );

stlStatus ztcmSetHostPortToGroupArea( stlChar       * aPort,
                                      stlErrorStack * aErrorStack );

stlStatus ztcmGetHostPort( stlInt32      * aPort,
                           stlErrorStack * aErrorStack );

stlStatus ztcmSetGoldilocksListenPort( stlChar       * aPort,
                                  stlErrorStack * aErrorStack );
                                  
stlStatus ztcmGetGoldilocksListenPort( stlInt32      * aPort,
                                  stlErrorStack * aErrorStack );

stlStatus ztcmSetHostIpToGlobalArea( stlChar       * aHostIp,
                                     stlErrorStack * aErrorStack );
                                     
stlStatus ztcmSetHostIpToGroupArea( stlChar       * aHostIp,
                                    stlErrorStack * aErrorStack );

stlChar * ztcmGetHostIp();

stlStatus ztcmSetDsnToGlobalArea( stlChar       * aDsn,
                                  stlErrorStack * aErrorStack );
                                     
stlStatus ztcmSetDsnToGroupArea( stlChar       * aDsn,
                                 stlErrorStack * aErrorStack );

stlChar * ztcmGetDsn();

stlStatus ztcmSetMasterIp( stlChar       * aMasterIp,
                           stlErrorStack * aErrorStack );

stlChar * ztcmGetMasterIp();

stlStatus ztcmSetTransSortAreaSize( stlChar       * aSize,
                                    stlErrorStack * aErrorStack );

stlInt64  ztcmGetTransSortAreaSize();

stlStatus ztcmSetApplierCount( stlChar       * aSize,
                               stlErrorStack * aErrorStack );

stlInt64 ztcmGetApplierCount( stlErrorStack * aErrorStack );

stlStatus ztcmSetApplyCommitSize( stlChar       * aSize,
                                  stlErrorStack * aErrorStack );

stlInt64  ztcmGetApplyCommitSize();

stlStatus ztcmSetApplyArraySize( stlChar       * aSize,
                                 stlErrorStack * aErrorStack );

stlInt64  ztcmGetApplyArraySize();

stlStatus ztcmSetReadLogBlockCount( stlChar       * aCount,
                                    stlErrorStack * aErrorStack );

stlInt64  ztcmGetReadLogBlockCount();

stlStatus ztcmSetCommChunkCount( stlChar       * aCount,
                                 stlErrorStack * aErrorStack );

stlInt32  ztcmGetCommChunkCount();

stlStatus ztcmSetPropagateMode( stlChar       * aMode,
                                stlErrorStack * aErrorStack );

stlBool   ztcmGetPropagateMode();

stlStatus ztcmSetUserLogFilePath( stlChar       * aLogPath,
                                  stlErrorStack * aErrorStack );

stlChar * ztcmGetUserLogFilePath();

stlBool   ztcmGetUserLogFilePathStatus();

stlStatus ztcmSetTransFilePath( stlChar       * aPath,
                                stlErrorStack * aErrorStack );

stlChar * ztcmGetTransFilePath();

stlStatus ztcmSetGiveupInterval( stlChar       * aInterval,
                                 stlErrorStack * aErrorStack );

stlInt64  ztcmGetGiveupInterval();

stlStatus ztcmGetTransFileName( stlInt32        aIdx,
                                stlChar       * aName,
                                stlErrorStack * aErrorStack );
                                
stlBool   ztcmGetArchiveLogMode();

stlStatus ztcmGetArchiveLogFile( stlChar       * aFileName,
                                 stlInt64        aFileSeqNo,
                                 stlErrorStack * aErrorStack );

stlStatus ztcmGetSlaveIp( stlChar       * aSlaveIp,
                          stlErrorStack * aErrorStack );

stlStatus ztcmGetSlaveUserId( stlChar       * aUserId,
                              stlErrorStack * aErrorStack );

stlStatus ztcmGetSlaveUserPw( stlChar       * aUserPw,
                              stlErrorStack * aErrorStack );

stlStatus ztcmGetSlaveGoldilocksListenPort( stlInt32      * aPort,
                                       stlErrorStack * aErrorStack );

stlStatus ztcmSetSyncherCount( stlChar       * aCount,
                               stlErrorStack * aErrorStack );

stlInt32  ztcmGetSyncherCount();

stlStatus ztcmSetSyncArraySize( stlChar       * aSize,
                                stlErrorStack * aErrorStack );

stlInt32  ztcmGetSyncArraySize();

stlStatus ztcmSetUpdateApplyMode( stlChar       * aMode,
                                  stlErrorStack * aErrorStack );

stlInt32  ztcmGetUpdateApplyMode( stlErrorStack * aErrorStack );

stlBool   ztcmGetDbConnected();

stlInt64  ztcmGetTransTableSize();

stlStatus ztcmRemoveMasterMetaFile( stlErrorStack * aErrorStack );

stlStatus ztcmGetTableMetaFileName( stlChar * aFileName );

stlStatus ztcmGetMetaFileName( stlChar * aFileName );

stlStatus ztcmBuildMasterMeta( stlErrorStack * aErrorStack );

stlStatus ztcmReadMasterTableMeta( stlErrorStack * aErrorStack );

stlStatus ztcmWriteMasterTableMeta( ztcMasterTableMeta * aTableMeta,
                                    stlErrorStack      * aErrorStack );

stlStatus ztcmReadMasterMeta( stlErrorStack * aErrorStack );

stlStatus ztcmWriteMasterMeta( stlErrorStack * aErrorStack );

stlStatus ztcmFindTableMeta( ztcMasterTableMeta ** aTableMeta,
                             stlChar             * aSchemaName,
                             stlChar             * aTableName,
                             stlErrorStack       * aErrorStack );

stlStatus ztcmUpdateMasterTableMeta( ztcMasterTableMeta  * aTableMeta,
                                     stlErrorStack       * aErrorStack );

stlStatus ztcmSetLogFileInfoUsingLSN( stlInt64        aBeginLsn,
                                      stlErrorStack * aErrorStack );

stlBool   ztcmGetSyncMode();

void      ztcmSetSyncMode( stlBool aMode );

void      ztcmInitSpinLock( ztcSpinLock * aSpinLock );

void      ztcmFinSpinLock( ztcSpinLock * aSpinLock );

void      ztcmAcquireSpinLock( ztcSpinLock * aSpinLock );

void      ztcmTrySpinLock( ztcSpinLock * aSpinLock,
                           stlBool     * aIsSuccess );
                           
void      ztcmReleaseSpinLock( ztcSpinLock * aSpinLock );

stlStatus ztcmPrintStatus( stlBool         aIsMaster,
                           stlErrorStack * aErrorStack );
                           
stlStatus ztcmSetHomeDir( stlErrorStack * aErrorStack );

dtlCharacterSet ztcmGetCharSetIDFunc( void * aArgs );
stlInt32        ztcmGetGMTOffsetFunc( void * aArgs );
stlChar       * ztcmGetDateFormatFunc( void * aArgs );
stlStatus       ztcmReallocLongVaryingMemFunc( void           * aArgs,
                                               stlInt32         aAllocSize,
                                               void          ** aAddr,
                                               stlErrorStack  * aErrorStack );
stlStatus ztcmReallocAndMoveLongVaryingMemFunc( void           * aArgs,
                                                dtlDataValue   * aDataValue,
                                                stlInt32         aAllocSize,
                                                stlErrorStack  * aErrorStack );
stlChar       * ztcmGetTimeFormatFunc( void * aArgs );
stlChar       * ztcmGetTimeTzFormatFunc( void * aArgs );
stlChar       * ztcmGetTimestampFormatFunc( void * aArgs );
stlChar       * ztcmGetTimestampTzFormatFunc( void * aArgs );
stlStatus       ztcmSetDTFuncVector();
stlStatus       ztcmSetSyncDTFuncVector( dtlFuncVector * aVectorFunc );

stlStatus ztcmLogConfigureMsgMaster( stlErrorStack * aErrorStack );
stlStatus ztcmLogConfigureMsgSlave( stlErrorStack * aErrorStack );

stlStatus ztcmSetLibConfig( stlChar       * aLibConfig,
                            stlErrorStack * aErrorStack );

stlStatus ztcmGetLibConfig( stlChar       * aLibConfig,
                            stlInt32        aBuffSize,
                            stlErrorStack * aErrorStack );

stlStatus ztcmSetSqlFileName( stlChar       * aFileName,
                              stlErrorStack * aErrorStack );

stlStatus ztcmGetSqlFileName( stlChar       * aFileName,
                              stlInt32        aBuffSize,
                              stlErrorStack * aErrorStack );

stlStatus ztcmSetSqlFileSize( stlChar       * aFileSize,
                              stlErrorStack * aErrorStack );

stlInt64 ztcmGetSqlFileSize();


stlStatus ztcmChangePath( stlChar           * aFilePath,
                          stlInt32            aBuffSize,
                          stlErrorStack     * aErrorStack );

/**
 * Communication
 */

stlStatus ztcmSendProtocolHdrNData( stlContext     * aContext,
                                    ztcProtocolHdr * aHeader,
                                    stlChar        * aData,
                                    stlErrorStack  * aErrorStack );

stlStatus ztcmReceiveProtocolHdr( stlContext     * aContext,
                                  ztcProtocolHdr * aHeader,
                                  stlErrorStack  * aErrorStack );
                                 
stlStatus ztcmReceiveProtocolHdrWithPoll( stlContext     * aContext,
                                          ztcProtocolHdr * aHeader,
                                          stlBool          aTimeoutCheck,
                                          stlErrorStack  * aErrorStack );

stlStatus ztcmSendData( stlContext    * aContext,
                        stlChar       * aData,
                        stlInt32        aSize,
                        stlErrorStack * aErrorStack );

stlStatus ztcmPubData( ztcMasterCMMgr     * aCMMgr,
                       ztclPublisher      * aContext,
                       stlChar            * aData,
                       stlSize              aSize,
                       stlInt32           * aTrSeq,
                       stlErrorStack      * aErrorStack );

stlStatus ztcmReceiveData( stlContext    * aContext,
                           stlChar       * aData,
                           stlInt32        aSize,     
                           stlErrorStack * aErrorStack );

stlStatus ztcmGetPeerIPAddr( stlContext    * aContext,
                             stlChar       * aIPAddr,
                             stlSize         aSize,
                             stlErrorStack * aErrorStack );


stlStatus ztcmWriteProtocolHdr( ztcProtocolHdr    * aProtocolHdr,
                                stlInt32            aEndian,
                                stlUInt8          * aBuffer,
                                stlErrorStack     * aErrorStack );

stlStatus ztcmReadProtocolHdr( stlUInt8          * aBuffer,
                               stlInt32            aEndian,
                               ztcProtocolHdr    * aProtocolHdr,
                               stlErrorStack     * aErrorStack );

stlStatus ztcmWriteColumnInfo( ztcColumnInfo    * aColumnInfo,
                               stlUInt8         * aBuffer,
                               stlErrorStack    * aErrorStack );

stlStatus ztcmReadColumnInfo( stlUInt8         * aBuffer,
                              ztcColumnInfo    * aColumnInfo,
                              stlErrorStack    * aErrorStack );

stlStatus ztcmChangeByteOrderCaptureInfo( stlInt32            aByteOrder,
                                          ztcCaptureInfo    * aCaptureInfo,
                                          stlErrorStack     * aErrorStack );

stlStatus ztcmChangeByteOrderTransHdr( stlInt32         aByteOrder,
                                       ztcTransHdr    * aTransHdr,
                                       stlErrorStack  * aErrorStack );
/**
 * Static Hash
 */

stlStatus ztcmCreateStaticHash( ztcStaticHash  ** aHash,
                                stlUInt16         aBucketCount,
                                stlUInt16         aLinkOffset,
                                stlUInt16         aKeyOffset,
                                stlAllocator    * aAllocator,
                                stlErrorStack   * aErrorStack );
                                
stlStatus ztcmInsertStaticHash( ztcStaticHash   * aHash, 
                                ztcHashingFunc    aHashFunc,
                                void            * aData,
                                stlErrorStack   * aErrorStack );
                               
stlStatus ztcmRemoveStaticHash( ztcStaticHash   * aHash, 
                                ztcHashingFunc    aHashFunc,
                                void            * aData,
                                stlErrorStack   * aErrorStack );
                               
stlStatus ztcmFindStaticHash( ztcStaticHash    * aHash, 
                              ztcHashingFunc     aHashFunc,
                              ztcCompareFunc     aCompareFunc,
                              void             * aKey,
                              void            ** aData,
                              stlErrorStack    * aErrorStack );
                             
stlStatus ztcmGetFirstHashItem( ztcStaticHash   * aHash, 
                                stlUInt32       * aFirstBucketSeq,
                                void           ** aData,
                                stlErrorStack   * aErrorStack );
                           
stlStatus ztcmGetNextHashItem( ztcStaticHash   * aHash, 
                               stlUInt32       * aBucketSeq,
                               void           ** aData,
                               stlErrorStack   * aErrorStack );

stlUInt32 ztcmMurmurHash( const void * aKey, stlInt32 aLength );

/**
 * Encrypt
 */

stlStatus ztcmPrintCipherText( stlChar          * aPlainText,
                               stlErrorStack    * aErrorStack);


stlStatus ztcmEncrypt( stlChar          * aPlainText,
                       stlSize            aPlainTextLen,
                       stlChar            aKey[16],
                       stlChar          * aCipherText,
                       stlSize          * aCipherTextLen,
                       stlErrorStack    * aErrorStack );

stlStatus ztcmDecrypt( stlChar          * aCipherText,
                       stlSize            aCipherTextLen,
                       stlChar            aKey[16],
                       stlChar          * aPlainText,
                       stlSize          * aPlainTextLen,
                       stlErrorStack    * aErrorStack );

stlStatus ztcmEncrypt8byte( stlUInt32       * aValue,
                            stlUInt32       * aKey,
                            stlErrorStack   * aErrorStack );

stlStatus ztcmDecrypt8byte( stlUInt32       * aValue,
                            stlUInt32       * aKey,
                            stlErrorStack   * aErrorStack );


stlStatus ztcmBase64Encode( stlChar       * aInput,
                            stlSize         aInputLen,
                            stlChar       * aBuf,
                            stlSize         aBufSize,
                            stlErrorStack * aErrorStack );

stlStatus ztcmBase64Decode( stlChar         * aInput,
                            stlUInt8        * aBuf,
                            stlSize         * aBufSize,
                            stlErrorStack   * aErrorStack );

/** @} */


/** @} */

#endif /* _ZTCM_H_ */
