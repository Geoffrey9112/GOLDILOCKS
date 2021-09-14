/*******************************************************************************
 * dtfNumeric.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfNumeric.c 1415 2011-07-14 02:48:23Z lym999 $
 *
 * NOTES
 *   이 소스는 postgres를 porting한 수준임.
 *   파라미터 변수 check는 하지 않음.
 *   현재 stack 변수로 DTF_NUMERIC_MAX_DIGITS_COUNT크기의 배열을 많이 사용하고 있음.
 *   postgres에서 alloc인것을 배열로 변경했음.
 *   DTF_NUMERIC_MAX_DIGITS_COUNT가 현재 100임으로 문제가 없는지 확인 필요함.
 *
 ******************************************************************************/

/**
 * @file dtfNumeric.c
 * @brief dtfNumeric Function DataType Library Layer
 */

#include <stlTypes.h>
#include <dtl.h>
#include <dtDef.h>
#include <dtfNumeric.h>


/**
 * @addtogroup dtfNumeric Numeric
 * @ingroup dtf
 * @internal
 * @{
 */

dtlNumericSumFunc gDtlNumericSum[2][2] =
{
    {
        dtfNumericSumNegaNega,
        dtfNumericSum
    },
    {
        dtfNumericSum,
        dtfNumericSumPosPos
    }
};


/**
 * @brief Numeric 구조체에 대한 덧셈 연산
 * @param[in]  aValue1      dtfNumericType
 * @param[in]  aValue2      dtfNumericType
 * @param[out] aResult      덧셈 연산 결과
 * @param[out] aErrorStack  에러 스택
 */
stlStatus dtfNumericAddition( dtlDataValue        * aValue1,
                              dtlDataValue        * aValue2,
                              dtlDataValue        * aResult,
                              stlErrorStack       * aErrorStack )
{
    dtlNumericType  * sNumeric1;
    dtlNumericType  * sNumeric2;
    dtlNumericType  * sResultNumeric;

    stlUInt8          sIsPositive1;
    stlUInt8          sIsPositive2;
    stlInt32          sDigitLen;
    stlInt32          sExponent;
    stlInt32          sExponent1;
    stlInt32          sExponent2;

    stlUInt8        * sResultDigit;
    stlUInt8        * sTrailResultDigit;
    stlUInt8        * sDigit1;
    stlUInt8        * sDigit2;
    stlUInt8          sBlank1;
    stlUInt8          sBlank2;

    stlInt32          sFirstPos1;
    stlInt32          sLastPos1;
    stlInt32          sFirstPos2;
    stlInt32          sLastPos2;
    stlInt32          sCurPos;

    stlBool           sIsGreater;
    stlInt32          sLoop;
    stlInt8         * sTable;

    const dtlNumericCarryInfo  * sAddMap;
    const dtlNumericCarryInfo  * sCarryInfo; 

    STL_DASSERT( ( aValue1->mType == DTL_TYPE_FLOAT ) ||
                 ( aValue1->mType == DTL_TYPE_NUMBER ) );
    STL_DASSERT( ( aValue2->mType == DTL_TYPE_FLOAT ) ||
                 ( aValue2->mType == DTL_TYPE_NUMBER ) );

    
    sNumeric1      = DTF_NUMERIC( aValue1 );
    sNumeric2      = DTF_NUMERIC( aValue2 );
    sResultNumeric = DTF_NUMERIC( aResult );
    
    if( DTL_NUMERIC_IS_ZERO( sNumeric1, aValue1->mLength ) )
    {
        /* copy */
        stlMemcpy( sResultNumeric,
                   sNumeric2,
                   aValue2->mLength );
        aResult->mLength = aValue2->mLength;
        STL_THROW( RAMP_EXIT );
    }

    if( DTL_NUMERIC_IS_ZERO( sNumeric2, aValue2->mLength ) )
    {
        /* copy */
        stlMemcpy( sResultNumeric,
                   sNumeric1,
                   aValue1->mLength );
        aResult->mLength = aValue1->mLength;
        STL_THROW( RAMP_EXIT );
    }

    /**
     * < numeric 간의 Addition 연산 >
     *
     * ## numeric1 과 numeric2의 Exponent를 고려한 Add 방법 구분
     *
     * numeric1의 digit은 'X' 
     * numeric1의 digit은 'Y'
     * numeric2의 평가가 필요 없는 digit은 'O'로 표시
     *
     *              01234567890123456789 (최대 20자리 Digit)
     *              
     * case 1)
     *              A          B
     *  numeric1 :  DTLDTLDTLDTL
     *  numeric2 :        YYYYYYYYYYY
     *                    C         D
     *
     *              A             B
     *  numeric1 :  DTLDTLDTLDTLDTL
     *  numeric2 :           YYYYYYYYYYYOOO
     *                       C         D   
     *
     * case 2)
     *              A      B
     *  numeric1 :  DTLDTLXX
     *  numeric2 :              YYYYY
     *                          C   D
     *
     *              A        B       
     *  numeric1 :  DTLDTLDTLX
     *  numeric2 :              YYYYYYYYYOOO
     *                          C       D
     *
     *              A        B
     *  numeric1 :  DTLDTLDTLX
     *  numeric2 :            YYYYYYYY
     *                        C      D
     *
     *              A        B
     *  numeric1 :  DTLDTLDTLX
     *  numeric2 :            YYYYYYYYYYYOOO
     *                        C         D
     *  
     * case 3)
     *              A     B
     *  numeric1 :  DTLDTLX
     *  numeric2 :  YYYYYYY
     *              C     D
     *  
     *  numeric1 :  DTLDTLDTLDTLDTL
     *  numeric2 :  YYYYYYYYY
     *  ==> 두 numeric간의 위치 변경
     *                  A       B
     *      numeric1 :  DTLDTLDTL
     *      numeric2 :  YYYYYYYYYYYYYYY
     *                  C             D
     *
     *              A     B                  
     *  numeric1 :  DTLDTLX
     *  numeric2 :  YYYYYYYYYYYY
     *              C          D
     *
     *  
     * case 4)
     *              A             B
     *  numeric1 :  DTLDTLDTLDTLDTL
     *  numeric2 :     YYYYYYYYYYYY
     *                 C          D
     *
     *              A             B
     *  numeric1 :  DTLDTLDTLDTLDTL
     *  numeric2 :     YYYYYY
     *                 C    D
     *
     *  
     * ## case 구분 방법 ( 반드시 numeric1은 numeric2보다 최대 exponent가 같거나 크다 )
     * 
     *  1. numeric1의 마지막 digit과 numeric2의 처음 digit과의 exponent 비교
     *     : ( B vs C )
     *  2. numeric1의 마지막 digit과 numeric2의 마지막 digit과의 exponent 비교
     *     : ( B vs D )
     *     
     */

    sExponent1 = DTL_NUMERIC_GET_EXPONENT( sNumeric1 );
    sExponent2 = DTL_NUMERIC_GET_EXPONENT( sNumeric2 );
        
    sIsGreater = STL_TRUE;
    if( sExponent1 > sExponent2 )
    {
        sIsGreater = STL_TRUE;
    }
    else
    {
        if( sExponent1 < sExponent2 )
        {
            sIsGreater = STL_FALSE;
        }
        else
        {
            sDigit1 = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
            sDigit2 = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );

            sTable = dtlNumericLenCompTable[ aValue1->mLength ][ aValue2->mLength ];

            if( DTL_NUMERIC_IS_POSITIVE( sNumeric1 ) ==
                DTL_NUMERIC_IS_POSITIVE( sNumeric2 ) )
            {
                for( sLoop = 0 ; sLoop < sTable[1] ; sLoop++ )
                {
                    if( sDigit1[ sLoop ] != sDigit2[ sLoop ] )
                    {
                        break;
                    }
                }

                if( sLoop == sTable[1] )
                {
                    sIsGreater = ( sTable[0] >= 0 );
                }
                else
                {
                    if( DTL_NUMERIC_IS_POSITIVE( sNumeric1 ) )
                    {
                        sIsGreater = sDigit1[ sLoop ] >= sDigit2[ sLoop ];
                    }
                    else
                    {
                        sIsGreater = sDigit1[ sLoop ] <= sDigit2[ sLoop ];
                    }
                }
            }
            else
            {
                for( sLoop = 0 ; sLoop < sTable[1] ; sLoop++ )
                {
                    if( sDigit1[ sLoop ] !=
                        DTL_NUMERIC_GET_REVERSE_DIGIT_FROM_DIGIT( & sDigit2[ sLoop ] ) )
                    {
                        break;
                    }
                }

                if( sLoop == sTable[1] )
                {
                    if( sTable[0] == 0 )
                    {
                        DTL_NUMERIC_SET_ZERO( sResultNumeric, aResult->mLength );
                        STL_THROW( RAMP_EXIT );
                    }
                    else
                    {
                        sIsGreater = ( sTable[0] >= 0 );
                    }
                }
                else
                {
                    if( DTL_NUMERIC_IS_POSITIVE( sNumeric1 ) )
                    {
                        sIsGreater = ( sDigit1[ sLoop ] >
                                       DTL_NUMERIC_GET_REVERSE_DIGIT_FROM_DIGIT( & sDigit2[ sLoop ] ) );
                    }
                    else
                    {
                        sIsGreater = ( sDigit1[ sLoop ] <
                                       DTL_NUMERIC_GET_REVERSE_DIGIT_FROM_DIGIT( & sDigit2[ sLoop ] ) );
                    }
                }
            }
        }
    }

        
    /**
     * 절대값이 큰 수가 Numeric1이 된다.
     */
        
    if( sIsGreater == STL_FALSE )
    {
        /**
         * Numeric2 + Numeric1
         */

        sDigit1      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );
        sDigit2      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
        sIsPositive1 = DTL_NUMERIC_IS_POSITIVE( sNumeric2 );
        sIsPositive2 = DTL_NUMERIC_IS_POSITIVE( sNumeric1 );

            
        /**
         * 분석 위치 정보 설정
         */
            
        sFirstPos1 = 1;
        sLastPos1  = DTL_NUMERIC_GET_DIGIT_COUNT( aValue2->mLength );
        sFirstPos2 = sExponent2 - sExponent1 + 1;
        sLastPos2  = sFirstPos2 + DTL_NUMERIC_GET_DIGIT_COUNT( aValue1->mLength ) - 1;
        sExponent  = sExponent2;
    }
    else
    {
        /**
         * Numeric1 + Numeric2
         */
            
        sDigit1      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
        sDigit2      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );
        sIsPositive1 = DTL_NUMERIC_IS_POSITIVE( sNumeric1 );
        sIsPositive2 = DTL_NUMERIC_IS_POSITIVE( sNumeric2 );


        /**
         * 분석 위치 정보 설정
         */
            
        sFirstPos1 = 1;
        sLastPos1  = DTL_NUMERIC_GET_DIGIT_COUNT( aValue1->mLength );
        sFirstPos2 = sExponent1 - sExponent2 + 1;
        sLastPos2  = sFirstPos2 + DTL_NUMERIC_GET_DIGIT_COUNT( aValue2->mLength ) - 1;
        sExponent  = sExponent1;
    }
    sResultDigit = DTL_NUMERIC_GET_DIGIT_PTR( sResultNumeric );
        
        
    /**
     * Add 연산 수행
     */
        
    /*
     * Numeric에 대한 분석 위치 정보 ( A ~ D )
     * A : 1 (고정)
     * B : Numeric1's Digit Count
     * C : Numeric1's Exponent - Numeric2's Exponent + 1
     * D : C + Numeric2's Digit Count
     *     단, 최대값은 DTL_NUMERIC_MAX_DIGIT_COUNT
     *
     * case A)  B < C :
     * 
     *    ==>    A ~ B   /   B+1 ~ C-1   /   C ~ D
     *           digit1          0           digit2
     *
     *           DTLDTLDTLXX
     *                      YYYYYYYYY
     *                    
     *     
     * case B)  B >= C :
     * 
     *      B-1)  B > D
     *                    A ~ C-1   /   C ~ D   /   D+1 ~ B
     *                    digit1    digit1+digit2   digit1
     *
     *           DTLDTLDTLXX
     *                YYYYY
     *
     *      B-2)  B <= D
     *                    A ~ C-1   /   C ~ B   /   B+1 ~ D
     *                    digit1    digit1+digit2   digit2
     *
     *           DTLDTLDTLXX
     *                  YYYYYYYYY
     */

        
    /**
     * case 구분 ( case A  vs  case B )
     */

    sAddMap = dtdNumericDigitMap[ sIsPositive1 ][ sIsPositive2 ];
    sBlank1 = dtlNumericZero[ sIsPositive1 ];
    sBlank2 = dtlNumericZero[ sIsPositive2 ];
    sResultNumeric->mData[0] = sBlank1;

    if( sLastPos1 == sLastPos2 )
    {
        sCarryInfo    = dtdNumericInvalidCarryInfo;
        sDigitLen     = sLastPos1;
        sCurPos       = sLastPos1;
        sDigit1      += sLastPos1;
        sDigit2      += sLastPos2 - sFirstPos2 + 1;
        sResultDigit += sDigitLen;
        sTrailResultDigit = sResultDigit - 1;

            
        /**
         * case B-1)의 ( C ~ D ) 구간 평가
         */
                
        while( sCurPos >= sFirstPos2 )
        {
            sCurPos--;
            sDigit1--;
            sDigit2--;
            sResultDigit--;

            sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
            *sResultDigit = sCarryInfo->mDigit;
        }

                
        /**
         * case B-1)의 ( A ~ C-1 ) 구간 평가
         */

        while( sCurPos >= sFirstPos1 )
        {
            sCurPos--;
            sDigit1--;
            sResultDigit--;

            sCarryInfo = & sAddMap[ *sDigit1 + sBlank2 + sCarryInfo->mCarry ];
            *sResultDigit = sCarryInfo->mDigit;
        }
        
        /**
         * Trailing Zero 제거
         */

        if( *sTrailResultDigit == sBlank1 )
        {
            sResultDigit--;
            while( ( sDigitLen > 0 ) &&
                   ( sResultDigit[ sDigitLen ] == sBlank1 ) )
            {
                sDigitLen--;
            }
            sResultDigit++;
            
            if( sDigitLen == 0 )
            {
                if( sCarryInfo->mCarry == 0 )
                {
                    /* set zero */
                    DTL_NUMERIC_SET_ZERO( sResultNumeric, aResult->mLength );
                }
                else
                {
                    sExponent++;
                    aResult->mLength = 2;
                    if( sIsPositive1 )
                    {
                        *sResultDigit = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
                        DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric, sExponent );
                    }
                    else
                    {
                        *sResultDigit = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 1 );
                        DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric, sExponent );
                    }
                }
                
                STL_THROW( RAMP_EXIT );
            }
        }
        
        /**
         * 최상위 Carry 처리
         */

        if( sCarryInfo->mCarry != 0 )
        {
            /**
             * 부호가 같은 경우 : 최상위 digit 추가
             */

            STL_TRY_THROW( ++sExponent <= DTL_NUMERIC_MAX_EXPONENT,
                           ERROR_OUT_OF_RANGE );

            sResultDigit[ -1 ] = DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sIsPositive1, 1 );
            if( sDigitLen == DTL_NUMERIC_MAX_DIGIT_COUNT )
            {
                if( sIsPositive1 )
                {
                    sDigitLen--;
                    sCarryInfo  =
                        & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sResultDigit[ sDigitLen ] ) > 49 ];

                    if( sCarryInfo->mCarry == 0 )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        while( sCarryInfo->mCarry != 0 )
                        {
                            sDigitLen--;
                            sCarryInfo = & sAddMap[ sResultDigit[ sDigitLen ] + sBlank1 + sCarryInfo->mCarry ];
                            sResultDigit[ sDigitLen ] = sCarryInfo->mDigit;
                        }
                        sDigitLen++;
                    }
                }
                else
                {
                    sDigitLen--;
                    sCarryInfo  =
                        & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sResultDigit[ sDigitLen ] ) > 49 ];

                    if( sCarryInfo->mCarry == 0 )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        while( sCarryInfo->mCarry != 0 )
                        {
                            sDigitLen--;
                            sCarryInfo = & sAddMap[ sResultDigit[ sDigitLen ] + sBlank1 + sCarryInfo->mCarry ];
                            sResultDigit[ sDigitLen ] = sCarryInfo->mDigit;
                        }
                        sDigitLen++;
                    }
                }
            }

            STL_DASSERT( sDigitLen >= 0 );
            aResult->mLength = sDigitLen + 2;
                
            sResultDigit += sDigitLen;
            while( sDigitLen >= 0 )
            {
                sResultDigit--;
                sResultDigit[ 1 ] = sResultDigit[ 0 ];
                sDigitLen--;
            }

            DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sResultNumeric, sIsPositive1, sExponent );
        }
        else
        {
            /**
             * Leading Zero 확인
             */

            if( sIsPositive1 != sIsPositive2 )
            {
                sCurPos = 0;
                while( sResultDigit[ sCurPos ] == sBlank1 )
                {
                    sCurPos++;
                    if( --sDigitLen == 0 )
                    {
                        /* set zero */
                        DTL_NUMERIC_SET_ZERO( sResultNumeric, aResult->mLength );
                        STL_THROW( RAMP_EXIT );
                    }
                }

                if( sCurPos > 0 )
                {
                    /**
                     * Leading Zero 제거
                     */

                    aResult->mLength = sDigitLen + 1;

                    while( sDigitLen > 0 )
                    {
                        *sResultDigit = sResultDigit[ sCurPos ];
                        sResultDigit++;
                        sDigitLen--;
                    }

                    if( sIsPositive1 )
                    {
                        DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                    sExponent - sCurPos );
                    }
                    else
                    {
                        DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                    sExponent - sCurPos );
                    }
                }
                else
                {
                    aResult->mLength = sDigitLen + 1;

                    if( sIsPositive1 )
                    {
                        DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                    sExponent );
                    }
                    else
                    {
                        DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                    sExponent );
                    }
                }
            }
            else
            {
                aResult->mLength = sDigitLen + 1;

                if( sIsPositive1 )
                {
                    DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent );
                }
                else
                {
                    DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent );
                }
            }
        }
    }
    else
    {
        if( sLastPos1 < sFirstPos2 )
        {
            /* CASE A */
            if( sLastPos2 > DTL_NUMERIC_MAX_DIGIT_COUNT )
            {
                sLastPos2  = DTL_NUMERIC_MAX_DIGIT_COUNT;
                sDigit2    += DTL_NUMERIC_MAX_DIGIT_COUNT - sFirstPos2 + 1;

                sCarryInfo  =
                    & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sIsPositive2, *sDigit2 ) > 49 ];
                
                sDigitLen  = sLastPos2;
                sCurPos    = sLastPos2;
            }
            else
            {
                sCarryInfo = dtdNumericInvalidCarryInfo;
                sDigit2   += sLastPos2 - sFirstPos2 + 1;
                sDigitLen  = sLastPos2;
                sCurPos    = sLastPos2;
            }
            sDigit1      += sLastPos1;
            sResultDigit += sDigitLen;
            
                
            /**
             * case A)의 ( C ~ D ) 구간 평가
             */

            while( sCurPos >= sFirstPos2 )
            {
                sCurPos--;
                sDigit2--;
                sResultDigit--;

                sCarryInfo = & sAddMap[ *sDigit2 + sBlank1 + sCarryInfo->mCarry ];
                *sResultDigit = sCarryInfo->mDigit;
            }


            /**
             * case A)의 ( B+1 ~ C-1 ) 구간 평가
             */
                
            while( sCurPos > sLastPos1 )
            {
                sCurPos--;
                sResultDigit--;

                sCarryInfo = & sAddMap[ sBlank1 + sBlank2 + sCarryInfo->mCarry ];
                *sResultDigit = sCarryInfo->mDigit;
            }

                
            /**
             * case A)의 ( A ~ B ) 구간 평가
             */

            while( sCurPos >= sFirstPos1 )
            {
                sCurPos--;
                sDigit1--;
                sResultDigit--;

                sCarryInfo = & sAddMap[ *sDigit1 + sBlank2 + sCarryInfo->mCarry ];
                *sResultDigit = sCarryInfo->mDigit;
            }

               
            /**
             * 최상위 Carry 처리
             */

            if( sCarryInfo->mCarry != 0 )
            {
                /**
                 * 부호가 같은 경우 : 최상위 digit 추가
                 */

                STL_TRY_THROW( ++sExponent <= DTL_NUMERIC_MAX_EXPONENT,
                               ERROR_OUT_OF_RANGE );

                aResult->mLength = 2;

                if( sIsPositive1 )
                {
                    *sResultDigit = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
                    DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric, sExponent );
                }
                else
                {
                    *sResultDigit = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 1 );
                    DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric, sExponent );
                }
            }
            else
            {
                /**
                 * Leading Zero 확인
                 */

                sCurPos = 0;
                while( sResultDigit[ sCurPos ] == sBlank1 )
                {
                    sCurPos++;
                }

                if( sCurPos > 0 )
                {
                    if( sDigitLen == sCurPos )
                    {
                        /* set zero */
                        DTL_NUMERIC_SET_ZERO( sResultNumeric, aResult->mLength );
                    }
                    else
                    {
                        /**
                         * Trailing Zero 제거
                         */

                        sResultDigit--;
                        while( ( sDigitLen > 0 ) &&
                               ( sResultDigit[ sDigitLen ] == sBlank1 ) )
                        {
                            sDigitLen--;
                        }
                        sDigitLen -= sCurPos;
                        STL_DASSERT( sDigitLen > 0 );
                    
                        
                        /**
                         * Leading Zero 제거
                         */

                        aResult->mLength = sDigitLen + 1;

                        sResultDigit++;
                        while( sDigitLen > 0 )
                        {
                            *sResultDigit = sResultDigit[ sCurPos ];
                            sResultDigit++;
                            sDigitLen--;
                        }

                        if( sIsPositive1 )
                        {
                            DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                        sExponent - sCurPos );
                        }
                        else
                        {
                            DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                        sExponent - sCurPos );
                        }
                    }
                }
                else
                {
                    /**
                     * Trailing Zero 제거
                     */

                    sResultDigit--;
                    while( ( sDigitLen > 0 ) &&
                           ( sResultDigit[ sDigitLen ] == sBlank1 ) )
                    {
                        sDigitLen--;
                    }
                    STL_DASSERT( sDigitLen > 0 );

                    aResult->mLength = sDigitLen + 1;
                    if( sIsPositive1 )
                    {
                        DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                    sExponent );
                    }
                    else
                    {
                        DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                    sExponent );
                    }
                }
            }
        }
        else
        {
            /**
             * CASE B-1  vs CASE B-2
             */
        
            if( sLastPos1 > sLastPos2 )
            {
                /*
                 * CASE B-1 
                 *
                 * digit 개수가 max digit count를 넘지 않는다.
                 * => round, trailing zero 필요 없음
                 */

                sCarryInfo    = dtdNumericInvalidCarryInfo;
                sDigitLen     = sLastPos1;
                sCurPos       = sLastPos1;
                sDigit1      += sLastPos1;
                sDigit2      += sLastPos2 - sFirstPos2 + 1;
                sResultDigit += sDigitLen;

            
                /**
                 * case B-1)의 ( D+1 ~ B ) 구간 평가
                 */
                
                while( sCurPos > sLastPos2 )
                {
                    sCurPos--;
                    sDigit1--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit1 + sBlank2 + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }

                /**
                 * case B-1)의 ( C ~ D ) 구간 평가
                 */
                
                while( sCurPos >= sFirstPos2 )
                {
                    sCurPos--;
                    sDigit1--;
                    sDigit2--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }

                
                /**
                 * case B-1)의 ( A ~ C-1 ) 구간 평가
                 */

                while( sCurPos >= sFirstPos1 )
                {
                    sCurPos--;
                    sDigit1--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit1 + sBlank2 + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }
            }
            else
            {
                /* CASE B-2 */
                if( sLastPos2 > DTL_NUMERIC_MAX_DIGIT_COUNT )
                {
                    sLastPos2  = DTL_NUMERIC_MAX_DIGIT_COUNT;
                    sDigit2    += DTL_NUMERIC_MAX_DIGIT_COUNT - sFirstPos2 + 1;

                    sCarryInfo  =
                        & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sIsPositive2, *sDigit2 ) > 49 ];
                
                    sDigitLen  = sLastPos2;
                    sCurPos    = sLastPos2;
                }
                else
                {
                    sCarryInfo = dtdNumericInvalidCarryInfo;
                    sDigit2   += sLastPos2 - sFirstPos2 + 1;
                    sDigitLen  = sLastPos2;
                    sCurPos    = sLastPos2;
                }
                sDigit1      += sLastPos1;
                sResultDigit += sDigitLen;

            
                /**
                 * case B-2)의 ( B+1 ~ D ) 구간 평가
                 */

                while( sCurPos > sLastPos1 )
                {
                    sCurPos--;
                    sDigit2--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit2 + sBlank1 + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }


                /**
                 * case B-2)의 ( C ~ B ) 구간 평가
                 */
                
                while( sCurPos >= sFirstPos2 )
                {
                    sCurPos--;
                    sDigit1--;
                    sDigit2--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }

                
                /**
                 * case B-2)의 ( A ~ C-1 ) 구간 평가
                 */

                while( sCurPos >= sFirstPos1 )
                {
                    sCurPos--;
                    sDigit1--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit1 + sBlank2 + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }


                /**
                 * Trailing Zero 제거
                 */

                sResultDigit--;
                while( ( sDigitLen > 0 ) &&
                       ( sResultDigit[ sDigitLen ] == sBlank1 ) )
                {
                    sDigitLen--;
                }
                sResultDigit++;

                if( sDigitLen == 0 )
                {
                    if( sCarryInfo->mCarry == 0 )
                    {
                        /* set zero */
                        DTL_NUMERIC_SET_ZERO( sResultNumeric, aResult->mLength );
                    }
                    else
                    {
                        sExponent++;
                        aResult->mLength = 2;
                        if( sIsPositive1 )
                        {
                            *sResultDigit = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
                            DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric, sExponent );
                        }
                        else
                        {
                            *sResultDigit = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 1 );
                            DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric, sExponent );
                        }
                    }
                
                    STL_THROW( RAMP_EXIT );
                }
                else
                {
                    /* Do Nothing */
                }
            }


            /**
             * 최상위 Carry 처리
             */

            if( sCarryInfo->mCarry != 0 )
            {
                /**
                 * 부호가 같은 경우 : 최상위 digit 추가
                 */

                STL_TRY_THROW( ++sExponent <= DTL_NUMERIC_MAX_EXPONENT,
                               ERROR_OUT_OF_RANGE );

                sResultDigit[ -1 ] = DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sIsPositive1, 1 );
                if( sDigitLen == DTL_NUMERIC_MAX_DIGIT_COUNT )
                {
                    if( sIsPositive1 )
                    {
                        sDigitLen--;
                        sCarryInfo  =
                            & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sResultDigit[ sDigitLen ] ) > 49 ];

                        if( sCarryInfo->mCarry == 0 )
                        {
                            /* Do Nothing */
                        }
                        else
                        {
                            while( sCarryInfo->mCarry != 0 )
                            {
                                sDigitLen--;
                                sCarryInfo = & sAddMap[ sResultDigit[ sDigitLen ] + sBlank1 + sCarryInfo->mCarry ];
                                sResultDigit[ sDigitLen ] = sCarryInfo->mDigit;
                            }
                            sDigitLen++;
                        }
                    }
                    else
                    {
                        sDigitLen--;
                        sCarryInfo  =
                            & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sResultDigit[ sDigitLen ] ) > 49 ];

                        if( sCarryInfo->mCarry == 0 )
                        {
                            /* Do Nothing */
                        }
                        else
                        {
                            while( sCarryInfo->mCarry != 0 )
                            {
                                sDigitLen--;
                                sCarryInfo = & sAddMap[ sResultDigit[ sDigitLen ] + sBlank1 + sCarryInfo->mCarry ];
                                sResultDigit[ sDigitLen ] = sCarryInfo->mDigit;
                            }
                            sDigitLen++;
                        }
                    }
                }

                STL_DASSERT( sDigitLen >= 0 );
                aResult->mLength = sDigitLen + 2;
                
                sResultDigit += sDigitLen;
                while( sDigitLen >= 0 )
                {
                    sResultDigit--;
                    sResultDigit[ 1 ] = sResultDigit[ 0 ];
                    sDigitLen--;
                }

                DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sResultNumeric, sIsPositive1, sExponent );
            }
            else
            {
                /**
                 * Leading Zero 확인
                 */

                sCurPos = 0;
                while( sResultDigit[ sCurPos ] == sBlank1 )
                {
                    sCurPos++;
                    if( --sDigitLen == 0 )
                    {
                        /* set zero */
                        DTL_NUMERIC_SET_ZERO( sResultNumeric, aResult->mLength );
                        STL_THROW( RAMP_EXIT );
                    }
                }

                if( sCurPos > 0 )
                {
                    /**
                     * Leading Zero 제거
                     */

                    aResult->mLength = sDigitLen + 1;

                    while( sDigitLen > 0 )
                    {
                        *sResultDigit = sResultDigit[ sCurPos ];
                        sResultDigit++;
                        sDigitLen--;
                    }

                    if( sIsPositive1 )
                    {
                        DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                    sExponent - sCurPos );
                    }
                    else
                    {
                        DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                    sExponent - sCurPos );
                    }
                }
                else
                {
                    aResult->mLength = sDigitLen + 1;

                    if( sIsPositive1 )
                    {
                        DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                    sExponent );
                    }
                    else
                    {
                        DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                    sExponent );
                    }
                }
            }
        }
    }    

    STL_RAMP( RAMP_EXIT );

