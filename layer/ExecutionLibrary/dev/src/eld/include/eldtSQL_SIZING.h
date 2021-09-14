/*******************************************************************************
 * eldtSQL_SIZING.h
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


#ifndef _ELDT_SQL_SIZING_H_
#define _ELDT_SQL_SIZING_H_ 1

/**
 * @file eldtSQL_SIZING.h
 * @brief SQL_SIZING dictionary base table
 */

/**
 * @defgroup eldtSQL_SIZING SQL_SIZING table
 * @ingroup eldtStandard
 * @{
 */

/**
 * @brief SQL_SIZING 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderSQL_SIZING
{
    /*
     * 식별자 영역
     */

    ELDT_SqlSizing_ColumnOrder_SIZING_ID = 0,
    ELDT_SqlSizing_ColumnOrder_SIZING_NAME,
    
    /*
     * SQL 표준 영역
     */

    ELDT_SqlSizing_ColumnOrder_SUPPORTED_VALUE,
    ELDT_SqlSizing_ColumnOrder_COMMENTS,

    ELDT_SqlSizing_ColumnOrder_MAX
    
} eldtColumnOrderSQL_SIZING;
    

extern eldtDefinitionTableDesc   gEldtTableDescSQL_SIZING;
extern eldtDefinitionColumnDesc  gEldtColumnDescSQL_SIZING[ELDT_SqlSizing_ColumnOrder_MAX];


/**
 * @brief SQL_SIZING 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( SIZING_ID )
 *   - => : PRIMARY KEY ( SIZING_ID )
 *  - SQL 2 : UNIQUE ( SIZING_NAME )
 *   - => : UNIQUE ( SIZING_NAME )
 * - 비표준
 *   - 별도의 key constraint가 없다. 
 */

typedef enum eldtKeyConstSQL_SIZING
{
    /*
     * Primary Key 영역
     */

    ELDT_SqlSizing_Const_PRIMARY_KEY = 0,

    /*
     * Unique 영역
     */

    ELDT_SqlSizing_Const_UNIQUE_SIZING_NAME,

    /*
     * Foreign Key 영역
     */
    
    ELDT_SqlSizing_Const_MAX
    
} eldtKeyConstSQL_SIZING;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescSQL_SIZING[ELDT_SqlSizing_Const_MAX];


/**
 * @brief SQL_SIZING 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 * - 별도로 생성할 인덱스가 없음.
 */

typedef enum eldtIndexSQL_SIZING
{
    ELDT_SqlSizing_Index_MAX = 0
    
} eldtIndexSQL_SIZING;


typedef enum eldtSqlSizingSupportedValue
{
    ELDT_SqlSizing_SUPPORTED_VALUE_NA        = STL_INT32_MIN,
    ELDT_SqlSizing_SUPPORTED_VALUE_UNLIMITED = 0
} eldtSqlSizingSupportedValue;

/**
 * @brief SCHEMA.SQL_SIZING 테이블의 레코드를 구성하기 위한 자료구조
 */

typedef struct eldtSqlSizingRecord
{
    stlInt32   mID;             /**< Identifier of the sizing item */
    stlChar  * mName;           /**< Descriptive name of the sizing item */
    stlInt32   mSupportedValue; /**< Value of the sizing item */
    stlChar  * mComments;       /**< Possibly a comment pertaining to the sizing item */
} eldtSqlSizingRecord;

stlStatus eldtInsertSqlSizingDesc( smlTransId     aTransID,
                                   smlStatement * aStmt,
                                   ellEnv       * aEnv );

/** @} eldtSQL_SIZING */



#endif /* _ELDT_SQL_SIZING_H_ */
