/*******************************************************************************
 * eldTable.c
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
 * @file eldTable.c
 * @brief Table Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldColumn.h>
#include <eldIndex.h>
#include <eldTableConstraint.h>
#include <eldTable.h>


/**
 * @addtogroup eldTable
 * @{
 */

/***********************************************************************
 * Dictionary Row 변경 함수 
 ***********************************************************************/

/**
 * @brief Table Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertTableDesc() 주석 참조 
 */

stlStatus eldInsertTableDesc( smlTransId     aTransID,
                              smlStatement * aStmt,
                              stlInt64       aOwnerID,
                              stlInt64       aSchemaID,
                              stlInt64     * aTableID,
                              stlInt64       aTablespaceID,
                              stlInt64       aPhysicalID,
                              stlChar      * aTableName,
                              ellTableType   aTableType,
                              stlChar      * aTableComment,
                              ellEnv       * aEnv )
{
    stlInt64                   sTableID = 0;
    
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
    STL_PARAM_VALIDATE( aOwnerID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aTableType > ELL_TABLE_TYPE_NA ) &&
                        ( aTableType < ELL_TABLE_TYPE_MAX ),
                        ELL_ERROR_STACK(aEnv) );

    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_TABLES,
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
     * Table ID 획득
     */
    
    STL_TRY( eldGetNewTableID( & sTableID, aEnv ) == STL_SUCCESS );

    /**
     * - table descriptor의 정보를 구성한다.
     */
    
    STL_TRY( eldMakeValueList4TableDesc( sDataValueList,
                                         aOwnerID,
                                         aSchemaID,
                                         sTableID,
                                         aTablespaceID,
                                         aPhysicalID,
                                         aTableName,
                                         aTableType,
                                         aTableComment,
                                         aEnv )
             == STL_SUCCESS );
    
    /**
     * - 레코드를 넣는다.
     */

    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_TABLES],
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
                                              ELDT_TABLE_ID_TABLES,
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

    *aTableID = sTableID;
    
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
 * @brief 시스템으로부터 Table ID 를 획득한다.
 * @param[out] aTableID  Table ID
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus eldGetNewTableID( stlInt64 * aTableID,  ellEnv * aEnv )
{
    stlInt64 sTableID = 0;
    
    if ( gEllIsMetaBuilding == STL_TRUE )
    {
        /* Database Dictionary 를 초기화하는 단계 */
        gEldtIdentityValue[ELDT_TABLE_ID_TABLES] += 1;
        sTableID = gEldtIdentityValue[ELDT_TABLE_ID_TABLES];
    }
    else
    {
        /* Database가 이미 구축되어 있는 단계 */
        
        /**
         * Identity Column으로부터 Table ID를 획득
         */
        STL_TRY( smlGetNextSequenceVal( gEldIdentityColumnHandle[ELDT_TABLE_ID_TABLES],
                                        & sTableID,
                                        SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        
    }

    /**
     * Output 설정
     */

    *aTableID = sTableID;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Table Descriptor 를 위한 Data Value List 를 구성한다.
 * @param[in]  aDataValueList   Data Value List
 * @param[in]  aOwnerID         Owner의 ID
 * @param[in]  aSchemaID        Schema ID
 * @param[in]  aTableID         Table 의 ID 
 * @param[in]  aTablespaceID    Tablespace ID
 * @param[in]  aPhysicalID      Table 의 Physical ID
 * @param[in]  aTableName       Table Name
 * @param[in]  aTableType       Table Type 
 * @param[in]  aTableComment    Table Comment (nullable)
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus eldMakeValueList4TableDesc( knlValueBlockList  * aDataValueList,
                                      stlInt64             aOwnerID,
                                      stlInt64             aSchemaID,
                                      stlInt64             aTableID,
                                      stlInt64             aTablespaceID,
                                      stlInt64             aPhysicalID,
                                      stlChar            * aTableName,
                                      ellTableType         aTableType,
                                      stlChar            * aTableComment,
                                      ellEnv             * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    stlBool         sBoolValue = STL_FALSE;
    
    /**
     * - table descriptor의 정보를 구성한다.
     */

    /*
     * OWNER_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_OWNER_ID,
                                aDataValueList,
                                & aOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCHEMA_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_SCHEMA_ID,
                                aDataValueList,
                                & aSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * TABLE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_TABLE_ID,
                                aDataValueList,
                                & aTableID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * TABLESPACE_ID
     */

    if ( aTablespaceID == ELL_DICT_TABLESPACE_ID_NA )
    {
        /* Viewed Table 등은 tablespace 가 존재하지 않는다. */
        STL_ASSERT( aTableType != ELL_TABLE_TYPE_BASE_TABLE );
        
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                    ELDT_Tables_ColumnOrder_TABLESPACE_ID,
                                    aDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                    ELDT_Tables_ColumnOrder_TABLESPACE_ID,
                                    aDataValueList,
                                    & aTablespaceID,
                                    aEnv )
                 == STL_SUCCESS );
    }
    
    /*
     * PHYSICAL_ID
     */

    if ( aPhysicalID == ELL_DICT_OBJECT_ID_NA )
    {
        /* Viewed Table 등은 physical object 가 존재하지 않는다. */
        STL_ASSERT( aTableType != ELL_TABLE_TYPE_BASE_TABLE );
        
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                    ELDT_Tables_ColumnOrder_PHYSICAL_ID,
                                    aDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                    ELDT_Tables_ColumnOrder_PHYSICAL_ID,
                                    aDataValueList,
                                    & aPhysicalID,
                                    aEnv )
                 == STL_SUCCESS );
    }
    
    /*
     * TABLE_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_TABLE_NAME,
                                aDataValueList,
                                aTableName,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * TABLE_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_TABLE_TYPE,
                                aDataValueList,
                                (void *) gEllTableTypeString[aTableType],
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * TABLE_TYPE_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_TABLE_TYPE_ID,
                                aDataValueList,
                                & aTableType,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SYSTEM_VERSION_START_COLUMN_NAME
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_SYSTEM_VERSION_START_COLUMN_NAME,
                                aDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SYSTEM_VERSION_END_COLUMN_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_SYSTEM_VERSION_END_COLUMN_NAME,
                                aDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SYSTEM_VERSION_RETENTION_PERIOD
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_SYSTEM_VERSION_RETENTION_PERIOD,
                                aDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SELF_REFERENCING_COLUMN_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_SELF_REFERENCING_COLUMN_NAME,
                                aDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * REFERENCE_GENERATION
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_REFERENCE_GENERATION,
                                aDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * USER_DEFINED_TYPE_CATALOG
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_USER_DEFINED_TYPE_CATALOG,
                                aDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * USER_DEFINED_TYPE_SCHEMA
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_USER_DEFINED_TYPE_SCHEMA,
                                aDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * USER_DEFINED_TYPE_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_USER_DEFINED_TYPE_NAME,
                                aDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_INSERTABLE_INTO
     */

    switch ( aTableType )
    {
        case ELL_TABLE_TYPE_BASE_TABLE:
            if ( aSchemaID == ELDT_SCHEMA_ID_DEFINITION_SCHEMA )
            {
                sBoolValue = STL_FALSE;
            }
            else
            {
                sBoolValue = STL_TRUE;
            }
            break;
        case ELL_TABLE_TYPE_GLOBAL_TEMPORARY:
            sBoolValue = STL_TRUE;
            break;
        case ELL_TABLE_TYPE_LOCAL_TEMPORARY:
            sBoolValue = STL_TRUE;
            break;
        case ELL_TABLE_TYPE_SYSTEM_VERSIONED:
            sBoolValue = STL_TRUE;
            break;
        case ELL_TABLE_TYPE_SEQUENCE:
            sBoolValue = STL_FALSE;
            break;
        case ELL_TABLE_TYPE_VIEW:
            {
                /**
                 * @todo insertable view 가 구현되면 변경되어야 함
                 */
                sBoolValue = STL_FALSE;
            }
            break;
        case ELL_TABLE_TYPE_SYNONYM:
            sBoolValue = STL_FALSE;
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_IS_INSERTABLE_INTO,
                                aDataValueList,
                                & sBoolValue,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_TYPED
     */

    sBoolValue = STL_FALSE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_IS_TYPED,
                                aDataValueList,
                                &sBoolValue,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COMMIT_ACTION
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_COMMIT_ACTION,
                                aDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_SET_SUPPLOG_PK
     */

    sBoolValue = STL_FALSE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_IS_SET_SUPPLOG_PK,
                                aDataValueList,
                                &sBoolValue,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IS_BUILTIN
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_IS_BUILTIN,
                                aDataValueList,
                                & gEllIsMetaBuilding,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * CREATED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_CREATED_TIME,
                                aDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * MODIFIED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_MODIFIED_TIME,
                                aDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COMMENTS
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_COMMENTS,
                                aDataValueList,
                                aTableComment,
                                aEnv )
             == STL_SUCCESS );
        
    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aDataValueList, 0, 1 );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief DROP TABLE 의 Table 의 Prime Element 에 해당하는 Dictionary 정보 삭제
 * @param[in] aTransID            Transaction ID
 * @param[in] aStmt               Statement
 * @param[in] aTableDictHandle    Table Dictionary Handle
 * @param[in] aEnv                Environment
 * @remarks
 * Prime Element : 다른 Table 과의 관계를 갖는 Foreign Key 를 제외한 Element
 * - Table 자체
 *  - Column
 *  - Constraint
 *   - Check Constraint
 *    - Not Null
 *    - Check Clause
 *   - Key Constraint
 *    - Primary Key
 *    - Unique Key
 *    - (X) Foreign Key  <---------------
 *    - (X) Child Foreign Key <--------------
 *  - Index
 *   - Unique Index
 *   - Non-Unique Index
 */
