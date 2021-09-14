/*******************************************************************************
 * qlssSetTimeZone.h
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

#ifndef _QLSS_SET_TIME_ZONE_H_
#define _QLSS_SET_TIME_ZONE_H_ 1

/**
 * @file qlssSetTimeZone.h
 * @brief Session Control Language
 */


/**
 * @brief SET TIME ZONE Init Plan
 */
typedef struct qlssInitSetTimeZone
{
    stlInt32 mGMTOffset;  /**< GMT offset */
} qlssInitSetTimeZone;




stlStatus qlssValidateSetTimeZone( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv );

stlStatus qlssOptCodeSetTimeZone( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

stlStatus qlssOptDataSetTimeZone( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

stlStatus qlssExecuteSetTimeZone( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );


#endif /* _QLSS_SET_TIME_ZONE_H_ */

