/*******************************************************************************
 * ellDictIndex.h
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


#ifndef _ELL_DICT_INDEX_H_
#define _ELL_DICT_INDEX_H_ 1

/**
 * @file ellDictIndex.h
 * @brief Index Dictionary Management
 */

/**
 * @addtogroup ellObjectIndex
 * @{
 */

/*
 * DDL 관련 Object 획득 함수
 */

stlStatus ellGetTableList4IndexList( smlTransId          aTransID,
                                     smlScn              aViewSCN,
                                     ellObjectList     * aIndexList,
                                     knlRegionMem      * aRegionMem,
                                     ellObjectList    ** aTableList,
                                     ellEnv            * aEnv );
                                     
                                         
/*
 * DDL Lock 함수 
 */

stlStatus ellLock4CreateIndex( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               ellDictHandle * aOwnerHandle,
                               ellDictHandle * aSpaceHandle,
                               ellDictHandle * aSchemaHandle,
                               ellDictHandle * aTableDictHandle,
                               stlBool       * aLockSuccess,
                               ellEnv        * aEnv );

stlStatus ellLock4DropIndex( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellDictHandle * aTableDictHandle,
                             ellDictHandle * aIndexDictHandle,
                             stlBool       * aLockSuccess,
                             ellEnv        * aEnv );

stlStatus ellLock4IndexList( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellObjectList * aIndexList,
                             stlBool       * aLockSuccess,
                             ellEnv        * aEnv );

stlStatus ellLock4AlterIndexPhysicalAttr( smlTransId      aTransID,
                                          smlStatement  * aStmt,
                                          ellDictHandle * aTableDictHandle,
                                          ellDictHandle * aIndexDictHandle,
                                          stlBool       * aLockSuccess,
                                          ellEnv        * aEnv );

/*
 * Dictionary 변경 함수 
 */

stlStatus ellInsertIndexDesc( smlTransId     aTransID,
                              smlStatement * aStmt,
                              stlInt64       aOwnerID,
                              stlInt64       aSchemaID,
                              stlInt64     * aIndexID,
                              stlInt64       aTablespaceID,
                              stlInt64       aPhysicalID,
                              stlChar      * aIndexName,
                              ellIndexType   aIndexType,
                              stlBool        aUnique,
                              stlBool        aInvalid,
                              stlBool        aByConstraint,
                              stlChar      * aIndexComment,
                              ellEnv       * aEnv );

stlStatus ellDeleteDict4DropIndex( smlTransId        aTransID,
                                   smlStatement    * aStmt,
                                   ellDictHandle   * aIndexHandle,
                                   ellEnv          * aEnv );

stlStatus ellDeleteDictIndexByObjectList( smlTransId        aTransID,
                                          smlStatement    * aStmt,
                                          ellObjectList   * aIndexObjectList,
                                          ellEnv          * aEnv );

stlStatus ellSetTimeAlterIndex( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                stlInt64        aIndexID,
                                ellEnv        * aEnv );

stlStatus ellModifyIndexPhysicalID( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    stlInt64       aIndexID,
                                    stlInt64       aPhysicalID,
                                    ellEnv       * aEnv );

stlStatus ellModifyKeyIndex2UserIndex( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       ellDictHandle * aIndexHandle,
                                       ellEnv        * aEnv );

stlStatus ellModifyKeyIndex2UserIndexByObjectList( smlTransId        aTransID,
                                                   smlStatement    * aStmt,
                                                   ellObjectList   * aIndexObjectList,
                                                   ellEnv          * aEnv );



/*
 * Cache 재구축 함수
 */

stlStatus ellRebuildIndexCache( smlTransId        aTransID,
                                smlStatement    * aStmt,
                                ellDictHandle   * aIndexDictHandle,
                                ellEnv          * aEnv );

stlStatus ellRefineCache4CreateIndex( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      stlInt64             aAuthID,
                                      stlInt64             aIndexID,
                                      ellEnv             * aEnv );


stlStatus ellRefineCache4DropIndex( smlTransId        aTransID,
                                    ellDictHandle   * aTableHandle,
                                    ellDictHandle   * aIndexHandle,
                                    ellEnv          * aEnv );

stlStatus ellRefineCache4DropKeyIndex( smlTransId        aTransID,
                                       ellDictHandle   * aIndexHandle,
                                       ellEnv          * aEnv );

