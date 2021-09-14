/*******************************************************************************
 * eldIndexKeyColumnUsage.h
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


#ifndef _ELD_INDEX_KEY_COLUMN_USAGE_H_
#define _ELD_INDEX_KEY_COLUMN_USAGE_H_ 1

/**
 * @file eldIndexKeyColumnUsage.h
 * @brief Index Key Column Usage Dictionary 관리 루틴 
 */


/**
 * @defgroup eldIndexKeyColumnUsage Index Key Column Usage Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertIndexKeyColumnUsageDesc( smlTransId       aTransID,
                                            smlStatement   * aStmt,
                                            stlInt64         aIndexOwnerID,
                                            stlInt64         aIndexSchemaID,
                                            stlInt64         aIndexID,
                                            stlInt64         aRefOwnerID,
                                            stlInt64         aRefSchemaID,
                                            stlInt64         aRefTableID,
                                            stlInt64         aRefColumnID,
                                            stlInt32         aKeyOrdinalPosition,
                                            ellIndexColumnOrdering      aColumnOrdering,
                                            ellIndexColumnNullsOrdering aNullsOrdering,
                                            ellEnv         * aEnv );

/** @} eldIndexKeyColumnUsage */

#endif /* _ELD_INDEX_KEY_COLUMN_USAGE_H_ */
