#include <stdio.h>
#include <stdarg.h>
#include <stl.h>

/*
 * Format Argument를 String으로 누적
 */
void FormatArg(stlChar *aStr, stlChar aOption, const stlChar *aArg)
{
    stlChar         sStr[8196];
    memset(sStr, 0x00, sizeof(sStr) );

    if (aArg != NULL)
    {
        stlSnprintf(sStr, 8196, "%soption: %c with %s\n", aStr, aOption, aArg);
    }
    else
    {
        stlSnprintf(sStr, 8196, "%soption: %c\n", aStr, aOption);
    }
    stlStrcpy( aStr, sStr );
}

/**
 * @brief stlGetOption() 함수 테스트 
 */
stlBool TestNoOptionsFound()
{
    // stlStatus       sStatus;
    stlErrorStack   sErrStack;

    stlChar         sStr[8196];
    
    stlGetOpt       sOption;
    stlChar         sOptCh;
    const stlChar * sOptArg;
    
    stlInt32              sArgc = 5;
    const stlChar * const sArgv[] = {"testlrog", "-a", "-b", "-c", "-d"};
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    sStr[0] = '\0';
    
    STL_TRY( stlInitGetOption( &sOption,
                               sArgc,
                               sArgv,
                               &sErrStack )
             == STL_SUCCESS);
   
    while ( stlGetOption( &sOption,
                          "abcd",
                          &sOptCh,
                          &sOptArg,
                          &sErrStack ) == STL_SUCCESS )
    {
        switch (sOptCh)
        {
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            default:
                FormatArg( sStr, sOptCh, sOptArg );
        }
    }

    STL_TRY( strcmp( "option: a\n"
                     "option: b\n"
                     "option: c\n"
                     "option: d\n", sStr) == 0 );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlGetOption() 함수 테스트 
 */
stlBool TestNoOptions()
{
    // stlStatus       sStatus;
    stlErrorStack   sErrStack;

    stlChar         sStr[8196];
    
    stlGetOpt       sOption;
    stlChar         sOptCh;
    const stlChar * sOptArg;
    
    stlInt32              sArgc = 5;
    const stlChar * const sArgv[] = {"testlrog", "-a", "-b", "-c", "-d"};

    STL_INIT_ERROR_STACK( &sErrStack );
    
    sStr[0] = '\0';
    STL_TRY( stlInitGetOption( &sOption,
                               sArgc,
                               sArgv,
                               &sErrStack )
             == STL_SUCCESS);
   
    /*
     * 부합하는 옵션이 없는 경우를 테스트
     */

    strcpy( sStr, "illegal option" );
    while ( stlGetOption( &sOption,
                          "efgh",
                          &sOptCh,
                          &sOptArg,
                          &sErrStack ) == STL_SUCCESS )
    {
        switch (sOptCh)
        {
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            default:
                FormatArg( sStr, sOptCh, sOptArg );
                break;
        }
    }

    STL_TRY( strcmp("illegal option", sStr) == 0 );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlGetOption() 함수 테스트 
 */
stlBool TestRequiredOption()
{
    // stlStatus       sStatus;
    stlErrorStack   sErrStack;

    stlChar         sStr[8196];
    
    stlGetOpt       sOption;
    stlChar         sOptCh;
    const stlChar * sOptArg;
    
    stlInt32              sArgc = 3;
    const stlChar * const sArgv[] = {"testlrog", "-a", "foo"};

    STL_INIT_ERROR_STACK( &sErrStack );
    
    sStr[0] = '\0';
    STL_TRY( stlInitGetOption( &sOption,
                               sArgc,
                               sArgv,
                               &sErrStack )
             == STL_SUCCESS);
   
    /*
     * 옵션이 인자를 필요로 하는 경우에 대한 테스트
     */
    while ( stlGetOption( &sOption,
                          "a:",
                          &sOptCh,
                          &sOptArg,
                          &sErrStack ) == STL_SUCCESS )
    {
        switch (sOptCh)
        {
            case 'a':
            case 'b':
            case 'c':
            case 'd':
                {
                    FormatArg( sStr, sOptCh, sOptArg );
                    break;
                }
            default:
                {
                    stlSnprintf( sStr, 8196, "illegal option" );
                    break;
                }
        }
    }

    STL_TRY( strcmp("option: a with foo\n", sStr) == 0 );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}


/**
 * @brief stlGetOption() 함수 테스트 
 */
stlBool TestOptionalOption()
{
    // stlStatus       sStatus;
    stlErrorStack   sErrStack;

    stlChar         sStr[8196];
    
    stlGetOpt       sOption;
    stlChar         sOptCh;
    const stlChar * sOptArg;

    stlInt32              sArgc   = 3;
    const stlChar * const sArgv[] = {"testlrog", "-a", "foo"};

    STL_INIT_ERROR_STACK( &sErrStack );
    
    sStr[0] = '\0';
    STL_TRY( stlInitGetOption( &sOption,
                               sArgc,
                               sArgv,
                               &sErrStack )
             == STL_SUCCESS);

    /*
     * Optional Option에 대한 테스트
     */
    while ( stlGetOption( &sOption,
                          "a::",
                          &sOptCh,
                          &sOptArg,
                          &sErrStack ) == STL_SUCCESS )
    {
        switch (sOptCh)
        {
            case 'a':
                FormatArg( sStr, sOptCh, sOptArg );
            default:
                break;
        }
    }

    STL_TRY( strcmp("option: a with foo\n", sStr) == 0 );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlGetOption() 함수 테스트 
 */
stlBool TestOptionalOptionNotGiven()
{
    // stlStatus       sStatus;
    stlErrorStack   sErrStack;

    stlChar         sStr[8196];
    
    stlGetOpt       sOption;
    stlChar         sOptCh;
    const stlChar * sOptArg;

    stlInt32              sArgc = 2;
    const stlChar * const sArgv[] = {"testlrog", "-a"};

    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlInitGetOption( &sOption,
                               sArgc,
                               sArgv,
                               &sErrStack )
             == STL_SUCCESS);

    strcpy( sStr, "illegal option" );
    while ( stlGetOption( &sOption,
                          "a::",
                          &sOptCh,
                          &sOptArg,
                          &sErrStack ) == STL_SUCCESS)
    {
        switch (sOptCh)
        {
            case 'a':
                {
                    if ( sOptArg != NULL)
                    {
                        FormatArg( sStr, sOptCh, sOptArg );
                    }
                    else
                    {
                        stlSnprintf( sStr, 8196, "illegal option" );
                    }
                    break;
                }
            default:
                break;
        }
    }

    STL_TRY( strcmp("illegal option", sStr) == 0 );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test NoOptionsFound
     */
    sState = 1;
    STL_TRY(TestNoOptionsFound() == STL_TRUE);

    /*
     * Test NoOptions
     */
    sState = 2;
    STL_TRY(TestNoOptions() == STL_TRUE);

    /*
     * Test RequiredOption
     */
    sState = 3;
    STL_TRY(TestRequiredOption() == STL_TRUE);
    
    /*
     * Test OptionalOption
     */
    sState = 5;
    STL_TRY(TestOptionalOption() == STL_TRUE);

    stlPrintf("PASS");

    return 0;

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
