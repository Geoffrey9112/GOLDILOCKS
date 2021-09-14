/*******************************************************************************
 * ztcnSyncher.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztcnSyncher.c
 * @brief GlieseTool Cyclone Syncher Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern stlBool        gRunState;
extern ztcMasterMgr * gMasterMgr;

stlStatus ztcnDoSyncher( ztcSyncher    * aSyncher,
                         stlErrorStack * aErrorStack )
{
    stlInt32             sState = 0; 
    SQLHSTMT             sStmtHandle;
    SQLRETURN            sRet;
    ztcSyncBufferInfo  * sBufferInfo  = aSyncher->mBufferInfoPtr;
    stlBool              sIsTimedOut  = STL_FALSE;
    stlBool              sDoSetSize   = STL_TRUE;
    stlBool              sDoCommit    = STL_FALSE;
    stlInt64             sArraySize   = aSyncher->mArraySize;
    stlInt64             sIdx         = 0;
    
    
    STL_TRY( ztcdSQLAllocHandle( aSyncher->mDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;

    while( STL_TRUE )
    {
        do
        {
            STL_TRY( gRunState == STL_TRUE );

            STL_TRY( stlTimedAcquireSemaphore( &sBufferInfo->mSyncSem,
                                               STL_SET_SEC_TIME( 1 ),
                                               &sIsTimedOut,
                                               aErrorStack ) == STL_SUCCESS );
            
            if( sIsTimedOut == STL_FALSE )
            {
                break;
            }
        } while( STL_TRUE );
        
        switch( sBufferInfo->mNextJob )
        {
            case ZTC_SYNC_NEXT_JOB_BINDPARAM:
            {
                if( sDoCommit == STL_TRUE )
                {
                    STL_TRY( ztcdSQLEndTran( SQL_HANDLE_DBC,
                                             aSyncher->mDbcHandle,
                                             SQL_COMMIT,
                                             STL_FALSE,
                                             aErrorStack ) == STL_SUCCESS );
                }
                sDoCommit = STL_TRUE;

                STL_TRY( ztcdSyncherSQLPrepare( sStmtHandle,
                                                sBufferInfo->mTableInfoPtr,
                                                aErrorStack ) == STL_SUCCESS );

                STL_TRY( ztcdSyncherSQLBindParameter( sStmtHandle,
                                                      aSyncher,
                                                      aErrorStack ) == STL_SUCCESS );
                
                sDoSetSize = STL_TRUE;
            }
            case ZTC_SYNC_NEXT_JOB_KEEP_EXEC:
            {
                if( ( sBufferInfo->mFetchedCount != sArraySize ) ||
                    ( sDoSetSize == STL_TRUE ) )
                {
                    STL_TRY( SQLSetStmtAttr( sStmtHandle,
                                             SQL_ATTR_PARAMSET_SIZE,
                                             (SQLPOINTER) sBufferInfo->mFetchedCount,
                                             0 ) == SQL_SUCCESS );
                    sDoSetSize = STL_FALSE;
                }
                
                sRet = SQLExecute( sStmtHandle );
                
                switch( sRet )
                {
                    case SQL_SUCCESS:
                        break;
                    case SQL_SUCCESS_WITH_INFO:
                    case SQL_ERROR:
                    {
                        for( sIdx = 0;sIdx < sBufferInfo->mFetchedCount;sIdx++ )
                        {
                            switch( aSyncher->mArrRecStatus[sIdx] )
                            {
                                case SQL_PARAM_SUCCESS:
                                case SQL_PARAM_UNUSED :
                                case SQL_PARAM_DIAG_UNAVAILABLE :
                                    break;
                                case SQL_PARAM_SUCCESS_WITH_INFO :
                                case SQL_PARAM_ERROR :
                                    sBufferInfo->mErrCount++;
                                    break;
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
                
                sBufferInfo->mIsFetched = STL_FALSE;
                break;
            }
            case ZTC_SYNC_NEXT_JOB_FINISH:
            {
                STL_THROW( RAMP_FINISH );
                break;
            }
            default:
            {
                STL_DASSERT( 0 );
                break;
            }
        }
    }

    STL_RAMP( RAMP_FINISH );

    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdSQLEndTran( SQL_HANDLE_DBC,
                             aSyncher->mDbcHandle,
                             SQL_COMMIT,
                             STL_FALSE,
                             aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) ztcdSQLFreeHandle( sStmtHandle,
                                      aErrorStack );
        default:
            (void) ztcdSQLEndTran( SQL_HANDLE_DBC,
                                   aSyncher->mDbcHandle,
                                   SQL_ROLLBACK,
                                   STL_FALSE,
                                   aErrorStack );
            break;
    }
    
    return STL_FAILURE;
}

/** @} */
