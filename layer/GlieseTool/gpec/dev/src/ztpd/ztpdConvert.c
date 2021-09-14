/*******************************************************************************
 * ztpdConvert.c
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
#include <ztpdConvert.h>
#include <ztpCParseFunc.h>
#include <ztpwPrecomp.h>

/**
 * @file ztpdConvert.c
 * @brief Gliese Embedded SQL in C precompiler miscellaneous functions
 */

/**
 * @addtogroup ztpd
 * @{
 */

ztpCvtDTFunc gCvtDTFunc[ZLPL_C_DATATYPE_MAX] =
{
    ztpdConvertNativeType,        /* ZLPL_C_DATATYPE_UNKNOWN */
    ztpdConvertNativeType,        /* ZLPL_C_DATATYPE_SCHAR */
    ztpdConvertNativeType,        /* ZLPL_C_DATATYPE_UCHAR */
    ztpdConvertNativeType,        /* ZLPL_C_DATATYPE_SSHORT */
    ztpdConvertNativeType,        /* ZLPL_C_DATATYPE_USHORT */
    ztpdConvertNativeType,        /* ZLPL_C_DATATYPE_SINT */
    ztpdConvertNativeType,        /* ZLPL_C_DATATYPE_UINT */
    ztpdConvertNativeType,        /* ZLPL_C_DATATYPE_SLONG */
    ztpdConvertNativeType,        /* ZLPL_C_DATATYPE_ULONG */
    ztpdConvertNativeType,        /* ZLPL_C_DATATYPE_SLONGLONG */
    ztpdConvertNativeType,        /* ZLPL_C_DATATYPE_ULONGLONG */
    ztpdConvertNativeType,        /* ZLPL_C_DATATYPE_FLOAT */
    ztpdConvertNativeType,        /* ZLPL_C_DATATYPE_DOUBLE */
    ztpdConvertNativeType,        /* ZLPL_C_DATATYPE_CHAR */
    ztpdConvertVarchar,           /* ZLPL_C_DATATYPE_VARCHAR */
    ztpdConvertLongVarchar,       /* ZLPL_C_DATATYPE_LONGVARCHAR */
    ztpdConvertBinary,            /* ZLPL_C_DATATYPE_BINARY */
    ztpdConvertVarBinary,         /* ZLPL_C_DATATYPE_VARBINARY */
    ztpdConvertLongVarBinary,     /* ZLPL_C_DATATYPE_LONGVARBINARY */
    ztpdConvertBoolean,           /* ZLPL_C_DATATYPE_BOOLEAN */
    ztpdConvertNumeric,           /* ZLPL_C_DATATYPE_NUMBER */
    ztpdConvertDate,              /* ZLPL_C_DATATYPE_DATE */
    ztpdConvertTime,              /* ZLPL_C_DATATYPE_TIME */
    ztpdConvertTimestamp,         /* ZLPL_C_DATATYPE_TIMESTAMP */
    ztpdConvertTimeTz,            /* ZLPL_C_DATATYPE_TIMETZ */
    ztpdConvertTimestampTz,       /* ZLPL_C_DATATYPE_TIMESTAMPTZ */
    ztpdConvertInterval,          /* ZLPL_C_DATATYPE_INTERVAL_YEAR */
    ztpdConvertInterval,          /* ZLPL_C_DATATYPE_INTERVAL_MONTH */
    ztpdConvertInterval,          /* ZLPL_C_DATATYPE_INTERVAL_DAY */
    ztpdConvertInterval,          /* ZLPL_C_DATATYPE_INTERVAL_HOUR */
    ztpdConvertInterval,          /* ZLPL_C_DATATYPE_INTERVAL_MINUTE */
    ztpdConvertInterval,          /* ZLPL_C_DATATYPE_INTERVAL_SECOND */
    ztpdConvertInterval,          /* ZLPL_C_DATATYPE_INTERVAL_YEAR_TO_MONTH */
    ztpdConvertInterval,          /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_HOUR */
    ztpdConvertInterval,          /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_MINUTE */
    ztpdConvertInterval,          /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_SECOND */
    ztpdConvertInterval,          /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_MINUTE */
    ztpdConvertInterval,          /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_SECOND */
    ztpdConvertInterval,          /* ZLPL_C_DATATYPE_INTERVAL_MINUTE_TO_SECOND */
    ztpdConvertNativeType,        /* ZLPL_C_DATATYPE_SQL_CONTEXT */
    ztpdConvertStruct,            /* ZLPL_C_DATATYPE_STRUCT */
};

