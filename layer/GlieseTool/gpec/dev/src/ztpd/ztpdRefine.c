/*******************************************************************************
 * ztpdRefine.c
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
#include <ztpdRefine.h>

/**
 * @file ztpdRefine.c
 * @brief Gliese Embedded SQL in C precompiler refine data type functions
 */

/**
 * @addtogroup ztpd
 * @{
 */

ztpdRefineDTFunc gRefineDTFunc[ZLPL_C_DATATYPE_MAX] =
{
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_UNKNOWN */
    ztpdRefineChar,              /* ZLPL_C_DATATYPE_SCHAR */
    ztpdRefineChar,              /* ZLPL_C_DATATYPE_UCHAR */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_SSHORT */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_USHORT */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_SINT */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_UINT */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_SLONG */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_ULONG */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_SLONGLONG */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_ULONGLONG */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_FLOAT */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_DOUBLE */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_CHAR */
    ztpdRefineVarchar,           /* ZLPL_C_DATATYPE_VARCHAR */
    ztpdRefineLongVarchar,       /* ZLPL_C_DATATYPE_LONGVARCHAR */
    ztpdRefineBinary,            /* ZLPL_C_DATATYPE_BINARY */
    ztpdRefineVarBinary,         /* ZLPL_C_DATATYPE_VARBINARY */
    ztpdRefineLongVarBinary,     /* ZLPL_C_DATATYPE_LONGVARBINARY */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_BOOLEAN */
    ztpdRefineNumber,            /* ZLPL_C_DATATYPE_NUMBER */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_DATE */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_TIME */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_TIMESTAMP */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_TIMETZ */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_TIMESTAMPTZ */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_INTERVAL_YEAR */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_INTERVAL_MONTH */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_INTERVAL_DAY */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_INTERVAL_HOUR */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_INTERVAL_MINUTE */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_INTERVAL_SECOND */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_INTERVAL_YEAR_TO_MONTH */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_HOUR */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_MINUTE */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_SECOND */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_MINUTE */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_SECOND */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_INTERVAL_MINUTE_TO_SECOND */
    ztpdRefineNativeType,        /* ZLPL_C_DATATYPE_SQL_CONTEXT */
    ztpdRefineStruct,            /* ZLPL_C_DATATYPE_Struct */
};

stlStatus ztpdRefineNativeType( ztpCVariable     *aCVariable,
                                ztpCDeclaration  *aCDeclaration )
{
    STL_TRY( aCVariable->mIsAvailHostVar == STL_TRUE );

    STL_TRY( aCVariable->mName != NULL );

    STL_TRY( aCVariable->mAddrDepth <= 1 );

    if( aCVariable->mArrayValue != NULL )
    {
        aCVariable->mIsArray = STL_TRUE;
    }
    else
    {
        aCVariable->mIsArray = STL_FALSE;
    }

    aCVariable->mIsAvailHostVar = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    aCVariable->mIsAvailHostVar = STL_FALSE;

    return STL_SUCCESS;
}

stlStatus ztpdRefineChar( ztpCVariable     *aCVariable,
                          ztpCDeclaration  *aCDeclaration )
{
    STL_TRY( aCVariable->mIsAvailHostVar == STL_TRUE );

    STL_TRY( aCVariable->mName != NULL );

    STL_TRY( aCVariable->mAddrDepth <= 2 );

    if( aCVariable->mArrayValue == NULL )
    {
        aCVariable->mArrayValue = aCDeclaration->mPrecision;
    }

    if( aCVariable->mArrayValue != NULL )
    {
        if( aCVariable->mCharLength == NULL )
        {
            aCVariable->mCharLength = aCVariable->mArrayValue;
            aCVariable->mArrayValue = NULL;
            aCVariable->mIsArray = STL_FALSE;
        }
        else
        {
            aCVariable->mIsArray = STL_TRUE;
        }
    }
    else
    {
        aCVariable->mIsArray = STL_FALSE;
    }

    aCVariable->mIsAvailHostVar = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    aCVariable->mIsAvailHostVar = STL_FALSE;

    return STL_FAILURE;
}

stlStatus ztpdRefineVarchar( ztpCVariable     *aCVariable,
                             ztpCDeclaration  *aCDeclaration )
{
    STL_TRY( aCVariable->mIsAvailHostVar == STL_TRUE );

    STL_TRY( aCVariable->mName != NULL );

    STL_TRY( aCVariable->mAddrDepth <= 2 );

    if( aCVariable->mArrayValue != NULL )
    {
        if( aCVariable->mCharLength == NULL )
        {
            aCVariable->mCharLength = aCVariable->mArrayValue;
            aCVariable->mArrayValue = NULL;
            aCVariable->mIsArray = STL_FALSE;
        }
        else
        {
            aCVariable->mIsArray = STL_TRUE;
        }
    }
    else
    {
        aCVariable->mIsArray = STL_FALSE;
    }

    aCVariable->mIsAvailHostVar = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    aCVariable->mIsAvailHostVar = STL_FALSE;

    return STL_FAILURE;
}

