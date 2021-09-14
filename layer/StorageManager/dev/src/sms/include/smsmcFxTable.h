/*******************************************************************************
 * smsmcFxTable.h
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


#ifndef _SMSMCFXTABLE_H_
#define _SMSMCFXTABLE_H_ 1

/**
 * @file smsmcFxTable.h
 * @brief Storage Manager Layer Memory Circular Fixed Table Component Internal Routines
 */

/**
 * @defgroup smsmcFxTable Memory Circular Fixed Table
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmcOpenExtentCallback( void   * aStmt,
                                   void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv );
stlStatus smsmcCloseExtentCallback( void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv );
stlStatus smsmcGetDumpObjectCallback( stlInt64   aTransID,
                                      void     * aStmt,
                                      stlChar  * aDumpObjName,
                                      void    ** aDumpObjHandle,
                                      knlEnv   * aEnv );
stlStatus smsmbBuildRecordExtentCallback( void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aTupleExist,
                                          knlEnv            * aEnv );

/** @} */
    
#endif /* _SMSMCFXTABLE_H_ */
