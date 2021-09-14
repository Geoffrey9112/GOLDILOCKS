/*******************************************************************************
 * eldIntegrity.h
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


#ifndef _ELD_INTEGRITY_H_
#define _ELD_INTEGRITY_H_ 1

/**
 * @file eldIntegrity.h
 * @brief Dictionary Table 의 Integrity Check Routine
 */


/**
 * @defgroup eldIntegrity Dictionary 테이블의 무결성 검증 
 * @ingroup eldDict
 * @{
 */

stlStatus eldFinishCheckAllConstraint( ellEnv * aEnv );

stlStatus eldFinishCheckTableIntegrity( smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        ellDictHandle      * aTableDictHandle,
                                        ellEnv             * aEnv );

stlStatus eldRefineIntegrityRowInsert( smlTransId              aTransID,
                                       smlStatement          * aStmt,
                                       eldtDefinitionTableID   aDictTableID,
                                       smlRowBlock           * aRowBlock,
                                       knlValueBlockList     * aInsertValueList,
                                       ellEnv                * aEnv );

stlStatus eldRefineIntegrityRowDelete( smlTransId              aTransID,
                                       smlStatement          * aStmt,
                                       stlBool                 aForUpdate,
                                       eldtDefinitionTableID   aDictTableID,
                                       smlRowBlock           * aRowBlock,
                                       knlValueBlockList     * aDeleteValueList,
                                       ellEnv                * aEnv );

stlStatus eldCheckRowPrimaryNotNull( ellDictHandle     * aTableDictHandle,
                                     ellDictHandle     * aPrimaryKeyHandle,
                                     knlValueBlockList * aTableValueList,
                                     ellEnv            * aEnv );

stlStatus eldCheckRowCheckNotNull( ellDictHandle     * aTableDictHandle,
                                   ellDictHandle     * aNotNullHandle,
                                   knlValueBlockList * aTableValueList,
                                   ellEnv            * aEnv );

stlStatus eldCheckIndexKeyExist( smlTransId              aTransID,
                                 smlStatement          * aStmt,
                                 eldtDefinitionTableID   aDictTableID,
                                 ellDictHandle         * aIndexDictHandle,
                                 knlPredicateList      * aIndexRange,
                                 knlExprContextInfo    * aIndexRangeContext,
                                 knlValueBlockList     * aIndexValueList,
                                 stlBool               * aKeyExist,
                                 ellEnv                * aEnv );

stlStatus eldMakeParentRange( ellDictHandle        * aChildTableHandle,
                              ellDictHandle        * aChildForeignHandle,
                              knlValueBlockList    * aChildTableValueList,
                              knlRegionMem         * aRegionMem,
                              knlPredicateList    ** aParentKeyRange,
                              knlExprContextInfo  ** aParentContextInfo,
                              knlValueBlockList   ** aParentIndexValueList,
                              ellEnv               * aEnv );

stlStatus eldMakeChildRange( ellDictHandle        * aParentTableHandle,
                             ellDictHandle        * aChildForeignHandle,
                             knlValueBlockList    * aParentTableValueList,
                             knlRegionMem         * aRegionMem,
                             knlPredicateList    ** aChildKeyRange,
                             knlExprContextInfo  ** aChildContextInfo,
                             knlValueBlockList   ** aChildIndexValueList,
                             ellEnv               * aEnv );

/** @} eldIntegrity */



#endif /* _ELD_INTEGRITY_H_ */
