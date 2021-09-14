/*******************************************************************************
 * eldtDEFINITION_SCHEMA.c
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
 * @file eldtDEFINITION_SCHEMA.c
 * @brief Database Dictionary 관리 모듈
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>
#include <eldCatalog.h>

/**
 * @addtogroup eldtSCHEMA
 * @{
 */


/**
 * @brief Dictionary 구축 과정 중 사용할 Dictionary Table 들의 Indentity Value
 */
stlInt64 gEldtIdentityValue[ELDT_TABLE_ID_MAX] = { 0, };

/**
 * @brief Dictionary 구축 과정 중 생성된 Index 의 개수
 */
stlInt32 gEldtIndexCnt = 0;

/**
 * @brief Dictionary 구축 과정 중 사용할 Index 들의 물리적 정보 
 */
eldtDefinitionPhysicalIndexInfo gEldtIndexInfo[ELDT_MAX_INDEX_COUNT];

/**
 * @brief Dictionary 에 정의된 Not Null Constraint 의 최대 CONSTRAINT ID
 * DEFINITION SCHEMA 의 제약조건 구축은 NOT NULL => KEY 제약 조건의 순으로 구축된다.
 */
stlInt64 gEldtNotNullMaxConstID = 0;

/**
 * @brief DEFINITION_SCHEMA의 Domain Descriptor 정보
 */
eldtDefinitionDomainDesc   gEldtDomainDesc[ELDT_DOMAIN_MAX] =
{
    {
        /* N/A */
        ELDT_DOMAIN_NA,
        DTL_TYPE_NA,
        "N/A",                    
        STL_FALSE,    /* identity 여부 */
        DTL_PRECISION_NA,
        DTL_PRECISION_NA
    },
    {
        /* SQL 표준, Object Name, VARCHAR(128) */
        ELDT_DOMAIN_SQL_IDENTIFIER,
        DTL_TYPE_VARCHAR,
        "CHARACTER VARYING",                    
        STL_FALSE,
        ELDT_DOMAIN_PRECISION_SQL_IDENTIFIER,
        DTL_PRECISION_NA
    },
    {
        /* SQL 표준, Short Description, VARCHAR(32) */
        ELDT_DOMAIN_SHORT_DESC,
        DTL_TYPE_VARCHAR,
        "CHARACTER VARYING",                    
        STL_FALSE,
        ELDT_DOMAIN_PRECISION_SHORT_DESC,
        DTL_PRECISION_NA
    },
    {
        /* 비표준, Long Description, VARCHAR(1024) */
        ELDT_DOMAIN_LONG_DESC,
        DTL_TYPE_VARCHAR,
        "CHARACTER VARYING",                    
        STL_FALSE,
        ELDT_DOMAIN_PRECISION_LONG_DESC,
        DTL_PRECISION_NA
    },
    {
        /* 비표준, Full Text, TEXT */
        ELDT_DOMAIN_SQL_SYNTAX,
        DTL_TYPE_LONGVARCHAR,
        "CHARACTER LONG VARYING",                    
        STL_FALSE,
        ELDT_DOMAIN_PRECISION_SQL_SYNTAX,
        DTL_PRECISION_NA
    },
    {
        /* SQL 표준, NATIVE_INTEGER */
        ELDT_DOMAIN_CARDINAL_NUMBER,
        DTL_TYPE_NATIVE_INTEGER,
        "NATIVE_INTEGER",
        STL_FALSE,
        DTL_LENGTH_NA,
        DTL_PRECISION_NA
    },
    {
        /* 비표준, NATIVE_BIGINT  */
        ELDT_DOMAIN_BIG_NUMBER,
        DTL_TYPE_NATIVE_BIGINT,
        "NATIVE_BIGINT",
        STL_FALSE,
        DTL_LENGTH_NA,
        DTL_PRECISION_NA
    },
    {
        /* 비표준, Object serial number, NATIVE_BIGINT SEQUENCE */
        ELDT_DOMAIN_SERIAL_NUMBER,
        DTL_TYPE_NATIVE_BIGINT,
        "NATIVE_BIGINT",                    
        STL_TRUE,
        DTL_LENGTH_NA,
        DTL_PRECISION_NA
    },
    {
        /* SQL 표준, BOOLEAN, 실제 표준은 'YES', 'NO' 값임 */
        ELDT_DOMAIN_YES_OR_NO,
        DTL_TYPE_BOOLEAN,
        "BOOLEAN", 
        STL_FALSE,
        DTL_LENGTH_NA,
        DTL_PRECISION_NA
    },
    {
        /* SQL 표준, TIMESTAMP(2) */
        ELDT_DOMAIN_TIME_STAMP ,
        DTL_TYPE_TIMESTAMP,
        "TIMESTAMP",
        STL_FALSE,
        DTL_LENGTH_NA,
        ELDT_DOMAIN_PRECISION_TIME_STAMP
    },
    {
        /* 비표준, INTERVAL HOUR TO MINUTE */
        ELDT_DOMAIN_INTERVAL,
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,
        "INTERVAL HOUR TO MINUTE",
        STL_FALSE,
        DTL_LENGTH_NA,
        ELDT_DOMAIN_INTERVAL_START_PRECISION
    },
    {
        /* 비표준, NUMBER(38,N/A) */
        ELDT_DOMAIN_FLOATING_NUMBER,
        DTL_TYPE_NUMBER,
        "NUMBER",
        STL_FALSE,
        DTL_LENGTH_NA,
        ELDT_DOMAIN_PRECISION_FLOATING_NUMBER
    }
};


/**
 * @brief DEFINITION_SCHEMA 에 속하는 Table Descriptor 들
 * @remarks eldtDefinitionTableID 와 동일한 순서로 정의해야 함.
 */
eldtDefinitionTableDesc * gEldtTableDescSCHEMA[ELDT_TABLE_ID_MAX] =
{
    NULL,                                       /**< ELDT_TABLE_ID_NA */
    
    /*
     * SQL 표준 core tables
     */

    & gEldtTableDescTABLES,                     /**< SCHEMA.TABLES */
    & gEldtTableDescCOLUMNS,                    /**< SCHEMA.COLUMNS */ 
    & gEldtTableDescDATA_TYPE_DESCRIPTOR,       /**< SCHEMA.DATA_TYPE_DESCRIPTOR */

    /*
     * SQL 표준 tables
     */
    
    & gEldtTableDescAUTHORIZATIONS,             /**< SCHEMA.AUTHORIZATIONS */
    & gEldtTableDescCATALOG,                    /**< SCHEMA.CATALOG_NAME */
    & gEldtTableDescCHECK_COLUMN_USAGE,         /**< SCHEMA.CHECK_COLUMN_USAGE */
    & gEldtTableDescCHECK_CONSTRAINTS,          /**< SCHEMA.CHECK_CONSTRAINTS */
    & gEldtTableDescCHECK_TABLE_USAGE,          /**< SCHEMA.CHECK_TABLE_USAGE */
    & gEldtTableDescCOLUMN_PRIVILEGES,          /**< SCHEMA.COLUMN_PRIVILEGES */
    & gEldtTableDescKEY_COLUMN_USAGE,           /**< SCHEMA.KEY_COLUMN_USAGE */
    & gEldtTableDescREFERENTIAL_CONSTRAINTS,    /**< SCHEMA.REFERENTIAL_CONSTRAINTS */
    & gEldtTableDescROLE_AUTHORIZATION_DESCRIPTORS, /**< SCHEMA.ROLE_AUTHORIZATION_DESC.. */
    & gEldtTableDescSCHEMATA,                   /**< SCHEMA.SCHEMATA */
    & gEldtTableDescSEQUENCES,                  /**< SCHEMA.SEQUENCES */
    & gEldtTableDescSQL_CONFORMANCE,            /**< SCHEMA.CONFORMANCE */
    & gEldtTableDescSQL_IMPLEMENTATION_INFO,    /**< SCHEMA.SQL_IMPLEMENTATION_INFO */
    & gEldtTableDescSQL_SIZING,                 /**< SCHEMA.SQL_SIZING */
    & gEldtTableDescTABLE_CONSTRAINTS,          /**< SCHEMA.TABLE_CONSTRAINTS */
    & gEldtTableDescTABLE_PRIVILEGES,           /**< SCHEMA.TABLE_PRIVILEGES */
    & gEldtTableDescUSAGE_PRIVILEGES,           /**< SCHEMA.USAGE_PRIVILEGES */
    & gEldtTableDescVIEW_TABLE_USAGE,           /**< SCHEMA.VIEW_TABLE_USAGE */
    & gEldtTableDescVIEWS,                      /**< SCHEMA.VIEWS */

    /*
     * 비표준 tables
     */

    & gEldtTableDescTYPE_INFO,                  /**< SCHEMA.DATA_TYPES */
    & gEldtTableDescDATABASE_PRIVILEGES,        /**< SCHEMA.DATABASE_PRIVILEGES */
    & gEldtTableDescINDEX_KEY_COLUMN_USAGE,     /**< SCHEMA.INDEX_KEY_COLUMN_USAGE */
    & gEldtTableDescINDEX_KEY_TABLE_USAGE,      /**< SCHEMA.INDEX_KEY_TABLE_USAGE */
    & gEldtTableDescINDEXES,                    /**< SCHEMA.INDEXES */
    & gEldtTableDescPENDING_TRANS,              /**< SCHEMA.PENDING_TRANS */
    & gEldtTableDescSCHEMA_PRIVILEGES,          /**< SCHEMA.SCHEMA_PRIVILEGES */
    & gEldtTableDescTABLESPACE_PRIVILEGES,      /**< SCHEMA.TABLESPACE_PRIVILEGES */
    & gEldtTableDescTABLESPACES,                /**< SCHEMA.TABLESPACES */
    & gEldtTableDescUSER_SCHEMA_PATH,           /**< SCHEMA.USER_SCHEMA_PATH */
    & gEldtTableDescUSERS,                      /**< SCHEMA.USERS */
    & gEldtTableDescSYNONYMS,                   /**< SCHEMA.SYNONYMS */
    & gEldtTableDescPUBLIC_SYNONYMS,            /**< SCHEMA.PUBLIC_SYNONYMS */
    & gEldtTableDescPROFILES,                   /**< SCHEMA.PROFILES */
    & gEldtTableDescPROFILE_PASSWORD_PARAMETER, /**< SCHEMA.PROFILE_PASSWORD_PARAMETER */
    & gEldtTableDescPROFILE_KERNEL_PARAMETER,   /**< SCHEMA.PROFILE_KERNEL_PARAMETER */
    & gEldtTableDescUSER_PASSWORD_HISTORY,      /**< SCHEMA.USER_PASSWORD_HISTORY */
};

/**
 * @brief DEFINITION_SCHEMA 에 속하는 Column Descriptor 들
 * @remarks eldtDefinitionTableID 와 동일한 순서로 정의해야 함.
 */
