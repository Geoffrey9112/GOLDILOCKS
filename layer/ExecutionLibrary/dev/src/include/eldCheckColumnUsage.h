/*******************************************************************************
 * eldCheckColumnUsage.h
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


#ifndef _ELD_CHECK_COLUMN_USAGE_H_
#define _ELD_CHECK_COLUMN_USAGE_H_ 1

/**
 * @file eldCheckColumnUsage.h
 * @brief Check Column Usage Dictionary 관리 루틴 
 */


/**
 * @defgroup eldCheckColumnUsage Check Column Usage Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertCheckColumnUsageDesc( smlTransId       aTransID,
                                         smlStatement   * aStmt,
                                         stlInt64         aCheckOwnerID,
                                         stlInt64         aCheckSchemaID,
                                         stlInt64         aCheckConstID,
                                         stlInt64         aRefOwnerID,
                                         stlInt64         aRefSchemaID,
                                         stlInt64         aRefTableID,
                                         stlInt64         aRefColumnID,
                                         ellEnv         * aEnv );


/** @} eldCheckColumnUsage */

#endif /* _ELD_CHECK_COLUMN_USAGE_H_ */
