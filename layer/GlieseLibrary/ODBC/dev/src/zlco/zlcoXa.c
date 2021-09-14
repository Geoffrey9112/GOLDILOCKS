/*******************************************************************************
 * zlcoXa.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlcoXa.c 12017 2014-04-15 05:44:19Z mae113 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <cml.h>
#include <goldilocks.h>
#include <zlDef.h>
#include <zle.h>
#include <zlc.h>

/**
 * @file zlcoXa.c
 * @brief ODBC API Internal XA Routines.
 */

/**
 * @addtogroup zloXa
 * @{
 */

stlStatus zlcoClose( zlcDbc        * aDbc,
                     stlInt32      * aReturn,
                     stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle = &aDbc->mComm;
    stlInt16    sResultType;

    STL_TRY_THROW( cmlWriteXaCloseCommand( sHandle,
                                           aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    STL_TRY( cmlSendPacket( sHandle, aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( cmlReadXaResult( sHandle,
                                    &sResultType,
                                    aReturn,
                                    aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_RESULT );

    STL_DASSERT( sResultType == CML_COMMAND_XA_CLOSE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        *aReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_RESULT )
    {
        /**
         * network error
         */
        if( *aReturn == XA_OK )
        {
            *aReturn = XAER_RMFAIL;
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcoStart( zlcDbc        * aDbc,
                     stlXid        * aXid,
                     stlInt32        aRmId,
                     stlInt32        aFlags,
                     stlInt32      * aReturn,
                     stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle = &aDbc->mComm;
    stlInt16    sResultType;

    STL_TRY_THROW( cmlWriteXaCommand( sHandle,
                                      CML_COMMAND_XA_START,
                                      aXid,
                                      aRmId,
                                      (stlInt64)aFlags,
                                      aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    STL_TRY( cmlSendPacket( sHandle, aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( cmlReadXaResult( sHandle,
                                    &sResultType,
                                    aReturn,
                                    aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_RESULT );
    
    STL_DASSERT( sResultType == CML_COMMAND_XA_START );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        *aReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_RESULT )
    {
        /**
         * network error
         */
        if( *aReturn == XA_OK )
        {
            *aReturn = XAER_RMFAIL;
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcoEnd( zlcDbc        * aDbc,
                   stlXid        * aXid,
                   stlInt32        aRmId,
                   stlInt32        aFlags,
                   stlInt32      * aReturn,
                   stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle = &aDbc->mComm;
    stlInt16    sResultType;

    STL_TRY_THROW( cmlWriteXaCommand( sHandle,
                                      CML_COMMAND_XA_END,
                                      aXid,
                                      aRmId,
                                      (stlInt64)aFlags,
                                      aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    STL_TRY( cmlSendPacket( sHandle, aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( cmlReadXaResult( sHandle,
                                    &sResultType,
                                    aReturn,
                                    aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_RESULT );
    
    STL_DASSERT( sResultType == CML_COMMAND_XA_END );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        *aReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_RESULT )
    {
        /**
         * network error
         */
        if( *aReturn == XA_OK )
        {
            *aReturn = XAER_RMFAIL;
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcoRollback( zlcDbc        * aDbc,
                        stlXid        * aXid,
                        stlInt32        aRmId,
                        stlInt32        aFlags,
                        stlInt32      * aReturn,
                        stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle = &aDbc->mComm;
    stlInt16    sResultType;

    STL_TRY_THROW( cmlWriteXaCommand( sHandle,
                                      CML_COMMAND_XA_ROLLBACK,
                                      aXid,
                                      aRmId,
                                      (stlInt64)aFlags,
                                      aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    STL_TRY( cmlSendPacket( sHandle, aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( cmlReadXaResult( sHandle,
                                    &sResultType,
                                    aReturn,
                                    aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_RESULT );
    
    STL_DASSERT( sResultType == CML_COMMAND_XA_ROLLBACK );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        *aReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_RESULT )
    {
        /**
         * network error
         */
        if( *aReturn == XA_OK )
        {
            *aReturn = XAER_RMFAIL;
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcoPrepare( zlcDbc        * aDbc,
                       stlXid        * aXid,
                       stlInt32        aRmId,
                       stlInt32        aFlags,
                       stlInt32      * aReturn,
                       stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle = &aDbc->mComm;
    stlInt16    sResultType;

    STL_TRY_THROW( cmlWriteXaCommand( sHandle,
                                      CML_COMMAND_XA_PREPARE,
                                      aXid,
                                      aRmId,
                                      (stlInt64)aFlags,
                                      aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    STL_TRY( cmlSendPacket( sHandle, aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( cmlReadXaResult( sHandle,
                                    &sResultType,
                                    aReturn,
                                    aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_RESULT );
    
    STL_DASSERT( sResultType == CML_COMMAND_XA_PREPARE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        *aReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_RESULT )
    {
        /**
         * network error
         */
        if( *aReturn == XA_OK )
        {
            *aReturn = XAER_RMFAIL;
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcoCommit( zlcDbc        * aDbc,
                      stlXid        * aXid,
                      stlInt32        aRmId,
                      stlInt32        aFlags,
                      stlInt32      * aReturn,
                      stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle = &aDbc->mComm;
    stlInt16    sResultType;

    STL_TRY_THROW( cmlWriteXaCommand( sHandle,
                                      CML_COMMAND_XA_COMMIT,
                                      aXid,
                                      aRmId,
                                      (stlInt64)aFlags,
                                      aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    STL_TRY( cmlSendPacket( sHandle, aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( cmlReadXaResult( sHandle,
                                    &sResultType,
                                    aReturn,
                                    aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_RESULT );
    
    STL_DASSERT( sResultType == CML_COMMAND_XA_COMMIT );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        *aReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_RESULT )
    {
        /**
         * network error
         */
        if( *aReturn == XA_OK )
        {
            *aReturn = XAER_RMFAIL;
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcoRecover( zlcDbc        * aDbc,
                       stlXid        * aXids,
                       stlInt32        aCount,
                       stlInt32        aRmId,
                       stlInt32        aFlags,
                       stlInt32      * aReturn,
                       stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle = &aDbc->mComm;
    stlInt64    sCount = 0;

    STL_TRY_THROW( cmlWriteXaRecoverCommand( sHandle,
                                             (stlInt64)aCount,
                                             aRmId,
                                             (stlInt64)aFlags,
                                             aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    STL_TRY( cmlSendPacket( sHandle, aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( cmlReadXaRecoverResult( sHandle,
                                           &sCount,
                                           aXids,
                                           aReturn,
                                           aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_RESULT );

    *aReturn = sCount;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        *aReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_RESULT )
    {
        /**
         * network error
         */
        if( *aReturn == XA_OK )
        {
            *aReturn = XAER_RMFAIL;
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcoForget( zlcDbc        * aDbc,
                      stlXid        * aXid,
                      stlInt32        aRmId,
                      stlInt32        aFlags,
                      stlInt32      * aReturn,
                      stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle = &aDbc->mComm;
    stlInt16    sResultType;

    STL_TRY_THROW( cmlWriteXaCommand( sHandle,
                                      CML_COMMAND_XA_FORGET,
                                      aXid,
                                      aRmId,
                                      (stlInt64)aFlags,
                                      aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    STL_TRY( cmlSendPacket( sHandle, aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( cmlReadXaResult( sHandle,
                                    &sResultType,
                                    aReturn,
                                    aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_RESULT );
    
    STL_DASSERT( sResultType == CML_COMMAND_XA_FORGET );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        *aReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_RESULT )
    {
        /**
         * network error
         */
        if( *aReturn == XA_OK )
        {
            *aReturn = XAER_RMFAIL;
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
