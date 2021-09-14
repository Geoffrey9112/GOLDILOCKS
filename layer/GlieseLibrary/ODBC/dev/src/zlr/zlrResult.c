/*******************************************************************************
 * zlrResult.c
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
#include <zllRowStatusDef.h>
#include <zlDef.h>
#include <zle.h>
#include <zld.h>
#include <zlvSqlToC.h>

#include <zlar.h>
#include <zlcr.h>

/**
 * @file zlrResult.c
 * @brief ODBC API Internal Result Routines.
 */

/**
 * @addtogroup zlr
 * @{
 */

#if defined( ODBC_ALL )
stlStatus (*gZlrCloseFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                    zlsStmt       * aStmt,
                                                    stlErrorStack * aErrorStack ) =
{
    zlarClose,
    zlcrClose
};

stlStatus (*gZlrFetchFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                    zlsStmt       * aStmt,
                                                    stlUInt64       aArraySize,
                                                    stlInt64        aOffset,
                                                    stlBool       * aNotFound,
                                                    stlBool       * aSuccessWithInfo,
                                                    stlErrorStack * aErrorStack ) =
{
    zlarFetch,
    zlcrFetch
};

stlStatus (*gZlrSetCursorNameFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                            zlsStmt       * aStmt,
                                                            stlChar       * aName,
                                                            stlErrorStack * aErrorStack ) =
{
    zlarSetCursorName,
    zlcrSetCursorName
};

stlStatus (*gZlrGetCursorNameFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                            zlsStmt       * aStmt,
                                                            stlErrorStack * aErrorStack ) =
{
    zlarGetCursorName,
    zlcrGetCursorName
};

stlStatus (*gZlrGetDataFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                      zlsStmt       * aStmt,
                                                      zlrResult     * sResult,
                                                      stlBool       * aEOF,
                                                      stlErrorStack * aErrorStack ) =
{
    zlarGetData,
    zlcrGetData
};

stlStatus (*gZlrFetch4ImpFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc             * aDbc,
                                                        zlsStmt            * aStmt,
                                                        dtlValueBlockList ** aDataPtr,
                                                        stlBool            * aNotFound,
                                                        stlErrorStack      * aErrorStack ) =
{
    zlarFetch4Imp,
    zlcrFetch4Imp
};

stlStatus (*gZlrSetPosFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                     zlsStmt       * aStmt,
                                                     stlInt64        aRowNumber,
                                                     stlInt16        aOperation,
                                                     stlUInt16       aLockType,
                                                     stlErrorStack * aErrorStack ) =
{
    zlarSetPos,
    zlcrSetPos
};
#endif

void zlrAddNullIndicatorError( zlsStmt       * aStmt,
                               stlInt64        aRowNumber,
                               stlInt32        aColumnNumber,
                               stlErrorStack * aErrorStack )
{
    zldDesc * sIrd;

    sIrd = &aStmt->mIrd;

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLE_ERRCODE_INDICATOR_VARIABLE_REQUIRED_BUT_NOT_SUPPLIED,
                  "NULL data was fetched into a column whose "
                  "aStrLen_or_IndPtr set by SQLBindCol "
                  "(or SQL_DESC_INDICATOR_PTR set by SQLSetDescField or "
                  "SQLSetDescRec) was a null pointer. ",
                  aErrorStack );

    zldDiagAdds( SQL_HANDLE_STMT,
                 (void*)aStmt,
                 aRowNumber,
                 aColumnNumber,
                 aErrorStack );

    if( sIrd->mArrayStatusPtr != NULL )
    {
        sIrd->mArrayStatusPtr[aRowNumber - 1] = SQL_ROW_ERROR;
    }

    return;
}
                    
