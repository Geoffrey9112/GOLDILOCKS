/*******************************************************************************
 * ellDictSchema.h
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


#ifndef _ELL_DICT_SCHEMA_H_
#define _ELL_DICT_SCHEMA_H_ 1

/**
 * @file ellDictSchema.h
 * @brief Schema Dictionary Management
 */

/**
 * @addtogroup ellObjectSchema
 * @{
 */

/*
 * DDL 관련 Object 획득 함수
 */

stlStatus ellGetObjectList4DropSchema( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       ellDictHandle  * aSchemaHandle,
                                       knlRegionMem   * aRegionMem,
                                       ellObjectList ** aInnerSeqList,
                                       ellObjectList ** aInnerSynonymList,
                                       ellObjectList ** aInnerTableList,
                                       ellObjectList ** aOuterTableInnerPrimaryKeyList,
                                       ellObjectList ** aOuterTableInnerUniqueKeyList,
                                       ellObjectList ** aOuterTableInnerIndexList,
                                       ellObjectList ** aOuterTableInnerNotNullList,
                                       ellObjectList ** aOuterTableInnerCheckClauseList,
                                       ellObjectList ** aInnerForeignKeyList,
                                       ellObjectList ** aOuterChildForeignKeyList,
                                       ellEnv         * aEnv );

/*
 * DDL Lock 함수 
 */

stlStatus ellLock4CreateSchema( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                ellDictHandle * aOwnerHandle,
                                stlBool       * aLockSuccess,
                                ellEnv        * aEnv );

stlStatus ellLock4DropSchema( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              ellDictHandle * aSchemaHandle,
                              stlBool       * aLockSuccess,
                              ellEnv        * aEnv );

/*
 * Dictionary 변경 함수 
 */


stlStatus ellInsertSchemaDesc( smlTransId     aTransID,
                               smlStatement * aStmt,
                               stlInt64       aOwnerID,
                               stlInt64     * aSchemaID,
                               stlChar      * aSchemaName,
                               stlChar      * aSchemaComment,
                               ellEnv       * aEnv );

stlStatus ellRemoveDict4DropSchema( smlTransId        aTransID,
                                    smlStatement    * aStmt,
                                    ellDictHandle   * aSchemaHandle,
                                    ellEnv          * aEnv );

/*
 * Cache 재구축 함수
 */

stlStatus ellRefineCache4AddSchema( smlTransId           aTransID,
                                    smlStatement       * aStmt,
                                    stlInt64             aAuthID,
                                    stlInt64             aSchemaID,
                                    ellEnv             * aEnv );

stlStatus ellRefineCache4DropSchema( smlTransId        aTransID,
                                     ellDictHandle   * aSchemaHandle,
                                     ellEnv          * aEnv );

/*
 * Handle 획득 함수 
 */

stlStatus ellGetSchemaDictHandle( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  stlChar            * aSchemaName,
                                  ellDictHandle      * aSchemaDictHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv );

stlStatus ellGetSchemaDictHandleByID( smlTransId           aTransID,
                                      smlScn               aViewSCN,
                                      stlInt64             aSchemaID,
                                      ellDictHandle      * aSchemaDictHandle,
                                      stlBool            * aExist,
                                      ellEnv             * aEnv );

/*
 * 정보 획득 함수
 */

stlInt64     ellGetSchemaID( ellDictHandle * aSchemaDictHandle );
stlInt64     ellGetSchemaOwnerID( ellDictHandle * aSchemaDictHandle );
stlInt64     ellGetSchemaIdPUBLIC();
stlInt64     ellGetSchemaIdINFORMATION_SCHEMA();
stlInt64     ellGetSchemaIdDICTIONARY_SCHEMA();
stlChar    * ellGetSchemaName( ellDictHandle * aSchemaDictHandle );
stlBool      ellGetSchemaWritable( ellDictHandle * aSchemaDictHandle );
stlBool      ellIsBuiltInSchema( ellDictHandle * aSchemaDictHandle );


/** @} ellObjectSchema */



#endif /* _ELL_DICT_SCHEMA_H_ */
