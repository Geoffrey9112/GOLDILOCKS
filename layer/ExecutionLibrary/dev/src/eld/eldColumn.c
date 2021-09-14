/*******************************************************************************
 * eldColumn.c
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
 * @file eldColumn.c
 * @brief Column Dictionary 관리 루틴
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldColumn.h>
#include <eldDictionary.h>

/**
 * @addtogroup eldColumn
 * @{
 */


/***********************************************************************
 * Dictionary Row 변경 함수 
 ***********************************************************************/

/**
 * @brief Column Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertColumnDesc() 주석 참조
*/

stlStatus eldInsertColumnDesc( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               stlInt64             aOwnerID,
                               stlInt64             aSchemaID,
                               stlInt64             aTableID,
                               stlInt64           * aColumnID,
                               stlChar            * aColumnName,
                               stlInt32             aPhysicalOrdinalPosition,
                               stlInt32             aLogicalOrdinalPosition,
                               stlChar            * aColumnDefault,
                               stlBool              aIsNullable,
                               stlBool              aIsIdentity,
                               ellIdentityGenOption aIdentityGenOption,
                               stlInt64             aIdentityStart,
                               stlInt64             aIdentityIncrement,
                               stlInt64             aIdentityMaximum,
                               stlInt64             aIdentityMinimum,
                               stlBool              aIdentityCycle,
                               stlInt64             aIdentityTablespaceID,
                               stlInt64             aIdentityPhysicalID,
                               stlInt64             aIdentityCacheSize,
                               stlBool              aIsUpdatable,
                               stlChar            * aColumnComment,
                               ellEnv             * aEnv )
{
    stlBool     sBoolValue = STL_FALSE;
    
    stlInt64                   sColumnID = 0;
    stlInt32                   sOrdinalPosition = 0;

    ellIdentityGenOption       sIdentityOption = 0;
    
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
    STL_PARAM_VALIDATE( aTableID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnID != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPhysicalOrdinalPosition >= 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLogicalOrdinalPosition >= 0, ELL_ERROR_STACK(aEnv) );

    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_COLUMNS,
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
     * - column descriptor의 정보를 구성한다.
     */

    /*
     * OWNER_ID
     */
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_OWNER_ID,
                                sDataValueList,
                                & aOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCHEMA_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_SCHEMA_ID,
                                sDataValueList,
                                & aSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * TABLE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_TABLE_ID,
                                sDataValueList,
                                & aTableID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COLUMN_ID
     */

    if ( gEllIsMetaBuilding == STL_TRUE )
    {
        /* Database Dictionary 를 초기화하는 단계 */
        gEldtIdentityValue[ELDT_TABLE_ID_COLUMNS] += 1;
        sColumnID = gEldtIdentityValue[ELDT_TABLE_ID_COLUMNS];
    }
    else
    {
        /* Database가 이미 구축되어 있는 단계 */
        
        /**
         * Identity Column으로부터 Column ID를 획득
         */
        STL_TRY( smlGetNextSequenceVal( gEldIdentityColumnHandle[ELDT_TABLE_ID_COLUMNS],
                                        & sColumnID,
                                        SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        
    }

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_COLUMN_ID,
                                sDataValueList,
                                & sColumnID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * COLUMN_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_COLUMN_NAME,
                                sDataValueList,
                                aColumnName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * PHYSICAL_ORDINAL_POSITION
     */

    sOrdinalPosition = ELD_ORDINAL_POSITION_TO_DATABASE(aPhysicalOrdinalPosition);
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_PHYSICAL_ORDINAL_POSITION,
                                sDataValueList,
                                & sOrdinalPosition,
                                aEnv )
             == STL_SUCCESS );

    /*
     * LOGICAL_ORDINAL_POSITION
     */

    sOrdinalPosition = ELD_ORDINAL_POSITION_TO_DATABASE(aLogicalOrdinalPosition);
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_LOGICAL_ORDINAL_POSITION,
                                sDataValueList,
                                & sOrdinalPosition,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * DTD_IDENIFIER
     * - COLUMN_ID 와 동일한 값
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_DTD_IDENTIFIER,
                                sDataValueList,
                                & sColumnID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DOMAIN_CATALOG
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_DOMAIN_CATALOG,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DOMAIN_SCHEMA
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_DOMAIN_SCHEMA,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DOMAIN_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_DOMAIN_NAME,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COLUMN_DEFAULT
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_COLUMN_DEFAULT,
                                sDataValueList,
                                aColumnDefault,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_NULLABLE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_NULLABLE,
                                sDataValueList,
                                & aIsNullable,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_SELF_REFERENCING
     */

    sBoolValue = STL_FALSE;
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_SELF_REFERENCING,
                                sDataValueList,
                                & sBoolValue,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_IDENTITY
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_IDENTITY,
                                sDataValueList,
                                & aIsIdentity,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IDENTITY_GENERATION
     */

    if ( aIsIdentity == STL_TRUE )
    {
        if ( aIdentityGenOption == ELL_IDENTITY_GENERATION_NA )
        {
            sIdentityOption = ELL_IDENTITY_GENERATION_DEFAULT;
        }
        else
        {
            sIdentityOption = aIdentityGenOption;
        }
        
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_GENERATION,
                                    sDataValueList,
                                    (void *) gEllIdentityGenOptionString[sIdentityOption],
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_GENERATION,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * IDENTITY_GENERATION_ID
     */

    if ( aIsIdentity == STL_TRUE )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_GENERATION_ID,
                                    sDataValueList,
                                    & sIdentityOption,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_GENERATION_ID,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * IDENTITY_START
     */

    if ( aIsIdentity == STL_TRUE )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_START,
                                    sDataValueList,
                                    & aIdentityStart,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_START,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * IDENTITY_INCREMENT
     */

    if ( aIsIdentity == STL_TRUE )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_INCREMENT,
                                    sDataValueList,
                                    & aIdentityIncrement,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_INCREMENT,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * IDENTITY_MAXIMUM
     */

    if ( aIsIdentity == STL_TRUE )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_MAXIMUM,
                                    sDataValueList,
                                    & aIdentityMaximum,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_MAXIMUM,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * IDENTITY_MINIMUM
     */

    if ( aIsIdentity == STL_TRUE )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_MINIMUM,
                                    sDataValueList,
                                    & aIdentityMinimum,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_MINIMUM,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * IDENTITY_CYCLE
     */

    if ( aIsIdentity == STL_TRUE )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_CYCLE,
                                    sDataValueList,
                                    & aIdentityCycle,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_CYCLE,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * IDENTITY_TABLESPACE_ID
     */

    if ( aIsIdentity == STL_TRUE )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_TABLESPACE_ID,
                                    sDataValueList,
                                    & aIdentityTablespaceID,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_TABLESPACE_ID,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * IDENTITY_PHYSICAL_ID
     */

    if ( aIsIdentity == STL_TRUE )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_PHYSICAL_ID,
                                    sDataValueList,
                                    & aIdentityPhysicalID,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_PHYSICAL_ID,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * IDENTITY_CACHE_SIZE
     */

    if ( aIsIdentity == STL_TRUE )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_CACHE_SIZE,
                                    sDataValueList,
                                    & aIdentityCacheSize,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_CACHE_SIZE,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * IS_GENERATED
     */

    sBoolValue = STL_FALSE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_GENERATED,
                                sDataValueList,
                                & sBoolValue,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_SYSTEM_VERSION_START
     */

    sBoolValue = STL_FALSE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_SYSTEM_VERSION_START,
                                sDataValueList,
                                & sBoolValue,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_SYSTEM_VERSION_END
     */

    sBoolValue = STL_FALSE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_SYSTEM_VERSION_END,
                                sDataValueList,
                                & sBoolValue,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SYSTEM_VERSION_TIMESTAMP_GENERATION
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_SYSTEM_VERSION_TIMESTAMP_GENERATION,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * IS_UPDATABLE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_UPDATABLE,
                                sDataValueList,
                                & aIsUpdatable,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_UNUSED
     */

    sBoolValue = STL_FALSE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_UNUSED,
                                sDataValueList,
                                & sBoolValue,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * COMMENTS
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_COMMENTS,
                                sDataValueList,
                                aColumnComment,
                                aEnv )
             == STL_SUCCESS );

    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_COLUMNS],
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
                                              ELDT_TABLE_ID_COLUMNS,
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
    
    *aColumnID = sColumnID;
    
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
 * @brief Column 을 UNUSED 로 변경한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aColumnID        Column 의 ID
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus eldModifyColumnSetUnused( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    stlInt64       aColumnID,
                                    ellEnv       * aEnv )
{
    knlValueBlockList * sRowValueList = NULL;

    eldMemMark          sMemMark;

    stlBool             sMemAlloc = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );

    /**
     * 갱신을 위한 Record 삭제
     */

    STL_TRY( eldDeleteDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_COLUMNS,
                                       ELDT_Columns_ColumnOrder_COLUMN_ID,
                                       aColumnID,
                                       & sMemMark,
                                       & sRowValueList,
                                       aEnv )
             == STL_SUCCESS );
    sMemAlloc = STL_TRUE;
    
    /**
     * UNUSED column 정보 초기화
     */
    
    STL_TRY( eldSetColumnUnused( sRowValueList, aEnv ) == STL_SUCCESS );
            
    /**
     * 갱신된 Row 추가 
     */

    sMemAlloc = STL_FALSE;
    STL_TRY( eldInsertDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_COLUMNS,
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
 * @brief Column 을 UNUSED 로 설정함.
 * @param[in] aTableValueList  Table Value List
 * @param[in] aEnv             Environment
 * @remarks
 */
stlStatus eldSetColumnUnused( knlValueBlockList        * aTableValueList,
                              ellEnv                   * aEnv )
{
    stlBool  sBoolValue = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableValueList  != NULL, ELL_ERROR_STACK(aEnv) );

    /*
     * OWNER_ID
     * SCHEMA_ID
     * TABLE_ID
     * COLUMN_ID
     */
    
    /********************************
     * COLUMN_NAME
     ********************************/
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_COLUMN_NAME,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * PHYSICAL_ORDINAL_POSITION
     */

    /********************************
     * LOGICAL_ORDINAL_POSITION
     ********************************/

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_LOGICAL_ORDINAL_POSITION,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );
    
    /* 
     * DTD_IDENTIFIER
     */
    
    /**
     * DOMAIN_CATALOG
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_DOMAIN_CATALOG,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * DOMAIN_SCHEMA
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_DOMAIN_SCHEMA,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );
                
    /**
     * DOMAIN_NAME
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_DOMAIN_NAME,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );
                
    /**
     * COLUMN_DEFAULT
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_COLUMN_DEFAULT,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IS_NULLABLE
     */
                
    sBoolValue = STL_TRUE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_NULLABLE,
                                aTableValueList,
                                & sBoolValue,
                                aEnv )
             == STL_SUCCESS );
                
    /**
     * IS_SELF_REFERENCING
     */
                
    sBoolValue = STL_FALSE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_SELF_REFERENCING,
                                aTableValueList,
                                & sBoolValue,
                                aEnv )
             == STL_SUCCESS );
                
    /**
     * IS_IDENTITY
     */
                
    sBoolValue = STL_FALSE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_IDENTITY,
                                aTableValueList,
                                & sBoolValue,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_GENERATION
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_GENERATION,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_GENERATION_ID
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_GENERATION_ID,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_START
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_START,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_INCREMENT
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_INCREMENT,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_MAXIMUM
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_MAXIMUM,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_MINIMUM
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_MINIMUM,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_CYCLE
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_CYCLE,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_TABLESPACE_ID
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_TABLESPACE_ID,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_PHYSICAL_ID
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_PHYSICAL_ID,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );
                
    /**
     * IDENTITY_CACHE_SIZE
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_CACHE_SIZE,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IS_GENERATED
     */
                
    sBoolValue = STL_FALSE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_GENERATED,
                                aTableValueList,
                                & sBoolValue,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IS_SYSTEM_VERSION_START
     */
                
    sBoolValue = STL_FALSE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_SYSTEM_VERSION_START,
                                aTableValueList,
                                & sBoolValue,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IS_SYSTEM_VERSION_END
     */

    sBoolValue = STL_FALSE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_SYSTEM_VERSION_END,
                                aTableValueList,
                                & sBoolValue,
                                aEnv )
             == STL_SUCCESS );

    /**
     * SYSTEM_VERSION_TIMESTAMP_GENERATION
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_SYSTEM_VERSION_TIMESTAMP_GENERATION,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * IS_UPDATABLE
     */

    sBoolValue = STL_FALSE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_UPDATABLE,
                                aTableValueList,
                                & sBoolValue,
                                aEnv )
             == STL_SUCCESS );
                
    /********************************
     * IS_UNUSED
     ********************************/
                
    sBoolValue = STL_TRUE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_UNUSED,
                                aTableValueList,
                                & sBoolValue,
                                aEnv )
             == STL_SUCCESS );
                
    /**
     * COMMENTS
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_COMMENTS,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );
                
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}




/**
 * @brief Column Desciptor 의 Logical 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aColumnID        Column 의 ID
 * @param[in]  aLogicalIdx      Column 의 Logical Ordinal Position
 * @param[in]  aSetNullable     Nullable 로 변경 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus eldModifyColumnLogicalIdx( smlTransId       aTransID,
                                     smlStatement   * aStmt,
                                     stlInt64         aColumnID,
                                     stlInt32         aLogicalIdx,
                                     stlBool          aSetNullable,
                                     ellEnv         * aEnv )
{
    knlValueBlockList * sRowValueList = NULL;

    eldMemMark          sMemMark;

    stlBool             sMemAlloc = STL_FALSE;

    stlInt32 sOrdinalPosition = 0;
    stlBool  sBoolValue = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLogicalIdx >= 0, ELL_ERROR_STACK(aEnv) );
    
    /**
     * 갱신을 위한 Record 삭제
     */

    STL_TRY( eldDeleteDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_COLUMNS,
                                       ELDT_Columns_ColumnOrder_COLUMN_ID,
                                       aColumnID,
                                       & sMemMark,
                                       & sRowValueList,
                                       aEnv )
             == STL_SUCCESS );
    sMemAlloc = STL_TRUE;
    
    /**
     * Logical Ordinal Position 정보 변경 
     */

    sOrdinalPosition = ELD_ORDINAL_POSITION_TO_DATABASE(aLogicalIdx);

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_LOGICAL_ORDINAL_POSITION,
                                sRowValueList,
                                & sOrdinalPosition,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * Nullable 정보 변경
     */

    if ( aSetNullable == STL_TRUE )
    {
        sBoolValue = STL_TRUE;

        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IS_NULLABLE,
                                    sRowValueList,
                                    & sBoolValue,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /*
         * 기존 정보를 그대로 유지
         * nothing to do
         */
    }
        
    /**
     * 갱신된 Row 추가 
     */

    sMemAlloc = STL_FALSE;
    STL_TRY( eldInsertDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_COLUMNS,
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
 * @brief Column Dictionary 정보만을 삭제한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aColumnDictHandle Column Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldDeleteDictColumn( smlTransId       aTransID,
                               smlStatement   * aStmt,
                               ellDictHandle  * aColumnDictHandle,
                               ellEnv         * aEnv )
{
    stlInt64 sColumnID = ELL_DICT_OBJECT_ID_NA;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Column ID 획득
     */
    
    sColumnID = ellGetColumnID( aColumnDictHandle );
    
    /**
     * Column Privilege Record 를 삭제
     */
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.COLUMN_PRIVILEGES
     *  WHERE COLUMN_ID = sTableID;
     */
    
    STL_TRY( eldDeletePrivRowsAndCache( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_COLUMN_PRIVILEGES,
                                        ELDT_ColumnPriv_ColumnOrder_COLUMN_ID,
                                        sColumnID,
                                        aEnv )
             == STL_SUCCESS );

    /**
     * Column Dictionary Record 를 삭제 
     */
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR
     *  WHERE COLUMN_ID = sColumnID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                  ELDT_DTDesc_ColumnOrder_COLUMN_ID,
                                  sColumnID,
                                  aEnv )
             == STL_SUCCESS );

    /*
     * DELETE FROM DEFINITION_SCHEMA.COLUMNS
     *  WHERE COLUMN_ID = sColumnID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_COLUMNS,
                                  ELDT_Columns_ColumnOrder_COLUMN_ID,
                                  sColumnID,
                                  aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief Column Desciptor 의 Identity 속성을 제거한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aColumnID        Column 의 ID
 * @param[in]  aIsNullable      Nullable 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus eldRemoveColumnIdentity( smlTransId       aTransID,
                                   smlStatement   * aStmt,
                                   stlInt64         aColumnID,
                                   stlBool          aIsNullable,
                                   ellEnv         * aEnv )
{
    knlValueBlockList * sRowValueList = NULL;

    eldMemMark          sMemMark;

    stlBool             sMemAlloc = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );

    /**
     * 갱신을 위한 Record 삭제
     */

    STL_TRY( eldDeleteDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_COLUMNS,
                                       ELDT_Columns_ColumnOrder_COLUMN_ID,
                                       aColumnID,
                                       & sMemMark,
                                       & sRowValueList,
                                       aEnv )
             == STL_SUCCESS );
    sMemAlloc = STL_TRUE;
    
    /**
     * Identity 속성 제거
     */

    STL_TRY( eldSetColumnNonIdentity( sRowValueList,
                                      aIsNullable,
                                      aEnv )
             == STL_SUCCESS );
            
    /**
     * 갱신된 Row 추가 
     */

    sMemAlloc = STL_FALSE;
    STL_TRY( eldInsertDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_COLUMNS,
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
 * @brief Column 의 Identity 속성을 제거 
 * @param[in] aTableValueList  Table Value List
 * @param[in] aIsNullable      Nullable 여부 
 * @param[in] aEnv             Environment
 * @remarks
 */
stlStatus eldSetColumnNonIdentity( knlValueBlockList        * aTableValueList,
                                   stlBool                    aIsNullable,
                                   ellEnv                   * aEnv )
{
    stlBool  sBoolValue = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableValueList  != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * IS_IDENTITY
     */
                
    sBoolValue = STL_FALSE;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_IDENTITY,
                                aTableValueList,
                                & sBoolValue,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_GENERATION
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_GENERATION,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_GENERATION_ID
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_GENERATION_ID,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_START
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_START,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_INCREMENT
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_INCREMENT,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_MAXIMUM
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_MAXIMUM,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_MINIMUM
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_MINIMUM,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_CYCLE
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_CYCLE,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_TABLESPACE_ID
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_TABLESPACE_ID,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IDENTITY_PHYSICAL_ID
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_PHYSICAL_ID,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );
                
    /**
     * IDENTITY_CACHE_SIZE
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_CACHE_SIZE,
                                aTableValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IS_NULLABLE
     */
                
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IS_NULLABLE,
                                aTableValueList,
                                & aIsNullable,
                                aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}



