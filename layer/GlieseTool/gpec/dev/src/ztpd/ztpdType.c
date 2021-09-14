/*******************************************************************************
 * ztpdType.c
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

#include <ztpDef.h>
#include <ztpMisc.h>
#include <ztpdType.h>

/**
 * @file ztpdType.c
 * @brief Gliese Embedded SQL in C precompiler miscellaneous functions
 */

/**
 * @addtogroup ztpd
 * @{
 */

/* zlplCDataType 과 mapping을 맞워야 함 */
static stlInt32 gPrecision[ZLPL_C_DATATYPE_MAX] =
{
    0,                             /* ZLPL_C_DATATYPE_UNKNOWN */
    0,                             /* ZLPL_C_DATATYPE_SCHAR */
    0,                             /* ZLPL_C_DATATYPE_UCHAR */
    0,                             /* ZLPL_C_DATATYPE_SSHORT */
    0,                             /* ZLPL_C_DATATYPE_USHORT */
    0,                             /* ZLPL_C_DATATYPE_SINT */
    0,                             /* ZLPL_C_DATATYPE_UINT */
    0,                             /* ZLPL_C_DATATYPE_SLONG */
    0,                             /* ZLPL_C_DATATYPE_ULONG */
    0,                             /* ZLPL_C_DATATYPE_SLONGLONG */
    0,                             /* ZLPL_C_DATATYPE_ULONGLONG */
    0,                             /* ZLPL_C_DATATYPE_FLOAT */
    0,                             /* ZLPL_C_DATATYPE_DOUBLE */
    0,                             /* ZLPL_C_DATATYPE_CHAR */
    0,                             /* ZLPL_C_DATATYPE_VARCHAR */
    0,                             /* ZLPL_C_DATATYPE_LONGVARCHAR */
    0,                             /* ZLPL_C_DATATYPE_BINARY */
    0,                             /* ZLPL_C_DATATYPE_VARBINARY */
    0,                             /* ZLPL_C_DATATYPE_LONGVARBINARY */
    0,                             /* ZLPL_C_DATATYPE_BOOLEAN */
    DTL_NUMERIC_DEFAULT_PRECISION, /* ZLPL_C_DATATYPE_NUMBER */
    0,                             /* ZLPL_C_DATATYPE_DATE */
    0,                             /* ZLPL_C_DATATYPE_TIME */
    0,                             /* ZLPL_C_DATATYPE_TIMESTAMP */
    0,                             /* ZLPL_C_DATATYPE_TIMETZ */
    0,                             /* ZLPL_C_DATATYPE_TIMESTAMPTZ */
    0,                             /* ZLPL_C_DATATYPE_INTERVAL_YEAR */
    0,                             /* ZLPL_C_DATATYPE_INTERVAL_MONTH */
    0,                             /* ZLPL_C_DATATYPE_INTERVAL_DAY */
    0,                             /* ZLPL_C_DATATYPE_INTERVAL_HOUR */
    0,                             /* ZLPL_C_DATATYPE_INTERVAL_MINUTE */
    0,                             /* ZLPL_C_DATATYPE_INTERVAL_SECOND */
    0,                             /* ZLPL_C_DATATYPE_INTERVAL_YEAR_TO_MONTH */
    0,                             /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_HOUR */
    0,                             /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_MINUTE */
    0,                             /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_SECOND */
    0,                             /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_MINUTE */
    0,                             /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_SECOND */
    0,                             /* ZLPL_C_DATATYPE_INTERVAL_MINUTE_TO_SECOND */
    0,                             /* ZLPL_C_DATATYPE_SQL_CONTEXT */
    0,                             /* ZLPL_C_DATATYPE_STRUCT */
};

stlInt32 ztpdGetPrecision( zlplCDataType aDataType )
{
    return gPrecision[aDataType];
}

