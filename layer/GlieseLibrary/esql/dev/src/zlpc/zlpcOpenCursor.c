/*******************************************************************************
 * zlpcOpenCursor.c
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
#include <zlpbBind.h>
#include <zlpeSqlca.h>
#include <zlpcOpenCursor.h>
#include <zlpcFetchCursor.h>
#include <zlpuMisc.h>
#include <zlpxExecute.h>
#include <zlpxDynExecute.h>
#include <zlpyCursor.h>
#include <zlpyDynStmt.h>

/**
 * @file zlpcOpenCursor.c
 * @brief Gliese Embedded SQL in C precompiler SQL cursor library functions
 */

/**
 * @defgroup zlpcOpenCursor   OPEN cursor
 * @addtogroup zlplCursor
 * @{
 */

/**
 * @brief OPEN cursor 구문을 수행한다.
 * @param[in]  aSqlContext   SQL Context
 * @param[in]  aSqlArgs      SQL Arguments
 * @remarks
 */
stlStatus zlpcOpenCursor( zlplSqlContext   *aSqlContext,
                          zlplSqlArgs      *aSqlArgs )
{
    SQLRETURN             sReturn;
    stlInt32              sIteration;

    stlInt32              sUsingCnt = 0;
    SQLSMALLINT           sParamCnt;

    sqlcursor_t         * sDeclCursor = NULL;
    zlplCursorSymbol    * sCursorSymbol = NULL;
    zlplSqlHostVar      * sHostVar = NULL;

    stlBool               sNeedPrepare       = STL_FALSE;
    stlBool               sNeedSetCursorProp = STL_FALSE;
    stlBool               sNeedBindParam     = STL_FALSE;

    stlChar        * sStringSQL = NULL;
    stlInt32         sStringLen = 0;

    stlBool          sSameQuery = STL_FALSE;

    /**
     * 초기화
     */

    ZLPL_INIT_SQLCA(aSqlContext, aSqlArgs->sql_ca);

    STL_TRY( zlpeIsInit() == STL_SUCCESS );

    (void)zlpuGetMinArraySize( aSqlArgs->iters,
                               aSqlArgs->hvc,
                               aSqlArgs->sqlhv,
                               &sIteration );

    STL_TRY( zlpbAdjustDataType( aSqlContext,
                                 aSqlArgs )
             == STL_SUCCESS );

    /**
     * 기본 정보 획득
     */

    sDeclCursor = aSqlArgs->sqlcursor;

    /*********************************************************************
     * Cursor Symbol 검색
     *********************************************************************/

    /**
     * USING Count 획득
     */

    STL_TRY( zlpxGetDynamicItemCount( aSqlContext,
                                      aSqlArgs->dynusing,
                                      & sUsingCnt )
             == STL_SUCCESS );

    /**
     * Cursor Symbol 이 존재하는 지 확인
     */

    STL_TRY( zlpyLookupCursorSymbol( aSqlContext,
                                     aSqlArgs->sqlcursor->sqlcname,
                                     & sCursorSymbol )
            == STL_SUCCESS );

    if ( sCursorSymbol != NULL )
    {
        /**
         * Cursor Query 획득
         */

        STL_TRY( zlpcGetCursorQuery( aSqlContext,
                                     aSqlArgs,
                                     sCursorSymbol,
                                     & sStringSQL,
                                     & sStringLen )
                 == STL_SUCCESS );

        if ( sStringLen == SQL_NTS )
        {
            sStringLen = stlStrlen( sStringSQL );
        }

        if ( sCursorSymbol->mIsPrepared == STL_TRUE )
        {
            /**
             * 이전 OPEN cursor 가 수행이 성공한 경우
             */

            switch ( sDeclCursor->sqlcurprops->org_type )
            {
                case ZLPL_CURSOR_ORIGIN_STANDING_CURSOR:
                    {
                        /**
                         * Standing Cursor 인 경우
                         * - 이미 Prepare 가 되어 있어 Prepare 할 필요가 없음.
                         */

                        /**
                         * 다시 bind 해야 하는 지 check
                         */

                        if ( zlpxCheckSameSymbolBindAddr( sCursorSymbol->mParamCnt,
                                                          sCursorSymbol->mParamAddrArray,
                                                          sCursorSymbol->mParamIndArray,
                                                          aSqlArgs->sqlhv )
                             == STL_TRUE )
                        {
                            /* nothing to do */
                        }
                        else
                        {
                            /**
                             * 다음과 같이 DECLARE CURSOR 구문의 Host 변수가 바뀜
                             * for ( i = 0; i < n; i++ )
                             * {
                             *     DECLARE cur1 CURSOR FOR SELECT * FROM t1 WHERE c1 = :sData[i];
                             *     OPEN cur1;
                             * }
                             */

                            /**
                             * 반복적인 SQLBindParameter() 방지를 위해 Host Variable 의 Address 저장
                             */

                            if ( sCursorSymbol->mParamCnt > 0 )
                            {
                                STL_TRY( zlpbBindParams( sCursorSymbol->mStmtHandle,
                                                         aSqlArgs,
                                                         aSqlArgs->hvc,
                                                         aSqlArgs->sqlhv,
                                                         sCursorSymbol->mParamAddrValueArray,
                                                         sCursorSymbol->mParamLocalIndArray,
                                                         NULL )  /* aDynParamIOType */
                                         == STL_SUCCESS);

                                zlpxStoreSymbolBindAddr( sCursorSymbol->mParamCnt,
                                                         sCursorSymbol->mParamAddrArray,
                                                         sCursorSymbol->mParamIndArray,
                                                         aSqlArgs->sqlhv );
                            }
                            else
                            {
                                /* nothing to do */
                            }
                        }

                        sNeedPrepare       = STL_FALSE;
                        sNeedSetCursorProp = STL_FALSE;
                        break;
                    }
                case ZLPL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR:
                    {
                        /**
                         * 동일한 Dynamic Query 인지 검사
                         */

                        sSameQuery = STL_FALSE;

                        if ( (sStringLen == stlStrlen(sCursorSymbol->mQueryString) ) &&
                             (stlStrcmp( sStringSQL, sCursorSymbol->mQueryString ) == 0 ) )
                        {
                            sSameQuery = STL_TRUE;
                        }
                        else
                        {
                            sSameQuery = STL_FALSE;
                        }

                        if ( sSameQuery == STL_TRUE )
                        {
                            /**
                             * 동일한 Dynamic Query 인 경우, 다시 Prepare 할 필요가 없음
                             */

                            if( sUsingCnt == sCursorSymbol->mParamCnt )
                            {
                                if ( sUsingCnt == 0 )
                                {
                                    sNeedBindParam = STL_FALSE;
                                }
                                else
                                {
                                    /**
                                     * @todo USING DESCRIPTOR 가 고려되어야 함
                                     */

                                    STL_TRY_THROW( aSqlArgs->dynusing->isdesc == 0, RAMP_NOT_IMPLEMENTED );

                                    if ( zlpxCheckSameSymbolBindAddr( sCursorSymbol->mParamCnt,
                                                                      sCursorSymbol->mParamAddrArray,
                                                                      sCursorSymbol->mParamIndArray,
                                                                      aSqlArgs->dynusing->dynhv )
                                         == STL_TRUE )
                                    {
                                        sNeedBindParam = STL_FALSE;
                                    }
                                    else
                                    {
                                        sNeedBindParam = STL_TRUE;
                                    }
                                }

                                if ( sNeedBindParam == STL_TRUE )
                                {
                                    /**
                                     * 다음과 같이 DECLARE CURSOR 구문의 Host 변수가 바뀜
                                     *
                                     *     PREPARE stmt1 FROM SELECT * FROM t1 WHERE :sData;
                                     *     DECLARE cur1 CURSOR FOR stmt1;
                                     *     OPEN cur1 USING :sIntData;
                                     *     OPEN cur1 USING :sLongData;
                                     */

                                    /**
                                     * 반복적인 SQLBindParameter() 방지를 위해 Host Variable 의 Address 저장
                                     */

                                    STL_TRY( zlpbBindParams( sCursorSymbol->mStmtHandle,
                                                             aSqlArgs,
                                                             aSqlArgs->dynusing->hvc,
                                                             aSqlArgs->dynusing->dynhv,
                                                             sCursorSymbol->mParamAddrValueArray,
                                                             sCursorSymbol->mParamLocalIndArray,
                                                             NULL )  /* aDynParamIOType */
                                             == STL_SUCCESS);

                                    zlpxStoreSymbolBindAddr( sCursorSymbol->mParamCnt,
                                                             sCursorSymbol->mParamAddrArray,
                                                             sCursorSymbol->mParamIndArray,
                                                             aSqlArgs->dynusing->dynhv );
                                }
                                else
                                {
                                    /* nothing to do */
                                }
                            }
                            else
                            {
                                /**
                                 * USING parameter 개수와 dynamic query 의 parameter 개수가 다름
                                 */

                                if ( sUsingCnt == 0 )
                                {
                                    STL_THROW( RAMP_ERR_USING_CLAUSE_REQUIRED );
                                }
                                else
                                {
                                    if ( sUsingCnt > sCursorSymbol->mParamCnt )
                                    {
                                        STL_THROW( RAMP_ERR_USING_VARIABLE_MISMATCH_OVER );
                                    }
                                    else
                                    {
                                        STL_THROW( RAMP_ERR_USING_VARIABLE_MISMATCH_LESS );
                                    }
                                }
                            }

                            sNeedPrepare       = STL_FALSE;
                            sNeedSetCursorProp = STL_FALSE;
                        }
                        else
                        {
                            /**
                             * 다른 Dynamic Query 인 경우, 다시 Prepare 해야 함.
                             */

                            sNeedPrepare       = STL_TRUE;
                            sNeedSetCursorProp = STL_TRUE;
                        }

                        break;
                    }
                case ZLPL_CURSOR_ORIGIN_EXTENDED_STATEMENT_CURSOR:
                    {
                        STL_THROW( RAMP_NOT_IMPLEMENTED );
                        break;
                    }
                case ZLPL_CURSOR_ORIGIN_RECEIVED_CURSOR:
                    {
                        STL_THROW( RAMP_NOT_IMPLEMENTED );
                        break;
                    }
                default:
                    {
                        STL_DASSERT(0);
                        break;
                    }
            }
        }
        else
        {
            /**
             * 이전 OPEN cursor 의 수행이 실패한 경우
             */

            sNeedPrepare       = STL_TRUE;
            sNeedSetCursorProp = STL_TRUE;
        }
    }
    else
    {
        /**
         * OPEN cursor 가 수행된 적이 없는 경우
         */

        /**
         * Cursor Symbol 생성
         */

        STL_TRY( zlpcMakeCursorSymbol( aSqlContext, aSqlArgs, & sCursorSymbol )
                 == STL_SUCCESS );

        /**
         * Cursor Symbol 등록
         */

        STL_TRY( zlpyAddCursorSymbol( aSqlContext, sCursorSymbol ) == STL_SUCCESS );

        /**
         * Cursor Query 획득
         */

        STL_TRY( zlpcGetCursorQuery( aSqlContext,
                                     aSqlArgs,
                                     sCursorSymbol,
                                     & sStringSQL,
                                     & sStringLen )
                 == STL_SUCCESS );

        if ( sStringLen == SQL_NTS )
        {
            sStringLen = stlStrlen( sStringSQL );
        }

        sNeedPrepare       = STL_TRUE;
        sNeedSetCursorProp = STL_TRUE;
    }

    /*********************************************************************
     * OPEN cursor 를 위한 PREPARE
     *********************************************************************/

    if ( sNeedPrepare == STL_TRUE )
    {
        /**
         * Cursor Prepare
         */

        STL_TRY( zlpcPrepareCursorQuery( aSqlContext,
                                         aSqlArgs,
                                         sCursorSymbol,
                                         sStringSQL,
                                         sStringLen,
                                         sNeedSetCursorProp )
                 == STL_SUCCESS );

        /**
         * Cursor Query 저장
         */

        if ( sCursorSymbol->mQueryString != NULL )
        {
            STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                        sCursorSymbol->mQueryString,
                                        ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                     == STL_SUCCESS );
        }
        sCursorSymbol->mQueryString = NULL;

        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     sStringLen + 1,
                                     (void **) & sCursorSymbol->mQueryString,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );

        stlMemcpy( sCursorSymbol->mQueryString, sStringSQL, sStringLen );
        sCursorSymbol->mQueryString[sStringLen] = '\0';

        /**
         * USING Count 검사
         */

        sReturn = SQLNumParams( sCursorSymbol->mStmtHandle, & sParamCnt );
        STL_TRY( zlpeCheckError( sReturn,
                                 SQL_HANDLE_STMT,
                                 sCursorSymbol->mStmtHandle,
                                 (zlplSqlca *) aSqlArgs->sql_ca,
                                 (stlChar *) aSqlArgs->sql_state )
                 == STL_SUCCESS);

        switch ( sCursorSymbol->mCursorOriginType )
        {
            case ZLPL_CURSOR_ORIGIN_STANDING_CURSOR:
                {
                    STL_DASSERT( sUsingCnt == 0 );
                    sHostVar = aSqlArgs->sqlhv;
                    break;
                }
            case ZLPL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR:
                {
                    if( sUsingCnt == sParamCnt )
                    {
                        /* no problem */
                        if( sUsingCnt > 0 )
                        {
                            sHostVar = aSqlArgs->dynusing->dynhv;
                        }
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
                    break;
                }
            case ZLPL_CURSOR_ORIGIN_EXTENDED_STATEMENT_CURSOR:
                {
                    STL_THROW( RAMP_NOT_IMPLEMENTED );
                    break;
                }
            case ZLPL_CURSOR_ORIGIN_RECEIVED_CURSOR:
                {
                    STL_THROW( RAMP_NOT_IMPLEMENTED );
                    break;
                }
            default:
                {
                    STL_DASSERT(0);
                    break;
                }
        }

        /**
         * Cursor Symbol 에 Parameter 공간 확보
         */

        STL_TRY( zlpcFreeCursorSymbolParam( aSqlContext,
                                            sCursorSymbol )
                 == STL_SUCCESS );

        STL_TRY( zlpcAllocCursorSymbolParam( aSqlContext,
                                             sCursorSymbol,
                                             sIteration,
                                             sParamCnt,
                                             sHostVar )
                 == STL_SUCCESS );

        /****************************************************************
         * Bind Parameter
         ****************************************************************/

        /**
         * Bind Parameter 수행
         */

        switch ( sCursorSymbol->mCursorOriginType )
        {
            case ZLPL_CURSOR_ORIGIN_STANDING_CURSOR:
            {
                STL_TRY( zlpbBindParams( sCursorSymbol->mStmtHandle,
                                         aSqlArgs,
                                         aSqlArgs->hvc,
                                         aSqlArgs->sqlhv,
                                         sCursorSymbol->mParamAddrValueArray,
                                         sCursorSymbol->mParamLocalIndArray,
                                         NULL )  /* aDynParamIOType */
                         == STL_SUCCESS);
                break;
            }
            case ZLPL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR:
            {
                if ( aSqlArgs->dynusing != NULL )
                {
                    if( aSqlArgs->dynusing->isdesc == 0 )
                    {
                        STL_TRY( zlpbBindParams( sCursorSymbol->mStmtHandle,
                                                 aSqlArgs,
                                                 aSqlArgs->dynusing->hvc,
                                                 aSqlArgs->dynusing->dynhv,
                                                 sCursorSymbol->mParamAddrValueArray,
                                                 sCursorSymbol->mParamLocalIndArray,
                                                 NULL )  /* aDynParamIOType */
                                 == STL_SUCCESS);
                    }
                    else
                    {
                        STL_THROW( RAMP_NOT_IMPLEMENTED );
                    }
                }
                else
                {
                    /* nohting to do */
                }
                break;
            }
            case ZLPL_CURSOR_ORIGIN_EXTENDED_STATEMENT_CURSOR:
            {
                STL_THROW( RAMP_NOT_IMPLEMENTED );
                break;
            }
            case ZLPL_CURSOR_ORIGIN_RECEIVED_CURSOR:
            {
                STL_THROW( RAMP_NOT_IMPLEMENTED );
                break;
            }
            default:
            {
                STL_DASSERT(0);
                break;
            }
        }
        /**
         * Cursor Name 설정
         */

        if ( sCursorSymbol->mIsSetCursorName == STL_TRUE )
        {
            /* nothing to do */
        }
        else
        {
            if ( sCursorSymbol->mForUpdate == STL_TRUE )
            {
                sReturn = SQLSetCursorName( sCursorSymbol->mStmtHandle,
                                            (SQLCHAR *) sCursorSymbol->mCursorName,
                                            SQL_NTS );

                STL_TRY( zlpeCheckError( sReturn,
                                         SQL_HANDLE_STMT,
                                         sCursorSymbol->mStmtHandle,
                                         (zlplSqlca *) aSqlArgs->sql_ca,
                                         (stlChar *) aSqlArgs->sql_state)
                         == STL_SUCCESS);

                sCursorSymbol->mIsSetCursorName = STL_TRUE;
            }
            else
            {
                /* updatable cursor 가 아닌 경우 */
            }
        }

        /**
         * 반복적인 SQLBindParameter() 방지를 위해 Host Variable 의 Address 저장
         */

        switch ( sCursorSymbol->mCursorOriginType )
        {
            case ZLPL_CURSOR_ORIGIN_STANDING_CURSOR:
                {
                    if ( sCursorSymbol->mParamCnt > 0 )
                    {
                        zlpxStoreSymbolBindAddr( sCursorSymbol->mParamCnt,
                                                 sCursorSymbol->mParamAddrArray,
                                                 sCursorSymbol->mParamIndArray,
                                                 aSqlArgs->sqlhv );
                    }
                    else
                    {
                        /* nothing to do */
                    }
                    break;
                }
            case ZLPL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR:
                {
                    if ( sCursorSymbol->mParamCnt > 0 )
                    {
                        /**
                         * @todo USING DESCRIPTOR 가 고려되어야 함
                         */

                        STL_TRY_THROW( aSqlArgs->dynusing->isdesc == 0, RAMP_NOT_IMPLEMENTED );

                        /**
                         * 반복적인 SQLBindParameter() 방지를 위해 Host Variable 의 Address 저장
                         */

                        zlpxStoreSymbolBindAddr( sCursorSymbol->mParamCnt,
                                                 sCursorSymbol->mParamAddrArray,
                                                 sCursorSymbol->mParamIndArray,
                                                 aSqlArgs->dynusing->dynhv );
                    }
                    else
                    {
                        /* nothing to do */
                    }
                    break;
                }
            case ZLPL_CURSOR_ORIGIN_EXTENDED_STATEMENT_CURSOR:
                {
                    STL_THROW( RAMP_NOT_IMPLEMENTED );
                    break;
                }
            case ZLPL_CURSOR_ORIGIN_RECEIVED_CURSOR:
                {
                    STL_THROW( RAMP_NOT_IMPLEMENTED );
                    break;
                }
            default:
                {
                    STL_DASSERT(0);
                    break;
                }
        }
    }
    else
    {
        /**
         * PREPARE 할 필요가 없는 경우
         */

        /**
         * Cursor Name 설정
         * - CLOSE cursor 가 되면, cursor name 이 제거되므로 매번 설정해 주어야 함.
         */

        if ( sCursorSymbol->mIsSetCursorName == STL_TRUE )
        {
            /* nothing to do */
        }
        else
        {
            if ( sCursorSymbol->mForUpdate == STL_TRUE )
            {
                sReturn = SQLSetCursorName( sCursorSymbol->mStmtHandle,
                                            (SQLCHAR *) sCursorSymbol->mCursorName,
                                            SQL_NTS );

                STL_TRY( zlpeCheckError( sReturn,
                                         SQL_HANDLE_STMT,
                                         sCursorSymbol->mStmtHandle,
                                         (zlplSqlca *) aSqlArgs->sql_ca,
                                         (stlChar *) aSqlArgs->sql_state )
                         == STL_SUCCESS);

                sCursorSymbol->mIsSetCursorName = STL_TRUE;
            }
            else
            {
                /* updatable cursor 가 아닌 경우 */
            }
        }
    }

    /*********************************************************************
     * OPEN cursor 를 위한 EXECUTE
     *********************************************************************/

    /**
     * OPEN 수행전 정보 설정
     */

    switch ( sCursorSymbol->mCursorOriginType )
    {
        case ZLPL_CURSOR_ORIGIN_STANDING_CURSOR:
            {
                STL_TRY(zlpbSetBeforeInfo( aSqlContext,
                                           aSqlArgs,
                                           sCursorSymbol->mStmtHandle,
                                           aSqlArgs->hvc,
                                           aSqlArgs->sqlhv,
                                           sIteration,
                                           &sCursorSymbol->mParamProcessed,
                                           sCursorSymbol->mParamStatusArray,
                                           sCursorSymbol->mParamAddrValueArray,
                                           sCursorSymbol->mParamLocalIndArray,
                                           NULL,       /* aDynParamIOType */
                                           STL_TRUE )  /* Bind parameter  */
                        == STL_SUCCESS);
                break;
            }
        case ZLPL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR:
            {
                if ( aSqlArgs->dynusing != NULL )
                {
                    if ( aSqlArgs->dynusing->isdesc == 0 )
                    {
                        STL_TRY(zlpbSetBeforeInfo( aSqlContext,
                                                   aSqlArgs,
                                                   sCursorSymbol->mStmtHandle,
                                                   aSqlArgs->dynusing->hvc,
                                                   aSqlArgs->dynusing->dynhv,
                                                   sIteration,
                                                   &sCursorSymbol->mParamProcessed,
                                                   sCursorSymbol->mParamStatusArray,
                                                   sCursorSymbol->mParamAddrValueArray,
                                                   sCursorSymbol->mParamLocalIndArray,
                                                   NULL,       /* aDynParamIOType */
                                                   STL_TRUE )  /* Bind parameter  */
                                == STL_SUCCESS);
                    }
                    else
                    {
                        STL_THROW( RAMP_NOT_IMPLEMENTED );
                    }
                }
                else
                {
                    STL_DASSERT( sUsingCnt == 0 );
                }
                break;
            }
        case ZLPL_CURSOR_ORIGIN_EXTENDED_STATEMENT_CURSOR:
            {
                STL_THROW( RAMP_NOT_IMPLEMENTED );
                break;
            }
        case ZLPL_CURSOR_ORIGIN_RECEIVED_CURSOR:
            {
                STL_THROW( RAMP_NOT_IMPLEMENTED );
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }

    /**
     * OPEN 을 Execute
     */

    sReturn = SQLExecute( sCursorSymbol->mStmtHandle );

    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             sCursorSymbol->mStmtHandle,
                             (zlplSqlca *)aSqlArgs->sql_ca,
                             (stlChar *)aSqlArgs->sql_state )
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_CATCH( RAMP_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_NOT_IMPLEMENTED_FEATURE,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext),
                      "zlpcOpenCursor()" );
    }

    STL_CATCH( RAMP_ERR_USING_CLAUSE_REQUIRED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_USING_CLAUSE_REQUIRED_FOR_DYNAMIC_PARAMTERS,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );

        if( sCursorSymbol != NULL )
        {
            sCursorSymbol->mIsPrepared = STL_FALSE;
            (void)zlpcFreeCursorSymbolParam( aSqlContext,
                                             sCursorSymbol );
            (void)zlpcFreeCursorSymbolColumn( aSqlContext,
                                              sCursorSymbol );
        }
    }

    STL_CATCH( RAMP_ERR_USING_VARIABLE_MISMATCH_OVER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_USING_CLAUSE_ILLEGAL_VARIABLE_NAME_NUMBER,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );

        if( sCursorSymbol != NULL )
        {
            sCursorSymbol->mIsPrepared = STL_FALSE;
            (void)zlpcFreeCursorSymbolParam( aSqlContext,
                                             sCursorSymbol );
            (void)zlpcFreeCursorSymbolColumn( aSqlContext,
                                              sCursorSymbol );
        }
    }

    STL_CATCH( RAMP_ERR_USING_VARIABLE_MISMATCH_LESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_USING_CLAUSE_NOT_ALL_VARIABLES_BOUND,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );

        if( sCursorSymbol != NULL )
        {
            sCursorSymbol->mIsPrepared = STL_FALSE;
            (void)zlpcFreeCursorSymbolParam( aSqlContext,
                                             sCursorSymbol );
            (void)zlpcFreeCursorSymbolColumn( aSqlContext,
                                              sCursorSymbol );
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Cursor Symbol 을 생성한다.
 * @param[in]  aSqlContext     SQL Context
 * @param[in]  aSqlArgs        SQL Arguments
 * @param[out] aCursorSymbol   Cursor Symbol
 * @remarks
 */
stlStatus zlpcMakeCursorSymbol( zlplSqlContext    * aSqlContext,
                                zlplSqlArgs       * aSqlArgs,
                                zlplCursorSymbol ** aCursorSymbol )
{
    zlplCursorSymbol   * sSymbol     = NULL;
    zlplConnInfo       * sConnInfo   = ZLPL_CTXT_CONNECTION(aSqlContext);
    sqlcursor_t        * sDeclCursor = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSqlArgs != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_PARAM_VALIDATE( aCursorSymbol != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );

    /**
     * 기본 정보 획득
     */

    sDeclCursor = aSqlArgs->sqlcursor;

    /**
     * Dynamic Statement Symbol 을 위한 공간 할당
     */

    STL_TRY_THROW( stlAllocRegionMem( ZLPL_CTXT_ALLOCATOR(aSqlContext),
                                      STL_SIZEOF( zlplCursorSymbol),
                                      (void **) & sSymbol,
                                      ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                   == STL_SUCCESS, RAMP_ERR_ALLOCATE_MEM);

    /**
     * 기본 정보 설정
     */

    stlStrncpy( sSymbol->mCursorName, sDeclCursor->sqlcname, STL_MAX_SQL_IDENTIFIER_LENGTH );
    sSymbol->mCursorOriginType = sDeclCursor->sqlcurprops->org_type;

    /**
     * @todo STANDING CURSOR 가 아닌 경우 Dynamic Statement Name 이 반드시 존재해야 한다.
     */

    switch ( sSymbol->mCursorOriginType )
    {
        case ZLPL_CURSOR_ORIGIN_STANDING_CURSOR:
            {
                sSymbol->mDynStmtName[0] = '\0';
                break;
            }
        case ZLPL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR:
            {
                stlStrncpy( sSymbol->mDynStmtName, sDeclCursor->sqldynstmt, STL_MAX_SQL_IDENTIFIER_LENGTH );
                break;
            }
        case ZLPL_CURSOR_ORIGIN_EXTENDED_STATEMENT_CURSOR:
            {
                STL_THROW( RAMP_NOT_IMPLEMENTED );
                break;
            }
        case ZLPL_CURSOR_ORIGIN_RECEIVED_CURSOR:
            {
                STL_THROW( RAMP_NOT_IMPLEMENTED );
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /**
     * Statement 할당
     *
     * Dynamic Cursor 의 경우 PREPARE 구문에 이미 statement 가 이미 SQLPrepare() 되고 handle 도 존재하나,
     * 다음과 같은 이유로 이를 사용할 수 없다.
     * - Cursor Property 는 SQLPrepare() 전에 설정되어야 함
     * - 다음과 같이 하나의 dynamic statement 로 다수의 Cursor 를 만드는 경우
     *  ; PREPARE stmt1 FROM :sSQL;
     *  ; DECLARE cur1 CURSOR FOR stmt1;
     *  ; DECLARE cur2 CURSOR FOR stmt1;
     *  ; OPEN cur1;
     *  ; OPEN cur2;
     * PREPARE 구문에 대해 SQLPrepare() 를 하지 않는 approach 도 있으나.. 다수의 난제가 발생함.
     * - PREPARE stmt1 FROM :sSQL;
     * - DESCRIBE INPUT stmt1 USING 'desc';
     */

    STL_TRY( zlpuAllocStmt( & sSymbol->mStmtHandle, sConnInfo->mDbc, aSqlArgs ) == STL_SUCCESS );

    sSymbol->mQueryString = NULL;
    sSymbol->mIsPrepared = STL_FALSE;
    sSymbol->mIsSetCursorName = STL_FALSE;

    sSymbol->mForUpdate = STL_FALSE;

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

    sSymbol->mColumnCnt = 0;
    sSymbol->mRowStatusArray = NULL;
    sSymbol->mColumnAddrArray = NULL;
    sSymbol->mColumnIndArray = NULL;
    sSymbol->mColumnAddrValueArray = NULL;
    sSymbol->mColumnIndValueArray = NULL;
    sSymbol->mColumnLocalIndArray = NULL;

    /**
     * Output 설정
     */

    *aCursorSymbol = sSymbol;

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_ALLOCATE_MEM)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_ALLOCATE_MEM,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    }

    STL_CATCH( RAMP_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_NOT_IMPLEMENTED_FEATURE,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext),
                      "zlplMakeCursorSymbol()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Cursor Symbol 의 statement handle 에 Cursor Property 를 설정한다.
 * @param[in]  aSqlContext     SQL Context
 * @param[in]  aSqlArgs        SQL Arguments
 * @param[in]  aStmtHandle     Statement Handle
 * @remarks
 */
stlStatus zlpcSetCursorProp( zlplSqlContext   * aSqlContext,
                             zlplSqlArgs      * aSqlArgs,
                             SQLHSTMT           aStmtHandle )
{
    SQLRETURN        sReturn;

    sqlcurprops_t  * sCursorProp = NULL;

    zlplCursorStandardType  sStandardType;
    zlplCursorSensitivity   sSensitivity;
    zlplCursorScrollability sScrollability;
    zlplCursorHoldability   sHoldability;
    zlplCursorReturnability sReturnability;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSqlArgs != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_PARAM_VALIDATE( aStmtHandle != SQL_NULL_HANDLE, ZLPL_CTXT_ERROR_STACK(aSqlContext) );

    /**
     * 기본 정보 획득
     */

    sCursorProp = aSqlArgs->sqlcursor->sqlcurprops;

    /******************************************************************
     * Cursor Type 설정
     ******************************************************************/

    sReturn = SQL_SUCCESS;

    sStandardType = sCursorProp->cur_type;
    switch ( sStandardType )
    {
        case ZLPL_CURSOR_STANDARD_ISO:
            {
                sReturn = SQLSetStmtAttr( aStmtHandle,
                                          SQL_ATTR_CURSOR_TYPE,
                                          (SQLPOINTER) SQL_CURSOR_ISO,
                                          0 );

                break;
            }
        case ZLPL_CURSOR_STANDARD_ODBC_STATIC:
            {
                sReturn = SQLSetStmtAttr( aStmtHandle,
                                          SQL_ATTR_CURSOR_TYPE,
                                          (SQLPOINTER) SQL_CURSOR_STATIC,
                                          0 );
                break;
            }
        case ZLPL_CURSOR_STANDARD_ODBC_KEYSET:
            {
                sReturn = SQLSetStmtAttr( aStmtHandle,
                                          SQL_ATTR_CURSOR_TYPE,
                                          (SQLPOINTER) SQL_CURSOR_KEYSET_DRIVEN,
                                          0 );

                break;
            }
        case ZLPL_CURSOR_STANDARD_ODBC_DYNAMIC:
            {
                /**
                 * not implemented
                 */

                STL_THROW( RAMP_NOT_IMPLEMENTED );

                sReturn = SQLSetStmtAttr( aStmtHandle,
                                          SQL_ATTR_CURSOR_TYPE,
                                          (SQLPOINTER) SQL_CURSOR_DYNAMIC,
                                          0 );

                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             aStmtHandle,
                             (zlplSqlca *) aSqlArgs->sql_ca,
                             (stlChar *) aSqlArgs->sql_state )
             == STL_SUCCESS );

    /******************************************************************
     * Cursor Sensitivity 설정
     ******************************************************************/

    sReturn = SQL_SUCCESS;

    if ( sStandardType == ZLPL_CURSOR_STANDARD_ISO )
    {
        sSensitivity = sCursorProp->sensitivity;

        switch ( sSensitivity )
        {
            case ZLPL_CURSOR_SENSITIVITY_NA:
                {
                    /**
                     * nothing to do
                     * - 명시되지 않은 경우
                     */
                    break;
                }
            case ZLPL_CURSOR_SENSITIVITY_INSENSITIVE:
                {
                    sReturn = SQLSetStmtAttr( aStmtHandle,
                                              SQL_ATTR_CURSOR_SENSITIVITY,
                                              (SQLPOINTER) SQL_INSENSITIVE,
                                              0 );

                    break;
                }
            case ZLPL_CURSOR_SENSITIVITY_SENSITIVE:
                {
                    sReturn = SQLSetStmtAttr( aStmtHandle,
                                              SQL_ATTR_CURSOR_SENSITIVITY,
                                              (SQLPOINTER) SQL_SENSITIVE,
                                              0 );
                    break;
                }
            case ZLPL_CURSOR_SENSITIVITY_ASENSITIVE:
                {
                    sReturn = SQLSetStmtAttr( aStmtHandle,
                                              SQL_ATTR_CURSOR_SENSITIVITY,
                                              (SQLPOINTER) SQL_UNSPECIFIED,
                                              0 );
                    break;
                }
            default:
                {
                    STL_ASSERT(0);
                    break;
                }
        }
    }
    else
    {
        /**
         * ISO 이외 type 의 cursor 는 SQL 구문으로 sensitivity 를 설정할 수 없음
         */
    }

    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             aStmtHandle,
                             (zlplSqlca *) aSqlArgs->sql_ca,
                             (stlChar *) aSqlArgs->sql_state )
             == STL_SUCCESS );

    /******************************************************************
     * Cursor Scrollability 설정
     ******************************************************************/

    sReturn = SQL_SUCCESS;

    if ( sStandardType == ZLPL_CURSOR_STANDARD_ISO )
    {
        sScrollability = sCursorProp->scrollability;

        switch ( sScrollability )
        {
            case ZLPL_CURSOR_SCROLLABILITY_NA:
                {
                    /**
                     * nothing to do
                     * - 구문으로 명시하지 않음
                     */
                    break;
                }

            case ZLPL_CURSOR_SCROLLABILITY_NO_SCROLL:
                {
                    sReturn = SQLSetStmtAttr( aStmtHandle,
                                              SQL_ATTR_CURSOR_SCROLLABLE,
                                              (SQLPOINTER) SQL_NONSCROLLABLE,
                                              0 );
                    break;
                }
            case ZLPL_CURSOR_SCROLLABILITY_SCROLL:
                {
                    sReturn = SQLSetStmtAttr( aStmtHandle,
                                              SQL_ATTR_CURSOR_SCROLLABLE,
                                              (SQLPOINTER) SQL_SCROLLABLE,
                                              0 );
                    break;
                }
            default:
                {
                    STL_ASSERT(0);
                    break;
                }
        }
    }
    else
    {
        /**
         * ISO 이외 type 의 cursor 는 SQL 구문으로 scrollability 를 설정할 수 없음
         */
    }

    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             aStmtHandle,
                             (zlplSqlca *) aSqlArgs->sql_ca,
                             (stlChar *) aSqlArgs->sql_state )
             == STL_SUCCESS );

    /******************************************************************
     * Cursor Holdability 설정
     ******************************************************************/

    /**
     * ISO 이외의 type 도 설정 가능함.
     */

    sReturn = SQL_SUCCESS;

    sHoldability = sCursorProp->holdability;

    switch ( sHoldability )
    {
        case ZLPL_CURSOR_HOLDABILITY_NA:
            {
                /**
                 * nothing to do
                 * - 구문으로 명시하지 않음
                 */
                break;
            }
        case ZLPL_CURSOR_HOLDABILITY_WITH_HOLD:
            {
                sReturn = SQLSetStmtAttr( aStmtHandle,
                                          SQL_ATTR_CURSOR_HOLDABLE,
                                          (SQLPOINTER) SQL_HOLDABLE,
                                          0 );
                break;
            }
        case ZLPL_CURSOR_HOLDABILITY_WITHOUT_HOLD:
            {
                sReturn = SQLSetStmtAttr( aStmtHandle,
                                          SQL_ATTR_CURSOR_HOLDABLE,
                                          (SQLPOINTER) SQL_NONHOLDABLE,
                                          0 );
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             aStmtHandle,
                             (zlplSqlca *) aSqlArgs->sql_ca,
                             (stlChar *) aSqlArgs->sql_state )
             == STL_SUCCESS );

    /******************************************************************
     * Cursor Returnability 설정
     ******************************************************************/

    /**
     * ISO 이외의 type 도 설정 가능함.
     */

    sReturn = SQL_SUCCESS;

    sReturnability = sCursorProp->returnability;

    switch ( sReturnability )
    {
        case ZLPL_CURSOR_RETURNABILITY_NA:
            {
                /**
                 * nothing to do
                 * - 구문으로 명시하지 않음
                 */
                break;
            }
        case ZLPL_CURSOR_RETURNABILITY_WITH_RETURN:
            {
                /**
                 * Server Engine 이 구현되어야 함.
                 */

                STL_THROW( RAMP_NOT_IMPLEMENTED );
                break;
            }
        case ZLPL_CURSOR_RETURNABILITY_WITHOUT_RETURN:
            {
                /**
                 * Server Engine 이 구현되어야 함.
                 */

                STL_THROW( RAMP_NOT_IMPLEMENTED );
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             aStmtHandle,
                             (zlplSqlca *) aSqlArgs->sql_ca,
                             (stlChar *) aSqlArgs->sql_state )
             == STL_SUCCESS );

    /******************************************************************
     * Cursor Updatability 설정
     ******************************************************************/

    /**
     * SQL 구문으로 제어되므로 별도로 설정하지 않음
     */

    return STL_SUCCESS;

    STL_CATCH( RAMP_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_NOT_IMPLEMENTED_FEATURE,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext),
                      "zlplSetCursorProp()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Cursor 의 Query String 을 획득한다.
 * @param[in] aSqlContext    SQL Context
 * @param[in] aSqlArgs       SQL Arguments
 * @param[in] aCursorSymbol  Cursor Symbol
 * @param[out] aStringSQL     Query String
 * @param[out] aStringLen     Query Length
 * @remarks
 */
