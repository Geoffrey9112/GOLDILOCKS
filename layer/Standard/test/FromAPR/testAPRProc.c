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

#define TESTSTR "This is a test\n"

stlProc gNewProc;

#define PRINTERR                                            \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( &sErrStack ),       \
                   stlGetLastErrorMessage( &sErrStack ) );  \
    } while(0)

#define PRINTDEBUG
// #define PRINTDEBUG  fprintf( stderr, "%s:%d\n", __FILE__, __LINE__ );

/**
 * stlForkProc(), stlExecProc() 함수 테스트
 */ 
stlBool TestCreateProc()
{
    stlFile        sPipeP2C[2];
    stlFile        sPipeC2P[2];

    stlFile        sStdInFile;
    stlFile        sStdOutFile;

    stlFile        sOrgIn;
    stlFile        sOrgOut;
    
    stlSize        sLength;
    stlChar        sBuf[256];

    stlChar      * sArgs[2];
    stlBool        sIsChild;

    stlErrorStack  sErrStack;

    stlMemset( sBuf, 0x00, 256 );

    STL_INIT_ERROR_STACK( &sErrStack );

    sArgs[0] = "procChild";
    sArgs[1] = NULL;

    /*
     * Pipe 를 설정한다.
     */

    /* Pipe for Parent => Child */
    STL_TRY( stlCreatePipe( &sPipeP2C[0],    
                            &sPipeP2C[1],    
                            &sErrStack )
             == STL_SUCCESS );

    /* Pipe for Child => Parent */
    STL_TRY( stlCreatePipe( &sPipeC2P[0],    
                            &sPipeC2P[1],    
                            &sErrStack )
             == STL_SUCCESS );

    /*
     * STDIN, STDOUT 복원을 위한 보관
     */
    STL_TRY( stlOpenStdinFile( &sStdInFile,
                               &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlDuplicateFile( &sOrgIn,
                               &sStdInFile,
                               1,
                               &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlOpenStdoutFile( &sStdOutFile,
                                &sErrStack )
             == STL_SUCCESS );
    STL_TRY( stlDuplicateFile( &sOrgOut,
                               &sStdOutFile,
                               1,
                               &sErrStack )
             == STL_SUCCESS );
    
    STL_TRY( stlForkProc( &gNewProc,
                          &sIsChild,
                          &sErrStack )
             == STL_SUCCESS );
    PRINTDEBUG;

    if ( sIsChild == STL_TRUE )
    {
        /* Child Process */

        /*
         * Set Pipe for Parent => Child
         */
        STL_TRY( stlDuplicateFile( &sStdInFile,
                                   &sPipeP2C[0],
                                   2,
                                   &sErrStack )
                 == STL_SUCCESS );
        STL_TRY( stlCloseFile( &sPipeP2C[1],
                               &sErrStack )
                 == STL_SUCCESS );

        /*
         * Set Pipe for Child => Parent
         */
        STL_TRY( stlDuplicateFile( &sStdOutFile,
                                   &sPipeC2P[1],
                                   2,
                                   &sErrStack )
                 == STL_SUCCESS );
        STL_TRY( stlCloseFile( &sPipeC2P[0],
                               &sErrStack )
                 == STL_SUCCESS );

        /*
         * procChild 프로세스를 수행
         */
        STL_TRY( stlExecuteProc( sArgs[0],
                                 sArgs,
                                 &sErrStack )
                 == STL_SUCCESS );
        PRINTDEBUG;
    }
    else
    {
        /* Server Process */
        
        /*
         * Set Pipe for Parent => Child
         */
        STL_TRY( stlDuplicateFile( &sStdOutFile,
                                   &sPipeP2C[1],
                                   2,
                                   &sErrStack )
                 == STL_SUCCESS );
        STL_TRY( stlCloseFile( &sPipeP2C[0],
                               &sErrStack )
                 == STL_SUCCESS );

        /*
         * Set Pipe for Child => Parent
         */
        STL_TRY( stlDuplicateFile( &sStdInFile,
                                   &sPipeC2P[0],
                                   2,
                                   &sErrStack )
                 == STL_SUCCESS );
        STL_TRY( stlCloseFile( &sPipeC2P[1],
                               &sErrStack )
                 == STL_SUCCESS );

        PRINTDEBUG;
    }

    sLength = strlen(TESTSTR);
    STL_TRY( stlWriteFile( &sStdOutFile,
                           TESTSTR,
                           sLength,
                           &sLength,
                           &sErrStack )
             == STL_SUCCESS );
    STL_TRY(strlen(TESTSTR) == sLength);
    PRINTDEBUG;

    sLength = 256;

    STL_TRY( stlReadFile( &sStdInFile,
                          sBuf,
                          sLength,
                          &sLength,
                          &sErrStack )
             == STL_SUCCESS );
    PRINTDEBUG;

    STL_TRY(strcmp(TESTSTR, sBuf) == 0);

    /*
     * STDIN, STDOUT 의 원복
     */
    STL_TRY( stlDuplicateFile( &sStdInFile,
                               &sOrgIn,
                               2,
                               &sErrStack )
             == STL_SUCCESS );
    STL_TRY( stlDuplicateFile( &sStdOutFile,
                               &sOrgOut,
                               2,
                               &sErrStack )
             == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlWaitProc() 함수 테스트
 */
stlBool TestProcWait()
{
    stlInt32      sExitCode;
    
    stlErrorStack sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlWaitProc( &gNewProc,
                          &sExitCode,
                          &sErrStack )
             == STL_SUCCESS );
    
    STL_TRY( sExitCode == 0 );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief File Redirection 테스트
 */
stlBool TestFileReDir()
{
    stlFile        sStdIn;
    stlFile        sStdOut;

    stlFile        sInFile;
    stlFile        sOutFile;
    
    stlChar      * sArgs[2];
    stlBool        sIsChild;
    stlInt32       sExitCode;

    stlChar        sBuf[256];
    stlSize        sLength;
    stlOffset      sOffset;

    stlStatus      sStatus;
    stlErrorStack  sErrStack;
    
    stlMemset( sBuf, 0x00, 256 );

    STL_INIT_ERROR_STACK( &sErrStack );
    
    sStatus = stlOpenFile( &sInFile,
                           "data/stdin",
                           STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_CREATE | STL_FOPEN_EXCL,
                           STL_FPERM_OS_DEFAULT,
                           &sErrStack);
    STL_TRY(sStatus == STL_SUCCESS);

    sStatus = stlOpenFile( &sOutFile,
                           "data/stdout",
                           STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_CREATE | STL_FOPEN_EXCL,
                           STL_FPERM_OS_DEFAULT,
                           &sErrStack);
    STL_TRY(sStatus == STL_SUCCESS);

    sLength = strlen(TESTSTR);
    STL_TRY( stlWriteFile( &sInFile,
                           TESTSTR,
                           sLength,
                           &sLength,
                           &sErrStack )
             == STL_SUCCESS );

    sOffset = 0;
    STL_TRY( stlSeekFile( &sInFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          &sErrStack )
             == STL_SUCCESS );
    STL_TRY(sOffset == 0);

    sArgs[0] = "procChild";
    sArgs[1] = NULL;

    STL_TRY( stlForkProc( &gNewProc,
                          &sIsChild,
                          &sErrStack )
             == STL_SUCCESS );
    PRINTDEBUG;

    if ( sIsChild == STL_TRUE )
    {
        /* Child Process */

        /*
         * File을 Redirection 한다.
         *   ; data/stdin  => STDIN
         *   ; data/stdout => STDOUT
         */
        STL_TRY( stlOpenStdinFile( &sStdIn,
                                   &sErrStack )
                 == STL_SUCCESS );
        
        STL_TRY( stlOpenStdoutFile( &sStdOut,
                                    &sErrStack )
                 == STL_SUCCESS );
        
        STL_TRY( stlDuplicateFile( &sStdIn,
                                   &sInFile,
                                   2,
                                   &sErrStack )
                 == STL_SUCCESS );
        STL_TRY( stlDuplicateFile( &sStdOut,
                                   &sOutFile,
                                   2,
                                   &sErrStack )
                 == STL_SUCCESS );

        /*
         * procChild 프로세스를 수행
         */
        STL_TRY( stlExecuteProc( sArgs[0],
                                 sArgs,
                                 &sErrStack )
                 == STL_SUCCESS );
        PRINTDEBUG;
    }
    else
    {
        /* Server Process */
        
        STL_TRY( stlWaitProc( &gNewProc,
                              &sExitCode,
                              &sErrStack )
                 == STL_SUCCESS );
        STL_TRY( sExitCode == 0 );

        PRINTDEBUG;
    }

    /*
     * Child Process인 procChild를 통해  
     * sInFile 의 내용이 sOutFile로 옮겨졌는지를 확인
     */
    sOffset = 0;
    STL_TRY( stlSeekFile( & sOutFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          &sErrStack )
             == STL_SUCCESS);

    sLength = 256;
    STL_TRY( stlReadFile( &sOutFile,
                          sBuf,
                          sLength,
                          &sLength,
                          &sErrStack )
             == STL_SUCCESS );
    
    STL_TRY(strcmp(TESTSTR, sBuf) == 0);

    STL_TRY( stlCloseFile( &sInFile, &sErrStack ) == STL_SUCCESS );
    STL_TRY( stlCloseFile( &sOutFile, &sErrStack ) == STL_SUCCESS );

    STL_TRY( stlRemoveFile( "data/stdin", &sErrStack ) == STL_SUCCESS );
    STL_TRY( stlRemoveFile( "data/stdout", &sErrStack ) == STL_SUCCESS );

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
     * Test CreateProc
     */
    sState = 1;
    STL_TRY(TestCreateProc() == STL_TRUE);

    /*
     * Test ProcWait
     */
    sState = 2;
    STL_TRY(TestProcWait() == STL_TRUE);

    /*
     * Test FileReDir
     */
    sState = 3;
    STL_TRY(TestFileReDir() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
