/*******************************************************************************
 * eldSequence.c
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
 * @file eldSequence.c
 * @brief Sequence Dictionary 관리 루틴
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldSequence.h>

/**
 * @addtogroup eldSequence
 * @{
 */

/**
 * @brief Sequence Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertSequenceDesc() 주석 참조
*/

stlStatus eldInsertSequenceDesc( smlTransId           aTransID,
                                 smlStatement       * aStmt,
                                 stlInt64             aOwnerID,
                                 stlInt64             aSchemaID,
                                 stlInt64           * aSequenceID,
                                 stlInt64             aSeqTableID,
                                 stlInt64             aTablespaceID,
                                 stlInt64             aPhysicalID,
                                 stlChar            * aSequenceName,
                                 stlInt64             aStart,
                                 stlInt64             aMinimum,
                                 stlInt64             aMaximum,
                                 stlInt64             aIncrement,
                                 stlBool              aIsCycle,
                                 stlInt32             aCacheSize,
                                 stlChar            * aComment,
                                 ellEnv             * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);
    
    stlInt64                   sSequenceID = 0;
    
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
    STL_PARAM_VALIDATE( aOwnerID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSequenceID != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqTableID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSequenceName != NULL, ELL_ERROR_STACK(aEnv) );

    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_SEQUENCES,
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
     * - sequence descriptor의 정보를 구성한다.
     */

    /*
     * OWNER_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_OWNER_ID,
                                sDataValueList,
                                & aOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCHEMA_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_SCHEMA_ID,
                                sDataValueList,
                                & aSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SEQUENCE_ID
     */

    if ( gEllIsMetaBuilding == STL_TRUE )
    {
        /* Database Dictionary 를 초기화하는 단계 */
        gEldtIdentityValue[ELDT_TABLE_ID_SEQUENCES] += 1;
        sSequenceID = gEldtIdentityValue[ELDT_TABLE_ID_SEQUENCES];
    }
    else
    {
        /* Database가 이미 구축되어 있는 단계 */
        
        /**
         * Identity Column으로부터 Sequence ID를 획득
         */
        STL_TRY( smlGetNextSequenceVal( gEldIdentityColumnHandle[ELDT_TABLE_ID_SEQUENCES],
                                        & sSequenceID,
                                        SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        
    }

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_SEQUENCE_ID,
                                sDataValueList,
                                & sSequenceID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SEQUENCE_TABLE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_SEQUENCE_TABLE_ID,
                                sDataValueList,
                                & aSeqTableID,
                                aEnv )
             == STL_SUCCESS );
    

    /*
     * TABLESPACE_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_TABLESPACE_ID,
                                sDataValueList,
                                & aTablespaceID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * PHYSICAL_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_PHYSICAL_ID,
                                sDataValueList,
                                & aPhysicalID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SEQUENCE_NAME
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_SEQUENCE_NAME,
                                sDataValueList,
                                aSequenceName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DTD_IDENTIFIER
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_DTD_IDENTIFIER,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * START_VALUE
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_START_VALUE,
                                sDataValueList,
                                & aStart,
                                aEnv )
             == STL_SUCCESS );

    /*
     * MINIMUM_VALUE
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_MINIMUM_VALUE,
                                sDataValueList,
                                & aMinimum,
                                aEnv )
             == STL_SUCCESS );

    /*
     * MAXIMUM_VALUE
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_MAXIMUM_VALUE,
                                sDataValueList,
                                & aMaximum,
                                aEnv )
             == STL_SUCCESS );

    /*
     * INCREMENT
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_INCREMENT,
                                sDataValueList,
                                & aIncrement,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CYCLE_OPTION
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_CYCLE_OPTION,
                                sDataValueList,
                                & aIsCycle,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CACHE_SIZE
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_CACHE_SIZE,
                                sDataValueList,
                                & aCacheSize,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IS_BUILTIN
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_IS_BUILTIN,
                                sDataValueList,
                                & gEllIsMetaBuilding,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * CREATED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_CREATED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * MODIFIED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_MODIFIED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COMMENTS
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_COMMENTS,
                                sDataValueList,
                                aComment,
                                aEnv )
             == STL_SUCCESS );
        
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_SEQUENCES],
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
                                              ELDT_TABLE_ID_SEQUENCES,
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
    
    *aSequenceID = sSequenceID;
    
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
 * @brief SQL-Sequence Cache 를 추가한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aAuthID       Authorization ID
 * @param[in]  aSequenceID   Sequence ID
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldRefineCache4AddSequence( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      stlInt64             aAuthID,
                                      stlInt64             aSequenceID,
                                      ellEnv             * aEnv )
{
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSequenceID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );

    /**
     * SQL-Sequence Cache 를 추가 
     */

    STL_TRY( eldcInsertCacheSequenceBySequenceID( aTransID,
                                                  aStmt,
                                                  aSequenceID,
                                                  aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
    
}


/**
 * @brief Sequence Dictionary 정보만을 삭제한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aSeqDictHandle    Sequence Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldDeleteDictSequence( smlTransId       aTransID,
                                 smlStatement   * aStmt,
                                 ellDictHandle  * aSeqDictHandle,
                                 ellEnv         * aEnv )
{
    stlInt64 sSeqID = ELL_DICT_OBJECT_ID_NA;
    stlInt64 sSeqTableID = ELL_DICT_OBJECT_ID_NA;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Sequence ID 획득
     */
    
    sSeqID = ellGetSequenceID( aSeqDictHandle );
    sSeqTableID = ellGetSeqTableID( aSeqDictHandle );
    
    /**
     * Sequence Privilege Record 를 삭제
     */
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.USAGE_PRIVILEGES
     *  WHERE OBJECT_ID = sSeqID;
     */
    
    STL_TRY( eldDeletePrivRowsAndCache( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_USAGE_PRIVILEGES,
                                        ELDT_UsagePriv_ColumnOrder_OBJECT_ID,
                                        sSeqID,
                                        aEnv )
             == STL_SUCCESS );

    /**
     * Sequence Dictionary Record 를 삭제
     */
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.SEQUENCES
     *  WHERE SEQUENCE_ID = sSeqID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_SEQUENCES,
                                  ELDT_Sequences_ColumnOrder_SEQUENCE_ID,
                                  sSeqID,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * Naming Resolution 을 위해 추가된 Table Dictionary Record 를 삭제
     */
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.TABLES
     *  WHERE TABLE_ID = sSeqTableID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_TABLES,
                                  ELDT_Tables_ColumnOrder_TABLE_ID,
                                  sSeqTableID,
                                  aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief Sequence Option 을 Dictionary 에서 변경한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aSequenceID    Sequence ID
 * @param[in]  aPhysicalID    Sequence 의 물리적 ID
 * @param[in]  aStart         변경할 Sequence의 시작값
 * @param[in]  aMinimum       변경할 Sequence의 최소값 
 * @param[in]  aMaximum       변경할 Sequence의 최대값
 * @param[in]  aIncrement     변경할 Sequence의 증가값
 * @param[in]  aIsCycle       변경할 Sequence의 Cycle 허용 여부
 * @param[in]  aCacheSize     변경할 Sequence의 Cache Size
 * @param[in]  aEnv           Execution Library Environment
 * @remarks
 */
stlStatus eldModifySequenceOption( smlTransId       aTransID,
                                   smlStatement   * aStmt,
                                   stlInt64         aSequenceID,
                                   stlInt64         aPhysicalID,
                                   stlInt64         aStart,
                                   stlInt64         aMinimum,
                                   stlInt64         aMaximum,
                                   stlInt64         aIncrement,
                                   stlBool          aIsCycle,
                                   stlInt32         aCacheSize,
                                   ellEnv         * aEnv )
{
    knlValueBlockList * sRowValueList = NULL;

    eldMemMark          sMemMark;

    stlBool             sMemAlloc = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSequenceID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );

    /**
     * 갱신을 위한 Record 삭제
     */

    STL_TRY( eldDeleteDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_SEQUENCES,
                                       ELDT_Sequences_ColumnOrder_SEQUENCE_ID,
                                       aSequenceID,
                                       & sMemMark,
                                       & sRowValueList,
                                       aEnv )
             == STL_SUCCESS );
    sMemAlloc = STL_TRUE;
    
    /**
     * SEQUENCE Option 정보 설정
     */
    
    STL_TRY( eldSetSequenceOption( sRowValueList,
                                   aPhysicalID,
                                   aStart,
                                   aMinimum,
                                   aMaximum,
                                   aIncrement,
                                   aIsCycle,
                                   aCacheSize,
                                   aEnv )
             == STL_SUCCESS );
            
    /**
     * 갱신된 Row 추가 
     */

    sMemAlloc = STL_FALSE;
    STL_TRY( eldInsertDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_SEQUENCES,
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
 * @brief Sequence Option 을 Value Block List 에 설정함
 * @param[in]  aDataValueList  Data Value List
 * @param[in]  aPhysicalID     Sequence 의 물리적 ID
 * @param[in]  aStart          변경할 Sequence의 시작값
 * @param[in]  aMinimum        변경할 Sequence의 최소값 
 * @param[in]  aMaximum        변경할 Sequence의 최대값
 * @param[in]  aIncrement      변경할 Sequence의 증가값
 * @param[in]  aIsCycle        변경할 Sequence의 Cycle 허용 여부
 * @param[in]  aCacheSize      변경할 Sequence의 Cache Size
 * @param[in]  aEnv            Execution Library Environment
 * @remarks
 */
stlStatus eldSetSequenceOption( knlValueBlockList        * aDataValueList,
                                stlInt64                   aPhysicalID,
                                stlInt64                   aStart,
                                stlInt64                   aMinimum,
                                stlInt64                   aMaximum,
                                stlInt64                   aIncrement,
                                stlBool                    aIsCycle,
                                stlInt32                   aCacheSize,
                                ellEnv                   * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDataValueList != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * PHYSICAL_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_PHYSICAL_ID,
                                aDataValueList,
                                & aPhysicalID,
                                aEnv )
             == STL_SUCCESS );

    /**
     * START_VALUE
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_START_VALUE,
                                aDataValueList,
                                & aStart,
                                aEnv )
             == STL_SUCCESS );

    /**
     * MINIMUM_VALUE
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_MINIMUM_VALUE,
                                aDataValueList,
                                & aMinimum,
                                aEnv )
             == STL_SUCCESS );

    /**
     * MAXIMUM_VALUE
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_MAXIMUM_VALUE,
                                aDataValueList,
                                & aMaximum,
                                aEnv )
             == STL_SUCCESS );

    /**
     * INCREMENT
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_INCREMENT,
                                aDataValueList,
                                & aIncrement,
                                aEnv )
             == STL_SUCCESS );

    /**
     * CYCLE_OPTION
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_CYCLE_OPTION,
                                aDataValueList,
                                & aIsCycle,
                                aEnv )
             == STL_SUCCESS );

    /**
     * CACHE_SIZE
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_CACHE_SIZE,
                                aDataValueList,
                                & aCacheSize,
                                aEnv )
             == STL_SUCCESS );

    /**
     * MODIFIED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SEQUENCES,
                                ELDT_Sequences_ColumnOrder_MODIFIED_TIME,
                                aDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
    
}











/**
 * @brief Sequence ID 를 이용해 Sequence Dictionary Handle 을 획득 
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             View SCN
 * @param[in]  aSequenceID          Sequence ID
 * @param[out] aSequenceDictHandle  Sequence Dictionary Handle
 * @param[out] aExist               존재 여부 
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus eldGetSequenceHandleByID( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aSequenceID,
                                    ellDictHandle      * aSequenceDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv )
{
    return eldcGetSequenceHandleByID( aTransID,
                                      aViewSCN,
                                      aSequenceID,
                                      aSequenceDictHandle,
                                      aExist,
                                      aEnv );
}

/**
 * @brief Schema ID 와 Sequence Name 을 이용해 Sequence Dictionary Handle 을 획득 
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             View SCN
 * @param[in]  aSchemaID            Schema ID
 * @param[in]  aSequenceName        Sequence Name
 * @param[out] aSequenceDictHandle  Sequence Dictionary Handle
 * @param[out] aExist               존재 여부 
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus eldGetSequenceHandleByName( smlTransId           aTransID,
                                      smlScn               aViewSCN,
                                      stlInt64             aSchemaID,
                                      stlChar            * aSequenceName,
                                      ellDictHandle      * aSequenceDictHandle,
                                      stlBool            * aExist,
                                      ellEnv             * aEnv )
{
    return eldcGetSequenceHandleByName( aTransID,
                                        aViewSCN,
                                        aSchemaID,
                                        aSequenceName,
                                        aSequenceDictHandle,
                                        aExist,
                                        aEnv );
}



/** @} eldSequence */
