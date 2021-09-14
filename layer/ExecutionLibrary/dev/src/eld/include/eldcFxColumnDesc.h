/*******************************************************************************
 * eldcFxColumnDesc.h
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


#ifndef _ELDC_FX_COLUMN_DESC_H_
#define _ELDC_FX_COLUMN_DESC_H_ 1

/**
 * @file eldcFxColumnDesc.h
 * @brief Cache for Fixed Column descriptor
 */


/**
 * @defgroup eldcFxColumnDesc Cache for Fixed Column descriptor
 * @ingroup eldcObject
 * @{
 */

/*
 * 초기화, 마무리 함수 
 */

stlStatus eldcBuildCacheFxColumn( smlTransId       aTransID,
                                  smlStatement   * aStmt,
                                  ellEnv         * aEnv );

stlStatus eldcInsertCacheFxColumn( smlTransId          aTransID,
                                   smlStatement      * aStmt,
                                   knlValueBlockList * aValueArray,
                                   ellEnv            * aEnv );

/*
 * 조회 함수 
 */



/*
 * 멤버 함수 
 */

stlStatus eldcMakeFxColumnDesc( ellColumnDesc    ** aColumnDesc,
                                knlValueBlockList * aValueArray,
                                ellEnv            * aEnv );

stlStatus eldcMakeHashElementFxColumnID( eldcHashElement   ** aHashElement,
                                         smlTransId           aTransID,
                                         ellColumnDesc      * aColumnDesc,
                                         ellEnv             * aEnv );

stlStatus eldcMakeHashElementFxColumnName( eldcHashElement   ** aHashElement,
                                           smlTransId           aTransID,
                                           ellColumnDesc      * aColumnDesc,
                                           eldcHashElement    * aHashElementID,
                                           ellEnv             * aEnv );

stlStatus eldcSetFxColumnType( ellColumnDesc * aColumnDesc,
                               stlInt64        aValue1,
                               stlInt64        aValue2,
                               stlInt64        aValue3,
                               ellEnv        * aEnv );

/** @} eldcFxColumnDesc */


#endif /* _ELDC_FX_COLUMN_DESC_H_ */
