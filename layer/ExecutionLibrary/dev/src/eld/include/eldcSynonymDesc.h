/*******************************************************************************
 * eldcSynonymDesc.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ELDC_SYNONYM_DESC_H_
#define _ELDC_SYNONYM_DESC_H_ 1

/**
 * @file eldcSynonymDesc.h
 * @brief Cache for Synonym descriptor
 */


/**
 * @defgroup eldcSynonymDesc Cache for Synonym descriptor
 * @ingroup eldcObject
 * @{
 */

/**
 * @brief Synonym 의 ID Hash Data
 */
typedef struct eldcHashDataSynonymID
{
    stlInt64                 mKeySynonymID;    /**< ID Hash 의 Key : Synonym ID */
} eldcHashDataSynonymID;

/**
 * @brief Synonym 의 Name Hash Data
 */
typedef struct eldcHashDataSynonymName
{
    stlInt64          mKeySchemaID;      /**< Name Hash 의 Key : Schema ID */
    stlChar         * mKeySynonymName;   /**< Name Hash 의 Key : Synonym Name */
    stlInt64          mDataSynonymID;    /**< Name Hash 의 Data: Synonym ID */
    eldcHashElement * mDataHashElement;  /**< Name Hash 의 Data: Synonym ID 의 Hash Element */
} eldcHashDataSynonymName;

/**
 * @brief Synonym Name 검색을 위한 Hash Key
 */
typedef struct eldcHashKeySynonymName
{
    stlInt64     mSchemaID;      /**< Schema ID */
    stlChar    * mSynonymName;   /**< Synonym Name */
} eldcHashKeySynonymName;

/*
 * Hash 함수 
 */

stlUInt32  eldcHashFuncSynonymID( void * aHashKey );
stlBool eldcCompareFuncSynonymID( void * aHashKey, eldcHashElement * aHashElement );

stlUInt32  eldcHashFuncSynonymName( void * aHashKey );
stlBool eldcCompareFuncSynonymName( void * aHashKey, eldcHashElement * aHashElement );

/*
 * Buffer Size 계산
 */

stlInt32  eldcCalSizeSynonymDesc( stlInt32 aSynonymNameLen,
                                  stlInt32 aObjectSchemaNameLen,
                                  stlInt32 aObjectNameLen );

stlChar * eldcGetSynonymNamePtr( ellSynonymDesc * aSynonymDesc );
stlChar * eldcGetObjectSchemaNamePtr( ellSynonymDesc * aSynonymDesc,
                                      stlInt32         aSynonymNameLen );
stlChar * eldcGetObjectNamePtr( ellSynonymDesc * aSynonymDesc,
                                stlInt32         aSynonymNameLen,
                                stlInt32         aObjectSchemaNameLen );

stlInt32  eldcCalSizeHashDataSynonymID();
stlInt32  eldcCalSizeHashDataSynonymName( stlInt32 aSynonymNameLen );
stlChar * eldcGetHashDataSynonymNamePtr( eldcHashDataSynonymName * aHashDataSynonymName );

/*
 * Dump Information
 */

void eldcPrintSynonymDesc( void * aSynonymDesc, stlChar * aStringBuffer );
void eldcPrintHashDataSynonymID( void * aHashDataSynonymID, stlChar * aStringBuffer );
void eldcPrintHashDataSynonymName( void * aHashDataSynonymName, stlChar * aStringBuffer );

/*
 * 초기화, 마무리 함수
 */

void eldcSetSQLSynonymDumpHandle();

stlStatus eldcBuildSynonymCache( smlTransId     aTransID,
                                 smlStatement * aStmt,
                                 ellEnv       * aEnv );

stlStatus eldcBuildCacheSQLSynonym( smlTransId       aTransID,
                                    smlStatement   * aStmt,
                                    ellEnv         * aEnv );

/*
 * 조회 함수 
 */

stlStatus eldcGetSynonymHandleByName( smlTransId           aTransID,
                                      smlScn               aViewSCN,
                                      stlInt64             aSchemaID,
                                      stlChar            * aSynonymName,
                                      ellDictHandle      * aSynonymDictHandle,
                                      stlBool            * aExist,
                                      ellEnv             * aEnv );

stlStatus eldcGetSynonymHandleByID( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aSynonymID,
                                    ellDictHandle      * aSynonymDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv );


/*
 * 제어 함수
 */

stlStatus eldcInsertCacheSynonym( smlTransId          aTransID,
                                  smlStatement      * aStmt,
                                  knlValueBlockList * aValueList,
                                  ellEnv            * aEnv );

stlStatus eldcInsertCacheSynonymBySynonymID( smlTransId          aTransID,
                                             smlStatement      * aStmt,
                                             stlInt64            aSynonymID,
                                             ellEnv            * aEnv );

stlStatus eldcDeleteCacheSynonym( smlTransId          aTransID,
                                  smlStatement      * aStmt,
                                  ellDictHandle     * aSynonymHandle,
                                  ellEnv            * aEnv );

/*
 * 멤버 함수 
 */

stlStatus eldcMakeSynonymDesc( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               knlValueBlockList  * aValueArray,
                               ellSynonymDesc    ** aSynonymDesc,
                               ellEnv             * aEnv );

stlStatus eldcMakeHashElementSynonymID( eldcHashElement  ** aHashElement,
                                        smlTransId          aTransID,
                                        ellSynonymDesc    * aSynonymDesc,
                                        ellEnv            * aEnv );

stlStatus eldcMakeHashElementSynonymName( eldcHashElement  ** aHashElement,
                                          smlTransId          aTransID,
                                          ellSynonymDesc    * aSynonymDesc,
                                          eldcHashElement   * aHashElementID,
                                          ellEnv            * aEnv );

/** @} eldcSynonymDesc */


#endif /* _ELDC_SYNONYM_DESC_H_ */
