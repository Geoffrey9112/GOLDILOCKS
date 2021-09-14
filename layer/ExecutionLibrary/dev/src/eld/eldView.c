/*******************************************************************************
 * eldView.c
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
 * @file eldView.c
 * @brief View Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldView.h>

/**
 * @addtogroup eldView
 * @{
 */

/**
 * @brief View Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertViewDesc() 주석 참조 
 */

stlStatus eldInsertViewDesc( smlTransId     aTransID,
                             smlStatement * aStmt,
                             stlInt64       aOwnerID,
                             stlInt64       aSchemaID,
                             stlInt64       aViewedTableID,
                             stlChar      * aViewColumns,
                             stlChar      * aViewString,
                             stlBool        aIsCompiled,
                             stlBool        aIsAffected,
                             ellEnv       * aEnv )
{
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid                 sRowRid;
    smlScn                 sRowScn;
    smlRowBlock            sRowBlock;
    eldMemMark             sMemMark;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOwnerID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewedTableID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewString != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_VIEWS,
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
     * - view descriptor의 정보를 구성한다.
     */

    /*
     * OWNER_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEWS,
                                ELDT_Views_ColumnOrder_OWNER_ID,
                                sDataValueList,
                                & aOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCHEMA_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEWS,
                                ELDT_Views_ColumnOrder_SCHEMA_ID,
                                sDataValueList,
                                & aSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * TABLE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEWS,
                                ELDT_Views_ColumnOrder_TABLE_ID,
                                sDataValueList,
                                & aViewedTableID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * VIEW_COLUMNS
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEWS,
                                ELDT_Views_ColumnOrder_VIEW_COLUMNS,
                                sDataValueList,
                                aViewColumns,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * VIEW_DEFINITION
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEWS,
                                ELDT_Views_ColumnOrder_VIEW_DEFINITION,
                                sDataValueList,
                                aViewString,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CHECK_OPTION
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEWS,
                                ELDT_Views_ColumnOrder_CHECK_OPTION,
                                sDataValueList,
                                "NONE",
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_COMPILED
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEWS,
                                ELDT_Views_ColumnOrder_IS_COMPILED,
                                sDataValueList,
                                & aIsCompiled,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_AFFECTED
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEWS,
                                ELDT_Views_ColumnOrder_IS_AFFECTED,
                                sDataValueList,
                                & aIsAffected,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * IS_UPDATABLE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEWS,
                                ELDT_Views_ColumnOrder_IS_UPDATABLE,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    
    /*
     * IS_TRIGGER_UPDATABLE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEWS,
                                ELDT_Views_ColumnOrder_IS_TRIGGER_UPDATABLE,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_TRIGGER_DELETABLE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEWS,
                                ELDT_Views_ColumnOrder_IS_TRIGGER_DELETABLE,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_TRIGGER_INSERTABLE_INTO
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEWS,
                                ELDT_Views_ColumnOrder_IS_TRIGGER_INSERTABLE_INTO,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_VIEWS],
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
                                              ELDT_TABLE_ID_VIEWS,
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



/**
 * @brief View Cache 를 구축한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aViewID           Viewed Table ID
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldBuildViewCache( smlTransId       aTransID,
                             smlStatement   * aStmt,
                             stlInt64         aViewID,
                             ellEnv         * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewID != ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );

    /**
     * SQL-Column Cache 를 추가 
     */

    STL_TRY( eldcInsertCacheColumnByTableID( aTransID,
                                             aStmt,
                                             aViewID,
                                             aEnv )
             == STL_SUCCESS );
    
    /**
     * SQL-Table Cache 를 추가 
     */

    STL_TRY( eldcInsertCacheTableByTableID( aTransID,
                                            aStmt,
                                            aViewID,
                                            aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} eldView */
