/*******************************************************************************
 * ellDictAuthorization.h
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


#ifndef _ELL_DICT_AUTHORIZATION_H_
#define _ELL_DICT_AUTHORIZATION_H_ 1

/**
 * @file ellDictAuthorization.h
 * @brief Authorization Dictionary Management
 */

/**
 * @addtogroup ellObjectAuth
 * @{
 */


/*
 * DDL 관련 Object 획득 함수
 */

stlStatus ellGetUserOwnedSchemaList( smlTransId       aTransID,
                                     smlStatement   * aStmt,
                                     ellDictHandle  * aUserHandle,
                                     knlRegionMem   * aRegionMem,
                                     ellObjectList ** aSchemaList,
                                     ellEnv         * aEnv );

stlStatus ellGetObjectList4DropUser( smlTransId       aTransID,
                                     smlStatement   * aStmt,
                                     ellDictHandle  * aUserHandle,
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
                                     ellObjectList ** aPrivObjectList,
                                     ellEnv         * aEnv );


/*
 * DDL Lock 함수 
 */

stlStatus ellLock4CreateUser( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              ellDictHandle * aCreatorHandle,
                              ellDictHandle * aDataSpaceHandle,
                              ellDictHandle * aTempSpaceHandle,
                              ellDictHandle * aProfileHandle,
                              stlBool       * aLockSuccess,
                              ellEnv        * aEnv );

stlStatus ellLock4CreateRole();


stlStatus ellLock4DropUser( smlTransId      aTransID,
                            smlStatement  * aStmt,
                            ellDictHandle * aUserHandle,
                            stlBool       * aLockSuccess,
                            ellEnv        * aEnv );

stlStatus ellLock4AlterUser( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellDictHandle * aUserHandle,
                             stlBool       * aLockSuccess,
                             ellEnv        * aEnv );


stlStatus ellLock4GrantPriv( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellDictHandle * aGrantorHandle,
                             ellObjectList * aGranteeList,
                             ellPrivObject   aPrivObject,
                             ellDictHandle * aObjectHandle,
                             stlBool       * aLockSuccess,
                             ellEnv        * aEnv );

stlStatus ellLock4RevokePriv( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              ellDictHandle * aRevokerHandle,
                              ellObjectList * aRevokeeList,
                              ellPrivObject   aPrivObject,
                              ellDictHandle * aObjectHandle,
                              stlBool       * aLockSuccess,
                              ellEnv        * aEnv );

stlStatus ellLock4PrivObjectList( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  ellObjectList * aPrivObjectList,
                                  stlBool       * aLockSuccess,
                                  ellEnv        * aEnv );

/*
 * Dictionary 변경 함수 
 */

stlStatus ellInsertAuthDesc( smlTransId             aTransID,
                             smlStatement         * aStmt,
                             stlInt64             * aAuthID,
                             stlChar              * aAuthName,
                             ellAuthorizationType   aAuthType,
                             stlChar              * aAuthComment,
                             ellEnv               * aEnv );

stlStatus ellRemoveDict4DropUser( smlTransId        aTransID,
                                  smlStatement    * aStmt,
                                  ellDictHandle   * aUserHandle,
                                  ellEnv          * aEnv );

stlStatus ellSetTimeAlterAuth( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               stlInt64        aAuthID,
                               ellEnv        * aEnv );

stlStatus ellModifyUserDataSpace( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  ellDictHandle * aUserHandle,
                                  stlInt64        aSpaceID,
                                  ellEnv        * aEnv );

stlStatus ellModifyUserTempSpace( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  ellDictHandle * aUserHandle,
                                  stlInt64        aSpaceID,
                                  ellEnv        * aEnv );

stlStatus ellModifyAuthSchemaPath( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   ellDictHandle * aAuthHandle,
                                   stlInt32        aSchemaCnt,
                                   ellDictHandle * aSchemaHandleArray,
                                   ellEnv        * aEnv );

/*
 * Cache 재구축 함수
 */

stlStatus ellRefineCache4AddUser( smlTransId           aTransID,
                                  smlStatement       * aStmt,
                                  stlInt64             aUserID,
                                  ellEnv             * aEnv );

stlStatus ellRefineCache4AddRole( smlTransId           aTransID,
                                  smlStatement       * aStmt,
                                  stlInt64             aRoleID,
                                  ellEnv             * aEnv );

stlStatus ellRefineCache4DropUser( smlTransId        aTransID,
                                   ellDictHandle   * aUserHandle,
                                   ellEnv          * aEnv );

stlStatus ellRefineCache4AlterUser( smlTransId        aTransID,
                                    smlStatement    * aStmt,
                                    ellDictHandle   * aUserHandle,
                                    ellEnv          * aEnv );

stlStatus ellRebuildAuthCache( smlTransId        aTransID,
                               smlStatement    * aStmt,
                               ellDictHandle   * aAuthHandle,
                               ellEnv          * aEnv );

/*
 * Handle 획득 함수 
 */

stlStatus ellGetAuthDictHandle( smlTransId           aTransID,
                                smlScn               aViewSCN,
                                stlChar            * aAuthName,
                                ellDictHandle      * aAuthDictHandle,
                                stlBool            * aExist,
                                ellEnv             * aEnv );

stlStatus ellGetAuthDictHandleByID( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aAuthID,
                                    ellDictHandle      * aAuthDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv );


stlStatus ellGetAuthFirstSchemaDictHandle( smlTransId           aTransID,
                                           smlScn               aViewSCN,
                                           ellDictHandle      * aAuthDictHandle,
                                           ellDictHandle      * aSchemaDictHandle,
                                           ellEnv             * aEnv );

stlStatus ellGetAuthDataSpaceHandle( smlTransId       aTransID,
                                     smlScn           aViewSCN,
                                     ellDictHandle  * aAuthHandle,
                                     ellDictHandle  * aSpaceHandle,
                                     ellEnv         * aEnv );
                               
stlStatus ellGetAuthTempSpaceHandle( smlTransId       aTransID,
                                     smlScn           aViewSCN,
                                     ellDictHandle  * aAuthHandle,
                                     ellDictHandle  * aSpaceHandle,
                                     ellEnv         * aEnv );

/*
 * 정보 획득 함수
 */


stlInt64    ellGetAuthID( ellDictHandle * aAuthDictHandle );
stlChar   * ellGetAuthName( ellDictHandle * aAuthDictHandle );
ellAuthorizationType ellGetAuthType( ellDictHandle * aAuthDictHandle );
stlBool     ellGetAuthAbleLogin( ellDictHandle * aAuthDictHandle );
stlInt32    ellGetAuthSchemaPathCnt( ellDictHandle * aAuthDictHandle );
stlInt64  * ellGetAuthSchemaPathArray( ellDictHandle * aAuthDictHandle );

stlInt64 ellGetAuthIdPUBLIC();
stlInt64 ellGetAuthIdSYSTEM();
stlInt64 ellGetAuthIdSYS();
stlInt64 ellGetAuthIdSYSDBA();
stlBool  ellIsBuiltInAuth( ellDictHandle * aAuthDictHandle );
stlBool  ellIsUpdatablePrivAuth( ellDictHandle * aAuthDictHandle );

/** @} ellObjectAuth */



#endif /* _ELL_DICT_AUTHORIZATION_H_ */
