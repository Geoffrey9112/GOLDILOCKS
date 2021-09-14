/*******************************************************************************
 * glgStartup.c
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
 * @file qlgStartup.c
 * @brief SQL Processor Layer Startup Internal Routines
 */


#include <qll.h>
#include <qlDef.h>
#include <qlgStartup.h>
#include <qlgFixedTable.h>

/**
 * @addtogroup qlgStartup
 * @{
 */

qllSharedEntry * gQllSharedEntry = NULL;

stlFatalHandler  gQlgOldFatalHandler;

/**
 * @brief SQL Processor Layer 를 No Mount상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param [in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus qlgStartupNoMount( knlStartupInfo * aStartupInfo,
                             void           * aEnv )
{
    qllEnv * sEnv = (qllEnv *) aEnv;

    void ** sEntryPoint = NULL;
    void  * sShareEntry = NULL;
    
    /*
     * Parameter Validation
     */
    
    STL_TRY( sEnv != NULL );

    /***************************************************************
     * Database Shared 영역을 위한 초기화  
     ***************************************************************/

    /**
     * SQL Layer 의 Shared Entry 초기화
     */
    
    STL_TRY( knlGetEntryPoint( STL_LAYER_SQL_PROCESSOR,
                               (void **) & sEntryPoint,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocFixedStaticArea( STL_SIZEOF( qllSharedEntry ),
                                      & sShareEntry,
                                      KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sShareEntry, 0x00, STL_SIZEOF( qllSharedEntry ) );

    *sEntryPoint = sShareEntry;

    /**
     * Global 변수에 Shared Entry 연결
     */
    
    STL_TRY( knlGetEntryPoint( STL_LAYER_SQL_PROCESSOR,
                               (void **) & sEntryPoint,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    gQllSharedEntry = *sEntryPoint;

    /***************************************************************
     * Process Shared 영역을 위한 초기화  
     ***************************************************************/
    
    /**
     * SQL layer 의 Fixed Table 들을 등록한다.
     */

    STL_TRY( qlgRegisterFixedTables( sEnv ) == STL_SUCCESS );

    /**
     * 현재 DB 상태를 기준으로 Session NLS 를 변경한다.
     */
    
    STL_TRY( qllSetDTFuncVector(aEnv) == STL_SUCCESS );
    STL_TRY( qllInitSessNLS( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    gQllSharedEntry = NULL;
    
    return STL_FAILURE;
}


/**
 * @brief SQL Processor Layer Layer를 Mount 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus qlgStartupMount( knlStartupInfo * aStartupInfo,
                           void           * aEnv )
{
    STL_TRY( qllSetDTFuncVector(aEnv) == STL_SUCCESS );
    STL_TRY( qllInitSessNLS( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SQL Processor Layer 를 PREOPEN 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus qlgStartupPreOpen( knlStartupInfo * aStartupInfo,
                             void           * aEnv )
{
    smlTransId      sTransId;
    stlInt32        sState = 0;
    stlXid          sXid;
    stlBool         sHeuristicCommit;
    stlInt64        sInDoubtDecision;
    stlBool         sIsPreparable;
    smlTransId      sInDoubtTransId = SML_INVALID_TRANSID;
    stlChar         sXidString[STL_XID_STRING_SIZE];
    smlStatement  * sStatement = NULL; 

    
    STL_TRY( qllSetDTFuncVector(aEnv) == STL_SUCCESS );
    STL_TRY( qllInitSessNLS( aEnv ) == STL_SUCCESS );

    sInDoubtDecision = knlGetPropertyBigIntValueByID( KNL_PROPERTY_IN_DOUBT_DECISION,
                                                      KNL_ENV(aEnv) );

    if( sInDoubtDecision == SML_IN_DOUBT_DECISION_COMMIT )
    {
        sHeuristicCommit = STL_TRUE;
    }
    else
    {
        sHeuristicCommit = STL_FALSE;
    }
                
    STL_TRY( smlFetchPreparedTransaction( &sInDoubtTransId,
                                          STL_FALSE, /* aRecoverLock */
                                          &sXid,
                                          SML_ENV(aEnv) )
             == STL_SUCCESS );

    while( sInDoubtTransId != SML_INVALID_TRANSID )
    {
        sIsPreparable = smlIsRepreparableTransaction( sInDoubtTransId );

        if( sIsPreparable == STL_FALSE )
        {
            /**
             * IN-DOUBT TRANSACTION에 대한 의사결정이 필요함.
             */
            
            STL_TRY( dtlXidToString( &sXid,
                                     STL_XID_STRING_SIZE,
                                     sXidString,
                                     KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );
                
            STL_TRY( smlBeginTrans( SML_TIL_READ_COMMITTED,
                                    STL_FALSE,  /* aIsGlobalTrans */
                                    &sTransId,
                                    aEnv )
                     == STL_SUCCESS );
            sState = 1;

            STL_TRY( smlAllocStatement( sTransId,
                                        NULL, /* aUpdateRelHandle */
                                        SML_STMT_ATTR_UPDATABLE | SML_STMT_ATTR_LOCKABLE,
                                        SML_LOCK_TIMEOUT_INVALID,
                                        STL_FALSE, /* aNeedSnapshotIterator */
                                        & sStatement,
                                        SML_ENV( aEnv ) )
                     == STL_SUCCESS );
            sState = 2;

            STL_TRY( ellInsertPendingTrans( sTransId,
                                            sStatement,
                                            sXidString,
                                            sInDoubtTransId,
                                            "heuristic completed: access nologging index",
                                            sHeuristicCommit,
                                            ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            sState = 1;
            STL_TRY( smlFreeStatement( sStatement,
                                       SML_ENV( aEnv ) )
                     == STL_SUCCESS );
            
            if( sHeuristicCommit == STL_TRUE )
            {
                STL_TRY( smlCommitInDoubtTransaction( sTransId,
                                                      sInDoubtTransId,
                                                      "heuristic completed: access nologging index",
                                                      aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                STL_TRY( smlRollbackInDoubtTransaction( sTransId,
                                                        sInDoubtTransId,
                                                        STL_TRUE,  /* aRestartRollback */
                                                        aEnv )
                         == STL_SUCCESS );
            }
            
            sState = 0;
            STL_TRY( smlEndTrans( sTransId,
                                  SML_PEND_ACTION_COMMIT,
                                  SML_TRANSACTION_CWM_WAIT,
                                  SML_ENV(aEnv) )
                     == STL_SUCCESS );
                
            STL_TRY( smlEndTrans( sInDoubtTransId,
                                  ( (sHeuristicCommit == STL_TRUE ) ?
                                    SML_PEND_ACTION_COMMIT : SML_PEND_ACTION_ROLLBACK ),
                                  SML_TRANSACTION_CWM_WAIT,
                                  SML_ENV(aEnv) )
                     == STL_SUCCESS );
        }

        STL_TRY( smlFetchPreparedTransaction( &sInDoubtTransId,
                                              STL_FALSE, /* aRecoverLock */
                                              &sXid,
                                              aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smlFreeStatement( sStatement, SML_ENV( aEnv ) );
        case 1:
            (void) smlRollback( sTransId,
                                SML_TRANSACTION_CWM_NO_WAIT,
                                SML_ENV( aEnv ) );
            (void) smlEndTrans( sTransId,
                                SML_PEND_ACTION_ROLLBACK,
                                SML_TRANSACTION_CWM_NO_WAIT,
                                SML_ENV( aEnv ) );
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief SQL Processor Layer 를 Service 가능한 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus qlgStartupOpen( knlStartupInfo * aStartupInfo,
                          void           * aEnv )
{
    stlInt32        sState = 0;
    
    /**
     * Dictionary Cache 를 위한 Memory 관리자 초기화
     */

    STL_TRY( knlCreateDynamicMem( & gQllSharedEntry->mMemContextXA,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_SQL_PROCESSOR_XA_CONTEXT,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  QLL_XA_CONTEXT_MEM_INIT_SIZE,
                                  QLL_XA_CONTEXT_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            {
                (void) knlDestroyDynamicMem( & gQllSharedEntry->mMemContextXA, KNL_ENV(aEnv) );
            }
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief SQL Processor Layer 를 Warm-up 단계로 전환시킨다.
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus qlgWarmup( void * aEnv )
{
    qllEnv * sEnv = (qllEnv *) aEnv;

    void ** sEntryPoint = NULL;
    
    /*
     * Parameter Validation
     */
    
    STL_TRY( sEnv != NULL );

    /***************************************************************
     * Database Shared 영역을 위한 초기화  
     ***************************************************************/

    /**
     * Global 변수에 Shared Entry 연결
     */
    
    STL_TRY( knlGetEntryPoint( STL_LAYER_SQL_PROCESSOR,
                               (void **) & sEntryPoint,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    gQllSharedEntry = *sEntryPoint;
    
    /***************************************************************
     * Process Shared 영역을 위한 초기화  
     ***************************************************************/
    
    /**
     * SQL layer 의 Fixed Table 들을 등록한다.
     */

    STL_TRY( qlgRegisterFixedTables( sEnv ) == STL_SUCCESS );

    /******************************************************************
     * Session NLS 설정
     ******************************************************************/
    
    /**
     * Session NLS 정보 설정
     */

    STL_TRY( qllSetDTFuncVector(aEnv) == STL_SUCCESS );
    STL_TRY( qllInitSessNLS( sEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    knlLogMsgUnsafe( NULL,
                     KNL_ENV( aEnv ),
                     KNL_LOG_LEVEL_FATAL,
                     "[SQL PROCESSOR WARMUP FAIL] %s\n",
                     stlGetLastErrorMessage( KNL_ERROR_STACK( aEnv ) ) );
    
    return STL_FAILURE;
}


/**
 * @brief SQL Processor Layer 를 Cool-down 시킨다.
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus qlgCooldown( void * aEnv )
{
    /**
     * Nothing To Do
     */
    
    return STL_SUCCESS;
}


/**
 * @brief SQL Processor Layer 를 INIT 시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus qlgShutdownInit( knlShutdownInfo * aShutdownInfo,
                           void            * aEnv )
{
    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );
    
    /**
     * Nothing To Do
     */
    
    STL_RAMP( RAMP_SHUTDOWN_ABORT );
    
    return STL_SUCCESS;
}


/**
 * @brief SQL Processor Layer 를 DISMOUNT 시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus qlgShutdownDismount( knlShutdownInfo * aShutdownInfo,
                               void            * aEnv )
{
    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    /**
     * Nothing To Do
     */
    
    STL_RAMP( RAMP_SHUTDOWN_ABORT );
    
    return STL_SUCCESS;
}


/**
 * @brief SQL Processor Layer 를 POST_CLOSE 시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus qlgShutdownPostClose( knlShutdownInfo * aShutdownInfo,
                                void            * aEnv )
{
    stlInt32        sState = 1;
    
    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    sState = 0;
    STL_TRY( knlDestroyDynamicMem( & gQllSharedEntry->mMemContextXA, KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_RAMP( RAMP_SHUTDOWN_ABORT );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            {
                (void) knlDestroyDynamicMem( & gQllSharedEntry->mMemContextXA, KNL_ENV(aEnv) );
            }
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief SQL Processor Layer 를 CLOSE 시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus qlgShutdownClose( knlShutdownInfo * aShutdownInfo,
                            void            * aEnv )
{
    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    /**
     * Nothing To Do
     */
    
    STL_RAMP( RAMP_SHUTDOWN_ABORT );

    return STL_SUCCESS;
}



/**
 * @brief SQL Processor Layer의 Fatal Handler.
 * @param[in] aCauseString Fatal 유발 원인을 알수 있는 스트링
 * @param[in] aSigInfo Signal Information
 * @param[in] aContext 이전 Context 정보
 */
void qlgFatalHandler( const stlChar * aCauseString,
                      void          * aSigInfo,
                      void          * aContext )
{
    qllEnv        * sMyEnv = NULL;
    qllSessionEnv * sSessEnv = NULL;
    stlChar         sMessage[1024] = {0,};

    stlInt64        i = 0;
    
    sMyEnv = (qllEnv*) knlGetMyEnv();

    STL_TRY_THROW( sMyEnv != NULL, RAMP_FINISH );
    STL_TRY_THROW( knlGetTopLayer(KNL_ENV(sMyEnv))
                   >= STL_LAYER_SQL_PROCESSOR,
                   RAMP_FINISH );

    if( KNL_GET_FATAL_HANDLING( sMyEnv, STL_LAYER_SQL_PROCESSOR ) == STL_FALSE )
    {
        /**
         * 무한 Fatal을 방지해야 한다.
         */
        
        KNL_SET_FATAL_HANDLING( sMyEnv, STL_LAYER_SQL_PROCESSOR );
    
    
        sSessEnv = QLL_SESS_ENV( sMyEnv );

        STL_TRY_THROW( sSessEnv != NULL, RAMP_SKIP_FATAL );
        
        /**
         * 수행 통계 정보 누적
         */
        
        qllAddSessStat2SystemStat( sSessEnv );
        
        /**
         * Session 에서 수행된 SQL 구문 유형 정보
         */

        for ( i = 0; i < (QLL_STMT_MAX - QLL_STMT_NA); i++ )
        {
            if ( sSessEnv->mExecStmtCnt[i] > 0 )
            {
                stlSnprintf( sMessage,
                             1024,
                             "%s"
                             "---- %-21s : %ld\n",
                             sMessage,
                             qlgGetStmtTypeKeyString( i ),
                             sSessEnv->mExecStmtCnt[i] );
            }
            else
            {
                /* 수행되지 않은 SQL 구문 */
            }
        }
        
        /**
         * Session 의 Call 횟수 정보
         */
        
        stlSnprintf( sMessage,
                     1024,
                     "%s"
                     "---- function call information \n"
                     "---- CALL_COUNT(PARSE)     : %ld\n"
                     "---- CALL_COUNT(VALIDATE)  : %ld\n"
                     "---- CALL_COUNT(OPTIMIZE)  : %ld\n"
                     "---- CALL_COUNT(EXECUTE)   : %ld\n"
                     "---- CALL_COUNT(FETCH)     : %ld\n"
                     "---- CALL_COUNT(RECOMPILE) : %ld\n",
                     sMessage,
                     sSessEnv->mParsingCnt,
                     sSessEnv->mValidateCnt,
                     sSessEnv->mOptCodeCnt,
                     sSessEnv->mExecuteCnt,
                     sSessEnv->mFetchCnt,
                     sSessEnv->mRecompileCnt );
                     
        /**
         * Fatal 당시의 정보를 남긴다.
         */
        
        (void) knlLogMsgUnsafe( NULL,
                                (knlEnv*) sMyEnv,
                                KNL_LOG_LEVEL_FATAL,
                                "[SQL PROCESSOR INFORMATION]\n"
                                "---- executed SQL phrase information \n"
                                "%s",
                                sMessage );
    }
    
    STL_RAMP( RAMP_FINISH );
    
    /**
     * Execution Library Layer의 Fatal Handler를 호출한다.
     */
    
    gQlgOldFatalHandler( aCauseString, aSigInfo, aContext );

    STL_RAMP( RAMP_SKIP_FATAL );
}





/**
 * @brief Character Set ID 획득 함수
 * @param[in]  aArgs  Environment
 * @return
 * Character Set ID
 */
dtlCharacterSet qlgGetCharSetIDFunc( void * aArgs )
{
    return ellGetDbCharacterSet();
}

/**
 * @brief GMT Offset 획득 함수
 * @param[in]  aArgs  Environment
 * @return
 * GMT Offset
 */
stlInt32 qlgGetGMTOffsetFunc( void * aArgs )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aArgs);

    return sSessEnv->mSessNLS.mTimeZoneOffset;
}

/**
 * @brief Original GMT Offset 획득 함수
 * @param[in]  aArgs  Environment
 * @return
 * GMT Offset
 */
stlInt32 qlgGetOriginalGMTOffsetFunc( void * aArgs )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aArgs);

    return sSessEnv->mSessNLS.mTimeZoneOffsetOrg;
}

/**
 * @brief Date Format 획득 함수
 * @param[in]  aArgs  Environment
 * @return 
 * Date Format String
 */
stlChar * qlgGetDateFormatStringFunc( void * aArgs )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aArgs);
    
    return sSessEnv->mSessNLS.mDATE_FORMAT;
}

/**
 * @brief 함수 DATE 용 Format information 획득 함수
 * @param[in]  aArgs  Environment
 * @return 
 * DATE 용 Format information
 */
dtlDateTimeFormatInfo * qlgGetDateFormatInfoFunc( void * aArgs )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aArgs);
    
    return sSessEnv->mSessNLS.mDateFormatInfo;
}


/**
 * @brief Time Format 획득 함수
 * @param[in]  aArgs  Environment
 * @return 
 * Time Format String
 */
stlChar * qlgGetTimeFormatStringFunc( void * aArgs )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aArgs);
    
    return sSessEnv->mSessNLS.mTIME_FORMAT;
}

/**
 * @brief 함수 TIME 용 Format information 획득 함수
 * @param[in]  aArgs  Environment
 * @return 
 * TIME 용 Format information
 */
dtlDateTimeFormatInfo * qlgGetTimeFormatInfoFunc( void * aArgs )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aArgs);
    
    return sSessEnv->mSessNLS.mTimeFormatInfo;
}


/**
 * @brief Time_with_time_zone Format 획득 함수
 * @param[in]  aArgs  Environment
 * @return 
 * time_with_time_zone Format String
 */
stlChar * qlgGetTimeWithTimeZoneFormatStringFunc( void * aArgs )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aArgs);
    
    return sSessEnv->mSessNLS.mTIME_WITH_TIME_ZONE_FORMAT;
}

