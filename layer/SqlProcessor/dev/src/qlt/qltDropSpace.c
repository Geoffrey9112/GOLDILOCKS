/*******************************************************************************
 * qltDropSpace.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qltDropSpace.c 5803 2012-09-27 06:40:12Z jhkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qltDropSpace.c
 * @brief DROP TABLESPACE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qltDropSpace DROP TABLESPACE
 * @ingroup qlt
 * @{
 */


/**
 * @brief DROP TABLESPACE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltValidateDropSpace( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv )
{
    stlInt32 sState = 0;
    
    qltInitDropSpace  * sInitPlan = NULL;
    qlpDropTablespace * sParseTree = NULL;
    stlBool             sObjectExist = STL_FALSE;

    ellObjectList * sInnerTableList = NULL;
    ellObjectList * sOuterTableInnerPrimaryKeyList = NULL;
    ellObjectList * sOuterTableInnerUniqueKeyList = NULL;
    ellObjectList * sOuterTableInnerIndexList = NULL;
    ellObjectList * sInnerForeignKeyList = NULL;
    ellObjectList * sOuterForeignKeyList = NULL;

    smlStatement  * sStmt = NULL;
    stlInt32        sStmtAttr = 0;
    stlInt64        sDatabaseTestOption = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_TABLESPACE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_DROP_TABLESPACE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    sDatabaseTestOption = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION,
                                                         KNL_ENV(aEnv) );

    STL_TRY_THROW( sDatabaseTestOption < 2, RAMP_ERR_DISALLOW_STATEMENT );
    
    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpDropTablespace *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qltInitDropSpace),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qltInitDropSpace) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /**
     * Statement 할당
     */
    
    sStmtAttr = SML_STMT_ATTR_READONLY;
    
    STL_TRY( smlAllocStatement( aTransID,
                                NULL, /* aUpdateRelHandle */
                                sStmtAttr,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_TRUE, /* aNeedSnapshotIterator */
                                & sStmt,
                                SML_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    aSQLStmt->mViewSCN = sStmt->mScn;
    
    /*************************************************
     * TABLESPACE name 검증 
     *************************************************/

    /**
     * mIfExists
     */

    sInitPlan->mIfExists = sParseTree->mIfExists;
    
    /**
     * Tablespace 존재 여부
     */

    STL_TRY( ellGetTablespaceDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         QLP_GET_PTR_VALUE( sParseTree->mName ),
                                         & sInitPlan->mSpaceHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sObjectExist == STL_TRUE )
    {
        /**
         * Tablespace 가 존재함
         */
        
        sInitPlan->mSpaceExist = STL_TRUE;
    }
    else
    {
        /**
         * Tablespace 가 없음
         */
        
        sInitPlan->mSpaceExist = STL_FALSE;
            
        /**
         * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
         */
        
        if ( sInitPlan->mIfExists == STL_TRUE )
        {
            STL_THROW( RAMP_IF_EXISTS_NO_SPACE );
        }
        else
        {
            STL_THROW( RAMP_ERR_SPACE_NOT_EXIST );
        }
    }
    
    /*************************************************
     * TABLESPACE 검증 
     *************************************************/

    /**
     * System tablespace 검사
     */

    STL_TRY_THROW( ellIsBuiltInTablespace( &sInitPlan->mSpaceHandle ) == STL_FALSE,
                   RAMP_ERR_DROP_BUILTIN_TABLESPACE );

    /**
     * Usage 검사
     */

    switch( ellGetTablespaceUsageType( &sInitPlan->mSpaceHandle ) )
    {
        case ELL_SPACE_USAGE_TYPE_DICT:
            STL_THROW( RAMP_ERR_DROP_BUILTIN_TABLESPACE );
            break;
        case ELL_SPACE_USAGE_TYPE_UNDO:
            /**
             * @todo 향후 UNDO space 도 DROP TABLESPACE 가능해야 함
             */
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
            break;
        case ELL_SPACE_USAGE_TYPE_DATA:
            break;
        case ELL_SPACE_USAGE_TYPE_TEMPORARY:
            break;
        default:
            STL_ASSERT(0);
            break;
    }
        
    /**
     * DROP TABLESPACE 권한 검사
     */

    STL_TRY( qlaCheckPrivDropSpace( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    aEnv )
             == STL_SUCCESS );
    
    /*************************************************
     * TABLESPACE OPTION 정보 획득  
     *************************************************/

    if( sParseTree->mIsIncluding == NULL )
    {
        sInitPlan->mIsIncluding = STL_FALSE;
    }
    else
    {
        sInitPlan->mIsIncluding = STL_TRUE;
    }

    if( sParseTree->mIsCascade == NULL )
    {
        sInitPlan->mIsCascade = STL_FALSE;
    }
    else
    {
        sInitPlan->mIsCascade = STL_TRUE;
    }
    
    if( sParseTree->mDropDatafiles == NULL )
    {
        sInitPlan->mIsDropFiles = STL_FALSE;
    }
    else
    {
        switch( QLP_GET_INT_VALUE( sParseTree->mDropDatafiles ) )
        {
            case QLP_DROP_DATAFILES_KEEP:
                sInitPlan->mIsDropFiles = STL_FALSE;
                break;
            case QLP_DROP_DATAFILES_AND:
                sInitPlan->mIsDropFiles = STL_TRUE;
                break;
            case QLP_DROP_DATAFILES_INVALID:
            default:
                STL_ASSERT(0);
                break;
        }
    }

    /*************************************************
     * Option Validation
     *************************************************/

    /**
     * Drop Tablespace 관련 객체 정보 획득
     */

    STL_TRY( ellGetObjectList4DropSpace( aTransID,
                                         sStmt,
                                         & sInitPlan->mSpaceHandle,
                                         QLL_INIT_PLAN(aDBCStmt),
                                         & sInnerTableList,
                                         & sOuterTableInnerPrimaryKeyList,
                                         & sOuterTableInnerUniqueKeyList,
                                         & sOuterTableInnerIndexList,
                                         & sInnerForeignKeyList,
                                         & sOuterForeignKeyList,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * INCLUDING CONTENTS 검증
     */
    
    if ( sInitPlan->mIsIncluding == STL_TRUE )
    {
        /**
         * nothing to do
         */
    }
    else
    {
        /**
         * TABLESPACE 내에 객체가 존재하지 않아야 함
         */

        STL_TRY_THROW( ellHasObjectItem( sInnerTableList ) == STL_FALSE,
                       RAMP_ERR_TABLESPACE_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerPrimaryKeyList ) == STL_FALSE,
                       RAMP_ERR_TABLESPACE_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerUniqueKeyList ) == STL_FALSE,
                       RAMP_ERR_TABLESPACE_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerIndexList ) == STL_FALSE,
                       RAMP_ERR_TABLESPACE_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sInnerForeignKeyList ) == STL_FALSE,
                       RAMP_ERR_TABLESPACE_NOT_EMPTY );
    }

    /**
     * CASCADE CONSTRAINTS 검증
     */
    
    if ( sInitPlan->mIsCascade == STL_TRUE )
    {
        /**
         * nothing to do
         */
    }
    else
    {
        /**
         * TABLESPACE 관련한 Unique Key 객체 (Space 내부 또는 Space 내부 Table 의 Key)를
         * TABLESPACE 외부의 Foreign Key 가 참조하지 않아야 함.
         */

        STL_TRY_THROW( ellHasObjectItem( sOuterForeignKeyList ) == STL_FALSE,
                       RAMP_ERR_TABLESPACE_HAS_OUTER_REFERENCE );
    } 

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mSpaceHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /*************************************************
     * Plan 연결 
     *************************************************/

    /**
     * IF EXISTS 구문이 있을 경우, Tablespace 가 존재하지 않는다면,
     * 수행없이 SUCCESS 하게 되고, 권한 검사 및 Writable 은 검사하지 않는다.
     */
    
    STL_RAMP( RAMP_IF_EXISTS_NO_SPACE );
    
    /**
     * Statement 해제
     */
    
    sState = 0;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV(aEnv) ) == STL_SUCCESS );
    sStmt = NULL;
        
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SPACE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mName ) );
    }
    
    STL_CATCH( RAMP_ERR_DROP_BUILTIN_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DROP_BUILTIN_TABLESPACE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_TABLESPACE_NOT_EMPTY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_EMPTY,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_TABLESPACE_HAS_OUTER_REFERENCE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_KEY_REFERENCED_BY_FOREIGN_KEY,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qltValidateDropSpace()" );
    }
    
    STL_CATCH( RAMP_ERR_DISALLOW_STATEMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DISALLOW_STATEMENT,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    switch (sState)
    {
        case 1:
            (void) smlFreeStatement( sStmt, SML_ENV(aEnv) );
        default:
            break;
    }
    
    return STL_FAILURE;
}



