/*******************************************************************************
 * dtlPhysicalFuncIsNotEqual.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlPhysicalFuncIsNotEqual.c 4494 2012-05-08 03:51:07Z ehpark $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file dtlPhysicalFuncIsNotEqual.c
 * @brief Align이 보장되지 않은 value간의 연산
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfNumeric.h>


/**
 * @addtogroup dtlPhysicalFunc
 * @{
 */

/**
 * @brief IS NOT EQUAL 연산 ( BOOLEAN VS BOOLEAN )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualBooleanToBoolean( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen )
{
    dtlBooleanType  sLeft;
    dtlBooleanType  sRight;
    
    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_BOOLEAN_SIZE ) );
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_BOOLEAN_SIZE ) );
    
    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_BOOLEAN_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_BOOLEAN_FROM_PTR( aRightVal, sRight );
        
        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( SMALLINT TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualSmallIntToSmallInt( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen )
{
    dtlSmallIntType  sLeft;
    dtlSmallIntType  sRight;
    
    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_SMALLINT_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_SMALLINT_SIZE ) ); 
    
    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_SMALLINT_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_SMALLINT_FROM_PTR( aRightVal, sRight );
        
        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( SMALLINT TO INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualSmallIntToInteger( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen )
{
    dtlSmallIntType  sLeft;
    dtlIntegerType   sRight;
    
    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_SMALLINT_SIZE ) );
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_INTEGER_SIZE ) );
    
    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_SMALLINT_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_INTEGER_FROM_PTR( aRightVal, sRight );
        
        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( SMALLINT TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualSmallIntToBigInt( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen )
{
    dtlSmallIntType  sLeft;
    dtlBigIntType    sRight;
    
    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_SMALLINT_SIZE ) );
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_BIGINT_SIZE ) );
    
    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_SMALLINT_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_BIGINT_FROM_PTR( aRightVal, sRight );
        
        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( SMALLINT TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualSmallIntToReal( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen )
{
    dtlSmallIntType  sLeft;
    dtlRealType      sRight;
    
    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_SMALLINT_SIZE ) );
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_REAL_SIZE ) );
    
    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_SMALLINT_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_REAL_FROM_PTR( aRightVal, sRight );
        
        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( SMALLINT TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualSmallIntToDouble( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen )
{
    dtlSmallIntType  sLeft;
    dtlDoubleType    sRight;
    
    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_SMALLINT_SIZE ) );
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_DOUBLE_SIZE ) );
    
    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_SMALLINT_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_DOUBLE_FROM_PTR( aRightVal, sRight );
        
        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( SMALLINT TO NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualSmallIntToNumeric( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen )
{
    dtlSmallIntType     sLeft;
    dtlNumericType    * sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_SMALLINT_SIZE ) ); 
    STL_DASSERT( ( aRightLen >= 0 ) && ( aRightLen <= DTL_NUMERIC_MAX_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_SMALLINT_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NUMERIC_PTR_FROM_PTR( aRightVal, sRight );

        return dtlIsNotEqualNumericToSmallInt_INLINE( sRight,
                                                      aRightLen,
                                                      sLeft,
                                                      aLeftLen );

    }
}

/**
 * @brief IS NOT EQUAL 연산 ( INTEGER TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualIntegerToSmallInt( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen )
{
    dtlIntegerType     sLeft;
    dtlSmallIntType    sRight;
    
    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_INTEGER_SIZE ) );
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_SMALLINT_SIZE ) );
    
    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_INTEGER_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_SMALLINT_FROM_PTR( aRightVal, sRight );
        
        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( INTEGER VS INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualIntegerToInteger( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen )
{
    dtlIntegerType  sLeft;
    dtlIntegerType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_INTEGER_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_INTEGER_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_INTEGER_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_INTEGER_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( INTEGER TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualIntegerToBigInt( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen )
{
    dtlIntegerType  sLeft;
    dtlBigIntType   sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_INTEGER_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_BIGINT_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_INTEGER_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_BIGINT_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( INTEGER TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualIntegerToReal( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen )
{
    dtlIntegerType  sLeft;
    dtlRealType     sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_INTEGER_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_REAL_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_INTEGER_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_REAL_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( INTEGER TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualIntegerToDouble( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen )
{
    dtlIntegerType  sLeft;
    dtlDoubleType   sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_INTEGER_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_DOUBLE_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_INTEGER_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_DOUBLE_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( INTEGER VS NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualIntegerToNumeric( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen )
{
    dtlIntegerType      sLeft;
    dtlNumericType    * sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_INTEGER_SIZE ) ); 
    STL_DASSERT( ( aRightLen >= 0 ) && ( aRightLen <= DTL_NUMERIC_MAX_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_INTEGER_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NUMERIC_PTR_FROM_PTR( aRightVal, sRight );

        return dtlIsNotEqualNumericToInteger_INLINE( sRight,
                                                     aRightLen,
                                                     sLeft,
                                                     aLeftLen );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( BIGINT TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualBigIntToSmallInt( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen )
{
    dtlBigIntType    sLeft;
    dtlSmallIntType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_BIGINT_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_SMALLINT_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_BIGINT_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_SMALLINT_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( BIGINT TO INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualBigIntToInteger( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen )
{
    dtlBigIntType   sLeft;
    dtlIntegerType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_BIGINT_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_INTEGER_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_BIGINT_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_INTEGER_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}


/**
 * @brief IS NOT EQUAL 연산 ( BIGINT TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualBigIntToBigInt( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen )
{
    dtlBigIntType  sLeft;
    dtlBigIntType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_BIGINT_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_BIGINT_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_BIGINT_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_BIGINT_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( BIGINT TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualBigIntToReal( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen )
{
    dtlBigIntType  sLeft;
    dtlRealType    sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_BIGINT_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_REAL_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_BIGINT_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_REAL_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( BIGINT TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualBigIntToDouble( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen )
{
    dtlBigIntType  sLeft;
    dtlDoubleType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_BIGINT_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_DOUBLE_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_BIGINT_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_DOUBLE_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( BIGINT TO NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualBigIntToNumeric( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen )
{
    dtlBigIntType        sLeft;
    dtlNumericType     * sRight;
    
    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_BIGINT_SIZE ) ); 
    STL_DASSERT( ( aRightLen >= 0 ) && ( aRightLen <= DTL_NUMERIC_MAX_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_BIGINT_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NUMERIC_PTR_FROM_PTR( aRightVal, sRight );

        return dtlIsNotEqualNumericToBigInt_INLINE( sRight,
                                                    aRightLen,
                                                    sLeft,
                                                    aLeftLen );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( REAL TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualRealToSmallInt( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen )
{
    dtlRealType      sLeft;
    dtlSmallIntType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_REAL_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_SMALLINT_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_REAL_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_SMALLINT_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( REAL TO INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualRealToInteger( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen )
{
    dtlRealType     sLeft;
    dtlIntegerType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_REAL_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_INTEGER_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_REAL_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_INTEGER_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( REAL TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualRealToBigInt( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen )
{
    dtlRealType    sLeft;
    dtlBigIntType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_REAL_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_BIGINT_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_REAL_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_BIGINT_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( RELA TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualRealToReal( void       * aLeftVal,
                                             stlInt64     aLeftLen,
                                             void       * aRightVal,
                                             stlInt64     aRightLen )
{
    dtlRealType  sLeft;
    dtlRealType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_REAL_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_REAL_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_REAL_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_REAL_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( REAL TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualRealToDouble( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen )
{
    dtlRealType    sLeft;
    dtlDoubleType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_REAL_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_DOUBLE_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_REAL_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_DOUBLE_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( REAL TO NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualRealToNumeric( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen )
{
    dtlRealType       sLeft;
    dtlNumericType  * sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_REAL_SIZE ) ); 
    STL_DASSERT( ( aRightLen >= 0 ) && ( aRightLen <= DTL_NUMERIC_MAX_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_REAL_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NUMERIC_PTR_FROM_PTR( aRightVal, sRight );
 
        return dtlIsNotEqualNumericToReal_INLINE( sRight,
                                                  aRightLen,
                                                  sLeft,
                                                  aLeftLen );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( DOUBLE TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualDoubleToSmallInt( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen )
{
    dtlDoubleType    sLeft;
    dtlSmallIntType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_DOUBLE_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_SMALLINT_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_DOUBLE_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_SMALLINT_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( DOUBLE TO INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualDoubleToInteger( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen )
{
    dtlDoubleType   sLeft;
    dtlIntegerType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_DOUBLE_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_INTEGER_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_DOUBLE_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_INTEGER_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( DOUBLE TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualDoubleToBigInt( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen )
{
    dtlDoubleType  sLeft;
    dtlBigIntType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_DOUBLE_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_BIGINT_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_DOUBLE_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_BIGINT_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( DOUBLE TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualDoubleToReal( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen )
{
    dtlDoubleType  sLeft;
    dtlRealType    sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_DOUBLE_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_REAL_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_DOUBLE_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_REAL_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( DOUBLE TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualDoubleToDouble( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen )
{
    dtlDoubleType  sLeft;
    dtlDoubleType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_DOUBLE_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_DOUBLE_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_DOUBLE_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_DOUBLE_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( DOUBLE TO NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualDoubleToNumeric( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen )
{
    dtlDoubleType     sLeft;
    dtlNumericType  * sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_DOUBLE_SIZE ) ); 
    STL_DASSERT( ( aRightLen >= 0 ) && ( aRightLen <= DTL_NUMERIC_MAX_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_DOUBLE_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NUMERIC_PTR_FROM_PTR( aRightVal, sRight );

        return dtlIsNotEqualNumericToDouble_INLINE( sRight,
                                                    aRightLen,
                                                    sLeft,
                                                    aLeftLen );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( NUMERIC TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualNumericToSmallInt( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen )
{
    dtlNumericType    * sLeft;
    dtlSmallIntType     sRight;
    
    STL_DASSERT( ( aLeftLen >= 0 ) && ( aLeftLen <= DTL_NUMERIC_MAX_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_SMALLINT_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_SMALLINT_FROM_PTR( aRightVal, sRight );

        return dtlIsNotEqualNumericToSmallInt_INLINE( sLeft,
                                                      aLeftLen,
                                                      sRight,
                                                      aRightLen );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( NUMERIC TO INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualNumericToInteger( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen )
{
    dtlNumericType    * sLeft;
    dtlIntegerType      sRight;
    
    STL_DASSERT( ( aLeftLen >= 0 ) && ( aLeftLen <= DTL_NUMERIC_MAX_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_INTEGER_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_INTEGER_FROM_PTR( aRightVal, sRight );

        return dtlIsNotEqualNumericToInteger_INLINE( sLeft,
                                                     aLeftLen,
                                                     sRight,
                                                     aRightLen );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( NUMERIC TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualNumericToBigInt( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen )
{
    dtlNumericType    * sLeft;
    dtlBigIntType       sRight;
    
    STL_DASSERT( ( aLeftLen >= 0 ) && ( aLeftLen <= DTL_NUMERIC_MAX_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_BIGINT_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_BIGINT_FROM_PTR( aRightVal, sRight );
        
        return dtlIsNotEqualNumericToBigInt_INLINE( sLeft,
                                                    aLeftLen,
                                                    sRight,
                                                    aRightLen );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( NUMERIC TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualNumericToReal( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen )
{
    dtlNumericType   * sLeft;
    dtlRealType        sRight;

    STL_DASSERT( ( aLeftLen >= 0 ) && ( aLeftLen <= DTL_NUMERIC_MAX_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_REAL_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_REAL_FROM_PTR( aRightVal, sRight );

        return dtlIsNotEqualNumericToReal_INLINE( sLeft,
                                                  aLeftLen,
                                                  sRight,
                                                  aRightLen );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( NUMERIC TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualNumericToDouble( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen )
{
    dtlNumericType  * sLeft;
    dtlDoubleType     sRight;

    STL_DASSERT( ( aLeftLen >= 0 ) && ( aLeftLen <= DTL_NUMERIC_MAX_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_NATIVE_DOUBLE_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_DOUBLE_FROM_PTR( aRightVal, sRight );
 
        return dtlIsNotEqualNumericToDouble_INLINE( sLeft,
                                                    aLeftLen,
                                                    sRight,
                                                    aRightLen );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( NUMERIC TO NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualNumericToNumeric( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen )
{
    stlUInt8  * sLeftDigit;
    stlUInt8  * sRightDigit;
    stlInt32    i;

    STL_DASSERT( ( aLeftLen >= 0 ) && ( aLeftLen <= DTL_NUMERIC_MAX_SIZE ) );
    STL_DASSERT( ( aRightLen >= 0 ) && ( aRightLen <= DTL_NUMERIC_MAX_SIZE ) );

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }

    if( aLeftLen != aRightLen )
    {
        return STL_TRUE;
    }
    
    sLeftDigit  = (stlUInt8*) aLeftVal;
    sRightDigit = (stlUInt8*) aRightVal;

    if( *sLeftDigit == *sRightDigit )
    {
        if( aLeftLen == 1 )
        {
            return STL_FALSE;
        }

        aLeftLen--;
        for( i = 1 ; sLeftDigit[i] == sRightDigit[i] ; i++ )
        {
            if( i == aLeftLen )
            {
                return STL_FALSE;
            }
        }
    }

    return STL_TRUE;
}

/**
 * @brief IS NOT EQUAL 연산 ( CHARACTER  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualFixedLengthChar( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen )
{
    dtlCharType sLeft;
    dtlCharType sRight;

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_CHAR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_CHAR_FROM_PTR( aRightVal, sRight );

        return dtlIsNotEqualFixedLengthChar_INLINE( sLeft,
                                                    aLeftLen,
                                                    sRight,
                                                    aRightLen );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( CHARACTER  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualVariableLengthChar( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen )
{
    dtlVarcharType sLeft;
    dtlVarcharType sRight;
    
    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        if( aLeftLen == aRightLen )
        {
            DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
            DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );

            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) != 0 );
        }
        else
        {
            return STL_TRUE;
        }
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( CHAR vs. LONGVARCHAR )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualCharAndLongvarchar( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen )
{
    dtlVarcharType sLeft;
    dtlVarcharType sRight;
    
    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        if( aLeftLen == aRightLen )
        {
            DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
            DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );

            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) != 0 );
        }
        else
        {
            return STL_TRUE;
        }
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( LONGVARCHAR vs. CHAR )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualLongvarcharAndChar( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen )
{
    dtlVarcharType sLeft;
    dtlVarcharType sRight;
    
    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        if( aLeftLen == aRightLen )
        {
            DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
            DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );

            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) != 0 );
        }
        else
        {
            return STL_TRUE;
        }
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( VARCHAR vs. LONGVARCHAR )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualVarcharAndLongvarchar( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen )
{
    dtlVarcharType sLeft;
    dtlVarcharType sRight;
    
    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        if( aLeftLen == aRightLen )
        {
            DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
            DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );

            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) != 0 );
        }
        else
        {
            return STL_TRUE;
        }
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( LONGVARCHAR vs. VARCHAR )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualLongvarcharAndVarchar( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen )
{
    dtlVarcharType sLeft;
    dtlVarcharType sRight;
    
    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        if( aLeftLen == aRightLen )
        {
            DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
            DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );

            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) != 0 );
        }
        else
        {
            return STL_TRUE;
        }
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( LONGVARCHAR vs. LONGVARCHAR )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualLongvarcharAndLongvarchar( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen )
{
    dtlVarcharType sLeft;
    dtlVarcharType sRight;
    
    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        if( aLeftLen == aRightLen )
        {
            DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
            DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );

            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) != 0 );
        }
        else
        {
            return STL_TRUE;
        }
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( BINARY  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualFixedLengthBinary( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen )
{
    dtlBinaryType sLeft;
    dtlBinaryType sRight;

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_BINARY_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_BINARY_FROM_PTR( aRightVal, sRight );
        
        return dtlIsNotEqualFixedLengthBinary_INLINE( sLeft,
                                                      aLeftLen,
                                                      sRight,
                                                      aRightLen );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( BINARY  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualVariableLengthBinary( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if(  ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        if( aLeftLen == aRightLen )
        {
            DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
            DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );
        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) != 0 );
        }
        else
        {
            return STL_TRUE;
        }
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( BINARY vs. LONGVARBINARY )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualBinaryAndLongvarbinary( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if(  ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        if( aLeftLen == aRightLen )
        {
            DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
            DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );
        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) != 0 );
        }
        else
        {
            return STL_TRUE;
        }
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( LONGVARBINARY vs. BINARY )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualLongvarbinaryAndBinary( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if(  ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        if( aLeftLen == aRightLen )
        {
            DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
            DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );
        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) != 0 );
        }
        else
        {
            return STL_TRUE;
        }
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( VARBINARY vs. LONGVARBINARY )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualVarbinaryAndLongvarbinary( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if(  ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        if( aLeftLen == aRightLen )
        {
            DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
            DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );
        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) != 0 );
        }
        else
        {
            return STL_TRUE;
        }
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( LONGVARBINARY vs. VARBINARY )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualLongvarbinaryAndVarbinary( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if(  ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        if( aLeftLen == aRightLen )
        {
            DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
            DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );
        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) != 0 );
        }
        else
        {
            return STL_TRUE;
        }
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( LONGVARBINARY vs. LONGVARBINARY )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualLongvarbinaryAndLongvarbinary( void       * aLeftVal,
                                                                stlInt64     aLeftLen,
                                                                void       * aRightVal,
                                                                stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if(  ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        if( aLeftLen == aRightLen )
        {
            DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
            DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );
        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) != 0 );
        }
        else
        {
            return STL_TRUE;
        }
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( DATE VS DATE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualDateToDate( void       * aLeftVal,
                                             stlInt64     aLeftLen,
                                             void       * aRightVal,
                                             stlInt64     aRightLen )
{
    dtlDateType  sLeft;
    dtlDateType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_DATE_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_DATE_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_DATE_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_DATE_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}


/**
 * @brief IS NOT EQUAL 연산 ( DATE VS TIMESTAMP )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualDateToTimestamp( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen )
{
    dtlDateType      sLeft;
    dtlTimestampType sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_DATE_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_TIMESTAMP_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_DATE_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_TIMESTAMP_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( TIME VS TIME )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualTimeToTime( void       * aLeftVal,
                                             stlInt64     aLeftLen,
                                             void       * aRightVal,
                                             stlInt64     aRightLen )
{
    dtlTimeType  sLeft;
    dtlTimeType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_TIME_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_TIME_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_TIME_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_TIME_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( TIME WITH TIME ZONE VS TIME WITH TIME ZONE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualTimeTzToTimeTz( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen )
{
    dtlTimeTzType  sLeft;
    dtlTimeTzType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_TIMETZ_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_TIMETZ_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_TIME_TZ_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_TIME_TZ_FROM_PTR( aRightVal, sRight );

        return ( sLeft.mTime != sRight.mTime );                
    }
}


/**
 * @brief IS NOT EQUAL 연산 ( TIMESTAMP VS DATE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualTimestampToDate( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen )
{
    dtlTimestampType  sLeft;
    dtlDateType       sRight;
    
    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_TIMESTAMP_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_DATE_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_TIMESTAMP_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_DATE_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( TIMESTAMP VS TIMESTAMP )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualTimestampToTimestamp( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen )
{
    dtlTimestampType  sLeft;
    dtlTimestampType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_TIMESTAMP_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_TIMESTAMP_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_TIMESTAMP_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_TIMESTAMP_FROM_PTR( aRightVal, sRight );

        return ( sLeft != sRight );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( TIMESTAMP WITH TIME ZONE VS TIMESTAMP WITH TIME ZONE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualTimestampTzToTimestampTz( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen )
{
    dtlTimestampTzType  sLeft;
    dtlTimestampTzType  sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_TIMESTAMPTZ_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_TIMESTAMPTZ_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_TIMESTAMP_TZ_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_TIMESTAMP_TZ_FROM_PTR( aRightVal, sRight );

        return ( sLeft.mTimestamp != sRight.mTimestamp );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( INTERVAL YEAR TO MONTH VS INTERVAL YEAR TO MONTH )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualIntervalYearToMonth( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen )
{
    dtlIntervalYearToMonthType   sLeft;
    dtlIntervalYearToMonthType   sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_INTERVAL_YEAR_TO_MONTH_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_INTERVAL_YEAR_TO_MONTH_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_INTERVAL_YEAR_TO_MONTH_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_INTERVAL_YEAR_TO_MONTH_FROM_PTR( aRightVal, sRight );

        return ( sLeft.mMonth != sRight.mMonth );
    }
}

/**
 * @brief IS NOT EQUAL 연산 ( INTERVAL DAY TO SECOND VS INTERVAL DAY TO SECOND )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualIntervalDayToSecond( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen )
{
    dtlIntervalDayToSecondType   sLeft;
    dtlIntervalDayToSecondType   sRight;
    dtlTimeOffset                sLeftValSpan;
    dtlTimeOffset                sRightValSpan;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_INTERVAL_DAY_TO_SECOND_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_INTERVAL_DAY_TO_SECOND_SIZE ) ); 
    
    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {        
        DTL_GET_INTERVAL_DAY_TO_SECOND_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_INTERVAL_DAY_TO_SECOND_FROM_PTR( aRightVal, sRight );

        sLeftValSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE( &sLeft );
        sRightValSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE( &sRight );

        return ( sLeftValSpan != sRightValSpan );
    }   
}

/**
 * @brief IS NOT EQUAL 연산 ( ROWID VS ROWID )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNotEqualRowIdToRowId( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen )
{
    dtlRowIdType  * sLeft;
    dtlRowIdType  * sRight;
    
    STL_DASSERT( (aLeftLen == 0) || (aLeftLen == DTL_ROWID_SIZE) );
    STL_DASSERT( (aRightLen == 0) || (aRightLen == DTL_ROWID_SIZE) );
    
    if( (aLeftLen == 0) || (aRightLen == 0) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_ROWID_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_ROWID_FROM_PTR( aRightVal, sRight );
        
        return( stlMemcmp( sLeft->mDigits,
                           sRight->mDigits,
                           DTL_ROWID_SIZE ) != 0 );
    }
}


/**
 * @brief Not In Physical Func List To List
 *   <BR> List로 넘어오는 dtlDataValue에 대해 해당 Physical Func을 연결해준다.
 * @param[in]  aLeftVal     Left dtlDataValue List 
 * @param[in]  aLeftLen     Left Length
 * @param[in]  aRightVal    Right dtlDataValue List
 * @param[in]  aRightCount  Right Argument List Count
 * @return 연산 결과 
 */
