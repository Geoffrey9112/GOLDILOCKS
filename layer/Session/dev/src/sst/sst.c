/*******************************************************************************
 * sst.c
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

/**
 * @file sst.c
 * @brief Session Statement Routines
 */

#include <qll.h>
#include <sslDef.h>
#include <ssc.h>

stlStatus sstFind( stlInt64        aStatementId,
                   sslStatement ** aStatement,
                   sslEnv        * aEnv )
{
    sslSessionEnv * sSessionEnv;
    sslStatement  * sStatement = NULL;
    stlInt32        sSlot;

    sSessionEnv = SSL_SESS_ENV( aEnv );

    sSlot = SSL_GET_STATEMENT_SLOT( aStatementId );
    sStatement = sSessionEnv->mStatementSlot[sSlot];

    STL_TRY_THROW( sStatement != NULL, RAMP_ERR_INVALID_STATEMENT );
    STL_TRY_THROW( SSL_GET_STATEMENT_SEQ( sStatement->mId ) == 
                   SSL_GET_STATEMENT_SEQ( aStatementId ),
                   RAMP_ERR_INVALID_STATEMENT);

    sSessionEnv->mLatestStatement = sStatement;
    
    *aStatement = sStatement;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STATEMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_INVALID_STATEMENT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;         
}

