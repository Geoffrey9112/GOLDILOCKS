/*******************************************************************************
 * knpcPlan.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knpcPlan.c 9033 2013-07-17 08:40:27Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file knpcPlan.c
 * @brief User Plan
 */

#include <knl.h>
#include <knDef.h>
#include <knpc.h>

extern knsEntryPoint * gKnEntryPoint;

/**
 * @brief Flange를 Empty로 만든다.
 * @param[in]     aFlange    Flange Pointer
 * @param[in]     aPlan      Plan Pointer
 * @param[in,out] aEnv       environment 정보
 */
stlStatus knpcFreePlan( knpcFlange * aFlange,
                        knpcPlan   * aPlan,
                        knlEnv     * aEnv )
{
    knpcClock * sClock;

    sClock = (knpcClock*)(gKnEntryPoint->mClock);
    sClock = &sClock[aPlan->mClockId % gKnEntryPoint->mClockCount];
        
    /**
     * free plan memory
     */
    
    STL_TRY( knlFreeAllArena( aPlan->mPlanMem,
                              &sClock->mDynamicMem,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );

    if( aFlange != NULL )
    {
        aFlange->mPlanCount--;
    }

    /**
     * reset plan
     */
    
    aPlan->mIsUsed = STL_FALSE;
    aPlan->mClockId = -1;
    aPlan->mPlanMem = NULL;
    aPlan->mCodePlan = NULL;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Flange에 Plan을 추가한다.
 * @param[in]     aClock     Clock Pointer
 * @param[in]     aFlange    Flange Pointer
 * @param[in]     aPlanMem   Plan Memory Pointer
 * @param[in]     aCodePlan  Code Plan Pointer
 */
knpcPlan * knpcAddPlan( knpcClock    * aClock,
                        knpcFlange   * aFlange,
                        knlRegionMem * aPlanMem,
                        void         * aCodePlan )
{
    stlInt32   i;
    knpcPlan * sPlan = NULL;
    
    for( i = 0; i < KNPC_MAX_PLAN_COUNT; i++ )
    {
        if( aFlange->mPlans[i].mIsUsed == STL_FALSE )
        {
            sPlan = &(aFlange->mPlans[i]);
            sPlan->mIsUsed = STL_TRUE;
            sPlan->mPlanMem = knlGetFirstArena( aPlanMem );
            sPlan->mCodePlan = aCodePlan;
            sPlan->mClockId = aClock->mClockId;
            aFlange->mPlanCount++;
            break;
        }
    }
    
    return sPlan;
}


/**
 * @brief 같은 Plan이 있는지 검사한다.
 * @param[in]     aFlange       Flange Pointer
 * @param[in]     aCompareFunc  Plan Compare Function
 * @param[in]     aSqlInfo      SQL Information
 * @param[in]     aCodePlan     Code Plan Pointer
 */
stlBool knpcComparePlan( knpcFlange         * aFlange,
                         knlComparePlanFunc   aCompareFunc,
                         knlPlanSqlInfo     * aSqlInfo,
                         void               * aCodePlan )
{
    stlInt32   i;
    knpcPlan * sPlan = NULL;
    stlBool    sSamePlan = STL_FALSE;
    
    for( i = 0; i < KNPC_MAX_PLAN_COUNT; i++ )
    {
        if( aFlange->mPlans[i].mIsUsed == STL_TRUE )
        {
            sPlan = &(aFlange->mPlans[i]);

            sSamePlan = aCompareFunc( aSqlInfo, aCodePlan, sPlan->mCodePlan );

            if( sSamePlan == STL_TRUE )
            {
                break;
            }
        }
    }

    return sSamePlan;
}
