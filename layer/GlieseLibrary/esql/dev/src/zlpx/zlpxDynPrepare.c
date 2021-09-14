/*******************************************************************************
 * zlpxDynPrepare.c
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

#include <stl.h>
#include <goldilocks.h>
#include <zllExtendedDescribeParam.h>
#include <zlpeSqlca.h>
#include <zlpuMisc.h>
#include <zlpxDynExecute.h>
#include <zlpxDynPrepare.h>
#include <zlpyDynStmt.h>
#include <zlpbBind.h>

/**
 * @file zlpxDynPrepare.c
 * @brief Gliese Embedded SQL in C precompiler SQL execution library functions
 */

/**
 * @defgroup zlpxDynPrepare  PREPARE library
 * @ingroup DynamicSQL
 * @{
 */

/**
 * @brief (internal) PREPARE 를 수행함
 * @param[in] aSqlContext    SQL Context
 * @param[in] aSqlArgs       SQL Arguments
 * @param[in] aStmtName      Statement Name
 * @remarks
 */
stlStatus zlpxDynPrepare( zlplSqlContext   * aSqlContext,
                          zlplSqlArgs      * aSqlArgs,
                          stlChar          * aStmtName )
{
    SQLRETURN           sReturn;

    zlplDynStmtSymbol  * sSymbol = NULL;

    stlChar        * sStringSQL = NULL;
    SQLINTEGER       sStringLen = 0;

    zlplSqlHostVar * sHostSQL = NULL;
    zlplVarchar    * sVarcharHost = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSqlArgs != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_PARAM_VALIDATE( aSqlArgs->sqltype == ZTPL_STMT_DYNAMIC_SQL_PREPARE,
                        ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_PARAM_VALIDATE( aStmtName != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );

    /**
     * 초기화
     */

    ZLPL_INIT_SQLCA( aSqlContext, aSqlArgs->sql_ca );

    STL_TRY( zlpeIsInit() == STL_SUCCESS );

    /***************************************************************
     * 수행 준비
     ***************************************************************/

    STL_TRY( zlpbAdjustDataType( aSqlContext,
                                 aSqlArgs )
             == STL_SUCCESS );

    /**
     * SQL String 정보 획득
     */

    if ( aSqlArgs->sqlstmt != NULL )
    {
        /**
         * PREPARE stmt FROM 'sql string' 인 경우
         */

        STL_DASSERT( aSqlArgs->hvc == 0 );

        sStringSQL = aSqlArgs->sqlstmt;
        sStringLen = SQL_NTS;
    }
    else
    {
        /**
         *  PREPARE stmt FROM :sStringVariable 인 경우
         */

        STL_DASSERT( aSqlArgs->hvc == 1 );
        STL_DASSERT( aSqlArgs->sqlhv != NULL );

        /**
         * @todo Array Host Variable 고려해야 함
         */

        /**
         * C type 에 따른 String 정보 획득
         */

        sHostSQL = (zlplSqlHostVar *) aSqlArgs->sqlhv;
        if ( sHostSQL[0].data_type == ZLPL_C_DATATYPE_VARCHAR )
        {
            sVarcharHost = (zlplVarchar *) sHostSQL[0].value;
            sStringSQL = sVarcharHost->mArr;
            sStringLen = sVarcharHost->mLen;
        }
        else
        {
            sStringSQL = (stlChar *) sHostSQL[0].value;
            sStringLen = SQL_NTS;
        }
    }

    /**
     * Dynamic Statement 를 검색
     */

    STL_TRY( zlpyLookupDynStmtSymbol( aSqlContext,
                                      aStmtName,
                                      & sSymbol )
            == STL_SUCCESS);

    /**
     * ODBC Statemnet Handle 획득
     */

    if( sSymbol == NULL )
    {
        /**
         * Dynamic Statement Symbol 생성
         */

        STL_TRY( zlpxMakeDynStmtSymbol( aSqlContext,
                                        aSqlArgs,
                                        aStmtName,
                                        & sSymbol )
                == STL_SUCCESS);

        /**
         * Dynamic Statement Symbol 을 등록
         */

        STL_TRY( zlpyAddDynStmtSymbol( aSqlContext, sSymbol ) == STL_SUCCESS );

    }
    else
    {
        /**
         * 이미 Prepare 되어 있는 경우
         * - 기존 SQL String 과 동일한 지 검사
         */

        if ( sSymbol->mIsPrepared == STL_TRUE )
        {
            if ( sStringLen == SQL_NTS )
            {
                if ( stlStrcmp( sStringSQL, sSymbol->mPrevSQL ) == 0 )
                {
                    /* 기존과 동일한 경우 다시 prepare 하지 않는다. */
                    STL_THROW( RAMP_FINISH );
                }
                else
                {
                    /* 서로 다른 SQL 인 경우 */
                }
            }
            else
            {
                if ( ( sStringLen == stlStrlen( sSymbol->mPrevSQL ) ) &&
                     ( stlStrncmp( sStringSQL, sSymbol->mPrevSQL, sStringLen ) == 0 ) )
                {
                    /* 기존과 동일한 경우 다시 prepare 하지 않는다. */
                    STL_THROW( RAMP_FINISH );
                }
                else
                {
                    /* 서로 다른 SQL 인 경우 */
                }
            }
        }
        else
        {
            /* 이전 prepare 가 성공하지 못한 경우 */
        }

        /**
         * BindParameter 정보 초기화
         */

        sReturn = SQLFreeStmt( sSymbol->mStmtHandle, SQL_RESET_PARAMS );

        STL_TRY( zlpeCheckError( sReturn,
                                 SQL_HANDLE_STMT,
                                 sSymbol->mStmtHandle,
                                 (zlplSqlca *) aSqlArgs->sql_ca,
                                 (stlChar *) aSqlArgs->sql_state )
                 == STL_SUCCESS);

        /**
         * BindCol 정보 초기화
         */

        sReturn = SQLFreeStmt( sSymbol->mStmtHandle, SQL_UNBIND );

        STL_TRY( zlpeCheckError( sReturn,
                                 SQL_HANDLE_STMT,
                                 sSymbol->mStmtHandle,
                                 (zlplSqlca *) aSqlArgs->sql_ca,
                                 (stlChar *) aSqlArgs->sql_state )
                 == STL_SUCCESS);

        /**
         * 메모리 공간 회수
         */

        STL_TRY( zlpxFreeDynStmtSymbolMemory( aSqlContext,
                                              sSymbol )
                 == STL_SUCCESS );
    }

    /***************************************************************
     * Dynamic Statement 를 Prepare
     ***************************************************************/

    /**
     * SQL 구문 저장
     */

    if ( sSymbol->mPrevSQL != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                    sSymbol->mPrevSQL,
                                    ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        sSymbol->mPrevSQL = NULL;
    }

    if ( sStringLen == SQL_NTS )
    {
        sStringLen = stlStrlen(sStringSQL);
    }
    else
    {
        /* nothing to do */
    }

    STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                 sStringLen + 1,
                                 (void **) & sSymbol->mPrevSQL,
                                 ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    stlMemcpy( sSymbol->mPrevSQL, sStringSQL, sStringLen );
    sSymbol->mPrevSQL[sStringLen] = '\0';

    /**
     * Dynamic Statement 를 Prepare
     */

    STL_TRY( zlpxPrepareDynStmtSymbol( aSqlContext,
                                       aSqlArgs,
                                       sSymbol )
             == STL_SUCCESS );

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Statement Symbol 을 생성한다.
 * @param[in]  aSqlContext     SQL Context
 * @param[in]  aSqlArgs        SQL Arguments
 * @param[in]  aDynStmtName    Dynamic Statement Name
 * @param[out] aDynStmtSymbol  Dynamic Statement Symbol
 * @remarks
 */
stlStatus zlpxMakeDynStmtSymbol(zlplSqlContext     * aSqlContext,
                                zlplSqlArgs        * aSqlArgs,
                                stlChar            * aDynStmtName,
                                zlplDynStmtSymbol ** aDynStmtSymbol )
{
    SQLHANDLE            sStmtHandle = SQL_NULL_HANDLE;
    zlplDynStmtSymbol  * sSymbol   = NULL;
    zlplConnInfo        *sConnInfo = ZLPL_CTXT_CONNECTION(aSqlContext);

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSqlArgs != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_PARAM_VALIDATE( aDynStmtName != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_PARAM_VALIDATE( aDynStmtSymbol != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );

    /**
     * Dynamic Statement Symbol 을 위한 공간 할당
     */

    STL_TRY_THROW( stlAllocRegionMem( ZLPL_CTXT_ALLOCATOR(aSqlContext),
                                      STL_SIZEOF( zlplDynStmtSymbol),
                                      (void **) & sSymbol,
                                      ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                   == STL_SUCCESS, RAMP_ERR_ALLOCATE_MEM);

    stlStrncpy( sSymbol->mStmtName, aDynStmtName, STL_MAX_SQL_IDENTIFIER_LENGTH );

    /**
     * Statement 할당
     */

    STL_TRY( zlpuAllocStmt( & sStmtHandle, sConnInfo->mDbc, aSqlArgs ) == STL_SUCCESS);

    sSymbol->mStmtHandle    = sStmtHandle;

    sSymbol->mPrevSQL = NULL;
    sSymbol->mIsPrepared = STL_FALSE;

    /**
     * Parameter 정보와 Column 정보 초기화
     */


    sSymbol->mParamCnt = 0;
    sSymbol->mParamStatusArray = NULL;
    sSymbol->mParamAddrArray = NULL;
    sSymbol->mParamIndArray = NULL;
    sSymbol->mParamAddrValueArray = NULL;
    sSymbol->mParamIndValueArray = NULL;
    sSymbol->mParamLocalIndArray = NULL;
    sSymbol->mParamIOType = NULL;

    sSymbol->mColumnCnt = 0;
    sSymbol->mColumnAddrArray = NULL;
    sSymbol->mColumnIndArray = NULL;
    sSymbol->mColumnAddrValueArray = NULL;
    sSymbol->mColumnIndValueArray = NULL;
    sSymbol->mColumnLocalIndArray = NULL;

    /**
     * Output 설정
     */

    *aDynStmtSymbol = sSymbol;

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_ALLOCATE_MEM)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_ALLOCATE_MEM,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Dynamic Statement Symbol 을 Prepare 한다.
 * @param[in]  aSqlContext     SQL Context
 * @param[in]  aSqlArgs        SQL Arguments
 * @param[in]  aDynStmtSymbol  Dynamic Statement Symbol
 * @remarks
 */
stlStatus zlpxPrepareDynStmtSymbol( zlplSqlContext    * aSqlContext,
                                    zlplSqlArgs       * aSqlArgs,
                                    zlplDynStmtSymbol * aDynStmtSymbol )
{
    SQLRETURN      sReturn;
    SQLHANDLE      sStmtHandle = SQL_NULL_HANDLE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSqlArgs != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_PARAM_VALIDATE( aDynStmtSymbol != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );

    /***************************************************************
     * 구문 수행
     ***************************************************************/

    aDynStmtSymbol->mIsPrepared = STL_FALSE;

    /**
     * SQLPrepare() 를 수행
     */

    sStmtHandle = aDynStmtSymbol->mStmtHandle;

    sReturn = SQLPrepare( sStmtHandle,
                          (SQLCHAR *) aDynStmtSymbol->mPrevSQL,
                          SQL_NTS );

    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             sStmtHandle,
                             (zlplSqlca *) aSqlArgs->sql_ca,
                             (stlChar *) aSqlArgs->sql_state )
             == STL_SUCCESS);

    aDynStmtSymbol->mIsPrepared = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    aDynStmtSymbol->mIsPrepared = STL_FALSE;

    return STL_FAILURE;
}


/** @} zlpxDynPrepare */
