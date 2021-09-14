/*******************************************************************************
 * stlThreadProc.c
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
 * @file stlThreadProc.c
 * @brief Standard Layer Thread & Process Routines
 */

#include <stlDef.h>
#include <stx.h>

/**
 * @addtogroup stlThreadProc
 * @{
 */

/**
 * @brief 쓰레드 속성 구조체를 초기화 한다.
 * @param[out] aThreadAttr 초기화 시킬 쓰레드 속성 구조체
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_THREAD_ATTR]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlInitThreadAttr( stlThreadAttr * aThreadAttr,
                                    stlErrorStack * aErrorStack )
{
    return stxInitThreadAttr( aThreadAttr, aErrorStack );
}

/**
 * @brief 쓰레드의 스택 크기를 설정한다.
 * @param[out] aThreadAttr 설정시킬 쓰레드 속성 구조체
 * @param[in] aStackSize 설정할 스택 크기
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_THREAD_ATTR]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlSetThreadAttrStackSize( stlThreadAttr * aThreadAttr,
                                            stlSize         aStackSize,
                                            stlErrorStack * aErrorStack )
{
    return stxSetThreadAttrStackSize( aThreadAttr, aStackSize, aErrorStack );
}

/**
 * @brief 새로운 쓰레드를 생성한다.
 * @param[out] aThread 생성된 쓰레드 핸들이 저장될 쓰레드 구조체
 * @param[in] aThreadAttr 쓰레드 속성 구조체
 * @param[in] aThreadFunc 실행시킬 쓰레드 함수
 * @param[in] aArgs aThreadFunc에 들어갈 파라미터 리스트
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_INSUFFICIENT_RESOURCE]
 *     시스템에 정의된 쓰레드 수 제한을 초과하는 경우에 발생함.
 * [STL_ERRCODE_THREAD_CREATE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlCreateThread( stlThread     * aThread,
                                  stlThreadAttr * aThreadAttr,
                                  stlThreadFunc   aThreadFunc,
                                  void          * aArgs,
                                  stlErrorStack * aErrorStack )
{
    return stxCreateThread( aThread, aThreadAttr, aThreadFunc, aArgs, aErrorStack );
}

/**
 * @brief 주어진 2개의 쓰레드가 같은지 확인한다.
 * @param[in] aThread1 비교 대상 첫번째 쓰레드
 * @param[in] aThread2 비교 대상 두번째 쓰레드
 * @return 같으면 STL_TRUE, 그렇지 않으면 STL_FALSE
 */
stlBool stlIsEqualThread( stlThreadHandle * aThread1,
                          stlThreadHandle * aThread2 )
{
    return stxIsEqualThread( aThread1, aThread2 );
}

/**
 * @brief 자신의 쓰레드를 종료한다.
 * @param[out] aThread 종료 상태를 저장할 쓰레드 구조체
 * @param[in] aReturnStatus 저장할 종료 상태
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * 현재는 무조건 STL_SUCCESS를 리턴한다.(향후 win32에서는 에러 발생 가능)
 * @endcode
 */
inline stlStatus stlExitThread( stlThread     * aThread,
                                stlStatus       aReturnStatus,
                                stlErrorStack * aErrorStack )
{
    return stxExitThread( aThread, aReturnStatus, aErrorStack );
}

/**
 * @brief 주어진 쓰레드를 종료한다.
 * @param[out] aThread 종료 상태를 저장할 쓰레드 구조체
 * @param[in] aReturnStatus 저장할 종료 상태
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * 현재는 무조건 STL_SUCCESS를 리턴한다.(향후 win32에서는 에러 발생 가능)
 * @endcode
 */
inline stlStatus stlCancelThread( stlThread     * aThread,
                                  stlStatus       aReturnStatus,
                                  stlErrorStack * aErrorStack )
{
    return stxCancelThread( aThread, aReturnStatus, aErrorStack );
}

/**
 * @brief 자신의 쓰레드를 종료한다.
 * @remarks 자신의 stlThread를 얻을수 없는 상황에서 사용된다.
 */
inline void stlNativeExitThread()
{
    stxNativeExitThread();
}

/**
 * @brief 쓰레드가 조인되기를 기다린다.
 * @param[in] aThread 기다릴 쓰레드
 * @param[out] aReturnStatus 쓰레드 종료할때 설정된 종료상태값
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_THREAD_JOIN]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlJoinThread( stlThread     * aThread,
                                stlStatus     * aReturnStatus,
                                stlErrorStack * aErrorStack )
{
    return stxJoinThread( aThread, aReturnStatus, aErrorStack );
}

/**
 * @brief 프로세서를 양보한다.
 */
void stlYieldThread( void )
{
    stxYieldThread();
}

/**
 * @brief 자신의 Thread Handle을 얻는다.
 * @param[out] aThreadHandle Thread Handle
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * @endcode
 */
stlStatus stlThreadSelf( stlThreadHandle * aThreadHandle,
                         stlErrorStack   * aErrorStack )
{
    return stxThreadSelf( aThreadHandle, aErrorStack );
}

