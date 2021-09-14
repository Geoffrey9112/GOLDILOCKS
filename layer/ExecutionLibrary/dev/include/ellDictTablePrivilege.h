/*******************************************************************************
 * ellDictTablePrivilege.h
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


#ifndef _ELL_DICT_TABLE_PRIVILEGE_H_
#define _ELL_DICT_TABLE_PRIVILEGE_H_ 1

/**
 * @file ellDictTablePrivilege.h
 * @brief Table Privilege Dictionary Management
 */

/**
 * @addtogroup ellPrivTable
 * @{
 */

stlStatus ellInsertTablePrivDesc( smlTransId           aTransID,
                                  smlStatement       * aStmt,
                                  stlInt64             aGrantorID,
                                  stlInt64             aGranteeID,
                                  stlInt64             aSchemaID,
                                  stlInt64             aTableID,
                                  ellTablePrivAction   aTablePrivAction,
                                  stlBool              aIsGrantable,
                                  stlBool              aWithHierarchy,
                                  ellEnv             * aEnv );


stlStatus ellGetGrantableAllTablePriv( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       ellDictHandle     * aTableHandle,
                                       ellPrivList      ** aPrivList,
                                       knlRegionMem      * aRegionMem,
                                       ellEnv            * aEnv );

stlStatus ellGetRevokableAllTablePriv( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       ellDictHandle     * aTableHandle,
                                       stlInt64            aRevokeeID,
                                       stlBool             aAccessControl,
                                       stlBool             aGrantOption,
                                       ellPrivList      ** aPrivList,
                                       knlRegionMem      * aRegionMem,
                                       ellEnv            * aEnv );

/** @} ellPrivTable */



#endif /* _ELL_DICT_TABLE_PRIVILEGE_H_ */
