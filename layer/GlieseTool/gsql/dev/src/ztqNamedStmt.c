/*******************************************************************************
 * ztqNamedStmt.c
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
 * @file ztqNamedStmt.c
 * @brief Named Statement Routines
 */

#include <stl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztqDef.h>
#include <ztqOdbcBridge.h>
#include <ztqNamedStmt.h>

extern SQLHDBC         gZtqDbcHandle;

/**
 * @addtogroup ztqNamedStmt
 * @{
 */

ztqNamedStmt * gZtqNamedStmt = NULL;


/**
 * @brief Named Statement 자료구조를 초기화
 * @param[in]  aNamedStmt  Named Statement
 * @remarks
 */
void ztqInitNamedStmt( ztqNamedStmt * aNamedStmt )
{
    aNamedStmt->mStmtName[0] = '\0';
    aNamedStmt->mIsPrepared  = STL_FALSE;
    
    aNamedStmt->mZtqSqlStmt.mCommandType   = ZTQ_COMMAND_TYPE_MAX;
    aNamedStmt->mZtqSqlStmt.mStatementType = ZTQ_STMT_MAX;
    aNamedStmt->mZtqSqlStmt.mExplainPlan   = STL_FALSE;
    aNamedStmt->mZtqSqlStmt.mObjectType    = ZTQ_OBJECT_MAX;
    aNamedStmt->mZtqSqlStmt.mObjectName[0] = '\0';
    
    stlMemset( & aNamedStmt->mZtqSqlStmt.mOdbcStmt, 0x00, STL_SIZEOF(SQLHSTMT) );
    aNamedStmt->mZtqSqlStmt.mHostVarList = NULL;
    
    aNamedStmt->mNext = NULL;
}


/**
 * @brief Named Statement 를 DROP (SQL_DROP 에 해당)
 * @param[in]   aNamedStmt  Named Statement
 * @param[in]   aErrorStack Error Stack 
 * @remarks
 */
stlStatus ztqDropNamedStmt( ztqNamedStmt  * aNamedStmt,
                            stlErrorStack * aErrorStack )
{
    stlInt32  sState = 0;

    ztqHostVariable * sHost = NULL;
    ztqHostVariable * sNextHost = NULL;

    /*
     * Member 초기화
     */
    
    aNamedStmt->mStmtName[0] = '\0';
    aNamedStmt->mIsPrepared  = STL_FALSE;
    
    aNamedStmt->mZtqSqlStmt.mCommandType   = ZTQ_COMMAND_TYPE_MAX;
    aNamedStmt->mZtqSqlStmt.mStatementType = ZTQ_STMT_MAX;
    aNamedStmt->mZtqSqlStmt.mExplainPlan   = STL_FALSE;
    aNamedStmt->mZtqSqlStmt.mObjectType    = ZTQ_OBJECT_MAX;
    aNamedStmt->mZtqSqlStmt.mObjectName[0] = '\0';

    /*
     * ODBC Statement 제거 
     */

    sState = 1;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               aNamedStmt->mZtqSqlStmt.mOdbcStmt,
                               aErrorStack )
             == STL_SUCCESS );
    stlMemset( & aNamedStmt->mZtqSqlStmt.mOdbcStmt, 0x00, STL_SIZEOF(SQLHSTMT) );

    /*
     * Host Variable 공간 제거 
     */

    sState = 2;

    sHost = aNamedStmt->mZtqSqlStmt.mHostVarList;

    while( sHost != NULL)
    {
        sNextHost = (ztqHostVariable *) sHost->mNext;
        stlFreeHeap( sHost );
        sHost = sNextHost;
    }

    aNamedStmt->mZtqSqlStmt.mHostVarList = NULL;

    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 0:
            (void) ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                     aNamedStmt->mZtqSqlStmt.mOdbcStmt,
                                     aErrorStack );
        case 1:
            {
                sHost = aNamedStmt->mZtqSqlStmt.mHostVarList;
                
                while( sHost != NULL)
                {
                    sNextHost = (ztqHostVariable *) sHost->mNext;
                    stlFreeHeap( sHost );
                    sHost = sNextHost;
                }
                
                aNamedStmt->mZtqSqlStmt.mHostVarList = NULL;
            }

        case 2:
        default:
            break;
    }
    
    return STL_FAILURE;
}



