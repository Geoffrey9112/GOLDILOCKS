#include <stdlib.h>
#include <stl.h>

typedef struct MsgBox
{
    stlChar  mMsg[1024]; 
    stlInt32 mMsgAvail; 
} MsgBox;

MsgBox *gBoxes;

#define N_BOXES 10
#define N_MESSAGES 100
#define MSG "Sending a message"

#define SHM_NAME ("SHMNAME")
#define SHM_KEY  (12355)

#define SEC2USEC (1000000)

stlInt32 MsgWait(stlInt32 aSleepSec,
                stlInt32 aFirstBox,
                stlInt32 aLastBox)
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
            if ( gBoxes[sIdx].mMsgAvail &&
                 !strcmp(gBoxes[sIdx].mMsg, MSG) )
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

stlInt32 main()
{
    stlShm    sShm;
    stlInt32  sRecvd;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    stlInitialize();
    
    STL_TRY( stlAttachShm( &sShm,
                           SHM_NAME,
                           SHM_KEY,
                           NULL,
                           &sErrStack )
             == STL_SUCCESS );

    gBoxes = stlGetShmBaseAddr( &sShm );

    /* consume messages on all of the boxes */
    sRecvd = MsgWait(3, 0, N_BOXES); /* wait for 3 seconds for messages */
    
    STL_TRY( stlDetachShm( &sShm, &sErrStack ) == STL_SUCCESS );

    return sRecvd;

    STL_FINISH;

    exit(-1);
}
