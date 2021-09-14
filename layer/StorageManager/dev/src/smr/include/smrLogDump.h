/*******************************************************************************
 * smrLogDump.h
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


#ifndef _SMRLOGDUMP_H_
#define _SMRLOGDUMP_H_ 1

/**
 * @file smrLogDump.h
 * @brief Storage Manager Layer Log File Dump Component Internal Routines
 */

/**
 * @defgroup smrLogDump Log File Dump
 * @ingroup smrInternal
 * @{
 */

stlStatus smrOpenLogCursor4Disk( smrLogCursor4Disk * aLogCursor,
                                 stlChar           * aFilePath,
                                 stlInt32            aHintBlockSeq,
                                 smrLsn              aHintLsn,
                                 smrLogFileHdr     * aLogFileHdr,
                                 smlEnv            * aEnv );
stlStatus smrReadBlockCursor4Disk( smrLogCursor4Disk  * aLogCursor,
                                   stlChar           ** aLogBlock,
                                   stlBool            * aEndOfFile,
                                   smlEnv             * aEnv );
stlBool smrIsValidLog( smrLogCursor4Disk * aLogCursor,
                       smrLogBlockHdr    * aLogBlockHdr );
stlStatus smrCloseLogCursor4Disk( smrLogCursor4Disk * aLogCursor,
                                  smlEnv            * aEnv );

/** @} */

#endif /* _SMRLOGDUMP_H_ */
