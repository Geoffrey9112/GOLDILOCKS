/*******************************************************************************
 * knlAggregation.c
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
 * @file knlAggregation.c
 * @brief Kernel Layer Aggregation Routines
 */

#include <knl.h>

/**
 * @addtogroup knlAggregation
 * @{
 */


/************************************************
 * Function Info
 ***********************************************/

knlAddValueToNumFuncPtr gKnlAddValueToNumFuncPtrArr[ DTL_TYPE_MAX ] =
{
    NULL,                           /**< BOOLEAN */
    knlAddValueSmallIntToBigInt,    /**< NATIVE_SMALLINT */
    knlAddValueIntegerToBigInt,     /**< NATIVE_INTEGER */
    knlAddValueBigIntToBigInt,      /**< NATIVE_BIGINT */

    knlAddValueRealToDouble,        /**< NATIVE_REAL */
    knlAddValueDoubleToDouble,      /**< NATIVE_DOUBLE */
    
    knlAddValueNumericToNumeric,    /**< FLOAT */
    knlAddValueNumericToNumeric,    /**< NUMBER */
    NULL,                           /**< unsupported feature, DECIMAL */
    
    knlAddValueStringToNumeric,     /**< CHARACTER */
    knlAddValueStringToNumeric,     /**< CHARACTER VARYING */
    knlAddValueStringToNumeric,     /**< CHARACTER LONG VARYING */
    NULL,                           /**< unsupported feature, CHARACTER LARGE OBJECT */
    
    NULL,                           /**< BINARY */
    NULL,                           /**< BINARY VARYING */
    NULL,                           /**< BINARY LONG VARYING */
    NULL,                           /**< unsupported feature, BINARY LARGE OBJECT */

    NULL,                           /**< DATE */
    NULL,                           /**< TIME WITHOUT TIME ZONE */
    NULL,                           /**< TIMESTAMP WITHOUT TIME ZONE */
    NULL,                           /**< TIME WITH TIME ZONE */
    NULL,                           /**< TIMESTAMP WITH TIME ZONE */

    knlAddValueYearToMonth,         /**< INTERVAL YEAR TO MONTH */
    knlAddValueDayToSecond,         /**< INTERVAL DAY TO SECOND */

    NULL                            /**< ROWID */
};


knlAddToNumFuncPtr gKnlAddToNumFuncPtrArr[ DTL_TYPE_MAX ] =
{
    NULL,                      /**< BOOLEAN */
    knlAddSmallIntToBigInt,    /**< NATIVE_SMALLINT */
    knlAddIntegerToBigInt,     /**< NATIVE_INTEGER */
    knlAddBigIntToBigInt,      /**< NATIVE_BIGINT */

    knlAddRealToDouble,        /**< NATIVE_REAL */
    knlAddDoubleToDouble,      /**< NATIVE_DOUBLE */

    knlAddNumericToNumeric,    /**< FLOAT */    
    knlAddNumericToNumeric,    /**< NUMBER */
    NULL,                      /**< unsupported feature, DECIMAL */
    
    knlAddStringToNumeric,     /**< CHARACTER */
    knlAddStringToNumeric,     /**< CHARACTER VARYING */
    knlAddStringToNumeric,     /**< CHARACTER LONG VARYING */
    NULL,                      /**< unsupported feature, CHARACTER LARGE OBJECT */
    
    NULL,                      /**< BINARY */
    NULL,                      /**< BINARY VARYING */
    NULL,                      /**< BINARY LONG VARYING */
    NULL,                      /**< unsupported feature, BINARY LARGE OBJECT */

    NULL,                      /**< DATE */
    NULL,                      /**< TIME WITHOUT TIME ZONE */
    NULL,                      /**< TIMESTAMP WITHOUT TIME ZONE */
    NULL,                      /**< TIME WITH TIME ZONE */
    NULL,                      /**< TIMESTAMP WITH TIME ZONE */

    knlAddYearToMonth,         /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
    knlAddDayToSecond,         /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

    NULL                       /**< DTL_TYPE_ROWID */
};


/************************************************
 * Aggregation Function By Data Value
 ***********************************************/

