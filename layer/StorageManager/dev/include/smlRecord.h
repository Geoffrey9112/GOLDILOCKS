/*******************************************************************************
 * smlRecord.h
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


#ifndef _SMLRECORD_H_
#define _SMLRECORD_H_ 1

/**
 * @file smlRecord.h
 * @brief Storage Manager Record Routines
 */

/**
 * @defgroup smlRecord Record
 * @ingroup smExternal
 * @{
 */

stlStatus smlInsertRecord( smlStatement      * aStatement,
                           void              * aRelHandle,
                           knlValueBlockList * aInsertCols,
                           knlValueBlockList * aUniqueViolation,
                           smlRowBlock       * aRowBlock,
                           smlEnv            * aEnv );

stlStatus smlDeleteRecord( smlStatement      * aStatement,
                           void              * aRelHandle,
                           smlRid            * aRowRid,
                           smlScn              aRowScn,
                           stlInt32            aValueIdx,
                           knlValueBlockList * aPrimaryKey, /* for replication */
                           stlBool           * aVersionConflict,
                           stlBool           * aSkipped,
                           smlEnv            * aEnv );

stlStatus smlUpdateRecord( smlStatement      * aStatement,
                           void              * aRelHandle,
                           smlRid            * aRowRid,
                           smlScn              aRowScn,
                           stlInt32            aValueIdx,
                           knlValueBlockList * aAfterCols,
                           knlValueBlockList * aBeforeCols,
                           knlValueBlockList * aPrimaryKey, /* for replication */
                           stlBool           * aVersionConflict,
                           stlBool           * aSkipped,
                           smlEnv            * aEnv );

stlStatus smlMergeRecord( smlStatement       * aStatement,
                          void               * aRelHandle,
                          knlValueBlockList  * aInsertCols,
                          smlMergeRecordInfo * aMergeReocordInfo,
                          smlEnv             * aEnv );

stlStatus smlFetchRecord( smlStatement        * aStatement,
                          smlFetchRecordInfo  * aFetchRecordInfo,
                          smlRid              * aRowRid,
                          stlInt32              aBlockIdx,
                          stlBool             * aIsMatched,
                          stlBool             * aDeleted,
                          stlBool             * aUpdated,
                          smlEnv              * aEnv );

stlStatus smlFetchRecordWithRowid( smlStatement        * aStatement,
                                   smlFetchRecordInfo  * aFetchRecordInfo,
                                   smlRid              * aRowRid,
                                   stlInt32              aBlockIdx,
                                   stlBool             * aIsMatched,
                                   stlBool             * aDeleted,
                                   smlEnv              * aEnv );

stlStatus smlInitRowBlock( smlRowBlock  * aRowBlock,
                           stlInt32       aBlockSize,
                           knlRegionMem * aRegionMem,
                           smlEnv       * aEnv );

stlStatus smlInitSingleRowBlock( smlRowBlock  * aRowBlock,
                                 smlRid       * aRowRid,
                                 smlScn       * aRowScn,
                                 smlEnv       * aEnv );

/** @} */

#endif /* _SMLRECORD_H_ */