eldtDefinitionColumnDescArray gEldtColumnDescSCHEMA[ELDT_TABLE_ID_MAX] =
{
    {
        0,
        NULL
    },
    
    /*
     * SQL 표준 core tables
     */

    {
        /**< SCHEMA.TABLES */
        ELDT_Tables_ColumnOrder_MAX,
        gEldtColumnDescTABLES
    },
    {
        /**< SCHEMA.COLUMNS */ 
        ELDT_Columns_ColumnOrder_MAX,
        gEldtColumnDescCOLUMNS
    },
    {
        /**< SCHEMA.DATA_TYPE_DESCRIPTOR */ 
        ELDT_DTDesc_ColumnOrder_MAX,
        gEldtColumnDescDATA_TYPE_DESCRIPTOR
    },

    /*
     * SQL 표준 tables
     */
    
    {
        /**< SCHEMA.AUTHORIZATIONS */ 
        ELDT_Auth_ColumnOrder_MAX,
        gEldtColumnDescAUTHORIZATIONS
    },
    {
        /**< SCHEMA.CATALOG_NAME */ 
        ELDT_Catalog_ColumnOrder_MAX,
        gEldtColumnDescCATALOG
    },
    {
        /**< SCHEMA.CHECK_COLUMN_USAGE */ 
        ELDT_CheckColumn_ColumnOrder_MAX,
        gEldtColumnDescCHECK_COLUMN_USAGE
    },
    {
        /**< SCHEMA.CHECK_CONSTRAINTS */ 
        ELDT_CheckConst_ColumnOrder_MAX,
        gEldtColumnDescCHECK_CONSTRAINTS
    },
    {
        /**< SCHEMA.CHECK_TABLE_USAGE */ 
        ELDT_CheckTable_ColumnOrder_MAX,
        gEldtColumnDescCHECK_TABLE_USAGE
    },
    {
        /**< SCHEMA.COLUMN_PRIVILEGES */ 
        ELDT_ColumnPriv_ColumnOrder_MAX,
        gEldtColumnDescCOLUMN_PRIVILEGES
    },
    {
        /**< SCHEMA.KEY_COLUMN_USAGE */ 
        ELDT_KeyColumn_ColumnOrder_MAX,
        gEldtColumnDescKEY_COLUMN_USAGE
    },
    {
        /**< SCHEMA.REFERENTIAL_CONSTRAINTS */ 
        ELDT_Reference_ColumnOrder_MAX,
        gEldtColumnDescREFERENTIAL_CONSTRAINTS
    },
    {
        /**< SCHEMA.ROLE_AUTHORIZATION_DESCRIPTORS */ 
        ELDT_RoleDesc_ColumnOrder_MAX,
        gEldtColumnDescROLE_AUTHORIZATION_DESCRIPTORS
    },
    {
        /**< SCHEMA.SCHEMATA */ 
        ELDT_Schemata_ColumnOrder_MAX,
        gEldtColumnDescSCHEMATA
    },
    {
        /**< SCHEMA.SEQUENCES */ 
        ELDT_Sequences_ColumnOrder_MAX,
        gEldtColumnDescSEQUENCES
    },
    {
        /**< SCHEMA.SQL_CONFORMANCE */ 
        ELDT_SqlConformance_ColumnOrder_MAX,
        gEldtColumnDescSQL_CONFORMANCE
    },
    {
        /**< SCHEMA.SQL_IMPLEMENTATION_INFO */ 
        ELDT_SqlImplementationInfo_ColumnOrder_MAX,
        gEldtColumnDescSQL_IMPLEMENTATION_INFO
    },
    {
        /**< SCHEMA.SQL_SIZING */ 
        ELDT_SqlSizing_ColumnOrder_MAX,
        gEldtColumnDescSQL_SIZING
    },
    {
        /**< SCHEMA.TABLE_CONSTRAINTS */
        ELDT_TableConst_ColumnOrder_MAX,
        gEldtColumnDescTABLE_CONSTRAINTS
    },
    {
        /**< SCHEMA.TABLE_PRIVILEGES */
        ELDT_TablePriv_ColumnOrder_MAX,
        gEldtColumnDescTABLE_PRIVILEGES
    },
    {
        /**< SCHEMA.USAGE_PRIVILEGES */
        ELDT_UsagePriv_ColumnOrder_MAX,
        gEldtColumnDescUSAGE_PRIVILEGES
    },
    {
        /**< SCHEMA.VIEW_TABLE_USAGE */
        ELDT_ViewTable_ColumnOrder_MAX,
        gEldtColumnDescVIEW_TABLE_USAGE
    },
    {
        /**< SCHEMA.VIEWS */
        ELDT_Views_ColumnOrder_MAX,
        gEldtColumnDescVIEWS
    },
    
    /*
     * 비표준 tables
     */

    {
        /**< SCHEMA.DATA_TYPES */ 
        ELDT_TypeInfo_ColumnOrder_MAX,
        gEldtColumnDescTYPE_INFO
    },
    {
        /**< SCHEMA.DATABASE_PRIVILEGES */ 
        ELDT_DBPriv_ColumnOrder_MAX,
        gEldtColumnDescDATABASE_PRIVILEGES
    },
    {
        /**< SCHEMA.INDEX_KEY_COLUMN_USAGE */ 
        ELDT_IndexKey_ColumnOrder_MAX,
        gEldtColumnDescINDEX_KEY_COLUMN_USAGE
    },
    {
        /**< SCHEMA.INDEX_KEY_TABLE_USAGE */ 
        ELDT_IndexTable_ColumnOrder_MAX,
        gEldtColumnDescINDEX_KEY_TABLE_USAGE
    },
    {
        /**< SCHEMA.INDEXES */ 
        ELDT_Indexes_ColumnOrder_MAX,
        gEldtColumnDescINDEXES
    },
    {
        /**< SCHEMA.PENDING_TRANS */ 
        ELDT_PendingTrans_ColumnOrder_MAX,
        gEldtColumnDescPENDING_TRANS
    },
    {
        /**< SCHEMA.SCHEMA_PRIVILEGES */ 
        ELDT_SchemaPriv_ColumnOrder_MAX,
        gEldtColumnDescSCHEMA_PRIVILEGES
    },
    {
        /**< SCHEMA.TABLESPACE_PRIVILEGES */ 
        ELDT_SpacePriv_ColumnOrder_MAX,
        gEldtColumnDescTABLESPACE_PRIVILEGES
    },
    {
        /**< SCHEMA.TABLESPACES */ 
        ELDT_Space_ColumnOrder_MAX,
        gEldtColumnDescTABLESPACES
    },
    {
        /**< SCHEMA.USER_SCHEMA_PATH */ 
        ELDT_UserPath_ColumnOrder_MAX,
        gEldtColumnDescUSER_SCHEMA_PATH
    },
    {
        /**< SCHEMA.USERS */ 
        ELDT_User_ColumnOrder_MAX,
        gEldtColumnDescUSERS
    },
    {
        /**< SCHEMA.SYNONYMS */ 
        ELDT_Synonyms_ColumnOrder_MAX,
        gEldtColumnDescSYNONYMS
    },
    {
        /**< SCHEMA.PUBLIC_SYNONYMS */ 
        ELDT_Public_Synonyms_ColumnOrder_MAX,
        gEldtColumnDescPUBLIC_SYNONYMS
    },
    {
        /**< SCHEMA.PROFILES */ 
        ELDT_Profile_ColumnOrder_MAX,
        gEldtColumnDescPROFILES
    },
    {
        /**< SCHEMA.PROFILE_PASSWORD_PARAMETER */ 
        ELDT_PassParam_ColumnOrder_MAX,
        gEldtColumnDescPROFILE_PASSWORD_PARAMETER
    },
    {
        /**< SCHEMA.PROFILE_KERNEL_PARAMETER */ 
        ELDT_KernelParam_ColumnOrder_MAX,
        gEldtColumnDescPROFILE_KERNEL_PARAMETER
    },
    {
        /**< SCHEMA.USER_PASSWORD_HISTORY */ 
        ELDT_PassHist_ColumnOrder_MAX,
        gEldtColumnDescUSER_PASSWORD_HISTORY
    }
};


/**
 * @brief DEFINITION_SCHEMA 에 속하는 Key Constraint Descriptor 들
 * @remarks eldtDefinitionTableID 와 동일한 순서로 정의해야 함.
 */
eldtDefinitionKeyConstDescArray gEldtKeyConstDescSCHEMA[ELDT_TABLE_ID_MAX] =
{
    {
        0,
        NULL
    },
    
    /*
     * SQL 표준 core tables
     */

    {
        /**< SCHEMA.TABLES */
        ELDT_Tables_Const_MAX,
        gEldtKeyConstDescTABLES
    },
    {
        /**< SCHEMA.COLUMNS */ 
        ELDT_Columns_Const_MAX,
        gEldtKeyConstDescCOLUMNS
    },
    {
        /**< SCHEMA.DATA_TYPE_DESCRIPTOR */ 
        ELDT_DTDesc_Const_MAX,
        gEldtKeyConstDescDATA_TYPE_DESCRIPTOR
    },

    /*
     * SQL 표준 tables
     */
    
    {
        /**< SCHEMA.AUTHORIZATIONS */ 
        ELDT_Auth_Const_MAX,
        gEldtKeyConstDescAUTHORIZATIONS
    },
    {
        /**< SCHEMA.CATALOG_NAME */
        ELDT_Catalog_Const_MAX,
        gEldtKeyConstDescCATALOG_NAME
    },
    {
        /**< SCHEMA.CHECK_COLUMN_USAGE */ 
        ELDT_CheckColumn_Const_MAX,
        gEldtKeyConstDescCHECK_COLUMN_USAGE
    },
    {
        /**< SCHEMA.CHECK_CONSTRAINTS */ 
        ELDT_CheckConst_Const_MAX,
        gEldtKeyConstDescCHECK_CONSTRAINTS
    },
    {
        /**< SCHEMA.CHECK_TABLE_USAGE */ 
        ELDT_CheckTable_Const_MAX,
        gEldtKeyConstDescCHECK_TABLE_USAGE
    },
    {
        /**< SCHEMA.COLUMN_PRIVILEGES */ 
        ELDT_ColumnPriv_Const_MAX,
        gEldtKeyConstDescCOLUMN_PRIVILEGES
    },
    {
        /**< SCHEMA.KEY_COLUMN_USAGE */ 
        ELDT_KeyColumn_Const_MAX,
        gEldtKeyConstDescKEY_COLUMN_USAGE
    },
    {
        /**< SCHEMA.REFERENTIAL_CONSTRAINTS */ 
        ELDT_Reference_Const_MAX,
        gEldtKeyConstDescREFERENTIAL_CONSTRAINTS
    },
    {
        /**< SCHEMA.ROLE_AUTHORIZATION_DESCRIPTORS */ 
        ELDT_RoleDesc_Const_MAX,
        gEldtKeyConstDescROLE_AUTHORIZATION_DESCRIPTORS
    },
    {
        /**< SCHEMA.SCHEMATA */ 
        ELDT_Schemata_Const_MAX,
        gEldtKeyConstDescSCHEMATA
    },
    {
        /**< SCHEMA.SEQUENCES */ 
        ELDT_Sequences_Const_MAX,
        gEldtKeyConstDescSEQUENCES
    },
    {
        /**< SCHEMA.SQL_CONFORMANCE */ 
        ELDT_SqlConformance_Const_MAX,
        gEldtKeyConstDescSQL_CONFORMANCE
    },
    {
        /**< SCHEMA.SQL_IMPLEMENTATION_INFO */ 
        ELDT_SqlImplementationInfo_Const_MAX,
        gEldtKeyConstDescSQL_IMPLEMENTATION_INFO
    },
    {
        /**< SCHEMA.SQL_SIZING */ 
        ELDT_SqlSizing_Const_MAX,
        gEldtKeyConstDescSQL_SIZING
    },
    {
        /**< SCHEMA.TABLE_CONSTRAINTS */
        ELDT_TableConst_Const_MAX,
        gEldtKeyConstDescTABLE_CONSTRAINTS
    },
    {
        /**< SCHEMA.TABLE_PRIVILEGES */
        ELDT_TablePriv_Const_MAX,
        gEldtKeyConstDescTABLE_PRIVILEGES
    },
    {
        /**< SCHEMA.USAGE_PRIVILEGES */
        ELDT_UsagePriv_Const_MAX,
        gEldtKeyConstDescUSAGE_PRIVILEGES
    },
    {
        /**< SCHEMA.VIEW_TABLE_USAGE */
        ELDT_ViewTable_Const_MAX,
        gEldtKeyConstDescVIEW_TABLE_USAGE
    },
    {
        /**< SCHEMA.VIEWS */
        ELDT_Views_Const_MAX,
        gEldtKeyConstDescVIEWS
    },
    
    /*
     * 비표준 tables
     */

    {
        /**< SCHEMA.DATA_TYPES */ 
        /* 별도의 Key 제약 조건이 없음 */
        ELDT_TypeInfo_Const_MAX,
        gEldtKeyConstDescTYPE_INFO
    },
    {
        /**< SCHEMA.DATABASE_PRIVILEGES */ 
        ELDT_DBPriv_Const_MAX,
        gEldtKeyConstDescDATABASE_PRIVILEGES
    },
    {
        /**< SCHEMA.INDEX_KEY_COLUMN_USAGE */ 
        ELDT_IndexKey_Const_MAX,
        gEldtKeyConstDescINDEX_KEY_COLUMN_USAGE
    },
    {
        /**< SCHEMA.INDEX_KEY_TABLE_USAGE */ 
        ELDT_IndexTable_Const_MAX,
        gEldtKeyConstDescINDEX_KEY_TABLE_USAGE
    },
    {
        /**< SCHEMA.INDEXES */ 
        ELDT_Indexes_Const_MAX,
        gEldtKeyConstDescINDEXES
    },
    {
        /**< SCHEMA.PENDING_TRANS */ 
        0,
        NULL
    },
    {
        /**< SCHEMA.SCHEMA_PRIVILEGES */ 
        ELDT_SchemaPriv_Const_MAX,
        gEldtKeyConstDescSCHEMA_PRIVILEGES
    },
    {
        /**< SCHEMA.TABLESPACE_PRIVILEGES */ 
        ELDT_SpacePriv_Const_MAX,
        gEldtKeyConstDescTABLESPACE_PRIVILEGES
    },
    {
        /**< SCHEMA.TABLESPACES */ 
        ELDT_Space_Const_MAX,
        gEldtKeyConstDescTABLESPACES
    },
    {
        /**< SCHEMA.USER_SCHEMA_PATH */ 
        ELDT_UserPath_Const_MAX,
        gEldtKeyConstDescUSER_SCHEMA_PATH
    },
    {
        /**< SCHEMA.USERS */ 
        ELDT_User_Const_MAX,
        gEldtKeyConstDescUSERS
    },
    {
        /**< SCHEMA.SYNONYMS */ 
        ELDT_Synonyms_Const_MAX,
        gEldtKeyConstDescSYNONYMS
    },
    {
        /**< SCHEMA.PUBLIC_SYNONYMS */ 
        ELDT_Public_Synonyms_Const_MAX,
        gEldtKeyConstDescPUBLIC_SYNONYMS
    },
    {
        /**< SCHEMA.PROFILES */ 
        ELDT_Profile_Const_MAX,
        gEldtKeyConstDescPROFILES
    },
    {
        /**< SCHEMA.PROFILE_PASSWORD_PARAMETER */ 
        ELDT_PassParam_Const_MAX,
        gEldtKeyConstDescPROFILE_PASSWORD_PARAMETER
    },
    {
        /**< SCHEMA.PROFILE_KERNEL_PARAMETER */ 
        ELDT_KernelParam_Const_MAX,
        gEldtKeyConstDescPROFILE_KERNEL_PARAMETER
    },
    {
        /**< SCHEMA.USER_PASSWORD_HISTORY */ 
        ELDT_PassHist_Const_MAX,
        gEldtKeyConstDescUSER_PASSWORD_HISTORY
    }
};


