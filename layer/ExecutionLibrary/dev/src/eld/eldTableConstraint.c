/*******************************************************************************
 * eldTableConstraint.c
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
 * @file eldTableConstraint.c
 * @brief Table Constraint Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldTableConstraint.h>

/**
 * @addtogroup eldTableConstraint
 * @{
 */

/**
 * @brief Table Constraint Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertTableConstraintDesc() 주석 참조 
 */
stlStatus eldInsertTableConstraintDesc( smlTransId               aTransID,
                                        smlStatement           * aStmt,
                                        stlInt64                 aConstOwnerID,
                                        stlInt64                 aConstSchemaID,
                                        stlInt64               * aConstID,
                                        stlInt64                 aTableOwnerID,
                                        stlInt64                 aTableSchemaID,
                                        stlInt64                 aTableID,
                                        stlChar                * aConstName,
                                        ellTableConstraintType   aConstType,
                                        stlBool                  aIsDeferrable,
                                        stlBool                  aInitiallyDeferred,
                                        stlBool                  aEnforced,
                                        stlInt64                 aIndexID,
                                        stlChar                * aConstComment,
                                        ellEnv                 * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    stlInt64      sConstID = ELL_DICT_OBJECT_ID_NA;

    /** @todo 제거해야 할 변수임 */
    stlBool       sValidate = STL_FALSE;
        
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    eldMemMark          sMemMark;

    stlInt32  sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstOwnerID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstSchemaID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstID != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableOwnerID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableSchemaID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aConstType > ELL_TABLE_CONSTRAINT_TYPE_NA) &&
                        (aConstType < ELL_TABLE_CONSTRAINT_TYPE_MAX)
                        , ELL_ERROR_STACK(aEnv) );

    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
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
     * - table constraint descriptor의 정보를 구성한다.
     */

    /*
     * CONSTRAINT_OWNER_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_CONSTRAINT_OWNER_ID,
                                sDataValueList,
                                & aConstOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CONSTRAINT_SCHEMA_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_CONSTRAINT_SCHEMA_ID,
                                sDataValueList,
                                & aConstSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CONSTRAINT_ID
     */

    if ( gEllIsMetaBuilding == STL_TRUE )
    {
        /* Database Dictionary 를 초기화하는 단계 */
        gEldtIdentityValue[ELDT_TABLE_ID_TABLE_CONSTRAINTS] += 1;
        sConstID = gEldtIdentityValue[ELDT_TABLE_ID_TABLE_CONSTRAINTS];
    }
    else
    {
        /* Database가 이미 구축되어 있는 단계 */
        
        /**
         * Identity Column으로부터 Table ID를 획득
         */
        STL_TRY( smlGetNextSequenceVal( gEldIdentityColumnHandle[ELDT_TABLE_ID_TABLE_CONSTRAINTS],
                                        & sConstID,
                                        SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        
    }

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_CONSTRAINT_ID,
                                sDataValueList,
                                & sConstID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * OWNER_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_OWNER_ID,
                                sDataValueList,
                                & aTableOwnerID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * SCHEMA_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_SCHEMA_ID,
                                sDataValueList,
                                & aTableSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * TABLE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_TABLE_ID,
                                sDataValueList,
                                & aTableID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CONSTRAINT_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_CONSTRAINT_NAME,
                                sDataValueList,
                                aConstName,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * CONSTRAINT_TYPE
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_CONSTRAINT_TYPE,
                                sDataValueList,
                                (void *) gEllTableConstraintTypeString[aConstType],
                                aEnv )
             == STL_SUCCESS );

    /*
     * CONSTRAINT_TYPE_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_CONSTRAINT_TYPE_ID,
                                sDataValueList,
                                & aConstType,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_DEFERRABLE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_IS_DEFERRABLE,
                                sDataValueList,
                                & aIsDeferrable,
                                aEnv )
             == STL_SUCCESS );

    /*
     * INITIALLY_DEFERRED
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_INITIALLY_DEFERRED,
                                sDataValueList,
                                & aInitiallyDeferred,
                                aEnv )
             == STL_SUCCESS );

    /*
     * ENFORCED
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_ENFORCED,
                                sDataValueList,
                                & aEnforced,
                                aEnv )
             == STL_SUCCESS );

    /*
     * ASSOCIATED_INDEX_ID
     */

    if ( aIndexID == ELL_DICT_OBJECT_ID_NA )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                    ELDT_TableConst_ColumnOrder_ASSOCIATED_INDEX_ID,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                    ELDT_TableConst_ColumnOrder_ASSOCIATED_INDEX_ID,
                                    sDataValueList,
                                    & aIndexID,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * VALIDATE
     */

    sValidate = STL_TRUE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_VALIDATE,
                                sDataValueList,
                                & sValidate,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * IS_BUILTIN
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_IS_BUILTIN,
                                sDataValueList,
                                & gEllIsMetaBuilding,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * CREATED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_CREATED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * MODIFIED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_MODIFIED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COMMENTS
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_COMMENTS,
                                sDataValueList,
                                aConstComment,
                                aEnv )
             == STL_SUCCESS );



    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_TABLE_CONSTRAINTS],
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
                                              ELDT_TABLE_ID_TABLE_CONSTRAINTS,
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
    
    /**
     * Output 설정 
     */
    
    *aConstID = sConstID;
    
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
 * @brief Constraint Dictionary 정보만을 삭제한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aConstHandle      Constraint Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldDeleteDictConst( smlTransId       aTransID,
                              smlStatement   * aStmt,
                              ellDictHandle  * aConstHandle,
                              ellEnv         * aEnv )
{
    stlInt64 sConstID = ELL_DICT_OBJECT_ID_NA;
    ellTableConstraintType  sConstType = ELL_TABLE_CONSTRAINT_TYPE_NA;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Constraint ID 획득
     */
    
    sConstID = ellGetConstraintID( aConstHandle );
    sConstType = ellGetConstraintType( aConstHandle );

    /**
     * Constraint 유형별 관련 Dictionary Record 정보를 삭제
     */

    switch( sConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            {
                /*
                 * DELETE FROM DEFINITION_SCHEMA.KEY_COLUMN_USAGE
                 *  WHERE CONSTRAINT_ID = sConstID;
                 */
    
                STL_TRY( eldDeleteObjectRows( aTransID,
                                              aStmt,
                                              ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                              ELDT_KeyColumn_ColumnOrder_CONSTRAINT_ID,
                                              sConstID,
                                              aEnv )
                         == STL_SUCCESS );
                
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            {
                /*
                 * DELETE FROM DEFINITION_SCHEMA.KEY_COLUMN_USAGE
                 *  WHERE CONSTRAINT_ID = sConstID;
                 */
    
                STL_TRY( eldDeleteObjectRows( aTransID,
                                              aStmt,
                                              ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                              ELDT_KeyColumn_ColumnOrder_CONSTRAINT_ID,
                                              sConstID,
                                              aEnv )
                         == STL_SUCCESS );
                
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                /*
                 * DELETE FROM DEFINITION_SCHEMA.REFERENTIAL_CONSTRAINTS
                 *  WHERE CONSTRAINT_ID = sConstID;
                 */
    
                STL_TRY( eldDeleteObjectRows( aTransID,
                                              aStmt,
                                              ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                              ELDT_Reference_ColumnOrder_CONSTRAINT_ID,
                                              sConstID,
                                              aEnv )
                         == STL_SUCCESS );
                
                /*
                 * DELETE FROM DEFINITION_SCHEMA.KEY_COLUMN_USAGE
                 *  WHERE CONSTRAINT_ID = sConstID;
                 */
    
                STL_TRY( eldDeleteObjectRows( aTransID,
                                              aStmt,
                                              ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                              ELDT_KeyColumn_ColumnOrder_CONSTRAINT_ID,
                                              sConstID,
                                              aEnv )
                         == STL_SUCCESS );
                
                break;
            }
            
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                /*
                 * DELETE FROM DEFINITION_SCHEMA.CHECK_COLUMN_USAGE
                 *  WHERE CONSTRAINT_ID = sConstID;
                 */
    
                STL_TRY( eldDeleteObjectRows( aTransID,
                                              aStmt,
                                              ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
                                              ELDT_CheckColumn_ColumnOrder_CONSTRAINT_ID,
                                              sConstID,
                                              aEnv )
                         == STL_SUCCESS );

                /*
                 * DELETE FROM DEFINITION_SCHEMA.CHECK_TABLE_USAGE
                 *  WHERE CONSTRAINT_ID = sConstID;
                 */
    
                STL_TRY( eldDeleteObjectRows( aTransID,
                                              aStmt,
                                              ELDT_TABLE_ID_CHECK_TABLE_USAGE,
                                              ELDT_CheckTable_ColumnOrder_CONSTRAINT_ID,
                                              sConstID,
                                              aEnv )
                         == STL_SUCCESS );

                /*
                 * DELETE FROM DEFINITION_SCHEMA.CHECK_CONSTRAINTS
                 *  WHERE CONSTRAINT_ID = sConstID;
                 */
    
                STL_TRY( eldDeleteObjectRows( aTransID,
                                              aStmt,
                                              ELDT_TABLE_ID_CHECK_CONSTRAINTS,
                                              ELDT_CheckConst_ColumnOrder_CONSTRAINT_ID,
                                              sConstID,
                                              aEnv )
                         == STL_SUCCESS );
                
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }


    /*
     * DELETE FROM DEFINITION_SCHEMA.TABLE_CONSTRAINTS
     *  WHERE CONSTRAINT_ID = sConstID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                  ELDT_TableConst_ColumnOrder_CONSTRAINT_ID,
                                  sConstID,
                                  aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}






/**
 * @brief Constraint Characteristic Dictionary 정보를 변경한다.
 * @param[in] aTransID            Transaction ID
 * @param[in] aStmt               Statement
 * @param[in] aConstHandle        Constraint Dictionary Handle
 * @param[in] aIsDeferrable       DEFERRABLE / NOT DEFERRABLE
 * @param[in] aInitiallyDeferred  INITIALLY DEFERRED / INITIALLY IMMEDIATE
 * @param[in] aEnforced           ENFORCED / NOT ENFORCED
 * @param[in] aEnv                Environment
 * @remarks
 */
stlStatus eldModifyDictConstFeature( smlTransId       aTransID,
                                     smlStatement   * aStmt,
                                     ellDictHandle  * aConstHandle,
                                     stlBool          aIsDeferrable,
                                     stlBool          aInitiallyDeferred,
                                     stlBool          aEnforced,
                                     ellEnv         * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);
    
    stlInt64 sConstID = ELL_DICT_OBJECT_ID_NA;

    knlValueBlockList * sRowValueList = NULL;
    eldMemMark          sMemMark;
    stlBool             sMemAlloc = STL_FALSE;

    /**
     * 기본 정보 획득
     */

    sConstID = ellGetConstraintID( aConstHandle );
    
    /******************************************
     * 갱신을 위한 Record 삭제
     ******************************************/

    STL_TRY( eldDeleteDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                       ELDT_TableConst_ColumnOrder_CONSTRAINT_ID,
                                       sConstID,
                                       & sMemMark,
                                       & sRowValueList,
                                       aEnv )
             == STL_SUCCESS );
    sMemAlloc = STL_TRUE;

    /******************************************
     * 정보 변경
     ******************************************/
    
    /**
     * IS_DEFERRABLE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_IS_DEFERRABLE,
                                sRowValueList,
                                & aIsDeferrable,
                                aEnv )
             == STL_SUCCESS );

    /**
     * INITIALLY_DEFERRED
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_INITIALLY_DEFERRED,
                                sRowValueList,
                                & aInitiallyDeferred,
                                aEnv )
             == STL_SUCCESS );

    /**
     * ENFORCED
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_ENFORCED,
                                sRowValueList,
                                & aEnforced,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * MODIFIED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                ELDT_TableConst_ColumnOrder_MODIFIED_TIME,
                                sRowValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );
    

    /******************************************
     * 갱신된 Row 추가 
     ******************************************/

    sMemAlloc = STL_FALSE;
    STL_TRY( eldInsertDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                       & sMemMark,
                                       sRowValueList,
                                       aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if ( sMemAlloc == STL_TRUE )
    {
        (void) eldFreeTableValueList( & sMemMark, aEnv );
    }
    
    return STL_FAILURE;
}






/**
 * @brief Constraint ID 를 이용해 Const Dictionary Handle 을 획득 
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aConstID           Const ID
 * @param[out] aConstDictHandle   Const Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus eldGetConstHandleByID( smlTransId           aTransID,
                                 smlScn               aViewSCN,
                                 stlInt64             aConstID,
                                 ellDictHandle      * aConstDictHandle,
                                 stlBool            * aExist,
                                 ellEnv             * aEnv )
{
    return eldcGetConstHandleByID( aTransID,
                                   aViewSCN,
                                   aConstID,
                                   aConstDictHandle,
                                   aExist,
                                   aEnv );
}



/**
 * @brief Constraint Name을 이용해 Const Dictionary Handle 을 획득 
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aSchemaID          Schema ID
 * @param[in]  aConstName         Constraint Name
 * @param[out] aConstDictHandle   Constraint Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus eldGetConstHandleByName( smlTransId           aTransID,
                                   smlScn               aViewSCN,
                                   stlInt64             aSchemaID,
                                   stlChar            * aConstName,
                                   ellDictHandle      * aConstDictHandle,
                                   stlBool            * aExist,
                                   ellEnv             * aEnv )
{
    return eldcGetConstHandleByName( aTransID,
                                     aViewSCN,
                                     aSchemaID,
                                     aConstName,
                                     aConstDictHandle,
                                     aExist,
                                     aEnv );
}


/** @} eldTableConstraint */
