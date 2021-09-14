/*******************************************************************************
 * ellDictColumn.h
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


#ifndef _ELL_DICT_COLUMN_H_
#define _ELL_DICT_COLUMN_H_ 1

/**
 * @file ellDictColumn.h
 * @brief Column Dictionary Management
 */

/**
 * @addtogroup ellColumn
 * @{
 */

/*
 * DDL behavior 관련 Object 획득 함수
 */

stlStatus ellGetObjectList4DropColumn( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       ellObjectList  * aColumnList,
                                       knlRegionMem   * aRegionMem,
                                       ellObjectList ** aUniqueKeyList,
                                       ellObjectList ** aForeignKeyList,
                                       ellObjectList ** aChildForeignKeyList,
                                       ellObjectList ** aCheckConstList,
                                       ellObjectList ** aNonKeyIndexList,
                                       ellObjectList ** aSetNullColumnList,
                                       ellEnv         * aEnv );

stlBool ellIsColumnInsideNotNullList( ellDictHandle    * aColumnHandle,
                                      ellObjectList    * aNotNullList,
                                      ellEnv           * aEnv );


stlStatus ellGetObjectList4RenameColumn( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         ellDictHandle     * aColumnDictHandle,
                                         knlRegionMem      * aRegionMem,
                                         ellObjectList    ** aCheckList,
                                         ellEnv            * aEnv );

stlStatus ellGetIndexListByColumn( smlTransId       aTransID,
                                   smlStatement   * aStmt,
                                   ellDictHandle  * aColumnHandle,
                                   knlRegionMem   * aRegionMem,
                                   ellObjectList ** aIndexList,
                                   ellEnv         * aEnv );

/*
 * Column 에 대한 DDL Lock 함수 
 */

stlStatus ellLock4AddColumn( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellDictHandle * aTableDictHandle,
                             ellObjectList * aConstSchemaList,
                             ellObjectList * aConstSpaceList,
                             stlBool       * aLockSuccess,
                             ellEnv        * aEnv );

stlStatus ellLock4DropColumn( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              ellDictHandle * aTableDictHandle,
                              stlBool       * aLockSuccess,
                              ellEnv        * aEnv );

stlStatus ellLock4AlterColumn( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               ellDictHandle * aTableDictHandle,
                               stlBool       * aLockSuccess,
                               ellEnv        * aEnv );

stlStatus ellLock4RenameColumn( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                ellDictHandle * aTableDictHandle,
                                stlBool       * aLockSuccess,
                                ellEnv        * aEnv );

/*
 * Column 관련 객체에 대한 DDL Lock 함수 
 */


/*
 * DDL 관련 Index 획득 함수
 */



/*
 * Dictionary 변경 함수 
 */

stlStatus ellInsertColumnDesc( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               stlInt64             aOwnerID,
                               stlInt64             aSchemaID,
                               stlInt64             aTableID,
                               ellTableType         aTableType,
                               stlInt64           * aColumnID,
                               stlChar            * aColumnName,
                               stlInt32             aPhysicalOrdinalPosition,
                               stlInt32             aLogicalOrdinalPosition,
                               stlChar            * aColumnDefault,
                               stlBool              aIsNullable,
                               stlBool              aIsIdentity,
                               ellIdentityGenOption aIdentityGenOption,
                               stlInt64             aIdentityStart,
                               stlInt64             aIdentityIncrement,
                               stlInt64             aIdentityMaximum,
                               stlInt64             aIdentityMinimum,
                               stlBool              aIdentityCycle,
                               stlInt64             aIdentityTablespaceID,
                               stlInt64             aIdentityPhysicalID,
                               stlInt64             aIdentityCacheSize,
                               stlBool              aIsUpdatable,
                               stlChar            * aColumnComment,
                               ellEnv             * aEnv );

stlStatus ellModifyColumnName( smlTransId     aTransID,
                               smlStatement * aStmt,
                               stlInt64       aColumnID,
                               stlChar      * aNewName,
                               ellEnv       * aEnv );

stlStatus ellModifyColumnDefault( smlTransId     aTransID,
                                  smlStatement * aStmt,
                                  stlInt64       aColumnID,
                                  stlChar      * aDefaultString,
                                  ellEnv       * aEnv );

stlStatus ellModifyColumnNullable( smlTransId     aTransID,
                                   smlStatement * aStmt,
                                   stlInt64       aColumnID,
                                   stlBool        aIsNullable,
                                   ellEnv       * aEnv );

stlStatus ellModifyColumnSetUnused( smlTransId        aTransID,
                                    smlStatement    * aStmt,
                                    ellDictHandle   * aColumnHandle,
                                    ellEnv          * aEnv );

