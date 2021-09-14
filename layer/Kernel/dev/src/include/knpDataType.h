/*******************************************************************************
 * knpDataType.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *        Property 관련 Data Type 정의 및 함수 원형
 *
 ******************************************************************************/

#ifndef _KNP_DATATYPE_H_
#define _KNP_DATATYPE_H_ 1

/**
 * @file knpDataType.h
 * @brief Kernel Layer 의 Property Data를 처리하기 위한 내부 처리 루틴
 */


/**
 * @defgroup knp Property DataType 별 내부 처리 루틴
 * @ingroup KNP
 * @internal
 * @{
 */

stlStatus knpVarcharVerifyValue( knpPropertyContent     * aContent, 
                                 void                   * aValue,
                                 knlEnv                 * aKnlEnv );

stlStatus knpBooleanVerifyValue( knpPropertyContent     * aContent, 
                                 void                   * aValue,
                                 knlEnv                 * aKnlEnv );

stlStatus knpBigIntVerifyValue( knpPropertyContent     * aContent, 
                                void                   * aValue,
                                knlEnv                 * aKnlEnv );

stlStatus knpVarcharConvertFromString( stlChar   * aValue,
                                       void     ** aResult,
                                       knlEnv    * aKnlEnv );

stlStatus knpBigIntConvertFromString( stlChar   * aValue,
                                      void     ** aResult,
                                      knlEnv    * aKnlEnv );

stlStatus knpBooleanConvertFromString( stlChar   * aValue,
                                       void     ** aResult,
                                       knlEnv    * aKnlEnv );


/** @} */

#endif /* _KNP_DATATYPE_H_ */
