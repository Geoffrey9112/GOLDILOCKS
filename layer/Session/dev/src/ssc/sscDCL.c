/*******************************************************************************
 * sscDCL.c
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
 * @file sscDCL.c
 * @brief Session Layer DCL Command Internal Routines
 */


#include <qll.h>
#include <sslDef.h>
#include <sslSession.h>

stlStatus sscSetDCL( sslStatement  * aStatement,
                     sslDCLContext * aDCLContext,
                     sslEnv        * aEnv )
{
    sslSessionEnv   * sSessionEnv;
    qllDCLType        sDCLType;
    qllAccessMode     sTxnAccessMode;
    sslIsolationLevel sIsolationLevel;
    qllUniqueMode     sTxnUniqueMode;
    
    sSessionEnv = SSL_SESS_ENV( aEnv );

    sDCLType = qllGetDCLType( &aStatement->mSqlStmt );

    switch( sDCLType )
    {
        case QLL_DCL_TYPE_END_TRANSACTION:
            sSessionEnv->mTransId = SML_INVALID_TRANSID;
            knlSetTransScope( sSessionEnv, KNL_TRANS_SCOPE_NONE );

            STL_TRY( sslSetTransactionMode( sSessionEnv, aEnv ) == STL_SUCCESS );

            aDCLContext->mType = SSL_DCL_TYPE_END_TRANSACTION;
            break;
        case QLL_DCL_TYPE_SET_TRANSACTION_ACCESS_MODE:
            sTxnAccessMode = (qllAccessMode)qllGetDCLAccessMode( &aStatement->mSqlStmt );
            if( sSessionEnv->mTransId == SML_INVALID_TRANSID )
            {
                qllSetTransAccessMode( sTxnAccessMode, QLL_ENV(aEnv) );
                sSessionEnv->mNextAccessMode = QLL_ACCESS_MODE_NONE;
            }
            else
            {
                sSessionEnv->mNextAccessMode = sTxnAccessMode;
            }
            break;
        case QLL_DCL_TYPE_SET_TRANSACTION_ISOLATION_LEVEL:
            sIsolationLevel = (sslIsolationLevel)qllGetDCLIsolationLevel( &aStatement->mSqlStmt );
            if( sSessionEnv->mTransId == SML_INVALID_TRANSID )
            {
                sSessionEnv->mCurrTxnIsolation = sIsolationLevel;
                sSessionEnv->mNextTxnIsolation = SSL_ISOLATION_LEVEL_NONE;
            }
            else
            {
                sSessionEnv->mNextTxnIsolation = sIsolationLevel;
            }
            break;
        case QLL_DCL_TYPE_SET_TRANSACTION_UNIQUE_INTEGRITY:
            sTxnUniqueMode = (qllUniqueMode)qllGetDCLUniqueMode( &aStatement->mSqlStmt );
            if( sSessionEnv->mTransId == SML_INVALID_TRANSID )
            {
                qllSetTransUniqueMode( sTxnUniqueMode, QLL_ENV(aEnv) );
                sSessionEnv->mNextUniqueMode = QLL_UNIQUE_MODE_NONE;
            }
            else
            {
                sSessionEnv->mNextUniqueMode = sTxnUniqueMode;
            }
            break;
        case QLL_DCL_TYPE_SET_SESSION_CHARACTERISTICS_ACCESS_MODE :
            sTxnAccessMode = (qllAccessMode)qllGetDCLAccessMode( &aStatement->mSqlStmt );
            
            sSessionEnv->mDefaultAccessMode = sTxnAccessMode;
            
            if( sSessionEnv->mTransId == SML_INVALID_TRANSID )
            {
                qllSetTransAccessMode( sTxnAccessMode, QLL_ENV(aEnv) );
                sSessionEnv->mNextAccessMode = QLL_ACCESS_MODE_NONE;
            }
            else
            {
                sSessionEnv->mNextAccessMode = sTxnAccessMode;
            }
            break;
        case QLL_DCL_TYPE_SET_SESSION_CHARACTERISTICS_ISOLATION_LEVEL :
            sIsolationLevel = (sslIsolationLevel)qllGetDCLIsolationLevel( &aStatement->mSqlStmt );

            sSessionEnv->mDefaultTxnIsolation = sIsolationLevel;

            if( sSessionEnv->mTransId == SML_INVALID_TRANSID )
            {
                sSessionEnv->mCurrTxnIsolation = sIsolationLevel;
                sSessionEnv->mNextTxnIsolation = SSL_ISOLATION_LEVEL_NONE;
            }
            else
            {
                sSessionEnv->mNextTxnIsolation = sIsolationLevel;
            }
            break;
        case QLL_DCL_TYPE_SET_SESSION_CHARACTERISTICS_UNIQUE_INTEGRITY :
            sTxnUniqueMode = (qllUniqueMode)qllGetDCLUniqueMode( &aStatement->mSqlStmt );
            
            sSessionEnv->mDefaultUniqueMode = sTxnUniqueMode;
            
            if( sSessionEnv->mTransId == SML_INVALID_TRANSID )
            {
                qllSetTransUniqueMode( sTxnUniqueMode, QLL_ENV(aEnv) );
                sSessionEnv->mNextUniqueMode = QLL_UNIQUE_MODE_NONE;
            }
            else
            {
                sSessionEnv->mNextUniqueMode = sTxnUniqueMode;
            }
            break;
        case QLL_DCL_TYPE_SET_TIME_ZONE:
            aDCLContext->mType      = SSL_DCL_TYPE_SET_TIME_ZONE;
            aDCLContext->mGMTOffset = qllGetDCLGMTOffset( &aStatement->mSqlStmt );
            break;            
        case QLL_DCL_TYPE_START_TRANSACTION:
            break;
        case QLL_DCL_TYPE_OPEN_DATABASE:
            aDCLContext->mType              = SSL_DCL_TYPE_OPEN_DATABASE;
            aDCLContext->mCharacterSet      = ellGetDbCharacterSet();
            /**
             * @todo NCHAR 지원시 올바른 값을 설정해야 한다.
             */
            aDCLContext->mNCharCharacterSet = DTL_CHARACTERSET_MAX;
            break;
        case QLL_DCL_TYPE_CLOSE_DATABASE:
            aDCLContext->mType = SSL_DCL_TYPE_CLOSE_DATABASE;
            break;
        default:
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
