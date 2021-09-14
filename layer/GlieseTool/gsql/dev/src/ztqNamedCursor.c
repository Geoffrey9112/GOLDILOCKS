/*******************************************************************************
 * ztqNamedCursor.c
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
 * @file ztqNamedCursor.c
 * @brief Named Cursor Routines
 */

#include <stl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztqDef.h>
#include <ztqOdbcBridge.h>
#include <ztqHostVariable.h>
#include <ztqNamedCursor.h>

/**
 * @addtogroup ztqNamedCursor
 * @{
 */

ztqNamedCursor * gNamedCursor = NULL;


/**
 * @brief Named Cursor 를 추가
 * @param[in]  aZtqSqlStmt DECLARE CURSOR statement
 * @param[in]  aErrorStack Error Stack 
 * @remarks
 */
stlStatus ztqAddNamedCursor( ztqSqlStmt    * aZtqSqlStmt,
                             stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;
    
    ztqNamedCursor * sCursor = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aZtqSqlStmt != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aZtqSqlStmt->mStatementType == ZTQ_STMT_DECLARE_CURSOR,
                        aErrorStack );
    
    /*
     * Named Cursor 공간 할당
     */

    STL_TRY( stlAllocHeap( (void **) & sCursor,
                           STL_SIZEOF(ztqNamedCursor),
                           aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    stlStrncpy( sCursor->mCursorName, aZtqSqlStmt->mObjectName, STL_MAX_SQL_IDENTIFIER_LENGTH );

    STL_TRY( ztqCloneHostVarList( aZtqSqlStmt->mHostVarList,
                                  & sCursor->mHostVarList,
                                  aErrorStack )
             == STL_SUCCESS );
    sCursor->mNext = NULL;
    
    /*
     * Named Statement list 에 추가
     */

    if ( gNamedCursor == NULL )
    {
        gNamedCursor = sCursor;
    }
    else
    {
        sCursor->mNext = gNamedCursor;
        gNamedCursor   = sCursor;
    }
       
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 1:
            stlFreeHeap( sCursor );
        case 0:
        default:
            break;
    }
    
    return STL_FAILURE;
}




/**
 * @brief OPEN cursor 에 해당하는 Host Variable 설정 
 * @param[in]  aCursorName   Cusor Name
 * @param[out] aNamedCursor  Named Statement
 * @remarks
 */
void ztqFindNamedCursor( stlChar         * aCursorName,
                         ztqNamedCursor ** aNamedCursor )
{
    ztqNamedCursor * sCursor = NULL;
    
    sCursor = gNamedCursor;

    while ( sCursor != NULL )
    {
        if ( stlStrcasecmp( sCursor->mCursorName, aCursorName ) == 0 )
        {
            break;
        }
        sCursor = sCursor->mNext;
    }

    *aNamedCursor = sCursor;
}

/**
 * @brief 할당된 모든 Named Statement 를 제거한다.
 * @param[in] aErrorStack
 * @remarks
 * 에러가 발생해도 무시하고 모든 자원을 해제한다.
 */
void ztqDestAllNamedCursor( stlErrorStack  * aErrorStack )
{
    ztqNamedCursor * sCursor = NULL;
    ztqNamedCursor * sNextCursor = NULL;

    ztqHostVariable * sHost = NULL;
    ztqHostVariable * sNextHost = NULL;
    
    sCursor = gNamedCursor;

    while( sCursor != NULL )
    {
        sNextCursor = sCursor->mNext;

        sHost = sCursor->mHostVarList;
        
        while( sHost != NULL)
        {
            sNextHost = (ztqHostVariable *) sHost->mNext;
            stlFreeHeap( sHost );
            sHost = sNextHost;
        }

        sCursor->mHostVarList = NULL;
        
        stlFreeHeap( sCursor );

        sCursor = sNextCursor;
    }

    gNamedCursor = NULL;
}
                               
/** @} */

