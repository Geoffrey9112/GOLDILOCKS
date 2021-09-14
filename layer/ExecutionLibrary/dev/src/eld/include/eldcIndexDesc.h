/*******************************************************************************
 * eldcIndexDesc.h
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


#ifndef _ELDC_INDEX_DESC_H_
#define _ELDC_INDEX_DESC_H_ 1

/**
 * @file eldcIndexDesc.h
 * @brief Cache for Index descriptor
 */


/**
 * @defgroup eldcIndexDesc Cache for Index descriptor
 * @ingroup eldcObject
 * @{
 */

/**
 * @brief Index 의 Name Hash Data
 */
typedef struct eldcHashDataIndexName
{
    stlInt64   mKeySchemaID;            /**< Name Hash 의 Key : Schema ID */
    stlChar  * mKeyIndexName;           /**< Name Hash 의 Key : Index Name */
    stlInt64   mDataIndexID;            /**< Name Hash 의 Data: Index ID */
    eldcHashElement * mDataHashElement; /**< Name Hash 의 Data: Index ID 의 Hash Element */
} eldcHashDataIndexName;

/**
 * @brief Index 의 ID Hash Data
 */
typedef struct eldcHashDataIndexID
{
    stlInt64   mKeyIndexID;    /**< ID Hash 의 Key : Index ID */
} eldcHashDataIndexID;


/**
 * @brief Index Name 검색을 위한 Hash Key
 */
typedef struct eldcHashKeyIndexName
{
    stlInt64     mSchemaID;     /**< Schema ID */
    stlChar    * mIndexName;    /**< Index Name */
} eldcHashKeyIndexName;






/*
 * Hash 함수 
 */

stlUInt32  eldcHashFuncIndexID( void * aHashKey );
stlBool    eldcCompareFuncIndexID( void * aHashKey, eldcHashElement * aHashElement );

stlUInt32  eldcHashFuncIndexName( void * aHashKey );
stlBool    eldcCompareFuncIndexName( void * aHashKey, eldcHashElement * aHashElement );


/*
 * Buffer Size 계산
 */

stlInt32  eldcCalSizeIndexDesc( stlInt32     aIndexNameLen );
stlChar * eldcGetIndexNamePtr( ellIndexDesc * aIndexDesc );

stlInt32  eldcCalSizeHashDataIndexID();
stlInt32  eldcCalSizeHashDataIndexName( stlInt32 aIndexNameLen );
stlChar * eldcGetHashDataIndexNamePtr( eldcHashDataIndexName * aHashDataIndexName );

/*
 * Dump Information
 */

void eldcPrintIndexDesc( void * aIndexDesc, stlChar * aStringBuffer );
void eldcPrintHashDataIndexID( void * aHashDataIndexID, stlChar * aStringBuffer );
void eldcPrintHashDataIndexName( void * aHashDataIndexName, stlChar * aStringBuffer );

/*
 * 초기화, 마무리 함수
 */

void eldcSetSQLIndexDumpHandle();

stlStatus eldcBuildIndexCache( smlTransId     aTransID,
                               smlStatement * aStmt,
                               ellEnv       * aEnv );

stlStatus eldcBuildCacheSQLIndex( smlTransId       aTransID,
                                  smlStatement   * aStmt,
                                  ellEnv         * aEnv );

/*
 * 조회 함수 
 */

stlStatus eldcGetIndexHandleByID( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  stlInt64             aIndexID,
                                  ellDictHandle      * aIndexDictHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv );

stlStatus eldcGetIndexHandleByName( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aSchemaID,
                                    stlChar            * aIndexName,
                                    ellDictHandle      * aIndexDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv );


/*
 * 제어 함수
 */

stlStatus eldcInsertCacheIndex( smlTransId          aTransID,
                                smlStatement      * aStmt,
                                knlValueBlockList * aValueArray,
                                ellEnv            * aEnv );

stlStatus eldcInsertCacheIndexByIndexID( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aIndexID,
                                         ellEnv            * aEnv );

stlStatus eldcInsertCacheIndexByTableID( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aTableID,
                                         ellEnv            * aEnv );

stlStatus eldcDeleteCacheIndex( smlTransId          aTransID,
                                ellDictHandle     * aIndexHandle,
                                ellEnv            * aEnv );

stlStatus eldcAddUniqueIndexIntoTable( smlTransId     aTransID,
                                       stlInt64       aIndexID,
                                       ellEnv       * aEnv );

stlStatus eldcDelUniqueIndexFromTable( smlTransId      aTransID,
                                       ellDictHandle * aTableHandle,
                                       ellDictHandle * aIndexHandle,
                                       ellEnv        * aEnv );

stlStatus eldcAddNonUniqueIndexIntoTable( smlTransId     aTransID,
                                          stlInt64       aIndexID,
                                          ellEnv       * aEnv );

stlStatus eldcDelNonUniqueIndexFromTable( smlTransId      aTransID,
                                          ellDictHandle * aTableHandle,
                                          ellDictHandle * aIndexHandle,
                                          ellEnv        * aEnv );

stlStatus eldcRuntimeCheckSameKey4UniqueKey( smlTransId      aTransID,
                                             ellDictHandle * aTableHandle,
                                             ellDictHandle * aIndexHandle,
                                             stlBool       * aExist,
                                             ellEnv        * aEnv );

stlStatus eldcRuntimeCheckSameKey4ForeignKey( smlTransId      aTransID,
                                              ellDictHandle * aTableHandle,
                                              ellDictHandle * aIndexHandle,
                                              stlBool       * aExist,
                                              ellEnv        * aEnv );

stlStatus eldcRuntimeCheckSameKey4Index( smlTransId      aTransID,
                                         ellDictHandle * aTableHandle,
                                         ellDictHandle * aIndexHandle,
                                         stlBool       * aExist,
                                         ellEnv        * aEnv );

/*
 * 멤버 함수 
 */

stlStatus eldcMakeIndexDesc( smlTransId          aTransID,
                             smlStatement      * aStmt,
                             knlValueBlockList * aValueArray,
                             ellIndexDesc     ** aIndexDesc,
                             ellEnv            * aEnv );


stlStatus eldcMakeHashElementIndexID( eldcHashElement   ** aHashElement,
                                      smlTransId           aTransID,
                                      ellIndexDesc       * aIndexDesc,
                                      ellEnv             * aEnv );


stlStatus eldcMakeHashElementIndexName( eldcHashElement   ** aHashElement,
                                        smlTransId           aTransID,
                                        ellIndexDesc       * aIndexDesc,
                                        eldcHashElement    * aHashElementID,
                                        ellEnv             * aEnv );

stlStatus eldcSetKeyColumn( smlTransId           aTransID,
                            smlStatement       * aStmt,
                            ellIndexDesc       * aIndexDesc,
                            ellEnv             * aEnv );


/** @} eldcIndexDesc */


#endif /* _ELDC_INDEX_DESC_H_ */
