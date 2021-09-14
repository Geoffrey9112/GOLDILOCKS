/*******************************************************************************
 * eldcCatalogDesc.h
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


#ifndef _ELDC_CATALOG_DESC_H_
#define _ELDC_CATALOG_DESC_H_ 1

/**
 * @file eldcCatalogDesc.h
 * @brief Cache for Catalog descriptor
 */


/**
 * @defgroup eldcCatalogDesc Cache for Catalog descriptor
 * @remarks 하나의 Row 로 구성되어 있는 Catalog 정보는 Hash 를 필요로 하지 않는다.
 * <BR> 그러나, ALTER DATABASE 등의 Cache Version 관리를 위해 다른 객체와 동일한 방식으로 관리한다.
 * @ingroup eldcObject
 * @{
 */


/**
 * @brief Catalog 의 Name Hash Data
 */
typedef struct eldcHashDataCatalogName
{
    stlChar         * mKeyCatalogName;  /**< Name Hash 의 Key : Catalog Name */
    stlInt64          mDataCatalogID;   /**< Name Hash 의 Data: Catalog ID */
    eldcHashElement * mDataHashElement; /**< Name Hash 의 Data: Catalog 의 Hash Element */
} eldcHashDataCatalogName;


/**
 * @brief Catalog 의 ID Hash Data
 */
typedef struct eldcHashDataCatalogID
{
    stlInt64   mKeyCatalogID;    /**< ID Hash 의 Key : Catalog ID */
} eldcHashDataCatalogID;


/*
 * Hash 함수 
 */

stlUInt32  eldcHashFuncCatalogID( void * aHashKey );
stlBool    eldcCompareFuncCatalogID( void            * aHashKey,
                                     eldcHashElement * aHashElement );

stlUInt32  eldcHashFuncCatalogName( void * aHashKey );
stlBool    eldcCompareFuncCatalogName( void            * aHashKey,
                                       eldcHashElement * aHashElement );


/*
 * Buffer Size 계산
 */

stlInt32  eldcCalSizeCatalogDesc( stlInt32 aCatalogNameLen );
stlChar * eldcGetCatalogNamePtr( ellCatalogDesc * aCatalogDesc );

stlInt32  eldcCalSizeHashDataCatalogID();

stlInt32  eldcCalSizeHashDataCatalogName( stlInt32 aCatalogNameLen );
stlChar * eldcGetHashDataCatalogNamePtr( eldcHashDataCatalogName * aHashDataCatalogName );


/*
 * Dump Information
 */

void eldcPrintCatalogDesc( void * aCatalogDesc, stlChar * aStringBuffer );
void eldcPrintHashDataCatalogID( void * aHashDataCatalogID, stlChar * aStringBuffer );
void eldcPrintHashDataCatalogName( void * aHashDataCatalogName, stlChar * aStringBuffer );




/*
 * 초기화, 마무리 함수
 */

void eldcSetSQLCatalogDumpHandle();

stlStatus eldcBuildCatalogCache( smlTransId     aTransID,
                                 smlStatement * aStmt,
                                 ellEnv       * aEnv );

stlStatus eldcBuildCacheSQLCatalog( smlTransId       aTransID,
                                    smlStatement   * aStmt,
                                    ellEnv         * aEnv );


/*
 * 조회 함수 
 */

stlStatus eldcGetCatalogHandle( smlTransId           aTransID,
                                smlScn               aViewSCN,
                                ellDictHandle      * sCatalogDictHandle,
                                stlBool            * aExist,
                                ellEnv             * aEnv );

/*
 * 제어 함수
 */

stlStatus eldcInsertCacheCatalog( smlTransId          aTransID,
                                  smlStatement      * aStmt,
                                  knlValueBlockList * aValueArray,
                                  ellEnv            * aEnv );


/*
 * 멤버 함수
 */

stlStatus eldcMakeCatalogDesc( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               knlValueBlockList  * aValueArray,
                               ellCatalogDesc    ** aCatalogDesc,
                               ellEnv             * aEnv );


stlStatus eldcMakeHashElementCatalogID( eldcHashElement   ** aHashElement,
                                        smlTransId           aTransID,
                                        ellCatalogDesc      * aCatalogDesc,
                                        ellEnv             * aEnv );


stlStatus eldcMakeHashElementCatalogName( eldcHashElement   ** aHashElement,
                                          smlTransId           aTransID,
                                          ellCatalogDesc      * aCatalogDesc,
                                          eldcHashElement    * aHashElementID,
                                          ellEnv             * aEnv );

/** @} eldcCatalogDesc */


#endif /* _ELDC_CATALOG_DESC_H_ */
