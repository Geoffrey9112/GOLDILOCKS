/*******************************************************************************
 * zlpxDynExecute.c
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
#include <zlpbBind.h>
#include <zlpeSqlca.h>
#include <zlpuMisc.h>
#include <zlpxDynExecute.h>
#include <zlpxDynPrepare.h>
#include <zlpyDynStmt.h>

/**
 * @file zlpxDynExecute.c
 * @brief Gliese Embedded SQL in C precompiler SQL execution library functions
 */

/**
 * @defgroup zlpxDynExecute  EXECUTE library
 * @ingroup DynamicSQL
 * @{
 */

/**
 * @brief (internal) EXECUTE 를 수행함
 * @param[in] aSqlContext    SQL Context
 * @param[in] aSqlArgs       SQL Arguments
 * @param[in] aStmtName      Statement Name
 * @remarks
 */
stlStatus zlpxDynExecute( zlplSqlContext   * aSqlContext,
                          zlplSqlArgs      * aSqlArgs,
                          stlChar          * aStmtName )
{
    SQLRETURN           sReturn;

    SQLHANDLE           sStmtHandle = SQL_NULL_HANDLE;

    zlplDynStmtSymbol  * sSymbol = NULL;

    stlInt32       sUsingCnt     = 0;
    stlInt32       sIntoCnt      = 0;

    stlInt32       i = 0;
    SQLHDESC       sDescArd = NULL;
    SQLHDESC       sDescApd = NULL;

    stlInt32       sIteration;

    SQLSMALLINT    sParamCnt;
    SQLSMALLINT    sColumnCnt;

    SQLSMALLINT    sBindIOType;
    SQLSMALLINT    sDataType;
    SQLULEN        sParameterSize;
    SQLSMALLINT    sDecimalDigits;
    SQLSMALLINT    sNullablePtr;
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSqlArgs != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_PARAM_VALIDATE( aSqlArgs->sqltype == ZTPL_STMT_DYNAMIC_SQL_EXECUTE,
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

    /* Get array size */
    (void)zlpuGetMinArraySize( aSqlArgs->iters,
                               aSqlArgs->hvc,
                               aSqlArgs->sqlhv,
                               &sIteration );

    STL_TRY( zlpbAdjustDataType( aSqlContext,
                                 aSqlArgs )
             == STL_SUCCESS );

    /**
     * USING Count 획득
     */

    STL_TRY( zlpxGetDynamicItemCount( aSqlContext,
                                      aSqlArgs->dynusing,
                                      & sUsingCnt )
             == STL_SUCCESS );

    /**
     * INTO Count 획득
     */

    STL_TRY( zlpxGetDynamicItemCount( aSqlContext,
                                      aSqlArgs->dyninto,
                                      & sIntoCnt )
             == STL_SUCCESS );

    /**
     * Dynamic Statement 를 검색
     */

    STL_TRY( zlpyLookupDynStmtSymbol( aSqlContext,
                                      aStmtName,
                                      & sSymbol )
            == STL_SUCCESS);

    STL_TRY_THROW( sSymbol != NULL, RAMP_ERR_INVALID_STMT_NAME );
    sStmtHandle = sSymbol->mStmtHandle;

    if ( sSymbol->mIsPrepared == STL_TRUE )
    {
        /**
         * 이미 유효한 PREPARE 인 경우
         * - nothing to do
         */
    }
    else
    {
        /**
         * Symbol 은 존재하나 PREPARE 되어 있지 않은 경우
         * - 다시 PREPARE 를 시도
         */

        STL_TRY( zlpxPrepareDynStmtSymbol( aSqlContext,
                                           aSqlArgs,
                                           sSymbol )
                 == STL_SUCCESS );
    }

    /**
     * Prepare된 statement의 Parameter와 Column 개수 획득
     */
    sReturn = SQLNumParams( sStmtHandle, & sParamCnt );
    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             sStmtHandle,
                             (zlplSqlca *) aSqlArgs->sql_ca,
                             (stlChar *) aSqlArgs->sql_state )
             == STL_SUCCESS);

    sReturn = SQLNumResultCols( sStmtHandle, & sColumnCnt );
    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             sStmtHandle,
                             (zlplSqlca *) aSqlArgs->sql_ca,
                             (stlChar *) aSqlArgs->sql_state )
             == STL_SUCCESS);


    /***************************************************************
     * 유효성 검사
     ***************************************************************/

    /**
     * USING Count 검사
     */

    if( sUsingCnt == sParamCnt )
    {
        /* no problem */
    }
    else
    {
        if ( sUsingCnt == 0 )
        {
            STL_THROW( RAMP_ERR_USING_CLAUSE_REQUIRED );
        }
        else
        {
            if ( sUsingCnt > sParamCnt )
            {
                STL_THROW( RAMP_ERR_USING_VARIABLE_MISMATCH_OVER );
            }
            else
            {
                STL_THROW( RAMP_ERR_USING_VARIABLE_MISMATCH_LESS );
            }
        }
    }

    /**
     * INTO Count 검사
     */

    if ( sIntoCnt == sColumnCnt )
    {
        /* no problem */
    }
    else
    {
        if ( sIntoCnt == 0 )
        {
            STL_THROW( RAMP_ERR_INTO_CLAUSE_REQUIRED );
        }
        else
        {
            if ( sIntoCnt > sColumnCnt )
            {
                STL_THROW( RAMP_ERR_INTO_VARIABLE_MISMATCH_OVER );
            }
            else
            {
                STL_THROW( RAMP_ERR_INTO_VARIABLE_MISMATCH_LESS );
            }
        }
    }

    /**
     * 여기까지 내려왔으면,
     *   sUsingCnt == sParamCnt
     *   sIntoCnt == sColumnCnt
     * 상태임
     */
    if( (sParamCnt != 0) || (sColumnCnt != 0) )
    {
        /*
         * Cache가 존재하지 않으면 Cache를 생성
         */
        if( zlpxCheckParamCacheExist( sSymbol,
                                      sUsingCnt,
                                      sIntoCnt ) == STL_FALSE )
        {
            /*
             * sSymbol에 저장된, param, column과 using, into 절을 비교
             *  ( matching되는 count가 다르면, SQL 문을 실행 못함 )
             *  1. 저장된 array size가 주어진 sIteration보다 작으면, FALSE
             *  2.
             */
            /**
             * Dynamic Statement symbol 에 Parameter, Column 정보 공간 확보
             */
            STL_TRY( zlpxFreeDynStmtSymbolMemory( aSqlContext,
                                                  sSymbol )
                     == STL_SUCCESS );

            STL_TRY( zlpxAllocDynStmtSymbolMemory( aSqlContext,
                                                   aSqlArgs,
                                                   sSymbol,
                                                   sIteration,
                                                   sUsingCnt,
                                                   sIntoCnt )
                     == STL_SUCCESS );

            /**
             * Bind Parameter I/O 정보 획득
             */

            for ( i = 0; i < sSymbol->mParamCnt; i++ )
            {
                sReturn = zllExtendedDescribeParam( sStmtHandle,
                                                    i + 1,
                                                    & sBindIOType,
                                                    & sDataType,
                                                    & sParameterSize,
                                                    & sDecimalDigits,
                                                    & sNullablePtr );

                STL_TRY( zlpeCheckError( sReturn,
                                         SQL_HANDLE_STMT,
                                         sStmtHandle,
                                         (zlplSqlca *) aSqlArgs->sql_ca,
                                         (stlChar *) aSqlArgs->sql_state )
                         == STL_SUCCESS);

                switch ( sBindIOType )
                {
                    case SQL_PARAM_INPUT:
                        sSymbol->mParamIOType[i] = ZLPL_PARAM_IO_TYPE_IN;
                        break;
                    case SQL_PARAM_INPUT_OUTPUT:
                        sSymbol->mParamIOType[i] = ZLPL_PARAM_IO_TYPE_INOUT;
                        break;
                    case SQL_PARAM_OUTPUT:
                        sSymbol->mParamIOType[i] = ZLPL_PARAM_IO_TYPE_OUT;
                        break;
                    default:
                        STL_DASSERT(0);
                        break;
                }
            }
        }
    }
    else
    {
        /**
         * Parameter와 Column이 존재하지 않는 stmt의 경우는 cache를 pass
         */
    }

    /***************************************************************
     * 구문 수행
     ***************************************************************/

    /**
     * SQLBindParameter() 수행
     */

    if ( aSqlArgs->dynusing == NULL )
    {
        /* nothing to do */
    }
    else
    {
        if ( aSqlArgs->dynusing->isdesc == 0 )
        {
            /**
             * USING :var, ...
             */

            /* 다시 bind 해야 하는 지 check */
            if ( zlpxCheckSameSymbolBindAddr( sSymbol->mParamCnt,
                                              sSymbol->mParamAddrArray,
                                              sSymbol->mParamIndArray,
                                              aSqlArgs->dynusing->dynhv )
                 == STL_TRUE )
            {
                /**
                 * 동일한 Bind Variable 임
                 */

                /**
                 * aSqlArgs 는 block 내의 지역 변수이므로,
                 * 사용자 buffer 공간이 아닌 Symbol의 Indicator 주소를 재설정해주어야 함.
                 */

                sReturn = SQLGetStmtAttr( sStmtHandle,
                                          SQL_ATTR_APP_PARAM_DESC,
                                          &sDescApd,
                                          SQL_IS_POINTER,
                                          NULL );

                STL_TRY( zlpeCheckError(sReturn,
                                        SQL_HANDLE_STMT,
                                        sStmtHandle,
                                        (zlplSqlca *)aSqlArgs->sql_ca,
                                        (stlChar *)aSqlArgs->sql_state)
                         == STL_SUCCESS );

                for ( i = 0; i < aSqlArgs->dynusing->hvc; i++ )
                {
                    sReturn = SQLSetDescField( sDescApd,
                                               i + 1,
                                               SQL_DESC_INDICATOR_PTR,
                                               (SQLPOINTER) & sSymbol->mParamLocalIndArray[i][0],
                                               0 );

                    STL_TRY( zlpeCheckError(sReturn,
                                            SQL_HANDLE_DESC,
                                            sDescApd,
                                            (zlplSqlca *)aSqlArgs->sql_ca,
                                            (stlChar *)aSqlArgs->sql_state)
                            == STL_SUCCESS );
                }
            }
            else
            {
                /**
                 * SQLBindParameter() 수행
                 */

                STL_TRY( zlpbBindParams( sSymbol->mStmtHandle,
                                         aSqlArgs,
                                         aSqlArgs->dynusing->hvc,
                                         aSqlArgs->dynusing->dynhv,
                                         sSymbol->mParamAddrValueArray,
                                         sSymbol->mParamLocalIndArray,
                                         sSymbol->mParamIOType )
                         == STL_SUCCESS );

                /**
                 * 반복적인 BindParameter() 방지를 위해 Host Variable 의 Address 저장
                 */
                zlpxStoreSymbolBindAddr( sSymbol->mParamCnt,
                                         sSymbol->mParamAddrArray,
                                         sSymbol->mParamIndArray,
                                         aSqlArgs->dynusing->dynhv );
            }
        }
        else
        {
            /**
             * USING DESCRIPTOR ..
             */

            STL_THROW( RAMP_NOT_IMPLEMENTED );
        }
    }

    /**
     * SQLBindCol() 수행
     */

    if ( aSqlArgs->dyninto == NULL )
    {
        /* nothing to do */
    }
    else
    {
        if ( aSqlArgs->dyninto->isdesc == 0 )
        {
            /**
             * INTO :var, ...
             */

            /* 다시 bind 해야 하는 지 check */
            if ( zlpxCheckSameSymbolBindAddr( sSymbol->mColumnCnt,
                                              sSymbol->mColumnAddrArray,
                                              sSymbol->mColumnIndArray,
                                              aSqlArgs->dyninto->dynhv )
                 == STL_TRUE )
            {
                /**
                 * 동일한 Bind Variable 임
                 */

                /**
                 * aSqlArgs 는 block 내의 지역 변수이므로,
                 * 사용자 buffer 공간이 아닌 Symbol 의 Local Indicator 주소를 재설정해주어야 함.
                 */

                sReturn = SQLGetStmtAttr( sStmtHandle,
                                          SQL_ATTR_APP_ROW_DESC,
                                          &sDescArd,
                                          SQL_IS_POINTER,
                                          NULL );

                STL_TRY( zlpeCheckError(sReturn,
                                        SQL_HANDLE_STMT,
                                        sStmtHandle,
                                        (zlplSqlca *)aSqlArgs->sql_ca,
                                        (stlChar *)aSqlArgs->sql_state)
                         == STL_SUCCESS );

                for ( i = 0; i < aSqlArgs->dyninto->hvc; i++ )
                {
                    sReturn = SQLSetDescField( sDescArd,
                                               i + 1,
                                               SQL_DESC_INDICATOR_PTR,
                                               (SQLPOINTER) & sSymbol->mColumnLocalIndArray[i][0],
                                               0 );

                    STL_TRY( zlpeCheckError(sReturn,
                                            SQL_HANDLE_DESC,
                                            sDescArd,
                                            (zlplSqlca *)aSqlArgs->sql_ca,
                                            (stlChar *)aSqlArgs->sql_state)
                            == STL_SUCCESS );
                }
            }
            else
            {
                /**
                 * SQLBindCol() 수행
                 */

                STL_TRY( zlpbBindCols( sSymbol->mStmtHandle,
                                       aSqlArgs,
                                       aSqlArgs->dyninto->hvc,
                                       aSqlArgs->dyninto->dynhv,
                                       sSymbol->mColumnAddrValueArray,
                                       sSymbol->mColumnLocalIndArray )
                         == STL_SUCCESS );

                /**
                 * 반복적인 SQLBindCol() 방지를 위해 Host Variable 의 Address 저장
                 */

                zlpxStoreSymbolBindAddr( sSymbol->mColumnCnt,
                                         sSymbol->mColumnAddrArray,
                                         sSymbol->mColumnIndArray,
                                         aSqlArgs->dyninto->dynhv );
            }
        }
        else
        {
            /**
             * INTO DESCRIPTOR ..
             */

            STL_THROW( RAMP_NOT_IMPLEMENTED );
        }
    }

    /***************************************************************
     * EXECUTE 수행
     ***************************************************************/

    /**
     * 수행전 C 변수에 대한 정보 설정
     */

    if ( aSqlArgs->dynusing == NULL )
    {
        /* nothing to do */
    }
    else
    {
        if ( aSqlArgs->dynusing->isdesc == 0 )
        {
            /**
             * USING :var, ...
             */

            STL_TRY( zlpbSetBeforeInfo( aSqlContext,
                                        aSqlArgs,
                                        sStmtHandle,
                                        aSqlArgs->dynusing->hvc,
                                        aSqlArgs->dynusing->dynhv,
                                        sIteration,
                                        &sSymbol->mParamProcessed,
                                        sSymbol->mParamStatusArray,
                                        sSymbol->mParamAddrValueArray,
                                        sSymbol->mParamLocalIndArray,
                                        sSymbol->mParamIOType,
                                        STL_TRUE )    /* Bind Parameter */
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * USING DESCRIPTOR ..
             */

            STL_THROW( RAMP_NOT_IMPLEMENTED );
        }
    }

    /**
     * SQLExecute() 를 수행
     */

    sReturn = SQLExecute( sStmtHandle );

    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             sStmtHandle,
                             (zlplSqlca *) aSqlArgs->sql_ca,
                             (stlChar *) aSqlArgs->sql_state )
             == STL_SUCCESS);

    /**
     * 수행 후 C 변수에 대한 정보 설정
     */

    if ( aSqlArgs->dynusing == NULL )
    {
        /* nothing to do */
    }
    else
    {
        if ( aSqlArgs->dynusing->isdesc == 0 )
        {
            /**
             * USING :var, ...
             */

            if ( sReturn == SQL_NO_DATA )
            {
                /* nohting to do */
            }
            else
            {
                STL_TRY( zlpbSetAfterInfo( aSqlContext,
                                           aSqlArgs,
                                           sStmtHandle,
                                           aSqlArgs->unsafenull,
                                           aSqlArgs->dynusing->hvc,
                                           aSqlArgs->dynusing->dynhv,
                                           sIteration,
                                           sSymbol->mParamAddrValueArray,
                                           sSymbol->mParamLocalIndArray,
                                           sSymbol->mParamIOType )
                         == STL_SUCCESS );
            }
        }
        else
        {
            /**
             * USING DESCRIPTOR ..
             */

            STL_THROW( RAMP_NOT_IMPLEMENTED );
        }
    }

    /***************************************************************
     * 마무리 작업
     ***************************************************************/

    /**
     * INTO .. 구문이 존재할 경우 SQLFetch() 수행
     */

    if ( aSqlArgs->dyninto == NULL )
    {
        /* nothing to do */
    }
    else
    {
        /**
         * Fetch 수행
         */

        sReturn = SQLFetch( sStmtHandle );

        STL_TRY( zlpeCheckError( sReturn,
                                 SQL_HANDLE_STMT,
                                 sStmtHandle,
                                 (zlplSqlca *) aSqlArgs->sql_ca,
                                 (stlChar *) aSqlArgs->sql_state )
                 == STL_SUCCESS);

        if ( sReturn == SQL_NO_DATA )
        {
            /* nothing to do */
        }
        else
        {
            /**
             * 결과가 있는 경우, FETCH 후 정보 처리
             */

            if ( aSqlArgs->dyninto->isdesc == 0 )
            {
                /**
                 * INTO :var, ...
                 */

                STL_TRY( zlpbSetAfterInfo( aSqlContext,
                                           aSqlArgs,
                                           sStmtHandle,
                                           aSqlArgs->unsafenull,
                                           aSqlArgs->dyninto->hvc,
                                           aSqlArgs->dyninto->dynhv,
                                           sIteration,
                                           sSymbol->mColumnAddrValueArray,
                                           sSymbol->mColumnLocalIndArray,
                                           NULL )   /* aDynParamIOType */
                         == STL_SUCCESS );
            }
            else
            {
                /**
                 * USING DESCRIPTOR ..
                 */

                STL_THROW( RAMP_NOT_IMPLEMENTED );
            }
        }

        /**
         * Close Cursor 수행
         * 한 건만 Fetch 하고 SQLCloseCursor() 수행
         */

        if ( sReturn == SQL_SUCCESS )
        {
            sReturn = SQLCloseCursor( sStmtHandle );

            STL_TRY( zlpeCheckError( sReturn,
                                     SQL_HANDLE_STMT,
                                     sStmtHandle,
                                     (zlplSqlca *) aSqlArgs->sql_ca,
                                     (stlChar *) aSqlArgs->sql_state )
                     == STL_SUCCESS);
        }
        else
        {
            /**
             * Fetch 에 대한 Result Code 정보를 유지한다.
             */

            (void) SQLCloseCursor( sStmtHandle );
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STMT_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_STATEMEMT_NAME_NOT_PREPARED,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext),
                      aStmtName );
    }

    STL_CATCH( RAMP_ERR_USING_CLAUSE_REQUIRED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_USING_CLAUSE_REQUIRED_FOR_DYNAMIC_PARAMTERS,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    }

    STL_CATCH( RAMP_ERR_USING_VARIABLE_MISMATCH_OVER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_USING_CLAUSE_ILLEGAL_VARIABLE_NAME_NUMBER,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    }

    STL_CATCH( RAMP_ERR_USING_VARIABLE_MISMATCH_LESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_USING_CLAUSE_NOT_ALL_VARIABLES_BOUND,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    }

    STL_CATCH( RAMP_ERR_INTO_CLAUSE_REQUIRED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_INTO_CLAUSE_REQUIRED_FOR_RESULT_FIELDS,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    }

    STL_CATCH( RAMP_ERR_INTO_VARIABLE_MISMATCH_OVER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_INTO_CLAUSE_ILLEGAL_VARIABLE_NAME_NUMBER,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    }

    STL_CATCH( RAMP_ERR_INTO_VARIABLE_MISMATCH_LESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_INTO_CLAUSE_NOT_ALL_VARIABLES_BOUND,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    }

    STL_CATCH( RAMP_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_NOT_IMPLEMENTED_FEATURE,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext),
                      "zlpxDynExecute()" );
    }

    STL_FINISH;

    if ( aSqlArgs->dyninto != NULL )
    {
        /**
         * SELECT 구문에 대한 EXECUTE 도중 에러가 발생했다면 Cursor 를 닫는다.
         */
        (void) SQLCloseCursor( sStmtHandle );
    }

    return STL_FAILURE;
}