/**
 * @brief 함수 TIME WITH TIME ZONE 용 Format information 획득 함수
 * @param[in]  aArgs  Environment
 * @return 
 * TIME WITH TIME ZONE 용 Format information
 */
dtlDateTimeFormatInfo * qlgGetTimeWithTimeZoneFormatInfoFunc( void * aArgs )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aArgs);
    
    return sSessEnv->mSessNLS.mTimeWithTimeZoneFormatInfo;
}


/**
 * @brief timestamp Format 획득 함수
 * @param[in]  aArgs  Environment
 * @return 
 * timestamp Format String
 */
stlChar * qlgGetTimestampFormatStringFunc( void * aArgs )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aArgs);
    
    return sSessEnv->mSessNLS.mTIMESTAMP_FORMAT;
}


/**
 * @brief 함수 TIMESTAMP 용 Format information 획득 함수
 * @param[in]  aArgs  Environment
 * @return 
 * TIMESTAMP 용 Format information
 */
dtlDateTimeFormatInfo * qlgGetTimestampFormatInfoFunc( void * aArgs )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aArgs);
    
    return sSessEnv->mSessNLS.mTimestampFormatInfo;
}


/**
 * @brief timestamp_with_time_zone Format 획득 함수
 * @param[in]  aArgs  Environment
 * @return 
 * timestamp_with_time_zone Format String
 */
