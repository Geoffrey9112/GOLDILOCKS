/*******************************************************************************
 * smd.h
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


#ifndef _SMD_H_
#define _SMD_H_ 1

#include <smdDef.h>

/**
 * @file smd.h
 * @brief Storage Manager Layer Table Internal Routines
 */

/**
 * @defgroup smd Table
 * @ingroup smInternal
 * @{
 */

stlStatus smdStartupNoMount( void  ** aWarmupEntry,
                             smlEnv * aEnv );
stlStatus smdWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv );
stlStatus smdCooldown( smlEnv * aEnv );
stlStatus smdBootdown( smlEnv * aEnv );

stlStatus smdRegisterFxTables( smlEnv * aEnv );

stlStatus smdDropAging( void      * aData,
                        stlUInt32   aDataSize,
                        stlBool   * aDone,
                        void      * aEnv );
stlStatus smdLockItemAging( void      * aData,
                            stlUInt32   aDataSize,
                            stlBool   * aDone,
                            void      * aEnv );
stlStatus smdDropAgingInternal( void        * aRelHandle,
                                stlBool       aOnStartup,
                                stlBool     * aDone,
                                smlEnv      * aEnv );
stlStatus smdTruncateAging( void      * aData,
                            stlUInt32   aDataSize,
                            stlBool   * aDone,
                            void      * aEnv );
stlStatus smdSetSegmentPending( stlUInt32   aActionFlag,
                                smgPendOp * aPendOp,
                                smlEnv    * aEnv );

inline smdTableModule * smdGetTableModule( smlTableType aTableType );

inline void smdGetSystemInfo( smdSystemInfo * aSystemInfo );
inline void * smdGetLockHandle( void * aRelationHandle );

inline stlStatus smdInitVolatileInfo( void * aRelationHandle, smlEnv * aEnv );

stlStatus smdCompareKeyValue( void                * aRelationHandle,
                              smlRid              * aRowRid,
                              smpHandle           * aPageHandle,
                              stlBool             * aPageLatchRelease,
                              smxlTransId           aMyTransId,
                              smlScn                aMyViewScn,
                              smlScn                aMyCommitScn,
                              smlTcn                aMyTcn,
                              knlKeyCompareList   * aKeyCompList,
                              stlBool             * aIsMatch,
                              smlScn              * aCommitScn,
                              smlEnv              * aEnv );
stlStatus smdExtractKeyValue( void                * aRelationHandle,
                              smpHandle           * aPageHandle,
                              stlBool               aFetchFirst,
                              smpRowItemIterator  * aRowIterator,
                              smdValueBlockList   * aColumnList,
                              stlInt32            * aKeyValueSize,
                              stlInt16            * aRowSeq,
                              stlBool             * aHasNullInBlock,
                              stlBool             * aIsSuccess,
                              smlEnv              * aEnv );
stlStatus smdExtractValidKeyValue( void                * aRelationHandle,
                                   smxlTransId           aTransId,
                                   smlScn                aViewScn,
                                   smlTcn                aTcn,
                                   smlRid              * aRid,
                                   knlValueBlockList   * aColumnList,
                                   smlEnv              * aEnv );

stlStatus smdExecutePendingOperation( smlStatement * aStatement,
                                      void         * aRelationHandle,
                                      smlEnv       * aEnv );

stlStatus smdInsertPendingOperation( smxlTransId    aTransId,
                                     smlTbsId       aTbsId,
                                     stlInt32       aOperationType,
                                     void         * aData,
                                     stlInt32       aDataSize,
                                     smlEnv       * aEnv );

stlStatus smdCreatePendingRelation( smlStatement * aStatement,
                                    smlTbsId       aTbsId,
                                    smlEnv       * aEnv );

stlStatus smdDropPendingRelation( smlStatement * aStatement,
                                  smlTbsId       aTbsId,
                                  smlEnv       * aEnv );

stlStatus smdTruncatePendingRelation( smlStatement * aStatement,
                                      smlTbsId       aTbsId,
                                      smlEnv       * aEnv );

stlStatus smdInternalError( smpHandle      * aPageHandle,
                            smlEnv         * aEnv,
                            const stlChar  * aFormat,
                            ... );

stlStatus smdAddHandleToSession( smlStatement      * aStatement,
                                 smeRelationHeader * aRelHeader,
                                 smlEnv            * aEnv );

stlStatus smdRemoveHandleFromSession( smlStatement      * aStatement,
                                      smeRelationHeader * aRelHeader,
                                      smlEnv            * aEnv );
stlInt64 smdGetActiveInstantRelationCount(  smlSessionEnv * aEnv );

stlStatus smdCleanupAllInstTables( smlEnv * aEnv );

stlStatus smdBuildCache( void      * aRelationHandle,
                         smlEnv    * aEnv );
stlStatus smdFetchRecord4Index( smiIterator         * aIterator,
                                void                * aBaseRelHandle,
                                smlFetchRecordInfo  * aFetchRecordInfo,
                                smpHandle           * aTablePageHandle,
                                stlBool             * aTablePageLatchReleased,
                                smlRid              * aRowRid,
                                stlInt32              aBlockIdx,
                                smlScn                aAgableViewScn,
                                stlBool             * aIsMatched,
                                smlEnv              * aEnv );
stlStatus smdCreatePending( stlUInt32   aActionFlag,
                            smgPendOp * aPendOp,
                            smlEnv    * aEnv );
stlStatus smdDropPending( stlUInt32   aActionFlag,
                          smgPendOp * aPendOp,
                          smlEnv    * aEnv );
stlStatus smdFreeLockPending( stlUInt32   aActionFlag,
                              smgPendOp * aPendOp,
                              smlEnv    * aEnv );

stlStatus smdWriteSupplementalLog( smxmTrans         * aMiniTrans,
                                   stlUInt16           aLogType,
                                   stlInt32            aValueIdx,
                                   knlValueBlockList * aPrimaryKey,
                                   knlValueBlockList * aAfterCols,
                                   smlRid            * aRowRid,
                                   smlEnv            * aEnv );

stlStatus smdWriteSupplUpdateColLogs( smxmTrans         * aMiniTrans,
                                      knlValueBlockList * aLogCols,
                                      stlInt32            aValueIdx,
                                      smlRid            * aRowRid,
                                      smlEnv            * aEnv );

stlStatus smdWriteSupplUpdateBeforeColLogs( smxmTrans         * aMiniTrans,
                                            knlValueBlockList * aLogCols,
                                            stlInt32            aValueIdx,
                                            smlRid            * aRowRid,
                                            smlEnv            * aEnv );

/** @} */
    
#endif /* _SMD_H_ */