/**
 * @brief Dynamic Parameter 의 item 개수를 얻는다.
 * @param[in]  aSqlContext    SQL Context
 * @param[in]  aDynParam      Dynamic Parameter
 * @param[out] aItemCount     Item Count
 * @remarks
 */
stlStatus zlpxGetDynamicItemCount( zlplSqlContext   * aSqlContext,
                                   sqldynhv_t       * aDynParam,
                                   stlInt32         * aItemCount )
{
    stlInt32 sCount = 0;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aItemCount != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );

    /**
     * Dynamic Parameter 의 유형에 따라 Item 개수를 획득한다.
     */

    if ( aDynParam == NULL )
    {
        sCount  = 0;
    }
    else
    {
        if ( aDynParam->isdesc == 0 )
        {
            /**
             * Host Variable List 인 경우
             */
            sCount = aDynParam->hvc;
        }
        else
        {
            /**
             * Dynamic Descriptor 인 경우
             */

            STL_TRY( zlpxGetDescItemCount( aSqlContext,
                                           aDynParam,
                                           & sCount )
                     == STL_SUCCESS );
        }
    }

    /**
     * Output 설정
     */

    *aItemCount = sCount;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Dynamic Descriptor 의 item 개수를 얻는다.
 * @param[in]  aSqlContext    SQL Context
 * @param[in]  aDynParam      Dynamic Parameter
 * @param[out] aItemCount     Item Count
 * @remarks
 */
