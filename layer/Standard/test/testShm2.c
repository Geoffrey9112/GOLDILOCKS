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

#define MAX_TEST 11

void * gAddressMatrix[MAX_TEST] =
{
    (void*)0xFFFFFFFFFFFF0000,
    (void*)0x0FFFFFFFFFFF0000,
    (void*)0x00FFFFFFFFFF0000,
    (void*)0x0000FFFFFFFF0000,
    (void*)0x00000FFFFFFF0000,
    (void*)0x000000FFFFFF0000,
    (void*)0x0000000FFFFF0000,
    (void*)0x00000000FFFF0000,
    (void*)0x000000000FFF0000,
    (void*)0x0000000000FF0000,
    (void*)0x00000000000F0000
};
    

stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlShm    sShm;
    stlInt32  sState = 0;
    stlProc   sProc;
    stlBool   sIsChild;
    stlInt32  sExitCode;
    stlInt32  i;
    
    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_INIT_ERROR_STACK( &gErrorStack );

    /*
     * Test attach shared memory
     */
    for( i = 0; i < MAX_TEST; i++ )
    {
        stlPrintf( "%d. Test stlCreateShm Address(0x%lX):\n",
                   i + 1,
                   gAddressMatrix[i] );
        
        if( stlCreateShm( &sShm,
                          "__SHM1",
                          1000000,
                          gAddressMatrix[i],
                          100,
                          &gErrorStack ) == STL_SUCCESS )
        {
            STL_TRY( stlDestroyShm( &sShm,
                                    &gErrorStack ) == STL_SUCCESS );
            PRINTPASS;
        }
        else
        {
            stlPopError( &gErrorStack );
            PRINTFAIL;
        }
    }

    stlPrintf( "%d. Test stlCreateShm inter-process:\n", MAX_TEST + 1 );
    
    STL_TRY( stlForkProc( &sProc,
                          &sIsChild,
                          &gErrorStack ) == STL_SUCCESS );

    if( sIsChild == STL_TRUE )
    {
        STL_TRY( stlCreateShm( &sShm,
                               "__SHM1",
                               1000000,
                               (void*)0x80000000,
                               100,
                               &gErrorStack ) == STL_SUCCESS );

        stlSleep( 1000000 );
        
        STL_TRY( stlDestroyShm( &sShm,
                                &gErrorStack ) == STL_SUCCESS );

        stlExitProc( STL_SUCCESS );
    }
    else
    {
        STL_TRY( stlCreateShm( &sShm,
                               "__SHM2",
                               1000001,
                               (void*)0x80000000,
                               100,
                               &gErrorStack ) == STL_SUCCESS );

        STL_TRY( stlWaitProc( &sProc,
                              &sExitCode,
                              &gErrorStack ) == STL_SUCCESS );

        STL_TRY( stlDestroyShm( &sShm,
                                &gErrorStack ) == STL_SUCCESS );

        STL_TRY_THROW( sExitCode == STL_SUCCESS, RAMP_ERR_CHILD );
    }
    
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
