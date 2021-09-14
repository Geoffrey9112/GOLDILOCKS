/*******************************************************************************
 * qlrAlterTableDropColumn.h
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

#ifndef _QLR_ALTER_TABLE_DROP_COLUMN_H_
#define _QLR_ALTER_TABLE_DROP_COLUMN_H_ 1

/**
 * @file qlrAlterTableDropColumn.h
 * @brief ALTER TABLE .. DROP COLUMN
 */

/**
 * @brief DROP COLUMN 을 위한 Init Plan
 */
typedef struct qlrInitDropColumn
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle         mSchemaHandle;     /**< Schema Handle */
    ellDictHandle         mTableHandle;      /**< Table Handle */

    ellObjectList       * mColumnList;       /**< Drop Column List */
    
    /*
     * Total Column Information
     */
    
    stlInt32              mTableColCnt;      /**< Table Column Count */
    ellDictHandle       * mTableColHandle;   /**< Table Total Column Handle */
    stlBool             * mUnusedFlag;       /**< Unused Column 여부 */
    stlBool             * mDropFlag;         /**< Drop Column 여부 */

    /*
     * Drop Column Action
     */
    
    qlpDropColumnAction   mDropAction;       /**< Drop Column Action */
    
    stlBool               mIsCascade;        /**< CASCADE CONSTRAINTS */
    
} qlrInitDropColumn;



stlStatus qlrValidateDropColumn( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv );

stlStatus qlrOptCodeDropColumn( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlrOptDataDropColumn( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlrExecuteDropColumn( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );


#endif /* _QLR_ALTER_TABLE_DROP_COLUMN_H_ */

