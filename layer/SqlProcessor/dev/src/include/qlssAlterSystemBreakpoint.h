/*******************************************************************************
 * qlssAlterSystemBreakpoint.h
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

#ifndef _QLSS_ALTER_SYSTEM_BREAKPOINT_H_
#define _QLSS_ALTER_SYSTEM_BREAKPOINT_H_ 1

/**
 * @file qlssAlterSystemBreakpoint.h
 * @brief System Control Language
 */


/**
 * @brief ALTER SYSTEM BREAKPOINT Init Plan
 */
typedef struct qlssInitSystemBreakpoint
{
    stlBool                  mIsRegister;        /**< REGISTER/CLEAR */

    knlBreakPointId          mBreakpointId;      /**< breakpoint ID */
    knlBreakPointSessType    mSessType;          /**< breakpoint session type */
    stlInt32                 mOwnerSessId;       /**< owner session ID */
    stlInt32                 mSkipCount;         /**< skip count */
    knlBreakPointAction      mActionType;        /**< action type */
    knlBreakPointPostAction  mPostActionType;    /**< post action type */
    stlInt32                 mArgument;          /**< breakpoint action argument */
} qlssInitSystemBreakpoint;



stlStatus qlssValidateSystemBreakpoint( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv );

stlStatus qlssOptCodeSystemBreakpoint( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qlssOptDataSystemBreakpoint( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qlssExecuteSystemBreakpoint( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

#endif /* _QLSS_ALTER_SYSTEM_BREAKPOINT_H_ */

