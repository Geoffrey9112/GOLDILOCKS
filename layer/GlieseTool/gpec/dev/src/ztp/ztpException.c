/*******************************************************************************
 * ztpException.c
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

#include <dtl.h>
#include <goldilocks.h>
#include <ztpDef.h>
#include <ztpMisc.h>
#include <ztpException.h>
#include <ztpCParseFunc.h>

/**
 * @file ztpException.c
 * @brief Gliese Embedded SQL in C precompiler exception handling functions
 */

/**
 * @addtogroup ztpException
 * @{
 */
ztpExceptCondFunc gExceptCondFunc[ZTP_EXCEPTION_COND_MAX] =
{
    ztpExceptCondSqlState,     /* ZTP_EXCEPTION_COND_SQLSTATE   */
    ztpExceptCondNotFound,     /* ZTP_EXCEPTION_COND_NOT_FOUND  */
    ztpExceptCondSqlError,     /* ZTP_EXCEPTION_COND_SQLERROR   */
    ztpExceptCondSqlWarning,   /* ZTP_EXCEPTION_COND_SQLWARNING */
};

ztpExceptActFunc gExceptActFunc[ZTP_EXCEPTION_ACT_MAX] =
{
    ztpExceptActGoto,          /* ZTP_EXCEPTION_ACT_GOTO      */
    ztpExceptActContinue,      /* ZTP_EXCEPTION_ACT_CONTINUE  */
    ztpExceptActStop,          /* ZTP_EXCEPTION_ACT_STOP      */
    ztpExceptActDo,            /* ZTP_EXCEPTION_ACT_DO        */
};

stlStatus ztpExceptCondSqlError(ztpCParseParam  *aParam,
                                stlInt32         aCondType,
                                stlChar         *aExceptionStr)
{
    stlSnprintf(aExceptionStr,
                ZTP_LINE_BUF_SIZE,
                "    if(sqlca.sqlcode < 0) ");

    return STL_SUCCESS;
}

stlStatus ztpExceptCondSqlWarning(ztpCParseParam  *aParam,
                                  stlInt32         aCondType,
                                  stlChar         *aExceptionStr)
{
    stlSnprintf(aExceptionStr,
                ZTP_LINE_BUF_SIZE,
                "    if(sqlca.sqlcode > 0) ");

    return STL_SUCCESS;
}

stlStatus ztpExceptCondNotFound(ztpCParseParam  *aParam,
                                stlInt32         aCondType,
                                stlChar         *aExceptionStr)
{
    stlSnprintf(aExceptionStr,
                ZTP_LINE_BUF_SIZE,
                "    if(sqlca.sqlcode == %d) ", SQL_NO_DATA);

    return STL_SUCCESS;
}

stlStatus ztpExceptCondSqlState(ztpCParseParam  *aParam,
                                stlInt32         aCondType,
                                stlChar         *aExceptionStr)
{
    ztpExceptionCondition  *sExceptCond = aParam->mException[aCondType]->mCondition;
    stlInt32                sSqlStateLen;
    stlChar                *sSqlState;

    STL_TRY(sExceptCond != NULL);
    sSqlState = sExceptCond->mValue;

    sSqlStateLen = stlStrlen(sSqlState);
    switch(sSqlStateLen)
    {
        case 5:
            stlSnprintf(aExceptionStr,
                        ZTP_LINE_BUF_SIZE,
                        "    if(   (SQLSTATE[0] == '%c')\n"
                        "       && (SQLSTATE[1] == '%c')\n"
                        "       && (SQLSTATE[2] == '%c')\n"
                        "       && (SQLSTATE[3] == '%c')\n"
                        "       && (SQLSTATE[4] == '%c') ) ",
                        sSqlState[0], sSqlState[1], sSqlState[2],
                        sSqlState[3], sSqlState[4]);
            break;
        case 2:
            stlSnprintf(aExceptionStr,
                        ZTP_LINE_BUF_SIZE,
                        "    if(   (SQLSTATE[0] == '%c')\n"
                        "       && (SQLSTATE[1] == '%c') ) ",
                        sSqlState[0], sSqlState[1]);
            break;
        default:
            STL_TRY(STL_FALSE);
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpExceptActGoto(ztpCParseParam  *aParam,
                           stlInt32         aCondType,
                           stlChar         *aExceptionStr)
{
    ztpExceptionAction  *sExceptAct = aParam->mException[aCondType]->mAction;
    stlInt32             sActionMaxLen;
    stlChar             *sActionStr;
    stlChar              sLineBuffer[ZTP_LINE_BUF_SIZE];

    STL_TRY(sExceptAct != NULL);
    sActionStr = sExceptAct->mValue;
    sActionMaxLen = ZTP_LINE_BUF_SIZE - stlStrlen(aExceptionStr);

    stlSnprintf(sLineBuffer, sActionMaxLen,
                "goto %s;\n", sActionStr);

    stlStrncat(aExceptionStr, sLineBuffer, sActionMaxLen);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpExceptActContinue(ztpCParseParam  *aParam,
                               stlInt32         aCondType,
                               stlChar         *aExceptionStr)
{
    aExceptionStr[0] = '\0';

    return STL_SUCCESS;
}

stlStatus ztpExceptActStop(ztpCParseParam  *aParam,
                           stlInt32         aCondType,
                           stlChar         *aExceptionStr)
{
    ztpExceptionAction  *sExceptAct = aParam->mException[aCondType]->mAction;
    stlInt32             sActionMaxLen;
    stlChar             *sActionStr;
    stlChar              sLineBuffer[ZTP_LINE_BUF_SIZE];

    STL_TRY(sExceptAct != NULL);
    sActionStr = sExceptAct->mValue;
    sActionMaxLen = ZTP_LINE_BUF_SIZE - stlStrlen(aExceptionStr);

    stlSnprintf(sLineBuffer, sActionMaxLen,
                "exit(1);\n", sActionStr);

    stlStrncat(aExceptionStr, sLineBuffer, sActionMaxLen);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpExceptActDo(ztpCParseParam  *aParam,
                         stlInt32         aCondType,
                         stlChar         *aExceptionStr)
{
    ztpExceptionAction  *sExceptAct = aParam->mException[aCondType]->mAction;
    stlInt32             sActionMaxLen;
    stlChar             *sActionStr;
    stlChar              sLineBuffer[ZTP_LINE_BUF_SIZE];

    STL_TRY(sExceptAct != NULL);
    sActionStr = sExceptAct->mValue;
    sActionMaxLen = ZTP_LINE_BUF_SIZE - stlStrlen(aExceptionStr);

    stlSnprintf(sLineBuffer, sActionMaxLen,
                "%s;\n", sActionStr);

    stlStrncat(aExceptionStr, sLineBuffer, sActionMaxLen);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
