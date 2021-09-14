/*******************************************************************************
 * dtfNumeric.h
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


#ifndef _DTF_NUMERIC_H_
#define _DTF_NUMERIC_H_ 1

/**
 * @file dtfNumeric.h
 * @brief Numeric for DataType Layer
 */

/**
 * @addtogroup dtfNumeric Numeric
 * @ingroup dtf
 * @internal
 * @{
 */

stlStatus dtfNumericAddition( dtlDataValue        * aValue1,
                              dtlDataValue        * aValue2,
                              dtlDataValue        * aResult,
                              stlErrorStack       * aErrorStack );

stlStatus dtfNumericSum( dtlDataValue   * aSumValue,
                         dtlDataValue   * aSrcValue,
                         stlErrorStack  * aErrorStack );

stlStatus dtfNumericSumPosPos( dtlDataValue   * aSumValue,
                               dtlDataValue   * aSrcValue,
                               stlErrorStack  * aErrorStack );

stlStatus dtfNumericSumNegaNega( dtlDataValue   * aSumValue,
                                 dtlDataValue   * aSrcValue,
                                 stlErrorStack  * aErrorStack );

stlStatus dtfNumericSubtraction( dtlDataValue        * aValue1,
                                 dtlDataValue        * aValue2,
                                 dtlDataValue        * aResult,
                                 stlErrorStack       * aErrorStack );

stlStatus dtfNumericMultiplication( dtlDataValue        * aValue1,
                                    dtlDataValue        * aValue2,
                                    dtlDataValue        * aResult,
                                    stlErrorStack       * aErrorStack );

stlStatus dtfNumericDivision( dtlDataValue        * aValue1,
                              dtlDataValue        * aValue2,
                              dtlDataValue        * aResult,
                              stlErrorStack       * aErrorStack );

stlStatus dtfNumericRounding( dtlDataValue        * aValue1,
                              stlInt32              aScale,
                              dtlDataValue        * aResult,
                              stlErrorStack       * aErrorStack );

/** @} */
    
#endif /* _DTF_NUMERIC_H_ */