stlBool dtlNotInPhysicalFuncListToList( void       * aLeftVal,
                                        stlInt64     aLeftLen,
                                        void       * aRightVal,
                                        stlInt64     aRightCount )
{
    dtlDataType     sLeftType;
    dtlDataValue    sRightValue;
    stlBool         sResultValue;
    stlInt32        i;

    STL_DASSERT( ( aRightCount >= 0 ) && ( aRightCount <= 1000 ) );

    sLeftType = (((dtlDataValue*)aRightVal)[aRightCount]).mType;
    
    for( i = 0 ; i < aRightCount ; i ++ )
    {
        sRightValue = *(dtlDataValue*)(((dtlDataValue*)aRightVal)[i].mValue);

        if( ( aLeftLen == 0 ) || ( sRightValue.mLength == 0 ) )
        {
            return STL_FALSE;
        }
        else
        {
            sResultValue =
                ( dtlPhysicalFuncArg2FuncList[ sLeftType ][ sRightValue.mType ]
                  [ DTL_PHYSICAL_FUNC_IS_NOT_EQUAL ]
                  ( aLeftVal,
                    aLeftLen,
                    sRightValue.mValue,
                    sRightValue.mLength ) );

            if( sResultValue == STL_FALSE )
            {
                return STL_FALSE;
            }
        }
    }
    return STL_TRUE;
}

