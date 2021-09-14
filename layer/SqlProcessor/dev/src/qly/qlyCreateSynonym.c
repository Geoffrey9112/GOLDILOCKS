/*******************************************************************************
 * qlyCreateSynonym.c
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
 * @file qlyCreateSynonym.c
 * @brief CREATE SYNONYM 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlyCreateSynonym CREATE SYNONYM
 * @ingroup qly
 * @{
 */

/**
 * @brief CREATE SYNONYM 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlyValidateCreateSynonym( smlTransId      aTransID,
                                    qllDBCStmt    * aDBCStmt,
                                    qllStatement  * aSQLStmt,
                                    stlChar       * aSQLString,
                                    qllNode       * aParseTree,
                                    qllEnv        * aEnv )
{
    ellDictHandle        * sAuthHandle = NULL;
    qlpSynonymDef        * sParseTree = NULL;
    qlyInitCreateSynonym * sInitPlan = NULL;
    stlBool                sObjectExist = STL_FALSE;
    ellDictHandle          sObjectHandle;
    ellDictHandle          sDefaultSchemaHandle;
    stlChar              * sDefaultSchemaName = NULL;
    stlChar              * sSchemaName;
    ellTableType           sObjectType = ELL_TABLE_TYPE_NA;
    
    ellInitDictHandle( & sObjectHandle );
    ellInitDictHandle( & sDefaultSchemaHandle );
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_SYNONYM_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_CREATE_SYNONYM_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpSynonymDef *) aParseTree;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlyInitCreateSynonym),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlyInitCreateSynonym) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Create Option 정보 획득
     **********************************************************/
    
    sInitPlan->mOrReplace = sParseTree->mOrReplace;
    sInitPlan->mIsPublic  = sParseTree->mIsPublic;

    /**********************************************************
     * Synonym Name Validation 
     **********************************************************/

    if ( sInitPlan->mIsPublic == STL_TRUE )
    {
        /**
         * PUBLIC SYNONYM
         */

        /**
         * PUBLIC SYNONYM은 스키마명 명시할 수 없음 
         */
        
        STL_DASSERT( sParseTree->mSynonymName->mSchema == NULL );

        /**
         * CREATE PUBLIC SYNONYM 권한 검사
         */

        STL_TRY( qlaCheckDatabasePriv( aTransID,
                                       aDBCStmt,
                                       aSQLStmt,
                                       ELL_DB_PRIV_ACTION_CREATE_PUBLIC_SYNONYM,
                                       aEnv )
             == STL_SUCCESS );

        /**
         * 같은 이름을 가진 Public Synonym 존재 여부 확인
        */

        STL_TRY( ellGetPublicSynonymDictHandle( aTransID,
                                                aSQLStmt->mViewSCN,
                                                sParseTree->mSynonymName->mObject,
                                                & sObjectHandle,
                                                & sObjectExist,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );


        if ( sObjectExist == STL_TRUE )
        {
            /**
             * OR REPLACE 아닌 경우, Error 
            */
            
            STL_TRY_THROW( sInitPlan->mOrReplace == STL_TRUE,
                           RAMP_ERR_SAME_NAME_PUBLIC_SYNONYM_EXISTS );

            
            /**
             * 동일한 Synonym 이 존재함
             */
        
            sInitPlan->mOldSynonymExist = STL_TRUE;
            stlMemcpy( & sInitPlan->mOldSynonymHandle,
                       & sObjectHandle,
                       STL_SIZEOF( ellDictHandle ) );
            
            /**
             * DROP PUBLIC SYNONYM 권한 검사
            */
            
            STL_TRY( qlaCheckDatabasePriv( aTransID,
                                           aDBCStmt,
                                           aSQLStmt,
                                           ELL_DB_PRIV_ACTION_DROP_PUBLIC_SYNONYM,
                                           aEnv )
                     == STL_SUCCESS );

        }
        else
        {
            sInitPlan->mOldSynonymExist = STL_FALSE;
        }
    }
    else
    {
        /**
         * PRIVATE SYNONYM
         */

        if ( sParseTree->mSynonymName->mSchema == NULL )
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
                                             sParseTree->mSynonymName->mSchema,
                                             & sInitPlan->mSchemaHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        
            STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXISTS );
        }

        /**
         * Wriable Schema 인지 검사
         */
        
        STL_TRY_THROW( ellGetSchemaWritable( & sInitPlan->mSchemaHandle ) == STL_TRUE,
                       RAMP_ERR_SCHEMA_NOT_WRITABLE );

        /**
         * CREATE SYNONYM ON SCHEMA 권한 검사
         */
        
        STL_TRY( qlaCheckSchemaPriv( aTransID,
                                     aDBCStmt,
                                     aSQLStmt,
                                     ELL_SCHEMA_PRIV_ACTION_CREATE_SYNONYM,
                                     & sInitPlan->mSchemaHandle,
                                     aEnv )
                 == STL_SUCCESS );

        /**
         * 같은 이름을 가진 Object( Table, View, Sequence, Synonym ) 존재 여부 확인
         */
        
        STL_TRY( qlyExistSameNameObject( aTransID,
                                         aSQLStmt,
                                         aSQLString,
                                         & sInitPlan->mSchemaHandle,
                                         sParseTree->mSynonymName,
                                         & sObjectHandle,
                                         & sObjectExist,
                                         & sObjectType,
                                         aEnv )
                 == STL_SUCCESS );

        if ( sObjectExist == STL_TRUE )
        {
            /**
             * OR REPLACE 아닌 경우, Error 
             */
            
            STL_TRY_THROW( sInitPlan->mOrReplace == STL_TRUE, RAMP_ERR_SAME_NAME_OBJECT_EXISTS );

            /**
             * OR REPLACE 이지만,
             * 동일한 이름 객체가 SYNONYM이 아닌 경우, Error 
             */
        
            STL_TRY_THROW( sObjectType == ELL_TABLE_TYPE_SYNONYM,
                           RAMP_ERR_SAME_NAME_OBJECT_EXISTS );
            
            /**
             * 동일한 Synonym 이 존재함
             */
        
            sInitPlan->mOldSynonymExist = STL_TRUE;
            stlMemcpy( & sInitPlan->mOldSynonymHandle,
                       & sObjectHandle,
                       STL_SIZEOF( ellDictHandle ) );

            /**
             * DROP SYNONYM 권한 검사
             */

            STL_TRY( qlaCheckPrivDropSynonym( aTransID,
                                              aDBCStmt,
                                              aSQLStmt,
                                              & sInitPlan->mOldSynonymHandle,
                                              aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sInitPlan->mOldSynonymExist = STL_FALSE;
        }
    }
    

    /**********************************************************
     * Create Synonym 정보 획득
     **********************************************************/

    /**
     * Synonym Name 설정
     */
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                stlStrlen( sParseTree->mSynonymName->mObject ) + 1,
                                (void **) & sInitPlan->mSynonymName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sInitPlan->mSynonymName, sParseTree->mSynonymName->mObject );
    
    /**
     * Object Schema Name 설정
     */
    
    if ( sParseTree->mObjectName->mSchema == NULL )
    {
        /**
         * 스키마 명을 명시하지 않았을 경우, 사용자의 기본 스키마로 설정된다.
         * 실행 시, 기본 스키마에 해당 객체 이름의 객체가 존재하지 않으면 Public Synonym을 찾는다.
         * ( 기본 스키마명을 명시하여도 해당 객체 이름의 객체가 존재하지 않으면 Public Synonym을 찾는다. )
         */

        STL_TRY( ellGetAuthFirstSchemaDictHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sAuthHandle,
                                                  & sDefaultSchemaHandle,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        sDefaultSchemaName = ellGetSchemaName( & sDefaultSchemaHandle );

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    stlStrlen( sDefaultSchemaName ) + 1,
                                    (void **) & sInitPlan->mObjectSchemaName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( sInitPlan->mObjectSchemaName, sDefaultSchemaName );

    }
    else
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    stlStrlen( sParseTree->mObjectName->mSchema ) + 1,
                                    (void **) & sInitPlan->mObjectSchemaName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( sInitPlan->mObjectSchemaName, sParseTree->mObjectName->mSchema );
    }
    
    /**
     * Object Name 설정
     */
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                stlStrlen( sParseTree->mObjectName->mObject ) + 1,
                                (void **) & sInitPlan->mObjectName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sInitPlan->mObjectName, sParseTree->mObjectName->mObject );

    /**********************************************************
     * Object Name Validation 
     **********************************************************/

    if ( sInitPlan->mIsPublic == STL_FALSE )
    {
        sSchemaName = ellGetSchemaName( & sInitPlan->mSchemaHandle );

        if ( stlStrcmp( sSchemaName, sInitPlan->mObjectSchemaName ) == 0 )
        {
            if ( stlStrcmp( sInitPlan->mSynonymName, sInitPlan->mObjectName ) == 0 )
            {
                STL_THROW( RAMP_ERR_LOOPING_CHAIN_SYNONYM_NAME );
            }
            else
            {
                /* nothing to do */
            }
        }
        else
        {
            /* nothing to do */
        }
    }
    else
    {
        /**
         * PUBLIC SYNONYM은 schema가 없기 때문에
         * looping chain은 실행시에만 할 수 있다. 
         */  
    }

    /***********************************************************
     * Init Plan 연결 
     ***********************************************************/
    
    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mSynonymName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mSynonymName->mSchema );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_WRITABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_WRITABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mSynonymName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sInitPlan->mSchemaHandle ) );
    }

    STL_CATCH( RAMP_ERR_SAME_NAME_OBJECT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USED_OBJECT_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mSynonymName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sInitPlan->mSchemaHandle ),
                      sParseTree->mSynonymName->mObject );
    }

    STL_CATCH( RAMP_ERR_SAME_NAME_PUBLIC_SYNONYM_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USED_PUBLIC_SYNONYM_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mSynonymName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mSynonymName->mObject  );
    }

    STL_CATCH( RAMP_ERR_LOOPING_CHAIN_SYNONYM_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_LOOPING_CHAIN_SYNONYM_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mSynonymName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief CREATE SYNONYM 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlyOptCodeCreateSynonym( smlTransId      aTransID,
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
 * @brief CREATE SYNONYM 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlyOptDataCreateSynonym( smlTransId      aTransID,
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
 * @brief CREATE SYNONYM 구문을 Execution 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlyExecuteCreateSynonym( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   qllEnv        * aEnv )
{
    ellDictHandle        * sAuthHandle = NULL;
    qlyInitCreateSynonym * sInitPlan = NULL;
    
    stlTime                sTime = 0;
    stlBool                sLocked = STL_TRUE;
    
    stlInt64               sSynonymTableID = ELL_DICT_OBJECT_ID_NA;
    stlInt64               sSynonymID = ELL_DICT_OBJECT_ID_NA;
        
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_SYNONYM_TYPE,
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
    
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sInitPlan   = (qlyInitCreateSynonym *) aSQLStmt->mInitPlan;

    if ( sInitPlan->mOldSynonymExist == STL_TRUE )
    {
        /**
         * OR REPLACE 옵션이 존재하고, 해당 Synonym이 존재하는 경우
         */
        
        if ( sInitPlan->mIsPublic == STL_TRUE )
        {
            /**
             * DROP PUBLIC SYNONYM 구문을 위한 DDL Lock 획득
             */
            
            STL_TRY( ellLock4DropPublicSynonym( aTransID,
                                                aStmt,
                                                & sInitPlan->mOldSynonymHandle,
                                                & sLocked,
                                                ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
        }
        else
        {
            /**
             * DROP SYNONYM 구문을 위한 DDL Lock 획득
             */
            
            STL_TRY( ellLock4DropSynonym( aTransID,
                                          aStmt,
                                          & sInitPlan->mOldSynonymHandle,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
        }
    }
    else
    {
        if ( sInitPlan->mIsPublic == STL_TRUE )
        {
            /**
             * CREATE PUBLIC SYNONYM 구문을 위한 DDL Lock 획득
             */
            
            STL_TRY( ellLock4CreatePublicSynonym( aTransID,
                                                  aStmt,
                                                  sAuthHandle,
                                                  & sLocked,
                                                  ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
        }
        else
        {
            /**
             * CREATE SYNONYM 구문을 위한 DDL Lock 획득
             */
            
            STL_TRY( ellLock4CreateSynonym( aTransID,
                                            aStmt,
                                            sAuthHandle,
                                            & sInitPlan->mSchemaHandle,
                                            & sLocked,
                                            ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
        }
    }
    
    /**
     * Valid Plan 을 다시 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );
    
    sInitPlan   = (qlyInitCreateSynonym *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /**********************************************************
     * Synonym Dictionary 구축 
     **********************************************************/

    /**
     * 기존 Synonym 관련 정보 삭제 및 Synonym ID 획득 
     */

    if ( sInitPlan->mOldSynonymExist == STL_TRUE )
    {
        /**
         * CREATE OR REPLACE 이고 Old Synonym이 존재하는 경우, 
         * 기존 Synonym Description 삭제한다.
         */
        
        if ( sInitPlan->mIsPublic == STL_TRUE )
        {
            /**
             * Public Synonym Description 삭제
             * 삭제 하기 전, 기존 SynonymID를 저장해둠 
             */
            
            sSynonymID = ellGetPublicSynonymID( & sInitPlan->mOldSynonymHandle );
            
            STL_TRY( ellRemoveDict4DropPublicSynonym( aTransID,
                                                      aStmt,
                                                      & sInitPlan->mOldSynonymHandle,
                                                      ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * Cache 재구성
             */
            
            STL_TRY( ellRefineCache4DropPublicSynonym( aTransID,
                                                       aStmt,
                                                       & sInitPlan->mOldSynonymHandle,
                                                       ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * Private Synonym Description 삭제
             * 삭제 하기 전, 기존 SynonymID를 저장해둠
             * ( Name resolution을 위해 삽입한 Table description은 삭제하지 않는다. )
             */
            
            sSynonymID      = ellGetSynonymID( & sInitPlan->mOldSynonymHandle );
            sSynonymTableID = ellGetSynonymTableID( & sInitPlan->mOldSynonymHandle );
            
            
            STL_TRY( ellRemoveDict4ReplaceSynonym( aTransID,
                                                   aStmt,
                                                   & sInitPlan->mOldSynonymHandle,
                                                   ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * Cache 재구성
             */

            STL_TRY( ellRefineCache4DropSynonym( aTransID,
                                                 aStmt,
                                                 & sInitPlan->mOldSynonymHandle,
                                                 ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /**
         * Old Synonym이 없는 경우,
         * 새로운 Synonym Description 추가를 위하여 새로운 Synonym ID를 획득한다.  
         */
        
        if ( sInitPlan->mIsPublic == STL_TRUE )
        {
            /* 새로운 Public Synonym ID 획득  */ 
            STL_TRY( ellGetNewPublicSynonymID( & sSynonymID, ELL_ENV(aEnv) )
                     == STL_SUCCESS );
                     
        }
        else
        {
            /* 새로운 Private Synonym ID 획득  */ 
            STL_TRY( ellGetNewSynonymID( & sSynonymID, ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            /* Name resolution을 위해 Table description도 추가하고,
             * Synonym Table ID도 획득한다.
             */

            STL_TRY( ellInsertTableDesc( aTransID,
                                         aStmt,
                                         ellGetAuthID( sAuthHandle ),
                                         ellGetSchemaID( & sInitPlan->mSchemaHandle ),
                                         & sSynonymTableID,
                                         ELL_DICT_TABLESPACE_ID_NA,
                                         ELL_DICT_OBJECT_ID_NA,
                                         sInitPlan->mSynonymName,
                                         ELL_TABLE_TYPE_SYNONYM,
                                         NULL,
                                         ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }

    /**
     * Synonym Descriptor 추가
     */
    
    if ( sInitPlan->mIsPublic == STL_TRUE )
    {
        /**
         * Public Synonym Descriptor 추가
         */
        
        STL_TRY( ellInsertPublicSynonymDesc( aTransID,
                                             aStmt,
                                             sSynonymID,
                                             sInitPlan->mSynonymName,
                                             ellGetAuthID( sAuthHandle ),
                                             sInitPlan->mObjectSchemaName,
                                             sInitPlan->mObjectName,
                                             ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /**
         * SQL-PublicSynonym Cache 구축
         */                                              
        
        STL_TRY( ellRefineCache4CreatePublicSynonym( aTransID,
                                                     aStmt,
                                                     sSynonymID,
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
                                                          ELL_OBJECT_PUBLIC_SYNONYM,
                                                          sSynonymID,
                                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * Synonym Descriptor 추가
         */
        
        STL_TRY( ellInsertSynonymDesc( aTransID,
                                       aStmt,
                                       ellGetAuthID( sAuthHandle ),
                                       ellGetSchemaID( & sInitPlan->mSchemaHandle ),
                                       sSynonymID,
                                       sSynonymTableID,
                                       sInitPlan->mSynonymName,
                                       sInitPlan->mObjectSchemaName,
                                       sInitPlan->mObjectName,
                                       ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /**
         * SQL-Synonym Cache 구축
         */                                              
        
        STL_TRY( ellRefineCache4CreateSynonym( aTransID,
                                               aStmt,
                                               sSynonymID,
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
                                                          ELL_OBJECT_SYNONYM,
                                                          sSynonymID,
                                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Table, Sequence, Synonym에 동일한 객체 이름이 존재하는지 검사 
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aSQLStmt            SQL Statement
 * @param[in]  aSQLString          SQL String
 * @param[in]  aSchemaHandle       Schema Handle
 * @param[in]  aObjectName         Object Name
 * @param[out] aObjectDictHandle   Object Handle
 * @param[out] aObjectExist        존재 여부
 * @param[out] aObjectType         객체가 이미 존재하는 경우, Object의 Type 정보  
 * @param[in]  aEnv                Environment
 * @remarks
 *  Oracle 호환성을 위해 Table,Sequence,Synonym 은 Name Space 를 같이 사용한다.
 */
stlStatus qlyExistSameNameObject( smlTransId      aTransID,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  ellDictHandle * aSchemaHandle,
                                  qlpObjectName * aObjectName,
                                  ellDictHandle * aObjectDictHandle,
                                  stlBool       * aObjectExist,
                                  ellTableType  * aObjectType,  
                                  qllEnv        * aEnv )
{
    stlBool sObjectExist = STL_FALSE;
    
    /**
     * 같은 이름을 가진  Table 존재 여부 확인
     */
    
    STL_TRY( ellGetTableDictHandleWithSchema( aTransID,
                                              aSQLStmt->mViewSCN,
                                              aSchemaHandle,
                                              aObjectName->mObject,
                                              aObjectDictHandle,
                                              & sObjectExist,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sObjectExist == STL_TRUE )
    {
        *aObjectType = ellGetTableType( aObjectDictHandle );
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* nothing to do */
    }

    /**
     * 같은 이름을 가진 Sequence 가 있는지 확인
     */

    STL_TRY( ellGetSequenceDictHandleWithSchema( aTransID,
                                                 aSQLStmt->mViewSCN,
                                                 aSchemaHandle,
                                                 aObjectName->mObject,
                                                 aObjectDictHandle,
                                                 & sObjectExist,
                                                 ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sObjectExist == STL_TRUE )
    {
        *aObjectType = ELL_TABLE_TYPE_SEQUENCE;
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* nothing to do */
    }

    /**
     * 같은 이름을 가진 Synonym 가 있는지 확인
     */
    
    STL_TRY( ellGetSynonymDictHandleWithSchema( aTransID,
                                                aSQLStmt->mViewSCN,
                                                aSchemaHandle,
                                                aObjectName->mObject,
                                                aObjectDictHandle,
                                                & sObjectExist,
                                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sObjectExist == STL_TRUE )
    {
        *aObjectType = ELL_TABLE_TYPE_SYNONYM;
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* nothing to do */
    }

    STL_RAMP( RAMP_FINISH );

    *aObjectExist = sObjectExist;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlyCreateSynonym */
