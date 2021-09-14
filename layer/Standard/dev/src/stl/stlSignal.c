/*******************************************************************************
 * stlSignal.c
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
 * @file stlSignal.c
 * @brief Standard Layer Signal Routines
 */

#include <stlDef.h>
#include <stx.h>

/**
 * @addtogroup stlSignal
 * @{
 */

/**
 * @brief Signal을 특정 프로세스에 보낸다.
 * @param[in] aProc Signal을 받을 프로세스
 * @param[in] aSignal 보낼 Signal의 종류
 * @param[out] aErrorStack 에러 스택
 * @see aSignal : stlSignalNumber
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     aSignal에 잘못된 Signal 번호를 지정한 경우에 발생함.
 * [STL_ERRCODE_SIGNAL_KILL]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlKillProc( stlProc       * aProc,
                              stlInt32        aSignal,
                              stlErrorStack * aErrorStack )
{
    return stxKillProc( aProc, aSignal, aErrorStack );
}

/**
 * @brief Signal Handler를 등록한다.
 * @param[in] aSignalNo 등록될 Signal 종류
 * @param[in] aNewFunc 등록할 새로운 Signal Handler
 * @param[in] aOldFunc 기존 Signal Handler
 * @param[out] aErrorStack 에러 스택
 * @see aSignalNo : stlSignalNumber
 * @remark aOldFunc이 NULL일수 있다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     aSignal에 잘못된 Signal 번호를 지정한 경우에 발생함.
 * [STL_ERRCODE_SIGNAL_SET_HANDLER]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlSetSignalHandler( stlInt32        aSignalNo,
                                      stlSignalFunc   aNewFunc,
                                      stlSignalFunc * aOldFunc,
                                      stlErrorStack * aErrorStack )
{
    return stxSetSignalHandler( aSignalNo, aNewFunc, aOldFunc, aErrorStack );
}

/**
 * @brief Extended Signal Handler를 등록한다.
 * @param[in] aSignalNo 등록될 Signal 종류
 * @param[in] aNewFunc 등록할 새로운 Signal Handler
 * @param[in] aOldFunc 기존 Signal Handler
 * @param[out] aErrorStack 에러 스택
 * @see aSignalNo : stlSignalNumber
 * @remark aOldFunc이 NULL일수 있다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     aSignal에 잘못된 Signal 번호를 지정한 경우에 발생함.
 * [STL_ERRCODE_SIGNAL_SET_HANDLER]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlSetSignalHandlerEx( stlInt32          aSignalNo,
                                        stlSignalExFunc   aNewFunc,
                                        stlSignalExFunc * aOldFunc,
                                        stlErrorStack   * aErrorStack )
{
    return stxSetSignalHandlerEx( aSignalNo, aNewFunc, aOldFunc, aErrorStack );
}

/**
 * @brief 자신의 프로세스로 들어오는 Signal을 차단한다.
 * @param[in] aSignalNo 차단할 Signal 종류
 * @param[out] aErrorStack 에러 스택
 * @see aSignalNo : stlSignalNumber
 * @remark
 *     STL_SIGNAL_KILL과 STL_SIGNAL_STOP은 차단시킬수 없으며 이를 무시한다.
 *     차단된 시스널은 stlUnblockSignal이 호출될때까지 대기한다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SIGNAL_BLOCK]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlBlockSignal( stlInt32        aSignalNo,
                                 stlErrorStack * aErrorStack )
{
    return stxBlockSignal( aSignalNo, aErrorStack );
}

/**
 * @brief 자신의 프로세스로 들어오는 Signal을 허용한다.
 * @param[in] aSignalNo 허용할 Signal 종류
 * @param[out] aErrorStack 에러 스택
 * @see aSignalNo : stlSignalNumber
 * @remark
 *     만약 대기하고 있던 Signal이 있다면 Signal Handler가 호출된다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SIGNAL_BLOCK]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlUnblockSignal( stlInt32        aSignalNo,
                                   stlErrorStack * aErrorStack )
{
    return stxUnblockSignal( aSignalNo, aErrorStack );
}

/**
 * @brief 자신의 프로세스로 들어오는 Controllable Signal 들을 차단한다.
 * @param[in] aExclSignalNo Block에서 제외해야 하는 시그날
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SIGNAL_BLOCK]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlBlockControllableSignals( stlInt32        aExclSignalNo,
                                              stlErrorStack * aErrorStack )
{
    if( aExclSignalNo != STL_SIGNAL_HUP )
    {
        STL_TRY( stlBlockSignal( STL_SIGNAL_HUP, aErrorStack ) == STL_SUCCESS );
    }
    
    if( aExclSignalNo != STL_SIGNAL_TERM )
    {
        STL_TRY( stlBlockSignal( STL_SIGNAL_TERM, aErrorStack ) == STL_SUCCESS );
    }
    
    if( aExclSignalNo != STL_SIGNAL_QUIT )
    {
        STL_TRY( stlBlockSignal( STL_SIGNAL_QUIT, aErrorStack ) == STL_SUCCESS );
    }

    if( aExclSignalNo != STL_SIGNAL_ABRT )
    {
        STL_TRY( stlBlockSignal( STL_SIGNAL_ABRT, aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 자신의 프로세스로 들어오는 Controllable Signal들을 허용한다.
 * @param[in] aExclSignalNo Unblock에서 제외해야 하는 시그날
 * @param[out] aErrorStack 에러 스택
 * @remark
 *     만약 대기하고 있던 Signal이 있다면 Signal Handler가 호출된다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SIGNAL_BLOCK]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlUnblockControllableSignals( stlInt32        aExclSignalNo,
                                                stlErrorStack * aErrorStack )
{
    if( aExclSignalNo != STL_SIGNAL_HUP )
    {
        STL_TRY( stlUnblockSignal( STL_SIGNAL_HUP, aErrorStack ) == STL_SUCCESS );
    }
    
    if( aExclSignalNo != STL_SIGNAL_TERM )
    {
        STL_TRY( stlUnblockSignal( STL_SIGNAL_TERM, aErrorStack ) == STL_SUCCESS );
    }
    
    if( aExclSignalNo != STL_SIGNAL_QUIT )
    {
        STL_TRY( stlUnblockSignal( STL_SIGNAL_QUIT, aErrorStack ) == STL_SUCCESS );
    }

    if( aExclSignalNo != STL_SIGNAL_ABRT )
    {
        STL_TRY( stlUnblockSignal( STL_SIGNAL_ABRT, aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 자신의 쓰레드로 들어오는 Signal을 차단한다.
 * @param[in] aSignalNo 차단할 Signal Array
 * @param[in] aSignalCount 차단할 Signal의 개수
 * @param[out] aErrorStack 에러 스택
 * @see aSignalNo : stlSignalNumber
 * @remark
 *     STL_SIGNAL_KILL과 STL_SIGNAL_STOP은 차단시킬수 없으며 이를 무시한다.
 *     차단된 시스널은 stlUnblockSignal이 호출될때까지 대기한다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SIGNAL_BLOCK]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlBlockThreadSignals( stlInt32      * aSignalNo,
                                        stlInt32        aSignalCount,
                                        stlErrorStack * aErrorStack )
{
    return stxBlockThreadSignals( aSignalNo, aSignalCount, aErrorStack );
}

/**
 * @brief 자신의 프로세스로 들어오는 Signal을 허용한다.
 * @param[in] aSignalNo 허용할 Signal Array
 * @param[in] aSignalCount 허용할 Signal의 개수
 * @param[out] aErrorStack 에러 스택
 * @see aSignalNo : stlSignalNumber
 * @remark
 *     만약 대기하고 있던 Signal이 있다면 Signal Handler가 호출된다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SIGNAL_BLOCK]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlUnblockThreadSignals( stlInt32      * aSignalNo,
                                          stlInt32        aSignalCount,
                                          stlErrorStack * aErrorStack )
{
    return stxUnblockThreadSignals( aSignalNo, aSignalCount, aErrorStack );
}

/**
 * @brief 자신의 프로세스로 들어오는 Controllable Signal 들을 차단한다.
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SIGNAL_BLOCK]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlBlockControllableThreadSignals( stlErrorStack * aErrorStack )
{
    stlInt32 sBlockSignals[5] = { STL_SIGNAL_HUP,
                                  STL_SIGNAL_TERM,
                                  STL_SIGNAL_QUIT,
                                  STL_SIGNAL_ABRT };
    
    return stxBlockThreadSignals( sBlockSignals, 5, aErrorStack );
}

/**
 * @brief 자신의 프로세스로 들어오는 Controllable Signal들을 허용한다.
 * @param[out] aErrorStack 에러 스택
 * @remark
 *     만약 대기하고 있던 Signal이 있다면 Signal Handler가 호출된다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SIGNAL_BLOCK]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlUnblockControllableThreadSignals( stlErrorStack * aErrorStack )
{
    stlInt32 sBlockSignals[5] = { STL_SIGNAL_HUP,
                                  STL_SIGNAL_TERM,
                                  STL_SIGNAL_QUIT,
                                  STL_SIGNAL_ABRT };
    
    return stxUnblockThreadSignals( sBlockSignals, 5, aErrorStack );
}

/**
 * @brief 자신의 쓰레드로 들어오는 Real-time Signal을 차단한다.
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SIGNAL_BLOCK]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlBlockRealtimeThreadSignals( stlErrorStack * aErrorStack )
{
   stlInt32 sSignalNo = STL_SIGNAL_RTMIN;
   
    STL_TRY( stxBlockThreadSignals( &sSignalNo, 
                                    1,
                                    aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 자신의 쓰레드로 들어오는 Real-time Signal을 허용한다.
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SIGNAL_BLOCK]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlUnblockRealtimeThreadSignals( stlErrorStack * aErrorStack )
{
   stlInt32 sSignalNo = STL_SIGNAL_RTMIN;
   
   STL_TRY( stxUnblockThreadSignals( &sSignalNo, 
                                     1,
                                     aErrorStack )
            == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Signal을 보낸 프로세스 아이디를 얻는다.
 * @param[in]  aSigInfo    Signal Information
 * @param[out] aOriginProc Signal을 보낸 프로세스 아이디
 */
inline void stlGetSignalOriginProcess( void    * aSigInfo,
                                       stlProc * aOriginProc )
{
    stxGetSignalOriginProcess( aSigInfo, aOriginProc );
}
                                       

/** @} */
