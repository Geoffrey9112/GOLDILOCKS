/*******************************************************************************
 * qlrCreateView.h
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

#ifndef _QLR_CREATE_VIEW_H_
#define _QLR_CREATE_VIEW_H_ 1

/**
 * @file qlrCreateView.h
 * @brief CREATE VIEW
 */


/****************************************************
 * CREATE VIEW Nodes
 ****************************************************/ 

/**
 * @brief CREATE VIEW 을 위한 Init Plan
 */
typedef struct qlrInitCreateView
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle      mSchemaHandle;      /**< Schema Handle */
    stlChar          * mViewName;          /**< View Name */

    stlBool            mOrReplace;         /**< OR REPLACE option */
    stlBool            mIsForce;           /**< FORCE / NO FORCE option */

    stlBool            mOldViewExist;      /**< Old View 존재 여부 */
    ellDictHandle      mOldViewHandle;     /**< Old View Handle */

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

    stlBool               mInitCompiled;         /**< Compile 성공 여부 */
    qlvInitNode         * mInitQueryNode;        /**< Query 의 Init Plan  */

    /*
     * underlying objects
     */

    ellObjectList       * mUnderlyingRelationList;  /**< Underlying Relations */
    
} qlrInitCreateView;



stlStatus qlrValidateCreateView( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv );

stlStatus qlrOptCodeCreateView( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlrOptDataCreateView( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlrExecuteCreateView( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlrExecuteCreateNonServicePerfView( qllDBCStmt    * aDBCStmt,
                                              qllStatement  * aSQLStmt,
                                              qllEnv        * aEnv );


/*
 * Validation 보조 함수
 */

stlStatus qlrValidateUserColList( qllDBCStmt        * aDBCStmt,
                                  stlChar           * aSQLString,
                                  stlChar           * aUserColString,
                                  qlpList           * aUserColList,
                                  stlInt32            aTargetCount,
                                  qlvInitTarget     * aTargetList,
                                  stlChar          ** aColumnName,
                                  stlBool             aIsForce,
                                  stlBool           * aInitCompiled,
                                  stlBool             aCheckSequence, 
                                  qllEnv            * aEnv );

                                     
/*
 * Execute 보조 함수
 */

void qlrSetColumnInfoFromTarget( qlrInitColumn * aColumnInfo,
                                 stlInt32        aOrdinalPosition,
                                 stlChar       * aColumnName,
                                 qllTarget     * aTargetItem );
    
#endif /* _QLR_CREATE_VIEW_H_ */
