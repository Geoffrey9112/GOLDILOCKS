/*******************************************************************************
 * eldcFxColumnDesc.c
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
 * @file eldcFxColumnDesc.c
 * @brief Cache for Fixed Column descriptor
 */

#include <ell.h>
#include <eldt.h>                               
#include <eldc.h>

#include <eldDictionary.h>


/**
 * @addtogroup eldcFxColumnDesc
 * @{
 */





/**
 * @brief Fixed Column 을 위한 Fixed Cache 를 구축한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildCacheFxColumn( smlTransId       aTransID,
                                  smlStatement   * aStmt,
                                  ellEnv         * aEnv )
{
    knlValueBlockList * sValueList = NULL;
    
    stlBool             sBeginSelect = STL_FALSE;
    
    void              * sFxTableHandle = NULL;
    void              * sFxTableHeapDesc = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;

    smlFetchInfo          sFetchInfo;

    eldMemMark  sMemMark;
    stlInt32    sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * X$COLUMNS 의 Heap Descriptor 을 획득
     */

    sFxTableHandle   = knlGetFxRelationHandle( ELDC_XCOLUMNS_FXTABLE_NO );
    sFxTableHeapDesc = knlGetFxTableHeapDesc( sFxTableHandle );
    
    /**
     * X$COLUMNS의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocFixedValueList( sFxTableHeapDesc,
                                     & sMemMark,
                                     & sValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForTableAccessQuery( aStmt,
                                          sFxTableHandle,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sValueList,
                                          NULL,
                                          NULL,
                                          & sFetchInfo,
                                          NULL,
                                          & sIterator,         
                                          aEnv )
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * Column Descriptor 를 Cache 에 추가한다.
     */

    while ( 1 )
    {
        /**
         * 레코드를 읽는다.
         */
        
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo,
                               aEnv )
                 == STL_SUCCESS );
            
        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }

        /**
         * Cache 정보를 추가
         */
        STL_TRY( eldcInsertCacheFxColumn( aTransID,
                                          aStmt,
                                          sValueList,
                                          aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * SELECT 종료
     */

    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

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
 * @brief Fixed Column 을 위한 Cache 정보를 추가한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aValueList        Data Value List
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcInsertCacheFxColumn( smlTransId          aTransID,
                                   smlStatement      * aStmt,
                                   knlValueBlockList * aValueList,
                                   ellEnv            * aEnv )
{
    ellColumnDesc     * sColumnDesc              = NULL;
    eldcHashElement * sHashElementColumnID   = NULL;
    eldcHashElement * sHashElementColumnName = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Column ID를 위한 Column Descriptor 구성
     */
    
    STL_TRY( eldcMakeFxColumnDesc( & sColumnDesc,
                                   aValueList,
                                   aEnv )
             == STL_SUCCESS );
    
    /**
     * Column ID를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementFxColumnID( & sHashElementColumnID,
                                            aTransID,
                                            sColumnDesc,
                                            aEnv )
             == STL_SUCCESS );
                                                  
    /**
     * ELDC_OBJECTCACHE_COLUMN_ID Hash 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_COLUMN_ID ),
                                    sHashElementColumnID,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Column Name을 위한 Hash Element 구성
     */

    STL_TRY( eldcMakeHashElementFxColumnName( & sHashElementColumnName,
                                              aTransID,
                                              sColumnDesc,
                                              sHashElementColumnID,
                                              aEnv )
             == STL_SUCCESS );
    
    /**
     * ELDC_OBJECTCACHE_COLUMN_NAME 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_COLUMN_NAME ),
                                    sHashElementColumnName,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}








/**
 * @brief 읽은 Value List 로부터 Fixed Column 의 Column Descriptor 를 생성한다.
 * @param[out] aColumnDesc      Column Descriptor
 * @param[in]  aValueList      Value List
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldcMakeFxColumnDesc( ellColumnDesc    ** aColumnDesc,
                                knlValueBlockList * aValueList,
                                ellEnv            * aEnv )
{
    stlInt32           sColumnDescSize = 0;
    ellColumnDesc    * sColumnDesc = NULL;

    stlInt64           sPrecLength = 0;
    dtlDataValue     * sDataValue = NULL;

    stlInt32           sColNameLen = 0;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aColumnDesc != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Column Descriptor 를 위한 공간의 크기 계산
     */

    sDataValue = eldFindDataValue( aValueList,
                                   KNL_ANONYMOUS_TABLE_ID,
                                   KNL_FXCOLUMN_ORDER_COLUMN_NAME );
    sColNameLen = sDataValue->mLength;
    sColumnDescSize = eldcCalSizeColumnDesc( sColNameLen, 0 );

    /**
     * Column Descriptor 를 위한 공간 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sColumnDesc,
                               sColumnDescSize,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Column Descriptor 의 각 정보를 설정한다.
     */

    /*
     * mOwnerID
     */

    sColumnDesc->mOwnerID = ELDT_AUTH_ID_SYSTEM;

    /*
     * mSchemaID
     */

    sColumnDesc->mSchemaID = ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA;

    /*
     * mTableID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   KNL_ANONYMOUS_TABLE_ID,
                                   KNL_FXCOLUMN_ORDER_TABLE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sColumnDesc->mTableID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sColumnDesc->mTableID) == sDataValue->mLength );
    }

    /*
     * mColumnID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   KNL_ANONYMOUS_TABLE_ID,
                                   KNL_FXCOLUMN_ORDER_COLUMN_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sColumnDesc->mColumnID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sColumnDesc->mColumnID) == sDataValue->mLength );
    }

    /*
     * mColumnName
     * 메모리 공간 내에서의 위치 계산
     */

    sColumnDesc->mColumnName = eldcGetColumnNamePtr( sColumnDesc );

    sDataValue = eldFindDataValue( aValueList,
                                   KNL_ANONYMOUS_TABLE_ID,
                                   KNL_FXCOLUMN_ORDER_COLUMN_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sColumnDesc->mColumnName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sColumnDesc->mColumnName[sDataValue->mLength] = '\0';
    }

    /*
     * mOrdinalPosition
     */

    sDataValue = eldFindDataValue( aValueList,
                                   KNL_ANONYMOUS_TABLE_ID,
                                   KNL_FXCOLUMN_ORDER_ORDINAL_POSITION );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sColumnDesc->mOrdinalPosition,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sColumnDesc->mOrdinalPosition) == sDataValue->mLength );

        /* Process 상의 ordinal position 으로 변경 */
        sColumnDesc->mOrdinalPosition
            = ELD_ORDINAL_POSITION_TO_PROCESS(sColumnDesc->mOrdinalPosition);
    }

    /*
     * mDefaultString
     */

    sColumnDesc->mDefaultString = NULL;
    
    /*
     * mNullable
     */

    sDataValue = eldFindDataValue( aValueList,
                                   KNL_ANONYMOUS_TABLE_ID,
                                   KNL_FXCOLUMN_ORDER_IS_NULLABLE );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sColumnDesc->mNullable,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sColumnDesc->mNullable) == sDataValue->mLength );
    }

    /*
     * mUpdatable
     */

    sColumnDesc->mUpdatable = STL_FALSE;

    /*
     * mUnused
     */

    sColumnDesc->mUnused = STL_FALSE;
    
    /*
     * mIdentityHandle
     * mIdentityGenOption
     */

    sColumnDesc->mIdentityHandle    = NULL;
    sColumnDesc->mIdentityGenOption = ELL_IDENTITY_GENERATION_NA;

    /*
     * mDataType
     */

    sDataValue = eldFindDataValue( aValueList,
                                   KNL_ANONYMOUS_TABLE_ID,
                                   KNL_FXCOLUMN_ORDER_DATA_TYPE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sColumnDesc->mDataType,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sColumnDesc->mDataType) == sDataValue->mLength );
    }
    
    /*
     * Data Type 정보
     * Fixed Column 의 Column Type 은 극히 제한적이다.
     * - String 계열의 경우, Character Set 에 관계없이 Char Length 와 Octet Length 는 동일하다.
     * - Timestamp 인 경우, Length 정보(8)가 설정되기 때문에 Fraction Second(2) 를 준다.
     */

    sDataValue = eldFindDataValue( aValueList,
                                   KNL_ANONYMOUS_TABLE_ID,
                                   KNL_FXCOLUMN_ORDER_COLUMN_LENGTH );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sPrecLength,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sPrecLength) == sDataValue->mLength );

        /*
         * Fixed Table 의 대부분의 변수의 길이 정보는 유효하지만,
         * Timestamp 인 경우, 길이 정보(8)가 유효하지 않기 때문에 Fraction Second(2) 를 준다.
         */
        if ( sColumnDesc->mDataType == DTL_TYPE_TIMESTAMP )
        {
            sPrecLength = ELDT_DOMAIN_PRECISION_TIME_STAMP;
        }
    }

    STL_TRY( eldcSetFxColumnType( sColumnDesc,
                                  sPrecLength,
                                  sPrecLength,  /* String 계열의 경우, Octet Length 값 */
                                  0,
                                  aEnv )
             == STL_SUCCESS );
    
    /**
     * return 값 설정
     */
    
    *aColumnDesc = sColumnDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Fixed Column ID 를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aColumnDesc     Column Descriptor
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementFxColumnID( eldcHashElement   ** aHashElement,
                                         smlTransId           aTransID,
                                         ellColumnDesc      * aColumnDesc,
                                         ellEnv             * aEnv )
{
    eldcHashElement   * sHashElement   = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Column ID 를 위한 Hash Element 생성
     */
    
    STL_TRY( eldcMakeHashElementColumnID( & sHashElement,
                                          aTransID,
                                          aColumnDesc,
                                          aEnv )
             == STL_SUCCESS );

    /**
     * 참조) ticket #222
     * Fixed Cache 의 SCN 및 Transaction ID 정보는 Version 관리 대상이 아니다.
     * 
     * Object Cache 가 Transaction Commit 시 변경하는 내용을
     * Fixed Cache 는 Transaction, Statement 와 관계 없이 즉시 변경한다.
     */

    /* Hash Element 의 Version 정보 변경 */
    eldcSetStableVersionInfo( & sHashElement->mVersionInfo );

    /**
     * Output 설정
     */

    *aHashElement = sHashElement;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Column Name 를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aColumnDesc     Column Descriptor
 * @param[in]  aHashElementID  Column ID 의 Hash Element
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementFxColumnName( eldcHashElement   ** aHashElement,
                                           smlTransId           aTransID,
                                           ellColumnDesc      * aColumnDesc,
                                           eldcHashElement    * aHashElementID,
                                           ellEnv             * aEnv )
{
    eldcHashElement   * sHashElement   = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Column Name을 위한 Hash Element 생성
     */
    
    STL_TRY( eldcMakeHashElementColumnName( & sHashElement,
                                            aTransID,
                                            aColumnDesc,
                                            aHashElementID,
                                            aEnv )
             == STL_SUCCESS );

    /**
     * 참조) ticket #222
     * Fixed Cache 의 SCN 및 Transaction ID 정보는 Version 관리 대상이 아니다.
     * 
     * Object Cache 가 Transaction Commit 시 변경하는 내용을
     * Fixed Cache 는 Transaction, Statement 와 관계 없이 즉시 변경한다.
     */

    /* Hash Element 의 Version 정보 변경 */
    eldcSetStableVersionInfo( & sHashElement->mVersionInfo );

    /**
     * Output 설정
     */

    *aHashElement = sHashElement;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Fixed Column Descriptor 의 Column Type 정보를 설정한다.
 * @param[in] aColumnDesc    Fixed Column Descriptor
 * @param[in] aValue1        ellColumnDesc.mColumnType.XXX 의 멤버에 대응하는 값
 * @param[in] aValue2        ellColumnDesc.mColumnType.XXX 의 멤버에 대응하는 값
 * @param[in] aValue3        ellColumnDesc.mColumnType.XXX 의 멤버에 대응하는 값
 * @param[in] aEnv           Environment
 * @remarks
 * DATA TYPE ID 에 따라 aValue1, aValue2, aValue3 는 각각 다르게 해석된다.
 */
stlStatus eldcSetFxColumnType( ellColumnDesc * aColumnDesc,
                               stlInt64        aValue1,
                               stlInt64        aValue2,
                               stlInt64        aValue3,
                               ellEnv        * aEnv )
{
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aColumnDesc != NULL, ELL_ERROR_STACK(aEnv) );
    
    switch (aColumnDesc->mDataType)
    {
        case DTL_TYPE_BOOLEAN:
            {
                // nothing todo
                break;
            }
        case DTL_TYPE_NATIVE_SMALLINT:
            {
                aColumnDesc->mColumnType.mBinaryNum.mBinaryPrec
                    = gDataTypeDefinition[DTL_TYPE_NATIVE_SMALLINT].mDefNumericPrec;
                break;
            }
        case DTL_TYPE_NATIVE_INTEGER:
            {
                aColumnDesc->mColumnType.mBinaryNum.mBinaryPrec
                    = gDataTypeDefinition[DTL_TYPE_NATIVE_INTEGER].mDefNumericPrec;
                break;
            }
        case DTL_TYPE_NATIVE_BIGINT:
            {
                aColumnDesc->mColumnType.mBinaryNum.mBinaryPrec
                    = gDataTypeDefinition[DTL_TYPE_NATIVE_BIGINT].mDefNumericPrec;
                break;
            }
        case DTL_TYPE_NATIVE_REAL:
            {
                aColumnDesc->mColumnType.mBinaryNum.mBinaryPrec
                    = gDataTypeDefinition[DTL_TYPE_NATIVE_REAL].mDefNumericPrec;
                break;
            }
        case DTL_TYPE_NATIVE_DOUBLE:
            {
                aColumnDesc->mColumnType.mBinaryNum.mBinaryPrec
                    = gDataTypeDefinition[DTL_TYPE_NATIVE_DOUBLE].mDefNumericPrec;
                break;
            }
        case DTL_TYPE_FLOAT:
        case DTL_TYPE_NUMBER:
            {
                aColumnDesc->mColumnType.mDecimalNum.mPrec  = aValue1;
                aColumnDesc->mColumnType.mDecimalNum.mScale = aValue2;
                break;
            }
        case DTL_TYPE_DECIMAL:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case DTL_TYPE_CHAR:
            {
                aColumnDesc->mColumnType.mString.mLengthUnit  = DTL_STRING_LENGTH_UNIT_OCTETS;
                aColumnDesc->mColumnType.mString.mCharLength  = aValue1;
                aColumnDesc->mColumnType.mString.mOctetLength = aValue2;
                break;
            }
        case DTL_TYPE_VARCHAR:
        case DTL_TYPE_LONGVARCHAR:
            {
                aColumnDesc->mColumnType.mString.mLengthUnit  = DTL_STRING_LENGTH_UNIT_OCTETS;
                aColumnDesc->mColumnType.mString.mCharLength  = aValue1;
                aColumnDesc->mColumnType.mString.mOctetLength = aValue2;
                break;
            }
        case DTL_TYPE_CLOB:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case DTL_TYPE_BINARY:
            {
                aColumnDesc->mColumnType.mBinary.mByteLength = aValue1;
                break;
            }
        case DTL_TYPE_VARBINARY:
        case DTL_TYPE_LONGVARBINARY:
            {
                aColumnDesc->mColumnType.mBinary.mByteLength = aValue1;
                break;
            }
        case DTL_TYPE_BLOB:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case DTL_TYPE_DATE:
            {
                // nothing to do
                break;
            }
        case DTL_TYPE_TIME:
            {
                aColumnDesc->mColumnType.mDateTime.mFracSecPrec = aValue1;
                break;
            }
        case DTL_TYPE_TIMESTAMP:
            {
                aColumnDesc->mColumnType.mDateTime.mFracSecPrec = aValue1;
                break;
            }
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
            {
                aColumnDesc->mColumnType.mDateTime.mFracSecPrec = aValue1;
                break;
            }
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                aColumnDesc->mColumnType.mDateTime.mFracSecPrec = aValue1;
                break;
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            {
                aColumnDesc->mColumnType.mInterval.mIntervalType = aValue1;
                aColumnDesc->mColumnType.mInterval.mStartPrec    = aValue2;
                aColumnDesc->mColumnType.mInterval.mEndPrec      = aValue3;
                break;
            }
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                aColumnDesc->mColumnType.mInterval.mIntervalType = aValue1;
                aColumnDesc->mColumnType.mInterval.mStartPrec    = aValue2;
                aColumnDesc->mColumnType.mInterval.mEndPrec      = aValue3;
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      ELL_ERROR_STACK(aEnv),
                      "eldcSetFxColumnType()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} eldcFxColumnDesc */
