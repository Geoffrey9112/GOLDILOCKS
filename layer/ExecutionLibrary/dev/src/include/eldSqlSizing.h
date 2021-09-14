/*******************************************************************************
 * eldSqlSizing.h
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


#ifndef _ELD_SQL_SIZING_H_
#define _ELD_SQL_SIZING_H_ 1

/**
 * @file eldSqlSizing.h
 * @brief SQL SIZING Dictionary 관리 루틴 
 */


/**
 * @defgroup eldSqlSizing SQL SIZING Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertSqlSizingDesc( smlTransId     aTransID,
                                  smlStatement * aStmt,
                                  stlInt32       aID,
                                  stlChar      * aName,
                                  stlInt32       aValue,
                                  stlChar      * aComments,
                                  ellEnv       * aEnv );


/** @} eldSqlSizing */

#endif /* _ELD_SQL_SIZING_H_ */
