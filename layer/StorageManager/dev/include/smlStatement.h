/*******************************************************************************
 * smlStatement.h
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


#ifndef _SMLSTATEMENT_H_
#define _SMLSTATEMENT_H_ 1

/**
 * @file smlStatement.h
 * @brief Storage Manager Statement Routines
 */

/**
 * @defgroup smlStatement Statement
 * @ingroup smExternal
 * @{
 */

stlStatus smlAllocStatement( smlTransId      aTransId,
                             void          * aUpdataRelHandle,
                             stlInt32        aAttribute,
                             stlInt64        aLockTimeout,
                             stlBool         aNeedSnapshotIterator,
                             smlStatement ** aStatement,
                             smlEnv        * aEnv );

stlStatus smlFreeStatement( smlStatement * aStatement,
                            smlEnv       * aEnv );

stlInt64 smlGetStatementTimestamp( smlStatement * aStatement );

stlStatus smlRollbackStatement( smlStatement * aStatement,
                                smlEnv       * aEnv );

stlStatus smlAllocIterator( smlStatement              * aStatement,
                            void                      * aRelationHandle,
                            smlTransReadMode            aTransReadMode,
                            smlStmtReadMode             aStmtReadMode,
                            smlIteratorProperty       * aProperty,
                            smlIteratorScanDirection    aScanDirection,
                            void                     ** aIterator,
                            smlEnv                    * aEnv );

stlStatus smlFreeIterator( void    * aIterator,
                           smlEnv  * aEnv );

stlStatus smlResetStatement( smlStatement * aStatement,
                             stlBool        aDoRollback,
                             smlEnv       * aEnv );

stlStatus smlUpdateRelHandle( smlStatement * aStatement,
                              void         * aRelHandle,
                              smlEnv       * aEnv );

/** @} */

#endif /* _SMLSTATEMENT_H_ */