stlStatus zlpxGetDescItemCount( zlplSqlContext   * aSqlContext,
                                sqldynhv_t       * aDynParam,
                                stlInt32         * aItemCount )
{
    stlChar          sHostDescName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    stlChar        * sDescName = NULL;

    zlplSqlHostVar * sHostVar = NULL;
    zlplVarchar    * sVarcharHost = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aItemCount != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_PARAM_VALIDATE( aDynParam != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_PARAM_VALIDATE( aDynParam->isdesc != 0, ZLPL_CTXT_ERROR_STACK(aSqlContext) );

    /**
     * Descriptor Name 획득
     */

    if ( aDynParam->descname == NULL )
    {
        /**
         * DESCRIPTOR :sVariable
         */

        STL_DASSERT( aDynParam->hvc == 1 );
        STL_DASSERT( aDynParam->dynhv != NULL );

        /**
         * @todo Array Host Variable 고려해야 함
         */

        /**
         * C type 에 따른 String 정보 획득
         */

        sHostVar = (zlplSqlHostVar *) aDynParam->dynhv;
        if ( sHostVar[0].data_type == ZLPL_C_DATATYPE_VARCHAR )
        {
            sVarcharHost = (zlplVarchar *) sHostVar[0].value;
            STL_TRY_THROW ( sVarcharHost->mLen < STL_MAX_SQL_IDENTIFIER_LENGTH,
                            RAMP_ERR_TOO_LONG_DESCRIPTOR_NAME );

            stlMemcpy( sHostDescName, sVarcharHost->mArr, sVarcharHost->mLen );
            sHostDescName[sVarcharHost->mLen] = '\0';
            sDescName = sHostDescName;
        }
        else
        {
            sDescName = (stlChar *) sHostVar[0].value;

            STL_TRY_THROW ( stlStrlen(sDescName) < STL_MAX_SQL_IDENTIFIER_LENGTH,
                            RAMP_ERR_TOO_LONG_DESCRIPTOR_NAME );
        }

    }
    else
    {
        /**
         * DESCRIPTOR 'desc_name'
         */

        sDescName = aDynParam->descname;

        STL_TRY_THROW ( stlStrlen(sDescName) < STL_MAX_SQL_IDENTIFIER_LENGTH,
                        RAMP_ERR_TOO_LONG_DESCRIPTOR_NAME );

    }

    /**
     * @todo Dynamic Descriptor 검색
     */

    /**
     * @todo Dynamic Descriptor 로부터 Descriptor Item 개수 획득
     */

    STL_THROW( RAMP_NOT_IMPLEMENTED );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LONG_DESCRIPTOR_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_DESCRIPTOR_NAME_IS_TOO_LONG,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    }

    STL_CATCH( RAMP_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_NOT_IMPLEMENTED_FEATURE,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext),
                      "zlplGetDescItemCount()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Symbol 에 저장된 Bind Address 가 Host Variable 의 Address 와 동일한지 검사
 * @param[in]  aDynStmtSymbol  Dynamic Statement Symbol
 * @param[in]  aParamCnt       Parameter count
 * @param[in]  aColumnCnt      Column count
 */

stlBool  zlpxCheckParamCacheExist( zlplDynStmtSymbol * aDynStmtSymbol,
                                   stlInt32            aParamCnt,
                                   stlInt32            aColumnCnt )
{
    if( (aDynStmtSymbol->mParamCnt != aParamCnt) ||
        (aDynStmtSymbol->mColumnCnt != aColumnCnt) )
    {
        return STL_FALSE;
    }

    return STL_TRUE;
}

/**
 * @brief Symbol 에 저장된 Bind Address 가 Host Variable 의 Address 와 동일한지 검사
 * @param[in] aAddrCnt    저장하고 있는 Address 의 개수
 * @param[in] aAddrArray  저장하고 있는 Address Array
 * @param[in] aIndArray   저장하고 있는 Indicator Address Array
 * @param[in] aHostArray  Host Variable Array
 */
stlBool  zlpxCheckSameSymbolBindAddr( stlInt32    aAddrCnt,
                                      stlChar  ** aAddrArray,
                                      stlChar  ** aIndArray,
                                      sqlhv_t   * aHostArray )
{
    stlInt32 i = 0;

    for ( i = 0; i < aAddrCnt; i++ )
    {
        if ( (aAddrArray[i] == aHostArray[i].value) && (aIndArray[i] == aHostArray[i].indicator) )
        {
            /* 동일한 주소임 */
        }
        else
        {
            return STL_FALSE;
        }
    }

    return STL_TRUE;
}

/**
 * @brief Symbol 에 Host Variable 의 Address 를 저장한다.
 * @param[in] aAddrCnt    저장할 Address 의 개수
 * @param[in] aAddrArray  저장할 Address Array
 * @param[in] aIndArray   저장할 Indicator Address Array
 * @param[in] aHostArray  Host Variable Array
 */
void  zlpxStoreSymbolBindAddr( stlInt32    aAddrCnt,
                               stlChar  ** aAddrArray,
                               stlChar  ** aIndArray,
                               sqlhv_t   * aHostArray )
{
    stlInt32 i = 0;

    STL_ASSERT( aAddrCnt > 0 );
    STL_ASSERT( aAddrArray != NULL );
    STL_ASSERT( aIndArray != NULL );
    STL_ASSERT( aHostArray != NULL );

    for ( i = 0; i < aAddrCnt; i++ )
    {
        aAddrArray[i] = aHostArray[i].value;
        aIndArray[i] = aHostArray[i].indicator;
    }
}

/**
 * @brief Dynamic Statement Symbol에서 Parameter, Column 메모리를 할당한다.
 * @param[in]  aSqlContext     SQL Context
 * @param[in]  aSqlArgs        SQL Arguments
 * @param[in]  aDynStmtSymbol  Dynamic Statement Symbol
 * @param[in]  aArraySize      Parameter, Column의 array size
 * @param[in]  aParamCnt       Parameter 개수
 * @param[in]  aColumnCnt      Column 개수
 * @remarks
 */
stlStatus zlpxAllocDynStmtSymbolMemory( zlplSqlContext    * aSqlContext,
                                        zlplSqlArgs       * aSqlArgs,
                                        zlplDynStmtSymbol * aDynStmtSymbol,
                                        stlInt32            aArraySize,
                                        stlInt32            aParamCnt,
                                        stlInt32            aColumnCnt )
{
    stlInt32          sRowSize = 0;
    stlInt32          sAllocSize = 0;
    stlInt32          sAddrAreaSize = 0;
    stlInt32          sRowBufOffset;
    zlplSqlHostVar   *sHostVar;
    stlChar          *sAllocBuffer = NULL;
    sqldynhv_t       *sDynUsing;
    sqldynhv_t       *sDynInto;
    stlInt32          i;

    aDynStmtSymbol->mArraySize = aArraySize;

    if( aArraySize > 1 )
    {
        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     STL_SIZEOF(SQLUSMALLINT) * aArraySize,
                                     (void **) & aDynStmtSymbol->mParamStatusArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
    }
    else
    {
        aDynStmtSymbol->mParamStatusArray = NULL;
    }

    aDynStmtSymbol->mParamCnt = aParamCnt;

    if ( aParamCnt > 0 )
    {
        sDynUsing = aSqlArgs->dynusing;
        sHostVar = sDynUsing->dynhv;

        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     STL_SIZEOF(stlChar *) * aParamCnt,
                                     (void **) & aDynStmtSymbol->mParamAddrArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( aDynStmtSymbol->mParamAddrArray, 0x00, STL_SIZEOF( stlChar *) * aParamCnt );

        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     STL_SIZEOF(stlChar *) * aParamCnt,
                                     (void **) & aDynStmtSymbol->mParamIndArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( aDynStmtSymbol->mParamIndArray, 0x00, STL_SIZEOF( stlChar *) * aParamCnt );

        /*
         * Parameter Value Buffer allocation
         *
         *  --------------------------------------------------------------------------
         *  |           address 영역          ||             Value 영 역               |
         *  --------------------------------------------------------------------------
         *  | V1 ptr | V2 ptr | ... | Vn ptr || V1 space | V2 space | ... | Vn space |
         *  --------------------------------------------------------------------------
         *      |        |               |    ^          ^                ^
         *      |        |               |    |          |                |
         *      ---------+---------------+-----          |                |
         *               |               |               |                |
         *               ----------------+----------------                |
         *                               |                                |
         *                               ----------------------------------
         *
         * 그림과 같이 전체 메모리를 1덩어리로 할당하여, 구역을 분할하여 쓴다.
         * space 공간은 array size 만큼 할당하여, 하나의 Buffer 공간을
         * V1의 array, V2의 array, ..., Vn의 array의 연속된 공간으로 사용한다.
         * (V의 개수의 상관없이, 관리의 편의를 위해 메모리를 1번만 할당하도록 함)
         */
        sAllocSize = 0;
        sRowSize = 0;
        for( i = 0; i < aParamCnt; i ++ )
        {
            sRowSize += ZLPL_ALIGN( zlpbGetDataBufferLength( &sHostVar[i] ) );
        }

        sAddrAreaSize = ZLPL_ALIGN( aParamCnt * STL_SIZEOF(stlChar *) );
        sAllocSize = sAddrAreaSize + sRowSize * aArraySize;
        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     sAllocSize,
                                     (void **) & sAllocBuffer,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( sAllocBuffer, 0x00, sAllocSize );
        aDynStmtSymbol->mParamAddrValueArray = (stlChar **)sAllocBuffer;

        sRowBufOffset = sAddrAreaSize;
        for( i = 0; i < aParamCnt; i ++ )
        {
            aDynStmtSymbol->mParamAddrValueArray[i] = sAllocBuffer + sRowBufOffset;
            sRowBufOffset += ZLPL_ALIGN( zlpbGetDataBufferLength( &sHostVar[i] ) ) * aArraySize;
        }

        /*
         * Parameter Indicator Buffer allocation
         * : 할당 방식은 위의 Value Buffer 할당 방식과 같다.
         */
        sAllocSize = 0;
        sRowSize = 0;
        for( i = 0; i < aParamCnt; i ++ )
        {
            if( sHostVar[i].indicator != NULL )
            {
                sRowSize += ZLPL_ALIGN( zlpbGetDataSize( sHostVar[i].ind_type ) );
            }
        }

        sAddrAreaSize = ZLPL_ALIGN( aParamCnt * STL_SIZEOF(stlChar *) );
        sAllocSize = sAddrAreaSize + sRowSize * aArraySize;
        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     sAllocSize,
                                     (void **) & sAllocBuffer,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( sAllocBuffer, 0x00, sAllocSize );
        aDynStmtSymbol->mParamIndValueArray = (stlChar **)sAllocBuffer;

        sRowBufOffset = sAddrAreaSize;
        for( i = 0; i < aParamCnt; i ++ )
        {
            if( sHostVar[i].indicator == NULL )
            {
                aDynStmtSymbol->mParamIndValueArray[i] = NULL;
            }
            else
            {
                aDynStmtSymbol->mParamIndValueArray[i] = sAllocBuffer + sRowBufOffset;
                sRowBufOffset += ZLPL_ALIGN( zlpbGetDataSize( sHostVar[i].ind_type ) ) * aArraySize;
            }
        }

        /*
         * Parameter Local Indicator Buffer allocation
         * : 할당 방식은 위의 Value Buffer 할당 방식과 같다.
         */
        sAllocSize = 0;
        sRowSize = STL_SIZEOF(SQLLEN) * aParamCnt;

        sAddrAreaSize = ZLPL_ALIGN( aParamCnt * STL_SIZEOF(SQLLEN *) );
        sAllocSize = sAddrAreaSize + sRowSize * aArraySize;
        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     sAllocSize,
                                     (void **) & sAllocBuffer,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( sAllocBuffer, 0x00, sAllocSize );
        aDynStmtSymbol->mParamLocalIndArray = (SQLLEN **)sAllocBuffer;

        sRowBufOffset = sAddrAreaSize;
        for( i = 0; i < aParamCnt; i ++ )
        {
            aDynStmtSymbol->mParamLocalIndArray[i] = (SQLLEN *)(sAllocBuffer + sRowBufOffset);
            sRowBufOffset += STL_SIZEOF(SQLLEN) * aArraySize;
        }

        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     STL_SIZEOF(zlplParamIOType) * aParamCnt,
                                     (void **) & aDynStmtSymbol->mParamIOType,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
    }
    else
    {
        aDynStmtSymbol->mParamStatusArray = NULL;
        aDynStmtSymbol->mParamAddrArray = NULL;
        aDynStmtSymbol->mParamIndArray = NULL;
        aDynStmtSymbol->mParamAddrValueArray = NULL;
        aDynStmtSymbol->mParamIndValueArray = NULL;
        aDynStmtSymbol->mParamLocalIndArray = NULL;

        aDynStmtSymbol->mParamIOType = NULL;
    }

    aDynStmtSymbol->mColumnCnt = aColumnCnt;

    if ( aColumnCnt > 0 )
    {
        sDynInto = aSqlArgs->dyninto;
        sHostVar = sDynInto->dynhv;

        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     STL_SIZEOF(stlChar *) * aColumnCnt,
                                     (void **) & aDynStmtSymbol->mColumnAddrArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( aDynStmtSymbol->mColumnAddrArray, 0x00, STL_SIZEOF( stlChar *) * aColumnCnt );

        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     STL_SIZEOF(stlChar *) * aColumnCnt,
                                     (void **) & aDynStmtSymbol->mColumnIndArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( aDynStmtSymbol->mColumnIndArray, 0x00, STL_SIZEOF( stlChar *) * aColumnCnt );

        /*
         * Column Value Buffer allocation
         *  --------------------------------------------------------------------------
         *  | V1 ptr | V2 ptr | ... | Vn ptr || V1 space | V2 space | ... | Vn space |
         *  --------------------------------------------------------------------------
         *      |        |               |    ^          ^                ^
         *      |        |               |    |          |                |
         *      ---------+---------------+-----          |                |
         *               |               |               |                |
         *               ----------------+----------------                |
         *                               |                                |
         *                               ----------------------------------
         *
         * 그림과 같이 메모리를 할당한다. space 공간은 array size 만큼 할당하여,
         * 하나의 Buffer 공간을 V1의 array, V2의 array, ..., Vn의 array의
         * 연속된 공간으로 사용한다.
         * (V의 개수의 상관없이, 관리의 편의를 위해 메모리를 1번만 할당하도록 함)
         */
        sAllocSize = 0;
        sRowSize = 0;
        for( i = 0; i < aColumnCnt; i ++ )
        {
            sRowSize += zlpbGetDataBufferLength( &sHostVar[i] );
        }

        sAddrAreaSize = ZLPL_ALIGN( aColumnCnt * STL_SIZEOF(stlChar *) );
        sAllocSize = sAddrAreaSize + sRowSize * aArraySize;
        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     sAllocSize,
                                     (void **) & sAllocBuffer,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( sAllocBuffer, 0x00, sAllocSize );
        aDynStmtSymbol->mColumnAddrValueArray = (stlChar **)sAllocBuffer;

        sRowBufOffset = sAddrAreaSize;
        for( i = 0; i < aColumnCnt; i ++ )
        {
            aDynStmtSymbol->mColumnAddrValueArray[i] = sAllocBuffer + sRowBufOffset;
            sRowBufOffset += zlpbGetDataBufferLength( &sHostVar[i] ) * aArraySize;
        }

        /*
         * Parameter Indicator Buffer allocation
         * : 할당 방식은 위의 Value Buffer 할당 방식과 같다.
         */
        sAllocSize = 0;
        sRowSize = 0;
        for( i = 0; i < aColumnCnt; i ++ )
        {
            if( sHostVar[i].indicator != NULL )
            {
                sRowSize += ZLPL_ALIGN( zlpbGetDataSize( sHostVar[i].ind_type ) );
            }
        }

        sAddrAreaSize = ZLPL_ALIGN( aColumnCnt * STL_SIZEOF(stlChar *) );
        sAllocSize = sAddrAreaSize + sRowSize * aArraySize;
        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     sAllocSize,
                                     (void **) & sAllocBuffer,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( sAllocBuffer, 0x00, sAllocSize );
        aDynStmtSymbol->mColumnIndValueArray = (stlChar **)sAllocBuffer;

        sRowBufOffset = sAddrAreaSize;
        for( i = 0; i < aColumnCnt; i ++ )
        {
            if( sHostVar[i].indicator == NULL )
            {
                aDynStmtSymbol->mColumnIndValueArray[i] = NULL;
            }
            else
            {
                aDynStmtSymbol->mColumnIndValueArray[i] = sAllocBuffer + sRowBufOffset;
                sRowBufOffset += ZLPL_ALIGN( zlpbGetDataSize( sHostVar[i].ind_type ) ) * aArraySize;
            }
        }

        /*
         * Parameter Local Indicator Buffer allocation
         * : 할당 방식은 위의 Value Buffer 할당 방식과 같다.
         */
        sAllocSize = 0;
        sRowSize = STL_SIZEOF(SQLLEN) * aColumnCnt;

        sAddrAreaSize = ZLPL_ALIGN( aColumnCnt * STL_SIZEOF(SQLLEN *) );
        sAllocSize = sAddrAreaSize + sRowSize * aArraySize;
        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     sAllocSize,
                                     (void **) & sAllocBuffer,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( sAllocBuffer, 0x00, sAllocSize );
        aDynStmtSymbol->mColumnLocalIndArray = (SQLLEN **)sAllocBuffer;

        sRowBufOffset = sAddrAreaSize;
        for( i = 0; i < aColumnCnt; i ++ )
        {
            aDynStmtSymbol->mColumnLocalIndArray[i] = (SQLLEN *)(sAllocBuffer + sRowBufOffset);
            sRowBufOffset += STL_SIZEOF(SQLLEN) * aArraySize;
        }
    }
    else
    {
        aDynStmtSymbol->mColumnAddrArray = NULL;
        aDynStmtSymbol->mColumnIndArray = NULL;
        aDynStmtSymbol->mColumnAddrValueArray = NULL;
        aDynStmtSymbol->mColumnIndValueArray = NULL;
        aDynStmtSymbol->mColumnLocalIndArray = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Dynamic Statement Symbol에서 Parameter, Column 메모리를 반환한다.
 * @param[in]  aSqlContext     SQL Context
 * @param[in]  aDynStmtSymbol  Dynamic Statement Symbol
 * @remarks
 */
stlStatus zlpxFreeDynStmtSymbolMemory( zlplSqlContext    * aSqlContext,
                                       zlplDynStmtSymbol * aDynStmtSymbol )
{
    aDynStmtSymbol->mArraySize = 0;
    aDynStmtSymbol->mParamCnt = 0;
    aDynStmtSymbol->mColumnCnt = 0;

    if( aDynStmtSymbol->mParamStatusArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                    aDynStmtSymbol->mParamStatusArray,
                                    ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aDynStmtSymbol->mParamStatusArray = NULL;
    }

    if( aDynStmtSymbol->mParamAddrArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aDynStmtSymbol->mParamAddrArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aDynStmtSymbol->mParamAddrArray = NULL;
    }

    if( aDynStmtSymbol->mParamIndArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aDynStmtSymbol->mParamIndArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aDynStmtSymbol->mParamIndArray = NULL;
    }

    if( aDynStmtSymbol->mParamAddrValueArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aDynStmtSymbol->mParamAddrValueArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aDynStmtSymbol->mParamAddrValueArray = NULL;
    }

    if( aDynStmtSymbol->mParamIndValueArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aDynStmtSymbol->mParamIndValueArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aDynStmtSymbol->mParamIndValueArray = NULL;
    }

    if( aDynStmtSymbol->mParamLocalIndArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aDynStmtSymbol->mParamLocalIndArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aDynStmtSymbol->mParamLocalIndArray = NULL;
    }

    if( aDynStmtSymbol->mParamIOType != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aDynStmtSymbol->mParamIOType,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aDynStmtSymbol->mParamIOType = NULL;
    }

    if( aDynStmtSymbol->mColumnAddrArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aDynStmtSymbol->mColumnAddrArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aDynStmtSymbol->mColumnAddrArray = NULL;
    }

    if( aDynStmtSymbol->mColumnIndArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aDynStmtSymbol->mColumnIndArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aDynStmtSymbol->mColumnIndArray = NULL;
    }

    if( aDynStmtSymbol->mColumnAddrValueArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aDynStmtSymbol->mColumnAddrValueArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aDynStmtSymbol->mColumnAddrValueArray = NULL;
    }

    if( aDynStmtSymbol->mColumnIndValueArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aDynStmtSymbol->mColumnIndValueArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aDynStmtSymbol->mColumnIndValueArray = NULL;
    }

    if( aDynStmtSymbol->mColumnLocalIndArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aDynStmtSymbol->mColumnLocalIndArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aDynStmtSymbol->mColumnLocalIndArray = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} zlpxDynExecute */
