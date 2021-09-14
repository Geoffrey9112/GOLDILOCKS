/*******************************************************************************
 * knpcFxTable.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knpcFxTable.c 9151 2013-07-26 05:55:05Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <knl.h>
#include <knDef.h>
#include <knpc.h>

extern knsEntryPoint * gKnEntryPoint;

/**
 * @file knpcFxTable.c
 * @brief Fixed Tables for Plan Cache
 */

/**
 * @brief X$SQL_CACHE 을 위한 컬럼 정의
 * @remarks
 */
knlFxColumnDesc gKnpcSqlCacheColumnDesc[] =
{
    {
        "SQL_HANDLE",
        "sql handle",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knpcFxSqlCache, mSqlHandle ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "HASH_VALUE",
        "hash value",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knpcFxSqlCache, mHashValue ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "REF_COUNT",
        "reference count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knpcFxSqlCache, mRefCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "PLAN_COUNT",
        "cached plan count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knpcFxSqlCache, mPlanCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "CLOCK_ID",
        "clock identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knpcFxSqlCache, mClockId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "AGE",
        "sql age",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knpcFxSqlCache, mAge ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "SQL_STRING",
        "sql string",
        KNL_FXTABLE_DATATYPE_LONGVARCHAR_POINTER,
        STL_OFFSETOF( knpcFxSqlCache, mSqlString ),
        DTL_LONGVARYING_INIT_STRING_SIZE,
        STL_FALSE  /* nullable */
    },
    {
        "USER_ID",
        "user identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knpcFxSqlCache, mUserId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "SQL_TYPE",
        "sql type",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knpcFxSqlCache, mStmtType ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "BIND_COUNT",
        "bind count in sql",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knpcFxSqlCache, mBindCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "DROPPED",
        "dropped flag",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( knpcFxSqlCache, mDropped ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "LAST_ACCESS_TIME",
        "last access time",
        KNL_FXTABLE_DATATYPE_TIMESTAMP,
        STL_OFFSETOF( knpcFxSqlCache, mLastAccessTime ),
        STL_SIZEOF( stlTime ),
        STL_FALSE  /* nullable */
    },

    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};


/**
 * @brief X$SQL_CACHE Table의 open 함수
 * @param[in]     aStmt          Statement Pointer
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus knpcOpenSqlCacheCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv )
{
    knpcFxSqlCachePathCtrl * sPathCtrl;
    stlBool                  sIsTimedOut;
    stlInt32                 sState = 0;
    knpcClock              * sClock;

    sPathCtrl = (knpcFxSqlCachePathCtrl*)aPathCtrl;

    sClock = (knpcClock*)(gKnEntryPoint->mClock);
    
    STL_TRY( knlAcquireLatch( &(sClock->mLatch),
                              KNL_LATCH_MODE_SHARED,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              aEnv )
             == STL_SUCCESS );
    sState = 1;

    sPathCtrl->mClockId     = sClock->mClockId;
    sPathCtrl->mFlangeBlock = STL_RING_GET_FIRST_DATA( &(sClock->mBlockList) );
    sPathCtrl->mFlangeId    = 0;
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(sClock->mLatch),
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &(sClock->mLatch), KNL_ENV( aEnv ) );
    }

    return STL_FAILURE;
}


/**
 * @brief X$SQL_CACHE Table의 close 함수
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus knpcCloseSqlCacheCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief X$SQL_CACHE Table의 레코드 구축 함수
 * @param[in]     aDumpObjHandle Dump Object Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aValueList     값을 담을 공간
 * @param[in]     aBlockIdx      Block Idx
 * @param[out]    aTupleExist    레코드 존재 여부
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus knpcBuildRecordSqlCacheCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aEnv )
{
    knpcFxSqlCachePathCtrl * sPathCtrl;
    stlBool                  sIsTimedOut;
    stlInt32                 sState = 0;
    knpcClock              * sClock;
    knpcFlange             * sFlange;
    knpcFxSqlCache           sFxRecord;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl = (knpcFxSqlCachePathCtrl*)aPathCtrl;
    sClock = &(((knpcClock*)(gKnEntryPoint->mClock))[sPathCtrl->mClockId]);
    
    STL_TRY( knlAcquireLatch( &(sClock->mLatch),
                              KNL_LATCH_MODE_SHARED,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              aEnv )
             == STL_SUCCESS );
    sState = 1;

    /**
     * search target flange
     */

    while( 1 )
    {
        if( sPathCtrl->mFlangeId >= KNPC_MAX_FLANGE_COUNT )
        {
            /**
             * move to next flange block
             */
        
            sPathCtrl->mFlangeBlock = STL_RING_GET_NEXT_DATA( &(sClock->mBlockList),
                                                              sPathCtrl->mFlangeBlock );

            sPathCtrl->mFlangeId = 0;
            
            if( STL_RING_IS_HEADLINK( &(sClock->mBlockList),
                                      sPathCtrl->mFlangeBlock )
                == STL_TRUE )
            {
                /**
                 * move to next clock
                 */
        
                sPathCtrl->mClockId++;

                sState = 0;
                STL_TRY( knlReleaseLatch( &(sClock->mLatch),
                                          KNL_ENV( aEnv ) ) == STL_SUCCESS );

                /**
                 * validate clock
                 */
        
                STL_TRY_THROW( sPathCtrl->mClockId < gKnEntryPoint->mClockCount, RAMP_FINISH );

                sClock = &(((knpcClock*)(gKnEntryPoint->mClock))[sPathCtrl->mClockId]);
            
                STL_TRY( knlAcquireLatch( &(sClock->mLatch),
                                          KNL_LATCH_MODE_SHARED,
                                          KNL_LATCH_PRIORITY_NORMAL,
                                          STL_INFINITE_TIME,
                                          &sIsTimedOut,
                                          aEnv )
                         == STL_SUCCESS );
                sState = 1;

                sPathCtrl->mClockId     = sClock->mClockId;
                sPathCtrl->mFlangeBlock = STL_RING_GET_FIRST_DATA( &(sClock->mBlockList) );
                sPathCtrl->mFlangeId    = 0;
            }
        }

        sFlange = &(sPathCtrl->mFlangeBlock->mFlanges[sPathCtrl->mFlangeId]);

        if( (KNPC_IS_EMPTY_FLANGE( sFlange ) == STL_TRUE) ||
            (sFlange->mPlanCount == 0 ) )
        {
            /**
             * move to next flange
             */
        
            sPathCtrl->mFlangeId++;
        }
        else
        {
            /**
             * found target flange
             */
    
            break;
        }
    }
    
    /**
     * build record
     */

    sFxRecord.mSqlHandle = sFlange;
    sFxRecord.mHashValue = sFlange->mHashValue;
    sFxRecord.mRefCount  = sFlange->mRefCount;
    sFxRecord.mPlanCount = sFlange->mPlanCount;
    sFxRecord.mClockId   = sPathCtrl->mClockId;
    sFxRecord.mAge       = sFlange->mAge;
    sFxRecord.mDropped   = sFlange->mDropped;
    sFxRecord.mSqlString = sFlange->mSqlString;
    sFxRecord.mUserId    = sFlange->mUserInfo.mUserId;
    sFxRecord.mStmtType  = sFlange->mSqlInfo.mStmtType;
    sFxRecord.mBindCount = sFlange->mSqlInfo.mBindCount;
    sFxRecord.mLastAccessTime = sFlange->mLastFixTime;
    
    STL_TRY( knlBuildFxRecord( gKnpcSqlCacheColumnDesc,
                               (void*)&sFxRecord,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(sClock->mLatch),
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    sPathCtrl->mFlangeId++;
    
    *aTupleExist = STL_TRUE;
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &(sClock->mLatch), KNL_ENV( aEnv ) );
    }

    return STL_FAILURE;
}


