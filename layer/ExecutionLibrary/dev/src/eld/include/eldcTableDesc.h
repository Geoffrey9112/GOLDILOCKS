/*******************************************************************************
 * eldcTableDesc.h
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


#ifndef _ELDC_TABLE_DESC_H_
#define _ELDC_TABLE_DESC_H_ 1

/**
 * @file eldcTableDesc.h
 * @brief Cache for Table(View) descriptor
 */


/**
 * @defgroup eldcTableDesc Cache for Table(View) descriptor
 * @ingroup eldcObject
 * @{
 */



/**
 * @brief Table 의 ID Hash Data
 */
typedef struct eldcHashDataTableID
{
    stlInt64                 mKeyTableID;        /**< ID Hash 의 Key : Table ID */
    eldcHashRelatedObjList * mCheckNotNull;      /**< ID Hash 의 Data: Check NOT NULL Const */
    eldcHashRelatedObjList * mCheckClause;       /**< ID Hash 의 Data: Check CLAUSE Const */
    eldcHashRelatedObjList * mPrimaryKey;        /**< ID Hash 의 Data: Primary Key */
    eldcHashRelatedObjList * mUniqueKey;         /**< ID Hash 의 Data: Unique Key */
    eldcHashRelatedObjList * mForeignKey;        /**< ID Hash 의 Data: Foreign Key */
    eldcHashRelatedObjList * mChildForeignKey;   /**< ID Hash 의 Data: Child Foreign Key */
    eldcHashRelatedObjList * mUniqueIndex;       /**< ID Hash 의 Data: Unique Index */
    eldcHashRelatedObjList * mNonUniqueIndex;    /**< ID Hash 의 Data: Non-Unique Index */
} eldcHashDataTableID;

/**
 * @brief Table 의 Name Hash Data
 */
typedef struct eldcHashDataTableName
{
    stlInt64          mKeySchemaID;     /**< Name Hash 의 Key : Schema ID */
    stlChar         * mKeyTableName;    /**< Name Hash 의 Key : Table Name */
    stlInt64          mDataTableID;     /**< Name Hash 의 Data: Table ID */
    eldcHashElement * mDataHashElement; /**< Name Hash 의 Data: Table ID 의 Hash Element */
} eldcHashDataTableName;

/**
 * @brief Table Name 검색을 위한 Hash Key
 */
typedef struct eldcHashKeyTableName
{
    stlInt64     mSchemaID;     /**< Schema ID */
    stlChar    * mTableName;    /**< Table Name */
} eldcHashKeyTableName;






/*
 * Hash 함수 
 */

stlUInt32  eldcHashFuncTableID( void * aHashKey );
stlBool    eldcCompareFuncTableID( void * aHashKey, eldcHashElement * aHashElement );

stlUInt32  eldcHashFuncTableName( void * aHashKey );
stlBool    eldcCompareFuncTableName( void * aHashKey, eldcHashElement * aHashElement );


/*
 * Buffer Size 계산
 */

stlInt32  eldcCalSizeTableDesc( stlInt32     aTableNameLen,
                                stlInt32     aColumnCnt,
                                stlInt32     aViewColumnLen,
                                stlInt32     aViewQueryLen );

stlChar * eldcGetTableNamePtr( ellTableDesc * aTableDesc );
ellDictHandle * eldcGetTableColumnDictHandlePtr( ellTableDesc * aTableDesc,
                                                 stlInt32       aTableNameLen );

stlChar * eldcGetViewColDefPtr( ellTableDesc * aTableDesc,
                                stlInt32       aTableNameLen );

stlChar * eldcGetViewSelectPtr( ellTableDesc * aTableDesc,
                                stlInt32       aTableNameLen,
                                stlInt32       aViewColDefLen );


stlInt32  eldcCalSizeHashDataTableID();
stlInt32  eldcCalSizeHashDataTableName( stlInt32 aTableNameLen );
stlChar * eldcGetHashDataTableNamePtr( eldcHashDataTableName * aHashDataTableName );

/*
 * Dump Information
 */

