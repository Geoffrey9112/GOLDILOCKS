/*******************************************************************************
 * smxlFxTable.h
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


#ifndef _SMXLFXTABLE_H_
#define _SMXLFXTABLE_H_ 1

/**
 * @file smxlFxTable.h
 * @brief Storage Manager Layer Local Transactional Fixed Table Component Internal Routines
 */

/**
 * @defgroup smxlFxTable Transaction Fixed Table
 * @ingroup smxlInternal
 * @{
 */

stlStatus smxlOpenTransCallback( void   * aStmt,
                                 void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv );
stlStatus smxlCloseTransCallback( void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv );
stlStatus smxlBuildRecordTransCallback( void              * aDumpObjHandle,
                                        void              * aPathCtrl,
                                        knlValueBlockList * aValueList,
                                        stlInt32            aBlockIdx,
                                        stlBool           * aTupleExist,
                                        knlEnv            * aEnv );

/** @} */

#endif /* _SMXLFXTABLE_H_ */
