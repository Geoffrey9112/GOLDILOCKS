/*******************************************************************************
 * dtlPhysicalFuncIsEqual.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
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
 * @file dtlPhysicalFuncIsEqual.c
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
 * @brief IS EQUAL 연산 ( BOOLEAN VS BOOLEAN )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualBooleanToBoolean( void       * aLeftVal,
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
        
        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( SMALLINT TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualSmallIntToSmallInt( void       * aLeftVal,
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
        
        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( SMALLINT TO INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualSmallIntToInteger( void       * aLeftVal,
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
        
        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( SMALLINT TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualSmallIntToBigInt( void       * aLeftVal,
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
        DTL_GET_NATIVE_BIGINT_FROM_PTR( aRightVal,  sRight );
        
        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( SMALLINT TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualSmallIntToReal( void       * aLeftVal,
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
        
        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( SMALLINT TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualSmallIntToDouble( void       * aLeftVal,
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
        
        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( SMALLINT TO NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualSmallIntToNumeric( void       * aLeftVal,
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

        return dtlIsEqualNumericToSmallInt_INLINE( sRight,
                                                   aRightLen,
                                                   sLeft,
                                                   aLeftLen );
    }
    
    return STL_FALSE;    
}

/**
 * @brief IS EQUAL 연산 ( INTEGER TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualIntegerToSmallInt( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen )
{
    dtlIntegerType    sLeft;
    dtlSmallIntType   sRight;
    
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
        
        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( INTEGER VS INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualIntegerToInteger( void       * aLeftVal,
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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( INTEGER TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualIntegerToBigInt( void       * aLeftVal,
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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( INTEGER TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualIntegerToReal( void       * aLeftVal,
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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( INTEGER TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualIntegerToDouble( void       * aLeftVal,
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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( INTEGER VS NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualIntegerToNumeric( void       * aLeftVal,
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

        return dtlIsEqualNumericToInteger_INLINE( sRight,
                                                  aRightLen,
                                                  sLeft,
                                                  aLeftLen );
    }
}

/**
 * @brief IS EQUAL 연산 ( BIGINT TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualBigIntToSmallInt( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen )
{
    dtlBigIntType       sLeft;
    dtlSmallIntType     sRight;

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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( BIGINT TO INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualBigIntToInteger( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen )
{
    dtlBigIntType       sLeft;
    dtlIntegerType      sRight;

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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( BIGINT TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualBigIntToBigInt( void       * aLeftVal,
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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( BIGINT TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualBigIntToReal( void       * aLeftVal,
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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( BIGINT TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualBigIntToDouble( void       * aLeftVal,
                                              stlInt64     aLeftLen,
                                              void       * aRightVal,
                                              stlInt64     aRightLen )
{
    dtlBigIntType    sLeft;
    dtlDoubleType    sRight;

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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( BIGINT TO NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualBigIntToNumeric( void       * aLeftVal,
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

        return dtlIsEqualNumericToBigInt_INLINE( sRight,
                                                 aRightLen,
                                                 sLeft,
                                                 aLeftLen );
    }
}

/**
 * @brief IS EQUAL 연산 ( REAL TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualRealToSmallInt( void       * aLeftVal,
                                              stlInt64     aLeftLen,
                                              void       * aRightVal,
                                              stlInt64     aRightLen )
{
    dtlRealType        sLeft;
    dtlSmallIntType    sRight;

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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( REAL TO INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualRealToInteger( void       * aLeftVal,
                                             stlInt64     aLeftLen,
                                             void       * aRightVal,
                                             stlInt64     aRightLen )
{
    dtlRealType        sLeft;
    dtlIntegerType     sRight;

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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( REAL TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualRealToBigInt( void       * aLeftVal,
                                            stlInt64     aLeftLen,
                                            void       * aRightVal,
                                            stlInt64     aRightLen )
{
    dtlRealType        sLeft;
    dtlBigIntType      sRight;

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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( RELA TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualRealToReal( void       * aLeftVal,
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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( REAL TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualRealToDouble( void       * aLeftVal,
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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( REAL TO NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualRealToNumeric( void       * aLeftVal,
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
 
        return dtlIsEqualNumericToReal_INLINE( sRight,
                                               aRightLen,
                                               sLeft,
                                               aLeftLen );
    }
}

/**
 * @brief IS EQUAL 연산 ( DOUBLE TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualDoubleToSmallInt( void       * aLeftVal,
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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( DOUBLE TO INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualDoubleToInteger( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen )
{
    dtlDoubleType    sLeft;
    dtlIntegerType   sRight;

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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( DOUBLE TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualDoubleToBigInt( void       * aLeftVal,
                                              stlInt64     aLeftLen,
                                              void       * aRightVal,
                                              stlInt64     aRightLen )
{
    dtlDoubleType    sLeft;
    dtlBigIntType    sRight;

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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( DOUBLE TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualDoubleToReal( void       * aLeftVal,
                                            stlInt64     aLeftLen,
                                            void       * aRightVal,
                                            stlInt64     aRightLen )
{
    dtlDoubleType    sLeft;
    dtlRealType      sRight;

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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( DOUBLE TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualDoubleToDouble( void       * aLeftVal,
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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( DOUBLE TO NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualDoubleToNumeric( void       * aLeftVal,
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

        return dtlIsEqualNumericToDouble_INLINE( sRight,
                                                 aRightLen,
                                                 sLeft,
                                                 aLeftLen );
    }
}

/**
 * @brief IS EQUAL 연산 ( NUMERIC TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualNumericToSmallInt( void       * aLeftVal,
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
        
        return dtlIsEqualNumericToSmallInt_INLINE( sLeft,
                                                   aLeftLen,
                                                   sRight,
                                                   aRightLen );
    }
}

/**
 * @brief IS EQUAL 연산 ( NUMERIC TO INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualNumericToInteger( void       * aLeftVal,
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

        return dtlIsEqualNumericToInteger_INLINE( sLeft,
                                                  aLeftLen,
                                                  sRight,
                                                  aRightLen );
    }
}

/**
 * @brief IS EQUAL 연산 ( NUMERIC TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualNumericToBigInt( void       * aLeftVal,
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

        return dtlIsEqualNumericToBigInt_INLINE( sLeft,
                                                 aLeftLen,
                                                 sRight,
                                                 aRightLen );
    }
}
/**
 * @brief IS EQUAL 연산 ( NUMERIC TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualNumericToReal( void       * aLeftVal,
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

        return dtlIsEqualNumericToReal_INLINE( sLeft,
                                               aLeftLen,
                                               sRight,
                                               aRightLen );
    }
}
/**
 * @brief IS EQUAL 연산 ( NUMERIC TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualNumericToDouble( void       * aLeftVal,
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
 
        return dtlIsEqualNumericToDouble_INLINE( sLeft,
                                                 aLeftLen,
                                                 sRight,
                                                 aRightLen );
    }
}    

/**
 * @brief IS EQUAL 연산 ( NUMERIC TO NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualNumericToNumeric( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen )
{
    stlUInt8  * sLeftDigit;
    stlUInt8  * sRightDigit;
    stlInt32    i;

    STL_DASSERT( ( aLeftLen >= 0 ) && ( aLeftLen <= DTL_NUMERIC_MAX_SIZE ) );
    STL_DASSERT( ( aRightLen >= 0 ) && ( aRightLen <= DTL_NUMERIC_MAX_SIZE ) );

    if( ( aLeftLen != aRightLen ) || ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
     
    sLeftDigit  = (stlUInt8*) aLeftVal;
    sRightDigit = (stlUInt8*) aRightVal;

    if( *sLeftDigit == *sRightDigit )
    {
        if( aLeftLen == 1 )
        {
            return STL_TRUE;
        }

        aLeftLen--;
        for( i = 1 ; sLeftDigit[i] == sRightDigit[i] ; i++ )
        {
            if( i == aLeftLen )
            {
                return STL_TRUE;
            }
        }
    }
    
    return STL_FALSE;
}
        
/**
 * @brief IS EQUAL 연산 ( CHARACTER  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualFixedLengthChar( void       * aLeftVal,
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
        
        return dtlIsEqualFixedLengthChar_INLINE( sLeft,
                                                 aLeftLen,
                                                 sRight,
                                                 aRightLen );
    }
}

/**
 * @brief IS EQUAL 연산 ( CHARACTER  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualVariableLengthChar( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen )
{

    dtlVarcharType sLeft;
    dtlVarcharType sRight;
    
    if( ( aLeftLen != aRightLen ) || ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );
        
        return ( stlMemcmp( sLeft,
                            sRight,
                            aLeftLen ) == 0 );
    }
}

/**
 * @brief IS EQUAL 연산 ( CHAR vs. LONGVARCHAR  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualCharAndLongvarchar( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen )
{

    dtlVarcharType sLeft;
    dtlVarcharType sRight;
    
    if( ( aLeftLen != aRightLen ) || ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );
        
        return ( stlMemcmp( sLeft,
                            sRight,
                            aLeftLen ) == 0 );
    }
}

/**
 * @brief IS EQUAL 연산 ( LONGVARCHAR vs. CHAR  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualLongvarcharAndChar( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen )
{

    dtlVarcharType sLeft;
    dtlVarcharType sRight;
    
    if( ( aLeftLen != aRightLen ) || ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );
        
        return ( stlMemcmp( sLeft,
                            sRight,
                            aLeftLen ) == 0 );
    }
}

/**
 * @brief IS EQUAL 연산 ( VARCHAR vs. LONGVARCHAR  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualVarcharAndLongvarchar( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen )
{

    dtlVarcharType sLeft;
    dtlVarcharType sRight;
    
    if( ( aLeftLen != aRightLen ) || ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );
        
        return ( stlMemcmp( sLeft,
                            sRight,
                            aLeftLen ) == 0 );
    }
}

/**
 * @brief IS EQUAL 연산 ( LONGVARCHAR vs. VARCHAR  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualLongvarcharAndVarchar( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen )
{

    dtlVarcharType sLeft;
    dtlVarcharType sRight;
    
    if( ( aLeftLen != aRightLen ) || ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );
        
        return ( stlMemcmp( sLeft,
                            sRight,
                            aLeftLen ) == 0 );
    }
}

/**
 * @brief IS EQUAL 연산 ( LONGVARCHAR vs. LONGVARCHAR  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualLongvarcharAndLongvarchar( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen )
{

    dtlVarcharType sLeft;
    dtlVarcharType sRight;
    
    if( ( aLeftLen != aRightLen ) || ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );
        
        return ( stlMemcmp( sLeft,
                            sRight,
                            aLeftLen ) == 0 );
    }
}

/**
 * @brief IS EQUAL 연산 ( BINARY  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualFixedLengthBinary( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen )
{
    dtlBinaryType    sLeft;
    dtlBinaryType    sRight;

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_BINARY_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_BINARY_FROM_PTR( aRightVal, sRight );
        
        return dtlIsEqualFixedLengthBinary_INLINE( sLeft,
                                                   aLeftLen,
                                                   sRight,
                                                   aRightLen );
    }
}

/**
 * @brief IS EQUAL 연산 ( BINARY  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualVariableLengthBinary( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if( ( aLeftLen != aRightLen ) || ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else 
    {
        DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );
        
        return ( stlMemcmp( sLeft,
                            sRight,
                            aRightLen ) == 0 );
    }
}

/**
 * @brief IS EQUAL 연산 ( BINARY vs. LONGVARBINARY )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualBinaryAndLongvarbinary( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if( ( aLeftLen != aRightLen ) || ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else 
    {
        DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );
        
        return ( stlMemcmp( sLeft,
                            sRight,
                            aRightLen ) == 0 );
    }
}

/**
 * @brief IS EQUAL 연산 ( LONGVARBINARY vs. BINARY )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualLongvarbinaryAndBinary( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if( ( aLeftLen != aRightLen ) || ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else 
    {
        DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );
        
        return ( stlMemcmp( sLeft,
                            sRight,
                            aRightLen ) == 0 );
    }
}

/**
 * @brief IS EQUAL 연산 ( VARBINARY vs. LONGVARBINARY )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualVarbinaryAndLongvarbinary( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if( ( aLeftLen != aRightLen ) || ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else 
    {
        DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );
        
        return ( stlMemcmp( sLeft,
                            sRight,
                            aRightLen ) == 0 );
    }
}

/**
 * @brief IS EQUAL 연산 ( LONGVARBINARY vs. VARBINARY )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualLongvarbinaryAndVarbinary( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if( ( aLeftLen != aRightLen ) || ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else 
    {
        DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );
        
        return ( stlMemcmp( sLeft,
                            sRight,
                            aRightLen ) == 0 );
    }
}

/**
 * @brief IS EQUAL 연산 ( LONGVARBINARY vs. LONGVARBINARY )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualLongvarbinaryAndLongvarbinary( void       * aLeftVal,
                                                             stlInt64     aLeftLen,
                                                             void       * aRightVal,
                                                             stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if( ( aLeftLen != aRightLen ) || ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else 
    {
        DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );
        
        return ( stlMemcmp( sLeft,
                            sRight,
                            aRightLen ) == 0 );
    }
}

/**
 * @brief IS EQUAL 연산 ( DATE VS DATE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualDateToDate( void       * aLeftVal,
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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( DATE VS TIMESTAMP )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualDateToTimestamp( void       * aLeftVal,
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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( TIME VS TIME )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualTimeToTime( void       * aLeftVal,
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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( TIME WITH TIME ZONE VS TIME WITH TIME ZONE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualTimeTzToTimeTz( void       * aLeftVal,
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

        return ( sLeft.mTime == sRight.mTime );        
    }
}

/**
 * @brief IS EQUAL 연산 ( TIMESTAMP VS DATE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualTimestampToDate( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen )
{
    dtlTimestampType sLeft;
    dtlDateType      sRight;

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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( TIMESTAMP VS TIMESTAMP )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualTimestampToTimestamp( void       * aLeftVal,
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

        return ( sLeft == sRight );
    }
}

/**
 * @brief IS EQUAL 연산 ( TIMESTAMP WITH TIME ZONE VS TIMESTAMP WITH TIME ZONE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualTimestampTzToTimestampTz( void       * aLeftVal,
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

        return ( sLeft.mTimestamp == sRight.mTimestamp );
    }
}

/**
 * @brief IS EQUAL 연산 ( INTERVAL YEAR TO MONTH VS INTERVAL YEAR TO MONTH )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualIntervalYearToMonth( void       * aLeftVal,
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

        return ( sLeft.mMonth == sRight.mMonth );
    }
}

/**
 * @brief IS EQUAL 연산 ( INTERVAL DAY TO SECOND VS INTERVAL DAY TO SECOND )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualIntervalDayToSecond( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen )
{
    dtlIntervalDayToSecondType    sLeft;
    dtlIntervalDayToSecondType    sRight;
    dtlTimeOffset                 sLeftValSpan;
    dtlTimeOffset                 sRightValSpan;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_INTERVAL_DAY_TO_SECOND_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_INTERVAL_DAY_TO_SECOND_SIZE ) ); 
    
    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {        
        DTL_GET_INTERVAL_DAY_TO_SECOND_FROM_PTR( aLeftVal,  sLeft );
        DTL_GET_INTERVAL_DAY_TO_SECOND_FROM_PTR( aRightVal, sRight );

        sLeftValSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE( &sLeft );
        sRightValSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE( &sRight );

        return ( sLeftValSpan == sRightValSpan );
    }
}

/**
 * @brief IS EQUAL 연산 ( ROWID VS ROWID )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsEqualRowIdToRowId( void       * aLeftVal,
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
                           DTL_ROWID_SIZE ) == 0 );
    }
}



/**********************************************************************
 * LIST FUNCTION PHYSICAL FILTER
 **********************************************************************/ 

/**
 * @brief In Physical Func List To List
 *   <BR> List로 넘어오는 dtlDataValue에 대해 해당 Physical Func을 연결해준다.
 * @param[in]  aLeftVal     Left dtlDataValue List 
 * @param[in]  aLeftLen     Left Length
 * @param[in]  aRightVal    Right dtlDataValue List
 * @param[in]  aRightCount  Right Argument List Count
 * @return 연산 결과 
 */
stlBool dtlInPhysicalFuncListToList( void       * aLeftVal,
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
                  [ DTL_PHYSICAL_FUNC_IS_EQUAL ]
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


/**
 * @brief Equal All Physical Func List To List
 *   <BR> List로 넘어오는 dtlDataValue에 대해 해당 Physical Func을 연결해준다.
 * @param[in]  aLeftVal     Left dtlDataValue List 
 * @param[in]  aLeftLen     Left Length
 * @param[in]  aRightVal    Right dtlDataValue List
 * @param[in]  aRightCount  Right Argument List Count
 * @return 연산 결과 
 */
stlBool dtlEqualAllPhysicalFunc( void       * aLeftVal,
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
                  [ DTL_PHYSICAL_FUNC_IS_EQUAL ]
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



/** @} dtlPhysicalFunc */
