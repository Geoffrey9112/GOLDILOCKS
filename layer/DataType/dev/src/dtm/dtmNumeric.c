/*******************************************************************************
 * dtmNumeric.c
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
 * @file dtmNumeric.c
 * @brief DataType Layer Numeric의 비교 연산
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtdDataType.h>
#include <dtfNumeric.h>


/**
 * @defgroup dtlNumericType Numeric Type간의 비교 연산
 * <BR>
 * <H2> Numeric Types </H2> 
 * <BR> - Numeric 간의 비교 연산 지원
 * <BR> - Decimal 과의 비교 연산은 지원하지 않음
 * <BR>
 * 
 * @ingroup dtlOperCompareType
 * @{
 *
 * @} dtlNumericType */

/*******************************************************************************
 * TYPE : IS EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlNumericType
 * <H2> Numeric Type : IS EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_SMALLINT           </th>
 *     <th> DTL_TYPE_INTEGER            </th>
 *     <th> DTL_TYPE_BIGINT             </th>
 *     <th> DTL_TYPE_REAL               </th>
 *     <th> DTL_TYPE_DOUBLE             </th>
 *     <th> DTL_TYPE_FLOAT              </th>
 *     <th> DTL_TYPE_NUMBER             </th>
 *     <th> DTL_TYPE_NUMBER             </th>
 *     <th> DTL_TYPE_DECIMAL            </th>
 *   </tr>
 *    <tr>
 *     <th> DTL_TYPE_FLOAT                   </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                  </th>
 *     <td> dtlOperIsEqualNumericToSmallInt   </td>
 *     <td> dtlOperIsEqualNumericToInteger    </td>
 *     <td> dtlOperIsEqualNumericToBigInt     </td>
 *     <td> dtlOperIsEqualNumericToReal       </td>
 *     <td> dtlOperIsEqualNumericToDouble     </td>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsEqualNumericToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *    <tr>
 *     <th> DTL_TYPE_NUMBER                  </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DECIMAL                 </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 * </table>
 * <BR>
 * <table>
 *   <tr>
 *     <td align=center> from \ to            </td>
 *     <th> DTL_TYPE_FLOAT                    </th>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <th> DTL_TYPE_DECIMAL                  </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsEqualSmallIntToNumeric   </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER                  </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsEqualIntegerToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT                   </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsEqualBigIntToNumeric     </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsEqualRealToNumeric       </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsEqualDoubleToNumeric     </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_FLOAT                    </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsEqualNumericToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DECIMAL                  </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */


/**
 * @brief Numeric vs SmallInt의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualNumericToSmallInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsEqualNumericToSmallInt_INLINE( (dtlNumericType *)(sLeftValue->mValue),
                                            sLeftValue->mLength,
                                            *((dtlSmallIntType *)(sRightValue->mValue)),
                                            sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Integer의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualNumericToInteger( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsEqualNumericToInteger_INLINE( (dtlNumericType *)(sLeftValue->mValue),
                                           sLeftValue->mLength,
                                           *((dtlIntegerType *)(sRightValue->mValue)),
                                           sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs BigInt의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualNumericToBigInt( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsEqualNumericToBigInt_INLINE( (dtlNumericType *)(sLeftValue->mValue),
                                          sLeftValue->mLength,
                                          *((dtlBigIntType *)(sRightValue->mValue)),
                                          sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Real의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualNumericToReal( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv )
{
    dtlDataValue    * sNumericDataValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sRealDataValue    = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsEqualNumericToReal_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                        sNumericDataValue->mLength,
                                        *(dtlRealType*)(sRealDataValue->mValue),
                                        sRealDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Double의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualNumericToDouble( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    dtlDataValue    * sNumericDataValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sDoubleDataValue  = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsEqualNumericToDouble_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                          sNumericDataValue->mLength,
                                          *(dtlDoubleType*)(sDoubleDataValue->mValue),
                                          sDoubleDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief SmallInt vs Numeric의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualSmallIntToNumeric( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsEqualNumericToSmallInt_INLINE( (dtlNumericType *)(sRightValue->mValue),
                                            sRightValue->mLength,
                                            *((dtlSmallIntType *)(sLeftValue->mValue)),
                                            sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Integer vs Numeric의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualIntegerToNumeric( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsEqualNumericToInteger_INLINE( (dtlNumericType *)(sRightValue->mValue),
                                           sRightValue->mLength,
                                           *((dtlIntegerType *)(sLeftValue->mValue)),
                                           sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief BigInt vs Numeric의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualBigIntToNumeric( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsEqualNumericToBigInt_INLINE( (dtlNumericType *)(sRightValue->mValue),
                                          sRightValue->mLength,
                                          *((dtlBigIntType *)(sLeftValue->mValue)),
                                          sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Real vs Numeric의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualRealToNumeric( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv )
{
    dtlDataValue    * sRealDataValue    = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sNumericDataValue = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsEqualNumericToReal_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                        sNumericDataValue->mLength,
                                        *(dtlRealType*)(sRealDataValue->mValue),
                                        sRealDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief Double vs Numeric 의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualDoubleToNumeric( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    dtlDataValue    * sDoubleDataValue  = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sNumericDataValue = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsEqualNumericToDouble_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                          sNumericDataValue->mLength,
                                          *(dtlDoubleType*)(sDoubleDataValue->mValue),
                                          sDoubleDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief numeric type간의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualNumericToNumeric( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    stlUInt8  * sLeftDigit  = (stlUInt8*) DTF_NUMERIC( aInputArgument[0].mValue.mDataValue );
    stlUInt8  * sRightDigit = (stlUInt8*) DTF_NUMERIC( aInputArgument[1].mValue.mDataValue );
    stlInt32    i;
    stlInt32    sLength;

    if( ( *sLeftDigit == *sRightDigit ) &&
        ( aInputArgument[0].mValue.mDataValue->mLength ==
          aInputArgument[1].mValue.mDataValue->mLength ) )
    {
        sLength = aInputArgument[0].mValue.mDataValue->mLength;
        if( sLength == 1 )
        {
            DTL_BOOLEAN( aResultValue ) = STL_TRUE;
            DTL_SET_BOOLEAN_LENGTH( aResultValue );
                
            return STL_SUCCESS;
        }

        sLength--;
        for( i = 1 ; sLeftDigit[i] == sRightDigit[i] ; i++ )
        {
            if( i == sLength )
            {
                DTL_BOOLEAN( aResultValue ) = STL_TRUE;
                DTL_SET_BOOLEAN_LENGTH( aResultValue );
                
                return STL_SUCCESS;
            }
        }
    }
    
    DTL_BOOLEAN( aResultValue ) = STL_FALSE;
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}

/** @} dtlNumericType */


/*******************************************************************************
 * TYPE : IS NOT EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlNumericType
 * <H2> Numeric Type : IS NOT EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_SMALLINT           </th>
 *     <th> DTL_TYPE_INTEGER            </th>
 *     <th> DTL_TYPE_BIGINT             </th>
 *     <th> DTL_TYPE_REAL               </th>
 *     <th> DTL_TYPE_DOUBLE             </th>
 *     <th> DTL_TYPE_FLAOT              </th>
 *     <th> DTL_TYPE_NUMBER             </th>
 *     <th> DTL_TYPE_NUMBER             </th>
 *     <th> DTL_TYPE_DECIMAL            </th>
 *   </tr>
 *    <tr>
 *     <th> DTL_TYPE_FLOAT                 </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                  </th>
 *     <td> dtlOperIsNotEqualNumericToSmallInt   </td>
 *     <td> dtlOperIsNotEqualNumericToInteger    </td>
 *     <td> dtlOperIsNotEqualNumericToBigInt     </td>
 *     <td> dtlOperIsNotEqualNumericToReal       </td>
 *     <td> dtlOperIsNotEqualNumericToDouble     </td>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsNotEqualNumericToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *    <tr>
 *     <th> DTL_TYPE_NUMBER                  </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DECIMAL                 </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 * </table>
 * <BR>
 * <table>
 *   <tr>
 *     <td align=center> from \ to            </td>
 *     <th> DTL_TYPE_FLOAT                    </th>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <th> DTL_TYPE_DECIMAL                  </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsNotEqualSmallIntToNumeric   </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER                  </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsNotEqualIntegerToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT                   </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsNotEqualBigIntToNumeric     </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsNotEqualRealToNumeric       </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsNotEqualDoubleToNumeric     </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_FLOAT                    </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsNotEqualNumericToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DECIMAL                  </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */



