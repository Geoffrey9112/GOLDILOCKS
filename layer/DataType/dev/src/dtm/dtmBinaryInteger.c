/*******************************************************************************
 * dtmBinaryInteger.c
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
 * @file dtmBinaryInteger.c
 * @brief DataType Layer Binary Integer의 비교 연산
 */

#include <dtl.h>


/**
 * @defgroup dtlBinaryIntegerType Binary Integer Type간의 비교 연산
 * <BR>
 * <H2> Binary Integer Types </H2> 
 * <BR> - Boolean / SmallInt / Integer / BigInt
 * <BR> - Boolean의 비교는 Boolean vs Boolean의 비교만 지원
 * <BR> - SmallInt / Integer / BigInt 간의 비교 연산 지원
 * <BR>
 * 
 * @ingroup dtlOperCompareType
 * @{
 *
 * @} dtlBinaryIntegerType */

/*******************************************************************************
 * TYPE : IS EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryIntegerType
 * <H2> Binary Integer Type : IS EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to         </td>
 *     <th> DTL_TYPE_BOOLEAN  </th>
 *     <th> DTL_TYPE_SMALLINT </th>
 *     <th> DTL_TYPE_INTEGER  </th>
 *     <th> DTL_TYPE_BIGINT   </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BOOLEAN        </th>
 *     <td> dtlOperIsEqualBooleanToBoolean </td>
 *     <td align=center> /          </td>
 *     <td align=center> /          </td>
 *     <td align=center> /          </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT         </th>
 *     <td align=center> /            </td>
 *     <td> dtlOperIsEqualSmallIntToSmallInt </td>
 *     <td> dtlOperIsEqualSmallIntToInteger  </td>
 *     <td> dtlOperIsEqualSmallIntToBigInt   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER         </th>
 *     <td align=center> /           </td>
 *     <td> dtlOperIsEqualIntegerToSmallInt </td>
 *     <td> dtlOperIsEqualIntegerToInteger  </td>
 *     <td> dtlOperIsEqualIntegerToBigInt   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT         </th>
 *     <td align=center> /          </td>
 *     <td> dtlOperIsEqualBigIntToSmallInt </td>
 *     <td> dtlOperIsEqualBigIntToInteger  </td>
 *     <td> dtlOperIsEqualBigIntToBigInt   </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Boolean vs Boolean의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualBooleanToBoolean( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBooleanType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlBooleanType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs SmallInt의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualSmallIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs Integer의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualSmallIntToInteger( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs BigInt의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualSmallIntToBigInt( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs SmallInt의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualIntegerToSmallInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs Integer의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualIntegerToInteger( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs BigInt의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualIntegerToBigInt( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}


/**
 * @brief BigInt vs SmallInt의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualBigIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Integer의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualBigIntToInteger( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs BigInt의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualBigIntToBigInt( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
           
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryIntegerType */

/*******************************************************************************
 * TYPE : IS NOT EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryIntegerType
 * <H2> Binary Integer Type : IS NOT EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to         </td>
 *     <th> DTL_TYPE_BOOLEAN  </th>
 *     <th> DTL_TYPE_SMALLINT </th>
 *     <th> DTL_TYPE_INTEGER  </th>
 *     <th> DTL_TYPE_BIGINT   </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BOOLEAN        </th>
 *     <td> dtlOperIsNotEqualBooleanToBoolean </td>
 *     <td align=center> /          </td>
 *     <td align=center> /          </td>
 *     <td align=center> /          </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT         </th>
 *     <td align=center> /            </td>
 *     <td> dtlOperIsNotEqualSmallIntToSmallInt </td>
 *     <td> dtlOperIsNotEqualSmallIntToInteger  </td>
 *     <td> dtlOperIsNotEqualSmallIntToBigInt   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER         </th>
 *     <td align=center> /           </td>
 *     <td> dtlOperIsNotEqualIntegerToSmallInt </td>
 *     <td> dtlOperIsNotEqualIntegerToInteger  </td>
 *     <td> dtlOperIsNotEqualIntegerToBigInt   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT         </th>
 *     <td align=center> /          </td>
 *     <td> dtlOperIsNotEqualBigIntToSmallInt </td>
 *     <td> dtlOperIsNotEqualBigIntToInteger  </td>
 *     <td> dtlOperIsNotEqualBigIntToBigInt   </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Boolean vs Boolean의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualBooleanToBoolean( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBooleanType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlBooleanType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs SmallInt의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualSmallIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs Integer의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualSmallIntToInteger( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs BigInt의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualSmallIntToBigInt( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs SmallInt의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualIntegerToSmallInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs Integer의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualIntegerToInteger( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs BigInt의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualIntegerToBigInt( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}


/**
 * @brief BigInt vs SmallInt의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualBigIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Integer의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualBigIntToInteger( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs BigInt의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualBigIntToBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryIntegerType */

/*******************************************************************************
 * TYPE : IS LESS THAN
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryIntegerType
 * <H2> Binary Integer Type : IS LESS THAN </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to         </td>
 *     <th> DTL_TYPE_BOOLEAN  </th>
 *     <th> DTL_TYPE_SMALLINT </th>
 *     <th> DTL_TYPE_INTEGER  </th>
 *     <th> DTL_TYPE_BIGINT   </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BOOLEAN        </th>
 *     <td> dtlOperIsLessThanBooleanToBoolean </td>
 *     <td align=center> /          </td>
 *     <td align=center> /          </td>
 *     <td align=center> /          </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT         </th>
 *     <td align=center> /            </td>
 *     <td> dtlOperIsLessThanSmallIntToSmallInt </td>
 *     <td> dtlOperIsLessThanSmallIntToInteger  </td>
 *     <td> dtlOperIsLessThanSmallIntToBigInt   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER         </th>
 *     <td align=center> /           </td>
 *     <td> dtlOperIsLessThanIntegerToSmallInt </td>
 *     <td> dtlOperIsLessThanIntegerToInteger  </td>
 *     <td> dtlOperIsLessThanIntegerToBigInt   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT         </th>
 *     <td align=center> /          </td>
 *     <td> dtlOperIsLessThanBigIntToSmallInt </td>
 *     <td> dtlOperIsLessThanBigIntToInteger  </td>
 *     <td> dtlOperIsLessThanBigIntToBigInt   </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Boolean vs Boolean의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanBooleanToBoolean( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBooleanType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlBooleanType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        

    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs SmallInt의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanSmallIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs Integer의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanSmallIntToInteger( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs BigInt의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanSmallIntToBigInt( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Integer vs SmallInt의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanIntegerToSmallInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Integer vs Integer의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanIntegerToInteger( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs BigInt의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanIntegerToBigInt( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}


/**
 * @brief BigInt vs SmallInt의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanBigIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Integer의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanBigIntToInteger( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs BigInt의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanBigIntToBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryIntegerType */

/*******************************************************************************
 * TYPE : IS LESS THAN EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryIntegerType
 * <H2> Binary Integer Type : IS LESS THAN EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to         </td>
 *     <th> DTL_TYPE_BOOLEAN  </th>
 *     <th> DTL_TYPE_SMALLINT </th>
 *     <th> DTL_TYPE_INTEGER  </th>
 *     <th> DTL_TYPE_BIGINT   </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BOOLEAN        </th>
 *     <td> dtlOperIsLessThanEqualBooleanToBoolean </td>
 *     <td align=center> /          </td>
 *     <td align=center> /          </td>
 *     <td align=center> /          </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT         </th>
 *     <td align=center> /            </td>
 *     <td> dtlOperIsLessThanEqualSmallIntToSmallInt </td>
 *     <td> dtlOperIsLessThanEqualSmallIntToInteger  </td>
 *     <td> dtlOperIsLessThanEqualSmallIntToBigInt   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER         </th>
 *     <td align=center> /           </td>
 *     <td> dtlOperIsLessThanEqualIntegerToSmallInt </td>
 *     <td> dtlOperIsLessThanEqualIntegerToInteger  </td>
 *     <td> dtlOperIsLessThanEqualIntegerToBigInt   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT         </th>
 *     <td align=center> /          </td>
 *     <td> dtlOperIsLessThanEqualBigIntToSmallInt </td>
 *     <td> dtlOperIsLessThanEqualBigIntToInteger  </td>
 *     <td> dtlOperIsLessThanEqualBigIntToBigInt   </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Boolean vs Boolean의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualBooleanToBoolean( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBooleanType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlBooleanType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        

    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs SmallInt의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualSmallIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs Integer의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualSmallIntToInteger( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs BigInt의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualSmallIntToBigInt( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Integer vs SmallInt의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualIntegerToSmallInt( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Integer vs Integer의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualIntegerToInteger( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs BigInt의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualIntegerToBigInt( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}


/**
 * @brief BigInt vs SmallInt의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualBigIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Integer의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualBigIntToInteger( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs BigInt의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualBigIntToBigInt( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryIntegerType */

/*******************************************************************************
 * TYPE : IS GREATER THAN
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryIntegerType
 * <H2> Binary Integer Type : IS GREATER THAN </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to         </td>
 *     <th> DTL_TYPE_BOOLEAN  </th>
 *     <th> DTL_TYPE_SMALLINT </th>
 *     <th> DTL_TYPE_INTEGER  </th>
 *     <th> DTL_TYPE_BIGINT   </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BOOLEAN        </th>
 *     <td> dtlOperIsGreaterThanBooleanToBoolean </td>
 *     <td align=center> /          </td>
 *     <td align=center> /          </td>
 *     <td align=center> /          </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT         </th>
 *     <td align=center> /            </td>
 *     <td> dtlOperIsGreaterThanSmallIntToSmallInt </td>
 *     <td> dtlOperIsGreaterThanSmallIntToInteger  </td>
 *     <td> dtlOperIsGreaterThanSmallIntToBigInt   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER         </th>
 *     <td align=center> /           </td>
 *     <td> dtlOperIsGreaterThanIntegerToSmallInt </td>
 *     <td> dtlOperIsGreaterThanIntegerToInteger  </td>
 *     <td> dtlOperIsGreaterThanIntegerToBigInt   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT         </th>
 *     <td align=center> /          </td>
 *     <td> dtlOperIsGreaterThanBigIntToSmallInt </td>
 *     <td> dtlOperIsGreaterThanBigIntToInteger  </td>
 *     <td> dtlOperIsGreaterThanBigIntToBigInt   </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Boolean vs Boolean의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanBooleanToBoolean( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBooleanType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlBooleanType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs SmallInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanSmallIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs Integer의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanSmallIntToInteger( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs BigInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanSmallIntToBigInt( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Integer vs SmallInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanIntegerToSmallInt( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Integer vs Integer의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanIntegerToInteger( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs BigInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanIntegerToBigInt( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}


/**
 * @brief BigInt vs SmallInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanBigIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Integer의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanBigIntToInteger( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs BigInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanBigIntToBigInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryIntegerType */

/*******************************************************************************
 * TYPE : IS GREATER THAN EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryIntegerType
 * <H2> Binary Integer Type : IS GREATER THAN EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to         </td>
 *     <th> DTL_TYPE_BOOLEAN  </th>
 *     <th> DTL_TYPE_SMALLINT </th>
 *     <th> DTL_TYPE_INTEGER  </th>
 *     <th> DTL_TYPE_BIGINT   </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BOOLEAN        </th>
 *     <td> dtlOperIsGreaterThanEqualBooleanToBoolean </td>
 *     <td align=center> /          </td>
 *     <td align=center> /          </td>
 *     <td align=center> /          </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT         </th>
 *     <td align=center> /            </td>
 *     <td> dtlOperIsGreaterThanEqualSmallIntToSmallInt </td>
 *     <td> dtlOperIsGreaterThanEqualSmallIntToInteger  </td>
 *     <td> dtlOperIsGreaterThanEqualSmallIntToBigInt   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER         </th>
 *     <td align=center> /           </td>
 *     <td> dtlOperIsGreaterThanEqualIntegerToSmallInt </td>
 *     <td> dtlOperIsGreaterThanEqualIntegerToInteger  </td>
 *     <td> dtlOperIsGreaterThanEqualIntegerToBigInt   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT         </th>
 *     <td align=center> /          </td>
 *     <td> dtlOperIsGreaterThanEqualBigIntToSmallInt </td>
 *     <td> dtlOperIsGreaterThanEqualBigIntToInteger  </td>
 *     <td> dtlOperIsGreaterThanEqualBigIntToBigInt   </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Boolean vs Boolean의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualBooleanToBoolean( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBooleanType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlBooleanType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs SmallInt의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualSmallIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                                       dtlValueEntry  * aInputArgument,
                                                       void           * aResultValue,
                                                       void           * aInfo,
                                                       dtlFuncVector  * aVectorFunc,
                                                       void           * aVectorArg,
                                                       void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs Integer의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualSmallIntToInteger( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief SmallInt vs BigInt의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualSmallIntToBigInt( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlSmallIntType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Integer vs SmallInt의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualIntegerToSmallInt( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
        
    return STL_SUCCESS;
}

/**
 * @brief Integer vs Integer의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualIntegerToInteger( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Integer vs BigInt의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualIntegerToBigInt( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlIntegerType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}


/**
 * @brief BigInt vs SmallInt의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualBigIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlSmallIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs Integer의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualBigIntToInteger( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlIntegerType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief BigInt vs BigInt의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualBigIntToBigInt( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlBigIntType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlBigIntType *) aInputArgument[1].mValue.mDataValue->mValue) ;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryIntegerType */


