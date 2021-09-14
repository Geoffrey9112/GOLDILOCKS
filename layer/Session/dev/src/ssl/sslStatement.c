/*******************************************************************************
 * sslStatement.c
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

#include <qll.h>
#include <sslDef.h>
#include <sslCommand.h>
#include <sslStatement.h>
#include <ssc.h>
#include <ssDef.h>
#include <sst.h>

/**
 * @file sslStatement.c
 * @brief Session Statement Routines
 */

/**
 * @brief X$SS_STMT_STAT
 */
knlFxStatementInfo gSssStmtStatRows[SSS_SS_STMT_STAT_ROW_COUNT] =
{
    {
        "SQL_SHARE_MEMORY_SYNTAX_TOTAL_SIZE",
        0,
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SQL_SHARE_MEMORY_SYNTAX_INIT_SIZE",
        0,
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    },
    {
        "SQL_SHARE_MEMORY_INIT_PLAN_TOTAL_SIZE",
        0,
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SQL_SHARE_MEMORY_INIT_PLAN_INIT_SIZE",
        0,
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    },
    {
        "SQL_SHARE_MEMORY_NONCACHE_PLAN_TOTAL_SIZE",
        0,
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SQL_SHARE_MEMORY_NONCACHE_PLAN_INIT_SIZE",
        0,
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    },
    {
        "SQL_SHARE_MEMORY_CACHE_PLAN_TOTAL_SIZE",
        0,
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SQL_SHARE_MEMORY_CACHE_PLAN_INIT_SIZE",
        0,
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    },
    {
        "SQL_SHARE_MEMORY_DATA_PLAN_TOTAL_SIZE",
        0,
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SQL_SHARE_MEMORY_DATA_PLAN_INIT_SIZE",
        0,
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    },
    {
        "SQL_SHARE_MEMORY_STMT_TOTAL_SIZE",
        0,
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SQL_SHARE_MEMORY_STMT_INIT_SIZE",
        0,
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    }
};

