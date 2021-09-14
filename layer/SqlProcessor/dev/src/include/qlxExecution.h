/*******************************************************************************
 * qlxExecution.h
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

#ifndef _QLXEXECUTION_H_
#define _QLXEXECUTION_H_ 1

/**
 * @file qlxExecution.h
 * @brief SQL Processor Layer Execution 
 */


/*******************************************************************************
 * Structures
 ******************************************************************************/

/**
 * @addtogroup qlx
 * @{
 */

struct qlxExecInsertRelObject
{
    /*
     * PRIMARY KEY constraint
     */
    
    knlValueBlockList ** mPrimaryKeyIndexBlock;  /**< Key Value 공간 */
    stlBool            * mPrimaryKeyIsDeferred;  /**< Deferred Constraint 여부 */
    stlInt64           * mPrimaryKeyCollCnt;     /**< Immediate Collision Count */

    /*
     * UNIQUE constraint
     */

    knlValueBlockList ** mUniqueKeyIndexBlock;   /**< Key Value 공간 */
    stlBool            * mUniqueKeyIsDeferred;   /**< Deferred Constraint 여부 */
    stlInt64           * mUniqueKeyCollCnt;     /**< Immediate Collision Count */
    
    /*
     * FOREIGN KEY constraint
     */

    knlValueBlockList ** mForeignKeyIndexBlock;  /**< Key Value 공간 */
    stlBool            * mForeignKeyIsDeferred;  /**< Deferred Constraint 여부 */
    stlInt64           * mForeignKeyCollCnt;     /**< Immediate Collision Count */
    knlValueBlockList ** mParentIndexBlock;      /**< Parent Unique Key Index Value 공간 */

    /*
     * @todo CHILD FOREIGN KEY constraint
     */
    
    /*
     * Unique Index
     */
        
    stlBool            * mUniqueIndexIsDeferred;  /**< Deferred Constraint 여부 */
    stlInt64           * mUniqueIndexCollCnt;     /**< Immediate Collision Count */
    knlValueBlockList ** mUniqueIndexBlock;       /**< Key Value 공간 */
    
    /*
     * Non-Unique Index
     */
        
    knlValueBlockList ** mNonUniqueIndexBlock;  /**< Key Value 공간 */

    /*
     * NOT NULL constraint
     */

    knlValueBlockList ** mNotNullColumnBlock; /**< Column Value 공간 */
    stlBool            * mNotNullIsDeferred;  /**< Deferred Constraint 여부 */
    stlInt64           * mNotNullCollCnt;     /**< Immediate Collision Count */
    
    /*
     * CHECK constraint
     */

    stlBool            * mCheckIsDeferred;  /**< Deferred Constraint 여부 */
    stlInt64           * mCheckCollCnt;     /**< Immediate Collision Count */


    /*
     * Identity Column
     */
    
    knlValueBlockList * mIdentityColumnBlock; /**< Column Value 공간 */

    /**
     * Supplemental Log
     */

    stlBool            mIsSuppLog;            /**< Supplemental Logging 여부 */
};


struct qlxExecUpdateRelObject
{
    /*
     * PRIMRAY KEY constraint
     */

    stlBool            * mPrimaryKeyIsDeferred;     /**< Deferred Constraint 여부 */
    stlInt64           * mPrimaryKeyCollCnt;        /**< Immediate Collision Count */
    knlValueBlockList ** mPrimaryKeyDeleteKeyBlock; /**< DELETE Key Value 공간 */
    knlValueBlockList ** mPrimaryKeyInsertKeyBlock; /**< INSERT Key Value 공간 */

    /*
     * UNIQUE constraint
     */

    stlBool            * mUniqueKeyIsDeferred;     /**< Deferred Constraint 여부 */
    stlInt64           * mUniqueKeyCollCnt;        /**< Immediate Collision Count */
    knlValueBlockList ** mUniqueKeyDeleteKeyBlock; /**< DELETE Key Value 공간 */
    knlValueBlockList ** mUniqueKeyInsertKeyBlock; /**< INSERT Key Value 공간 */
    
    /*
     * @todo FOREIGN KEY constraint
     */

    /*
     * @todo CHILD FOREGIN KEY constraint
     */

    /*
     * Unique Index
     */
        