stlStatus eldDeleteDict4DropTablePrimeElement( smlTransId       aTransID,
                                               smlStatement   * aStmt,
                                               ellDictHandle  * aTableDictHandle,
                                               ellEnv         * aEnv )
{
    stlInt32        i = 0;

    ellDictHandle * sIndexHandle = NULL;
    
    stlInt32        sColumnCnt = 0;
    ellDictHandle * sColumnHandle = NULL;

    stlInt32        sCheckNotNullCnt = 0;
    ellDictHandle * sCheckNotNullHandle = NULL;
    
    stlInt32        sCheckClauseCnt = 0;
    ellDictHandle * sCheckClauseHandle = NULL;

    stlInt32        sPrimaryKeyCnt = 0;
    ellDictHandle * sPrimaryKeyHandle = NULL;

    stlInt32        sUniqueKeyCnt = 0;
    ellDictHandle * sUniqueKeyHandle = NULL;

    stlInt32        sUniqueIndexCnt = 0;
    ellDictHandle * sUniqueIndexHandle = NULL;
    
    stlInt32        sNonUniqueIndexCnt = 0;
    ellDictHandle * sNonUniqueIndexHandle = NULL;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /*********************************************************
     * Table 관련 정보 획득
     *********************************************************/

    STL_TRY( ellGetTableCheckNotNullDictHandle( aTransID,
                                                ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                & aEnv->mMemDictOP,
                                                aTableDictHandle,
                                                & sCheckNotNullCnt,
                                                & sCheckNotNullHandle,
                                                aEnv )
             == STL_SUCCESS );

    STL_TRY( ellGetTableCheckClauseDictHandle( aTransID,
                                               ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                               & aEnv->mMemDictOP,
                                               aTableDictHandle,
                                               & sCheckClauseCnt,
                                               & sCheckClauseHandle,
                                               aEnv )
             == STL_SUCCESS );

    STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                              ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                              & aEnv->mMemDictOP,
                                              aTableDictHandle,
                                              & sPrimaryKeyCnt,
                                              & sPrimaryKeyHandle,
                                              aEnv )
             == STL_SUCCESS );
    
    STL_TRY( ellGetTableUniqueKeyDictHandle( aTransID,
                                             ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                             & aEnv->mMemDictOP,
                                             aTableDictHandle,
                                             & sUniqueKeyCnt,
                                             & sUniqueKeyHandle,
                                             aEnv )
             == STL_SUCCESS );

    STL_TRY( ellGetTableUniqueIndexDictHandle( aTransID,
                                               ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                               & aEnv->mMemDictOP,
                                               aTableDictHandle,
                                               & sUniqueIndexCnt,
                                               & sUniqueIndexHandle,
                                               aEnv )
             == STL_SUCCESS );
    
    STL_TRY( ellGetTableNonUniqueIndexDictHandle( aTransID,
                                                  ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                  & aEnv->mMemDictOP,
                                                  aTableDictHandle,
                                                  & sNonUniqueIndexCnt,
                                                  & sNonUniqueIndexHandle,
                                                  aEnv )
             == STL_SUCCESS );

    /*********************************************************
     * Table 의 Check Not Null 정보를 삭제
     *********************************************************/

    for ( i = 0; i < sCheckNotNullCnt; i++ )
    {
        /**
         * Check Not Null Dictionary 정보를 삭제
         */
        
        STL_TRY( eldDeleteDictConst( aTransID,
                                     aStmt,
                                     & sCheckNotNullHandle[i],
                                     aEnv )
                 == STL_SUCCESS );
    }
    
    /*********************************************************
     * Table 의 Check Clause 정보를 삭제
     *********************************************************/
    
    for ( i = 0; i < sCheckClauseCnt; i++ )
    {
        /**
         * Check Clause Dictionary 정보를 삭제
         */
        
        STL_TRY( eldDeleteDictConst( aTransID,
                                     aStmt,
                                     & sCheckClauseHandle[i],
                                     aEnv )
                 == STL_SUCCESS );
    }
    
    /*********************************************************
     * Table 의 Primary Key 정보를 삭제
     *********************************************************/

    for ( i = 0; i < sPrimaryKeyCnt; i++ )
    {
        sIndexHandle = ellGetConstraintIndexDictHandle( & sPrimaryKeyHandle[i] ); 

        /**
         * Primary Key Index Dictionary 정보를 삭제
         */
        
        STL_TRY( eldDeleteDictIndex( aTransID,
                                     aStmt,
                                     sIndexHandle,
                                     aEnv )
                 == STL_SUCCESS );

        /**
         * Primary Key Dictionary 정보를 삭제
         */
        
        STL_TRY( eldDeleteDictConst( aTransID,
                                     aStmt,
                                     & sPrimaryKeyHandle[i],
                                     aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table 의 Unique Key 정보를 삭제
     *********************************************************/
    
    for ( i = 0; i < sUniqueKeyCnt; i++ )
    {
        sIndexHandle = ellGetConstraintIndexDictHandle( & sUniqueKeyHandle[i] ); 

        /**
         * Unique Key Index Dictionary 정보를 삭제
         */
        
        STL_TRY( eldDeleteDictIndex( aTransID,
                                     aStmt,
                                     sIndexHandle,
                                     aEnv )
                 == STL_SUCCESS );

        /**
         * Unique Key Dictionary 정보를 삭제
         */
        
        STL_TRY( eldDeleteDictConst( aTransID,
                                     aStmt,
                                     & sUniqueKeyHandle[i],
                                     aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table 의 Unique Index 정보를 삭제
     *********************************************************/

    for ( i = 0; i < sUniqueIndexCnt; i++ )
    {
        sIndexHandle = & sUniqueIndexHandle[i];

        /**
         * Unique Index Dictionary 정보를 삭제
         */
        
        STL_TRY( eldDeleteDictIndex( aTransID,
                                     aStmt,
                                     sIndexHandle,
                                     aEnv )
                 == STL_SUCCESS );
    }
    
    /*********************************************************
     * Table 의 Non-Unique Index 정보를 삭제
     *********************************************************/

    for ( i = 0; i < sNonUniqueIndexCnt; i++ )
    {
        sIndexHandle = & sNonUniqueIndexHandle[i];

        /**
         * Non-Unique Index Dictionary 정보를 삭제
         */
        
        STL_TRY( eldDeleteDictIndex( aTransID,
                                     aStmt,
                                     sIndexHandle,
                                     aEnv )
                 == STL_SUCCESS );
    }
    

    /*********************************************************
     * Table 의 Column Dictionary 정보를 삭제
     *********************************************************/
    
    sColumnCnt = ellGetTableColumnCnt( aTableDictHandle );
    sColumnHandle = ellGetTableColumnDictHandle( aTableDictHandle );

    for ( i = 0; i < sColumnCnt; i++ )
    {
        STL_TRY( eldDeleteDictColumn( aTransID,
                                      aStmt,
                                      & sColumnHandle[i],
                                      aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table Privilege 정보를 삭제
     *********************************************************/
    
    /**
     * Table Privilege Record 를 삭제
     */
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.TABLE_PRIVILEGES
     *  WHERE TABLE_ID = sTableID;
     */
    
    STL_TRY( eldDeletePrivRowsAndCache( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_TABLE_PRIVILEGES,
                                        ELDT_TablePriv_ColumnOrder_TABLE_ID,
                                        ellGetTableID(aTableDictHandle),
                                        aEnv )
             == STL_SUCCESS );
    
    /*********************************************************
     * Table 정보를 삭제
     *********************************************************/
    
    /**
     * Table 의 관련 Dictionary Record 정보를 삭제
     */

    /*
     * DELETE FROM DEFINITION_SCHEMA.TABLES
     *  WHERE TABLE_ID = sTableID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_TABLES,
                                  ELDT_Tables_ColumnOrder_TABLE_ID,
                                  ellGetTableID(aTableDictHandle),
                                  aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
                                               
    return STL_FAILURE;
}





                              
/***********************************************************************
 * Dictionary Cache 변경 함수 
 ***********************************************************************/



/**
 * @brief Table Cache 의 Prime Element 를 제거한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aTableDictHandle  Table Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldRemoveTablePrimeElementCache( smlTransId       aTransID,
                                           ellDictHandle  * aTableDictHandle,
                                           ellEnv         * aEnv )
{
    stlInt32        i = 0;
    ellDictHandle * sIndexHandle = NULL;

    stlInt32        sColumnCnt = 0;
    ellDictHandle * sColumnHandle = NULL;

    /*
     * Related Object Handle
     */
    
    stlInt32        sCheckNotNullCnt = 0;
    ellDictHandle * sCheckNotNullHandle = NULL;
    
    stlInt32        sCheckClauseCnt = 0;
    ellDictHandle * sCheckClauseHandle = NULL;

    stlInt32        sPrimaryKeyCnt = 0;
    ellDictHandle * sPrimaryKeyHandle = NULL;

    stlInt32        sUniqueKeyCnt = 0;
    ellDictHandle * sUniqueKeyHandle = NULL;

    stlInt32        sUniqueIndexCnt = 0;
    ellDictHandle * sUniqueIndexHandle = NULL;
    
    stlInt32        sNonUniqueIndexCnt = 0;
    ellDictHandle * sNonUniqueIndexHandle = NULL;

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table 관련 정보 획득
     * - Dictionary Object Handle 이 dropping 되면,
     * - 해당 Handle 정보를 다시 읽을 때 dropping flag 으로 인해 접근할 수 없으므로,
     * - 미리 Handle 을 얻어 두어야 한다.
     */

    STL_TRY( ellGetTableCheckNotNullDictHandle( aTransID,
                                                SML_MAXIMUM_STABLE_SCN,
                                                & aEnv->mMemDictOP,
                                                aTableDictHandle,
                                                & sCheckNotNullCnt,
                                                & sCheckNotNullHandle,
                                                aEnv )
             == STL_SUCCESS );

    STL_TRY( ellGetTableCheckClauseDictHandle( aTransID,
                                               SML_MAXIMUM_STABLE_SCN,
                                               & aEnv->mMemDictOP,
                                               aTableDictHandle,
                                               & sCheckClauseCnt,
                                               & sCheckClauseHandle,
                                               aEnv )
             == STL_SUCCESS );

    STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                              SML_MAXIMUM_STABLE_SCN,
                                              & aEnv->mMemDictOP,
                                              aTableDictHandle,
                                              & sPrimaryKeyCnt,
                                              & sPrimaryKeyHandle,
                                              aEnv )
             == STL_SUCCESS );
    
    STL_TRY( ellGetTableUniqueKeyDictHandle( aTransID,
                                             SML_MAXIMUM_STABLE_SCN,
                                             & aEnv->mMemDictOP,
                                             aTableDictHandle,
                                             & sUniqueKeyCnt,
                                             & sUniqueKeyHandle,
                                             aEnv )
             == STL_SUCCESS );

    STL_TRY( ellGetTableUniqueIndexDictHandle( aTransID,
                                               SML_MAXIMUM_STABLE_SCN,
                                               & aEnv->mMemDictOP,
                                               aTableDictHandle,
                                               & sUniqueIndexCnt,
                                               & sUniqueIndexHandle,
                                               aEnv )
             == STL_SUCCESS );
    
    STL_TRY( ellGetTableNonUniqueIndexDictHandle( aTransID,
                                                  SML_MAXIMUM_STABLE_SCN,
                                                  & aEnv->mMemDictOP,
                                                  aTableDictHandle,
                                                  & sNonUniqueIndexCnt,
                                                  & sNonUniqueIndexHandle,
                                                  aEnv )
             == STL_SUCCESS );

    /*********************************************************
     * Table 의 Check Not Null 관련 Cache 를 삭제
     *********************************************************/

    for ( i = 0; i < sCheckNotNullCnt; i++ )
    {
        /**
         * SQL-Constraint Cache 를 삭제
         */
        
        STL_TRY( eldcDeleteCacheConst( aTransID,
                                       & sCheckNotNullHandle[i],
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Table 에서 Check Not Null 연결 정보를 삭제
         */

        STL_TRY( eldcDelConstraintFromTable( aTransID,
                                             aTableDictHandle,
                                             & sCheckNotNullHandle[i],
                                             aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table 의 Check Clause 관련 Cache 를 삭제
     *********************************************************/

    for ( i = 0; i < sCheckClauseCnt; i++ )
    {
        /**
         * SQL-Constraint Cache 를 삭제
         */
        
        STL_TRY( eldcDeleteCacheConst( aTransID,
                                       & sCheckClauseHandle[i],
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Table 에서 Check Clause 연결 정보를 삭제
         */

        STL_TRY( eldcDelConstraintFromTable( aTransID,
                                             aTableDictHandle,
                                             & sCheckClauseHandle[i],
                                             aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table 의 Primary Key 관련 Cache 를 삭제
     *********************************************************/

    for ( i = 0; i < sPrimaryKeyCnt; i++ )
    {
        sIndexHandle = ellGetConstraintIndexDictHandle( & sPrimaryKeyHandle[i] ); 

        /**
         * Primary Key 의 SQL-Index Cache 를 삭제
         */
        
        STL_TRY( eldcDeleteCacheIndex( aTransID,
                                       sIndexHandle,
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Primary Key 의 SQL-Constraint Cache 를 삭제
         */

        STL_TRY( eldcDeleteCacheConst( aTransID,
                                       & sPrimaryKeyHandle[i],
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Table 에서 Primary Key Cache 연결 정보를 삭제
         */

        STL_TRY( eldcDelConstraintFromTable( aTransID,
                                             aTableDictHandle,
                                             & sPrimaryKeyHandle[i],
                                             aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table 의 Unique Key 관련 Cache 를 삭제
     *********************************************************/
    
    for ( i = 0; i < sUniqueKeyCnt; i++ )
    {
        sIndexHandle = ellGetConstraintIndexDictHandle( & sUniqueKeyHandle[i] ); 

        /**
         * Unique Key 의 SQL-Index Cache 를 삭제
         */
        
        STL_TRY( eldcDeleteCacheIndex( aTransID,
                                       sIndexHandle,
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Unique Key 의 SQL-Constraint Cache 를 삭제
         */

        STL_TRY( eldcDeleteCacheConst( aTransID,
                                       & sUniqueKeyHandle[i],
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Table 에서 Unique Key Cache 연결 정보를 삭제
         */

        STL_TRY( eldcDelConstraintFromTable( aTransID,
                                             aTableDictHandle,
                                             & sUniqueKeyHandle[i],
                                             aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table 의 Unique Index 관련 Cache 를 삭제
     *********************************************************/

    for ( i = 0; i < sUniqueIndexCnt; i++ )
    {
        sIndexHandle = & sUniqueIndexHandle[i];

        /**
         * Unique Index 의 SQL-Index Cache 를 삭제
         */
        
        STL_TRY( eldcDeleteCacheIndex( aTransID,
                                       sIndexHandle,
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Table 에서 Unique Index Cache 연결 정보를 삭제
         */

        STL_TRY( eldcDelUniqueIndexFromTable( aTransID,
                                              aTableDictHandle,
                                              sIndexHandle,
                                              aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table 의 Non-Unique Index 관련 Cache 를 삭제
     *********************************************************/

    for ( i = 0; i < sNonUniqueIndexCnt; i++ )
    {
        sIndexHandle = & sNonUniqueIndexHandle[i];

        /**
         * Non-Unique Index 의 SQL-Index Cache 를 삭제
         */
        
        STL_TRY( eldcDeleteCacheIndex( aTransID,
                                       sIndexHandle,
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Table 에서 Non-Unique Index Cache 연결 정보를 삭제
         */

        STL_TRY( eldcDelNonUniqueIndexFromTable( aTransID,
                                                 aTableDictHandle,
                                                 sIndexHandle,
                                                 aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table 의 SQL-Column Cache 정보를 삭제
     *********************************************************/
    
    sColumnCnt = ellGetTableColumnCnt( aTableDictHandle );
    sColumnHandle = ellGetTableColumnDictHandle( aTableDictHandle );

    for ( i = 0; i < sColumnCnt; i++ )
    {
        /**
         * SQL-Column Cache 를 삭제 
         */
        
        STL_TRY( eldcDeleteCacheColumn( aTransID,
                                        & sColumnHandle[i],
                                        aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * SQL-Table Cache 를 삭제 
     *********************************************************/
    
    STL_TRY( eldcDeleteCacheTable( aTransID,
                                   aTableDictHandle,
                                   aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                                           

/**
 * @brief Table Cache 를 모두 제거한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aTableDictHandle  Table Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldRemoveTableAllCache( smlTransId       aTransID,
                                  smlStatement   * aStmt,
                                  ellDictHandle  * aTableDictHandle,
                                  ellEnv         * aEnv )
{
    stlInt32        i = 0;
    ellDictHandle * sIndexHandle = NULL;

    stlInt32        sColumnCnt = 0;
    ellDictHandle * sColumnHandle = NULL;

    /*
     * Related Object Handle
     */
    
    stlInt32        sCheckNotNullCnt = 0;
    ellDictHandle * sCheckNotNullHandle = NULL;
    
    stlInt32        sCheckClauseCnt = 0;
    ellDictHandle * sCheckClauseHandle = NULL;

    stlInt32        sPrimaryKeyCnt = 0;
    ellDictHandle * sPrimaryKeyHandle = NULL;

    stlInt32        sUniqueKeyCnt = 0;
    ellDictHandle * sUniqueKeyHandle = NULL;

    stlInt32        sForeignKeyCnt = 0;
    ellDictHandle * sForeignKeyHandle = NULL;

    stlInt32        sChildForeignKeyCnt = 0;
    ellDictHandle * sChildForeignKeyHandle = NULL;
    
    stlInt32        sUniqueIndexCnt = 0;
    ellDictHandle * sUniqueIndexHandle = NULL;
    
    stlInt32        sNonUniqueIndexCnt = 0;
    ellDictHandle * sNonUniqueIndexHandle = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table 관련 정보 획득
     * - Dictionary Object Handle 이 dropping 되면,
     * - 해당 Handle 정보를 다시 읽을 때 dropping flag 으로 인해 접근할 수 없으므로,
     * - 미리 Handle 을 얻어 두어야 한다.
     */

    STL_TRY( ellGetTableCheckNotNullDictHandle( aTransID,
                                                ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                & aEnv->mMemDictOP,
                                                aTableDictHandle,
                                                & sCheckNotNullCnt,
                                                & sCheckNotNullHandle,
                                                aEnv )
             == STL_SUCCESS );

    STL_TRY( ellGetTableCheckClauseDictHandle( aTransID,
                                               ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                               & aEnv->mMemDictOP,
                                               aTableDictHandle,
                                               & sCheckClauseCnt,
                                               & sCheckClauseHandle,
                                               aEnv )
             == STL_SUCCESS );

    STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                              ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                              & aEnv->mMemDictOP,
                                              aTableDictHandle,
                                              & sPrimaryKeyCnt,
                                              & sPrimaryKeyHandle,
                                              aEnv )
             == STL_SUCCESS );
    
    STL_TRY( ellGetTableUniqueKeyDictHandle( aTransID,
                                             ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                             & aEnv->mMemDictOP,
                                             aTableDictHandle,
                                             & sUniqueKeyCnt,
                                             & sUniqueKeyHandle,
                                             aEnv )
             == STL_SUCCESS );
    
    STL_TRY( ellGetTableForeignKeyDictHandle( aTransID,
                                              ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                              & aEnv->mMemDictOP,
                                              aTableDictHandle,
                                              & sForeignKeyCnt,
                                              & sForeignKeyHandle,
                                              aEnv )
             == STL_SUCCESS );

    STL_TRY( ellGetTableChildForeignKeyDictHandle( aTransID,
                                                   ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                   & aEnv->mMemDictOP,
                                                   aTableDictHandle,
                                                   & sChildForeignKeyCnt,
                                                   & sChildForeignKeyHandle,
                                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( ellGetTableUniqueIndexDictHandle( aTransID,
                                               ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                               & aEnv->mMemDictOP,
                                               aTableDictHandle,
                                               & sUniqueIndexCnt,
                                               & sUniqueIndexHandle,
                                               aEnv )
             == STL_SUCCESS );
    
    STL_TRY( ellGetTableNonUniqueIndexDictHandle( aTransID,
                                                  ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                  & aEnv->mMemDictOP,
                                                  aTableDictHandle,
                                                  & sNonUniqueIndexCnt,
                                                  & sNonUniqueIndexHandle,
                                                  aEnv )
             == STL_SUCCESS );

    /*********************************************************
     * Table 의 Check Not Null 관련 Cache 를 삭제
     *********************************************************/

    for ( i = 0; i < sCheckNotNullCnt; i++ )
    {
        /**
         * SQL-Constraint Cache 를 삭제
         */
        
        STL_TRY( eldcDeleteCacheConst( aTransID,
                                       & sCheckNotNullHandle[i],
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Table 에서 Check Not Null 연결 정보를 삭제
         */

        STL_TRY( eldcDelConstraintFromTable( aTransID,
                                             aTableDictHandle,
                                             & sCheckNotNullHandle[i],
                                             aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table 의 Check Clause 관련 Cache 를 삭제
     *********************************************************/

    for ( i = 0; i < sCheckClauseCnt; i++ )
    {
        /**
         * SQL-Constraint Cache 를 삭제
         */
        
        STL_TRY( eldcDeleteCacheConst( aTransID,
                                       & sCheckClauseHandle[i],
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Table 에서 Check Clause 연결 정보를 삭제
         */

        STL_TRY( eldcDelConstraintFromTable( aTransID,
                                             aTableDictHandle,
                                             & sCheckClauseHandle[i],
                                             aEnv )
                 == STL_SUCCESS );
        
    }

    /*********************************************************
     * Table 의 Primary Key 관련 Cache 를 삭제
     *********************************************************/

    for ( i = 0; i < sPrimaryKeyCnt; i++ )
    {
        sIndexHandle = ellGetConstraintIndexDictHandle( & sPrimaryKeyHandle[i] ); 

        /**
         * Primary Key 의 SQL-Index Cache 를 삭제
         */
        
        STL_TRY( eldcDeleteCacheIndex( aTransID,
                                       sIndexHandle,
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Primary Key 의 SQL-Constraint Cache 를 삭제
         */

        STL_TRY( eldcDeleteCacheConst( aTransID,
                                       & sPrimaryKeyHandle[i],
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Table 에서 Primary Key Cache 연결 정보를 삭제
         */

        STL_TRY( eldcDelConstraintFromTable( aTransID,
                                             aTableDictHandle,
                                             & sPrimaryKeyHandle[i],
                                             aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table 의 Unique Key 관련 Cache 를 삭제
     *********************************************************/
    
    for ( i = 0; i < sUniqueKeyCnt; i++ )
    {
        sIndexHandle = ellGetConstraintIndexDictHandle( & sUniqueKeyHandle[i] ); 

        /**
         * Unique Key 의 SQL-Index Cache 를 삭제
         */
        
        STL_TRY( eldcDeleteCacheIndex( aTransID,
                                       sIndexHandle,
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Unique Key 의 SQL-Constraint Cache 를 삭제
         */

        STL_TRY( eldcDeleteCacheConst( aTransID,
                                       & sUniqueKeyHandle[i],
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Table 에서 Unique Key Cache 연결 정보를 삭제
         */

        STL_TRY( eldcDelConstraintFromTable( aTransID,
                                             aTableDictHandle,
                                             & sUniqueKeyHandle[i],
                                             aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table 의 Foreign Key 관련 Cache 를 삭제
     *********************************************************/
    
    for ( i = 0; i < sForeignKeyCnt; i++ )
    {
        sIndexHandle = ellGetConstraintIndexDictHandle( & sForeignKeyHandle[i] ); 

        /**
         * Foreign Key 의 SQL-Index Cache 를 삭제
         */
        
        STL_TRY( eldcDeleteCacheIndex( aTransID,
                                       sIndexHandle,
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Foreign Key 의 SQL-Constraint Cache 를 삭제
         */

        STL_TRY( eldcDeleteCacheConst( aTransID,
                                       & sForeignKeyHandle[i],
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Table 에서 Foreign Key Cache 연결 정보를 삭제
         */

        STL_TRY( eldcDelConstraintFromTable( aTransID,
                                             aTableDictHandle,
                                             & sForeignKeyHandle[i],
                                             aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table 의 Child Foreign Key 연결 정보를 삭제
     *********************************************************/
    
    for ( i = 0; i < sChildForeignKeyCnt; i++ )
    {
        /**
         * Table 에서 Child Foreign Key 연결 정보를 삭제
         */

        STL_TRY( eldcDelChildForeignKeyFromTable( aTransID,
                                                  aStmt,
                                                  aTableDictHandle,
                                                  & sChildForeignKeyHandle[i],
                                                  aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table 의 Unique Index 관련 Cache 를 삭제
     *********************************************************/

    for ( i = 0; i < sUniqueIndexCnt; i++ )
    {
        sIndexHandle = & sUniqueIndexHandle[i];

        /**
         * Unique Index 의 SQL-Index Cache 를 삭제
         */
        
        STL_TRY( eldcDeleteCacheIndex( aTransID,
                                       sIndexHandle,
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Table 에서 Unique Index Cache 연결 정보를 삭제
         */

        STL_TRY( eldcDelUniqueIndexFromTable( aTransID,
                                              aTableDictHandle,
                                              sIndexHandle,
                                              aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table 의 Non-Unique Index 관련 Cache 를 삭제
     *********************************************************/

    for ( i = 0; i < sNonUniqueIndexCnt; i++ )
    {
        sIndexHandle = & sNonUniqueIndexHandle[i];

        /**
         * Non-Unique Index 의 SQL-Index Cache 를 삭제
         */
        
        STL_TRY( eldcDeleteCacheIndex( aTransID,
                                       sIndexHandle,
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Table 에서 Non-Unique Index Cache 연결 정보를 삭제
         */

        STL_TRY( eldcDelNonUniqueIndexFromTable( aTransID,
                                                 aTableDictHandle,
                                                 sIndexHandle,
                                                 aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * Table 의 SQL-Column Cache 정보를 삭제
     *********************************************************/
    
    sColumnCnt = ellGetTableColumnCnt( aTableDictHandle );
    sColumnHandle = ellGetTableColumnDictHandle( aTableDictHandle );

    for ( i = 0; i < sColumnCnt; i++ )
    {
        /**
         * SQL-Column Cache 를 삭제 
         */
        
        STL_TRY( eldcDeleteCacheColumn( aTransID,
                                        & sColumnHandle[i],
                                        aEnv )
                 == STL_SUCCESS );
    }

    /*********************************************************
     * SQL-Table Cache 를 삭제 
     *********************************************************/
    
    STL_TRY( eldcDeleteCacheTable( aTransID,
                                   aTableDictHandle,
                                   aEnv )
             == STL_SUCCESS );
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                                           
                                           

/**
 * @brief Table Cache 를 구축한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aTableID          Table ID
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldBuildTableCache( smlTransId       aTransID,
                              smlStatement   * aStmt,
                              stlInt64         aTableID,
                              ellEnv         * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID != ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );

    /**
     * SQL-Column Cache 를 추가 
     */

    STL_TRY( eldcInsertCacheColumnByTableID( aTransID,
                                             aStmt,
                                             aTableID,
                                             aEnv )
             == STL_SUCCESS );

    /**
     * SQL-Table Cache 를 추가 
     */

    STL_TRY( eldcInsertCacheTableByTableID( aTransID,
                                            aStmt,
                                            aTableID,
                                            aEnv )
             == STL_SUCCESS );
    
    /**
     * SQL-Index Cache 를 추가
     */

    STL_TRY( eldcInsertCacheIndexByTableID( aTransID,
                                            aStmt,
                                            aTableID,
                                            aEnv )
             == STL_SUCCESS );

    /**
     * SQL-Constraint Cache 를 추가
     */

    STL_TRY( eldcInsertCacheConstByTableID( aTransID,
                                            aStmt,
                                            aTableID,
                                            aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/***********************************************************************
 * Handle 획득 함수 
 ***********************************************************************/



/**
 * @brief Table ID 를 이용해 Table Dictionary Handle 을 얻는다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aViewSCN          View SCN
 * @param[in]  aTableID          Table ID
 * @param[out] aTableDictHandle  Table Dictionary Handle
 * @param[out] aExist            존재 여부
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus eldGetTableHandleByID( smlTransId           aTransID,
                                 smlScn               aViewSCN,
                                 stlInt64             aTableID,
                                 ellDictHandle      * aTableDictHandle,
                                 stlBool            * aExist,
                                 ellEnv             * aEnv )
{
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableID != ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Normal Table 에 대한 Table Handle 검색
     */
    
    STL_TRY( eldcGetTableHandleByID( aTransID,
                                     aViewSCN,
                                     aTableID,
                                     aTableDictHandle,
                                     aExist,
                                     aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Schema ID 와 Table Name 을 이용해 Table Dictionary Handle 을 얻는다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aViewSCN          View SCN
 * @param[in]  aSchemaID         Schema ID
 * @param[in]  aTableName        Table Name
 * @param[out] aTableDictHandle  Table Dictionary Handle
 * @param[out] aExist            존재 여부
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus eldGetTableHandleByName( smlTransId           aTransID,
                                   smlScn               aViewSCN,
                                   stlInt64             aSchemaID,
                                   stlChar            * aTableName,
                                   ellDictHandle      * aTableDictHandle,
                                   stlBool            * aExist,
                                   ellEnv             * aEnv )
{
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table Handle 검색
     */
    
    STL_TRY( eldcGetTableHandleByName( aTransID,
                                       aViewSCN,
                                       aSchemaID,
                                       aTableName,
                                       aTableDictHandle,
                                       aExist,
                                       aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Table 관련 Object 의 Dictionary Handle Array 를 얻는다.
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aViewSCN               View SCN
 * @param[in]  aRegionMem             Object Handle Array 를 위한 Region Memory
 * @param[in]  aTableDictHandle       Table Dictionary Handle
 * @param[in]  aRelatedType           Related Object 의 유형
 * @param[out] aRelatedCnt            Related Object 의 갯수
 * @param[out] aRelatedObjectHandle   Related Object 들의 Handle Array
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus eldGetTableRelatedDictHandle( smlTransId               aTransID,
                                        smlScn                   aViewSCN,
                                        knlRegionMem           * aRegionMem,
                                        ellDictHandle          * aTableDictHandle,
                                        ellTableRelatedObject    aRelatedType,
                                        stlInt32               * aRelatedCnt,
                                        ellDictHandle         ** aRelatedObjectHandle,
                                        ellEnv                 * aEnv )
{
    stlInt32        sObjectCnt = 0;
    ellDictHandle * sObjectHandleArray = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRelatedCnt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRelatedObjectHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Related Object Handle 획득
     */

    STL_TRY( eldcGetTableRelatedObjectHandle( aTransID,
                                              aViewSCN,
                                              aTableDictHandle,
                                              aRelatedType,
                                              & sObjectCnt,
                                              & sObjectHandleArray,
                                              aRegionMem,
                                              aEnv )
             == STL_SUCCESS );
    
    /**
     * Output 설정 
     */
    
    *aRelatedCnt = sObjectCnt;
    *aRelatedObjectHandle = sObjectHandleArray;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}





/**
 * @brief DDL 을 위해 Table 유형에 따른 lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aLockMode         Lock Mode on Table
 * @param[in]  aTableDictHandle  Table Dictionary Handle
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus eldLockTable4DDL( smlTransId       aTransID,
                            smlStatement   * aStmt,
                            stlInt32         aLockMode,
                            ellDictHandle  * aTableDictHandle,
                            stlBool        * aLockSuccess,
                            ellEnv         * aEnv )
{
    stlInt32   sRowLockMode;
    stlBool    sTableExist = STL_TRUE;
    stlBool    sLocked = STL_TRUE;

    /**
     * Paramter Validtion
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aLockMode == SML_LOCK_S) || (aLockMode == SML_LOCK_X) ||
                        (aLockMode == SML_LOCK_IS) || (aLockMode == SML_LOCK_IX),
                        ELL_ERROR_STACK(aEnv) );

    /**
     * Table 객체 유형에 따른 Lock 획득
     */

    sLocked = STL_TRUE;
    switch ( ellGetTableType( aTableDictHandle ) )
    {
        case ELL_TABLE_TYPE_BASE_TABLE:
            {
                /**
                 * Base Table 인 경우
                 * - Table Segment 에 lock 을 잡는다.
                 */
                
                STL_TRY( smlLockTable( aTransID,
                                       aLockMode,
                                       ellGetTableHandle( aTableDictHandle ),
                                       SML_LOCK_TIMEOUT_PROPERTY,
                                       &sTableExist,
                                       SML_ENV(aEnv) )
                         == STL_SUCCESS );

                /**
                 * Dictionary Cache에서 테이블이 유효하다고 판단했더라도 삭제된 테이블일수 있다.
                 * - 테이블 삭제후 SM commit까지 수행한 시점에서는 객체는 삭제되어있고 Dictionary Cache는
                 *   유효하다고 판단할수 있다.
                 * 반드시 삭제되었는지 확인해야 한다.
                 */

                if ( sTableExist != STL_TRUE )
                {
                    sLocked = STL_FALSE;
                    STL_THROW( RAMP_FINISH );
                }
                
                KNL_BREAKPOINT( KNL_BREAKPOINT_ELDLOCKANDVALIDATETABLE4DDL_AFTER_LOCK,
                                KNL_ENV( aEnv ) );

                break;
            }
        case ELL_TABLE_TYPE_VIEW:
            {
                /**
                 * Viewed Table 인 경우
                 * - Table Segment 가 존재하지 않으므로 Dictionary Row 에 Lock 을 획득한다.
                 */

                if ( (aLockMode == SML_LOCK_S) || (aLockMode == SML_LOCK_IS) )
                {
                    sRowLockMode = SML_LOCK_S;
                }
                else
                {
                    sRowLockMode = SML_LOCK_X;
                }
                
                STL_TRY( eldLockRowByObjectHandle( aTransID,
                                                   aStmt,
                                                   ELDT_TABLE_ID_TABLES,
                                                   ELDT_Tables_ColumnOrder_TABLE_ID,
                                                   aTableDictHandle,
                                                   sRowLockMode,
                                                   & sLocked,
                                                   aEnv )
                         == STL_SUCCESS );

                break;
            }
        case ELL_TABLE_TYPE_GLOBAL_TEMPORARY:
        case ELL_TABLE_TYPE_LOCAL_TEMPORARY:
        case ELL_TABLE_TYPE_SYSTEM_VERSIONED:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }

        case ELL_TABLE_TYPE_SEQUENCE:
        case ELL_TABLE_TYPE_FIXED_TABLE:
        case ELL_TABLE_TYPE_DUMP_TABLE:
        case ELL_TABLE_TYPE_SYNONYM:
        default: 
            {
                /**
                 * SQL layer 에서 error handling 해야 함.
                 */
                
                STL_ASSERT(0);
                break;
            }
    }

    /**
     * Output 설정
     */

    STL_RAMP( RAMP_FINISH );
    
    *aLockSuccess = sLocked;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      ELL_ERROR_STACK(aEnv),
                      "eldLockTable4DDL()" );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}




/**
 * @brief Table DDL 에 대한 Supplemental Logging 을 기록한다.
 * @param[in]  aTransID    Transaction ID
 * @param[in]  aTableID    Table ID
 * @param[in]  aStmtType   Table DDL Supplemental Stmt Type
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus eldWriteSuppLogTableDDL( smlTransId           aTransID,
                                   stlInt64             aTableID,
                                   ellSuppLogTableDDL   aStmtType,
                                   ellEnv             * aEnv )
{
    ellDictHandle sTableHandle;
    stlBool       sObjectExist = STL_FALSE;
    
    /**
     * DDL Supplemental Logging
     */

    STL_TRY( ellGetTableDictHandleByID( aTransID,
                                        SML_MAXIMUM_STABLE_SCN,
                                        aTableID,
                                        & sTableHandle,
                                        & sObjectExist,
                                        aEnv )
             == STL_SUCCESS );
    STL_ASSERT( sObjectExist == STL_TRUE );

    /**
     * Supplemental Logging 대상인지 확인
     */
    
    if ( ellGetTableNeedSupplePK( & sTableHandle, aEnv ) == STL_TRUE )
    {
        /**
         * Supplemental Logging 기록
         */
        
        STL_TRY( smlWriteSupplementalDdlLog( aTransID,
                                             NULL,
                                             0,
                                             aStmtType,
                                             ellGetTablePhysicalID( & sTableHandle ),
                                             SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * nothing to do
         */
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Truncate Table 에 대한 Supplemental Logging 을 기록한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aTableID        Table ID
 * @param[in]  aNewPhysicalID  Table 의 New Physical ID
 * @param[in]  aStmtType       Table DDL Supplemental Stmt Type
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus eldWriteSuppLogTable4Truncate( smlTransId           aTransID,
                                         stlInt64             aTableID,
                                         stlInt64             aNewPhysicalID,
                                         ellSuppLogTableDDL   aStmtType,
                                         ellEnv             * aEnv )
{
    ellDictHandle sTableHandle;
    stlBool       sObjectExist = STL_FALSE;
    
    /**
     * DDL Supplemental Logging
     */

    STL_TRY( ellGetTableDictHandleByID( aTransID,
                                        SML_MAXIMUM_STABLE_SCN,
                                        aTableID,
                                        & sTableHandle,
                                        & sObjectExist,
                                        aEnv )
             == STL_SUCCESS );
    STL_ASSERT( sObjectExist == STL_TRUE );

    /**
     * Supplemental Logging 대상인지 확인
     */
    
    if ( ellGetTableNeedSupplePK( & sTableHandle, aEnv ) == STL_TRUE )
    {
        /**
         * Supplemental Logging 기록
         */
        
        STL_TRY( smlWriteSupplementalDdlLog( aTransID,
                                             &aNewPhysicalID,
                                             STL_SIZEOF( stlInt64 ),
                                             aStmtType,
                                             ellGetTablePhysicalID( & sTableHandle ),
                                             SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * nothing to do
         */
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} eldTable */
