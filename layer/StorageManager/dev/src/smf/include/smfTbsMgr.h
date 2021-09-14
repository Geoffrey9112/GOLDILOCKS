/*******************************************************************************
 * smfTbsMgr.h
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


#ifndef _SMFTBSMGR_H_
#define _SMFTBSMGR_H_ 1

/**
 * @file smfTbsMgr.h
 * @brief Storage Manager Layer Tablespace Manager Component Internal Routines
 */

/**
 * @defgroup smfTbsMgr Tablespace Manager
 * @ingroup smfInternal
 * @{
 */

stlStatus smfAddDatafilePending( stlUInt32   aActionFlag,
                                 smgPendOp * aPendOp,
                                 smlEnv    * aEnv );
stlStatus smfCreateTbsPending( stlUInt32   aActionFlag,
                               smgPendOp * aPendOp,
                               smlEnv    * aEnv );
stlStatus smfDropTbsPending( stlUInt32   aActionFlag,
                             smgPendOp * aPendOp,
                             smlEnv    * aEnv );
stlStatus smfDropDatafilePending( stlUInt32   aActionFlag,
                                  smgPendOp * aPendOp,
                                  smlEnv    * aEnv );
stlStatus smfOnlineTbsPending( stlUInt32   aActionFlag,
                               smgPendOp * aPendOp,
                               smlEnv    * aEnv );
stlStatus smfOfflineTbsPending( stlUInt32   aActionFlag,
                                smgPendOp * aPendOp,
                                smlEnv    * aEnv );
smfDatafileInfo * smfGetDatafileInfo( smlTbsId      aTbsId,
                                      smlDatafileId aDatafileId );

stlStatus smfFirstDatafilePersData( void                 * aTbsIterator,
                                    smfDatafilePersData  * aDatafilePersData,
                                    void                ** aIterator,
                                    smlEnv               * aEnv );
stlStatus smfNextDatafilePersData( void                 * aTbsIterator,
                                   smfDatafilePersData  * aDatafilePersData,
                                   void                ** aIterator,
                                   smlEnv               * aEnv );

stlStatus smfValidateDatafileNames( stlChar   ** aDatafileNameArray,
                                    stlInt16     aDatafileCount,
                                    stlBool      aMyTbsId,
                                    smlEnv     * aEnv );

stlStatus smfAcquireTablespace( smfTbsInfo * aTbsInfo,
                                smlEnv     * aEnv );

stlStatus smfReleaseTablespace( smfTbsInfo * aTbsInfo,
                                smlEnv     * aEnv );

void * STL_THREAD_FUNC smfParallelLoadThread( stlThread * aThread,
                                              void      * aArg );

stlStatus smfWriteDatafileHeader( stlFile           * aFile,
                                  smfDatafileHeader * aDatafileHeader,
                                  smlEnv            * aEnv );

stlStatus smfReadDatafileHeader( stlFile           * aFile,
                                 stlChar           * aFilePath,
                                 smfDatafileHeader * aDatafileHeader,
                                 smlEnv            * aEnv );

stlStatus smfChangeDatafileHeaderInternal( smlStatement  * aStatement,
                                           smlTbsId        aTbsId,
                                           smlDatafileId   aDatafileId,
                                           smrLid          aChkptLid,
                                           smrLsn          aChkptLsn,
                                           smlEnv        * aEnv );

stlStatus smfRewriteDatafileHeader( smfDatafileInfo * aDatafileInfo,
                                    smrLid            aChkptLid,
                                    smrLsn            aChkptLsn,
                                    smlEnv          * aEnv );

void smfSetTbsState( smlTbsId     aTbsId,
                     smfTbsInfo * aTbsInfo,
                     stlUInt16    aState );

stlStatus smfFillDummyTbsInfo( smfTbsInfo ** aTbsInfoArray,
                               smlEnv      * aEnv );

stlStatus smfCreateDatafilePending( stlUInt32   aActionFlag,
                                    smgPendOp * aPendOp,
                                    smlEnv    * aEnv );

stlStatus smfRenameDatafilePending( stlUInt32   aActionFlag,
                                    smgPendOp * aPendOp,
                                    smlEnv    * aEnv );

/** @} */
    
#endif /* _SMFTBSMGR_H_ */
