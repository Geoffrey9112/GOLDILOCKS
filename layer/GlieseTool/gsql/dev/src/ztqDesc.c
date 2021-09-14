/*******************************************************************************
 * ztqDesc.c
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

#include <stl.h>
#include <dtl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztqDef.h>
#include <ztqExecute.h>
#include <ztqDisplay.h>
#include <ztqOdbcBridge.h>
#include <ztqHostVariable.h>
#include <ztqDesc.h>

extern SQLHDBC gZtqDbcHandle;
extern SQLHENV gZtqEnvHandle;

extern stlInt32 gZtqPageSize;
extern stlChar  gZtqDriver[];
extern stlChar  gZtqUserName[];

#define ZTQ_C_SBIGINT  SQL_C_SBIGINT

/*
 * Oracle은 SBIGINT를 지원하지 않으며 SQL_C_LONG이 64비트 정수형이다.
#define ZTQ_C_SBIGINT  SQL_C_LONG
 */

/**
 * @file ztqDesc.c
 * @brief Table Description Routines
 */

/**
 * @addtogroup ztqDesc
 * @{
 */

stlStatus ztqExecuteTableDesc( ztqIdentifier * aIdentifierList,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    stlInt32        sIdentifierCnt = 0;
    stlInt64        sSchemaId;
    stlInt64        sTableId;
    ztqIdentifier * sIdentifier;
    stlChar       * sTableName;
    stlChar       * sSchemaName;
    stlBool         sIsSuccess;
    
    stlChar         sOutSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar         sOutTableName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    
    /*
     * Identifier 개수를 구한다.
     * ex) desc PULIC.table_name  --> 2개
     * ex) desc table_name --> 1개
     */
    sIdentifier = aIdentifierList;
    
    while( sIdentifier != NULL )
    {
        sIdentifier = (ztqIdentifier*)(sIdentifier->mNext);
        sIdentifierCnt += 1;
    }

    if( sIdentifierCnt == 1 )
    {
        sSchemaName = NULL;
        sTableName = aIdentifierList->mIdentifier;
    }
    else
    {
        sSchemaName = aIdentifierList->mIdentifier;
        sTableName = ((ztqIdentifier*)(aIdentifierList->mNext))->mIdentifier;
    }

    /**
     * 테이블 정보를 출력한다.
     */
    
    STL_TRY( ztqPrintTableDesc( sSchemaName,
                                sTableName,
                                sOutSchemaName,
                                sOutTableName,
                                aAllocator,
                                aErrorStack )
             == STL_SUCCESS );

    if ( (stlStrcmp(sOutSchemaName, "FIXED_TABLE_SCHEMA" ) == 0) ||
         (stlStrcmp(sOutSchemaName, "PERFORMANCE_VIEW_SCHEMA" ) == 0) )
    {
        /**
         * FIXED TABLE, PERFORMANCE VIEW 들은 Index 및 Constraint 가 존재하지 않는다.
         */

        /**
         * OPEN 이전 단계에서 desc 조회시 Dictionary 정보를 조회할 수 없기 때문이며,
         * gsql 이 OPEN 단계 여부를 구별하지 않도록 하기 위함이다.
         */
    }
    else
    {
        /**
         * Schema ID 와 Table ID 획득
         */
        
        STL_TRY( ztqGetSchemaId( sOutSchemaName,
                                 & sSchemaId,
                                 & sIsSuccess,
                                 aAllocator,
                                 aErrorStack )
                 == STL_SUCCESS );

        STL_TRY_THROW( sIsSuccess == STL_TRUE, RAMP_ERR_TABLE_NOT_EXIST );

        STL_TRY( ztqGetTableId( sOutTableName,
                                sSchemaId,
                                & sTableId,
                                & sIsSuccess,
                                aAllocator,
                                aErrorStack )
                 == STL_SUCCESS );

        STL_TRY_THROW( sIsSuccess == STL_TRUE, RAMP_ERR_TABLE_NOT_EXIST );
        
        /**
         * 인덱스 정보를 출력한다.
         */
        
        STL_TRY( ztqPrintIndexInfo( sSchemaId,
                                    sTableId,
                                    aAllocator,
                                    aErrorStack )
                 == STL_SUCCESS );
        
        /**
         * 제약조건 정보를 출력한다.
         */
        
        STL_TRY( ztqPrintConstraintInfo( sSchemaId,
                                         sTableId,
                                         aAllocator,
                                         aErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TABLE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_TABLE_NAME,
                      NULL,
                      aErrorStack,
                      sTableName );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqExecuteIndexDesc( ztqIdentifier * aIdentifierList,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    stlInt32        sIdentifierCnt = 0;
    stlInt64        sSchemaId;
    stlInt64        sIndexId;
    ztqIdentifier * sIdentifier;
    stlChar       * sIndexName;
    stlChar       * sSchemaName;
    stlBool         sIsSuccess;

    stlChar         sOutIndexName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar         sOutSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    /*
     * Identifier 개수를 구한다.
     * ex) idesc PULIC.index_name  --> 2개
     * ex) idesc index_name --> 1개
     */
    sIdentifier = aIdentifierList;
    
    while( sIdentifier != NULL )
    {
        sIdentifier = (ztqIdentifier*)(sIdentifier->mNext);
        sIdentifierCnt += 1;
    }

    if( sIdentifierCnt == 1 )
    {
        sSchemaName = NULL;
        sIndexName = aIdentifierList->mIdentifier;
    }
    else
    {
        sSchemaName = aIdentifierList->mIdentifier;
        sIndexName = ((ztqIdentifier*)(aIdentifierList->mNext))->mIdentifier;
    }

    /**
     * 사용자가 접근 가능한 Index Name 과 Schema Name 을 얻는다
     */

    STL_TRY( ztqGetAccessableIndex( sSchemaName,
                                    sIndexName,
                                    sOutSchemaName,
                                    sOutIndexName,
                                    aAllocator,
                                    aErrorStack )
             == STL_SUCCESS );

    /**
     * Schema ID 와 Table ID 획득
     */
    
    STL_TRY( ztqGetSchemaId( sOutSchemaName,
                             & sSchemaId,
                             & sIsSuccess,
                             aAllocator,
                             aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sIsSuccess == STL_TRUE, RAMP_ERR_INDEX_NOT_EXIST );

    STL_TRY( ztqGetIndexId( sOutIndexName,
                            sSchemaId,
                            & sIndexId,
                            & sIsSuccess,
                            aAllocator,
                            aErrorStack )
                 == STL_SUCCESS );

    STL_TRY_THROW( sIsSuccess == STL_TRUE, RAMP_ERR_INDEX_NOT_EXIST );
    
    /**
     * 인덱스 정보를 출력한다.
     */
    
    STL_TRY( ztqPrintIndexDesc( sIndexId,
                                aAllocator,
                                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INDEX_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_INDEX_NAME,
                      NULL,
                      aErrorStack,
                      sIndexName );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqGetSchemaId( stlChar       * aSchemaName,
                          stlInt64      * aSchemaId,
                          stlBool       * aIsSuccess,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack )
{
    stlInt64 * sSchemaId;
    stlInt32   sSchemaCount;
    stlChar    sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT SCHEMA_ID FROM DEFINITION_SCHEMA.SCHEMATA "
                 "WHERE SCHEMA_NAME = \'%s\'",
                 aSchemaName );
    
    STL_TRY( ztqGetObjectIds( sSqlString,
                              &sSchemaId,
                              &sSchemaCount,
                              aIsSuccess,
                              aAllocator,
                              aErrorStack )
             == STL_SUCCESS );

    if( *aIsSuccess == STL_TRUE )
    {
        *aSchemaId = sSchemaId[0];
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztqGetTableId( stlChar       * aTableName,
                         stlInt64        aSchemaId,
                         stlInt64      * aTableId,
                         stlBool       * aIsSuccess,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack )
{
    stlInt64 * sTableId;
    stlInt32   sTableCount;
    stlChar    sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT TABLE_ID FROM DEFINITION_SCHEMA.TABLES "
                 "WHERE TABLE_NAME = \'%s\' "
                 "  AND SCHEMA_ID = %ld "
                 "  AND TABLE_TYPE <> 'SEQUENCE' ",
                 aTableName,
                 aSchemaId );

    STL_TRY( ztqGetObjectIds( sSqlString,
                              &sTableId,
                              &sTableCount,
                              aIsSuccess,
                              aAllocator,
                              aErrorStack )
             == STL_SUCCESS );

    if( *aIsSuccess == STL_TRUE )
    {
        *aTableId = sTableId[0];
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztqGetIndexId( stlChar       * aIndexName,
                         stlInt64        aSchemaId,
                         stlInt64      * aIndexId,
                         stlBool       * aIsSuccess,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack )
{
    stlInt64 * sIndexId;
    stlInt32   sIndexCount;
    stlChar    sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT INDEX_ID FROM DEFINITION_SCHEMA.INDEXES "
                 "WHERE INDEX_NAME = \'%s\' AND SCHEMA_ID = %ld",
                 aIndexName,
                 aSchemaId );

    STL_TRY( ztqGetObjectIds( sSqlString,
                              &sIndexId,
                              &sIndexCount,
                              aIsSuccess,
                              aAllocator,
                              aErrorStack )
             == STL_SUCCESS );

    if( *aIsSuccess == STL_TRUE )
    {
        *aIndexId = sIndexId[0];
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztqGetTablespaceName( stlInt64        aTablespaceId,
                                stlChar       * aTablespaceName,
                                stlBool       * aIsSuccess,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack )
{
    stlChar sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT TABLESPACE_NAME FROM DEFINITION_SCHEMA.TABLESPACES "
                 "WHERE TABLESPACE_ID = %ld",
                 aTablespaceId );

    STL_TRY( ztqGetObjectName( sSqlString,
                               aTablespaceName,
                               aIsSuccess,
                               aAllocator,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztqGetColumnName( stlInt64        aColumnId,
                            stlChar       * aColumnName,
                            stlBool       * aIsSuccess,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack )
{
    stlChar sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT COLUMN_NAME "
                 "FROM DEFINITION_SCHEMA.COLUMNS "
                 "WHERE COLUMN_ID = %ld",
                 aColumnId );

    STL_TRY( ztqGetObjectName( sSqlString,
                               aColumnName,
                               aIsSuccess,
                               aAllocator,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztqGetIndexInfo( stlInt64        aIndexId,
                           stlChar       * aIndexName,
                           stlChar       * aTablespaceName,
                           stlChar       * aIndexTypeStr,
                           stlChar       * aIsUniqueStr,
                           stlBool       * aIsSuccess,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack )
{
    stlChar      sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    SQLHSTMT     sStmtHandle;
    stlInt32     sState = 0;
    SQLLEN       sIndicator;
    SQLRETURN    sResult;
    stlInt64     sTablespaceId;
    
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT INDEX_NAME, TABLESPACE_ID, INDEX_TYPE, IS_UNIQUE "
                 "FROM DEFINITION_SCHEMA.INDEXES "
                 "WHERE INDEX_ID = %ld",
                 aIndexId );

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*)sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            aIndexName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            &sIndicator,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            ZTQ_C_SBIGINT,
                            &sTablespaceId,
                            STL_SIZEOF( stlInt64 ),
                            &sIndicator,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            aIndexTypeStr,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            &sIndicator,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            aIsUniqueStr,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            &sIndicator,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );

    if( sResult == SQL_NO_DATA )
    {
        *aIsSuccess = STL_FALSE;
    }
    else
    {
        STL_TRY( ztqGetTablespaceName( sTablespaceId,
                                       aTablespaceName,
                                       aIsSuccess,
                                       aAllocator,
                                       aErrorStack )
                 == STL_SUCCESS );
    }

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );
    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                      aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqGetIndexColumnNameList( stlInt64        aIndexId,
                                     stlChar       * aColumnNameList,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack )
{
    stlBool           sIsSuccess;
    stlInt64        * sColumnIds;
    stlInt32          sColumnIdCount;
    stlChar           sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    stlInt32          i;
    stlChar           sColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    aColumnNameList[0] = '\0';
    
    /**
     * SQL 생성
     * ORDER BY 를 사용할 경우, user default TEMPORARY TABLE 가 없으면 구문 에러가 발생함.
     */
    
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       /*+ INDEX( INDEX_KEY_COLUMN_USAGE, INDEX_KEY_COLUMN_USAGE_UNIQUE_INDEX_SCHEMA_ID_INDEX_ID_ORDINAL_POSITION_INDEX ) */  "
                 "       COLUMN_ID "
                 "  FROM DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE "
                 " WHERE INDEX_ID = %ld ",
                 aIndexId );

    STL_TRY( ztqGetObjectIds( sSqlString,
                              &sColumnIds,
                              &sColumnIdCount,
                              &sIsSuccess,
                              aAllocator,
                              aErrorStack )
             == STL_SUCCESS );

    for( i = 0; i < sColumnIdCount; i++ )
    {
        STL_TRY( ztqGetColumnName( sColumnIds[i],
                                   sColumnName,
                                   &sIsSuccess,
                                   aAllocator,
                                   aErrorStack )
                 == STL_SUCCESS );

        /*
         * 충분한 버퍼공간이 남아 있는 경우까지 인덱스 리스트를 구성한다.
         * 아래 3의 의미는 ("," + NULL 문자)를 위한 공간이다.
         */
        if( (stlStrlen( aColumnNameList ) + stlStrlen( sColumnName ) + 2) >=
            ZTQ_MAX_COMMAND_BUFFER_SIZE )
        {
            break;
        }

        if( i != 0 )
        {
            stlStrnncat( aColumnNameList,
                         ",",
                         ZTQ_MAX_COMMAND_BUFFER_SIZE,
                         STL_UINT32_MAX );
        }
        
        stlStrnncat( aColumnNameList,
                     sColumnName,
                     ZTQ_MAX_COMMAND_BUFFER_SIZE,
                     STL_UINT32_MAX );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqGetKeyConstColumnNameList( stlInt64        aConstId,
                                        stlChar       * aColumnNameList,
                                        stlAllocator  * aAllocator,
                                        stlErrorStack * aErrorStack )
{
    stlBool           sIsSuccess;
    stlInt64        * sColumnIds;
    stlInt32          sColumnIdCount;
    stlChar           sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    stlInt32          i;
    stlChar           sColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    aColumnNameList[0] = '\0';
    
    /**
     * SQL 생성
     * ORDER BY 를 사용할 경우, user default TEMPORARY TABLE 가 없으면 구문 에러가 발생함.
     */
    
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "    /*+ INDEX( KEY_COLUMN_USAGE, KEY_COLUMN_USAGE_UNIQUE_CONSTRAINT_SCHEMA_ID_CONSTRAINT_ID_ORDINAL_POSITION_INDEX ) */ "  
                 "       COLUMN_ID "
                 "  FROM DEFINITION_SCHEMA.KEY_COLUMN_USAGE "
                 " WHERE CONSTRAINT_ID = %ld ",
                 aConstId );

    STL_TRY( ztqGetObjectIds( sSqlString,
                              &sColumnIds,
                              &sColumnIdCount,
                              &sIsSuccess,
                              aAllocator,
                              aErrorStack )
             == STL_SUCCESS );

    for( i = 0; i < sColumnIdCount; i++ )
    {
        STL_TRY( ztqGetColumnName( sColumnIds[i],
                                   sColumnName,
                                   &sIsSuccess,
                                   aAllocator,
                                   aErrorStack )
                 == STL_SUCCESS );

        /*
         * 충분한 버퍼공간이 남아 있는 경우까지 인덱스 리스트를 구성한다.
         * 아래 3의 의미는 ("," + NULL 문자)를 위한 공간이다.
         */
        if( (stlStrlen( aColumnNameList ) + stlStrlen( sColumnName ) + 2) >=
            ZTQ_MAX_COMMAND_BUFFER_SIZE )
        {
            break;
        }

        if( i != 0 )
        {
            stlStrnncat( aColumnNameList,
                         ",",
                         ZTQ_MAX_COMMAND_BUFFER_SIZE,
                         STL_UINT32_MAX );
        }
        
        stlStrnncat( aColumnNameList,
                     sColumnName,
                     ZTQ_MAX_COMMAND_BUFFER_SIZE,
                     STL_UINT32_MAX );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqGetCheckConstColumnNameList( stlInt64        aConstId,
                                          stlChar       * aColumnNameList,
                                          stlAllocator  * aAllocator,
                                          stlErrorStack * aErrorStack )
{
    stlBool           sIsSuccess;
    stlInt64        * sColumnIds;
    stlInt32          sColumnIdCount;
    stlChar           sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    stlInt32          i;
    stlChar           sColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    aColumnNameList[0] = '\0';
    
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT COLUMN_ID "
                 "  FROM DEFINITION_SCHEMA.CHECK_COLUMN_USAGE "
                 " WHERE CONSTRAINT_ID = %ld",
                 aConstId );

    STL_TRY( ztqGetObjectIds( sSqlString,
                              &sColumnIds,
                              &sColumnIdCount,
                              &sIsSuccess,
                              aAllocator,
                              aErrorStack )
             == STL_SUCCESS );

    for( i = 0; i < sColumnIdCount; i++ )
    {
        STL_TRY( ztqGetColumnName( sColumnIds[i],
                                   sColumnName,
                                   &sIsSuccess,
                                   aAllocator,
                                   aErrorStack )
                 == STL_SUCCESS );

        /*
         * 충분한 버퍼공간이 남아 있는 경우까지 인덱스 리스트를 구성한다.
         * 아래 3의 의미는 ("," + NULL 문자)를 위한 공간이다.
         */
        if( (stlStrlen( aColumnNameList ) + stlStrlen( sColumnName ) + 2) >=
            ZTQ_MAX_COMMAND_BUFFER_SIZE )
        {
            break;
        }

        if( i != 0 )
        {
            stlStrnncat( aColumnNameList,
                         ",",
                         ZTQ_MAX_COMMAND_BUFFER_SIZE,
                         STL_UINT32_MAX );
        }
        
        stlStrnncat( aColumnNameList,
                     sColumnName,
                     ZTQ_MAX_COMMAND_BUFFER_SIZE,
                     STL_UINT32_MAX );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqGetObjectIds( stlChar       * aSqlString,
                           stlInt64     ** aObjectIds,
                           stlInt32      * aObjectIdCount,
                           stlBool       * aIsSuccess,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack )
{
    SQLHSTMT     sStmtHandle;
    stlInt32     sState = 0;
    SQLLEN       sIndicator;
    SQLRETURN    sResult;
    stlInt64     sObjectId;
    stlInt64     sObjectIdCount = 0;
    stlInt64   * sObjectIds = NULL;
    stlInt64   * sTmpObjectIds = NULL;
    stlInt32     sRowCount = 0;
    
    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*)aSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    sRowCount = 100;

    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(stlInt64) * sRowCount,
                                (void**)&sObjectIds,
                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            ZTQ_C_SBIGINT,
                            &sObjectId,
                            STL_SIZEOF( stlInt64 ),
                            &sIndicator,
                            aErrorStack )
             == STL_SUCCESS );

    while( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if( sResult == SQL_NO_DATA )
        {
            break;
        }

        if( sObjectIdCount >= sRowCount )
        {
            sTmpObjectIds = sObjectIds;
            
            STL_TRY( stlAllocRegionMem( aAllocator,
                                        STL_SIZEOF(stlInt64) * (sRowCount + 100),
                                        (void**)&sObjectIds,
                                        aErrorStack )
                     == STL_SUCCESS );

            stlMemcpy( sObjectIds,
                       sTmpObjectIds,
                       STL_SIZEOF( stlInt64 ) * sRowCount );
        }
        
        sObjectIds[sObjectIdCount] = sObjectId;
        sObjectIdCount += 1;
    }

    *aObjectIdCount = sObjectIdCount;
    *aObjectIds = sObjectIds;

    if( sObjectIdCount == 0 )
    {
        *aIsSuccess = STL_FALSE;
    }
    else
    {
        *aIsSuccess = STL_TRUE;
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztqGetObjectName( stlChar       * aSqlString,
                            stlChar       * aObjectName,
                            stlBool       * aIsSuccess,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack )
{
    SQLHSTMT     sStmtHandle;
    stlInt32     sState = 0;
    SQLLEN       sIndicator;
    SQLRETURN    sResult;
    
    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*)aSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            aObjectName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            &sIndicator,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );

    if( sResult == SQL_NO_DATA )
    {
        *aIsSuccess = STL_FALSE;
    }
    else
    {
        *aIsSuccess = STL_TRUE;
    }

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintTableDesc( stlChar       * aInSchemaName,
                             stlChar       * aInTableName,
                             stlChar       * aOutSchemaName,
                             stlChar       * aOutTableName,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack )
{
    SQLHSTMT          sStmtHandle;
    stlInt32          i = 0;
    stlInt32          sState = 0;
    stlChar           sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    SQLRETURN         sResult;
    stlChar           sNullableStr[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar           sDataTypeStr[STL_MAX_SQL_IDENTIFIER_LENGTH];
    ztqRowBuffer    * sRowBuffer;
    ztqTargetColumn   sTargetColumns[3];
    stlInt64          sRowCount = 0;          
    stlInt64          sLineFetchCount = 0;

    SQLHDESC      sDescIrd = NULL;
    
    SQLSMALLINT   sColIdx = 0;
    SQLSMALLINT   sColumnCount = 0;

    stlChar       sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar       sTableName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar       sColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    SQLSMALLINT   sConciseType;
    SQLSMALLINT   sCharLengthUnit;
    stlChar       sTypeName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    SQLSMALLINT   sFixedPrecScale;
    SQLINTEGER    sNumPrecRadix;
    SQLSMALLINT   sPrecision;
    SQLSMALLINT   sScale;

    SQLULEN       sLength;
    SQLINTEGER    sDatetimeIntervalPrecision;
    
    SQLSMALLINT   sNullable;

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    /**
     * 출력을 위한 대상 컬럼을 구성한다
     * 
     * COLUMN_NAME TYPE IS_NULLABLE
     * ----------- ---- -----------
     *    ...       ..      ...
     */
    stlStrcpy( sTargetColumns[0].mName, "COLUMN_NAME" );
    sTargetColumns[0].mDataType = SQL_CHAR;
    sTargetColumns[0].mDataSize = STL_MAX_SQL_IDENTIFIER_LENGTH;
    sTargetColumns[0].mDataValue = sColumnName;
    stlStrcpy( sTargetColumns[0].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_STRING );
    sTargetColumns[0].mDataTypeGroup = ZTQ_GROUP_STRING;
    
    stlStrcpy( sTargetColumns[1].mName, "TYPE" );
    sTargetColumns[1].mDataType = SQL_CHAR;
    sTargetColumns[1].mDataSize = STL_MAX_SQL_IDENTIFIER_LENGTH;
    sTargetColumns[1].mDataValue = sDataTypeStr;
    stlStrcpy( sTargetColumns[1].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_STRING );
    sTargetColumns[1].mDataTypeGroup = ZTQ_GROUP_STRING;
    
    stlStrcpy( sTargetColumns[2].mName, "IS_NULLABLE" );
    sTargetColumns[2].mDataType = SQL_CHAR;
    sTargetColumns[2].mDataSize = STL_MAX_SQL_IDENTIFIER_LENGTH;
    sTargetColumns[2].mDataValue = sNullableStr;
    stlStrcpy( sTargetColumns[2].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_STRING );
    sTargetColumns[2].mDataTypeGroup = ZTQ_GROUP_STRING;
    
    STL_TRY( ztqAllocRowBuffer( &sRowBuffer,
                                3,
                                gZtqPageSize,
                                sTargetColumns,
                                aAllocator,
                                aErrorStack )
             == STL_SUCCESS );
    
    /**
     * 해당 Table Name 으로 결과가 없는 SELECT 구문을 수행
     */

    if ( aInSchemaName == NULL )
    {
        stlSnprintf( sSqlString,
                     ZTQ_MAX_COMMAND_BUFFER_SIZE,
                     "SELECT * "
                     "  FROM %s "
                     " WHERE 1 = 0 ",
                     aInTableName );
    }
    else
    {
        stlSnprintf( sSqlString,
                     ZTQ_MAX_COMMAND_BUFFER_SIZE,
                     "SELECT * "
                     "  FROM %s.%s "
                     " WHERE 1 = 0 ",
                     aInSchemaName,
                     aInTableName );
    }

    STL_TRY( ztqDescPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             SQL_NTS,
                             & sResult,
                             aAllocator,
                             aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_TABLE_NOT_EXIST );

    /**
     * Target 개수를 얻는다
     */
    
    STL_TRY( ztqSQLNumResultCols( sStmtHandle,
                                  &sColumnCount,
                                  aErrorStack ) == STL_SUCCESS );

    /**
     * Target 의 IRD(Implementaion Row Descriptor) 정보를 획득한다.
     */
    
    STL_TRY( ztqSQLGetStmtAttr( sStmtHandle,
                                SQL_ATTR_IMP_ROW_DESC,
                                & sDescIrd,
                                SQL_IS_POINTER, 
                                NULL,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * IRD 로부터 Schema Name 과 Table Name 획득
     */

    STL_TRY( ztqSQLGetDescField( sDescIrd,
                                 1,
                                 SQL_DESC_SCHEMA_NAME,
                                 sSchemaName,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH,
                                 NULL,
                                 aErrorStack )
             == STL_SUCCESS );

    stlStrncpy( aOutSchemaName,
                sSchemaName,
                STL_MAX_SQL_IDENTIFIER_LENGTH );
    
    STL_TRY( ztqSQLGetDescField( sDescIrd,
                                 1,
                                 SQL_DESC_TABLE_NAME,
                                 sTableName,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH,
                                 NULL,
                                 aErrorStack )
             == STL_SUCCESS );

    stlStrncpy( aOutTableName,
                sTableName,
                STL_MAX_SQL_IDENTIFIER_LENGTH );
    
    /**
     * 각 Column 의 정보를 구축
     */
    
    for( sColIdx = 0; sColIdx < sColumnCount; sColIdx++ )
    {
        /**
         * IRD 로부터 Target 관련 정보를 획득
         */

        STL_TRY( ztqSQLGetDescField( sDescIrd,
                                     sColIdx + 1,
                                     SQL_DESC_LABEL,
                                     sColumnName,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     NULL,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqSQLGetDescField( sDescIrd,
                                     sColIdx + 1,
                                     SQL_DESC_CONCISE_TYPE,
                                     (SQLPOINTER) & sConciseType,
                                     STL_SIZEOF( SQLSMALLINT ),
                                     NULL,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqSQLGetDescField( sDescIrd,
                                     sColIdx + 1,
                                     SQL_DESC_TYPE_NAME,
                                     sTypeName,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     NULL,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqSQLGetDescField( sDescIrd,
                                     sColIdx + 1,
                                     SQL_DESC_FIXED_PREC_SCALE,
                                     (SQLPOINTER) & sFixedPrecScale,
                                     STL_SIZEOF( SQLSMALLINT ),
                                     NULL,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqSQLGetDescField( sDescIrd,
                                     sColIdx + 1,
                                     SQL_DESC_NUM_PREC_RADIX,
                                     (SQLPOINTER) & sNumPrecRadix,
                                     STL_SIZEOF( SQLINTEGER ),
                                     NULL,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqSQLGetDescField( sDescIrd,
                                     sColIdx + 1,
                                     SQL_DESC_PRECISION,
                                     (SQLPOINTER) & sPrecision,
                                     STL_SIZEOF( SQLSMALLINT ),
                                     NULL,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqSQLGetDescField( sDescIrd,
                                     sColIdx + 1,
                                     SQL_DESC_SCALE,
                                     (SQLPOINTER) & sScale,
                                     STL_SIZEOF( SQLSMALLINT ),
                                     NULL,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqSQLGetDescField( sDescIrd,
                                     sColIdx + 1,
                                     SQL_DESC_LENGTH,
                                     (SQLPOINTER) & sLength,
                                     STL_SIZEOF( SQLULEN ),
                                     NULL,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqSQLGetDescField( sDescIrd,
                                     sColIdx + 1,
                                     SQL_DESC_DATETIME_INTERVAL_PRECISION,
                                     (SQLPOINTER) & sDatetimeIntervalPrecision,
                                     STL_SIZEOF( SQLINTEGER ),
                                     NULL,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqSQLGetDescField( sDescIrd,
                                     sColIdx + 1,
                                     SQL_DESC_NULLABLE,
                                     (SQLPOINTER) & sNullable,
                                     STL_SIZEOF( SQLSMALLINT ),
                                     NULL,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqSQLGetDescField( sDescIrd,
                                     sColIdx + 1,
                                     SQL_DESC_CHAR_LENGTH_UNITS,
                                     (SQLPOINTER) & sCharLengthUnit,
                                     STL_SIZEOF( SQLSMALLINT ),
                                     NULL,
                                     aErrorStack )
                 == STL_SUCCESS );

        /**
         * Type 이름을 결정
         */

        switch( sConciseType )
        {
            case SQL_CHAR :
                if( sCharLengthUnit == SQL_CLU_CHARACTERS )
                {
                    stlSnprintf( sDataTypeStr,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH,
#if (STL_SIZEOF_VOIDP == 8)
                                 "%s(%lu CHAR)",
#else
                                 "%s(%u CHAR)",
#endif
                                 sTypeName,
                                 sLength );
                }
                else
                {
                    stlSnprintf( sDataTypeStr,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH,
#if (STL_SIZEOF_VOIDP == 8)
                                 "%s(%lu)",
#else
                                 "%s(%u)",
#endif
                                 sTypeName,
                                 sLength );
                }
                break;
            case SQL_VARCHAR :
                if( sCharLengthUnit == SQL_CLU_CHARACTERS )
                {
                    stlSnprintf( sDataTypeStr,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH,
#if (STL_SIZEOF_VOIDP == 8)
                                 "%s(%lu CHAR)",
#else
                                 "%s(%u CHAR)",
#endif
                                 sTypeName,
                                 sLength );
                }
                else
                {
                    stlSnprintf( sDataTypeStr,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH,
#if (STL_SIZEOF_VOIDP == 8)
                                 "%s(%lu)",
#else
                                 "%s(%u)",
#endif
                                 sTypeName,
                                 sLength );
                }
                break;
            case SQL_LONGVARCHAR :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             sTypeName );
                break;
            case SQL_WCHAR :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
#if (STL_SIZEOF_VOIDP == 8)
                             "%s(%lu)",
#else
                             "%s(%u)",
#endif
                             sTypeName,
                             sLength );
                break;
            case SQL_WVARCHAR :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
#if (STL_SIZEOF_VOIDP == 8)
                             "%s(%lu)",
#else
                             "%s(%u)",
#endif
                             sTypeName,
                             sLength );
                break;
            case SQL_WLONGVARCHAR :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             sTypeName );
                break;
            case SQL_NUMERIC :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "NUMBER(%d,%d)",
                             sPrecision,
                             sScale );
                break;
            case SQL_DECIMAL :
                if ( sFixedPrecScale == SQL_TRUE )
                {
                    stlSnprintf( sDataTypeStr,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH,
                                 "DECIMAL(%d,%d)",
                                 sPrecision,
                                 sScale );
                }
                else
                {
                    stlSnprintf( sDataTypeStr,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH,
                                 "DECIMAL" );
                }
                break;
            case SQL_FLOAT :
                if( stlStrcmp( sTypeName, "NUMBER" ) == 0 )
                {
                    stlSnprintf( sDataTypeStr,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH,
                                 "%s",
                                 sTypeName );
                }
                else
                {
                    stlSnprintf( sDataTypeStr,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH,
                                 "%s(%d)",
                                 sTypeName,
                                 sPrecision );
                }
                break;
            case SQL_SMALLINT :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             sTypeName );
                break;
            case SQL_INTEGER :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             sTypeName );
                break;
            case SQL_BIGINT :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             sTypeName );
                break;
            case SQL_REAL :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             sTypeName );
                break;
            case SQL_DOUBLE :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             sTypeName );
                break;
            case SQL_BINARY :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
#if (STL_SIZEOF_VOIDP == 8)
                             "%s(%lu)",
#else
                             "%s(%u)",
#endif
                             sTypeName,
                             sLength );
                break;
            case SQL_VARBINARY :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
#if (STL_SIZEOF_VOIDP == 8)
                             "%s(%lu)",
#else
                             "%s(%u)",
#endif
                             sTypeName,
                             sLength );
                break;
            case SQL_LONGVARBINARY :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             sTypeName );
                break;
            case SQL_TYPE_DATE :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             sTypeName );
                break;
            case SQL_TYPE_TIME :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "TIME(%d) WITHOUT TIME ZONE",
                             sPrecision );
                break;
            case SQL_TYPE_TIME_WITH_TIMEZONE :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "TIME(%d) WITH TIME ZONE",
                             sPrecision );
                break;
            case SQL_TYPE_TIMESTAMP :
                if( stlStrcmp( sTypeName, "DATE" ) == 0 )
                {
                    stlSnprintf( sDataTypeStr,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH,
                                 "DATE",
                                 sTypeName );
                }
                else
                {
                    stlSnprintf( sDataTypeStr,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH,
                                 "TIMESTAMP(%d) WITHOUT TIME ZONE",
                                 sPrecision );                    
                }
                break;
            case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "TIMESTAMP(%d) WITH TIME ZONE",
                             sPrecision );
                break;
            case SQL_INTERVAL_YEAR :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "INTERVAL YEAR(%d)",
                             sDatetimeIntervalPrecision );
                break;
            case SQL_INTERVAL_MONTH :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "INTERVAL MONTH(%d)",
                             sDatetimeIntervalPrecision );
                break;
            case SQL_INTERVAL_YEAR_TO_MONTH :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "INTERVAL YEAR(%d) TO MONTH",
                             sDatetimeIntervalPrecision );
                break;
            case SQL_INTERVAL_DAY :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "INTERVAL DAY(%d)",
                             sDatetimeIntervalPrecision );
                break;
            case SQL_INTERVAL_HOUR :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "INTERVAL HOUR(%d)",
                             sDatetimeIntervalPrecision );
                break;
            case SQL_INTERVAL_MINUTE :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "INTERVAL MINUTE(%d)",
                             sDatetimeIntervalPrecision );
                break;
            case SQL_INTERVAL_SECOND :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "INTERVAL SECOND(%d,%d)",
                             sDatetimeIntervalPrecision,
                             sPrecision );
                break;
            case SQL_INTERVAL_DAY_TO_HOUR :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "INTERVAL DAY(%d) TO HOUR",
                             sDatetimeIntervalPrecision );
                break;
            case SQL_INTERVAL_DAY_TO_MINUTE :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "INTERVAL DAY(%d) TO MINUTE",
                             sDatetimeIntervalPrecision );
                break;
            case SQL_INTERVAL_DAY_TO_SECOND :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "INTERVAL DAY(%d) TO SECOND(%d)",
                             sDatetimeIntervalPrecision,
                             sPrecision );
                break;
            case SQL_INTERVAL_HOUR_TO_MINUTE :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "INTERVAL HOUR(%d) TO MINUTE",
                             sDatetimeIntervalPrecision );
                break;
            case SQL_INTERVAL_HOUR_TO_SECOND :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "INTERVAL HOUR(%d) TO SECOND(%d)",
                             sDatetimeIntervalPrecision,
                             sPrecision );
                break;
            case SQL_INTERVAL_MINUTE_TO_SECOND :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "INTERVAL MINUTE(%d) TO SECOND(%d)",
                             sDatetimeIntervalPrecision,
                             sPrecision );
                break;
            case SQL_BOOLEAN :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             sTypeName );
                break;
            case SQL_ROWID:
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             sTypeName );
                break;
            default :
                stlSnprintf( sDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             sTypeName );
                break;
        }

        /**
         * Nullable 정보 설정
         */

        if ( sNullable == SQL_NO_NULLS )
        {
            stlSnprintf( sNullableStr, 
                         STL_MAX_SQL_IDENTIFIER_LENGTH,
                         "FALSE" );
        }
        else
        {
            stlSnprintf( sNullableStr, 
                         STL_MAX_SQL_IDENTIFIER_LENGTH,
                         "TRUE" );
        }

        /**
         * 출력 Format 조정
         */

        sTargetColumns[0].mIndicator = stlStrlen( sColumnName );
        sTargetColumns[1].mIndicator = stlStrlen( sDataTypeStr );
        sTargetColumns[2].mIndicator = stlStrlen( sNullableStr );
        
        STL_TRY( ztqMakeRowString( sRowBuffer,
                                   sLineFetchCount,
                                   sTargetColumns,
                                   STL_TRUE,
                                   aErrorStack )
                 == STL_SUCCESS );
        sLineFetchCount += 1;
        sRowCount += 1;
        
        if( sLineFetchCount == gZtqPageSize )
        {
            /**
             * Line Buffer 가 꽉 찬 경우
             */
            
            for( i = 0; i < sLineFetchCount; i++ )
            {
                if( i == 0 )
                {
                    STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                                  sTargetColumns )
                             == STL_SUCCESS );
                }
                
                STL_TRY( ztqPrintRowString( sRowBuffer,
                                            sTargetColumns,
                                            i,
                                            aAllocator,
                                            aErrorStack )
                         == STL_SUCCESS );
            }

            /**
             * Page에 결과가 한건이라도 있다면 현재 라인과 다음 라인을 분리함.
             */
            
            if( sLineFetchCount > 0 )
            {
                ztqPrintf( "\n" );
            }

            ztqSetInitRowStringLen( sRowBuffer, sTargetColumns );
            sLineFetchCount = 0;
        }
    }

    /**
     * 결과 출력 
     */
    
    for( i = 0; i < sLineFetchCount; i++ )
    {
        if( i == 0 )
        {
            STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                          sTargetColumns )
                     == STL_SUCCESS );
        }
        
        STL_TRY( ztqPrintRowString( sRowBuffer,
                                    sTargetColumns,
                                    i,
                                    aAllocator,
                                    aErrorStack )
                 == STL_SUCCESS );
    }
    
    /**
     * Page에 결과가 한건이라도 있다면 현재 라인과 다음 라인을 분리함.
     */
    
    if( sLineFetchCount > 0 )
    {
        ztqPrintf( "\n" );
    }
    
    ztqSetInitRowStringLen( sRowBuffer, sTargetColumns );
    sLineFetchCount = 0;
    
    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TABLE_NOT_EXIST )
    {
        /**
         * Server Error 를 그대로 보여줌
         */
        
        (void)ztqPrintError( sStmtHandle,
                             SQL_HANDLE_STMT,
                             aErrorStack );
        
        /* sNativeError = 0; */
        /* (void) ztqSQLGetDiagRec( SQL_HANDLE_STMT, */
        /*                          sStmtHandle, */
        /*                          1, */
        /*                          NULL, */
        /*                          &sNativeError, */
        /*                          NULL, */
        /*                          0, */
        /*                          NULL, */
        /*                          & sResult, */
        /*                          aErrorStack ); */

        /* if ( sNativeError == 16245 ) */
        /* { */
        /*     /\** */
        /*      * 16245 is QLL_ERRCODE_VIEW_HAS_ERRORS */
        /*      *\/ */

        /*     (void) ztqSQLGetDiagRec( SQL_HANDLE_STMT, */
        /*                              sStmtHandle, */
        /*                              2, */
        /*                              NULL, */
        /*                              &sNativeError, */
        /*                              NULL, */
        /*                              0, */
        /*                              NULL, */
        /*                              & sResult, */
        /*                              aErrorStack ); */

        /*     if ( sNativeError == 16254 ) */
        /*     { */
        /*         /\** */
        /*          * 16254 is QLL_ERRCODE_LACKS_PRIVILEGES_ON_TABLE */
        /*          *\/ */
                
        /*         stlPushError( STL_ERROR_LEVEL_ABORT, */
        /*                       ZTQ_ERRCODE_INVALID_TABLE_NAME, */
        /*                       NULL, */
        /*                       aErrorStack, */
        /*                       aInTableName ); */
        /*     } */
        /*     else */
        /*     { */
        /*         stlPushError( STL_ERROR_LEVEL_ABORT, */
        /*                       ZTQ_ERRCODE_INVALID_OBJECT_DESCRIBE, */
        /*                       NULL, */
        /*                       aErrorStack, */
        /*                       aInTableName ); */
        /*     } */
        /* } */
        /* else */
        /* { */
        /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
        /*                   ZTQ_ERRCODE_INVALID_TABLE_NAME, */
        /*                   NULL, */
        /*                   aErrorStack, */
        /*                   aInTableName ); */
        /* } */
    }

    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqGetAccessableIndex( stlChar       * aInSchemaName,
                                 stlChar       * aInIndexName,
                                 stlChar       * aOutSchemaName,
                                 stlChar       * aOutIndexName,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    stlInt32      sState = 0;
    SQLRETURN     sResult;
    
    SQLHSTMT      sStmtHandle;
    stlChar       sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    stlChar       sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar       sIndexName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    stlChar       sInSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar       sInIndexName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlInt32      sLen;

    stlMemset( sInSchemaName, 0x00, STL_MAX_SQL_IDENTIFIER_LENGTH );
    stlMemset( sInIndexName, 0x00, STL_MAX_SQL_IDENTIFIER_LENGTH );
    
    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    /**
     * 해당 Table Name 으로 결과가 없는 SELECT 구문을 수행
     */

    STL_TRY( ztqSQLBindParameter( sStmtHandle,
                                  1,
                                  SQL_PARAM_OUTPUT,
                                  SQL_C_CHAR,
                                  SQL_VARCHAR,
                                  STL_MAX_SQL_IDENTIFIER_LENGTH, 
                                  0, 
                                  sSchemaName,
                                  STL_MAX_SQL_IDENTIFIER_LENGTH,
                                  NULL,
                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindParameter( sStmtHandle,
                                  2,
                                  SQL_PARAM_OUTPUT,
                                  SQL_C_CHAR,
                                  SQL_VARCHAR,
                                  STL_MAX_SQL_IDENTIFIER_LENGTH, 
                                  0, 
                                  sIndexName,
                                  STL_MAX_SQL_IDENTIFIER_LENGTH,
                                  NULL,
                                  aErrorStack )
             == STL_SUCCESS );

    /**
     * @todo
     * User Default Temporary TABLESPACE 가 DROP 될 경우,
     * Instant Table 을 생성할 수 없어, 아래 Query 들이 Error 가 발생할 수 있다.
     */
    
    if ( aInSchemaName == NULL )
    {
        if( aInIndexName[0] == '"' )
        {
            sLen = stlStrlen( aInIndexName );
            stlMemcpy( sInIndexName, aInIndexName + 1, sLen - 2 );
            sInIndexName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sInIndexName, aInIndexName );
        }
        
        /**
         * Search Order 를 통해 첫번째 사용 가능한 Index 의 정보를 얻는다.
         */
        
        stlSnprintf( sSqlString,
                     ZTQ_MAX_COMMAND_BUFFER_SIZE,
                     "SELECT sch.SCHEMA_NAME, "
                     "       idx.INDEX_NAME "
                     "  INTO ?, "
                     "       ? "
                     "  FROM "
                     "       DEFINITION_SCHEMA.INDEXES          AS idx "
                     "     , DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE AS ikey "
                     "     , DEFINITION_SCHEMA.SCHEMATA         AS sch "
                     "     , DEFINITION_SCHEMA.TABLES           AS tab "
                     "     , ( "
                     "         ( SELECT "
                     "                sch1.SCHEMA_ID  "
                     "              , path1.SEARCH_ORDER  "
                     "           FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH AS path1 "
                     "              , DEFINITION_SCHEMA.SCHEMATA         AS sch1 "
                     "              , DEFINITION_SCHEMA.AUTHORIZATIONS   AS usr1 "
                     "          WHERE "
                     "                usr1.AUTHORIZATION_NAME = CURRENT_USER "
                     "            AND path1.SCHEMA_ID         = sch1.SCHEMA_ID "
                     "            AND path1.AUTH_ID           = usr1.AUTH_ID "
                     "          ORDER BY "
                     "                path1.SEARCH_ORDER "
                     "         ) "
                     "       UNION ALL "
                     "         ( SELECT "
                     "                sch2.SCHEMA_ID "
                     "              , ( SELECT COUNT(*) "
                     "                    FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH "
                     "                   WHERE AUTH_ID = USER_ID() ) "
                     "                + path2.SEARCH_ORDER "
                     "           FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH AS path2 "
                     "              , DEFINITION_SCHEMA.SCHEMATA         AS sch2 " 
                     "              , DEFINITION_SCHEMA.AUTHORIZATIONS   AS usr2 "
                     "          WHERE "
                     "                usr2.AUTHORIZATION_NAME = 'PUBLIC' "
                     "            AND path2.SCHEMA_ID         = sch2.SCHEMA_ID "
                     "            AND path2.AUTH_ID           = usr2.AUTH_ID "
                     "          ORDER BY "
                     "                path2.SEARCH_ORDER "
                     "         ) "
                     "       ) AS upath ( SCHEMA_ID, SEARCH_ORDER ) "
                     " WHERE idx.INDEX_NAME = '%s' "
                     "   AND idx.INDEX_ID    = ikey.INDEX_ID "
                     "   AND ikey.TABLE_ID   = tab.TABLE_ID "
                     "   AND idx.SCHEMA_ID   = sch.SCHEMA_ID "
                     "   AND sch.SCHEMA_ID   = upath.SCHEMA_ID "
                     "   AND ( tab.TABLE_ID IN ( SELECT pvcol.TABLE_ID "
                     "                             FROM DEFINITION_SCHEMA.COLUMN_PRIVILEGES AS pvcol "
                     "                            WHERE ( pvcol.GRANTEE_ID IN ( SELECT AUTH_ID "
                     "                                                           FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS aucol "
                     "                                                          WHERE aucol.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  "
                     "                                                        )  "
                     "                                -- OR  \n"
                     "                                -- pvcol.GRANTEE_ID IN ( SELECT AUTH_ID \n"
                     "                                --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  \n"
                     "                                   ) "
                     "                         ) "
                     "         OR "
                     "         tab.TABLE_ID IN ( SELECT pvtab.TABLE_ID "
                     "                             FROM DEFINITION_SCHEMA.TABLE_PRIVILEGES AS pvtab "
                     "                            WHERE ( pvtab.GRANTEE_ID IN ( SELECT AUTH_ID "
                     "                                                            FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS autab "
                     "                                                           WHERE autab.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  "
                     "                                                        ) "
                     "                                 -- OR  \n"
                     "                                 -- pvtab.GRANTEE_ID IN ( SELECT AUTH_ID \n"
                     "                                 --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  \n"
                     "                                  ) "
                     "                          ) "
                     "         OR "
                     "         tab.SCHEMA_ID IN ( SELECT pvsch.SCHEMA_ID "
                     "                              FROM DEFINITION_SCHEMA.SCHEMA_PRIVILEGES AS pvsch "
                     "                             WHERE pvsch.PRIVILEGE_TYPE IN ( 'CONTROL SCHEMA', 'ALTER TABLE', 'DROP TABLE', "
                     "                                                             'SELECT TABLE', 'INSERT TABLE', 'DELETE TABLE', 'UPDATE TABLE', 'LOCK TABLE' ) "
                     "                               AND ( pvsch.GRANTEE_ID IN ( SELECT AUTH_ID "
                     "                                                             FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS ausch "
                     "                                                            WHERE ausch.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  "
                     "                                                         ) "
                     "                                  -- OR  \n"
                     "                                  -- pvsch.GRANTEE_ID IN ( SELECT AUTH_ID \n"
                     "                                  --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  \n"
                     "                                   ) "
                     "                          ) "
                     "         OR "
                     "         EXISTS ( SELECT GRANTEE_ID  "
                     "                    FROM DEFINITION_SCHEMA.DATABASE_PRIVILEGES pvdba "
                     "                   WHERE pvdba.PRIVILEGE_TYPE IN ( 'ALTER ANY TABLE', 'DROP ANY TABLE', "
                     "                                                   'SELECT ANY TABLE', 'INSERT ANY TABLE', 'DELETE ANY TABLE', 'UPDATE ANY TABLE', 'LOCK ANY TABLE' ) "
                     "                     AND ( pvdba.GRANTEE_ID IN ( SELECT AUTH_ID "
                     "                                                   FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS audba "
                     "                                                  WHERE audba.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  "
                     "                                                ) "
                     "                        -- OR  \n"
                     "                        -- pvdba.GRANTEE_ID IN ( SELECT AUTH_ID \n"
                     "                        --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  \n"
                     "                         )  "
                     "                ) "
                     "       ) "
                     " ORDER BY "
                     "       upath.SEARCH_ORDER "
                     "  FETCH 1 ",
                     sInIndexName );
    }
    else
    {
        /**
         * Schema Name 과 Index Name 으로 access 가능한 Index Name 을 얻는다.
         */
        
        if( aInSchemaName[0] == '"' )
        {
            sLen = stlStrlen( aInSchemaName );
            stlMemcpy( sInSchemaName, aInSchemaName + 1, sLen - 2 );
            sInSchemaName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sInSchemaName, aInSchemaName );
        }

        if( aInIndexName[0] == '"' )
        {
            sLen = stlStrlen( aInIndexName );
            stlMemcpy( sInIndexName, aInIndexName + 1, sLen - 2 );
            sInIndexName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sInIndexName, aInIndexName );
        }

        stlSnprintf( sSqlString,
                     ZTQ_MAX_COMMAND_BUFFER_SIZE,
                     "SELECT sch.SCHEMA_NAME, "
                     "       idx.INDEX_NAME "
                     "  INTO ?, "
                     "       ? "
                     "  FROM "
                     "       DEFINITION_SCHEMA.INDEXES          AS idx "
                     "     , DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE AS ikey "
                     "     , DEFINITION_SCHEMA.SCHEMATA         AS sch "
                     "     , DEFINITION_SCHEMA.TABLES           AS tab "
                     " WHERE idx.INDEX_NAME  = '%s' "
                     "   AND sch.SCHEMA_NAME = '%s' "
                     "   AND idx.INDEX_ID    = ikey.INDEX_ID "
                     "   AND ikey.TABLE_ID   = tab.TABLE_ID "
                     "   AND idx.SCHEMA_ID   = sch.SCHEMA_ID "
                     "   AND ( tab.TABLE_ID IN ( SELECT pvcol.TABLE_ID "
                     "                             FROM DEFINITION_SCHEMA.COLUMN_PRIVILEGES AS pvcol "
                     "                            WHERE ( pvcol.GRANTEE_ID IN ( SELECT AUTH_ID "
                     "                                                           FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS aucol "
                     "                                                          WHERE aucol.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  "
                     "                                                        )  "
                     "                                -- OR  \n"
                     "                                -- pvcol.GRANTEE_ID IN ( SELECT AUTH_ID \n"
                     "                                --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  \n"
                     "                                   ) "
                     "                         ) "
                     "         OR "
                     "         tab.TABLE_ID IN ( SELECT pvtab.TABLE_ID "
                     "                             FROM DEFINITION_SCHEMA.TABLE_PRIVILEGES AS pvtab "
                     "                            WHERE ( pvtab.GRANTEE_ID IN ( SELECT AUTH_ID "
                     "                                                            FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS autab "
                     "                                                           WHERE autab.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  "
                     "                                                        ) "
                     "                                 -- OR  \n"
                     "                                 -- pvtab.GRANTEE_ID IN ( SELECT AUTH_ID \n"
                     "                                 --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  \n"
                     "                                  ) "
                     "                          ) "
                     "         OR "
                     "         tab.SCHEMA_ID IN ( SELECT pvsch.SCHEMA_ID "
                     "                              FROM DEFINITION_SCHEMA.SCHEMA_PRIVILEGES AS pvsch "
                     "                             WHERE pvsch.PRIVILEGE_TYPE IN ( 'CONTROL SCHEMA', 'ALTER TABLE', 'DROP TABLE', "
                     "                                                             'SELECT TABLE', 'INSERT TABLE', 'DELETE TABLE', 'UPDATE TABLE', 'LOCK TABLE' ) "
                     "                               AND ( pvsch.GRANTEE_ID IN ( SELECT AUTH_ID "
                     "                                                             FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS ausch "
                     "                                                            WHERE ausch.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  "
                     "                                                         ) "
                     "                                  -- OR  \n"
                     "                                  -- pvsch.GRANTEE_ID IN ( SELECT AUTH_ID \n"
                     "                                  --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  \n"
                     "                                   ) "
                     "                          ) "
                     "         OR "
                     "         EXISTS ( SELECT GRANTEE_ID  "
                     "                    FROM DEFINITION_SCHEMA.DATABASE_PRIVILEGES pvdba "
                     "                   WHERE pvdba.PRIVILEGE_TYPE IN ( 'ALTER ANY TABLE', 'DROP ANY TABLE', "
                     "                                                   'SELECT ANY TABLE', 'INSERT ANY TABLE', 'DELETE ANY TABLE', 'UPDATE ANY TABLE', 'LOCK ANY TABLE' ) "
                     "                     AND ( pvdba.GRANTEE_ID IN ( SELECT AUTH_ID "
                     "                                                   FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS audba "
                     "                                                  WHERE audba.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  "
                     "                                                ) "
                     "                        -- OR  \n"
                     "                        -- pvdba.GRANTEE_ID IN ( SELECT AUTH_ID \n"
                     "                        --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  \n"
                     "                         )  "
                     "                ) "
                     "       ) ",
                     sInIndexName,
                     sInSchemaName );
    }
    
    STL_TRY( ztqDescExecDirect( sStmtHandle,
                                (SQLCHAR*)sSqlString,
                                SQL_NTS,
                                & sResult,
                                aAllocator,
                                aErrorStack )
             == STL_SUCCESS );

    switch ( sResult )
    {
        case SQL_SUCCESS:
        case SQL_SUCCESS_WITH_INFO:
            /* index 가 존재함 */
            break;
        case SQL_NO_DATA:
            /* index 가 존재하지 않음 */
            STL_THROW( RAMP_ERR_INDEX_NOT_EXIST );
            break;
        default:
            /* server error 를 그대로 보여줌 */
            STL_THROW( RAMP_ERR_SERVER_ERROR );
            break;
    }

    stlStrncpy( aOutSchemaName, sSchemaName, STL_MAX_SQL_IDENTIFIER_LENGTH );
    stlStrncpy( aOutIndexName, sIndexName, STL_MAX_SQL_IDENTIFIER_LENGTH );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INDEX_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_INDEX_NAME,
                      NULL,
                      aErrorStack,
                      sInIndexName );
    }

    STL_CATCH( RAMP_ERR_SERVER_ERROR )
    {
        /**
         * Server Error 를 그대로 보여줌
         */
        
        (void)ztqPrintError( sStmtHandle,
                             SQL_HANDLE_STMT,
                             aErrorStack );
    }
    
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintIndexDesc( stlInt64        aIndexId,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack )
{
    SQLHSTMT          sStmtHandle;
    SQLINTEGER        i;
    stlInt32          sState = 0;
    stlChar           sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    SQLLEN            sIndicator;
    SQLRETURN         sResult;
    stlChar           sColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar           sOrdinalPositionStr[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar           sAscendingOrderStr[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar           sNullsFirstStr[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlInt64          sColumnId;
    ztqRowBuffer    * sRowBuffer;
    ztqTargetColumn   sTargetColumns[4];
    stlInt64          sRowCount = 0;          
    stlInt64          sLineFetchCount = 0;
    stlBool           sIsSuccess;
    
    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    /**
     * SQL 생성
     * ORDER BY 를 사용할 경우, user default TEMPORARY TABLE 가 없으면 구문 에러가 발생함.
     */
    
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "    /*+ INDEX( DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE, INDEX_KEY_COLUMN_USAGE_UNIQUE_INDEX_SCHEMA_ID_INDEX_ID_ORDINAL_POSITION_INDEX ) */ "
                 "       COLUMN_ID, ORDINAL_POSITION, IS_ASCENDING_ORDER, IS_NULLS_FIRST "
                 "  FROM DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE "
                 " WHERE INDEX_ID = %ld ",
                 aIndexId );
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*)sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            ZTQ_C_SBIGINT,
                            &sColumnId,
                            STL_SIZEOF( stlInt64 ),
                            &sIndicator,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sOrdinalPositionStr,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            &sTargetColumns[1].mIndicator,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sAscendingOrderStr,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            &sTargetColumns[2].mIndicator,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            sNullsFirstStr,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            &sTargetColumns[3].mIndicator,
                            aErrorStack )
             == STL_SUCCESS );

    /*
     * 출력을 위한 대상 컬럼을 구성한다
     * 
     * COLUMN_NAME ORDINAL_POSITION IS_ASCENDING_ORDER IS_NULLS_FIRST
     * ----------- ---------------- ------------------ --------------
     *     ...            ...                ...             ...
     */
    stlStrcpy( sTargetColumns[0].mName, "COLUMN_NAME" );
    sTargetColumns[0].mDataType = SQL_CHAR;
    sTargetColumns[0].mDataSize = STL_MAX_SQL_IDENTIFIER_LENGTH;
    sTargetColumns[0].mDataValue = sColumnName;
    stlStrcpy( sTargetColumns[0].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_STRING );
    sTargetColumns[0].mDataTypeGroup = ZTQ_GROUP_STRING;
    
    stlStrcpy( sTargetColumns[1].mName, "ORDINAL_POSITION" );
    sTargetColumns[1].mDataType = SQL_CHAR;
    sTargetColumns[1].mDataSize = STL_MAX_SQL_IDENTIFIER_LENGTH;
    sTargetColumns[1].mDataValue = sOrdinalPositionStr;
    stlStrcpy( sTargetColumns[1].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_NUMERIC );
    sTargetColumns[1].mDataTypeGroup = ZTQ_GROUP_NUMERIC;
    
    stlStrcpy( sTargetColumns[2].mName, "IS_ASCENDING_ORDER" );
    sTargetColumns[2].mDataType = SQL_CHAR;
    sTargetColumns[2].mDataSize = STL_MAX_SQL_IDENTIFIER_LENGTH;
    sTargetColumns[2].mDataValue = sAscendingOrderStr;
    stlStrcpy( sTargetColumns[2].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_STRING );
    sTargetColumns[2].mDataTypeGroup = ZTQ_GROUP_STRING;
    
    stlStrcpy( sTargetColumns[3].mName, "IS_NULLS_FIRST" );
    sTargetColumns[3].mDataType = SQL_CHAR;
    sTargetColumns[3].mDataSize = STL_MAX_SQL_IDENTIFIER_LENGTH;
    sTargetColumns[3].mDataValue = sNullsFirstStr;
    stlStrcpy( sTargetColumns[3].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_STRING );
    sTargetColumns[3].mDataTypeGroup = ZTQ_GROUP_STRING;
    
    STL_TRY( ztqAllocRowBuffer( &sRowBuffer,
                                4,
                                gZtqPageSize,
                                sTargetColumns,
                                aAllocator,
                                aErrorStack )
             == STL_SUCCESS );
    
    while( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqGetColumnName( sColumnId,
                                   sColumnName,
                                   &sIsSuccess,
                                   aAllocator,
                                   aErrorStack )
                 == STL_SUCCESS );
        sTargetColumns[0].mIndicator = stlStrlen( sColumnName );
        
        if( sResult != SQL_NO_DATA )
        {
            STL_TRY( ztqMakeRowString( sRowBuffer,
                                       sLineFetchCount,
                                       sTargetColumns,
                                       STL_TRUE,
                                       aErrorStack )
                     == STL_SUCCESS );
            sLineFetchCount += 1;
            sRowCount += 1;
        }
        
        if( (sLineFetchCount == gZtqPageSize) || (sResult == SQL_NO_DATA ) )
        {
            /*
             * Line Buffer 가 꽉 차거나 종료 시점인 경우
             */
            for( i = 0; i < sLineFetchCount; i++ )
            {
                if( i == 0 )
                {
                    STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                                  sTargetColumns )
                             == STL_SUCCESS );
                }
                
                STL_TRY( ztqPrintRowString( sRowBuffer,
                                            sTargetColumns,
                                            i,
                                            aAllocator,
                                            aErrorStack )
                         == STL_SUCCESS );
            }

            /*
             * Page에 결과가 한건이라도 있고 Fetch가 종료되지 않았다면
             * 현재 라인과 다음 라인을 분리함.
             */
            if( (sLineFetchCount > 0) && (sResult != SQL_NO_DATA ) )
            {
                ztqPrintf( "\n" );
            }

            ztqSetInitRowStringLen( sRowBuffer, sTargetColumns );
            sLineFetchCount = 0;
        }

        if( sResult == SQL_NO_DATA )
        {
            break;
        }
    }

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztqPrintIndexInfo( stlInt64        aSchemaId,
                             stlInt64        aTableId,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack )
{
    stlInt64        * sIndexIds;
    stlInt32          sIndexCount;
    stlChar           sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    stlInt32          i;
    stlInt32          j;
    stlBool           sIsSuccess;
    ztqRowBuffer    * sRowBuffer;
    ztqTargetColumn   sTargetColumns[5];
    stlInt64          sRowCount = 0;          
    stlInt64          sLineFetchCount = 0;
    stlChar           sTablespaceName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar           sIndexName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar           sIndexTypeStr[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar           sIsUniqueStr[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar           sColumnNameList[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    /*
     * Test Case Diff 를 방지하기 위해 ordering 한다.
     */
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "    /*+ INDEX( DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE, INDEX_KEY_TABLE_USAGE_PRIMARY_KEY_INDEX ) */ "
                 "    INDEX_ID "
                 " FROM DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE "
                 "WHERE TABLE_SCHEMA_ID = %ld AND TABLE_ID = %ld",
                 aSchemaId,
                 aTableId );    
    
    STL_TRY( ztqGetObjectIds( sSqlString,
                              &sIndexIds,
                              &sIndexCount,
                              &sIsSuccess,
                              aAllocator,
                              aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sIsSuccess == STL_TRUE, RAMP_SUCCESS );

    /**
     * 출력을 위한 대상 컬럼을 구성한다
     * 
     * INDEX_NAME  TABLESPACE_NAME INDEX_TYPE IS_UNIQUE COLUMNS
     * ----------- --------------- ---------- --------- -------
     *    ...            ...           ...       ...      ...
     */
    stlStrcpy( sTargetColumns[0].mName, "INDEX_NAME" );
    sTargetColumns[0].mDataType = SQL_CHAR;
    sTargetColumns[0].mDataSize = STL_MAX_SQL_IDENTIFIER_LENGTH;
    sTargetColumns[0].mDataValue = sIndexName;
    stlStrcpy( sTargetColumns[0].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_STRING );
    sTargetColumns[0].mDataTypeGroup = ZTQ_GROUP_STRING;
    
    stlStrcpy( sTargetColumns[1].mName, "TABLESPACE_NAME" );
    sTargetColumns[1].mDataType = SQL_CHAR;
    sTargetColumns[1].mDataSize = STL_MAX_SQL_IDENTIFIER_LENGTH;
    sTargetColumns[1].mDataValue = sTablespaceName;
    stlStrcpy( sTargetColumns[1].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_STRING );
    sTargetColumns[1].mDataTypeGroup = ZTQ_GROUP_STRING;
    
    stlStrcpy( sTargetColumns[2].mName, "INDEX_TYPE" );
    sTargetColumns[2].mDataType = SQL_CHAR;
    sTargetColumns[2].mDataSize = STL_MAX_SQL_IDENTIFIER_LENGTH;
    sTargetColumns[2].mDataValue = sIndexTypeStr;
    stlStrcpy( sTargetColumns[2].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_STRING );
    sTargetColumns[2].mDataTypeGroup = ZTQ_GROUP_STRING;
    
    stlStrcpy( sTargetColumns[3].mName, "IS_UNIQUE" );
    sTargetColumns[3].mDataType = SQL_CHAR;
    sTargetColumns[3].mDataSize = STL_MAX_SQL_IDENTIFIER_LENGTH;
    sTargetColumns[3].mDataValue = sIsUniqueStr;
    stlStrcpy( sTargetColumns[3].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_STRING );
    sTargetColumns[3].mDataTypeGroup = ZTQ_GROUP_STRING;
    
    stlStrcpy( sTargetColumns[4].mName, "COLUMNS" );
    sTargetColumns[4].mDataType = SQL_CHAR;
    sTargetColumns[4].mDataSize = ZTQ_MAX_COMMAND_BUFFER_SIZE;
    sTargetColumns[4].mDataValue = sColumnNameList;
    stlStrcpy( sTargetColumns[4].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_STRING );
    sTargetColumns[4].mDataTypeGroup = ZTQ_GROUP_STRING;
    
    STL_TRY( ztqAllocRowBuffer( &sRowBuffer,
                                5,
                                gZtqPageSize,
                                sTargetColumns,
                                aAllocator,
                                aErrorStack )
             == STL_SUCCESS );
    
    for( i = 0; i < sIndexCount; i++ )
    {
        STL_TRY( ztqGetIndexInfo( sIndexIds[i],
                                  sIndexName,
                                  sTablespaceName,
                                  sIndexTypeStr,
                                  sIsUniqueStr,
                                  &sIsSuccess,
                                  aAllocator,
                                  aErrorStack )
                 == STL_SUCCESS );
        sTargetColumns[0].mIndicator = stlStrlen( sIndexName );
        sTargetColumns[1].mIndicator = stlStrlen( sTablespaceName );
        sTargetColumns[2].mIndicator = stlStrlen( sIndexTypeStr );
        sTargetColumns[3].mIndicator = stlStrlen( sIsUniqueStr );
        
        STL_TRY( ztqGetIndexColumnNameList( sIndexIds[i],
                                            sColumnNameList,
                                            aAllocator,
                                            aErrorStack )
                 == STL_SUCCESS );
        sTargetColumns[4].mIndicator = stlStrlen( sColumnNameList );
        
        STL_TRY( ztqMakeRowString( sRowBuffer,
                                   sLineFetchCount,
                                   sTargetColumns,
                                   STL_TRUE,
                                   aErrorStack )
                 == STL_SUCCESS );
        sLineFetchCount += 1;
        sRowCount += 1;
            
        if( sLineFetchCount == gZtqPageSize )
        {
            /*
             * Line Buffer 가 꽉 차거나 종료 시점인 경우
             */
            for( j = 0; j < sLineFetchCount; j++ )
            {
                if( j == 0 )
                {
                    STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                                  sTargetColumns )
                             == STL_SUCCESS );
                }
                
                STL_TRY( ztqPrintRowString( sRowBuffer,
                                            sTargetColumns,
                                            j,
                                            aAllocator,
                                            aErrorStack )
                         == STL_SUCCESS );
            }

            /*
             * Page에 결과가 한건이라도 있다면 현재 라인과 다음 라인을 분리함.
             */
            if( sLineFetchCount > 0 )
            {
                ztqPrintf( "\n" );
            }

            ztqSetInitRowStringLen( sRowBuffer, sTargetColumns );
            sLineFetchCount = 0;
        }
    }

    if( sLineFetchCount > 0 )
    {
        /*
         * Line Buffer 가 꽉 차거나 종료 시점인 경우
         */
        for( i = 0; i < sLineFetchCount; i++ )
        {
            if( i == 0 )
            {
                STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                              sTargetColumns )
                         == STL_SUCCESS );
            }
                
            STL_TRY( ztqPrintRowString( sRowBuffer,
                                        sTargetColumns,
                                        i,
                                        aAllocator,
                                        aErrorStack )
                     == STL_SUCCESS );
        }

        if( sLineFetchCount > 0 )
        {
            ztqPrintf( "\n" );
        }
    }
    
    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