void eldcPrintTableDesc( void * aTableDesc, stlChar * aStringBuffer );
void eldcPrintHashDataTableID( void * aHashDataTableID, stlChar * aStringBuffer );
void eldcPrintHashDataTableName( void * aHashDataTableName, stlChar * aStringBuffer );

/*
 * 초기화, 마무리 함수
 */

void eldcSetSQLTableDumpHandle();

stlStatus eldcBuildTableCache( smlTransId     aTransID,
                               smlStatement * aStmt,
                               ellEnv       * aEnv );

stlStatus eldcBuildCacheSQLTable( smlTransId       aTransID,
                                  smlStatement   * aStmt,
                                  ellEnv         * aEnv );

/*
 * 조회 함수 
 */

stlStatus eldcGetTableHandleByID( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  stlInt64             aTableID,
                                  ellDictHandle      * aTableDictHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv );

stlStatus eldcGetTableHandleByName( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aSchemaID,
                                    stlChar            * aTableName,
                                    ellDictHandle      * aTableDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv );

stlStatus eldcGetTableRelatedObjectHandle( smlTransId               aTransID,
                                           smlScn                   aViewSCN,
                                           ellDictHandle          * aTableHandle,
                                           ellTableRelatedObject    aRelatedType,
                                           stlInt32               * aRelatedCnt,
                                           ellDictHandle         ** aRelatedHandleArray,
                                           knlRegionMem           * aRegionMem,
                                           ellEnv                 * aEnv );

/*
 * 제어 함수
 */

stlStatus eldcInsertCacheTable( smlTransId          aTransID,
                                smlStatement      * aStmt,
                                knlValueBlockList * aValueArray,
                                ellEnv            * aEnv );

stlStatus eldcInsertCacheNonServicePerfView( knlValueBlockList * aValueList,
                                             stlChar           * aViewColumnString,
                                             stlChar           * aViewQueryString,
                                             ellEnv            * aEnv );

stlStatus eldcInsertCacheTableByTableID( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aTableID,
                                         ellEnv            * aEnv );

stlStatus eldcDeleteCacheTable( smlTransId          aTransID,
                                ellDictHandle     * aTableHandle,
                                ellEnv            * aEnv );

/*
 * 멤버 함수 
 */

stlStatus eldcMakeTableDesc( smlTransId          aTransID,
                             smlStatement      * aStmt,
                             knlValueBlockList * aValueArray,
                             ellTableDesc     ** aTableDesc,
                             ellEnv            * aEnv );


stlStatus eldcMakeNonServicePerfViewDesc( knlValueBlockList * aValueList,
                                          stlChar           * aViewColumnString,
                                          stlChar           * aViewQueryString,
                                          ellTableDesc     ** aTableDesc,
                                          ellEnv            * aEnv );

stlStatus eldcMakeHashElementTableID( eldcHashElement   ** aHashElement,
                                      smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      ellTableDesc       * aTableDesc,
                                      ellEnv             * aEnv );


stlStatus eldcMakeHashElementTableName( eldcHashElement   ** aHashElement,
                                        smlTransId           aTransID,
                                        ellTableDesc       * aTableDesc,
                                        eldcHashElement    * aHashElementID,
                                        ellEnv             * aEnv );

stlStatus eldcGetColumnCount( smlTransId           aTransID,
                              smlStatement       * aStmt,
                              stlInt64             aTableID,
                              stlInt32           * aColumnCnt,
                              ellEnv             * aEnv );

stlStatus eldcSetColumnDictHandle( smlTransId           aTransID,
                                   smlStatement       * aStmt,
                                   ellTableDesc       * aTableDesc,
                                   ellEnv             * aEnv );

stlStatus eldcGetViewStringLength( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   stlInt64        aViewID,
                                   stlInt32      * aSelectLen,
                                   stlInt32      * aColDefLen,
                                   ellEnv        * aEnv );
                             
stlStatus eldcSetViewStringDesc( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 stlInt64        aViewID,
                                 stlChar       * aViewColumns,
                                 stlInt32        aColDefLen,
                                 stlChar       * aViewSelect,
                                 stlInt32        aSelectLen,
                                 ellEnv        * aEnv );
                             

/** @} eldcTableDesc */


#endif /* _ELDC_TABLE_DESC_H_ */
