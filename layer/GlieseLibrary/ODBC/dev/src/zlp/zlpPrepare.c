/*******************************************************************************
 * zlpPrepare.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlpPrepare.c 13325 2014-08-04 05:35:14Z htkim $
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

#include <zlap.h>
#include <zlcp.h>

/**
 * @file zlpPrepare.c
 * @brief ODBC API Internal Prepare Routines.
 */

/**
 * @addtogroup zlpPrepare
 * @{
 */

#if defined( ODBC_ALL )
stlStatus (*gZlpPrepareFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc               * aDbc,
                                                      zlsStmt              * aStmt,
                                                      stlChar              * aSql,
                                                      stlBool              * aResultSet,
                                                      stlBool              * aIsWithoutHoldCursor,
                                                      stlBool              * aIsUpdatable,
                                                      zllCursorSensitivity * aSensitivity,
                                                      stlBool              * aSuccessWithInfo,
                                                      stlErrorStack        * aErrorStack ) =
{
    zlapPrepare,
    zlcpPrepare
};
#endif

stlStatus zlpPrepare( zlsStmt       * aStmt,
                      stlChar       * aSql,
                      stlBool       * aSuccessWithInfo,
                      stlErrorStack * aErrorStack )
{
    zlcDbc             * sDbc;
    stlBool              sResultSet;
    stlBool              sIsWithoutHoldCursor = STL_FALSE;
    stlBool              sIsUpdatable         = STL_FALSE;
    zllCursorSensitivity sSensitivity         = ZLL_CURSOR_SENSITIVITY_ASENSITIVE;
    stlBool              sOptionChanged       = STL_FALSE;
    stlStatus            sRet                 = STL_FAILURE;

    sDbc = aStmt->mParentDbc;

    *aSuccessWithInfo = STL_FALSE;

    switch( aStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S1 :
            aStmt->mCursorConcurrencyOrg = aStmt->mCursorConcurrency;
            aStmt->mCursorSensitivityOrg = aStmt->mCursorSensitivity;
            aStmt->mCursorHoldableOrg    = aStmt->mCursorHoldable;
            break;
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            aStmt->mCursorConcurrency = aStmt->mCursorConcurrencyOrg;
            aStmt->mCursorSensitivity = aStmt->mCursorSensitivityOrg;
            aStmt->mCursorHoldable    = aStmt->mCursorHoldableOrg;
            break;
        case ZLS_TRANSITION_STATE_S5 :
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    aStmt->mReadBlockCount4Imp   = 0;
    aStmt->mNeedResultDescriptor = STL_TRUE;

    aStmt->mParamCount = ZLS_PARAM_COUNT_UNKNOWN;

#if defined( ODBC_ALL )
    STL_TRY( gZlpPrepareFunc[sDbc->mProtocolType]( sDbc,
                                                   aStmt,
                                                   aSql,
                                                   &sResultSet,
                                                   &sIsWithoutHoldCursor,
                                                   &sIsUpdatable,
                                                   &sSensitivity,
                                                   aSuccessWithInfo,
                                                   aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlapPrepare( sDbc,
                          aStmt,
                          aSql,
                          &sResultSet,
                          &sIsWithoutHoldCursor,
                          &sIsUpdatable,
                          &sSensitivity,
                          aSuccessWithInfo,
                          aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcpPrepare( sDbc,
                          aStmt,
                          aSql,
                          &sResultSet,
                          &sIsWithoutHoldCursor,
                          &sIsUpdatable,
                          &sSensitivity,
                          aSuccessWithInfo,
                          aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif

    /*
     * CURSOR 속성 확인
     */

    if( sResultSet == STL_TRUE )
    {
        if( sIsWithoutHoldCursor == STL_TRUE )
        {
            sDbc->mHasWithoutHoldCursor = STL_TRUE;
            
            if( aStmt->mCursorHoldable == SQL_HOLDABLE )
            {
                sOptionChanged = STL_TRUE;
            }

            aStmt->mCursorHoldable = SQL_NONHOLDABLE;
        }
        else
        {
            if( aStmt->mCursorHoldable == SQL_NONHOLDABLE )
            {
                sOptionChanged = STL_TRUE;
            }

            aStmt->mCursorHoldable = SQL_HOLDABLE;
        }

        aStmt->mPreparedCursorHoldable = aStmt->mCursorHoldable;

        switch( sSensitivity )
        {
            case ZLL_CURSOR_SENSITIVITY_INSENSITIVE :
                if( aStmt->mCursorSensitivity == SQL_SENSITIVE )
                {
                    sOptionChanged = STL_TRUE;
                }

                aStmt->mCursorSensitivity = SQL_INSENSITIVE;
                break;
            case ZLL_CURSOR_SENSITIVITY_SENSITIVE :
                if( aStmt->mCursorSensitivity == SQL_INSENSITIVE )
                {
                    sOptionChanged = STL_TRUE;
                }

                aStmt->mCursorSensitivity = SQL_SENSITIVE;
                break;
            default :
                STL_ASSERT( STL_FALSE );
                break;
        }

        aStmt->mPreparedCursorSensitivity = aStmt->mCursorSensitivity;
        
        if( aStmt->mCursorConcurrency != ZLS_CONCURRENCY_NA )
        {
            if( sIsUpdatable == STL_TRUE )
            {
                if( aStmt->mCursorConcurrency != SQL_CONCUR_LOCK )
                {
                    aStmt->mCursorConcurrency = SQL_CONCUR_LOCK;
                    sOptionChanged = STL_TRUE;
                }
            }
            else
            {
                if( aStmt->mCursorConcurrency != SQL_CONCUR_READ_ONLY )
                {
                    aStmt->mCursorConcurrency = SQL_CONCUR_READ_ONLY;
                    sOptionChanged = STL_TRUE;
                }
            }
        }
        else
        {
            if( sIsUpdatable == STL_TRUE )
            {
                aStmt->mCursorConcurrency = SQL_CONCUR_LOCK;
            }
            else
            {
                aStmt->mCursorConcurrency = SQL_CONCUR_READ_ONLY;
            }
        }

        aStmt->mPreparedCursorConcurrency = aStmt->mCursorConcurrency;
    }

    if( (aStmt->mCursorHoldable == SQL_NONHOLDABLE) &&
        (sResultSet == STL_TRUE) &&
        (sDbc->mAttrAutoCommit == STL_TRUE) )
    {
        /*
         * AUTOCOMMIT시 INSERT/UPDATE/DELETE RETURNING 의 result set은 존재하지 않는다.
         */
        sResultSet = STL_FALSE;
    }
    
    if( sResultSet == STL_FALSE )
    {
        aStmt->mTransition = ZLS_TRANSITION_STATE_S2;
    }
    else
    {
        aStmt->mTransition = ZLS_TRANSITION_STATE_S3;
    }
    
    STL_TRY_THROW( sOptionChanged == STL_FALSE, RAMP_WARNINGS_OPTION_VALUE_CHANGED );
    
    sRet = STL_SUCCESS;
    
    return sRet;

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_WARNINGS_OPTION_VALUE_CHANGED )
    {
        sRet              = STL_SUCCESS;
        *aSuccessWithInfo = STL_TRUE;
    
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_OPTION_VALUE_CHANGED,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    if( sRet == STL_FAILURE )
    {
        switch( aStmt->mTransition )
        {
            case ZLS_TRANSITION_STATE_S2 :
            case ZLS_TRANSITION_STATE_S3 :
            case ZLS_TRANSITION_STATE_S4 :
                aStmt->mTransition = ZLS_TRANSITION_STATE_S1;
                break;
            default :
                break;
        }
    }

    return sRet;
}

/** @} */
