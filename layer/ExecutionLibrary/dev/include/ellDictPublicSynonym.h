/*******************************************************************************
 * ellDictPublicSynonym.h
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


#ifndef _ELL_DICT_PUBLIC_SYNONYM_H_
#define _ELL_DICT_PUBLIC_SYNONYM_H_ 1

/**
 * @file ellDictPublicSynonym.h
 * @brief Public Synonym Dictionary Management
 */

/**
 * @addtogroup ellObjectPublicSynonym
 * @{
 */

/*
 * DDL Lock 함수 
 */

stlStatus ellLock4CreatePublicSynonym( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       ellDictHandle * aCreatorHandle,
                                       stlBool       * aLockSuccess,
                                       ellEnv        * aEnv );

stlStatus ellLock4DropPublicSynonym( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     ellDictHandle * aSynonymDictHandle,
                                     stlBool       * aLockSuccess,
                                     ellEnv        * aEnv );
/*
 * Dictionary 변경 함수 
 */

stlStatus ellInsertPublicSynonymDesc( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      stlInt64             aSynonymID,
                                      stlChar            * aSynonymName,
                                      stlInt64             aCreatorID,
                                      stlChar            * aObjectSchemaName,
                                      stlChar            * aObjectName,
                                      ellEnv             * aEnv );

stlStatus ellRemoveDict4DropPublicSynonym( smlTransId        aTransID,
                                           smlStatement    * aStmt,
                                           ellDictHandle   * aSynonymDictHandle,
                                           ellEnv          * aEnv );

/*
 * Cache 재구축 함수
 */

stlStatus ellRefineCache4CreatePublicSynonym( smlTransId           aTransID,
                                              smlStatement       * aStmt,
                                              stlInt64             aSynonymID,
                                              ellEnv             * aEnv );

stlStatus ellRefineCache4DropPublicSynonym( smlTransId        aTransID,
                                            smlStatement    * aStmt,
                                            ellDictHandle   * aSynonymDictHandle,
                                            ellEnv          * aEnv );


/*
 * Handle 획득 함수 
 */

stlStatus ellGetPublicSynonymDictHandle( smlTransId           aTransID,
                                         smlScn               aViewSCN,
                                         stlChar            * aSynonymName,
                                         ellDictHandle      * aSynonymDictHandle,
                                         stlBool            * aExist,
                                         ellEnv             * aEnv );

/*
 * 정보 획득 함수
 */

stlStatus ellGetNewPublicSynonymID( stlInt64 * aSynonymID,
                                    ellEnv   * aEnv );
stlInt64 ellGetPublicSynonymID( ellDictHandle * aPublicSynonymDictHandle );
stlChar * ellGetPublicSynonymName( ellDictHandle * aPublicSynonymDictHandle );
stlChar * ellGetPublicSynonymObjectSchemaName( ellDictHandle * aSynonymDictHandle );
stlChar * ellGetPublicSynonymObjectName( ellDictHandle * aSynonymDictHandle );


/** @} ellObjectPublicSynonym */



#endif /* _ELL_DICT_PUBLIC_SYNONYM_H_ */
