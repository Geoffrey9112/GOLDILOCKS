/*******************************************************************************
 * knpDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *        Property 관련 Definition
 *
 ******************************************************************************/

#ifndef _KNP_DEF_H_
#define _KNP_DEF_H_ 1

/**
 * @file knpDef.h
 * @brief Kernel Layer 의 Property 관리를 위한 내부 자료 구조 정의
 */


/**
 * @defgroup KNP Property
 * @ingroup knInternal
 * @internal
 * @{
 */

/** @} */

/**
 * @defgroup KNPDEF Property 내부 자료 구조
 * @ingroup KNP
 * @internal
 * @{
 */

/**
 * @brief 프로퍼티 이름 및 값의 최대 길이 : 8byte align을 위해서 8의 배수가 되어야 함
 */

#define KNP_PROPERTY_MAX_SIZE   (256)

/**
 * @brief 프로퍼티 PREFIX
 */

#define KNP_PROPERTY_PREFIX    "GOLDILOCKS_"

/**
 * @brief 프로퍼티 Default File 및 경로
 */

#define KNP_DEFAULT_PROPERTY_FILE   "conf"STL_PATH_SEPARATOR"goldilocks.properties.conf"

/**
 * @brief 프로퍼티 Default File 및 경로
 */

#define KNP_DEFAULT_BINARY_PROPERTY_FILE   "conf"STL_PATH_SEPARATOR"goldilocks.properties.binary"

/**
 * @brief 프로퍼티 VARCHAR에서 사용되는 HOME DIRECTORY STRING
 */

#define KNP_HOME_DIRECTORY           "<"KNP_PROPERTY_PREFIX"DATA>"

/**
 * @brief 프로퍼티 단위 변환
 */

#define KNP_KB_PER_MB                (1024)
#define KNP_KB_PER_GB                (1024 * 1024)
#define KNP_KB_PER_TB                (1024 * 1024 * 1024)
#define KNP_KB_PER_PB                (1024 * 1024 * 1024 * 1024)

#define KNP_MS_PER_S                 (1000)
#define KNP_S_PER_MIN                (60)
#define KNP_MS_PER_MIN               (1000 * 60)
#define KNP_MIN_PER_H                (60)
#define KNP_S_PER_H                  (60 * 60)
#define KNP_MS_PER_H                 (1000 * 60 * 60)
#define KNP_MIN_PER_D                (60 * 24)
#define KNP_S_PER_D                  (60 * 60 * 24)
#define KNP_MS_PER_D                 (1000 * 60 * 60 * 24)


/**
 * @brief 프로퍼티 속성 리스트
 */

/**
 * @brief Property Unit for Size & Time
 */
#define KNP_ATTR_UNIT_TIME_SIZE_MASK      0x00000007  /* mask for size */
#define KNP_ATTR_UNIT_NONE                0x00000000  /* none-value */
#define KNP_ATTR_UNIT_SIZE                0x00000001  /* value is in Bytes */
#define KNP_ATTR_UNIT_TIME                0x00000002  /* value is in Milliseconds */

/**
 * @brief Property SYSTEM Modificable : Alter System/Alter Database
 */ 
#define KNP_ATTR_MD_SYS_MASK              0x0F000000
#define KNP_ATTR_MD_SYS_DEFERRED          0x00000000   /* Default */
#define KNP_ATTR_MD_SYS_IMMEDIATE         0x01000000
#define KNP_ATTR_MD_SYS_FALSE             0x03000000
#define KNP_ATTR_MD_SYS_NONE              0x07000000

#define KNP_ATTR_MD_DB_MASK               0xC0000000
#define KNP_ATTR_MD_DB_DEFFERED           0x00000000    /* Default */
#define KNP_ATTR_MD_DB_IMMEDIATE          0x40000000
#define KNP_ATTR_MD_DB_FALSE              0x80000000

/**
 * @brief Property SESION Modificable : Alter Session
 */ 
#define KNP_ATTR_MD_SES_MASK              0x00001000
#define KNP_ATTR_MD_SES_FALSE             0x00000000    /* Default */
#define KNP_ATTR_MD_SES_TRUE              0x00001000

/**
 * @brief Property Modifiable
 */ 
#define KNP_ATTR_MD_MASK                  0x00002000
#define KNP_ATTR_MD_WRITABLE              0x00000000    /* Default */
#define KNP_ATTR_MD_READ_ONLY             0x00002000    

/**
 * @brief Property Domain
 */

#define KNP_ATTR_DOMAIN_MASK              0x0000C000
#define KNP_ATTR_DOMAIN_EXTERNAL          0x00000000    /* Default */
#define KNP_ATTR_DOMAIN_INTERNAL          0x00004000
#define KNP_ATTR_DOMAIN_HIDDEN            0x00008000

