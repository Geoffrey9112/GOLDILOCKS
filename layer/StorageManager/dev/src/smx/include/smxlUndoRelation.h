/*******************************************************************************
 * smxlUndoRelation.h
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


#ifndef _SMXLUNDORELATION_H_
#define _SMXLUNDORELATION_H_ 1

/**
 * @file smxlUndoRelation.h
 * @brief Storage Manager Layer Local Transactional Undo Relaion Component Internal Routines
 */

/**
 * @defgroup smxlUndoRelation Undo Relation
 * @ingroup smxlInternal
 * @{
 */

stlStatus smxlInsertTransRecordInternal( smxmTrans * aMiniTrans,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv );
stlStatus smxlPrepareTransRecordInternal( smxmTrans   * aMiniTrans,
                                          smxlTransId * aTransId, 
                                          smlEnv      * aEnv );

stlStatus smxlAllocPage( smxmTrans        * aMiniTrans,
                         smxlUndoRelCache * aUndoRelCache,
                         smpHandle        * aPageHandle,
                         smlEnv           * aEnv );

stlStatus smxlStealPages( smxmTrans        * aMiniTrans,
                          smxlUndoRelCache * aUndoRelCache,
                          stlInt64         * aStolenPageCount,
                          smlEnv           * aEnv );

/** @} */

#endif /* _SMXLUNDORELATION_H_ */
