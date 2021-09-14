#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stl.h>

#define PRINTERR                                                    \
    do                                                              \
    {                                                               \
        printf( "ERRCODE: %d, ERRSTRING: %s\n",                     \
                stlGetLastErrorCode( &sErrStack ),                  \
                stlGetLastErrorMessage( &sErrStack ) );             \
    } while(0)

/**
 * @brief stlSnprintf() 함수 테스트
 */
stlBool TestSnprintf0NULL()
{
    stlInt32 sRet;

    sRet = stlSnprintf(NULL, 0, "%sBAR", "FOO");
    STL_TRY(sRet == 0);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlSnprintf() 함수 테스트
 */
stlBool TestSnprintf0NonNULL()
{
    stlInt32 sRet;
    stlChar *sBuf = "testing";

    sRet = stlSnprintf(sBuf, 0, "%sBAR", "FOO");
    STL_TRY( sRet == 0 );
    STL_TRY( stlStrcmp(sBuf, "testing") == 0 );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlSnprintf() 함수 테스트
 */
stlBool TestSnprintfNoNULL()
{
    stlChar  sBuf[100];
    stlChar  sTesting[10];

    sTesting[0] = 't';
    sTesting[1] = 'e';
    sTesting[2] = 's';
    sTesting[3] = 't';
    sTesting[4] = 'i';
    sTesting[5] = 'n';
    sTesting[6] = 'g';
    
    /* If this test fails, we are going to seg fault. */
    stlSnprintf(sBuf, sizeof(sBuf), "%.*s", 7, sTesting);
    STL_TRY( strncmp(sBuf, sTesting, 7) == 0 );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlSnprintf() 함수 테스트
 */
stlBool TestSnprintfUnderflow()
{
    stlChar  sBuf[20];
    stlInt32 sRet;

    sRet = stlSnprintf(sBuf, sizeof(sBuf), "%.2lf", (double)0.0001);
    STL_TRY(4 == sRet);
    STL_TRY(stlStrcmp("0.00", sBuf) == 0);
    
    sRet = stlSnprintf(sBuf, sizeof(sBuf), "%.2lf", (double)0.001);
    STL_TRY(4 == sRet);
    STL_TRY(stlStrcmp("0.00", sBuf) == 0);
    
    sRet = stlSnprintf(sBuf, sizeof(sBuf), "%.2lf", (double)0.01);
    STL_TRY(4 == sRet);
    STL_TRY(stlStrcmp("0.01", sBuf) == 0);
    
    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlStrtok() 함수 테스트
 */
stlBool TestStrtok()
{
    struct
    {
        stlChar *mInput;
        stlChar *mSep;
    }
    sCases[] = {
        {
            "",
            "Z"
        },
        {
            "      asdf jkl; 77889909            \r\n\1\2\3Z",
            " \r\n\3\2\1"
        },
        {
            NULL,  /* but who cares if apr_strtok() segfaults? */
            " \t"
        },
        {
            "   a       b        c   ",
            ""
        },
        {
            "a              b c         ",
            " "
        }
    };
    
    stlInt32 sCur;
    stlChar *sRetVal1;
    stlChar *sRetVal2;
    stlChar *sStr1;
    stlChar *sStr2;
    stlChar  sStrA[128];
    stlChar  sStrB[128];    
    stlChar *sSep1;
    stlChar *sSep2;

    for (sCur = 0; sCur < (sizeof(sCases) / sizeof(sCases[0])); sCur++)
    {
        if ( sCases[sCur].mInput == NULL )
        {
            sStr1 = NULL;
            sStr2 = NULL;
        }
        else
        {
            strcpy( sStrA, sCases[sCur].mInput );
            strcpy( sStrB, sCases[sCur].mInput );
            sStr1 = sStrA;
            sStr2 = sStrB;
        }
            
        do
        {
            sRetVal1 = stlStrtok( sStr1, sCases[sCur].mSep, &sSep1 );
            if ( sCases[sCur].mInput == NULL )
            {
                // ANSI strtok() 은 NULL 입력시 segment fault
                sRetVal2 = NULL;
            }
            else
            {
                sRetVal2 = strtok_r( sStr2 , sCases[sCur].mSep, &sSep2 );
            }
            
            if (sRetVal1 == NULL)
            {
                STL_TRY(sRetVal2 == NULL);
            }
            else
            {
                STL_TRY( sRetVal2 != NULL );
                STL_TRY( stlStrcmp( sRetVal1, sRetVal2 ) == 0 );
            }
            
            /* make sure we pass NULL on subsequent calls */
            sStr1 = NULL;
            sStr2 = NULL;
        } while (sRetVal1);
    }

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}



#define MY_LLONG_MAX (STL_INT64_C(9223372036854775807))
#define MY_LLONG_MIN (-MY_LLONG_MAX - STL_INT64_C(1))

/**
 * @brief stlStrToInt64() 함수 테스트
 */
stlBool TestStringStrtoi64()
{
    static const struct
    {
        stlInt32       mErrNum;
        stlInt32       mBase;
        const stlChar *mIn;
        const stlChar *mEnd;
        stlInt64       mResult;
    } sTest[] = {
        
        /* base 10 tests */
        { STL_SUCCESS, 10, "123545", NULL, STL_INT64_C(123545) },
        { STL_SUCCESS, 10, "   123545", NULL, STL_INT64_C(123545) },
        { STL_SUCCESS, 10, "   +123545", NULL, STL_INT64_C(123545) },
        { STL_SUCCESS, 10, "-123545", NULL, STL_INT64_C(-123545) },
        { STL_SUCCESS, 10, "   00000123545", NULL, STL_INT64_C(123545) },
        { STL_SUCCESS, 10, "123545ZZZ", "ZZZ", STL_INT64_C(123545) },
        { STL_SUCCESS, 10, "   123545   ", "   ", STL_INT64_C(123545) },

        /* base 16 tests */
        { STL_SUCCESS, 16, "1E299", NULL, STL_INT64_C(123545) },
        { STL_SUCCESS, 16, "1e299", NULL, STL_INT64_C(123545) },
        { STL_SUCCESS, 16, "0x1e299", NULL, STL_INT64_C(123545) },
        { STL_SUCCESS, 16, "0X1E299", NULL, STL_INT64_C(123545) },
        { STL_SUCCESS, 16, "+1e299", NULL, STL_INT64_C(123545) },
        { STL_SUCCESS, 16, "-1e299", NULL, STL_INT64_C(-123545) },
        { STL_SUCCESS, 16, "   -1e299", NULL, STL_INT64_C(-123545) },

        /* automatic base detection tests */
        { STL_SUCCESS, 0, "123545", NULL, STL_INT64_C(123545) },
        { STL_SUCCESS, 0, "0x1e299", NULL, STL_INT64_C(123545) },
        { STL_SUCCESS, 0, "  0x1e299", NULL, STL_INT64_C(123545) },
        { STL_SUCCESS, 0, "+0x1e299", NULL, STL_INT64_C(123545) },
        { STL_SUCCESS, 0, "-0x1e299", NULL, STL_INT64_C(-123545) },

        /* large number tests */
        { STL_SUCCESS, 10, "8589934605", NULL, STL_INT64_C(8589934605) },
        { STL_SUCCESS, 10, "-8589934605", NULL, STL_INT64_C(-8589934605) },
        { STL_SUCCESS, 16, "0x20000000D", NULL, STL_INT64_C(8589934605) },
        { STL_SUCCESS, 16, "-0x20000000D", NULL, STL_INT64_C(-8589934605) },
        { STL_SUCCESS, 16, "   0x20000000D", NULL, STL_INT64_C(8589934605) },
        { STL_SUCCESS, 16, "   0x20000000D", NULL, STL_INT64_C(8589934605) },

        /* error cases */
        { STL_ERRCODE_OUT_OF_RANGE, 10, "999999999999999999999999999999999", "", MY_LLONG_MAX },
        { STL_ERRCODE_OUT_OF_RANGE, 10, "-999999999999999999999999999999999", "", MY_LLONG_MIN },

        /* some strtoll implementations give EINVAL when no conversion
         * is performed. */
        { STL_ERRCODE_STRING_IS_NOT_NUMBER /* don't care */, 10, "zzz", "zzz", STL_INT64_C(0) },
        { STL_ERRCODE_STRING_IS_NOT_NUMBER /* don't care */, 10, "", NULL, STL_INT64_C(0) }

    };

    stlInt32   sIdx;
    stlChar  * sEnd;
    stlStatus  sResult;
    stlInt64   sNumber;
    stlInt64   sDummy;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    for (sIdx = 0; sIdx < (sizeof(sTest)/sizeof(sTest[0])); sIdx++)
    {
        sResult = stlStrToInt64( sTest[sIdx].mIn,
                                 &sEnd,
                                 sTest[sIdx].mBase,
                                 &sNumber,
                                 &sErrStack );
        if ( sResult == STL_SUCCESS )
        {
            STL_TRY( sTest[sIdx].mErrNum == STL_SUCCESS );
            STL_TRY( sNumber == sTest[sIdx].mResult );
                     
            if ( sTest[sIdx].mEnd == NULL )
            {
                STL_TRY( *sEnd == '\0' );
            }
            else
            {
                stlChar * sValidEnd;

                sDummy = strtol( sTest[sIdx].mIn, &sValidEnd, sTest[sIdx].mBase );
                STL_TRY( sEnd == sValidEnd );
            }
        }
        else
        {
            STL_TRY( sTest[sIdx].mErrNum == stlGetLastErrorCode( &sErrStack ) );
        }
    }

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlStrToFloat64() 함수 테스트
 */
stlBool TestStringStrtoff()
{
    stlFloat64   sResult;
    stlChar    * sEndPtr;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlStrToFloat64( "999999999999999999999999999999",
                              &sEndPtr,
                              &sResult,
                              &sErrStack )
             == STL_SUCCESS);
    STL_TRY( sResult > 0 );
    
    STL_TRY( stlStrToFloat64( "1234",
                              &sEndPtr,
                              &sResult,
                              &sErrStack )
             == STL_SUCCESS);
    STL_TRY( sResult == 1234);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief sltStrncpy() 함수 테스트
 */
stlBool TestStringCpyStrn()
{
    stlChar  sBuf[6];
    stlChar *sRet;
    
    sBuf[5] = 'Z';

    sRet = stlStrncpy(sBuf, "123456", 5);

    STL_TRY( stlStrcmp("1234", sBuf) == 0 );
    STL_TRY( &sBuf[4] == sRet);
    STL_TRY( sBuf[4] == '\0');
    STL_TRY( sBuf[5] == 'Z');

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief sltSnprintf() 함수 테스트
 */
stlBool TestSnprintfOverflow()
{
    stlChar  sBuf[4];
    stlInt32 sRet;
    
    sBuf[2] = '4';
    sBuf[3] = '2';

    sRet = stlSnprintf(sBuf, 2, "%s", "a");
    STL_TRY(1 == sRet);

    sRet = stlSnprintf(sBuf, 2, "%s", "abcd");
    STL_TRY(1 == sRet);

    STL_TRY(stlStrcmp("a", sBuf) == 0);

    /* Check the buffer really hasn't been overflowed. */
    STL_TRY(sBuf[2] == '4');
    STL_TRY(sBuf[3] == '2');

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test Snprintf0NULL
     */
    sState = 1;
    STL_TRY(TestSnprintf0NULL() == STL_TRUE);

    /*
     * Test Snprintf0NonNULL
     */
    sState = 2;
    STL_TRY(TestSnprintf0NonNULL() == STL_TRUE);

    /*
     * Test SnprintfNoNULL
     */
    sState = 3;
    STL_TRY(TestSnprintfNoNULL() == STL_TRUE);

    /*
     * Test SnprintfUnderflow
     */
    sState = 4;
    STL_TRY(TestSnprintfUnderflow() == STL_TRUE);

    /*
     * Test Strtok
     */
    sState = 5;
    STL_TRY(TestStrtok() == STL_TRUE);

    /*
     * Test StringStrtoi64
     */
    sState = 8;
    STL_TRY(TestStringStrtoi64() == STL_TRUE);

    /*
     * Test StringStrtoff
     */
    sState = 9;
    STL_TRY(TestStringStrtoff() == STL_TRUE);

    /*
     * Test StringCpyStrn
     */
    sState = 12;
    STL_TRY(TestStringCpyStrn() == STL_TRUE);

    /*
     * Test SnprintfOverflow
     */
    sState = 13;
    STL_TRY(TestSnprintfOverflow() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
