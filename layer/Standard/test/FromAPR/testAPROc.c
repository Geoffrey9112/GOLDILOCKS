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
 * @brief stlKillProc() 함수 테스트
 */
stlBool TestChildKill()
{
    stlChar      * sArgs[2];

    stlProc        sNewProc;
    stlBool        sIsChild;
    stlInt32       sExitCode;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    sArgs[0] = "ocChild";
    sArgs[1] = NULL;

    STL_TRY( stlForkProc( &sNewProc,
                          &sIsChild,
                          &sErrStack )
             == STL_SUCCESS );
    PRINTDEBUG;
    
    if ( sIsChild == STL_TRUE )
    {
        /* Child Process */

        /*
         * ocChild 프로세스를 수행
         */
        STL_TRY( stlExecuteProc( sArgs[0],
                                 sArgs,
                                 &sErrStack )
                 == STL_SUCCESS );
    }
    else
    {
        /* Server Process */
    }
    
    /* allow time (2 secs) for things to settle... */
    stlSleep(2000000);
    PRINTDEBUG;

    STL_TRY( stlKillProc( &sNewProc,
                          STL_SIGNAL_USR1,
                          &sErrStack )
             == STL_SUCCESS );
    PRINTDEBUG;

    /* allow time (2 secs) for things to settle... */
    // stlSleep(2000000);

    /* 검증해야 함 */
    STL_TRY( stlWaitProc( &sNewProc,
                          &sExitCode,
                          &sErrStack )
             == STL_SUCCESS );
    
    STL_TRY( sExitCode == STL_SIGNAL_USR1 );
    
    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    stlPrintf("sExitCode:%d\n", sExitCode );
    
    return STL_FALSE;
}    


stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test ChildKill
     */
    sState = 1;
    STL_TRY(TestChildKill() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
