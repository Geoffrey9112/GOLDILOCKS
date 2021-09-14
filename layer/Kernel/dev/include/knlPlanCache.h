/*******************************************************************************
 * knlPlanCache.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knlPlanCache.h 9945 2013-10-18 01:35:21Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _KNLPLANCACHE_H_
#define _KNLPLANCACHE_H_ 1

/**
 * @file knlPlanCache.h
 * @brief Plan Cache
 */

/**
 * @defgroup knlPlanCache Plan Cache
 * @ingroup knExternal
 * @{
 */

stlStatus knlFixSql( stlChar           * aSqlString,
                     stlInt64            aSqlLength,
                     knlPlanUserInfo   * aUserInfo,
                     knlPlanCursorInfo * aCursorInfo,
                     knlSqlHandle      * aSqlHandle,
                     knlPlanSqlInfo    * aSqlInfo,
                     knlEnv            * aEnv );

stlStatus knlUnfixSql( knlSqlHandle   aSqlHandle,
                       knlEnv       * aEnv );

stlStatus knlCacheSql( stlChar            * aSqlString,
                       stlInt64             aSqlLength,
                       knlRegionMem       * aPlanMem,
                       void               * aCodePlan,
                       knlPlanUserInfo    * aUserInfo,
                       knlPlanCursorInfo  * aCursorInfo,
                       knlPlanSqlInfo     * aSqlInfo,
                       knlComparePlanFunc   aCompareFunc,
                       knlSqlHandle       * aSqlHandle,
                       knlEnv             * aEnv );

stlStatus knlDiscardSql( knlSqlHandle   aSqlHandle,
                         knlEnv       * aEnv );

stlStatus knlCleanupPlans( knlEnv * aEnv );

stlStatus knlCachePlan( knlSqlHandle     aSqlHandle,
                        knlRegionMem   * aPlanMem,
                        void           * aCodePlan,
                        knlPlanHandle  * aPlanHandle,
                        knlEnv         * aEnv );

stlStatus knlAddFixedSql( knlSqlHandle   aSqlHandle,
                          stlBool        aIgnoreError,
                          stlBool      * aIsSuccess,
                          knlEnv       * aEnv );

stlStatus knlRemoveFixedSql( knlSqlHandle   aSqlHandle,
                             knlEnv       * aEnv );

void knlSetPlanUserInfo( knlPlanUserInfo  * aUserInfo,
                         stlInt64           aUserId );

void knlSetPlanStmtCursorInfo( knlPlanCursorInfo        * aCursorInfo,
                               stlBool                    aIsDbcCursor,
                               stlInt32                   aStandardType,
                               stlInt32                   aSensitivity,
                               stlInt32                   aScrollability,
                               stlInt32                   aHoldability,
                               stlInt32                   aUpdatability,
                               stlInt32                   aReturnability );

void knlSetPlanSqlInfo( knlPlanSqlInfo  * aSqlInfo,
                        stlBool           aIsQuery,
                        stlInt32          aStmtType,
                        stlInt32          aBindCount,
                        stlInt32          aPrivCount,
                        stlInt32          aPrivSize,
                        void            * aPrivArray );

void knlGetPlanSqlInfo( knlSqlHandle    * aSqlHandle,
                        knlPlanSqlInfo  * aSqlInfo );

knlDynamicMem * knlGetPlanRegionMem( knlEnv * aEnv );

stlStatus knlInitPlanIterator( knlPlanIterator * aIterator,
                               knlSqlHandle    * aSqlHandle,
                               knlEnv          * aEnv );
    
stlStatus knlPlanFetchNext( knlPlanIterator  * aIterator,
                            void            ** aCodePlan,
                            stlInt64         * aPlanSize,
                            knlEnv           * aEnv );

stlStatus knlFiniPlanIterator( knlPlanIterator * aIterator,
                               knlEnv          * aEnv );

stlStatus knlInitFlangeIterator( knlFlangeIterator * aIterator,
                                 knlEnv            * aEnv );

stlStatus knlFlangeFetchNext( knlFlangeIterator  * aIterator,
                              knlSqlHandle      ** aSqlHandle,
                              stlInt64           * aRefCount,
                              knlEnv             * aEnv );

stlStatus knlFiniFlangeIterator( knlFlangeIterator * aIterator,
                                 knlEnv            * aEnv );

/** @} */
    
#endif /* _KNLPLANCACHE_H_ */
