/*******************************************************************************
 * ztmt.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmt.h 2567 2011-12-05 04:54:22Z enigma $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ZTMT_H_
#define _ZTMT_H_ 1

#include <sll.h>

/**
 * @file ztmt.h
 * @brief Gliese Master Threads Routines
 */

/**
 * @defgroup ztmThreads Gliese Master Threads Routines
 * @{
 */

/*
 * Checkpointer
 */

stlStatus ztmtCreateCheckPointThread( ztmEnv * aEnv );
stlStatus ztmtStopCheckPointThread( ztmEnv * aEnv );

/*
 * Log Flusher
 */

stlStatus ztmtCreateLogFlushThreads( ztmEnv * aEnv );
stlStatus ztmtStopLogFlushThreads( ztmEnv * aEnv );

/*
 * Ager
 */

stlStatus ztmtCreateAgerThread( ztmEnv * aEnv );
stlStatus ztmtStopAgerThread( ztmEnv * aEnv );

/*
 * Cleanup
 */

stlStatus ztmtCreateCleanupThread( ztmEnv * aEnv );
stlStatus ztmtStopCleanupThread( ztmEnv * aEnv );
stlStatus ztmtCleanupDeadEnv( ztmEnv * aSystemEnv );
stlStatus ztmtCleanupLongStatement( ztmEnv * aSystemEnv );

/*
 * Timer
 */

stlStatus ztmtCreateTimerThread( ztmEnv * aEnv );
stlStatus ztmtStopTimerThread( ztmEnv * aEnv );

/*
 * ProcessMonitor
 */

stlStatus ztmtCreateProcessMonitorThread( ztmEnv * aEnv );
stlStatus ztmtStopProcessMonitorThread( ztmEnv * aEnv );

stlStatus ztmtStartProcess( sllSharedMemory  * aSharedMemory,
                            ztmEnv           * aSystemEnv );

stlStatus ztmtStartBalancer( ztmEnv           * aSystemEnv );

stlStatus ztmtStartDispatcher( stlInt32           aStartIdx,
                               stlInt32           aEndIdx,
                               ztmEnv           * aSystemEnv );

stlStatus ztmtStartSharedServer( stlInt32           aStartIdx,
                                 stlInt32           aEndIdx,
                                 ztmEnv           * aSystemEnv );

stlStatus ztmtStopProcess( sllSharedMemory  * aSharedMemory,
                           ztmEnv           * aSystemEnv );
stlStatus ztmtKillProcess( sllSharedMemory  * aSharedMemory,
                           ztmEnv           * aSystemEnv );

stlStatus ztmtCheckProcess( sllSharedMemory  * aSharedMemory,
                            ztmEnv           * aSystemEnv );

/*
 * Archive Logger
 */

stlStatus ztmtCreateArchivelogThread( ztmEnv * aEnv );
stlStatus ztmtStopArchivelogThread( ztmEnv * aEnv );

/*
 * Page Flusher
 */

stlStatus ztmtCreatePageFlushThread( ztmEnv * aEnv );
stlStatus ztmtStopPageFlushThread( ztmEnv * aEnv );

/*
 * IO Slaves
 */

stlStatus ztmtCreateIoSlaveThread( stlInt64 aThreadIdx, ztmEnv * aEnv );
stlStatus ztmtStopIoSlaveThreads( stlInt32 aIoSlaveCount, ztmEnv * aEnv );
stlStatus ztmtCancelIoSlaveThread( stlInt64 aThreadIdx, ztmEnv * aSystemEnv );

/** @} */

#endif /* _ZTMT_H_ */
