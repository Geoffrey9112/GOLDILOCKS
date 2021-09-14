/*******************************************************************************
 * qlrDropTable.c
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
 * @file qlrDropTable.c
 * @brief DROP TABLE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlrDropTable DROP TABLE
 * @ingroup qlr
 * @{
 */


/**
 * @brief DROP TABLE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateDropTable( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv )
{
    stlInt32 sState = 0;
    
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitDropTable * sInitPlan = NULL;
    qlpDropTable     * sParseTree = NULL;

    stlBool   sObjectExist = STL_FALSE;

    ellObjectList * sInnerForeignKeyList   = NULL;
    ellObjectList * sOuterChildForeignKeyList   = NULL;

    smlStatement  * sStmt = NULL;
    stlInt32        sStmtAttr = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_TABLE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_DROP_TABLE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sParseTree = (qlpDropTable *) aParseTree;
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitDropTable),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitDropTable) );

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
    
    /**********************************************************
     * Table Name Validation
     **********************************************************/

    /**
     * mIfExists
     */

    sInitPlan->mIfExists = sParseTree->mIfExists;

    /**
     * Table Name 존재 여부
     */
    
    if ( sParseTree->mName->mSchema == NULL )
    {
        /**
         * Schema 가 명시되지 않은 경우
         */

        STL_TRY( ellGetTableDictHandleWithAuth( aTransID,
                                                aSQLStmt->mViewSCN,
                                                sAuthHandle,
                                                sParseTree->mName->mObject,
                                                & sInitPlan->mTableHandle,
                                                & sObjectExist,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sObjectExist == STL_TRUE )
        {
            /**
             * Table 이 존재함
             */
            
            sInitPlan->mTableExist = STL_TRUE;
        }
        else
        {
            /**
             * Table 이 없음
             */
            
            sInitPlan->mTableExist = STL_FALSE;
            
            /**
             * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
             */
            
            if ( sInitPlan->mIfExists == STL_TRUE )
            {
                STL_THROW( RAMP_IF_EXISTS_NO_TABLE );
            }
            else
            {
                STL_THROW( RAMP_ERR_TABLE_NOT_EXIST );
            }
        }

        /**
         * Table 의 Schema Handle 획득
         */
        
        STL_TRY( ellGetSchemaDictHandleByID(
                     aTransID,
                     aSQLStmt->mViewSCN,
                     ellGetTableSchemaID( & sInitPlan->mTableHandle ),
                     & sInitPlan->mSchemaHandle,
                     & sObjectExist,
                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXIST );
    }
    else
    {
        /**
         * Schema 가 명시된 경우
         */

        STL_TRY( ellGetSchemaDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         sParseTree->mName->mSchema,
                                         & sInitPlan->mSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sObjectExist == STL_TRUE )
        {
            /* Schema 가 존재함 */
        }
        else
        {
            /**
             * Schema 가 존재하지 않을 경우,
             * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
             */

            sInitPlan->mTableExist = STL_FALSE;
            
            if ( sInitPlan->mIfExists == STL_TRUE )
            {
                STL_THROW( RAMP_IF_EXISTS_NO_TABLE );
            }
            else
            {
                STL_THROW( RAMP_ERR_SCHEMA_NOT_EXIST );
            }
        }
        
        STL_TRY( ellGetTableDictHandleWithSchema( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  & sInitPlan->mSchemaHandle,
                                                  sParseTree->mName->mObject,
                                                  & sInitPlan->mTableHandle,
                                                  & sObjectExist,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sObjectExist == STL_TRUE )
        {
            /**
             * Table 이 존재함
             */
            
            sInitPlan->mTableExist = STL_TRUE;
        }
        else
        {
            /**
             * Table 이 없음
             */
            
            sInitPlan->mTableExist = STL_FALSE;
            
            /**
             * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
             */
            
            if ( sInitPlan->mIfExists == STL_TRUE )
            {
                STL_THROW( RAMP_IF_EXISTS_NO_TABLE );
            }
            else
            {
                STL_THROW( RAMP_ERR_TABLE_NOT_EXIST );
            }
        }
    }

    /**
     * Built-In Table 인지 검사
     */

    STL_TRY_THROW( ellIsBuiltInTable( & sInitPlan->mTableHandle ) == STL_FALSE,
                   RAMP_ERR_BUILT_IN_TABLE );

    /**
     * Base Table 인지 검사
     */
    
    STL_TRY_THROW( ELL_TABLE_TYPE_BASE_TABLE == ellGetTableType( & sInitPlan->mTableHandle ),
                   RAMP_ERR_NOT_BASE_TABLE );
    
    /**
     * DROP TABLE 권한 검사
     */

    STL_TRY( qlaCheckPrivDropTable( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    & sInitPlan->mTableHandle,
                                    aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Table 관련 객체 획득 
     **********************************************************/

    STL_TRY( ellGetObjectList4DropTable( aTransID,
                                         sStmt,
                                         & sInitPlan->mTableHandle,
                                         QLL_INIT_PLAN(aDBCStmt),
                                         & sInnerForeignKeyList,
                                         & sOuterChildForeignKeyList,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**********************************************************
     * CASCADE CONSTRAINTS
     **********************************************************/

    
    sInitPlan->mIsCascade = sParseTree->mIsCascade;

    if ( ellHasObjectItem( sOuterChildForeignKeyList ) == STL_TRUE )
    {
        /**
         * Outer Child Foreign Key 가 존재할 경우, CASCADE CONSTRAINTS 가 있어야 함.
         */

        STL_TRY_THROW( sInitPlan->mIsCascade == STL_TRUE, RAMP_ERR_CAUSED_BY_FOREIGN_KEY );
    }
    else
    {
        /**
         * No Problem
         */
    }

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mTableHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /***********************************************************
     * Init Plan 연결 
     ***********************************************************/
    
    /**
     * IF EXISTS 구문이 있을 경우, Table 이 존재하지 않는다면,
     * 수행없이 SUCCESS 하게 되고, 권한 검사 및 Writable 은 검사하지 않는다.
     */
    
    STL_RAMP( RAMP_IF_EXISTS_NO_TABLE );

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

    STL_CATCH( RAMP_ERR_TABLE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_OR_VIEW_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mName->mSchema );
    }

    STL_CATCH( RAMP_ERR_BUILT_IN_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_TABLE_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mName->mObject );
    }

    STL_CATCH( RAMP_ERR_NOT_BASE_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mName->mObject );
    }
    
    
    STL_CATCH( RAMP_ERR_CAUSED_BY_FOREIGN_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_KEY_REFERENCED_BY_FOREIGN_KEY,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mObjectPos,
                                           aEnv ),
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
 * @brief DROP TABLE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeDropTable( smlTransId      aTransID,
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
 * @brief DROP TABLE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataDropTable( smlTransId      aTransID,
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
 * @brief DROP TABLE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteDropTable( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv )
{
    qlrInitDropTable * sInitPlan = NULL;

    stlTime sTime = 0;
    stlBool   sLocked = STL_TRUE;

    ellObjectList * sInnerForeignKeyList   = NULL;
    ellObjectList * sOuterChildForeignKeyList   = NULL;

    ellObjectList * sParentTableList = NULL;
    ellObjectList * sChildTableList = NULL;

    ellObjectList * sForeignKeyIndexList = NULL;
    ellObjectList * sChildForeignKeyIndexList = NULL;
    
    ellObjectList * sAffectedViewList = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_TABLE_TYPE,
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

    sInitPlan = (qlrInitDropTable *) aSQLStmt->mInitPlan;

    /**
     * IF EXISTS 옵션 처리
     */

    if ( (sInitPlan->mIfExists == STL_TRUE) &&
         (sInitPlan->mTableExist != STL_TRUE) )
    {
        /**
         * IF EXISTS 옵션을 사용하고, Table 이 존재하지 않는 경우
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

    STL_TRY( ellLock4DropTable( aTransID,
                                aStmt,
                                & sInitPlan->mTableHandle,
                                & sLocked,
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );

    /**
     * Valid Plan 을 다시 획득
     * - 관련 객체에 Lock 을 획득 후 다시 Validation 해야 함.
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qlrInitDropTable *) aSQLStmt->mInitPlan;

    /***************************************************************
     *  관련 객체 정보 획득 및 Lock 획득 
     ***************************************************************/
    
    /**
     * Table 관련 객체 획득 
     */

    STL_TRY( ellGetObjectList4DropTable( aTransID,
                                         aStmt,
                                         & sInitPlan->mTableHandle,
                                         & QLL_EXEC_DDL_MEM(aEnv),
                                         & sInnerForeignKeyList,
                                         & sOuterChildForeignKeyList,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Foreign Key List 로부터 Parent Table List 에 대해 S Lock 을 획득 
     */
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sInnerForeignKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sParentTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_S,
                                          sParentTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sInnerForeignKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sParentTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Outer Child Foreign Key List 로부터 Child Table List 에 대해 S Lock 을 획득 
     */
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterChildForeignKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sChildTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_S,
                                          sChildTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterChildForeignKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sChildTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Affected View List 에 대해 X Lock 획득
     */
    
    STL_TRY( ellGetAffectedViewListByRelation( aTransID,
                                               aStmt,
                                               & sInitPlan->mTableHandle,
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
    
    STL_TRY( ellGetAffectedViewListByRelation( aTransID,
                                               aStmt,
                                               & sInitPlan->mTableHandle,
                                               & QLL_EXEC_DDL_MEM(aEnv),
                                               & sAffectedViewList,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************************************
     * 물리적 객체 제거 
     ***************************************************************/

    /**
     * Outer Child Foreign Key List 로부터 Index List 를 획득하여 제거 
     */
    
    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sOuterChildForeignKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sChildForeignKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sChildForeignKeyIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Inner Foreign Key List 로부터 Index List 를 획득하여 제거 
     */
    
    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sInnerForeignKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sForeignKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sForeignKeyIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Table Prime Element 의 객체 제거
     */
    
    STL_TRY( qlrDropPhyObject4TablePrimeElement( aTransID,
                                                 aStmt,
                                                 aDBCStmt,
                                                 & sInitPlan->mTableHandle,
                                                 aEnv )
             == STL_SUCCESS );
    
    /***************************************************************
     * Dictionary Record 제거 
     ***************************************************************/

    /**
     * Outer Child Foreign Key List 의 Dictionary Record 삭제 
     */
    
    STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                  aStmt,
                                                  sOuterChildForeignKeyList,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                             aStmt,
                                             sChildForeignKeyIndexList,
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
                                             sForeignKeyIndexList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Table 의 Dictionary 정보 제거
     */
    
    STL_TRY( ellRemoveDict4DropTablePrimeElement( aTransID,
                                                  aStmt,
                                                  & sInitPlan->mTableHandle,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Affected View List 에 대해 IS_AFFECTED 를 TRUE 로 변경
     */

    STL_TRY( ellSetAffectedViewList( aTransID,
                                     aStmt,
                                     sAffectedViewList,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    /***************************************************************
     * Dictionary Cache 재구성 
     ***************************************************************/

    /**
     * Outer Child Foreign Key List 에 대한 Refine Cache
     */
    
    STL_TRY( qlrRefineCache4DropKeyList( aTransID,
                                         aStmt,
                                         sOuterChildForeignKeyList,
                                         STL_FALSE,  /* means DROP INDEX */
                                         sChildForeignKeyIndexList,
                                         sChildTableList,
                                         aEnv )
             == STL_SUCCESS );

    /**
     * Inner Foreign Key List 에 대한 Refine Cache
     */
    
    STL_TRY( qlrRefineParentList4DropForeignKeyList( aTransID,
                                                     aStmt,
                                                     sInnerForeignKeyList,
                                                     sParentTableList,
                                                     aEnv )
             == STL_SUCCESS );
    
    /**
     * Table Prime Element 의 Cache 제거
     */

    STL_TRY( ellRefineCache4DropTablePrimeElement( aTransID,
                                                   aStmt,
                                                   & sInitPlan->mTableHandle,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                       
    /**
     * If Exists 옵션을 사용하고, Table 이 존재하지 않는 경우
     */
    
    STL_RAMP( RAMP_IF_EXISTS_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Table Prime Element 의 Physical Object 들을 제거한다.
 * @param[in] aTransID      Transaction ID
 * @param[in] aStmt         Statement
 * @param[in] aDBCStmt      DBC Statement
 * @param[in] aTableHandle  Table Handle
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus qlrDropPhyObject4TablePrimeElement( smlTransId      aTransID,
                                              smlStatement  * aStmt,
                                              qllDBCStmt    * aDBCStmt,
                                              ellDictHandle * aTableHandle,
                                              qllEnv        * aEnv )
{
    stlInt32        i = 0;
    
    stlInt32        sColumnCnt = 0;
    ellDictHandle * sColumnHandle = NULL;
    void          * sIdentityHandle = NULL;

    ellObjectList * sIndexObjectList = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    
    /***************************************************************
     *  시쿼스 제거
     ***************************************************************/
    
    /**
     * Identity Column 이 존재할 경우 Sequence 객체를 제거해야 함.
     */
    
    sColumnCnt = ellGetTableColumnCnt( aTableHandle );
    sColumnHandle = ellGetTableColumnDictHandle( aTableHandle );

    for ( i = 0; i < sColumnCnt; i++ )
    {
        sIdentityHandle = ellGetColumnIdentityHandle( & sColumnHandle[i] );
        
        if( sIdentityHandle != NULL )
        {
            STL_TRY( smlDropSequence( aStmt,
                                      sIdentityHandle,
                                      SML_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        else
        {
            /* Normal Column 임 */
        }
    }

    /***************************************************************
     *  Prime 인덱스 제거 (Foreign Key 는 외부 Table 과의 관계를 갖는다)
     ***************************************************************/
    
    /**
     * Table Prime Element 의 Index 목록 획득
     */

    STL_TRY( ellGetPrimeIndexList4DropTable( aTransID,
                                             aStmt,
                                             aTableHandle,
                                             & QLL_EXEC_DDL_MEM(aEnv),
                                             & sIndexObjectList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sIndexObjectList,
                                       aEnv )
             == STL_SUCCESS );
    

    /***************************************************************
     *  테이블 제거
     ***************************************************************/

    STL_TRY( smlDropTable( aStmt,
                           ellGetTableHandle( aTableHandle ),
                           SML_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Table List 를 순회하며 Table Prime Element 의 Physical Object 들을 제거한다.
 * @param[in] aTransID      Transaction ID
 * @param[in] aStmt         Statement
 * @param[in] aDBCStmt      DBC Statement
 * @param[in] aTableList    Table List
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus qlrDropPhyTablePrimeElement4TableList( smlTransId      aTransID,
                                                 smlStatement  * aStmt,
                                                 qllDBCStmt    * aDBCStmt,
                                                 ellObjectList * aTableList,
                                                 qllEnv        * aEnv )
{
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    ellTableType      sTableType = ELL_TABLE_TYPE_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableList != NULL, QLL_ERROR_STACK(aEnv) );


    STL_RING_FOREACH_ENTRY( & aTableList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        sTableType = ellGetTableType( sObjectHandle );

        switch ( sTableType )
        {
            case ELL_TABLE_TYPE_BASE_TABLE:
                STL_TRY( qlrDropPhyObject4TablePrimeElement( aTransID,
                                                             aStmt,
                                                             aDBCStmt,
                                                             sObjectHandle,
                                                             aEnv )
                         == STL_SUCCESS );
                break;
            case ELL_TABLE_TYPE_VIEW:
                /*
                 * nothing to do
                 */
                break;

            case ELL_TABLE_TYPE_GLOBAL_TEMPORARY:
            case ELL_TABLE_TYPE_LOCAL_TEMPORARY:
            case ELL_TABLE_TYPE_SYSTEM_VERSIONED:
                /*
                 * not implemented
                 */
                STL_DASSERT(0);
                break;

            case ELL_TABLE_TYPE_SEQUENCE:
            case ELL_TABLE_TYPE_SYNONYM:
            case ELL_TABLE_TYPE_FIXED_TABLE:
            case ELL_TABLE_TYPE_DUMP_TABLE:
            default:
                /*
                 * invalid use
                 */
                STL_DASSERT(0);
                break;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/** @} qlrDropTable */
