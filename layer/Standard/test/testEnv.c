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

stlStatus testNormalEnv()
{
    stlChar  sValue[TEST_BUFFER_SIZE];
    stlBool  sIsFound;
    
    STL_INIT_ERROR_STACK( &gErrorStack );

    /*
     * Test Set Environment
     */
    stlPrintf("Test stlSetEnv:\n");
    stlStrncpy( sValue, "YES", TEST_BUFFER_SIZE );
    STL_TRY( stlSetEnv( "MKKIM_TEST_ENV",
                        sValue,
                        &gErrorStack ) == STL_SUCCESS );
    PRINTPASS;
    
    /*
     * Test Get Environment
     */
    stlPrintf("Test stlGetEnv:\n");
    STL_TRY( stlGetEnv( sValue,
                        TEST_BUFFER_SIZE,
                        "MKKIM_TEST_ENV",
                        &sIsFound,
                        &gErrorStack  ) == STL_SUCCESS );

    STL_TRY_THROW( stlStrcmp( sValue, "YES" ) == 0,
                   RAMP_ERR_COMPARE_ENV );
    PRINTPASS;
    
    /*
     * Test Delete Environment
     */
    stlPrintf("Test stlDelEnv:\n");
    STL_TRY( stlDelEnv( "MKKIM_TEST_ENV",
                        &gErrorStack ) == STL_SUCCESS );
    PRINTPASS;

    return STL_SUCCESS;

    STL_FINISH;

    STL_CATCH( RAMP_ERR_COMPARE_ENV )
    {
        stlPrintf( "does not matching environment variable" );
    }
    
    return STL_FAILURE;
}

stlStatus testAbnormalEnv()
{
    stlChar  sValue[TEST_BUFFER_SIZE];
    stlBool  sIsFound;
    
    /*
     * Test Set Environment twice
     */
    stlPrintf("Test stlSetEnv(twice):\n");
    stlStrncpy( sValue, "YES", TEST_BUFFER_SIZE );
    STL_TRY( stlSetEnv( "MKKIM_TEST_ENV",
                        sValue,
                        &gErrorStack ) == STL_SUCCESS );
    STL_TRY( stlSetEnv( "MKKIM_TEST_ENV",
                        sValue,
                        &gErrorStack ) == STL_SUCCESS );
    STL_TRY( stlDelEnv( "MKKIM_TEST_ENV",
                        &gErrorStack ) == STL_SUCCESS );
    PRINTPASS;
    
    stlPrintf("Test stlSetEnv(Invalid argument):\n");
    stlStrncpy( sValue, "YES", TEST_BUFFER_SIZE );
    STL_TRY( stlSetEnv( "MKKIM_TEST_ENV?+-@$%",
                        sValue,
                        &gErrorStack ) == STL_SUCCESS );
    STL_TRY( stlDelEnv( "MKKIM_TEST_ENV?+-@$%",
                        &gErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlSetEnv( "MKKIM_TEST_ENV=",
                        sValue,
                        &gErrorStack ) != STL_SUCCESS );
    (void)stlPopError( &gErrorStack );
    PRINTERR( &gErrorStack );
    
    STL_TRY( stlSetEnv( "",
                        sValue,
                        &gErrorStack ) != STL_SUCCESS );
    PRINTERR( &gErrorStack );
    
    STL_TRY( stlSetEnv( NULL,
                        sValue,
                        &gErrorStack ) != STL_SUCCESS );
    (void)stlPopError( &gErrorStack );
    PRINTERR( &gErrorStack );
    
    PRINTPASS;

    stlPrintf("Test stlGetEnv(No Entry):\n");
    STL_TRY( stlGetEnv( sValue,
                        TEST_BUFFER_SIZE,
                        "MKKIM_TEST_ENV",
                        &sIsFound,
                        &gErrorStack  ) != STL_SUCCESS );
    (void)stlPopError( &gErrorStack );
    PRINTERR( &gErrorStack );
    
    PRINTPASS;

    /*
     * stlDelEnv는 No Entry 에러가 발생하지 않는다.
     */
    stlPrintf("Test stlDelEnv(No Entry):\n");
    STL_TRY( stlDelEnv( "MKKIM_TEST_ENV",
                        &gErrorStack ) == STL_SUCCESS );
    
    PRINTPASS;
    
    stlPrintf("Test stlDelEnv(Invalid argument):\n");
    stlStrncpy( sValue, "YES", TEST_BUFFER_SIZE );
    STL_TRY( stlDelEnv( "MKKIM_TEST_ENV?+-@$%",
                        &gErrorStack ) == STL_SUCCESS );
    STL_TRY( stlDelEnv( "MKKIM_TEST_ENV=",
                        &gErrorStack ) != STL_SUCCESS );
    (void)stlPopError( &gErrorStack );
    PRINTERR( &gErrorStack );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlInt32 sState = 0;
    
    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_INIT_ERROR_STACK( &gErrorStack );

    /*
     * Test Normal Environment
     */
    stlPrintf("1. Test Normal Case\n");
    STL_TRY( testNormalEnv() == STL_SUCCESS );
    
    /*
     * Test Abnormal Environment
     */
    stlPrintf("2. Test Abnormal Case\n");
    STL_TRY( testAbnormalEnv() == STL_SUCCESS );
    
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
