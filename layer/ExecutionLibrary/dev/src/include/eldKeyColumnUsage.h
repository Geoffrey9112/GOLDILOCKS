/*******************************************************************************
 * eldKeyColumnUsage.h
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


#ifndef _ELD_KEY_COLUMN_USAGE_H_
#define _ELD_KEY_COLUMN_USAGE_H_ 1

/**
 * @file eldKeyColumnUsage.h
 * @brief Key Column Usage Dictionary 관리 루틴 
 */


/**
 * @defgroup eldKeyColumnUsage Key Column Usage Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertKeyColumnUsageDesc( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       stlInt64         aKeyOwnerID,
                                       stlInt64         aKeySchemaID,
                                       stlInt64         aKeyConstID,
                                       stlInt64         aRefOwnerID,
                                       stlInt64         aRefSchemaID,
                                       stlInt64         aRefTableID,
                                       stlInt64         aRefColumnID,
                                       stlInt32         aKeyOrdinalPosition,
                                       stlInt32         aPositionInUniqueConst,
                                       ellEnv         * aEnv );


/** @} eldKeyColumnUsage */

#endif /* _ELD_KEY_COLUMN_USAGE_H_ */
