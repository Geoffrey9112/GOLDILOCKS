/*******************************************************************************
 * sslXaCommand.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sslXaCommand.c 9033 2013-07-17 08:40:27Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file sslXaCommand.c
 * @brief XA Session Command Routines
 */

#include <qll.h>
#include <sslDef.h>
#include <sslEnv.h>
#include <sslCommand.h>
#include <sslSession.h>
#include <sslStatement.h>
#include <ssDef.h>
#include <ssx.h>
#include <sst.h>

extern ssgWarmupEntry  * gSsgWarmupEntry;

/**
 * @brief XA CLOSE 시 Validation을 위한 함수
 * @param[in] aEnv          Environment Pointer
 * @remarks
 */
stlInt32 sslXaClose( sslEnv * aEnv )
{
    sslSessionEnv * sSessionEnv;
    stlInt32        sReturn = SSL_XA_OK;
    
    sSessionEnv = SSL_SESS_ENV( aEnv );
    
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY_THROW( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMFAIL );
    STL_TRY_THROW( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );
    
    STL_TRY_THROW( knlBreakPointWaitToStart( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    STL_TRY_THROW( knlGetTransScope( sSessionEnv ) != KNL_TRANS_SCOPE_GLOBAL,
                   RAMP_ERR_XA_PROTO );
    
    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }

    STL_TRY( ssxLogMsg( NULL,
                        "xa_close",
                        0,
                        sReturn,
                        aEnv )
             == STL_SUCCESS );
                        
    return sReturn;

    STL_CATCH( RAMP_ERR_XA_RMFAIL )
    {
        sReturn = SSL_XAER_RMFAIL;
    }
    
    STL_CATCH( RAMP_ERR_XA_RMERR )
    {
        sReturn = SSL_XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_XA_PROTO )
    {
        sReturn = SSL_XAER_PROTO;
    }

    STL_FINISH;

    (void) ssxLogMsg( NULL,
                      "xa_close",
                      0,
                      sReturn,
                      aEnv );
    
    sslSetCallStack( aEnv );

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }
    
    return sReturn;
}


/**
 * @brief Global Transaction을 Commit한다.
 * @param[in] aXid          Global Transaction Identifier
 * @param[in] aRmId         Resource Manager Identifier
 * @param[in] aFlags        Option Flags
 * @param[in] aEnv          Environment Pointer
 * @remarks
 */
