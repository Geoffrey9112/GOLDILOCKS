/*******************************************************************************
 * eldcPrivDesc.h
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


#ifndef _ELDC_PRIV_DESC_H_
#define _ELDC_PRIV_DESC_H_ 1

/**
 * @file eldcPrivDesc.h
 * @brief Cache for Privilege descriptor
 */


/**
 * @defgroup eldcPrivDesc Cache for Privilege descriptor
 * @ingroup eldcPrivilege
 * @{
 */




/*
 * Privilege 공통 Hash 함수 
 */

stlUInt32  ellGetHashValuePrivKey( void * aHashKey );

stlBool    eldcCompareFuncPrivWithoutGrant( void * aHashKey, eldcHashElement * aHashElement );
stlBool    eldcCompareFuncPrivWithGrant( void * aHashKey, eldcHashElement * aHashElement );
stlBool    eldcCompareFuncPrivRevoke( void * aHashKey, eldcHashElement * aHashElement );

/*
 * Privilege 공통 Dump Information
 */

void eldcPrintHashDataPrivDesc( void * aPrivDesc, stlChar * aStringBuffer );

/*
 * Privilege 공통 초기화, 마무리 함수
 */

stlStatus eldcBuildCachePriv( smlTransId        aTransID,
                              smlStatement    * aStmt,
                              eldcPrivCacheNO   aPrivCacheNO,
                              ellEnv          * aEnv );

/*
 * Privilege 공통 제어 함수
 */

void eldcSetPrivDumpHandle( eldcPrivCacheNO aPrivCacheNO );

stlStatus eldcInsertCachePriv( smlTransId          aTransID,
                               smlStatement      * aStmt,
                               eldcPrivCacheNO     aPrivCacheNO,
                               knlValueBlockList * aValueList,
                               ellEnv            * aEnv );

stlStatus eldcDeletePrivCache( smlTransId           aTransID,
                               ellPrivObject        aPrivObject,
                               stlInt64             aGrantorID,
                               stlInt64             aGranteeID,
                               stlInt64             aObjectID,
                               stlInt32             aPrivAction,
                               ellEnv             * aEnv );

/*
 * Privilege 공통 멤버 함수 
 */

stlStatus eldcMakePrivDesc( smlTransId           aTransID,
                            smlStatement       * aStmt,
                            eldcPrivCacheNO      aPrivCacheNO,
                            knlValueBlockList  * aValueList,
                            ellPrivDesc       ** aPrivDesc,
                            ellEnv             * aEnv );

stlStatus eldcMakeHashElementPriv( eldcHashElement ** aHashElement,
                                   smlTransId         aTransID,
                                   ellPrivDesc      * aPrivDesc,
                                   ellEnv           * aEnv );

/*
 * Privilege 공통 조회 함수 
 */

stlStatus eldcGetGrantPrivHandle( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  ellPrivObject        aPrivObject,
                                  stlInt64             aGranteeID,
                                  stlInt64             aObjectID,
                                  stlInt32             aPrivAction,
                                  stlBool              aWithGrant,
                                  ellDictHandle      * aPrivHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv );

stlStatus eldcGetRevokePrivHandle( smlTransId           aTransID,
                                   smlScn               aViewSCN,
                                   ellPrivObject        aPrivObject,
                                   stlInt64             aGrantorID,
                                   stlInt64             aGranteeID,
                                   stlInt64             aObjectID,
                                   stlInt32             aPrivAction,
                                   ellDictHandle      * aPrivHandle,
                                   stlBool            * aExist,
                                   ellEnv             * aEnv );
/** @} eldcPrivDesc */


#endif /* _ELDC_PRIV_DESC_H_ */
