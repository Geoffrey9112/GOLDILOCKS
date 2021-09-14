/*******************************************************************************
 * ztca.h
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

#ifndef _ZTCA_H_
#define _ZTCA_H_ 1



/**
 * @file ztca.h
 * @brief GlieseTool Cyclone Applier Routines
 */


/**
 * @defgroup ztca Cyclone Applier Routines
 * @{
 */

stlStatus ztcaInitializeApplier( ztcApplierMgr * aApplierMgr,
                                 stlErrorStack * aErrorStack );

stlStatus ztcaFinalizeApplier( ztcApplierMgr * aApplierMgr,
                               stlErrorStack * aErrorStack );

stlStatus ztcaProcessFlow( ztcApplierMgr * aApplierMgr,
                           stlErrorStack * aErrorStack );

stlStatus ztcaInitializeTableInfo( ztcApplierMgr * aApplierMgr,
                                   stlErrorStack * aErrorStack );

stlStatus ztcaInitializeStatement( ztcApplierMgr * aApplierMgr,
                                   stlErrorStack * aErrorStack );

stlStatus ztcaFinalizeStatement( ztcApplierMgr * aApplierMgr,
                                 stlErrorStack * aErrorStack );

stlStatus ztcaInitializeBuffer( ztcApplierMgr * aApplierMgr,
                                stlErrorStack * aErrorStack );

stlStatus ztcaFinalizeBuffer( ztcApplierMgr * aApplierMgr,
                              stlErrorStack * aErrorStack );

stlStatus ztcaPrepareNBindParam4Insert( ztcApplierMgr * aApplierMgr,
                                        stlErrorStack * aErrorStack );

stlStatus ztcaPrepareNBindParam4Delete( ztcApplierMgr * aApplierMgr,
                                        stlErrorStack * aErrorStack );

stlStatus ztcaDoPreviousExecute( ztcApplierMgr * aApplierMgr,
                                 stlErrorStack * aErrorStack );

stlBool   ztcaCheckNeedExecute( ztcApplierMgr       * aApplierMgr,
                                ztcApplierTableInfo * aTableInfo,
                                stlInt32              aTransType );
                                
stlStatus ztcaFindApplierTableInfo( ztcApplierMgr         * aApplierMgr,
                                    ztcApplierTableInfo  ** aTableInfo,
                                    stlInt32                aId,
                                    stlErrorStack         * aErrorStack );

stlStatus ztcaVacateSlot( ztcApplierMgr  * aApplierMgr,
                          stlErrorStack  * aErrorStack );

void      ztcaAddRecState( ztcApplierMgr * aApplierMgr,
                           stlInt32        aSlotIdx );

stlStatus ztcaWriteErrorValueToLog( stlChar              * aErrMsg,
                                    stlBool                aIsInsert,
                                    ztcApplierTableInfo  * aTableInfo,
                                    stlErrorStack        * aErrorStack );

/**
 * Chunk Manager
 */

stlStatus ztcaWriteDataToChunk( ztcApplierMgr  * aApplierMgr,
                                stlChar        * aData,
                                stlInt64         aSize,
                                stlErrorStack  * aErrorStack );
                                
stlStatus ztcaReadDataFromChunk( ztcApplierMgr  * aApplierMgr,
                                 stlChar        * aData,
                                 stlInt64         aSize,
                                 stlErrorStack  * aErrorStack );

stlStatus ztcaGetChunkItemToWrite( ztcApplierMgr  * aApplierMgr,
                                   ztcChunkItem  ** aChunkItem,
                                   stlErrorStack  * aErrorStack );
                                   
stlStatus ztcaGetChunkItemToRead( ztcApplierMgr  * aApplierMgr,
                                  ztcChunkItem  ** aChunkItem,
                                  stlErrorStack  * aErrorStack );

stlStatus ztcaPushChunkToWaitWriteList( ztcApplierMgr  * aApplierMgr,
                                        ztcChunkItem   * aChunkItem,
                                        stlErrorStack  * aErrorStack );

stlStatus ztcaPushChunkToWaitReadList( ztcApplierMgr  * aApplierMgr,
                                       ztcChunkItem   * aChunkItem,
                                       stlErrorStack  * aErrorStack );
                                       
stlStatus ztcaPushFirstChunkToWaitReadList( ztcApplierMgr * aApplierMgr,
                                            stlErrorStack * aErrorStack );
/**
 * Analyze
 */ 

stlStatus ztcaIsMasterRow( stlChar       * aData,
                           stlBool       * aIsMasterRow,
                           stlInt32      * aFirstColOrd,
                           stlErrorStack * aErrorStack );

stlStatus ztcaAnalyzeInsert( ztcApplierMgr       * aApplierMgr,
                             ztcApplierTableInfo * aTableInfo,
                             stlChar             * aData,
                             stlInt64              aDataSize,
                             stlInt64              aArrIdx,
                             stlInt32              aColIdx,
                             stlErrorStack       * aErrorStack );

stlStatus ztcaAnalyzeSuppLog( ztcApplierTableInfo * aTableInfo,
                              stlChar             * aData,
                              stlInt64              aDataSize,
                              stlInt64              aArrIdx,
                              stlErrorStack       * aErrorStack );


stlStatus ztcaAnalyzeSuppUpdateColLog( ztcApplierTableInfo * aTableInfo,
                                       stlChar             * aData,
                                       stlInt64              aDataSize,
                                       stlBool             * aNeedPrepare,
                                       stlErrorStack       * aErrorStack );

stlStatus ztcaValidateSuppUpdateColLog( ztcApplierTableInfo * aTableInfo,
                                        stlChar             * aData,
                                        stlInt64              aDataSize,
                                        stlBool             * aNeedPrepare,
                                        stlErrorStack       * aErrorStack );

stlStatus ztcaAnalyzeSuppUpdateBefColLog( ztcApplierMgr       * aApplierMgr,
                                          ztcApplierTableInfo * aTableInfo,
                                          stlChar             * aData,
                                          stlInt64              aDataSize,
                                          stlInt64              aArrIdx,
                                          stlErrorStack       * aErrorStack );

stlStatus ztcaAnalyzeUpdate( ztcApplierMgr       * aApplierMgr,
                             ztcApplierTableInfo * aTableInfo,
                             stlChar             * aData,
                             stlInt64              aDataSize,
                             stlInt64              aArrIdx,
                             stlErrorStack       * aErrorStack );

stlStatus ztcaAnalyzeInsert4Update( ztcApplierMgr       * aApplierMgr,
                                    ztcApplierTableInfo * aTableInfo,
                                    stlChar             * aData,
                                    stlInt64              aDataSize,
                                    stlInt64              aArrIdx,
                                    stlErrorStack       * aErrorStack );

stlStatus ztcaAnalyzeAppend4Update( ztcApplierMgr       * aApplierMgr,
                                    ztcApplierTableInfo * aTableInfo,
                                    stlChar             * aData,
                                    stlInt64              aDataSize,
                                    stlInt64              aArrIdx,
                                    stlErrorStack       * aErrorStack );

stlStatus ztcaMovePrimaryValue( ztcApplierTableInfo * aTableInfo,
                                stlInt64              aIdx,
                                stlErrorStack       * aErrorStack );

/** @} */


/** @} */

#endif /* _ZTCA_H_ */


