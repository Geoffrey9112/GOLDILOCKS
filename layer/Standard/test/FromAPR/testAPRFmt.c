#include <stdio.h>
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
stlBool TestSsizeFmt()
{
    stlChar  sBuf[100];
    stlSsize sVar = 0;

    sprintf(sBuf, "%ld", sVar);
    STL_TRY( stlStrcmp("0", sBuf) == 0 );

    stlSnprintf(sBuf, sizeof(sBuf), "%ld", sVar);
    STL_TRY(stlStrcmp("0", sBuf) == 0);

    return STL_TRUE;

    STL_FINISH;
    
    return STL_FALSE;
}

/**
 * @brief stlSnprintf() 함수 테스트
 */
stlBool TestSizeFmt()
{
    stlChar  sBuf[100];
    stlSsize sVar = 0;

    sprintf(sBuf, "%ld", sVar);
    STL_TRY(stlStrcmp("0", sBuf) == 0);

    stlSnprintf(sBuf, sizeof(sBuf), "%ld", sVar);
    STL_TRY(stlStrcmp("0", sBuf) == 0);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlSnprintf() 함수 테스트
 */
stlBool TestOffFmt()
{
    stlChar  sBuf[100];
    stlOffset   sVar = 0;

    sprintf(sBuf, "%lu", sVar);
    STL_TRY(stlStrcmp("0", sBuf) == 0);

    stlSnprintf(sBuf, sizeof(sBuf), "%lu", sVar);
    STL_TRY(stlStrcmp("0", sBuf) == 0);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlSnprintf() 함수 테스트
 */
stlBool TestPidFmt()
{
    stlChar  sBuf[100];
    pid_t    sVar = 0;

    sprintf(sBuf, "%u", sVar);
    STL_TRY(stlStrcmp("0", sBuf) == 0);

    stlSnprintf(sBuf, sizeof(sBuf), "%u", sVar);
    STL_TRY(stlStrcmp("0", sBuf) == 0);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlSnprintf() 함수 테스트
 */
stlBool TestInt64Fmt()
{
    stlChar  sBuf[100];
    stlInt64 sVar = 0;

    sprintf(sBuf, "%ld", sVar);
    STL_TRY(stlStrcmp("0", sBuf) == 0);

    stlSnprintf(sBuf, sizeof(sBuf), "%ld", sVar);
    STL_TRY(stlStrcmp("0", sBuf) == 0);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlSnprintf() 함수 테스트
 */
stlBool TestUInt64Fmt()
{
    stlChar   sBuf[100];
    stlUInt64 sVar = STL_UINT64_C(14000000);

    sprintf(sBuf, "%lu", sVar);
    STL_TRY(stlStrcmp("14000000", sBuf) == 0);

    stlSnprintf(sBuf, sizeof(sBuf), "%lu", sVar);
    STL_TRY(stlStrcmp("14000000", sBuf) == 0);

    return STL_TRUE;

    STL_FINISH;
    
    return STL_FALSE;
}

/**
 * @brief stlSnprintf() 함수 테스트
 */
stlBool TestUInt64HexFmt()
{
    stlChar   sBuf[100];
    stlUInt64 sVar = STL_UINT64_C(14000000);

    sprintf(sBuf, "%lx", sVar);
    STL_TRY(stlStrcmp("d59f80", sBuf) == 0);

    stlSnprintf(sBuf, sizeof(sBuf), "%lx", sVar);
    STL_TRY(stlStrcmp("d59f80", sBuf) == 0);

    return STL_TRUE;

    STL_FINISH;
    
    return STL_FALSE;
}

/**
 * @brief stlSnprintf() 함수 테스트
 */
stlBool TestMoreInt64Fmts()
{
    stlChar   sBuf[100];
    stlInt64  sVal   = STL_INT64_C(-42);
    stlInt64  sBig   = STL_INT64_C(-314159265358979323);
    stlUInt64 sUval = STL_UINT64_C(42);
    stlUInt64 sUBig = STL_UINT64_C(10267677267010969076);

    stlSnprintf(sBuf, sizeof(sBuf), "%ld", sVal);
    STL_TRY(stlStrcmp("-42", sBuf) == 0);

    stlSnprintf(sBuf, sizeof(sBuf), "%lu", sUval);
    STL_TRY(stlStrcmp("42", sBuf) == 0);

    stlSnprintf(sBuf, sizeof(sBuf), "%lu", sUBig);
    STL_TRY(stlStrcmp("10267677267010969076", sBuf) == 0);

    stlSnprintf(sBuf, sizeof(sBuf), "%ld", sBig);
    STL_TRY(stlStrcmp("-314159265358979323", sBuf) == 0);

    return STL_TRUE;

    STL_FINISH;
    
    return STL_FALSE;
}

stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test SsizeFmt
     */
    sState = 1;
    STL_TRY(TestSsizeFmt() == STL_TRUE);

    /*
     * Test SizeFmt
     */
    sState = 2;
    STL_TRY(TestSizeFmt() == STL_TRUE);

    /*
     * Test OffFmt
     */
    sState = 3;
    STL_TRY(TestOffFmt() == STL_TRUE);

    /*
     * Test PidFmt
     */
    sState = 4;
    STL_TRY(TestPidFmt() == STL_TRUE);

    /*
     * Test Int64Fmt
     */
    sState = 5;
    STL_TRY(TestInt64Fmt() == STL_TRUE);

    /*
     * Test UInt64Fmt
     */
    sState = 6;
    STL_TRY(TestUInt64Fmt() == STL_TRUE);

    /*
     * Test UInt64HexFmt
     */
    sState = 7;
    STL_TRY(TestUInt64HexFmt() == STL_TRUE);

    /*
     * Test MoreInt64Fmts
     */
    sState = 8;
    STL_TRY(TestMoreInt64Fmts() == STL_TRUE);

    printf("PASS");

    return (0);

    STL_FINISH;

    printf("FAILURE %d", sState);

    return (-1);
}