/**
 * @brief Property Range-Check 
 * @remarks
 * <BR> Property Type이 숫자형일때만 체크한다.
 */

#define KNP_ATTR_RANGE_MASK               0x00010000
#define KNP_ATTR_RANGE_NOCHECK            0x00000000    /* Default */
#define KNP_ATTR_RANGE_CHECK              0x00010000

/**
 * @brief Property Increase(property값이 증가만 해야 하는 속성)
 * @remarks
 * <BR> Property Type이 숫자형일때만 체크한다.
 */

#define KNP_ATTR_INCREASE_MASK            0x00020000
#define KNP_ATTR_INCREASE_NOCHECK         0x00000000    /* Default */
#define KNP_ATTR_INCREASE_CHECK           0x00020000


/**
 * @brief Property For Cluster
 * @remarks
 * <BR> 사용하고 있지 않음
 */
#define KNP_ATTR_INSTANCE_UNIQUE_MASK     0x00100000
#define KNP_ATTR_INSTANCE_UNIQUE_ANY      0x00000000    /* Default */
#define KNP_ATTR_INSTANCE_UNIQUE_MUST     0x00100000

#define KNP_ATTR_INSTANCE_SHARED_MASK     0x00200000
#define KNP_ATTR_INSTANCE_SHARED_ANY      0x00000000    /* Default */
#define KNP_ATTR_INSTANCE_SHARED_MUST     0x00200000

/**
 * @brief Property Source
 * @remarks
 * <BR> Property의 값을 어디에서 얻어왔는지를 구분
 */

typedef enum
{
    KNP_SOURCE_DEFAULT        = 0,      /**< 개발자가 입력한 Default */
    KNP_SOURCE_ENV            = 1,      /**< 시스템 환경변수 */
    KNP_SOURCE_BINARYFILE     = 2,      /**< Binary File  */
    KNP_SOURCE_TEXTFILE       = 3,      /**< Text File */
    KNP_SOURCE_SYSTEM         = 4,      /**< 시스템 관리자 */
    KNP_SOURCE_USER           = 5,      /**< 사용자 */
    KNP_SOURCE_MAX            = 6       /**< Invalid */
}knpPropertySource;


/**
 * @brief Property 변경 가능 Startup Phase 범위
 * @remarks
 * <BR> 
 */

typedef enum
{
    KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW     = 0,      /**< 해당 StartupPhase의 이하에서 변경 가능 */
    KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER     = 1,      /**< 해당 StartupPhase의 이상에서 변경 가능 */
    KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY      = 2,      /**< 해당 StartupPhase에서만 변경 가능 */
    KNP_PROPERTY_PHASE_DOMAIN_MAX               = 3       /**< Invalid */
}knpPropertyStartupPhaseDomain;


/**
 * @brief PropertyGroup
 * @todo  Property Group 리스트를 정리해야 함
 * @remarks
 * <BR> Property 그룹을 나열한다.
 */

typedef enum
{
    Group1   = 0,
    Group2   = 1,
    Group3   = 2,
    GroupMax   
}knpPropertyGroup;


/**
 * @brief Property를 등록하기 위한 정보
 * @remarks
 * <BR> 등록을 위해서만 사용됨
 */

typedef struct knpPropertyDesc
{
    stlChar      * mName;                       /**< Property 이름 */
    stlChar      * mDesc;                       /**< Property 설명 */
    stlChar      * mValue;                      /**< Property Default 값 */
    stlUInt32      mFlag;                       /**< Property 속성 */
    stlUInt16      mID;                         /**< Property ID - knlPropertyID*/
    stlUInt16      mGroup;                      /**< Property가 속한 그룹 */
    stlUInt32      mDataType;                   /**< Property에 저장될 값의 데이터타입 */
    stlUInt16      mStartupPhase;               /**< Property를 설정할 수 있는 Phase */
    stlUInt16      mStartupPhaseDomain;         /**< Property를 설정할 수 있는 Phase 범위 */
    stlInt64       mMin;                        /**< Property가 최소값 또는 최소 길이 */
    stlInt64       mMax;                        /**< Property가 최대값 또는 최대 길이 */
}knpPropertyDesc;


/**
 * @brief Property 정보를 저장
 * @remarks
 * <BR> 각각의 Property의 정보를 관리하기 위해 사용됨
 */