/**
 * @brief Column Desciptor 의 Identity 속성을 변경한다.
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aColumnID              Column 의 ID
 * @param[in]  aIdentityGenOption     (nullable) IDENTITY 컬럼의 생성 옵션 
 * @param[in]  aIdentityStart         (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 시작값
 * @param[in]  aIdentityIncrement     (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 증가값
 * @param[in]  aIdentityMaximum       (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 최대값
 * @param[in]  aIdentityMinimum       (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 최소값 
 * @param[in]  aIdentityCycle         (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 Cycle 허용 여부
 * @param[in]  aIdentityPhysicalID    IDENTITY 객체의 Physical ID
 * @param[in]  aIdentityCacheSize     (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 Cache Size
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus eldModifyColumnIdentity( smlTransId             aTransID,
                                   smlStatement         * aStmt,
                                   stlInt64               aColumnID,
                                   ellIdentityGenOption * aIdentityGenOption,
                                   stlInt64             * aIdentityStart,
                                   stlInt64             * aIdentityIncrement,
                                   stlInt64             * aIdentityMaximum,
                                   stlInt64             * aIdentityMinimum,
                                   stlBool              * aIdentityCycle,
                                   stlInt64             * aIdentityPhysicalID,
                                   stlInt64             * aIdentityCacheSize,
                                   ellEnv               * aEnv )
{
    knlValueBlockList * sRowValueList = NULL;

    eldMemMark          sMemMark;

    stlBool             sMemAlloc = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIdentityPhysicalID != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * 갱신을 위한 Record 삭제
     */

    STL_TRY( eldDeleteDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_COLUMNS,
                                       ELDT_Columns_ColumnOrder_COLUMN_ID,
                                       aColumnID,
                                       & sMemMark,
                                       & sRowValueList,
                                       aEnv )
             == STL_SUCCESS );
    sMemAlloc = STL_TRUE;

    /**
     * Identity 속성 변경
     */

    STL_TRY( eldSetColumnIdentityOption( sRowValueList,
                                         aIdentityGenOption,
                                         aIdentityStart,
                                         aIdentityIncrement,
                                         aIdentityMaximum,
                                         aIdentityMinimum,
                                         aIdentityCycle,
                                         aIdentityPhysicalID,
                                         aIdentityCacheSize,
                                         aEnv )
             == STL_SUCCESS );
            
    /**
     * 갱신된 Row 추가 
     */

    sMemAlloc = STL_FALSE;
    STL_TRY( eldInsertDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_COLUMNS,
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
 * @brief Column 의 Identity 속성을 변경
 * @param[in] aTableValueList  Table Value List
 * @param[in]  aIdentityGenOption     (nullable) IDENTITY 컬럼의 생성 옵션 
 * @param[in]  aIdentityStart         (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 시작값
 * @param[in]  aIdentityIncrement     (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 증가값
 * @param[in]  aIdentityMaximum       (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 최대값
 * @param[in]  aIdentityMinimum       (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 최소값 
 * @param[in]  aIdentityCycle         (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 Cycle 허용 여부
 * @param[in]  aIdentityPhysicalID    IDENTITY 객체의 Physical ID
 * @param[in]  aIdentityCacheSize     (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 Cache Size
 * @param[in] aEnv             Environment
 * @remarks
 */
stlStatus eldSetColumnIdentityOption( knlValueBlockList        * aTableValueList,
                                      ellIdentityGenOption     * aIdentityGenOption,
                                      stlInt64                 * aIdentityStart,
                                      stlInt64                 * aIdentityIncrement,
                                      stlInt64                 * aIdentityMaximum,
                                      stlInt64                 * aIdentityMinimum,
                                      stlBool                  * aIdentityCycle,
                                      stlInt64                 * aIdentityPhysicalID,
                                      stlInt64                 * aIdentityCacheSize,
                                      ellEnv                   * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableValueList  != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIdentityPhysicalID != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * IDENTITY_GENERATION
     */

    if ( aIdentityGenOption == NULL )
    {
        /**
         * 변경하지 않음
         */
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_GENERATION,
                                    aTableValueList,
                                    (void *)gEllIdentityGenOptionString[*aIdentityGenOption],
                                    aEnv )
                 == STL_SUCCESS );
    }

    /**
     * IDENTITY_GENERATION_ID
     */

    
    if ( aIdentityGenOption == NULL )
    {
        /**
         * 변경하지 않음
         */
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_GENERATION_ID,
                                    aTableValueList,
                                    aIdentityGenOption,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /**
     * IDENTITY_START
     */

    if ( aIdentityStart == NULL )
    {
        /**
         * 변경하지 않음
         */
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_START,
                                    aTableValueList,
                                    aIdentityStart,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /**
     * IDENTITY_INCREMENT
     */

    if ( aIdentityIncrement == NULL )
    {
        /**
         * 변경하지 않음
         */
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_INCREMENT,
                                    aTableValueList,
                                    aIdentityIncrement,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /**
     * IDENTITY_MAXIMUM
     */

    if ( aIdentityMaximum == NULL )
    {
        /**
         * 변경하지 않음
         */
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_MAXIMUM,
                                    aTableValueList,
                                    aIdentityMaximum,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /**
     * IDENTITY_MINIMUM
     */
                
    if ( aIdentityMinimum == NULL )
    {
        /**
         * 변경하지 않음
         */
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_MINIMUM,
                                    aTableValueList,
                                    aIdentityMinimum,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /**
     * IDENTITY_CYCLE
     */
                
    if ( aIdentityCycle == NULL )
    {
        /**
         * 변경하지 않음
         */
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_CYCLE,
                                    aTableValueList,
                                    aIdentityCycle,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /**
     * IDENTITY_TABLESPACE_ID
     * - 변경하지 않음
     */
                

    /**
     * IDENTITY_PHYSICAL_ID
     * - 항상 변경됨
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_IDENTITY_PHYSICAL_ID,
                                aTableValueList,
                                aIdentityPhysicalID,
                                aEnv )
             == STL_SUCCESS );
                
    /**
     * IDENTITY_CACHE_SIZE
     */

    if ( aIdentityCacheSize == NULL )
    {
        /**
         * 변경하지 않음
         */
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMNS,
                                    ELDT_Columns_ColumnOrder_IDENTITY_CACHE_SIZE,
                                    aTableValueList,
                                    aIdentityCacheSize,
                                    aEnv )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief DEFINITION_SCHEMA 구축후 재구동시 사용할 Identity Column 의 현재값을 설정한다.
 * @param[in]  aEnv    Environment
 * @remarks
 * 최초 DB 생성시에만 사용되는 함수임.
 */
stlStatus eldFinishSetValueIdentityColumn( ellEnv * aEnv )
{
    stlInt32 i = 0;
    smlSequenceAttr sSeqAttr;

    /**
     * Identity Column 들의 현재값 설정
     */
    for ( i = 0; i < ELDT_TABLE_ID_MAX; i++ )
    {
        if ( gEldIdentityColumnHandle[i] != NULL )
        {
            if ( gEldtIdentityValue[i] > 0 )
            {
                STL_TRY( smlGetSequenceAttr( gEldIdentityColumnHandle[i],
                                             & sSeqAttr,
                                             SML_ENV(aEnv) )
                         == STL_SUCCESS );

                if ( gEldtIdentityValue[i] == sSeqAttr.mRestartValue )
                {
                    STL_TRY( smlSetCurrSequenceVal( gEldIdentityColumnHandle[i],
                                                    gEldtIdentityValue[i] + 1,
                                                    SML_ENV( aEnv ) )
                             == STL_SUCCESS );
                }
                else if ( gEldtIdentityValue[i] > sSeqAttr.mRestartValue )
                {
                    STL_TRY( smlSetCurrSequenceVal( gEldIdentityColumnHandle[i],
                                                    gEldtIdentityValue[i],
                                                    SML_ENV( aEnv ) )
                             == STL_SUCCESS );
                }
                else
                {
                    /* META SYNC 과정에서 발생할 수 있음 */
                }
            }
            else
            {
                /* 설정할 필요 없음 */
            }
        }
        else
        {
            /* Identity Column 이 없는 Dictionary Table 임 */
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/***********************************************************************
 * Handle 획득 함수 
 ***********************************************************************/





/**
 * @brief Column ID 를 이용해 Column Handle 을 얻는다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aColumnID          Column ID
 * @param[out] aColumnDictHandle  Column Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus eldGetColumnHandleByID( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  stlInt64             aColumnID,
                                  ellDictHandle      * aColumnDictHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv )
{
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aColumnID != ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Column Handle 검색
     */

    STL_TRY( eldcGetColumnHandleByID( aTransID,
                                      aViewSCN,
                                      aColumnID,
                                      aColumnDictHandle,
                                      aExist,
                                      aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Column Name 을 이용해 Column Handle 을 얻는다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aTableID           Table ID
 * @param[in]  aColumnName        Column Name
 * @param[out] aColumnDictHandle  Column Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus eldGetColumnHandleByName( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aTableID,
                                    stlChar            * aColumnName,
                                    ellDictHandle      * aColumnDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv )
{
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableID != ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Column Handle 검색
     */
    
    STL_TRY( eldcGetColumnHandleByName( aTransID,
                                        aViewSCN,
                                        aTableID,
                                        aColumnName,
                                        aColumnDictHandle,
                                        aExist,
                                        aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/** @} eldColumn */
