/*******************************************************************************
 * qlrCreateTable.c
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
 * @file qlrCreateTable.c
 * @brief CREATE TABLE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlrCreateTable CREATE TABLE
 * @ingroup qlr
 * @{
 */

/**
 * @brief CREATE TABLE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateCreateTable( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv )
{    
    qlpTableDef        * sParseTree = NULL;
    qlpListElement     * sListElement = NULL;
    qllNode            * sTableElement = NULL;
    qlpColumnDef       * sColumnDef = NULL;
    qlvInitExprList    * sQueryExprList = NULL;
    qlpList            * sConstraintList = NULL;
    qlrInitCreateTable * sInitPlan = NULL;
    qlrInitColumn      * sCurColumn;
    stlInt32             sColCount = 0;
    stlInt32             i;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_TABLE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_CREATE_TABLE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpTableDef *) aParseTree;

    /**
     * Init Plan 생성 및 공통 정보 설정
     */
    
    STL_TRY( qlrValidateCreateTableCommon( aTransID,
                                           aDBCStmt,
                                           aSQLStmt,
                                           aSQLString,
                                           sParseTree,
                                           & sInitPlan,
                                           & sQueryExprList,
                                           aEnv )
             == STL_SUCCESS );

    /**
     * Column Definition 추가
     */
    
    QLP_LIST_FOR_EACH( sParseTree->mTableElems, sListElement )
    {
        sTableElement = (qllNode *) QLP_LIST_GET_POINTER_VALUE( sListElement );

        switch ( sTableElement->mType )
        {
            case QLL_COLUMN_DEF_TYPE:
                {
                    sColumnDef = (qlpColumnDef *) sTableElement;
                    
                    STL_TRY( qlrAddColumnDefinition( aTransID,
                                                     aDBCStmt,
                                                     aSQLStmt,
                                                     aSQLString,
                                                     sQueryExprList,
                                                     & sInitPlan->mSchemaHandle,
                                                     NULL, /* table handle */
                                                     sInitPlan->mTableType,
                                                     sInitPlan->mTableName,
                                                     sColumnDef,
                                                     & sInitPlan->mHasIdentity,
                                                     & sInitPlan->mColumnList,
                                                     & sInitPlan->mAllConstraint,
                                                     aEnv )
                             == STL_SUCCESS );

                    sColCount++;
                    break;
                }
            case QLL_POINTER_LIST_TYPE: /* constraint를 pointer list로 가지고 있음 */
                {
                    /**
                     * 컬럼 정의를 먼저 수행한 후 Constraint 처리를 한다.
                     */
                    break;
                }
            default:
                STL_PARAM_VALIDATE( 1 == 0, QLL_ERROR_STACK(aEnv) );
                break;
        }
    }

    if( sInitPlan->mTableType == SML_MEMORY_PERSISTENT_HEAP_COLUMNAR_TABLE )
    {
        /**
         * Columnar Table은 하나의 page에 모든 column value들이 다 저장되어야 하며
         * 최소 두개 이상의 row를 저장할 수 있는 제약사항을 가지고 있다.
         * => continuous column 및 여러 개의 page에 나뉘어 저장하는 구조로 확장 시 수정
         */
        sInitPlan->mPhysicalAttr.mColumnarAttr.mMinRowCount = SML_COLUMNAR_TABLE_MIN_ROW_COUNT_MIN;
        sInitPlan->mPhysicalAttr.mColumnarAttr.mContColumnThreshold =
            SML_COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD_MAX;
        sInitPlan->mPhysicalAttr.mColumnarAttr.mColCount = sColCount;

        STL_TRY( knlAllocRegionMem(
                     QLL_INIT_PLAN(aDBCStmt),
                     STL_SIZEOF(stlInt16) * sColCount,
                     (void **) & (sInitPlan->mPhysicalAttr.mColumnarAttr.mColLengthArray),
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sCurColumn = sInitPlan->mColumnList;
        i = 0;

        while( sCurColumn != NULL )
        {
            sInitPlan->mPhysicalAttr.mColumnarAttr.mColLengthArray[i++] = sCurColumn->mColLength;

            sCurColumn = sCurColumn->mNext;
        }
    }

    /**
     * Table Constraint 추가
     */
    
    QLP_LIST_FOR_EACH( sParseTree->mTableElems, sListElement )
    {
        sTableElement = (qllNode *) QLP_LIST_GET_POINTER_VALUE( sListElement );

        switch ( sTableElement->mType )
        {
            case QLL_COLUMN_DEF_TYPE:
                {
                    /**
                     * Nothing To Do
                     */
                    break;
                }
            case QLL_POINTER_LIST_TYPE: /* constraint를 pointer list로 가지고 있음 */
                {
                    sConstraintList = (qlpList *) sTableElement;
                    
                    STL_TRY( qlrAddOutlineConstDefinition( aTransID,
                                                           aDBCStmt,
                                                           aSQLStmt,
                                                           aSQLString,
                                                           & sInitPlan->mSchemaHandle,
                                                           NULL, /* table handle */
                                                           sInitPlan->mTableName,
                                                           sInitPlan->mColumnList,
                                                           & sInitPlan->mAllConstraint,
                                                           sConstraintList,
                                                           aEnv )
                             == STL_SUCCESS );
                    break;
                }
            default:
                STL_PARAM_VALIDATE( 1 == 0, QLL_ERROR_STACK(aEnv) );
                break;
        }
    }
    
   

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectList( sInitPlan->mCommonInit.mValidationObjList,
                                  sInitPlan->mAllConstraint.mConstSchemaList,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellAppendObjectList( sInitPlan->mCommonInit.mValidationObjList,
                                  sInitPlan->mAllConstraint.mConstSpaceList,
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
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief CREATE TABLE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeCreateTable( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    /**
     * 할 일 없음
     */
    
    return STL_SUCCESS;
}


/**
 * @brief CREATE TABLE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataCreateTable( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    qlrInitCreateTable * sInitPlan = NULL;
    qlrDataCreateTable * sDataPlan = NULL;

    qlrInitColumn      * sCodeColumn = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aSQLStmt->mStmtType == QLL_STMT_CREATE_TABLE_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_CREATE_TABLE_AS_SELECT_TYPE ),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * Valid Plan 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_DATA_OPTIMIZE,
                                  aEnv )
             == STL_SUCCESS );
    
    sInitPlan = (qlrInitCreateTable *) aSQLStmt->mInitPlan;
    
    /**
     * Data Plan 생성
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrDataCreateTable),
                                (void **) & sDataPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sDataPlan, 0x00, STL_SIZEOF(qlrDataCreateTable) );

    /**
     * Table 의 Column Element 공간 확보
     */

    sDataPlan->mColumnCount = 0;
    
    for ( sCodeColumn = sInitPlan->mColumnList;
          sCodeColumn != NULL;
          sCodeColumn = sCodeColumn->mNext )
    {
        sDataPlan->mColumnCount++;
    }

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                STL_SIZEOF(stlInt64) * sDataPlan->mColumnCount,
                                (void **) & sDataPlan->mColumnID,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sDataPlan->mColumnID,
               0x00,
               STL_SIZEOF(stlInt64) * sDataPlan->mColumnCount );

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                STL_SIZEOF(ellAddColumnDesc) * sDataPlan->mColumnCount,
                                (void **) & sDataPlan->mAddColumnDesc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sDataPlan->mAddColumnDesc,
               0x00,
               STL_SIZEOF(ellAddColumnDesc) * sDataPlan->mColumnCount );
    
    /**
     * Data Plan 연결
     */

    aSQLStmt->mDataPlan = (void *) sDataPlan;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief CREATE TABLE 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrExecuteCreateTable( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    ellDictHandle      * sAuthHandle = NULL;
    qlrInitCreateTable * sInitPlan = NULL;

    stlTime   sTime = 0;
    stlBool   sLocked = STL_TRUE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_TABLE_TYPE,
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

    sInitPlan = (qlrInitCreateTable *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * CREATE TABLE 구문을 위한 DDL Lock 획득
     */

    STL_TRY( ellLock4CreateTable( aTransID,
                                  aStmt,
                                  sAuthHandle,
                                  & sInitPlan->mSchemaHandle,
                                  & sInitPlan->mSpaceHandle,
                                  sInitPlan->mAllConstraint.mConstSchemaList,
                                  sInitPlan->mAllConstraint.mConstSpaceList,
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

    /**
     * Create Table 공통 부분 수행 
     */
    
    STL_TRY( qlrExecuteCreateTableCommon( aTransID,
                                          aStmt,
                                          aDBCStmt,
                                          aSQLStmt,
                                          aEnv )
             == STL_SUCCESS );
    
                
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief CREATE TABLE과 CREATE TABLE AS SELECT 공통 Validation
 * @param[in] aTransID        Transaction ID
 * @param[in] aDBCStmt        DBC Statement
 * @param[in] aSQLStmt        SQL Statement
 * @param[in] aSQLString      SQL String 
 * @param[in] aParseTree      Parse Tree
 * @param[out] aInitPlan      Init Plan 
 * @param[out] aQueryExprList Query Expr List 
 * @param[in] aEnv            Environment
 * @remarks
 */
stlStatus qlrValidateCreateTableCommon( smlTransId            aTransID,
                                        qllDBCStmt          * aDBCStmt,
                                        qllStatement        * aSQLStmt,
                                        stlChar             * aSQLString,
                                        qlpTableDef         * aParseTree,
                                        qlrInitCreateTable ** aInitPlan,
                                        qlvInitExprList    ** aQueryExprList,
                                        qllEnv              * aEnv )
{
    ellDictHandle        * sAuthHandle = NULL;
    qlpObjectName        * sObjectName = NULL;
    qlrInitCreateTable   * sInitPlan = NULL;
    stlBool                sObjectExist = STL_FALSE;
    ellDictHandle          sObjectHandle;
    ellTableType           sObjectType = ELL_TABLE_TYPE_NA;
    qlvInitStmtExprList  * sStmtExprList  = NULL;
    qlvInitExprList      * sQueryExprList = NULL;
    stlInt64               sDefaultTableType;
    stlInt64               sBigintValue = 0;

    ellInitDictHandle( & sObjectHandle );
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitCreateTable),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitCreateTable) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * Schema Validation
     **********************************************************/

    /**
     * Schema 존재 여부 확인
     */
    
    sObjectName = aParseTree->mRelation->mName;

    if ( sObjectName->mSchema == NULL )
    {
        /**
         * Schema Name 이 명시되지 않은 경우
         */

        STL_TRY( ellGetAuthFirstSchemaDictHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sAuthHandle,
                                                  & sInitPlan->mSchemaHandle,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * Schema Name 이 명시된 경우
         */

        STL_TRY( ellGetSchemaDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         sObjectName->mSchema,
                                         & sInitPlan->mSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXISTS );
    }
    

    /**
     * Writable Schema 인지 검사
     */
    
    STL_TRY_THROW( ellGetSchemaWritable( & sInitPlan->mSchemaHandle ) == STL_TRUE,
                   RAMP_ERR_SCHEMA_NOT_WRITABLE );

    /**
     * CREATE TABLE ON SCHEMA 권한 검사
     */

    STL_TRY( qlaCheckSchemaPriv( aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 ELL_SCHEMA_PRIV_ACTION_CREATE_TABLE,
                                 & sInitPlan->mSchemaHandle,
                                 aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Table Validation
     **********************************************************/

    /**
     * 같은 이름을 가진 Object( Table, Sequence, Synonym) 가 있는지 확인
     * - Oracle 호환성을 위해 Name Space 를 Table,Sequence, Synonym 이 같이 사용한다.
     * - 호환성을 위해 Dictionary 체계를 바꾸는 것보다 QP 단에서 검사한다.
     */
    
    STL_TRY( qlyExistSameNameObject( aTransID,
                                     aSQLStmt,
                                     aSQLString,
                                     & sInitPlan->mSchemaHandle,
                                     sObjectName,
                                     & sObjectHandle,
                                     & sObjectExist,
                                     & sObjectType, /* Not Used In this function */
                                     aEnv )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sObjectExist != STL_TRUE, RAMP_ERR_SAME_NAME_OBJECT_EXISTS );
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                stlStrlen( sObjectName->mObject ) + 1,
                                (void **) & sInitPlan->mTableName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sInitPlan->mTableName, sObjectName->mObject );
    
    /**********************************************************
     * Tablespace Validation
     **********************************************************/
    
    /**
     * Tablespace 존재 여부 확인 
     */

    if ( aParseTree->mTablespaceName == NULL )
    {
        STL_TRY( ellGetAuthDataSpaceHandle( aTransID,
                                            aSQLStmt->mViewSCN,
                                            sAuthHandle,
                                            & sInitPlan->mSpaceHandle,
                                            ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ellGetTablespaceDictHandle( aTransID,
                                             aSQLStmt->mViewSCN,
                                             QLP_GET_PTR_VALUE( aParseTree->mTablespaceName ),
                                             & sInitPlan->mSpaceHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLESPACE_NOT_EXISTS );
    }
    
    

    /**
     * DATA Tablespace 인지 검사
     */

    STL_TRY_THROW( ellGetTablespaceUsageType( & sInitPlan->mSpaceHandle )
                   == ELL_SPACE_USAGE_TYPE_DATA,
                   RAMP_ERR_TABLESPACE_NOT_WRITABLE );
                   
    /**
     * CREATE OBJECT ON TABLESPACE 권한 검사
     */

    STL_TRY( qlaCheckSpacePriv( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                ELL_SPACE_PRIV_ACTION_CREATE_OBJECT,
                                & sInitPlan->mSpaceHandle,
                                aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Expression List 초기화
     **********************************************************/
    
    /**
     * Statement 단위 Expression List 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitStmtExprList ),
                                (void **) & sStmtExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mPseudoColExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mConstExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    sStmtExprList->mHasSubQuery = STL_FALSE;

    /**
     * Query 단위 Expression List 설정
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitExprList ),
                                (void **) & sQueryExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mPseudoColExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mAggrExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mNestedAggrExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    sQueryExprList->mStmtExprList = sStmtExprList;
    
    /**********************************************************
     * Table Element Validation
     **********************************************************/

    sInitPlan->mHasIdentity = STL_FALSE;
    sInitPlan->mColumnList  = NULL;
    
    stlMemset( & sInitPlan->mAllConstraint, 0x00, STL_SIZEOF(qlrInitTableConstraint) );
    
    STL_TRY( ellInitObjectList( & sInitPlan->mAllConstraint.mConstSchemaList,
                                QLL_INIT_PLAN( aDBCStmt ),
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellInitObjectList( & sInitPlan->mAllConstraint.mConstSpaceList,
                                QLL_INIT_PLAN( aDBCStmt ),
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Table columnar option
     */

    if( aParseTree->mTableDefType == QLP_TABLE_DEFINITION_TYPE_NONE )
    {
        /**
         * Table 생성 시 row/columnar 저장방식에 대해 정의하지 않았으면
         * DEFAULT_TABLE_TYPE property를 따른다.
         */

        sDefaultTableType = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DEFAULT_TABLE_TYPE,
                                                           KNL_ENV( aEnv ) );

        /* Default table type ( 0 : row table, 1 : columnar table ) */
        if( sDefaultTableType == 0 )
        {
            sInitPlan->mTableType = SML_MEMORY_PERSISTENT_HEAP_ROW_TABLE;
        }
        else
        {
            sInitPlan->mTableType = SML_MEMORY_PERSISTENT_HEAP_COLUMNAR_TABLE;
        }
    }
    else
    {
        /* WITHOUT COLUMNAR OPTIONS이면 default table type property 관계없이
         * row based table을 생성한다. */
        if( aParseTree->mTableDefType == QLP_TABLE_DEFINITION_TYPE_ROW )
        {
            sInitPlan->mTableType = SML_MEMORY_PERSISTENT_HEAP_ROW_TABLE;
        }
        else
        {
            /* WITH COLUMNAR OPTIONS이면 default table type property 관계없이
             * column based table을 생성한다. */
            STL_DASSERT( aParseTree->mTableDefType == QLP_TABLE_DEFINITION_TYPE_COLUMNAR );

            sInitPlan->mTableType = SML_MEMORY_PERSISTENT_HEAP_COLUMNAR_TABLE;
        }
    }
         

    /**********************************************************
     * Physical Attribute Validation
     **********************************************************/

    /**
     * PCTFREE Validation
     */
    
    if ( aParseTree->mPctFree == NULL )
    {
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_TABLE_PCTFREE_NO;
    }
    else
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(aParseTree->mPctFree),
                           stlStrlen( QLP_GET_PTR_VALUE(aParseTree->mPctFree) ),
                           & sBigintValue,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_PCTFREE );

        STL_TRY_THROW( (sBigintValue >= SML_MIN_PCTFREE) &&
                       (sBigintValue <= SML_MAX_PCTFREE),
                       RAMP_ERR_INVALID_PCTFREE );
        
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_TABLE_PCTFREE_YES;
        sInitPlan->mPhysicalAttr.mPctFree = (stlInt16) sBigintValue;
    }

    /**
     * PCTUSED Validation
     */
    
    if ( aParseTree->mPctUsed == NULL )
    {
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_TABLE_PCTUSED_NO;
    }
    else
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(aParseTree->mPctUsed),
                           stlStrlen( QLP_GET_PTR_VALUE(aParseTree->mPctUsed) ),
                           & sBigintValue,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_PCTUSED );

        STL_TRY_THROW( (sBigintValue >= SML_MIN_PCTUSED) &&
                       (sBigintValue <= SML_MAX_PCTUSED),
                       RAMP_ERR_INVALID_PCTUSED );
        
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_TABLE_PCTUSED_YES;
        sInitPlan->mPhysicalAttr.mPctUsed = (stlInt16) sBigintValue;
    }

    /**
     * INITRANS Validation
     */
    
    if ( aParseTree->mIniTrans == NULL )
    {
        STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_DEFAULT_INITRANS,
                                          &sBigintValue,
                                          KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(aParseTree->mIniTrans),
                           stlStrlen( QLP_GET_PTR_VALUE(aParseTree->mIniTrans) ),
                           & sBigintValue,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_INITRANS );

        STL_TRY_THROW( (sBigintValue >= SML_MIN_INITRANS ) &&
                       (sBigintValue <= SML_MAX_INITRANS),
                       RAMP_ERR_INVALID_INITRANS );
    }
    
    sInitPlan->mPhysicalAttr.mValidFlags |= SML_TABLE_INITRANS_YES;
    sInitPlan->mPhysicalAttr.mIniTrans = (stlInt16) sBigintValue;

    /**
     * MAXTRANS Validation
     */
    
    if ( aParseTree->mMaxTrans == NULL )
    {
        STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_DEFAULT_MAXTRANS,
                                          &sBigintValue,
                                          KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(aParseTree->mMaxTrans),
                           stlStrlen( QLP_GET_PTR_VALUE(aParseTree->mMaxTrans) ),
                           & sBigintValue,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_MAXTRANS );

        STL_TRY_THROW( (sBigintValue >= SML_MIN_MAXTRANS ) &&
                       (sBigintValue <= SML_MAX_MAXTRANS),
                       RAMP_ERR_INVALID_MAXTRANS );
    }

    sInitPlan->mPhysicalAttr.mValidFlags |= SML_TABLE_MAXTRANS_YES;
    sInitPlan->mPhysicalAttr.mMaxTrans = (stlInt16)sBigintValue;

    STL_TRY_THROW( sInitPlan->mPhysicalAttr.mMaxTrans >=
                   sInitPlan->mPhysicalAttr.mIniTrans,
                   RAMP_ERR_TRANS_RANGE );
    
    /**
     * Segment Attribute Validation
     */

    STL_TRY( qlrValidateSegmentAttr( aSQLString,
                                     (smlTbsId) ellGetTablespaceID( & sInitPlan->mSpaceHandle ),
                                     aParseTree->mSegInitialSize,
                                     aParseTree->mSegNextSize,
                                     aParseTree->mSegMinSize,
                                     aParseTree->mSegMaxSize,
                                     & sInitPlan->mPhysicalAttr.mSegAttr,
                                     aEnv )
             == STL_SUCCESS );

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mSpaceHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mSchemaHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    *aInitPlan      = sInitPlan;
    *aQueryExprList = sQueryExprList;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mSchema );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_WRITABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_WRITABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sInitPlan->mSchemaHandle ) );
    }
    
    STL_CATCH( RAMP_ERR_SAME_NAME_OBJECT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USED_OBJECT_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sInitPlan->mSchemaHandle ),
                      sObjectName->mObject );
    }

    STL_CATCH( RAMP_ERR_TABLESPACE_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mTablespaceName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE(aParseTree->mTablespaceName) );
    }

    STL_CATCH( RAMP_ERR_TABLESPACE_NOT_WRITABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_WRITABLE,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mTablespaceName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE(aParseTree->mTablespaceName) );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_PCTFREE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_PCTVALUE,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mPctFree->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_PCTUSED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_PCTVALUE,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mPctUsed->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_INITRANS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_INITRANS,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mIniTrans->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_MAXTRANS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_MAXTRANS,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mMaxTrans->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_TRANS_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TRANS_RANGE,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mIniTrans->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Init Plan 에 컬럼 정의를 추가한다.
 * @param[in]     aTransID         Transaction ID
 * @param[in]     aDBCStmt         DBC Statement
 * @param[in]     aSQLStmt         SQL Statement
 * @param[in]     aSQLString       SQL String
 * @param[in]     aDefaultExprList Expression List for DEFAULT
 * @param[in]     aSchemaHandle    Schema Handle
 * @param[in]     aTableHandle     Table Handle(nullable:CREATE TABLE)
 * @param[in]     aRelationType    Relation Type
 * @param[in]     aTableName       Table Name
 * @param[in]     aColumnDef       Column 정의 Parsing 정보
 * @param[in,out] aHasIdentity     Identity Column 존재 여부 
 * @param[in,out] aInitColumnList  Init Column List
 * @param[in]     aAllConstraint   All Constraint
 * @param[in]     aEnv             Environment
 * @remarks
 */
