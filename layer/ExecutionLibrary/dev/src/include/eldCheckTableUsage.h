/*******************************************************************************
 * eldCheckTableUsage.h
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


#ifndef _ELD_CHECK_TABLE_USAGE_H_
#define _ELD_CHECK_TABLE_USAGE_H_ 1

/**
 * @file eldCheckTableUsage.h
 * @brief Check Table Usage Dictionary 관리 루틴 
 */


/**
 * @defgroup eldCheckTableUsage Check Table Usage Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertCheckTableUsageDesc( smlTransId       aTransID,
                                        smlStatement   * aStmt,
                                        stlInt64         aCheckOwnerID,
                                        stlInt64         aCheckSchemaID,
                                        stlInt64         aCheckConstID,
                                        stlInt64         aRefOwnerID,
                                        stlInt64         aRefSchemaID,
                                        stlInt64         aRefTableID,
                                        ellEnv         * aEnv );


/** @} eldCheckTableUsage */

#endif /* _ELD_CHECK_TABLE_USAGE_H_ */
