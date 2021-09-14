/*******************************************************************************
 * eldSchema.h
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


#ifndef _ELD_SCHEMA_H_
#define _ELD_SCHEMA_H_ 1

/**
 * @file eldSchema.h
 * @brief Schema Dictionary 관리 루틴 
 */


/**
 * @defgroup eldSchema Schema Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertSchemaDesc( smlTransId     aTransID,
                               smlStatement * aStmt,
                               stlInt64       aOwnerID,
                               stlInt64     * aSchemaID,
                               stlChar      * aSchemaName,
                               stlChar      * aSchemaComment,
                               ellEnv       * aEnv );

stlStatus eldRefineCache4AddSchema( smlTransId           aTransID,
                                    smlStatement       * aStmt,
                                    stlInt64             aAuthID,
                                    stlInt64             aSchemaID,
                                    ellEnv             * aEnv );

stlStatus eldGetSchemaHandleByID( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  stlInt64             aSchemaID,
                                  ellDictHandle      * aSchemaDictHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv );

stlStatus eldGetSchemaHandleByName( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlChar            * aSchemaName,
                                    ellDictHandle      * aSchemaDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv );


/** @} eldSchema */

#endif /* _ELD_SCHEMA_H_ */
