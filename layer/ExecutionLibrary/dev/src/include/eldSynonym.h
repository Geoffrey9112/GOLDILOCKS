/*******************************************************************************
 * eldSynonym.h
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

#ifndef _ELD_SYNONYM_H_
#define _ELD_SYNONYM_H_ 1

/**
 * @file eldSynonym.h
 * @brief Synonym Dictionary 관리 루틴 
 */

/**
 * @defgroup eldSynonym Synonym Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

/*
 * 정보 생성 함수
 */

stlStatus eldInsertSynonymDesc( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                stlInt64        aOwnerID,
                                stlInt64        aSchemaID,
                                stlInt64        aSynonymID,
                                stlInt64        aSynonymTableID,
                                stlChar       * aSynonymName,
                                stlChar       * aObjectSchemaName,
                                stlChar       * aObjectName,
                                ellEnv        * aEnv );

stlStatus eldGetNewSynonymID( stlInt64 * aSynonymID,
                              ellEnv   * aEnv );


stlStatus eldRefineCache4AddSynonym( smlTransId     aTransID,
                                     smlStatement * aStmt,
                                     stlInt64       aSynonymID,
                                     ellEnv       * aEnv );

/*
 * 정보 삭제 함수
 */

stlStatus eldDeleteDictSynonym4Drop( smlTransId       aTransID,
                                     smlStatement   * aStmt,
                                     ellDictHandle  * aSynonymDictHandle,
                                     ellEnv         * aEnv );

stlStatus eldDeleteDictSynonym4Replace( smlTransId       aTransID,
                                        smlStatement   * aStmt,
                                        ellDictHandle  * aSynonymDictHandle,
                                        ellEnv         * aEnv );

/*
 * Handle 획득 함수 
 */

stlStatus eldGetSynonymHandleByName( smlTransId      aTransID,
                                     smlScn          aViewSCN,
                                     stlInt64        aSchemaID,
                                     stlChar       * aSynonymName,
                                     ellDictHandle * aSynonymDictHandle,
                                     stlBool       * aExist,
                                     ellEnv        * aEnv );


/** @} eldSynonym */

#endif /* _ELD_SYNONYM_H_ */
