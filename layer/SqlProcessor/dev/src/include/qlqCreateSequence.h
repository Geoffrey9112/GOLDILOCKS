/*******************************************************************************
 * qlqCreateSequence.h
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

#ifndef _QLQ_CREATE_SEQUENCE_H_
#define _QLQ_CREATE_SEQUENCE_H_ 1

/**
 * @file qlqCreateSequence.h
 * @brief CREATE SEQUENCE
 */

/**
 * @brief CREATE SEQUENCE 을 위한 Init Plan
 */
typedef struct qlqInitCreateSeq
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle          mSchemaHandle;    /**< Schema Handle */
    stlChar              * mSeqName;         /**< Sequence Name */
    
    smlSequenceAttr        mSeqAttr;         /**< Sequence Attribute */
} qlqInitCreateSeq;
    


stlStatus qlqValidateCreateSeq( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv );

stlStatus qlqOptCodeCreateSeq( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qlqOptDataCreateSeq( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qlqExecuteCreateSeq( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

/*
 * Validation 보조 함수 
 */


stlStatus qlqValidateSeqGenOption( stlChar           * aSQLString,
                                   smlSequenceAttr   * aSeqAttr,
                                   qlpSequenceOption * aSeqOption,
                                   qllEnv            * aEnv );
                                   
    
stlStatus qlqValidateSeqOptRelation( stlChar           * aSQLString,
                                     smlSequenceAttr   * aSeqAttr,
                                     qlpSequenceOption * aSeqOption,
                                     qllEnv            * aEnv );

#endif /* _QLQ_CREATE_SEQUENCE_H_ */