/**
 * @brief X$SQL_CACHE Table 정의
 */
knlFxTableDesc gKnpcSqlCacheTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    knpcOpenSqlCacheCallback,
    knpcCloseSqlCacheCallback,
    knpcBuildRecordSqlCacheCallback,
    STL_SIZEOF( knpcFxSqlCachePathCtrl ),
    "X$SQL_CACHE",
    "sql cache",
    gKnpcSqlCacheColumnDesc
};


/**
 * @brief X$PLAN_CLOCK 을 위한 컬럼 정의
 * @remarks
 */
knlFxColumnDesc gKnpcPlanClockColumnDesc[] =
{
    {
        "CLOCK_ID",
        "clock identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knpcFxPlanClock, mClockId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "FLANGE_ALLOC_SIZE",
        "used memory size for flange",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knpcFxPlanClock, mFlangeAllocSize ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "FLANGE_COUNT",
        "total flange count in a clock",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knpcFxPlanClock, mFlangeCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "FREE_FLANGE_COUNT",
        "free flange count in a clock",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knpcFxPlanClock, mFreeFlangeCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "PLAN_ALLOC_SIZE",
        "used memory size for plan",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knpcFxPlanClock, mPlanAllocSize ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "PLAN_COUNT",
        "total plan count in a clock",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knpcFxPlanClock, mPlanCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};


/**
 * @brief X$PLAN_CLOCK Table의 open 함수
 * @param[in]     aStmt          Statement Pointer
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus knpcOpenPlanClockCallback( void   * aStmt,
                                     void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    knpcFxPlanClockPathCtrl * sPathCtrl;
    stlBool                   sIsTimedOut;
    stlInt32                  sState = 0;
    knpcClock               * sClock;

    sPathCtrl = (knpcFxPlanClockPathCtrl*)aPathCtrl;

    sClock = (knpcClock*)(gKnEntryPoint->mClock);
    
    STL_TRY( knlAcquireLatch( &(sClock->mLatch),
                              KNL_LATCH_MODE_SHARED,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              aEnv )
             == STL_SUCCESS );
    sState = 1;

    sPathCtrl->mClockId = sClock->mClockId;
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(sClock->mLatch),
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &(sClock->mLatch), KNL_ENV( aEnv ) );
    }

    return STL_FAILURE;
}


/**
 * @brief X$PLAN_CLOCK Table의 close 함수
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus knpcClosePlanClockCallback( void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief X$PLAN_CLOCK Table의 레코드 구축 함수
 * @param[in]     aDumpObjHandle Dump Object Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aValueList     값을 담을 공간
 * @param[in]     aBlockIdx      Block Idx
 * @param[out]    aTupleExist    레코드 존재 여부
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus knpcBuildRecordPlanClockCallback( void              * aDumpObjHandle,
                                            void              * aPathCtrl,
                                            knlValueBlockList * aValueList,
                                            stlInt32            aBlockIdx,
                                            stlBool           * aTupleExist,
                                            knlEnv            * aEnv )
{
    knpcFxPlanClockPathCtrl * sPathCtrl;
    stlBool                   sIsTimedOut;
    stlInt32                  sState = 0;
    knpcClock               * sClock;
    knpcFlange              * sFlange;
    knpcFlangeBlock         * sFlangeBlock;
    knpcFxPlanClock           sFxRecord;
    stlInt32                  i = 0;
    stlInt64                  sFlangeCount = 0;
    stlInt64                  sPlanCount = 0;
    stlInt64                  sFreeFlangeCount = 0;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl = (knpcFxPlanClockPathCtrl*)aPathCtrl;
    
    STL_TRY_THROW( sPathCtrl->mClockId < gKnEntryPoint->mClockCount, RAMP_FINISH );

    sClock = &(((knpcClock*)(gKnEntryPoint->mClock))[sPathCtrl->mClockId]);
    
    STL_TRY( knlAcquireLatch( &(sClock->mLatch),
                              KNL_LATCH_MODE_SHARED,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              aEnv )
             == STL_SUCCESS );
    sState = 1;

    /**
     * search target flange
     */

    sFlangeBlock = STL_RING_GET_FIRST_DATA( &(sClock->mBlockList) );
    
    while( 1 )
    {
        if( STL_RING_IS_HEADLINK( &(sClock->mBlockList),
                                  sFlangeBlock )
            == STL_TRUE )
        {
            break;
        }

        sFlangeCount += KNPC_MAX_FLANGE_COUNT;
        
        for( i = 0; i < KNPC_MAX_FLANGE_COUNT; i++ )
        {
            sFlange = &(sFlangeBlock->mFlanges[i]);

            if( KNPC_IS_EMPTY_FLANGE( sFlange ) == STL_TRUE )
            {
                sFreeFlangeCount += 1;
            }
            else
            {
                sPlanCount += sFlange->mPlanCount;
            }
        }
        
        /**
         * move to next flange block
         */
        
        sFlangeBlock = STL_RING_GET_NEXT_DATA( &(sClock->mBlockList), sFlangeBlock );
    }

    /**
     * build record
     */

    sFxRecord.mClockId          = sPathCtrl->mClockId;
    sFxRecord.mFlangeAllocSize  = sClock->mFlangeDynamicMem.mTotalSize;
    sFxRecord.mFlangeCount      = sFlangeCount;
    sFxRecord.mFreeFlangeCount  = sFreeFlangeCount;
    sFxRecord.mPlanAllocSize    = sClock->mDynamicMem.mTotalSize;
    sFxRecord.mPlanCount        = sPlanCount;
    
    STL_TRY( knlBuildFxRecord( gKnpcPlanClockColumnDesc,
                               (void*)&sFxRecord,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(sClock->mLatch),
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    sPathCtrl->mClockId += 1;
    
    *aTupleExist = STL_TRUE;

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &(sClock->mLatch), KNL_ENV( aEnv ) );
    }

    return STL_FAILURE;
}


/**
 * @brief X$PLAN_CLOCK Table 정의
 */
knlFxTableDesc gKnpcPlanClockTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    knpcOpenPlanClockCallback,
    knpcClosePlanClockCallback,
    knpcBuildRecordPlanClockCallback,
    STL_SIZEOF( knpcFxPlanClockPathCtrl ),
    "X$PLAN_CLOCK",
    "plan information in clocks",
    gKnpcPlanClockColumnDesc
};
