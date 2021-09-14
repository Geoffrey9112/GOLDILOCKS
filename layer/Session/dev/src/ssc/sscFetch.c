/*******************************************************************************
 * sscFetch.c
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
 * @file sscFetch.c
 * @brief Session Fetch Command Internal Routines
 */

#include <qll.h>
#include <sslDef.h>
#include <ssc.h>

stlStatus sscFetch( sslStatement       * aStatement,
                    stlInt64             aOffset,
                    knlValueBlockList ** aTargetBuffer,
                    stlBool            * aIsEndOfScan,
                    stlBool            * aIsBlockRead,
                    stlInt32           * aOneRowIdx,
                    qllCursorRowStatus * aOneRowStatus,
                    stlInt64           * aAbsIdxOfFirstRow,
                    stlInt64           * aKnownLastRowIdx,
                    sslEnv             * aEnv )
{
    sslSessionEnv          * sSessionEnv;
    qllCursorRowStatus       sCursorRowStatus = QLL_CURSOR_ROW_STATUS_NO_CHANGE;
    stlInt32                 sState;
    stlBool                  sEndOfScan = STL_FALSE;
    ellCursorUpdatability    sUpdatability;
    ellCursorScrollability   sScrollability;
    ellCursorSensitivity     sSensitivity;


    STL_PARAM_VALIDATE( aStatement->mCursorStmt != NULL, KNL_ERROR_STACK( aEnv ) );
    
    sSessionEnv = SSL_SESS_ENV( aEnv );

    *aTargetBuffer = NULL;
    
    /*
     * Fetch
     */

    if( aStatement->mResultSetCursor == NULL )
    {
        aStatement->mResultSetCursor = qllGetCursorResultSetDesc( &aStatement->mSqlStmt );
    }
    else
    {
        /* nothing to do */
    }

    sUpdatability  = qllGetCursorUpdatability( &aStatement->mSqlStmt );
    sScrollability = qllGetCursorScrollability( &aStatement->mSqlStmt );
    sSensitivity   = qllGetCursorSensitivity( &aStatement->mSqlStmt );

    if( (aStatement->mIsEndOfScan == STL_TRUE) &&
        (sUpdatability == ELL_CURSOR_UPDATABILITY_FOR_READ_ONLY ) &&
        (sScrollability == ELL_CURSOR_SCROLLABILITY_NO_SCROLL) )
    {
        STL_TRY( sscCloseCursor( aStatement, aEnv ) == STL_SUCCESS );

        STL_THROW( RAMP_FINISH );
    }

    sState = 1;

    aStatement->mIsEndOfScan = STL_FALSE;
    
    if( sSensitivity == ELL_CURSOR_SENSITIVITY_SENSITIVE )
    {
        /**
         * SENSITIVE Cursor 인 경우
         */
        
        *aIsBlockRead = STL_FALSE;

        if( qllFetchCursorOneRow( sSessionEnv->mTransId,
                                  aStatement->mCursorStmt,
                                  &aStatement->mDbcStmt,
                                  aStatement->mResultSetCursor,
                                  QLL_FETCH_ABSOLUTE,
                                  aOffset,
                                  & sCursorRowStatus,
                                  aOneRowIdx,
                                  QLL_ENV(aEnv) )
            == STL_SUCCESS )
        {
            /* Do Nothing */
        }
        else
        {
            if( QLL_OPT_TRACE_FAILURE_SQL_PLAN_ON( aEnv ) == STL_TRUE )
            {
                STL_TRY( qllTraceExplainErrorSQL( sSessionEnv->mTransId,
                                                  &aStatement->mDbcStmt,
                                                  &aStatement->mSqlStmt,
                                                  QLL_ENV( aEnv ) )
                         == STL_SUCCESS );
            }

            STL_THROW( STL_FINISH_LABEL );
        }

        if ( *aOneRowIdx < 0 )
        {
            if( (aStatement->mResultSetCursor->mPositionType == QLL_CURSOR_POSITION_BEFORE_THE_FIRST_ROW) ||
                (aStatement->mResultSetCursor->mPositionType == QLL_CURSOR_POSITION_AFTER_THE_LAST_ROW) )
            {
                sEndOfScan = STL_TRUE;

                if( sScrollability == ELL_CURSOR_SCROLLABILITY_NO_SCROLL )
                {
                    aStatement->mIsEndOfScan = STL_TRUE;
                }
            }
        }

        /* block read가 아닐 경우 mPositionRowIdx로 row의 위치 정보를 얻을 수 있다. */
        *aAbsIdxOfFirstRow = aStatement->mResultSetCursor->mPositionRowIdx;
    }
    else
    {
        /**
         * INSENSITIVE Cursor 인 경우
         */
        
        *aIsBlockRead = STL_TRUE;
        *aOneRowIdx   = -1;

        if ( sScrollability == ELL_CURSOR_SCROLLABILITY_NO_SCROLL )
        {
            /**
             * NO SCROLL Cursor 인 경우
             */
            
            if( qllFetchCursorForwardBlock( sSessionEnv->mTransId,
                                            aStatement->mCursorStmt,
                                            &aStatement->mDbcStmt,
                                            aStatement->mResultSetCursor,
                                            &sEndOfScan,
                                            QLL_ENV( aEnv ) ) == STL_SUCCESS )
            {
                /* Do Nothing */
            }
            else
            {
                if( QLL_OPT_TRACE_FAILURE_SQL_PLAN_ON( aEnv ) == STL_TRUE )
                {
                    STL_TRY( qllTraceExplainErrorSQL( sSessionEnv->mTransId,
                                                      &aStatement->mDbcStmt,
                                                      &aStatement->mSqlStmt,
                                                      QLL_ENV( aEnv ) )
                             == STL_SUCCESS );
                }

                STL_THROW( STL_FINISH_LABEL );
            }

            aStatement->mIsEndOfScan = sEndOfScan;
        }
        else
        {
            /**
             * SCROLL Cursor 인 경우
             */

            if( qllFetchCursorScrollBlock( sSessionEnv->mTransId,
                                           aStatement->mCursorStmt,
                                           &aStatement->mDbcStmt,
                                           aStatement->mResultSetCursor,
                                           aOffset,
                                           &sEndOfScan,
                                           QLL_ENV( aEnv ) ) == STL_SUCCESS )
            {
                /* Do Nothing */
            }
            else
            {
                if( QLL_OPT_TRACE_FAILURE_SQL_PLAN_ON( aEnv ) == STL_TRUE )
                {
                    STL_TRY( qllTraceExplainErrorSQL( sSessionEnv->mTransId,
                                                      &aStatement->mDbcStmt,
                                                      &aStatement->mSqlStmt,
                                                      QLL_ENV( aEnv ) )
                             == STL_SUCCESS );
                }

                STL_THROW( STL_FINISH_LABEL );
            }
        }

        /* block read일 경우 mBlockStartPositionIdx로 위치 정보를 얻을 수 있다. */
        *aAbsIdxOfFirstRow = aStatement->mResultSetCursor->mBlockStartPositionIdx;
    }

    if( aStatement->mResultSetCursor->mKnownLastPosition == STL_TRUE )
    {
        *aKnownLastRowIdx = aStatement->mResultSetCursor->mLastPositionIdx;

        if( sScrollability == ELL_CURSOR_SCROLLABILITY_NO_SCROLL )
        {
            if( ( aStatement->mExplainType == SSL_EXPLAIN_PLAN_ON ) ||
                ( aStatement->mExplainType == SSL_EXPLAIN_PLAN_ON_VERBOSE ) )
            {
                STL_TRY( qllExplainSQL( sSessionEnv->mTransId,
                                        &aStatement->mDbcStmt,
                                        &aStatement->mSqlStmt,
                                        ( aStatement->mExplainType == SSL_EXPLAIN_PLAN_ON
                                          ? STL_FALSE : STL_TRUE ), /* Is Verbose */
                                        QLL_ENV( aEnv ) ) == STL_SUCCESS );
            }

            if( QLL_OPT_TRACE_SUCCESS_SQL_PLAN_ON( aEnv ) == STL_TRUE )
            {
                STL_TRY( qllTraceExplainSQL( sSessionEnv->mTransId,
                                             &aStatement->mDbcStmt,
                                             &aStatement->mSqlStmt,
                                             QLL_ENV( aEnv ) ) == STL_SUCCESS );
            }
        }
    }
    else
    {
        *aKnownLastRowIdx = SSL_LAST_ROW_IDX_UNKNOWN;
    }
 
    *aTargetBuffer = aStatement->mResultSetCursor->mTargetBlock;

    STL_RAMP( RAMP_FINISH );
    
    if( aIsEndOfScan != NULL )
    {
        *aIsEndOfScan = sEndOfScan;
    }

    if( aOneRowStatus != NULL )
    {
        *aOneRowStatus = sCursorRowStatus;
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1 :
            (void)sscCloseCursor( aStatement, aEnv );
            break;
    }

    return STL_FAILURE;
}
