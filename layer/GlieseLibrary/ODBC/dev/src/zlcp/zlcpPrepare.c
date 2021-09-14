/*******************************************************************************
 * zlcpPrepare.c
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

/**
 * @file zlcpPrepare.c
 * @brief ODBC API Internal Prepare Routines.
 */

/**
 * @addtogroup zlcpPrepare
 * @{
 */

stlStatus zlcpPrepare( zlcDbc               * aDbc,
                       zlsStmt              * aStmt,
                       stlChar              * aSql,
                       stlBool              * aResultSet,
                       stlBool              * aIsWithoutHoldCursor,
                       stlBool              * aIsUpdatable,
                       zllCursorSensitivity * aSensitivity,
                       stlBool              * aSuccessWithInfo,
                       stlErrorStack        * aErrorStack )
{
    cmlHandle * sHandle              = NULL;
    stlBool     sHasTransaction      = STL_FALSE;
    stlInt32    sStmtType            = ZLL_STMT_TYPE_UNKNOWN;

    stlInt8     sSensitivity;
    
    zldDesc   * sIrd;
    stlInt32    sColumnCount    = 0;
    stlInt32    sParamCount     = 0;
    stlInt32    sState          = 0;
    stlBool     sPrepareIgnored = STL_FALSE;
    stlBool     sIgnored        = STL_FALSE;

    sHandle = &aDbc->mComm;
    sIrd    = &aStmt->mIrd;

    STL_TRY( cmlWritePrepareCommand( sHandle,
                                     CML_CONTROL_HEAD,
                                     aStmt->mStmtId,
                                     (stlChar*)aSql,
                                     stlStrlen( aSql ),
                                     aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlWriteNumResultColsCommand( sHandle,
                                           CML_CONTROL_NONE,
                                           aStmt->mStmtId,
                                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlWriteNumParamsCommand( sHandle,
                                       CML_CONTROL_TAIL,
                                       aStmt->mStmtId,
                                       aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlSendPacket( sHandle,
                            aErrorStack ) == STL_SUCCESS );

    sState = 2;
    STL_TRY( cmlReadPrepareResult( sHandle,
                                   &sPrepareIgnored,
                                   &aStmt->mStmtId,
                                   &sStmtType,
                                   &sHasTransaction,
                                   aResultSet,
                                   aIsWithoutHoldCursor,
                                   aIsUpdatable,
                                   &sSensitivity,
                                   aErrorStack ) == STL_SUCCESS );

    /* SUCCESS_WITH_INFO */
    if( STL_HAS_ERROR( aErrorStack ) == STL_TRUE )
    {
        zldDiagAdds( SQL_HANDLE_STMT,
                     (void*)aStmt,
                     SQL_NO_ROW_NUMBER,
                     SQL_NO_COLUMN_NUMBER,
                     aErrorStack );

        *aSuccessWithInfo = STL_TRUE;
    }

    *aSensitivity = sSensitivity;

    zldDiagSetDynamicFunctionCode( &aStmt->mDiag, sStmtType );

    /*
     * IRD 할당
     */

    sState = 1;
    STL_TRY( cmlReadNumResultColsResult( sHandle,
                                         &sIgnored,
                                         &sColumnCount,
                                         aErrorStack ) == STL_SUCCESS );

    STL_TRY( zldDescReallocRec( sIrd,
                                (stlInt16)sColumnCount,
                                aErrorStack ) == STL_SUCCESS );

    /*
     * Parameter
     */
    sState = 0;
    STL_TRY( cmlReadNumParamsResult( sHandle,
                                     &sIgnored,
                                     &sParamCount,
                                     aErrorStack ) == STL_SUCCESS );

    aStmt->mParamCount = (stlInt16)sParamCount;
    
    STL_TRY_THROW( sPrepareIgnored == STL_FALSE, RAMP_IGNORE );
    
    aStmt->mIsPrepared = STL_TRUE;
                         
    if( aDbc->mTransition == ZLC_TRANSITION_STATE_C5 )
    {
        if( aDbc->mAttrAutoCommit == STL_TRUE )
        {
            STL_TRY( cmlWriteTransactionCommand( sHandle,
                                                 CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                                 ZLL_COMPLETION_TYPE_COMMIT,
                                                 aErrorStack ) == STL_SUCCESS );

            STL_TRY( cmlSendPacket( sHandle,
                                    aErrorStack ) == STL_SUCCESS );

            STL_TRY( cmlReadTransactionResult( sHandle,
                                               aErrorStack ) == STL_SUCCESS );
        }
        else
        {
            if( sHasTransaction == STL_TRUE )
            {
                aDbc->mTransition = ZLC_TRANSITION_STATE_C6;
            }
        }
    }

    STL_RAMP( RAMP_IGNORE );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2 :
            (void)cmlReadNumResultColsResult( sHandle,
                                              &sIgnored,
                                              &sColumnCount,
                                              aErrorStack );
        case 1 :
            (void)cmlReadNumParamsResult( sHandle,
                                          &sIgnored,
                                          &sParamCount,
                                          aErrorStack );
        default:
            break;
    }

    if( aDbc->mTransition == ZLC_TRANSITION_STATE_C5 )
    {
        if( aDbc->mAttrAutoCommit == STL_TRUE )
        {
            (void)cmlWriteTransactionCommand( sHandle,
                                              CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                              ZLL_COMPLETION_TYPE_ROLLBACK,
                                              aErrorStack );
            (void)cmlSendPacket( sHandle,
                                 aErrorStack );
            (void)cmlReadTransactionResult( sHandle, aErrorStack );
        }
    }
    
    return STL_FAILURE;
}

/** @} */
