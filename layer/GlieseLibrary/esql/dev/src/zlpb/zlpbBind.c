/*******************************************************************************
 * zlpbBind.c
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

#include <zlpeSqlca.h>
#include <zlpbBind.h>

/**
 * @file zlpbBind.c
 * @brief Gliese Embedded SQL in C precompiler bind variable
 */

/**
 * @addtogroup zlpbBind
 * @{
 */

/**
 * @brief Internal
 */

/* zlplParamIOType 과 mapping을 맞춰야 함 */
static SQLSMALLINT gParamIOType[ZLPL_PARAM_IO_TYPE_MAX] =
{
    SQL_PARAM_INPUT,         /* ZLPL_PARAM_IO_TYPE_IN */
    SQL_PARAM_OUTPUT,        /* ZLPL_PARAM_IO_TYPE_OUT */
    SQL_PARAM_INPUT_OUTPUT,  /* ZLPL_PARAM_IO_TYPE_INOUT */
};

/* zlplCDataType 과 mapping을 맞워야 함 */
static SQLSMALLINT gInCDataType[ZLPL_C_DATATYPE_MAX] =
{
    SQL_C_DEFAULT,                      /* ZLPL_C_DATATYPE_UNKNOWN */
    SQL_C_CHAR,                         /* ZLPL_C_DATATYPE_SCHAR */
    SQL_C_CHAR,                         /* ZLPL_C_DATATYPE_UCHAR */
    SQL_C_SSHORT,                       /* ZLPL_C_DATATYPE_SSHORT */
    SQL_C_USHORT,                       /* ZLPL_C_DATATYPE_USHORT */
    SQL_C_SLONG,                        /* ZLPL_C_DATATYPE_SINT */
    SQL_C_ULONG,                        /* ZLPL_C_DATATYPE_UINT */
    SQL_C_SBIGINT,                      /* ZLPL_C_DATATYPE_SLONG */
    SQL_C_UBIGINT,                      /* ZLPL_C_DATATYPE_ULONG */
    SQL_C_SBIGINT,                      /* ZLPL_C_DATATYPE_SLONGLONG */
    SQL_C_UBIGINT,                      /* ZLPL_C_DATATYPE_ULONGLONG */
    SQL_C_FLOAT,                        /* ZLPL_C_DATATYPE_FLOAT */
    SQL_C_DOUBLE,                       /* ZLPL_C_DATATYPE_DOUBLE */
    SQL_C_CHAR,                         /* ZLPL_C_DATATYPE_CHAR */
    SQL_C_CHAR,                         /* ZLPL_C_DATATYPE_VARCHAR */
    SQL_C_LONGVARCHAR,                  /* ZLPL_C_DATATYPE_LONGVARCHAR */
    SQL_C_BINARY,                       /* ZLPL_C_DATATYPE_BINARY */
    SQL_C_BINARY,                       /* ZLPL_C_DATATYPE_VARBINARY */
    SQL_C_LONGVARBINARY,                /* ZLPL_C_DATATYPE_LONGVARBINARY */
    SQL_C_BIT,                          /* ZLPL_C_DATATYPE_BOOLEAN */
    SQL_C_NUMERIC,                      /* ZLPL_C_DATATYPE_NUMBER */
    SQL_C_TYPE_TIMESTAMP,               /* ZLPL_C_DATATYPE_DATE */
    SQL_C_TYPE_TIME,                    /* ZLPL_C_DATATYPE_TIME */
    SQL_C_TYPE_TIMESTAMP,               /* ZLPL_C_DATATYPE_TIMESTAMP */
    SQL_C_TYPE_TIME_WITH_TIMEZONE,      /* ZLPL_C_DATATYPE_TIMETZ */
    SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE, /* ZLPL_C_DATATYPE_TIMESTAMPTZ */
    SQL_C_INTERVAL_YEAR,                /* ZLPL_C_DATATYPE_INTERVAL_YEAR */
    SQL_C_INTERVAL_MONTH,               /* ZLPL_C_DATATYPE_INTERVAL_MONTH */
    SQL_C_INTERVAL_DAY,                 /* ZLPL_C_DATATYPE_INTERVAL_DAY */
    SQL_C_INTERVAL_HOUR,                /* ZLPL_C_DATATYPE_INTERVAL_HOUR */
    SQL_C_INTERVAL_MINUTE,              /* ZLPL_C_DATATYPE_INTERVAL_MINUTE */
    SQL_C_INTERVAL_SECOND,              /* ZLPL_C_DATATYPE_INTERVAL_SECOND */
    SQL_C_INTERVAL_YEAR_TO_MONTH,       /* ZLPL_C_DATATYPE_INTERVAL_YEAR_TO_MONTH */
    SQL_C_INTERVAL_DAY_TO_HOUR,         /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_HOUR */
    SQL_C_INTERVAL_DAY_TO_MINUTE,       /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_MINUTE */
    SQL_C_INTERVAL_DAY_TO_SECOND,       /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_SECOND */
    SQL_C_INTERVAL_HOUR_TO_MINUTE,      /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_MINUTE */
    SQL_C_INTERVAL_HOUR_TO_SECOND,      /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_SECOND */
    SQL_C_INTERVAL_MINUTE_TO_SECOND,    /* ZLPL_C_DATATYPE_INTERVAL_MINUTE_TO_SECOND */
    SQL_C_DEFAULT,                      /* ZLPL_C_DATATYPE_SQL_CONTEXT */
    SQL_C_DEFAULT,                      /* ZLPL_C_DATATYPE_STRUCT */
};

