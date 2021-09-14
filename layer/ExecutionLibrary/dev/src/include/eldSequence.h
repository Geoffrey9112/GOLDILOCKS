/*******************************************************************************
 * eldSequence.h
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


#ifndef _ELD_SEQUENCE_H_
#define _ELD_SEQUENCE_H_ 1

/**
 * @file eldSequence.h
 * @brief Sequence Dictionary 관리 루틴
 */


/**
 * @defgroup eldSequence Sequence Descriptor 관리 루틴
 * @internal 
 * @ingroup eldDesc
 * @{
 */

/*
 * 정보 생성 함수
 */

stlStatus eldInsertSequenceDesc( smlTransId           aTransID,
                                 smlStatement       * aStmt,
                                 stlInt64             aOwnerID,
                                 stlInt64             aSchemaID,
                                 stlInt64           * aSequenceID,
                                 stlInt64             aSeqTableID,
                                 stlInt64             aTablespaceID,
                                 stlInt64             aPhysicalID,
                                 stlChar            * aSequenceName,
                                 stlInt64             aStart,
                                 stlInt64             aMinimum,
                                 stlInt64             aMaximum,
                                 stlInt64             aIncrement,
                                 stlBool              aIsCycle,
                                 stlInt32             aCacheSize,
                                 stlChar            * aComment,
                                 ellEnv             * aEnv );

stlStatus eldRefineCache4AddSequence( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      stlInt64             aAuthID,
                                      stlInt64             aSequenceID,
                                      ellEnv             * aEnv );

/*
 * 정보 삭제 함수
 */

stlStatus eldDeleteDictSequence( smlTransId       aTransID,
                                 smlStatement   * aStmt,
                                 ellDictHandle  * aSeqDictHandle,
                                 ellEnv         * aEnv );

/*
 * 정보 변경 함수
 */

stlStatus eldModifySequenceOption( smlTransId       aTransID,
                                   smlStatement   * aStmt,
                                   stlInt64         aSequenceID,
                                   stlInt64         aPhysicalID,
                                   stlInt64         aStart,
                                   stlInt64         aMinimum,
                                   stlInt64         aMaximum,
                                   stlInt64         aIncrement,
                                   stlBool          aIsCycle,
                                   stlInt32         aCacheSize,
                                   ellEnv         * aEnv );

stlStatus eldSetSequenceOption( knlValueBlockList        * aDataValueList,
                                stlInt64                   aPhysicalID,
                                stlInt64                   aStart,
                                stlInt64                   aMinimum,
                                stlInt64                   aMaximum,
                                stlInt64                   aIncrement,
                                stlBool                    aIsCycle,
                                stlInt32                   aCacheSize,
                                ellEnv                   * aEnv );

/*
 * Handle 획득 함수 
 */

stlStatus eldGetSequenceHandleByID( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aSequenceID,
                                    ellDictHandle      * aSequenceDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv );

stlStatus eldGetSequenceHandleByName( smlTransId           aTransID,
                                      smlScn               aViewSCN,
                                      stlInt64             aSchemaID,
                                      stlChar            * aSequenceName,
                                      ellDictHandle      * aSequenceDictHandle,
                                      stlBool            * aExist,
                                      ellEnv             * aEnv );

/** @} eldSequence */

#endif /* _ELD_SEQUENCE_H_ */