    stlBool            * mUniqueIndexIsDeferred;  /**< Deferred Constraint 여부 */
    stlInt64           * mUniqueIndexCollCnt;        /**< Collision Count */
    knlValueBlockList ** mUniqueIndexDeleteKeyBlock; /**< DELETE Key Value 공간 */
    knlValueBlockList ** mUniqueIndexInsertKeyBlock; /**< INSERT Key Value 공간 */
    
    /*
     * Non-Unique Index
     */
        
    knlValueBlockList ** mNonUniqueIndexDeleteKeyBlock; /**< DELETE Key Value 공간 */
    knlValueBlockList ** mNonUniqueIndexInsertKeyBlock; /**< INSERT Key Value 공간 */

    /*
     * NOT NULL constraint
     */

    stlBool            * mNotNullIsDeferred;     /**< Deferred Constraint 여부 */
    stlInt64           * mNotNullCollCnt;        /**< Immediate Collision Count */
    knlValueBlockList ** mOldNotNullColumnBlock; /**< Column Value 공간 */
    knlValueBlockList ** mNewNotNullColumnBlock; /**< Column Value 공간 */
    
    /*
     * @todo CHECK constraint
     */

    /*
     * Identity Column
     */
    
    knlValueBlockList * mIdentityColumnBlock; /**< Column Value 공간 */
    
    /**
     * Supplemental Log
     */

    stlBool              mIsSuppLog;                 /**< Supplemental Logging 여부 */
    knlValueBlockList  * mSuppLogPrimaryKeyBlock;    /**< Supplemental Logging을 위한 Primary Key Value 공간 */
    knlValueBlockList  * mSuppLogBeforeColBlock;     /**< Supplemental Logging 지원시 Before Column List */
};


struct qlxExecDeleteRelObject
{
    /*
     * PRIMARY KEY constraint
     */
    
    knlValueBlockList ** mPrimaryKeyIndexBlock;   /**< Key Value 공간 */
    stlBool            * mPrimaryKeyIsDeferred;   /**< Deferred Constraint 여부 */
    stlInt64           * mPrimaryKeyCollCnt;      /**< Immediate Collision Count */

    /*
     * UNIQUE KEY constraint
     */

    knlValueBlockList ** mUniqueKeyIndexBlock;    /**< Key Value 공간 */
    stlBool            * mUniqueKeyIsDeferred;    /**< Deferred Constraint 여부 */
    stlInt64           * mUniqueKeyCollCnt;       /**< Immediate Collision Count */
    
    /*
     * FOREGIN KEY constraint
     */

    knlValueBlockList ** mForeignKeyIndexBlock;   /**< Key Value 공간 */
    stlBool            * mForeignKeyIsDeferred;   /**< Deferred Constraint 여부 */
    stlInt64           * mForeignKeyCollCnt;      /**< Immediate Collision Count */

    /*
     * CHILD FOREIGN KEY constraint
     */

    knlValueBlockList ** mChildForeignKeyIndexBlock;    /**< Key Value 공간 */

    /*
     * Unique Index
     */
        
    stlBool            * mUniqueIndexIsDeferred;  /**< Deferred Constraint 여부 */
    stlInt64           * mUniqueIndexCollCnt;     /**< Immediate Collision Count */
    knlValueBlockList ** mUniqueIndexBlock;       /**< Key Value 공간 */
    
    /*
     * Non-Unique Index
     */
        
    knlValueBlockList ** mNonUniqueIndexBlock;    /**< Key Value 공간 */

    /*
     * deferrable NOT NULL constraint
     */

    knlValueBlockList ** mNotNullColumnBlock; /**< Column Value 공간 */
    stlBool            * mNotNullIsDeferred;  /**< Deferred Constraint 여부 */
    stlInt64           * mNotNullCollCnt;     /**< Immediate Collision Count */

    /*
     * @todo deferrable CHECK constraint
     */
    
    /**
     * Supplemental Log
     */

    stlBool              mIsSuppLog;                 /**< Supplemental Logging 여부 */
    knlValueBlockList  * mSuppLogPrimaryKeyBlock;    /**< Supplemental Logging을 위한 Primary Key Value 공간 */
};


/** @} qlx */


/*******************************************************************************
 * SELECT
 ******************************************************************************/

/**
 * @defgroup qlxSelect SELECT Execution
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecuteSelect( smlTransId      aTransID,
                            smlStatement  * aStmt,
                            qllDBCStmt    * aDBCStmt,
                            qllStatement  * aSQLStmt,
                            qllEnv        * aEnv );

/** @} qlxSelect */


