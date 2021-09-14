/*******************************************************************************
 * qltAlterSpaceAddFile.h
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

#ifndef _QLT_ALTER_SPACE_ADD_FILE_H_
#define _QLT_ALTER_SPACE_ADD_FILE_H_ 1

/**
 * @file qltAlterSpaceAddFile.h
 * @brief ALTER TABLESPACE .. ADD FILE
 */

/**
 * @brief ALTER TABLESPACE ADD FILE 를 위한 Init Plan
 */
typedef struct qltInitAlterSpaceAddFile
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle   mSpaceHandle;     /**< Tablespace Handle */

    smlDatafileAttr * mFileAttr;      /**< File Attribute List */
    stlInt32          mFileAttrLen;   /**< File Attribute List Length */
} qltInitAlterSpaceAddFile;

stlStatus qltValidateAlterSpaceAddFile( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv );


stlStatus qltOptCodeAlterSpaceAddFile( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qltOptDataAlterSpaceAddFile( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qltExecuteAlterSpaceAddFile( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );


stlStatus qltSetFileAttr( stlChar           * aSQLString,
                          qlpDatafile       * aParseFile,
                          qlpSpaceMediaType   aSpaceMediaType,
                          smlDatafileAttr   * aFileAttr,
                          qllEnv            * aEnv );


#endif /* _QLT_ALTER_SPACE_ADD_FILE_H_ */
