/*******************************************************************************
 * eldViewTableUsage.h
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


#ifndef _ELD_VIEW_TABLE_USAGE_H_
#define _ELD_VIEW_TABLE_USAGE_H_ 1

/**
 * @file eldViewTableUsage.h
 * @brief View Table Usage Dictionary 관리 루틴 
 */


/**
 * @defgroup eldViewTableUsage View Table Usage Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertViewTableUsageDesc( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       stlInt64         aViewOwnerID,
                                       stlInt64         aViewSchemaID,
                                       stlInt64         aViewID,
                                       stlInt64         aTableOwnerID,
                                       stlInt64         aTableSchemaID,
                                       stlInt64         aTableID,
                                       ellEnv         * aEnv );


/** @} eldViewTableUsage */

#endif /* _ELD_VIEW_TABLE_USAGE_H_ */
