/*******************************************************************************
 * ellDictSynonym.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: 
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _ELL_DICT_SYNONYM_H_
#define _ELL_DICT_SYNONYM_H_ 1

/**
 * @file ellDictSynonym.h
 * @brief Synonym Dictionary Management
 */

/**
 * @addtogroup ellObjectSynonym
 * @{
 */

/*
 * DDL Lock 함수 
 */

stlStatus ellLock4CreateSynonym( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 ellDictHandle * aOwnerHandle,
                                 ellDictHandle * aSchemaHandle,
                                 stlBool       * aLockSuccess,
                                 ellEnv        * aEnv );


stlStatus ellLock4DropSynonym( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               ellDictHandle * aSynonymDictHandle,
                               stlBool       * aLockSuccess,
                               ellEnv        * aEnv );

stlStatus ellLockRelatedSynonymList4DDL( smlTransId      aTransID,
                                         smlStatement  * aStmt,
                                         ellObjectList * aSynonymList,
                                         stlBool       * aLockSuccess,
                                         ellEnv        * aEnv );


/*
 * Dictionary 변경 함수 
 */

stlStatus ellInsertSynonymDesc( smlTransId           aTransID,
                                smlStatement       * aStmt,
                                stlInt64             aOwnerID,
                                stlInt64             aSchemaID,
                                stlInt64             aSynonymID,
                                stlInt64             aSynonymTableID,
                                stlChar            * aSynonymName,
                                stlChar            * aObjectSchemaName,
                                stlChar            * aObjectName,
                                ellEnv             * aEnv );

stlStatus ellRemoveDict4DropSynonym( smlTransId        aTransID,
                                     smlStatement    * aStmt,
                                     ellDictHandle   * aSynonymDictHandle,
                                     ellEnv          * aEnv );

stlStatus ellRemoveDict4ReplaceSynonym( smlTransId        aTransID,
                                        smlStatement    * aStmt,
                                        ellDictHandle   * aSynonymDictHandle,
                                        ellEnv          * aEnv );

stlStatus ellRemoveDictSynonymByObjectList( smlTransId        aTransID,
                                            smlStatement    * aStmt,
                                            ellObjectList   * aSynonymObjectList,
                                            ellEnv          * aEnv );

/*
 * Cache 재구축 함수
 */

stlStatus ellRefineCache4CreateSynonym( smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        stlInt64             aSynonymID,
                                        ellEnv             * aEnv );

stlStatus ellRefineCache4DropSynonym( smlTransId        aTransID,
                                      smlStatement    * aStmt,
                                      ellDictHandle   * aSynonymDictHandle,
                                      ellEnv          * aEnv );

stlStatus ellRefineCacheDropSynonymByObjectList( smlTransId      aTransID,
                                                 smlStatement  * aStmt,
                                                 ellObjectList * aSynonymObjectList,
                                                 ellEnv        * aEnv );

/*
 * Handle 획득 함수 
 */

stlStatus ellGetSynonymDictHandleWithSchema( smlTransId           aTransID,
                                             smlScn               aViewSCN,
                                             ellDictHandle      * aSchemaDictHandle,
                                             stlChar            * aSynonymName,
                                             ellDictHandle      * aSynonymDictHandle,
                                             stlBool            * aExist,
                                             ellEnv             * aEnv );

stlStatus ellGetSynonymDictHandleWithAuth( smlTransId           aTransID,
                                           smlScn               aViewSCN,
                                           ellDictHandle      * aAuthDictHandle,
                                           stlChar            * aSynonymName,
                                           ellDictHandle      * aSynonymDictHandle,
                                           stlBool            * aExist,
                                           ellEnv             * aEnv );

/*
 * 정보 획득 함수
 */
stlStatus ellGetNewSynonymID( stlInt64 * aSynonymID,
                              ellEnv   * aEnv );

stlInt64 ellGetSynonymID( ellDictHandle * aSynonymDictHandle );
stlInt64 ellGetSynonymTableID( ellDictHandle * aSynonymDictHandle );
stlInt64 ellGetSynonymOwnerID( ellDictHandle * aSynonymDictHandle );
stlInt64 ellGetSynonymSchemaID( ellDictHandle * aSynonymDictHandle );
stlChar * ellGetSynonymName( ellDictHandle * aSynonymDictHandle );
stlChar * ellGetSynonymObjectSchemaName( ellDictHandle * aSynonymDictHandle );
stlChar * ellGetSynonymObjectName( ellDictHandle * aSynonymDictHandle );

/** @} ellObjectSynonym */



#endif /* _ELL_DICT_SYNONYM_H_ */
