/*******************************************************************************
 * qldCommentStmt.c
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
 * @file qldCommentStmt.c
 * @brief COMMENT ON 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>

#include <qldCommentStmt.h>


/**
 * @defgroup qldComment COMMENT ON
 * @ingroup qld
 * @{
 */


/**
 * @brief COMMENT 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateCommentStmt( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv )
{
    ellDictHandle      * sAuthHandle = NULL;
    
    qlpCommentOnStmt   * sParseTree = NULL;
    qldInitCommentStmt * sInitPlan = NULL;

    stlBool   sObjectExist = STL_FALSE;

    stlChar        * sNonSchemaObject = NULL;
    qlpObjectName  * sSchemaObject = NULL; 
    qlpColumnName  * sColumnObject = NULL; 

    stlChar        * sCommentString = NULL;
    stlInt32         sCommentLength = 0;
    
    ellDictHandle    sSchemaHandle;

    ellInitDictHandle( & sSchemaHandle );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_COMMENT_ON_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_COMMENT_ON_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sParseTree = (qlpCommentOnStmt *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qldInitCommentStmt),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qldInitCommentStmt) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /****************************************************
     * Object 유형에 따른 객체 정보 획득
     ****************************************************/

    sInitPlan->mObjectType = sParseTree->mCommentObject->mObjectType;

    ellInitDictHandle( & sInitPlan->mRelatedTableHandle );
    
    switch ( sInitPlan->mObjectType )
    {
        case ELL_OBJECT_COLUMN:
            {
                sColumnObject = sParseTree->mCommentObject->mColumnObjectName;

                /**
                 * Column 이름이 단독으로 존재할 수 없음.
                 */
                
                STL_TRY_THROW( sColumnObject->mTable != NULL, RAMP_ERR_COLUMN_NOT_EXIST );

                /**
                 * Table Dictionary Handle 획득
                 */
                
                if ( sColumnObject->mSchema == NULL )
                {
                    STL_TRY( qlvGetTableDictHandleByAuthHandleNTblName( aTransID,
                                                                        aDBCStmt,
                                                                        aSQLStmt,
                                                                        aSQLString,
                                                                        sAuthHandle,
                                                                        sColumnObject->mTable,
                                                                        sColumnObject->mTablePos,
                                                                        & sInitPlan->mRelatedTableHandle,
                                                                        & sObjectExist,
                                                                        aEnv )
                             == STL_SUCCESS );
                    
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE,
                                   RAMP_ERR_COLUMN_TABLE_NOT_EXIST );
                }
                else
                {
                    STL_TRY( ellGetSchemaDictHandle( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     sColumnObject->mSchema,
                                                     & sSchemaHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE,
                                   RAMP_ERR_QUALIFIED_COLUMN_SCHEMA_NOT_EXIST );

                    STL_TRY( qlvGetTableDictHandleBySchHandleNTblName( aTransID,
                                                                       aDBCStmt,
                                                                       aSQLStmt,
                                                                       aSQLString,
                                                                       & sSchemaHandle,
                                                                       sColumnObject->mTable,
                                                                       sColumnObject->mTablePos,
                                                                       & sInitPlan->mRelatedTableHandle,
                                                                       & sObjectExist,
                                                                       aEnv )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE,
                                   RAMP_ERR_COLUMN_TABLE_NOT_EXIST );
                }

                /**
                 * Built-In Table 인지 확인
                 */

                if ( ellGetAuthID( sAuthHandle ) == ellGetAuthIdSYSTEM() )
                {
                    /**
                     * _SYSTEM 계정으로 COMMENT 를 하고 있는 경우
                     */
                }
                else
                {
                    STL_TRY_THROW( ellIsBuiltInTable( & sInitPlan->mRelatedTableHandle )
                                   == STL_FALSE,
                                   RAMP_ERR_BUILT_IN_COLUMN_TABLE );
                }
                    
                /**
                 * Column Dictionary Handle 획득 
                 */

                STL_TRY( ellGetColumnDictHandle( aTransID,
                                                 aSQLStmt->mViewSCN,
                                                 & sInitPlan->mRelatedTableHandle,
                                                 sColumnObject->mColumn,
                                                 & sInitPlan->mObjectHandle,
                                                 & sObjectExist,
                                                 ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_COLUMN_NOT_EXIST );
                break;
            }
        case ELL_OBJECT_TABLE:
            {
                /**
                 * Table Dictionary Handle 획득
                 */
                
                sSchemaObject = sParseTree->mCommentObject->mSchemaObjectName;
                
                if ( sSchemaObject->mSchema == NULL )
                {
                    STL_TRY( qlvGetTableDictHandleByAuthHandleNTblName( aTransID,
                                                                        aDBCStmt,
                                                                        aSQLStmt,
                                                                        aSQLString,
                                                                        sAuthHandle,
                                                                        sSchemaObject->mObject,
                                                                        sSchemaObject->mObjectPos,
                                                                        & sInitPlan->mObjectHandle,
                                                                        & sObjectExist,
                                                                        aEnv )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXIST );
                }
                else
                {
                    STL_TRY( ellGetSchemaDictHandle( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     sSchemaObject->mSchema,
                                                     & sSchemaHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE,
                                   RAMP_ERR_QUALIFIED_SCHEMA_NOT_EXIST );

                    STL_TRY( qlvGetTableDictHandleBySchHandleNTblName( aTransID,
                                                                       aDBCStmt,
                                                                       aSQLStmt,
                                                                       aSQLString,
                                                                       & sSchemaHandle,
                                                                       sSchemaObject->mObject,
                                                                       sSchemaObject->mObjectPos,
                                                                       & sInitPlan->mObjectHandle,
                                                                       & sObjectExist,
                                                                       aEnv )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXIST );
                }

                /**
                 * Built-In Table 인지 확인
                 */

                if ( ellGetAuthID( sAuthHandle ) == ellGetAuthIdSYSTEM() )
                {
                    /**
                     * _SYSTEM 계정으로 COMMENT 를 하고 있는 경우
                     */
                }
                else
                {
                    STL_TRY_THROW( ellIsBuiltInTable( & sInitPlan->mObjectHandle ) == STL_FALSE,
                                   RAMP_ERR_BUILT_IN_TABLE );
                }
                
                break;
            }
        case ELL_OBJECT_CONSTRAINT:
            {
                /**
                 * Constraint Dictionary Handle 획득
                 */
                
                sSchemaObject = sParseTree->mCommentObject->mSchemaObjectName;
                
                if ( sSchemaObject->mSchema == NULL )
                {
                    STL_TRY( ellGetConstraintDictHandleWithAuth( aTransID,
                                                                 aSQLStmt->mViewSCN,
                                                                 sAuthHandle,
                                                                 sSchemaObject->mObject,
                                                                 & sInitPlan->mObjectHandle,
                                                                 & sObjectExist,
                                                                 ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    STL_TRY_THROW( sObjectExist == STL_TRUE,
                                   RAMP_ERR_CONSTRAINT_NOT_EXIST );
                }
                else
                {
                    STL_TRY( ellGetSchemaDictHandle( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     sSchemaObject->mSchema,
                                                     & sSchemaHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE,
                                   RAMP_ERR_QUALIFIED_SCHEMA_NOT_EXIST );

                    STL_TRY( ellGetConstraintDictHandleWithSchema(
                                 aTransID,
                                 aSQLStmt->mViewSCN,
                                 & sSchemaHandle,
                                 sSchemaObject->mObject,
                                 & sInitPlan->mObjectHandle,
                                 & sObjectExist,
                                 ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_CONSTRAINT_NOT_EXIST );
                }

                /**
                 * Built-In Constraint 인지 확인
                 */

                STL_TRY_THROW( ellIsBuiltInConstraint( & sInitPlan->mObjectHandle )
                               == STL_FALSE, RAMP_ERR_BUILT_IN_CONSTRAINT );

                /**
                 * Related Table Dictionary Handle 획득
                 */

                STL_TRY( ellGetTableDictHandleByID(
                             aTransID,
                             aSQLStmt->mViewSCN,
                             ellGetConstraintTableID( & sInitPlan->mObjectHandle ),
                             & sInitPlan->mRelatedTableHandle,
                             & sObjectExist,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_CONSTRAINT_NOT_EXIST );

                break;
            }
        case ELL_OBJECT_INDEX:
            {
                /**
                 * Index Dictionary Handle 획득
                 */
                
                sSchemaObject = sParseTree->mCommentObject->mSchemaObjectName;
                
                if ( sSchemaObject->mSchema == NULL )
                {
                    STL_TRY( ellGetIndexDictHandleWithAuth( aTransID,
                                                            aSQLStmt->mViewSCN,
                                                            sAuthHandle,
                                                            sSchemaObject->mObject,
                                                            & sInitPlan->mObjectHandle,
                                                            & sObjectExist,
                                                            ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_INDEX_NOT_EXIST );
                }
                else
                {
                    STL_TRY( ellGetSchemaDictHandle( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     sSchemaObject->mSchema,
                                                     & sSchemaHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE,
                                   RAMP_ERR_QUALIFIED_SCHEMA_NOT_EXIST );

                    STL_TRY( ellGetIndexDictHandleWithSchema( aTransID,
                                                              aSQLStmt->mViewSCN,
                                                              & sSchemaHandle,
                                                              sSchemaObject->mObject,
                                                              & sInitPlan->mObjectHandle,
                                                              & sObjectExist,
                                                              ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_INDEX_NOT_EXIST );
                }

                /**
                 * Built-In Index 인지 확인
                 */

                STL_TRY_THROW( ellIsBuiltInIndex( & sInitPlan->mObjectHandle ) == STL_FALSE,
                               RAMP_ERR_BUILT_IN_INDEX );

                /**
                 * Constraint 에 의해 생성된 index 인지 확인
                 */

                STL_TRY_THROW( ellGetIndexIsByConstraint( & sInitPlan->mObjectHandle )
                               == STL_FALSE,
                               RAMP_ERR_INDEX_WITH_KEY_CONSTRAINT );

                /**
                 * Related Table Dictionary Handle 획득
                 */

                STL_TRY( ellGetTableDictHandleByID(
                             aTransID,
                             aSQLStmt->mViewSCN,
                             ellGetIndexTableID( & sInitPlan->mObjectHandle ),
                             & sInitPlan->mRelatedTableHandle,
                             & sObjectExist,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_INDEX_NOT_EXIST );
                    
                break;
            }
        case ELL_OBJECT_SEQUENCE:
            {
                /**
                 * Sequence Dictionary Handle 획득
                 */
                
                sSchemaObject = sParseTree->mCommentObject->mSchemaObjectName;
                
                if ( sSchemaObject->mSchema == NULL )
                {
                    STL_TRY( qlvGetSequenceDictHandleByAuthHandleNSeqName( aTransID,
                                                                           aDBCStmt,
                                                                           aSQLStmt,
                                                                           aSQLString,
                                                                           sAuthHandle,
                                                                           sSchemaObject->mObject,
                                                                           sSchemaObject->mObjectPos,
                                                                           & sInitPlan->mObjectHandle,
                                                                           & sObjectExist,
                                                                           aEnv )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SEQUENCE_NOT_EXIST );
                }
                else
                {
                    STL_TRY( ellGetSchemaDictHandle( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     sSchemaObject->mSchema,
                                                     & sSchemaHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE,
                                   RAMP_ERR_QUALIFIED_SCHEMA_NOT_EXIST );

                    STL_TRY( qlvGetSequenceDictHandleBySchHandleNSeqName( aTransID,
                                                                          aDBCStmt,
                                                                          aSQLStmt,
                                                                          aSQLString,
                                                                          & sSchemaHandle,
                                                                          sSchemaObject->mObject,
                                                                          sSchemaObject->mObjectPos,
                                                                          & sInitPlan->mObjectHandle,
                                                                          & sObjectExist,
                                                                          aEnv )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SEQUENCE_NOT_EXIST );
                }

                /**
                 * Built-In Sequence 인지 확인
                 */

                STL_TRY_THROW( ellIsBuiltInSequence( & sInitPlan->mObjectHandle )
                               == STL_FALSE, RAMP_ERR_BUILT_IN_SEQUENCE );

                break;
            }
        case ELL_OBJECT_SCHEMA:
            {
                /**
                 * Schema Dictionary Handle 획득
                 */
                
                sNonSchemaObject =
                    QLP_GET_PTR_VALUE( sParseTree->mCommentObject->mNonSchemaObjectName );

                STL_TRY( ellGetSchemaDictHandle( aTransID,
                                                 aSQLStmt->mViewSCN,
                                                 sNonSchemaObject,
                                                 & sInitPlan->mObjectHandle,
                                                 & sObjectExist,
                                                 ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXIST );

                /**
                 * Built-In Schema 인지 확인
                 */

                STL_TRY_THROW( ellIsBuiltInSchema( & sInitPlan->mObjectHandle ) ==
                               STL_FALSE, RAMP_ERR_BUILT_IN_SCHEMA );
                
                break;
            }
        case ELL_OBJECT_TABLESPACE:
            {
                /**
                 * Tablespace Dictionary Handle 획득
                 */

                sNonSchemaObject =
                    QLP_GET_PTR_VALUE( sParseTree->mCommentObject->mNonSchemaObjectName );

                STL_TRY( ellGetTablespaceDictHandle( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     sNonSchemaObject,
                                                     & sInitPlan->mObjectHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SPACE_NOT_EXIST );

                /**
                 * Built-In Tablespace 인지 확인
                 */

                STL_TRY_THROW( ellIsBuiltInTablespace( & sInitPlan->mObjectHandle ) ==
                               STL_FALSE, RAMP_ERR_BUILT_IN_SPACE );
                
                break;
            }
        case ELL_OBJECT_AUTHORIZATION:
            {
                /**
                 * Authorization Dictionary Handle 획득
                 */
                
                sNonSchemaObject =
                    QLP_GET_PTR_VALUE( sParseTree->mCommentObject->mNonSchemaObjectName );

                STL_TRY( ellGetAuthDictHandle( aTransID,
                                               aSQLStmt->mViewSCN,
                                               sNonSchemaObject,
                                               & sInitPlan->mObjectHandle,
                                               & sObjectExist,
                                               ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_AUTH_NOT_EXIST );

                /**
                 * Built-In Authorization 인지 확인
                 */

                STL_TRY_THROW( ellIsBuiltInAuth( & sInitPlan->mObjectHandle ) ==
                               STL_FALSE, RAMP_ERR_BUILT_IN_AUTH );
                break;
            }
        case ELL_OBJECT_PROFILE:
            {
                /**
                 * Profile Dictionary Handle 획득
                 */
                
                sNonSchemaObject =
                    QLP_GET_PTR_VALUE( sParseTree->mCommentObject->mNonSchemaObjectName );

                STL_TRY( ellGetProfileDictHandleByName( aTransID,
                                                        aSQLStmt->mViewSCN,
                                                        sNonSchemaObject,
                                                        & sInitPlan->mObjectHandle,
                                                        & sObjectExist,
                                                        ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_PROFILE_NOT_EXIST );

                break;
            }
        case ELL_OBJECT_CATALOG:
            {
                /**
                 * Cataloga Dictionary Handle 획득
                 */
                
                STL_TRY( ellGetCatalogDictHandle( aTransID,
                                                  & sInitPlan->mObjectHandle,
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

    /****************************************************
     * COMMENT ON 권한 검사 
     ****************************************************/

    STL_TRY( qlaCheckPrivCommentObject( aTransID,
                                        aDBCStmt,
                                        aSQLStmt,
                                        sInitPlan->mObjectType,
                                        & sInitPlan->mObjectHandle,
                                        aEnv )
             == STL_SUCCESS );
                                        
    /****************************************************
     * Comment String 정보 획득 
     ****************************************************/

    sCommentString = QLP_GET_PTR_VALUE( sParseTree->mCommentString );

    if ( sCommentString == NULL )
    {
        sInitPlan->mCommentString = NULL;
    }
    else
    {
        sCommentLength = stlStrlen( sCommentString );
        
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    sCommentLength + 1,
                                    (void **) & sInitPlan->mCommentString,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemcpy( sInitPlan->mCommentString, sCommentString, sCommentLength );
        sInitPlan->mCommentString[sCommentLength] = '\0';
    }

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mObjectHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Object 가 종속된 Table Handle 이 존재할 경우 Handle 유효성 검사
     */
    
    if ( ellGetObjectType( & sInitPlan->mRelatedTableHandle ) != ELL_OBJECT_NA )
    {
        STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                      & sInitPlan->mRelatedTableHandle,
                                      QLL_INIT_PLAN(aDBCStmt),
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * Table 에 종속된 Object 가 아님
         */
    }
    
    /****************************************************
     * Init Plan 연결 
     ****************************************************/

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    /**
     * COLUMN COMMENT ERROR
     */
    
    STL_CATCH( RAMP_ERR_QUALIFIED_COLUMN_SCHEMA_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sColumnObject->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sColumnObject->mSchema );
    }

    STL_CATCH( RAMP_ERR_COLUMN_TABLE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_OR_VIEW_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sColumnObject->mTablePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sColumnObject->mTable );
    }

    STL_CATCH( RAMP_ERR_BUILT_IN_COLUMN_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_TABLE_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sColumnObject->mTablePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sColumnObject->mTable );
    }

    STL_CATCH( RAMP_ERR_COLUMN_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sColumnObject->mColumnPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sColumnObject->mColumn );
    }

    /**
     * TABLE COMMENT ERROR
     */
    
    STL_CATCH( RAMP_ERR_QUALIFIED_SCHEMA_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sSchemaObject->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sSchemaObject->mSchema );
    }

    STL_CATCH( RAMP_ERR_TABLE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_OR_VIEW_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sSchemaObject->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sSchemaObject->mObject );
    }

    STL_CATCH( RAMP_ERR_BUILT_IN_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_TABLE_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sSchemaObject->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sSchemaObject->mObject );
    }

    /**
     * CONSTRAINT COMMENT ERROR
     */
    
    STL_CATCH( RAMP_ERR_CONSTRAINT_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CONSTRAINT_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sSchemaObject->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sSchemaObject->mObject );
    }

    STL_CATCH( RAMP_ERR_BUILT_IN_CONSTRAINT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_CONSTRAINT_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sSchemaObject->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sSchemaObject->mObject );
    }

    /**
     * INDEX COMMENT ERROR
     */
    
    STL_CATCH( RAMP_ERR_INDEX_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INDEX_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sSchemaObject->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sSchemaObject->mObject );
    }

    STL_CATCH( RAMP_ERR_BUILT_IN_INDEX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_INDEX_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sSchemaObject->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sSchemaObject->mObject );
    }

    STL_CATCH( RAMP_ERR_INDEX_WITH_KEY_CONSTRAINT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INDEX_WITH_KEY_CONSTRAINT,
                      qlgMakeErrPosString( aSQLString,
                                           sSchemaObject->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    /**
     * SEQUENCE COMMENT ERROR
     */
    
    STL_CATCH( RAMP_ERR_SEQUENCE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SEQUENCE_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sSchemaObject->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_BUILT_IN_SEQUENCE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_SEQUENCE_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sSchemaObject->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sSchemaObject->mObject );
    }

    /**
     * SCHEMA COMMENT ERROR
     */
    
    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCommentObject->mNonSchemaObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sNonSchemaObject );
    }

    STL_CATCH( RAMP_ERR_BUILT_IN_SCHEMA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_SCHEMA_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCommentObject->mNonSchemaObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sNonSchemaObject );
    }

    /**
     * TABLESPACE COMMENT ERROR
     */
    
    STL_CATCH( RAMP_ERR_SPACE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCommentObject->mNonSchemaObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sNonSchemaObject );
    }

    STL_CATCH( RAMP_ERR_BUILT_IN_SPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_TABLESPACE_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCommentObject->mNonSchemaObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sNonSchemaObject );
    }
    
    /**
     * AUTHORIZATION COMMENT ERROR
     */
    
    STL_CATCH( RAMP_ERR_AUTH_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_AUTHORIZATION_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCommentObject->mNonSchemaObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sNonSchemaObject );
    }

    STL_CATCH( RAMP_ERR_BUILT_IN_AUTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_AUTHORIZATION_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCommentObject->mNonSchemaObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sNonSchemaObject );
    }

    /**
     * PROFILE COMMENT ERROR
     */
    
    STL_CATCH( RAMP_ERR_PROFILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PROFILE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCommentObject->mNonSchemaObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sNonSchemaObject );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief COMMENT ON 구문의 Code Plan 을 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeCommentStmt( smlTransId      aTransID,
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
 * @brief COMMENT ON 구문의 Data Plan 을 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataCommentStmt( smlTransId      aTransID,
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
 * @brief COMMENT ON 구문을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteCommentStmt( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    qldInitCommentStmt * sInitPlan = NULL;

    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_COMMENT_ON_TYPE,
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

    sInitPlan = (qldInitCommentStmt *) aSQLStmt->mInitPlan;
    
    /**
     * COMMENT 구문을 위한 Lock 획득
     */

    STL_TRY( ellLock4SetObjectComment( aTransID,
                                       aStmt,
                                       & sInitPlan->mObjectHandle,
                                       & sInitPlan->mRelatedTableHandle,
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

    sInitPlan = (qldInitCommentStmt *) aSQLStmt->mInitPlan;

    /***************************************************************
     * COMMENT 수행
     ***************************************************************/
    
    /**
     * COMMENT 설정 
     */

    STL_TRY( ellSetObjectComment( aTransID,
                                  aStmt,
                                  & sInitPlan->mObjectHandle,
                                  sInitPlan->mCommentString,
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
                                  
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qldComment */
