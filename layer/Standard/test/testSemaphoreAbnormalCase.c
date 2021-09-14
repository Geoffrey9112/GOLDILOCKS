#include <stl.h>

#define TEST_BUFFER_SIZE 6

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

stlSemaphore gSemaphore;

stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlInt32 sState = 0;
    stlChar  sSemName[STL_MAX_SEM_NAME+2];
    stlBool  sIsSuccess;
    stlBool  sIsTimedOut;
    
    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_INIT_ERROR_STACK( &gErrorStack );

    /*
     * Test create semaphore
     */
    stlPrintf("Test stlCreateSemaphore(name too long):\n");
    stlMemset( sSemName, 'A', STL_MAX_SEM_NAME+2 );
    sSemName[STL_MAX_SEM_NAME+1] = '\0';

    STL_TRY( stlCreateSemaphore( &gSemaphore,
                                 sSemName,
                                 1,
                                 &gErrorStack ) != STL_SUCCESS );
    PRINTERR( &gErrorStack );
    PRINTPASS;
    
    /*
     * Test try acquire semaphore
     */
    stlPrintf("Test stlTryAcquireSemaphore(twice):\n");
    stlStrncpy( sSemName, "__SEM", TEST_BUFFER_SIZE );

    STL_TRY( stlCreateSemaphore( &gSemaphore,
                                 sSemName,
                                 1,
                                 &gErrorStack ) == STL_SUCCESS );

    STL_TRY( stlTryAcquireSemaphore( &gSemaphore,
                                     &sIsSuccess,
                                     &gErrorStack ) == STL_SUCCESS );
    STL_TRY( sIsSuccess == STL_TRUE );
    STL_TRY( stlTryAcquireSemaphore( &gSemaphore,
                                     &sIsSuccess,
                                     &gErrorStack ) == STL_SUCCESS );
    STL_TRY( sIsSuccess == STL_FALSE );
    STL_TRY( stlDestroySemaphore( &gSemaphore,
                                  &gErrorStack ) == STL_SUCCESS );
    PRINTERR( &gErrorStack );
    PRINTPASS;

    /*
     * Test try timed acquire semaphore
     */
    stlPrintf("Test stlTimedAcquireSemaphore(twice):\n");
    stlStrncpy( sSemName, "__SEM", TEST_BUFFER_SIZE );

    STL_TRY( stlCreateSemaphore( &gSemaphore,
                                 sSemName,
                                 1,
                                 &gErrorStack ) == STL_SUCCESS );

    STL_TRY( stlTimedAcquireSemaphore( &gSemaphore,
                                       1000,
                                       &sIsTimedOut,
                                       &gErrorStack ) == STL_SUCCESS );
    STL_TRY( sIsTimedOut == STL_FALSE );
    STL_TRY( stlTimedAcquireSemaphore( &gSemaphore,
                                       1000,
                                       &sIsTimedOut,
                                       &gErrorStack ) == STL_SUCCESS );
    STL_TRY( sIsTimedOut == STL_TRUE );
    STL_TRY( stlDestroySemaphore( &gSemaphore,
                                  &gErrorStack ) == STL_SUCCESS );
    PRINTERR( &gErrorStack );
    PRINTPASS;

    /*
     * Test release semaphore
     */
    stlPrintf("Test stlReleaseSemaphore(1 release => 2 acquire):\n");
    stlStrncpy( sSemName, "__SEM", TEST_BUFFER_SIZE );

    STL_TRY( stlCreateSemaphore( &gSemaphore,
                                 sSemName,
                                 1,
                                 &gErrorStack ) == STL_SUCCESS );

    STL_TRY( stlReleaseSemaphore( &gSemaphore,
                                  &gErrorStack ) == STL_SUCCESS );
    STL_TRY( stlAcquireSemaphore( &gSemaphore,
                                  &gErrorStack ) == STL_SUCCESS );
    STL_TRY( stlAcquireSemaphore( &gSemaphore,
                                  &gErrorStack ) == STL_SUCCESS );
    STL_TRY( stlDestroySemaphore( &gSemaphore,
                                  &gErrorStack ) == STL_SUCCESS );
    PRINTPASS;

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