/**
 * @brief Thread를 특정 CPUs에 Binding 한다.
 * @param[in]    aThreadHandle   Thread Handle
 * @param[in]    aCpuSet         Binding할 CPU set
 * @param[out]   aErrorStack     에러 스택
 * @par Error Codes:
 * @code
 * @endcode
 */
stlStatus stlSetThreadAffinity( stlThreadHandle * aThreadHandle,
                                stlCpuSet       * aCpuSet,
                                stlErrorStack   * aErrorStack )
{
    return stxSetThreadAffinity( aThreadHandle, aCpuSet, aErrorStack );
}

/**
 * @brief 쓰레드 once를 초기화한다.
 * @param[out] aThreadOnce 초기화할 쓰레드 once 구조체
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * 현재는 무조건 STL_SUCCESS를 리턴한다.(향후 win32에서는 에러 발생 가능)
 * @endcode
 */
inline stlStatus stlInitThreadOnce( stlThreadOnce * aThreadOnce,
                                    stlErrorStack * aErrorStack )
{
    return stxInitThreadOnce( aThreadOnce, aErrorStack );
}

/**
 * @brief 쓰레드 once 함수를 설정한다.
 * @param[in] aThreadOnce 쓰레드 once 구조체
 * @param[in] aInitFunc 쓰레드 once 초기화 함수
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_THREAD_ONCE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlSetThreadOnce( stlThreadOnce * aThreadOnce, 
                                   void         (* aInitFunc)(void),
                                   stlErrorStack * aErrorStack )
{
    return stxThreadOnce( aThreadOnce, aInitFunc, aErrorStack );
}

/**
 * @brief 새로운 프로세스를 생성한다. (fork & exec)
 * @param[in] aPath 실행 이미지 경로
 * @param[in] aArgv 실행 이미지의 파라미터 리스트
 * @param[out] aExitCode 프로세스 종료할때 설정된 종료상태값
 * @param[out] aErrorStack 에러 스택
 */
inline stlStatus stlCreateProc( const stlChar * aPath,
                                stlChar * const aArgv[],
                                stlInt32 *      aExitCode,                                
                                stlErrorStack * aErrorStack )
{
    return stxCreateProc( aPath, aArgv, aExitCode, aErrorStack );
}

/**
 * @brief 새로운 프로세스를 생성한다.
 * @param[out] aProc 생성된 프로세스 핸들이 저장될 구조체
 * @param[out] aIsChild Child 프로세스인지 여부
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     커널 메모리를 할당받을수 없는 경우에 발생함.
 * [STL_ERRCODE_INSUFFICIENT_RESOURCE]
 *     시스템에 정의된 프로세스 제한을 초과하는 경우에 발생함.
 * @endcode
 */
inline stlStatus stlForkProc( stlProc       * aProc,
                              stlBool       * aIsChild,
                              stlErrorStack * aErrorStack )
{
    return stxForkProc( aProc, aIsChild, aErrorStack );
}

/**
 * @brief 주어진 프로세스를 기다린다.
 * @param[in] aProc 기다릴 대상 프로세스
 * @param[out] aExitCode 프로세스 종료할때 설정된 종료상태값
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_PROC_WAIT]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlWaitProc( stlProc       * aProc,
                              stlInt32      * aExitCode,
                              stlErrorStack * aErrorStack )
{
    return stxWaitProc( aProc, aExitCode, aErrorStack );
}

/**
 * @brief Any Child 프로세스의 끝남을 기다린다.
 * @param[out] aExitCode 프로세스 종료할때 설정된 종료상태값
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_PROC_WAIT]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlWaitAnyProc( stlInt32      * aExitCode,
                                 stlErrorStack * aErrorStack )
{
    return stxWaitAnyProc( aExitCode, aErrorStack );
}

/**
 * @brief 현재 프로세스 이미지를 주어진 실행 이미지로 교체한다.
 * @param[in] aPath 실행 이미지 경로
 * @param[in] aArgv 실행 이미지의 파라미터 리스트
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_ACCESS]
 *     실행 권한 또는 레귤라 파일이 아닐 경우에 발생함.
 * [STL_ERRCODE_NO_ENTRY]
 *     aPath에 지정한 실행 이미지를 찾을수 없는 경우에 발생함.
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     OS 커널 메모리가 부족한 경우에 발생함.
 * [STL_ERRCODE_INSUFFICIENT_RESOURCE]
 *     시스템에서 열수 있는 파일 개수의 제한을 넘어선 경우에 발생함.
 * [STL_ERRCODE_PROC_EXECUTE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 * @par Example:
 * @code
 *     stlChar * sArgv = { "/경로/gcc", "-c", "test.c" };
 *
 *     stlExecuteProc( sArgv[0], sArgv, &sErrorStack );
 * @endcode
 */
inline stlStatus stlExecuteProc( const stlChar * aPath,
                                 stlChar * const aArgv[],
                                 stlErrorStack * aErrorStack )
{
    return stxExecuteProc( aPath, aArgv, aErrorStack );
}