/**
 * @brief DEFINITION_SCHEMA 에 속하는 Index Descriptor 들
 * @remarks eldtDefinitionTableID 와 동일한 순서로 정의해야 함.
 */
eldtDefinitionIndexDescArray gEldtIndexDescSCHEMA[ELDT_TABLE_ID_MAX] =
{
    {
        0,
        NULL
    },
    
    /*
     * SQL 표준 core tables
     */

    {
        /**< SCHEMA.TABLES */
        ELDT_Tables_Index_MAX,
        gEldtIndexDescTABLES
    },
    {
        /**< SCHEMA.COLUMNS */ 
        ELDT_Columns_Index_MAX,
        gEldtIndexDescCOLUMNS
    },
    {
        /**< SCHEMA.DATA_TYPE_DESCRIPTOR */ 
        ELDT_DTDesc_Index_MAX,
        gEldtIndexDescDATA_TYPE_DESCRIPTOR
    },

    /*
     * SQL 표준 tables
     */
    
    {
        /**< SCHEMA.AUTHORIZATIONS */
        /* 부가적인 인덱스가 없음 */
        ELDT_Auth_Index_MAX,
        NULL
    },
    {
        /**< SCHEMA.CATALOG_NAME */
        /* 부가적인 인덱스가 없음 */
        ELDT_Catalog_Index_MAX,
        NULL
    },
    {
        /**< SCHEMA.CHECK_COLUMN_USAGE */ 
        ELDT_CheckColumn_Index_MAX,
        gEldtIndexDescCHECK_COLUMN_USAGE
    },
    {
        /**< SCHEMA.CHECK_CONSTRAINTS */ 
        ELDT_CheckConst_Index_MAX,
        gEldtIndexDescCHECK_CONSTRAINTS
    },
    {
        /**< SCHEMA.CHECK_TABLE_USAGE */ 
        ELDT_CheckTable_Index_MAX,
        gEldtIndexDescCHECK_TABLE_USAGE
    },
    {
        /**< SCHEMA.COLUMN_PRIVILEGES */ 
        ELDT_ColumnPriv_Index_MAX,
        gEldtIndexDescCOLUMN_PRIVILEGES
    },
    {
        /**< SCHEMA.KEY_COLUMN_USAGE */ 
        ELDT_KeyColumn_Index_MAX,
        gEldtIndexDescKEY_COLUMN_USAGE
    },
    {
        /**< SCHEMA.REFERENTIAL_CONSTRAINTS */ 
        ELDT_Reference_Index_MAX,
        gEldtIndexDescREFERENTIAL_CONSTRAINTS
    },
    {
        /**< SCHEMA.ROLE_AUTHORIZATION_DESCRIPTORS */ 
        ELDT_RoleDesc_Index_MAX,
        gEldtIndexDescROLE_AUTHORIZATION_DESCRIPTORS
    },
    {
        /**< SCHEMA.SCHEMATA */ 
        ELDT_Schemata_Index_MAX,
        gEldtIndexDescSCHEMATA
    },
    {
        /**< SCHEMA.SEQUENCES */ 
        ELDT_Sequences_Index_MAX,
        gEldtIndexDescSEQUENCES
    },
    {
        /**< SCHEMA.SQL_CONFORMANCE */ 
        ELDT_SqlConformance_Index_MAX,
        NULL
    },
    {
        /**< SCHEMA.SQL_IMPLEMENTATION_INFO */ 
        ELDT_SqlImplementationInfo_Index_MAX,
        NULL
    },
    {
        /**< SCHEMA.SQL_SIZING */ 
        ELDT_SqlSizing_Index_MAX,
        NULL
    },
    {
        /**< SCHEMA.TABLE_CONSTRAINTS */
        ELDT_TableConst_Index_MAX,
        gEldtIndexDescTABLE_CONSTRAINTS
    },
    {
        /**< SCHEMA.TABLE_PRIVILEGES */
        ELDT_TablePriv_Index_MAX,
        gEldtIndexDescTABLE_PRIVILEGES
    },
    {
        /**< SCHEMA.USAGE_PRIVILEGES */
        ELDT_UsagePriv_Index_MAX,
        gEldtIndexDescUSAGE_PRIVILEGES
    },
    {
        /**< SCHEMA.VIEW_TABLE_USAGE */
        ELDT_ViewTable_Index_MAX,
        gEldtIndexDescVIEW_TABLE_USAGE
    },
    {
        /**< SCHEMA.VIEWS */
        ELDT_Views_Index_MAX,
        gEldtIndexDescVIEWS
    },
    
    /*
     * 비표준 tables
     */

    {
        /**< SCHEMA.DATA_TYPES */ 
        ELDT_TypeInfo_Index_MAX,
        gEldtIndexDescTYPE_INFO
    },
    {
        /**< SCHEMA.DATABASE_PRIVILEGES */ 
        ELDT_DBPriv_Index_MAX,
        gEldtIndexDescDATABASE_PRIVILEGES
    },
    {
        /**< SCHEMA.INDEX_KEY_COLUMN_USAGE */ 
        ELDT_IndexKey_Index_MAX,
        gEldtIndexDescINDEX_KEY_COLUMN_USAGE
    },
    {
        /**< SCHEMA.INDEX_KEY_TABLE_USAGE */ 
        ELDT_IndexTable_Index_MAX,
        gEldtIndexDescINDEX_KEY_TABLE_USAGE
    },
    {
        /**< SCHEMA.INDEXES */ 
        ELDT_Indexes_Index_MAX,
        gEldtIndexDescINDEXES
    },
    {
        /**< SCHEMA.PENDING_TRANS */ 
        0,
        NULL
    },
    {
        /**< SCHEMA.SCHEMA_PRIVILEGES */ 
        ELDT_SchemaPriv_Index_MAX,
        gEldtIndexDescSCHEMA_PRIVILEGES
    },
    {
        /**< SCHEMA.TABLESPACE_PRIVILEGES */ 
        ELDT_SpacePriv_Index_MAX,
        gEldtIndexDescTABLESPACE_PRIVILEGES
    },
    {
        /**< SCHEMA.TABLESPACES */ 
        ELDT_Space_Index_MAX,
        gEldtIndexDescTABLESPACES
    },
    {
        /**< SCHEMA.USER_SCHEMA_PATH */ 
        ELDT_UserPath_Index_MAX,
        gEldtIndexDescUSER_SCHEMA_PATH
    },
    {
        /**< SCHEMA.USERS */ 
        ELDT_User_Index_MAX,
        gEldtIndexDescUSERS
    },
    {
        /**< SCHEMA.SYNONYMS */ 
        ELDT_Synonyms_Index_MAX,
        gEldtIndexDescSYNONYMS
    },
    {
        /**< SCHEMA.PUBLIC_SYNONYMS */ 
        0,
        NULL
    },
    {
        /**< SCHEMA.PROFILES */ 
        ELDT_Profile_Index_MAX,
        NULL
    },
    {
        /**< SCHEMA.PROFILE_PASSWORD_PARAMETER */ 
        ELDT_PassParam_Index_MAX,
        NULL
    },
    {
        /**< SCHEMA.PROFILE_KERNEL_PARAMETER */ 
        ELDT_KernelParam_Index_MAX,
        NULL
    },
    {
        /**< SCHEMA.USER_PASSWORD_HISTORY */ 
        ELDT_PassHist_Index_MAX,
        NULL
    }
};


/**
 * @brief DATABASE를 위한 DEFINITION_SCHEMA 스키마를 생성한다.
 * @remarks
 * 함수 ellBuildDatabaseDictionary() 주석 참조
 * Named Object 또는 Named Element 에 대한 descriptor 들을 추가한다.
 * - Core
 *  - TABLE
 *  - COLUMN
 * - SQL standard
 *  - AUTHORIZATION
 *  - CATALOG_NAME
 *  - SCHEMA
 *  - SEQUENCE
 *  - TABLE CONSTRAINTS
 *  - VIEW
 *  - SQL_CONFORMANCE
 *  - SQL_IMPLEMENTATION_INFO
 *  - SQL_SIZING
 * - Non standard
 *  - DATA_TYPES
 *  - INDEX
 *  - TABLESPACE
 */
stlStatus eldtBuildDEFINITION_SCHEMA( smlTransId            aTransID,
                                      smlStatement        * aStmt,
                                      stlChar             * aDatabaseName,
                                      stlChar             * aDatabaseComment,
                                      stlInt64              aFirstDictPhysicalID,
                                      void                * aFirstDictPhysicalHandle,
                                      smlTbsId              aSystemTbsID,
                                      smlTbsId              aDefaultUserTbsID,
                                      stlChar             * aDefaultUserTbsName,
                                      smlTbsId              aDefaultUndoTbsID,
                                      stlChar             * aDefaultUndoTbsName,
                                      smlTbsId              aDefaultTempTbsID,
                                      stlChar             * aDefaultTempTbsName,
                                      dtlCharacterSet       aCharacterSetID,
                                      dtlStringLengthUnit   aCharLengthUnit,
                                      stlChar             * aEncryptedSysPassword,
                                      stlChar             * aTimeZoneIntervalString,
                                      ellEnv              * aEnv )
{
    /*
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aStmt         != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDatabaseName != NULL, ELL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aFirstDictPhysicalID >= 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFirstDictPhysicalHandle != NULL, ELL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( (aCharacterSetID >= DTL_SQL_ASCII) &&
                        (aCharacterSetID < DTL_CHARACTERSET_MAX),
                        ELL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( (aCharLengthUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS) ||
                        (aCharLengthUnit == DTL_STRING_LENGTH_UNIT_OCTETS),
                        ELL_ERROR_STACK(aEnv) );
    
    STL_PARAM_VALIDATE( aTimeZoneIntervalString != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Dictionary Table 구축 과정임을 설정함
     */

    gEllIsMetaBuilding = STL_TRUE;

    /**
     * DB 생성 과정을 위한 Catalog Handle 정보를 임의로 생성
     */
    
    stlMemcpy( & gEllCacheBuildEntry->mCatalogHandle,
               & gEllSharedEntry->mPhaseCacheEntry[KNL_STARTUP_PHASE_MOUNT].mCatalogHandle,
               STL_SIZEOF(ellDictHandle) );
    
    /*
     * SQL 표준 core tables
     * 다음 Named Object 에 대한 descriptor 를 추가한다.
     * - TABLE
     * - COLUMN
     */
    
    /**
     * - Dictionary Table 들을 생성하고 Table Descriptor 를 추가한다.
     */
    STL_TRY( eldtCreateDictionaryTables( aTransID,
                                         aStmt,
                                         aFirstDictPhysicalID,
                                         aFirstDictPhysicalHandle,
                                         aSystemTbsID,
                                         aEnv )
             == STL_SUCCESS );
    
    /**
     * - Dictionary Column 과 관련된 descriptor 정보를 추가한다.
     */
    STL_TRY( eldtInsertDictionaryColumns( aTransID,
                                          aStmt,
                                          aSystemTbsID,
                                          aEnv )
             == STL_SUCCESS );


    /*
     * SQL 표준 tables
     * 다음 Named Object 에 대한 descriptor 를 추가한다.
     * - AUTHORIZATION
     * - CATALOG_NAME
     * - SCHEMA
     * - SEQUENCE
     * - TABLE CONSTRAINTS
     * - VIEW : DEFINITION_SCHEMA 에는 built-in VIEW 가 존재하지 않음.
     * - SQL_CONFORMANCE
     * - SQL_IMPLEMENTATION_INFO
     * - SQL_SIZING
     */

    /**
     * - built-in Profile 을 추가한다.
     */

    STL_TRY(eldtInsertBuiltInProfileDesc( aTransID,
                                          aStmt,
                                          aEnv )
            == STL_SUCCESS );
    
    /**
     * - built-in Authorization를 추가한다.
     */
    STL_TRY( eldtInsertBuiltInAuthDesc( aTransID,
                                        aStmt,
                                        aEncryptedSysPassword,
                                        aSystemTbsID,
                                        aDefaultUserTbsID,
                                        aDefaultTempTbsID,
                                        aEnv ) == STL_SUCCESS );

    /**
     * - Catalog(Database) 를 추가한다.
     */
    STL_TRY( eldInsertCatalogDesc( aTransID,
                                   aStmt,
                                   aDatabaseName,
                                   aDatabaseComment,
                                   aCharacterSetID,
                                   gDtlCharacterSetString[aCharacterSetID],
                                   aCharLengthUnit,
                                   aTimeZoneIntervalString,
                                   aDefaultUserTbsID,
                                   aDefaultTempTbsID,
                                   aEnv )
             == STL_SUCCESS );
    
    /**
     * - built-in Schema 를 추가한다.
     */
    STL_TRY( eldtInsertBuiltInSchemaDesc( aTransID,
                                          aStmt,
                                          aEnv )
             == STL_SUCCESS );

    /**
     * - Key Constraint (Primary Key, Foreign Key, Unique) 를 추가한다.
     */

    STL_TRY( eldtInsertKeyConstraints( aTransID,
                                       aStmt,
                                       aSystemTbsID,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * - SQL-implementation information 을 추가한다.
     */

    STL_TRY( eldtInsertSqlImplementationInfoDesc( aTransID,
                                                  aStmt,
                                                  gDtlCharacterSetString[aCharacterSetID],
                                                  aEnv )
             == STL_SUCCESS );
    
    /**
     * - conformance element 를 추가한다.
     */

    STL_TRY( eldtInsertSqlConformanceDesc( aTransID,
                                           aStmt,
                                           aEnv )
             == STL_SUCCESS );
    
    /**
     * - sizing item 을 추가한다.
     */

    STL_TRY( eldtInsertSqlSizingDesc( aTransID,
                                      aStmt,
                                      aEnv )
             == STL_SUCCESS );
    
    /*
     * 비표준 tables
     * 다음 Named Object 에 대한 descriptor 를 추가한다.
     * - TYPE_INFO
     * - INDEX
     * - TABLESPACE
     */

    /**
     * - Built-In Data Type Descriptor 를 추가한다.
     */
    
    STL_TRY( eldtInsertBuiltInTypeInfoDesc( aTransID, aStmt, aEnv ) == STL_SUCCESS );

    /**
     * - Built-In Index 를 추가한다.
     */
    STL_TRY( eldtInsertIndexes( aTransID,
                                aStmt,
                                aSystemTbsID,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * - Built-In Tablespace Descriptor 를 추가한다.
     */

    STL_TRY( eldtInsertBuiltInTablespaces( aTransID,
                                           aStmt,
                                           aSystemTbsID,
                                           aDefaultUserTbsID,
                                           aDefaultUserTbsName,
                                           aDefaultUndoTbsID,
                                           aDefaultUndoTbsName,
                                           aDefaultTempTbsID,
                                           aDefaultTempTbsName,
                                           aEnv )
             == STL_SUCCESS );

    /**
     * Dicitionary 구축이 종료되었음
     */
    
    gEllIsMetaBuilding = STL_FALSE;
         
    return STL_SUCCESS;

    STL_FINISH;

    gEllIsMetaBuilding = STL_FALSE;
    
    return STL_FAILURE;
}


/**
 * @brief Dictionary Table 들을 생성하고 Table Descriptor 를 추가한다.
 * @param[in]  aTransID                Transaction ID
 * @param[in]  aStmt                   Statement
 * @param[in]  aFirstDictPhysicalID    최초 dictionary table(SCHEMA.TABLES)의 Physical ID
 * @param[in]  aFirstDictPhysicalHandle 최초 dictionary table(SCHEMA.TABLES)의 Physical Handle
 * @param[in]  aSystemTbsID            System Tablespace의 ID
 * @param[in]  aEnv                    Execution Library Environment
 * @remarks
 */
stlStatus eldtCreateDictionaryTables( smlTransId     aTransID,
                                      smlStatement * aStmt,
                                      stlInt64       aFirstDictPhysicalID,
                                      void         * aFirstDictPhysicalHandle,
                                      smlTbsId       aSystemTbsID,
                                      ellEnv       * aEnv )
{
    stlInt32   i = 0;
    
    stlInt64   sTableID = 0;
    stlInt64   sPhysicalID[ELDT_TABLE_ID_MAX] = {0,};
    void     * sPhysicalHandle = NULL;
    
    smlTableAttr               sTableAttr;
    eldtDefinitionTableDesc  * sTableDesc = NULL;

    stlMemset( &sTableAttr, 0x00, STL_SIZEOF(smlTableAttr) );

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt         != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFirstDictPhysicalHandle != NULL, ELL_ERROR_STACK(aEnv) );

    
    sTableAttr.mValidFlags =
        SML_TABLE_LOGGING_YES |
        SML_TABLE_TYPE_MEMORY_PERSISTENT;

    sTableAttr.mLoggingFlag = STL_TRUE;
    
    for ( i = ELDT_TABLE_ID_TABLES; i < ELDT_TABLE_ID_MAX; i++ )
    {
        sTableDesc = gEldtTableDescSCHEMA[i];

        /*
         * Table을 생성
         */
        
        if ( sTableDesc->mTableID == ELDT_TABLE_ID_TABLES )
        {
            /*
             * 최초 Dictionary Table은 CREATE DATABASE시 이미 생성되어 있고,
             * 이를 입력 인자로 받는다.
             */
            sPhysicalID[sTableDesc->mTableID] = aFirstDictPhysicalID;
            sPhysicalHandle = aFirstDictPhysicalHandle;
        }
        else
        {
            /*
             * 테이블을 생성한다.
             */
            STL_TRY( smlCreateTable( aStmt,
                                     aSystemTbsID,
                                     SML_MEMORY_PERSISTENT_HEAP_ROW_TABLE,
                                     & sTableAttr,
                                     & sPhysicalID[sTableDesc->mTableID],
                                     & sPhysicalHandle,
                                     SML_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

        /*
         * Dictionary Table Handle 정보를 설정
         */
        gEldTablePhysicalHandle[sTableDesc->mTableID] = sPhysicalHandle;
    }

    
    for ( i = ELDT_TABLE_ID_TABLES; i < ELDT_TABLE_ID_MAX; i++ )
    {
        sTableDesc = gEldtTableDescSCHEMA[i];
        
        /*
         * Table Descriptor를 추가
         */

        STL_TRY( ellInsertTableDesc( aTransID,
                                     aStmt,                      /* Statement */
                                     ELDT_AUTH_ID_SYSTEM,        /* Auth ID */
                                     ELDT_SCHEMA_ID_DEFINITION_SCHEMA, /* Schema ID */
                                     & sTableID,                 /* Table ID */
                                     aSystemTbsID,               /* Tablespace ID */
                                     sPhysicalID[i],                /* Physical ID */
                                     sTableDesc->mTableName,     /* Table Name */
                                     ELL_TABLE_TYPE_BASE_TABLE,  /* BASE TABLE */
                                     sTableDesc->mComment,       /* COMMENT  */
                                     aEnv )
                 == STL_SUCCESS );
        
        /*
         * Enumeration 정의 순서와 DATABASE 정보가 일치하는 지 검증 
         */ 
        STL_ASSERT( sTableID == sTableDesc->mTableID );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Dictionary Column 과 관련된 descriptor 정보를 추가한다.
 * @param[in]  aTransID                Transaction ID
 * @param[in]  aStmt                   Statement
 * @param[in]  aSystemTbsID            Identity Column 을 생성할 테이블스페이스 ID
 * @param[in]  aEnv                    Execution Library Environment
 * @remarks
 * - Column descriptor 추가
 * - Column Data Type descriptor 추가
 * - Not Null Column 에 대해
 *  - Table Constraint descriptor 추가
 *  - Check Constraint descriptor 추가
 *  - Check Table Usage descriptor 추가
 *  - Check Column Usage descriptor 추가 
 */
stlStatus eldtInsertDictionaryColumns( smlTransId     aTransID,
                                       smlStatement * aStmt,
                                       smlTbsId       aSystemTbsID,
                                       ellEnv       * aEnv )
{
    stlInt32  i = 0;
    stlInt32  j = 0;

    stlInt32                    sTableColumnCnt  = 0;
    eldtDefinitionColumnDesc  * sTableColumnDesc = NULL;
    
    eldtDefinitionDomainDesc  * sDomainDesc = NULL;
    eldtDefinitionColumnDesc  * sColumnDesc = NULL;
    
    stlInt64                    sCalcColumnID = 0;
    stlInt64                    sColumnID     = 0;

    stlBool                     sIsNullable = STL_FALSE;

    void                      * sSequenceHandle = NULL;
    stlInt64                    sSequencePhysicalID = 0;
    
    smlSequenceAttr             sSeqInAttr;
    smlSequenceAttr             sSeqOutAttr;
    
    stlMemset( & sSeqInAttr, 0x00, STL_SIZEOF( smlSequenceAttr ) );
    stlMemset( & sSeqOutAttr, 0x00, STL_SIZEOF( smlSequenceAttr ) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt         != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * IDENTITY COLUMN attribute 설정
     */

    sSeqInAttr.mValidFlags =
        SML_SEQ_STARTWITH_YES |
        SML_SEQ_INCREMENTBY_YES |
        SML_SEQ_MAXVALUE_YES |
        SML_SEQ_MINVALUE_YES |
        SML_SEQ_CACHESIZE_YES |
        SML_SEQ_CYCLE_YES |
        SML_SEQ_NOCACHE_YES |
        SML_SEQ_NOMAXVALUE_YES |
        SML_SEQ_NOMINVALUE_YES;
    
    sSeqInAttr.mStartWith   = ELDT_IDENTITY_VALUE_START;
    sSeqInAttr.mIncrementBy = ELDT_IDENTITY_VALUE_INCREMENT;
    sSeqInAttr.mMaxValue    = ELDT_IDENTITY_VALUE_MAXIMUM;
    sSeqInAttr.mMinValue    = ELDT_IDENTITY_VALUE_MINIMUM;
    sSeqInAttr.mCacheSize   = ELDT_IDENTITY_CACHE_SIZE;
    sSeqInAttr.mCycle       = ELDT_IDENTITY_VALUE_CYCLE;
    sSeqInAttr.mNoCache     = STL_FALSE;
    sSeqInAttr.mNoMaxValue  = STL_FALSE;
    sSeqInAttr.mNoMinValue  = STL_FALSE;
    
    /**
     * - Table 개수만큼 반복
     */
    
    sCalcColumnID = 0;
    for ( i = ELDT_TABLE_ID_TABLES; i < ELDT_TABLE_ID_MAX; i++ )
    {
        /*
         * 해당 Table 의 Column Descriptor 정보를 획득
         */
        sTableColumnCnt  = gEldtColumnDescSCHEMA[i].mDictColumnCnt;
        sTableColumnDesc = gEldtColumnDescSCHEMA[i].mDictColumnDesc;
        
        for ( j = 0; j < sTableColumnCnt; j++ )
        {
            sColumnDesc = & sTableColumnDesc[j];
            sDomainDesc = & gEldtDomainDesc[sColumnDesc->mDomainNO];

            /**
             * IDENTITY COLUMN을 위한 Sequence Generator 객체를 생성
             */

            if ( sDomainDesc->mIsIdentity == STL_TRUE )
            {
                STL_TRY( smlCreateSequence( aStmt,
                                            & sSeqInAttr,
                                            & sSequencePhysicalID,
                                            & sSequenceHandle,
                                            SML_ENV( aEnv ) )
                         == STL_SUCCESS );

                /* Indentity Column Handle을 설정 */
                gEldIdentityColumnHandle[i] = sSequenceHandle;
                
                STL_TRY( smlGetSequenceAttr( sSequenceHandle,
                                             & sSeqOutAttr,
                                             SML_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
            else
            {
                sSequencePhysicalID = ELL_DICT_OBJECT_ID_NA;
            }

            /*
             * 컬럼의 nullable 여부 정보 
             */

            switch ( sColumnDesc->mNullable )
            {
                case ELDT_NULLABLE_YES:
                    {
                        /* NULLABLE */
                        sIsNullable = STL_TRUE;
                        break;
                    }
                case ELDT_NULLABLE_COLUMN_NOT_NULL:
                    {
                        /*
                         * 컬럼에 정의된 NOT NULL
                         * - CHECK CONSTRAINT 를 추가해야 하며,
                         * - 별도의 정의 없이 추가할 수 있다.
                         */
                        sIsNullable = STL_FALSE;
                        break;
                    }
                case ELDT_NULLABLE_PK_NOT_NULL:
                    {
                        /*
                         * Primary Key 에 포함된 NOT NULL
                         * - PRIMARY KEY CONSTRAINT를 추가해야 하며,
                         * - 별도의 정의를 통해 추가해야 한다.
                         */
                        sIsNullable = STL_FALSE;
                        break;
                    }
                default:
                    {
                        STL_ASSERT(0);
                        break;
                    }
            }
            
            /**
             * - SCHEMA.COLUMNS 테이블에 Column Descriptor를 추가
             */
            
            sCalcColumnID++; /* COLUMN_ID 는 SCHEMA.COLUMNS 테이블에서 Unique 하게 계속 증가 */
            STL_TRY( ellInsertColumnDesc( aTransID,
                                          aStmt,
                                          ELDT_AUTH_ID_SYSTEM,       /* Owner ID */
                                          ELDT_SCHEMA_ID_DEFINITION_SCHEMA, /* Schema ID */
                                          sColumnDesc->mTableID,     /* Table ID */
                                          ELL_TABLE_TYPE_BASE_TABLE, 
                                          & sColumnID,               /* Column ID */
                                          sColumnDesc->mColumnName,  /* Column NAME */
                                          j,                         /* Physcial Ordinal Position */
                                          j,                         /* Logical Ordinal Position */
                                          NULL,                      /* Default */
                                          sIsNullable,               /* Is Nullable */
                                          sDomainDesc->mIsIdentity,  /* Is Identity */
                                          ELL_IDENTITY_GENERATION_ALWAYS,
                                          sSeqOutAttr.mStartWith,
                                          sSeqOutAttr.mIncrementBy,
                                          sSeqOutAttr.mMaxValue,
                                          sSeqOutAttr.mMinValue,
                                          sSeqOutAttr.mCycle,
                                          aSystemTbsID,              /* Identity Tbs */
                                          sSequencePhysicalID,       /* PhysicalID */
                                          sSeqOutAttr.mCacheSize,    /* Cache Size */
                                          STL_FALSE,                 /* Is Updatable */
                                          sColumnDesc->mComment,     /* Comment */
                                          aEnv )
                     == STL_SUCCESS );
            
            /*
             * Enumeration 정의 순서와 DATABASE 정보가 일치하는 지 검증 
             */ 
            STL_ASSERT( sColumnID == sCalcColumnID );
            
                                          
            /**
             *- SCHEMA.DATA_TYPE_DESCRIPTOR 테이블에 Column 의 Data Type Descriptor를 추가
             */

            STL_TRY( eldtInsertColumnDomainDesc( aTransID,
                                                 aStmt,
                                                 sColumnDesc->mTableID,
                                                 sColumnID,
                                                 sDomainDesc,
                                                 aEnv )
                     == STL_SUCCESS );

            /**
             * - Not Null Column 인 경우(Primary Key는 아님), CHECK CONSTRAINT를 추가한다.
             */
            if ( sColumnDesc->mNullable == ELDT_NULLABLE_COLUMN_NOT_NULL )
            {
                STL_TRY( eldtInsertNotNullColumn( aTransID,
                                                  aStmt,
                                                  sColumnID,
                                                  sColumnDesc,
                                                  aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /* 별도의 Constraint 정의를 통해 제약조건을 추가해야 한다. */
            }
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    

/**
 * @brief Dictionary Column 의 Domain 정보를 이용하여 Data Type Descriptor 정보를 추가한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableID         Column 이 속한 Table ID
 * @param[in]  aColumnID        Column의 ID
 * @param[in]  aDomainDesc      Column의 Domain Descriptor
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus eldtInsertColumnDomainDesc( smlTransId                 aTransID,
                                      smlStatement             * aStmt,
                                      stlInt64                   aTableID,
                                      stlInt64                   aColumnID,
                                      eldtDefinitionDomainDesc * aDomainDesc,
                                      ellEnv                   * aEnv )
{
    stlInt64   sOwnerID  = ELDT_AUTH_ID_SYSTEM;
    stlInt64   sSchemaID = ELDT_SCHEMA_ID_DEFINITION_SCHEMA;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL,       ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnID > 0,       ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDomainDesc != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * @remarks - Column 의 Type Descriptor 정보를 추가 
     */
    
    switch ( aDomainDesc->mDataTypeID )
    {
        case DTL_TYPE_BOOLEAN:
            {
                /* BOOLEAN */
                STL_TRY( ellInsertBooleanColumnTypeDesc( aTransID,
                                                         aStmt,
                                                         sOwnerID,
                                                         sSchemaID,
                                                         aTableID,
                                                         aColumnID,
                                                         aDomainDesc->mDataTypeName,
                                                         aEnv )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NATIVE_SMALLINT:      
            {
                /* NATIVE_SMALLINT */
                STL_TRY( ellInsertSmallintColumnTypeDesc( aTransID,
                                                          aStmt,
                                                          sOwnerID,
                                                          sSchemaID,
                                                          aTableID,
                                                          aColumnID,
                                                          aDomainDesc->mDataTypeName,
                                                          aEnv )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NATIVE_INTEGER:      
            {
                /* NATIVE_INTEGER */
                STL_TRY( ellInsertIntegerColumnTypeDesc( aTransID,
                                                         aStmt,
                                                         sOwnerID,
                                                         sSchemaID,
                                                         aTableID,
                                                         aColumnID,
                                                         aDomainDesc->mDataTypeName,
                                                         aEnv )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NATIVE_BIGINT:        
            {
                /* NATIVE_BIGINT */                        
                STL_TRY( ellInsertBigintColumnTypeDesc( aTransID,
                                                        aStmt,
                                                        sOwnerID,
                                                        sSchemaID,
                                                        aTableID,
                                                        aColumnID,
                                                        aDomainDesc->mDataTypeName,
                                                        aEnv )
                         == STL_SUCCESS );
                break;
            }
                    
        case DTL_TYPE_NATIVE_REAL:          
            {
                /* NATIVE_REAL */                        
                STL_TRY( ellInsertRealColumnTypeDesc( aTransID,
                                                      aStmt,
                                                      sOwnerID,
                                                      sSchemaID,
                                                      aTableID,
                                                      aColumnID,
                                                      aDomainDesc->mDataTypeName,
                                                      aEnv )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NATIVE_DOUBLE:        
            {
                /* NATIVE_DOUBLE  */
                STL_TRY( ellInsertDoubleColumnTypeDesc( aTransID,
                                                        aStmt,
                                                        sOwnerID,
                                                        sSchemaID,
                                                        aTableID,
                                                        aColumnID,
                                                        aDomainDesc->mDataTypeName,
                                                        aEnv )
                         == STL_SUCCESS );
                break;
            }

        case DTL_TYPE_FLOAT:
            {
                STL_TRY( ellInsertFloatColumnTypeDesc( aTransID,
                                                       aStmt,
                                                       sOwnerID,
                                                       sSchemaID,
                                                       aTableID,
                                                       aColumnID,
                                                       aDomainDesc->mDataTypeName,
                                                       DTL_NUMERIC_PREC_RADIX_BINARY,
                                                       aDomainDesc->mPrecision,
                                                       DTL_SCALE_NA,
                                                       aEnv )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NUMBER:       
            {
                STL_TRY( ellInsertNumericColumnTypeDesc( aTransID,
                                                         aStmt,
                                                         sOwnerID,
                                                         sSchemaID,
                                                         aTableID,
                                                         aColumnID,
                                                         aDomainDesc->mDataTypeName,
                                                         DTL_NUMERIC_PREC_RADIX_DECIMAL,
                                                         aDomainDesc->mPrecision,
                                                         DTL_SCALE_NA,
                                                         aEnv )
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
                /**
                 * DEFINITION_SCHEMA 의 모든 CHAR type 은 Octet Length 기준임
                 */
                
                /* CHARACTER */
                STL_TRY( ellInsertCharColumnTypeDesc( aTransID,
                                                      aStmt,
                                                      sOwnerID,
                                                      sSchemaID,
                                                      aTableID,
                                                      aColumnID,
                                                      aDomainDesc->mDataTypeName,
                                                      DTL_STRING_LENGTH_UNIT_OCTETS,
                                                      aDomainDesc->mLength,
                                                      aEnv )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_VARCHAR:       
            {
                /**
                 * DEFINITION_SCHEMA 의 모든 VARCHAR type 은 Octet Length 기준임
                 */

                /* CHARACTER VARYING */
                STL_TRY( ellInsertVarcharColumnTypeDesc( aTransID,
                                                         aStmt,
                                                         sOwnerID,
                                                         sSchemaID,
                                                         aTableID,
                                                         aColumnID,
                                                         aDomainDesc->mDataTypeName,
                                                         DTL_STRING_LENGTH_UNIT_OCTETS,
                                                         aDomainDesc->mLength,
                                                         aEnv )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_LONGVARCHAR:   
            {
                /* CHARACTER LONG VARYING */
                STL_TRY( ellInsertLongVarcharColumnTypeDesc( aTransID,
                                                             aStmt,
                                                             sOwnerID,
                                                             sSchemaID,
                                                             aTableID,
                                                             aColumnID,
                                                             aDomainDesc->mDataTypeName,
                                                             aEnv )
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
                /* BINARY */
                STL_TRY( ellInsertBinaryColumnTypeDesc( aTransID,
                                                        aStmt,
                                                        sOwnerID,
                                                        sSchemaID,
                                                        aTableID,
                                                        aColumnID,
                                                        aDomainDesc->mDataTypeName,
                                                        aDomainDesc->mLength,
                                                        aEnv )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_VARBINARY:     
            {
                /* BINARY VARYING */
                STL_TRY( ellInsertVarBinaryColumnTypeDesc( aTransID,
                                                           aStmt,
                                                           sOwnerID,
                                                           sSchemaID,
                                                           aTableID,
                                                           aColumnID,
                                                           aDomainDesc->mDataTypeName,
                                                           aDomainDesc->mLength,
                                                           aEnv )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_LONGVARBINARY: 
            {
                /* BINARY LONG VARYING */
                STL_TRY( ellInsertLongVarBinaryColumnTypeDesc( aTransID,
                                                               aStmt,
                                                               sOwnerID,
                                                               sSchemaID,
                                                               aTableID,
                                                               aColumnID,
                                                               aDomainDesc->mDataTypeName,
                                                               aEnv )
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
                /* DATE */
                STL_TRY( ellInsertDateColumnTypeDesc( aTransID,
                                                      aStmt,
                                                      sOwnerID,
                                                      sSchemaID,
                                                      aTableID,
                                                      aColumnID,
                                                      aDomainDesc->mDataTypeName,
                                                      aEnv )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_TIME:                     
            {
                /* TIME WITHOUT TIME ZONE */
                STL_TRY( ellInsertTimeColumnTypeDesc( aTransID,
                                                      aStmt,
                                                      sOwnerID,
                                                      sSchemaID,
                                                      aTableID,
                                                      aColumnID,
                                                      aDomainDesc->mDataTypeName,
                                                      aDomainDesc->mPrecision,
                                                      aEnv )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_TIMESTAMP:                
            {
                /* TIMESTAMP WITHOUT TIME ZONE */
                STL_TRY( ellInsertTimestampColumnTypeDesc( aTransID,
                                                           aStmt,
                                                           sOwnerID,
                                                           sSchemaID,
                                                           aTableID,
                                                           aColumnID,
                                                           aDomainDesc->mDataTypeName,
                                                           aDomainDesc->mPrecision,
                                                           aEnv )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_TIME_WITH_TIME_ZONE:      
            {
                /* TIME WITH TIME ZONE */
                STL_TRY( ellInsertTimeWithZoneColumnTypeDesc( aTransID,
                                                              aStmt,
                                                              sOwnerID,
                                                              sSchemaID,
                                                              aTableID,
                                                              aColumnID,
                                                              aDomainDesc->mDataTypeName,
                                                              aDomainDesc->mPrecision,
                                                              aEnv )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE: 
            {
                /* TIMESTAMP WITH TIME ZONE */
                STL_TRY( ellInsertTimestampWithZoneColumnTypeDesc( aTransID,
                                                                   aStmt,
                                                                   sOwnerID,
                                                                   sSchemaID,
                                                                   aTableID,
                                                                   aColumnID,
                                                                   aDomainDesc->mDataTypeName,
                                                                   aDomainDesc->mPrecision,
                                                                   aEnv )
                         == STL_SUCCESS );
                break;
            }
                    
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            {
                /* INTERVAL YEAR TO MONTH */
                STL_TRY( ellInsertIntervalYearToMonthColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sOwnerID,
                             sSchemaID,
                             aTableID,
                             aColumnID,
                             aDomainDesc->mDataTypeName,
                             ELDT_DOMAIN_INTERVAL_INDICATOR,
                             ELDT_DOMAIN_INTERVAL_START_PRECISION,
                             DTL_PRECISION_NA,
                             aEnv )
                         == STL_SUCCESS );
                break;
            }

        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                /* INTERVAL */
                STL_TRY( ellInsertIntervalDayToSecondColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sOwnerID,
                             sSchemaID,
                             aTableID,
                             aColumnID,
                             aDomainDesc->mDataTypeName,
                             ELDT_DOMAIN_INTERVAL_INDICATOR,
                             ELDT_DOMAIN_INTERVAL_START_PRECISION,
                             DTL_PRECISION_NA,
                             aEnv )
                         == STL_SUCCESS );
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
                      ELL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      ELL_ERROR_STACK(aEnv),
                      "eldtInsertColumnDomainDesc()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DEFINITION_SCHEMA 의 Key Constraint 관련 정보를 추가한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTablespaceID    Key Constraint 를 위해 생성할 Index의 tablespace ID
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus eldtInsertKeyConstraints( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    stlInt64       aTablespaceID,
                                    ellEnv       * aEnv )
{
    stlInt32  i = 0;
    stlInt32  j = 0;

    stlInt64  sCalcConstID = ELL_DICT_OBJECT_ID_NA;
    stlInt64  sConstID = ELL_DICT_OBJECT_ID_NA;
    stlInt64  sConstIndexID = ELL_DICT_OBJECT_ID_NA;
    
    stlInt32                       sConstCnt  = 0;
    eldtDefinitionKeyConstDesc   * sConstDesc = NULL;

    stlChar   sConstName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    
    /*
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - DEFINITION_SCHEMA에 정의된 Table 수만큼 반복
     */

    /* Key Constraint는 Not Null Constraint 가 모두 구축된 후 생성함 */
    sCalcConstID = gEldtNotNullMaxConstID;
    
    for ( i = ELDT_TABLE_ID_TABLES; i < ELDT_TABLE_ID_MAX; i++ )
    {
        /**
         * - 해당 Table 의 Table Constraint Descriptor 정보를 획득
         */
        sConstCnt  = gEldtKeyConstDescSCHEMA[i].mDictConstCnt;
        sConstDesc = gEldtKeyConstDescSCHEMA[i].mDictConstDesc;
        
        for ( j = 0; j < sConstCnt; j++ )
        {
            sCalcConstID++; /* CONSTRAINT_ID 는 Unique 하게 계속 증가 */

            /**
             * - key constraint name 생성
             */
            
            STL_TRY( eldtSetKeyConstraintName( sConstName,
                                               STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                                               & sConstDesc[j],
                                               aEnv )
                     == STL_SUCCESS );
            
            
            /**
             * - Key Constraint 의 경우 Index 를 생성함
             */

            switch ( sConstDesc[j].mConstType )
            {
                case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
                case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
                case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
                    {
                        STL_TRY( eldtInsertKeyIndex( aTransID,
                                                     aStmt,
                                                     & sConstDesc[j],
                                                     sConstName,
                                                     & sConstIndexID,
                                                     aTablespaceID,
                                                     aEnv )
                                 == STL_SUCCESS );
                        
                        break;
                    }
                case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
                case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
                default:
                    {
                        STL_ASSERT(0);
                        break;
                    }
            }

            /**
             * - table constraint descriptor 를 추가
             */

            STL_TRY( ellInsertTableConstraintDesc(
                         aTransID,
                         aStmt,
                         ELDT_AUTH_ID_SYSTEM,   
                         ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                         & sConstID, /* Constraint ID */
                         ELDT_AUTH_ID_SYSTEM,   
                         ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                         sConstDesc[j].mTableID,
                         sConstName,
                         sConstDesc[j].mConstType,
                         ELL_TABLE_CONSTRAINT_DEFERRABLE_DEFAULT, 
                         ELL_TABLE_CONSTRAINT_INITIALLY_DEFERRED_DEFAULT, 
                         ELL_TABLE_CONSTRAINT_ENFORCED_DEFAULT, 
                         sConstIndexID,
                         NULL,
                         aEnv )
                     == STL_SUCCESS );
                                                   
            /*
             * Enumeration 정의 순서와 DATABASE 정보가 일치하는 지 검증 
             */ 
            STL_ASSERT( sConstID == sCalcConstID );

            
            /**
             * - Constraint 유형별로 필요한 descriptor 정보를 추가 
             */

            switch ( sConstDesc[j].mConstType )
            {
                case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
                    {
                        /* key column usage descriptor 들을 추가 */
                        STL_TRY( eldtInsertKeyDesc( aTransID,
                                                    aStmt,
                                                    sConstID,
                                                    & sConstDesc[j],
                                                    aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
                    {
                        /* key column usage descriptor 들을 추가 */
                        STL_TRY( eldtInsertKeyDesc( aTransID,
                                                    aStmt,
                                                    sConstID,
                                                    & sConstDesc[j],
                                                    aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
                    {
                        /* referential constraint descriptor 를 추가 */
                        STL_TRY( eldtInsertForeignKey( aTransID,
                                                       aStmt,
                                                       sConstID,
                                                       & sConstDesc[j],
                                                       aEnv )
                                 == STL_SUCCESS );
                        
                        /* key column usage descriptor 들을 추가 */
                        STL_TRY( eldtInsertKeyDesc( aTransID,
                                                    aStmt,
                                                    sConstID,
                                                    & sConstDesc[j],
                                                    aEnv )
                                 == STL_SUCCESS );
                        
                        break;
                    }
                default:
                    {
                        STL_ASSERT(0);
                        break;
                    }
            }
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DEFINITION_SCHEMA 에 Key descriptor 정보를 추가한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aConstID         Constraint ID
 * @param[in]  aConstDesc       Key 제약 조건 
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus eldtInsertKeyDesc( smlTransId                     aTransID,
                             smlStatement                 * aStmt,
                             stlInt64                       aConstID,
                             eldtDefinitionKeyConstDesc   * aConstDesc,
                             ellEnv                       * aEnv )
{
    stlInt32    i = 0;
    stlInt32    sPositionInUniqueConst = 0;
    stlInt32    sKeyCnt = 0;
    stlInt64    sRefColumnID = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDesc != NULL, ELL_ERROR_STACK(aEnv) );

    sKeyCnt = aConstDesc->mKeyCount;
    STL_PARAM_VALIDATE( (sKeyCnt > 0) && (sKeyCnt <= ELDT_KEY_COLUMN_COUNT_MAXIMUM),
                        ELL_ERROR_STACK(aEnv) );

    /**
     * - Key 갯수만큼 반복
     */
    
    for ( i = 0; i < sKeyCnt; i++ )
    {
        /**
         * - Column ID 를 계산
         */
        
        STL_TRY( eldtGetDictColumnID( aConstDesc->mTableID,
                                      aConstDesc->mColOrder[i],
                                      & sRefColumnID,
                                      aEnv )
                 == STL_SUCCESS );

        /**
         * - key column descriptor 를 추가
         */

        if ( aConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY )
        {
            sPositionInUniqueConst = i;
        }
        else
        {
            sPositionInUniqueConst = ELL_DICT_POSITION_NA;
        }
        
        STL_TRY( ellInsertKeyColumnUsageDesc( aTransID,
                                              aStmt,
                                              ELDT_AUTH_ID_SYSTEM,   
                                              ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                              aConstID,
                                              ELDT_AUTH_ID_SYSTEM,
                                              ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                              aConstDesc->mTableID,
                                              sRefColumnID,
                                              i,
                                              sPositionInUniqueConst,
                                              aEnv )
                 == STL_SUCCESS );
    }
                                              
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DEFINITION_SCHEMA 에 Foreign Key 정보를 추가한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aConstID         Constraint ID
 * @param[in]  aConstDesc       Foreign Key 제약 조건 
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus eldtInsertForeignKey( smlTransId                     aTransID,
                                smlStatement                 * aStmt,
                                stlInt64                       aConstID,
                                eldtDefinitionKeyConstDesc   * aConstDesc,
                                ellEnv                       * aEnv )
{
    stlInt64   sUniqueConstID = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - 참조하는 Unique Constraint ID 를 계산
     */
    
    STL_TRY( eldtGetDictKeyConstID( aConstDesc->mRefTableID,
                                    aConstDesc->mRefConstNum,
                                    & sUniqueConstID,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * - referential constraint descriptor 를 추가
     */

    STL_TRY( ellInsertRefConstraintDesc( aTransID,
                                         aStmt,
                                         ELDT_AUTH_ID_SYSTEM,   
                                         ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                         aConstID,
                                         ELDT_AUTH_ID_SYSTEM,
                                         ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                         sUniqueConstID,
                                         ELL_REFERENTIAL_MATCH_OPTION_FULL,
                                         ELL_REFERENTIAL_ACTION_RULE_NO_ACTION,
                                         ELL_REFERENTIAL_ACTION_RULE_NO_ACTION,
                                         aEnv )
             == STL_SUCCESS );
                                         
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DEFINITION_SCHEMA 에 Not Null 컬럼을 위한 NOT NULL CHECK CONSTRAINT를 추가한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aColumnID        Not Null 컬럼의 COLUMN ID
 * @param[in]  aColumnDesc      Not Null 컬럼의 Column Descriptor
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus eldtInsertNotNullColumn( smlTransId                     aTransID,
                                   smlStatement                 * aStmt,
                                   stlInt64                       aColumnID,
                                   eldtDefinitionColumnDesc     * aColumnDesc,
                                   ellEnv                       * aEnv )
{
    stlInt64    sConstID = 0;
    stlInt32    sNameLen = 0;
    stlChar     sConstName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /*
     * Not Null Check Constraint 의 제약조건 이름을 생성
     */

    sNameLen = stlSnprintf( sConstName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                            "%s_%s_NOT_NULL",
                            gEldtTableDescSCHEMA[aColumnDesc->mTableID]->mTableName, 
                            aColumnDesc->mColumnName );
    STL_ASSERT( sNameLen <= STL_MAX_SQL_IDENTIFIER_LENGTH );
                            
    /**
     * - table constraint descriptor 를 추가
     */
    
    STL_TRY( ellInsertTableConstraintDesc( aTransID,
                                           aStmt,
                                           ELDT_AUTH_ID_SYSTEM,   
                                           ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                           & sConstID, /* Constraint ID */
                                           ELDT_AUTH_ID_SYSTEM,   
                                           ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                           aColumnDesc->mTableID,
                                           sConstName,
                                           ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL,
                                           ELL_TABLE_CONSTRAINT_DEFERRABLE_DEFAULT, 
                                           ELL_TABLE_CONSTRAINT_INITIALLY_DEFERRED_DEFAULT, 
                                           ELL_TABLE_CONSTRAINT_ENFORCED_DEFAULT, 
                                           ELL_DICT_OBJECT_ID_NA, /* Index ID */
                                           NULL,        /* COMMENT */
                                           aEnv )
             == STL_SUCCESS );

    /**
     * - check constraint descriptor 를 추가
     */

    STL_TRY( ellInsertCheckConstraintDesc ( aTransID,
                                            aStmt,
                                            ELDT_AUTH_ID_SYSTEM,   
                                            ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                            sConstID,
                                            NULL,    /* NOT NULL check constraint 임 */
                                            aEnv )
             == STL_SUCCESS );
                                            
    /**
     * - check table usage descriptor 를 추가
     */

    STL_TRY( ellInsertCheckTableUsageDesc( aTransID,
                                           aStmt,
                                           ELDT_AUTH_ID_SYSTEM,   
                                           ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                           sConstID,
                                           ELDT_AUTH_ID_SYSTEM,   
                                           ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                           aColumnDesc->mTableID,
                                           aEnv )
             == STL_SUCCESS );
                                           
    /**
     * - check column usage descriptor 를 추가
     */

    STL_TRY( ellInsertCheckColumnUsageDesc( aTransID,
                                            aStmt,
                                            ELDT_AUTH_ID_SYSTEM,   
                                            ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                            sConstID,
                                            ELDT_AUTH_ID_SYSTEM,   
                                            ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                            aColumnDesc->mTableID,
                                            aColumnID,
                                            aEnv )
             == STL_SUCCESS );

    /**
     * - 최대 CONSTRAINT ID 를 저장
     */
    gEldtNotNullMaxConstID = sConstID;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Key Constraint 를 위한 Index 를 추가한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aConstDesc     Key Constraint Descriptor
 * @param[in]  aKeyConstraintName Key Constraint Name
 * @param[out] aIndexID       생성한 Index ID
 * @param[in]  aTablespaceID  Tablespace ID
 * @param[in]  aEnv           Environment
 */
stlStatus eldtInsertKeyIndex( smlTransId                     aTransID,
                              smlStatement                 * aStmt,
                              eldtDefinitionKeyConstDesc   * aConstDesc,
                              stlChar                      * aKeyConstraintName,
                              stlInt64                     * aIndexID,
                              stlInt64                       aTablespaceID,
                              ellEnv                       * aEnv )
{
    stlInt64 sPhysicalID = ELL_DICT_OBJECT_ID_NA;
    void   * sPhysicalHandle = NULL;
    
    stlInt64 sIndexID = 0;
    stlBool  sUniqueIndex = STL_FALSE;
    stlChar  sIndexName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];

    stlInt64 sColumnID = 0;
    
    stlInt32 i = 0;

    smlIndexAttr  sIndexAttr;
    eldMemMark    sMemMark;

    knlValueBlockList * sTableValueList = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    stlUInt8         * sKeyColFlags = NULL;
    stlUInt8           sKeyFlag = 0;

    stlInt32 sState = 0;
    
    stlMemset( & sIndexAttr, 0x00, STL_SIZEOF(smlIndexAttr) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDesc != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyConstraintName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexID != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Unique Index 여부 결정
     */
    switch ( aConstDesc->mConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            {
                /* Primary Key 와 Unique Key 는 Unique Index */
                sUniqueIndex = STL_TRUE;
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                /* Foreign Key 는 Non-Unique Index */
                sUniqueIndex = STL_FALSE;
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /**
     * Index Name 설정
     */

    STL_TRY( eldtSetIndexName( sIndexName,
                               STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                               aKeyConstraintName,
                               aConstDesc->mTableID,
                               aConstDesc->mKeyCount,
                               aConstDesc->mColOrder,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Index 생성을 위한 정보 생성
     * Diable Index 로 생성한다.
     */

    /*
     * Index Attribute
     */
    
    sIndexAttr.mValidFlags     |= SML_INDEX_UNIQUENESS_MASK;
    sIndexAttr.mUniquenessFlag = sUniqueIndex;

    sIndexAttr.mValidFlags     |= SML_INDEX_LOGGING_YES;
    sIndexAttr.mLoggingFlag    = STL_TRUE;

    sIndexAttr.mValidFlags |= SML_INDEX_PRIMARY_MASK;
    if ( aConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY )
    {
        sIndexAttr.mPrimaryFlag = STL_TRUE;
    }
    else
    {
        sIndexAttr.mPrimaryFlag = STL_FALSE;
    }
    
    /*
     * Table Value Array 할당
     */

    STL_TRY( eldAllocTableValueList( aConstDesc->mTableID,
                                     & sMemMark,
                                     & sTableValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    /*
     * Index Value Array 할당 
     */

    STL_TRY( eldShareKeyConstIndexValueList( aConstDesc,
                                             sTableValueList,
                                             & sIndexValueList,
                                             aEnv )
             == STL_SUCCESS );

    
    /*
     * Index Key Column Flag
     */

    if ( aConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY )
    {
        sKeyFlag =
            DTL_KEYCOLUMN_INDEX_ORDER_ASC |
            DTL_KEYCOLUMN_INDEX_NULLABLE_FALSE |
            DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST;
    }
    else
    {
        sKeyFlag =
            DTL_KEYCOLUMN_INDEX_ORDER_ASC |
            DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE |
            DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST;
    }
        
    STL_TRY( eldAllocOPMem( (void**) & sKeyColFlags,
                            aConstDesc->mKeyCount * STL_SIZEOF(stlUInt8),
                            aEnv )
             == STL_SUCCESS );

    for ( i = 0; i < aConstDesc->mKeyCount; i++ )
    {
        sKeyColFlags[i] = sKeyFlag;
    }
    
    /**
     * Index를 생성 
     * - 레코드는 추가하지 않아야 함, Dictionary 완료후 Index에 일괄 추가
     */

        
    sPhysicalID = ELL_DICT_OBJECT_ID_NA;
    sPhysicalHandle = NULL;

    STL_TRY( smlCreateIndex( aStmt,
                             aTablespaceID,
                             SML_INDEX_TYPE_MEMORY_PERSISTENT_BTREE,
                             & sIndexAttr,
                             gEldTablePhysicalHandle[aConstDesc->mTableID],
                             aConstDesc->mKeyCount,
                             sIndexValueList,
                             sKeyColFlags,
                             & sPhysicalID,
                             & sPhysicalHandle,
                             SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Index descriptor 추가
     */

    STL_TRY( ellInsertIndexDesc( aTransID,
                                 aStmt,
                                 ELDT_AUTH_ID_SYSTEM,   
                                 ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                 & sIndexID, 
                                 aTablespaceID,
                                 sPhysicalID,
                                 sIndexName,
                                 ELL_INDEX_TYPE_BTREE,
                                 sUniqueIndex,
                                 STL_FALSE,/* invalid */
                                 STL_TRUE, /* BY CONSTRAINT */
                                 NULL,     /* index comment */
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dictionary 구축 후 Index Data 를 구축하기 위한 정보 저장
     */

    gEldtIndexCnt++;
    gEldtIndexInfo[sIndexID].mIndexHandle = sPhysicalHandle;
    gEldtIndexInfo[sIndexID].mIsKeyConst = STL_TRUE;  /* Key Constraint 로부터 생성된 인덱스 */
    gEldtIndexInfo[sIndexID].mDescInfo = (void *) aConstDesc;

    /**
     * Index Key Table Usage Descriptor 추가
     */

    STL_TRY( ellInsertIndexKeyTableUsageDesc( aTransID,
                                              aStmt,
                                              ELDT_AUTH_ID_SYSTEM,   
                                              ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                              sIndexID,
                                              ELDT_AUTH_ID_SYSTEM,   
                                              ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                              aConstDesc->mTableID,
                                              aEnv )
             == STL_SUCCESS );
    
    /**
     * Index Key Column Usage descriptor 추가
     */

    for ( i = 0; i < aConstDesc->mKeyCount; i++ )
    {
        /* Column ID 획득 */
        STL_TRY( eldtGetDictColumnID( aConstDesc->mTableID,
                                      aConstDesc->mColOrder[i],
                                      & sColumnID,
                                      aEnv )
                 == STL_SUCCESS );

        /* index key column usage descriptor 추가 */
        STL_TRY( ellInsertIndexKeyColumnUsageDesc( aTransID,
                                                   aStmt,
                                                   ELDT_AUTH_ID_SYSTEM,   
                                                   ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                                   sIndexID,
                                                   ELDT_AUTH_ID_SYSTEM,   
                                                   ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                                   aConstDesc->mTableID,
                                                   sColumnID,
                                                   i,    /* key order */
                                                   ELL_INDEX_COLUMN_ORDERING_DEFAULT,
                                                   ELL_INDEX_COLUMN_NULLS_ORDERING_DEFAULT,
                                                   aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    *aIndexID = sIndexID;
                                 
    return STL_SUCCESS;
    
    STL_FINISH;

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief DEFINITION_SCHEMA 의 Index 관련 정보를 추가한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTablespaceID    생성할 Index의 tablespace ID
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus eldtInsertIndexes( smlTransId     aTransID,
                             smlStatement * aStmt,
                             stlInt64       aTablespaceID,
                             ellEnv       * aEnv )
{
    stlInt32  i = 0;
    stlInt32  j = 0;
    stlInt32  k = 0;

    stlInt64  sIndexID = ELL_DICT_OBJECT_ID_NA;
    stlInt64  sPhysicalID = 0;
    void    * sPhysicalHandle = NULL;
    stlInt64  sColumnID = 0;
    
    stlInt32                   sIndexCnt  = 0;
    eldtDefinitionIndexDesc  * sIndexDesc = NULL;

    stlChar   sIndexName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];

    smlIndexAttr  sIndexAttr;
    eldMemMark    sMemMark;

    knlValueBlockList * sTableValueList = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    stlUInt8         * sKeyColFlags = NULL;
    stlUInt8           sKeyFlag = 0;

    stlInt32  sState = 0;
    
    stlMemset( & sIndexAttr, 0x00, STL_SIZEOF(smlIndexAttr) );
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - DEFINITION_SCHEMA에 정의된 Table 수만큼 반복
     */

    for ( i = ELDT_TABLE_ID_TABLES; i < ELDT_TABLE_ID_MAX; i++ )
    {
        /**
         * - 해당 Table 의 Table Constraint Descriptor 정보를 획득
         */
        sIndexCnt  = gEldtIndexDescSCHEMA[i].mDictIndexCnt;
        sIndexDesc = gEldtIndexDescSCHEMA[i].mDictIndexDesc;
        
        for ( j = 0; j < sIndexCnt; j++ )
        {
            /**
             * - index name 생성
             */
            
            STL_TRY( eldtSetIndexName( sIndexName,
                                       STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                                       NULL,
                                       sIndexDesc[j].mTableID,
                                       sIndexDesc[j].mKeyCount,
                                       sIndexDesc[j].mColOrder,
                                       aEnv )
                     == STL_SUCCESS );

            /**
             * Index 생성을 위한 정보 생성
             * - Diable Index 로 생성한다.
             */

            /*
             * Index Attribute
             */
    
            sIndexAttr.mValidFlags     |= SML_INDEX_UNIQUENESS_MASK;
            sIndexAttr.mUniquenessFlag = STL_FALSE;
            
            sIndexAttr.mValidFlags     |= SML_INDEX_LOGGING_YES;
            sIndexAttr.mLoggingFlag    = STL_TRUE;

            sIndexAttr.mValidFlags     |= SML_INDEX_PRIMARY_MASK;
            sIndexAttr.mPrimaryFlag    = STL_FALSE;
        
            /*
             * Table Value Array 할당
             */
            
            STL_TRY( eldAllocTableValueList( sIndexDesc[j].mTableID,
                                             & sMemMark,
                                             & sTableValueList,
                                             aEnv )
                     == STL_SUCCESS );
            sState = 1;
            
            /*
             * Index Value Array 할당 
             */

            
            STL_TRY( eldShareNormalIndexValueList( & sIndexDesc[j],
                                                   sTableValueList,
                                                   & sIndexValueList,
                                                   aEnv )
                     == STL_SUCCESS );
    
            /*
             * Index Key Column Flag
             */

            sKeyFlag =
                DTL_KEYCOLUMN_INDEX_ORDER_ASC |
                DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE |
                DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST;
        
            STL_TRY( eldAllocOPMem( (void**) & sKeyColFlags,
                                    sIndexDesc->mKeyCount * STL_SIZEOF(stlUInt8),
                                    aEnv )
                     == STL_SUCCESS );

            for ( k = 0; k < sIndexDesc->mKeyCount; k++ )
            {
                sKeyColFlags[k] = sKeyFlag;
            }
    
            /**
             * Index를 생성 
             * - 레코드는 추가하지 않아야 함, Dictionary 완료후 Index에 일괄 추가
             * - 레코드를 추가하지 않는 Index 생성 interface 를 요구해야 함.
             */

        
            sPhysicalID = ELL_DICT_OBJECT_ID_NA;
            sPhysicalHandle = NULL;

            STL_TRY( smlCreateIndex( aStmt,
                                     aTablespaceID,
                                     SML_INDEX_TYPE_MEMORY_PERSISTENT_BTREE,
                                     & sIndexAttr,
                                     gEldTablePhysicalHandle[sIndexDesc->mTableID],
                                     sIndexDesc->mKeyCount,
                                     sIndexValueList,
                                     sKeyColFlags,
                                     & sPhysicalID,
                                     & sPhysicalHandle,
                                     SML_ENV(aEnv) )
                     == STL_SUCCESS );
            
            /**
             * - index descriptor 추가
             */
            
            STL_TRY( ellInsertIndexDesc( aTransID,
                                         aStmt,
                                         ELDT_AUTH_ID_SYSTEM,   
                                         ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                         & sIndexID, 
                                         aTablespaceID,
                                         sPhysicalID,
                                         sIndexName,
                                         ELL_INDEX_TYPE_BTREE,
                                         STL_FALSE, /* unique index */
                                         STL_FALSE, /* invalid */
                                         STL_FALSE, /* By Constraint */
                                         NULL,      /* index comment */
                                         aEnv )
                     == STL_SUCCESS );

            /**
             * Dictionary 구축 후 Index Data 를 구축하기 위한 정보 저장
             */
            
            gEldtIndexCnt++;
            gEldtIndexInfo[sIndexID].mIndexHandle = sPhysicalHandle;
            gEldtIndexInfo[sIndexID].mIsKeyConst = STL_FALSE;  /* 일반적인 인덱스 생성 */
            gEldtIndexInfo[sIndexID].mDescInfo = (void *) & sIndexDesc[j];

            /**
             * Index Key Table Usage descriptor 추가
             */

            STL_TRY( ellInsertIndexKeyTableUsageDesc( aTransID,
                                                      aStmt,
                                                      ELDT_AUTH_ID_SYSTEM,   
                                                      ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                                      sIndexID,
                                                      ELDT_AUTH_ID_SYSTEM,   
                                                      ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                                      sIndexDesc[j].mTableID,
                                                      aEnv )
                         == STL_SUCCESS );
            
            /**
             * Index Key Column Usage descriptor 추가
             */
            
            for ( k = 0; k < sIndexDesc[j].mKeyCount; k++ )
            {
                /* Column ID 획득 */
                STL_TRY( eldtGetDictColumnID( sIndexDesc[j].mTableID,
                                              sIndexDesc[j].mColOrder[k],
                                              & sColumnID,
                                              aEnv )
                         == STL_SUCCESS );
                
                /* index key column usage descriptor 추가 */
                STL_TRY( ellInsertIndexKeyColumnUsageDesc( aTransID,
                                                           aStmt,
                                                           ELDT_AUTH_ID_SYSTEM,   
                                                           ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                                           sIndexID,
                                                           ELDT_AUTH_ID_SYSTEM,   
                                                           ELDT_SCHEMA_ID_DEFINITION_SCHEMA,
                                                           sIndexDesc[j].mTableID,
                                                           sColumnID,
                                                           k,    /* key order */
                                               ELL_INDEX_COLUMN_ORDERING_DEFAULT,
                                               ELL_INDEX_COLUMN_NULLS_ORDERING_DEFAULT,
                                                   aEnv )
                         == STL_SUCCESS );
            }
            
            /**
             * 사용한 메모리를 해제한다.
             */

            sState = 0;
            STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief DEFINITION_SCHEMA 의 Key Constraint 이름을 설정한다.
 * @param[out] aConstNameBuffer  Constraint Name 을 설정할 buffer 공간
 * @param[in]  aBufferSize       버퍼 공간의 크기
 * @param[in]  aConstDesc        Dictionary Key Constraint descriptor
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus eldtSetKeyConstraintName( stlChar                    * aConstNameBuffer,
                                    stlInt32                     aBufferSize,
                                    eldtDefinitionKeyConstDesc * aConstDesc,
                                    ellEnv                     * aEnv )
{
    stlChar   sStringBuffer[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlInt32  sNameLen = 0;

    stlInt32  i = 0;
    eldtDefinitionColumnDesc * sColumnDesc = NULL;
    /*
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aConstNameBuffer != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBufferSize > STL_MAX_SQL_IDENTIFIER_LENGTH,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /*
     * Table Name 설정
     */
    sNameLen = stlSnprintf( sStringBuffer,
                            STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                            "%s",
                            gEldtTableDescSCHEMA[aConstDesc->mTableID]->mTableName );
    STL_ASSERT( sNameLen <= STL_MAX_SQL_IDENTIFIER_LENGTH );

    /*
     * Key Constraint 유형별 이름 설정
     */

    switch ( aConstDesc->mConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            {
                sNameLen = stlSnprintf( sStringBuffer,
                                        STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                                        "%s_PRIMARY_KEY",
                                        sStringBuffer );
                STL_ASSERT( sNameLen <= STL_MAX_SQL_IDENTIFIER_LENGTH );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            {
                sNameLen = stlSnprintf( sStringBuffer,
                                        STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                                        "%s_UNIQUE",
                                        sStringBuffer );
                STL_ASSERT( sNameLen <= STL_MAX_SQL_IDENTIFIER_LENGTH );

                for ( i = 0; i < aConstDesc->mKeyCount; i++ )
                {
                    STL_TRY( eldtGetDictColumnDescArray( aConstDesc->mTableID,
                                                         & sColumnDesc,
                                                         aEnv )
                             == STL_SUCCESS );
                    
                    sNameLen = stlSnprintf( sStringBuffer,
                                            STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                                            "%s_%s",
                                            sStringBuffer,
                                            sColumnDesc[aConstDesc->mColOrder[i]].mColumnName );
                    STL_ASSERT( sNameLen <= STL_MAX_SQL_IDENTIFIER_LENGTH );
                }

                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                sNameLen = stlSnprintf( sStringBuffer,
                                        STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                                        "%s_FOREIGN_KEY",
                                        sStringBuffer );
                STL_ASSERT( sNameLen <= STL_MAX_SQL_IDENTIFIER_LENGTH );

                sNameLen = stlSnprintf( sStringBuffer,
                                        STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                                        "%s_%s",
                                        sStringBuffer,
                                        gEldtTableDescSCHEMA[aConstDesc->mRefTableID]->mTableName );
                STL_ASSERT( sNameLen <= STL_MAX_SQL_IDENTIFIER_LENGTH );
                
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    stlStrcpy( aConstNameBuffer, sStringBuffer );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DEFINITION_SCHEMA 의 Index 이름을 설정한다.
 * @param[out] aIndexNameBuffer   Index Name 을 설정할 buffer 공간
 * @param[in]  aBufferSize        버퍼 공간의 크기
 * @param[in]  aKeyConstraintName Key Constraint Name (nullable) 
 * @param[in]  aTableID           Table ID
 * @param[in]  aKeyCount          Index Key 의 갯수
 * @param[in]  aKeyColumnArray    Index Key Column 의 Column Order Array
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus eldtSetIndexName( stlChar                    * aIndexNameBuffer,
                            stlInt32                     aBufferSize,
                            stlChar                    * aKeyConstraintName,
                            eldtDefinitionTableID        aTableID,
                            stlInt32                     aKeyCount,
                            stlInt32                   * aKeyColumnArray,
                            ellEnv                     * aEnv )
{
    stlChar   sStringBuffer[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlInt32  sNameLen = 0;

    stlInt32  i = 0;
    eldtDefinitionColumnDesc * sColumnDesc = NULL;

    /*
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aIndexNameBuffer != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBufferSize > STL_MAX_SQL_IDENTIFIER_LENGTH,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyCount > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyColumnArray != NULL, ELL_ERROR_STACK(aEnv) );

    if ( aKeyConstraintName == NULL )
    {
        /*
         * Table Name 설정
         */
        sNameLen = stlSnprintf( sStringBuffer,
                                STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                                "%s_INDEX",
                                gEldtTableDescSCHEMA[aTableID]->mTableName );
        STL_ASSERT( sNameLen <= STL_MAX_SQL_IDENTIFIER_LENGTH );
        
        /*
         * Key Column 정보를 이용한 이름 추가 
         */
        
        for ( i = 0; i < aKeyCount; i++ )
        {
            STL_TRY( eldtGetDictColumnDescArray( aTableID,
                                                 & sColumnDesc,
                                                 aEnv )
                 == STL_SUCCESS );
            
            sNameLen = stlSnprintf( sStringBuffer,
                                    STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                                    "%s_%s",
                                    sStringBuffer,
                                    sColumnDesc[aKeyColumnArray[i]].mColumnName );
            STL_ASSERT( sNameLen <= STL_MAX_SQL_IDENTIFIER_LENGTH );
        }
    }
    else
    {
        /**
         * Key Constraint Name 을 이용
         */
        sNameLen = stlSnprintf( sStringBuffer,
                                STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                                "%s_INDEX",
                                aKeyConstraintName );
        STL_ASSERT( sNameLen <= STL_MAX_SQL_IDENTIFIER_LENGTH );
    }
        
    stlStrcpy( aIndexNameBuffer, sStringBuffer );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Dictionary Table의 Column 갯수를 얻는다.
 * @param[in]  aDictTableID    Dictionary Table의 ID
 * @param[out] aColumnCnt      Dictionary Table의 Column 갯수 
 * @param[in]  aEnv            Environment
 */
stlStatus eldtGetDictTableColumnCnt( eldtDefinitionTableID      aDictTableID,
                                     stlInt32                 * aColumnCnt,
                                     ellEnv                   * aEnv )
{
    /*
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( ( (aDictTableID > ELDT_TABLE_ID_NA) &&
                          (aDictTableID < ELDT_TABLE_ID_MAX) ),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnCnt != NULL, ELL_ERROR_STACK(aEnv) );

    *aColumnCnt = gEldtColumnDescSCHEMA[aDictTableID].mDictColumnCnt;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Dictionary Table의 Column 들의 dictionary column descriptor 의 Array를 얻는다.
 * @param[in]  aDictTableID    Dictionary Table의 ID
 * @param[out] aDictColumnDesc Dictionary Table의 dictionary column descriptor Array
 * @param[in]  aEnv            Environment
 */
stlStatus eldtGetDictColumnDescArray( eldtDefinitionTableID       aDictTableID,
                                      eldtDefinitionColumnDesc ** aDictColumnDesc,
                                      ellEnv                    * aEnv )
{
    /*
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( ( (aDictTableID > ELDT_TABLE_ID_NA) &&
                          (aDictTableID < ELDT_TABLE_ID_MAX) ),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDictColumnDesc != NULL, ELL_ERROR_STACK(aEnv) );

    *aDictColumnDesc = gEldtColumnDescSCHEMA[aDictTableID].mDictColumnDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Domain 번호로 Dictionary Domain Descriptor를 얻는다.
 * @param[in]  aDomainNO       DEFINITION_SCHEMA 의 Domain 번호 
 * @return
 * Domain descriptor 
 */
eldtDefinitionDomainDesc * eldtGetDictDomainDesc( eldtDefinitionDomainNO aDomainNO )
{
    /*
     * Parameter Validation
     */
    STL_DASSERT( aDomainNO < ELDT_DOMAIN_MAX );

    return & gEldtDomainDesc[aDomainNO];
}

/**
 * @brief Dictionary Column의 Column ID 를 계산하여 얻는다.
 * @param[in]  aDictTableID     Dictionary Table 의 TABLE_ID
 * @param[in]  aDictColumnOrder Dictionary Column 의 ordinal position
 * @param[out] aCalcColumnID    해당 컬럼의 계산된 COLUMN_ID
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus eldtGetDictColumnID( eldtDefinitionTableID  aDictTableID,
                               stlInt32               aDictColumnOrder,
                               stlInt64             * aCalcColumnID,
                               ellEnv               * aEnv )
{
    stlBool     sFound = STL_FALSE;
    
    stlInt32    i = 0;
    stlInt32    j = 0;
    stlInt64    sColumnID = ELL_DICT_OBJECT_ID_NA;

    stlInt32                    sTableColumnCnt  = 0;
    eldtDefinitionColumnDesc  * sTableColumnDesc = NULL;
    
    STL_PARAM_VALIDATE( (aDictTableID > ELDT_TABLE_ID_NA) && 
                        (aDictTableID < ELDT_TABLE_ID_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCalcColumnID != NULL, ELL_ERROR_STACK(aEnv) );

    /*
     * Table 개수만큼 반복
     */
    
    sColumnID = 0;
    for ( i = ELDT_TABLE_ID_TABLES; i < ELDT_TABLE_ID_MAX; i++ )
    {
        /*
         * 해당 Table 의 Column Descriptor 정보를 획득
         */
        sTableColumnCnt  = gEldtColumnDescSCHEMA[i].mDictColumnCnt;
        sTableColumnDesc = gEldtColumnDescSCHEMA[i].mDictColumnDesc;
        
        for ( j = 0; j < sTableColumnCnt; j++ )
        {
            sColumnID++; /* COLUMN_ID 는 SCHEMA.COLUMNS 테이블에서 Unique 하게 계속 증가 */

            if ( (sTableColumnDesc[j].mTableID == aDictTableID) &&
                 (sTableColumnDesc[j].mOrdinalPosition == aDictColumnOrder) )
            {
                sFound = STL_TRUE;
                break;
            }
        }

        if ( sFound == STL_TRUE )
        {
            break;
        }
    }

    STL_ASSERT( sFound == STL_TRUE );
    
    *aCalcColumnID = sColumnID;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Dictionary Constraint 의 Key Constraint ID 를 계산하여 얻는다.
 * @param[in]  aDictTableID        Dictionary Table 의 TABLE_ID
 * @param[in]  aDictConstNum       Dictionary Constraint 의 테이블 내 ordinal position
 * @param[out] aCalcConstID        해당 제약조건의 계산된 CONSTRAINT_ID
 * @param[in]  aEnv                Environment
 * @remarks
 */
stlStatus eldtGetDictKeyConstID( eldtDefinitionTableID  aDictTableID,
                                 stlInt32               aDictConstNum,
                                 stlInt64             * aCalcConstID,
                                 ellEnv               * aEnv )
{
    stlBool     sFound = STL_FALSE;

    eldtDefinitionTableID  i = ELDT_TABLE_ID_NA;
    stlInt32               j = 0;
    
    stlInt64    sConstID = ELL_DICT_OBJECT_ID_NA;

    stlInt32                      sKeyConstCnt  = 0;
    eldtDefinitionKeyConstDesc  * sKeyConstDesc = NULL;
    
    STL_PARAM_VALIDATE( (aDictTableID > ELDT_TABLE_ID_NA) && 
                        (aDictTableID < ELDT_TABLE_ID_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCalcConstID != NULL, ELL_ERROR_STACK(aEnv) );

    /*
     * Table 개수만큼 반복
     */

    /* DEFINITION_SCHEMA 의 제약조건은 NOT NULL 이 모두 구축된 이후 KEY 제약조건이 구축된다. */
    sConstID = gEldtNotNullMaxConstID;
    
    for ( i = ELDT_TABLE_ID_TABLES; i < ELDT_TABLE_ID_MAX; i++ )
    {
        /*
         * 해당 Table 의 Column Descriptor 정보를 획득
         */
        sKeyConstCnt  = gEldtKeyConstDescSCHEMA[i].mDictConstCnt;
        sKeyConstDesc = gEldtKeyConstDescSCHEMA[i].mDictConstDesc;
        
        for ( j = 0; j < sKeyConstCnt; j++ )
        {
            sConstID++; /* COLUMN_ID 는 SCHEMA.COLUMNS 테이블에서 Unique 하게 계속 증가 */

            if ( (sKeyConstDesc[j].mTableID == aDictTableID) &&
                 (sKeyConstDesc[j].mConstNum == aDictConstNum) )
            {
                sFound = STL_TRUE;
                break;
            }
        }

        if ( sFound == STL_TRUE )
        {
            break;
        }
    }

    STL_ASSERT( sFound == STL_TRUE );
    
    *aCalcConstID = sConstID;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} eldtSCHEMA */
