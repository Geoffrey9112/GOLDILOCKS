/*******************************************************************************
 * eldtAUTHORIZATIONS.h
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


#ifndef _ELDT_AUTHORIZATIONS_H_
#define _ELDT_AUTHORIZATIONS_H_ 1

/**
 * @file eldtAUTHORIZATIONS.h
 * @brief AUTHORIZATIONS dictionary base table
 */

/**
 * @defgroup eldtAUTHORIZATIONS AUTHORIZATIONS table
 * @ingroup eldtStandard
 * @{
 */

/**
 * @brief SYS 사용자에 대한 default password
 */
#define ELDT_DEFAULT_SYS_PASSWORD   ( "gliese" )


/**
 * @brief AUTHORIZATIONS 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderAUTHORIZATIONS
{
    /*
     * 식별자 영역
     */
    
    ELDT_Auth_ColumnOrder_AUTH_ID = 0,

    /*
     * SQL 표준 영역
     */

    ELDT_Auth_ColumnOrder_AUTHORIZATION_NAME,
    ELDT_Auth_ColumnOrder_AUTHORIZATION_TYPE,
    ELDT_Auth_ColumnOrder_AUTHORIZATION_TYPE_ID,   /* 비표준 */
    
    /*
     * 부가 정보 영역
     */

    ELDT_Auth_ColumnOrder_IS_BUILTIN, 
    
    ELDT_Auth_ColumnOrder_CREATED_TIME,
    ELDT_Auth_ColumnOrder_MODIFIED_TIME,
    ELDT_Auth_ColumnOrder_COMMENTS,

    ELDT_Auth_ColumnOrder_MAX

} eldtColumnOrderAUTHORIZATIONS;


extern eldtDefinitionTableDesc   gEldtTableDescAUTHORIZATIONS;
extern eldtDefinitionColumnDesc  gEldtColumnDescAUTHORIZATIONS[ELDT_Auth_ColumnOrder_MAX];


/**
 * @brief AUTHORIZATIONS 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY (AUTHORIZATION_NAME)
 *   - => PRIMARY KEY (AUTH_ID)
 * - 비표준
 *  - 비표준 1 : UNIQUE (AUTHORIZATION_NAME) 
 */

typedef enum eldtKeyConstAUTHORIZATIONS
{
    /*
     * Primary Key 영역
     */

    ELDT_Auth_Const_PRIMARY_KEY = 0,                  /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    ELDT_Auth_Const_UNIQUE_AUTHORIZATION_NAME,        /**< 비표준 1 */
    
    /*
     * Foreign Key 영역
     */

    
    ELDT_Auth_Const_MAX
    
} eldtKeyConstAUTHORIZATIONS;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescAUTHORIZATIONS[ELDT_Auth_Const_MAX];


/**
 * @brief AUTHORIZATIONS 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 * - 별도로 생성할 인덱스가 없음.
 */

typedef enum eldtIndexAUTHORIZATIONS
{
    ELDT_Auth_Index_MAX = 0
    
} eldtIndexAUTHORIZATIONS;




/**
 * @brief DEFINITION_SCHEMA 의 built-in AUTHORIZATION ID
 */
typedef enum eldtBuiltInAuthID
{
    ELDT_AUTH_ID_NOT_AVAILABLE = 0,  /**< Not Available */
    
    ELDT_AUTH_ID_SYSTEM,             /**< _SYSTEM special auth identifier */
    ELDT_AUTH_ID_SYS,                /**< SYS user */
    ELDT_AUTH_ID_ADMIN,              /**< ADMIN role */
    ELDT_AUTH_ID_SYSDBA,             /**< SYSDBA role */
    ELDT_AUTH_ID_PUBLIC,             /**< PUBLIC special auth identifier */

    ELDT_AUTH_ID_TEST,               /**< TEST user */

    ELDT_AUTH_ID_MAX
    
} eldtBuiltInAuthID;

extern stlChar * gEldtAuthString[ELDT_AUTH_ID_MAX];

/**
 * @brief 삭제할 수 없는 Built-In Authorization
 * - test built-in user 는 삭제할 수 있어야 함.
 */
#define ELDT_AUTH_BUILT_IN_ID_MAX (ELDT_AUTH_ID_TEST)

/**
 * @brief Privilege 를 변경할 수 없는 Built-In Authorization
 * - SYS 등의 privilege 를 변경할 수 없어야 함.
 */
#define ELDT_AUTH_NOT_UPDATABLE_PRIVILEGE_BUILT_IN_ID_MAX (ELDT_AUTH_ID_PUBLIC)

/**
 * @brief DEFINITION_SCHEMA의 built-in Authorization Descriptor
 */
typedef struct eldtBuiltInAuthDesc
{
    eldtBuiltInAuthID        mAuthID;           /**< Authorization ID */
    ellAuthorizationType     mAuthType;         /**< Authorization 유형  */
    stlChar                * mAuthComment;      /**< Authorization 의 주석 */
} eldtBuiltInAuthDesc;

extern eldtBuiltInAuthDesc gEldtBuiltInAuthSYS;
extern eldtBuiltInAuthDesc gEldtBuiltInAuthADMIN;
extern eldtBuiltInAuthDesc gEldtBuiltInAuthSYSDBA;
extern eldtBuiltInAuthDesc gEldtBuiltInAuthPUBLIC;

stlStatus eldtInsertBuiltInAuthDesc( smlTransId     aTransID,
                                     smlStatement * aStmt,
                                     stlChar      * aSysPassword,
                                     smlTbsId       aSystemTbsID,
                                     smlTbsId       aUserTablespaceID,
                                     smlTbsId       aTempTablespaceID,
                                     ellEnv       * aEnv );

stlStatus eldtGetDefaultSysPassword( stlChar * aPassword,
                                     ellEnv  * aEnv );

    
/** @} eldtAUTHORIZATIONS */



#endif /* _ELDT_AUTHORIZATIONS_H_ */
