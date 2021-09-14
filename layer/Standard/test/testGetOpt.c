#include <stl.h>

void FormatArg( stlChar       * aStr,
                stlChar         aOption,
                const stlChar * aArg )
{
    if( aArg != NULL )
    {
        stlSnprintf(aStr, 8196, "%soption: %c with %s\n", aStr, aOption, aArg);
    }
    else
    {
        stlSnprintf(aStr, 8196, "%soption: %c\n", aStr, aOption);
    }
}

stlBool TestNoOptionsFound()
{
    stlInt32   sArgc = 5;
    stlChar    sCh;
    stlGetOpt  sOpt;
    stlChar    sStr[8196];
    const stlChar *sOptArg;
    const stlChar * const sArgv[] = {"testprog", "-a", "-b", "-c", "-d"};
    stlErrorStack sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY(stlInitGetOption(&sOpt,
                             sArgc,
                             sArgv,
                             &sErrorStack)
            == STL_SUCCESS);

    while (1)
    {
        sStr[0] = '\0';

        if (stlGetOption(&sOpt, "abcd", &sCh, &sOptArg, &sErrorStack) != STL_SUCCESS)
        {
            STL_TRY(stlGetLastErrorCode(&sErrorStack) == STL_ERRCODE_EOF);
            break;
        }

        switch (sCh)
        {
            case 'a':
                FormatArg(sStr, sCh, sOptArg);
                STL_TRY(strcmp("option: a\n", sStr) == 0);
                break;
            case 'b':
                FormatArg(sStr, sCh, sOptArg);
                STL_TRY(strcmp("option: b\n", sStr) == 0);
                break;
            case 'c':
                FormatArg(sStr, sCh, sOptArg);
                STL_TRY(strcmp("option: c\n", sStr) == 0);
                break;
            case 'd':
                FormatArg(sStr, sCh, sOptArg);
                STL_TRY(strcmp("option: d\n", sStr) == 0);
                break;
            default:
                STL_TRY(0);
                break;
        }
    }

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

stlBool TestRequiredOption()
{
    stlInt32   sArgc = 3;
    stlChar    sCh;
    stlGetOpt  sOpt;
    stlChar    sStr[8196];
    stlErrorStack sErrorStack;
    const stlChar * sOptArg;
    const stlChar * const sArgv[] = {"testprog", "-a", "foo"};

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY(stlInitGetOption(&sOpt,
                             sArgc,
                             sArgv,
                             &sErrorStack)
            == STL_SUCCESS);

    while (1)
    {
        sStr[0] = '\0';

        if (stlGetOption(&sOpt,  "a:", &sCh, &sOptArg, &sErrorStack) != STL_SUCCESS)
        {
            STL_TRY(stlGetLastErrorCode(&sErrorStack) == STL_ERRCODE_EOF);
            break;
        }

        switch (sCh)
        {
            case 'a':
                FormatArg(sStr, sCh, sOptArg);
                STL_TRY(strcmp("option: a with foo\n", sStr) == 0);
                break;
            default:
                STL_TRY(0);
                break;
        }
    }

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

int main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;
    
    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test NoOptionsFound
     */
    sState = 1;
    STL_TRY(TestNoOptionsFound() == STL_TRUE);

    /*
     * Test RequiredOption
     */
    sState = 2;
    STL_TRY(TestRequiredOption() == STL_TRUE);

    stlPrintf("PASS");

    return 0;
    
    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return -1;
}
