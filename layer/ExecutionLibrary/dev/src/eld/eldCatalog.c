/*******************************************************************************
 * eldCatalog.c
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
 * @file eldCatalog.c
 * @brief Catalog Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldCatalog.h>

/**
 * @addtogroup eldCatalog
 * @{
 */

/**
 * @brief Dictionary 에 Catalog(database) Descriptor를 추가한다.
 * @param[in]     aTransID                Transaction ID
 * @param[in]     aStmt                   Statement
 * @param[in]     aDatabaseName           Database 의 이름 (null-terminated)
 * @param[in]     aDatabaseComment        Database 의 주석 (nullable)
 * @param[in]     aCharacterSetID         Character Set의 ID
 * @param[in]     aCharacterSetName       Character Set의 이름
 * @param[in]     aCharLengthUnit         Char Length Unit
 * @param[in]     aTimeZoneIntervalString Time Zone Interval String
 *                                   <BR> ex) "INTERVAL '+09:00' HOUR TO MINUTE"
 * @param[in]     aDefaultDataTablespaceID  Default Data Tablespace ID                      
 * @param[in]     aDefaultTempTablespaceID  Default Temp Tablespace ID                      
 * @param[in]     aEnv                    Execution Library Environment
 * @remarks
 * Database 생성과정에서 호출되며, 외부 layer에서 호출하지 않는다.
 */

stlStatus eldInsertCatalogDesc( smlTransId            aTransID,
                                smlStatement        * aStmt,
                                stlChar             * aDatabaseName,
                                stlChar             * aDatabaseComment,
                                dtlCharacterSet       aCharacterSetID,
                                stlChar             * aCharacterSetName,
                                dtlStringLengthUnit   aCharLengthUnit,
                                stlChar             * aTimeZoneIntervalString,
                                stlInt64              aDefaultDataTablespaceID,
                                stlInt64              aDefaultTempTablespaceID,
                                ellEnv              * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    knlValueBlockList        * sDataValueList  = NULL;
    
    stlInt64     sCatalogID = ELDT_DEFAULT_CATALOG_ID;
    
    smlRid       sRowRid;
    smlScn       sRowScn;
    smlRowBlock  sRowBlock;
    eldMemMark   sMemMark;

    stlInt32  sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDatabaseName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aCharacterSetID >= DTL_SQL_ASCII) &&
                        (aCharacterSetID < DTL_CHARACTERSET_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aCharLengthUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS) ||
                        (aCharLengthUnit == DTL_STRING_LENGTH_UNIT_OCTETS),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCharacterSetName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTimeZoneIntervalString != NULL, ELL_ERROR_STACK(aEnv) );

    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_CATALOG_NAME,
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
     * - catalog descriptor의 정보를 구성한다.
     */

    /*
     * CATALOG_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CATALOG_NAME,
                                ELDT_Catalog_ColumnOrder_CATALOG_ID,
                                sDataValueList,
                                & sCatalogID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * CATALOG_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CATALOG_NAME,
                                ELDT_Catalog_ColumnOrder_CATALOG_NAME,
                                sDataValueList,
                                aDatabaseName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CHARACTER_SET_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CATALOG_NAME,
                                ELDT_Catalog_ColumnOrder_CHARACTER_SET_ID,
                                sDataValueList,
                                & aCharacterSetID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CHARACTER_SET_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CATALOG_NAME,
                                ELDT_Catalog_ColumnOrder_CHARACTER_SET_NAME,
                                sDataValueList,
                                aCharacterSetName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CHAR_LEGNTH_UNITS_ID
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CATALOG_NAME,
                                ELDT_Catalog_ColumnOrder_CHAR_LENGTH_UNITS_ID,
                                sDataValueList,
                                & aCharLengthUnit,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CHAR_LEGNTH_UNITS
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CATALOG_NAME,
                                ELDT_Catalog_ColumnOrder_CHAR_LENGTH_UNITS,
                                sDataValueList,
                                gDtlLengthUnitString[aCharLengthUnit],
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * TIME_ZONE_INTERVAL
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CATALOG_NAME,
                                ELDT_Catalog_ColumnOrder_TIME_ZONE_INTERVAL,
                                sDataValueList,
                                aTimeZoneIntervalString,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DEFAULT_DATA_TABLESPACE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CATALOG_NAME,
                                ELDT_Catalog_ColumnOrder_DEFAULT_DATA_TABLESPACE_ID,
                                sDataValueList,
                                & aDefaultDataTablespaceID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * DEFAULT_TEMP_TABLESPACE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CATALOG_NAME,
                                ELDT_Catalog_ColumnOrder_DEFAULT_TEMP_TABLESPACE_ID,
                                sDataValueList,
                                & aDefaultTempTablespaceID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * CREATED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CATALOG_NAME,
                                ELDT_Catalog_ColumnOrder_CREATED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * MODIFIED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CATALOG_NAME,
                                ELDT_Catalog_ColumnOrder_MODIFIED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COMMENTS
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_CATALOG_NAME,
                                ELDT_Catalog_ColumnOrder_COMMENTS,
                                sDataValueList,
                                aDatabaseComment,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_CATALOG_NAME],
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
                                              ELDT_TABLE_ID_CATALOG_NAME,
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


/** @} eldCatalog */
