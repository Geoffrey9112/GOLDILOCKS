/*******************************************************************************
 * zliCol.c
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
#include <zld.h>
#include <zlvSqlToC.h>

/**
 * @file zliCol.c
 * @brief ODBC API Internal Column Routines.
 */

/**
 * @addtogroup zli
 * @{
 */

void * zliGetDataBuffer( zldDesc        * aDesc,
                         zldDescElement * aDescRec,
                         stlInt64         aNumber )
{
    void      * sBufferAddress = NULL;
    stlInt64    sBindingOffset = 0;
    stlUInt64   sElementSize   = 0;

    if( aDescRec->mDataPtr != NULL )
    {
        if( aDesc->mBindOffsetPtr != NULL )
        {
            sBindingOffset = *aDesc->mBindOffsetPtr;
        }
    
        if( aDesc->mBindType == SQL_BIND_BY_COLUMN )
        {
            switch( aDescRec->mConsiceType )
            {
                case SQL_C_CHAR :
                case SQL_C_WCHAR :
                case SQL_C_BINARY :
                    sElementSize = aDescRec->mLength;
                    break;
                case SQL_C_LONGVARCHAR :
                case SQL_C_LONGVARBINARY :
                    sElementSize = STL_SIZEOF( SQL_LONG_VARIABLE_LENGTH_STRUCT );
                    break;
                case SQL_C_SSHORT :
                case SQL_C_USHORT :
                case SQL_C_SHORT :
                    sElementSize = STL_SIZEOF( stlInt16 );
                    break;
                case SQL_C_SLONG :
                case SQL_C_ULONG :
                case SQL_C_LONG :
                    sElementSize = STL_SIZEOF( stlInt32 );
                    break;
                case SQL_C_FLOAT :
                    sElementSize = STL_SIZEOF( stlFloat32 );
                    break;
                case SQL_C_DOUBLE :
                    sElementSize = STL_SIZEOF( stlFloat64 );
                    break;
                case SQL_C_BIT :
                case SQL_C_BOOLEAN :
                case SQL_C_STINYINT :
                case SQL_C_UTINYINT :
                case SQL_C_TINYINT :
                    sElementSize = STL_SIZEOF( stlInt8 );
                    break;
                case SQL_C_SBIGINT :
                case SQL_C_UBIGINT :
                    sElementSize = STL_SIZEOF( stlInt64 );
                    break;
                case SQL_C_TYPE_DATE :
                    sElementSize = STL_SIZEOF( SQL_DATE_STRUCT );
                    break;
                case SQL_C_TYPE_TIME :
                    sElementSize = STL_SIZEOF( SQL_TIME_STRUCT );
                    break;
                case SQL_C_TYPE_TIME_WITH_TIMEZONE :
                    sElementSize = STL_SIZEOF( SQL_TIME_WITH_TIMEZONE_STRUCT );
                    break;
                case SQL_C_TYPE_TIMESTAMP :
                    sElementSize = STL_SIZEOF( SQL_TIMESTAMP_STRUCT );
                    break;
                case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    sElementSize = STL_SIZEOF( SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT );
                    break;
                case SQL_C_INTERVAL_YEAR :
                case SQL_C_INTERVAL_MONTH :
                case SQL_C_INTERVAL_DAY :
                case SQL_C_INTERVAL_HOUR :
                case SQL_C_INTERVAL_MINUTE :
                case SQL_C_INTERVAL_SECOND :
                case SQL_C_INTERVAL_YEAR_TO_MONTH :
                case SQL_C_INTERVAL_DAY_TO_HOUR :
                case SQL_C_INTERVAL_DAY_TO_MINUTE :
                case SQL_C_INTERVAL_DAY_TO_SECOND :
                case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                case SQL_C_INTERVAL_HOUR_TO_SECOND :
                case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                    sElementSize = STL_SIZEOF( SQL_INTERVAL_STRUCT );
                    break;
                case SQL_C_NUMERIC :
                    sElementSize = STL_SIZEOF( SQL_NUMERIC_STRUCT );
                    break;
                case SQL_C_GUID :
                    sElementSize = STL_SIZEOF( SQLGUID );
                    break;
                default :
                    stlPrintf("UNKNOWN TYPE %d\n", aDescRec->mConsiceType );
                    STL_ASSERT( 0 );
                    break;
            }
        }
        else
        {
            sElementSize = aDesc->mBindType;
        }

        /*
         * Bound Address + Binding Offset + ((Row Number – 1) x Element Size)
         */
        sBufferAddress = (void*)((stlChar*)aDescRec->mDataPtr +
                                 sBindingOffset +
                                 ( ( aNumber - 1 ) * sElementSize ) );
    }

    return sBufferAddress;
}

SQLLEN * zliGetIndicatorBuffer( zldDesc        * aDesc,
                                zldDescElement * aDescRec,
                                stlInt64         aNumber )
{
    void      * sBufferAddress = NULL;
    stlInt64    sBindingOffset = 0;
    stlUInt64   sElementSize   = 0;

    if( aDescRec->mIndicatorPtr != NULL )
    {
        if( aDesc->mBindOffsetPtr != NULL )
        {
            sBindingOffset = *aDesc->mBindOffsetPtr;
        }
    
        if( aDesc->mBindType == SQL_BIND_BY_COLUMN )
        {
            sElementSize = STL_SIZEOF( SQLLEN );
        }
        else
        {
            sElementSize = aDesc->mBindType;
        }

        /*
         * Bound Address + Binding Offset + ((Row Number – 1) x Element Size)
         */
        sBufferAddress = (SQLLEN*)((stlChar*)aDescRec->mIndicatorPtr +
                                   sBindingOffset +
                                   ( ( aNumber - 1 ) * sElementSize ) );
    }

    return sBufferAddress;
}

