/*******************************************************************************
 * eldtAUTHORIZATIONS.c
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
 * @file eldtAUTHORIZATIONS.c
 * @brief DEFINITION_SCHEMA.AUTHORIZATIONS 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtAUTHORIZATIONS
 * @{
 */

stlChar * gEldtAuthString[ELDT_AUTH_ID_MAX] =
{
    "N/A",       /* ELDT_AUTH_ID_NOT_AVAILABLE */
    
    "_SYSTEM",   /* ELDT_AUTH_ID_SYSTEM */
    "SYS",       /* ELDT_AUTH_ID_SYS */
    "ADMIN",     /* ELDT_AUTH_ID_ADMIN */
    "SYSDBA",    /* ELDT_AUTH_ID_SYSDBA */
    "PUBLIC",    /* ELDT_AUTH_ID_PUBLIC */

    "TEST",      /* ELDT_AUTH_ID_TEST */
};
    
/**
 * @brief DEFINITION_SCHEMA.AUTHORIZATIONS 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescAUTHORIZATIONS[ELDT_Auth_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_AUTHORIZATIONS,           /**< Table ID */
        "AUTH_ID",                              /**< 컬럼의 이름  */
        ELDT_Auth_ColumnOrder_AUTH_ID,          /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SERIAL_NUMBER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "authorization identifier"        
    },
    {
        ELDT_TABLE_ID_AUTHORIZATIONS,           /**< Table ID */
        "AUTHORIZATION_NAME",                   /**< 컬럼의 이름  */
        ELDT_Auth_ColumnOrder_AUTHORIZATION_NAME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "authorization name( user identifier or role name )"        
    },
    {
        ELDT_TABLE_ID_AUTHORIZATIONS,           /**< Table ID */
        "AUTHORIZATION_TYPE",                   /**< 컬럼의 이름  */
        ELDT_Auth_ColumnOrder_AUTHORIZATION_TYPE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "authorization type (USER, ROLE, or special identifier(SYSTEM, PUBLIC) )"        
    },
    {
        ELDT_TABLE_ID_AUTHORIZATIONS,           /**< Table ID */
        "AUTHORIZATION_TYPE_ID",                /**< 컬럼의 이름  */
        ELDT_Auth_ColumnOrder_AUTHORIZATION_TYPE_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,            /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "AUTHORIZATION_TYPE identifier"        
    },
    {
        ELDT_TABLE_ID_AUTHORIZATIONS,           /**< Table ID */
        "IS_BUILTIN",                           /**< 컬럼의 이름  */
        ELDT_Auth_ColumnOrder_IS_BUILTIN, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "is built-in object or not"        
    },
    
    {
        ELDT_TABLE_ID_AUTHORIZATIONS,           /**< Table ID */
        "CREATED_TIME",                         /**< 컬럼의 이름  */
        ELDT_Auth_ColumnOrder_CREATED_TIME,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "created time of the authorization"        
    },
    {
        ELDT_TABLE_ID_AUTHORIZATIONS,           /**< Table ID */
        "MODIFIED_TIME",                        /**< 컬럼의 이름  */
        ELDT_Auth_ColumnOrder_MODIFIED_TIME,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "last modified time of the authorization"        
    },
    {
        ELDT_TABLE_ID_AUTHORIZATIONS,           /**< Table ID */
        "COMMENTS",                             /**< 컬럼의 이름  */
        ELDT_Auth_ColumnOrder_COMMENTS,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "comments of the authorization"        
    }
};


/**
 * @brief DEFINITION_SCHEMA.AUTHORIZATIONS 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescAUTHORIZATIONS =
{
    ELDT_TABLE_ID_AUTHORIZATIONS,                  /**< Table ID */
    "AUTHORIZATIONS",                              /**< 테이블의 이름  */
    "The AUTHORIZATIONS table has one row for each role name "
    "and one row for each user identifier."
};


