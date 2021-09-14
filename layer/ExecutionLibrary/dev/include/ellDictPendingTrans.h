/*******************************************************************************
 * ellDictPendingTrans.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ellDictPendingTrans.h 7614 2013-03-17 02:40:20Z leekmo $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _ELL_DICT_PENDING_TRANS_H_
#define _ELL_DICT_PENDING_TRANS_H_ 1

/**
 * @file ellDictPendingTrans.h
 * @brief PENDING_TRANS Dictionary Management
 */

/**
 * @addtogroup ellDictPendingTrans
 * @{
 */

stlStatus ellInsertPendingTrans( smlTransId              aTransID,
                                 smlStatement          * aStmt,
                                 stlChar               * aXidString,
                                 smlTransId              aTransId,
                                 stlChar               * aComments,
                                 stlBool                 aHeuristicDecision,
                                 ellEnv                * aEnv );

/** @} ellDictPendingTrans */



#endif /* _ELL_DICT_PENDING_TRANS_H_ */
