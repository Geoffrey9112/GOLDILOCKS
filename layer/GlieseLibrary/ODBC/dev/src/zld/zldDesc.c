/*******************************************************************************
 * zldDesc.c
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

#include <cml.h>
#include <goldilocks.h>

#include <zlDef.h>
#include <zle.h>
#include <zld.h>

#include <zlvSqlToC.h>
#include <zlvCToSql.h>

#include <zlad.h>
#include <zlcd.h>

/**
 * @file zldDesc.c
 * @brief Gliese API Internal Desc Routines.
 */

/**
 * @addtogroup zld
 * @{
 */

#if defined( ODBC_ALL )
stlStatus (*gZldDescBuildResultFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                              zlsStmt       * aStmt,
                                                              stlBool         aNeedResultCols,
                                                              stlErrorStack * aErrorStack ) =
{
    zladDescBuildResult,
    zlcdDescBuildResult
};
#endif

stlStatus zldDescCheckConsistency( zldDesc        * aDesc,
                                   zldDescElement * aDescRec,
                                   stlErrorStack  * aErrorStack )
{
    switch( aDesc->mDescType )
    {
        case ZLD_DESC_TYPE_ARD :
        case ZLD_DESC_TYPE_APD :
            switch( aDescRec->mConsiceType )
            {
                case SQL_C_DEFAULT :
                    break;
                case SQL_C_CHAR :
                case SQL_C_WCHAR :
                case SQL_C_SSHORT :
                case SQL_C_USHORT :
                case SQL_C_SHORT :
                case SQL_C_SLONG :
                case SQL_C_ULONG :
                case SQL_C_LONG :
                case SQL_C_FLOAT :
                case SQL_C_DOUBLE :
                case SQL_C_BIT :
                case SQL_C_STINYINT :
                case SQL_C_UTINYINT :
                case SQL_C_TINYINT :
                case SQL_C_SBIGINT :
                case SQL_C_UBIGINT :
                case SQL_C_BINARY :
                case SQL_C_NUMERIC :
                case SQL_C_BOOLEAN :
                case SQL_C_LONGVARCHAR :
                case SQL_C_LONGVARBINARY :
                    STL_TRY_THROW( aDescRec->mType == aDescRec->mConsiceType,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_TYPE_DATE :
                    STL_TRY_THROW( aDescRec->mType == SQL_DATETIME,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_DATE,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_TYPE_TIME :
                    STL_TRY_THROW( aDescRec->mType == SQL_DATETIME,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_TIME,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_TYPE_TIME_WITH_TIMEZONE :
                    STL_TRY_THROW( aDescRec->mType == SQL_DATETIME,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_TIME_WITH_TIMEZONE,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_TYPE_TIMESTAMP :
                    STL_TRY_THROW( aDescRec->mType == SQL_DATETIME,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_TIMESTAMP,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    STL_TRY_THROW( aDescRec->mType == SQL_DATETIME,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_TIMESTAMP_WITH_TIMEZONE,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_INTERVAL_YEAR :
                    STL_TRY_THROW( aDescRec->mType == SQL_INTERVAL,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_YEAR,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_INTERVAL_MONTH :
                    STL_TRY_THROW( aDescRec->mType == SQL_INTERVAL,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_MONTH,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_INTERVAL_DAY :
                    STL_TRY_THROW( aDescRec->mType == SQL_INTERVAL,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_DAY,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_INTERVAL_HOUR :
                    STL_TRY_THROW( aDescRec->mType == SQL_INTERVAL,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_HOUR,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_INTERVAL_MINUTE :
                    STL_TRY_THROW( aDescRec->mType == SQL_INTERVAL,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_MINUTE,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_INTERVAL_SECOND :
                    STL_TRY_THROW( aDescRec->mType == SQL_INTERVAL,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_SECOND,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_INTERVAL_YEAR_TO_MONTH :
                    STL_TRY_THROW( aDescRec->mType == SQL_INTERVAL,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_YEAR_TO_MONTH,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_INTERVAL_DAY_TO_HOUR :
                    STL_TRY_THROW( aDescRec->mType == SQL_INTERVAL,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_DAY_TO_HOUR,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_INTERVAL_DAY_TO_MINUTE :
                    STL_TRY_THROW( aDescRec->mType == SQL_INTERVAL,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_DAY_TO_MINUTE,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_INTERVAL_DAY_TO_SECOND :
                    STL_TRY_THROW( aDescRec->mType == SQL_INTERVAL,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_DAY_TO_SECOND,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                    STL_TRY_THROW( aDescRec->mType == SQL_INTERVAL,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_HOUR_TO_MINUTE,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_INTERVAL_HOUR_TO_SECOND :
                    STL_TRY_THROW( aDescRec->mType == SQL_INTERVAL,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_HOUR_TO_SECOND,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                    STL_TRY_THROW( aDescRec->mType == SQL_INTERVAL,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_MINUTE_TO_SECOND,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                default :
                    STL_THROW( RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION1 );
                    break;
            }
            break;
        case ZLD_DESC_TYPE_IPD :
            switch( aDescRec->mType )
            {
                case SQL_CHAR:
                case SQL_BINARY:
                case SQL_VARCHAR:
                case SQL_VARBINARY:
                case SQL_LONGVARCHAR:
                case SQL_LONGVARBINARY:
                case SQL_SMALLINT:
                case SQL_INTEGER:
                case SQL_REAL:
                case SQL_DOUBLE:
                case SQL_BIT:
                case SQL_BOOLEAN:
                case SQL_TINYINT:
                case SQL_BIGINT:
                case SQL_ROWID :            
                    STL_TRY_THROW( aDescRec->mType == aDescRec->mConsiceType,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
                case SQL_FLOAT:
                    STL_TRY_THROW( aDescRec->mType == aDescRec->mConsiceType,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    if( aDesc->mDescType == ZLD_DESC_TYPE_IPD )
                    {
                        STL_TRY_THROW( (aDescRec->mPrecision >= DTL_FLOAT_MIN_PRECISION) &&
                                       (aDescRec->mPrecision <= DTL_FLOAT_MAX_PRECISION),
                                       RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2);
                    }
                    break;
                case SQL_NUMERIC:
                    STL_TRY_THROW( aDescRec->mType == aDescRec->mConsiceType,
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );

                    STL_TRY_THROW( (aDescRec->mPrecision >= DTL_NUMERIC_MIN_PRECISION) &&
                                   (aDescRec->mPrecision <= DTL_NUMERIC_MAX_PRECISION),
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2);

                    STL_TRY_THROW( (aDescRec->mScale >= DTL_NUMERIC_MIN_SCALE) &&
                                   (aDescRec->mScale <= DTL_NUMERIC_MAX_SCALE),
                                   RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2);
                    break;
                case SQL_INTERVAL:
                    switch( aDescRec->mConsiceType )
                    {
                        case SQL_INTERVAL_DAY :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_DAY,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_INTERVAL_DAY_TO_HOUR :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_DAY_TO_HOUR,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_INTERVAL_DAY_TO_MINUTE :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_DAY_TO_MINUTE,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_INTERVAL_MONTH :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_MONTH,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_INTERVAL_YEAR :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_YEAR,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_INTERVAL_YEAR_TO_MONTH :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_YEAR_TO_MONTH,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_INTERVAL_HOUR :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_HOUR,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_INTERVAL_MINUTE :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_MINUTE,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_INTERVAL_HOUR_TO_MINUTE :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_HOUR_TO_MINUTE,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_INTERVAL_DAY_TO_SECOND :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_DAY_TO_SECOND,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_INTERVAL_SECOND :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_SECOND,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_INTERVAL_HOUR_TO_SECOND :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_HOUR_TO_SECOND,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_INTERVAL_MINUTE_TO_SECOND :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_MINUTE_TO_SECOND,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        default :
                            STL_THROW( RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION1 );
                            break;
                    }

                    switch( aDescRec->mConsiceType )
                    {
                        case SQL_INTERVAL_DAY_TO_SECOND :
                        case SQL_INTERVAL_SECOND :
                        case SQL_INTERVAL_HOUR_TO_SECOND :
                        case SQL_INTERVAL_MINUTE_TO_SECOND :
                            STL_TRY_THROW( (aDescRec->mPrecision >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                                           (aDescRec->mPrecision <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION),
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                        case SQL_INTERVAL_DAY :
                        case SQL_INTERVAL_DAY_TO_HOUR :
                        case SQL_INTERVAL_DAY_TO_MINUTE :
                        case SQL_INTERVAL_MONTH :
                        case SQL_INTERVAL_YEAR :
                        case SQL_INTERVAL_YEAR_TO_MONTH :
                        case SQL_INTERVAL_HOUR :
                        case SQL_INTERVAL_MINUTE :
                        case SQL_INTERVAL_HOUR_TO_MINUTE :
                            STL_TRY_THROW( (aDescRec->mDatetimeIntervalPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                                           (aDescRec->mDatetimeIntervalPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                        
                        default :
                            break;
                    }
                    break;
                case SQL_DATETIME :
                    switch( aDescRec->mConsiceType )
                    {
                        case SQL_TYPE_DATE :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_DATE,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_TYPE_TIME :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_TIME,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_TYPE_TIME_WITH_TIMEZONE :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_TIME_WITH_TIMEZONE,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_TYPE_TIMESTAMP :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_TIMESTAMP,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
                            STL_TRY_THROW( aDescRec->mDatetimeIntervalCode == SQL_CODE_TIMESTAMP_WITH_TIMEZONE,
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        default :
                            STL_THROW( RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION1 );
                            break;
                    }

                    switch( aDescRec->mConsiceType )
                    {
                        case SQL_TYPE_TIME :
                            STL_TRY_THROW( (aDescRec->mPrecision >= DTL_TIME_MIN_PRECISION) &&
                                           (aDescRec->mPrecision <= DTL_TIME_MAX_PRECISION),
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_TYPE_TIME_WITH_TIMEZONE :
                            STL_TRY_THROW( (aDescRec->mPrecision >= DTL_TIMETZ_MIN_PRECISION) &&
                                           (aDescRec->mPrecision <= DTL_TIMETZ_MAX_PRECISION),
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_TYPE_TIMESTAMP :
                            STL_TRY_THROW( (aDescRec->mPrecision >= DTL_TIMESTAMP_MIN_PRECISION) &&
                                           (aDescRec->mPrecision <= DTL_TIMESTAMP_MAX_PRECISION),
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
                            STL_TRY_THROW( (aDescRec->mPrecision >= DTL_TIMESTAMPTZ_MIN_PRECISION) &&
                                           (aDescRec->mPrecision <= DTL_TIMESTAMPTZ_MAX_PRECISION),
                                           RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                            break;
                        default :
                            break;
                    }
                    break;
                default :
                    STL_THROW( RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION1 );
                    break;
            }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INCONSISTENT_DESCRIPTOR_INFORMATION,
                      "The Type field, or any other field associated with the "
                      "SQL_DESC_TYPE field in the descriptor, was not valid or consistent.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INCONSISTENT_DESCRIPTOR_INFORMATION,
                      "Descriptor information checked during a consistency check was "
                      "not consistent.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus zldDescCheckParameterArgs( stlInt16        aParameterType,
                                            stlInt64        aColumnSize,
                                            stlInt16        aDecimalDigits,
                                            stlErrorStack * aErrorStack )
{
    /*
     * check Parameter ColumnSize/DecimalDigits
     */
    
    switch( aParameterType )
    {
        case SQL_CHAR :
            STL_TRY_THROW( ( aColumnSize > 0 ) &&
                           ( aColumnSize <= DTL_CHAR_MAX_PRECISION ),
                           RAMP_ERR_INVALID_PRECISION_OR_SCALE_VALUE );
            break;
        case SQL_VARCHAR :
            STL_TRY_THROW( ( aColumnSize > 0 ) &&
                           ( aColumnSize <= DTL_VARCHAR_MAX_PRECISION ),
                           RAMP_ERR_INVALID_PRECISION_OR_SCALE_VALUE );
            break;
        case SQL_WCHAR :
        case SQL_WVARCHAR :
        case SQL_DECIMAL :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
            break;
        case SQL_NUMERIC :
            STL_TRY_THROW( ( aColumnSize >= DTL_NUMERIC_MIN_PRECISION ) &&
                           ( aColumnSize <= DTL_NUMERIC_MAX_PRECISION ),
                           RAMP_ERR_INVALID_PRECISION_OR_SCALE_VALUE );

            STL_TRY_THROW( ( aDecimalDigits >= DTL_NUMERIC_MIN_SCALE ) &&
                           ( aDecimalDigits <= DTL_NUMERIC_MAX_SCALE ),
                           RAMP_ERR_INVALID_PRECISION_OR_SCALE_VALUE );
            break;
        case SQL_FLOAT :
            STL_TRY_THROW( ( aColumnSize >= DTL_FLOAT_MIN_PRECISION ) &&
                           ( aColumnSize <= DTL_FLOAT_MAX_PRECISION ),
                           RAMP_ERR_INVALID_PRECISION_OR_SCALE_VALUE );
            break;
        case SQL_BINARY :
            STL_TRY_THROW( ( aColumnSize > 0 ) &&
                           ( aColumnSize <= DTL_BINARY_MAX_PRECISION ),
                           RAMP_ERR_INVALID_PRECISION_OR_SCALE_VALUE );
            break;
        case SQL_VARBINARY :
            STL_TRY_THROW( ( aColumnSize > 0 ) &&
                           ( aColumnSize <= DTL_VARBINARY_MAX_PRECISION ),
                           RAMP_ERR_INVALID_PRECISION_OR_SCALE_VALUE );
            break;
        case SQL_TYPE_TIME :
            STL_TRY_THROW( ( aDecimalDigits >= DTL_TIME_MIN_PRECISION ) &&
                           ( aDecimalDigits <= DTL_TIME_MAX_PRECISION ),
                           RAMP_ERR_INVALID_PRECISION_OR_SCALE_VALUE );
            break;
        case SQL_TYPE_TIME_WITH_TIMEZONE :
            STL_TRY_THROW( ( aDecimalDigits >= DTL_TIMETZ_MIN_PRECISION ) &&
                           ( aDecimalDigits <= DTL_TIMETZ_MAX_PRECISION ),
                           RAMP_ERR_INVALID_PRECISION_OR_SCALE_VALUE );
            break;
        case SQL_TYPE_TIMESTAMP :
            STL_TRY_THROW( ( aDecimalDigits >= DTL_TIMESTAMP_MIN_PRECISION ) &&
                           ( aDecimalDigits <= DTL_TIMESTAMP_MAX_PRECISION ),
                           RAMP_ERR_INVALID_PRECISION_OR_SCALE_VALUE );
            break;
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
            STL_TRY_THROW( ( aDecimalDigits >= DTL_TIMESTAMPTZ_MIN_PRECISION ) &&
                           ( aDecimalDigits <= DTL_TIMESTAMPTZ_MAX_PRECISION ),
                           RAMP_ERR_INVALID_PRECISION_OR_SCALE_VALUE );
            break;
        case SQL_INTERVAL_SECOND :
        case SQL_INTERVAL_DAY_TO_SECOND :
        case SQL_INTERVAL_HOUR_TO_SECOND :
        case SQL_INTERVAL_MINUTE_TO_SECOND :
            STL_TRY_THROW( ( aDecimalDigits >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION ) &&
                           ( aDecimalDigits <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION ),
                           RAMP_ERR_INVALID_PRECISION_OR_SCALE_VALUE );
            break;
        default :
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_PRECISION_OR_SCALE_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The value specified for the argument ColumnSize or DecimalDigits "
                      "was outside the range of values supported by the data source "
                      "for a column of the SQL data type specified by the ParameterType "
                      "argument.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The value specified for the argument ParameterType was "
                      "a valid ODBC SQL data type identifier for the version of "
                      "ODBC supported by the driver but was not supported by the driver or data source.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zldDescAllocRec( zldDesc         * aDesc,
                           stlUInt16         aRecNumber,
                           zldDescElement ** aDescRec,
                           stlErrorStack   * aErrorStack )
{
    zldDescElement * sDescRec = NULL;
    stlInt32         sIdx;

    for( sIdx = aDesc->mCount; sIdx < aRecNumber; sIdx++ )
    {
        STL_TRY( stlAllocHeap( (void**)&sDescRec,
                               STL_SIZEOF( zldDescElement ),
                               aErrorStack ) == STL_SUCCESS );
        stlMemset( sDescRec, 0x00, STL_SIZEOF( zldDescElement ) );
                     
        STL_RING_INIT_DATALINK( sDescRec, STL_OFFSETOF( zldDescElement, mLink ) );
        STL_RING_ADD_LAST( &aDesc->mDescRecRing, sDescRec );

        STL_TRY( stlCreateRegionAllocator( &sDescRec->mAllocator,
                                           ZLD_DEFAULT_MEMORY_SIZE,
                                           ZLD_DEFAULT_MEMORY_SIZE,
                                           aErrorStack ) == STL_SUCCESS );

        sDescRec->mSqlToCFunc       = zlvInvalidSqlToC;
        sDescRec->mCtoSqlFunc       = zlvInvalidCtoSql;
        sDescRec->mParameterType    = SQL_PARAM_TYPE_UNKNOWN;
        sDescRec->mNullable         = SQL_NULLABLE_UNKNOWN;
        sDescRec->mUpdatable        = SQL_ATTR_READWRITE_UNKNOWN;
        sDescRec->mInputValueBlock  = NULL;
        sDescRec->mOutputValueBlock = NULL;
        sDescRec->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;

        aDesc->mCount++;
    }

    if( aDescRec != NULL)
    {
        *aDescRec = sDescRec;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zldDescReallocRec(zldDesc       * aDesc,
                            stlUInt16       aRecNumber,
                            stlErrorStack * aErrorStack )
{
    zldDescElement    * sRec;
    zldDescElement    * sNext;
    stlInt32            sIdx = 0;
    dtlValueBlockList * sBlockList  = NULL;
    dtlDataValue      * sDataValue  = NULL;
    dtlDataType         sDtlDataType;
    stlUInt64           sArrayIdx;
    stlInt32            i;
    
    if( aRecNumber >= aDesc->mCount )
    {
        STL_TRY( zldDescAllocRec( aDesc,
                                  aRecNumber,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_RING_FOREACH_ENTRY_SAFE( &aDesc->mDescRecRing,
                                     sRec,
                                     sNext )
        {
            sIdx++;
            if( sIdx <= aRecNumber )
            {
                continue;
            }

            STL_RING_UNLINK( &aDesc->mDescRecRing,
                             sRec );
            aDesc->mCount--;

            if( sRec->mBaseColumnName != NULL )
            {
                stlFreeHeap( sRec->mBaseColumnName );
                sRec->mBaseColumnName = NULL;
            }

            if( sRec->mBaseTableName != NULL )
            {
                stlFreeHeap( sRec->mBaseTableName );
                sRec->mBaseTableName = NULL;
            }

            if( sRec->mCatalogName != NULL )
            {
                stlFreeHeap( sRec->mCatalogName );
                sRec->mCatalogName = NULL;
            }

            if( sRec->mLabel != NULL )
            {
                stlFreeHeap( sRec->mLabel );
                sRec->mLabel = NULL;
            }

            if( sRec->mLiteralPrefix != NULL )
            {
                stlFreeHeap( sRec->mLiteralPrefix );
                sRec->mLiteralPrefix = NULL;
            }

            if( sRec->mLiteralSuffix != NULL )
            {
                stlFreeHeap( sRec->mLiteralSuffix );
                sRec->mLiteralSuffix = NULL;
            }

            if( sRec->mLocalTypeName != NULL )
            {
                stlFreeHeap( sRec->mLocalTypeName );
                sRec->mLocalTypeName = NULL;
            }

            if( sRec->mName != NULL )
            {
                stlFreeHeap( sRec->mName );
                sRec->mName = NULL;
            }

            if( sRec->mSchemaName != NULL )
            {
                stlFreeHeap( sRec->mSchemaName );
                sRec->mSchemaName = NULL;
            }

            if( sRec->mTableName != NULL )
            {
                stlFreeHeap( sRec->mTableName );
                sRec->mTableName = NULL;
            }

            if( sRec->mTypeName != NULL )
            {
                stlFreeHeap( sRec->mTypeName );
                sRec->mTypeName = NULL;
            }

            if( sRec->mPutDataBuffer != NULL )
            {
                for( sArrayIdx = 0; sArrayIdx < aDesc->mPutDataArraySize; sArrayIdx++ )
                {
                    if( sRec->mPutDataBuffer[sArrayIdx] != NULL )
                    {
                        STL_TRY( stlFreeDynamicMem( &aDesc->mStmt->mLongVaryingMem,
                                                    sRec->mPutDataBuffer[sArrayIdx],
                                                    aErrorStack ) == STL_SUCCESS );

                        sRec->mPutDataBuffer[sArrayIdx] = NULL;
                    }
                }

                STL_TRY( stlFreeDynamicMem( &aDesc->mStmt->mLongVaryingMem,
                                            sRec->mPutDataBuffer,
                                            aErrorStack ) == STL_SUCCESS );

                sRec->mPutDataBuffer = NULL;
                aDesc->mPutDataArraySize = 0;
            }

            if( sRec->mPutDataIndicator != NULL )
            {
                STL_TRY( stlFreeDynamicMem( &aDesc->mStmt->mLongVaryingMem,
                                            sRec->mPutDataIndicator,
                                            aErrorStack ) == STL_SUCCESS );

                sRec->mPutDataIndicator = NULL;
            }

            if( sRec->mPutDataBufferSize != NULL )
            {
                STL_TRY( stlFreeDynamicMem( &aDesc->mStmt->mLongVaryingMem,
                                            sRec->mPutDataBufferSize,
                                            aErrorStack ) == STL_SUCCESS );

                sRec->mPutDataBufferSize = NULL;
            }

            /**
             * Input Value Block만 ODBC에서 책임진다.
             */
            
            sBlockList = sRec->mInputValueBlock;
    
            if( sRec->mInputValueBlock != NULL )
            {
                sDtlDataType = DTL_GET_BLOCK_DB_TYPE( sBlockList );
                
                if( (sDtlDataType == DTL_TYPE_LONGVARCHAR) ||
                    (sDtlDataType == DTL_TYPE_LONGVARBINARY) )
                {
                    sDataValue = sBlockList->mValueBlock->mDataValue;
                
                    for( i = 0; i < DTL_GET_BLOCK_ALLOC_CNT(sBlockList); i++ )
                    {
                        STL_TRY( stlFreeDynamicMem( &aDesc->mStmt->mLongVaryingMem,
                                                    sDataValue[i].mValue,
                                                    aErrorStack )
                                 == STL_SUCCESS );
                    }
                }
            }

            sRec->mInputValueBlock  = NULL;
            sRec->mOutputValueBlock = NULL;

            STL_TRY( stlDestroyRegionAllocator( &sRec->mAllocator,
                                                aErrorStack ) == STL_SUCCESS );

            stlFreeHeap( sRec );
        }

        STL_DASSERT( aDesc->mCount == aRecNumber );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;   
}

stlStatus zldDescFreeRecs( zldDesc       * aDesc,
                           stlErrorStack * aErrorStack )
{
    zldDescElement    * sRec;
    zldDescElement    * sNext;
    dtlValueBlockList * sBlockList  = NULL;
    dtlDataValue      * sDataValue  = NULL;
    dtlDataType         sDtlDataType;
    stlUInt64           sArrayIdx;
    stlInt32            i;

    STL_RING_FOREACH_ENTRY_SAFE( &aDesc->mDescRecRing,
                                 sRec,
                                 sNext )
    {
        STL_RING_UNLINK( &aDesc->mDescRecRing,
                         sRec );
        aDesc->mCount--;

        if( sRec->mBaseColumnName != NULL )
        {
            stlFreeHeap( sRec->mBaseColumnName );
            sRec->mBaseColumnName = NULL;
        }

        if( sRec->mBaseTableName != NULL )
        {
            stlFreeHeap( sRec->mBaseTableName );
            sRec->mBaseTableName = NULL;
        }

        if( sRec->mCatalogName != NULL )
        {
            stlFreeHeap( sRec->mCatalogName );
            sRec->mCatalogName = NULL;
        }

        if( sRec->mLabel != NULL )
        {
            stlFreeHeap( sRec->mLabel );
            sRec->mLabel = NULL;
        }

        if( sRec->mLiteralPrefix != NULL )
        {
            stlFreeHeap( sRec->mLiteralPrefix );
            sRec->mLiteralPrefix = NULL;
        }

        if( sRec->mLiteralSuffix != NULL )
        {
            stlFreeHeap( sRec->mLiteralSuffix );
            sRec->mLiteralSuffix = NULL;
        }

        if( sRec->mLocalTypeName != NULL )
        {
            stlFreeHeap( sRec->mLocalTypeName );
            sRec->mLocalTypeName = NULL;
        }

        if( sRec->mName != NULL )
        {
            stlFreeHeap( sRec->mName );
            sRec->mName = NULL;
        }

        if( sRec->mSchemaName != NULL )
        {
            stlFreeHeap( sRec->mSchemaName );
            sRec->mSchemaName = NULL;
        }

        if( sRec->mTableName != NULL )
        {
            stlFreeHeap( sRec->mTableName );
            sRec->mTableName = NULL;
        }

        if( sRec->mTypeName != NULL )
        {
            stlFreeHeap( sRec->mTypeName );
            sRec->mTypeName = NULL;
        }

        if( sRec->mPutDataBuffer != NULL )
        {
            for( sArrayIdx = 0; sArrayIdx < aDesc->mPutDataArraySize; sArrayIdx++ )
            {
                if( sRec->mPutDataBuffer[sArrayIdx] != NULL )
                {
                    STL_TRY( stlFreeDynamicMem( &aDesc->mStmt->mLongVaryingMem,
                                                sRec->mPutDataBuffer[sArrayIdx],
                                                aErrorStack ) == STL_SUCCESS );

                    sRec->mPutDataBuffer[sArrayIdx] = NULL;
                }
            }

            STL_TRY( stlFreeDynamicMem( &aDesc->mStmt->mLongVaryingMem,
                                        sRec->mPutDataBuffer,
                                        aErrorStack ) == STL_SUCCESS );

            sRec->mPutDataBuffer = NULL;
            aDesc->mPutDataArraySize = 0;
        }

        if( sRec->mPutDataIndicator != NULL )
        {
            STL_TRY( stlFreeDynamicMem( &aDesc->mStmt->mLongVaryingMem,
                                        sRec->mPutDataIndicator,
                                        aErrorStack ) == STL_SUCCESS );

            sRec->mPutDataIndicator = NULL;
        }

        if( sRec->mPutDataBufferSize != NULL )
        {
            STL_TRY( stlFreeDynamicMem( &aDesc->mStmt->mLongVaryingMem,
                                        sRec->mPutDataBufferSize,
                                        aErrorStack ) == STL_SUCCESS );

            sRec->mPutDataBufferSize = NULL;
        }

        /**
         * Input Value Block만 ODBC에서 책임진다.
         */
            
        sBlockList = sRec->mInputValueBlock;
    
        if( sRec->mInputValueBlock != NULL )
        {
            sDtlDataType = DTL_GET_BLOCK_DB_TYPE( sBlockList );
                
            if( (sDtlDataType == DTL_TYPE_LONGVARCHAR) ||
                (sDtlDataType == DTL_TYPE_LONGVARBINARY) )
            {
                sDataValue = sBlockList->mValueBlock->mDataValue;
                
                for( i = 0; i < DTL_GET_BLOCK_ALLOC_CNT(sBlockList); i++ )
                {
                    STL_TRY( stlFreeDynamicMem( &aDesc->mStmt->mLongVaryingMem,
                                                sDataValue[i].mValue,
                                                aErrorStack )
                             == STL_SUCCESS );
                }
            }
        }

        sRec->mInputValueBlock  = NULL;
        sRec->mOutputValueBlock = NULL;

        STL_TRY( stlDestroyRegionAllocator( &sRec->mAllocator,
                                            aErrorStack ) == STL_SUCCESS );

        stlFreeHeap( sRec );
    }

    STL_DASSERT( aDesc->mCount == 0 );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zldDescFindRec( zldDesc         * aDesc,
                          stlUInt16         aRecNumber,
                          zldDescElement ** aDescRec )
{
    zldDescElement * sDescRec;
    stlInt32          sIdx;

    sIdx = 1;
    STL_RING_FOREACH_ENTRY( &aDesc->mDescRecRing, sDescRec )
    {
        if( sIdx == aRecNumber )
        {
            break;
        }

        sIdx++;
    }

    STL_TRY( STL_RING_IS_HEADLINK( &aDesc->mDescRecRing,
                                   sDescRec ) == STL_FALSE );

    *aDescRec = sDescRec;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zldDescSetTargetType( zlsStmt        * aStmt,
                                zldDescElement * aDescRec,
                                stlInt16         aTargetType,
                                stlInt64         aBufferLength )
{
    switch( aTargetType )
    {
        case SQL_C_CHAR :
        case SQL_C_WCHAR :
        case SQL_C_BINARY :
        case SQL_C_LONGVARCHAR :
        case SQL_C_LONGVARBINARY :
            aDescRec->mType = aTargetType;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mLength = aBufferLength;
            break;
        case SQL_C_NUMERIC :
            aDescRec->mType = aTargetType;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mPrecision = DTL_NUMERIC_DEFAULT_PRECISION;
            aDescRec->mScale = DTL_NUMERIC_DEFAULT_SCALE;
            break;
        case SQL_C_TYPE_DATE :
            aDescRec->mType = SQL_DATETIME;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_DATE;
            aDescRec->mLength = DTL_DATE_ODBC_COLUMN_SIZE( ZLS_STMT_DT_VECTOR(aStmt), aStmt ); 
            break;
        case SQL_C_TYPE_TIME :
            aDescRec->mType = SQL_DATETIME;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_TIME;
            aDescRec->mPrecision = DTL_TIME_MIN_PRECISION;
            aDescRec->mLength = DTL_TIME_ODBC_COLUMN_SIZE( ZLS_STMT_DT_VECTOR(aStmt), aStmt );
            break;
        case SQL_C_TYPE_TIME_WITH_TIMEZONE :
            aDescRec->mType = SQL_DATETIME;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_TIME_WITH_TIMEZONE;
            aDescRec->mPrecision = DTL_TIMETZ_MAX_PRECISION;
            aDescRec->mLength = DTL_TIME_WITH_TIME_ZONE_ODBC_COLUMN_SIZE( ZLS_STMT_DT_VECTOR(aStmt), aStmt );
            break;
        case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
            aDescRec->mType = SQL_DATETIME;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_TIMESTAMP_WITH_TIMEZONE;
            aDescRec->mPrecision = DTL_TIMESTAMPTZ_MAX_PRECISION;
            aDescRec->mLength = DTL_TIMESTAMP_WITH_TIME_ZONE_ODBC_COLUMN_SIZE( ZLS_STMT_DT_VECTOR(aStmt), aStmt );
            break;
        case SQL_C_TYPE_TIMESTAMP :
            aDescRec->mType = SQL_DATETIME;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_TIMESTAMP;
            aDescRec->mPrecision = DTL_TIMESTAMP_MAX_PRECISION;
            aDescRec->mLength = DTL_TIMESTAMP_ODBC_COLUMN_SIZE( ZLS_STMT_DT_VECTOR(aStmt), aStmt );
            break;
        case SQL_C_INTERVAL_YEAR :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_YEAR;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION;            
            aDescRec->mLength = DTL_INTERVAL_YEAR_ODBC_COLUMN_SIZE( aDescRec->mDatetimeIntervalPrecision );
            break;
        case SQL_C_INTERVAL_MONTH :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_MONTH;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION;            
            aDescRec->mLength = DTL_INTERVAL_MONTH_ODBC_COLUMN_SIZE( aDescRec->mDatetimeIntervalPrecision );
            break;
        case SQL_C_INTERVAL_DAY :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_DAY;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION;            
            aDescRec->mLength = DTL_INTERVAL_DAY_ODBC_COLUMN_SIZE( aDescRec->mDatetimeIntervalPrecision );
            break;
        case SQL_C_INTERVAL_HOUR :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_HOUR;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION;            
            aDescRec->mLength = DTL_INTERVAL_HOUR_ODBC_COLUMN_SIZE( aDescRec->mDatetimeIntervalPrecision );
            break;
        case SQL_C_INTERVAL_MINUTE :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_MINUTE;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION;            
            aDescRec->mLength = DTL_INTERVAL_MINUTE_ODBC_COLUMN_SIZE( aDescRec->mDatetimeIntervalPrecision );
            break;
        case SQL_C_INTERVAL_SECOND :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_SECOND;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION;            
            aDescRec->mLength = DTL_INTERVAL_SECOND_ODBC_COLUMN_SIZE( aDescRec->mDatetimeIntervalPrecision,
                                                                      aDescRec->mPrecision );
            break;
        case SQL_C_INTERVAL_YEAR_TO_MONTH :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_YEAR_TO_MONTH;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION;            
            aDescRec->mLength = DTL_INTERVAL_YEAR_TO_MONTH_ODBC_COLUMN_SIZE( aDescRec->mDatetimeIntervalPrecision );
            break;
        case SQL_C_INTERVAL_DAY_TO_HOUR :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_DAY_TO_HOUR;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION;            
            aDescRec->mLength = DTL_INTERVAL_DAY_TO_HOUR_ODBC_COLUMN_SIZE( aDescRec->mDatetimeIntervalPrecision );
            break;
        case SQL_C_INTERVAL_DAY_TO_MINUTE :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_DAY_TO_MINUTE;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION;            
            aDescRec->mLength = DTL_INTERVAL_DAY_TO_MINUTE_ODBC_COLUMN_SIZE( aDescRec->mDatetimeIntervalPrecision );
            break;
        case SQL_C_INTERVAL_DAY_TO_SECOND :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_DAY_TO_SECOND;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION;            
            aDescRec->mLength = DTL_INTERVAL_DAY_TO_SECOND_ODBC_COLUMN_SIZE( aDescRec->mDatetimeIntervalPrecision,
                                                                             aDescRec->mPrecision );
            break;
        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_HOUR_TO_MINUTE;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION;            
            aDescRec->mLength = DTL_INTERVAL_HOUR_TO_MINUTE_ODBC_COLUMN_SIZE( aDescRec->mDatetimeIntervalPrecision );
            break;
        case SQL_C_INTERVAL_HOUR_TO_SECOND :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_HOUR_TO_SECOND;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION;            
            aDescRec->mLength = DTL_INTERVAL_HOUR_TO_SECOND_ODBC_COLUMN_SIZE( aDescRec->mDatetimeIntervalPrecision,
                                                                              aDescRec->mPrecision );
            break;
        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aTargetType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_MINUTE_TO_SECOND;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION;            
            aDescRec->mLength = DTL_INTERVAL_MINUTE_TO_SECOND_ODBC_COLUMN_SIZE( aDescRec->mDatetimeIntervalPrecision,
                                                                                aDescRec->mPrecision );
            break;
        default :
            aDescRec->mType = aTargetType;
            aDescRec->mConsiceType = aTargetType;
            break;
    }

    return STL_SUCCESS;
}

stlStatus zldDescSetParameterType( zldDescElement * aDescRec,
                                   stlInt16         aParameterType,
                                   stlInt64         aColumnSize,
                                   stlInt16         aDecimalDigits,
                                   stlErrorStack  * aErrorStack )
{
    SQLLEN sNoTotal = SQL_NO_TOTAL;

    STL_TRY( zldDescCheckParameterArgs( aParameterType,
                                        aColumnSize,
                                        aDecimalDigits,
                                        aErrorStack ) == STL_SUCCESS );

    aDescRec->mType                      = SQL_UNKNOWN_TYPE;
    aDescRec->mConsiceType               = SQL_UNKNOWN_TYPE;
    aDescRec->mDatetimeIntervalCode      = 0;
    aDescRec->mDatetimeIntervalPrecision = 0;
    aDescRec->mPrecision                 = 0;            
    aDescRec->mLength                    = 0;
    
    switch( aParameterType )
    {
        case SQL_CHAR :
        case SQL_VARCHAR :
        case SQL_WCHAR :
        case SQL_WVARCHAR :
        case SQL_BINARY :
        case SQL_VARBINARY :
            aDescRec->mType = aParameterType;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mLength = aColumnSize;
            break;
        case SQL_LONGVARCHAR :
        case SQL_LONGVARBINARY :
        case SQL_WLONGVARCHAR :
            aDescRec->mType = aParameterType;
            aDescRec->mConsiceType = aParameterType;
            stlMemcpy(&aDescRec->mLength, &sNoTotal, STL_SIZEOF(SQLLEN));
            break;
        case SQL_TYPE_DATE :
            aDescRec->mType = SQL_DATETIME;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_DATE;
            break;
        case SQL_TYPE_TIME :
            aDescRec->mType = SQL_DATETIME;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_TIME;
            aDescRec->mPrecision = aDecimalDigits;
            break;
        case SQL_TYPE_TIME_WITH_TIMEZONE :
            aDescRec->mType = SQL_DATETIME;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_TIME_WITH_TIMEZONE;
            aDescRec->mPrecision = aDecimalDigits;
            break;
        case SQL_TYPE_TIMESTAMP :
            aDescRec->mType = SQL_DATETIME;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_TIMESTAMP;
            aDescRec->mPrecision = aDecimalDigits;
            break;
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
            aDescRec->mType = SQL_DATETIME;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_TIMESTAMP_WITH_TIMEZONE;
            aDescRec->mPrecision = aDecimalDigits;
            break;
        case SQL_INTERVAL_YEAR :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_YEAR;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            break;
        case SQL_INTERVAL_MONTH :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_MONTH;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            break;
        case SQL_INTERVAL_DAY :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_DAY;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            break;
        case SQL_INTERVAL_HOUR :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_HOUR;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            break;
        case SQL_INTERVAL_MINUTE :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_MINUTE;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            break;
        case SQL_INTERVAL_SECOND :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_SECOND;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = aDecimalDigits;
            break;
        case SQL_INTERVAL_YEAR_TO_MONTH :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_YEAR_TO_MONTH;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            break;
        case SQL_INTERVAL_DAY_TO_HOUR :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_DAY_TO_HOUR;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            break;
        case SQL_INTERVAL_DAY_TO_MINUTE :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_DAY_TO_MINUTE;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            break;
        case SQL_INTERVAL_DAY_TO_SECOND :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_DAY_TO_SECOND;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = aDecimalDigits;
            break;
        case SQL_INTERVAL_HOUR_TO_MINUTE :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_HOUR_TO_MINUTE;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            break;
        case SQL_INTERVAL_HOUR_TO_SECOND :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_HOUR_TO_SECOND;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = aDecimalDigits;
            break;
        case SQL_INTERVAL_MINUTE_TO_SECOND :
            aDescRec->mType = SQL_INTERVAL;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mDatetimeIntervalCode = SQL_CODE_MINUTE_TO_SECOND;
            aDescRec->mDatetimeIntervalPrecision = DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION;
            aDescRec->mPrecision = aDecimalDigits;
            break;
        case SQL_FLOAT :
            aDescRec->mType = aParameterType;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mPrecision = aColumnSize;
            break;
        case SQL_NUMERIC :
            aDescRec->mType = aParameterType;
            aDescRec->mConsiceType = aParameterType;
            aDescRec->mPrecision = aColumnSize;
            aDescRec->mScale = aDecimalDigits;
            break;
        default :
            aDescRec->mType = aParameterType;
            aDescRec->mConsiceType = aParameterType;
            break;
    }

    aDescRec->mChanged = STL_TRUE;
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus zldDescSetRec( zldDesc       * aDesc,
                         stlInt16        aRecNumber,
                         stlInt16        aType,
                         stlInt16        aSubType,
                         stlInt64        aLength,
                         stlInt16        aPrecision,
                         stlInt16        aScale,
                         void          * aDataPtr,
                         SQLLEN        * aStringLength,
                         SQLLEN        * aIndicator,
                         stlErrorStack * aErrorStack )
{
    zldDescElement * sDescRec;

    if( aRecNumber > aDesc->mCount )
    {
        STL_TRY( zldDescAllocRec( aDesc,
                                  aRecNumber,
                                  &sDescRec,
                                  aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( zldDescFindRec( aDesc, aRecNumber, &sDescRec ) == STL_SUCCESS );

        STL_TRY( zldDescCheckConsistency( aDesc,
                                          sDescRec,
                                          aErrorStack ) == STL_SUCCESS );
    }

    sDescRec->mType = aType;

    switch( aType )
    {
        case SQL_DATETIME :
            switch( aSubType )
            {
                case SQL_CODE_DATE :
                    sDescRec->mConsiceType = SQL_TYPE_DATE;
                    break;
                case SQL_CODE_TIME :
                    sDescRec->mConsiceType = SQL_TYPE_TIME;
                    break;
                case SQL_CODE_TIME_WITH_TIMEZONE :
                    sDescRec->mConsiceType = SQL_TYPE_TIME_WITH_TIMEZONE;
                    break;
                case SQL_CODE_TIMESTAMP :
                    sDescRec->mConsiceType = SQL_TYPE_TIMESTAMP;
                    break;
                case SQL_CODE_TIMESTAMP_WITH_TIMEZONE :
                    sDescRec->mConsiceType = SQL_TYPE_TIMESTAMP_WITH_TIMEZONE;
                    break;
                default :
                    STL_THROW( RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION );
                    break;
            }
            break;
        case SQL_INTERVAL :
            switch( aSubType )
            {
                case SQL_CODE_DAY :
                    sDescRec->mConsiceType = SQL_INTERVAL_DAY;
                    break;
                case SQL_CODE_DAY_TO_HOUR :
                    sDescRec->mConsiceType = SQL_INTERVAL_DAY_TO_HOUR;
                    break;
                case SQL_CODE_DAY_TO_MINUTE :
                    sDescRec->mConsiceType = SQL_INTERVAL_DAY_TO_MINUTE;
                    break;
                case SQL_CODE_MONTH :
                    sDescRec->mConsiceType = SQL_INTERVAL_MONTH;
                    break;
                case SQL_CODE_YEAR :
                    sDescRec->mConsiceType = SQL_INTERVAL_YEAR;
                    break;
                case SQL_CODE_YEAR_TO_MONTH :
                    sDescRec->mConsiceType = SQL_INTERVAL_YEAR_TO_MONTH;
                    break;
                case SQL_CODE_HOUR :
                    sDescRec->mConsiceType = SQL_INTERVAL_HOUR;
                    break;
                case SQL_CODE_MINUTE :
                    sDescRec->mConsiceType = SQL_INTERVAL_MINUTE;
                    break;
                case SQL_CODE_HOUR_TO_MINUTE :
                    sDescRec->mConsiceType = SQL_INTERVAL_HOUR_TO_MINUTE;
                    break;
                case SQL_CODE_DAY_TO_SECOND :
                    sDescRec->mConsiceType = SQL_INTERVAL_DAY_TO_SECOND;
                    break;
                case SQL_CODE_SECOND :
                    sDescRec->mConsiceType = SQL_INTERVAL_SECOND;
                    break;
                case SQL_CODE_HOUR_TO_SECOND :
                    sDescRec->mConsiceType = SQL_INTERVAL_HOUR_TO_SECOND;
                    break;
                case SQL_CODE_MINUTE_TO_SECOND :
                    sDescRec->mConsiceType = SQL_INTERVAL_MINUTE_TO_SECOND;
                    break;
                default :
                    STL_THROW( RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION );
                    break;
            }
            break;
        default :
            sDescRec->mConsiceType = aType;
            break;
    }

    sDescRec->mOctetLength = aLength;
    sDescRec->mPrecision = aPrecision;
    sDescRec->mScale = aScale;
    sDescRec->mDataPtr = aDataPtr;
    sDescRec->mOctetLengthPtr = aStringLength;
    sDescRec->mIndicatorPtr = aIndicator;

    if( aDesc->mDescType == ZLD_DESC_TYPE_IPD )
    {
        sDescRec->mChanged = STL_TRUE;
    }

    STL_TRY( zldDescCheckConsistency( aDesc,
                                      sDescRec,
                                      aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INCONSISTENT_DESCRIPTOR_INFORMATION,
                      "The Type field, or any other field associated with the "
                      "SQL_DESC_TYPE field in the descriptor, was not valid or consistent.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;   
}

stlStatus zldDescSetField( zldDesc       * aDesc,
                           stlInt16        aRecNumber,
                           stlInt16        aFieldId,
                           void          * aValue,
                           stlInt32        aBufferLen,
                           stlErrorStack * aErrorStack )
{
    zldDescElement      * sDescRec;
    stlInt16              sInt16;
    dtlStringLengthUnit   sStringLengthUnit;
    stlBool               sUnbind = STL_FALSE;
    stlBool               sCheckConsistency = STL_FALSE;

    switch( aFieldId )
    {
        case SQL_DESC_ARRAY_SIZE :
            STL_ASSERT( aDesc->mDescType != ZLD_DESC_TYPE_APD );

            STL_TRY_THROW( (SQLULEN)(stlVarInt)aValue > 0, RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                
            aDesc->mArraySize = (SQLULEN)(stlVarInt)aValue;
            break;
        case SQL_DESC_ARRAY_STATUS_PTR :
            aDesc->mArrayStatusPtr = (stlUInt16*)aValue;
            break;
        case SQL_DESC_BIND_OFFSET_PTR :
            aDesc->mBindOffsetPtr = (SQLLEN*)aValue;
            break;
        case SQL_DESC_BIND_TYPE :
            aDesc->mBindType = (stlInt32)(stlVarInt)aValue;
            break;
        case SQL_DESC_COUNT :
            STL_TRY( zldDescReallocRec(aDesc,
                                       (stlInt16)(stlVarInt)aValue,
                                       aErrorStack ) == STL_SUCCESS );
            break;
        case SQL_DESC_ROWS_PROCESSED_PTR :
            aDesc->mRowProcessed = (SQLULEN*)aValue;
            break;
        default :
            if( aRecNumber > aDesc->mCount )
            {
                STL_TRY( zldDescAllocRec( aDesc,
                                          aRecNumber,
                                          &sDescRec,
                                          aErrorStack ) == STL_SUCCESS );
            }
            else
            {
                STL_TRY( zldDescFindRec( aDesc, aRecNumber, &sDescRec ) == STL_SUCCESS );
            }

            sUnbind = STL_TRUE;
            
            switch( aFieldId )
            {
                case SQL_DESC_CONCISE_TYPE :
                    sDescRec->mConsiceType = (stlInt16)((stlVarInt)aValue);
                    if( aDesc->mDescType == ZLD_DESC_TYPE_IPD )
                    {
                        sDescRec->mChanged = STL_TRUE;
                    }
                    break;
                case SQL_DESC_DATA_PTR :
                    switch( aDesc->mDescType )
                    {
                        case ZLD_DESC_TYPE_ARD :
                        case ZLD_DESC_TYPE_APD :
                        case ZLD_DESC_TYPE_IRD :
                            sDescRec->mDataPtr = aValue;
                            break;
                        case ZLD_DESC_TYPE_IPD :
                            break;
                        default :
                            STL_DASSERT( STL_FALSE );
                            break;
                    }
                    sUnbind           = STL_FALSE;
                    sCheckConsistency = STL_TRUE;
                    break;
                case SQL_DESC_DATETIME_INTERVAL_CODE :
                    sDescRec->mDatetimeIntervalCode = (stlInt16)((stlVarInt)aValue);
                    if( aDesc->mDescType == ZLD_DESC_TYPE_IPD )
                    {
                        sDescRec->mChanged = STL_TRUE;
                    }
                    break;
                case SQL_DESC_DATETIME_INTERVAL_PRECISION :
                    sDescRec->mDatetimeIntervalPrecision = (stlInt32)((stlVarInt)aValue);
                    if( aDesc->mDescType == ZLD_DESC_TYPE_IPD )
                    {
                        sDescRec->mChanged = STL_TRUE;
                    }
                    break;
                case SQL_DESC_INDICATOR_PTR :
                    sDescRec->mIndicatorPtr = (SQLLEN*)aValue;
                    sUnbind = STL_FALSE;
                    break;
                case SQL_DESC_LENGTH :
                    sDescRec->mLength = (SQLULEN)(stlVarInt)aValue;
                    break;
                case SQL_DESC_NAME :
                    if( sDescRec->mName != NULL )
                    {
                        stlFreeHeap( sDescRec->mName );
                        sDescRec->mName = NULL;
                    }

                    if( aBufferLen == SQL_NTS )
                    {
                        aBufferLen = stlStrlen( (stlChar*)aValue );
                    }

                    STL_TRY( stlAllocHeap( (void**)&sDescRec->mName,
                                           aBufferLen + 1,
                                           aErrorStack ) == STL_SUCCESS );
                    
                    stlStrcpy( sDescRec->mName, (stlChar*)aValue );
                    break;
                case SQL_DESC_NUM_PREC_RADIX :
                    sDescRec->mNumPrecRadix = (stlInt32)((stlVarInt)aValue);
                    break;
                case SQL_DESC_OCTET_LENGTH :
                    sDescRec->mOctetLength = (SQLLEN)(stlVarInt)aValue;
                    break;
                case SQL_DESC_OCTET_LENGTH_PTR :
                    sDescRec->mOctetLengthPtr = (SQLLEN*)aValue;
                    sUnbind = STL_FALSE;
                    break;
                case SQL_DESC_PARAMETER_TYPE :
                    sDescRec->mParameterType = (stlInt16)((stlVarInt)aValue);
                    if( aDesc->mDescType == ZLD_DESC_TYPE_IPD )
                    {
                        sDescRec->mChanged = STL_TRUE;
                    }
                    break;
                case SQL_DESC_PRECISION :
                    sDescRec->mPrecision = (stlInt16)((stlVarInt)aValue);
                    if( aDesc->mDescType == ZLD_DESC_TYPE_IPD )
                    {
                        sDescRec->mChanged = STL_TRUE;
                    }
                    break;
                case SQL_DESC_SCALE :
                    sDescRec->mScale = (stlInt16)((stlVarInt)aValue);
                    if( aDesc->mDescType == ZLD_DESC_TYPE_IPD )
                    {
                        sDescRec->mChanged = STL_TRUE;
                    }
                    break;
                case SQL_DESC_TYPE :
                    sDescRec->mType = (stlInt16)((stlVarInt)aValue);
                    if( aDesc->mDescType == ZLD_DESC_TYPE_IPD )
                    {
                        sDescRec->mChanged = STL_TRUE;
                    }
                    break;
                case SQL_DESC_UNNAMED :
                    sDescRec->mUnnamed = (stlInt16)((stlVarInt)aValue);
                    break;
                case SQL_DESC_CHAR_LENGTH_UNITS :
                    sInt16 = (stlInt16)((stlVarInt)aValue);

                    switch( sInt16 )
                    {
                        case SQL_CLU_NONE :
                            sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                            break;
                        case SQL_CLU_CHARACTERS :
                            sStringLengthUnit = DTL_STRING_LENGTH_UNIT_CHARACTERS;
                            break;
                        case SQL_CLU_OCTETS :
                            sStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
                            break;
                        default :
                            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                            break;
                    }

                    if( (aDesc->mDescType == ZLD_DESC_TYPE_IPD) &&
                        (sDescRec->mStringLengthUnit != sStringLengthUnit) )
                    {   
                        aDesc->mStmt->mParamChanged = STL_TRUE;
                        sDescRec->mChanged = STL_TRUE;
                    }

                    sDescRec->mStringLengthUnit = sStringLengthUnit;

                    break;
            }

            if( sUnbind == STL_TRUE )
            {
                sDescRec->mDataPtr = NULL;
            }

            if( sCheckConsistency == STL_TRUE )
            {
                STL_TRY( zldDescCheckConsistency( aDesc,
                                                  sDescRec,
                                                  aErrorStack ) == STL_SUCCESS );
            }
            
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_VALUE,
                      "Given the specified Attribute value, "
                      "an invalid value was specified in ValuePtr.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zldDescGetSqlType( dtlDataType            aDBType,
                             stlInt64               aScale,
                             dtlIntervalIndicator   aIntervalCode,
                             stlInt16             * aSqlType,
                             stlInt16             * aSqlConciseType,
                             stlInt16             * aSqlIntervalCode,
                             stlErrorStack        * aErrorStack )
{
    stlInt16 sSqlType;
    stlInt16 sConciseType;
    stlInt16 sIntervalCode = 0;

    switch( aDBType )
    {
        case DTL_TYPE_BOOLEAN :
            sSqlType = SQL_BOOLEAN;
            sConciseType = SQL_BOOLEAN;
            break;
        case DTL_TYPE_NATIVE_SMALLINT :
            sSqlType = SQL_SMALLINT;
            sConciseType = SQL_SMALLINT;
            break;
        case DTL_TYPE_NATIVE_INTEGER :
            sSqlType = SQL_INTEGER;
            sConciseType = SQL_INTEGER;
            break;
        case DTL_TYPE_NATIVE_BIGINT :
            sSqlType = SQL_BIGINT;
            sConciseType = SQL_BIGINT;
            break;
        case DTL_TYPE_NATIVE_REAL :
            sSqlType = SQL_REAL;
            sConciseType = SQL_REAL;
            break;
        case DTL_TYPE_NATIVE_DOUBLE :
            sSqlType = SQL_DOUBLE;
            sConciseType = SQL_DOUBLE;
            break;
        case DTL_TYPE_FLOAT :
            sSqlType = SQL_FLOAT;
            sConciseType = SQL_FLOAT;
            break;
        case DTL_TYPE_NUMBER :
            if( aScale == DTL_SCALE_NA )
            {
                sSqlType = SQL_FLOAT;
                sConciseType = SQL_FLOAT;
            }
            else
            {
                sSqlType = SQL_NUMERIC;
                sConciseType = SQL_NUMERIC;
            }
            break;
        case DTL_TYPE_CHAR :
            sSqlType = SQL_CHAR;
            sConciseType = SQL_CHAR;
            break;
        case DTL_TYPE_VARCHAR :
            sSqlType = SQL_VARCHAR;
            sConciseType = SQL_VARCHAR;
            break;
        case DTL_TYPE_LONGVARCHAR :
            sSqlType = SQL_LONGVARCHAR;
            sConciseType = SQL_LONGVARCHAR;
            break;
        case DTL_TYPE_BINARY :
            sSqlType = SQL_BINARY;
            sConciseType = SQL_BINARY;
            break;
        case DTL_TYPE_VARBINARY :
            sSqlType = SQL_VARBINARY;
            sConciseType = SQL_VARBINARY;
            break;
        case DTL_TYPE_LONGVARBINARY :
            sSqlType = SQL_LONGVARBINARY;
            sConciseType = SQL_LONGVARBINARY;
            break;
        case DTL_TYPE_TIME :
            sSqlType = SQL_DATETIME;
            sConciseType = SQL_TYPE_TIME;
            break;
        case DTL_TYPE_TIME_WITH_TIME_ZONE :
            sSqlType = SQL_DATETIME;
            sConciseType = SQL_TYPE_TIME_WITH_TIMEZONE;
            break;
        case DTL_TYPE_DATE :            
        case DTL_TYPE_TIMESTAMP :
            sSqlType = SQL_DATETIME;
            sConciseType = SQL_TYPE_TIMESTAMP;
            break;
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE :
            sSqlType = SQL_DATETIME;
            sConciseType = SQL_TYPE_TIMESTAMP_WITH_TIMEZONE;
            break;
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            sSqlType = SQL_INTERVAL;
            switch( aIntervalCode )
            {
                case DTL_INTERVAL_INDICATOR_YEAR :
                    sConciseType = SQL_INTERVAL_YEAR;
                    sIntervalCode = SQL_CODE_YEAR;
                    break;
                case DTL_INTERVAL_INDICATOR_MONTH :
                    sConciseType = SQL_INTERVAL_MONTH;
                    sIntervalCode = SQL_CODE_MONTH;
                    break;
                case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH :
                    sConciseType = SQL_INTERVAL_YEAR_TO_MONTH;
                    sIntervalCode = SQL_CODE_YEAR_TO_MONTH;
                    break;
                default :
                    STL_THROW( RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
            }
            break;
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            sSqlType = SQL_INTERVAL;
            switch( aIntervalCode )
            {
                case DTL_INTERVAL_INDICATOR_DAY :
                    sConciseType = SQL_INTERVAL_DAY;
                    sIntervalCode = SQL_CODE_DAY;
                    break;
                case DTL_INTERVAL_INDICATOR_HOUR :
                    sConciseType = SQL_INTERVAL_HOUR;
                    sIntervalCode = SQL_CODE_HOUR;
                    break;
                case DTL_INTERVAL_INDICATOR_MINUTE :
                    sConciseType = SQL_INTERVAL_MINUTE;
                    sIntervalCode = SQL_CODE_MINUTE;
                    break;
                case DTL_INTERVAL_INDICATOR_SECOND :
                    sConciseType = SQL_INTERVAL_SECOND;
                    sIntervalCode = SQL_CODE_SECOND;
                    break;
                case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR :
                    sConciseType = SQL_INTERVAL_DAY_TO_HOUR;
                    sIntervalCode = SQL_CODE_DAY_TO_HOUR;
                    break;
                case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE :
                    sConciseType = SQL_INTERVAL_DAY_TO_MINUTE;
                    sIntervalCode = SQL_CODE_DAY_TO_MINUTE;
                    break;
                case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND :
                    sConciseType = SQL_INTERVAL_DAY_TO_SECOND;
                    sIntervalCode = SQL_CODE_DAY_TO_SECOND;
                    break;
                case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE :
                    sConciseType = SQL_INTERVAL_HOUR_TO_MINUTE;
                    sIntervalCode = SQL_CODE_HOUR_TO_MINUTE;
                    break;
                case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND :
                    sConciseType = SQL_INTERVAL_HOUR_TO_SECOND;
                    sIntervalCode = SQL_CODE_HOUR_TO_SECOND;
                    break;
                case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND :
                    sConciseType = SQL_INTERVAL_MINUTE_TO_SECOND;
                    sIntervalCode = SQL_CODE_MINUTE_TO_SECOND;
                    break;
                default :
                    STL_THROW( RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 );
                    break;
            }
            break;
        case DTL_TYPE_ROWID:
            sSqlType = SQL_ROWID;
            sConciseType = SQL_ROWID;
            break;            
        default :
            STL_THROW( RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION1 );
            break;
    }

    if( aSqlType != NULL )
    {
        *aSqlType = sSqlType;
    }

    if( aSqlConciseType != NULL )
    {
        *aSqlConciseType = sConciseType;
    }

    if( aSqlIntervalCode != NULL )
    {
        *aSqlIntervalCode = sIntervalCode;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INCONSISTENT_DESCRIPTOR_INFORMATION,
                      "The Type field, or any other field associated with the "
                      "SQL_DESC_TYPE field in the descriptor, was not valid or consistent.",
                      aErrorStack );

        STL_DASSERT( STL_FALSE );
    }

    STL_CATCH( RAMP_ERR_INCONSISTENT_DESCRIPTOR_INFORMATION2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INCONSISTENT_DESCRIPTOR_INFORMATION,
                      "Descriptor information checked during a consistency check was "
                      "not consistent.",
                      aErrorStack );

        STL_DASSERT( STL_FALSE );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zldDescBuildResult( zlsStmt       * aStmt,
                              stlBool         aNeedResultCols,
                              stlErrorStack * aErrorStack )
{
    zlcDbc * sDbc;

    sDbc = aStmt->mParentDbc;

#if defined( ODBC_ALL )
    STL_TRY( gZldDescBuildResultFunc[sDbc->mProtocolType]( sDbc,
                                                           aStmt,
                                                           aNeedResultCols,
                                                           aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zladDescBuildResult( sDbc,
                                  aStmt,
                                  aNeedResultCols,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcdDescBuildResult( sDbc,
                                  aStmt,
                                  aNeedResultCols,
                                  aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zldDescGetField( zldDesc       * aDesc,
                           stlInt16        aRecNumber,
                           stlInt16        aFieldId,
                           void          * aValue,
                           stlInt32        aBufferLen,
                           stlInt32      * aStringLen,
                           stlErrorStack * aErrorStack )
{
    zlsStmt        * sStmt;
    zldDescElement * sDescRec;

    sStmt = aDesc->mStmt;

    if( ( aDesc->mDescType == ZLD_DESC_TYPE_IRD ) &&
        ( sStmt->mNeedResultDescriptor == STL_TRUE ) )
    {
        STL_TRY( zldDescBuildResult( sStmt,
                                     STL_FALSE,
                                     aErrorStack ) == STL_SUCCESS );

        sStmt->mNeedResultDescriptor = STL_FALSE;
    }

    switch( aFieldId )
    {
        case SQL_DESC_ALLOC_TYPE :
            if( aValue != NULL )
            {
                *(SQLSMALLINT*)aValue = aDesc->mAllocType;
            }
            break;
        case SQL_DESC_ARRAY_SIZE :
            if( aValue != NULL )
            {
                *(SQLULEN*)aValue = aDesc->mArraySize;
            }
            break;
        case SQL_DESC_ARRAY_STATUS_PTR :
            if( aValue != NULL )
            {
                *(SQLUSMALLINT**)aValue = aDesc->mArrayStatusPtr;
            }
            break;
        case SQL_DESC_BIND_OFFSET_PTR :
            if( aValue != NULL )
            {
                *(SQLLEN**)aValue = aDesc->mBindOffsetPtr;
            }
            break;
        case SQL_DESC_BIND_TYPE :
            if( aValue != NULL )
            {
                *(SQLINTEGER*)aValue = aDesc->mBindType;
            }
            break;
        case SQL_DESC_COUNT :
            if( aValue != NULL )
            {
                *(SQLSMALLINT*)aValue = aDesc->mCount;
            }
            break;
        case SQL_DESC_ROWS_PROCESSED_PTR :
            if( aValue != NULL )
            {
                *(SQLULEN**)aValue = aDesc->mRowProcessed;
            }
            break;
        default :
            STL_TRY( zldDescFindRec( aDesc, aRecNumber, &sDescRec ) == STL_SUCCESS );

            switch( aFieldId )
            {
                case SQL_DESC_AUTO_UNIQUE_VALUE :
                    if( aValue != NULL )
                    {
                        *(SQLINTEGER*)aValue = sDescRec->mAutoUniqueValue;
                    }
                    break;
                case SQL_DESC_BASE_COLUMN_NAME :
                    if( aValue != NULL )
                    {
                        if( sDescRec->mBaseColumnName != NULL )
                        {
                            stlStrncpy( (stlChar*)aValue,
                                        sDescRec->mBaseColumnName,
                                        aBufferLen );
                        }
                        else
                        {
                            *(stlChar*)aValue = 0;
                        }
                    }
                    if( aStringLen != NULL )
                    {
                        if( sDescRec->mBaseColumnName != NULL )
                        {
                            *aStringLen = (stlInt32)stlStrlen( sDescRec->mBaseColumnName );
                        }
                        else
                        {
                            *aStringLen = 0;
                        }
                    }
                    break;
                case SQL_DESC_BASE_TABLE_NAME :
                    if( aValue != NULL )
                    {
                        if( sDescRec->mBaseTableName != NULL )
                        {
                            stlStrncpy( (stlChar*)aValue,
                                        sDescRec->mBaseTableName,
                                        aBufferLen );
                        }
                        else
                        {
                            *(stlChar*)aValue = 0;
                        }
                    }
                    if( aStringLen != NULL )
                    {
                        if( sDescRec->mBaseTableName != NULL )
                        {
                            *aStringLen = (stlInt32)stlStrlen( sDescRec->mBaseTableName );
                        }
                        else
                        {
                            *aStringLen = 0;
                        }
                    }
                    break;
                case SQL_DESC_CASE_SENSITIVE :
                    if( aValue != NULL )
                    {
                        *(SQLINTEGER*)aValue = sDescRec->mCaseSensitive;
                    }
                    break;
                case SQL_DESC_CATALOG_NAME :
                    if( aValue != NULL )
                    {
                        if( sDescRec->mCatalogName != NULL )
                        {
                            stlStrncpy( (stlChar*)aValue,
                                        sDescRec->mCatalogName,
                                        aBufferLen );
                        }
                        else
                        {
                            *(stlChar*)aValue = 0;
                        }
                    }
                    if( aStringLen != NULL )
                    {
                        if( sDescRec->mCatalogName != NULL )
                        {
                            *aStringLen = (stlInt32)stlStrlen( sDescRec->mCatalogName );
                        }
                        else
                        {
                            *aStringLen = 0;
                        }
                    }
                    break;
                case SQL_DESC_CONCISE_TYPE :
                    if( aValue != NULL )
                    {
                        *(SQLSMALLINT*)aValue = sDescRec->mConsiceType;
                    }
                    break;
                case SQL_DESC_DATA_PTR :
                    if( aValue != NULL )
                    {
                        *(SQLPOINTER*)aValue = sDescRec->mDataPtr;
                    }
                    break;
                case SQL_DESC_DATETIME_INTERVAL_CODE :
                    if( aValue != NULL )
                    {
                        *(SQLSMALLINT*)aValue = sDescRec->mDatetimeIntervalCode;
                    }
                    break;
                case SQL_DESC_DATETIME_INTERVAL_PRECISION :
                    if( aValue != NULL )
                    {
                        *(SQLINTEGER*)aValue = sDescRec->mDatetimeIntervalPrecision;
                    }
                    break;
                case SQL_DESC_DISPLAY_SIZE :
                    if( aValue != NULL )
                    {
                        *(SQLLEN*)aValue = sDescRec->mDisplaySize;
                    }
                    break;
                case SQL_DESC_FIXED_PREC_SCALE :
                    if( aValue != NULL )
                    {
                        *(SQLSMALLINT*)aValue = sDescRec->mFixedPrecScale;
                    }
                    break;
                case SQL_DESC_INDICATOR_PTR :
                    if( aValue != NULL )
                    {
                        *(SQLLEN**)aValue = sDescRec->mIndicatorPtr;
                    }
                    break;
                case SQL_DESC_LABEL :
                    if( aValue != NULL )
                    {
                        if( sDescRec->mLabel != NULL )
                        {
                            stlStrncpy( (stlChar*)aValue,
                                        sDescRec->mLabel,
                                        aBufferLen );
                        }
                        else
                        {
                            *(stlChar*)aValue = 0;
                        }
                    }
                    if( aStringLen != NULL )
                    {
                        if( sDescRec->mLabel != NULL )
                        {
                            *aStringLen = (stlInt32)stlStrlen( sDescRec->mLabel );
                        }
                        else
                        {
                            *aStringLen = 0;
                        }
                    }
                    break;
                case SQL_DESC_LENGTH :
                    if( aValue != NULL )
                    {
                        *(SQLULEN*)aValue = sDescRec->mLength;
                    }
                    break;
                case SQL_DESC_LITERAL_PREFIX :
                    if( aValue != NULL )
                    {
                        if( sDescRec->mLiteralPrefix != NULL )
                        {
                            stlStrncpy( (stlChar*)aValue,
                                        sDescRec->mLiteralPrefix,
                                        aBufferLen );
                        }
                        else
                        {
                            *(stlChar*)aValue = 0;
                        }
                    }
                    if( aStringLen != NULL )
                    {
                        if( sDescRec->mLiteralPrefix != NULL )
                        {
                            *aStringLen = (stlInt32)stlStrlen( sDescRec->mLiteralPrefix );
                        }
                        else
                        {
                            *aStringLen = 0;
                        }
                    }
                    break;
                case SQL_DESC_LITERAL_SUFFIX :
                    if( aValue != NULL )
                    {
                        if( sDescRec->mLiteralSuffix != NULL )
                        {
                            stlStrncpy( (stlChar*)aValue,
                                        sDescRec->mLiteralSuffix,
                                        aBufferLen );
                        }
                        else
                        {
                            *(stlChar*)aValue = 0;
                        }
                    }
                    if( aStringLen != NULL )
                    {
                        if( sDescRec->mLiteralSuffix != NULL )
                        {
                            *aStringLen = (stlInt32)stlStrlen( sDescRec->mLiteralSuffix );
                        }
                        else
                        {
                            *aStringLen = 0;
                        }
                    }
                    break;
                case SQL_DESC_LOCAL_TYPE_NAME :
                    if( aValue != NULL )
                    {
                        if( sDescRec->mLocalTypeName != NULL )
                        {
                            stlStrncpy( (stlChar*)aValue,
                                        sDescRec->mLocalTypeName,
                                        aBufferLen );
                        }
                        else
                        {
                            *(stlChar*)aValue = 0;
                        }
                    }
                    if( aStringLen != NULL )
                    {
                        if( sDescRec->mLocalTypeName != NULL )
                        {
                            *aStringLen = (stlInt32)stlStrlen( sDescRec->mLocalTypeName );
                        }
                        else
                        {
                            *aStringLen = 0;
                        }
                    }
                    break;
                case SQL_DESC_NAME :
                    if( aValue != NULL )
                    {
                        if( sDescRec->mName != NULL )
                        {
                            stlStrncpy( (stlChar*)aValue,
                                        sDescRec->mName,
                                        aBufferLen );
                        }
                        else
                        {
                            *(stlChar*)aValue = 0;
                        }
                    }
                    if( aStringLen != NULL )
                    {
                        if( sDescRec->mName != NULL )
                        {
                            *aStringLen = (stlInt32)stlStrlen( sDescRec->mName );
                        }
                        else
                        {
                            *aStringLen = 0;
                        }
                    }
                    break;
                case SQL_DESC_NULLABLE :
                    if( aValue != NULL )
                    {
                        *(SQLSMALLINT*)aValue = sDescRec->mNullable;
                    }
                    break;
                case SQL_DESC_NUM_PREC_RADIX :
                    if( aValue != NULL )
                    {
                        *(SQLINTEGER*)aValue = sDescRec->mNumPrecRadix;
                    }
                    break;
                case SQL_DESC_OCTET_LENGTH :
                    if( aValue != NULL )
                    {
                        *(SQLLEN*)aValue = sDescRec->mOctetLength;
                    }
                    break;
                case SQL_DESC_OCTET_LENGTH_PTR :
                    if( aValue != NULL )
                    {
                        *(SQLLEN**)aValue = sDescRec->mOctetLengthPtr;
                    }
                    break;
                case SQL_DESC_PARAMETER_TYPE :
                    if( aValue != NULL )
                    {
                        *(SQLSMALLINT*)aValue = sDescRec->mParameterType;
                    }
                    break;
                case SQL_DESC_PRECISION :
                    if( aValue != NULL )
                    {
                        *(SQLSMALLINT*)aValue = sDescRec->mPrecision;
                    }
                    break;
                case SQL_DESC_ROWVER :
                    if( aValue != NULL )
                    {
                        *(SQLSMALLINT*)aValue = sDescRec->mRowver;
                    }
                    break;
                case SQL_DESC_SCALE :
                    if( aValue != NULL )
                    {
                        *(SQLSMALLINT*)aValue = sDescRec->mScale;
                    }
                    break;
                case SQL_DESC_SCHEMA_NAME :
                    if( aValue != NULL )
                    {
                        if( sDescRec->mSchemaName != NULL )
                        {
                            stlStrncpy( (stlChar*)aValue,
                                        sDescRec->mSchemaName,
                                        aBufferLen );
                        }
                        else
                        {
                            *(stlChar*)aValue = 0;
                        }
                    }
                    if( aStringLen != NULL )
                    {
                        if( sDescRec->mSchemaName != NULL )
                        {
                            *aStringLen = (stlInt32)stlStrlen( sDescRec->mSchemaName );
                        }
                        else
                        {
                            *aStringLen = 0;
                        }
                    }
                    break;
                case SQL_DESC_SEARCHABLE :
                    if( aValue != NULL )
                    {
                        *(SQLSMALLINT*)aValue = sDescRec->mSearchable;
                    }
                    break;
                case SQL_DESC_TABLE_NAME :
                    if( aValue != NULL )
                    {
                        if( sDescRec->mTableName != NULL )
                        {
                            stlStrncpy( (stlChar*)aValue,
                                        sDescRec->mTableName,
                                        aBufferLen );
                        }
                        else
                        {
                            *(stlChar*)aValue = 0;
                        }
                    }
                    if( aStringLen != NULL )
                    {
                        if( sDescRec->mTableName != NULL )
                        {
                            *aStringLen = (stlInt32)stlStrlen( sDescRec->mTableName );
                        }
                        else
                        {
                            *aStringLen = 0;
                        }
                    }
                    break;
                case SQL_DESC_TYPE :
                    if( aValue != NULL )
                    {
                        *(SQLSMALLINT*)aValue = sDescRec->mType;
                    }
                    break;
                case SQL_DESC_TYPE_NAME :
                    if( aValue != NULL )
                    {
                        if( sDescRec->mTypeName != NULL )
                        {
                            stlStrncpy( (stlChar*)aValue,
                                        sDescRec->mTypeName,
                                        aBufferLen );
                        }
                        else
                        {
                            *(stlChar*)aValue = 0;
                        }
                    }
                    if( aStringLen != NULL )
                    {
                        if( sDescRec->mTypeName != NULL )
                        {
                            *aStringLen = (stlInt32)stlStrlen( sDescRec->mTypeName );
                        }
                        else
                        {
                            *aStringLen = 0;
                        }
                    }
                    break;
                case SQL_DESC_UNNAMED :
                    if( aValue != NULL )
                    {
                        *(SQLSMALLINT*)aValue = sDescRec->mUnnamed;
                    }
                    break;
                case SQL_DESC_UNSIGNED :
                    if( aValue != NULL )
                    {
                        *(SQLSMALLINT*)aValue = sDescRec->mUnsigned;
                    }
                    break;
                case SQL_DESC_UPDATABLE :
                    if( aValue != NULL )
                    {
                        *(SQLSMALLINT*)aValue = sDescRec->mUpdatable;
                    }
                    break;
                case SQL_DESC_CHAR_LENGTH_UNITS :
                    if( aValue != NULL )
                    {
                        switch( sDescRec->mStringLengthUnit )
                        {
                            case DTL_STRING_LENGTH_UNIT_NA :
                                *(SQLSMALLINT*)aValue = SQL_CLU_NONE;
                                break;
                            case DTL_STRING_LENGTH_UNIT_CHARACTERS :
                                *(SQLSMALLINT*)aValue = SQL_CLU_CHARACTERS;
                                break;
                            case DTL_STRING_LENGTH_UNIT_OCTETS :
                                *(SQLSMALLINT*)aValue = SQL_CLU_OCTETS;
                                break;
                            default :
                                STL_DASSERT( STL_FALSE );
                                break;
                        }
                    }
                    break;
            }
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zldDescGetRec( zldDesc       * aDesc,
                         stlInt16        aRecNumber,
                         stlChar       * aName,
                         stlInt16        aBufferLen,
                         stlInt16      * aStringLen,
                         stlInt16      * aType,
                         stlInt16      * aSubType,
                         stlInt64      * aLength,
                         stlInt16      * aPrecision,
                         stlInt16      * aScale,
                         stlInt16      * aNullable,
                         stlErrorStack * aErrorStack )
{
    zlsStmt        * sStmt;
    zldDescElement * sDescRec;

    sStmt = aDesc->mStmt;

    if( ( aDesc->mDescType == ZLD_DESC_TYPE_IRD ) &&
        ( sStmt->mNeedResultDescriptor == STL_TRUE ) )
    {
        STL_TRY( zldDescBuildResult( sStmt,
                                     STL_FALSE,
                                     aErrorStack ) == STL_SUCCESS );

        sStmt->mNeedResultDescriptor = STL_FALSE;
    }

    STL_TRY( zldDescFindRec( aDesc, aRecNumber, &sDescRec ) == STL_SUCCESS );

    if( aName != NULL )
    {
        if( sDescRec->mName != NULL )
        {
            stlStrncpy( aName, sDescRec->mName, aBufferLen );
        }
        else
        {
            aName[0] = 0;
        }
    }

    if( aStringLen != NULL )
    {
        if( sDescRec->mName != NULL )
        {
            *aStringLen = (stlInt16)stlStrlen( sDescRec->mName );
        }
        else
        {
            *aStringLen = 0;
        }
    }

    if( aType != NULL )
    {
        *aType = sDescRec->mType;
    }

    if( aSubType != NULL )
    {
        switch( sDescRec->mType )
        {
            case SQL_DATETIME :
            case SQL_INTERVAL :
                *aSubType = sDescRec->mDatetimeIntervalCode;
                break;
            default :
                *aSubType = 0;
                break;
        }
    }

    if( aLength != NULL )
    {
        *aLength = sDescRec->mOctetLength;
    }

    if( aPrecision != NULL )
    {
        *aPrecision = sDescRec->mPrecision;
    }

    if( aScale != NULL)
    {
        *aScale = sDescRec->mScale;
    }

    if( aNullable != NULL )
    {
        *aNullable = sDescRec->mNullable;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