/* zlplCDataType 과 mapping을 맞워야 함 */
static stlInt32 gScale[ZLPL_C_DATATYPE_MAX] =
{
    0,                                            /* ZLPL_C_DATATYPE_UNKNOWN */
    0,                                            /* ZLPL_C_DATATYPE_SCHAR */
    0,                                            /* ZLPL_C_DATATYPE_UCHAR */
    0,                                            /* ZLPL_C_DATATYPE_SSHORT */
    0,                                            /* ZLPL_C_DATATYPE_USHORT */
    0,                                            /* ZLPL_C_DATATYPE_SINT */
    0,                                            /* ZLPL_C_DATATYPE_UINT */
    0,                                            /* ZLPL_C_DATATYPE_SLONG */
    0,                                            /* ZLPL_C_DATATYPE_ULONG */
    0,                                            /* ZLPL_C_DATATYPE_SLONGLONG */
    0,                                            /* ZLPL_C_DATATYPE_ULONGLONG */
    0,                                            /* ZLPL_C_DATATYPE_FLOAT */
    0,                                            /* ZLPL_C_DATATYPE_DOUBLE */
    0,                                            /* ZLPL_C_DATATYPE_CHAR */
    0,                                            /* ZLPL_C_DATATYPE_VARCHAR */
    0,                                            /* ZLPL_C_DATATYPE_LONGVARCHAR */
    0,                                            /* ZLPL_C_DATATYPE_BINARY */
    0,                                            /* ZLPL_C_DATATYPE_VARBINARY */
    0,                                            /* ZLPL_C_DATATYPE_LONGVARBINARY */
    0,                                            /* ZLPL_C_DATATYPE_BOOLEAN */
    DTL_NUMERIC_DEFAULT_SCALE,                    /* ZLPL_C_DATATYPE_NUMBER */
    0,                                            /* ZLPL_C_DATATYPE_DATE */
    0,                                            /* ZLPL_C_DATATYPE_TIME */
    DTL_TIMESTAMP_MAX_PRECISION,                  /* ZLPL_C_DATATYPE_TIMESTAMP */
    DTL_TIMETZ_MAX_PRECISION,                     /* ZLPL_C_DATATYPE_TIMETZ */
    DTL_TIMESTAMPTZ_MAX_PRECISION,                /* ZLPL_C_DATATYPE_TIMESTAMPTZ */
    0,                                            /* ZLPL_C_DATATYPE_INTERVAL_YEAR */
    0,                                            /* ZLPL_C_DATATYPE_INTERVAL_MONTH */
    0,                                            /* ZLPL_C_DATATYPE_INTERVAL_DAY */
    0,                                            /* ZLPL_C_DATATYPE_INTERVAL_HOUR */
    0,                                            /* ZLPL_C_DATATYPE_INTERVAL_MINUTE */
    DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION, /* ZLPL_C_DATATYPE_INTERVAL_SECOND */
    0,                                            /* ZLPL_C_DATATYPE_INTERVAL_YEAR_TO_MONTH */
    0,                                /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_HOUR */
    0,                                /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_MINUTE */
    DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION, /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_SECOND */
    0,                                /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_MINUTE */
    DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION, /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_SECOND */
    DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION, /* ZLPL_C_DATATYPE_INTERVAL_MINUTE_TO_SECOND */
    0,                                            /* ZLPL_C_DATATYPE_SQL_CONTEXT */
    0,                                            /* ZLPL_C_DATATYPE_STRUCT */
};

stlInt32 ztpdGetScale( zlplCDataType aDataType )
{
    return gScale[aDataType];
}

#define ZTPD_NONE      (-1)
#define ZTPD_SIGNED    (0)
#define ZTPD_UNSIGNED  (1)

