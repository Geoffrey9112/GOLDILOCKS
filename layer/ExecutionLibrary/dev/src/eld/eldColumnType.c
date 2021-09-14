/*******************************************************************************
 * eldColumnType.c
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
 * @file eldColumnType.c
 * @brief Column Type Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldColumnType.h>

/**
 * @addtogroup eldColumnType
 * @{
 */

/*
 * Character String 유형의 컬럼
 */

/**
 * @brief Length Unit 과 입력 길이값을 통해 CHARACTER LENGTH 값을 구한다.
 * @param[in] aLengthUnit      Length Unit
 * @param[in] aLength          Length Value
 * @remarks
 */
stlInt64  eldGetCharacterLength( dtlStringLengthUnit aLengthUnit,
                                 stlInt64            aLength )
{
    stlInt64 sCharLen = 0;

    STL_ASSERT( (aLengthUnit >= DTL_STRING_LENGTH_UNIT_NA) &&
                (aLengthUnit < DTL_STRING_LENGTH_UNIT_MAX) );
    
    switch ( aLengthUnit )
    {
        case DTL_STRING_LENGTH_UNIT_NA:
        case DTL_STRING_LENGTH_UNIT_CHARACTERS:
            {
                sCharLen = aLength;
                break;
            }
        case DTL_STRING_LENGTH_UNIT_OCTETS:
            {
                /**
                 * Length Unit 이 OCTET 일 경우
                 * Octet Length 와 Charcter Length 는 동일한 값을 갖는다.
                 */
                sCharLen = aLength;
                break;
            }
        default:
            {
                STL_ASSERT( 0 );
                break;
            }
    }

    return sCharLen;
}


/**
 * @brief Length Unit 과 입력 길이값을 통해 OCTET LENGTH 값을 구한다.
 * @param[in] aLengthUnit      Length Unit
 * @param[in] aLength          Length Value
 * @param[in] aEnv             Environment
 * @remarks
 */
stlInt64  eldGetOctetLength( dtlStringLengthUnit   aLengthUnit,
                             stlInt64              aLength,
                             ellEnv              * aEnv )
{
    stlInt8  sMaxOctetByChar = 0;
    stlInt64 sOctetLen = 0;

    dtlStringLengthUnit   sUnit;

    STL_ASSERT( (aLengthUnit >= DTL_STRING_LENGTH_UNIT_NA) &&
                (aLengthUnit < DTL_STRING_LENGTH_UNIT_MAX) );

    if ( aLengthUnit == DTL_STRING_LENGTH_UNIT_NA  )
    {
        if ( gEllIsMetaBuilding == STL_TRUE )
        {
            sUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
        }
        else
        {
            sUnit = ellGetDbCharLengthUnit();
        }
    }
    else
    {
        sUnit = aLengthUnit;
    }
    
    switch ( sUnit )
    {
        case DTL_STRING_LENGTH_UNIT_CHARACTERS:
            {
                sMaxOctetByChar =
                    dtlGetMbMaxLength(
                        ellGetDbCharacterSet() );
                sOctetLen = aLength * sMaxOctetByChar;
                break;
            }
        case DTL_STRING_LENGTH_UNIT_OCTETS:
            {
                sOctetLen = aLength;
                break;
            }
        default:
            {
                STL_ASSERT( 0 );
                break;
            }
    }

    return sOctetLen;
}





