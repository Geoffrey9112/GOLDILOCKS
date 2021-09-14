/*******************************************************************************
 * eldColumn.h
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


#ifndef _ELD_COLUMN_H_
#define _ELD_COLUMN_H_ 1

/**
 * @file eldColumn.h
 * @brief Column Dictionary 관리 루틴
 */


/**
 * @defgroup eldColumn Column Descriptor 관리 루틴
 * @internal 
 * @ingroup eldDesc
 * @{
 */

/*
 * Dictionary Row 변경 함수 
 */

stlStatus eldInsertColumnDesc( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               stlInt64             aOwnerID,
                               stlInt64             aSchemaID,
                               stlInt64             aTableID,
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

stlStatus eldModifyColumnSetUnused( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    stlInt64       aColumnID,
                                    ellEnv       * aEnv );

stlStatus eldSetColumnUnused( knlValueBlockList        * aTableValueList,
                              ellEnv                   * aEnv );

stlStatus eldModifyColumnLogicalIdx( smlTransId       aTransID,
                                     smlStatement   * aStmt,
                                     stlInt64         aColumnID,
                                     stlInt32         aLogicalIdx,
                                     stlBool          aSetNullable,
                                     ellEnv         * aEnv );

stlStatus eldDeleteDictColumn( smlTransId       aTransID,
                               smlStatement   * aStmt,
                               ellDictHandle  * aColumnDictHandle,
                               ellEnv         * aEnv );

stlStatus eldRemoveColumnIdentity( smlTransId       aTransID,
                                   smlStatement   * aStmt,
                                   stlInt64         aColumnID,
                                   stlBool          aIsNullable,
                                   ellEnv         * aEnv );

stlStatus eldSetColumnNonIdentity( knlValueBlockList        * aTableValueList,
                                   stlBool                    aIsNullable,
                                   ellEnv                   * aEnv );

stlStatus eldModifyColumnIdentity( smlTransId             aTransID,
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

stlStatus eldSetColumnIdentityOption( knlValueBlockList        * aTableValueList,
                                      ellIdentityGenOption     * aIdentityGenOption,
                                      stlInt64                 * aIdentityStart,
                                      stlInt64                 * aIdentityIncrement,
                                      stlInt64                 * aIdentityMaximum,
                                      stlInt64                 * aIdentityMinimum,
                                      stlBool                  * aIdentityCycle,
                                      stlInt64                 * aIdentityPhysicalID,
                                      stlInt64                 * aIdentityCacheSize,
                                      ellEnv                   * aEnv );

stlStatus eldFinishSetValueIdentityColumn( ellEnv * aEnv );


/*
 * Handle 획득 함수 
 */

stlStatus eldGetColumnHandleByID( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  stlInt64             aColumnID,
                                  ellDictHandle      * aColumnDictHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv );

stlStatus eldGetColumnHandleByName( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aTableID,
                                    stlChar            * aColumnName,
                                    ellDictHandle      * aColumnDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv );


/** @} eldColumn */

#endif /* _ELD_COLUMN_H_ */