stlStatus ztpdGetCDataType( ztpCParseParam        *aParam,
                            ztpTypeSpecifierList  *aTypeSpecifierList,
                            zlplCDataType         *aCDataType,
                            stlChar              **aPrecision,
                            stlChar              **aScale,
                            ztpCVariable         **aCVariable )
{
    ztpTypeSpecifierNode  *sSignTypeSpecifier = NULL;
    ztpTypeSpecifierNode  *sPrevTypeSpecifier = NULL;
    ztpTypeSpecifierNode  *sCurrTypeSpecifier;
    ztpTypeSpecifierList  *sNode;
    ztpCVariable          *sCVariable;
    zlplCDataType          sDataType = ZLPL_C_DATATYPE_UNKNOWN;
    stlInt32               sSignedFlag = ZTPD_NONE;
    stlInt32               sLongFlag = 0;
    stlInt32               sIntCount = 0;

    *aPrecision = NULL;
    *aScale     = NULL;

    /*
     * sSignedFlag: signed, unsigned 키워드의 등장 여부
     *  -1 : 등장하지 않음
     *   0 : signed 등장
     *   1 : unsigned 등장
     */

    /*
     * 우선 ztpCVariable을 찾는다.(구조체 정의)
     */
    *aCVariable = NULL;
    for( sNode = aTypeSpecifierList; sNode != NULL; sNode = sNode->mNext )
    {
        sCurrTypeSpecifier = sNode->mTypeSpecifierNode;
        if( sCurrTypeSpecifier->mCVariable != NULL )
        {
            if( sCurrTypeSpecifier->mCVariable->mDataType == ZLPL_C_DATATYPE_STRUCT )
            {
                STL_DASSERT( sCurrTypeSpecifier->mCVariable->mStructMember != NULL );
                sDataType = sCurrTypeSpecifier->mCVariable->mDataType;
                *aCVariable = sCurrTypeSpecifier->mCVariable->mStructMember;

                STL_THROW( EXIT_FUNC );
            }
        }
    }

    /*
     * 구조체 정의가 없다면, signed, unsigned keyword를 먼저 찾는다.
     */
    for( sNode = aTypeSpecifierList; sNode != NULL; sNode = sNode->mNext )
    {
        sCurrTypeSpecifier = sNode->mTypeSpecifierNode;

        switch( sCurrTypeSpecifier->mTypeSpecifier )
        {
            case ZTP_SPECIFIER_SIGNED:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_DUPLICATE_KEYWORD );
                sSignedFlag = ZTPD_SIGNED;
                sSignTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_UNSIGNED:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_DUPLICATE_KEYWORD );
                sSignedFlag = ZTPD_UNSIGNED;
                sSignTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_INT:
                STL_TRY_THROW( sIntCount == 0, ERR_DUPLICATE_KEYWORD );
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                sIntCount ++;
                break;
            default:
                break;
        }
    }

    /*
     * Default data type : int
     */
    if( sSignedFlag == ZTPD_UNSIGNED )
    {
        sDataType = ZLPL_C_DATATYPE_UINT;
    }
    else
    {
        sDataType = ZLPL_C_DATATYPE_SINT;
    }

    for( sNode = aTypeSpecifierList; sNode != NULL; sNode = sNode->mNext )
    {
        sCurrTypeSpecifier = sNode->mTypeSpecifierNode;

        switch( sCurrTypeSpecifier->mTypeSpecifier )
        {
            case ZTP_SPECIFIER_TYPEDEF:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                STL_DASSERT( sCurrTypeSpecifier->mCVariable != NULL );
                sCVariable = sCurrTypeSpecifier->mCVariable;
                sDataType = sCVariable->mDataType;
                *aPrecision = sCVariable->mCharLength;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_CHAR:
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                if( sSignedFlag == ZTPD_UNSIGNED )
                {
                    sDataType = ZLPL_C_DATATYPE_UCHAR;
                }
                else
                {
                    sDataType = ZLPL_C_DATATYPE_SCHAR;
                }
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_SHORT:
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                if( sSignedFlag == ZTPD_UNSIGNED )
                {
                    sDataType = ZLPL_C_DATATYPE_USHORT;
                }
                else
                {
                    sDataType = ZLPL_C_DATATYPE_SSHORT;
                }
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_LONG:
                STL_TRY_THROW( sLongFlag < 2, ERR_TRIPLE_LONG );
                if( sPrevTypeSpecifier != NULL )
                {
                    STL_TRY_THROW( sPrevTypeSpecifier->mTypeSpecifier == ZTP_SPECIFIER_LONG,
                                   ERR_BOTH_KEYWORD );
                }

                if( sLongFlag == 0 )
                {
                    if( sSignedFlag == ZTPD_UNSIGNED )
                    {
                        sDataType = ZLPL_C_DATATYPE_ULONG;
                    }
                    else
                    {
                        sDataType = ZLPL_C_DATATYPE_SLONG;
                    }
                    sLongFlag = 1;
                    sPrevTypeSpecifier = sCurrTypeSpecifier;
                }
                else
                {
                    if( sSignedFlag == ZTPD_UNSIGNED )
                    {
                        sDataType = ZLPL_C_DATATYPE_ULONGLONG;
                    }
                    else
                    {
                        sDataType = ZLPL_C_DATATYPE_SLONGLONG;
                    }
                    sLongFlag = 2;
                }
                break;
            case ZTP_SPECIFIER_FLOAT:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_FLOAT;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_DOUBLE:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_DOUBLE;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_SQLCONTEXT:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_SQL_CONTEXT;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_VARCHAR:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_VARCHAR;
                break;
            case ZTP_SPECIFIER_LONGVARCHAR:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_LONGVARCHAR;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_BINARY:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_BINARY;
                break;
            case ZTP_SPECIFIER_VARBINARY:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_VARBINARY;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_LONGVARBINARY:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_LONGVARBINARY;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_BIT:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_BOOLEAN;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_BOOLEAN:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_BOOLEAN;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_NUMBER:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_NUMBER;
                *aPrecision = sCurrTypeSpecifier->mPrecision;
                *aScale     = sCurrTypeSpecifier->mScale;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_DATE:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_DATE;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_TIME:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_TIME;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_TIMETZ:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_TIMETZ;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_TIMESTAMP:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_TIMESTAMP;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_TIMESTAMPTZ:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_TIMESTAMPTZ;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_INTERVAL_YEAR:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_INTERVAL_YEAR;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_INTERVAL_MONTH:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_INTERVAL_MONTH;
                sPrevTypeSpecifier = sCurrTypeSpecifier;

                break;
            case ZTP_SPECIFIER_INTERVAL_DAY:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_INTERVAL_DAY;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_INTERVAL_HOUR:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_INTERVAL_HOUR;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_INTERVAL_MINUTE:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_INTERVAL_MINUTE;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_INTERVAL_SECOND:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_INTERVAL_SECOND;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_INTERVAL_YEAR_TO_MONTH:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_INTERVAL_YEAR_TO_MONTH;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_INTERVAL_DAY_TO_HOUR:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_INTERVAL_DAY_TO_HOUR;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_INTERVAL_DAY_TO_MINUTE:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_INTERVAL_DAY_TO_MINUTE;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_INTERVAL_DAY_TO_SECOND:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_INTERVAL_DAY_TO_SECOND;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_INTERVAL_HOUR_TO_MINUTE:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_MINUTE;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_INTERVAL_HOUR_TO_SECOND:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_SECOND;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            case ZTP_SPECIFIER_INTERVAL_MINUTE_TO_SECOND:
                STL_TRY_THROW( sSignedFlag == ZTPD_NONE, ERR_SIGNED_KEYWORD );
                STL_TRY_THROW( sPrevTypeSpecifier == NULL, ERR_BOTH_KEYWORD );
                sDataType = ZLPL_C_DATATYPE_INTERVAL_MINUTE_TO_SECOND;
                sPrevTypeSpecifier = sCurrTypeSpecifier;
                break;
            default:
                break;
        }
    }

    STL_RAMP( EXIT_FUNC );

    *aCDataType = sDataType;

    return STL_SUCCESS;

    STL_CATCH( ERR_DUPLICATE_KEYWORD )
    {
        stlChar   sErrorToken[128];

        stlStrncpy( sErrorToken,
                    &aParam->mBuffer[sCurrTypeSpecifier->mPosition],
                    sCurrTypeSpecifier->mLength + 1 );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_DUPLICATE_KEYWORD,
                      ztpMakeErrPosString( aParam->mBuffer,
                                           sCurrTypeSpecifier->mPosition ),
                      aParam->mErrorStack,
                      sErrorToken );

        /* error: duplicate %s */
        /*
        if( sSignedFlag == ZTPD_SIGNED )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          ZTP_ERRCODE_DUPLICATE_KEYWORD,
                          ztpMakeErrPosString( aParam->mBuffer,
                                               sCurrTypeSpecifier->mPosition ),
                          aParam->mErrorStack,
                          "signed" );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          ZTP_ERRCODE_DUPLICATE_KEYWORD,
                          ztpMakeErrPosString( aParam->mBuffer,
                                               sCurrTypeSpecifier->mPosition ),
                          aParam->mErrorStack,
                          "unsigned" );
        }
        */
    }
    STL_CATCH( ERR_TRIPLE_LONG )
    {
        /* error: both %s and %s in declaration specifiers */
        stlChar   sErrorToken[128];

        stlStrncpy( sErrorToken,
                    &aParam->mBuffer[sCurrTypeSpecifier->mPosition],
                    sCurrTypeSpecifier->mLength + 1 );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_BOTH_KEYWORD,
                      ztpMakeErrPosString( aParam->mBuffer,
                                           sCurrTypeSpecifier->mPosition ),
                      aParam->mErrorStack,
                      "long long",
                      sErrorToken );
        /*
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_BOTH_KEYWORD,
                      ztpMakeErrPosString( aParam->mBuffer,
                                           sCurrTypeSpecifier->mPosition ),
                      aParam->mErrorStack,
                      "long long",
                      "long");
        */
    }
    STL_CATCH( ERR_SIGNED_KEYWORD )
    {
        /* error: both %s and %s in declaration specifiers */
        stlChar   sErrorToken1[128];
        stlChar   sErrorToken2[128];

        stlStrncpy( sErrorToken1,
                    &aParam->mBuffer[sSignTypeSpecifier->mPosition],
                    sSignTypeSpecifier->mLength + 1 );
        stlStrncpy( sErrorToken2,
                    &aParam->mBuffer[sCurrTypeSpecifier->mPosition],
                    sCurrTypeSpecifier->mLength + 1 );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_BOTH_KEYWORD,
                      ztpMakeErrPosString( aParam->mBuffer,
                                           sCurrTypeSpecifier->mPosition ),
                      aParam->mErrorStack,
                      sErrorToken1,
                      sErrorToken2 );
    }
    STL_CATCH( ERR_BOTH_KEYWORD )
    {
        stlChar   sErrorToken1[128];
        stlChar   sErrorToken2[128];

        stlStrncpy( sErrorToken1,
                    &aParam->mBuffer[sPrevTypeSpecifier->mPosition],
                    sPrevTypeSpecifier->mLength + 1 );
        stlStrncpy( sErrorToken2,
                    &aParam->mBuffer[sCurrTypeSpecifier->mPosition],
                    sCurrTypeSpecifier->mLength + 1 );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_BOTH_KEYWORD,
                      ztpMakeErrPosString( aParam->mBuffer,
                                           sCurrTypeSpecifier->mPosition ),
                      aParam->mErrorStack,
                      sErrorToken1,
                      sErrorToken2 );

        /* error: both %s and %s in declaration specifiers */
        /*
        if( sSignedFlag == ZTPD_SIGNED )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          ZTP_ERRCODE_BOTH_KEYWORD,
                          ztpMakeErrPosString( aParam->mBuffer,
                                               sCurrTypeSpecifier->mPosition ),
                          aParam->mErrorStack,
                          "signed",
                          sCurrTypeStr );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          ZTP_ERRCODE_BOTH_KEYWORD,
                          ztpMakeErrPosString( aParam->mBuffer,
                                               sCurrTypeSpecifier->mPosition ),
                          aParam->mErrorStack,
                          "unsigned",
                          sCurrTypeStr );
        }
        */
    }
    STL_FINISH;

    *aCDataType = ZLPL_C_DATATYPE_UNKNOWN;
    *aCVariable = NULL;

    aParam->mErrorStatus = STL_FAILURE;

    return STL_FAILURE;
}