stlStatus ztqPrintConstraintInfo( stlInt64        aSchemaId,
                                  stlInt64        aTableId,
                                  stlAllocator  * aAllocator,
                                  stlErrorStack * aErrorStack )
{
    stlInt64        * sConstIds;
    stlInt32          sConstCount;
    stlChar           sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    stlInt32          i;
    stlInt32          j;
    stlBool           sIsSuccess;
    ztqRowBuffer    * sRowBuffer;
    ztqTargetColumn   sTargetColumns[5];
    stlInt64          sRowCount = 0;          
    stlInt64          sLineFetchCount = 0;
    stlChar           sConstName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar           sConstTypeStr[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar           sAssociatedIndexName[STL_MAX_SQL_IDENTIFIER_LENGTH * 2];
    stlChar           sColumnNameList[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    stlBool           sIsKeyConst;

    /*
     * Test Case DIFF 를 방지하기 위해서 Index Hint 를 이용해 Ordering 함
     */
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "    /*+ INDEX( DEFINITION_SCHEMA.TABLE_CONSTRAINTS, TABLE_CONSTRAINTS_PRIMARY_KEY_INDEX ) */ "
                 "    CONSTRAINT_ID "
                 " FROM DEFINITION_SCHEMA.TABLE_CONSTRAINTS "
                 "WHERE TABLE_SCHEMA_ID = %ld AND TABLE_ID = %ld",
                 aSchemaId,
                 aTableId );
    
    STL_TRY( ztqGetObjectIds( sSqlString,
                              &sConstIds,
                              &sConstCount,
                              &sIsSuccess,
                              aAllocator,
                              aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sIsSuccess == STL_TRUE, RAMP_SUCCESS );

    /*
     * 출력을 위한 대상 컬럼을 구성한다
     * 
     * CONSTRAINT_NAME CONSTRAINT_TYPE ASSOCIATED_INDEX COLUMNS
     * --------------- --------------- ---------------- -------
     *    ...               ...              ...          ...
     */
    stlStrcpy( sTargetColumns[0].mName, "CONSTRAINT_NAME" );
    sTargetColumns[0].mDataType = SQL_CHAR;
    sTargetColumns[0].mDataSize = STL_MAX_SQL_IDENTIFIER_LENGTH;
    sTargetColumns[0].mDataValue = sConstName;
    stlStrcpy( sTargetColumns[0].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_STRING );
    sTargetColumns[0].mDataTypeGroup = ZTQ_GROUP_STRING;
    
    stlStrcpy( sTargetColumns[1].mName, "CONSTRAINT_TYPE" );
    sTargetColumns[1].mDataType = SQL_CHAR;
    sTargetColumns[1].mDataSize = STL_MAX_SQL_IDENTIFIER_LENGTH;
    sTargetColumns[1].mDataValue = sConstTypeStr;
    stlStrcpy( sTargetColumns[1].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_STRING );
    sTargetColumns[1].mDataTypeGroup = ZTQ_GROUP_STRING;
    
    stlStrcpy( sTargetColumns[2].mName, "ASSOCIATED_INDEX" );
    sTargetColumns[2].mDataType = SQL_CHAR;
    sTargetColumns[2].mDataSize = STL_MAX_SQL_IDENTIFIER_LENGTH * 2;
    sTargetColumns[2].mDataValue = sAssociatedIndexName;
    stlStrcpy( sTargetColumns[2].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_STRING );
    sTargetColumns[2].mDataTypeGroup = ZTQ_GROUP_STRING;
    
    stlStrcpy( sTargetColumns[3].mName, "COLUMNS" );
    sTargetColumns[3].mDataType = SQL_CHAR;
    sTargetColumns[3].mDataSize = ZTQ_MAX_COMMAND_BUFFER_SIZE;
    sTargetColumns[3].mDataValue = sColumnNameList;
    stlStrcpy( sTargetColumns[3].mColumnFormat,
               ZTQ_COLUMN_FORMAT_CLASS_STRING );
    sTargetColumns[3].mDataTypeGroup = ZTQ_GROUP_STRING;
    
    STL_TRY( ztqAllocRowBuffer( &sRowBuffer,
                                4,
                                gZtqPageSize,
                                sTargetColumns,
                                aAllocator,
                                aErrorStack )
             == STL_SUCCESS );
    
    for( i = 0; i < sConstCount; i++ )
    {
        STL_TRY( ztqGetConstraintInfo( sConstIds[i],
                                       sConstName,
                                       sConstTypeStr,
                                       & sIsKeyConst,
                                       sAssociatedIndexName,
                                       &sIsSuccess,
                                       aAllocator,
                                       aErrorStack )
                 == STL_SUCCESS );
        sTargetColumns[0].mIndicator = stlStrlen( sConstName );
        sTargetColumns[1].mIndicator = stlStrlen( sConstTypeStr );
        sTargetColumns[2].mIndicator = stlStrlen( sAssociatedIndexName );

        if ( sIsKeyConst == STL_TRUE )
        {
            STL_TRY( ztqGetKeyConstColumnNameList( sConstIds[i],
                                                   sColumnNameList,
                                                   aAllocator,
                                                   aErrorStack )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( ztqGetCheckConstColumnNameList( sConstIds[i],
                                                     sColumnNameList,
                                                     aAllocator,
                                                     aErrorStack )
                     == STL_SUCCESS );
        }

        sTargetColumns[3].mIndicator = stlStrlen( sColumnNameList );
        
        STL_TRY( ztqMakeRowString( sRowBuffer,
                                   sLineFetchCount,
                                   sTargetColumns,
                                   STL_TRUE,
                                   aErrorStack )
                 == STL_SUCCESS );
        sLineFetchCount += 1;
        sRowCount += 1;
            
        if( sLineFetchCount == gZtqPageSize )
        {
            /*
             * Line Buffer 가 꽉 차거나 종료 시점인 경우
             */
            for( j = 0; j < sLineFetchCount; j++ )
            {
                if( j == 0 )
                {
                    STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                                  sTargetColumns )
                             == STL_SUCCESS );
                }
                
                STL_TRY( ztqPrintRowString( sRowBuffer,
                                            sTargetColumns,
                                            j,
                                            aAllocator,
                                            aErrorStack )
                         == STL_SUCCESS );
            }

            /*
             * Page에 결과가 한건이라도 있다면 현재 라인과 다음 라인을 분리함.
             */
            if( sLineFetchCount > 0 )
            {
                ztqPrintf( "\n" );
            }

            ztqSetInitRowStringLen( sRowBuffer, sTargetColumns );
            sLineFetchCount = 0;
        }
    }

    if( sLineFetchCount > 0 )
    {
        /*
         * Line Buffer 가 꽉 차거나 종료 시점인 경우
         */
        for( i = 0; i < sLineFetchCount; i++ )
        {
            if( i == 0 )
            {
                STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                              sTargetColumns )
                         == STL_SUCCESS );
            }
                
            STL_TRY( ztqPrintRowString( sRowBuffer,
                                        sTargetColumns,
                                        i,
                                        aAllocator,
                                        aErrorStack )
                     == STL_SUCCESS );
        }
    }
    
    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



stlStatus ztqGetConstraintInfo( stlInt64        aConstId,
                                stlChar       * aConstName,
                                stlChar       * aConstTypeStr,
                                stlBool       * aIsKeyConst,
                                stlChar       * aAssociatedIndexName,
                                stlBool       * aIsSuccess,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack )
{
    stlChar      sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    SQLHSTMT     sStmtHandle;
    stlInt32     sState = 0;
    SQLLEN       sIndicator[5];
    SQLRETURN    sResult;
    stlInt64     sAssocatedIndexID;
    
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT CONSTRAINT_NAME, CONSTRAINT_TYPE, ASSOCIATED_INDEX_ID "
                 "  FROM DEFINITION_SCHEMA.TABLE_CONSTRAINTS "
                 " WHERE CONSTRAINT_ID = %ld",
                 aConstId );

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*)sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            aConstName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            &sIndicator[1],
                            aErrorStack )
             == STL_SUCCESS );


    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            aConstTypeStr,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            &sIndicator[2],
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            ZTQ_C_SBIGINT,
                            & sAssocatedIndexID,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            &sIndicator[3],
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );

    if( sResult == SQL_NO_DATA )
    {
        *aIsSuccess = STL_FALSE;
    }
    else
    {
        if ( (stlStrncmp( aConstTypeStr, "PRIMARY", 7 ) == 0) ||
             (stlStrncmp( aConstTypeStr, "FOREIGN", 7 ) == 0) ||
             (stlStrncmp( aConstTypeStr, "UNIQUE", 6 ) == 0) )
        {
            *aIsKeyConst = STL_TRUE;
        }
        else
        {
            *aIsKeyConst = STL_FALSE;
        }
            
        if ( sIndicator[3] == SQL_NULL_DATA )
        {
            aAssociatedIndexName[0] = '\0';
        }
        else
        {
            stlSnprintf( sSqlString,
                         ZTQ_MAX_COMMAND_BUFFER_SIZE,
                         "SELECT INDEX_NAME "
                         "  FROM DEFINITION_SCHEMA.INDEXES "
                         " WHERE INDEX_ID = %ld",
                         sAssocatedIndexID );
            
            STL_TRY( ztqGetObjectName( sSqlString,
                                       aAssociatedIndexName,
                                       aIsSuccess,
                                       aAllocator,
                                       aErrorStack )
                     == STL_SUCCESS );
        }
    }

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );
    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                      aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/** @} */
