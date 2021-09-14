/*******************************************************************************
 * eldcPublicSynonymDesc.h
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


#ifndef _ELDC_PUBLIC_SYNONYM_DESC_H_
#define _ELDC_PUBLIC_SYNONYM_DESC_H_ 1

/**
 * @file eldcPublicSynonymDesc.h
 * @brief Cache for Public Synonym descriptor
 */


/**
 * @defgroup eldcPublicSynonymDesc Cache for Public Synonym descriptor
 * @ingroup eldcObject
 * @{
 */

/**
 * @brief Public Synonym 의 ID Hash Data
 */
typedef struct eldcHashDataPublicSynonymID
{
    stlInt64                 mKeySynonymID;    /**< ID Hash 의 Key : Synonym ID */
} eldcHashDataPublicSynonymID;

/**
 * @brief Synonym 의 Name Hash Data
 */
typedef struct eldcHashDataPublicSynonymName
{
    stlChar         * mKeySynonymName;   /**< Name Hash 의 Key : Synonym Name */
    stlInt64          mDataSynonymID;    /**< Name Hash 의 Data: Synonym ID */
    eldcHashElement * mDataHashElement;  /**< Name Hash 의 Data: Synonym ID 의 Hash Element */
} eldcHashDataPublicSynonymName;

/*
 * Hash 함수 
 */

stlUInt32  eldcHashFuncPublicSynonymID( void * aHashKey );
stlBool eldcCompareFuncPublicSynonymID( void * aHashKey, eldcHashElement * aHashElement );

stlUInt32  eldcHashFuncPublicSynonymName( void * aHashKey );
stlBool eldcCompareFuncPublicSynonymName( void * aHashKey, eldcHashElement * aHashElement );

/*
 * Buffer Size 계산
 */

stlInt32  eldcCalSizePublicSynonymDesc( stlInt32 aSynonymNameLen,
                                        stlInt32 aObjectSchemaNameLen,
                                        stlInt32 aObjectNameLen );

stlChar * eldcGetSynonymNamePtrOfPublic( ellPublicSynonymDesc * aPublicSynonymDesc );
stlChar * eldcGetObjectSchemaNamePtrOfPublic( ellPublicSynonymDesc * aPublicSynonymDesc,
                                              stlInt32               aSynonymNameLen );
stlChar * eldcGetObjectNamePtrOfPublic( ellPublicSynonymDesc * aPublicSynonymDesc,
                                        stlInt32               aSynonymNameLen,
                                        stlInt32               aObjectSchemaNameLen );

stlInt32  eldcCalSizeHashDataPublicSynonymID();
stlInt32  eldcCalSizeHashDataPublicSynonymName( stlInt32 aSynonymNameLen );
stlChar * eldcGetHashDataPublicSynonymNamePtr( eldcHashDataPublicSynonymName * aHashDataPublicSynonymName );

/*
 * Dump Information
 */

void eldcPrintPublicSynonymDesc( void * aSynonymDesc, stlChar * aStringBuffer );
void eldcPrintHashDataPublicSynonymID( void * aHashDataSynonymID, stlChar * aStringBuffer );
void eldcPrintHashDataPublicSynonymName( void * aHashDataSynonymName, stlChar * aStringBuffer );

/*
 * 초기화, 마무리 함수
 */

void eldcSetSQLPublicSynonymDumpHandle();

stlStatus eldcBuildPublicSynonymCache( smlTransId     aTransID,
                                       smlStatement * aStmt,
                                       ellEnv       * aEnv );

stlStatus eldcBuildCacheSQLPublicSynonym( smlTransId       aTransID,
                                          smlStatement   * aStmt,
                                          ellEnv         * aEnv );


/*
 * 조회 함수 
 */

stlStatus eldcGetPublicSynonymHandleByName( smlTransId           aTransID,
                                            smlScn               aViewSCN,
                                            stlChar            * aSynonymName,
                                            ellDictHandle      * aSynonymDictHandle,
                                            stlBool            * aExist,
                                            ellEnv             * aEnv );

/*
 * 제어 함수
 */

stlStatus eldcInsertCachePublicSynonym( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        knlValueBlockList * aValueList,
                                        ellEnv            * aEnv );

stlStatus eldcInsertCachePublicSynonymBySynonymID( smlTransId          aTransID,
                                                   smlStatement      * aStmt,
                                                   stlInt64            aSynonymID,
                                                   ellEnv            * aEnv );

stlStatus eldcDeleteCachePublicSynonym( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        ellDictHandle     * aSynonymHandle,
                                        ellEnv            * aEnv );

/*
 * 멤버 함수 
 */

stlStatus eldcMakePublicSynonymDesc( smlTransId              aTransID,
                                     smlStatement          * aStmt,
                                     knlValueBlockList     * aValueArray,
                                     ellPublicSynonymDesc ** aPublicSynonymDesc,
                                     ellEnv                * aEnv );

stlStatus eldcMakeHashElementPublicSynonymID( eldcHashElement      ** aHashElement,
                                              smlTransId              aTransID,
                                              ellPublicSynonymDesc  * aSynonymDesc,
                                              ellEnv                * aEnv );

stlStatus eldcMakeHashElementPublicSynonymName( eldcHashElement      ** aHashElement,
                                                smlTransId              aTransID,
                                                ellPublicSynonymDesc  * aPublicSynonymDesc,
                                                eldcHashElement       * aHashElementID,
                                                ellEnv                * aEnv );

/** @} eldcPublicSynonymDesc */


#endif /* _ELDC_SYNONYM_DESC_H_ */
