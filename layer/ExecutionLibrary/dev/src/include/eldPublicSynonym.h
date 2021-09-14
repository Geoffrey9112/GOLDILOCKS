/*******************************************************************************
 * eldPublicSynonym.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ELD_PUBLIC_SYNONYM_H_
#define _ELD_PUBLIC_SYNONYM_H_ 1

/**
 * @file eldPublicSynonym.h
 * @brief Public Synonym Dictionary 관리 루틴 
 */

/**
 * @defgroup eldPublicSynonym Public Synonym Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

/*
 * 정보 생성 함수
 */

stlStatus eldInsertPublicSynonymDesc( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      stlInt64        aSynonymID,
                                      stlChar       * aSynonymName,
                                      stlInt64        aCreatorID,
                                      stlChar       * aObjectSchemaName,
                                      stlChar       * aObjectName,
                                      ellEnv        * aEnv );

stlStatus eldGetNewPublicSynonymID( stlInt64 * aSynonymID,
                                    ellEnv   * aEnv );

stlStatus eldRefineCache4AddPublicSynonym( smlTransId           aTransID,
                                           smlStatement       * aStmt,
                                           stlInt64             aSynonymID,
                                           ellEnv             * aEnv );

/*
 * 정보 삭제 함수
 */

stlStatus eldDeleteDictPublicSynonym( smlTransId       aTransID,
                                      smlStatement   * aStmt,
                                      ellDictHandle  * aSynonymDictHandle,
                                      ellEnv         * aEnv );

/*
 * Handle 획득 함수 
 */

stlStatus eldGetPublicSynonymHandleByName( smlTransId           aTransID,
                                           smlScn               aViewSCN,
                                           stlChar            * aSynonymName,
                                           ellDictHandle      * aSynonymDictHandle,
                                           stlBool            * aExist,
                                           ellEnv             * aEnv );


/** @} eldPublicSynonym */

#endif /* _ELD_PUBLIC_SYNONYM_H_ */
