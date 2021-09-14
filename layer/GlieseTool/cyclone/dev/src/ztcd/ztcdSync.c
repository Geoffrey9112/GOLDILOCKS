/*******************************************************************************
 * ztcdSync.c
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
 * @file ztcdSync.c
 * @brief GlieseTool Cyclone Sync Operation Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcConfigure   gConfigure;

stlStatus ztcdGetSyncColumnInfo( SQLHDBC         aDbcHandle,
                                 stlChar       * aSchemaName,
                                 stlChar       * aTableName,
                                 dtlFuncVector * aDTVector,
                                 ztcColumnInfo * aColumnInfo,
                                 stlErrorStack * aErrorStack )
{
    stlInt64           sSchemaId;
    stlInt64           sTableId;
    stlInt64           sColumnId;
    stlInt64           sDataTypeId;
    stlInt64           sIntervalTypeId;
    
    STL_TRY( ztcdGetSchemaIDFromDB( aDbcHandle,
                                    aSchemaName,
                                    &sSchemaId,
                                    aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdGetTableIDFromDB( aDbcHandle,
                                   sSchemaId,
                                   aTableName,
                                   &sTableId,
                                   aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdGetColumnId( aDbcHandle,
                              sSchemaId,
                              sTableId,
                              aColumnInfo->mColumnName,
                              &sColumnId,
                              aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdGetColumnTypeId( aDbcHandle,
                                  sTableId,
                                  sColumnId,
                                  &sDataTypeId,
                                  &sIntervalTypeId,
                                  aErrorStack ) == STL_SUCCESS );

    aColumnInfo->mColumnId   = sColumnId;
    aColumnInfo->mDataTypeId = sDataTypeId;

    if( ( sDataTypeId == DTL_TYPE_FLOAT ) ||
        ( sDataTypeId == DTL_TYPE_NUMBER ) )
    {
        STL_TRY( ztcdGetNumericInfo( aDbcHandle,
                                     sTableId,
                                     sColumnId,
                                     &aColumnInfo->mNumericPrecision,
                                     &aColumnInfo->mNumericScale,
                                     aErrorStack ) == STL_SUCCESS );
    }

    STL_TRY( ztcdGetColumnParamInfo( aDbcHandle,
                                     sTableId,
                                     aDTVector,
                                     aColumnInfo,
                                     aErrorStack ) == STL_SUCCESS );
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcdSyncMgrSQLBindCol( SQLHSTMT            aStmt,
                                 ztcSyncBufferInfo * aBufferInfo,
                                 stlErrorStack     * aErrorStack )
{
    ztcSyncColumnValue * sColumnValue;
    ztcColumnInfo      * sColumnInfo;
    SQLUSMALLINT         sColumnNumber = 0;
    
    STL_RING_FOREACH_ENTRY( &aBufferInfo->mColumnValueList, sColumnValue )
    {
        sColumnInfo = sColumnValue->mColumnInfoPtr;

        sColumnNumber++;
        
        if( ( sColumnInfo->mDataTypeId == DTL_TYPE_LONGVARBINARY ) ||
            ( sColumnInfo->mDataTypeId == DTL_TYPE_LONGVARCHAR ) )
        {
            continue;
        }

        STL_TRY( ztcdSQLBindCol( aStmt,
                                 sColumnNumber,
                                 sColumnInfo->mValueType,
                                 (SQLPOINTER)sColumnValue->mValue,
                                 sColumnInfo->mBufferLength,
                                 sColumnValue->mIndicator,
                                 aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcdDeleteSlaveRecord( SQLHSTMT           aStmtHandle,
                                 ztcSyncTableInfo * aTableInfo,
                                 stlErrorStack    * aErrorStack )
{
    stlChar   sPrepareStr[ ZTC_COMMAND_BUFFER_SIZE ];
    SQLRETURN sRet;

    /**
     * DELETE 대신에 TRUNCATE 사용하는 이유
     * 서로 다른 DB가 동일한 Table에 이중으로 이중화를 하는데 Sync 기능을 나중에 수행하는 경우에 발생
     * : DELETE를 사용하면 Sync를 수행하는 Master에서 Slave의 Data를 지우고 Data를 삽입하는데
     * Slave(이전에 수행한 cyclone에서 Master)에서 Master(이전에 수행한 cyclone에서 Slave)로
     * Log를 보내어 Master에서 DELETE와 INSERT 작업을 이중으로 하게된다.
     * TRUNCATE를 사용하는 경우 Table의 physical id가 달라져서 이중으로 작업을 수행하지는 않지만,
     * 이전에 수행한 cyclone에서 해당 Table의 이중화는 동작하지 않게 된다.
     */ 
    stlSnprintf( sPrepareStr,
                 STL_SIZEOF( sPrepareStr ),
                 "TRUNCATE TABLE \"%s\".\"%s\"",
                 aTableInfo->mSlaveSchemaName,
                 aTableInfo->mSlaveTableName );

    STL_TRY( ztcdSQLExecDirect( aStmtHandle,
                                (SQLCHAR*) sPrepareStr,
                                SQL_NTS,
                                &sRet,
                                aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sRet == SQL_SUCCESS || sRet == SQL_NO_DATA, RAMP_ERR_EXECDIRECT );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EXECDIRECT )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief
 */ 
