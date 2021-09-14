#include <stl.h>

void * STL_THREAD_FUNC thread1( stlThread * aThread, void * aArg ); 
void * STL_THREAD_FUNC thread2( stlThread * aThread, void * aArg ); 

stlThread      gThread[2];
stlShm         gShm;
stlSemaphore * gSemaphore;

stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlInt32       sState = 0;
    stlErrorStack  sErrorStack;
    stlStatus      sThreadStatus;
    
    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( stlAttachShm( &gShm,
                           "__SHM",
                           1000000,
                           NULL,
                           &sErrorStack ) == STL_SUCCESS );
    
    gSemaphore = (stlSemaphore*)stlGetShmBaseAddr( &gShm );
    
    STL_TRY( stlCreateThread( &gThread[0],
                              NULL,
                              thread1,
                              NULL,
                              &sErrorStack ) == STL_SUCCESS );
    STL_TRY( stlCreateThread( &gThread[1],
                              NULL,
                              thread2,
                              NULL,
                              &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlJoinThread( &gThread[1],
                            &sThreadStatus,
                            &sErrorStack ) == STL_SUCCESS );
    STL_TRY( sThreadStatus == STL_SUCCESS );
    STL_TRY( stlJoinThread( &gThread[0],
                            &sThreadStatus,
                            &sErrorStack ) == STL_SUCCESS );
    STL_TRY( sThreadStatus == STL_SUCCESS );

    STL_TRY( stlDetachShm( &gShm,
                           &sErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    stlTerminate();

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            stlTerminate();
        default:
            break;
    }

    return STL_FAILURE;
}

void * STL_THREAD_FUNC thread1( stlThread * aThread, void * aArg )
{
    stlErrorStack sErrorStack;
    
    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( stlReleaseSemaphore( &gSemaphore[2],
                                  &sErrorStack ) == STL_SUCCESS );
    STL_TRY( stlAcquireSemaphore( &gSemaphore[0],
                                  &sErrorStack ) == STL_SUCCESS );

    stlPrintf( "work done (thread 1)\n" );

    STL_TRY( stlReleaseSemaphore( &gSemaphore[2],
                                  &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlExitThread( aThread,
                            STL_SUCCESS,
                            &sErrorStack ) == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;
    
    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
    
    return NULL;
}
                                                                       
void * STL_THREAD_FUNC thread2( stlThread * aThread, void * aArg )
{
    stlErrorStack sErrorStack;
    
    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( stlReleaseSemaphore( &gSemaphore[3],
                                  &sErrorStack ) == STL_SUCCESS );
    STL_TRY( stlAcquireSemaphore( &gSemaphore[1],
                                  &sErrorStack ) == STL_SUCCESS );

    stlPrintf( "work done (thread 2)\n" );

    STL_TRY( stlReleaseSemaphore( &gSemaphore[3],
                                  &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlExitThread( aThread,
                            STL_SUCCESS,
                            &sErrorStack ) == STL_SUCCESS );
    
    return NULL;

    STL_FINISH;

    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
    
    return NULL;
}
                                                                       
