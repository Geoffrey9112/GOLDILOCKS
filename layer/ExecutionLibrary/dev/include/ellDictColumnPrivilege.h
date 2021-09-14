/*******************************************************************************
 * ellDictColumnPrivilege.h
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


#ifndef _ELL_DICT_COLUMN_PRIVILEGE_H_
#define _ELL_DICT_COLUMN_PRIVILEGE_H_ 1

/**
 * @file ellDictColumnPrivilege.h
 * @brief Column Privilege Dictionary Management
 */

/**
 * @addtogroup ellPrivColumn
 * @{
 */

stlStatus ellInsertColumnPrivDesc( smlTransId            aTransID,
                                   smlStatement        * aStmt,
                                   stlInt64              aGrantorID,
                                   stlInt64              aGranteeID,
                                   stlInt64              aSchemaID,
                                   stlInt64              aTableID,
                                   stlInt64              aColumnID,
                                   ellColumnPrivAction   aColumnPrivAction,
                                   stlBool               aIsGrantable,
                                   ellEnv              * aEnv );

stlStatus ellGetRevokableColumnPrivByAccessControl( smlTransId           aTransID,
                                                    smlStatement       * aStmt,
                                                    ellDictHandle      * aColumnHandle,
                                                    stlInt64             aRevokeeID,
                                                    ellColumnPrivAction  aColumnPrivAction,
                                                    stlBool            * aPrivExist,
                                                    stlBool            * aDuplicate,
                                                    ellPrivList        * aPrivList,
                                                    knlRegionMem       * aRegionMem,
                                                    ellEnv             * aEnv );

/** @} ellPrivColumn */



#endif /* _ELL_DICT_COLUMN_PRIVILEGE_H_ */
