/*******************************************************************************
 * smqFxTable.h
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


#ifndef _SMQFXTABLE_H_
#define _SMQFXTABLE_H_ 1

/**
 * @file smqFxTable.h
 * @brief Storage Manager Layer Sequence Fixed Table Component Internal Routines
 */

/**
 * @defgroup smqFxTable Sequence Fixed Table
 * @ingroup smqInternal
 * @{
 */

stlStatus smqOpenSequenceCallback( void   * aStmt,
                                   void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv );
stlStatus smqCloseSequenceCallback( void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv );
stlStatus smqBuildRecordSequenceCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aTupleExist,
                                          knlEnv            * aEnv );

/** @} */
    
#endif /* _SMQFXTABLE_H_ */
