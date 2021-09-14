/*******************************************************************************
 * dtmBinaryString.c
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
 * @file dtmBinaryString.c
 * @brief DataType Layer Binary String의 비교 연산
 */

#include <dtl.h>

#include <dtdDataType.h>


/**
 * @defgroup dtlBinaryStringType Binary String Type간의 비교 연산
 * <BR>
 * <H2> Binary String Types </H2> 
 * <BR> - Binary / Varbinary / Longvarbinary 간의 비교 연산 지원
 * <BR> - Blob 과의 비교 연산은 지원하지 않음
 * <BR>
 * <BR>
 * <H2> < binary 또는 varying binary의 비교 > </H2>
 * <BR>
 * <BR> binary타입은 padding을 붙여 평가하는 type이
 * <BR> 
 * <BR> 두 값의 짧은 길이값을 기준으로 memcmp한 값이 "동일"할 경우,
 * <BR> 두 값의 길이가 같으면 동일한 값이지만, 
 * <BR> 그렇지 않은 경우 아래와 같은 규칙을 따른다.
 * <BR> 
 * <BR> (long varying binary는 varbinary와 동일하게 취급)
 * <BR> 
 * <BR>  
 * <BR>    length가 큰 쪽 | length가 작은 쪽 |            결과 
 * <BR>  -----------------------------------------------------------------------------
 * <BR>  1)  binary1      |    binary2       | length가 큰 쪽의 나머지 부분 평가하여
 * <BR>                                      | 모두 0x00이면, binary1    = binary2
 * <BR>                   |                  | 그렇지 않으면, binary1    > binary2
 * <BR>  2)  binary       |    varbinary     | 항상,          binary     > varbinary
 * <BR>  3)  varbinary    |    binary        | 항상,          varbinary  > binary
 * <BR>  4)  varbinary1   |    varbinary2    | 항상,          varbinary1 > varbinary2
 * <BR> 
 * <BR>  1)의 경우에만 length가 큰 쪽의 값을 평가한다.
 * <BR>
 * <BR>
 * @ingroup dtlOperCompareType
 * @{
 *
 * @} dtlBinaryStringType */

/*******************************************************************************
 * TYPE : IS EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryStringType
 * <H2> Binary String Type : IS EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_BINARY               </th>
 *     <th> DTL_TYPE_VARBINARY            </th>
 *     <th> DTL_TYPE_LONGVARBINARY        </th>
 *     <th> DTL_TYPE_BLOB               </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BINARY               </th>
 *     <td> dtlOperIsEqualFixedLengthBinary    </td>
 *     <td> dtlOperIsEqualVariableLengthBinary </td>
 *     <td> dtlOperIsEqualVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_VARBINARY            </th>
 *     <td> dtlOperIsEqualVariableLengthBinary </td>
 *     <td> dtlOperIsEqualVariableLengthBinary </td>
 *     <td> dtlOperIsEqualVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_LONGVARBINARY        </th>
 *     <td> dtlOperIsEqualVariableLengthBinary </td>
 *     <td> dtlOperIsEqualVariableLengthBinary </td>
 *     <td> dtlOperIsEqualVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BLOB               </th>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief 고정 길이 binary string type간의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualFixedLengthBinary( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) =
        dtlIsEqualFixedLengthBinary_INLINE( (dtlBinaryType)(sLeftValue->mValue),
                                            sLeftValue->mLength,
                                            (dtlBinaryType)(sRightValue->mValue),
                                            sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief 가변 길이 binary string type을 포함한 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualVariableLengthBinary( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     == 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualBinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     == 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualLongvarbinaryAndBinary( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     == 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualVarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     == 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualLongvarbinaryAndVarbinary( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     == 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualLongvarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                       dtlValueEntry  * aInputArgument,
                                                       void           * aResultValue,
                                                       void           * aInfo,
                                                       dtlFuncVector  * aVectorFunc,
                                                       void           * aVectorArg,
                                                       void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     == 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryStringType */

/*******************************************************************************
 * TYPE : IS NOT EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryStringType
 * <H2> Binary String Type : IS NOT EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_BINARY               </th>
 *     <th> DTL_TYPE_VARBINARY            </th>
 *     <th> DTL_TYPE_LONGVARBINARY        </th>
 *     <th> DTL_TYPE_BLOB               </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BINARY               </th>
 *     <td> dtlOperIsNotEqualFixedLengthBinary    </td>
 *     <td> dtlOperIsNotEqualVariableLengthBinary </td>
 *     <td> dtlOperIsNotEqualVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_VARBINARY            </th>
 *     <td> dtlOperIsNotEqualVariableLengthBinary </td>
 *     <td> dtlOperIsNotEqualVariableLengthBinary </td>
 *     <td> dtlOperIsNotEqualVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_LONGVARBINARY        </th>
 *     <td> dtlOperIsNotEqualVariableLengthBinary </td>
 *     <td> dtlOperIsNotEqualVariableLengthBinary </td>
 *     <td> dtlOperIsNotEqualVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BLOB               </th>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief 고정 길이 binary string type간의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualFixedLengthBinary( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsNotEqualFixedLengthBinary_INLINE( (dtlBinaryType)(sLeftValue->mValue),
                                               sLeftValue->mLength,
                                               (dtlBinaryType)(sRightValue->mValue),
                                               sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}

/**
 * @brief 가변 길이 binary string type을 포함한 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualVariableLengthBinary( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     != 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_TRUE;
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualBinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     != 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_TRUE;
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualLongvarbinaryAndBinary( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     != 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_TRUE;
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualVarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     != 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_TRUE;
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualLongvarbinaryAndVarbinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     != 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_TRUE;
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualLongvarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                          dtlValueEntry  * aInputArgument,
                                                          void           * aResultValue,
                                                          void           * aInfo,
                                                          dtlFuncVector  * aVectorFunc,
                                                          void           * aVectorArg,
                                                          void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     != 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_TRUE;
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryStringType */

/*******************************************************************************
 * TYPE : IS LESS THAN
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryStringType
 * <H2> Binary String Type : IS LESS THAN </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_BINARY               </th>
 *     <th> DTL_TYPE_VARBINARY            </th>
 *     <th> DTL_TYPE_LONGVARBINARY        </th>
 *     <th> DTL_TYPE_BLOB               </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BINARY               </th>
 *     <td> dtlOperIsLessThanFixedLengthBinary    </td>
 *     <td> dtlOperIsLessThanVariableLengthBinary </td>
 *     <td> dtlOperIsLessThanVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_VARBINARY            </th>
 *     <td> dtlOperIsLessThanVariableLengthBinary </td>
 *     <td> dtlOperIsLessThanVariableLengthBinary </td>
 *     <td> dtlOperIsLessThanVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_LONGVARBINARY        </th>
 *     <td> dtlOperIsLessThanVariableLengthBinary </td>
 *     <td> dtlOperIsLessThanVariableLengthBinary </td>
 *     <td> dtlOperIsLessThanVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BLOB               </th>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief 고정 길이 binary string type간의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanFixedLengthBinary( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanFixedLengthBinary_INLINE( (dtlBinaryType)(sLeftValue->mValue),
                                               sLeftValue->mLength,
                                               (dtlBinaryType)(sRightValue->mValue),
                                               sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}

/**
 * @brief 가변 길이 binary string type을 포함한 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanVariableLengthBinary( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen < sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     < 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanBinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen < sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     < 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanLongvarbinaryAndBinary( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen < sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     < 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanVarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen < sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     < 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanLongvarbinaryAndVarbinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen < sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     < 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanLongvarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                          dtlValueEntry  * aInputArgument,
                                                          void           * aResultValue,
                                                          void           * aInfo,
                                                          dtlFuncVector  * aVectorFunc,
                                                          void           * aVectorArg,
                                                          void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen < sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     < 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryStringType */

/*******************************************************************************
 * TYPE : IS LESS THAN EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryStringType
 * <H2> Binary String Type : IS LESS THAN EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_BINARY               </th>
 *     <th> DTL_TYPE_VARBINARY            </th>
 *     <th> DTL_TYPE_LONGVARBINARY        </th>
 *     <th> DTL_TYPE_BLOB               </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BINARY               </th>
 *     <td> dtlOperIsLessThanEqualFixedLengthBinary    </td>
 *     <td> dtlOperIsLessThanEqualVariableLengthBinary </td>
 *     <td> dtlOperIsLessThanEqualVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_VARBINARY            </th>
 *     <td> dtlOperIsLessThanEqualVariableLengthBinary </td>
 *     <td> dtlOperIsLessThanEqualVariableLengthBinary </td>
 *     <td> dtlOperIsLessThanEqualVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_LONGVARBINARY        </th>
 *     <td> dtlOperIsLessThanEqualVariableLengthBinary </td>
 *     <td> dtlOperIsLessThanEqualVariableLengthBinary </td>
 *     <td> dtlOperIsLessThanEqualVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BLOB               </th>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief 고정 길이 binary string type간의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualFixedLengthBinary( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanEqualFixedLengthBinary_INLINE( (dtlBinaryType)(sLeftValue->mValue),
                                                    sLeftValue->mLength,
                                                    (dtlBinaryType)(sRightValue->mValue),
                                                    sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}

/**
 * @brief 가변 길이 binary string type을 포함한 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualVariableLengthBinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen <= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     < 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualBinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aInfo,
                                                        dtlFuncVector  * aVectorFunc,
                                                        void           * aVectorArg,
                                                        void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen <= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     < 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualLongvarbinaryAndBinary( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aInfo,
                                                        dtlFuncVector  * aVectorFunc,
                                                        void           * aVectorArg,
                                                        void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen <= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     < 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualVarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                           dtlValueEntry  * aInputArgument,
                                                           void           * aResultValue,
                                                           void           * aInfo,
                                                           dtlFuncVector  * aVectorFunc,
                                                           void           * aVectorArg,
                                                           void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen <= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     < 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualLongvarbinaryAndVarbinary( stlUInt16        aInputArgumentCnt,
                                                           dtlValueEntry  * aInputArgument,
                                                           void           * aResultValue,
                                                           void           * aInfo,
                                                           dtlFuncVector  * aVectorFunc,
                                                           void           * aVectorArg,
                                                           void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen <= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     < 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualLongvarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                               dtlValueEntry  * aInputArgument,
                                                               void           * aResultValue,
                                                               void           * aInfo,
                                                               dtlFuncVector  * aVectorFunc,
                                                               void           * aVectorArg,
                                                               void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen <= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     < 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryStringType */

/*******************************************************************************
 * TYPE : IS GREATER THAN
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryStringType
 * <H2> Binary String Type : IS GREATER THAN </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_BINARY               </th>
 *     <th> DTL_TYPE_VARBINARY            </th>
 *     <th> DTL_TYPE_LONGVARBINARY        </th>
 *     <th> DTL_TYPE_BLOB               </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BINARY               </th>
 *     <td> dtlOperIsGreaterThanFixedLengthBinary    </td>
 *     <td> dtlOperIsGreaterThanVariableLengthBinary </td>
 *     <td> dtlOperIsGreaterThanVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_VARBINARY            </th>
 *     <td> dtlOperIsGreaterThanVariableLengthBinary </td>
 *     <td> dtlOperIsGreaterThanVariableLengthBinary </td>
 *     <td> dtlOperIsGreaterThanVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_LONGVARBINARY        </th>
 *     <td> dtlOperIsGreaterThanVariableLengthBinary </td>
 *     <td> dtlOperIsGreaterThanVariableLengthBinary </td>
 *     <td> dtlOperIsGreaterThanVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BLOB               </th>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief 고정 길이 binary string type간의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanFixedLengthBinary( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;

    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanFixedLengthBinary_INLINE( (dtlBinaryType)(sLeftValue->mValue),
                                                  sLeftValue->mLength,
                                                  (dtlBinaryType)(sRightValue->mValue),
                                                  sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}

/**
 * @brief 가변 길이 binary string type을 포함한 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanVariableLengthBinary( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen > sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     >= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     > 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanBinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen > sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     >= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     > 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanLongvarbinaryAndBinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen > sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     >= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     > 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanVarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                         dtlValueEntry  * aInputArgument,
                                                         void           * aResultValue,
                                                         void           * aInfo,
                                                         dtlFuncVector  * aVectorFunc,
                                                         void           * aVectorArg,
                                                         void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen > sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     >= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     > 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanLongvarbinaryAndVarbinary( stlUInt16        aInputArgumentCnt,
                                                         dtlValueEntry  * aInputArgument,
                                                         void           * aResultValue,
                                                         void           * aInfo,
                                                         dtlFuncVector  * aVectorFunc,
                                                         void           * aVectorArg,
                                                         void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen > sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     >= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     > 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanLongvarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                             dtlValueEntry  * aInputArgument,
                                                             void           * aResultValue,
                                                             void           * aInfo,
                                                             dtlFuncVector  * aVectorFunc,
                                                             void           * aVectorArg,
                                                             void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen > sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sRightValueLen )
                     >= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp(
                         sLeftValue->mValue,
                         sRightValue->mValue,
                         sLeftValueLen )
                     > 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryStringType */

/*******************************************************************************
 * TYPE : IS GREATER THAN EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlBinaryStringType
 * <H2> Binary String Type : IS GREATER THAN EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_BINARY               </th>
 *     <th> DTL_TYPE_VARBINARY            </th>
 *     <th> DTL_TYPE_LONGVARBINARY        </th>
 *     <th> DTL_TYPE_BLOB               </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BINARY               </th>
 *     <td> dtlOperIsGreaterThanEqualFixedLengthBinary    </td>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthBinary </td>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_VARBINARY            </th>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthBinary </td>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthBinary </td>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_LONGVARBINARY        </th>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthBinary </td>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthBinary </td>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthBinary </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_BLOB               </th>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *     <td align=center> /              </td>
 *   </tr>
 * </table>
 * <BR>
 * @{
 */

/**
 * @brief 고정 길이 binary string type간의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualFixedLengthBinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    
    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanEqualFixedLengthBinary_INLINE( (dtlBinaryType)(sLeftValue->mValue),
                                                       sLeftValue->mLength,
                                                       (dtlBinaryType)(sRightValue->mValue),
                                                       sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}

/**
 * @brief 가변 길이 binary string type을 포함한 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualVariableLengthBinary( stlUInt16        aInputArgumentCnt,
                                                         dtlValueEntry  * aInputArgument,
                                                         void           * aResultValue,
                                                         void           * aInfo,
                                                         dtlFuncVector  * aVectorFunc,
                                                         void           * aVectorArg,
                                                         void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen >= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        >= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        > 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualBinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                           dtlValueEntry  * aInputArgument,
                                                           void           * aResultValue,
                                                           void           * aInfo,
                                                           dtlFuncVector  * aVectorFunc,
                                                           void           * aVectorArg,
                                                           void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen >= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        >= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        > 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualLongvarbinaryAndBinary( stlUInt16        aInputArgumentCnt,
                                                           dtlValueEntry  * aInputArgument,
                                                           void           * aResultValue,
                                                           void           * aInfo,
                                                           dtlFuncVector  * aVectorFunc,
                                                           void           * aVectorArg,
                                                           void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen >= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        >= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        > 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualVarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                              dtlValueEntry  * aInputArgument,
                                                              void           * aResultValue,
                                                              void           * aInfo,
                                                              dtlFuncVector  * aVectorFunc,
                                                              void           * aVectorArg,
                                                              void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen >= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        >= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        > 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualLongvarbinaryAndVarbinary( stlUInt16        aInputArgumentCnt,
                                                              dtlValueEntry  * aInputArgument,
                                                              void           * aResultValue,
                                                              void           * aInfo,
                                                              dtlFuncVector  * aVectorFunc,
                                                              void           * aVectorArg,
                                                              void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen >= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        >= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        > 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable binary string type을 포함한 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualLongvarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                                  dtlValueEntry  * aInputArgument,
                                                                  void           * aResultValue,
                                                                  void           * aInfo,
                                                                  dtlFuncVector  * aVectorFunc,
                                                                  void           * aVectorArg,
                                                                  void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    stlInt64         sLeftValueLen  = sLeftValue->mLength;
    stlInt64         sRightValueLen = sRightValue->mLength;

    if( sLeftValueLen >= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        >= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        > 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlBinaryStringType */
