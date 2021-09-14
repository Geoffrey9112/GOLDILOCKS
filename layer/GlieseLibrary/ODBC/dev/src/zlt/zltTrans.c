/*******************************************************************************
 * zltTrans.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zltTrans.c 12017 2014-04-15 05:44:19Z mae113 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <cml.h>
#include <goldilocks.h>

#include <zlDef.h>
#include <zlr.h>

#include <zlat.h>
#include <zlct.h>

/**
 * @file zltTrans.c
 * @brief ODBC API Transaction Routines.
 */

/**
 * @addtogroup zltTrans
 * @{
 */

#if defined( ODBC_ALL )
stlStatus (*gZltEndTranFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                      SQLSMALLINT     aCompletionType,
                                                      stlErrorStack * aErrorStack ) =
{
    zlatEndTran,
    zlctEndTran
};
#endif

stlStatus zltEndTran( zlcDbc        * aDbc,
                      SQLSMALLINT     aCompletionType,
                      stlErrorStack * aErrorStack )
{
    stlErrorData  * sErrorData = NULL;

#if defined( ODBC_ALL )
    STL_TRY_THROW( gZltEndTranFunc[aDbc->mProtocolType]( aDbc,
                                                         aCompletionType,
                                                         aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_END_TRAN );
#elif defined( ODBC_DA )
    STL_TRY_THROW( zlatEndTran( aDbc,
                                aCompletionType,
                                aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_END_TRAN );
#elif defined( ODBC_CS )
    STL_TRY_THROW( zlctEndTran( aDbc,
                                aCompletionType,
                                aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_END_TRAN );
#else
    STL_ASSERT( STL_FALSE );
#endif

    STL_TRY( zlrCloseWithoutHold( aDbc,
                                  aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_END_TRAN )
    {
        sErrorData = stlGetLastErrorData( aErrorStack );

        if( sErrorData != NULL )
        {
            switch( stlGetExternalErrorCode( sErrorData ) )
            {
                case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_NO_SUBCLASS :
                case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_SERIALIZATION_FAILURE :
                case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_INTEGRITY_CONSTRAINT_VIOLATION :
                case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_STATEMENT_COMPLETION_UNKNOWN :
                case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_TRIGGERED_ACTION_EXCEPTION :
                    (void)zlrCloseWithoutHold( aDbc, aErrorStack );
                    aDbc->mTransition = ZLC_TRANSITION_STATE_C5;
                    break;
                default :
                    break;
            }
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