/**
 * @brief DEFINITION_SCHEMA.AUTHORIZATIONS 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescAUTHORIZATIONS[ELDT_Auth_Const_MAX] =
{
    {
        ELDT_TABLE_ID_AUTHORIZATIONS,                /**< Table ID */
        ELDT_Auth_Const_PRIMARY_KEY,                 /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Auth_ColumnOrder_AUTH_ID,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_AUTHORIZATIONS,                /**< Table ID */
        ELDT_Auth_Const_UNIQUE_AUTHORIZATION_NAME,   /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Auth_ColumnOrder_AUTHORIZATION_NAME,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};




/**
 * @brief DEFINITION_SCHEMA 의 built-in authorization, _SYSTEM
 */
eldtBuiltInAuthDesc gEldtBuiltInAuth_SYSTEM = 
{
    ELDT_AUTH_ID_SYSTEM,                        /**< Authorization ID */
    ELL_AUTHORIZATION_TYPE_SYSTEM,              /**< Authorization 유형  */
    "The _SYSTEM is a special identifier for internal use."  /**< Authorization 의 주석 */
};

/**
 * @brief DEFINITION_SCHEMA 의 built-in authorization, SYS
 */
eldtBuiltInAuthDesc gEldtBuiltInAuthSYS = 
{
    ELDT_AUTH_ID_SYS,                        /**< Authorization ID */
    ELL_AUTHORIZATION_TYPE_USER,             /**< Authorization 유형  */
    "The SYS user can perform all administrative functions."  /**< Authorization 의 주석 */
};

/**
 * @brief DEFINITION_SCHEMA 의 built-in authorization, ADMIN
 */
eldtBuiltInAuthDesc gEldtBuiltInAuthADMIN = 
{
    ELDT_AUTH_ID_ADMIN,                        /**< Authorization ID */
    ELL_AUTHORIZATION_TYPE_ROLE,             /**< Authorization 유형  */
    "The ADMIN role can perform all administrative functions."  /**< Authorization 의 주석 */
};


/**
 * @brief DEFINITION_SCHEMA 의 built-in authorization, SYSDBA
 */
eldtBuiltInAuthDesc gEldtBuiltInAuthSYSDBA = 
{
    ELDT_AUTH_ID_SYSDBA,                        /**< Authorization ID */
    ELL_AUTHORIZATION_TYPE_ROLE,                /**< Authorization 유형  */
    "The SYSDBA role can perform some database administrative functions."  /**< Authorization 의 주석 */
};


/**
 * @brief DEFINITION_SCHEMA 의 built-in authorization, PUBLIC
 */
eldtBuiltInAuthDesc gEldtBuiltInAuthPUBLIC = 
{
    ELDT_AUTH_ID_PUBLIC,                     /**< Authorization ID */
    ELL_AUTHORIZATION_TYPE_ALL_USER,         /**< Authorization 유형  */
    "The PUBLIC is a special identifier that indicates all users and roles."  /**< Authorization 의 주석 */
};

/**
 * @brief DEFINITION_SCHEMA 의 built-in authorization, TEST
 */
eldtBuiltInAuthDesc gEldtBuiltInAuthTEST = 
{
    ELDT_AUTH_ID_TEST,                      /**< Authorization ID */
    ELL_AUTHORIZATION_TYPE_USER,            /**< Authorization 유형  */
    "The TEST user is a built-in test user."  /**< Authorization 의 주석 */
};

/**
 * @brief DEFINITION_SCHEMA 의 built-in Authorization 들
 */

eldtBuiltInAuthDesc * gEldtBuiltInAuthDesc[] =
{
    & gEldtBuiltInAuth_SYSTEM,
    & gEldtBuiltInAuthSYS,
    & gEldtBuiltInAuthADMIN,
    & gEldtBuiltInAuthSYSDBA,
    & gEldtBuiltInAuthPUBLIC,
    & gEldtBuiltInAuthTEST,
    NULL
};