stlStatus zliBindCol( zlsStmt       * aStmt,
                      stlUInt16       aColIdx,
                      stlInt16        aCType,
                      void          * aCBuffer,
                      SQLLEN          aBufferLen,
                      SQLLEN        * aInd,
                      stlErrorStack * aErrorStack )
{
    zldDesc        * sArd;
    zldDescElement * sArdRec;

    sArd = aStmt->mArd;

    if( aColIdx > sArd->mCount )
    {
        STL_TRY( zldDescAllocRec( sArd,
                                  aColIdx,
                                  &sArdRec,
                                  aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( zldDescFindRec( sArd, aColIdx, &sArdRec ) == STL_SUCCESS );
    }

    STL_TRY( zldDescSetTargetType( aStmt, sArdRec, aCType, aBufferLen ) == STL_SUCCESS );

    sArdRec->mSqlToCFunc     = zlvInvalidSqlToC;
    sArdRec->mOctetLength    = aBufferLen;
    sArdRec->mDataPtr        = aCBuffer;
    sArdRec->mIndicatorPtr   = aInd;
    sArdRec->mOctetLengthPtr = aInd;

    STL_TRY( zldDescCheckConsistency( sArd,
                                      sArdRec,
                                      aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zliDescribeCol( zlsStmt       * aStmt,
                          stlUInt16       aColumnIdx,
                          stlChar       * aColumnName,
                          stlInt16        aBufferLen,
                          stlInt16      * aNameLen,
                          stlInt16      * aDataType,
                          stlUInt64     * aColumnSize,
                          stlInt16      * aDecimnalDigits,
                          stlInt16      * aNullable,
                          stlErrorStack * aErrorStack )
{
    zldDesc        * sIrd;
    zldDescElement * sIrdRec;

    sIrd = &aStmt->mIrd;

    if( aStmt->mNeedResultDescriptor == STL_TRUE )
    {
        STL_TRY( zldDescBuildResult( aStmt,
                                     STL_TRUE,
                                     aErrorStack ) == STL_SUCCESS );

        aStmt->mNeedResultDescriptor = STL_FALSE;
    }

    STL_TRY( zldDescFindRec( sIrd, aColumnIdx, &sIrdRec ) == STL_SUCCESS );

    if( aColumnName != NULL )
    {
        stlStrncpy( aColumnName, sIrdRec->mLabel, aBufferLen );
    }

    if( aNameLen != NULL )
    {
        *aNameLen = (stlInt16)stlStrlen( sIrdRec->mLabel );
    }

    if( aDataType != NULL )
    {
        *aDataType = sIrdRec->mConsiceType;
    }

    /*
     * ===================================================================
     * SQL type                        | Descriptor field corresponding to
     *                                 | column or parameter size
     * ===================================================================
     * All character and binary types  | LENGTH
     * All numeric types               | PRECISION
     * All datetime and interval types | LENGTH
     * SQL_BIT                         | LENGTH
     * ===================================================================
     */
    if( aColumnSize != NULL )
    {
        switch( sIrdRec->mConsiceType )
        {
            case SQL_NUMERIC :
            case SQL_DECIMAL :
            case SQL_TINYINT :
            case SQL_SMALLINT :
            case SQL_INTEGER :
            case SQL_BIGINT :
            case SQL_REAL :
            case SQL_FLOAT :
            case SQL_DOUBLE :
                *aColumnSize = sIrdRec->mPrecision;
                break;
                
            default :
                *aColumnSize = sIrdRec->mLength;
                break;
        }
    }

    /*
     * ================================================================================
     * SQL type                                     | Descriptor field corresponding to
     *                                              | decimal digits
     * ================================================================================
     * All character and binary types               | n/a
     * All exact numeric types                      | SCALE
     * SQL_BIT                                      | n/a
     * All approximate numeric types                | n/a
     * All datetime types                           | PRECISION
     * All interval types with a seconds component  | PRECISION
     * All interval types with no seconds component | n/a
     * ================================================================================
     */
    if( aDecimnalDigits != NULL )
    {
        switch( sIrdRec->mConsiceType )
        {
            case SQL_NUMERIC :
            case SQL_DECIMAL :
            case SQL_TINYINT :
            case SQL_SMALLINT :
            case SQL_INTEGER :
            case SQL_BIGINT :
                *aDecimnalDigits = sIrdRec->mScale;
                break;
            case SQL_TYPE_DATE :
            case SQL_TYPE_TIME :
            case SQL_TYPE_TIMESTAMP :
            case SQL_TYPE_TIME_WITH_TIMEZONE :
            case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
            case SQL_INTERVAL_SECOND :
            case SQL_INTERVAL_DAY_TO_SECOND :
            case SQL_INTERVAL_HOUR_TO_SECOND :
            case SQL_INTERVAL_MINUTE_TO_SECOND :
                *aDecimnalDigits = sIrdRec->mPrecision;
                break;
            default :
                *aDecimnalDigits = 0;
                break;
        }
    }

    if( aNullable != NULL )
    {
        *aNullable = sIrdRec->mNullable;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
