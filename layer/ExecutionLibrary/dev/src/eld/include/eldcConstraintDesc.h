/*******************************************************************************
 * eldcConstraintDesc.h
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


#ifndef _ELDC_CONSTRAINT_DESC_H_
#define _ELDC_CONSTRAINT_DESC_H_ 1

/**
 * @file eldcConstraintDesc.h
 * @brief Cache for Constraint descriptor
 */


/**
 * @defgroup eldcConstraintDesc Cache for Constraint descriptor
 * @ingroup eldcObject
 * @{
 */

/**
 * @brief Constraint 의 Name Hash Data
 */
typedef struct eldcHashDataConstName
{
    stlInt64          mKeySchemaID;      /**< Name Hash 의 Key : Schema ID */
    stlChar         * mKeyConstName;     /**< Name Hash 의 Key : Constraint Name */
    stlInt64          mDataConstID;      /**< Name Hash 의 Data: Constraint ID */
    eldcHashElement * mDataHashElement;  /**< Name Hash 의 Data: Const ID 의 Hash Element */
} eldcHashDataConstName;

/**
 * @brief Constraint 의 ID Hash Data
 */
typedef struct eldcHashDataConstID
{
    stlInt64   mKeyConstID;              /**< ID Hash 의 Key : Constraint ID */
} eldcHashDataConstID;


/**
 * @brief Constraint Name 검색을 위한 Hash Key
 */
typedef struct eldcHashKeyConstName
{
    stlInt64     mSchemaID;              /**< Schema ID */
    stlChar    * mConstName;             /**< Constraint Name */
} eldcHashKeyConstName;







/*
 * Hash 함수 
 */

stlUInt32  eldcHashFuncConstID( void * aHashKey );
stlBool    eldcCompareFuncConstID( void * aHashKey, eldcHashElement * aHashElement );

stlUInt32  eldcHashFuncConstName( void * aHashKey );
stlBool    eldcCompareFuncConstName( void * aHashKey, eldcHashElement * aHashElement );


/*
 * Buffer Size 계산
 */

stlInt32  eldcCalSizeConstDesc( stlInt32               aConstNameLen,
                                ellTableConstraintType aConstType,
                                stlInt32               aCheckClauseLen );
stlChar * eldcGetConstNamePtr( ellTableConstDesc * aConstDesc );
void    * eldcGetConstTypePtr( ellTableConstDesc * aConstDesc,
                               stlInt32            aConstNameLen );
stlChar * eldcGetCheckClausePtr( ellTableConstDesc * aConstDesc,
                                 stlInt32            aConstNameLen );

stlInt32  eldcCalSizeHashDataConstID();
stlInt32  eldcCalSizeHashDataConstName( stlInt32 aConstNameLen );
stlChar * eldcGetHashDataConstNamePtr( eldcHashDataConstName * aHashDataConstName );


/*
 * Dump Information
 */

void eldcPrintConstDesc( void * aConstDesc, stlChar * aStringBuffer );
void eldcPrintHashDataConstID( void * aHashDataConstID, stlChar * aStringBuffer );
void eldcPrintHashDataConstName( void * aHashDataConstName, stlChar * aStringBuffer );

/*
 * 초기화, 마무리 함수
 */

void eldcSetSQLConstDumpHandle();

stlStatus eldcBuildConstCache( smlTransId     aTransID,
                               smlStatement * aStmt,
                               ellEnv       * aEnv );

stlStatus eldcBuildCacheSQLConst( smlTransId       aTransID,
                                  smlStatement   * aStmt,
                                  ellEnv         * aEnv );

/*
 * 조회 함수 
 */

stlStatus eldcGetConstHandleByID( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  stlInt64             aConstID,
                                  ellDictHandle      * aConstDictHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv );

stlStatus eldcGetConstHandleByName( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aSchemaID,
                                    stlChar            * aConstName,
                                    ellDictHandle      * aConstDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv );

/*
 * 제어 함수
 */

stlStatus eldcInsertCacheConst( smlTransId          aTransID,
                                smlStatement      * aStmt,
                                knlValueBlockList * aValueArray,
                                ellEnv            * aEnv );

stlStatus eldcInsertCacheConstByConstID( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aConstID,
                                         ellEnv            * aEnv );

stlStatus eldcInsertCacheConstByTableID( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aTableID,
                                         ellEnv            * aEnv );

stlStatus eldcDeleteCacheConst( smlTransId          aTransID,
                                ellDictHandle     * aConstHandle,
                                ellEnv            * aEnv );

stlStatus eldcAddConstraintIntoTable( smlTransId     aTransID,
                                      stlInt64       aConstID,
                                      ellEnv       * aEnv );

stlStatus eldcDelConstraintFromTable( smlTransId      aTransID,
                                      ellDictHandle * aTableHandle,
                                      ellDictHandle * aConstHandle,
                                      ellEnv        * aEnv );

stlStatus eldcAddChildForeignKeyIntoTable( smlTransId      aTransID,
                                           ellDictHandle * aChildForeignKeyHandle,
                                           ellEnv        * aEnv );

stlStatus eldcDelChildForeignKeyFromTable( smlTransId      aTransID,
                                           smlStatement  * aStmt,
                                           ellDictHandle * aTableHandle,
                                           ellDictHandle * aChildForeignKeyHandle,
                                           ellEnv        * aEnv );

/*
 * 멤버 함수 
 */

stlStatus eldcMakeConstDesc( smlTransId           aTransID,
                             smlStatement       * aStmt,
                             knlValueBlockList  * aValueArray,
                             ellTableConstDesc ** aConstDesc,
                             ellEnv             * aEnv );


stlStatus eldcMakeHashElementConstID( eldcHashElement   ** aHashElement,
                                      smlTransId           aTransID,
                                      ellTableConstDesc  * aConstDesc,
                                      ellEnv             * aEnv );


stlStatus eldcMakeHashElementConstName( eldcHashElement   ** aHashElement,
                                        smlTransId           aTransID,
                                        ellTableConstDesc  * aConstDesc,
                                        eldcHashElement    * aHashElementID,
                                        ellEnv             * aEnv );

stlStatus eldcGetCheckClauseLen( smlTransId           aTransID,
                                 smlStatement       * aStmt,
                                 stlInt64             aCheckClauseConstID,
                                 stlInt32           * aCheckClauseLen,
                                 ellEnv             * aEnv );

stlStatus eldcSetCheckNotNull( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               ellTableConstDesc  * aConstDesc,
                               ellEnv             * aEnv );

stlStatus eldcSetCheckClause( smlTransId           aTransID,
                              smlStatement       * aStmt,
                              ellTableConstDesc  * aConstDesc,
                              ellEnv             * aEnv );

stlStatus eldcSetPrimaryKey( smlTransId           aTransID,
                             smlStatement       * aStmt,
                             ellTableConstDesc  * aConstDesc,
                             ellEnv             * aEnv );

stlStatus eldcSetUniqueKey( smlTransId           aTransID,
                            smlStatement       * aStmt,
                            ellTableConstDesc  * aConstDesc,
                            ellEnv             * aEnv );

stlStatus eldcSetForeignKey( smlTransId           aTransID,
                             smlStatement       * aStmt,
                             ellTableConstDesc  * aConstDesc,
                             ellEnv             * aEnv );

stlStatus eldcAddReferentialIntoTable( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       ellDictHandle * aForeignKeyHandle,
                                       ellEnv        * aEnv );

                                 
/** @} eldcConstraintDesc */


#endif /* _ELDC_CONSTRAINT_DESC_H_ */
