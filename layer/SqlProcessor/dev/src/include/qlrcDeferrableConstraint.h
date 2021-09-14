/*******************************************************************************
 * qlrcDeferrableConstraint.h
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


#ifndef _QLRC_DEFERRABLE_CONSTRAINT_H_
#define _QLRC_DEFERRABLE_CONSTRAINT_H_ 1

/**
 * @file qlrcDeferrableConstraint.h
 * @brief SQL Processor Layer Deferrable Constraint
 */

/*********************************************************
 * Initialize & Finalize
 *********************************************************/

void qlrcInitDeferConstraintInfo( qllSessionEnv * aSessEnv );

void qlrcStmtEndDeferConstraint( qllEnv * aEnv );
void qlrcStmtRollbackDeferConstraint( qllEnv * aEnv );

void qlrcTransEndDeferConstraint( qllEnv * aEnv );
void qlrcResetDeferConstraint( qllEnv * aEnv );

stlStatus qlrcAddSavepointIntoDeferConstraint( stlInt64     aSavepointTimestamp,
                                               qllEnv     * aEnv );

void qlrcRollbackToSavepointDeferConstraint( stlInt64     aSavepointTimestamp,
                                             qllEnv     * aEnv );

/*********************************************************
 * Constraint Mode
 *********************************************************/

stlStatus qlrcAddSetConstraintMode( qllSetConstraintMode   aSetMode,
                                    ellDictHandle        * aConstraintHandle,
                                    qllEnv               * aEnv );

stlBool  qlrcIsCheckTimeDeferred( ellDictHandle        * aConstraintHandle,
                                  qllEnv               * aEnv );

/*********************************************************
 * Constraint Collision
 *********************************************************/

stlBool  qlrcIsSameCollisionItem( qllCollisionItem * aItem1, qllCollisionItem * aItem2 );
    
stlStatus qlrcSetDeferredCollision( qllCollItemType   aCollItemType,
                                    ellDictHandle   * aObjectHandle,
                                    stlInt64          aCollisionCount,
                                    qllEnv          * aEnv );

stlBool qlrcHasViolatedDeferredConstraint( qllEnv * aEnv );
stlBool qlrcHasXaViolatedConstraint( knlXaContext * aXaContext );

stlBool qlrcIsViolatedConstraint( ellDictHandle * aConstHandle, qllEnv * aEnv );

stlStatus qlrcSetViolatedHandleMessage( smlTransId      aTransID,
                                        stlChar       * aBuffer,
                                        stlInt32        aBufferLength,
                                        ellDictHandle * aConstHandle,
                                        qllEnv        * aEnv );
                                  


#endif /* _QLRC_DEFERRABLE_CONSTRAINT_H_ */
