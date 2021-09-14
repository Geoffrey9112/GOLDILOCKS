/*******************************************************************************
 * zlpxExecute.c
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
#include <zlpnConnect.h>
#include <zlpuMisc.h>
#include <zlpxExecute.h>
#include <zlpxExecuteSelectIntoArray.h>
#include <zlpyQueryStmt.h>

/**
 * @file zlpxExecute.c
 * @brief Gliese Embedded SQL in C precompiler SQL execution library functions
 */

/**
 * @addtogroup zlpxExecute
 * @{
 */


stlStatus zlpxEndTranContext(zlplSqlContext   *aSqlContext,
                             zlplSqlArgs      *aSqlArgs,
                             stlBool           aIsCommit,
                             stlBool           aIsRelease)
{
    SQLRETURN     sReturn;
    SQLSMALLINT   sCompleteType;
    zlplConnInfo *sConnInfo = &aSqlContext->mConnInfo;

    ZLPL_INIT_SQLCA(aSqlContext, aSqlArgs->sql_ca);

    STL_TRY( zlpeIsInit() == STL_SUCCESS );

    if(aIsCommit == STL_TRUE)
    {
        sCompleteType = SQL_COMMIT;
    }
    else
    {
        sCompleteType = SQL_ROLLBACK;
    }

    sReturn = SQLEndTran(SQL_HANDLE_DBC,
                         sConnInfo->mDbc,
                         sCompleteType);

    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_DBC,
                           sConnInfo->mDbc,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);

    if(aIsRelease == STL_TRUE)
    {
        STL_TRY(zlpnDisconnectContext(aSqlContext,
                                      aSqlArgs,
                                      NULL)
                == STL_SUCCESS);
    }

    return STL_SUCCESS;

    STL_FINISH;

    (void)SQLEndTran(SQL_HANDLE_DBC,
                     sConnInfo->mDbc,
                     SQL_ROLLBACK);

    return STL_FAILURE;
}

