/*******************************************************************************
 * ztpdRefineHostVar.c
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
#include <ztpdRefineHostVar.h>

/**
 * @file ztpdRefineHostVar.c
 * @brief Gliese Embedded SQL in C precompiler refine data type functions
 */

/**
 * @addtogroup ztpd
 * @{
 */

/***********************************************/
/** Refine Host Variable ***********************/
/***********************************************/
ztpdRefineDTHostVarFunc gRefineDTHostVarFunc[ZLPL_C_DATATYPE_MAX] =
{
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_UNKNOWN */
    ztpdRefineCharHostVar,              /* ZLPL_C_DATATYPE_SCHAR */
    ztpdRefineCharHostVar,              /* ZLPL_C_DATATYPE_UCHAR */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_SSHORT */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_USHORT */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_SINT */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_UINT */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_SLONG */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_ULONG */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_SLONGLONG */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_ULONGLONG */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_FLOAT */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_DOUBLE */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_CHAR */
    ztpdRefineVarcharHostVar,           /* ZLPL_C_DATATYPE_VARCHAR */
    ztpdRefineVarcharHostVar,           /* ZLPL_C_DATATYPE_LONGVARCHAR */
    ztpdRefineBinaryHostVar,            /* ZLPL_C_DATATYPE_BINARY */
    ztpdRefineVarcharHostVar,           /* ZLPL_C_DATATYPE_VARBINARY */
    ztpdRefineVarcharHostVar,           /* ZLPL_C_DATATYPE_LONGVARBINARY */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_BOOLEAN */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_NUMBER */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_DATE */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_TIME */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_TIMESTAMP */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_TIMETZ */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_TIMESTAMPTZ */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_INTERVAL_YEAR */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_INTERVAL_MONTH */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_INTERVAL_DAY */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_INTERVAL_HOUR */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_INTERVAL_MINUTE */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_INTERVAL_SECOND */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_INTERVAL_YEAR_TO_MONTH */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_HOUR */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_MINUTE */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_SECOND */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_MINUTE */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_SECOND */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_INTERVAL_MINUTE_TO_SECOND */
    ztpdRefineNativeTypeHostVar,        /* ZLPL_C_DATATYPE_SQL_CONTEXT */
    ztpdRefineStructHostVar,            /* ZLPL_C_DATATYPE_STRUCT */
};

stlStatus ztpdRefineNativeTypeHostVar(ztpCVariable *aCVariable)
{
    STL_TRY( aCVariable->mIsAvailHostVar == STL_TRUE );

    STL_TRY( aCVariable->mAddrDepth <= 1 );

    aCVariable->mIsAvailHostVar = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    aCVariable->mIsAvailHostVar = STL_FALSE;

    return STL_SUCCESS;
}

stlStatus ztpdRefineCharHostVar(ztpCVariable *aCVariable)
{
    STL_TRY( aCVariable->mIsAvailHostVar == STL_TRUE );

    STL_TRY( aCVariable->mAddrDepth <= 2 );

    if( aCVariable->mCharLength != NULL )
    {
        /*
         * char 계열은 string 표현식으로 []를 쓰고 있으므로,
         * [] 1개를 AddrDepth에서 감소시킨다.
         */
        aCVariable->mAddrDepth --;

        aCVariable->mDataType = ZLPL_C_DATATYPE_CHAR;

    }

    if( aCVariable->mAddrDepth == 1 )
    {
        /*
         * char * 타입의 변수를 사용할 때는,
         * 이를 string으로 봐야 한다.
         */
        aCVariable->mDataType = ZLPL_C_DATATYPE_CHAR;
    }

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

    return STL_FAILURE;
}

stlStatus ztpdRefineVarcharHostVar(ztpCVariable *aCVariable)
{
    STL_TRY( aCVariable->mIsAvailHostVar == STL_TRUE );

    STL_TRY( aCVariable->mAddrDepth <= 2 );

    if( aCVariable->mCharLength != NULL )
    {
        /*
         * char 계열은 string 표현식으로 []를 쓰고 있으므로,
         * [] 1개를 AddrDepth에서 감소시킨다.
         */
        aCVariable->mAddrDepth --;
    }

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

    return STL_FAILURE;
}

stlStatus ztpdRefineBinaryHostVar(ztpCVariable *aCVariable)
{
    STL_TRY( aCVariable->mIsAvailHostVar == STL_TRUE );

    STL_TRY( aCVariable->mAddrDepth <= 2 );

    if( aCVariable->mCharLength != NULL )
    {
        /*
         * char 계열은 string 표현식으로 []를 쓰고 있으므로,
         * [] 1개를 AddrDepth에서 감소시킨다.
         */
        aCVariable->mAddrDepth --;

        aCVariable->mDataType = ZLPL_C_DATATYPE_BINARY;

    }

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

    return STL_FAILURE;
}


stlStatus ztpdRefineStructHostVar(ztpCVariable *aCVariable)
{
    STL_TRY( aCVariable->mIsAvailHostVar == STL_TRUE );

    aCVariable->mIsAvailHostVar = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    aCVariable->mIsAvailHostVar = STL_FALSE;

    return STL_FAILURE;
}


/** @} */