stlStatus ellModifyColumnLogicalIdx( smlTransId        aTransID,
                                     smlStatement    * aStmt,
                                     ellDictHandle   * aColumnHandle,
                                     stlInt32          aLogicalOrdinalPosition,
                                     stlBool           aSetNullable,
                                     ellEnv          * aEnv );

stlStatus ellModifyColumnDropIdentity( smlTransId     aTransID,
                                       smlStatement * aStmt,
                                       stlInt64       aColumnID,
                                       stlBool        aIsNullable,
                                       ellEnv       * aEnv );

stlStatus ellModifyColumnAlterIdentity( smlTransId             aTransID,
                                        smlStatement         * aStmt,
                                        stlInt64               aColumnID,
                                        ellIdentityGenOption * aIdentityGenOption,
                                        stlInt64             * aIdentityStart,
                                        stlInt64             * aIdentityIncrement,
                                        stlInt64             * aIdentityMaximum,
                                        stlInt64             * aIdentityMinimum,
                                        stlBool              * aIdentityCycle,
                                        stlInt64             * aIdentityPhysicalID,
                                        stlInt64             * aIdentityCacheSize,
                                        ellEnv               * aEnv );

/*
 * Cache 재구축 함수
 */


stlStatus ellRefineCache4AddColumn( smlTransId        aTransID,
                                    smlStatement    * aStmt,
                                    ellDictHandle   * aTableDictHandle,
                                    ellEnv          * aEnv );

stlStatus ellRefineCache4DropColumn( smlTransId        aTransID,
                                     smlStatement    * aStmt,
                                     ellDictHandle   * aTableDictHandle,
                                     ellEnv          * aEnv );

stlStatus ellRefineCache4AlterColumn( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      ellDictHandle      * aTableDictHandle,
                                      ellSuppLogTableDDL   aAlterColumnSuppLog,
                                      ellEnv             * aEnv );

stlStatus ellRefineCache4RenameColumn( smlTransId        aTransID,
                                       smlStatement    * aStmt,
                                       ellDictHandle   * aTableDictHandle,
                                       ellEnv          * aEnv );


/*
 * Handle 획득 함수 
 */

stlStatus ellGetColumnDictHandle( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  ellDictHandle      * aTableDictHandle,
                                  stlChar            * aColumnName,
                                  ellDictHandle      * aColumnDictHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv );

stlStatus ellGetColumnDictHandleByID( smlTransId           aTransID,
                                      smlScn               aViewSCN,
                                      stlInt64             aColumnID,
                                      ellDictHandle      * aColumnDictHandle,
                                      stlBool            * aExist,
                                      ellEnv             * aEnv );


/*
 * 정보 획득 함수 
 */

stlInt64    ellGetColumnID( ellDictHandle * aColumnDictHandle );
stlInt64    ellGetColumnOwnerID( ellDictHandle * aColumnDictHandle );
stlInt64    ellGetColumnSchemaID( ellDictHandle * aColumnDictHandle );
stlInt64    ellGetColumnTableID( ellDictHandle * aColumnDictHandle );

stlChar   * ellGetColumnName( ellDictHandle * aColumnDictHandle );
stlInt32    ellGetColumnIdx( ellDictHandle * aColumnDictHandle );
stlChar   * ellGetColumnDefaultValue( ellDictHandle * aColumnDictHandle );
stlBool     ellGetColumnNullable( ellDictHandle * aColumnDictHandle );
stlBool     ellGetColumnUpdatable( ellDictHandle * aColumnDictHandle );
stlBool     ellGetColumnUnused( ellDictHandle * aColumnDictHandle );

void      * ellGetColumnIdentityHandle( ellDictHandle * aColumnDictHandle );
stlInt64    ellGetColumnIdentityPhysicalID( ellDictHandle * aColumnDictHandle );
ellIdentityGenOption ellGetColumnIdentityGenOption( ellDictHandle * aColumnDictHandle );
dtlDataType ellGetColumnDBType( ellDictHandle * aColumnDictHandle );
stlInt64    ellGetColumnPrecision( ellDictHandle * aColumnDictHandle );
stlInt64    ellGetColumnScale( ellDictHandle * aColumnDictHandle );
dtlStringLengthUnit  ellGetColumnStringLengthUnit( ellDictHandle * aColumnDictHandle );
dtlIntervalIndicator ellGetColumnIntervalIndicator( ellDictHandle * aColumnDictHandle );
ellColumnDesc * ellGetColumnDesc( ellDictHandle * aColumnDictHandle );
/** @} ellColumn */




#endif /* _ELL_DICT_COLUMN_H_ */