/**
 * @brief DROP TABLESPACE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptCodeDropSpace( smlTransId      aTransID,
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
 * @brief DROP TABLESPACE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptDataDropSpace( smlTransId      aTransID,
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
 * @brief DROP TABLESPACE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qltExecuteDropSpace( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv )
{
    qltInitDropSpace * sInitPlan = NULL;

    ellObjectList * sInnerTableList = NULL;
    ellObjectList * sOuterTableInnerPrimaryKeyList = NULL;
    ellObjectList * sOuterTableInnerUniqueKeyList = NULL;
    ellObjectList * sOuterTableInnerIndexList = NULL;
    ellObjectList * sInnerForeignKeyList = NULL;
    ellObjectList * sOuterForeignKeyList = NULL;

    ellObjectList * sAffectedViewList = NULL;
    
    ellObjectList * sOuterTableInnerPrimaryKeyTableList = NULL;
    ellObjectList * sOuterTableInnerPrimaryKeyIndexList = NULL;

    ellObjectList * sOuterTableInnerUniqueKeyTableList = NULL;
    ellObjectList * sOuterTableInnerUniqueKeyIndexList = NULL;

    ellObjectList * sOuterTableInnerIndexTableList = NULL;
    
    ellObjectList * sInnerForeignKeyParentTableList = NULL;
    ellObjectList * sInnerForeignKeyChildTableList = NULL;
    ellObjectList * sInnerForeignKeyIndexList = NULL;

    ellObjectList * sOuterForeignKeyParentTableList = NULL;
    ellObjectList * sOuterForeignKeyChildTableList = NULL;
    ellObjectList * sOuterForeignKeyIndexList = NULL;
    
    stlTime  sTime = 0;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_TABLESPACE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * 기본 정보 설정 
     ***************************************************************/

    /**
     * DDL 수행 시간 설정
     */

    sTime = stlNow();
    STL_TRY( ellBeginStmtDDL( aStmt, sTime, ELL_ENV(aEnv) ) == STL_SUCCESS );

    /***************************************************************
     *  Run-Time Validation
     ***************************************************************/

    STL_RAMP( RAMP_RETRY );
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * Valid Plan 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );
    
    sInitPlan = (qltInitDropSpace *) aSQLStmt->mInitPlan;

    /**
     * IF EXISTS 옵션 처리
     */

    if ( (sInitPlan->mIfExists == STL_TRUE) &&
         (sInitPlan->mSpaceExist != STL_TRUE) )
    {
        /**
         * IF EXISTS 옵션을 사용하고, Space 가 존재하지 않는 경우
         */
        
        STL_THROW( RAMP_IF_EXISTS_SUCCESS );
    }
    else
    {
        /* go go */
    }
    
    /**
     * DDL Lock 획득 
     */

    STL_TRY( ellLock4DropTablespace( aTransID,
                                     aStmt,
                                     & sInitPlan->mSpaceHandle,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );

    /**
     * Valid Plan 을 다시 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qltInitDropSpace *) aSQLStmt->mInitPlan;

    /***************************************************************
     * 관련 정보 획득 및 Lock 획득
     ***************************************************************/
    
    /**
     * DROP TABLESPACE 관련 정보 획득
     */
    
    STL_TRY( ellGetObjectList4DropSpace( aTransID,
                                         aStmt,
                                         & sInitPlan->mSpaceHandle,
                                         & QLL_EXEC_DDL_MEM(aEnv),
                                         & sInnerTableList,
                                         & sOuterTableInnerPrimaryKeyList,
                                         & sOuterTableInnerUniqueKeyList,
                                         & sOuterTableInnerIndexList,
                                         & sInnerForeignKeyList,
                                         & sOuterForeignKeyList,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * INCLUDING CONTENTS 검증
     */
    
    if ( sInitPlan->mIsIncluding == STL_TRUE )
    {
        /**
         * nothing to do
         */
    }
    else
    {
        /**
         * TABLESPACE 내에 객체가 존재하지 않아야 함
         */

        STL_TRY_THROW( ellHasObjectItem( sInnerTableList ) == STL_FALSE,
                       RAMP_ERR_TABLESPACE_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerPrimaryKeyList ) == STL_FALSE,
                       RAMP_ERR_TABLESPACE_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerUniqueKeyList ) == STL_FALSE,
                       RAMP_ERR_TABLESPACE_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sOuterTableInnerIndexList ) == STL_FALSE,
                       RAMP_ERR_TABLESPACE_NOT_EMPTY );

        STL_TRY_THROW( ellHasObjectItem( sInnerForeignKeyList ) == STL_FALSE,
                       RAMP_ERR_TABLESPACE_NOT_EMPTY );
    }

    /**
     * CASCADE CONSTRAINTS 검증
     */

    if ( sInitPlan->mIsCascade == STL_TRUE )
    {
        /**
         * nothing to do
         */
    }
    else
    {
        /**
         * TABLESPACE 관련한 Unique Key 객체 (Space 내부 또는 Space 내부 Table 의 Key)를
         * TABLESPACE 외부의 Foreign Key 가 참조하지 않아야 함.
         */

        STL_TRY_THROW( ellHasObjectItem( sOuterForeignKeyList ) == STL_FALSE,
                       RAMP_ERR_TABLESPACE_HAS_OUTER_REFERENCE );
    } 
    
    /**
     * Inner Table List 에 X lock 획득
     */
    
    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_X,
                                          sInnerTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Outer Table Inner Primary Key 의 Table List 에 X lock 획득 
     */

    STL_TRY( ellLock4ConstraintList( aTransID,
                                     aStmt,
                                     sOuterTableInnerPrimaryKeyList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Outer Table Inner Unique Key 의 Table List 에 S lock 획득 
     */

    STL_TRY( ellLock4ConstraintList( aTransID,
                                     aStmt,
                                     sOuterTableInnerUniqueKeyList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Outer Table Inner Index 의 Table List 에 S lock 획득 
     */

    STL_TRY( ellLock4IndexList( aTransID,
                                aStmt,
                                sOuterTableInnerIndexList,
                                & sLocked,
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Inner Foreign Key 의 Table List 에 S lock 획득 
     */

    STL_TRY( ellLock4ConstraintList( aTransID,
                                     aStmt,
                                     sInnerForeignKeyList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Outer Foreign Key 의 Parent Table 과 Child Table 에 S lock 획득 
     */

    STL_TRY( ellLock4ConstraintList( aTransID,
                                     aStmt,
                                     sOuterForeignKeyList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    /**
     * Lock 을 모두 획득한 후 DROP TABLESPACE 관련 정보를 다시 획득
     */
    
    STL_TRY( ellGetObjectList4DropSpace( aTransID,
                                         aStmt,
                                         & sInitPlan->mSpaceHandle,
                                         & QLL_EXEC_DDL_MEM(aEnv),
                                         & sInnerTableList,
                                         & sOuterTableInnerPrimaryKeyList,
                                         & sOuterTableInnerUniqueKeyList,
                                         & sOuterTableInnerIndexList,
                                         & sInnerForeignKeyList,
                                         & sOuterForeignKeyList,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );


    /***************************************************************
     * 관련 객체로부터 영향을 받는 정보 회득 및 Lock 획득 
     ***************************************************************/

    /**
     * Inner Table 의 영향을 받는 Outer View List 획득 및 X lock 획득 
     */

    STL_TRY( ellGetAffectedOuterViewList4DropNonSchema( aTransID,
                                                        aStmt,
                                                        sInnerTableList,
                                                        & QLL_EXEC_DDL_MEM(aEnv),
                                                        & sAffectedViewList,
                                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_X,
                                          sAffectedViewList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetAffectedOuterViewList4DropNonSchema( aTransID,
                                                        aStmt,
                                                        sInnerTableList,
                                                        & QLL_EXEC_DDL_MEM(aEnv),
                                                        & sAffectedViewList,
                                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Outer Table Inner Primary Key 와 관련된 정보 획득 및 Table List 에 X lock 획득 
     */

    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerPrimaryKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerPrimaryKeyTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_X,
                                          sOuterTableInnerPrimaryKeyTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerPrimaryKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerPrimaryKeyTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sOuterTableInnerPrimaryKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sOuterTableInnerPrimaryKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Unique Key 와 관련된 정보 획득 및 Table List 에 S lock 획득 
     */

    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerUniqueKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerUniqueKeyTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_S,
                                          sOuterTableInnerUniqueKeyTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerUniqueKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerUniqueKeyTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    
    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sOuterTableInnerUniqueKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sOuterTableInnerUniqueKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Outer Table Inner Index 와 관련된 정보 획득 및 Table List 에 S lock 획득 
     */

    STL_TRY( ellGetTableList4IndexList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerIndexList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerIndexTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_S,
                                          sOuterTableInnerIndexTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetTableList4IndexList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerIndexList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerIndexTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Inner Foreign Key 와 관련된 정보 획득 및 Parent Table 에 S lock 획득 
     */

    STL_TRY( ellGetParentTableList4ForeignKeyList( aTransID,
                                                   ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                   sInnerForeignKeyList,
                                                   & QLL_EXEC_DDL_MEM(aEnv),
                                                   & sInnerForeignKeyParentTableList,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_S,
                                          sInnerForeignKeyParentTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetParentTableList4ForeignKeyList( aTransID,
                                                   ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                   sInnerForeignKeyList,
                                                   & QLL_EXEC_DDL_MEM(aEnv),
                                                   & sInnerForeignKeyParentTableList,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Inner Foreign Key 와 관련된 정보 획득 및 Child Table 에 S lock 획득 
     */
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sInnerForeignKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sInnerForeignKeyChildTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_S,
                                          sInnerForeignKeyChildTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sInnerForeignKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sInnerForeignKeyChildTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sInnerForeignKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sInnerForeignKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Outer Foreign Key 와 관련된 정보 획득 및 Parent Table 에 S lock 획득 
     */

    STL_TRY( ellGetParentTableList4ForeignKeyList( aTransID,
                                                   ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                   sOuterForeignKeyList,
                                                   & QLL_EXEC_DDL_MEM(aEnv),
                                                   & sOuterForeignKeyParentTableList,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_S,
                                          sOuterForeignKeyParentTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetParentTableList4ForeignKeyList( aTransID,
                                                   ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                   sOuterForeignKeyList,
                                                   & QLL_EXEC_DDL_MEM(aEnv),
                                                   & sOuterForeignKeyParentTableList,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Outer Foreign Key 와 관련된 정보 획득 및 Child Table 에 S lock 획득 
     */
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterForeignKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterForeignKeyChildTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_S,
                                          sOuterForeignKeyChildTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterForeignKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterForeignKeyChildTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sOuterForeignKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sOuterForeignKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************************************
     * 물리적 객체 제거 
     ***************************************************************/

    /**
     * Inner Foreign Key List 의 Index 제거
     */
    
    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sInnerForeignKeyIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Outer Foreign Key List 의 Index 제거
     */
    
    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sOuterForeignKeyIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Primary Key List 의 Index 제거
     */
    
    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sOuterTableInnerPrimaryKeyIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Unique Key List 의 Index 제거
     */
    
    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sOuterTableInnerUniqueKeyIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Index List 의 Index 제거
     */
    
    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sOuterTableInnerIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Inner Table List 의 Table Prime Element 객체 제거 
     */
    
    STL_TRY( qlrDropPhyTablePrimeElement4TableList( aTransID,
                                                    aStmt,
                                                    aDBCStmt,
                                                    sInnerTableList,
                                                    aEnv )
             == STL_SUCCESS );

    /**
     * Tablespace 의 물리적 객체 제거
     */

    STL_TRY( smlDropTablespace( aStmt,
                                ellGetTablespaceID( & sInitPlan->mSpaceHandle ),
                                sInitPlan->mIsDropFiles,
                                SML_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************************************
     * Dictionary Record 제거 
     ***************************************************************/

    /**
     * Affected View List 에 대해 IS_AFFECTED 를 TRUE 로 변경
     */

    STL_TRY( ellSetAffectedViewList( aTransID,
                                     aStmt,
                                     sAffectedViewList,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Inner Foreign Key List 의 Dictionary Record 삭제 
     */
    
    STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                  aStmt,
                                                  sInnerForeignKeyList,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                             aStmt,
                                             sInnerForeignKeyIndexList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Outer Foreign Key List 의 Dictionary Record 삭제 
     */
    
    STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                  aStmt,
                                                  sOuterForeignKeyList,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                             aStmt,
                                             sOuterForeignKeyIndexList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Outer Table Inner Primary Key List 의 Dictionary Record 삭제 
     */
    
    STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                  aStmt,
                                                  sOuterTableInnerPrimaryKeyList,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                             aStmt,
                                             sOuterTableInnerPrimaryKeyIndexList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlrSetColumnNullable4DropPrimaryKeyList( aTransID,
                                                      aStmt,
                                                      aDBCStmt,
                                                      sOuterTableInnerPrimaryKeyList,
                                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * Outer Table Inner Unique Key List 의 Dictionary Record 삭제 
     */
    
    STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                  aStmt,
                                                  sOuterTableInnerUniqueKeyList,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                             aStmt,
                                             sOuterTableInnerUniqueKeyIndexList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Index List 의 Dictionary Record 삭제 
     */
    
    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                             aStmt,
                                             sOuterTableInnerIndexList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Inner Table List 의 Dictionary Record 삭제 
     */
    
    STL_TRY( ellRemoveDictTablePrimeElement4TableList( aTransID,
                                                       aStmt,
                                                       sInnerTableList,
                                                       ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Tablespace 의 Dictionary Record 삭제 
     */

    STL_TRY( ellDeleteDict4DropSpace( aTransID,
                                      aStmt,
                                      & sInitPlan->mSpaceHandle,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /***************************************************************
     * Dictionary Cache 재구성 
     ***************************************************************/

    /**
     * Inner Foreign Key List 에 대한 Refine Cache
     */

    STL_TRY( qlrRefineParentList4DropForeignKeyList( aTransID,
                                                     aStmt,
                                                     sInnerForeignKeyList,
                                                     sInnerForeignKeyParentTableList,
                                                     aEnv )
             == STL_SUCCESS );
    
    STL_TRY( qlrRefineCache4DropKeyList( aTransID,
                                         aStmt,
                                         sInnerForeignKeyList,
                                         STL_FALSE,  /* aKeepIndex */
                                         sInnerForeignKeyIndexList,
                                         sInnerForeignKeyChildTableList,
                                         aEnv )
             == STL_SUCCESS );

    /**
     * Outer Foreign Key List 에 대한 Refine Cache
     */

    STL_TRY( qlrRefineParentList4DropForeignKeyList( aTransID,
                                                     aStmt,
                                                     sOuterForeignKeyList,
                                                     sOuterForeignKeyParentTableList,
                                                     aEnv )
             == STL_SUCCESS );
    
    STL_TRY( qlrRefineCache4DropKeyList( aTransID,
                                         aStmt,
                                         sOuterForeignKeyList,
                                         STL_FALSE,  /* aKeepIndex */
                                         sOuterForeignKeyIndexList,
                                         sOuterForeignKeyChildTableList,
                                         aEnv )
             == STL_SUCCESS );
    
    /**
     * Outer Table Inner Unique Key List 에 대한 Refine Cache
     */

    STL_TRY( qlrRefineCache4DropKeyList( aTransID,
                                         aStmt,
                                         sOuterTableInnerUniqueKeyList,
                                         STL_FALSE,  /* aKeepIndex */
                                         sOuterTableInnerUniqueKeyIndexList,
                                         sOuterTableInnerUniqueKeyTableList,
                                         aEnv )
             == STL_SUCCESS );
    
    /**
     * Outer Table Inner Index List 에 대한 Refine Cache
     */

    STL_TRY( qliRefineCache4DropIndexList( aTransID,
                                           aStmt,
                                           sOuterTableInnerIndexList,
                                           sOuterTableInnerIndexTableList,
                                           aEnv )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Primary Key List 에 대한 Refine Cache
     * - Table 관련 모든 Cache 가 재구성되므로 Outer Table 관련하여 마지막에 수행되어야 함.
     */

    STL_TRY( qlrRefineCache4DropKeyList( aTransID,
                                         aStmt,
                                         sOuterTableInnerPrimaryKeyList,
                                         STL_FALSE,  /* aKeepIndex */
                                         sOuterTableInnerPrimaryKeyIndexList,
                                         sOuterTableInnerPrimaryKeyTableList,
                                         aEnv )
             == STL_SUCCESS );
    
    /**
     * Inner Table List 에 대한 Refine Cache
     */

    STL_TRY( ellRefineCacheDropTablePrime4TableList( aTransID,
                                                     aStmt,
                                                     sInnerTableList,
                                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Tablespace 에 대한 Refine Cache
     */

    STL_TRY( ellRefineCache4DropSpace( aTransID,
                                       aStmt,
                                       & sInitPlan->mSpaceHandle,
                                       ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * If Exists 옵션을 사용하고, Tablespace 가 존재하지 않는 경우
     */

    STL_RAMP( RAMP_IF_EXISTS_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TABLESPACE_NOT_EMPTY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_EMPTY,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_TABLESPACE_HAS_OUTER_REFERENCE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_KEY_REFERENCED_BY_FOREIGN_KEY,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qltExecuteDropSpace()" ); */
    /* } */
    
    STL_FINISH;
    
    return STL_FAILURE;
}





/** @} qltDropSpace */