/**
 * @brief Numeric vs SmallInt의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualNumericToSmallInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsNotEqualNumericToSmallInt_INLINE( (dtlNumericType *)(sLeftValue->mValue),
                                               sLeftValue->mLength,
                                               *((dtlSmallIntType *)(sRightValue->mValue)),
                                               sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Integer의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualNumericToInteger( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsNotEqualNumericToInteger_INLINE( (dtlNumericType *)(sLeftValue->mValue),
                                              sLeftValue->mLength,
                                              *((dtlIntegerType *)(sRightValue->mValue)),
                                              sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs BigInt의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualNumericToBigInt( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsNotEqualNumericToBigInt_INLINE( (dtlNumericType *)(sLeftValue->mValue),
                                             sLeftValue->mLength,
                                             *((dtlBigIntType *)(sRightValue->mValue)),
                                             sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Real의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualNumericToReal( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    dtlDataValue    * sNumericDataValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sRealDataValue    = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsNotEqualNumericToReal_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                           sNumericDataValue->mLength,
                                           *(dtlRealType*)(sRealDataValue->mValue),
                                           sRealDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Double의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualNumericToDouble( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    dtlDataValue    * sNumericDataValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sDoubleDataValue  = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsNotEqualNumericToDouble_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                             sNumericDataValue->mLength,
                                             *(dtlDoubleType*)(sDoubleDataValue->mValue),
                                             sDoubleDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief SmallInt vs Numeric의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualSmallIntToNumeric( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsNotEqualNumericToSmallInt_INLINE( (dtlNumericType *)(sRightValue->mValue),
                                               sRightValue->mLength,
                                               *((dtlSmallIntType *)(sLeftValue->mValue)),
                                               sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Integer vs Numeric의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualIntegerToNumeric( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsNotEqualNumericToInteger_INLINE( (dtlNumericType *)(sRightValue->mValue),
                                              sRightValue->mLength,
                                              *((dtlIntegerType *)(sLeftValue->mValue)),
                                              sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief BigInt vs Numeric의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualBigIntToNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsNotEqualNumericToBigInt_INLINE( (dtlNumericType *)(sRightValue->mValue),
                                             sRightValue->mLength,
                                             *((dtlBigIntType *)(sLeftValue->mValue)),
                                             sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}    

/**
 * @brief Real vs Numeric의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualRealToNumeric( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    dtlDataValue    * sRealDataValue    = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sNumericDataValue = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsNotEqualNumericToReal_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                           sNumericDataValue->mLength,
                                           *(dtlRealType*)(sRealDataValue->mValue),
                                           sRealDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief Double vs Numeric 의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualDoubleToNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    dtlDataValue    * sDoubleDataValue  = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sNumericDataValue = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsNotEqualNumericToDouble_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                             sNumericDataValue->mLength,
                                             *(dtlDoubleType*)(sDoubleDataValue->mValue),
                                             sDoubleDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief numeric type간의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualNumericToNumeric( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    stlUInt8  * sLeftDigit  = (stlUInt8*) DTF_NUMERIC( aInputArgument[0].mValue.mDataValue );
    stlUInt8  * sRightDigit = (stlUInt8*) DTF_NUMERIC( aInputArgument[1].mValue.mDataValue );
    stlInt32    i;
    stlInt32    sLength;

    if( ( *sLeftDigit == *sRightDigit ) &&
        ( aInputArgument[0].mValue.mDataValue->mLength ==
          aInputArgument[1].mValue.mDataValue->mLength ) )
    {
        sLength = aInputArgument[0].mValue.mDataValue->mLength;
        if( sLength == 1 )
        {
            DTL_BOOLEAN( aResultValue ) = STL_FALSE;
            DTL_SET_BOOLEAN_LENGTH( aResultValue );
                
            return STL_SUCCESS;
        }

        sLength--;
        for( i = 1 ; sLeftDigit[i] == sRightDigit[i] ; i++ )
        {
            if( i == sLength )
            {
                DTL_BOOLEAN( aResultValue ) = STL_FALSE;
                DTL_SET_BOOLEAN_LENGTH( aResultValue );
                
                return STL_SUCCESS;
            }
        }
    }

    DTL_BOOLEAN( aResultValue ) = STL_TRUE;
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}

/** @} dtlNumericType */

/*******************************************************************************
 * TYPE : IS LESS THAN
 ******************************************************************************/
