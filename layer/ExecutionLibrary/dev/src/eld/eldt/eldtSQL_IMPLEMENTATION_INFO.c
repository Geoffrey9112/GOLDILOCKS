/*******************************************************************************
 * eldtSQL_IMPLEMENTATION_INFO.c
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
 * @file eldtSQL_IMPLEMENTATION_INFO.c
 * @brief SQL_IMPLEMENTATION_INFO dictionary table 
 */

#include <ell.h>

#include <eldt.h>

/**
 * @addtogroup eldtSQL_IMPLEMENTATION_INFO
 * @internal
 * @{
 */

eldtSqlImplementationInfoRecord gEldtSqlImplementationInfoRecord[] =
{
    {
        "10003",
        "CATALOG NAME",
        STL_INT32_MIN,
        "N",
        "CHAR: 'Y' if supported, otherwise 'N'"
    },
    {
        "10004",
        "COLLATING SEQUENCE",
        STL_INT32_MIN,
        NULL,   /* record 구성시 자동으로 설정함. */
        "CHAR: default collation name"
    },
    {
        "23",
        "CURSOR COMMIT BEHAVIOR",
        2,
        NULL,
        "INT: 0: close cursors & delete prepared stmts\n"
        "     1: close cursors & retain prepared stmts\n"
        "     2: leave cursors open & retain stmts"
    },
    {
        "2",
        "DATA SOURCE NAME",
        STL_INT32_MIN,
        "DSN",
        "CHAR: <connection name> on CONNECT statement"
    },
    {
        "17",
        "DBMS NAME",
        STL_INT32_MIN,
        STL_PRODUCT_NAME,
        "CHAR: Name of the implementation software"
    },
    {
        "18",
        "DBMS VERSION",
        STL_INT32_MIN,
        NULL,   /* record 구성시 자동으로 설정함. */
        "CHAR: Version of the implementation software\n"
        "      The format is:\n"
        "       <part1>.<part2>.<part3>[<part4>]\n"
        "      where:\n"
        "       <part1> ::= <digit><digit>\n"
        "       <part2> ::= <digit><digit>\n"
        "       <part3> ::= <digit><digit><digit><digit>\n"
        "       <part4> ::= <character representation>"
    },
    {
        "26",
        "DEFAULT TRANSACTION ISOLATION",
        2,
        NULL,
        "INT: 1: READ UNCOMMITTED\n"
        "     2: READ COMMITTED\n"
        "     3: REPEATABLE READ\n"
        "     4: SERIALIZABLE"
    },
    {
        "28",
        "IDENTIFIER CASE",
        1,
        NULL,
        "The case in which identifiers are stored in the Definition Schema\n"
        "INT: 1: stored in upper case\n"
        "     2: stored in lower case\n"
        "     3: stored in mixed case - case sensitive\n"
        "     4: stored in mixed case - case insensitive"
    },
    {
        "85",
        "NULL COLLATION",
        0,
        NULL,
        "INT: 0: nulls higher than non-nulls\n"
        "     1: nulls lower than non-nulls"
    },
    {
        "13",
        "SERVER NAME",
        STL_INT32_MIN,
        STL_PRODUCT_NAME,
        "CHAR: <SQL server name> on CONNECT statement"
    },
    {
        "94",
        "SPECIAL CHARACTERS",
        STL_INT32_MIN,
        "$",
        "CHAR: All special chars OK in non-delimited ids"
    },
    {
        "46",
        "TRANSACTION CAPABLE",
        2,
        NULL,
        "INT: 0: not supported\n"
        "     1: DML only - error if DDL\n"
        "     2: both DML and DDL\n"
        "     3: DML only - commit before DDL\n"
        "     4: DML only - ignore DDL"
    },

    
    {
        NULL,
        NULL,
        STL_INT32_MIN,
        NULL,
        NULL
    }
};

