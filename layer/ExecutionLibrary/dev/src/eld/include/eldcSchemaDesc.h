/*******************************************************************************
 * eldcSchemaDesc.h
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


#ifndef _ELDC_SCHEMA_DESC_H_
#define _ELDC_SCHEMA_DESC_H_ 1

/**
 * @file eldcSchemaDesc.h
 * @brief Cache for Schema descriptor
 */


/**
 * @defgroup eldcSchemaDesc Cache for Schema descriptor
 * @ingroup eldcObject
 * @{
 */

/**
 * @brief Schema 의 Name Hash Data
 */
typedef struct eldcHashDataSchemaName
{
    stlChar         * mKeySchemaName;   /**< Name Hash 의 Key : Schema Name */
    stlInt64          mDataSchemaID;    /**< Name Hash 의 Data: Schema ID */
    eldcHashElement * mDataHashElement; /**< Name Hash 의 Data: Const ID 의 Hash Element */
} eldcHashDataSchemaName;

/**
 * @brief Schema 의 ID Hash Data
 */
typedef struct eldcHashDataSchemaID
{
    stlInt64   mKeySchemaID;    /**< ID Hash 의 Key : Schema ID */
} eldcHashDataSchemaID;









/*
 * Hash 함수 
 */

stlUInt32  eldcHashFuncSchemaID( void * aHashKey );
stlBool    eldcCompareFuncSchemaID( void * aHashKey, eldcHashElement * aHashElement );

stlUInt32  eldcHashFuncSchemaName( void * aHashKey );
stlBool    eldcCompareFuncSchemaName( void * aHashKey, eldcHashElement * aHashElement );


/*
 * Buffer Size 계산
 */

stlInt32  eldcCalSizeSchemaDesc( stlInt32 aSchemaNameLen );
stlChar * eldcGetSchemaNamePtr( ellSchemaDesc * aSchemaDesc );

stlInt32  eldcCalSizeHashDataSchemaID();

stlInt32  eldcCalSizeHashDataSchemaName( stlInt32 aSchemaNameLen );
stlChar * eldcGetHashDataSchemaNamePtr( eldcHashDataSchemaName * aHashDataSchemaName );


/*
 * Dump Information
 */

void eldcPrintSchemaDesc( void * aSchemaDesc, stlChar * aStringBuffer );
void eldcPrintHashDataSchemaID( void * aHashDataSchemaID, stlChar * aStringBuffer );
void eldcPrintHashDataSchemaName( void * aHashDataSchemaName, stlChar * aStringBuffer );


/*
 * 초기화, 마무리 함수
 */

void eldcSetSQLSchemaDumpHandle();

stlStatus eldcBuildSchemaCache( smlTransId     aTransID,
                                smlStatement * aStmt,
                                ellEnv       * aEnv );

stlStatus eldcBuildCacheSQLSchema( smlTransId       aTransID,
                                   smlStatement   * aStmt,
                                   ellEnv         * aEnv );

/*
 * 조회 함수 
 */

stlStatus eldcGetSchemaHandleByID( smlTransId           aTransID,
                                   smlScn               aViewSCN,
                                   stlInt64             aSchemaID,
                                   ellDictHandle      * aSchemaDictHandle,
                                   stlBool            * aExist,
                                   ellEnv             * aEnv );

stlStatus eldcGetSchemaHandleByName( smlTransId           aTransID,
                                     smlScn               aViewSCN,
                                     stlChar            * aSchemaName,
                                     ellDictHandle      * aSchemaDictHandle,
                                     stlBool            * aExist,
                                     ellEnv             * aEnv );

/*
 * 제어 함수
 */

stlStatus eldcInsertCacheSchema( smlTransId          aTransID,
                                 smlStatement      * aStmt,
                                 knlValueBlockList * aValueArray,
                                 ellEnv            * aEnv );

stlStatus eldcInsertCacheSchemaBySchemaID( smlTransId          aTransID,
                                           smlStatement      * aStmt,
                                           stlInt64            aSchemaID,
                                           ellEnv            * aEnv );

stlStatus eldcDeleteCacheSchema( smlTransId          aTransID,
                                 ellDictHandle     * aSchemaHandle,
                                 ellEnv            * aEnv );

/*
 * 멤버 함수 
 */

stlStatus eldcMakeSchemaDesc( smlTransId          aTransID,
                              smlStatement      * aStmt,
                              knlValueBlockList * aValueArray,
                              ellSchemaDesc    ** aSchemaDesc,
                              ellEnv            * aEnv );

stlStatus eldcMakeHashElementSchemaID( eldcHashElement   ** aHashElement,
                                       smlTransId           aTransID,
                                       ellSchemaDesc      * aSchemaDesc,
                                       ellEnv             * aEnv );


stlStatus eldcMakeHashElementSchemaName( eldcHashElement   ** aHashElement,
                                         smlTransId           aTransID,
                                         ellSchemaDesc      * aSchemaDesc,
                                         eldcHashElement    * aHashElementID,
                                         ellEnv             * aEnv );


/** @} eldcSchemaDesc */


#endif /* _ELDC_SCHEMA_DESC_H_ */
