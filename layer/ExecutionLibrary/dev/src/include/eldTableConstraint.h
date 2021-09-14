/*******************************************************************************
 * eldTableConstraint.h
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


#ifndef _ELD_TABLE_CONSTRAINT_H_
#define _ELD_TABLE_CONSTRAINT_H_ 1

/**
 * @file eldTableConstraint.h
 * @brief Check Constraint Dictionary 관리 루틴 
 */


/**
 * @defgroup eldTableConstraint Table Constraint Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

/*
 * 정보 생성 함수
 */

stlStatus eldInsertTableConstraintDesc( smlTransId               aTransID,
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

                              
/*
 * 정보 삭제 함수
 */
                              

stlStatus eldDeleteDictConst( smlTransId       aTransID,
                              smlStatement   * aStmt,
                              ellDictHandle  * aConstHandle,
                              ellEnv         * aEnv );


/*
 * 정보 갱신 함수
 */

stlStatus eldModifyDictConstFeature( smlTransId       aTransID,
                                     smlStatement   * aStmt,
                                     ellDictHandle  * aConstHandle,
                                     stlBool          aIsDeferrable,
                                     stlBool          aInitiallyDeferred,
                                     stlBool          aEnforced,
                                     ellEnv         * aEnv );

/*
 * Handle 획득 함수 
 */

stlStatus eldGetConstHandleByID( smlTransId           aTransID,
                                 smlScn               aViewSCN,
                                 stlInt64             aConstID,
                                 ellDictHandle      * aConstDictHandle,
                                 stlBool            * aExist,
                                 ellEnv             * aEnv );

stlStatus eldGetConstHandleByName( smlTransId           aTransID,
                                   smlScn               aViewSCN,
                                   stlInt64             aSchemaID,
                                   stlChar            * aConstName,
                                   ellDictHandle      * aConstDictHandle,
                                   stlBool            * aExist,
                                   ellEnv             * aEnv );

/** @} eldTableConstraint */



#endif /* _ELD_TABLE_CONSTRAINT_H_ */
