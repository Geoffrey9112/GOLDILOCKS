/*******************************************************************************
 * qlaPrivCheck.c
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
 * @file qlaPrivCheck.c
 * @brief Privilege Check Logic
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlaPrivCheck Privilege Check 
 * @ingroup qla
 * @{
 */

/*****************************************************************************
 * privilege for COMMENT ON object
 *****************************************************************************/

/**
 * @brief COMMENT ON 구문을 위한 권한 검사
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aObjectType    Object 유형
 * @param[in] aObjectHandle  Object Handle
 * @param[in] aEnv           Environment
 */
stlStatus qlaCheckPrivCommentObject( smlTransId              aTransID,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     ellObjectType           aObjectType,
                                     ellDictHandle         * aObjectHandle,
                                     qllEnv                * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    stlInt64        sAuthID = ELL_DICT_OBJECT_ID_NA;

    ellPrivObject sPrivObject = ELL_PRIV_NA;
    ellDictHandle sPrivHandle;
    stlBool       sPrivExist  = STL_FALSE;

    stlInt64      sOwnerID  = ELL_DICT_OBJECT_ID_NA;
    stlInt64      sSchemaID = ELL_DICT_OBJECT_ID_NA;
    stlInt64      sTableID  = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aObjectType > ELL_OBJECT_NA) && (aObjectType < ELL_OBJECT_MAX),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHandle != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * 기본 정보 획득
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sAuthID = ellGetAuthID( sAuthHandle );

    /**
     * 객체 유형별 COMMENT 권한 검사
     */

    switch ( aObjectType )
    {
        case ELL_OBJECT_COLUMN:
        case ELL_OBJECT_TABLE:
            {
                if ( aObjectType == ELL_OBJECT_COLUMN )
                {
                    sTableID = ellGetColumnTableID( aObjectHandle );
                    sSchemaID = ellGetColumnSchemaID( aObjectHandle );
                    sOwnerID = ellGetColumnOwnerID( aObjectHandle );
                }
                else
                {
                    sTableID = ellGetTableID( aObjectHandle );
                    sSchemaID = ellGetTableSchemaID( aObjectHandle );
                    sOwnerID = ellGetTableOwnerID( aObjectHandle );
                }

                /**
                 * Owner 인지 검사
                 */

                if ( sOwnerID == sAuthID )
                {
                    stlMemcpy( & sPrivHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );

                    sPrivObject = ELL_PRIV_OWNER;
                    sPrivExist = STL_TRUE;
                    break;
                }
                else
                {
                    /* nothing to do */
                }

                /**
                 * CONTROL TABLE ON TABLE 검사 
                 */
                
                sPrivObject = ELL_PRIV_TABLE;
                STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sPrivObject,
                                                  sTableID,
                                                  ELL_TABLE_PRIV_ACTION_CONTROL_TABLE,
                                                  STL_FALSE, /* aWithGrant */
                                                  & sPrivHandle,
                                                  & sPrivExist,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                if ( sPrivExist == STL_TRUE )
                {
                    break;
                }
                else
                {
                    /* go go */
                }

                /**
                 * CONTROL SCHEMA ON SCHEMA 검사 
                 */
                
                sPrivObject = ELL_PRIV_SCHEMA;
                STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sPrivObject,
                                                  sSchemaID,
                                                  ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA,
                                                  STL_FALSE, /* aWithGrant */
                                                  & sPrivHandle,
                                                  & sPrivExist,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                if ( sPrivExist == STL_TRUE )
                {
                    break;
                }
                else
                {
                    /* go go */
                }

                /**
                 * ALTER ANY TABLE ON DATABASE 검사 
                 */
                
                sPrivObject = ELL_PRIV_DATABASE;
                STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sPrivObject,
                                                  ellGetDbCatalogID(),
                                                  ELL_DB_PRIV_ACTION_ALTER_ANY_TABLE,
                                                  STL_FALSE, /* aWithGrant */
                                                  & sPrivHandle,
                                                  & sPrivExist,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                break;
            }
        case ELL_OBJECT_CONSTRAINT:
            {
                sSchemaID = ellGetConstraintSchemaID( aObjectHandle );
                sOwnerID = ellGetConstraintOwnerID( aObjectHandle );

                /**
                 * Owner 인지 검사
                 */

                if ( sOwnerID == sAuthID )
                {
                    stlMemcpy( & sPrivHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );

                    sPrivObject = ELL_PRIV_OWNER;
                    sPrivExist = STL_TRUE;
                    break;
                }
                else
                {
                    /* nothing to do */
                }
                
                /**
                 * CONTROL SCHEMA ON SCHEMA 검사 
                 */
                
                sPrivObject = ELL_PRIV_SCHEMA;
                STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sPrivObject,
                                                  sSchemaID,
                                                  ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA,
                                                  STL_FALSE, /* aWithGrant */
                                                  & sPrivHandle,
                                                  & sPrivExist,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                if ( sPrivExist == STL_TRUE )
                {
                    break;
                }
                else
                {
                    /* go go */
                }

                /**
                 * ALTER ANY TABLE ON DATABASE 검사 
                 */
                
                sPrivObject = ELL_PRIV_DATABASE;
                STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sPrivObject,
                                                  ellGetDbCatalogID(),
                                                  ELL_DB_PRIV_ACTION_ALTER_ANY_TABLE,
                                                  STL_FALSE, /* aWithGrant */
                                                  & sPrivHandle,
                                                  & sPrivExist,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                break;
            }
        case ELL_OBJECT_INDEX:
            {
                sSchemaID = ellGetIndexSchemaID( aObjectHandle );
                sOwnerID = ellGetIndexOwnerID( aObjectHandle );

                /**
                 * Owner 인지 검사
                 */

                if ( sOwnerID == sAuthID )
                {
                    stlMemcpy( & sPrivHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );

                    sPrivObject = ELL_PRIV_OWNER;
                    sPrivExist = STL_TRUE;
                    break;
                }
                else
                {
                    /* nothing to do */
                }
                
                /**
                 * CONTROL SCHEMA ON SCHEMA 검사 
                 */
                
                sPrivObject = ELL_PRIV_SCHEMA;
                STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sPrivObject,
                                                  sSchemaID,
                                                  ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA,
                                                  STL_FALSE, /* aWithGrant */
                                                  & sPrivHandle,
                                                  & sPrivExist,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                if ( sPrivExist == STL_TRUE )
                {
                    break;
                }
                else
                {
                    /* go go */
                }

                /**
                 * ALTER ANY INDEX ON DATABASE 검사 
                 */
                
                sPrivObject = ELL_PRIV_DATABASE;
                STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sPrivObject,
                                                  ellGetDbCatalogID(),
                                                  ELL_DB_PRIV_ACTION_ALTER_ANY_INDEX,
                                                  STL_FALSE, /* aWithGrant */
                                                  & sPrivHandle,
                                                  & sPrivExist,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                
                break;
            }
        case ELL_OBJECT_SEQUENCE:
            {
                sSchemaID = ellGetSequenceSchemaID( aObjectHandle );
                sOwnerID = ellGetSequenceOwnerID( aObjectHandle );

                /**
                 * Owner 인지 검사
                 */

                if ( sOwnerID == sAuthID )
                {
                    stlMemcpy( & sPrivHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );

                    sPrivObject = ELL_PRIV_OWNER;
                    sPrivExist = STL_TRUE;
                    break;
                }
                else
                {
                    /* nothing to do */
                }
                
                /**
                 * CONTROL SCHEMA ON SCHEMA 검사 
                 */
                
                sPrivObject = ELL_PRIV_SCHEMA;
                STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sPrivObject,
                                                  sSchemaID,
                                                  ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA,
                                                  STL_FALSE, /* aWithGrant */
                                                  & sPrivHandle,
                                                  & sPrivExist,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                if ( sPrivExist == STL_TRUE )
                {
                    break;
                }
                else
                {
                    /* go go */
                }

                /**
                 * ALTER ANY SEQUENCE ON DATABASE 검사 
                 */
                
                sPrivObject = ELL_PRIV_DATABASE;
                STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sPrivObject,
                                                  ellGetDbCatalogID(),
                                                  ELL_DB_PRIV_ACTION_ALTER_ANY_SEQUENCE,
                                                  STL_FALSE, /* aWithGrant */
                                                  & sPrivHandle,
                                                  & sPrivExist,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                break;
            }
        case ELL_OBJECT_SCHEMA:
            {
                /**
                 * Owner 인지 검사
                 */
                
                if ( ellGetSchemaOwnerID( aObjectHandle ) == sAuthID )
                {
                    stlMemcpy( & sPrivHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );
                    
                    sPrivObject = ELL_PRIV_OWNER;
                    sPrivExist = STL_TRUE;
                }
                else
                {
                    /* nothing to do */
                }

                if ( sPrivExist == STL_TRUE )
                {
                    break;
                }
                else
                {
                    /* go go */
                }
                
                /**
                 * CONTROL SCHEMA ON SCHEMA 검사 
                 */
                
                sSchemaID = ellGetSchemaID( aObjectHandle );
                
                sPrivObject = ELL_PRIV_SCHEMA;
                STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sPrivObject,
                                                  sSchemaID,
                                                  ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA,
                                                  STL_FALSE, /* aWithGrant */
                                                  & sPrivHandle,
                                                  & sPrivExist,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                if ( sPrivExist == STL_TRUE )
                {
                    break;
                }
                else
                {
                    /* go go */
                }
                
                
                /**
                 * ALTER SCHEMA ON DATABASE 검사
                 */

                sPrivObject = ELL_PRIV_DATABASE;
                STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sPrivObject,
                                                  ellGetDbCatalogID(),
                                                  ELL_DB_PRIV_ACTION_ALTER_SCHEMA,
                                                  STL_FALSE, /* aWithGrant */
                                                  & sPrivHandle,
                                                  & sPrivExist,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                break;
            }
        case ELL_OBJECT_TABLESPACE:
            {
                /**
                 * ALTER TABLESPACE ON DATABASE 검사
                 */

                sPrivObject = ELL_PRIV_DATABASE;
                STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sPrivObject,
                                                  ellGetDbCatalogID(),
                                                  ELL_DB_PRIV_ACTION_ALTER_TABLESPACE,
                                                  STL_FALSE, /* aWithGrant */
                                                  & sPrivHandle,
                                                  & sPrivExist,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case ELL_OBJECT_AUTHORIZATION:
            {
                if ( ellGetAuthType( aObjectHandle ) == ELL_AUTHORIZATION_TYPE_USER )
                {
                    /**
                     * ALTER USER ON DATABASE 검사
                     */

                    sPrivObject = ELL_PRIV_DATABASE;
                    STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                      aSQLStmt->mViewSCN,
                                                      sPrivObject,
                                                      ellGetDbCatalogID(),
                                                      ELL_DB_PRIV_ACTION_ALTER_USER,
                                                      STL_FALSE, /* aWithGrant */
                                                      & sPrivHandle,
                                                      & sPrivExist,
                                                      ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                }
                else
                {
                    /**
                     * ALTER ROLE ON DATABASE 검사
                     */
                    
                    sPrivObject = ELL_PRIV_DATABASE;
                    STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                      aSQLStmt->mViewSCN,
                                                      sPrivObject,
                                                      ellGetDbCatalogID(),
                                                      ELL_DB_PRIV_ACTION_ALTER_ROLE,
                                                      STL_FALSE, /* aWithGrant */
                                                      & sPrivHandle,
                                                      & sPrivExist,
                                                      ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                    STL_DASSERT( ellGetAuthType( aObjectHandle ) == ELL_AUTHORIZATION_TYPE_ROLE );
                }
                break;
            }
        case ELL_OBJECT_PROFILE:
            {
                /**
                 * ALTER PROFILE ON DATABASE 검사
                 */
                
                sPrivObject = ELL_PRIV_DATABASE;
                STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                      aSQLStmt->mViewSCN,
                                                      sPrivObject,
                                                      ellGetDbCatalogID(),
                                                      ELL_DB_PRIV_ACTION_ALTER_PROFILE,
                                                      STL_FALSE, /* aWithGrant */
                                                      & sPrivHandle,
                                                      & sPrivExist,
                                                      ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case ELL_OBJECT_CATALOG:
            {
                /**
                 * ALTER DATABASE ON DATABASE 검사
                 */

                sPrivObject = ELL_PRIV_DATABASE;
                STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sPrivObject,
                                                  ellGetDbCatalogID(),
                                                  ELL_DB_PRIV_ACTION_ALTER_DATABASE,
                                                  STL_FALSE, /* aWithGrant */
                                                  & sPrivHandle,
                                                  & sPrivExist,
                                                  ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        default:
            STL_ASSERT(0);
            break;
    }
    
    
    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    /**
     * Granted Privilege 를 추가
     */

    STL_TRY( qllAddGrantedPriv( aDBCStmt,
                                aSQLStmt,
                                sPrivObject,
                                & sPrivHandle,
                                aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INSUFFICENT_PRIVILEGES,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/*****************************************************************************
 * privilege for INDEX object
 *****************************************************************************/


/**
 * @brief DROP INDEX 권한 검사
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aIndexHandle   Index Handle
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckPrivDropIndex( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 ellDictHandle         * aIndexHandle,
                                 qllEnv                * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    stlInt64        sAuthID = ELL_DICT_OBJECT_ID_NA;
    
    ellDictHandle sPrivHandle;
    stlBool       sPrivExist  = STL_FALSE;

    ellPrivObject sPrivObject = ELL_PRIV_NA;
    stlInt32      sPrivAction = 0;

    stlInt64      sObjectID = ELL_DICT_OBJECT_ID_NA;
    ellDictHandle sObjectHandle;
    stlBool       sObjectExist = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexHandle != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sAuthID = ellGetAuthID( sAuthHandle );
    
    while (1)
    {
        /**
         * INDEX Owner 인지 검사
         */

        sPrivObject = ELL_PRIV_OWNER;
        stlMemcpy( & sPrivHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );

        if ( ellGetIndexOwnerID( aIndexHandle ) == sAuthID )
        {
            sPrivExist = STL_TRUE;
            break;
        }

        /*****************************************************************
         * 테이블 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 테이블 정보 획득
         */

        sObjectID = ellGetIndexTableID( aIndexHandle );

        STL_TRY( ellGetTableDictHandleByID( aTransID,
                                            aSQLStmt->mViewSCN,
                                            sObjectID,
                                            & sObjectHandle,
                                            & sObjectExist,
                                            ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_DASSERT( sObjectExist == STL_TRUE );
        
        /**
         * 권한이 없다면, TABLE OWNER 인지 검사
         */

        sPrivObject = ELL_PRIV_OWNER;
        stlMemcpy( & sPrivHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );
        
        if ( ellGetTableOwnerID( & sObjectHandle ) == sAuthID )
        {
            sPrivExist = STL_TRUE;
            break;
        }

        /**
         * 권한이 없다면, CONTROL TABLE ON TABLE 권한 검사
         */

        sPrivObject = ELL_PRIV_TABLE;
        sPrivAction = ELL_TABLE_PRIV_ACTION_CONTROL_TABLE;
        
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /*****************************************************************
         * 스키마 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 스키마 정보 획득
         */

        sObjectID = ellGetIndexSchemaID( aIndexHandle );

        STL_TRY( ellGetSchemaDictHandleByID( aTransID,
                                             aSQLStmt->mViewSCN,
                                             sObjectID,
                                             & sObjectHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_DASSERT( sObjectExist == STL_TRUE );
        
        /**
         * 권한이 없다면, DROP INDEX ON SCHEMA 권한을 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = ELL_SCHEMA_PRIV_ACTION_DROP_INDEX;

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /**
         * 권한이 없다면, CONTROL SCHEMA ON SCHEMA 권한을 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA;

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /*****************************************************************
         * DB 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 권한이 없다면, DROP ANY INDEX ON DATABASE 권한을 검사 
         */
        
        sPrivObject = ELL_PRIV_DATABASE;
        sPrivAction = ELL_DB_PRIV_ACTION_DROP_ANY_INDEX;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetDbCatalogID(),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        break;
    }    

    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    /**
     * Granted Privilege 를 추가
     */

    STL_TRY( qllAddGrantedPriv( aDBCStmt,
                                aSQLStmt,
                                sPrivObject,
                                & sPrivHandle,
                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INSUFFICENT_PRIVILEGES,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ALTER INDEX 권한 검사
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aIndexHandle   Index Handle
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckPrivAlterIndex( smlTransId              aTransID,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  ellDictHandle         * aIndexHandle,
                                  qllEnv                * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    stlInt64        sAuthID = ELL_DICT_OBJECT_ID_NA;
    
    ellDictHandle sPrivHandle;
    stlBool       sPrivExist  = STL_FALSE;

    ellPrivObject sPrivObject = ELL_PRIV_NA;
    stlInt32      sPrivAction = 0;

    stlInt64      sObjectID = ELL_DICT_OBJECT_ID_NA;
    ellDictHandle sObjectHandle;
    stlBool       sObjectExist = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexHandle != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sAuthID = ellGetAuthID( sAuthHandle );
    
    while (1)
    {
        /**
         * INDEX Owner 인지 검사
         */

        sPrivObject = ELL_PRIV_OWNER;
        stlMemcpy( & sPrivHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );

        if ( ellGetIndexOwnerID( aIndexHandle ) == sAuthID )
        {
            sPrivExist = STL_TRUE;
            break;
        }

        /*****************************************************************
         * 테이블 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 테이블 정보 획득
         */

        sObjectID = ellGetIndexTableID( aIndexHandle );

        STL_TRY( ellGetTableDictHandleByID( aTransID,
                                            aSQLStmt->mViewSCN,
                                            sObjectID,
                                            & sObjectHandle,
                                            & sObjectExist,
                                            ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_DASSERT( sObjectExist == STL_TRUE );
        
        /**
         * 권한이 없다면, TABLE OWNER 인지 검사
         */

        sPrivObject = ELL_PRIV_OWNER;
        stlMemcpy( & sPrivHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );
        
        if ( ellGetTableOwnerID( & sObjectHandle ) == sAuthID )
        {
            sPrivExist = STL_TRUE;
            break;
        }

        /**
         * 권한이 없다면, CONTROL TABLE ON TABLE 권한 검사
         */

        sPrivObject = ELL_PRIV_TABLE;
        sPrivAction = ELL_TABLE_PRIV_ACTION_CONTROL_TABLE;
        
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /*****************************************************************
         * 스키마 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 스키마 정보 획득
         */

        sObjectID = ellGetIndexSchemaID( aIndexHandle );

        STL_TRY( ellGetSchemaDictHandleByID( aTransID,
                                             aSQLStmt->mViewSCN,
                                             sObjectID,
                                             & sObjectHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_DASSERT( sObjectExist == STL_TRUE );
        
        /**
         * 권한이 없다면, ALTER INDEX ON SCHEMA 권한을 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = ELL_SCHEMA_PRIV_ACTION_ALTER_INDEX;

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /**
         * 권한이 없다면, CONTROL SCHEMA ON SCHEMA 권한을 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA;

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /*****************************************************************
         * DB 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 권한이 없다면, ALTER ANY INDEX ON DATABASE 권한을 검사 
         */
        
        sPrivObject = ELL_PRIV_DATABASE;
        sPrivAction = ELL_DB_PRIV_ACTION_ALTER_ANY_INDEX;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetDbCatalogID(),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        break;
    }    

    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    /**
     * Granted Privilege 를 추가
     */

    STL_TRY( qllAddGrantedPriv( aDBCStmt,
                                aSQLStmt,
                                sPrivObject,
                                & sPrivHandle,
                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INSUFFICENT_PRIVILEGES,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/*****************************************************************************
 * privilege for SEQUENCE object
 *****************************************************************************/

/**
 * @brief DROP SEQUENCE 권한 검사
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aSeqHandle     Sequence Handle
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckPrivDropSequence( smlTransId              aTransID,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    ellDictHandle         * aSeqHandle,
                                    qllEnv                * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    stlInt64        sAuthID = ELL_DICT_OBJECT_ID_NA;
    
    ellDictHandle sPrivHandle;
    stlBool       sPrivExist  = STL_FALSE;

    ellPrivObject sPrivObject = ELL_PRIV_NA;
    stlInt32      sPrivAction = 0;

    stlInt64      sObjectID = ELL_DICT_OBJECT_ID_NA;
    ellDictHandle sObjectHandle;
    stlBool       sObjectExist = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqHandle != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sAuthID = ellGetAuthID( sAuthHandle );
    
    while (1)
    {
        /**
         * SEQUENCE Owner 인지 검사
         */

        sPrivObject = ELL_PRIV_OWNER;
        stlMemcpy( & sPrivHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );

        if ( ellGetSequenceOwnerID( aSeqHandle ) == sAuthID )
        {
            sPrivExist = STL_TRUE;
            break;
        }

        /*****************************************************************
         * 스키마 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 스키마 정보 획득
         */

        sObjectID = ellGetSequenceSchemaID( aSeqHandle );

        STL_TRY( ellGetSchemaDictHandleByID( aTransID,
                                             aSQLStmt->mViewSCN,
                                             sObjectID,
                                             & sObjectHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_DASSERT( sObjectExist == STL_TRUE );
        
        /**
         * 권한이 없다면, DROP SEQUENCE ON SCHEMA 권한을 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = ELL_SCHEMA_PRIV_ACTION_DROP_SEQUENCE;

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /**
         * 권한이 없다면, CONTROL SCHEMA ON SCHEMA 권한을 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA;

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /*****************************************************************
         * DB 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 권한이 없다면, DROP ANY SEQUENCE ON DATABASE 권한을 검사 
         */
        
        sPrivObject = ELL_PRIV_DATABASE;
        sPrivAction = ELL_DB_PRIV_ACTION_DROP_ANY_SEQUENCE;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetDbCatalogID(),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        break;
    }    

    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    /**
     * Granted Privilege 를 추가
     */

    STL_TRY( qllAddGrantedPriv( aDBCStmt,
                                aSQLStmt,
                                sPrivObject,
                                & sPrivHandle,
                                aEnv )
             == STL_SUCCESS );
                                
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INSUFFICENT_PRIVILEGES,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ALTER SEQUENCE 권한 검사
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aSeqHandle     Sequence Handle
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckPrivAlterSequence( smlTransId              aTransID,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     ellDictHandle         * aSeqHandle,
                                     qllEnv                * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    stlInt64        sAuthID = ELL_DICT_OBJECT_ID_NA;
    
    ellDictHandle sPrivHandle;
    stlBool       sPrivExist  = STL_FALSE;

    ellPrivObject sPrivObject = ELL_PRIV_NA;
    stlInt32      sPrivAction = 0;

    stlInt64      sObjectID = ELL_DICT_OBJECT_ID_NA;
    ellDictHandle sObjectHandle;
    stlBool       sObjectExist = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqHandle != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sAuthID = ellGetAuthID( sAuthHandle );
    
    while (1)
    {
        /**
         * SEQUENCE Owner 인지 검사
         */

        sPrivObject = ELL_PRIV_OWNER;
        stlMemcpy( & sPrivHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );

        if ( ellGetSequenceOwnerID( aSeqHandle ) == sAuthID )
        {
            sPrivExist = STL_TRUE;
            break;
        }

        /*****************************************************************
         * 스키마 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 스키마 정보 획득
         */

        sObjectID = ellGetSequenceSchemaID( aSeqHandle );

        STL_TRY( ellGetSchemaDictHandleByID( aTransID,
                                             aSQLStmt->mViewSCN,
                                             sObjectID,
                                             & sObjectHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_DASSERT( sObjectExist == STL_TRUE );
        
        /**
         * 권한이 없다면, ALTER SEQUENCE ON SCHEMA 권한을 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = ELL_SCHEMA_PRIV_ACTION_ALTER_SEQUENCE;

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /**
         * 권한이 없다면, CONTROL SCHEMA ON SCHEMA 권한을 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA;

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /*****************************************************************
         * DB 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 권한이 없다면, ALTER ANY SEQUENCE ON DATABASE 권한을 검사 
         */
        
        sPrivObject = ELL_PRIV_DATABASE;
        sPrivAction = ELL_DB_PRIV_ACTION_ALTER_ANY_SEQUENCE;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetDbCatalogID(),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        break;
    }    

    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    /**
     * Granted Privilege 를 추가
     */

    STL_TRY( qllAddGrantedPriv( aDBCStmt,
                                aSQLStmt,
                                sPrivObject,
                                & sPrivHandle,
                                aEnv )
             == STL_SUCCESS );
                                
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INSUFFICENT_PRIVILEGES,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/*****************************************************************************
 * privilege for DROP object
 *****************************************************************************/



/**
 * @brief DROP USER 권한 검사
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckPrivDropUser( smlTransId              aTransID,
                                qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllEnv                * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * DROP USER ON DATABASE 권한 검사
     */
    
    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_DROP_USER,
                                   aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief DROP TABLESPACE 권한 검사
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckPrivDropSpace( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllEnv                * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * DROP TABLESPACE ON DATABASE 권한 검사
     */
    
    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_DROP_TABLESPACE,
                                   aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief DROP SCHEMA 권한 검사
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aSchemaHandle  Schema Handle
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckPrivDropSchema( smlTransId              aTransID,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  ellDictHandle         * aSchemaHandle,
                                  qllEnv                * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    stlInt64        sAuthID = ELL_DICT_OBJECT_ID_NA;
    
    ellDictHandle sPrivHandle;
    stlBool       sPrivExist  = STL_FALSE;

    ellPrivObject sPrivObject = ELL_PRIV_NA;
    stlInt32      sPrivAction = 0;

    stlInt64      sObjectID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sAuthID = ellGetAuthID( sAuthHandle );
    
    while (1)
    {
        /*****************************************************************
         * 스키마 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * SCHEMA Owner 인지 검사
         */

        sPrivObject = ELL_PRIV_OWNER;
        stlMemcpy( & sPrivHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );

        if ( ellGetSchemaOwnerID( aSchemaHandle ) == sAuthID )
        {
            sPrivExist = STL_TRUE;
            break;
        }

        /**
         * 권한이 없다면, CONTROL SCHEMA ON SCHEMA 권한을 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA;
        sObjectID   = ellGetSchemaID( aSchemaHandle );

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /*****************************************************************
         * DB 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 권한이 없다면, DROP SCHEMA ON DATABASE 권한을 검사 
         */
        
        sPrivObject = ELL_PRIV_DATABASE;
        sPrivAction = ELL_DB_PRIV_ACTION_DROP_SCHEMA;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetDbCatalogID(),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        break;
    }    

    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    /**
     * Granted Privilege 를 추가
     */

    STL_TRY( qllAddGrantedPriv( aDBCStmt,
                                aSQLStmt,
                                sPrivObject,
                                & sPrivHandle,
                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INSUFFICENT_PRIVILEGES,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DROP TABLE 권한 검사
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aTableHandle   Table Handle
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckPrivDropTable( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 ellDictHandle         * aTableHandle,
                                 qllEnv                * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    stlInt64        sAuthID = ELL_DICT_OBJECT_ID_NA;
    
    ellDictHandle sPrivHandle;
    stlBool       sPrivExist  = STL_FALSE;

    ellPrivObject sPrivObject = ELL_PRIV_NA;
    stlInt32      sPrivAction = 0;

    stlInt64      sObjectID = ELL_DICT_OBJECT_ID_NA;
    ellDictHandle sObjectHandle;
    stlBool       sObjectExist = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ellGetTableType(aTableHandle) == ELL_TABLE_TYPE_BASE_TABLE, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sAuthID = ellGetAuthID( sAuthHandle );
    
    while (1)
    {
        /*****************************************************************
         * 테이블 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * TABLE Owner 인지 검사
         */

        sPrivObject = ELL_PRIV_OWNER;
        stlMemcpy( & sPrivHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );

        if ( ellGetTableOwnerID( aTableHandle ) == sAuthID )
        {
            sPrivExist = STL_TRUE;
            break;
        }

        /**
         * 권한이 없다면, CONTROL TABLE ON TABLE 권한 검사
         */

        sObjectID = ellGetTableID( aTableHandle );
        
        sPrivObject = ELL_PRIV_TABLE;
        sPrivAction = ELL_TABLE_PRIV_ACTION_CONTROL_TABLE;
        
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /*****************************************************************
         * 스키마 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 스키마 정보 획득
         */

        sObjectID = ellGetTableSchemaID( aTableHandle );

        STL_TRY( ellGetSchemaDictHandleByID( aTransID,
                                             aSQLStmt->mViewSCN,
                                             sObjectID,
                                             & sObjectHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_DASSERT( sObjectExist == STL_TRUE );

        /**
         * 권한이 없다면, DROP TABLE ON SCHEMA 권한을 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = ELL_SCHEMA_PRIV_ACTION_DROP_TABLE;

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        /**
         * 권한이 없다면, CONTROL SCHEMA ON SCHEMA 권한을 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA;

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /*****************************************************************
         * DB 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 권한이 없다면, DROP ANY TABLE ON DATABASE 권한을 검사 
         */
        
        sPrivObject = ELL_PRIV_DATABASE;
        sPrivAction = ELL_DB_PRIV_ACTION_DROP_ANY_TABLE;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetDbCatalogID(),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        break;
    }    

    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    /**
     * Granted Privilege 를 추가
     */

    STL_TRY( qllAddGrantedPriv( aDBCStmt,
                                aSQLStmt,
                                sPrivObject,
                                & sPrivHandle,
                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INSUFFICENT_PRIVILEGES,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief DROP VIEW 권한 검사
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aViewHandle    View Handle
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckPrivDropView( smlTransId              aTransID,
                                qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                ellDictHandle         * aViewHandle,
                                qllEnv                * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    stlInt64        sAuthID = ELL_DICT_OBJECT_ID_NA;
    
    ellDictHandle sPrivHandle;
    stlBool       sPrivExist  = STL_FALSE;

    ellPrivObject sPrivObject = ELL_PRIV_NA;
    stlInt32      sPrivAction = 0;

    stlInt64      sObjectID = ELL_DICT_OBJECT_ID_NA;
    ellDictHandle sObjectHandle;
    stlBool       sObjectExist = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ellGetTableType(aViewHandle) == ELL_TABLE_TYPE_VIEW, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sAuthID = ellGetAuthID( sAuthHandle );
    
    while (1)
    {
        /*****************************************************************
         * 뷰 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * View Owner 인지 검사
         */

        sPrivObject = ELL_PRIV_OWNER;
        stlMemcpy( & sPrivHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );

        if ( ellGetTableOwnerID( aViewHandle ) == sAuthID )
        {
            sPrivExist = STL_TRUE;
            break;
        }

        /**
         * 권한이 없다면, CONTROL TABLE ON TABLE 권한 검사
         */

        sObjectID = ellGetTableID( aViewHandle );
        
        sPrivObject = ELL_PRIV_TABLE;
        sPrivAction = ELL_TABLE_PRIV_ACTION_CONTROL_TABLE;
        
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /*****************************************************************
         * 스키마 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 스키마 정보 획득
         */

        sObjectID = ellGetTableSchemaID( aViewHandle );

        STL_TRY( ellGetSchemaDictHandleByID( aTransID,
                                             aSQLStmt->mViewSCN,
                                             sObjectID,
                                             & sObjectHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_DASSERT( sObjectExist == STL_TRUE );

        /**
         * 권한이 없다면, DROP VIEW ON SCHEMA 권한을 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = ELL_SCHEMA_PRIV_ACTION_DROP_VIEW;

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        /**
         * 권한이 없다면, CONTROL SCHEMA ON SCHEMA 권한을 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA;

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /*****************************************************************
         * DB 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 권한이 없다면, DROP ANY VIEW ON DATABASE 권한을 검사 
         */
        
        sPrivObject = ELL_PRIV_DATABASE;
        sPrivAction = ELL_DB_PRIV_ACTION_DROP_ANY_VIEW;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetDbCatalogID(),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        break;
    }    

    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    /**
     * Granted Privilege 를 추가
     */

    STL_TRY( qllAddGrantedPriv( aDBCStmt,
                                aSQLStmt,
                                sPrivObject,
                                & sPrivHandle,
                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INSUFFICENT_PRIVILEGES,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}




stlStatus qlaCheckPrivDropSynonym( smlTransId              aTransID,
                                   qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   ellDictHandle         * aSynonymHandle,
                                   qllEnv                * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    stlInt64        sAuthID = ELL_DICT_OBJECT_ID_NA;
    
    ellDictHandle   sPrivHandle;
    stlBool         sPrivExist  = STL_FALSE;

    ellPrivObject   sPrivObject = ELL_PRIV_NA;
    stlInt32        sPrivAction = 0;

    stlInt64        sObjectID = ELL_DICT_OBJECT_ID_NA;
    ellDictHandle   sObjectHandle;
    stlBool         sObjectExist = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymHandle->mObjectType == ELL_OBJECT_SYNONYM,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sAuthID     = ellGetAuthID( sAuthHandle );
    
    while (1)
    {
        /*****************************************************************
         * Synonym 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * Synonym Owner 인지 검사
         */

        sPrivObject = ELL_PRIV_OWNER;
        stlMemcpy( & sPrivHandle, sAuthHandle, STL_SIZEOF(ellDictHandle) );

        if ( ellGetSynonymOwnerID( aSynonymHandle ) == sAuthID )
        {
            sPrivExist = STL_TRUE;
            break;
        }

        /*****************************************************************
         * 스키마 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 스키마 정보 획득
         */

        sObjectID = ellGetSynonymSchemaID( aSynonymHandle );

        STL_TRY( ellGetSchemaDictHandleByID( aTransID,
                                             aSQLStmt->mViewSCN,
                                             sObjectID,
                                             & sObjectHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_DASSERT( sObjectExist == STL_TRUE );

        /**
         * 권한이 없다면, DROP SYNONYM ON SCHEMA 권한을 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = ELL_SCHEMA_PRIV_ACTION_DROP_SYNONYM;

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        /**
         * 권한이 없다면, CONTROL SCHEMA ON SCHEMA 권한을 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA;

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          sObjectID,
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /*****************************************************************
         * DB 객체에 대한 검사 
         *****************************************************************/
        
        /**
         * 권한이 없다면, DROP ANY SYNONYM ON DATABASE 권한을 검사 
         */
        
        sPrivObject = ELL_PRIV_DATABASE;
        sPrivAction = ELL_DB_PRIV_ACTION_DROP_ANY_SYNONYM;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetDbCatalogID(),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        break;
    }    

    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    /**
     * Granted Privilege 를 추가
     */

    STL_TRY( qllAddGrantedPriv( aDBCStmt,
                                aSQLStmt,
                                sPrivObject,
                                & sPrivHandle,
                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INSUFFICENT_PRIVILEGES,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}







/*****************************************************************************
 * privilege ON DATABASE
 *****************************************************************************/


/**
 * @brief DATABASE 권한 검사
 * @param[in] aTransID         Transaction ID
 * @param[in] aDBCStmt         DBC Statement
 * @param[in] aSQLStmt         SQL Statement
 * @param[in] aDatabaseAction  Database Action
 * @param[in] aEnv             Environment
 * @remarks
 */
stlStatus qlaCheckDatabasePriv( smlTransId              aTransID,
                                qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                ellDatabasePrivAction   aDatabaseAction,
                                qllEnv                * aEnv )
{
    ellDictHandle sPrivHandle;
    stlBool       sPrivExist  = STL_FALSE;

    ellPrivObject sPrivObject = ELL_PRIV_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aDatabaseAction > ELL_DB_PRIV_ACTION_NA) &&
                        (aDatabaseAction < ELL_DB_PRIV_ACTION_MAX),
                        QLL_ERROR_STACK(aEnv) );

    while (1)
    {
        sPrivObject = ELL_PRIV_DATABASE;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetDbCatalogID(),
                                          aDatabaseAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        break;
    }    

    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    /**
     * Granted Privilege 를 추가
     */

    STL_TRY( qllAddGrantedPriv( aDBCStmt,
                                aSQLStmt,
                                sPrivObject,
                                & sPrivHandle,
                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_LACKS_PRIVILEGE_ON_DATABASE,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      gEllDatabasePrivActionString[aDatabaseAction] );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/*****************************************************************************
 * privilege ON TABLESPACE
 *****************************************************************************/

/**
 * @brief TABLESPACE 권한 검사
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aSpaceAction   Space Action
 * @param[in] aSpaceHandle   Space Handle
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckSpacePriv( smlTransId            aTransID,
                             qllDBCStmt          * aDBCStmt,
                             qllStatement        * aSQLStmt,
                             ellSpacePrivAction    aSpaceAction,
                             ellDictHandle       * aSpaceHandle,
                             qllEnv              * aEnv )
{
    ellDictHandle sPrivHandle;
    stlBool       sPrivExist  = STL_FALSE;

    ellPrivObject sPrivObject = ELL_PRIV_NA;
    stlInt32      sPrivAction = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSpaceAction > ELL_SPACE_PRIV_ACTION_NA) &&
                        (aSpaceAction < ELL_SPACE_PRIV_ACTION_MAX),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, QLL_ERROR_STACK(aEnv) );

    while (1)
    {
        /**
         * privilege ON TABLESPACE 권한이 있는지 검사
         */

        sPrivObject = ELL_PRIV_SPACE;
        sPrivAction = aSpaceAction;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetTablespaceID(aSpaceHandle),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /**
         * 권한이 없다면, 대응하는 privilege ON DATABASE 권한이 있는지 검사
         */

        switch (aSpaceAction)
        {
            case ELL_SPACE_PRIV_ACTION_CREATE_OBJECT:
                sPrivAction = ELL_DB_PRIV_ACTION_USAGE_TABLESPACE;
                break;
            default:
                STL_ASSERT(0);
                break;
        }
        
        sPrivObject = ELL_PRIV_DATABASE;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetDbCatalogID(),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        break;
    }    

    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    /**
     * Granted Privilege 를 추가
     */

    STL_TRY( qllAddGrantedPriv( aDBCStmt,
                                aSQLStmt,
                                sPrivObject,
                                & sPrivHandle,
                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_LACKS_PRIVILEGE_ON_TABLESPACE,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      gEllSpacePrivActionString[aSpaceAction],
                      ellGetTablespaceName(aSpaceHandle) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/*****************************************************************************
 * privilege ON SCHEMA
 *****************************************************************************/



/**
 * @brief SCHEMA 권한 검사
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aSchemaAction  Schema Action
 * @param[in] aSchemaHandle  Schema Handle
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckSchemaPriv( smlTransId            aTransID,
                              qllDBCStmt          * aDBCStmt,
                              qllStatement        * aSQLStmt,
                              ellSchemaPrivAction   aSchemaAction,
                              ellDictHandle       * aSchemaHandle,
                              qllEnv              * aEnv )
{
    ellDictHandle sPrivHandle;
    stlBool       sPrivExist  = STL_FALSE;

    ellPrivObject sPrivObject = ELL_PRIV_NA;
    stlInt32      sPrivAction = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSchemaAction > ELL_SCHEMA_PRIV_ACTION_NA) &&
                        (aSchemaAction < ELL_SCHEMA_PRIV_ACTION_MAX),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, QLL_ERROR_STACK(aEnv) );

    while (1)
    {
        /**
         * privilege ON SCHEMA 권한이 있는지 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        sPrivAction = aSchemaAction;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetSchemaID(aSchemaHandle),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /**
         * 권한이 없다면, CONTROL SCHMEA ON SCHEMA 권한이 있는지 검사
         */

        sPrivAction = ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA;
        sPrivObject = ELL_PRIV_SCHEMA;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetSchemaID(aSchemaHandle),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        /**
         * 권한이 없다면, 대응하는 privilege ON DATABASE 권한이 있는지 검사
         */

        switch (aSchemaAction)
        {
            case ELL_SCHEMA_PRIV_ACTION_CREATE_TABLE:
                sPrivAction = ELL_DB_PRIV_ACTION_CREATE_ANY_TABLE;
                break;
            case ELL_SCHEMA_PRIV_ACTION_CREATE_VIEW:
                sPrivAction = ELL_DB_PRIV_ACTION_CREATE_ANY_VIEW;
                break;
            case ELL_SCHEMA_PRIV_ACTION_CREATE_SEQUENCE:
                sPrivAction = ELL_DB_PRIV_ACTION_CREATE_ANY_SEQUENCE;
                break;
            case ELL_SCHEMA_PRIV_ACTION_CREATE_INDEX:
                sPrivAction = ELL_DB_PRIV_ACTION_CREATE_ANY_INDEX;
                break;
            case ELL_SCHEMA_PRIV_ACTION_ADD_CONSTRAINT:
                sPrivAction = ELL_DB_PRIV_ACTION_ALTER_ANY_TABLE;
                break;
            case ELL_SCHEMA_PRIV_ACTION_CREATE_SYNONYM:
                sPrivAction = ELL_DB_PRIV_ACTION_CREATE_ANY_SYNONYM;
                break;

            default:
                STL_ASSERT(0);
                break;
        }
        
        sPrivObject = ELL_PRIV_DATABASE;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetDbCatalogID(),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        break;
    }    

    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    /**
     * Granted Privilege 를 추가
     */

    STL_TRY( qllAddGrantedPriv( aDBCStmt,
                                aSQLStmt,
                                sPrivObject,
                                & sPrivHandle,
                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_LACKS_PRIVILEGES_ON_SCHEMA,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      gEllSchemaPrivActionString[aSchemaAction],
                      ellGetSchemaName(aSchemaHandle) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/*****************************************************************************
 * privilege ON TABLE
 *****************************************************************************/



/**
 * @brief TABLE 권한 검사
 * @param[in] aTransID      Transaction ID
 * @param[in] aDBCStmt      DBC Statement
 * @param[in] aSQLStmt      SQL Statement
 * @param[in] aTableAction  Table Action
 * @param[in] aTableHandle  Table Handle
 * @param[in] aColumnList   Column List (nullable)
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus qlaCheckTablePriv( smlTransId           aTransID,
                             qllDBCStmt         * aDBCStmt,
                             qllStatement       * aSQLStmt,
                             ellTablePrivAction   aTableAction,
                             ellDictHandle      * aTableHandle,
                             qlvRefExprList     * aColumnList,
                             qllEnv             * aEnv )
{
    ellDictHandle sPrivHandle;
    stlBool       sPrivExist  = STL_FALSE;

    ellPrivObject sPrivObject = ELL_PRIV_NA;
    stlInt32      sPrivAction = 0;

    stlBool       sCheckUpper = STL_FALSE;
    
    stlBool              sCheckColumnPriv  = STL_FALSE;
    ellColumnPrivAction  sColumnPrivAction = ELL_COLUMN_PRIV_ACTION_NA;

    stlInt64      sErrSchemaID;
    ellDictHandle sErrSchemaHandle;
    stlBool       sObjectExist = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aTableAction > ELL_TABLE_PRIV_ACTION_NA) &&
                        (aTableAction < ELL_TABLE_PRIV_ACTION_MAX),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );

    while (1)
    {
        /**
         * privilege ON TABLE 권한이 있는지 검사
         */

        sPrivObject = ELL_PRIV_TABLE;
        sPrivAction = aTableAction;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetTableID(aTableHandle),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /**
         * 권한이 없다면, CONTROL TABLE ON TABLE 권한이 있는지 검사
         */

        sPrivObject = ELL_PRIV_TABLE;
        sPrivAction = ELL_TABLE_PRIV_ACTION_CONTROL_TABLE;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetTableID(aTableHandle),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        /**
         * 권한이 없다면, 대응하는 privilege ON SCHEMA 권한이 있는지 검사
         */

        switch (aTableAction)
        {
            case ELL_TABLE_PRIV_ACTION_SELECT:
                sPrivAction = ELL_SCHEMA_PRIV_ACTION_SELECT_TABLE;
                sCheckUpper = STL_TRUE;
                break;
            case ELL_TABLE_PRIV_ACTION_INSERT:
                sPrivAction = ELL_SCHEMA_PRIV_ACTION_INSERT_TABLE;
                sCheckUpper = STL_TRUE;
                break;
            case ELL_TABLE_PRIV_ACTION_UPDATE:
                sPrivAction = ELL_SCHEMA_PRIV_ACTION_UPDATE_TABLE;
                sCheckUpper = STL_TRUE;
                break;
            case ELL_TABLE_PRIV_ACTION_DELETE:
                sPrivAction = ELL_SCHEMA_PRIV_ACTION_DELETE_TABLE;
                sCheckUpper = STL_TRUE;
                break;
            case ELL_TABLE_PRIV_ACTION_TRIGGER:
                sCheckUpper = STL_FALSE;
                break;
            case ELL_TABLE_PRIV_ACTION_REFERENCES:
                sCheckUpper = STL_FALSE;
                break;
            case ELL_TABLE_PRIV_ACTION_LOCK:
                sPrivAction = ELL_SCHEMA_PRIV_ACTION_LOCK_TABLE;
                sCheckUpper = STL_TRUE;
                break;
            case ELL_TABLE_PRIV_ACTION_INDEX:
                /**
                 * ELL_TABLE_PRIV_ACTION_INDEX 는 CREATE_INDEX 의 의미로 해석함.
                 * - DROP INDEX, ALTER INDEX 등은 처리 방식이 달라 별도의 함수로 처리함
                 */
                sPrivAction = ELL_SCHEMA_PRIV_ACTION_CREATE_INDEX;
                sCheckUpper = STL_TRUE;
                break;
            case ELL_TABLE_PRIV_ACTION_ALTER:
                sPrivAction = ELL_SCHEMA_PRIV_ACTION_ALTER_TABLE;
                sCheckUpper = STL_TRUE;
                break;
            default:
                STL_ASSERT(0);
                break;
        }

        if ( sCheckUpper == STL_TRUE )
        {
            sPrivObject = ELL_PRIV_SCHEMA;
            STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                              aSQLStmt->mViewSCN,
                                              sPrivObject,
                                              ellGetTableSchemaID(aTableHandle),
                                              sPrivAction,
                                              STL_FALSE, /* aWithGrant */
                                              & sPrivHandle,
                                              & sPrivExist,
                                              ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            if ( sPrivExist == STL_TRUE )
            {
                break;
            }
        }
        else
        {
            /* go go */
        }
        
        /**
         * 권한이 없다면, CONTROL SCHMEA ON SCHEMA 권한이 있는지 검사
         */

        sPrivAction = ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA;
        sPrivObject = ELL_PRIV_SCHEMA;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetTableSchemaID(aTableHandle),
                                          sPrivAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        /**
         * 권한이 없다면, 대응하는 privilege ON DATABASE 권한이 있는지 검사
         */

        switch (aTableAction)
        {
            case ELL_TABLE_PRIV_ACTION_SELECT:
                sPrivAction = ELL_DB_PRIV_ACTION_SELECT_ANY_TABLE;
                sCheckUpper = STL_TRUE;
                break;
            case ELL_TABLE_PRIV_ACTION_INSERT:
                sPrivAction = ELL_DB_PRIV_ACTION_INSERT_ANY_TABLE;
                sCheckUpper = STL_TRUE;
                break;
            case ELL_TABLE_PRIV_ACTION_UPDATE:
                sPrivAction = ELL_DB_PRIV_ACTION_UPDATE_ANY_TABLE;
                sCheckUpper = STL_TRUE;
                break;
            case ELL_TABLE_PRIV_ACTION_DELETE:
                sPrivAction = ELL_DB_PRIV_ACTION_DELETE_ANY_TABLE;
                sCheckUpper = STL_TRUE;
                break;
            case ELL_TABLE_PRIV_ACTION_TRIGGER:
                sCheckUpper = STL_FALSE;
                break;
            case ELL_TABLE_PRIV_ACTION_REFERENCES:
                sCheckUpper = STL_FALSE;
                break;
            case ELL_TABLE_PRIV_ACTION_LOCK:
                sPrivAction = ELL_DB_PRIV_ACTION_LOCK_ANY_TABLE;
                sCheckUpper = STL_TRUE;
                break;
            case ELL_TABLE_PRIV_ACTION_INDEX:
                /**
                 * ELL_TABLE_PRIV_ACTION_INDEX 는 CREATE_INDEX 의 의미로 해석함.
                 * - DROP INDEX, ALTER INDEX 등은 처리 방식이 달라 별도의 함수로 처리함
                 */
                sPrivAction = ELL_DB_PRIV_ACTION_CREATE_ANY_INDEX;
                sCheckUpper = STL_TRUE;
                break;
            case ELL_TABLE_PRIV_ACTION_ALTER:
                sPrivAction = ELL_DB_PRIV_ACTION_ALTER_ANY_TABLE;
                break;
            default:
                STL_ASSERT(0);
                break;
        }

        if ( sCheckUpper == STL_TRUE )
        {
            sPrivObject = ELL_PRIV_DATABASE;
            STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                              aSQLStmt->mViewSCN,
                                              sPrivObject,
                                              ellGetDbCatalogID(),
                                              sPrivAction,
                                              STL_FALSE, /* aWithGrant */
                                              & sPrivHandle,
                                              & sPrivExist,
                                              ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            if ( sPrivExist == STL_TRUE )
            {
                break;
            }
        }
        else
        {
            /* go go */
        }
        
        /**
         * Column Privilege 를 검사
         */

        switch (aTableAction)
        {
            case ELL_TABLE_PRIV_ACTION_SELECT:
                sColumnPrivAction = ELL_COLUMN_PRIV_ACTION_SELECT;
                sCheckColumnPriv = STL_TRUE;
                break;
            case ELL_TABLE_PRIV_ACTION_INSERT:
                sColumnPrivAction = ELL_COLUMN_PRIV_ACTION_INSERT;
                sCheckColumnPriv = STL_TRUE;
                break;
            case ELL_TABLE_PRIV_ACTION_UPDATE:
                sColumnPrivAction = ELL_COLUMN_PRIV_ACTION_UPDATE;
                sCheckColumnPriv = STL_TRUE;
                break;
            case ELL_TABLE_PRIV_ACTION_REFERENCES:
                sColumnPrivAction = ELL_COLUMN_PRIV_ACTION_REFERENCES;
                sCheckColumnPriv = STL_TRUE;
                break;
            default:
                /**
                 * Column Privilege 검사 대상이 아님
                 */
                sColumnPrivAction = ELL_COLUMN_PRIV_ACTION_NA;
                sCheckColumnPriv = STL_FALSE;
                break;
        }

        if ( (sCheckColumnPriv == STL_TRUE) && (aColumnList != NULL) )
        {
            STL_TRY( qlaCheckColumnPriv( aTransID,
                                         aDBCStmt,
                                         aSQLStmt,
                                         aTableHandle,
                                         sColumnPrivAction,
                                         aColumnList,
                                         aEnv )
                     == STL_SUCCESS );

            sPrivExist = STL_TRUE;
        }
        else
        {
            sPrivExist = STL_FALSE;
        }
        
        break;
    }    

    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    /**
     * Granted Privilege 를 추가
     */

    if ( sCheckColumnPriv == STL_TRUE )
    {
        /* 이미 추가됨 */
    }
    else
    {
        STL_TRY( qllAddGrantedPriv( aDBCStmt,
                                    aSQLStmt,
                                    sPrivObject,
                                    & sPrivHandle,
                                    aEnv )
                 == STL_SUCCESS );
    }
                                
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        sErrSchemaID = ellGetTableSchemaID(aTableHandle);

        if ( ellGetSchemaDictHandleByID( aTransID,
                                         aSQLStmt->mViewSCN,
                                         sErrSchemaID,
                                         & sErrSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS )
        {
            if ( sObjectExist == STL_TRUE )
            {
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              QLL_ERRCODE_LACKS_PRIVILEGES_ON_TABLE,
                              NULL,
                              QLL_ERROR_STACK(aEnv),
                              gEllTablePrivActionString[aTableAction],
                              ellGetSchemaName( & sErrSchemaHandle ),
                              ellGetTableName( aTableHandle ) );
            }
            else
            {
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              QLL_ERRCODE_INSUFFICENT_PRIVILEGES,
                              NULL,
                              QLL_ERROR_STACK(aEnv) );
            }
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_INSUFFICENT_PRIVILEGES,
                          NULL,
                          QLL_ERROR_STACK(aEnv) );
        }
    }
    
    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief COLUMN 권한 검사
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aTableHandle   Table Handle
 * @param[in] aColumnAction  Column Action
 * @param[in] aColumnList    Column List
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckColumnPriv( smlTransId            aTransID,
                              qllDBCStmt          * aDBCStmt,
                              qllStatement        * aSQLStmt,
                              ellDictHandle       * aTableHandle,
                              ellColumnPrivAction   aColumnAction,
                              qlvRefExprList      * aColumnList,
                              qllEnv              * aEnv )
{
    ellDictHandle sPrivHandle;
    stlBool       sPrivExist  = STL_FALSE;

    stlInt32         sColumnCount = 0;
    qlvRefExprItem * sColumnItem = NULL;
    ellDictHandle  * sColumnHandle = NULL;

    stlInt64      sErrSchemaID;
    ellDictHandle sErrSchemaHandle;
    stlBool       sObjectExist = STL_FALSE;
    
    stlInt32  i = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aColumnAction > ELL_COLUMN_PRIV_ACTION_NA) &&
                        (aColumnAction < ELL_COLUMN_PRIV_ACTION_MAX),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 모든 컬럼에 대한 권한을 만족해야 함.
     */

    sColumnCount = aColumnList->mCount;
    sColumnItem = aColumnList->mHead;

    /**
     * 검사할 컬럼이 없다면, column privilege 검사를 통과할 수 없음.
     * ex) SELECT COUNT(*) FROM T1;
     */
    
    STL_TRY_THROW( sColumnCount > 0, RAMP_ERR_INSUFFICIENT_PRIVILEGES );
                   
    for ( i = 0; i < sColumnCount; i++ )
    {
        /**
         * Column Handle 획득
         */
        
        sColumnHandle = sColumnItem->mExprPtr->mExpr.mColumn->mColumnHandle;
        
        /**
         * privilege COLUMN ON TABLE 권한이 있는지 검사
         */

        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          ELL_PRIV_COLUMN,
                                          ellGetColumnID(sColumnHandle),
                                          aColumnAction,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Column Privilege 추가
         */
        
        STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

        STL_TRY( qllAddGrantedPriv( aDBCStmt,
                                    aSQLStmt,
                                    ELL_PRIV_COLUMN,
                                    & sPrivHandle,
                                    aEnv )
                 == STL_SUCCESS );

        /**
         * Next Column
         */
        
        sColumnItem = sColumnItem->mNext;
    }    

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        sErrSchemaID = ellGetTableSchemaID(aTableHandle);

        if ( ellGetSchemaDictHandleByID( aTransID,
                                         aSQLStmt->mViewSCN,
                                         sErrSchemaID,
                                         & sErrSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS )
        {
            if ( sObjectExist == STL_TRUE )
            {
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              QLL_ERRCODE_LACKS_PRIVILEGES_ON_TABLE,
                              NULL,
                              QLL_ERROR_STACK(aEnv),
                              gEllColumnPrivActionString[aColumnAction],
                              ellGetSchemaName( & sErrSchemaHandle ),
                              ellGetTableName( aTableHandle ) );
            }
            else
            {
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              QLL_ERRCODE_INSUFFICENT_PRIVILEGES,
                              NULL,
                              QLL_ERROR_STACK(aEnv) );
            }
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_INSUFFICENT_PRIVILEGES,
                          NULL,
                          QLL_ERROR_STACK(aEnv) );
        }
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/*****************************************************************************
 * USAGE ON SEQUENCE
 *****************************************************************************/


/**
 * @brief SEQUENCE 권한 검사
 * @param[in] aTransID    Transaction ID
 * @param[in] aDBCStmt    DBC Statement
 * @param[in] aSQLStmt    SQL Statement
 * @param[in] aSeqHandle  Sequence Handle
 * @param[in] aEnv        Environment
 * @remarks
 */
stlStatus qlaCheckSeqPriv( smlTransId      aTransID,
                           qllDBCStmt    * aDBCStmt,
                           qllStatement  * aSQLStmt,
                           ellDictHandle * aSeqHandle,
                           qllEnv        * aEnv )
{
    ellDictHandle sPrivHandle;

    ellPrivObject sPrivObject = ELL_PRIV_NA;
    stlBool       sPrivExist  = STL_FALSE;

    stlInt64      sErrSchemaID;
    ellDictHandle sErrSchemaHandle;
    stlBool       sObjectExist = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqHandle != NULL, QLL_ERROR_STACK(aEnv) );

    while (1)
    {
        /**
         * USAGE ON SEQUENCE 권한이 있는지 검사
         */

        sPrivObject = ELL_PRIV_USAGE;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetSequenceID(aSeqHandle),
                                          ELL_USAGE_OBJECT_TYPE_SEQUENCE,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /**
         * 권한이 없다면, USAGE ANY SEQUENCE ON DATABASE 권한 검사
         */

        sPrivObject = ELL_PRIV_DATABASE;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetDbCatalogID(),
                                          ELL_DB_PRIV_ACTION_USAGE_ANY_SEQUENCE,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }
        
        /**
         * 권한이 없다면, USAGE SEQUENCE ON SCHEMA 권한 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetSequenceSchemaID(aSeqHandle),
                                          ELL_SCHEMA_PRIV_ACTION_USAGE_SEQUENCE,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /**
         * 권한이 없다면, CONTROL SCHEMA ON SCHEMA 권한 검사
         */

        sPrivObject = ELL_PRIV_SCHEMA;
        STL_TRY( ellGetGrantedPrivHandle( aTransID,
                                          aSQLStmt->mViewSCN,
                                          sPrivObject,
                                          ellGetSequenceSchemaID(aSeqHandle),
                                          ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA,
                                          STL_FALSE, /* aWithGrant */
                                          & sPrivHandle,
                                          & sPrivExist,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        break;
    }

    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    /**
     * Granted Privilege 를 추가
     */

    STL_TRY( qllAddGrantedPriv( aDBCStmt,
                                aSQLStmt,
                                sPrivObject,
                                & sPrivHandle,
                                aEnv )
             == STL_SUCCESS );
                                
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        sErrSchemaID = ellGetSequenceSchemaID(aSeqHandle);

        if ( ellGetSchemaDictHandleByID( aTransID,
                                         aSQLStmt->mViewSCN,
                                         sErrSchemaID,
                                         & sErrSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS )
        {
            if ( sObjectExist == STL_TRUE )
            {
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              QLL_ERRCODE_LACKS_PRIVILEGES_ON_SEQUENCE,
                              NULL,
                              QLL_ERROR_STACK(aEnv),
                              ellGetSchemaName( & sErrSchemaHandle ),
                              ellGetSequenceName( aSeqHandle ) );
            }
            else
            {
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              QLL_ERRCODE_INSUFFICENT_PRIVILEGES,
                              NULL,
                              QLL_ERROR_STACK(aEnv) );
            }
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_INSUFFICENT_PRIVILEGES,
                          NULL,
                          QLL_ERROR_STACK(aEnv) );
        }
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/*****************************************************************************
 * Query Spec 
 *****************************************************************************/

/**
 * @brief Query Spec 의 SELECT 권한을 검사
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aRelationTree  Relation Tree
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckPrivQuerySpec( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qlvInitNode   * aRelationTree,
                                 qllEnv        * aEnv )
{
    /**
     * Parameter Vadlition
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRelationTree != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Relation 유형에 따른 SELECT 권한 검사
     */

    switch( aRelationTree->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
            STL_TRY( qlaCheckPrivBaseTable( aTransID,
                                            aDBCStmt,
                                            aSQLStmt,
                                            (qlvInitBaseTableNode *) aRelationTree,
                                            aEnv )
                     == STL_SUCCESS );
            break;
        case QLV_NODE_TYPE_SUB_TABLE:
            STL_TRY( qlaCheckPrivSubTable( aTransID,
                                           aDBCStmt,
                                           aSQLStmt,
                                           aRelationTree,
                                           aEnv )
                     == STL_SUCCESS );
            break;
        case QLV_NODE_TYPE_JOIN_TABLE:
            STL_TRY( qlaCheckPrivJoinedTable( aTransID,
                                              aDBCStmt,
                                              aSQLStmt,
                                              (qlvInitJoinTableNode *) aRelationTree,
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
 * @brief Query Spec 의 Base Table 의 SELECT privilege check
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aBaseTable     Base Table Node
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckPrivBaseTable( smlTransId               aTransID,
                                 qllDBCStmt             * aDBCStmt,
                                 qllStatement           * aSQLStmt,
                                 qlvInitBaseTableNode   * aBaseTable,
                                 qllEnv                 * aEnv )
{
    return qlaCheckTablePriv( aTransID,
                              aDBCStmt,
                              aSQLStmt,
                              ELL_TABLE_PRIV_ACTION_SELECT,
                              & aBaseTable->mTableHandle,
                              aBaseTable->mRefColumnList,
                              aEnv );
}


/**
 * @brief Query Spec 의 Joined Table 의 SELECT privilege check
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aJoinTable     Joined Table Node
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckPrivJoinedTable( smlTransId               aTransID,
                                   qllDBCStmt             * aDBCStmt,
                                   qllStatement           * aSQLStmt,
                                   qlvInitJoinTableNode   * aJoinTable,
                                   qllEnv                 * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinTable != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Left Table 의 SELECT Privilege 검사
     */

    switch( aJoinTable->mLeftTableNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
            STL_TRY( qlaCheckPrivBaseTable( aTransID,
                                            aDBCStmt,
                                            aSQLStmt,
                                            (qlvInitBaseTableNode *) aJoinTable->mLeftTableNode,
                                            aEnv )
                     == STL_SUCCESS );
            break;
        case QLV_NODE_TYPE_SUB_TABLE:
            STL_TRY( qlaCheckPrivSubTable( aTransID,
                                           aDBCStmt,
                                           aSQLStmt,
                                           aJoinTable->mLeftTableNode,
                                           aEnv )
                     == STL_SUCCESS );
            break;
        case QLV_NODE_TYPE_JOIN_TABLE:
            STL_TRY( qlaCheckPrivJoinedTable( aTransID,
                                              aDBCStmt,
                                              aSQLStmt,
                                              (qlvInitJoinTableNode *) aJoinTable->mLeftTableNode,
                                              aEnv )
                     == STL_SUCCESS );
            break;
        default:
            STL_ASSERT(0);
            break;
    }
    
    /**
     * Right Table 의 SELECT Privilege 검사
     */

    switch( aJoinTable->mRightTableNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
            STL_TRY( qlaCheckPrivBaseTable( aTransID,
                                            aDBCStmt,
                                            aSQLStmt,
                                            (qlvInitBaseTableNode *) aJoinTable->mRightTableNode,
                                            aEnv )
                     == STL_SUCCESS );
            break;
        case QLV_NODE_TYPE_SUB_TABLE:
            STL_TRY( qlaCheckPrivSubTable( aTransID,
                                           aDBCStmt,
                                           aSQLStmt,
                                           aJoinTable->mRightTableNode,
                                           aEnv )
                     == STL_SUCCESS );
            break;
        case QLV_NODE_TYPE_JOIN_TABLE:
            STL_TRY( qlaCheckPrivJoinedTable( aTransID,
                                              aDBCStmt,
                                              aSQLStmt,
                                              (qlvInitJoinTableNode *) aJoinTable->mRightTableNode,
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
 * @brief Query Spec 의 Sub Table 의 SELECT privilege check
 * @param[in] aTransID       Transaction ID
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aSQLStmt       SQL Statement
 * @param[in] aSubTable      Subquery Table Node
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlaCheckPrivSubTable( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qlvInitNode   * aSubTable,
                                qllEnv        * aEnv )
{
    qlvInitSubTableNode * sViewedTable = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubTable != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sViewedTable = (qlvInitSubTableNode *) aSubTable;

    if ( sViewedTable->mIsView == STL_TRUE )
    {
        if ( sViewedTable->mHasHandle == STL_TRUE )
        {
            /**
             * Created View 인 경우
             * - View 의 Column Privilege 는 존재하지 않는다.
             */
            
            STL_TRY( qlaCheckTablePriv( aTransID,
                                        aDBCStmt,
                                        aSQLStmt,
                                        ELL_TABLE_PRIV_ACTION_SELECT,
                                        & sViewedTable->mViewHandle,
                                        NULL, /* aColumnList */
                                        aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * In-line View 의 경우, Query Spec Validation 단계에서 권한 검사됨
             * - nothing to do 
             */
        }
    }
    else
    {
        /**
         * Subquery 등의 경우, Query Spec Validation 단계에서 권한 검사됨
         * - nothing to do 
         */
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlaPrivCheck */
