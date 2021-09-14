/*******************************************************************************
 * zlctTrans.c
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

/**
 * @file zlctTrans.c
 * @brief ODBC API Transaction Routines.
 */

/**
 * @addtogroup zlctTrans
 * @{
 */

stlStatus zlctEndTran( zlcDbc        * aDbc,
                       SQLSMALLINT     aCompletionType,
                       stlErrorStack * aErrorStack )
{
    cmlHandle         * sHandle;
    zllCompletionType   sCompletionType = ZLL_COMPLETION_TYPE_COMMIT;

    sHandle = &aDbc->mComm;
    
    switch( aCompletionType )
    {
        case SQL_COMMIT :
            sCompletionType = ZLL_COMPLETION_TYPE_COMMIT;
            break;
        case SQL_ROLLBACK :
            sCompletionType = ZLL_COMPLETION_TYPE_ROLLBACK;
            break;
        default :
            STL_ASSERT( STL_FALSE );
            break;
    }

    STL_TRY( cmlWriteTransactionCommand( sHandle,
                                         CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                         sCompletionType,
                                         aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlSendPacket( sHandle, aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlReadTransactionResult( sHandle, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
