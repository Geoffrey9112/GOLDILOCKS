/*******************************************************************************
 * eldtVIEWS.c
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
 * @file eldtVIEWS.c
 * @brief DEFINITION_SCHEMA.VIEWS 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtVIEWS
 * @{
 */


/**
 * @brief DEFINITION_SCHEMA.VIEWS 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescVIEWS[ELDT_Views_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_VIEWS,                    /**< Table ID */
        "OWNER_ID",                             /**< 컬럼의 이름  */
        ELDT_Views_ColumnOrder_OWNER_ID,        /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the viewed table"        
    },
    {
        ELDT_TABLE_ID_VIEWS,                    /**< Table ID */
        "SCHEMA_ID",                            /**< 컬럼의 이름  */
        ELDT_Views_ColumnOrder_SCHEMA_ID,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "schema identifier of the viewed table"        
    },
    {
        ELDT_TABLE_ID_VIEWS,                    /**< Table ID */
        "TABLE_ID",                             /**< 컬럼의 이름  */
        ELDT_Views_ColumnOrder_TABLE_ID,        /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "table identifier of the viewed table"        
    },
    {
        ELDT_TABLE_ID_VIEWS,                    /**< Table ID */
        "VIEW_COLUMNS",                         /**< 컬럼의 이름  */
        ELDT_Views_ColumnOrder_VIEW_COLUMNS,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_SYNTAX,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "the character representation of the user-specified view column list "
    },
    {
        ELDT_TABLE_ID_VIEWS,                    /**< Table ID */
        "VIEW_DEFINITION",                      /**< 컬럼의 이름  */
        ELDT_Views_ColumnOrder_VIEW_DEFINITION, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_SYNTAX,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "the character representation of the user-specified query expression "
        "contained in the corresponding view descriptor can be represented "
        "without truncation"        
    },
    {
        ELDT_TABLE_ID_VIEWS,                    /**< Table ID */
        "CHECK_OPTION",                         /**< 컬럼의 이름  */
        ELDT_Views_ColumnOrder_CHECK_OPTION,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "The values of IS_COMPILED have following meanings (unsupported feature):\n"
        "- CASCADED: The corresponding view descriptor indicates that the view has the CHECK OPTION that is to be applied as CASCADED\n"
        "- LOCAL: The corresponding view descriptor indicates that the view has the CHECK OPTION that is to be applied as LOCAL\n"
        "- NONE: The corresponding view descriptor indicates that the view does not have the CHECK OPTION"
    },
    {
        ELDT_TABLE_ID_VIEWS,                    /**< Table ID */
        "IS_COMPILED",                          /**< 컬럼의 이름  */
        ELDT_Views_ColumnOrder_IS_COMPILED,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "The values of IS_COMPILED have following meanings:\n"
        "- TRUE : The view is compiled\n"
        "- FALSE: The view is not compiled\n"
    },
    {
        ELDT_TABLE_ID_VIEWS,                    /**< Table ID */
        "IS_AFFECTED",                          /**< 컬럼의 이름  */
        ELDT_Views_ColumnOrder_IS_AFFECTED,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "The values of IS_AFFECTED have following meanings:\n"
        "- TRUE : The view is affected by modification of underlying objects\n"
        "- FALSE: The underlying objects of the view are not modified\n"
    },
    {
        ELDT_TABLE_ID_VIEWS,                    /**< Table ID */
        "IS_UPDATABLE",                         /**< 컬럼의 이름  */
        ELDT_Views_ColumnOrder_IS_UPDATABLE,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "The values of IS_UPDATABLE have the following meanings (unsupported feature):\n"
        "- TRUE : The view is effectively updatable\n"
        "- FALSE: The view is not effectively updatable\n"
    },
    {
        ELDT_TABLE_ID_VIEWS,                    /**< Table ID */
        "IS_TRIGGER_UPDATABLE",                 /**< 컬럼의 이름  */
        ELDT_Views_ColumnOrder_IS_TRIGGER_UPDATABLE,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "The values of IS_TRIGGER_UPDATABLE have the following meanings (unsupported feature):\n"
        "- TRUE : An update INSTEAD OF trigger is defined on the view\n"
        "- FALSE: An update INSTEAD OF trigger is not defined on the view\n"
    },
    {
        ELDT_TABLE_ID_VIEWS,                    /**< Table ID */
        "IS_TRIGGER_DELETABLE",                 /**< 컬럼의 이름  */
        ELDT_Views_ColumnOrder_IS_TRIGGER_DELETABLE,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "The values of IS_TRIGGER_DELETABLE have the following meanings (unsupported feature):\n"
        "- TRUE : A delete INSTEAD OF trigger is defined on the view\n"
        "- FALSE: A delete INSTEAD OF trigger is not defined on the view\n"
    },
    {
        ELDT_TABLE_ID_VIEWS,                    /**< Table ID */
        "IS_TRIGGER_INSERTABLE_INTO",           /**< 컬럼의 이름  */
        ELDT_Views_ColumnOrder_IS_TRIGGER_INSERTABLE_INTO,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "The values of IS_TRIGGER_INSERTABLE_INTO have the following meanings (unsupported feature):\n"
        "- TRUE : An insert INSTEAD OF trigger is defined on the view\n"
        "- FALSE: An insert INSTEAD OF trigger is not defined on the view\n"
    }
};


/**
 * @brief DEFINITION_SCHEMA.VIEWS 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescVIEWS =
{
    ELDT_TABLE_ID_VIEWS,                  /**< Table ID */
    "VIEWS",                              /**< 테이블의 이름  */
    "The VIEWS table contains one row for each row in the TABLES table "
    "with a TABLE_TYPE of 'VIEW'. "
    "Each row describes the query expression that defines a view."
};


/**
 * @brief DEFINITION_SCHEMA.VIEWS 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescVIEWS[ELDT_Views_Const_MAX] =
{
    {
        ELDT_TABLE_ID_VIEWS,                         /**< Table ID */
        ELDT_Views_Const_PRIMARY_KEY,                /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        2,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Views_ColumnOrder_SCHEMA_ID,
            ELDT_Views_ColumnOrder_TABLE_ID,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};


/**
 * @brief DEFINITION_SCHEMA.VIEWS 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescVIEWS[ELDT_Views_Index_MAX] =
{
    {
        ELDT_TABLE_ID_VIEWS,                         /**< Table ID */
        ELDT_Views_Index_OWNER_ID,                   /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Views_ColumnOrder_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_VIEWS,                         /**< Table ID */
        ELDT_Views_Index_SCHEMA_ID,                  /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Views_ColumnOrder_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_VIEWS,                         /**< Table ID */
        ELDT_Views_Index_TABLE_ID,                   /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Views_ColumnOrder_TABLE_ID,
        }
    }
};


/** @} eldtVIEWS */

