/*******************************************************************************
 * ellDictSequence.h
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


#ifndef _ELL_DICT_SEQUENCE_H_
#define _ELL_DICT_SEQUENCE_H_ 1

/**
 * @file ellDictSequence.h
 * @brief Column Dictionary Management
 */

/**
 * @addtogroup ellObjectSequence
 * @{
 */

/*
 * DDL Lock 함수 
 */

stlStatus ellLock4CreateSequence( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  ellDictHandle * aOwnerHandle,
                                  ellDictHandle * aSchemaHandle,
                                  stlBool       * aLockSuccess,
                                  ellEnv        * aEnv );

stlStatus ellLock4DropSequence( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                ellDictHandle * aSeqDictHandle,
                                stlBool       * aLockSuccess,
                                ellEnv        * aEnv );

stlStatus ellLock4AlterSequence( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 ellDictHandle * aSeqDictHandle,
                                 stlBool       * aLockSuccess,
                                 ellEnv        * aEnv );

stlStatus ellLockRelatedSeqList4DDL( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     ellObjectList * aSeqList,
                                     stlBool       * aLockSuccess,
                                     ellEnv        * aEnv );

/*
 * Dictionary 변경 함수 
 */

stlStatus ellInsertSequenceDesc( smlTransId           aTransID,
                                 smlStatement       * aStmt,
                                 stlInt64             aOwnerID,
                                 stlInt64             aSchemaID,
                                 stlInt64           * aSequenceID,
                                 stlInt64             aSeqTableID,
                                 stlInt64             aTablespaceID,
                                 stlInt64             aPhysicalID,
                                 stlChar            * aSequenceName,
                                 stlInt64             aStart,
                                 stlInt64             aMinimum,
                                 stlInt64             aMaximum,
                                 stlInt64             aIncrement,
                                 stlBool              aIsCycle,
                                 stlInt32             aCacheSize,
                                 stlChar            * aComment,
                                 ellEnv             * aEnv );

stlStatus ellRemoveDict4DropSequence( smlTransId        aTransID,
                                      smlStatement    * aStmt,
                                      ellDictHandle   * aSeqDictHandle,
                                      ellEnv          * aEnv );

stlStatus ellRemoveDictSeqByObjectList( smlTransId        aTransID,
                                        smlStatement    * aStmt,
                                        ellObjectList   * aSeqObjectList,
                                        ellEnv          * aEnv );


stlStatus ellModifySequenceOption( smlTransId             aTransID,
                                   smlStatement         * aStmt,
                                   stlInt64               aSequenceID,
                                   stlInt64               aPhysicalID,
                                   stlInt64               aStart,
                                   stlInt64               aMinimum,
                                   stlInt64               aMaximum,
                                   stlInt64               aIncrement,
                                   stlBool                aIsCycle,
                                   stlInt32               aCacheSize,
                                   ellEnv               * aEnv );

/*
 * Cache 재구축 함수
 */

stlStatus ellRefineCache4CreateSequence( smlTransId           aTransID,
                                         smlStatement       * aStmt,
                                         stlInt64             aAuthID,
                                         stlInt64             aSequenceID,
                                         ellEnv             * aEnv );

stlStatus ellRefineCache4DropSequence( smlTransId        aTransID,
                                       smlStatement    * aStmt,
                                       ellDictHandle   * aSeqDictHandle,
                                       ellEnv          * aEnv );

stlStatus ellRefineCacheDropSeqByObjectList( smlTransId        aTransID,
                                             smlStatement    * aStmt,
                                             ellObjectList   * aSeqObjectList,
                                             ellEnv          * aEnv );

stlStatus ellRefineCache4AlterSequence( smlTransId        aTransID,
                                        smlStatement    * aStmt,
                                        ellDictHandle   * aSeqDictHandle,
                                        ellEnv          * aEnv );

/*
 * Handle 획득 함수 
 */

stlStatus ellGetSequenceDictHandleWithSchema( smlTransId           aTransID,
                                              smlScn               aViewSCN,
                                              ellDictHandle      * aSchemaDictHandle,
                                              stlChar            * aSequenceName,
                                              ellDictHandle      * aSequenceDictHandle,
                                              stlBool            * aExist,
                                              ellEnv             * aEnv );

stlStatus ellGetSequenceDictHandleWithAuth( smlTransId           aTransID,
                                            smlScn               aViewSCN,
                                            ellDictHandle      * aAuthDictHandle,
                                            stlChar            * aSequenceName,
                                            ellDictHandle      * aSequenceDictHandle,
                                            stlBool            * aExist,
                                            ellEnv             * aEnv );

stlStatus ellGetSequenceDictHandleByID( smlTransId           aTransID,
                                        smlScn               aViewSCN,
                                        stlInt64             aSequenceID,
                                        ellDictHandle      * aSequenceDictHandle,
                                        stlBool            * aExist,
                                        ellEnv             * aEnv );


/*
 * 정보 획득 함수
 */

stlInt64     ellGetSequenceID( ellDictHandle * aSequenceDictHandle );
stlInt64     ellGetSeqTableID( ellDictHandle * aSequenceDictHandle );
stlInt64     ellGetSequenceOwnerID( ellDictHandle * aSequenceDictHandle );
stlInt64     ellGetSequenceSchemaID( ellDictHandle * aSequenceDictHandle );
stlChar    * ellGetSequenceName( ellDictHandle * aSequenceDictHandle );
void       * ellGetSequenceHandle( ellDictHandle * aSequenceDictHandle );
stlBool      ellIsBuiltInSequence( ellDictHandle * aSequenceDictHandle );

/** @} ellObjectSequence */



#endif /* _ELL_DICT_SEQUENCE_H_ */
