/*******************************************************************************
 * ellPseudoCol.h
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

#ifndef _ELLPSEUDOCOL_H_
#define _ELLPSEUDOCOL_H_ 1

/**
 * @file ellPseudoCol.h
 * @brief ellPseudoCol Execution Layer Pseudo Column
 */
 

/**
 * @addtogroup ellPseudoCol 
 * @{
 */


/*
 * Sequence
 */

stlStatus ellPseudoSequenceCurrVal( smlTransId       aTransID,
                                    smlStatement   * aStmt,
                                    smlRid         * aSmlRid,
                                    stlInt64         aTableID,
                                    stlUInt16        aInputArgumentCnt,
                                    ellPseudoArg   * aInputArgument,
                                    dtlDataValue   * aResultValue,
                                    ellEnv         * aEnv );

stlStatus ellPseudoSequenceNextVal( smlTransId       aTransID,
                                    smlStatement   * aStmt,
                                    smlRid         * aSmlRid,
                                    stlInt64         aTableID,
                                    stlUInt16        aInputArgumentCnt,
                                    ellPseudoArg   * aInputArgument,
                                    dtlDataValue   * aResultValue,
                                    ellEnv         * aEnv );

/*
 * RowId
 */

stlStatus ellPseudoRowId( smlTransId       aTransID,
                          smlStatement   * aStmt,
                          smlRid         * aSmlRid,
                          stlInt64         aTableID,
                          stlUInt16        aInputArgumentCnt,
                          ellPseudoArg   * aInputArgument,
                          dtlDataValue   * aResultValue,
                          ellEnv         * aEnv );

/** @} ellPseudoCol */

#endif /* _ELLPSEUDOCOL_H_ */
