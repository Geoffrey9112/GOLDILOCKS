/*******************************************************************************
 * ellFilter.h
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

#ifndef _ELL_FILTER_H_
#define _ELL_FILTER_H_ 1

/**
 * @file ellFilter.h
 * @brief SCAN & FILTER 관리 모듈
 */

/**
 * @defgroup ellFilter  SCAN / FILTER 
 * @ingroup elExternal
 * @{
 */



/*
 * GLIESE 예상 API :
 *   make_filter( & filter, FUNC_ID,
 *                type_col, db_type_char, col_order, type_value, db_value );  => va_list part
 */

stlStatus ellMakeOneFilter( knlBuiltInFunc         aFuncID,
                            knlValueBlockList    * aArgList,
                            knlPredicateList    ** aFilterPredList,
                            knlExprContextInfo  ** aFilterContext,
                            knlRegionMem         * aMemMgr,  
                            ellEnv               * aEnv );

stlStatus ellGetKeyCompareList( ellDictHandle       * aIndexHandle,
                                knlRegionMem        * aRegionMem,
                                knlKeyCompareList  ** aKeyCompList,
                                ellEnv              * aEnv );


/*******************************************************************/

// stlStatus ellMakeFilter( knlBuiltInFunc         aFuncID,
//                          knlValueBlockList    * aArgList,
//                          knlPredicateList    ** aFilterPredList,
//                          knlRegionMem         * aMemMgr,  
//                          ellEnv               * aEnv );

// stlStatus ellMakeMinMaxRange( stlBool                aIsAscOrder,
//                               stlBool                aIsNullsFirst,
//                               knlValueBlockList    * aColValue,
//                               knlRangeValueType      aValueTypeMin,
//                               knlValueBlockList    * aDataValueMin,
//                               stlBool                aInculdeEqualMin,
//                               knlRangeValueType      aValueTypeMax,
//                               knlValueBlockList    * aDataValueMax,
//                               stlBool                aInculdeEqualMax,
//                               knlPredicateList    ** aRangePredList,
//                               knlRegionMem         * aMemMgr,
//                               ellEnv               * aEnv );

// stlStatus ellMakeEqualRange( stlBool               aIsAscOrder,
//                              stlBool               aIsNullsFirst,
//                              knlValueBlockList   * aColValue,
//                              knlValueBlockList   * aDataValue,
//                              knlPredicateList   ** aRangePredList,
//                              knlRegionMem        * aMemMgr,
//                              ellEnv              * aEnv );


/** @} ellFilter */

#endif /* _ELL_FILTER_H_ */