stlChar * qlgGetTimestampWithTimeZoneFormatStringFunc( void * aArgs )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aArgs);
    
    return sSessEnv->mSessNLS.mTIMESTAMP_WITH_TIME_ZONE_FORMAT;
}

/**
 * @brief 함수 TIMESTAMP WITH TIME ZONE 용 Format information 획득 함수
 * @param[in]  aArgs  Environment
 * @return 
 * TIMESTAMP WITH TIME ZONE 용 Format information
 */
dtlDateTimeFormatInfo * qlgGetTimestampWithTimeZoneFormatInfoFunc( void * aArgs )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aArgs);
    
    return sSessEnv->mSessNLS.mTimestampWithTimeZoneFormatInfo;
}


/**
 * @brief Long Varchar/Varbinary를 위한 추가 공간 할당 함수
 * @param[in]   aArgs       Callback context
 * @param[in]   aAllocSize  할당할 메모리 크기
 * @param[out]  aAddr       할당된 메모리 주소
 * @param[in]   aErrorStack Environment
 */
stlStatus qlgReallocLongVaryingMemFunc( void           * aArgs,
                                        stlInt32         aAllocSize,
                                        void          ** aAddr,
                                        stlErrorStack  * aErrorStack )
{
    return knlReallocLongVaryingMem( aAllocSize, aAddr, KNL_ENV(aArgs) );
}