#ifdef STL_DEBUG
    STL_DASSERT( aResult->mLength > 0 );
    STL_DASSERT( aResult->mLength <= DTL_NUMERIC_MAX_SIZE );

    for( sLoop = 1; sLoop < aResult->mLength; sLoop++ )
    {
        STL_DASSERT( ((stlUInt8*)aResult->mValue)[sLoop] <=
                     DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE );
    }
#endif
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Numeric & Numeric 의 SUM 연산
 * @param[in,out]  aSumValue      dtlDataValue
 * @param[in]      aSrcValue      dtlDataValue
 * @param[out]     aErrorStack    에러 스택
 */
stlStatus dtfNumericSum( dtlDataValue   * aSumValue,
                         dtlDataValue   * aSrcValue,
                         stlErrorStack  * aErrorStack )
{
    dtlNumericType  * sNumeric1;
    dtlNumericType  * sNumeric2;
    dtlNumericType  * sResultNumeric;

    stlUInt8          sIsPositive1;
    stlUInt8          sIsPositive2;
    stlInt32          sDigitLen;
    stlInt32          sExponent;
    stlInt32          sExponent1;
    stlInt32          sExponent2;

    stlUInt8        * sResultDigit;
    stlUInt8        * sOrgResultDigit;
    stlUInt8        * sTrailResultDigit;
    stlUInt8        * sDigit1;
    stlUInt8        * sDigit2;
    stlUInt8          sBlank1;
    stlUInt8          sBlank2;

    stlInt32          sFirstPos1;
    stlInt32          sLastPos1;
    stlInt32          sFirstPos2;
    stlInt32          sLastPos2;
    stlInt32          sCurPos;

    stlBool           sIsGreater;
    stlInt32          sLoop;
    stlInt8         * sTable;

    const dtlNumericCarryInfo  * sAddMap;
    const dtlNumericCarryInfo  * sCarryInfo;

    STL_DASSERT( ( aSrcValue->mType == DTL_TYPE_FLOAT ) ||
                 ( aSrcValue->mType == DTL_TYPE_NUMBER ) );
    STL_DASSERT( ( aSumValue->mType == DTL_TYPE_FLOAT ) ||
                 ( aSumValue->mType == DTL_TYPE_NUMBER ) );

    
    sNumeric1      = DTF_NUMERIC( aSumValue );
    sNumeric2      = DTF_NUMERIC( aSrcValue );
    sResultNumeric = DTF_NUMERIC( aSumValue );
    
    if( DTL_NUMERIC_IS_ZERO( sNumeric1, aSumValue->mLength ) )
    {
        /* copy */
        stlMemcpy( sResultNumeric,
                   sNumeric2,
                   aSrcValue->mLength );
        aSumValue->mLength = aSrcValue->mLength;
        STL_THROW( RAMP_EXIT );
    }

    if( DTL_NUMERIC_IS_ZERO( sNumeric2, aSrcValue->mLength ) )
    {
        STL_THROW( RAMP_EXIT );
    }

    /**
     * < numeric 간의 Addition 연산 >
     *
     * ## numeric1 과 numeric2의 Exponent를 고려한 Add 방법 구분
     *
     * numeric1의 digit은 'X' 
     * numeric1의 digit은 'Y'
     * numeric2의 평가가 필요 없는 digit은 'O'로 표시
     *
     *              01234567890123456789 (최대 20자리 Digit)
     *              
     * case 1)
     *              A          B
     *  numeric1 :  DTLDTLDTLDTL
     *  numeric2 :        YYYYYYYYYYY
     *                    C         D
     *
     *              A             B
     *  numeric1 :  DTLDTLDTLDTLDTL
     *  numeric2 :           YYYYYYYYYYYOOO
     *                       C         D   
     *
     * case 2)
     *              A      B
     *  numeric1 :  DTLDTLXX
     *  numeric2 :              YYYYY
     *                          C   D
     *
     *              A        B       
     *  numeric1 :  DTLDTLDTLX
     *  numeric2 :              YYYYYYYYYOOO
     *                          C       D
     *
     *              A        B
     *  numeric1 :  DTLDTLDTLX
     *  numeric2 :            YYYYYYYY
     *                        C      D
     *
     *              A        B
     *  numeric1 :  DTLDTLDTLX
     *  numeric2 :            YYYYYYYYYYYOOO
     *                        C         D
     *  
     * case 3)
     *              A     B
     *  numeric1 :  DTLDTLX
     *  numeric2 :  YYYYYYY
     *              C     D
     *  
     *  numeric1 :  DTLDTLDTLDTLDTL
     *  numeric2 :  YYYYYYYYY
     *  ==> 두 numeric간의 위치 변경
     *                  A       B
     *      numeric1 :  DTLDTLDTL
     *      numeric2 :  YYYYYYYYYYYYYYY
     *                  C             D
     *
     *              A     B                  
     *  numeric1 :  DTLDTLX
     *  numeric2 :  YYYYYYYYYYYY
     *              C          D
     *
     *  
     * case 4)
     *              A             B
     *  numeric1 :  DTLDTLDTLDTLDTL
     *  numeric2 :     YYYYYYYYYYYY
     *                 C          D
     *
     *              A             B
     *  numeric1 :  DTLDTLDTLDTLDTL
     *  numeric2 :     YYYYYY
     *                 C    D
     *
     *  
     * ## case 구분 방법 ( 반드시 numeric1은 numeric2보다 최대 exponent가 같거나 크다 )
     * 
     *  1. numeric1의 마지막 digit과 numeric2의 처음 digit과의 exponent 비교
     *     : ( B vs C )
     *  2. numeric1의 마지막 digit과 numeric2의 마지막 digit과의 exponent 비교
     *     : ( B vs D )
     *     
     */

    sExponent1 = DTL_NUMERIC_GET_EXPONENT( sNumeric1 );
    sExponent2 = DTL_NUMERIC_GET_EXPONENT( sNumeric2 );
        
    sIsGreater = STL_TRUE;
    if( sExponent1 > sExponent2 )
    {
        sIsGreater = STL_TRUE;
    }
    else
    {
        if( sExponent1 < sExponent2 )
        {
            sIsGreater = STL_FALSE;
        }
        else
        {
            sDigit1 = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
            sDigit2 = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );

            sTable = dtlNumericLenCompTable[ aSumValue->mLength ][ aSrcValue->mLength ];

            for( sLoop = 0 ; sLoop < sTable[1] ; sLoop++ )
            {
                if( sDigit1[ sLoop ] !=
                    DTL_NUMERIC_GET_REVERSE_DIGIT_FROM_DIGIT( & sDigit2[ sLoop ] ) )
                {
                    break;
                }
            }

            if( sLoop == sTable[1] )
            {
                if( sTable[0] == 0 )
                {
                    DTL_NUMERIC_SET_ZERO( sResultNumeric, aSumValue->mLength );
                    STL_THROW( RAMP_EXIT );
                }
                else
                {
                    sIsGreater = ( sTable[0] >= 0 );
                }
            }
            else
            {
                if( DTL_NUMERIC_IS_POSITIVE( sNumeric1 ) )
                {
                    sIsGreater = ( sDigit1[ sLoop ] >
                                   DTL_NUMERIC_GET_REVERSE_DIGIT_FROM_DIGIT( & sDigit2[ sLoop ] ) );
                }
                else
                {
                    sIsGreater = ( sDigit1[ sLoop ] <
                                   DTL_NUMERIC_GET_REVERSE_DIGIT_FROM_DIGIT( & sDigit2[ sLoop ] ) );
                }
            }
        }
    }

        
    /**
     * 절대값이 큰 수가 Numeric1이 된다.
     */
        
    if( sIsGreater == STL_FALSE )
    {
        /**
         * Numeric2 + Numeric1
         */

        sDigit1      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );
        sDigit2      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
        sIsPositive1 = DTL_NUMERIC_IS_POSITIVE( sNumeric2 );
        sIsPositive2 = DTL_NUMERIC_IS_POSITIVE( sNumeric1 );

            
        /**
         * 분석 위치 정보 설정
         */
            
        sFirstPos1 = 1;
        sLastPos1  = DTL_NUMERIC_GET_DIGIT_COUNT( aSrcValue->mLength );
        sFirstPos2 = sExponent2 - sExponent1 + 1;
        sLastPos2  = sFirstPos2 + DTL_NUMERIC_GET_DIGIT_COUNT( aSumValue->mLength ) - 1;
        sExponent  = sExponent2;
    }
    else
    {
        /**
         * Numeric1 + Numeric2
         */
            
        sDigit1      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
        sDigit2      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );
        sIsPositive1 = DTL_NUMERIC_IS_POSITIVE( sNumeric1 );
        sIsPositive2 = DTL_NUMERIC_IS_POSITIVE( sNumeric2 );


        /**
         * 분석 위치 정보 설정
         */
            
        sFirstPos1 = 1;
        sLastPos1  = DTL_NUMERIC_GET_DIGIT_COUNT( aSumValue->mLength );
        sFirstPos2 = sExponent1 - sExponent2 + 1;
        sLastPos2  = sFirstPos2 + DTL_NUMERIC_GET_DIGIT_COUNT( aSrcValue->mLength ) - 1;
        sExponent  = sExponent1;
    }
    
    sResultDigit = DTL_NUMERIC_GET_DIGIT_PTR( sResultNumeric );
    sOrgResultDigit = sResultDigit;
        
        
    /**
     * Add 연산 수행
     */
        
    /*
     * Numeric에 대한 분석 위치 정보 ( A ~ D )
     * A : 1 (고정)
     * B : Numeric1's Digit Count
     * C : Numeric1's Exponent - Numeric2's Exponent + 1
     * D : C + Numeric2's Digit Count
     *     단, 최대값은 DTL_NUMERIC_MAX_DIGIT_COUNT
     *
     * case A)  B < C :
     * 
     *    ==>    A ~ B   /   B+1 ~ C-1   /   C ~ D
     *           digit1          0           digit2
     *
     *           DTLDTLDTLXX
     *                      YYYYYYYYY
     *                    
     *     
     * case B)  B >= C :
     * 
     *      B-1)  B > D
     *                    A ~ C-1   /   C ~ D   /   D+1 ~ B
     *                    digit1    digit1+digit2   digit1
     *
     *           DTLDTLDTLXX
     *                YYYYY
     *
     *      B-2)  B <= D
     *                    A ~ C-1   /   C ~ B   /   B+1 ~ D
     *                    digit1    digit1+digit2   digit2
     *
     *           DTLDTLDTLXX
     *                  YYYYYYYYY
     *
     * case C)  B == D :
     * 
     *    ==>    A ~ B   /   B+1 ~ D
     *           digit1   digit1+digit2
     *
     *           DTLDTLDTLXX
     *                 YYYYY
     *                    
     */

        
    sAddMap = dtdNumericDigitMap[ sIsPositive1 ][ sIsPositive2 ];
    sBlank1 = dtlNumericZero[ sIsPositive1 ];
    sBlank2 = dtlNumericZero[ sIsPositive2 ];
    sResultNumeric->mData[0] = sBlank1;

    if( sLastPos1 == sLastPos2 )
    {
        sCarryInfo    = dtdNumericInvalidCarryInfo;
        sDigitLen     = sLastPos1;
        sCurPos       = sLastPos1;
        sDigit1      += sLastPos1;
        sDigit2      += sLastPos2 - sFirstPos2 + 1;
        sResultDigit += sDigitLen;
        sTrailResultDigit = sResultDigit - 1;

            
        /**
         * case C)의 ( B+1 ~ D ) 구간 평가
         */
                
        while( sCurPos >= sFirstPos2 )
        {
            sCurPos--;
            sDigit1--;
            sDigit2--;
            sResultDigit--;

            sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
            *sResultDigit = sCarryInfo->mDigit;
        }

                
        /**
         * case C)의 ( A ~ B ) 구간 평가
         */

        if( sIsGreater == STL_TRUE )
        {
            if( sCarryInfo->mCarry != 0 )
            {
                while( sCurPos >= sFirstPos1 )
                {
                    sCurPos--;
                    sDigit1--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit1 + sBlank2 + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                
                    if( sCarryInfo->mCarry == 0 )
                    {
                        sResultDigit = sOrgResultDigit;
                        break;
                    }
                }
            }
            else
            {
                sResultDigit = sOrgResultDigit;
            }
        }
        else
        {
            while( sCurPos >= sFirstPos1 )
            {
                sCurPos--;
                sDigit1--;
                sResultDigit--;

                sCarryInfo = & sAddMap[ *sDigit1 + sBlank2 + sCarryInfo->mCarry ];
                *sResultDigit = sCarryInfo->mDigit;
            }
        }
        
        /**
         * Trailing Zero 제거
         */

        if( *sTrailResultDigit == sBlank1 )
        {
            sResultDigit--;
            while( ( sDigitLen > 0 ) &&
                   ( sResultDigit[ sDigitLen ] == sBlank1 ) )
            {
                sDigitLen--;
            }
            sResultDigit++;
            
            if( sDigitLen == 0 )
            {
                if( sCarryInfo->mCarry == 0 )
                {
                    /* set zero */
                    DTL_NUMERIC_SET_ZERO( sResultNumeric, aSumValue->mLength );
                }
                else
                {
                    sExponent++;
                    aSumValue->mLength = 2;
                    if( sIsPositive1 )
                    {
                        *sResultDigit = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
                        DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric, sExponent );
                    }
                    else
                    {
                        *sResultDigit = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 1 );
                        DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric, sExponent );
                    }
                }
                
                STL_THROW( RAMP_EXIT );
            }
        }
        
        /**
         * 최상위 Carry 처리
         */

        STL_DASSERT( sCarryInfo->mCarry == 0 );


        /**
         * Leading Zero 확인
         */

        sCurPos = 0;
        while( sResultDigit[ sCurPos ] == sBlank1 )
        {
            sCurPos++;
            if( --sDigitLen == 0 )
            {
                /* set zero */
                DTL_NUMERIC_SET_ZERO( sResultNumeric, aSumValue->mLength );
                STL_THROW( RAMP_EXIT );
            }
        }

        if( sCurPos > 0 )
        {
            /**
             * Leading Zero 제거
             */

            aSumValue->mLength = sDigitLen + 1;

            while( sDigitLen > 0 )
            {
                *sResultDigit = sResultDigit[ sCurPos ];
                sResultDigit++;
                sDigitLen--;
            }

            if( sIsPositive1 )
            {
                DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                            sExponent - sCurPos );
            }
            else
            {
                DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                            sExponent - sCurPos );
            }
        }
        else
        {
            aSumValue->mLength = sDigitLen + 1;

            if( sIsPositive1 )
            {
                DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                            sExponent );
            }
            else
            {
                DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                            sExponent );
            }
        }
    }
    else
    {
        /**
         * case 구분 ( case A  vs  case B )
         */

        if( sLastPos1 < sFirstPos2 )
        {
            /* CASE A */
            if( sLastPos2 > DTL_NUMERIC_MAX_DIGIT_COUNT )
            {
                sLastPos2  = DTL_NUMERIC_MAX_DIGIT_COUNT;
                sDigit2    += DTL_NUMERIC_MAX_DIGIT_COUNT - sFirstPos2 + 1;

                sCarryInfo  =
                    & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sIsPositive2, *sDigit2 ) > 49 ];
                
                sDigitLen  = sLastPos2;
                sCurPos    = sLastPos2;
            }
            else
            {
                sCarryInfo = dtdNumericInvalidCarryInfo;
                sDigit2   += sLastPos2 - sFirstPos2 + 1;
                sDigitLen  = sLastPos2;
                sCurPos    = sLastPos2;
            }
            sDigit1      += sLastPos1;
            sResultDigit += sDigitLen;
            
                
            /**
             * case A)의 ( C ~ D ) 구간 평가
             */

            while( sCurPos >= sFirstPos2 )
            {
                sCurPos--;
                sDigit2--;
                sResultDigit--;

                sCarryInfo = & sAddMap[ *sDigit2 + sBlank1 + sCarryInfo->mCarry ];
                *sResultDigit = sCarryInfo->mDigit;
            }


            /**
             * case A)의 ( B+1 ~ C-1 ) 구간 평가
             */
                
            while( sCurPos > sLastPos1 )
            {
                sCurPos--;
                sResultDigit--;

                sCarryInfo = & sAddMap[ sBlank1 + sBlank2 + sCarryInfo->mCarry ];
                *sResultDigit = sCarryInfo->mDigit;
            }

                
            /**
             * case A)의 ( A ~ B ) 구간 평가
             */

            while( sCurPos >= sFirstPos1 )
            {
                sCurPos--;
                sDigit1--;
                sResultDigit--;

                sCarryInfo = & sAddMap[ *sDigit1 + sBlank2 + sCarryInfo->mCarry ];
                *sResultDigit = sCarryInfo->mDigit;
            }

               
            /**
             * 최상위 Carry 처리
             */

            STL_DASSERT( sCarryInfo->mCarry == 0 );


            /**
             * Leading Zero 확인
             */

            sCurPos = 0;
            while( sResultDigit[ sCurPos ] == sBlank1 )
            {
                sCurPos++;
            }

            if( sCurPos > 0 )
            {
                if( sDigitLen == sCurPos )
                {
                    /* set zero */
                    DTL_NUMERIC_SET_ZERO( sResultNumeric, aSumValue->mLength );
                }
                else
                {
                    /**
                     * Trailing Zero 제거
                     */

                    sResultDigit--;
                    while( ( sDigitLen > 0 ) &&
                           ( sResultDigit[ sDigitLen ] == sBlank1 ) )
                    {
                        sDigitLen--;
                    }
                    sDigitLen -= sCurPos;
                    STL_DASSERT( sDigitLen > 0 );
                    
                        
                    /**
                     * Leading Zero 제거
                     */

                    aSumValue->mLength = sDigitLen + 1;

                    sResultDigit++;
                    while( sDigitLen > 0 )
                    {
                        *sResultDigit = sResultDigit[ sCurPos ];
                        sResultDigit++;
                        sDigitLen--;
                    }

                    if( sIsPositive1 )
                    {
                        DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                    sExponent - sCurPos );
                    }
                    else
                    {
                        DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                    sExponent - sCurPos );
                    }
                }
            }
            else
            {
                /**
                 * Trailing Zero 제거
                 */

                sResultDigit--;
                while( ( sDigitLen > 0 ) &&
                       ( sResultDigit[ sDigitLen ] == sBlank1 ) )
                {
                    sDigitLen--;
                }
                STL_DASSERT( sDigitLen > 0 );

                aSumValue->mLength = sDigitLen + 1;
                if( sIsPositive1 )
                {
                    DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent );
                }
                else
                {
                    DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent );
                }
            }
        }
        else
        {
            /**
             * CASE B-1  vs CASE B-2
             */
        
            if( sLastPos1 > sLastPos2 )
            {
                /*
                 * CASE B-1 
                 *
                 * digit 개수가 max digit count를 넘지 않는다.
                 * => round, trailing zero 필요 없음
                 */

                sCarryInfo    = dtdNumericInvalidCarryInfo;
                sDigitLen     = sLastPos1;
                sCurPos       = sLastPos1;
                sDigit1      += sLastPos1;
                sDigit2      += sLastPos2 - sFirstPos2 + 1;
                sResultDigit += sDigitLen;

            
                /**
                 * case B-1)의 ( D+1 ~ B ) 구간 평가
                 */
                
                while( sCurPos > sLastPos2 )
                {
                    sCurPos--;
                    sDigit1--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit1 + sBlank2 + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }

                /**
                 * case B-1)의 ( C ~ D ) 구간 평가
                 */
                
                while( sCurPos >= sFirstPos2 )
                {
                    sCurPos--;
                    sDigit1--;
                    sDigit2--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }

                
                /**
                 * case B-1)의 ( A ~ C-1 ) 구간 평가
                 */

                while( sCurPos >= sFirstPos1 )
                {
                    sCurPos--;
                    sDigit1--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit1 + sBlank2 + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }
            }
            else
            {
                /* CASE B-2 */
                if( sLastPos2 > DTL_NUMERIC_MAX_DIGIT_COUNT )
                {
                    sLastPos2  = DTL_NUMERIC_MAX_DIGIT_COUNT;
                    sDigit2    += DTL_NUMERIC_MAX_DIGIT_COUNT - sFirstPos2 + 1;

                    sCarryInfo  =
                        & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sIsPositive2, *sDigit2 ) > 49 ];
                
                    sDigitLen  = sLastPos2;
                    sCurPos    = sLastPos2;
                }
                else
                {
                    sCarryInfo = dtdNumericInvalidCarryInfo;
                    sDigit2   += sLastPos2 - sFirstPos2 + 1;
                    sDigitLen  = sLastPos2;
                    sCurPos    = sLastPos2;
                }
                sDigit1      += sLastPos1;
                sResultDigit += sDigitLen;

            
                /**
                 * case B-2)의 ( B+1 ~ D ) 구간 평가
                 */

                while( sCurPos > sLastPos1 )
                {
                    sCurPos--;
                    sDigit2--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit2 + sBlank1 + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }


                /**
                 * case B-2)의 ( C ~ B ) 구간 평가
                 */
                
                while( sCurPos >= sFirstPos2 )
                {
                    sCurPos--;
                    sDigit1--;
                    sDigit2--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }

                
                /**
                 * case B-2)의 ( A ~ C-1 ) 구간 평가
                 */

                while( sCurPos >= sFirstPos1 )
                {
                    sCurPos--;
                    sDigit1--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit1 + sBlank2 + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }


                /**
                 * Trailing Zero 제거
                 */

                sResultDigit--;
                while( ( sDigitLen > 0 ) &&
                       ( sResultDigit[ sDigitLen ] == sBlank1 ) )
                {
                    sDigitLen--;
                }
                sResultDigit++;

                if( sDigitLen == 0 )
                {
                    if( sCarryInfo->mCarry == 0 )
                    {
                        /* set zero */
                        DTL_NUMERIC_SET_ZERO( sResultNumeric, aSumValue->mLength );
                    }
                    else
                    {
                        sExponent++;
                        aSumValue->mLength = 2;
                        if( sIsPositive1 )
                        {
                            *sResultDigit = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
                            DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric, sExponent );
                        }
                        else
                        {
                            *sResultDigit = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 1 );
                            DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric, sExponent );
                        }
                    }
                
                    STL_THROW( RAMP_EXIT );
                }
                else
                {
                    /* Do Nothing */
                }
            }


            /**
             * 최상위 Carry 처리
             */

            STL_DASSERT( sCarryInfo->mCarry == 0 );


            /**
             * Leading Zero 확인
             */

            sCurPos = 0;
            while( sResultDigit[ sCurPos ] == sBlank1 )
            {
                sCurPos++;
                if( --sDigitLen == 0 )
                {
                    /* set zero */
                    DTL_NUMERIC_SET_ZERO( sResultNumeric, aSumValue->mLength );
                    STL_THROW( RAMP_EXIT );
                }
            }

            if( sCurPos > 0 )
            {
                /**
                 * Leading Zero 제거
                 */

                aSumValue->mLength = sDigitLen + 1;

                while( sDigitLen > 0 )
                {
                    *sResultDigit = sResultDigit[ sCurPos ];
                    sResultDigit++;
                    sDigitLen--;
                }

                if( sIsPositive1 )
                {
                    DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent - sCurPos );
                }
                else
                {
                    DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent - sCurPos );
                }
            }
            else
            {
                aSumValue->mLength = sDigitLen + 1;

                if( sIsPositive1 )
                {
                    DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent );
                }
                else
                {
                    DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent );
                }
            }
        }
    }    

    STL_RAMP( RAMP_EXIT );

