#include <stdio.h>
#include <stl.h>

#define STRLEN 21

stlSocket  gSock;

#define PRINTERR                                            \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( &sErrStack ),       \
                   stlGetLastErrorMessage( &sErrStack ) );  \
    } while(0)

/**
 * @brief stlCreateSocket() 테스트
 */
stlBool TestCreateSocket()
{
    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlCreateSocket( &gSock,
                              STL_AF_INET,
                              STL_SOCK_STREAM,
                              0,
                              &sErrStack)
             == STL_SUCCESS);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief STL_SOPT_SO_KEEPALIVE
 */
stlBool TestSetKeepAlive()
{
    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlSetSocketOption( gSock,
                                 STL_SOPT_SO_KEEPALIVE,
                                 1,
                                 &sErrStack )
             == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief STL_SOPT_SO_DEBUG
 */
stlBool TestSetDebug()
{
    /* On some platforms APR_SO_DEBUG can only be set as root; just test
     * for get/set consistency of this option. */

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlSetSocketOption( gSock,
                                 STL_SOPT_SO_DEBUG,
                                 1,
                                 &sErrStack )
             == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief STL_SOPT_SO_KEEPALIVE
 */
stlBool TestRemoveKeepAlive()
{
    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlSetSocketOption( gSock,
                                 STL_SOPT_SO_KEEPALIVE,
                                 0,
                                 &sErrStack )
             == STL_SUCCESS);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief STL_SOPT_TCP_NODELAY, STL_SOPT_TCP_NOPUSH
 */
stlBool TestCorkable()
{
    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlSetSocketOption( gSock,
                                 STL_SOPT_TCP_NODELAY,
                                 1,
                                 &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlSetSocketOption( gSock,
                                 STL_SOPT_TCP_NOPUSH,
                                 1,
                                 &sErrStack )
             == STL_SUCCESS);

    /* TCP_NODELAY is now in an unknown state; it may be zero if
     * TCP_NOPUSH and TCP_NODELAY are mutually exclusive on this
     * platform, e.g. Linux < 2.6. */

    STL_TRY( stlSetSocketOption( gSock,
                                 STL_SOPT_TCP_NOPUSH,
                                 0,
                                 &sErrStack )
             == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlCloseSocket() 함수 테스트
 */
stlBool TestCloseSocket()
{
    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlCloseSocket(gSock, &sErrStack ) == STL_SUCCESS );

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
     * Test CreateSocket
     */
    sState = 1;
    STL_TRY(TestCreateSocket() == STL_TRUE);

    /*
     * Test SetKeepAlive
     */
    sState = 2;
    STL_TRY(TestSetKeepAlive() == STL_TRUE);

    /*
     * Test SetDebug
     */
    sState = 3;
    STL_TRY(TestSetDebug() == STL_TRUE);

    /*
     * Test RemoveKeepAlive
     */
    sState = 4;
    STL_TRY(TestRemoveKeepAlive() == STL_TRUE);

    /*
     * Test Corkable
     */
    sState = 5;
    STL_TRY(TestCorkable() == STL_TRUE);

    /*
     * Test CloseSocket
     */
    sState = 6;
    STL_TRY(TestCloseSocket() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
