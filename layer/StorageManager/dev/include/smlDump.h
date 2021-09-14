/*******************************************************************************
 * smlDump.h
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


#ifndef _SMLDUMP_H_
#define _SMLDUMP_H_ 1

/**
 * @file smlDump.h
 * @brief Storage Manager Dump Routines
 */

/**
 * @defgroup smlDump Dump
 * @ingroup smExternal
 * @{
 */

stlStatus smlDumpControlFile( stlChar * aFilePath,
                              stlInt8   aSection,
                              smlEnv  * aEnv );

stlStatus smlDumpLogFile( stlChar * aFilePath,
                          stlInt64  aTargetLsn,
                          stlInt64  aLsnOffset,
                          stlInt64  aFetchCnt,
                          smlEnv  * aEnv );

stlStatus smlDumpDatafileHeader( stlChar * aFilePath,
                                 smlEnv  * aEnv );

stlStatus smlDumpDatafilePage( stlChar  * aFilePath,
                               stlInt64   aPageSeqId,
                               stlInt64   aFetchCnt,
                               smlEnv   * aEnv );

stlStatus smlDumpIncBackupFile( stlChar  * aFilePath,
                                stlInt8    aDumpBody,
                                stlUInt16  aTbsId,
                                stlInt64   aPageSeqId,
                                stlInt64   aFetchCnt,
                                smlEnv   * aEnv );
/** @} */

#endif /* _SMLDUMP_H_ */