#ifdef STL_DEBUG
    STL_DASSERT( aSumValue->mLength > 0 );
    STL_DASSERT( aSumValue->mLength <= DTL_NUMERIC_MAX_SIZE );

    for( sLoop = 1; sLoop < aSumValue->mLength; sLoop++ )
    {
        STL_DASSERT( ((stlUInt8*)aSumValue->mValue)[sLoop] <=
                     DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE );
    }
#endif
    
    return STL_SUCCESS;

    /* STL_FINISH; */

    /* return STL_FAILURE; */
}


/**
 * @brief Positive Numeric & Positive Numeric 의 SUM 연산
 * @param[in,out]  aSumValue      dtlDataValue
 * @param[in]      aSrcValue      dtlDataValue
 * @param[out]     aErrorStack    에러 스택
 */
stlStatus dtfNumericSumPosPos( dtlDataValue   * aSumValue,
                               dtlDataValue   * aSrcValue,
                               stlErrorStack  * aErrorStack )
{
    dtlNumericType  * sNumeric1;
    dtlNumericType  * sNumeric2;
    dtlNumericType  * sResultNumeric;

    stlInt32          sDigitLen;
    stlInt32          sExponent;
    stlInt32          sExponent1;
    stlInt32          sExponent2;

    stlUInt8        * sResultDigit;
    stlUInt8        * sOrgResultDigit;
    stlUInt8        * sTrailResultDigit;
    stlUInt8        * sDigit1;
    stlUInt8        * sDigit2;
    stlUInt8          sBlank;

    stlInt32          sFirstPos1;
    stlInt32          sLastPos1;
    stlInt32          sFirstPos2;
    stlInt32          sLastPos2;
    stlInt32          sCurPos;

    stlBool           sIsGreater;
    stlInt32          sLoop;
    stlInt8         * sTable;

    const dtlNumericCarryInfo  * sAddMap;
    const dtlNumericCarryInfo  * sCarryInfo; 

    STL_DASSERT( ( aSrcValue->mType == DTL_TYPE_FLOAT ) ||
                 ( aSrcValue->mType == DTL_TYPE_NUMBER ) );
    STL_DASSERT( ( aSumValue->mType == DTL_TYPE_FLOAT ) ||
                 ( aSumValue->mType == DTL_TYPE_NUMBER ) );

    
    sNumeric1      = DTF_NUMERIC( aSumValue );
    sNumeric2      = DTF_NUMERIC( aSrcValue );
    sResultNumeric = DTF_NUMERIC( aSumValue );
    
    if( DTL_NUMERIC_IS_ZERO( sNumeric1, aSumValue->mLength ) )
    {
        /* copy */
        stlMemcpy( sResultNumeric,
                   sNumeric2,
                   aSrcValue->mLength );
        aSumValue->mLength = aSrcValue->mLength;
        STL_THROW( RAMP_EXIT );
    }

    if( DTL_NUMERIC_IS_ZERO( sNumeric2, aSrcValue->mLength ) )
    {
        STL_THROW( RAMP_EXIT );
    }

#ifdef STL_DEBUG
    stlChar   sTmpBuffer[100];
    /* stlInt32  sTempLength; */

    /* sTempLength = aSumValue->mLength; */
    stlMemcpy( sTmpBuffer, aSumValue->mValue, aSumValue->mLength );
#endif

    sDigit1      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
    sDigit2      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );

    sExponent1 = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sNumeric1 );
    sExponent2 = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sNumeric2 );
    
    if( sExponent1 == sExponent2 )
    {
        sDigit1 = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
        sDigit2 = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );

        sTable = dtlNumericLenCompTable[ aSumValue->mLength ][ aSrcValue->mLength ];

        for( sLoop = 0 ; sLoop < sTable[1] ; sLoop++ )
        {
            if( sDigit1[ sLoop ] != sDigit2[ sLoop ] )
            {
                break;
            }
        }

        if( sLoop == sTable[1] )
        {
            sIsGreater = ( sTable[0] >= 0 );
        }
        else
        {
            sIsGreater = sDigit1[ sLoop ] >= sDigit2[ sLoop ];
        }
    }
    else
    {
        if( sExponent1 < sExponent2 )
        {
            sIsGreater = STL_FALSE;
        }
        else
        {
            sIsGreater = STL_TRUE;
        }
    }

        
    /**
     * 절대값이 큰 수가 Numeric1이 된다.
     */
        
    if( sIsGreater == STL_FALSE )
    {
        /**
         * Numeric2 + Numeric1
         */

        sDigit1      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );
        sDigit2      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );

            
        /**
         * 분석 위치 정보 설정
         */
            
        sFirstPos1 = 1;
        sLastPos1  = DTL_NUMERIC_GET_DIGIT_COUNT( aSrcValue->mLength );
        sFirstPos2 = sExponent2 - sExponent1 + 1;
        sLastPos2  = sFirstPos2 + DTL_NUMERIC_GET_DIGIT_COUNT( aSumValue->mLength ) - 1;
        sExponent  = sExponent2;
    }
    else
    {
        /**
         * Numeric1 + Numeric2
         */
            
        sDigit1      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
        sDigit2      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );


        /**
         * 분석 위치 정보 설정
         */
            
        sFirstPos1 = 1;
        sLastPos1  = DTL_NUMERIC_GET_DIGIT_COUNT( aSumValue->mLength );
        sFirstPos2 = sExponent1 - sExponent2 + 1;
        sLastPos2  = sFirstPos2 + DTL_NUMERIC_GET_DIGIT_COUNT( aSrcValue->mLength ) - 1;
        sExponent  = sExponent1;
    }
    
    sResultDigit = DTL_NUMERIC_GET_DIGIT_PTR( sResultNumeric );
    sOrgResultDigit = sResultDigit;
        
        
    /**
     * Add 연산 수행
     */
        
    /*
     * Numeric에 대한 분석 위치 정보 ( A ~ D )
     * A : 1 (고정)
     * B : Numeric1's Digit Count
     * C : Numeric1's Exponent - Numeric2's Exponent + 1
     * D : C + Numeric2's Digit Count
     *     단, 최대값은 DTL_NUMERIC_MAX_DIGIT_COUNT
     *
     * case A)  B < C :
     * 
     *    ==>    A ~ B   /   B+1 ~ C-1   /   C ~ D
     *           digit1          0           digit2
     *
     *           DTLDTLDTLXX
     *                      YYYYYYYYY
     *                    
     *     
     * case B)  B >= C :
     * 
     *      B-1)  B > D
     *                    A ~ C-1   /   C ~ D   /   D+1 ~ B
     *                    digit1    digit1+digit2   digit1
     *
     *           DTLDTLDTLXX
     *                YYYYY
     *
     *      B-2)  B <= D
     *                    A ~ C-1   /   C ~ B   /   B+1 ~ D
     *                    digit1    digit1+digit2   digit2
     *
     *           DTLDTLDTLXX
     *                  YYYYYYYYY
     *                  
     * case C)  B == D :
     * 
     *    ==>    A ~ B   /   B+1 ~ D
     *           digit1   digit1+digit2
     *
     *           DTLDTLDTLXX
     *                 YYYYY
     *                    
     */

        
    sAddMap = dtdNumericDigitMap[ 1 ][ 1 ];
    sBlank = DTL_NUMERIC_DIGIT_POSITIVE_FENCE_VALUE;
    sResultNumeric->mData[0] = sBlank;

    if( sLastPos1 == sLastPos2 )
    {
        sCarryInfo    = dtdNumericInvalidCarryInfo;
        sDigitLen     = sLastPos1;
        sCurPos       = sLastPos1;
        sDigit1      += sLastPos1;
        sDigit2      += sLastPos2 - sFirstPos2 + 1;
        
        sResultDigit += sDigitLen;
        sTrailResultDigit = sResultDigit - 1;

            
        /**
         * case C)의 ( B+1 ~ D ) 구간 평가
         */
                
        while( sCurPos >= sFirstPos2 )
        {
            sCurPos--;
            sDigit1--;
            sDigit2--;
            sResultDigit--;

            sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
            *sResultDigit = sCarryInfo->mDigit;
        }
                
        /**
         * case C)의 ( A ~ B ) 구간 평가
         */

        if( sIsGreater == STL_TRUE )
        {
            if( sCarryInfo->mCarry != 0 )
            {
                while( sCurPos >= sFirstPos1 )
                {
                    sCurPos--;
                    sDigit1--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
            
                    if( sCarryInfo->mCarry == 0 )
                    {
                        sResultDigit = sOrgResultDigit;
                        break;
                    }
                }
            }
            else
            {
                sResultDigit = sOrgResultDigit;
            }
        }
        else
        {
            while( sCurPos >= sFirstPos1 )
            {
                sCurPos--;
                sDigit1--;
                sResultDigit--;

                sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                *sResultDigit = sCarryInfo->mDigit;
            }
        }
        
        /**
         * Trailing Zero 제거
         */

        if( *sTrailResultDigit == sBlank )
        {
            sResultDigit--;
            while( ( sDigitLen > 0 ) &&
                   ( sResultDigit[ sDigitLen ] == sBlank ) )
            {
                sDigitLen--;
            }
            sResultDigit++;
        }
        
        /**
         * 최상위 Carry 처리
         */

        if( sCarryInfo->mCarry != 0 )
        {
            /**
             * 부호가 같은 경우 : 최상위 digit 추가
             */

            STL_TRY_THROW( ++sExponent <= DTL_NUMERIC_MAX_EXPONENT,
                           ERROR_OUT_OF_RANGE );

            sResultDigit[ -1 ] = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
            if( sDigitLen == DTL_NUMERIC_MAX_DIGIT_COUNT )
            {
                sDigitLen--;
                sCarryInfo  =
                    & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sResultDigit[ sDigitLen ] ) > 49 ];

                if( sCarryInfo->mCarry == 0 )
                {
                    /* Do Nothing */
                }
                else
                {
                    while( sCarryInfo->mCarry != 0 )
                    {
                        sDigitLen--;
                        sCarryInfo = & sAddMap[ sResultDigit[ sDigitLen ] + sBlank + sCarryInfo->mCarry ];
                        sResultDigit[ sDigitLen ] = sCarryInfo->mDigit;
                    }
                    sDigitLen++;
                }
            }

            STL_DASSERT( sDigitLen >= 0 );
            aSumValue->mLength = sDigitLen + 2;
                
            sResultDigit += sDigitLen;
            while( sDigitLen >= 0 )
            {
                sResultDigit--;
                sResultDigit[ 1 ] = sResultDigit[ 0 ];
                sDigitLen--;
            }

            DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric, sExponent );
        }
        else
        {
            aSumValue->mLength = sDigitLen + 1;
            DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                        sExponent );
        }
    }
    else
    {
        /**
         * case 구분 ( case A  vs  case B )
         */

        if( sLastPos1 < sFirstPos2 )
        {
            /* CASE A */
            if( sLastPos2 > DTL_NUMERIC_MAX_DIGIT_COUNT )
            {
                sLastPos2  = DTL_NUMERIC_MAX_DIGIT_COUNT;
                sDigit2    += DTL_NUMERIC_MAX_DIGIT_COUNT - sFirstPos2 + 1;

                sCarryInfo  =
                    & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit2 ) > 49 ];
                
                sDigitLen  = sLastPos2;
                sCurPos    = sLastPos2;
                
                sDigit1      += sLastPos1;
                sResultDigit += sDigitLen;
                sTrailResultDigit = sResultDigit - 1;
            
                /**
                 * case A)의 ( C ~ D ) 구간 평가
                 */

                while( sCurPos >= sFirstPos2 )
                {
                    sCurPos--;
                    sDigit2--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit2 + sBlank + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }


                /**
                 * case A)의 ( B+1 ~ C-1 ) 구간 평가
                 */
                
                while( sCurPos > sLastPos1 )
                {
                    sCurPos--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ sBlank + sBlank + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }

                
                /**
                 * case A)의 ( A ~ B ) 구간 평가
                 */

                while( sCurPos >= sFirstPos1 )
                {
                    sCurPos--;
                    sDigit1--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }

                /**
                 * 최상위 Carry 처리
                 */

                if( sCarryInfo->mCarry != 0 )
                {
                    /**
                     * 부호가 같은 경우 : 최상위 digit 추가
                     */

                    STL_TRY_THROW( ++sExponent <= DTL_NUMERIC_MAX_EXPONENT,
                                   ERROR_OUT_OF_RANGE );

                    aSumValue->mLength = 2;
                
                    *sResultDigit = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
                    DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric, sExponent );
                }
                else
                {
                    /**
                     * Trailing Zero 제거
                     */

                    if( *sTrailResultDigit == sBlank )
                    {
                        sResultDigit--;
                        while( ( sDigitLen > 0 ) &&
                               ( sResultDigit[ sDigitLen ] == sBlank ) )
                        {
                            sDigitLen--;
                        }
                        sResultDigit++;
                    }
                
                    aSumValue->mLength = sDigitLen + 1;
                    DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent );
                }
            }
            else
            {
                sCarryInfo = dtdNumericInvalidCarryInfo;
                sDigitLen  = sLastPos2;
                
                if( sIsGreater == STL_TRUE )
                {
                    sDigit2      += sLastPos2 - sFirstPos2 + 1;
                    sCurPos       = sLastPos2;
                    sResultDigit += sLastPos2;
                    
                    /**
                     * case A)의 ( C ~ D ) 구간 평가
                     */

                    while( sCurPos >= sFirstPos2 )
                    {
                        sCurPos--;
                        sDigit2--;
                        sResultDigit--;
                        *sResultDigit = *sDigit2;
                    }
                    
                    /**
                     * case A)의 ( B+1 ~ C-1 ) 구간 평가
                     */

                    while( sCurPos > sLastPos1 )
                    {
                        sCurPos--;
                        sResultDigit--;
                        *sResultDigit = sBlank;
                    }
                }
                else
                {
                    sDigit2      += sLastPos2 - sFirstPos2 + 1;
                    sDigit1      += sLastPos1;
                    sCurPos       = sLastPos2;
                    sResultDigit += sLastPos2;
                    
                    /**
                     * case A)의 ( C ~ D ) 구간 평가
                     */

                    while( sCurPos >= sFirstPos2 )
                    {
                        sCurPos--;
                        sDigit2--;
                        sResultDigit--;
                        *sResultDigit = *sDigit2;
                    }
                    
                    /**
                     * case A)의 ( B+1 ~ C-1 ) 구간 평가
                     */

                    while( sCurPos > sLastPos1 )
                    {
                        sCurPos--;
                        sResultDigit--;
                        *sResultDigit = sBlank;
                    }
                    
                    /**
                     * case A)의 ( A ~ B ) 구간 평가
                     */

                    while( sCurPos >= sFirstPos1 )
                    {
                        sCurPos--;
                        sDigit1--;
                        sResultDigit--;
                        *sResultDigit = *sDigit1;
                    }
                }
            
                aSumValue->mLength = sDigitLen + 1;
                DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                            sExponent );
            }
        }
        else
        {
            /**
             * CASE B-1  vs CASE B-2
             */
        
            if( sLastPos1 > sLastPos2 )
            {
                /*
                 * CASE B-1 
                 *
                 * digit 개수가 max digit count를 넘지 않는다.
                 * => round, trailing zero 필요 없음
                 */

                sCarryInfo    = dtdNumericInvalidCarryInfo;
                sDigitLen     = sLastPos1;
                sDigit2      += sLastPos2 - sFirstPos2 + 1;

                if( sIsGreater == STL_FALSE )
                {
                    sDigit1      += sLastPos1;
                    sCurPos       = sLastPos1;
                    sResultDigit += sLastPos1;
                    
                    /**
                     * case B-1)의 ( D+1 ~ B ) 구간 평가
                     */
                
                    while( sCurPos > sLastPos2 )
                    {
                        sCurPos--;
                        sDigit1--;
                        sResultDigit--;
                        *sResultDigit = *sDigit1;
                    }
                    
                    /**
                     * case B-1)의 ( C ~ D ) 구간 평가
                     */
                
                    while( sCurPos >= sFirstPos2 )
                    {
                        sCurPos--;
                        sDigit1--;
                        sDigit2--;
                        sResultDigit--;

                        sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
                        *sResultDigit = sCarryInfo->mDigit;
                    }

                
                    /**
                     * case B-1)의 ( A ~ C-1 ) 구간 평가
                     */

                    while( sCurPos >= sFirstPos1 )
                    {
                        sCurPos--;
                        sDigit1--;
                        sResultDigit--;

                        sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                        *sResultDigit = sCarryInfo->mDigit;
                    }
                }
                else
                {
                    sDigit1      += sLastPos2;
                    sCurPos       = sLastPos2;
                    sResultDigit += sLastPos2;
                    
                    /**
                     * case B-1)의 ( C ~ D ) 구간 평가
                     */
                
                    while( sCurPos >= sFirstPos2 )
                    {
                        sCurPos--;
                        sDigit1--;
                        sDigit2--;
                        sResultDigit--;

                        sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
                        *sResultDigit = sCarryInfo->mDigit;
                    }

                
                    /**
                     * case B-1)의 ( A ~ C-1 ) 구간 평가
                     */

                    if( sCarryInfo->mCarry != 0 )
                    {
                        while( sCurPos >= sFirstPos1 )
                        {
                            sCurPos--;
                            sDigit1--;
                            sResultDigit--;

                            sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                            *sResultDigit = sCarryInfo->mDigit;
                        
                            if( sCarryInfo->mCarry == 0 )
                            {
                                sResultDigit = sOrgResultDigit;
                                break;
                            }
                        }
                    }
                    else
                    {
                        sResultDigit = sOrgResultDigit;
                    }
                }
                
            }
            else
            {
                /* CASE B-2 */
                if( sLastPos2 > DTL_NUMERIC_MAX_DIGIT_COUNT )
                {
                    sLastPos2  = DTL_NUMERIC_MAX_DIGIT_COUNT;
                    sDigit2    += DTL_NUMERIC_MAX_DIGIT_COUNT - sFirstPos2 + 1;

                    sCarryInfo  =
                        & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit2 ) > 49 ];
                
                    sDigitLen  = sLastPos2;
                    sCurPos    = sLastPos2;
                    
                    sDigit1      += sLastPos1;
                    sResultDigit += sDigitLen;
                    sTrailResultDigit = sResultDigit - 1;
            
                    /**
                     * case B-2)의 ( B+1 ~ D ) 구간 평가
                     */

                    while( sCurPos > sLastPos1 )
                    {
                        sCurPos--;
                        sDigit2--;
                        sResultDigit--;

                        sCarryInfo = & sAddMap[ *sDigit2 + sBlank + sCarryInfo->mCarry ];
                        *sResultDigit = sCarryInfo->mDigit;
                    }
                    
                    /**
                     * case B-2)의 ( C ~ B ) 구간 평가
                     */
                
                    while( sCurPos >= sFirstPos2 )
                    {
                        sCurPos--;
                        sDigit1--;
                        sDigit2--;
                        sResultDigit--;

                        sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
                        *sResultDigit = sCarryInfo->mDigit;
                    }

                
                    /**
                     * case B-2)의 ( A ~ C-1 ) 구간 평가
                     */

                    if( sIsGreater == STL_TRUE )
                    {
                        if( sCarryInfo->mCarry != 0 )
                        {
                            while( sCurPos >= sFirstPos1 )
                            {
                                sCurPos--;
                                sDigit1--;
                                sResultDigit--;

                                sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                                *sResultDigit = sCarryInfo->mDigit;
                        
                                if( sCarryInfo->mCarry == 0 )
                                {
                                    sResultDigit = sOrgResultDigit;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            sResultDigit = sOrgResultDigit;
                        }
                    }
                    else
                    {
                        while( sCurPos >= sFirstPos1 )
                        {
                            sCurPos--;
                            sDigit1--;
                            sResultDigit--;

                            sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                            *sResultDigit = sCarryInfo->mDigit;
                        }
                    }
                
                    /**
                     * Trailing Zero 제거
                     */

                    if( *sTrailResultDigit == sBlank )
                    {
                        sResultDigit--;
                        while( ( sDigitLen > 0 ) &&
                               ( sResultDigit[ sDigitLen ] == sBlank ) )
                        {
                            sDigitLen--;
                        }
                        sResultDigit++;
                    }
                }
                else
                {
                    sCarryInfo = dtdNumericInvalidCarryInfo;
                    sDigitLen  = sLastPos2;
                    sDigit1   += sLastPos1;
                    
                    sDigit2      += sLastPos2 - sFirstPos2 + 1;
                    sCurPos       = sLastPos2;
                    sResultDigit += sLastPos2;
                        
                    /**
                     * case B-2)의 ( B+1 ~ D ) 구간 평가
                     */

                    while( sCurPos > sLastPos1 )
                    {
                        sCurPos--;
                        sDigit2--;
                        sResultDigit--;
                        *sResultDigit = *sDigit2;
                    }
                        
                    if( sIsGreater == STL_TRUE )
                    {
                        /**
                         * case B-2)의 ( C ~ B ) 구간 평가
                         */
                
                        while( sCurPos >= sFirstPos2 )
                        {
                            sCurPos--;
                            sDigit1--;
                            sDigit2--;
                            sResultDigit--;

                            sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
                            *sResultDigit = sCarryInfo->mDigit;
                        }

                
                        /**
                         * case B-2)의 ( A ~ C-1 ) 구간 평가
                         */

                        if( sCarryInfo->mCarry != 0 )
                        {
                            while( sCurPos >= sFirstPos1 )
                            {
                                sCurPos--;
                                sDigit1--;
                                sResultDigit--;

                                sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                                *sResultDigit = sCarryInfo->mDigit;
                        
                                if( sCarryInfo->mCarry == 0 )
                                {
                                    sResultDigit = sOrgResultDigit;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            sResultDigit = sOrgResultDigit;
                        }
                    }
                    else
                    {
                        /**
                         * case B-2)의 ( C ~ B ) 구간 평가
                         */
                
                        while( sCurPos >= sFirstPos2 )
                        {
                            sCurPos--;
                            sDigit1--;
                            sDigit2--;
                            sResultDigit--;

                            sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
                            *sResultDigit = sCarryInfo->mDigit;
                        }

                
                        /**
                         * case B-2)의 ( A ~ C-1 ) 구간 평가
                         */

                        while( sCurPos >= sFirstPos1 )
                        {
                            sCurPos--;
                            sDigit1--;
                            sResultDigit--;

                            sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                            *sResultDigit = sCarryInfo->mDigit;
                        }
                    }
                }
            }

            /**
             * 최상위 Carry 처리
             */

            if( sCarryInfo->mCarry != 0 )
            {
                /**
                 * 부호가 같은 경우 : 최상위 digit 추가
                 */

                STL_TRY_THROW( ++sExponent <= DTL_NUMERIC_MAX_EXPONENT,
                               ERROR_OUT_OF_RANGE );

                sResultDigit[ -1 ] = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
                if( sDigitLen == DTL_NUMERIC_MAX_DIGIT_COUNT )
                {
                    sDigitLen--;
                    sCarryInfo  =
                        & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sResultDigit[ sDigitLen ] ) > 49 ];

                    if( sCarryInfo->mCarry == 0 )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        while( sCarryInfo->mCarry != 0 )
                        {
                            sDigitLen--;
                            sCarryInfo = & sAddMap[ sResultDigit[ sDigitLen ] + sBlank + sCarryInfo->mCarry ];
                            sResultDigit[ sDigitLen ] = sCarryInfo->mDigit;
                        }
                        sDigitLen++;
                    }
                }

                STL_DASSERT( sDigitLen >= 0 );
                aSumValue->mLength = sDigitLen + 2;
                
                sResultDigit += sDigitLen;
                while( sDigitLen >= 0 )
                {
                    sResultDigit--;
                    sResultDigit[ 1 ] = sResultDigit[ 0 ];
                    sDigitLen--;
                }

                DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric, sExponent );
            }
            else
            {
                aSumValue->mLength = sDigitLen + 1;
                DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                            sExponent );
            }
        }
    }    

    STL_RAMP( RAMP_EXIT );

