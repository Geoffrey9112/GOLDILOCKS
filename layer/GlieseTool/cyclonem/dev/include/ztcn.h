/*******************************************************************************
 * ztcn.h
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


#ifndef _ZTCN_H_
#define _ZTCN_H_ 1

/**
 * @file ztcn.h
 * @brief GlieseTool Cyclone Sync Routines
 */


/**
 * @defgroup ztcn Cyclone Sync Routines
 * @{
 */

/**
 * ztcnGeneral
 * @{
 */ 
stlStatus ztcnSync( stlErrorStack * aErrorStack );

stlStatus ztcnInitializeSyncMgr( ztcSyncMgr    * aSyncMgr,
                                 stlErrorStack * aErrorStack );

stlStatus ztcnFinalizeSyncMgr( ztcSyncMgr    * aSyncMgr,
                               stlErrorStack * aErrorStack );

stlStatus ztcnInitializeSyncher( ztcSyncher    * aSyncher,
                                 stlErrorStack * aErrorStack );

stlStatus ztcnFinalizeSyncher( ztcSyncher    * aSyncher,
                               stlErrorStack * aErrorStack );

stlStatus ztcnInitailzeSyncBufferInfoNSyncher( ztcSyncMgr    * aSyncMgr,
                                               stlErrorStack * aErrorStack );

stlStatus ztcnFinalizeSyncBufferInfoNSyncher( ztcSyncMgr    * aSyncMgr,
                                              stlErrorStack * aErrorStack );

stlStatus ztcnInitializeSyncTableInfo( ztcSyncMgr    * aSyncMgr,
                                       ztcMasterMgr  * aMasterMgr,
                                       stlErrorStack * aErrorStack );

stlStatus ztcnFinalizeSyncTableInfo( ztcSyncMgr    * aSyncMgr,
                                     stlErrorStack * aErrorStack );

stlStatus ztcnInitializeSyncColumnInfo( ztcSyncMgr          * aSyncMgr,
                                        ztcSyncTableInfo    * aSyncTableInfo,
                                        ztcMasterTableMeta  * aTableMeta,
                                        stlErrorStack       * aErrorStack );

stlStatus ztcnFinalizeSyncColumnInfo( stlDynamicAllocator * aDynamicMem,
                                      ztcSyncTableInfo    * aSyncTableInfo,
                                      stlErrorStack       * aErrorStack );

stlStatus ztcnGetSyncColumnInfo( SQLHDBC       * aDbcHandle,
                                 stlChar       * aSchemaName,
                                 stlChar       * aTableName,
                                 dtlFuncVector * aDTVector,
                                 ztcColumnInfo * aColumnInfo,
                                 stlErrorStack * aErrorStack );

stlStatus ztcnSetSyncStartPosition( stlErrorStack * aErrorStack );

/** @} */

/**
 * ztcnSyncManager
 * @{
 */ 
stlStatus ztcnDoSyncMgr( ztcSyncMgr    * aSyncMgr,
                         stlErrorStack * aErrorStack );

stlStatus ztcnAllocSyncColumnValue( ztcSyncMgr        * aSyncMgr,
                                    ztcSyncTableInfo  * aSyncBufferInfo,
                                    stlErrorStack     * aErrorStack );

stlStatus ztcnFreeSyncColumnValue( ztcSyncMgr        * aSyncMgr,
                                   stlInt64          * aErrCount,
                                   stlErrorStack     * aErrorStack );

stlStatus ztcnReAllocLongVariable( stlDynamicAllocator             * aDynamicMem,
                                   SQL_LONG_VARIABLE_LENGTH_STRUCT * aLongVariable,
                                   stlInt64                          aSize,
                                   stlErrorStack                   * aErrorStack );
/** @} */

/**
 * ztcnSyncher
 * @{
 */

stlStatus ztcnDoSyncher( ztcSyncher    * aSyncher,
                         stlErrorStack * aErrorStack );

/** @} */

/** @} */

#endif /* _ZTCN_H_ */
