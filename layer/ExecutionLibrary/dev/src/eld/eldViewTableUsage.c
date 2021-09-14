/*******************************************************************************
 * eldViewTableUsage.c
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
 * @file eldViewTableUsage.c
 * @brief View Table Usage Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldViewTableUsage.h>

/**
 * @addtogroup eldViewTableUsage
 * @{
 */

/**
 * @brief View Table Usage Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertViewTableUsageDesc() 주석 참조 
 */

stlStatus eldInsertViewTableUsageDesc( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       stlInt64         aViewOwnerID,
                                       stlInt64         aViewSchemaID,
                                       stlInt64         aViewID,
                                       stlInt64         aTableOwnerID,
                                       stlInt64         aTableSchemaID,
                                       stlInt64         aTableID,
                                       ellEnv         * aEnv )
{
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid               sRowRid;
    smlScn               sRowScn;
    smlRowBlock          sRowBlock;
    eldMemMark           sMemMark;

    stlInt32 sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewOwnerID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewSchemaID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableOwnerID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableSchemaID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID != 0, ELL_ERROR_STACK(aEnv) );
    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_VIEW_TABLE_USAGE,
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
     * - view table usage descriptor의 정보를 구성한다.
     */

    /*
     * VIEW_OWNER_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEW_TABLE_USAGE,
                                ELDT_ViewTable_ColumnOrder_VIEW_OWNER_ID,
                                sDataValueList,
                                & aViewOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * VIEW_SCHEMA_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEW_TABLE_USAGE,
                                ELDT_ViewTable_ColumnOrder_VIEW_SCHEMA_ID,
                                sDataValueList,
                                & aViewSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * VIEW_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEW_TABLE_USAGE,
                                ELDT_ViewTable_ColumnOrder_VIEW_ID,
                                sDataValueList,
                                & aViewID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * OWNER_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEW_TABLE_USAGE,
                                ELDT_ViewTable_ColumnOrder_OWNER_ID,
                                sDataValueList,
                                & aTableOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCHEMA_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEW_TABLE_USAGE,
                                ELDT_ViewTable_ColumnOrder_SCHEMA_ID,
                                sDataValueList,
                                & aTableSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * TABLE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_VIEW_TABLE_USAGE,
                                ELDT_ViewTable_ColumnOrder_TABLE_ID,
                                sDataValueList,
                                & aTableID,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_VIEW_TABLE_USAGE],
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
                                              ELDT_TABLE_ID_VIEW_TABLE_USAGE,
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

/** @} eldViewTableUsage */