#ifdef STL_DEBUG
    STL_DASSERT( aSumValue->mLength > 0 );
    STL_DASSERT( aSumValue->mLength <= DTL_NUMERIC_MAX_SIZE );

    for( sLoop = 1; sLoop < aSumValue->mLength; sLoop++ )
    {
        STL_DASSERT( ((stlUInt8*)aSumValue->mValue)[sLoop] <
                     DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE );
    }
#endif
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Negative Numeric & Negative Numeric 의 SUM 연산
 * @param[in,out]  aSumValue      dtlDataValue
 * @param[in]      aSrcValue      dtlDataValue
 * @param[out]     aErrorStack    에러 스택
 */
stlStatus dtfNumericSumNegaNega( dtlDataValue   * aSumValue,
                                 dtlDataValue   * aSrcValue,
                                 stlErrorStack  * aErrorStack )
{
    dtlNumericType  * sNumeric1;
    dtlNumericType  * sNumeric2;
    dtlNumericType  * sResultNumeric;

    stlInt32          sDigitLen;
    stlInt32          sExponent;
    stlInt32          sExponent1;
    stlInt32          sExponent2;

    stlUInt8        * sResultDigit;
    stlUInt8        * sOrgResultDigit;
    stlUInt8        * sTrailResultDigit;
    stlUInt8        * sDigit1;
    stlUInt8        * sDigit2;
    stlUInt8          sBlank;

    stlInt32          sFirstPos1;
    stlInt32          sLastPos1;
    stlInt32          sFirstPos2;
    stlInt32          sLastPos2;
    stlInt32          sCurPos;

    stlBool           sIsGreater;
    stlInt32          sLoop;
    stlInt8         * sTable;

    const dtlNumericCarryInfo  * sAddMap;
    const dtlNumericCarryInfo  * sCarryInfo; 

    STL_DASSERT( ( aSrcValue->mType == DTL_TYPE_FLOAT ) ||
                 ( aSrcValue->mType == DTL_TYPE_NUMBER ) );
    STL_DASSERT( ( aSumValue->mType == DTL_TYPE_FLOAT ) ||
                 ( aSumValue->mType == DTL_TYPE_NUMBER ) );

    
    sNumeric1      = DTF_NUMERIC( aSumValue );
    sNumeric2      = DTF_NUMERIC( aSrcValue );
    sResultNumeric = DTF_NUMERIC( aSumValue );
    
    if( DTL_NUMERIC_IS_ZERO( sNumeric1, aSumValue->mLength ) )
    {
        /* copy */
        stlMemcpy( sResultNumeric,
                   sNumeric2,
                   aSrcValue->mLength );
        aSumValue->mLength = aSrcValue->mLength;
        STL_THROW( RAMP_EXIT );
    }

    if( DTL_NUMERIC_IS_ZERO( sNumeric2, aSrcValue->mLength ) )
    {
        STL_THROW( RAMP_EXIT );
    }

    sDigit1      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
    sDigit2      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );

    sExponent1 = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sNumeric1 );
    sExponent2 = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sNumeric2 );
    
    if( sExponent1 == sExponent2 )
    {
        sDigit1 = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
        sDigit2 = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );

        sTable = dtlNumericLenCompTable[ aSumValue->mLength ][ aSrcValue->mLength ];

        for( sLoop = 0 ; sLoop < sTable[1] ; sLoop++ )
        {
            if( sDigit1[ sLoop ] != sDigit2[ sLoop ] )
            {
                break;
            }
        }

        if( sLoop == sTable[1] )
        {
            sIsGreater = ( sTable[0] >= 0 );
        }
        else
        {
            sIsGreater = sDigit1[ sLoop ] >= sDigit2[ sLoop ];
        }
    }
    else
    {
        if( sExponent1 < sExponent2 )
        {
            sIsGreater = STL_FALSE;
        }
        else
        {
            sIsGreater = STL_TRUE;
        }
    }

        
    /**
     * 절대값이 큰 수가 Numeric1이 된다.
     */
        
    if( sIsGreater == STL_FALSE )
    {
        /**
         * Numeric2 + Numeric1
         */

        sDigit1      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );
        sDigit2      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );

            
        /**
         * 분석 위치 정보 설정
         */
            
        sFirstPos1 = 1;
        sLastPos1  = DTL_NUMERIC_GET_DIGIT_COUNT( aSrcValue->mLength );
        sFirstPos2 = sExponent2 - sExponent1 + 1;
        sLastPos2  = sFirstPos2 + DTL_NUMERIC_GET_DIGIT_COUNT( aSumValue->mLength ) - 1;
        sExponent  = sExponent2;
    }
    else
    {
        /**
         * Numeric1 + Numeric2
         */
            
        sDigit1      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
        sDigit2      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );


        /**
         * 분석 위치 정보 설정
         */
            
        sFirstPos1 = 1;
        sLastPos1  = DTL_NUMERIC_GET_DIGIT_COUNT( aSumValue->mLength );
        sFirstPos2 = sExponent1 - sExponent2 + 1;
        sLastPos2  = sFirstPos2 + DTL_NUMERIC_GET_DIGIT_COUNT( aSrcValue->mLength ) - 1;
        sExponent  = sExponent1;
    }
    
    sResultDigit = DTL_NUMERIC_GET_DIGIT_PTR( sResultNumeric );
    sOrgResultDigit = sResultDigit;
        
        
    /**
     * Add 연산 수행
     */
        
    /*
     * Numeric에 대한 분석 위치 정보 ( A ~ D )
     * A : 1 (고정)
     * B : Numeric1's Digit Count
     * C : Numeric1's Exponent - Numeric2's Exponent + 1
     * D : C + Numeric2's Digit Count
     *     단, 최대값은 DTL_NUMERIC_MAX_DIGIT_COUNT
     *
     * case A)  B < C :
     * 
     *    ==>    A ~ B   /   B+1 ~ C-1   /   C ~ D
     *           digit1          0           digit2
     *
     *           DTLDTLDTLXX
     *                      YYYYYYYYY
     *                    
     *     
     * case B)  B >= C :
     * 
     *      B-1)  B > D
     *                    A ~ C-1   /   C ~ D   /   D+1 ~ B
     *                    digit1    digit1+digit2   digit1
     *
     *           DTLDTLDTLXX
     *                YYYYY
     *
     *      B-2)  B <= D
     *                    A ~ C-1   /   C ~ B   /   B+1 ~ D
     *                    digit1    digit1+digit2   digit2
     *
     *           DTLDTLDTLXX
     *                  YYYYYYYYY
     *                  
     * case C)  B == D :
     * 
     *    ==>    A ~ B   /   B+1 ~ D
     *           digit1   digit1+digit2
     *
     *           DTLDTLDTLXX
     *                 YYYYY
     *                    
     */

        
    sAddMap = dtdNumericDigitMap[ 0 ][ 0 ];
    sBlank = DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE;
    sResultNumeric->mData[0] = sBlank;

    if( sLastPos1 == sLastPos2 )
    {
        sCarryInfo    = dtdNumericInvalidCarryInfo;
        sDigitLen     = sLastPos1;
        sCurPos       = sLastPos1;
        sDigit1      += sLastPos1;
        sDigit2      += sLastPos2 - sFirstPos2 + 1;
        
        sResultDigit += sDigitLen;
        sTrailResultDigit = sResultDigit - 1;

            
        /**
         * case C)의 ( B+1 ~ D ) 구간 평가
         */
                
        while( sCurPos >= sFirstPos2 )
        {
            sCurPos--;
            sDigit1--;
            sDigit2--;
            sResultDigit--;

            sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
            *sResultDigit = sCarryInfo->mDigit;
        }
                
        /**
         * case C)의 ( A ~ B ) 구간 평가
         */

        if( sIsGreater == STL_TRUE )
        {
            if( sCarryInfo->mCarry != 0 )
            {
                while( sCurPos >= sFirstPos1 )
                {
                    sCurPos--;
                    sDigit1--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
            
                    if( sCarryInfo->mCarry == 0 )
                    {
                        sResultDigit = sOrgResultDigit;
                        break;
                    }
                }
            }
            else
            {
                sResultDigit = sOrgResultDigit;
            }
        }
        else
        {
            while( sCurPos >= sFirstPos1 )
            {
                sCurPos--;
                sDigit1--;
                sResultDigit--;

                sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                *sResultDigit = sCarryInfo->mDigit;
            }
        }
        
        /**
         * Trailing Zero 제거
         */

        if( *sTrailResultDigit == sBlank )
        {
            sResultDigit--;
            while( ( sDigitLen > 0 ) &&
                   ( sResultDigit[ sDigitLen ] == sBlank ) )
            {
                sDigitLen--;
            }
            sResultDigit++;
        }
        
        /**
         * 최상위 Carry 처리
         */

        if( sCarryInfo->mCarry != 0 )
        {
            /**
             * 부호가 같은 경우 : 최상위 digit 추가
             */

            STL_TRY_THROW( ++sExponent <= DTL_NUMERIC_MAX_EXPONENT,
                           ERROR_OUT_OF_RANGE );

            sResultDigit[ -1 ] = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 1 );
            if( sDigitLen == DTL_NUMERIC_MAX_DIGIT_COUNT )
            {
                sDigitLen--;
                sCarryInfo  =
                    & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sResultDigit[ sDigitLen ] ) > 49 ];

                if( sCarryInfo->mCarry == 0 )
                {
                    /* Do Nothing */
                }
                else
                {
                    while( sCarryInfo->mCarry != 0 )
                    {
                        sDigitLen--;
                        sCarryInfo = & sAddMap[ sResultDigit[ sDigitLen ] + sBlank + sCarryInfo->mCarry ];
                        sResultDigit[ sDigitLen ] = sCarryInfo->mDigit;
                    }
                    sDigitLen++;
                }
            }
            
            STL_DASSERT( sDigitLen >= 0 );
            aSumValue->mLength = sDigitLen + 2;
                
            sResultDigit += sDigitLen;
            while( sDigitLen >= 0 )
            {
                sResultDigit--;
                sResultDigit[ 1 ] = sResultDigit[ 0 ];
                sDigitLen--;
            }

            DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric, sExponent );
        }
        else
        {
            aSumValue->mLength = sDigitLen + 1;
            DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                        sExponent );
        }
    }
    else
    {
        /**
         * case 구분 ( case A  vs  case B )
         */

        if( sLastPos1 < sFirstPos2 )
        {
            /* CASE A */
            if( sLastPos2 > DTL_NUMERIC_MAX_DIGIT_COUNT )
            {
                sLastPos2  = DTL_NUMERIC_MAX_DIGIT_COUNT;
                sDigit2    += DTL_NUMERIC_MAX_DIGIT_COUNT - sFirstPos2 + 1;

                sCarryInfo  =
                    & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit2 ) > 49 ];
                
                sDigitLen  = sLastPos2;
                sCurPos    = sLastPos2;
                
                sDigit1      += sLastPos1;
                sResultDigit += sDigitLen;
                sTrailResultDigit = sResultDigit - 1;
            
                /**
                 * case A)의 ( C ~ D ) 구간 평가
                 */

                while( sCurPos >= sFirstPos2 )
                {
                    sCurPos--;
                    sDigit2--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit2 + sBlank + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }


                /**
                 * case A)의 ( B+1 ~ C-1 ) 구간 평가
                 */
                
                while( sCurPos > sLastPos1 )
                {
                    sCurPos--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ sBlank + sBlank + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }

                
                /**
                 * case A)의 ( A ~ B ) 구간 평가
                 */

                while( sCurPos >= sFirstPos1 )
                {
                    sCurPos--;
                    sDigit1--;
                    sResultDigit--;

                    sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                    *sResultDigit = sCarryInfo->mDigit;
                }

                /**
                 * 최상위 Carry 처리
                 */

                if( sCarryInfo->mCarry != 0 )
                {
                    /**
                     * 부호가 같은 경우 : 최상위 digit 추가
                     */

                    STL_TRY_THROW( ++sExponent <= DTL_NUMERIC_MAX_EXPONENT,
                                   ERROR_OUT_OF_RANGE );

                    aSumValue->mLength = 2;
                
                    *sResultDigit = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 1 );
                    DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric, sExponent );
                }
                else
                {
                    /**
                     * Trailing Zero 제거
                     */

                    if( *sTrailResultDigit == sBlank )
                    {
                        sResultDigit--;
                        while( ( sDigitLen > 0 ) &&
                               ( sResultDigit[ sDigitLen ] == sBlank ) )
                        {
                            sDigitLen--;
                        }
                        sResultDigit++;
                    }
                
                    aSumValue->mLength = sDigitLen + 1;
                    DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent );
                }
            }
            else
            {
                sCarryInfo = dtdNumericInvalidCarryInfo;
                sDigitLen  = sLastPos2;
                
                if( sIsGreater == STL_TRUE )
                {
                    sDigit2      += sLastPos2 - sFirstPos2 + 1;
                    sCurPos       = sLastPos2;
                    sResultDigit += sLastPos2;
                    
                    /**
                     * case A)의 ( C ~ D ) 구간 평가
                     */

                    while( sCurPos >= sFirstPos2 )
                    {
                        sCurPos--;
                        sDigit2--;
                        sResultDigit--;
                        *sResultDigit = *sDigit2;
                    }
                    
                    /**
                     * case A)의 ( B+1 ~ C-1 ) 구간 평가
                     */

                    while( sCurPos > sLastPos1 )
                    {
                        sCurPos--;
                        sResultDigit--;
                        *sResultDigit = sBlank;
                    }
                }
                else
                {
                    sDigit2      += sLastPos2 - sFirstPos2 + 1;
                    sDigit1      += sLastPos1;
                    sCurPos       = sLastPos2;
                    sResultDigit += sLastPos2;
                    
                    /**
                     * case A)의 ( C ~ D ) 구간 평가
                     */

                    while( sCurPos >= sFirstPos2 )
                    {
                        sCurPos--;
                        sDigit2--;
                        sResultDigit--;
                        *sResultDigit = *sDigit2;
                    }
                    
                    /**
                     * case A)의 ( B+1 ~ C-1 ) 구간 평가
                     */

                    while( sCurPos > sLastPos1 )
                    {
                        sCurPos--;
                        sResultDigit--;
                        *sResultDigit = sBlank;
                    }
                    
                    /**
                     * case A)의 ( A ~ B ) 구간 평가
                     */

                    while( sCurPos >= sFirstPos1 )
                    {
                        sCurPos--;
                        sDigit1--;
                        sResultDigit--;
                        *sResultDigit = *sDigit1;
                    }
                }
            
                aSumValue->mLength = sDigitLen + 1;
                DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                            sExponent );
            }
        }
        else
        {
            /**
             * CASE B-1  vs CASE B-2
             */
        
            if( sLastPos1 > sLastPos2 )
            {
                /*
                 * CASE B-1 
                 *
                 * digit 개수가 max digit count를 넘지 않는다.
                 * => round, trailing zero 필요 없음
                 */

                sCarryInfo    = dtdNumericInvalidCarryInfo;
                sDigitLen     = sLastPos1;
                sDigit2      += sLastPos2 - sFirstPos2 + 1;

                if( sIsGreater == STL_FALSE )
                {
                    sDigit1      += sLastPos1;
                    sCurPos       = sLastPos1;
                    sResultDigit += sLastPos1;
                    
                    /**
                     * case B-1)의 ( D+1 ~ B ) 구간 평가
                     */
                
                    while( sCurPos > sLastPos2 )
                    {
                        sCurPos--;
                        sDigit1--;
                        sResultDigit--;
                        *sResultDigit = *sDigit1;
                    }
                    
                    /**
                     * case B-1)의 ( C ~ D ) 구간 평가
                     */
                
                    while( sCurPos >= sFirstPos2 )
                    {
                        sCurPos--;
                        sDigit1--;
                        sDigit2--;
                        sResultDigit--;

                        sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
                        *sResultDigit = sCarryInfo->mDigit;
                    }

                
                    /**
                     * case B-1)의 ( A ~ C-1 ) 구간 평가
                     */

                    while( sCurPos >= sFirstPos1 )
                    {
                        sCurPos--;
                        sDigit1--;
                        sResultDigit--;

                        sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                        *sResultDigit = sCarryInfo->mDigit;
                    }
                }
                else
                {
                    sDigit1      += sLastPos2;
                    sCurPos       = sLastPos2;
                    sResultDigit += sLastPos2;
                    
                    /**
                     * case B-1)의 ( C ~ D ) 구간 평가
                     */
                
                    while( sCurPos >= sFirstPos2 )
                    {
                        sCurPos--;
                        sDigit1--;
                        sDigit2--;
                        sResultDigit--;

                        sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
                        *sResultDigit = sCarryInfo->mDigit;
                    }

                
                    /**
                     * case B-1)의 ( A ~ C-1 ) 구간 평가
                     */

                    if( sCarryInfo->mCarry != 0 )
                    {
                        while( sCurPos >= sFirstPos1 )
                        {
                            sCurPos--;
                            sDigit1--;
                            sResultDigit--;

                            sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                            *sResultDigit = sCarryInfo->mDigit;
                        
                            if( sCarryInfo->mCarry == 0 )
                            {
                                sResultDigit = sOrgResultDigit;
                                break;
                            }
                        }
                    }
                    else
                    {
                        sResultDigit = sOrgResultDigit;
                    }
                }
                
            }
            else
            {
                /* CASE B-2 */
                if( sLastPos2 > DTL_NUMERIC_MAX_DIGIT_COUNT )
                {
                    sLastPos2  = DTL_NUMERIC_MAX_DIGIT_COUNT;
                    sDigit2    += DTL_NUMERIC_MAX_DIGIT_COUNT - sFirstPos2 + 1;

                    sCarryInfo  =
                        & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit2 ) > 49 ];
                
                    sDigitLen  = sLastPos2;
                    sCurPos    = sLastPos2;
                    
                    sDigit1      += sLastPos1;
                    sResultDigit += sDigitLen;
                    sTrailResultDigit = sResultDigit - 1;
            
                    /**
                     * case B-2)의 ( B+1 ~ D ) 구간 평가
                     */

                    while( sCurPos > sLastPos1 )
                    {
                        sCurPos--;
                        sDigit2--;
                        sResultDigit--;

                        sCarryInfo = & sAddMap[ *sDigit2 + sBlank + sCarryInfo->mCarry ];
                        *sResultDigit = sCarryInfo->mDigit;
                    }
                    
                    /**
                     * case B-2)의 ( C ~ B ) 구간 평가
                     */
                
                    while( sCurPos >= sFirstPos2 )
                    {
                        sCurPos--;
                        sDigit1--;
                        sDigit2--;
                        sResultDigit--;

                        sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
                        *sResultDigit = sCarryInfo->mDigit;
                    }

                
                    /**
                     * case B-2)의 ( A ~ C-1 ) 구간 평가
                     */

                    if( sIsGreater == STL_TRUE )
                    {
                        if( sCarryInfo->mCarry != 0 )
                        {
                            while( sCurPos >= sFirstPos1 )
                            {
                                sCurPos--;
                                sDigit1--;
                                sResultDigit--;

                                sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                                *sResultDigit = sCarryInfo->mDigit;
                        
                                if( sCarryInfo->mCarry == 0 )
                                {
                                    sResultDigit = sOrgResultDigit;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            sResultDigit = sOrgResultDigit;
                        }
                    }
                    else
                    {
                        while( sCurPos >= sFirstPos1 )
                        {
                            sCurPos--;
                            sDigit1--;
                            sResultDigit--;

                            sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                            *sResultDigit = sCarryInfo->mDigit;
                        }
                    }
                
                    /**
                     * Trailing Zero 제거
                     */

                    if( *sTrailResultDigit == sBlank )
                    {
                        sResultDigit--;
                        while( ( sDigitLen > 0 ) &&
                               ( sResultDigit[ sDigitLen ] == sBlank ) )
                        {
                            sDigitLen--;
                        }
                        sResultDigit++;
                    }
                }
                else
                {
                    sCarryInfo = dtdNumericInvalidCarryInfo;
                    sDigitLen  = sLastPos2;
                    sDigit1   += sLastPos1;

                    sDigit2      += sLastPos2 - sFirstPos2 + 1;
                    sCurPos       = sLastPos2;
                    sResultDigit += sLastPos2;
                        
                    /**
                     * case B-2)의 ( B+1 ~ D ) 구간 평가
                     */

                    while( sCurPos > sLastPos1 )
                    {
                        sCurPos--;
                        sDigit2--;
                        sResultDigit--;
                        *sResultDigit = *sDigit2;
                    }
                        
                    if( sIsGreater == STL_TRUE )
                    {
                        /**
                         * case B-2)의 ( C ~ B ) 구간 평가
                         */
                
                        while( sCurPos >= sFirstPos2 )
                        {
                            sCurPos--;
                            sDigit1--;
                            sDigit2--;
                            sResultDigit--;

                            sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
                            *sResultDigit = sCarryInfo->mDigit;
                        }

                
                        /**
                         * case B-2)의 ( A ~ C-1 ) 구간 평가
                         */

                        if( sCarryInfo->mCarry != 0 )
                        {
                            while( sCurPos >= sFirstPos1 )
                            {
                                sCurPos--;
                                sDigit1--;
                                sResultDigit--;

                                sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                                *sResultDigit = sCarryInfo->mDigit;
                        
                                if( sCarryInfo->mCarry == 0 )
                                {
                                    sResultDigit = sOrgResultDigit;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            sResultDigit = sOrgResultDigit;
                        }
                    }
                    else
                    {
                        /**
                         * case B-2)의 ( C ~ B ) 구간 평가
                         */
                
                        while( sCurPos >= sFirstPos2 )
                        {
                            sCurPos--;
                            sDigit1--;
                            sDigit2--;
                            sResultDigit--;

                            sCarryInfo = & sAddMap[ *sDigit1 + *sDigit2 + sCarryInfo->mCarry ];
                            *sResultDigit = sCarryInfo->mDigit;
                        }

                
                        /**
                         * case B-2)의 ( A ~ C-1 ) 구간 평가
                         */

                        while( sCurPos >= sFirstPos1 )
                        {
                            sCurPos--;
                            sDigit1--;
                            sResultDigit--;

                            sCarryInfo = & sAddMap[ *sDigit1 + sBlank + sCarryInfo->mCarry ];
                            *sResultDigit = sCarryInfo->mDigit;
                        }
                    }
                }
            }

            /**
             * 최상위 Carry 처리
             */

            if( sCarryInfo->mCarry != 0 )
            {
                /**
                 * 부호가 같은 경우 : 최상위 digit 추가
                 */

                STL_TRY_THROW( ++sExponent <= DTL_NUMERIC_MAX_EXPONENT,
                               ERROR_OUT_OF_RANGE );

                sResultDigit[ -1 ] = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 1 );
                if( sDigitLen == DTL_NUMERIC_MAX_DIGIT_COUNT )
                {
                    sDigitLen--;
                    sCarryInfo  =
                        & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sResultDigit[ sDigitLen ] ) > 49 ];

                    if( sCarryInfo->mCarry == 0 )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        while( sCarryInfo->mCarry != 0 )
                        {
                            sDigitLen--;
                            sCarryInfo = & sAddMap[ sResultDigit[ sDigitLen ] + sBlank + sCarryInfo->mCarry ];
                            sResultDigit[ sDigitLen ] = sCarryInfo->mDigit;
                        }
                        sDigitLen++;
                    }
                }

                STL_DASSERT( sDigitLen >= 0 );
                aSumValue->mLength = sDigitLen + 2;
                
                sResultDigit += sDigitLen;
                while( sDigitLen >= 0 )
                {
                    sResultDigit--;
                    sResultDigit[ 1 ] = sResultDigit[ 0 ];
                    sDigitLen--;
                }

                DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric, sExponent );
            }
            else
            {
                aSumValue->mLength = sDigitLen + 1;
                DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                            sExponent );
            }
        }
    }    

    STL_RAMP( RAMP_EXIT );

