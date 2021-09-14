/*******************************************************************************
 * ellDictTable.h
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


#ifndef _ELL_DICT_TABLE_H_
#define _ELL_DICT_TABLE_H_ 1

/**
 * @file ellDictTable.h
 * @brief Table Dictionary Management
 */

/**
 * @addtogroup ellObjectTable
 * @{
 */

/*
 * DDL 관련 Object 획득 함수
 */

stlStatus ellGetObjectList4DropTable( smlTransId          aTransID,
                                      smlStatement      * aStmt,
                                      ellDictHandle     * aTableDictHandle,
                                      knlRegionMem      * aRegionMem,
                                      ellObjectList    ** aInnerForeignKeyList,
                                      ellObjectList    ** aOuterChildForeignKeyList,
                                      ellEnv            * aEnv );

stlStatus ellGetObjectList4TruncateTable( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          ellDictHandle     * aTableDictHandle,
                                          knlRegionMem      * aRegionMem,
                                          ellObjectList    ** aEnableOuterChildForeignKeyList,
                                          ellEnv            * aEnv );


/*
 * Table 에 대한 DDL Lock 함수 
 */

stlStatus ellLock4CreateTable( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               ellDictHandle * aOwnerHandle,
                               ellDictHandle * aSchemaHandle,
                               ellDictHandle * aSpaceHandle,
                               ellObjectList * aConstSchemaList,
                               ellObjectList * aConstSpaceList,
                               stlBool       * aLockSuccess,
                               ellEnv        * aEnv );

stlStatus ellLock4DropTable( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellDictHandle * aTableDictHandle,
                             stlBool       * aLockSuccess,
                             ellEnv        * aEnv );

stlStatus ellLock4TruncateTable( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 ellDictHandle * aTableDictHandle,
                                 stlBool       * aLockSuccess,
                                 ellEnv        * aEnv );

stlStatus ellLock4RenameTable( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               ellDictHandle * aTableDictHandle,
                               stlBool       * aLockSuccess,
                               ellEnv        * aEnv );

stlStatus ellLock4AlterPhysicalAttr( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     ellDictHandle * aTableDictHandle,
                                     stlBool       * aLockSuccess,
                                     ellEnv        * aEnv );

stlStatus ellLock4TableSuppLog( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                ellDictHandle * aTableDictHandle,
                                stlBool       * aLockSuccess,
                                ellEnv        * aEnv );

stlStatus ellLockRelatedTableList4DDL( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       stlInt32        aLockMode,
                                       ellObjectList * aTableList,
                                       stlBool       * aLockSuccess,
                                       ellEnv        * aEnv );

stlStatus ellLockUnderlyingTableList4ViewDDL( smlTransId      aTransID,
                                              smlStatement  * aStmt,
                                              stlInt32        aLockMode,
                                              ellObjectList * aTableList,
                                              stlBool       * aLockSuccess,
                                              ellEnv        * aEnv );

/*
 * DDL 관련 Index 획득 함수
 */

stlStatus ellGetPrimeIndexList4DropTable( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          ellDictHandle     * aTableDictHandle,
                                          knlRegionMem      * aRegionMem,
                                          ellObjectList    ** aIndexObjectList,
                                          ellEnv            * aEnv );

stlStatus ellGetIndexList4TruncateTable( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         ellDictHandle     * aTableDictHandle,
                                         knlRegionMem      * aRegionMem,
                                         ellObjectList    ** aIndexObjectList,
                                         ellEnv            * aEnv );

/*
 * Dictionary 변경 함수 
 */

stlStatus ellSetTime4ModifyTableElement( smlTransId      aTransID,
                                         smlStatement  * aStmt,
                                         ellDictHandle * aTableDictHandle,
                                         ellEnv        * aEnv );

stlStatus ellInsertTableDesc( smlTransId     aTransID,
                              smlStatement * aStmt,
                              stlInt64       aOwnerID,
                              stlInt64       aSchemaID,
                              stlInt64     * aTableID,
                              stlInt64       aTablespaceID,
                              stlInt64       aPhysicalID,
                              stlChar      * aTableName,
                              ellTableType   aTableType,
                              stlChar      * aTableComment,
                              ellEnv       * aEnv );

stlStatus ellRemoveDict4DropTablePrimeElement( smlTransId        aTransID,
                                               smlStatement    * aStmt,
                                               ellDictHandle   * aTableDictHandle,
                                               ellEnv          * aEnv );

stlStatus ellRemoveDictTablePrimeElement4TableList( smlTransId        aTransID,
                                                    smlStatement    * aStmt,
                                                    ellObjectList   * aTableList,
                                                    ellEnv          * aEnv );

stlStatus ellModifyTablePhysicalID( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    stlInt64       aTableID,
                                    stlInt64       aPhysicalID,
                                    ellEnv       * aEnv );

