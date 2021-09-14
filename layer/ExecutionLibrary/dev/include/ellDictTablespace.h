/*******************************************************************************
 * ellDictTablespace.h
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


#ifndef _ELL_DICT_TABLESPACE_H_
#define _ELL_DICT_TABLESPACE_H_ 1

/**
 * @file ellDictTablespace.h
 * @brief Tablespace Dictionary Management
 */

/**
 * @addtogroup ellObjectTablespace
 * @{
 */


/*
 * DDL 관련 Object 획득 함수
 */

stlStatus ellGetObjectList4DropSpace( smlTransId       aTransID,
                                      smlStatement   * aStmt,
                                      ellDictHandle  * aSpaceHandle,
                                      knlRegionMem   * aRegionMem,
                                      ellObjectList ** aInnerTableList,
                                      ellObjectList ** aOuterTableInnerPrimaryKeyList,
                                      ellObjectList ** aOuterTableInnerUniqueKeyList,
                                      ellObjectList ** aOuterTableInnerIndexList,
                                      ellObjectList ** aInnerForeignKeyList,
                                      ellObjectList ** aOuterChildForeignKeyList,
                                      ellEnv         * aEnv );

stlStatus ellGetIndexObjectList4SpaceOnline( smlTransId       aTransID,
                                             smlStatement   * aStmt,
                                             ellDictHandle  * aSpaceHandle,
                                             knlRegionMem   * aRegionMem,
                                             ellObjectList ** aInnerTableIndexObjList,
                                             ellEnv         * aEnv );
                                   

/*
 * DDL Lock 함수 
 */

stlStatus ellLock4CreateTablespace( smlTransId      aTransID,
                                    smlStatement  * aStmt,
                                    ellDictHandle * aCreatorHandle,
                                    stlBool       * aLockSuccess,
                                    ellEnv        * aEnv );

stlStatus ellLock4DropTablespace( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  ellDictHandle * aSpaceHandle,
                                  stlBool       * aLockSuccess,
                                  ellEnv        * aEnv );

stlStatus ellLock4AlterTablespace( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   ellDictHandle * aSpaceHandle,
                                   stlBool       * aLockSuccess,
                                   ellEnv        * aEnv );

/*
 * Dictionary 변경 함수 
 */


stlStatus ellSetTimeAlterTablespace( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     stlInt64        aTablespaceID,
                                     ellEnv        * aEnv );

stlStatus ellInsertTablespaceDesc( smlTransId               aTransID,
                                   smlStatement           * aStmt,
                                   stlInt64                 aTablespaceID,
                                   stlInt64                 aCreatorID,
                                   stlChar                * aTablespaceName,
                                   ellTablespaceMediaType   aMediaType,
                                   ellTablespaceUsageType   aUsageType,
                                   stlChar                * aTablespaceComment,
                                   ellEnv                 * aEnv );

stlStatus ellModifySpaceName( smlTransId     aTransID,
                              smlStatement * aStmt,
                              stlInt64       aSpaceID,
                              stlChar      * aNewName,
                              ellEnv       * aEnv );

stlStatus ellDeleteDict4DropSpace( smlTransId        aTransID,
                                   smlStatement    * aStmt,
                                   ellDictHandle   * aSpaceHandle,
                                   ellEnv          * aEnv );

/*
 * Cache 재구축 함수
 */

stlStatus ellRebuildSpaceCache( smlTransId        aTransID,
                                smlStatement    * aStmt,
                                ellDictHandle   * aSpaceHandle,
                                ellEnv          * aEnv );

stlStatus ellRefineCache4AddTablespace( smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        stlInt64             aAuthID,
                                        stlInt64             aTablespaceID,
                                        ellEnv             * aEnv );

stlStatus ellRefineCache4DropSpace( smlTransId        aTransID,
                                    smlStatement    * aStmt,
                                    ellDictHandle   * aSpaceHandle,
                                    ellEnv          * aEnv );

stlStatus ellRefineCache4RenameSpace( smlTransId        aTransID,
                                      smlStatement    * aStmt,
                                      ellDictHandle   * aSpaceHandle,
                                      ellEnv          * aEnv );

/*
 * Handle 획득 함수
 */

stlStatus ellGetTablespaceDictHandle( smlTransId           aTransID,
                                      smlScn               aViewSCN,
                                      stlChar            * aTablespaceName,
                                      ellDictHandle      * aSpaceDictHandle,
                                      stlBool            * aExist,
                                      ellEnv             * aEnv );

stlStatus ellGetTablespaceDictHandleByID( smlTransId           aTransID,
                                          smlScn               aViewSCN,
                                          stlInt64             aTablespaceID,
                                          ellDictHandle      * aSpaceDictHandle,
                                          stlBool            * aExist,
                                          ellEnv             * aEnv );


/*
 * 정보 획득 함수
 */

stlInt64    ellGetTablespaceID( ellDictHandle * aSpaceDictHandle );
stlChar   * ellGetTablespaceName( ellDictHandle * aSpaceDictHandle );
ellTablespaceMediaType  ellGetTablespaceMediaType( ellDictHandle * aSpaceDictHandle );
ellTablespaceUsageType  ellGetTablespaceUsageType( ellDictHandle * aSpaceDictHandle );
stlBool  ellIsBuiltInTablespace( ellDictHandle * aSpaceDictHandle );
stlInt64 ellGetTablespaceCreatorID( ellDictHandle * aSpaceDictHandle );


/** @} ellObjectTablespace */



#endif /* _ELL_DICT_TABLESPACE_H_ */
