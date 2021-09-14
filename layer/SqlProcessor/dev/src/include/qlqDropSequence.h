/*******************************************************************************
 * qlqDropSequence.h
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

#ifndef _QLQ_DROP_SEQUENCE_H_
#define _QLQ_DROP_SEQUENCE_H_ 1

/**
 * @file qlqDropSequence.h
 * @brief DROP SEQUENCE
 */

/**
 * @brief DROP SEQUENCE 를 위한 Init Plan
 */
typedef struct qlqInitDropSeq
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    stlBool          mIfExists;         /**< IF EXISTS 옵션 사용 여부 */
    stlBool          mSeqExist;         /**< Sequence 존재 여부 */
    
    ellDictHandle    mSchemaHandle;     /**< Schema Handle */
    ellDictHandle    mSeqHandle;        /**< Sequence Handle */
} qlqInitDropSeq;



stlStatus qlqValidateDropSeq( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              stlChar       * aSQLString,
                              qllNode       * aParseTree,
                              qllEnv        * aEnv );

stlStatus qlqOptCodeDropSeq( smlTransId      aTransID,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             qllEnv        * aEnv );

stlStatus qlqOptDataDropSeq( smlTransId      aTransID,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             qllEnv        * aEnv );

stlStatus qlqExecuteDropSeq( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             qllEnv        * aEnv );

stlStatus qlqDropSequence4SeqList( smlStatement  * aStmt,
                                   ellObjectList * aSeqList,
                                   qllEnv        * aEnv );


#endif /* _QLQ_DROP_SEQUENCE_H_ */