/**
 * @brief Named Statement 를 RESET (SQL_UNBIND 과 SQL_RESET_PARAMS 에 해당)
 * @param[out]  aNamedStmt  Named Statement
 * @param[in]   aErrorStack Error Stack 
 * @remarks
 */
stlStatus ztqResetNamedStmt( ztqNamedStmt  * aNamedStmt,
                             stlErrorStack * aErrorStack )
{
    stlInt32  sState = 0;

    ztqHostVariable * sHost = NULL;
    ztqHostVariable * sNextHost = NULL;

    /*
     * Member 초기화
     */
    
    aNamedStmt->mIsPrepared  = STL_FALSE;
    
    aNamedStmt->mZtqSqlStmt.mCommandType   = ZTQ_COMMAND_TYPE_MAX;
    aNamedStmt->mZtqSqlStmt.mStatementType = ZTQ_STMT_MAX;
    aNamedStmt->mZtqSqlStmt.mExplainPlan   = STL_FALSE;
    aNamedStmt->mZtqSqlStmt.mObjectType    = ZTQ_OBJECT_MAX;
    aNamedStmt->mZtqSqlStmt.mObjectName[0] = '\0';

    /*
     * ODBC Statement 의 SQL_UNBIND 
     */

    sState = 1;
    STL_TRY( ztqSQLFreeStmt( aNamedStmt->mZtqSqlStmt.mOdbcStmt,
                             SQL_UNBIND,
                             aErrorStack )
             == STL_SUCCESS );

    /*
     * ODBC Statement 의 SQL_RESET_PARAMS
     */

    sState = 2;
    STL_TRY( ztqSQLFreeStmt( aNamedStmt->mZtqSqlStmt.mOdbcStmt,
                             SQL_RESET_PARAMS,
                             aErrorStack )
             == STL_SUCCESS );
    
    /*
     * Host Variable 공간 제거 
     */

    sState = 3;

    sHost = aNamedStmt->mZtqSqlStmt.mHostVarList;

    while( sHost != NULL)
    {
        sNextHost = (ztqHostVariable *) sHost->mNext;
        stlFreeHeap( sHost );
        sHost = sNextHost;
    }

    aNamedStmt->mZtqSqlStmt.mHostVarList = NULL;

    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 0:
            (void) ztqSQLFreeStmt( aNamedStmt->mZtqSqlStmt.mOdbcStmt,
                                   SQL_UNBIND,
                                   aErrorStack );
        case 1:
            (void) ztqSQLFreeStmt( aNamedStmt->mZtqSqlStmt.mOdbcStmt,
                                   SQL_RESET_PARAMS,
                                   aErrorStack );
        case 2:
            {
                sHost = aNamedStmt->mZtqSqlStmt.mHostVarList;
                
                while( sHost != NULL)
                {
                    sNextHost = (ztqHostVariable *) sHost->mNext;
                    stlFreeHeap( sHost );
                    sHost = sNextHost;
                }
                
                aNamedStmt->mZtqSqlStmt.mHostVarList = NULL;
            }
        case 3:
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Named Statment 를 할당 
 * @param[in]  aStmtName   Statement Name
 * @param[in]  aErrorStack Error Stack 
 * @remarks
 */
stlStatus ztqAllocNamedStmt( stlChar       * aStmtName,
                             stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;
    
    ztqNamedStmt * sStmt = NULL;

    /*
     * 이미 존재하는 지 검사
     */
    
    STL_TRY( ztqFindNamedStmt( aStmtName, & sStmt, aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sStmt == NULL, RAMP_ERR_STMT_EXIST );

    /*
     * Named Statment 공간 할당
     */

    STL_TRY( stlAllocHeap( (void **) & sStmt,
                           STL_SIZEOF(ztqNamedStmt),
                           aErrorStack )
             == STL_SUCCESS );
    sState = 1;
    ztqInitNamedStmt( sStmt );

    stlToupperString( aStmtName, sStmt->mStmtName );
    
    /*
     * ODBC Statement 할당
     */

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                & sStmt->mZtqSqlStmt.mOdbcStmt,
                                aErrorStack )
             == STL_SUCCESS );
    sState = 2;

    /*
     * Named Statement list 에 추가
     */

    if ( gZtqNamedStmt == NULL )
    {
        gZtqNamedStmt = sStmt;
    }
    else
    {
        sStmt->mNext  = gZtqNamedStmt;
        gZtqNamedStmt = sStmt;
    }
       
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_STMT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NAMED_STMT_DUPLICATE,
                      NULL,
                      aErrorStack,
                      aStmtName );
    }
    
    STL_FINISH;

    switch (sState)
    {
        case 2:
            (void) ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                     sStmt->mZtqSqlStmt.mOdbcStmt,
                                     aErrorStack );
        case 1:
            stlFreeHeap( sStmt );
        case 0:
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Named Statment 를 제거 
 * @param[in]  aStmtName   Statement Name
 * @param[in]  aErrorStack Error Stack 
 * @remarks
 */
