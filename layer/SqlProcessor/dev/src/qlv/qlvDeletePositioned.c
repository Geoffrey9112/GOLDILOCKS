/*******************************************************************************
 * qlvDeletePositioned.c
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
 * @file qlvDeletePositioned.c
 * @brief SQL Processor Layer : Validation of DELELE CURRENT OF statement
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlvDeletePositioned
 * @{
 */


/**
 * @brief DELETE CURRENT OF 구문을 Validation 한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aSQLString    SQL String 
 * @param[in]      aParseTree    Parse Tree
 * @param[in]      aEnv          Environment
 */
stlStatus qlvValidateDeletePositioned( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       stlChar       * aSQLString,
                                       qllNode       * aParseTree,
                                       qllEnv        * aEnv )
{
    qlpDelete     * sParseTree = NULL;
    qlvInitDelete * sInitPlan  = NULL;

    stlChar  * sString = NULL;
    stlInt32   sStrLen = 0;

    ellCursorInstDesc      * sCursorInstDesc = NULL;
    qllStatement           * sCursorSQLStmt = NULL;
    
    qllResultSetDesc       * sResultSetDesc = NULL;
    qllCursorLockItem      * sLockTableItem = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sParseTree = (qlpDelete *) aParseTree;

    /**
     * Positioned DML 에는 Hint 를 사용할 수 없다.
     * - Validation 단계에서 Hint 를 검사하는 이유는
     * - Parser 단계에서 BNF 로 제약할 경우 shift/reduce warning 을 처리하기가 난감하기 때문임.
     */

    STL_TRY_THROW( sParseTree->mHints == NULL, RAMP_ERR_NOT_ALLOW_HINT_POSITIONED_DML );
    
    /*****************************************************************
     * DELETE 기본 구문 Validation
     *****************************************************************/
    
    /**
     * DELETE 기본 구문 Validation
     */
    
    STL_TRY( qlvValidateDelete( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                aSQLString,
                                aParseTree,
                                aEnv )
             == STL_SUCCESS );

    sInitPlan = (qlvInitDelete *) aSQLStmt->mInitPlan;

    /*****************************************************************
     * Cursor Validation
     * - PSM, Embedded SQL 과 달리 ODBC 는 Cursor 를 재선언할 수 있다.
     * - 따라서, Run-Time Validation 이 필요하다.
     *****************************************************************/
    
    /**
     * Cursor Key 정보 생성 
     */
    
    sString = QLP_GET_PTR_VALUE( sParseTree->mCursorName );
    sStrLen = stlStrlen(sString);

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                sStrLen + 1,
                                (void **) & sInitPlan->mCursorName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemcpy( sInitPlan->mCursorName, sString, sStrLen );
    sInitPlan->mCursorName[sStrLen] = '\0';

    sInitPlan->mCursorProcID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Cursor 가 선언되어 있어야 함.
     */

    
    STL_TRY( ellFindCursorInstDesc( sInitPlan->mCursorName,
                                    sInitPlan->mCursorProcID, 
                                    & sCursorInstDesc,
                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sCursorInstDesc != NULL, RAMP_ERR_CURSOR_NOT_EXIST );
    
    /**
     * Cursor 가 Open 되어 있어야 함.
     */

    STL_TRY_THROW( ellCursorIsOpen( sCursorInstDesc ) == STL_TRUE,
                   RAMP_ERR_CURSOR_IS_NOT_OPEN );
    
    /**
     * Result Set Desciptor 가 Updatable Cursor 여야 함.
     */

    sCursorSQLStmt = (qllStatement *)     sCursorInstDesc->mCursorSQLStmt;
    sResultSetDesc = (qllResultSetDesc *) sCursorSQLStmt->mResultSetDesc;
    
    STL_TRY_THROW( sResultSetDesc->mCursorProperty.mUpdatability == ELL_CURSOR_UPDATABILITY_FOR_UPDATE,
                   RAMP_ERR_CURSOR_IS_NOT_UPDATABLE );
                   
    /**
     * 갱신할 Table 이 Result Set 의 Lock Item 목록에 있어야 함.
     */

    for ( sLockTableItem = sResultSetDesc->mLockItemList;
          sLockTableItem != NULL;
          sLockTableItem = sLockTableItem->mNext )
    {
        /**
         * DELETE table 과 동일한 Result Set Desc 의 Lock Item 이 존재하는 지 검사
         */
        
        if ( ellGetTableHandle( & sInitPlan->mTableHandle ) == sLockTableItem->mTablePhyHandle )
        {
            /**
             * 동일한 Table 을 찾음
             */
            break;
        }
    }

    STL_TRY_THROW( sLockTableItem != NULL, RAMP_ERR_CURSOR_NOT_IDENTIFY_TABLE );

    /*****************************************************
     * CURRENT OF 를 위한 skip, fetch count 설정 
     *****************************************************/
    
    sInitPlan->mSkipCnt  = 0;
    sInitPlan->mFetchCnt = 1;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_ALLOW_HINT_POSITIONED_DML )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_HINT_NOT_ALLOWED_POSITIONED_DELETE,
                      NULL,
                      QLL_ERROR_STACK(aEnv));
    }

    STL_CATCH( RAMP_ERR_CURSOR_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCursorName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mCursorName );
    }

    STL_CATCH( RAMP_ERR_CURSOR_IS_NOT_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_IS_NOT_OPEN,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCursorName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mCursorName );
    }

    STL_CATCH( RAMP_ERR_CURSOR_IS_NOT_UPDATABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_IS_NOT_UPDATABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCursorName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mCursorName );
    }

    STL_CATCH( RAMP_ERR_CURSOR_NOT_IDENTIFY_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_CANNOT_IDENTIFY_UNDERLYING_TABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mRelation->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlvDeletePositioned */


