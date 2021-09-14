/*******************************************************************************
 * ellDictSqlConformance.h
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


#ifndef _ELL_DICT_SQL_CONFORMANCE_H_
#define _ELL_DICT_SQL_CONFORMANCE_H_ 1

/**
 * @file ellDictSqlConformance.h
 * @brief SqlConformance Dictionary Management
 */

/**
 * @addtogroup ellSqlConformance
 * @{
 */


stlStatus ellInsertSqlConformanceDesc( smlTransId     aTransID,
                                       smlStatement * aStmt,
                                       stlChar      * aType,
                                       stlChar      * aID,
                                       stlChar      * aSubID,
                                       stlChar      * aName,
                                       stlChar      * aSubName,
                                       stlBool        aIsSupported,
                                       stlBool        aIsMandatory,
                                       stlChar      * aIsVerifiedBy,
                                       stlChar      * aComments,
                                       ellEnv       * aEnv );

/** @} ellSqlConformance */



#endif /* _ELL_DICT_SQL_CONFORMANCE_H_ */
