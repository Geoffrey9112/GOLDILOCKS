/*******************************************************************************
 * ellDictDatabasePrivilege.h
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


#ifndef _ELL_DICT_DATABASE_PRIVILEGE_H_
#define _ELL_DICT_DATABASE_PRIVILEGE_H_ 1

/**
 * @file ellDictDatabasePrivilege.h
 * @brief Database Privilege Dictionary Management
 */

/**
 * @addtogroup ellPrivDatabase
 * @{
 */

stlStatus ellInsertDatabasePrivDesc( smlTransId              aTransID,
                                     smlStatement          * aStmt,
                                     stlInt64                aGrantorID,
                                     stlInt64                aGranteeID,
                                     ellDatabasePrivAction   aDatabasePrivAction,
                                     stlBool                 aIsGrantable,
                                     ellEnv                * aEnv );

stlStatus ellGetGrantableAllDatabasePriv( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          ellPrivList      ** aPrivList,
                                          knlRegionMem      * aRegionMem,
                                          ellEnv            * aEnv );

stlStatus ellGetRevokableAllDatabasePriv( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aRevokeeID,
                                          stlBool             aAccessControl,
                                          stlBool             aGrantOption,
                                          ellPrivList      ** aPrivList,
                                          knlRegionMem      * aRegionMem,
                                          ellEnv            * aEnv );

                                 
/** @} ellPrivDatabase */



#endif /* _ELL_DICT_DATABASE_PRIVILEGE_H_ */