/******* Convert Data type functions ***************/
stlStatus ztpdConvertNativeType(ztpCParseParam  *aParam,
                                ztpCDeclaration *aCDeclaration,
                                ztpCVariable    *aCVariableListHead)
{
    return STL_SUCCESS;
}

stlStatus ztpdConvertVarchar(ztpCParseParam  *aParam,
                             ztpCDeclaration *aCDeclaration,
                             ztpCVariable    *aCVariableListHead)
{
    stlChar            sStructBuf[ZTP_LINE_BUF_SIZE];
    ztpCVariable      *sCVariable;

    aParam->mCCodeEndLoc = aCDeclaration->mStartPos;
    ztpwBypassCCode(aParam);

    ztpwSendStringToOutFile(aParam, "/* ");
    ztpwBypassCCodeByLoc(aParam,
                         aCDeclaration->mStartPos,
                         aCDeclaration->mEndPos);
    ztpwSendStringToOutFile(aParam, " */");

    for(sCVariable = aCVariableListHead;
        sCVariable != NULL;
        sCVariable = sCVariable->mNext)
    {
        if( sCVariable->mIsArray == STL_TRUE )
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "struct { int len; char arr[%s]; } %s[%s];\n",
                        sCVariable->mCharLength,
                        sCVariable->mName, sCVariable->mArrayValue);
        }
        else
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "struct { int len; char arr[%s]; } %s;\n",
                        sCVariable->mCharLength,
                        sCVariable->mName);
        }

        ztpwBypassCCodeByLoc(aParam,
                             aCDeclaration->mStartPos - aCDeclaration->mIndent,
                             aCDeclaration->mStartPos);
        ztpwSendStringToOutFile(aParam, sStructBuf);
    }

    aParam->mCCodeStartLoc = aCDeclaration->mEndPos;

    STL_TRY(ztpwPrecompSourceLine(aParam,
                                  aCDeclaration->mLineNo,
                                  aParam->mInFileName,
                                  aParam->mOutFileName)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpdConvertLongVarchar(ztpCParseParam  *aParam,
                                 ztpCDeclaration *aCDeclaration,
                                 ztpCVariable    *aCVariableListHead)
{
    stlChar            sStructBuf[ZTP_LINE_BUF_SIZE];
    ztpCVariable      *sCVariable;

    aParam->mCCodeEndLoc = aCDeclaration->mStartPos;
    ztpwBypassCCode(aParam);

    ztpwSendStringToOutFile(aParam, "/* ");
    ztpwBypassCCodeByLoc(aParam,
                         aCDeclaration->mStartPos,
                         aCDeclaration->mEndPos);
    ztpwSendStringToOutFile(aParam, " */");

    for(sCVariable = aCVariableListHead;
        sCVariable != NULL;
        sCVariable = sCVariable->mNext)
    {
        if( sCVariable->mIsArray == STL_TRUE )
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_LONG_VARIABLE_LENGTH_STRUCT %s[%s];\n",
                        sCVariable->mName, sCVariable->mArrayValue);
        }
        else
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_LONG_VARIABLE_LENGTH_STRUCT %s;\n",
                        sCVariable->mName);
        }

        ztpwBypassCCodeByLoc(aParam,
                             aCDeclaration->mStartPos - aCDeclaration->mIndent,
                             aCDeclaration->mStartPos);
        ztpwSendStringToOutFile(aParam, sStructBuf);
    }

    aParam->mCCodeStartLoc = aCDeclaration->mEndPos;

    STL_TRY(ztpwPrecompSourceLine(aParam,
                                  aCDeclaration->mLineNo,
                                  aParam->mInFileName,
                                  aParam->mOutFileName)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpdConvertBinary(ztpCParseParam  *aParam,
                            ztpCDeclaration *aCDeclaration,
                            ztpCVariable    *aCVariableListHead)
{
    stlChar            sStructBuf[ZTP_LINE_BUF_SIZE];
    ztpCVariable      *sCVariable;

    aParam->mCCodeEndLoc = aCDeclaration->mStartPos;
    ztpwBypassCCode(aParam);

    ztpwSendStringToOutFile(aParam, "/* ");
    ztpwBypassCCodeByLoc(aParam,
                         aCDeclaration->mStartPos,
                         aCDeclaration->mEndPos);
    ztpwSendStringToOutFile(aParam, " */");

    for(sCVariable = aCVariableListHead;
        sCVariable != NULL;
        sCVariable = sCVariable->mNext)
    {
        if( sCVariable->mIsArray == STL_TRUE )
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "char %s[%s][%s];\n",
                        sCVariable->mName, sCVariable->mCharLength,
                        sCVariable->mArrayValue);
        }
        else
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "char %s[%s];\n",
                        sCVariable->mName, sCVariable->mCharLength);
        }

        ztpwBypassCCodeByLoc(aParam,
                             aCDeclaration->mStartPos - aCDeclaration->mIndent,
                             aCDeclaration->mStartPos);
        ztpwSendStringToOutFile(aParam, sStructBuf);
    }

    aParam->mCCodeStartLoc = aCDeclaration->mEndPos;

    STL_TRY(ztpwPrecompSourceLine(aParam,
                                  aCDeclaration->mLineNo,
                                  aParam->mInFileName,
                                  aParam->mOutFileName)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpdConvertVarBinary(ztpCParseParam  *aParam,
                               ztpCDeclaration *aCDeclaration,
                               ztpCVariable    *aCVariableListHead)
{
    stlChar            sStructBuf[ZTP_LINE_BUF_SIZE];
    ztpCVariable      *sCVariable;

    aParam->mCCodeEndLoc = aCDeclaration->mStartPos;
    ztpwBypassCCode(aParam);

    ztpwSendStringToOutFile(aParam, "/* ");
    ztpwBypassCCodeByLoc(aParam,
                         aCDeclaration->mStartPos,
                         aCDeclaration->mEndPos);
    ztpwSendStringToOutFile(aParam, " */");

    for(sCVariable = aCVariableListHead;
        sCVariable != NULL;
        sCVariable = sCVariable->mNext)
    {
        if( sCVariable->mIsArray == STL_TRUE )
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "struct { int len; char arr[%s]; } %s[%s];\n",
                        sCVariable->mCharLength,
                        sCVariable->mName, sCVariable->mArrayValue);
        }
        else
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "struct { int len; char arr[%s]; } %s;\n",
                        sCVariable->mCharLength,
                        sCVariable->mName);
        }

        ztpwBypassCCodeByLoc(aParam,
                             aCDeclaration->mStartPos - aCDeclaration->mIndent,
                             aCDeclaration->mStartPos);
        ztpwSendStringToOutFile(aParam, sStructBuf);
    }

    aParam->mCCodeStartLoc = aCDeclaration->mEndPos;

    STL_TRY(ztpwPrecompSourceLine(aParam,
                                  aCDeclaration->mLineNo,
                                  aParam->mInFileName,
                                  aParam->mOutFileName)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpdConvertLongVarBinary(ztpCParseParam  *aParam,
                                   ztpCDeclaration *aCDeclaration,
                                   ztpCVariable    *aCVariableListHead)
{
    stlChar            sStructBuf[ZTP_LINE_BUF_SIZE];
    ztpCVariable      *sCVariable;

    aParam->mCCodeEndLoc = aCDeclaration->mStartPos;
    ztpwBypassCCode(aParam);

    ztpwSendStringToOutFile(aParam, "/* ");
    ztpwBypassCCodeByLoc(aParam,
                         aCDeclaration->mStartPos,
                         aCDeclaration->mEndPos);
    ztpwSendStringToOutFile(aParam, " */");

    for(sCVariable = aCVariableListHead;
        sCVariable != NULL;
        sCVariable = sCVariable->mNext)
    {
        if( sCVariable->mIsArray == STL_TRUE )
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_LONG_VARIABLE_LENGTH_STRUCT %s[%s];\n",
                        sCVariable->mName, sCVariable->mArrayValue);
        }
        else
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_LONG_VARIABLE_LENGTH_STRUCT %s;\n",
                        sCVariable->mName);
        }

        ztpwBypassCCodeByLoc(aParam,
                             aCDeclaration->mStartPos - aCDeclaration->mIndent,
                             aCDeclaration->mStartPos);
        ztpwSendStringToOutFile(aParam, sStructBuf);
    }

    aParam->mCCodeStartLoc = aCDeclaration->mEndPos;

    STL_TRY(ztpwPrecompSourceLine(aParam,
                                  aCDeclaration->mLineNo,
                                  aParam->mInFileName,
                                  aParam->mOutFileName)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpdConvertBoolean(ztpCParseParam  *aParam,
                             ztpCDeclaration *aCDeclaration,
                             ztpCVariable    *aCVariableListHead)
{
    stlChar            sStructBuf[ZTP_LINE_BUF_SIZE];
    ztpCVariable      *sCVariable;

    aParam->mCCodeEndLoc = aCDeclaration->mStartPos;
    ztpwBypassCCode(aParam);

    ztpwSendStringToOutFile(aParam, "/* ");
    ztpwBypassCCodeByLoc(aParam,
                         aCDeclaration->mStartPos,
                         aCDeclaration->mEndPos);
    ztpwSendStringToOutFile(aParam, " */");

    for(sCVariable = aCVariableListHead;
        sCVariable != NULL;
        sCVariable = sCVariable->mNext)
    {
        if( sCVariable->mIsArray == STL_TRUE )
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "char %s[%s];\n",
                        sCVariable->mName, sCVariable->mArrayValue);
        }
        else
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "char %s;\n",
                        sCVariable->mName);
        }

        ztpwBypassCCodeByLoc(aParam,
                             aCDeclaration->mStartPos - aCDeclaration->mIndent,
                             aCDeclaration->mStartPos);
        ztpwSendStringToOutFile(aParam, sStructBuf);
    }

    aParam->mCCodeStartLoc = aCDeclaration->mEndPos;

    STL_TRY(ztpwPrecompSourceLine(aParam,
                                  aCDeclaration->mLineNo,
                                  aParam->mInFileName,
                                  aParam->mOutFileName)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpdConvertNumeric(ztpCParseParam  *aParam,
                             ztpCDeclaration *aCDeclaration,
                             ztpCVariable    *aCVariableListHead)
{
    stlChar            sStructBuf[ZTP_LINE_BUF_SIZE];
    ztpCVariable      *sCVariable;

    aParam->mCCodeEndLoc = aCDeclaration->mStartPos;
    ztpwBypassCCode(aParam);

    ztpwSendStringToOutFile(aParam, "/* ");
    ztpwBypassCCodeByLoc(aParam,
                         aCDeclaration->mStartPos,
                         aCDeclaration->mEndPos);
    ztpwSendStringToOutFile(aParam, " */");

    for(sCVariable = aCVariableListHead;
        sCVariable != NULL;
        sCVariable = sCVariable->mNext)
    {
        if( sCVariable->mIsArray == STL_TRUE )
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_NUMERIC_STRUCT %s[%s];\n",
                        sCVariable->mName, sCVariable->mArrayValue);
        }
        else
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_NUMERIC_STRUCT %s;\n",
                        sCVariable->mName);
        }

        ztpwBypassCCodeByLoc(aParam,
                             aCDeclaration->mStartPos - aCDeclaration->mIndent,
                             aCDeclaration->mStartPos);
        ztpwSendStringToOutFile(aParam, sStructBuf);
    }

    aParam->mCCodeStartLoc = aCDeclaration->mEndPos;

    STL_TRY(ztpwPrecompSourceLine(aParam,
                                  aCDeclaration->mLineNo,
                                  aParam->mInFileName,
                                  aParam->mOutFileName)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpdConvertDate(ztpCParseParam  *aParam,
                          ztpCDeclaration *aCDeclaration,
                          ztpCVariable    *aCVariableListHead)
{
    stlChar            sStructBuf[ZTP_LINE_BUF_SIZE];
    ztpCVariable      *sCVariable;

    aParam->mCCodeEndLoc = aCDeclaration->mStartPos;
    ztpwBypassCCode(aParam);

    ztpwSendStringToOutFile(aParam, "/* ");
    ztpwBypassCCodeByLoc(aParam,
                         aCDeclaration->mStartPos,
                         aCDeclaration->mEndPos);
    ztpwSendStringToOutFile(aParam, " */");

    for(sCVariable = aCVariableListHead;
        sCVariable != NULL;
        sCVariable = sCVariable->mNext)
    {
        if( sCVariable->mIsArray == STL_TRUE )
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_TIMESTAMP_STRUCT %s[%s];\n",
                        sCVariable->mName, sCVariable->mArrayValue);
        }
        else
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_TIMESTAMP_STRUCT %s;\n",
                        sCVariable->mName);
        }

        ztpwBypassCCodeByLoc(aParam,
                             aCDeclaration->mStartPos - aCDeclaration->mIndent,
                             aCDeclaration->mStartPos);
        ztpwSendStringToOutFile(aParam, sStructBuf);
    }

    aParam->mCCodeStartLoc = aCDeclaration->mEndPos;

    STL_TRY(ztpwPrecompSourceLine(aParam,
                                  aCDeclaration->mLineNo,
                                  aParam->mInFileName,
                                  aParam->mOutFileName)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpdConvertTime(ztpCParseParam  *aParam,
                          ztpCDeclaration *aCDeclaration,
                          ztpCVariable    *aCVariableListHead)
{
    stlChar            sStructBuf[ZTP_LINE_BUF_SIZE];
    ztpCVariable      *sCVariable;

    aParam->mCCodeEndLoc = aCDeclaration->mStartPos;
    ztpwBypassCCode(aParam);

    ztpwSendStringToOutFile(aParam, "/* ");
    ztpwBypassCCodeByLoc(aParam,
                         aCDeclaration->mStartPos,
                         aCDeclaration->mEndPos);
    ztpwSendStringToOutFile(aParam, " */");

    for(sCVariable = aCVariableListHead;
        sCVariable != NULL;
        sCVariable = sCVariable->mNext)
    {
        if( sCVariable->mIsArray == STL_TRUE )
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_TIME_STRUCT %s[%s];\n",
                        sCVariable->mName, sCVariable->mArrayValue);
        }
        else
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_TIME_STRUCT %s;\n",
                        sCVariable->mName);
        }

        ztpwBypassCCodeByLoc(aParam,
                             aCDeclaration->mStartPos - aCDeclaration->mIndent,
                             aCDeclaration->mStartPos);
        ztpwSendStringToOutFile(aParam, sStructBuf);
    }

    aParam->mCCodeStartLoc = aCDeclaration->mEndPos;

    STL_TRY(ztpwPrecompSourceLine(aParam,
                                  aCDeclaration->mLineNo,
                                  aParam->mInFileName,
                                  aParam->mOutFileName)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpdConvertTimestamp(ztpCParseParam  *aParam,
                               ztpCDeclaration *aCDeclaration,
                               ztpCVariable    *aCVariableListHead)
{
    stlChar            sStructBuf[ZTP_LINE_BUF_SIZE];
    ztpCVariable      *sCVariable;

    aParam->mCCodeEndLoc = aCDeclaration->mStartPos;
    ztpwBypassCCode(aParam);

    ztpwSendStringToOutFile(aParam, "/* ");
    ztpwBypassCCodeByLoc(aParam,
                         aCDeclaration->mStartPos,
                         aCDeclaration->mEndPos);
    ztpwSendStringToOutFile(aParam, " */");

    for(sCVariable = aCVariableListHead;
        sCVariable != NULL;
        sCVariable = sCVariable->mNext)
    {
        if( sCVariable->mIsArray == STL_TRUE )
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_TIMESTAMP_STRUCT %s[%s];\n",
                        sCVariable->mName, sCVariable->mArrayValue);
        }
        else
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_TIMESTAMP_STRUCT %s;\n",
                        sCVariable->mName);
        }

        ztpwBypassCCodeByLoc(aParam,
                             aCDeclaration->mStartPos - aCDeclaration->mIndent,
                             aCDeclaration->mStartPos);
        ztpwSendStringToOutFile(aParam, sStructBuf);
    }

    aParam->mCCodeStartLoc = aCDeclaration->mEndPos;

    STL_TRY(ztpwPrecompSourceLine(aParam,
                                  aCDeclaration->mLineNo,
                                  aParam->mInFileName,
                                  aParam->mOutFileName)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpdConvertTimeTz(ztpCParseParam  *aParam,
                            ztpCDeclaration *aCDeclaration,
                            ztpCVariable    *aCVariableListHead)
{
    stlChar            sStructBuf[ZTP_LINE_BUF_SIZE];
    ztpCVariable      *sCVariable;

    aParam->mCCodeEndLoc = aCDeclaration->mStartPos;
    ztpwBypassCCode(aParam);

    ztpwSendStringToOutFile(aParam, "/* ");
    ztpwBypassCCodeByLoc(aParam,
                         aCDeclaration->mStartPos,
                         aCDeclaration->mEndPos);
    ztpwSendStringToOutFile(aParam, " */");

    for(sCVariable = aCVariableListHead;
        sCVariable != NULL;
        sCVariable = sCVariable->mNext)
    {
        if( sCVariable->mIsArray == STL_TRUE )
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_TIME_WITH_TIMEZONE_STRUCT %s[%s];\n",
                        sCVariable->mName, sCVariable->mArrayValue);
        }
        else
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_TIME_WITH_TIMEZONE_STRUCT %s;\n",
                        sCVariable->mName);
        }

        ztpwBypassCCodeByLoc(aParam,
                             aCDeclaration->mStartPos - aCDeclaration->mIndent,
                             aCDeclaration->mStartPos);
        ztpwSendStringToOutFile(aParam, sStructBuf);
    }

    aParam->mCCodeStartLoc = aCDeclaration->mEndPos;

    STL_TRY(ztpwPrecompSourceLine(aParam,
                                  aCDeclaration->mLineNo,
                                  aParam->mInFileName,
                                  aParam->mOutFileName)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpdConvertTimestampTz(ztpCParseParam  *aParam,
                                 ztpCDeclaration *aCDeclaration,
                                 ztpCVariable    *aCVariableListHead)
{
    stlChar            sStructBuf[ZTP_LINE_BUF_SIZE];
    ztpCVariable      *sCVariable;

    aParam->mCCodeEndLoc = aCDeclaration->mStartPos;
    ztpwBypassCCode(aParam);

    ztpwSendStringToOutFile(aParam, "/* ");
    ztpwBypassCCodeByLoc(aParam,
                         aCDeclaration->mStartPos,
                         aCDeclaration->mEndPos);
    ztpwSendStringToOutFile(aParam, " */");

    for(sCVariable = aCVariableListHead;
        sCVariable != NULL;
        sCVariable = sCVariable->mNext)
    {
        if( sCVariable->mIsArray == STL_TRUE )
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT %s[%s];\n",
                        sCVariable->mName, sCVariable->mArrayValue);
        }
        else
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT %s;\n",
                        sCVariable->mName);
        }

        ztpwBypassCCodeByLoc(aParam,
                             aCDeclaration->mStartPos - aCDeclaration->mIndent,
                             aCDeclaration->mStartPos);
        ztpwSendStringToOutFile(aParam, sStructBuf);
    }

    aParam->mCCodeStartLoc = aCDeclaration->mEndPos;

    STL_TRY(ztpwPrecompSourceLine(aParam,
                                  aCDeclaration->mLineNo,
                                  aParam->mInFileName,
                                  aParam->mOutFileName)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpdConvertInterval(ztpCParseParam  *aParam,
                              ztpCDeclaration *aCDeclaration,
                              ztpCVariable    *aCVariableListHead)
{
    stlChar            sStructBuf[ZTP_LINE_BUF_SIZE];
    ztpCVariable      *sCVariable;

    aParam->mCCodeEndLoc = aCDeclaration->mStartPos;
    ztpwBypassCCode(aParam);

    ztpwSendStringToOutFile(aParam, "/* ");
    ztpwBypassCCodeByLoc(aParam,
                         aCDeclaration->mStartPos,
                         aCDeclaration->mEndPos);
    ztpwSendStringToOutFile(aParam, " */");

    for(sCVariable = aCVariableListHead;
        sCVariable != NULL;
        sCVariable = sCVariable->mNext)
    {
        if( sCVariable->mIsArray == STL_TRUE )
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_INTERVAL_STRUCT %s[%s];\n",
                        sCVariable->mName, sCVariable->mArrayValue);
        }
        else
        {
            stlSnprintf(sStructBuf,
                        ZTP_LINE_BUF_SIZE,
                        "SQL_INTERVAL_STRUCT %s;\n",
                        sCVariable->mName);
        }

        ztpwBypassCCodeByLoc(aParam,
                             aCDeclaration->mStartPos - aCDeclaration->mIndent,
                             aCDeclaration->mStartPos);
        ztpwSendStringToOutFile(aParam, sStructBuf);
    }

    aParam->mCCodeStartLoc = aCDeclaration->mEndPos;

    STL_TRY(ztpwPrecompSourceLine(aParam,
                                  aCDeclaration->mLineNo,
                                  aParam->mInFileName,
                                  aParam->mOutFileName)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpdConvertStruct(ztpCParseParam  *aParam,
                            ztpCDeclaration *aCDeclaration,
                            ztpCVariable    *aCVariableListHead)
{
    return STL_SUCCESS;
}


/** @} */
