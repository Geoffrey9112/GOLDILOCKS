/*******************************************************************************
 * eldcTablespaceDesc.h
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


#ifndef _ELDC_TABLESPACE_DESC_H_
#define _ELDC_TABLESPACE_DESC_H_ 1

/**
 * @file eldcTablespaceDesc.h
 * @brief Cache for Tablespace descriptor
 */


/**
 * @defgroup eldcTablespaceDesc Cache for Tablespace descriptor
 * @ingroup eldcObject
 * @{
 */

/**
 * @brief Tablespace 의 Name Hash Data
 */
typedef struct eldcHashDataTablespaceName
{
    stlChar         * mKeyTablespaceName;/**< Name Hash 의 Key : Tablespace Name */
    stlInt64          mDataTablespaceID; /**< Name Hash 의 Data: Tablespace ID */
    eldcHashElement * mDataHashElement;  /**< Name Hash 의 Data: Space ID 의 Hash Element */
} eldcHashDataTablespaceName;

/**
 * @brief Tablespace 의 ID Hash Data
 */
typedef struct eldcHashDataTablespaceID
{
    stlInt64   mKeyTablespaceID;    /**< ID Hash 의 Key : Tablespace ID */
} eldcHashDataTablespaceID;









/*
 * Hash 함수 
 */

stlUInt32  eldcHashFuncTablespaceID( void * aHashKey );
stlBool    eldcCompareFuncTablespaceID( void            * aHashKey,
                                        eldcHashElement * aHashElement );

stlUInt32  eldcHashFuncTablespaceName( void * aHashKey );
stlBool    eldcCompareFuncTablespaceName( void            * aHashKey,
                                          eldcHashElement * aHashElement );


/*
 * Buffer Size 계산
 */

stlInt32  eldcCalSizeTablespaceDesc( stlInt32 aTablespaceNameLen );
stlChar * eldcGetTablespaceNamePtr( ellTablespaceDesc * aTablespaceDesc );

stlInt32  eldcCalSizeHashDataTablespaceID();

stlInt32  eldcCalSizeHashDataTablespaceName( stlInt32 aTablespaceNameLen );
stlChar * eldcGetHashDataTablespaceNamePtr( eldcHashDataTablespaceName * aHashDataTablespaceName );


/*
 * Dump Information
 */

void eldcPrintTablespaceDesc( void    * aTablespaceDesc,
                              stlChar * aStringBuffer );
void eldcPrintHashDataTablespaceID( void    * aHashDataTablespaceID,
                                    stlChar * aStringBuffer );
void eldcPrintHashDataTablespaceName( void    * aHashDataTablespaceName,
                                      stlChar * aStringBuffer );


/*
 * 초기화, 마무리 함수
 */

void eldcSetSQLTablespaceDumpHandle();

stlStatus eldcBuildTablespaceCache( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    ellEnv       * aEnv );

stlStatus eldcBuildCacheSQLTablespace( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       ellEnv         * aEnv );

/*
 * 조회 함수 
 */

stlStatus eldcGetTablespaceHandleByID( smlTransId           aTransID,
                                       smlScn               aViewSCN,
                                       stlInt64             aTablespaceID,
                                       ellDictHandle      * aSpaceDictHandle,
                                       stlBool            * aExist,
                                       ellEnv             * aEnv );

stlStatus eldcGetTablespaceHandleByName( smlTransId           aTransID,
                                         smlScn               aViewSCN,
                                         stlChar            * aTablespaceName,
                                         ellDictHandle      * aSpaceDictHandle,
                                         stlBool            * aExist,
                                         ellEnv             * aEnv );

/*
 * 제어 함수
 */

stlStatus eldcInsertCacheTablespace( smlTransId          aTransID,
                                     smlStatement      * aStmt,
                                     knlValueBlockList * aValueArray,
                                     ellEnv            * aEnv );

stlStatus eldcInsertCacheTablespaceByTablespaceID( smlTransId          aTransID,
                                                   smlStatement      * aStmt,
                                                   stlInt64            aTablespaceID,
                                                   ellEnv            * aEnv );

stlStatus eldcDeleteCacheTablespace( smlTransId          aTransID,
                                     smlStatement      * aStmt,
                                     ellDictHandle     * aSpaceHandle,
                                     ellEnv            * aEnv );

/*
 * 멤버 함수 
 */

stlStatus eldcMakeTablespaceDesc( smlTransId           aTransID,
                                  smlStatement       * aStmt,
                                  knlValueBlockList  * aValueArray,
                                  ellTablespaceDesc ** aTablespaceDesc,
                                  ellEnv             * aEnv );

stlStatus eldcMakeHashElementTablespaceID( eldcHashElement   ** aHashElement,
                                           smlTransId           aTransID,
                                           ellTablespaceDesc  * aTablespaceDesc,
                                           ellEnv             * aEnv );


stlStatus eldcMakeHashElementTablespaceName( eldcHashElement   ** aHashElement,
                                             smlTransId           aTransID,
                                             ellTablespaceDesc  * aTablespaceDesc,
                                             eldcHashElement    * aHashElementID,
                                             ellEnv             * aEnv );



/** @} eldcTablespaceDesc */


#endif /* _ELDC_TABLESPACE_DESC_H_ */
