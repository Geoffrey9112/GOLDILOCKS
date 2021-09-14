/*******************************************************************************
 * qlgFixedTable.c
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

/**
 * @file qlgFixedTable.c
 * @brief SQL layer 의 Fixed Table
 */

#include <qll.h>
#include <qlDef.h>

#include <qlpParserLex.h>
#include <qlgFixedTable.h>
#include <qlgStartup.h>


/**
 * @addtogroup qllFixedTable
 * @{
 */

/**
 * @brief SQL layer 의 Fixed Table 들을 등록한다.
 * @param[in]  aEnv  Environment
 */
stlStatus qlgRegisterFixedTables( qllEnv * aEnv )
{
    /**
     * X$SQL_SYSTEM_INFO Fixed Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gQlgFxSqlSystemInfoTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * X$SQL_PROC_ENV Fixed Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gQlgFxSqlProcEnvTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * X$SQL_SESS_ENV Fixed Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gQlgFxSqlSessEnvTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * X$SQL_SYSTEM_STAT_EXEC_STMT Fixed Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gQlgSystExecTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * X$SQL_PROC_STAT_EXEC_STMT Fixed Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gQlgProcExecTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * X$SQL_SESS_STAT_EXEC_STMT Fixed Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gQlgSessExecTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * X$SQL_KEYWORDS Fixed Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gQlgKeywordTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * X$BUILTIN_AGGREGATION Fixed Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gQlgFixedBuiltInAggrTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * X$FIXED_VIEW Fixed Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gQlgFixedViewTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * X$SQL_CACHE_PLAN Fixed Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gQlgSqlCachePlanTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} qllFixedTable */






/**
 * @defgroup qllFixedSystemInfo X$SQL_SYSTEM_INFO fixed table for SQL system environment
 * @ingroup qllFixedTable
 * @{
 */



/**
 * @brief X$SQL_SYSTEM_INFO 을 위한 여러 레코드 정의
 * @remarks
 */
knlFxSystemInfo gQlgSqlSystemInfoRows[QLG_SQL_SYSTEM_INFO_ROW_COUNT] =
{
    {
        "CALL_PARSE",
        0,
        KNL_STAT_CATEGORY_SQL,
        "total call count of parse function in the SQL processor layer"
    },
    {
        "CALL_VALIDATE",
        0,
        KNL_STAT_CATEGORY_SQL,
        "total call count of validate function in the SQL processor layer"
    },
    {
        "CALL_BINDCONTEXT",
        0,
        KNL_STAT_CATEGORY_SQL,
        "total call count of bind context function in the SQL processor layer"
    },
    {
        "CALL_OPTCODE",
        0,
        KNL_STAT_CATEGORY_SQL,
        "total call count of optimize code area function in the SQL processor layer"
    },
    {
        "CALL_OPTDATA",
        0,
        KNL_STAT_CATEGORY_SQL,
        "total call count of optimize data area function in the SQL processor layer",
    },
    {
        "CALL_CHECKPRIV",
        0,
        KNL_STAT_CATEGORY_SQL,
        "total call count of check execution privilege function in the SQL processor layer"
    },
    {
        "CALL_EXECUTE",
        0,
        KNL_STAT_CATEGORY_SQL,
        "total call count of execute function in the SQL processor layer"
    },
    {
        "CALL_RECOMPILE",
        0,
        KNL_STAT_CATEGORY_SQL,
        "total call count of recompile function in the SQL processor layer"
    },
    {
        "CALL_FETCH",
        0,
        KNL_STAT_CATEGORY_SQL,
        "total call count of fetch function in the SQL processor layer"
    }
};

/**
 * @brief X$SQL_SYSTEM_INFO Table의 open 함수
 * @param[in]     aStmt          Statement Pointer
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus openFxQlgSystemInfoCallback( void   * aStmt,
                                       void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aEnv )
{
    qlgFxSqlSystemInfoPathCtrl * sPathCtrl;
    stlInt64                   * sValues;

    knlSessionEnv           * sSessEnv;
    qllSessionEnv           * sQllSessEnv;
    
    sPathCtrl  = (qlgFxSqlSystemInfoPathCtrl *) aPathCtrl;
    sPathCtrl->mIterator = 0;

    sValues = sPathCtrl->mValues;

    /**********************************************************
     * 현재 누적된 System 정보
     **********************************************************/

    /**
     * CALL_PARSE
     */
    
    sValues[0] = gQllSharedEntry->mParsingCnt;

    /**
     * CALL_VALIDATE
     */
    
    sValues[1] = gQllSharedEntry->mValidateCnt;
    

    /**
     * CALL_BINDCONTEXT
     */

    sValues[2] = gQllSharedEntry->mBindContextCnt;
    
    /**
     * CALL_OPTCODE
     */

    sValues[3] = gQllSharedEntry->mOptCodeCnt;
    
    /**
     * CALL_OPTDATA
     */

    sValues[4] = gQllSharedEntry->mOptDataCnt;
    
    /**
     *  CALL_CHECKPRIV
     */

    sValues[5] = gQllSharedEntry->mRecheckCnt;
    
    /**
     *  CALL_EXECUTE
     */

    sValues[6] = gQllSharedEntry->mExecuteCnt;
    
    /**
     *  CALL_RECOMPILE
     */

    sValues[7] = gQllSharedEntry->mRecompileCnt;
    
    /**
     *  CALL_FETCH
     */

    sValues[8] = gQllSharedEntry->mFetchCnt;

    /**********************************************************
     * 유효한 Session 의 정보 추가
     * - System 통계 정보를 수행시마다 누적하는 것은
     * - Multi CPU 환경에서 Cache-Miss 로 Scalability 저하 요인이 된다.
     **********************************************************/
    
    sSessEnv = knlGetFirstSessionEnv();

    while( sSessEnv != NULL )
    {
        if( knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE )
        {
            sQllSessEnv = (qllSessionEnv *) sSessEnv;

            sValues[0] += sQllSessEnv->mParsingCnt;
            sValues[1] += sQllSessEnv->mValidateCnt;
            sValues[2] += sQllSessEnv->mBindContextCnt;
            sValues[3] += sQllSessEnv->mOptCodeCnt;
            sValues[4] += sQllSessEnv->mOptDataCnt;
            sValues[5] += sQllSessEnv->mRecheckCnt;
            sValues[6] += sQllSessEnv->mExecuteCnt;
            sValues[7] += sQllSessEnv->mRecompileCnt;
            sValues[8] += sQllSessEnv->mFetchCnt;
        }
        else
        {
            /* 유효한 Session 이 아님 */
        }

        sSessEnv = knlGetNextSessionEnv( sSessEnv );
    }
    
    return STL_SUCCESS;
}

/**
 * @brief X$SQL_SYSTEM_INFO Table의 close 함수
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus closeFxQlgSystemInfoCallback( void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief X$SQL_SYSTEM_INFO Table의 레코드 구축 함수
 * @param[in]     aDumpObjHandle Dump Object Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aValueList     값을 담을 공간
 * @param[in]     aBlockIdx      Block Idx
 * @param[out]    aTupleExist    레코드 존재 여부
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus buildRecordFxQlgSystemInfoCallback( void              * aDumpObjHandle,
                                              void              * aPathCtrl,
                                              knlValueBlockList * aValueList,
                                              stlInt32            aBlockIdx,
                                              stlBool           * aTupleExist,
                                              knlEnv            * aEnv )
{
    qlgFxSqlSystemInfoPathCtrl * sPathCtrl;
    knlFxSystemInfo              sFxSystemInfo;
    knlFxSystemInfo            * sRowDesc;
    stlInt64                   * sValues;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl = (qlgFxSqlSystemInfoPathCtrl *) aPathCtrl;
    sValues = sPathCtrl->mValues;

    sRowDesc = & gQlgSqlSystemInfoRows[sPathCtrl->mIterator];

    if( sPathCtrl->mIterator < QLG_SQL_SYSTEM_INFO_ROW_COUNT )
    {
        sFxSystemInfo.mName     = sRowDesc->mName;
        sFxSystemInfo.mValue    = sValues[sPathCtrl->mIterator];
        sFxSystemInfo.mComment  = sRowDesc->mComment;
        sFxSystemInfo.mCategory = sRowDesc->mCategory;

        STL_TRY( knlBuildFxRecord( gKnlSystemInfoColumnDesc,
                                   &sFxSystemInfo,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );

        *aTupleExist = STL_TRUE;
        sPathCtrl->mIterator += 1;
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$SQL_SYSTEM_INFO Table 정보
 */
knlFxTableDesc gQlgFxSqlSystemInfoTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxQlgSystemInfoCallback,
    closeFxQlgSystemInfoCallback,
    buildRecordFxQlgSystemInfoCallback,
    STL_SIZEOF( qlgFxSqlSystemInfoPathCtrl ),
    "X$SQL_SYSTEM_INFO",
    "system information of SQL processor layer",
    gKnlSystemInfoColumnDesc
};


/** @} qllFixedSystemInfo */








/**
 * @defgroup qllFixedProcEnv X$SQL_PROC_ENV fixed table for SQL process environment
 * @ingroup qllFixedTable
 * @{
 */


/**
 * @brief X$SQL_PROC_ENV 을 위한 여러 레코드 정의
 * @remarks
 */
