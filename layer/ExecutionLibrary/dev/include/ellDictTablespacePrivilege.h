/*******************************************************************************
 * ellDictTablespacePrivilege.h
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


#ifndef _ELL_DICT_TABLESPACE_PRIVILEGE_H_
#define _ELL_DICT_TABLESPACE_PRIVILEGE_H_ 1

/**
 * @file ellDictTablespacePrivilege.h
 * @brief Tablespace Privilege Dictionary Management
 */

/**
 * @addtogroup ellPrivTablespace
 * @{
 */

stlStatus ellInsertTablespacePrivDesc( smlTransId              aTransID,
                                       smlStatement          * aStmt,
                                       stlInt64                aGrantorID,
                                       stlInt64                aGranteeID,
                                       stlInt64                aTablespaceID,
                                       ellSpacePrivAction      aSpacePrivAction,
                                       stlBool                 aIsGrantable,
                                       ellEnv                * aEnv );

stlStatus ellGetGrantableAllSpacePriv( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       ellDictHandle     * aSpaceHandle,
                                       ellPrivList      ** aPrivList,
                                       knlRegionMem      * aRegionMem,
                                       ellEnv            * aEnv );

stlStatus ellGetRevokableAllSpacePriv( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       ellDictHandle     * aSpaceHandle,
                                       stlInt64            aRevokeeID,
                                       stlBool             aAccessControl,
                                       stlBool             aGrantOption,
                                       ellPrivList      ** aPrivList,
                                       knlRegionMem      * aRegionMem,
                                       ellEnv            * aEnv );

/** @} ellPrivTable */



#endif /* _ELL_DICT_TABLESPACE_PRIVILEGE_H_ */
