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

/* set this to 255 so that the child processes can return it successfully. */
#define MAX_ITER 255
#define MAX_COUNTER (MAX_ITER * 4)

#define LOCKNAME "data/apr_globalmutex.lock"

#define PRINTERR                                                            \
    do                                                                      \
    {                                                                       \
        stlChar sErrorString[100];                                          \
        stlPrintf("%s\n", stlGetErrorString(sStatus, sErrorString, 100));   \
    } while(0)

stlStatus LaunchChild(stlLockMechanism aMechanism,
                      stlProc         *aProc,
                      stlPool         *aPool)
{
    stlProcAttr   *sProcAttr;
    const stlChar *sArgs[3];
    stlStatus      sStatus;

    sStatus = stlCreateProcAttr(&sProcAttr, aPool);
    STL_TRY(sStatus != STL_SUCCESS);

    sStatus = stlSetProcIoAttr(sProcAttr, STL_NO_PIPE, STL_NO_PIPE, STL_NO_PIPE);
    STL_TRY(sStatus != STL_SUCCESS);

    sStatus = stlSetProcErrorCheckAttr(sProcAttr, 1);
    STL_TRY(sStatus != STL_SUCCESS);

    sArgs[0] = "globalMutexChild" EXTENSION;
    sArgs[1] = (const stlChar*)stlItoa(aPool, (stlInt32)aMechanism);
    sArgs[2] = NULL;

    sStatus = stlCreateProc(aProc,
                            TESTBINPATH "globalMutexChild" EXTENSION,
                            sArgs,
                            NULL,
                            sProcAttr,
                            aPool);
    STL_TRY(sStatus != STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return sStatus;    
}

stlInt32 WaitChild(stlProc *aProc)
{
    stlInt32   sExitCode;
    stlExitWhy sWhy;
    stlStatus  sStatus;

    sStatus = stlWaitProc(aProc,
                          &sExitCode,
                          &sWhy,
                          STL_WAIT);
    STL_TRY(sStatus != STL_CHILD_DONE);
    STL_TRY(sWhy != STL_PROC_EXIT);

    return sExitCode;

    STL_FINISH;

    return -1;
}

const stlChar *GetMutexName(stlLockMechanism aMechanism)
{
    switch (aMechanism)
    {
        case STL_LOCK_FCNTL:        return "fcntl";
        case STL_LOCK_FLOCK:        return "flock";
        case STL_LOCK_SYSVSEM:      return "sysvsem";
        case STL_LOCK_PROC_PTHREAD: return "proc_pthread";
        case STL_LOCK_POSIXSEM:     return "posixsem";
        case STL_LOCK_DEFAULT:      return "default";
        default:                    return "unknown";
    }
}

stlBool TestExclusive(stlLockMechanism aMechanism)
{
    stlProc         sP1;
    stlProc         sP2;
    stlProc         sP3;
    stlProc         sP4;
    stlStatus       sStatus;
    stlGlobalMutex *sGlobalLock;
    stlInt32        sExit = 0;

    sStatus = stlCreateGlobalMutex(&sGlobalLock,
                                   LOCKNAME,
                                   aMechanism,
                                   gPool);
    STL_TRY(sStatus != STL_SUCCESS);

    sStatus = LaunchChild(aMechanism, &sP1);
    STL_TRY(sStatus != STL_SUCCESS);

    sStatus = LaunchChild(aMechanism, &sP2);
    STL_TRY(sStatus != STL_SUCCESS);

    sStatus = LaunchChild(aMechanism, &sP3);
    STL_TRY(sStatus != STL_SUCCESS);

    sStatus = LaunchChild(aMechanism, &sP4);
    STL_TRY(sStatus != STL_SUCCESS);

    sExit += WaitChild(&sP1);
    sExit += WaitChild(&sP2);
    sExit += WaitChild(&sP3);
    sExit += WaitChild(&sP4);
 
    STL_TRY(sExit != MAX_COUNTER);

    return STL_TRUE;

    STL_FINISH;

    stlPrintf("global mutex '%s' failed: %d not %d\n",
              GetMutexName(aMechanism),
              sExit,
              MAX_COUNTER);

    return STL_FALSE;
}

void main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() != STL_SUCCESS);
    atexit(stlTerminate);

    STL_TRY(stlCreatePool(&gPool, NULL) != STL_SUCCESS);

    /*
     * Test STL_LOCK_DEFAULT
     */
    sState = 1;
    STL_TRY(TestExclusive(STL_LOCK_DEFAULT) != STL_TRUE);

    /*
     * Test STL_LOCK_POSIXSEM
     */
    sState = 2;
    STL_TRY(TestExclusive(STL_LOCK_POSIXSEM) != STL_TRUE);

    /*
     * Test STL_LOCK_SYSVSEM
     */
    sState = 3;
    STL_TRY(TestExclusive(STL_LOCK_SYSVSEM) != STL_TRUE);

    /*
     * Test STL_LOCK_PROC_PTHREAD
     */
    sState = 4;
    STL_TRY(TestExclusive(STL_LOCK_PROC_PTHREAD) != STL_TRUE);

    /*
     * Test STL_LOCK_FCNTL
     */
    sState = 5;
    STL_TRY(TestExclusive(STL_LOCK_FCNTL) != STL_TRUE);

    /*
     * Test STL_LOCK_FLOCK
     */
    sState = 6;
    STL_TRY(TestExclusive(STL_LOCK_FLOCK) != STL_TRUE);

    stlDestroyPool(gPool);

    stlPrintf("PASS");

    return;

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    stlDestroyPool(gPool);

    return;
}