stlStatus ellModifyTableName( smlTransId     aTransID,
                              smlStatement * aStmt,
                              stlInt64       aTableID,
                              stlChar      * aNewName,
                              ellEnv       * aEnv );

stlStatus ellModifyTableSuppLog( smlTransId     aTransID,
                                 smlStatement * aStmt,
                                 stlInt64       aTableID,
                                 stlBool        aOnOff,
                                 ellEnv       * aEnv );

/*
 * Cache 재구축 함수
 */

stlStatus ellRebuildTableCache( smlTransId        aTransID,
                                smlStatement    * aStmt,
                                ellDictHandle   * aTableDictHandle,
                                ellEnv          * aEnv );

stlStatus ellRefineCache4CreateTable( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      stlInt64             aTableID,
                                      ellEnv             * aEnv );

stlStatus ellRefineCache4DropTablePrimeElement( smlTransId        aTransID,
                                                smlStatement    * aStmt,
                                                ellDictHandle   * aTableDictHandle,
                                                ellEnv          * aEnv );

stlStatus ellRefineCacheDropTablePrime4TableList( smlTransId        aTransID,
                                                  smlStatement    * aStmt,
                                                  ellObjectList   * aTableList,
                                                  ellEnv          * aEnv );

stlStatus ellRefineCache4TruncateTable( smlTransId        aTransID,
                                        smlStatement    * aStmt,
                                        stlInt64          aNewPhysicalID,
                                        ellDictHandle   * aTableDictHandle,
                                        ellEnv          * aEnv );

stlStatus ellRefineCache4AlterPhysicalAttr( smlTransId        aTransID,
                                            ellDictHandle   * aTableDictHandle,
                                            ellEnv          * aEnv );

stlStatus ellRefineCache4RenameTable( smlTransId        aTransID,
                                      smlStatement    * aStmt,
                                      ellDictHandle   * aTableDictHandle,
                                      ellEnv          * aEnv );

stlStatus ellRefineCache4SuppLogTable( smlTransId        aTransID,
                                       smlStatement    * aStmt,
                                       ellDictHandle   * aTableDictHandle,
                                       stlBool           aIsAdd,
                                       ellEnv          * aEnv );

/*
 * Handle 획득 함수 
 */

stlStatus ellGetTableDictHandleWithSchema( smlTransId           aTransID,
                                           smlScn               aViewSCN,
                                           ellDictHandle      * aSchemaDictHandle,
                                           stlChar            * aTableName,
                                           ellDictHandle      * aTableDictHandle,
                                           stlBool            * aExist,
                                           ellEnv             * aEnv );

stlStatus ellGetTableDictHandleWithAuth( smlTransId           aTransID,
                                         smlScn               aViewSCN,
                                         ellDictHandle      * aAuthDictHandle,
                                         stlChar            * aTableName,
                                         ellDictHandle      * aTableDictHandle,
                                         stlBool            * aExist,
                                         ellEnv             * aEnv );

stlStatus ellGetTableDictHandleByID( smlTransId           aTransID,
                                     smlScn               aViewSCN,
                                     stlInt64             aTableID,
                                     ellDictHandle      * aTableDictHandle,
                                     stlBool            * aExist,
                                     ellEnv             * aEnv );

stlStatus ellGetPhyHandle4PendingTransTable( void   ** aPhyHandle,
                                             smlScn    aViewSCN,
                                             ellEnv  * aEnv );


/*
 * 정보 획득 함수
 */

stlInt64        ellGetTableID( ellDictHandle * aTableDictHandle );
stlInt64        ellGetTableOwnerID( ellDictHandle * aTableDictHandle );
stlInt64        ellGetTableSchemaID( ellDictHandle * aTableDictHandle );
stlInt64        ellGetTableTablespaceID( ellDictHandle * aTableDictHandle );
stlChar       * ellGetTableName( ellDictHandle * aTableDictHandle );
ellTableType    ellGetTableType( ellDictHandle * aTableDictHandle );
void          * ellGetTableHandle( ellDictHandle * aTableDictHandle );
stlInt32        ellGetTableColumnCnt( ellDictHandle * aTableDictHandle );
stlInt32        ellGetTableUsedColumnCnt( ellDictHandle * aTableDictHandle );
ellDictHandle * ellGetTableColumnDictHandle( ellDictHandle * aTableDictHandle );
stlBool         ellIsBuiltInTable( ellDictHandle * aTableDictHandle );
stlBool         ellIsUpdatableTable( ellDictHandle * aTableDictHandle );
stlBool         ellIsTableSuppLogPK( ellDictHandle * aTableDictHandle );
stlBool         ellGetTableNeedSupplePK( ellDictHandle * aTableDictHandle,
                                         ellEnv        * aEnv );
stlInt64        ellGetTablePhysicalID( ellDictHandle * aTableDictHandle );

/** @} ellObjectTable */



#endif /* _ELL_DICT_TABLE_H_ */
