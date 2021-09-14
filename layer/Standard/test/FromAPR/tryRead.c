#include <stdlib.h>
#include <stl.h>

#define TESTFILE "data/testfile.lock"

#define FAILED_READ      0
#define SUCCESSFUL_READ  1
#define UNEXPECTED_ERROR 2

stlInt32 main(stlInt32 aArgc, const stlChar * const *aArgv)
{
    stlFile   sFile;
    stlStatus sStatus;

    stlErrorStack sErrStack;

    STL_TRY(stlInitialize() == STL_SUCCESS);
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlOpenFile( &sFile,
                          TESTFILE,
                          STL_FOPEN_WRITE,
                          STL_FPERM_OS_DEFAULT,
                          &sErrStack )
             == STL_SUCCESS);

    sStatus = stlLockFile( &sFile,
                           STL_FLOCK_EXCLUSIVE | STL_FLOCK_NONBLOCK,
                           &sErrStack );

    if (sStatus == STL_SUCCESS)
    {
        exit(SUCCESSFUL_READ);
    }
    else
    {
        if ( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_AGAIN )
        {
            exit(FAILED_READ);
        }
    }

    exit(UNEXPECTED_ERROR);

    STL_FINISH;

    exit( stlGetLastErrorCode( &sErrStack ) );
}

