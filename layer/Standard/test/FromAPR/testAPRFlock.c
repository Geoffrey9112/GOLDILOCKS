#include <stdio.h>
#include <stl.h>

#ifdef WIN32
#ifdef BINPATH
#define TESTBINPATH STL_STRINGIFY(BINPATH) "/"
#else
#define TESTBINPATH ""
#endif
#else
#define TESTBINPATH "./"
#endif

#ifdef WIN32
#define EXTENSION ".exe"
#elif NETWARE
#define EXTENSION ".nlm"
#else
#define EXTENSION
#endif

#define TESTFILE "data/testfile.lock"

#define FAILED_READ      0
#define SUCCESSFUL_READ  1
#define UNEXPECTED_ERROR 2

#define PRINTERR                                            \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( &sErrStack ),       \
                   stlGetLastErrorMessage( &sErrStack ) );  \
    } while(0)

/*
 * Run Reader Process
 */
stlInt32 LaunchReader()
{
    stlProc          sProc = {0};
    stlChar       *  sArgs[2];
    stlInt32         sExitCode;
    stlBool          sIsChild;

    stlErrorStack sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    sArgs[0] = "tryRead";
    sArgs[1] = NULL;

    STL_TRY( stlForkProc( &sProc,
                          &sIsChild,
                          &sErrStack )
             == STL_SUCCESS );

    if ( sIsChild == STL_TRUE )
    {
        STL_TRY( stlExecuteProc( sArgs[0],
                                 sArgs,
                                 &sErrStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( stlWaitProc( &sProc,
                              &sExitCode,
                              &sErrStack )
                 == STL_SUCCESS );
    }
                          
    return sExitCode;

    STL_FINISH;

    PRINTERR;

    return UNEXPECTED_ERROR;
}

/**
 * @brief stlLockFile() 함수 테스트
 */
stlBool TestWithLock()
{
    stlFile  sFile;
    stlInt32 sCode;

    stlErrorStack sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY(stlOpenFile( &sFile,
                         TESTFILE,
                         STL_FOPEN_WRITE | STL_FOPEN_CREATE,
                         STL_FPERM_OS_DEFAULT,
                         &sErrStack )
            == STL_SUCCESS);

    STL_TRY( stlLockFile( &sFile,
                          STL_FLOCK_EXCLUSIVE,
                          &sErrStack )
             == STL_SUCCESS);

    /*
     * File Lock을 걸었으므로 Client Process가 실패해야 함
     */
    sCode = LaunchReader();
    STL_TRY( sCode == FAILED_READ );

    STL_TRY( stlCloseFile( &sFile, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief Lock없이 Reader 만 수행
 */
stlBool TestWithoutLock()
{
    stlInt32 sCode;
    /*
     * Lock을 걸지 않았으므로, 성공해야 함.
     */
    sCode = LaunchReader();
    STL_TRY( sCode == SUCCESSFUL_READ );

    return STL_TRUE;

    STL_FINISH;
    
    return STL_FALSE;
}

/**
 * @brief stlRemoveFile() 함수 테스트
 */
stlBool TestRemoveLockFile()
{
    stlErrorStack sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlRemoveFile( TESTFILE,
                            &sErrStack )
             == STL_SUCCESS);

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
     * Test WithLock
     */
    sState = 1;
    STL_TRY(TestWithLock() == STL_TRUE);

    /*
     * Test WithoutLock
     */
    sState = 2;
    STL_TRY(TestWithoutLock() == STL_TRUE);

    /*
     * Test RemoveLockFile
     */
    sState = 3;
    STL_TRY(TestRemoveLockFile() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