/**
 * @brief Built-In Authorization Descriptor 를 추가한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aSysPassword      SYS 사용자의 encrypted password
 * @param[in]  aSystemTbsID      System Tablespace ID
 * @param[in]  aUserTablespaceID Default User Tablespace ID
 * @param[in]  aTempTablespaceID Default Temp Tablespace ID
 * @param[in]  aEnv              Execution Library Environment
 * @remarks
 */
stlStatus eldtInsertBuiltInAuthDesc( smlTransId     aTransID,
                                     smlStatement * aStmt,
                                     stlChar      * aSysPassword,
                                     smlTbsId       aSystemTbsID,
                                     smlTbsId       aUserTablespaceID,
                                     smlTbsId       aTempTablespaceID,
                                     ellEnv       * aEnv )
{
    stlInt32               i = 0;
    
    stlInt64               sAuthID   = 0;
    eldtBuiltInAuthDesc  * sAuthDesc = NULL;

    stlChar                sSysPassword[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};

    eldtBuiltInSchemaID    sSchemaPathSYS[]
        = { ELDT_SCHEMA_ID_PUBLIC,
            ELDT_SCHEMA_ID_DICTIONARY_SCHEMA,
            ELDT_SCHEMA_ID_INFORMATION_SCHEMA,
            ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
            ELDT_SCHEMA_ID_PERFORMANCE_VIEW_SCHEMA,
            ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA,
            ELDT_SCHEMA_ID_NA };
    
    eldtBuiltInSchemaID    sSchemaPathPUBLIC[]
        = { ELDT_SCHEMA_ID_DICTIONARY_SCHEMA,
            ELDT_SCHEMA_ID_INFORMATION_SCHEMA,
            ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
            ELDT_SCHEMA_ID_PERFORMANCE_VIEW_SCHEMA,  
            ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA,
            ELDT_SCHEMA_ID_NA };

    eldtBuiltInSchemaID    sSchemaPathTEST[]
        = { ELDT_SCHEMA_ID_PUBLIC,
            ELDT_SCHEMA_ID_NA };
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Password 정보 생성
     */
    
    if ( aSysPassword == NULL )
    {
        STL_TRY( stlMakeEncryptedPassword( ELDT_DEFAULT_SYS_PASSWORD,
                                           stlStrlen( ELDT_DEFAULT_SYS_PASSWORD ),
                                           sSysPassword,
                                           STL_MAX_SQL_IDENTIFIER_LENGTH,
                                           ELL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        stlStrcpy( sSysPassword, aSysPassword );
    }
    

    /***************************************************************
     * built-in authorization identifier 를 추가함
     ***************************************************************/
    
    for ( i = 0; gEldtBuiltInAuthDesc[i] != NULL; i++ )
    {
        sAuthDesc = gEldtBuiltInAuthDesc[i];
        
        STL_TRY( ellInsertAuthDesc( aTransID,
                                    aStmt,
                                    & sAuthID,                    /* Auth ID */
                                    gEldtAuthString[sAuthDesc->mAuthID],   /* Auth Name */
                                    sAuthDesc->mAuthType,         /* Auth Type */
                                    sAuthDesc->mAuthComment,      /* Auth Comment */
                                    aEnv )
                 == STL_SUCCESS );

        /*
         * Enumeration 정의 순서와 DATABASE 정보가 일치하는 지 검증 
         */ 
        STL_ASSERT( sAuthID == sAuthDesc->mAuthID );
    }

    /**
     * 다음 built-in authorization identifer 에 대한 descriptor 는 추가하지 않는다.
     * - built-in authorization PUBLIC 은 Role 이 아닌 전체를 의미하는 spectial identifier이므로
     *   Role authorization descriptor 를 추가하지 않는다.
     * - built-in authorization _SYSTEM 은 User 가 아닌 내부 identifier 를 의미하는
     *   special identifier이므로 user descriptor 를 추가하지 않는다.
     */

    /***************************************************************
     * _SYSTEM special identifier 
     ***************************************************************/

    /**
     * _SYSTEM 은 USER 가 아님
     */
    
    /*****************************************
     * privileges for _SYSTEM
     *****************************************/
    
    /**
     * 모든 database privilege WITH GRANT OPTION
     */

    for ( i = ELL_DB_PRIV_ACTION_NA + 1; i < ELL_DB_PRIV_ACTION_MAX; i++ )
    {
        STL_TRY( ellInsertDatabasePrivDesc( aTransID,
                                            aStmt,
                                            ELDT_AUTH_ID_SYSTEM, /* Grantor */
                                            ELDT_AUTH_ID_SYSTEM, /* Grantee */
                                            i,                   /* Priv Action */
                                            STL_TRUE,            /* Grantable */
                                            aEnv )
                 == STL_SUCCESS );
    }

    /*****************************************
     * schema path for _SYSTEM
     *****************************************/
    
    /**
     * user 가 아니므로 schema path 가 없음
     */

    
    /***************************************************************
     * SYS user
     ***************************************************************/

    /**
     * SYS 사용자에 대한 user descriptor 를 추가함
     */

    STL_TRY( ellInsertUserDesc( aTransID,
                                aStmt,
                                ELDT_AUTH_ID_SYS,
                                ELL_DICT_OBJECT_ID_NA, /* aProfileID */
                                sSysPassword,
                                aUserTablespaceID,
                                aTempTablespaceID,
                                ELL_USER_LOCKED_STATUS_OPEN,
                                ELL_DICT_STLTIME_NA,   /* aLockedTime */
                                ELL_USER_EXPIRY_STATUS_OPEN,
                                ELL_DICT_STLTIME_NA,   /* aExpiryTime */
                                aEnv )
             == STL_SUCCESS );

    
    /*****************************************
     * privileges for SYS
     *****************************************/
    
    /**
     * SYS 사용자의 Database Privilege 를 추가한다.
     * SYSDBA 권한외의 모든 Database Privilege 를 소유한다.
     */

    for ( i = ELL_DB_PRIV_ACTION_NA + 1; i < ELL_DB_PRIV_ACTION_MAX; i++ )
    {
        if ( i == ELL_DB_PRIV_ACTION_ADMINISTRATION )
        {
            /**
             * ADMINISTRATION 권한은 소유하지 않는다.
             */
        }
        else
        {
            STL_TRY( ellInsertDatabasePrivDesc( aTransID,
                                                aStmt,
                                                ELDT_AUTH_ID_SYSTEM, /* Grantor */
                                                ELDT_AUTH_ID_SYS,    /* Grantee */
                                                i,                   /* Priv Action */
                                                STL_TRUE,            /* Grantable */
                                                aEnv )
                     == STL_SUCCESS );
        }
    }

    /**
     * SYS 사용자의 SELECT ON SCHEMA DEFINITION_SCHEMA 권한을 추가
     */

    STL_TRY( ellInsertSchemaPrivDesc( aTransID,
                                      aStmt,
                                      ELDT_AUTH_ID_SYSTEM,  /* Grantor */
                                      ELDT_AUTH_ID_SYS,     /* Grantee */
                                      ELDT_SCHEMA_ID_DEFINITION_SCHEMA,   /* aSchemaID */
                                      ELL_SCHEMA_PRIV_ACTION_SELECT_TABLE,  /* SELECT Priv */
                                      STL_TRUE,             /* aIsGrantable */
                                      aEnv )
             == STL_SUCCESS );

    /**
     * SYS 사용자의 SELECT ON SCHEMA FIXED_TABLE_SCHEMA 권한을 추가
     */

    STL_TRY( ellInsertSchemaPrivDesc( aTransID,
                                      aStmt,
                                      ELDT_AUTH_ID_SYSTEM,  /* Grantor */
                                      ELDT_AUTH_ID_SYS,     /* Grantee */
                                      ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA,  /* aSchemaID */
                                      ELL_SCHEMA_PRIV_ACTION_SELECT_TABLE,  /* SELECT Priv */
                                      STL_TRUE,             /* aIsGrantable */
                                      aEnv )
             == STL_SUCCESS );

    /**
     * SYS 사용자의 SELECT ON SCHEMA DICTIONARY_SCHEMA 권한을 추가
     */

    STL_TRY( ellInsertSchemaPrivDesc( aTransID,
                                      aStmt,
                                      ELDT_AUTH_ID_SYSTEM,                           /* Grantor */
                                      ELDT_AUTH_ID_SYS,                              /* Grantee */
                                      ELDT_SCHEMA_ID_DICTIONARY_SCHEMA,              /* aSchemaID */
                                      ELL_SCHEMA_PRIV_ACTION_SELECT_TABLE,           /* SELECT Priv */
                                      STL_TRUE,                                      /* aIsGrantable */
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * SYS 사용자의 SELECT ON SCHEMA INFORMATION_SCHEMA 권한을 추가
     */

    STL_TRY( ellInsertSchemaPrivDesc( aTransID,
                                      aStmt,
                                      ELDT_AUTH_ID_SYSTEM,  /* Grantor */
                                      ELDT_AUTH_ID_SYS,     /* Grantee */
                                      ELDT_SCHEMA_ID_INFORMATION_SCHEMA,  /* aSchemaID */
                                      ELL_SCHEMA_PRIV_ACTION_SELECT_TABLE,  /* SELECT Priv */
                                      STL_TRUE,             /* aIsGrantable */
                                      aEnv )
             == STL_SUCCESS );

    /**
     * SYS 사용자의 SELECT ON SCHEMA PERFORMANCE_VIEW_SCHEMA 권한을 추가
     */

    STL_TRY( ellInsertSchemaPrivDesc( aTransID,
                                      aStmt,
                                      ELDT_AUTH_ID_SYSTEM,                     /* Grantor */
                                      ELDT_AUTH_ID_SYS,                        /* Grantee */
                                      ELDT_SCHEMA_ID_PERFORMANCE_VIEW_SCHEMA,  /* aSchemaID */
                                      ELL_SCHEMA_PRIV_ACTION_SELECT_TABLE,       /* SELECT Priv */
                                      STL_TRUE,                                /* aIsGrantable */
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * SYS 사용자의 SELECT ON SCHEMA PUBLIC 권한을 추가
     */

    STL_TRY( ellInsertSchemaPrivDesc( aTransID,
                                      aStmt,
                                      ELDT_AUTH_ID_SYSTEM,                /* Grantor */
                                      ELDT_AUTH_ID_SYS,                   /* Grantee */
                                      ELDT_SCHEMA_ID_PUBLIC,              /* aSchemaID */
                                      ELL_SCHEMA_PRIV_ACTION_SELECT_TABLE,  /* SELECT Priv */
                                      STL_TRUE,                           /* aIsGrantable */
                                      aEnv )
             == STL_SUCCESS );
    
    
    /*****************************************
     * schema path for SYS
     *****************************************/
    
    /**
     * SYS 사용자를 위한 schema path descriptor를 추가
     */

    i = 0;
    while( sSchemaPathSYS[i] != ELDT_SCHEMA_ID_NA )
    {
        STL_TRY( ellInsertUserSchemaPathDesc( aTransID,
                                              aStmt,
                                              ELDT_AUTH_ID_SYS, /* SYS 사용자 */
                                              sSchemaPathSYS[i],
                                              i,
                                              aEnv )
                 == STL_SUCCESS );
        i++;
    }
    
    /***************************************************************
     * ADMIN role
     ***************************************************************/

    /**
     * ADMIN role 에 대한 role descriptor 를 추가함
     */

    STL_TRY( ellInsertRoleAuthDescForCreateRole( aTransID,
                                                 aStmt,
                                                 ELDT_AUTH_ID_ADMIN,     /* role id */
                                                 ELDT_AUTH_ID_SYSTEM,    /* creator */
                                                 STL_TRUE,               /* grantable */
                                                 aEnv )
             == STL_SUCCESS );

    /*****************************************
     * privileges for ADMIN
     *****************************************/
    
    /**
     * ADMIN 사용자의 Database Privilege 를 추가한다.(w/o GRANT OPTION)
     */

    for ( i = ELL_DB_PRIV_ACTION_NA + 1; i < ELL_DB_PRIV_ACTION_MAX; i++ )
    {
        STL_TRY( ellInsertDatabasePrivDesc( aTransID,
                                            aStmt,
                                            ELDT_AUTH_ID_SYSTEM,   /* Grantor */
                                            ELDT_AUTH_ID_ADMIN,    /* Grantee */
                                            i,                     /* Priv Action */
                                            STL_FALSE,             /* Grantable */
                                            aEnv )
                 == STL_SUCCESS );
    }

    /*****************************************
     * schema path for ADMIN
     *****************************************/
    
    /**
     * user 가 아니므로 schema path 가 없음
     */
    
    /***************************************************************
     * SYSDBA role
     ***************************************************************/

    /**
     * SYSDBA role 에 대한 role descriptor 를 추가함
     */

    STL_TRY( ellInsertRoleAuthDescForCreateRole( aTransID,
                                                 aStmt,
                                                 ELDT_AUTH_ID_SYSDBA,    /* role id */
                                                 ELDT_AUTH_ID_SYSTEM,    /* creator */
                                                 STL_TRUE,               /* grantable */
                                                 aEnv )
             == STL_SUCCESS );

    /*****************************************
     * privileges for SYSDBA
     *****************************************/
    
    /**
     * SYSDBA role 의 Database Privilege 를 추가한다.
     * - ADMINISTRATION 권한만을 갖는다. (w/o GRANT OPTION)
     */

    STL_TRY( ellInsertDatabasePrivDesc( aTransID,
                                        aStmt,
                                        ELDT_AUTH_ID_SYSTEM,     /* Grantor */
                                        ELDT_AUTH_ID_SYSDBA,     /* Grantee */
                                        ELL_DB_PRIV_ACTION_ADMINISTRATION, /* Priv Action */
                                        STL_FALSE,               /* Grantable */
                                        aEnv )
             == STL_SUCCESS );

    /*****************************************
     * schema path for SYSDBA
     *****************************************/
    
    /**
     * user 가 아니므로 schema path 가 없음
     */
    
    /***************************************************************
     * PUBLIC special identifier
     ***************************************************************/

    /**
     * User 도 Role 도 아닌 모든 사용자를 의미하는 특이한 authorization identifier 임
     */

    /*****************************************
     * privileges for PUBLIC
     *****************************************/
    
    /**
     * PUBLIC 사용자의 CREATE TABLE ON SCHEMA PUBLIC 권한을 추가
     * w/o GRANT OPTION
     */

    STL_TRY( ellInsertSchemaPrivDesc( aTransID,
                                      aStmt,
                                      ELDT_AUTH_ID_SYSTEM,                /* Grantor */
                                      ELDT_AUTH_ID_PUBLIC,                /* Grantee */
                                      ELDT_SCHEMA_ID_PUBLIC,              /* aSchemaID */
                                      ELL_SCHEMA_PRIV_ACTION_CREATE_TABLE,  /* CREATE TABLE Priv */
                                      STL_FALSE,                          /* aIsGrantable */
                                      aEnv )
             == STL_SUCCESS );

    /**
     * PUBLIC 사용자의 CREATE VIEW ON SCHEMA PUBLIC 권한을 추가
     * w/o GRANT OPTION
     */

    STL_TRY( ellInsertSchemaPrivDesc( aTransID,
                                      aStmt,
                                      ELDT_AUTH_ID_SYSTEM,                /* Grantor */
                                      ELDT_AUTH_ID_PUBLIC,                /* Grantee */
                                      ELDT_SCHEMA_ID_PUBLIC,              /* aSchemaID */
                                      ELL_SCHEMA_PRIV_ACTION_CREATE_VIEW,   /* CREATE VIEW Priv */
                                      STL_FALSE,                           /* aIsGrantable */
                                      aEnv )
             == STL_SUCCESS );

    /**
     * PUBLIC 사용자의 CREATE SEQUENCE ON SCHEMA PUBLIC 권한을 추가
     * w/o GRANT OPTION
     */

    STL_TRY( ellInsertSchemaPrivDesc( aTransID,
                                      aStmt,
                                      ELDT_AUTH_ID_SYSTEM,                /* Grantor */
                                      ELDT_AUTH_ID_PUBLIC,                /* Grantee */
                                      ELDT_SCHEMA_ID_PUBLIC,              /* aSchemaID */
                                      ELL_SCHEMA_PRIV_ACTION_CREATE_SEQUENCE,   /* CREATE SEQUENCE Priv */
                                      STL_FALSE,                           /* aIsGrantable */
                                      aEnv )
             == STL_SUCCESS );

    /**
     * PUBLIC 사용자의 CREATE INDEX ON SCHEMA PUBLIC 권한을 추가
     * w/o GRANT OPTION
     */

    STL_TRY( ellInsertSchemaPrivDesc( aTransID,
                                      aStmt,
                                      ELDT_AUTH_ID_SYSTEM,                /* Grantor */
                                      ELDT_AUTH_ID_PUBLIC,                /* Grantee */
                                      ELDT_SCHEMA_ID_PUBLIC,              /* aSchemaID */
                                      ELL_SCHEMA_PRIV_ACTION_CREATE_INDEX,   /* CREATE INDEX Priv */
                                      STL_FALSE,                           /* aIsGrantable */
                                      aEnv )
             == STL_SUCCESS );

    /**
     * PUBLIC 사용자의 ADD CONSTRAINT ON SCHEMA PUBLIC 권한을 추가
     * w/o GRANT OPTION
     */

    STL_TRY( ellInsertSchemaPrivDesc( aTransID,
                                      aStmt,
                                      ELDT_AUTH_ID_SYSTEM,                    /* Grantor */
                                      ELDT_AUTH_ID_PUBLIC,                    /* Grantee */
                                      ELDT_SCHEMA_ID_PUBLIC,                  /* aSchemaID */
                                      ELL_SCHEMA_PRIV_ACTION_ADD_CONSTRAINT,  /* ADD CONSTRAINT Priv */
                                      STL_FALSE,                              /* aIsGrantable */
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * PUBLIC 사용자의 SELECT ON SCHEMA DEFINITION_SCHEMA 권한을 추가
     * w/o GRANT OPTION
     */

    STL_TRY( ellInsertSchemaPrivDesc( aTransID,
                                      aStmt,
                                      ELDT_AUTH_ID_SYSTEM,  /* Grantor */
                                      ELDT_AUTH_ID_PUBLIC,  /* Grantee */
                                      ELDT_SCHEMA_ID_DEFINITION_SCHEMA,  /* aSchemaID */
                                      ELL_SCHEMA_PRIV_ACTION_SELECT_TABLE,  /* SELECT Priv */
                                      STL_FALSE,             /* aIsGrantable */
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * PUBLIC 사용자의 SELECT ON SCHEMA FIXED_TABLE_SCHEMA 권한을 추가
     * w/o GRANT OPTION
     */

    STL_TRY( ellInsertSchemaPrivDesc( aTransID,
                                      aStmt,
                                      ELDT_AUTH_ID_SYSTEM,  /* Grantor */
                                      ELDT_AUTH_ID_PUBLIC,  /* Grantee */
                                      ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA,  /* aSchemaID */
                                      ELL_SCHEMA_PRIV_ACTION_SELECT_TABLE,  /* SELECT Priv */
                                      STL_FALSE,             /* aIsGrantable */
                                      aEnv )
             == STL_SUCCESS );
    
    /*****************************************
     * schema path for PUBLIC
     *****************************************/
    
    /**
     * PUBLIC 을 위한 public schema path descriptor를 추가
     */

    i = 0;
    while( sSchemaPathPUBLIC[i] != ELDT_SCHEMA_ID_NA )
    {
        STL_TRY( ellInsertUserSchemaPathDesc( aTransID,
                                              aStmt,
                                              ELDT_AUTH_ID_PUBLIC, 
                                              sSchemaPathPUBLIC[i],
                                              i,
                                              aEnv )
                 == STL_SUCCESS );
        i++;
    }
    
    
    /***************************************************************
     * TEST user
     ***************************************************************/
    
    /**
     * TEST 사용자를 위한 user descriptor 를 추가
     */

    STL_TRY( stlMakeEncryptedPassword( "test",
                                       stlStrlen( "test" ),
                                       sSysPassword,
                                       STL_MAX_SQL_IDENTIFIER_LENGTH,
                                       ELL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellInsertUserDesc( aTransID,
                                aStmt,
                                ELDT_AUTH_ID_TEST,
                                ELL_DICT_OBJECT_ID_NA, /* aProfileID */
                                sSysPassword,
                                aUserTablespaceID,
                                aTempTablespaceID,
                                ELL_USER_LOCKED_STATUS_OPEN,
                                ELL_DICT_STLTIME_NA,   /* aLockedTime */
                                ELL_USER_EXPIRY_STATUS_OPEN,
                                ELL_DICT_STLTIME_NA,   /* aExpiryTime */
                                aEnv )
             == STL_SUCCESS );

    /*****************************************
     * privileges for TEST
     *****************************************/
    
    /**
     * TEST user 의 Database Privilege 를 추가한다.
     * - CREATE SESSION (w/o GRANT OPTION)
     */

    STL_TRY( ellInsertDatabasePrivDesc( aTransID,
                                        aStmt,
                                        ELDT_AUTH_ID_SYSTEM,     /* Grantor */
                                        ELDT_AUTH_ID_TEST,       /* Grantee */
                                        ELL_DB_PRIV_ACTION_CREATE_SESSION, /* Priv Action */
                                        STL_FALSE,               /* Grantable */
                                        aEnv )
             == STL_SUCCESS );

    /**
     * TEST user 의 Tablespace Privilege 를 추가한다.
     * - CREATE OBJECT ON TABLESPACE mem_data_tbs (w/o GRANT OPTION)
     */

    STL_TRY( ellInsertTablespacePrivDesc( aTransID,
                                          aStmt,
                                          ELDT_AUTH_ID_SYSTEM,     /* Grantor */
                                          ELDT_AUTH_ID_TEST,       /* Grantee */
                                          SML_MEMORY_DATA_SYSTEM_TBS_ID,   /* Space ID */
                                          ELL_SPACE_PRIV_ACTION_CREATE_OBJECT, /* Priv Action */
                                          STL_FALSE,               /* Grantable */
                                          aEnv )
             == STL_SUCCESS );
    
    /**
     * TEST user 의 Tablespace Privilege 를 추가한다.
     * - CREATE OBJECT ON TABLESPACE mem_temp_tbs (w/o GRANT OPTION)
     */

    STL_TRY( ellInsertTablespacePrivDesc( aTransID,
                                          aStmt,
                                          ELDT_AUTH_ID_SYSTEM,     /* Grantor */
                                          ELDT_AUTH_ID_TEST,       /* Grantee */
                                          SML_MEMORY_TEMP_SYSTEM_TBS_ID,   /* Space ID */
                                          ELL_SPACE_PRIV_ACTION_CREATE_OBJECT, /* Priv Action */
                                          STL_FALSE,               /* Grantable */
                                          aEnv )
             == STL_SUCCESS );
    
    /*****************************************
     * schema path for TEST
     *****************************************/
    
    /**
     * TEST 사용자를 위한 schema path descriptor를 추가
     */

    i = 0;
    while( sSchemaPathTEST[i] != ELDT_SCHEMA_ID_NA )
    {
        STL_TRY( ellInsertUserSchemaPathDesc( aTransID,
                                              aStmt,
                                              ELDT_AUTH_ID_TEST, /* TEST 사용자 */
                                              sSchemaPathTEST[i],
                                              i,
                                              aEnv )
                 == STL_SUCCESS );
        i++;
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} eldtAUTHORIZATIONS */