stlInt32 sslXaCommit( stlXid    * aXid,
                      stlInt32    aRmId,
                      stlInt64    aFlags,
                      sslEnv    * aEnv )
{
    sslSessionEnv      * sSessionEnv;
    knlXaState           sXaState;
    knlXaContext       * sXaContext;
    stlBool              sOnePhaseCommit;
    stlBool              sFound;
    stlInt32             sState = 0;
    smlTransId           sTransId;
    knlHeuristicDecision sHeuristicDecision;
    stlInt32             sReturn = SSL_XA_OK;
    stlInt32             sHashValue;
    stlBool              sIsSuccess;
    stlUInt32            sSessionId;
    stlErrorData       * sErrorData = NULL;
    
    sSessionEnv = SSL_SESS_ENV( aEnv );
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY_THROW( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMFAIL );
    STL_TRY_THROW( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );
    
    STL_TRY_THROW( knlBreakPointWaitToStart( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    STL_TRY( knlGetSessionEnvId( sSessionEnv,
                                 &sSessionId,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    sOnePhaseCommit = (aFlags & SSL_TM_ONEPHASE) ? STL_TRUE : STL_FALSE;

    sHashValue = knlGetXidHashValue( aXid );
    
    while( 1 )
    {
        STL_TRY_THROW( knlFindXaContext( aXid,
                                         sHashValue,
                                         &sXaContext,
                                         &sFound,
                                         KNL_ENV( aEnv ) ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );

        STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_XA_NOTA );
        
        STL_TRY_THROW( ssxLatchContext( sXaContext,
                                        aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
        sState = 1;

        /**
         * latch 획득 이후에 다시 검사한다.
         */
        
        STL_TRY_THROW( knlIsEqualXid( aXid,
                                      sHashValue,
                                      sXaContext ) == STL_TRUE,
                       RAMP_ERR_XA_NOTA );
        
        sXaState = sXaContext->mState;

        if( sXaState == KNL_XA_STATE_ACTIVE )
        {
            if( aFlags & SSL_TM_NOWAIT )
            {
                /**
                 * 다른 세션에서 사용중인 경우는 에러
                 */
                    
                STL_THROW( RAMP_ERR_XA_RETRY );
            }
            else
            {
                    
                /**
                 * xa_end를 호출하지 않고, xa_commit을 호출할수 없다.
                 */
                
                STL_TRY_THROW( sSessionId != sXaContext->mSessionId,
                               RAMP_ERR_XA_PROTO );
                    
                /**
                 * 다른 세션에서 사용중인 경우는 assocation을 suspend할때까지
                 * 기다린다.
                 */
                    
                sState = 0;
                STL_TRY_THROW( ssxUnlatchContext( sXaContext,
                                                  aEnv ) == STL_SUCCESS,
                               RAMP_ERR_XA_RMERR );
            
                STL_TRY_THROW( ssxWaitContext( sXaContext,
                                               &sIsSuccess,
                                               aEnv ) == STL_SUCCESS,
                               RAMP_ERR_XA_RMERR );

                STL_TRY_THROW( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS,
                               RAMP_ERR_XA_RMFAIL );
            
                continue;
            }
        }

        sTransId = sXaContext->mTransId;
        sHeuristicDecision = sXaContext->mHeuristicDecision;
        
        if( sXaState == KNL_XA_STATE_IDLE )
        {
            STL_TRY_THROW( sOnePhaseCommit == STL_TRUE, RAMP_ERR_XA_PROTO );
        }
        else
        {
            if( sXaState == KNL_XA_STATE_PREPARED )
            {
                STL_TRY_THROW( sOnePhaseCommit == STL_FALSE, RAMP_ERR_XA_PROTO );
            }
            else
            {
                if( sXaState == KNL_XA_STATE_ROLLBACK_ONLY )
                {
                    STL_TRY_THROW( ssxRollback( sTransId,
                                                sXaContext,
                                                aEnv ) == STL_SUCCESS,
                                   RAMP_ERR_XA_RMERR );
                }
                else
                {
                    STL_DASSERT( sXaState == KNL_XA_STATE_HEURISTIC_COMPLETED );
                }
            }
        }

        STL_TRY_THROW( knlUnlinkXaContext( sXaContext,
                                           KNL_ENV( aEnv ) ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
        
        sState = 0;
        STL_TRY_THROW( ssxUnlatchContext( sXaContext,
                                          aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );

        STL_TRY_THROW( ssxDestroyContext( sXaContext,
                                          aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );

        STL_TRY_THROW( sXaState != KNL_XA_STATE_ROLLBACK_ONLY,
                       RAMP_ERR_XA_ROLLBACK_ONLY );
        
        break;
    }

    if( sHeuristicDecision == KNL_XA_HEURISTIC_DECISION_NONE )
    {
        STL_DASSERT( sTransId != SML_INVALID_TRANSID );

        STL_TRY_THROW( ssxCommit( sTransId,
                                  sXaContext,
                                  aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_COMMIT );
    }
    else
    {
        STL_TRY_THROW( sHeuristicDecision == KNL_XA_HEURISTIC_DECISION_COMMITTED,
                       RAMP_ERR_XA_HEURRB );
        STL_TRY_THROW( sHeuristicDecision == KNL_XA_HEURISTIC_DECISION_ROLLED_BACK,
                       RAMP_ERR_XA_HEURCOM );
    }

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }

    STL_TRY( ssxLogMsg( aXid,
                        "xa_commit",
                        aFlags,
                        sReturn,
                        aEnv )
             == STL_SUCCESS );

    return sReturn;

    STL_CATCH( RAMP_ERR_XA_PROTO )
    {
        sReturn = SSL_XAER_PROTO;
    }

    STL_CATCH( RAMP_ERR_XA_RMFAIL )
    {
        sReturn = SSL_XAER_RMFAIL;
    }

    STL_CATCH( RAMP_ERR_XA_RMERR )
    {
        sReturn = SSL_XAER_RMERR;
    }

    STL_CATCH( RAMP_ERR_XA_HEURRB )
    {
        sReturn = SSL_XA_HEURRB;
    }

    STL_CATCH( RAMP_ERR_XA_HEURCOM )
    {
        sReturn = SSL_XA_HEURCOM;
    }

    STL_CATCH( RAMP_ERR_XA_NOTA )
    {
        sReturn = SSL_XAER_NOTA;
    }

    STL_CATCH( RAMP_ERR_XA_ROLLBACK_ONLY )
    {
        sReturn = SSL_XA_RBROLLBACK;
    }

    STL_CATCH( RAMP_ERR_XA_RETRY )
    {
        sReturn = SSL_XA_RETRY;
    }

    STL_CATCH( RAMP_ERR_XA_COMMIT )
    {
        sReturn = SSL_XAER_RMERR;

        /*
         *  The resource manager did not commit the work done on behalf of the transaction
         *  branch. Upon return, the resource manager has rolled back the branch’s work and
         *  has released all held resources. These values may be returned only if
         *  TMONEPHASE is set in flags
         */

        if( sOnePhaseCommit == STL_TRUE )
        {
            sErrorData = stlGetLastErrorData( KNL_ERROR_STACK( aEnv ) );

            STL_DASSERT( sErrorData != NULL );

            if( sErrorData != NULL )
            {
                switch( stlGetExternalErrorCode( sErrorData ) )
                {
                    case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_INTEGRITY_CONSTRAINT_VIOLATION :
                        sReturn = SSL_XA_RBINTEGRITY;
                        break;
                    case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_NO_SUBCLASS :
                    case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_STATEMENT_COMPLETION_UNKNOWN :
                    case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_TRIGGERED_ACTION_EXCEPTION :
                        sReturn = SSL_XA_RBROLLBACK;
                        break;
                    default :
                        break;
                }
            }
        }

        (void) ssxRollback( sTransId, sXaContext, aEnv );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void) ssxUnlatchContext( sXaContext, aEnv );
    }

    (void) ssxLogMsg( aXid,
                      "xa_commit",
                      aFlags,
                      sReturn,
                      aEnv );
    
    sslSetCallStack( aEnv );

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }
    
    return sReturn;
}
                      
/**
 * @brief Global Transaction을 Rollback한다.
 * @param[in] aXid          Global Transaction Identifier
 * @param[in] aRmId         Resource Manager Identifier
 * @param[in] aFlags        Option Flags
 * @param[in] aEnv          Environment Pointer
 * @remarks
 */
stlInt32 sslXaRollback( stlXid    * aXid,
                        stlInt32    aRmId,
                        stlInt64    aFlags,
                        sslEnv    * aEnv )
{
    sslSessionEnv      * sSessionEnv;
    knlXaState           sXaState;
    knlXaContext       * sXaContext;
    stlBool              sFound;
    stlInt32             sState = 0;
    smlTransId           sTransId;
    knlHeuristicDecision sHeuristicDecision;
    stlInt32             sReturn = SSL_XA_OK;
    stlInt32             sHashValue;
    
    sSessionEnv = SSL_SESS_ENV( aEnv );
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY_THROW( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMFAIL );
    STL_TRY_THROW( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );
    
    STL_TRY_THROW( knlBreakPointWaitToStart( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    sHashValue = knlGetXidHashValue( aXid );
    
    STL_TRY_THROW( knlFindXaContext( aXid,
                                     sHashValue,
                                     &sXaContext,
                                     &sFound,
                                     KNL_ENV( aEnv ) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_XA_NOTA );
        
    STL_TRY_THROW( ssxLatchContext( sXaContext,
                                    aEnv ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );
    sState = 1;

    /**
     * latch 획득 이후에 다시 검사한다.
     */
        
    STL_TRY_THROW( knlIsEqualXid( aXid,
                                  sHashValue,
                                  sXaContext ) == STL_TRUE,
                   RAMP_ERR_XA_NOTA );
        
    sXaState = sXaContext->mState;

    STL_TRY_THROW( (sXaState == KNL_XA_STATE_IDLE)          ||
                   (sXaState == KNL_XA_STATE_PREPARED)      ||
                   (sXaState == KNL_XA_STATE_ROLLBACK_ONLY) ||
                   (sXaState == KNL_XA_STATE_HEURISTIC_COMPLETED),
                   RAMP_ERR_XA_RMFAIL );
    
    sTransId = sXaContext->mTransId;
    sHeuristicDecision = sXaContext->mHeuristicDecision;
        
    STL_TRY_THROW( knlUnlinkXaContext( sXaContext,
                                       KNL_ENV( aEnv ) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );
    
    sState = 0;
    STL_TRY_THROW( ssxUnlatchContext( sXaContext,
                                      aEnv ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    STL_TRY_THROW( ssxDestroyContext( sXaContext,
                                      aEnv ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    if( sHeuristicDecision == KNL_XA_HEURISTIC_DECISION_NONE )
    {
        STL_DASSERT( sTransId != SML_INVALID_TRANSID );
        
        STL_TRY_THROW( ssxRollback( sTransId,
                                    sXaContext,
                                    aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
    }
    else
    {
        STL_TRY_THROW( sHeuristicDecision == KNL_XA_HEURISTIC_DECISION_COMMITTED,
                       RAMP_ERR_XA_HEURRB );
        STL_TRY_THROW( sHeuristicDecision == KNL_XA_HEURISTIC_DECISION_ROLLED_BACK,
                       RAMP_ERR_XA_HEURCOM );
    }

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) == KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }

    STL_TRY( ssxLogMsg( aXid,
                        "xa_rollback",
                        aFlags,
                        sReturn,
                        aEnv )
             == STL_SUCCESS );

    return sReturn;

    STL_CATCH( RAMP_ERR_XA_RMFAIL )
    {
        sReturn = SSL_XAER_RMFAIL;
    }
    
    STL_CATCH( RAMP_ERR_XA_RMERR )
    {
        sReturn = SSL_XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_XA_HEURRB )
    {
        sReturn = SSL_XA_HEURRB;
    }

    STL_CATCH( RAMP_ERR_XA_HEURCOM )
    {
        sReturn = SSL_XA_HEURCOM;
    }

    STL_CATCH( RAMP_ERR_XA_NOTA )
    {
        sReturn = SSL_XAER_NOTA;
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void) ssxUnlatchContext( sXaContext, aEnv );
    }

    (void) ssxLogMsg( aXid,
                      "xa_rollback",
                      aFlags,
                      sReturn,
                      aEnv );
    
    sslSetCallStack( aEnv );

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }
    
    return sReturn;
}
                      
/**
 * @brief Global Transaction을 Start한다.
 * @param[in] aXid          Global Transaction Identifier
 * @param[in] aRmId         Resource Manager Identifier
 * @param[in] aFlags        Option Flags
 * @param[in] aEnv          Environment Pointer
 * @remarks
 */
stlInt32 sslXaStart( stlXid    * aXid,
                     stlInt32    aRmId,
                     stlInt64    aFlags,
                     sslEnv    * aEnv )
{
    sslSessionEnv * sSessionEnv;
    knlXaState      sXaState;
    knlXaContext  * sXaContext;
    stlBool         sFound;
    stlInt32        sState = 0;
    stlBool         sIsSuccess;
    stlInt32        sReturn = SSL_XA_OK;
    stlInt32        sHashValue;
    stlUInt32       sSessionId;
    
    sSessionEnv = SSL_SESS_ENV( aEnv );
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY_THROW( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMFAIL );
    STL_TRY_THROW( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );
    
    STL_TRY_THROW( knlBreakPointWaitToStart( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    STL_TRY_THROW( knlGetTransScope( sSessionEnv ) != KNL_TRANS_SCOPE_LOCAL,
                   RAMP_ERR_XA_OUTSIDE );

    STL_TRY( knlGetSessionEnvId( sSessionEnv,
                                 &sSessionId,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY_THROW( smlHasActiveStatement( SML_ENV(aEnv) ) == STL_FALSE,
                   RAMP_ERR_XA_OUTSIDE );
    
    if( (aFlags & SSL_TM_RESUME) || (aFlags & SSL_TM_JOIN) )
    {
        if( (aFlags & SSL_TM_RESUME) && (aFlags & SSL_TM_JOIN) )
        {
            /**
             * RESUME & JOIN은 동시에 사용할수 없다.
             */
        
            STL_THROW( RAMP_ERR_XA_INVAL );
        }
        
        sHashValue = knlGetXidHashValue( aXid );
    
        while( 1 )
        {
            STL_TRY_THROW( knlFindXaContext( aXid,
                                             sHashValue,
                                             &sXaContext,
                                             &sFound,
                                             KNL_ENV( aEnv ) ) == STL_SUCCESS,
                           RAMP_ERR_XA_RMERR );

            STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_XA_NOTA );
        
            STL_TRY_THROW( ssxLatchContext( sXaContext, aEnv ) == STL_SUCCESS,
                           RAMP_ERR_XA_RMERR );
            sState = 1;

            /**
             * latch 획득 이후에 다시 검사한다.
             */
        
            STL_TRY_THROW( knlIsEqualXid( aXid,
                                          sHashValue,
                                          sXaContext ) == STL_TRUE,
                           RAMP_ERR_XA_NOTA );
        
            sXaState = sXaContext->mState;

            if( sXaState == KNL_XA_STATE_ACTIVE )
            {
                if( aFlags & SSL_TM_NOWAIT )
                {
                    /**
                     * 다른 세션에서 사용중인 경우는 에러
                     */
                    
                    STL_THROW( RAMP_ERR_XA_RETRY );
                }
                else
                {
                    /**
                     * 같은 세션에서 Start를 2번 이상 호출한 경우는 무시한다.
                     */

                    if( sSessionId != sXaContext->mSessionId )
                    {
                        /**
                         * 다른 세션에서 사용중인 경우는 assocation을 suspend할때까지
                         * 기다린다.
                         */
                    
                        sState = 0;
                        STL_TRY_THROW( ssxUnlatchContext( sXaContext, aEnv ) == STL_SUCCESS,
                                       RAMP_ERR_XA_RMERR );
            
                        STL_TRY_THROW( ssxWaitContext( sXaContext, &sIsSuccess, aEnv ) == STL_SUCCESS,
                                       RAMP_ERR_XA_RMERR );

                        STL_TRY_THROW( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS,
                                       RAMP_ERR_XA_RMFAIL );
            
                        continue;
                    }
                }
            }
            
            /**
             * 이미 xa_start() 가 수행되어있는 경우
             */
            
            STL_TRY_THROW( sXaContext->mAssociateState != KNL_XA_ASSOCIATE_STATE_TRUE,
                           RAMP_ERR_XA_PROTO );
            
            STL_TRY_THROW( sXaState != KNL_XA_STATE_ROLLBACK_ONLY,
                           RAMP_ERR_XA_ROLLBACK_ONLY );

            /**
             * xa_start() with JOIN 은 xa_end()시에 SUCCESS를 수행했어야 한다.
             * xa_start() with RESUME 은 xa_end()시에 SUSPEND를 수행했어야 한다.
             */
            
            if( aFlags & SSL_TM_JOIN )
            {
                STL_TRY_THROW( sXaContext->mAssociateState != KNL_XA_ASSOCIATE_STATE_SUSPEND,
                               RAMP_ERR_XA_PROTO );
            }
            else
            {
                STL_TRY_THROW( sXaContext->mAssociateState == KNL_XA_ASSOCIATE_STATE_SUSPEND,
                               RAMP_ERR_XA_PROTO );
            }
            
            STL_TRY_THROW( sXaState == KNL_XA_STATE_IDLE,
                           RAMP_ERR_XA_PROTO );
        
            sXaContext->mState = KNL_XA_STATE_ACTIVE;
        
            if( sXaState == KNL_XA_STATE_IDLE )
            {
                STL_TRY_THROW( ssxAssociate( sSessionId,
                                             sXaContext,
                                             aEnv ) == STL_SUCCESS,
                               RAMP_ERR_XA_RMERR );
            }

            break;
        }
    }
    else
    {
        /**
         * 해당 세션에 이미 Context가 Associate되어 있다면 에러
         */
        
        STL_TRY_THROW( sSessionEnv->mXaContext == NULL, RAMP_ERR_XA_PROTO );
        
        /**
         * 새로운 Context를 생성한다.
         */
        
        STL_TRY_THROW( ssxCreateContext( sSessionId,
                                         aXid,
                                         &sXaContext,
                                         aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );

        /**
         * Context를 재사용하는 경우는 래치를 획득해야 한다.
         */
        
        STL_TRY_THROW( ssxLatchContext( sXaContext, aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
        sState = 1;

        sXaContext->mState = KNL_XA_STATE_ACTIVE;
        
        STL_TRY_THROW( knlLinkXaContext( sXaContext,
                                         &sIsSuccess,
                                         KNL_ENV( aEnv ) ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );

        if( sIsSuccess == STL_FALSE )
        {
            sState = 0;
            STL_TRY_THROW( ssxUnlatchContext( sXaContext, aEnv ) == STL_SUCCESS,
                           RAMP_ERR_XA_RMERR );
            
            STL_TRY_THROW( ssxDestroyContext( sXaContext, aEnv ) == STL_SUCCESS,
                           RAMP_ERR_XA_RMERR );
            
            STL_THROW( RAMP_ERR_XA_DUPID );
        }

        STL_TRY_THROW( smlBeginTrans( sSessionEnv->mCurrTxnIsolation,
                                      STL_TRUE,  /* aIsGlobalTrans */
                                      &sSessionEnv->mTransId,
                                      SML_ENV( aEnv ) ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );

        sXaContext->mTransId = sSessionEnv->mTransId;
    }

    sXaContext->mAssociateState = KNL_XA_ASSOCIATE_STATE_TRUE;
            
    sState = 0;
    STL_TRY_THROW( ssxUnlatchContext( sXaContext, aEnv ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );
            
    knlSetTransScope( sSessionEnv, KNL_TRANS_SCOPE_GLOBAL );
    sSessionEnv->mXaContext = sXaContext;

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }

    STL_TRY( ssxLogMsg( aXid,
                        "xa_start",
                        aFlags,
                        sReturn,
                        aEnv )
             == STL_SUCCESS );

    return sReturn;

    STL_CATCH( RAMP_ERR_XA_RMFAIL )
    {
        sReturn = SSL_XAER_RMFAIL;
    }
    
    STL_CATCH( RAMP_ERR_XA_RMERR )
    {
        sReturn = SSL_XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_XA_PROTO )
    {
        sReturn = SSL_XAER_PROTO;
    }

    STL_CATCH( RAMP_ERR_XA_INVAL )
    {
        sReturn = SSL_XAER_INVAL;
    }

    STL_CATCH( RAMP_ERR_XA_DUPID )
    {
        sReturn = SSL_XAER_DUPID;
    }

    STL_CATCH( RAMP_ERR_XA_NOTA )
    {
        sReturn = SSL_XAER_NOTA;
    }

    STL_CATCH( RAMP_ERR_XA_RETRY )
    {
        sReturn = SSL_XA_RETRY;
    }

    STL_CATCH( RAMP_ERR_XA_OUTSIDE )
    {
        sReturn = SSL_XAER_OUTSIDE;
    }

    STL_CATCH( RAMP_ERR_XA_ROLLBACK_ONLY )
    {
        sReturn = SSL_XA_RBROLLBACK;
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void) ssxUnlatchContext( sXaContext, aEnv );
    }

    (void) ssxLogMsg( aXid,
                      "xa_start",
                      aFlags,
                      sReturn,
                      aEnv );
    
    sslSetCallStack( aEnv );

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }
    
    return sReturn;
}

/**
 * @brief Global Transaction을 종료하거나 Suspend한다.
 * @param[in] aXid          Global Transaction Identifier
 * @param[in] aRmId         Resource Manager Identifier
 * @param[in] aFlags        Option Flags
 * @param[in] aEnv          Environment Pointer
 * @remarks
 */
stlInt32 sslXaEnd( stlXid    * aXid,
                   stlInt32    aRmId,
                   stlInt64    aFlags,
                   sslEnv    * aEnv )
{
    sslSessionEnv * sSessionEnv = NULL;
    stlInt32        sXaState = KNL_XA_STATE_IDLE; 
    stlInt32        sAssociateState = KNL_XA_ASSOCIATE_STATE_FALSE; 
    knlXaContext  * sXaContext;
    stlBool         sFound;
    stlInt32        sState = 0;
    stlInt32        sReturn = SSL_XA_OK;
    stlInt32        sHashValue;
    stlUInt32       sSessionId;
    

    sSessionEnv = SSL_SESS_ENV( aEnv );
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY_THROW( aFlags & (SSL_TM_SUCCESS | SSL_TM_FAIL | SSL_TM_SUSPEND),
                   RAMP_ERR_XA_INVAL );
    
    STL_TRY_THROW( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMFAIL );
    STL_TRY_THROW( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );
    
    STL_TRY_THROW( knlBreakPointWaitToStart( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    STL_TRY( knlGetSessionEnvId( sSessionEnv,
                                 &sSessionId,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY_THROW( smlHasActiveStatement( SML_ENV(aEnv) ) == STL_FALSE,
                   RAMP_ERR_XA_RMERR );
    
    sXaContext = sSessionEnv->mXaContext;
    sHashValue = knlGetXidHashValue( aXid );
        
    if( sXaContext == NULL )
    {
        STL_TRY_THROW( knlFindXaContext( aXid,
                                         sHashValue,
                                         &sXaContext,
                                         &sFound,
                                         KNL_ENV( aEnv ) ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );

        STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_XA_NOTA );
    }

    STL_TRY_THROW( ssxLatchContext( sXaContext,
                                    aEnv ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );
    sState = 1;

    /**
     * latch 획득 이후에 다시 검사한다.
     */
        
    STL_TRY_THROW( knlIsEqualXid( aXid,
                                  sHashValue,
                                  sXaContext ) == STL_TRUE,
                   RAMP_ERR_XA_NOTA );
        
    STL_TRY_THROW( sXaContext->mAssociateState != KNL_XA_ASSOCIATE_STATE_FALSE,
                   RAMP_ERR_XA_PROTO );

    /**
     * 자신의 session에서 xa_start()를 했어야 한다.
     */
    STL_TRY_THROW( sXaContext->mSessionId == sSessionId, RAMP_ERR_XA_PROTO );
        
    if( aFlags & SSL_TM_SUSPEND )
    {
        if( (aFlags & SSL_TM_SUCCESS) || (aFlags & SSL_TM_FAIL) )
        {
            STL_THROW( RAMP_ERR_XA_INVAL );
        }

        if( aFlags & SSL_TM_MIGRATE )
        {
            STL_THROW( RAMP_ERR_XA_MIGRATE );
        }
        else
        {
            sAssociateState = KNL_XA_ASSOCIATE_STATE_SUSPEND;
        }
    }
    else
    {
        if( aFlags & SSL_TM_FAIL )
        {
            sXaState = KNL_XA_STATE_ROLLBACK_ONLY;
        }
    }

    STL_TRY_THROW( ssxDissociate( sXaContext,
                                  SSL_SESS_ENV(aEnv),
                                  STL_FALSE,  /* aIsCleanUp */
                                  aEnv ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );
    
    sXaContext->mState = sXaState;
    sXaContext->mAssociateState = sAssociateState;
    
    STL_TRY_THROW( ssxWakeupContext( sXaContext,
                                     aEnv ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    sState = 0;
    STL_TRY_THROW( ssxUnlatchContext( sXaContext,
                                      aEnv ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    sSessionEnv->mXaContext = NULL;
    knlSetTransScope( sSessionEnv, KNL_TRANS_SCOPE_NONE );

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }

    STL_TRY( ssxLogMsg( aXid,
                        "xa_end",
                        aFlags,
                        sReturn,
                        aEnv )
             == STL_SUCCESS );

    return sReturn;

    STL_CATCH( RAMP_ERR_XA_RMFAIL )
    {
        sReturn = SSL_XAER_RMFAIL;
    }
    
    STL_CATCH( RAMP_ERR_XA_RMERR )
    {
        sReturn = SSL_XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_XA_INVAL )
    {
        sReturn = SSL_XAER_INVAL;
    }

    STL_CATCH( RAMP_ERR_XA_PROTO )
    {
        sReturn = SSL_XAER_PROTO;
    }

    STL_CATCH( RAMP_ERR_XA_NOTA )
    {
        sReturn = SSL_XAER_NOTA;
    }

    STL_CATCH( RAMP_ERR_XA_MIGRATE )
    {
        sReturn = SSL_XA_NOMIGRATE;
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void) ssxUnlatchContext( sXaContext, aEnv );
    }

    (void) ssxLogMsg( aXid,
                      "xa_end",
                      aFlags,
                      sReturn,
                      aEnv );
    
    sslSetCallStack( aEnv );

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }
    
    return sReturn;
}

/**
 * @brief Global Transaction을 Commit이나 Rollback할 준비를 한다.
 * @param[in] aXid          Global Transaction Identifier
 * @param[in] aRmId         Resource Manager Identifier
 * @param[in] aFlags        Option Flags
 * @param[in] aEnv          Environment Pointer
 * @remarks
 */
stlInt32 sslXaPrepare( stlXid    * aXid,
                       stlInt32    aRmId,
                       stlInt64    aFlags,
                       sslEnv    * aEnv )
{
    sslSessionEnv * sSessionEnv;
    stlInt32        sXaState = KNL_XA_STATE_IDLE; 
    knlXaContext  * sXaContext;
    stlBool         sFound;
    stlInt32        sState = 0;
    smlTransId      sTransId;
    stlInt32        sReturn = SSL_XA_OK;
    stlInt32        sRBReturn = SSL_XA_RBROLLBACK;
    stlInt32        sHashValue;
    stlErrorData  * sErrorData = NULL;
    
    sSessionEnv = SSL_SESS_ENV( aEnv );
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY_THROW( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMFAIL );
    STL_TRY_THROW( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );
    
    STL_TRY_THROW( knlBreakPointWaitToStart( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    sHashValue = knlGetXidHashValue( aXid );
    
    STL_TRY_THROW( knlFindXaContext( aXid,
                                     sHashValue,
                                     &sXaContext,
                                     &sFound,
                                     KNL_ENV( aEnv ) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_XA_NOTA );

    STL_TRY_THROW( ssxLatchContext( sXaContext,
                                    aEnv ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );
    sState = 1;

    /**
     * latch 획득 이후에 다시 검사한다.
     */
        
    STL_TRY_THROW( knlIsEqualXid( aXid,
                                  sHashValue,
                                  sXaContext ) == STL_TRUE,
                   RAMP_ERR_XA_NOTA );
        
    sXaState = sXaContext->mState;
    sTransId = sXaContext->mTransId;

    /**
     * xa_end() with SUCCESS 만이 Prepare가 가능하다.
     */
    
    STL_TRY_THROW( (sXaState == KNL_XA_STATE_IDLE) ||
                   (sXaState == KNL_XA_STATE_HEURISTIC_COMPLETED),
                   RAMP_ERR_XA_PROTO );

    STL_TRY_THROW( sXaContext->mAssociateState == KNL_XA_ASSOCIATE_STATE_FALSE,
                   RAMP_ERR_XA_PROTO );

    if( sXaState == KNL_XA_STATE_HEURISTIC_COMPLETED )
    {
        STL_TRY_THROW( knlUnlinkXaContext( sXaContext,
                                           KNL_ENV( aEnv ) ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
        sState = 0;
        STL_TRY_THROW( ssxUnlatchContext( sXaContext,
                                          aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
        STL_TRY_THROW( ssxDestroyContext( sXaContext,
                                          aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
        
        STL_THROW( RAMP_ERR_XA_PROTO );
    }
    
    /**
     * READ-ONLY Transaction은 Prepare가 불가능하다.
     */
    
    if( smlIsReadOnlyTrans( sTransId ) == STL_TRUE )
    {
        STL_TRY_THROW( knlUnlinkXaContext( sXaContext,
                                           KNL_ENV( aEnv ) ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
        sState = 0;
        STL_TRY_THROW( ssxUnlatchContext( sXaContext,
                                          aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
        STL_TRY_THROW( ssxDestroyContext( sXaContext,
                                          aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
        
        STL_TRY_THROW( ssxCommit( sTransId,
                                  sXaContext,
                                  aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
        
        sSessionEnv->mXaContext = NULL;
        
        STL_THROW( RAMP_ERR_XA_READONLY );
    }

    if( qllPrepare( aXid,
                    sXaContext->mTransId,
                    SML_TRANSACTION_CWM_NONE,
                    sXaContext,
                    QLL_ENV(aEnv) ) == STL_FAILURE )
    {
        sErrorData = stlGetLastErrorData( KNL_ERROR_STACK( aEnv ) );

        STL_DASSERT( sErrorData != NULL );

        if( sErrorData != NULL )
        {
            switch( stlGetExternalErrorCode( sErrorData ) )
            {
                case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_INTEGRITY_CONSTRAINT_VIOLATION :
                    sRBReturn = SSL_XA_RBINTEGRITY;
                    break;
                case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_NO_SUBCLASS :
                case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_STATEMENT_COMPLETION_UNKNOWN :
                case STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_TRIGGERED_ACTION_EXCEPTION :
                    sRBReturn = SSL_XA_RBROLLBACK;
                    break;
                default :
                    break;
            }
        }
        
        STL_TRY_THROW( knlUnlinkXaContext( sXaContext,
                                           KNL_ENV( aEnv ) ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
        sState = 0;
        STL_TRY_THROW( ssxUnlatchContext( sXaContext,
                                          aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
        STL_TRY_THROW( ssxDestroyContext( sXaContext,
                                          aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );

        STL_TRY_THROW( ssxRollback( sTransId,
                                    sXaContext,
                                    aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
        
        sSessionEnv->mXaContext = NULL;
        
        STL_THROW( RAMP_ERR_XA_ROLLBACK );
    }
    else
    {
        sXaContext->mState = KNL_XA_STATE_PREPARED;
    }
    
    sState = 0;
    STL_TRY_THROW( ssxUnlatchContext( sXaContext,
                                      aEnv ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }

    STL_TRY( ssxLogMsg( aXid,
                        "xa_prepare",
                        aFlags,
                        sReturn,
                        aEnv )
             == STL_SUCCESS );
    
    return sReturn;

    STL_CATCH( RAMP_ERR_XA_RMFAIL )
    {
        sReturn = SSL_XAER_RMFAIL;
    }
    
    STL_CATCH( RAMP_ERR_XA_RMERR )
    {
        sReturn = SSL_XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_XA_ROLLBACK )
    {
        sReturn = sRBReturn;
    }

    STL_CATCH( RAMP_ERR_XA_READONLY )
    {
        sReturn = SSL_XA_RDONLY;
    }

    STL_CATCH( RAMP_ERR_XA_PROTO )
    {
        sReturn = SSL_XAER_PROTO;
    }

    STL_CATCH( RAMP_ERR_XA_NOTA )
    {
        sReturn = SSL_XAER_NOTA;
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void) ssxUnlatchContext( sXaContext, aEnv );
    }

    (void) ssxLogMsg( aXid,
                      "xa_prepare",
                      aFlags,
                      sReturn,
                      aEnv );
    
    sslSetCallStack( aEnv );

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }
    
    return sReturn;
}

/**
 * @brief Heuristic Completed Global Transaction의 정보를 삭제한다.
 * @param[in] aXid          Global Transaction Identifier
 * @param[in] aRmId         Resource Manager Identifier
 * @param[in] aFlags        Option Flags
 * @param[in] aEnv          Environment Pointer
 * @remarks
 */
stlInt32 sslXaForget( stlXid    * aXid,
                      stlInt32    aRmId,
                      stlInt64    aFlags,
                      sslEnv    * aEnv )
{
    sslSessionEnv * sSessionEnv;
    knlXaContext  * sXaContext;
    stlBool         sFound;
    stlBool         sIsDeleted;
    stlInt32        sReturn = SSL_XA_OK;
    stlInt32        sHashValue;
    stlInt32        sState = 0;

    
    sSessionEnv = SSL_SESS_ENV( aEnv );
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY_THROW( aFlags == SSL_TM_NOFLAGS, RAMP_ERR_XA_INVAL );
    
    STL_TRY_THROW( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMFAIL );
    STL_TRY_THROW( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );
    
    STL_TRY_THROW( knlBreakPointWaitToStart( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    sHashValue = knlGetXidHashValue( aXid );
    
    STL_TRY_THROW( knlFindXaContext( aXid,
                                     sHashValue,
                                     &sXaContext,
                                     &sFound,
                                     KNL_ENV( aEnv ) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    if( sFound == STL_TRUE )
    {
        STL_TRY_THROW( ssxLatchContext( sXaContext,
                                        aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
        sState = 1;
        
        /**
         * latch 획득 이후에 다시 검사한다.
         */
        
        STL_TRY_THROW( knlIsEqualXid( aXid,
                                      sHashValue,
                                      sXaContext ) == STL_TRUE,
                       RAMP_ERR_XA_RMERR );
        
        STL_TRY_THROW( sXaContext->mState == KNL_XA_STATE_HEURISTIC_COMPLETED,
                       RAMP_ERR_XA_PROTO );

        STL_TRY_THROW( knlUnlinkXaContext( sXaContext,
                                           KNL_ENV( aEnv ) ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
        
        sState = 0;
        STL_TRY_THROW( ssxUnlatchContext( sXaContext,
                                          aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );

        STL_TRY_THROW( ssxDestroyContext( sXaContext,
                                          aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
    }

    STL_TRY_THROW( ssxDeleteRecover( aXid,
                                     &sIsDeleted,
                                     aEnv ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    STL_TRY_THROW( sIsDeleted == STL_TRUE, RAMP_ERR_XA_NOTA );
    
    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }

    STL_TRY( ssxLogMsg( aXid,
                        "xa_forget",
                        aFlags,
                        sReturn,
                        aEnv )
             == STL_SUCCESS );
                        
    return sReturn;

    STL_CATCH( RAMP_ERR_XA_RMFAIL )
    {
        sReturn = SSL_XAER_RMFAIL;
    }
    
    STL_CATCH( RAMP_ERR_XA_RMERR )
    {
        sReturn = SSL_XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_XA_INVAL )
    {
        sReturn = SSL_XAER_INVAL;
    }
    
    STL_CATCH( RAMP_ERR_XA_PROTO )
    {
        sReturn = SSL_XAER_PROTO;
    }

    STL_CATCH( RAMP_ERR_XA_NOTA )
    {
        sReturn = SSL_XAER_NOTA;
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void) ssxUnlatchContext( sXaContext, aEnv );
    }

    (void) ssxLogMsg( aXid,
                      "xa_forget",
                      aFlags,
                      sReturn,
                      aEnv );
    
    sslSetCallStack( aEnv );

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }
    
    return sReturn;
}

/**
 * @brief Prepared 또는 Heuristic Completed Global Transaction의 정보를 얻는다.
 * @param[out] aXids         대상 Global Transaction Identifier들
 * @param[in]  aCount        Maximum number of XIDs
 * @param[in]  aRmId         Resource Manager Identifier
 * @param[in]  aFlags        Option Flags
 * @param[in]  aEnv          Environment Pointer
 * @remarks
 */
stlInt32 sslXaRecover( stlXid    * aXids,
                       stlInt64    aCount,
                       stlInt32    aRmId,
                       stlInt64    aFlags,
                       sslEnv    * aEnv )
{
    sslSessionEnv * sSessionEnv;
    stlBool         sMustEnd = STL_FALSE;
    stlInt64        sFetchCount = 0;
    stlInt32        sReturn = SSL_XA_OK;
    

    STL_PARAM_VALIDATE( aXids != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCount > 0, KNL_ERROR_STACK(aEnv) );

    sSessionEnv = SSL_SESS_ENV( aEnv );
    STL_DASSERT( knlIsUsedEnv( (void*)aEnv ) == STL_TRUE );

    STL_TRY_THROW( knlCheckEndSession( sSessionEnv, KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMFAIL );
    STL_TRY_THROW( knlExecuteProcessEvent( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );
    
    STL_TRY_THROW( knlBreakPointWaitToStart( KNL_ENV(aEnv) ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    if( aFlags & SSL_TM_ENDRSCAN )
    {
        sMustEnd = STL_TRUE;
    }
    
    if( aFlags & SSL_TM_STARTRSCAN )
    {
        STL_TRY_THROW( ssxResetRecover( aEnv ) == STL_SUCCESS, RAMP_ERR_XA_RMERR );
        STL_TRY_THROW( ssxOpenRecover( aEnv ) == STL_SUCCESS, RAMP_ERR_XA_RMERR );
    }
    else
    {
        /**
         * Start flag가 없으면 Iterator가 open되어 있어야 한다.
         */
        
        STL_TRY_THROW( ssxIsOpenRecover( aEnv ) == STL_TRUE,
                       RAMP_ERR_XA_PROTO );
    }
    
    STL_TRY_THROW( ssxFetchRecover( aXids,
                                    aCount,
                                    &sFetchCount,
                                    aEnv ) == STL_SUCCESS,
                   RAMP_ERR_XA_RMERR );

    if( (sFetchCount != aCount) || (sMustEnd == STL_TRUE) )
    {
        STL_TRY_THROW( ssxCloseRecover( aEnv ) == STL_SUCCESS,
                       RAMP_ERR_XA_RMERR );
    }

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }

    STL_TRY( ssxLogMsg( NULL,
                        "xa_recover",
                        aFlags,
                        sReturn,
                        aEnv )
             == STL_SUCCESS );
    
    return sFetchCount;

    STL_CATCH( RAMP_ERR_XA_RMFAIL )
    {
        sReturn = SSL_XAER_RMFAIL;
    }
    
    STL_CATCH( RAMP_ERR_XA_RMERR )
    {
        sReturn = SSL_XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_XA_PROTO )
    {
        sReturn = SSL_XAER_PROTO;
    }

    STL_FINISH;

    if( aEnv == NULL )
    {
        return sReturn;
    }
        
    (void) ssxLogMsg( NULL,
                      "xa_recover",
                      aFlags,
                      sReturn,
                      aEnv );
    
    sslSetCallStack( aEnv );

    if( knlGetEndSessionOption( (knlSessionEnv*)sSessionEnv ) ==
        KNL_END_SESSION_OPTION_POST_TRANSACTION )
    {
        knlKillSession( sSessionEnv );
    }
    
    return sReturn;
}

/**
 * @brief XA Context를 Dissociate한다.
 * @param[in]  aContext          Target Context
 * @param[in]  aDeadSessionEnv   Dead Session Environment Pointer
 * @param[in]  aEnv              Environment Pointer
 * @remarks
 */
stlStatus sslCleanupContext( knlXaContext  * aContext,
                             sslSessionEnv * aDeadSessionEnv,
                             sslEnv        * aEnv )
{
    stlInt32 sState = 0;
    
    STL_TRY( ssxLatchContext( aContext,
                              aEnv ) == STL_SUCCESS );
    sState = 1;
            
    aContext->mState = KNL_XA_STATE_ROLLBACK_ONLY;
            
    (void) ssxDissociate( aContext,
                          aDeadSessionEnv,
                          STL_TRUE,  /* aIsCleanUp */
                          aEnv );
            
    aContext->mAssociateState = KNL_XA_ASSOCIATE_STATE_FALSE;
    
    sState = 0;
    STL_TRY( ssxUnlatchContext( aContext,
                                aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) ssxUnlatchContext( aContext, aEnv );
    }

    return STL_FAILURE;
}
