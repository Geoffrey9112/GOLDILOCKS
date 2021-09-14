/*******************************************************************************
 * qlrAlterTableDropConst.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlrAlterTableDropConst.c 5812 2012-09-27 07:56:09Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlrAlterTableDropConst.c
 * @brief ALTER TABLE .. DROP CONSTRAINT .. 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlrAlterTableDropConst ALTER TABLE .. DROP CONSTRAINT
 * @ingroup qlrAlterTable
 * @{
 */


/**
 * @brief DROP CONSTRAINT 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateDropConst( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv )
{
    stlInt32 sState = 0;
    
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitDropConst            * sInitPlan = NULL;
    qlpAlterTableDropConstraint * sParseTree = NULL;

    ellObjectList  * sChildForeignKeyList = NULL;
    stlBool          sHasOuterChildForeignKey = STL_FALSE;
    ellObjectList  * sNotNullList = NULL;

    ellTableConstraintType sConstType = ELL_TABLE_CONSTRAINT_TYPE_NA;

    smlStatement  * sStmt = NULL;
    stlInt32        sStmtAttr = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_DROP_CONSTRAINT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_TABLE_DROP_CONSTRAINT_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sParseTree = (qlpAlterTableDropConstraint *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitDropConst),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitDropConst) );

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

    STL_TRY( qlrGetAlterTableHandle( aTransID,
                                     aDBCStmt,
                                     aSQLStmt,
                                     aSQLString,
                                     sAuthHandle,
                                     sParseTree->mTableName,
                                     & sInitPlan->mSchemaHandle,
                                     & sInitPlan->mTableHandle,
                                     aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * Constraint Object Validation
     **********************************************************/

    STL_TRY( qlrGetDropConstHandle( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    aSQLString,
                                    & sInitPlan->mTableHandle,
                                    sParseTree->mConstObject,
                                    & sInitPlan->mConstHandle,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * ALTER TABLE 권한이 이미 검사됨
     */
    
    /**********************************************************
     * KEEP INDEX or DROP INDEX
     **********************************************************/

    sInitPlan->mKeepIndex = sParseTree->mKeepIndex;

    /**********************************************************
     * CASCADE CONSTRAINT
     **********************************************************/

    /**
     * Child Foreign Key 검증
     */
    
    sInitPlan->mIsCascade = sParseTree->mIsCascade;

    sConstType = ellGetConstraintType( & sInitPlan->mConstHandle );
    switch( sConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            {
                /**
                 * Outer Child Foreign Key 가 존재하는 지 확인
                 */
                
                if ( sConstType == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY )
                {
                    STL_TRY( ellGetObjectList4DropPK( aTransID,
                                                      sStmt,
                                                      & sInitPlan->mConstHandle,
                                                      QLL_INIT_PLAN(aDBCStmt),
                                                      & sChildForeignKeyList,
                                                      & sHasOuterChildForeignKey,
                                                      & sNotNullList,
                                                      ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( ellGetObjectList4DropUK( aTransID,
                                                      sStmt,
                                                      & sInitPlan->mConstHandle,
                                                      QLL_INIT_PLAN(aDBCStmt),
                                                      & sChildForeignKeyList,
                                                      & sHasOuterChildForeignKey,
                                                      ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }               
                
                if( sHasOuterChildForeignKey == STL_TRUE )
                {
                    /**
                     * Outer Child Foreign Key 가 존재한다면, CASCADE CONSTRAINTS 옵션이 필요함
                     */
                    
                    STL_TRY_THROW( sInitPlan->mIsCascade == STL_TRUE,
                                   RAMP_ERR_CAUSED_BY_FOREIGN_KEY );
                }
                else
                {
                    /**
                     * No Problem
                     */
                }
                
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                /**
                 * Nothing To Do
                 */
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            {
                /**
                 * Nothing To Do
                 */
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                /**
                 * Nothing To Do
                 */
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }
            

    /**
     * Statement 해제
     */
    
    sState = 0;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV(aEnv) ) == STL_SUCCESS );
    sStmt = NULL;

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
     * Init Plan 연결
     */
    
    aSQLStmt->mInitPlan = (void *) sInitPlan;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CAUSED_BY_FOREIGN_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_KEY_REFERENCED_BY_FOREIGN_KEY,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mConstObject->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlrValidateDropConst()" ); */
    /* } */

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
 * @brief DROP CONSTRAINT 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeDropConst( smlTransId      aTransID,
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
 * @brief DROP CONSTRAINT 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataDropConst( smlTransId      aTransID,
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
 * @brief DROP CONSTRAINT 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteDropConst( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv )
{
    qlrInitDropConst * sInitPlan = NULL;

    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;

    ellObjectList  * sChildForeignKeyList = NULL;
    ellObjectList  * sChildIndexList = NULL;
    stlBool          sHasOuterChildForeignKey = STL_FALSE;
    
    ellObjectList  * sParentKeyList = NULL;
    ellObjectList  * sNotNullList = NULL;

    ellObjectList  * sChildTableList = NULL;
    ellObjectList  * sParentTableList = NULL;

    ellDictHandle  * sIndexHandle = NULL;

    ellObjectList  * sPrimaryKeyList = NULL;
    
    ellTableConstraintType sConstType = ELL_TABLE_CONSTRAINT_TYPE_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_DROP_CONSTRAINT_TYPE,
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
    
    sInitPlan = (qlrInitDropConst *) aSQLStmt->mInitPlan;
    
    /**
     * DDL Lock 획득 
     */

    STL_TRY( ellLock4DropConst( aTransID,
                                aStmt,
                                & sInitPlan->mTableHandle,
                                & sInitPlan->mConstHandle,
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

    sInitPlan = (qlrInitDropConst *) aSQLStmt->mInitPlan;
    
    /***************************************************************
     *  Constraint 유형별 관련 객체에 대한 Lock 설정 
     ***************************************************************/
    
    /**
     *  Constraint 유형별 관련 객체에 대한 Lock 설정 
     */

    sConstType = ellGetConstraintType( & sInitPlan->mConstHandle );
    
    switch ( sConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            {
                if ( sConstType == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY )
                {
                    /**
                     * DROP PRIMARY KEY 를 위한 정보 획득
                     */
                    
                    STL_TRY( ellGetObjectList4DropPK( aTransID,
                                                      aStmt,
                                                      & sInitPlan->mConstHandle,
                                                      & QLL_EXEC_DDL_MEM(aEnv),
                                                      & sChildForeignKeyList,
                                                      & sHasOuterChildForeignKey,
                                                      & sNotNullList,
                                                      ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    /**
                     * DROP UNIQUE KEY 를 위한 정보 획득
                     */
                    
                    STL_TRY( ellGetObjectList4DropUK( aTransID,
                                                      aStmt,
                                                      & sInitPlan->mConstHandle,
                                                      & QLL_EXEC_DDL_MEM(aEnv),
                                                      & sChildForeignKeyList,
                                                      & sHasOuterChildForeignKey,
                                                      ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                }

                /**
                 * Run-Time Validation
                 */
                
                if( sHasOuterChildForeignKey == STL_TRUE )
                {
                    /**
                     * Outer Child Foreign Key 가 존재한다면, CASCADE CONSTRAINTS 옵션이 필요함
                     */
                    
                    STL_TRY_THROW( sInitPlan->mIsCascade == STL_TRUE,
                                   RAMP_ERR_CAUSED_BY_FOREIGN_KEY );
                }
                else
                {
                    /**
                     * No Problem
                     */
                }

                /**
                 * Child Foreign Key List 로부터 Child Table List 획득 및 S lock 을 획득
                 */

                STL_TRY( ellGetTableList4ConstList( aTransID,
                                                    ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                    sChildForeignKeyList,
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
                                                    sChildForeignKeyList,
                                                    & QLL_EXEC_DDL_MEM(aEnv),
                                                    & sChildTableList,
                                                    ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                /**
                 * Child Foreign Key List 로부터 Index List 획득
                 */
                
                STL_TRY( ellGetIndexList4KeyList( aTransID,
                                                  sChildForeignKeyList,
                                                  & QLL_EXEC_DDL_MEM(aEnv),
                                                  & sChildIndexList,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                /**
                 * Foreign Key 로부터 Parent Table List 획득 및 S lock 획득
                 */

                STL_TRY( ellGetObjectList4DropFK( aTransID,
                                                  aStmt,
                                                  & sInitPlan->mConstHandle,
                                                  & QLL_EXEC_DDL_MEM(aEnv),
                                                  & sParentKeyList,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_TRY( ellGetTableList4ConstList( aTransID,
                                                    ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                    sParentKeyList,
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
                                                    sParentKeyList,
                                                    & QLL_EXEC_DDL_MEM(aEnv),
                                                    & sParentTableList,
                                                    ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            {
                STL_TRY( ellGetObjectList4DropNotNull( aTransID,
                                                       aStmt,
                                                       & sInitPlan->mConstHandle,
                                                       & QLL_EXEC_DDL_MEM(aEnv),
                                                       & sPrimaryKeyList,
                                                       ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                /**
                 * Nothing To Do
                 */
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /***************************************************************
     *  Constraint 유형별 물리적 객체 삭제 
     ***************************************************************/

    switch ( sConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            {
                if ( sInitPlan->mKeepIndex == STL_TRUE )
                {
                    /**
                     * KEEP INDEX 임
                     */
                }
                else
                {
                    /**
                     * Child Foreign Key 의 Index 를 제거 
                     */

                                                      
                    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                                       sChildIndexList,
                                                       aEnv )
                             == STL_SUCCESS );

                    /**
                     * Key Constraint 의 Index 를 제거
                     */

                    sIndexHandle =
                        ellGetConstraintIndexDictHandle( & sInitPlan->mConstHandle );
                    
                    STL_TRY( smlDropIndex( aStmt,
                                           ellGetIndexHandle(sIndexHandle),
                                           SML_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                if ( sInitPlan->mKeepIndex == STL_TRUE )
                {
                    /**
                     * KEEP INDEX 임
                     */
                }
                else
                {
                    /**
                     * Key Constraint 의 Index 를 제거
                     */
                    
                    sIndexHandle =
                        ellGetConstraintIndexDictHandle( & sInitPlan->mConstHandle );
                    
                    STL_TRY( smlDropIndex( aStmt,
                                           ellGetIndexHandle(sIndexHandle),
                                           SML_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            {
                /**
                 * 물리적 객체가 존재하지 않음
                 */
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                /**
                 * 물리적 객체가 존재하지 않음
                 */
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /***************************************************************
     *  Constraint 유형별 Dictionary Record 삭제 
     ***************************************************************/

    switch ( sConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            {
                /**
                 * Child Foreign Key List 의 Dictionary Record 삭제 
                 */

                STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                              aStmt,
                                                              sChildForeignKeyList,
                                                              ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                if ( sInitPlan->mKeepIndex == STL_TRUE )
                {
                    STL_TRY( ellModifyKeyIndex2UserIndexByObjectList( aTransID,
                                                                      aStmt,
                                                                      sChildIndexList,
                                                                      ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                                             aStmt,
                                                             sChildIndexList,
                                                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                }
                
                /**
                 * PRIMARY/UNIQUE KEY 의 Dictionary Record 삭제 
                 */

                STL_TRY( ellDeleteDict4DropConstraint( aTransID,
                                                       aStmt,
                                                       & sInitPlan->mConstHandle,
                                                       ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                sIndexHandle = ellGetConstraintIndexDictHandle(&sInitPlan->mConstHandle);
                
                if ( sInitPlan->mKeepIndex == STL_TRUE )
                {
                    STL_TRY( ellModifyKeyIndex2UserIndex( aTransID,
                                                          aStmt,
                                                          sIndexHandle,
                                                          ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( ellDeleteDict4DropIndex( aTransID,
                                                      aStmt,
                                                      sIndexHandle,
                                                      ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
 
                if ( sConstType == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY )
                {
                    /**
                     * Column 의 Nullable 정보 변경
                     */
                    
                    STL_TRY( qlrSetColumnNullable4DropPK( aTransID,
                                                          aStmt,
                                                          & sInitPlan->mConstHandle,
                                                          sNotNullList,
                                                          aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /**
                     * Unique Key 는 Column 의 nullable 속성과 무관함
                     */
                }               
                
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                /**
                 * FOREIGN KEY 의 Dictionary Record 삭제 
                 */

                STL_TRY( ellDeleteDict4DropConstraint( aTransID,
                                                       aStmt,
                                                       & sInitPlan->mConstHandle,
                                                       ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                sIndexHandle = ellGetConstraintIndexDictHandle(&sInitPlan->mConstHandle);
                
                if ( sInitPlan->mKeepIndex == STL_TRUE )
                {
                    STL_TRY( ellModifyKeyIndex2UserIndex( aTransID,
                                                          aStmt,
                                                          sIndexHandle,
                                                          ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( ellDeleteDict4DropIndex( aTransID,
                                                      aStmt,
                                                      sIndexHandle,
                                                      ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            {
                /**
                 * NOT NULL 의 Dictionary Record 삭제 
                 */

                STL_TRY( ellDeleteDict4DropConstraint( aTransID,
                                                       aStmt,
                                                       & sInitPlan->mConstHandle,
                                                       ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                /**
                 * Column 의 Nullable 정보 변경
                 */

                STL_TRY( qlrSetColumnNullable4DropNotNull( aTransID,
                                                           aStmt,
                                                           & sInitPlan->mConstHandle,
                                                           sPrimaryKeyList,
                                                           aEnv )
                         == STL_SUCCESS );
                
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                /**
                 * CHECK 의 Dictionary Record 삭제 
                 */

                STL_TRY( ellDeleteDict4DropConstraint( aTransID,
                                                       aStmt,
                                                       & sInitPlan->mConstHandle,
                                                       ELL_ENV(aEnv) )
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
     * Table 구조가 변경된 시간을 설정
     * - Table Constraint 의 변경을 Table 변경 시간에 적용할 경우,
     * - 동시 수행시 table descriptor 를 위한 dictionary record 변경이 함께 발생하므로,
     * - 충분히 동시에 수행할 수 있는 ADD/DROP UNIQUE KEY 등을 동시에 수행할 수 없게 된다.
     */

    switch( sConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            STL_TRY( ellSetTime4ModifyTableElement( aTransID,
                                                    aStmt,
                                                    & sInitPlan->mTableHandle,
                                                    ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            /**
             * 동시에 수행할 수 있다.
             */
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            /**
             * 동시에 수행할 수 있다.
             */
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            STL_TRY( ellSetTime4ModifyTableElement( aTransID,
                                                    aStmt,
                                                    & sInitPlan->mTableHandle,
                                                    ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            /**
             * 동시에 수행할 수 있다.
             */
            break;
        default:
            STL_ASSERT(0);
            break;
    }
            
    
    
    /***************************************************************
     *  Constraint 유형별 Cache Refinement
     ***************************************************************/

    switch ( sConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            {
                /**
                 * Child Foreign Key List 에 대한 Refine Cache
                 */

                STL_TRY( qlrRefineCache4DropKeyList( aTransID,
                                                     aStmt,
                                                     sChildForeignKeyList,
                                                     sInitPlan->mKeepIndex,
                                                     sChildIndexList,
                                                     sChildTableList,
                                                     aEnv )
                         == STL_SUCCESS );
                
                /**
                 * Key Index 에 대한 Refine Cache
                 */
                
                sIndexHandle = ellGetConstraintIndexDictHandle(&sInitPlan->mConstHandle);
                
                if ( sInitPlan->mKeepIndex == STL_TRUE )
                {
                    STL_TRY( ellRefineCache4KeepKeyIndex( aTransID,
                                                          aStmt,
                                                          & sInitPlan->mTableHandle,
                                                          sIndexHandle,
                                                          ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( ellRefineCache4DropKeyIndex( aTransID,
                                                          sIndexHandle,
                                                          ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                /**
                 * Parent Key List 에 대한 Refine Cache
                 */

                STL_TRY( qlrRefineParentTable4DropForeignKey( aTransID,
                                                              aStmt,
                                                              & sInitPlan->mConstHandle,
                                                              sParentTableList,
                                                              aEnv )
                         == STL_SUCCESS );

                /**
                 * Key Index 에 대한 Refine Cache
                 */
                
                sIndexHandle = ellGetConstraintIndexDictHandle(&sInitPlan->mConstHandle);
                
                if ( sInitPlan->mKeepIndex == STL_TRUE )
                {
                    STL_TRY( ellRefineCache4KeepKeyIndex( aTransID,
                                                          aStmt,
                                                          & sInitPlan->mTableHandle,
                                                          sIndexHandle,
                                                          ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( ellRefineCache4DropKeyIndex( aTransID,
                                                          sIndexHandle,
                                                          ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            {
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }
    
    /**
     * DROP Constraint 에 대한 Refine Cache
     */
    
    STL_TRY( ellRefineCache4DropConstraint( aTransID,
                                            aStmt,
                                            & sInitPlan->mTableHandle,
                                            & sInitPlan->mConstHandle,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CAUSED_BY_FOREIGN_KEY )
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
    /*                   "qlrExecuteDropConst()" ); */
    /* } */

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief DROP constraint object 에 대한 Handle 을 획득한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aDBCStmt      DBC Statement
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aSQLString    SQL String
 * @param[in]  aTableHandle  Table Handle
 * @param[in]  aConstObject  ParseTree for Constraint
 * @param[out] aConstHandle  Constraint Handle
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus qlrGetDropConstHandle( smlTransId           aTransID,
                                 qllDBCStmt         * aDBCStmt,
                                 qllStatement       * aSQLStmt,
                                 stlChar            * aSQLString,
                                 ellDictHandle      * aTableHandle,
                                 qlpDropConstObject * aConstObject,
                                 ellDictHandle      * aConstHandle,
                                 qllEnv             * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstHandle != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Drop Constraint 유형에 따른 handle 획득
     */

    switch ( aConstObject->mDropConstType )
    {
        case QLP_DROP_CONST_NAME:
            STL_TRY( qlrGetDropNameConstHandle( aTransID,
                                                aDBCStmt,
                                                aSQLStmt,
                                                aSQLString,
                                                aTableHandle,
                                                aConstObject->mConstraintName,
                                                aConstHandle,
                                                aEnv )
                     == STL_SUCCESS );
            break;
        case QLP_DROP_CONST_PRIMARY_KEY:
            STL_TRY( qlrGetDropPKHandle( aTransID,
                                         aDBCStmt,
                                         aSQLStmt,
                                         aSQLString,
                                         aTableHandle,
                                         aConstObject,
                                         aConstHandle,
                                         aEnv )
                     == STL_SUCCESS );
            break;
        case QLP_DROP_CONST_UNIQUE:
            STL_TRY( qlrGetDropUKHandle( aTransID,
                                         aDBCStmt,
                                         aSQLStmt,
                                         aSQLString,
                                         aTableHandle,
                                         aConstObject,
                                         aConstHandle,
                                         aEnv )
                     == STL_SUCCESS );
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DROP CONSTRAINT name 에 대한 Handle 을 획득한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aDBCStmt        DBC Statement
 * @param[in]  aSQLStmt        SQL Statement
 * @param[in]  aSQLString      SQL String
 * @param[in]  aTableHandle    Table Handle
 * @param[in]  aConstName      Quantified Constraint Name
 * @param[out] aConstHandle    Constraint Handle
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qlrGetDropNameConstHandle( smlTransId           aTransID,
                                     qllDBCStmt         * aDBCStmt,
                                     qllStatement       * aSQLStmt,
                                     stlChar            * aSQLString,
                                     ellDictHandle      * aTableHandle,
                                     qlpObjectName      * aConstName,
                                     ellDictHandle      * aConstHandle,
                                     qllEnv             * aEnv )
{
    stlBool         sObjectExist = STL_FALSE;
    
    stlInt32        i = 0;
    
    stlInt32        sConstCnt = 0;
    ellDictHandle * sConstHandle = NULL;
    ellDictHandle * sMatchHandle = NULL;

    ellDictHandle   sSchemaHandle;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstHandle != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Quantified Constraint Name 이 존재하는 지 검사 
     */

    if ( aConstName->mSchema == NULL )
    {
        /**
         * Schema 가 명시되지 않은 경우
         */

        sMatchHandle = NULL;
        while (1)
        {
            /**
             * NOT NULL Constraint 에서 검색
             */
            STL_TRY( ellGetTableCheckNotNullDictHandle( aTransID,
                                                        aSQLStmt->mViewSCN,
                                                        QLL_INIT_PLAN(aDBCStmt),
                                                        aTableHandle,
                                                        & sConstCnt,
                                                        & sConstHandle,
                                                        ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            for ( i = 0; i < sConstCnt; i++ )
            {
                if ( stlStrcmp( ellGetConstraintName( & sConstHandle[i] ),
                                aConstName->mObject )
                     == 0 )
                {
                    sMatchHandle = & sConstHandle[i];
                    break;
                }
            }

            if ( sMatchHandle != NULL )
            {
                break;
            }
            
            /**
             * CHECK Constraint 에서 검색
             */
            
            STL_TRY( ellGetTableCheckClauseDictHandle( aTransID,
                                                       aSQLStmt->mViewSCN,
                                                       QLL_INIT_PLAN(aDBCStmt),
                                                       aTableHandle,
                                                       & sConstCnt,
                                                       & sConstHandle,
                                                       ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            for ( i = 0; i < sConstCnt; i++ )
            {
                if ( stlStrcmp( ellGetConstraintName( & sConstHandle[i] ),
                                aConstName->mObject )
                     == 0 )
                {
                    sMatchHandle = & sConstHandle[i];
                    break;
                }
            }

            if ( sMatchHandle != NULL )
            {
                break;
            }
            
            /**
             * PRIMARY KEY Constraint 에서 검색
             */
            
            STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                                      aSQLStmt->mViewSCN,
                                                      QLL_INIT_PLAN(aDBCStmt),
                                                      aTableHandle,
                                                      & sConstCnt,
                                                      & sConstHandle,
                                                      ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            for ( i = 0; i < sConstCnt; i++ )
            {
                if ( stlStrcmp( ellGetConstraintName( & sConstHandle[i] ),
                                aConstName->mObject )
                     == 0 )
                {
                    sMatchHandle = & sConstHandle[i];
                    break;
                }
            }

            if ( sMatchHandle != NULL )
            {
                break;
            }
            
            
            /**
             * UNIQUE Constraint 에서 검색
             */
            
            STL_TRY( ellGetTableUniqueKeyDictHandle( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     QLL_INIT_PLAN(aDBCStmt),
                                                     aTableHandle,
                                                     & sConstCnt,
                                                     & sConstHandle,
                                                     ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            for ( i = 0; i < sConstCnt; i++ )
            {
                if ( stlStrcmp( ellGetConstraintName( & sConstHandle[i] ),
                                aConstName->mObject )
                     == 0 )
                {
                    sMatchHandle = & sConstHandle[i];
                    break;
                }
            }

            if ( sMatchHandle != NULL )
            {
                break;
            }
            
            
            /**
             * FOREIGN KEY Constraint 에서 검색
             */
            
            STL_TRY( ellGetTableForeignKeyDictHandle( aTransID,
                                                      aSQLStmt->mViewSCN,
                                                      QLL_INIT_PLAN(aDBCStmt),
                                                      aTableHandle,
                                                      & sConstCnt,
                                                      & sConstHandle,
                                                      ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            for ( i = 0; i < sConstCnt; i++ )
            {
                if ( stlStrcmp( ellGetConstraintName( & sConstHandle[i] ),
                                aConstName->mObject )
                     == 0 )
                {
                    sMatchHandle = & sConstHandle[i];
                    break;
                }
            }

            break;
        }
        
        STL_TRY_THROW( sMatchHandle != NULL, RAMP_ERR_CONSTRAINT_NOT_EXIST );

        stlMemcpy( aConstHandle, sMatchHandle, STL_SIZEOF(ellDictHandle) );
    }
    else
    {
        /**
         * Schema 가 명시된 경우 
         */

        STL_TRY( ellGetSchemaDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         aConstName->mSchema,
                                         & sSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXIST );

        STL_TRY( ellGetConstraintDictHandleWithSchema( aTransID,
                                                       aSQLStmt->mViewSCN,
                                                       & sSchemaHandle,
                                                       aConstName->mObject,
                                                       aConstHandle,
                                                       & sObjectExist,
                                                       ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_CONSTRAINT_NOT_EXIST );
        STL_TRY_THROW( ellGetTableID(aTableHandle) == ellGetConstraintTableID(aConstHandle),
                       RAMP_ERR_CONSTRAINT_NOT_EXIST );
        
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CONSTRAINT_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DROP_CONSTRAINT_NONEXISTENT_CONSTRAINT,
                      qlgMakeErrPosString( aSQLString,
                                           aConstName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aConstName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      aConstName->mSchema );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DROP PRIMARY KEY 에 대한 Handle 을 획득한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aDBCStmt        DBC Statement
 * @param[in]  aSQLStmt        SQL Statement
 * @param[in]  aSQLString      SQL String
 * @param[in]  aTableHandle    Table Handle
 * @param[in]  aConstObject    ParseTree for Constraint
 * @param[out] aConstHandle    Constraint Handle
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qlrGetDropPKHandle( smlTransId           aTransID,
                              qllDBCStmt         * aDBCStmt,
                              qllStatement       * aSQLStmt,
                              stlChar            * aSQLString,
                              ellDictHandle      * aTableHandle,
                              qlpDropConstObject * aConstObject,
                              ellDictHandle      * aConstHandle,
                              qllEnv             * aEnv )
{
    stlInt32        sConstCnt = 0;
    ellDictHandle * sConstHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstObject->mDropConstType == QLP_DROP_CONST_PRIMARY_KEY,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstHandle != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * PRIMARY KEY Constraint 에서 검색
     */
    
    STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                              aSQLStmt->mViewSCN,
                                              QLL_INIT_PLAN(aDBCStmt),
                                              aTableHandle,
                                              & sConstCnt,
                                              & sConstHandle,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sConstCnt == 1, RAMP_ERR_PK_NOT_EXIST );
    
    stlMemcpy( aConstHandle, & sConstHandle[0], STL_SIZEOF(ellDictHandle) );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_PK_NOT_EXIST )
    {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_DROP_CONSTRAINT_NONEXISTENT_PRIMARY_KEY,
                          qlgMakeErrPosString( aSQLString,
                                               aConstObject->mNodePos,
                                               aEnv ),
                          QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief DROP UNIQUE(columns) 에 대한 Handle 을 획득한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aDBCStmt        DBC Statement
 * @param[in]  aSQLStmt        SQL Statement
 * @param[in]  aSQLString      SQL String
 * @param[in]  aTableHandle    Table Handle
 * @param[in]  aConstObject    ParseTree for Constraint
 * @param[out] aConstHandle    Constraint Handle
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qlrGetDropUKHandle( smlTransId           aTransID,
                              qllDBCStmt         * aDBCStmt,
                              qllStatement       * aSQLStmt,
                              stlChar            * aSQLString,
                              ellDictHandle      * aTableHandle,
                              qlpDropConstObject * aConstObject,
                              ellDictHandle      * aConstHandle,
                              qllEnv             * aEnv )
{
    stlInt32        i = 0;
    stlInt32        j = 0;
    stlBool         sFind = STL_FALSE;
    
    stlInt32        sConstCnt = 0;
    ellDictHandle * sConstHandle = NULL;

    stlInt32        sKeyColCnt = 0;
    ellDictHandle * sKeyColHandle = NULL;

    qlpListElement * sListElement = NULL;
    qlpValue       * sColumnElem  = NULL;
    stlChar        * sColumnName  = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstObject->mDropConstType == QLP_DROP_CONST_UNIQUE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstHandle != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * UNIQUE KEY Constraint 에서 검색
     */
    
    STL_TRY( ellGetTableUniqueKeyDictHandle( aTransID,
                                             aSQLStmt->mViewSCN,
                                             QLL_INIT_PLAN(aDBCStmt),
                                             aTableHandle,
                                             & sConstCnt,
                                             & sConstHandle,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sConstCnt > 0, RAMP_ERR_UK_NOT_EXIST );

    sFind = STL_FALSE;
    for ( i = 0; i < sConstCnt; i++ )
    {
        /**
         * Column Count 가 동일한지 검사
         */

        sKeyColCnt = ellGetUniqueKeyColumnCount( & sConstHandle[i] );
        
        if ( QLP_LIST_GET_COUNT(aConstObject->mUniqueColumnList) !=
             sKeyColCnt )
        {
            continue;
        }
        else
        {
            /**
             * 동일한 컬럼 이름으로 구성되었는지 검사 
             */
            
            sKeyColHandle = ellGetUniqueKeyColumnHandle( & sConstHandle[i] );

            j = 0;
            sFind = STL_TRUE;
            QLP_LIST_FOR_EACH( aConstObject->mUniqueColumnList, sListElement )
            {
                sColumnElem = (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sListElement );
                sColumnName = QLP_GET_PTR_VALUE(sColumnElem);

                if ( stlStrcmp( sColumnName, ellGetColumnName( & sKeyColHandle[j] ) )
                     != 0 )
                {
                    sFind = STL_FALSE;
                    break;
                }
                j++;
            }

            if ( sFind == STL_TRUE )
            {
                stlMemcpy( aConstHandle, & sConstHandle[i], STL_SIZEOF(ellDictHandle) );
                break;
            }
            else
            {
                continue;
            }
        }
    }
        
    STL_TRY_THROW( sFind == STL_TRUE, RAMP_ERR_UK_NOT_EXIST );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_UK_NOT_EXIST )
    {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_DROP_CONSTRAINT_NONEXISTENT_UNIQUE_KEY,
                          qlgMakeErrPosString( aSQLString,
                                               aConstObject->mNodePos,
                                               aEnv ),
                          QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DROP PRIMARY KEY 에 의해 컬럼을 NULLABLE 로 설정한다.
 * @param[in] aTransID      Transaction ID
 * @param[in] aStmt         Statement
 * @param[in] aPrimaryKeyHandle Priamry Key Handle
 * @param[in] aNotNullList      NOT NULL Object List
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus qlrSetColumnNullable4DropPK( smlTransId         aTransID,
                                       smlStatement     * aStmt,
                                       ellDictHandle    * aPrimaryKeyHandle,
                                       ellObjectList    * aNotNullList,
                                       qllEnv           * aEnv )
{
    stlInt32 i = 0;
    stlBool  sExist = STL_FALSE;
    
    ellDictHandle * sIndexHandle = NULL;

    stlInt32          sKeyCount = 0;
    ellIndexKeyDesc * sKeyDesc = NULL;

    ellObjectItem   * sObjectItem   = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    ellDictHandle   * sKeyColumnHandle = NULL;
    ellDictHandle   * sNotNullColumnHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrimaryKeyHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ellGetConstraintType(aPrimaryKeyHandle)
                         == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNotNullList != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * 기본 정보 획득
     */
    
    sIndexHandle = ellGetConstraintIndexDictHandle( aPrimaryKeyHandle );

    sKeyCount = ellGetIndexKeyCount( sIndexHandle );
    sKeyDesc = ellGetIndexKeyDesc( sIndexHandle );

    /**
     * Key 개수만큼 순회 
     */

    for ( i = 0; i < sKeyCount; i++ )
    {
        sKeyColumnHandle = & sKeyDesc[i].mKeyColumnHandle;
        
        /**
         * Key Column 을 포함하고 있는 NOT NULL 이 존재하는 지 확인
         */

        sExist = STL_FALSE;
        
        STL_RING_FOREACH_ENTRY( & aNotNullList->mHeadList, sObjectItem )
        {
            sObjectHandle = & sObjectItem->mObjectHandle;

            sNotNullColumnHandle = ellGetCheckNotNullColumnHandle( sObjectHandle );

            if ( ellGetColumnID(sKeyColumnHandle) == ellGetColumnID(sNotNullColumnHandle) )
            {
                sExist = STL_TRUE;
                break;
            }
            else
            {
                continue;
            }
        }

        if ( sExist == STL_TRUE )
        {
            /**
             * Column 의 Nullable 속성을 변경하지 않는다.
             */
        }
        else
        {
            if ( ellGetColumnIdentityHandle( sKeyColumnHandle ) != NULL )
            {
                /**
                 * Identity Column 이므로  Column 의 Nullable 속성을 변경하지 않는다.
                 */
            }
            else
            {
                /**
                 * Column 의 Nullable 속성을 변경: NOT NULL => Nullable
                 */
                
                STL_TRY( ellModifyColumnNullable( aTransID,
                                                  aStmt,
                                                  ellGetColumnID(sKeyColumnHandle),
                                                  STL_TRUE,  /* aIsNullable */
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );
            }
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief PRIMARY KEY list 로부터 컬럼을 NULLABLE 로 설정한다.
 * @param[in] aTransID        Transaction ID
 * @param[in] aStmt           Statement
 * @param[in] aDBCStmt        DBC Statement
 * @param[in] aPrimaryKeyList Priamry Key List
 * @param[in] aEnv            Environment
 * @remarks
 */
stlStatus qlrSetColumnNullable4DropPrimaryKeyList( smlTransId         aTransID,
                                                   smlStatement     * aStmt,
                                                   qllDBCStmt       * aDBCStmt,
                                                   ellObjectList    * aPrimaryKeyList,
                                                   qllEnv           * aEnv )
{
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    ellObjectList   * sNotNullList = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrimaryKeyList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Primary Key List 를 순회하며 Column 의 Nullable 정보를 변경한다.
     */
    
    STL_RING_FOREACH_ENTRY( & aPrimaryKeyList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        /**
         * Primary Key Table 의 Not Null List 정보 획득
         */

        sNotNullList = NULL;
        
        STL_TRY( ellGetTableConstTypeList( aTransID,
                                           aStmt,
                                           ellGetConstraintTableID( sObjectHandle ),
                                           ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL,
                                           & QLL_EXEC_DDL_MEM(aEnv),
                                           & sNotNullList,
                                           ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Primary Key 와 Not Null List 정보를 이용해 Column 의 Nullable 속성 결정
         */
        
        STL_TRY( qlrSetColumnNullable4DropPK( aTransID,
                                              aStmt,
                                              sObjectHandle,
                                              sNotNullList,
                                              aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    

/**
 * @brief DROP NOT NULL 에 의해 컬럼을 NULLABLE 로 설정한다.
 * @param[in] aTransID           Transaction ID
 * @param[in] aStmt              Statement
 * @param[in] aNotNullHandle     NOT NULL Handle
 * @param[in] aPrimaryKeyList    Primary Key Object List
 * @param[in] aEnv               Environment
 * @remarks
 */
stlStatus qlrSetColumnNullable4DropNotNull( smlTransId         aTransID,
                                            smlStatement     * aStmt,
                                            ellDictHandle    * aNotNullHandle,
                                            ellObjectList    * aPrimaryKeyList,
                                            qllEnv           * aEnv )
{
    stlInt32 i = 0;
    stlBool  sExist = STL_FALSE;
    
    ellObjectItem   * sObjectItem   = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    stlInt32          sKeyCount = 0;
    ellDictHandle   * sKeyColumnHandle = NULL;
    ellDictHandle   * sNotNullColumnHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNotNullHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ellGetConstraintType(aNotNullHandle)
                         == ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrimaryKeyList != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * 기본 정보 획득
     */

    sNotNullColumnHandle = ellGetCheckNotNullColumnHandle( aNotNullHandle );
    
    if ( ellGetColumnIdentityHandle( sNotNullColumnHandle ) != NULL )
    {
        /**
         * Identity Column 이므로 Not Nullable 이 유지됨
         */
    }
    else
    {
        /**
         * PRIMARY KEY 의 Key Column 에 포함된 컬럼인지 확인
         */
        
        sExist = STL_FALSE;
        
        STL_RING_FOREACH_ENTRY( & aPrimaryKeyList->mHeadList, sObjectItem )
        {
            sObjectHandle = & sObjectItem->mObjectHandle;
            
            sKeyCount = ellGetPrimaryKeyColumnCount( sObjectHandle );
            sKeyColumnHandle = ellGetPrimaryKeyColumnHandle( sObjectHandle );
            
            sExist = STL_FALSE;
            
            for ( i = 0; i < sKeyCount; i++ )
            {
                if ( ellGetColumnID( &sKeyColumnHandle[i] ) == ellGetColumnID(sNotNullColumnHandle) )
                {
                    sExist = STL_TRUE;
                    break;
                }
                else
                {
                    continue;
                }
            }
            
            if ( sExist == STL_TRUE )
            {
                break;
            }
            else
            {
                continue;
            }
        }
        
        /**
         * Not Null Column 의 속성 변경 
         */
        
        if ( sExist == STL_TRUE )
        {
            /**
             * Column 의 Nullable 속성을 변경하지 않는다.
             */
        }
        else
        {
            /**
             * Column 의 Nullable 속성을 변경: NOT NULL => Nullable
             */
            
            STL_TRY( ellModifyColumnNullable( aTransID,
                                              aStmt,
                                              ellGetColumnID(sNotNullColumnHandle),
                                              STL_TRUE,    /* aIsNullable */
                                              ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Key List 삭제에 대한 Cache 를 재구성한다.
 * @param[in] aTransID           Transaction ID
 * @param[in] aStmt              Statement
 * @param[in] aKeyList           Key List
 * @param[in] aKeepIndex         KEEP INDEX or DROP INDEX
 * @param[in] aKeyIndexList      Key Index List
 * @param[in] aKeyTableList      Key Table List
 * @param[in] aEnv               Environment
 * @remarks
 */
stlStatus qlrRefineCache4DropKeyList( smlTransId         aTransID,
                                      smlStatement     * aStmt,
                                      ellObjectList    * aKeyList,
                                      stlBool            aKeepIndex,
                                      ellObjectList    * aKeyIndexList,
                                      ellObjectList    * aKeyTableList,
                                      qllEnv           * aEnv )
{
    ellObjectItem   * sKeyItem   = NULL;
    ellDictHandle   * sKeyHandle = NULL;

    ellObjectItem   * sIndexItem   = NULL;
    ellDictHandle   * sIndexHandle = NULL;
    
    ellObjectItem   * sTableItem   = NULL;
    ellDictHandle   * sTableHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyIndexList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyTableList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Key List 를 순회하며, Cache 를 Refine 한다.
     */
    
    sIndexItem = STL_RING_GET_FIRST_DATA( & aKeyIndexList->mHeadList );
    sTableItem = STL_RING_GET_FIRST_DATA( & aKeyTableList->mHeadList );
    
    STL_RING_FOREACH_ENTRY( & aKeyList->mHeadList, sKeyItem )
    {
        sKeyHandle = & sKeyItem->mObjectHandle;

        sIndexHandle = & sIndexItem->mObjectHandle;
        sTableHandle = & sTableItem->mObjectHandle;

        /**
         * Drop Constraint 를 위한 Refine Cache
         */
        
        STL_TRY( ellRefineCache4DropConstraint( aTransID,
                                                aStmt,
                                                sTableHandle,
                                                sKeyHandle,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Drop Index 를 위한 Refine Cache
         */
        
        if ( aKeepIndex == STL_TRUE )
        {
            STL_TRY( ellRefineCache4KeepKeyIndex( aTransID,
                                                  aStmt,
                                                  sTableHandle,
                                                  sIndexHandle,
                                                  ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( ellRefineCache4DropKeyIndex( aTransID,
                                                  sIndexHandle,
                                                  ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        
        sIndexItem = STL_RING_GET_NEXT_DATA( & aKeyIndexList->mHeadList, sIndexItem );
        sTableItem = STL_RING_GET_NEXT_DATA( & aKeyTableList->mHeadList, sTableItem );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}






/**
 * @brief Foreign Key 제거를 위해 참조하는 Parent Table 에 대한 Cache 를 재구성한다.
 * @param[in] aTransID           Transaction ID
 * @param[in] aStmt              Statement
 * @param[in] aForeignKeyHandle  Foreign Key Handle
 * @param[in] aParentTableList   Parent Table List
 * @param[in] aEnv               Environment
 * @remarks
 */
stlStatus qlrRefineParentTable4DropForeignKey( smlTransId         aTransID,
                                               smlStatement     * aStmt,
                                               ellDictHandle    * aForeignKeyHandle,
                                               ellObjectList    * aParentTableList,
                                               qllEnv           * aEnv )
{
    ellObjectItem   * sObjectItem   = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aForeignKeyHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParentTableList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Parent Table List 를 순회하며, Cache 를 Refine 한다.
     * - 하나밖에 존재하지 않는다.
     */
    
    STL_RING_FOREACH_ENTRY( & aParentTableList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        /**
         * Table 에서 Child Foreign Key 를 Un-link
         */
        
        STL_TRY( ellUnlinkChildForeignKeyFromParentTable( aTransID,
                                                          aStmt,
                                                          sObjectHandle,
                                                          aForeignKeyHandle,
                                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Foreign Key 가 참조하는 Parent Table 에 대한 Cache 를 재구성한다.
 * @param[in] aTransID           Transaction ID
 * @param[in] aStmt              Statement
 * @param[in] aForeignKeyList    Foreign Key List
 * @param[in] aParentTableList   Parent Table List
 * @param[in] aEnv               Environment
 * @remarks
 */
stlStatus qlrRefineParentList4DropForeignKeyList( smlTransId         aTransID,
                                                  smlStatement     * aStmt,
                                                  ellObjectList    * aForeignKeyList,
                                                  ellObjectList    * aParentTableList,
                                                  qllEnv           * aEnv )
{
    ellObjectItem   * sKeyItem   = NULL;
    ellDictHandle   * sKeyHandle = NULL;

    ellObjectItem   * sTableItem   = NULL;
    ellDictHandle   * sTableHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aForeignKeyList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParentTableList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Foreign Key List 를 순회하며, Parent Table 의 Cache 를 Refine 한다.
     */
    
    sTableItem = STL_RING_GET_FIRST_DATA( & aParentTableList->mHeadList );
    
    STL_RING_FOREACH_ENTRY( & aForeignKeyList->mHeadList, sKeyItem )
    {
        sKeyHandle = & sKeyItem->mObjectHandle;

        sTableHandle = & sTableItem->mObjectHandle;

        /**
         * Parent Table 에서 Child Foreign Key 를 Un-link
         */
        
        STL_TRY( ellUnlinkChildForeignKeyFromParentTable( aTransID,
                                                 aStmt,
                                                 sTableHandle,
                                                 sKeyHandle,
                                                 ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        sTableItem = STL_RING_GET_NEXT_DATA( & aParentTableList->mHeadList, sTableItem );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Check Clause List 삭제에 대한 Cache 를 재구성한다.
 * @param[in] aTransID           Transaction ID
 * @param[in] aStmt              Statement
 * @param[in] aConstList         Check Constraint List
 * @param[in] aConstTableList    Constraint Table List
 * @param[in] aEnv               Environment
 * @remarks
 */
stlStatus qlrRefineCache4DropCheckClauseList( smlTransId         aTransID,
                                              smlStatement     * aStmt,
                                              ellObjectList    * aConstList,
                                              ellObjectList    * aConstTableList,
                                              qllEnv           * aEnv )
{
    ellObjectItem   * sConstItem   = NULL;
    ellDictHandle   * sConstHandle = NULL;

    ellObjectItem   * sTableItem   = NULL;
    ellDictHandle   * sTableHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstTableList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Constraint List 를 순회하며, Cache 를 Refine 한다.
     */
    
    sTableItem = STL_RING_GET_FIRST_DATA( & aConstTableList->mHeadList );
    
    STL_RING_FOREACH_ENTRY( & aConstList->mHeadList, sConstItem )
    {
        sConstHandle = & sConstItem->mObjectHandle;

        sTableHandle = & sTableItem->mObjectHandle;

        /**
         * Drop Constraint 를 위한 Refine Cache
         */
        
        STL_TRY( ellRefineCache4DropConstraint( aTransID,
                                                aStmt,
                                                sTableHandle,
                                                sConstHandle,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        sTableItem = STL_RING_GET_NEXT_DATA( & aConstTableList->mHeadList, sTableItem );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Not Null List 삭제에 대한 Cache 를 재구성한다.
 * @param[in] aTransID           Transaction ID
 * @param[in] aStmt              Statement
 * @param[in] aConstList         Check Constraint List
 * @param[in] aConstTableList    Constraint Table List
 * @param[in] aEnv               Environment
 * @remarks
 */
stlStatus qlrRefineCache4DropNotNullList( smlTransId         aTransID,
                                          smlStatement     * aStmt,
                                          ellObjectList    * aConstList,
                                          ellObjectList    * aConstTableList,
                                          qllEnv           * aEnv )
{
    ellObjectList * sDistinctTableList = NULL;

    ellObjectItem   * sTableItem   = NULL;
    ellDictHandle   * sTableHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstTableList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Not Null Constraint List 의 Table 목록의 중복 제거
     */
    
    STL_TRY( ellInitObjectList( & sDistinctTableList,
                                & QLL_EXEC_DDL_MEM(aEnv),
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_RING_FOREACH_ENTRY( & aConstTableList->mHeadList, sTableItem )
    {
        sTableHandle = & sTableItem->mObjectHandle;
        
        STL_TRY( ellAddNewObjectItem( sDistinctTableList,
                                      sTableHandle,
                                      NULL, /* aDuplicate */
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**
     * Not Null Constraint List 에 대해 refine cache 한다.
     */
    
    STL_TRY( ellRefineCache4DropNotNullList( aTransID,
                                             aStmt,
                                             aConstList,
                                             aConstTableList,
                                             sDistinctTableList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/** @} qlrAlterTableDropConst */
