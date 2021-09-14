/*******************************************************************************
 * eldtTABLESPACES.c
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
 * @file eldtTABLESPACES.c
 * @brief DEFINITION_SCHEMA.TABLESPACES 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtTABLESPACES
 * @{
 */

stlChar * gEldtBuiltInSpaceNameString[SML_MAX_SYSTEM_TBS_ID] =
{
    SML_MEMORY_DICT_SYSTEM_TBS_NAME,  
    SML_MEMORY_UNDO_SYSTEM_TBS_NAME,  
    SML_MEMORY_DATA_SYSTEM_TBS_NAME,  
    SML_MEMORY_TEMP_SYSTEM_TBS_NAME   
};


stlChar * gEldtBuiltInSpaceCommentString[SML_MAX_SYSTEM_TBS_ID] = 
{
    "system tablespace for dictionary management",        
    "system tablespace for default undo tablespace",      
    "system tablespace for default user data tablespace", 
    "system tablespace for default temporary tablespace"  
};


/**
 * @brief DEFINITION_SCHEMA.TABLESPACES 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescTABLESPACES[ELDT_Space_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_TABLESPACES,               /**< Table ID */
        "TABLESPACE_ID",                         /**< 컬럼의 이름  */
        ELDT_Space_ColumnOrder_TABLESPACE_ID,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,               /**< 컬럼의 Nullable 여부 */
        "tablespace identifier"        
    },
    {
        ELDT_TABLE_ID_TABLESPACES,               /**< Table ID */
        "CREATOR_ID",                            /**< 컬럼의 이름  */
        ELDT_Space_ColumnOrder_CREATOR_ID,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "authorization identifier who created the tablespace"        
    },
    {
        ELDT_TABLE_ID_TABLESPACES,               /**< Table ID */
        "TABLESPACE_NAME",                       /**< 컬럼의 이름  */
        ELDT_Space_ColumnOrder_TABLESPACE_NAME,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "tablespace name"        
    },
    {
        ELDT_TABLE_ID_TABLESPACES,               /**< Table ID */
        "MEDIA_TYPE",                            /**< 컬럼의 이름  */
        ELDT_Space_ColumnOrder_MEDIA_TYPE,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "The values of MEDIA_TYPE have the following meanings:\n"
        "- DISK : a disk tablespace (unsupported feature)\n"
        "- MEMORY : a memory tablespace\n"
        "- SSD : a solid-state disk tablespace (unsupported feature)\n"
    },
    {
        ELDT_TABLE_ID_TABLESPACES,               /**< Table ID */
        "MEDIA_TYPE_ID",                         /**< 컬럼의 이름  */
        ELDT_Space_ColumnOrder_MEDIA_TYPE_ID,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "MEDIA_TYPE identifier"
    },
    {
        ELDT_TABLE_ID_TABLESPACES,               /**< Table ID */
        "USAGE_TYPE",                            /**< 컬럼의 이름  */
        ELDT_Space_ColumnOrder_USAGE_TYPE,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "The values of USAGE_TYPE have the following meanings:\n"
        "- SYSTEM : a system data tablespace\n"
        "- DATA : a user data tablespace\n"
        "- TEMPORARY : a temporary data tablespace\n"
        "- UNDO : a undo data tablespace\n"
    },
    {
        ELDT_TABLE_ID_TABLESPACES,               /**< Table ID */
        "USAGE_TYPE_ID",                         /**< 컬럼의 이름  */
        ELDT_Space_ColumnOrder_USAGE_TYPE_ID,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "USAGE_TYPE identifier"
    },
    {
        ELDT_TABLE_ID_TABLESPACES,               /**< Table ID */
        "IS_BUILTIN",                            /**< 컬럼의 이름  */
        ELDT_Space_ColumnOrder_IS_BUILTIN,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "is built-in object or not"        
    },
    {
        ELDT_TABLE_ID_TABLESPACES,               /**< Table ID */
        "CREATED_TIME",                          /**< 컬럼의 이름  */
        ELDT_Space_ColumnOrder_CREATED_TIME,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "created time of the tablespace"        
    },
    {
        ELDT_TABLE_ID_TABLESPACES,               /**< Table ID */
        "MODIFIED_TIME",                         /**< 컬럼의 이름  */
        ELDT_Space_ColumnOrder_MODIFIED_TIME,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "last modified time of the tablespace"        
    },
    {
        ELDT_TABLE_ID_TABLESPACES,               /**< Table ID */
        "COMMENTS",                              /**< 컬럼의 이름  */
        ELDT_Space_ColumnOrder_COMMENTS,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "comments of the tablespace"        
    }
};


/**
 * @brief DEFINITION_SCHEMA.TABLESPACES 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescTABLESPACES =
{
    ELDT_TABLE_ID_TABLESPACES,                  /**< Table ID */
    "TABLESPACES",                              /**< 테이블의 이름  */
    "The TABLESPACES table contains one row for each tablespace. "
    "This table is not declared in SQL standard."
};


/**
 * @brief DEFINITION_SCHEMA.TABLESPACES 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescTABLESPACES[ELDT_Space_Const_MAX] =
{
    {
        ELDT_TABLE_ID_TABLESPACES,                   /**< Table ID */
        ELDT_Space_Const_PRIMARY_KEY,                /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Space_ColumnOrder_TABLESPACE_ID,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_TABLESPACES,                   /**< Table ID */
        ELDT_Space_Const_UNIQUE_TABLESPACE_NAME,     /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Space_ColumnOrder_TABLESPACE_NAME,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};



