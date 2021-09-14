/*******************************************************************************
 * qllDBCStmt.h
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

#ifndef _QLL_DBC_STMT_H_
#define _QLL_DBC_STMT_H_ 1

/**
 * @file qllDBCStmt.h
 * @brief SQL Processor Layer DBC Statement Definition
 */

stlStatus qllInitDBC( qllDBCStmt     * aDBCStmt,
                      qllEnv         * aEnv );

stlStatus qllFiniDBC( qllDBCStmt   * aDBCStmt,
                      qllEnv       * aEnv );

void qllPlanCacheMemON( qllDBCStmt * aDBCStmt );

void qllPlanCacheMemOFF( qllDBCStmt * aDBCStmt );

stlStatus qllAddGrantedPriv( qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             ellPrivObject   aPrivObject,
                             ellDictHandle * aPrivHandle,
                             qllEnv        * aEnv );

    
#endif /* _QLL_DBC_STMT_H_ */
