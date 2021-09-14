/*******************************************************************************
 * dtlPhysicalFuncIsGreaterThan.c
 *
 * Copyright (c) 2012   unjesoft
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlPhysicalFuncIsGreaterThan.c 4494 2012-05-08 03:51:07Z ehpark $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file dtlPhysicalFuncIsGreaterThan.c
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
 * @brief IS GREATER THAN 연산 ( BOOLEAN VS BOOLEAN )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanBooleanToBoolean( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( SMALLINT TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanSmallIntToSmallInt( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( SMALLINT TO INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanSmallIntToInteger( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( SMALLINT TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanSmallIntToBigInt( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( SMALLINT TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanSmallIntToReal( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( SMALLINT TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanSmallIntToDouble( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( SMALLINT TO NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanSmallIntToNumeric( void       * aLeftVal,
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
        return  STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_SMALLINT_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NUMERIC_PTR_FROM_PTR( aRightVal, sRight );
        
        return dtlIsGreaterThanSmallIntToNumeric_INLINE( sLeft,
                                                         aLeftLen,
                                                         sRight,
                                                         aRightLen );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( INTEGER TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanIntegerToSmallInt( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen )
{
    dtlIntegerType   sLeft;
    dtlSmallIntType  sRight;
    
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( INTEGER VS INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanIntegerToInteger( void       * aLeftVal,
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

        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( INTEGER TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanIntegerToBigInt( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( INTEGER TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanIntegerToReal( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( INTEGER TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanIntegerToDouble( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( INTEGER VS NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanIntegerToNumeric( void       * aLeftVal,
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
        return  STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_INTEGER_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NUMERIC_PTR_FROM_PTR( aRightVal, sRight );
        
        return dtlIsGreaterThanIntegerToNumeric_INLINE( sLeft,
                                                        aLeftLen,
                                                        sRight,
                                                        aRightLen );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( BIGINT TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanBigIntToSmallInt( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( BIGINT TO INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanBigIntToInteger( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( BIGINT TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanBigIntToBigInt( void       * aLeftVal,
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

        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( BIGINT TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanBigIntToReal( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( BIGINT TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanBigIntToDouble( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( BIGINT TO NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanBigIntToNumeric( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen )
{
    dtlBigIntType       sLeft;
    dtlNumericType    * sRight;

    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_NATIVE_BIGINT_SIZE ) ); 
    STL_DASSERT( ( aRightLen >= 0 ) && ( aRightLen <= DTL_NUMERIC_MAX_SIZE ) ); 

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return  STL_FALSE;
    }
    else
    {
        DTL_GET_NATIVE_BIGINT_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NUMERIC_PTR_FROM_PTR( aRightVal, sRight );

        return dtlIsGreaterThanBigIntToNumeric_INLINE( sLeft,
                                                       aLeftLen,
                                                       sRight,
                                                       aRightLen );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( REAL TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanRealToSmallInt( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen )
{
    dtlRealType     sLeft;
    dtlSmallIntType sRight;
    
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( REAL TO INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanRealToInteger( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen )
{
    dtlRealType    sLeft;
    dtlIntegerType sRight;
    
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( REAL TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanRealToBigInt( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( RELA TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanRealToReal( void       * aLeftVal,
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

        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( REAL TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanRealToDouble( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( REAL TO NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanRealToNumeric( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen )
{
    dtlRealType         sLeft;
    dtlNumericType    * sRight;

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

        return dtlIsLessThanNumericToReal_INLINE( sRight,
                                                  aRightLen,
                                                  sLeft,
                                                  aLeftLen );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( DOUBLE TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanDoubleToSmallInt( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( DOUBLE TO INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanDoubleToInteger( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( DOUBLE TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanDoubleToBigInt( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( DOUBLE TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanDoubleToReal( void       * aLeftVal,
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
        
        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( DOUBLE TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanDoubleToDouble( void       * aLeftVal,
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

        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( DOUBLE TO NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanDoubleToNumeric( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen )
{
    dtlDoubleType       sLeft;
    dtlNumericType    * sRight;

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
 
        return dtlIsLessThanNumericToDouble_INLINE( sRight,
                                                    aRightLen,
                                                    sLeft,
                                                    aLeftLen );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( NUMERIC TO SMALLINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanNumericToSmallInt( void       * aLeftVal,
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
        return  STL_FALSE;
    }
    else
    {
        DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_SMALLINT_FROM_PTR( aRightVal, sRight );

        return dtlIsLessThanSmallIntToNumeric_INLINE( sRight,
                                                      aRightLen,
                                                      sLeft,
                                                      aLeftLen );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( NUMERIC TO INTEGER )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanNumericToInteger( void       * aLeftVal,
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
        return  STL_FALSE;
    }
    else
    {
        DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_INTEGER_FROM_PTR( aRightVal, sRight );

        return dtlIsLessThanIntegerToNumeric_INLINE( sRight,
                                                     aRightLen,
                                                     sLeft,
                                                     aLeftLen );        
    }
}

/**
 * @brief IS GREATER THAN 연산 ( NUMERIC TO BIGINT )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanNumericToBigInt( void       * aLeftVal,
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
        return  STL_FALSE;
    }
    else
    {
        DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_NATIVE_BIGINT_FROM_PTR( aRightVal, sRight );
        
        return dtlIsLessThanBigIntToNumeric_INLINE( sRight,
                                                    aRightLen,
                                                    sLeft,
                                                    aLeftLen );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( NUMERIC TO REAL )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanNumericToReal( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen )
{
    dtlNumericType    * sLeft;
    dtlRealType         sRight;

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

        return dtlIsGreaterThanNumericToReal_INLINE( sLeft,
                                                     aLeftLen,
                                                     sRight,
                                                     aRightLen );
    }    
}

/**
 * @brief IS GREATER THAN 연산 ( NUMERIC TO DOUBLE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanNumericToDouble( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen )
{
    dtlNumericType    * sLeft;
    dtlDoubleType       sRight;

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

        return dtlIsGreaterThanNumericToDouble_INLINE( sLeft,
                                                       aLeftLen,
                                                       sRight,
                                                       aRightLen );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( NUMERIC TO NUMERIC )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanNumericToNumeric( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen )
{
    stlUInt8  * sLeftDigit;
    stlUInt8  * sRightDigit;
    stlInt8   * sTable;
    stlInt32    i = 0;

    STL_DASSERT( ( aLeftLen >= 0 ) && ( aLeftLen <= DTL_NUMERIC_MAX_SIZE ) );
    STL_DASSERT( ( aRightLen >= 0 ) && ( aRightLen <= DTL_NUMERIC_MAX_SIZE ) );     

    if( ( aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
     
    sLeftDigit  = (stlUInt8*) aLeftVal;
    sRightDigit = (stlUInt8*) aRightVal;
     
    if( sLeftDigit[0] == sRightDigit[0] )
    {
        sTable = dtlNumericLenCompTable[ aLeftLen ][ aRightLen ];

        if( sTable[1] == 0 )
        {
            if( sLeftDigit[0] > 127 )
            {
                return ( sTable[0] == 1 );
            }
            else
            {
                return ( sTable[0] == -1 );
            }
        }

        for( i = 1 ; sLeftDigit[i] == sRightDigit[i] ; i++ )
        {
            if( i == sTable[1] )
            {
                if( sLeftDigit[0] > 127 )
                {
                    return ( sTable[0] == 1 );
                }
                else
                {
                    return ( sTable[0] == -1 );
                }
            }
        }
    }

    return ( sLeftDigit[i] > sRightDigit[i] );
}

/**
 * @brief IS GREATER THAN 연산 ( CHARACTER  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanFixedLengthChar( void       * aLeftVal,
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

        return dtlIsGreaterThanFixedLengthChar_INLINE( sLeft,
                                                       aLeftLen,
                                                       sRight,
                                                       aRightLen );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( CHARACTER  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanVariableLengthChar( void       * aLeftVal,
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
        DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );

        if( aLeftLen > aRightLen )
        {        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) >= 0 );
        }
        else
        {        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) > 0 );
        }
    }
}

/**
 * @brief IS GREATER THAN 연산 ( CHAR vs. LONGVARCHAR  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanCharAndLongvarchar( void       * aLeftVal,
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
        DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );

        if( aLeftLen > aRightLen )
        {        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) >= 0 );
        }
        else
        {        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) > 0 );
        }
    }
}

/**
 * @brief IS GREATER THAN 연산 ( LONGVARCHAR vs. CHAR  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanLongvarcharAndChar( void       * aLeftVal,
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
        DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );

        if( aLeftLen > aRightLen )
        {        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) >= 0 );
        }
        else
        {        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) > 0 );
        }
    }
}

/**
 * @brief IS GREATER THAN 연산 ( VARCHAR vs. LONGVARCHAR  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanVarcharAndLongvarchar( void       * aLeftVal,
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
        DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );

        if( aLeftLen > aRightLen )
        {        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) >= 0 );
        }
        else
        {        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) > 0 );
        }
    }
}

/**
 * @brief IS GREATER THAN 연산 ( LONGVARCHAR vs. VARCHAR  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanLongvarcharAndVarchar( void       * aLeftVal,
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
        DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );

        if( aLeftLen > aRightLen )
        {        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) >= 0 );
        }
        else
        {        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) > 0 );
        }
    }
}

/**
 * @brief IS GREATER THAN 연산 ( LONGVARCHAR vs. LONGVARCHAR  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanLongvarcharAndLongvarchar( void       * aLeftVal,
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
        DTL_GET_VARCHAR_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARCHAR_FROM_PTR( aRightVal, sRight );

        if( aLeftLen > aRightLen )
        {        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) >= 0 );
        }
        else
        {        
            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) > 0 );
        }
    }
}

/**
 * @brief IS GREATER THAN 연산 ( BINARY  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanFixedLengthBinary( void       * aLeftVal,
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
        
        return dtlIsGreaterThanFixedLengthBinary_INLINE( sLeft,
                                                         aLeftLen,
                                                         sRight,
                                                         aRightLen );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( BINARY  )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanVariableLengthBinary( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if( (aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );

        if( aLeftLen > aRightLen )
        {            
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) >= 0 );
        }
        else
        {
            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) > 0 );
        }
    }
}

/**
 * @brief IS GREATER THAN 연산 ( BINARY vs. LONGVARBINARY )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanBinaryAndLongvarbinary( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if( (aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );

        if( aLeftLen > aRightLen )
        {            
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) >= 0 );
        }
        else
        {
            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) > 0 );
        }
    }
}

/**
 * @brief IS GREATER THAN 연산 ( LONGVARBINARY vs. BINARY )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanLongvarbinaryAndBinary( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if( (aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );

        if( aLeftLen > aRightLen )
        {            
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) >= 0 );
        }
        else
        {
            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) > 0 );
        }
    }
}

/**
 * @brief IS GREATER THAN 연산 ( VARBINARY vs. LONGVARBINARY )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanVarbinaryAndLongvarbinary( void       * aLeftVal,
                                                               stlInt64     aLeftLen,
                                                               void       * aRightVal,
                                                               stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if( (aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );

        if( aLeftLen > aRightLen )
        {            
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) >= 0 );
        }
        else
        {
            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) > 0 );
        }
    }
}

/**
 * @brief IS GREATER THAN 연산 ( LONGVARBINARY vs. VARBINARY )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanLongvarbinaryAndVarbinary( void       * aLeftVal,
                                                               stlInt64     aLeftLen,
                                                               void       * aRightVal,
                                                               stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if( (aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );

        if( aLeftLen > aRightLen )
        {            
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) >= 0 );
        }
        else
        {
            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) > 0 );
        }
    }
}

/**
 * @brief IS GREATER THAN 연산 ( LONGVARBINARY vs. LONGVARBINARY )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanLongvarbinaryAndLongvarbinary( void       * aLeftVal,
                                                                   stlInt64     aLeftLen,
                                                                   void       * aRightVal,
                                                                   stlInt64     aRightLen )
{
    dtlVarbinaryType  sLeft;
    dtlVarbinaryType  sRight;
    
    if( (aLeftLen == 0 ) || ( aRightLen == 0 ) )
    {
        return STL_FALSE;
    }
    else
    {
        DTL_GET_VARBINARY_FROM_PTR( aLeftVal, sLeft );
        DTL_GET_VARBINARY_FROM_PTR( aRightVal, sRight );

        if( aLeftLen > aRightLen )
        {            
            return ( stlMemcmp( sLeft,
                                sRight,
                                aRightLen ) >= 0 );
        }
        else
        {
            return ( stlMemcmp( sLeft,
                                sRight,
                                aLeftLen ) > 0 );
        }
    }
}

/**
 * @brief IS GREATER THAN 연산 ( DATE VS DATE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanDateToDate( void       * aLeftVal,
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

        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( DATE VS TIMESTAMP )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanDateToTimestamp( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen )
{
    dtlDateType       sLeft;
    dtlTimestampType  sRight;

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

        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( TIME VS TIME )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanTimeToTime( void       * aLeftVal,
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

        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( TIME WITH TIME ZONE VS TIME WITH TIME ZONE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanTimeTzToTimeTz( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen )
{
    dtlTimeTzType    sLeft;
    dtlTimeTzType    sRight;

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

        return ( sLeft.mTime > sRight.mTime );                
    }
}

/**
 * @brief IS GREATER THAN 연산 ( TIMESTAMP VS DATE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanTimestampToDate( void       * aLeftVal,
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

        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( TIMESTAMP VS TIMESTAMP )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanTimestampToTimestamp( void       * aLeftVal,
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

        return ( sLeft > sRight );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( TIMESTAMP WITH TIME ZONE VS TIMESTAMP WITH TIME ZONE )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanTimestampTzToTimestampTz( void       * aLeftVal,
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

        return ( sLeft.mTimestamp > sRight.mTimestamp );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( INTERVAL YEAR TO MONTH VS INTERVAL YEAR TO MONTH )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanIntervalYearToMonth( void       * aLeftVal,
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

        return ( sLeft.mMonth > sRight.mMonth );
    }
}

/**
 * @brief IS GREATER THAN 연산 ( INTERVAL DAY TO SECOND VS INTERVAL DAY TO SECOND )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanIntervalDayToSecond( void       * aLeftVal,
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

        return ( sLeftValSpan > sRightValSpan );
    }   
}

/**
 * @brief IS GREATER THAN 연산 ( ROWID VS ROWID )
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsGreaterThanRowIdToRowId( void       * aLeftVal,
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
                           DTL_ROWID_SIZE ) > 0 );
    }
}


/**
 * @brief Greater Than Any Physical Func List To List
 *   <BR> List로 넘어오는 dtlDataValue에 대해 해당 Physical Func을 연결해준다.
 * @param[in]  aLeftVal     Left dtlDataValue List 
 * @param[in]  aLeftLen     Left Length
 * @param[in]  aRightVal    Right dtlDataValue List
 * @param[in]  aRightCount  Right Argument List Count
 * @return 연산 결과 
 */
stlBool dtlGreaterThanAnyPhysicalFunc( void       * aLeftVal,
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
                  [ DTL_PHYSICAL_FUNC_IS_GREATER_THAN ]
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
 * @brief Greater Than All Physical Func List To List
 *   <BR> List로 넘어오는 dtlDataValue에 대해 해당 Physical Func을 연결해준다.
 * @param[in]  aLeftVal     Left dtlDataValue List 
 * @param[in]  aLeftLen     Left Length
 * @param[in]  aRightVal    Right dtlDataValue List
 * @param[in]  aRightCount  Right Argument List Count
 * @return 연산 결과 
 */
stlBool dtlGreaterThanAllPhysicalFunc( void       * aLeftVal,
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
                  [ DTL_PHYSICAL_FUNC_IS_GREATER_THAN ]
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
