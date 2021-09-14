/*******************************************************************************
 * eldKeyColumnUsage.c
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
 * @file eldKeyColumnUsage.c
 * @brief Key Column Usage Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldKeyColumnUsage.h>

/**
 * @addtogroup eldKeyColumnUsage
 * @{
 */

/**
 * @brief Key Column Usage Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertKeyColumnUsageDesc() 주석 참조 
 */

stlStatus eldInsertKeyColumnUsageDesc( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       stlInt64         aKeyOwnerID,
                                       stlInt64         aKeySchemaID,
                                       stlInt64         aKeyConstID,
                                       stlInt64         aRefOwnerID,
                                       stlInt64         aRefSchemaID,
                                       stlInt64         aRefTableID,
                                       stlInt64         aRefColumnID,
                                       stlInt32         aKeyOrdinalPosition,
                                       stlInt32         aPositionInUniqueConst,
                                       ellEnv         * aEnv )
{
    stlInt32                   sPosition = 0;
    
    knlValueBlockList        * sDataValueList  = NULL;

    smlRowBlock  sRowBlock;
    smlRid       sRowRid;
    smlScn       sRowScn;
    eldMemMark   sMemMark;

    stlInt32  sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyOwnerID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeySchemaID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyConstID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefOwnerID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefSchemaID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefTableID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefColumnID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyOrdinalPosition >= 0, ELL_ERROR_STACK(aEnv) );
    

    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_KEY_COLUMN_USAGE,
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
     * - key column usage descriptor의 정보를 구성한다.
     */

    /*
     * CONSTRAINT_OWNER_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                ELDT_KeyColumn_ColumnOrder_CONSTRAINT_OWNER_ID,
                                sDataValueList,
                                & aKeyOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CONSTRAINT_SCHEMA_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                ELDT_KeyColumn_ColumnOrder_CONSTRAINT_SCHEMA_ID,
                                sDataValueList,
                                & aKeySchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CONSTRAINT_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                ELDT_KeyColumn_ColumnOrder_CONSTRAINT_ID,
                                sDataValueList,
                                & aKeyConstID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * OWNER_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                ELDT_KeyColumn_ColumnOrder_OWNER_ID,
                                sDataValueList,
                                & aRefOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCHEMA_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                ELDT_KeyColumn_ColumnOrder_SCHEMA_ID,
                                sDataValueList,
                                & aRefSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * TABLE_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                ELDT_KeyColumn_ColumnOrder_TABLE_ID,
                                sDataValueList,
                                & aRefTableID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COLUMN_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                ELDT_KeyColumn_ColumnOrder_COLUMN_ID,
                                sDataValueList,
                                & aRefColumnID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * ORDINAL_POSITION
     */

    sPosition = ELD_ORDINAL_POSITION_TO_DATABASE(aKeyOrdinalPosition);
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                ELDT_KeyColumn_ColumnOrder_ORDINAL_POSITION,
                                sDataValueList,
                                & sPosition,
                                aEnv )
             == STL_SUCCESS );

    /*
     * POSITION_IN_UNIQUE_CONSTRAINT
     */

    if ( aPositionInUniqueConst == ELL_DICT_POSITION_NA )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                    ELDT_KeyColumn_ColumnOrder_POSITION_IN_UNIQUE_CONSTRAINT,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sPosition = ELD_ORDINAL_POSITION_TO_DATABASE(aPositionInUniqueConst);
        
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                    ELDT_KeyColumn_ColumnOrder_POSITION_IN_UNIQUE_CONSTRAINT,
                                    sDataValueList,
                                    & sPosition,
                                    aEnv )
                 == STL_SUCCESS );
    }

    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_KEY_COLUMN_USAGE],
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
                                              ELDT_TABLE_ID_KEY_COLUMN_USAGE,
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

/** @} eldKeyColumnUsage */