stlStatus zlrGetDefaultCType( zlsStmt        * aStmt,
                              dtlDataValue   * aDataValue,
                              zlrResult      * aResult,
                              zldDescElement * aArdRec,
                              stlErrorStack  * aErrorStack )
{
    /*
     * SQL_C_DEFAULT 타입을 검사하고, 환경을 설정한다.
     *  ===========================================================
     *   SQL type                         | ODBC type
     *  ===========================================================
     *   SQL_BOOLEAN                      | SQL_C_BOOLEAN
     *   SQL_CHAR                         | SQL_C_CHAR
     *   SQL_VARCHAR                      | SQL_C_CHAR
     *   SQL_NUMERIC                      | SQL_C_CHAR
     *   SQL_DECIMAL                      | SQL_C_CHAR
     *   SQL_SMALLINT                     | SQL_C_SSHORT
     *   SQL_INTEGER                      | SQL_C_SLONG
     *   SQL_BIGINT                       | SQL_C_SBIGINT
     *   SQL_REAL                         | SQL_C_FLOAT
     *   SQL_FLOAT                        | SQL_C_DOUBLE
     *   SQL_DOUBLE                       | SQL_C_DOUBLE
     *   SQL_BINARY                       | SQL_C_BINARY
     *   SQL_VARBINARY                    | SQL_C_BINARY
     *   SQL_TYPE_DATE                    | SQL_C_TYPE_DATE
     *   SQL_TYPE_TIME                    | SQL_C_TYPE_TIME
     *   SQL_TYPE_TIMESTAMP               | SQL_C_TYPE_TIMESTAMP
     *   SQL_TYPE_TIME_WITH_TIMEZONE      | SQL_C_TYPE_TIME_WITH_TIMEZONE
     *   SQL_TYPE_TIMESTAMP_WITH_TIMEZONE | SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE
     *   INTERVAL_SQL(YEAR-MONUTH)        | SQL_C_INTERVAL_YEAR_TO_MONTH
     *   INTERVAL_SQL(DATE_TIME)          | SQL_C_INTERVAL_DAY_TO_SECOND
     *   SQL_ROWID                        | SQL_C_CHAR
     *  ===========================================================
     */

    switch( aDataValue->mType )
    {
        case DTL_TYPE_BOOLEAN:
            STL_TRY( zldDescSetTargetType( aStmt,
                                           aArdRec,
                                           SQL_C_BOOLEAN,
                                           aArdRec->mLength ) == STL_SUCCESS );
            break;
        case DTL_TYPE_NATIVE_SMALLINT:
            STL_TRY( zldDescSetTargetType( aStmt,
                                           aArdRec,
                                           SQL_C_SSHORT,
                                           aArdRec->mLength ) == STL_SUCCESS );
            break;
        case DTL_TYPE_NATIVE_INTEGER:
            STL_TRY( zldDescSetTargetType( aStmt,
                                           aArdRec,
                                           SQL_C_SLONG,
                                           aArdRec->mLength ) == STL_SUCCESS );
            break;
        case DTL_TYPE_NATIVE_BIGINT:
            STL_TRY( zldDescSetTargetType( aStmt,
                                           aArdRec,
                                           SQL_C_SBIGINT,
                                           aArdRec->mLength ) == STL_SUCCESS );
            break;
        case DTL_TYPE_NATIVE_REAL:
            STL_TRY( zldDescSetTargetType( aStmt,
                                           aArdRec,
                                           SQL_C_FLOAT,
                                           aArdRec->mLength ) == STL_SUCCESS );
            break;
        case DTL_TYPE_FLOAT:            
        case DTL_TYPE_NATIVE_DOUBLE:
            STL_TRY( zldDescSetTargetType( aStmt,
                                           aArdRec,
                                           SQL_C_DOUBLE,
                                           aArdRec->mLength ) == STL_SUCCESS );
            break;
        case DTL_TYPE_NUMBER:
            /* FALL_THROUGH */
        case DTL_TYPE_DECIMAL:
            /* FALL_THROUGH */
        case DTL_TYPE_CHAR:
            /* FALL_THROUGH */
        case DTL_TYPE_VARCHAR:
            /* FALL_THROUGH */
        case DTL_TYPE_LONGVARCHAR:
            /* FALL_THROUGH */
        case DTL_TYPE_ROWID:                        
            STL_TRY( zldDescSetTargetType( aStmt,
                                           aArdRec,
                                           SQL_C_CHAR,
                                           aArdRec->mLength ) == STL_SUCCESS );
            break;
        case DTL_TYPE_BINARY:
            /* FALL_THROUGH */
        case DTL_TYPE_VARBINARY:
            /* FALL_THROUGH */
        case DTL_TYPE_LONGVARBINARY:
            STL_TRY( zldDescSetTargetType( aStmt,
                                           aArdRec,
                                           SQL_C_BINARY,
                                           aArdRec->mLength ) == STL_SUCCESS );
            break;
        case DTL_TYPE_TIME:
            STL_TRY( zldDescSetTargetType( aStmt,
                                           aArdRec,
                                           SQL_C_TYPE_TIME,
                                           aArdRec->mLength ) == STL_SUCCESS );
            break;
        case DTL_TYPE_DATE:
        case DTL_TYPE_TIMESTAMP:
            STL_TRY( zldDescSetTargetType( aStmt,
                                           aArdRec,
                                           SQL_C_TYPE_TIMESTAMP,
                                           aArdRec->mLength ) == STL_SUCCESS );
            break;
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
            STL_TRY( zldDescSetTargetType( aStmt,
                                           aArdRec,
                                           SQL_C_TYPE_TIME_WITH_TIMEZONE,
                                           aArdRec->mLength ) == STL_SUCCESS );
            break;
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            STL_TRY( zldDescSetTargetType( aStmt,
                                           aArdRec,
                                           SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE,
                                           aArdRec->mLength ) == STL_SUCCESS );
            break;
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            /*
             *  Interval 타입을 확인하여, 디폴트 타입을 지정한다.
             */
            switch( aResult->mTargetBuffer->mValueBlock->mIntervalIndicator )
            {
                case DTL_INTERVAL_INDICATOR_YEAR:
                    STL_TRY( zldDescSetTargetType( aStmt,
                                                   aArdRec,
                                                   SQL_C_INTERVAL_YEAR,
                                                   aArdRec->mLength ) == STL_SUCCESS );
                    break;
                case DTL_INTERVAL_INDICATOR_MONTH:
                    STL_TRY( zldDescSetTargetType( aStmt,
                                                   aArdRec,
                                                   SQL_C_INTERVAL_MONTH,
                                                   aArdRec->mLength ) == STL_SUCCESS );
                    break;
                case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
                    STL_TRY( zldDescSetTargetType( aStmt,
                                                   aArdRec,
                                                   SQL_C_INTERVAL_YEAR_TO_MONTH,
                                                   aArdRec->mLength ) == STL_SUCCESS );
                    break;
                default:
                    break;
            }
            break;
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            /*
             * Interval 타입을 확인하여, 디폴트 타입을 지정한다.
             */
            switch( aResult->mTargetBuffer->mValueBlock->mIntervalIndicator )
            {
                case DTL_INTERVAL_INDICATOR_DAY:
                    STL_TRY( zldDescSetTargetType( aStmt,
                                                   aArdRec,
                                                   SQL_C_INTERVAL_DAY,
                                                   aArdRec->mLength ) == STL_SUCCESS );
                    break;
                case DTL_INTERVAL_INDICATOR_HOUR:
                    STL_TRY( zldDescSetTargetType( aStmt,
                                                   aArdRec,
                                                   SQL_C_INTERVAL_HOUR,
                                                   aArdRec->mLength ) == STL_SUCCESS );
                    break;
                case DTL_INTERVAL_INDICATOR_MINUTE:
                    STL_TRY( zldDescSetTargetType( aStmt,
                                                   aArdRec,
                                                   SQL_C_INTERVAL_MINUTE,
                                                   aArdRec->mLength ) == STL_SUCCESS );
                    break;
                case DTL_INTERVAL_INDICATOR_SECOND:
                    STL_TRY( zldDescSetTargetType( aStmt,
                                                   aArdRec,
                                                   SQL_C_INTERVAL_SECOND,
                                                   aArdRec->mLength ) == STL_SUCCESS );
                    break;
                case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR:
                    STL_TRY( zldDescSetTargetType( aStmt,
                                                   aArdRec,
                                                   SQL_C_INTERVAL_DAY_TO_HOUR,
                                                   aArdRec->mLength ) == STL_SUCCESS );
                    break;
                case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE:
                    STL_TRY( zldDescSetTargetType( aStmt,
                                                   aArdRec,
                                                   SQL_C_INTERVAL_DAY_TO_MINUTE,
                                                   aArdRec->mLength ) == STL_SUCCESS );
                    break;
                case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND:
                    STL_TRY( zldDescSetTargetType( aStmt,
                                                   aArdRec,
                                                   SQL_C_INTERVAL_DAY_TO_SECOND,
                                                   aArdRec->mLength ) == STL_SUCCESS );
                    break;
                case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:
                    STL_TRY( zldDescSetTargetType( aStmt,
                                                   aArdRec,
                                                   SQL_C_INTERVAL_HOUR_TO_MINUTE,
                                                   aArdRec->mLength ) == STL_SUCCESS );
                    break;
                case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:
                    STL_TRY( zldDescSetTargetType( aStmt,
                                                   aArdRec,
                                                   SQL_C_INTERVAL_HOUR_TO_SECOND,
                                                   aArdRec->mLength ) == STL_SUCCESS );
                    break;
                case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
                    STL_TRY( zldDescSetTargetType( aStmt,
                                                   aArdRec,
                                                   SQL_C_INTERVAL_MINUTE_TO_SECOND,
                                                   aArdRec->mLength ) == STL_SUCCESS );
                    break;
                default:
                    break;
            }
            break;
        default:
            STL_THROW( RAMP_ERR_INTERNAL_DATA_TYPE_IS_INVALID );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL_DATA_TYPE_IS_INVALID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INTERNAL_DATA_TYPE_IS_INVALID,
                      "Column's data type is not valid.",
                      aErrorStack );
    }


    STL_FINISH;

    return STL_FAILURE;    
}