/**
 * @brief Long Varchar/Varbinary를 위한 추가 공간 할당및 복사 함수
 * @param[in]   aArgs       Callback context
 * @param[out]  aDataValue  Data Value Pointer
 * @param[in]   aAllocSize  할당할 메모리 크기
 * @param[in]   aErrorStack Environment
 */
stlStatus qlgReallocAndMoveLongVaryingMemFunc( void           * aArgs,
                                               dtlDataValue   * aDataValue,
                                               stlInt32         aAllocSize,
                                               stlErrorStack  * aErrorStack )
{
    return knlReallocAndMoveLongVaryingMem( aDataValue, aAllocSize, KNL_ENV(aArgs) );
}


/**
 * @brief Non Service 단계에서의 Performance View 를 구축한다.
 * @param[in] aStartPhase Startup Phase
 * @param[in] aEnv        Environment
 */
stlStatus qlgBuildNonServicePerfView( knlStartupPhase   aStartPhase,
                                      qllEnv          * aEnv )
{
    qlgNonServicePerfView * sViewDefArray = NULL;
    
    stlChar  * sViewString   = NULL;
    
    stlInt32 i = 0;

    qllDBCStmt     sDBCStmt;
    qllStatement   sSQLStmt;
    qllNode      * sParseTree = sParseTree;

    ellDictHandle sLoginHandle;
    ellDictHandle sRoleHandle;

    stlBool sObjectExist = STL_FALSE;
    
    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( (aStartPhase == KNL_STARTUP_PHASE_NO_MOUNT) ||
                        (aStartPhase == KNL_STARTUP_PHASE_MOUNT),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * SQL Stmt 초기화
     */
    
    STL_TRY( qllInitDBC( & sDBCStmt, aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( qllInitSQL( & sSQLStmt, aEnv ) == STL_SUCCESS );
    sState = 2;

    sSQLStmt.mViewSCN = smlGetSystemScn();
    sSQLStmt.mStmt = NULL;
    
    /**
     * SYS --AS SYSDBA authorization 으로 설정 
     */
    
    STL_TRY( ellGetAuthDictHandleByID( SML_INVALID_TRANSID,
                                       sSQLStmt.mViewSCN,
                                       ellGetAuthIdSYS(),
                                       & sLoginHandle,
                                       & sObjectExist,
                                       ELL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_DASSERT( sObjectExist == STL_TRUE );

    STL_TRY( ellGetAuthDictHandleByID( SML_INVALID_TRANSID,
                                       sSQLStmt.mViewSCN,
                                       ellGetAuthIdSYSDBA(),
                                       & sRoleHandle,
                                       & sObjectExist,
                                       ELL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sObjectExist == STL_TRUE );

    /**
     * Session 정보 설정
     */
    
    ellSetLoginUser( & sLoginHandle, ELL_ENV(aEnv) );
    ellSetSessionRole( & sRoleHandle, ELL_ENV(aEnv) );

    /**
     * 단계별 View Definition Array 획득
     */
    
    if ( aStartPhase == KNL_STARTUP_PHASE_NO_MOUNT )
    {
        sViewDefArray = gQlgNoMountPerfViewDefinition;
    }
    else
    {
        sViewDefArray = gQlgMountPerfViewDefinition;
    }

    /**
     * 단계별 View Definition 구문을 수행
     */

    for ( i = 0; sViewDefArray[i].mViewName != NULL; i++ )
    {
        sViewString = sViewDefArray[i].mViewDefinition;

        /************************************
         * SQLStmt 초기화 
         ************************************/

        /**
         * Cursor 속성 설정
         */
        
        qllSetSQLStmtCursorProperty( & sSQLStmt,
                                     ELL_CURSOR_STANDARD_ISO,  
                                     ELL_CURSOR_SENSITIVITY_INSENSITIVE,
                                     ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                     ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD,
                                     ELL_CURSOR_UPDATABILITY_FOR_READ_ONLY,
                                     ELL_CURSOR_DEFAULT_RETURNABILITY );
        
        /************************************
         * Parsing
         ************************************/
        
        STL_TRY( qlgParseSQL( & sDBCStmt,
                              & sSQLStmt,
                              QLL_PHASE_PARSING,
                              sViewString,
                              STL_FALSE,  /* aIsRecompile */
                              STL_FALSE,   /* aSearchPlanCache */
                              & sParseTree,
                              aEnv )
                 == STL_SUCCESS );

        STL_DASSERT( sSQLStmt.mStmtType == QLL_STMT_CREATE_VIEW_TYPE ); 

        /************************************
         * Validation
         ************************************/

        sSQLStmt.mViewSCN = smlGetSystemScn();
        sSQLStmt.mStmt = NULL;

        /**
         * Granted Privilege List 초기화
         */

        STL_TRY( ellInitPrivList( & sSQLStmt.mGrantedPrivList,
                                  STL_FALSE,  /* aForRevoke */
                                  & sDBCStmt.mShareMemStmt,
                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Cycle Detector 초기화
         */

        STL_TRY( ellInitObjectList( & sSQLStmt.mViewCycleList,
                                    & sDBCStmt.mShareMemStmt,
                                    ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        qlgSetQueryPhaseMemMgr( & sDBCStmt, QLL_PHASE_VALIDATION, aEnv );
        
        STL_TRY( qlrValidateCreateView( SML_INVALID_TRANSID,
                                        & sDBCStmt,
                                        & sSQLStmt,
                                        sViewString,
                                        sParseTree,
                                        aEnv )
                 == STL_SUCCESS );

        /************************************
         * Code Optimization
         ************************************/
        
        qlgSetQueryPhaseMemMgr( & sDBCStmt, QLL_PHASE_CODE_OPTIMIZE, aEnv );
        
        STL_TRY( qlrOptCodeCreateView( SML_INVALID_TRANSID,
                                       & sDBCStmt,
                                       & sSQLStmt,
                                       aEnv )
                 == STL_SUCCESS );

        /************************************
         * Data Optimization
         ************************************/
        
        qlgSetQueryPhaseMemMgr( & sDBCStmt, QLL_PHASE_DATA_OPTIMIZE, aEnv );
        
        STL_TRY( qlrOptDataCreateView( SML_INVALID_TRANSID,
                                       & sDBCStmt,
                                       & sSQLStmt,
                                       aEnv )
                 == STL_SUCCESS );

        /************************************
         * Execution
         ************************************/
        
        qlgSetQueryPhaseMemMgr( & sDBCStmt, QLL_PHASE_EXECUTE, aEnv );

        STL_TRY( qlrExecuteCreateNonServicePerfView( & sDBCStmt,
                                                     & sSQLStmt,
                                                     aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( qllResetSQL( & sDBCStmt, & sSQLStmt, aEnv ) == STL_SUCCESS );
    }

    sState = 1;
    STL_TRY( qllFiniSQL( & sDBCStmt, & sSQLStmt, aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( qllFiniDBC( & sDBCStmt, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) qllFiniSQL( & sDBCStmt, & sSQLStmt, aEnv );
        case 1:
            (void) qllFiniDBC( & sDBCStmt, aEnv );
        default:
            break;
    }
    return STL_FAILURE;
}

/** @} qlgStartup */