#ifdef STL_DEBUG
    STL_DASSERT( aSumValue->mLength > 0 );
    STL_DASSERT( aSumValue->mLength <= DTL_NUMERIC_MAX_SIZE );

    for( sLoop = 1; sLoop < aSumValue->mLength; sLoop++ )
    {
        STL_DASSERT( ((stlUInt8*)aSumValue->mValue)[sLoop] <=
                     DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE );
    }
#endif
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Numeric 구조체에 대한 뺄셈 연산
 * @param[in]  aValue1      dtfNumericType
 * @param[in]  aValue2      dtfNumericType
 * @param[out] aResult      뺄셈 연산 결과
 * @param[out] aErrorStack  에러 스택
 */
stlStatus dtfNumericSubtraction( dtlDataValue        * aValue1,
                                 dtlDataValue        * aValue2,
                                 dtlDataValue        * aResult,
                                 stlErrorStack       * aErrorStack )
{
    dtlNumericType  * sNumeric1;
    dtlNumericType  * sNumeric2;
    dtlNumericType  * sResultNumeric;

    stlUInt8          sIsPositive1;
    stlUInt8          sIsPositive2;
    stlInt32          sDigitLen;
    stlInt32          sExponent;
    stlInt32          sExponent1;
    stlInt32          sExponent2;

    stlUInt8        * sResultDigit;
    stlUInt8        * sDigit1;
    stlUInt8        * sDigit2;
    stlUInt8          sBlank1;
    stlUInt8          sBlank2;

    stlInt32          sFirstPos1;
    stlInt32          sLastPos1;
    stlInt32          sFirstPos2;
    stlInt32          sLastPos2;
    stlInt32          sCurPos;

    stlBool           sIsGreater;
    stlBool           sIsReverse;
    stlInt32          sLoop;
    stlInt8         * sTable;

    const dtlNumericCarryInfo  * sAddMap;
    const dtlNumericCarryInfo  * sSubMap;
    const dtlNumericCarryInfo  * sCarryInfo; 

    STL_DASSERT( ( aValue1->mType == DTL_TYPE_FLOAT ) ||
                 ( aValue1->mType == DTL_TYPE_NUMBER ) );
    STL_DASSERT( ( aValue2->mType == DTL_TYPE_FLOAT ) ||
                 ( aValue2->mType == DTL_TYPE_NUMBER ) );

    
    sNumeric1      = DTF_NUMERIC( aValue1 );
    sNumeric2      = DTF_NUMERIC( aValue2 );
    sResultNumeric = DTF_NUMERIC( aResult );
    
    if( DTL_NUMERIC_IS_ZERO( sNumeric1, aValue1->mLength ) )
    {
        if( DTL_NUMERIC_IS_ZERO( sNumeric2, aValue2->mLength ) )
        {
            DTL_NUMERIC_SET_ZERO( sResultNumeric, aResult->mLength );
        }
        else
        {
            /* reverse */
            DTL_NUMERIC_SET_REVERSE_SIGN_AND_EXPONENT( sNumeric2, sResultNumeric );
            sDigitLen    = DTL_NUMERIC_GET_DIGIT_COUNT( aValue2->mLength );
            sDigit2      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );
            sResultDigit = DTL_NUMERIC_GET_DIGIT_PTR( sResultNumeric );
            while( sDigitLen )
            {
                sDigitLen--;
                sResultDigit[ sDigitLen ] =
                    DTL_NUMERIC_GET_REVERSE_DIGIT_FROM_DIGIT( & sDigit2[ sDigitLen ] );
            }
            aResult->mLength = aValue2->mLength;
        }
        STL_THROW( RAMP_EXIT );
    }

    if( DTL_NUMERIC_IS_ZERO( sNumeric2, aValue2->mLength ) )
    {
        /* copy */
        stlMemcpy( sResultNumeric,
                   sNumeric1,
                   aValue1->mLength );
        aResult->mLength = aValue1->mLength;
        STL_THROW( RAMP_EXIT );
    }

    /**
     * < numeric 간의 Subtraction 연산 >
     *
     * ## numeric1 과 numeric2의 Exponent를 고려한 Sub 방법 구분
     *
     * numeric1의 digit은 'X' 
     * numeric1의 digit은 'Y'
     * numeric2의 평가가 필요 없는 digit은 'O'로 표시
     *
     *              01234567890123456789 (최대 20자리 Digit)
     *              
     * case 1)
     *              A          B
     *  numeric1 :  DTLDTLDTLDTL
     *  numeric2 :        YYYYYYYYYYY
     *                    C         D
     *
     *              A             B
     *  numeric1 :  DTLDTLDTLDTLDTL
     *  numeric2 :           YYYYYYYYYYYOOO
     *                       C         D   
     *
     * case 2)
     *              A      B
     *  numeric1 :  DTLDTLXX
     *  numeric2 :              YYYYY
     *                          C   D
     *
     *              A        B       
     *  numeric1 :  DTLDTLDTLX
     *  numeric2 :              YYYYYYYYYOOO
     *                          C       D
     *
     *              A        B
     *  numeric1 :  DTLDTLDTLX
     *  numeric2 :            YYYYYYYY
     *                        C      D
     *
     *              A        B
     *  numeric1 :  DTLDTLDTLX
     *  numeric2 :            YYYYYYYYYYYOOO
     *                        C         D
     *  
     * case 3)
     *              A     B
     *  numeric1 :  DTLDTLX
     *  numeric2 :  YYYYYYY
     *              C     D
     *  
     *  numeric1 :  DTLDTLDTLDTLDTL
     *  numeric2 :  YYYYYYYYY
     *  ==> 두 numeric간의 위치 변경
     *                  A       B
     *      numeric1 :  DTLDTLDTL
     *      numeric2 :  YYYYYYYYYYYYYYY
     *                  C             D
     *
     *              A     B                  
     *  numeric1 :  DTLDTLX
     *  numeric2 :  YYYYYYYYYYYY
     *              C          D
     *
     *  
     * case 4)
     *              A             B
     *  numeric1 :  DTLDTLDTLDTLDTL
     *  numeric2 :     YYYYYYYYYYYY
     *                 C          D
     *
     *              A             B
     *  numeric1 :  DTLDTLDTLDTLDTL
     *  numeric2 :     YYYYYY
     *                 C    D
     *
     *  
     * ## case 구분 방법 ( 반드시 numeric1은 numeric2보다 최대 exponent가 같거나 크다 )
     * 
     *  1. numeric1의 마지막 digit과 numeric2의 처음 digit과의 exponent 비교
     *     : ( B vs C )
     *  2. numeric1의 마지막 digit과 numeric2의 마지막 digit과의 exponent 비교
     *     : ( B vs D )
     *     
     */

    sExponent1 = DTL_NUMERIC_GET_EXPONENT( sNumeric1 );
    sExponent2 = DTL_NUMERIC_GET_EXPONENT( sNumeric2 );

    sIsGreater = STL_TRUE;
    if( sExponent1 > sExponent2 )
    {
        sIsGreater = STL_TRUE;
    }
    else
    {
        if( sExponent1 < sExponent2 )
        {
            sIsGreater = STL_FALSE;
        }
        else
        {
            sDigit1 = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
            sDigit2 = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );

            sTable = dtlNumericLenCompTable[ aValue1->mLength ][ aValue2->mLength ];

            if( DTL_NUMERIC_IS_POSITIVE( sNumeric1 ) ==
                DTL_NUMERIC_IS_POSITIVE( sNumeric2 ) )
            {
                for( sLoop = 0 ; sLoop < sTable[1] ; sLoop++ )
                {
                    if( sDigit1[ sLoop ] != sDigit2[ sLoop ] )
                    {
                        break;
                    }
                }

                if( sLoop == sTable[1] )
                {
                    if( sTable[0] == 0 )
                    {
                        DTL_NUMERIC_SET_ZERO( sResultNumeric, aResult->mLength );
                        
                        STL_THROW( RAMP_EXIT );
                    }
                    else
                    {
                        sIsGreater = ( sTable[0] >= 0 );
                    }
                }
                else
                {
                    if( DTL_NUMERIC_IS_POSITIVE( sNumeric1 ) )
                    {
                        sIsGreater = sDigit1[ sLoop ] >= sDigit2[ sLoop ];
                    }
                    else
                    {
                        sIsGreater = sDigit1[ sLoop ] <= sDigit2[ sLoop ];
                    }
                }
            }
            else
            {
                for( sLoop = 0 ; sLoop < sTable[1] ; sLoop++ )
                {
                    if( sDigit1[ sLoop ] !=
                        DTL_NUMERIC_GET_REVERSE_DIGIT_FROM_DIGIT( & sDigit2[ sLoop ] ) )
                    {
                        break;
                    }
                }

                if( sLoop == sTable[1] )
                {
                    sIsGreater = ( sTable[0] >= 0 );
                }
                else
                {
                    if( DTL_NUMERIC_IS_POSITIVE( sNumeric1 ) )
                    {
                        sIsGreater = ( sDigit1[ sLoop ] >
                                       DTL_NUMERIC_GET_REVERSE_DIGIT_FROM_DIGIT( & sDigit2[ sLoop ] ) );
                    }
                    else
                    {
                        sIsGreater = ( sDigit1[ sLoop ] <
                                       DTL_NUMERIC_GET_REVERSE_DIGIT_FROM_DIGIT( & sDigit2[ sLoop ] ) );
                    }
                }
            }
        }
    }

        
    /**
     * 절대값이 큰 수가 Numeric1이 된다.
     */
        
    if( sIsGreater == STL_FALSE )
    {
        /**
         * Numeric2 + Numeric1
         */

        sDigit1      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );
        sDigit2      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
        sIsPositive1 = DTL_NUMERIC_IS_POSITIVE( sNumeric2 );
        sIsPositive2 = DTL_NUMERIC_IS_POSITIVE( sNumeric1 );

            
        /**
         * 분석 위치 정보 설정
         */
            
        sFirstPos1 = 1;
        sLastPos1  = DTL_NUMERIC_GET_DIGIT_COUNT( aValue2->mLength );
        sFirstPos2 = sExponent2 - sExponent1 + 1;
        sLastPos2  = sFirstPos2 + DTL_NUMERIC_GET_DIGIT_COUNT( aValue1->mLength ) - 1;
        sExponent  = sExponent2;

        sIsReverse = STL_TRUE;
    }
    else
    {
        /**
         * Numeric1 + Numeric2
         */
            
        sDigit1      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
        sDigit2      = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );
        sIsPositive1 = DTL_NUMERIC_IS_POSITIVE( sNumeric1 );
        sIsPositive2 = DTL_NUMERIC_IS_POSITIVE( sNumeric2 );


        /**
         * 분석 위치 정보 설정
         */
            
        sFirstPos1 = 1;
        sLastPos1  = DTL_NUMERIC_GET_DIGIT_COUNT( aValue1->mLength );
        sFirstPos2 = sExponent1 - sExponent2 + 1;
        sLastPos2  = sFirstPos2 + DTL_NUMERIC_GET_DIGIT_COUNT( aValue2->mLength ) - 1;
        sExponent  = sExponent1;

        sIsReverse = STL_FALSE;
    }
    sResultDigit = DTL_NUMERIC_GET_DIGIT_PTR( sResultNumeric );

    
    /**
     * Sub 연산 수행
     */
        
    /*
     * Numeric에 대한 분석 위치 정보 ( A ~ D )
     * A : 1 (고정)
     * B : Numeric1's Digit Count
     * C : Numeric1's Exponent - Numeric2's Exponent + 1
     * D : C + Numeric2's Digit Count
     *     단, 최대값은 DTL_NUMERIC_MAX_DIGIT_COUNT
     *
     * case A)  B < C :
     * 
     *    ==>    A ~ B   /   B+1 ~ C-1   /   C ~ D
     *           digit1          0           digit2
     *
     *           DTLDTLDTLXX
     *                      YYYYYYYYY
     *                    
     *     
     * case B)  B >= C :
     * 
     *      B-1)  B > D
     *                    A ~ C-1   /   C ~ D   /   D+1 ~ B
     *                    digit1    digit1+digit2   digit1
     *
     *           DTLDTLDTLXX
     *                YYYYY
     *
     *      B-2)  B <= D
     *                    A ~ C-1   /   C ~ B   /   B+1 ~ D
     *                    digit1    digit1+digit2   digit2
     *
     *           DTLDTLDTLXX
     *                  YYYYYYYYY
     */


    /**
     * case 구분 ( case A  vs  case B )
     */
        
    sSubMap = & dtdNumericDigitMap[ sIsPositive1 ][ !sIsPositive2 ][ 102 ];
    sBlank1 = dtlNumericZero[ sIsPositive1 ];
    sBlank2 = dtlNumericZero[ sIsPositive2 ];
    sResultNumeric->mData[0] = sBlank1;
                
        
    if( sLastPos1 < sFirstPos2 )
    {
        /* CASE A */
        if( sLastPos2 > DTL_NUMERIC_MAX_DIGIT_COUNT )
        {
            sLastPos2  = DTL_NUMERIC_MAX_DIGIT_COUNT;
            sDigit2    += DTL_NUMERIC_MAX_DIGIT_COUNT - sFirstPos2 + 1;

            sAddMap = dtdNumericDigitMap[ sIsPositive1 ][ !sIsPositive2 ];
            
            sCarryInfo  =
                & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sIsPositive2, *sDigit2 ) > 49 ];
                
            sDigitLen  = sLastPos2;
            sCurPos    = sLastPos2;
        }
        else
        {
            sCarryInfo = dtdNumericInvalidCarryInfo;
            sDigit2   += sLastPos2 - sFirstPos2 + 1;
            sDigitLen  = sLastPos2;
            sCurPos    = sLastPos2;
        }
        sDigit1      += sLastPos1;
        sResultDigit += sDigitLen;
            
                
        /**
         * case A)의 ( C ~ D ) 구간 평가
         */

        while( sCurPos >= sFirstPos2 )
        {
            sCurPos--;
            sDigit2--;
            sResultDigit--;

            sCarryInfo = & sSubMap[ sBlank1 - *sDigit2 + sCarryInfo->mCarry ];
            *sResultDigit = sCarryInfo->mDigit;
        }


        /**
         * case A)의 ( B+1 ~ C-1 ) 구간 평가
         */
                
        while( sCurPos > sLastPos1 )
        {
            sCurPos--;
            sResultDigit--;

            sCarryInfo = & sSubMap[ sBlank1 - sBlank2 + sCarryInfo->mCarry ];
            *sResultDigit = sCarryInfo->mDigit;
        }

                
        /**
         * case A)의 ( A ~ B ) 구간 평가
         */

        while( sCurPos >= sFirstPos1 )
        {
            sCurPos--;
            sDigit1--;
            sResultDigit--;

            sCarryInfo = & sSubMap[ *sDigit1 - sBlank2 + sCarryInfo->mCarry ];
           *sResultDigit = sCarryInfo->mDigit;
        }

               
        /**
         * 최상위 Carry 처리
         */

        if( sCarryInfo->mCarry != 0 )
        {
            /**
             * 부호가 같은 경우 : 최상위 digit 추가
             */

            STL_TRY_THROW( ++sExponent <= DTL_NUMERIC_MAX_EXPONENT,
                           ERROR_OUT_OF_RANGE );

            aResult->mLength = 2;
            if( sIsPositive1 )
            {
                *sResultDigit = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
                DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric, sExponent );
            }
            else
            {
                *sResultDigit = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 1 );
                DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric, sExponent );
            }
        }
        else
        {
            /**
             * Leading Zero 확인
             */

            sCurPos = 0;
            while( sResultDigit[ sCurPos ] == sBlank1 )
            {
                sCurPos++;
            }

            if( sCurPos > 0 )
            {
                if( sDigitLen == sCurPos )
                {
                    /* set zero */
                    DTL_NUMERIC_SET_ZERO( sResultNumeric, aResult->mLength );
                    aResult->mLength = DTL_NUMERIC_ZERO_LENGTH;
                }
                else
                {
                    /**
                     * Trailing Zero 제거
                     */

                    sResultDigit--;
                    while( ( sDigitLen > 0 ) &&
                           ( sResultDigit[ sDigitLen ] == sBlank1 ) )
                    {
                        sDigitLen--;
                    }
                    sDigitLen -= sCurPos;
                    STL_DASSERT( sDigitLen > 0 );
    

                    /**
                     * Leading Zero 제거
                     */

                    aResult->mLength = sDigitLen + 1;

                    sResultDigit++;
                    while( sDigitLen > 0 )
                    {
                        *sResultDigit = sResultDigit[ sCurPos ];
                        sResultDigit++;
                        sDigitLen--;
                    }

                    if( sIsPositive1 )
                    {
                        DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                    sExponent - sCurPos );
                    }
                    else
                    {
                        DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                    sExponent - sCurPos );
                    }
                }
            }
            else
            {
                /**
                 * Trailing Zero 제거
                 */

                sResultDigit--;
                while( ( sDigitLen > 0 ) &&
                       ( sResultDigit[ sDigitLen ] == sBlank1 ) )
                {
                    sDigitLen--;
                }
                STL_DASSERT( sDigitLen > 0 );

                aResult->mLength = sDigitLen + 1;
                if( sIsPositive1 )
                {
                    DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent );
                }
                else
                {
                    DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent );
                }
            }
        }
    }
    else
    {
        /**
         * CASE B-1  vs CASE B-2
         */
        
        if( sLastPos1 > sLastPos2 )
        {
            /*
             * CASE B-1 
             *
             * digit 개수가 max digit count를 넘지 않는다.
             * => round, trailing zero 필요 없음
             */

            sCarryInfo    = dtdNumericInvalidCarryInfo;
            sDigitLen     = sLastPos1;
            sCurPos       = sLastPos1;
            sDigit1      += sLastPos1;
            sDigit2      += sLastPos2 - sFirstPos2 + 1;
            sResultDigit += sDigitLen;

            
            /**
             * case B-1)의 ( D+1 ~ B ) 구간 평가
             */
                
            while( sCurPos > sLastPos2 )
            {
                sCurPos--;
                sDigit1--;
                sResultDigit--;

                sCarryInfo = & sSubMap[ *sDigit1 - sBlank2 + sCarryInfo->mCarry ];
                *sResultDigit = sCarryInfo->mDigit;
            }

            /**
             * case B-1)의 ( C ~ D ) 구간 평가
             */
                
            while( sCurPos >= sFirstPos2 )
            {
                sCurPos--;
                sDigit1--;
                sDigit2--;
                sResultDigit--;

                sCarryInfo = & sSubMap[ *sDigit1 - *sDigit2 + sCarryInfo->mCarry ];
                *sResultDigit = sCarryInfo->mDigit;
            }

                
            /**
             * case B-1)의 ( A ~ C-1 ) 구간 평가
             */

            while( sCurPos >= sFirstPos1 )
            {
                sCurPos--;
                sDigit1--;
                sResultDigit--;

                sCarryInfo = & sSubMap[ *sDigit1 - sBlank2 + sCarryInfo->mCarry ];
                *sResultDigit = sCarryInfo->mDigit;
            }
        }
        else
        {
            /* CASE B-2 */
            if( sLastPos2 > DTL_NUMERIC_MAX_DIGIT_COUNT )
            {
                sLastPos2  = DTL_NUMERIC_MAX_DIGIT_COUNT;
                sDigit2    += DTL_NUMERIC_MAX_DIGIT_COUNT - sFirstPos2 + 1;

                sAddMap = dtdNumericDigitMap[ sIsPositive1 ][ !sIsPositive2 ];
            
                sCarryInfo  =
                    & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sIsPositive2, *sDigit2 ) > 49 ];
                
                sDigitLen  = sLastPos2;
                sCurPos    = sLastPos2;
            }
            else
            {
                sCarryInfo = dtdNumericInvalidCarryInfo;
                sDigit2   += sLastPos2 - sFirstPos2 + 1;
                sDigitLen  = sLastPos2;
                sCurPos    = sLastPos2;
            }
            sDigit1      += sLastPos1;
            sResultDigit += sDigitLen;

            
            /**
             * case B-2)의 ( B+1 ~ D ) 구간 평가
             */

            while( sCurPos > sLastPos1 )
            {
                sCurPos--;
                sDigit2--;
                sResultDigit--;

                sCarryInfo = & sSubMap[ sBlank1 - *sDigit2 + sCarryInfo->mCarry ];
                *sResultDigit = sCarryInfo->mDigit;
            }


            /**
             * case B-2)의 ( C ~ B ) 구간 평가
             */
                
            while( sCurPos >= sFirstPos2 )
            {
                sCurPos--;
                sDigit1--;
                sDigit2--;
                sResultDigit--;

                sCarryInfo = & sSubMap[ *sDigit1 - *sDigit2 + sCarryInfo->mCarry ];
                *sResultDigit = sCarryInfo->mDigit;
            }

                
            /**
             * case B-2)의 ( A ~ C-1 ) 구간 평가
             */

            while( sCurPos >= sFirstPos1 )
            {
                sCurPos--;
                sDigit1--;
                sResultDigit--;

                sCarryInfo = & sSubMap[ *sDigit1 - sBlank2 + sCarryInfo->mCarry ];
                *sResultDigit = sCarryInfo->mDigit;
            }


            /**
             * Trailing Zero 제거
             */

            sResultDigit--;
            while( ( sDigitLen > 0 ) &&
                   ( sResultDigit[ sDigitLen ] == sBlank1 ) )
            {
                sDigitLen--;
            }
            sResultDigit++;

            if( sDigitLen == 0 )
            {
                if( sCarryInfo->mCarry == 0 )
                {
                    /* set zero */
                    DTL_NUMERIC_SET_ZERO( sResultNumeric, aResult->mLength );
                }
                else
                {
                    sExponent++;
                    aResult->mLength = 2;
                    if( sIsPositive1 )
                    {
                        *sResultDigit = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
                        DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric, sExponent );
                    }
                    else
                    {
                        *sResultDigit = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 1 );
                        DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric, sExponent );
                    }
                }
                
                STL_THROW( RAMP_EXIT );
            }
            else
            {
                /* Do Nothing */
            }
        }


        /**
         * 최상위 Carry 처리
         */

        if( sCarryInfo->mCarry != 0 )
        {
            /**
             * 부호가 같은 경우 : 최상위 digit 추가
             */

            STL_TRY_THROW( ++sExponent <= DTL_NUMERIC_MAX_EXPONENT,
                           ERROR_OUT_OF_RANGE );

            sResultDigit[ -1 ] = DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sIsPositive1, 1 );
            if( sDigitLen == DTL_NUMERIC_MAX_DIGIT_COUNT )
            {
                sAddMap = dtdNumericDigitMap[ sIsPositive1 ][ sIsPositive1 ];
                if( sIsPositive1 )
                {
                    sDigitLen--;
                    sCarryInfo  =
                        & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sResultDigit[ sDigitLen ] ) > 49 ];

                    if( sCarryInfo->mCarry == 0 )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        while( sCarryInfo->mCarry != 0 )
                        {
                            sDigitLen--;
                            sCarryInfo = & sAddMap[ sResultDigit[ sDigitLen ] + sBlank1 + sCarryInfo->mCarry ];
                            sResultDigit[ sDigitLen ] = sCarryInfo->mDigit;
                        }
                        sDigitLen++;
                    }
                }
                else
                {
                    sDigitLen--;
                    sCarryInfo  =
                        & sAddMap[ DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sResultDigit[ sDigitLen ] ) > 49 ];

                    if( sCarryInfo->mCarry == 0 )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        while( sCarryInfo->mCarry != 0 )
                        {
                            sDigitLen--;
                            sCarryInfo = & sAddMap[ sResultDigit[ sDigitLen ] + sBlank1 + sCarryInfo->mCarry ];
                            sResultDigit[ sDigitLen ] = sCarryInfo->mDigit;
                        }
                        sDigitLen++;
                    }
                }
            }

            STL_DASSERT( sDigitLen >= 0 );
            aResult->mLength = sDigitLen + 2;

            sResultDigit += sDigitLen;
            while( sDigitLen >= 0 )
            {
                sResultDigit--;
                sResultDigit[ 1 ] = sResultDigit[ 0 ];
                sDigitLen--;
            }

            DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sResultNumeric, sIsPositive1, sExponent );
        }
        else
        {
            /**
             * Leading Zero 확인
             */

            sCurPos = 0;
            while( sResultDigit[ sCurPos ] == sBlank1 )
            {
                sCurPos++;
                if( --sDigitLen == 0 )
                {
                    /* set zero */
                    DTL_NUMERIC_SET_ZERO( sResultNumeric, aResult->mLength );
                    STL_THROW( RAMP_EXIT );
                }
            }

            if( sCurPos > 0 )
            {
                /**
                 * Leading Zero 제거
                 */

                aResult->mLength = sDigitLen + 1;

                while( sDigitLen > 0 )
                {
                    *sResultDigit = sResultDigit[ sCurPos ];
                    sResultDigit++;
                    sDigitLen--;
                }

                if( sIsPositive1 )
                {
                    DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent - sCurPos );
                }
                else
                {
                    DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent - sCurPos );
                }
            }
            else
            {
                aResult->mLength = sDigitLen + 1;
                if( sIsPositive1 )
                {
                    DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent );
                }
                else
                {
                    DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric,
                                                                sExponent );
                }
            }
        }
    }

    /* reverse */
    if( sIsReverse )
    {
        /* reverse */
        DTL_NUMERIC_SET_REVERSE_SIGN_AND_EXPONENT( sResultNumeric, sResultNumeric );
        sDigitLen    = DTL_NUMERIC_GET_DIGIT_COUNT( aResult->mLength );
        sResultDigit = DTL_NUMERIC_GET_DIGIT_PTR( sResultNumeric );
        while( sDigitLen )
        {
            sDigitLen--;
            sResultDigit[ sDigitLen ] =
                DTL_NUMERIC_GET_REVERSE_DIGIT_FROM_DIGIT( & sResultDigit[ sDigitLen ] );
        }
    }
    
    STL_RAMP( RAMP_EXIT );

