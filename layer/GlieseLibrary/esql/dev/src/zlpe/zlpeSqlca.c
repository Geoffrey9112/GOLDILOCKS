/*******************************************************************************
 * zlpeSqlca.c
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
#include <zlpeSqlca.h>
#include <zlpuMisc.h>

/**
 * @file zlpeSqlca.c
 * @brief Gliese Embedded SQL in C precompiler SQLCA functions
 */

/**
 * @addtogroup zlpeSqlca
 * @{
 */

sqlca_t sqlca_init =
{
    {'S', 'Q', 'L', 'C', 'A', ' ', ' ', ' '},
    sizeof(sqlca_t),
    0,
    { 0, {0}},
    {'N', 'O', 'T', ' ', 'S', 'E', 'T', ' '},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    NULL
};

stlStatus zlpeIsInit()
{
    stlBool sInit = (stlBool) stlAtomicRead32(&gIsInit);

    STL_TRY_THROW( sInit == STL_TRUE, RAMP_ERR_UNINTIALIZED );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_UNINTIALIZED )
    {
        /* 초기화되지 않은 경우이므로, 독자적인 처리 routine을 갖는다. */
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_CONNECTION_NOT_ESTABLISHED,
                      NULL,
                      &gErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlpeCheckError(SQLRETURN    sReturn,
                         SQLSMALLINT  aHandleType,
                         SQLHANDLE    aHandle,
                         zlplSqlca   *aSqlca,
                         stlChar     *aSqlState)
{
    switch(sReturn)
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            (void)zlpeSetException(aHandleType,
                                   aHandle,
                                   aSqlca,
                                   aSqlState);
            break;
        case SQL_NO_DATA:
            zlpeSetSqlCode(aSqlca, SQL_NO_DATA);
            zlpeSetErrorMsg(aSqlca, "No Data");
            stlStrncpy(aSqlState, "02000", 6);
            break;
        case SQL_INVALID_HANDLE:
            zlpeSetSqlCode(aSqlca, SQL_INVALID_HANDLE);
            zlpeSetErrorMsg(aSqlca, "Invalid handle");
            stlStrncpy(aSqlState, "HY000", 6);
            STL_TRY(STL_FALSE);
            break;
        default:
            (void)zlpeSetException(aHandleType,
                                   aHandle,
                                   aSqlca,
                                   aSqlState);
            STL_TRY(STL_FALSE);
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlpeSetException(SQLSMALLINT  aHandleType,
                           SQLHANDLE    aHandle,
                           zlplSqlca   *aSqlca,
                           stlChar     *aSqlState)
{
    SQLINTEGER     sSqlStateNum;
    SQLINTEGER     sNativeError;
    SQLRETURN      sRet;

    /* SQLGetDiagRec returns the currrent values that contains error, warning */
    sRet = SQLGetDiagRec(aHandleType,
                         aHandle,
                         1,
                         (SQLCHAR *)aSqlState,
                         (SQLINTEGER *)&sNativeError,
                         (SQLCHAR *)aSqlca->sqlerrm.sqlerrmc,
                         SQLERRMC_LEN,
                         (SQLSMALLINT *)&aSqlca->sqlerrm.sqlerrml);

    STL_TRY_THROW(sRet != SQL_NO_DATA && sRet != SQL_INVALID_HANDLE,
                  EXIT_FUNC);

    /* Check Warning */
    if(stlStrncmp(aSqlState, "00000", 5) == 0)
    {
        aSqlca->sqlcode = 0;
    }
    else if(stlStrncmp(aSqlState, "01", 2) == 0)
    {
        ZLPL_SET_WARNING_GENERAL(aSqlca);
        sSqlStateNum = STL_MAKE_SQLSTATE(aSqlState[0],
                                         aSqlState[1],
                                         aSqlState[2],
                                         aSqlState[3],
                                         aSqlState[4]);

        switch( sSqlStateNum )
        {
            case STL_EXT_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATION_WARNING:
                zlpeSetWarnStringTruncate(aSqlca);
                break;
            default:
                break;
        }

        aSqlca->sqlcode = sNativeError;
    }
    else
    {
        aSqlca->sqlcode = -sNativeError;
    }

    STL_RAMP(EXIT_FUNC);
    return STL_SUCCESS;
}

stlStatus zlpeInternalToSqlca(stlErrorStack *aErrorStack,
                              zlplSqlca     *aSqlca,
                              stlChar       *aSqlState)
{
    stlErrorData  *sLastErrorData;
    stlInt32       sMsgLen;

    sLastErrorData = stlGetLastErrorData(aErrorStack);

    sMsgLen = stlStrlen(sLastErrorData->mDetailErrorMessage);
    if(sMsgLen != 0)
    {
        (void)stlStrncpy(aSqlca->sqlerrm.sqlerrmc,
                         sLastErrorData->mDetailErrorMessage,
                         SQLERRMC_LEN);
        aSqlca->sqlerrm.sqlerrml = sMsgLen;
    }
    else
    {
        (void)stlStrncpy(aSqlca->sqlerrm.sqlerrmc,
                         sLastErrorData->mErrorMessage,
                         SQLERRMC_LEN);
        aSqlca->sqlerrm.sqlerrml = stlStrlen(sLastErrorData->mErrorMessage);
    }

    aSqlca->sqlcode = -(stlInt32)sLastErrorData->mErrorCode;
    stlMakeSqlState( sLastErrorData->mExternalErrorCode,
                     aSqlState );

    return STL_SUCCESS;
}

stlStatus zlpeSetSuccess(zlplSqlca *aSqlca)
{
    aSqlca->sqlcode = 0;
    aSqlca->sqlerrm.sqlerrml = 0;
    aSqlca->sqlerrm.sqlerrmc[0] = '\0';

    return STL_SUCCESS;
}

stlStatus zlpeSetErrorMsg(zlplSqlca *aSqlca, stlChar *aMsg)
{
    stlInt32    sLen;

    sLen = stlStrlen(aMsg);
    STL_TRY(sLen < SQLERRMC_LEN);

    aSqlca->sqlerrm.sqlerrml = sLen;
    stlStrncpy(aSqlca->sqlerrm.sqlerrmc, aMsg, sLen + 1);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlpeSetSqlCode(zlplSqlca *aSqlca, stlInt32 aSqlCode)
{
    aSqlca->sqlcode = aSqlCode;

    return STL_SUCCESS;
}

stlStatus zlpeSetProcessRowCount(zlplSqlca *aSqlca, stlInt32 aRowCount)
{
    aSqlca->sqlerrd[2] = aRowCount;

    return STL_SUCCESS;
}

stlStatus zlpeSetWarnStringTruncate(zlplSqlca *aSqlca)
{
    ZLPL_MARK_WARNING(aSqlca, 1);
    ZLPL_SET_WARNING_GENERAL(aSqlca);

    return STL_SUCCESS;
}

stlStatus zlpeSetErrorConnectionDoesNotExist(zlplSqlca *aSqlca)
{
    zlpeSetSqlCode(aSqlca, ZLPL_ERRCODE_CONNECTION_NOT_OPEN);
    zlpeSetErrorMsg(aSqlca, "connection not open");
    stlMakeSqlState( STL_EXT_ERRCODE_CONNECTION_EXCEPTION_CONNECTION_DOES_NOT_EXIST,
                     aSqlca->sqlstate );

    return STL_SUCCESS;
}

/** @} */
