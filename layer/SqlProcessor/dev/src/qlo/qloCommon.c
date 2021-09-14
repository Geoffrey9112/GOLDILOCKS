/*******************************************************************************
 * qloCommon.c
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
 * @file qloCommon.c
 * @brief SQL Processor Layer Common Functions
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlocCommon
 * @{
 */


/**
 * @brief Code Plan에 존재하는 Valid Table Statistic List를 반환한다.
 * @param[in]   aSQLStmt    SQL Statement
 */
qloValidTblStatList * qloGetValidTblStatList( qllStatement  * aSQLStmt )
{
    qloValidTblStatList * sValidTblStatList = NULL;


    STL_DASSERT( aSQLStmt != NULL );


    switch( aSQLStmt->mStmtType )
    {
        case QLL_STMT_SELECT_TYPE :
        case QLL_STMT_SELECT_FOR_UPDATE_TYPE :
        case QLL_STMT_SELECT_INTO_TYPE :
        case QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE :
        case QLL_STMT_DELETE_TYPE :
        case QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE :
        case QLL_STMT_DELETE_RETURNING_QUERY_TYPE :
        case QLL_STMT_DELETE_RETURNING_INTO_TYPE :
        case QLL_STMT_UPDATE_TYPE :
        case QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE :
        case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE :
        case QLL_STMT_UPDATE_RETURNING_INTO_TYPE :
        case QLL_STMT_INSERT_TYPE :
        case QLL_STMT_INSERT_SELECT_TYPE :
        case QLL_STMT_INSERT_RETURNING_INTO_TYPE :
        case QLL_STMT_INSERT_RETURNING_QUERY_TYPE :
            {
                if( aSQLStmt->mCodePlan != NULL )
                {
                    sValidTblStatList =
                        ((qlnoDmlStmt *)(((qllOptimizationNode*)(aSQLStmt->mCodePlan))->mOptNode))->mValidTblStatList;
                }
                else
                {
                    sValidTblStatList = NULL;
                }

                break;
            }
        default:
            {
                sValidTblStatList = NULL;
                break;
            }
    }


    return sValidTblStatList;
}


/** @} qlocCommon */
