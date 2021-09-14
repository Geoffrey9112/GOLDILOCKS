/*******************************************************************************
 * eldTable.h
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


#ifndef _ELD_TABLE_H_
#define _ELD_TABLE_H_ 1

/**
 * @file eldTable.h
 * @brief Table Dictionary 관리 루틴 
 */


/**
 * @defgroup eldTable Table Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

/*
 * Dictionary Row 변경 함수 
 */

stlStatus eldInsertTableDesc( smlTransId     aTransID,
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

stlStatus eldGetNewTableID( stlInt64 * aTableID, ellEnv * aEnv );

stlStatus eldMakeValueList4TableDesc( knlValueBlockList  * aDataValueList,
                                      stlInt64             aOwnerID,
                                      stlInt64             aSchemaID,
                                      stlInt64             aTableID,
                                      stlInt64             aTablespaceID,
                                      stlInt64             aPhysicalID,
                                      stlChar            * aTableName,
                                      ellTableType         aTableType,
                                      stlChar            * aTableComment,
                                      ellEnv             * aEnv );

stlStatus eldDeleteDict4DropTablePrimeElement( smlTransId       aTransID,
                                               smlStatement   * aStmt,
                                               ellDictHandle  * aTableDictHandle,
                                               ellEnv         * aEnv );

/*
 * Dictionary Cache 변경 함수 
 */

stlStatus eldRemoveTablePrimeElementCache( smlTransId       aTransID,
                                           ellDictHandle  * aTableDictHandle,
                                           ellEnv         * aEnv );

stlStatus eldRemoveTableAllCache( smlTransId       aTransID,
                                  smlStatement   * aStmt,
                                  ellDictHandle  * aTableDictHandle,
                                  ellEnv         * aEnv );

stlStatus eldBuildTableCache( smlTransId       aTransID,
                              smlStatement   * aStmt,
                              stlInt64         aTableID,
                              ellEnv         * aEnv );

/*
 * Handle 획득 함수 
 */


stlStatus eldGetTableHandleByID( smlTransId           aTransID,
                                 smlScn               aViewSCN,
                                 stlInt64             aTableID,
                                 ellDictHandle      * aTableDictHandle,
                                 stlBool            * aExist,
                                 ellEnv             * aEnv );

stlStatus eldGetTableHandleByName( smlTransId           aTransID,
                                   smlScn               aViewSCN,
                                   stlInt64             aSchemaID,
                                   stlChar            * aTableName,
                                   ellDictHandle      * aTableDictHandle,
                                   stlBool            * aExist,
                                   ellEnv             * aEnv );

stlStatus eldGetTableRelatedDictHandle( smlTransId               aTransID,
                                        smlScn                   aViewSCN,
                                        knlRegionMem           * aRegionMem,
                                        ellDictHandle          * aTableDictHandle,
                                        ellTableRelatedObject    aRelatedType,
                                        stlInt32               * aRelatedCnt,
                                        ellDictHandle         ** aRelatedObjectHandle,
                                        ellEnv                 * aEnv );

/*
 * Lock 제어 
 */

stlStatus eldLockTable4DDL( smlTransId       aTransID,
                            smlStatement   * aStmt,
                            stlInt32         aLockMode,
                            ellDictHandle  * aTableDictHandle,
                            stlBool        * aLockSuccess,
                            ellEnv         * aEnv );

/*
 * DDL Supplemental Logging
 */

stlStatus eldWriteSuppLogTableDDL( smlTransId           aTransID,
                                   stlInt64             aTableID,
                                   ellSuppLogTableDDL   aStmtType,
                                   ellEnv             * aEnv );

stlStatus eldWriteSuppLogTable4Truncate( smlTransId           aTransID,
                                         stlInt64             aTableID,
                                         stlInt64             aNewPhysicalID,
                                         ellSuppLogTableDDL   aStmtType,
                                         ellEnv             * aEnv );

/** @} eldTable */

#endif /* _ELD_TABLE_H_ */