/**
 * @brief DEFINITION_SCHEMA.TABLESPACES 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescTABLESPACES[ELDT_Space_Index_MAX] =
{
    {
        ELDT_TABLE_ID_TABLESPACES,                   /**< Table ID */
        ELDT_Space_Index_CREATOR_ID,                 /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Space_ColumnOrder_CREATOR_ID,
        }
    }
};







/**
 * @brief DEFINITION_SCHEMA의 built-in tablespace descriptor 를 추가한다.
 * @param[in] aTransID             Transaction ID
 * @param[in] aStmt                Statement
 * @param[in] aSystemTbsID         System Tablespace ID
 * @param[in] aDefaultUserTbsID    Default User Tablespace ID
 * @param[in] aDefaultUserTbsName  Default User Tablespace 의 이름 (nullable)
 * @param[in] aDefaultUndoTbsID    Default Undo Tablespace ID
 * @param[in] aDefaultUndoTbsName  Default Undo Tablespace 의 이름 (nullable)
 * @param[in] aDefaultTempTbsID    Default Temp Tablespace ID
 * @param[in] aDefaultTempTbsName  Default Temp Tablespace 의 이름 (nullable)
 * @param[in] aEnv                 Envirionment
 * @remarks
 */
stlStatus eldtInsertBuiltInTablespaces( smlTransId       aTransID,
                                        smlStatement   * aStmt,
                                        smlTbsId         aSystemTbsID,
                                        smlTbsId         aDefaultUserTbsID,
                                        stlChar        * aDefaultUserTbsName,
                                        smlTbsId         aDefaultUndoTbsID,
                                        stlChar        * aDefaultUndoTbsName,
                                        smlTbsId         aDefaultTempTbsID,
                                        stlChar        * aDefaultTempTbsName,
                                        ellEnv         * aEnv )
{
    stlChar * sUserTbsName = NULL;
    stlChar * sUndoTbsName = NULL;
    stlChar * sTempTbsName = NULL;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt         != NULL, ELL_ERROR_STACK(aEnv) );

    if ( aDefaultUserTbsName == NULL )
    {
        sUserTbsName = gEldtBuiltInSpaceNameString[SML_MEMORY_DATA_SYSTEM_TBS_ID];
    }
    else
    {
        sUserTbsName = aDefaultUserTbsName;
    }

    if ( aDefaultUndoTbsName == NULL )
    {
        sUndoTbsName = gEldtBuiltInSpaceNameString[SML_MEMORY_UNDO_SYSTEM_TBS_ID];
    }
    else
    {
        sUndoTbsName = aDefaultUndoTbsName;
    }
    
    if ( aDefaultTempTbsName == NULL )
    {
        sTempTbsName = gEldtBuiltInSpaceNameString[SML_MEMORY_TEMP_SYSTEM_TBS_ID];
    }
    else
    {
        sTempTbsName = aDefaultTempTbsName;
    }

    /**
     * DICTIONARY tablespace 정보 추가
     */
    
    STL_TRY( ellInsertTablespaceDesc(
                 aTransID,
                 aStmt,
                 aSystemTbsID,
                 ELDT_AUTH_ID_SYSTEM,
                 gEldtBuiltInSpaceNameString[SML_MEMORY_DICT_SYSTEM_TBS_ID],
                 ELL_SPACE_MEDIA_TYPE_MEMORY,
                 ELL_SPACE_USAGE_TYPE_DICT,
                 gEldtBuiltInSpaceCommentString[SML_MEMORY_DICT_SYSTEM_TBS_ID],
                 aEnv )
             == STL_SUCCESS );
    
    /**
     * UNDO tablespace 정보 추가
     */

    STL_TRY( ellInsertTablespaceDesc(
                 aTransID,
                 aStmt,
                 aDefaultUndoTbsID,
                 ELDT_AUTH_ID_SYSTEM,
                 sUndoTbsName,
                 ELL_SPACE_MEDIA_TYPE_MEMORY,
                 ELL_SPACE_USAGE_TYPE_UNDO,
                 gEldtBuiltInSpaceCommentString[SML_MEMORY_UNDO_SYSTEM_TBS_ID],
                 aEnv )
             == STL_SUCCESS );

    /**
     * USER DATA tablespace 정보 추가
     */

    STL_TRY( ellInsertTablespaceDesc(
                 aTransID,
                 aStmt,
                 aDefaultUserTbsID,
                 ELDT_AUTH_ID_SYSTEM,
                 sUserTbsName,
                 ELL_SPACE_MEDIA_TYPE_MEMORY,
                 ELL_SPACE_USAGE_TYPE_DATA,
                 gEldtBuiltInSpaceCommentString[SML_MEMORY_DATA_SYSTEM_TBS_ID],
                 aEnv )
             == STL_SUCCESS );

    
    /**
     * TEMP tablespace 정보 추가
     */

    STL_TRY( ellInsertTablespaceDesc(
                 aTransID,
                 aStmt,
                 aDefaultTempTbsID,
                 ELDT_AUTH_ID_SYSTEM,
                 sTempTbsName,
                 ELL_SPACE_MEDIA_TYPE_MEMORY,
                 ELL_SPACE_USAGE_TYPE_TEMPORARY,
                 gEldtBuiltInSpaceCommentString[SML_MEMORY_TEMP_SYSTEM_TBS_ID],
                 aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} eldtTABLESPACES */

