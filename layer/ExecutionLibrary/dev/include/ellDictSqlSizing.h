/*******************************************************************************
 * ellDictSqlSizing.h
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


#ifndef _ELL_DICT_SQL_SIZING_H_
#define _ELL_DICT_SQL_SIZING_H_ 1

/**
 * @file ellDictSqlSizing.h
 * @brief SqlSizing Dictionary Management
 */

/**
 * @addtogroup ellSqlSizing
 * @{
 */


stlStatus ellInsertSqlSizingDesc( smlTransId     aTransID,
                                  smlStatement * aStmt,
                                  stlInt32       aID,
                                  stlChar      * aName,
                                  stlInt32       aValue,
                                  stlChar      * aComments,
                                  ellEnv       * aEnv );

/** @} ellSqlSizing */



#endif /* _ELL_DICT_SQL_SIZING_H_ */
