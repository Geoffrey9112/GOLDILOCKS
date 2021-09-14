/*******************************************************************************
 * qlrAlterView.h
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

#ifndef _QLR_ALTER_VIEW_H_
#define _QLR_ALTER_VIEW_H_ 1

/**
 * @file qlrAlterView.h
 * @brief ALTER TABLE .. DROP COLUMN
 */

/**
 * @brief DROP COLUMN 을 위한 Init Plan
 */
typedef struct qlrInitAlterView
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle         mSchemaHandle;     /**< Schema Handle */
    ellDictHandle         mViewHandle;       /**< View Handle */

    qlpAlterViewAction    mAlterAction;      /**< Alter View Action */

    /*
     * View Column List
     */
    
    stlChar          * mViewColumnString;  /**< "VIEW view_name( ... )" */
    
    stlInt32           mColumnCount;       /**< View Column Count */
    stlChar         ** mColumnName;        /**< View Column Names */

    /*
     * Original Query Expression
     */
    
    stlChar             * mViewQueryString;      /**< "SELECT ... " */
    qlvInitNode         * mInitQueryNode;        /**< Query 의 Init Plan  */

    /*
     * underlying objects
     */

    ellObjectList       * mUnderlyingRelationList;  /**< Underlying Relations */
    
} qlrInitAlterView;



/**
 * @brief ALTER VIEW 을 위한 Code Plan
 */
typedef struct qlrCodeAlterView
{
    qllOptimizationNode * mCodeQueryPlan;        /**< Query 의 Code Plan  */
} qlrCodeAlterView;


stlStatus qlrValidateAlterView( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv );

stlStatus qlrOptCodeAlterView( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qlrOptDataAlterView( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qlrExecuteAlterView( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

#endif /* _QLR_ALTER_VIEW_H_ */

