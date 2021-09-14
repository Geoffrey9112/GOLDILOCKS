/*******************************************************************************
 * dtfDump.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    현재 NaN은 고려되어 있지 않다.
 *    함수의 피연산자에 NULL값이 오지 않는다고 가정하고 구현되어 있다.
 ******************************************************************************/

/**
 * @file 
 * @brief Dump Function DataType Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfSystem.h>

/**
 * @ingroup dtfArithmetic
 * @internal
 * @{
 */

/*******************************************************************************
 * Dump
 *******************************************************************************/

const stlChar dtfDecimalString[ 256 ][ 4 ] =
{
    /* 0 ~ 99 */
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
    "60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
    "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
    "80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
    "90", "91", "92", "93", "94", "95", "96", "97", "98", "99",
   
    /* 100 ~ 199 */
    "100", "101", "102", "103", "104", "105", "106", "107", "108", "109",
    "110", "111", "112", "113", "114", "115", "116", "117", "118", "119",
    "120", "121", "122", "123", "124", "125", "126", "127", "128", "129",
    "130", "131", "132", "133", "134", "135", "136", "137", "138", "139",
    "140", "141", "142", "143", "144", "145", "146", "147", "148", "149",
    "150", "151", "152", "153", "154", "155", "156", "157", "158", "159",
    "160", "161", "162", "163", "164", "165", "166", "167", "168", "169",
    "170", "171", "172", "173", "174", "175", "176", "177", "178", "179",
    "180", "181", "182", "183", "184", "185", "186", "187", "188", "189",
    "190", "191", "192", "193", "194", "195", "196", "197", "198", "199",

    /* 200 ~ 255 */
    "200", "201", "202", "203", "204", "205", "206", "207", "208", "209",
    "210", "211", "212", "213", "214", "215", "216", "217", "218", "219",
    "220", "221", "222", "223", "224", "225", "226", "227", "228", "229",
    "230", "231", "232", "233", "234", "235", "236", "237", "238", "239",
    "240", "241", "242", "243", "244", "245", "246", "247", "248", "249",
    "250", "251", "252", "253", "254", "255"
};

const stlInt32 dtfDecimalStringSize[ 256 ] =
{
    /* 0 ~ 99 */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

    /* 100 ~ 199 */
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

    /* 200 ~ 255 */
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3
};


dtfBuiltInFuncPtr gDtfDumpFunc[ DTL_TYPE_MAX ] =
{
    dtfBooleanDump,
    dtfSmallIntDump,
    dtfIntegerDump,
    dtfBigIntDump,
    
    dtfRealDump,
    dtfDoubleDump,

    dtfNumericDump,    
    dtfNumericDump,
    dtfNumericDump,

    dtfCharDump,
    dtfVarcharDump,
    dtfLongvarcharDump,
    dtfInvalidDump,

    dtfBinaryDump,
    dtfVarbinaryDump,
    dtfLongvarbinaryDump,
    dtfInvalidDump,

    dtfDateDump,
    dtfTimeDump,
    dtfTimestampDump,
    dtfTimeTzDump,
    dtfTimestampTzDump,

    dtfIntervalYearToMonthDump,
    dtfIntervalDaytoSecondDump,

    dtfRowIdDump
};


stlSize dtfGetDeciamlStringFromBytesByEndian( stlChar   * aByte,
                                              stlInt32    aByteLen,
                                              stlChar   * aHexaString )
{
    stlSize     sTotalStrLen = 0;
    stlInt32    sStrLen      = 0;
    stlInt32    sLoop        = 0;
    stlChar   * sDecimalStr  = NULL;
    
#ifdef STL_IS_BIGENDIAN
    for( ; aByteLen > 0 ; aByteLen-- )
    {
        sStrLen     = dtfDecimalStringSize[ (stlUInt8) *aByte ];
        sDecimalStr = (stlChar *) dtfDecimalString[ (stlUInt8) *aByte ];

        for( sLoop = 0 ; sLoop < sStrLen ; sLoop++ )
        {
            *aHexaString = sDecimalStr[ sLoop ];
            aHexaString++;
        }
        
        *aHexaString = ',';
        aHexaString++;
        
        sTotalStrLen += sStrLen + 1;
        aByte++;

        if( sTotalStrLen > DTL_VARCHAR_MAX_PRECISION - DTL_DUMP_FUNC_TYPE_INFO_SIZE )
        {
            aHexaString[-1] = '.';
            aHexaString[0] = '.';
            aHexaString[1] = '.';

            sTotalStrLen += 3;
            break;
        }
    }
#else
    aByte += (aByteLen - 1);
    for( ; aByteLen > 0 ; aByteLen-- )
    {
        sStrLen     = dtfDecimalStringSize[ (stlUInt8) *aByte ];
        sDecimalStr = (stlChar *) dtfDecimalString[ (stlUInt8) *aByte ];

        for( sLoop = 0 ; sLoop < sStrLen ; sLoop++ )
        {
            *aHexaString = sDecimalStr[ sLoop ];
            aHexaString++;
        }
        
        *aHexaString = ',';
        aHexaString++;
        
        sTotalStrLen += sStrLen + 1;
        aByte--;

        if( sTotalStrLen > DTL_VARCHAR_MAX_PRECISION - DTL_DUMP_FUNC_TYPE_INFO_SIZE )
        {
            aHexaString[-1] = '.';
            aHexaString[0] = '.';
            aHexaString[1] = '.';

            sTotalStrLen += 3;
            break;
        }
    }
#endif
    return --sTotalStrLen;
}


