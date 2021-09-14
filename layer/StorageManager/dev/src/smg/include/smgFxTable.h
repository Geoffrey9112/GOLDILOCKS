/*******************************************************************************
 * smgFxTable.h
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


#ifndef _SMGFXTABLE_H_
#define _SMGFXTABLE_H_ 1

/**
 * @file smgFxTable.h
 * @brief General Fixed Table Component Internal Routines
 */

/**
 * @defgroup smgFxTable General Fixed Table
 * @ingroup smgInternal
 * @{
 */

stlStatus smgOpenEnvCallback( void   * aStmt,
                              void   * aDumpObjHandle,
                              void   * aPathCtrl,
                              knlEnv * aEnv );

stlStatus smgCloseEnvCallback( void   * aDumpObjHandle,
                               void   * aPathCtrl,
                               knlEnv * aEnv );

stlStatus smgBuildRecordEnvCallback( void              * aDumpObjHandle,
                                     void              * aPathCtrl,
                                     knlValueBlockList * aValueList,
                                     stlInt32            aBlockIdx,
                                     stlBool           * aTupleExist,
                                     knlEnv            * aEnv );

/** @} */
    
#endif /* _SMGFXTABLE_H_ */
