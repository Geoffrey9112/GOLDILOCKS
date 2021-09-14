/*******************************************************************************
 * qlssSetTimeZone.c
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
 * @file qlssSetTimeZone.c
 * @brief SET TIME ZONE 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>
#include <qlgStartup.h>

/**
 * @defgroup qlssSessionSetTimeZone SET TIME ZONE
 * @ingroup qlssSession
 * @{
 */


/**
 * @brief SET TIME ZONE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateSetTimeZone( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv )
{
    qlssInitSetTimeZone * sInitPlan   = NULL;
    qlpSetTimeZone      * sParseTree  = NULL;

    stlChar                   * sTimeZoneString = NULL;
    dtlDataValue                sTimeZoneValue;
    dtlIntervalDayToSecondType  sTimeZoneInterval;
    stlBool                     sTimeZoneWithInfo = STL_FALSE;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_SET_TIMEZONE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_SET_TIMEZONE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpSetTimeZone *) aParseTree;


    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSetTimeZone),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSetTimeZone) );


    /**
     * Init Plan 구성
     */

    if( sParseTree->mValue != NULL )
    {
        sTimeZoneString = QLP_GET_PTR_VALUE( sParseTree->mValue );
        
        /**
         * TimeZone Interval String 을 dtlInterval 로 변경
         */
        
        sTimeZoneValue.mValue = & sTimeZoneInterval;
        
        STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                                   STL_SIZEOF(dtlIntervalDayToSecondType),
                                   & sTimeZoneValue,
                                   QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY( dtlIntervalDayToSecondSetValueFromString(
                     sTimeZoneString,
                     stlStrlen( sTimeZoneString ),
                     DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,
                     DTL_SCALE_NA,
                     DTL_STRING_LENGTH_UNIT_NA,
                     DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE,
                     DTL_INTERVAL_DAY_TO_SECOND_SIZE,
                     & sTimeZoneValue,
                     & sTimeZoneWithInfo,
                     KNL_DT_VECTOR(aEnv),
                     aEnv,
                     KNL_DT_VECTOR(aEnv),
                     aEnv,
                     QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
        
        /**
         * dtlInterval 을 Timezone Offset 으로 변경 
         */
        
        STL_TRY( dtlTimeZoneToGMTOffset( & sTimeZoneInterval,
                                         & sInitPlan->mGMTOffset,
                                         QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /*
         * SET TIME ZONE LOCAL
         */

        sInitPlan->mGMTOffset = qlgGetOriginalGMTOffsetFunc( aEnv );
    }

    /**
     * 권한 검사 없음
     */
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief SET TIME ZONE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeSetTimeZone( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv )
{
    /**
     * nothing to do
     */
    
    return STL_SUCCESS;
}

/**
 * @brief SET TIME ZONE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataSetTimeZone( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv )
{
    /**
     * nothing to do
     */
    
    return STL_SUCCESS;
}


/**
 * @brief SET TIME ZONE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteSetTimeZone( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv )
{
    qlssInitSetTimeZone * sInitPlan = NULL;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Init Plan 획득 
     */

    sInitPlan = (qlssInitSetTimeZone *) aSQLStmt->mInitPlan;

    /**
     * Session 의 Time Zone Offset 설정
     */

    STL_TRY( qllSetSessTimeZoneOffset( sInitPlan->mGMTOffset, aEnv )
             == STL_SUCCESS );

    /**
     * Context 정보 설정
     */
    
    aSQLStmt->mDCLContext.mType      = QLL_DCL_TYPE_SET_TIME_ZONE;
    aSQLStmt->mDCLContext.mGMTOffset = (stlInt16)sInitPlan->mGMTOffset;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlssSessionSetTimeZone */
