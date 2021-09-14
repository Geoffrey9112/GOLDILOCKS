/*******************************************************************************
 * eldIndex.c
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
 * @file eldIndex.c
 * @brief Index Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldIndex.h>

/**
 * @addtogroup eldIndex
 * @{
 */

/**
 * @brief Index Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertIndexDesc() 주석 참조 
 */

stlStatus eldInsertIndexDesc( smlTransId     aTransID,
                              smlStatement * aStmt,
                              stlInt64       aOwnerID,
                              stlInt64       aSchemaID,
                              stlInt64     * aIndexID,
                              stlInt64       aTablespaceID,
                              stlInt64       aPhysicalID,
                              stlChar      * aIndexName,
                              ellIndexType   aIndexType,
                              stlBool        aUnique,
                              stlBool        aInvalid,
                              stlBool        aByConstraint,
                              stlChar      * aIndexComment,
                              ellEnv       * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    stlInt64                   sIndexID = 0;
    
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
    STL_PARAM_VALIDATE( aOwnerID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexID != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIndexType > ELL_INDEX_TYPE_NA ) &&
                        ( aIndexType < ELL_INDEX_TYPE_MAX ),
                        ELL_ERROR_STACK(aEnv) );

    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_INDEXES,
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
     * - index descriptor의 정보를 구성한다.
     */


    /*
     * OWNER_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_OWNER_ID,
                                sDataValueList,
                                & aOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCHEMA_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_SCHEMA_ID,
                                sDataValueList,
                                & aSchemaID,
                                aEnv )
             == STL_SUCCESS );


    /*
     * INDEX_ID
     */

    if ( gEllIsMetaBuilding == STL_TRUE )
    {
        /* Database Dictionary 를 초기화하는 단계 */
        gEldtIdentityValue[ELDT_TABLE_ID_INDEXES] += 1;
        sIndexID = gEldtIdentityValue[ELDT_TABLE_ID_INDEXES];
    }
    else
    {
        /* Database가 이미 구축되어 있는 단계 */
        
        /**
         * Identity Column으로부터 Index ID를 획득
         */
        STL_TRY( smlGetNextSequenceVal( gEldIdentityColumnHandle[ELDT_TABLE_ID_INDEXES],
                                        & sIndexID,
                                        SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        
    }

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_INDEX_ID,
                                sDataValueList,
                                & sIndexID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * TABLESPACE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_TABLESPACE_ID,
                                sDataValueList,
                                & aTablespaceID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * PHYSICAL_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_PHYSICAL_ID,
                                sDataValueList,
                                & aPhysicalID,
                                aEnv )
             == STL_SUCCESS );

    
    /*
     * INDEX_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_INDEX_NAME,
                                sDataValueList,
                                aIndexName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * INDEX_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_INDEX_TYPE,
                                sDataValueList,
                                (void*) gEllIndexTypeString[aIndexType],
                                aEnv )
             == STL_SUCCESS );

    /*
     * INDEX_TYPE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_INDEX_TYPE_ID,
                                sDataValueList,
                                & aIndexType,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * IS_UNIQUE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_IS_UNIQUE,
                                sDataValueList,
                                & aUnique,
                                aEnv )
             == STL_SUCCESS );

    /*
     * INVALID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_INVALID,
                                sDataValueList,
                                & aInvalid,
                                aEnv )
             == STL_SUCCESS );

    /*
     * BY_CONSTRAINT
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_BY_CONSTRAINT,
                                sDataValueList,
                                & aByConstraint,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IS_BUILTIN
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_IS_BUILTIN,
                                sDataValueList,
                                & gEllIsMetaBuilding,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * CREATED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_CREATED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * MODIFIED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_MODIFIED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COMMENTS
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_COMMENTS,
                                sDataValueList,
                                aIndexComment,
                                aEnv )
             == STL_SUCCESS );
        
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_INDEXES],
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
                                              ELDT_TABLE_ID_INDEXES,
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
    
    *aIndexID = sIndexID;
    
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
 * @brief DEFINITION_SCHEMA 구축후 인덱스 데이타를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 * 최초 DB 생성시에만 사용되는 함수임.
 */
