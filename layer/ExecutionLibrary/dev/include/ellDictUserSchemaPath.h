/*******************************************************************************
 * ellDictUserSchemaPath.h
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


#ifndef _ELL_DICT_USER_SCHEMA_PATH_H_
#define _ELL_DICT_USER_SCHEMA_PATH_H_ 1

/**
 * @file ellDictUserSchemaPath.h
 * @brief User Schema Path Dictionary Management
 */

/**
 * @addtogroup ellObjectAuth
 * @{
 */


stlStatus ellInsertUserSchemaPathDesc( smlTransId             aTransID,
                                       smlStatement         * aStmt,
                                       stlInt64               aAuthID,
                                       stlInt64               aSchemaID,
                                       stlInt32               aSearchOrder,
                                       ellEnv               * aEnv );

/** @} ellObjectAuth */



#endif /* _ELL_DICT_USER_SCHEMA_PATH_H_ */
