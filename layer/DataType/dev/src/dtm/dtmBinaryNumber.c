/*******************************************************************************
 * dtmBinaryNumber.c
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

/**
 * @file dtmBinaryNumber.c
 * @brief DataType Layer Binary Integer Type과 Binary Real Type간의 비교 연산
 */

#include <dtl.h>


/**
 * @defgroup dtlBinaryNumberType Binary Integer Type과 Binary Real Type간의 비교 연산
 * <BR>
 * <H2> Binary Number Types </H2> 
 * <BR> - SmallInt / Integer / BigInt 와 Real / Double 간의 비교 연산 지원
 * <BR>
 * 
 * @ingroup dtlOperCompareType
 * @{
 *
 * @} dtlBinaryNumberType */

/*******************************************************************************
 * TYPE : IS EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryNumberType
 * <H2> Binary Number Type : IS EQUAL (Binary Integer -> Binary Real) </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to  </td>
 *     <th> DTL_TYPE_REAL           </th>
 *     <th> DTL_TYPE_DOUBLE         </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT              </th>
 *     <td> dtlOperIsEqualSmallIntToReal   </td>
 *     <td> dtlOperIsEqualSmallIntToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER              </th>
 *     <td> dtlOperIsEqualIntegerToReal   </td>
 *     <td> dtlOperIsEqualIntegerToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT              </th>
 *     <td> dtlOperIsEqualBigIntToReal   </td>
 *     <td> dtlOperIsEqualBigIntToDouble </td>
 *   </tr>
 * </table>
 * <BR>
 * <H2> Binary Number Type : IS EQUAL (Binary Real -> Binary Integer) </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to  </td>
 *     <th> DTL_TYPE_SMALLINT       </th>
 *     <th> DTL_TYPE_INTEGER        </th>
 *     <th> DTL_TYPE_BIGINT         </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_REAL                 </th>
 *     <td> dtlOperIsEqualRealToSmallInt  </td>
 *     <td> dtlOperIsEqualRealToInteger   </td>
 *     <td> dtlOperIsEqualRealToBigInt    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DOUBLE                 </th>
 *     <td> dtlOperIsEqualDoubleToSmallInt  </td>
 *     <td> dtlOperIsEqualDoubleToInteger   </td>
 *     <td> dtlOperIsEqualDoubleToBigInt    </td>
 *   </tr>
 * </table>
 * <BR>
 * <BR>
 * @{
 */

/**
 * @brief SmallInt vs Real의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualSmallIntToReal( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs Double의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualSmallIntToDouble( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Integer vs Real의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualIntegerToReal( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Integer vs Double의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualIntegerToDouble( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Real의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualBigIntToReal( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Double의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualBigIntToDouble( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Real vs SmallInt의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualRealToSmallInt( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Real vs Integer의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualRealToInteger( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Real vs BigInt의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualRealToBigInt( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Double vs SmallInt의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualDoubleToSmallInt( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Double vs Integer의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualDoubleToInteger( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Double vs BigInt의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualDoubleToBigInt( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/** @} dtlBinaryNumberType */

/*******************************************************************************
 * TYPE : IS NOT EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryNumberType
 * <H2> Binary Number Type : IS NOT EQUAL (Binary Integer -> Binary Real) </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to  </td>
 *     <th> DTL_TYPE_REAL           </th>
 *     <th> DTL_TYPE_DOUBLE         </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT              </th>
 *     <td> dtlOperIsNotEqualSmallIntToReal   </td>
 *     <td> dtlOperIsNotEqualSmallIntToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER              </th>
 *     <td> dtlOperIsNotEqualIntegerToReal   </td>
 *     <td> dtlOperIsNotEqualIntegerToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT              </th>
 *     <td> dtlOperIsNotEqualBigIntToReal   </td>
 *     <td> dtlOperIsNotEqualBigIntToDouble </td>
 *   </tr>
 * </table>
 * <BR>
 * <H2> Binary Number Type : IS NOT EQUAL (Binary Real -> Binary Integer) </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to  </td>
 *     <th> DTL_TYPE_SMALLINT       </th>
 *     <th> DTL_TYPE_INTEGER        </th>
 *     <th> DTL_TYPE_BIGINT         </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_REAL                 </th>
 *     <td> dtlOperIsNotEqualRealToSmallInt  </td>
 *     <td> dtlOperIsNotEqualRealToInteger   </td>
 *     <td> dtlOperIsNotEqualRealToBigInt    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DOUBLE                 </th>
 *     <td> dtlOperIsNotEqualDoubleToSmallInt  </td>
 *     <td> dtlOperIsNotEqualDoubleToInteger   </td>
 *     <td> dtlOperIsNotEqualDoubleToBigInt    </td>
 *   </tr>
 *    </table>
 * <BR>
 * <BR>
 * @{
 */

