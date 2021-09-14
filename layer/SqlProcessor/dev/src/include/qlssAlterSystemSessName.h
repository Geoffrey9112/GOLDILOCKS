/*******************************************************************************
 * qlssAlterSystemSessName.h
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

#ifndef _QLSS_ALTER_SYSTEM_SESS_NAME_H_
#define _QLSS_ALTER_SYSTEM_SESS_NAME_H_ 1

/**
 * @file qlssAlterSystemSessName.h
 * @brief System Control Language
 */


/**
 * @brief ALTER SYSTEM SESSION NAME Init Plan
 */
typedef struct qlssInitSystemSessName
{
    stlBool         mIsRegister;                                /**< REGISTER/CLEAR */

    stlChar         mSessName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];   /**< Session Name */
} qlssInitSystemSessName;



stlStatus qlssValidateSystemSessName( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      stlChar       * aSQLString,
                                      qllNode       * aParseTree,
                                      qllEnv        * aEnv );

stlStatus qlssOptCodeSystemSessName( smlTransId      aTransID,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv );

stlStatus qlssOptDataSystemSessName( smlTransId      aTransID,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv );

stlStatus qlssExecuteSystemSessName( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv );

#endif /* _QLSS_ALTER_SYSTEM_SESS_NAME_H_ */

