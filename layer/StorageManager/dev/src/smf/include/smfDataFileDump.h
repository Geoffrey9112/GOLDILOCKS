/*******************************************************************************
 * smfDataFileDump.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smfDataFileDump.h $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _SMFDATAFILEDUMP_H_
#define _SMFDATAFILEDUMP_H_ 1

/**
 * @file smfDataFileDump.h
 * @brief Storage Manager Layer Datafile Dump Component Internal Routines
 */

/**
 * @defgroup smfDataFileDump Datafile Dump
 * @ingroup smfInternal
 * @{
 */

#define SMF_DATAFILE_DUMP_LINE_LENGTH       32
#define SMF_DATAFILE_DUMP_PAGE_OFFSET_LEN   5
#define SMF_DATAFILE_DUMP_COLUMN_MAX_SIZE   17

#define SMF_DUMP_PAGE_TYPE_LEN              20
#define SMF_DUMP_FREENESS_TYPE_LEN          12
#define SMF_DUMP_INC_BACKUP_OBJECT_TYPE_LEN 11

void smfWriteHexStringToColumn( stlChar  * aColumn,
                                stlChar  * aHexString,
                                stlInt32 * aHexOffset,
                                stlInt32   aMaxSize );

void smfGetDumpPageType( smpPageType   aPageType,
                         stlChar     * aTypeStr );

void smfGetDumpFreeness( smpFreeness   aFreeness,
                         stlChar     * aFreeStr );

void smfGetIncBackupObjectType( smlTbsId   aTbsId,
                                stlChar  * aObject );

stlStatus smfDumpPage( stlChar * aPage,
                       stlInt8   aDumpFlag,
                       smlEnv  * aEnv );

/** @} */

#endif /* _SMFDATAFILEDUMP_H_ */
