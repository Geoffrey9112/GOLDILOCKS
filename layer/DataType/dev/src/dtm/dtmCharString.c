/*******************************************************************************
 * dtmCharString.c
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
 * @file dtmCharString.c
 * @brief DataType Layer Character String의 비교 연산
 */

#include <dtl.h>

#include <dtdDataType.h>

/**
 * @defgroup dtlCharStringType Character String Type간의 비교 연산
 * <BR>
 * <H2> Character String Types </H2> 
 * <BR> - Char / Varchar / Longvarchar 간의 비교 연산 지원
 * <BR> - Clob 과의 비교 연산은 지원하지 않음
 * <BR>
 * <BR>
 * @ingroup dtlOperCompareType
 * @{
 *
 * @} dtlCharStringType */

/*******************************************************************************
 * TYPE : IS EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlCharStringType
 * <H2> Character String Type : IS EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_CHAR               </th>
 *     <th> DTL_TYPE_VARCHAR            </th>
 *     <th> DTL_TYPE_LONGVARCHAR        </th>
 *     <th> DTL_TYPE_CLOB               </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_CHAR               </th>
 *     <td> dtlOperIsEqualFixedLengthChar    </td>
 *     <td> dtlOperIsEqualVariableLengthChar </td>
 *     <td> dtlOperIsEqualVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_VARCHAR            </th>
 *     <td> dtlOperIsEqualVariableLengthChar </td>
 *     <td> dtlOperIsEqualVariableLengthChar </td>
 *     <td> dtlOperIsEqualVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_LONGVARCHAR        </th>
 *     <td> dtlOperIsEqualVariableLengthChar </td>
 *     <td> dtlOperIsEqualVariableLengthChar </td>
 *     <td> dtlOperIsEqualVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_CLOB               </th>
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
 * @brief 고정 길이 character string type간의IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualFixedLengthChar( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;

    /**
     * @todo locale 지원여부가 ... 
     */

    DTL_BOOLEAN( aResultValue ) =
        dtlIsEqualFixedLengthChar_INLINE( (dtlCharType)(sLeftValue->mValue),
                                          sLeftValue->mLength,
                                          (dtlCharType)(sRightValue->mValue),
                                          sRightValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief 가변 길이 character string type을 포함한 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualVariableLengthChar( stlUInt16        aInputArgumentCnt,
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
    
    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
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
 * @brief long variable character string type을 포함한 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualCharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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
    
    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
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
 * @brief long variable character string type을 포함한 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualLongvarcharAndChar( stlUInt16        aInputArgumentCnt,
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
    
    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
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
 * @brief long variable character string type을 포함한 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualVarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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
    
    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
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
 * @brief long variable character string type을 포함한 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualLongvarcharAndVarchar( stlUInt16        aInputArgumentCnt,
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
    
    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
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
 * @brief long variable character string type을 포함한 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualLongvarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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
    
    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        == 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
    }
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlCharStringType */

/*******************************************************************************
 * TYPE : IS NOT EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlCharStringType
 * <H2> Character String Type : IS NOT EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_CHAR               </th>
 *     <th> DTL_TYPE_VARCHAR            </th>
 *     <th> DTL_TYPE_LONGVARCHAR        </th>
 *     <th> DTL_TYPE_CLOB               </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_CHAR               </th>
 *     <td> dtlOperIsNotEqualFixedLengthChar    </td>
 *     <td> dtlOperIsNotEqualVariableLengthChar </td>
 *     <td> dtlOperIsNotEqualVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_VARCHAR            </th>
 *     <td> dtlOperIsNotEqualVariableLengthChar </td>
 *     <td> dtlOperIsNotEqualVariableLengthChar </td>
 *     <td> dtlOperIsNotEqualVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_LONGVARCHAR        </th>
 *     <td> dtlOperIsNotEqualVariableLengthChar </td>
 *     <td> dtlOperIsNotEqualVariableLengthChar </td>
 *     <td> dtlOperIsNotEqualVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_CLOB               </th>
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
 * @brief 고정 길이 character string type간의IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualFixedLengthChar( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;

    /**
     * @todo locale 지원여부가 ... 
     */

    DTL_BOOLEAN( aResultValue ) =
        dtlIsNotEqualFixedLengthChar_INLINE( (dtlCharType)(sLeftValue->mValue),
                                             sLeftValue->mLength,
                                             (dtlCharType)(sRightValue->mValue),
                                             sRightValue->mLength );
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief 가변 길이 character string type을 포함한 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualVariableLengthChar( stlUInt16        aInputArgumentCnt,
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
    
    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
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
 * @brief long variable character string type을 포함한 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualCharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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
    
    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
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
 * @brief long variable character string type을 포함한 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualLongvarcharAndChar( stlUInt16        aInputArgumentCnt,
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
    
    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
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
 * @brief long variable character string type을 포함한 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualVarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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
    
    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
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
 * @brief long variable character string type을 포함한 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualLongvarcharAndVarchar( stlUInt16        aInputArgumentCnt,
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
    
    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
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
 * @brief long variable character string type을 포함한 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualLongvarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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
    
    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen == sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        != 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_TRUE;
    }
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlCharStringType */

/*******************************************************************************
 * TYPE : IS LESS THAN
 ******************************************************************************/
/**
 * @addtogroup dtlCharStringType
 * <H2> Character String Type : IS LESS THAN </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_CHAR               </th>
 *     <th> DTL_TYPE_VARCHAR            </th>
 *     <th> DTL_TYPE_LONGVARCHAR        </th>
 *     <th> DTL_TYPE_CLOB               </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_CHAR               </th>
 *     <td> dtlOperIsLessThanFixedLengthChar    </td>
 *     <td> dtlOperIsLessThanVariableLengthChar </td>
 *     <td> dtlOperIsLessThanVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_VARCHAR            </th>
 *     <td> dtlOperIsLessThanVariableLengthChar </td>
 *     <td> dtlOperIsLessThanVariableLengthChar </td>
 *     <td> dtlOperIsLessThanVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_LONGVARCHAR        </th>
 *     <td> dtlOperIsLessThanVariableLengthChar </td>
 *     <td> dtlOperIsLessThanVariableLengthChar </td>
 *     <td> dtlOperIsLessThanVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_CLOB               </th>
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
 * @brief 고정 길이 character string type간의IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanFixedLengthChar( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;

    /**
     * @todo locale 지원여부가 ... 
     */

    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanFixedLengthChar_INLINE( (dtlCharType)(sLeftValue->mValue),
                                             sLeftValue->mLength,
                                             (dtlCharType)(sRightValue->mValue),
                                             sRightValue->mLength );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief 가변 길이 character string type을 포함한 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanVariableLengthChar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen < sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        < 0 );
    }
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable character string type을 포함한 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanCharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen < sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        < 0 );
    }
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable character string type을 포함한 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanLongvarcharAndChar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen < sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        < 0 );
    }
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable character string type을 포함한 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanVarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen < sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        < 0 );
    }
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable character string type을 포함한 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanLongvarcharAndVarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen < sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        < 0 );
    }
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable character string type을 포함한 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanLongvarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen < sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        < 0 );
    }
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlCharStringType */

/*******************************************************************************
 * TYPE : IS LESS THAN EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlCharStringType
 * <H2> Character String Type : IS LESS THAN EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_CHAR               </th>
 *     <th> DTL_TYPE_VARCHAR            </th>
 *     <th> DTL_TYPE_LONGVARCHAR        </th>
 *     <th> DTL_TYPE_CLOB               </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_CHAR               </th>
 *     <td> dtlOperIsLessThanEqualFixedLengthChar    </td>
 *     <td> dtlOperIsLessThanEqualVariableLengthChar </td>
 *     <td> dtlOperIsLessThanEqualVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_VARCHAR            </th>
 *     <td> dtlOperIsLessThanEqualVariableLengthChar </td>
 *     <td> dtlOperIsLessThanEqualVariableLengthChar </td>
 *     <td> dtlOperIsLessThanEqualVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_LONGVARCHAR        </th>
 *     <td> dtlOperIsLessThanEqualVariableLengthChar </td>
 *     <td> dtlOperIsLessThanEqualVariableLengthChar </td>
 *     <td> dtlOperIsLessThanEqualVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_CLOB               </th>
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
 * @brief 고정 길이 character string type간의IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualFixedLengthChar( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;

    /**
     * @todo locale 지원여부가 ... 
     */

    DTL_BOOLEAN( aResultValue ) =
        dtlIsLessThanEqualFixedLengthChar_INLINE( (dtlCharType)(sLeftValue->mValue),
                                                  sLeftValue->mLength,
                                                  (dtlCharType)(sRightValue->mValue),
                                                  sRightValue->mLength );
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief 가변 길이 character string type을 포함한 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualVariableLengthChar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen <= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        < 0 );
    }
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable character string type을 포함한 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualCharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen <= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        < 0 );
    }
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable character string type을 포함한 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualLongvarcharAndChar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen <= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        < 0 );
    }
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable character string type을 포함한 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualVarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen <= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        < 0 );
    }
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable character string type을 포함한 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualLongvarcharAndVarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen <= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        < 0 );
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief long variable character string type을 포함한 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualLongvarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen <= sRightValueLen )
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sLeftValueLen )
                                        <= 0 );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mValue,
                                                   sRightValue->mValue,
                                                   sRightValueLen )
                                        < 0 );
    }
        
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlCharStringType */

/*******************************************************************************
 * TYPE : IS GREATER THAN
 ******************************************************************************/
/**
 * @addtogroup dtlCharStringType
 * <H2> Character String Type : IS GREATER THAN </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_CHAR               </th>
 *     <th> DTL_TYPE_VARCHAR            </th>
 *     <th> DTL_TYPE_LONGVARCHAR        </th>
 *     <th> DTL_TYPE_CLOB               </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_CHAR               </th>
 *     <td> dtlOperIsGreaterThanFixedLengthChar    </td>
 *     <td> dtlOperIsGreaterThanVariableLengthChar </td>
 *     <td> dtlOperIsGreaterThanVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_VARCHAR            </th>
 *     <td> dtlOperIsGreaterThanVariableLengthChar </td>
 *     <td> dtlOperIsGreaterThanVariableLengthChar </td>
 *     <td> dtlOperIsGreaterThanVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_LONGVARCHAR        </th>
 *     <td> dtlOperIsGreaterThanVariableLengthChar </td>
 *     <td> dtlOperIsGreaterThanVariableLengthChar </td>
 *     <td> dtlOperIsGreaterThanVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_CLOB               </th>
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
 * @brief 고정 길이 character string type간의IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanFixedLengthChar( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;

    /**
     * @todo locale 지원여부가 ... 
     */

    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanFixedLengthChar_INLINE( (dtlCharType)(sLeftValue->mValue),
                                                sLeftValue->mLength,
                                                (dtlCharType)(sRightValue->mValue),
                                                sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}

/**
 * @brief 가변 길이 character string type을 포함한 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanVariableLengthChar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen > sRightValueLen )
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
 * @brief long variable character string type을 포함한 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanCharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen > sRightValueLen )
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
 * @brief long variable character string type을 포함한 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanLongvarcharAndChar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen > sRightValueLen )
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
 * @brief long variable character string type을 포함한 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanVarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen > sRightValueLen )
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
 * @brief long variable character string type을 포함한 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanLongvarcharAndVarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen > sRightValueLen )
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
 * @brief long variable character string type을 포함한 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanLongvarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

    if( sLeftValueLen > sRightValueLen )
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

/** @} dtlCharStringType */

/*******************************************************************************
 * TYPE : IS GREATER THAN EQUAL
 ******************************************************************************/
/**
 * @addtogroup dtlCharStringType
 * <H2> Character String Type : IS GREATER THAN EQUAL </H2>
 * <table>
 *   <tr>
 *     <td align=center> from \ to      </td>
 *     <th> DTL_TYPE_CHAR               </th>
 *     <th> DTL_TYPE_VARCHAR            </th>
 *     <th> DTL_TYPE_LONGVARCHAR        </th>
 *     <th> DTL_TYPE_CLOB               </th>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_CHAR               </th>
 *     <td> dtlOperIsGreaterThanEqualFixedLengthChar    </td>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthChar </td>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_VARCHAR            </th>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthChar </td>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthChar </td>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_LONGVARCHAR        </th>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthChar </td>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthChar </td>
 *     <td> dtlOperIsGreaterThanEqualVariableLengthChar </td>
 *     <td align=center> /              </td>
 *   </tr>
 *   <tr>
 *     <th> DTL_TYPE_CLOB               </th>
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
 * @brief 고정 길이 character string type간의IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualFixedLengthChar( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv )
{
    dtlDataValue   * sLeftValue     = aInputArgument[0].mValue.mDataValue;
    dtlDataValue   * sRightValue    = aInputArgument[1].mValue.mDataValue;
    
    /**
     * @todo locale 지원여부가 ... 
     */

    DTL_BOOLEAN( aResultValue ) =
        dtlIsGreaterThanEqualFixedLengthChar_INLINE( (dtlCharType)(sLeftValue->mValue),
                                                     sLeftValue->mLength,
                                                     (dtlCharType)(sRightValue->mValue),
                                                     sRightValue->mLength );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief 가변 길이 character string type을 포함한 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualVariableLengthChar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

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
 * @brief long variable character string type을 포함한 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualCharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

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
 * @brief long variable character string type을 포함한 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualLongvarcharAndChar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

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
 * @brief long variable character string type을 포함한 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualVarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

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
 * @brief long variable character string type을 포함한 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualLongvarcharAndVarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

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
 * @brief long variable character string type을 포함한 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualLongvarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
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

    /**
     * @todo locale 지원여부가 ... 
     */ 

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

/** @} dtlCharStringType */