/* zlplCDataType 과 mapping을 맞워야 함 */
static SQLSMALLINT gOutCDataType[ZLPL_C_DATATYPE_MAX] =
{
    SQL_C_DEFAULT,                      /* ZLPL_C_DATATYPE_UNKNOWN */
    SQL_C_BINARY,                       /* ZLPL_C_DATATYPE_SCHAR */
    SQL_C_BINARY,                       /* ZLPL_C_DATATYPE_UCHAR */
    SQL_C_SSHORT,                       /* ZLPL_C_DATATYPE_SSHORT */
    SQL_C_USHORT,                       /* ZLPL_C_DATATYPE_USHORT */
    SQL_C_SLONG,                        /* ZLPL_C_DATATYPE_SINT */
    SQL_C_ULONG,                        /* ZLPL_C_DATATYPE_UINT */
    SQL_C_SBIGINT,                      /* ZLPL_C_DATATYPE_SLONG */
    SQL_C_UBIGINT,                      /* ZLPL_C_DATATYPE_ULONG */
    SQL_C_SBIGINT,                      /* ZLPL_C_DATATYPE_SLONGLONG */
    SQL_C_UBIGINT,                      /* ZLPL_C_DATATYPE_ULONGLONG */
    SQL_C_FLOAT,                        /* ZLPL_C_DATATYPE_FLOAT */
    SQL_C_DOUBLE,                       /* ZLPL_C_DATATYPE_DOUBLE */
    SQL_C_CHAR,                         /* ZLPL_C_DATATYPE_CHAR */
    SQL_C_CHAR,                         /* ZLPL_C_DATATYPE_VARCHAR */
    SQL_C_LONGVARCHAR,                  /* ZLPL_C_DATATYPE_LONGVARCHAR */
    SQL_C_BINARY,                       /* ZLPL_C_DATATYPE_BINARY */
    SQL_C_BINARY,                       /* ZLPL_C_DATATYPE_VARBINARY */
    SQL_C_LONGVARBINARY,                /* ZLPL_C_DATATYPE_LONGVARBINARY */
    SQL_C_BIT,                          /* ZLPL_C_DATATYPE_BOOLEAN */
    SQL_C_NUMERIC,                      /* ZLPL_C_DATATYPE_NUMBER */
    SQL_C_TYPE_TIMESTAMP,               /* ZLPL_C_DATATYPE_DATE */
    SQL_C_TYPE_TIME,                    /* ZLPL_C_DATATYPE_TIME */
    SQL_C_TYPE_TIMESTAMP,               /* ZLPL_C_DATATYPE_TIMESTAMP */
    SQL_C_TYPE_TIME_WITH_TIMEZONE,      /* ZLPL_C_DATATYPE_TIMETZ */
    SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE, /* ZLPL_C_DATATYPE_TIMESTAMPTZ */
    SQL_C_INTERVAL_YEAR,                /* ZLPL_C_DATATYPE_INTERVAL_YEAR */
    SQL_C_INTERVAL_MONTH,               /* ZLPL_C_DATATYPE_INTERVAL_MONTH */
    SQL_C_INTERVAL_DAY,                 /* ZLPL_C_DATATYPE_INTERVAL_DAY */
    SQL_C_INTERVAL_HOUR,                /* ZLPL_C_DATATYPE_INTERVAL_HOUR */
    SQL_C_INTERVAL_MINUTE,              /* ZLPL_C_DATATYPE_INTERVAL_MINUTE */
    SQL_C_INTERVAL_SECOND,              /* ZLPL_C_DATATYPE_INTERVAL_SECOND */
    SQL_C_INTERVAL_YEAR_TO_MONTH,       /* ZLPL_C_DATATYPE_INTERVAL_YEAR_TO_MONTH */
    SQL_C_INTERVAL_DAY_TO_HOUR,         /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_HOUR */
    SQL_C_INTERVAL_DAY_TO_MINUTE,       /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_MINUTE */
    SQL_C_INTERVAL_DAY_TO_SECOND,       /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_SECOND */
    SQL_C_INTERVAL_HOUR_TO_MINUTE,      /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_MINUTE */
    SQL_C_INTERVAL_HOUR_TO_SECOND,      /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_SECOND */
    SQL_C_INTERVAL_MINUTE_TO_SECOND,    /* ZLPL_C_DATATYPE_INTERVAL_MINUTE_TO_SECOND */
    SQL_C_DEFAULT,                      /* ZLPL_C_DATATYPE_SQL_CONTEXT */
    SQL_C_DEFAULT,                      /* ZLPL_C_DATATYPE_STRUCT */
};

/* zlplCDataType 과 mapping을 맞워야 함 */
static SQLSMALLINT gSqlType[ZLPL_C_DATATYPE_MAX] =
{
    SQL_TYPE_NULL,                    /* ZLPL_C_DATATYPE_UNKNOWN */
    SQL_VARCHAR,                      /* ZLPL_C_DATATYPE_SCHAR */
    SQL_VARCHAR,                      /* ZLPL_C_DATATYPE_UCHAR */
    SQL_SMALLINT,                     /* ZLPL_C_DATATYPE_SSHORT */
    SQL_SMALLINT,                     /* ZLPL_C_DATATYPE_USHORT */
    SQL_INTEGER,                      /* ZLPL_C_DATATYPE_SINT */
    SQL_INTEGER,                      /* ZLPL_C_DATATYPE_UINT */
    SQL_BIGINT,                       /* ZLPL_C_DATATYPE_SLONG */
    SQL_BIGINT,                       /* ZLPL_C_DATATYPE_ULONG */
    SQL_BIGINT,                       /* ZLPL_C_DATATYPE_SLONGLONG */
    SQL_BIGINT,                       /* ZLPL_C_DATATYPE_ULONGLONG */
    SQL_REAL,                         /* ZLPL_C_DATATYPE_FLOAT */
    SQL_DOUBLE,                       /* ZLPL_C_DATATYPE_DOUBLE */
    SQL_VARCHAR,                      /* ZLPL_C_DATATYPE_CHAR */
    SQL_VARCHAR,                      /* ZLPL_C_DATATYPE_VARCHAR */
    SQL_LONGVARCHAR,                  /* ZLPL_C_DATATYPE_LONGVARCHAR */
    SQL_BINARY,                       /* ZLPL_C_DATATYPE_BINARY */
    SQL_VARBINARY,                    /* ZLPL_C_DATATYPE_VARBINARY */
    SQL_LONGVARBINARY,                /* ZLPL_C_DATATYPE_LONGVARBINARY */
    SQL_BIT,                          /* ZLPL_C_DATATYPE_BOOLEAN */
    SQL_NUMERIC,                      /* ZLPL_C_DATATYPE_NUMBER */
    SQL_TYPE_TIMESTAMP,               /* ZLPL_C_DATATYPE_DATE */
    SQL_TYPE_TIME,                    /* ZLPL_C_DATATYPE_TIME */
    SQL_TYPE_TIMESTAMP,               /* ZLPL_C_DATATYPE_TIMESTAMP */
    SQL_TYPE_TIME_WITH_TIMEZONE,      /* ZLPL_C_DATATYPE_TIMETZ */
    SQL_TYPE_TIMESTAMP_WITH_TIMEZONE, /* ZLPL_C_DATATYPE_TIMESTAMPTZ */
    SQL_INTERVAL_YEAR,                /* ZLPL_C_DATATYPE_INTERVAL_YEAR */
    SQL_INTERVAL_MONTH,               /* ZLPL_C_DATATYPE_INTERVAL_MONTH */
    SQL_INTERVAL_DAY,                 /* ZLPL_C_DATATYPE_INTERVAL_DAY */
    SQL_INTERVAL_HOUR,                /* ZLPL_C_DATATYPE_INTERVAL_HOUR */
    SQL_INTERVAL_MINUTE,              /* ZLPL_C_DATATYPE_INTERVAL_MINUTE */
    SQL_INTERVAL_SECOND,              /* ZLPL_C_DATATYPE_INTERVAL_SECOND */
    SQL_INTERVAL_YEAR_TO_MONTH,       /* ZLPL_C_DATATYPE_INTERVAL_YEAR_TO_MONTH */
    SQL_INTERVAL_DAY_TO_HOUR,         /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_HOUR */
    SQL_INTERVAL_DAY_TO_MINUTE,       /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_MINUTE */
    SQL_INTERVAL_DAY_TO_SECOND,       /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_SECOND */
    SQL_INTERVAL_HOUR_TO_MINUTE,      /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_MINUTE */
    SQL_INTERVAL_HOUR_TO_SECOND,      /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_SECOND */
    SQL_INTERVAL_MINUTE_TO_SECOND,    /* ZLPL_C_DATATYPE_INTERVAL_MINUTE_TO_SECOND */
    SQL_TYPE_NULL,                    /* ZLPL_C_DATATYPE_SQL_CONTEXT */
    SQL_TYPE_NULL,                    /* ZLPL_C_DATATYPE_STRUCT */
};

