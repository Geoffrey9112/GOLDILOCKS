/*******************************************************************************
 * knlAggregation.h
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


#ifndef _KNL_AGGREGATION_H_
#define _KNL_AGGREGATION_H_ 1

/**
 * @file knlAggregation.h
 * @brief Kernel Layer Aggregation Routines
 */

/**
 * @addtogroup knlAggregation
 * @{
 */


#define KNL_INTERVAL_MIN_MONTH  ( -12000000 )
#define KNL_INTERVAL_MAX_MONTH  ( 12000000 )

#define KNL_INTERVAL_MIN_DAY    ( -1000000 )
#define KNL_INTERVAL_MAX_DAY    ( 1000000 )

#define KNL_DIFFSIGN( aA, aB )    ( ( (aA) < 0 ) != ( (aB) < 0 ) )

#define KNL_SAMESIGN( aA, aB )    ( ( (aA) < 0 ) == ( (aB) < 0 ) )
#define KNL_DIFFSIGN( aA, aB )    ( ( (aA) < 0 ) != ( (aB) < 0 ) )

#define KNL_GET_NUMERIC_TYPE( aValue ) ( (dtlNumericType *)(aValue)->mValue )


/*****************************************************
 * PRIMITIVE FUNCTIONS FOR AGGREGATION
 ****************************************************/

stlStatus knlGetNumericValue( dtlDataValue  * aSrcValue,
                              dtlDataValue  * aResult,
                              knlEnv        * aEnv );

stlStatus knlAddIntegerValueToNumericValue( stlInt64        aIntValue,
                                            dtlDataValue  * aDestValue,
                                            stlInt64        aPrecision,
                                            knlEnv        * aEnv );


/* for Aggregation Function By Data Value (for SMO Aggregation) */
stlStatus knlAddValueSmallIntToBigInt( dtlDataValue  * aSrcValue,
                                       dtlDataValue  * aDestValue,
                                       knlEnv        * aEnv );

stlStatus knlAddValueIntegerToBigInt( dtlDataValue  * aSrcValue,
                                      dtlDataValue  * aDestValue,
                                      knlEnv        * aEnv );

stlStatus knlAddValueBigIntToBigInt( dtlDataValue  * aSrcValue,
                                     dtlDataValue  * aDestValue,
                                     knlEnv        * aEnv );

stlStatus knlAddValueRealToDouble( dtlDataValue  * aSrcValue,
                                   dtlDataValue  * aDestValue,
                                   knlEnv        * aEnv );

stlStatus knlAddValueDoubleToDouble( dtlDataValue  * aSrcValue,
                                     dtlDataValue  * aDestValue,
                                     knlEnv        * aEnv );

stlStatus knlAddValueNumericToNumeric( dtlDataValue  * aSrcValue,
                                       dtlDataValue  * aDestValue,
                                       knlEnv        * aEnv );

stlStatus knlAddValueStringToNumeric( dtlDataValue  * aSrcValue,
                                      dtlDataValue  * aDestValue,
                                      knlEnv        * aEnv );

stlStatus knlAddValueYearToMonth( dtlDataValue  * aSrcValue,
                                  dtlDataValue  * aDestValue,
                                  knlEnv        * aEnv );

stlStatus knlAddValueDayToSecond( dtlDataValue  * aSrcValue,
                                  dtlDataValue  * aDestValue,
                                  knlEnv        * aEnv );


/* for Aggregation Function */
stlStatus knlAddSmallIntToBigInt( knlValueBlockList  * aSrcValue,
                                  knlValueBlockList  * aDestValue,
                                  stlInt32             aStartBlockIdx,
                                  stlInt32             aBlockCnt,
                                  stlInt32             aResultBlockIdx,
                                  knlEnv             * aEnv );

stlStatus knlAddIntegerToBigInt( knlValueBlockList  * aSrcValue,
                                 knlValueBlockList  * aDestValue,
                                 stlInt32             aStartBlockIdx,
                                 stlInt32             aBlockCnt,
                                 stlInt32             aResultBlockIdx,
                                 knlEnv             * aEnv );

stlStatus knlAddBigIntToBigInt( knlValueBlockList  * aSrcValue,
                                knlValueBlockList  * aDestValue,
                                stlInt32             aStartBlockIdx,
                                stlInt32             aBlockCnt,
                                stlInt32             aResultBlockIdx,
                                knlEnv             * aEnv );

stlStatus knlAddRealToDouble( knlValueBlockList  * aSrcValue,
                              knlValueBlockList  * aDestValue,
                              stlInt32             aStartBlockIdx,
                              stlInt32             aBlockCnt,
                              stlInt32             aResultBlockIdx,
                              knlEnv             * aEnv );

stlStatus knlAddDoubleToDouble( knlValueBlockList  * aSrcValue,
                                knlValueBlockList  * aDestValue,
                                stlInt32             aStartBlockIdx,
                                stlInt32             aBlockCnt,
                                stlInt32             aResultBlockIdx,
                                knlEnv             * aEnv );

stlStatus knlAddNumericToNumeric( knlValueBlockList  * aSrcValue,
                                  knlValueBlockList  * aDestValue,
                                  stlInt32             aStartBlockIdx,
                                  stlInt32             aBlockCnt,
                                  stlInt32             aResultBlockIdx,
                                  knlEnv             * aEnv );

stlStatus knlAddStringToNumeric( knlValueBlockList  * aSrcValue,
                                 knlValueBlockList  * aDestValue,
                                 stlInt32             aStartBlockIdx,
                                 stlInt32             aBlockCnt,
                                 stlInt32             aResultBlockIdx,
                                 knlEnv             * aEnv );

stlStatus knlAddYearToMonth( knlValueBlockList  * aSrcValue,
                             knlValueBlockList  * aDestValue,
                             stlInt32             aStartBlockIdx,
                             stlInt32             aBlockCnt,
                             stlInt32             aResultBlockIdx,
                             knlEnv             * aEnv );

stlStatus knlAddDayToSecond( knlValueBlockList  * aSrcValue,
                             knlValueBlockList  * aDestValue,
                             stlInt32             aStartBlockIdx,
                             stlInt32             aBlockCnt,
                             stlInt32             aResultBlockIdx,
                             knlEnv             * aEnv );


stlStatus knlAddNumericForInteger( knlValueBlockList  * aSrcValueList,
                                   dtlDataValue       * aDestValue,
                                   stlInt32             aStartIdx,
                                   stlInt32             aEndIdx,
                                   knlEnv             * aEnv );

/** @} */

#endif /* _KNL_AGGREGATION_H_ */
