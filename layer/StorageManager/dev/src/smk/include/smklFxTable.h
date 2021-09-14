/*******************************************************************************
 * smklFxTable.h
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


#ifndef _SMKLFXTABLE_H_
#define _SMKLFXTABLE_H_ 1

/**
 * @file smklFxTable.h
 * @brief Storage Manager Layer Lock Manager Fixed Table Component Internal Routines
 */

/**
 * @defgroup smklFxTable Lock Fixed Table
 * @ingroup smxlInternal
 * @{
 */

stlStatus smklOpenLockWaitCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv );
stlStatus smklCloseLockWaitCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv );
stlStatus smklBuildRecordLockWaitCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aEnv );

/** @} */

#endif /* _SMKLFXTABLE_H_ */