/**
 * @brief Not Equal Any Physical Func List To List
 *   <BR> List로 넘어오는 dtlDataValue에 대해 해당 Physical Func을 연결해준다.
 * @param[in]  aLeftVal     Left dtlDataValue List 
 * @param[in]  aLeftLen     Left Length
 * @param[in]  aRightVal    Right dtlDataValue List
 * @param[in]  aRightCount  Right Argument List Count
 * @return 연산 결과 
 */
stlBool dtlNotEqualAnyPhysicalFunc( void       * aLeftVal,
                                    stlInt64     aLeftLen,
                                    void       * aRightVal,
                                    stlInt64     aRightCount )
{
    dtlDataType     sLeftType;
    dtlDataValue    sRightValue;
    stlBool         sResultValue;
    stlInt32        i;

    STL_DASSERT( ( aRightCount >= 0 ) && ( aRightCount <= 1000 ) );

    sLeftType = (((dtlDataValue*)aRightVal)[aRightCount]).mType;
    
    for( i = 0 ; i < aRightCount ; i ++ )
    {
        sRightValue = *(dtlDataValue*)(((dtlDataValue*)aRightVal)[i].mValue);

        if( ( aLeftLen == 0 ) || ( sRightValue.mLength == 0 ) )
        {
            /* Do Nothing */
        }
        else
        {
            sResultValue =
                ( dtlPhysicalFuncArg2FuncList[ sLeftType ][ sRightValue.mType ]
                  [ DTL_PHYSICAL_FUNC_IS_NOT_EQUAL ]
                  ( aLeftVal,
                    aLeftLen,
                    sRightValue.mValue,
                    sRightValue.mLength ) );

            if( sResultValue == STL_TRUE )
            {
                return STL_TRUE;
            }
        }
    }
    return STL_FALSE;
}

/** @} dtlPhysicalFunc */
