/*******************************************************************************
 * ellDictTableConstraint.h
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


#ifndef _ELL_DICT_TABLE_CONSTRAINT_H_
#define _ELL_DICT_TABLE_CONSTRAINT_H_ 1

/**
 * @file ellDictTableConstraint.h
 * @brief Constraint Dictionary Management
 */

/**
 * @addtogroup ellObjectConstraint
 * @{
 */

/*
 * DDL 관련 Object 획득 함수
 */

stlStatus ellGetObjectList4DropPK( smlTransId          aTransID,
                                   smlStatement      * aStmt,
                                   ellDictHandle     * aPrimaryKeyHandle,
                                   knlRegionMem      * aRegionMem,
                                   ellObjectList    ** aChildForeignKeyList,
                                   stlBool           * aHasOuterChildForeignKey,
                                   ellObjectList    ** aNotNullList,
                                   ellEnv            * aEnv );

stlStatus ellGetObjectList4DropUK( smlTransId          aTransID,
                                   smlStatement      * aStmt,
                                   ellDictHandle     * aPrimaryKeyHandle,
                                   knlRegionMem      * aRegionMem,
                                   ellObjectList    ** aChildForeignKeyList,
                                   stlBool           * aHasOuterChildForeignKey,
                                   ellEnv            * aEnv );

stlStatus ellGetObjectList4DropFK( smlTransId          aTransID,
                                   smlStatement      * aStmt,
                                   ellDictHandle     * aForeignKeyHandle,
                                   knlRegionMem      * aRegionMem,
                                   ellObjectList    ** aParentKeyList,
                                   ellEnv            * aEnv );

stlStatus ellGetChildForeignKeyListByUniqueList( smlTransId          aTransID,
                                                 smlStatement      * aStmt,
                                                 ellObjectList     * aUniqueList,
                                                 knlRegionMem      * aRegionMem,
                                                 ellObjectList    ** aChildForeignKeyList,
                                                 ellEnv            * aEnv );
                                        
                                        
stlStatus ellGetObjectList4DropNotNull( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        ellDictHandle     * aNotNullHandle,
                                        knlRegionMem      * aRegionMem,
                                        ellObjectList    ** aPrimaryKeyList,
                                        ellEnv            * aEnv );

stlStatus ellGetTableAllConstraintList( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        stlInt64            aTableID,
                                        knlRegionMem      * aRegionMem,
                                        ellObjectList    ** aConstraintList,
                                        ellEnv            * aEnv );

stlStatus ellGetTableConstTypeList( smlTransId                aTransID,
                                    smlStatement            * aStmt,
                                    stlInt64                  aTableID,
                                    ellTableConstraintType    aConstType,
                                    knlRegionMem            * aRegionMem,
                                    ellObjectList          ** aConstList,
                                    ellEnv                  * aEnv );

stlStatus ellGetParentTableList4ForeignKeyList( smlTransId          aTransID,
                                                smlScn              aViewSCN,
                                                ellObjectList     * aForeignKeyList,
                                                knlRegionMem      * aRegionMem,
                                                ellObjectList    ** aParentTableList,
                                                ellEnv            * aEnv );

stlStatus ellGetTableList4ConstList( smlTransId          aTransID,
                                     smlScn              aViewSCN,
                                     ellObjectList     * aConstList,
                                     knlRegionMem      * aRegionMem,
                                     ellObjectList    ** aTableList,
                                     ellEnv            * aEnv );

stlStatus ellGetIndexList4KeyList( smlTransId          aTransID,
                                   ellObjectList     * aKeyList,
                                   knlRegionMem      * aRegionMem,
                                   ellObjectList    ** aIndexList,
                                   ellEnv            * aEnv );

/*
 * Constraint 에 대한 DDL Lock 함수 
 */

stlStatus ellLock4AddConst( smlTransId               aTransID,
                            smlStatement           * aStmt,
                            ellDictHandle          * aConstOwnerHandle,
                            ellDictHandle          * aConstSchemaHandle,
                            ellDictHandle          * aConstSpaceHandle,
                            ellDictHandle          * aTableDictHandle,
                            ellTableConstraintType   aConstType,
                            stlBool                * aLockSuccess,
                            ellEnv                 * aEnv );

stlStatus ellLock4DropConst( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellDictHandle * aTableDictHandle,
                             ellDictHandle * aConstDictHandle,
                             stlBool       * aLockSuccess,
                             ellEnv        * aEnv );

stlStatus ellLock4AlterConst( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              ellDictHandle * aTableDictHandle,
                              ellDictHandle * aConstDictHandle,
                              stlBool       * aLockSuccess,
                              ellEnv        * aEnv );

stlStatus ellLock4ConstraintList( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  ellObjectList * aConstList,
                                  stlBool       * aLockSuccess,
                                  ellEnv        * aEnv );

/*
 * Constraint 관련 객체에 대한 DDL Lock 함수 
 */





/*
 * Dictionary 변경 함수
 */

