/*******************************************************************************
 * eldcFxTableDesc.h
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


#ifndef _ELDC_FX_TABLE_DESC_H_
#define _ELDC_FX_TABLE_DESC_H_ 1

/**
 * @file eldcFxTableDesc.h
 * @brief Cache for Fixed Table descriptor
 */


/**
 * @defgroup eldcFxTableDesc Cache for Fixed Table descriptor
 * @ingroup eldcObject
 * @{
 */

/*
 * 초기화, 마무리 함수 
 */

stlStatus eldcBuildCacheFxTable( smlTransId       aTransID,
                                 smlStatement   * aStmt,
                                 ellEnv         * aEnv );


stlStatus eldcInsertCacheFxTable( smlTransId          aTransID,
                                  smlStatement      * aStmt,
                                  knlFxColumnDesc   * aFixedColumnDesc,
                                  knlValueBlockList * aValueArray,
                                  ellEnv            * aEnv );


/*
 * 조회 함수 
 */


/*
 * 멤버 함수 
 */

stlStatus eldcMakeFxTableDesc( smlTransId          aTransID,
                               smlStatement      * aStmt,
                               knlFxColumnDesc   * aFixedColumnDesc,
                               knlValueBlockList * aValueArray,
                               ellTableDesc     ** aTableDesc,
                               ellEnv            * aEnv );

stlStatus eldcMakeHashElementFxTableID( eldcHashElement   ** aHashElement,
                                        smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        ellTableDesc       * aTableDesc,
                                        ellEnv             * aEnv );

stlStatus eldcMakeHashElementFxTableName( eldcHashElement   ** aFxHashElement,
                                          smlTransId           aTransID,
                                          ellTableDesc       * aTableDesc,
                                          eldcHashElement    * aHashElementID,
                                          ellEnv             * aEnv );

stlStatus eldcGetFxColumnCount( smlTransId           aTransID,
                                smlStatement       * aStmt,
                                stlInt64             aTableID,
                                stlInt32           * aColumnCnt,
                                ellEnv             * aEnv );

// stlStatus eldcMakeFxFilter( dtlConditionInfo   ** aConditionInfo,
//                             knlValueBlockList   * aValueList,
//                             dtlComparisonOper     aCompOP,
//                             void                * aTableID,
//                             ellEnv              * aEnv );

stlStatus eldcSetFxColumnDictHandle( smlTransId           aTransID,
                                     smlStatement       * aStmt,
                                     ellTableDesc       * aTableDesc,
                                     ellEnv             * aEnv );
    
/** @} eldcFxTableDesc */


#endif /* _ELDC_FX_TABLE_DESC_H_ */