knlFxSessionInfo gQlgSqlProcEnvRows[QLG_SQL_PROC_ENV_ROW_COUNT] =
{
    {
        "CALL_PARSE",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_VALIDATE",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_BINDCONTEXT",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_OPTCODE",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_OPTDATA",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_CHECKPRIV",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_EXECUTE",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_RECOMPILE",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_FETCH",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "SQL_HEAP_MEMORY_PARSING_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SQL_HEAP_MEMORY_PARSING_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    },
    {
        "SQL_HEAP_MEMORY_RUNNING_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SQL_HEAP_MEMORY_RUNNING_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    }
};

/**
 * @brief X$SQL_PROC_ENV Table의 open 함수
 * @param[in]     aStmt          Statement Pointer
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus openFxQlgProcEnvCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv )
{
    qlgFxSqlProcEnvPathCtrl * sPathCtrl;
    void                    * sProcEnv;
    qllEnv                  * sQllEnv;
    stlInt64                * sValues;

    sPathCtrl  = (qlgFxSqlProcEnvPathCtrl *) aPathCtrl;

    sPathCtrl->mCurrEnv = NULL;
    sPathCtrl->mIterator = 0;
    sValues = sPathCtrl->mValues;

    /**
     * 연결이 유효한 첫 Proc Env 에 대해 레코드들을 구축한다.
     */
    sProcEnv = knlGetFirstEnv();

    while( sProcEnv != NULL )
    {
        if( knlIsUsedEnv( sProcEnv ) == STL_TRUE )
        {
            STL_TRY( knlGetEnvId( sProcEnv,
                                  & sPathCtrl->mProcId )
                     == STL_SUCCESS );
            
            sPathCtrl->mCurrEnv = sProcEnv;
            sQllEnv = QLL_ENV(sProcEnv);

            /**
             * CALL_PARSE
             */
            
            sValues[0] = sQllEnv->mParsingCnt;

            /**
             * CALL_VALIDATE
             */
    
            sValues[1] = sQllEnv->mValidateCnt;
    

            /**
             * CALL_BINDCONTEXT
             */

            sValues[2] = sQllEnv->mBindContextCnt;
    
            /**
             * CALL_OPTCODE
             */

            sValues[3] = sQllEnv->mOptCodeCnt;
    
            /**
             * CALL_OPTDATA
             */

            sValues[4] = sQllEnv->mOptDataCnt;
    
            /**
             *  CALL_CHECKPRIV
             */

            sValues[5] = sQllEnv->mRecheckCnt;
    
            /**
             *  CALL_EXECUTE
             */

            sValues[6] = sQllEnv->mExecuteCnt;
    
            /**
             *  CALL_RECOMPILE
             */

            sValues[7] = sQllEnv->mRecompileCnt;
    
            /**
             *  CALL_FETCH
             */

            sValues[8] = sQllEnv->mFetchCnt;

            /**
             * SQL_HEAP_MEMORY_PARSING_TOTAL_SIZE
             */

            sValues[9] = sQllEnv->mHeapMemParsing.mTotalSize;

            /**
             * SQL_HEAP_MEMORY_PARSING_INIT_SIZE
             */

            sValues[10] = sQllEnv->mHeapMemParsing.mInitSize;

            /**
             * SQL_HEAP_MEMORY_RUNNING_TOTAL_SIZE
             */

            sValues[11] = sQllEnv->mHeapMemRunning.mTotalSize;

            /**
             * SQL_HEAP_MEMORY_RUNNING_INIT_SIZE
             */

            sValues[12] = sQllEnv->mHeapMemRunning.mInitSize;
            
            break;
        }
        else
        {
            sProcEnv = knlGetNextEnv( sProcEnv );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$SQL_PROC_ENV Table의 close 함수
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus closeFxQlgProcEnvCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief X$SQL_PROC_ENV Table의 레코드 구축 함수
 * @param[in]     aDumpObjHandle Dump Object Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aValueList     값을 담을 공간
 * @param[in]     aBlockIdx      Block Idx
 * @param[out]    aTupleExist    레코드 존재 여부
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus buildRecordFxQlgProcEnvCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aEnv )
{
    qlgFxSqlProcEnvPathCtrl  * sPathCtrl;
    knlFxSessionInfo           sFxProcEnvInfo;
    knlFxSessionInfo         * sRowDesc;
    stlInt64                 * sValues;
    void                     * sProcEnv;
    qllEnv                   * sQllEnv;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl = (qlgFxSqlProcEnvPathCtrl *) aPathCtrl;
    sValues = sPathCtrl->mValues;

    /**
     * 하나의 Proc Env 에 대한 모든 레코드들을 Fetch 했다면,
     * 다음 유효한 Proc Env 에 대해 레코드들을 구축 
     */
    if( sPathCtrl->mIterator >= QLG_SQL_PROC_ENV_ROW_COUNT )
    {
        sProcEnv = knlGetNextEnv( sPathCtrl->mCurrEnv );

        while( sProcEnv != NULL )
        {
            if( knlIsUsedEnv( sProcEnv ) == STL_TRUE )
            {
                STL_TRY( knlGetEnvId( sProcEnv,
                                      & sPathCtrl->mProcId )
                         == STL_SUCCESS );
                
                sPathCtrl->mCurrEnv = sProcEnv;
                sQllEnv = QLL_ENV(sProcEnv);
                
                /**
                 * CALL_PARSE
                 */
            
                sValues[0] = sQllEnv->mParsingCnt;

                /**
                 * CALL_VALIDATE
                 */
    
                sValues[1] = sQllEnv->mValidateCnt;
    

                /**
                 * CALL_BINDCONTEXT
                 */

                sValues[2] = sQllEnv->mBindContextCnt;
    
                /**
                 * CALL_OPTCODE
                 */

                sValues[3] = sQllEnv->mOptCodeCnt;
    
                /**
                 * CALL_OPTDATA
                 */

                sValues[4] = sQllEnv->mOptDataCnt;
    
                /**
                 *  CALL_CHECKPRIV
                 */

                sValues[5] = sQllEnv->mRecheckCnt;
    
                /**
                 *  CALL_EXECUTE
                 */

                sValues[6] = sQllEnv->mExecuteCnt;
    
                /**
                 *  CALL_RECOMPILE
                 */

                sValues[7] = sQllEnv->mRecompileCnt;
    
                /**
                 *  CALL_FETCH
                 */

                sValues[8] = sQllEnv->mFetchCnt;

                /**
                 * SQL_HEAP_MEMORY_PARSING_TOTAL_SIZE
                 */
                
                sValues[9] = sQllEnv->mHeapMemParsing.mTotalSize;
                
                /**
                 * SQL_HEAP_MEMORY_PARSING_INIT_SIZE
                 */
                
                sValues[10] = sQllEnv->mHeapMemParsing.mInitSize;
                
                /**
                 * SQL_HEAP_MEMORY_RUNNING_TOTAL_SIZE
                 */
                
                sValues[11] = sQllEnv->mHeapMemRunning.mTotalSize;
                
                /**
                 * SQL_HEAP_MEMORY_RUNNING_INIT_SIZE
                 */
                
                sValues[12] = sQllEnv->mHeapMemRunning.mInitSize;
                
                break;
            }
            else
            {
                sProcEnv = knlGetNextEnv( sProcEnv );
            }
        }

        sPathCtrl->mCurrEnv = sProcEnv;
        sPathCtrl->mIterator = 0;
    }
    else
    {
        /**
         * 해당 Session 의 레코드 정보가 아직 있음
         */
    }
    
    STL_TRY_THROW( sPathCtrl->mCurrEnv != NULL, RAMP_SUCCESS );

    sRowDesc = &gQlgSqlProcEnvRows[sPathCtrl->mIterator];

    sFxProcEnvInfo.mName     = sRowDesc->mName;
    sFxProcEnvInfo.mCategory = sRowDesc->mCategory;
    sFxProcEnvInfo.mSessId   = sPathCtrl->mProcId;
    sFxProcEnvInfo.mValue    = sValues[sPathCtrl->mIterator];

    STL_TRY( knlBuildFxRecord( gKnlSessionInfoColumnDesc,
                               &sFxProcEnvInfo,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;
    sPathCtrl->mIterator += 1;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief X$SQL_PROC_ENV Table 정보
 */
knlFxTableDesc gQlgFxSqlProcEnvTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxQlgProcEnvCallback,
    closeFxQlgProcEnvCallback,
    buildRecordFxQlgProcEnvCallback,
    STL_SIZEOF( qlgFxSqlProcEnvPathCtrl ),
    "X$SQL_PROC_ENV",
    "process environment information of SQL processor layer",
    gKnlSessionInfoColumnDesc
};


/** @} qllFixedProcEnv */








/**
 * @defgroup qllFixedSessEnv X$SQL_SESS_ENV fixed table for SQL session environment
 * @ingroup qllFixedTable
 * @{
 */


/**
 * @brief X$SQL_SESS_ENV 을 위한 여러 레코드 정의
 * @remarks
 */
knlFxSessionInfo gQlgSqlSessEnvRows[QLG_SQL_SESS_ENV_ROW_COUNT] =
{
    {
        "SQL_COLLISION_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SQL_COLLISION_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    },
    {
        "SQL_NLS_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SQL_NLS_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    },
    {
        "SQL_PARENT_DBC_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SQL_PARENT_DBC_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    },
    {
        "CALL_PARSE",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_VALIDATE",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_BINDCONTEXT",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_OPTCODE",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_OPTDATA",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_CHECKPRIV",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_EXECUTE",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_RECOMPILE",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    },
    {
        "CALL_FETCH",
        0,
        KNL_STAT_CATEGORY_SQL,
        0
    }
};



/**
 * @brief X$SQL_SESS_ENV Table의 open 함수
 * @param[in]     aStmt          Statement Pointer
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus openFxQlgSessEnvCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv )
{
    qlgFxSqlSessEnvPathCtrl * sPathCtrl;
    knlSessionEnv           * sSessEnv;
    qllSessionEnv           * sQllSessEnv;
    stlInt64                * sValues;

    sPathCtrl  = (qlgFxSqlSessEnvPathCtrl *) aPathCtrl;

    sPathCtrl->mCurrEnv = NULL;
    sPathCtrl->mIterator = 0;
    sValues = sPathCtrl->mValues;

    /**
     * 연결이 유효한 첫 Session 에 대해 레코드들을 구축한다.
     */
    
    sSessEnv = knlGetFirstSessionEnv();

    while( sSessEnv != NULL )
    {
        if( knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE )
        {
            STL_TRY( knlGetSessionEnvId( sSessEnv,
                                         & sPathCtrl->mSessId,
                                         aEnv )
                     == STL_SUCCESS );
            
            sPathCtrl->mCurrEnv = sSessEnv;
            sQllSessEnv = (qllSessionEnv *) sSessEnv;

            /**
             * SQL_COLLISION_MEMORY_TOTAL_SIZE
             */

            sValues[0] = sQllSessEnv->mMemCollision.mTotalSize;
            
            /**
             * SQL_COLLISION_MEMORY_INIT_SIZE
             */

            sValues[1] = sQllSessEnv->mMemCollision.mInitSize;

            /**
             * SQL_NLS_MEMORY_TOTAL_SIZE
             */

            sValues[2] = sQllSessEnv->mMemNLS.mTotalSize;
            
            /**
             * SQL_NLS_MEMORY_INIT_SIZE
             */

            sValues[3] = sQllSessEnv->mMemNLS.mInitSize;
            
            /**
             * SQL_PARENT_DBC_MEMORY_TOTAL_SIZE
             */

            sValues[4] = sQllSessEnv->mMemParentDBC.mTotalSize;
            
            /**
             * SQL_PARENT_DBC_MEMORY_INIT_SIZE
             */

            sValues[5] = sQllSessEnv->mMemParentDBC.mInitSize;
            

            /**
             * CALL_PARSE
             */
            
            sValues[6] = sQllSessEnv->mParsingCnt;

            /**
             * CALL_VALIDATE
             */
    
            sValues[7] = sQllSessEnv->mValidateCnt;
    

            /**
             * CALL_BINDCONTEXT
             */

            sValues[8] = sQllSessEnv->mBindContextCnt;
    
            /**
             * CALL_OPTCODE
             */

            sValues[9] = sQllSessEnv->mOptCodeCnt;
    
            /**
             * CALL_OPTDATA
             */

            sValues[10] = sQllSessEnv->mOptDataCnt;
    
            /**
             *  CALL_CHECKPRIV
             */

            sValues[11] = sQllSessEnv->mRecheckCnt;
    
            /**
             *  CALL_EXECUTE
             */

            sValues[12] = sQllSessEnv->mExecuteCnt;
    
            /**
             *  CALL_RECOMPILE
             */

            sValues[13] = sQllSessEnv->mRecompileCnt;
    
            /**
             *  CALL_FETCH
             */

            sValues[14] = sQllSessEnv->mFetchCnt;
            
            break;
        }
        else
        {
            sSessEnv = knlGetNextSessionEnv( sSessEnv );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$SQL_SESS_ENV Table의 close 함수
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus closeFxQlgSessEnvCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief X$SQL_SESS_ENV Table의 레코드 구축 함수
 * @param[in]     aDumpObjHandle Dump Object Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aValueList     값을 담을 공간
 * @param[in]     aBlockIdx      Block Idx
 * @param[out]    aTupleExist    레코드 존재 여부
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus buildRecordFxQlgSessEnvCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aEnv )
{
    qlgFxSqlSessEnvPathCtrl  * sPathCtrl;
    knlFxSessionInfo           sFxSessEnvInfo;
    knlFxSessionInfo         * sRowDesc;
    stlInt64                 * sValues;
    knlSessionEnv            * sSessEnv;
    qllSessionEnv            * sQllSessEnv;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl = (qlgFxSqlSessEnvPathCtrl *) aPathCtrl;
    sValues = sPathCtrl->mValues;

    /**
     * 하나의 Session Env 에 대한 모든 레코드들을 Fetch 했다면,
     * 다음 유효한 Session Env 에 대해 레코드들을 구축 
     */
    if( sPathCtrl->mIterator >= QLG_SQL_SESS_ENV_ROW_COUNT )
    {
        sSessEnv = knlGetNextSessionEnv( sPathCtrl->mCurrEnv );

        while( sSessEnv != NULL )
        {
            if( knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE )
            {
                STL_TRY( knlGetSessionEnvId( sSessEnv,
                                             & sPathCtrl->mSessId,
                                             aEnv )
                         == STL_SUCCESS );
                
                sPathCtrl->mCurrEnv = sSessEnv;
                sQllSessEnv = (qllSessionEnv *) sSessEnv;

                /**
                 * SQL_COLLISION_MEMORY_TOTAL_SIZE
                 */

                sValues[0] = sQllSessEnv->mMemCollision.mTotalSize;
            
                /**
                 * SQL_COLLISION_MEMORY_INIT_SIZE
                 */

                sValues[1] = sQllSessEnv->mMemCollision.mInitSize;

                /**
                 * SQL_NLS_MEMORY_TOTAL_SIZE
                 */

                sValues[2] = sQllSessEnv->mMemNLS.mTotalSize;
            
                /**
                 * SQL_NLS_MEMORY_INIT_SIZE
                 */

                sValues[3] = sQllSessEnv->mMemNLS.mInitSize;

                /**
                 * SQL_PARENT_DBC_MEMORY_TOTAL_SIZE
                 */
                
                sValues[4] = sQllSessEnv->mMemParentDBC.mTotalSize;
                
                /**
                 * SQL_PARENT_DBC_MEMORY_INIT_SIZE
                 */
                
                sValues[5] = sQllSessEnv->mMemParentDBC.mInitSize;
                
                /**
                 * CALL_PARSE
                 */
            
                sValues[6] = sQllSessEnv->mParsingCnt;

                /**
                 * CALL_VALIDATE
                 */
    
                sValues[7] = sQllSessEnv->mValidateCnt;
    

                /**
                 * CALL_BINDCONTEXT
                 */

                sValues[8] = sQllSessEnv->mBindContextCnt;
    
                /**
                 * CALL_OPTCODE
                 */

                sValues[9] = sQllSessEnv->mOptCodeCnt;
    
                /**
                 * CALL_OPTDATA
                 */

                sValues[10] = sQllSessEnv->mOptDataCnt;
    
                /**
                 *  CALL_CHECKPRIV
                 */

                sValues[11] = sQllSessEnv->mRecheckCnt;
    
                /**
                 *  CALL_EXECUTE
                 */

                sValues[12] = sQllSessEnv->mExecuteCnt;
    
                /**
                 *  CALL_RECOMPILE
                 */

                sValues[13] = sQllSessEnv->mRecompileCnt;
    
                /**
                 *  CALL_FETCH
                 */

                sValues[14] = sQllSessEnv->mFetchCnt;
                
                break;
            }
            else
            {
                sSessEnv = knlGetNextSessionEnv( sSessEnv );
            }
        }

        sPathCtrl->mCurrEnv = sSessEnv;
        sPathCtrl->mIterator = 0;
    }
    else
    {
        /**
         * 해당 Session 의 레코드 정보가 아직 있음
         */
    }
    
    STL_TRY_THROW( sPathCtrl->mCurrEnv != NULL, RAMP_SUCCESS );

    sRowDesc = &gQlgSqlSessEnvRows[sPathCtrl->mIterator];

    sFxSessEnvInfo.mName     = sRowDesc->mName;
    sFxSessEnvInfo.mCategory = sRowDesc->mCategory;
    sFxSessEnvInfo.mSessId   = sPathCtrl->mSessId;
    sFxSessEnvInfo.mValue    = sValues[sPathCtrl->mIterator];

    STL_TRY( knlBuildFxRecord( gKnlSessionInfoColumnDesc,
                               &sFxSessEnvInfo,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;
    sPathCtrl->mIterator += 1;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$SQL_SESS_ENV Table 정보
 */
knlFxTableDesc gQlgFxSqlSessEnvTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxQlgSessEnvCallback,
    closeFxQlgSessEnvCallback,
    buildRecordFxQlgSessEnvCallback,
    STL_SIZEOF( qlgFxSqlSessEnvPathCtrl ),
    "X$SQL_SESS_ENV",
    "session environment information of SQL processor layer",
    gKnlSessionInfoColumnDesc
};


/** @} qllFixedSessEnv */







/**
 * @defgroup qllFixedSystExecStmt X$SQL_SYSTEM_STAT_EXEC_STMT fixed table for SQL system
 * @ingroup qllFixedTable
 * @{
 */


/**
 * @brief X$SQL_SYSTEM_STAT_EXEC_STMT Table 정의
 */
knlFxColumnDesc gQlgSystExecColumnDesc[] =
{
    {
        "STMT_TYPE",
        "SQL statement type",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( qlgSystExecFxRecord, mStmtTypeName ),
        QLG_STMT_TYPE_NAME_SIZE + 1,
        STL_TRUE  /* nullable */
    },
    {
        "STMT_TYPE_ID",
        "SQL statement type ID",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( qlgSystExecFxRecord, mStmtTypeID ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "EXECUTE_COUNT",
        "total execution count of the SQL type in the SQL processor layer",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( qlgSystExecFxRecord, mStmtExecCnt ),
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
 * @brief X$SQL_SYSTEM_STAT_EXEC_STMT Table의 open 함수
 * @param[in]     aStmt          Statement Pointer
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus openFxQlgSystExecCallback( void   * aStmt,
                                     void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    knlSessionEnv * sSessEnv;
    qllSessionEnv * sQllSessEnv;

    qlgSystExecPathCtrl * sCtl = (qlgSystExecPathCtrl*) aPathCtrl;


    /**
     * 1 번 이상 수행된 Stmt Type 을 얻는다.
     */
    
    for ( sCtl->mStmtTypeID = 0;
          sCtl->mStmtTypeID < (QLL_STMT_MAX - QLL_STMT_NA);
          sCtl->mStmtTypeID++ )
    {
        /**
         * System 누적 정보를 확인
         */
        
        sCtl->mStmtExecCnt = gQllSharedEntry->mExecStmtCnt[sCtl->mStmtTypeID];

        /**
         * 유효한 Session 정보를 확인
         */
        
        sSessEnv = knlGetFirstSessionEnv();
        
        while( sSessEnv != NULL )
        {
            if( knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE )
            {
                sQllSessEnv = (qllSessionEnv *) sSessEnv;
                
                sCtl->mStmtExecCnt += sQllSessEnv->mExecStmtCnt[sCtl->mStmtTypeID];
            }
            else
            {
                /* 유효한 Session 이 아님 */
            }

            sSessEnv = knlGetNextSessionEnv( sSessEnv );
        }

        if ( sCtl->mStmtExecCnt > 0 )
        {
            break;
        }
    }

    return STL_SUCCESS;
}

/**
 * @brief X$SQL_SYSTEM_STAT_EXEC_STMT Table의 close 함수
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus closeFxQlgSystExecCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief X$SQL_SYSTEM_STAT_EXEC_STMT Table의 레코드 구축 함수
 * @param[in]     aDumpObjHandle Dump Object Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aValueList     값을 담을 공간
 * @param[in]     aBlockIdx      Block Idx
 * @param[out]    aTupleExist    레코드 존재 여부
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus buildRecordFxQlgSystExecCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aEnv )
{
    knlSessionEnv * sSessEnv;
    qllSessionEnv * sQllSessEnv;

    qlgSystExecPathCtrl * sCtl = (qlgSystExecPathCtrl*) aPathCtrl;
    qlgSystExecFxRecord   sRec;

    
    stlMemset( &sRec, 0x00, STL_SIZEOF(qlgSystExecFxRecord) );
    
    *aTupleExist = STL_FALSE;

    if( sCtl->mStmtTypeID < (QLL_STMT_MAX - QLL_STMT_NA) )
    {
        /**
         * Fixed Record 구성 
         */
        
        sRec.mStmtTypeID   = sCtl->mStmtTypeID;
        sRec.mStmtTypeName = qlgGetStmtTypeKeyString( sCtl->mStmtTypeID );
        sRec.mStmtExecCnt  = sCtl->mStmtExecCnt;
        
        /**
         * Fixed Record 구축
         */
        
        STL_TRY( knlBuildFxRecord( gQlgSystExecColumnDesc,
                                   & sRec,
                                   aValueList,
                                   aBlockIdx,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );

        *aTupleExist = STL_TRUE;

        /**
         * 다음 Fixed Record 위치 설정
         */
        
        for ( sCtl->mStmtTypeID = sCtl->mStmtTypeID + 1;
              sCtl->mStmtTypeID < (QLL_STMT_MAX - QLL_STMT_NA);
              sCtl->mStmtTypeID++ )
        {
            /**
             * System 누적 정보를 확인
             */
            
            sCtl->mStmtExecCnt = gQllSharedEntry->mExecStmtCnt[sCtl->mStmtTypeID];
            
            /**
             * 유효한 Session 정보를 확인
             */
            
            sSessEnv = knlGetFirstSessionEnv();
            
            while( sSessEnv != NULL )
            {
                if( knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE )
                {
                    sQllSessEnv = (qllSessionEnv *) sSessEnv;
                    
                    sCtl->mStmtExecCnt += sQllSessEnv->mExecStmtCnt[sCtl->mStmtTypeID];
                }
                else
                {
                    /* 유효한 Session 이 아님 */
                }

                sSessEnv = knlGetNextSessionEnv( sSessEnv );
            }

            if ( sCtl->mStmtExecCnt > 0 )
            {
                break;
            }
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$SQL_SYSTEM_STAT_EXEC_STMT Table 정보
 */
knlFxTableDesc gQlgSystExecTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxQlgSystExecCallback,
    closeFxQlgSystExecCallback,
    buildRecordFxQlgSystExecCallback,
    STL_SIZEOF( qlgSystExecFxRecord ),
    "X$SQL_SYSTEM_STAT_EXEC_STMT",
    "system statistic information about executed SQL statement of SQL processor layer",
    gQlgSystExecColumnDesc
};



/** @} qllFixedSystExecStmt */








/**
 * @defgroup qllFixedProcExecStmt X$SQL_PROC_STAT_EXEC_STMT fixed table for SQL process env
 * @ingroup qllFixedTable
 * @{
 */


/**
 * @brief X$SQL_PROC_STAT_EXEC_STMT Table 정의
 */
knlFxColumnDesc gQlgProcExecColumnDesc[] =
{
    {
        "ID",
        "SQL process environment ID",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( qlgProcExecFxRecord, mID ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "STATE",
        "process environment state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( qlgProcExecFxRecord, mState ),
        QLG_STATE_STRING_SIZE + 1,
        STL_TRUE  /* nullable */
    },
    {
        "STMT_TYPE",
        "SQL statement type",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( qlgProcExecFxRecord, mStmtTypeName ),
        QLG_STMT_TYPE_NAME_SIZE + 1,
        STL_TRUE  /* nullable */
    },
    {
        "STMT_TYPE_ID",
        "SQL statement type ID",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( qlgProcExecFxRecord, mStmtTypeID ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "EXECUTE_COUNT",
        "total execution count of the SQL type in the SQL process environment",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( qlgProcExecFxRecord, mStmtExecCnt ),
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
 * @brief X$SQL_PROC_STAT_EXEC_STMT Table의 open 함수
 * @param[in]     aStmt          Statement Pointer
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus openFxQlgProcExecCallback( void   * aStmt,
                                     void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    qlgProcExecPathCtrl * sCtl = (qlgProcExecPathCtrl*) aPathCtrl;

    /**
     * 1 번 이상 수행된 Stmt Type 을 얻는다.
     */
    
    for ( sCtl->mProcEnv = knlGetFirstEnv();
          sCtl->mProcEnv != NULL;
          sCtl->mProcEnv = knlGetNextEnv( (void*) sCtl->mProcEnv ) )
    {
        for ( sCtl->mStmtTypeID = 0;
              sCtl->mStmtTypeID < (QLL_STMT_MAX - QLL_STMT_NA);
              sCtl->mStmtTypeID++ )
        {
            if ( ((qllEnv*)sCtl->mProcEnv)->mExecStmtCnt[sCtl->mStmtTypeID] > 0 )
            {
                break;
            }
        }

        if ( sCtl->mStmtTypeID < (QLL_STMT_MAX - QLL_STMT_NA) )
        {
            break;
        }
    }
    
    return STL_SUCCESS;
}

/**
 * @brief X$SQL_PROC_STAT_EXEC_STMT Table의 close 함수
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus closeFxQlgProcExecCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief X$SQL_PROC_STAT_EXEC_STMT Table의 레코드 구축 함수
 * @param[in]     aDumpObjHandle Dump Object Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aValueList     값을 담을 공간
 * @param[in]     aBlockIdx      Block Idx
 * @param[out]    aTupleExist    레코드 존재 여부
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus buildRecordFxQlgProcExecCallback( void              * aDumpObjHandle,
                                            void              * aPathCtrl,
                                            knlValueBlockList * aValueList,
                                            stlInt32            aBlockIdx,
                                            stlBool           * aTupleExist,
                                            knlEnv            * aEnv )
{
    qlgProcExecPathCtrl * sCtl = (qlgProcExecPathCtrl*) aPathCtrl;
    qlgProcExecFxRecord   sRec;

    knlEnv             * sKnProcEnv = NULL;
    qllEnv             * sQlProcEnv = NULL;

    stlMemset( &sRec, 0x00, STL_SIZEOF(qlgProcExecFxRecord) );
    
    *aTupleExist = STL_FALSE;

    if ( sCtl->mProcEnv != NULL )
    {
        sKnProcEnv = (knlEnv*) sCtl->mProcEnv;
        sQlProcEnv = (qllEnv*) sCtl->mProcEnv;

        /**
         * Fixed Record 구성 
         */

        STL_TRY( knlGetEnvId( (void*) sKnProcEnv, &(sRec.mID) ) == STL_SUCCESS );
        
        knlGetEnvStateString( sKnProcEnv,
                              sRec.mState,
                              QLG_STATE_STRING_SIZE );

        sRec.mStmtTypeID   = sCtl->mStmtTypeID;
        sRec.mStmtTypeName = qlgGetStmtTypeKeyString( sCtl->mStmtTypeID );
        sRec.mStmtExecCnt  = sQlProcEnv->mExecStmtCnt[sCtl->mStmtTypeID];
        
        /**
         * Fixed Record 구축
         */
        
        STL_TRY( knlBuildFxRecord( gQlgProcExecColumnDesc,
                                   & sRec,
                                   aValueList,
                                   aBlockIdx,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );

        *aTupleExist = STL_TRUE;

        /**
         * 다음 Fixed Record 위치 설정
         */
        
        for ( ;
              sCtl->mProcEnv != NULL;
              sCtl->mProcEnv = knlGetNextEnv( (void*) sCtl->mProcEnv ) )
        {
            for ( sCtl->mStmtTypeID = sCtl->mStmtTypeID + 1;
                  sCtl->mStmtTypeID < (QLL_STMT_MAX - QLL_STMT_NA);
                  sCtl->mStmtTypeID++ )
            {
                if ( ((qllEnv*)sCtl->mProcEnv)->mExecStmtCnt[sCtl->mStmtTypeID] > 0 )
                {
                    break;
                }
            }

            if ( sCtl->mStmtTypeID < (QLL_STMT_MAX - QLL_STMT_NA) )
            {
                break;
            }
            else
            {
                sCtl->mStmtTypeID = 0;
            }
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$SQL_PROC_STAT_EXEC_STMT Table 정보
 */
knlFxTableDesc gQlgProcExecTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxQlgProcExecCallback,
    closeFxQlgProcExecCallback,
    buildRecordFxQlgProcExecCallback,
    STL_SIZEOF( qlgProcExecFxRecord ),
    "X$SQL_PROC_STAT_EXEC_STMT",
    "process statistic information about executed SQL statement of SQL processor layer",
    gQlgProcExecColumnDesc
};



/** @} qllFixedProcExecStmt */








/**
 * @defgroup qllFixedSessExecStmt X$SQL_SESS_STAT_EXEC_STMT fixed table for SQL session env
 * @ingroup qllFixedTable
 * @{
 */


/**
 * @brief X$SQL_SESS_STAT_EXEC_STMT Table 정의
 */
knlFxColumnDesc gQlgSessExecColumnDesc[] =
{
    {
        "ID",
        "SQL session environment ID",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( qlgSessExecFxRecord, mID ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "STATE",
        "session environment state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( qlgSessExecFxRecord, mState ),
        QLG_STATE_STRING_SIZE + 1,
        STL_TRUE  /* nullable */
    },
    {
        "STMT_TYPE",
        "SQL statement type",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( qlgSessExecFxRecord, mStmtTypeName ),
        QLG_STMT_TYPE_NAME_SIZE + 1,
        STL_TRUE  /* nullable */
    },
    {
        "STMT_TYPE_ID",
        "SQL statement type ID",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( qlgSessExecFxRecord, mStmtTypeID ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "EXECUTE_COUNT",
        "total execution count of the SQL type in the SQL session environment",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( qlgSessExecFxRecord, mStmtExecCnt ),
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
 * @brief X$SQL_SESS_STAT_EXEC_STMT Table의 open 함수
 * @param[in]     aStmt          Statement Pointer
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus openFxQlgSessExecCallback( void   * aStmt,
                                     void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    qlgSessExecPathCtrl * sCtl = (qlgSessExecPathCtrl*) aPathCtrl;

    /**
     * 1 번 이상 수행된 Stmt Type 을 얻는다.
     */
    
    for ( sCtl->mSessEnv = knlGetFirstSessionEnv();
          sCtl->mSessEnv != NULL;
          sCtl->mSessEnv = knlGetNextSessionEnv( (void*) sCtl->mSessEnv ) )
    {
        for ( sCtl->mStmtTypeID = 0;
              sCtl->mStmtTypeID < (QLL_STMT_MAX - QLL_STMT_NA);
              sCtl->mStmtTypeID++ )
        {
            if ( ((qllSessionEnv*)sCtl->mSessEnv)->mExecStmtCnt[sCtl->mStmtTypeID] > 0 )
            {
                break;
            }
        }

        if ( sCtl->mStmtTypeID < (QLL_STMT_MAX - QLL_STMT_NA) )
        {
            break;
        }
    }
    
    return STL_SUCCESS;
}

/**
 * @brief X$SQL_SESS_STAT_EXEC_STMT Table의 close 함수
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus closeFxQlgSessExecCallback( void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief X$SQL_SESS_STAT_EXEC_STMT Table의 레코드 구축 함수
 * @param[in]     aDumpObjHandle Dump Object Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aValueList     값을 담을 공간
 * @param[in]     aBlockIdx      Block Idx
 * @param[out]    aTupleExist    레코드 존재 여부
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus buildRecordFxQlgSessExecCallback( void              * aDumpObjHandle,
                                            void              * aPathCtrl,
                                            knlValueBlockList * aValueList,
                                            stlInt32            aBlockIdx,
                                            stlBool           * aTupleExist,
                                            knlEnv            * aEnv )
{
    qlgSessExecPathCtrl * sCtl = (qlgSessExecPathCtrl*) aPathCtrl;
    qlgSessExecFxRecord   sRec;

    knlSessionEnv    * sKnSessEnv = NULL;
    qllSessionEnv    * sQlSessEnv = NULL;

    stlMemset( &sRec, 0x00, STL_SIZEOF(qlgSessExecFxRecord) );
    
    *aTupleExist = STL_FALSE;

    if ( sCtl->mSessEnv != NULL )
    {
        sKnSessEnv = (knlSessionEnv*) sCtl->mSessEnv;
        sQlSessEnv = (qllSessionEnv*) sCtl->mSessEnv;

        /**
         * Fixed Record 구성 
         */

        STL_TRY( knlGetSessionEnvId( (void*) sKnSessEnv,
                                     &(sRec.mID),
                                     aEnv ) == STL_SUCCESS );
        
        if( knlIsUsedSessionEnv(sKnSessEnv) == STL_TRUE )
        {
            if( knlIsDeadSessionEnv(sKnSessEnv) == STL_FALSE )
            {
                if( knlIsTerminatedSessionEnv(sKnSessEnv) == STL_TRUE )
                {
                    stlStrcpy( sRec.mState, "USED | TERMINATED" );
                }
                else
                {
                    stlStrcpy( sRec.mState, "USED | ALIVE" );
                }
            }
            else
            {
                stlStrcpy( sRec.mState, "DEAD" );
            }
        }
        else
        {
            stlStrcpy( sRec.mState, "UNUSED" );
        }
        
        sRec.mStmtTypeID   = sCtl->mStmtTypeID;
        sRec.mStmtTypeName = qlgGetStmtTypeKeyString( sCtl->mStmtTypeID );
        sRec.mStmtExecCnt  = sQlSessEnv->mExecStmtCnt[sCtl->mStmtTypeID];
        
        /**
         * Fixed Record 구축
         */
        
        STL_TRY( knlBuildFxRecord( gQlgSessExecColumnDesc,
                                   & sRec,
                                   aValueList,
                                   aBlockIdx,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );

        *aTupleExist = STL_TRUE;

        /**
         * 다음 Fixed Record 위치 설정
         */
        
        for ( ;
              sCtl->mSessEnv != NULL;
              sCtl->mSessEnv = knlGetNextSessionEnv( (void*) sCtl->mSessEnv ) )
        {
            for ( sCtl->mStmtTypeID = sCtl->mStmtTypeID + 1;
                  sCtl->mStmtTypeID < (QLL_STMT_MAX - QLL_STMT_NA);
                  sCtl->mStmtTypeID++ )
            {
                if ( ((qllSessionEnv*)sCtl->mSessEnv)->mExecStmtCnt[sCtl->mStmtTypeID] > 0 )
                {
                    break;
                }
            }

            if ( sCtl->mStmtTypeID < (QLL_STMT_MAX - QLL_STMT_NA) )
            {
                break;
            }
            else
            {
                sCtl->mStmtTypeID = 0;
            }
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$SQL_SESS_STAT_EXEC_STMT Table 정보
 */
knlFxTableDesc gQlgSessExecTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxQlgSessExecCallback,
    closeFxQlgSessExecCallback,
    buildRecordFxQlgSessExecCallback,
    STL_SIZEOF( qlgSessExecFxRecord ),
    "X$SQL_SESS_STAT_EXEC_STMT",
    "session statistic information about executed SQL statement of SQL processor layer",
    gQlgSessExecColumnDesc
};




/** @} qllFixedSessExecStmt */











/**
 * @defgroup qllFixedSQLKeyword X$SQL_KEYWORDS fixed table
 * @ingroup qllFixedTable
 * @{
 */


/**
 * @brief X$SQL_KEYWORDS Table 정의
 */
knlFxColumnDesc gQlgKeywordColumnDesc[] =
{
    {
        "NAME",
        "SQL keyword",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( qlgKeywordFxRecord, mName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "CATEGORY",
        "keyword category",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( qlgKeywordFxRecord, mCategory ),
        STL_SIZEOF( stlInt32 ),
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
 * @brief X$SQL_KEYWORDS Table의 open 함수
 * @param[in]     aStmt          Statement Pointer
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus openFxQlgKeywordCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv )
{
    qlgKeywordPathCtrl * sCtl = (qlgKeywordPathCtrl*) aPathCtrl;

    sCtl->mCurrKeyIdx = 0;
    
    return STL_SUCCESS;
}

/**
 * @brief X$SQL_KEYWORDS Table의 close 함수
 * @param[in]     aDumpObjHandle Dump Object의 Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus closeFxQlgKeywordCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief X$SQL_KEYWORDS Table의 레코드 구축 함수
 * @param[in]     aDumpObjHandle Dump Object Handle
 * @param[in]     aPathCtrl      Path Controller
 * @param[in,out] aValueList     값을 담을 공간
 * @param[in]     aBlockIdx      Block Idx
 * @param[out]    aTupleExist    레코드 존재 여부
 * @param[in,out] aEnv           Kernel Environment
 */
stlStatus buildRecordFxQlgKeywordCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aEnv )
{
    qlgKeywordPathCtrl * sCtl = (qlgKeywordPathCtrl*) aPathCtrl;
    qlgKeywordFxRecord   sRec;

    const stlKeyWord  * sKeyword = NULL;

    stlMemset( &sRec, 0x00, STL_SIZEOF(qlgKeywordFxRecord) );

    if ( sCtl->mCurrKeyIdx < qlpParserKeyWordCount )
    {
        sKeyword = & qlpParserKeyWordList[sCtl->mCurrKeyIdx];
        
        /**
         * Fixed Record 구성 
         */
        
        sRec.mName     = sKeyword->mName;
        sRec.mCategory = sKeyword->mCategory;
        
        /**
         * Fixed Record 구축
         */
        
        STL_TRY( knlBuildFxRecord( gQlgKeywordColumnDesc,
                                   & sRec,
                                   aValueList,
                                   aBlockIdx,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );
        
        *aTupleExist = STL_TRUE;
        
        /**
         * 다음 Fixed Record 위치 설정
         */

        sCtl->mCurrKeyIdx++;
    }
    else
    {
        *aTupleExist = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$SQL_KEYWORDS Table 정보
 */
knlFxTableDesc gQlgKeywordTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxQlgKeywordCallback,
    closeFxQlgKeywordCallback,
    buildRecordFxQlgKeywordCallback,
    STL_SIZEOF( qlgKeywordFxRecord ),
    "X$SQL_KEYWORDS",
    "SQL reserved/non-reserved keywords",
    gQlgKeywordColumnDesc
};


/** @} qllFixedSQLKeyword */



/**
 * @defgroup qllFixedBuiltInAggregation X$BUILTIN_AGGREGATION fixed table
 * @ingroup qllFixedTable
 * @{
 */

/**
 * @brief X$BUILTIN_AGGREGATION 을 위한 컬럼 정의
 * @remarks
 */

knlFxColumnDesc gQlgFixedBuiltInAggrColDesc[] =
{
    {
        "FUNC_ID",
        "function identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( qlgFixedBuiltInAggr, mFuncID ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "FUNC_NAME",
        "function name",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( qlgFixedBuiltInAggr, mFuncName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "MIN_ARG_COUNT",
        "minimum argument count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( qlgFixedBuiltInAggr, mMinArgCnt ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "MAX_ARG_COUNT",
        "maximum argument count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( qlgFixedBuiltInAggr, mMaxArgCnt ),
        STL_SIZEOF( stlInt32 ),
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
 * @brief X$BUILTIN_AGGREGATION 을 위한 Open Callback 함수 정의 
 * @remarks
 */
stlStatus qlgOpenFixedBuiltInAggrCallback( void   * aStmt,
                                           void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aKnlEnv )
{
    qlgFixedBuiltInAggrPathCtrl * sPathCtrl = NULL;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sPathCtrl = aPathCtrl;
    
    /**
     * 첫번째 Record를 구성하기 위해 필요한 정보를 설정한다
     */
    
    sPathCtrl->mNO = KNL_BUILTIN_AGGREGATION_INVALID + 1;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$BUILTIN_AGGREGATION 을 위한 Close Callback 함수 정의 
 * @remarks
 */
stlStatus qlgCloseFixedBuiltInAggrCallback( void   * aDumpObjHandle,
                                            void   * aPathCtrl,
                                            knlEnv * aKnlEnv )
{
    /**
     * 할 일 없음
     */
    
    return STL_SUCCESS;
}


/**
 * @brief X$BUILTIN_AGGREGATION 을 위한 BuildRecord 함수 정의 
 * @remarks
 */
stlStatus qlgBuildRecordBuiltInAggrCallback( void              * aDumpObjHandle,
                                             void              * aPathCtrl,
                                             knlValueBlockList * aValueList,
                                             stlInt32            aBlockIdx,
                                             stlBool           * aIsExist,
                                             knlEnv            * aKnlEnv )
{
    qlnfAggrFuncInfo             * sBuiltInFunc  = NULL;
    qlgFixedBuiltInAggrPathCtrl  * sPathCtrl = NULL;

    qlgFixedBuiltInAggr   sXBuiltInRecord;
    stlMemset( & sXBuiltInRecord, 0x00, STL_SIZEOF(qlgFixedBuiltInAggr) );

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aIsExist != NULL, KNL_ERROR_STACK(aKnlEnv) );
    sPathCtrl = aPathCtrl;

    /**
     * 지정된 위치가 종료 위치인지 검사
     */
    
    if ( sPathCtrl->mNO == KNL_BUILTIN_AGGREGATION_UNARY_MAX )
    {
        /**
         * 종료 지점일 경우 구성할 레코드가 없음을 알림
         */
        *aIsExist = STL_FALSE;
    }
    else
    {
        /**
         * 유효한 정보가 있으면 레코드를 위한 정보를 구성하고 
         */

        sBuiltInFunc = & gQlnfAggrFuncInfoArr[sPathCtrl->mNO];

        sXBuiltInRecord.mFuncID           = sBuiltInFunc->mFuncID;
        sXBuiltInRecord.mFuncName         = (stlChar *) sBuiltInFunc->mName;
        sXBuiltInRecord.mMinArgCnt        = sBuiltInFunc->mArgumentCntMin;
        sXBuiltInRecord.mMaxArgCnt        = sBuiltInFunc->mArgumentCntMax;
        
        /**
         * Value List를 생성한 후
         */
        STL_TRY( knlBuildFxRecord( gQlgFixedBuiltInAggrColDesc,
                                   & sXBuiltInRecord,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        *aIsExist = STL_TRUE;
        
        /**
         * 다음 레코드를 구성할 위치를 지정함.
         */

        sPathCtrl->mNO++;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$BUILTIN_AGGREGATION 을 위한 테이블 정의
 * @remarks
 */

knlFxTableDesc gQlgFixedBuiltInAggrTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,               /**< Basic Fixed Table 임 */
    KNL_STARTUP_PHASE_NO_MOUNT,                  /**< NO_MOUNT 단계부터 조회 가능 */
    NULL,                                        /**< Dump Object Handle 획득 함수 */
    qlgOpenFixedBuiltInAggrCallback,             /**< Open Callback */
    qlgCloseFixedBuiltInAggrCallback,            /**< Close Callback */
    qlgBuildRecordBuiltInAggrCallback,           /**< BuildRecord Callback */
    STL_SIZEOF(qlgFixedBuiltInAggrPathCtrl),     /**< Path Controller Size */
    "X$BUILTIN_AGGREGATION",                     /**< Table Name */
    "supported built-in aggregate functions",    /**< Table Comment */
    gQlgFixedBuiltInAggrColDesc                  /**< 컬럼 정보 */
};


/** @} qllFixedBuiltInAggregation */





/**
 * @defgroup qllFixedView X$FIXED_VIEW fixed table
 * @ingroup qllFixedTable
 * @{
 */

/**
 * @brief X$FIXED_VIEW 을 위한 컬럼 정의
 * @remarks
 */

knlFxColumnDesc gQlgFixedViewColDesc[] =
{
    {
        "VIEW_NAME",
        "performance view name",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( qlgFixedView, mViewName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "STARTUP_PHASE",
        "startup phase name",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( qlgFixedView, mStartupPhase ),
        32,
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
 * @brief X$FIXED_VIEW 을 위한 Open Callback 함수 정의 
 * @remarks
 */
stlStatus qlgOpenFixedViewCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aKnlEnv )
{
    qlgFixedViewPathCtrl * sPathCtrl = NULL;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sPathCtrl = aPathCtrl;

    /**
     * 첫번째 Record를 구성하기 위해 필요한 정보를 설정한다
     */

    sPathCtrl->mStartupPhase = KNL_STARTUP_PHASE_NO_MOUNT;
    sPathCtrl->mNO = 0;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$FIXED_VIEW 을 위한 Close Callback 함수 정의 
 * @remarks
 */
stlStatus qlgCloseFixedViewCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aKnlEnv )
{
    /**
     * 할 일 없음
     */
    
    return STL_SUCCESS;
}


/**
 * @brief X$FIXED_VIEW 을 위한 BuildRecord 함수 정의 
 * @remarks
 */
stlStatus qlgBuildRecordFixedViewCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aIsExist,
                                           knlEnv            * aKnlEnv )
{
    qlgFixedViewPathCtrl  * sPathCtrl = NULL;

    qlgNonServicePerfView * sViewDefinition = NULL;
    
    qlgFixedView   sXBuiltInRecord;
    stlMemset( & sXBuiltInRecord, 0x00, STL_SIZEOF(qlgFixedView) );

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aIsExist != NULL, KNL_ERROR_STACK(aKnlEnv) );
    sPathCtrl = aPathCtrl;

    
    /**
     * 지정된 위치가 종료 위치인지 검사
     */

    if ( sPathCtrl->mNO == -1 )
    {
        /**
         * 종료 지점일 경우 구성할 레코드가 없음을 알림
         */
        *aIsExist = STL_FALSE;
    }
    else
    {
        /**
         * 유효한 정보가 있으면 레코드를 위한 정보를 구성하고 
         */

        if ( sPathCtrl->mStartupPhase == KNL_STARTUP_PHASE_NO_MOUNT )
        {
            sViewDefinition = gQlgNoMountPerfViewDefinition;
        }
        else
        {
            STL_DASSERT( sPathCtrl->mStartupPhase == KNL_STARTUP_PHASE_MOUNT );
            sViewDefinition = gQlgMountPerfViewDefinition;
        }

        sXBuiltInRecord.mViewName         = (stlChar *) sViewDefinition[sPathCtrl->mNO].mViewName;
        sXBuiltInRecord.mStartupPhase     = (stlChar *) gPhaseString[sPathCtrl->mStartupPhase];
    
        /**
         * Value List를 생성한 후
         */
        STL_TRY( knlBuildFxRecord( gQlgFixedViewColDesc,
                                   & sXBuiltInRecord,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        *aIsExist = STL_TRUE;
        
        /**
         * 다음 레코드를 구성할 위치를 지정함.
         */

        if ( sViewDefinition[sPathCtrl->mNO + 1].mViewName == NULL )
        {
            if ( sPathCtrl->mStartupPhase == KNL_STARTUP_PHASE_MOUNT )
            {
                sPathCtrl->mNO = -1;
            }
            else
            {
                sPathCtrl->mStartupPhase = KNL_STARTUP_PHASE_MOUNT;
                sPathCtrl->mNO = 0;
            }
        }
        else
        {
            sPathCtrl->mNO++;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$FIXED_VIEW 을 위한 테이블 정의
 * @remarks
 */

knlFxTableDesc gQlgFixedViewTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,        /**< Basic Fixed Table 임 */
    KNL_STARTUP_PHASE_NO_MOUNT,           /**< NO_MOUNT 단계부터 조회 가능 */
    NULL,                                 /**< Dump Object Handle 획득 함수 */
    qlgOpenFixedViewCallback,             /**< Open Callback */
    qlgCloseFixedViewCallback,            /**< Close Callback */
    qlgBuildRecordFixedViewCallback,      /**< BuildRecord Callback */
    STL_SIZEOF(qlgFixedViewPathCtrl),     /**< Path Controller Size */
    "X$FIXED_VIEW",                       /**< Table Name */
    "performance views in NOMOUNT, MOUNT phase",    /**< Table Comment */
    gQlgFixedViewColDesc                  /**< 컬럼 정보 */
};

/** @} qllFixedView */


/**
 * @defgroup qllSqlCachePlan X$SQL_CACHE_PLAN fixed table
 * @ingroup qllFixedTable
 * @{
 */

/**
 * @brief X$SQL_CACHE_PLAN 을 위한 컬럼 정의
 * @remarks
 */

knlFxColumnDesc gQlgSqlCachePlanColDesc[] =
{
    {
        "SQL_HANDLE",
        "sql handle",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( qlgSqlCachePlan, mSqlHandle ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "REF_COUNT",
        "reference count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( qlgSqlCachePlan, mRefCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "PLAN_IDX",
        "plan idx",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( qlgSqlCachePlan, mPlanIdx ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "PLAN_SIZE",
        "plan size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( qlgSqlCachePlan, mPlanSize ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_ATOMIC",
        "is atomic",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( qlgSqlCachePlan, mIsAtomic ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "HAS_HINT_ERROR",
        "has hint error",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( qlgSqlCachePlan, mHasHintError ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "BIND_COUNT",
        "bind count in sql",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( qlgSqlCachePlan, mBindCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "PLAN_STRING",
        "plan string",
        KNL_FXTABLE_DATATYPE_LONGVARCHAR_POINTER,
        STL_OFFSETOF( qlgSqlCachePlan, mPlanString ),
        DTL_LONGVARYING_INIT_STRING_SIZE,
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
 * @brief X$SQL_CACHE_PLAN 을 위한 Open Callback 함수 정의 
 * @remarks
 */
stlStatus qlgOpenSqlCachePlanCallback( void   * aStmt,
                                       void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aKnlEnv )
{
    qlgSqlCachePlanPathCtrl * sPathCtrl;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sPathCtrl = (qlgSqlCachePlanPathCtrl*)aPathCtrl;

    stlMemset( sPathCtrl, 0x00, STL_SIZEOF( qlgSqlCachePlanPathCtrl ) );


    /**
     * Flange Iterator 초기화
     */
    
    STL_TRY( knlInitFlangeIterator( & sPathCtrl->mFlangeIterator,
                                    aKnlEnv )
             == STL_SUCCESS );

    sPathCtrl->mSetFlangeIterator = STL_TRUE;
    
    
    /**
     * Next Flange 얻기
     */
    
    STL_TRY( knlFlangeFetchNext( & sPathCtrl->mFlangeIterator,
                                 & sPathCtrl->mSqlHandle,
                                 & sPathCtrl->mSqlHandleRefCount,
                                 aKnlEnv )
             == STL_SUCCESS );

    if( sPathCtrl->mSqlHandle != NULL )
    {
        /**
         * Plan 얻기
         */

        STL_TRY( knlInitPlanIterator( & sPathCtrl->mPlanIterator,
                                      sPathCtrl->mSqlHandle,
                                      aKnlEnv )
                 == STL_SUCCESS );

        sPathCtrl->mSetPlanIterator = STL_TRUE;
    }
    else
    {
        /**
         * Flange Iterator 종료
         */
        
        sPathCtrl->mSetFlangeIterator = STL_FALSE;

        STL_TRY( knlFiniFlangeIterator( & sPathCtrl->mFlangeIterator,
                                        aKnlEnv )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;
    
    STL_FINISH;

    if( sPathCtrl->mSetPlanIterator == STL_TRUE )
    {
        (void) knlFiniPlanIterator( & sPathCtrl->mPlanIterator,
                                    aKnlEnv );

        sPathCtrl->mSetPlanIterator = STL_FALSE;
    }

    if( sPathCtrl->mSetFlangeIterator == STL_TRUE )
    {
        (void) knlFiniFlangeIterator( & sPathCtrl->mFlangeIterator,
                                      aKnlEnv );

        sPathCtrl->mSetFlangeIterator = STL_FALSE;
    }

    return STL_FAILURE;
}

/**
 * @brief X$SQL_CACHE_PLAN 을 위한 Close Callback 함수 정의 
 * @remarks
 */
stlStatus qlgCloseSqlCachePlanCallback( void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aKnlEnv )
{
    qlgSqlCachePlanPathCtrl  * sPathCtrl;

    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sPathCtrl = (qlgSqlCachePlanPathCtrl*)aPathCtrl;


    /**
     * Plan Iterator 종료
     */

    if( sPathCtrl->mSetPlanIterator == STL_TRUE )
    {
        (void) knlFiniPlanIterator( & sPathCtrl->mPlanIterator,
                                    aKnlEnv );

        sPathCtrl->mSetPlanIterator = STL_FALSE;
    }


    /**
     * Flange Iterator 종료
     */

    if( sPathCtrl->mSetFlangeIterator == STL_TRUE )
    {
        (void) knlFiniFlangeIterator( & sPathCtrl->mFlangeIterator,
                                      aKnlEnv );

        sPathCtrl->mSetFlangeIterator = STL_FALSE;
    }


    /**
     * Unmark Region Memory
     */

    if( sPathCtrl->mSetMemMark == STL_TRUE )
    {
        (void) knlFreeUnmarkedRegionMem( & QLL_CANDIDATE_MEM( QLL_ENV( aKnlEnv ) ),
                                         & sPathCtrl->mRegionMemMark,
                                         STL_TRUE, /* aFreeChunk */
                                         aKnlEnv );

        sPathCtrl->mSetMemMark = STL_FALSE;
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief X$SQL_CACHE_PLAN 을 위한 BuildRecord 함수 정의 
 * @remarks
 */
stlStatus qlgBuildRecordSqlCachePlanCallback( void              * aDumpObjHandle,
                                              void              * aPathCtrl,
                                              knlValueBlockList * aValueList,
                                              stlInt32            aBlockIdx,
                                              stlBool           * aIsExist,
                                              knlEnv            * aKnlEnv )
{
    qlgSqlCachePlanPathCtrl  * sPathCtrl;
    void                     * sCodePlan;
    stlInt64                   sPlanSize = 0;
    qlgSqlCachePlan            sXBuiltInRecord;
    knlPlanSqlInfo             sSqlInfo;
    

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aIsExist != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sPathCtrl = (qlgSqlCachePlanPathCtrl*)aPathCtrl;

    
    /**
     * Next Flange 얻기
     */

    if( sPathCtrl->mSetPlanIterator == STL_TRUE )
    {
        STL_RAMP( RAMP_GET_NEXT_PLAN );
    
        STL_DASSERT( sPathCtrl->mSqlHandle != NULL );

        
        /**
         * Next Plan 얻기
         */

        STL_TRY( knlPlanFetchNext( & sPathCtrl->mPlanIterator,
                                   & sCodePlan,
                                   & sPlanSize,
                                   aKnlEnv )
                 == STL_SUCCESS );

        if( sCodePlan != NULL )
        {
            knlGetPlanSqlInfo( sPathCtrl->mSqlHandle,
                               & sSqlInfo );
            
            sXBuiltInRecord.mSqlHandle    = sPathCtrl->mSqlHandle;
            sXBuiltInRecord.mRefCount     = sPathCtrl->mSqlHandleRefCount;
            sXBuiltInRecord.mPlanIdx      = sPathCtrl->mPlanIterator.mPlanIdx - 1;
            sXBuiltInRecord.mPlanSize     = sPlanSize;
            sXBuiltInRecord.mIsAtomic     = ((qllOptimizationNode*) sCodePlan)->mIsAtomic;
            sXBuiltInRecord.mHasHintError = ((qlvInitPlan *) QLL_GET_CACHED_INIT_PLAN( sCodePlan ))->mHasHintError;
            sXBuiltInRecord.mBindCount    = sSqlInfo.mBindCount;

            KNL_INIT_REGION_MARK( & sPathCtrl->mRegionMemMark );

            STL_TRY( knlMarkRegionMem( & QLL_CANDIDATE_MEM( QLL_ENV( aKnlEnv ) ),
                                       & sPathCtrl->mRegionMemMark,
                                       aKnlEnv )
                     == STL_SUCCESS );
            
            sPathCtrl->mSetMemMark = STL_TRUE;
            
            STL_TRY( qlgExplainPlanForPlanCache( sSqlInfo.mStmtType,
                                                 (qlvInitPlan *) QLL_GET_CACHED_INIT_PLAN( sCodePlan ),
                                                 (qllOptimizationNode *) sCodePlan,
                                                 sXBuiltInRecord.mBindCount,
                                                 & QLL_CANDIDATE_MEM( QLL_ENV( aKnlEnv ) ),
                                                 & sXBuiltInRecord.mPlanString,
                                                 aKnlEnv )
                     == STL_SUCCESS );
    
            STL_TRY( knlBuildFxRecord( gQlgSqlCachePlanColDesc,
                                       (void*)& sXBuiltInRecord,
                                       aValueList,
                                       aBlockIdx,
                                       aKnlEnv )
                     == STL_SUCCESS );

            sPathCtrl->mSetMemMark = STL_FALSE;

            STL_TRY( knlFreeUnmarkedRegionMem( & QLL_CANDIDATE_MEM( QLL_ENV( aKnlEnv ) ),
                                               & sPathCtrl->mRegionMemMark,
                                               STL_FALSE, /* aFreeChunk */
                                               aKnlEnv )
                     == STL_SUCCESS );

            *aIsExist = STL_TRUE;
        }
        else
        {
            sPathCtrl->mSetPlanIterator = STL_FALSE;
            
            STL_TRY( knlFiniPlanIterator( & sPathCtrl->mPlanIterator,
                                          aKnlEnv )
                     == STL_SUCCESS );

            STL_TRY( knlFlangeFetchNext( & sPathCtrl->mFlangeIterator,
                                         & sPathCtrl->mSqlHandle,
                                         & sPathCtrl->mSqlHandleRefCount,
                                         aKnlEnv )
                     == STL_SUCCESS );

            if( sPathCtrl->mSqlHandle != NULL )
            {
                /**
                 * Plan 얻기
                 */

                STL_TRY( knlInitPlanIterator( & sPathCtrl->mPlanIterator,
                                              sPathCtrl->mSqlHandle,
                                              aKnlEnv )
                         == STL_SUCCESS );

                sPathCtrl->mSetPlanIterator = STL_TRUE;

                STL_THROW( RAMP_GET_NEXT_PLAN );
            }
            else
            {
                /**
                 * Flange Iterator 종료
                 */

                sPathCtrl->mSetFlangeIterator = STL_FALSE;
                
                STL_TRY( knlFiniFlangeIterator( & sPathCtrl->mFlangeIterator,
                                                aKnlEnv )
                         == STL_SUCCESS );

                *aIsExist = STL_FALSE;
            }
        }
    }
    else
    {
        /**
         * 종료 지점일 경우 구성할 레코드가 없음을 알림
         */
        
        *aIsExist = STL_FALSE;
    }

    
    return STL_SUCCESS;
    
    STL_FINISH;

    /* Plan Iterator 종료 */
    if( sPathCtrl->mSetPlanIterator == STL_TRUE )
    {
        (void) knlFiniPlanIterator( & sPathCtrl->mPlanIterator,
                                    aKnlEnv );

        sPathCtrl->mSetPlanIterator = STL_FALSE;
    }

    /* Flange Iterator 종료 */
    if( sPathCtrl->mSetFlangeIterator == STL_TRUE )
    {
        (void) knlFiniFlangeIterator( & sPathCtrl->mFlangeIterator,
                                      aKnlEnv );

        sPathCtrl->mSetFlangeIterator = STL_FALSE;
    }

    /* Unmark Region Memory */
    if( sPathCtrl->mSetMemMark == STL_TRUE )
    {
        (void) knlFreeUnmarkedRegionMem( & QLL_CANDIDATE_MEM( QLL_ENV( aKnlEnv ) ),
                                         & sPathCtrl->mRegionMemMark,
                                         STL_TRUE, /* aFreeChunk */
                                         aKnlEnv );

        sPathCtrl->mSetMemMark = STL_FALSE;
    }

    return STL_FAILURE;
}


/**
 * @brief Plan Cache의 Code Plan 출력을 구성한다.
 * @param[in]   aStmtType         Statement Type
 * @param[in]   aInitPlan         Init Plan
 * @param[in]   aCodePlan         Code Plan
 * @param[in]   aBindCount        Bind Parameter Count
 * @param[in]   aRegionMem        Region Memory
 * @param[out]  aExplainPlanText  Explain Code Plan Info
 * @param[in]   aKnlEnv           Environment
 * @remarks
 */
stlStatus qlgExplainPlanForPlanCache( qllNodeType            aStmtType,
                                      qlvInitPlan          * aInitPlan,
                                      qllOptimizationNode  * aCodePlan,
                                      stlInt32               aBindCount,
                                      knlRegionMem         * aRegionMem,
                                      stlChar             ** aExplainPlanText,
                                      knlEnv               * aKnlEnv )
{
    qllExplainNodeText  * sPlanText         = NULL;
    qllExplainNodeText  * sHeadPlanText     = NULL;
    qllExplainPredText  * sPredText         = NULL;
    qlvInitPlan         * sInitPlan         = NULL;
    qllDataArea         * sDataArea         = NULL;

    qllOptNodeList      * sOptNodeList      = NULL;
    qllOptNodeItem      * sOptNodeItem      = NULL;
    qlnxCommonInfo      * sExecNodes        = NULL;
    knlBindType         * sBindIOType       = NULL;
    dtlDataTypeInfo     * sBindDataTypeInfo = NULL;
    stlChar             * sStrBuf           = NULL;
    stlChar               sTmpBuf[128];
    qllExecutionCost      sExecutionCost;
    stlInt32              sLoop;

    stlInt32              sStrSize;
    stlInt32              sFormatStrSize    = 0;
    stlInt32              sTmpStrSize;
    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitPlan != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aExplainPlanText != NULL, KNL_ERROR_STACK(aKnlEnv) );


    /**
     * Explain Plan 설정
     */

    /* Explain Node Text 생성 */
    STL_TRY( qleCreateExplainNodeText( aRegionMem,
                                       0,
                                       NULL,
                                       &sHeadPlanText,
                                       QLL_ENV( aKnlEnv ) )
             == STL_SUCCESS );

    sPlanText = sHeadPlanText;


    /**
     * Plan Text 구성
     */  

    sInitPlan = (qlvInitPlan *) aInitPlan;
    switch( aStmtType )
    {
        case QLL_STMT_SELECT_TYPE :
        case QLL_STMT_SELECT_FOR_UPDATE_TYPE :
        case QLL_STMT_SELECT_INTO_TYPE :
        case QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE :
            {
                /* Valid Plan 얻기 */
                sOptNodeList = ((qlnoSelectStmt *) aCodePlan->mOptNode)->mOptNodeList;
                break;
            }
            
        case QLL_STMT_DELETE_TYPE :
        case QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE :
        case QLL_STMT_DELETE_RETURNING_QUERY_TYPE :
        case QLL_STMT_DELETE_RETURNING_INTO_TYPE :
            {
                /* Valid Plan 얻기 */
                sOptNodeList = ((qlnoDeleteStmt *) aCodePlan->mOptNode)->mOptNodeList;
                break;
            }

        case QLL_STMT_UPDATE_TYPE :
        case QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE :
        case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE :
        case QLL_STMT_UPDATE_RETURNING_INTO_TYPE :
            {
                /* Valid Plan 얻기 */
                sOptNodeList = ((qlnoUpdateStmt *) aCodePlan->mOptNode)->mOptNodeList;
                break;
            }

        case QLL_STMT_INSERT_TYPE :
        case QLL_STMT_INSERT_SELECT_TYPE :
        case QLL_STMT_INSERT_RETURNING_INTO_TYPE :
        case QLL_STMT_INSERT_RETURNING_QUERY_TYPE :
            {
                /* Valid Plan 얻기 */
                sOptNodeList = ((qlnoInsertStmt *) aCodePlan->mOptNode)->mOptNodeList;
                break;
            }

        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }


    /**
     * 임시 Data Area 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qllDataArea ),
                                (void**) & sDataArea,
                                aKnlEnv )
             == STL_SUCCESS );

    stlMemset( sDataArea, 0x00, STL_SIZEOF( qllDataArea ) );


    /**
     * 임시 qllExecutionNode 생성
     */

    sDataArea->mExecNodeCnt = sOptNodeList->mCount;

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qllExecutionNode ) * sOptNodeList->mCount,
                                (void**) & sDataArea->mExecNodeList,
                                aKnlEnv )
             == STL_SUCCESS );

    stlMemset( sDataArea->mExecNodeList,
               0x00,
               STL_SIZEOF( qllExecutionNode ) * sOptNodeList->mCount );


    /**
     * 임시 Common Execution Node 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlnxCommonInfo ) * sOptNodeList->mCount,
                                (void**) & sExecNodes,
                                aKnlEnv )
             == STL_SUCCESS );

    stlMemset( sExecNodes,
               0x00,
               STL_SIZEOF( qlnxCommonInfo ) * sOptNodeList->mCount );

    stlMemset( & sExecutionCost,
               0x00,
               STL_SIZEOF( qllExecutionCost ) );
    
    sOptNodeItem = sOptNodeList->mHead;
    for( sLoop = 0 ; sLoop < sOptNodeList->mCount ; sLoop++ )
    {
        sDataArea->mExecNodeList[ sLoop ].mNodeType      = sOptNodeItem->mNode->mType;
        sDataArea->mExecNodeList[ sLoop ].mOptNode       = sOptNodeItem->mNode;
        sDataArea->mExecNodeList[ sLoop ].mExecNode      = & sExecNodes[ sLoop ];
        sDataArea->mExecNodeList[ sLoop ].mExecutionCost = & sExecutionCost;

        sOptNodeItem = sOptNodeItem->mNext;
    }
    
    
    /**
     * Explain Plan Text 얻기
     */
    
    STL_TRY( qlneExplainNodeList[ aCodePlan->mType ] ( aCodePlan,
                                                       sDataArea,
                                                       aRegionMem,
                                                       0,
                                                       & sPlanText,
                                                       QLL_ENV( aKnlEnv ) )
             == STL_SUCCESS );


    /****************************
     * String Length 분석
     ****************************/
    
    /**
     * Node들의 Description 설정
     */
    
    sStrSize = 0;
    sPlanText = sHeadPlanText->mNext;

    while( sPlanText != NULL )
    {
        sFormatStrSize = ( 6 + sPlanText->mDepth * 2 + sPlanText->mDescBuffer.mCurSize + 1 );
        sStrSize += sFormatStrSize;

        STL_TRY_THROW( sStrSize <= DTL_LONGVARCHAR_MAX_PRECISION,
                       RAMP_MAXIMUM_SIZE_OF_BUFFER_EXCEEDED );
        
        sPlanText = sPlanText->mNext;
    }

    
    /**
     * 줄 구분자
     */

    sStrSize++;
    STL_TRY_THROW( sStrSize <= DTL_LONGVARCHAR_MAX_PRECISION,
                   RAMP_MAXIMUM_SIZE_OF_BUFFER_EXCEEDED );

    
    /**
     * Node의 Predicate 정보
     */
    
    sPlanText = sHeadPlanText->mNext;
    while( sPlanText != NULL )
    {
        sPredText = sPlanText->mPredText;
        while( sPredText != NULL )
        {
            sFormatStrSize = ( sPredText->mLineBuffer.mCurSize - 2 );
            sStrSize += sFormatStrSize;
            
            STL_TRY_THROW( sStrSize <= DTL_LONGVARCHAR_MAX_PRECISION,
                           RAMP_MAXIMUM_SIZE_OF_BUFFER_EXCEEDED );

            sPredText = sPredText->mNext;
        }

        sPlanText = sPlanText->mNext;
    }

    
    /**
     * Bind Parameter 정보
     */

    sBindIOType       = sInitPlan->mBindIOType;
    sBindDataTypeInfo = aCodePlan->mBindParamInfo->mBindDataTypeInfo;

    if( aBindCount > 0 )
    {
        /**
         * 줄 구분자
         */

        sStrSize++;
        STL_TRY_THROW( sStrSize <= DTL_LONGVARCHAR_MAX_PRECISION,
                       RAMP_MAXIMUM_SIZE_OF_BUFFER_EXCEEDED );

        
        for( sLoop = 0 ; sLoop < aBindCount ; sLoop++ )
        {
            /**
             * Bind Type (IN/OUT)
             */

            switch( sBindDataTypeInfo[ sLoop ].mDataType )
            {
                case DTL_TYPE_BOOLEAN :
                case DTL_TYPE_NATIVE_SMALLINT :
                case DTL_TYPE_NATIVE_INTEGER :
                case DTL_TYPE_NATIVE_BIGINT :
                case DTL_TYPE_NATIVE_REAL :
                case DTL_TYPE_NATIVE_DOUBLE :
                case DTL_TYPE_LONGVARCHAR :
                case DTL_TYPE_LONGVARBINARY :
                case DTL_TYPE_DATE :
                case DTL_TYPE_ROWID :
                    {
                        STL_DASSERT( sBindDataTypeInfo[ sLoop ].mArgNum3 == 0 );
                        
                        sFormatStrSize = stlSnprintf( sTmpBuf,
                                                      128,
                                                      "#%d[%s] : %s\n",
                                                      sLoop,
                                                      ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_IN ? "IN"
                                                        : ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_OUT ? "OUT" : "INOUT" ) ),
                                                      dtlDataTypeNameForDumpFunc[ sBindDataTypeInfo[ sLoop ].mDataType ] );
                        break;
                    }
                case DTL_TYPE_FLOAT :
                case DTL_TYPE_NUMBER :
                    {
                        STL_DASSERT( sBindDataTypeInfo[ sLoop ].mArgNum3 == 0 );
                        
                        if( sBindDataTypeInfo[ sLoop ].mArgNum2 == DTL_SCALE_NA )
                        {
                            sFormatStrSize = stlSnprintf( sTmpBuf,
                                                          128,
                                                          "#%d[%s] : %s(%d)\n",
                                                          sLoop,
                                                          ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_IN ? "IN"
                                                            : ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_OUT ? "OUT" : "INOUT" ) ),
                                                          dtlDataTypeNameForDumpFunc[ sBindDataTypeInfo[ sLoop ].mDataType ],
                                                          sBindDataTypeInfo[ sLoop ].mArgNum1 );
                        }
                        else
                        {
                            sFormatStrSize = stlSnprintf( sTmpBuf,
                                                          128,
                                                          "#%d[%s] : %s(%d,%d)\n",
                                                          sLoop,
                                                          ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_IN ? "IN"
                                                            : ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_OUT ? "OUT" : "INOUT" ) ),
                                                          dtlDataTypeNameForDumpFunc[ sBindDataTypeInfo[ sLoop ].mDataType ],
                                                          sBindDataTypeInfo[ sLoop ].mArgNum1,
                                                          sBindDataTypeInfo[ sLoop ].mArgNum2 );
                        }
                        break;
                    }
                case DTL_TYPE_CHAR :
                case DTL_TYPE_VARCHAR :
                    {
                        STL_DASSERT( sBindDataTypeInfo[ sLoop ].mArgNum3 >= 0 );
                        STL_DASSERT( sBindDataTypeInfo[ sLoop ].mArgNum3 < DTL_STRING_LENGTH_UNIT_MAX );
                        
                        sFormatStrSize = stlSnprintf( sTmpBuf,
                                                      128,
                                                      "#%d[%s] : %s(%d) %s\n",
                                                      sLoop,
                                                      ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_IN ? "IN"
                                                        : ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_OUT ? "OUT" : "INOUT" ) ),
                                                      dtlDataTypeNameForDumpFunc[ sBindDataTypeInfo[ sLoop ].mDataType ],
                                                      sBindDataTypeInfo[ sLoop ].mArgNum1,
                                                      ( sBindDataTypeInfo[ sLoop ].mArgNum3 == DTL_STRING_LENGTH_UNIT_CHARACTERS
                                                        ? "CHARATERS" : "OCTETS" ) );
                        break;
                    }
                case DTL_TYPE_BINARY :
                case DTL_TYPE_VARBINARY :
                case DTL_TYPE_TIME :
                case DTL_TYPE_TIMESTAMP :
                case DTL_TYPE_TIME_WITH_TIME_ZONE :
                case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE :
                    {
                        STL_DASSERT( sBindDataTypeInfo[ sLoop ].mArgNum3 == 0 );
                        
                        sFormatStrSize = stlSnprintf( sTmpBuf,
                                                      128,
                                                      "#%d[%s] : %s(%d)\n",
                                                      sLoop,
                                                      ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_IN ? "IN"
                                                        : ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_OUT ? "OUT" : "INOUT" ) ),
                                                      dtlDataTypeNameForDumpFunc[ sBindDataTypeInfo[ sLoop ].mDataType ],
                                                      sBindDataTypeInfo[ sLoop ].mArgNum1 );
                        break;
                    }
                case DTL_TYPE_INTERVAL_YEAR_TO_MONTH :
                case DTL_TYPE_INTERVAL_DAY_TO_SECOND :
                    {
                        STL_DASSERT( sBindDataTypeInfo[ sLoop ].mArgNum3 >= 0 );
                        STL_DASSERT( sBindDataTypeInfo[ sLoop ].mArgNum3 < DTL_INTERVAL_INDICATOR_MAX );
                        
                        sFormatStrSize = stlSnprintf( sTmpBuf,
                                                      128,
                                                      "#%d[%s] : %s(%d) %s\n",
                                                      sLoop,
                                                      ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_IN ? "IN"
                                                        : ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_OUT ? "OUT" : "INOUT" ) ),
                                                      dtlDataTypeNameForDumpFunc[ sBindDataTypeInfo[ sLoop ].mDataType ],
                                                      sBindDataTypeInfo[ sLoop ].mArgNum1,
                                                      gDtlIntervalIndicatorStringForDump[ sBindDataTypeInfo[ sLoop ].mArgNum3 ] );
                        break;
                    }
                default :
                    {
                        STL_DASSERT( 0 );
                        break;
                    }
            }

            sStrSize += sFormatStrSize;
            
            STL_TRY_THROW( sStrSize <= DTL_LONGVARCHAR_MAX_PRECISION,
                           RAMP_MAXIMUM_SIZE_OF_BUFFER_EXCEEDED );
        }
    }
    else
    {
        /* Do Nothing */
    }

    STL_RAMP( RAMP_MAXIMUM_SIZE_OF_BUFFER_EXCEEDED );


    if( sStrSize > DTL_LONGVARCHAR_MAX_PRECISION )
    {
        sStrSize = DTL_LONGVARCHAR_MAX_PRECISION;
    }
    else
    {
        /* Do Nothing */
    }
    
    STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( QLL_ENV( aKnlEnv ) ),
                                STL_SIZEOF( stlChar ) * sStrSize + 1,
                                (void**) & sStrBuf,
                                aKnlEnv )
             == STL_SUCCESS );

    *aExplainPlanText = sStrBuf;
    

    /****************************
     * String 구성
     ****************************/
    
    /**
     * Node들의 Description 설정
     */
    
    sStrSize = 0;
    sPlanText = sHeadPlanText->mNext;

    while( sPlanText != NULL )
    {
        sFormatStrSize = ( 6 + sPlanText->mDepth * 2 + sPlanText->mDescBuffer.mCurSize + 1 );
        sStrSize += sFormatStrSize;

        STL_TRY_THROW( sStrSize <= DTL_LONGVARCHAR_MAX_PRECISION,
                       RAMP_CANNOT_MAKE_NO_MORE_STRING );

        sTmpStrSize = stlSnprintf( sStrBuf,
                                   sFormatStrSize,
                                   "%4d  %*s%s\n",
                                   sPlanText->mIdx,
                                   sPlanText->mDepth * 2, "",
                                   sPlanText->mDescBuffer.mText );

        STL_DASSERT( sTmpStrSize + 1 == sFormatStrSize );

        sStrBuf += sTmpStrSize;
        
        sPlanText = sPlanText->mNext;
    }

    
    /**
     * 줄 구분자
     */

    sStrSize++;
    STL_TRY_THROW( sStrSize <= DTL_LONGVARCHAR_MAX_PRECISION,
                   RAMP_CANNOT_MAKE_NO_MORE_STRING );

    stlStrcpy( sStrBuf, "\n" );
    sStrBuf++;

    
    /**
     * Node의 Predicate 정보
     */
    
    sPlanText = sHeadPlanText->mNext;
    while( sPlanText != NULL )
    {
        sPredText = sPlanText->mPredText;
        while( sPredText != NULL )
        {
            sFormatStrSize = ( sPredText->mLineBuffer.mCurSize - 2 );
            sStrSize += sFormatStrSize;
            
            STL_TRY_THROW( sStrSize <= DTL_LONGVARCHAR_MAX_PRECISION,
                           RAMP_CANNOT_MAKE_NO_MORE_STRING );

            stlStrcpy( sStrBuf, & sPredText->mLineText[2] );

            sStrBuf += sFormatStrSize;

            sPredText = sPredText->mNext;
        }

        sPlanText = sPlanText->mNext;
    }

    
    /**
     * Bind Parameter 정보
     */

    sBindIOType       = sInitPlan->mBindIOType;
    sBindDataTypeInfo = aCodePlan->mBindParamInfo->mBindDataTypeInfo;

    if( aBindCount > 0 )
    {
        /**
         * 줄 구분자
         */

        sStrSize++;
        STL_TRY_THROW( sStrSize <= DTL_LONGVARCHAR_MAX_PRECISION,
                       RAMP_CANNOT_MAKE_NO_MORE_STRING );

        stlStrcpy( sStrBuf, "\n" );
        sStrBuf++;

        
        for( sLoop = 0 ; sLoop < aBindCount ; sLoop++ )
        {
            /**
             * Bind Type (IN/OUT)
             */

            switch( sBindDataTypeInfo[ sLoop ].mDataType )
            {
                case DTL_TYPE_BOOLEAN :
                case DTL_TYPE_NATIVE_SMALLINT :
                case DTL_TYPE_NATIVE_INTEGER :
                case DTL_TYPE_NATIVE_BIGINT :
                case DTL_TYPE_NATIVE_REAL :
                case DTL_TYPE_NATIVE_DOUBLE :
                case DTL_TYPE_LONGVARCHAR :
                case DTL_TYPE_LONGVARBINARY :
                case DTL_TYPE_DATE :
                case DTL_TYPE_ROWID :
                    {
                        STL_DASSERT( sBindDataTypeInfo[ sLoop ].mArgNum3 == 0 );
                        
                        sFormatStrSize = stlSnprintf( sTmpBuf,
                                                      128,
                                                      "#%d[%s] : %s\n",
                                                      sLoop,
                                                      ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_IN ? "IN"
                                                        : ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_OUT ? "OUT" : "INOUT" ) ),
                                                      dtlDataTypeNameForDumpFunc[ sBindDataTypeInfo[ sLoop ].mDataType ] );
                        break;
                    }
                case DTL_TYPE_FLOAT :
                case DTL_TYPE_NUMBER :
                    {
                        STL_DASSERT( sBindDataTypeInfo[ sLoop ].mArgNum3 == 0 );
                        
                        if( sBindDataTypeInfo[ sLoop ].mArgNum2 == DTL_SCALE_NA )
                        {
                            sFormatStrSize = stlSnprintf( sTmpBuf,
                                                          128,
                                                          "#%d[%s] : %s(%d)\n",
                                                          sLoop,
                                                          ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_IN ? "IN"
                                                            : ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_OUT ? "OUT" : "INOUT" ) ),
                                                          dtlDataTypeNameForDumpFunc[ sBindDataTypeInfo[ sLoop ].mDataType ],
                                                          sBindDataTypeInfo[ sLoop ].mArgNum1 );
                        }
                        else
                        {
                            sFormatStrSize = stlSnprintf( sTmpBuf,
                                                          128,
                                                          "#%d[%s] : %s(%d,%d)\n",
                                                          sLoop,
                                                          ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_IN ? "IN"
                                                            : ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_OUT ? "OUT" : "INOUT" ) ),
                                                          dtlDataTypeNameForDumpFunc[ sBindDataTypeInfo[ sLoop ].mDataType ],
                                                          sBindDataTypeInfo[ sLoop ].mArgNum1,
                                                          sBindDataTypeInfo[ sLoop ].mArgNum2 );
                        }
                        break;
                    }
                case DTL_TYPE_CHAR :
                case DTL_TYPE_VARCHAR :
                    {
                        STL_DASSERT( sBindDataTypeInfo[ sLoop ].mArgNum3 >= 0 );
                        STL_DASSERT( sBindDataTypeInfo[ sLoop ].mArgNum3 < DTL_STRING_LENGTH_UNIT_MAX );
                        
                        sFormatStrSize = stlSnprintf( sTmpBuf,
                                                      128,
                                                      "#%d[%s] : %s(%d) %s\n",
                                                      sLoop,
                                                      ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_IN ? "IN"
                                                        : ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_OUT ? "OUT" : "INOUT" ) ),
                                                      dtlDataTypeNameForDumpFunc[ sBindDataTypeInfo[ sLoop ].mDataType ],
                                                      sBindDataTypeInfo[ sLoop ].mArgNum1,
                                                      ( sBindDataTypeInfo[ sLoop ].mArgNum3 == DTL_STRING_LENGTH_UNIT_CHARACTERS
                                                        ? "CHARATERS" : "OCTETS" ) );
                        break;
                    }
                case DTL_TYPE_BINARY :
                case DTL_TYPE_VARBINARY :
                case DTL_TYPE_TIME :
                case DTL_TYPE_TIMESTAMP :
                case DTL_TYPE_TIME_WITH_TIME_ZONE :
                case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE :
                    {
                        STL_DASSERT( sBindDataTypeInfo[ sLoop ].mArgNum3 == 0 );
                        
                        sFormatStrSize = stlSnprintf( sTmpBuf,
                                                      128,
                                                      "#%d[%s] : %s(%d)\n",
                                                      sLoop,
                                                      ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_IN ? "IN"
                                                        : ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_OUT ? "OUT" : "INOUT" ) ),
                                                      dtlDataTypeNameForDumpFunc[ sBindDataTypeInfo[ sLoop ].mDataType ],
                                                      sBindDataTypeInfo[ sLoop ].mArgNum1 );
                        break;
                    }
                case DTL_TYPE_INTERVAL_YEAR_TO_MONTH :
                case DTL_TYPE_INTERVAL_DAY_TO_SECOND :
                    {
                        STL_DASSERT( sBindDataTypeInfo[ sLoop ].mArgNum3 >= 0 );
                        STL_DASSERT( sBindDataTypeInfo[ sLoop ].mArgNum3 < DTL_INTERVAL_INDICATOR_MAX );
                        
                        sFormatStrSize = stlSnprintf( sTmpBuf,
                                                      128,
                                                      "#%d[%s] : %s(%d) %s\n",
                                                      sLoop,
                                                      ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_IN ? "IN"
                                                        : ( sBindIOType[ sLoop ] == KNL_BIND_TYPE_OUT ? "OUT" : "INOUT" ) ),
                                                      dtlDataTypeNameForDumpFunc[ sBindDataTypeInfo[ sLoop ].mDataType ],
                                                      sBindDataTypeInfo[ sLoop ].mArgNum1,
                                                      gDtlIntervalIndicatorStringForDump[ sBindDataTypeInfo[ sLoop ].mArgNum3 ] );
                        break;
                    }
                default :
                    {
                        STL_DASSERT( 0 );
                        break;
                    }
            }

            sStrSize += sFormatStrSize;
            
            STL_TRY_THROW( sStrSize <= DTL_LONGVARCHAR_MAX_PRECISION,
                           RAMP_CANNOT_MAKE_NO_MORE_STRING );

            stlStrcpy( sStrBuf, sTmpBuf );

            sStrBuf += sFormatStrSize;
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    STL_RAMP( RAMP_CANNOT_MAKE_NO_MORE_STRING );
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief X$SQL_CACHE_PLAN 을 위한 테이블 정의
 * @remarks
 */
knlFxTableDesc gQlgSqlCachePlanTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,         /**< Basic Fixed Table 임 */
    KNL_STARTUP_PHASE_NO_MOUNT,            /**< NO_MOUNT 단계부터 조회 가능 */
    NULL,                                  /**< Dump Object Handle 획득 함수 */
    qlgOpenSqlCachePlanCallback,           /**< Open Callback */
    qlgCloseSqlCachePlanCallback,          /**< Close Callback */
    qlgBuildRecordSqlCachePlanCallback,    /**< BuildRecord Callback */
    STL_SIZEOF(qlgSqlCachePlanPathCtrl),   /**< Path Controller Size */
    "X$SQL_CACHE_PLAN",                    /**< Table Name */
    "sql cache plan",                      /**< Table Comment */
    gQlgSqlCachePlanColDesc                /**< 컬럼 정보 */
};



/** @} qllSqlCachePlan */