#ifdef STL_DEBUG
    STL_DASSERT( aResult->mLength > 0 );
    STL_DASSERT( aResult->mLength <= DTL_NUMERIC_MAX_SIZE );

    for( sLoop = 1; sLoop < aResult->mLength; sLoop++ )
    {
        STL_DASSERT( ((stlUInt8*)aResult->mValue)[sLoop] <=
                     DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE );
    }
#endif
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Numeric 구조체에 대한 곱셈 연산
 * @param[in]  aValue1      dtfNumericType
 * @param[in]  aValue2      dtfNumericType
 * @param[out] aResult      곱셈 연산 결과
 * @param[out] aErrorStack  에러 스택
 */
stlStatus dtfNumericMultiplication( dtlDataValue        * aValue1,
                                    dtlDataValue        * aValue2,
                                    dtlDataValue        * aResult,
                                    stlErrorStack       * aErrorStack )
{
    stlInt32          i;
    stlInt32          j;

    stlBool           sLeftIsPositive;
    stlBool           sRightIsPositive;

    stlInt32          sLeftDigitCount;
    stlInt32          sRightDigitCount;

    stlUInt64         sTmpLeftIntArr[(DTL_NUMERIC_MAX_DIGIT_COUNT - 1) / 4 + 1];
    stlUInt64         sTmpRightIntArr[(DTL_NUMERIC_MAX_DIGIT_COUNT - 1) / 4 + 1];
    stlUInt64         sTmpResultIntArr[((DTL_NUMERIC_MAX_DIGIT_COUNT - 1) / 4 + 1) * 2 + 1];

    stlUInt64       * sLeftIntArrEndPtr = &sTmpLeftIntArr[(DTL_NUMERIC_MAX_DIGIT_COUNT - 1) / 4];
    stlUInt64       * sRightIntArrEndPtr = &sTmpRightIntArr[(DTL_NUMERIC_MAX_DIGIT_COUNT - 1) / 4];
    stlUInt64       * sResultIntArrEndPtr = &sTmpResultIntArr[((DTL_NUMERIC_MAX_DIGIT_COUNT - 1) / 4 + 1) * 2];

    stlUInt64       * sLeftIntArrPtr;
    stlUInt64       * sRightIntArrPtr;
    stlUInt64       * sResultIntArrPtr = NULL;

    stlUInt64       * sResultIntArrBeginPtr = NULL;

    stlUInt64         sTmpInteger;
    stlUInt64         sTmpQuot;

    stlInt32          sLeftIntArrCount;
    stlInt32          sRightIntArrCount;
    stlInt32          sTmpCount;

    stlUInt8        * sDigitPtr;

    stlUInt32         sCarry;

    stlBool           sResultIsPositive;
    stlInt32          sResultDigitCount;
    stlInt32          sResultExponent;
    stlUInt8        * sResultDigitPtr;

    dtlNumericType  * sNumeric1;
    dtlNumericType  * sNumeric2;
    dtlNumericType  * sResult;
    
    STL_DASSERT( ( aValue1->mType == DTL_TYPE_FLOAT ) ||
                 ( aValue1->mType == DTL_TYPE_NUMBER ) );
    STL_DASSERT( ( aValue2->mType == DTL_TYPE_FLOAT ) ||
                 ( aValue2->mType == DTL_TYPE_NUMBER ) );

    
    /**
     * Macro 정의
     */

#define NM_TRANSFORM_DIGIT_TO_INTEGER_ARRAY( aDigitEndPtr,          \
                                             aDigitCount,           \
                                             aGetValueMacro,        \
                                             aIntArrPtr,            \
                                             aIntArrCount,          \
                                             aTmpCount )            \
    {                                                               \
        (aTmpCount) = (aDigitCount);                                \
        (aIntArrCount) = 0;                                         \
        while( (aTmpCount) >= 4 )                                   \
        {                                                           \
            (*(aIntArrPtr)) =                                       \
                aGetValueMacro( *(aDigitEndPtr) ) +                 \
                aGetValueMacro( *((aDigitEndPtr)-1) ) * 100 +       \
                aGetValueMacro( *((aDigitEndPtr)-2) ) * 10000 +     \
                aGetValueMacro( *((aDigitEndPtr)-3) ) * 1000000;    \
            (aTmpCount) -= 4;                                       \
            (aDigitEndPtr) -= 4;                                    \
            (aIntArrPtr)--;                                         \
            (aIntArrCount)++;                                       \
        }                                                           \
                                                                    \
        switch( (aTmpCount) )                                       \
        {                                                           \
            case 3:                                                 \
                (*(aIntArrPtr)) =                                   \
                aGetValueMacro( *(aDigitEndPtr) ) +                 \
                aGetValueMacro( *((aDigitEndPtr)-1) ) * 100 +       \
                aGetValueMacro( *((aDigitEndPtr)-2) ) * 10000;      \
                (aIntArrCount)++;                                   \
                break;                                              \
            case 2:                                                 \
                (*(aIntArrPtr)) =                                   \
                aGetValueMacro( *(aDigitEndPtr) ) +                 \
                aGetValueMacro( *((aDigitEndPtr)-1) ) * 100;        \
                (aIntArrCount)++;                                   \
                break;                                              \
            case 1:                                                 \
                (*(aIntArrPtr)) =                                   \
                aGetValueMacro( *(aDigitEndPtr) );                  \
                (aIntArrCount)++;                                   \
                break;                                              \
            case 0:                                                 \
                (aIntArrPtr)++;                                     \
                break;                                              \
            default:                                                \
                STL_ASSERT( 0 );                                    \
                break;                                              \
        }                                                           \
    }

