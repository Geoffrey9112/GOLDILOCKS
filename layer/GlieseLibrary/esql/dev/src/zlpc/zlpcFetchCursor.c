/*******************************************************************************
 * zlpcFetchCursor.c
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
#include <zlpcFetchCursor.h>
#include <zlpuMisc.h>
#include <zlpxDynExecute.h>
#include <zlpyCursor.h>

/**
 * @file zlpcFetchCursor.c
 * @brief Gliese Embedded SQL in C precompiler SQL cursor library functions
 */

/**
 * @defgroup zlpcFetchCursor   FETCH cursor
 * @addtogroup zlplCursor
 * @{
 */


/* zlplFetchOrientation 과 mapping을 맞춰야 함 */
SQLSMALLINT gFetchOrientation[ZLPL_FETCH_MAX] =
{
    0,                     /* ZLPL_FETCH_NA = 0 */
    SQL_FETCH_NEXT,        /* ZLPL_FETCH_NEXT */
    SQL_FETCH_PRIOR,       /* ZLPL_FETCH_PRIOR */
    SQL_FETCH_FIRST,       /* ZLPL_FETCH_FIRST */
    SQL_FETCH_LAST,        /* ZLPL_FETCH_LAST */
    SQL_FETCH_RELATIVE,    /* ZLPL_FETCH_CURRENT */
    SQL_FETCH_ABSOLUTE,    /* ZLPL_FETCH_ABSOLUTE */
    SQL_FETCH_RELATIVE,    /* ZLPL_FETCH_RELATIVE */
};

/**
 * @brief FETCH cursor 구문을 수행한다.
 * @param[in]  aSqlContext   SQL Context
 * @param[in]  aSqlArgs      SQL Arguments
 * @remarks
 */
