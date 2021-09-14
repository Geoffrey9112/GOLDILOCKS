/*******************************************************************************
 * qllTarget.c
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
 * @file qllTarget.c
 * @brief Target Basic Information for ODBC, JDBC
 */

#include <qll.h>
#include <qlDef.h>

#include <qlfTarget.h>

/**
 * @addtogroup qllTargetInfo
 * @{
 */


/**
 * @brief SQL 구문에 있는 Target 의 개수를 구한다.
 * @param[in] aSQLStmt      SQL Statement
 * @return
 * - Target 개수
 * - Invalid 한 구문일 경우 0
 * @remarks
 * - Prepare 시에도 불릴 수 있다.
 */
stlInt32 qllGetTargetCount( qllStatement * aSQLStmt )
{
    /* Query Node in SELECT --> Query Spec  */
    /*                      |               */
    /*                      --> Query Set   */
    
    qlvInitNode          * sIniNode        = NULL;
    qlvInitSelect        * sInitPlanSelect = NULL;

    stlInt32               sCount          = 0;


    /**
     * Fetch 가능 여부에 따라 Target Count 설정
     */

    if( aSQLStmt == NULL )
    {
        sCount = 0;
        STL_THROW( RAMP_FINISH );
    }

    if ( ( aSQLStmt->mStmtAttr & QLL_STMT_ATTR_FETCH_MASK )
         != QLL_STMT_ATTR_FETCH_YES )
    {
        sCount = 0;
        STL_THROW( RAMP_FINISH );
    }


    /**
     * Init Plan으로부터 Target Count 획득
     */
    
    sIniNode = (qlvInitNode *)( qllGetInitPlan( aSQLStmt ) );

    if( sIniNode == NULL )
    {
        sCount = 0;
        STL_THROW( RAMP_FINISH );
    }
    

    /**
     * SQL 구분별 Target Count 설정
     */
    
    switch( aSQLStmt->mStmtType )
    {
        case QLL_STMT_SELECT_TYPE:
        case QLL_STMT_SELECT_FOR_UPDATE_TYPE:
            {
                sInitPlanSelect = (qlvInitSelect *) sIniNode;

                if( sInitPlanSelect->mQueryNode->mType == QLV_NODE_TYPE_QUERY_SPEC )
                {
                    sCount = ( (qlvInitQuerySpecNode*) sInitPlanSelect->mQueryNode )->mTargetCount;
                }
                else
                {
                    STL_DASSERT( sInitPlanSelect->mQueryNode->mType == QLV_NODE_TYPE_QUERY_SET );
                    sCount = ( (qlvInitQuerySetNode*) sInitPlanSelect->mQueryNode )->mSetTargetCount;
                }
                
                break;
            }
        case QLL_STMT_INSERT_RETURNING_QUERY_TYPE:
            {
                if( ((qlvInitInsertReturnInto *) sIniNode)->mIsInsertValues == STL_TRUE )
                {
                    sCount = ((qlvInitInsertReturnInto *) sIniNode)->mInitInsertValues->mReturnTargetCnt;
                }
                else
                {
                    sCount = ((qlvInitInsertReturnInto *) sIniNode)->mInitInsertSelect->mReturnTargetCnt;
                }

                break;
            }
        case QLL_STMT_DELETE_RETURNING_QUERY_TYPE:
            {
                sCount = ((qlvInitDelete *) sIniNode)->mReturnTargetCnt;
                break;
            }
        case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE:
            {
                sCount = ((qlvInitUpdate *) sIniNode)->mReturnTargetCnt;
                break;
            }
        default:
            {
                sCount = 0;
                break;
            }
    }

    STL_RAMP( RAMP_FINISH );

    return sCount;
}


/**
 * @brief ODBC, JDBC 등에서 활용할 수 있는 Target 의 기초 정보를 획득한다.
 * @param[in]  aSQLStmt     SQL Statement
 * @param[in]  aRegionMem   Target Information 을 위한 Memory Manager
 * @param[out] aTargetInfo  Target Information List
 * @param[in]  aEnv         Environment
 * @remarks
 * 호출 시점에 따라 정보가 달라질 수 있다.
 * - Prepare 이후: Type 을 결정할 수 없는 경우가 있다.
 * - Execute 이후: 모든 Type 이 결정되었다.
 * - 호출 시점에 구분없이 정보를 얻을 수 있도록 dictionary handle 을 사용하지 않아야 함.
 */
stlStatus qllGetTargetInfo( qllStatement  * aSQLStmt,
                            knlRegionMem  * aRegionMem,
                            qllTarget    ** aTargetInfo,
                            qllEnv        * aEnv )
{
    /* Prepare 시에도 불릴 수 있음 : Validation 이후 호출 가능 */

    qllOptimizationNode  * sOptNode       = NULL;
    qlvInitNode          * sIniNode       = NULL;
    qllTarget            * sTargetList    = NULL;
    qllTarget            * sOrgTargetList = NULL;

    
    /**
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetInfo != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * SQL 구분별 Target Information 얻기
     */
    
    if( aSQLStmt->mCodePlan != NULL )
    {
        /* Code Optimization 단계 이후에 Target 정보를 얻어 가는 경우 */
        sOptNode = aSQLStmt->mCodePlan;
                
        switch( aSQLStmt->mStmtType )
        {
            case QLL_STMT_SELECT_TYPE:
            case QLL_STMT_SELECT_FOR_UPDATE_TYPE:
                {
                    sOrgTargetList = ((qlnoSelectStmt *) sOptNode->mOptNode)->mTargetInfo;
                    break;
                }
            case QLL_STMT_INSERT_RETURNING_QUERY_TYPE:
                {
                    sOrgTargetList = ((qlnoInsertStmt *) sOptNode->mOptNode)->mTargetInfo;
                    break;
                }
            case QLL_STMT_DELETE_RETURNING_QUERY_TYPE:
                {
                    sOrgTargetList = ((qlnoDeleteStmt *) sOptNode->mOptNode)->mTargetInfo;
                    break;
                }
            case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE:
                {
                    sOrgTargetList = ((qlnoUpdateStmt *) sOptNode->mOptNode)->mTargetInfo;
                    break;
                }
            default:
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
    }
    else
    {
        /* Code Optimization 단계 이전에 Target 정보를 얻어 가는 경우 */
        sIniNode = (qlvInitNode *)( qllGetInitPlan( aSQLStmt ) );
                
        STL_DASSERT( sIniNode != NULL );
    
        switch( aSQLStmt->mStmtType )
        {
            case QLL_STMT_SELECT_TYPE:
            case QLL_STMT_SELECT_FOR_UPDATE_TYPE:
                {
                    sOrgTargetList = ((qlvInitSelect *) sIniNode)->mTargetInfo;
                    break;
                }
            case QLL_STMT_INSERT_RETURNING_QUERY_TYPE:
                {
                    sOrgTargetList = ((qlvInitInsertReturnInto *) sIniNode)->mTargetInfo;
                    break;
                }
            case QLL_STMT_DELETE_RETURNING_QUERY_TYPE:
                {
                    sOrgTargetList = ((qlvInitDelete *) sIniNode)->mTargetInfo;
                    break;
                }
            case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE:
                {
                    sOrgTargetList = ((qlvInitUpdate *) sIniNode)->mTargetInfo;
                    break;
                }
            default:
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
    }

    
    /**
     * SQL 구분별 Target Information 복사
     */

    STL_TRY( qlfCopyTarget( sOrgTargetList,
                            & sTargetList,
                            aRegionMem,
                            aEnv )
             == STL_SUCCESS );
    
            
    /**
     * Output 설정
     */

    *aTargetInfo = sTargetList;
    

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} qllTargetInfo */
