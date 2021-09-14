/*******************************************************************************
 * dtmBinaryReal.c
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
 * @file dtmBinaryReal.c
 * @brief DataType Layer Binary Real의 비교 연산
 */

#include <dtl.h>


/**
 * @defgroup dtlBinaryRealType Binary Real Type간의 비교 연산
 * <BR>
 * <H2> Binary Real Types </H2> 
 * <BR> - Real / Double 간의 비교 연산 지원
 * <BR>
 *
 * @ingroup dtlOperCompareType
 * @{
 *
 * @} dtlBinaryRealType */

/*******************************************************************************
 * TYPE : IS EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryRealType
 * <H2> Binary Real Type : IS EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to         </td>
 *     <th> DTL_TYPE_REAL   </th>
 *     <th> DTL_TYPE_DOUBLE </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_REAL              </th>
 *     <td> dtlOperIsEqualRealToReal   </td>
 *     <td> dtlOperIsEqualRealToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DOUBLE              </th>
 *     <td> dtlOperIsEqualDoubleToReal   </td>
 *     <td> dtlOperIsEqualDoubleToDouble </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Real vs Real의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualRealToReal( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Real vs Double의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualRealToDouble( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs Real의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualDoubleToReal( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs Double의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualDoubleToDouble( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryRealType */

/*******************************************************************************
 * TYPE : IS NOT EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryRealType
 * <H2> Binary Real Type : IS NOT EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to         </td>
 *     <th> DTL_TYPE_REAL   </th>
 *     <th> DTL_TYPE_DOUBLE </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_REAL              </th>
 *     <td> dtlOperIsNotEqualRealToReal   </td>
 *     <td> dtlOperIsNotEqualRealToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DOUBLE              </th>
 *     <td> dtlOperIsNotEqualDoubleToReal   </td>
 *     <td> dtlOperIsNotEqualDoubleToDouble </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Real vs Real의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualRealToReal( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Real vs Double의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualRealToDouble( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs Real의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualDoubleToReal( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs Double의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualDoubleToDouble( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryRealType */

/*******************************************************************************
 * TYPE : IS LESS THAN
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryRealType
 * <H2> Binary Real Type : IS LESS THAN </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to         </td>
 *     <th> DTL_TYPE_REAL   </th>
 *     <th> DTL_TYPE_DOUBLE </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_REAL              </th>
 *     <td> dtlOperIsLessThanRealToReal   </td>
 *     <td> dtlOperIsLessThanRealToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DOUBLE              </th>
 *     <td> dtlOperIsLessThanDoubleToReal   </td>
 *     <td> dtlOperIsLessThanDoubleToDouble </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Real vs Real의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanRealToReal( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Real vs Double의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanRealToDouble( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs Real의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanDoubleToReal( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs Double의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanDoubleToDouble( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryRealType */

/*******************************************************************************
 * TYPE : IS LESS THAN EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryRealType
 * <H2> Binary Real Type : IS LESS THAN EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to         </td>
 *     <th> DTL_TYPE_REAL   </th>
 *     <th> DTL_TYPE_DOUBLE </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_REAL              </th>
 *     <td> dtlOperIsLessThanEqualRealToReal   </td>
 *     <td> dtlOperIsLessThanEqualRealToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DOUBLE              </th>
 *     <td> dtlOperIsLessThanEqualDoubleToReal   </td>
 *     <td> dtlOperIsLessThanEqualDoubleToDouble </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Real vs Real의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualRealToReal( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Real vs Double의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualRealToDouble( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs Real의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualDoubleToReal( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs Double의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualDoubleToDouble( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryRealType */

/*******************************************************************************
 * TYPE : IS GREATER THAN
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryRealType
 * <H2> Binary Real Type : IS GREATER THAN </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to         </td>
 *     <th> DTL_TYPE_REAL   </th>
 *     <th> DTL_TYPE_DOUBLE </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_REAL              </th>
 *     <td> dtlOperIsGreaterThanRealToReal   </td>
 *     <td> dtlOperIsGreaterThanRealToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DOUBLE              </th>
 *     <td> dtlOperIsGreaterThanDoubleToReal   </td>
 *     <td> dtlOperIsGreaterThanDoubleToDouble </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Real vs Real의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanRealToReal( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Real vs Double의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanRealToDouble( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs Real의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanDoubleToReal( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs Double의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanDoubleToDouble( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryRealType */

/*******************************************************************************
 * TYPE : IS GREATER THAN EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryRealType
 * <H2> Binary Real Type : IS GREATER THAN EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to         </td>
 *     <th> DTL_TYPE_REAL   </th>
 *     <th> DTL_TYPE_DOUBLE </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_REAL              </th>
 *     <td> dtlOperIsGreaterThanEqualRealToReal   </td>
 *     <td> dtlOperIsGreaterThanEqualRealToDouble </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DOUBLE              </th>
 *     <td> dtlOperIsGreaterThanEqualDoubleToReal   </td>
 *     <td> dtlOperIsGreaterThanEqualDoubleToDouble </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Real vs Real의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualRealToReal( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Real vs Double의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualRealToDouble( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlRealType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs Real의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualDoubleToReal( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlRealType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Double vs Double의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualDoubleToDouble( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDoubleType *) aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlDoubleType *) aInputArgument[1].mValue.mDataValue->mValue) ;
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}

/** @} dtlBinaryRealType */
