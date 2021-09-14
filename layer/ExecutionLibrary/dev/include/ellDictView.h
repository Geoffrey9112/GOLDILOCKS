/*******************************************************************************
 * ellDictView.h
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


#ifndef _ELL_DICT_VIEW_H_
#define _ELL_DICT_VIEW_H_ 1

/**
 * @file ellDictView.h
 * @brief View Dictionary Management
 */

/**
 * @addtogroup ellObjectView
 * @{
 */

/*
 * DDL 관련 Object 획득 함수
 */

stlStatus ellGetViewColumnList( smlTransId          aTransID,
                                smlStatement      * aStmt,
                                ellDictHandle     * aViewHandle,
                                knlRegionMem      * aRegionMem,
                                ellObjectList    ** aViewColumnList,
                                ellEnv            * aEnv );

stlStatus ellGetViewColumnListByRelationList( smlTransId          aTransID,
                                              smlStatement      * aStmt,
                                              ellObjectList     * aRelationList,
                                              knlRegionMem      * aRegionMem,
                                              ellObjectList    ** aViewColumnList,
                                              ellEnv            * aEnv );

stlStatus ellGetAffectedViewListByRelation( smlTransId          aTransID,
                                            smlStatement      * aStmt,
                                            ellDictHandle     * aRelationHandle,
                                            knlRegionMem      * aRegionMem,
                                            ellObjectList    ** aAffectedViewList,
                                            ellEnv            * aEnv );

stlStatus ellGetAffectedOuterViewList4DropNonSchema( smlTransId          aTransID,
                                                     smlStatement      * aStmt,
                                                     ellObjectList     * aRelationList,
                                                     knlRegionMem      * aRegionMem,
                                                     ellObjectList    ** aAffectedViewList,
                                                     ellEnv            * aEnv );


/*
 * DDL Lock 함수 
 */

stlStatus ellLock4CreateView( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              ellDictHandle * aOwnerHandle,
                              ellDictHandle * aSchemaHandle,
                              stlBool       * aLockSuccess,
                              ellEnv        * aEnv );
    
stlStatus ellLock4DropView( smlTransId      aTransID,
                            smlStatement  * aStmt,
                            ellDictHandle * aViewHandle,
                            stlBool       * aLockSuccess,
                            ellEnv        * aEnv );

stlStatus ellLock4AlterView( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellDictHandle * aViewHandle,
                             stlBool       * aLockSuccess,
                             ellEnv        * aEnv );

/*
 * Dictionary 변경 함수 
 */

stlStatus ellInsertViewDesc( smlTransId     aTransID,
                             smlStatement * aStmt,
                             stlInt64       aOwnerID,
                             stlInt64       aSchemaID,
                             stlInt64       aViewedTableID,
                             stlChar      * aViewColumns,
                             stlChar      * aViewString,
                             stlBool        aIsCompiled,
                             stlBool        aIsAffected,
                             ellEnv       * aEnv );

stlStatus ellRemoveDict4DropView( smlTransId        aTransID,
                                  smlStatement    * aStmt,
                                  ellDictHandle   * aViewHandle,
                                  ellEnv          * aEnv );

stlStatus ellRemoveDict4AlterView( smlTransId        aTransID,
                                   smlStatement    * aStmt,
                                   ellDictHandle   * aViewHandle,
                                   ellEnv          * aEnv );

stlStatus ellRemoveDict4ReplaceView( smlTransId        aTransID,
                                     smlStatement    * aStmt,
                                     ellDictHandle   * aViewHandle,
                                     ellEnv          * aEnv );

stlStatus ellSetAffectedViewList( smlTransId          aTransID,
                                  smlStatement      * aStmt,
                                  ellObjectList     * aAffectedViewList,
                                  ellEnv            * aEnv );
                                  
/*
 * Cache 재구축 함수
 */

stlStatus ellRefineCache4RemoveViewColumns( smlTransId        aTransID,
                                            smlStatement    * aStmt,
                                            ellObjectList   * aViewColumnList,
                                            ellEnv          * aEnv );

stlStatus ellRefineCache4CreateView( smlTransId           aTransID,
                                     smlStatement       * aStmt,
                                     stlInt64             aViewedTableID,
                                     ellEnv             * aEnv );

stlStatus ellRefineCache4DropView( smlTransId        aTransID,
                                   ellDictHandle   * aViewHandle,
                                   ellEnv          * aEnv );

stlStatus ellRefineCache4ReplaceView( smlTransId        aTransID,
                                      smlStatement    * aStmt,
                                      ellDictHandle   * aViewHandle,
                                      ellEnv          * aEnv );

stlStatus ellRefineCache4AlterViewCompile( smlTransId        aTransID,
                                           smlStatement    * aStmt,
                                           ellDictHandle   * aViewHandle,
                                           ellEnv          * aEnv );


stlStatus ellAddNonServicePerfViewCache( stlInt64       aOwnerID,
                                         stlInt64       aSchemaID,
                                         stlChar      * aViewName,
                                         stlChar      * aViewColumnString,
                                         stlChar      * aViewQueryString,
                                         ellEnv       * aEnv );

/*
 * 정보 획득 함수
 */

stlChar    * ellGetViewColumnString( ellDictHandle * aTableDictHandle );
stlChar    * ellGetViewQueryString( ellDictHandle * aTableDictHandle );

/** @} ellObjectView */



#endif /* _ELL_DICT_VIEW_H_ */
