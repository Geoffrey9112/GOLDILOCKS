/*******************************************************************************
 * sslStatement.h
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


#ifndef _SSLSTATEMENT_H_
#define _SSLSTATEMENT_H_ 1

/**
 * @file sslStatement.h
 * @brief Session Statement Routines
 */

/*
 * X$SS_STMT_STAT
 */

stlStatus openFxSsStmtStatCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aKnlEnv );

stlStatus closeFxSsStmtStatCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aKnlEnv );

stlStatus buildRecordFxSsStmtStatCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aKnlEnv );
#endif /* _SSLSTATEMENT_H_ */