/**
 * @brief Process를 실행하고 결과를 기다린다.
 * @param[in] aArgv         실행 이미지의 파라미터 리스트
 * @param[in] aErrorStack   에러 스택
 * @remark aArgv는 최대 STX_MAX_PROCESS_ARGS - 2개 까지 가능하다.\n
 *         실행되는 process에 '--key=xxx' argument가 추가로 붙여진다.
 *         실행되는 process에서 관련처리를 해주어야 한다.
 *         (process가 실패하면 STL_FAILURE 리턴 하고 stlSendErrorMsgQueue 호출)
 */
stlStatus stlExecuteProcAndWait( stlChar        * const aArgv[],
                                 stlErrorStack  * aErrorStack )
{
    return stxExecuteProcAndWait( aArgv, aErrorStack );
}

/**
 * @brief Process 실행에 대한 결과를 전송한다.
 * @param[in] aMsgKey       전송할 msg key
 * @param[in] aErrorStack   전송할 에러 스택
 * @remark stlExecuteProcAndWait참고
 */
stlStatus stlWakeupExecProc( stlInt64          aMsgKey,
                             stlErrorStack   * aErrorStack )
{
    return stxWakeupExecProc( aMsgKey, aErrorStack );
}

/**
 * @brief 주어진 2개의 PROCESS가 같은지 확인한다.
 * @param[in] aProc1 비교 대상 첫번째 PROCESS
 * @param[in] aProc2 비교 대상 두번째 PROCESS
 * @return 같으면 STL_TRUE, 그렇지 않으면 STL_FALSE
 */
stlBool stlIsEqualProc( stlProc * aProc1,
                        stlProc * aProc2 )
{
    return ( aProc1->mProcID == aProc2->mProcID ) ? STL_TRUE : STL_FALSE;
}

/**
 * @brief 현재 프로세스를 종료한다.
 * @param[in] aStatus 부모 프로세스에 전달될 값
 * @par Error Codes:
 * @code
 *      지금은 에러가 발생되지 않는다.
 * @endcode
 */
inline stlStatus stlExitProc( stlInt32 aStatus )
{
    exit( aStatus );
    return STL_SUCCESS;
}

/**
 * @brief 현재 프로세스의 정보를 읽어서 @a aProc에 채운다.
 * @param[out] aProc 정보가 채워질 프로세스 구조체
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 *      지금은 에러가 발생되지 않는다.
 * @endcode
 */
inline stlStatus stlGetCurrentProc( stlProc       * aProc,
                                    stlErrorStack * aErrorStack )
{
    return stxGetCurrentProc( aProc, aErrorStack );
}

/**
 * @brief 부모 프로세스의 정보를 읽어서 @a aProc에 채운다.
 * @param[out] aProc 정보가 채워질 프로세스 구조체
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 *      지금은 에러가 발생되지 않는다.
 * @endcode
 */
inline stlStatus stlGetParentProc( stlProc       * aProc,
                                   stlErrorStack * aErrorStack )
{
    return stxGetParentProc( aProc, aErrorStack );
}

/**
 * @brief 주어진 프로세스가 존재하는지 확인한다.
 * @param[in] aProc 존재 여부를 확인할 프로세스 구조체
 * @note 시스템콜을 사용하기 때문에 호출시 성능 저하가 발생할 수 있다.
 */
inline stlBool stlIsActiveProc( stlProc * aProc )
{
    return stxIsExistProc( aProc );
}

/**
 * @brief 주어진 프로세스의 이름을 반환한다.
 * @param[in] aProc 프로세스 이름을 확인할 프로세스 구조체
 * @param[out] aName 프로세스 이름
 * @param[in] aSize aName의 크기
 * @param[out] aErrorStack 에러 스택
 */
inline stlStatus stlGetProcName( stlProc       * aProc,
                                 stlChar       * aName,
                                 stlSize         aSize,
                                 stlErrorStack * aErrorStack )
{
    return stxGetProcName( aProc, aName, aSize, aErrorStack );
}

/**
 * @brief 실행 Command 에서 argument 정보를 감춤 처리한다.
 * @param[in] aArgc  Command argument count
 * @param[in] aArgv  Command argument list
 * @param[out] aErrorStack 에러 스택
 * @param[in] aCount 감출 argument의 개수
 * @param[in] ...    감출 argument의 idx list
 * @remark  Command list에서 지정된 argument 들을 감춤 처리한다.\n
 *          만약 1번 argument를 감추고 싶다면
 *           stlHideArgument( aArgc, aArgv, aErrorStack, 1, 1 )과 같이 호출하고,
 *          1, 2번 argument를 감추고 싶다면
 *           stlHideArgument( aArgc, aArgv, aErrorStack, 2, 1, 2 )와 같이 호출한다.
 *          가변 인자로 주어지는 것들은 모두 int type이 되어야만 한다.
 */
inline stlStatus stlHideArgument( stlInt32        aArgc,
                                  stlChar       * aArgv[],
                                  stlErrorStack * aErrorStack,
                                  stlInt32        aCount,
                                  ... )
{
    stlStatus sReturn;
    va_list   sVarArgList;

    va_start(sVarArgList, aCount);
    sReturn = stxHideArgument( aArgc, aArgv, aErrorStack, aCount, sVarArgList );
    va_end(sVarArgList);

    return sReturn;
}

/** @} */
