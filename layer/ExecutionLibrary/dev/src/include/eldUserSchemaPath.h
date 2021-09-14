/*******************************************************************************
 * eldUserSchemaPath.h
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


#ifndef _ELD_USER_SCHEMA_PATH_H_
#define _ELD_USER_SCHEMA_PATH_H_ 1

/**
 * @file eldUserSchemaPath.h
 * @brief User Schema Path Dictionary 관리 루틴 
 */


/**
 * @defgroup eldUserSchemaPath User Schema Path Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertUserSchemaPathDesc( smlTransId             aTransID,
                                       smlStatement         * aStmt,
                                       stlInt64               aAuthID,
                                       stlInt64               aSchemaID,
                                       stlInt32               aSearchOrder,
                                       ellEnv               * aEnv );


/** @} eldUserSchemaPath */

#endif /* _ELD_USER_SCHEMA_PATH_H_ */