stlStatus openFxSsStmtStatCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aKnlEnv )
{
    sssStmtStatPathControl * sPathCtrl;
    sslSessionEnv          * sSession;
    knlSessionEnv          * sSessEnv;
    sslStatement           * sStatement;
    sslStatement          ** sStatementSlot;
    stlInt64               * sValues;
    stlInt32                 sMaxStmtCount;
    stlInt32                 i;

    sPathCtrl  = (sssStmtStatPathControl*)aPathCtrl;

    sPathCtrl->mCurEnv    = NULL;
    sPathCtrl->mSessId    = 0;
    sPathCtrl->mIterator  = 0;
    sValues = sPathCtrl->mValues;
    
    sSessEnv = knlGetFirstSessionEnv();

    while( sSessEnv != NULL )
    {
        /**
         * admin session은 DBC를 위해 heap memory를 사용한다.
         */
        
        if( (knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE) &&
            (knlIsAdminSession( sSessEnv ) == STL_FALSE) )
        {
            STL_TRY( knlGetSessionEnvId( sSessEnv,
                                         &sPathCtrl->mSessId,
                                         aKnlEnv )
                     == STL_SUCCESS );
            
            sPathCtrl->mCurEnv = sSessEnv;
            sPathCtrl->mSlot   = 0;
            sPathCtrl->mStmtId = -1;

            sSession = (sslSessionEnv*)sSessEnv;
            sMaxStmtCount = sSession->mMaxStmtCount;

            sStatementSlot = sSession->mStatementSlot;
                
            if( sStatementSlot != NULL )
            {
                for( i = 0; i < sMaxStmtCount; i++ )
                {
                    sStatement = sStatementSlot[i];

                    if( sStatement != NULL )
                    {
                        if( knlIsValidStaticAddress( sStatement,
                                                     STL_SIZEOF(sslStatement) )
                            == STL_TRUE )
                        {
                            sPathCtrl->mStmtId = sStatement->mId;

                            sValues[0]  = sStatement->mDbcStmt.mShareMemSyntaxSQL.mTotalSize;
                            sValues[1]  = sStatement->mDbcStmt.mShareMemSyntaxSQL.mInitSize;
                            sValues[2]  = sStatement->mDbcStmt.mShareMemInitPlan.mTotalSize;
                            sValues[3]  = sStatement->mDbcStmt.mShareMemInitPlan.mInitSize;
                            sValues[4]  = sStatement->mDbcStmt.mNonPlanCacheMem.mTotalSize;
                            sValues[5]  = sStatement->mDbcStmt.mNonPlanCacheMem.mInitSize;
                            sValues[6]  = sStatement->mDbcStmt.mPlanCacheMem.mTotalSize;
                            sValues[7]  = sStatement->mDbcStmt.mPlanCacheMem.mInitSize;
                            sValues[8]  = sStatement->mDbcStmt.mShareMemDataPlan.mTotalSize;
                            sValues[9]  = sStatement->mDbcStmt.mShareMemDataPlan.mInitSize;
                            sValues[10] = sStatement->mDbcStmt.mShareMemStmt.mTotalSize;
                            sValues[11] = sStatement->mDbcStmt.mShareMemStmt.mInitSize;
            
                            sPathCtrl->mSlot = i + 1;

                            STL_THROW( RAMP_FIND_STATEMENT );
                        }
                        
                        /**
                         * goto next session
                         */
                        break;
                    }
                }
            }
        }
        
        sSessEnv = knlGetNextSessionEnv( sSessEnv );
    }

    STL_RAMP( RAMP_FIND_STATEMENT );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus closeFxSsStmtStatCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus buildRecordFxSsStmtStatCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aKnlEnv )
{
    sssStmtStatPathControl * sPathCtrl;
    knlFxStatementInfo       sFxStatementInfo;
    knlFxStatementInfo     * sRowDesc;
    stlInt64               * sValues;
    sslStatement           * sStatement = NULL;
    sslStatement          ** sStatementSlot = NULL;
    sslSessionEnv          * sSession;
    knlSessionEnv          * sSessEnv;
    stlInt32                 sMaxStmtCount;
    stlInt32                 i;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl  = (sssStmtStatPathControl*)aPathCtrl;

    sSession      = (sslSessionEnv*)(sPathCtrl->mCurEnv);
    sMaxStmtCount = sSession->mMaxStmtCount;
    sValues       = sPathCtrl->mValues;

    if( sPathCtrl->mIterator >= SSS_SS_STMT_STAT_ROW_COUNT )
    {
        /*
         * 다음 statement 획득
         */
        sStatementSlot = sSession->mStatementSlot;

        if( sStatementSlot != NULL )
        {
            for( i = sPathCtrl->mSlot; i < sMaxStmtCount; i++ )
            {
                sStatement = sStatementSlot[i];

                if( sStatement != NULL )
                {
                    if( knlIsValidStaticAddress( sStatement,
                                                 STL_SIZEOF(sslStatement) )
                        == STL_TRUE )
                    {
                        sPathCtrl->mSlot = i + 1;
                    }
                    else
                    {
                        /**
                         * goto next session
                         */
                        sStatement = NULL;
                    }
                    
                    break;
                }
                else
                {
                    /**
                     * goto next slot
                     */
                }
            }
        }

        if( sStatement != NULL )
        {
            sPathCtrl->mStmtId = sStatement->mId;
            
            sValues[0]  = sStatement->mDbcStmt.mShareMemSyntaxSQL.mTotalSize;
            sValues[1]  = sStatement->mDbcStmt.mShareMemSyntaxSQL.mInitSize;
            sValues[2]  = sStatement->mDbcStmt.mShareMemInitPlan.mTotalSize;
            sValues[3]  = sStatement->mDbcStmt.mShareMemInitPlan.mInitSize;
            sValues[4]  = sStatement->mDbcStmt.mNonPlanCacheMem.mTotalSize;
            sValues[5]  = sStatement->mDbcStmt.mNonPlanCacheMem.mInitSize;
            sValues[6]  = sStatement->mDbcStmt.mPlanCacheMem.mTotalSize;
            sValues[7]  = sStatement->mDbcStmt.mPlanCacheMem.mInitSize;
            sValues[8]  = sStatement->mDbcStmt.mShareMemDataPlan.mTotalSize;
            sValues[9]  = sStatement->mDbcStmt.mShareMemDataPlan.mInitSize;
            sValues[10] = sStatement->mDbcStmt.mShareMemStmt.mTotalSize;
            sValues[11] = sStatement->mDbcStmt.mShareMemStmt.mInitSize;
        }
        else
        {
            sSessEnv = knlGetNextSessionEnv( sPathCtrl->mCurEnv );

            while( sSessEnv != NULL )
            {
                /**
                 * admin session은 DBC를 위해 heap memory를 사용한다.
                 */
        
                if( (knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE) &&
                    (knlIsAdminSession( sSessEnv ) == STL_FALSE) )
                {
                    STL_TRY( knlGetSessionEnvId( sSessEnv,
                                                 &sPathCtrl->mSessId,
                                                 aKnlEnv )
                             == STL_SUCCESS );
            
                    sPathCtrl->mCurEnv = sSessEnv;
                    sPathCtrl->mSlot   = 0;
                    sPathCtrl->mStmtId = -1;

                    sSession = (sslSessionEnv*)sSessEnv;
                    sMaxStmtCount = sSession->mMaxStmtCount;

                    sStatementSlot = sSession->mStatementSlot;
                        
                    if( sStatementSlot != NULL )
                    {
                        for( i = 0; i < sMaxStmtCount; i++ )
                        {
                            sStatement = sStatementSlot[i];

                            if( sStatement != NULL )
                            {
                                if( knlIsValidStaticAddress( sStatement,
                                                             STL_SIZEOF(sslStatement) )
                                    == STL_TRUE )
                                {
                                    sPathCtrl->mStmtId = sStatement->mId;
                                    
                                    sValues[0]  = sStatement->mDbcStmt.mShareMemSyntaxSQL.mTotalSize;
                                    sValues[1]  = sStatement->mDbcStmt.mShareMemSyntaxSQL.mInitSize;
                                    sValues[2]  = sStatement->mDbcStmt.mShareMemInitPlan.mTotalSize;
                                    sValues[3]  = sStatement->mDbcStmt.mShareMemInitPlan.mInitSize;
                                    sValues[4]  = sStatement->mDbcStmt.mNonPlanCacheMem.mTotalSize;
                                    sValues[5]  = sStatement->mDbcStmt.mNonPlanCacheMem.mInitSize;
                                    sValues[6]  = sStatement->mDbcStmt.mPlanCacheMem.mTotalSize;
                                    sValues[7]  = sStatement->mDbcStmt.mPlanCacheMem.mInitSize;
                                    sValues[8]  = sStatement->mDbcStmt.mShareMemDataPlan.mTotalSize;
                                    sValues[9]  = sStatement->mDbcStmt.mShareMemDataPlan.mInitSize;
                                    sValues[10] = sStatement->mDbcStmt.mShareMemStmt.mTotalSize;
                                    sValues[11] = sStatement->mDbcStmt.mShareMemStmt.mInitSize;
                            
                                    sPathCtrl->mSlot = i + 1;

                                    STL_THROW( RAMP_FIND_STATEMENT );
                                }
                                
                                /**
                                 * goto next session
                                 */
                                break;
                            }
                            else
                            {
                                /**
                                 * goto next slot
                                 */
                            }
                        }
                    }
                }
        
                sSessEnv = knlGetNextSessionEnv( sSessEnv );
            }

            STL_RAMP( RAMP_FIND_STATEMENT );

            sPathCtrl->mCurEnv = sSessEnv;
        }

        sPathCtrl->mIterator = 0;
    }
    
    STL_TRY_THROW( sPathCtrl->mCurEnv != NULL, RAMP_SUCCESS );

    sRowDesc = &gSssStmtStatRows[sPathCtrl->mIterator];

    sFxStatementInfo.mName     = sRowDesc->mName;
    sFxStatementInfo.mCategory = sRowDesc->mCategory;
    sFxStatementInfo.mSessId   = sPathCtrl->mSessId;
    sFxStatementInfo.mStmtId   = SSL_GET_STATEMENT_SEQ( sPathCtrl->mStmtId );
    sFxStatementInfo.mValue    = sValues[sPathCtrl->mIterator];

    STL_TRY( knlBuildFxRecord( gKnlStatementInfoColumnDesc,
                               &sFxStatementInfo,
                               aValueList,
                               aBlockIdx,
                               aKnlEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;
    sPathCtrl->mIterator += 1;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSsStmtStatTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxSsStmtStatCallback,
    closeFxSsStmtStatCallback,
    buildRecordFxSsStmtStatCallback,
    STL_SIZEOF( sssStmtStatPathControl ),
    "X$SS_STMT_STAT",
    "statement statistic information of session layer",
    gKnlStatementInfoColumnDesc
};


knlFxColumnDesc gSsStmtColumnDesc[] =
{
    {
        "ID",
        "statement id",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( sssFxStmt, mId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "SESSION_ID",
        "current sql",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( sssFxStmt, mSessId ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "VIEW_SCN",
        "statement view scn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( sssFxStmt, mViewScn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "SQL_TEXT",
        "current sql",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( sssFxStmt, mStmtSql ),
        QLL_MAX_STMT_SQL_SIZE,
        STL_TRUE  /* nullable */
    },
    {
        "START_TIME",
        "start time",
        KNL_FXTABLE_DATATYPE_TIMESTAMP,
        STL_OFFSETOF( sssFxStmt, mStartTime ),
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


stlStatus openFxSsStmtCallback( void   * aStmt,
                                void   * aDumpObjHandle,
                                void   * aPathCtrl,
                                knlEnv * aKnlEnv )
{
    sssStmtPathControl * sPathCtrl;

    sPathCtrl  = (sssStmtPathControl*)aPathCtrl;

    sPathCtrl->mCurEnv   = NULL;
    sPathCtrl->mCurSlot  = -1;
    
    return STL_SUCCESS;
}

stlStatus closeFxSsStmtCallback( void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus buildRecordFxSsStmtCallback( void              * aDumpObjHandle,
                                       void              * aPathCtrl,
                                       knlValueBlockList * aValueList,
                                       stlInt32            aBlockIdx,
                                       stlBool           * aTupleExist,
                                       knlEnv            * aKnlEnv )
{
    sssStmtPathControl  * sPathCtrl;
    sslStatement        * sStatement = NULL;
    sslStatement       ** sStmtSlot = NULL;
    knlSessionEnv       * sSessEnv;
    sslSessionEnv       * sSession;
    stlInt32              sMaxStmtCount;
    stlInt32              i;
    sssFxStmt             sFxStmtRecord;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl  = (sssStmtPathControl*)aPathCtrl;

    if( sPathCtrl->mCurEnv == NULL )
    {
        sSessEnv = knlGetFirstSessionEnv();
        
        while( sSessEnv != NULL )
        {
            /**
             * admin session은 DBC를 위해 heap memory를 사용한다.
             */
        
            if( (knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE) &&
                (knlIsAdminSession( sSessEnv ) == STL_FALSE) )
            {
                break;
            }
        
            sSessEnv = knlGetNextSessionEnv( sSessEnv );
        }

        sPathCtrl->mCurEnv = sSessEnv;
    }

    while( sPathCtrl->mCurEnv != NULL )
    {
        sSession = (sslSessionEnv*)sPathCtrl->mCurEnv;
        sMaxStmtCount = sSession->mMaxStmtCount;
        sStatement = NULL;

        sStmtSlot = sSession->mStatementSlot;

        if( sStmtSlot != NULL )
        {
            /**
             * 다음 statement 획득
             */
            for( i = sPathCtrl->mCurSlot + 1; i < sMaxStmtCount; i++ )
            {
                if( sStmtSlot[i] != NULL )
                {
                    sStatement = sStmtSlot[i];
                    sPathCtrl->mCurSlot = i;

                    if( sStatement != NULL )
                    {
                        /**
                         * garbage 라도 유효한 포인터라면 출력한다.
                         */
                        if( knlIsValidStaticAddress( sStatement,
                                                     STL_SIZEOF(sslStatement) )
                            == STL_TRUE )
                        {
                            break;
                        }
                
                        /**
                         * goto next Session
                         */
                        sStatement = NULL;
                        break;
                    }
                }
            }
        }

        if( sStatement != NULL )
        {
            break;
        }
        else
        {
            sSessEnv = knlGetNextSessionEnv( sPathCtrl->mCurEnv );

            while( sSessEnv != NULL )
            {
                /**
                 * admin session은 DBC를 위해 heap memory를 사용한다.
                 */
        
                if( (knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE) &&
                    (knlIsAdminSession( sSessEnv ) == STL_FALSE) )
                {
                    break;
                }
        
                sSessEnv = knlGetNextSessionEnv( sSessEnv );
            }
            
            sPathCtrl->mCurEnv = sSessEnv;
        }

        sPathCtrl->mCurSlot = -1;
    }
    
    STL_TRY_THROW( sStatement != NULL, RAMP_SUCCESS );

    STL_DASSERT( sPathCtrl->mCurEnv != NULL );

    STL_TRY( knlGetSessionEnvId( sPathCtrl->mCurEnv,
                                 &sFxStmtRecord.mSessId,
                                 aKnlEnv )
             == STL_SUCCESS );
    
    sFxStmtRecord.mId = sPathCtrl->mCurSlot;
    sFxStmtRecord.mViewScn = sStatement->mViewScn;
    sFxStmtRecord.mStartTime = sStatement->mSqlStmt.mStartTime;
    
    stlStrncpy( sFxStmtRecord.mStmtSql,
                sStatement->mSqlStmt.mStmtSQL,
                QLL_MAX_STMT_SQL_SIZE );

    STL_TRY( knlBuildFxRecord( gSsStmtColumnDesc,
                               &sFxStmtRecord,
                               aValueList,
                               aBlockIdx,
                               aKnlEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSsStmtTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxSsStmtCallback,
    closeFxSsStmtCallback,
    buildRecordFxSsStmtCallback,
    STL_SIZEOF( sssStmtPathControl ),
    "X$STATEMENT",
    "statement information",
    gSsStmtColumnDesc
};
