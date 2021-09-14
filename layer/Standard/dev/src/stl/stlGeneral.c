/*******************************************************************************
 * stlGeneral.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file stlGeneral.c
 * @brief Standard Layer General Routines
 */

#include <stc.h>
#include <stlDef.h>
#include <stlMemorys.h>
#include <stlSignal.h>
#include <ste.h>
#include <stlStrings.h>
#include <stlGeneral.h>

extern stlErrorRecord * gLayerErrorTables[STL_ERROR_MODULE_MAX];
extern stlErrorRecord   gStandardErrorTable[STL_MAX_ERROR + 1];
stlFatalHandler         gStlFatalHandler;
stlBool                 gStlProcessInitialized[STL_LAYER_MAX] = {STL_FALSE,};

static stlInt32 gStlInitialized = 0;

#ifdef WIN32
extern DWORD gStxTls;
#endif

/**
 * @addtogroup stlGeneral
 * @{
 */

/**
 * @brief Layer String
 */
const stlChar *const gLayerString[] =
{
    "NULL",
    "STANDARD",
    "DATATYPE",
    "COMMUNICAION",
    "KERNEL",
    "SERVER_COMMUNICATION",
    "STORAGE_MANAGER",
    "EXECUTION_LIBRARY",
    "QUERY_PROCESSOR",
    "PSM_PROCESSOR",
    "SESSION_MANAGER",
    "SERVER_LIBRARY",
    "GLIESE_LIBRARY",
    "GLIESE_TOOL",
    "GOLDILOCKS_LIBRARY",
    "GOLDILOCKS_TOOL",
    NULL
};

/**
 * @brief Standard Layer를 초기화한다.
 */
stlStatus stlInitialize()
{
    stlErrorStack   sErrorStack;

#ifdef WIN32
    WSADATA  sWsaData;
    stlInt32 sReturn;
#else
    stlSignalExFunc sOldFunc;
#endif

    if (gStlInitialized != 0)
    {
        STL_THROW(RAMP_SUCCESS);
    }

    gStlInitialized++;

    STL_INIT_ERROR_STACK( &sErrorStack );

    if( stlGetProcessInitialized( STL_LAYER_STANDARD ) == STL_FALSE )
    {
        /* Standard Layer 에러 테이블을 등록한다. */
        gLayerErrorTables[STL_ERROR_MODULE_STANDARD] = gStandardErrorTable;
        gStlFatalHandler = steDefaultFatalHandler;

#ifndef WIN32
        STL_TRY( stlSetSignalHandlerEx( STL_SIGNAL_SEGV,
                                        steFatalHandler,
                                        &sOldFunc,
                                        &sErrorStack )
                 == STL_SUCCESS );

        STL_TRY( stlSetSignalHandlerEx( STL_SIGNAL_FPE,
                                        steFatalHandler,
                                        &sOldFunc,
                                        &sErrorStack )
                 == STL_SUCCESS );

        STL_TRY( stlSetSignalHandlerEx( STL_SIGNAL_ILL,
                                        steFatalHandler,
                                        &sOldFunc,
                                        &sErrorStack )
                 == STL_SUCCESS );

        STL_TRY( stlSetSignalHandlerEx( STL_SIGNAL_BUS,
                                        steFatalHandler,
                                        &sOldFunc,
                                        &sErrorStack )
                 == STL_SUCCESS );

        /**
         * STL_SIGNAL_TSTP( Ctrl + 'Z' ) signal을 무시한다.
         */
        STL_TRY( stlSetSignalHandler( STL_SIGNAL_TSTP,
                                      STL_SIGNAL_IGNORE,
                                      NULL,  /* aOldHandler */
                                      &sErrorStack )
                 == STL_SUCCESS );

        /**
         * STL_SIGNAL_PIPE signal을 무시한다.
         */
        STL_TRY( stlSetSignalHandler( STL_SIGNAL_PIPE,
                                      STL_SIGNAL_IGNORE,
                                      NULL,  /* aOldHandler */
                                      &sErrorStack )
                 == STL_SUCCESS );
#endif
        stlSetProcessInitialized( STL_LAYER_STANDARD );
    }

#ifdef WIN32
    gStxTls = TlsAlloc();

    sReturn = WSAStartup(MAKEWORD(2, 2), &sWsaData);
	
    STL_TRY_THROW(sReturn == 0, RAMP_ERR_WSASTARTUP);

    STL_TRY_THROW((LOBYTE(sWsaData.wVersion) == 2) &&
                  (HIBYTE(sWsaData.wVersion) == 2),
                  RAMP_ERR_NOT_SUPPORT);
#endif

    STL_RAMP(RAMP_SUCCESS);

    return STL_SUCCESS;

#ifdef WIN32
    STL_CATCH(RAMP_ERR_WSASTARTUP)
    {
        stlPrintf("WSAStartup failure. (%d)\n", sReturn);
    }
    
    STL_CATCH(RAMP_ERR_NOT_SUPPORT)
    {
        WSACleanup();

        stlPrintf("WSAStartup failure. (%d)\n", sReturn);
    }
#endif
	
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Standard Layer를 종료한다.
 */
stlStatus stlTerminate()
{
    gStlInitialized--;

    if (gStlInitialized == 0)
    {
#ifdef WIN32
        WSACleanup();

        TlsFree(gStxTls);
#endif
    }

    return STL_SUCCESS;
}

/**
 * @brief 프로세스를 종료한다. Server에서는 호출하지 않는다.
 * @param[in] aStatus 프로세스 종료시 반환되는 상태 코드
 */
void stlExit( stlInt32 aStatus )
{
    exit( aStatus );
}

/**
 * @brief 프로그램이 끝났을 때 호출 될 function을 등록한다.
 * @param[in] aFunction 종료시 호출 될 function
 */
stlStatus stlAtExit( void (*aFunction)(void) )
{
    STL_TRY( atexit( aFunction ) == 0 );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief 해당 레이어에 프로세스가 초기화 되었음을 설정한다.
 * @param[in] aLayerClass Layer Class
 */
void stlSetProcessInitialized( stlLayerClass aLayerClass )
{
    gStlProcessInitialized[aLayerClass] = STL_TRUE;
}

/**
 * @brief 해당 레이어에 프로세스 초기화 여부를 얻는다.
 * @param[in] aLayerClass Layer Class
 * @return 이미 초기화되었다면 STL_TRUE, 그렇지 않다면 STL_FALSE
 */
stlBool stlGetProcessInitialized( stlLayerClass aLayerClass )
{
    return gStlProcessInitialized[aLayerClass];
}

/** @} */
