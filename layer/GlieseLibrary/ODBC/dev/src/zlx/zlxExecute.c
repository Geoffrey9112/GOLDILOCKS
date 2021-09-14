/*******************************************************************************
 * zlxExecute.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlxExecute.c 12017 2014-04-15 05:44:19Z mae113 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <cml.h>
#include <goldilocks.h>

#include <zlDef.h>
#include <zle.h>

#include <zlax.h>
#include <zlcx.h>

/**
 * @file zlxExecute.c
 * @brief ODBC API Internal Execute Routines.
 */

/**
 * @addtogroup zlxExecute
 * @{
 */

#if defined( ODBC_ALL )
stlStatus (*gZlxExecuteFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                      zlsStmt       * aStmt,
                                                      stlBool       * aResultSet,
                                                      stlInt64      * aAffectedRowCount,
                                                      stlBool       * aSuccessWithInfo,
                                                      stlErrorStack * aErrorStack ) =
{
    zlaxExecute,
    zlcxExecute
};
#endif

stlStatus zlxExecute( zlsStmt       * aStmt,
                      stlInt64      * aAffectedRowCount,
                      stlBool       * aSuccessWithInfo,
                      stlErrorStack * aErrorStack )
{
    stlStatus   sRet = STL_FAILURE;
    zlcDbc    * sDbc;
    stlBool     sResultSet = STL_FALSE;
    stlBool     sOptionChanged = STL_FALSE;

    sDbc = aStmt->mParentDbc;

    *aSuccessWithInfo = STL_FALSE;

#if defined( ODBC_ALL )
    STL_TRY( gZlxExecuteFunc[sDbc->mProtocolType]( sDbc,
                                                   aStmt,
                                                   &sResultSet,
                                                   aAffectedRowCount,
                                                   aSuccessWithInfo,
                                                   aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlaxExecute( sDbc,
                          aStmt,
                          &sResultSet,
                          aAffectedRowCount,
                          aSuccessWithInfo,
                          aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcxExecute( sDbc,
                          aStmt,
                          &sResultSet,
                          aAffectedRowCount,
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
        if( aStmt->mPreparedCursorHoldable != aStmt->mCursorHoldable )
        {
            if( aStmt->mCursorHoldable != ZLS_HOLDABILITY_NA )
            {
                sOptionChanged = STL_TRUE;
            }

            aStmt->mCursorHoldable = aStmt->mPreparedCursorHoldable;
        }

        switch( aStmt->mPreparedCursorSensitivity )
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

        if( aStmt->mPreparedCursorConcurrency != aStmt->mCursorConcurrency )
        {
            if( aStmt->mCursorConcurrency != ZLS_CONCURRENCY_NA )
            {
                sOptionChanged = STL_TRUE;
            }

            aStmt->mCursorConcurrency = aStmt->mPreparedCursorConcurrency;
        }
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
    
    switch( aStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S2 :
            aStmt->mTransition = ZLS_TRANSITION_STATE_S4;
            break;
        case ZLS_TRANSITION_STATE_S3 :
            aStmt->mTransition = ZLS_TRANSITION_STATE_S5;
            break;
        case ZLS_TRANSITION_STATE_S4 :
            if( sResultSet == STL_FALSE )
            {
                aStmt->mTransition = ZLS_TRANSITION_STATE_S4;
            }
            else
            {
                aStmt->mTransition = ZLS_TRANSITION_STATE_S5;
            }
            break;
    }

    STL_TRY_THROW( sOptionChanged == STL_FALSE, RAMP_WARNINGS_OPTION_VALUE_CHANGED );

    sRet = STL_SUCCESS;

    return sRet;

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
            case ZLS_TRANSITION_STATE_S4 :
                aStmt->mTransition = ZLS_TRANSITION_STATE_S2;
                break;
            default:
                break;
        }
    }
    
    return sRet;
}

/** @} */
