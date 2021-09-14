/*******************************************************************************
 * qlrAlterTableDropColumn.c
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
 * @file qlrAlterTableDropColumn.c
 * @brief ALTER TABLE .. DROP COLUMN .. 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>



/**
 * @defgroup qlrAlterTableDropColumn ALTER TABLE .. DROP COLUMN
 * @ingroup qlrAlterTable
 * @{
 */


/**
 * @brief DROP COLUMN 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateDropColumn( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv )
{
    stlInt32  sState = 0;
    
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitDropColumn       * sInitPlan = NULL;
    qlpAlterTableDropColumn * sParseTree = NULL;

    qlpListElement * sListElement = NULL;
    qlpValue       * sColNameValue = NULL;
    stlChar        * sColName = NULL;
    ellDictHandle    sColHandle;
    stlInt32         sColPos = 0;
    stlBool          sObjectExist = STL_FALSE;

    smlStatement  * sStmt = NULL;
    stlInt32        sStmtAttr = 0;
    
    ellObjectList * sUniqueKeyList = NULL;
    ellObjectList * sForeignKeyList = NULL;
    ellObjectList * sChildForeignKeyList = NULL;
    ellObjectList * sCheckConstList = NULL;
    ellObjectList * sNonKeyIndexList = NULL;
    ellObjectList * sSetNullColumnList = NULL;
    

    stlInt32 i = 0;
    void          * sTableHandle = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_SET_UNUSED_COLUMN_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_DROP_COLUMN_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_DROP_UNUSED_COLUMNS_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseTree->mType == QLL_STMT_ALTER_TABLE_SET_UNUSED_COLUMN_TYPE) ||
                        (aParseTree->mType == QLL_STMT_ALTER_TABLE_DROP_COLUMN_TYPE) ||
                        (aParseTree->mType == QLL_STMT_ALTER_TABLE_DROP_UNUSED_COLUMNS_TYPE),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sParseTree = (qlpAlterTableDropColumn *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitDropColumn),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitDropColumn) );

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
     * Column Name Validation
     **********************************************************/

    /**
     * Unused / Drop Column 공간 확보
     */

    sInitPlan->mTableColCnt = ellGetTableColumnCnt( & sInitPlan->mTableHandle );
    sInitPlan->mTableColHandle = ellGetTableColumnDictHandle( & sInitPlan->mTableHandle );

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(stlBool) * sInitPlan->mTableColCnt,
                                (void **) & sInitPlan->mUnusedFlag,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(stlBool) * sInitPlan->mTableColCnt,
                                (void **) & sInitPlan->mDropFlag,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Unused Column 정보 설정
     */
    
    for ( i = 0; i < sInitPlan->mTableColCnt; i++ )
    {
        if ( ellGetColumnUnused( & sInitPlan->mTableColHandle[i] ) == STL_TRUE )
        {
            sInitPlan->mUnusedFlag[i] = STL_TRUE;
        }
        else
        {
            sInitPlan->mUnusedFlag[i] = STL_FALSE;
        }

        sInitPlan->mDropFlag[i] = STL_FALSE;
    }

    /**
     * Drop Column 정보 설정
     */

    STL_TRY( ellInitObjectList( & sInitPlan->mColumnList,
                                QLL_INIT_PLAN(aDBCStmt),
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    sInitPlan->mDropAction = sParseTree->mDropAction;
    switch( sInitPlan->mDropAction )
    {
        case QLP_DROP_COLUMN_ACTION_SET_UNUSED_COLUMN:
        case QLP_DROP_COLUMN_ACTION_DROP_COLUMN:
            {
                /**
                 * 나열된 컬럼을 Drop Column 목록에 등록
                 */
                
                QLP_LIST_FOR_EACH( sParseTree->mColumnNameList, sListElement )
                {
                    sColNameValue = (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sListElement );
                    sColName = QLP_GET_PTR_VALUE( sColNameValue );
                    
                    /**
                     * 존재하는 컬럼인지 검사
                     */
                    
                    STL_TRY( ellGetColumnDictHandle( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     & sInitPlan->mTableHandle,
                                                     sColName,
                                                     & sColHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_COLUMN_NOT_EXISTS );
        
                    /**
                     * 중복된 컬럼인지 검사 
                     */

                    sColPos = ellGetColumnIdx( & sColHandle );

                    STL_TRY_THROW( sInitPlan->mDropFlag[sColPos] == STL_FALSE,
                                   RAMP_ERR_SAME_COLUMN_EXIST );

                    sInitPlan->mDropFlag[sColPos] = STL_TRUE;

                   /**
                     * Drop Column List 에 추가
                     */

                    STL_TRY( ellAddNewObjectItem( sInitPlan->mColumnList,
                                                  & sColHandle,
                                                  NULL,
                                                  QLL_INIT_PLAN(aDBCStmt),
                                                  ELL_ENV(aEnv) )
                             == STL_SUCCESS );                    
                }
                
                break;
            }
        case QLP_DROP_COLUMN_ACTION_DROP_UNUSED_COLUMNS:
            {
                /**
                 * Unused Column 을 Drop Column 으로 구성
                 */

                stlMemcpy( sInitPlan->mDropFlag,
                           sInitPlan->mUnusedFlag,
                           STL_SIZEOF(stlBool) * STL_SIZEOF(stlBool) * sInitPlan->mTableColCnt );
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /**
     * 테이블에 unused/drop column 이외의 유효한 컬럼이 존재 있는지 검사
     */

    sObjectExist = STL_FALSE;

    for ( i = 0; i < sInitPlan->mTableColCnt; i++ )
    {
        if ( (sInitPlan->mDropFlag[i] == STL_FALSE) && (sInitPlan->mUnusedFlag[i] == STL_FALSE) )
        {
            sObjectExist = STL_TRUE;
            break;
        }
        else
        {
            continue;
        }
    }

    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_DROP_ALL_COLUMNS );

    /**********************************************************
     * CASCADE CONSTRAINTS Validation
     **********************************************************/

    /**
     * 관련 정보 획득
     */
    
    switch( sInitPlan->mDropAction )
    {
        case QLP_DROP_COLUMN_ACTION_SET_UNUSED_COLUMN:
        case QLP_DROP_COLUMN_ACTION_DROP_COLUMN:
            {
                STL_TRY( ellGetObjectList4DropColumn( aTransID,
                                                      sStmt,
                                                      sInitPlan->mColumnList,
                                                      QLL_INIT_PLAN(aDBCStmt),
                                                      & sUniqueKeyList,
                                                      & sForeignKeyList,
                                                      & sChildForeignKeyList,
                                                      & sCheckConstList,
                                                      & sNonKeyIndexList,
                                                      & sSetNullColumnList,
                                                      ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                break;
            }
        case QLP_DROP_COLUMN_ACTION_DROP_UNUSED_COLUMNS:
            {
                /**
                 * UNUSED Column 은 관련객체가 존재하지 않음
                 */

                STL_TRY( ellInitObjectList( & sChildForeignKeyList,
                                            QLL_INIT_PLAN(aDBCStmt),
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
     * Child Foreign Key 존재 여부 검사
     */
    
    sInitPlan->mIsCascade = sParseTree->mIsCascade;

    if ( ellHasObjectItem( sChildForeignKeyList ) == STL_TRUE )
    {
        /**
         * Child Foreign Key 가 존재할 경우, CASCADE CONSTRAINTS 가 있어야 함.
         */

        STL_TRY_THROW( sInitPlan->mIsCascade == STL_TRUE, RAMP_ERR_CAUSED_BY_FOREIGN_KEY );
    }
    else
    {
        /**
         * No Problem
         */
    }
    
    /**********************************************************
     * Not Implemented Feature
     **********************************************************/
    
    switch( sParseTree->mDropAction )
    {
        case QLP_DROP_COLUMN_ACTION_SET_UNUSED_COLUMN:
            break;
        case QLP_DROP_COLUMN_ACTION_DROP_COLUMN:
        case QLP_DROP_COLUMN_ACTION_DROP_UNUSED_COLUMNS:
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED_DROP_ACTION );
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /**********************************************************
     * Columnar Table Validation
     **********************************************************/

    sTableHandle = ellGetTableHandle( & sInitPlan->mTableHandle );

    STL_TRY_THROW( smlIsAlterableTable( sTableHandle ) == STL_TRUE,
                   RAMP_ERR_NOT_IMPLEMENTED );

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
     * Statement 해제
     */

    sState = 0;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV(aEnv) ) == STL_SUCCESS );
    sStmt = NULL;
    
    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sColNameValue->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sColName );
    }

    STL_CATCH( RAMP_ERR_SAME_COLUMN_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_COLUMN_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sColNameValue->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_DROP_ALL_COLUMNS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_DROP_ALL_COLUMNS_IN_TABLE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_CAUSED_BY_FOREIGN_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_KEY_REFERENCED_BY_FOREIGN_KEY,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED_DROP_ACTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mDropActionPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "qlrValidateDropColumn()" );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "ALTER TABLE DROP COLUMN" );
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
 * @brief DROP COLUMN 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeDropColumn( smlTransId      aTransID,
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
 * @brief DROP COLUMN 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataDropColumn( smlTransId      aTransID,
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
 * @brief DROP COLUMN 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteDropColumn( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv )
{
    qlrInitDropColumn * sInitPlan = NULL;

    ellObjectList * sUniqueKeyList = NULL;
    ellObjectList * sForeignKeyList = NULL;
    ellObjectList * sChildForeignKeyList = NULL;
    ellObjectList * sCheckConstList = NULL;
    ellObjectList * sNonKeyIndexList = NULL;
    ellObjectList * sSetNullColumnList = NULL;

    ellObjectList * sParentTableList = NULL;
    ellObjectList * sChildTableList = NULL;

    ellObjectList * sUniqueKeyIndexList = NULL;
    ellObjectList * sForeignKeyIndexList = NULL;
    ellObjectList * sChildForeignKeyIndexList = NULL;

    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;
    void            * sIdentityHandle = NULL;

    ellObjectList * sAffectedViewList = NULL;

    stlInt32  sLogOrdinalPos = 0;
    stlInt32  sPhyOrdinalPos = 0;
    
    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_SET_UNUSED_COLUMN_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_DROP_COLUMN_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_DROP_UNUSED_COLUMNS_TYPE),
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

    sInitPlan = (qlrInitDropColumn *) aSQLStmt->mInitPlan;
    
    /**
     * DDL Lock 획득 
     */

    STL_TRY( ellLock4DropColumn( aTransID,
                                 aStmt,
                                 & sInitPlan->mTableHandle,
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

    sInitPlan = (qlrInitDropColumn *) aSQLStmt->mInitPlan;

    /***************************************************************
     *  관련 객체 정보 획득
     ***************************************************************/
    
    /**
     * DROP COLUMN 관련 객체 획득
     */

    switch( sInitPlan->mDropAction )
    {
        case QLP_DROP_COLUMN_ACTION_SET_UNUSED_COLUMN:
        case QLP_DROP_COLUMN_ACTION_DROP_COLUMN:
            {
                STL_TRY( ellGetObjectList4DropColumn( aTransID,
                                                      aStmt,
                                                      sInitPlan->mColumnList,
                                                      & QLL_EXEC_DDL_MEM(aEnv),
                                                      & sUniqueKeyList,
                                                      & sForeignKeyList,
                                                      & sChildForeignKeyList,
                                                      & sCheckConstList,
                                                      & sNonKeyIndexList,
                                                      & sSetNullColumnList,
                                                      ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case QLP_DROP_COLUMN_ACTION_DROP_UNUSED_COLUMNS:
            {
                /**
                 * UNUSED Column 은 관련객체가 존재하지 않음
                 */

                STL_TRY( ellInitObjectList( & sUniqueKeyList,
                                            & QLL_EXEC_DDL_MEM(aEnv),
                                            ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_TRY( ellInitObjectList( & sForeignKeyList,
                                            & QLL_EXEC_DDL_MEM(aEnv),
                                            ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                STL_TRY( ellInitObjectList( & sChildForeignKeyList,
                                            & QLL_EXEC_DDL_MEM(aEnv),
                                            ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_TRY( ellInitObjectList( & sCheckConstList,
                                            & QLL_EXEC_DDL_MEM(aEnv),
                                            ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                STL_TRY( ellInitObjectList( & sNonKeyIndexList,
                                            & QLL_EXEC_DDL_MEM(aEnv),
                                            ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_TRY( ellInitObjectList( & sNonKeyIndexList,
                                            & QLL_EXEC_DDL_MEM(aEnv),
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
    
    /**
     * Foreign Key List 로부터 Parent Table List 에 대해 S Lock 을 획득 
     */
    
    STL_TRY( ellGetTableList4ConstList( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sForeignKeyList,
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
                                        sForeignKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sParentTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Child Foreign Key List 로부터 Child Table List 에 대해 S Lock 을 획득 
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

    /***************************************************************
     * 물리적 객체 제거 
     ***************************************************************/

    /**
     * Unique Key List 로부터 Index List 를 획득하여 제거 
     */
    
    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sUniqueKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sUniqueKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sUniqueKeyIndexList,
                                       aEnv )
             == STL_SUCCESS );

    
    /**
     * Foreign Key List 로부터 Index List 를 획득하여 제거 
     */
    
    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sForeignKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sForeignKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sForeignKeyIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Child Foreign Key List 로부터 Index List 를 획득하여 제거 
     */
    
    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sChildForeignKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sChildForeignKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sChildForeignKeyIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Child Foreign Key List 로부터 Index List 를 획득하여 제거 
     */
    
    STL_TRY( ellGetIndexList4KeyList( aTransID,
                                      sChildForeignKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sChildForeignKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sChildForeignKeyIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Non-Key Index List 를 제거 
     */
    
    STL_TRY( qliDropIndexes4IndexList( aStmt,
                                       sNonKeyIndexList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Identity Column 일 경우 Sequence 객체를 제거
     */

    if( (sInitPlan->mDropAction == QLP_DROP_COLUMN_ACTION_SET_UNUSED_COLUMN) ||
        (sInitPlan->mDropAction == QLP_DROP_COLUMN_ACTION_DROP_COLUMN) )
    {
        STL_RING_FOREACH_ENTRY( & sInitPlan->mColumnList->mHeadList, sObjectItem )
        {
            sObjectHandle = & sObjectItem->mObjectHandle;
            
            sIdentityHandle = ellGetColumnIdentityHandle( sObjectHandle );
            
            if ( sIdentityHandle != NULL )
            {
                STL_TRY( smlDropSequence( aStmt,
                                          sIdentityHandle,
                                          SML_ENV(aEnv) )
                         == STL_SUCCESS );
            }
            else
            {
                /**
                 * identity column 이 아님
                 */ 
            }
        }
    }
    else
    {
        /**
         * UNUSED Column 임
         */
        
        STL_ASSERT(sInitPlan->mDropAction == QLP_DROP_COLUMN_ACTION_DROP_UNUSED_COLUMNS);
    }

    /***************************************************************
     * Dictionary Record 제거 
     ***************************************************************/

    /**
     * Unique Key List 의 Dictionary Record 제거 
     */

    STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                  aStmt,
                                                  sUniqueKeyList,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                             aStmt,
                                             sUniqueKeyIndexList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                                  
    /**
     * Foreign Key List 의 Dictionary Record 제거 
     */

    STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                  aStmt,
                                                  sForeignKeyList,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                             aStmt,
                                             sForeignKeyIndexList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                                  
    /**
     * Child Foreign Key List 의 Dictionary Record 제거 
     */
    
    STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                  aStmt,
                                                  sChildForeignKeyList,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                             aStmt,
                                             sChildForeignKeyIndexList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                                  
    
    /**
     * Check Constraint List 의 Dictionary Record 제거 
     */
    
    STL_TRY( ellDeleteDictConstraintByObjectList( aTransID,
                                                  aStmt,
                                                  sCheckConstList,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Non-Key Index List 의 Dictionary Record 제거 
     */
    
    STL_TRY( ellDeleteDictIndexByObjectList( aTransID,
                                             aStmt,
                                             sNonKeyIndexList,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Column 의 Dictionary Record 제거
     */

    switch( sInitPlan->mDropAction )
    {
        case QLP_DROP_COLUMN_ACTION_SET_UNUSED_COLUMN:
            {
                sLogOrdinalPos = 0;
                
                for ( sPhyOrdinalPos = 0; sPhyOrdinalPos < sInitPlan->mTableColCnt; sPhyOrdinalPos++ )
                {
                    if ( sInitPlan->mDropFlag[sPhyOrdinalPos] == STL_TRUE )
                    {
                        /**
                         * Column 을 UNUSED 로 설정함
                         */

                        STL_TRY( ellModifyColumnSetUnused( aTransID,
                                                           aStmt,
                                                           & sInitPlan->mTableColHandle[sPhyOrdinalPos],
                                                           ELL_ENV(aEnv) )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        if ( sInitPlan->mUnusedFlag[sPhyOrdinalPos] == STL_TRUE )
                        {
                            /**
                             * Unused Column 임
                             * - nothing to do
                             */
                        }
                        else
                        {
                            /**
                             * USED Column 임
                             */

                            /**
                             * Logical Ordinal Position 변경
                             */

                            if ( ellObjectExistInObjectList(
                                     ELL_OBJECT_COLUMN,
                                     ellGetColumnID( & sInitPlan->mTableColHandle[sPhyOrdinalPos] ),
                                     sSetNullColumnList )
                                 == STL_TRUE )
                            {
                                /**
                                 * NULLABLE column 으로 변경
                                 */
                                STL_TRY( ellModifyColumnLogicalIdx( aTransID,
                                                                    aStmt,
                                                                    & sInitPlan->mTableColHandle[sPhyOrdinalPos],
                                                                    sLogOrdinalPos,
                                                                    STL_TRUE, /* aSetNullable */
                                                                    ELL_ENV(aEnv) )
                                         == STL_SUCCESS );
                            }
                            else
                            {
                                /**
                                 * NULLABLE 정보를 그대로 유지
                                 */
                                
                                STL_TRY( ellModifyColumnLogicalIdx( aTransID,
                                                                    aStmt,
                                                                    & sInitPlan->mTableColHandle[sPhyOrdinalPos],
                                                                    sLogOrdinalPos,
                                                                    STL_FALSE, /* aSetNullable */
                                                                    ELL_ENV(aEnv) )
                                         == STL_SUCCESS );
                            }

                            /**
                             * Logical Ordinal Position 증가
                             */
                            
                            sLogOrdinalPos++;
                        }
                    }   
                }
                
                break;
            }
        case QLP_DROP_COLUMN_ACTION_DROP_COLUMN:
        case QLP_DROP_COLUMN_ACTION_DROP_UNUSED_COLUMNS:
            {
                /**
                 * @todo 제거되지 않는 Key, Index 의 Column Order 를 변경해야 함.
                 */
                
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /**
     * Affected View List 에 대해 IS_AFFECTED 를 TRUE 로 변경
     */

    STL_TRY( ellSetAffectedViewList( aTransID,
                                     aStmt,
                                     sAffectedViewList,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Table 구조가 변경된 시간을 설정
     */
    
    STL_TRY( ellSetTime4ModifyTableElement( aTransID,
                                            aStmt,
                                            & sInitPlan->mTableHandle,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**********************************************************
     * Dictionary Cache 재구성 
     **********************************************************/

    /**
     * Child Foreign Key List 에 대한 Refine Cache
     */
    
    STL_TRY( qlrRefineCache4DropKeyList( aTransID,
                                         aStmt,
                                         sChildForeignKeyList,
                                         STL_FALSE,  /* aKeepIndex */
                                         sChildForeignKeyIndexList,
                                         sChildTableList,
                                         aEnv )
             == STL_SUCCESS );

    /**
     * Foreign Key List 에 대한 Refine Cache
     */
    
    STL_TRY( qlrRefineParentList4DropForeignKeyList( aTransID,
                                                     aStmt,
                                                     sForeignKeyList,
                                                     sParentTableList,
                                                     aEnv )
             == STL_SUCCESS );
    
    
    /**
     * Table Cache 재구축
     */
    
    STL_TRY( ellRefineCache4DropColumn( aTransID,
                                        aStmt,
                                        & sInitPlan->mTableHandle,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlrExecuteDropColumn()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}




/** @} qlrAlterTableDropColumn */