stlStatus ellInsertTableConstraintDesc( smlTransId               aTransID,
                                        smlStatement           * aStmt,
                                        stlInt64                 aConstOwnerID,
                                        stlInt64                 aConstSchemaID,
                                        stlInt64               * aConstID,
                                        stlInt64                 aTableOwnerID,
                                        stlInt64                 aTableSchemaID,
                                        stlInt64                 aTableID,
                                        stlChar                * aConstName,
                                        ellTableConstraintType   aConstType,
                                        stlBool                  aIsDeferrable,
                                        stlBool                  aInitiallyDeferred,
                                        stlBool                  aEnforced,
                                        stlInt64                 aIndexID,
                                        stlChar                * aConstComment,
                                        ellEnv                 * aEnv );

stlStatus ellDeleteDict4DropConstraint( smlTransId        aTransID,
                                        smlStatement    * aStmt,
                                        ellDictHandle   * aConstHandle,
                                        ellEnv          * aEnv );

stlStatus ellDeleteDictConstraintByObjectList( smlTransId        aTransID,
                                               smlStatement    * aStmt,
                                               ellObjectList   * aConstObjectList,
                                               ellEnv          * aEnv );

stlStatus ellModifyDict4AlterConstFeature( smlTransId       aTransID,
                                           smlStatement   * aStmt,
                                           ellDictHandle  * aConstHandle,
                                           stlBool          aIsDeferrable,
                                           stlBool          aInitiallyDeferred,
                                           stlBool          aEnforced,
                                           ellEnv         * aEnv );

/*
 * Cache 재구축 함수
 */

stlStatus ellRefineCache4AddConstraint( smlTransId               aTransID,
                                        smlStatement           * aStmt,
                                        ellDictHandle          * aTableHandle,
                                        stlInt64                 aConstID,
                                        ellTableConstraintType   aConstType,
                                        ellEnv                 * aEnv );

stlStatus ellRefineCache4DropConstraint( smlTransId           aTransID,
                                         smlStatement       * aStmt,
                                         ellDictHandle      * aTableHandle,
                                         ellDictHandle      * aConstHandle,
                                         ellEnv             * aEnv );

stlStatus ellRefineCache4DropNotNullList( smlTransId         aTransID,
                                          smlStatement     * aStmt,
                                          ellObjectList    * aConstList,
                                          ellObjectList    * aConstTableList,
                                          ellObjectList    * aDistinctTableList,
                                          ellEnv           * aEnv );

stlStatus ellRefineCache4AlterConstraint( smlTransId           aTransID,
                                          smlStatement       * aStmt,
                                          ellDictHandle      * aTableHandle,
                                          ellDictHandle      * aConstHandle,
                                          ellEnv             * aEnv );

stlStatus ellUnlinkChildForeignKeyFromParentTable( smlTransId      aTransID,
                                                   smlStatement  * aStmt,
                                                   ellDictHandle * aParentTableHandle,
                                                   ellDictHandle * aChildForeignKeyHandle,
                                                   ellEnv        * aEnv );


/*
 * Handle 획득 함수 
 */

stlStatus ellGetConstraintDictHandleWithSchema( smlTransId           aTransID,
                                                smlScn               aViewSCN,
                                                ellDictHandle      * aSchemaDictHandle,
                                                stlChar            * aConstName,
                                                ellDictHandle      * aConstDictHandle,
                                                stlBool            * aExist,
                                                ellEnv             * aEnv );

stlStatus ellGetConstraintDictHandleWithAuth( smlTransId           aTransID,
                                              smlScn               aViewSCN,
                                              ellDictHandle      * aAuthDictHandle,
                                              stlChar            * aConstName,
                                              ellDictHandle      * aConstDictHandle,
                                              stlBool            * aExist,
                                              ellEnv             * aEnv );


stlStatus ellGetTablePrimaryKeyDictHandle( smlTransId       aTransID,
                                           smlScn           aViewSCN,
                                           knlRegionMem   * aRegionMem,
                                           ellDictHandle  * aTableDictHandle,
                                           stlInt32       * aPrimaryKeyCnt,
                                           ellDictHandle ** aPrimaryKeyDictHandleArray,
                                           ellEnv         * aEnv );

stlStatus ellGetTableUniqueKeyDictHandle( smlTransId       aTransID,
                                          smlScn           aViewSCN,
                                          knlRegionMem   * aRegionMem,
                                          ellDictHandle  * aTableDictHandle,
                                          stlInt32       * aUniqueKeyCnt,
                                          ellDictHandle ** aUniqueKeyDictHandleArray,
                                          ellEnv         * aEnv );

stlStatus ellGetTableForeignKeyDictHandle( smlTransId       aTransID,
                                           smlScn           aViewSCN,
                                           knlRegionMem   * aRegionMem,
                                           ellDictHandle  * aTableDictHandle,
                                           stlInt32       * aForeignKeyCnt,
                                           ellDictHandle ** aForeignKeyDictHandleArray,
                                           ellEnv         * aEnv );

stlStatus ellGetTableChildForeignKeyDictHandle( smlTransId       aTransID,
                                                smlScn           aViewSCN,
                                                knlRegionMem   * aRegionMem,
                                                ellDictHandle  * aTableDictHandle,
                                                stlInt32       * aChildForeignKeyCnt,
                                                ellDictHandle ** aChildForeignKeyArray,
                                                ellEnv         * aEnv );

