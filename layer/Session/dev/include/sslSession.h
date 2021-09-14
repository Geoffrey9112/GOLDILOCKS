/*******************************************************************************
 * sslSession.h
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


#ifndef _SSLSESSION_H_
#define _SSLSESSION_H_ 1

/**
 * @file sslSession.h
 * @brief Session Layer Session Routines
 */

stlStatus sslAllocSession( stlChar        * aRoleName,
                           sslSessionEnv ** aSessionEnv,
                           sslEnv         * aEnv );

stlStatus sslFreeSession( sslSessionEnv * aSessionEnv,
                          sslEnv        * aEnv );

stlStatus sslInitSessionEnv( sslSessionEnv     * aSessEnv,
                             knlConnectionType   aConnectionType,
                             knlSessionType      aSessType,
                             knlSessEnvType      aSessEnvType,
                             sslEnv            * aEnv );

stlStatus sslInitSessionStmtSlot(  sslSessionEnv * aSessEnv,
                                   sslEnv        * aEnv );

stlStatus sslFiniSessionEnv( sslSessionEnv * aSessEnv,
                             sslEnv        * aEnv );

stlStatus sslCleanupSessionEnv( sslSessionEnv * aSessEnv,
                                sslEnv        * aEnv );

stlStatus sslAuthenticateSession( stlChar * aUserName,
                                  stlChar * aEncryptedPasswd,
                                  sslEnv  * aEnv );

/*
 * NLS
 */
stlStatus sslSetNLS( dtlCharacterSet   aCharacterSet,
                     stlInt32          aTimezone,
                     sslEnv          * aEnv );


/*
 * Transaction
 */
stlStatus sslSetTransactionMode( sslSessionEnv * aSessionEnv,
                                 sslEnv        * aEnv );

/*
 * X$SESSION
 */

stlStatus openFxSessionCallback( void   * aStmt,
                                 void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv );

stlStatus closeFxSessionCallback( void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv );

stlStatus buildRecordFxSessionCallback( void              * aDumpObjHandle,
                                        void              * aPathCtrl,
                                        knlValueBlockList * aValueList,
                                        stlInt32            aBlockIdx,
                                        stlBool           * aTupleExist,
                                        knlEnv            * aEnv );

/*
 * X$SS_SESS_STAT
 */

stlStatus openFxSsSessStatCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aKnlEnv );

stlStatus closeFxSsSessStatCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aKnlEnv );

stlStatus buildRecordFxSsSessStatCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aKnlEnv );
#endif /* _SSLSESSION_H_ */
