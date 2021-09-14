/*******************************************************************************
 * eldIndexKeyTableUsage.h
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


#ifndef _ELD_INDEX_KEY_TABLE_USAGE_H_
#define _ELD_INDEX_KEY_TABLE_USAGE_H_ 1

/**
 * @file eldIndexKeyTableUsage.h
 * @brief Index Key Table Usage Dictionary 관리 루틴 
 */


/**
 * @defgroup eldIndexKeyTableUsage Index Key Table Usage Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertIndexKeyTableUsageDesc( smlTransId       aTransID,
                                           smlStatement   * aStmt,
                                           stlInt64         aIndexOwnerID,
                                           stlInt64         aIndexSchemaID,
                                           stlInt64         aIndexID,
                                           stlInt64         aRefOwnerID,
                                           stlInt64         aRefSchemaID,
                                           stlInt64         aRefTableID,
                                           ellEnv         * aEnv );

/** @} eldIndexKeyTableUsage */

#endif /* _ELD_INDEX_KEY_TABLE_USAGE_H_ */