stlStatus zlpcGetCursorQuery( zlplSqlContext    * aSqlContext,
                              zlplSqlArgs       * aSqlArgs,
                              zlplCursorSymbol  * aCursorSymbol,
                              stlChar          ** aStringSQL,
                              stlInt32          * aStringLen )
{
    zlplDynStmtSymbol  * sStmtSymbol = NULL;

    stlChar  * sQuery = NULL;
    stlInt32   sQueryLen = 0;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aCursorSymbol != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_PARAM_VALIDATE( aStringSQL != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_PARAM_VALIDATE( aStringLen != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );

    /**
     * Cursor Origin 에 따라 획득 방법이 다르다.
     */

    switch ( aCursorSymbol->mCursorOriginType )
    {
        case ZLPL_CURSOR_ORIGIN_STANDING_CURSOR:
            {
                sQuery    = aSqlArgs->sqlstmt;
                sQueryLen = SQL_NTS;
                break;
            }
        case ZLPL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR:
            {
                /**
                 * Dynamic Statement 를 검색
                 */

                STL_TRY( zlpyLookupDynStmtSymbol( aSqlContext,
                                                  aCursorSymbol->mDynStmtName,
                                                  & sStmtSymbol )
                         == STL_SUCCESS);

                /**
                 * PREPARE 되지 않았거나, PREPARE 가 실패한 경우에 대한 검사
                 */

                STL_TRY_THROW( sStmtSymbol != NULL, RAMP_ERR_STMT_NOT_PREPARED );
                STL_TRY_THROW( sStmtSymbol->mPrevSQL != NULL, RAMP_ERR_STMT_NOT_PREPARED );

                sQuery    = sStmtSymbol->mPrevSQL;
                sQueryLen = SQL_NTS;
                break;
            }
        case ZLPL_CURSOR_ORIGIN_EXTENDED_STATEMENT_CURSOR:
            {
                STL_THROW( RAMP_NOT_IMPLEMENTED );
                break;
            }
        case ZLPL_CURSOR_ORIGIN_RECEIVED_CURSOR:
            {
                STL_THROW( RAMP_NOT_IMPLEMENTED );
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /**
     * Output 설정
     */

    *aStringSQL = sQuery;
    *aStringLen = sQueryLen;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_STMT_NOT_PREPARED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_STATEMEMT_NAME_NOT_PREPARED,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext),
                      aCursorSymbol->mDynStmtName );
    }

    STL_CATCH( RAMP_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_NOT_IMPLEMENTED_FEATURE,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext),
                      "zlplGetCursorQuery()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Cursor Query 를 validation 한다.
 * @param[in] aSqlContext    SQL Context
 * @param[in] aSqlArgs       SQL Arguments
 * @param[in] aCursorSymbol  Cursor Symbol
 * @param[in] aStringSQL     Query String
 * @param[in] aStringLen     Query Length
 * @param[in] aNeedSetCursorProp     Need for Set Cursor Property
 * @remarks
 */
stlStatus zlpcPrepareCursorQuery( zlplSqlContext   * aSqlContext,
                                  zlplSqlArgs      * aSqlArgs,
                                  zlplCursorSymbol * aCursorSymbol,
                                  stlChar          * aStringSQL,
                                  stlInt32           aStringLen,
                                  stlBool            aNeedSetCursorProp )
{
    SQLHSTMT       sStmtHandle;
    zlplConnInfo  *sConnInfo   = ZLPL_CTXT_CONNECTION(aSqlContext);
    SQLRETURN      sReturn;

    SQLULEN        sForUpdate = 0;


    SQLSMALLINT    sColumnCnt = 0;

    stlBool        sRollbackNewSQL = STL_FALSE;
    stlBool        sOldPreparedState;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aCursorSymbol != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    STL_PARAM_VALIDATE( aStringSQL != NULL, ZLPL_CTXT_ERROR_STACK(aSqlContext) );

    /****************************************************************
     * Prepare 수행
     ****************************************************************/

    sOldPreparedState = aCursorSymbol->mIsPrepared;
    aCursorSymbol->mIsPrepared = STL_FALSE;

    STL_TRY( zlpuAllocStmt( & sStmtHandle, sConnInfo->mDbc, aSqlArgs ) == STL_SUCCESS );

    /**
     * Cursor Property 설정
     */

    if ( aNeedSetCursorProp == STL_TRUE )
    {
        STL_TRY( zlpcSetCursorProp( aSqlContext, aSqlArgs, sStmtHandle ) == STL_SUCCESS );
    }
    else
    {
        /* nothing to do */
    }

    /**
     * Cursor Query 를 Prepare 함.
     */

    sReturn = SQLPrepare( sStmtHandle,
                          (SQLCHAR *) aStringSQL,
                          aStringLen );

    STL_TRY_THROW( zlpeCheckError( sReturn,
                                   SQL_HANDLE_STMT,
                                   sStmtHandle,
                                   (zlplSqlca *)aSqlArgs->sql_ca,
                                   (stlChar *)aSqlArgs->sql_state )
                   == STL_SUCCESS, RAMP_ERR_PREPARE );

    /****************************************************************
     * 유효성 검사
     ****************************************************************/

    /**
     * Query 인지 검사
     */

    sReturn = SQLNumResultCols( sStmtHandle, & sColumnCnt );
    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             sStmtHandle,
                             (zlplSqlca *) aSqlArgs->sql_ca,
                             (stlChar *) aSqlArgs->sql_state )
             == STL_SUCCESS);

    /**
     * Standing Cursor 인 경우는 parsing 단계에서 걸러지나,
     * Dynamic Cursor 인 경우, Query 인지 검사해야 함
     */

    STL_TRY_THROW( sColumnCnt > 0, RAMP_ERR_NOT_QUERY );

    /**
     * 새로 Prepare 한 문장으로, Cursor symbol의 statement를 교체
     */
    zlpuFreeStmt( aCursorSymbol->mStmtHandle );
    aCursorSymbol->mStmtHandle = sStmtHandle;

    /**
     * FOR UPDATE 여부 정보 획득
     */

    sReturn = SQLGetStmtAttr( aCursorSymbol->mStmtHandle,
                              SQL_CONCURRENCY,
                              (SQLPOINTER) & sForUpdate,
                              0,
                              NULL );

    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             aCursorSymbol->mStmtHandle,
                             (zlplSqlca *) aSqlArgs->sql_ca,
                             (stlChar *) aSqlArgs->sql_state)
             == STL_SUCCESS);

    if ( sForUpdate == SQL_CONCUR_LOCK )
    {
        aCursorSymbol->mForUpdate = STL_TRUE;
    }
    else
    {
        aCursorSymbol->mForUpdate = STL_FALSE;
    }

    /****************************************************************
     * 재수행 방지를 위한 Parameter 공간 확보
     ****************************************************************/

    /**
     * 기존 공간 제거
     */

    STL_TRY( zlpcFreeCursorSymbolParam( aSqlContext,
                                        aCursorSymbol )
             == STL_SUCCESS );

    STL_TRY( zlpcFreeCursorSymbolColumn( aSqlContext,
                                         aCursorSymbol )
             == STL_SUCCESS );


    aCursorSymbol->mIsPrepared = STL_TRUE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_QUERY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_STATEMENT_IS_NOT_A_QUERY,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext),
                      aSqlArgs->sqlcursor->sqlcname );
        sRollbackNewSQL = STL_TRUE;
    }

    STL_CATCH( RAMP_ERR_PREPARE )
    {
        zlpuFreeStmt( sStmtHandle );
        sRollbackNewSQL = STL_TRUE;
    }

    STL_FINISH;

    /**
     * cursor symbol의 stmt handle 을 종료해서는 안된다.
     * 현재 cursor 가 open 되어 있는 등의 상태에서는 handle 의 상태가 유지되어야 한다.
     * 해당 stmt handle 은 cursor symbol 과 함께 재사용된다.
     */

    /**
     * 새로 Prepare하는 SQL이 실패했을 경우에는, 기존 cursor의 상태를 유지한다.
     */
    if( sRollbackNewSQL == STL_TRUE )
    {
        aCursorSymbol->mIsPrepared = sOldPreparedState;
    }
    else
    {
        aCursorSymbol->mIsPrepared = STL_FALSE;
    }

    return STL_FAILURE;
}