/**
 * @brief DEFINITION_SCHEMA.SQL_IMPLEMENTATION_INFO 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescSQL_IMPLEMENTATION_INFO[ELDT_SqlImplementationInfo_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO,                         /**< Table ID */
        "IMPLEMENTATION_INFO_ID",                                      /**< 컬럼의 이름  */
        ELDT_SqlImplementationInfo_ColumnOrder_IMPLEMENTATION_INFO_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                                     /**< 컬럼의 Nullable 여부 */
        "identifier string of the implementation information item"
    },
    {
        ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO,                           /**< Table ID */
        "IMPLEMENTATION_INFO_NAME",                                      /**< 컬럼의 이름  */
        ELDT_SqlImplementationInfo_ColumnOrder_IMPLEMENTATION_INFO_NAME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                                           /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                                   /**< 컬럼의 Nullable 여부 */
        "descriptive name of the implementation information item"        
    },
    {
        ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO,                /**< Table ID */
        "INTEGER_VALUE",                                      /**< 컬럼의 이름  */
        ELDT_SqlImplementationInfo_ColumnOrder_INTEGER_VALUE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                          /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                                    /**< 컬럼의 Nullable 여부 */
        "Value of the implementation information item, or "
        "null if the value is contained in the column CHARACTER_VALUE"
    },
    {
        ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO,                  /**< Table ID */
        "CHARACTER_VALUE",                                      /**< 컬럼의 이름  */
        ELDT_SqlImplementationInfo_ColumnOrder_CHARACTER_VALUE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                                      /**< 컬럼의 Nullable 여부 */
        "Value of the implementation information item, or "
        "null if the value is contained in the column INTEGER_VALUE"
    },
    {
        ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO,           /**< Table ID */
        "COMMENTS",                                      /**< 컬럼의 이름  */
        ELDT_SqlImplementationInfo_ColumnOrder_COMMENTS, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                           /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                               /**< 컬럼의 Nullable 여부 */
        "possibly a comment pertaining to the implementation information item"
    }
};


/**
 * @brief DEFINITION_SCHEMA.SQL_IMPLEMENTATION_INFO 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescSQL_IMPLEMENTATION_INFO =
{
    ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO, /**< Table ID */
    "SQL_IMPLEMENTATION_INFO",             /**< 테이블의 이름  */
    "The SQL_IMPLEMENTATION_INFO base table has one row for "
    "each SQL-implementation information item defined by ISO/IEC 9075."
};

/**
 * @brief DEFINITION_SCHEMA.SQL_IMPLEMENTATION_INFO 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescSQL_IMPLEMENTATION_INFO[ELDT_SqlImplementationInfo_Const_MAX] =
{
    {
        ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO,        /**< Table ID */
        ELDT_SqlImplementationInfo_Const_PRIMARY_KEY, /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,        /**< Table Constraint 유형 */
        1,                                            /**< 키를 구성하는 컬럼의 개수 */
        {                                  /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_SqlImplementationInfo_ColumnOrder_IMPLEMENTATION_INFO_ID
        },
        ELL_DICT_OBJECT_ID_NA,                        /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                        /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};


/**
 * @brief SQL-implementation information item 명세를 추가한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aStmt              Statement
 * @param[in]  aCharacterSetName  Character Set의 이름 
 * @param[in]  aEnv               Execution Library Environment
 * @remarks
 */
stlStatus eldtInsertSqlImplementationInfoDesc( smlTransId     aTransID,
                                               smlStatement * aStmt,
                                               stlChar      * aCharacterSetName,
                                               ellEnv       * aEnv )
{
    eldtSqlImplementationInfoRecord * sRecord = NULL;

    stlVersionInfo   sVersionInfo;
    stlChar          sVersion[32];

    stlChar  * sCharValue = NULL;
    stlInt32   i = -1;

    while( STL_TRUE )
    {
        i++;

        sRecord = &gEldtSqlImplementationInfoRecord[i];
        
        if( sRecord->mID == NULL )
        {
            break;
        }

        if( stlStrcmp( sRecord->mID, "10004" ) == 0 )
        {
            /* COLLATING SEQUENCE */
            sCharValue = aCharacterSetName;
        }
        else if( stlStrcmp( sRecord->mID, "18" ) == 0 )
        {
            /* DBMS VERSION */
            stlGetVersionInfo( &sVersionInfo );
            stlSnprintf( sVersion,
                         32,
                         "%02d.%02d.%04d",
                         sVersionInfo.mMajor,
                         sVersionInfo.mMinor,
                         sVersionInfo.mPatch );

            sCharValue = sVersion;
        }
        else
        {
            sCharValue = sRecord->mCharacterValue;
        }
        
        STL_TRY( ellInsertSqlImplementationInfoDesc( aTransID,
                                                     aStmt,
                                                     sRecord->mID,
                                                     sRecord->mName,
                                                     sRecord->mIntegerValue,
                                                     sCharValue,
                                                     sRecord->mComments,
                                                     aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} eldtSQL_IMPLEMENTATION_INFO */
