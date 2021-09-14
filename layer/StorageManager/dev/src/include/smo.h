/*******************************************************************************
 * smo.h
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


#ifndef _SMO_H_
#define _SMO_H_ 1

/**
 * @file smo.h
 * @brief Storage Manager Layer Operators Internal Routines
 */

/**
 * @defgroup smo Operators
 * @ingroup smInternal
 * @{
 */


/*****************************************************
 * AGGREGATION FUNCTIONS
 ****************************************************/

stlStatus smoAssignAggrFunc( smlAggrFuncInfo   * aAggrExecInfo,
                             stlInt32            aBlockIdx,
                             smlEnv            * aEnv );

stlStatus smoBuildAggrFunc( smlAggrFuncInfo   * aAggrExecInfo,
                            stlInt32            aBlockIdx,
                            smlEnv            * aEnv );

stlStatus smoBlockBuildAggrFunc( smlAggrFuncInfo   * aAggrExecInfo,
                                 stlInt32            aStartBlockIdx,
                                 stlInt32            aBlockCnt,
                                 smlEnv            * aEnv );


/*****************************************************
 * AGGREGATION FUNCTIONS (Internal)
 ****************************************************/

/* invalid */
stlStatus smoInvalidAssign( smlAggrFuncInfo   * aAggrExecInfo,
                            stlInt32            aBlockIdx,
                            smlEnv            * aEnv );

stlStatus smoInvalidBuild( smlAggrFuncInfo   * aAggrExecInfo,
                           stlInt32            aBlockIdx,
                           smlEnv            * aEnv );

stlStatus smoInvalidBlockBuild( smlAggrFuncInfo   * aAggrExecInfo,
                                stlInt32            aStartBlockIdx,
                                stlInt32            aBlockCnt,
                                smlEnv            * aEnv );


/* sum */
stlStatus smoSumAssign( smlAggrFuncInfo   * aAggrExecInfo,
                        stlInt32            aBlockIdx,
                        smlEnv            * aEnv );

stlStatus smoSumBuild( smlAggrFuncInfo   * aAggrExecInfo,
                       stlInt32            aBlockIdx,
                       smlEnv            * aEnv );

stlStatus smoSumBlockBuild( smlAggrFuncInfo   * aAggrExecInfo,
                            stlInt32            aStartBlockIdx,
                            stlInt32            aBlockCnt,
                            smlEnv            * aEnv );


/* count */
stlStatus smoCountAssign( smlAggrFuncInfo   * aAggrExecInfo,
                          stlInt32            aBlockIdx,
                          smlEnv            * aEnv );

stlStatus smoCountBuild( smlAggrFuncInfo   * aAggrExecInfo,
                         stlInt32            aBlockIdx,
                         smlEnv            * aEnv );

stlStatus smoCountBlockBuild( smlAggrFuncInfo   * aAggrExecInfo,
                              stlInt32            aStartBlockIdx,
                              stlInt32            aBlockCnt,
                              smlEnv            * aEnv );


/* count asterisk */
stlStatus smoCountAsteriskAssign( smlAggrFuncInfo   * aAggrExecInfo,
                                  stlInt32            aBlockIdx,
                                  smlEnv            * aEnv );

stlStatus smoCountAsteriskBuild( smlAggrFuncInfo   * aAggrExecInfo,
                                 stlInt32            aBlockIdx,
                                 smlEnv            * aEnv );

stlStatus smoCountAsteriskBlockBuild( smlAggrFuncInfo   * aAggrExecInfo,
                                      stlInt32            aStartBlockIdx,
                                      stlInt32            aBlockCnt,
                                      smlEnv            * aEnv );


/* min */
stlStatus smoMinAssign( smlAggrFuncInfo   * aAggrExecInfo,
                        stlInt32            aBlockIdx,
                        smlEnv            * aEnv );

stlStatus smoMinBuild( smlAggrFuncInfo   * aAggrExecInfo,
                       stlInt32            aBlockIdx,
                       smlEnv            * aEnv );

stlStatus smoMinBlockBuild( smlAggrFuncInfo   * aAggrExecInfo,
                            stlInt32            aStartBlockIdx,
                            stlInt32            aBlockCnt,
                            smlEnv            * aEnv );


/* max */
stlStatus smoMaxAssign( smlAggrFuncInfo   * aAggrExecInfo,
                        stlInt32            aBlockIdx,
                        smlEnv            * aEnv );

stlStatus smoMaxBuild( smlAggrFuncInfo   * aAggrExecInfo,
                       stlInt32            aBlockIdx,
                       smlEnv            * aEnv );

stlStatus smoMaxBlockBuild( smlAggrFuncInfo   * aAggrExecInfo,
                            stlInt32            aStartBlockIdx,
                            stlInt32            aBlockCnt,
                            smlEnv            * aEnv );


/** @} */
    
#endif /* _SMO_H_ */
