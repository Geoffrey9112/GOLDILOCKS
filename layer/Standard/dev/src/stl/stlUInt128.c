/*******************************************************************************
 * stlUInt128.c
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
 * @file stlUInt128.c
 * @brief Standard Layer UInt128 Routines
 */

#include <stlDef.h>
#include <stlTypes.h>
#include <stlMath.h>
#include <stlUInt128.h>


/**
 * @addtogroup stlUInt128
 * @{
 */

const stlUInt128 gUInt128MAX = { 
    STL_UINT64_MAX,
    STL_UINT64_MAX
};

const stlUInt128 gUInt128MIN = { 
    STL_UINT64_MIN,
    STL_UINT64_MIN
};

const stlUInt128 g10Pow19 = {
    0,
    STL_UINT64_C(10000000000000000000)
};

const stlUInt64 gPreDefinedPow[20] = {
    STL_UINT64_C(1),
    STL_UINT64_C(10),
    STL_UINT64_C(100),
    STL_UINT64_C(1000),
    STL_UINT64_C(10000),
    STL_UINT64_C(100000),
    STL_UINT64_C(1000000),
    STL_UINT64_C(10000000),
    STL_UINT64_C(100000000),
    STL_UINT64_C(1000000000),
    STL_UINT64_C(10000000000),
    STL_UINT64_C(100000000000),
    STL_UINT64_C(1000000000000),
    STL_UINT64_C(10000000000000),
    STL_UINT64_C(100000000000000),
    STL_UINT64_C(1000000000000000),
    STL_UINT64_C(10000000000000000),
    STL_UINT64_C(100000000000000000),
    STL_UINT64_C(1000000000000000000),
    STL_UINT64_C(10000000000000000000)
};

const stlInt32 gPreDefinedPowMax = 19;

const stlUInt128 gPreDefinedPowUInt128[39] = {
    /* 0 */
    { STL_UINT64_C(0), STL_UINT64_C(1) },
    { STL_UINT64_C(0), STL_UINT64_C(10) },
    { STL_UINT64_C(0), STL_UINT64_C(100) },
    { STL_UINT64_C(0), STL_UINT64_C(1000) },
    { STL_UINT64_C(0), STL_UINT64_C(10000) },

    /* 5 */
    { STL_UINT64_C(0), STL_UINT64_C(100000) },
    { STL_UINT64_C(0), STL_UINT64_C(1000000) },
    { STL_UINT64_C(0), STL_UINT64_C(10000000) },
    { STL_UINT64_C(0), STL_UINT64_C(100000000) },
    { STL_UINT64_C(0), STL_UINT64_C(1000000000) },

    /* 10 */
    { STL_UINT64_C(0), STL_UINT64_C(10000000000) },
    { STL_UINT64_C(0), STL_UINT64_C(100000000000) },
    { STL_UINT64_C(0), STL_UINT64_C(1000000000000) },
    { STL_UINT64_C(0), STL_UINT64_C(10000000000000) },
    { STL_UINT64_C(0), STL_UINT64_C(100000000000000) },

    /* 15 */
    { STL_UINT64_C(0), STL_UINT64_C(1000000000000000) },
    { STL_UINT64_C(0), STL_UINT64_C(10000000000000000) },
    { STL_UINT64_C(0), STL_UINT64_C(100000000000000000) },
    { STL_UINT64_C(0), STL_UINT64_C(1000000000000000000) },
    { STL_UINT64_C(0), STL_UINT64_C(10000000000000000000) },

    /* 20 */
    { STL_UINT64_C(5), STL_UINT64_C(7766279631452241920) },
    { STL_UINT64_C(54), STL_UINT64_C(3875820019684212736) },
    { STL_UINT64_C(542), STL_UINT64_C(1864712049423024128) },
    { STL_UINT64_C(5421), STL_UINT64_C(200376420520689664) },
    { STL_UINT64_C(54210), STL_UINT64_C(2003764205206896640) },

    /* 25 */
    { STL_UINT64_C(542101), STL_UINT64_C(1590897978359414784) },
    { STL_UINT64_C(5421010), STL_UINT64_C(15908979783594147840) },
    { STL_UINT64_C(54210108), STL_UINT64_C(11515845246265065472) },
    { STL_UINT64_C(542101086), STL_UINT64_C(4477988020393345024) },
    { STL_UINT64_C(5421010862), STL_UINT64_C(7886392056514347008) },

    /* 30 */
    { STL_UINT64_C(54210108624), STL_UINT64_C(5076944270305263616) },
    { STL_UINT64_C(542101086242), STL_UINT64_C(13875954555633532928) },
    { STL_UINT64_C(5421010862427), STL_UINT64_C(9632337040368467968) },
    { STL_UINT64_C(54210108624275), STL_UINT64_C(4089650035136921600) },
    { STL_UINT64_C(542101086242752), STL_UINT64_C(4003012203950112768) },

    /* 35 */
    { STL_UINT64_C(5421010862427522), STL_UINT64_C(3136633892082024448) },
    { STL_UINT64_C(54210108624275221), STL_UINT64_C(12919594847110692864) },
    { STL_UINT64_C(542101086242752217), STL_UINT64_C(68739955140067328) },
    { STL_UINT64_C(5421010862427522170), STL_UINT64_C(687399551400673280) }
};

const stlInt32 gPreDefinedPowMaxUInt128 = 38;

const stlUInt64 gUInt64HighBitMask[64] = {
    STL_UINT64_C(0x8000000000000000),
    STL_UINT64_C(0xC000000000000000),
    STL_UINT64_C(0xE000000000000000),
    STL_UINT64_C(0xF000000000000000),

    STL_UINT64_C(0xF800000000000000),
    STL_UINT64_C(0xFC00000000000000),
    STL_UINT64_C(0xFE00000000000000),
    STL_UINT64_C(0xFF00000000000000),

    STL_UINT64_C(0xFF80000000000000),
    STL_UINT64_C(0xFFC0000000000000),
    STL_UINT64_C(0xFFE0000000000000),
    STL_UINT64_C(0xFFF0000000000000),

    STL_UINT64_C(0xFFF8000000000000),
    STL_UINT64_C(0xFFFC000000000000),
    STL_UINT64_C(0xFFFE000000000000),
    STL_UINT64_C(0xFFFF000000000000),

    STL_UINT64_C(0xFFFF800000000000),
    STL_UINT64_C(0xFFFFC00000000000),
    STL_UINT64_C(0xFFFFE00000000000),
    STL_UINT64_C(0xFFFFF00000000000),

    STL_UINT64_C(0xFFFFF80000000000),
    STL_UINT64_C(0xFFFFFC0000000000),
    STL_UINT64_C(0xFFFFFE0000000000),
    STL_UINT64_C(0xFFFFFF0000000000),

    STL_UINT64_C(0xFFFFFF8000000000),
    STL_UINT64_C(0xFFFFFFC000000000),
    STL_UINT64_C(0xFFFFFFE000000000),
    STL_UINT64_C(0xFFFFFFF000000000),

    STL_UINT64_C(0xFFFFFFF800000000),
    STL_UINT64_C(0xFFFFFFFC00000000),
    STL_UINT64_C(0xFFFFFFFE00000000),
    STL_UINT64_C(0xFFFFFFFF00000000),

    STL_UINT64_C(0xFFFFFFFF80000000),
    STL_UINT64_C(0xFFFFFFFFC0000000),
    STL_UINT64_C(0xFFFFFFFFE0000000),
    STL_UINT64_C(0xFFFFFFFFF0000000),

    STL_UINT64_C(0xFFFFFFFFF8000000),
    STL_UINT64_C(0xFFFFFFFFFC000000),
    STL_UINT64_C(0xFFFFFFFFFE000000),
    STL_UINT64_C(0xFFFFFFFFFF000000),

    STL_UINT64_C(0xFFFFFFFFFF800000),
    STL_UINT64_C(0xFFFFFFFFFFC00000),
    STL_UINT64_C(0xFFFFFFFFFFE00000),
    STL_UINT64_C(0xFFFFFFFFFFF00000),

    STL_UINT64_C(0xFFFFFFFFFFF80000),
    STL_UINT64_C(0xFFFFFFFFFFFC0000),
    STL_UINT64_C(0xFFFFFFFFFFFE0000),
    STL_UINT64_C(0xFFFFFFFFFFFF0000),

    STL_UINT64_C(0xFFFFFFFFFFFF8000),
    STL_UINT64_C(0xFFFFFFFFFFFFC000),
    STL_UINT64_C(0xFFFFFFFFFFFFE000),
    STL_UINT64_C(0xFFFFFFFFFFFFF000),

    STL_UINT64_C(0xFFFFFFFFFFFFF800),
    STL_UINT64_C(0xFFFFFFFFFFFFFC00),
    STL_UINT64_C(0xFFFFFFFFFFFFFE00),
    STL_UINT64_C(0xFFFFFFFFFFFFFF00),

    STL_UINT64_C(0xFFFFFFFFFFFFFF80),
    STL_UINT64_C(0xFFFFFFFFFFFFFFC0),
    STL_UINT64_C(0xFFFFFFFFFFFFFFE0),
    STL_UINT64_C(0xFFFFFFFFFFFFFFF0),

    STL_UINT64_C(0xFFFFFFFFFFFFFFF8),
    STL_UINT64_C(0xFFFFFFFFFFFFFFFC),
    STL_UINT64_C(0xFFFFFFFFFFFFFFFE),
    STL_UINT64_C(0xFFFFFFFFFFFFFFFF)
};