stlStatus zlrInitRowStatusArray( zlsStmt * aStmt )
{
    zldDesc   * sArd;
    zldDesc   * sIrd;
    stlUInt64   sArraySize;
    stlUInt64   i;

    sArd = aStmt->mArd;
    sIrd = &aStmt->mIrd;
    
    /*
     * fetch 정보 초기화
     */
    aStmt->mGetDataIdx    = 0;
    aStmt->mGetDataOffset = 0;

    if( sIrd->mRowProcessed != NULL )
    {
        *sIrd->mRowProcessed = 0;
    }
    
    sArraySize = sArd->mArraySize;

    for( i = 0; i < sArraySize; i++ )
    {
        if( sIrd->mArrayStatusPtr != NULL )
        {
            sIrd->mArrayStatusPtr[i] = SQL_ROW_NOROW;
        }
    }

    return STL_SUCCESS;
}

stlStatus zlrCloseWithoutHold( zlcDbc        * aDbc,
                               stlErrorStack * aErrorStack )
{
    zlsStmt * sStmt;

    STL_TRY_THROW( aDbc->mHasWithoutHoldCursor == STL_TRUE,
                   RAMP_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( &aDbc->mStmtRing, sStmt )
    {
        /*
         * WITHOUT HOLD 커서를 닫는다.
         */
        switch( sStmt->mTransition )
        {
            case ZLS_TRANSITION_STATE_S5 :
            case ZLS_TRANSITION_STATE_S6 :
            case ZLS_TRANSITION_STATE_S7 :
                if( sStmt->mCursorHoldable == SQL_NONHOLDABLE )
                {
                    if( sStmt->mIsPrepared != STL_TRUE )
                    {
                        sStmt->mTransition = ZLS_TRANSITION_STATE_S1;
                    }
                    else
                    {
                        sStmt->mTransition = ZLS_TRANSITION_STATE_S3;
                    }
                }
                break;
            default :
                break;
        }
    }

    aDbc->mHasWithoutHoldCursor = STL_FALSE;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
}

stlStatus zlrClose( zlsStmt       * aStmt,
                    stlErrorStack * aErrorStack )
{
    zlcDbc * sDbc;

    sDbc = aStmt->mParentDbc;

#if defined( ODBC_ALL )
    STL_TRY( gZlrCloseFunc[sDbc->mProtocolType]( sDbc,
                                                 aStmt,
                                                 aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlarClose( sDbc,
                        aStmt,
                        aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcrClose( sDbc,
                        aStmt,
                        aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif

    aStmt->mCursorConcurrency = aStmt->mCursorConcurrencyOrg;
    aStmt->mCursorSensitivity = aStmt->mCursorSensitivityOrg;
    aStmt->mCursorHoldable    = aStmt->mCursorHoldableOrg;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlrFetch( zlsStmt       * aStmt,
                    stlUInt64       aArraySize,
                    stlInt64        aOffset,
                    stlBool       * aNotFound,
                    stlBool       * aSuccessWithInfo,
                    stlErrorStack * aErrorStack )
{
    zlcDbc * sDbc;

    sDbc = aStmt->mParentDbc;

#if defined( ODBC_ALL )
    STL_TRY( gZlrFetchFunc[sDbc->mProtocolType]( sDbc,
                                                 aStmt,
                                                 aArraySize,
                                                 aOffset,
                                                 aNotFound,
                                                 aSuccessWithInfo,
                                                 aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlarFetch( sDbc,
                        aStmt,
                        aArraySize,
                        aOffset,
                        aNotFound,
                        aSuccessWithInfo,
                        aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcrFetch( sDbc,
                        aStmt,
                        aArraySize,
                        aOffset,
                        aNotFound,
                        aSuccessWithInfo,
                        aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlrSetCursorName( zlsStmt       * aStmt,
                            stlChar       * aName,
                            stlInt16        aNameLen,
                            stlErrorStack * aErrorStack )
{
    zlcDbc  * sDbc;
    stlChar   sNewName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];

    sDbc = aStmt->mParentDbc;
    
    stlStrncpy( sNewName, aName, aNameLen + 1 );

#if defined( ODBC_ALL )
    STL_TRY( gZlrSetCursorNameFunc[sDbc->mProtocolType]( sDbc,
                                                         aStmt,
                                                         sNewName,
                                                         aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlarSetCursorName( sDbc,
                                aStmt,
                                sNewName,
                                aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcrSetCursorName( sDbc,
                                aStmt,
                                sNewName,
                                aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif
    stlStrncpy( aStmt->mCursorName, aName, aNameLen + 1 );

    aStmt->mSetCursorName  = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlrGetCursorName( zlsStmt       * aStmt,
                            stlChar       * aName,
                            stlInt16        aBufferLen,
                            stlInt16      * aNameLen,
                            stlErrorStack * aErrorStack )
{
    zlcDbc  * sDbc;

    sDbc = aStmt->mParentDbc;
    
    if( aStmt->mSetCursorName == STL_FALSE )
    {
#if defined( ODBC_ALL )
        STL_TRY( gZlrGetCursorNameFunc[sDbc->mProtocolType]( sDbc,
                                                             aStmt,
                                                             aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
        STL_TRY( zlarGetCursorName( sDbc,
                                    aStmt,
                                    aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
        STL_TRY( zlcrGetCursorName( sDbc,
                                    aStmt,
                                    aErrorStack ) == STL_SUCCESS );
#else
        STL_ASSERT( STL_FALSE );
#endif
    }

    if( aName != NULL )
    {
        stlStrncpy( aName, aStmt->mCursorName, aBufferLen );
    }

    if( aNameLen != NULL )
    {
        *aNameLen = stlStrlen( aStmt->mCursorName );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlrGetData( zlsStmt       * aStmt,
                      stlUInt16       aIndex,
                      stlInt16        aCType,
                      void          * aCBuffer,
                      stlInt64        aBufferLen,
                      SQLLEN        * aInd,
                      SQLRETURN     * aReturn,
                      stlErrorStack * aErrorStack )
{
    zlcDbc            * sDbc;
    zldDescElement    * sGetDataRec = NULL;
    zldDescElement    * sArdRec = NULL;
    dtlDataValue      * sDataValue = NULL;
    zlrResult         * sResult;
    stlUInt16           sIdx;
    stlUInt64           sArraySize;
    dtlValueBlockList * sBlockList = NULL;

    stlBool             sNoData = STL_FALSE;
    stlStatus           sReturn = STL_FAILURE;

    *aReturn = SQL_ERROR;
    
    sDbc       = aStmt->mParentDbc;
    sResult    = &aStmt->mResult;
    sArraySize = aStmt->mArd->mArraySize;

    STL_DASSERT( sResult->mPosition > 0 );

    if( sArraySize > 1 )
    {
        if( (sResult->mPosition >= sResult->mRowsetFirstRowIdx) &&
            (sResult->mPosition < sResult->mRowsetFirstRowIdx + sResult->mBlockLastRow) )
        {
            sResult->mBlockCurrentRow = sResult->mPosition - sResult->mRowsetFirstRowIdx;
        }
        else
        {
#if defined( ODBC_ALL )
            STL_TRY( gZlrGetDataFunc[sDbc->mProtocolType]( sDbc,
                                                           aStmt,
                                                           sResult,
                                                           &sNoData,
                                                           aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlarGetData( sDbc,
                                  aStmt,
                                  sResult,
                                  &sNoData,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlcrGetData( sDbc,
                                  aStmt,
                                  sResult,
                                  &sNoData,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif
            STL_TRY_THROW( sNoData == STL_FALSE, RAMP_NO_DATA );
        }
    }
    
    sGetDataRec = &aStmt->mGetDataElement;

    if( sResult->mStatus != ZLL_CURSOR_ROW_STATUS_DELETED )
    {
        sBlockList = sResult->mTargetBuffer;
        
        for( sIdx = 1; sIdx < aIndex; sIdx++ )
        {
            sBlockList = sBlockList->mNext;

            if( sBlockList == NULL )
            {
                break;
            }
        }
    
        STL_TRY_THROW( sBlockList != NULL, RAMP_ERR_INVALID_DESCRIPTOR_INDEX_1 );

        sDataValue = DTL_GET_BLOCK_DATA_VALUE( sBlockList,
                                               sResult->mBlockCurrentRow );
    
        if( aIndex != aStmt->mGetDataIdx )
        {
            ZLS_INIT_GET_DATA_REC(aStmt, aIndex);
        }

        STL_TRY( zldDescSetTargetType( aStmt, sGetDataRec, aCType, aBufferLen ) == STL_SUCCESS );

        sGetDataRec->mSqlToCFunc     = zlvInvalidSqlToC;
        sGetDataRec->mOctetLength    = aBufferLen;
        sGetDataRec->mDataPtr        = aCBuffer;
        sGetDataRec->mIndicatorPtr   = aInd;
        sGetDataRec->mOctetLengthPtr = aInd;

        switch( aCType )
        {
            case SQL_C_DEFAULT:
                STL_TRY( zlrGetDefaultCType( aStmt,
                                             sDataValue,
                                             sResult,
                                             sGetDataRec,
                                             aErrorStack ) == STL_SUCCESS );
                break;
            case SQL_ARD_TYPE:
                STL_TRY_THROW( zldDescFindRec( aStmt->mArd,
                                               aIndex,
                                               &sArdRec ) == STL_SUCCESS,
                               RAMP_ERR_INVALID_DESCRIPTOR_INDEX_2 );

                sGetDataRec->mType                      = sArdRec->mType;
                sGetDataRec->mConsiceType               = sArdRec->mConsiceType;
                sGetDataRec->mDatetimeIntervalPrecision = sArdRec->mDatetimeIntervalPrecision;
                sGetDataRec->mPrecision                 = sArdRec->mPrecision;
                sGetDataRec->mScale                     = sArdRec->mScale;
                break;
            default:
                if( zldDescFindRec( aStmt->mArd, aIndex, &sArdRec ) == STL_SUCCESS )
                {
                    sGetDataRec->mDatetimeIntervalPrecision = sArdRec->mDatetimeIntervalPrecision;
                    sGetDataRec->mPrecision                 = sArdRec->mPrecision;
                    sGetDataRec->mScale                     = sArdRec->mScale;
                }
                break;
        }

        if( DTL_IS_NULL( sDataValue ) == STL_FALSE )
        {
            STL_TRY( zlvGetSqlToCFunc( aStmt,
                                       sDataValue,
                                       sGetDataRec->mConsiceType,
                                       &sGetDataRec->mSqlToCFunc,
                                       aErrorStack ) == STL_SUCCESS );

            STL_TRY( sGetDataRec->mSqlToCFunc( aStmt,
                                               sBlockList,
                                               sDataValue,
                                               &aStmt->mGetDataOffset,
                                               aCBuffer,
                                               aInd,
                                               sGetDataRec,
                                               aReturn,
                                               aErrorStack ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY_THROW( aInd != NULL,
                           RAMP_ERR_INDICATOR_VARIABLE_REQUIRED_BUT_NOT_SUPPLIED );

            STL_TRY_THROW( aStmt->mGetDataOffset == 0, RAMP_NO_DATA );

            aStmt->mGetDataOffset = 1;
            
            *aInd    = SQL_NULL_DATA;
            *aReturn = SQL_SUCCESS;
        }
    }

    sReturn = STL_SUCCESS;

    return sReturn;

    STL_CATCH( RAMP_NO_DATA )
    {
        *aReturn = SQL_NO_DATA;
        sReturn  = STL_SUCCESS;
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The value specified for the argument Col_or_Param_Num was "
                      "greater than the number of columns in the result set.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The TargetType argument was SQL_ARD_TYPE, "
                      "and the value in the SQL_DESC_COUNT field of the ARD was "
                      "less than the Col_or_Param_Num argument.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INDICATOR_VARIABLE_REQUIRED_BUT_NOT_SUPPLIED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INDICATOR_VARIABLE_REQUIRED_BUT_NOT_SUPPLIED,
                      "StrLen_or_IndPtr was a null pointer and NULL data was retrieved.",
                      aErrorStack );
    }

    STL_FINISH;

    return sReturn;
}

stlStatus zlrFetch4Imp( zlsStmt            * aStmt,
                        dtlValueBlockList ** aDataPtr,
                        stlBool            * aNotFound,
                        stlErrorStack      * aErrorStack )
{
    zlcDbc * sDbc = aStmt->mParentDbc;

#if defined( ODBC_ALL )
    STL_TRY( gZlrFetch4ImpFunc[sDbc->mProtocolType]( sDbc,
                                                     aStmt,
                                                     aDataPtr,
                                                     aNotFound,
                                                     aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlarFetch4Imp( sDbc,
                            aStmt,
                            aDataPtr,
                            aNotFound,
                            aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcrFetch4Imp( sDbc,
                            aStmt,
                            aDataPtr,
                            aNotFound,
                            aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlrSetPos( zlsStmt       * aStmt,
                     stlInt64        aRowNumber,
                     stlInt16        aOperation,
                     stlUInt16       aLockType,
                     stlErrorStack * aErrorStack )
{
    zlcDbc   * sDbc;
    stlInt64   sRowNumber;

    sDbc = aStmt->mParentDbc;

    sRowNumber = aStmt->mResult.mCurrentRowIdx + aRowNumber - 1;

#if defined( ODBC_ALL )
    STL_TRY( gZlrSetPosFunc[sDbc->mProtocolType]( sDbc,
                                                  aStmt,
                                                  sRowNumber,
                                                  aOperation,
                                                  aLockType,
                                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlarSetPos( sDbc,
                         aStmt,
                         sRowNumber,
                         aOperation,
                         aLockType,
                         aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcrSetPos( sDbc,
                         aStmt,
                         sRowNumber,
                         aOperation,
                         aLockType,
                         aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif

    ZLS_INIT_GET_DATA_REC(aStmt, 0);
    
    aStmt->mResult.mPosition = sRowNumber;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