#define NM_TRANSFORM_INTEGER_ARRAY_TO_DIGIT( aResultIntArrPtr,          \
                                             aResultDigitCount,         \
                                             aGetDigitMacro,            \
                                             aResultDigitPtr )          \
    {                                                                   \
        sTmpCount = (aResultDigitCount);                                \
        while( sTmpCount >= 4 )                                         \
        {                                                               \
            sTmpInteger = *(aResultIntArrPtr);                          \
            sTmpQuot= DTL_DIV_UINT32_BY_100( sTmpInteger );             \
            (aResultDigitPtr)[sTmpCount - 1] =                          \
                aGetDigitMacro( (sTmpInteger - sTmpQuot * 100) );       \
                                                                        \
            sTmpInteger = sTmpQuot;                                     \
            sTmpQuot= DTL_DIV_UINT32_BY_100( sTmpInteger );             \
            (aResultDigitPtr)[sTmpCount - 2] =                          \
                aGetDigitMacro( (sTmpInteger - sTmpQuot * 100) );       \
                                                                        \
            sTmpInteger = sTmpQuot;                                     \
            sTmpQuot= DTL_DIV_UINT32_BY_100( sTmpInteger );             \
            (aResultDigitPtr)[sTmpCount - 3] =                          \
                aGetDigitMacro( (sTmpInteger - sTmpQuot * 100) );       \
                                                                        \
            (aResultDigitPtr)[sTmpCount - 4] =                          \
                aGetDigitMacro( sTmpQuot );                             \
                                                                        \
            sTmpCount -= 4;                                             \
            (aResultIntArrPtr)--;                                       \
        }                                                               \
                                                                        \
        switch( sTmpCount )                                             \
        {                                                               \
            case 3:                                                     \
                sTmpInteger = *(aResultIntArrPtr);                      \
                sTmpQuot= DTL_DIV_UINT32_BY_100( sTmpInteger );         \
                (aResultDigitPtr)[sTmpCount - 1] =                      \
                    aGetDigitMacro( (sTmpInteger - sTmpQuot * 100) );   \
                                                                        \
                sTmpInteger = sTmpQuot;                                 \
                sTmpQuot= DTL_DIV_UINT32_BY_100( sTmpInteger );         \
                (aResultDigitPtr)[sTmpCount - 2] =                      \
                    aGetDigitMacro( (sTmpInteger - sTmpQuot * 100) );   \
                                                                        \
                (aResultDigitPtr)[sTmpCount - 3] =                      \
                    aGetDigitMacro( sTmpQuot );                         \
                break;                                                  \
            case 2:                                                     \
                sTmpInteger = *(aResultIntArrPtr);                      \
                sTmpQuot= DTL_DIV_UINT32_BY_100( sTmpInteger );         \
                (aResultDigitPtr)[sTmpCount - 1] =                      \
                    aGetDigitMacro( (sTmpInteger - sTmpQuot * 100) );   \
                                                                        \
                (aResultDigitPtr)[sTmpCount - 2] =                      \
                    aGetDigitMacro( sTmpQuot );                         \
                break;                                                  \
            case 1:                                                     \
                (aResultDigitPtr)[sTmpCount - 1] =                      \
                    aGetDigitMacro( *sResultIntArrPtr );                \
                break;                                                  \
            case 0:                                                     \
                break;                                                  \
            default:                                                    \
                STL_ASSERT( 0 );                                        \
                break;                                                  \
        }                                                               \
    }



    sNumeric1 = DTF_NUMERIC( aValue1 );
    sNumeric2 = DTF_NUMERIC( aValue2 );
    sResult   = DTF_NUMERIC( aResult );
    
    if( DTL_NUMERIC_IS_ZERO( sNumeric1, aValue1->mLength ) ||
        DTL_NUMERIC_IS_ZERO( sNumeric2, aValue2->mLength ) )
    {
        DTL_NUMERIC_SET_ZERO( sResult, aResult->mLength );
        STL_THROW( RAMP_FINISH );
    }


    /**
     * Left Positive Digit를 Integer Array로 변환
     */

    sLeftIsPositive = DTL_NUMERIC_IS_POSITIVE( sNumeric1 );
    sLeftDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( aValue1->mLength );
    sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
    sDigitPtr += (sLeftDigitCount - 1);
    sLeftIntArrPtr = sLeftIntArrEndPtr;

    if( sLeftIsPositive == STL_TRUE )
    {
        NM_TRANSFORM_DIGIT_TO_INTEGER_ARRAY( sDigitPtr,
                                             sLeftDigitCount,
                                             DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT,
                                             sLeftIntArrPtr,
                                             sLeftIntArrCount,
                                             sTmpCount );
    }
    else
    {
        NM_TRANSFORM_DIGIT_TO_INTEGER_ARRAY( sDigitPtr,
                                             sLeftDigitCount,
                                             DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT,
                                             sLeftIntArrPtr,
                                             sLeftIntArrCount,
                                             sTmpCount );
    }


    /**
     * Right Positive Digit를 Integer Array로 변환
     */

    sRightIsPositive = DTL_NUMERIC_IS_POSITIVE( sNumeric2 );
    sRightDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( aValue2->mLength );
    sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );
    sDigitPtr += (sRightDigitCount - 1);
    sRightIntArrPtr = sRightIntArrEndPtr;

    if( sRightIsPositive == STL_TRUE )
    {
        NM_TRANSFORM_DIGIT_TO_INTEGER_ARRAY( sDigitPtr,
                                             sRightDigitCount,
                                             DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT,
                                             sRightIntArrPtr,
                                             sRightIntArrCount,
                                             sTmpCount );
    }
    else
    {
        NM_TRANSFORM_DIGIT_TO_INTEGER_ARRAY( sDigitPtr,
                                             sRightDigitCount,
                                             DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT,
                                             sRightIntArrPtr,
                                             sRightIntArrCount,
                                             sTmpCount );
    }


    /**
     * Result 부호 및 exponent 설정
     */

    sResultIsPositive = (sLeftIsPositive == sRightIsPositive);
    sResultExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric1 ) +
                      DTL_NUMERIC_GET_EXPONENT( sNumeric2 );


    /**
     * Multiplication
     */

    sTmpCount = 0;

    for( i = 0; i < sLeftIntArrCount + sRightIntArrCount + 1; i++ )
    {
        sResultIntArrEndPtr[-i] = 0;
    }

    for( i = sRightIntArrCount - 1; i >= 0; i-- )
    {
        sResultIntArrPtr = &sResultIntArrEndPtr[sTmpCount];
        sTmpCount--;

        for( j = sLeftIntArrCount - 1; j >= 0; j-- )
        {
            sTmpInteger = sLeftIntArrPtr[j] * sRightIntArrPtr[i];
            if( sTmpInteger < 100000000 )
            {
                (*sResultIntArrPtr) += sTmpInteger;
            }
            else
            {
                sTmpQuot = (sTmpInteger / 100000000);
                (*sResultIntArrPtr) += (sTmpInteger - sTmpQuot * 100000000);
                sResultIntArrPtr[-1] += sTmpQuot;
            }
            sResultIntArrPtr--;
        }
    }

    sResultIntArrBeginPtr = sResultIntArrPtr;

    /* carry 처리 */
    sCarry = 0;
    sResultIntArrPtr = sResultIntArrEndPtr;
    sResultDigitCount = 0;
    while( sResultIntArrPtr > sResultIntArrBeginPtr )
    {
        (*sResultIntArrPtr) += sCarry;
        if( *sResultIntArrPtr >= 100000000 )
        {
            sCarry = (*sResultIntArrPtr) / 100000000;
            *sResultIntArrPtr -= (sCarry * 100000000);
        }
        else
        {
            sCarry = 0;
        }
        sResultIntArrPtr--;
        sResultDigitCount += 4;
    }

    (*sResultIntArrBeginPtr) += sCarry;
    if( *sResultIntArrBeginPtr == 0 )
    {
        sResultIntArrBeginPtr++;
        if( *sResultIntArrBeginPtr < 100 )
        {
            sResultDigitCount -= 3;
        }
        else if( *sResultIntArrBeginPtr < 10000 )
        {
            sResultDigitCount -= 2;
        }
        else if( *sResultIntArrBeginPtr < 1000000 )
        {
            sResultDigitCount -= 1;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        if( *sResultIntArrBeginPtr < 100 )
        {
            sResultDigitCount += 1;
        }
        else if( *sResultIntArrBeginPtr < 10000 )
        {
            sResultDigitCount += 2;
        }
        else if( *sResultIntArrBeginPtr < 1000000 )
        {
            sResultDigitCount += 3;
        }
        else
        {
            sResultDigitCount += 4;
        }
    }


    /**
     * Set Numeric
     */

    /* Result Exponent 계산 */
    if( sResultDigitCount == (sLeftDigitCount + sRightDigitCount) )
    {
        sResultExponent++;
    }

    /* 반올림 여부에 따른 Numeric 셋팅 */
    if( sResultDigitCount >= DTL_NUMERIC_MAX_DIGIT_COUNT )
    {
        /* 반올림 처리 */
        sResultIntArrEndPtr -= ((sResultDigitCount >> 2) - 5);
        switch( sResultDigitCount & 0x00000003 )
        {
            case 0:
                break;
            case 1:
                sResultIntArrPtr = sResultIntArrBeginPtr;
                (*sResultIntArrPtr) *= 1000000;
                while( sResultIntArrPtr < sResultIntArrEndPtr )
                {
                    sTmpQuot = sResultIntArrPtr[1] / 100;
                    (*sResultIntArrPtr) += sTmpQuot;
                    sResultIntArrPtr[1] = (sResultIntArrPtr[1] - (sTmpQuot * 100)) * 1000000;
                    sResultIntArrPtr++;
                }
                sResultIntArrEndPtr--;
                break;
            case 2:
                sResultIntArrPtr = sResultIntArrBeginPtr;
                (*sResultIntArrPtr) *= 10000;
                while( sResultIntArrPtr < sResultIntArrEndPtr )
                {
                    sTmpQuot = sResultIntArrPtr[1] / 10000;
                    (*sResultIntArrPtr) += sTmpQuot;
                    sResultIntArrPtr[1] = (sResultIntArrPtr[1] - (sTmpQuot * 10000)) * 10000;
                    sResultIntArrPtr++;
                }
                sResultIntArrEndPtr--;
                break;
            case 3:
                sResultIntArrPtr = sResultIntArrBeginPtr;
                (*sResultIntArrPtr) *= 100;
                while( sResultIntArrPtr < sResultIntArrEndPtr )
                {
                    sTmpQuot = sResultIntArrPtr[1] / 1000000;
                    (*sResultIntArrPtr) += sTmpQuot;
                    sResultIntArrPtr[1] = (sResultIntArrPtr[1] - (sTmpQuot * 1000000)) * 100;
                    sResultIntArrPtr++;
                }
                sResultIntArrEndPtr--;
                break;
            default:
                STL_DASSERT( 0 );
                break;
        }

        if( (*sResultIntArrBeginPtr) >= 10000000 )
        {
            /* First Digit가 2자리임 */
            (*sResultIntArrEndPtr) += 50;
            sResultDigitCount = 19;
        }
        else
        {
            /* First Digit가 1자리임 */
            (*sResultIntArrEndPtr) += 5;
            (*sResultIntArrEndPtr) = ((*sResultIntArrEndPtr) / 10) * 10;
            sResultDigitCount = 20;
        }

        sResultIntArrPtr = sResultIntArrEndPtr;

        if( *sResultIntArrPtr >= 100000000 )
        {
            sResultDigitCount = 16;
            sResultIntArrPtr--;
            (*sResultIntArrPtr) += 1;
            sResultIntArrEndPtr = sResultIntArrPtr;
            while( (sResultIntArrPtr > sResultIntArrBeginPtr) &&
                   (*sResultIntArrPtr >= 100000000) )
            {
                sResultIntArrPtr--;
                (*sResultIntArrPtr) += 1;
                sResultIntArrEndPtr = sResultIntArrPtr;
                sResultDigitCount -= 4;
            }

            if( *sResultIntArrBeginPtr >= 100000000 )
            {
                sResultDigitCount = 1;
                sResultExponent += 1;

                STL_TRY_THROW( sResultExponent <= DTL_NUMERIC_MAX_EXPONENT,
                               ERROR_OUT_OF_RANGE );

                if( sResultExponent < DTL_NUMERIC_MIN_EXPONENT )
                {
                    DTL_NUMERIC_SET_ZERO( sResult, aResult->mLength );
                    STL_THROW( RAMP_FINISH );
                }

                /* Set Header */
                aResult->mLength = sResultDigitCount + 1;
                DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sResult, sResultIsPositive, sResultExponent );

                /* Set Digit */
                sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sResult );
                *sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, 1 );

                STL_THROW( RAMP_FINISH );
            }
        }

        STL_TRY_THROW( sResultExponent <= DTL_NUMERIC_MAX_EXPONENT,
                       ERROR_OUT_OF_RANGE );

        if( sResultExponent < DTL_NUMERIC_MIN_EXPONENT )
        {
            DTL_NUMERIC_SET_ZERO( sResult, aResult->mLength );
            STL_THROW( RAMP_FINISH );
        }

        /* Set Digit */
        sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sResult );
        sResultIntArrPtr = sResultIntArrBeginPtr;
        sTmpCount = sResultDigitCount;

        while( sResultIntArrPtr < sResultIntArrEndPtr )
        {
            sTmpInteger = *sResultIntArrPtr;
            sTmpQuot = sTmpInteger / 100;
            sResultDigitPtr[3] =
                DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, (sTmpInteger - sTmpQuot * 100) );

            sTmpInteger = sTmpQuot;
            sTmpQuot = sTmpInteger / 100;
            sResultDigitPtr[2] =
                DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, (sTmpInteger - sTmpQuot * 100) );

            sTmpInteger = sTmpQuot;
            sTmpQuot = sTmpInteger / 100;
            sResultDigitPtr[1] =
                DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, (sTmpInteger - sTmpQuot * 100) );

            sResultDigitPtr[0] =
                DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, sTmpQuot );

            sResultIntArrPtr++;
            sResultDigitPtr += 4;
            sTmpCount -= 4;
        }

        switch( sTmpCount )
        {
            case 4:
                sTmpInteger = *sResultIntArrPtr;
                sTmpQuot = sTmpInteger / 100;
                sResultDigitPtr[3] =
                    DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, (sTmpInteger - sTmpQuot * 100) );

                sTmpInteger = sTmpQuot;
                sTmpQuot = sTmpInteger / 100;
                sResultDigitPtr[2] =
                    DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, (sTmpInteger - sTmpQuot * 100) );

                sTmpInteger = sTmpQuot;
                sTmpQuot = sTmpInteger / 100;
                sResultDigitPtr[1] =
                    DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, (sTmpInteger - sTmpQuot * 100) );

                sResultDigitPtr[0] =
                    DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, sTmpQuot );

                break;
            case 3:
                sTmpInteger = (*sResultIntArrPtr) / 100;
                sTmpQuot = sTmpInteger / 100;
                sResultDigitPtr[2] =
                    DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, (sTmpInteger - sTmpQuot * 100) );

                sTmpInteger = sTmpQuot;
                sTmpQuot = sTmpInteger / 100;
                sResultDigitPtr[1] =
                    DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, (sTmpInteger - sTmpQuot * 100) );

                sResultDigitPtr[0] =
                    DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, sTmpQuot );

                break;
            default:
                STL_DASSERT( 0 );
                break;
        }
    }
    else
    {
        STL_TRY_THROW( sResultExponent <= DTL_NUMERIC_MAX_EXPONENT,
                       ERROR_OUT_OF_RANGE );

        if( sResultExponent < DTL_NUMERIC_MIN_EXPONENT )
        {
            DTL_NUMERIC_SET_ZERO( sResult, aResult->mLength );
            STL_THROW( RAMP_FINISH );
        }

        /* Set Digit */
        sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sResult );

        sResultIntArrPtr = sResultIntArrEndPtr;
        if( sResultIsPositive == STL_TRUE )
        {
            NM_TRANSFORM_INTEGER_ARRAY_TO_DIGIT( sResultIntArrPtr,
                                                 sResultDigitCount,
                                                 DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE,
                                                 sResultDigitPtr );
        }
        else
        {
            NM_TRANSFORM_INTEGER_ARRAY_TO_DIGIT( sResultIntArrPtr,
                                                 sResultDigitCount,
                                                 DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE,
                                                 sResultDigitPtr );
        }
    }

    /**
     * Trailing Zero 제거
     */

    sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sResult ) + sResultDigitCount - 1;
    while( *sResultDigitPtr ==
           DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, 0 ) )
    {
        sResultDigitCount--;
        sResultDigitPtr--;
    }

    STL_DASSERT( sResultDigitCount > 0 );


    /* Set Header */
    aResult->mLength = sResultDigitCount + 1;
    DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sResult, sResultIsPositive, sResultExponent );

    STL_RAMP( RAMP_FINISH );

#ifdef STL_DEBUG
    STL_DASSERT( aResult->mLength > 0 );
    STL_DASSERT( aResult->mLength <= DTL_NUMERIC_MAX_SIZE );

    for( i = 1; i < aResult->mLength; i++ )
    {
        STL_DASSERT( ((stlUInt8*)aResult->mValue)[i] <=
                     DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE );
    }
#endif

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_STRING_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Numeric 구조체에 대한 나눗셈 연산
 * @param[in]  aValue1      dtfNumericType
 * @param[in]  aValue2      dtfNumericType
 * @param[out] aResult      나눗셈 연산 결과
 * @param[out] aErrorStack  에러 스택
 */
