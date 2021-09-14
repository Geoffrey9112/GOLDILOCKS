/*******************************************************************************
 * qltDropSpace.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qltDropSpace.h 5803 2012-09-27 06:40:12Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLT_DROP_SPACE_H_
#define _QLT_DROP_SPACE_H_ 1

/**
 * @file qltDropSpace.h
 * @brief DROP TABLESPACE
 */

/**
 * @brief ALTER TABLESPACE RENAME TO 를 위한 Init Plan
 */
typedef struct qltInitDropSpace
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    stlBool          mIfExists;       /**< IF EXISTS 옵션 사용 여부 */
    stlBool          mSpaceExist;     /**< Space 존재 여부 */
    
    ellDictHandle    mSpaceHandle;    /**< Tablespace Handle */

    stlBool          mIsIncluding;    /**< INCLUDING CONTENTS */
    stlBool          mIsDropFiles;    /**< KEEP DATAFILES or AND DATAFILES */
    stlBool          mIsCascade;      /**< CASCADE CONSTRAINTS */
} qltInitDropSpace;




stlStatus qltValidateDropSpace( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv );


stlStatus qltOptCodeDropSpace( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qltOptDataDropSpace( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qltExecuteDropSpace( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

/*
 * Execution 보조 함수
 */


#endif /* _QLT_DROP_SPACE_H_ */
