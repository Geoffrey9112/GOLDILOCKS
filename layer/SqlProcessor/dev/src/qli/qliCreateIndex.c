/*******************************************************************************
 * qliCreateIndex.c
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
 * @file qliCreateIndex.c
 * @brief CREATE INDEX 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qliCreateIndex CREATE INDEX
 * @ingroup qli
 * @{
 */


/**
 * @brief CREATE INDEX 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 * 생성하는 인덱스가 동일한 Key Column Spec일 경우 생성 가능 여부는 다음과 같다.
 * - Oracle 과 달리, GOLDILOCKS 는 User Index 와 Constraint Index 가 무관하다.
 * 
 * - 동일한 Key Column Spec : 동일한 컬럼, 동일한 Asc/Desc, 동일한 Null First/Last
 * - Legend 
 *  - O : 가능함
 *  - X : 가능함
 *  
 * <table>
 *     <caption align="TOP"> 인덱스 생성 가능 여부 </caption>
 *     <tr align="center">
 *         <th> 기존 인덱스 </th>
 *         <th> Primary Key       </th>
 *         <th> Unique Key        </th>
 *         <th> Foreign Key       </th>
 *         <th> Unique Index      </th>
 *         <th> Non-Key Index     </th>
 *     </tr>
 *     <tr align="center">
 *         <th> 생성할 인덱스 </th>
 *         <td>     -      </td>
 *         <td>     -      </td>
 *         <td>     -      </td>
 *         <td>     -      </td>
 *         <td>     -      </td>
 *     </tr>
 *     <tr align="center">
 *         <td> Primary Key  </td>
 *         <td>     X        </td>
 *         <td>     X        </td>
 *         <td>     O        </td>
 *         <td>     O        </td>
 *         <td>     O        </td>
 *     </tr>
 *     <tr align="center">
 *         <td> Unique Key   </td>
 *         <td>     X        </td>
 *         <td>     X        </td>
 *         <td>     O        </td>
 *         <td>     O        </td>
 *         <td>     O        </td>
 *     </tr>
 *     <tr align="center">
 *         <td> Foreign Key(diff parent) </td>
 *         <td>     O        </td>
 *         <td>     O        </td>
 *         <td>     O        </td>
 *         <td>     O        </td>
 *         <td>     O        </td>
 *     </tr>
 *     <tr align="center">
 *         <td>  Unique Index  </td>
 *         <td>     O        </td>
 *         <td>     O        </td>
 *         <td>     O        </td>
 *         <td>     X        </td>
 *         <td>     X        </td>
 *     </tr>
 *     <tr align="center">
 *         <td> Non-Unique Index </td>
 *         <td>     O        </td>
 *         <td>     O        </td>
 *         <td>     O        </td>
 *         <td>     X        </td>
 *         <td>     X        </td>
 *     </tr>
 * </table>
 *
 * - Key Constraint 또는 Create Index 수행시 동일한 Key Column Spec 에 대한
 *   Index 생성 가능여부는 다음과 같이 판단한다.
 *  - Primary/Unique Key
 *   - Key Column Spec 을 기존의 Primary/Unique Key 범위에서만 검사
 *   - Column List 로 판단.
 *  - Foreign Key
 *   - Key Column Spec 을 기존의 Foreign Key 에서만 검사
 *   - Column List, Parent Key 로 판단.
 *  - Unique Index / Non-Unique Index
 *   - 모든 범위에서 Key Column Spec 을 검사
 *   - Column List, Asc/Desc, Nulls First/Last 로 판단 
 */