stlStatus dtfNumericDivision( dtlDataValue        * aValue1,
                              dtlDataValue        * aValue2,
                              dtlDataValue        * aResult,
                              stlErrorStack       * aErrorStack )
{
    stlInt32          i;

    stlBool           sLeftIsPositive;
    stlBool           sRightIsPositive;

    stlInt32          sLeftDigitCount;
    stlInt32          sRightDigitCount;

    stlInt32          sLeftExponent;
    stlInt32          sRightExponent;

    stlUInt8        * sLeftDigitPtr;
    stlUInt8        * sRightDigitPtr;

    stlBool           sResultIsPositive;
    stlInt32          sResultDigitCount;
    stlUInt8        * sResultDigitPtr;
    stlInt32          sResultExponent;

    stlUInt64         sTmpValue;
    stlUInt64         sTmpQuot;

    stlBool           sCarry;

    stlInt32          sTmpDigitCount;

    dtlNumericType  * sNumeric1;
    dtlNumericType  * sNumeric2;
    dtlNumericType  * sResult;

    STL_DASSERT( ( aValue1->mType == DTL_TYPE_FLOAT ) ||
                 ( aValue1->mType == DTL_TYPE_NUMBER ) );
    STL_DASSERT( ( aValue2->mType == DTL_TYPE_FLOAT ) ||
                 ( aValue2->mType == DTL_TYPE_NUMBER ) );


    /**
     * Macro 정의
     */

#define NM_GET_POSITIVE_NUMERIC_INFO( aNumeric,                                         \
                                      aLength,                                          \
                                      aIsPositive,                                      \
                                      aDigitCount,                                      \
                                      aExponent,                                        \
                                      aDigitPtr )                                       \
    {                                                                                   \
        if( DTL_NUMERIC_IS_POSITIVE( (aNumeric) ) )                                     \
        {                                                                               \
            (aIsPositive) = STL_TRUE;                                                   \
            (aDigitCount) = DTL_NUMERIC_GET_DIGIT_COUNT( (aLength) );                   \
            (aExponent) = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( (aNumeric) );   \
            (aDigitPtr) = DTL_NUMERIC_GET_DIGIT_PTR( (aNumeric) );                      \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            (aIsPositive) = STL_FALSE;                                                  \
            (aDigitCount) = DTL_NUMERIC_GET_DIGIT_COUNT( (aLength) );                   \
            (aExponent) = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( (aNumeric) );   \
            (aDigitPtr) = DTL_NUMERIC_GET_DIGIT_PTR( (aNumeric) );                      \
        }                                                                               \
    }


    sNumeric1 = DTF_NUMERIC( aValue1 );
    sNumeric2 = DTF_NUMERIC( aValue2 );
    sResult   = DTF_NUMERIC( aResult );

    /**
     * Fourier Division Algorithm
     * ----------------------------------
     *   c     c1, c2, c3, ...
     *  --- = ----------------- = b1, b2, b3, ... = b
     *   a     a1, a2, a3, ...
     *  (c = c1, c2의 의미는 c = c1 * 100 + c2 이다.)
     *
     */


    /**
     * N / 0 인 경우
     */

    STL_TRY_THROW( DTL_NUMERIC_IS_ZERO( sNumeric2, aValue2->mLength) == STL_FALSE, ERROR_DIV_BY_ZERO );


    /**
     * 0 / N 인 경우
     */

    if( DTL_NUMERIC_IS_ZERO( sNumeric1, aValue1->mLength ) )
    {
        DTL_NUMERIC_SET_ZERO( sResult, aResult->mLength );
        STL_THROW( RAMP_FINISH );
    }


    /**
     * 기본 정보 가져오기
     */

    NM_GET_POSITIVE_NUMERIC_INFO( sNumeric1,
                                  aValue1->mLength,
                                  sLeftIsPositive,
                                  sLeftDigitCount,
                                  sLeftExponent,
                                  sLeftDigitPtr );

    NM_GET_POSITIVE_NUMERIC_INFO( sNumeric2,
                                  aValue2->mLength,
                                  sRightIsPositive,
                                  sRightDigitCount,
                                  sRightExponent,
                                  sRightDigitPtr );

    sResultIsPositive = (sLeftIsPositive == sRightIsPositive);


    /**
     * Right Digit Count에 따른 Div 수행
     */

    if( sRightDigitCount < 9 )
    {
        stlUInt64         sLeftValue;
        stlUInt64         sRightValue;

        stlInt32          sLeftReadPos;
        stlInt32          sLeftRemainedDigitCount;
        stlInt32          sMoveDigitCount;

        stlInt32          sResultWritePos;

        stlUInt64         sQuot;
        stlInt32          sTmpExponent;


        /**
         * Exponent 계산
         */

        sTmpExponent = sLeftExponent - sRightExponent;


        /**
         * 초기 Left Value 셋팅
         */

        sLeftValue = 0;
        if( sLeftDigitCount < sRightDigitCount )
        {
            for( i = 0; i < sLeftDigitCount; i++ )
            {
                sLeftValue =
                    ( (sLeftValue * 100) + 
                      DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sLeftIsPositive, sLeftDigitPtr[i] ) );
            }

            sLeftReadPos = sLeftDigitCount;
            sLeftRemainedDigitCount = 0;

            sLeftValue *= gPreDefinedPow[(sRightDigitCount-i)*2];
        }
        else
        {
            for( i = 0; i < sRightDigitCount; i++ )
            {
                sLeftValue =
                    ( (sLeftValue * 100) + 
                      DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sLeftIsPositive, sLeftDigitPtr[i] ) );
            }
            sLeftReadPos = sRightDigitCount;
            sLeftRemainedDigitCount = sLeftDigitCount - sRightDigitCount;
        }


        /**
         * 초기 Right Value 셋팅
         */

        sRightValue = 0;
        for( i = 0; i < sRightDigitCount; i++ )
        {
            sRightValue =
                ( (sRightValue *100) +
                  DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sRightIsPositive, sRightDigitPtr[i] ) );
        }


        /**
         * 기타 필요 정보 설정
         */

        sMoveDigitCount = 9 - sRightDigitCount;
        sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sResult );
        sResultWritePos = 0;


        /**
         * 첫번째 Div 연산
         */

        sQuot = sLeftValue / sRightValue;
        if( sQuot > 0 )
        {
            STL_DASSERT( sQuot < 100 );
            sResultDigitPtr[sResultWritePos] =
                DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, sQuot );
            sResultWritePos++;
            sLeftValue -= sQuot * sRightValue;
        }
        else
        {
            sTmpExponent -= 1;
        }


        /**
         * Result Digit Count가 20이 되거나 
         * Remainder가 0일때까지 Div를 수행
         */

        while( ((DTL_NUMERIC_MAX_DIGIT_COUNT - sResultWritePos) > sMoveDigitCount) &&
               ( (sLeftRemainedDigitCount > 0) || (sLeftValue > 0) ) )
        {
            if( sLeftRemainedDigitCount < sMoveDigitCount )
            {
                for( i = 0; i < sLeftRemainedDigitCount; i++ )
                {
                    sLeftValue =
                        ( (sLeftValue * 100) +
                          DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sLeftIsPositive,
                                                            sLeftDigitPtr[sLeftReadPos] ) );
                    sLeftReadPos++;
                }

                sLeftRemainedDigitCount = 0;

                sLeftValue *= gPreDefinedPow[(sMoveDigitCount-i)*2];
            }
            else
            {
                for( i = 0; i < sMoveDigitCount; i++ )
                {
                    sLeftValue =
                        ( (sLeftValue * 100) +
                          DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sLeftIsPositive,
                                                            sLeftDigitPtr[sLeftReadPos] ) );
                    sLeftReadPos++;
                }
                sLeftRemainedDigitCount -= sMoveDigitCount;
            }

            sQuot = sLeftValue / sRightValue;
            sLeftValue = sLeftValue - sQuot * sRightValue;

            sTmpValue = sQuot;
            for( i = sMoveDigitCount - 1; i >= 0; i-- )
            {
                sTmpQuot = sTmpValue / 100;
                sResultDigitPtr[sResultWritePos+i] =
                    DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive,
                                                      (sTmpValue - sTmpQuot * 100) );
                sTmpValue = sTmpQuot;
            }
            sResultWritePos += sMoveDigitCount;
        }

        if( (sResultWritePos < DTL_NUMERIC_MAX_DIGIT_COUNT) &&
            ( (sLeftRemainedDigitCount > 0) || (sLeftValue > 0) ) )
        {
            sMoveDigitCount = DTL_NUMERIC_MAX_DIGIT_COUNT - sResultWritePos;

            if( sLeftRemainedDigitCount < sMoveDigitCount )
            {
                for( i = 0; i < sLeftRemainedDigitCount; i++ )
                {
                    sLeftValue =
                        ( (sLeftValue * 100) +
                          DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sLeftIsPositive,
                                                            sLeftDigitPtr[sLeftReadPos] ) );
                    sLeftReadPos++;
                }

                sLeftRemainedDigitCount = 0;

                sLeftValue *= gPreDefinedPow[(sMoveDigitCount-i)*2];
            }
            else
            {
                for( i = 0; i < sMoveDigitCount; i++ )
                {
                    sLeftValue =
                        ( (sLeftValue * 100) +
                          DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sLeftIsPositive,
                                                            sLeftDigitPtr[sLeftReadPos] ) );
                    sLeftReadPos++;
                }
                sLeftRemainedDigitCount -= sMoveDigitCount;
            }

            sQuot = sLeftValue / sRightValue;
            sLeftValue = sLeftValue - sQuot * sRightValue;

            sTmpValue = sQuot;
            for( i = sMoveDigitCount - 1; i >= 0; i-- )
            {
                sTmpQuot = sTmpValue / 100;
                sResultDigitPtr[sResultWritePos+i] =
                    DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive,
                                                      (sTmpValue - sTmpQuot * 100) );
                sTmpValue = sTmpQuot;
            }
            sResultWritePos += sMoveDigitCount;
        }


        /**
         * Traling Zero 제거
         */

        for( i = sResultWritePos - 1; i >= 0; i-- )
        {
            if( DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sResultIsPositive, sResultDigitPtr[i] ) != 0 )
            {
                break;
            }
        }

        STL_DASSERT( i >= 0 );

        sResultDigitCount = i + 1;
        sResultExponent = sTmpExponent;


        /**
         * 반올림 처리
         */

        if( sResultDigitCount == DTL_NUMERIC_MAX_DIGIT_COUNT )
        {
            if( DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sResultIsPositive, sResultDigitPtr[0] ) < 10 )
            {
                /* 첫 Digit이 1자리인 경우 */
                if( DTL_NUMERIC_GET_VALUE_FROM_DIGIT(
                        sResultIsPositive,
                        sResultDigitPtr[ sResultDigitCount - 1 ]
                    ) >= 95 )
                {
                    sCarry = STL_TRUE;
                    sResultDigitCount--;
                }
                else
                {
                    sResultDigitPtr[ sResultDigitCount - 1 ] =
                        DTL_NUMERIC_GET_DIGIT_FROM_VALUE(
                            sResultIsPositive,
                            ( (DTL_NUMERIC_GET_VALUE_FROM_DIGIT(
                                  sResultIsPositive,
                                  sResultDigitPtr[ sResultDigitCount - 1 ]
                              ) + 5) / 10 ) * 10
                        );
                    sCarry = STL_FALSE;
                }
            }
            else
            {
                /* 첫 Digit이 2자리인 경우 */
                if( DTL_NUMERIC_GET_VALUE_FROM_DIGIT(
                        sResultIsPositive,
                        sResultDigitPtr[ sResultDigitCount - 1 ]
                    ) >= 50 )
                {
                    sCarry = STL_TRUE;
                }
                else
                {
                    sCarry = STL_FALSE;
                }
                sResultDigitCount--;
            }

            while( (sCarry == STL_TRUE) && (sResultDigitCount > 0) )
            {
                if( DTL_NUMERIC_GET_VALUE_FROM_DIGIT(
                        sResultIsPositive,
                        sResultDigitPtr[sResultDigitCount-1] ) ==
                    99 )
                {
                    sResultDigitCount--;
                }
                else
                {
                    if( sResultIsPositive == STL_TRUE )
                    {
                        sResultDigitPtr[sResultDigitCount-1]++;
                    }
                    else
                    {
                        sResultDigitPtr[sResultDigitCount-1]--;
                    }
                    sCarry = STL_FALSE;
                    break;
                }
            }

            if( sResultDigitCount == 0 )
            {
                /* 첫번째 Digit에서도 반올림이 일어난 경우 */
                STL_DASSERT( sCarry == STL_TRUE );
                sResultDigitPtr[0] =
                    DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, 1 );
                sResultExponent++;
                sResultDigitCount = 1;
            }
        }

        STL_TRY_THROW( sResultExponent <= DTL_NUMERIC_MAX_EXPONENT,
                       ERROR_OUT_OF_RANGE );

        if( sResultExponent < DTL_NUMERIC_MIN_EXPONENT )
        {
            DTL_NUMERIC_SET_ZERO( sResult, aResult->mLength );
            STL_THROW( RAMP_FINISH );
        }

        aResult->mLength = sResultDigitCount + 1;
        DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sResult, sResultIsPositive, sResultExponent );
    }
    else
    {
#define NM_FOURIER_COUNT                ( DTL_NUMERIC_MAX_DIGIT_COUNT + 1 + 1 + 1 )
#define NM_LAST_RESULT_INT_ARRAY_POS    ( DTL_NUMERIC_MAX_DIGIT_COUNT - 1 )
#define NM_INT_ARRAY_BASE_VALUE         ( 100 )

        stlInt32          j;

        stlInt64          sLeftIntArr[NM_FOURIER_COUNT + 1] = {0, };
        stlInt64          sRightIntArr[NM_FOURIER_COUNT + 1] = {0, };
        stlInt64          sResultIntArr[NM_FOURIER_COUNT] = {0, };
        stlInt64          sRemainderIntArr[NM_FOURIER_COUNT] = {0, };

        stlInt64        * sTmpIntArrPtr;

        stlUInt8        * sTmpDigitPtr;

        stlInt64          sTmpFourier;

        stlBool           sIsRightTwoDigitAdjust = STL_FALSE;
        stlInt64          sTmpValue1;
        stlInt64          sTmpValue2;        
        
        
        /**
         * Right Value 변환
         */
        
        sTmpIntArrPtr = sRightIntArr;
        sTmpDigitCount = sRightDigitCount;
        sTmpDigitPtr = sRightDigitPtr;
        
        for( i = 0; i < sTmpDigitCount; i++ )
        {
            sTmpIntArrPtr[i] = DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sRightIsPositive, sTmpDigitPtr[i] );
        }
        
        if( sTmpIntArrPtr[0] < 10 )
        {
            sIsRightTwoDigitAdjust = STL_TRUE;
            
            /*
             * 두 자리 숫자로 맞춘다.
             * 예) 1234567890123456789012345678901234567
             *     [12][34][56][78][90]...[56][70]
             */
            sTmpIntArrPtr[0] = sTmpIntArrPtr[0] * 10;
            
            for( i = 1; i < NM_FOURIER_COUNT - 1; i++ )
            {
                sTmpIntArrPtr[i - 1] += sTmpIntArrPtr[i] / 10;
                sTmpIntArrPtr[i] -= sTmpIntArrPtr[i] / 10 * 10;
                sTmpIntArrPtr[i] = sTmpIntArrPtr[i] * 10;                
            }
        }
        else
        {
            /* Do Nothing */
        }
        
        /**
         * Left Value 변환
         */
        
        sTmpIntArrPtr = sLeftIntArr;
        sTmpDigitCount = sLeftDigitCount;
        sTmpDigitPtr = sLeftDigitPtr;
        
        for( i = 0; i < sTmpDigitCount; i++)
        {
            sTmpIntArrPtr[i] = DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sLeftIsPositive, sTmpDigitPtr[i] );
        }
        
        if( sIsRightTwoDigitAdjust == STL_TRUE )
        {
            /*
             * right를 두자리 숫자로 맞추기 위해 조정된 경우,
             * left도 그에 따라 조정한다.
             */

            if( sTmpIntArrPtr[0] < 10 )
            {
                /*
                 * 예) 1234567890123456789012345678901234567
                 *     [12][34][56][78][90]...[56][70]
                 */
                
                sTmpIntArrPtr[0] = sTmpIntArrPtr[0] * 10;
                
                for( i = 1; i < NM_FOURIER_COUNT - 1; i++ )
                {
                    sTmpIntArrPtr[i - 1] += sTmpIntArrPtr[i] / 10;
                    sTmpIntArrPtr[i] -= sTmpIntArrPtr[i] / 10 * 10;
                    sTmpIntArrPtr[i] = sTmpIntArrPtr[i] * 10;                
                }
            }
            else
            {
                /* 
                 * 예) 12345678901234567890123456789012345678
                 *     [1][23][45][67][89]...[67][80]
                 */     
                sTmpValue1 = sTmpIntArrPtr[0];
                sTmpIntArrPtr[0] = sTmpValue1 / 10;
                sTmpValue1 = sTmpValue1 - ( sTmpValue1 / 10 * 10 );
                
                for( i = 1; i < NM_FOURIER_COUNT - 1; i++ )
                {
                    sTmpValue2 = sTmpIntArrPtr[i];
                    sTmpIntArrPtr[i] = sTmpValue1 * 10;
                    sTmpIntArrPtr[i] += sTmpValue2 / 10;
                    sTmpValue1 = sTmpValue2 - ( sTmpValue2 / 10 * 10 );
                }
                
                sLeftExponent++;
            }
        }
        else
        {
            /* Do Nothing */
        }
        

        /**
         * Fourier 연산
         */

        sTmpFourier = sLeftIntArr[0] * NM_INT_ARRAY_BASE_VALUE + sLeftIntArr[1];
        sResultIntArr[0] = sTmpFourier / sRightIntArr[0];
        sRemainderIntArr[0] = sTmpFourier - sResultIntArr[0] * sRightIntArr[0];

        for( i = 1; i < NM_FOURIER_COUNT; i++ )
        {
            sTmpFourier = sRemainderIntArr[i-1] * NM_INT_ARRAY_BASE_VALUE + sLeftIntArr[i+1];
            for( j = 1; j <= i; j++ )
            {
                sTmpFourier -= (sResultIntArr[i-j] * sRightIntArr[j]);
            }

            sResultIntArr[i] = sTmpFourier / sRightIntArr[0];
            sRemainderIntArr[i] = sTmpFourier - sResultIntArr[i] * sRightIntArr[0];
        }
        
        sTmpFourier = 0;
        for( i = NM_FOURIER_COUNT - 1; i >= 0; i-- )
        {
            sResultIntArr[i] += sTmpFourier;
            if( sResultIntArr[i] < 0 )
            {
                sTmpFourier = (sResultIntArr[i] + 1) / NM_INT_ARRAY_BASE_VALUE - 1;
                sResultIntArr[i] -= (sTmpFourier * NM_INT_ARRAY_BASE_VALUE);
            }
            else
            {
                sTmpFourier = sResultIntArr[i] / NM_INT_ARRAY_BASE_VALUE;
                sResultIntArr[i] = sResultIntArr[i] - sTmpFourier * NM_INT_ARRAY_BASE_VALUE;
            }
        }

        /*
         * sResultIntArr[0]의 몫 >= 100 인 경우,
         * 배열 및 exponent 조정하여, result 결과가 정확할 수 있도록 한다.
         * 예) 18446744073709551617 / 18446744073709551617 인 경우,
         *   => 1844/18 => sResultIntArr[0] = 102
         *   => 현시점 : sTmpFourier = 1  &  sResultIntArr[0] = 02
         *   => 조정후 : sResultIntArr[0] = 1
         *               sResultIntArr[1] = 02
         *               ...
         */ 
        if( sTmpFourier > 0 )
        {
            for( i = NM_FOURIER_COUNT - 1; i > 0; i-- )
            {
                sResultIntArr[i] = sResultIntArr[i-1];
            }

            sResultIntArr[0] = sTmpFourier;

            sLeftExponent++;
            
        }
        else
        {
            /* Do Nothing */
        }

        
        /**
         * 반올림 처리
         */
        
        if( sResultIntArr[0] < (NM_INT_ARRAY_BASE_VALUE / 10) )
        {
            sResultIntArr[NM_LAST_RESULT_INT_ARRAY_POS] += 5;
            if( sResultIntArr[NM_LAST_RESULT_INT_ARRAY_POS] >= NM_INT_ARRAY_BASE_VALUE )
            {
                sCarry = 1;
                sResultDigitCount = 19;
            }
            else
            {
                sCarry = 0;
                sResultIntArr[NM_LAST_RESULT_INT_ARRAY_POS]
                    = sResultIntArr[NM_LAST_RESULT_INT_ARRAY_POS] / 10 * 10;
                sResultDigitCount = 20;
            }
        }
        else
        {
            sResultIntArr[NM_LAST_RESULT_INT_ARRAY_POS] += 50;
            if( sResultIntArr[NM_LAST_RESULT_INT_ARRAY_POS] >= NM_INT_ARRAY_BASE_VALUE )
            {
                sCarry = 1;
            }
            else
            {
                sCarry = 0;
            }
            
            sResultDigitCount = 19;            
        }
        
        /*
         * 반올림
         */
        
        for( i = (NM_LAST_RESULT_INT_ARRAY_POS - 1) ;
             (sCarry > 0) && (i >= 0);
             i-- )
        {
            sResultIntArr[i] += sCarry;
            
            if( sResultIntArr[i] >= NM_INT_ARRAY_BASE_VALUE )
            {
                sCarry = 1;
                sResultDigitCount --;
            }
            else
            {
                break;
            }
        }
        
        sResultExponent = sLeftExponent - sRightExponent - 1;
        sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sResult );
        if( sResultDigitCount == 0 )
        {
            /* 첫번째 Digit에서도 반올림이 일어난 경우 */
            STL_DASSERT( sCarry == STL_TRUE );
            
            sResultExponent++;
            STL_TRY_THROW( sResultExponent <= DTL_NUMERIC_MAX_EXPONENT,
                           ERROR_OUT_OF_RANGE );
            
            if( sResultExponent < DTL_NUMERIC_MIN_EXPONENT )
            {
                DTL_NUMERIC_SET_ZERO( sResult, aResult->mLength );
                STL_THROW( RAMP_FINISH );
            }

            sResultDigitPtr[0] = DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, 1 );
            sResultDigitCount = 1;

            aResult->mLength = sResultDigitCount + 1;
            DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sResult, sResultIsPositive, sResultExponent );
        }
        else
        {
            STL_TRY_THROW( sResultExponent <= DTL_NUMERIC_MAX_EXPONENT,
                           ERROR_OUT_OF_RANGE );

            if( sResultExponent < DTL_NUMERIC_MIN_EXPONENT )
            {
                DTL_NUMERIC_SET_ZERO( sResult, aResult->mLength );
                STL_THROW( RAMP_FINISH );
            }

            sTmpDigitCount = sResultDigitCount;
            sTmpIntArrPtr = sResultIntArr;

            for( i = 0; i < sTmpDigitCount; i++ )
            {
                *sResultDigitPtr =
                    DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, sResultIntArr[i] );
                sResultDigitPtr++;
            }
            
            
            /**
             * Trailing Zero 제거
             */

            sResultDigitPtr--;
            while( *sResultDigitPtr ==
                   DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sResultIsPositive, 0 ) )
            {
                sResultDigitCount--;
                sResultDigitPtr--;
            }

            aResult->mLength = sResultDigitCount + 1;
            DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sResult, sResultIsPositive, sResultExponent );
        }
    }

    STL_RAMP( RAMP_FINISH );

#ifdef STL_DEBUG
    STL_DASSERT( aResult->mLength > 0 );
    STL_DASSERT( aResult->mLength <= DTL_NUMERIC_MAX_SIZE );

    for( i = 1; i < aResult->mLength; i++ )
    {
        STL_DASSERT( ((stlUInt8*)aResult->mValue)[i] <=
                     DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE );
    }
#endif

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_STRING_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_DIV_BY_ZERO )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMERIC_DIVISION_BY_ZERO,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Numeric 구조체에 대한 ROUND(반올림) 연산
 * @param[in]  aValue1      dtfNumericType
 * @param[in]  aScale       scale
 * @param[out] aResult      ROUND(반올림) 연산 결과
 * @param[out] aErrorStack  에러 스택
 */
stlStatus dtfNumericRounding( dtlDataValue        * aValue1,
                              stlInt32              aScale,
                              dtlDataValue        * aResult,
                              stlErrorStack       * aErrorStack )
{
    stlBool       sIsPositive;
    stlInt32      sDigitCount;
    stlInt32      sExponent;
    stlInt32      sRoundPos;
    stlUInt8    * sDigitPtr;
    stlUInt8      sFirstValueSize;
    stlUInt8      sTmpValue;
    stlUInt8      sTmpPos;

    stlUInt8    * sResultDigitPtr;
    stlInt32      sResultDigitCount;
    
    dtlNumericType  * sNumeric;
    dtlNumericType  * sResult;

    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aValue1, aErrorStack );    
    DTL_CHECK_TYPE( DTL_TYPE_NUMBER, aResult, aErrorStack );

    sNumeric = DTF_NUMERIC( aValue1 );
    sResult  = DTF_NUMERIC( aResult );
    
    if( DTL_NUMERIC_IS_ZERO( sNumeric, aValue1->mLength ) == STL_TRUE )
    {
        DTL_NUMERIC_SET_ZERO( sResult, aResult->mLength );
        STL_THROW( RAMP_FINISH );
    }

    if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) == STL_TRUE )
    {
        sIsPositive = STL_TRUE;
        sExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sNumeric );
        sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( aValue1->mLength );
        sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
        sFirstValueSize = ( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr ) < 10
                            ? 1 : 2 );
    }
    else
    {
        sIsPositive = STL_FALSE;
        sExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sNumeric );
        sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( aValue1->mLength );
        sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
        sFirstValueSize = ( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr ) < 10
                            ? 1 : 2 );
    }

    sRoundPos = sExponent * 2 + aScale + sFirstValueSize;

    if( sRoundPos < 0 )
    {
        DTL_NUMERIC_SET_ZERO( sResult, aResult->mLength );
    }
    else if( sRoundPos >= ((sDigitCount-1) * 2 + sFirstValueSize) )
    {
        DTL_COPY_DATA_VALUE( aResult->mBufferSize, aValue1, aResult );
    }
    else
    {
        if( sFirstValueSize == 1 )
        {
            sRoundPos++;
        }

        sTmpPos = (sRoundPos / 2);

        sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sResult );

        if( sRoundPos & ((stlInt32)1) )
        {
            sTmpValue = DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sIsPositive, sDigitPtr[sTmpPos] ) + 5;
            if( sTmpValue >= 100 )
            {
                while( (sTmpValue >= 100) && sTmpPos > 0 )
                {
                    sTmpPos--;
                    sTmpValue = DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sIsPositive, sDigitPtr[sTmpPos] ) + 1;
                }
            }
            else
            {
                if( sTmpPos > 0 )
                {
                    sTmpValue = (sTmpValue / 10) * 10;
                }
                else
                {
                    if( sTmpValue < 10 )
                    {
                        DTL_NUMERIC_SET_ZERO( sResult, aResult->mLength );
                    }
                    else
                    {
                        *sResultDigitPtr =
                            DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sIsPositive, (sTmpValue / 10 * 10) );
                        DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sResult, sIsPositive, sExponent );
                        aResult->mLength = 2;
                    }
                    STL_THROW( RAMP_FINISH );
                }
            }
        }
        else
        {
            sTmpValue = DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sIsPositive, sDigitPtr[sTmpPos] ) + 50;
            if( sTmpValue >= 100 )
            {
                while( (sTmpValue >= 100) && sTmpPos > 0 )
                {
                    sTmpPos--;
                    sTmpValue = DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sIsPositive, sDigitPtr[sTmpPos] ) + 1;
                }
            }
            else
            {
                if( sTmpPos > 0 )
                {
                    sTmpPos--;
                    sTmpValue = DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sIsPositive, sDigitPtr[sTmpPos] );
                }
                else
                {
                    DTL_NUMERIC_SET_ZERO( sResult, aResult->mLength );
                    STL_THROW( RAMP_FINISH );
                }
            }

        }

        if( sTmpValue >= 100 )
        {
            *sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sIsPositive, 1 );
            DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sResult, sIsPositive, sExponent + 1 );
            aResult->mLength = 2;
        }
        else
        {
            sResultDigitCount = sTmpPos + 1;
            sResultDigitPtr[sTmpPos] = DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sIsPositive, sTmpValue );
            while( sTmpPos > 0 )
            {
                sTmpPos--;
                sResultDigitPtr[sTmpPos] = sDigitPtr[sTmpPos];
            }

            DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sResult, sIsPositive, sExponent );
            aResult->mLength = sResultDigitCount + 1;
        }
    }

    STL_RAMP( RAMP_FINISH );

#ifdef STL_DEBUG
    STL_DASSERT( aResult->mLength > 0 );
    STL_DASSERT( aResult->mLength <= DTL_NUMERIC_MAX_SIZE );

    stlInt32 i;
    for( i = 1; i < aResult->mLength; i++ )
    {
        STL_DASSERT( ((stlUInt8*)aResult->mValue)[i] <=
                     DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE );
    }
#endif

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */

