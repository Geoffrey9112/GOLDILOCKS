/*******************************************************************************
 * qllGeneral.c
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
 * @file qllGeneral.c
 * @brief SQL Processor Startup Routines
 */

#include <qll.h>
#include <qlDef.h>
#include <qlgStartup.h>

/**
 * @addtogroup qllProcEnv
 * @{
 */

extern stlErrorRecord gSqlProcessorLibraryErrorTable[QLL_MAX_ERROR + 1];
extern stlFatalHandler  gQlgOldFatalHandler;

knlStartupFunc gStartupQP[KNL_STARTUP_PHASE_MAX] = 
{
    NULL,
    qlgStartupNoMount,
    qlgStartupMount,
    qlgStartupPreOpen,
    qlgStartupOpen
};

knlWarmupFunc gWarmupQP = qlgWarmup;

knlShutdownFunc gShutdownQP[KNL_SHUTDOWN_PHASE_MAX] =
{
    NULL,
    qlgShutdownInit,
    qlgShutdownDismount,
    qlgShutdownPostClose,
    qlgShutdownClose
};

knlCooldownFunc    gCooldownQP = qlgCooldown;

/**
 * @brief SQL Processor를 초기화한다.
 * @remark 프로세스당 한번 호출되어야 한다.
 */
stlStatus qllInitialize()
{
    if( stlGetProcessInitialized( STL_LAYER_SQL_PROCESSOR ) == STL_FALSE )
    {
        /**
         * 하위 Layer 초기화
         */
    
        STL_TRY( ellInitialize() == STL_SUCCESS );

        /**
         * FATAL Handler 등록
         */

        STL_TRY( stlHookFatalHandler( qlgFatalHandler,
                                      & gQlgOldFatalHandler )
                 == STL_SUCCESS );
    
        /**
         * 에러 테이블 등록
         */

        stlRegisterErrorTable( STL_ERROR_MODULE_SQL_PROCESSOR,
                               gSqlProcessorLibraryErrorTable );

        stlSetProcessInitialized( STL_LAYER_SQL_PROCESSOR );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Sql Processor 의 Process Envrionment를 초기화 한다.
 * @param[out] aEnv     초기화될 Process Environment 포인터
 * @param[in]  aEnvType Env Type
 */
stlStatus qllInitializeEnv( qllEnv     * aEnv,
                            knlEnvType   aEnvType )
{
    stlInt32 sState = 0;
    
    /**
     * 하위 layer 의 Process Environment 초기화
     */
    
    STL_TRY( ellInitializeEnv( (ellEnv*)aEnv, aEnvType ) == STL_SUCCESS );
    sState = 1;

    /**
     * Message 영역 초기화
     */

    stlMemset( aEnv->mMessage, 0x00, STL_MAX_ERROR_MESSAGE );

    /**********************************************
     * Heap Memory
     **********************************************/
    
    /**
     * SQL Processor 의 Parsing 을 위한 메모리 관리자를 초기화
     */

    STL_TRY( knlCreateRegionMem( & aEnv->mHeapMemParsing,
                                 KNL_ALLOCATOR_SQL_PROCESSOR_PARSING,
                                 NULL,  /* aParentMem */
                                 KNL_MEM_STORAGE_TYPE_HEAP,
                                 QLL_PARSING_MEM_INIT_SIZE,
                                 QLL_PARSING_MEM_NEXT_SIZE,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;

    /**
     * SQL Processor 의 Running 을 위한 메모리 관리자를 초기화
     */

    STL_TRY( knlCreateRegionMem( & aEnv->mHeapMemRunning,
                                 KNL_ALLOCATOR_SQL_PROCESSOR_RUNNING,
                                 NULL,  /* aParentMem */
                                 KNL_MEM_STORAGE_TYPE_HEAP,
                                 QLL_RUNNING_MEM_INIT_SIZE,
                                 QLL_RUNNING_MEM_NEXT_SIZE,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 3;
    
    
    /**
     * SQL Process Env 의 Fixed Table 변수 초기화
     */

    aEnv->mParsingCnt     = 0;
    aEnv->mValidateCnt    = 0;
    aEnv->mBindContextCnt = 0;
    aEnv->mOptCodeCnt     = 0;
    aEnv->mOptDataCnt     = 0;
    aEnv->mRecheckCnt     = 0;
    aEnv->mExecuteCnt     = 0;
    aEnv->mRecompileCnt   = 0;
    aEnv->mFetchCnt       = 0;

    stlMemset( aEnv->mExecStmtCnt,
               0x00,
               STL_SIZEOF(stlInt64) * (QLL_STMT_MAX - QLL_STMT_NA) );

    STL_TRY( qllSetDTFuncVector( aEnv ) == STL_SUCCESS );
    
    /**
     * Environment에 Top Layer 설정
     */
    
    knlSetTopLayer( STL_LAYER_SQL_PROCESSOR, KNL_ENV( aEnv ) );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void) knlDestroyRegionMem( & aEnv->mHeapMemRunning, KNL_ENV(aEnv) );
        case 2:
            (void) knlDestroyRegionMem( & aEnv->mHeapMemParsing, KNL_ENV(aEnv) );
        case 1:
            (void) ellFinalizeEnv( ELL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Sql Processor 의 Process Envrionment를 종료한다.
 * @param[in] aEnv 종료할 Process Environment 포인터
 */
stlStatus qllFinalizeEnv( qllEnv * aEnv )
{
    /**********************************************
     * Heap Memory
     **********************************************/
    
    /**
     * Parsing 을 위한 메모리 관리자를 종료
     */
    
    STL_TRY( knlDestroyRegionMem( & aEnv->mHeapMemParsing,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Running 을 위한 메모리 관리자를 종료
     */
    
    STL_TRY( knlDestroyRegionMem( & aEnv->mHeapMemRunning,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * 하위 layer 의 Process Environment 를 종료
     */
    
    STL_TRY( ellFinalizeEnv( (ellEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NOMOUNT 단계용 Performance View 구축
 * @param[in] aEnv Environment
 * @remarks
 */
stlStatus qllBuildPerfViewNoMount( qllEnv * aEnv )
{
    /**
     * NOMOUNT 단계 Performance View 구축
     */
    
    STL_TRY( qlgBuildNonServicePerfView( KNL_STARTUP_PHASE_NO_MOUNT, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NOMOUNT 단계용 Performance View 구축
 * @param[in] aEnv Environment
 * @remarks
 */
stlStatus qllBuildPerfViewMount( qllEnv * aEnv )
{
    /**
     * NOMOUNT 단계 Performance View 구축
     */
    
    STL_TRY( qlgBuildNonServicePerfView( KNL_STARTUP_PHASE_NO_MOUNT, aEnv ) == STL_SUCCESS );

    /**
     * MOUNT 단계 Performance View 구축
     */
    
    STL_TRY( qlgBuildNonServicePerfView( KNL_STARTUP_PHASE_MOUNT, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qllProcEnv */






/**
 * @addtogroup qllSessEnv
 * @{
 */


/**
 * @brief Sql Processor 의 Session Environment 를 초기화한다.
 * @param[in] aSessEnv     Sql Processor 의 Session Environment
 * @param[in] aConnectionType Session Connection Type
 * @param[in] aSessType    Session Type
 * @param[in] aSessEnvType Session Envrionment Type
 * @param[in] aEnv         Sql Processor 의 Process Environment
 * @remarks
 */
stlStatus qllInitSessionEnv( qllSessionEnv     * aSessEnv,
                             knlConnectionType   aConnectionType,
                             knlSessionType      aSessType,
                             knlSessEnvType      aSessEnvType,
                             qllEnv            * aEnv )
{
    stlInt32         sState = 0;
    
    stlUInt32        sMemType = 0;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSessEnv != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 하위 Layer 의 Session Env 를 초기화
     */
    
    STL_TRY( ellInitSessionEnv( (ellSessionEnv *) aSessEnv,
                                aConnectionType,
                                aSessType,
                                aSessEnvType,
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Collision 관리를 위한 메모리 관리자를 초기화
     * - XA associate/dissociate 를 위해 Shared Memory 상에 유지되어야 함.
     */

    sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
    switch (aSessEnvType)
    {
        case KNL_SESS_ENV_HEAP:
            {
                sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
                break;
            }
        case KNL_SESS_ENV_SHARED:
            {
                if( knlIsAdminSession( aSessEnv ) == STL_TRUE )
                {
                    sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
                }
                else
                {
                    sMemType = KNL_MEM_STORAGE_TYPE_SHM;
                }
                break;
            }
        default:
            {
                STL_DASSERT( 0 );
                break;
            }
    }
    
    STL_TRY( knlCreateDynamicMem( & aSessEnv->mMemCollision,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_SQL_PROCESSOR_COLLISION,
                                  sMemType,
                                  QLL_COLLISION_MEM_INIT_SIZE,
                                  QLL_COLLISION_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;

    /**
     * 메모리 유형 결정
     * - Dedicated Session 일 경우, Init/Code Plan 과 Fetch Memory 를 공유할 필요가 없다.
     * - Shared Session 일 경우, Init/Code Plan 과 Fetch Memory 는 공유할 수 있다.
     */

    sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
    switch (aSessEnvType)
    {
        case KNL_SESS_ENV_HEAP:
            {
                sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
                break;
            }
        case KNL_SESS_ENV_SHARED:
            {
                /**
                 * admin session은 shared memory를 사용하지 않는다.
                 * - static memory가 부족한 상황에서도 세션을 사용할수
                 *   있어야 한다.
                 */
                if( knlIsAdminSession( aSessEnv ) == STL_TRUE )
                {
                    sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
                }
                else
                {
                    switch (aSessType)
                    {
                        case KNL_SESSION_TYPE_DEDICATE:
                            sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
                            break;
                        case KNL_SESSION_TYPE_SHARED:
                            sMemType = KNL_MEM_STORAGE_TYPE_SHM;
                            break;
                        default:
                            STL_DASSERT( 0 );
                            break;
                    }
                }
                break;
            }
        default:
            {
                STL_DASSERT( 0 );
                break;
            }
    }
    
    STL_TRY( knlCreateDynamicMem( & aSessEnv->mMemNLS,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_SQL_PROCESSOR_NLS,
                                  sMemType,
                                  QLL_NLS_MEM_INIT_SIZE,
                                  QLL_NLS_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 3;

    STL_TRY( knlCreateDynamicMem( & aSessEnv->mMemParentDBC,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_SQL_PROCESSOR_PARENT_DBC,
                                  sMemType,
                                  QLL_PARENT_DBC_MEM_INIT_SIZE,
                                  QLL_PARENT_DBC_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 4;
    
    
    /**
     * 멤버 변수를 초기화
     */

    aSessEnv->mAccessMode = QLL_ACCESS_MODE_READ_WRITE;

    aSessEnv->mUniqueMode = QLL_UNIQUE_MODE_DEFAULT;
    qlrcInitDeferConstraintInfo( aSessEnv );
    
    stlMemset( & aSessEnv->mSessNLS, 0x00, STL_SIZEOF(qllSessNLS) );
    
    /**
     * Trace Logger 정보 초기화
     */

    KNL_INIT_TRACE_LOGGER( &(aSessEnv->mTraceLogger) );
    aSessEnv->mTraceLoggerInited = STL_FALSE;
    aSessEnv->mTraceLoggerEnabled = STL_FALSE;


    /**
     * SQL Session Env 의 Fixed Table 변수 초기화 
     */

    aSessEnv->mParsingCnt     = 0;
    aSessEnv->mValidateCnt    = 0;
    aSessEnv->mBindContextCnt = 0;
    aSessEnv->mOptCodeCnt     = 0;
    aSessEnv->mOptDataCnt     = 0;
    aSessEnv->mRecheckCnt     = 0;
    aSessEnv->mExecuteCnt     = 0;
    aSessEnv->mRecompileCnt   = 0;
    aSessEnv->mFetchCnt       = 0;

    stlMemset( aSessEnv->mExecStmtCnt,
               0x00,
               STL_SIZEOF(stlInt64) * (QLL_STMT_MAX - QLL_STMT_NA) );
    
    knlSetSessionTopLayer( (knlSessionEnv*)aSessEnv, STL_LAYER_SQL_PROCESSOR );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 4:
            (void) knlDestroyDynamicMem( & aSessEnv->mMemParentDBC,
                                         KNL_ENV(aEnv) );
        case 3: 
            (void) knlDestroyDynamicMem( & aSessEnv->mMemNLS,
                                         KNL_ENV(aEnv) );
        case 2:
            (void) knlDestroyDynamicMem( & aSessEnv->mMemCollision,
                                         KNL_ENV(aEnv) );
        case 1:
            (void) ellFiniSessionEnv( (ellSessionEnv *) aSessEnv,
                                      ELL_ENV(aEnv) );
        default:
            break;
    }
    
    return STL_FAILURE;
}




/**
 * @brief Sql Processor 의 Session Environment 를 종료한다.
 * @param[in] aSessEnv    Sql Processor 의 Session Environment
 * @param[in] aEnv        Sql Processor 의 Process Environment
 * @remarks
 */
stlStatus qllFiniSessionEnv( qllSessionEnv * aSessEnv,
                             qllEnv        * aEnv )
{
    stlInt32        sState   = 0;
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSessEnv != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 수행 통계 정보 누적
     */

    sState = 1;
    qllAddSessStat2SystemStat( aSessEnv );
        
    /**
     * SQL-Session Object 들을 정리함
     */

    sState = 2;
    STL_TRY( qllFiniSQLSessionObject( aEnv ) == STL_SUCCESS );
    
    /**
     * Trace Logger 종료
     */

    sState = 3;
    if( aSessEnv->mTraceLoggerInited == STL_TRUE )
    {
        STL_TRY( knlDestroyProcessLogger( &(aSessEnv->mTraceLogger),
                                          KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    aSessEnv->mTraceLoggerInited  = STL_FALSE;
    aSessEnv->mTraceLoggerEnabled = STL_FALSE;

    /**
     * Deferred Constraint 을 위한 메모리 관리자를 종료
     */

    sState = 4;
    STL_TRY( qllFiniSessNLSFromProperty( aEnv ) == STL_SUCCESS );
    
    knlValidateDynamicMem( & aSessEnv->mMemCollision, KNL_ENV(aEnv) );

    sState = 5;
    STL_TRY( knlDestroyDynamicMem( & aSessEnv->mMemCollision,
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sState = 6;
    knlValidateDynamicMem( & aSessEnv->mMemNLS, KNL_ENV(aEnv) );
    STL_TRY( knlDestroyDynamicMem( & aSessEnv->mMemNLS,
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );

    knlValidateDynamicMem( & aSessEnv->mMemParentDBC, KNL_ENV(aEnv) );

    sState = 7;
    STL_TRY( knlDestroyDynamicMem( & aSessEnv->mMemParentDBC,
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * 하위 Layer 의 Session Env 를 종료
     */

    sState = 8;
    STL_TRY( ellFiniSessionEnv( (ellSessionEnv*) aSessEnv,
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2 :
            if( aSessEnv->mTraceLoggerInited == STL_TRUE )
            {
                (void) knlDestroyProcessLogger( &(aSessEnv->mTraceLogger),
                                                KNL_ENV(aEnv) );
            }
            aSessEnv->mTraceLoggerInited  = STL_FALSE;
            aSessEnv->mTraceLoggerEnabled = STL_FALSE;
        case 3 :
            (void) qllFiniSessNLSFromProperty( aEnv );
        case 4 :
            (void) knlDestroyDynamicMem( & aSessEnv->mMemCollision,
                                         KNL_ENV(aEnv) );
        case 5 :
            (void) knlDestroyDynamicMem( & aSessEnv->mMemNLS,
                                         KNL_ENV(aEnv) );
        case 6 :
            (void) knlDestroyDynamicMem( & aSessEnv->mMemParentDBC,
                                         KNL_ENV(aEnv) );
        case 7 :
            (void) ellFiniSessionEnv( (ellSessionEnv*) aSessEnv,
                                      ELL_ENV(aEnv) );
        default :
            {
                break;
            }
    }

    return STL_FAILURE;
}


/**
 * @brief Session Fatal시 Sql Processor 의 Session Environment 를 정리한다.
 * @param[in] aSessEnv    Sql Processor 의 Session Environment
 * @param[in] aEnv        Sql Processor 의 Process Environment
 * @remarks
 */
stlStatus qllCleanupSessionEnv( qllSessionEnv * aSessEnv,
                                qllEnv        * aEnv )
{
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSessEnv != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 수행 통계 정보 누적
     */

    qllAddSessStat2SystemStat( aSessEnv );
        
    /**
     * Deferred Constraint 을 위한 메모리 관리자를 종료
     */

    if( KNL_DYNAMIC_MEMORY_TYPE( &aSessEnv->mMemCollision ) == KNL_MEM_STORAGE_TYPE_SHM )
    {
        STL_TRY( knlDestroyDynamicMem( & aSessEnv->mMemCollision,
                                       KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    if( KNL_DYNAMIC_MEMORY_TYPE( &aSessEnv->mMemNLS ) == KNL_MEM_STORAGE_TYPE_SHM )
    {
        STL_TRY( knlDestroyDynamicMem( & aSessEnv->mMemNLS,
                                       KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**
     * Parent DBC Memory 를 제거하여 SQL-Cursor 등을 위해 내부적으로 사용되었던 DBC 메모리등이 모두 정리된다.
     */
    if( KNL_DYNAMIC_MEMORY_TYPE( &aSessEnv->mMemParentDBC ) == KNL_MEM_STORAGE_TYPE_SHM )
    {
        STL_TRY( knlDestroyDynamicMem( & aSessEnv->mMemParentDBC,
                                       KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Data Type layer 를 위해 Function Vector 를 설정한다.
 * @param[in] aEnv  Environment
 */
stlStatus qllSetDTFuncVector( void * aEnv )
{
    dtlFuncVector   sVector;
    
    /**
     * mGetCharSetIDFunc
     */

    sVector.mGetCharSetIDFunc = qlgGetCharSetIDFunc;
        
    /**
     * mGetGMTOffsetFunc
     */

    sVector.mGetGMTOffsetFunc = qlgGetGMTOffsetFunc;
    
    /**
     * mReallocLongVaryingMemFunc
     */

    sVector.mReallocLongVaryingMemFunc = qlgReallocLongVaryingMemFunc;

    /**
     * mReallocAndMoveLongVaryingMemFunc
     */

    sVector.mReallocAndMoveLongVaryingMemFunc = qlgReallocAndMoveLongVaryingMemFunc;

    /**
     * mGetDateFormatStringFunc
     */

    sVector.mGetDateFormatStringFunc = qlgGetDateFormatStringFunc;

    /**
     * mGetDateFormatInfoFunc
     */

    sVector.mGetDateFormatInfoFunc = qlgGetDateFormatInfoFunc;

    /**
     * mGetTimeFormatStringFunc
     */

    sVector.mGetTimeFormatStringFunc = qlgGetTimeFormatStringFunc;

    /**
     * mGetTimeFormatInfoFunc
     */

    sVector.mGetTimeFormatInfoFunc = qlgGetTimeFormatInfoFunc;

    /**
     * mGetTimeWithTimeZoneFormatStringFunc
     */

    sVector.mGetTimeWithTimeZoneFormatStringFunc = qlgGetTimeWithTimeZoneFormatStringFunc;

    /**
     * mGetTimeWithTimeZoneFormatInfoFunc
     */

    sVector.mGetTimeWithTimeZoneFormatInfoFunc = qlgGetTimeWithTimeZoneFormatInfoFunc;

    /**
     * mGetTimestampFormatStringFunc
     */

    sVector.mGetTimestampFormatStringFunc = qlgGetTimestampFormatStringFunc;

    /**
     * mGetTimestampFormatInfoFunc
     */

    sVector.mGetTimestampFormatInfoFunc = qlgGetTimestampFormatInfoFunc;

    /**
     * mGetTimestampWithTimeZoneFormatStringFunc
     */

    sVector.mGetTimestampWithTimeZoneFormatStringFunc = qlgGetTimestampWithTimeZoneFormatStringFunc;

    /**
     * mGetTimestampWithTimeZoneFormatInfoFunc
     */

    sVector.mGetTimestampWithTimeZoneFormatInfoFunc = qlgGetTimestampWithTimeZoneFormatInfoFunc;

    stlMemcpy( KNL_DT_VECTOR(aEnv),
               & sVector,
               STL_SIZEOF(dtlFuncVector) );
    
    return STL_SUCCESS;
}


/**
 * @brief Session 의 NLS Property 를 설정한다.
 */
stlStatus qllInitSessNLS( qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV( aEnv );

    dtlIntervalDayToSecondType sTimeZone;
    
    /**
     * TIME ZONE 정보 설정
     */

    sTimeZone = ellGetDbTimezone();
    
    STL_TRY( dtlTimeZoneToGMTOffset( & sTimeZone,
                                     & sSessEnv->mSessNLS.mTimeZoneOffset,
                                     QLL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qllInitSessNLSFromProperty( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Session 의 NLS Property 를 설정한다.
 */
stlStatus qllInitSessNLSFromProperty( qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV( aEnv );

    stlChar  sPropString[KNL_PROPERTY_STRING_MAX_LENGTH];

    /* TO_CHAR() TO_DATETIME FORMAT 용 임시버퍼 */        
    stlInt64  sFormatInfoBuf[DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE / STL_SIZEOF(stlInt64)];

    dtlDateTimeFormatInfo  * sFormatInfo = (dtlDateTimeFormatInfo *)sFormatInfoBuf;

    /**
     * NLS_DATE_FORMAT 정보 설정
     */

    STL_TRY( knlGetPropertyValueByID ( KNL_PROPERTY_NLS_DATE_FORMAT,
                                       sPropString,
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qllSetSessNlsFormat( DTL_TYPE_DATE,
                                  sPropString,
                                  sFormatInfo,
                                  sSessEnv->mSessNLS.mDATE_FORMAT,
                                  sSessEnv->mSessNLS.mDateFormatInfo,
                                  & sSessEnv->mSessNLS.mDATE_FORMAT,
                                  & sSessEnv->mSessNLS.mDateFormatInfo,
                                  aEnv )
             == STL_SUCCESS );
    
    /**
     * NLS_TIME_FORMAT 정보 설정
     */

    STL_TRY( knlGetPropertyValueByID ( KNL_PROPERTY_NLS_TIME_FORMAT,
                                       sPropString,
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qllSetSessNlsFormat( DTL_TYPE_TIME,
                                  sPropString,
                                  sFormatInfo,
                                  sSessEnv->mSessNLS.mTIME_FORMAT,
                                  sSessEnv->mSessNLS.mTimeFormatInfo,
                                  & sSessEnv->mSessNLS.mTIME_FORMAT,
                                  & sSessEnv->mSessNLS.mTimeFormatInfo,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * NLS_TIME_WITH_TIME_ZONE_FORMAT 정보 설정
     */

    STL_TRY( knlGetPropertyValueByID ( KNL_PROPERTY_NLS_TIME_WITH_TIME_ZONE_FORMAT,
                                       sPropString,
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qllSetSessNlsFormat( DTL_TYPE_TIME_WITH_TIME_ZONE,
                                  sPropString,
                                  sFormatInfo,
                                  sSessEnv->mSessNLS.mTIME_WITH_TIME_ZONE_FORMAT,
                                  sSessEnv->mSessNLS.mTimeWithTimeZoneFormatInfo,
                                  & sSessEnv->mSessNLS.mTIME_WITH_TIME_ZONE_FORMAT,
                                  & sSessEnv->mSessNLS.mTimeWithTimeZoneFormatInfo,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * NLS_TIMESTAMP_FORMAT 정보 설정
     */

    STL_TRY( knlGetPropertyValueByID ( KNL_PROPERTY_NLS_TIMESTAMP_FORMAT,
                                       sPropString,
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qllSetSessNlsFormat( DTL_TYPE_TIMESTAMP,
                                  sPropString,
                                  sFormatInfo,
                                  sSessEnv->mSessNLS.mTIMESTAMP_FORMAT,
                                  sSessEnv->mSessNLS.mTimestampFormatInfo,
                                  & sSessEnv->mSessNLS.mTIMESTAMP_FORMAT,
                                  & sSessEnv->mSessNLS.mTimestampFormatInfo,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * NLS_TIMESTAMP_WITH_TIME_ZONE_FORMAT 정보 설정
     */

    STL_TRY( knlGetPropertyValueByID ( KNL_PROPERTY_NLS_TIMESTAMP_WITH_TIME_ZONE_FORMAT,
                                       sPropString,
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qllSetSessNlsFormat( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                  sPropString,
                                  sFormatInfo,
                                  sSessEnv->mSessNLS.mTIMESTAMP_WITH_TIME_ZONE_FORMAT,
                                  sSessEnv->mSessNLS.mTimestampWithTimeZoneFormatInfo,
                                  & sSessEnv->mSessNLS.mTIMESTAMP_WITH_TIME_ZONE_FORMAT,
                                  & sSessEnv->mSessNLS.mTimestampWithTimeZoneFormatInfo,
                                  aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    /* Error 가 발생하더라도 Property의 변경을 atomic action으로 보기 때문에 해제하지 않는다. */

    return STL_FAILURE;
}


/**
 * @brief Session 의 NLS Property 를 해제한다.
 */
stlStatus qllFiniSessNLSFromProperty( qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV( aEnv );
    stlInt32        sState   = 0;

    
    /**
     * NLS_DATE_FORMAT 정보 해제
     */

    if( sSessEnv->mSessNLS.mDATE_FORMAT != NULL )
    {
        STL_TRY( knlFreeDynamicMem( & sSessEnv->mMemNLS,
                                    sSessEnv->mSessNLS.mDATE_FORMAT,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sSessEnv->mSessNLS.mDATE_FORMAT = NULL;
    }
    sState = 1;

    /**
     * NLS_TIME_FORMAT 정보 해제
     */

    if( sSessEnv->mSessNLS.mTIME_FORMAT != NULL )
    {
        STL_TRY( knlFreeDynamicMem( & sSessEnv->mMemNLS,
                                    sSessEnv->mSessNLS.mTIME_FORMAT,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sSessEnv->mSessNLS.mTIME_FORMAT = NULL;
    }
    sState = 2;

    /**
     * NLS_TIME_WITH_TIME_ZONE_FORMAT 정보 해제
     */

    if( sSessEnv->mSessNLS.mTIME_WITH_TIME_ZONE_FORMAT != NULL )
    {
        STL_TRY( knlFreeDynamicMem( & sSessEnv->mMemNLS,
                                    sSessEnv->mSessNLS.mTIME_WITH_TIME_ZONE_FORMAT,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sSessEnv->mSessNLS.mTIME_WITH_TIME_ZONE_FORMAT = NULL;
    }
    sState = 3;

    /**
     * NLS_TIMESTAMP_FORMAT 정보 해제
     */

    if( sSessEnv->mSessNLS.mTIMESTAMP_FORMAT != NULL )
    {
        STL_TRY( knlFreeDynamicMem( & sSessEnv->mMemNLS,
                                    sSessEnv->mSessNLS.mTIMESTAMP_FORMAT,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sSessEnv->mSessNLS.mTIMESTAMP_FORMAT = NULL;
    }
    sState = 4;

    /**
     * NLS_TIMESTAMP_WITH_TIME_ZONE_FORMAT 정보 해제
     */

    if( sSessEnv->mSessNLS.mTIMESTAMP_WITH_TIME_ZONE_FORMAT != NULL )
    {
        STL_TRY( knlFreeDynamicMem( & sSessEnv->mMemNLS,
                                    sSessEnv->mSessNLS.mTIMESTAMP_WITH_TIME_ZONE_FORMAT,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sSessEnv->mSessNLS.mTIMESTAMP_FORMAT = NULL;
    }
    sState = 5;

    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 0 :
            if( sSessEnv->mSessNLS.mDATE_FORMAT != NULL )
            {
                (void) knlFreeDynamicMem( & sSessEnv->mMemNLS,
                                          sSessEnv->mSessNLS.mDATE_FORMAT,
                                          KNL_ENV(aEnv) );
            }
        case 1 :
            if( sSessEnv->mSessNLS.mTIME_FORMAT != NULL )
            {
                (void) knlFreeDynamicMem( & sSessEnv->mMemNLS,
                                          sSessEnv->mSessNLS.mTIME_FORMAT,
                                          KNL_ENV(aEnv) );
            }
        case 2 :
            if( sSessEnv->mSessNLS.mTIME_WITH_TIME_ZONE_FORMAT != NULL )
            {
                (void) knlFreeDynamicMem( & sSessEnv->mMemNLS,
                                          sSessEnv->mSessNLS.mTIME_WITH_TIME_ZONE_FORMAT,
                                          KNL_ENV(aEnv) );
            }
        case 3 :
            if( sSessEnv->mSessNLS.mTIMESTAMP_FORMAT != NULL )
            {
                (void) knlFreeDynamicMem( & sSessEnv->mMemNLS,
                                          sSessEnv->mSessNLS.mTIMESTAMP_FORMAT,
                                          KNL_ENV(aEnv) );
            }
        case 4 :
            if( sSessEnv->mSessNLS.mTIMESTAMP_WITH_TIME_ZONE_FORMAT != NULL )
            {
                (void) knlFreeDynamicMem( & sSessEnv->mMemNLS,
                                          sSessEnv->mSessNLS.mTIMESTAMP_WITH_TIME_ZONE_FORMAT,
                                          KNL_ENV(aEnv) );
            }
        default :
            {
                break;
            }
    }

    return STL_FAILURE;
}


/**
 * @brief Session NLS 의 Time Zone Offset 을 초기화 한다.
 * @param[in]   aTimeZoneOffset   Time Zone 의 GMT offset
 * @param[in]   aEnv              Environment
 */
stlStatus qllSetLoginTimeZoneOffset( stlInt32   aTimeZoneOffset,
                                     qllEnv   * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV( aEnv );
    
    /**
     * GMT Offset 유효성 검사 
     */

    STL_TRY( dtlCheckGMTOffset( aTimeZoneOffset, QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

    /**
     * Time Zone NLS 설정
     */
    
    sSessEnv->mSessNLS.mTimeZoneOffset    = aTimeZoneOffset;
    sSessEnv->mSessNLS.mTimeZoneOffsetOrg = aTimeZoneOffset;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Session NLS 의 Character Set 을 초기화 한다.
 * @param[in]   aCharacterSet     character set
 * @param[in]   aEnv              Environment
 */
stlStatus qllSetLoginCharacterSet( dtlCharacterSet   aCharacterSet,
                                   qllEnv          * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV( aEnv );
    
    /**
     * Character Set ID 의 유효성 검사 
     */

    STL_TRY( dtlCheckCharacterSet( aCharacterSet, QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS );
             
    /**
     * Character Set 설정
     */
    
    sSessEnv->mSessNLS.mCharacterSet = aCharacterSet;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Session NLS 의 Time Zone Offset 을 설정한다.
 * @param[in]   aTimeZoneOffset   Time Zone 의 GMT offset
 * @param[in]   aEnv              Environment
 */
stlStatus qllSetSessTimeZoneOffset( stlInt32   aTimeZoneOffset,
                                    qllEnv   * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV( aEnv );

    /**
     * GMT Offset 유효성 검사 
     */

    STL_TRY( dtlCheckGMTOffset( aTimeZoneOffset, QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

    /**
     * Time Zone NLS 설정
     */
    
    sSessEnv->mSessNLS.mTimeZoneOffset = aTimeZoneOffset;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Session NLS 의 DATETIME FORMAT 을 설정한다. <BR>
 *        ( DATE, TIME, TIME WITH TIME ZONE, TIMESTAMP, TIMESTAMP WITH TIME ZONE )
 * @param[in]   aDataType                format 관련 data type
 * @param[in]   aFormat                  FORMAT string
 * @param[in]   aFormatInfoBuf           FORMAT info를 구성하기 위한 임시 버퍼 공간
 * @param[in]   aOrgFormatString         FORMAT STRING 주소
 * @param[in]   aOrgFormatInfo           org FORMAT info 주소
 * @param[out]  aNewFormatString         FORMAT STRING이 저장될 주소
 * @param[out]  aNewFormatInfo           new FORMAT info 정보가 저장될 주소
 * @param[in]   aEnv                     Environment
 * @remarks
 */
stlStatus qllSetSessNlsFormat( dtlDataType                aDataType,
                               stlChar                  * aFormat,
                               dtlDateTimeFormatInfo    * aFormatInfoBuf,
                               stlChar                  * aOrgFormatString,
                               dtlDateTimeFormatInfo    * aOrgFormatInfo,
                               stlChar                 ** aNewFormatString,
                               dtlDateTimeFormatInfo   ** aNewFormatInfo,
                               qllEnv                   * aEnv )
{
    qllSessionEnv * sSessEnv   = QLL_SESS_ENV( aEnv );
    
    stlInt32        sFormatLen = 0;
    stlInt32        sState     = 0;

    stlUInt32       sStrAlignSize        = 0;
    stlUInt32       sDateTimeFormatInfoAlignSize = 0;
    stlChar       * sNewFormatBuffer     = NULL;
    stlChar       * sOldFormatBuffer     = NULL;    
    
    
    stlBool                   sOrgExist  = STL_FALSE;
    stlChar                 * sOrgString = NULL;
    dtlDateTimeFormatInfo   * sOrgFormatInfo = NULL;

    stlChar                 * sNewString;
    dtlDateTimeFormatInfo   * sNewFormatInfo;

    stlUInt16                 sToCharMaxResultLen = 0;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aFormat != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFormatInfoBuf != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNewFormatString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNewFormatInfo != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Original Information 저장
     */
    
    if( aOrgFormatString != NULL )
    {
        sOrgExist = STL_TRUE;

        sOldFormatBuffer = aOrgFormatString;
        
        sOrgString     = aOrgFormatString;
        sOrgFormatInfo = aOrgFormatInfo;
        
        STL_DASSERT( sOrgString     != NULL );
        STL_DASSERT( sOrgFormatInfo != NULL );
    }
    else
    {
        sOrgExist = STL_FALSE;
    }
    
    /*****************************************************
     * Session NLS 정보 구축
     *****************************************************/
    
    sFormatLen = stlStrlen( aFormat );    
    
    /**
     * TO_CHAR 용 Format 정보를 위한 size를 얻는다.
     */
    
    STL_TRY( dtlGetDateTimeFormatInfoForToChar( aDataType,
                                                aFormat,
                                                sFormatLen,
                                                aFormatInfoBuf,
                                                DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                KNL_DT_VECTOR(aEnv),
                                                aEnv,
                                                QLL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    /* sToCharMaxResultLen를 임시저장 */
    sToCharMaxResultLen = ((dtlDateTimeFormatInfo*)aFormatInfoBuf)->mToCharMaxResultLen;
    
    /**
     * TO_DATE 용 Format 정보를 위한 size를 얻는다.
     */
    
    STL_TRY( dtlGetDateTimeFormatInfoForToDateTime( aDataType,
                                                    aFormat,
                                                    sFormatLen,
                                                    STL_TRUE, /* aIsSaveToCharMaxResultLen */
                                                    aFormatInfoBuf,
                                                    DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                    KNL_DT_VECTOR(aEnv),
                                                    aEnv,
                                                    QLL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    /*****************************************************
     * Session NLS 변경
     *****************************************************/
    
    /**
     * 메모리 할당
     */
    
    sStrAlignSize = STL_ALIGN_DEFAULT( sFormatLen + 1 );
    sDateTimeFormatInfoAlignSize = STL_ALIGN_DEFAULT( aFormatInfoBuf->mSize );
    
    STL_TRY( knlAllocDynamicMem( & sSessEnv->mMemNLS,
                                 ( sStrAlignSize + sDateTimeFormatInfoAlignSize ),
                                 (void **) & sNewFormatBuffer,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    sNewString = sNewFormatBuffer;
    sNewFormatInfo = (dtlDateTimeFormatInfo *)(sNewFormatBuffer + sStrAlignSize);
    
    sState = 1;
    
    /**
     * format string 복사
     */

    stlStrcpy( sNewString, aFormat );

    /**
     * NEW FORMAT info 정보 구축
     * 
     * fomat strng에 대한 검증은
     * to_char , to_datetime info에 대한 size를 얻는 과정에서 이루어졌으므로,
     * dtlGetDateTimeFormatInfoForToDateTime()을 통해서 format info 를 구축한다.
     *
     * to_char, to_datetime 에 대한 validation 과정은 다르지만,
     * format info에 대한 정보는 결국 동일하므로 ( to_datetime에 대한 부가적인 정보를 제외하고는 )
     * 동일한 자료구조의 format info를 사용할 수 있다.
     */
    
    STL_TRY( dtlGetDateTimeFormatInfoForToDateTime( aDataType,
                                                    aFormat,
                                                    sFormatLen,
                                                    STL_FALSE, /* aIsSaveToCharMaxResultLen */
                                                    sNewFormatInfo,
                                                    aFormatInfoBuf->mSize,
                                                    KNL_DT_VECTOR(aEnv),
                                                    aEnv,
                                                    QLL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    /* 임시 저장해둔 sToCharMaxResultLen 값을 sNewFormatInfo에 저장한다. */
    ((dtlDateTimeFormatInfo*)sNewFormatInfo)->mToCharMaxResultLen = sToCharMaxResultLen;
    
    /**
     * New NLS session 정보 설정
     */

    *aNewFormatString = sNewString;
    *aNewFormatInfo   = sNewFormatInfo;
    
    /**
     * 메모리 해제
     */
    
    if ( sOrgExist == STL_FALSE )
    {
        /* nothing to do */
    }
    else
    {
        sState = 0;
        
        STL_TRY( knlFreeDynamicMem( & sSessEnv->mMemNLS,
                                    sOldFormatBuffer,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
             
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlFreeDynamicMem( & sSessEnv->mMemNLS,
                                  sNewFormatBuffer,
                                  KNL_ENV(aEnv) );
        
        if( sOrgExist == STL_TRUE )
        {
            *aNewFormatString = sOrgString;
            *aNewFormatInfo   = sOrgFormatInfo;
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    return STL_FAILURE;
}


/**
 * @brief Session 의 수행 통계 정보를 System 통계 정보로 추가한다.
 * @param[in]  aSessEnv   Session Environment
 * @remarks
 * System 통계 정보를 수행시마다 누적하는 것은 Multi CPU 환경에서
 * Cache-Miss 로 Scalability 저하 요인이 된다.
 */
void qllAddSessStat2SystemStat( qllSessionEnv * aSessEnv )
{
    stlInt32 i;
    
    if ( (aSessEnv != NULL) && (gQllSharedEntry != NULL) )
    {
        if ( knlIsSharedSessionEnv( (knlSessionEnv *) aSessEnv ) == STL_TRUE )
        {
            gQllSharedEntry->mParsingCnt     += aSessEnv->mParsingCnt;
            gQllSharedEntry->mValidateCnt    += aSessEnv->mValidateCnt;
            gQllSharedEntry->mBindContextCnt += aSessEnv->mBindContextCnt;
            gQllSharedEntry->mOptCodeCnt     += aSessEnv->mOptCodeCnt;
            gQllSharedEntry->mOptDataCnt     += aSessEnv->mOptDataCnt;
            gQllSharedEntry->mRecheckCnt     += aSessEnv->mRecheckCnt;
            gQllSharedEntry->mExecuteCnt     += aSessEnv->mExecuteCnt;
            gQllSharedEntry->mRecompileCnt   += aSessEnv->mRecompileCnt;
            gQllSharedEntry->mFetchCnt       += aSessEnv->mFetchCnt;
            
            for ( i = 0; i < (QLL_STMT_MAX - QLL_STMT_NA); i++ )
            {
                gQllSharedEntry->mExecStmtCnt[i] += aSessEnv->mExecStmtCnt[i];
            }
        }
        else
        {
            /* heap env 임 */
        }
    }
    else
    {
        /* nothing to do */
    }
}

/** @} qllSessEnv */