const stlUInt64 gUInt64LowBitMask[64] = {
    STL_UINT64_C(0x0000000000000001),
    STL_UINT64_C(0x0000000000000003),
    STL_UINT64_C(0x0000000000000007),
    STL_UINT64_C(0x000000000000000F),

    STL_UINT64_C(0x000000000000001F),
    STL_UINT64_C(0x000000000000003F),
    STL_UINT64_C(0x000000000000007F),
    STL_UINT64_C(0x00000000000000FF),

    STL_UINT64_C(0x00000000000001FF),
    STL_UINT64_C(0x00000000000003FF),
    STL_UINT64_C(0x00000000000007FF),
    STL_UINT64_C(0x0000000000000FFF),

    STL_UINT64_C(0x0000000000001FFF),
    STL_UINT64_C(0x0000000000003FFF),
    STL_UINT64_C(0x0000000000007FFF),
    STL_UINT64_C(0x000000000000FFFF),

    STL_UINT64_C(0x000000000001FFFF),
    STL_UINT64_C(0x000000000003FFFF),
    STL_UINT64_C(0x000000000007FFFF),
    STL_UINT64_C(0x00000000000FFFFF),

    STL_UINT64_C(0x00000000001FFFFF),
    STL_UINT64_C(0x00000000003FFFFF),
    STL_UINT64_C(0x00000000007FFFFF),
    STL_UINT64_C(0x0000000000FFFFFF),

    STL_UINT64_C(0x0000000001FFFFFF),
    STL_UINT64_C(0x0000000003FFFFFF),
    STL_UINT64_C(0x0000000007FFFFFF),
    STL_UINT64_C(0x000000000FFFFFFF),

    STL_UINT64_C(0x000000001FFFFFFF),
    STL_UINT64_C(0x000000003FFFFFFF),
    STL_UINT64_C(0x000000007FFFFFFF),
    STL_UINT64_C(0x00000000FFFFFFFF),

    STL_UINT64_C(0x00000001FFFFFFFF),
    STL_UINT64_C(0x00000003FFFFFFFF),
    STL_UINT64_C(0x00000007FFFFFFFF),
    STL_UINT64_C(0x0000000FFFFFFFFF),

    STL_UINT64_C(0x0000001FFFFFFFFF),
    STL_UINT64_C(0x0000003FFFFFFFFF),
    STL_UINT64_C(0x0000007FFFFFFFFF),
    STL_UINT64_C(0x000000FFFFFFFFFF),

    STL_UINT64_C(0x000001FFFFFFFFFF),
    STL_UINT64_C(0x000003FFFFFFFFFF),
    STL_UINT64_C(0x000007FFFFFFFFFF),
    STL_UINT64_C(0x00000FFFFFFFFFFF),

    STL_UINT64_C(0x00001FFFFFFFFFFF),
    STL_UINT64_C(0x00003FFFFFFFFFFF),
    STL_UINT64_C(0x00007FFFFFFFFFFF),
    STL_UINT64_C(0x0000FFFFFFFFFFFF),

    STL_UINT64_C(0x0001FFFFFFFFFFFF),
    STL_UINT64_C(0x0003FFFFFFFFFFFF),
    STL_UINT64_C(0x0007FFFFFFFFFFFF),
    STL_UINT64_C(0x000FFFFFFFFFFFFF),

    STL_UINT64_C(0x001FFFFFFFFFFFFF),
    STL_UINT64_C(0x003FFFFFFFFFFFFF),
    STL_UINT64_C(0x007FFFFFFFFFFFFF),
    STL_UINT64_C(0x00FFFFFFFFFFFFFF),

    STL_UINT64_C(0x01FFFFFFFFFFFFFF),
    STL_UINT64_C(0x03FFFFFFFFFFFFFF),
    STL_UINT64_C(0x07FFFFFFFFFFFFFF),
    STL_UINT64_C(0x0FFFFFFFFFFFFFFF),

    STL_UINT64_C(0x1FFFFFFFFFFFFFFF),
    STL_UINT64_C(0x3FFFFFFFFFFFFFFF),
    STL_UINT64_C(0x7FFFFFFFFFFFFFFF),
    STL_UINT64_C(0xFFFFFFFFFFFFFFFF)
};

