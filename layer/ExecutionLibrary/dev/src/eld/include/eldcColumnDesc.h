/*******************************************************************************
 * eldcColumnDesc.h
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


#ifndef _ELDC_COLUMN_DESC_H_
#define _ELDC_COLUMN_DESC_H_ 1

/**
 * @file eldcColumnDesc.h
 * @brief Cache for Column descriptor
 */


/**
 * @defgroup eldcColumnDesc Cache for Column descriptor
 * @ingroup eldcObject
 * @{
 */

/**
 * @brief Column 의 ID Hash Data
 */
typedef struct eldcHashDataColumnID
{
    stlInt64   mKeyColumnID;            /**< ID Hash 의 Key : Column ID */
} eldcHashDataColumnID;

/**
 * @brief Column 의 Name Hash Data
 */
typedef struct eldcHashDataColumnName
{
    stlInt64          mKeyTableID;      /**< Name Hash 의 Key : Table ID */
    stlChar         * mKeyColumnName;   /**< Name Hash 의 Key : Column Name */
    stlInt64          mDataColumnID;    /**< Name Hash 의 Data: Column ID */
    eldcHashElement * mDataHashElement; /**< Name Hash 의 Data: Column ID 의 Hash Element */
} eldcHashDataColumnName;

/**
 * @brief Column Name 검색을 위한 Hash Key
 */
typedef struct eldcHashKeyColumnName
{
    stlInt64     mTableID;       /**< Table ID */
    stlChar    * mColumnName;    /**< Column Name */
} eldcHashKeyColumnName;







/*
 * Hash 함수 
 */

stlUInt32  eldcHashFuncColumnID( void * aHashKey );
stlBool    eldcCompareFuncColumnID( void * aHashKey, eldcHashElement * aHashElement );

stlUInt32  eldcHashFuncColumnName( void * aHashKey );
stlBool    eldcCompareFuncColumnName( void * aHashKey, eldcHashElement * aHashElement );


/*
 * Buffer Size 계산
 */

stlInt32  eldcCalSizeColumnDesc( stlInt32 aColNameLen, stlInt32 aDefStringLen );
stlChar * eldcGetColumnNamePtr( ellColumnDesc * aColumnDesc );
stlChar * eldcGetColumnDefStringPtr( ellColumnDesc * aColumnDesc, stlInt32 aColNameLen );

stlInt32  eldcCalSizeHashDataColumnID();

stlInt32  eldcCalSizeHashDataColumnName( stlInt32 aColNameLen );
stlChar * eldcGetHashDataColumnNamePtr( eldcHashDataColumnName * aHashDataColumnName );


/*
 * Dump Information
 */

void eldcPrintColumnDesc( void * aColumnDesc, stlChar * aStringBuffer );
void eldcPrintHashDataColumnID( void * aHashDataColumnID, stlChar * aStringBuffer );
void eldcPrintHashDataColumnName( void * aHashDataColumnName, stlChar * aStringBuffer );


/*
 * 초기화, 마무리 함수
 */

void eldcSetSQLColumnDumpHandle();

stlStatus eldcBuildColumnCache( smlTransId     aTransID,
                                smlStatement * aStmt,
                                ellEnv       * aEnv );

stlStatus eldcBuildCacheSQLColumn( smlTransId       aTransID,
                                   smlStatement   * aStmt,
                                   ellEnv         * aEnv );

/*
 * 조회 함수 
 */

stlStatus eldcGetColumnHandleByID( smlTransId           aTransID,
                                   smlScn               aViewSCN,
                                   stlInt64             aColumnID,
                                   ellDictHandle      * aColumnDictHandle,
                                   stlBool            * aExist,
                                   ellEnv             * aEnv );

stlStatus eldcGetColumnHandleByName( smlTransId           aTransID,
                                     smlScn               aViewSCN,
                                     stlInt64             aTableID,
                                     stlChar            * aColumnName,
                                     ellDictHandle      * aColumnDictHandle,
                                     stlBool            * aExist,
                                     ellEnv             * aEnv );


/*
 * 제어 함수
 */

stlStatus eldcInsertCacheColumn( smlTransId          aTransID,
                                 smlStatement      * aStmt,
                                 knlValueBlockList * aValueArray,
                                 ellEnv            * aEnv );

stlStatus eldcInsertCacheColumnByTableID( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aTableID,
                                          ellEnv            * aEnv );

stlStatus eldcDeleteCacheColumn( smlTransId          aTransID,
                                 ellDictHandle     * aColumnHandle,
                                 ellEnv            * aEnv );

/*
 * 멤버 함수 
 */

stlStatus eldcMakeColumnDesc( smlTransId          aTransID,
                              smlStatement      * aStmt,
                              knlValueBlockList * aValueArray,
                              ellColumnDesc    ** aColumnDesc,
                              ellEnv            * aEnv );

stlStatus eldcMakeHashElementColumnID( eldcHashElement   ** aHashElement,
                                       smlTransId           aTransID,
                                       ellColumnDesc      * aColumnDesc,
                                       ellEnv             * aEnv );


stlStatus eldcMakeHashElementColumnName( eldcHashElement   ** aHashElement,
                                         smlTransId           aTransID,
                                         ellColumnDesc      * aColumnDesc,
                                         eldcHashElement    * aHashElementID,
                                         ellEnv             * aEnv );

stlStatus eldcSetColumnType( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             stlInt64        aDTD,
                             ellColumnDesc * aColumnDesc,
                             ellEnv        * aEnv );


/** @} eldcColumnDesc */


#endif /* _ELDC_COLUMN_DESC_H_ */