/**
 * @brief 다양한 Data Type 을 하나의 Data Type Descriptor 정보로 Dictionary 에 추가한다.
 * @param[in] aTransID              Transaction ID
 * @param[in] aStmt                 Statement
 * @param[in] aOwnerID              Owner ID
 * @param[in] aSchemaID             Schema ID
 * @param[in] aTableID              Table ID
 * @param[in] aColumnID             Column ID
 * @param[in] aNormalTypeName       Data Type 의 정규화된 이름
 * @param[in] aDataTypeID           Data Type ID (현재는 Built-In Type ID만 가능)
 * @param[in] aLengthUnit           String Length Unit
 * @param[in] aCharMaxLength        String 타입의 Character Length
 * @param[in] aCharOctetLength      String 타입의 Octet Length
 * @param[in] aNumPrec              숫자형 타입의 Precision
 * @param[in] aNumPrecRadix         숫자형 타입의 Precision Radix
 * @param[in] aNumScale             숫자형 타입의 Scale
 * @param[in] aDeclaredTypeName     사용자가 정의한 Data Type 이름
 * @param[in] aDeclaredPrecOrLen    사용자가 정의한 Precision 이나 Length
 *                         <BR> - Character String Type : character length
 *                         <BR> - Binary String Type : octet length
 *                         <BR> - 숫자형 Type : precision
 *                         <BR> - 시간형, Interval Type : fractional second precision
 * @param[in] aDeclaredPrecOrScale  사용자가 정의한 Precision 이나 Scale
 *                         <BR> - 숫자형 Type : scale
 *                         <BR> - Interval Type : leading precision
 * @param[in] aDatetimePrec         날짜시간형 타입의 Precision
 *                         <BR> - 시간형, Interval Type : fractional second precision
 * @param[in] aIntervalType         Interval 타입의 유형 이름
 * @param[in] aIntervalTypeID       Interval 타입의 유형 ID
 * @param[in] aIntervalPrec         Interval 타입의 Precision
 *                         <BR> - Interval Type : leading precision
 * @param[in] aPhysicalLength       Physical Byte Length of the data type defintion                        
 * @param[in] aEnv                  Environment
 * @remarks
 */