stlStatus ztpdRefineLongVarchar( ztpCVariable     *aCVariable,
                                 ztpCDeclaration  *aCDeclaration )
{
    STL_TRY( aCVariable->mIsAvailHostVar == STL_TRUE );

    STL_TRY( aCVariable->mName != NULL );

    STL_TRY( aCVariable->mAddrDepth <= 2 );

    if( aCVariable->mArrayValue != NULL )
    {
        if( aCVariable->mCharLength == NULL )
        {
            aCVariable->mCharLength = aCVariable->mArrayValue;
            aCVariable->mArrayValue = NULL;
            aCVariable->mIsArray = STL_FALSE;
        }
        else
        {
            aCVariable->mIsArray = STL_TRUE;
        }
    }
    else
    {
        aCVariable->mIsArray = STL_FALSE;
    }

    aCVariable->mIsAvailHostVar = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    aCVariable->mIsAvailHostVar = STL_FALSE;

    return STL_FAILURE;
}

stlStatus ztpdRefineBinary( ztpCVariable     *aCVariable,
                            ztpCDeclaration  *aCDeclaration )
{
    STL_TRY( aCVariable->mIsAvailHostVar == STL_TRUE );

    STL_TRY( aCVariable->mName != NULL );

    STL_TRY( aCVariable->mAddrDepth <= 2 );

    if( aCVariable->mArrayValue != NULL )
    {
        if( aCVariable->mCharLength == NULL )
        {
            aCVariable->mCharLength = aCVariable->mArrayValue;
            aCVariable->mArrayValue = NULL;
            aCVariable->mIsArray = STL_FALSE;
        }
        else
        {
            aCVariable->mIsArray = STL_TRUE;
        }
    }
    else
    {
        aCVariable->mIsArray = STL_FALSE;
    }

    aCVariable->mIsAvailHostVar = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    aCVariable->mIsAvailHostVar = STL_FALSE;

    return STL_FAILURE;
}

stlStatus ztpdRefineVarBinary( ztpCVariable     *aCVariable,
                               ztpCDeclaration  *aCDeclaration )
{
    STL_TRY( aCVariable->mIsAvailHostVar == STL_TRUE );

    STL_TRY( aCVariable->mName != NULL );

    STL_TRY( aCVariable->mAddrDepth <= 2 );

    if( aCVariable->mArrayValue != NULL )
    {
        if( aCVariable->mCharLength == NULL )
        {
            aCVariable->mCharLength = aCVariable->mArrayValue;
            aCVariable->mArrayValue = NULL;
            aCVariable->mIsArray = STL_FALSE;
        }
        else
        {
            aCVariable->mIsArray = STL_TRUE;
        }
    }
    else
    {
        aCVariable->mIsArray = STL_FALSE;
    }

    aCVariable->mIsAvailHostVar = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    aCVariable->mIsAvailHostVar = STL_FALSE;

    return STL_FAILURE;
}

stlStatus ztpdRefineLongVarBinary( ztpCVariable     *aCVariable,
                                   ztpCDeclaration  *aCDeclaration )
{
    STL_TRY( aCVariable->mIsAvailHostVar == STL_TRUE );

    STL_TRY( aCVariable->mName != NULL );

    STL_TRY( aCVariable->mAddrDepth <= 2 );

    if( aCVariable->mArrayValue != NULL )
    {
        if( aCVariable->mCharLength == NULL )
        {
            aCVariable->mCharLength = aCVariable->mArrayValue;
            aCVariable->mArrayValue = NULL;
            aCVariable->mIsArray = STL_FALSE;
        }
        else
        {
            aCVariable->mIsArray = STL_TRUE;
        }
    }
    else
    {
        aCVariable->mIsArray = STL_FALSE;
    }

    aCVariable->mIsAvailHostVar = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    aCVariable->mIsAvailHostVar = STL_FALSE;

    return STL_FAILURE;
}

stlStatus ztpdRefineNumber( ztpCVariable     *aCVariable,
                            ztpCDeclaration  *aCDeclaration )
{
    STL_TRY( aCVariable->mIsAvailHostVar == STL_TRUE );

    STL_TRY( aCVariable->mName != NULL );

    STL_TRY( aCVariable->mAddrDepth <= 1 );

    aCVariable->mPrecision = aCDeclaration->mPrecision;
    aCVariable->mScale     = aCDeclaration->mScale;

    if( aCVariable->mArrayValue != NULL )
    {
        aCVariable->mIsArray = STL_TRUE;
    }
    else
    {
        aCVariable->mIsArray = STL_FALSE;
    }

    aCVariable->mIsAvailHostVar = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    aCVariable->mIsAvailHostVar = STL_FALSE;

    return STL_SUCCESS;
}

stlStatus ztpdRefineStruct( ztpCVariable     *aCVariable,
                            ztpCDeclaration  *aCDeclaration )
{
    STL_TRY( aCDeclaration != NULL );
    STL_TRY( aCDeclaration->mCVariable != NULL );

    STL_TRY( aCVariable->mIsAvailHostVar == STL_TRUE );

    STL_TRY( aCVariable->mName != NULL );

    STL_TRY( aCVariable->mAddrDepth <= 1 );

    if( aCVariable->mArrayValue != NULL )
    {
        aCVariable->mIsArray = STL_TRUE;
    }
    else
    {
        aCVariable->mIsArray = STL_FALSE;
    }

    aCVariable->mStructMember = aCDeclaration->mCVariable;

    aCVariable->mIsAvailHostVar = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    aCVariable->mIsAvailHostVar = STL_FALSE;

    return STL_SUCCESS;
}

/** @} */
