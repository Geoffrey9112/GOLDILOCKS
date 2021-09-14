/*******************************************************************************
 * knpc.h
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knpc.h 9401 2013-08-21 03:07:33Z bsyou $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _KNPC_H_
#define _KNPC_H_ 1

/**
 * @file knpc.h
 * @brief Plan Cache
 */

/**
 * @addtogroup knpc
 * @{
 */


/*
 * Clock
 */

stlStatus knpcGetClock( knpcClock ** aClock,
                        knlEnv     * aEnv );

stlStatus knpcAgingClock( knpcClock * aClock,
                          stlInt32    aDeadAge,
                          stlInt32    aAgingThreshold,
                          stlInt64    aAgingPlanInterval,
                          stlBool   * aIsSuccess,
                          knlEnv    * aEnv );

stlStatus knpcInitClock( knpcClock        * aClock,
                         stlInt32           aClockId,
                         knlMemController * aMemController,
                         knlEnv           * aEnv );

stlStatus knpcExtendClock( knpcClock   * aClock,
                           stlBool     * aIsSuccess,
                           knlEnv      * aEnv );

/*
 * Flange
 */

#define KNPC_IS_EMPTY_FLANGE( aFlange ) \
    ( ( (aFlange)->mSqlString == NULL ) ? STL_TRUE : STL_FALSE )

void knpcInitFlange( knpcFlange        * aFlange,
                     stlInt32            aHashValue,
                     stlChar           * aSqlString,
                     stlInt32            aSqlLength,
                     knlPlanUserInfo   * aUserInfo,
                     knlPlanCursorInfo * aCursorInfo,
                     knlPlanSqlInfo    * aSqlInfo,
                     stlBool             aReInit );

void knpcResetFlange( knpcFlange * aFlange );

stlStatus knpcAllocFlange( knpcClock          * aClock,
                           stlInt32             aHashValue,
                           stlChar            * aSqlString,
                           stlInt64             aSqlLength,
                           knlPlanUserInfo    * aUserInfo,
                           knlPlanCursorInfo  * aCursorInfo,
                           knlPlanSqlInfo     * aSqlInfo,
                           knpcFlange        ** aFlange,
                           knlEnv             * aEnv );

stlStatus knpcFreeFlange( knpcClock  * aClock,
                          knpcFlange * aFlange,
                          knlEnv     * aEnv );

/*
 * Plan Hash
 */

knpcFlange * knpcFindHashItem( knpcHashBucket    * aHashBucket,
                               stlInt32            aHashValue,
                               stlChar           * aSqlString,
                               stlInt64            aSqlLength,
                               knlPlanUserInfo   * aUserInfo,
                               knlPlanCursorInfo * aCursorInfo );

knpcHashBucket * knpcGetHashBucket( stlInt32 aHashValue );

stlStatus knpcInitPlanCache( knsEntryPoint  * aSharedEntry,
                             knlEnv         * aEnv );

/*
 * User Plan
 */

stlStatus knpcFreePlan( knpcFlange * aFlange,
                        knpcPlan   * aPlan,
                        knlEnv     * aEnv );

knpcPlan * knpcAddPlan( knpcClock    * aClock,
                        knpcFlange   * aFlange,
                        knlRegionMem * aPlanMem,
                        void         * aCodePlan );

stlBool knpcComparePlan( knpcFlange         * aFlange,
                         knlComparePlanFunc   aCompareFunc,
                         knlPlanSqlInfo     * aSqlInfo,
                         void               * aCodePlan );

/** @} knpc */

#endif /* _KNPC_H_ */
