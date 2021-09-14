/*******************************************************************************
 * eldIndex.h
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


#ifndef _ELD_INDEX_H_
#define _ELD_INDEX_H_ 1

/**
 * @file eldIndex.h
 * @brief Index Dictionary 관리 루틴 
 */


/**
 * @defgroup eldIndex Index Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

/*
 * 정보 생성 함수
 */

stlStatus eldInsertIndexDesc( smlTransId     aTransID,
                              smlStatement * aStmt,
                              stlInt64       aOwnerID,
                              stlInt64       aSchemaID,
                              stlInt64     * aIndexID,
                              stlInt64       aTablespaceID,
                              stlInt64       aPhysicalID,
                              stlChar      * aIndexName,
                              ellIndexType   aIndexType,
                              stlBool        aUnique,
                              stlBool        aInvalid,
                              stlBool        aByConstraint,
                              stlChar      * aIndexComment,
                              ellEnv       * aEnv );

stlStatus eldFinishBuildAllIndexData( ellEnv * aEnv );

stlStatus eldFinishBuildOneIndexData( smlStatement                    * aStmt,
                                      eldtDefinitionPhysicalIndexInfo * aIndexInfo,
                                      ellEnv                          * aEnv );

stlStatus eldRefineCache4AddIndex( smlTransId           aTransID,
                                   smlStatement       * aStmt,
                                   stlInt64             aAuthID,
                                   stlInt64             aIndexID,
                                   ellEnv             * aEnv );


/*
 * 정보 삭제 함수
 */

stlStatus eldDeleteDictIndex( smlTransId       aTransID,
                              smlStatement   * aStmt,
                              ellDictHandle  * aIndexDictHandle,
                              ellEnv         * aEnv );







/*
 * Handle 획득 함수 
 */


stlStatus eldGetIndexHandleByID( smlTransId           aTransID,
                                 smlScn               aViewSCN,
                                 stlInt64             aIndexID,
                                 ellDictHandle      * aIndexDictHandle,
                                 stlBool            * aExist,
                                 ellEnv             * aEnv );

stlStatus eldGetIndexHandleByName( smlTransId           aTransID,
                                   smlScn               aViewSCN,
                                   stlInt64             aSchemaID,
                                   stlChar            * aIndexName,
                                   ellDictHandle      * aIndexDictHandle,
                                   stlBool            * aExist,
                                   ellEnv             * aEnv );


/** @} eldIndex */

#endif /* _ELD_INDEX_H_ */
