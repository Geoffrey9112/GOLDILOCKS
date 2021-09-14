/*******************************************************************************
 * ellDictSchemaPrivilege.h
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


#ifndef _ELL_DICT_SCHEMA_PRIVILEGE_H_
#define _ELL_DICT_SCHEMA_PRIVILEGE_H_ 1

/**
 * @file ellDictSchemaPrivilege.h
 * @brief Schema Privilege Dictionary Management
 */

/**
 * @addtogroup ellPrivSchema
 * @{
 */

stlStatus ellInsertSchemaPrivDesc( smlTransId            aTransID,
                                   smlStatement        * aStmt,
                                   stlInt64              aGrantorID,
                                   stlInt64              aGranteeID,
                                   stlInt64              aSchemaID,
                                   ellSchemaPrivAction   aSchemaPrivAction,
                                   stlBool               aIsGrantable,
                                   ellEnv              * aEnv );

stlStatus ellGetGrantableAllSchemaPriv( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        ellDictHandle     * aSchemaHandle,
                                        ellPrivList      ** aPrivList,
                                        knlRegionMem      * aRegionMem,
                                        ellEnv            * aEnv );

stlStatus ellGetRevokableAllSchemaPriv( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        ellDictHandle     * aSchemaHandle,
                                        stlInt64            aRevokeeID,
                                        stlBool             aAccessControl,
                                        stlBool             aGrantOption,
                                        ellPrivList      ** aPrivList,
                                        knlRegionMem      * aRegionMem,
                                        ellEnv            * aEnv );

/** @} ellPrivTable */



#endif /* _ELL_DICT_SCHEMA_PRIVILEGE_H_ */
