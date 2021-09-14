/*******************************************************************************
 * smtFxTable.h
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


#ifndef _SMTFXTABLE_H_
#define _SMTFXTABLE_H_ 1

/**
 * @file smtFxTable.h
 * @brief Storage Manager Layer Tablespace Fixed Table Component Internal Routines
 */

/**
 * @defgroup smtFxTable Tablespace Fixed Table
 * @ingroup smtInternal
 * @{
 */

stlStatus smtOpenTbsExtCallback( void   * aStmt,
                                 void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv );
stlStatus smtCloseTbsExtCallback( void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv );
stlStatus smtGetDumpObjectCallback( void   ** aDumpObjHandle,
                                    stlChar * aDumpObjName,
                                    knlEnv  * aEnv );
stlStatus smtBuildRecordTbsExtCallback( void           * aPathCtrl,
                                        stlValueList   * aValueList,
                                        stlBool        * aTupleExist,
                                        knlEnv         * aEnv );

/** @} */
    
#endif /* _SMTFXTABLE_H_ */
