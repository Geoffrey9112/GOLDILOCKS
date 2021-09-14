#include <stl.h>

#define STR_SIZE 45

stlStatus DummyDepth1()
{
    void        * sFrameArray[10];
    stlInt32      sFrameSize;
    stlFile       sFile;
    stlErrorStack sErrorStack;
    stlUInt32     sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( stlOpenFile( &sFile,
                          "testBacktrace.log",
                          STL_FOPEN_TRUNCATE | STL_FOPEN_CREATE | STL_FOPEN_WRITE,
                          STL_FPERM_UWRITE | STL_FPERM_UREAD,
                          &sErrorStack ) == STL_SUCCESS );
    sState = 1;
                          
    sFrameSize = stlBacktrace( sFrameArray, 10, NULL, NULL );
    stlBacktraceSymbolsToFile( sFrameArray,
                               sFrameSize,
                               &sFile );

    STL_TRY( stlCloseFile( &sFile, &sErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    stlPrintf( "%s\n", stlGetLastErrorMessage( &sErrorStack ) );
    
    if( sState == 1 )
    {
        (void)stlCloseFile( &sFile, &sErrorStack );
    }
    
    return STL_FAILURE;
}

stlStatus DummyDepth2()
{
    return DummyDepth1();
}

stlStatus DummyDepth3()
{
    return DummyDepth2();
}

stlStatus TestBacktrace()
{
    return DummyDepth3();
}

int main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;
    
    STL_TRY( stlInitialize() == STL_SUCCESS );

    /*
     * Test Backtrace
     */
    sState = 1;
    STL_TRY( TestBacktrace() == STL_SUCCESS );

    stlPrintf( "PASS" );

    return 0;
    
    STL_FINISH;

    stlPrintf( "FAILURE %d", sState );

    return -1;
}
