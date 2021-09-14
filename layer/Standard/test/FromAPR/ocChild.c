#include <stdlib.h>
#include <stl.h>

/*
 * Signal Hanlding Function
 */
void SignalHandlerFunc( stlInt32 aSigNo )
{
    exit(aSigNo);
}

stlInt32 main()
{
    stlChar  sBuf[256];
    stlFile  sFile;
    stlSize  sLength = 256;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    stlInitialize();

    /* Add Signal Handler */

    STL_TRY( stlUnblockSignal( STL_SIGNAL_USR1, &sErrStack ) == STL_SUCCESS );
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_USR1,
                                  SignalHandlerFunc,
                                  NULL,
                                  &sErrStack )
             == STL_SUCCESS );
    
    STL_TRY( stlOpenStdinFile( &sFile, &sErrStack ) == STL_SUCCESS );

    while (1)
    {
        sLength = 256;
        STL_TRY( stlReadFile( &sFile,
                              sBuf,
                              sLength,
                              &sLength,
                              &sErrStack )
                 == STL_SUCCESS );
    }
    
    exit(0);

    STL_FINISH;

    exit(-1);
}