stlInt32 stlGetFirstBitPosUInt128( stlUInt128   aValue )
{
    stlInt32    sBitPos = 0;
    stlUInt64   s64;
    stlInt8     sBitPosInByte[256] = {
        0, 8, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    if( aValue.mHighDigit > 0 )
    {
        s64     = aValue.mHighDigit;
    }
    else
    {
        if( aValue.mLowDigit == 0 )
        {
            sBitPos = 0;
            STL_THROW( RAMP_SUCCESS );
        }
        else
        {
            s64     = aValue.mLowDigit;
        }
    }

    if( s64 & STL_UINT64_C(0xFFFFFFFF00000000) )
    {
        if( s64 & STL_UINT64_C(0xFFFF000000000000) )
        {
            if( s64 & STL_UINT64_C(0xFF00000000000000) )
            {
                sBitPos += sBitPosInByte[s64 >> 56];
            }
            else
            {
                sBitPos += 8 + sBitPosInByte[s64 >> 48];
            }
        }
        else
        {
            if( s64 & STL_UINT64_C(0x0000FF0000000000) )
            {
                sBitPos += 16 + sBitPosInByte[s64 >> 40];
            }
            else
            {
                sBitPos += 24 + sBitPosInByte[s64 >> 32];
            }
        }
    }
    else
    {
        if( s64 & STL_UINT64_C(0x00000000FFFF0000) )
        {
            if( s64 & STL_UINT64_C(0x00000000FF000000) )
            {
                sBitPos += 32 + sBitPosInByte[s64 >> 24];
            }
            else
            {
                sBitPos += 40 + sBitPosInByte[s64 >> 16];
            }
        }
        else
        {
            if( s64 & STL_UINT64_C(0x000000000000FF00) )
            {
                sBitPos += 48 + sBitPosInByte[s64 >> 8];
            }
            else
            {
                sBitPos += 56 + sBitPosInByte[s64];
            }
        }
    }

    if( sBitPos > 0 )
    {
        sBitPos += ( aValue.mHighDigit == 0 ? 64 : 0 );
    }
    else
    {
        /* Do Nothing */
    }

    STL_RAMP( RAMP_SUCCESS );

    return sBitPos;
}

/**
 * @brief stlInt64을 stlUInt128 형태로 변경한다.
 * @param aValue stlInt64형 입력 값
 * @param aResult stlUInt128형 결과를 저장할 포인터
 * @return 에러가 발생할 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlSetToUInt128FromInt64( stlInt64      aValue,
                                  stlUInt128  * aResult )
{
    STL_TRY( aValue >= 0 );

    aResult->mHighDigit = 0;
    aResult->mLowDigit  = (stlUInt64)aValue;

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlFloat32을 stlUInt128 형태로 변경한다.
 * @param aValue stlFloat32형 입력 값
 * @param aResult stlUInt128형 결과를 저장할 포인터
 * @param aErrorStack  ErrorStack
 * @return 에러가 발생할 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlSetToUInt128FromReal( stlFloat32       aValue,
                                 stlUInt128     * aResult,
                                 stlErrorStack  * aErrorStack )
{
    stlFloat32  sQuotient;
    stlInt32    sDigitCount;
    stlInt32    sExponent;
    stlUInt128  sResult;
    stlFloat64  sPow;

    /* 음수인지 체크한다. */
    STL_TRY( aValue >= 0 );

    if( aValue == 0.0 )
    {
        aResult->mHighDigit = 0;
        aResult->mLowDigit = 0;

        return STL_TRUE;
    }

    if( aValue <= STL_UINT64_MAX )
    {
        sDigitCount = stlLog10f( aValue );
        if( aValue < 1 )
        {
            STL_TRY( stlGet10Pow( sDigitCount, &sPow, aErrorStack ) );
            if( aValue == sPow )
            {
                sDigitCount++;
            }
        }
        else
        {
            sDigitCount++;
        }

        if( sDigitCount < 0 )
        {
            /* sDigitCount가 음수인 경우 소수점 아래
             * 0의 개수를 의미한다.
             * (ex: 0.123의 sDigitCount는 0)
             *
             * 따라서 0.0999와 같이 소수점 아래 0이
             * 하나라도 있으면 유효자리 반올림이 되더라도
             * 0.1이므로 최종 값은 0 이다. */
            aResult->mHighDigit = 0;
            aResult->mLowDigit = 0;
        }
        else if( sDigitCount == 0 )
        {
            /* 0.xxx인 경우
             * 이는 유효자리에 대하여 정확히 계산한 다음
             * 잘려나가는 소수점 이하의 수들에 대하여
             * 소수점 아래 첫번째 자리에서 반올림한다. */
            STL_TRY( stlGet10Pow( STL_FLT_DIG, &sPow, aErrorStack ) );
            sQuotient = ((aValue * sPow) + 0.5) / sPow;
            aResult->mHighDigit = 0;
            aResult->mLowDigit = (stlUInt64)(sQuotient + 0.5);
        }
        else
        {
            sExponent = STL_FLT_DIG - sDigitCount;
            if( sExponent < 0 )
            {
                /* 유효자리를 넘는 경우 */
                STL_TRY( stlGet10Pow( -sExponent, &sPow, aErrorStack ) == STL_TRUE );
                sQuotient = aValue / sPow;

                aResult->mHighDigit = 0;
                aResult->mLowDigit = (stlUInt64)((stlUInt64)(sQuotient + 0.5) * (stlUInt64)sPow);
            }
            else if( sExponent == 0 )
            {
                /* 유효자리와 같은 경우 */
                aResult->mHighDigit = 0;
                aResult->mLowDigit = (stlUInt64)( aValue + 0.5 );
            }
            else
            {
                /* 유효자리보다 모자란 경우 */
                STL_TRY( stlGet10Pow( sExponent, &sPow, aErrorStack ) == STL_TRUE );
                sQuotient = ((aValue * sPow) + 0.5) / sPow;

                aResult->mHighDigit = 0;
                aResult->mLowDigit = (stlUInt64)(sQuotient + 0.5);
            }
        }
    }
    else
    {
        sDigitCount = stlLog10f( aValue ) + 1;
        STL_TRY( sDigitCount <= STL_UINT128_PRECISION ); /* check overflow */
        
        /* 유효자리수 조정 
         * 여기서 값을 FLOAT32의 유효자리수 이하의 숫자와
         * exponent 값으로 분리하여 놓는다.
         * ex) 123 -> 123 and exponent 0
         *     12345678901234567890 -> 123456 and exponent 14 */
        sExponent = sDigitCount - STL_FLT_DIG;
        STL_TRY( stlGet10Pow( sExponent, &sPow, aErrorStack ) == STL_TRUE );
        sQuotient = aValue / sPow;

        sResult.mHighDigit = 0;
        sResult.mLowDigit = (stlUInt64) ( sQuotient + 0.5 );

        STL_TRY( stlMulUInt128( sResult, gPreDefinedPowUInt128[sExponent], &sResult )
                 == STL_TRUE );

        aResult->mHighDigit = sResult.mHighDigit;
        aResult->mLowDigit  = sResult.mLowDigit;
    }

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlFloat64을 stlUInt128 형태로 변경한다.
 * @param aValue stlFloat64형 입력 값
 * @param aResult stlUInt128형 결과를 저장할 포인터
 * @param aErrorStack   ErrorStack
 * @return 에러가 발생할 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlSetToUInt128FromDouble( stlFloat64      aValue,
                                   stlUInt128    * aResult,
                                   stlErrorStack * aErrorStack )
{
    stlFloat64  sQuotient;
    stlInt32    sDigitCount;
    stlInt32    sExponent;
    stlUInt128  sResult;
    stlFloat64  sPow;

    /* 음수인지 체크한다. */
    STL_TRY( aValue >= 0 );

    if( aValue == 0.0 )
    {
        aResult->mHighDigit = 0;
        aResult->mLowDigit = 0;

        return STL_TRUE;
    }

    if( aValue <= STL_UINT64_MAX )
    {
        sDigitCount = stlLog10( aValue );
        if( aValue < 1 )
        {
            STL_TRY( stlGet10Pow( sDigitCount, &sPow, aErrorStack ) );
            if( aValue == sPow )
            {
                sDigitCount++;
            }
        }
        else
        {
            sDigitCount++;
        }

        if( sDigitCount < 0 )
        {
            /* sDigitCount가 음수인 경우 소수점 아래
             * 0의 개수를 의미한다.
             * (ex: 0.123의 sDigitCount는 0)
             *
             * 따라서 0.0999와 같이 소수점 아래 0이
             * 하나라도 있으면 유효자리 반올림이 되더라도
             * 0.1이므로 최종 값은 0 이다. */
            aResult->mHighDigit = 0;
            aResult->mLowDigit = 0;
        }
        else if( sDigitCount == 0 )
        {
            /* 0.xxx인 경우
             * 이는 유효자리에 대하여 정확히 계산한 다음
             * 잘려나가는 소수점 이하의 수들에 대하여
             * 소수점 아래 첫번째 자리에서 반올림한다. */
            STL_TRY( stlGet10Pow( STL_DBL_DIG, &sPow, aErrorStack ) );
            sQuotient = ((aValue * sPow) + 0.5) / sPow;
            aResult->mHighDigit = 0;
            aResult->mLowDigit = (stlUInt64)(sQuotient + 0.5);
        }
        else
        {
            sExponent = STL_DBL_DIG - sDigitCount;
            if( sExponent < 0 )
            {
                /* 유효자리를 넘는 경우 */
                STL_TRY( stlGet10Pow( -sExponent, &sPow, aErrorStack ) == STL_TRUE );
                sQuotient = aValue / sPow;

                aResult->mHighDigit = 0;
                aResult->mLowDigit = (stlUInt64)((stlUInt64)(sQuotient + 0.5) * (stlUInt64)sPow);
            }
            else if( sExponent == 0 )
            {
                /* 유효자리와 같은 경우 */
                aResult->mHighDigit = 0;
                aResult->mLowDigit = (stlUInt64)( aValue + 0.5 );
            }
            else
            {
                /* 유효자리보다 모자란 경우 */
                STL_TRY( stlGet10Pow( sExponent, &sPow, aErrorStack ) == STL_TRUE );
                sQuotient = ((aValue * sPow) + 0.5) / sPow;

                aResult->mHighDigit = 0;
                aResult->mLowDigit = (stlUInt64)(sQuotient + 0.5);
            }
        }
    }
    else
    {
        sDigitCount = stlLog10( aValue ) + 1;
        STL_TRY( sDigitCount <= STL_UINT128_PRECISION ); /* check overflow */

        /* 유효자리수 조정 
         * 여기서 값을 FLOAT64의 유효자리수 이하의 숫자와
         * exponent 값으로 분리하여 놓는다.
         * ex) 123 -> 123 and exponent 0
         *     12345678901234567890 -> 123456789012345 and exponent 5 */
        sExponent = sDigitCount - STL_DBL_DIG;
        STL_TRY( stlGet10Pow( sExponent, &sPow, aErrorStack ) == STL_TRUE );
        sQuotient = aValue / sPow;

        sResult.mHighDigit = 0;
        sResult.mLowDigit = (stlUInt64) ( sQuotient + 0.5 );

        STL_TRY( stlMulUInt128( sResult, gPreDefinedPowUInt128[sExponent], &sResult )
                 == STL_TRUE );

        aResult->mHighDigit = sResult.mHighDigit;
        aResult->mLowDigit  = sResult.mLowDigit;
    }

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlUInt128을 stlInt64 형태로 변경한다.
 * @param aValue stlUInt128형 입력 값
 * @param aResult stlInt64형 결과를 저장할 포인터
 * @return 에러가 발생할 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlGetToInt64FromUInt128( stlUInt128    aValue,
                                  stlInt64    * aResult )
{
    STL_TRY( (aValue.mHighDigit == 0) && (aValue.mLowDigit <= (stlUInt64)STL_INT64_MAX) );

    *aResult = (stlInt64)(aValue.mLowDigit);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlUInt128을 stlUInt64 형태로 변경한다.
 * @param aValue stlUInt128형 입력 값
 * @param aResult stlUInt64형 결과를 저장할 포인터
 * @return 에러가 발생할 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlGetToUInt64FromUInt128( stlUInt128    aValue,
                                   stlUInt64   * aResult )
{
    STL_TRY( aValue.mHighDigit == 0 );

    *aResult = (stlUInt64)(aValue.mLowDigit);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlUInt128을 stlFlost32 형태로 변경한다.
 * @param aValue stlUInt128형 입력 값
 * @param aResult stlFloat32형 결과를 저장할 포인터
 * @return 에러가 발생할 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
void stlGetToRealFromUInt128( stlUInt128    aValue,
                              stlFloat32  * aResult )
{
    /**
     * @todo float의 범위 체크가 필요하다.
     * The minimum positive (subnormal) value is 2^−149 ≈ 1.4 × 10^−45.
     * The minimum positive normal value is 2^−126 ≈ 1.18 × 10^-38.
     * The maximum representable value is (2−2^-23) × 2127 ≈ 3.4 × 10^38.
     */
    
    *aResult = (stlFloat32)(aValue.mHighDigit) * (STL_FLOAT64_2POW64);
    *aResult = *aResult + (stlFloat32)(aValue.mLowDigit);
}

/**
 * @brief stlUInt128을 stlFloat64 형태로 변경한다.
 * @param aValue stlUInt128형 입력 값
 * @param aResult stlFlost64형 결과를 저장할 포인터
 * @return 에러가 발생할 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
void stlGetToDoubleFromUInt128( stlUInt128    aValue,
                                stlFloat64  * aResult )
{
    *aResult = (stlFloat64)(aValue.mHighDigit) * (STL_FLOAT64_2POW64);
    *aResult = *aResult + (stlFloat64)(aValue.mLowDigit);
}

/**
 * @brief uint128의 두 값에 대해 덧셈 연산을 수행한다.
 * @param aOperand1 덧셈 연산을 수행할 uint128 왼쪽 값
 * @param aOperand2 덧셈 연산을 수행할 uint128 오른쪽 값
 * @param aResult 덧셈 연산 결과를 저장할 포인터
 * @return 결과가 uint128의 최대값을 초과하는 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlAddUInt128( stlUInt128    aOperand1,
                       stlUInt128    aOperand2,
                       stlUInt128  * aResult )
{
    stlUInt128  sSum;

    sSum.mHighDigit = aOperand1.mHighDigit + aOperand2.mHighDigit;
    sSum.mLowDigit = aOperand1.mLowDigit + aOperand2.mLowDigit;

    /* check high digit overflow */
    STL_TRY( (sSum.mHighDigit >= aOperand1.mHighDigit) && 
             (sSum.mHighDigit >= aOperand2.mHighDigit) );

    if( (sSum.mLowDigit < aOperand1.mLowDigit) ||
        (sSum.mLowDigit < aOperand2.mLowDigit) )
    {
        /* low digit overflow */
        STL_TRY( sSum.mHighDigit < STL_UINT64_MAX );
        sSum.mHighDigit++;
    }
    else
    {
        /* Do Nothing */
    }

    STL_COPY_UINT128( aResult, &sSum );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief uint128의 두 값에 대해 뺄셈 연산을 수행한다.
 * @param aOperand1 뺄셈 연산을 수행할 uint128 왼쪽 값
 * @param aOperand2 뺄셈 연산을 수행할 uint128 오른쪽 값
 * @param aResult 뺄셈 연산 결과를 저장할 포인터
 * @return 결과가 uint128의 최대값을 초과하는 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlSubUInt128( stlUInt128    aOperand1,
                       stlUInt128    aOperand2,
                       stlUInt128  * aResult )
{
    stlUInt128  sDiff;

    sDiff.mHighDigit = aOperand1.mHighDigit - aOperand2.mHighDigit;
    sDiff.mLowDigit = aOperand1.mLowDigit - aOperand2.mLowDigit;

    /* check high digit underflow */
    STL_TRY( sDiff.mHighDigit <= aOperand1.mHighDigit );

    if( sDiff.mLowDigit > aOperand1.mLowDigit )
    {
        /* low digit underflow */
        STL_TRY( sDiff.mHighDigit > 0 );
        sDiff.mHighDigit--;
    }
    else
    {
        /* Do Nothing */
    }

    STL_COPY_UINT128( aResult, &sDiff );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief uint128의 두 값에 대해 곱셈 연산을 수행한다.
 * @param aOperand1 곱셈 연산을 수행할 uint128 왼쪽 값
 * @param aOperand2 곱셈 연산을 수행할 uint128 오른쪽 값
 * @param aResult 곱셈 연산 결과를 저장할 포인터
 * @return 결과가 uint128의 최대값을 초과하는 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlMulUInt128( stlUInt128    aOperand1,
                       stlUInt128    aOperand2,
                       stlUInt128  * aResult )
{
    stlUInt128      sResHighLow;
    stlUInt128      sResLowLow;

#define MUL_LEFT_HIGHLOW_AND_RIGHT_LOW( aLeftHigh, aLeftLow, aRightLow ) {                      \
        STL_TRY( stlMulUInt64( (aLeftHigh), (aRightLow), &sResHighLow ) == STL_TRUE );          \
        /* left high digit와 right low digit의 곱셈 결과에서 high digit값이 발생했다는 것은 */  \
        /* 결과값이 uint128을 넘는다는 것을 의미한다. */                                        \
        STL_TRY( sResHighLow.mHighDigit == 0 );                                                 \
        STL_TRY( stlMulUInt64( (aLeftLow), (aRightLow), &sResLowLow ) == STL_TRUE );            \
                                                                                                \
        aResult->mLowDigit = sResLowLow.mLowDigit;                                              \
        aResult->mHighDigit = sResHighLow.mLowDigit + sResLowLow.mHighDigit;                    \
                                                                                                \
        /* 결과의 high digit 값에서 overflow가 발생했는지 체크한다. */                          \
        STL_TRY( (aResult->mHighDigit >= sResHighLow.mLowDigit) &&                              \
                 (aResult->mHighDigit >= sResLowLow.mHighDigit) );                              \
    }

#define MUL_LEFT_HIGH_AND_RIGHT_LOW( aLeftHigh, aRightLow ) {                                   \
        STL_TRY( stlMulUInt64( (aLeftHigh), (aRightLow), &sResHighLow ) == STL_TRUE );          \
        /* left high digit와 right low digit의 곱셈 결과에서 high digit값이 발생했다는 것은 */  \
        /* 결과값이 uint128을 넘는다는 것을 의미한다. */                                        \
        STL_TRY( sResHighLow.mHighDigit == 0 );                                                 \
                                                                                                \
        aResult->mLowDigit  = 0;                                                                \
        aResult->mHighDigit = sResHighLow.mLowDigit;                                            \
    }

#define MUL_LEFT_LOW_AND_RIGHT_LOW( aLeftLow, aRightLow ) {                             \
        STL_TRY( stlMulUInt64( (aLeftLow), (aRightLow), aResult ) == STL_TRUE );        \
    }

    /* check overflow 
     * 두 수 모두 high digit가 0보다 크면 overflow가 발생한다.*/
    STL_TRY( (aOperand1.mHighDigit == 0) || (aOperand2.mHighDigit == 0) );

    /* 둘 중 하나 이상의 숫자가 0이면 0이다. */
    if( STL_IS_UINT128_ZERO( aOperand1 ) || STL_IS_UINT128_ZERO( aOperand2 ) )
    {
        aResult->mHighDigit = 0;
        aResult->mLowDigit  = 0;
        STL_THROW( FINISH_SUCCESS );
    }

    /* 둘 중 하나의 숫자가 1이면 다른 숫자가 결과이다. */
    if( aOperand1.mLowDigit == 1 && aOperand1.mHighDigit == 0 )
    {
        aResult->mHighDigit = aOperand2.mHighDigit;
        aResult->mLowDigit  = aOperand2.mLowDigit;
        STL_THROW( FINISH_SUCCESS );
    }
    else if( aOperand2.mLowDigit == 1 && aOperand2.mHighDigit == 0 )
    {
        aResult->mHighDigit = aOperand1.mHighDigit;
        aResult->mLowDigit  = aOperand1.mLowDigit;
        STL_THROW( FINISH_SUCCESS );
    }

    /* 위 조건 체크 이후 나오는 가능한 연산은 아래와 같다.
     * 1) left high digit > 0 && left low digit > 0 && right high digit == 0 && right low digit > 0
     * 2) left high digit > 0 && left low digit == 0 && right high digit == 0 && right low digit > 0
     * 3) left high digit == 0 && left low digit > 0 && right high digit > 0 && right low digit > 0
     * 4) left high digit == 0 && left low digit > 0 && right high digit > 0 && right low digit == 0
     * 5) left high digit == 0 && left low digit > 0 && right high digit == 0 && right low digit > 0
     *
     * 각 digit들은 64bit이기 때문에 64bit 끼리의 곱셈연산을 해야 하는데
     * 이 비용이 크므로 최대한 연산이 필요없는 부분은 연산하지 않도록 한다.
     */
    if( aOperand1.mHighDigit > 0 )
    {
        if( aOperand1.mLowDigit > 0 )
        {
            /* 1)의 경우로 (left high digit, left low digit) * right low digit 연산을 수행한다. */
            MUL_LEFT_HIGHLOW_AND_RIGHT_LOW( aOperand1.mHighDigit, aOperand1.mLowDigit, aOperand2.mLowDigit );
        }
        else
        {
            /* 2)의 경우로 left high digit * right low digit 연산을 수행한다. */
            MUL_LEFT_HIGH_AND_RIGHT_LOW( aOperand1.mHighDigit, aOperand2.mLowDigit );
        }
    }
    else if( aOperand2.mHighDigit > 0 )
    {
        if( aOperand2.mLowDigit > 0 )
        {
            /* 3)의 경우로 left low digit * (right high digit, right low digit) 연산을 수행한다. */
            MUL_LEFT_HIGHLOW_AND_RIGHT_LOW( aOperand2.mHighDigit, aOperand2.mLowDigit, aOperand1.mLowDigit );
        }
        else
        {
            /* 4)의 경우로 left low digit * right high digit 연산을 수행한다. */
            MUL_LEFT_HIGH_AND_RIGHT_LOW( aOperand2.mHighDigit, aOperand1.mLowDigit );
        }
    }
    else
    {
        /* 5)의 경우로 left low digit * left low digit 연산을 수행한다. */
        MUL_LEFT_LOW_AND_RIGHT_LOW( aOperand1.mLowDigit, aOperand2.mLowDigit );
    }

    STL_RAMP( FINISH_SUCCESS );
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief uint64의 곱셈 연산을 수행한다.
 * @param aOperand1 곱셈 연산을 수행할 왼쪽 수의 값
 * @param aOperand2 곱셈 연산을 수행할 오른쪽 수의 값
 * @param aResult 곱셈 연산 결과를 저장할 포인터
 * @return 에러가 발생한 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlMulUInt64( stlUInt64     aOperand1,
                      stlUInt64     aOperand2,
                      stlUInt128  * aResult )
{
    stlUInt64   sLeftHigh32;
    stlUInt64   sLeftLow32;
    stlUInt64   sRightHigh32;
    stlUInt64   sRightLow32;

    stlUInt64   sResLowLow;
    stlUInt64   sResHighLow;
    stlUInt64   sResLowHigh;
    stlUInt64   sResHighHigh;

    stlUInt64   sSum;

    if( aOperand1 == 0 || aOperand2 == 0 )
    {
        aResult->mHighDigit = 0;
        aResult->mLowDigit = 0;
        STL_THROW( RAMP_SUCCESS );
    }

#define SET_HIGH32_LOW32( aSrc, aHigh, aLow ) { \
        (aHigh) = ((aSrc) >> 32);               \
        (aLow)  = aSrc - ((aHigh) << 32);       \
    }

    SET_HIGH32_LOW32( aOperand1, sLeftHigh32, sLeftLow32 );
    SET_HIGH32_LOW32( aOperand2, sRightHigh32, sRightLow32 );

    sResLowLow      = sLeftLow32 * sRightLow32;     /* mLowDigit 범위 */
    sResHighLow     = sLeftHigh32 * sRightLow32;    /* mHighDight의 하위 32bit와 mLowDigit의 상위 32bit 범위 */
    sResLowHigh     = sLeftLow32 * sRightHigh32;    /* mHighDight의 하위 32bit와 mLowDigit의 상위 32bit 범위 */
    sResHighHigh    = sLeftHigh32 * sRightHigh32;   /* mHighDigit 범위 */

    /* high digit와 low digit에만 저장되어야 할 값들을 먼저 저장한다. */
    aResult->mLowDigit  = sResLowLow;
    aResult->mHighDigit = sResHighHigh;

    /* high digit와 low digit에 걸쳐서 저장되어야 할 값들에 대하여 계산한다. */
    sSum    = sResHighLow + sResLowHigh;
    if( (sSum < sResHighLow) || (sSum < sResLowHigh) )
    {
        /* sSum값에 대해 overflow가 발생한 경우 high digit에 상위 32bit의 1값을 더해준다. */
        aResult->mHighDigit = aResult->mHighDigit + STL_UINT64_C(0x0000000100000000);
    }
    else
    {
        /* Do Nothing */
    }

    /* sSum값의 상위 32bit를 high digit에 더해준다. (64bit * 64bit이므로 절대 128bit를 넘지 않는다.) */
    aResult->mHighDigit = aResult->mHighDigit + (sSum >> 32);

    /* sSum값의 하위 32bit를 low digit에 더해준다.
     * 이때 sSum값의 하위 32bit를 low digit의 상위 32bit에 더해주어야 한다.
     * 또한, overflow가 발생하는지 체크하여 발생하는 경우 high digit에 1을 더해준다. */
    sSum = (sSum << 32);
    if( (STL_UINT64_MAX - sSum) < aResult->mLowDigit )
    {
        /* overflow가 발생하는 경우 */
        aResult->mHighDigit++;
        aResult->mLowDigit = aResult->mLowDigit + sSum;
    }
    else
    {
        aResult->mLowDigit = aResult->mLowDigit + sSum;
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_TRUE;
}

/**
 * @brief 128bit의 dividend를 32bit의 divisor로 나누는 연산을 수행한다.
 * @param aDividend 나눗셈 연산을 수행할 값
 * @param aDivisor 나눗셈 연산을 수행할 divisor 값
 * @param aQuotient 나눗셈 연산 결과의 값을 저장할 포인터
 * @param aRemainder 나눗셈 연산 결과 나머지 값을 저장할 포인터
 * @return 에러가 발생한 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlDivisionUInt128By32( stlUInt128  * aDividend,
                                stlUInt128    aDivisor,
                                stlUInt128  * aQuotient,
                                stlUInt128  * aRemainder )
{
    stlUInt64   sNumber[4];
    stlUInt64   sQuotient[4];
    stlUInt64   sDivisor        = aDivisor.mLowDigit;
    stlUInt64   sRemainder      = 0;
    stlInt32    sCount;

    STL_TRY( STL_IS_NOT_UINT128_ZERO( aDivisor ) );
    sNumber[0] = (stlUInt32)( (aDividend->mHighDigit & STL_UINT64_C(0xFFFFFFFF00000000)) >> 32 );
    sNumber[1] = (stlUInt32)( aDividend->mHighDigit & STL_UINT64_C(0x00000000FFFFFFFF) );
    sNumber[2] = (stlUInt32)( (aDividend->mLowDigit & STL_UINT64_C(0xFFFFFFFF00000000)) >> 32 );
    sNumber[3] = (stlUInt32)( aDividend->mLowDigit & STL_UINT64_C(0x00000000FFFFFFFF) );

    /* skip leading zero */
    for( sCount = 0; sCount < 4; sCount++ )
    {
        if( sNumber[sCount] != 0 )
        {
            break;
        }
        else
        {
            sQuotient[sCount] = 0;
        }
    }

    for( ; sCount < 4; sCount++ )
    {
        sRemainder <<= 32;
        sRemainder |= sNumber[sCount];

        sQuotient[sCount] = sRemainder / sDivisor;
        sRemainder = sRemainder % sDivisor;
    }

    aQuotient->mHighDigit   = ( (sQuotient[0] << 32) | sQuotient[1] );
    aQuotient->mLowDigit    = ( (sQuotient[2] << 32) | sQuotient[3] );

    aRemainder->mHighDigit  = 0;
    aRemainder->mLowDigit   = sRemainder;

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief uint128의 두 값에 대해 나눗셈 연산을 수행하고, 몫과 나머지를 반환한다.
 * @param aOperand1 나눗셈 연산을 수행할 uint128 왼쪽 값
 * @param aOperand2 나눗셈 연산을 수행할 uint128 오른쪽 값
 * @param aQuotient 나눗셈 연산 결과의 몫을 저장할 포인터
 * @param aRemainder 나눗셈 연산 결과의 나머지 값을 저장할 포인터
 * @return 에러가 발생한 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlDivRemUInt128( stlUInt128    aOperand1,
                          stlUInt128    aOperand2,
                          stlUInt128  * aQuotient,
                          stlUInt128  * aRemainder )
{
    stlInt32    sCount;
    stlInt32    sCmp;
    stlUInt128  sQuotient;
    stlUInt128  sRemainder;

    STL_TRY( STL_IS_NOT_UINT128_ZERO( aOperand2 ) );

    if( aOperand2.mHighDigit == 0 )
    {
        if( aOperand2.mLowDigit == 1 )
        {
            /* divisor == 1 */
            aQuotient->mHighDigit   = aOperand1.mHighDigit;
            aQuotient->mLowDigit    = aOperand1.mLowDigit;
            aRemainder->mHighDigit  = 0;
            aRemainder->mLowDigit   = 0;

            STL_THROW( RAMP_SUCCESS );
        }
        else if( aOperand2.mLowDigit <= STL_UINT32_MAX )
        {
            STL_TRY( stlDivisionUInt128By32( &aOperand1, aOperand2,
                                             aQuotient, aRemainder )
                     == STL_TRUE );
            STL_THROW( RAMP_SUCCESS );
        }
    }

    /* simple check */
    sCmp = stlCmpUInt128( aOperand1, aOperand2 );
    if( sCmp < 0 )
    {
        aQuotient->mHighDigit   = 0;
        aQuotient->mLowDigit    = 0;
        aRemainder->mHighDigit  = aOperand1.mHighDigit;
        aRemainder->mLowDigit   = aOperand1.mLowDigit;

        STL_THROW( RAMP_SUCCESS );
    }
    else if( sCmp == 0 )
    {
        aQuotient->mHighDigit   = 0;
        aQuotient->mLowDigit    = 1;
        aRemainder->mHighDigit  = 0;
        aRemainder->mLowDigit   = 0;

        STL_THROW( RAMP_SUCCESS );
    }

    if( aOperand1.mHighDigit > 0 )
    {  
        if( aOperand2.mHighDigit > 0 )
        {  
            /* (X1, X2)와 (Y1, Y2)의 연산 */
            if( aOperand1.mHighDigit > aOperand2.mHighDigit )
            {
                sQuotient.mHighDigit    = aOperand1.mLowDigit;
                sQuotient.mLowDigit     = 0;
                sRemainder.mHighDigit   = 0;
                sRemainder.mLowDigit    = aOperand1.mHighDigit;
                for( sCount = 0; sCount < 64; sCount++ )
                {
                    stlShiftLeftUInt128( sRemainder );
                    if( sQuotient.mHighDigit & STL_UINT64_HIGH_BIT )
                    {
                        sRemainder.mLowDigit |= (stlUInt64)1;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                    stlShiftLeftUInt128( sQuotient );

                    if( stlCmpUInt128( sRemainder, aOperand2 ) >= 0 )
                    {
                        STL_TRY( stlSubUInt128( sRemainder, aOperand2, &sRemainder ) == STL_TRUE );
                        sQuotient.mLowDigit |= (stlUInt64)1;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }

                STL_COPY_UINT128( aQuotient, &sQuotient );
                STL_COPY_UINT128( aRemainder, &sRemainder );
            }
            else if( aOperand1.mHighDigit == aOperand2.mHighDigit )
            {
                aQuotient->mHighDigit   = 0;
                aQuotient->mLowDigit    = 1;
                aRemainder->mHighDigit  = 0;
                aRemainder->mLowDigit   = aOperand1.mLowDigit - aOperand2.mLowDigit;
            }
            else
            {
                /* Do Nothing (이 경우는 위 compare 과정에서 체크되었음) */
            }
        }
        else
        {
            /* (X1, X2)와 (0, Y2)의 연산 */
            sQuotient.mHighDigit    = aOperand1.mLowDigit;
            sQuotient.mLowDigit     = 0;
            sRemainder.mHighDigit   = 0;
            sRemainder.mLowDigit    = aOperand1.mHighDigit;

            sQuotient.mLowDigit = sRemainder.mLowDigit / aOperand2.mLowDigit;
            sRemainder.mLowDigit = sRemainder.mLowDigit % aOperand2.mLowDigit;

            for( sCount = 0; sCount < 64; sCount++ )
            {
                stlShiftLeftUInt128( sRemainder );
                if( sQuotient.mHighDigit & STL_UINT64_HIGH_BIT )
                {
                    sRemainder.mLowDigit |= (stlUInt64)1;
                }
                else
                {
                    /* Do Nothing */
                }
                stlShiftLeftUInt128( sQuotient );

                if( stlCmpUInt128( sRemainder, aOperand2 ) >= 0 )
                {
                    STL_TRY( stlSubUInt128( sRemainder, aOperand2, &sRemainder ) == STL_TRUE );
                    sQuotient.mLowDigit |= (stlUInt64)1;
                }
                else
                {
                    /* Do Nothing */
                }
            }
            STL_COPY_UINT128( aQuotient, &sQuotient );
            STL_COPY_UINT128( aRemainder, &sRemainder );
        }
    }
    else
    {
        /* (0, X2)와 (0, Y2)의 연산 */
        /* ( (0, X)와 (Y1, Y2)의 연산인 경우는 위 compare 과정에서 체크되었음 ) */
        aQuotient->mHighDigit   = 0;
        aQuotient->mLowDigit    = aOperand1.mLowDigit / aOperand2.mLowDigit;
        aRemainder->mHighDigit  = 0;
        aRemainder->mLowDigit   = aOperand1.mLowDigit % aOperand2.mLowDigit;
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief uint128의 두 값의 곱을 256의 high 128bit 부분과 low 128bit 부분으로 나누어 반환한다. 
 * @param aOperand1 곱셈 연산을 수행할 uint128 왼쪽 첫번째 값
 * @param aOperand2 곱셈 연산을 수행할 uint128 왼쪽 두번째 값
 * @param aResultHigh 곱셈 연산 결과의 high 128bit 값을 저장할 포인터
 * @param aResultLow 곱셈 연산 결과의 low 128bit 값을 저장할 포인터
 * @return 없음
 */
void stlMultiplicationOfUInt128( stlUInt128       aOperand1,
                                 stlUInt128       aOperand2,
                                 stlUInt128     * aResultHigh,
                                 stlUInt128     * aResultLow )
{
    stlUInt128  s256HighDigits;
    stlUInt128  s256LowDigits;
    stlUInt128  sTemp;

    (void)stlMulUInt64( aOperand1.mHighDigit, aOperand2.mHighDigit, &s256HighDigits );
    (void)stlMulUInt64( aOperand1.mLowDigit, aOperand2.mLowDigit, &s256LowDigits );
    (void)stlMulUInt64( aOperand1.mHighDigit, aOperand2.mLowDigit, &sTemp );
    if( STL_UINT64_MAX - s256HighDigits.mLowDigit < sTemp.mHighDigit )
    {
        s256HighDigits.mHighDigit++;
        s256HighDigits.mLowDigit += sTemp.mHighDigit;
    }
    else
    {
        s256HighDigits.mLowDigit += sTemp.mHighDigit;
    }
    if( STL_UINT64_MAX - s256LowDigits.mHighDigit < sTemp.mLowDigit )
    {
        stlIncUInt128( &s256HighDigits );
        s256LowDigits.mHighDigit += sTemp.mLowDigit;
    }
    else
    {
        s256LowDigits.mHighDigit += sTemp.mLowDigit;
    }
    (void)stlMulUInt64( aOperand1.mLowDigit, aOperand2.mHighDigit, &sTemp );
    if( STL_UINT64_MAX - s256HighDigits.mLowDigit < sTemp.mHighDigit )
    {
        s256HighDigits.mHighDigit++;
        s256HighDigits.mLowDigit += sTemp.mHighDigit;
    }
    else
    {
        s256HighDigits.mLowDigit += sTemp.mHighDigit;
    }
    if( STL_UINT64_MAX - s256LowDigits.mHighDigit < sTemp.mLowDigit )
    {
        stlIncUInt128( &s256HighDigits );
        s256LowDigits.mHighDigit += sTemp.mLowDigit;
    }
    else
    {
        s256LowDigits.mHighDigit += sTemp.mLowDigit;
    }

    STL_COPY_UINT128( aResultHigh, &s256HighDigits );
    STL_COPY_UINT128( aResultLow, &s256LowDigits );
}

/**
 * @brief 256bit의 dividend를 32bit의 divisor로 나누는 연산을 수행한다.
 * @param a256HighDigits 나눗셈 연산을 수행할 high 128bit 값
 * @param a256LowDigits 나눗셈 연산을 수행할 low 128bit 값
 * @param aDivisor 나눗셈 연산을 수행할 divisor 값
 * @param aQuotientHigh 나눗셈 연산 결과의 high 128bit 값을 저장할 포인터
 * @param aQuotientLow 나눗셈 연산 결과의 low 128bit 값을 저장할 포인터
 * @param aRemainder 나눗셈 연산 결과 나머지 값을 저장할 포인터
 * @return 에러가 발생한 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlDivisionUInt256By32( stlUInt128  * a256HighDigits,
                                stlUInt128  * a256LowDigits,
                                stlUInt128    aDivisor,
                                stlUInt128  * aQuotientHigh,
                                stlUInt128  * aQuotientLow,
                                stlUInt128  * aRemainder )
{
    stlUInt64   sNumber[8];
    stlUInt64   sQuotient[8];
    stlUInt64   sDivisor        = aDivisor.mLowDigit;
    stlUInt64   sRemainder      = 0;
    stlInt32    sCount;

    STL_TRY( STL_IS_NOT_UINT128_ZERO( aDivisor ) );
    sNumber[0] = (stlUInt32)( (a256HighDigits->mHighDigit & STL_UINT64_C(0xFFFFFFFF00000000)) >> 32 );
    sNumber[1] = (stlUInt32)( a256HighDigits->mHighDigit & STL_UINT64_C(0x00000000FFFFFFFF) );
    sNumber[2] = (stlUInt32)( (a256HighDigits->mLowDigit & STL_UINT64_C(0xFFFFFFFF00000000)) >> 32 );
    sNumber[3] = (stlUInt32)( a256HighDigits->mLowDigit & STL_UINT64_C(0x00000000FFFFFFFF) );
    sNumber[4] = (stlUInt32)( (a256LowDigits->mHighDigit & STL_UINT64_C(0xFFFFFFFF00000000)) >> 32 );
    sNumber[5] = (stlUInt32)( a256LowDigits->mHighDigit & STL_UINT64_C(0x00000000FFFFFFFF) );
    sNumber[6] = (stlUInt32)( (a256LowDigits->mLowDigit & STL_UINT64_C(0xFFFFFFFF00000000)) >> 32 );
    sNumber[7] = (stlUInt32)( a256LowDigits->mLowDigit & STL_UINT64_C(0x00000000FFFFFFFF) );

    /* skip leading zero */
    for( sCount = 0; sCount < 8; sCount++ )
    {
        if( sNumber[sCount] != 0 )
        {
            break;
        }
        else
        {
            sQuotient[sCount] = 0;
        }
    }

    for( ; sCount < 8; sCount++ )
    {
        sRemainder <<= 32;
        sRemainder |= sNumber[sCount];

        sQuotient[sCount] = sRemainder / sDivisor;
        sRemainder = sRemainder % sDivisor;
    }

    aQuotientHigh->mHighDigit   = ( (sQuotient[0] << 32) | sQuotient[1] );
    aQuotientHigh->mLowDigit    = ( (sQuotient[2] << 32) | sQuotient[3] );
    aQuotientLow->mHighDigit    = ( (sQuotient[4] << 32) | sQuotient[5] );
    aQuotientLow->mLowDigit     = ( (sQuotient[6] << 32) | sQuotient[7] );

    aRemainder->mHighDigit      = 0;
    aRemainder->mLowDigit       = sRemainder;

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief 256bit의 dividend를 128bit의 divisor로 나누는 연산을 수행한다.
 * @param a256HighDigits 나눗셈 연산을 수행할 high 128bit 값
 * @param a256LowDigits 나눗셈 연산을 수행할 low 128bit 값
 * @param aDivisor 나눗셈 연산을 수행할 divisor 값
 * @param aQuotientHigh 나눗셈 연산 결과의 high 128bit 값을 저장할 포인터
 * @param aQuotientLow 나눗셈 연산 결과의 low 128bit 값을 저장할 포인터
 * @param aRemainder 나눗셈 연산 결과 나머지 값을 저장할 포인터
 * @return 에러가 발생한 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlDivisionUInt256By128( stlUInt128  * a256HighDigits,
                                 stlUInt128  * a256LowDigits,
                                 stlUInt128    aDivisor,
                                 stlUInt128  * aQuotientHigh,
                                 stlUInt128  * aQuotientLow,
                                 stlUInt128  * aRemainder )
{
    stlUInt128  s256HighDigits = { 0, 0 };
    stlUInt128  s256LowDigits  = { 0, 0 };
    stlUInt128  sQuotientHigh;
    stlUInt128  sQuotientLow;
    stlUInt128  sRemainder;
    stlInt32    sRemainedBitLength;
    stlInt32    sDivisorBitLength;
    stlInt32    sTempLength;

    STL_TRY( STL_IS_NOT_UINT128_ZERO( aDivisor ) );

    if( STL_IS_UINT128_ZERO( *a256HighDigits ) )
    {
        aQuotientHigh->mHighDigit   = 0;
        aQuotientHigh->mLowDigit    = 0;
        STL_TRY( stlDivRemUInt128( *a256LowDigits, aDivisor, aQuotientLow, aRemainder )
                 == STL_TRUE );
        STL_THROW( RAMP_SUCCESS );
    }

    if( aDivisor.mHighDigit == 0 )
    {
        if( aDivisor.mLowDigit == 1 )
        {
            /* divisor == 1 */
            aQuotientHigh->mHighDigit   = a256HighDigits->mHighDigit;
            aQuotientHigh->mLowDigit    = a256HighDigits->mLowDigit;
            aQuotientLow->mHighDigit    = a256LowDigits->mHighDigit;
            aQuotientLow->mLowDigit     = a256LowDigits->mLowDigit;
            aRemainder->mHighDigit  = 0;
            aRemainder->mLowDigit   = 0;

            STL_THROW( RAMP_SUCCESS );
        }
        else if( aDivisor.mLowDigit <= STL_UINT32_MAX )
        {
            STL_TRY( stlDivisionUInt256By32( a256HighDigits, a256LowDigits, aDivisor,
                                             aQuotientHigh, aQuotientLow, aRemainder )
                     == STL_TRUE );
            STL_THROW( RAMP_SUCCESS );
        }
    }

    STL_COPY_UINT128( &s256HighDigits, a256HighDigits );
    STL_COPY_UINT128( &s256LowDigits, a256LowDigits );

    /* check bit length */
    if( STL_IS_UINT128_ZERO( s256HighDigits ) )
    {
        sRemainedBitLength = 128 - stlGetFirstBitPosUInt128( s256LowDigits ) + 1;
    }
    else
    {
        sRemainedBitLength = 256 - stlGetFirstBitPosUInt128( s256HighDigits ) + 1;
    }

    sDivisorBitLength = 128 - stlGetFirstBitPosUInt128( aDivisor ) + 1;

    /* adjust dividend bits */
    sTempLength = 256 - sRemainedBitLength;
    stlShiftLeftUInt256WithCount( s256HighDigits, s256LowDigits, sTempLength );

    if( sDivisorBitLength > 64 )
    {
        sRemainedBitLength     -= ( sDivisorBitLength - 1 );
        sRemainder.mHighDigit   = s256HighDigits.mHighDigit;
        sRemainder.mLowDigit    = s256HighDigits.mLowDigit;
        
        sTempLength = 128 - (sDivisorBitLength - 1);
        stlShiftRightUInt128WithCount( sRemainder, sTempLength );
        if( sRemainedBitLength > 128 )
        {
            sTempLength = sDivisorBitLength - 1;
            stlShiftLeftUInt256WithCount( s256HighDigits, s256LowDigits, sTempLength );
        }
        else
        {
            sTempLength = sDivisorBitLength - 1;
            stlShiftRightUInt128WithCount( s256HighDigits, sTempLength );
        }

        sQuotientHigh.mHighDigit = 0;
        sQuotientHigh.mLowDigit  = 0;
        sQuotientLow.mHighDigit  = 0;
        sQuotientLow.mLowDigit   = 0;
    }
    else
    {
        sRemainedBitLength         -= 64;
        sQuotientHigh.mHighDigit    = 0;
        sQuotientHigh.mLowDigit     = 0;
        sQuotientLow.mHighDigit     = 0;
        sQuotientLow.mLowDigit      = s256HighDigits.mHighDigit / aDivisor.mLowDigit;
        sRemainder.mHighDigit       = 0;
        sRemainder.mLowDigit        = s256HighDigits.mHighDigit % aDivisor.mLowDigit;
        s256HighDigits.mHighDigit   = s256HighDigits.mLowDigit;
        s256HighDigits.mLowDigit    = s256LowDigits.mHighDigit;
        s256LowDigits.mHighDigit    = s256LowDigits.mLowDigit;
        s256LowDigits.mLowDigit     = 0;
    }

    /* s256HighDigits와 s256LowDigits에 모두 숫자가 있는 경우 */
    while( sRemainedBitLength > 128 )
    {
        stlShiftLeftUInt128( sRemainder );
        if( s256HighDigits.mHighDigit & STL_UINT64_HIGH_BIT )
        {
            sRemainder.mLowDigit |= STL_UINT64_LOW_BIT;
        }
        else
        {
            /* Do Nothing */
        }

        stlShiftLeftUInt256( s256HighDigits, s256LowDigits );
        stlShiftLeftUInt128( sQuotientLow );

        if( stlCmpUInt128( sRemainder, aDivisor ) >= 0 )
        {
            (void)stlSubUInt128( sRemainder, aDivisor, &sRemainder );
            sQuotientLow.mLowDigit |= (stlUInt64)STL_UINT64_LOW_BIT;
        }
        else
        {
            /* Do Nothing */
        }

        sRemainedBitLength--;
    }

    /* s256HighDigits에만 숫자가 있는 경우 */
    while( sRemainedBitLength > 0 )
    {
        stlShiftLeftUInt128( sRemainder );
        if( s256HighDigits.mHighDigit & STL_UINT64_HIGH_BIT )
        {
            sRemainder.mLowDigit |= STL_UINT64_LOW_BIT;
        }
        else
        {
            /* Do Nothing */
        }

        stlShiftLeftUInt128( s256HighDigits );
        stlShiftLeftUInt256( sQuotientHigh, sQuotientLow );

        if( stlCmpUInt128( sRemainder, aDivisor ) >= 0 )
        {
            (void)stlSubUInt128( sRemainder, aDivisor, &sRemainder );
            sQuotientLow.mLowDigit |= (stlUInt64)STL_UINT64_LOW_BIT;
        }
        else
        {
            /* Do Nothing */
        }

        sRemainedBitLength--;
    }

    STL_COPY_UINT128( aQuotientHigh, &sQuotientHigh );
    STL_COPY_UINT128( aQuotientLow, &sQuotientLow );
    STL_COPY_UINT128( aRemainder, &sRemainder );

    STL_RAMP( RAMP_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief uint128의 두 값의 곱에 대하여 uint128로 나눗셈 연산을 수행하고, 38자리의 몫을 반환한다. <br>
 * 인자로 받는 dividend와 divisor의 나눗셈 연산으로는 38~39자리의 몫이 나올 수 있도록 주어야 한다.
 * @param aDividend1 나눗셈 연산을 수행할 uint128 왼쪽 첫번째 값
 * @param aDividend2 나눗셈 연산을 수행할 uint128 왼쪽 두번째 값
 * @param aDivisor 나눗셈 연산을 수행할 uint128 오른쪽 값
 * @param aQuotient 나눗셈 연산 결과의 몫을 저장할 포인터
 * @param aExponent 나눗셈 연산 결과의 몫이 원하는 precision을 넘었을 경우 이를 맞추기 위해 조정한 exponent의 값
 * @return 에러가 발생한 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlDivUInt256( stlUInt128    aDividend1,
                       stlUInt128    aDividend2,
                       stlUInt128    aDivisor,
                       stlUInt128  * aQuotient,
                       stlInt32    * aExponent )
{
    stlUInt128  s256HighDigits;
    stlUInt128  s256LowDigits;
    stlUInt128  sQuotientHigh;
    stlUInt128  sQuotientLow;
    stlUInt128  sRemainder;
    stlInt64    sPrecision;

    /* make 256 digits */
    stlMultiplicationOfUInt128( aDividend1, aDividend2, &s256HighDigits, &s256LowDigits );

    STL_TRY( stlDivisionUInt256By128( &s256HighDigits, &s256LowDigits, aDivisor,
                                      &sQuotientHigh, &sQuotientLow, &sRemainder )
             == STL_TRUE );

    if( STL_IS_NOT_UINT128_ZERO( sQuotientHigh ) )
    {
        /* 넘겨주는 값이 최대 39자리를 넘지 않게 넘겨주므로
         * 이 조건을 만족하면 39자리인 경우이다. */
        if( STL_UINT64_MAX - sQuotientLow.mLowDigit < 5 )
        {
            sQuotientLow.mLowDigit += 5;
            if( STL_UINT64_MAX - sQuotientLow.mHighDigit < 1 )
            {
                sQuotientLow.mHighDigit++;
                sQuotientHigh.mLowDigit++;
            }
            else
            {
                sQuotientLow.mHighDigit++;
            }
        }
        else
        {
            sQuotientLow.mLowDigit += 5;
        }
        STL_TRY( stlDivisionUInt256By128( &sQuotientHigh, &sQuotientLow, gPreDefinedPowUInt128[1],
                                          &sQuotientHigh, aQuotient, &sRemainder )
                 == STL_TRUE );
        STL_TRY( STL_IS_UINT128_ZERO( sQuotientHigh ) );
        *aExponent = 1;
    }
    else
    {
        stlGetPrecisionUInt128( sQuotientLow, sPrecision );
        if( sPrecision > STL_UINT128_PRECISION )
        {
            *aExponent = sPrecision - STL_UINT128_PRECISION;
            stlDivRemUInt128( sQuotientLow, gPreDefinedPowUInt128[sPrecision - STL_UINT128_PRECISION], aQuotient, &sRemainder );
            if( sRemainder.mLowDigit > 4 )
            {
                stlIncUInt128( &sQuotientLow );
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            STL_COPY_UINT128( aQuotient, &sQuotientLow );
            *aExponent = 0;

            if( STL_IS_NOT_UINT128_ZERO( sRemainder ) )
            {
                stlMultiplicationOfUInt128( sRemainder, gPreDefinedPowUInt128[1],
                                            &s256HighDigits, &s256LowDigits );
                STL_TRY( stlDivisionUInt256By128( &s256HighDigits, &s256LowDigits, aDivisor,
                                                  &sQuotientHigh, &sQuotientLow, &sRemainder )
                         == STL_TRUE );
                if( sQuotientLow.mLowDigit > 4 )
                {
                    stlIncUInt128( aQuotient );
                }
                else
                {
                    /* Do Nothing */
                }
            }
            else
            {
                /* Do Nothing */
            }
        }
    }

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief uint128의 두 값에 대해 나눗셈 연산을 수행한다.
 * @param aOperand1 나눗셈 연산을 수행할 uint128 왼쪽 값
 * @param aOperand2 나눗셈 연산을 수행할 uint128 오른쪽 값
 * @param aResult 나눗셈 연산 결과를 저장할 포인터
 * @return 에러가 발생한 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlDivUInt128( stlUInt128    aOperand1,
                       stlUInt128    aOperand2,
                       stlUInt128  * aResult )
{
    stlUInt128 sRemainder;

    return stlDivRemUInt128( aOperand1, aOperand2, aResult, &sRemainder );
}

/**
 * @brief uint128의 두 값에 대해 나머지 연산을 수행한다.
 * @param aOperand1 나머지 연산을 수행할 uint128 왼쪽 값
 * @param aOperand2 나머지 연산을 수행할 uint128 오른쪽 값
 * @param aResult 나머지 연산 결과를 저장할 포인터
 * @return 에러가 발생한 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlRemUInt128( stlUInt128    aOperand1,
                       stlUInt128    aOperand2,
                       stlUInt128  * aResult )
{
    stlUInt128 sQuotient;

    return stlDivRemUInt128( aOperand1, aOperand2, &sQuotient, aResult );
}

/**
 * @brief uint128 값에 대해 increment by one 연산을 수행한다.
 * @param aOperand increment by one 연산을 수행할 uint128 포인터
 * @return aOperand 값이 uint128의 최대값인 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlIncUInt128( stlUInt128  * aOperand )
{
    /* check overflow */
    STL_TRY( !STL_IS_UINT128_MAX( *aOperand ) );

    if( aOperand->mLowDigit == STL_UINT64_MAX )
    {
        aOperand->mLowDigit++;
        aOperand->mHighDigit++;
    }
    else
    {
        aOperand->mLowDigit++;
    }

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief uint128 값에 대해 decrement by one 연산을 수행한다.
 * @param aOperand decrement by one 연산을 수행할 uint128 포인터
 * @return aOperand 값이 uint128의 최소값인 경우 STL_FALSE,
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlDecUInt128( stlUInt128  * aOperand )
{
    /* check underflow */
    STL_TRY( !STL_IS_UINT128_MIN( *aOperand ) );

    if( aOperand->mLowDigit == 0 )
    {
        aOperand->mLowDigit--;
        aOperand->mHighDigit--;
    }
    else
    {
        aOperand->mLowDigit--;
    }

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief uint128의 두 값에 대해 Equal 연산을 수행한다.
 * @param aOperand1 Equal 연산을 수행할 왼쪽 값
 * @param aOperand2 Equal 연산을 수행할 오른쪽 값
 * @return 동일한 값일 경우 STL_TRUE, 다른 값일 경우 STL_FALSE
 */
stlBool stlIsEqUInt128( stlUInt128    aOperand1,
                        stlUInt128    aOperand2 )
{
    return ( (aOperand1.mLowDigit == aOperand2.mLowDigit) &&
             (aOperand1.mHighDigit == aOperand2.mHighDigit) );
}

/**
 * @brief uint128의 두 값에 대해 비교연산을 수행한다.
 * @param aOperand1 비교연산을 수행할 왼쪽 값
 * @param aOperand2 비교연산을 수행할 오른쪽 값
 * @return aOperand1 < aOperand2일 경우 -1,
 *         aOperand1 > aOperand2일 경우 1,
 *         aOperand1 == aOperand2일 경우 0
 */
stlInt32 stlCmpUInt128( stlUInt128    aOperand1,
                        stlUInt128    aOperand2 )
{
    if( aOperand1.mHighDigit > aOperand2.mHighDigit )
    {
        return 1;
    }
    else if( aOperand1.mHighDigit < aOperand2.mHighDigit )
    {
        return -1;
    }
    else
    {
        if( aOperand1.mLowDigit < aOperand2.mLowDigit )
        {
            return -1;
        }
        else
        {
            return aOperand1.mLowDigit > aOperand2.mLowDigit;
        }
    }

    return 0;
}

/**
 * @brief uint128의 두 값에 대해 최대 공약수를 구한다.
 * @param aOperand1 최대 공약수를 구할 왼쪽 값
 * @param aOperand2 최대 공약수를 구할 오른쪽 값
 * @param aResult aOperand1과 aOperand2의 최대 공약수 결과를 저장할 포인터
 * @return 에러 발생시 STL_FALSE
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlGcfUInt128( stlUInt128    aOperand1,
                       stlUInt128    aOperand2,
                       stlUInt128  * aResult )
{
    stlUInt128  sRem;

    sRem.mHighDigit = 0;
    sRem.mLowDigit  = 0;

    STL_TRY( (STL_IS_NOT_UINT128_ZERO( aOperand1 )) && 
             (STL_IS_NOT_UINT128_ZERO( aOperand2 )) );

    while( STL_IS_NOT_UINT128_ZERO( aOperand2 ) )
    {
        STL_TRY( stlRemUInt128( aOperand1, aOperand2, &sRem ) == STL_TRUE );
        STL_COPY_UINT128( &aOperand1, &aOperand2 );
        STL_COPY_UINT128( &aOperand2, &sRem );
    }

    STL_COPY_UINT128( aResult, &aOperand1 );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief uint128의 두 값에 대해 최소 공배수를 구한다.
 * @param aOperand1 최소 공배수를 구할 왼쪽 값
 * @param aOperand2 최소 공배수를 구할 오른쪽 값
 * @param aResult aOperand1과 aOperand2의 최소 공배수 결과를 저장할 포인터
 * @return 에러 발생시 STL_FALSE
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlLcmUInt128( stlUInt128    aOperand1,
                       stlUInt128    aOperand2,
                       stlUInt128  * aResult )
{
    stlUInt128  sGcf;
    stlUInt128  sQuotient;

    STL_TRY( stlGcfUInt128( aOperand1, aOperand2, &sGcf ) == STL_TRUE );

    STL_TRY( stlDivUInt128( aOperand1, sGcf, &sQuotient ) == STL_TRUE );

    STL_TRY( stlMulUInt128( aOperand2, sQuotient, aResult ) == STL_TRUE );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief uint128의 값에 대해 scale에 대한 round 연산을 수행한다.
 * @param aValue round 연산을 수행할 값
 * @param aScale scale 값
 * @param aResult aValue의 round 연산 결과를 저장할 포인터
 * @return 에러 발생시 STL_FALSE
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlRoundUInt128( stlUInt128     aValue,
                         stlInt32       aScale,
                         stlUInt128   * aResult )
{
    stlUInt128  sQuotient;
    stlUInt128  sDividend;
    stlUInt128  sRem;
    stlUInt128  sTempValue;
    stlUInt64   sTemp64;

    if( STL_IS_UINT128_ZERO( aValue ) )
    {
        aResult->mHighDigit = 0;
        aResult->mLowDigit  = 0;
    }
    else if( aScale >= 0 )
    {
        STL_COPY_UINT128( aResult, &aValue );
    }
    else if( (-aScale) >= STL_UINT128_PRECISION )
    {
        aResult->mHighDigit = 0;
        aResult->mLowDigit  = 0;
    }
    else
    {
        if( aValue.mHighDigit > 0 )
        {
            /* aValue의 값이 STL_UINT64_MAX보다 큰 경우 */

            sDividend = gPreDefinedPowUInt128[-aScale];
            STL_TRY( stlDivRemUInt128( aValue, sDividend, &sQuotient, &sRem ) == STL_TRUE );
            STL_TRY( stlSubUInt128( aValue, sRem, aResult ) == STL_TRUE );

            STL_COPY_UINT128( &sTempValue, &sDividend );
            stlShiftRightUInt128( sTempValue );

            if( stlCmpUInt128( sRem, sTempValue ) >= 0 )
            {
                STL_TRY( stlAddUInt128( *aResult, sDividend, aResult ) == STL_TRUE );
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            /* aValue의 값이 STL_UINT64_MAX와 같거나 작은 경우 */

            /* UInt64의 MAX값은 1.xx * 10^20이다.
             * 따라서 position이 20(uint64의 precision)보다 크면 0이다. */
            if( (-aScale) > STL_UINT64_PRECISION )
            {
                aResult->mHighDigit = 0;
                aResult->mLowDigit  = 0;
            }
            else
            {
                sTemp64 = aValue.mLowDigit % gPreDefinedPowUInt128[-aScale].mLowDigit;
                aResult->mHighDigit = 0;
                aResult->mLowDigit  = aValue.mLowDigit - sTemp64;

                if( sTemp64 >= (gPreDefinedPowUInt128[-aScale].mLowDigit >> 1) )
                {
                    STL_TRY( stlAddUInt128( *aResult, gPreDefinedPowUInt128[-aScale], aResult ) == STL_TRUE );
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }
    }

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief uint128의 값에 대해 scale에 대한 ceil 연산을 수행한다.
 * @param aValue ceil 연산을 수행할 값
 * @param aScale scale 값
 * @param aResult aValue의 ceil 연산 결과를 저장할 포인터
 * @return 에러 발생시 STL_FALSE
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlCeilUInt128( stlUInt128      aValue,
                        stlInt32        aScale,
                        stlUInt128    * aResult )
{
    stlUInt128  sQuotient;
    stlUInt128  sDividend;
    stlUInt128  sMul;
    stlUInt128  sRem;
    stlUInt128  sTempValue;
    stlUInt128  sTempValue2;
    stlUInt64   sTemp64;
    stlUInt32   sExponent;

    if( STL_IS_UINT128_ZERO( aValue ) )
    {
        aResult->mHighDigit = 0;
        aResult->mLowDigit  = 0;
    }
    else if( aScale >= 0 )
    {
        STL_COPY_UINT128( aResult, &aValue );
    }
    else if( (-aScale) > (STL_UINT128_PRECISION + 1) )
    {
        aResult->mHighDigit = 0;
        aResult->mLowDigit  = 0;
    }
    else
    {
        if( aValue.mHighDigit > 0 )
        {
            /* aValue의 값이 STL_UINT64_MAX보다 큰 경우 */

            if( (-aScale) > gPreDefinedPowMax )
            {
                sDividend.mHighDigit    = 0;
                sDividend.mLowDigit     = gPreDefinedPow[gPreDefinedPowMax];
                sExponent               = (-aScale) - gPreDefinedPowMax;
                while( sExponent > gPreDefinedPowMax )
                {
                    sTempValue.mHighDigit   = 0;
                    sTempValue.mLowDigit    = gPreDefinedPow[gPreDefinedPowMax];
                    STL_TRY( stlMulUInt128( sDividend, sTempValue, &sDividend ) == STL_TRUE );
                    sExponent               = sExponent - gPreDefinedPowMax;
                }
                if( sExponent > 0 )
                {
                    sTempValue.mHighDigit   = 0;
                    sTempValue.mLowDigit    = gPreDefinedPow[sExponent];
                    STL_TRY( stlMulUInt128( sDividend, sTempValue, &sDividend ) == STL_TRUE );
                }
                else
                {
                    /* Do Nothing */
                }

            }
            else
            {
                sDividend.mHighDigit    = 0;
                sDividend.mLowDigit     = gPreDefinedPow[-aScale];
            }

            STL_TRY( stlDivUInt128( aValue, sDividend, &sQuotient ) == STL_TRUE );
            STL_TRY( stlMulUInt128( sQuotient, sDividend, &sMul ) == STL_TRUE );

            STL_COPY_UINT128( &sTempValue, &sDividend );
            sTempValue2.mHighDigit  = 0;
            sTempValue2.mLowDigit   = 10;
            STL_TRY( stlDivUInt128( sTempValue, sTempValue2, &sTempValue ) == STL_TRUE );
            STL_TRY( stlSubUInt128( aValue, sMul, &sRem ) == STL_TRUE );

            if( stlCmpUInt128( sRem, sTempValue ) >= 0 )
            {
                STL_TRY( stlAddUInt128( sMul, sDividend, aResult ) == STL_TRUE );
            }
            else
            {
                STL_COPY_UINT128( aResult, &sMul );
            }
        }
        else
        {
            /* aValue의 값이 STL_UINT64_MAX와 같거나 작은 경우 */

            /* UInt64의 MAX값은 1.xx * 10^20이다.
             * 따라서 position이 20보다 크면 0이다. */
            if( (-aScale) > (gPreDefinedPowMax + 1) )
            {
                aResult->mHighDigit = 0;
                aResult->mLowDigit  = 0;
            }
            else if( (-aScale) > gPreDefinedPowMax )
            {
                sDividend.mHighDigit    = 0;
                sDividend.mLowDigit     = gPreDefinedPow[gPreDefinedPowMax];
                sExponent               = (-aScale) - gPreDefinedPowMax;
                while( sExponent > gPreDefinedPowMax )
                {
                    sTempValue.mHighDigit   = 0;
                    sTempValue.mLowDigit    = gPreDefinedPow[gPreDefinedPowMax];
                    STL_TRY( stlMulUInt128( sDividend, sTempValue, &sDividend ) == STL_TRUE );
                    sExponent               = sExponent - gPreDefinedPowMax;
                }
                if( sExponent > 0 )
                {
                    sTempValue.mHighDigit   = 0;
                    sTempValue.mLowDigit    = gPreDefinedPow[sExponent];
                    STL_TRY( stlMulUInt128( sDividend, sTempValue, &sDividend ) == STL_TRUE );
                }
                else
                {
                    /* Do Nothing */
                }

                STL_TRY( stlDivUInt128( aValue, sDividend, &sQuotient ) == STL_TRUE );
                STL_TRY( stlMulUInt128( sQuotient, sDividend, &sMul ) == STL_TRUE );

                STL_COPY_UINT128( &sTempValue, &sDividend );
                sTempValue2.mHighDigit  = 0;
                sTempValue2.mLowDigit   = 10;
                STL_TRY( stlDivUInt128( sTempValue, sTempValue2, &sTempValue ) == STL_TRUE );
                STL_TRY( stlSubUInt128( aValue, sMul, &sRem ) == STL_TRUE );

                if( stlCmpUInt128( sRem, sTempValue ) >= 0 )
                {
                    STL_TRY( stlAddUInt128( sMul, sDividend, aResult ) == STL_TRUE );
                }
                else
                {
                    STL_COPY_UINT128( aResult, &sMul );
                }
            }

            else
            {
                sTemp64             = ( aValue.mLowDigit / gPreDefinedPow[-aScale] ) * 
                    gPreDefinedPow[-aScale];
                aResult->mHighDigit = 0;
                aResult->mLowDigit  = sTemp64;

                if( (aValue.mLowDigit - sTemp64) >= gPreDefinedPow[-aScale-1] )
                {
                    sTempValue.mHighDigit   = 0;
                    sTempValue.mLowDigit    = gPreDefinedPow[-aScale];
                    STL_TRY( stlAddUInt128( *aResult, sTempValue, aResult ) == STL_TRUE );
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }
    }

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief uint128의 값에 대해 scale에 대한 trunc 연산을 수행한다.
 * @param aValue trunc 연산을 수행할 값
 * @param aScale scale 값
 * @param aResult aValue의 trunc 연산 결과를 저장할 포인터
 * @return 에러 발생시 STL_FALSE
 *         그렇지 않은 경우 STL_TRUE
 */
stlBool stlTruncUInt128( stlUInt128     aValue,
                         stlInt32       aScale,
                         stlUInt128   * aResult )
{
    stlUInt128  sQuotient;
    stlUInt128  sDividend;
    stlUInt128  sMul;
    stlUInt128  sTempValue;
    stlUInt32   sExponent;

    if( STL_IS_UINT128_ZERO( aValue ) )
    {
        aResult->mHighDigit = 0;
        aResult->mLowDigit  = 0;
    }
    else if( aScale >= 0 )
    {
        STL_COPY_UINT128( aResult, &aValue );
    }
    else if( (-aScale) > STL_UINT128_PRECISION )
    {
        aResult->mHighDigit = 0;
        aResult->mLowDigit  = 0;
    }
    else
    {
        if( aValue.mHighDigit > 0 )
        {
            /* aValue의 값이 STL_UINT64_MAX보다 큰 경우 */

            if( (-aScale) > gPreDefinedPowMax )
            {
                sDividend.mHighDigit    = 0;
                sDividend.mLowDigit     = gPreDefinedPow[gPreDefinedPowMax];
                sExponent               = (-aScale) - gPreDefinedPowMax;
                while( sExponent > gPreDefinedPowMax )
                {
                    sTempValue.mHighDigit   = 0;
                    sTempValue.mLowDigit    = gPreDefinedPow[gPreDefinedPowMax];
                    STL_TRY( stlMulUInt128( sDividend, sTempValue, &sDividend ) == STL_TRUE );
                    sExponent               = sExponent - gPreDefinedPowMax;
                }
                if( sExponent > 0 )
                {
                    sTempValue.mHighDigit   = 0;
                    sTempValue.mLowDigit    = gPreDefinedPow[sExponent];
                    STL_TRY( stlMulUInt128( sDividend, sTempValue, &sDividend ) == STL_TRUE );
                }
                else
                {
                    /* Do Nothing */
                }

            }
            else
            {
                sDividend.mHighDigit    = 0;
                sDividend.mLowDigit     = gPreDefinedPow[-aScale];
            }

            STL_TRY( stlDivUInt128( aValue, sDividend, &sQuotient ) == STL_TRUE );
            STL_TRY( stlMulUInt128( sQuotient, sDividend, &sMul ) == STL_TRUE );

            STL_COPY_UINT128( aResult, &sMul );
        }
        else
        {
            /* aValue의 값이 STL_UINT64_MAX와 같거나 작은 경우 */

            /* UInt64의 MAX값은 1.xx * 10^20이다.
             * 따라서 position이 19보다 크면 0이다. */
            if( (-aScale) > gPreDefinedPowMax )
            {
                aResult->mHighDigit = 0;
                aResult->mLowDigit  = 0;
            }
            else
            {
                aResult->mHighDigit = 0;
                aResult->mLowDigit  = ( aValue.mLowDigit / gPreDefinedPow[-aScale] ) * 
                    gPreDefinedPow[-aScale];
            }
        }
    }

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}


/** @} */
