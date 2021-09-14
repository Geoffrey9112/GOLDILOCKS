/*******************************************************************************
 * eldSqlConformance.h
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


#ifndef _ELD_SQL_CONFORMANCE_H_
#define _ELD_SQL_CONFORMANCE_H_ 1

/**
 * @file eldSqlConformance.h
 * @brief SQL CONFORMANCE Dictionary 관리 루틴 
 */


/**
 * @defgroup eldSqlConformance SQL CONFORMANCE Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertSqlConformanceDesc( smlTransId     aTransID,
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


/** @} eldSqlConformance */

#endif /* _ELD_SQL_CONFORMANCE_H_ */
