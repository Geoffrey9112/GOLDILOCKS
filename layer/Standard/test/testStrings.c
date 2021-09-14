#include <stl.h>

#define TEST_BUFFER_SIZE 4

stlErrorStack gErrorStack;

#define PRINTPASS stlPrintf( "  => PASS\n" );
#define PRINTFAIL stlPrintf( "  => FAIL\n" );
#define PRINTERR( aErrorStack )                             \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( aErrorStack ),      \
                   stlGetLastErrorMessage( aErrorStack ) ); \
    } while(0)

#define TEST_VERIFY( aExpression )                                  \
    do                                                              \
    {                                                               \
        if( !(aExpression) )                                        \
        {                                                           \
            stlPrintf( "ERRORIGIN: %s(%d)\n", __FILE__, __LINE__ ); \
            goto STL_FINISH_LABEL;                                  \
        }                                                           \
    } while( 0 )

stlStatus test_stlStrlen()
{
    TEST_VERIFY( stlStrlen( "This is a test" ) == 14 );
    TEST_VERIFY( stlStrlen( "" ) == 0 );
    
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlStrcpy()
{
    stlChar sBuffer[15];
    
    stlMemset( sBuffer, 'A', 15 );
    stlStrcpy( sBuffer, "This is a test" );
    TEST_VERIFY( stlStrlen( sBuffer ) == 14 );
    
    stlStrcpy( sBuffer, "" );
    TEST_VERIFY( stlStrlen( sBuffer ) == 0 );
    
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlStrncpy()
{
    stlChar sBuffer[14];

    stlStrncpy( sBuffer, "This is a test", 14 );
    TEST_VERIFY( stlStrlen( sBuffer ) == 13 );
    
    stlStrncpy( sBuffer, "", 14 );
    TEST_VERIFY( stlStrlen( sBuffer ) == 0 );
    
    stlStrncpy( sBuffer, "AAAAAAAAAAAAA", 14 );
    stlStrncpy( sBuffer, "", 0 );
    TEST_VERIFY( stlStrlen( sBuffer ) == 13 );
    
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlStrchr()
{
    stlChar   sBuffer[15];
    stlChar * sTarget;

    stlStrncpy( sBuffer, "This is a test", 15 );
    sTarget = stlStrchr( sBuffer, 'a' );
    TEST_VERIFY( (sTarget - sBuffer) == 8 );
    
    sTarget = stlStrchr( sBuffer, 'z' );
    TEST_VERIFY( sTarget == NULL );
    
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlStrrchr()
{
    stlChar   sBuffer[15];
    stlChar * sTarget;

    stlStrncpy( sBuffer, "This is a test", 15 );
    sTarget = stlStrrchr( sBuffer, 'i' );
    TEST_VERIFY( (sTarget - sBuffer) == 5 );
    
    sTarget = stlStrrchr( sBuffer, 'z' );
    TEST_VERIFY( sTarget == NULL );
    
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlStrcmp()
{
    stlChar   sBuffer[15];
    stlChar   sBuffer2[15];

    stlStrcpy( sBuffer, "This is a test" );
    stlStrcpy( sBuffer2, "This is a test" );
    TEST_VERIFY( stlStrcmp( sBuffer, sBuffer2 ) == 0 );
    
    stlStrcpy( sBuffer, "This is a tesa" );
    stlStrcpy( sBuffer2, "This is a test" );
    TEST_VERIFY( stlStrcmp( sBuffer, sBuffer2 ) < 0 );
    
    stlStrcpy( sBuffer, "This is" );
    stlStrcpy( sBuffer2, "This is a test" );
    TEST_VERIFY( stlStrcmp( sBuffer, sBuffer2 ) < 0 );
    
    stlStrcpy( sBuffer, "This is a test" );
    stlStrcpy( sBuffer2, "This is a tesz" );
    TEST_VERIFY( stlStrcmp( sBuffer, sBuffer2 ) < 0 );
    
    stlStrcpy( sBuffer, "This is a test" );
    stlStrcpy( sBuffer2, "This is" );
    TEST_VERIFY( stlStrcmp( sBuffer, sBuffer2 ) >= 0 );
    
    stlStrcpy( sBuffer, "" );
    stlStrcpy( sBuffer2, "" );
    TEST_VERIFY( stlStrcmp( sBuffer, sBuffer2 ) == 0 );
    
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlStrncmp()
{
    stlChar   sBuffer[15];
    stlChar   sBuffer2[15];

    stlStrcpy( sBuffer, "This is a test" );
    stlStrcpy( sBuffer2, "This is a test" );
    TEST_VERIFY( stlStrncmp( sBuffer, sBuffer2, 14 ) == 0 );
    
    stlStrcpy( sBuffer, "This is a tesa" );
    stlStrcpy( sBuffer2, "This is a test" );
    TEST_VERIFY( stlStrncmp( sBuffer, sBuffer2, 14 ) < 0 );
    
    stlStrcpy( sBuffer, "This is" );
    stlStrcpy( sBuffer2, "This is a test" );
    TEST_VERIFY( stlStrncmp( sBuffer, sBuffer2, 7 ) == 0 );
    
    stlStrcpy( sBuffer, "This is" );
    stlStrcpy( sBuffer2, "This is a test" );
    TEST_VERIFY( stlStrncmp( sBuffer, sBuffer2, 8 ) < 0 );
    
    stlStrcpy( sBuffer, "This is a test" );
    stlStrcpy( sBuffer2, "This is" );
    TEST_VERIFY( stlStrncmp( sBuffer, sBuffer2, 15 ) > 0 );
    
    stlStrcpy( sBuffer, "" );
    stlStrcpy( sBuffer2, "" );
    TEST_VERIFY( stlStrncmp( sBuffer, sBuffer2, 15 ) == 0 );
    
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlStrcasecmp()
{
    stlChar   sBuffer[15];
    stlChar   sBuffer2[15];

    stlStrcpy( sBuffer, "tHIS IS A TEST" );
    stlStrcpy( sBuffer2, "This is a test" );
    TEST_VERIFY( stlStrcasecmp( sBuffer, sBuffer2 ) == 0 );
    
    stlStrcpy( sBuffer, "tHIS IS A TESA" );
    stlStrcpy( sBuffer2, "This is a test" );
    TEST_VERIFY( stlStrcasecmp( sBuffer, sBuffer2 ) < 0 );
    
    stlStrcpy( sBuffer, "tHIS IS" );
    stlStrcpy( sBuffer2, "This is a test" );
    TEST_VERIFY( stlStrcasecmp( sBuffer, sBuffer2 ) < 0 );
    
    stlStrcpy( sBuffer, "tHIS IS A TEST" );
    stlStrcpy( sBuffer2, "This is a tesz" );
    TEST_VERIFY( stlStrcasecmp( sBuffer, sBuffer2 ) < 0 );
    
    stlStrcpy( sBuffer, "tHIS IS A TEST" );
    stlStrcpy( sBuffer2, "This is" );
    TEST_VERIFY( stlStrcasecmp( sBuffer, sBuffer2 ) > 0 );
    
    stlStrcpy( sBuffer, "" );
    stlStrcpy( sBuffer2, "" );
    TEST_VERIFY( stlStrcasecmp( sBuffer, sBuffer2 ) == 0 );
    
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlStrncasecmp()
{
    stlChar   sBuffer[15];
    stlChar   sBuffer2[15];

    stlStrcpy( sBuffer, "tHIS IS A TEST" );
    stlStrcpy( sBuffer2, "This is a test" );
    TEST_VERIFY( stlStrncasecmp( sBuffer, sBuffer2, 14 ) == 0 );
    
    stlStrcpy( sBuffer, "tHIS IS A TESA" );
    stlStrcpy( sBuffer2, "This is a test" );
    TEST_VERIFY( stlStrncasecmp( sBuffer, sBuffer2, 14 ) < 0 );
    
    stlStrcpy( sBuffer, "tHIS IS" );
    stlStrcpy( sBuffer2, "This is a test" );
    TEST_VERIFY( stlStrncasecmp( sBuffer, sBuffer2, 7 ) == 0 );
    
    stlStrcpy( sBuffer, "tHIS IS" );
    stlStrcpy( sBuffer2, "This is a test" );
    TEST_VERIFY( stlStrncasecmp( sBuffer, sBuffer2, 8 ) < 0 );
    
    stlStrcpy( sBuffer, "tHIS IS A TEST" );
    stlStrcpy( sBuffer2, "This is" );
    TEST_VERIFY( stlStrncasecmp( sBuffer, sBuffer2, 15 ) > 0 );
    
    stlStrcpy( sBuffer, "" );
    stlStrcpy( sBuffer2, "" );
    TEST_VERIFY( stlStrncasecmp( sBuffer, sBuffer2, 15 ) == 0 );
    
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlStrstr()
{
    stlChar   sBuffer[15];
    stlChar   sBuffer2[15];
    stlChar * sTarget;

    stlStrcpy( sBuffer, "This is a test" );
    stlStrcpy( sBuffer2, "is" );
    sTarget = stlStrstr( sBuffer, sBuffer2 );
    TEST_VERIFY( (sTarget - sBuffer) == 2 );
    
    stlStrcpy( sBuffer, "This is a test" );
    stlStrcpy( sBuffer2, "si" );
    sTarget = stlStrstr( sBuffer, sBuffer2 );
    TEST_VERIFY( sTarget == NULL );
    
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlStrtok()
{
    stlChar    sSrc[] = "a/bbb///cc;xxx:yyy:";
    stlChar    sDelim1[] = ":;";
    stlChar    sDelim2[] = "/";
    stlChar   *sToken;
    stlChar   *sSubToken;
    stlChar   *sSavePtr1;
    stlChar   *sSavePtr2;

    sToken = stlStrtok(sSrc, sDelim1, &sSavePtr1);
    TEST_VERIFY(stlStrcmp(sToken, "a/bbb///cc") == 0);

    sSubToken = stlStrtok(sToken, sDelim2, &sSavePtr2);
    TEST_VERIFY(stlStrcmp(sSubToken, "a") == 0);

    sSubToken = stlStrtok(NULL, sDelim2, &sSavePtr2);
    TEST_VERIFY(stlStrcmp(sSubToken, "bbb") == 0);

    sSubToken = stlStrtok(NULL, sDelim2, &sSavePtr2);
    TEST_VERIFY(stlStrcmp(sSubToken, "cc") == 0);

    sToken = stlStrtok(NULL, sDelim1, &sSavePtr1);
    TEST_VERIFY(stlStrcmp(sToken, "xxx") == 0);

    sToken = stlStrtok(NULL, sDelim1, &sSavePtr1);
    TEST_VERIFY(stlStrcmp(sToken, "yyy") == 0);
    
    PRINTPASS;

    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlStrcat()
{
    stlChar sBuffer[30];
    
    stlStrcpy(sBuffer, "This is a test!");
    stlStrcat(sBuffer, "This is a test");
    TEST_VERIFY(stlStrlen(sBuffer) == 29);
    TEST_VERIFY(stlStrcmp(sBuffer, "This is a test!This is a test") == 0);
    
    stlStrcpy(sBuffer, "");
    stlStrcat(sBuffer, "");
    TEST_VERIFY(stlStrlen(sBuffer) == 0);
    
    PRINTPASS;

    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlStrncat()
{
    stlChar sBuffer[30];

    stlStrcpy(sBuffer, "This is a test!");
    stlStrncat(sBuffer, "This is a test", 14);
    TEST_VERIFY(stlStrlen(sBuffer) == 28);
    TEST_VERIFY(stlStrcmp(sBuffer, "This is a test!This is a tes") == 0);
    
    stlStrcpy(sBuffer, "This is a test!");
    stlStrncat(sBuffer, "", 14);
    TEST_VERIFY(stlStrlen(sBuffer) == 15);
    
    stlStrcpy(sBuffer, "This is a test!");
    stlStrncat(sBuffer, "", 0);
    stlPrintf("%s\n", sBuffer);
    TEST_VERIFY(stlStrlen(sBuffer) == 15);
    TEST_VERIFY(stlStrcmp(sBuffer, "This is a test!") == 0);
    
    PRINTPASS;

    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlPrintf()
{
    stlChar     sBuffer[15];
    stlInt32    sCount;
    stlInt32    sNumber32;
    stlInt64    sNumber64;
    stlFloat32  sFloat32;
    stlFloat64  sFloat64;
    
    stlStrncpy( sBuffer, "This is a test", 15 );
    sCount = stlPrintf( "%s", sBuffer );
    TEST_VERIFY( sCount == 14 );
    
    sCount = stlPrintf( "%s", "" );
    TEST_VERIFY( sCount == 0 );
    
    sCount = stlPrintf( "\n" );
    TEST_VERIFY( sCount == 1 );

    sNumber32 = 123456789;
    sNumber64 = 12345678912345;
    sFloat32 = 123456.123;
    sFloat64 = 123456789123.123;
    sCount = stlPrintf( "%d", -sNumber32 );
    TEST_VERIFY( sCount == 10 );
    stlPrintf( "\n" );
    sCount = stlPrintf( "%d", sNumber32 );
    TEST_VERIFY( sCount == 9 );
    stlPrintf( "\n" );
    sCount = stlPrintf( "%ld", -sNumber64 );
    TEST_VERIFY( sCount == 15 );
    stlPrintf( "\n" );
    sCount = stlPrintf( "%ld", sNumber64 );
    TEST_VERIFY( sCount == 14 );
    stlPrintf( "\n" );
    sCount = stlPrintf( "%i", sNumber32 );
    TEST_VERIFY( sCount == 9 );
    stlPrintf( "\n" );
    sCount = stlPrintf( "%o", sNumber32 );
    TEST_VERIFY( sCount == 9 );
    stlPrintf( "\n" );
    sCount = stlPrintf( "%x", sNumber32 );
    TEST_VERIFY( sCount == 7 );
    stlPrintf( "\n" );
    sCount = stlPrintf( "%X", sNumber32 );
    TEST_VERIFY( sCount == 7 );
    stlPrintf( "\n" );
    sCount = stlPrintf( "%.3f", sFloat32 );
    TEST_VERIFY( sCount == 10 );
    stlPrintf( "\n" );
    sCount = stlPrintf( "%.3lf", sFloat64 );
    TEST_VERIFY( sCount == 16 );
    stlPrintf( "\n" );
    
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlSnprintf()
{
    stlChar   sBuffer[15];
    stlChar   sBuffer2[30];
    stlInt32  sCount;
    stlInt32  sCount2;
    stlInt64  sNumber;

    /* Test format string type specifier */
    /* d, i, o, u, x, X, e, E, f, F, g, G, c, s, p, n, % */

    sNumber = 123456789;
    sCount = stlSnprintf( sBuffer, 15, "%d", (stlInt32)-sNumber);
    TEST_VERIFY( sCount == 10 );
    TEST_VERIFY( sNumber + strtol(sBuffer, NULL, 10) == 0);
    sCount = stlSnprintf( sBuffer, 15, "%d", (stlUInt32)sNumber);
    TEST_VERIFY( sCount == 9 );
    TEST_VERIFY( sNumber - strtol(sBuffer, NULL, 10) == 0);
    sCount = stlSnprintf( sBuffer, 15, "%ld", (stlInt64)-sNumber);
    TEST_VERIFY( sCount == 10 );
    TEST_VERIFY( sNumber + strtol(sBuffer, NULL, 10) == 0);
    sCount = stlSnprintf( sBuffer, 15, "%lu", (stlUInt64)sNumber);
    TEST_VERIFY( sCount == 9 );
    TEST_VERIFY( sNumber - strtol(sBuffer, NULL, 10) == 0);
    sCount = stlSnprintf( sBuffer, 15, "%i", (stlInt32)sNumber);
    TEST_VERIFY( sCount == 9 );
    TEST_VERIFY( sNumber - strtol(sBuffer, NULL, 10) == 0);
    sCount = stlSnprintf( sBuffer, 15, "%o", (stlInt32)sNumber);
    TEST_VERIFY( sCount == 9 );
    TEST_VERIFY( sNumber - strtol(sBuffer, NULL, 8) == 0);
    sCount = stlSnprintf( sBuffer, 15, "%x", (stlInt32)sNumber);
    TEST_VERIFY( sCount == 7 );
    TEST_VERIFY( sNumber - strtol(sBuffer, NULL, 16) == 0);
    sCount = stlSnprintf( sBuffer, 15, "%X", (stlInt32)sNumber);
    TEST_VERIFY( sCount == 7 );
    TEST_VERIFY( sNumber - strtol(sBuffer, NULL, 16) == 0);

    sCount = stlSnprintf( sBuffer, 15, "%.2lf", (double)0.00001 );
    TEST_VERIFY( stlStrcmp(sBuffer, "0.00") == 0 );
    TEST_VERIFY( sCount == 4 );

    sCount = stlSnprintf( sBuffer, 15, "%.2lf", (double)0.00001 );
    TEST_VERIFY( stlStrcmp(sBuffer, "0.00") == 0 );
    TEST_VERIFY( sCount == 4 );

    sCount = stlSnprintf( sBuffer, 15, "%.2e", (double)0.00001 );
    TEST_VERIFY( stlStrcmp(sBuffer, "1.00e-05") == 0 );
    TEST_VERIFY( sCount == 8 );

    sCount = stlSnprintf( sBuffer, 15, "%.2E", (double)0.00001 );
    TEST_VERIFY( stlStrcmp(sBuffer, "1.00E-05") == 0 );
    TEST_VERIFY( sCount == 8 );

    sCount = stlSnprintf( sBuffer, 15, "%g", (double)0.0000123 );
    TEST_VERIFY( stlStrcmp(sBuffer, "1.23e-5") == 0 );
    TEST_VERIFY( sCount == 7 );

    sCount = stlSnprintf( sBuffer, 15, "%G", (double)0.0000123 );
    TEST_VERIFY( stlStrcmp(sBuffer, "1.23E-5") == 0 );
    TEST_VERIFY( sCount == 7 );

    sCount = stlSnprintf( sBuffer, 15, "%c", (stlChar)'A' );
    TEST_VERIFY( stlStrcmp(sBuffer, "A") == 0 );
    TEST_VERIFY( sCount == 1 );

    sCount = stlSnprintf( sBuffer, 15, "%p", NULL );
    TEST_VERIFY( stlStrcmp(sBuffer, "0") == 0 );
    TEST_VERIFY( sCount == 1 );

    stlStrncpy( sBuffer, "This is a test", 15 );
    sCount = stlSnprintf( sBuffer2, 20, "%s%n", sBuffer, &sCount2 );
    TEST_VERIFY( stlStrcmp(sBuffer2, "This is a test") == 0 );
    TEST_VERIFY( sCount == 14 );
    TEST_VERIFY( sCount2 == sCount );
    
    stlStrncpy( sBuffer, "This is a test", 15 );
    sCount = stlSnprintf( sBuffer2, 15, "%s", sBuffer );
    TEST_VERIFY( sCount == 14 );
    
    sCount = stlSnprintf( sBuffer2, 11, "%s", sBuffer );
    TEST_VERIFY( sCount == 10 );
    
    sCount = stlSnprintf( sBuffer2, 0, "%s", sBuffer );
    TEST_VERIFY( sCount == 0 );
    TEST_VERIFY( stlStrlen( sBuffer2 ) == 10 );
    
    sCount = stlGetPrintfBufferSize( "%s", sBuffer );
    TEST_VERIFY( sCount == 14 );
    
    sCount = stlSnprintf( sBuffer2, 15, "" );
    TEST_VERIFY( sCount == 0 );
    
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlVprintf()
{
    TEST_VERIFY(test_stlPrintf() == STL_SUCCESS);

    PRINTPASS;

    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlVsnprintf()
{
    TEST_VERIFY(test_stlSnprintf() == STL_SUCCESS);

    PRINTPASS;

    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlGetPrintfBufferSize()
{
    stlInt32  sCount;

    sCount = stlGetPrintfBufferSize("%4.4lf, %s", 1234.56789, "This is a test!");
    TEST_VERIFY(sCount == 26);

    PRINTPASS;

    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlStrToFloat64()
{
    stlStatus      sStatus;
    stlChar        sBuffer1[15];
    stlChar       *sEndPtr;
    stlFloat64     sResult;
    stlErrorStack  sErrorStack;

    stlStrcpy(sBuffer1, "1234.5678");
    sStatus = stlStrToFloat64(sBuffer1, &sEndPtr, &sResult, &sErrorStack);
    TEST_VERIFY(sStatus == STL_SUCCESS);

    PRINTPASS;

    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlStrToInt64()
{
    stlStatus      sStatus;
    stlChar        sBuffer1[15];
    stlChar       *sEndPtr;
    stlInt64       sResult;
    stlErrorStack  sErrorStack;

    stlStrcpy(sBuffer1, "12345678");
    sStatus = stlStrToInt64(sBuffer1, &sEndPtr, 10, &sResult, &sErrorStack);
    TEST_VERIFY(sStatus == STL_SUCCESS);

    PRINTPASS;

    return STL_SUCCESS;

    STL_FINISH;

    PRINTFAIL;

    return STL_FAILURE;
}

stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlInt32 sState = 0;
    
    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_INIT_ERROR_STACK( &gErrorStack );

    stlPrintf( "1. Test stlStrlen\n" );
    (void)test_stlStrlen();
    
    stlPrintf( "2. Test stlStrcpy\n" );
    (void)test_stlStrcpy();
    
    stlPrintf( "3. Test stlStrncpy\n" );
    (void)test_stlStrncpy();
    
    stlPrintf( "4. Test stlStrchr\n" );
    (void)test_stlStrchr();
    
    stlPrintf( "5. Test stlStrrchr\n" );
    (void)test_stlStrrchr();
    
    stlPrintf( "6. Test stlStrcmp\n" );
    (void)test_stlStrcmp();
    
    stlPrintf( "7. Test stlStrncmp\n" );
    (void)test_stlStrncmp();
    
    stlPrintf( "8. Test stlStrcasecmp\n" );
    (void)test_stlStrcasecmp();
    
    stlPrintf( "9. Test stlStrncasecmp\n" );
    (void)test_stlStrncasecmp();
    
    stlPrintf( "10. Test stlStrstr\n" );
    (void)test_stlStrstr();
    
    stlPrintf( "11. Test stlStrtok\n" );
    (void)test_stlStrtok();
    
    stlPrintf( "12. Test stlStrcat\n" );
    (void)test_stlStrcat();
    
    stlPrintf( "13. Test stlStrncat\n" );
    (void)test_stlStrncat();
    
    stlPrintf( "14. Test stlPrintf\n" );
    (void)test_stlPrintf();
    
    stlPrintf( "15. Test stlSnprintf\n" );
    (void)test_stlSnprintf();
    
    stlPrintf( "16. Test stlVprintf\n" );
    (void)test_stlVprintf();
    
    stlPrintf( "17. Test stlVsnprintf\n" );
    (void)test_stlVsnprintf();
    
    stlPrintf( "18. Test stlGetPrintfBufferSize\n" );
    (void)test_stlGetPrintfBufferSize();
    
    stlPrintf( "19. Test stlStrToFloat64\n" );
    (void)test_stlStrToFloat64();
    
    stlPrintf( "20. Test stlStrToInt64\n" );
    (void)test_stlStrToInt64();
    
    sState = 0;
    stlTerminate();

    return STL_SUCCESS;

    STL_FINISH;

    PRINTERR( &gErrorStack );
    
    switch( sState )
    {
        case 1:
            stlTerminate();
        default:
            break;
    }

    PRINTFAIL;

    return STL_FAILURE;
}
