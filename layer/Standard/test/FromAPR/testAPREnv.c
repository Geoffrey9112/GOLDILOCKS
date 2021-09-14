#include <stdio.h>
#include <stl.h>

#define TESTL_ENVVAR_NAME  "apr_test_envvar"
#define TESTL_ENVVAR2_NAME "apr_test_envvar2"
#define TESTL_ENVVAR_VALUE "Just a value that we'll check"

#define PRINTERR                                            \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( &sErrStack ),       \
                   stlGetLastErrorMessage( &sErrStack ) );  \
    } while(0)

/**
 * @brief stlSetEnv() 함수 테스트
 */
stlBool TestSetEnv()
{
    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlSetEnv( TESTL_ENVVAR_NAME,
                        TESTL_ENVVAR_VALUE,
                        &sErrStack )
             == STL_SUCCESS);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlGetEnv() 함수 테스트
 */
stlBool TestGetEnv()
{
    stlChar sValue[256];

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlGetEnv( sValue,
                        256,
                        TESTL_ENVVAR_NAME,
                        &sErrStack )
             == STL_SUCCESS);

    STL_TRY( strcmp(TESTL_ENVVAR_VALUE, sValue) == 0 );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlDelEnv() 함수 테스트
 */
stlBool TestDelEnv()
{
    stlChar sValue[256];

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlDelEnv( TESTL_ENVVAR_NAME,
                        &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlGetEnv( sValue,
                        256,
                        TESTL_ENVVAR_NAME,
                        &sErrStack )
             != STL_SUCCESS );
    STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_NO_ENTRY );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlDelEnv() 함수 테스트
 */
stlBool TestEmptyEnv()
{
    stlChar sValue[256];

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );


    STL_TRY( stlSetEnv( TESTL_ENVVAR_NAME,
                        "",
                        &sErrStack )
             == STL_SUCCESS );
    STL_TRY( stlGetEnv( sValue,
                        256,
                        TESTL_ENVVAR_NAME,
                        &sErrStack )
             == STL_SUCCESS );
    STL_TRY( strcmp("", sValue) == 0 );

    /** Delete and retest */
    STL_TRY( stlDelEnv( TESTL_ENVVAR_NAME,
                        &sErrStack )
             == STL_SUCCESS );
    STL_TRY( stlGetEnv( sValue,
                        256, 
                        TESTL_ENVVAR_NAME,
                        &sErrStack )
             != STL_SUCCESS );
    STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_NO_ENTRY );

    /** Set second variable + test*/
    STL_TRY( stlSetEnv( TESTL_ENVVAR2_NAME,
                        TESTL_ENVVAR_VALUE,
                        &sErrStack )
             == STL_SUCCESS);
    STL_TRY( stlGetEnv( sValue,
                        256,
                        TESTL_ENVVAR2_NAME,
                        &sErrStack )
             == STL_SUCCESS);
    STL_TRY( strcmp(TESTL_ENVVAR_VALUE, sValue) == 0 );

    /** Finally, test ENOENT (first variable) followed by second != ENOENT) */
    STL_TRY( stlGetEnv( sValue,
                        256,
                        TESTL_ENVVAR_NAME,
                        &sErrStack )
             != STL_SUCCESS );
    STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_NO_ENTRY );

    STL_TRY( stlGetEnv( sValue,
                        256,
                        TESTL_ENVVAR2_NAME,
                        &sErrStack )
             == STL_SUCCESS );
    STL_TRY( strcmp(TESTL_ENVVAR_VALUE, sValue) == 0 );
    
    /** Cleanup */
    STL_TRY( stlDelEnv( TESTL_ENVVAR2_NAME, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test SetEnv
     */
    sState = 1;
    STL_TRY(TestSetEnv() == STL_TRUE);

    /*
     * Test GetEnv
     */
    sState = 2;
    STL_TRY(TestGetEnv() == STL_TRUE);

    /*
     * Test DelEnv
     */
    sState = 3;
    STL_TRY(TestDelEnv() == STL_TRUE);

    /*
     * Test EmptyEnv
     */
    sState = 4;
    STL_TRY(TestEmptyEnv() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
