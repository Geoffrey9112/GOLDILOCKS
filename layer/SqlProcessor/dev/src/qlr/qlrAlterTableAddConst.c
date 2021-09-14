/*******************************************************************************
 * qlrAlterTableAddConst.c
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
 * @file qlrAlterTableAddConst.c
 * @brief ALTER TABLE .. ADD CONSTRAINT .. 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlrAlterTableAddConst ALTER TABLE .. ADD CONSTRAINT
 * @ingroup qlrAlterTable
 * @{
 */


/**
 * @brief ADD CONSTRAINT 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateAddConst( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               stlChar       * aSQLString,
                               qllNode       * aParseTree,
                               qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitAddConst            * sInitPlan = NULL;
    qlpAlterTableAddConstraint * sParseTree = NULL;

    qlpConstraint * sParseConst = NULL;
    qlrInitConst  * sInitConst = NULL;

    qlpListElement  * sListElement = NULL;
    qlpIndexElement * sKeyElement = NULL;
    stlChar         * sColumnName = NULL;
    stlInt64          sBigintValue = 0;

    stlBool          sObjectExist = STL_FALSE;

    stlInt32        sPrimaryKeyCnt = 0;
    ellDictHandle * sPrimaryHandle = NULL;

    stlInt32        sUniqueKeyCnt = 0;
    ellDictHandle * sUniqueHandle = NULL;

    stlInt32        sForeignKeyCnt = 0;
    ellDictHandle * sForeignHandle = NULL;

    ellDictHandle   * sIndexHandle = NULL;
    ellIndexKeyDesc * sKeyDesc = NULL;
    stlBool           sSameKey = STL_FALSE;

    dtlDataType   sColumnType = DTL_TYPE_NA;
    
    stlInt32         i = 0;
    stlInt32         j = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ADD_CONSTRAINT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_TABLE_ADD_CONSTRAINT_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sParseTree = (qlpAlterTableAddConstraint *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitAddConst),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitAddConst) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
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
     * Table 내 Constraint 정보 획득 
     **********************************************************/

    sInitConst  = & sInitPlan->mConstDef;
    sParseConst = (qlpConstraint *)
        QLP_LIST_GET_POINTER_VALUE( QLP_LIST_GET_FIRST( sParseTree->mConstDefinition ) );
    
    switch ( sParseConst->mConstType )
    {
        case QLP_CONSTRAINT_CHECK:
            {
                /**
                 * @todo Check Clause 를 구현해야 함
                 */
                
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case QLP_CONSTRAINT_PRIMARY:
        case QLP_CONSTRAINT_UNIQUE:
        case QLP_CONSTRAINT_FOREIGN:
            {
                /**
                 * 테이블 내 Key Constraint 정보를 획득
                 */

                STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                                          aSQLStmt->mViewSCN,
                                                          QLL_INIT_PLAN(aDBCStmt),
                                                          & sInitPlan->mTableHandle,
                                                          & sPrimaryKeyCnt,
                                                          & sPrimaryHandle,
                                                          ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                STL_TRY( ellGetTableUniqueKeyDictHandle( aTransID,
                                                         aSQLStmt->mViewSCN,
                                                         QLL_INIT_PLAN(aDBCStmt),
                                                         & sInitPlan->mTableHandle,
                                                         & sUniqueKeyCnt,
                                                         & sUniqueHandle,
                                                         ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                STL_TRY( ellGetTableForeignKeyDictHandle( aTransID,
                                                          aSQLStmt->mViewSCN,
                                                          QLL_INIT_PLAN(aDBCStmt),
                                                          & sInitPlan->mTableHandle,
                                                          & sForeignKeyCnt,
                                                          & sForeignHandle,
                                                          ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                break;
            }
        default:
            {
                /**
                 * NOT NULL constraint 나 NULL constraint 가
                 * - ADD CONSTRAINT 구문으로 올 수 없음.
                 * - ALTER COLUMN 으로 제어됨 
                 */
                STL_ASSERT( 0 );
                break;
            }
    }
            
            
    /**********************************************************
     * Constraint Type Validation
     **********************************************************/

    /**
     * Constraint Type 설정 
     */

    switch ( sParseConst->mConstType )
    {
        case QLP_CONSTRAINT_CHECK:
            {
                /**
                 * @todo Check Clause 를 구현해야 함
                 */
                
                sInitConst->mConstType = ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE;
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case QLP_CONSTRAINT_PRIMARY:
            {
                /**
                 * Table 에 Primary Key 가 존재하는지 확인
                 */

                STL_TRY_THROW( sPrimaryKeyCnt == 0, RAMP_ERR_MULTIPLE_PRIMARY_KEY );
                
                sInitConst->mConstType = ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY;
                break;
            }
        case QLP_CONSTRAINT_UNIQUE:
            {
                sInitConst->mConstType = ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY;
                break;
            }
        case QLP_CONSTRAINT_FOREIGN:
            {
                /**
                 * @todo Foreign Key 를 구현해야 함
                 */
                
                sInitConst->mConstType = ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY;
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        default:
            {
                STL_ASSERT( 0 );
                break;
            }
    }

    /**
     * Constraint Characteristics 설정
     */
    
    STL_TRY( qlrSetConstCharacteristics( aSQLString,
                                         sParseConst,
                                         sInitConst,
                                         aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Constraint 대상 컬럼 Validation
     **********************************************************/

    /**
     * mColumnCnt
     * mColumnIdx
     */

    switch ( sParseConst->mConstType )
    {
        case QLP_CONSTRAINT_PRIMARY:
        case QLP_CONSTRAINT_UNIQUE:
            {
                sInitConst->mColumnCnt = 0;
    
                QLP_LIST_FOR_EACH( sParseConst->mUniqueFields, sListElement )
                {
                    /**
                     * Key Column 의 최대 개수를 초과하는 지 검사  
                     */
        
                    sKeyElement = (qlpIndexElement *) QLP_LIST_GET_POINTER_VALUE( sListElement );
                    sColumnName = QLP_GET_PTR_VALUE(sKeyElement->mColumnName);
        
                    STL_TRY_THROW( sInitConst->mColumnCnt < DTL_INDEX_COLUMN_MAX_COUNT,
                                   RAMP_ERR_MAXIMUM_COLUMN_EXCEEDED );

                    /**
                     * 이미 존재하는 컬럼인지 검사
                     */

                    STL_TRY( ellGetColumnDictHandle( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     & sInitPlan->mTableHandle,
                                                     sColumnName,
                                                     & sInitConst->mColumnHandle[sInitConst->mColumnCnt],
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_COLUMN_NOT_EXISTS );

                    /**
                     * Key Compare 가 가능한 Type 인지 검사
                     */

                    sColumnType = ellGetColumnDBType( & sInitConst->mColumnHandle[sInitConst->mColumnCnt] );
                    STL_TRY_THROW( dtlCheckKeyCompareType( sColumnType ) == STL_TRUE,
                                   RAMP_ERR_INVALID_KEY_COMP_TYPE );
                    
                    /**
                     * 동일한 Column 이 존재하는 지 검사 
                     */

                    for ( i = 0; i < sInitConst->mColumnCnt; i++ )
                    {
                        STL_TRY_THROW( ellGetColumnID( & sInitConst->mColumnHandle[i] )
                                       != ellGetColumnID( & sInitConst->mColumnHandle[sInitConst->mColumnCnt] ),
                                       RAMP_ERR_DUPLICATE_COLUMN_NAME );
                    }

                    /**********************************************************
                     * 인덱스 Key Column 속성 설정 
                     **********************************************************/
                    
                    sInitConst->mIndexKeyFlag[sInitConst->mColumnCnt] = 0;
                    
                    /**
                     * Key Column Nullable 여부 
                     */
                    
                    if ( sInitConst->mConstType == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY )
                    {
                        sInitConst->mIndexKeyFlag[sInitConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_NULLABLE_FALSE;
                    }
                    else
                    {
                        sInitConst->mIndexKeyFlag[sInitConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE;
                    }
                    
                    /**
                     * Key Column Order (ASC/DESC)
                     */
                    
                    
                    if ( sKeyElement->mIsAsc == NULL )
                    {
                        sInitConst->mIndexKeyFlag[sInitConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_ORDER_ASC;
                    }
                    else
                    {
                        sBigintValue = QLP_GET_INT_VALUE(sKeyElement->mIsAsc);
                        
                        if ( sBigintValue == STL_TRUE )
                        {
                            sInitConst->mIndexKeyFlag[sInitConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_ORDER_ASC;
                        }
                        else
                        {
                            sInitConst->mIndexKeyFlag[sInitConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_ORDER_DESC;
                        }
                    }
                    
                    /**
                     * Key Nulls First/Last 
                     */
                    
                    if ( sKeyElement->mIsNullsFirst == NULL )
                    {
                        sInitConst->mIndexKeyFlag[sInitConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_NULL_ORDER_DEFAULT;
                    }
                    else
                    {
                        sBigintValue = QLP_GET_INT_VALUE(sKeyElement->mIsNullsFirst);
                
                        if ( sBigintValue == STL_TRUE )
                        {
                            sInitConst->mIndexKeyFlag[sInitConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST;
                        }
                        else
                        {
                            sInitConst->mIndexKeyFlag[sInitConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST;
                        }
                    }
                    
                    sInitConst->mColumnCnt++;
                }
                break;
            }
        case QLP_CONSTRAINT_FOREIGN:
            {
                /**
                 * @todo Foreign Key 를 구현해야 함
                 */
                
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case QLP_CONSTRAINT_CHECK:
            {
                /**
                 * @todo Check Constraint 를 구현해야 함
                 */
                
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        default:
            {
                STL_ASSERT( 0 );
                break;
            }
    }

    /**********************************************************
     * Constraint 유형별 Validation
     **********************************************************/

    switch ( sParseConst->mConstType )
    {
        case QLP_CONSTRAINT_PRIMARY:
            {
                /**
                 * Primary Key Name Validation
                 */
                
                STL_TRY( qlrValidateUniquePrimaryKey( aTransID,
                                                      aDBCStmt,
                                                      aSQLStmt,
                                                      STL_TRUE,   /* aIsPrimary */
                                                      aSQLString,
                                                      & sInitPlan->mSchemaHandle,
                                                      & sInitPlan->mTableHandle,
                                                      ellGetTableName( & sInitPlan->mTableHandle ),
                                                      NULL, /* aInitColumnList */
                                                      NULL, /* aAllConstraint */
                                                      sInitConst,
                                                      sParseConst,
                                                      aEnv )
                         == STL_SUCCESS );

                /**
                 * 동일한 Key Column 을 갖는 Unique Key Constraint 가 존재하는 지 확인
                 */
                
                for ( i = 0; i < sUniqueKeyCnt; i++ )
                {
                    sIndexHandle = ellGetConstraintIndexDictHandle( & sUniqueHandle[i] );
                    sKeyDesc = ellGetIndexKeyDesc( sIndexHandle );
                    
                    if ( ellGetIndexKeyCount( sIndexHandle ) == sInitConst->mColumnCnt )
                    {
                        sSameKey = STL_TRUE;
                        
                        for ( j = 0; j < sInitConst->mColumnCnt; j++ )
                        {
                            if ( ellGetColumnID( & sKeyDesc[j].mKeyColumnHandle )
                                 == ellGetColumnID( & sInitConst->mColumnHandle[j] ) )
                            {
                                /* 동일한 Key 임 */
                            }
                            else
                            {
                                sSameKey = STL_FALSE;
                                break;
                            }
                        }
                        
                        STL_TRY_THROW( sSameKey != STL_TRUE, RAMP_ERR_SAME_KEY_COLUMN_LIST );
                    }
                    else
                    {
                        /* 서로 다름 */
                    }
                }
                
                break;
            }
        case QLP_CONSTRAINT_UNIQUE:
            {
                /**
                 * Primary Key Name Validation
                 */
                
                STL_TRY( qlrValidateUniquePrimaryKey( aTransID,
                                                      aDBCStmt,
                                                      aSQLStmt,
                                                      STL_FALSE,   /* aIsPrimary */
                                                      aSQLString,
                                                      & sInitPlan->mSchemaHandle,
                                                      & sInitPlan->mTableHandle,
                                                      ellGetTableName( & sInitPlan->mTableHandle ),
                                                      NULL, /* aInitColumnList */
                                                      NULL, /* aAllConstraint */
                                                      sInitConst,
                                                      sParseConst,
                                                      aEnv )
                         == STL_SUCCESS );

                /**
                 * 동일한 Key Column 을 갖는 Primary Key Constraint 가 존재하는 지 확인
                 */

                for ( i = 0; i < sPrimaryKeyCnt; i++ )
                {
                    sIndexHandle = ellGetConstraintIndexDictHandle( & sPrimaryHandle[i] );
                    sKeyDesc = ellGetIndexKeyDesc( sIndexHandle );
                    
                    if ( ellGetIndexKeyCount( sIndexHandle ) == sInitConst->mColumnCnt )
                    {
                        sSameKey = STL_TRUE;
                        
                        for ( j = 0; j < sInitConst->mColumnCnt; j++ )
                        {
                            if ( ellGetColumnID( & sKeyDesc[j].mKeyColumnHandle )
                                 == ellGetColumnID( & sInitConst->mColumnHandle[j] ) )
                            {
                                /* 동일한 Key 임 */
                            }
                            else
                            {
                                sSameKey = STL_FALSE;
                                break;
                            }
                        }
                        
                        STL_TRY_THROW( sSameKey != STL_TRUE, RAMP_ERR_SAME_KEY_COLUMN_LIST );
                    }
                    else
                    {
                        /* 서로 다름 */
                    }
                }
                
                /**
                 * 동일한 Key Column 을 갖는 Unique Key Constraint 가 존재하는 지 확인
                 */

                for ( i = 0; i < sUniqueKeyCnt; i++ )
                {
                    sIndexHandle = ellGetConstraintIndexDictHandle( & sUniqueHandle[i] );
                    sKeyDesc = ellGetIndexKeyDesc( sIndexHandle );
                    
                    if ( ellGetIndexKeyCount( sIndexHandle ) == sInitConst->mColumnCnt )
                    {
                        sSameKey = STL_TRUE;
                        
                        for ( j = 0; j < sInitConst->mColumnCnt; j++ )
                        {
                            if ( ellGetColumnID( & sKeyDesc[j].mKeyColumnHandle )
                                 == ellGetColumnID( & sInitConst->mColumnHandle[j] ) )
                            {
                                /* 동일한 Key 임 */
                            }
                            else
                            {
                                sSameKey = STL_FALSE;
                                break;
                            }
                        }
                        
                        STL_TRY_THROW( sSameKey != STL_TRUE, RAMP_ERR_SAME_KEY_COLUMN_LIST );
                    }
                    else
                    {
                        /* 서로 다름 */
                    }
                }
                
                break;
            }
        case QLP_CONSTRAINT_FOREIGN:
            {
                /**
                 * @todo Foreign Key 를 구현해야 함
                 */
                
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case QLP_CONSTRAINT_CHECK:
            {
                /**
                 * @todo Check Constraint 를 구현해야 함
                 */
                
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        default:
            {
                STL_ASSERT( 0 );
                break;
            }
    }

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mTableHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mConstDef.mConstSchemaHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    switch( sInitPlan->mConstDef.mConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                              & sInitPlan->mConstDef.mIndexSpaceHandle,
                                              QLL_INIT_PLAN(aDBCStmt),
                                              ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                /**
                 * Index 가 없음
                 */
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
    
    
    /***********************************************************
     * Init Plan 연결 
     ***********************************************************/

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlrValidateAddConst()" );
    }

    STL_CATCH( RAMP_ERR_MULTIPLE_PRIMARY_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MULTIPLE_PRIMARY_KEY,
                      qlgMakeErrPosString( aSQLString,
                                           sParseConst->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }

    
    STL_CATCH( RAMP_ERR_MAXIMUM_COLUMN_EXCEEDED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MAXIMUM_COLUMN_EXCEEDED,
                      qlgMakeErrPosString( aSQLString,
                                           sKeyElement->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_COLUMN_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sKeyElement->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sColumnName );
    }

    STL_CATCH( RAMP_ERR_INVALID_KEY_COMP_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ILLEGAL_USE_OF_DATA_TYPE,
                      qlgMakeErrPosString( aSQLString,
                                           sKeyElement->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      dtlDataTypeName[ sColumnType ] );
    }

    STL_CATCH( RAMP_ERR_DUPLICATE_COLUMN_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_COLUMN_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sKeyElement->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SAME_KEY_COLUMN_LIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MULTIPLE_UNIQUE_KEY,
                      qlgMakeErrPosString( aSQLString,
                                           sParseConst->mConstTypePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ADD CONSTRAINT 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeAddConst( smlTransId      aTransID,
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
 * @brief ADD CONSTRAINT 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataAddConst( smlTransId      aTransID,
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
 * @brief ADD CONSTRAINT 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteAddConst( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitAddConst * sInitPlan = NULL;

    stlInt32          sPrimaryKeyCnt = 0;
    ellDictHandle   * sPrimaryHandle = NULL;

    stlInt64            sKeyColumnID[DTL_INDEX_COLUMN_MAX_COUNT] = {0,};
    knlValueBlockList * sKeyValueBlockList = NULL;

    ellIndexColumnOrdering      sKeyColOrder  = ELL_INDEX_COLUMN_ORDERING_DEFAULT;
    ellIndexColumnNullsOrdering sKeyNullOrder = ELL_INDEX_COLUMN_NULLS_ORDERING_DEFAULT;
    
    stlInt64   sIndexID = ELL_DICT_OBJECT_ID_NA;
    stlInt64   sIndexPhyID = 0;
    void     * sIndexPhyHandle = NULL;

    
    stlInt64   sConstID = ELL_DICT_OBJECT_ID_NA;
    stlInt32   sRefUniqueKeyColIdx = ELL_DICT_POSITION_NA;


    ellDictHandle sIndexHandle;
    stlBool       sObjectExist = STL_FALSE;
    stlBool       sSameKey = STL_FALSE;
    
    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;
    stlInt32 i = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ADD_CONSTRAINT_TYPE,
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
    
    sInitPlan = (qlrInitAddConst *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * DDL Lock 획득 
     */

    switch( sInitPlan->mConstDef.mConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                STL_TRY( ellLock4AddConst( aTransID,
                                           aStmt,
                                           sAuthHandle,
                                           & sInitPlan->mConstDef.mConstSchemaHandle,
                                           & sInitPlan->mConstDef.mIndexSpaceHandle,
                                           & sInitPlan->mTableHandle,
                                           sInitPlan->mConstDef.mConstType,
                                           & sLocked,
                                           ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                /**
                 * Index 가 없음
                 */

                STL_TRY( ellLock4AddConst( aTransID,
                                           aStmt,
                                           sAuthHandle,
                                           & sInitPlan->mConstDef.mConstSchemaHandle,
                                           NULL,
                                           & sInitPlan->mTableHandle,
                                           sInitPlan->mConstDef.mConstType,
                                           & sLocked,
                                           ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
    
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
    
    sInitPlan = (qlrInitAddConst *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /***************************************************************
     * Constraint 유형별 Validation
     ***************************************************************/
    
    /**
     * Constraint 유형별 Validation
     */

    switch (sInitPlan->mConstDef.mConstType)
    {
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                /**
                 * nothing to do 
                 */
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            {
                /**
                 * 테이블 내 Primary Key 가 존재하지 않아야 함
                 */

                STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                                          ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                          & QLL_EXEC_DDL_MEM(aEnv),
                                                          & sInitPlan->mTableHandle,
                                                          & sPrimaryKeyCnt,
                                                          & sPrimaryHandle,
                                                          ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_TRY_THROW( sPrimaryKeyCnt == 0, RAMP_ERR_MULTIPLE_PRIMARY_KEY );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                /**
                 * S lock 을 통해 동시 수행할 수 있어 Dictionary Integrity 를 통해 제어됨.
                 */
                
                break;
            }
        default:
            {
                /**
                 * NOT NULL constraint 나 NULL constraint 가
                 * - ADD CONSTRAINT 구문으로 올 수 없음.
                 * - ALTER COLUMN 으로 제어됨 
                 */
                STL_ASSERT( 0 );
                break;
            }
    }

    /**********************************************************
     * Key Constraint 일 경우 Key Index 생성
     **********************************************************/

    switch (sInitPlan->mConstDef.mConstType)
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                /*********************************************************
                 * Key Index 객체 생성 (Build 는 모든 과정이 완료된 후)
                 *********************************************************/
                
                /**
                 * Index Key Column ID 정보 구축
                 */
                
                for ( i = 0; i < sInitPlan->mConstDef.mColumnCnt; i++ )
                {
                    sKeyColumnID[i] = ellGetColumnID( & sInitPlan->mConstDef.mColumnHandle[i] );
                }

                /**
                 * Index Value List 공간 확보 
                 */
                
                STL_TRY( ellAllocIndexValueListForCREATE( aTransID,
                                                          ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                          sInitPlan->mConstDef.mColumnCnt,
                                                          sKeyColumnID,
                                                          NULL,   
                                                          NULL,
                                                          & QLL_EXEC_DDL_MEM(aEnv),
                                                          & sKeyValueBlockList,
                                                          ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                /**
                 * Index 객체 생성
                 */
                
                STL_TRY( smlCreateIndex( aStmt,
                                         ellGetTablespaceID( & sInitPlan->mConstDef.mIndexSpaceHandle ),
                                         SML_INDEX_TYPE_MEMORY_PERSISTENT_BTREE,
                                         & sInitPlan->mConstDef.mIndexAttr,
                                         ellGetTableHandle( & sInitPlan->mTableHandle ),
                                         sInitPlan->mConstDef.mColumnCnt,
                                         sKeyValueBlockList,
                                         sInitPlan->mConstDef.mIndexKeyFlag,
                                         & sIndexPhyID,
                                         & sIndexPhyHandle,
                                         SML_ENV(aEnv) )
                         == STL_SUCCESS );

                break;
            }
        default:
            {
                /**
                 * nothing to do
                 */
                break;
            }
    }
            
    /**********************************************************
     * Constraint 별 Dictionary 변경 
     **********************************************************/
    
    switch (sInitPlan->mConstDef.mConstType)
    {
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                /**
                 * @todo Check Constraint 를 구현해야 함
                 */
                
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                /*********************************************************
                 * Column 의 Nullable 정보 변경 
                 *********************************************************/

                if ( sInitPlan->mConstDef.mConstType == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY )
                {
                    /**
                     * Column 속성을 NOT NULL 로 변경 
                     */

                    for ( i = 0; i < sInitPlan->mConstDef.mColumnCnt; i++ )
                    {
                        STL_TRY( ellModifyColumnNullable( aTransID,
                                                          aStmt,
                                                          sKeyColumnID[i],
                                                          STL_FALSE,  /* aIsNullable */
                                                          ELL_ENV(aEnv) )
                                 == STL_SUCCESS );
                    }
                    
                }
                else
                {
                    /**
                     * nothing to do
                     */
                }
                
                /*********************************************************
                 * Key Index 에 대한 Dictionary 변경 
                 *********************************************************/
                
                /**
                 * Index Descriptor 추가
                 * - Key Index 는 Key Constraint 와 동일한 Schema 를 가져야 함.
                 */
                
                STL_TRY( ellInsertIndexDesc( aTransID,
                                             aStmt,
                                             ellGetAuthID( sAuthHandle ),
                                             ellGetSchemaID( & sInitPlan->mConstDef.mConstSchemaHandle ),
                                             & sIndexID,
                                             ellGetTablespaceID( & sInitPlan->mConstDef.mIndexSpaceHandle ),
                                             sIndexPhyID,
                                             sInitPlan->mConstDef.mIndexName,
                                             ELL_INDEX_TYPE_BTREE,
                                             sInitPlan->mConstDef.mIndexAttr.mUniquenessFlag,
                                             STL_FALSE,  /* aInvalid */
                                             STL_TRUE,   /* aByConstraint */
                                             NULL,       /* aIndexComment */
                                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                /**
                 * Index Key Table Usage 추가
                 */
                
                STL_TRY( ellInsertIndexKeyTableUsageDesc(
                             aTransID,
                             aStmt,
                             ellGetAuthID( sAuthHandle ),
                             ellGetSchemaID( & sInitPlan->mConstDef.mConstSchemaHandle ),
                             sIndexID,
                             ellGetTableOwnerID( & sInitPlan->mTableHandle ),
                             ellGetTableSchemaID( & sInitPlan->mTableHandle ),
                             ellGetTableID( & sInitPlan->mTableHandle ),
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                /**
                 * Index Key Column Usage 추가
                 */
                
                for ( i = 0; i < sInitPlan->mConstDef.mColumnCnt; i++ )
                {
                    if ( (sInitPlan->mConstDef.mIndexKeyFlag[i] & DTL_KEYCOLUMN_INDEX_ORDER_MASK)
                         == DTL_KEYCOLUMN_INDEX_ORDER_ASC )
                    {
                        sKeyColOrder = ELL_INDEX_COLUMN_ASCENDING;
                    }
                    else
                    {
                        sKeyColOrder = ELL_INDEX_COLUMN_DESCENDING;
                    }
                    
                    if ( (sInitPlan->mConstDef.mIndexKeyFlag[i] & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                         == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
                    {
                        sKeyNullOrder = ELL_INDEX_COLUMN_NULLS_FIRST;
                    }
                    else
                    {
                        sKeyNullOrder = ELL_INDEX_COLUMN_NULLS_LAST;
                    }
                    
                    STL_TRY( ellInsertIndexKeyColumnUsageDesc(
                                 aTransID,
                                 aStmt,
                                 ellGetAuthID( sAuthHandle ),
                                 ellGetSchemaID( & sInitPlan->mConstDef.mConstSchemaHandle ),
                                 sIndexID,
                                 ellGetTableOwnerID( & sInitPlan->mTableHandle ),
                                 ellGetTableSchemaID( & sInitPlan->mTableHandle ),
                                 ellGetTableID( & sInitPlan->mTableHandle ),
                                 sKeyColumnID[i],
                                 i,
                                 sKeyColOrder,
                                 sKeyNullOrder,
                                 ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                
                /*********************************************************
                 * Key Constraint 에 대한 Dictionary 변경 
                 *********************************************************/
                
                /**
                 * Table Constraint Descriptor 추가
                 */
                
                STL_TRY( ellInsertTableConstraintDesc(
                             aTransID,
                             aStmt,
                             ellGetAuthID( sAuthHandle ),
                             ellGetSchemaID( & sInitPlan->mConstDef.mConstSchemaHandle ),
                             & sConstID,
                             ellGetTableOwnerID( & sInitPlan->mTableHandle ),
                             ellGetTableSchemaID( & sInitPlan->mTableHandle ),
                             ellGetTableID( & sInitPlan->mTableHandle ),
                             sInitPlan->mConstDef.mConstName,
                             sInitPlan->mConstDef.mConstType,
                             sInitPlan->mConstDef.mDeferrable,
                             sInitPlan->mConstDef.mInitDeferred,
                             sInitPlan->mConstDef.mEnforced,
                             sIndexID,
                             NULL,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                /**
                 * Table Constraint 의 Key Column Usage Descriptor 추가
                 */
                
                for ( i = 0; i < sInitPlan->mConstDef.mColumnCnt; i++ )
                {
                    /**
                     * @todo Foreign Key 인 경우 해당 정보를 조정해야 함.
                     */
                    sRefUniqueKeyColIdx = ELL_DICT_POSITION_NA;
                    
                    STL_TRY( ellInsertKeyColumnUsageDesc(
                                 aTransID,
                                 aStmt,
                                 ellGetAuthID( sAuthHandle ),
                                 ellGetSchemaID( & sInitPlan->mConstDef.mConstSchemaHandle ),
                                 sConstID,
                                 ellGetTableOwnerID( & sInitPlan->mTableHandle ),
                                 ellGetTableSchemaID( & sInitPlan->mTableHandle ),
                                 ellGetTableID( & sInitPlan->mTableHandle ),
                                 sKeyColumnID[i],
                                 i,
                                 sRefUniqueKeyColIdx, 
                                 ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                break;
            }
        default:
            {
                STL_ASSERT( 0 );
                break;
            }
    }

    /**
     * Table 구조가 변경된 시간을 설정
     * - Table Constraint 의 변경을 Table 변경 시간에 적용할 경우,
     * - 동시 수행시 table descriptor 를 위한 dictionary record 변경이 함께 발생하므로,
     * - 충분히 동시에 수행할 수 있는 ADD/DROP UNIQUE KEY 등을 동시에 수행할 수 없게 된다.
     */

    switch( sInitPlan->mConstDef.mConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            STL_TRY( ellSetTime4ModifyTableElement( aTransID,
                                                    aStmt,
                                                    & sInitPlan->mTableHandle,
                                                    ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            /**
             * 동시에 수행할 수 있다.
             */
            break;
        default:
            STL_ASSERT(0);
            break;
    }
    
    /**********************************************************
     * Dictionary Cache Refinement
     **********************************************************/

    /**
     * Key Index 에 대한 Refine Cache
     */

    switch (sInitPlan->mConstDef.mConstType)
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            {
                /**
                 * Index 에 대한 Refine Cache
                 */
                
                STL_TRY( ellRefineCache4CreateIndex( aTransID,
                                                     aStmt,
                                                     ellGetAuthID( sAuthHandle ),
                                                     sIndexID,
                                                     ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                /**
                 * 동일한 Key Column 을 갖는 Key Index  존재하는지 검사
                 * Primary/Unique Key 영역에서 검사
                 */

                STL_TRY( ellGetIndexDictHandleByID( aTransID,
                                                    ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                    sIndexID,
                                                    & sIndexHandle,
                                                    & sObjectExist,
                                                    ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_ASSERT( sObjectExist == STL_TRUE );
                
                STL_TRY( ellRuntimeCheckSameKey4UniqueKey( aTransID,
                                                           & sInitPlan->mTableHandle,
                                                           & sIndexHandle,
                                                           & sSameKey,
                                                           ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                STL_TRY_THROW( sSameKey != STL_TRUE, RAMP_ERR_SAME_UNIQUE_KEY_COLUMN_LIST );
                
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            {
                /**
                 * Index 에 대한 Refine Cache
                 */
                
                STL_TRY( ellRefineCache4CreateIndex( aTransID,
                                                     aStmt,
                                                     ellGetAuthID( sAuthHandle ),
                                                     sIndexID,
                                                     ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                /**
                 * 동일한 Key Column 을 갖는 Key Index  존재하는지 검사
                 * Primary/Unique Key 영역에서 검사
                 */
                
                STL_TRY( ellGetIndexDictHandleByID( aTransID,
                                                    ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                    sIndexID,
                                                    & sIndexHandle,
                                                    & sObjectExist,
                                                    ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_ASSERT( sObjectExist == STL_TRUE );
                
                
                STL_TRY( ellRuntimeCheckSameKey4UniqueKey( aTransID,
                                                           & sInitPlan->mTableHandle,
                                                           & sIndexHandle,
                                                           & sSameKey,
                                                           ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                STL_TRY_THROW( sSameKey != STL_TRUE, RAMP_ERR_SAME_UNIQUE_KEY_COLUMN_LIST );

                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                /**
                 * Index 에 대한 Refine Cache
                 */
                
                STL_TRY( ellRefineCache4CreateIndex( aTransID,
                                                     aStmt,
                                                     ellGetAuthID( sAuthHandle ),
                                                     sIndexID,
                                                     ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                /**
                 * 동일한 Key Column 을 갖는 Key Index  존재하는지 검사
                 * Primary/Unique/Foreign Key 영역에서 검사
                 */
                
                STL_TRY( ellGetIndexDictHandleByID( aTransID,
                                                    ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                    sIndexID,
                                                    & sIndexHandle,
                                                    & sObjectExist,
                                                    ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_ASSERT( sObjectExist == STL_TRUE );
                
                STL_TRY( ellRuntimeCheckSameKey4ForeignKey( aTransID,
                                                            & sInitPlan->mTableHandle,
                                                            & sIndexHandle,
                                                            & sSameKey,
                                                            ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                /**
                 * @todo 동일한 Foreign Key 에 대한 error 설정
                 */
                /* STL_TRY_THROW( sSameKey != STL_TRUE, RAMP_ERR_SAME_FOREIGN_KEY_COLUMN_LIST ); */

                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                /**
                 * nothing to do
                 */
                break;
            }
        default:
            {
                STL_ASSERT( 0 );
                break;
            }
    }
                                           
    /**
     * Constraint 에 대한 Refine Cache
     */
    
    STL_TRY( ellRefineCache4AddConstraint( aTransID,
                                           aStmt,
                                           & sInitPlan->mTableHandle,
                                           sConstID,
                                           sInitPlan->mConstDef.mConstType,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );
                
    /**
     * 생성한 객체에 대해 Lock 을 획득
     * 
     * commit -> pending -> trans end (unlock) 과정 중에
     * 해당 객체에 대한 DROP, ALTER 가 수행될 경우를 방지하기 위해
     * 생성한 객체에 대한 lock 을 획득해야 함.
     */

    STL_TRY( ellLockRowAfterCreateNonBaseTableObject( aTransID,
                                                      aStmt,
                                                      ELL_OBJECT_CONSTRAINT,
                                                      sConstID,
                                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                           
    /**********************************************************
     * 모든 Run-Time Validation 이 완료되면 Index Build 
     **********************************************************/

    switch( sInitPlan->mConstDef.mConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                /**
                 * Key Index Build
                 */
                
                STL_TRY( smlBuildIndex( aStmt,
                                        sIndexPhyHandle,
                                        ellGetTableHandle( & sInitPlan->mTableHandle ),
                                        sInitPlan->mConstDef.mColumnCnt,
                                        sKeyValueBlockList,
                                        sInitPlan->mConstDef.mIndexKeyFlag,
                                        sInitPlan->mConstDef.mIndexParallel,
                                        SML_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                /**
                 * nothing to do
                 */
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlrExecuteAddConst()" );
    }

    STL_CATCH( RAMP_ERR_MULTIPLE_PRIMARY_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MULTIPLE_PRIMARY_KEY,
                      NULL,
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_SAME_UNIQUE_KEY_COLUMN_LIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MULTIPLE_UNIQUE_KEY,
                      NULL,
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}





/** @} qlrAlterTableAddConst */
