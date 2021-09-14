/*******************************************************************************
 * qllDBCStmt.c
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
 * @file qllDBCStmt.c
 * @brief SQL DBC Statement API
 */

#include <qll.h>

#include <qlDef.h>

/**
 * @addtogroup qllDBCStmt
 * @{
 */

/**
 * @brief DBC Stmt 를 초기화한다.
 * @param[in]  aDBCStmt  DBC Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus qllInitDBC( qllDBCStmt     * aDBCStmt,
                      qllEnv         * aEnv )
{
    stlUInt32        sMemType = 0;
    knlSessionType   sSessType;
    qllSessionEnv  * sSessEnv;

    stlInt32  sState = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );

    sSessType = KNL_SESS_ENV(aEnv)->mSessionType;
    sSessEnv  = QLL_SESS_ENV(aEnv);

    /**********************************************
     * Shared Memory
     **********************************************/
    
    /**
     * 메모리 유형 결정
     * - Dedicated Session 일 경우, Init/Code Plan 과 Fetch Memory 를 공유할 필요가 없다.
     * - Shared Session 일 경우, Init/Code Plan 과 Fetch Memory 는 공유할 수 있다.
     */
    
    switch (sSessType)
    {
        case KNL_SESSION_TYPE_DEDICATE:
            sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
            break;
        case KNL_SESSION_TYPE_SHARED:
            sMemType = KNL_MEM_STORAGE_TYPE_SHM;
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /**
     * Retry SQL 을 위한 메모리 관리자를 초기화
     */

    STL_TRY( knlCreateRegionMem( & aDBCStmt->mShareMemSyntaxSQL,
                                 KNL_ALLOCATOR_SQL_PROCESSOR_SHARE_SYNTAX_SQL,
                                 & sSessEnv->mMemParentDBC,  /* aParentMem */
                                 sMemType,
                                 QLL_RETRY_MEM_INIT_SIZE,
                                 QLL_RETRY_MEM_NEXT_SIZE,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Init Plan 을 위한 메모리 관리자를 초기화
     */

    STL_TRY( knlCreateRegionMem( & aDBCStmt->mShareMemInitPlan,
                                 KNL_ALLOCATOR_SQL_PROCESSOR_SHARE_INIT_PLAN,
                                 & sSessEnv->mMemParentDBC,  /* aParentMem */
                                 sMemType,
                                 QLL_INIT_PLAN_MEM_INIT_SIZE,
                                 QLL_INIT_PLAN_MEM_NEXT_SIZE,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;

    /**
     * Non-Cacheable SQL 을 위한 메모리 관리자를 초기화
     */

    STL_TRY( knlCreateRegionMem( & aDBCStmt->mNonPlanCacheMem,
                                 KNL_ALLOCATOR_SQL_PROCESSOR_NON_PLAN_CACHE,
                                 & sSessEnv->mMemParentDBC,  /* aParentMem */
                                 sMemType,
                                 QLL_NON_CACHE_PLAN_MEM_INIT_SIZE,
                                 QLL_NON_CACHE_PLAN_MEM_NEXT_SIZE,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 3;
    
    /**
     * Cacheable SQL 을 위한 메모리 관리자를 초기화
     */

    if( knlIsAdminSession( QLL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        /**
         * admin session은 shared memory를 사용하지 않기 위해
         * plan cache를 사용하지 않는다.
         */
        STL_TRY( knlCreateRegionMem( & aDBCStmt->mPlanCacheMem,
                                     KNL_ALLOCATOR_SQL_PROCESSOR_PLAN_CACHE_FOR_ADMIN_SESSION,
                                     NULL,  /* aParentMem */
                                     KNL_MEM_STORAGE_TYPE_HEAP,
                                     0,
                                     QLL_NEXT_PLAN_CACHE_MEM_NEXT_SIZE,
                                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlCreateRegionMem( & aDBCStmt->mPlanCacheMem,
                                     KNL_ALLOCATOR_SQL_PROCESSOR_PLAN_CACHE_FOR_ADMIN_SESSION,
                                     knlGetPlanRegionMem( KNL_ENV(aEnv) ),
                                     KNL_MEM_STORAGE_TYPE_SHM,
                                     0,
                                     QLL_NEXT_PLAN_CACHE_MEM_NEXT_SIZE,
                                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    sState = 4;

    /**
     * Data Plan 을 위한 메모리 관리자를 초기화
     */

    STL_TRY( knlCreateRegionMem( & aDBCStmt->mShareMemDataPlan,
                                 KNL_ALLOCATOR_SQL_PROCESSOR_SHARE_DATA_PLAN,
                                 & sSessEnv->mMemParentDBC,  /* aParentMem */
                                 sMemType,
                                 QLL_DATA_PLAN_MEM_INIT_SIZE,
                                 QLL_DATA_PLAN_MEM_NEXT_SIZE,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 5;
    
    /**
     * Statement life-time을 갖는 메모리 관리자 초기화
     */

    STL_TRY( knlCreateRegionMem( & aDBCStmt->mShareMemStmt,
                                 KNL_ALLOCATOR_SQL_PROCESSOR_SHARE_STATEMENT,
                                 & sSessEnv->mMemParentDBC,  /* aParentMem */
                                 sMemType,
                                 QLL_STMT_MEM_INIT_SIZE,
                                 QLL_STMT_MEM_NEXT_SIZE,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 6;

    aDBCStmt->mShareMemCodePlan = NULL;

    qlgDisableQueryPhaseMemMgr( aDBCStmt, aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 6:
            (void) knlDestroyRegionMem( & aDBCStmt->mShareMemStmt,
                                        KNL_ENV(aEnv) );
        case 5:
            (void) knlDestroyRegionMem( & aDBCStmt->mShareMemDataPlan,
                                        KNL_ENV(aEnv) );
        case 4:
            (void) knlDestroyRegionMem( & aDBCStmt->mPlanCacheMem,
                                        KNL_ENV(aEnv) );
        case 3:
            (void) knlDestroyRegionMem( & aDBCStmt->mNonPlanCacheMem,
                                        KNL_ENV(aEnv) );
        case 2:
            (void) knlDestroyRegionMem( & aDBCStmt->mShareMemInitPlan,
                                        KNL_ENV(aEnv) );
        case 1:
            (void) knlDestroyRegionMem( & aDBCStmt->mShareMemSyntaxSQL,
                                        KNL_ENV(aEnv) );
        default:
            break;
    }
   
    return STL_FAILURE;
}
    
/**
 * @brief DBC Stmt 를 종료한다.
 * @param[in]  aDBCStmt DBC Statement
 * @param[in]  aEnv     Environment
 * @remarks
 */
stlStatus qllFiniDBC( qllDBCStmt   * aDBCStmt,
                      qllEnv       * aEnv )
{
    stlInt32  sState = 0;
    
    qlgDisableQueryPhaseMemMgr( aDBCStmt, aEnv );

    /**********************************************
     * Shared Memory
     **********************************************/
    
    /**
     * Syntax 저장을 위한 메모리 관리자를 종료
     */

    sState = 1;
    STL_TRY( knlDestroyRegionMem( & aDBCStmt->mShareMemSyntaxSQL,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Init Plan 을 위한 메모리 관리자를 종료
     */
    
    sState = 2;
    STL_TRY( knlDestroyRegionMem( & aDBCStmt->mShareMemInitPlan,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Cacheable Plan 을 위한 메모리 관리자를 종료
     */
    
    sState = 3;
    STL_TRY( knlDestroyRegionMem( & aDBCStmt->mPlanCacheMem,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Non-Cacheable Plan 을 위한 메모리 관리자를 종료
     */
    
    sState = 4;
    STL_TRY( knlDestroyRegionMem( & aDBCStmt->mNonPlanCacheMem,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Data Plan 을 위한 메모리 관리자를 종료
     */
    
    sState = 5;
    STL_TRY( knlDestroyRegionMem( & aDBCStmt->mShareMemDataPlan,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    /**
     * Statement life-time을 갖는 메모리 관리자 종료
     */
    
    sState = 6;
    STL_TRY( knlDestroyRegionMem( & aDBCStmt->mShareMemStmt,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 0:
            (void) knlDestroyRegionMem( & aDBCStmt->mShareMemSyntaxSQL,
                                        KNL_ENV(aEnv) );
        case 1:
            (void) knlDestroyRegionMem( & aDBCStmt->mShareMemInitPlan,
                                        KNL_ENV(aEnv) );
        case 2:
            (void) knlDestroyRegionMem( & aDBCStmt->mPlanCacheMem,
                                        KNL_ENV(aEnv) );
        case 3:
            (void) knlDestroyRegionMem( & aDBCStmt->mNonPlanCacheMem,
                                        KNL_ENV(aEnv) );
        case 4:
            (void) knlDestroyRegionMem( & aDBCStmt->mShareMemDataPlan,
                                        KNL_ENV(aEnv) );
        case 5:
            (void) knlDestroyRegionMem( & aDBCStmt->mShareMemStmt,
                                        KNL_ENV(aEnv) );
        case 6:
        default:
            break;
    }
    
    return STL_FAILURE;
}



/**
 * @brief DBC Statement 에 Granted Privilege 를 추가한다.
 * @param[in]   aDBCStmt     DBC Statement
 * @param[in]   aSQLStmt     SQL Statement
 * @param[in]   aPrivObject  Privilege Object 유형 
 * @param[in]   aPrivHandle  Privilege Handle
 * @param[in]   aEnv         Environment
 * @remarks
 */
stlStatus qllAddGrantedPriv( qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             ellPrivObject   aPrivObject,
                             ellDictHandle * aPrivHandle,
                             qllEnv        * aEnv )
{
    stlBool sDuplicate;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mGrantedPrivList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aPrivObject > ELL_PRIV_NA) && (aPrivObject < ELL_PRIV_MAX),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivHandle != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Privilege 를 추가, 중복은 제거됨
     */

    STL_TRY( ellAddNewPrivItem( aSQLStmt->mGrantedPrivList,
                                aPrivObject,
                                aPrivHandle,
                                NULL,  /* aPrivDesc */
                                & sDuplicate,
                                & aDBCStmt->mShareMemStmt,
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DBC Statement 의 Memory Allocator를 Plan Cache Allocator로 변경한다.
 * @param[in]   aDBCStmt     DBC Statement
 * @remarks
 */
void qllPlanCacheMemON( qllDBCStmt * aDBCStmt )
{
    aDBCStmt->mShareMemCodePlan = & (aDBCStmt->mPlanCacheMem);
}


/**
 * @brief DBC Statement 의 Memory Allocator를 Non-Plan Cache Allocator로 변경한다.
 * @param[in]   aDBCStmt     DBC Statement
 * @remarks
 */
void qllPlanCacheMemOFF( qllDBCStmt * aDBCStmt )
{
    aDBCStmt->mShareMemCodePlan = & (aDBCStmt->mNonPlanCacheMem);
}

/** @} qllDBCStmt */
