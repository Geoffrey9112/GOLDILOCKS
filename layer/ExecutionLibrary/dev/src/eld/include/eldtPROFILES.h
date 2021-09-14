/*******************************************************************************
 * eldtPROFILES.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldtPROFILES.h 14729 2015-04-06 03:16:14Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ELDT_PROFILES_H_
#define _ELDT_PROFILES_H_ 1

/**
 * @file eldtPROFILES.h
 * @brief PROFILES dictionary base table
 */

/**
 * @defgroup eldtPROFILES PROFILES table
 * @ingroup eldtNonStandard
 * @{
 */

/**
 * @brief PROFILES 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderPROFILES
{
    /*
     * 식별자 영역
     */
    
    ELDT_Profile_ColumnOrder_PROFILE_ID = 0,
    ELDT_Profile_ColumnOrder_PROFILE_NAME,
    
    /*
     * 부가 정보 영역
     */

    ELDT_Profile_ColumnOrder_IS_BUILTIN, 
    
    ELDT_Profile_ColumnOrder_CREATED_TIME,
    ELDT_Profile_ColumnOrder_MODIFIED_TIME,
    ELDT_Profile_ColumnOrder_COMMENTS,

    ELDT_Profile_ColumnOrder_MAX

} eldtColumnOrderPROFILES;


extern eldtDefinitionTableDesc   gEldtTableDescPROFILES;
extern eldtDefinitionColumnDesc  gEldtColumnDescPROFILES[ELDT_Profile_ColumnOrder_MAX];


/**
 * @brief PROFILES 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY (AUTHORIZATION_NAME)
 *   - => PRIMARY KEY (AUTH_ID)
 * - 비표준
 *  - 비표준 1 : UNIQUE (AUTHORIZATION_NAME) 
 */

typedef enum eldtKeyConstPROFILES
{
    /*
     * Primary Key 영역
     */

    ELDT_Profile_Const_PRIMARY_KEY = 0,                  
    
    /*
     * Unique 영역
     */

    ELDT_Profile_Const_UNIQUE_PROFILE_NAME,        
    
    /*
     * Foreign Key 영역
     */

    ELDT_Profile_Const_MAX
    
} eldtKeyConstPROFILES;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescPROFILES[ELDT_Profile_Const_MAX];


/**
 * @brief PROFILES 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 * - 별도로 생성할 인덱스가 없음.
 */

typedef enum eldtIndexPROFILES
{
    ELDT_Profile_Index_MAX = 0
    
} eldtIndexPROFILES;




/**
 * @brief DEFINITION_SCHEMA 의 built-in PROFILE ID
 */
typedef enum eldtBuiltInProfileID
{
    ELDT_PROFILE_ID_NA = 0,             /**< Not Available */
    
    ELDT_PROFILE_ID_DEFAULT,            /**< DEFAULT profile */

    ELDT_PROFILE_ID_MAX
    
} eldtBuiltInProfileID;


#define ELDT_DEFAULT_LIMIT_FAILED_LOGIN_ATTEMPTS            (10)
#define ELDT_DEFAULT_LIMIT_FAILED_LOGIN_ATTEMPTS_STRING     ("10")
#define ELDT_DEFAULT_LIMIT_PASSWORD_LOCK_TIME              (1)
#define ELDT_DEFAULT_LIMIT_PASSWORD_LOCK_TIME_STRING       ("1")
#define ELDT_DEFAULT_LIMIT_PASSWORD_LIFE_TIME              (180)
#define ELDT_DEFAULT_LIMIT_PASSWORD_LIFE_TIME_STRING       ("180")
#define ELDT_DEFAULT_LIMIT_PASSWORD_GRACE_TIME             (7)
#define ELDT_DEFAULT_LIMIT_PASSWORD_GRACE_TIME_STRING      ("7")
#define ELDT_DEFAULT_LIMIT_PASSWORD_REUSE_MAX              (ELL_PROFILE_LIMIT_UNLIMITED)
#define ELDT_DEFAULT_LIMIT_PASSWORD_REUSE_MAX_STRING       (ELL_PROFILE_LIMIT_STRING_UNLIMITED)
#define ELDT_DEFAULT_LIMIT_PASSWORD_REUSE_TIME             (ELL_PROFILE_LIMIT_UNLIMITED)
#define ELDT_DEFAULT_LIMIT_PASSWORD_REUSE_TIME_STRING      (ELL_PROFILE_LIMIT_STRING_UNLIMITED)
#define ELDT_DEFAULT_LIMIT_PASSWORD_VERIFY_FUNCITON        (ELL_DICT_OBJECT_ID_NA)
#define ELDT_DEFAULT_LIMIT_PASSWORD_VERIFY_FUNCITON_STRING (ELL_PROFILE_LIMIT_STRING_NULL)


/**
 * @brief DEFINITION_SCHEMA의 built-in Profile Parameter
 */
typedef struct eldtBuiltInProfileParam
{
    ellProfileParam   mParamID;
    stlInt64          mLimitValue;
    stlChar         * mLimitString;
} eldtBuiltInProfileParam;
        
/**
 * @brief DEFINITION_SCHEMA의 built-in Profile Descriptor
 */
typedef struct eldtBuiltInProfileDesc
{
    eldtBuiltInProfileID        mProfileID;           /**< Profile ID */
    stlChar                   * mProfileName;         /**< Profile Name */
    stlChar                   * mProfileComment;      /**< Profile 의 주석 */

    eldtBuiltInProfileParam     mPasswordParam[ELL_PROFILE_PARAM_MAX];
} eldtBuiltInProfileDesc;


extern eldtBuiltInProfileDesc gEldtBuiltInProfileDEFAULT;

stlStatus eldtInsertBuiltInProfileDesc( smlTransId     aTransID,
                                        smlStatement * aStmt,
                                        ellEnv       * aEnv );


    
/** @} eldtPROFILES */



#endif /* _ELDT_PROFILES_H_ */