stlStatus eldFinishBuildAllIndexData( ellEnv * aEnv )
{
    stlInt32  sState = 0;
    
    stlInt32  i = 0;
    
    smlTransId          sTransID  = SML_INVALID_TRANSID;
    smlStatement      * sStmt     = NULL; 

    /**
     * Storage Component 할당
     */
    
    /* Transaction */
    STL_TRY( smlBeginTrans( SML_TIL_READ_COMMITTED,
                            STL_FALSE,  /* aIsGlobalTrans */
                            & sTransID,
                            SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 2;

    /* Statement */
    STL_TRY( smlAllocStatement( sTransID,
                                NULL, /* aUpdateRelHandle */
                                SML_STMT_ATTR_UPDATABLE,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE, /* aNeedSnapshotIterator */
                                & sStmt,
                                SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 3;

    /**
     * 각 인덱스에 대하여 데이타를 구축
     */
    
    for ( i = ELL_DICT_OBJECT_ID_NA + 1; i <= gEldtIndexCnt; i++ )
    {
        STL_TRY( eldFinishBuildOneIndexData( sStmt,
                                             & gEldtIndexInfo[i],
                                             aEnv )
                 == STL_SUCCESS );
        STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    }
    

    
    /**
     * Storage Component 해제 
     */

    /* Statement */
    sState = 2;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sStmt = NULL;
    
    /* Transaction */
    sState = 1;
    STL_TRY( smlCommit( sTransID,
                        NULL, /* aComment */
                        SML_TRANSACTION_CWM_WAIT,
                        SML_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 0;
    STL_TRY( smlEndTrans( sTransID,
                          SML_PEND_ACTION_COMMIT,
                          SML_TRANSACTION_CWM_WAIT,
                          SML_ENV( aEnv ) ) == STL_SUCCESS );
    sTransID = SML_INVALID_TRANSID;
             
    return STL_SUCCESS;

    STL_FINISH;

    switch ( sState )
    {
        case 3:
            (void) smlFreeStatement( sStmt, SML_ENV( aEnv ) );
        case 2:
            (void) smlRollback( sTransID,
                                SML_TRANSACTION_CWM_NO_WAIT,
                                SML_ENV( aEnv ) );
        case 1:
            (void) smlEndTrans( sTransID,
                                SML_PEND_ACTION_ROLLBACK,
                                SML_TRANSACTION_CWM_NO_WAIT,
                                SML_ENV( aEnv ) );
        default:
            break;
            
    }

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief 하나의 인덱스에 대한 데이터를 구축한다.
 * @param[in] aStmt        Statement
 * @param[in] aIndexInfo   DEFINITION_SCHEMA 의 Index Physical Information
 * @param[in] aEnv         Environment
 * @remarks
 */
stlStatus eldFinishBuildOneIndexData( smlStatement                    * aStmt,
                                      eldtDefinitionPhysicalIndexInfo * aIndexInfo,
                                      ellEnv                          * aEnv )
{
    stlInt32   i = 0;
    
    eldtDefinitionKeyConstDesc * sKeyConstDesc = NULL;
    eldtDefinitionIndexDesc    * sIndexDesc = NULL;
    
    knlValueBlockList * sTableValueList = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    
    void              * sTableHandle = NULL;
    void              * sIndexHandle = NULL;

    stlUInt16          sKeyCount = 0;
    stlUInt8         * sKeyColFlags = NULL;
    stlUInt8           sKeyFlag = 0;
    eldMemMark         sMemMark;

    stlInt32 sState = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexInfo != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * - Data Value List 를 위한 공간 할당 및 초기화
     */

    if ( aIndexInfo->mIsKeyConst == STL_TRUE )
    {
        sKeyConstDesc = (eldtDefinitionKeyConstDesc *) aIndexInfo->mDescInfo;

        sTableHandle = gEldTablePhysicalHandle[sKeyConstDesc->mTableID];
        sIndexHandle = aIndexInfo->mIndexHandle;

        sKeyCount = sKeyConstDesc->mKeyCount;
            
        /**
         * Table Value Array 할당
         */
        STL_TRY( eldAllocTableValueList( sKeyConstDesc->mTableID,
                                         & sMemMark,
                                         & sTableValueList,
                                         aEnv )
                 == STL_SUCCESS );
        sState = 1;
        
        /**
         * Index Value Array 할당 
         */
        STL_TRY( eldShareKeyConstIndexValueList( sKeyConstDesc,
                                                 sTableValueList,
                                                 & sIndexValueList,
                                                 aEnv )
                     == STL_SUCCESS );

        /**
         * Index Key Column Flag
         */
        
        if ( sKeyConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY )
        {
            sKeyFlag =
                DTL_KEYCOLUMN_INDEX_ORDER_ASC |
                DTL_KEYCOLUMN_INDEX_NULLABLE_FALSE |
                DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST;
        }
        else
        {
            sKeyFlag =
                DTL_KEYCOLUMN_INDEX_ORDER_ASC |
                DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE |
                DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST;
        }
        
    }
    else
    {
        sIndexDesc = (eldtDefinitionIndexDesc *) aIndexInfo->mDescInfo;

        sTableHandle = gEldTablePhysicalHandle[sIndexDesc->mTableID];
        sIndexHandle = aIndexInfo->mIndexHandle;

        sKeyCount = sIndexDesc->mKeyCount;
        
        /**
         * Table Value Array 할당
         */
        STL_TRY( eldAllocTableValueList( sIndexDesc->mTableID,
                                         & sMemMark,
                                         & sTableValueList,
                                         aEnv )
                 == STL_SUCCESS );
        sState = 1;
        
        /**
         * Index Value Array 할당 
         */
        STL_TRY( eldShareNormalIndexValueList( sIndexDesc,
                                               sTableValueList,
                                               & sIndexValueList,
                                               aEnv )
                     == STL_SUCCESS );

        /**
         * Index Key Column Flag
         */
        
        sKeyFlag =
            DTL_KEYCOLUMN_INDEX_ORDER_ASC |
            DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE |
            DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST;
    }


    /**
     * 모든 컬럼의 Key Flag 설정
     */
    
    STL_TRY( eldAllocOPMem( (void**) & sKeyColFlags,
                            sKeyCount * STL_SIZEOF(stlUInt8),
                            aEnv )
             == STL_SUCCESS );
    
    for ( i = 0; i < sKeyCount; i++ )
    {
        sKeyColFlags[i] = sKeyFlag;
    }
    
    /**
     * 인덱스 데이타를 구축 
     */

    STL_TRY( smlBuildIndex( aStmt,
                            sIndexHandle,
                            sTableHandle,
                            sKeyCount,
                            sIndexValueList,
                            sKeyColFlags,
                            0, /* aParallelFactor */
                            SML_ENV(aEnv) )
             == STL_SUCCESS );
    
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
 * @brief SQL-Index Cache 를 추가한다.
 * @param[in] aTransID    Transaction ID
 * @param[in] aStmt       Statement
 * @param[in] aAuthID     Authorization ID
 * @param[in] aIndexID    Index ID
 * @param[in] aEnv        Environment
 * @remarks
 */
stlStatus eldRefineCache4AddIndex( smlTransId           aTransID,
                                   smlStatement       * aStmt,
                                   stlInt64             aAuthID,
                                   stlInt64             aIndexID,
                                   ellEnv             * aEnv )
{
    stlBool         sIndexExist = STL_FALSE;
    
    ellDictHandle   sIndexHandle;
    stlMemset( & sIndexHandle, 0x00, STL_SIZEOF(ellDictHandle) );

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );

    /**
     * SQL-Index Cache 를 추가 
     */

    STL_TRY( eldcInsertCacheIndexByIndexID( aTransID,
                                            aStmt,
                                            aIndexID,
                                            aEnv )
             == STL_SUCCESS );

    /**
     * Unique Index 인지 검사
     */

    STL_TRY( eldGetIndexHandleByID( aTransID,
                                    SML_MAXIMUM_STABLE_SCN,
                                    aIndexID,
                                    & sIndexHandle,
                                    & sIndexExist,
                                    aEnv )
             == STL_SUCCESS );
    STL_ASSERT( sIndexExist == STL_TRUE );
    
    if ( ellGetIndexIsUnique( & sIndexHandle ) == STL_TRUE )
    {
        if ( ellGetIndexIsByConstraint( & sIndexHandle ) == STL_TRUE )
        {
            /**
             * Unique Key 를 위해 생성된 Unique Index 는
             * Table 의 Unique Key Constraint 정보로 등록된다.
             * - nothing to do
             */

        }
        else
        {
            /**
             * 관련 Table 에 Unique Index 정보를 등록
             */
            
            STL_TRY( eldcAddUniqueIndexIntoTable( aTransID,
                                                  aIndexID,
                                                  aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /**
         * 관련 Table 에 Non-Unique Index 정보를 등록
         */
        
        STL_TRY( eldcAddNonUniqueIndexIntoTable( aTransID,
                                                 aIndexID,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}






/**
 * @brief Index Dictionary 정보만을 삭제한다.
 * @param[in] aTransID         Transaction ID
 * @param[in] aStmt            Statement
 * @param[in] aIndexDictHandle Index Dictionary Handle
 * @param[in] aEnv             Environment
 * @remarks
 */
stlStatus eldDeleteDictIndex( smlTransId       aTransID,
                              smlStatement   * aStmt,
                              ellDictHandle  * aIndexDictHandle,
                              ellEnv         * aEnv )
{
    stlInt64 sIndexID = ELL_DICT_OBJECT_ID_NA;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Index ID 획득
     */
    
    sIndexID = ellGetIndexID( aIndexDictHandle );

    /**
     * Index 관련 Dictionary Record 정보를 삭제
     */
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE
     *  WHERE INDEX_ID = sIndexID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,
                                  ELDT_IndexKey_ColumnOrder_INDEX_ID,
                                  sIndexID,
                                  aEnv )
             == STL_SUCCESS );
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE
     *  WHERE INDEX_ID = sIndexID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,
                                  ELDT_IndexTable_ColumnOrder_INDEX_ID,
                                  sIndexID,
                                  aEnv )
             == STL_SUCCESS );
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.INDEXES
     *  WHERE INDEX_ID = sIndexID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_INDEXES,
                                  ELDT_Indexes_ColumnOrder_INDEX_ID,
                                  sIndexID,
                                  aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}























/**
 * @brief Index ID 를 이용해 Index Dictionary Handle 을 획득 
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aIndexID           Index ID
 * @param[out] aIndexDictHandle   Index Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus eldGetIndexHandleByID( smlTransId           aTransID,
                                 smlScn               aViewSCN,
                                 stlInt64             aIndexID,
                                 ellDictHandle      * aIndexDictHandle,
                                 stlBool            * aExist,
                                 ellEnv             * aEnv )
{
    return eldcGetIndexHandleByID( aTransID,
                                   aViewSCN,
                                   aIndexID,
                                   aIndexDictHandle,
                                   aExist,
                                   aEnv );
}

/**
 * @brief Index Name을 이용해 Index Dictionary Handle 을 획득 
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aSchemaID          Schema ID
 * @param[in]  aIndexName         Index Name
 * @param[out] aIndexDictHandle   Index Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus eldGetIndexHandleByName( smlTransId           aTransID,
                                   smlScn               aViewSCN,
                                   stlInt64             aSchemaID,
                                   stlChar            * aIndexName,
                                   ellDictHandle      * aIndexDictHandle,
                                   stlBool            * aExist,
                                   ellEnv             * aEnv )
{
    return eldcGetIndexHandleByName( aTransID,
                                     aViewSCN,
                                     aSchemaID,
                                     aIndexName,
                                     aIndexDictHandle,
                                     aExist,
                                     aEnv );
}




/** @} eldIndex */
