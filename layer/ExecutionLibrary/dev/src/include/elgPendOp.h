/*******************************************************************************
 * elgPendOp.h
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


#ifndef _ELG_PEND_OP_H_
#define _ELG_PEND_OP_H_ 1

/**
 * @file elgPendOp.h
 * @brief Execution Library Layer Pending Operation
 */

/**
 * @defgroup elgPendOp Pending Operation
 * @ingroup elg
 * @internal
 * @{
 */

/**
 * @brief Transaction Pending Action Type
 */

typedef enum
{
    ELG_PEND_ACT_NA = 0,                    /**< N/A */

    ELG_PEND_ACT_COMMIT,                    /**< Commit 시 수행할 Pending Action */
    ELG_PEND_ACT_ROLLBACK,                  /**< Rollback 시 수행할 Action */

    ELG_PEND_ACT_MAX
    
} elgPendActType;
    
/**
 * @brief Pending Operation의 Function Pointer 정의
 */
typedef stlStatus (*elgPendOpFunc) ( smlTransId     aTransID,
                                     void         * aArgs,
                                     void         * aAgingEventBuffer,
                                     ellEnv       * aEnv );


/**
 * @brief Pending Operation ID
 */
typedef enum
{
    ELG_PEND_NA = 0,                        /**< N/A */

    ELG_PRE_ROLLBACK_SET_FLAG,              /**< Pre-ROLLBACK Action */
    
    ELG_PEND_FREE_DICT_MEM,                 /**< Free Dictionary Memory */
    
    ELG_PEND_ROLLBACK_INSERT_HASH_ELEMENT,  /**< Rollback Insert Hash Element */
    ELG_PEND_ROLLBACK_DELETE_HASH_ELEMENT,  /**< Rollback Delete Hash Element */
    ELG_PEND_ROLLBACK_ADD_HASH_RELATED,     /**< Rollback Add Hash Related */
    ELG_PEND_ROLLBACK_DEL_HASH_RELATED,     /**< Rollback del Hash Related */
    
    ELG_PEND_COMMIT_INSERT_HASH_ELEMENT,    /**< Commit Insert Hash Element */
    ELG_PEND_COMMIT_DELETE_HASH_ELEMENT,    /**< Commit Delete Hash Element */
    ELG_PEND_COMMIT_ADD_HASH_RELATED,       /**< Commit Add Hash Related */
    ELG_PEND_COMMIT_DEL_HASH_RELATED,       /**< Commit del Hash Related */
    
    ELG_PEND_MAX
} elgPendOpID;


/**
 * @brief Pending Operation을 위한 구조
 */
typedef struct elgPendOP
{
    elgPendOpID              mPendOpID;      /**< Pending Operation ID */
    stlInt64                 mStmtTimestamp; /**< DDL Statement Timestamp */
    elgPendActType           mActionType;    /**< Pending Action Type */
    void                   * mArgs;          /**< Arguments */
    void                   * mAgingEventBuffer;   /**< Aging Event Buffer */
    struct elgPendOP  * mNext;               /**< Next Pending Operation */
} elgPendOP;

void elgRegisterPendOperations();

void elgSetPendOp( elgPendOpID    aPendOpID,
                   elgPendOpFunc  aPendFunc );

void elgAddPreRollbackAction( elgPendOpID       aPendOpID,
                              elgPendOP       * aPendOP,
                              void            * aArgs,
                              stlUInt32         aArgsByteSize,
                              ellEnv          * aEnv );

void elgAddStmtRollbackPendOP( elgPendOpID       aPendOpID,
                               elgPendOP       * aPendOP,
                               void            * aArgs,
                               stlUInt32         aArgsByteSize,
                               ellEnv          * aEnv );

void elgAddStmtSuccessPendOP( elgPendOpID       aPendOpID,
                              elgPendOP       * aPendOP,
                              void            * aArgs,
                              stlUInt32         aArgsByteSize,
                              void            * aAgingEventBuffer,
                              ellEnv          * aEnv );

void elgAddStmtRollbackFreeCacheMemPendOP( elgPendOP       * aPendOP,
                                           void            * aArgs,
                                           stlUInt32         aArgsByteSize,
                                           void            * aAgingEventBuffer,
                                           ellEnv          * aEnv );

stlStatus elgExecStmtRollbackPendOP( smlTransId     aTransID,
                                     ellEnv       * aEnv );

void elgExecStmtSuccessPendOP( ellEnv * aEnv );

stlStatus elgExecPreActionTransEnd( smlTransId       aTransID,
                                    elgPendActType   aActionType,
                                    ellEnv         * aEnv );

stlStatus elgExecStmtPreRollbackAction( smlTransId   aTransID,
                                        stlBool      aIsSuccess,
                                        ellEnv     * aEnv );

stlStatus elgExecTransPendOP( smlTransId       aTransID,
                              elgPendActType   aActionType,
                              ellEnv         * aEnv );

stlStatus elgExecRollbackToSavepointPendOP( smlTransId       aTransID,
                                            stlInt64         aSavepointTimestamp,
                                            ellEnv         * aEnv );

stlStatus elgResetSessionPendOp4NotOpenPhase( ellEnv * aEnv );

/** @} */
    
#endif /* _ELG_PEND_OP_H_ */
