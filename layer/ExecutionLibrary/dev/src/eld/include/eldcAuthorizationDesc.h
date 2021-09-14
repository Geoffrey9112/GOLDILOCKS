/*******************************************************************************
 * eldcAuthorizationDesc.h
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


#ifndef _ELDC_AUTHORIZATION_DESC_H_
#define _ELDC_AUTHORIZATION_DESC_H_ 1

/**
 * @file eldcAuthorizationDesc.h
 * @brief Cache for Authorization descriptor
 */


/**
 * @defgroup eldcAuthorizationDesc Cache for Authorization descriptor
 * @ingroup eldcObject
 * @{
 */

/**
 * @brief Authorization 의 Name Hash Data
 */
typedef struct eldcHashDataAuthName
{
    stlChar         * mKeyAuthName;     /**< Name Hash 의 Key : Authorization Name */
    stlInt64          mDataAuthID;      /**< Name Hash 의 Data: Authorization ID */
    eldcHashElement * mDataHashElement; /**< Name Hash 의 Data: Auth ID 의 Hash Element */
} eldcHashDataAuthName;

/**
 * @brief Authorization 의 ID Hash Data
 */
typedef struct eldcHashDataAuthID
{
    stlInt64   mKeyAuthID;    /**< ID Hash 의 Key : Authorization ID */
} eldcHashDataAuthID;







/*
 * Hash 함수 
 */

stlUInt32  eldcHashFuncAuthID( void * aHashKey );
stlBool    eldcCompareFuncAuthID( void            * aHashKey,
                                  eldcHashElement * aHashElement );

stlUInt32  eldcHashFuncAuthName( void * aHashKey );
stlBool    eldcCompareFuncAuthName( void            * aHashKey,
                                    eldcHashElement * aHashElement );


/*
 * Buffer Size 계산
 */

stlInt32  eldcCalSizeAuthDesc( stlInt32 aAuthNameLen,
                               stlInt32 aSchemaCnt,
                               stlInt32 aGrantedRoleCnt );
stlChar  * eldcGetAuthNamePtr( ellAuthDesc * aAuthDesc );
stlInt64 * eldcGetSchemaPathPtr( ellAuthDesc * aAuthDesc,
                                 stlInt32      aAuthNameLen );
stlInt64 * eldcGetGrantedRolePtr( ellAuthDesc * aAuthDesc,
                                  stlInt32      aAuthNameLen,
                                  stlInt32      aSchemaCnt );
stlBool * eldcGetIsGrantablePtr( ellAuthDesc * aAuthDesc,
                                 stlInt32      aAuthNameLen,
                                 stlInt32      aSchemaCnt,
                                 stlInt32      aGrantedRoleCnt );

stlInt32  eldcCalSizeHashDataAuthID();

stlInt32  eldcCalSizeHashDataAuthName( stlInt32 aAuthNameLen );
stlChar * eldcGetHashDataAuthNamePtr( eldcHashDataAuthName * aHashDataAuthName );


/*
 * Dump Information
 */

void eldcPrintAuthDesc( void    * aAuthDesc,
                        stlChar * aStringBuffer );
void eldcPrintHashDataAuthID( void    * aHashDataAuthID,
                              stlChar * aStringBuffer );
void eldcPrintHashDataAuthName( void    * aHashDataAuthName,
                                stlChar * aStringBuffer );


/*
 * 초기화, 마무리 함수
 */

void eldcSetSQLAuthDumpHandle();

stlStatus eldcBuildAuthCache( smlTransId     aTransID,
                              smlStatement * aStmt,
                              ellEnv       * aEnv );

stlStatus eldcBuildCacheSQLAuth( smlTransId       aTransID,
                                 smlStatement   * aStmt,
                                 ellEnv         * aEnv );

/*
 * 조회 함수 
 */

stlStatus eldcGetAuthHandleByID( smlTransId           aTransID,
                                 smlScn               aViewSCN,
                                 stlInt64             aAuthID,
                                 ellDictHandle      * aAuthHandle,
                                 stlBool            * aExist,
                                 ellEnv             * aEnv );

stlStatus eldcGetAuthHandleByName( smlTransId      aTransID,
                                   smlScn          aViewSCN,
                                   stlChar       * aAuthName,
                                   ellDictHandle * aAuthHandle,
                                   stlBool       * aExist,
                                   ellEnv        * aEnv );

/*
 * 제어 함수
 */

stlStatus eldcInsertCacheAuth( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               knlValueBlockList  * aValueArray,
                               ellEnv             * aEnv );

stlStatus eldcInsertCacheAuthByAuthID( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aAuthID,
                                       ellEnv            * aEnv );

stlStatus eldcDeleteCacheAuth( smlTransId          aTransID,
                               ellDictHandle     * aAuthHandle,
                               ellEnv            * aEnv );

/*
 * 멤버 함수 
 */

stlStatus eldcMakeAuthDesc( smlTransId           aTransID,
                            smlStatement       * aStmt,
                            knlValueBlockList  * aValueArray,
                            ellAuthDesc       ** aAuthDesc,
                            ellEnv             * aEnv );

stlStatus eldcMakeHashElementAuthID( eldcHashElement ** aHashElement,
                                     smlTransId         aTransID,
                                     ellAuthDesc      * aAuthDesc,
                                     ellEnv           * aEnv );


stlStatus eldcMakeHashElementAuthName( eldcHashElement   ** aHashElement,
                                       smlTransId           aTransID,
                                       ellAuthDesc        * aAuthDesc,
                                       eldcHashElement    * aHashElementID,
                                       ellEnv             * aEnv );

stlStatus eldcGetSchemaCnt( smlTransId     aTransID,
                            smlStatement * aStmt,
                            stlInt64       aAuthID,
                            stlInt32     * aSchemaCnt,
                            ellEnv       * aEnv );

stlStatus eldcSetSchemaPath( smlTransId     aTransID,
                             smlStatement * aStmt,
                             ellAuthDesc  * aAuthDesc,
                             ellEnv       * aEnv );

stlStatus eldcGetGrantedRoleCnt( smlTransId     aTransID,
                                 smlStatement * aStmt,
                                 stlInt64       aAuthID,
                                 stlInt32     * aGrantedRoleCnt,
                                 ellEnv       * aEnv );

stlStatus eldcSetGrantedRole( smlTransId     aTransID,
                              smlStatement * aStmt,
                              ellAuthDesc  * aAuthDesc,
                              ellEnv       * aEnv );

stlStatus eldcSetUserSpaceProfileInfo( smlTransId     aTransID,
                                       smlStatement * aStmt,
                                       ellAuthDesc  * aAuthDesc,
                                       ellEnv       * aEnv );

/** @} eldcAuthorizationDesc */


#endif /* _ELDC_AUTHORIZATION_DESC_H_ */
