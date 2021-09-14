/*******************************************************************************
 * qlfFetch.h
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

#ifndef _QLFFETCH_H_
#define _QLFFETCH_H_ 1

/**
 * @file qlfFetch.h
 * @brief SQL Processor Layer Fetch 
 */

#include <qlDef.h>


/*******************************************************************************
 * SELECT
 ******************************************************************************/

/**
 * @defgroup qlfSelect Select Fetch
 * @ingroup qlf
 * @{
 */

stlStatus qlfFetchSelect( smlTransId      aTransID,
                          smlStatement  * aStmt,
                          qllDBCStmt    * aDBCStmt,
                          qllStatement  * aSQLStmt,
                          stlBool       * aEOF,
                          qllEnv        * aEnv );

stlBool qlfIsAlwaysFalseRange( knlCompareList * aMinRangeList,
                               knlCompareList * aMaxRangeList );

stlBool qlfHasFalseFilter( qlvInitExpression  * aFilterExpr,
                           qllDataArea        * aDataArea );

/** @} qlfSelect */


/*******************************************************************************
 * INSERT 
 ******************************************************************************/

/**
 * @defgroup qlfInsert Insert Fetch
 * @ingroup qlf
 * @{
 */

// stlStatus qlfFetchInsert( smlTransId      aTransID,
//                           smlStatement  * aStmt,
//                           qllStatement  * aSQLStmt,
//                           stlBool       * aEOF,
//                           qllEnv        * aEnv );

/** @} qlfInsert */


/*******************************************************************************
 * UPDATE 
 ******************************************************************************/

/**
 * @defgroup qlfUpdate Update Fetch
 * @ingroup qlf
 * @{
 */

// stlStatus qlfFetchUpdate( smlTransId      aTransID,
//                           smlStatement  * aStmt,
//                           qllStatement  * aSQLStmt,
//                           stlBool       * aEOF,
//                           qllEnv        * aEnv );

/** @} qlfUpdate */


/*******************************************************************************
 * DELETE 
 ******************************************************************************/

/**
 * @defgroup qlfDelete Delete Fetch
 * @ingroup qlf
 * @{
 */

// stlStatus qlfFetchDelete( smlTransId      aTransID,
//                           smlStatement  * aStmt,
//                           qllStatement  * aSQLStmt,
//                           stlBool       * aEOF,
//                           qllEnv        * aEnv );

/** @} qlfDelete */

#endif /* _QLFFETCH_H_ */
