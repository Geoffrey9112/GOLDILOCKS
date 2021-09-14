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

void MsgPut(stlInt32 aBoxNum, stlChar *aMsg)
{
    strncpy( gBoxes[aBoxNum].mMsg,
             aMsg,
             strlen(aMsg) + 1 );
    gBoxes[aBoxNum].mMsgAvail = 1;
}

stlInt32 main()
{
    stlShm     sShm;
    stlInt32   sIdx;
    stlInt32   sSent;

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

    /*
     * produce messages on all of the boxes, in descending order,
     * Yes, we could just return N_BOXES, but I want to have a double-check
     * in this code.  The original code actually sent N_BOXES - 1 messages,
     * so rather than rely on possibly buggy code, this way we know that we
     * are returning the right number.
     */
    for (sIdx = N_BOXES - 1, sSent = 0;
         sIdx >= 0;
         sIdx--, sSent++)
    {
        MsgPut(sIdx, MSG);
        // stlSleep(10000);  
    }

    STL_TRY( stlDetachShm( &sShm, &sErrStack ) == STL_SUCCESS );

    return sSent;

    STL_FINISH;

    exit(-1);
}