stlStatus eldInsertColumnTypeDesc( smlTransId            aTransID,
                                   smlStatement        * aStmt,
                                   stlInt64            * aOwnerID,
                                   stlInt64            * aSchemaID,
                                   stlInt64            * aTableID,
                                   stlInt64            * aColumnID,
                                   stlChar             * aNormalTypeName,
                                   stlInt32            * aDataTypeID,
                                   dtlStringLengthUnit * aLengthUnit,
                                   stlInt64            * aCharMaxLength,
                                   stlInt64            * aCharOctetLength,
                                   stlInt32            * aNumPrec,
                                   stlInt32            * aNumPrecRadix,
                                   stlInt32            * aNumScale,
                                   stlChar             * aDeclaredTypeName,
                                   stlInt64            * aDeclaredPrecOrLen,
                                   stlInt64            * aDeclaredPrecOrScale,
                                   stlInt32            * aDatetimePrec,
                                   stlChar             * aIntervalType,
                                   stlInt32            * aIntervalTypeID,
                                   stlInt32            * aIntervalPrec,
                                   stlInt64              aPhysicalLength,
                                   ellEnv              * aEnv )
{
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid          sRowRid;
    smlScn          sRowScn;
    smlRowBlock     sRowBlock;
    eldMemMark      sMemMark;

    stlInt32   sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOwnerID != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaID != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnID != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNormalTypeName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataTypeID != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPhysicalLength > 0, ELL_ERROR_STACK(aEnv) );

    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
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
     * - column 의 data type descriptor의 정보를 구성한다.
     */

    /*
     * OWNER_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_OWNER_ID,
                                sDataValueList,
                                aOwnerID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCHEMA_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_SCHEMA_ID,
                                sDataValueList,
                                aSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * TABLE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_TABLE_ID,
                                sDataValueList,
                                aTableID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COLUMN_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_COLUMN_ID,
                                sDataValueList,
                                aColumnID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * OBJECT_CATALOG
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_OBJECT_CATALOG,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * OBJECT_SCHEMA
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_OBJECT_SCHEMA,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * OBJECT_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_OBJECT_NAME,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * OBJECT_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_OBJECT_TYPE,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DTD_IDENTIFIER
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_DTD_IDENTIFIER,
                                sDataValueList,
                                aColumnID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DATA_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_DATA_TYPE,
                                sDataValueList,
                                aNormalTypeName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DATA_TYPE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_DATA_TYPE_ID,
                                sDataValueList,
                                aDataTypeID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * CHARACTER_SET_CATALOG
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_CHARACTER_SET_CATALOG,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CHARACTER_SET_SCHEMA
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_CHARACTER_SET_SCHEMA,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CHARACTER_SET_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_CHARACTER_SET_NAME,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * STRING_LEGNTH_UNIT
     */

    if ( aLengthUnit == NULL )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                    ELDT_DTDesc_ColumnOrder_STRING_LENGTH_UNIT,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                    ELDT_DTDesc_ColumnOrder_STRING_LENGTH_UNIT,
                                    sDataValueList,
                                    gDtlLengthUnitString[*aLengthUnit],
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * STRING_LEGNTH_UNIT_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_STRING_LENGTH_UNIT_ID,
                                sDataValueList,
                                aLengthUnit,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * CHARACTER_MAXIMUM_LENGTH
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_CHARACTER_MAXIMUM_LENGTH,
                                sDataValueList,
                                aCharMaxLength,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CHARACTER_OCTET_LENGTH
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_CHARACTER_OCTET_LENGTH,
                                sDataValueList,
                                aCharOctetLength,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COLLATION_CATALOG
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_COLLATION_CATALOG,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COLLATION_SCHEMA
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_COLLATION_SCHEMA,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COLLATION_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_COLLATION_NAME,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * NUMERIC_PRECISION
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_NUMERIC_PRECISION,
                                sDataValueList,
                                aNumPrec,
                                aEnv )
             == STL_SUCCESS );

    /*
     * NUMERIC_PRECISION_RADIX
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_NUMERIC_PRECISION_RADIX,
                                sDataValueList,
                                aNumPrecRadix,
                                aEnv )
             == STL_SUCCESS );

    /*
     * NUMERIC_SCALE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_NUMERIC_SCALE,
                                sDataValueList,
                                aNumScale,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DECLARED_DATA_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_DECLARED_DATA_TYPE,
                                sDataValueList,
                                aDeclaredTypeName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DECLARED_NUMERIC_PRECISION
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_DECLARED_NUMERIC_PRECISION,
                                sDataValueList,
                                aDeclaredPrecOrLen,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DECLARED_NUMERIC_SCALE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_DECLARED_NUMERIC_SCALE,
                                sDataValueList,
                                aDeclaredPrecOrScale,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * DATETIME_PRECISION
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_DATETIME_PRECISION,
                                sDataValueList,
                                aDatetimePrec,
                                aEnv )
             == STL_SUCCESS );

    /*
     * INTERVAL_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_INTERVAL_TYPE,
                                sDataValueList,
                                aIntervalType,
                                aEnv )
             == STL_SUCCESS );

    /*
     * INTERVAL_TYPE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_INTERVAL_TYPE_ID,
                                sDataValueList,
                                aIntervalTypeID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * INTERVAL_PRECISION
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_INTERVAL_PRECISION,
                                sDataValueList,
                                aIntervalPrec,
                                aEnv )
             == STL_SUCCESS );

    /*
     * USER_DEFINED_TYPE_CATALOG
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_USER_DEFINED_TYPE_CATALOG,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * USER_DEFINED_TYPE_SCHEMA
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_USER_DEFINED_TYPE_SCHEMA,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * USER_DEFINED_TYPE_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_USER_DEFINED_TYPE_NAME,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCOPE_CATALOG
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_SCOPE_CATALOG,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCOPE_SCHEMA
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_SCOPE_SCHEMA,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCOPE_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_SCOPE_NAME,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * MAXIMUM_CARDINALITY
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_MAXIMUM_CARDINALITY,
                                sDataValueList,
                                NULL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * PHYSICAL_MAXIMUM_LENGTH
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_PHYSICAL_MAXIMUM_LENGTH,
                                sDataValueList,
                                & aPhysicalLength,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR],
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
                                              ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
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
 * @brief Column 의 Data Type Descriptor 정보를 변경한다.
 * @param[in] aTransID              Transaction ID
 * @param[in] aStmt                 Statement
 * @param[in] aColumnID             Column ID
 * @param[in] aNormalTypeName       Data Type 의 정규화된 이름
 * @param[in] aDataTypeID           Data Type ID (현재는 Built-In Type ID만 가능)
 * @param[in] aLengthUnit           String Length Unit
 * @param[in] aCharMaxLength        String 타입의 Character Length
 * @param[in] aCharOctetLength      String 타입의 Octet Length
 * @param[in] aNumPrec              숫자형 타입의 Precision
 * @param[in] aNumPrecRadix         숫자형 타입의 Precision Radix
 * @param[in] aNumScale             숫자형 타입의 Scale
 * @param[in] aDeclaredTypeName     사용자가 정의한 Data Type 이름
 * @param[in] aDeclaredPrecOrLen    사용자가 정의한 Precision 이나 Length
 *                         <BR> - Character String Type : character length
 *                         <BR> - Binary String Type : octet length
 *                         <BR> - 숫자형 Type : precision
 *                         <BR> - 시간형, Interval Type : fractional second precision
 * @param[in] aDeclaredPrecOrScale  사용자가 정의한 Precision 이나 Scale
 *                         <BR> - 숫자형 Type : scale
 *                         <BR> - Interval Type : leading precision
 * @param[in] aDatetimePrec         날짜시간형 타입의 Precision
 *                         <BR> - 시간형, Interval Type : fractional second precision
 * @param[in] aIntervalType         Interval 타입의 유형 이름
 * @param[in] aIntervalTypeID       Interval 타입의 유형 ID
 * @param[in] aIntervalPrec         Interval 타입의 Precision
 *                         <BR> - Interval Type : leading precision
 * @param[in] aPhysicalLength       Maximum Physical Length of the column definition                        
 * @param[in] aEnv                  Environment
 * @remarks
 */