/**
 * @brief SmallInt 와 BigInt 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 BigInt Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddValueSmallIntToBigInt( dtlDataValue  * aSrcValue,
                                       dtlDataValue  * aDestValue,
                                       knlEnv        * aEnv )
{
    dtlBigIntType   * sResultVal;
    stlInt64          sInput;
    stlInt64          sSum;
    
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aSrcValue->mType == DTL_TYPE_NATIVE_SMALLINT,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue->mType == DTL_TYPE_NATIVE_BIGINT,
                        KNL_ERROR_STACK( aEnv ) );


    /**
     * ADD 연산 수행
     */

    sResultVal = (dtlBigIntType *) aDestValue->mValue;

    sInput = *(dtlSmallIntType*) aSrcValue->mValue;

    STL_DASSERT( DTL_IS_NULL( aSrcValue ) == STL_FALSE );
    
    if( DTL_IS_NULL( aDestValue ) )
    {
        *sResultVal = sInput;
        aDestValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    }
    else
    {
        sSum = *sResultVal + sInput;

        STL_TRY_THROW( ( KNL_DIFFSIGN( sInput, *sResultVal ) ) ||
                       ( KNL_SAMESIGN( sSum, sInput ) ),
                       ERROR_OUT_OF_RANGE );
            
        *sResultVal = sSum;
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Integer 와 BigInt 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 BigInt Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddValueIntegerToBigInt( dtlDataValue  * aSrcValue,
                                      dtlDataValue  * aDestValue,
                                      knlEnv        * aEnv )
{
    dtlBigIntType   * sResultVal;
    stlInt64          sInput;
    stlInt64          sSum;
    
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aSrcValue->mType == DTL_TYPE_NATIVE_INTEGER,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue->mType == DTL_TYPE_NATIVE_BIGINT,
                        KNL_ERROR_STACK( aEnv ) );


    /**
     * ADD 연산 수행
     */

    sResultVal = (dtlBigIntType *) aDestValue->mValue;

    STL_DASSERT( DTL_IS_NULL( aSrcValue ) == STL_FALSE );

    sInput = *(dtlIntegerType*) aSrcValue->mValue;
                
    if( DTL_IS_NULL( aDestValue ) )
    {
        *sResultVal = sInput;
        aDestValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    }
    else
    {
        sSum = *sResultVal + sInput;

        STL_TRY_THROW( ( KNL_DIFFSIGN( sInput, *sResultVal ) ) ||
                       ( KNL_SAMESIGN( sSum, sInput ) ),
                       ERROR_OUT_OF_RANGE );
            
        *sResultVal = sSum;
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief BigInt 와 BigInt 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 BigInt Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddValueBigIntToBigInt( dtlDataValue  * aSrcValue,
                                     dtlDataValue  * aDestValue,
                                     knlEnv        * aEnv )
{
    dtlBigIntType   * sResultVal;
    stlInt64          sInput;
    stlInt64          sSum;
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aSrcValue->mType == DTL_TYPE_NATIVE_BIGINT,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue->mType == DTL_TYPE_NATIVE_BIGINT,
                        KNL_ERROR_STACK( aEnv ) );


    /**
     * ADD 연산 수행
     */

    sResultVal = (dtlBigIntType *) aDestValue->mValue;

    STL_DASSERT( DTL_IS_NULL( aSrcValue ) == STL_FALSE );

    sInput = *(dtlBigIntType*) aSrcValue->mValue;
                
    if( DTL_IS_NULL( aDestValue ) )
    {
        *sResultVal = sInput;
        aDestValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    }
    else
    {
        sSum = *sResultVal + sInput;

        STL_TRY_THROW( ( KNL_DIFFSIGN( sInput, *sResultVal ) ) ||
                       ( KNL_SAMESIGN( sSum, sInput ) ),
                       ERROR_OUT_OF_RANGE );
            
        *sResultVal = sSum;
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Real 과 BigInt 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 BigInt Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddValueRealToDouble( dtlDataValue  * aSrcValue,
                                   dtlDataValue  * aDestValue,
                                   knlEnv        * aEnv )
{
    dtlDoubleType   * sResultVal;
    stlFloat64        sInput;
    stlFloat64        sSum;
    
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aSrcValue->mType == DTL_TYPE_NATIVE_REAL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue->mType == DTL_TYPE_NATIVE_DOUBLE,
                        KNL_ERROR_STACK( aEnv ) );


    /**
     * ADD 연산 수행
     */

    sResultVal = (dtlDoubleType *) aDestValue->mValue;

    STL_DASSERT( DTL_IS_NULL( aSrcValue ) == STL_FALSE );

    sInput = *(dtlRealType*) aSrcValue->mValue;

    STL_TRY_THROW( stlIsinfinite( sInput ) == STL_IS_INFINITE_FALSE,
                   ERROR_OUT_OF_RANGE );
                
    if( DTL_IS_NULL( aDestValue ) )
    {
        *sResultVal = sInput;
        aDestValue->mLength = DTL_NATIVE_DOUBLE_SIZE;
    }
    else
    {
        sSum = *sResultVal + sInput;
        STL_TRY_THROW( stlIsinfinite( sSum ) == STL_IS_INFINITE_FALSE,
                       ERROR_OUT_OF_RANGE );
        *sResultVal = sSum;
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Double 과 Double 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 Double Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddValueDoubleToDouble( dtlDataValue  * aSrcValue,
                                     dtlDataValue  * aDestValue,
                                     knlEnv        * aEnv )
{
    dtlDoubleType   * sResultVal;
    stlFloat64        sInput;
    stlFloat64        sSum;
    
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aSrcValue->mType == DTL_TYPE_NATIVE_DOUBLE,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue->mType == DTL_TYPE_NATIVE_DOUBLE,
                        KNL_ERROR_STACK( aEnv ) );


    /**
     * ADD 연산 수행
     */

    sResultVal = (dtlDoubleType *) aDestValue->mValue;

    STL_DASSERT( DTL_IS_NULL( aSrcValue ) == STL_FALSE );

    sInput = *(dtlDoubleType*) aSrcValue->mValue;
                
    if( DTL_IS_NULL( aDestValue ) )
    {
        *sResultVal = sInput;
        aDestValue->mLength = DTL_NATIVE_DOUBLE_SIZE;
    }
    else
    {
        sSum = *sResultVal + sInput;
        STL_TRY_THROW( stlIsinfinite( sSum ) == STL_IS_INFINITE_FALSE,
                       ERROR_OUT_OF_RANGE );
        *sResultVal = sSum;
    }


    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Numeric 과 Numeric 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 Numeric Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddValueNumericToNumeric( dtlDataValue  * aSrcValue,
                                       dtlDataValue  * aDestValue,
                                       knlEnv        * aEnv )
{
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( ( aSrcValue->mType == DTL_TYPE_FLOAT ) || ( aSrcValue->mType == DTL_TYPE_NUMBER ),
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue->mType == DTL_TYPE_NUMBER,
                        KNL_ERROR_STACK( aEnv ) );


    /**
     * ADD 연산 수행
     */

    STL_DASSERT( DTL_IS_NULL( aSrcValue ) == STL_FALSE );

    if( DTL_IS_NULL( aDestValue ) )
    {
        /* copy */
        DTL_COPY_DATA_VALUE( aDestValue->mBufferSize, aSrcValue, aDestValue );
        aDestValue->mType = DTL_TYPE_NUMBER;
    }
    else
    {
        /* add */
        STL_TRY( dtlNumericSum( aDestValue,
                                aSrcValue,
                                KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief String 과 Numeric 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 Numeric Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddValueStringToNumeric( dtlDataValue  * aSrcValue,
                                      dtlDataValue  * aDestValue,
                                      knlEnv        * aEnv )
{
    dtlDataValue      sNumValue;
    stlChar           sNumBuffer[DTL_NUMERIC_MAX_SIZE];

    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( ( aSrcValue->mType == DTL_TYPE_CHAR ) ||
                        ( aSrcValue->mType == DTL_TYPE_VARCHAR ) ||
                        ( aSrcValue->mType == DTL_TYPE_LONGVARCHAR ),
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue->mType == DTL_TYPE_NUMBER,
                        KNL_ERROR_STACK( aEnv ) );


    /**
     * ADD 연산 수행
     */

    STL_DASSERT( DTL_IS_NULL( aSrcValue ) == STL_FALSE );

    sNumValue.mType   = DTL_TYPE_NUMBER;
    sNumValue.mLength = 0;
    sNumValue.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumValue.mValue  = (dtlNumericType*)&sNumBuffer;

    STL_TRY( dtlGetNumericFromString( (stlChar *) aSrcValue->mValue,
                                      aSrcValue->mLength,
                                      DTL_NUMERIC_MAX_PRECISION,
                                      DTL_SCALE_NA,
                                      & sNumValue,
                                      KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    if( DTL_IS_NULL( aDestValue ) )
    {
        /* copy */
        DTL_COPY_DATA_VALUE( aDestValue->mBufferSize, & sNumValue, aDestValue );
        aDestValue->mType = DTL_TYPE_NUMBER;
    }
    else
    {
        /* add */
        STL_TRY( dtlNumericSum( aDestValue,
                                & sNumValue,
                                KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Interval YEAR_TO_MONTH 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 Interval YEAR_TO_MONTH Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddValueYearToMonth( dtlDataValue  * aSrcValue,
                                  dtlDataValue  * aDestValue,
                                  knlEnv        * aEnv )
{
    dtlIntervalYearToMonthType   * sResultVal;
    stlInt64                       sInput;
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aSrcValue->mType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue->mType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH,
                        KNL_ERROR_STACK( aEnv ) );

    
    /**
     * ADD 연산 수행
     */

    sResultVal = (dtlIntervalYearToMonthType *) aDestValue->mValue;

    STL_DASSERT( DTL_IS_NULL( aSrcValue ) == STL_FALSE );

    sInput = ((dtlIntervalYearToMonthType *) aSrcValue->mValue)->mMonth;

    if( DTL_IS_NULL( aDestValue ) )
    {
        STL_TRY_THROW( ( sInput > KNL_INTERVAL_MIN_MONTH ) &&
                       ( sInput < KNL_INTERVAL_MAX_MONTH ),
                       ERROR_OUT_OF_RANGE );

        sResultVal->mMonth     = sInput;
        sResultVal->mIndicator = DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH;
        aDestValue->mLength    = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
    }
    else
    {
        sResultVal->mMonth += sInput;

        STL_TRY_THROW( ( sResultVal->mMonth > KNL_INTERVAL_MIN_MONTH ) &&
                       ( sResultVal->mMonth < KNL_INTERVAL_MAX_MONTH ),
                       ERROR_OUT_OF_RANGE );
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_C_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Interval DAY_TO_SECOND 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 Interval DAY_TO_SECOND Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddValueDayToSecond( dtlDataValue  * aSrcValue,
                                  dtlDataValue  * aDestValue,
                                  knlEnv        * aEnv )
{
    dtlIntervalDayToSecondType   * sResultVal;
    stlInt64                       sInput;
    
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aSrcValue->mType == DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue->mType == DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                        KNL_ERROR_STACK( aEnv ) );

    
    /**
     * ADD 연산 수행
     */

    sResultVal = (dtlIntervalDayToSecondType *) aDestValue->mValue;

    STL_DASSERT( DTL_IS_NULL( aSrcValue ) == STL_FALSE );

    sInput = ( ((dtlIntervalDayToSecondType *) aSrcValue->mValue)->mDay * DTL_USECS_PER_DAY +
               ((dtlIntervalDayToSecondType *) aSrcValue->mValue)->mTime );

    if( DTL_IS_NULL( aDestValue ) )
    {
        sResultVal->mDay       = sInput / DTL_USECS_PER_DAY;
        sResultVal->mTime      = sInput % DTL_USECS_PER_DAY;
        sResultVal->mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
                
        STL_TRY_THROW( ( sResultVal->mDay > KNL_INTERVAL_MIN_DAY ) &&
                       ( sResultVal->mDay < KNL_INTERVAL_MAX_DAY ),
                       ERROR_OUT_OF_RANGE );

        aDestValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    }
    else
    {
        sInput += ( sResultVal->mDay * DTL_USECS_PER_DAY +
                    sResultVal->mTime );
                
        sResultVal->mDay  = sInput / DTL_USECS_PER_DAY;
        sResultVal->mTime = sInput % DTL_USECS_PER_DAY;

        STL_TRY_THROW( ( sResultVal->mDay > KNL_INTERVAL_MIN_DAY ) &&
                       ( sResultVal->mDay < KNL_INTERVAL_MAX_DAY ),
                       ERROR_OUT_OF_RANGE );
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_C_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/************************************************
 * Aggregation Function
 ***********************************************/

/**
 * @brief SmallInt 와 BigInt 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Block Count
 * @param[in]      aResultBlockIdx 결과 Value Block Idx
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 BigInt Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddSmallIntToBigInt( knlValueBlockList  * aSrcValue,
                                  knlValueBlockList  * aDestValue,
                                  stlInt32             aStartBlockIdx,
                                  stlInt32             aBlockCnt,
                                  stlInt32             aResultBlockIdx,
                                  knlEnv             * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlBigIntType   * sResultVal;

    stlInt64          sInput;
    stlInt64          sSum;
    stlInt32          sLoop;
    stlBool           sHasValue;
    
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStartBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResultBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );


    /**
     * ADD 연산 수행
     */

    sValue1    = KNL_GET_BLOCK_DATA_VALUE( aSrcValue, aStartBlockIdx );
    sValue2    = KNL_GET_BLOCK_DATA_VALUE( aDestValue, aResultBlockIdx );
    sResultVal = (dtlBigIntType *) sValue2->mValue;

    STL_PARAM_VALIDATE( sValue1->mType == DTL_TYPE_NATIVE_SMALLINT,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( sValue2->mType == DTL_TYPE_NATIVE_BIGINT,
                        KNL_ERROR_STACK( aEnv ) );

    if( KNL_GET_BLOCK_IS_SEP_BUFF( aSrcValue ) == STL_FALSE )
    {
        /**
         * Used Block Count 만큼 반복하여 ADD 연산 수행
         */

        if( DTL_IS_NULL( sValue1 ) )
        {
            /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            /* Do Nothing */
        }
        else
        {
            sInput = *(dtlSmallIntType*) sValue1->mValue;
            sInput *= aBlockCnt;

            STL_TRY_THROW( ( sInput / aBlockCnt == *(dtlSmallIntType*) sValue1->mValue ),
                           ERROR_OUT_OF_RANGE );
                
            if( DTL_IS_NULL( sValue2 ) )
            {
                *sResultVal = sInput;
                sValue2->mLength = DTL_NATIVE_BIGINT_SIZE;
            }
            else
            {
                sSum = *sResultVal + sInput;

                STL_TRY_THROW( ( KNL_DIFFSIGN( sInput, *sResultVal ) ) ||
                               ( KNL_SAMESIGN( sSum, sInput ) ),
                               ERROR_OUT_OF_RANGE );
            
                *sResultVal = sSum;
            }
        }
    }
    else
    {
        /**
         * Src Value의 Used Block Count 만큼 반복하여 ADD 연산 수행
         */

        sHasValue = STL_FALSE;
        sSum = 0;
        for( sLoop = 0 ; sLoop < aBlockCnt ; sLoop++ )
        {
            if( DTL_IS_NULL( sValue1 ) )
            {
                /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            }
            else
            {
                sInput = *(dtlSmallIntType*) sValue1->mValue;
                
                STL_TRY_THROW( ( KNL_DIFFSIGN( sInput, sSum ) ) ||
                               ( KNL_SAMESIGN( sInput, ( sInput + sSum ) ) ),
                               ERROR_OUT_OF_RANGE );

                sSum += sInput;
                sHasValue = STL_TRUE;
            }
            sValue1++;
        }

        if( sHasValue == STL_TRUE )
        {
            if( DTL_IS_NULL( sValue2 ) )
            {
                *sResultVal = sSum;
                sValue2->mLength = DTL_NATIVE_BIGINT_SIZE;
            }
            else
            {
                sInput = sSum;
                sSum   = *sResultVal + sInput;

                STL_TRY_THROW( ( KNL_DIFFSIGN( sInput, *sResultVal ) ) ||
                               ( KNL_SAMESIGN( sSum, sInput ) ),
                               ERROR_OUT_OF_RANGE );
            
                *sResultVal = sSum;
            }
        }
        else
        {
            /* Do Nothing */
        }
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Integer 와 BigInt 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Block Count
 * @param[in]      aResultBlockIdx 결과 Value Block Idx
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 BigInt Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddIntegerToBigInt( knlValueBlockList  * aSrcValue,
                                 knlValueBlockList  * aDestValue,
                                 stlInt32             aStartBlockIdx,
                                 stlInt32             aBlockCnt,
                                 stlInt32             aResultBlockIdx,
                                 knlEnv             * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlBigIntType   * sResultVal;

    stlInt64          sInput;
    stlInt64          sSum;
    stlInt32          sLoop;
    stlBool           sHasValue;
    
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStartBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResultBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );


    /**
     * ADD 연산 수행
     */

    sValue1    = KNL_GET_BLOCK_DATA_VALUE( aSrcValue, aStartBlockIdx );
    sValue2    = KNL_GET_BLOCK_DATA_VALUE( aDestValue, aResultBlockIdx );
    sResultVal = (dtlBigIntType *) sValue2->mValue;

    STL_PARAM_VALIDATE( sValue1->mType == DTL_TYPE_NATIVE_INTEGER,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( sValue2->mType == DTL_TYPE_NATIVE_BIGINT,
                        KNL_ERROR_STACK( aEnv ) );

    if( KNL_GET_BLOCK_IS_SEP_BUFF( aSrcValue ) == STL_FALSE )
    {
        /**
         * Used Block Count 만큼 반복하여 ADD 연산 수행
         */

        if( DTL_IS_NULL( sValue1 ) )
        {
            /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            /* Do Nothing */
        }
        else
        {
            sInput = *(dtlIntegerType*) sValue1->mValue;
            sInput *= aBlockCnt;

            STL_TRY_THROW( ( sInput / aBlockCnt == *(dtlIntegerType*) sValue1->mValue ),
                           ERROR_OUT_OF_RANGE );
                
            if( DTL_IS_NULL( sValue2 ) )
            {
                *sResultVal = sInput;
                sValue2->mLength = DTL_NATIVE_BIGINT_SIZE;
            }
            else
            {
                sSum = *sResultVal + sInput;

                STL_TRY_THROW( ( KNL_DIFFSIGN( sInput, *sResultVal ) ) ||
                               ( KNL_SAMESIGN( sSum, sInput ) ),
                               ERROR_OUT_OF_RANGE );
            
                *sResultVal = sSum;
            }
        }
    }
    else
    {
        /**
         * Src Value의 Used Block Count 만큼 반복하여 ADD 연산 수행
         */

        sHasValue = STL_FALSE;
        sSum = 0;
        for( sLoop = 0 ; sLoop < aBlockCnt ; sLoop++ )
        {
            if( DTL_IS_NULL( sValue1 ) )
            {
                /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            }
            else
            {
                sInput = *(dtlIntegerType*) sValue1->mValue;
                
                STL_TRY_THROW( ( KNL_DIFFSIGN( sInput, sSum ) ) ||
                               ( KNL_SAMESIGN( sInput, ( sInput + sSum ) ) ),
                               ERROR_OUT_OF_RANGE );

                sSum += sInput;
                sHasValue = STL_TRUE;
            }
            sValue1++;
        }

        if( sHasValue == STL_TRUE )
        {
            if( DTL_IS_NULL( sValue2 ) )
            {
                *sResultVal = sSum;
                sValue2->mLength = DTL_NATIVE_BIGINT_SIZE;
            }
            else
            {
                sInput = sSum;
                sSum   = *sResultVal + sInput;

                STL_TRY_THROW( ( KNL_DIFFSIGN( sInput, *sResultVal ) ) ||
                               ( KNL_SAMESIGN( sSum, sInput ) ),
                               ERROR_OUT_OF_RANGE );
            
                *sResultVal = sSum;
            }
        }
        else
        {
            /* Do Nothing */
        }
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief BigInt 와 BigInt 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Block Count
 * @param[in]      aResultBlockIdx 결과 Value Block Idx
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 BigInt Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddBigIntToBigInt( knlValueBlockList  * aSrcValue,
                                knlValueBlockList  * aDestValue,
                                stlInt32             aStartBlockIdx,
                                stlInt32             aBlockCnt,
                                stlInt32             aResultBlockIdx,
                                knlEnv             * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlBigIntType   * sResultVal;

    stlInt64          sInput;
    stlInt64          sSum;
    stlInt32          sLoop;
    stlBool           sHasValue;
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStartBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResultBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );


    /**
     * ADD 연산 수행
     */

    sValue1    = KNL_GET_BLOCK_DATA_VALUE( aSrcValue, aStartBlockIdx );
    sValue2    = KNL_GET_BLOCK_DATA_VALUE( aDestValue, aResultBlockIdx );
    sResultVal = (dtlBigIntType *) sValue2->mValue;

    STL_PARAM_VALIDATE( sValue1->mType == DTL_TYPE_NATIVE_BIGINT,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( sValue2->mType == DTL_TYPE_NATIVE_BIGINT,
                        KNL_ERROR_STACK( aEnv ) );

    if( KNL_GET_BLOCK_IS_SEP_BUFF( aSrcValue ) == STL_FALSE )
    {
        /**
         * Used Block Count 만큼 반복하여 ADD 연산 수행
         */

        if( DTL_IS_NULL( sValue1 ) )
        {
            /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            /* Do Nothing */
        }
        else
        {
            sInput = *(dtlBigIntType*) sValue1->mValue;
            sInput *= aBlockCnt;

            STL_TRY_THROW( ( sInput / aBlockCnt == *(dtlBigIntType*) sValue1->mValue ),
                           ERROR_OUT_OF_RANGE );
                
            if( DTL_IS_NULL( sValue2 ) )
            {
                *sResultVal = sInput;
                sValue2->mLength = DTL_NATIVE_BIGINT_SIZE;
            }
            else
            {
                sSum = *sResultVal + sInput;

                STL_TRY_THROW( ( KNL_DIFFSIGN( sInput, *sResultVal ) ) ||
                               ( KNL_SAMESIGN( sSum, sInput ) ),
                               ERROR_OUT_OF_RANGE );
            
                *sResultVal = sSum;
            }
        }
    }
    else
    {
        /**
         * Src Value의 Used Block Count 만큼 반복하여 ADD 연산 수행
         */
        
        sHasValue = STL_FALSE;
        sSum = 0;
        for( sLoop = 0 ; sLoop < aBlockCnt ; sLoop++ )
        {
            if( DTL_IS_NULL( sValue1 ) )
            {
                /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            }
            else
            {
                sInput = *(dtlBigIntType*) sValue1->mValue;
                
                STL_TRY_THROW( ( KNL_DIFFSIGN( sInput, sSum ) ) ||
                               ( KNL_SAMESIGN( sInput, ( sInput + sSum ) ) ),
                               ERROR_OUT_OF_RANGE );

                sSum += sInput;
                sHasValue = STL_TRUE;
            }
            sValue1++;
        }

        if( sHasValue == STL_TRUE )
        {
            if( DTL_IS_NULL( sValue2 ) )
            {
                *sResultVal = sSum;
                sValue2->mLength = DTL_NATIVE_BIGINT_SIZE;
            }
            else
            {
                sInput = sSum;
                sSum   = *sResultVal + sInput;
            
                STL_TRY_THROW( ( KNL_DIFFSIGN( sInput, *sResultVal ) ) ||
                               ( KNL_SAMESIGN( sSum, sInput ) ),
                               ERROR_OUT_OF_RANGE );
            
                *sResultVal = sSum;
            }
        }
        else
        {
            /* Do Nothing */
        }
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Real 과 BigInt 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Block Count
 * @param[in]      aResultBlockIdx 결과 Value Block Idx
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 BigInt Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddRealToDouble( knlValueBlockList  * aSrcValue,
                              knlValueBlockList  * aDestValue,
                              stlInt32             aStartBlockIdx,
                              stlInt32             aBlockCnt,
                              stlInt32             aResultBlockIdx,
                              knlEnv             * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDoubleType   * sResultVal;

    stlFloat64        sInput;
    stlFloat64        sSum;
    stlInt32          sLoop;
    stlBool           sHasValue;
    
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStartBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResultBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );


    /**
     * ADD 연산 수행
     */

    sValue1    = KNL_GET_BLOCK_DATA_VALUE( aSrcValue, aStartBlockIdx );
    sValue2    = KNL_GET_BLOCK_DATA_VALUE( aDestValue, aResultBlockIdx );
    sResultVal = (dtlDoubleType *) sValue2->mValue;

    STL_PARAM_VALIDATE( sValue1->mType == DTL_TYPE_NATIVE_REAL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( sValue2->mType == DTL_TYPE_NATIVE_DOUBLE,
                        KNL_ERROR_STACK( aEnv ) );

    if( KNL_GET_BLOCK_IS_SEP_BUFF( aSrcValue ) == STL_FALSE )
    {
        /**
         * Used Block Count 만큼 반복하여 ADD 연산 수행
         */

        if( DTL_IS_NULL( sValue1 ) )
        {
            /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            /* Do Nothing */
        }
        else
        {
            sInput = *(dtlRealType*) sValue1->mValue;
            sInput *= aBlockCnt;

            STL_TRY_THROW( stlIsinfinite( sInput ) == STL_IS_INFINITE_FALSE,
                           ERROR_OUT_OF_RANGE );
                
            if( DTL_IS_NULL( sValue2 ) )
            {
                *sResultVal = sInput;
                sValue2->mLength = DTL_NATIVE_DOUBLE_SIZE;
            }
            else
            {
                sSum = *sResultVal + sInput;
                STL_TRY_THROW( stlIsinfinite( sSum ) == STL_IS_INFINITE_FALSE,
                               ERROR_OUT_OF_RANGE );
                *sResultVal = sSum;
            }
        }
    }
    else
    {
        /**
         * Src Value의 Used Block Count 만큼 반복하여 ADD 연산 수행
         */
        
        sHasValue = STL_FALSE;
        sSum = 0;
        for( sLoop = 0 ; sLoop < aBlockCnt ; sLoop++ )
        {
            if( DTL_IS_NULL( sValue1 ) )
            {
                /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            }
            else
            {
                sSum += *(dtlRealType*) sValue1->mValue;
                
                STL_TRY_THROW( stlIsinfinite( sSum ) == STL_IS_INFINITE_FALSE,
                               ERROR_OUT_OF_RANGE );

                sHasValue = STL_TRUE;
            }
            sValue1++;
        }

        if( sHasValue == STL_TRUE )
        {
            if( DTL_IS_NULL( sValue2 ) )
            {
                *sResultVal = sSum;
                sValue2->mLength = DTL_NATIVE_DOUBLE_SIZE;
            }
            else
            {
                sSum += *sResultVal;
            
                STL_TRY_THROW( stlIsinfinite( sSum ) == STL_IS_INFINITE_FALSE,
                               ERROR_OUT_OF_RANGE );
            
                *sResultVal = sSum;
            }
        }
        else
        {
            /* Do Nothing */
        }
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Double 과 Double 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Block Count
 * @param[in]      aResultBlockIdx 결과 Value Block Idx
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 Double Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddDoubleToDouble( knlValueBlockList  * aSrcValue,
                                knlValueBlockList  * aDestValue,
                                stlInt32             aStartBlockIdx,
                                stlInt32             aBlockCnt,
                                stlInt32             aResultBlockIdx,
                                knlEnv             * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDoubleType   * sResultVal;

    stlFloat64        sInput;
    stlFloat64        sSum;
    stlInt32          sLoop;
    stlBool           sHasValue;
    
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStartBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResultBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );


    /**
     * ADD 연산 수행
     */

    sValue1    = KNL_GET_BLOCK_DATA_VALUE( aSrcValue, aStartBlockIdx );
    sValue2    = KNL_GET_BLOCK_DATA_VALUE( aDestValue, aResultBlockIdx );
    sResultVal = (dtlDoubleType *) sValue2->mValue;

    STL_PARAM_VALIDATE( sValue1->mType == DTL_TYPE_NATIVE_DOUBLE,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( sValue2->mType == DTL_TYPE_NATIVE_DOUBLE,
                        KNL_ERROR_STACK( aEnv ) );

    if( KNL_GET_BLOCK_IS_SEP_BUFF( aSrcValue ) == STL_FALSE )
    {
        /**
         * Used Block Count 만큼 반복하여 ADD 연산 수행
         */

        if( DTL_IS_NULL( sValue1 ) )
        {
            /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            /* Do Nothing */
        }
        else
        {
            sInput = *(dtlDoubleType*) sValue1->mValue;
            sInput *= aBlockCnt;

            STL_TRY_THROW( stlIsinfinite( sInput ) == STL_IS_INFINITE_FALSE,
                           ERROR_OUT_OF_RANGE );
                
            if( DTL_IS_NULL( sValue2 ) )
            {
                *sResultVal = sInput;
                sValue2->mLength = DTL_NATIVE_DOUBLE_SIZE;
            }
            else
            {
                sSum = *sResultVal + sInput;
                STL_TRY_THROW( stlIsinfinite( sSum ) == STL_IS_INFINITE_FALSE,
                               ERROR_OUT_OF_RANGE );
                *sResultVal = sSum;
            }
        }
    }
    else
    {
        /**
         * Src Value의 Used Block Count 만큼 반복하여 ADD 연산 수행
         */
        
        sHasValue = STL_FALSE;
        sSum = 0;
        for( sLoop = 0 ; sLoop < aBlockCnt ; sLoop++ )
        {
            if( DTL_IS_NULL( sValue1 ) )
            {
                /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            }
            else
            {
                sSum += *(dtlDoubleType*) sValue1->mValue;
                
                STL_TRY_THROW( stlIsinfinite( sSum ) == STL_IS_INFINITE_FALSE,
                               ERROR_OUT_OF_RANGE );

                sHasValue = STL_TRUE;
            }
            sValue1++;
        }

        if( sHasValue == STL_TRUE )
        {
            if( DTL_IS_NULL( sValue2 ) )
            {
                *sResultVal = sSum;
                sValue2->mLength = DTL_NATIVE_DOUBLE_SIZE;
            }
            else
            {
                sSum += *sResultVal;
            
                STL_TRY_THROW( stlIsinfinite( sSum ) == STL_IS_INFINITE_FALSE,
                               ERROR_OUT_OF_RANGE );
            
                *sResultVal = sSum;
            }
        }
        else
        {
            /* Do Nothing */
        }
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Numeric 과 Numeric 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Block Count
 * @param[in]      aResultBlockIdx 결과 Value Block Idx
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 Numeric Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddNumericToNumeric( knlValueBlockList  * aSrcValue,
                                  knlValueBlockList  * aDestValue,
                                  stlInt32             aStartBlockIdx,
                                  stlInt32             aBlockCnt,
                                  stlInt32             aResultBlockIdx,
                                  knlEnv             * aEnv )
{
    stlInt32          sLoop;

    stlBool           sIsSepSrc;
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;

    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStartBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResultBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );

    
    /**
     * ADD 연산 수행
     */

    sIsSepSrc  = KNL_GET_BLOCK_IS_SEP_BUFF( aSrcValue );
    sValue1    = KNL_GET_BLOCK_DATA_VALUE( aSrcValue, aStartBlockIdx );
    sValue2    = KNL_GET_BLOCK_DATA_VALUE( aDestValue, aResultBlockIdx );

    STL_PARAM_VALIDATE( ( sValue1->mType == DTL_TYPE_FLOAT ) ||
                        ( sValue1->mType == DTL_TYPE_NUMBER ) ,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( ( sValue2->mType == DTL_TYPE_FLOAT ) ||
                        ( sValue2->mType == DTL_TYPE_NUMBER ) ,
                        KNL_ERROR_STACK( aEnv ) );

    if( sIsSepSrc == STL_FALSE )
    {
        /**
         * Used Block Count 만큼 반복하여 ADD 연산 수행
         */

        if( DTL_IS_NULL( sValue1 ) )
        {
            /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            /* Do Nothing */
        }
        else
        {
            stlChar           sTmpCountBuffer[DTL_NUMERIC_MAX_SIZE];
            dtlDataValue      sCount;
            
            sCount.mType   = DTL_TYPE_NUMBER;
            sCount.mLength = 0;
            sCount.mBufferSize = DTL_NUMERIC_MAX_SIZE;
            sCount.mValue  = (dtlNumericType*)&sTmpCountBuffer;

            /* Integer로 부터 dtfNumeric 얻기 */
            STL_TRY( dtlInt64ToNumeric( aBlockCnt,
                                        & sCount,
                                        KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            if( DTL_IS_NULL( sValue2 ) )
            {
                /* mul */
                STL_TRY( dtlNumericMultiplication( sValue1,
                                                   & sCount,
                                                   sValue2,
                                                   KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
            }
            else
            {
                stlChar           sTmpNumericBuffer1[DTL_NUMERIC_MAX_SIZE];
                dtlDataValue      sNumeric1;

                sNumeric1.mType   = DTL_TYPE_NUMBER;
                sNumeric1.mLength = 0;
                sNumeric1.mBufferSize = DTL_NUMERIC_MAX_SIZE;
                sNumeric1.mValue  = (dtlNumericType*)&sTmpNumericBuffer1;

                /* mul */
                STL_TRY( dtlNumericMultiplication( sValue1,
                                                   & sCount,
                                                   & sNumeric1,
                                                   KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
                
                /* add */
                STL_TRY( dtlNumericSum( sValue2,
                                        & sNumeric1,
                                        KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
            }
        }
    }
    else
    {
        /**
         * Src Value의 Used Block Count 만큼 반복하여 SUM 연산 수행
         */

        if( ( aBlockCnt > 1 ) &&
            ( KNL_GET_BLOCK_ARG_NUM2( aSrcValue ) == 0 ) &&
            ( KNL_GET_BLOCK_ARG_NUM1( aSrcValue )
              <= DTL_DECIMAL_INTEGER_DEFAULT_PRECISION ) )
        {
            STL_TRY( knlAddNumericForInteger( aSrcValue,
                                              sValue2,
                                              aStartBlockIdx,
                                              aStartBlockIdx + aBlockCnt,
                                              aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            for( sLoop = 0 ; sLoop < aBlockCnt ; sLoop++ )
            {
                if( DTL_IS_NULL( sValue1 ) )
                {
                    /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
                }
                else
                {
                    if( DTL_IS_NULL( sValue2 ) )
                    {
                        DTL_COPY_DATA_VALUE( sValue2->mBufferSize, sValue1, sValue2 );
                    }
                    else
                    {
                        /* add */
                        STL_TRY( dtlNumericSum( sValue2,
                                                sValue1,
                                                KNL_ERROR_STACK( aEnv ) )
                                 == STL_SUCCESS );
                    }
                }

                sValue1++;
            }
        }
    }

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Char String 과 Numeric 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Block Count
 * @param[in]      aResultBlockIdx 결과 Value Block Idx
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 Numeric Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddStringToNumeric( knlValueBlockList  * aSrcValue,
                                 knlValueBlockList  * aDestValue,
                                 stlInt32             aStartBlockIdx,
                                 stlInt32             aBlockCnt,
                                 stlInt32             aResultBlockIdx,
                                 knlEnv             * aEnv )
{
    stlInt32          sLoop;

    stlBool           sIsSepSrc;
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue      sNumValue;
    stlChar           sNumBuffer[DTL_NUMERIC_MAX_SIZE];

    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStartBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResultBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );

    
    /**
     * ADD 연산 수행
     */

    sIsSepSrc  = KNL_GET_BLOCK_IS_SEP_BUFF( aSrcValue );
    sValue1    = KNL_GET_BLOCK_DATA_VALUE( aSrcValue, aStartBlockIdx );
    sValue2    = KNL_GET_BLOCK_DATA_VALUE( aDestValue, aResultBlockIdx );

    STL_PARAM_VALIDATE( ( sValue1->mType == DTL_TYPE_CHAR ) ||
                        ( sValue1->mType == DTL_TYPE_VARCHAR ) ||
                        ( sValue1->mType == DTL_TYPE_LONGVARCHAR ),
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( ( sValue2->mType == DTL_TYPE_FLOAT ) ||
                        ( sValue2->mType == DTL_TYPE_NUMBER ) ,
                        KNL_ERROR_STACK( aEnv ) );

    if( sIsSepSrc == STL_FALSE )
    {
        /**
         * Used Block Count 만큼 반복하여 ADD 연산 수행
         */

        if( DTL_IS_NULL( sValue1 ) )
        {
            /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            /* Do Nothing */
        }
        else
        {
            stlChar           sTmpCountBuffer[DTL_NUMERIC_MAX_SIZE];
            dtlDataValue      sCount;
            
            sNumValue.mType   = DTL_TYPE_NUMBER;
            sNumValue.mLength = 0;
            sNumValue.mBufferSize = DTL_NUMERIC_MAX_SIZE;
            sNumValue.mValue  = (dtlNumericType*)&sNumBuffer;

            STL_TRY( dtlGetNumericFromString( (stlChar *) sValue1->mValue,
                                              sValue1->mLength,
                                              DTL_NUMERIC_MAX_PRECISION,
                                              DTL_SCALE_NA,
                                              & sNumValue,
                                              KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            sCount.mType   = DTL_TYPE_NUMBER;
            sCount.mLength = 0;
            sCount.mBufferSize = DTL_NUMERIC_MAX_SIZE;
            sCount.mValue  = (dtlNumericType*)&sTmpCountBuffer;

            /* Integer로 부터 dtfNumeric 얻기 */
            STL_TRY( dtlInt64ToNumeric( aBlockCnt,
                                        & sCount,
                                        KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            if( DTL_IS_NULL( sValue2 ) )
            {
                /* mul */
                STL_TRY( dtlNumericMultiplication( & sNumValue,
                                                   & sCount,
                                                   sValue2,
                                                   KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
            }
            else
            {
                stlChar           sTmpNumericBuffer1[DTL_NUMERIC_MAX_SIZE];
                dtlDataValue      sNumeric1;

                sNumeric1.mType   = DTL_TYPE_NUMBER;
                sNumeric1.mLength = 0;
                sNumeric1.mBufferSize = DTL_NUMERIC_MAX_SIZE;
                sNumeric1.mValue  = (dtlNumericType*)&sTmpNumericBuffer1;

                /* mul */
                STL_TRY( dtlNumericMultiplication( & sNumValue,
                                                   & sCount,
                                                   & sNumeric1,
                                                   KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
                
                /* add */
                STL_TRY( dtlNumericSum( sValue2,
                                        & sNumeric1,
                                        KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
            }
        }
    }
    else
    {
        /**
         * Src Value의 Used Block Count 만큼 반복하여 SUM 연산 수행
         */

        sNumValue.mType   = DTL_TYPE_NUMBER;
        sNumValue.mLength = 0;
        sNumValue.mBufferSize = DTL_NUMERIC_MAX_SIZE;
        sNumValue.mValue  = (dtlNumericType*)&sNumBuffer;
        
        for( sLoop = 0 ; sLoop < aBlockCnt ; sLoop++ )
        {
            if( DTL_IS_NULL( sValue1 ) )
            {
                /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            }
            else
            {
                STL_TRY( dtlGetNumericFromString( (stlChar *) sValue1->mValue,
                                                  sValue1->mLength,
                                                  DTL_NUMERIC_MAX_PRECISION,
                                                  DTL_SCALE_NA,
                                                  & sNumValue,
                                                  KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
                
                if( DTL_IS_NULL( sValue2 ) )
                {
                    DTL_COPY_DATA_VALUE( sValue2->mBufferSize, &sNumValue, sValue2 );
                }
                else
                {
                    /* add */
                    STL_TRY( dtlNumericSum( sValue2,
                                            &sNumValue,
                                            KNL_ERROR_STACK( aEnv ) )
                             == STL_SUCCESS );
                }
            }

            sValue1++;
        }
    }

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Interval YEAR_TO_MONTH 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Block Count
 * @param[in]      aResultBlockIdx 결과 Value Block Idx
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 Interval YEAR_TO_MONTH Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddYearToMonth( knlValueBlockList  * aSrcValue,
                             knlValueBlockList  * aDestValue,
                             stlInt32             aStartBlockIdx,
                             stlInt32             aBlockCnt,
                             stlInt32             aResultBlockIdx,
                             knlEnv             * aEnv )
{
    dtlDataValue                 * sValue1;
    dtlDataValue                 * sValue2;
    dtlIntervalYearToMonthType   * sResultVal;

    stlInt64                       sInput;
    stlInt64                       sSum;
    stlInt32                       sLoop;
    stlBool                        sHasValue;
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStartBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResultBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );

    
    /**
     * ADD 연산 수행
     */

    sValue1    = KNL_GET_BLOCK_DATA_VALUE( aSrcValue, aStartBlockIdx );
    sValue2    = KNL_GET_BLOCK_DATA_VALUE( aDestValue, aResultBlockIdx );
    sResultVal = (dtlIntervalYearToMonthType *) sValue2->mValue;

    STL_PARAM_VALIDATE( sValue1->mType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( sValue2->mType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH,
                        KNL_ERROR_STACK( aEnv ) );

    if( KNL_GET_BLOCK_IS_SEP_BUFF( aSrcValue ) == STL_FALSE )
    {
        /**
         * Used Block Count 만큼 반복하여 ADD 연산 수행
         */

        if( DTL_IS_NULL( sValue1 ) )
        {
            /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            /* Do Nothing */
        }
        else
        {
            sInput = ((dtlIntervalYearToMonthType *) sValue1->mValue)->mMonth;
            sInput *= aBlockCnt;

            if( DTL_IS_NULL( sValue2 ) )
            {
                STL_TRY_THROW( ( sInput > KNL_INTERVAL_MIN_MONTH ) &&
                               ( sInput < KNL_INTERVAL_MAX_MONTH ),
                               ERROR_OUT_OF_RANGE );

                sResultVal->mMonth     = sInput;
                sResultVal->mIndicator = DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH;
                sValue2->mLength       = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
            }
            else
            {
                sResultVal->mMonth += sInput;

                STL_TRY_THROW( ( sResultVal->mMonth > KNL_INTERVAL_MIN_MONTH ) &&
                               ( sResultVal->mMonth < KNL_INTERVAL_MAX_MONTH ),
                               ERROR_OUT_OF_RANGE );
            }
        }
    }
    else
    {
        /**
         * Src Value의 Used Block Count 만큼 반복하여 ADD 연산 수행
         */
        
        sHasValue = STL_FALSE;
        sSum = 0;
        for( sLoop = 0 ; sLoop < aBlockCnt ; sLoop++ )
        {
            if( DTL_IS_NULL( sValue1 ) )
            {
                /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            }
            else
            {
                sSum += ((dtlIntervalYearToMonthType *) sValue1->mValue)->mMonth;
                sHasValue = STL_TRUE;
            }
            sValue1++;
        }

        if( sHasValue == STL_TRUE )
        {
            if( DTL_IS_NULL( sValue2 ) )
            {
                STL_TRY_THROW( ( sSum > KNL_INTERVAL_MIN_MONTH ) &&
                               ( sSum < KNL_INTERVAL_MAX_MONTH ),
                               ERROR_OUT_OF_RANGE );
            
                sResultVal->mMonth     = sSum;
                sResultVal->mIndicator = DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH;
                sValue2->mLength       = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
            }
            else
            {
                sResultVal->mMonth += sSum;
            
                STL_TRY_THROW( ( sResultVal->mMonth > KNL_INTERVAL_MIN_MONTH ) &&
                               ( sResultVal->mMonth < KNL_INTERVAL_MAX_MONTH ),
                               ERROR_OUT_OF_RANGE );
            }
        }
        else
        {
            /* Do Nothing */
        }
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_C_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Interval DAY_TO_SECOND 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValue       Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Block Count
 * @param[in]      aResultBlockIdx 결과 Value Block Idx
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 Interval DAY_TO_SECOND Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddDayToSecond( knlValueBlockList  * aSrcValue,
                             knlValueBlockList  * aDestValue,
                             stlInt32             aStartBlockIdx,
                             stlInt32             aBlockCnt,
                             stlInt32             aResultBlockIdx,
                             knlEnv             * aEnv )
{
    dtlDataValue                 * sValue1;
    dtlDataValue                 * sValue2;
    dtlIntervalDayToSecondType   * sResultVal;

    stlInt64                       sInput;
    stlInt64                       sSum;
    stlInt32                       sLoop;
    stlBool                        sHasValue;
    
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aStartBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResultBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );

    
    /**
     * ADD 연산 수행
     */

    sValue1    = KNL_GET_BLOCK_DATA_VALUE( aSrcValue, aStartBlockIdx );
    sValue2    = KNL_GET_BLOCK_DATA_VALUE( aDestValue, aResultBlockIdx );
    sResultVal = (dtlIntervalDayToSecondType *) sValue2->mValue;

    STL_PARAM_VALIDATE( sValue1->mType == DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( sValue2->mType == DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                        KNL_ERROR_STACK( aEnv ) );

    if( KNL_GET_BLOCK_IS_SEP_BUFF( aSrcValue ) == STL_FALSE )
    {
        /**
         * Used Block Count 만큼 반복하여 ADD 연산 수행
         */

        if( DTL_IS_NULL( sValue1 ) )
        {
            /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            /* Do Nothing */
        }
        else
        {
            sInput = ( ((dtlIntervalDayToSecondType *) sValue1->mValue)->mDay * DTL_USECS_PER_DAY +
                       ((dtlIntervalDayToSecondType *) sValue1->mValue)->mTime );
            sInput *= aBlockCnt;


            if( DTL_IS_NULL( sValue2 ) )
            {
                sResultVal->mDay       = sInput / DTL_USECS_PER_DAY;
                sResultVal->mTime      = sInput % DTL_USECS_PER_DAY;
                sResultVal->mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
                
                STL_TRY_THROW( ( sResultVal->mDay > KNL_INTERVAL_MIN_DAY ) &&
                               ( sResultVal->mDay < KNL_INTERVAL_MAX_DAY ),
                               ERROR_OUT_OF_RANGE );

                sValue2->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
            }
            else
            {
                sInput += ( sResultVal->mDay * DTL_USECS_PER_DAY +
                            sResultVal->mTime );
                
                sResultVal->mDay  = sInput / DTL_USECS_PER_DAY;
                sResultVal->mTime = sInput % DTL_USECS_PER_DAY;

                STL_TRY_THROW( ( sResultVal->mDay > KNL_INTERVAL_MIN_DAY ) &&
                               ( sResultVal->mDay < KNL_INTERVAL_MAX_DAY ),
                               ERROR_OUT_OF_RANGE );
            }
        }
    }
    else
    {
        /**
         * Src Value의 Used Block Count 만큼 반복하여 ADD 연산 수행
         */
        
        sHasValue = STL_FALSE;
        sSum = 0;
        for( sLoop = 0 ; sLoop < aBlockCnt ; sLoop++ )
        {
            if( DTL_IS_NULL( sValue1 ) )
            {
                /* Src Value가 NULL이면 ADD 연산 수행하지 않음 */
            }
            else
            {
                sSum += ( ((dtlIntervalDayToSecondType *) sValue1->mValue)->mDay * DTL_USECS_PER_DAY + 
                          ((dtlIntervalDayToSecondType *) sValue1->mValue)->mTime );
                sHasValue = STL_TRUE;
            }
            sValue1++;
        }
        
        if( sHasValue == STL_TRUE )
        {
            if( DTL_IS_NULL( sValue2 ) )
            {
                sResultVal->mDay       = sSum / DTL_USECS_PER_DAY;
                sResultVal->mTime      = sSum % DTL_USECS_PER_DAY;
                sResultVal->mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
                
                STL_TRY_THROW( ( sResultVal->mDay > KNL_INTERVAL_MIN_DAY ) &&
                               ( sResultVal->mDay < KNL_INTERVAL_MAX_DAY ),
                               ERROR_OUT_OF_RANGE );

                sValue2->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
            }
            else
            {
                sSum += ( sResultVal->mDay * DTL_USECS_PER_DAY +
                          sResultVal->mTime );
            
                sResultVal->mDay  = sSum / DTL_USECS_PER_DAY;
                sResultVal->mTime = sSum % DTL_USECS_PER_DAY;
            
                STL_TRY_THROW( ( sResultVal->mDay > KNL_INTERVAL_MIN_DAY ) &&
                               ( sResultVal->mDay < KNL_INTERVAL_MAX_DAY ),
                               ERROR_OUT_OF_RANGE );
            }
        }
        else
        {
            /* Do Nothing */
        }
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_C_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/************************************************
 * Primitive Function for Aggregation Function
 ***********************************************/

/**
 * @brief 정수와 Numeric Value의 ADD 함수 
 * @param[in]      aIntValue       Int64 정수
 * @param[in,out]  aDestValue      aIntValue와 aDestValue의 ADD 결과가 저장될 data value
 * @param[in]      aPrecision      결과에 대한 NUMERIC Precision 값
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 Numeric Type 이어야 한다.
 *         Dest Value는 반드시 초기화되어 있어야 한다. (NULL값 허용하지 않음)
 */
stlStatus knlAddIntegerValueToNumericValue( stlInt64        aIntValue,
                                            dtlDataValue  * aDestValue,
                                            stlInt64        aPrecision,
                                            knlEnv        * aEnv )
{
    stlChar           sTmpNumericBuffer1[DTL_NUMERIC_MAX_SIZE];
    stlChar           sTmpNumericBuffer2[DTL_NUMERIC_MAX_SIZE];

    dtlDataValue      sNumeric1;
    dtlDataValue      sNumeric2;
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aDestValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue->mType == DTL_TYPE_NUMBER,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDestValue->mValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aPrecision > 0, KNL_ERROR_STACK( aEnv ) );
    

    /**
     * ADD
     */
    
    sNumeric1.mType   = DTL_TYPE_NUMBER;
    sNumeric1.mLength = 0;
    sNumeric1.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumeric1.mValue  = (dtlNumericType*)&sTmpNumericBuffer1;
    
    sNumeric2.mType   = DTL_TYPE_NUMBER;
    sNumeric2.mLength = 0;
    sNumeric2.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumeric2.mValue  = (dtlNumericType*)&sTmpNumericBuffer2;

    /* Int64로 부터 dtfNumeric 얻기 */
    STL_TRY( dtlInt64ToNumeric( aIntValue,
                                & sNumeric1,
                                KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    /* aDestValue 복사 */
    DTL_COPY_DATA_VALUE( sNumeric2.mBufferSize, aDestValue, & sNumeric2 );

    /* add */
    STL_TRY( dtlNumAddition( & sNumeric1,
                             & sNumeric2,
                             aDestValue,
                             KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Data Value로 부터 dtlNumericType 값을 얻는다.
 * @param[in]      aSrcValue       Source Value 
 * @param[out]     aResult         Result Value (Numeric)
 * @param[in]      aEnv            Environment
 */
stlStatus knlGetNumericValue( dtlDataValue  * aSrcValue,
                              dtlDataValue  * aResult,
                              knlEnv        * aEnv )
{
    stlInt64     sInteger;
    stlFloat64   sReal;
    stlBool      sSuccessWithInfo = STL_FALSE;

    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aSrcValue != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResult != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResult->mType == DTL_TYPE_NUMBER, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aResult->mValue != NULL, KNL_ERROR_STACK( aEnv ) );

    switch( dtlDataTypeGroup[ aSrcValue->mType ] )
    {
        case DTL_GROUP_BINARY_INTEGER :
            {
                switch( aSrcValue->mType )
                {
                    case DTL_TYPE_NATIVE_BIGINT :
                        {
                            sInteger = *(dtlBigIntType*) aSrcValue->mValue;
                        }
                    case DTL_TYPE_NATIVE_INTEGER :
                        {
                            sInteger = *(dtlIntegerType*) aSrcValue->mValue;                            
                        }
                    default :
                        {
                            STL_DASSERT( aSrcValue->mType == DTL_TYPE_NATIVE_SMALLINT );
                            sInteger = *(dtlSmallIntType*) aSrcValue->mValue;
                            break;
                        }
                }

                STL_TRY( dtlNumericSetValueFromInteger( sInteger,
                                                        DTL_NUMERIC_DEFAULT_PRECISION,
                                                        DTL_SCALE_NA,
                                                        DTL_STRING_LENGTH_UNIT_NA,
                                                        DTL_INTERVAL_INDICATOR_NA,
                                                        DTL_NUMERIC_MAX_SIZE,
                                                        aResult,
                                                        & sSuccessWithInfo,
                                                        NULL,
                                                        NULL,
                                                        KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
                break;
            }
            
        case DTL_GROUP_BINARY_REAL :
            {
                if( aSrcValue->mType == DTL_TYPE_NATIVE_DOUBLE )
                {
                    sReal = *(dtlDoubleType*) aSrcValue->mValue;
                }
                else
                {
                    STL_DASSERT( aSrcValue->mType == DTL_TYPE_NATIVE_REAL );
                    sReal = *(dtlRealType*) aSrcValue->mValue;
                }
                
                STL_TRY( dtlNumericSetValueFromReal( sReal,
                                                     DTL_NUMERIC_DEFAULT_PRECISION,
                                                     DTL_SCALE_NA,
                                                     DTL_STRING_LENGTH_UNIT_NA,
                                                     DTL_INTERVAL_INDICATOR_NA,
                                                     DTL_NUMERIC_MAX_SIZE,
                                                     aResult,
                                                     & sSuccessWithInfo,
                                                     NULL,
                                                     NULL,
                                                     KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
                break;
            }
            
        case DTL_GROUP_NUMBER :
            {
                DTL_COPY_DATA_VALUE( aResult->mBufferSize, aSrcValue, aResult );
                break;
            }
            
        case DTL_GROUP_CHAR_STRING :
        case DTL_GROUP_LONGVARCHAR_STRING :
            {
                STL_TRY( dtlNumericSetValueFromString( (stlChar *) aSrcValue->mValue,
                                                       aSrcValue->mLength,
                                                       DTL_NUMERIC_DEFAULT_PRECISION,
                                                       DTL_SCALE_NA,
                                                       DTL_STRING_LENGTH_UNIT_NA,
                                                       DTL_INTERVAL_INDICATOR_NA,
                                                       DTL_NUMERIC_MAX_SIZE,
                                                       aResult,
                                                       & sSuccessWithInfo,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
                break;
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Scale 0을 갖는 Numeric 의 Block 단위 ADD 함수 
 * @param[in]      aSrcValueList   Dest에 Add 할 값
 * @param[in]      aDestValue      Src와 Dest의 ADD 결과가 저장
 * @param[in]      aStartIdx       유효한 Value 의 시작 위치
 * @param[in]      aEndIdx         유효한 Value 의 마지막 위치
 * @param[in]      aEnv            Environment
 *
 * @remark Dest Value는 Numeric Type 이어야 한다. (NULL값 허용)
 */
stlStatus knlAddNumericForInteger( knlValueBlockList  * aSrcValueList,
                                   dtlDataValue       * aDestValue,
                                   stlInt32             aStartIdx,
                                   stlInt32             aEndIdx,
                                   knlEnv             * aEnv )
{
    stlInt32            sBlockIdx;
    dtlDataValue      * sSrcValue;

    stlInt64            sIntValue;
    stlInt64            sSumValue;
    stlInt64            sTmpIntValue;
    stlBool             sHasNotNull;
    stlUInt8            sTmpDigit[DTL_NUMERIC_MAX_DIGIT_COUNT + 1];
    dtlDataValue        sTmpDataValue;
    dtlNumericType    * sNumeric;
    stlUInt8          * sDigit;
    stlInt32            sDigitCount;
    stlInt32            sExponent;
    stlInt32            sLength;
    stlBool             sIsTrail;
        
    sSumValue = 0;
    sHasNotNull = STL_FALSE;

    for( sBlockIdx = aStartIdx; sBlockIdx < aEndIdx; sBlockIdx++ )
    {
        sSrcValue = KNL_GET_BLOCK_DATA_VALUE( aSrcValueList, sBlockIdx );

        if( DTL_IS_NULL( sSrcValue ) )
        {
            /* Do Nothing */
        }
        else
        {
            sHasNotNull = STL_TRUE;
                
            sNumeric = (dtlNumericType*)( sSrcValue->mValue );
            sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sSrcValue->mLength );
            sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric ) - sDigitCount + 1;
            sDigit = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
            sIntValue = 0;
                
            if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
            {
                while( sDigitCount > 0 )
                {
                    sIntValue *= 100;
                    sIntValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    sDigit++;
                    sDigitCount--;
                }

                while( sExponent > 0 )
                {
                    sIntValue *= 100;
                    sExponent--;
                }

                sSumValue += sIntValue;
            }
            else
            {
                while( sDigitCount > 0 )
                {
                    sIntValue *= 100;
                    sIntValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    sDigit++;
                    sDigitCount--;
                }

                while( sExponent > 0 )
                {
                    sIntValue *= 100;
                    sExponent--;
                }
                    
                sSumValue -= sIntValue;
            }
        }
    }

    if( sHasNotNull == STL_FALSE )
    {
        /* Do Nothing */
    }
    else
    {
        sTmpDataValue.mType = DTL_TYPE_NUMBER;

        if( sSumValue == 0 )
        {
            sTmpDataValue.mValue = sTmpDigit;
            DTL_NUMERIC_SET_ZERO( ((dtlNumericType*)sTmpDataValue.mValue), sTmpDataValue.mLength );
        }
        else if( sSumValue > 0 )
        {
            /* set numeric value */
            sDigit = & sTmpDigit[ DTL_NUMERIC_MAX_DIGIT_COUNT ];
            sExponent = -1;
            sLength = 1;
            sIsTrail = STL_TRUE;

            while( sSumValue > 0 )
            {
                sTmpIntValue = sSumValue / 100;
                *sDigit = sSumValue - ( sTmpIntValue * 100 ) + DTL_NUMERIC_DIGIT_POSITIVE_FENCE_VALUE;

                if( ( sIsTrail == STL_FALSE ) ||
                    ( *sDigit != DTL_NUMERIC_POSITIVE_DIGIT_ZERO_VALUE ) )
                {
                    sDigit--;
                    sIsTrail = STL_FALSE;
                    sLength++;
                }

                sExponent++;
                sSumValue = sTmpIntValue;
            }

            sTmpDataValue.mLength = sLength;
            sTmpDataValue.mValue = sDigit;
            DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( ((dtlNumericType*)sTmpDataValue.mValue), sExponent );
        }
        else
        {
            /* set numeric value */
            sDigit = & sTmpDigit[ DTL_NUMERIC_MAX_DIGIT_COUNT ];
            sExponent = -1;
            sLength = 1;
            sIsTrail = STL_TRUE;

            sSumValue = -sSumValue;
            while( sSumValue > 0 )
            {
                sTmpIntValue = sSumValue / 100;
                *sDigit = DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE - ( sSumValue - ( sTmpIntValue * 100 ) );

                if( ( sIsTrail == STL_FALSE ) ||
                    ( *sDigit != DTL_NUMERIC_NEGATIVE_DIGIT_ZERO_VALUE ) )
                {
                    sDigit--;
                    sIsTrail = STL_FALSE;
                    sLength++;
                }
                
                sExponent++;
                sSumValue = sTmpIntValue;
            }

            sTmpDataValue.mLength = sLength;
            sTmpDataValue.mValue = sDigit;
            DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( ((dtlNumericType*)sTmpDataValue.mValue), sExponent );
        }
            
        if( DTL_IS_NULL( aDestValue ) )
        {
            /* assign */
            aDestValue->mLength = sTmpDataValue.mLength;
            stlMemcpy( aDestValue->mValue, sTmpDataValue.mValue, aDestValue->mLength );
        }
        else
        {
            /* add */
            STL_TRY( dtlNumericSum( aDestValue,
                                    &sTmpDataValue,
                                    KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );
        }
    }
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