stlStatus qlrAddColumnDefinition( smlTransId                aTransID,
                                  qllDBCStmt              * aDBCStmt,
                                  qllStatement            * aSQLStmt,
                                  stlChar                 * aSQLString,
                                  qlvInitExprList         * aDefaultExprList,
                                  ellDictHandle           * aSchemaHandle,
                                  ellDictHandle           * aTableHandle,
                                  smlRelationType           aRelationType,
                                  stlChar                 * aTableName,
                                  qlpColumnDef            * aColumnDef,
                                  stlBool                 * aHasIdentity,
                                  qlrInitColumn          ** aInitColumnList,
                                  qlrInitTableConstraint  * aAllConstraint,
                                  qllEnv                  * aEnv )
{
    stlInt32           sIdx = 0;

    qlpSequenceOption * sSeqOption = NULL;
    
    qlrInitColumn * sColumnList = NULL;
    qlrInitColumn * sCodeColumn = NULL;

    stlInt32             sDefaultLen = 0;
    qlpValue           * sDefaultNullNode = NULL;

    qlvInitExpression  * sIdentityInit = NULL;
    qlvInitPseudoCol   * sIdentityPseudo = NULL;
    
    qlpValueExpr  * sValueExpr = NULL;
    
    qlvStmtInfo        sStmtInfo;
    
    ellObjectList * sObjList = NULL;
    stlInt64              sPrecision = DTL_PRECISION_NA;
    dtlStringLengthUnit   sCharUnit = DTL_STRING_LENGTH_UNIT_NA;
    stlInt64              sColLen;
    stlInt32              sNodePos = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDefaultExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnDef != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHasIdentity != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitColumnList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAllConstraint != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Statement Information
     */

    stlMemset( & sStmtInfo, 0x00, STL_SIZEOF( qlvStmtInfo ) );

    sStmtInfo.mTransID = aTransID;
    sStmtInfo.mDBCStmt = aDBCStmt;
    sStmtInfo.mSQLStmt = aSQLStmt;
    sStmtInfo.mSQLString = aSQLString;
    sStmtInfo.mQueryExprList = aDefaultExprList;

    /**
     * Column Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitColumn),
                                (void **) & sCodeColumn,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sCodeColumn, 0x00, STL_SIZEOF(qlrInitColumn) );

    /**
     * 동일한 컬럼 이름이 존재하는지 확인
     */

    sIdx = 0;
    
    for ( sColumnList = *aInitColumnList;
          sColumnList != NULL;
          sColumnList = sColumnList->mNext )
    {
        sIdx++;
        
        if ( stlStrcmp( aColumnDef->mColumnName, sColumnList->mColumnName ) == 0 )
        {
            STL_THROW( RAMP_ERR_SAME_COLUMN_EXIST );
        }
        else
        {
            /* 다른 컬럼 이름임 */
        }
    }

    /**
     * mColumnName 정보 
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                stlStrlen( aColumnDef->mColumnName ) + 1,
                                (void **) & sCodeColumn->mColumnName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sCodeColumn->mColumnName, aColumnDef->mColumnName );

    /**
     * mDBType
     */

    sCodeColumn->mTypeDef.mDBType = aColumnDef->mTypeName->mDBType;

    /**
     * mUserTypeName
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                aColumnDef->mTypeName->mNodeLen + 1,
                                (void **) & sCodeColumn->mTypeDef.mUserTypeName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemcpy( sCodeColumn->mTypeDef.mUserTypeName,
               aSQLString + aColumnDef->mTypeName->mNodePos,
               aColumnDef->mTypeName->mNodeLen );
    sCodeColumn->mTypeDef.mUserTypeName[aColumnDef->mTypeName->mNodeLen] = '\0';

    /**
     * Type 정보 설정
     * - mArgNum1
     * - mArgNum2
     * - mStringLengthUnit
     * - mIntervalIndicator
     */

    STL_TRY( qlvSetColumnType( & sStmtInfo,
                               & sCodeColumn->mTypeDef,
                               aColumnDef->mTypeName,
                               aEnv )
             == STL_SUCCESS );
    
    /**
     * mColumnDefault 정보
     * mNullDefault 정보 
     */

    if( aRelationType == SML_MEMORY_PERSISTENT_HEAP_COLUMNAR_TABLE )
    {
        STL_TRY_THROW( (aColumnDef->mTypeName->mDBType != DTL_TYPE_LONGVARCHAR) &&
                       (aColumnDef->mTypeName->mDBType != DTL_TYPE_LONGVARBINARY),
                       RAMP_ERR_COLUMNAR_TABLE_NOT_SUPPORT_COLUMN_TYPE );

        if( (sCodeColumn->mTypeDef.mArgNum1 == DTL_LENGTH_NA) &&
            ((sCodeColumn->mTypeDef.mDBType == DTL_TYPE_CHAR) ||
             (sCodeColumn->mTypeDef.mDBType == DTL_TYPE_BINARY)) )
        {
            sPrecision = 1;
        }
        else
        {
            sPrecision = sCodeColumn->mTypeDef.mArgNum1;
        }

        sCharUnit = sCodeColumn->mTypeDef.mStringLengthUnit;
        
        if ( sCharUnit == DTL_STRING_LENGTH_UNIT_NA )
        {
            sCharUnit = ellGetDbCharLengthUnit();
        }
        else
        {
            /* nothing to do */
        }

        STL_TRY( dtlGetDataValueBufferSize( sCodeColumn->mTypeDef.mDBType,
                                            sPrecision,
                                            sCharUnit,
                                            & sColLen,
                                            KNL_DT_VECTOR(aEnv),
                                            aEnv,
                                            KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sColLen <= SML_COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD_MAX,
                       RAMP_ERR_COLUMNAR_TABLE_INVALID_COLUMN_LENGTH );

        sCodeColumn->mColLength = sColLen;
    }
    
    sCodeColumn->mInitDefault = NULL;
    
    if ( aColumnDef->mDefSecond != NULL )
    {
        if ( aColumnDef->mDefSecond->mRawDefault != NULL )
        {
            STL_PARAM_VALIDATE( aColumnDef->mDefSecond->mRawDefault->mType
                                == QLL_VALUE_EXPR_TYPE, QLL_ERROR_STACK(aEnv) );

            sValueExpr = (qlpValueExpr *) aColumnDef->mDefSecond->mRawDefault;
            
            if ( sValueExpr->mExpr->mType == QLL_BNF_NULL_CONSTANT_TYPE )
            {
                /**
                 * DEFAULT NULL 임
                 */
                
                sCodeColumn->mColumnDefault = NULL;
                sCodeColumn->mNullDefault = STL_TRUE;

                /**
                 * DEFAULT NULL Expression 구성
                 */
                
                STL_TRY( qlrMakeDefaultNullNode( aDBCStmt, 
                                                 & sDefaultNullNode,
                                                 aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlvValidateNullBNF( & sStmtInfo,
                                             sDefaultNullNode,
                                             & sCodeColumn->mInitDefault,
                                             KNL_BUILTIN_FUNC_INVALID,
                                             DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                             DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                             QLL_INIT_PLAN( aDBCStmt ),
                                             aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /**
                 * Default Expression String 을 저장
                 */

                sDefaultLen = sValueExpr->mNodeLen
                    + ( sValueExpr->mNodePos - aColumnDef->mDefSecond->mNodePos );
                
                STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                            sDefaultLen + 1,
                                            (void **) & sCodeColumn->mColumnDefault,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                stlMemcpy( sCodeColumn->mColumnDefault,
                           aSQLString + aColumnDef->mDefSecond->mNodePos,
                           sDefaultLen );
                sCodeColumn->mColumnDefault[sDefaultLen] = '\0';
                stlTrimString( sCodeColumn->mColumnDefault );
                sCodeColumn->mNullDefault = STL_FALSE;

                /**
                 * Default Expression 을 Validation
                 */

                STL_TRY( ellInitObjectList( & sObjList, QLL_INIT_PLAN( aDBCStmt ), ELL_ENV(aEnv) ) == STL_SUCCESS );
                
                STL_TRY( qlvValidateValueExpr( & sStmtInfo,
                                               QLV_EXPR_PHRASE_DEFAULT,
                                               sObjList,
                                               NULL, /* aRefTableList */
                                               STL_FALSE,  /* aIsAtomicInsert */
                                               STL_FALSE,  /* Row Expr */
                                               0, /* Allowed Aggregation Depth */
                                               aColumnDef->mDefSecond->mRawDefault,
                                               & sCodeColumn->mInitDefault,
                                               NULL, /* aHasOuterJoinOperator */
                                               KNL_BUILTIN_FUNC_INVALID,
                                               DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                               DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                               QLL_INIT_PLAN( aDBCStmt ),
                                               aEnv )
                         == STL_SUCCESS );

                /**
                 * 호환 가능하지 않은 타입이거나 공간이 부족한 경우,
                 * INSERT 나 UPDATE 구문에서 DEFAULT 사용시 에러가 발생한다.
                 */
            }
        }
        else
        {
            /**
             * DEFAULT 구문이 없음 */
            sCodeColumn->mColumnDefault = NULL;
            sCodeColumn->mNullDefault = STL_FALSE;

            /**
             * DEFAULT NULL Expression 구성
             */
            
            STL_TRY( qlrMakeDefaultNullNode( aDBCStmt, 
                                             & sDefaultNullNode,
                                             aEnv )
                     == STL_SUCCESS );
            
            STL_TRY( qlvValidateNullBNF( & sStmtInfo,
                                         sDefaultNullNode,
                                         & sCodeColumn->mInitDefault,
                                         KNL_BUILTIN_FUNC_INVALID,
                                         DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                         DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                         QLL_INIT_PLAN( aDBCStmt ),
                                         aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /**
         * 부가 정보가 없음
         */
        
        sCodeColumn->mColumnDefault = NULL;
        sCodeColumn->mNullDefault = STL_FALSE;

        /**
         * DEFAULT NULL Expression 구성
         */
        
        STL_TRY( qlrMakeDefaultNullNode( aDBCStmt, 
                                         & sDefaultNullNode,
                                         aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( qlvValidateNullBNF( & sStmtInfo,
                                     sDefaultNullNode,
                                     & sCodeColumn->mInitDefault,
                                     KNL_BUILTIN_FUNC_INVALID,
                                     DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                     DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                     QLL_INIT_PLAN( aDBCStmt ),
                                     aEnv )
                 == STL_SUCCESS );
    }

    /**
     * mPhysicalOrdinalPosition 정보
     * mLogicalOrdinalPosition 정보
     * - ADD COLUMN 의 경우 재조정 됨
     */

    sCodeColumn->mPhysicalOrdinalPosition = sIdx;
    sCodeColumn->mLogicalOrdinalPosition  = sIdx;

    /**
     * mIsNullable 정보
     * - Constraint 에 의해 변경됨.
     */

    sCodeColumn->mIsNullable = STL_TRUE;

    /**
     * Itentity 정보
     * - mIsIdentity
     * - mIdentityOption
     * - mIdentityAttr
     */

    if ( aColumnDef->mDefSecond != NULL )
    {
        if ( aColumnDef->mDefSecond->mIdentType == QLP_IDENTITY_INVALID )
        {
            /**
             * Identity 컬럼이 아님
             */
            sCodeColumn->mIsIdentity = STL_FALSE;
            sCodeColumn->mIdentityOption = ELL_IDENTITY_GENERATION_NA;
            stlMemset( & sCodeColumn->mIdentityAttr, 0x00, STL_SIZEOF(smlSequenceAttr) );
        }
        else
        {
            /**
             * Identity 컬럼임
             */

            /**
             * Idnetity Column 은 하나만 가질 수 있다.
             */
            
            STL_TRY_THROW( *aHasIdentity == STL_FALSE,
                           RAMP_ERR_MULTIPLE_IDENTITY);
            
            *aHasIdentity = STL_TRUE;

            /**
             * Identity 컬럼은 Null 일 수 없다.
             */
            
            sCodeColumn->mIsIdentity = STL_TRUE;
            sCodeColumn->mIsNullable = STL_FALSE;

            switch ( aColumnDef->mDefSecond->mIdentType )
            {
                case QLP_IDENTITY_ALWAYS:
                    {
                        sCodeColumn->mIdentityOption = ELL_IDENTITY_GENERATION_ALWAYS;
                        break;
                    }
                case QLP_IDENTITY_BY_DEFAULT:
                    {
                        sCodeColumn->mIdentityOption = ELL_IDENTITY_GENERATION_BY_DEFAULT;
                        break;
                    }
                default:
                    {
                        sCodeColumn->mIdentityOption = ELL_IDENTITY_GENERATION_DEFAULT;
                        break;
                    }
            }

            stlMemset( & sCodeColumn->mIdentityAttr, 0x00, STL_SIZEOF(smlSequenceAttr) );
            
            sSeqOption = aColumnDef->mDefSecond->mIdentSeqOptions;

            STL_TRY( qlqValidateSeqGenOption( aSQLString,
                                              & sCodeColumn->mIdentityAttr,
                                              sSeqOption,
                                              aEnv )
                     == STL_SUCCESS );

            /**
             * Identity Column 을 위한 Expression 생성
             */

            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                        STL_SIZEOF( qlvInitPseudoCol ),
                                        (void **) & sIdentityPseudo,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemset( sIdentityPseudo,
                       0x00,
                       STL_SIZEOF( qlvInitPseudoCol ) );

            sIdentityPseudo->mPseudoId = KNL_PSEUDO_COL_NEXTVAL;
            sIdentityPseudo->mArgCount = 1;

            /**
             * Identity Handle 을 Argument 설정 
             */
            
            STL_TRY( knlAllocRegionMem(
                         QLL_INIT_PLAN(aDBCStmt),
                         STL_SIZEOF(ellPseudoArg),
                         (void **) & sIdentityPseudo->mArgs,
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemset( sIdentityPseudo->mArgs, 0x00, STL_SIZEOF(ellPseudoArg) );

            sIdentityPseudo->mSeqDictHandle = NULL;

            /**
             * Execution 시점에 Sequence Handle 을 설정해야 함.
             */
            
            sIdentityPseudo->mArgs[0].mPseudoArg = NULL;
            
            /**
             * Init Expression 설정 
             */
            
            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                        STL_SIZEOF( qlvInitExpression ),
                                        (void **) & sIdentityInit,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemset( sIdentityInit, 0x00, STL_SIZEOF( qlvInitExpression ) );

            sIdentityInit->mType            = QLV_EXPR_TYPE_PSEUDO_COLUMN; 
            sIdentityInit->mPosition        = aColumnDef->mDefSecond->mNodePos;
            sIdentityInit->mRelatedFuncId   = KNL_BUILTIN_FUNC_INVALID;
            sIdentityInit->mColumnName      = NULL;
            sIdentityInit->mIncludeExprCnt  = 1;
            sIdentityInit->mIterationTime   = gPseudoColInfoArr[KNL_PSEUDO_COL_NEXTVAL].mIterationTime;
            sIdentityInit->mExpr.mPseudoCol = sIdentityPseudo;

            sCodeColumn->mInitDefault = sIdentityInit;
        }
    }
    else
    {
        /**
         * 이름과 Type 정보만 존재함.
         */
        
        sCodeColumn->mIsIdentity = STL_FALSE;
        sCodeColumn->mIdentityOption = ELL_IDENTITY_GENERATION_NA;
        stlMemset( & sCodeColumn->mIdentityAttr, 0x00, STL_SIZEOF(smlSequenceAttr) );
    }

    /**
     * Rewrite DEFAULT Expression
     */
    
    STL_TRY( qlvRewriteExpr( sCodeColumn->mInitDefault,
                             DTL_ITERATION_TIME_FOR_EACH_EXPR,
                             aDefaultExprList,
                             & sCodeColumn->mInitDefault,
                             QLL_INIT_PLAN( aDBCStmt ),
                             aEnv )
             == STL_SUCCESS );
    
    /**
     * Column Definition Link 연결
     */

    if ( *aInitColumnList != NULL )
    {
        for ( sColumnList = *aInitColumnList;
              sColumnList->mNext != NULL;
              sColumnList = sColumnList->mNext )
        {
        }
        sColumnList->mNext = sCodeColumn;
    }
    else
    {
        *aInitColumnList = sCodeColumn;
    }

    /**
     * 컬럼에 제약조건이 있는 경우
     */
    if ( aColumnDef->mConstraints != NULL )
    {
        STL_TRY( qlrAddInlineConstDefinition( aTransID,
                                              aDBCStmt,
                                              aSQLStmt,
                                              aSQLString,
                                              aSchemaHandle,
                                              aTableHandle,
                                              aTableName,
                                              *aInitColumnList,
                                              aAllConstraint,
                                              sCodeColumn,
                                              aColumnDef->mConstraints,
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SAME_COLUMN_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_COLUMN_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           aColumnDef->mColumnNamePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_MULTIPLE_IDENTITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MULTIPLE_IDENTITY_COLUMN,
                      qlgMakeErrPosString( aSQLString,
                                           aColumnDef->mDefSecond->mIdentTypePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_COLUMNAR_TABLE_NOT_SUPPORT_COLUMN_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMNAR_TABLE_NOT_SUPPORT_COLUMN_TYPE,
                      qlgMakeErrPosString( aSQLString,
                                           aColumnDef->mTypeName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_COLUMNAR_TABLE_INVALID_COLUMN_LENGTH )
    {
        switch( aColumnDef->mTypeName->mDBType )
        {
            case DTL_TYPE_CHAR:
            case DTL_TYPE_VARCHAR:
                sNodePos = aColumnDef->mTypeName->mStringLength->mNodePos;
                break;
            case DTL_TYPE_BINARY:
            case DTL_TYPE_VARBINARY:
                sNodePos = aColumnDef->mTypeName->mBinaryLength->mNodePos;
                break;
            default:
                STL_DASSERT( STL_FALSE );
                break;
        }

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMNAR_TABLE_INVALID_COLUMN_LENGTH,
                      qlgMakeErrPosString( aSQLString,
                                           sNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      SML_COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD_MAX );
    }

    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   qlgMakeErrPosString( aSQLString, */
    /*                                        aColumnDef->mDefSecond->mRawDefault->mNodePos, */
    /*                                        aEnv ), */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlrAddColumnDefinition()" ); */
    /* } */

    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief DEFAULT NULL 에 해당하는 Parse Node 노드 생성
 * @param[in]  aDBCStmt          DBC Statement
 * @param[out] aDefaultNullNode  DEFAULT NULL parse node
 * @param[in]  aEnv              Environment
 * @remarks
 *
 */
stlStatus qlrMakeDefaultNullNode( qllDBCStmt  * aDBCStmt, 
                                  qlpValue   ** aDefaultNullNode,
                                  qllEnv      * aEnv )
{
    qlpValue  * sValue = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDefaultNullNode != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * DEFAULT NULL 노드 생성
     */
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlpValue ),
                                (void **) & sValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    sValue->mType          = QLL_BNF_NULL_CONSTANT_TYPE;
    sValue->mNodePos       = 0;
    sValue->mValue.mString = NULL;

    /**
     * Output 설정
     */

    *aDefaultNullNode = sValue;

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Column 정의와 함께 기술된 Table Constraint 제약조건 정의를 추가한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aSQLStmt         SQL Statement
 * @param[in]  aSQLString       SQL String
 * @param[in]  aSchemaHandle    Schema Handle
 * @param[in]  aTableHandle     Table Handle (nullable:CREATE TABLE)
 * @param[in]  aTableName       Table Name
 * @param[in]  aInitColumnList  Init Column List
 * @param[in]  aAllConstraint   All Constraint
 * @param[in]  aCodeColumn      Constraint 가 있는 Code Column
 * @param[in]  aConstList       Column 의 Table Constraint Parse Tree에 대한 list
 * @param[in]  aEnv             Environment
 */
stlStatus qlrAddInlineConstDefinition( smlTransId               aTransID,
                                       qllDBCStmt             * aDBCStmt,
                                       qllStatement           * aSQLStmt,
                                       stlChar                * aSQLString,
                                       ellDictHandle          * aSchemaHandle,
                                       ellDictHandle          * aTableHandle,
                                       stlChar                * aTableName,
                                       qlrInitColumn          * aInitColumnList,
                                       qlrInitTableConstraint * aAllConstraint,
                                       qlrInitColumn          * aCodeColumn,
                                       qlpList                * aConstList,
                                       qllEnv                 * aEnv )
{
    qlrInitConst * sCodeConst = NULL;

    qlpListElement * sListElem = NULL;
    qlpConstraint * sConst = NULL;
    stlInt32 sConstNodePos = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitColumnList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAllConstraint != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodeColumn != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Constraint list
     */

    QLP_LIST_FOR_EACH( aConstList, sListElem )
    {
        sConst = (qlpConstraint *) QLP_LIST_GET_POINTER_VALUE( sListElem );
        sConstNodePos = sConst->mNodePos;

        /**
         * Init Constraint 영역을 확보
         */

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    STL_SIZEOF(qlrInitConst),
                                    (void **) & sCodeConst,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sCodeConst, 0x00, STL_SIZEOF(qlrInitConst) );

        /**
         * mConstType
         */

        switch ( sConst->mConstType )
        {
            case QLP_CONSTRAINT_NULL:
                {
                    /**
                     * NULL Constraint 인 경우, Table Constraint 를 별도로 생성하지 않는다.
                     */

                    aCodeColumn->mIsNullable = STL_TRUE;
                    STL_THROW( RAMP_SUCCESS );
                    break;
                }
            case QLP_CONSTRAINT_NOTNULL:
                {
                    sCodeConst->mConstType = ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL;

                    aCodeColumn->mIsNullable = STL_FALSE;
                    break;
                }
            case QLP_CONSTRAINT_CHECK:
                {
                    /**
                     * @todo Check Clause 를 구현해야 함.
                     */

                    sCodeConst->mConstType = ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE;

                    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                    break;
                }
            case QLP_CONSTRAINT_PRIMARY:
                {
                    sCodeConst->mConstType = ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY;

                    /**
                     * Key Compare 가 가능한 Type 인지 검사
                     */
                    
                    STL_TRY_THROW( dtlCheckKeyCompareType( aCodeColumn->mTypeDef.mDBType ) == STL_TRUE,
                                   RAMP_ERR_INVALID_KEY_COMP_TYPE );
                    
                    aCodeColumn->mIsNullable = STL_FALSE;
                    break;
                }
            case QLP_CONSTRAINT_UNIQUE:
                {
                    sCodeConst->mConstType = ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY;

                    /**
                     * Key Compare 가 가능한 Type 인지 검사
                     */
                    
                    STL_TRY_THROW( dtlCheckKeyCompareType( aCodeColumn->mTypeDef.mDBType ) == STL_TRUE,
                                   RAMP_ERR_INVALID_KEY_COMP_TYPE );
                    
                    break;
                }
            case QLP_CONSTRAINT_FOREIGN:
                {
                    /**
                     * @todo Foreign Key 를 구현해야 함
                     */

                    sCodeConst->mConstType = ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY;

                    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );

                    /**
                     * Key Compare 가 가능한 Type 인지 검사
                     */
                    
                    STL_TRY_THROW( dtlCheckKeyCompareType( aCodeColumn->mTypeDef.mDBType ) == STL_TRUE,
                                   RAMP_ERR_INVALID_KEY_COMP_TYPE );
                    
                    break;
                }
            default:
                {
                    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                    break;
                }
        }

        /**
         * Constraint Characteristics 설정
         */
        
        STL_TRY( qlrSetConstCharacteristics( aSQLString,
                                             sConst,
                                             sCodeConst,
                                             aEnv )
                 == STL_SUCCESS );
    
        /**
         * mColumnCnt
         * mColumnIdx
         */

        sCodeConst->mColumnCnt = 1;
        sCodeConst->mColumnIdx[0] = aCodeColumn->mPhysicalOrdinalPosition;

        /**
         * Constraint 유형별 Validation
         */

        switch ( sCodeConst->mConstType )
        {
            case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
                {
                    sCodeConst->mIndexKeyFlag[0] =
                        DTL_KEYCOLUMN_INDEX_ORDER_ASC |
                        DTL_KEYCOLUMN_INDEX_NULLABLE_FALSE |
                        DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST;
                    
                    STL_TRY( qlrValidateUniquePrimaryKey( aTransID,
                                                          aDBCStmt,
                                                          aSQLStmt,
                                                          STL_TRUE,   /* Primary Key */
                                                          aSQLString,
                                                          aSchemaHandle,
                                                          aTableHandle,
                                                          aTableName,
                                                          aInitColumnList,
                                                          aAllConstraint,
                                                          sCodeConst,
                                                          sConst,
                                                          aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
                {
                    sCodeConst->mIndexKeyFlag[0] =
                        DTL_KEYCOLUMN_INDEX_ORDER_ASC |
                        DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE |
                        DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST;
                    
                    STL_TRY( qlrValidateUniquePrimaryKey( aTransID,
                                                          aDBCStmt,
                                                          aSQLStmt,
                                                          STL_FALSE,   /* Unique Key */
                                                          aSQLString,
                                                          aSchemaHandle,
                                                          aTableHandle,
                                                          aTableName,
                                                          aInitColumnList,
                                                          aAllConstraint,
                                                          sCodeConst,
                                                          sConst,
                                                          aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
                {
                    /**
                     * @todo Foreign Key 를 구현해야 함.
                     */

                    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                    break;
                }
            case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
                {
                    STL_TRY( qlrValidateCheckNotNull( aTransID,
                                                      aDBCStmt,
                                                      aSQLStmt,
                                                      aSQLString,
                                                      aSchemaHandle,
                                                      aTableName,
                                                      aInitColumnList,
                                                      aAllConstraint,
                                                      sCodeConst,
                                                      sConst,
                                                      aEnv )
                             == STL_SUCCESS );

                    /**
                     * identity column 의 NOT NULL 제약조건은
                     * NOT DEFERRABLE INITIALLY IMMEDIATE ENFORCED 이어야 한다.
                     */
                    
                    if ( aCodeColumn->mIsIdentity == STL_TRUE )
                    {
                        STL_TRY_THROW( (sCodeConst->mDeferrable == STL_FALSE) &&
                                       (sCodeConst->mInitDeferred == STL_FALSE) &&
                                       (sCodeConst->mEnforced == STL_TRUE),
                                       RAMP_ERR_INDENTITY_NOT_NULL_INVALID_CHARACTERISTICS );
                    }
                    else
                    {
                        /* nothing to do */
                    }
                    
                    break;
                }
            case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
                {
                    /**
                     * @todo Check Clause 를 구현해야 함.
                     */

                    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                    break;
                }
            default:
                {
                    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                    break;
                }
        }

        STL_RAMP( RAMP_SUCCESS );
        
        sCodeConst = NULL;
    }   /* END of QLP_LIST_FOR_EACH( aConstList, sListElem ) */

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_KEY_COMP_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ILLEGAL_USE_OF_DATA_TYPE,
                      qlgMakeErrPosString( aSQLString,
                                           sConstNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      dtlDataTypeName[ aCodeColumn->mTypeDef.mDBType ] );
    }

    
    STL_CATCH( RAMP_ERR_INDENTITY_NOT_NULL_INVALID_CHARACTERISTICS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_NOT_NULL_CONSTRAINT_SPECIFIED_ON_IDENTITY_COLUMN,
                      qlgMakeErrPosString( aSQLString,
                                           sConstNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      qlgMakeErrPosString( aSQLString,
                                           sConstNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "qlrAddInlineConstDefinition()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Out-line Table Constraint 제약조건 정의를 추가한다.
 * @param[in] aTransID         Transaction ID
 * @param[in] aDBCStmt         DBC Statement
 * @param[in] aSQLStmt         SQL Statement
 * @param[in] aSQLString       SQL String
 * @param[in] aSchemaHandle    Schema Handle
 * @param[in] aTableHandle     Table Handle (nullable: CREATE TABLE)
 * @param[in] aTableName       Table Name
 * @param[in] aInitColumnList  Init Column List
 * @param[in] aAllConstraint   All Constraint
 * @param[in] aConstList       Constraint 정의 Parsing 정보 List
 * @param[in] aEnv             Environment
 * @remarks
 */
stlStatus qlrAddOutlineConstDefinition( smlTransId               aTransID,
                                        qllDBCStmt             * aDBCStmt,
                                        qllStatement           * aSQLStmt,
                                        stlChar                * aSQLString,
                                        ellDictHandle          * aSchemaHandle,
                                        ellDictHandle          * aTableHandle,
                                        stlChar                * aTableName,
                                        qlrInitColumn          * aInitColumnList,
                                        qlrInitTableConstraint * aAllConstraint,
                                        qlpList                * aConstList,
                                        qllEnv                 * aEnv )
{
    qlrInitColumn * sCodeColumn = NULL;
    qlrInitConst  * sCodeConst = NULL;

    qlpListElement  * sListElement = NULL;
    qlpIndexElement * sKeyElement = NULL;
    stlChar         * sColumnName = NULL;
    stlInt64          sBigintValue = 0;

    qlpListElement * sListElem = NULL;
    qlpConstraint  * sConst = NULL;
    stlInt32         sConstNodePos = 0;

    stlInt32  i = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAllConstraint != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Constraint list
     */

    QLP_LIST_FOR_EACH( aConstList, sListElem )
    {
        sConst = (qlpConstraint *) QLP_LIST_GET_POINTER_VALUE( sListElem );
        sConstNodePos = sConst->mNodePos;

        /**
         * Init Constraint 영역을 확보
         */

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    STL_SIZEOF(qlrInitConst),
                                    (void **) & sCodeConst,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sCodeConst, 0x00, STL_SIZEOF(qlrInitConst) );

        /**
         * mConstType
         */

        switch ( sConst->mConstType )
        {
            case QLP_CONSTRAINT_NULL:
                {
                    /**
                     * Outline Constraint 에는 Key Constraint 만 존재한다.
                     */

                    STL_DASSERT(0);
                    break;
                }
            case QLP_CONSTRAINT_NOTNULL:
                {
                    /**
                     * Outline Constraint 에는 Key Constraint 만 존재한다.
                     */

                    STL_DASSERT(0);
                    break;
                }
            case QLP_CONSTRAINT_CHECK:
                {
                    /**
                     * Outline Constraint 에는 Key Constraint 만 존재한다.
                     */

                    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                    break;
                }
            case QLP_CONSTRAINT_PRIMARY:
                {
                    sCodeConst->mConstType = ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY;
                    break;
                }
            case QLP_CONSTRAINT_UNIQUE:
                {
                    sCodeConst->mConstType = ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY;
                    break;
                }
            case QLP_CONSTRAINT_FOREIGN:
                {
                    /**
                     * @todo Foreign Key 를 구현해야 함
                     */

                    sCodeConst->mConstType = ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY;
                    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                    break;
                }
            default:
                {
                    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                    break;
                }
        }

        /**
         * Constraint Characteristics 설정
         */
        
        STL_TRY( qlrSetConstCharacteristics( aSQLString,
                                             sConst,
                                             sCodeConst,
                                             aEnv )
                 == STL_SUCCESS );

        /**
         * mColumnCnt
         * mColumnIdx
         */

        sCodeConst->mColumnCnt = 0;

        QLP_LIST_FOR_EACH( sConst->mUniqueFields, sListElement )
        {
            /**
             * Key Column 의 최대 개수를 초과하는 지 검사  
             */

            sKeyElement = (qlpIndexElement *) QLP_LIST_GET_POINTER_VALUE( sListElement );
            sColumnName = QLP_GET_PTR_VALUE(sKeyElement->mColumnName);

            STL_TRY_THROW( sCodeConst->mColumnCnt < DTL_INDEX_COLUMN_MAX_COUNT,
                           RAMP_ERR_MAXIMUM_COLUMN_EXCEEDED );

            /**
             * Table 정의에 Column Name 에 대한 정의가 존재하는 지 검사
             */

            for ( sCodeColumn = aInitColumnList;
                  sCodeColumn != NULL;
                  sCodeColumn = sCodeColumn->mNext )
            {
                if ( stlStrcmp(sCodeColumn->mColumnName, sColumnName) == 0 )
                {
                    break;
                }
            }

            STL_TRY_THROW( sCodeColumn != NULL, RAMP_ERR_COLUMN_NOT_EXISTS );

            /**
             * Key Compare 가 가능한 Type 인지 검사
             */

            switch ( sCodeConst->mConstType )
            {
                case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
                case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
                case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
                    {
                        STL_TRY_THROW( dtlCheckKeyCompareType( sCodeColumn->mTypeDef.mDBType ) == STL_TRUE,
                                       RAMP_ERR_INVALID_KEY_COMP_TYPE );
                        break;
                    }
                case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
                case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
                    {
                        /* nothing to do */
                        break;
                    }
                default:
                    {
                        STL_DASSERT(0);
                        break;
                    }
            }
                    
            
            /**
             * Primary Key 인 경우 Column 정보를 Nullable 로 설정함
             */

            if ( sCodeConst->mConstType == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY )
            {
                sCodeColumn->mIsNullable = STL_FALSE;
            }

            /**
             * 동일한 Column Name 이 존재하는 지 검사 
             */

            for ( i = 0; i < sCodeConst->mColumnCnt; i++ )
            {
                STL_TRY_THROW( sCodeConst->mColumnIdx[i] != sCodeColumn->mPhysicalOrdinalPosition,
                               RAMP_ERR_DUPLICATE_COLUMN_NAME );
            }

            sCodeConst->mColumnIdx[sCodeConst->mColumnCnt] = sCodeColumn->mPhysicalOrdinalPosition;

            
            /**********************************************************
             * 인덱스 Key Column 속성 설정 
             **********************************************************/

            sCodeConst->mIndexKeyFlag[sCodeConst->mColumnCnt] = 0;

            /**
             * Key Column Nullable 여부 
             */
            
            if ( sCodeConst->mConstType == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY )
            {
                sCodeConst->mIndexKeyFlag[sCodeConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_NULLABLE_FALSE;
            }
            else
            {
                sCodeConst->mIndexKeyFlag[sCodeConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE;
            }
            
            /**
             * Key Column Order (ASC/DESC)
             */
            
            
            if ( sKeyElement->mIsAsc == NULL )
            {
                sCodeConst->mIndexKeyFlag[sCodeConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_ORDER_ASC;
            }
            else
            {
                sBigintValue = QLP_GET_INT_VALUE(sKeyElement->mIsAsc);
                
                if ( sBigintValue == STL_TRUE )
                {
                    sCodeConst->mIndexKeyFlag[sCodeConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_ORDER_ASC;
                }
                else
                {
                    sCodeConst->mIndexKeyFlag[sCodeConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_ORDER_DESC;
                }
            }
            
            /**
             * Key Nulls First/Last 
             */
            
            if ( sKeyElement->mIsNullsFirst == NULL )
            {
                sCodeConst->mIndexKeyFlag[sCodeConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_NULL_ORDER_DEFAULT;
            }
            else
            {
                sBigintValue = QLP_GET_INT_VALUE(sKeyElement->mIsNullsFirst);
                
                if ( sBigintValue == STL_TRUE )
                {
                    sCodeConst->mIndexKeyFlag[sCodeConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST;
                }
                else
                {
                    sCodeConst->mIndexKeyFlag[sCodeConst->mColumnCnt] |= DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST;
                }
            }

            sCodeConst->mColumnCnt++;
        }

        /**
         * Constraint 유형별 Validation
         */

        switch ( sCodeConst->mConstType )
        {
            case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
                {
                    STL_TRY( qlrValidateUniquePrimaryKey( aTransID,
                                                          aDBCStmt,
                                                          aSQLStmt,
                                                          STL_TRUE,   /* Primary Key */
                                                          aSQLString,
                                                          aSchemaHandle,
                                                          aTableHandle,
                                                          aTableName,
                                                          aInitColumnList,
                                                          aAllConstraint,
                                                          sCodeConst,
                                                          sConst,
                                                          aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
                {
                    STL_TRY( qlrValidateUniquePrimaryKey( aTransID,
                                                          aDBCStmt,
                                                          aSQLStmt,
                                                          STL_FALSE,   /* Unique Key */
                                                          aSQLString,
                                                          aSchemaHandle,
                                                          aTableHandle,
                                                          aTableName,
                                                          aInitColumnList,
                                                          aAllConstraint,
                                                          sCodeConst,
                                                          sConst,
                                                          aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
                {
                    /**
                     * @todo Foreign Key 를 구현해야 함.
                     */

                    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                    break;
                }
            case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
                {
                    /**
                     * Outline Constraint 에는 Key Constraint 만 존재한다.
                     */

                    STL_PARAM_VALIDATE( 1 == 0, QLL_ERROR_STACK(aEnv) );
                    break;
                }
            default:
                {
                    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                    break;
                }
        }
    }

    return STL_SUCCESS;

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
                      dtlDataTypeName[ sCodeColumn->mTypeDef.mDBType ] );
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
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      qlgMakeErrPosString( aSQLString,
                                           sConstNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "qlrAddOutlineConstDefinition()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Constraint 의 Characterics 를 설정한다.
 * @param[in] aSQLString  SQL String
 * @param[in] aParseConst Constraint Parse Tree
 * @param[in] aInitConst  Constraint Init Plan
 * @param[in] aEnv        Environment
 * @remarks
 */
stlStatus qlrSetConstCharacteristics( stlChar       * aSQLString,
                                      qlpConstraint * aParseConst,
                                      qlrInitConst  * aInitConst,
                                      qllEnv        * aEnv )
{
    stlInt64  sBigintValue = 0;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseConst != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitConst != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * mDeferrable
     * mInitDeferred
     * mEnforced
     */
    
    if ( aParseConst->mConstAttr == NULL )
    {
        aInitConst->mDeferrable = ELL_TABLE_CONSTRAINT_DEFERRABLE_DEFAULT;
        aInitConst->mInitDeferred = ELL_TABLE_CONSTRAINT_INITIALLY_DEFERRED_DEFAULT;
        aInitConst->mEnforced = ELL_TABLE_CONSTRAINT_ENFORCED_DEFAULT;
    }
    else
    {
        /**
         * mInitDeferred
         */

        if ( aParseConst->mConstAttr->mInitDeferred != NULL )
        {
            sBigintValue = QLP_GET_INT_VALUE( aParseConst->mConstAttr->mInitDeferred );
            aInitConst->mInitDeferred = (stlBool) sBigintValue;
        }
        else
        {
            aInitConst->mInitDeferred = ELL_TABLE_CONSTRAINT_INITIALLY_DEFERRED_DEFAULT;
        }

        /**
         * mDeferrable
         */
        
        if ( aParseConst->mConstAttr->mDeferrable != NULL )
        {
            sBigintValue = QLP_GET_INT_VALUE( aParseConst->mConstAttr->mDeferrable );
            aInitConst->mDeferrable = (stlBool) sBigintValue;

            if ( (aInitConst->mDeferrable == STL_FALSE) && (aInitConst->mInitDeferred == STL_TRUE) )
            {
                STL_THROW( RAMP_ERR_CANNOT_DEFER_NOT_DEFERRABLE );
            }
            else
            {
                /* ok */
            }
        }
        else
        {
            /**
             * Deferrable 이 명시되지 않은 경우 INIT DEFERRED/IMMEDIATE 값을 따른다
             */
            if ( aInitConst->mInitDeferred == STL_TRUE )
            {
                aInitConst->mDeferrable = STL_TRUE;
            }
            else
            {
                aInitConst->mDeferrable = STL_FALSE;
            }
        }

        /**
         * mEnforced
         */

        if ( aParseConst->mConstAttr->mEnforce != NULL )
        {
            sBigintValue = QLP_GET_INT_VALUE( aParseConst->mConstAttr->mEnforce );
            aInitConst->mEnforced = (stlBool) sBigintValue;
            
            /**
             * @todo ENFORCED / NOT ENFORCED constraint 구현해야 함
             */

            STL_TRY_THROW( aInitConst->mEnforced == STL_TRUE, RAMP_ERR_NOT_IMPLEMENTED );
        }
        else
        {
            aInitConst->mEnforced = ELL_TABLE_CONSTRAINT_ENFORCED_DEFAULT;
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlrSetConstCharacteristics()" );
    }

    STL_CATCH( RAMP_ERR_CANNOT_DEFER_NOT_DEFERRABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_DEFER_CONSTRAINT_THAT_IS_NOT_DEFERRABLE,
                      qlgMakeErrPosString( aSQLString,
                                           aParseConst->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Source Type 으로부터 Destine Type 을 재구성한다.
 * @param[in]  sSrcType    사용자가 정의 Type 정보
 * @param[out] sDstType    모든 정보가 채워진 Type 정보 
 * @remarks
 * 사용자가 정의한 Column Type 정보로부터 DEFAULT expression 등을 처리하기 위해서는
 * Dictionary Handle 로부터 얻는 정보와 동일한 상태로 얻을 수 있어야 한다.
 */ 
void qlrSetRefineColumnType( qlvInitTypeDef * sSrcType,
                             qlvInitTypeDef * sDstType )
{
    stlMemcpy( sDstType, sSrcType, STL_SIZEOF(qlvInitTypeDef) );

    switch( sDstType->mDBType )
    {
        case DTL_TYPE_BOOLEAN:
            {
                /* nothing to do */
                break;
            }
        case DTL_TYPE_NATIVE_SMALLINT:
            {
                /* nothing to do */
                break;
            }
        case DTL_TYPE_NATIVE_INTEGER:
            {
                /* nothing to do */
                break;
            }
        case DTL_TYPE_NATIVE_BIGINT:
            {
                /* nothing to do */
                break;
            }
        case DTL_TYPE_NATIVE_REAL:
            {
                /* nothing to do */
                break;
            }
        case DTL_TYPE_NATIVE_DOUBLE:
            {
                /* nothing to do */
                break;
            }
        case DTL_TYPE_FLOAT:
            {
                if ( sDstType->mArgNum1 == DTL_PRECISION_NA )
                {
                    sDstType->mArgNum1 = gDataTypeDefinition[DTL_TYPE_FLOAT].mDefNumericPrec;
                }

                break;
            }
        case DTL_TYPE_NUMBER:
            {
                if ( sDstType->mArgNum1 == DTL_PRECISION_NA )
                {
                    sDstType->mArgNum1 = gDataTypeDefinition[DTL_TYPE_NUMBER].mDefNumericPrec;
                }
                
                /**
                 * scale 은 N/A 인 경우가 있음
                 * (c1 NUMBER) 와 같이 정의한 경우
                 */
                break;
            }
        case DTL_TYPE_DECIMAL:
            {
                /* not implemented */
                STL_DASSERT( 0 );
                break;
            }
        case DTL_TYPE_CHAR:
            {
                if ( sDstType->mArgNum1 == DTL_LENGTH_NA )
                {
                    sDstType->mArgNum1 = gDataTypeDefinition[DTL_TYPE_CHAR].mDefStringLength;
                }

                if ( sDstType->mStringLengthUnit == DTL_STRING_LENGTH_UNIT_NA )
                {
                    sDstType->mStringLengthUnit = ellGetDbCharLengthUnit();
                }
                
                break;
            }
        case DTL_TYPE_VARCHAR:
            {
                if ( sDstType->mArgNum1 == DTL_LENGTH_NA )
                {
                    sDstType->mArgNum1 = gDataTypeDefinition[DTL_TYPE_VARCHAR].mDefStringLength;
                }

                if ( sDstType->mStringLengthUnit == DTL_STRING_LENGTH_UNIT_NA )
                {
                    sDstType->mStringLengthUnit = ellGetDbCharLengthUnit();
                }
                
                break;
            }
        case DTL_TYPE_LONGVARCHAR:
            {
                /* nothing to do */
                break;
            }
        case DTL_TYPE_CLOB:
            {
                /* nothing to do */
                break;
            }
        case DTL_TYPE_BINARY:
            {
                if ( sDstType->mArgNum1 == DTL_LENGTH_NA )
                {
                    sDstType->mArgNum1 = gDataTypeDefinition[DTL_TYPE_BINARY].mDefStringLength;
                }
                
                break;
            }
        case DTL_TYPE_VARBINARY:
            {
                if ( sDstType->mArgNum1 == DTL_LENGTH_NA )
                {
                    sDstType->mArgNum1 = gDataTypeDefinition[DTL_TYPE_VARBINARY].mDefStringLength;
                }
                
                break;
            }
        case DTL_TYPE_LONGVARBINARY:
            {
                /* nothing to do */
                break;
            }
        case DTL_TYPE_BLOB:
            {
                /* nothing to do */
                break;
            }
        case DTL_TYPE_DATE:
            {
                /* nothing to do */
                break;
            }
        case DTL_TYPE_TIME:
            {
                if ( sDstType->mArgNum1 == DTL_PRECISION_NA )
                {
                    sDstType->mArgNum1 = gDataTypeDefinition[DTL_TYPE_TIME].mDefFractionalSecondPrec;
                }
                
                break;
            }
        case DTL_TYPE_TIMESTAMP:
            {
                if ( sDstType->mArgNum1 == DTL_PRECISION_NA )
                {
                    sDstType->mArgNum1 = gDataTypeDefinition[DTL_TYPE_TIMESTAMP].mDefFractionalSecondPrec;
                }
                
                break;
            }
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
            {
                if ( sDstType->mArgNum1 == DTL_PRECISION_NA )
                {
                    sDstType->mArgNum1 =
                        gDataTypeDefinition[DTL_TYPE_TIME_WITH_TIME_ZONE].mDefFractionalSecondPrec;
                }
                
                break;
            }
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                if ( sDstType->mArgNum1 == DTL_PRECISION_NA )
                {
                    sDstType->mArgNum1 =
                        gDataTypeDefinition[DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE].mDefFractionalSecondPrec;
                }
                
                break;
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            {
                switch ( sDstType->mIntervalIndicator )
                {
                    case DTL_INTERVAL_INDICATOR_YEAR:
                    case DTL_INTERVAL_INDICATOR_MONTH:
                    case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
                        {
                            if ( sDstType->mArgNum1 == DTL_PRECISION_NA )
                            {
                                sDstType->mArgNum1 =
                                    gDataTypeDefinition[DTL_TYPE_INTERVAL_YEAR_TO_MONTH].mDefIntervalPrec;
                            }
                            break;
                        }
                    default:
                        {
                            STL_DASSERT(0);
                            break;
                        }
                }
                
                break;
            }
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                switch ( sDstType->mIntervalIndicator )
                {
                    case DTL_INTERVAL_INDICATOR_DAY:
                    case DTL_INTERVAL_INDICATOR_HOUR:
                    case DTL_INTERVAL_INDICATOR_MINUTE:
                    case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR:
                    case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE:
                    case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:
                        {
                            if ( sDstType->mArgNum1 == DTL_PRECISION_NA )
                            {
                                sDstType->mArgNum1 =
                                    gDataTypeDefinition[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mDefIntervalPrec;
                            }
                            
                            break;
                        }
                    case DTL_INTERVAL_INDICATOR_SECOND:
                    case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND:
                    case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:
                    case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
                        {
                            if ( sDstType->mArgNum1 == DTL_PRECISION_NA )
                            {
                                sDstType->mArgNum1 =
                                    gDataTypeDefinition[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mDefIntervalPrec;
                            }
                            
                            if ( sDstType->mArgNum2 == DTL_PRECISION_NA )
                            {
                                sDstType->mArgNum2 =
                                    gDataTypeDefinition[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mDefFractionalSecondPrec;
                            }
                            break;
                        }
                    default:
                        {
                            break;
                        }
                }
                
                break;
            }
        case DTL_TYPE_ROWID:
            {
                /* nothing to do */
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
}



/**
 * @brief Check Not Null Constraint 를 Validation 한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aSQLStmt         SQL Statement
 * @param[in]  aSQLString       SQL String
 * @param[in]  aSchemaHandle    Schema Dictionary Handle
 * @param[in]  aTableName       Table Name
 * @param[in]  aInitColumnList  Init Column List
 * @param[in]  aAllConstraint   All Constraint
 * @param[in]  aCodeConst       Constraint Init Plan
 * @param[in]  aConstDef        Constraint Parse Tree
 * @param[in]  aEnv             Environment
 * @remarks
 * In-line Constraint 로만 생성할 수 있다.
 */
stlStatus qlrValidateCheckNotNull( smlTransId               aTransID,
                                   qllDBCStmt             * aDBCStmt,
                                   qllStatement           * aSQLStmt,
                                   stlChar                * aSQLString,
                                   ellDictHandle          * aSchemaHandle,
                                   stlChar                * aTableName,
                                   qlrInitColumn          * aInitColumnList,
                                   qlrInitTableConstraint * aAllConstraint,
                                   qlrInitConst           * aCodeConst,
                                   qlpConstraint          * aConstDef,
                                   qllEnv                 * aEnv )
{
    stlInt32  i = 0;
    
    stlBool   sAutoGenName = STL_FALSE;
    stlChar   sNameBuffer[STL_MAX_SQL_IDENTIFIER_LENGTH + 1] = {0,};

    qlrInitColumn * sCodeColumn = NULL;
    qlrInitConst  * sConstList = NULL;

    stlBool       sObjectExist = STL_FALSE;
    stlBool       sDuplicate = STL_FALSE;
    
    ellDictHandle sConstHandle;
    ellInitDictHandle( & sConstHandle );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAllConstraint != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodeConst != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDef != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aCodeConst->mConstType == ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodeConst->mColumnCnt == 1, QLL_ERROR_STACK(aEnv) );

    /**
     * mConstSchemaHandle
     * mConstName
     */

    if ( aConstDef->mName == NULL )
    {
        /**
         * Constraint 의 Schema Handle 설정
         * - Table 의 Schema 와 동일한 Schema 를 갖는다.
         */

        stlMemcpy( & aCodeConst->mConstSchemaHandle,
                   aSchemaHandle,
                   STL_SIZEOF( ellDictHandle ) );

        /**
         * 관련 Column Name 정보 취합
         * - Check Not Null 은 In-line Constraint 로 하나의 컬럼만을 참조한다.
         */

        for ( sCodeColumn = aInitColumnList;
              sCodeColumn != NULL;
              sCodeColumn = sCodeColumn->mNext )
        {
            if ( sCodeColumn->mPhysicalOrdinalPosition == aCodeConst->mColumnIdx[0] )
            {
                break;
            }
        }
        
        STL_PARAM_VALIDATE( sCodeColumn != NULL, QLL_ERROR_STACK(aEnv) );
        
        /**
         * Constraint Name 을 자동으로 생성
         */

        ellMakeCheckNotNullName( sNameBuffer,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                                 aTableName,
                                 sCodeColumn->mColumnName );

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    stlStrlen( sNameBuffer ) + 1,
                                    (void **) & aCodeConst->mConstName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( aCodeConst->mConstName, sNameBuffer );

        sAutoGenName = STL_TRUE;
    }
    else
    {
        /**
         * Constraint 의 Schema Hanlde 결정
         */
        
        if ( aConstDef->mName->mSchema != NULL )
        {
            STL_TRY( ellGetSchemaDictHandle( aTransID,
                                             aSQLStmt->mViewSCN,
                                             aConstDef->mName->mSchema,
                                             & aCodeConst->mConstSchemaHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * 존재하지 않는 Schema 인지 검사
             */

            STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXISTS );
            
            /**
             * Wriable Schema 인지 검사
             */
            
            STL_TRY_THROW( ellGetSchemaWritable( & aCodeConst->mConstSchemaHandle )
                           == STL_TRUE, RAMP_ERR_SCHEMA_NOT_WRITABLE );
        }
        else
        {
            /**
             * Table 과 동일한 Schema 를 가져감
             */
            
            stlMemcpy( & aCodeConst->mConstSchemaHandle,
                       aSchemaHandle,
                       STL_SIZEOF( ellDictHandle ) );
        }

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    stlStrlen( aConstDef->mName->mObject ) + 1,
                                    (void **) & aCodeConst->mConstName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( aCodeConst->mConstName, aConstDef->mName->mObject );
        
        sAutoGenName = STL_FALSE;
    }

    /**
     * Constraint Schema 에 대해 ADD CONSTRAINT ON SCHEMA 권한 검사 
     */

    STL_TRY( qlaCheckSchemaPriv( aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 ELL_SCHEMA_PRIV_ACTION_ADD_CONSTRAINT,
                                 & aCodeConst->mConstSchemaHandle,
                                 aEnv )
             == STL_SUCCESS );
    
    /**
     * Table 정의에 동일한 Constraint Name 이 존재하는 지 검사
     */

    STL_TRY( qlrCheckSameConstNameInTableDef( aAllConstraint,
                                              & aCodeConst->mConstSchemaHandle,
                                              aCodeConst->mConstName,
                                              & sObjectExist,
                                              aEnv )
             == STL_SUCCESS );

    if ( sObjectExist == STL_TRUE )
    {
        if ( sAutoGenName == STL_TRUE )
        {
            STL_THROW( RAMP_ERR_AUTO_NAME_EXISTS );
        }
        else
        {
            STL_THROW( RAMP_ERR_NAME_EXISTS );
        }
    }
    else
    {
        /* 존재하지 않는 Constraint Name 임. */
    }
    
    /**
     * Schema 에 동일한 Constraint Name 이 존재하는 지 검사
     */

    STL_TRY( ellGetConstraintDictHandleWithSchema( aTransID,
                                                   aSQLStmt->mViewSCN,
                                                   & aCodeConst->mConstSchemaHandle,
                                                   aCodeConst->mConstName,
                                                   & sConstHandle,
                                                   & sObjectExist,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sObjectExist == STL_TRUE )
    {
        if ( sAutoGenName == STL_TRUE )
        {
            STL_THROW( RAMP_ERR_AUTO_NAME_EXISTS );
        }
        else
        {
            STL_THROW( RAMP_ERR_NAME_EXISTS );
        }
    }
    else
    {
        /* 존재하지 않는 Constraint Name 임. */
    }
                                                   
    /**
     * mIndexSpaceHandle
     * mIndexName
     * mIndexAttr
     * mIndexKeyFlag
     */

    ellInitDictHandle( & aCodeConst->mIndexSpaceHandle );
    aCodeConst->mIndexName = NULL;
    stlMemset( & aCodeConst->mIndexAttr, 0x00, STL_SIZEOF(smlIndexAttr) );
    stlMemset( aCodeConst->mIndexKeyFlag,
               0x00,
               STL_SIZEOF(stlUInt8) * DTL_INDEX_COLUMN_MAX_COUNT );
        
    /**
     * mCheckClause
     * - CHECK NOT NULL 은 별도의 구문을 저장하지 않는다.
     */

    aCodeConst->mCheckClause = NULL;

    /**
     * mRefColumnHandle
     */

    for ( i = 0; i < DTL_INDEX_COLUMN_MAX_COUNT; i++ )
    {
        ellInitDictHandle( & aCodeConst->mRefColumnHandle[i] );
    }

    /**
     * mMatchOption
     * mUpdateRule
     * mDeleteRule
     */

    aCodeConst->mMatchOption = ELL_REFERENTIAL_MATCH_OPTION_NA;
    aCodeConst->mUpdateRule = ELL_REFERENTIAL_ACTION_RULE_NA;
    aCodeConst->mDeleteRule = ELL_REFERENTIAL_ACTION_RULE_NA;
    
    /**
     * Check Not Null Constraint 에 연결
     */

    STL_TRY( ellAddNewObjectItem( aAllConstraint->mConstSchemaList,
                                  & aCodeConst->mConstSchemaHandle,
                                  & sDuplicate,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                  
    if ( aAllConstraint->mCheckNotNull == NULL )
    {
        aAllConstraint->mCheckNotNull = aCodeConst;
    }
    else
    {
        for ( sConstList = aAllConstraint->mCheckNotNull;
              sConstList->mNext != NULL;
              sConstList = sConstList->mNext )
        {
        }
        sConstList->mNext = aCodeConst;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aConstDef->mName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      aConstDef->mName->mSchema );
    }
    
    STL_CATCH( RAMP_ERR_SCHEMA_NOT_WRITABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_WRITABLE,
                      qlgMakeErrPosString( aSQLString,
                                           aConstDef->mName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      aConstDef->mName->mSchema );
    }
    
    STL_CATCH( RAMP_ERR_AUTO_NAME_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_AUTO_CONSTRAINT_NAME_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aConstDef->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_CATCH( RAMP_ERR_NAME_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CONSTRAINT_NAME_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aConstDef->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief Primary or Unique Key Constraint 를 Validation 한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aSQLStmt         SQL Statement
 * @param[in]  aIsPrimary       Primary Key 인지 Unique Key 인지의 여부 
 * @param[in]  aSQLString       SQL String
 * @param[in]  aSchemaHandle    Schema Handle
 * @param[in]  aTableHandle     Table Handle (nullable: CREATE TABLE)
 * @param[in]  aTableName       Table Name
 * @param[in]  aInitColumnList  Init Column List (nullable: ADD CONSTRAINT)
 * @param[in]  aAllConstraint   All Constraint (nullable: ADD CONSTRAINT)
 * @param[in]  aCodeConst       Constraint Init Plan
 * @param[in]  aConstDef        Constraint Parse Tree
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlrValidateUniquePrimaryKey( smlTransId               aTransID,
                                       qllDBCStmt             * aDBCStmt,
                                       qllStatement           * aSQLStmt,
                                       stlBool                  aIsPrimary,
                                       stlChar                * aSQLString,
                                       ellDictHandle          * aSchemaHandle,
                                       ellDictHandle          * aTableHandle,
                                       stlChar                * aTableName,
                                       qlrInitColumn          * aInitColumnList,
                                       qlrInitTableConstraint * aAllConstraint,
                                       qlrInitConst           * aCodeConst,
                                       qlpConstraint          * aConstDef,
                                       qllEnv                 * aEnv )
{
    stlInt32  i = 0;

    ellDictHandle * sAuthHandle = NULL;
    
    stlBool   sAutoGenName = STL_FALSE;
    stlChar   sNameBuffer[STL_MAX_SQL_IDENTIFIER_LENGTH + 1] = {0,};

    ellNameList   * sNameList = NULL;
    qlrInitColumn * sColumnList = NULL;
    
    stlBool       sObjectExist = STL_FALSE;
    stlBool       sDuplicate = STL_FALSE;

    stlInt64      sBigintValue = 0;
    stlBool       sLoggingFlag = STL_FALSE;
    
    ellDictHandle sConstHandle;
    ellDictHandle sIndexHandle;
    
    qlrInitConst * sConstList = NULL;

    stlInt32        sPrimaryKeyCnt = 0;   
    ellDictHandle * sPrimaryKeyHandle = NULL;

    stlChar       * sIndexName = NULL;
    
    ellInitDictHandle( & sConstHandle );
    ellInitDictHandle( & sIndexHandle );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableName != NULL, QLL_ERROR_STACK(aEnv) );
    /* STL_PARAM_VALIDATE( aAllConstraint != NULL, QLL_ERROR_STACK(aEnv) ); */
    STL_PARAM_VALIDATE( aCodeConst != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDef != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( (aCodeConst->mConstType == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY)
                        ||
                        (aCodeConst->mConstType == ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodeConst->mColumnCnt > 0, QLL_ERROR_STACK(aEnv) );

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**********************************************************
     * 동일한 Key Constraint 가 존재하는지 검사
     **********************************************************/
     
    /**
     * Primary Key 가 중복 정의되는지 검사
     */

    if ( aIsPrimary == STL_TRUE )
    {
        if ( aAllConstraint != NULL )
        {
            STL_TRY_THROW( aAllConstraint->mPrimaryKey == NULL, RAMP_ERR_MULTIPLE_PRIMARY_KEY );
        }
        else
        {
            /**
             * ALTER TALBE .. ADD CONSTRAINT
             * - nothing to do 
             */
        }
    }
    else
    {
        /* unique key 는 검사할 필요가 없음 */
    }

    /**
     * 구문내에 동일한 순서의 Key Column 을 갖는 Unique Key 가 있는지 검사
     */

    if ( aAllConstraint != NULL )
    {
        STL_TRY( qlrCheckSameUniqueKeyInTableDef( aAllConstraint,
                                                  aCodeConst,
                                                  & sObjectExist,
                                                  aEnv )
                 == STL_SUCCESS );
    
        STL_TRY_THROW( sObjectExist != STL_TRUE, RAMP_ERR_UNIQUE_KEY_EXIST );
    }
    else
    {
        /**
         * ALTER TALBE .. ADD CONSTRAINT
         * - nothing to do 
         */
    }
    
    /**
     * mConstSchemaHandle
     * mConstName
     */

    if ( aConstDef->mName == NULL )
    {
        /**
         * Constraint 의 Schema Handle 설정
         * - Table 의 Schema 와 동일한 Schema 를 갖는다.
         */

        stlMemcpy( & aCodeConst->mConstSchemaHandle,
                   aSchemaHandle,
                   STL_SIZEOF( ellDictHandle ) );

        /**
         * Constraint Name 을 자동으로 생성
         */

        if ( aIsPrimary == STL_TRUE )
        {
            if ( aTableHandle != NULL )
            {
                /**
                 * 이미 존재하는 Table 인 경우(ADD COLUMN)
                 */
                
                /**
                 * Primary Key 정보 획득
                 */
                
                STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                                          aSQLStmt->mViewSCN,
                                                          QLL_INIT_PLAN(aDBCStmt),
                                                          aTableHandle,
                                                          & sPrimaryKeyCnt,
                                                          & sPrimaryKeyHandle,
                                                          ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_TRY_THROW( sPrimaryKeyCnt == 0, RAMP_ERR_MULTIPLE_PRIMARY_KEY );
            }
            else
            {
                /**
                 * CREATE TABLE 인 경우 Nothing To Do
                 */
            }
            
            /**
             * Primary Key Name 을 자동으로 생성
             */
            
            ellMakePrimaryKeyName( sNameBuffer,
                                   STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                                   aTableName );
        }
        else
        {
            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                        STL_SIZEOF(ellNameList) * aCodeConst->mColumnCnt,
                                        (void **) & sNameList,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            stlMemset( sNameList, 0x00, STL_SIZEOF(ellNameList) * aCodeConst->mColumnCnt );
            
            for ( i = 0; i < aCodeConst->mColumnCnt; i++ )
            {
                if ( aAllConstraint != NULL )
                {
                    /**
                     * CREATE TABLE or ADD COLUMN
                     */
                    for ( sColumnList = aInitColumnList;
                          sColumnList != NULL;
                          sColumnList = sColumnList->mNext )
                    {
                        if ( sColumnList->mPhysicalOrdinalPosition == aCodeConst->mColumnIdx[i] )
                        {
                            break;
                        }
                    }
                    
                    STL_PARAM_VALIDATE( sColumnList != NULL, QLL_ERROR_STACK(aEnv) );
                    
                    sNameList[i].mName = sColumnList->mColumnName;
                }
                else
                {
                    /**
                     * ALTER TALBE .. ADD CONSTRAINT
                     */

                    sNameList[i].mName = ellGetColumnName( & aCodeConst->mColumnHandle[i] );
                }
                
                if ( (i + 1) == aCodeConst->mColumnCnt )
                {
                    sNameList[i].mNext = NULL;
                }
                else
                {
                    sNameList[i].mNext = & sNameList[i+1];
                }
            }
        
            /**
             * Unique Key Name 을 자동으로 생성
             */
            
            ellMakeUniqueKeyName( sNameBuffer,
                                  STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                                  aTableName,
                                  sNameList );
        }

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    stlStrlen( sNameBuffer ) + 1,
                                    (void **) & aCodeConst->mConstName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( aCodeConst->mConstName, sNameBuffer );

        sAutoGenName = STL_TRUE;
    }
    else
    {
        /**
         * Constraint 의 Schema Hanlde 결정
         */
        
        if ( aConstDef->mName->mSchema != NULL )
        {
            STL_TRY( ellGetSchemaDictHandle( aTransID,
                                             aSQLStmt->mViewSCN,
                                             aConstDef->mName->mSchema,
                                             & aCodeConst->mConstSchemaHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * 존재하지 않는 Schema 인지 검사
             */

            STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXISTS );
            
            /**
             * Wriable Schema 인지 검사
             */
            
            STL_TRY_THROW( ellGetSchemaWritable( & aCodeConst->mConstSchemaHandle )
                           == STL_TRUE, RAMP_ERR_SCHEMA_NOT_WRITABLE );
        }
        else
        {
            /**
             * Table 과 동일한 Schema 를 가져감
             */
            
            stlMemcpy( & aCodeConst->mConstSchemaHandle,
                       aSchemaHandle,
                       STL_SIZEOF( ellDictHandle ) );
        }

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    stlStrlen( aConstDef->mName->mObject ) + 1,
                                    (void **) & aCodeConst->mConstName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( aCodeConst->mConstName, aConstDef->mName->mObject );
        
        sAutoGenName = STL_FALSE;
    }

    /**
     * Constraint Schema 에 대해 ADD CONSTRAINT ON SCHEMA 권한 검사 
     */

    STL_TRY( qlaCheckSchemaPriv( aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 ELL_SCHEMA_PRIV_ACTION_ADD_CONSTRAINT,
                                 & aCodeConst->mConstSchemaHandle,
                                 aEnv )
             == STL_SUCCESS );
    
    /**
     * Table 정의에 동일한 Constraint Name 이 존재하는 지 검사
     */

    if ( aAllConstraint != NULL )
    {
        STL_TRY( qlrCheckSameConstNameInTableDef( aAllConstraint,
                                                  & aCodeConst->mConstSchemaHandle,
                                                  aCodeConst->mConstName,
                                                  & sObjectExist,
                                                  aEnv )
                 == STL_SUCCESS );

        if ( sObjectExist == STL_TRUE )
        {
            if ( sAutoGenName == STL_TRUE )
            {
                STL_THROW( RAMP_ERR_AUTO_NAME_EXISTS );
            }
            else
            {
                STL_THROW( RAMP_ERR_NAME_EXISTS );
            }
        }
        else
        {
            /* 존재하지 않는 Constraint Name 임. */
        }
    }
    else
    {
        /**
         * ALTER TALBE .. ADD CONSTRAINT
         * - nothing to do 
         */
        
    }
    
    /**
     * Schema 에 동일한 Constraint Name 이 존재하는 지 검사
     */

    STL_TRY( ellGetConstraintDictHandleWithSchema( aTransID,
                                                   aSQLStmt->mViewSCN,
                                                   & aCodeConst->mConstSchemaHandle,
                                                   aCodeConst->mConstName,
                                                   & sConstHandle,
                                                   & sObjectExist,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sObjectExist == STL_TRUE )
    {
        if ( sAutoGenName == STL_TRUE )
        {
            STL_THROW( RAMP_ERR_AUTO_NAME_EXISTS );
        }
        else
        {
            STL_THROW( RAMP_ERR_NAME_EXISTS );
        }
    }
    else
    {
        /* 존재하지 않는 Constraint Name 임. */
    }

    /**********************************************************
     * 동일한 Index 가 존재하는지 검사
     **********************************************************/
    
    /**
     * mIndexSchemaHandle
     * mIndexSpaceHandle
     * mIndexName
     */

    if ( aConstDef->mIndexName == NULL )
    {
        /**
         * INDEX 이름을 명시하지 않음
         * - Key Index 의 Schema 는 Key Constraint 의 Schema 를 따름 
         */
        
        ellMakeKeyIndexName( sNameBuffer,
                             STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                             aCodeConst->mConstName );

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    stlStrlen( sNameBuffer ) + 1,
                                    (void **) & aCodeConst->mIndexName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( aCodeConst->mIndexName, sNameBuffer );

        sAutoGenName = STL_TRUE;
    }
    else
    {
        /**
         * 생성할 INDEX 이름을 명시함
         */

        sIndexName = QLP_GET_PTR_VALUE( aConstDef->mIndexName );
        
        /**
         * 인덱스 이름 복사
         */

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    stlStrlen( sIndexName ) + 1,
                                    (void **) & aCodeConst->mIndexName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( aCodeConst->mIndexName, sIndexName );
        
        sAutoGenName = STL_FALSE;
    }

    /**
     * 구문내에 동일한 Index Name 이 존재하는 지 검사
     */

    if ( aAllConstraint != NULL )
    {
        STL_TRY( qlrCheckSameIndexNameInTableDef( aAllConstraint,
                                                  & aCodeConst->mConstSchemaHandle,
                                                  aCodeConst->mIndexName,
                                                  & sObjectExist,
                                                  aEnv )
                 == STL_SUCCESS );
        
        if ( sObjectExist == STL_TRUE )
        {
            if ( sAutoGenName == STL_TRUE )
            {
                STL_THROW( RAMP_ERR_AUTO_INDEX_NAME_EXISTS );
            }
            else
            {
                STL_THROW( RAMP_ERR_INDEX_NAME_EXISTS );
            }
        }
        else
        {
            /* 존재하지 않는 Index Name 임. */
        }
    }
    else
    {
        /**
         * ALTER TALBE .. ADD CONSTRAINT
         * - nothing to do 
         */
    }
    
    /**
     * Schema 내에 동일한 Index Name 이 존재하는 지 검사
     */

    STL_TRY( ellGetIndexDictHandleWithSchema( aTransID,
                                              aSQLStmt->mViewSCN,
                                              & aCodeConst->mConstSchemaHandle,
                                              aCodeConst->mIndexName,
                                              & sIndexHandle,
                                              & sObjectExist,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sObjectExist == STL_TRUE )
    {
        if ( sAutoGenName == STL_TRUE )
        {
            STL_THROW( RAMP_ERR_AUTO_INDEX_NAME_EXISTS );
        }
        else
        {
            STL_THROW( RAMP_ERR_INDEX_NAME_EXISTS );
        }
    }
    else
    {
        /* 존재하지 않는 Index Name 임. */
    }

    /**********************************************************
     * 인덱스 테이블 스페이스의 물리적 속성 검사 
     **********************************************************/

    /**
     * Index Logging 정보 획득
     */

    qliGetIndexLoggingFlag( aSQLString,
                            & aConstDef->mIndexAttr,
                            & aCodeConst->mIndexAttr,
                            & sLoggingFlag,
                            aEnv );
    
    /**
     * Tablespace 존재 여부 확인 
     */

    if ( aConstDef->mIndexSpace == NULL )
    {
        if ( sLoggingFlag == STL_TRUE )
        {
            STL_TRY( ellGetAuthDataSpaceHandle( aTransID,
                                                aSQLStmt->mViewSCN,
                                                sAuthHandle,
                                                & aCodeConst->mIndexSpaceHandle,
                                                ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( ellGetAuthTempSpaceHandle( aTransID,
                                                aSQLStmt->mViewSCN,
                                                sAuthHandle,
                                                & aCodeConst->mIndexSpaceHandle,
                                                ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        STL_TRY( ellGetTablespaceDictHandle( aTransID,
                                             aSQLStmt->mViewSCN,
                                             QLP_GET_PTR_VALUE( aConstDef->mIndexSpace ),
                                             & aCodeConst->mIndexSpaceHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_INDEX_TABLESPACE_NOT_EXISTS );

    }

    /**
     * Index Tablespace 에 대한 CREATE INDEX 무결성 검사
     */

    if ( sLoggingFlag == STL_TRUE )
    {
        STL_TRY_THROW( ellGetTablespaceUsageType( & aCodeConst->mIndexSpaceHandle )
                       == ELL_SPACE_USAGE_TYPE_DATA,
                       RAMP_ERR_TABLESPACE_NOT_WRITABLE_FOR_LOGGING_INDEX );
    }
    else
    {
        STL_TRY_THROW( ellGetTablespaceUsageType( & aCodeConst->mIndexSpaceHandle )
                       == ELL_SPACE_USAGE_TYPE_TEMPORARY,
                       RAMP_ERR_TABLESPACE_NOT_WRITABLE_FOR_NOLOGGING_INDEX );
    }

    /**
     * CREATE OBJECT ON TABLESPACE 권한 검사
     */
    
    STL_TRY( qlaCheckSpacePriv( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                ELL_SPACE_PRIV_ACTION_CREATE_OBJECT,
                                & aCodeConst->mIndexSpaceHandle,
                                aEnv )
             == STL_SUCCESS );
    
    
    /**********************************************************
     * 인덱스의 물리적 속성 검사 
     **********************************************************/
     
    
    /**
     * mIndexAttr
     */

    STL_TRY( qliValidateIndexAttr( aSQLString,
                                   (smlTbsId) ellGetTablespaceID( & aCodeConst->mIndexSpaceHandle ),
                                   & aConstDef->mIndexAttr,
                                   & aCodeConst->mIndexAttr,
                                   aEnv )
             == STL_SUCCESS );
                                   
    aCodeConst->mIndexAttr.mValidFlags  |= SML_INDEX_UNIQUENESS_MASK;
    aCodeConst->mIndexAttr.mUniquenessFlag = STL_TRUE;

    aCodeConst->mIndexAttr.mValidFlags |= SML_INDEX_PRIMARY_MASK;
    
    if ( aIsPrimary == STL_TRUE )
    {
        aCodeConst->mIndexAttr.mPrimaryFlag = STL_TRUE;
    }
    else
    {
        aCodeConst->mIndexAttr.mPrimaryFlag = STL_FALSE;
    }

    /**
     * Parallel Factor 값
     * - Create Table 시 사용자 입력에 관계없이
     * 유효성 검사만 수행하고 Parallel Factor 는 1 로 결정한다.
     */

    if ( aConstDef->mIndexAttr.mParallel == NULL )
    {
        if ( aAllConstraint != NULL )
        {
            /**
             * CREATE TABLE, ADD COLUMN
             */
            
            aCodeConst->mIndexParallel = 1;
        }
        else
        {
            /**
             * ADD CONSTRAINT
             */
            
            aCodeConst->mIndexParallel = 0;
        }
    }
    else
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(aConstDef->mIndexAttr.mParallel),
                           stlStrlen( QLP_GET_PTR_VALUE(aConstDef->mIndexAttr.mParallel) ),
                           & sBigintValue,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_PARALLEL_FACTOR );

        STL_TRY_THROW( (sBigintValue >= 0 ) &&
                       (sBigintValue <= SML_MAX_INDEX_BUILD_PARALLEL_FACTOR),
                       RAMP_ERR_INVALID_PARALLEL_FACTOR );

        
        if ( aAllConstraint != NULL )
        {
            /**
             * CREATE TABLE, ADD COLUMN
             */
            
            aCodeConst->mIndexParallel = 1;
        }
        else
        {
            /**
             * ADD CONSTRAINT
             */
            
            aCodeConst->mIndexParallel = sBigintValue;
        }
    }

    /**********************************************************
     * 관련 없는 정보 초기화 
     **********************************************************/
    
    /**
     * mCheckClause
     * - CHECK NOT NULL 은 별도의 구문을 저장하지 않는다.
     */

    aCodeConst->mCheckClause = NULL;

    /**
     * mRefColumnHandle
     */

    for ( i = 0; i < DTL_INDEX_COLUMN_MAX_COUNT; i++ )
    {
        ellInitDictHandle( & aCodeConst->mRefColumnHandle[i] );
    }

    /**
     * mMatchOption
     * mUpdateRule
     * mDeleteRule
     */

    aCodeConst->mMatchOption = ELL_REFERENTIAL_MATCH_OPTION_NA;
    aCodeConst->mUpdateRule = ELL_REFERENTIAL_ACTION_RULE_NA;
    aCodeConst->mDeleteRule = ELL_REFERENTIAL_ACTION_RULE_NA;

    /**********************************************************
     * Constraint 연결 
     **********************************************************/

    if ( aAllConstraint != NULL )
    {
        STL_TRY( ellAddNewObjectItem( aAllConstraint->mConstSchemaList,
                                      & aCodeConst->mConstSchemaHandle,
                                      & sDuplicate,
                                      QLL_INIT_PLAN(aDBCStmt),
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( ellAddNewObjectItem( aAllConstraint->mConstSpaceList,
                                      & aCodeConst->mIndexSpaceHandle,
                                      & sDuplicate,
                                      QLL_INIT_PLAN(aDBCStmt),
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        if ( aIsPrimary == STL_TRUE )
        {
            /**
             * Primary Key Constraint 에 연결
             */
            
            aAllConstraint->mPrimaryKey = aCodeConst;
        }
        else
        {
            /**
             * Unique Key Constraint 에 연결
             */
            
            if ( aAllConstraint->mUniqueKey == NULL )
            {
                aAllConstraint->mUniqueKey = aCodeConst;
            }
            else
            {
                for ( sConstList = aAllConstraint->mUniqueKey;
                      sConstList->mNext != NULL;
                      sConstList = sConstList->mNext )
                {
                }
                
                sConstList->mNext = aCodeConst;
            }
        }
    }
    else
    {
        /**
         * ALTER TALBE .. ADD CONSTRAINT
         * - nothing to do 
         */
    }

         
                                   
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MULTIPLE_PRIMARY_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MULTIPLE_PRIMARY_KEY,
                      qlgMakeErrPosString( aSQLString,
                                           aConstDef->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_UNIQUE_KEY_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MULTIPLE_UNIQUE_KEY,
                      qlgMakeErrPosString( aSQLString,
                                           aConstDef->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aConstDef->mName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      aConstDef->mName->mSchema );
    }
    
    STL_CATCH( RAMP_ERR_SCHEMA_NOT_WRITABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_WRITABLE,
                      qlgMakeErrPosString( aSQLString,
                                           aConstDef->mName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      aConstDef->mName->mSchema );
    }

    STL_CATCH( RAMP_ERR_AUTO_NAME_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_AUTO_CONSTRAINT_NAME_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aConstDef->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_CATCH( RAMP_ERR_NAME_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CONSTRAINT_NAME_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aConstDef->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_INVALID_PARALLEL_FACTOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_PARALLEL_FACTOR,
                      qlgMakeErrPosString( aSQLString,
                                           aConstDef->mIndexAttr.mParallel->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      SML_MAX_INDEX_BUILD_PARALLEL_FACTOR );
    }
    
    STL_CATCH( RAMP_ERR_INDEX_TABLESPACE_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aConstDef->mIndexSpace->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE(aConstDef->mIndexSpace) );
    }
    
    STL_CATCH( RAMP_ERR_TABLESPACE_NOT_WRITABLE_FOR_LOGGING_INDEX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_WRITABLE_FOR_LOGGING_INDEX,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetTablespaceName( & aCodeConst->mIndexSpaceHandle ) );
    }

    STL_CATCH( RAMP_ERR_TABLESPACE_NOT_WRITABLE_FOR_NOLOGGING_INDEX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_WRITABLE_FOR_NOLOGGING_INDEX,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetTablespaceName( & aCodeConst->mIndexSpaceHandle ) );
    }
    
    STL_CATCH( RAMP_ERR_AUTO_INDEX_NAME_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_AUTO_INDEX_NAME_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aConstDef->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_CATCH( RAMP_ERR_INDEX_NAME_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INDEX_NAME_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aConstDef->mIndexName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}





/**
 * @brief 구문 내에 동일한 이름의 Constraint 가 존재하는지 검사
 * @param[in]  aAllConstraint  All Constraint
 * @param[in]  aSchemaHandle   Constraint 의 Schema Handle
 * @param[in]  aNewName        Constraint Name
 * @param[in]  aObjectExist    존재 여부
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qlrCheckSameConstNameInTableDef( qlrInitTableConstraint * aAllConstraint,
                                           ellDictHandle          * aSchemaHandle,
                                           stlChar                * aNewName,
                                           stlBool                * aObjectExist,
                                           qllEnv                 * aEnv )
{
    stlBool   sExist = STL_FALSE;

    qlrInitConst * sConstList = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aAllConstraint != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNewName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectExist != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Primary Key 에서 확인 
     */
    
    for ( sConstList = aAllConstraint->mPrimaryKey;
          sConstList != NULL;
          sConstList = sConstList->mNext )
    {
        if ( stlStrcmp( sConstList->mConstName, aNewName ) == 0 )
        {
            if ( ellGetSchemaID( & sConstList->mConstSchemaHandle )
                 == ellGetSchemaID( aSchemaHandle ) )
            {
                /* 동일한 Schema 임 */
                sExist = STL_TRUE;
                STL_THROW( RAMP_EXISTS );
            }
            else
            {
                /* 이름은 동일하나 다른 Schema 임 */
            }
        }
        else
        {
            /* 다른 이름임 */
        }
    }

    /**
     * Unique Key 에서 확인 
     */
    
    for ( sConstList = aAllConstraint->mUniqueKey;
          sConstList != NULL;
          sConstList = sConstList->mNext )
    {
        if ( stlStrcmp( sConstList->mConstName, aNewName ) == 0 )
        {
            if ( ellGetSchemaID( & sConstList->mConstSchemaHandle )
                 == ellGetSchemaID( aSchemaHandle ) )
            {
                /* 동일한 Schema 임 */
                sExist = STL_TRUE;
                STL_THROW( RAMP_EXISTS );
            }
            else
            {
                /* 이름은 동일하나 다른 Schema 임 */
            }
        }
        else
        {
            /* 다른 이름임 */
        }
    }

    /**
     * Foreign Key 에서 확인 
     */
    
    for ( sConstList = aAllConstraint->mForeignKey;
          sConstList != NULL;
          sConstList = sConstList->mNext )
    {
        if ( stlStrcmp( sConstList->mConstName, aNewName ) == 0 )
        {
            if ( ellGetSchemaID( & sConstList->mConstSchemaHandle )
                 == ellGetSchemaID( aSchemaHandle ) )
            {
                /* 동일한 Schema 임 */
                sExist = STL_TRUE;
                STL_THROW( RAMP_EXISTS );
            }
            else
            {
                /* 이름은 동일하나 다른 Schema 임 */
            }
        }
        else
        {
            /* 다른 이름임 */
        }
    }

    /**
     * Check Not Null 에서 확인 
     */
    
    for ( sConstList = aAllConstraint->mCheckNotNull;
          sConstList != NULL;
          sConstList = sConstList->mNext )
    {
        if ( stlStrcmp( sConstList->mConstName, aNewName ) == 0 )
        {
            if ( ellGetSchemaID( & sConstList->mConstSchemaHandle )
                 == ellGetSchemaID( aSchemaHandle ) )
            {
                /* 동일한 Schema 임 */
                sExist = STL_TRUE;
                STL_THROW( RAMP_EXISTS );
            }
            else
            {
                /* 이름은 동일하나 다른 Schema 임 */
            }
        }
        else
        {
            /* 다른 이름임 */
        }
    }

    /**
     * Check Clause 에서 확인 
     */
    
    for ( sConstList = aAllConstraint->mCheckClause;
          sConstList != NULL;
          sConstList = sConstList->mNext )
    {
        if ( stlStrcmp( sConstList->mConstName, aNewName ) == 0 )
        {
            if ( ellGetSchemaID( & sConstList->mConstSchemaHandle )
                 == ellGetSchemaID( aSchemaHandle ) )
            {
                /* 동일한 Schema 임 */
                sExist = STL_TRUE;
                STL_THROW( RAMP_EXISTS );
            }
            else
            {
                /* 이름은 동일하나 다른 Schema 임 */
            }
        }
        else
        {
            /* 다른 이름임 */
        }
    }
    
    /**
     * Output 설정
     */

    STL_RAMP( RAMP_EXISTS );
    
    *aObjectExist = sExist;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief 구문내에 동일한 이름의 Key Index 가 존재하는지 검사
 * @param[in]  aAllConstraint  All Constraint
 * @param[in]  aSchemaHandle Key Index 의 Schema Handle
 * @param[in]  aNewName      Key Index Name
 * @param[in]  aObjectExist  존재 여부
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus qlrCheckSameIndexNameInTableDef( qlrInitTableConstraint * aAllConstraint,
                                           ellDictHandle          * aSchemaHandle,
                                           stlChar                * aNewName,
                                           stlBool                * aObjectExist,
                                           qllEnv                 * aEnv )
{
    stlBool   sExist = STL_FALSE;

    qlrInitConst * sConstList = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aAllConstraint != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNewName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectExist != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Primary Key 에서 확인 
     */
    
    for ( sConstList = aAllConstraint->mPrimaryKey;
          sConstList != NULL;
          sConstList = sConstList->mNext )
    {
        if ( stlStrcmp( sConstList->mIndexName, aNewName ) == 0 )
        {
            if ( ellGetSchemaID( & sConstList->mConstSchemaHandle )
                 == ellGetSchemaID( aSchemaHandle ) )
            {
                /* 동일한 Schema 임 */
                sExist = STL_TRUE;
                STL_THROW( RAMP_EXISTS );
            }
            else
            {
                /* 이름은 동일하나 다른 Schema 임 */
            }
        }
        else
        {
            /* 다른 이름임 */
        }
    }

    /**
     * Unique Key 에서 확인 
     */
    
    for ( sConstList = aAllConstraint->mUniqueKey;
          sConstList != NULL;
          sConstList = sConstList->mNext )
    {
        if ( stlStrcmp( sConstList->mIndexName, aNewName ) == 0 )
        {
            if ( ellGetSchemaID( & sConstList->mConstSchemaHandle )
                 == ellGetSchemaID( aSchemaHandle ) )
            {
                /* 동일한 Schema 임 */
                sExist = STL_TRUE;
                STL_THROW( RAMP_EXISTS );
            }
            else
            {
                /* 이름은 동일하나 다른 Schema 임 */
            }
        }
        else
        {
            /* 다른 이름임 */
        }
    }

    /**
     * Foreign Key 에서 확인 
     */
    
    for ( sConstList = aAllConstraint->mForeignKey;
          sConstList != NULL;
          sConstList = sConstList->mNext )
    {
        if ( stlStrcmp( sConstList->mIndexName, aNewName ) == 0 )
        {
            if ( ellGetSchemaID( & sConstList->mConstSchemaHandle )
                 == ellGetSchemaID( aSchemaHandle ) )
            {
                /* 동일한 Schema 임 */
                sExist = STL_TRUE;
                STL_THROW( RAMP_EXISTS );
            }
            else
            {
                /* 이름은 동일하나 다른 Schema 임 */
            }
        }
        else
        {
            /* 다른 이름임 */
        }
    }

    /**
     * Output 설정
     */

    STL_RAMP( RAMP_EXISTS );
    
    *aObjectExist = sExist;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 구문 내에 동일한 Key 를 갖는 Unique or Primary Key 가 존재하는지 검사
 * @param[in]  aAllConstraint  All Constraint
 * @param[in]  aCodeConst    Constraint Init Plan
 * @param[in]  aObjectExist  존재 여부
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus qlrCheckSameUniqueKeyInTableDef( qlrInitTableConstraint * aAllConstraint,
                                           qlrInitConst           * aCodeConst,
                                           stlBool                * aObjectExist,
                                           qllEnv                 * aEnv )
{
    stlBool  sExist = STL_FALSE;

    stlInt32 i = 0;
    
    qlrInitConst * sConstList = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aAllConstraint != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodeConst != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectExist != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Primary Key 에서 확인 
     */
    
    for ( sConstList = aAllConstraint->mPrimaryKey;
          sConstList != NULL;
          sConstList = sConstList->mNext )
    {
        if ( sConstList->mColumnCnt == aCodeConst->mColumnCnt )
        {
            /**
             * Key Column Count 가 동일한 경우
             */

            sExist = STL_TRUE;
            
            for ( i = 0; i < sConstList->mColumnCnt; i++ )
            {
                if ( sConstList->mColumnIdx[i] != aCodeConst->mColumnIdx[i] )
                {
                    sExist = STL_FALSE;
                    break;
                }
            }

            /**
             * 모든 Key 가 동일하다면 Error 임
             * - Key 의 순서가 다르다면 무방함.
             */
            STL_TRY_THROW( sExist != STL_TRUE, RAMP_EXISTS );
        }
        else
        {
            /* Key Column 개수가 다름 */
        }
    }

    /**
     * Unique Key 에서 확인 
     */
    
    for ( sConstList = aAllConstraint->mUniqueKey;
          sConstList != NULL;
          sConstList = sConstList->mNext )
    {
        if ( sConstList->mColumnCnt == aCodeConst->mColumnCnt )
        {
            /**
             * Key Column Count 가 동일한 경우
             */

            sExist = STL_TRUE;
            
            for ( i = 0; i < sConstList->mColumnCnt; i++ )
            {
                if ( sConstList->mColumnIdx[i] != aCodeConst->mColumnIdx[i] )
                {
                    sExist = STL_FALSE;
                    break;
                }
            }

            /**
             * 모든 Key 가 동일하다면 Error 임
             * - Key 의 순서가 다르다면 무방함.
             */
            STL_TRY_THROW( sExist != STL_TRUE, RAMP_EXISTS );
        }
        else
        {
            /* Key Column 개수가 다름 */
        }
    }
    
    /**
     * Output 설정
     */

    STL_RAMP( RAMP_EXISTS );
    
    *aObjectExist = sExist;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Segment 의 Attribute 를 Validation 한다.
 * @param[in]  aSQLString      SQL text
 * @param[in]  aSpacePhyID     Tablespace Physical ID
 * @param[in]  aSegInitialSize Segment Initial Bytes
 * @param[in]  aSegNextSize    Segment Next Bytes
 * @param[in]  aSegMinSize     Segment Minimum Size
 * @param[in]  aSegMaxSize     Segment Maximum Size
 * @param[out] aSegAttr        Segment Attribute
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qlrValidateSegmentAttr( stlChar         * aSQLString,
                                  smlTbsId          aSpacePhyID,
                                  qlpSize         * aSegInitialSize,
                                  qlpSize         * aSegNextSize,
                                  qlpSize         * aSegMinSize,
                                  qlpSize         * aSegMaxSize,
                                  smlSegmentAttr  * aSegAttr,
                                  qllEnv          * aEnv )
{
    stlInt64 sBigint = 0;
    stlInt64 sMaxSize = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSegAttr != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 초기화
     */
    
    aSegAttr->mValidFlags = 0;
    
    /**
     * Segment INITIAL
     */
    
    if ( aSegInitialSize != NULL )
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(aSegInitialSize->mSize),
                           stlStrlen( QLP_GET_PTR_VALUE(aSegInitialSize->mSize) ),
                           & sBigint,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_SEGMENT_INITIAL );
        sBigint *= QLP_GET_BYTES_FROM_SIZE_UNIT( QLP_GET_INT_VALUE( aSegInitialSize->mSizeUnit ) );

        STL_TRY_THROW( sBigint > 0, RAMP_ERR_INVALID_SEGMENT_INITIAL );
        
        aSegAttr->mValidFlags |= SML_SEGMENT_INITIAL_YES;
        aSegAttr->mInitial = sBigint;
    }
    else
    {
        aSegAttr->mValidFlags |= SML_SEGMENT_INITIAL_NO;
    }

    /**
     * Segment NEXT
     */

    if ( aSegNextSize != NULL )
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(aSegNextSize->mSize),
                           stlStrlen( QLP_GET_PTR_VALUE(aSegNextSize->mSize) ),
                           & sBigint,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_SEGMENT_NEXT );
        sBigint *= QLP_GET_BYTES_FROM_SIZE_UNIT( QLP_GET_INT_VALUE( aSegNextSize->mSizeUnit ) );

        STL_TRY_THROW( sBigint > 0, RAMP_ERR_INVALID_SEGMENT_NEXT );
        
        aSegAttr->mValidFlags |= SML_SEGMENT_NEXT_YES;
        aSegAttr->mNext = sBigint;
    }
    else
    {
        aSegAttr->mValidFlags |= SML_SEGMENT_NEXT_NO;
    }

    /**
     * Segment MINSIZE
     */

    if ( aSegMinSize != NULL )
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(aSegMinSize->mSize),
                           stlStrlen( QLP_GET_PTR_VALUE(aSegMinSize->mSize) ),
                           & sBigint,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_SEGMENT_MINSIZE );
        sBigint *= QLP_GET_BYTES_FROM_SIZE_UNIT( QLP_GET_INT_VALUE( aSegMinSize->mSizeUnit ) );

        STL_TRY_THROW( sBigint > 0, RAMP_ERR_INVALID_SEGMENT_MINSIZE );
        
        aSegAttr->mValidFlags |= SML_SEGMENT_MINSIZE_YES;
        aSegAttr->mMinSize = sBigint;
    }
    else
    {
        aSegAttr->mValidFlags |= SML_SEGMENT_MINSIZE_NO;
    }

    /**
     * Segment MAXSIZE
     */

    if ( aSegMaxSize != NULL )
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(aSegMaxSize->mSize),
                           stlStrlen( QLP_GET_PTR_VALUE(aSegMaxSize->mSize) ),
                           & sBigint,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_SEGMENT_MAXSIZE );
        sBigint *= QLP_GET_BYTES_FROM_SIZE_UNIT( QLP_GET_INT_VALUE( aSegMaxSize->mSizeUnit ) );

        STL_TRY_THROW( sBigint > 0, RAMP_ERR_INVALID_SEGMENT_MAXSIZE );

        sMaxSize = smlSegmentMaxSize( aSpacePhyID );
        STL_TRY_THROW( sBigint <= sMaxSize, RAMP_ERR_INVALID_SEGMENT_MAXSIZE );
        
        if ( aSegMinSize != NULL )
        {
            STL_TRY_THROW( sBigint >= aSegAttr->mMinSize,
                           RAMP_ERR_INVALID_SEGMENT_EXTENT_MINMAX );
        }
        
        aSegAttr->mValidFlags |= SML_SEGMENT_MAXSIZE_YES;
        aSegAttr->mMaxSize = sBigint;
    }
    else
    {
        aSegAttr->mValidFlags |= SML_SEGMENT_MAXSIZE_NO;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_SEGMENT_INITIAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_INITIAL_STORAGE_OPTION,
                      qlgMakeErrPosString( aSQLString,
                                           aSegInitialSize->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_INVALID_SEGMENT_NEXT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_NEXT_STORAGE_OPTION,
                      qlgMakeErrPosString( aSQLString,
                                           aSegNextSize->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_INVALID_SEGMENT_MINSIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_MINSIZE_STORAGE_OPTION,
                      qlgMakeErrPosString( aSQLString,
                                           aSegMinSize->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_INVALID_SEGMENT_MAXSIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_MAXSIZE_STORAGE_OPTION,
                      qlgMakeErrPosString( aSQLString,
                                           aSegMaxSize->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_INVALID_SEGMENT_EXTENT_MINMAX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_MINMAX_STORAGE_OPTION,
                      qlgMakeErrPosString( aSQLString,
                                           aSegMaxSize->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    
    STL_FINISH;

    return STL_FAILURE;
}





/**
 * @brief 구문 수행을 위해 Column 관련 Dictionary 정보를 추가한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aTableOwnerID     Table Owner ID
 * @param[in]  aTableSchemaID    Table Schema ID
 * @param[in]  aTableID          Table ID
 * @param[in]  aTableType        Table Type
 * @param[in]  aCodeColumn       Column 의 Init Plan
 * @param[in]  aAddColumnDesc    Add Column Desc
 * @param[out] aColumnID         Column ID
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus qlrExecuteAddColumnDesc( smlTransId           aTransID,
                                   smlStatement       * aStmt,
                                   stlInt64             aTableOwnerID,
                                   stlInt64             aTableSchemaID,
                                   stlInt64             aTableID,
                                   ellTableType         aTableType,
                                   qlrInitColumn      * aCodeColumn,
                                   ellAddColumnDesc   * aAddColumnDesc,
                                   stlInt64           * aColumnID,
                                   qllEnv             * aEnv )
{
    stlInt64  sSeqPhysicalID = 0;
    void    * sSeqHandle = NULL;

    stlInt64  sColumnID = ELL_DICT_OBJECT_ID_NA;

    qlvInitTypeDef  sRefineColumnType;
    

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableOwnerID > ELL_DICT_OBJECT_ID_NA, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableSchemaID > ELL_DICT_OBJECT_ID_NA, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID > ELL_DICT_OBJECT_ID_NA, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodeColumn != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnID != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAddColumnDesc != NULL, QLL_ERROR_STACK(aEnv) );
    
        
    /**
     * Identity Column 일 경우 SEQUENCE 생성
     */
    
    if ( aCodeColumn->mIsIdentity == STL_TRUE )
    {
        STL_TRY( smlCreateSequence( aStmt,
                                    & aCodeColumn->mIdentityAttr,
                                    & sSeqPhysicalID,
                                    & sSeqHandle,
                                    SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        sSeqPhysicalID = 0;
        sSeqHandle = NULL;
    }
    
    STL_TRY( ellInsertColumnDesc( aTransID,
                                  aStmt,
                                  aTableOwnerID,
                                  aTableSchemaID,
                                  aTableID,
                                  aTableType,
                                  & sColumnID,
                                  aCodeColumn->mColumnName,
                                  aCodeColumn->mPhysicalOrdinalPosition,
                                  aCodeColumn->mLogicalOrdinalPosition,
                                  aCodeColumn->mColumnDefault,
                                  aCodeColumn->mIsNullable,
                                  aCodeColumn->mIsIdentity,
                                  aCodeColumn->mIdentityOption,
                                  aCodeColumn->mIdentityAttr.mStartWith,
                                  aCodeColumn->mIdentityAttr.mIncrementBy,
                                  aCodeColumn->mIdentityAttr.mMaxValue,
                                  aCodeColumn->mIdentityAttr.mMinValue,
                                  aCodeColumn->mIdentityAttr.mCycle,
                                  SML_MEMORY_DICT_SYSTEM_TBS_ID,
                                  sSeqPhysicalID,
                                  aCodeColumn->mIdentityAttr.mCacheSize,
                                  STL_TRUE,  /* Updatable Column */
                                  NULL,
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Column Data Type Descriptor 추가
     */

    switch ( aCodeColumn->mTypeDef.mDBType )
    {
        case DTL_TYPE_BOOLEAN:
            {
                STL_TRY( ellInsertBooleanColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NATIVE_SMALLINT:
            {
                STL_TRY( ellInsertSmallintColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NATIVE_INTEGER:
            {
                STL_TRY( ellInsertIntegerColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NATIVE_BIGINT:
            {
                STL_TRY( ellInsertBigintColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NATIVE_REAL:
            {
                STL_TRY( ellInsertRealColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NATIVE_DOUBLE:
            {
                STL_TRY( ellInsertDoubleColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_FLOAT:
            {
                STL_TRY( ellInsertFloatColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             aCodeColumn->mTypeDef.mNumericRadix,
                             aCodeColumn->mTypeDef.mArgNum1,
                             aCodeColumn->mTypeDef.mArgNum2,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NUMBER:
            {
                STL_TRY( ellInsertNumericColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             aCodeColumn->mTypeDef.mNumericRadix,
                             aCodeColumn->mTypeDef.mArgNum1,
                             aCodeColumn->mTypeDef.mArgNum2,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_DECIMAL:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case DTL_TYPE_CHAR:
            {
                STL_TRY( ellInsertCharColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             aCodeColumn->mTypeDef.mStringLengthUnit,
                             aCodeColumn->mTypeDef.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_VARCHAR:
            {
                STL_TRY( ellInsertVarcharColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             aCodeColumn->mTypeDef.mStringLengthUnit,
                             aCodeColumn->mTypeDef.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_LONGVARCHAR:
            {
                STL_TRY( ellInsertLongVarcharColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_CLOB:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case DTL_TYPE_BINARY:
            {
                STL_TRY( ellInsertBinaryColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             aCodeColumn->mTypeDef.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_VARBINARY:
            {
                STL_TRY( ellInsertVarBinaryColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             aCodeColumn->mTypeDef.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_LONGVARBINARY:
            {
                STL_TRY( ellInsertLongVarBinaryColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_BLOB:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case DTL_TYPE_DATE:
            {
                STL_TRY( ellInsertDateColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_TIME:
            {
                STL_TRY( ellInsertTimeColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             aCodeColumn->mTypeDef.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_TIMESTAMP:
            {
                STL_TRY( ellInsertTimestampColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             aCodeColumn->mTypeDef.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
            {
                STL_TRY( ellInsertTimeWithZoneColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             aCodeColumn->mTypeDef.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                STL_TRY( ellInsertTimestampWithZoneColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             aCodeColumn->mTypeDef.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            {
                STL_TRY( ellInsertIntervalYearToMonthColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             aCodeColumn->mTypeDef.mIntervalIndicator,
                             aCodeColumn->mTypeDef.mArgNum1,
                             aCodeColumn->mTypeDef.mArgNum2,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                STL_TRY( ellInsertIntervalDayToSecondColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             aCodeColumn->mTypeDef.mIntervalIndicator,
                             aCodeColumn->mTypeDef.mArgNum1,
                             aCodeColumn->mTypeDef.mArgNum2,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_ROWID:
            {
                STL_TRY( ellInsertRowIdColumnTypeDesc(
                             aTransID,
                             aStmt,
                             aTableOwnerID,
                             aTableSchemaID,
                             aTableID,
                             sColumnID,
                             aCodeColumn->mTypeDef.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                             
                break;
            }
        default:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
    }

    /**
     * Output 설정
     */

    qlrSetRefineColumnType( & aCodeColumn->mTypeDef,
                            & sRefineColumnType );
    
    aAddColumnDesc->mTableID           = aTableID;
    aAddColumnDesc->mOrdinalPosition   = aCodeColumn->mPhysicalOrdinalPosition;
    aAddColumnDesc->mDataType          = sRefineColumnType.mDBType;
    aAddColumnDesc->mArgNum1           = sRefineColumnType.mArgNum1;
    aAddColumnDesc->mArgNum2           = sRefineColumnType.mArgNum2;
    aAddColumnDesc->mStringLengthUnit  = sRefineColumnType.mStringLengthUnit;
    aAddColumnDesc->mIntervalIndicator = sRefineColumnType.mIntervalIndicator;
    aAddColumnDesc->mIdentityHandle    = sSeqHandle;
    
    *aColumnID = sColumnID;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlrExecuteAddColumnDesc()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Key Constraint 를 위한 Index 를 생성한다.
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aDBCStmt            DBC Statement
 * @param[in]  aTableOwnerID       Table Owner ID
 * @param[in]  aTableSchemaID      Table Schema ID
 * @param[in]  aTableID            Table ID
 * @param[in]  aTablePhyHandle     Table Physical Handle
 * @param[in]  aCodeConst          Key Constraint Init Plan
 * @param[in]  aKeyColumnID        Key Column ID s
 * @param[in]  aKeyColumnPos       Key Column Position
 * @param[in]  aNewColumnDesc      New Column Desc
 * @param[out] aIndexID            생성된 Index ID
 * @param[in]  aEnv                Environemnt
 * @remarks
 */
stlStatus qlrExecuteAddKeyIndex( smlTransId         aTransID,
                                 smlStatement     * aStmt,
                                 qllDBCStmt       * aDBCStmt,
                                 stlInt64           aTableOwnerID,
                                 stlInt64           aTableSchemaID,
                                 stlInt64           aTableID,
                                 void             * aTablePhyHandle,
                                 qlrInitConst     * aCodeConst,
                                 stlInt64         * aKeyColumnID,
                                 stlInt32         * aKeyColumnPos,
                                 ellAddColumnDesc * aNewColumnDesc,
                                 stlInt64         * aIndexID,
                                 qllEnv           * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;

    stlInt64   sIndexID = ELL_DICT_OBJECT_ID_NA;
    stlInt64   sIndexPhyID = 0;
    void     * sIndexPhyHandle = NULL;
        
    knlValueBlockList * sIndexValueList = NULL;

    ellIndexColumnOrdering      sKeyColOrder  = ELL_INDEX_COLUMN_ORDERING_DEFAULT;
    ellIndexColumnNullsOrdering sKeyNullOrder = ELL_INDEX_COLUMN_NULLS_ORDERING_DEFAULT;
    
    stlInt32  i = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableOwnerID > ELL_DICT_OBJECT_ID_NA, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableSchemaID > ELL_DICT_OBJECT_ID_NA, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID > ELL_DICT_OBJECT_ID_NA, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTablePhyHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodeConst != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyColumnID != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyColumnPos != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNewColumnDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexID != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * Build 를 위한 공간 확보
     * - 공유할 필요가 없는 Execution Memory 를 사용한다.
     */
    
    STL_TRY( ellAllocIndexValueListForCREATE( aTransID,
                                              ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                              aCodeConst->mColumnCnt,
                                              NULL,
                                              aKeyColumnPos,
                                              aNewColumnDesc,
                                              & QLL_EXEC_DDL_MEM(aEnv),
                                              & sIndexValueList,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Index 생성
     */
    
    STL_TRY( smlCreateIndex( aStmt,
                             ellGetTablespaceID( & aCodeConst->mIndexSpaceHandle ),
                             SML_INDEX_TYPE_MEMORY_PERSISTENT_BTREE,
                             & aCodeConst->mIndexAttr,
                             aTablePhyHandle,
                             aCodeConst->mColumnCnt,
                             sIndexValueList,
                             aCodeConst->mIndexKeyFlag,
                             & sIndexPhyID,
                             & sIndexPhyHandle,
                             SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Index Descriptor 추가
     */

    STL_TRY( ellInsertIndexDesc( aTransID,
                                 aStmt,
                                 ellGetAuthID( sAuthHandle ),
                                 ellGetSchemaID( & aCodeConst->mConstSchemaHandle ),
                                 & sIndexID,
                                 ellGetTablespaceID( & aCodeConst->mIndexSpaceHandle ),
                                 sIndexPhyID,
                                 aCodeConst->mIndexName,
                                 ELL_INDEX_TYPE_BTREE,
                                 aCodeConst->mIndexAttr.mUniquenessFlag,
                                 STL_FALSE,  /* invalid */
                                 STL_TRUE,   /* by key constraints */
                                 NULL,
                                 ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Index Key Table Usage 추가
     */

    STL_TRY( ellInsertIndexKeyTableUsageDesc(
                 aTransID,
                 aStmt,
                 ellGetAuthID( sAuthHandle ),
                 ellGetSchemaID( & aCodeConst->mConstSchemaHandle ),
                 sIndexID,
                 aTableOwnerID,
                 aTableSchemaID,
                 aTableID,
                 ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Index Key Column Usage 추가
     */

    for ( i = 0; i < aCodeConst->mColumnCnt; i++ )
    {
        if ( (aCodeConst->mIndexKeyFlag[i] & DTL_KEYCOLUMN_INDEX_ORDER_MASK)
             == DTL_KEYCOLUMN_INDEX_ORDER_ASC )
        {
            sKeyColOrder = ELL_INDEX_COLUMN_ASCENDING;
        }
        else
        {
            sKeyColOrder = ELL_INDEX_COLUMN_DESCENDING;
        }

        if ( (aCodeConst->mIndexKeyFlag[i] & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
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
                     ellGetSchemaID( & aCodeConst->mConstSchemaHandle ),
                     sIndexID,
                     aTableOwnerID,
                     aTableSchemaID,
                     aTableID,
                     aKeyColumnID[i],
                     i,
                     sKeyColOrder,
                     sKeyNullOrder,
                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**
     * Index 구축
     */
    
    STL_TRY( smlBuildIndex( aStmt,
                            sIndexPhyHandle,
                            aTablePhyHandle,
                            aCodeConst->mColumnCnt,
                            sIndexValueList,
                            aCodeConst->mIndexKeyFlag,
                            aCodeConst->mIndexParallel,
                            SML_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Output 설정 
     */

    *aIndexID = sIndexID;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief CREATE TABLE 구문내의 Key Constraint 를 추가한다.
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aTableOwnerID       Table Owner ID
 * @param[in]  aTableSchemaID      Table Schema ID
 * @param[in]  aTableID            Table ID
 * @param[in]  aCodeConst          Key Constraint Init Plan
 * @param[in]  aKeyIndexID         Key Constraint 를 위한 Index ID
 * @param[in]  aKeyColumnID        Key Column IDs
 * @param[in]  aEnv                Environemnt
 * @remarks
 */
stlStatus qlrExecuteAddKeyConst( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 stlInt64        aTableOwnerID,
                                 stlInt64        aTableSchemaID,
                                 stlInt64        aTableID,
                                 qlrInitConst  * aCodeConst,
                                 stlInt64        aKeyIndexID,
                                 stlInt64      * aKeyColumnID,
                                 qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    stlInt64  sKeyConstID = ELL_DICT_OBJECT_ID_NA;

    stlInt32  sRefUniqueKeyColIdx = 0;
    
    stlInt32  i = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableOwnerID > ELL_DICT_OBJECT_ID_NA, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableSchemaID > ELL_DICT_OBJECT_ID_NA, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID > ELL_DICT_OBJECT_ID_NA, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodeConst != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyIndexID > ELL_DICT_OBJECT_ID_NA,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyColumnID != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /**
     * Table Constraint Descriptor 추가
     */
    
    STL_TRY( ellInsertTableConstraintDesc(
                 aTransID,
                 aStmt,
                 ellGetAuthID( sAuthHandle ),
                 ellGetSchemaID( & aCodeConst->mConstSchemaHandle ),
                 & sKeyConstID,
                 aTableOwnerID,
                 aTableSchemaID,
                 aTableID,
                 aCodeConst->mConstName,
                 aCodeConst->mConstType,
                 aCodeConst->mDeferrable,
                 aCodeConst->mInitDeferred,
                 aCodeConst->mEnforced,
                 aKeyIndexID,
                 NULL,
                 ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Table Constraint 의 Key Column Usage Descriptor 추가
     */

    for ( i = 0; i < aCodeConst->mColumnCnt; i++ )
    {
        /**
         * @todo Foreign Key 의 경우, Unique Key 의 Key Column 위치 정보를 설정해야 함.
         */
        sRefUniqueKeyColIdx = ELL_DICT_POSITION_NA;
        
        STL_TRY( ellInsertKeyColumnUsageDesc(
                     aTransID,
                     aStmt,
                     ellGetAuthID( sAuthHandle ),
                     ellGetSchemaID( & aCodeConst->mConstSchemaHandle ),
                     sKeyConstID,
                     aTableOwnerID,
                     aTableSchemaID,
                     aTableID,
                     aKeyColumnID[i],
                     i,
                     sRefUniqueKeyColIdx,
                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief CREATE TABLE 구문내의 Check Constraint 를 추가한다.
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aTableOwnerID       Table Owner ID
 * @param[in]  aTableSchemaID      Table Schema ID
 * @param[in]  aTableID            Table ID
 * @param[in]  aCodeConst          Check Constraint Init Plan
 * @param[in]  aCheckColumnID      Check Column ID
 * @param[in]  aEnv                Environemnt
 * @remarks
 */
stlStatus qlrExecuteAddCheckConst( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   stlInt64        aTableOwnerID,
                                   stlInt64        aTableSchemaID,
                                   stlInt64        aTableID,
                                   qlrInitConst  * aCodeConst,
                                   stlInt64        aCheckColumnID,
                                   qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    stlInt64  sCheckConstID = ELL_DICT_OBJECT_ID_NA;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableOwnerID > ELL_DICT_OBJECT_ID_NA, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableSchemaID > ELL_DICT_OBJECT_ID_NA, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID > ELL_DICT_OBJECT_ID_NA, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodeConst != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCheckColumnID > ELL_DICT_OBJECT_ID_NA,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */
    
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * Table Constraint Descriptor 추가
     */
        
    STL_TRY( ellInsertTableConstraintDesc(
                 aTransID,
                 aStmt,
                 ellGetAuthID( sAuthHandle ),
                 ellGetSchemaID( & aCodeConst->mConstSchemaHandle ),
                 & sCheckConstID,
                 aTableOwnerID,
                 aTableSchemaID,
                 aTableID,
                 aCodeConst->mConstName,
                 aCodeConst->mConstType,
                 aCodeConst->mDeferrable,
                 aCodeConst->mInitDeferred,
                 aCodeConst->mEnforced,
                 ELL_DICT_OBJECT_ID_NA, /* 관련 index 가 없음 */
                 NULL,
                 ELL_ENV(aEnv) )
             == STL_SUCCESS );
                     
    /**
     * Check Constraint Descriptor 추가
     * - CHECK NOT NULL 은 구문을 저장하지 않음.
     */
    STL_TRY( ellInsertCheckConstraintDesc(
                 aTransID,
                 aStmt,
                 ellGetAuthID( sAuthHandle ),
                 ellGetSchemaID( & aCodeConst->mConstSchemaHandle ),
                 sCheckConstID,
                 aCodeConst->mCheckClause,   
                 ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Check Table Usage Descriptor 추가
     */

    STL_TRY( ellInsertCheckTableUsageDesc(
                 aTransID,
                 aStmt,
                 ellGetAuthID( sAuthHandle ),
                 ellGetSchemaID( & aCodeConst->mConstSchemaHandle ),
                 sCheckConstID,
                 aTableOwnerID,
                 aTableSchemaID,
                 aTableID,
                 ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Check Column Usage Descriptor 추가
     */

    STL_TRY( ellInsertCheckColumnUsageDesc(
                 aTransID,
                 aStmt,
                 ellGetAuthID( sAuthHandle ),
                 ellGetSchemaID( & aCodeConst->mConstSchemaHandle ),
                 sCheckConstID,
                 aTableOwnerID,
                 aTableSchemaID,
                 aTableID,
                 aCheckColumnID,
                 ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}                                               

/**
 * @brief CREATE TABLE과 CREATE TABLE AS SELECT 공통 Execution
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrExecuteCreateTableCommon( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv )
{
    ellDictHandle      * sAuthHandle = NULL;
    
    qlrInitCreateTable * sInitPlan = NULL;
    qlrDataCreateTable * sDataPlan = NULL;

    qlrInitColumn * sCodeColumn = NULL;
    qlrInitConst  * sCodeConst = NULL;

    stlInt64   sCheckColumnID = ELL_DICT_OBJECT_ID_NA;
    
    stlInt64   sKeyIndexID = ELL_DICT_OBJECT_ID_NA;
    
    stlInt64   sKeyColumnID[DTL_INDEX_COLUMN_MAX_COUNT];

    stlBool    sTableExist = STL_FALSE;
    
    stlInt32     i = 0;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aSQLStmt->mStmtType == QLL_STMT_CREATE_TABLE_TYPE ) ||
                        ( aSQLStmt->mStmtType == QLL_STMT_CREATE_TABLE_AS_SELECT_TYPE ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * 기본 정보 설정 
     ***************************************************************/
    
    sInitPlan = (qlrInitCreateTable *) aSQLStmt->mInitPlan;
    sDataPlan = (qlrDataCreateTable *) aSQLStmt->mDataPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**********************************************************
     * Table Definition
     **********************************************************/

    /**
     * Table 상위 객체 정보 설정
     */

    sDataPlan->mTableOwnerID  = ellGetAuthID( sAuthHandle );
    sDataPlan->mTableSchemaID = ellGetSchemaID( & sInitPlan->mSchemaHandle );
    sDataPlan->mTableSpaceID  = ellGetTablespaceID( & sInitPlan->mSpaceHandle );
    
    /**
     * Table 생성
     */

    STL_TRY( smlCreateTable( aStmt,
                             sDataPlan->mTableSpaceID,
                             sInitPlan->mTableType,         /* row-based or column-based */
                             & sInitPlan->mPhysicalAttr,
                             & sDataPlan->mTablePhyID,
                             & sDataPlan->mTablePhyHandle,
                             SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Table Descriptor 추가
     */

    STL_TRY( ellInsertTableDesc( aTransID,
                                 aStmt,
                                 sDataPlan->mTableOwnerID,
                                 sDataPlan->mTableSchemaID,
                                 & sDataPlan->mTableID,
                                 sDataPlan->mTableSpaceID,
                                 sDataPlan->mTablePhyID,
                                 sInitPlan->mTableName,
                                 ELL_TABLE_TYPE_BASE_TABLE,
                                 NULL,
                                 ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Column 정보를 Dictionary 에 추가
     */

    for ( sCodeColumn = sInitPlan->mColumnList, i = 0;
          sCodeColumn != NULL;
          sCodeColumn = sCodeColumn->mNext, i++ )
    {
        STL_TRY( qlrExecuteAddColumnDesc( aTransID,
                                          aStmt,
                                          sDataPlan->mTableOwnerID,
                                          sDataPlan->mTableSchemaID,
                                          sDataPlan->mTableID,
                                          ELL_TABLE_TYPE_BASE_TABLE,
                                          sCodeColumn,
                                          & sDataPlan->mAddColumnDesc[i],
                                          & sDataPlan->mColumnID[i],
                                          aEnv )
                 == STL_SUCCESS );

    }
                             
    /**********************************************************
     * Primary Key Definition
     **********************************************************/

    if ( sInitPlan->mAllConstraint.mPrimaryKey == NULL )
    {
        /* Primary Key 정의가 없음 */
    }
    else
    {
        /**
         * Key Column 정보 구축
         */
        
        sCodeConst = sInitPlan->mAllConstraint.mPrimaryKey;

        for ( i = 0; i < sCodeConst->mColumnCnt; i++ )
        {
            sKeyColumnID[i]  = sDataPlan->mColumnID[sCodeConst->mColumnIdx[i]];
        }
        
        /**
         * Primary Key Index 생성 
         */

        STL_TRY( qlrExecuteAddKeyIndex( aTransID,
                                        aStmt,
                                        aDBCStmt,
                                        sDataPlan->mTableOwnerID,
                                        sDataPlan->mTableSchemaID,
                                        sDataPlan->mTableID,
                                        sDataPlan->mTablePhyHandle,
                                        sCodeConst,
                                        sKeyColumnID,
                                        sCodeConst->mColumnIdx,
                                        sDataPlan->mAddColumnDesc,
                                        & sKeyIndexID,
                                        aEnv )
                 == STL_SUCCESS );

        /**
         * Primary Key Constraint 추가
         */
        
        STL_TRY( qlrExecuteAddKeyConst( aTransID,
                                        aStmt,
                                        sDataPlan->mTableOwnerID,
                                        sDataPlan->mTableSchemaID,
                                        sDataPlan->mTableID,
                                        sCodeConst,
                                        sKeyIndexID,
                                        sKeyColumnID,
                                        aEnv )
                 == STL_SUCCESS );
    }

    /**********************************************************
     * Unique Key Definition
     **********************************************************/

    for ( sCodeConst = sInitPlan->mAllConstraint.mUniqueKey;
          sCodeConst != NULL;
          sCodeConst = sCodeConst->mNext )
    {
        /**
         * Key Column 정보 구축
         */
        
        for ( i = 0; i < sCodeConst->mColumnCnt; i++ )
        {
            sKeyColumnID[i]  = sDataPlan->mColumnID[sCodeConst->mColumnIdx[i]];
        }
        
        /**
         * Unique Key Index 생성 
         */

        STL_TRY( qlrExecuteAddKeyIndex( aTransID,
                                        aStmt,
                                        aDBCStmt,
                                        sDataPlan->mTableOwnerID,
                                        sDataPlan->mTableSchemaID,
                                        sDataPlan->mTableID,
                                        sDataPlan->mTablePhyHandle,
                                        sCodeConst,
                                        sKeyColumnID,
                                        sCodeConst->mColumnIdx,
                                        sDataPlan->mAddColumnDesc,
                                        & sKeyIndexID,
                                        aEnv )
                 == STL_SUCCESS );

        /**
         * Unique Key Constraint 추가
         */
        
        STL_TRY( qlrExecuteAddKeyConst( aTransID,
                                        aStmt,
                                        sDataPlan->mTableOwnerID,
                                        sDataPlan->mTableSchemaID,
                                        sDataPlan->mTableID,
                                        sCodeConst,
                                        sKeyIndexID,
                                        sKeyColumnID,
                                        aEnv )
                 == STL_SUCCESS );
    }

    /**********************************************************
     * Foreign Key Definition
     **********************************************************/

    for ( sCodeConst = sInitPlan->mAllConstraint.mUniqueKey;
          sCodeConst != NULL;
          sCodeConst = sCodeConst->mNext )
    {
        /**
         * @todo Foreign Key Constraint 를 구현해야 함
         */
    }

    /**********************************************************
     * Check Not Null Definition
     **********************************************************/

    for ( sCodeConst = sInitPlan->mAllConstraint.mCheckNotNull;
          sCodeConst != NULL;
          sCodeConst = sCodeConst->mNext )
    {
        /**
         * Check Column 정보 구축
         * - Check Constraint 는 하나의 컬럼에만 해당됨 
         */

        sCheckColumnID = sDataPlan->mColumnID[sCodeConst->mColumnIdx[0]];

        /**
         * Check Constraint 추가
         */

        STL_TRY( qlrExecuteAddCheckConst( aTransID,
                                          aStmt,
                                          sDataPlan->mTableOwnerID,
                                          sDataPlan->mTableSchemaID,
                                          sDataPlan->mTableID,
                                          sCodeConst,
                                          sCheckColumnID,
                                          aEnv )
                 == STL_SUCCESS );
    }
    
    /**********************************************************
     * Check Clause Definition
     **********************************************************/

    for ( sCodeConst = sInitPlan->mAllConstraint.mCheckClause;
          sCodeConst != NULL;
          sCodeConst = sCodeConst->mNext )
    {
        /**
         * Check Column 정보 구축
         * - Check Constraint 는 하나의 컬럼에만 해당됨 
         */

        sCheckColumnID = sDataPlan->mColumnID[sCodeConst->mColumnIdx[0]];

        /**
         * Check Constraint 추가
         */

        STL_TRY( qlrExecuteAddCheckConst( aTransID,
                                          aStmt,
                                          sDataPlan->mTableOwnerID,
                                          sDataPlan->mTableSchemaID,
                                          sDataPlan->mTableID,
                                          sCodeConst,
                                          sCheckColumnID,
                                          aEnv )
                 == STL_SUCCESS );
    }

    /**********************************************************
     * SQL-Table Cache 구축 
     **********************************************************/

    STL_TRY( ellRefineCache4CreateTable( aTransID,
                                         aStmt,
                                         sDataPlan->mTableID,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * 생성한 객체에 대해 Lock 을 획득
     * 
     * commit -> pending -> trans end (unlock) 과정 중에
     * 해당 객체에 대한 DROP, ALTER 가 수행될 경우를 방지하기 위해
     * 생성한 객체에 대한 lock 을 획득해야 함.
     */

    STL_TRY( smlLockTable( aTransID,
                           SML_LOCK_X,
                           sDataPlan->mTablePhyHandle,
                           SML_LOCK_TIMEOUT_PROPERTY,
                           &sTableExist,
                           SML_ENV(aEnv) )
             == STL_SUCCESS );
    STL_DASSERT( sTableExist == STL_TRUE );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}                                               




/** @} qlrCreateTable */
