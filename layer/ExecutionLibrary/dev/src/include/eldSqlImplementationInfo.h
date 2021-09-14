/*******************************************************************************
 * eldSqlImplementationInfo.h
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


#ifndef _ELD_SQL_IMPLEMENTATION_INFO_H_
#define _ELD_SQL_IMPLEMENTATION_INFO_H_ 1

/**
 * @file eldSqlImplementationInfo.h
 * @brief SQL IMPLEMENTATION INFO Dictionary 관리 루틴 
 */


/**
 * @defgroup eldSqlImplementationInfo SQL IMPLEMENTATION INFO Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertSqlImplementationInfoDesc( smlTransId     aTransID,
                                              smlStatement * aStmt,
                                              stlChar      * aID,
                                              stlChar      * aName,
                                              stlInt32       aIntegerValue,
                                              stlChar      * aCharacterValue,
                                              stlChar      * aComments,
                                              ellEnv       * aEnv );


/** @} eldSqlImplementationInfo */

#endif /* _ELD_SQL_IMPLEMENTATION_INFO_H_ */
