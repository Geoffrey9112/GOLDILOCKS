/*******************************************************************************
 * eldtROLE_AUTHORIZATION_DESCRIPTORS.h
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


#ifndef _ELDT_ROLE_AUTHORIZATION_DESCRIPTORS_H_
#define _ELDT_ROLE_AUTHORIZATION_DESCRIPTORS_H_ 1

/**
 * @file eldtROLE_AUTHORIZATION_DESCRIPTORS.h
 * @brief ROLE_AUTHORIZATION_DESCRIPTORS dictionary base table
 */

/**
 * @defgroup eldtROLE_AUTHORIZATION_DESCRIPTORS ROLE_AUTHORIZATION_DESCRIPTORS table
 * @ingroup eldtStandard
 * @{
 */

/**
 * @brief ROLE_AUTHORIZATION_DESCRIPTORS 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderROLE_AUTHORIZATION_DESCRIPTORS
{
    /*
     * 식별자 영역
     */
    
    ELDT_RoleDesc_ColumnOrder_ROLE_ID = 0,
    ELDT_RoleDesc_ColumnOrder_GRANTEE_ID,
    ELDT_RoleDesc_ColumnOrder_GRANTOR_ID,
    

    /*
     * SQL 표준 영역
     */

    /*
    ELDT_RoleDesc_ColumnOrder_ROLE_NAME,
    ELDT_RoleDesc_ColumnOrder_GRANTEE,
    ELDT_RoleDesc_ColumnOrder_GRANTOR,
    */
    
    ELDT_RoleDesc_ColumnOrder_IS_GRANTABLE,
    
    /*
     * 부가 정보 영역
     */

    ELDT_RoleDesc_ColumnOrder_MAX

} eldtColumnOrderROLE_AUTHORIZATION_DESCRIPTORS;


extern eldtDefinitionTableDesc   gEldtTableDescROLE_AUTHORIZATION_DESCRIPTORS;
extern eldtDefinitionColumnDesc  gEldtColumnDescROLE_AUTHORIZATION_DESCRIPTORS[ELDT_RoleDesc_ColumnOrder_MAX];


/**
 * @brief ROLE_AUTHORIZATION_DESCRIPTORS 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( ROLE_NAME, GRANTEE, GRANTOR )
 *   - => PRIMARY KEY ( ROLE_ID, GRANTEE_ID, GRANTOR_ID )
 * - 비표준
 */

typedef enum eldtKeyConstROLE_AUTHORIZATION_DESCRIPTORS
{
    /*
     * Primary Key 영역
     */

    ELDT_RoleDesc_Const_PRIMARY_KEY = 0,             /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    
    /*
     * Foreign Key 영역
     */

    
    ELDT_RoleDesc_Const_MAX
    
} eldtKeyConstROLE_AUTHORIZATION_DESCRIPTORS;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescROLE_AUTHORIZATION_DESCRIPTORS[ELDT_RoleDesc_Const_MAX];


/**
 * @brief ROLE_AUTHORIZATION_DESCRIPTORS 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexROLE_AUTHORIZATION_DESCRIPTORS
{
    ELDT_RoleDesc_Index_ROLE_ID = 0,
    ELDT_RoleDesc_Index_GRANTOR_ID,
    ELDT_RoleDesc_Index_GRANTEE_ID,

    ELDT_RoleDesc_Index_MAX
    
} eldtIndexROLE_AUTHORIZATION_DESCRIPTORS;

extern eldtDefinitionIndexDesc  gEldtIndexDescROLE_AUTHORIZATION_DESCRIPTORS[ELDT_RoleDesc_Index_MAX];


/** @} eldtROLE_AUTHORIZATION_DESCRIPTORS */



#endif /* _ELDT_ROLE_AUTHORIZATION_DESCRIPTORS_H_ */