stlStatus ztcdSyncMgrSQLExecDirect( SQLHSTMT           aStmt,
                                    ztcSyncTableInfo * aTableInfo,
                                    stlErrorStack    * aErrorStack )
{
    stlChar         sPrepareStr[ ZTC_COMMAND_BUFFER_SIZE ];
    stlChar         sColumnStr[ ZTC_COMMAND_BUFFER_SIZE ] = { 0 };
    SQLRETURN       sRet;
    ztcColumnInfo * sColumnInfo;
    stlInt64        sColumnCount = 0;

    STL_TRY( ztcdSQLFreeStmt( aStmt,
                              SQL_UNBIND,
                              &sRet,
                              aErrorStack ) == STL_SUCCESS );
    
    /**
     * statement_view_scn을 위한 ztcColumnInfo 하나 더 있다.
     */ 
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnInfoList, sColumnInfo )
    {
        sColumnCount++;
        
        if( sColumnCount <= aTableInfo->mColumnCount )
        {
            stlSnprintf( sColumnStr,
                         STL_SIZEOF( sColumnStr ),
                         "%s \"%s\",",
                         sColumnStr, 
                         sColumnInfo->mColumnName );
        }
        else
        {
            /**
             * Column 마지막은 statement_view_scn()이다...
             */
            stlSnprintf( sColumnStr,
                         STL_SIZEOF( sColumnStr ),
                         "%s %s",
                         sColumnStr, 
                         sColumnInfo->mColumnName );
        }
    }
    
    stlSnprintf( sPrepareStr,
                 STL_SIZEOF( sPrepareStr ),
                 "SELECT %s FROM \"%s\".\"%s\"",
                 sColumnStr,
                 aTableInfo->mMasterSchemaName,
                 aTableInfo->mMasterTableName );
    
    STL_TRY( ztcdSQLExecDirect( aStmt,
                                (SQLCHAR*) sPrepareStr,
                                SQL_NTS,
                                &sRet,
                                aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sRet == SQL_SUCCESS, RAMP_ERR_EXECDIRECT );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EXECDIRECT )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmt,
                             aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcdSyncMgrSQLGetData( SQLHSTMT              aStmt,
                                 stlDynamicAllocator * aDynamicMem,
                                 ztcSyncBufferInfo   * aBufferInfo,
                                 stlInt64              aFetchedSize,
                                 stlErrorStack       * aErrorStack )
{
    ztcSyncColumnValue * sColumnValue;
    ztcColumnInfo      * sColumnInfo;
    SQLRETURN            sRet;
    SQLUSMALLINT         sColumnNumber = 0;
    stlInt64             sIdx          = 0;
    SQLCHAR              sSqlState[ZTC_DEFAULT_SQLSTATE_LENGTH];
    
    STL_RING_FOREACH_ENTRY( &aBufferInfo->mColumnValueList, sColumnValue )
    {
        sColumnInfo = sColumnValue->mColumnInfoPtr;

        sColumnNumber++;
        
        if( ( sColumnInfo->mDataTypeId == DTL_TYPE_LONGVARBINARY ) ||
            ( sColumnInfo->mDataTypeId == DTL_TYPE_LONGVARCHAR ) )
        {
            for( sIdx = 0; sIdx < aFetchedSize; sIdx++ )
            {
                STL_RAMP( RAMP_RETRY );
                
                STL_TRY( SQLSetPos( aStmt,
                                    (sIdx + 1),
                                    SQL_POSITION,
                                    SQL_LOCK_NO_CHANGE ) == SQL_SUCCESS );

                STL_TRY( ztcdSQLGetData( aStmt,
                                         sColumnNumber,
                                         sColumnInfo->mValueType,
                                         &sColumnValue->mLongValue[sIdx],
                                         sColumnValue->mLongValue[sIdx].len,
                                         &sColumnValue->mIndicator[sIdx],
                                         &sRet,
                                         aErrorStack ) == STL_SUCCESS );

                if( sRet == SQL_SUCCESS )
                {
                    /* do nothing */
                }
                else if( sRet == SQL_SUCCESS_WITH_INFO )
                {
                    STL_TRY( ztcdSyncMgrGetDiagSQLState( aStmt,
                                                         sSqlState,
                                                         aErrorStack ) == STL_SUCCESS );

                    if( stlStrcmp((stlChar*)sSqlState, ZTC_SQLSTATE_DATA_RIGHT_TRUNCATED )
                        == 0 )
                    {
                        STL_TRY( ztcnReAllocLongVariable( aDynamicMem,
                                                          &sColumnValue->mLongValue[sIdx],
                                                          sColumnValue->mIndicator[sIdx],
                                                          aErrorStack ) == STL_SUCCESS );
                        STL_THROW( RAMP_RETRY );
                    }
                    else
                    {
                        STL_THROW( RAMP_ERR_GET_DATA );
                    }
                }
                else
                {
                    STL_THROW( RAMP_ERR_GET_DATA );
                }
            }
        }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GET_DATA )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmt,
                             aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcdSyncherSQLBindParameter( SQLHSTMT        aStmtHandle,
                                       ztcSyncher    * aSyncher,
                                       stlErrorStack * aErrorStack )
{
    ztcSyncBufferInfo  * sBufferInfo = aSyncher->mBufferInfoPtr;
    ztcSyncColumnValue * sColumnValue;
    ztcColumnInfo      * sColumnInfo;
    SQLUSMALLINT         sColumnNumber = 0;
    SQLPOINTER           sBufferPtr;
    SQLLEN               sBufferLength;

    STL_RING_FOREACH_ENTRY( &sBufferInfo->mColumnValueList, sColumnValue )
    {
        sColumnInfo = sColumnValue->mColumnInfoPtr;

        sColumnNumber++;

        if( ( sColumnInfo->mDataTypeId == DTL_TYPE_LONGVARBINARY ) ||
            ( sColumnInfo->mDataTypeId == DTL_TYPE_LONGVARCHAR ) )
        {
            sBufferPtr    = (SQLPOINTER) sColumnValue->mLongValue;
            sBufferLength = 0;
        }
        else
        {
            sBufferPtr    = (SQLPOINTER) sColumnValue->mValue;
            
            /**
             * INTERVAL 데이터타입은 스트링으로 변경한후에 INSERT한다...
             */
            if( ( sColumnInfo->mDataTypeId == DTL_TYPE_INTERVAL_YEAR_TO_MONTH ) ||
                ( sColumnInfo->mDataTypeId == DTL_TYPE_INTERVAL_DAY_TO_SECOND ) )
            {
                sBufferLength = sColumnInfo->mStrValueMaxSize;
            }
            else
            {
                sBufferLength = sColumnInfo->mValueMaxSize;
            }
        }

        STL_TRY( ztcdSQLBindParameter( aStmtHandle,
                                       sColumnNumber,
                                       SQL_PARAM_INPUT,
                                       sColumnInfo->mValueType,
                                       sColumnInfo->mParameterType,
                                       sColumnInfo->mStringLengthUnit,
                                       sColumnInfo->mColumnSize,
                                       sColumnInfo->mDecimalDigits,
                                       sBufferPtr,
                                       sBufferLength,
                                       sColumnValue->mIndicator,
                                       aErrorStack ) == STL_SUCCESS );

        if( sColumnNumber == sBufferInfo->mTableInfoPtr->mColumnCount )
        {
            break;
        }
    }

    STL_TRY( SQLSetStmtAttr( aStmtHandle,
                             SQL_ATTR_PARAM_BIND_TYPE,
                             (SQLPOINTER) SQL_BIND_BY_COLUMN,
                             0 ) == SQL_SUCCESS );

    STL_TRY( SQLSetStmtAttr( aStmtHandle,
                             SQL_ATTR_PARAM_STATUS_PTR,
                             aSyncher->mArrRecStatus,
                             0 ) == SQL_SUCCESS );

    STL_TRY( SQLSetStmtAttr( aStmtHandle,
                             SQL_ATTR_PARAMS_PROCESSED_PTR,
                             &aSyncher->mRecProcessedCount,
                             0 ) == SQL_SUCCESS );
    
    /**
     * 에러 발생시 부분적인 INSERT 수행이 되어야 하므로 ...
    STL_TRY( SQLSetStmtAttr( aStmtHandle,
                             SQL_ATTR_ATOMIC_EXECUTION,
                             (SQLPOINTER) SQL_ATOMIC_EXECUTION_ON,
                             0 ) == SQL_SUCCESS );
    */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcdSyncherSQLPrepare( SQLHSTMT           aStmtHandle,
                                 ztcSyncTableInfo * aTableInfo,
                                 stlErrorStack    * aErrorStack )
{
    stlChar         sPrepareStr[ ZTC_COMMAND_BUFFER_SIZE ];
    stlInt64        sColumnCount = aTableInfo->mColumnCount;
    stlChar       * sSchemaName  = aTableInfo->mSlaveSchemaName;
    stlChar       * sTableName   = aTableInfo->mSlaveTableName;
    stlInt64        i;
    SQLRETURN       sRet;

    STL_TRY( ztcdSQLFreeStmt( aStmtHandle,
                              SQL_RESET_PARAMS,
                              &sRet,
                              aErrorStack ) == STL_SUCCESS );
    
    stlSnprintf( sPrepareStr,
                 STL_SIZEOF( sPrepareStr ),
                 "INSERT INTO \"%s\".\"%s\" VALUES(",
                 sSchemaName,
                 sTableName );

    for( i = 0; i < sColumnCount; i++ )
    {
        if( i + 1 == sColumnCount )
        {
            stlStrcat( sPrepareStr, " ? " );
        }
        else
        {
            stlStrcat( sPrepareStr, " ?," );
        }
    }

    stlStrcat( sPrepareStr, ")" );

    STL_TRY( ztcdSQLPrepare( aStmtHandle,
                             (SQLCHAR*)sPrepareStr,
                             SQL_NTS,
                             aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcdSyncMgrGetDiagSQLState( SQLHSTMT        aStmtHandle,
                                      SQLCHAR       * aSqlState,
                                      stlErrorStack * aErrorStack )
{
    SQLRETURN   sRet;
    SQLSMALLINT sStateLength;
    
    sRet = SQLGetDiagField( SQL_HANDLE_STMT,
                            aStmtHandle,
                            1,
                            SQL_DIAG_SQLSTATE,
                            aSqlState,
                            ZTC_DEFAULT_SQLSTATE_LENGTH,
                            &sStateLength );

    STL_TRY_THROW( SQL_SUCCEEDED( sRet ) || sRet == SQL_NO_DATA,
                   RAMP_ERR_GETDIAG );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GETDIAG )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