stlStatus eldModifyColumnTypeDesc( smlTransId            aTransID,
                                   smlStatement        * aStmt,
                                   stlInt64              aColumnID,
                                   stlChar             * aNormalTypeName,
                                   stlInt32            * aDataTypeID,
                                   dtlStringLengthUnit * aLengthUnit,
                                   stlInt64            * aCharMaxLength,
                                   stlInt64            * aCharOctetLength,
                                   stlInt32            * aNumPrec,
                                   stlInt32            * aNumPrecRadix,
                                   stlInt32            * aNumScale,
                                   stlChar             * aDeclaredTypeName,
                                   stlInt64            * aDeclaredPrecOrLen,
                                   stlInt64            * aDeclaredPrecOrScale,
                                   stlInt32            * aDatetimePrec,
                                   stlChar             * aIntervalType,
                                   stlInt32            * aIntervalTypeID,
                                   stlInt32            * aIntervalPrec,
                                   stlInt64              aPhysicalLength,
                                   ellEnv              * aEnv )
{
    knlValueBlockList * sRowValueList = NULL;

    eldMemMark          sMemMark;

    stlBool             sMemAlloc = STL_FALSE;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNormalTypeName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataTypeID != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDeclaredTypeName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPhysicalLength > 0, ELL_ERROR_STACK(aEnv) );

    /**
     * 갱신을 위한 Record 삭제
     */

    STL_TRY( eldDeleteDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                       ELDT_DTDesc_ColumnOrder_DTD_IDENTIFIER,
                                       aColumnID,
                                       & sMemMark,
                                       & sRowValueList,
                                       aEnv )
             == STL_SUCCESS );
    sMemAlloc = STL_TRUE;
    
    /****************************************************************
     * 변경할 정보 설정 
     ****************************************************************/

    /**
     * OWNER_ID
     * SCHEMA_ID
     * TABLE_ID
     * COLUMN_ID
     * OBJECT_CATALOG
     * OBJECT_SCHEMA
     * OBJECT_NAME
     * OBJECT_TYPE
     * DTD_IDENTIFIER
     */

    /*
     * DATA_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_DATA_TYPE,
                                sRowValueList,
                                aNormalTypeName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DATA_TYPE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_DATA_TYPE_ID,
                                sRowValueList,
                                aDataTypeID,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * CHARACTER_SET_CATALOG
     * CHARACTER_SET_SCHEMA
     * CHARACTER_SET_NAME
     */

    /*
     * STRING_LEGNTH_UNIT
     */

    if ( aLengthUnit == NULL )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                    ELDT_DTDesc_ColumnOrder_STRING_LENGTH_UNIT,
                                    sRowValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                    ELDT_DTDesc_ColumnOrder_STRING_LENGTH_UNIT,
                                    sRowValueList,
                                    gDtlLengthUnitString[*aLengthUnit],
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * STRING_LEGNTH_UNIT_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_STRING_LENGTH_UNIT_ID,
                                sRowValueList,
                                aLengthUnit,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * CHARACTER_MAXIMUM_LENGTH
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_CHARACTER_MAXIMUM_LENGTH,
                                sRowValueList,
                                aCharMaxLength,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CHARACTER_OCTET_LENGTH
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_CHARACTER_OCTET_LENGTH,
                                sRowValueList,
                                aCharOctetLength,
                                aEnv )
             == STL_SUCCESS );

    /**
     * COLLATION_CATALOG
     * COLLATION_SCHEMA
     * COLLATION_NAME
     */

    /*
     * NUMERIC_PRECISION
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_NUMERIC_PRECISION,
                                sRowValueList,
                                aNumPrec,
                                aEnv )
             == STL_SUCCESS );

    /*
     * NUMERIC_PRECISION_RADIX
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_NUMERIC_PRECISION_RADIX,
                                sRowValueList,
                                aNumPrecRadix,
                                aEnv )
             == STL_SUCCESS );

    /*
     * NUMERIC_SCALE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_NUMERIC_SCALE,
                                sRowValueList,
                                aNumScale,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DECLARED_DATA_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_DECLARED_DATA_TYPE,
                                sRowValueList,
                                aDeclaredTypeName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DECLARED_NUMERIC_PRECISION
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_DECLARED_NUMERIC_PRECISION,
                                sRowValueList,
                                aDeclaredPrecOrLen,
                                aEnv )
             == STL_SUCCESS );

    /*
     * DECLARED_NUMERIC_SCALE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_DECLARED_NUMERIC_SCALE,
                                sRowValueList,
                                aDeclaredPrecOrScale,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * DATETIME_PRECISION
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_DATETIME_PRECISION,
                                sRowValueList,
                                aDatetimePrec,
                                aEnv )
             == STL_SUCCESS );

    /*
     * INTERVAL_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_INTERVAL_TYPE,
                                sRowValueList,
                                aIntervalType,
                                aEnv )
             == STL_SUCCESS );

    /*
     * INTERVAL_TYPE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_INTERVAL_TYPE_ID,
                                sRowValueList,
                                aIntervalTypeID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * INTERVAL_PRECISION
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_INTERVAL_PRECISION,
                                sRowValueList,
                                aIntervalPrec,
                                aEnv )
             == STL_SUCCESS );

    /**
     * USER_DEFINED_TYPE_CATALOG
     * USER_DEFINED_TYPE_SCHEMA
     * USER_DEFINED_TYPE_NAME
     * SCOPE_CATALOG
     * SCOPE_SCHEMA
     * SCOPE_NAME
     * MAXIMUM_CARDINALITY
     */

    /*
     * PHYSICAL_MAXIMUM_LENGTH
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                ELDT_DTDesc_ColumnOrder_PHYSICAL_MAXIMUM_LENGTH,
                                sRowValueList,
                                & aPhysicalLength,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * 갱신된 Row 추가 
     */

    sMemAlloc = STL_FALSE;
    STL_TRY( eldInsertDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
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




/** @} eldColumnType */

