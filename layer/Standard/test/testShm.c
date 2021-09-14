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

stlShm  gShm;

stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlInt32  sState = 0;
    stlProc   sProc;
    stlChar * sArgv[2];
    stlChar * sBuffer;
    stlBool   sIsChild;
    stlInt32  sExitCode;
    
    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_INIT_ERROR_STACK( &gErrorStack );

    /*
     * Test create shared memory
     */
    stlPrintf("Test stlCreateSemaphore(name too long):\n");
    STL_TRY( stlCreateShm( &gShm,
                           "INVALID_SHM_NAME",
                           1000000,
                           (void*)0x80000000,
                           100,
                           &gErrorStack ) != STL_SUCCESS );
    PRINTERR( &gErrorStack );
    PRINTPASS;
    
    /*
     * Test attach shared memory
     */
    stlPrintf("Test stlAttachShm:\n");
    STL_TRY( stlCreateShm( &gShm,
                           "__SHM",
                           1000000,
                           (void*)0x80000000,
                           100,
                           &gErrorStack ) == STL_SUCCESS );
    
    sBuffer = stlGetShmBaseAddr( &gShm );

    STL_TRY( stlForkProc( &sProc,
                          &sIsChild,
                          &gErrorStack ) == STL_SUCCESS );

    if( sIsChild == STL_TRUE )
    {
        sArgv[0] = "./readShm";
        sArgv[1] = NULL;
        STL_TRY( stlExecuteProc( sArgv[0],
                                 sArgv,
                                 &gErrorStack ) == STL_SUCCESS );
        
        stlExitProc( STL_SUCCESS );
    }
    else
    {
        STL_TRY( stlWaitProc( &sProc,
                              &sExitCode,
                              &gErrorStack ) == STL_SUCCESS );

        STL_TRY_THROW( sExitCode == STL_SUCCESS, RAMP_ERR_CHILD );
    }
    
    STL_TRY_THROW( stlStrncmp( sBuffer, "This is a test", 100 ) == 0,
                   RAMP_ERR_COMPARE_SHM );
    
    STL_TRY( stlDestroyShm( &gShm,
                            &gErrorStack ) == STL_SUCCESS );

    PRINTPASS;

    sState = 0;
    stlTerminate();

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COMPARE_SHM )
    {
        stlPrintf( " make sure that \"This is a test\"\n" );
    }
    
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