stlStatus zlpxSetAutocommitContext(zlplSqlContext   *aSqlContext,
                                   zlplSqlArgs      *aSqlArgs,
                                   stlBool           aIsOn)
{
    SQLRETURN     sReturn;
    SQLPOINTER    sAutocommitMode;
    zlplConnInfo *sConnInfo = &aSqlContext->mConnInfo;

    ZLPL_INIT_SQLCA(aSqlContext, aSqlArgs->sql_ca);

    STL_TRY( zlpeIsInit() == STL_SUCCESS );

    if(aIsOn == STL_TRUE)
    {
        sAutocommitMode = (SQLPOINTER)SQL_AUTOCOMMIT_ON;
    }
    else
    {
        sAutocommitMode = (SQLPOINTER)SQL_AUTOCOMMIT_OFF;
    }

    sReturn = SQLSetConnectAttr(sConnInfo->mDbc,
                                SQL_ATTR_AUTOCOMMIT,
                                sAutocommitMode,
                                SQL_NTS);

    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_DBC,
                           sConnInfo->mDbc,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlpxExecuteContext(zlplSqlContext   *aSqlContext,
                             zlplSqlArgs      *aSqlArgs)
{
    SQLRETURN             sReturn;
    SQLHANDLE             sStmtHandle = SQL_NULL_HANDLE;
    zlplConnInfo         *sConnInfo = ZLPL_CTXT_CONNECTION(aSqlContext);
    SQLLEN                sAffectedRowCount;
    zlplSqlStmtKey        sQueryKey;
    zlplSqlStmtSymbol    *sSqlSymbol = NULL;
    stlBool               sIsFound = STL_FALSE;
    stlInt32              i;
    zlplSqlHostVar       *sHostVar;
    stlInt32              sIteration = 0;
    stlInt32              sSqlLen = 0;

    ZLPL_INIT_SQLCA(aSqlContext, aSqlArgs->sql_ca);

    STL_TRY( zlpeIsInit() == STL_SUCCESS );

    /*
     * Find Min array size
     */
    (void)zlpuGetMinArraySize( aSqlArgs->iters,
                               aSqlArgs->hvc,
                               aSqlArgs->sqlhv,
                               &sIteration );

    STL_TRY( zlpbAdjustDataType( aSqlContext,
                                 aSqlArgs )
             == STL_SUCCESS );

    if( zlpxIsSelectIntoArray( aSqlContext,
                               aSqlArgs ) == STL_TRUE )
    {
        STL_TRY( zlpxExecuteSelectIntoArray( aSqlContext, aSqlArgs )
                 == STL_SUCCESS );
    }
    else if(aSqlArgs->sqltype <= ZLPL_STMT_DELETE)
    {
        /**
         * Static DML 인 경우
         */

        sQueryKey.mLine = aSqlArgs->sqlln;
        sQueryKey.mFileName = aSqlArgs->sqlfn;
        STL_TRY(zlpyLookupSqlStmtSymbol(aSqlContext,
                                          &sQueryKey,
                                          &sSqlSymbol,
                                          &sIsFound)
                == STL_SUCCESS);

        if(sIsFound == STL_FALSE)
        {
            /**
             * 최초 Query 실행
             */

            /**
             *  1. Symbol의 할당
             */
            STL_TRY(zlpxCreateSqlSymbol(aSqlContext,
                                        aSqlArgs,
                                        STL_TRUE, /* aDoPrepare */
                                        &sSqlSymbol)
                    == STL_SUCCESS);

            /**
             *  2. Symbol에 SQL 저장
             */
            sSqlLen = stlStrlen(aSqlArgs->sqlstmt);

            STL_TRY( stlAllocRegionMem( ZLPL_CTXT_ALLOCATOR(aSqlContext),
                                        sSqlLen + 1,
                                        (void **)&sSqlSymbol->mQuery,
                                        ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                     == STL_SUCCESS );
            stlStrncpy(sSqlSymbol->mQuery, aSqlArgs->sqlstmt, sSqlLen + 1);

            /**
             *  3. Prepare
             */
            STL_TRY(zlpxPrepareSqlStmt(aSqlContext,
                                       aSqlArgs,
                                       sSqlSymbol)
                    == STL_SUCCESS);

            /**
             *  4. Symbol에 Host variable 메모리, 임시 버퍼 생성
             */
            STL_TRY(zlpxAllocHostVarBuffer(aSqlContext,
                                           aSqlArgs,
                                           sIteration,
                                           sSqlSymbol)
                    == STL_SUCCESS);

            /**
             *  5. Symbol에 host variable 구성
             */
            sHostVar = (zlplSqlHostVar *)aSqlArgs->sqlhv;
            for(i = 0; i < aSqlArgs->hvc; i ++)
            {
                ZLPL_COPY_HOST_VAR( &sSqlSymbol->mHostVar[i], &sHostVar[i] );
            }

            /**
             *  6. Bind parameter
             */
            STL_TRY(zlpxBindSqlStmt(aSqlContext,
                                    aSqlArgs,
                                    sIteration,
                                    sSqlSymbol)
                    == STL_SUCCESS);
        }
        else
        {
            if ( sSqlSymbol->mIsPrepared == STL_TRUE )
            {
                /* 다시 bind 해야 하는 지 check */
                if(zlpxIsEqualHostVar(aSqlArgs->sqlhv,
                                      sSqlSymbol->mHostVar,
                                      aSqlArgs->hvc) != STL_TRUE)
                {
                    /**
                     * Prepare및 Host variable Buffer가 준비된 상태이므로,
                     * Host variable 재구성과, Bind parameter만 실시한다.
                     * 위 스텝의 5~6 번만 수행
                     */

                    /**
                     *  5. Symbol에 host variable 구성
                     */
                    sHostVar = (zlplSqlHostVar *)aSqlArgs->sqlhv;
                    for(i = 0; i < aSqlArgs->hvc; i ++)
                    {
                        ZLPL_COPY_HOST_VAR( &sSqlSymbol->mHostVar[i], &sHostVar[i] );
                    }

                    /**
                     *  6. Bind parameter
                     */
                    STL_TRY(zlpxBindSqlStmt(aSqlContext,
                                            aSqlArgs,
                                            sIteration,
                                            sSqlSymbol)
                            == STL_SUCCESS);
                }
            }
            else
            {
                /**
                 * Prepare부터 수행해야 하므로,
                 * 위 스텝의 3~6을 수행
                 */

                /**
                 *  3. Prepare
                 */
                STL_TRY(zlpxPrepareSqlStmt(aSqlContext,
                                           aSqlArgs,
                                           sSqlSymbol)
                        == STL_SUCCESS);

                /**
                 *  4. Symbol에 Host variable 메모리, 임시 버퍼 생성
                 */
                STL_TRY(zlpxAllocHostVarBuffer(aSqlContext,
                                               aSqlArgs,
                                               sIteration,
                                               sSqlSymbol)
                        == STL_SUCCESS);

                /**
                 *  5. Symbol에 host variable 구성
                 */
                sHostVar = (zlplSqlHostVar *)aSqlArgs->sqlhv;
                for(i = 0; i < aSqlArgs->hvc; i ++)
                {
                    ZLPL_COPY_HOST_VAR( &sSqlSymbol->mHostVar[i], &sHostVar[i] );
                }

                /**
                 *  6. Bind parameter
                 */
                STL_TRY(zlpxBindSqlStmt(aSqlContext,
                                        aSqlArgs,
                                        sIteration,
                                        sSqlSymbol)
                        == STL_SUCCESS);
            }
        }

        sStmtHandle = sSqlSymbol->mStmtHandle;

        STL_TRY(zlpbSetBeforeInfo(aSqlContext,
                                  aSqlArgs,
                                  sStmtHandle,
                                  aSqlArgs->hvc,
                                  aSqlArgs->sqlhv,
                                  sIteration,
                                  &sSqlSymbol->mParamProcessed,
                                  sSqlSymbol->mParamStatusArray,
                                  sSqlSymbol->mValueBuf,
                                  sSqlSymbol->mParamLocalIndArray,
                                  NULL,       /* aDynParamIOType */
                                  STL_TRUE )  /* Bind Parameter */
                == STL_SUCCESS);

        sReturn = SQLExecute(sStmtHandle);
        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_STMT,
                               sStmtHandle,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);

        sReturn = SQLRowCount(sStmtHandle,
                              &sAffectedRowCount);

        /*
         * SQLCA 내에는 ExecDirect 수행 결과를 담아서 주도록 하기 위해
         * 여기서는 zlpeCheckError()를 부르지 않는다.
         */

        /* Affected Row count set */
        ((zlplSqlca *)(aSqlArgs->sql_ca))->sqlerrd[2] = (int)sAffectedRowCount;

        if ( sAffectedRowCount > 0 )
        {
            STL_TRY(zlpbSetAfterInfo( aSqlContext,
                                      aSqlArgs,
                                      sStmtHandle,
                                      aSqlArgs->unsafenull,
                                      aSqlArgs->hvc,
                                      aSqlArgs->sqlhv,
                                      sIteration,
                                      sSqlSymbol->mValueBuf,
                                      sSqlSymbol->mParamLocalIndArray,
                                      NULL )   /* aDynParamIOType */
                    == STL_SUCCESS);
        }
        else
        {
            /* nothing to do */
        }
    }
    else
    {
        /**
         * Static DDL, DCL 인 경우
         */

        STL_TRY(zlpuAllocStmt(&sStmtHandle,
                              sConnInfo->mDbc,
                              aSqlArgs)
                == STL_SUCCESS);

        /* Host Variable 이 존재할 수 없음 */
        STL_DASSERT( aSqlArgs->hvc == 0 );

        sReturn = SQLExecDirect(sStmtHandle,
                                (SQLCHAR *)aSqlArgs->sqlstmt,
                                SQL_NTS);

        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_STMT,
                               sStmtHandle,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);

        STL_TRY(zlpuFreeStmt(sStmtHandle) == STL_SUCCESS);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Sql Statement Symbol 을 생성한다.
 * @param[in] aSqlContext     SQL Context
 * @param[in] aSqlArgs        SQL Arguments
 * @param[in] aDoPrepare      SQLPrepare() 수행 여부: TRUE(Prepare) or FALSE(Alloc only)
 * @param[out] aSqlSymbol   Sql Statement Symbol
 * @remarks
 */
stlStatus zlpxCreateSqlSymbol(zlplSqlContext       * aSqlContext,
                              zlplSqlArgs          * aSqlArgs,
                              stlBool                aDoPrepare,
                              zlplSqlStmtSymbol   ** aSqlSymbol)
{
    SQLHANDLE            sStmtHandle = SQL_NULL_HANDLE;
    zlplSqlStmtSymbol   *sSqlSymbol = NULL;
    zlplConnInfo        *sConnInfo = ZLPL_CTXT_CONNECTION(aSqlContext);

    /**
     * Query Symbol 을 위한 공간 할당
     */

    STL_TRY( stlAllocRegionMem( ZLPL_CTXT_ALLOCATOR(aSqlContext),
                                STL_SIZEOF(zlplSqlStmtSymbol),
                                (void **)&sSqlSymbol,
                                ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );

    sSqlSymbol->mIsPrepared = STL_FALSE;

    sSqlSymbol->mQuery = NULL;
    sSqlSymbol->mHostVar = NULL;
    sSqlSymbol->mParamStatusArray   = NULL;
    sSqlSymbol->mValueBuf           = NULL;
    sSqlSymbol->mParamLocalIndArray = NULL;

    /**
     * Query Symbol 정보 설정
     */

    STL_TRY( zlpuAllocStmt( & sStmtHandle, sConnInfo->mDbc, aSqlArgs ) == STL_SUCCESS);

    sSqlSymbol->mStmtHandle    = sStmtHandle;
    sSqlSymbol->mKey.mLine     = aSqlArgs->sqlln;
    sSqlSymbol->mKey.mFileName = aSqlArgs->sqlfn;

    /**
     * Query Symbol 등록
     */

    STL_TRY( zlpyAddSqlStmtSymbol(aSqlContext, sSqlSymbol ) == STL_SUCCESS );

    *aSqlSymbol = sSqlSymbol;

    return STL_SUCCESS;

    STL_FINISH;

    *aSqlSymbol = NULL;

    return STL_FAILURE;
}

/**
 * @brief Sql Statement Symbol 을 생성한다.
 * @param[in] aSqlContext     SQL Context
 * @param[in] aSqlArgs        SQL Arguments
 * @param[in] aSqlSymbol    Sql Statement Symbol
 * @remarks
 */
stlStatus zlpxPrepareSqlStmt(zlplSqlContext       * aSqlContext,
                             zlplSqlArgs          * aSqlArgs,
                             zlplSqlStmtSymbol    * aSqlSymbol)
{
    SQLRETURN            sReturn;

    /**
     * Prepare 를 수행하는 경우
     */

    sReturn = SQLPrepare( aSqlSymbol->mStmtHandle,
                          (SQLCHAR *) aSqlArgs->sqlstmt,
                          SQL_NTS );

    STL_TRY(zlpeCheckError( sReturn,
                            SQL_HANDLE_STMT,
                            aSqlSymbol->mStmtHandle,
                            (zlplSqlca *)aSqlArgs->sql_ca,
                            (stlChar *)aSqlArgs->sql_state )
            == STL_SUCCESS);

    aSqlSymbol->mIsPrepared = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Sql Statement Symbol 을 생성한다.
 * @param[in] aSqlContext     SQL Context
 * @param[in] aSqlArgs        SQL Arguments
 * @param[in] aArraySize      Array Size
 * @param[in] aSqlSymbol      Sql Statement Symbol
 * @remarks
 */
stlStatus zlpxAllocHostVarBuffer(zlplSqlContext       * aSqlContext,
                                 zlplSqlArgs          * aSqlArgs,
                                 stlInt32               aArraySize,
                                 zlplSqlStmtSymbol    * aSqlSymbol)
{
    if ( aSqlArgs->hvc > 0 )
    {
        STL_TRY( zlpxAllocHostVarTempBuffer( aSqlContext,
                                             aSqlSymbol,
                                             aArraySize,
                                             aSqlArgs->hvc,
                                             aSqlArgs->sqlhv )
                 == STL_SUCCESS );

        STL_TRY( stlAllocRegionMem( ZLPL_CTXT_ALLOCATOR(aSqlContext),
                                    STL_SIZEOF(zlplSqlHostVar) * aSqlArgs->hvc,
                                    (void **)&aSqlSymbol->mHostVar,
                                    ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
    }
    else
    {
        aSqlSymbol->mValueBuf           = NULL;
        aSqlSymbol->mParamLocalIndArray = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Sql Statement Symbol 을 생성한다.
 * @param[in] aSqlContext     SQL Context
 * @param[in] aSqlArgs        SQL Arguments
 * @param[in] aArraySize      Array Size
 * @param[in] aSqlSymbol      Sql Statement Symbol
 * @remarks
 */
stlStatus zlpxBindSqlStmt(zlplSqlContext       * aSqlContext,
                          zlplSqlArgs          * aSqlArgs,
                          stlInt32               aArraySize,
                          zlplSqlStmtSymbol    * aSqlSymbol)
{
    STL_TRY( zlpbBindParams( aSqlSymbol->mStmtHandle,
                             aSqlArgs,
                             aSqlArgs->hvc,
                             aSqlArgs->sqlhv,
                             aSqlSymbol->mValueBuf,
                             aSqlSymbol->mParamLocalIndArray,
                             NULL )  /* aDynParamIOType */
             == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlBool zlpxIsEqualHostVar(zlplSqlHostVar  *aHostVar1,
                           zlplSqlHostVar  *aHostVar2,
                           stlInt32         aHostVarCount)
{
    stlInt32   i;

    for(i = 0; i < aHostVarCount; i ++)
    {
        if ( ZLPL_EQUAL_HOST_VAR(&aHostVar1[i], &aHostVar2[i]) != STL_TRUE )
        {
            return STL_FALSE;
        }
    }

    return STL_TRUE;
}

/**
 * @brief Host Variable을 임시 저장할 내부 Buffer를 생성한다.
 * @param[in] aSqlContext     SQL Context
 * @param[in] aSqlSymbol      Sql Statement Symbol
 * @param[in] aArraySize      Array Size
 * @param[in] aHostVarCnt     Host variable count
 * @param[in] aHostVar        Host variable array
 * @remarks
 */
stlStatus zlpxAllocHostVarTempBuffer(zlplSqlContext       * aSqlContext,
                                     zlplSqlStmtSymbol    * aSqlSymbol,
                                     stlInt32               aArraySize,
                                     stlInt32               aHostVarCnt,
                                     zlplSqlHostVar       * aHostVar)
{
    stlChar             *sAllocBuffer;
    stlInt32             sRowSize = 0;
    stlInt32             sAllocSize = 0;
    stlInt32             sAddrAreaSize = 0;
    stlInt32             sRowBufOffset = 0;
    stlInt32             i;

    aSqlSymbol->mValueBuf = NULL;
    aSqlSymbol->mParamLocalIndArray = NULL;

    if( aArraySize > 1 )
    {
        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     STL_SIZEOF(SQLUSMALLINT) * aArraySize,
                                     (void **) & aSqlSymbol->mParamStatusArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
    }
    else
    {
        aSqlSymbol->mParamStatusArray = NULL;
    }

    /*
     * Value Buffer allocation
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
    for( i = 0; i < aHostVarCnt; i ++ )
    {
        sRowSize += ZLPL_ALIGN( zlpbGetDataBufferLength( &aHostVar[i] ) );
    }

    sAddrAreaSize = ZLPL_ALIGN( aHostVarCnt * STL_SIZEOF(stlChar *) );
    sAllocSize = sAddrAreaSize + sRowSize * aArraySize;
    STL_TRY( stlAllocDynamicMem( &aSqlContext->mDynMemory,
                                 sAllocSize,
                                 (void **)& sAllocBuffer,
                                 ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );
    stlMemset( sAllocBuffer, 0x00, sAllocSize );
    aSqlSymbol->mValueBuf = (stlChar **)sAllocBuffer;

    sRowBufOffset = sAddrAreaSize;
    for( i = 0; i < aHostVarCnt; i ++ )
    {
        aSqlSymbol->mValueBuf[i]= sAllocBuffer + sRowBufOffset;
        sRowBufOffset += ZLPL_ALIGN( zlpbGetDataBufferLength( &aHostVar[i] ) ) * aArraySize;
    }

    /*
     * Parameter Local Indicator Buffer allocation
     * : 할당 방식은 위의 Value Buffer 할당 방식과 같다.
     */
    sAllocSize = 0;
    sRowSize = STL_SIZEOF(SQLLEN) * aHostVarCnt;

    sAddrAreaSize = ZLPL_ALIGN( aHostVarCnt * STL_SIZEOF(SQLLEN *) );
    sAllocSize = sAddrAreaSize + sRowSize * aArraySize;
    STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                 sAllocSize,
                                 (void **) & sAllocBuffer,
                                 ZLPL_CTXT_ERROR_STACK(aSqlContext) )
             == STL_SUCCESS );
    stlMemset( sAllocBuffer, 0x00, sAllocSize );
    aSqlSymbol->mParamLocalIndArray = (SQLLEN **)sAllocBuffer;

    sRowBufOffset = sAddrAreaSize;
    for( i = 0; i < aHostVarCnt; i ++ )
    {
        aSqlSymbol->mParamLocalIndArray[i] = (SQLLEN *)(sAllocBuffer + sRowBufOffset);
        sRowBufOffset += STL_SIZEOF(SQLLEN) * aArraySize;
    }

    aSqlSymbol->mArraySize = aArraySize;

    return STL_SUCCESS;

    STL_FINISH;

    if ( aSqlSymbol->mParamStatusArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                    aSqlSymbol->mParamStatusArray,
                                    ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );

        aSqlSymbol->mParamStatusArray = NULL;
    }

    if( aSqlSymbol->mParamLocalIndArray != NULL )
    {
        (void)stlFreeDynamicMem( &aSqlContext->mDynMemory,
                                 aSqlSymbol->mParamLocalIndArray,
                                 ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    }

    if( aSqlSymbol->mValueBuf != NULL )
    {
        (void)stlFreeDynamicMem( &aSqlContext->mDynMemory,
                                 aSqlSymbol->mValueBuf,
                                 ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    }

    aSqlSymbol->mArraySize = 0;
    aSqlSymbol->mValueBuf = NULL;
    aSqlSymbol->mParamLocalIndArray = NULL;

    return STL_FAILURE;
}

/**
 * @brief Host Variable 임시 Buffer를 해제한다.
 * @param[in] aSqlContext     SQL Context
 * @param[in] aSqlSymbol      Sql Statement Symbol
 * @remarks
 */
stlStatus zlpxFreeHostVarTempBuffer(zlplSqlContext       * aSqlContext,
                                    zlplSqlStmtSymbol    * aSqlSymbol )
{
    if ( aSqlSymbol->mParamStatusArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                    aSqlSymbol->mParamStatusArray,
                                    ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );

        aSqlSymbol->mParamStatusArray = NULL;
    }

    if( aSqlSymbol->mValueBuf != NULL )
    {
        STL_TRY( stlFreeDynamicMem( &aSqlContext->mDynMemory,
                                    aSqlSymbol->mValueBuf,
                                    ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );

        aSqlSymbol->mValueBuf = NULL;
    }

    if ( aSqlSymbol->mParamLocalIndArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                    aSqlSymbol->mParamLocalIndArray,
                                    ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );

        aSqlSymbol->mParamLocalIndArray = NULL;
    }

    aSqlSymbol->mArraySize = 0;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
