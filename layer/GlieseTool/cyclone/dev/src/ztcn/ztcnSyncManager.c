/*******************************************************************************
 * ztcnSyncManager.c
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
 * @file ztcnSyncManager.c
 * @brief GlieseTool Cyclone Sync Manager Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern stlBool        gRunState;
extern ztcMasterMgr * gMasterMgr;

/**
 * @brief Synchronization Manager를 수행한다.
 * @param[in]     aSyncMgr     Sync Manager
 * @param[in,out] aErrorStack  error stack 
 */  
stlStatus ztcnDoSyncMgr( ztcSyncMgr    * aSyncMgr,
                         stlErrorStack * aErrorStack )
{
    SQLHSTMT              sSelectStmtHandle;
    SQLHSTMT              sDeleteStmtHandle;
    SQLRETURN             sRet;
    stlInt32              sState       = 0;
    stlInt32              sNextJob;
    stlInt64              sArraySize   = aSyncMgr->mArraySize;
    stlInt64              sTotalCnt    = 0;
    stlInt64              sRecordCnt   = 0;
    stlInt64              sTrcMsgCnt   = ZTC_SYNC_LOG_MSG_FREQUENCY;
    stlInt64              sErrCount    = 0;
    
    ztcSyncBufferInfo   * sBufferInfo  = NULL;
    ztcSyncTableInfo    * sTableInfo   = NULL;
    ztcSyncColumnValue  * sColumnValue = NULL;

    STL_TRY( ztcdSQLAllocHandle( aSyncMgr->mDbcHandle,
                                 &sSelectStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( ztcdSQLAllocHandle( aSyncMgr->mSlaveDbcHandle,
                                 &sDeleteStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 2;

    /**
     * Array Binding 속성 설정.
     */ 
    STL_TRY( SQLSetStmtAttr( sSelectStmtHandle,
                             SQL_ATTR_ROW_BIND_TYPE,
                             (SQLPOINTER) SQL_BIND_BY_COLUMN,
                             0 ) == SQL_SUCCESS );

    STL_TRY( SQLSetStmtAttr( sSelectStmtHandle,
                             SQL_ATTR_ROW_ARRAY_SIZE,
                             (SQLPOINTER)sArraySize,
                             0 ) == SQL_SUCCESS );
        
    STL_TRY( SQLSetStmtAttr( sSelectStmtHandle,
                             SQL_ATTR_ROW_STATUS_PTR,
                             (SQLPOINTER)aSyncMgr->mArrRowStatus,
                             0 ) == SQL_SUCCESS );

    STL_TRY( SQLSetStmtAttr( sSelectStmtHandle,
                             SQL_ATTR_ROWS_FETCHED_PTR,
                             (SQLPOINTER)&aSyncMgr->mRowFetchedCount,
                             0 ) == SQL_SUCCESS );

    STL_TRY( SQLSetStmtAttr( sSelectStmtHandle,
                             SQL_ATTR_CURSOR_TYPE,
                             (SQLPOINTER)SQL_CURSOR_STATIC,
                             0 ) == STL_SUCCESS );

    STL_RING_FOREACH_ENTRY( &aSyncMgr->mTableList, sTableInfo )
    {
        sErrCount = 0;
        /**
         * ztcSyncColumnValue 할당.
         */
        STL_TRY( ztcnAllocSyncColumnValue( aSyncMgr,
                                           sTableInfo,
                                           aErrorStack ) == STL_SUCCESS );
        sState = 3;

        /**
         * Slave의 데이터가 운영중인 데이터일 수 있으므로 자동으로 삭제하지 않도록 한다.
         * 
        STL_TRY( ztcdDeleteSlaveRecord( sDeleteStmtHandle,
                                        sTableInfo,
                                        aErrorStack ) == STL_SUCCESS );
        */
        
        STL_TRY( ztcdSyncMgrSQLExecDirect( sSelectStmtHandle,
                                           sTableInfo,
                                           aErrorStack ) == STL_SUCCESS );

        sNextJob = ZTC_SYNC_NEXT_JOB_BINDPARAM;

        STL_TRY( ztcmLogMsg( aErrorStack,
                             "[SYNC] SyncManager (%s.%s) Table Prepared.\n",
                             sTableInfo->mMasterSchemaName,
                             sTableInfo->mMasterTableName ) == STL_SUCCESS );
        
        while( STL_TRUE )
        {
            STL_RING_FOREACH_ENTRY( &aSyncMgr->mBufferInfoList, sBufferInfo  )
            {
                /* BufferInfo에 현재 TableInfo 설정 */
                sBufferInfo->mTableInfoPtr = sTableInfo;

                /** Wait until Syncher finish executing */
                ZTC_WAIT_SYNC_MANAGER_TURN( gRunState, sBufferInfo->mIsFetched );

                STL_TRY( ztcdSyncMgrSQLBindCol( sSelectStmtHandle,
                                                sBufferInfo,
                                                aErrorStack ) == STL_SUCCESS );
                
                STL_TRY( ztcdSQLFetch( sSelectStmtHandle,
                                       &sRet,
                                       aErrorStack ) == STL_SUCCESS );
                switch( sRet )
                {
                    case SQL_SUCCESS:
                        STL_TRY( ztcdSyncMgrSQLGetData( sSelectStmtHandle,
                                                        &aSyncMgr->mDynamicMem,
                                                        sBufferInfo,
                                                        aSyncMgr->mRowFetchedCount,
                                                        aErrorStack ) == STL_SUCCESS );

                        sRecordCnt += sArraySize;

                        if( sRecordCnt > sTrcMsgCnt )
                        {
                            sTotalCnt += sRecordCnt;

                            STL_TRY( ztcmLogMsg( aErrorStack,
                                                 "[SYNC] SyncManager (%s.%s) Table %ld Records Fetched.\n",
                                                 sTableInfo->mMasterSchemaName,
                                                 sTableInfo->mMasterTableName,
                                                 sTotalCnt ) == STL_SUCCESS );
                            
                            sRecordCnt = 0;
                        }
                        
                        break;
                    case SQL_SUCCESS_WITH_INFO:
                        STL_THROW( RAMP_ERR_FETCH );
                        break;
                    case SQL_NO_DATA: /* sNextJob: */
                        STL_THROW( RAMP_NO_DATA_FINISH );
                        break;
                    default:
                        STL_THROW( RAMP_ERR_FETCH );
                        break;
                }

                sBufferInfo->mIsFetched = STL_TRUE;
                sBufferInfo->mNextJob = sNextJob;
                sBufferInfo->mFetchedCount = aSyncMgr->mRowFetchedCount;

                STL_TRY( stlReleaseSemaphore( &sBufferInfo->mSyncSem,
                                              aErrorStack ) == STL_SUCCESS );
            }

            sNextJob = ZTC_SYNC_NEXT_JOB_KEEP_EXEC;
        }

        STL_RAMP( RAMP_NO_DATA_FINISH );

        /**
         * @remarks SQLFetch 이후에 ztcSyncTableInfo에 ViewScn을 설정한다.
         */
        sBufferInfo  = STL_RING_GET_FIRST_DATA( &aSyncMgr->mBufferInfoList );
        sColumnValue = STL_RING_GET_LAST_DATA( &sBufferInfo->mColumnValueList );

        sTableInfo->mViewScn = *(stlUInt64 *) sColumnValue->mValue;
        
        /**
         * CaptureTableInfo와 TableMeta를 Update한다.
         */
        STL_TRY( ztcmUpdateTableCommitScn( sTableInfo->mPhysicalId,
                                           sTableInfo->mViewScn,
                                           aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcmLogMsg( aErrorStack,
                             "[SYNC] SyncManager (%s.%s) Table Fetch Done.\n",
                             sTableInfo->mMasterSchemaName,
                             sTableInfo->mMasterTableName ) == STL_SUCCESS );
        /**
         * Cursor Close: SQL_SUCCESS 이외는 ERROR
         */
        STL_TRY( ztcdSQLCloseCursor( sSelectStmtHandle,
                                     &sRet,
                                     aErrorStack ) == STL_SUCCESS );
        
        /**
         * Table Fetch 끝나고 Memory Free.
         */
        sState = 2;
        STL_TRY( ztcnFreeSyncColumnValue( aSyncMgr,
                                          &sErrCount,
                                          aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcmLogMsg( aErrorStack,
                             "[SYNC] SyncManager (%s.%s) Table Sync Done. Error Count = (%ld)\n",
                             sTableInfo->mMasterSchemaName,
                             sTableInfo->mMasterTableName,
                             sErrCount ) == STL_SUCCESS );
    }

    /**
     * Syncher에게 Table이 존재하지 않다는 것을 알린다.
     */
    STL_RING_FOREACH_ENTRY( &aSyncMgr->mBufferInfoList, sBufferInfo )
    {
        /** Wait until Syncher finish executing */
        ZTC_WAIT_SYNC_MANAGER_TURN( gRunState, sBufferInfo->mIsFetched );

        sBufferInfo->mNextJob   = ZTC_SYNC_NEXT_JOB_FINISH;
        sBufferInfo->mIsFetched = STL_TRUE;
        
        STL_TRY( stlReleaseSemaphore( &sBufferInfo->mSyncSem,
                                      aErrorStack ) == STL_SUCCESS );
    }

    sState = 1;
    STL_TRY( ztcdSQLFreeHandle( sDeleteStmtHandle,
                                aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sSelectStmtHandle,
                                aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FETCH )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             sSelectStmtHandle,
                             aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void) ztcnFreeSyncColumnValue( aSyncMgr,
                                            &sErrCount,
                                            aErrorStack );
        case 2:
            (void) ztcdSQLFreeHandle( sDeleteStmtHandle,
                                      aErrorStack );
        case 1:
            (void) ztcdSQLFreeHandle( sSelectStmtHandle,
                                      aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztcnAllocSyncColumnValue( ztcSyncMgr        * aSyncMgr,
                                    ztcSyncTableInfo  * aTableInfo,
                                    stlErrorStack     * aErrorStack )
{
    ztcColumnInfo      * sColumnInfo  = NULL;
    ztcSyncBufferInfo  * sBufferInfo  = NULL;
    ztcSyncColumnValue * sColumnValue = NULL;
    stlInt64             sArraySize   = aSyncMgr->mArraySize;
    stlInt64             sAllocSize;
    stlInt64             i;

    STL_RING_FOREACH_ENTRY( &aSyncMgr->mBufferInfoList, sBufferInfo )
    {
        STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnInfoList, sColumnInfo )
        {
            STL_TRY( stlAllocDynamicMem( &aSyncMgr->mDynamicMem,
                                         STL_SIZEOF( ztcSyncColumnValue ),
                                         (void**)&sColumnValue,
                                         aErrorStack ) == STL_SUCCESS );

            if( ( sColumnInfo->mDataTypeId == DTL_TYPE_LONGVARBINARY ) ||
                ( sColumnInfo->mDataTypeId == DTL_TYPE_LONGVARCHAR ) )
            {
                sAllocSize = STL_SIZEOF( SQL_LONG_VARIABLE_LENGTH_STRUCT ) * sArraySize;

                STL_TRY( stlAllocDynamicMem( &aSyncMgr->mDynamicMem,
                                             sAllocSize,
                                             (void**)&sColumnValue->mLongValue,
                                             aErrorStack ) == STL_SUCCESS );

                for( i = 0; i < sArraySize; i++ )
                {
                    sColumnValue->mLongValue[i].len = ZTC_LONG_VARYING_MEMORY_INIT_SIZE;
                    STL_TRY( stlAllocDynamicMem( &aSyncMgr->mDynamicMem,
                                                 sColumnValue->mLongValue[i].len,
                                                 (void**)&sColumnValue->mLongValue[i].arr,
                                                 aErrorStack ) == STL_SUCCESS );
                }
                
                sColumnValue->mValue = NULL;
            }
            else
            {
                /**
                 * INTERVAL 데이터타입은 스트링으로 변경한후에 INSERT한다...
                 */
                if( ( sColumnInfo->mDataTypeId == DTL_TYPE_INTERVAL_YEAR_TO_MONTH ) ||
                    ( sColumnInfo->mDataTypeId == DTL_TYPE_INTERVAL_DAY_TO_SECOND ) )
                {
                    sAllocSize = sArraySize * sColumnInfo->mStrValueMaxSize;
                }
                else
                {
                    sAllocSize = sArraySize * sColumnInfo->mValueMaxSize;
                }
                
                STL_TRY( stlAllocDynamicMem( &aSyncMgr->mDynamicMem,
                                             sAllocSize,
                                             (void**)&sColumnValue->mValue,
                                             aErrorStack ) == STL_SUCCESS );

                sColumnValue->mLongValue = NULL;
            }
            
            STL_TRY( stlAllocDynamicMem( &aSyncMgr->mDynamicMem,
                                         STL_SIZEOF( SQLLEN ) * sArraySize,
                                         (void**)&sColumnValue->mIndicator,
                                         aErrorStack ) == STL_SUCCESS );

            sColumnValue->mColumnInfoPtr = sColumnInfo;
            
            STL_RING_ADD_LAST( &sBufferInfo->mColumnValueList, sColumnValue );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcnFreeSyncColumnValue( ztcSyncMgr        * aSyncMgr,
                                   stlInt64          * aErrCount,
                                   stlErrorStack     * aErrorStack )
{
    ztcSyncBufferInfo  * sBufferInfo      = NULL;
    ztcSyncColumnValue * sColumnValue     = NULL;
    ztcSyncColumnValue * sNextColumnValue = NULL;
    ztcColumnInfo      * sColumnInfo      = NULL;
    stlInt64             sArraySize       = aSyncMgr->mArraySize;
    stlInt64             i;
    stlInt64             sErrCount        = 0;
    
    STL_RING_FOREACH_ENTRY( &aSyncMgr->mBufferInfoList, sBufferInfo )
    {
        /**
         * Wait until Syncher executed
         */ 
        ZTC_WAIT_SYNC_MANAGER_TURN( gRunState, sBufferInfo->mIsFetched );
        
        sErrCount += sBufferInfo->mErrCount;
        
        sBufferInfo->mErrCount = 0;
        
        STL_RING_FOREACH_ENTRY_SAFE( &sBufferInfo->mColumnValueList,
                                     sColumnValue,
                                     sNextColumnValue )
        {
            sColumnInfo = sColumnValue->mColumnInfoPtr;
            
            STL_RING_UNLINK( &sBufferInfo->mColumnValueList, sColumnValue );

            STL_TRY( stlFreeDynamicMem( &aSyncMgr->mDynamicMem,
                                        (void*)sColumnValue->mIndicator,
                                        aErrorStack ) == STL_SUCCESS );

            if( ( sColumnInfo->mDataTypeId == DTL_TYPE_LONGVARBINARY ) ||
                ( sColumnInfo->mDataTypeId == DTL_TYPE_LONGVARCHAR ) )
            {
                for( i = 0; i < sArraySize; i++ )
                {
                    STL_TRY( stlFreeDynamicMem( &aSyncMgr->mDynamicMem,
                                                sColumnValue->mLongValue[i].arr,
                                                aErrorStack ) == STL_SUCCESS );
                }

                STL_TRY( stlFreeDynamicMem( &aSyncMgr->mDynamicMem,
                                            (void*)sColumnValue->mLongValue,
                                            aErrorStack ) == STL_SUCCESS );

            }
            else
            {
                STL_TRY( stlFreeDynamicMem( &aSyncMgr->mDynamicMem,
                                            (void*)sColumnValue->mValue,
                                            aErrorStack ) == STL_SUCCESS );
            }

            STL_TRY( stlFreeDynamicMem( &aSyncMgr->mDynamicMem,
                                        (void*)sColumnValue,
                                        aErrorStack ) == STL_SUCCESS );
        }

    }
    
    *aErrCount = sErrCount;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcnReAllocLongVariable( stlDynamicAllocator             * aDynamicMem,
                                   SQL_LONG_VARIABLE_LENGTH_STRUCT * aLongVariable,
                                   stlInt64                          aSize,
                                   stlErrorStack                   * aErrorStack )
{
    stlInt64 sAllocSize;

    sAllocSize = aSize + 1;
    
    STL_TRY( stlFreeDynamicMem( aDynamicMem,
                                aLongVariable->arr,
                                aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlAllocDynamicMem( aDynamicMem,
                                 sAllocSize,
                                 (void**)&aLongVariable->arr,
                                 aErrorStack ) == STL_SUCCESS );

    aLongVariable->len = sAllocSize;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