/**
 * @addtogroup dtlNumericType
 * <H2> Numeric Type : IS LESS THAN </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_SMALLINT           </th>
 *     <th> DTL_TYPE_INTEGER            </th>
 *     <th> DTL_TYPE_BIGINT             </th>
 *     <th> DTL_TYPE_REAL               </th>
 *     <th> DTL_TYPE_DOUBLE             </th>
 *     <th> DTL_TYPE_FLOAT              </th>
 *     <th> DTL_TYPE_NUMBER             </th>
 *     <th> DTL_TYPE_NUMBER             </th>
 *     <th> DTL_TYPE_DECIMAL            </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_FLOAT                 </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <td> dtlOperIsLessThanNumericToSmallInt   </td>
 *     <td> dtlOperIsLessThanNumericToInteger    </td>
 *     <td> dtlOperIsLessThanNumericToBigInt     </td>
 *     <td> dtlOperIsLessThanNumericToReal       </td>
 *     <td> dtlOperIsLessThanNumericToDouble     </td>
 *     <td align=center> /                   </td>
 *     <td> dtlOperIsLessThanNumericToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                 </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DECIMAL                 </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 * </table>
 * <BR>
 * <table>
 *   <tr>
 *     <td align=center> from \ to            </td>
 *     <th> DTL_TYPE_FLOAT                    </th>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <th> DTL_TYPE_DECIMAL                  </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td> dtlOperIsLessThanSmallIntToNumeric   </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER                  </th>
 *     <td> dtlOperIsLessThanIntegerToNumeric    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT                   </th>
 *     <td> dtlOperIsLessThanBigIntToNumeric     </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td> dtlOperIsLessThanRealToNumeric       </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DOUBLE                  </th>
 *     <td> dtlOperIsLessThanDoubleToNumeric     </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_FLOAT                   </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <td> dtlOperIsLessThanNumericToNumeric    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DECIMAL                  </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Numeric vs SmallInt의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanNumericToSmallInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanSmallIntToNumeric_INLINE( *((dtlSmallIntType *)(sRightValue->mValue)),
                                                  sRightValue->mLength,
                                                  (dtlNumericType *)(sLeftValue->mValue),
                                                  sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Integer의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanNumericToInteger( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanIntegerToNumeric_INLINE( *((dtlIntegerType *)(sRightValue->mValue)),
                                                 sRightValue->mLength,
                                                 (dtlNumericType *)(sLeftValue->mValue),
                                                 sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs BigInt의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanNumericToBigInt( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanBigIntToNumeric_INLINE( *((dtlBigIntType *)(sRightValue->mValue)),
                                                sRightValue->mLength,
                                                (dtlNumericType *)(sLeftValue->mValue),
                                                sLeftValue->mLength ); 
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Real의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanNumericToReal( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    dtlDataValue    * sNumericDataValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sRealDataValue    = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsLessThanNumericToReal_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                           sNumericDataValue->mLength,
                                           *(dtlRealType*)(sRealDataValue->mValue),
                                           sRealDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Double의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanNumericToDouble( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    dtlDataValue    * sNumericDataValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sDoubleDataValue  = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsLessThanNumericToDouble_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                             sNumericDataValue->mLength,
                                             *(dtlDoubleType*)(sDoubleDataValue->mValue),
                                             sDoubleDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief SmallInt vs Numeric의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanSmallIntToNumeric( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanSmallIntToNumeric_INLINE( *((dtlSmallIntType *)(sLeftValue->mValue)),
                                               sLeftValue->mLength,
                                               (dtlNumericType *)(sRightValue->mValue),
                                               sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Integer vs Numeric의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanIntegerToNumeric( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanIntegerToNumeric_INLINE( *((dtlIntegerType *)(sLeftValue->mValue)),
                                              sLeftValue->mLength,
                                              (dtlNumericType *)(sRightValue->mValue),
                                              sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief BigInt vs Numeric의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanBigIntToNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanBigIntToNumeric_INLINE( *((dtlBigIntType *)(sLeftValue->mValue)),
                                             sLeftValue->mLength,
                                             (dtlNumericType *)(sRightValue->mValue),
                                             sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Real vs Numeric의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanRealToNumeric( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    dtlDataValue    * sRealDataValue    = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sNumericDataValue = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsGreaterThanNumericToReal_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                              sNumericDataValue->mLength,
                                              *(dtlRealType*)(sRealDataValue->mValue),
                                              sRealDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief Double vs Numeric 의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanDoubleToNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    dtlDataValue    * sDoubleDataValue  = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sNumericDataValue = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsGreaterThanNumericToDouble_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                                sNumericDataValue->mLength,
                                                *(dtlDoubleType*)(sDoubleDataValue->mValue),
                                                sDoubleDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief numeric type간의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanNumericToNumeric( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    stlUInt8  * sLeftDigit  = (stlUInt8*) DTF_NUMERIC( aInputArgument[0].mValue.mDataValue );
    stlUInt8  * sRightDigit = (stlUInt8*) DTF_NUMERIC( aInputArgument[1].mValue.mDataValue );
    stlInt8   * sTable;
    stlInt32    i = 0;

    if( sLeftDigit[0] == sRightDigit[0] )
    {
        sTable = dtlNumericLenCompTable
            [ aInputArgument[0].mValue.mDataValue->mLength ]
            [ aInputArgument[1].mValue.mDataValue->mLength ];

        if( sTable[1] == 0 )
        {
            if( sLeftDigit[0] > 127 )
            {
                DTL_BOOLEAN( aResultValue ) = ( sTable[0] == -1 );
            }
            else
            {
                DTL_BOOLEAN( aResultValue ) = ( sTable[0] == 1 );
            }

            DTL_SET_BOOLEAN_LENGTH( aResultValue );
                
            return STL_SUCCESS;
        }

        for( i = 1 ; sLeftDigit[i] == sRightDigit[i] ; i++ )
        {
            if( i == sTable[1] )
            {
                if( sLeftDigit[0] > 127 )
                {
                    DTL_BOOLEAN( aResultValue ) = ( sTable[0] == -1 );
                }
                else
                {
                    DTL_BOOLEAN( aResultValue ) = ( sTable[0] == 1 );
                }

                DTL_SET_BOOLEAN_LENGTH( aResultValue );
                
                return STL_SUCCESS;
            }
        }
    }

    DTL_BOOLEAN( aResultValue ) = ( sLeftDigit[i] < sRightDigit[i] );
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}

/** @} dtlNumericType */