typedef struct knpPropertyContent
{
    stlRingEntry   mLink;                                 /**< Link 정보 */
    stlChar        mName[KNL_PROPERTY_NAME_MAX_LENGTH];   /**< Property 이름 */
    stlChar        mDesc[KNL_PROPERTY_STRING_MAX_LENGTH]; /**< Property 설명 */
    stlUInt32      mFlag;                                 /**< Property 속성 */
    stlUInt16      mID;                                   /**< Property ID - knlPropertyID*/
    stlUInt16      mGroup;                                /**< Property가 속한 그룹  : knpPropertyGroup */
    stlUInt32      mDataType;                             /**< Property에 저장될 값의 데이터타입 : knlPropertyDataType */
    stlUInt16      mStartupPhase;                         /**< Property를 설정할 수 있는 Phase : knlStartupPhase */
    stlUInt16      mStartupPhaseDomain;                   /**< Property를 설정할 수 있는 Phase Domain : knpPropertyStartupPhaseDomain */
    stlUInt32      mSource;                               /**< Property 값을 얻어온 소스 : knpPropertySource */
    stlUInt32      mInitSource;                           /**< Property 값을 얻어온 소스 : knpPropertySource */
    
    void         * mValue;                                /**< Property 값  */
    void         * mInitValue;                            /**< Reset Value 값 */
    stlUInt32      mValueLength;                          /**< Property 값의 길이 */
    stlUInt32      mInitValueLength;                      /**< Reset Value 값의 길이 */
    
    stlBool        mHasFileValue;                         /**< ALTER SYSTEM .. scope=FILE로 지정한 값이 있는지 여부 */
    stlChar        mPadding[3];
    stlUInt32      mFileValueLength;                      /**< Alter System .. scope=FILE로 적용한 값의 길이 */
    void         * mFileValue;                            /**< Alter System .. scope=FILE로 적용한 값 */
    
    stlInt64       mMin;                                  /**< Property가 최소값 또는 최소 길이 */
    stlInt64       mMax;                                  /**< Property가 최대값 또는 최대 길이 */
}knpPropertyContent;


/**
 * @brief Property 관리를 위한 정보
 * @remarks
 * <BR> Property의 Header 정보로 사용됨
 */

typedef struct knpPropertyMgr
{
    stlChar               mHomeDir[KNL_PROPERTY_STRING_MAX_LENGTH];          /**< HOME Directory */
    stlChar               mSID[KNL_PROPERTY_STRING_MAX_LENGTH];              /**< SID */
    stlChar               mPropertyFile[KNL_PROPERTY_STRING_MAX_LENGTH];     /**< Property File 경로 및 이름 */
    stlUInt32             mPropertyCnt;                                      /**< 등록된 Property의 개수 */
    stlUInt32             mPadding;                                  
    stlUInt64             mPropertySize;                                     /**< 전체 Property Size : Shared Memory 할당을 위해서 저장 */
    knlStaticHash       * mPropertyHash;                                     /**< Hash */
    knlRegionMem          mHeapMem;                                          /**< 임시적으로 사용되는 Static Hash Memory */
    knpPropertyContent  * mPropertyAddr[KNP_PROPERTY_MAX_SIZE];              /**< 각 프로퍼티의 시작주소 */
    stlBool               mDoCopyToSessionEnv[KNP_PROPERTY_MAX_SIZE];        /**< Session 정보에 복사할지 여부  */
    stlUInt64             mVarcharTotalLen;                                  /**< String Type의 모든 Property의 길이의 합 */
    knlLatch              mLatch;                                            /**< String Type을 변경하기 위한 Latch */
}knpPropertyMgr;


/**
 * @brief Binary Property 파일의 Header 정보
 * @remarks
 * <BR> BINARY File에서 Header 정보로 사용됨
 */

typedef struct knpPropertyFileHeader
{
    stlUInt64    mFileSize;             /**< Property File의 Size */
    stlUInt64    mChecksum;             /**< Property File의 CheckSum : 유효성 판단 (to do) */
    stlUInt32    mPropertyCount;        /**< Property File에 포함된 Property 개수 */
    stlInt32     mPadding;
}knpPropertyFileHeader;

/**
 * @brief Read-Only 파일의 Property 정보
 * @remarks
 * <BR> READ_ONLY BINARY File에서 Property 정보로 사용됨
 */

typedef struct knpPropertyFileBody
{
    stlUInt32      mNameLength;
    stlUInt32      mValueLength;
    stlInt32       mDataType;
    stlInt32       mPadding; 
}knpPropertyFileBody;


/**
 * @brief Global Variable 
 */

extern knpPropertyMgr    * gKnpPropertyMgr;
extern knpPropertyDesc     gPropertyContentList[];
extern const stlChar     * const gPropertyGroupList[];

/** @} */

#endif /* _KNP_DEF_H_ */
