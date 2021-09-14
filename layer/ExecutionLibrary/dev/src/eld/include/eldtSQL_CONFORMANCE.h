/*******************************************************************************
 * eldtSQL_CONFORMANCE.h
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


#ifndef _ELDT_SQL_CONFORMANCE_H_
#define _ELDT_SQL_CONFORMANCE_H_ 1

/**
 * @file eldtSQL_CONFORMANCE.h
 * @brief SQL_CONFORMANCE dictionary base table
 */

/**
 * @defgroup eldtSQL_CONFORMANCE SQL_CONFORMANCE table
 * @ingroup eldtStandard
 * @{
 */

/**
 * @brief SQL_CONFORMANCE 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderSQL_CONFORMANCE
{
    /*
     * 식별자 영역
     */

    ELDT_SqlConformance_ColumnOrder_TYPE = 0,
    ELDT_SqlConformance_ColumnOrder_ID,
    ELDT_SqlConformance_ColumnOrder_SUB_ID,
    ELDT_SqlConformance_ColumnOrder_NAME,
    ELDT_SqlConformance_ColumnOrder_SUB_NAME,
    
    /*
     * SQL 표준 영역
     */

    ELDT_SqlConformance_ColumnOrder_IS_SUPPORTED,
    ELDT_SqlConformance_ColumnOrder_IS_MANDATORY,
    ELDT_SqlConformance_ColumnOrder_IS_VERIFIED_BY,
    ELDT_SqlConformance_ColumnOrder_COMMENTS,

    ELDT_SqlConformance_ColumnOrder_MAX
    
} eldtColumnOrderSQL_CONFORMANCE;
    

extern eldtDefinitionTableDesc   gEldtTableDescSQL_CONFORMANCE;
extern eldtDefinitionColumnDesc  gEldtColumnDescSQL_CONFORMANCE[ELDT_SqlConformance_ColumnOrder_MAX];


/**
 * @brief SQL_CONFORMANCE 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( TYPE, ID, SUB_ID )
 *   - => : PRIMARY KEY ( TYPE, ID, SUB_ID )
 *  - SQL 2 : CHECK
 *   - => : CHECK ( TYPE IN ( 'PART', 'FEATURE', 'SUBFEATURE', 'PACKAGE' ) )
 *  - SQL 3 : CHECK
 *   - => : CHECK ( IS_SUPPORTED = 'YES' OR IS_VERIFIED_BY IS NULL )
 * - 비표준
 *   - 별도의 key constraint가 없다. 
 */

typedef enum eldtKeyConstSQL_CONFORMANCE
{
    /*
     * Primary Key 영역
     */

    ELDT_SqlConformance_Const_PRIMARY_KEY = 0,

    /*
     * Unique 영역
     */

    /*
     * Foreign Key 영역
     */
    
    ELDT_SqlConformance_Const_MAX
    
} eldtKeyConstSQL_CONFORMANCE;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescSQL_CONFORMANCE[ELDT_SqlConformance_Const_MAX];


/**
 * @brief SQL_CONFORMANCE 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 * - 별도로 생성할 인덱스가 없음.
 */

typedef enum eldtIndexSQL_CONFORMANCE
{
    ELDT_SqlConformance_Index_MAX = 0
    
} eldtIndexSQL_CONFORMANCE;


/**
 * @brief SCHEMA.SQL_CONFORMANCE 테이블의 레코드를 구성하기 위한 자료구조
 */

typedef struct eldtSqlConformanceRecord
{
    stlChar  * mType;
    stlChar  * mID;
    stlChar  * mSubID;
    stlChar  * mName;
    stlChar  * mSubName;
    stlBool    mIsSupported;
    stlBool    mIsMandatory;
    stlChar  * mIsVerifiedBy;
    stlChar  * mComments;
} eldtSqlConformanceRecord;

stlStatus eldtInsertSqlConformanceDesc( smlTransId     aTransID,
                                        smlStatement * aStmt,
                                        ellEnv       * aEnv );

/** @} eldtSQL_CONFORMANCE */



#endif /* _ELDT_SQL_CONFORMANCE_H_ */
