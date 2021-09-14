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
stlBool gAck;

void testSignalFunc( stlInt32 aSignalNo )
{
    gAck = STL_TRUE;
    stlPrintf( "received signal\n" );
}

stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlInt32      sState = 0;
    stlProc       sProc;
    stlSignalFunc sOldSignalFunc;
    
    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_INIT_ERROR_STACK( &gErrorStack );

    /*
     * Test send signal
     */
    gAck = STL_FALSE;
    stlPrintf("Test stlKillProc:\n");
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_QUIT,
                                  testSignalFunc,
                                  &sOldSignalFunc,
                                  &gErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlGetCurrentProc( &sProc,
                                &gErrorStack ) == STL_SUCCESS );
                                
    STL_TRY( stlKillProc( &sProc,
                           STL_SIGNAL_QUIT,
                           &gErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_QUIT,
                                  sOldSignalFunc,
                                  NULL,
                                  &gErrorStack ) == STL_SUCCESS );
    STL_TRY( gAck == STL_TRUE );
    PRINTPASS;
    
    /*
     * Test block signal
     */
    gAck = STL_FALSE;
    stlPrintf("Test stlBlockProc:\n");
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_QUIT,
                                  testSignalFunc,
                                  &sOldSignalFunc,
                                  &gErrorStack ) == STL_SUCCESS );

    STL_TRY( stlBlockSignal( STL_SIGNAL_QUIT,
                             &gErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlGetCurrentProc( &sProc,
                                &gErrorStack ) == STL_SUCCESS );
                                
    STL_TRY( stlKillProc( &sProc,
                           STL_SIGNAL_QUIT,
                           &gErrorStack ) == STL_SUCCESS );
    
    STL_TRY( gAck != STL_TRUE );
    PRINTPASS;

    /*
     * Test unblock signal
     */
    gAck = STL_FALSE;
    stlPrintf("Test stlUnblockProc:\n");
    STL_TRY( stlUnblockSignal( STL_SIGNAL_QUIT,
                               &gErrorStack ) == STL_SUCCESS );
    STL_TRY( gAck == STL_TRUE );
    
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_QUIT,
                                  sOldSignalFunc,
                                  NULL,
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
