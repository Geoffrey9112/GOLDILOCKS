/*******************************************************************************
 * eldView.h
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


#ifndef _ELD_VIEW_H_
#define _ELD_VIEW_H_ 1

/**
 * @file eldView.h
 * @brief View Dictionary 관리 루틴 
 */


/**
 * @defgroup eldView View Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertViewDesc( smlTransId     aTransID,
                             smlStatement * aStmt,
                             stlInt64       aOwnerID,
                             stlInt64       aSchemaID,
                             stlInt64       aViewedTableID,
                             stlChar      * aViewColumns,
                             stlChar      * aViewString,
                             stlBool        aIsCompiled,
                             stlBool        aIsAffected,
                             ellEnv       * aEnv );

stlStatus eldBuildViewCache( smlTransId       aTransID,
                             smlStatement   * aStmt,
                             stlInt64         aViewID,
                             ellEnv         * aEnv );

/** @} eldView */

#endif /* _ELD_VIEW_H_ */
