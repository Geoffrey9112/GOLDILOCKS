/*******************************************************************************
 * sscCursor.c
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
 * @file sscCursor.c
 * @brief Session Layer Cursor Command Internal Routines
 */


#include <qll.h>
#include <sslDef.h>

stlStatus sscSetCursorName( sslStatement * aStatement,
                            stlChar      * aCursorName,
                            sslEnv       * aEnv )
{
    STL_PARAM_VALIDATE( aCursorName != NULL, KNL_ERROR_STACK( aEnv ) );

    /*
     * 커서 이름을 대문자로 변경한다.
     */
    stlToupperString( aCursorName, aCursorName );
    
    if( aStatement->mCursorDesc == NULL )
    {
        STL_TRY( qllDeclareNamedCursor( aCursorName,
                                        &aStatement->mCursorDesc,
                                        QLL_ENV( aEnv ) ) == STL_SUCCESS );
    }
    else
    {
        if( stlStrcmp( aCursorName, aStatement->mCursorName ) != 0 )
        {
            STL_TRY( qllRenameCursor( aStatement->mCursorName,
                                      aCursorName,
                                      &aStatement->mCursorDesc,
                                      QLL_ENV( aEnv ) ) == STL_SUCCESS );
        }
    }

    stlStrncpy( aStatement->mCursorName, aCursorName, STL_MAX_SQL_IDENTIFIER_LENGTH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus sscGetCursorName(sslStatement * aStatement,
                           stlChar      * aCursorName,
                           sslEnv       * aEnv )
{
    STL_PARAM_VALIDATE( aCursorName != NULL, KNL_ERROR_STACK( aEnv ) );

    if( aStatement->mCursorDesc == NULL )
    {
        stlSnprintf( aStatement->mCursorName,
                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                     "SQL_CUR%ld",
                     aStatement->mId );

        STL_TRY( qllDeclareNamedCursor( aStatement->mCursorName,
                                        &aStatement->mCursorDesc,
                                        QLL_ENV( aEnv ) ) == STL_SUCCESS );
    }

    /*
     * result set이 이미 존재할 경우 result set과 cursor name 연결
     */
    if( aStatement->mCursorStmt != NULL )
    {
        if( ellCursorIsOpen( aStatement->mCursorDesc ) == STL_FALSE )
        {
            STL_TRY( qllSetNamedCursorOpen( aStatement->mCursorStmt,
                                            & aStatement->mDbcStmt,
                                            & aStatement->mSqlStmt,
                                            aStatement->mCursorDesc,
                                            QLL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }

    stlStrncpy( aCursorName, aStatement->mCursorName, STL_MAX_SQL_IDENTIFIER_LENGTH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

stlStatus sscCloseCursor( sslStatement * aStatement,
                          sslEnv       * aEnv )
{
    aStatement->mResultSetCursor = NULL;

    if( aStatement->mCursorStmt != NULL )
    {
        STL_TRY( qllCloseCursor( &aStatement->mDbcStmt,
                                 &aStatement->mSqlStmt,
                                 QLL_ENV( aEnv ) ) == STL_SUCCESS );
        
        STL_TRY( smlFreeStatement( aStatement->mCursorStmt,
                                   SML_ENV( aEnv ) ) == STL_SUCCESS );
        aStatement->mViewScn = SML_INFINITE_SCN;
        aStatement->mCursorStmt = NULL;
    }

    aStatement->mRowsetFirstRow   = SSL_CURSOR_POSITION_BEFORE_FIRST;
    aStatement->mRowsetCurrentPos = 0;
    aStatement->mRowsetLastPos    = 0;
    aStatement->mIsEndOfScan      = STL_FALSE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus sscCloseWithoutHoldCursors( sslEnv * aEnv )
{
    /*
     * 세션의 모든 WITHOUT HOLD 커서를 닫는다.
     */
    sslSessionEnv          * sSessionEnv;
    sslStatement           * sStatement = NULL;
    qllResultSetDesc       * sCursor;
    stlInt64                 sActiveStatement = 0;
    stlInt64                 i;

    sSessionEnv = SSL_SESS_ENV( aEnv );

    STL_TRY_THROW( sSessionEnv->mHasWithoutHoldCursor == STL_TRUE,
                   RAMP_SUCCESS );
    
    if( sSessionEnv->mActiveStmtCount > 0 )
    {
        for( i = 0; i < sSessionEnv->mMaxStmtCount; i++ )
        {
            sStatement = sSessionEnv->mStatementSlot[i];
        
            if( sStatement != NULL )
            {
                sActiveStatement++;

                if( sStatement->mCursorStmt != NULL )
                {
                    sCursor = sStatement->mResultSetCursor;
        
                    if( sCursor == NULL )
                    {
                        sCursor = qllGetCursorResultSetDesc( &sStatement->mSqlStmt );
                    }

                    if( sCursor->mCursorProperty.mHoldability == ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD )
                    {
                        STL_TRY( sscCloseCursor( sStatement,
                                                 aEnv ) == STL_SUCCESS );
                    }
                }

                if( sSessionEnv->mActiveStmtCount <= sActiveStatement )
                {
                    break;
                }
            }
        }
    }

    sSessionEnv->mHasWithoutHoldCursor = STL_FALSE;

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus sscSetPos(sslStatement  * aStatement,
                    stlInt64        aRowNumber,
                    stlInt16        aOperation,
                    sslEnv        * aEnv )
{
    sslSessionEnv      * sSessionEnv;
    qllCursorRowStatus   sCursorRowStatus = QLL_CURSOR_ROW_STATUS_NO_CHANGE;
    stlInt32             sOneRowIdx;

    STL_PARAM_VALIDATE( aStatement->mCursorStmt != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStatement->mResultSetCursor != NULL, KNL_ERROR_STACK( aEnv ) );

    sSessionEnv = SSL_SESS_ENV( aEnv );

    STL_TRY( qllFetchCursorOneRow( sSessionEnv->mTransId,
                                   aStatement->mCursorStmt,
                                   &aStatement->mDbcStmt,
                                   aStatement->mResultSetCursor,
                                   QLL_FETCH_ABSOLUTE,
                                   aRowNumber,
                                   &sCursorRowStatus,
                                   &sOneRowIdx,
                                   QLL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
