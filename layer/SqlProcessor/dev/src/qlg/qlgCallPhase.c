/*******************************************************************************
 * glgCallPhase.c
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
 * @file qlgCallPhase.c
 * @brief SQL Processor Layer CallPhase Internal Routines
 */


#include <qll.h>
#include <qlDef.h>
#include <qlgCallPhase.h>
#include <qlpParserMacro.h>

/**
 * @defgroup qlgCallPhase QP API call 단계별 내부 함수 
 * @ingroup qlInternal
 * @{
 */


/*************************************************************************
 * API 단계별 수행 함수 획득
 *************************************************************************/


/**
 * @brief 수행 단계별 Alloc 가능한 메모리를 설정함
 * @param[in] aDBCStmt DBC Statement
 * @param[in] aPhase   Query Phase
 * @param[in] aEnv     Environment
 */
void qlgSetQueryPhaseMemMgr( qllDBCStmt    * aDBCStmt,
                             qllQueryPhase   aPhase,
                             qllEnv        * aEnv )
{
    STL_DASSERT( aDBCStmt != NULL );
    STL_DASSERT( (aPhase > QLL_PHASE_NA) && (aPhase < QLL_PHASE_MAX) );

    switch ( aPhase )
    {
        case QLL_PHASE_PARSING:
            {
                knlEnableAllocation( & aEnv->mHeapMemParsing );
                knlEnableAllocation( & aEnv->mHeapMemRunning );
                knlEnableAllocation( & aDBCStmt->mShareMemSyntaxSQL );
                knlEnableAllocation( & aDBCStmt->mShareMemStmt );

                knlDisableAllocation( & aDBCStmt->mShareMemInitPlan );
                knlDisableAllocation( & aDBCStmt->mPlanCacheMem );
                knlDisableAllocation( & aDBCStmt->mNonPlanCacheMem );
                knlDisableAllocation( & aDBCStmt->mShareMemDataPlan );
                break;
            }
        case QLL_PHASE_VALIDATION:
            {
                knlEnableAllocation( & aEnv->mHeapMemParsing );
                knlDisableAllocation( & aEnv->mHeapMemRunning );
                knlDisableAllocation( & aDBCStmt->mShareMemSyntaxSQL );
                knlEnableAllocation( & aDBCStmt->mShareMemStmt );

                knlEnableAllocation( & aDBCStmt->mShareMemInitPlan );
                knlDisableAllocation( & aDBCStmt->mPlanCacheMem );
                knlDisableAllocation( & aDBCStmt->mNonPlanCacheMem );
                knlDisableAllocation( & aDBCStmt->mShareMemDataPlan );
                break;
            }
        case QLL_PHASE_CODE_OPTIMIZE:
            {
                knlDisableAllocation( & aEnv->mHeapMemParsing );
                knlEnableAllocation( & aEnv->mHeapMemRunning );
                knlDisableAllocation( & aDBCStmt->mShareMemSyntaxSQL );
                knlEnableAllocation( & aDBCStmt->mShareMemStmt );

                knlDisableAllocation( & aDBCStmt->mShareMemInitPlan );
                if ( aDBCStmt->mShareMemCodePlan == & aDBCStmt->mPlanCacheMem )
                {
                    knlEnableAllocation( & aDBCStmt->mPlanCacheMem );
                    knlDisableAllocation( & aDBCStmt->mNonPlanCacheMem );
                }
                else
                {
                    knlDisableAllocation( & aDBCStmt->mPlanCacheMem );
                    knlEnableAllocation( & aDBCStmt->mNonPlanCacheMem );
                }
                knlDisableAllocation( & aDBCStmt->mShareMemDataPlan );
                break;
            }
        case QLL_PHASE_DATA_OPTIMIZE:
            {
                knlDisableAllocation( & aEnv->mHeapMemParsing );
                knlDisableAllocation( & aEnv->mHeapMemRunning );
                knlDisableAllocation( & aDBCStmt->mShareMemSyntaxSQL );
                knlEnableAllocation( & aDBCStmt->mShareMemStmt );

                knlDisableAllocation( & aDBCStmt->mShareMemInitPlan );
                knlDisableAllocation( & aDBCStmt->mPlanCacheMem );
                knlDisableAllocation( & aDBCStmt->mNonPlanCacheMem );
                knlEnableAllocation( & aDBCStmt->mShareMemDataPlan );
                break;
            }
        case QLL_PHASE_RECHECK_PRIVILEGE:
            {
                knlDisableAllocation( & aEnv->mHeapMemParsing );
                knlDisableAllocation( & aEnv->mHeapMemRunning );
                knlDisableAllocation( & aDBCStmt->mShareMemSyntaxSQL );
                knlDisableAllocation( & aDBCStmt->mShareMemStmt );

                knlDisableAllocation( & aDBCStmt->mShareMemInitPlan );
                knlDisableAllocation( & aDBCStmt->mPlanCacheMem );
                knlDisableAllocation( & aDBCStmt->mNonPlanCacheMem );
                knlDisableAllocation( & aDBCStmt->mShareMemDataPlan );
                break;
            }
        case QLL_PHASE_EXECUTE:
            {
                knlDisableAllocation( & aEnv->mHeapMemParsing );
                knlEnableAllocation( & aEnv->mHeapMemRunning );
                knlDisableAllocation( & aDBCStmt->mShareMemSyntaxSQL );
                knlEnableAllocation( & aDBCStmt->mShareMemStmt );

                knlDisableAllocation( & aDBCStmt->mShareMemInitPlan );
                knlDisableAllocation( & aDBCStmt->mPlanCacheMem );
                knlDisableAllocation( & aDBCStmt->mNonPlanCacheMem );
                knlDisableAllocation( & aDBCStmt->mShareMemDataPlan );
                break;
            }
        default:
            STL_DASSERT(0);
            break;
    }
}



/**
 * @brief 수행 단계별 메모리 관리자를 모두 Disable 시킴
 * @param[in] aDBCStmt DBC Statement
 * @param[in] aEnv     Environment
 */
void qlgDisableQueryPhaseMemMgr( qllDBCStmt    * aDBCStmt,
                                 qllEnv        * aEnv )
{
    STL_DASSERT( aDBCStmt != NULL );

    knlDisableAllocation( & aEnv->mHeapMemParsing );
    knlDisableAllocation( & aEnv->mHeapMemRunning );
    knlDisableAllocation( & aDBCStmt->mShareMemSyntaxSQL );
    knlDisableAllocation( & aDBCStmt->mShareMemStmt );
    
    knlDisableAllocation( & aDBCStmt->mShareMemInitPlan );
    knlDisableAllocation( & aDBCStmt->mPlanCacheMem );
    knlDisableAllocation( & aDBCStmt->mNonPlanCacheMem );
    knlDisableAllocation( & aDBCStmt->mShareMemDataPlan );
}



/**
 * @brief SQL Statement 의 Validate Function Pointer 를 얻는다. 
 * @param[in] aType  SQL Statement Type
 * @return
 * SQL Statement 의 Validate Function Pointer
 */
qllValidateFunc qlgGetValidateFuncSQL( qllNodeType aType ) 
{
    stlInt32 sIdx = 0;

    if ( (aType > QLL_STMT_NA) && (aType < QLL_STMT_MAX) )
    {
        sIdx = aType - QLL_STMT_NA;
    }
    else
    {
        sIdx = 0;
    }

    return gSQLStmtTable[sIdx].mValidFunc;
}

/**
 * @brief SQL Statement 의 Code Optimize Function Pointer 를 얻는다. 
 * @param[in] aType  SQL Statement Type
 * @return
 * SQL Statement 의 Code Optimize Function Pointer
 */
qllCodeOptFunc  qlgGetCodeOptimizeFuncSQL( qllNodeType aType )  
{
    stlInt32 sIdx = 0;

    if ( (aType > QLL_STMT_NA) && (aType < QLL_STMT_MAX) )
    {
        sIdx = aType - QLL_STMT_NA;
    }
    else
    {
        sIdx = 0;
    }

    return gSQLStmtTable[sIdx].mCodeOptFunc;
}


/**
 * @brief SQL Statement 의 Data Optimize Function Pointer 를 얻는다. 
 * @param[in] aType  SQL Statement Type
 * @return
 * SQL Statement 의 Data Optimize Function Pointer
 */
qllDataOptFunc  qlgGetDataOptimizeFuncSQL( qllNodeType aType )
{
    stlInt32 sIdx = 0;

    if ( (aType > QLL_STMT_NA) && (aType < QLL_STMT_MAX) )
    {
        sIdx = aType - QLL_STMT_NA;
    }
    else
    {
        sIdx = 0;
    }

    return gSQLStmtTable[sIdx].mDataOptFunc;
}
    
    
/**
 * @brief SQL Statement 의 Execution Function Pointer 를 얻는다. 
 * @param[in] aType  SQL Statement Type
 * @return
 * SQL Statement 의 Execution Function Pointer
 */
qllExecuteFunc  qlgGetExecuteFuncSQL( qllNodeType aType )
{
    stlInt32 sIdx = 0;

    if ( (aType > QLL_STMT_NA) && (aType < QLL_STMT_MAX) )
    {
        sIdx = aType - QLL_STMT_NA;
    }
    else
    {
        sIdx = 0;
    }

    return gSQLStmtTable[sIdx].mExecuteFunc;
}
    

/**
 * @brief SQL Statement 를 수행 가능한 Phase 인지 검사한다.
 * @param[in] aType  SQL Statement Type
 * @param[in] aEnv   Environment
 * @return
 */
stlStatus  qlgCheckExecutableStatement( qllNodeType aType, qllEnv * aEnv )  
{
    stlInt32          sIdx = 0;
    qlgStmtTable    * sStmtTable;
    knlStartupPhase   sCurrPhase = KNL_STARTUP_PHASE_NONE;

    /**
     * SQL stmt 의 상대 ID 획득 
     */
    
    if ( (aType > QLL_STMT_NA) && (aType < QLL_STMT_MAX) )
    {
        sIdx = aType - QLL_STMT_NA;
    }
    else
    {
        sIdx = 0;
    }

    sStmtTable = & gSQLStmtTable[sIdx];

    /**
     * 현재 Phase 정보 획득 
     */
    
    sCurrPhase = knlGetCurrStartupPhase();

    /**
     * 현재 Phase 에서 수행가능하지 검사
     */

    STL_TRY_THROW ( (sCurrPhase >= sStmtTable->mExecutableFrom) &&
                    (sCurrPhase <= sStmtTable->mExecutableUntil),
                    RAMP_ERR_INVALID_EXECUTABLE_PHASE );
    
    /**
     * 현재 Database 에서 수행가능하지 검사
     */

    if( (sStmtTable->mStmtAttr & QLL_STMT_ATTR_SUPPORT_CDS_MASK) ==
        QLL_STMT_ATTR_SUPPORT_CDS_NO )
    {
        STL_TRY_THROW( smlGetDataStoreMode() == SML_DATA_STORE_MODE_TDS,
                       RAMP_ERR_NOT_ALLOW_COMMAND_IN_CDS );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_EXECUTABLE_PHASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_EXECUTABLE_PHASE,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      gPhaseString[sCurrPhase],
                      gSQLStmtTable[sIdx].mStmtTypeKeyword,
                      gPhaseString[sStmtTable->mExecutableFrom],
                      gPhaseString[sStmtTable->mExecutableUntil] );
    }
    
    STL_CATCH( RAMP_ERR_NOT_ALLOW_COMMAND_IN_CDS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_ALLOW_COMMAND_IN_CDS,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/*************************************************************************
 * API 단계별 수행 정보 누적
 *************************************************************************/


/**
 * @brief Fixed Table 을 위한 Parsing Call 회수를 증가한다.
 * @param[in] aEnv Environment
 */
void qlgAddCallCntParse( qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    aEnv->mParsingCnt++;
    sSessEnv->mParsingCnt++;
}


/**
 * @brief Fixed Table 을 위한 Validate Call 회수를 증가한다.
 * @param[in] aEnv Environment
 */
void qlgAddCallCntValidate( qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    aEnv->mValidateCnt++;
    sSessEnv->mValidateCnt++;
}



/**
 * @brief Fixed Table 을 위한 BindContext Call 회수를 증가한다.
 * @param[in] aEnv Environment
 */
void qlgAddCallCntBindContext( qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    aEnv->mBindContextCnt++;
    sSessEnv->mBindContextCnt++;
}



/**
 * @brief Fixed Table 을 위한 Optimize Code Area Call 회수를 증가한다.
 * @param[in] aEnv Environment
 */
void qlgAddCallCntOptCode( qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    aEnv->mOptCodeCnt++;
    sSessEnv->mOptCodeCnt++;
}


/**
 * @brief Fixed Table 을 위한 Optimize Data Area Call 회수를 증가한다.
 * @param[in] aEnv Environment
 */
void qlgAddCallCntOptData( qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    aEnv->mOptDataCnt++;
    sSessEnv->mOptDataCnt++;
}



/**
 * @brief Fixed Table 을 위한 Recheck Privilege Call 회수를 증가한다.
 * @param[in] aEnv Environment
 */
void qlgAddCallCntRecheck( qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    aEnv->mRecheckCnt++;
    sSessEnv->mRecheckCnt++;
}


/**
 * @brief Fixed Table 을 위한 Execute Call 회수를 증가한다.
 * @param[in] aSQLStmt SQL Stmt
 * @param[in] aEnv     Environment
 */
void qlgAddCallCntExecute( qllStatement * aSQLStmt,
                           qllEnv       * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    stlInt32   sStmtTypeID = 0;

    aEnv->mExecuteCnt++;
    sSessEnv->mExecuteCnt++;
    
    sStmtTypeID = qlgGetStmtTypeRelativeID( aSQLStmt->mStmtType );
    
    (aEnv->mExecStmtCnt[sStmtTypeID])++;
    (sSessEnv->mExecStmtCnt[sStmtTypeID])++;
}


/**
 * @brief Fixed Table 을 위한 Recompile Call 회수를 증가한다.
 * @param[in] aEnv Environment
 */
void qlgAddCallCntRecompile( qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);
    
    aEnv->mRecompileCnt++;
    sSessEnv->mRecompileCnt++;
}


