/*******************************************************************************
 * eldReferentialConstraint.c
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
 * @file eldReferentialConstraint.c
 * @brief Referential Constraint Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldReferentialConstraint.h>

/**
 * @addtogroup eldReferentialConstraint
 * @{
 */

/**
 * @brief Referential Constraint Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertReferentialConstraintDesc() 주석 참조 
 */
stlStatus eldInsertRefConstraintDesc( smlTransId                aTransID,
                                      smlStatement            * aStmt,
                                      stlInt64                  aRefConstOwnerID,
                                      stlInt64                  aRefConstSchemaID,
                                      stlInt64                  aRefConstID,
                                      stlInt64                  aUniqueConstOwnerID,
                                      stlInt64                  aUniqueConstSchemaID,
                                      stlInt64                  aUniqueConstID,
                                      ellReferentialMatchOption aMatchOption,
                                      ellReferentialActionRule  aUpdateRule,
                                      ellReferentialActionRule  aDeleteRule,
                                      ellEnv                  * aEnv )
{
    ellReferentialMatchOption  sMatchOption = ELL_REFERENTIAL_MATCH_OPTION_NA;
    ellReferentialActionRule   sUpdateRule  = ELL_REFERENTIAL_ACTION_RULE_NA;
    ellReferentialActionRule   sDeleteRule  = ELL_REFERENTIAL_ACTION_RULE_NA;
        
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid               sRowRid;
    smlScn               sRowScn;
    smlRowBlock          sRowBlock;
    eldMemMark           sMemMark;

    stlInt32  sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefConstOwnerID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefConstSchemaID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefConstID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUniqueConstOwnerID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUniqueConstSchemaID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUniqueConstID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aMatchOption >= ELL_REFERENTIAL_MATCH_OPTION_NA) &&
                        (aMatchOption < ELL_REFERENTIAL_MATCH_OPTION_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aUpdateRule >= ELL_REFERENTIAL_ACTION_RULE_NA) &&
                        (aUpdateRule < ELL_REFERENTIAL_ACTION_RULE_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aDeleteRule >= ELL_REFERENTIAL_ACTION_RULE_NA) &&
                        (aDeleteRule < ELL_REFERENTIAL_ACTION_RULE_MAX),
                        ELL_ERROR_STACK(aEnv) );
    
    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
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
     * - referential constraint descriptor의 정보를 구성한다.
     */

    /*
     * CONSTRAINT_OWNER_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                ELDT_Reference_ColumnOrder_CONSTRAINT_OWNER_ID,
                                sDataValueList,
                                & aRefConstOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CONSTRAINT_SCHEMA_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                ELDT_Reference_ColumnOrder_CONSTRAINT_SCHEMA_ID,
                                sDataValueList,
                                & aRefConstSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CONSTRAINT_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                ELDT_Reference_ColumnOrder_CONSTRAINT_ID,
                                sDataValueList,
                                & aRefConstID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * UNIQUE_CONSTRAINT_OWNER_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_OWNER_ID,
                                sDataValueList,
                                & aUniqueConstOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * UNIQUE_CONSTRAINT_SCHEMA_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_SCHEMA_ID,
                                sDataValueList,
                                & aUniqueConstSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * UNIQUE_CONSTRAINT_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_ID,
                                sDataValueList,
                                & aUniqueConstID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * MATCH_OPTION
     */
    
    if ( aMatchOption == ELL_REFERENTIAL_MATCH_OPTION_NA )
    {
        sMatchOption = ELL_REFERENTIAL_MATCH_OPTION_DEFAULT;
    }
    else
    {
        sMatchOption = aMatchOption;
    }

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                ELDT_Reference_ColumnOrder_MATCH_OPTION,
                                sDataValueList,
                                (void *) gEllReferentialMatchOptionString[sMatchOption],
                                aEnv )
             == STL_SUCCESS );

    /*
     * MATCH_OPTION_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                ELDT_Reference_ColumnOrder_MATCH_OPTION_ID,
                                sDataValueList,
                                & sMatchOption,
                                aEnv )
             == STL_SUCCESS );

    /*
     * UPDATE_RULE
     */
    
    if ( aUpdateRule == ELL_REFERENTIAL_ACTION_RULE_NA )
    {
        sUpdateRule = ELL_REFERENTIAL_ACTION_RULE_DEFAULT;
    }
    else
    {
        sUpdateRule = aUpdateRule;
    }

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                ELDT_Reference_ColumnOrder_UPDATE_RULE,
                                sDataValueList,
                                (void *) gEllReferentialActionRuleString[sUpdateRule],
                                aEnv )
             == STL_SUCCESS );

    /*
     * UPDATE_RULE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                ELDT_Reference_ColumnOrder_UPDATE_RULE_ID,
                                sDataValueList,
                                & sUpdateRule,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DELETE_RULE
     */
    
    if ( aDeleteRule == ELL_REFERENTIAL_ACTION_RULE_NA )
    {
        sDeleteRule = ELL_REFERENTIAL_ACTION_RULE_DEFAULT;
    }
    else
    {
        sDeleteRule = aUpdateRule;
    }

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                ELDT_Reference_ColumnOrder_DELETE_RULE,
                                sDataValueList,
                                (void *) gEllReferentialActionRuleString[sDeleteRule],
                                aEnv )
             == STL_SUCCESS );

    /*
     * DELETE_RULE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                ELDT_Reference_ColumnOrder_DELETE_RULE_ID,
                                sDataValueList,
                                & sDeleteRule,
                                aEnv )
             == STL_SUCCESS );



    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS],
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
                                              ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
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

/** @} eldReferentialConstraint */
