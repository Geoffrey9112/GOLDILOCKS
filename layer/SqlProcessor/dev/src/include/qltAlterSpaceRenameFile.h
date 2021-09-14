/*******************************************************************************
 * qltAlterSpaceRenameFile.h
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

#ifndef _QLT_ALTER_SPACE_RENAME_FILE_H_
#define _QLT_ALTER_SPACE_RENAME_FILE_H_ 1

/**
 * @file qltAlterSpaceRenameFile.h
 * @brief ALTER TABLESPACE .. RENAME FILE
 */

/**
 * @brief ALTER TABLESPACE RENAME FILE 를 위한 Init Plan
 */
typedef struct qltInitAlterSpaceRenameFile
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle   mSpaceHandle;     /**< Tablespace Handle */

    smlDatafileAttr * mFromFileAttr;  /**< From File Attribute List */
    smlDatafileAttr * mToFileAttr;    /**< To File Attribute List */

    smlDatafileAttr * mFileAttr;  /**< From And To File Attribute List */
    stlInt32          mFileAttrLen;
} qltInitAlterSpaceRenameFile;

stlStatus qltValidateAlterSpaceRenameFile( smlTransId      aTransID,
                                           qllDBCStmt    * aDBCStmt,
                                           qllStatement  * aSQLStmt,
                                           stlChar       * aSQLString,
                                           qllNode       * aParseTree,
                                           qllEnv        * aEnv );


stlStatus qltOptCodeAlterSpaceRenameFile( smlTransId      aTransID,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          qllEnv        * aEnv );

stlStatus qltOptDataAlterSpaceRenameFile( smlTransId      aTransID,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          qllEnv        * aEnv );

stlStatus qltExecuteAlterSpaceRenameFile( smlTransId      aTransID,
                                          smlStatement  * aStmt,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          qllEnv        * aEnv );


#endif /* _QLT_ALTER_SPACE_RENAME_FILE_H_ */
