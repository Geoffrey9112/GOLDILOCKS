/*******************************************************************************
 * eldtSCHEMATA.c
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
 * @file eldtSCHEMATA.c
 * @brief DEFINITION_SCHEMA.SCHEMATA 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtSCHEMATA
 * @{
 */

stlChar * gEldtBuiltInSchemaString[ELDT_SCHEMA_ID_MAX] =
{
    "N/A",                            /* ELDT_SCHEMA_ID_NA */
    
    "DEFINITION_SCHEMA",              /* ELDT_SCHEMA_ID_DEFINITION_SCHEMA */
    "FIXED_TABLE_SCHEMA",             /* ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA */
    "DICTIONARY_SCHEMA",              /* ELDT_SCHEMA_ID_DICTIONARY_SCHEMA */
    "INFORMATION_SCHEMA",             /* ELDT_SCHEMA_ID_INFORMATION_SCHEMA */
    "PERFORMANCE_VIEW_SCHEMA",        /* ELDT_SCHEMA_ID_PERFORMANCE_VIEW_SCHEMA */
    
    "PUBLIC"                          /* ELDT_SCHEMA_ID_PUBLIC */

};

    
/**
 * @brief DEFINITION_SCHEMA.SCHEMATA 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescSCHEMATA[ELDT_Schemata_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_SCHEMATA,                  /**< Table ID */
        "OWNER_ID",                              /**< 컬럼의 이름  */
        ELDT_Schemata_ColumnOrder_OWNER_ID,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the schema"        
    },
    {
        ELDT_TABLE_ID_SCHEMATA,                  /**< Table ID */
        "SCHEMA_ID",                             /**< 컬럼의 이름  */
        ELDT_Schemata_ColumnOrder_SCHEMA_ID,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SERIAL_NUMBER,               /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,               /**< 컬럼의 Nullable 여부 */
        "schema identifier"        
    },
    {
        ELDT_TABLE_ID_SCHEMATA,                  /**< Table ID */
        "SCHEMA_NAME",                           /**< 컬럼의 이름  */
        ELDT_Schemata_ColumnOrder_SCHEMA_NAME,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "schema name"        
    },
    {
        ELDT_TABLE_ID_SCHEMATA,                  /**< Table ID */
        "DEFAULT_CHARACTER_SET_CATALOG",         /**< 컬럼의 이름  */
        ELDT_Schemata_ColumnOrder_DEFAULT_CHARACTER_SET_CATALOG,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_SCHEMATA,                  /**< Table ID */
        "DEFAULT_CHARACTER_SET_SCHEMA",          /**< 컬럼의 이름  */
        ELDT_Schemata_ColumnOrder_DEFAULT_CHARACTER_SET_SCHEMA,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_SCHEMATA,                  /**< Table ID */
        "DEFAULT_CHARACTER_SET_NAME",            /**< 컬럼의 이름  */
        ELDT_Schemata_ColumnOrder_DEFAULT_CHARACTER_SET_NAME,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_SCHEMATA,                  /**< Table ID */
        "SQL_PATH",                              /**< 컬럼의 이름  */
        ELDT_Schemata_ColumnOrder_SQL_PATH,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_SCHEMATA,                  /**< Table ID */
        "IS_BUILTIN",                           /**< 컬럼의 이름  */
        ELDT_Schemata_ColumnOrder_IS_BUILTIN,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "is built-in object or not"        
    },
    {
        ELDT_TABLE_ID_SCHEMATA,                  /**< Table ID */
        "CREATED_TIME",                          /**< 컬럼의 이름  */
        ELDT_Schemata_ColumnOrder_CREATED_TIME,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "created time of the schema"        
    },
    {
        ELDT_TABLE_ID_SCHEMATA,                  /**< Table ID */
        "MODIFIED_TIME",                         /**< 컬럼의 이름  */
        ELDT_Schemata_ColumnOrder_MODIFIED_TIME,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "last modified time of the schema"        
    },
    {
        ELDT_TABLE_ID_SCHEMATA,                 /**< Table ID */
        "COMMENTS",                             /**< 컬럼의 이름  */
        ELDT_Schemata_ColumnOrder_COMMENTS,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "comments of the schema"        
    }
};


/**
 * @brief DEFINITION_SCHEMA.SCHEMATA 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescSCHEMATA =
{
    ELDT_TABLE_ID_SCHEMATA,                  /**< Table ID */
    "SCHEMATA",                              /**< 테이블의 이름  */
    "The SCHEMATA table has one row for each schema"
};


/**
 * @brief DEFINITION_SCHEMA.SCHEMATA 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescSCHEMATA[ELDT_Schemata_Const_MAX] =
{
    {
        ELDT_TABLE_ID_SCHEMATA,                      /**< Table ID */
        ELDT_Schemata_Const_PRIMARY_KEY,             /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Schemata_ColumnOrder_SCHEMA_ID,
        },
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_SCHEMATA,                      /**< Table ID */
        ELDT_Schemata_Const_UNIQUE_SCHEMA_NAME,      /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Schemata_ColumnOrder_SCHEMA_NAME,
        },
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_SCHEMATA,                      /**< Table ID */
        ELDT_Schemata_Const_FOREIGN_KEY_AUTHORIZATIONS,  /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,       /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Schemata_ColumnOrder_OWNER_ID,
        },
        ELDT_TABLE_ID_AUTHORIZATIONS,                /**< 참조 제약이 참조하는 Table ID */
        ELDT_Auth_Const_PRIMARY_KEY,                 /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};


