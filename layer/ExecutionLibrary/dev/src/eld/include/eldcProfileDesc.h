/*******************************************************************************
 * eldcProfileDesc.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldcProfileDesc.h 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ELDC_PROFILE_DESC_H_
#define _ELDC_PROFILE_DESC_H_ 1

/**
 * @file eldcProfileDesc.h
 * @brief Cache for Profile descriptor
 */


/**
 * @defgroup eldcProfileDesc Cache for Profile descriptor
 * @ingroup eldcObject
 * @{
 */

/**
 * @brief Profile 의 Name Hash Data
 */
typedef struct eldcHashDataProfileName
{
    stlChar         * mKeyProfileName;     /**< Name Hash 의 Key : Profile Name */
    stlInt64          mDataProfileID;      /**< Name Hash 의 Data: Profile ID */
    eldcHashElement * mDataHashElement; /**< Name Hash 의 Data: Profile ID 의 Hash Element */
} eldcHashDataProfileName;

/**
 * @brief Profile 의 ID Hash Data
 */
typedef struct eldcHashDataProfileID
{
    stlInt64   mKeyProfileID;    /**< ID Hash 의 Key : Profile ID */
} eldcHashDataProfileID;







/*
 * Hash 함수 
 */

stlUInt32  eldcHashFuncProfileID( void * aHashKey );
stlBool    eldcCompareFuncProfileID( void            * aHashKey,
                                     eldcHashElement * aHashElement );

stlUInt32  eldcHashFuncProfileName( void * aHashKey );
stlBool    eldcCompareFuncProfileName( void            * aHashKey,
                                       eldcHashElement * aHashElement );


/*
 * Buffer Size 계산
 */

stlInt32  eldcCalSizeProfileDesc( stlInt32 aProfileNameLen );
stlChar * eldcGetProfileNamePtr( ellProfileDesc * aProfileDesc );

stlInt32  eldcCalSizeHashDataProfileID();

stlInt32  eldcCalSizeHashDataProfileName( stlInt32 aProfileNameLen );
stlChar * eldcGetHashDataProfileNamePtr( eldcHashDataProfileName * aHashDataProfileName );


/*
 * Dump Information
 */

void eldcPrintProfileDesc( void    * aProfileDesc,
                           stlChar * aStringBuffer );
void eldcPrintHashDataProfileID( void    * aHashDataProfileID,
                                 stlChar * aStringBuffer );
void eldcPrintHashDataProfileName( void    * aHashDataProfileName,
                                   stlChar * aStringBuffer );


/*
 * 초기화, 마무리 함수
 */

void eldcSetSQLProfileDumpHandle();

stlStatus eldcBuildProfileCache( smlTransId     aTransID,
                                 smlStatement * aStmt,
                                 ellEnv       * aEnv );

stlStatus eldcBuildCacheSQLProfile( smlTransId       aTransID,
                                    smlStatement   * aStmt,
                                    ellEnv         * aEnv );

/*
 * 조회 함수 
 */

stlStatus eldcGetProfileHandleByID( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aProfileID,
                                    ellDictHandle      * aProfileHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv );

stlStatus eldcGetProfileHandleByName( smlTransId      aTransID,
                                      smlScn          aViewSCN,
                                      stlChar       * aProfileName,
                                      ellDictHandle * aProfileHandle,
                                      stlBool       * aExist,
                                      ellEnv        * aEnv );

/*
 * 제어 함수
 */

stlStatus eldcInsertCacheProfile( smlTransId           aTransID,
                                  smlStatement       * aStmt,
                                  knlValueBlockList  * aValueArray,
                                  ellEnv             * aEnv );

stlStatus eldcInsertCacheProfileByProfileID( smlTransId          aTransID,
                                             smlStatement      * aStmt,
                                             stlInt64            aProfileID,
                                             ellEnv            * aEnv );

stlStatus eldcDeleteCacheProfile( smlTransId          aTransID,
                                  ellDictHandle     * aProfileHandle,
                                  ellEnv            * aEnv );


/*
 * 멤버 함수 
 */

stlStatus eldcMakeProfileDesc( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               knlValueBlockList  * aValueArray,
                               ellProfileDesc    ** aProfileDesc,
                               ellEnv             * aEnv );

stlStatus eldcMakeHashElementProfileID( eldcHashElement ** aHashElement,
                                        smlTransId         aTransID,
                                        ellProfileDesc   * aProfileDesc,
                                        ellEnv           * aEnv );


stlStatus eldcMakeHashElementProfileName( eldcHashElement   ** aHashElement,
                                          smlTransId           aTransID,
                                          ellProfileDesc     * aProfileDesc,
                                          eldcHashElement    * aHashElementID,
                                          ellEnv             * aEnv );

stlStatus eldcSetProfilePassParam( smlTransId       aTransID,
                                   smlStatement   * aStmt,
                                   ellProfileDesc * aProfileDesc,
                                   ellEnv         * aEnv );

/** @} eldcProfileDesc */


#endif /* _ELDC_PROFILE_DESC_H_ */
