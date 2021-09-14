/*******************************************************************************
 * eldtSQL_IMPLEMENTATION_INFO.h
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


#ifndef _ELDT_SQL_IMPLEMENTATION_INFO_H_
#define _ELDT_SQL_IMPLEMENTATION_INFO_H_ 1

/**
 * @file eldtSQL_IMPLEMENTATION_INFO.h
 * @brief SQL_IMPLEMENTATION_INFO dictionary base table
 */

/**
 * @defgroup eldtSQL_IMPLEMENTATION_INFO SQL_IMPLEMENTATION_INFO table
 * @ingroup eldtStandard
 * @{
 */

/**
 * @brief SQL_IMPLEMENTATION_INFO 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderSQL_IMPLEMENTATION_INFO
{
    /*
     * 식별자 영역
     */

    ELDT_SqlImplementationInfo_ColumnOrder_IMPLEMENTATION_INFO_ID = 0,
    ELDT_SqlImplementationInfo_ColumnOrder_IMPLEMENTATION_INFO_NAME,
    
    /*
     * SQL 표준 영역
     */

    ELDT_SqlImplementationInfo_ColumnOrder_INTEGER_VALUE,
    ELDT_SqlImplementationInfo_ColumnOrder_CHARACTER_VALUE,
    ELDT_SqlImplementationInfo_ColumnOrder_COMMENTS,

    ELDT_SqlImplementationInfo_ColumnOrder_MAX
    
} eldtColumnOrderSQL_IMPLEMENTATION_INFO;
    

extern eldtDefinitionTableDesc   gEldtTableDescSQL_IMPLEMENTATION_INFO;
extern eldtDefinitionColumnDesc  gEldtColumnDescSQL_IMPLEMENTATION_INFO[ELDT_SqlImplementationInfo_ColumnOrder_MAX];


/**
 * @brief SQL_IMPLEMENTATION_INFO 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( IMPLEMENTATION_INFO_ID )
 *   - => : PRIMARY KEY ( IMPLEMENTATION_INFO_ID )
 *  - SQL 2 : CHECK
 *   - => : CHECK ( INTERGER_VALUE IS NULL OR CHARACTER_VALUE IS NULL )
 * - 비표준
 *   - 별도의 key constraint가 없다. 
 */

typedef enum eldtKeyConstSQL_IMPLEMENTATION_INFO
{
    /*
     * Primary Key 영역
     */

    ELDT_SqlImplementationInfo_Const_PRIMARY_KEY = 0,

    /*
     * Unique 영역
     */

    /*
     * Foreign Key 영역
     */
    
    ELDT_SqlImplementationInfo_Const_MAX
    
} eldtKeyConstSQL_IMPLEMENTATION_INFO;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescSQL_IMPLEMENTATION_INFO[ELDT_SqlImplementationInfo_Const_MAX];


/**
 * @brief SQL_IMPLEMENTATION_INFO 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 * - 별도로 생성할 인덱스가 없음.
 */

typedef enum eldtIndexSQL_IMPLEMENTATION_INFO
{
    ELDT_SqlImplementationInfo_Index_MAX = 0
    
} eldtIndexSQL_IMPLEMENTATION_INFO;


/**
 * @brief SCHEMA.SQL_IMPLEMENTATION_INFO 테이블의 레코드를 구성하기 위한 자료구조
 */

typedef struct eldtSqlImplementationInfoRecord
{
    stlChar  * mID;             /**< Identifier string of the implementation information item */
    stlChar  * mName;           /**< Descriptive name of the implementation information item */
    stlInt32   mIntegerValue;   /**< Value of the implementation information item */
    stlChar  * mCharacterValue; /**< Value of the implementation information item */
    stlChar  * mComments;       /**< Possibly a comment pertaining to the implementation information item */
} eldtSqlImplementationInfoRecord;

stlStatus eldtInsertSqlImplementationInfoDesc( smlTransId     aTransID,
                                               smlStatement * aStmt,
                                               stlChar      * aCharacterSetName,
                                               ellEnv       * aEnv );

/** @} eldtSQL_IMPLEMENTATION_INFO */



#endif /* _ELDT_SQL_IMPLEMENTATION_INFO_H_ */