/*************************************************************************
 * 단계별 주요 함수 
 *************************************************************************/


stlChar * qlgMakeErrPosString( stlChar * aSQLString,
                               stlInt32  aPosition,
                               qllEnv  * aEnv )
{
    stlInt8  sMBLen = 0;

    stlInt32 sOffset = 0;
    stlInt32 sLineNo = 0;
    stlInt32 sLineStartOffset = 0;
    stlInt32 sLineEndOffset = 0;

    dtlCharacterSet sCharSet;

    dtlFuncVector * sVector = KNL_DT_VECTOR(aEnv);
    
    sCharSet = sVector->mGetCharSetIDFunc(aEnv);

    aEnv->mMessage[0] = '\0';
    
    while( aSQLString[sOffset] != '\0' )
    {
        if ( sOffset >= aPosition )
        {
            /**
             * 위치를 찾음
             */

            sLineEndOffset = sOffset;
            while ( (aSQLString[sLineEndOffset] != '\n') &&
                    (aSQLString[sLineEndOffset] != '\0') )
            {
                if( dtlGetMbLength( sCharSet,
                                    (stlChar *) aSQLString + sOffset,
                                    & sMBLen,
                                    QLL_ERROR_STACK(aEnv) )
                    != STL_SUCCESS )
                {
                    stlSnprintf( aEnv->mMessage,
                                 STL_MAX_ERROR_MESSAGE,
                                 "string position : %d\n",
                                 aPosition );
                    
                    (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                    STL_THROW( RAMP_SUCCESS );
                }
                else
                {
                    /* 정상 상황임 */
                }
                
                sLineEndOffset += sMBLen;
            }
            
            break;
        }
        
        if( dtlGetMbLength( sCharSet,
                            (stlChar *) aSQLString + sOffset,
                            & sMBLen,
                            QLL_ERROR_STACK(aEnv) )
            != STL_SUCCESS )
        {
            stlSnprintf( aEnv->mMessage,
                         STL_MAX_ERROR_MESSAGE,
                         "string position : %d\n",
                         aPosition );
            
            (void)stlPopError( QLL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_SUCCESS );
        }
        else
        {
            /* 정상 상황임 */
        }

        if ( (sMBLen == 1) && (aSQLString[sOffset] == '\n') )
        {
            sLineNo++;
            sLineStartOffset = sOffset + 1;
        }
        
        sOffset += sMBLen;
    }

    /**
     * 한 라인의 길이가 너무 길 경우 시작 위치를 조정한다.
     */
    
    while ( ( (sLineEndOffset - sLineStartOffset) * 2 + 20 ) > STL_MAX_ERROR_MESSAGE )
    {
        if( dtlGetMbLength( sCharSet,
                            aSQLString + sLineStartOffset,
                            & sMBLen,
                            QLL_ERROR_STACK(aEnv) )
            != STL_SUCCESS )
        {
            (void)stlPopError( QLL_ERROR_STACK(aEnv) );
            sLineStartOffset++;
        }
        else
        {
            sLineStartOffset += sMBLen;
        }
    }
    
    stlSnprintf( aEnv->mMessage,
                 STL_MAX_ERROR_MESSAGE,
                 "\n" );

    for ( sOffset = sLineStartOffset; sOffset < sLineEndOffset; sOffset++ )
    {
        stlSnprintf( aEnv->mMessage,
                     STL_MAX_ERROR_MESSAGE,
                     "%s%c",
                     aEnv->mMessage,
                     aSQLString[sOffset] );
    }

    stlSnprintf( aEnv->mMessage,
                 STL_MAX_ERROR_MESSAGE,
                 "%s\n",
                 aEnv->mMessage );

    for ( sOffset = sLineStartOffset; sOffset < aPosition;  )
    {
        if( dtlGetMbLength( sCharSet,
                            (stlChar *) aSQLString + sOffset,
                            & sMBLen,
                            QLL_ERROR_STACK(aEnv) )
            != STL_SUCCESS )
        {
            stlSnprintf( aEnv->mMessage,
                         STL_MAX_ERROR_MESSAGE,
                         "string position : %d\n",
                         aPosition );
            (void)stlPopError( QLL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_SUCCESS );
        }
        else
        {
            if ( sMBLen > 1 )
            {
                stlSnprintf( aEnv->mMessage,
                             STL_MAX_ERROR_MESSAGE,
                             "%s  ",
                             aEnv->mMessage );
            }
            else
            {
                stlSnprintf( aEnv->mMessage,
                             STL_MAX_ERROR_MESSAGE,
                             "%s ",
                             aEnv->mMessage );
            }
            sOffset += sMBLen;
        }
    }
    stlSnprintf( aEnv->mMessage,
                 STL_MAX_ERROR_MESSAGE,
                 "%s%c\n",
                 aEnv->mMessage,
                 '*' );
    
    stlSnprintf( aEnv->mMessage,
                 STL_MAX_ERROR_MESSAGE,
                 "%sERROR at line %d:\n",
                 aEnv->mMessage,
                 sLineNo + 1);

    STL_RAMP( RAMP_SUCCESS );
    
    return aEnv->mMessage;
}


/**
 * @brief 주어진 SQL String 을 Parsing 하고 Parse Tree 를 생성한다.
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aSQLStmt         SQL Statement
 * @param[in]  aQueryPhase      Query Phase
 * @param[in]  aSQLString       SQL Plain Text
 * @param[in]  aIsRecompile     Recompile 여부 
 * @param[in]  aSearchPlanCache Plan Cache 탐색 여부 
 * @param[out] aParseTree       Parse Tree
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlgParseSQL( qllDBCStmt    * aDBCStmt,
                       qllStatement  * aSQLStmt,
                       qllQueryPhase   aQueryPhase,
                       stlChar       * aSQLString,
                       stlBool         aIsRecompile,
                       stlBool         aSearchPlanCache,
                       qllNode      ** aParseTree,
                       qllEnv        * aEnv )
{
    qllNode       * sParseTree = NULL;
    
    stlInt32           sBindCount = 0;
    knlPlanSqlInfo     sSqlInfo;
    knlPlanUserInfo    sUserInfo;
    knlPlanCursorInfo  sCursorInfo;
    stlInt64           sAuthID = ELL_DICT_OBJECT_ID_NA;

    QLL_OPT_DECLARE( stlTime sBeginTime );
    QLL_OPT_DECLARE( stlTime sEndTime );


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aQueryPhase > QLL_PHASE_NA) && (aQueryPhase < QLL_PHASE_MAX), QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    QLL_OPT_CHECK_TIME( sBeginTime );

    /**
     * Alloc 가능한 Memory 설정
     */
    
    qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_PARSING, aEnv );

    /**
     * search sql cache
     */

    /* parsing 단계이므로, handle 이 참조하고 있는 정보가 aging 될수 있다. */
    /* handle 로부터 정보를 얻지 않고 Session Environment 로부터 Auth ID 를 얻는다. */
    sAuthID = ellGetSessionAuthID( ELL_ENV(aEnv) );
    
    knlSetPlanUserInfo( &sUserInfo,
                        sAuthID );

    knlSetPlanStmtCursorInfo( & sCursorInfo,
                              aSQLStmt->mStmtCursorProperty.mIsDbcCursor,
                              aSQLStmt->mStmtCursorProperty.mProperty.mStandardType,
                              aSQLStmt->mStmtCursorProperty.mProperty.mSensitivity,
                              aSQLStmt->mStmtCursorProperty.mProperty.mScrollability,
                              aSQLStmt->mStmtCursorProperty.mProperty.mHoldability,
                              aSQLStmt->mStmtCursorProperty.mProperty.mUpdatability,
                              aSQLStmt->mStmtCursorProperty.mProperty.mReturnability );
    
    /**
     * Plan Cache를 강제적으로 사용하지 못하는 경우도 있다.
     *  - SQL은 Cache되어 있는데 적당한 Plan이 없는 경우
     */

    if ( aSearchPlanCache == STL_TRUE )
    {
        STL_TRY( knlFixSql( aSQLString,
                            stlStrlen( aSQLString ),
                            & sUserInfo,
                            & sCursorInfo,
                            & aSQLStmt->mSqlHandle,
                            & sSqlInfo,
                            KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    if( aSQLStmt->mSqlHandle != NULL )
    {
        aSQLStmt->mStmtType  = sSqlInfo.mStmtType;
        aSQLStmt->mBindCount = sSqlInfo.mBindCount;
        aSQLStmt->mStmtAttr  = qlgGetStatementAttr( sSqlInfo.mStmtType );
    }
    else
    {
        /**
         * Fixed Table 정보 누적
         */

        qlgAddCallCntParse( aEnv );

        /**
         * SQL Parsing
         */

        STL_TRY( qlgParseInternal( aDBCStmt,
                                   aSQLString,
                                   & sParseTree,
                                   & sBindCount,
                                   aEnv )
                 == STL_SUCCESS );

        /**
         * SQL statement 가 아닌 경우 (syntax replacement 를 위한 phrase 인 경우
         */
    
        STL_TRY_THROW( (sParseTree->mType > QLL_STMT_NA) && (sParseTree->mType < QLL_STMT_MAX),
                       RAMP_ERR_INVALID_SQL_STATEMENT );

        /**
         * SQL Statement 에 구문 유형 정보 설정 
         */

        aSQLStmt->mStmtAttr = qlgGetStatementAttr( sParseTree->mType );
        aSQLStmt->mStmtType = sParseTree->mType;
        aSQLStmt->mBindCount = sBindCount;
    }

    /**
     * DDL에서의 Bind Parameter 사용 체크
     */
    
    if( qllIsDDL( aSQLStmt ) == STL_TRUE )
    {
        STL_TRY_THROW( aSQLStmt->mBindCount == 0, RAMP_ERR_BIND_DDL );
    }
    
    /**
     * Statement Attribute에 대한 설정이 필요함
     */
    
    STL_TRY_THROW( aSQLStmt->mStmtAttr != QLL_STMT_ATTR_NA, RAMP_ERR_NOT_IMPLEMENTED );

    /**
     * SQL Syntax 저장
     */

    if ( aIsRecompile == STL_TRUE )
    {
        /**
         * Recompile 하는 경우임
         */
    }
    else
    {
        /**
         * SQL 을 저장
         */
        
        STL_TRY( knlClearRegionMem( & aDBCStmt->mShareMemSyntaxSQL,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( & aDBCStmt->mShareMemSyntaxSQL,
                                    stlStrlen(aSQLString) + 1,
                                    (void **) & aSQLStmt->mRetrySQL,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( aSQLStmt->mRetrySQL, aSQLString );
        stlStrncpy( aSQLStmt->mStmtSQL, aSQLString, QLL_MAX_STMT_SQL_SIZE );
    }
    
    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aSQLStmt->mOptInfo.mParseTime, sBeginTime, sEndTime );
    QLL_OPT_ADD_COUNT( aSQLStmt->mOptInfo.mParseCallCount, 1 );


    /**
     * Output 설정
     */

    *aParseTree = sParseTree;

    /**
     * Alloc 가능한 Memory 원복
     */
    
    qlgSetQueryPhaseMemMgr( aDBCStmt, aQueryPhase, aEnv );

    STL_DASSERT( aSQLStmt->mLongTypeValueList.mCount == 0 );    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_SQL_STATEMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYNTAX_ERROR,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      qlgMakeErrPosString( aSQLString, 0, aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "qllParseSQL()" );
    }
    
    STL_CATCH( RAMP_ERR_BIND_DDL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BIND_VARIABLES_NOT_ALLOWED_FOR_DDL,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    qlgDisableQueryPhaseMemMgr( aDBCStmt, aEnv );
    
    (void) knlClearRegionMem( & aEnv->mHeapMemParsing, KNL_ENV(aEnv) );

    aSQLStmt->mSqlHandle = NULL;
    aSQLStmt->mInitPlan  = NULL;
    aSQLStmt->mCodePlan  = NULL;
    aSQLStmt->mDataPlan  = NULL;

    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aSQLStmt->mOptInfo.mParseTime, sBeginTime, sEndTime );
    QLL_OPT_ADD_COUNT( aSQLStmt->mOptInfo.mParseCallCount, 1 );

    STL_DASSERT( aSQLStmt->mLongTypeValueList.mCount == 0 );

    return STL_FAILURE;
}


/**
 * @brief 주어진 SQL Statement 및 SQL Phrase 을 Parsing 하고 Parse Tree 를 생성한다.
 * @param[in]  aDBCStmt        DBC Statement
 * @param[in]  aSQLString      SQL Plain Text
 * @param[out] aParseTree      Parse Tree
 * @param[out] aBindCount      Bind Variable Count
 * @param[in]  aEnv            Environment
 * @todo
 * aBindIO 인자는 Validation 단계에서 전체 param list 를 관리하는 체계가 수립되면, 제거되어야 함.
 * @remarks
 * SQL layer 외부에서 호출하지 않는다.
 */
stlStatus qlgParseInternal( qllDBCStmt    * aDBCStmt,
                            stlChar       * aSQLString,
                            qllNode      ** aParseTree,
                            stlInt32      * aBindCount,
                            qllEnv        * aEnv )
{
    stlInt32  sState = 0;
    qllNode * sParseTree = NULL;
    
    stlInt32      sBindCnt = 0;

    stlParseParam     sParseParam;
    
    qlpParseContext   sParseContext;

    stlMemset( & sParseParam, 0x00, STL_SIZEOF(stlParseParam) );
    stlMemset( & sParseContext, 0x00, STL_SIZEOF(qlpParseContext) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBindCount != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Parser 초기화
     */

    sParseContext.mMemMgr    = & aEnv->mHeapMemParsing;
    sParseContext.mEnv       = aEnv;

    sState = 0;
    
    STL_INIT_PARSE_PARAM( & sParseParam,
                          aSQLString,
                          qlpAllocator,
                          QLL_ERROR_STACK(aEnv),
                          & sParseContext );

    sState = 1;

    
    /**
     * Parsing
     */


    STL_TRY_THROW( STL_PARSER_PARSE( & sParseParam, sParseParam.mScanner ) == 0,
                   RAMP_ERR_SYNTAX);

    STL_TRY_THROW( sParseParam.mParseTree != NULL, RAMP_ERR_SYNTAX );

    /* codesonar */
    sState = 2;
    STL_PARSER_LEX_DESTROY( sParseParam.mScanner );
    sState = 0;
    
    sParseTree = (qllNode *) sParseParam.mParseTree;
    
    if( sParseContext.mParamList != NULL )
    {
        sBindCnt = QLP_LIST_GET_COUNT( sParseContext.mParamList );
    }
    else
    {
        sBindCnt = 0;
    }

    /**
     * Output 설정
     */

    *aParseTree  = sParseTree;
    *aBindCount  = sBindCnt;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SYNTAX )
    {
        if( stlGetLastErrorData( QLL_ERROR_STACK(aEnv) ) == NULL )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_SYNTAX_ERROR,
                          sParseParam.mErrMsg,
                          QLL_ERROR_STACK(aEnv) );
        }
    }

    STL_FINISH;

    switch( sState )
    {
        case 0 :
            switch( errno )
            {
                case ENOMEM :
                    stlPushError( STL_ERROR_LEVEL_ABORT,
                                  STL_ERRCODE_OUT_OF_MEMORY,
                                  "error occurred on flex/bison",
                                  QLL_ERROR_STACK(aEnv) );
                    break;
                case EINVAL:
                    stlPushError( STL_ERROR_LEVEL_ABORT,
                                  STL_ERRCODE_INVALID_ARGUMENT,
                                  "error occurred on flex/bison",
                                  QLL_ERROR_STACK(aEnv) );
                    break;
                default :
                    STL_DASSERT( 0 );
                    break;
            }
            break;
            
        case 1 :
            STL_PARSER_LEX_DESTROY( sParseParam.mScanner );
            break;
            
        default :
            STL_DASSERT( 0 );
            break;
    }
    
    (void) knlClearRegionMem( & aEnv->mHeapMemParsing, KNL_ENV(aEnv) );
    
    return STL_FAILURE;
}



/**
 * @brief SQL Statement 객체의 Parse Tree 에 Ini Plan 정보를 생성하고, 구문의 유효성을 검사한다.
 * @param[in] aTransID    Transaction ID
 * @param[in] aDBCStmt    DBC Statement
 * @param[in] aSQLStmt    SQL Statement
 * @param[in] aQueryPhase Query Phase
 * @param[in] aSQLString  SQL String
 * @param[in] aParseTree  Parse Tree
 * @param[in] aEnv        Environment
 * @remarks
 */
stlStatus qlgValidateSQL( smlTransId      aTransID,
                          qllDBCStmt    * aDBCStmt,
                          qllStatement  * aSQLStmt,
                          qllQueryPhase   aQueryPhase,
                          stlChar       * aSQLString,
                          qllNode       * aParseTree,
                          qllEnv        * aEnv )
{
    qllValidateFunc sValidateFunc = NULL;

    stlUInt32        sSessionID = 0;
    stlBool          sAuthModified = STL_FALSE;
    qllNode        * sParseTree;
    knlPlanSqlInfo   sSqlInfo;

    QLL_OPT_DECLARE( stlTime sBeginTime );
    QLL_OPT_DECLARE( stlTime sEndTime );

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aQueryPhase > QLL_PHASE_NA) && (aQueryPhase < QLL_PHASE_MAX), QLL_ERROR_STACK(aEnv) );

    QLL_OPT_CHECK_TIME( sBeginTime );

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLG_VALIDATE_SQL_BEFORE, KNL_ENV(aEnv) );
    
    sParseTree = aParseTree;

    /**
     * Alloc 가능한 Memory 설정
     */
    
    qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_VALIDATION, aEnv );
    
    /**
     * 해당 SQL 구문을 수행 가능한 상태인지 검사
     */
    
    STL_TRY( qlgCheckExecutableStatement( aSQLStmt->mStmtType, aEnv ) == STL_SUCCESS );
    
    /**
     * Session Authorization 정보의 유효성 검사
     */

    STL_TRY( ellValidateSessionAuth( aTransID,
                                     aSQLStmt->mViewSCN,
                                     & sAuthModified,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * 이미 Plan이 Caching되어 있는 경우는 Validation 과정을
     * 생략한다.
     */

    if( aSQLStmt->mSqlHandle != NULL )
    {
        /**
         * Flange 에 유효한 plan 이 존재하는 지 검사
         */

        STL_TRY( qlgGetValidCacheInitPlan( aTransID,
                                           aSQLStmt,
                                           aSQLStmt->mSqlHandle,
                                           & aSQLStmt->mInitPlan,
                                           aEnv )
                 == STL_SUCCESS );

        if ( aSQLStmt->mInitPlan != NULL )
        {
            /**
             * 유효한 Plan 이 존재하는 경우
             */

            knlGetPlanSqlInfo( aSQLStmt->mSqlHandle, & sSqlInfo );

            aSQLStmt->mGrantedPrivCount = sSqlInfo.mPrivCount;
            if ( sSqlInfo.mPrivCount > 0 )
            {
                STL_TRY( knlAllocRegionMem( & aDBCStmt->mShareMemStmt,
                                            sSqlInfo.mPrivArraySize,
                                            (void **) & aSQLStmt->mGrantedPrivArray,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            
                stlMemcpy( aSQLStmt->mGrantedPrivArray,
                           sSqlInfo.mPrivArray,
                           sSqlInfo.mPrivArraySize );
            }
            else
            {
                aSQLStmt->mGrantedPrivArray = NULL;
            }
        
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            /**
             * 유효한 Plan 이 없는 경우
             */

            /**
             * Plan을 삭제한다.
             */
            
            STL_TRY( knlDiscardSql( aSQLStmt->mSqlHandle,
                                    KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            STL_TRY( knlUnfixSql( aSQLStmt->mSqlHandle,
                                  KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            aSQLStmt->mSqlHandle = NULL;
            
            /**
             * Parsing 부터 수행한다.
             */
            
            STL_TRY( qlgInit4RecompileSQL( aDBCStmt,
                                           aSQLStmt,
                                           aEnv )
                     == STL_SUCCESS );
            
            STL_TRY( qlgParseSQL( aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_VALIDATION,
                                  aSQLString,
                                  STL_TRUE,   /* RECOMPILE */
                                  STL_FALSE,  /* Search Plan Cache */
                                  & sParseTree,
                                  aEnv )
                     == STL_SUCCESS );
        }
    }
    
    STL_PARAM_VALIDATE( sParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Fixed Table 정보 누적
     */

    qlgAddCallCntValidate( aEnv );

    while( 1 )
    {
        /**
         * Granted Privilege List 초기화
         */

        STL_TRY( ellInitPrivList( & aSQLStmt->mGrantedPrivList,
                                  STL_FALSE,  /* aForRevoke */
                                  & aDBCStmt->mShareMemStmt,
                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Cycle Detector 초기화
         */

        STL_TRY( ellInitObjectList( & aSQLStmt->mViewCycleList,
                                    & aDBCStmt->mShareMemStmt,
                                    ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * 구문별 함수 Pointer 를 수행
         */

        aSQLStmt->mKeepParseTree = STL_FALSE;
    
        sValidateFunc = qlgGetValidateFuncSQL( aSQLStmt->mStmtType );

        if( sValidateFunc( aTransID,
                           aDBCStmt,
                           aSQLStmt,
                           aSQLString,
                           sParseTree,
                           aEnv ) == STL_FAILURE )
        {
            /**
             * Plan Cache Memory를 할당할수 없다면
             */
            
            if( stlCheckErrorCode( QLL_ERROR_STACK(aEnv),
                                   KNL_ERRCODE_INSUFFICIENT_PLAN_CACHE_MEMORY )
                == STL_TRUE )
            {
                STL_DASSERT( aSQLStmt->mSqlHandle == NULL );

                STL_INIT_ERROR_STACK( QLL_ERROR_STACK(aEnv) );
                
                /**
                 * Plan Memory를 사용하지 않는다.
                 */
                
                qllPlanCacheMemOFF( aDBCStmt );
            
                /**
                 * Heap Memory를 사용해서 Validate를 수행한다.
                 */

                STL_TRY( qlgInit4RecompileSQL( aDBCStmt,
                                               aSQLStmt,
                                               aEnv )
                         == STL_SUCCESS );
                
                STL_TRY( qlgParseSQL( aDBCStmt,
                                      aSQLStmt,
                                      QLL_PHASE_VALIDATION,
                                      aSQLString,
                                      STL_TRUE,   /* RECOMPILE */
                                      STL_FALSE,  /* aSearchPlanCache */
                                      & sParseTree,
                                      aEnv )
                         == STL_SUCCESS );

                continue;
            }
            else
            {
                STL_TRY( STL_FALSE );
            }
        }

        STL_TRY( ellMakePrivList2Array( aSQLStmt->mGrantedPrivList,
                                        & aSQLStmt->mGrantedPrivCount,
                                        & aSQLStmt->mGrantedPrivArray,
                                        & aDBCStmt->mShareMemStmt,
                                        ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Validation 단계 이후로 list 를 접근해서는 안됨 */
        aSQLStmt->mGrantedPrivList = NULL;
        
        break;
    }

    /**
     * 구문 공통으로 Bind Parameter List와 Alias List 구성
     *  : parser context 정보를 Validation 정보로 구축
     */

    if ( aSQLStmt->mKeepParseTree == STL_TRUE )
    {
        /* parse tree 를 유지함 */
    }
    else
    {
        STL_TRY( knlClearRegionMem( & aEnv->mHeapMemParsing,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    
    STL_RAMP( RAMP_FINISH );

    STL_DASSERT( aSQLStmt->mCodePlan == NULL );

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLG_VALIDATE_SQL_COMPLETE, KNL_ENV(aEnv) );
 
    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aSQLStmt->mOptInfo.mValidateTime, sBeginTime, sEndTime );
    QLL_OPT_ADD_COUNT( aSQLStmt->mOptInfo.mValidateCallCount, 1 );

    /**
     * Alloc 가능한 Memory 원복
     */
    
    qlgSetQueryPhaseMemMgr( aDBCStmt, aQueryPhase, aEnv );
    
    STL_DASSERT( aSQLStmt->mLongTypeValueList.mCount == 0 );

    return STL_SUCCESS;

    STL_FINISH;

    qlgDisableQueryPhaseMemMgr( aDBCStmt, aEnv );
    
    aSQLStmt->mSqlHandle = NULL;
    aSQLStmt->mInitPlan  = NULL;
    aSQLStmt->mCodePlan  = NULL;
    aSQLStmt->mDataPlan  = NULL;

    (void) knlClearRegionMem( & aEnv->mHeapMemParsing, KNL_ENV(aEnv) );
    (void) knlClearRegionMem( & aDBCStmt->mShareMemInitPlan, KNL_ENV(aEnv) );

    /**
     * DDL Trace Message
     */
    
    if ( knlGetPropertyBoolValueByID( KNL_PROPERTY_TRACE_DDL, KNL_ENV(aEnv) ) == STL_TRUE )
    {
        if ( qllIsDDL( aSQLStmt ) == STL_TRUE )
        {
            (void)knlGetSessionEnvId( (void*) ELL_SESS_ENV(aEnv),
                                      & sSessionID,
                                      KNL_ENV(aEnv) );
            
            knlLogMsg( NULL,
                       KNL_ENV( aEnv ),
                       KNL_LOG_LEVEL_INFO,
                       "[SESSION:%d][DDL failure] %s\n",
                       sSessionID,
                       aSQLStmt->mRetrySQL );
            
        }
        else
        {
            /* nothing to do */
        }
    }
    else
    {
        /* nothing to do */
    }

    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aSQLStmt->mOptInfo.mValidateTime, sBeginTime, sEndTime );
    QLL_OPT_ADD_COUNT( aSQLStmt->mOptInfo.mValidateCallCount, 1 );

    STL_DASSERT( aSQLStmt->mLongTypeValueList.mCount == 0 );

    return STL_FAILURE;
}




/**
 * @brief SQL Statement 의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aQueryPhase   Query Phase
 * @param[in] aEnv       Environment
 * @remarks
 * Bind Context 이후 Optimize Data Area 이전에 수행되어야 한다.
 */
stlStatus qlgOptimizeCodeSQL( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllQueryPhase   aQueryPhase,
                              qllEnv        * aEnv )
{
    qllCodeOptFunc    sCodeOptimizeFunc = NULL;

    stlUInt32         sSessionID = 0;
    stlBool           sAuthModified = STL_FALSE;
    knlPlanUserInfo   sUserInfo;
    knlPlanSqlInfo    sSqlInfo;
    knlPlanCursorInfo sCursorInfo;
    ellDictHandle   * sAuthHandle;
    knlSqlHandle      sSqlHandle;

    QLL_OPT_DECLARE( stlTime sBeginTime );
    QLL_OPT_DECLARE( stlTime sEndTime );


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aQueryPhase > QLL_PHASE_NA) && (aQueryPhase < QLL_PHASE_MAX), QLL_ERROR_STACK(aEnv) );

    QLL_OPT_CHECK_TIME( sBeginTime );

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLG_CODE_OPTIMIZE_SQL_BEFORE, KNL_ENV(aEnv) );

    /**
     * Alloc 가능한 Memory 설정
     */
    
    qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_CODE_OPTIMIZE, aEnv );
    
    /**
     * Session Authorization 정보의 유효성 검사
     */

    STL_TRY( ellValidateSessionAuth( aTransID,
                                     aSQLStmt->mViewSCN,
                                     & sAuthModified,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Parameter Validation
     */

    if( aSQLStmt->mBindCount > 0 )
    {
        STL_TRY_THROW( aSQLStmt->mBindContext != NULL,
                       RAMP_ERR_WRONG_NUMBER_OF_PARAMETERS );

        STL_TRY_THROW( knlGetBindContextSize( aSQLStmt->mBindContext ) >=
                       aSQLStmt->mBindCount,
                       RAMP_ERR_WRONG_NUMBER_OF_PARAMETERS );

        STL_TRY( qlgValidateBindParamDataType( aSQLStmt->mBindCount,
                                               aSQLStmt->mBindContext,
                                               aEnv )
                 == STL_SUCCESS );
    }

    sSqlHandle = aSQLStmt->mSqlHandle;

    while( 1 )
    {
        if( sSqlHandle == NULL )
        {
            /**
             * Fixed Table 정보 누적
             */

            qlgAddCallCntOptCode( aEnv );

            /**
             * 구문별 함수 Pointer 를 수행
             */

            sCodeOptimizeFunc = qlgGetCodeOptimizeFuncSQL( aSQLStmt->mStmtType );
            
            if( sCodeOptimizeFunc( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   aEnv ) == STL_FAILURE )
            {
                /**
                 * Plan Cache Memory를 할당할수 없다면
                 */

                if( stlCheckErrorCode( QLL_ERROR_STACK(aEnv),
                                       KNL_ERRCODE_INSUFFICIENT_PLAN_CACHE_MEMORY )
                    == STL_TRUE )
                {
                    STL_INIT_ERROR_STACK( QLL_ERROR_STACK(aEnv) );
                    
                    /**
                     * Allocator를 Non-Cacheable Memory로 변경한다.
                     */
                
                    qllPlanCacheMemOFF( aDBCStmt );

                    if( aSQLStmt->mSqlHandle != NULL )
                    {
                        STL_TRY( knlUnfixSql( aSQLStmt->mSqlHandle,
                                              KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        aSQLStmt->mSqlHandle = NULL;
                    }

                    /**
                     * Heap Memory를 사용해서 Code Plan을 만든다.
                     */
                
                    STL_TRY( qlgRecompileSQL( aTransID,
                                              aDBCStmt,
                                              aSQLStmt,
                                              STL_FALSE,  /* aSearchPlanCache */
                                              QLL_PHASE_CODE_OPTIMIZE,
                                              aEnv )
                             == STL_SUCCESS );

                    continue;
                }
                else
                {
                    STL_TRY( STL_FALSE );
                }
            }
            else
            {
                /**
                 * cacheable plan이라면
                 */

                if( knlGetParentMemType( aDBCStmt->mShareMemCodePlan ) != KNL_MEM_STORAGE_TYPE_PLAN )
                {
                    break;
                }

                
                /**
                 * Recomplile시 Plan Cache를 참조한 경우
                 */
                
                if( aSQLStmt->mSqlHandle != NULL )
                {
                    break;
                }

                /**
                 * 변경되고 있는 object를 포함한 Plan은 caching하지 않는다.
                 */
                
                if( qlgHasUncommittedObject( aSQLStmt, aEnv ) == STL_FALSE )
                {
                    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
                
                    knlSetPlanUserInfo( &sUserInfo,
                                        ellGetAuthID( sAuthHandle ) );

                    knlSetPlanSqlInfo( &sSqlInfo,
                                       qllNeedFetch( aSQLStmt ),  
                                       aSQLStmt->mStmtType,
                                       aSQLStmt->mBindCount,
                                       aSQLStmt->mGrantedPrivCount,
                                       STL_ALIGN( aSQLStmt->mGrantedPrivCount * STL_SIZEOF(ellPrivItem), 8 ),
                                       aSQLStmt->mGrantedPrivArray );
                    
                    knlSetPlanStmtCursorInfo( & sCursorInfo,
                                              aSQLStmt->mStmtCursorProperty.mIsDbcCursor,
                                              aSQLStmt->mStmtCursorProperty.mProperty.mStandardType,
                                              aSQLStmt->mStmtCursorProperty.mProperty.mSensitivity,
                                              aSQLStmt->mStmtCursorProperty.mProperty.mScrollability,
                                              aSQLStmt->mStmtCursorProperty.mProperty.mHoldability,
                                              aSQLStmt->mStmtCursorProperty.mProperty.mUpdatability,
                                              aSQLStmt->mStmtCursorProperty.mProperty.mReturnability );
                    
                    STL_TRY( knlCacheSql( aSQLStmt->mRetrySQL,
                                          stlStrlen( aSQLStmt->mRetrySQL ),
                                          QLL_CODE_PLAN( aDBCStmt ),
                                          aSQLStmt->mCodePlan,
                                          & sUserInfo,
                                          & sCursorInfo,
                                          & sSqlInfo,
                                          qlgComparePlan,
                                          & aSQLStmt->mSqlHandle,
                                          KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
            }

            break;
        }
        else
        {
            /**
             * Validation 단계에서 설정한 Cached Init Plan 을 초기화한다.
             */

            aSQLStmt->mInitPlan = NULL;
            
            /**
             * 적당한 Code Plan을 찾는다.
             */

            STL_TRY( qlgGetValidCacheCodePlan( aSQLStmt,
                                               aSQLStmt->mSqlHandle,
                                               & aSQLStmt->mCodePlan,
                                               aEnv )
                     == STL_SUCCESS );

            if( aSQLStmt->mCodePlan != NULL )
            {
                break;
            }

            STL_TRY( knlUnfixSql( aSQLStmt->mSqlHandle,
                                  KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            aSQLStmt->mSqlHandle = NULL;

            STL_TRY( qlgRecompileSQL( aTransID,
                                      aDBCStmt,
                                      aSQLStmt,
                                      STL_FALSE, /* aSearchPlanCache */
                                      QLL_PHASE_CODE_OPTIMIZE,
                                      aEnv )
                     == STL_SUCCESS );

            /**
             * 적당한 Plan이 없기 때문에 처음부터 다시 시작한다.
             */
            
            sSqlHandle = NULL;
        }
    }

    /**
     * Trace Log 처리
     */

    if( (QLL_OPT_TRACE_CODE_PLAN_ON( aEnv ) == STL_TRUE) &&
        (aSQLStmt->mCodePlan != NULL) )
    {
        qllOptimizationNode * sNode = NULL;

        /* Node 설정 */
        sNode = (qllOptimizationNode*)(aSQLStmt->mCodePlan);

        if( QLL_IS_INIT_TRACE_LOGGER(aEnv) )
        {
            /* Trace Logger 종료 */
            STL_TRY( qllTraceDestroyLogger( aEnv ) == STL_SUCCESS );
        }

        STL_TRY( qllTraceCreateLogger( aEnv ) == STL_SUCCESS );

        QLL_SET_ENABLE_TRACE_LOGGER(aEnv);

        switch( sNode->mType )
        {
            case QLL_PLAN_NODE_STMT_SELECT_TYPE:
                /* Trace All Expression List */
                STL_TRY( qlncTraceAllExprList(
                             &QLL_CANDIDATE_MEM( aEnv ),
                             ((qlnoSelectStmt*)(sNode->mOptNode))->mStmtExprList->mAllExprList,
                             aEnv )
                         == STL_SUCCESS );

                /* Trace Code Plan */
                STL_TRY( qlncTraceTreeFromCodePlan( &QLL_CANDIDATE_MEM( aEnv ),
                                                    sNode,
                                                    aEnv )
                         == STL_SUCCESS );

                break;

            case QLL_PLAN_NODE_STMT_INSERT_TYPE:
                /* Trace All Expression List */
                STL_TRY( qlncTraceAllExprList(
                             &QLL_CANDIDATE_MEM( aEnv ),
                             ((qlnoInsertStmt*)(sNode->mOptNode))->mStmtExprList->mAllExprList,
                             aEnv )
                         == STL_SUCCESS );

                /* Trace Code Plan */
                STL_TRY( qlncTraceTreeFromCodePlan( &QLL_CANDIDATE_MEM( aEnv ),
                                                    sNode,
                                                    aEnv )
                         == STL_SUCCESS );

                break;

            case QLL_PLAN_NODE_STMT_UPDATE_TYPE:
                /* Trace All Expression List */
                STL_TRY( qlncTraceAllExprList(
                             &QLL_CANDIDATE_MEM( aEnv ),
                             ((qlnoUpdateStmt*)(sNode->mOptNode))->mStmtExprList->mAllExprList,
                             aEnv )
                         == STL_SUCCESS );

                /* Trace Code Plan */
                STL_TRY( qlncTraceTreeFromCodePlan( &QLL_CANDIDATE_MEM( aEnv ),
                                                    sNode,
                                                    aEnv )
                         == STL_SUCCESS );

                break;

            case QLL_PLAN_NODE_STMT_DELETE_TYPE:
                /* Trace All Expression List */
                STL_TRY( qlncTraceAllExprList(
                             &QLL_CANDIDATE_MEM( aEnv ),
                             ((qlnoDeleteStmt*)(sNode->mOptNode))->mStmtExprList->mAllExprList,
                             aEnv )
                         == STL_SUCCESS );

                /* Trace Code Plan */
                STL_TRY( qlncTraceTreeFromCodePlan( &QLL_CANDIDATE_MEM( aEnv ),
                                                    sNode,
                                                    aEnv )
                         == STL_SUCCESS );

                break;

            default:
                break;
        }

        QLL_SET_DISABLE_TRACE_LOGGER(aEnv);
    }

    /**
     * Running(for Candidate Plan) Memory 반납
     */

    STL_TRY( knlClearRegionMem( & aEnv->mHeapMemRunning,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLG_CODE_OPTIMIZE_SQL_COMPLETE, KNL_ENV(aEnv) );
    
    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aSQLStmt->mOptInfo.mCodeOptTime, sBeginTime, sEndTime );
    QLL_OPT_ADD_COUNT( aSQLStmt->mOptInfo.mCodeOptCallCount, 1 );

    /**
     * Alloc 가능한 Memory 원복
     */
    
    qlgSetQueryPhaseMemMgr( aDBCStmt, aQueryPhase, aEnv );
    
    STL_DASSERT( aSQLStmt->mLongTypeValueList.mCount == 0 );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_WRONG_NUMBER_OF_PARAMETERS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_WRONG_NUMBER_OF_PARAMETERS,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    qlgDisableQueryPhaseMemMgr( aDBCStmt, aEnv );
    
    /**
     * Prepare-Execution 일 수 있다.
     */
    (void) qllCleanMemBeforeOptimizeCodeSQL( aDBCStmt, aSQLStmt, aEnv );

    
    /**
     * DDL Trace Message
     */
    
    if ( knlGetPropertyBoolValueByID( KNL_PROPERTY_TRACE_DDL, KNL_ENV(aEnv) ) == STL_TRUE )
    {
        if ( qllIsDDL( aSQLStmt ) == STL_TRUE )
        {
            (void) knlGetSessionEnvId( (void*) ELL_SESS_ENV(aEnv),
                                       & sSessionID,
                                       KNL_ENV(aEnv) );
        
            knlLogMsg( NULL,
                       KNL_ENV( aEnv ),
                       KNL_LOG_LEVEL_INFO,
                       "[SESSION:%d][DDL failure] %s\n",
                       sSessionID,
                       aSQLStmt->mRetrySQL );
            
        }
        else
        {
            /* nothing to do */
        }
    }
    else
    {
        /* nothing to do */
    }

    aSQLStmt->mSqlHandle = NULL;
    aSQLStmt->mCodePlan  = NULL;
    aSQLStmt->mDataPlan  = NULL;

    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aSQLStmt->mOptInfo.mCodeOptTime, sBeginTime, sEndTime );
    QLL_OPT_ADD_COUNT( aSQLStmt->mOptInfo.mCodeOptCallCount, 1 );

    STL_DASSERT( aSQLStmt->mLongTypeValueList.mCount == 0 );

    return STL_FAILURE;
}



/**
 * @brief SQL Statement 의 최적화된 Data Area 를 생성한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aQueryPhase  Query Phase
 * @param[in] aEnv       Environment
 * @remarks
 * Data Area 는 Plan Cache 대상이 되지 않는 영역이다.
 * Optimize Code Area 이후 Execute SQL 전에 수행되어야 한다.
 */
stlStatus qlgOptimizeDataSQL( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllQueryPhase   aQueryPhase,
                              qllEnv        * aEnv )
{
    qllDataOptFunc  sDataOptimizeFunc = NULL;

    stlUInt32  sSessionID = 0;
    stlBool    sAuthModified = STL_FALSE;

    QLL_OPT_DECLARE( stlTime sBeginTime );
    QLL_OPT_DECLARE( stlTime sEndTime );


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aQueryPhase > QLL_PHASE_NA) && (aQueryPhase < QLL_PHASE_MAX), QLL_ERROR_STACK(aEnv) );

    QLL_OPT_CHECK_TIME( sBeginTime );

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLG_DATA_OPTIMIZE_SQL_BEFORE, KNL_ENV(aEnv) );

    /**
     * Alloc 가능한 Memory 설정
     */
    
    qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_DATA_OPTIMIZE, aEnv );
    
    /**
     * Fixed Table 정보 누적
     */

    qlgAddCallCntOptData( aEnv );

    /**
     * Session Authorization 정보의 유효성 검사
     */

    STL_TRY( ellValidateSessionAuth( aTransID,
                                     aSQLStmt->mViewSCN,
                                     & sAuthModified,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Parameter Validation
     */

    if( aSQLStmt->mBindCount > 0 )
    {
        STL_TRY( qlgValidateBindParamDataType( aSQLStmt->mBindCount,
                                               aSQLStmt->mBindContext,
                                               aEnv )
                 == STL_SUCCESS );
    }

    /**
     * 구문별 함수 Pointer 를 수행
     */

    sDataOptimizeFunc  = qlgGetDataOptimizeFuncSQL( aSQLStmt->mStmtType );

    STL_TRY( sDataOptimizeFunc( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                aEnv )
             == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLG_DATA_OPTIMIZE_SQL_COMPLETE, KNL_ENV(aEnv) );
    
    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aSQLStmt->mOptInfo.mDataOptTime, sBeginTime, sEndTime );
    QLL_OPT_ADD_COUNT( aSQLStmt->mOptInfo.mDataOptCallCount, 1 );

    /**
     * Alloc 가능한 Memory 원복
     */
    
    qlgSetQueryPhaseMemMgr( aDBCStmt, aQueryPhase, aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    qlgDisableQueryPhaseMemMgr( aDBCStmt, aEnv );
    
    /**
     * Prepare-Execution 일 수 있다.
     */
    (void) qllCleanMemBeforeOptimizeDataSQL( aDBCStmt, aSQLStmt, aEnv );

    /**
     * DDL Trace Message
     */
    
    if ( knlGetPropertyBoolValueByID( KNL_PROPERTY_TRACE_DDL, KNL_ENV(aEnv) ) == STL_TRUE )
    {
        if ( qllIsDDL( aSQLStmt ) == STL_TRUE )
        {
            (void) knlGetSessionEnvId( (void*) ELL_SESS_ENV(aEnv),
                                       & sSessionID,
                                       KNL_ENV(aEnv) );
        
            knlLogMsg( NULL,
                       KNL_ENV( aEnv ),
                       KNL_LOG_LEVEL_INFO,
                       "[SESSION:%d][DDL failure] %s\n",
                       sSessionID,
                       aSQLStmt->mRetrySQL );
            
        }
        else
        {
            /* nothing to do */
        }
    }
    else
    {
        /* nothing to do */
    }
    
    aSQLStmt->mDataPlan = NULL;

    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aSQLStmt->mOptInfo.mDataOptTime, sBeginTime, sEndTime );
    QLL_OPT_ADD_COUNT( aSQLStmt->mOptInfo.mDataOptCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief SQL Statement 권한을 다시 검사한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aQueryPhase   Query Phase
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlgRecheckPrivSQL( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             qllQueryPhase   aQueryPhase,
                             qllEnv        * aEnv )
{
    stlBool        sAuthModified = STL_FALSE;
    stlBool        sGranted = STL_TRUE;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aQueryPhase > QLL_PHASE_NA) && (aQueryPhase < QLL_PHASE_MAX), QLL_ERROR_STACK(aEnv) );

    /**
     * Alloc 가능한 Memory 설정
     */
    
    qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_RECHECK_PRIVILEGE, aEnv );
    
    /**
     * Fixed Table 정보 누적
     */

    qlgAddCallCntRecheck( aEnv );

    /**
     * Session Authorization 정보의 유효성 검사
     */

    STL_TRY( ellValidateSessionAuth( aTransID,
                                     aSQLStmt->mViewSCN,
                                     & sAuthModified,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sAuthModified == STL_TRUE )
    {
        sGranted = STL_FALSE;
    }
    else
    {
        /**
         * Granted Privilege Handle 이 변경되지 않았는지 확인
         */
        
        STL_TRY( ellIsRecentPrivArray( aTransID,
                                       aSQLStmt->mGrantedPrivCount,
                                       aSQLStmt->mGrantedPrivArray,
                                       & sGranted,
                                       ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    if ( sGranted == STL_TRUE )
    {
        /**
         * Granted Privilege 가 변경되지 않음.
         */
    }
    else
    {
        /**
         * Granted Privilege 가 변경됨
         */
        
        if ( (aSQLStmt->mStmtAttr & QLL_STMT_ATTR_PLAN_CACHE_MASK) == QLL_STMT_ATTR_PLAN_CACHE_YES )
        {
            /**
             * Plan Cache 대상인 구문인 경우 Recompile 을 수행
             */

            if( aSQLStmt->mSqlHandle != NULL )
            {
                /**
                 * Plan을 삭제한다.
                 */
                
                STL_TRY( knlDiscardSql( aSQLStmt->mSqlHandle,
                                        KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_TRY( knlUnfixSql( aSQLStmt->mSqlHandle,
                                      KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                aSQLStmt->mSqlHandle = NULL;
            }

        }
        else
        {
            /**
             * RECOMPILE 대상이 아닌 구문인 경우
             */
        }

        STL_TRY( qlgRecompileSQL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  STL_TRUE, /* aSearchPlanCache */
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Alloc 가능한 Memory 원복
     */
    
    qlgSetQueryPhaseMemMgr( aDBCStmt, aQueryPhase, aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    qlgDisableQueryPhaseMemMgr( aDBCStmt, aEnv );
    
    if ( aStmt != NULL )
    {
        (void) smlRollbackStatement( aStmt, SML_ENV( aEnv ) );
    }

    return STL_FAILURE;
}


/**
 * @brief SQL Statement 를 실행한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aQueryPhase    Query Phase
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlgExecuteSQL( smlTransId      aTransID,
                         smlStatement  * aStmt,
                         qllDBCStmt    * aDBCStmt,
                         qllStatement  * aSQLStmt,
                         qllQueryPhase   aQueryPhase,
                         qllEnv        * aEnv )
{
    qllExecuteFunc      sExecuteFunc = NULL;
    stlUInt32           sSessionID = 0;
        
    QLL_OPT_DECLARE( stlTime sBeginTime );
    QLL_OPT_DECLARE( stlTime sEndTime );

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aQueryPhase > QLL_PHASE_NA) && (aQueryPhase < QLL_PHASE_MAX), QLL_ERROR_STACK(aEnv) );

    QLL_OPT_CHECK_TIME( sBeginTime );

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLG_EXECUTE_SQL_BEFORE, KNL_ENV(aEnv) );

    /**
     * Alloc 가능한 Memory 설정
     */
    
    qlgSetQueryPhaseMemMgr( aDBCStmt, QLL_PHASE_EXECUTE, aEnv );
    
    /**
     * Fixed Table 정보 누적
     */

    qlgAddCallCntExecute( aSQLStmt, aEnv );

    /**
     * Constraint Violation 이 존재할 경우 DDL 을 수행할 수 없다.
     */
    if ( qllIsDDL( aSQLStmt ) == STL_TRUE )
    {
        STL_TRY_THROW( qlrcHasViolatedDeferredConstraint( aEnv ) != STL_TRUE,
                       RAMP_ERR_INTEGRITY_CONSTRAINT_VIOLATION );
    }
    
    /**
     * Execute 단계에서는 SM Statement 가 이미 할당되어 있어 Disable Aging 을 설정할 필요가 없음.
     */

    /**
     * Parameter Validation
     */

    if( aSQLStmt->mBindCount > 0 )
    {
        STL_TRY( qlgValidateBindParamDataValue( aSQLStmt->mBindCount,
                                                aSQLStmt->mBindContext,
                                                aEnv )
                 == STL_SUCCESS );
    }

    /**
     * 구문별 함수 Pointer 를 수행
     */

    sExecuteFunc  = qlgGetExecuteFuncSQL( aSQLStmt->mStmtType );
 
    STL_TRY( sExecuteFunc( aTransID,
                           aStmt,
                           aDBCStmt,
                           aSQLStmt,
                           aEnv )
             == STL_SUCCESS );

    /**
     * DDL Statement 가 존재할 경우에
     * Statement Pending Operation 을 Transaction Pending Operation 으로 전환 
     */

    if ( qllIsDDL( aSQLStmt ) == STL_TRUE )
    {
        STL_TRY( knlClearRegionMem( & aEnv->mHeapMemRunning,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY( ellEndStmtDDL( aStmt, ELL_ENV(aEnv) ) == STL_SUCCESS );
    }
    else
    {
        /* DDL 이 아닌 경우 */
    }


    /**
     * DDL Trace Message
     */
    
    if ( knlGetPropertyBoolValueByID( KNL_PROPERTY_TRACE_DDL, KNL_ENV(aEnv) ) == STL_TRUE )
    {
        if ( qllIsDDL( aSQLStmt ) == STL_TRUE )
        {
            STL_TRY( knlGetSessionEnvId( (void*) ELL_SESS_ENV(aEnv),
                                         & sSessionID,
                                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        
            knlLogMsg( NULL,
                       KNL_ENV( aEnv ),
                       KNL_LOG_LEVEL_INFO,
                       "[SESSION:%d][DDL success] %s\n",
                       sSessionID,
                       aSQLStmt->mRetrySQL );
            
        }
        else
        {
            /* nothing to do */
        }
    }
    else
    {
        /* nothing to do */
    }

    /**
     * Long Varying Data를 초기 크기로 원복한다.
     */

    if( qllNeedFetch( aSQLStmt ) == STL_FALSE )
    {
        if( aSQLStmt->mLongTypeValueList.mCount > 0 )
        {
            STL_TRY( qlndResetLongTypeValues( & aSQLStmt->mLongTypeValueList,
                                              aEnv )
                     == STL_SUCCESS );
        }
    }

    /* statement deferred collision 정보를 추가한다. */
    qlrcStmtEndDeferConstraint( aEnv );
    
    aSQLStmt->mStmt = NULL;

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLG_EXECUTE_SQL_COMPLETE, KNL_ENV(aEnv) );
    
    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aSQLStmt->mOptInfo.mExecuteTime, sBeginTime, sEndTime );
    QLL_OPT_ADD_COUNT( aSQLStmt->mOptInfo.mExecuteCallCount, 1 );

    /**
     * Alloc 가능한 Memory 원복
     */
    
    qlgSetQueryPhaseMemMgr( aDBCStmt, aQueryPhase, aEnv );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTEGRITY_CONSTRAINT_VIOLATION )
    {
        stlChar sErrMsg[STL_MAX_ERROR_MESSAGE] = {0,};

        (void) qlrcSetViolatedHandleMessage( aTransID,
                                             sErrMsg,
                                             STL_MAX_ERROR_MESSAGE,
                                             NULL, /* aConstHandle */
                                             aEnv );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INTEGRITY_CONSTRAINT_VIOLATION,
                      sErrMsg,
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    qlgDisableQueryPhaseMemMgr( aDBCStmt, aEnv );
    
    (void) ellRollbackStmtDDL( aTransID, ELL_ENV(aEnv) );
    
    if ( aStmt != NULL )
    {
        (void) smlRollbackStatement( aStmt, SML_ENV(aEnv) );
    }

    (void) knlClearRegionMem( & aEnv->mHeapMemRunning, KNL_ENV(aEnv) );
    
    /**
     * execute시 정상적인 에러가 발생할 수 있으므로 SQL Statement 를 초기화 하면 안된다.
     */

    /**
     * DDL Trace Message
     */
    
    if ( knlGetPropertyBoolValueByID( KNL_PROPERTY_TRACE_DDL, KNL_ENV(aEnv) ) == STL_TRUE )
    {
        if ( qllIsDDL( aSQLStmt ) == STL_TRUE )
        {
            (void) knlGetSessionEnvId( (void*) ELL_SESS_ENV(aEnv),
                                       & sSessionID,
                                       KNL_ENV(aEnv) );
        
            knlLogMsg( NULL,
                       KNL_ENV( aEnv ),
                       KNL_LOG_LEVEL_INFO,
                       "[SESSION:%d][DDL failure] %s\n",
                       sSessionID,
                       aSQLStmt->mRetrySQL );
        }
        else
        {
            /* nothing to do */
        }
    }
    else
    {
        /* nothing to do */
    }
    
    if( aSQLStmt->mResultSetDesc != NULL )
    {
        (void) qlcrCloseResultSet( aSQLStmt->mResultSetDesc->mQueryStmt, aEnv );
    }

    if( aSQLStmt->mLongTypeValueList.mCount > 0 )
    {
        (void) qlndResetLongTypeValues( & aSQLStmt->mLongTypeValueList,
                                        aEnv );
    }

    qlrcStmtRollbackDeferConstraint( aEnv );
    
    aSQLStmt->mStmt = NULL;
    
    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aSQLStmt->mOptInfo.mExecuteTime, sBeginTime, sEndTime );
    QLL_OPT_ADD_COUNT( aSQLStmt->mOptInfo.mExecuteCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief SQL Recompile 을 위한 정보를 초기화한다.
 * @param[in]  aDBCStmt  DBC Statement
 * @param[in]  aSQLStmt  SQL Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus qlgInit4RecompileSQL( qllDBCStmt   * aDBCStmt,
                                qllStatement * aSQLStmt,
                                qllEnv       * aEnv )
{
    /**
     * Plan 초기화
     */
    
    aSQLStmt->mInitPlan = NULL;
    aSQLStmt->mCodePlan = NULL;
    aSQLStmt->mDataPlan = NULL;

    /**
     * Dictionary View SCN 설정
     */

    if ( aSQLStmt->mStmt != NULL )
    {
        /* EXECUTE 단계에서 Recompile 을 통해 수행되는 경우 */

        STL_TRY( smlResetStatement( aSQLStmt->mStmt,
                                    STL_FALSE, /* aDoRollback */
                                    SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        aSQLStmt->mViewSCN = aSQLStmt->mStmt->mScn;
    }
    else
    {
        /* EXECUTE 이전 단계에서 Recompile 을 통해 수행되는 경우 */
        
        aSQLStmt->mViewSCN = smlGetSystemScn();
    }
    
    /**
     * Result Set 초기화
     */

    aSQLStmt->mResultSetDesc = NULL;

    /********************************
     * Cycle Detector
     ********************************/

    aSQLStmt->mViewCycleList = NULL;
    
    /**
     * Long Type Block 정리
     */

    if( aSQLStmt->mLongTypeValueList.mCount > 0 )
    {
        STL_TRY( qlndFreeLongTypeValues( & aSQLStmt->mLongTypeValueList,
                                         aEnv )
                 == STL_SUCCESS );
    }

    STL_DASSERT( aSQLStmt->mLongTypeValueList.mCount == 0 );
    STL_DASSERT( aSQLStmt->mLongTypeValueList.mHead  == NULL );
    STL_DASSERT( aSQLStmt->mLongTypeValueList.mTail  == NULL );

    /********************************
     * Heap Memory 정리 
     ********************************/
    
    /**
     * Parsing Memory 반납
     */

    STL_TRY( knlClearRegionMem( & aEnv->mHeapMemParsing,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Running Memory 반납
     */

    STL_TRY( knlClearRegionMem( & aEnv->mHeapMemRunning,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /********************************
     * Shared Memory 정리 
     ********************************/

    /**
     * Init Plan Memory 반납
     */

    STL_TRY( knlClearRegionMem( & aDBCStmt->mShareMemInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Non-Cacheable Memory 반납
     */

    STL_TRY( knlClearRegionMem( & aDBCStmt->mNonPlanCacheMem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Cacheable Memory 반납
     */

    STL_TRY( knlClearRegionMem( & aDBCStmt->mPlanCacheMem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Data Plan Memory 반납
     */

    STL_TRY( knlClearRegionMem( & aDBCStmt->mShareMemDataPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Statement Memory 반납
     */
    
    STL_TRY( knlClearRegionMem( & aDBCStmt->mShareMemStmt,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( aSQLStmt->mLongTypeValueList.mCount > 0 )
    {
        (void) qlndFreeLongTypeValues( & aSQLStmt->mLongTypeValueList,
                                       aEnv );
    }

    STL_DASSERT( aSQLStmt->mLongTypeValueList.mCount == 0 );
    STL_DASSERT( aSQLStmt->mLongTypeValueList.mHead  == NULL );
    STL_DASSERT( aSQLStmt->mLongTypeValueList.mTail  == NULL );
    
    (void) knlClearRegionMem( & aEnv->mHeapMemParsing,
                              KNL_ENV(aEnv) );

    (void) knlClearRegionMem( & aEnv->mHeapMemRunning,
                              KNL_ENV(aEnv) );

    (void) knlClearRegionMem( & aDBCStmt->mShareMemInitPlan,
                              KNL_ENV(aEnv) );

    (void) knlClearRegionMem( & aDBCStmt->mNonPlanCacheMem,
                              KNL_ENV(aEnv) );
    
    (void) knlClearRegionMem( & aDBCStmt->mPlanCacheMem,
                              KNL_ENV(aEnv) );

    (void) knlClearRegionMem( & aDBCStmt->mShareMemDataPlan,
                              KNL_ENV(aEnv) );
    
    (void) knlClearRegionMem( & aDBCStmt->mShareMemStmt,
                              KNL_ENV(aEnv) );
    
    return STL_FAILURE;
}


/**
 * @brief 구문 수행 중 RETRY 에러 상황인 경우, Recompile 한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aDBCStmt          DBC Statement
 * @param[in]  aSQLStmt          SQL Stmt
 * @param[in]  aSearchPlanCache  Search Plan Cache
 * @param[in]  aQueryPhase       구문 수행 단계
 * @param[in]  aEnv              Environment
 * @remarks
 * RETRY 상황은 Execute 시작전에 Table Handle 등이
 * DDL 등으로 인해 변경되었을 경우 발생한다.
 */
stlStatus qlgRecompileSQL( smlTransId     aTransID,
                           qllDBCStmt   * aDBCStmt,
                           qllStatement * aSQLStmt,
                           stlBool        aSearchPlanCache,
                           qllQueryPhase  aQueryPhase,
                           qllEnv       * aEnv )
{
    qllNode         * sParseTree = NULL;
    knlPlanSqlInfo    sSqlInfo;
    
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mRetrySQL != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aQueryPhase > QLL_PHASE_VALIDATION) &&
                        (aQueryPhase <= QLL_PHASE_EXECUTE),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * Fixed Table 정보 누적
     */

    qlgAddCallCntRecompile( aEnv );

    /****************************************************
     * 사용중이던 메모리 정리
     ****************************************************/

    STL_TRY( qlgInit4RecompileSQL( aDBCStmt, aSQLStmt, aEnv ) == STL_SUCCESS );
    
    /****************************************************
     * 재컴파일 
     ****************************************************/

    if( aSearchPlanCache == STL_TRUE )
    {
        /**
         * Parsing
         */
                
        STL_TRY( qlgParseSQL( aDBCStmt,
                              aSQLStmt,
                              aQueryPhase,
                              aSQLStmt->mRetrySQL,
                              STL_TRUE,  /* RECOMPILE */
                              STL_TRUE,  /* SEARCH PLAN CACHE */
                              & sParseTree,
                              aEnv )
                 == STL_SUCCESS );

        
        /**
         * Validation
         */
        
        aSQLStmt->mInitPlan = NULL;
        aSQLStmt->mCodePlan = NULL;

        if( aSQLStmt->mSqlHandle != NULL )
        {
            /**
             * Flange 에 유효한 plan 이 존재하는 지 검사
             */

            STL_TRY( qlgGetValidCacheInitPlan( aTransID,
                                               aSQLStmt,
                                               aSQLStmt->mSqlHandle,
                                               & aSQLStmt->mInitPlan,
                                               aEnv )
                     == STL_SUCCESS );
                
            if ( aSQLStmt->mInitPlan != NULL )
            {
                /**
                 * 유효한 Plan 이 존재하는 경우
                 */

                knlGetPlanSqlInfo( aSQLStmt->mSqlHandle, & sSqlInfo );

                aSQLStmt->mGrantedPrivCount = sSqlInfo.mPrivCount;
                if ( sSqlInfo.mPrivCount > 0 )
                {
                    STL_TRY( knlAllocRegionMem( & aDBCStmt->mShareMemStmt,
                                                sSqlInfo.mPrivArraySize,
                                                (void **) & aSQLStmt->mGrantedPrivArray,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
            
                    stlMemcpy( aSQLStmt->mGrantedPrivArray,
                               sSqlInfo.mPrivArray,
                               sSqlInfo.mPrivArraySize );
                }
                else
                {
                    aSQLStmt->mGrantedPrivArray = NULL;
                }
            }
            else
            {
                /**
                 * 유효한 Plan 이 없는 경우 Plan을 삭제한다.
                 */
            
                STL_TRY( knlDiscardSql( aSQLStmt->mSqlHandle,
                                        KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            
                STL_TRY( knlUnfixSql( aSQLStmt->mSqlHandle,
                                      KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            
                aSQLStmt->mSqlHandle = NULL;
            }
        }
        

        /**
         * Search Code Plan
         */
        
        if( aSQLStmt->mSqlHandle != NULL )
        {
            STL_DASSERT( aSQLStmt->mInitPlan != NULL );
            
            STL_TRY( qlgGetValidCacheCodePlan( aSQLStmt,
                                               aSQLStmt->mSqlHandle,
                                               & aSQLStmt->mCodePlan,
                                               aEnv )
                     == STL_SUCCESS );

            if( aSQLStmt->mCodePlan == NULL )
            {
                STL_TRY( knlUnfixSql( aSQLStmt->mSqlHandle,
                                      KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                aSQLStmt->mSqlHandle = NULL;
            }
            
            aSQLStmt->mInitPlan = NULL;
        }
    }

    switch ( aQueryPhase )
    {
        case QLL_PHASE_CODE_OPTIMIZE:
            {
                if( aSQLStmt->mSqlHandle == NULL )
                {
                    /**
                     * Parsing
                     */
                
                    STL_TRY( qlgParseSQL( aDBCStmt,
                                          aSQLStmt,
                                          aQueryPhase,
                                          aSQLStmt->mRetrySQL,
                                          STL_TRUE,  /* RECOMPILE */
                                          STL_FALSE, /* SEARCH PLAN CACHE */
                                          & sParseTree,
                                          aEnv )
                             == STL_SUCCESS );

                    /**
                     * Validation
                     */

                    STL_TRY( qlgValidateSQL( aTransID,
                                             aDBCStmt,
                                             aSQLStmt,
                                             aQueryPhase,
                                             aSQLStmt->mRetrySQL,
                                             sParseTree,
                                             aEnv )
                             == STL_SUCCESS );

                    STL_DASSERT( aSQLStmt->mCodePlan == NULL );
                }
                else
                {
                    /**
                     * Code Plan을 확보하였음.
                     */
                    
                    STL_DASSERT( aSQLStmt->mCodePlan != NULL );
                    /* Do Nothing */
                }
                
                KNL_BREAKPOINT( KNL_BREAKPOINT_QLG_RECOMPILE_SQL_PHASE_CODE_OPTIMIZE, KNL_ENV(aEnv) );
                
                break;
            }
        case QLL_PHASE_DATA_OPTIMIZE:
            {
                if( aSQLStmt->mSqlHandle == NULL )
                {
                    /**
                     * Parsing
                     */
                
                    STL_TRY( qlgParseSQL( aDBCStmt,
                                          aSQLStmt,
                                          aQueryPhase,
                                          aSQLStmt->mRetrySQL,
                                          STL_TRUE,  /* RECOMPILE */
                                          STL_FALSE, /* SEARCH PLAN CACHE */
                                          & sParseTree,
                                          aEnv )
                             == STL_SUCCESS );
                
                    /**
                     * Validation
                     */
                
                    STL_TRY( qlgValidateSQL( aTransID,
                                             aDBCStmt,
                                             aSQLStmt,
                                             aQueryPhase,
                                             aSQLStmt->mRetrySQL,
                                             sParseTree,
                                             aEnv )
                             == STL_SUCCESS );

                    /**
                     * Optimize Code Area
                     */
                
                    STL_TRY( qlgOptimizeCodeSQL( aTransID,
                                                 aDBCStmt,
                                                 aSQLStmt,
                                                 aQueryPhase,
                                                 aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /**
                     * Code Plan을 확보하였음.
                     */
                    
                    STL_DASSERT( aSQLStmt->mCodePlan != NULL );
                    /* Do Nothing */
                }

                break;
            }
        case QLL_PHASE_RECHECK_PRIVILEGE:
        case QLL_PHASE_EXECUTE:
            {
                /**
                 * Execution 시점에는 SM statement 가 할당되어 관련 handle 이 aging 되지 않음.
                 * - 따라서, Enable/Disable Aging 을 고려할 필요가 없음.
                 */
                
                if( aSQLStmt->mSqlHandle == NULL )
                {
                    /**
                     * Parsing
                     */
                
                    STL_TRY( qlgParseSQL( aDBCStmt,
                                          aSQLStmt,
                                          aQueryPhase,
                                          aSQLStmt->mRetrySQL,
                                          STL_TRUE,  /* RECOMPILE */
                                          STL_FALSE, /* SEARCH PLAN CACHE */
                                          & sParseTree,
                                          aEnv )
                             == STL_SUCCESS );
                
                    /**
                     * Validation
                     */
                
                    STL_TRY( qlgValidateSQL( aTransID,
                                             aDBCStmt,
                                             aSQLStmt,
                                             aQueryPhase,
                                             aSQLStmt->mRetrySQL,
                                             sParseTree,
                                             aEnv )
                             == STL_SUCCESS );

                    /**
                     * Optimize Code Area
                     */
                
                    STL_TRY( qlgOptimizeCodeSQL( aTransID,
                                                 aDBCStmt,
                                                 aSQLStmt,
                                                 aQueryPhase,
                                                 aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /**
                     * Code Plan을 확보하였음.
                     */
                    
                    STL_DASSERT( aSQLStmt->mCodePlan != NULL );
                    /* Do Nothing */
                }
                
                /**
                 * Optimize Data Area
                 */
                
                STL_TRY( qlgOptimizeDataSQL( aTransID,
                                             aDBCStmt,
                                             aSQLStmt,
                                             aQueryPhase,
                                             aEnv )
                         == STL_SUCCESS );
                
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /**
     * Recompile 이 발생함.
     */

    aSQLStmt->mIsRecompile = STL_TRUE;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    (void) qlgInit4RecompileSQL( aDBCStmt, aSQLStmt, aEnv );
    
    return STL_FAILURE;
}



/*************************************************************************
 * 정보 획득 함수 
 *************************************************************************/


/**
 * @brief Plan Cache Handle 로부터 Init Plan 을 획득하여 유효성 여부를 검사한다.
 * @param[in]  aTransID    Transaction ID
 * @param[in]  aSQLStmt    SQL Statement
 * @param[in]  aSqlHandle  Plan Cache Handle
 * @param[out] aInitPlan   Cached Init Plan
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus qlgGetValidCacheInitPlan( smlTransId      aTransID,
                                    qllStatement  * aSQLStmt,
                                    void          * aSqlHandle,
                                    void         ** aInitPlan,
                                    qllEnv        * aEnv )
{
    knlPlanIterator   sIterator;
    
    qlvInitPlan * sInitPlan = NULL;
    void        * sCodePlan = NULL;
    stlInt64      sPlanSize = 0;

    ellObjectItem * sObjectItem = NULL;
    stlBool         sResult = STL_TRUE;

    ellDictHandle * sObjectHandle = NULL;
    
    ellDictHandle   sDictHandle;
    stlBool         sObjectExist = STL_FALSE;

    ellDictHandle * sAuthHandle = NULL;

    /**
     * 기본 정보 획득
     */
    
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * 첫 번째 Init Plan 을 획득
     */

    STL_TRY( knlInitPlanIterator( & sIterator,
                                  aSqlHandle,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlPlanFetchNext( & sIterator,
                               & sCodePlan,
                               & sPlanSize,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlFiniPlanIterator( & sIterator,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );

    

    if ( sCodePlan == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        /**
         * Plan 이 사용하고 있는 객체들이 유효한 지 검사
         */

        sInitPlan = (qlvInitPlan *) QLL_GET_CACHED_INIT_PLAN( sCodePlan );
            
        if ( STL_RING_IS_EMPTY( & sInitPlan->mValidationObjList->mHeadList ) == STL_TRUE )
        {
            /* nothing to do */
        }
        else
        {
            STL_RING_FOREACH_ENTRY( & sInitPlan->mValidationObjList->mHeadList, sObjectItem )
            {
                sObjectHandle = & sObjectItem->mObjectHandle;
                
                /**
                 * Handle 이 유효한지 검사
                 */

                STL_TRY( ellIsRecentDictHandle( aTransID,
                                                sObjectHandle,
                                                & sResult,
                                                ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                if ( sResult == STL_FALSE )
                {
                    /**
                     * 유효하지 않은 Handle 임
                     */
                    
                    sCodePlan = NULL;
                    break;
                }
                
                /**
                 * 유효한 Handle 인 경우 Name 은 같으나 서로 다른 객체인지 검사
                 * 
                 * ex) u1 --> public 의 schema path 인 경우
                 * 
                 * - SELECT * FROM t1;        <----------------
                 *  : (public.t1 임)
                 * - CREATE TABLE t1 ( c1 INTEGER );
                 *  : (u1.t1 임)
                 * - SELECT * FROM t1;        <----------------
                 *  : (u1.t1 임)
                 */
                
                if ( ellIsFullNameObject( sObjectHandle ) == STL_TRUE )
                {
                    /**
                     * public.t1 과 같이 fully qualified name 이 사용됨
                     */

                    continue;
                }

                /**
                 * Unqualified Name 인 경우
                 * User 의 Schema Path 앞단에 동일한 이름의 객체가 추가되었는지 검사
                 */
                
                switch ( ellGetObjectType( sObjectHandle ) )
                {
                    case ELL_OBJECT_TABLE:
                        {
                            /**
                             * 동일한 Table 인지 검사
                             */
                                
                            STL_TRY( ellGetTableDictHandleWithAuth( aTransID,
                                                                    aSQLStmt->mViewSCN,
                                                                    sAuthHandle,
                                                                    ellGetTableName( sObjectHandle ),
                                                                    & sDictHandle,
                                                                    & sObjectExist,
                                                                    ELL_ENV(aEnv) )
                                     == STL_SUCCESS );
                            
                            if ( ( sObjectExist == STL_TRUE ) &&
                                 ( ellGetTableID( sObjectHandle ) == ellGetTableID( & sDictHandle ) ) )
                            {
                                sResult = STL_TRUE;
                            }
                            else
                            {
                                sResult = STL_FALSE;
                            }
                            break;
                        }
                    case ELL_OBJECT_CONSTRAINT:
                        {
                            /**
                             * 동일한 Table Constraint 인지 검사
                             */
                                    
                            STL_TRY( ellGetConstraintDictHandleWithAuth( aTransID,
                                                                         aSQLStmt->mViewSCN,
                                                                         sAuthHandle,
                                                                         ellGetConstraintName( sObjectHandle ),
                                                                         & sDictHandle,
                                                                         & sObjectExist,
                                                                         ELL_ENV(aEnv) )
                                     == STL_SUCCESS );
                            
                            if ( ( sObjectExist == STL_TRUE ) &&
                                 ( ellGetConstraintID( sObjectHandle ) == ellGetConstraintID( & sDictHandle ) ) )
                            {
                                sResult = STL_TRUE;
                            }
                            else
                            {
                                sResult = STL_FALSE;
                            }
                            break;
                        }
                    case ELL_OBJECT_INDEX:
                        {
                            /**
                             * 동일한 Index 인지 검사
                             */
                                    
                            STL_TRY( ellGetIndexDictHandleWithAuth( aTransID,
                                                                    aSQLStmt->mViewSCN,
                                                                    sAuthHandle,
                                                                    ellGetIndexName( sObjectHandle ),
                                                                    & sDictHandle,
                                                                    & sObjectExist,
                                                                    ELL_ENV(aEnv) )
                                     == STL_SUCCESS );
                            
                            if ( ( sObjectExist == STL_TRUE ) &&
                                 ( ellGetIndexID( sObjectHandle ) == ellGetIndexID( & sDictHandle ) ) )
                            {
                                sResult = STL_TRUE;
                            }
                            else
                            {
                                sResult = STL_FALSE;
                            }
                            break;
                        }
                    case ELL_OBJECT_SEQUENCE:
                        {
                            /**
                             * 동일한 Sequence 인지 검사
                             */
                                    
                            STL_TRY( ellGetSequenceDictHandleWithAuth( aTransID,
                                                                       aSQLStmt->mViewSCN,
                                                                       sAuthHandle,
                                                                       ellGetSequenceName( sObjectHandle ),
                                                                       & sDictHandle,
                                                                       & sObjectExist,
                                                                       ELL_ENV(aEnv) )
                                     == STL_SUCCESS );
                            
                            if ( ( sObjectExist == STL_TRUE ) &&
                                 ( ellGetSequenceID( sObjectHandle ) == ellGetSequenceID( & sDictHandle ) ) )
                            {
                                sResult = STL_TRUE;
                            }
                            else
                            {
                                sResult = STL_FALSE;
                            }
                            break;
                        }
                    default:
                        {
                            /**
                             * SQL-Schema Object 가 아님 
                             */
                            sResult = STL_TRUE;
                            break;
                        }
                } 

                if ( sResult == STL_FALSE )
                {
                    /**
                     * 유효한 객체가 아님
                     */
                    
                    sCodePlan = NULL;
                    break;
                }
                
            }
        }
    }

    /**
     * Output 설정
     */

    if( sCodePlan != NULL )
    {
        *aInitPlan = (qlvInitPlan *) QLL_GET_CACHED_INIT_PLAN( sCodePlan );
    }
    else
    {
        *aInitPlan = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Plan Cache Handle 로부터 Code Plan 을 획득하여 유효성 여부를 검사한다.
 * @param[in]  aSQLStmt    SQL Statement
 * @param[in]  aSqlHandle  Plan Cache Handle
 * @param[out] aCodePlan   Cached Code Plan
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus qlgGetValidCacheCodePlan( qllStatement  * aSQLStmt,
                                    void          * aSqlHandle,
                                    void         ** aCodePlan,
                                    qllEnv        * aEnv )
{
    void             * sCodePlan;
    stlInt64           sPlanSize = 0;
    knlPlanIterator    sIterator;
    stlBool            sIsValid = STL_FALSE;
    stlBool            sEnabledHintError;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSqlHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * 적당한 Plan을 찾는다.
     */

    STL_TRY( knlInitPlanIterator( & sIterator,
                                  aSqlHandle,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_HINT_ERROR,
                                      &sEnabledHintError,
                                      KNL_ENV(aEnv) )
             == STL_SUCCESS );

    while( 1 )
    {
        sIsValid = STL_FALSE;
                
        STL_TRY( knlPlanFetchNext( & sIterator,
                                   & sCodePlan,
                                   & sPlanSize,
                                   KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( sCodePlan == NULL )
        {
            break;
        }

        STL_TRY( qlnoValidateParamInfo( aSQLStmt->mBindCount,
                                        aSQLStmt->mBindContext,
                                        (qllOptimizationNode*) sCodePlan,
                                        & sIsValid,
                                        aEnv )
                 == STL_SUCCESS );

        if( sIsValid == STL_FALSE )
        {
            continue;
        }

        STL_TRY( qlnoValidateParamType( aSQLStmt,
                                        (qlvInitPlan *) QLL_GET_CACHED_INIT_PLAN( sCodePlan ),
                                        & sIsValid,
                                        aEnv )
                 == STL_SUCCESS );

        if( sIsValid == STL_FALSE )
        {
            continue;
        }

        if( aSQLStmt->mIsAtomic != ((qllOptimizationNode*) sCodePlan)->mIsAtomic )
        {
            continue;
        }

        if( ( sEnabledHintError == STL_FALSE ) ||
            ( ((qlvInitPlan *) QLL_GET_CACHED_INIT_PLAN( sCodePlan ))->mHasHintError == STL_FALSE ) )
        {
            break;
        }
    }

    STL_TRY( knlFiniPlanIterator( & sIterator,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
            

    
    /**
     * Code Plan을 설정한다,
     */
    
    *aCodePlan = sCodePlan;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DDL 의 유효한 Plan 을 얻는다
 * @param[in]      aTransID       Transaction ID
 * @param[in]      aDBCStmt       DBC Statement
 * @param[in,out]  aSQLStmt       SQL Statement
 * @param[in]      aQueryPhase    Query Processing Phase
 * @param[in]      aEnv           Environment
 * @remarks
 */
stlStatus qlgGetValidPlan4DDL( smlTransId       aTransID,
                               qllDBCStmt     * aDBCStmt,
                               qllStatement   * aSQLStmt,
                               qllQueryPhase    aQueryPhase,
                               qllEnv         * aEnv )
{
    stlBool  sIsValid = STL_FALSE;
    
    qlvInitPlan    * sInitPlan = NULL;
    
    /**
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aQueryPhase > QLL_PHASE_VALIDATION) && (aQueryPhase <= QLL_PHASE_EXECUTE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sInitPlan = (qlvInitPlan *) aSQLStmt->mInitPlan;
    
    /**
     * DDL 의 Init Plan 의 Object Handle 이 최신인 지 검사
     */
    
    STL_TRY( qllIsRecentAccessObject( aTransID,
                                      sInitPlan->mValidationObjList,
                                      NULL, /* Validation Table Statistic List */
                                      & sIsValid,
                                      aEnv )
             == STL_SUCCESS );

    /**
     * 최신이 아닌 경우 DDL 을 Recompile
     */
    
    if ( sIsValid == STL_TRUE )
    {
        /* nothing to do */
    }
    else
    {
        STL_TRY( qlgRecompileSQL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  STL_FALSE,  /* aSearchPlanCache */
                                  aQueryPhase,
                                  aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 2개의 Code Plan이 같은지 비교한다.
 * @param[in]     aSqlInfo       SQL Information
 * @param[in]     aCodePlanA     A Code Plan Address
 * @param[in]     aCodePlanB     B Code Plan Address
 * @remarks
 */
stlBool qlgComparePlan( knlPlanSqlInfo * aSqlInfo,
                        void           * aCodePlanA,
                        void           * aCodePlanB )
{
    qlvInitPlan          * sInitPlanA     = NULL;
    qlvInitPlan          * sInitPlanB     = NULL;
    qllOptimizationNode  * sCodePlanA     = NULL;
    qllOptimizationNode  * sCodePlanB     = NULL;
    dtlDataTypeInfo      * sDataTypeInfoA = NULL;
    dtlDataTypeInfo      * sDataTypeInfoB = NULL;
    stlInt32               sLoop          = 0;
    
    STL_DASSERT( aSqlInfo != NULL );
    STL_DASSERT( aCodePlanA != NULL );
    STL_DASSERT( aCodePlanB != NULL );


    /**
     * Code Plan 비교
     */

    sCodePlanA = (qllOptimizationNode *) aCodePlanA;
    sCodePlanB = (qllOptimizationNode *) aCodePlanB;
    
    /* Atomic & Check Hint Error flag 비교 */
    if( sCodePlanA->mIsAtomic != sCodePlanB->mIsAtomic )
    {
        return STL_FALSE;
    }
    

    /**
     * Init Plan 비교
     */

    sInitPlanA = (qlvInitPlan *) QLL_GET_CACHED_INIT_PLAN( sCodePlanA );
    sInitPlanB = (qlvInitPlan *) QLL_GET_CACHED_INIT_PLAN( sCodePlanB );

    if( sInitPlanA->mHasHintError != sInitPlanB->mHasHintError )
    {
        return STL_FALSE;
    }

    /* Bind Parameter Type 비교 */
    if( stlMemcmp( sInitPlanA->mBindIOType,
                   sInitPlanB->mBindIOType,
                   STL_SIZEOF( knlBindType ) * aSqlInfo->mBindCount ) != 0 )
    {
        return STL_FALSE;
    }


    /**
     * Bind Parameter들의 Data Type 비교
     */

    sDataTypeInfoA = sCodePlanA->mBindParamInfo->mBindDataTypeInfo;
    sDataTypeInfoB = sCodePlanB->mBindParamInfo->mBindDataTypeInfo;

    for( sLoop = 0 ; sLoop < aSqlInfo->mBindCount ; sLoop++ )
    {
        if( ( sDataTypeInfoA[ sLoop ].mDataType !=
              sDataTypeInfoB[ sLoop ].mDataType ) ||
            ( sDataTypeInfoA[ sLoop ].mArgNum1 !=
              sDataTypeInfoB[ sLoop ].mArgNum1 ) ) 
        {
            return STL_FALSE;
        }

        switch( sDataTypeInfoA[ sLoop ].mDataType )
        {
            case DTL_TYPE_FLOAT :
            case DTL_TYPE_NUMBER :
            case DTL_TYPE_DECIMAL :
                {
                    if( sDataTypeInfoA[ sLoop ].mArgNum2 !=
                        sDataTypeInfoB[ sLoop ].mArgNum2 )
                    {
                        return STL_FALSE;
                    }
                    
                    break;
                }
            case DTL_TYPE_CHAR :
            case DTL_TYPE_VARCHAR :
                {
                    if( sDataTypeInfoA[ sLoop ].mArgNum3 !=
                        sDataTypeInfoB[ sLoop ].mArgNum3 )
                    {
                        return STL_FALSE;
                    }
                    
                    break;
                }
            case DTL_TYPE_INTERVAL_YEAR_TO_MONTH :
            case DTL_TYPE_INTERVAL_DAY_TO_SECOND :
                {
                    if( ( sDataTypeInfoA[ sLoop ].mArgNum2 !=
                          sDataTypeInfoB[ sLoop ].mArgNum2 ) ||
                        ( sDataTypeInfoA[ sLoop ].mArgNum3 !=
                          sDataTypeInfoB[ sLoop ].mArgNum3 ) )
                    {
                        return STL_FALSE;
                    }
                    
                    break;
                }
            default :
                {
                    break;
                }
        }
    }
    
    return STL_TRUE;
}


/**
 * @brief Bind Parameter의 Data Type에 대한 validation을 수행한다.
 * @param[in]     aBindCount       Bind Parameter Count
 * @param[in]     aBindContext     Bind Context
 * @param[in]     aEnv             Environment
 * @remarks
 */
stlStatus qlgValidateBindParamDataType( stlInt32          aBindCount,
                                        knlBindContext  * aBindContext,
                                        qllEnv          * aEnv )
{
    knlValueBlockList  * sValueBlock = NULL;
    dtlDataValue       * sDataValue  = NULL;
    stlInt32             sLoop       = 0;
    stlInt32             sIdx        = 0;
    stlChar              sTmpMsgBuf[ STL_MAX_ERROR_MESSAGE ];
    

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aBindCount > 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBindContext != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Data Type Validation
     */
    
    for( sLoop = 0 ; sLoop < aBindCount ; sLoop++ )
    {
        if( aBindContext->mMap[ sLoop ]->mBindType == KNL_BIND_TYPE_INOUT )
        {
            /**
             * IN value block
             */
            
            sValueBlock = aBindContext->mMap[ sLoop ]->mINValueBlock;

            STL_DASSERT( sValueBlock->mValueBlock != NULL );
            STL_DASSERT( sValueBlock->mValueBlock->mDataValue != NULL );

            /* Value Block의 Data Type 체크 */
            STL_TRY_THROW( dtlVaildateDataTypeInfo(
                               KNL_GET_BLOCK_DB_TYPE( sValueBlock ),
                               KNL_GET_BLOCK_ARG_NUM1( sValueBlock ),
                               KNL_GET_BLOCK_ARG_NUM2( sValueBlock ),
                               KNL_GET_BLOCK_STRING_LENGTH_UNIT( sValueBlock ),
                               KNL_GET_BLOCK_INTERVAL_INDICATOR( sValueBlock ),
                               QLL_ERROR_STACK( aEnv ) )
                           == STL_SUCCESS,
                           RAMP_ERR_INVALID_DATA_TYPE_INFO );

            /* Data Value의 Data Type 체크 */
            if( KNL_GET_BLOCK_IS_SEP_BUFF( sValueBlock ) == STL_TRUE )
            {
                sDataValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock );
            
                for( sIdx = KNL_GET_BLOCK_ALLOC_CNT( sValueBlock ) - 1 ; sIdx > 0 ; sIdx-- )
                {
                    STL_TRY_THROW( sDataValue[ 0 ].mType == sDataValue[ sIdx ].mType,
                                   RAMP_ERR_MISMATCH_DATA_TYPE );
                }
            }
            else
            {
                /* Do Nothing */
            }

            
            /**
             * OUT value block
             */
            
            sValueBlock = aBindContext->mMap[ sLoop ]->mOUTValueBlock;
                        
            STL_DASSERT( sValueBlock->mValueBlock != NULL );
            STL_DASSERT( sValueBlock->mValueBlock->mDataValue != NULL );

            /* Value Block의 Data Type 체크 */
            STL_TRY_THROW( dtlVaildateDataTypeInfo(
                               KNL_GET_BLOCK_DB_TYPE( sValueBlock ),
                               KNL_GET_BLOCK_ARG_NUM1( sValueBlock ),
                               KNL_GET_BLOCK_ARG_NUM2( sValueBlock ),
                               KNL_GET_BLOCK_STRING_LENGTH_UNIT( sValueBlock ),
                               KNL_GET_BLOCK_INTERVAL_INDICATOR( sValueBlock ),
                               QLL_ERROR_STACK( aEnv ) )
                           == STL_SUCCESS,
                           RAMP_ERR_INVALID_DATA_TYPE_INFO );

            /* Data Value의 Data Type 체크 */
            if( KNL_GET_BLOCK_IS_SEP_BUFF( sValueBlock ) == STL_TRUE )
            {
                sDataValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock );
            
                for( sIdx = KNL_GET_BLOCK_ALLOC_CNT( sValueBlock ) - 1 ; sIdx > 0 ; sIdx-- )
                {
                    STL_TRY_THROW( sDataValue[ 0 ].mType == sDataValue[ sIdx ].mType,
                                   RAMP_ERR_MISMATCH_DATA_TYPE );
                }
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            if( aBindContext->mMap[ sLoop ]->mBindType == KNL_BIND_TYPE_IN )
            {
                sValueBlock = aBindContext->mMap[ sLoop ]->mINValueBlock;
            }
            else
            {
                STL_DASSERT( aBindContext->mMap[ sLoop ]->mBindType == KNL_BIND_TYPE_OUT );
                
                sValueBlock = aBindContext->mMap[ sLoop ]->mOUTValueBlock;
            }

            STL_DASSERT( sValueBlock->mValueBlock != NULL );
            STL_DASSERT( sValueBlock->mValueBlock->mDataValue != NULL );

            /* Value Block의 Data Type 체크 */
            STL_TRY_THROW( dtlVaildateDataTypeInfo(
                               KNL_GET_BLOCK_DB_TYPE( sValueBlock ),
                               KNL_GET_BLOCK_ARG_NUM1( sValueBlock ),
                               KNL_GET_BLOCK_ARG_NUM2( sValueBlock ),
                               KNL_GET_BLOCK_STRING_LENGTH_UNIT( sValueBlock ),
                               KNL_GET_BLOCK_INTERVAL_INDICATOR( sValueBlock ),
                               QLL_ERROR_STACK( aEnv ) )
                           == STL_SUCCESS,
                           RAMP_ERR_INVALID_DATA_TYPE_INFO );

            /* Data Value의 Data Type 체크 */
            if( KNL_GET_BLOCK_IS_SEP_BUFF( sValueBlock ) == STL_TRUE )
            {
                sDataValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock );
            
                for( sIdx = KNL_GET_BLOCK_ALLOC_CNT( sValueBlock ) - 1 ; sIdx > 0 ; sIdx-- )
                {
                    STL_TRY_THROW( sDataValue[ 0 ].mType == sDataValue[ sIdx ].mType,
                                   RAMP_ERR_MISMATCH_DATA_TYPE );
                }
            }
            else
            {
                /* Do Nothing */
            }
        }
    }

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_DATA_TYPE_INFO )
    {
        sTmpMsgBuf[ 0 ] = '\0';
        
        stlSnprintf( sTmpMsgBuf,
                     STL_MAX_ERROR_MESSAGE,
                     "parameter number (%d)",
                     sLoop );

        stlSetLastDetailErrorMessage( QLL_ERROR_STACK( aEnv ),
                                      sTmpMsgBuf );
    }

    STL_CATCH( RAMP_ERR_MISMATCH_DATA_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BIND_PARAM_INCONSISTENT_DATATYPES,
                      NULL,
                      QLL_ERROR_STACK( aEnv ),
                      sLoop );
    }


    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Bind Parameter의 Data Value에 대한 validation을 수행한다.
 * @param[in]     aBindCount       Bind Parameter Count
 * @param[in]     aBindContext     Bind Context
 * @param[in]     aEnv             Environment
 * @remarks
 */
stlStatus qlgValidateBindParamDataValue( stlInt32          aBindCount,
                                         knlBindContext  * aBindContext,
                                         qllEnv          * aEnv )
{
    knlValueBlockList  * sValueBlock = NULL;
    dtlDataValue       * sDataValue  = NULL;
    stlInt32             sAllocCnt   = 0;
    stlInt32             sLoop       = 0;
    stlInt32             sIdx        = 0;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aBindCount > 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBindContext != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Data Type Validation
     */
    
    STL_TRY( qlgValidateBindParamDataType( aBindCount,
                                           aBindContext,
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Data Value Validation
     */
    
    for( sLoop = 0 ; sLoop < aBindCount ; sLoop++ )
    {
        if( ( aBindContext->mMap[ sLoop ]->mBindType == KNL_BIND_TYPE_IN ) ||
            ( aBindContext->mMap[ sLoop ]->mBindType == KNL_BIND_TYPE_INOUT ) )
        {
            /**
             * IN value block
             */
            
            sValueBlock = aBindContext->mMap[ sLoop ]->mINValueBlock;

            STL_DASSERT( sValueBlock->mValueBlock != NULL );
            STL_DASSERT( sValueBlock->mValueBlock->mDataValue != NULL );

            sDataValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock );
    
            STL_DASSERT( sDataValue != NULL );
            STL_DASSERT( KNL_GET_BLOCK_ALLOC_CNT( sValueBlock ) > 0 );
            
            if( KNL_GET_BLOCK_IS_SEP_BUFF( sValueBlock ) == STL_FALSE )
            {
                sAllocCnt = 1;
            }
            else
            {
                sAllocCnt = KNL_GET_BLOCK_ALLOC_CNT( sValueBlock );
            }
            
            for( sIdx = 0 ; sIdx < sAllocCnt ; sIdx++ )
            {
                STL_TRY_THROW( sDataValue[ sIdx ].mBufferSize >= sDataValue[ sIdx ].mLength,
                               RAMP_ERR_INVALID_DATA_VALUE );
            }
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    
    return STL_SUCCESS;
                       
    STL_CATCH( RAMP_ERR_INVALID_DATA_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BIND_PARAM_INVALID_VALUE,
                      NULL,
                      QLL_ERROR_STACK( aEnv ),
                      sLoop );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlgCallPhase */
