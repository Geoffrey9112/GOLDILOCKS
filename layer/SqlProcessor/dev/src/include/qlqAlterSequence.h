/*******************************************************************************
 * qlqAlterSequence.h
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

#ifndef _QLQ_ALTER_SEQUENCE_H_
#define _QLQ_ALTER_SEQUENCE_H_ 1

/**
 * @file qlqAlterSequence.h
 * @brief ALTER SEQUENCE
 */

/**
 * @brief ALTER SEQUENCE 를 위한 Init Plan
 */
typedef struct qlqInitAlterSeq
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle    mSchemaHandle;     /**< Schema Handle */
    ellDictHandle    mSeqHandle;        /**< Sequence Handle */

    smlSequenceAttr  mSeqAttr;          /**< Sequence Attribute */
    
} qlqInitAlterSeq;



stlStatus qlqValidateAlterSeq( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               stlChar       * aSQLString,
                               qllNode       * aParseTree,
                               qllEnv        * aEnv );

stlStatus qlqOptCodeAlterSeq( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );

stlStatus qlqOptDataAlterSeq( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );

stlStatus qlqExecuteAlterSeq( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );

/*
 * Validation 보조 함수 
 */

stlStatus qlqValidateAlterSeqOption( stlChar           * aSQLString,
                                     void              * aSeqPhyHandle,
                                     smlSequenceAttr   * aSeqAttr,
                                     qlpSequenceOption * aSeqOption,
                                     qllEnv            * aEnv );


/*
 * Execution 보조 함수 
 */



#endif /* _QLQ_ALTER_SEQUENCE_H_ */