/*******************************************************************************
 * TYPE : IS LESS THAN EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlNumericType
 * <H2> Numeric Type : IS LESS THAN EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_SMALLINT           </th>
 *     <th> DTL_TYPE_INTEGER            </th>
 *     <th> DTL_TYPE_BIGINT             </th>
 *     <th> DTL_TYPE_REAL               </th>
 *     <th> DTL_TYPE_DOUBLE             </th>
 *     <th> DTL_TYPE_FLOAT              </th>
 *     <th> DTL_TYPE_NUMBER             </th>
 *     <th> DTL_TYPE_NUMBER             </th>
 *     <th> DTL_TYPE_DECIMAL            </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_FLOAT                   </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                  </th>
 *     <td> dtlOperIsLessThanEqualNumericToSmallInt   </td>
 *     <td> dtlOperIsLessThanEqualNumericToInteger    </td>
 *     <td> dtlOperIsLessThanEqualNumericToBigInt     </td>
 *     <td> dtlOperIsLessThanEqualNumericToReal       </td>
 *     <td> dtlOperIsLessThanEqualNumericToDouble     </td>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsLessThanEqualNumericToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                  </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DECIMAL                 </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 * </table>
 * <BR>
 * <table>
 *   <tr>
 *     <td align=center> from \ to            </td>
 *     <th> DTL_TYPE_FLOAT                    </th>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <th> DTL_TYPE_DECIMAL                  </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsLessThanEqualSmallIntToNumeric   </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER                  </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsLessThanEqualIntegerToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT                   </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsLessThanEqualBigIntToNumeric     </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsLessThanEqualRealToNumeric       </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsLessThanEqualDoubleToNumeric     </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_FLOAT                    </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsLessThanEqualNumericToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DECIMAL                  </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Numeric vs SmallInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualNumericToSmallInt( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanEqualSmallIntToNumeric_INLINE( *((dtlSmallIntType *)(sRightValue->mValue)),
                                                       sRightValue->mLength,
                                                       (dtlNumericType *)(sLeftValue->mValue),
                                                       sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Integer의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualNumericToInteger( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanEqualIntegerToNumeric_INLINE( *((dtlIntegerType *)(sRightValue->mValue)),
                                                      sRightValue->mLength,
                                                      (dtlNumericType *)(sLeftValue->mValue),
                                                      sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs BigInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualNumericToBigInt( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanEqualBigIntToNumeric_INLINE( *((dtlBigIntType *)(sRightValue->mValue)),
                                                     sRightValue->mLength,
                                                     (dtlNumericType *)(sLeftValue->mValue),
                                                     sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Real의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualNumericToReal( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    dtlDataValue    * sNumericDataValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sRealDataValue    = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsLessThanEqualNumericToReal_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                                sNumericDataValue->mLength,
                                                *(dtlRealType*)(sRealDataValue->mValue),
                                                sRealDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Double의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualNumericToDouble( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    dtlDataValue    * sNumericDataValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sDoubleDataValue  = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsLessThanEqualNumericToDouble_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                                  sNumericDataValue->mLength,
                                                  *(dtlDoubleType*)(sDoubleDataValue->mValue),
                                                  sDoubleDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief SmallInt vs Numeric의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualSmallIntToNumeric( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanEqualSmallIntToNumeric_INLINE( *((dtlSmallIntType *)(sLeftValue->mValue)),
                                                    sLeftValue->mLength,
                                                    (dtlNumericType *)(sRightValue->mValue),
                                                    sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Integer vs Numeric의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualIntegerToNumeric( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanEqualIntegerToNumeric_INLINE( *((dtlIntegerType *)(sLeftValue->mValue)),
                                                    sLeftValue->mLength,
                                                    (dtlNumericType *)(sRightValue->mValue),
                                                    sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief BigInt vs Numeric의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualBigIntToNumeric( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanEqualBigIntToNumeric_INLINE( *((dtlBigIntType *)(sLeftValue->mValue)),
                                                  sLeftValue->mLength,
                                                  (dtlNumericType *)(sRightValue->mValue),
                                                  sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Real vs Numeric의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualRealToNumeric( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    dtlDataValue    * sRealDataValue    = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sNumericDataValue = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsGreaterThanEqualNumericToReal_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                                   sNumericDataValue->mLength,
                                                   *(dtlRealType*)(sRealDataValue->mValue),
                                                   sRealDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief Double vs Numeric 의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualDoubleToNumeric( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    dtlDataValue    * sDoubleDataValue  = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sNumericDataValue = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsGreaterThanEqualNumericToDouble_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                                     sNumericDataValue->mLength,
                                                     *(dtlDoubleType*)(sDoubleDataValue->mValue),
                                                     sDoubleDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief numeric type간의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualNumericToNumeric( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    stlUInt8  * sLeftDigit  = (stlUInt8*) DTF_NUMERIC( aInputArgument[0].mValue.mDataValue );
    stlUInt8  * sRightDigit = (stlUInt8*) DTF_NUMERIC( aInputArgument[1].mValue.mDataValue );
    stlInt8   * sTable;
    stlInt32    i = 0;

    if( sLeftDigit[0] == sRightDigit[0] )
    {
        sTable = dtlNumericLenCompTable
            [ aInputArgument[0].mValue.mDataValue->mLength ]
            [ aInputArgument[1].mValue.mDataValue->mLength ];

        if( sTable[1] == 0 )
        {
            if( sLeftDigit[0] > 127 )
            {
                DTL_BOOLEAN( aResultValue ) = ( sTable[0] != 1 );
            }
            else
            {
                DTL_BOOLEAN( aResultValue ) = ( sTable[0] != -1 );
            }

            DTL_SET_BOOLEAN_LENGTH( aResultValue );
                
            return STL_SUCCESS;
        }

        for( i = 1 ; sLeftDigit[i] == sRightDigit[i] ; i++ )
        {
            if( i == sTable[1] )
            {
                if( sLeftDigit[0] > 127 )
                {
                    DTL_BOOLEAN( aResultValue ) = ( sTable[0] != 1 );
                }
                else
                {
                    DTL_BOOLEAN( aResultValue ) = ( sTable[0] != -1 );
                }

                DTL_SET_BOOLEAN_LENGTH( aResultValue );
                
                return STL_SUCCESS;
            }
        }
    }

    DTL_BOOLEAN( aResultValue ) = ( sLeftDigit[i] < sRightDigit[i] );
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}

/** @} dtlNumericType */

