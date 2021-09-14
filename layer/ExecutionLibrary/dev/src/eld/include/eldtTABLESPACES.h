/*******************************************************************************
 * eldtTABLESPACES.h
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


#ifndef _ELDT_TABLESPACES_H_
#define _ELDT_TABLESPACES_H_ 1

/**
 * @file eldtTABLESPACES.h
 * @brief TABLESPACES dictionary base table
 */

/**
 * @defgroup eldtTABLESPACES TABLESPACES table
 * @ingroup eldtNonStandard
 * @{
 */

/**
 * @brief TABLESPACES 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderTABLESPACES
{
    /*
     * 식별자 영역
     */
    
    ELDT_Space_ColumnOrder_TABLESPACE_ID,
    ELDT_Space_ColumnOrder_CREATOR_ID,

    /*
     * SQL 표준 영역
     */

    
    /*
     * 부가 정보 영역
     */

    ELDT_Space_ColumnOrder_TABLESPACE_NAME,
    ELDT_Space_ColumnOrder_MEDIA_TYPE,
    ELDT_Space_ColumnOrder_MEDIA_TYPE_ID,
    ELDT_Space_ColumnOrder_USAGE_TYPE,
    ELDT_Space_ColumnOrder_USAGE_TYPE_ID,

    ELDT_Space_ColumnOrder_IS_BUILTIN,
    
    ELDT_Space_ColumnOrder_CREATED_TIME,
    ELDT_Space_ColumnOrder_MODIFIED_TIME,
    ELDT_Space_ColumnOrder_COMMENTS,

    ELDT_Space_ColumnOrder_MAX

} eldtColumnOrderTABLESPACES;


extern eldtDefinitionTableDesc   gEldtTableDescTABLESPACES;
extern eldtDefinitionColumnDesc  gEldtColumnDescTABLESPACES[ELDT_Space_ColumnOrder_MAX];


/**
 * @brief TABLESPACES 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - 표준에 존재하지 않는 dictionary table 임. 
 * - 비표준
 *  - 비표준 1 : PRIMARY KEY (TABLESPACE_ID)
 *  - 비표준 2 : UNIQUE (TABLESPACE_NAME)
 */

typedef enum eldtKeyConstTABLESPACES
{
    /*
     * Primary Key 영역
     */

    ELDT_Space_Const_PRIMARY_KEY = 0,            /**< 비표준 1 */
    
    /*
     * Unique 영역
     */

    ELDT_Space_Const_UNIQUE_TABLESPACE_NAME,     /**< 비표준 2 */
    
    /*
     * Foreign Key 영역
     */

    ELDT_Space_Const_MAX
    
} eldtKeyConstTABLESPACES;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescTABLESPACES[ELDT_Space_Const_MAX];




/**
 * @brief TABLESPACES 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexTABLESPACES
{
    /* ELDT_Space_Index_TABLESPACE_ID, Primary Key 로 존재함 */
    ELDT_Space_Index_CREATOR_ID = 0,

    /* ELDT_Space_Index_TABLESPACE_NAME, Unique Key 로 존재함 */
    
    ELDT_Space_Index_MAX
    
} eldtIndexTABLESPACES;

extern eldtDefinitionIndexDesc  gEldtIndexDescTABLESPACES[ELDT_Space_Index_MAX];


extern stlChar * gEldtBuiltInSpaceNameString[SML_MAX_SYSTEM_TBS_ID];
extern stlChar * gEldtBuiltInSpaceCommentString[SML_MAX_SYSTEM_TBS_ID];


stlStatus eldtInsertBuiltInTablespaces( smlTransId       aTransID,
                                        smlStatement   * aStmt,
                                        smlTbsId         aSystemTbsID,
                                        smlTbsId         aDefaultUserTbsID,
                                        stlChar        * aDefaultUserTbsName,
                                        smlTbsId         aDefaultUndoTbsID,
                                        stlChar        * aDefaultUndoTbsName,
                                        smlTbsId         aDefaultTempTbsID,
                                        stlChar        * aDefaultTempTbsName,
                                        ellEnv         * aEnv );

/** @} eldtTABLESPACES */



#endif /* _ELDT_TABLESPACES_H_ */