/**
 * @defgroup qlxSelectInto SELECT INTO Execution
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecuteSelectInto( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

/** @} qlxSelectInto */


/*******************************************************************************
 * QUERY EXPR
 ******************************************************************************/

/**
 * @defgroup qlxQueryExpr QUERY EXPR Execution
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecuteQueryExpr( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

/** @} qlxQueryExpr */


/*******************************************************************************
 * INSERT 
 ******************************************************************************/

/**
 * @defgroup qlxInsert INSERT VALUES Execution
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecuteInsertValues( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

/** @} qlxInsert */


/**
 * @defgroup qlxInsertSelect INSERT .. SELECT Execution
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecuteInsertSelect( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

/** @} qlxInsertSelect */


/**
 * @defgroup qlxInsertReturnInto INSERT RETURNING INTO Execution
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecuteInsertReturnInto( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv );

/** @} qlxInsertReturnInto */


/**
 * @defgroup qlxInsertReturnQuery INSERT RETURNING Query Execution
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecuteInsertReturnQuery( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

/** @} qlxInsertReturnQuery */




/*******************************************************************************
 * UPDATE 
 ******************************************************************************/

/**
 * @defgroup qlxUpdate UPDATE Execution
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecuteUpdate( smlTransId      aTransID,
                            smlStatement  * aStmt,
                            qllDBCStmt    * aDBCStmt,
                            qllStatement  * aSQLStmt,
                            qllEnv        * aEnv );

/** @} qlxUpdate */



/**
 * @defgroup qlxUpdateReturnInto UPDATE RETURNING INTO Execution
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecuteUpdateReturnInto( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv );

/** @} qlxUpdateReturnInto */


/**
 * @defgroup qlxUpdateReturnQuery UPDATE RETURNING Query Execution
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecuteUpdateReturnQuery( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

/** @} qlxUpdateReturnInto */



/**
 * @defgroup qlxUpdatePositioned UPDATE CURRENT OF Execution
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecuteUpdatePositioned( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv );

/** @} qlxUpdatePositioned */



/*******************************************************************************
 * DELETE 
 ******************************************************************************/

/**
 * @defgroup qlxDelete DELETE Execution
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecuteDelete( smlTransId      aTransID,
                            smlStatement  * aStmt,
                            qllDBCStmt    * aDBCStmt,
                            qllStatement  * aSQLStmt,
                            qllEnv        * aEnv );

/** @} qlxDelete */



/**
 * @defgroup qlxDeleteReturnInto DELETE RETURNING INTO Execution
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecuteDeleteReturnInto( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv );

/** @} qlxDeleteReturnInto */


/**
 * @defgroup qlxDeleteReturnQuery DELETE RETURNING Query Execution
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecuteDeleteReturnQuery( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

/** @} qlxDeleteReturnQuery */


/**
 * @defgroup qlxDeletePositioned DELETE CURRENT OF Execution
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecuteDeletePositioned( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv );

/** @} qlxDeletePositioned */


/*******************************************************************************
 * Pseudo Column 
 ******************************************************************************/

/**
 * @defgroup qlxPseudo Pseudo Column Execution 
 * @ingroup qlx
 * @{
 */

stlStatus qlxExecutePseudoColumnBlock( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       dtlIterationTime    aIterationTime,
                                       ellDictHandle     * aTableHandle,
                                       smlRowBlock       * aSmlRowBlock,
                                       qlvRefExprList    * aPseudoColList,
                                       knlValueBlockList * aPseudoBlockList,
                                       stlInt32            aStartBlockIdx,
                                       stlInt32            aBlockCnt,
                                       qllEnv            * aEnv );

stlStatus qlxExecuteRowIdColumnBlock( smlTransId          aTransID,
                                      smlStatement      * aStmt,
                                      ellDictHandle     * aTableHandle,
                                      smlRowBlock       * aSmlRowBlock,
                                      qlvInitExpression * aRowIdColumnExpr,
                                      knlValueBlockList * aRowIdColBlockList,
                                      stlInt32            aStartBlockIdx,
                                      stlInt32            aBlockCnt,
                                      qllEnv            * aEnv );

/** @} qlxPseudo */

#endif /* _QLXEXECUTION_H_ */