/**
 * @brief Cursor Symbol에서 Parameter 메모리를 할당한다.
 * @param[in]  aSqlContext     SQL Context
 * @param[in]  aCursorSymbol   Cursor Symbol
 * @param[in]  aArraySize      Parameter의 array size
 * @param[in]  aParamCnt       Parameter 개수
 * @param[in]  aHostVar        Host variable array
 * @remarks
 */
stlStatus zlpcAllocCursorSymbolParam( zlplSqlContext    * aSqlContext,
                                      zlplCursorSymbol  * aCursorSymbol,
                                      stlInt32            aArraySize,
                                      stlInt32            aParamCnt,
                                      zlplSqlHostVar    * aHostVar )
{
    stlInt32          sRowSize = 0;
    stlInt32          sAllocSize = 0;
    stlInt32          sAddrAreaSize = 0;
    stlInt32          sRowBufOffset;
    stlChar          *sAllocBuffer = NULL;
    stlInt32          i;

    aCursorSymbol->mArraySize = aArraySize;
    if( aArraySize > 1 )
    {
        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     STL_SIZEOF(SQLUSMALLINT) * aArraySize,
                                     (void **) & aCursorSymbol->mParamStatusArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
    }
    else
    {
        aCursorSymbol->mParamStatusArray = NULL;
    }

    aCursorSymbol->mParamCnt = aParamCnt;

    if ( aParamCnt > 0 )
    {
        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     STL_SIZEOF(stlChar *) * aParamCnt,
                                     (void **) & aCursorSymbol->mParamAddrArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( aCursorSymbol->mParamAddrArray, 0x00, STL_SIZEOF( stlChar *) * aParamCnt );

        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     STL_SIZEOF(stlChar *) * aParamCnt,
                                     (void **) & aCursorSymbol->mParamIndArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( aCursorSymbol->mParamIndArray, 0x00, STL_SIZEOF( stlChar *) * aParamCnt );

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
            sRowSize += ZLPL_ALIGN( zlpbGetDataBufferLength( &aHostVar[i] ) );
        }

        sAddrAreaSize = ZLPL_ALIGN( aParamCnt * STL_SIZEOF(stlChar *) );
        sAllocSize = sAddrAreaSize + sRowSize * aArraySize;
        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     sAllocSize,
                                     (void **) & sAllocBuffer,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( sAllocBuffer, 0x00, sAllocSize );
        aCursorSymbol->mParamAddrValueArray = (stlChar **)sAllocBuffer;

        sRowBufOffset = sAddrAreaSize;
        for( i = 0; i < aParamCnt; i ++ )
        {
            aCursorSymbol->mParamAddrValueArray[i] = sAllocBuffer + sRowBufOffset;
            sRowBufOffset += ZLPL_ALIGN( zlpbGetDataBufferLength( &aHostVar[i] ) ) * aArraySize;
        }

        /*
         * Parameter Indicator Buffer allocation
         * : 할당 방식은 위의 Value Buffer 할당 방식과 같다.
         */
        sAllocSize = 0;
        sRowSize = 0;
        for( i = 0; i < aParamCnt; i ++ )
        {
            if( aHostVar[i].indicator != NULL )
            {
                sRowSize += ZLPL_ALIGN( zlpbGetDataSize( aHostVar[i].ind_type ) );
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
        aCursorSymbol->mParamIndValueArray = (stlChar **)sAllocBuffer;

        sRowBufOffset = sAddrAreaSize;
        for( i = 0; i < aParamCnt; i ++ )
        {
            if( aHostVar[i].indicator == NULL )
            {
                aCursorSymbol->mParamIndValueArray[i] = NULL;
            }
            else
            {
                aCursorSymbol->mParamIndValueArray[i] = sAllocBuffer + sRowBufOffset;
                sRowBufOffset += ZLPL_ALIGN( zlpbGetDataSize( aHostVar[i].ind_type ) ) * aArraySize;
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
        aCursorSymbol->mParamLocalIndArray = (SQLLEN **)sAllocBuffer;

        sRowBufOffset = sAddrAreaSize;
        for( i = 0; i < aParamCnt; i ++ )
        {
            aCursorSymbol->mParamLocalIndArray[i] = (SQLLEN *)(sAllocBuffer + sRowBufOffset);
            sRowBufOffset += STL_SIZEOF(SQLLEN) * aArraySize;
        }
    }
    else
    {
        aCursorSymbol->mParamAddrArray = NULL;
        aCursorSymbol->mParamIndArray = NULL;
        aCursorSymbol->mParamAddrValueArray = NULL;
        aCursorSymbol->mParamIndValueArray = NULL;
        aCursorSymbol->mParamLocalIndArray = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Cursor Symbol에서 Parameter 메모리를 반환한다.
 * @param[in]  aSqlContext     SQL Context
 * @param[in]  aCursorSymbol   Cursor Symbol
 * @remarks
 */
stlStatus zlpcFreeCursorSymbolParam( zlplSqlContext    * aSqlContext,
                                     zlplCursorSymbol  * aCursorSymbol )
{
    aCursorSymbol->mArraySize = 0;
    aCursorSymbol->mParamCnt = 0;

    if( aCursorSymbol->mParamStatusArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                    aCursorSymbol->mParamStatusArray,
                                    ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aCursorSymbol->mParamStatusArray = NULL;
    }

    if( aCursorSymbol->mParamAddrArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                    aCursorSymbol->mParamAddrArray,
                                    ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aCursorSymbol->mParamAddrArray = NULL;
    }

    if( aCursorSymbol->mParamIndArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aCursorSymbol->mParamIndArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aCursorSymbol->mParamIndArray = NULL;
    }

    if( aCursorSymbol->mParamAddrValueArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aCursorSymbol->mParamAddrValueArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aCursorSymbol->mParamAddrValueArray = NULL;
    }

    if( aCursorSymbol->mParamIndValueArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aCursorSymbol->mParamIndValueArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aCursorSymbol->mParamIndValueArray = NULL;
    }

    if( aCursorSymbol->mParamLocalIndArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aCursorSymbol->mParamLocalIndArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aCursorSymbol->mParamLocalIndArray = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} zlpcOpenCursor */