stlStatus qliValidateCreateIndex( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;

    qlpIndexDef        * sParseTree = NULL;
    qliInitCreateIndex * sInitPlan = NULL;

    stlInt64             sBigintValue = 0;
    stlBool              sLoggingFlag = STL_FALSE;

    stlBool sObjectExist = STL_FALSE;

    ellDictHandle sIndexHandle;
    ellDictHandle    sKeyHandle;
    
    ellInitDictHandle( & sIndexHandle );
    ellInitDictHandle( & sKeyHandle );
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_INDEX_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_CREATE_INDEX_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    sParseTree = (qlpIndexDef *) aParseTree;
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qliInitCreateIndex),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qliInitCreateIndex) );

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
    
    if ( sParseTree->mName->mSchema == NULL )
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
                                         sParseTree->mName->mSchema,
                                         & sInitPlan->mSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXIST );
    }
    

    /**
     * Wriable Schema 인지 검사
     */
    
    STL_TRY_THROW( ellGetSchemaWritable( & sInitPlan->mSchemaHandle ) == STL_TRUE,
                   RAMP_ERR_SCHEMA_NOT_WRITABLE );

     /**
     * CREATE INDEX ON SCHEMA 권한 검사
     */

    STL_TRY( qlaCheckSchemaPriv( aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 ELL_SCHEMA_PRIV_ACTION_CREATE_INDEX,
                                 & sInitPlan->mSchemaHandle,
                                 aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Index Validation
     **********************************************************/
    
    /**
     * Index 존재 여부 확인
     */

    STL_TRY( ellGetIndexDictHandleWithSchema( aTransID,
                                              aSQLStmt->mViewSCN,
                                              & sInitPlan->mSchemaHandle,
                                              sParseTree->mName->mObject,
                                              & sIndexHandle,
                                              & sObjectExist,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sObjectExist != STL_TRUE, RAMP_ERR_SAME_NAME_INDEX_EXIST );

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                stlStrlen( sParseTree->mName->mObject ) + 1,
                                (void **) & sInitPlan->mIndexName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sInitPlan->mIndexName, sParseTree->mName->mObject );

    
    /**********************************************************
     * Table Validation
     **********************************************************/
    
    if ( sParseTree->mRelInfo->mName->mSchema == NULL )
    {
        /**
         * Schema 가 명시되지 않은 경우
         */

        STL_TRY( ellGetTableDictHandleWithAuth( aTransID,
                                                aSQLStmt->mViewSCN,
                                                sAuthHandle,
                                                sParseTree->mRelInfo->mName->mObject,
                                                & sInitPlan->mTableHandle,
                                                & sObjectExist,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXIST );

        /**
         * Table 의 Schema Handle 획득
         */
        
        STL_TRY( ellGetSchemaDictHandleByID(
                     aTransID,
                     aSQLStmt->mViewSCN,
                     ellGetTableSchemaID( & sInitPlan->mTableHandle ),
                     & sInitPlan->mTableSchemaHandle,
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
                                         sParseTree->mRelInfo->mName->mSchema,
                                         & sInitPlan->mTableSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_SCHEMA_NOT_EXIST );
        
        STL_TRY( ellGetTableDictHandleWithSchema( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  & sInitPlan->mTableSchemaHandle,
                                                  sParseTree->mRelInfo->mName->mObject,
                                                  & sInitPlan->mTableHandle,
                                                  & sObjectExist,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXIST );
    }

    /**
     * Built-In Table 인지 검사
     */

    STL_TRY_THROW( ellIsBuiltInTable( & sInitPlan->mTableHandle ) == STL_FALSE,
                   RAMP_ERR_BUILT_IN_TABLE );

    /**
     * View 에는 인덱스를 구성할 수 없다.
     */

    switch( ellGetTableType( & sInitPlan->mTableHandle ) )
    {
        case ELL_TABLE_TYPE_BASE_TABLE:
        case ELL_TABLE_TYPE_GLOBAL_TEMPORARY:
        case ELL_TABLE_TYPE_LOCAL_TEMPORARY:
        case ELL_TABLE_TYPE_SYSTEM_VERSIONED:
            {
                /**
                 * Index 생성 가능한 테이블임.
                 */
                break;
            }
        default:
            {
                /**
                 * Index 를 생성할 수 없는 테이블임 (View 등)
                 */
                STL_THROW( RAMP_ERR_TABLE_NOT_WRITABLE );
                break;
            }
    }

    /**
     * INDEX ON TABLE 권한 검사 
     */

    STL_TRY( qlaCheckTablePriv( aTransID,
                                aDBCStmt,
                                aSQLStmt,
                                ELL_TABLE_PRIV_ACTION_INDEX,
                                & sInitPlan->mTableHandle,
                                NULL, /* aColumnList */
                                aEnv )
             == STL_SUCCESS );
                                
    /**********************************************************
     * Index Key Validation
     **********************************************************/

    STL_TRY( qliValidateKeyDefinition( aTransID,
                                       aDBCStmt,
                                       aSQLStmt,
                                       aSQLString,
                                       sInitPlan,
                                       sParseTree->mElems,
                                       aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Tablespace Validation
     **********************************************************/

    /**
     * Index Logging 정보 획득
     */

    qliGetIndexLoggingFlag( aSQLString,
                            & sParseTree->mIndexAttr,
                            & sInitPlan->mPhysicalAttr,
                            & sLoggingFlag,
                            aEnv );
    
    /**
     * Tablespace 존재 여부 확인 
     */

    if ( sParseTree->mTablespaceName == NULL )
    {
        if ( sLoggingFlag == STL_TRUE )
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
            STL_TRY( ellGetAuthTempSpaceHandle( aTransID,
                                                aSQLStmt->mViewSCN,
                                                sAuthHandle,
                                                & sInitPlan->mSpaceHandle,
                                                ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        STL_TRY( ellGetTablespaceDictHandle( aTransID,
                                             aSQLStmt->mViewSCN,
                                             QLP_GET_PTR_VALUE( sParseTree->mTablespaceName ),
                                             & sInitPlan->mSpaceHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLESPACE_NOT_EXIST );
    }
    
    /**
     * DATA Tablespace 인지 검사
     */

    if ( sLoggingFlag == STL_TRUE )
    {
        STL_TRY_THROW( ellGetTablespaceUsageType( & sInitPlan->mSpaceHandle )
                       == ELL_SPACE_USAGE_TYPE_DATA,
                       RAMP_ERR_TABLESPACE_NOT_WRITABLE_FOR_LOGGING_INDEX );
    }
    else
    {
        STL_TRY_THROW( ellGetTablespaceUsageType( & sInitPlan->mSpaceHandle )
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
                                & sInitPlan->mSpaceHandle,
                                aEnv )
             == STL_SUCCESS );

    
    /**********************************************************
     * Physical Attribute Validation
     **********************************************************/

    STL_TRY( qliValidateIndexAttr( aSQLString,
                                   (smlTbsId) ellGetTablespaceID( & sInitPlan->mSpaceHandle ),
                                   & sParseTree->mIndexAttr,
                                   & sInitPlan->mPhysicalAttr,
                                   aEnv )
             == STL_SUCCESS );
             
    /**
     * Unique Index 여부
     */

    sInitPlan->mIsUnique = sParseTree->mIsUnique;
    
    sInitPlan->mPhysicalAttr.mValidFlags |= SML_INDEX_UNIQUENESS_YES;
    
    if ( sInitPlan->mIsUnique == STL_TRUE )
    {
        sInitPlan->mPhysicalAttr.mUniquenessFlag = STL_TRUE;
    }
    else
    {
        sInitPlan->mPhysicalAttr.mUniquenessFlag = STL_FALSE;
    }

    /**
     * Parallel Factor 값
     */

    if ( sParseTree->mIndexAttr.mParallel == NULL )
    {
        sInitPlan->mParallelFactor = 0;
    }
    else
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(sParseTree->mIndexAttr.mParallel),
                           stlStrlen( QLP_GET_PTR_VALUE(sParseTree->mIndexAttr.mParallel) ),
                           & sBigintValue,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_PARALLEL_FACTOR );

        STL_TRY_THROW( (sBigintValue >= 0 ) &&
                       (sBigintValue <= SML_MAX_INDEX_BUILD_PARALLEL_FACTOR),
                       RAMP_ERR_INVALID_PARALLEL_FACTOR );

        sInitPlan->mParallelFactor = (stlUInt16) sBigintValue;
    }

    
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

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mTableSchemaHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mTableHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**********************************************************
     * Init Plan 연결 
     **********************************************************/

    aSQLStmt->mInitPlan = (void *) sInitPlan;
        
    
    return STL_SUCCESS;

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

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_WRITABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_WRITABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sInitPlan->mSchemaHandle ) );
    }

    STL_CATCH( RAMP_ERR_SAME_NAME_INDEX_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USED_OBJECT_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sInitPlan->mSchemaHandle ),
                      sParseTree->mName->mObject );
    }

    STL_CATCH( RAMP_ERR_TABLE_SCHEMA_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mRelInfo->mName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mRelInfo->mName->mSchema );
    }
    
    STL_CATCH( RAMP_ERR_TABLE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_OR_VIEW_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mRelInfo->mName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_BUILT_IN_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_TABLE_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mRelInfo->mName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mRelInfo->mName->mObject );
    }
    
    STL_CATCH( RAMP_ERR_TABLE_NOT_WRITABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_NOT_WRITABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mRelInfo->mName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mRelInfo->mName->mObject );
    }

    STL_CATCH( RAMP_ERR_INVALID_PARALLEL_FACTOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_PARALLEL_FACTOR,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mIndexAttr.mParallel->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      SML_MAX_INDEX_BUILD_PARALLEL_FACTOR );
    }

    STL_CATCH( RAMP_ERR_TABLESPACE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mTablespaceName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mTablespaceName ) );
    }

    STL_CATCH( RAMP_ERR_TABLESPACE_NOT_WRITABLE_FOR_LOGGING_INDEX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_WRITABLE_FOR_LOGGING_INDEX,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mTablespaceName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mTablespaceName ) );
    }

    STL_CATCH( RAMP_ERR_TABLESPACE_NOT_WRITABLE_FOR_NOLOGGING_INDEX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_WRITABLE_FOR_NOLOGGING_INDEX,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mTablespaceName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mTablespaceName ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief CREATE INDEX 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qliOptCodeCreateIndex( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief CREATE INDEX 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qliOptDataCreateIndex( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    qliInitCreateIndex * sInitPlan = NULL;
    qliDataCreateIndex * sDataPlan = NULL;

    stlInt32   i = 0;
    stlInt64   sKeyColumnID[DTL_INDEX_COLUMN_MAX_COUNT] = {0,};
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_INDEX_TYPE,
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
    
    sInitPlan = (qliInitCreateIndex *) aSQLStmt->mInitPlan;
    
    /**
     * Data Plan 생성
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                STL_SIZEOF(qliDataCreateIndex),
                                (void **) & sDataPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sDataPlan, 0x00, STL_SIZEOF(qliDataCreateIndex) );

    /**
     * Index Key Column ID 정보 구축
     */

    for ( i = 0; i < sInitPlan->mKeyCnt; i++ )
    {
        sKeyColumnID[i] = ellGetColumnID( & sInitPlan->mKeyHandle[i] );
    }

    /**
     * Index Value List 공간 확보 
     */

    STL_TRY( ellAllocIndexValueListForCREATE( aTransID,
                                              aSQLStmt->mViewSCN,
                                              sInitPlan->mKeyCnt,
                                              sKeyColumnID,
                                              NULL,   
                                              NULL,
                                              & QLL_DATA_PLAN(aDBCStmt),
                                              & sDataPlan->mValueList,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                              
    /**
     * Data Plan 연결
     */

    aSQLStmt->mDataPlan = sDataPlan;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief CREATE INDEX 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qliExecuteCreateIndex( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qliInitCreateIndex * sInitPlan = NULL;
    qliDataCreateIndex * sDataPlan = NULL;

    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;

    stlInt64   sIndexID = ELL_DICT_OBJECT_ID_NA;
    stlInt64   sIndexPhyID = 0;
    void     * sIndexPhyHandle = NULL;

    stlBool    sSameKey = STL_FALSE;

    stlBool    sObjectExist = STL_FALSE;
    
    ellDictHandle sIndexHandle;
    
    stlInt32 i = 0;

    ellInitDictHandle ( & sIndexHandle );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_INDEX_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mDataPlan != NULL, QLL_ERROR_STACK(aEnv) );

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
    
    sInitPlan = (qliInitCreateIndex *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * CREATE INDEX 구문을 위한 DDL Lock 획득 
     */

    STL_TRY( ellLock4CreateIndex( aTransID,
                                  aStmt,
                                  sAuthHandle,
                                  & sInitPlan->mSpaceHandle,
                                  & sInitPlan->mSchemaHandle,
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
    
    sInitPlan = (qliInitCreateIndex *) aSQLStmt->mInitPlan;
    sDataPlan = (qliDataCreateIndex *) aSQLStmt->mDataPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /***************************************************************
     *  Index 객체 생성
     ***************************************************************/
    
    /**
     * Index 객체 생성
     */

    STL_TRY( smlCreateIndex( aStmt,
                             ellGetTablespaceID( & sInitPlan->mSpaceHandle ),
                             SML_INDEX_TYPE_MEMORY_PERSISTENT_BTREE,
                             & sInitPlan->mPhysicalAttr,
                             ellGetTableHandle( & sInitPlan->mTableHandle ),
                             sInitPlan->mKeyCnt,
                             sDataPlan->mValueList,
                             sInitPlan->mKeyFlag,
                             & sIndexPhyID,
                             & sIndexPhyHandle,
                             SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**************************************************************
     * Dictionary 정보 구축
     **************************************************************/
    
    /**
     * Index Descriptor 추가 
     */
    
    STL_TRY( ellInsertIndexDesc( aTransID,
                                 aStmt,
                                 ellGetAuthID( sAuthHandle ),
                                 ellGetSchemaID( & sInitPlan->mSchemaHandle ),
                                 & sIndexID,
                                 ellGetTablespaceID( & sInitPlan->mSpaceHandle ),
                                 sIndexPhyID,
                                 sInitPlan->mIndexName,
                                 ELL_INDEX_TYPE_BTREE,
                                 sInitPlan->mIsUnique,
                                 STL_FALSE, /* is invalid */
                                 STL_FALSE, /* by constraint */
                                 NULL,      /* comment */
                                 ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Index Key Table Usage descriptor 추가
     */

    STL_TRY( ellInsertIndexKeyTableUsageDesc(
                 aTransID,
                 aStmt,
                 ellGetAuthID( sAuthHandle ),
                 ellGetSchemaID( & sInitPlan->mSchemaHandle ),
                 sIndexID,
                 ellGetTableOwnerID( & sInitPlan->mTableHandle ),
                 ellGetTableSchemaID( & sInitPlan->mTableHandle ),
                 ellGetTableID( & sInitPlan->mTableHandle ),
                 ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Index Key Column Usage descriptor 추가
     */
    
    for ( i = 0; i < sInitPlan->mKeyCnt; i++ )
    {
        STL_TRY( ellInsertIndexKeyColumnUsageDesc(
                     aTransID,
                     aStmt,
                     ellGetAuthID( sAuthHandle ),
                     ellGetSchemaID( & sInitPlan->mSchemaHandle ),
                     sIndexID,
                     ellGetColumnOwnerID( & sInitPlan->mKeyHandle[i] ),
                     ellGetColumnSchemaID( & sInitPlan->mKeyHandle[i] ),
                     ellGetColumnTableID( & sInitPlan->mKeyHandle[i] ),
                     ellGetColumnID( & sInitPlan->mKeyHandle[i] ),
                     i,  /* key order */
                     sInitPlan->mKeyOrder[i],
                     sInitPlan->mKeyNullOrder[i],
                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
                                              
    /**
     * SQL-Index Cache 를 구축
     */

    STL_TRY( ellRefineCache4CreateIndex( aTransID,
                                         aStmt,
                                         ellGetAuthID( sAuthHandle ),
                                         sIndexID,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * 동일한 Key Column 을 갖는 인덱스가 존재하는지 검사
     * - CREATE INDEX 는 S lock 을 통해 접근하므로
     * - 서로 다른 Trasaction 에서 동일한 Key Column 의 Index 가 생성되지 않도록 해야함.
     * - Validation 단계에서 이미 Commit 된 DDL 과 자신이 수행한 DDL 에 대해서만 검사한다.
     * - Execution 단계에서 다른 Tx 이 수행한 Uncommitted DDL 도 검사한다.
     */

    STL_TRY( ellGetIndexDictHandleByID( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sIndexID,
                                        & sIndexHandle,
                                        & sObjectExist,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_PARAM_VALIDATE( sObjectExist == STL_TRUE, QLL_ERROR_STACK(aEnv) );
    
    STL_TRY( ellRuntimeCheckSameKey4Index( aTransID,
                                           & sInitPlan->mTableHandle,
                                           & sIndexHandle,
                                           & sSameKey,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sSameKey != STL_TRUE, RAMP_ERR_SAME_KEY_COLUMN_LIST );
                                          
    /**
     * 생성한 객체에 대해 Lock 을 획득
     * 
     * commit -> pending -> trans end (unlock) 과정 중에
     * 해당 객체에 대한 DROP, ALTER 가 수행될 경우를 방지하기 위해
     * 생성한 객체에 대한 lock 을 획득해야 함.
     */

    STL_TRY( ellLockRowAfterCreateNonBaseTableObject( aTransID,
                                                      aStmt,
                                                      ELL_OBJECT_INDEX,
                                                      sIndexID,
                                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
                                          
    /**************************************************************
     * Index 구축 
     **************************************************************/

    STL_TRY( smlBuildIndex( aStmt,
                            sIndexPhyHandle,
                            ellGetTableHandle( & sInitPlan->mTableHandle ),
                            sInitPlan->mKeyCnt,
                            sDataPlan->mValueList,
                            sInitPlan->mKeyFlag,
                            sInitPlan->mParallelFactor,
                            SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SAME_KEY_COLUMN_LIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SAME_INDEX_KEY_COLUMNS,
                      NULL,
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}
    






/**
 * @brief CREATE INDEX Init Plan 에 Key 정의를 추가한다.
 * @param[in] aTransID     Transaction ID
 * @param[in] aDBCStmt     DBC Statement
 * @param[in] aSQLStmt     SQL Statement
 * @param[in] aSQLString   SQL String
 * @param[in] aInitPlan    CREATE INDEX 의 Init Plan
 * @param[in] aKeyElement  Key Column 정의 Parsing 정보 
 * @param[in] aEnv         Environment
 * @remarks
 */
stlStatus qliValidateKeyDefinition( smlTransId           aTransID,
                                    qllDBCStmt         * aDBCStmt,
                                    qllStatement       * aSQLStmt,
                                    stlChar            * aSQLString,
                                    qliInitCreateIndex * aInitPlan,
                                    qlpList            * aKeyElement,
                                    qllEnv             * aEnv )
{
    qlpListElement  * sListElement = NULL;
    qlpIndexElement * sKeyElement = NULL;

    stlBool    sObjectExist = STL_FALSE;
    
    stlInt64   sBigintValue = 0;

    stlInt32  i = 0;
    stlInt32  j = 0;

    stlInt32        sUniqueIndexCnt = 0;
    ellDictHandle * sUniqueIndexHandle = NULL;
    
    stlInt32        sNonUniqueIndexCnt = 0;
    ellDictHandle * sNonUniqueIndexHandle = NULL;

    ellDictHandle   * sIndexHandle = NULL;
    ellIndexKeyDesc * sKeyDesc = NULL;
    stlBool           sSameKey = STL_FALSE;

    dtlDataType    sColumnType = DTL_TYPE_NA;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyElement != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Key 컬럼 정의 정보를 획득한다.
     */

    aInitPlan->mKeyCnt = 0;
    
    QLP_LIST_FOR_EACH( aKeyElement, sListElement )
    {
        /**
         * Key Column 개수를 초과하는지 검사 
         */
        
        sKeyElement = (qlpIndexElement *) QLP_LIST_GET_POINTER_VALUE( sListElement );

        STL_TRY_THROW( aInitPlan->mKeyCnt < DTL_INDEX_COLUMN_MAX_COUNT,
                       RAMP_ERR_MAXIMUM_COLUMN_EXCEEDED );

        /**
         * 컬럼이 존재하는지 검사
         */

        STL_TRY( ellGetColumnDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         & aInitPlan->mTableHandle,
                                         QLP_GET_PTR_VALUE(sKeyElement->mColumnName),
                                         & aInitPlan->mKeyHandle[aInitPlan->mKeyCnt],
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_COLUMN_NOT_EXISTS );

        /**
         * Key Compare 가 가능한 Type 인지 검사
         */
        
        sColumnType = ellGetColumnDBType( & aInitPlan->mKeyHandle[aInitPlan->mKeyCnt] );
        STL_TRY_THROW( dtlCheckKeyCompareType( sColumnType ) == STL_TRUE,
                       RAMP_ERR_INVALID_KEY_COMP_TYPE );
        
        
        /**
         * 동일한 Key Column Name 이 존재하는 지 검사
         */

        for ( i = 0; i < aInitPlan->mKeyCnt; i++ )
        {
            if ( stlStrcmp(
                     ellGetColumnName(& aInitPlan->mKeyHandle[i]),
                     ellGetColumnName(& aInitPlan->mKeyHandle[aInitPlan->mKeyCnt]) )
                 == 0 )
            {
                STL_THROW( RAMP_ERR_SAME_COLUMN_EXIST );
            }
        }

        aInitPlan->mKeyFlag[aInitPlan->mKeyCnt] = 0;
        aInitPlan->mKeyFlag[aInitPlan->mKeyCnt] |= DTL_KEYCOLUMN_INDEX_NULLABLE_DEFAULT;
        
        /**
         * Key Column Order (ASC/DESC)
         */

        if ( sKeyElement->mIsAsc == NULL )
        {
            aInitPlan->mKeyOrder[aInitPlan->mKeyCnt] = ELL_INDEX_COLUMN_ORDERING_DEFAULT;
            aInitPlan->mKeyFlag[aInitPlan->mKeyCnt] |= DTL_KEYCOLUMN_INDEX_ORDER_DEFAULT;
        }
        else
        {
            sBigintValue = QLP_GET_INT_VALUE(sKeyElement->mIsAsc);

            if ( sBigintValue == STL_TRUE )
            {
                aInitPlan->mKeyOrder[aInitPlan->mKeyCnt] = ELL_INDEX_COLUMN_ASCENDING;
                aInitPlan->mKeyFlag[aInitPlan->mKeyCnt] |= DTL_KEYCOLUMN_INDEX_ORDER_ASC;
            }
            else
            {
                aInitPlan->mKeyOrder[aInitPlan->mKeyCnt] = ELL_INDEX_COLUMN_DESCENDING;
                aInitPlan->mKeyFlag[aInitPlan->mKeyCnt] |= DTL_KEYCOLUMN_INDEX_ORDER_DESC;
            }
        }
        
        /**
         * Key Nulls First/Last 
         */

        if ( sKeyElement->mIsNullsFirst == NULL )
        {
            aInitPlan->mKeyNullOrder[aInitPlan->mKeyCnt]
                = ELL_INDEX_COLUMN_NULLS_ORDERING_DEFAULT;
            aInitPlan->mKeyFlag[aInitPlan->mKeyCnt] |= DTL_KEYCOLUMN_INDEX_NULL_ORDER_DEFAULT;
        }
        else
        {
            sBigintValue = QLP_GET_INT_VALUE(sKeyElement->mIsNullsFirst);

            if ( sBigintValue == STL_TRUE )
            {
                aInitPlan->mKeyNullOrder[aInitPlan->mKeyCnt]
                    = ELL_INDEX_COLUMN_NULLS_FIRST;
                aInitPlan->mKeyFlag[aInitPlan->mKeyCnt] |= DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST;
            }
            else
            {
                aInitPlan->mKeyNullOrder[aInitPlan->mKeyCnt]
                    = ELL_INDEX_COLUMN_NULLS_LAST;
                aInitPlan->mKeyFlag[aInitPlan->mKeyCnt] |= DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST;
            }
        }

        aInitPlan->mKeyCnt++;
    }

    /**
     * 동일한 Key Column 을 갖는 인덱스가 존재하는지 검사
     * - CREATE INDEX 는 S lock 을 통해 접근하므로
     * - 서로 다른 Trasaction 에서 동일한 Key Column 의 Index 가 생성되지 않도록 해야함.
     * - Validation 단계에서 이미 Commit 된 DDL 과 자신이 수행한 DDL 에 대해서만 검사한다.
     * - Execution 단계에서 다른 Tx 이 수행한 Uncommitted DDL 도 검사한다.
     */

    sListElement = QLP_LIST_GET_FIRST( aKeyElement );
    sKeyElement = (qlpIndexElement *) QLP_LIST_GET_POINTER_VALUE( sListElement );
    
    STL_TRY( ellGetTableUniqueIndexDictHandle( aTransID,
                                               aSQLStmt->mViewSCN,
                                               QLL_INIT_PLAN(aDBCStmt),
                                               & aInitPlan->mTableHandle,
                                               & sUniqueIndexCnt,
                                               & sUniqueIndexHandle,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( ellGetTableNonUniqueIndexDictHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  QLL_INIT_PLAN(aDBCStmt),
                                                  & aInitPlan->mTableHandle,
                                                  & sNonUniqueIndexCnt,
                                                  & sNonUniqueIndexHandle,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    for ( i = 0; i < sUniqueIndexCnt; i++ )
    {
        sIndexHandle = & sUniqueIndexHandle[i];
        sKeyDesc = ellGetIndexKeyDesc( sIndexHandle );
        
        if ( ellGetIndexKeyCount( sIndexHandle ) == aInitPlan->mKeyCnt )
        {
            sSameKey = STL_TRUE;
            
            for ( j = 0; j < aInitPlan->mKeyCnt; j++ )
            {
                if ( ellGetColumnID( & sKeyDesc[j].mKeyColumnHandle )
                     == ellGetColumnID( & aInitPlan->mKeyHandle[j] ) )
                {
                    if ( aInitPlan->mKeyOrder[j] == sKeyDesc[j].mKeyOrdering )
                    {
                        if ( aInitPlan->mKeyNullOrder[j] == sKeyDesc[j].mKeyNullOrdering )
                        {
                            /* 동일한 Key 임 */
                        }
                        else
                        {
                            sSameKey = STL_FALSE;
                            break;
                        }
                    }
                    else
                    {
                        sSameKey = STL_FALSE;
                        break;
                    }
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
    
    for ( i = 0; i < sNonUniqueIndexCnt; i++ )
    {
        sIndexHandle = & sNonUniqueIndexHandle[i];
        sKeyDesc = ellGetIndexKeyDesc( sIndexHandle );
        
        if ( ellGetIndexKeyCount( sIndexHandle ) == aInitPlan->mKeyCnt )
        {
            sSameKey = STL_TRUE;
            
            for ( j = 0; j < aInitPlan->mKeyCnt; j++ )
            {
                if ( ellGetColumnID( & sKeyDesc[j].mKeyColumnHandle )
                     == ellGetColumnID( & aInitPlan->mKeyHandle[j] ) )
                {
                    if ( aInitPlan->mKeyOrder[j] == sKeyDesc[j].mKeyOrdering )
                    {
                        if ( aInitPlan->mKeyNullOrder[j] == sKeyDesc[j].mKeyNullOrdering )
                        {
                            /* 동일한 Key 임 */
                        }
                        else
                        {
                            sSameKey = STL_FALSE;
                            break;
                        }
                    }
                    else
                    {
                        sSameKey = STL_FALSE;
                        break;
                    }
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
                      QLP_GET_PTR_VALUE(sKeyElement->mColumnName) );
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
    
    STL_CATCH( RAMP_ERR_SAME_COLUMN_EXIST )
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
                      QLL_ERRCODE_SAME_INDEX_KEY_COLUMNS,
                      qlgMakeErrPosString( aSQLString,
                                           sKeyElement->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index Logging Flag 을 획득한다.
 * @param[in]  aSQLString        SQL string
 * @param[in]  aParseIndexAttr   Parsing 된 Index Attribute 정보
 * @param[out] aPhysicalAttr     정보를 채울 Index Attribute
 * @param[out] aLoggingFlag      LOGGING / NOLOGGING 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
void qliGetIndexLoggingFlag( stlChar       * aSQLString,
                             qlpIndexAttr  * aParseIndexAttr,
                             smlIndexAttr  * aPhysicalAttr,
                             stlBool       * aLoggingFlag,
                             qllEnv        * aEnv )
{
    stlInt64     sBigintValue = 0;   
    stlBool      sLoggingFlag = STL_FALSE;
    
    /**
     * Logging Option Validation
     */

    if ( aParseIndexAttr->mLogging == NULL )
    {
        /**
         * @todo Disk Index 라면 logging 을 기본으로 해야 함.
         */
        sLoggingFlag = knlGetPropertyBoolValueByID( KNL_PROPERTY_DEFAULT_INDEX_LOGGING,
                                                    KNL_ENV( aEnv ) );
    }
    else
    {
        sBigintValue = QLP_GET_INT_VALUE(aParseIndexAttr->mLogging);
        if ( sBigintValue == 0 )
        {
            sLoggingFlag = STL_FALSE;
        }
        else
        {
            sLoggingFlag = STL_TRUE;
        }
    }
    
    aPhysicalAttr->mValidFlags |= SML_INDEX_LOGGING_YES;
    aPhysicalAttr->mLoggingFlag = sLoggingFlag;

    /**
     * Output 설정
     */

    *aLoggingFlag = sLoggingFlag;
}
                             

/**
 * @brief Index Attribute 를 검증한다.
 * @param[in]  aSQLString        SQL string
 * @param[in]  aSpacePhyID       Tablespace Physical ID
 * @param[in]  aParseIndexAttr   Parsing 된 Index Attribute 정보
 * @param[out] aPhysicalAttr     정보를 채울 Index Attribute
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus qliValidateIndexAttr( stlChar       * aSQLString,
                                smlTbsId        aSpacePhyID,
                                qlpIndexAttr  * aParseIndexAttr,
                                smlIndexAttr  * aPhysicalAttr,
                                qllEnv        * aEnv )
{
    stlInt64             sBigintValue = 0;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aParseIndexAttr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPhysicalAttr != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * PCTFREE Validation
     */
    
    if ( aParseIndexAttr->mPctFree == NULL )
    {
        STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_DEFAULT_PCTFREE,
                                          &sBigintValue,
                                          KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(aParseIndexAttr->mPctFree),
                           stlStrlen( QLP_GET_PTR_VALUE(aParseIndexAttr->mPctFree) ),
                           & sBigintValue,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_PCTFREE );

        STL_TRY_THROW( (sBigintValue >= SML_MIN_PCTFREE) &&
                       (sBigintValue <= SML_MAX_PCTFREE),
                       RAMP_ERR_INVALID_PCTFREE );
    }

    aPhysicalAttr->mValidFlags |= SML_INDEX_PCTFREE_YES;
    aPhysicalAttr->mPctFree = (stlInt16) sBigintValue;

    /**
     * INITRANS Validation
     */
    
    if ( aParseIndexAttr->mIniTrans == NULL )
    {
        STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_DEFAULT_INITRANS,
                                          &sBigintValue,
                                          KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(aParseIndexAttr->mIniTrans),
                           stlStrlen( QLP_GET_PTR_VALUE(aParseIndexAttr->mIniTrans) ),
                           & sBigintValue,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_INITRANS );

        STL_TRY_THROW( (sBigintValue >= SML_MIN_INITRANS ) &&
                       (sBigintValue <= SML_MAX_INITRANS),
                       RAMP_ERR_INVALID_INITRANS );
    }
    
    aPhysicalAttr->mValidFlags |= SML_INDEX_INITRANS_YES;
    aPhysicalAttr->mIniTrans = (stlInt16) sBigintValue;

    /**
     * MAXTRANS Validation
     */
    
    if ( aParseIndexAttr->mMaxTrans == NULL )
    {
        STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_DEFAULT_MAXTRANS,
                                          &sBigintValue,
                                          KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(aParseIndexAttr->mMaxTrans),
                           stlStrlen( QLP_GET_PTR_VALUE(aParseIndexAttr->mMaxTrans) ),
                           & sBigintValue,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_MAXTRANS );

        STL_TRY_THROW( (sBigintValue >= SML_MIN_MAXTRANS ) &&
                       (sBigintValue <= SML_MAX_MAXTRANS),
                       RAMP_ERR_INVALID_MAXTRANS );
    }

    aPhysicalAttr->mValidFlags |= SML_INDEX_MAXTRANS_YES;
    aPhysicalAttr->mMaxTrans = (stlInt16)sBigintValue;

    STL_TRY_THROW( aPhysicalAttr->mMaxTrans >=
                   aPhysicalAttr->mIniTrans,
                   RAMP_ERR_TRANS_RANGE );
    
    /**
     * Segment Attribute Validation
     */

    STL_TRY( qlrValidateSegmentAttr( aSQLString,
                                     aSpacePhyID,
                                     aParseIndexAttr->mSegInitialSize,
                                     aParseIndexAttr->mSegNextSize,
                                     aParseIndexAttr->mSegMinSize,
                                     aParseIndexAttr->mSegMaxSize,
                                     & aPhysicalAttr->mSegAttr,
                                     aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_INITRANS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_INITRANS,
                      qlgMakeErrPosString( aSQLString,
                                           aParseIndexAttr->mIniTrans->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_MAXTRANS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_MAXTRANS,
                      qlgMakeErrPosString( aSQLString,
                                           aParseIndexAttr->mMaxTrans->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_PCTFREE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_PCTVALUE,
                      qlgMakeErrPosString( aSQLString,
                                           aParseIndexAttr->mPctFree->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_TRANS_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TRANS_RANGE,
                      qlgMakeErrPosString( aSQLString,
                                           aParseIndexAttr->mIniTrans != NULL
                                           ? aParseIndexAttr->mIniTrans->mNodePos
                                           : aParseIndexAttr->mMaxTrans->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} qliCreateIndex */
