#include <stdlib.h>
#include <stl.h>

#define DATASTR "This is a test"
#define STRLEN 8092

stlInt32 main(stlInt32 aArgc, stlChar *aArgv[])
{
    stlSocket    sSock;
    stlSockAddr  sRemoteSockAddr;
    
    stlChar      sDataRecv[STRLEN];
    stlSize      sLength = STRLEN;
    stlInt32     sExitValue;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    stlInitialize();

    if (aArgc < 2)
    {
        exit(-1);
    }

    STL_TRY( stlSetSockAddr( &sRemoteSockAddr,
                             STL_AF_INET,
                             "127.0.0.1",
                             8021,
                             NULL,
                             &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlCreateSocket( &sSock,
                              STL_AF_INET,
                              SOCK_STREAM,
                              0,
                              &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlConnect( sSock,
                         &sRemoteSockAddr,
                         STL_INFINITE_TIME,
                         &sErrStack )
             == STL_SUCCESS );

    if (strcmp("read", aArgv[1]) == 0)
    {
        memset(sDataRecv, 0, STRLEN);
        
        STL_TRY( stlRecv( sSock,
                          sDataRecv,
                          &sLength,
                          &sErrStack )
                 == STL_SUCCESS );
        
        STL_TRY( stlCloseSocket( sSock, &sErrStack ) == STL_SUCCESS );

        STL_TRY( strcmp(sDataRecv, DATASTR) == 0 );

        sExitValue = (stlInt32) sLength;
    }
    else if (strcmp("write", aArgv[1]) == 0)
    {
        sLength = strlen(DATASTR);

        STL_TRY( stlSend( sSock,
                          DATASTR,
                          &sLength,
                          &sErrStack )
                 == STL_SUCCESS );

        STL_TRY( stlCloseSocket( sSock, &sErrStack ) == STL_SUCCESS );
        
        sExitValue = (stlInt32) sLength;
    }
    else if (strcmp("close", aArgv[1]) == 0)
    {
        STL_TRY( stlCloseSocket( sSock, &sErrStack ) == STL_SUCCESS );
        sExitValue = 0;
    }
    else
    {
        sExitValue = -1;
    }

    exit( sExitValue );

    STL_FINISH;

    exit(-1);
}
