/*******************************************************************************
 * ztcb.h
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

#ifndef _ZTCB_H_
#define _ZTCB_H_ 1



/**
 * @file ztcb.h
 * @brief GlieseTool Cyclone Distributor Routines
 */


/**
 * @defgroup ztcb Cyclone Distributor Routines
 * @{
 */

stlStatus ztcbInitializeSlotMgr( stlErrorStack * aErrorStack );

stlStatus ztcbFinalizeSlotMgr( stlErrorStack * aErrorStack );

stlStatus ztcbInitializeSubDistributor( stlErrorStack * aErrorStack );

stlStatus ztcbFinalizeSubDistributor( stlErrorStack * aErrorStack );


/**
 * Chunk Manager
 */

stlStatus ztcbGetChunkItemToRead( ztcChunkItem ** aChunkItem,
                                  stlErrorStack * aErrorStack );

stlStatus ztcbReadDataFromChunk( stlChar       * aData,
                                 stlInt64        aSize,
                                 stlErrorStack * aErrorStack );

stlStatus ztcbSkipDataFromChunk( stlInt64        aSize,
                                 stlErrorStack * aErrorStack );
                                
stlStatus ztcbPushChunkToWaitWriteList( ztcChunkItem  * aChunkItem,
                                        stlErrorStack * aErrorStack );
                                        
/**
 * Sub-Dist Chunk Manager
 */


stlStatus ztcbGetSubChunkItemToRead( ztcChunkItem ** aChunkItem,
                                     stlErrorStack * aErrorStack );

stlStatus ztcbGetSubChunkItemToWrite( ztcChunkItem ** aChunkItem,
                                      stlErrorStack * aErrorStack );

stlStatus ztcbPushSubChunkToWaitWriteList( ztcChunkItem  * aChunkItem,
                                           stlErrorStack * aErrorStack );

stlStatus ztcbPushSubChunkToWaitReadList( ztcChunkItem  * aChunkItem,
                                          stlErrorStack * aErrorStack );

stlStatus ztcbReadDataFromSubChunk( stlChar       * aData,
                                    stlInt64        aSize,
                                    stlErrorStack * aErrorStack );

stlStatus ztcbWriteDataToSubChunk( ztcApplierMgr * aApplierMgr,
                                   stlChar       * aData,
                                   stlInt64        aSize,
                                   stlErrorStack * aErrorStack );
     
stlStatus ztcbPushFirstSubChunkToWaitReadList( stlErrorStack * aErrorStack );

/**
 * Analyze & Distribute
 */

ztcApplierMgr * ztcbGetApplierMgr( stlInt32 aAppId );

stlStatus ztcbGetNextApplierMgr( ztcApplierMgr ** aApplierMgr );

stlStatus ztcbPushTransaction( ztcApplierMgr * aApplierMgr,
                               stlChar       * aTrans,
                               stlUInt16       aSize,
                               stlBool         aOccupied,
                               stlErrorStack * aErrorStack );

stlStatus ztcbPushTransToSubBuffer( ztcApplierMgr * aApplierMgr,
                                    stlChar       * aTrans,
                                    stlUInt16       aSize,
                                    stlErrorStack * aErrorStack );

stlStatus ztcbPushTransToApplier( ztcApplierMgr * aApplierMgr,
                                  stlChar       * aTrans,
                                  stlUInt16       aSize,
                                  stlErrorStack * aErrorStack );

stlStatus ztcbFlushApplierChunk( stlErrorStack * aErrorStack );

stlStatus ztcbDistributeFlow( stlErrorStack * aErrorStack );

stlStatus ztcbSubDistributeFlow( stlErrorStack * aErrorStack );

stlStatus ztcbOccupySlot( ztcApplierMgr * aApplierMgr,
                          stlInt64        aTransId,
                          stlUInt64       aHashValue,
                          stlInt32      * aSlotIdx,
                          stlBool       * aOccupied,
                          stlErrorStack * aErrorStack );

stlStatus ztcbOccupySubSlot( ztcApplierMgr * aApplierMgr,
                             stlInt32        aSlotIdx,
                             stlErrorStack * aErrorStack );

/**
 * Build Hash Value
 */

stlStatus ztcbAnalyzeInsert( ztcSlaveTableInfo * aTableInfo,
                             stlUInt64         * aHashValue,
                             stlInt32            aColIdx,
                             stlChar           * aData,
                             stlErrorStack     * aErrorStack );

stlStatus ztcbAnalyzeSuppLog( ztcSlaveTableInfo * aTableInfo,
                              stlUInt64         * aHashValue,
                              stlChar           * aData,
                              stlErrorStack     * aErrorStack );

stlStatus ztcbAnalyzeSuppUpdateColLog( ztcSlaveTableInfo * aTableInfo,
                                       stlChar           * aData,
                                       stlInt64            aDataSize,
                                       stlBool           * aKeyChanged,
                                       stlErrorStack     * aErrorStack );

stlStatus ztcbAnalyzeUpdate( ztcSlaveTableInfo * aTableInfo,
                             stlChar           * aData,
                             stlInt64            aDataSize,
                             stlErrorStack     * aErrorStack );

stlStatus ztcbAnalyzeInsert4Update( ztcSlaveTableInfo * aTableInfo,
                                    stlChar           * aData,
                                    stlInt64            aDataSize,
                                    stlErrorStack     * aErrorStack );

stlStatus ztcbBuildAfterHashValue( ztcSlaveTableInfo * aTableInfo,
                                   stlUInt64         * aHashValue,
                                   stlErrorStack     * aErrorStack );
        
/** @} */

/** @} */

#endif /* _ZTCB_H_ */