/* zlplCDataType 과 mapping을 맞워야 함 */
static stlInt32 gDataSize[ZLPL_C_DATATYPE_MAX] =
{
    0, /* ZLPL_C_DATATYPE_UNKNOWN */
    1, /* ZLPL_C_DATATYPE_SCHAR */
    1, /* ZLPL_C_DATATYPE_UCHAR */
    2, /* ZLPL_C_DATATYPE_SSHORT */
    2, /* ZLPL_C_DATATYPE_USHORT */
    4, /* ZLPL_C_DATATYPE_SINT */
    4, /* ZLPL_C_DATATYPE_UINT */
    8, /* ZLPL_C_DATATYPE_SLONG */
    8, /* ZLPL_C_DATATYPE_ULONG */
    8, /* ZLPL_C_DATATYPE_SLONGLONG */
    8, /* ZLPL_C_DATATYPE_ULONGLONG */
    4, /* ZLPL_C_DATATYPE_FLOAT */
    8, /* ZLPL_C_DATATYPE_DOUBLE */
    0, /* ZLPL_C_DATATYPE_CHAR */
    0, /* ZLPL_C_DATATYPE_VARCHAR */
    0, /* ZLPL_C_DATATYPE_LONGVARCHAR */
    0, /* ZLPL_C_DATATYPE_BINARY */
    0, /* ZLPL_C_DATATYPE_VARBINARY */
    0, /* ZLPL_C_DATATYPE_LONGVARBINARY */
    1, /* ZLPL_C_DATATYPE_BOOLEAN */
    STL_SIZEOF(SQL_NUMERIC_STRUCT),                 /* ZLPL_C_DATATYPE_NUMBER */
    STL_SIZEOF(SQL_TIMESTAMP_STRUCT),               /* ZLPL_C_DATATYPE_DATE */
    STL_SIZEOF(SQL_TIME_STRUCT),                    /* ZLPL_C_DATATYPE_TIME */
    STL_SIZEOF(SQL_TIMESTAMP_STRUCT),               /* ZLPL_C_DATATYPE_TIMESTAMP */
    STL_SIZEOF(SQL_TIME_WITH_TIMEZONE_STRUCT),      /* ZLPL_C_DATATYPE_TIMETZ */
    STL_SIZEOF(SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT), /* ZLPL_C_DATATYPE_TIMESTAMPTZ */
    STL_SIZEOF(SQL_INTERVAL_STRUCT), /* ZLPL_C_DATATYPE_INTERVAL_YEAR */
    STL_SIZEOF(SQL_INTERVAL_STRUCT), /* ZLPL_C_DATATYPE_INTERVAL_MONTH */
    STL_SIZEOF(SQL_INTERVAL_STRUCT), /* ZLPL_C_DATATYPE_INTERVAL_DAY */
    STL_SIZEOF(SQL_INTERVAL_STRUCT), /* ZLPL_C_DATATYPE_INTERVAL_HOUR */
    STL_SIZEOF(SQL_INTERVAL_STRUCT), /* ZLPL_C_DATATYPE_INTERVAL_MINUTE */
    STL_SIZEOF(SQL_INTERVAL_STRUCT), /* ZLPL_C_DATATYPE_INTERVAL_SECOND */
    STL_SIZEOF(SQL_INTERVAL_STRUCT), /* ZLPL_C_DATATYPE_INTERVAL_YEAR_TO_MONTH */
    STL_SIZEOF(SQL_INTERVAL_STRUCT), /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_HOUR */
    STL_SIZEOF(SQL_INTERVAL_STRUCT), /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_MINUTE */
    STL_SIZEOF(SQL_INTERVAL_STRUCT), /* ZLPL_C_DATATYPE_INTERVAL_DAY_TO_SECOND */
    STL_SIZEOF(SQL_INTERVAL_STRUCT), /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_MINUTE */
    STL_SIZEOF(SQL_INTERVAL_STRUCT), /* ZLPL_C_DATATYPE_INTERVAL_HOUR_TO_SECOND */
    STL_SIZEOF(SQL_INTERVAL_STRUCT), /* ZLPL_C_DATATYPE_INTERVAL_MINUTE_TO_SECOND */
    0, /* ZLPL_C_DATATYPE_SQL_CONTEXT */
    0, /* ZLPL_C_DATATYPE_STRUCT */
};

stlInt32 zlpbGetDataSize( zlplCDataType aDataType )
{
    return gDataSize[aDataType];
}

stlInt32 zlpbGetDataBufferLength( zlplSqlHostVar *aHostVar )
{
    stlInt32  sColSize;

    sColSize = zlpbGetDataSize( aHostVar->data_type );
    if( sColSize == 0 )
    {
        if( (aHostVar->data_type == ZLPL_C_DATATYPE_VARCHAR) ||
            (aHostVar->data_type == ZLPL_C_DATATYPE_VARBINARY) )
        {
            if( (aHostVar->buflen >= DTL_VARCHAR_MAX_PRECISION) ||
                (aHostVar->buflen == 0) )
            {
                sColSize = DTL_VARCHAR_MAX_PRECISION;
            }
            else
            {
                sColSize = aHostVar->buflen;
            }
        }
        else if( (aHostVar->data_type == ZLPL_C_DATATYPE_CHAR) ||
                 (aHostVar->data_type == ZLPL_C_DATATYPE_BINARY) )
        {
            if( (aHostVar->buflen >= DTL_CHAR_MAX_PRECISION) ||
                (aHostVar->buflen == 0) )
            {
                sColSize = DTL_CHAR_MAX_PRECISION;
            }
            else
            {
                sColSize = aHostVar->buflen;
            }
        }
        else
        {
            if( aHostVar->buflen == 0 )
            {
                sColSize = DTL_CHAR_MAX_PRECISION;
            }
            else
            {
                sColSize = aHostVar->buflen;
            }
        }
    }

    return sColSize;
}

stlStatus zlpbAdjustDataType( zlplSqlContext  * aSqlContext,
                              zlplSqlArgs     * aSqlArgs )
{
    if( aSqlArgs->hvc > 0 )
    {
        STL_TRY( zlpbAdjustDataTypeHostVar( aSqlArgs->hvc,
                                            aSqlArgs->sqlhv )
                 == STL_SUCCESS );
    }

    if( aSqlArgs->sqlfetch != NULL )
    {
        if( aSqlArgs->sqlfetch->offsethv != NULL )
        {
            STL_TRY( zlpbAdjustDataTypeHostVar( 1,
                                                aSqlArgs->sqlfetch->offsethv )
                     == STL_SUCCESS );
        }
    }

    if( aSqlArgs->dynusing != NULL )
    {
        if( aSqlArgs->dynusing->hvc > 0 )
        {
            STL_TRY( zlpbAdjustDataTypeHostVar( aSqlArgs->dynusing->hvc,
                                                aSqlArgs->dynusing->dynhv )
                     == STL_SUCCESS );
        }
    }

    if( aSqlArgs->dyninto != NULL )
    {
        if( aSqlArgs->dyninto->hvc > 0 )
        {
            STL_TRY( zlpbAdjustDataTypeHostVar( aSqlArgs->dyninto->hvc,
                                                aSqlArgs->dyninto->dynhv )
                     == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlpbAdjustDataTypeHostVar( stlInt32        aHostVarCnt,
                                     zlplSqlHostVar *aHostVar )
{
    stlInt32        i;

    for( i = 0; i < aHostVarCnt; i ++ )
    {
        switch( aHostVar[i].data_type )
        {
            case ZLPL_C_DATATYPE_SLONG:
                if( aHostVar[i].el_size == STL_SIZEOF(stlInt32) )
                {
                    aHostVar[i].data_type = ZLPL_C_DATATYPE_SINT;
                }
                break;
            case ZLPL_C_DATATYPE_ULONG:
                if( aHostVar[i].el_size == STL_SIZEOF(stlUInt32) )
                {
                    aHostVar[i].data_type = ZLPL_C_DATATYPE_UINT;
                }
                break;
            case ZLPL_C_DATATYPE_SLONGLONG:
                aHostVar[i].data_type = ZLPL_C_DATATYPE_SLONG;
                break;
            case ZLPL_C_DATATYPE_ULONGLONG:
                aHostVar[i].data_type = ZLPL_C_DATATYPE_ULONG;
                break;
            default:
                break;
        }

        switch( aHostVar[i].ind_type )
        {
            case ZLPL_C_DATATYPE_SLONG:
                if( aHostVar[i].ind_el_size == STL_SIZEOF(stlInt32) )
                {
                    aHostVar[i].ind_type = ZLPL_C_DATATYPE_SINT;
                }
                break;
            case ZLPL_C_DATATYPE_ULONG:
                if( aHostVar[i].ind_el_size == STL_SIZEOF(stlUInt32) )
                {
                    aHostVar[i].ind_type = ZLPL_C_DATATYPE_UINT;
                }
                break;
            case ZLPL_C_DATATYPE_SLONGLONG:
                aHostVar[i].ind_type = ZLPL_C_DATATYPE_SLONG;
                break;
            case ZLPL_C_DATATYPE_ULONGLONG:
                aHostVar[i].ind_type = ZLPL_C_DATATYPE_ULONG;
                break;
            default:
                break;
        }
    }

    return STL_SUCCESS;
}

/**
 * @brief Host Variable 에 대한 SQLBindParamter() 를 수행한다.
 * @param[in]  aStmtHandle       Statement Handle
 * @param[in]  aSqlArgs          SQL arguments
 * @param[in]  aHostCnt          Host Variable Count
 * @param[in]  aHostVar          Host Variable Array
 * @param[in]  aValueArray       Value Array
 * @param[in]  aSymbolIndArray   Symbol 이 소유한 Indicator Array
 * @param[in]  aDynParamIOType   Dynamic Parameter I/O Type (nullable)
 * @remarks
 */
stlStatus zlpbBindParams( SQLHANDLE          aStmtHandle,
                          zlplSqlArgs      * aSqlArgs,
                          stlInt32           aHostCnt,
                          zlplSqlHostVar   * aHostVar,
                          stlChar         ** aValueArray,
                          SQLLEN          ** aSymbolIndArray,
                          zlplParamIOType  * aDynParamIOType )
{
    stlInt32         i;
    SQLRETURN        sReturn;
    zlplParamIOType  sBindIOType;
    SQLLEN           sBufLen;
    SQLULEN          sColumnSize;
    SQLSMALLINT      sCDataType;

    for(i = 0; i < aHostCnt; i ++)
    {
        if ( aDynParamIOType == NULL )
        {
            /**
             * Dynamic Paramter 가 아닌 경우
             */

            sBindIOType = aHostVar[i].io_type;
        }
        else
        {
            /**
             * Dynamic Paramter 인 경우
             */
            sBindIOType = aDynParamIOType[i];
        }

        sBufLen     = aHostVar[i].buflen;
        sColumnSize = aHostVar[i].buflen;

        if( (aHostVar[i].data_type == ZLPL_C_DATATYPE_VARCHAR) ||
            (aHostVar[i].data_type == ZLPL_C_DATATYPE_VARBINARY) )
        {
            if( sColumnSize >= DTL_VARCHAR_MAX_PRECISION )
            {
                sColumnSize = DTL_VARCHAR_MAX_PRECISION;
                sBufLen = DTL_VARCHAR_MAX_PRECISION;
            }
        }
        else
        {
            if(aHostVar[i].data_type == ZLPL_C_DATATYPE_CHAR)
            {
                if( (sColumnSize >= DTL_CHAR_MAX_PRECISION) ||
                    (sColumnSize == 0) )
                {
                    sColumnSize = DTL_CHAR_MAX_PRECISION;
                    sBufLen = DTL_CHAR_MAX_PRECISION;
                }
            }
        }

        if( sBindIOType == ZLPL_PARAM_IO_TYPE_IN )
        {
            sCDataType = gInCDataType[aHostVar[i].data_type];
        }
        else
        {
            sCDataType = gOutCDataType[aHostVar[i].data_type];
        }

        sReturn = SQLBindParameter(aStmtHandle,
                                   i + 1,
                                   gParamIOType[sBindIOType],
                                   sCDataType,
                                   gSqlType[aHostVar[i].data_type],
                                   sColumnSize,
                                   aHostVar[i].scale,
                                   aValueArray[i],
                                   sBufLen,
                                   & aSymbolIndArray[i][0] );

        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_STMT,
                               aStmtHandle,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Host Variable 에 대한 SQLBindCol() 를 수행한다.
 * @param[in]  aStmtHandle       Statement Handle
 * @param[in]  aSqlArgs          SQL arguments
 * @param[in]  aHostCnt          Host Variable Count
 * @param[in]  aHostVar          Host Variable Array
 * @param[in]  aValueArray       Value Array
 * @param[in]  aSymbolIndArray   Symbol 이 소유한 Indicator Array
 * @remarks
 */
stlStatus zlpbBindCols( SQLHANDLE         aStmtHandle,
                        zlplSqlArgs     * aSqlArgs,
                        stlInt32          aHostCnt,
                        zlplSqlHostVar  * aHostVar,
                        stlChar        ** aValueArray,
                        SQLLEN         ** aSymbolIndArray )
{
    stlInt32         i;
    SQLRETURN        sReturn;

    for(i = 0; i < aHostCnt; i ++)
    {
        sReturn = SQLBindCol(aStmtHandle,
                             i + 1,
                             gOutCDataType[aHostVar[i].data_type],
                             aValueArray[i],
                             aHostVar[i].buflen,
                             & aSymbolIndArray[i][0] );

        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_STMT,
                               aStmtHandle,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief  SQLExecute 수행전 SQLBindParameter 에 포함되는 Host Variable 의 값들에 대한 길이 정보를 설정한다.
 * @param[in]  aSqlContext       SQL Context
 * @param[in]  aSqlArgs          SQL arguments
 * @param[in]  aStmtHandle       Statement Handle
 * @param[in]  aHostVarCount     Host Variable Count
 * @param[in]  aHostVar          Host Variable Array
 * @param[in]  aArraySize        Array Size
 * @param[in]  aItemProcessed    Parameters processed count
 * @param[in]  aItemStatusArray  Parameters status array
 * @param[in]  aValueBuf         Value Buffer
 * @param[in]  aSymbolIndArray   Symbol 이 소유한 Indicator Array
 * @param[in]  aDynParamIOType   Dynamic Parameter 의 I/O Type (nullable)
 * @param[in]  aIsBindParam      SQLBinddParameter에 대한 정보 설정은 TRUE, SQLBindCol 정보는 FALSE
 * @remarks
 */
stlStatus zlpbSetBeforeInfo(zlplSqlContext   * aSqlContext,
                            zlplSqlArgs      * aSqlArgs,
                            SQLHANDLE          aStmtHandle,
                            stlInt32           aHostVarCount,
                            zlplSqlHostVar   * aHostVar,
                            stlInt32           aArraySize,
                            SQLULEN          * aItemProcessed,
                            SQLUSMALLINT     * aItemStatusArray,
                            stlChar         ** aValueBuf,
                            SQLLEN          ** aSymbolIndArray,
                            zlplParamIOType  * aDynParamIOType,
                            stlBool            aIsBindParam )
{
    stlInt32             i;
    zlplVarchar         *sVarchar = NULL;
    stlInt16             sIOType = 0;
    SQLPOINTER           sValuePtr;
    SQLPOINTER           sArraySizeArg;
    SQLLEN               sBufLen;
    stlChar             *sIndPtr;
    SQL_NUMERIC_STRUCT  *sNumeric;
    stlInt32             sPrecision;
    stlInt32             sScale;
    stlInt32             sArrayIdx;
    SQLRETURN            sReturn;
    stlChar             *sValueBase;
    stlInt32             sCopySize;

    if( aIsBindParam == STL_TRUE )
    {
        if( aArraySize > 1 )
        {
            /**
             * Parameter Binding
             */

            /* Set the SQL_ATTR_PARAM_BIND_TYPE statement attribute to use
             * column-wise binding.
             */
            sReturn = SQLSetStmtAttr(aStmtHandle,
                                     SQL_ATTR_PARAM_BIND_TYPE,
                                     SQL_PARAM_BIND_BY_COLUMN,
                                     0);
            STL_TRY(zlpeCheckError(sReturn,
                                   SQL_HANDLE_STMT,
                                   aStmtHandle,
                                   (zlplSqlca *)aSqlArgs->sql_ca,
                                   (stlChar *)aSqlArgs->sql_state)
                    == STL_SUCCESS);

            /*
             * Specify the number of elements in each parameter array.
             */
            sArraySizeArg = (SQLPOINTER)(SQLLEN)aArraySize;
            sReturn = SQLSetStmtAttr(aStmtHandle,
                                     SQL_ATTR_PARAMSET_SIZE,
                                     sArraySizeArg,
                                     0);
            STL_TRY(zlpeCheckError(sReturn,
                                   SQL_HANDLE_STMT,
                                   aStmtHandle,
                                   (zlplSqlca *)aSqlArgs->sql_ca,
                                   (stlChar *)aSqlArgs->sql_state)
                    == STL_SUCCESS);

            /*
             * Atomic option support
             */
            if( aSqlArgs->atomic == 1 )
            {
                sReturn = SQLSetStmtAttr(aStmtHandle,
                                         SQL_ATTR_ATOMIC_EXECUTION,
                                         (SQLPOINTER)SQL_ATOMIC_EXECUTION_ON,
                                         0);
                STL_TRY(zlpeCheckError(sReturn,
                                       SQL_HANDLE_STMT,
                                       aStmtHandle,
                                       (zlplSqlca *)aSqlArgs->sql_ca,
                                       (stlChar *)aSqlArgs->sql_state)
                        == STL_SUCCESS);
            }
        }

        /*
         * Specify an array in which to return the status of each set of
         * parameters.
         */
        sReturn = SQLSetStmtAttr(aStmtHandle,
                                 SQL_ATTR_PARAM_STATUS_PTR,
                                 aItemStatusArray,
                                 0);
        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_STMT,
                               aStmtHandle,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);

        /*
         * Specify an SQLUINTEGER value in which to return the number of sets of
         * parameters processed.
         */
        sReturn = SQLSetStmtAttr(aStmtHandle,
                                 SQL_ATTR_PARAMS_PROCESSED_PTR,
                                 aItemProcessed,
                                 0);
        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_STMT,
                               aStmtHandle,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);
    }
    else
    {
        /**
         * Row Binding
         */

        if( aArraySize > 1 )
        {
            /* Set the SQL_ATTR_PARAM_BIND_TYPE statement attribute to use
             * column-wise binding.
             */
            sReturn = SQLSetStmtAttr(aStmtHandle,
                                     SQL_ATTR_ROW_BIND_TYPE,
                                     SQL_BIND_BY_COLUMN,
                                     0);
            STL_TRY(zlpeCheckError(sReturn,
                                   SQL_HANDLE_STMT,
                                   aStmtHandle,
                                   (zlplSqlca *)aSqlArgs->sql_ca,
                                   (stlChar *)aSqlArgs->sql_state)
                    == STL_SUCCESS);

            /*
             * Specify the number of elements in each parameter array.
             */
            sArraySizeArg = (SQLPOINTER)(SQLLEN)aArraySize;
            sReturn = SQLSetStmtAttr(aStmtHandle,
                                     SQL_ATTR_ROW_ARRAY_SIZE,
                                     sArraySizeArg,
                                     0);
            STL_TRY(zlpeCheckError(sReturn,
                                   SQL_HANDLE_STMT,
                                   aStmtHandle,
                                   (zlplSqlca *)aSqlArgs->sql_ca,
                                   (stlChar *)aSqlArgs->sql_state)
                    == STL_SUCCESS);
        }

        /*
         * Specify an array in which to return the status of each set of
         * parameters.
         */
        sReturn = SQLSetStmtAttr(aStmtHandle,
                                 SQL_ATTR_ROW_STATUS_PTR,
                                 aItemStatusArray,
                                 0);
        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_STMT,
                               aStmtHandle,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);

        /*
         * Specify an SQLUINTEGER value in which to return the number of sets of
         * parameters processed.
         */
        sReturn = SQLSetStmtAttr(aStmtHandle,
                                 SQL_ATTR_ROWS_FETCHED_PTR,
                                 aItemProcessed,
                                 0);
        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_STMT,
                               aStmtHandle,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);
    }

    for( i = 0; i < aHostVarCount; i++ )
    {
        if ( aDynParamIOType == NULL )
        {
            sIOType = aHostVar[i].io_type;
        }
        else
        {
            sIOType = aDynParamIOType[i];
        }

        /*
         * 기존에 bind parameter된 column에 대해서
         * Value Data pointer를 재설정
         */
        sValuePtr = aHostVar[i].value;

        if( ZLPL_IS_INTERVAL_TYPE( aHostVar[i].data_type ) )
        {
            STL_TRY( zlpbSetIntervalLeadingPrecision( aSqlArgs,
                                                      aStmtHandle,
                                                      i + 1,
                                                      aIsBindParam )
                     == STL_SUCCESS );
        }
        else if( ZLPL_IS_NUMBER_TYPE( aHostVar[i].data_type ) )
        {
            sNumeric = (SQL_NUMERIC_STRUCT *)sValuePtr;
            if( sIOType == ZLPL_PARAM_IO_TYPE_IN )
            {
                sPrecision = sNumeric->precision;
                sScale = sNumeric->scale;
            }
            else
            {
                sPrecision = aHostVar[i].precision;
                sScale = aHostVar[i].scale;
            }

            STL_TRY( zlpbSetNumberTypeInfo( aSqlArgs,
                                            aStmtHandle,
                                            i + 1,
                                            aIsBindParam,
                                            sPrecision,
                                            sScale )
                     == STL_SUCCESS );
        }
        else if( (aHostVar[i].data_type == ZLPL_C_DATATYPE_CHAR) ||
                 (aHostVar[i].data_type == ZLPL_C_DATATYPE_BINARY) )
        {
            if( sIOType == ZLPL_PARAM_IO_TYPE_IN )
            {
                sBufLen = aHostVar[i].buflen;
                if(sBufLen == 0)
                {
                    //sBufLen = stlStrlen( sValuePtr ) + 1;
                    STL_TRY( zlpbSetCharBufLen( aSqlArgs,
                                                aStmtHandle,
                                                i + 1,
                                                DTL_CHAR_MAX_PRECISION )
                             == STL_SUCCESS );
                }
            }
        }

        sValueBase = aValueBuf[i];
        sCopySize = zlpbGetDataBufferLength( &aHostVar[i] );

        for( sArrayIdx = 0; sArrayIdx < aArraySize; sArrayIdx ++ )
        {
            sValuePtr = aHostVar[i].value;
            sValuePtr = (stlChar*)sValuePtr + (aHostVar[i].el_size * sArrayIdx);

            /*
             * 사용자 영역의 Value를 내부의 Value buffer로 copy
             *   : aHostVar[i].value  ->  aValueBuf[i]
             */
            if( (aHostVar[i].data_type == ZLPL_C_DATATYPE_VARCHAR)
                || (aHostVar[i].data_type == ZLPL_C_DATATYPE_VARBINARY) )
            {
                sVarchar = (zlplVarchar *)sValuePtr;
                sValuePtr = sVarchar->mArr;
            }

            stlMemcpy( sValueBase, sValuePtr, sCopySize );
            sValueBase += sCopySize;

            if( (sIOType == ZLPL_PARAM_IO_TYPE_IN) ||
                (sIOType == ZLPL_PARAM_IO_TYPE_INOUT) )
            {
                /**
                 * E-SQL VARCHAR type 인 경우
                 * mLen 값이 우선한다.
                 */
                if( (aHostVar[i].data_type == ZLPL_C_DATATYPE_VARCHAR)
                    || (aHostVar[i].data_type == ZLPL_C_DATATYPE_VARBINARY) )
                {
                    if(sVarchar->mLen >= DTL_VARCHAR_MAX_PRECISION)
                    {
                        sVarchar->mLen = DTL_VARCHAR_MAX_PRECISION;
                    }
                    aSymbolIndArray[i][sArrayIdx] = sVarchar->mLen;
                }
                else if( (aHostVar[i].data_type == ZLPL_C_DATATYPE_SCHAR)
                         || (aHostVar[i].data_type == ZLPL_C_DATATYPE_UCHAR) )
                {
                    aSymbolIndArray[i][sArrayIdx] = aHostVar[i].buflen;
                }
                else
                {
                    if ( aHostVar[i].indicator == NULL )
                    {
                        if(aHostVar[i].data_type == ZLPL_C_DATATYPE_CHAR)
                        {
                            if(stlStrlen(sValuePtr) >= DTL_CHAR_MAX_PRECISION)
                            {
                                ((stlChar *)sValuePtr)[DTL_CHAR_MAX_PRECISION] = '\0';
                                aSymbolIndArray[i][sArrayIdx] = DTL_CHAR_MAX_PRECISION;
                            }
                            else
                            {
                                aSymbolIndArray[i][sArrayIdx] = SQL_NTS;
                            }
                        }
                        else
                        {
                            aSymbolIndArray[i][sArrayIdx] = SQL_NTS;
                        }
                    }
                    else
                    {
                        sIndPtr = aHostVar[i].indicator;
                        sIndPtr += sArrayIdx * aHostVar[i].ind_el_size;
                        switch(aHostVar[i].ind_type)
                        {
                            case ZLPL_C_DATATYPE_SLONGLONG:
                            case ZLPL_C_DATATYPE_SLONG:
                                aSymbolIndArray[i][sArrayIdx] = *((stlInt64 *)sIndPtr);
                                break;
                            case ZLPL_C_DATATYPE_SINT:
                                aSymbolIndArray[i][sArrayIdx] = *((stlInt32 *)sIndPtr);
                                break;
                            case ZLPL_C_DATATYPE_SSHORT:
                                aSymbolIndArray[i][sArrayIdx] = *((stlInt16 *)sIndPtr);
                                break;
                            case ZLPL_C_DATATYPE_SCHAR:
                                aSymbolIndArray[i][sArrayIdx] = *((stlInt8 *)sIndPtr);
                                break;
                            case ZLPL_C_DATATYPE_ULONGLONG:
                            case ZLPL_C_DATATYPE_ULONG:
                                aSymbolIndArray[i][sArrayIdx] = *((stlUInt64 *)sIndPtr);
                                break;
                            case ZLPL_C_DATATYPE_UINT:
                                aSymbolIndArray[i][sArrayIdx] = *((stlUInt32 *)sIndPtr);
                                break;
                            case ZLPL_C_DATATYPE_USHORT:
                                aSymbolIndArray[i][sArrayIdx] = *((stlInt16 *)sIndPtr);
                                break;
                            case ZLPL_C_DATATYPE_UCHAR:
                                aSymbolIndArray[i][sArrayIdx] = *((stlUInt8 *)sIndPtr);
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief  Host 변수가 Interval type일 경우, Leading precision을 설정한다.
 * @param[in]  aSqlArgs          SQL arguments
 * @param[in]  aStmtHandle       Statement Handle
 * @param[in]  aColumnIdx        Column Index (1-based)
 * @param[in]  aIsBindParam      SQLBinddParameter에 대한 정보 설정은 TRUE, SQLBindCol 정보는 FALSE
 * @remarks
 */
stlStatus zlpbSetIntervalLeadingPrecision(zlplSqlArgs *aSqlArgs,
                                          SQLHANDLE    aStmtHandle,
                                          stlInt32     aColumnIdx,
                                          stlBool      aIsBindParam)
{
    SQLRETURN        sReturn;
    SQLHDESC         sApdDesc;
    SQLHDESC         sIpdDesc;
    SQLHDESC         sArdDesc;
    SQLPOINTER       sDataPtr = NULL;

    if( aIsBindParam == STL_TRUE )
    {
        sReturn = SQLGetStmtAttr( aStmtHandle,
                                  SQL_ATTR_IMP_PARAM_DESC,
                                  &sIpdDesc,
                                  SQL_IS_POINTER,
                                  NULL );

        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_STMT,
                               aStmtHandle,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);

        sReturn = SQLGetStmtAttr( aStmtHandle,
                                  SQL_ATTR_APP_PARAM_DESC,
                                  &sApdDesc,
                                  SQL_IS_POINTER,
                                  NULL );

        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_STMT,
                               aStmtHandle,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);

        sReturn = SQLSetDescField( sIpdDesc,
                                   aColumnIdx,
                                   SQL_DESC_DATETIME_INTERVAL_PRECISION,
                                   (SQLPOINTER)DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                   0 );

        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_DESC,
                               sIpdDesc,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);

        sDataPtr = NULL;
        sReturn = SQLSetDescField( sIpdDesc,
                                   aColumnIdx,
                                   SQL_DESC_DATA_PTR,
                                   sDataPtr,
                                   0 );

        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_DESC,
                               sIpdDesc,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);

        sReturn = SQLGetDescField( sApdDesc,
                                   aColumnIdx,
                                   SQL_DESC_DATA_PTR,
                                   (SQLPOINTER)&sDataPtr,
                                   0,
                                   NULL );
        
        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_DESC,
                               sApdDesc,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);

        sReturn = SQLSetDescField( sApdDesc,
                                   aColumnIdx,
                                   SQL_DESC_DATETIME_INTERVAL_PRECISION,
                                   (SQLPOINTER)DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                   0 );

        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_DESC,
                               sApdDesc,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);

        sReturn = SQLSetDescField( sApdDesc,
                                   aColumnIdx,
                                   SQL_DESC_DATA_PTR,
                                   sDataPtr,
                                   0 );

        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_DESC,
                               sApdDesc,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);
    }
    else
    {
        sReturn = SQLGetStmtAttr( aStmtHandle,
                                  SQL_ATTR_APP_ROW_DESC,
                                  &sArdDesc,
                                  SQL_IS_POINTER,
                                  NULL );

        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_STMT,
                               aStmtHandle,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);

        sReturn = SQLGetDescField( sArdDesc,
                                   aColumnIdx,
                                   SQL_DESC_DATA_PTR,
                                   (SQLPOINTER)&sDataPtr,
                                   0,
                                   NULL );
        
        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_DESC,
                               sArdDesc,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);

        sReturn = SQLSetDescField( sArdDesc,
                                   aColumnIdx,
                                   SQL_DESC_DATETIME_INTERVAL_PRECISION,
                                   (SQLPOINTER)DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                   0 );

        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_DESC,
                               sArdDesc,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);

        sReturn = SQLSetDescField( sArdDesc,
                                   aColumnIdx,
                                   SQL_DESC_DATA_PTR,
                                   sDataPtr,
                                   0 );

        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_DESC,
                               sArdDesc,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief  Host 변수가 Number type일 경우, precision, scale을 설정한다.
 * @param[in]  aSqlArgs          SQL arguments
 * @param[in]  aStmtHandle       Statement Handle
 * @param[in]  aColumnIdx        Column Index (1-based)
 * @param[in]  aIsBindParam      SQLBinddParameter에 대한 정보 설정은 TRUE, SQLBindCol 정보는 FALSE
 * @param[in]  aPrecision        Precision
 * @param[in]  aScale            Scale
 * @remarks
 */
stlStatus zlpbSetNumberTypeInfo(zlplSqlArgs *aSqlArgs,
                                SQLHANDLE    aStmtHandle,
                                stlInt32     aColumnIdx,
                                stlBool      aIsBindParam,
                                stlInt32     aPrecision,
                                stlInt32     aScale)
{
    if( aIsBindParam == STL_TRUE )
    {
        STL_TRY( zlpbSetNumberPrecision(aSqlArgs,
                                        aStmtHandle,
                                        SQL_ATTR_IMP_PARAM_DESC,
                                        aColumnIdx,
                                        aPrecision,
                                        aScale)
                 == STL_SUCCESS );

        STL_TRY( zlpbSetNumberPrecision(aSqlArgs,
                                        aStmtHandle,
                                        SQL_ATTR_APP_PARAM_DESC,
                                        aColumnIdx,
                                        aPrecision,
                                        aScale)
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( zlpbSetNumberPrecision(aSqlArgs,
                                        aStmtHandle,
                                        SQL_ATTR_APP_ROW_DESC,
                                        aColumnIdx,
                                        aPrecision,
                                        aScale)
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief  Host 변수가 Number type일 경우, precision, scale을 설정한다.
 * @param[in]  aSqlArgs          SQL arguments
 * @param[in]  aStmtHandle       Statement Handle
 * @param[in]  aDescType         Descriptor Handle Type
 * @param[in]  aColumnIdx        Column Index (1-based)
 * @param[in]  aPrecision        Precision
 * @param[in]  aScale            Scale
 * @remarks
 */
stlStatus zlpbSetNumberPrecision(zlplSqlArgs *aSqlArgs,
                                 SQLHANDLE    aStmtHandle,
                                 stlInt32     aDescType,
                                 stlInt32     aColumnIdx,
                                 stlInt32     aPrecision,
                                 stlInt32     aScale)
{
    SQLRETURN        sReturn;
    SQLHDESC         sDesc;
    SQLPOINTER       sPrecision;
    SQLPOINTER       sScale;
    SQLPOINTER       sDataPtr = NULL;

    sPrecision = (SQLPOINTER)(SQLLEN)aPrecision;
    sScale     = (SQLPOINTER)(SQLLEN)aScale;

    sReturn = SQLGetStmtAttr( aStmtHandle,
                              aDescType,
                              &sDesc,
                              SQL_IS_POINTER,
                              NULL );

    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_STMT,
                           aStmtHandle,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);

    sReturn = SQLGetDescField( sDesc,
                               aColumnIdx,
                               SQL_DESC_DATA_PTR,
                               (SQLPOINTER)&sDataPtr,
                               0,
                               NULL );

    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_DESC,
                           sDesc,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);

    sReturn = SQLSetDescField( sDesc,
                               aColumnIdx,
                               SQL_DESC_PRECISION,
                               (SQLPOINTER)sPrecision,
                               0 );

    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_DESC,
                           sDesc,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);

    sReturn = SQLSetDescField( sDesc,
                               aColumnIdx,
                               SQL_DESC_SCALE,
                               (SQLPOINTER)sScale,
                               0 );

    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_DESC,
                           sDesc,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);

    sReturn = SQLSetDescField( sDesc,
                               aColumnIdx,
                               SQL_DESC_DATA_PTR,
                               sDataPtr,
                               0 );

    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_DESC,
                           sDesc,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief  Host 변수가 char type일 경우, column size, buffer length을 설정한다.
 * @param[in]  aSqlArgs          SQL arguments
 * @param[in]  aStmtHandle       Statement Handle
 * @param[in]  aColumnIdx        Column Index (1-based)
 * @param[in]  aBufLen           Buffer Length
 * @remarks
 */
stlStatus zlpbSetCharBufLen(zlplSqlArgs *aSqlArgs,
                            SQLHANDLE    aStmtHandle,
                            stlInt32     aColumnIdx,
                            SQLLEN       aBufLen)
{
    SQLRETURN        sReturn;
    SQLHDESC         sIpdDesc;
    SQLHDESC         sApdDesc;
    SQLPOINTER       sDataPtr = NULL;

    sReturn = SQLGetStmtAttr( aStmtHandle,
                              SQL_ATTR_IMP_PARAM_DESC,
                              &sIpdDesc,
                              SQL_IS_POINTER,
                              NULL );

    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_STMT,
                           aStmtHandle,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);

    sReturn = SQLSetDescField( sIpdDesc,
                               aColumnIdx,
                               SQL_DESC_LENGTH,
                               (SQLPOINTER)aBufLen,
                               0 );

    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_DESC,
                           sIpdDesc,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);

    sDataPtr = NULL;
    sReturn = SQLSetDescField( sIpdDesc,
                               aColumnIdx,
                               SQL_DESC_DATA_PTR,
                               sDataPtr,
                               0 );

    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_DESC,
                           sIpdDesc,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);

   sReturn = SQLGetStmtAttr( aStmtHandle,
                              SQL_ATTR_APP_PARAM_DESC,
                              &sApdDesc,
                              SQL_IS_POINTER,
                              NULL );

    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_STMT,
                           aStmtHandle,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);

    sReturn = SQLGetDescField( sApdDesc,
                               aColumnIdx,
                               SQL_DESC_DATA_PTR,
                               (SQLPOINTER)&sDataPtr,
                               0,
                               NULL );
        
    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_DESC,
                           sApdDesc,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);

    sReturn = SQLSetDescField( sApdDesc,
                               aColumnIdx,
                               SQL_DESC_LENGTH,
                               (SQLPOINTER)aBufLen,
                               0 );

    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_DESC,
                           sApdDesc,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);

    sReturn = SQLSetDescField( sApdDesc,
                               aColumnIdx,
                               SQL_DESC_DATA_PTR,
                               sDataPtr,
                               0 );

    STL_TRY(zlpeCheckError(sReturn,
                           SQL_HANDLE_DESC,
                           sApdDesc,
                           (zlplSqlca *)aSqlArgs->sql_ca,
                           (stlChar *)aSqlArgs->sql_state)
            == STL_SUCCESS);
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief  SQLExecute 수행후 SQLBindParameter 에 포함되는 Host Variable 의 값들에 대한 길이 정보를 설정한다.
 * @param[in]  aSqlContext       SQL Context
 * @param[in]  aSqlArgs          SQL arguments
 * @param[in]  aStmtHandle       Statement Handle
 * @param[in]  aUnsafeNull       --unsafe-null option 사용 여부
 * @param[in]  aHostVarCount     Host Variable Count
 * @param[in]  aHostVar          Host Variable Array
 * @param[in]  aArraySize        Array Size
 * @param[in]  aValueBuf         Value Buffer
 * @param[in]  aSymbolIndArray   Symbol 이 소유한 Indicator Array
 * @param[in]  aDynParamIOType   Dynamic Parameter 의 I/O Type (nullable)
 * @remarks
 */
