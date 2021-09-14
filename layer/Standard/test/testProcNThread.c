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

stlShm gShm;

stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlInt32       sState = 0;
    stlProc        sProc;
    stlChar      * sArgv[2];
    stlSemaphore * sSemaphore;
    stlBool        sIsChild;
    stlInt32       sExitCode;

    stlPrintf( "Test Thread & Process & Semaphore\n" );
    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_INIT_ERROR_STACK( &gErrorStack );

    STL_TRY( stlCreateShm( &gShm,
                           "__SHM",
                           1000000,
                           NULL,
                           STL_SIZEOF(stlSemaphore) * 4,
                           &gErrorStack ) == STL_SUCCESS );
    
    sSemaphore = (stlSemaphore*)stlGetShmBaseAddr( &gShm );

    STL_TRY( stlCreateSemaphore( &sSemaphore[0],
                                 "SEM1",
                                 1,
                                 &gErrorStack ) == STL_SUCCESS );
    STL_TRY( stlCreateSemaphore( &sSemaphore[1],
                                 "SEM2",
                                 1,
                                 &gErrorStack ) == STL_SUCCESS );
    STL_TRY( stlCreateSemaphore( &sSemaphore[2],
                                 "SEM3",
                                 0,
                                 &gErrorStack ) == STL_SUCCESS );
    STL_TRY( stlCreateSemaphore( &sSemaphore[3],
                                 "SEM4",
                                 0,
                                 &gErrorStack ) == STL_SUCCESS );

    STL_TRY( stlAcquireSemaphore( &sSemaphore[0],
                                  &gErrorStack ) == STL_SUCCESS );
    STL_TRY( stlAcquireSemaphore( &sSemaphore[1],
                                  &gErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlForkProc( &sProc,
                          &sIsChild,
                          &gErrorStack ) == STL_SUCCESS );

    if( sIsChild == STL_TRUE )
    {
        sArgv[0] = "./workThreads";
        sArgv[1] = NULL;
        STL_TRY( stlExecuteProc( sArgv[0],
                                 sArgv,
                                 &gErrorStack ) == STL_SUCCESS );
    }
    else
    {
        stlPrintf( "waiting ... (thread 2)\n" );
        STL_TRY( stlAcquireSemaphore( &sSemaphore[3],
                                      &gErrorStack ) == STL_SUCCESS );
        STL_TRY( stlReleaseSemaphore( &sSemaphore[1],
                                      &gErrorStack ) == STL_SUCCESS );
        STL_TRY( stlAcquireSemaphore( &sSemaphore[3],
                                      &gErrorStack ) == STL_SUCCESS );
        stlPrintf( "waiting ... (thread 1)\n" );
        STL_TRY( stlAcquireSemaphore( &sSemaphore[2],
                                      &gErrorStack ) == STL_SUCCESS );
        STL_TRY( stlReleaseSemaphore( &sSemaphore[0],
                                      &gErrorStack ) == STL_SUCCESS );
        STL_TRY( stlAcquireSemaphore( &sSemaphore[2],
                                      &gErrorStack ) == STL_SUCCESS );
        
        STL_TRY( stlWaitProc( &sProc,
                              &sExitCode,
                              &gErrorStack ) == STL_SUCCESS );

        STL_TRY_THROW( sExitCode == STL_SUCCESS, RAMP_ERR_CHILD );
    }
    
    STL_TRY( stlDestroySemaphore( &sSemaphore[3],
                                  &gErrorStack ) == STL_SUCCESS );
    STL_TRY( stlDestroySemaphore( &sSemaphore[2],
                                  &gErrorStack ) == STL_SUCCESS );
    STL_TRY( stlDestroySemaphore( &sSemaphore[1],
                                  &gErrorStack ) == STL_SUCCESS );
    STL_TRY( stlDestroySemaphore( &sSemaphore[0],
                                  &gErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlDestroyShm( &gShm,
                            &gErrorStack ) == STL_SUCCESS );

    PRINTPASS;

    sState = 0;
    stlTerminate();

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CHILD )
    {
        stlPrintf( " The child process was failed\n" );
    }

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