stlStatus ellRefineCache4KeepKeyIndex( smlTransId        aTransID,
                                       smlStatement    * aStmt,
                                       ellDictHandle   * aTableHandle,
                                       ellDictHandle   * aIndexHandle,
                                       ellEnv          * aEnv );


/*
 * Index DDL 관련 RunTime 검증 함수 
 */

stlStatus ellRuntimeCheckSameKey4UniqueKey( smlTransId      aTransID,
                                            ellDictHandle * aTableHandle,
                                            ellDictHandle * aIndexHandle,
                                            stlBool       * aExist,
                                            ellEnv        * aEnv );

stlStatus ellRuntimeCheckSameKey4ForeignKey( smlTransId      aTransID,
                                             ellDictHandle * aTableHandle,
                                             ellDictHandle * aIndexHandle,
                                             stlBool       * aExist,
                                             ellEnv        * aEnv );

stlStatus ellRuntimeCheckSameKey4Index( smlTransId      aTransID,
                                        ellDictHandle * aTableHandle,
                                        ellDictHandle * aIndexHandle,
                                        stlBool       * aExist,
                                        ellEnv        * aEnv );


/*
 * Handle 획득 함수 
 */

stlStatus ellGetIndexDictHandleWithSchema( smlTransId           aTransID,
                                           smlScn               aViewSCN,
                                           ellDictHandle      * aSchemaDictHandle,
                                           stlChar            * aIndexName,
                                           ellDictHandle      * aIndexDictHandle,
                                           stlBool            * aExist,
                                           ellEnv             * aEnv );

stlStatus ellGetIndexDictHandleWithAuth( smlTransId           aTransID,
                                         smlScn               aViewSCN,
                                         ellDictHandle      * aAuthDictHandle,
                                         stlChar            * aIndexName,
                                         ellDictHandle      * aIndexDictHandle,
                                         stlBool            * aExist,
                                         ellEnv             * aEnv );

stlStatus ellGetIndexDictHandleByID( smlTransId           aTransID,
                                     smlScn               aViewSCN,
                                     stlInt64             aIndexID,
                                     ellDictHandle      * aIndexDictHandle,
                                     stlBool            * aExist,
                                     ellEnv             * aEnv );

stlStatus ellGetTableUniqueIndexDictHandle( smlTransId       aTransID,
                                            smlScn           aViewSCN,
                                            knlRegionMem   * aRegionMem,
                                            ellDictHandle  * aTableDictHandle,
                                            stlInt32       * aUnqiueIndexCnt,
                                            ellDictHandle ** aIndexDictHandleArray,
                                            ellEnv         * aEnv );

stlStatus ellGetTableNonUniqueIndexDictHandle( smlTransId       aTransID,
                                               smlScn           aViewSCN,
                                               knlRegionMem   * aRegionMem,
                                               ellDictHandle  * aTableDictHandle,
                                               stlInt32       * aNonUnqiueIndexCnt,
                                               ellDictHandle ** aIndexDictHandleArray,
                                               ellEnv         * aEnv );

/*
 * 정보 획득 함수
 */


stlInt64     ellGetIndexID( ellDictHandle * aIndexDictHandle );
stlInt64     ellGetIndexOwnerID( ellDictHandle * aIndexDictHandle );
stlInt64     ellGetIndexSchemaID( ellDictHandle * aIndexDictHandle );
stlInt64     ellGetIndexTableID( ellDictHandle * aIndexDictHandle );
stlInt64     ellGetIndexTablespaceID( ellDictHandle * aIndexDictHandle );
stlChar    * ellGetIndexName( ellDictHandle * aIndexDictHandle );
stlBool      ellGetIndexIsUnique( ellDictHandle * aIndexDictHandle );
stlBool      ellGetIndexIsInvalid( ellDictHandle * aIndexDictHandle );
stlBool      ellGetIndexIsByConstraint( ellDictHandle * aIndexDictHandle );
void       * ellGetIndexHandle( ellDictHandle * aIndexDictHandle );
stlInt32     ellGetIndexKeyCount( ellDictHandle * aIndexDictHandle );
ellIndexKeyDesc * ellGetIndexKeyDesc( ellDictHandle * aIndexDictHandle );
stlBool      ellIsBuiltInIndex( ellDictHandle * aIndexDictHandle );

/** @} ellObjectIndex */



#endif /* _ELL_DICT_INDEX_H_ */
