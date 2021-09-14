#include <stdio.h>
#include <stl.h>

/**
 * @brief stlStrcmp() 함수 테스트
 */
stlBool TestLess0()
{
    stlInt32 sRet = stlStrcmp("a", "b");
    STL_TRY(sRet < 0);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlStrcmp() 함수 테스트
 */
stlBool TestStrEqual()
{
    stlInt32 sRet = stlStrcmp("a", "a");
    STL_TRY(sRet == 0);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlStrcmp() 함수 테스트
 */
stlBool TestMore0()
{
    stlInt32 sRet = stlStrcmp("b", "a");
    STL_TRY(sRet > 0);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlStrcasecmp() 함수 테스트
 */
stlBool TestLessIgnoreCase()
{
    stlInt32 sRet = stlStrcasecmp("a", "B");
    STL_TRY(sRet < 0);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlStrcasecmp() 함수 테스트
 */
stlBool TestStrEqualIgnoreCase()
{
    stlInt32 sRet = stlStrcasecmp("a", "A");
    STL_TRY(sRet == 0);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlStrcasecmp() 함수 테스트
 */
stlBool TestMoreIgnoreCase()
{
    stlInt32 sRet = stlStrcasecmp("b", "A");
    STL_TRY(sRet > 0);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlStrcasecmp() 함수 테스트
 */
stlBool TestCmp()
{
    stlInt32 sRet = stlStrcasecmp("a2", "a10");
    STL_TRY(sRet > 0);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test Less0
     */
    sState = 1;
    STL_TRY(TestLess0() == STL_TRUE);

    /*
     * Test StrEqual
     */
    sState = 2;
    STL_TRY(TestStrEqual() == STL_TRUE);

    /*
     * Test More0
     */
    sState = 3;
    STL_TRY(TestMore0() == STL_TRUE);

    /*
     * Test LessIgnoreCase
     */
    sState = 4;
    STL_TRY(TestLessIgnoreCase() == STL_TRUE);

    /*
     * Test StrEqualIgnoreCase
     */
    sState = 5;
    STL_TRY(TestStrEqualIgnoreCase() == STL_TRUE);

    /*
     * Test MoreIgnoreCase
     */
    sState = 6;
    STL_TRY(TestMoreIgnoreCase() == STL_TRUE);

    /*
     * Test Cmp
     */
    sState = 7;
    STL_TRY(TestCmp() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
