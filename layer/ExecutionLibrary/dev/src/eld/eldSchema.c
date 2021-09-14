/*******************************************************************************
 * eldSchema.c
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
 * @file eldSchema.c
 * @brief Schema Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldSchema.h>


/**
 * @addtogroup eldSchema
 * @{
 */

/**
 * @brief Schema Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertSchemaDesc() 주석 참조 
 */

stlStatus eldInsertSchemaDesc( smlTransId     aTransID,
                               smlStatement * aStmt,
                               stlInt64       aOwnerID,
                               stlInt64     * aSchemaID,
                               stlChar      * aSchemaName,
                               stlChar      * aSchemaComment,
                               ellEnv       * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    stlInt64                   sSchemaID = 0;
    
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid            sRowRid;
    smlScn            sRowScn;
    smlRowBlock       sRowBlock;
    eldMemMark        sMemMark;

    stlInt32 sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOwnerID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaID != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaName != NULL, ELL_ERROR_STACK(aEnv) );

    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_SCHEMATA,
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
     * - schemata descriptor의 정보를 구성한다.
     */

    /*
     * OWNER_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SCHEMATA,
                                ELDT_Schemata_ColumnOrder_OWNER_ID,
                                sDataValueList,
                                & aOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCHEMA_ID
     */

    if ( gEllIsMetaBuilding == STL_TRUE )
    {
        /* Database Dictionary 를 초기화하는 단계 */
        gEldtIdentityValue[ELDT_TABLE_ID_SCHEMATA] += 1;
        sSchemaID = gEldtIdentityValue[ELDT_TABLE_ID_SCHEMATA];
    }
    else
    {
        /* Database가 이미 구축되어 있는 단계 */
        
        /**
         * Identity Column으로부터 Schema ID를 획득
         */
        STL_TRY( smlGetNextSequenceVal( gEldIdentityColumnHandle[ELDT_TABLE_ID_SCHEMATA],
                                        & sSchemaID,
                                        SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        
    }

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SCHEMATA,
                                ELDT_Schemata_ColumnOrder_SCHEMA_ID,
                                sDataValueList,
                                & sSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCHEMA_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SCHEMATA,
                                ELDT_Schemata_ColumnOrder_SCHEMA_NAME,
                                sDataValueList,
                                aSchemaName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DEFAULT_CHARACTER_SET_CATALOG
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SCHEMATA,
                                ELDT_Schemata_ColumnOrder_DEFAULT_CHARACTER_SET_CATALOG,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * DEFAULT_CHARACTER_SET_SCHEMA
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SCHEMATA,
                                ELDT_Schemata_ColumnOrder_DEFAULT_CHARACTER_SET_SCHEMA,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * DEFAULT_CHARACTER_SET_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SCHEMATA,
                                ELDT_Schemata_ColumnOrder_DEFAULT_CHARACTER_SET_NAME,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SQL_PATH
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SCHEMATA,
                                ELDT_Schemata_ColumnOrder_SQL_PATH,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IS_BUILTIN
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SCHEMATA,
                                ELDT_Schemata_ColumnOrder_IS_BUILTIN,
                                sDataValueList,
                                & gEllIsMetaBuilding,
                                aEnv )
             == STL_SUCCESS );
    
    
    /*
     * CREATED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SCHEMATA,
                                ELDT_Schemata_ColumnOrder_CREATED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * MODIFIED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SCHEMATA,
                                ELDT_Schemata_ColumnOrder_MODIFIED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COMMENTS
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SCHEMATA,
                                ELDT_Schemata_ColumnOrder_COMMENTS,
                                sDataValueList,
                                aSchemaComment,
                                aEnv )
             == STL_SUCCESS );
        
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_SCHEMATA],
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
                                              ELDT_TABLE_ID_SCHEMATA,
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
    
    *aSchemaID = sSchemaID;
    
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
 * @brief SQL-Schema Cache 를 추가한다.
 * @param[in]  aTransID   Transaction ID
 * @param[in]  aStmt      Statement
 * @param[in]  aAuthID    Authorization ID
 * @param[in]  aSchemaID  Schema ID
 * @param[in]  aEnv       Environment
 * @remarks
 */
stlStatus eldRefineCache4AddSchema( smlTransId           aTransID,
                                    smlStatement       * aStmt,
                                    stlInt64             aAuthID,
                                    stlInt64             aSchemaID,
                                    ellEnv             * aEnv )
{


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );


    
    /**
     * SQL-Schema Cache 를 추가 
     */

    STL_TRY( eldcInsertCacheSchemaBySchemaID( aTransID,
                                              aStmt,
                                              aSchemaID,
                                              aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Schema ID 를 이용해 Schema Handle 을 얻는다.
 * @param[in]   aTransID          Transaction ID
 * @param[in]   aViewSCN          View SCN
 * @param[in]   aSchemaID         Schema ID
 * @param[out]  aSchemaDictHandle Schema Dictionary Handle
 * @param[out]  aExist            존재 여부
 * @param[in]   aEnv              Environment
 * @remarks
 */
stlStatus eldGetSchemaHandleByID( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  stlInt64             aSchemaID,
                                  ellDictHandle      * aSchemaDictHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv )
{
    return eldcGetSchemaHandleByID( aTransID,
                                    aViewSCN,
                                    aSchemaID,
                                    aSchemaDictHandle,
                                    aExist,
                                    aEnv );
}

/**
 * @brief Schema Name 를 이용해 Schema Handle 을 얻는다.
 * @param[in]   aTransID          Transaction ID
 * @param[in]   aViewSCN          View SCN
 * @param[in]   aSchemaName       Schema Name
 * @param[out]  aSchemaDictHandle Schema Dictionary Handle
 * @param[out]  aExist            존재 여부
 * @param[in]   aEnv              Environment
 * @remarks
 */
stlStatus eldGetSchemaHandleByName( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlChar            * aSchemaName,
                                    ellDictHandle      * aSchemaDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv )
{
    return eldcGetSchemaHandleByName( aTransID,
                                      aViewSCN,
                                      aSchemaName,
                                      aSchemaDictHandle,
                                      aExist,
                                      aEnv );
}



/** @} eldSchema */
