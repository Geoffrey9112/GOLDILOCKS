/****************************************
 * stlMath.h
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
 ***************************************/


#ifndef _STLMATH_H_
#define _STLMATH_H_ 1

/**
 * @file stlMath.h
 * @brief Standard Layer MATH
 */

#include <stlDef.h>
#include <stlTypes.h>

STL_BEGIN_DECLS


/**
 * @defgroup stlMath MATH Routines
 * @ingroup STL 
 * @{
 */

#define STL_IS_FINITE_FALSE     (0)     /* for stlIsfinite */
#define STL_IS_INFINITE_FALSE   (0)     /* for stlIsinfinite */
#define STL_IS_NAN_FALSE        (0)     /* for stlIsnan */
#define STL_RAND_MAX            (RAND_MAX)

/** @see isfinite */
#define stlIsfinite(value)              (isfinite(value))

/** @see isinf */
#define stlIsinfinite(value)            (isinf(value))

/** @see isinf */
#define stlIsnan(value)                 (isnan(value))

/** @see fmod */
#define stlFmod( aValue1, aValue2 )     ( fmod( aValue1, aValue2 ) )

/** @see modf */
#define stlModf( aValue1, aValue2 )     ( modf( aValue1, aValue2 ) )

/** @see modff */
#define stlModff( aValue1, aValue2 )    ( modff( aValue1, aValue2 ) )

/** @see atol */
#define stlAtol( aValue )               ( atol( aValue ) )

/** @see atof */
#define stlAtof( aValue )               ( atof( aValue ) )

/** @see sqrt */
#define stlSqrt( aValue )               ( sqrt( aValue ) )

/** @see cbrt */
#define stlCbrt( aValue )               ( cbrt( aValue ) )

/** @see ceil */
#define stlCeil( aValue )               ( ceil( aValue ) )

/** @see exp */
#define stlExp( aValue )                ( exp( aValue ) )

/** @see exp */
#define stlExp2( aValue )               ( exp2( aValue ) )

/** @see floor */
#define stlFloor( aValue )              ( floor( aValue ) )

/** @see floor */
#define stlFloorf( aValue )             ( floorf( aValue ) )

/** @see trunc */
#define stlTrunc( aValue )              ( trunc( aValue ) )

/** @see log */
#define stlLog( aValue )                ( log( aValue ) )

/** @see log */
#define stlLog2( aValue )               ( log2( aValue ) )

/** @see log10 */
#define stlLog10( aValue )              ( log10( aValue ) )

/** @see log10f */
#define stlLog10f( aValue )             ( log10f( aValue ) )

/** @see M_PI */
#define stlPi()                         ( M_PI )

/** @see random */
#define stlRandom()                     ( random() )

#ifdef WIN32
/** @see srandom */
#define stlSrandom( aValue )            ( srand( aValue ) )
#else
/** @see srandom */
#define stlSrandom( aValue )            ( srandom( aValue ) )
#endif

/** @see acos */
#define stlAcos( aValue )               ( acos( aValue ) )

/** @see asin */
#define stlAsin( aValue )               ( asin( aValue ) )

/** @see atan */
#define stlAtan( aValue )               ( atan( aValue ) )

/** @see atan2 */
#define stlAtan2( aValue1, aValue2 )    ( atan2( aValue1, aValue2 ) )

/** @see cos */
#define stlCos( aValue )                ( cos( aValue ) )

/** @see tan */
#define stlTan( aValue )                ( tan( aValue ) )

/** @see sin */
#define stlSin( aValue )                ( sin( aValue ) )




/**
 * @brief 32비트 정수형의 절대값을 얻는다.
 * @param[in] aValue Target Value
 */
#define  stlAbsInt32( aValue )           ( abs( aValue ) )
stlInt64 stlAbsInt64( stlInt64 aValue );

/**
 * @brief double형의 절대값을 얻는다.
 * @param[in] aValue Target Value
 */
#define stlAbsDouble( aValue )          ( fabs( aValue ) )

/**
 * @brief 가장 가까운 정수로 라운딩하는 함수
 * @param[in] aValue stlFloat64(double)의 value
 * @return           stlFloat64(double)의 라운딩된 정수 
 */
#define stlRint( aValue ) ( rint(aValue) ) 

/**
 * @brief 가장 가까운 정수로 라운딩하는 함수
 * @param[in] aValue stlFloat32(float)의 value
 * @return           stlFloat32(float)의 라운딩된 정수 
 */
#define stlRintf( aValue ) ( rintf(aValue) ) 

/* stlPow */
stlStatus stlPow( stlFloat64       aValue,
                  stlFloat64       aPow,
                  stlFloat64     * aResult,
                  stlErrorStack  * aErrorStack );

/* pow연산과 관련한 함수들 */
stlBool stlGet10Pow( stlInt32        aValue,
                     stlFloat64    * aResult,
                     stlErrorStack * aErrorStack );

extern const stlFloat64 * const gPredefined10PowValue;

#define STL_POW_10( aValue ) ( gPredefined10PowValue[(aValue)] )

/** @} */
    
STL_END_DECLS

#endif /* _STLMATH_H_ */