stlStatus zlpcFetchCursor( zlplSqlContext   *aSqlContext,
                           zlplSqlArgs      *aSqlArgs )
{
    SQLRETURN          sReturn;
    zlplCursorSymbol  *sCursorSymbol = NULL;
    SQLLEN             sFetchOffset = 0;
    SQLSMALLINT        sFetchOrientation;

    stlInt32    sIntoCnt = 0;
    SQLLEN             sAffectedRowCount;

    stlInt32           sIteration;
    stlInt64           sTempFetchOffset = 0;

    SQLSMALLINT        sColumnCnt = 0;

    /**
     * 초기화
     */

    ZLPL_INIT_SQLCA(aSqlContext, aSqlArgs->sql_ca);
    STL_TRY( zlpeIsInit() == STL_SUCCESS );

    /* Get array size */
    (void)zlpuGetMinArraySize( aSqlArgs->iters,
                               aSqlArgs->hvc,
                               aSqlArgs->sqlhv,
                               &sIteration );

    STL_TRY( zlpbAdjustDataType( aSqlContext,
                                 aSqlArgs )
             == STL_SUCCESS );

    /*****************************************************************
     * Cursor Symbol 을 검색
     *****************************************************************/

    /**
     * Cursor Symbol 을 검색
     */

    STL_TRY( zlpyLookupCursorSymbol( aSqlContext,
                                     aSqlArgs->sqlfetch->sqlcname,
                                     & sCursorSymbol )
             == STL_SUCCESS );

    STL_TRY_THROW( sCursorSymbol != NULL, RAMP_ERR_LOOKUP_SYMBOL );

    /**
     * 유효성 검사
     */

    STL_TRY_THROW( sCursorSymbol->mIsPrepared == STL_TRUE, RAMP_ERR_LOOKUP_SYMBOL );

    sReturn = SQLNumResultCols( sCursorSymbol->mStmtHandle, & sColumnCnt );
    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             sCursorSymbol->mStmtHandle,
                             (zlplSqlca *) aSqlArgs->sql_ca,
                             (stlChar *) aSqlArgs->sql_state )
             == STL_SUCCESS);

    sIntoCnt = aSqlArgs->hvc;

    if ( sIntoCnt == sColumnCnt )
    {
        /* no problem */
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

    /**
     * 여기까지 내려왔으면,
     *   sIntoCnt == sColumnCnt
     * 상태임
     */
    if( sColumnCnt != 0 )
    {
        /*
         * Cache가 존재하지 않으면 Cache를 생성
         */
        if( zlpcCheckColumnCacheExist( sCursorSymbol,
                                       sIntoCnt ) == STL_FALSE )
        {
            /*
             * sCursorSymbol에 저장된, column과 into 절을 비교
             */
            /**
             * 기존 공간 제거
             */

            STL_TRY( zlpcFreeCursorSymbolColumn( aSqlContext,
                                                 sCursorSymbol )
                     == STL_SUCCESS );

            /**
             * Cursor Symbol 에 Parameter, Result Column 공간 확보
             */

            STL_TRY( zlpcAllocCursorSymbolColumn( aSqlContext,
                                                  sCursorSymbol,
                                                  sIteration,
                                                  sIntoCnt,
                                                  aSqlArgs->sqlhv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /**
         * Parameter와 Column이 존재하지 않는 stmt의 경우는 cache를 pass
         */
    }

    /*****************************************************************
     * Bind Column 을 수행
     *****************************************************************/

    /**
     * 다시 bind 해야 하는 지 check
     */

    if ( zlpxCheckSameSymbolBindAddr( sColumnCnt,
                                      sCursorSymbol->mColumnAddrArray,
                                      sCursorSymbol->mColumnIndArray,
                                      aSqlArgs->sqlhv )
         == STL_TRUE )
    {
        /* nothing to do */
    }
    else
    {
        /****************************************************************
         * 재수행 방지를 위한 Parameter 공간 확보
         ****************************************************************/

        /**
         * SQLBindCol() 을 수행
         */

        STL_TRY( zlpbBindCols( sCursorSymbol->mStmtHandle,
                               aSqlArgs,
                               aSqlArgs->hvc,
                               aSqlArgs->sqlhv,
                               sCursorSymbol->mColumnAddrValueArray,
                               sCursorSymbol->mColumnLocalIndArray )
                 == STL_SUCCESS );

        /**
         * 반복적인 SQLBindCol() 방지를 위해 Host Variable 의 Address 저장
         */

        zlpxStoreSymbolBindAddr( sCursorSymbol->mColumnCnt,
                                 sCursorSymbol->mColumnAddrArray,
                                 sCursorSymbol->mColumnIndArray,
                                 aSqlArgs->sqlhv );
    }

    /*****************************************************************
     * FETCH 를 수행
     *****************************************************************/

    /**
     * Fetch 수행
     */

    STL_TRY( zlpbSetBeforeInfo( aSqlContext,
                                aSqlArgs,
                                sCursorSymbol->mStmtHandle,
                                aSqlArgs->hvc,
                                aSqlArgs->sqlhv,
                                sIteration,
                                &sCursorSymbol->mRowFetched,
                                sCursorSymbol->mRowStatusArray,
                                sCursorSymbol->mColumnAddrValueArray,
                                sCursorSymbol->mColumnLocalIndArray,
                                NULL,
                                STL_FALSE )
             == STL_SUCCESS );

    if(aSqlArgs->sqlfetch->sqlfo == ZLPL_FETCH_NA)
    {
        sReturn = SQLFetch(sCursorSymbol->mStmtHandle);
    }
    else
    {
        sFetchOrientation = gFetchOrientation[aSqlArgs->sqlfetch->sqlfo];
        if ( aSqlArgs->sqlfetch->offsethv != NULL )
        {
            switch ( aSqlArgs->sqlfetch->offsethv[0].data_type )
            {
                case ZLPL_C_DATATYPE_SSHORT:
                    sTempFetchOffset = *(stlInt16*) aSqlArgs->sqlfetch->offsethv[0].value;
                    break;
                case ZLPL_C_DATATYPE_USHORT:
                    sTempFetchOffset = *(stlUInt16*) aSqlArgs->sqlfetch->offsethv[0].value;
                    break;
                case ZLPL_C_DATATYPE_SINT:
                    sTempFetchOffset = *(stlInt32*) aSqlArgs->sqlfetch->offsethv[0].value;
                    break;
                case ZLPL_C_DATATYPE_UINT:
                    sTempFetchOffset = *(stlUInt32*) aSqlArgs->sqlfetch->offsethv[0].value;
                    break;
                case ZLPL_C_DATATYPE_SLONG:
                    sTempFetchOffset = *(stlInt64*) aSqlArgs->sqlfetch->offsethv[0].value;
                    break;
                case ZLPL_C_DATATYPE_ULONG:
                    sTempFetchOffset = *(stlUInt64*) aSqlArgs->sqlfetch->offsethv[0].value;
                    break;
                case ZLPL_C_DATATYPE_SLONGLONG:
                    sTempFetchOffset = *(stlInt64*) aSqlArgs->sqlfetch->offsethv[0].value;
                    break;
                case ZLPL_C_DATATYPE_ULONGLONG:
                    sTempFetchOffset = *(stlUInt64*) aSqlArgs->sqlfetch->offsethv[0].value;
                    break;
                default:
                    STL_DASSERT(0);
                    break;
            }
            sFetchOffset = (SQLLEN)sTempFetchOffset;
        }
        else
        {
            sTempFetchOffset =
                (((stlInt64) aSqlArgs->sqlfetch->sqlfph) << 32)
                + (aSqlArgs->sqlfetch->sqlfpl & 0xFFFFFFFF);
            sFetchOffset = (SQLLEN)sTempFetchOffset;
        }

        sReturn = SQLFetchScroll( sCursorSymbol->mStmtHandle,
                                  sFetchOrientation,
                                  sFetchOffset );
    }

    STL_TRY( zlpeCheckError( sReturn,
                             SQL_HANDLE_STMT,
                             sCursorSymbol->mStmtHandle,
                             (zlplSqlca *)aSqlArgs->sql_ca,
                             (stlChar *)aSqlArgs->sql_state )
             == STL_SUCCESS);

    /**
     * Fetch 후 작업 수행
     */

    if ( sReturn == SQL_NO_DATA )
    {
        /* nohting to do */
    }
    else
    {
        /* Affected Row count set */
        sAffectedRowCount = sCursorSymbol->mRowFetched;
        ((zlplSqlca *)(aSqlArgs->sql_ca))->sqlerrd[2] = (int)sAffectedRowCount;

        /**
         * ROWSTATUS 정보를 얻기 위한 설정
         */

        aSqlArgs->sql_ca->rowstatus = sCursorSymbol->mRowStatusArray;

        STL_TRY( zlpbSetAfterInfo( aSqlContext,
                                   aSqlArgs,
                                   sCursorSymbol->mStmtHandle,
                                   aSqlArgs->unsafenull,
                                   aSqlArgs->hvc,
                                   aSqlArgs->sqlhv,
                                   sIteration,
                                   sCursorSymbol->mColumnAddrValueArray,
                                   sCursorSymbol->mColumnLocalIndArray,
                                   NULL )   /* aDynParamIOType */
                == STL_SUCCESS);
    }

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_LOOKUP_SYMBOL)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_CURSOR_IS_NOT_OPEN,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext),
                      aSqlArgs->sqlfetch->sqlcname );
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

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Symbol 에 저장된 Bind Address 가 Host Variable 의 Address 와 동일한지 검사
 * @param[in]  sCursorSymbol   Cursor Symbol
 * @param[in]  aColumnCnt      Column count
 */
stlBool zlpcCheckColumnCacheExist( zlplCursorSymbol * sCursorSymbol,
                                   stlInt32           aColumnCnt )
{
    if( sCursorSymbol->mColumnCnt != aColumnCnt )
    {
        return STL_FALSE;
    }

    return STL_TRUE;
}

/**
 * @brief Cursor Symbol에서 Column 메모리를 할당한다.
 * @param[in]  aSqlContext     SQL Context
 * @param[in]  aCursorSymbol   Cursor Symbol
 * @param[in]  aArraySize      Parameter, Column의 array size
 * @param[in]  aColumnCnt      Column 개수
 * @param[in]  aHostVar        Host variable array
 * @remarks
 */
stlStatus zlpcAllocCursorSymbolColumn( zlplSqlContext    * aSqlContext,
                                       zlplCursorSymbol  * aCursorSymbol,
                                       stlInt32            aArraySize,
                                       stlInt32            aColumnCnt,
                                       zlplSqlHostVar    * aHostVar )
{
    stlInt32          sRowSize = 0;
    stlInt32          sAllocSize = 0;
    stlInt32          sAddrAreaSize = 0;
    stlInt32          sRowBufOffset;
    stlChar          *sAllocBuffer = NULL;
    stlInt32          i;

    aCursorSymbol->mArraySize = aArraySize;
    if( aArraySize > 0 )
    {
        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     STL_SIZEOF(SQLUSMALLINT) * aArraySize,
                                     (void **) & aCursorSymbol->mRowStatusArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
    }
    else
    {
        aCursorSymbol->mRowStatusArray = NULL;
    }

    aCursorSymbol->mColumnCnt = aColumnCnt;

    if ( aColumnCnt > 0 )
    {
        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     STL_SIZEOF(stlChar *) * aColumnCnt,
                                     (void **) & aCursorSymbol->mColumnAddrArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( aCursorSymbol->mColumnAddrArray, 0x00, STL_SIZEOF( stlChar *) * aColumnCnt );

        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     STL_SIZEOF(stlChar *) * aColumnCnt,
                                     (void **) & aCursorSymbol->mColumnIndArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( aCursorSymbol->mColumnIndArray, 0x00, STL_SIZEOF( stlChar *) * aColumnCnt );

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
            sRowSize += ZLPL_ALIGN( zlpbGetDataBufferLength( &aHostVar[i] ) );
        }

        sAddrAreaSize = ZLPL_ALIGN( aColumnCnt * STL_SIZEOF(stlChar *) );
        sAllocSize = sAddrAreaSize + sRowSize * aArraySize;
        STL_TRY( stlAllocDynamicMem( & aSqlContext->mDynMemory,
                                     sAllocSize,
                                     (void **) & sAllocBuffer,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        stlMemset( sAllocBuffer, 0x00, sAllocSize );
        aCursorSymbol->mColumnAddrValueArray = (stlChar **)sAllocBuffer;

        sRowBufOffset = sAddrAreaSize;
        for( i = 0; i < aColumnCnt; i ++ )
        {
            aCursorSymbol->mColumnAddrValueArray[i] = sAllocBuffer + sRowBufOffset;
            sRowBufOffset += ZLPL_ALIGN( zlpbGetDataBufferLength( &aHostVar[i] ) ) * aArraySize;
        }

        /*
         * Parameter Indicator Buffer allocation
         * : 할당 방식은 위의 Value Buffer 할당 방식과 같다.
         */
        sAllocSize = 0;
        sRowSize = 0;
        for( i = 0; i < aColumnCnt; i ++ )
        {
            if( aHostVar[i].indicator != NULL )
            {
                sRowSize += ZLPL_ALIGN( zlpbGetDataSize( aHostVar[i].ind_type ) );
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
        aCursorSymbol->mColumnIndValueArray = (stlChar **)sAllocBuffer;

        sRowBufOffset = sAddrAreaSize;
        for( i = 0; i < aColumnCnt; i ++ )
        {
            if( aHostVar[i].indicator == NULL )
            {
                aCursorSymbol->mColumnIndValueArray[i] = NULL;
            }
            else
            {
                aCursorSymbol->mColumnIndValueArray[i] = sAllocBuffer + sRowBufOffset;
                sRowBufOffset += ZLPL_ALIGN( zlpbGetDataSize( aHostVar[i].ind_type ) ) * aArraySize;
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
        aCursorSymbol->mColumnLocalIndArray = (SQLLEN **)sAllocBuffer;

        sRowBufOffset = sAddrAreaSize;
        for( i = 0; i < aColumnCnt; i ++ )
        {
            aCursorSymbol->mColumnLocalIndArray[i] = (SQLLEN *)(sAllocBuffer + sRowBufOffset);
            sRowBufOffset += STL_SIZEOF(SQLLEN) * aArraySize;
        }
    }
    else
    {
        aCursorSymbol->mColumnAddrArray = NULL;
        aCursorSymbol->mColumnIndArray = NULL;
        aCursorSymbol->mColumnAddrValueArray = NULL;
        aCursorSymbol->mColumnIndValueArray = NULL;
        aCursorSymbol->mColumnLocalIndArray = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Cursor Symbol에서 Column 메모리를 반환한다.
 * @param[in]  aSqlContext     SQL Context
 * @param[in]  aCursorSymbol   Cursor Symbol
 * @remarks
 */
stlStatus zlpcFreeCursorSymbolColumn( zlplSqlContext    * aSqlContext,
                                      zlplCursorSymbol  * aCursorSymbol )
{
    aCursorSymbol->mColumnCnt = 0;
    aCursorSymbol->mArraySize = 0;

    if( aCursorSymbol->mRowStatusArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aCursorSymbol->mRowStatusArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aCursorSymbol->mRowStatusArray = NULL;
    }

    if( aCursorSymbol->mColumnAddrArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aCursorSymbol->mColumnAddrArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aCursorSymbol->mColumnAddrArray = NULL;
    }

    if( aCursorSymbol->mColumnIndArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aCursorSymbol->mColumnIndArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aCursorSymbol->mColumnIndArray = NULL;
    }

    if( aCursorSymbol->mColumnAddrValueArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aCursorSymbol->mColumnAddrValueArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aCursorSymbol->mColumnAddrValueArray = NULL;
    }

    if( aCursorSymbol->mColumnIndValueArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aCursorSymbol->mColumnIndValueArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aCursorSymbol->mColumnIndValueArray = NULL;
    }

    if( aCursorSymbol->mColumnLocalIndArray != NULL )
    {
        STL_TRY( stlFreeDynamicMem( & aSqlContext->mDynMemory,
                                     aCursorSymbol->mColumnLocalIndArray,
                                     ZLPL_CTXT_ERROR_STACK(aSqlContext) )
                 == STL_SUCCESS );
        aCursorSymbol->mColumnLocalIndArray = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} zlpcFetchCursor */
