/*******************************************************************************
 * eldPublicSynonym.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: 
 *
 * NOTES
 *    
 *
 ******************************************************************************/


/**
 * @file eldPublicSynonym.c
 * @brief Public Synonym Dictionary 관리 루틴
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldPublicSynonym.h>

/**
 * @addtogroup eldPublicSynonym
 * @{
 */

/**
 * @brief Public Synonym Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertPublicSynonymDesc() 주석 참조
 */

stlStatus eldInsertPublicSynonymDesc( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      stlInt64             aSynonymID,
                                      stlChar            * aSynonymName,
                                      stlInt64             aCreatorID,
                                      stlChar            * aObjectSchemaName,
                                      stlChar            * aObjectName,
                                      ellEnv             * aEnv )
{
    ellSessionEnv      * sSessEnv = ELL_SESS_ENV(aEnv);
    knlValueBlockList  * sDataValueList  = NULL;
    eldMemMark           sMemMark;
    smlRowBlock          sRowBlock;
    smlRid               sRowRid;
    smlScn               sRowScn;

    stlInt32  sState = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectSchemaName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectName != NULL, ELL_ERROR_STACK(aEnv) );


    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_PUBLIC_SYNONYMS,
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
     * - public synonym descriptor의 정보를 구성한다.
     */
    
    /*
     * SYNONYM_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                ELDT_Public_Synonyms_ColumnOrder_SYNONYM_ID,
                                sDataValueList,
                                & aSynonymID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SYNONYM_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                ELDT_Public_Synonyms_ColumnOrder_SYNONYM_NAME,
                                sDataValueList,
                                aSynonymName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CREATOR_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                ELDT_Public_Synonyms_ColumnOrder_CREATOR_ID,
                                sDataValueList,
                                & aCreatorID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * OBJECT_SCHEMA_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                ELDT_Public_Synonyms_ColumnOrder_OBJECT_SCHEMA_NAME,
                                sDataValueList,
                                aObjectSchemaName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * OBJECT_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                ELDT_Public_Synonyms_ColumnOrder_OBJECT_NAME,
                                sDataValueList,
                                aObjectName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_BUILTIN
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                ELDT_Public_Synonyms_ColumnOrder_IS_BUILTIN,
                                sDataValueList,
                                & gEllIsMetaBuilding,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * CREATED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                ELDT_Public_Synonyms_ColumnOrder_CREATED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * MODIFIED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                ELDT_Public_Synonyms_ColumnOrder_MODIFIED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_PUBLIC_SYNONYMS],
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
                                              ELDT_TABLE_ID_PUBLIC_SYNONYMS,
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
 * @brief 시스템으로부터 새로운 Public Synonym ID 를 획득한다.
 * @param[out] aSynonymID  Synonym ID
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus eldGetNewPublicSynonymID( stlInt64 * aSynonymID,
                                    ellEnv   * aEnv )
{
    stlInt64 sSynonymID = 0;

    if ( gEllIsMetaBuilding == STL_TRUE )
    {
        /* Database Dictionary 를 초기화하는 단계 */
        gEldtIdentityValue[ELDT_TABLE_ID_PUBLIC_SYNONYMS] += 1;
        sSynonymID = gEldtIdentityValue[ELDT_TABLE_ID_PUBLIC_SYNONYMS];
    }
    else
    {
        /* Database가 이미 구축되어 있는 단계 */
        
        /**
         * Identity Column으로부터 Synonym ID를 획득
         */
        STL_TRY( smlGetNextSequenceVal( gEldIdentityColumnHandle[ELDT_TABLE_ID_PUBLIC_SYNONYMS],
                                        & sSynonymID,
                                        SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        
    }

    /**
     * Output 설정
     */

    *aSynonymID = sSynonymID;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief SQL-PublicSynonym Cache 를 추가한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aSynonymID    Synonym ID
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldRefineCache4AddPublicSynonym( smlTransId           aTransID,
                                           smlStatement       * aStmt,
                                           stlInt64             aSynonymID,
                                           ellEnv             * aEnv )
{
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );

    /**
     * SQL-Synonym Cache 를 추가 
     */

    STL_TRY( eldcInsertCachePublicSynonymBySynonymID( aTransID,
                                                      aStmt,
                                                      aSynonymID,
                                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
    
}

/**
 * @brief Public Synonym Dictionary 정보만을 삭제한다.
 * @param[in] aTransID              Transaction ID
 * @param[in] aStmt                 Statement
 * @param[in] aSynonymDictHandle    Synonym Dictionary Handle
 * @param[in] aEnv                  Environment
 * @remarks
 */
stlStatus eldDeleteDictPublicSynonym( smlTransId       aTransID,
                                      smlStatement   * aStmt,
                                      ellDictHandle  * aSynonymDictHandle,
                                      ellEnv         * aEnv )
{
    stlInt64 sSynonymID = ELL_DICT_OBJECT_ID_NA;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Synonym ID 획득
     */
    
    sSynonymID = ellGetPublicSynonymID( aSynonymDictHandle );
    
    /**
     * Synonym Dictionary Record 를 삭제
     */
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.PUBLIC_SYNONYMS
     *  WHERE SYNONYM_ID = sSynonymID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                  ELDT_Public_Synonyms_ColumnOrder_SYNONYM_ID,
                                  sSynonymID,
                                  aEnv )
             == STL_SUCCESS );
   
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

 /**
 * @brief Public Synonyme Name 을 이용해 Public Synonym Dictionary Handle 을 획득 
 * @param[in]  aTransID                  Transaction ID
 * @param[in]  aViewSCN                  View SCN
 * @param[in]  aSynonymName              Synonym Name
 * @param[out] aSynonymDictHandle        Synonym Dictionary Handle
 * @param[out] aExist                    존재 여부 
 * @param[in]  aEnv                      Envirionment 
 * @remarks
 */
stlStatus eldGetPublicSynonymHandleByName( smlTransId           aTransID,
                                           smlScn               aViewSCN,
                                           stlChar            * aSynonymName,
                                           ellDictHandle      * aSynonymDictHandle,
                                           stlBool            * aExist,
                                           ellEnv             * aEnv )
{
    return eldcGetPublicSynonymHandleByName( aTransID,
                                             aViewSCN,
                                             aSynonymName,
                                             aSynonymDictHandle,
                                             aExist,
                                             aEnv );
}

/** @} eldPublicSynonym */