/**
 * @brief DEFINITION_SCHEMA.SCHEMATA 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescSCHEMATA[ELDT_Schemata_Index_MAX] =
{
    {
        ELDT_TABLE_ID_SCHEMATA,                      /**< Table ID */
        ELDT_Schemata_Index_OWNER_ID,                /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Schemata_ColumnOrder_OWNER_ID,
        }
    }
};


/**
 * @brief DEFINITION_SCHEMA 의 built-in schema, DEFINITION_SCHEMA
 */

eldtBuiltInSchemaDesc gEldtBuiltInSchemaDEFINITION_SCHEMA =
{
    ELDT_SCHEMA_ID_DEFINITION_SCHEMA,      /**< Schema ID */
    "The DEFINITION_SCHEMA schema completely describes "
    "all contents of every schema contained in a database."
};

/**
 * @brief DEFINITION_SCHEMA 의 built-in schema, FIXED_TABLE_SCHEMA
 */

eldtBuiltInSchemaDesc gEldtBuiltInSchemaFIXED_TABLE_SCHEMA =
{
    ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA,      /**< Schema ID */
    "The FIXED_TABLE_SCHEMA schema is a special schema identifier for fixed tables, but "
    "no information of fixed tables exists in DEFINITION_SCHEMA schema."
};

/**
 * @brief DEFINITION_SCHEMA 의 built-in schema, DICTIONARY_SCHEMA
 */

eldtBuiltInSchemaDesc gEldtBuiltInSchemaDICTIONARY_SCHEMA =
{
    ELDT_SCHEMA_ID_DICTIONARY_SCHEMA,      /**< Schema ID */
    "The views of the DICTIONARY_SCHEMA schema are viewed tables or base tables "
    "defined in similiar terms of oracle dictionary views."
};

/**
 * @brief DEFINITION_SCHEMA 의 built-in schema, DEFINITION_SCHEMA
 */

eldtBuiltInSchemaDesc gEldtBuiltInSchemaINFORMATION_SCHEMA =
{
    ELDT_SCHEMA_ID_INFORMATION_SCHEMA,      /**< Schema ID */
    "The views of the INFORMATION_SCHEMA schema are viewed tables "
    "defined in terms of the base tables of the DEFINITION_SCHEMA."
};

/**
 * @brief DEFINITION_SCHEMA 의 built-in schema, PERFORMANCE_VIEW_SCHEMA
 */

eldtBuiltInSchemaDesc gEldtBuiltInSchemaPERFORMANCE_VIEW_SCHEMA =
{
    ELDT_SCHEMA_ID_PERFORMANCE_VIEW_SCHEMA,      /**< Schema ID */
    "The views of the PERFORMANCE_VIEW_SCHEMA schema are viewed tables "
    "defined in terms of the fixed tables of the FIXED_TABLE_SCHEMA schema."
};

/**
 * @brief DEFINITION_SCHEMA 의 built-in schema, PUBLIC
 */

eldtBuiltInSchemaDesc gEldtBuiltInSchemaPUBLIC =
{
    ELDT_SCHEMA_ID_PUBLIC,      /**< Schema ID */
    "The PUBLIC schema is an usable schema for all users."
};

/**
 * @brief DEFINITION_SCHEMA 의 built-in schema 들
 */

eldtBuiltInSchemaDesc * gEldtBuiltInSchemaDesc[] =
{
    & gEldtBuiltInSchemaDEFINITION_SCHEMA,
    & gEldtBuiltInSchemaFIXED_TABLE_SCHEMA,
    & gEldtBuiltInSchemaDICTIONARY_SCHEMA,
    & gEldtBuiltInSchemaINFORMATION_SCHEMA,
    & gEldtBuiltInSchemaPERFORMANCE_VIEW_SCHEMA,
    & gEldtBuiltInSchemaPUBLIC,
    NULL
};


/**
 * @brief Built-In Schema Descriptor 를 추가한다.
 * @param[in]  aTransID   Transaction ID
 * @param[in]  aStmt      Statement
 * @param[in]  aEnv       Execution Library Environment
 * @remarks
 */
stlStatus eldtInsertBuiltInSchemaDesc( smlTransId     aTransID,
                                       smlStatement * aStmt,
                                       ellEnv       * aEnv )
{
    stlInt32                 i = 0;
    
    stlInt64                 sSchemaID   = 0;
    eldtBuiltInSchemaDesc  * sSchemaDesc = NULL;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt      != NULL, ELL_ERROR_STACK(aEnv) );
    
    for ( i = 0; gEldtBuiltInSchemaDesc[i] != NULL; i++ )
    {
        sSchemaDesc = gEldtBuiltInSchemaDesc[i];
        sSchemaID   = sSchemaDesc->mSchemaID;

        STL_TRY( ellInsertSchemaDesc( aTransID,
                                      aStmt,
                                      ELDT_AUTH_ID_SYSTEM,            /* Owner ID */
                                      & sSchemaID,                    /* Schema ID */
                                      gEldtBuiltInSchemaString[sSchemaID], /* Schema Name */
                                      sSchemaDesc->mSchemaComment,    /* Schema Comment */
                                      aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} eldtSCHEMATA */