stlStatus ellGetTableCheckNotNullDictHandle( smlTransId       aTransID,
                                             smlScn           aViewSCN,
                                             knlRegionMem   * aRegionMem,
                                             ellDictHandle  * aTableDictHandle,
                                             stlInt32       * aCheckNotNullCnt,
                                             ellDictHandle ** aCheckNotNullDictHandleArray,
                                             ellEnv         * aEnv );

stlStatus ellGetTableCheckClauseDictHandle( smlTransId       aTransID,
                                            smlScn           aViewSCN,
                                            knlRegionMem   * aRegionMem,
                                            ellDictHandle  * aTableDictHandle,
                                            stlInt32       * aCheckClauseCnt,
                                            ellDictHandle ** aCheckClauseDictHandleArray,
                                            ellEnv         * aEnv );

/*
 * 정보 생성 함수
 */

void ellMakePrimaryKeyName( stlChar  * aNameBuffer,
                            stlInt32   aNameBufferSize,
                            stlChar  * aTableName );

void ellMakeUniqueKeyName( stlChar     * aNameBuffer,
                           stlInt32      aNameBufferSize,
                           stlChar     * aTableName,
                           ellNameList * aColumnNameList );

void ellMakeForeignKeyName( stlChar     * aNameBuffer,
                            stlInt32      aNameBufferSize,
                            stlChar     * aMyTableName,
                            stlChar     * aRefTableName );

void ellMakeCheckNotNullName( stlChar     * aNameBuffer,
                              stlInt32      aNameBufferSize,
                              stlChar     * aTableName,
                              stlChar     * aColumnName );
                           
void ellMakeCheckClauseName( stlChar     * aNameBuffer,
                             stlInt32      aNameBufferSize,
                             stlChar     * aTableName,
                             ellNameList * aColumnNameList );

void ellMakeKeyIndexName( stlChar     * aNameBuffer,
                          stlInt32      aNameBufferSize,
                          stlChar     * aKeyConstraintName );
                           
/*
 * 정보 획득 함수
 */

stlInt64 ellGetConstraintID( ellDictHandle * aConstDictHandle );
stlChar * ellGetConstraintName( ellDictHandle * aConstDictHandle );
stlInt64 ellGetConstraintOwnerID( ellDictHandle * aConstDictHandle );
stlInt64 ellGetConstraintSchemaID( ellDictHandle * aConstDictHandle );

stlInt64 ellGetConstraintTableOwnerID( ellDictHandle * aConstDictHandle );
stlInt64 ellGetConstraintTableSchemaID( ellDictHandle * aConstDictHandle );
stlInt64 ellGetConstraintTableID(ellDictHandle * aConstDictHandle);

ellTableConstraintType ellGetConstraintType( ellDictHandle * aConstDictHandle );

stlBool  ellGetConstraintAttrIsDeferrable( ellDictHandle * aConstDictHandle );
stlBool  ellGetConstraintAttrIsInitDeferred( ellDictHandle * aConstDictHandle );
stlBool  ellGetConstraintAttrIsEnforced( ellDictHandle * aConstDictHandle );
stlBool  ellGetConstraintAttrIsValidate( ellDictHandle * aConstDictHandle );

ellDictHandle * ellGetConstraintIndexDictHandle( ellDictHandle * aConstDictHandle );

stlInt32        ellGetPrimaryKeyColumnCount( ellDictHandle * aPrimaryKeyHandle );
ellDictHandle * ellGetPrimaryKeyColumnHandle( ellDictHandle * aPrimaryKeyHandle );


stlInt32        ellGetUniqueKeyColumnCount( ellDictHandle * aUniqueKeyHandle );
ellDictHandle * ellGetUniqueKeyColumnHandle( ellDictHandle * aUniqueKeyHandle );


stlInt32        ellGetForeignKeyColumnCount( ellDictHandle * aForeignKeyHandle );
ellDictHandle * ellGetForeignKeyColumnHandle( ellDictHandle * aForeignKeyHandle );
stlInt32      * ellGetForeignKeyMatchIdxInParent( ellDictHandle * aForeignKeyHandle );
ellReferentialMatchOption ellGetForeignKeyMatchOption( ellDictHandle * aForeignKeyHandle );
ellReferentialActionRule ellGetForeignKeyUpdateRule( ellDictHandle * aForeignKeyHandle );
ellReferentialActionRule ellGetForeignKeyDeleteRule( ellDictHandle * aForeignKeyHandle );
ellDictHandle * ellGetForeignKeyParentUniqueKeyHandle( ellDictHandle * aForeignKeyHandle );

ellDictHandle * ellGetCheckNotNullColumnHandle( ellDictHandle * aCheckNotNullHandle );

stlChar  * ellGetCheckClausePhrase( ellDictHandle * aCheckClauseHandle );

stlBool  ellIsBuiltInConstraint( ellDictHandle * aConstDictHandle );

/** @} ellObjectConstraint */



#endif /* _ELL_DICT_TABLE_CONSTRAINT_H_ */
