/*******************************************************************************
 * eldSqlSizing.c
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
 * @file eldSqlSizing.c
 * @brief SQL SIZING Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldSqlSizing.h>

/**
 * @addtogroup eldSqlSizing
 * @{
 */

/**
 * @brief SQL SIZING Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertSqlSizingDesc() 주석 참조 
 */
stlStatus eldInsertSqlSizingDesc( smlTransId     aTransID,
                                  smlStatement * aStmt,
                                  stlInt32       aID,
                                  stlChar      * aName,
                                  stlInt32       aValue,
                                  stlChar      * aComments,
                                  ellEnv       * aEnv )

{
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid               sRowRid;
    smlScn               sRowScn;
    smlRowBlock          sRowBlock;
    eldMemMark           sMemMark;

    stlInt32   sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aName != NULL, ELL_ERROR_STACK(aEnv) );

    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_SQL_SIZING,
                                     & sMemMark,
                                     & sDataValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * - SQL SIZING descriptor의 정보를 구성한다.
     */

    /*
     * SIZING_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_SIZING,
                                ELDT_SqlSizing_ColumnOrder_SIZING_ID,
                                sDataValueList,
                                & aID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SIZING_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_SIZING,
                                ELDT_SqlSizing_ColumnOrder_SIZING_NAME,
                                sDataValueList,
                                aName,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * SUPPORTED_VALUE
     */

    if( aValue == STL_INT32_MIN )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_SIZING,
                                    ELDT_SqlSizing_ColumnOrder_SUPPORTED_VALUE,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_SIZING,
                                    ELDT_SqlSizing_ColumnOrder_SUPPORTED_VALUE,
                                    sDataValueList,
                                    & aValue,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * COMMENTS
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_SIZING,
                                ELDT_SqlSizing_ColumnOrder_COMMENTS,
                                sDataValueList,
                                aComments,
                                aEnv )
             == STL_SUCCESS );

    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_SQL_SIZING],
                              sDataValueList,
                              NULL, /* unique violation */
                              & sRowBlock,
                              SML_ENV( aEnv ) )
             == STL_SUCCESS );
                              
    /**
     * Dictionary 무결성 유지 
     */

    if ( gEllIsMetaBuilding == STL_TRUE )
    {
        /**
         * 모두 구축후 무결성 검사함.
         */
    }
    else
    {
        STL_TRY( eldRefineIntegrityRowInsert( aTransID,
                                              aStmt,
                                              ELDT_TABLE_ID_SQL_SIZING,
                                              & sRowBlock,
                                              sDataValueList,
                                              aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/** @} eldSqlSizing */
