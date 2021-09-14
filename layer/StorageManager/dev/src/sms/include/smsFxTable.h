/*******************************************************************************
 * smsFxTable.h
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


#ifndef _SMSFXTABLE_H_
#define _SMSFXTABLE_H_ 1

/**
 * @file smsFxTable.h
 * @brief Storage Manager Layer Segment Fixed Table Component Internal Routines
 */

/**
 * @defgroup smsFxTable Segment Fixed Table
 * @ingroup smsInternal
 * @{
 */

stlStatus smsOpenSegmentCallback( void   * aStmt,
                                  void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv );
stlStatus smsCloseSegmentCallback( void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv );
stlStatus smsBuildRecordSegmentCallback( void             * aPathCtrl,
                                         dtlDataValueList * aValueList,
                                         stlBool          * aTupleExist,
                                         knlEnv           * aEnv );

/** @} */
    
#endif /* _SMSFXTABLE_H_ */
