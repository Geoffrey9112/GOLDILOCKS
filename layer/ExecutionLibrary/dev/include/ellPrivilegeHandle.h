/*******************************************************************************
 * ellPrivilegeHandle.h
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


#ifndef _ELL_PRIVILEGE_HANDLE_H_
#define _ELL_PRIVILEGE_HANDLE_H_ 1

/**
 * @file ellPrivilegeHandle.h
 * @brief Execution Layer 의 Privilege Dictionary Handle 관리 공통 모듈 
 */

/**
 * @defgroup ellPrivHandle Privilege Dictionary Handle
 * @ingroup ellSqlObject
 * @{
 */


/**
 * @brief Privilege Handle List
 */
typedef struct ellPrivList
{
    stlInt32    mCount;      /**< Privilege Count */
    stlBool     mForRevoke;  /**< for GRANT or for REVOKE */
    stlRingHead mHeadList;   /**<  List */
} ellPrivList;



/**
 * @brief Privilege Object Item
 * @remarks
 */
typedef struct ellPrivItem
{
    ellPrivDesc   mPrivDesc;    /**< Privilege Descriptor */
    ellDictHandle mPrivHandle;  /**< Privilege Dictionary Handle */
    stlRingEntry  mLink;        /**< Link */
} ellPrivItem;



/*
 * Privilege Dictionary Handle
 */

ellPrivDesc * ellGetPrivDesc( ellDictHandle * aDictHandle );

void ellSetPrivHandle( smlTransId       aTransID,
                       smlScn           aViewSCN,
                       ellDictHandle  * aDictHandle,
                       ellPrivObject    aPrivObject,
                       void *           aPrivHashElement,
                       ellEnv         * aEnv );

stlStatus ellIsRecentPrivHandle( smlTransId      aTransID,
                                 ellDictHandle * aPrivHandle,
                                 stlBool       * aIsValid,
                                 ellEnv        * aEnv );

stlStatus ellGetGrantedPrivHandle( smlTransId           aTransID,
                                   smlScn               aViewSCN,
                                   ellPrivObject        aPrivObject,
                                   stlInt64             aObjectID,
                                   stlInt32             aPrivAction,
                                   stlBool              aWithGrant,
                                   ellDictHandle      * aPrivHandle,
                                   stlBool            * aExist,
                                   ellEnv             * aEnv );

stlStatus ellGetPrivDescHandle( smlTransId           aTransID,
                                smlScn               aViewSCN,
                                ellPrivDesc        * aPrivDesc,
                                ellDictHandle      * aPrivHandle,
                                stlBool            * aExist,
                                ellEnv             * aEnv );

/*
 * Privilege Handle List
 */

stlStatus ellInitPrivList( ellPrivList   ** aPrivList,
                           stlBool          aForGrant,
                           knlRegionMem   * aRegionMem,
                           ellEnv         * aEnv );

stlStatus ellAddNewPrivItem( ellPrivList   * aPrivList,
                             ellPrivObject   aPrivObject,
                             ellDictHandle * aPrivHandle,
                             ellPrivDesc   * aPrivDesc,
                             stlBool       * aDuplicate,
                             knlRegionMem  * aRegionMem,
                             ellEnv        * aEnv );

stlStatus ellMakePrivList2Array( ellPrivList   * aPrivList,
                                 stlInt32      * aPrivCnt,
                                 ellPrivItem  ** aPrivArray,
                                 knlRegionMem  * aRegionMem,
                                 ellEnv        * aEnv );

stlStatus ellIsRecentPrivArray( smlTransId     aTransID,
                                stlInt32       aPrivCnt,
                                ellPrivItem  * aPrivArray,
                                stlBool      * aIsValid,
                                ellEnv       * aEnv );


stlBool ellHasPrivItem( ellPrivList   * aPrivList );

stlStatus ellGetRevokablePrivByAccessControl( smlTransId           aTransID,
                                              smlStatement       * aStmt,
                                              ellPrivObject        aPrivObject,
                                              ellDictHandle      * aObjectHandle,
                                              stlInt64             aRevokeeID,
                                              stlInt32             aPrivAction,
                                              stlBool            * aPrivExist,
                                              stlBool            * aDuplicate,
                                              ellPrivList        * aPrivList,
                                              knlRegionMem       * aRegionMem,
                                              ellEnv             * aEnv );

stlStatus ellGetRevokeDependPrivList( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      stlBool              aGrantOption,
                                      ellPrivDesc        * aRevokePriv,
                                      ellPrivList       ** aDependList,
                                      knlRegionMem       * aRegionMem,
                                      ellEnv             * aEnv );

stlStatus ellGetDescendentPrivList( smlTransId           aTransID,
                                    smlStatement       * aStmt,
                                    stlBool              aGrantOption,
                                    ellPrivDesc        * aParentPriv,
                                    ellPrivList        * aCycleDetectList,
                                    ellPrivList       ** aDecendentList,
                                    knlRegionMem       * aRegionMem,
                                    ellEnv             * aEnv );


stlStatus ellGetChildPrivList( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               stlBool              aGrantOption,
                               ellPrivDesc        * aParentPriv,
                               ellPrivList       ** aChildList,
                               knlRegionMem       * aRegionMem,
                               ellEnv             * aEnv );

stlStatus ellGetRivalPrivList( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               stlBool              aGrantOption,
                               ellPrivDesc        * aRevokePriv,
                               ellPrivList       ** aSiblingList,
                               knlRegionMem       * aRegionMem,
                               ellEnv             * aEnv );

stlStatus ellRevokeAuthGranteeAllPrivilege( smlTransId     aTransID,
                                            smlStatement * aStmt,
                                            stlInt64       aAuthID,
                                            ellEnv       * aEnv );

stlStatus ellGetAuthGrantorPrivList( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     stlInt64        aAuthID,
                                     ellPrivList  ** aUserPrivList,
                                     knlRegionMem  * aRegionMem,
                                     ellEnv        * aEnv );

/*
 * Privilege Dictionary Descriptor 조작 
 */

stlStatus ellGrantPrivDescAndCache( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    ellPrivDesc  * aPrivDesc,
                                    ellEnv       * aEnv );

stlStatus ellRevokePrivDescAndCache( smlTransId     aTransID,
                                     smlStatement * aStmt,
                                     stlBool        aGrantOption,
                                     ellPrivDesc  * aPrivDesc,
                                     ellEnv       * aEnv );

/** @} ellPrivHandle */



#endif /* _ELL_PRIVILEGE_HANDLE_H_ */
