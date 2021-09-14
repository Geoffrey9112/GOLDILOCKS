/*******************************************************************************
 * eldBuiltInDataType.c
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
 * @file eldBuiltInDataType.c
 * @brief Built-In Data Type Dictionary 관리 모듈
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldBuiltInDataType.h>

/**
 * @addtogroup eldBuiltInDataType 
 * @{
 */

/**
 * @brief Built-In Data Type Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertBuiltInTypeDesc() 주석 참조 
 */
stlStatus eldInsertBuiltInTypeDesc( smlTransId                      aTransID,
                                    smlStatement                  * aStmt,
                                    stlChar                       * aTypeName,
                                    stlInt32                        aODBCDataType,
                                    stlInt32                        aJDBCDataType,
                                    stlInt32                        aColumnSize,
                                    stlChar                       * aLiteralPrefix,
                                    stlChar                       * aLiteralSuffix,
                                    stlChar                       * aCreateParams,
                                    stlInt32                        aNullable,
                                    stlBool                         aCaseSensitive,
                                    dtlSearchable                   aSearchable,
                                    stlInt32                        aUnsignedAttribute,
                                    stlBool                         aFixedPrecScale,
                                    stlInt32                        aAutoUniqueValue,
                                    stlChar                       * aLocalTypeName,
                                    stlInt32                        aMinimumScale,
                                    stlInt32                        aMaximumScale,
                                    stlInt32                        aSQLDataType,
                                    stlInt32                        aSQLDateTimeSub,
                                    dtlNumericPrecRadix             aNumPrecRadix,
                                    stlInt32                        aIntervalPrecision,
                                    stlBool                         aSupportedFeature,
                                    ellEnv                        * aEnv )
{
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid      sRowRid;
    smlScn      sRowScn;
    smlRowBlock sRowBlock;

    stlInt32    sSearchable;
    stlBool     sUnsignedAttribute;
    stlBool     sAutoUniqueValue;
    stlInt32    sNumPrecRadix;
    eldMemMark  sMemMark;

    stlInt32   sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTypeName != NULL, ELL_ERROR_STACK(aEnv) );

    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_TYPE_INFO,
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
     * - built-in data type descriptor의 정보를 구성한다.
     */

    /*
     * TYPE_NAME
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                ELDT_TypeInfo_ColumnOrder_TYPE_NAME,
                                sDataValueList,
                                aTypeName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * ODBC_DATA_TYPE
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                ELDT_TypeInfo_ColumnOrder_ODBC_DATA_TYPE,
                                sDataValueList,
                                & aODBCDataType,
                                aEnv )
             == STL_SUCCESS );

    /*
     * JDBC_DATA_TYPE
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                ELDT_TypeInfo_ColumnOrder_JDBC_DATA_TYPE,
                                sDataValueList,
                                & aJDBCDataType,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COLUMN_SIZE
     */

    if( aColumnSize == DTL_PRECISION_NA )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_COLUMN_SIZE,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_COLUMN_SIZE,
                                    sDataValueList,
                                    & aColumnSize,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * LITERAL_PREFIX
     */

    if( aLiteralPrefix == NULL )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_LITERAL_PREFIX,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_LITERAL_PREFIX,
                                    sDataValueList,
                                    aLiteralPrefix,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * LITERAL_SUFFIX
     */

    if( aLiteralSuffix == NULL )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_LITERAL_SUFFIX,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_LITERAL_SUFFIX,
                                    sDataValueList,
                                    aLiteralSuffix,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * CREATE_PARAMS
     */

    if( aCreateParams == NULL )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_CREATE_PARAMS,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_CREATE_PARAMS,
                                    sDataValueList,
                                    aCreateParams,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * NULLABLE
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                ELDT_TypeInfo_ColumnOrder_NULLABLE,
                                sDataValueList,
                                & aNullable,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * CASE_SENSITIVE
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                ELDT_TypeInfo_ColumnOrder_CASE_SENSITIVE,
                                sDataValueList,
                                & aCaseSensitive,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SEARCHABLE
     */

    sSearchable = aSearchable;

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                ELDT_TypeInfo_ColumnOrder_SEARCHABLE,
                                sDataValueList,
                                & sSearchable,
                                aEnv )
             == STL_SUCCESS );

    /*
     * UNSIGNED_ATTRIBUTE
     */

    if( aUnsignedAttribute == ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_UNSIGNED_ATTRIBUTE,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        if( aUnsignedAttribute == ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_TRUE )
        {
            sUnsignedAttribute = STL_TRUE;
        }
        else
        {
            sUnsignedAttribute = STL_FALSE;
        }

        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_UNSIGNED_ATTRIBUTE,
                                    sDataValueList,
                                    & sUnsignedAttribute,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * FIXED_PREC_SCALE
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                ELDT_TypeInfo_ColumnOrder_FIXED_PREC_SCALE,
                                sDataValueList,
                                & aFixedPrecScale,
                                aEnv )
             == STL_SUCCESS );

    /*
     * AUTO_UNIQUE_VALUE
     */

    if( aAutoUniqueValue == ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_AUTO_UNIQUE_VALUE,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        if( aAutoUniqueValue == ELDT_TypeInfo_AUTO_UNIQUE_VALUE_TRUE )
        {
            sAutoUniqueValue = STL_TRUE;
        }
        else
        {
            sAutoUniqueValue = STL_FALSE;
        }

        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_AUTO_UNIQUE_VALUE,
                                    sDataValueList,
                                    & sAutoUniqueValue,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * LOCAL_TYPE_NAME
     */

    if( aLocalTypeName == NULL )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_LOCAL_TYPE_NAME,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_LOCAL_TYPE_NAME,
                                    sDataValueList,
                                    aLocalTypeName,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * MINIMUM_SCALE
     */

    if( aMinimumScale == DTL_SCALE_NA )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_MINIMUM_SCALE,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_MINIMUM_SCALE,
                                    sDataValueList,
                                    & aMinimumScale,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * MAXIMUM_SCALE
     */

    if( aMaximumScale == DTL_SCALE_NA )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_MAXIMUM_SCALE,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_MAXIMUM_SCALE,
                                    sDataValueList,
                                    & aMaximumScale,
                                    aEnv )
                 == STL_SUCCESS );
    }


    /*
     * SQL_DATA_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                ELDT_TypeInfo_ColumnOrder_SQL_DATA_TYPE,
                                sDataValueList,
                                & aSQLDataType,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SQL_DATETIME_SUB
     */

    if( aSQLDateTimeSub == ELDT_TypeInfo_SQL_DATETIME_SUB_NA )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_SQL_DATETIME_SUB,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_SQL_DATETIME_SUB,
                                    sDataValueList,
                                    & aSQLDateTimeSub,
                                    aEnv )
                 == STL_SUCCESS );
    }
    
    /*
     * NUM_PREC_RADIX
     */

    if ( aNumPrecRadix == DTL_NUMERIC_PREC_RADIX_NA )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_NUM_PREC_RADIX,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sNumPrecRadix = aNumPrecRadix;

        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_NUM_PREC_RADIX,
                                    sDataValueList,
                                    & sNumPrecRadix,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * INTERVAL_PRECISION
     */

    if( aIntervalPrecision == DTL_PRECISION_NA )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_INTERVAL_PRECISION,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                    ELDT_TypeInfo_ColumnOrder_INTERVAL_PRECISION,
                                    sDataValueList,
                                    & aIntervalPrecision,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * IS_SUPPORTED
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TYPE_INFO,
                                ELDT_TypeInfo_ColumnOrder_IS_SUPPORTED,
                                sDataValueList,
                                & aSupportedFeature,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * - 레코드를 넣는다.
     */
                 
    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );

    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_TYPE_INFO],
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
                                              ELDT_TABLE_ID_TYPE_INFO,
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



/** @} eldBuiltInDataType */
