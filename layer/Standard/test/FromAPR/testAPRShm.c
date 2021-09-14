#include <stdio.h>
#include <stdlib.h>
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

typedef struct MsgBox
{
    stlChar  mMsg[1024]; 
    stlInt32 mMsgAvail; 
} MsgBox;

MsgBox * gBoxes = NULL;

#define N_BOXES 10
#define N_MESSAGES 100
#define MSG "Sending a message"

#define TEST_SHM_SIZE (stlSize)(N_BOXES * sizeof(MsgBox))
#define TEST_SHM_NAME ("SHMNAME")
#define TEST_SHM_KEY  (12355)

#define SEC2USEC (1000000)

#define PRINTERR                                            \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( &sErrStack ),       \
                   stlGetLastErrorMessage( &sErrStack ) );  \
    } while(0)

/**
 * @brief 일정 시간동안 메시지를 기다리는 함수
 */
stlInt32 MsgWait( stlInt32 aSleepSec,
                  stlInt32 aFirstBox,
                  stlInt32 aLastBox )
{
    stlInt32    sIdx;
    stlInt32    sRecvd = 0;
    stlTime     sStart = stlNow();
    stlTime     sNow;
    stlInterval sSleepDuration = aSleepSec * SEC2USEC;

    sNow = stlNow();
    while ( (sNow - sStart) < sSleepDuration )
    {
        for (sIdx = aFirstBox; sIdx < aLastBox; sIdx++)
        {
            if (gBoxes[sIdx].mMsgAvail &&
                !strcmp(gBoxes[sIdx].mMsg, MSG))
            {
                sRecvd++;
                gBoxes[sIdx].mMsgAvail = 0; /* reset back to 0 */
                /* reset the msg field.  1024 is a magic number and it should
                 * be a macro, but I am being lazy.
                 */
                memset(gBoxes[sIdx].mMsg, 0, 1024);
            }
        }
        stlSleep( 10000 ); /* 10ms */
        sNow = stlNow();
    }

    return sRecvd;
}

void MsgPut(stlInt32  aBoxNum,
            stlChar * aMsg )
{
    strncpy( gBoxes[aBoxNum].mMsg,
             aMsg,
             strlen(aMsg) + 1 );
    gBoxes[aBoxNum].mMsgAvail = 1;
}

/**
 * @brief stlCreateShm() 함수 테스트
 */
stlBool TestAnonCreate()
{
    stlShm    sShm;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlCreateShm( &sShm,
                           TEST_SHM_NAME,
                           TEST_SHM_KEY,
                           NULL,
                           TEST_SHM_SIZE,
                           &sErrStack )
             == STL_SUCCESS );
    STL_TRY( stlGetShmSize(&sShm) >= TEST_SHM_SIZE );

    STL_TRY( stlDestroyShm( &sShm, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlGetShmSize() 함수 테스트
 */
stlBool TestCheckSize()
{
    stlShm    sShm;
    stlSize   sRetSize;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlCreateShm( &sShm,
                           TEST_SHM_NAME,
                           TEST_SHM_KEY,
                           NULL,
                           TEST_SHM_SIZE,
                           &sErrStack )
             == STL_SUCCESS );

    sRetSize = stlGetShmSize(&sShm);
    STL_TRY( sRetSize == TEST_SHM_SIZE );

    STL_TRY( stlDestroyShm( &sShm, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlGetShmBaseAddr() 함수 테스트
 */
stlBool TestShmAllocate()
{
    stlShm    sShm;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlCreateShm( &sShm,
                           TEST_SHM_NAME,
                           TEST_SHM_KEY,
                           NULL,
                           TEST_SHM_SIZE,
                           &sErrStack )
             == STL_SUCCESS );

    gBoxes = stlGetShmBaseAddr( &sShm );
    STL_TRY( gBoxes != NULL );

    STL_TRY( stlDestroyShm( &sShm, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief Parent와 Child의 Shared Memory 공유 테스트
 */
stlBool TestAnon()
{
    stlProc   sProc;
    stlBool   sIsChild;
    stlInt32  sExitCode;
    
    stlShm    sShm;
    stlSize   sRetSize;
    
    stlInt32  sCount;
    stlInt32  sIdx;
    stlInt32  sNum;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlCreateShm( &sShm,
                           TEST_SHM_NAME,
                           TEST_SHM_KEY,
                           NULL,
                           TEST_SHM_SIZE,
                           &sErrStack )
             == STL_SUCCESS );

    sRetSize = stlGetShmSize( &sShm );
    STL_TRY( sRetSize == TEST_SHM_SIZE );

    gBoxes = stlGetShmBaseAddr( &sShm );
    STL_TRY( gBoxes != NULL );

    STL_TRY( stlForkProc( &sProc,
                          &sIsChild,
                          &sErrStack )
             == STL_SUCCESS );

    if ( sIsChild == STL_TRUE )
    {
        /* child process */
        sNum = MsgWait(3, 0, N_BOXES);
        
        /*
         * exit with the number of messages received so that the parent
         * can check that all messages were received.
         */
        exit(sNum);
    }
    else
    {
        /* parent process */
        sIdx = N_BOXES;
        sCount = 0;
        while ( sCount < N_MESSAGES )
        {
            sCount++;
            sIdx = sIdx - 3;
            if ( sIdx < 0 )
            {
                sIdx += N_BOXES; /* start over at the top */
            }
            MsgPut(sIdx, MSG);
            stlSleep(10000);  /* 10 msec */
        }
    }

    /* wait for the child */
    STL_TRY( stlWaitProc( &sProc,
                          &sExitCode,
                          &sErrStack )
             == STL_SUCCESS );

    STL_TRY( sExitCode == N_MESSAGES );

    STL_TRY( stlDestroyShm( &sShm, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief Shared Memory를 이용한 Producer 와 Consumer 테스트
 */
stlBool TestNamed()
{
    stlShm         sShm;
    stlSize        sRetSize;

    stlProc        sProducer;
    stlProc        sConsumer;
    stlBool        sIsChild;

    stlInt32       sSendExit;
    stlInt32       sRecvExit;
    
    stlChar      * sArgs[4];

    stlErrorStack sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlCreateShm( &sShm,
                           TEST_SHM_NAME,
                           TEST_SHM_KEY,
                           NULL,
                           TEST_SHM_SIZE,
                           &sErrStack )
             == STL_SUCCESS );

    sRetSize = stlGetShmSize( &sShm );
    STL_TRY( sRetSize == TEST_SHM_SIZE );

    gBoxes = stlGetShmBaseAddr( &sShm );
    STL_TRY(NULL != gBoxes);

    /*
     * Producer Process를 생성
     */

    STL_TRY( stlForkProc( &sProducer,
                          &sIsChild,
                          &sErrStack )
             == STL_SUCCESS );
    
    if ( sIsChild == STL_TRUE )
    {
        /* Child Process */

        /*
         * shmProducer 프로세스를 수행
         */

        sArgs[0] = "shmProducer" EXTENSION;
        sArgs[1] = NULL;
        STL_TRY( stlExecuteProc( sArgs[0],
                                 sArgs,
                                 &sErrStack )
                 == STL_SUCCESS );
    }
    else
    {
        /* Server Process */
    }
    
    /*
     * Consumer Process를 생성
     */

    STL_TRY( stlForkProc( &sConsumer,
                          &sIsChild,
                          &sErrStack )
             == STL_SUCCESS );
    
    if ( sIsChild == STL_TRUE )
    {
        /* Child Process */

        /*
         * shmConsumer 프로세스를 수행
         */

        sArgs[0] = "shmConsumer" EXTENSION;
        sArgs[1] = NULL;
        STL_TRY( stlExecuteProc( sArgs[0],
                                 sArgs,
                                 &sErrStack )
                 == STL_SUCCESS );
    }
    else
    {
        /* Server Process */
    }
    
    /*
     * Producer와 Consumer 의 종료를 대기
     */

    STL_TRY( stlWaitProc( &sProducer,
                          &sSendExit,
                          &sErrStack )
             == STL_SUCCESS );
    
    STL_TRY( stlWaitProc( &sConsumer,
                          &sRecvExit,
                          &sErrStack )
             == STL_SUCCESS );

    /*
     * Cleanup before testing that producer and consumer worked correctly.
     * This way, if they didn't succeed, we can just run this test again
     * without having to cleanup manually.
     */
    STL_TRY( stlDestroyShm( &sShm, &sErrStack ) == STL_SUCCESS );

    STL_TRY( sSendExit == sRecvExit );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlDestroyShm() 함수 테스트
 */
stlBool TestNamedRemove()
{
    stlShm   sShm1;
    stlShm   sShm2;

    stlErrorStack sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlCreateShm( &sShm1,
                           TEST_SHM_NAME,
                           TEST_SHM_KEY,
                           NULL,
                           TEST_SHM_SIZE,
                           &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlDestroyShm( &sShm1, &sErrStack ) == STL_SUCCESS );

    /*
     * On platforms which acknowledge the removal of the shared resource,
     * ensure another of the same name may be created after removal;
     */
    STL_TRY( stlCreateShm( &sShm2,
                           TEST_SHM_NAME,
                           TEST_SHM_KEY,
                           NULL,
                           TEST_SHM_SIZE,
                           &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlDestroyShm( &sShm2, &sErrStack ) == STL_SUCCESS );

    /*
     * 실패해야 함
     * Now ensure no named resource remains which we may attach to
     */
    
    STL_TRY( stlAttachShm( &sShm1,
                           TEST_SHM_NAME,
                           TEST_SHM_KEY,
                           NULL,
                           &sErrStack )
             != STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test AnonCreate
     */
    sState = 1;
    STL_TRY(TestAnonCreate() == STL_TRUE);

    /*
     * Test CheckSize
     */
    sState = 2;
    STL_TRY(TestCheckSize() == STL_TRUE);

    /*
     * Test ShmAllocate
     */
    sState = 3;
    STL_TRY(TestShmAllocate() == STL_TRUE);

    /*
     * Test Anon
     */
    sState = 4;
    STL_TRY(TestAnon() == STL_TRUE);

    /*
     * Test Named
     */
    sState = 5;
    STL_TRY(TestNamed() == STL_TRUE);

    /*
     * Test NamedRemove
     */
    sState = 6;
    STL_TRY(TestNamedRemove() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