/*******************************************************************************
 * TYPE : IS GREATER THAN
 ******************************************************************************/
/**
 * @addtogroup dtlNumericType
 * <H2> Numeric Type : IS GREATER THAN </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_SMALLINT           </th>
 *     <th> DTL_TYPE_INTEGER            </th>
 *     <th> DTL_TYPE_BIGINT             </th>
 *     <th> DTL_TYPE_REAL               </th>
 *     <th> DTL_TYPE_DOUBLE             </th>
 *     <th> DTL_TYPE_FLOAT              </th>
 *     <th> DTL_TYPE_NUMBER             </th>
 *     <th> DTL_TYPE_NUMBER             </th>
 *     <th> DTL_TYPE_DECIMAL            </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_FLOAT                   </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <td> dtlOperIsGreaterThanNumericToSmallInt   </td>
 *     <td> dtlOperIsGreaterThanNumericToInteger    </td>
 *     <td> dtlOperIsGreaterThanNumericToBigInt     </td>
 *     <td> dtlOperIsGreaterThanNumericToReal       </td>
 *     <td> dtlOperIsGreaterThanNumericToDouble     </td>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsGreaterThanNumericToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                  </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DECIMAL                 </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 * </table>
 * <BR>
 * <table>
 *   <tr>
 *     <td align=center> from \ to            </td>
 *     <th> DTL_TYPE_FLOAT                    </th>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <th> DTL_TYPE_DECIMAL                  </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsGreaterThanSmallIntToNumeric   </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER                  </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsGreaterThanIntegerToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT                   </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsGreaterThanBigIntToNumeric     </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsGreaterThanRealToNumeric       </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsGreaterThanDoubleToNumeric     </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_FLOAT                    </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsGreaterThanNumericToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DECIMAL                  </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Numeric vs SmallInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanNumericToSmallInt( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanSmallIntToNumeric_INLINE( *((dtlSmallIntType *)(sRightValue->mValue)),
                                               sRightValue->mLength,
                                               (dtlNumericType *)(sLeftValue->mValue),
                                               sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Integer의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanNumericToInteger( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanIntegerToNumeric_INLINE( *((dtlIntegerType *)(sRightValue->mValue)),
                                              sRightValue->mLength,
                                              (dtlNumericType *)(sLeftValue->mValue),
                                              sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs BigInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanNumericToBigInt( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanBigIntToNumeric_INLINE( *((dtlBigIntType *)(sRightValue->mValue)),
                                             sRightValue->mLength,
                                             (dtlNumericType *)(sLeftValue->mValue),
                                             sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Real의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanNumericToReal( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    dtlDataValue    * sNumericDataValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sRealDataValue    = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsGreaterThanNumericToReal_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                              sNumericDataValue->mLength,
                                              *(dtlRealType*)(sRealDataValue->mValue),
                                              sRealDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Double의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanNumericToDouble( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    dtlDataValue    * sNumericDataValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sDoubleDataValue  = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsGreaterThanNumericToDouble_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                                sNumericDataValue->mLength,
                                                *(dtlDoubleType*)(sDoubleDataValue->mValue),
                                                sDoubleDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief SmallInt vs Numeric의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanSmallIntToNumeric( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanSmallIntToNumeric_INLINE( *((dtlSmallIntType *)(sLeftValue->mValue)),
                                                  sLeftValue->mLength,
                                                  (dtlNumericType *)(sRightValue->mValue),
                                                  sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Integer vs Numeric의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanIntegerToNumeric( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanIntegerToNumeric_INLINE( *((dtlIntegerType *)(sLeftValue->mValue)),
                                                 sLeftValue->mLength,
                                                 (dtlNumericType *)(sRightValue->mValue),
                                                 sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief BigInt vs Numeric의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanBigIntToNumeric( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanBigIntToNumeric_INLINE( *((dtlBigIntType *)(sLeftValue->mValue)),
                                                sLeftValue->mLength,
                                                (dtlNumericType *)(sRightValue->mValue),
                                                sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Real vs Numeric의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanRealToNumeric( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv )
{
    dtlDataValue    * sRealDataValue    = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sNumericDataValue = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsLessThanNumericToReal_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                           sNumericDataValue->mLength,
                                           *(dtlRealType*)(sRealDataValue->mValue),
                                           sRealDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief Double vs Numeric 의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanDoubleToNumeric( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    dtlDataValue    * sDoubleDataValue  = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sNumericDataValue = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsLessThanNumericToDouble_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                             sNumericDataValue->mLength,
                                             *(dtlDoubleType*)(sDoubleDataValue->mValue),
                                             sDoubleDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief numeric type간의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanNumericToNumeric( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    stlUInt8  * sLeftDigit  = (stlUInt8*) DTF_NUMERIC( aInputArgument[0].mValue.mDataValue );
    stlUInt8  * sRightDigit = (stlUInt8*) DTF_NUMERIC( aInputArgument[1].mValue.mDataValue );
    stlInt8   * sTable;
    stlInt32    i = 0;

    if( sLeftDigit[0] == sRightDigit[0] )
    {
        sTable = dtlNumericLenCompTable
            [ aInputArgument[0].mValue.mDataValue->mLength ]
            [ aInputArgument[1].mValue.mDataValue->mLength ];

        if( sTable[1] == 0 )
        {
            if( sLeftDigit[0] > 127 )
            {
                DTL_BOOLEAN( aResultValue ) = ( sTable[0] == 1 );
            }
            else
            {
                DTL_BOOLEAN( aResultValue ) = ( sTable[0] == -1 );
            }

            DTL_SET_BOOLEAN_LENGTH( aResultValue );
                
            return STL_SUCCESS;
        }

        for( i = 1 ; sLeftDigit[i] == sRightDigit[i] ; i++ )
        {
            if( i == sTable[1] )
            {
                if( sLeftDigit[0] > 127 )
                {
                    DTL_BOOLEAN( aResultValue ) = ( sTable[0] == 1 );
                }
                else
                {
                    DTL_BOOLEAN( aResultValue ) = ( sTable[0] == -1 );
                }

                DTL_SET_BOOLEAN_LENGTH( aResultValue );
                
                return STL_SUCCESS;
            }
        }
    }

    DTL_BOOLEAN( aResultValue ) = ( sLeftDigit[i] > sRightDigit[i] );
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}

/** @} dtlNumericType */

