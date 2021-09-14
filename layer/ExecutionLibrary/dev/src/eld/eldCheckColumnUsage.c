/*******************************************************************************
 * eldCheckColumnUsage.c
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
 * @file eldCheckColumnUsage.c
 * @brief Check Column Usage Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldCheckColumnUsage.h>

/**
 * @addtogroup eldCheckColumnUsage
 * @{
 */

/**
 * @brief Check Column Usage Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertCheckColumnUsageDesc() 주석 참조 
 */

stlStatus eldInsertCheckColumnUsageDesc( smlTransId       aTransID,
                                         smlStatement   * aStmt,
                                         stlInt64         aCheckOwnerID,
                                         stlInt64         aCheckSchemaID,
                                         stlInt64         aCheckConstID,
                                         stlInt64         aRefOwnerID,
                                         stlInt64         aRefSchemaID,
                                         stlInt64         aRefTableID,
                                         stlInt64         aRefColumnID,
                                         ellEnv         * aEnv )
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
    STL_PARAM_VALIDATE( aCheckOwnerID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCheckSchemaID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCheckConstID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefOwnerID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefSchemaID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefTableID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefColumnID > 0, ELL_ERROR_STACK(aEnv) );

    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
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
     * - check column usage descriptor의 정보를 구성한다.
     */

    /*
     * CONSTRAINT_OWNER_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
                                ELDT_CheckColumn_ColumnOrder_CONSTRAINT_OWNER_ID,
                                sDataValueList,
                                & aCheckOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CONSTRAINT_SCHEMA_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
                                ELDT_CheckColumn_ColumnOrder_CONSTRAINT_SCHEMA_ID,
                                sDataValueList,
                                & aCheckSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CONSTRAINT_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
                                ELDT_CheckColumn_ColumnOrder_CONSTRAINT_ID,
                                sDataValueList,
                                & aCheckConstID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * OWNER_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
                                ELDT_CheckColumn_ColumnOrder_OWNER_ID,
                                sDataValueList,
                                & aRefOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCHEMA_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
                                ELDT_CheckColumn_ColumnOrder_SCHEMA_ID,
                                sDataValueList,
                                & aRefSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * TABLE_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
                                ELDT_CheckColumn_ColumnOrder_TABLE_ID,
                                sDataValueList,
                                & aRefTableID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COLUMN_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
                                ELDT_CheckColumn_ColumnOrder_COLUMN_ID,
                                sDataValueList,
                                & aRefColumnID,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_CHECK_COLUMN_USAGE],
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
                                              ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
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

/** @} eldCheckColumnUsage */