stlStatus zlpbSetAfterInfo( zlplSqlContext   * aSqlContext,
                            zlplSqlArgs      * aSqlArgs,
                            SQLHANDLE          aStmtHandle,
                            stlInt32           aUnsafeNull,
                            stlInt32           aHostVarCount,
                            zlplSqlHostVar   * aHostVar,
                            stlInt32           aArraySize,
                            stlChar         ** aValueBuf,
                            SQLLEN          ** aSymbolIndArray,
                            zlplParamIOType  * aDynParamIOType )
{
    zlplVarchar     *sVarchar;
    stlChar         *sIndPtr;
    SQLPOINTER       sValuePtr;
    SQLRETURN        sReturn;
    stlInt32         sArrayIdx;
    stlInt32         i;

    stlInt16         sIOType = 0;

    stlChar         *sValueBase;
    stlInt32         sCopySize;

    if( aArraySize > 1 )
    {
        sReturn = SQLSetStmtAttr(aStmtHandle,
                                 SQL_ATTR_ATOMIC_EXECUTION,
                                 (SQLPOINTER)SQL_ATOMIC_EXECUTION_OFF,
                                 0);
        STL_TRY(zlpeCheckError(sReturn,
                               SQL_HANDLE_STMT,
                               aStmtHandle,
                               (zlplSqlca *)aSqlArgs->sql_ca,
                               (stlChar *)aSqlArgs->sql_state)
                == STL_SUCCESS);
    }

    for(i = 0; i < aHostVarCount; i ++)
    {
        if ( aDynParamIOType == NULL )
        {
            sIOType = aHostVar[i].io_type;
        }
        else
        {
            sIOType = aDynParamIOType[i];
        }

        if( (sIOType == ZLPL_PARAM_IO_TYPE_OUT) ||
            (sIOType == ZLPL_PARAM_IO_TYPE_INOUT) )
        {
            sValueBase = aValueBuf[i];
            sCopySize = zlpbGetDataBufferLength( &aHostVar[i] );

            for( sArrayIdx = 0; sArrayIdx < aArraySize; sArrayIdx ++ )
            {
                /*
                 * 내부 Value buffer의 내용을 사용자 영역으로 copy
                 *   : aValueBuf[i] -> aHostVar[i].value
                 */

                sValuePtr = aHostVar[i].value;
                sValuePtr = (stlChar*)sValuePtr + (aHostVar[i].el_size * sArrayIdx);

                if(aHostVar[i].data_type == ZLPL_C_DATATYPE_VARCHAR)
                {
                    sVarchar = (zlplVarchar *)sValuePtr;
                    stlMemcpy( sVarchar->mArr, sValueBase, sCopySize );
                    sVarchar->mLen = stlStrlen(sVarchar->mArr);
                }
                else if(aHostVar[i].data_type == ZLPL_C_DATATYPE_VARBINARY)
                {
                    sVarchar = (zlplVarchar *)sValuePtr;
                    stlMemcpy( sVarchar->mArr, sValueBase, sCopySize );
                    sVarchar->mLen = aSymbolIndArray[i][sArrayIdx];
                }
                else
                {
                    stlMemcpy( sValuePtr, sValueBase, sCopySize );
                }
                sValueBase += sCopySize;

                if(aHostVar[i].indicator != NULL)
                {
                    sIndPtr = aHostVar[i].indicator;
                    sIndPtr += sArrayIdx * aHostVar[i].ind_el_size;
                    switch(aHostVar[i].ind_type)
                    {
                        case ZLPL_C_DATATYPE_SLONGLONG:
                        case ZLPL_C_DATATYPE_SLONG:
                            *((stlInt64 *)sIndPtr) = (stlInt64) aSymbolIndArray[i][sArrayIdx];
                            break;
                        case ZLPL_C_DATATYPE_SINT:
                            *((stlInt32 *)sIndPtr) = (stlInt32) aSymbolIndArray[i][sArrayIdx];
                            break;
                        case ZLPL_C_DATATYPE_SSHORT:
                            *((stlInt16 *)sIndPtr) = (stlInt16) aSymbolIndArray[i][sArrayIdx];
                            break;
                        case ZLPL_C_DATATYPE_SCHAR:
                            *((stlInt8 *)sIndPtr) = (stlInt8) aSymbolIndArray[i][sArrayIdx];
                            break;
                        case ZLPL_C_DATATYPE_ULONGLONG:
                        case ZLPL_C_DATATYPE_ULONG:
                            *((stlUInt64 *)sIndPtr) = (stlUInt64) aSymbolIndArray[i][sArrayIdx];
                            break;
                        case ZLPL_C_DATATYPE_UINT:
                            *((stlUInt32 *)sIndPtr) = (stlUInt32) aSymbolIndArray[i][sArrayIdx];
                            break;
                        case ZLPL_C_DATATYPE_USHORT:
                            *((stlInt16 *)sIndPtr) = (stlInt16) aSymbolIndArray[i][sArrayIdx];
                            break;
                        case ZLPL_C_DATATYPE_UCHAR:
                            *((stlUInt8 *)sIndPtr) = (stlUInt8) aSymbolIndArray[i][sArrayIdx];
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    /**
                     * null value 라면 indicator 가 존재해야만 한다.
                     */

                    if ( (aUnsafeNull == 1)
                         || (aHostVar[i].data_type == ZLPL_C_DATATYPE_VARCHAR)
                         || (aHostVar[i].data_type == ZLPL_C_DATATYPE_VARBINARY) )
                    {
                        /**
                         * --usafe-null 옵션을 사용하여 pre-compile 한 경우나 E-SQL VARCHAR type 인 경우,
                         * indicator 가 존재하지 않아도 에러를 발생시키지 않는다.
                         */
                    }
                    else
                    {
                        STL_TRY_THROW( aSymbolIndArray[i][sArrayIdx] != SQL_NULL_DATA, RAMP_NULL_VALUE_NO_INDICATOR );
                    }
                }
            }
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_NULL_VALUE_NO_INDICATOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLPL_ERRCODE_NULL_VALUE_NO_INDICATOR_PARAMETER,
                      NULL,
                      ZLPL_CTXT_ERROR_STACK(aSqlContext) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