/**
 * @brief Host Variable 의 C type 이 String 계열인지 여부
 * @param[in]  aCVariable  Host Variable
 * @return
 * - TRUE : String 계열 C type
 * - FALSE : Non-String 계열 C type
 * @remarks
 */
stlBool   ztpdIsHostStringType( ztpCVariable   * aCVariable )
{
    zlplCDataType sType = ZLPL_C_DATATYPE_UNKNOWN;

    /**
     * Parameter Valiadation
     */

    STL_DASSERT( aCVariable != NULL );

    /**
     * Type 정보 검사
     */

    sType = aCVariable->mDataType;

    return ZLPL_IS_STRING_TYPE( sType );
}




/**
 * @brief Host Variable 의 C type 이 Integer 계열인지 여부
 * @param[in]  aCVariable  Host Variable
 * @return
 * - TRUE : Integer 계열 C type
 * - FALSE : Non-Integer 계열 C type
 * @remarks
 */
stlBool   ztpdIsHostIntegerType( ztpCVariable   * aCVariable )
{
    zlplCDataType sType = ZLPL_C_DATATYPE_UNKNOWN;

    /**
     * Parameter Valiadation
     */

    STL_DASSERT( aCVariable != NULL );

    /**
     * Type 정보 검사
     */

    sType = aCVariable->mDataType;

    return ZLPL_IS_INTEGER_TYPE( sType );
}

/** @} */
