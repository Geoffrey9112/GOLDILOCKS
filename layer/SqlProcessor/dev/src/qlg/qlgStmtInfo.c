/*******************************************************************************
 * glgStmtInfo.c
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
 * @file qlgStmtInfo.c
 * @brief SQL Processor Layer StmtInfo Internal Routines
 */


#include <qll.h>
#include <qlDef.h>
#include <qlgStmtInfo.h>

/**
 * @defgroup qlgStmtInfo SQL Statement 에 대한 정보 획득 함수 
 * @ingroup qlInternal
 * @{
 */


/**
 * @brief SQL Stmt Type 의 상대값 ID (0부터 시작)를 얻는다.
 * @param[in] aType  SQL Statement Type
 * @return
 * SQL Stmt 의 상대값 ID
 */
stlInt32 qlgGetStmtTypeRelativeID( qllNodeType aType )
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

    return sIdx;
}


/**
 * @brief SQL Stmt Related ID 로부터 Stmt Type 의 Keyword String 을 얻는다.
 * @param[in] aRelStmtID  
 * @return
 * SQL Stmt 의 Keyword String
 */
stlChar * qlgGetStmtTypeKeyString( stlInt32 aRelStmtID )
{
    return gSQLStmtTable[aRelStmtID].mStmtTypeKeyword;
}


/**
 * @brief SQL Statement 의 속성 값을 얻는다.
 * @param[in] aType  SQL Statement Type
 * @return
 * SQL Statement의 속성 값 ellStatementAttr
 */
stlInt32 qlgGetStatementAttr( qllNodeType aType )
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

    return gSQLStmtTable[sIdx].mStmtAttr;
}


/**
 * @brief SQL Statement 의 Cursor Property 와 동일한 Statement Property 인지 비교
 * @param[in]  aSQLStmt       SQL Statement
 * @param[in]  aStmtProperty  비교할 Statement Cursor Property
 */
stlBool  qlgIsSameStmtCursorProperty( qllStatement           * aSQLStmt,
                                      qllStmtCursorProperty  * aStmtProperty )
{
    qllStmtCursorProperty * sCurr = & aSQLStmt->mStmtCursorProperty;
    
    if ( (sCurr->mIsDbcCursor             == aStmtProperty->mIsDbcCursor)             &&
         (sCurr->mProperty.mStandardType  == aStmtProperty->mProperty.mStandardType)  &&
         (sCurr->mProperty.mSensitivity   == aStmtProperty->mProperty.mSensitivity)   &&
         (sCurr->mProperty.mScrollability == aStmtProperty->mProperty.mScrollability) &&
         (sCurr->mProperty.mHoldability   == aStmtProperty->mProperty.mHoldability)   &&
         (sCurr->mProperty.mUpdatability  == aStmtProperty->mProperty.mUpdatability)  &&
         (sCurr->mProperty.mReturnability == aStmtProperty->mProperty.mReturnability) )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}

/**
 * @brief SQL Statement 의 Init Plan 에 UnCommitted Object 가 존재하는지 검사한다.
 * @param[in]  aSQLStmt    SQL Statement
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlBool qlgHasUncommittedObject( qllStatement * aSQLStmt,
                                 qllEnv       * aEnv )
{
    ellObjectList * sAccessObjectList = NULL;
    
    ellObjectItem * sObjectItem = NULL;

    stlBool sResult = STL_FALSE;
    

    /**
     * Parameter Validation
     */

    STL_DASSERT( aSQLStmt != NULL );
    STL_DASSERT( aSQLStmt->mInitPlan != NULL );

    /**
     * Access Object List 획득
     */

    sAccessObjectList = ((qlvInitPlan*) aSQLStmt->mInitPlan)->mValidationObjList;

    /**
     * Object List 목록에 Un-commit Object 가 존재하는지 검사
     */
    
    sResult = STL_FALSE;

    if ( STL_RING_IS_EMPTY( & sAccessObjectList->mHeadList ) == STL_TRUE )
    {
        sResult = STL_FALSE;
    }
    else
    {
        STL_RING_FOREACH_ENTRY( & sAccessObjectList->mHeadList, sObjectItem )
        {
            if ( sObjectItem->mObjectHandle.mLocalModifySeq == 0 )
            {
                continue;
            }
            else
            {
                /**
                 * Uncommited Handle 임
                 */
                sResult = STL_TRUE;
                break;
            }
        }
    }

    return sResult;
}

/** @} qlgStmtInfo */