stlStatus ztqFreeNamedStmt( stlChar       * aStmtName,
                            stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;
    
    ztqNamedStmt * sStmt = NULL;
    ztqNamedStmt * sPrevStmt = NULL;

    /*
     * Named Statement list 에서 검색 
     */

    sPrevStmt = NULL;
    sStmt = gZtqNamedStmt;

    while ( sStmt != NULL )
    {
        if ( stlStrcasecmp( aStmtName, sStmt->mStmtName ) == 0 )
        {
            break;
        }
        sPrevStmt = sStmt;
        sStmt = sStmt->mNext;
    }

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_STMT_NOT_EXIST );

    /*
     * Named Statement list 에서 제거 
     */

    sState = 1;
    if ( sPrevStmt == NULL )
    {
        gZtqNamedStmt = sStmt->mNext;
    }
    else
    {
        sPrevStmt->mNext = sStmt->mNext;
    }
    
    /*
     * ZtqSqlStmt DROP
     */

    sState = 2;
    STL_TRY( ztqDropNamedStmt( sStmt, aErrorStack ) == STL_SUCCESS );

    /*
     * Named Statment 공간 해제 
     */

    sState = 3;
    stlFreeHeap( sStmt );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_STMT_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NAMED_STMT_NOT_EXIST,
                      NULL,
                      aErrorStack,
                      aStmtName );
    }
    
    STL_FINISH;

    switch (sState)
    {
        case 0:
            break;
        case 1:
            (void) ztqDropNamedStmt( sStmt, aErrorStack );
        case 2:
            stlFreeHeap( sStmt );
        case 3:
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Named Statment 를 검색
 * @param[in]  aStmtName   Statement Name
 * @param[out] aNamedStmt  Named Statement
 * @param[in]  aErrorStack Error Stack
 * @remarks
 */
stlStatus ztqFindNamedStmt( stlChar        * aStmtName,
                            ztqNamedStmt  ** aNamedStmt,
                            stlErrorStack  * aErrorStack )
{
    ztqNamedStmt * sStmt = NULL;

    sStmt = gZtqNamedStmt;

    while ( sStmt != NULL )
    {
        if ( stlStrcasecmp( aStmtName, sStmt->mStmtName ) == 0 )
        {
            break;
        }
        sStmt = sStmt->mNext;
    }

    *aNamedStmt = sStmt;
    
    return STL_SUCCESS;
}

/**
 * @brief 할당된 모든 Named Statement 를 제거한다.
 * @param[in] aErrorStack
 * @remarks
 * 에러가 발생해도 무시하고 모든 자원을 해제한다.
 */
void ztqDestAllNamedStmt( stlErrorStack  * aErrorStack )
{
    ztqNamedStmt * sStmt = NULL;
    ztqNamedStmt * sNextStmt = NULL;

    sStmt = gZtqNamedStmt;

    while( sStmt != NULL )
    {
        sNextStmt = sStmt->mNext;

        (void) ztqDropNamedStmt( sStmt, aErrorStack );
        stlFreeHeap( sStmt );

        sStmt = sNextStmt;
    }

    gZtqNamedStmt = NULL;
}
                               
/** @} */

