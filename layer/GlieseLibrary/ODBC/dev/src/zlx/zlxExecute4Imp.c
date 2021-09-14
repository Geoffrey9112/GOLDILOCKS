/*******************************************************************************
 * zlxExecute4Imp.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlxExecute4Imp.c 12017 2014-04-15 05:44:19Z mae113 $
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
 * @file zlxExecute4Imp.c
 * @brief ODBC API Internal Execute Routines.
 */

/**
 * @addtogroup zlxExecute
 * @{
 */

#if defined( ODBC_ALL )
stlStatus (*gZlxExecute4ImpFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc             * aDbc,
                                                          zlsStmt            * aStmt,
                                                          stlUInt16            aParameterCount,
                                                          dtlValueBlockList ** aParameterData, 
                                                          stlInt64           * aAffectedRowCount,
                                                          stlBool            * aSuccessWithInfo,
                                                          stlErrorStack      * aErrorStack ) =
{
    zlaxExecute4Imp,
    zlcxExecute4Imp
};
#endif

stlStatus zlxExecute4Imp( zlsStmt            * aStmt,
                          stlUInt16            aParameterCount,
                          dtlValueBlockList ** aParameterData, 
                          stlInt64           * aAffectedRowCount,
                          stlBool            * aSuccessWithInfo,
                          stlErrorStack      * aErrorStack )
{
    zlcDbc * sDbc = aStmt->mParentDbc;
    
    switch( aStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            break;
        case ZLS_TRANSITION_STATE_S5 :
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

#if defined( ODBC_ALL )
    STL_TRY( gZlxExecute4ImpFunc[sDbc->mProtocolType]( sDbc,
                                                       aStmt,
                                                       aParameterCount,
                                                       aParameterData,
                                                       aAffectedRowCount,
                                                       aSuccessWithInfo,
                                                       aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlaxExecute4Imp( sDbc,
                              aStmt,
                              aParameterCount,
                              aParameterData,
                              aAffectedRowCount,
                              aSuccessWithInfo,
                              aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcxExecute4Imp( sDbc,
                              aStmt,
                              aParameterCount,
                              aParameterData,
                              aAffectedRowCount,
                              aSuccessWithInfo,
                              aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif

    switch( aStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S2 :
            aStmt->mTransition = ZLS_TRANSITION_STATE_S4;
            break;
        case ZLS_TRANSITION_STATE_S3 :
            aStmt->mTransition = ZLS_TRANSITION_STATE_S5;
            break;
        case ZLS_TRANSITION_STATE_S4 :
            aStmt->mTransition = ZLS_TRANSITION_STATE_S4;
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The StatementHandle was not prepared.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle.",
                      aErrorStack );
    }

    STL_FINISH;

    switch( aStmt->mTransition )
    {
        case ZLC_TRANSITION_STATE_C4 :
            aStmt->mTransition = ZLS_TRANSITION_STATE_S2;
            break;
        default:
            break;
    }

    return STL_FAILURE;
}

/** @} */