stlSize dtfGetDeciamlStringFromBytes( stlChar   * aByte,
                                      stlInt32    aByteLen,
                                      stlChar   * aHexaString )
{
    stlSize     sTotalStrLen = 0;
    stlInt32    sStrLen      = 0;
    stlInt32    sLoop        = 0;
    stlChar   * sDecimalStr  = NULL;
    
    for( ; aByteLen > 0 ; aByteLen-- )
    {
        sStrLen     = dtfDecimalStringSize[ (stlUInt8) *aByte ];
        sDecimalStr = (stlChar *) dtfDecimalString[ (stlUInt8) *aByte ];

        for( sLoop = 0 ; sLoop < sStrLen ; sLoop++ )
        {
            *aHexaString = sDecimalStr[ sLoop ];
            aHexaString++;
        }
        
        *aHexaString = ',';
        aHexaString++;
        
        sTotalStrLen += sStrLen + 1;
        aByte++;

        if( sTotalStrLen > DTL_VARCHAR_MAX_PRECISION - DTL_DUMP_FUNC_TYPE_INFO_SIZE )
        {
            aHexaString[-1] = '.';
            aHexaString[0] = '.';
            aHexaString[1] = '.';

            sTotalStrLen += 3;
            break;
        }
    }

    return --sTotalStrLen;
}


/**
 * @brief Invalid Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfInvalidDump( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    return STL_FAILURE;
}


/**
 * @brief Boolean의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=BOOLEAN Len=1 : Val=40"
 * <BR>     "Type=BOOLEAN Len=1 : Val=41"
 */