/*******************************************************************************
 * TYPE : IS GREATER THAN EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlNumericType
 * <H2> Numeric Type : IS GREATER THAN EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_SMALLINT           </th>
 *     <th> DTL_TYPE_INTEGER            </th>
 *     <th> DTL_TYPE_BIGINT             </th>
 *     <th> DTL_TYPE_REAL               </th>
 *     <th> DTL_TYPE_DOUBLE             </th>
 *     <th> DTL_TYPE_FLOAT              </th>
 *     <th> DTL_TYPE_NUMBER             </th>
 *     <th> DTL_TYPE_NUMBER             </th>
 *     <th> DTL_TYPE_DECIMAL            </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_FLOAT                  </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <td> dtlOperIsGreaterThanEqualNumericToSmallInt   </td>
 *     <td> dtlOperIsGreaterThanEqualNumericToInteger    </td>
 *     <td> dtlOperIsGreaterThanEqualNumericToBigInt     </td>
 *     <td> dtlOperIsGreaterThanEqualNumericToReal       </td>
 *     <td> dtlOperIsGreaterThanEqualNumericToDouble     </td>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsGreaterThanEqualNumericToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                  </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DECIMAL                 </th>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *     <td align=center> /                   </td>
 *   </tr>
 * </table>
 * <BR>
 * <table>
 *   <tr>
 *     <td align=center> from \ to            </td>
 *     <th> DTL_TYPE_FLOAT                    </th>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <th> DTL_TYPE_DECIMAL                  </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsGreaterThanEqualSmallIntToNumeric   </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_INTEGER                  </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsGreaterThanEqualIntegerToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BIGINT                   </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsGreaterThanEqualBigIntToNumeric     </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsGreaterThanEqualRealToNumeric       </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_SMALLINT                 </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsGreaterThanEqualDoubleToNumeric     </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_FLOAT                    </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <td align=center> /                    </td>
 *     <td> dtlOperIsGreaterThanEqualNumericToNumeric    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_NUMBER                   </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_DECIMAL                  </th>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *     <td align=center> /                    </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief Numeric vs SmallInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualNumericToSmallInt( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanEqualSmallIntToNumeric_INLINE( *((dtlSmallIntType *)(sRightValue->mValue)),
                                                    sRightValue->mLength,
                                                    (dtlNumericType *)(sLeftValue->mValue),
                                                    sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Integer의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualNumericToInteger( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanEqualIntegerToNumeric_INLINE( *((dtlIntegerType *)(sRightValue->mValue)),
                                                   sRightValue->mLength,
                                                   (dtlNumericType *)(sLeftValue->mValue),
                                                   sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs BigInt의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualNumericToBigInt( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void          * aVectorArg,
                                                    void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanEqualBigIntToNumeric_INLINE( *((dtlBigIntType *)(sRightValue->mValue)),
                                                  sRightValue->mLength,
                                                  (dtlNumericType *)(sLeftValue->mValue),
                                                  sLeftValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Real의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualNumericToReal( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    dtlDataValue    * sNumericDataValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sRealDataValue    = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsGreaterThanEqualNumericToReal_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                                   sNumericDataValue->mLength,
                                                   *(dtlRealType*)(sRealDataValue->mValue),
                                                   sRealDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief Numeric vs Double의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualNumericToDouble( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv )
{
    dtlDataValue    * sNumericDataValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sDoubleDataValue  = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsGreaterThanEqualNumericToDouble_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                                     sNumericDataValue->mLength,
                                                     *(dtlDoubleType*)(sDoubleDataValue->mValue),
                                                     sDoubleDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief SmallInt vs Numeric의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualSmallIntToNumeric( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanEqualSmallIntToNumeric_INLINE( *((dtlSmallIntType *)(sLeftValue->mValue)),
                                                       sLeftValue->mLength,
                                                       (dtlNumericType *)(sRightValue->mValue),
                                                       sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Integer vs Numeric의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualIntegerToNumeric( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanEqualIntegerToNumeric_INLINE(  *((dtlIntegerType *)(sLeftValue->mValue)),
                                                       sLeftValue->mLength,
                                                       (dtlNumericType *)(sRightValue->mValue),
                                                       sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief BigInt vs Numeric의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualBigIntToNumeric( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv )
{
    dtlDataValue * sLeftValue = aInputArgument[0].mValue.mDataValue;
    dtlDataValue * sRightValue = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanEqualBigIntToNumeric_INLINE( *((dtlBigIntType *)(sLeftValue->mValue)),
                                                     sLeftValue->mLength,
                                                     (dtlNumericType *)(sRightValue->mValue),
                                                     sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/**
 * @brief Real vs Numeric의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualRealToNumeric( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    dtlDataValue    * sRealDataValue    = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sNumericDataValue = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsLessThanEqualNumericToReal_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                                sNumericDataValue->mLength,
                                                *(dtlRealType*)(sRealDataValue->mValue),
                                                sRealDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief Double vs Numeric 의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualDoubleToNumeric( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv )
{
    dtlDataValue    * sDoubleDataValue  = aInputArgument[0].mValue.mDataValue;
    dtlDataValue    * sNumericDataValue = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) = 
        dtlIsLessThanEqualNumericToDouble_INLINE( (dtlNumericType*)(sNumericDataValue->mValue),
                                                  sNumericDataValue->mLength,
                                                  *(dtlDoubleType*)(sDoubleDataValue->mValue),
                                                  sDoubleDataValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}


/**
 * @brief numeric type간의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualNumericToNumeric( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv )
{
    stlUInt8  * sLeftDigit  = (stlUInt8*) DTF_NUMERIC( aInputArgument[0].mValue.mDataValue );
    stlUInt8  * sRightDigit = (stlUInt8*) DTF_NUMERIC( aInputArgument[1].mValue.mDataValue );
    stlInt8   * sTable;
    stlInt32    i = 0;

    if( sLeftDigit[0] == sRightDigit[0] )
    {
        sTable = dtlNumericLenCompTable
            [ aInputArgument[0].mValue.mDataValue->mLength ]
            [ aInputArgument[1].mValue.mDataValue->mLength ];

        if( sTable[1] == 0 )
        {
            if( sLeftDigit[0] > 127 )
            {
                DTL_BOOLEAN( aResultValue ) = ( sTable[0] != -1 );
            }
            else
            {
                DTL_BOOLEAN( aResultValue ) = ( sTable[0] != 1 );
            }

            DTL_SET_BOOLEAN_LENGTH( aResultValue );
                
            return STL_SUCCESS;
        }

        for( i = 1 ; sLeftDigit[i] == sRightDigit[i] ; i++ )
        {
            if( i == sTable[1] )
            {
                if( sLeftDigit[0] > 127 )
                {
                    DTL_BOOLEAN( aResultValue ) = ( sTable[0] != -1 );
                }
                else
                {
                    DTL_BOOLEAN( aResultValue ) = ( sTable[0] != 1 );
                }

                DTL_SET_BOOLEAN_LENGTH( aResultValue );
                
                return STL_SUCCESS;
            }
        }
    }

    DTL_BOOLEAN( aResultValue ) = ( sLeftDigit[i] > sRightDigit[i] );
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
   
    return STL_SUCCESS;
}

/** @} dtlNumericType */

