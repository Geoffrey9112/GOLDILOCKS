/*******************************************************************************
 * qllSession.h
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

#ifndef _QLL_SESSION_H_
#define _QLL_SESSION_H_ 1

/**
 * @file qllSession.h
 * @brief SQL Session 관리 
 */

stlStatus qllLogin( stlChar * aUserName,
                    stlChar * aEncryptedPasswd,
                    stlChar * aRoleName,
                    stlChar * aPlainOrgPassword,
                    stlChar * aPlainNewPassword,
                    qllEnv  * aEnv );


stlStatus qllCommit( smlTransId     aTransID,
                     stlChar      * aComment,
                     stlInt64       aWriteMode,
                     knlXaContext * aXaContext,
                     qllEnv       * aEnv );

stlStatus qllRollback( smlTransId     aTransID,
                       knlXaContext * aXaContext,
                       qllEnv       * aEnv );

stlStatus qllXaAssociate( knlXaContext * aContext,
                          qllEnv       * aEnv );

stlStatus qllXaDissociate( knlXaContext * aContext,
                           qllEnv       * aEnv );

void qllClearXaDeferContext( knlXaContext * aContext,
                             qllEnv       * aEnv );

stlStatus qllPrepare( stlXid       * aXid,
                      smlTransId     aTransID,
                      stlInt64       aWriteMode,
                      knlXaContext * aContext,
                      qllEnv       * aEnv );

inline void          qllSetTransAccessMode( qllAccessMode   aAccessMode,
                                            qllEnv        * aEnv );
inline qllAccessMode qllGetTransAccessMode( qllEnv * aEnv );

inline void qllSetTransUniqueMode( qllUniqueMode   aUniqueMode,
                                   qllEnv        * aEnv );
inline qllUniqueMode qllGetTransUniqueMode( qllEnv * aEnv );


stlStatus qllFiniSQLSessionObject( qllEnv  * aEnv );

stlStatus qllCheckLoginAuthentication( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       ellDictHandle * aUserHandle,
                                       stlChar       * aEncryptPassword,
                                       stlChar       * aPlainOrgPassword,
                                       stlChar       * aPlainNewPassword,
                                       stlBool       * aVersionConflict,
                                       stlBool       * aHasModifyError,
                                       qllEnv        * aEnv );

#endif /* _QLL_SESSION_H_ */
