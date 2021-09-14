/*******************************************************************************
 * smxlScnMgr.h
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


#ifndef _SMXLSCNMGR_H_
#define _SMXLSCNMGR_H_ 1

/**
 * @file smxlScnMgr.h
 * @brief Storage Manager Layer Local Transactional Undo Relaion Component Internal Routines
 */

/**
 * @defgroup smxlScnMgr Undo Relation
 * @ingroup smxlInternal
 * @{
 */

stlStatus smxlWriteCommitLog( smxlTransId   aTransId,
                              smlScn        aCommitScn,
                              stlBool       aIsCommit,
                              stlChar     * aComment,
                              stlInt64      aWriteMode,
                              smrSbsn     * aCommitSbsn,
                              smrLsn      * aCommitLsn,
                              smlEnv      * aEnv );

stlStatus smxlWriteInDoubtCommitLog( smxlTransId   aTransId,
                                     smxlTransId   aInDoubtTransId,
                                     smlScn        aCommitScn,
                                     smlScn        aInDoubtCommitScn,
                                     stlBool       aIsInDoubtCommit,
                                     stlChar     * aComment,
                                     smrSbsn     * aCommitSbsn,
                                     smrLsn      * aCommitLsn,
                                     smrSbsn     * aInDoubtCommitSbsn,
                                     smrLsn      * aInDoubtCommitLsn,
                                     smlEnv      * aEnv );

/** @} */

#endif /* _SMXLSCNMGR_H_ */
