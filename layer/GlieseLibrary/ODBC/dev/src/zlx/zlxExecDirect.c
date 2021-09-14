/*******************************************************************************
 * zlxExecDirect.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlxExecDirect.c 12017 2014-04-15 05:44:19Z mae113 $
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
 * @file zlxExecDirect.c
 * @brief ODBC API Internal Execute Routines.
 */

/**
 * @addtogroup zlxExecDirect
 * @{
 */

#if defined( ODBC_ALL )
stlStatus (*gZlxExecDirectFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc               * aDbc,
                                                         zlsStmt              * aStmt,
                                                         stlChar              * aSql,
                                                         stlBool              * aResultSet,
                                                         stlBool              * aIsWithoutHoldCursor,
                                                         stlBool              * aIsUpdatable,
                                                         zllCursorSensitivity * aSensitivity,
                                                         stlInt64             * aAffectedRowCount,
                                                         stlBool              * aSuccessWithInfo,
                                                         stlErrorStack        * aErrorStack ) =
{
    zlaxExecDirect,
    zlcxExecDirect
};
#endif

stlStatus zlxExecDirect( zlsStmt       * aStmt,
                         stlChar       * aSql,
                         stlInt64      * aAffectedRowCount,
                         stlBool       * aSuccessWithInfo,
                         stlErrorStack * aErrorStack )
{
    zlcDbc             * sDbc;
    stlBool              sResultSet           = STL_FALSE;
    stlBool              sIsWithoutHoldCursor = STL_FALSE;
    stlBool              sIsUpdatable         = STL_FALSE;
    zllCursorSensitivity sSensitivity         = ZLL_CURSOR_SENSITIVITY_ASENSITIVE;
    stlBool              sOptionChanged       = STL_FALSE;
    stlStatus            sRet                 = STL_FAILURE;

    sDbc = aStmt->mParentDbc;

    *aSuccessWithInfo = STL_FALSE;

    aStmt->mNeedResultDescriptor = STL_TRUE;
    aStmt->mIsPrepared = STL_FALSE;
    
    aStmt->mCursorConcurrencyOrg = aStmt->mCursorConcurrency;
    aStmt->mCursorSensitivityOrg = aStmt->mCursorSensitivity;
    aStmt->mCursorHoldableOrg    = aStmt->mCursorHoldable;

#if defined( ODBC_ALL )
    STL_TRY( gZlxExecDirectFunc[sDbc->mProtocolType]( sDbc,
                                                      aStmt,
                                                      aSql,
                                                      &sResultSet,
                                                      &sIsWithoutHoldCursor,
                                                      &sIsUpdatable,
                                                      &sSensitivity,
                                                      aAffectedRowCount,
                                                      aSuccessWithInfo,
                                                      aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlaxExecDirect( sDbc,
                             aStmt,
                             aSql,
                             &sResultSet,
                             &sIsWithoutHoldCursor,
                             &sIsUpdatable,
                             &sSensitivity,
                             aAffectedRowCount,
                             aSuccessWithInfo,
                             aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcxExecDirect( sDbc,
                             aStmt,
                             aSql,
                             &sResultSet,
                             &sIsWithoutHoldCursor,
                             &sIsUpdatable,
                             &sSensitivity,
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
        aStmt->mTransition = ZLS_TRANSITION_STATE_S4;
    }
    else
    {
        aStmt->mTransition = ZLS_TRANSITION_STATE_S5;
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
            case ZLC_TRANSITION_STATE_C4 :
            case ZLC_TRANSITION_STATE_C5 :
                aStmt->mTransition = ZLS_TRANSITION_STATE_S1;
                break;
            default:
                break;
        }
    }
    
    return sRet;
}

/** @} */