/**
 * @brief SmallInt vs Real의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualSmallIntToReal( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs Double의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualSmallIntToDouble( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Integer vs Real의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualIntegerToReal( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Integer vs Double의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualIntegerToDouble( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Real의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualBigIntToReal( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Double의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualBigIntToDouble( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Real vs SmallInt의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualRealToSmallInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Real vs Integer의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualRealToInteger( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Real vs BigInt의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualRealToBigInt( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Double vs SmallInt의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualDoubleToSmallInt( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Double vs Integer의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualDoubleToInteger( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Double vs BigInt의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualDoubleToBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/** @} dtlBinaryNumberType */

/*******************************************************************************
 * TYPE : IS LESS THAN
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryNumberType
 * <H2> Binary Number Type : IS LESS THAN (Binary Integer -> Binary Real) </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to  </td>
 *     <th> DTL_TYPE_REAL           </th>
 *     <th> DTL_TYPE_DOUBLE         </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT              </th>
 *     <td> dtlOperIsLessThanSmallIntToReal   </td>
 *     <td> dtlOperIsLessThanSmallIntToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER              </th>
 *     <td> dtlOperIsLessThanIntegerToReal   </td>
 *     <td> dtlOperIsLessThanIntegerToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT              </th>
 *     <td> dtlOperIsLessThanBigIntToReal   </td>
 *     <td> dtlOperIsLessThanBigIntToDouble </td>
 *   </tr>
 * </table>
 * <BR>
 * <H2> Binary Number Type : IS LESS THAN (Binary Real -> Binary Integer) </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to  </td>
 *     <th> DTL_TYPE_SMALLINT       </th>
 *     <th> DTL_TYPE_INTEGER        </th>
 *     <th> DTL_TYPE_BIGINT         </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_REAL                 </th>
 *     <td> dtlOperIsLessThanRealToSmallInt  </td>
 *     <td> dtlOperIsLessThanRealToInteger   </td>
 *     <td> dtlOperIsLessThanRealToBigInt    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DOUBLE                 </th>
 *     <td> dtlOperIsLessThanDoubleToSmallInt  </td>
 *     <td> dtlOperIsLessThanDoubleToInteger   </td>
 *     <td> dtlOperIsLessThanDoubleToBigInt    </td>
 *   </tr>
 * </table>
 * <BR>
 * <BR>
 * @{
 */

/**
 * @brief SmallInt vs Real의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanSmallIntToReal( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs Double의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanSmallIntToDouble( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Integer vs Real의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanIntegerToReal( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Integer vs Double의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanIntegerToDouble( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Real의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanBigIntToReal( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Double의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanBigIntToDouble( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Real vs SmallInt의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanRealToSmallInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Real vs Integer의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanRealToInteger( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Real vs BigInt의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanRealToBigInt( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Double vs SmallInt의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanDoubleToSmallInt( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Double vs Integer의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanDoubleToInteger( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Double vs BigInt의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanDoubleToBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/** @} dtlBinaryNumberType */

/*******************************************************************************
 * TYPE : IS LESS THAN EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryNumberType
 * <H2> Binary Number Type : IS LESS THAN EQUAL (Binary Integer -> Binary Real) </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to  </td>
 *     <th> DTL_TYPE_REAL           </th>
 *     <th> DTL_TYPE_DOUBLE         </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT              </th>
 *     <td> dtlOperIsLessThanEqualSmallIntToReal   </td>
 *     <td> dtlOperIsLessThanEqualSmallIntToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER              </th>
 *     <td> dtlOperIsLessThanEqualIntegerToReal   </td>
 *     <td> dtlOperIsLessThanEqualIntegerToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT              </th>
 *     <td> dtlOperIsLessThanEqualBigIntToReal   </td>
 *     <td> dtlOperIsLessThanEqualBigIntToDouble </td>
 *   </tr>
 * </table>
 * <BR>
 * <H2> Binary Number Type : IS LESS THAN EQUAL (Binary Real -> Binary Integer) </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to  </td>
 *     <th> DTL_TYPE_SMALLINT       </th>
 *     <th> DTL_TYPE_INTEGER        </th>
 *     <th> DTL_TYPE_BIGINT         </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_REAL                 </th>
 *     <td> dtlOperIsLessThanEqualRealToSmallInt  </td>
 *     <td> dtlOperIsLessThanEqualRealToInteger   </td>
 *     <td> dtlOperIsLessThanEqualRealToBigInt    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DOUBLE                 </th>
 *     <td> dtlOperIsLessThanEqualDoubleToSmallInt  </td>
 *     <td> dtlOperIsLessThanEqualDoubleToInteger   </td>
 *     <td> dtlOperIsLessThanEqualDoubleToBigInt    </td>
 *   </tr>
 * </table>
 * <BR>
 * <BR>
 * @{
 */

/**
 * @brief SmallInt vs Real의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualSmallIntToReal( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs Double의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualSmallIntToDouble( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs Real의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualIntegerToReal( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs Double의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualIntegerToDouble( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Real의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualBigIntToReal( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Double의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualBigIntToDouble( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Real vs SmallInt의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualRealToSmallInt( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Real vs Integer의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualRealToInteger( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Real vs BigInt의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualRealToBigInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs SmallInt의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualDoubleToSmallInt( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs Integer의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualDoubleToInteger( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs BigInt의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualDoubleToBigInt( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryNumberType */

/*******************************************************************************
 * TYPE : IS GREATER THAN
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryNumberType
 * <H2> Binary Number Type : IS GREATER THAN (Binary Integer -> Binary Real) </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to  </td>
 *     <th> DTL_TYPE_REAL           </th>
 *     <th> DTL_TYPE_DOUBLE         </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT              </th>
 *     <td> dtlOperIsGreaterThanSmallIntToReal   </td>
 *     <td> dtlOperIsGreaterThanSmallIntToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER              </th>
 *     <td> dtlOperIsGreaterThanIntegerToReal   </td>
 *     <td> dtlOperIsGreaterThanIntegerToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT              </th>
 *     <td> dtlOperIsGreaterThanBigIntToReal   </td>
 *     <td> dtlOperIsGreaterThanBigIntToDouble </td>
 *   </tr>
 * </table>
 * <BR>
 * <H2> Binary Number Type : IS GREATER THAN (Binary Real -> Binary Integer) </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to  </td>
 *     <th> DTL_TYPE_SMALLINT       </th>
 *     <th> DTL_TYPE_INTEGER        </th>
 *     <th> DTL_TYPE_BIGINT         </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_REAL                 </th>
 *     <td> dtlOperIsGreaterThanRealToSmallInt  </td>
 *     <td> dtlOperIsGreaterThanRealToInteger   </td>
 *     <td> dtlOperIsGreaterThanRealToBigInt    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DOUBLE                 </th>
 *     <td> dtlOperIsGreaterThanDoubleToSmallInt  </td>
 *     <td> dtlOperIsGreaterThanDoubleToInteger   </td>
 *     <td> dtlOperIsGreaterThanDoubleToBigInt    </td>
 *   </tr>
 * </table>
 * <BR>
 * <BR>
 * @{
 */

/**
 * @brief SmallInt vs Real의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanSmallIntToReal( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs Double의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanSmallIntToDouble( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs Real의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanIntegerToReal( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs Double의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanIntegerToDouble( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
    *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Real의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanBigIntToReal( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Double의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanBigIntToDouble( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Real vs SmallInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanRealToSmallInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Real vs Integer의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanRealToInteger( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Real vs BigInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanRealToBigInt( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs SmallInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanDoubleToSmallInt( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs Integer의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanDoubleToInteger( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs BigInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanDoubleToBigInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryNumberType */

/*******************************************************************************
 * TYPE : IS GREATER THAN EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryNumberType
 * <H2> Binary Number Type : IS GREATER THAN EQUAL (Binary Integer -> Binary Real) </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to  </td>
 *     <th> DTL_TYPE_REAL           </th>
 *     <th> DTL_TYPE_DOUBLE         </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT              </th>
 *     <td> dtlOperIsGreaterThanEqualSmallIntToReal   </td>
 *     <td> dtlOperIsGreaterThanEqualSmallIntToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER              </th>
 *     <td> dtlOperIsGreaterThanEqualIntegerToReal   </td>
 *     <td> dtlOperIsGreaterThanEqualIntegerToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT              </th>
 *     <td> dtlOperIsGreaterThanEqualBigIntToReal   </td>
 *     <td> dtlOperIsGreaterThanEqualBigIntToDouble </td>
 *   </tr>
 * </table>
 * <BR>
 * <H2> Binary Number Type : IS GREATER THAN EQUAL (Binary Real -> Binary Integer) </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to  </td>
 *     <th> DTL_TYPE_SMALLINT       </th>
 *     <th> DTL_TYPE_INTEGER        </th>
 *     <th> DTL_TYPE_BIGINT         </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_REAL                 </th>
 *     <td> dtlOperIsGreaterThanEqualRealToSmallInt  </td>
 *     <td> dtlOperIsGreaterThanEqualRealToInteger   </td>
 *     <td> dtlOperIsGreaterThanEqualRealToBigInt    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DOUBLE                 </th>
 *     <td> dtlOperIsGreaterThanEqualDoubleToSmallInt  </td>
 *     <td> dtlOperIsGreaterThanEqualDoubleToInteger   </td>
 *     <td> dtlOperIsGreaterThanEqualDoubleToBigInt    </td>
 *   </tr>
 * </table>
 * <BR>
 * <BR>
 * @{
 */

/**
 * @brief SmallInt vs Real의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualSmallIntToReal( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs Double의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualSmallIntToDouble( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs Real의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualIntegerToReal( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs Double의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualIntegerToDouble( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Real의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualBigIntToReal( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Double의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualBigIntToDouble( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Real vs SmallInt의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualRealToSmallInt( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Real vs Integer의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualRealToInteger( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Real vs BigInt의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualRealToBigInt( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs SmallInt의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualDoubleToSmallInt( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs Integer의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualDoubleToInteger( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs BigInt의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualDoubleToBigInt( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = 
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}
/** @} dtlBinaryNumberType */


