/*******************************************************************************
 * ellDictUsagePrivilege.h
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


#ifndef _ELL_DICT_USAGE_PRIVILEGE_H_
#define _ELL_DICT_USAGE_PRIVILEGE_H_ 1

/**
 * @file ellDictUsagePrivilege.h
 * @brief Usage Privilege Dictionary Management
 */

/**
 * @addtogroup ellPrivUsage
 * @{
 */

stlStatus ellInsertUsagePrivDesc( smlTransId          aTransID,
                                  smlStatement      * aStmt,
                                  stlInt64            aGrantorID,
                                  stlInt64            aGranteeID,
                                  stlInt64            aSchemaID,
                                  stlInt64            aObjectID,
                                  ellUsageObjectType  aUsageObjectType,
                                  stlBool             aIsGrantable,
                                  ellEnv            * aEnv );

stlStatus ellGetGrantableAllSeqPriv( smlTransId          aTransID,
                                     smlStatement      * aStmt,
                                     ellDictHandle     * aSeqHandle,
                                     ellPrivList      ** aPrivList,
                                     knlRegionMem      * aRegionMem,
                                     ellEnv            * aEnv );

stlStatus ellGetRevokableAllSeqPriv( smlTransId          aTransID,
                                     smlStatement      * aStmt,
                                     ellDictHandle     * aSeqHandle,
                                     stlInt64            aRevokeeID,
                                     stlBool             aAccessControl,
                                     stlBool             aGrantOption,
                                     ellPrivList      ** aPrivList,
                                     knlRegionMem      * aRegionMem,
                                     ellEnv            * aEnv );

/** @} ellPrivUsage */



#endif /* _ELL_DICT_TABLE_PRIVILEGE_H_ */