stlStatus dtfBooleanDump( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_BOOLEAN, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Val=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytes( (stlChar *) sValue1->mValue,
                                             sValue1->mLength,
                                             & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SmallInt의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=NATIVE_SMALLINT Len=2 : Val=20, 40"
 */
stlStatus dtfSmallIntDump( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_SMALLINT, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Val=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) sValue1->mValue,
                                                     sValue1->mLength,
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Integer의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=NATIVE_INTEGER Len=4 : Val=1,56,3,40"
 */
stlStatus dtfIntegerDump( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_INTEGER, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Val=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) sValue1->mValue,
                                                     sValue1->mLength,
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Bigint의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=NATIVE_BIGINT Len=8 : Val=0,2,0,0,4,56,1,40"
 */
stlStatus dtfBigIntDump( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Val=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) sValue1->mValue,
                                                     sValue1->mLength,
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

    
/**
 * @brief Real의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=NATIVE_REAL Len=4 : Val=40,45,246,31"
 */
stlStatus dtfRealDump( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_REAL, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Val=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) sValue1->mValue,
                                                     sValue1->mLength,
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Native_Double의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=NATIVE_DOUBLE Len=8 : Val=1,23,5,6,7,2,1,22"
 */
stlStatus dtfDoubleDump( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_DOUBLE, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Val=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) sValue1->mValue,
                                                     sValue1->mLength,
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Numeric의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=NUMERIC Len=1 : Expo=40 Digit="
 * <BR>     "Type=NUMERIC Len=1 : Val=41"
 */
stlStatus dtfNumericDump( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;
    dtlNumericType  * sNumeric;
    stlSize           sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Expo=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sNumeric = (dtlNumericType *) sValue1->mValue;
        
    sLength += dtfGetDeciamlStringFromBytes( (stlChar *) & sNumeric->mData[0],
                                             1,
                                             & ((stlChar *) sResultValue->mValue)[ sLength ] );

    if( sValue1->mLength > 1 )
    {
        sLength += stlSnprintf( & ((stlChar *) sResultValue->mValue)[ sLength ],
                                DTL_VARCHAR_MAX_PRECISION,
                                " Digit=" );

        sLength += dtfGetDeciamlStringFromBytes( (stlChar *) & sNumeric->mData[1],
                                                 sValue1->mLength - 1,
                                                 & ((stlChar *) sResultValue->mValue)[ sLength ] );
    }
    else
    {
        sLength += stlSnprintf( & ((stlChar *) sResultValue->mValue)[ sLength ],
                                DTL_VARCHAR_MAX_PRECISION,
                                " Digit=(empty)" );
    }

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Char의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=CHAR Len=5 : Val=67,72,67,87,88"
 */
stlStatus dtfCharDump( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_CHAR, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Str=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytes( (stlChar *) sValue1->mValue,
                                             sValue1->mLength,
                                             & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Varchar의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=VARCHAR Len=3 : Str=56,33,78"
 */
stlStatus dtfVarcharDump( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_VARCHAR, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Str=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytes( (stlChar *) sValue1->mValue,
                                             sValue1->mLength,
                                             & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Longvarchar의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=LONG VARCHAR Len=2 : Str=42,67"
 */
stlStatus dtfLongvarcharDump( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_LONGVARCHAR, (stlErrorStack *)aEnv );

    STL_DASSERT( sResultValue->mBufferSize == DTL_VARCHAR_MAX_PRECISION );
    
    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Str=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytes( (stlChar *) sValue1->mValue,
                                             sValue1->mLength,
                                             & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Binary의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=BINARY Len=3 : Str=38,231,40"
 */
stlStatus dtfBinaryDump( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_BINARY, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Str=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytes( (stlChar *) sValue1->mValue,
                                             sValue1->mLength,
                                             & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Varbinary의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=VARBINARY Len=3 : Str=234,4,221"
 */
stlStatus dtfVarbinaryDump( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_VARBINARY, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Str=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytes( (stlChar *) sValue1->mValue,
                                             sValue1->mLength,
                                             & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Longvarbinary의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=LONG VARBINARY Len=2 : Str=123,234"
 */
stlStatus dtfLongvarbinaryDump( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_LONGVARBINARY, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Str=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytes( (stlChar *) sValue1->mValue,
                                             sValue1->mLength,
                                             & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Date의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=DATE Len=4 : Day=40,00,00,00"
 */
stlStatus dtfDateDump( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_DATE, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Day=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) sValue1->mValue,
                                                     sValue1->mLength,
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Time의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=TIME Len=8 : Sec=40,00,00,00,123,64,12,77"
 */
stlStatus dtfTimeDump( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIME, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Sec=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) sValue1->mValue,
                                                     sValue1->mLength,
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Timestamp의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=TIMESTAMP Len=8 : Val=12,23,34,45,67,89,00"
 */
stlStatus dtfTimestampDump( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Sec=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) sValue1->mValue,
                                                     sValue1->mLength,
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief TimeTz의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=TIMETZ Len=12 : Sec=40,0,0,0,12,33,44,66 Zone=0,23,33,77"
 */
stlStatus dtfTimeTzDump( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIME_WITH_TIME_ZONE, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Sec=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) & ((dtlTimeTzType*) sValue1->mValue)->mTime,
                                                     STL_SIZEOF( ((dtlTimeTzType*) sValue1->mValue)->mTime ),
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sLength += stlSnprintf( & ((stlChar *) sResultValue->mValue)[ sLength ],
                            DTL_VARCHAR_MAX_PRECISION,
                            " Zone=" );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) & ((dtlTimeTzType*) sValue1->mValue)->mTimeZone,
                                                     STL_SIZEOF( ((dtlTimeTzType*) sValue1->mValue)->mTimeZone ),
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief TimestampTz의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=TIMESTAMPTZ Len=12 : Sec=40,0,0,0,123,234,34,00 Zone=0,23,33,77"
 */
stlStatus dtfTimestampTzDump( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Sec=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) & ((dtlTimestampTzType*) sValue1->mValue)->mTimestamp,
                                                     STL_SIZEOF( ((dtlTimestampTzType*) sValue1->mValue)->mTimestamp ),
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sLength += stlSnprintf( & ((stlChar *) sResultValue->mValue)[ sLength ],
                            DTL_VARCHAR_MAX_PRECISION,
                            " Zone=" );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) & ((dtlTimestampTzType*) sValue1->mValue)->mTimeZone,
                                                     STL_SIZEOF( ((dtlTimestampTzType*) sValue1->mValue)->mTimeZone ),
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Interval Year To Month의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=DTL_TYPE_INTERVAL_YM(YY) Len=8 : Ind=00,00,00,01 Mon=123,34,64,66"
 */
stlStatus dtfIntervalYearToMonthDump( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, (stlErrorStack *)aEnv );

    sLength = stlSnprintf(
        (stlChar *) sResultValue->mValue,
        DTL_VARCHAR_MAX_PRECISION,
        "Type=%s(%s) Len=%d : Ind=",
        dtlDataTypeNameForDumpFunc[ sValue1->mType ],
        gDtlIntervalIndicatorStringForDump[ ((dtlIntervalYearToMonthType *) sValue1->mValue)->mIndicator ],
        sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) & ((dtlIntervalYearToMonthType*) sValue1->mValue)->mIndicator,
                                                     STL_SIZEOF( ((dtlIntervalYearToMonthType*) sValue1->mValue)->mIndicator ),
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sLength += stlSnprintf( & ((stlChar *) sResultValue->mValue)[ sLength ],
                            DTL_VARCHAR_MAX_PRECISION,
                            " Mon=" );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) & ((dtlIntervalYearToMonthType*) sValue1->mValue)->mMonth,
                                                     STL_SIZEOF( ((dtlIntervalYearToMonthType*) sValue1->mValue)->mMonth ),
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Interval Day To Second의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=DTL_TYPE_INTERVAL_DS(DS) Len=12 : Ind=00,00,00,01 Day=123,34,64,66 Sec=111,88,233,33"
 */
stlStatus dtfIntervalDaytoSecondDump( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_DAY_TO_SECOND, (stlErrorStack *)aEnv );

    sLength = stlSnprintf(
        (stlChar *) sResultValue->mValue,
        DTL_VARCHAR_MAX_PRECISION,
        "Type=%s(%s) Len=%d : Ind=",
        dtlDataTypeNameForDumpFunc[ sValue1->mType ],
        gDtlIntervalIndicatorStringForDump[ ((dtlIntervalDayToSecondType *) sValue1->mValue)->mIndicator ],
        sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) & ((dtlIntervalDayToSecondType*) sValue1->mValue)->mIndicator,
                                                     STL_SIZEOF( ((dtlIntervalDayToSecondType*) sValue1->mValue)->mIndicator ),
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sLength += stlSnprintf( & ((stlChar *) sResultValue->mValue)[ sLength ],
                            DTL_VARCHAR_MAX_PRECISION,
                            " Day=" );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) & ((dtlIntervalDayToSecondType*) sValue1->mValue)->mDay,
                                                     STL_SIZEOF( ((dtlIntervalDayToSecondType*) sValue1->mValue)->mDay ),
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sLength += stlSnprintf( & ((stlChar *) sResultValue->mValue)[ sLength ],
                            DTL_VARCHAR_MAX_PRECISION,
                            " Sec=" );

    sLength += dtfGetDeciamlStringFromBytesByEndian( (stlChar *) & ((dtlIntervalDayToSecondType*) sValue1->mValue)->mTime,
                                                     STL_SIZEOF( ((dtlIntervalDayToSecondType*) sValue1->mValue)->mTime ),
                                                     & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief RowID의 Dump 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Dump
 * <BR> ex) "Type=ROWID Len=16 : Str=123,45,35,34,12,67,99,34,89,11,0,0,12,11,87,211"
 */
stlStatus dtfRowIdDump( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlSize         sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_ROWID, (stlErrorStack *)aEnv );

    sLength = stlSnprintf( (stlChar *) sResultValue->mValue,
                           DTL_VARCHAR_MAX_PRECISION,
                           "Type=%s Len=%d : Str=",
                           dtlDataTypeNameForDumpFunc[ sValue1->mType ],
                           sValue1->mLength );

    sLength += dtfGetDeciamlStringFromBytes( (stlChar *) sValue1->mValue,
                                             sValue1->mLength,
                                             & ((stlChar *) sResultValue->mValue)[ sLength ] );

    sResultValue->mLength = sLength;
    
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */

