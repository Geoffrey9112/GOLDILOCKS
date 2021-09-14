/*******************************************************************************
 * eldcDef.h
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


#ifndef _ELDC_DEF_H_
#define _ELDC_DEF_H_ 1

/**
 * @file eldcDef.h
 * @brief Execution Library Dictionary Cache Definition
 */


/**
 * @defgroup eldc Dictionary Cache
 * @ingroup eld
 * @brief
 * DEFINITION SCHEMA로부터 구성되는 Dictionary Cache 정의 
 * @{
 */

/**
 * @brief Cache 를 dump 하기 위한 string buffer 크기
 */
#define ELDC_DUMP_PRINT_BUFFER_SIZE             ( 1024 )
#define ELDC_DUMP_PRINT_NULL_TERMINATE_POSITION ( ELDC_DUMP_PRINT_BUFFER_SIZE - 6)
#define ELDC_DUMP_PRINT_NULL_TERMINATE_STRING   ( "... )\n" )

/**
 * @brief Object Handle 을 얻기 위한 Callback 함수
 */
typedef stlStatus (* eldcGetObjectHandleByID) ( smlTransId           aTransID,
                                                smlScn               aViewSCN,
                                                stlInt64             aIndexID,
                                                ellDictHandle      * aIndexDictHandle,
                                                stlBool            * aExist,
                                                ellEnv             * aEnv );

/**
 * @brief Hash Data 출력을 위한 Callback 함수
 */
typedef void (* eldcPrintHashData) ( void    * aHashData,
                                     stlChar * aStringBuffer );

/**
 * @brief Object Descriptor 출력을 위한 Callback 함수
 */
typedef void (* eldcPrintObjectDesc) ( void    * aObjectDesc,
                                       stlChar * aStringBuffer );

/**
 * @defgroup eldcDictHash Dictionary Cache 를 위한 전용 Hash
 * @brief
 *   Dictionary Cache 의 Non-Blocking MVCC 를 위해
 *   Kernel layer의 범용 hash 가 아닌 전용 Hash 를 사용한다.
 * @ingroup eldc
 * @{
 * @remarks
 * 
 * <PRE> =========================================== </PRE>
 * <PRE> Dictionary Cache 전용 Hash 의 구조 </PRE>
 * <PRE> =========================================== </PRE>
 *
 * <PRE> ===== ID Cache 의 구조 ===== </PRE>
 * <PRE> ID Cache 는 객체의 Descriptor 정보를 포함하고 있다.   </PRE>
 * 
 * <PRE> Hash Bucket Array                                              </PRE>
 * <PRE> []--------------->[Hash Element]------->[Hash Element]------>  </PRE>
 * <PRE> []                      |    |                                 </PRE>
 * <PRE> []                      |    + [Hash Data]                     </PRE>
 * <PRE> []                      V                                      </PRE>
 * <PRE> []                [Object Descriptor]                          </PRE>
 * 
 * <PRE> ===== Name Cache 의 구조 ===== </PRE>
 * <PRE> Name Cache 는 ID Cache의 Hash Element 정보를 가지고 있다.   </PRE>
 * <PRE> 변경이 발생하지 않기 때문에 Object Descriptor 를 갖지 않는다. </PRE>
 * 
 * <PRE> Hash Bucket Array                                              </PRE>
 * <PRE> []------------>[Hash Element]------->[Hash Element]------>     </PRE>
 * <PRE> []                      |                                      </PRE>
 * <PRE> []                      + [Hash Data: ID Cache의 Hash Element] </PRE>
 */


    
/**
 * @brief Related Object를 수정하고 있는 트랜잭션들의 리스트
 */
typedef struct eldcRelatedTransList
{
    smlTransId                    mTransId;    /**< Transaction Identifier */
    stlInt64                      mRefCount;   /**< Transaction Reference Count */
    struct eldcRelatedTransList * mNext;       /**< Next Transaction */
} eldcRelatedTransList;

/**
 * @brief Hash Element
 * @remarks
 * Aging Scheme 에 의한 Garbage Collection 을 위해
 * 고정 길이 크기의 공유 영역의 Dynamic Memory 로 관리되어야 한다.
 */
typedef struct eldcHashElement
{
    ellVersionInfo           mVersionInfo;      /**< Cache Version 정보 */
    
    knlLatch                 mWriteLatch;       /**< 내부구조 변경이 필요할 경우 사용하는 Latch, 검색시에는 사용안함 */
    stlUInt32                mHashValue;        /**< Hash Value */
    void                   * mHashData;         /**< Hash Key 정보 및 부가정보를 포함하고 있는 Data */
    eldcRelatedTransList   * mRelatedTransList; /**< Related Object를 변경하고 있는 Transaction List */
    void                   * mObjectDesc;       /**< Object Descriptor */
    struct eldcHashElement * mNext;             /**< Next Hash Element */
} eldcHashElement;

/**
 * @brief Object 와 Relationship을 갖는 Object
 * @remarks
 * - 필요한 경우, Hash Element의 Hash Data 의 멤버로 구성된다.
 * - Table 의 경우, Index, Constraint, 자신을 참조하는 Constraint 가 대상이 된다.
 * - Related Object 의 항상 최신의 유효한 정보를 사용할 수 있도록 한다.
 * - Dictionary Handle 형태로 관리하지 않는 이유
 *  - 종료되지 않은 트랜잭션에서 Index 등을 제거한 경우
 *  - 해당 Transaction 이 Table 에서 참조할 Index 가 아님을 판단할 수 없다.
 */
typedef struct eldcHashRelatedObjList
{
    ellVersionInfo   mVersionInfo;    /**< Cache Version 정보 */
    ellDictHandle    mRelatedHandle;  /**< Related Object 의 Dictionary Handle */
    struct eldcHashRelatedObjList * mNext;   
} eldcHashRelatedObjList;

/**
 * @brief Hash Bucket
 */
typedef struct eldcHashBucket
{
    knlLatch          mInsDelLatch;   /**<
                                       * Hash Element를 추가/삭제만을 위한 Latch,
                                       * 검색시에는 사용안함
                                       */
    eldcHashElement * mElement;       /**< Hash Element 의 List */
} eldcHashBucket;
    
/**
 * @brief Dictionary 전용 Hash
 */
struct eldcDictHash
{
    stlUInt32           mBucketCnt;        /**< Bucket Count */
    eldcHashBucket      mBucket[1];        /**< Hash Bucket Array */
};

/**
 * @brief Dictionary Cache 를 위한 Dump Object Handle
 * @remarks
 * 함수 포인터를 공유 메모리상에 유지할 수 없으므로, 별도의 Process 자료 구조를 통해 관리한다.
 */
typedef struct eldcDictDump
{
    eldcDictHash      * mDictHash;         /**< 공유 메모리상의 Dictionary Hash */
    eldcPrintHashData   mPrintHashData;    /**< Hash Data 출력을 위한 Callback 함수 */
    eldcPrintObjectDesc mPrintObjectDesc;  /**< Object Descriptor 출력을 위한 Callback 함수 */
} eldcDictDump;


/**
 * @brief Dictionary Hash 의 Hash Function Pointer
 * @remarks
 * - param[in] aHashKey  임의의 Hash Key 자료 구조 
 * - 반환값     Hash Value
 */
typedef stlUInt32 (* eldcHashFunc) ( void * aHashKey );

/**
 * @brief Dictionary Hash 의 Key Compare Function Pointer
 * @remarks
 * - param[in] aHashKey  임의의 Hash Key 자료 구조 
 * - 반환값     TRUE/FALSE
 */
typedef stlBool (* eldcKeyCompareFunc) ( void * aHashKey, eldcHashElement * aHashElement );


/** @} eldcDictHash */










/**
 * @defgroup eldcObject SQL-Object Cache
 * @ingroup eldc
 * @{
 * @remarks SQL-Object 를 위한 Cache
 * <BR> Name Cache 와 ID Cache 로 구성된다.
 * <BR> - Name Cache : Name으로부터 Object ID 를 얻기 위한 Cache
 * <BR> - ID Cache : Object ID 로부터 Object Descriptor 를 얻기 위한 Cache
 */


/**
 * @brief X$TABLES 는 0 번째 fixed table로 고정되어 있음
 */
#define ELDC_XTABLES_FXTABLE_NO  (0)

/**
 * @brief X$COLUMNS 는 1 번째 fixed table로 고정되어 있음
 */
#define ELDC_XCOLUMNS_FXTABLE_NO (1)


/**
 * @brief Object Cache 종류
 */

typedef enum eldcObjectCacheNO
{
    ELDC_OBJECTCACHE_COLUMN_ID = 0,     /**< Column Cache By ID */
    ELDC_OBJECTCACHE_COLUMN_NAME,       /**< Column Cache By Name */

    ELDC_OBJECTCACHE_INDEX_ID,          /**< Index Cache By ID */
    ELDC_OBJECTCACHE_INDEX_NAME,        /**< Index Cache By Name */
    
    ELDC_OBJECTCACHE_CONSTRAINT_ID,     /**< Constraint Cache By ID */
    ELDC_OBJECTCACHE_CONSTRAINT_NAME,   /**< Constraint Cache By Name */
    
    ELDC_OBJECTCACHE_TABLE_ID,          /**< Table Cache By ID */
    ELDC_OBJECTCACHE_TABLE_NAME,        /**< Table Cache By Name */

    ELDC_OBJECTCACHE_SEQUENCE_ID,       /**< Sequence Cache By ID */
    ELDC_OBJECTCACHE_SEQUENCE_NAME,     /**< Sequence Cache By Name */

    ELDC_OBJECTCACHE_SCHEMA_ID,         /**< Schema Cache By ID */
    ELDC_OBJECTCACHE_SCHEMA_NAME,       /**< Schema Cache By Name */

    ELDC_OBJECTCACHE_SPACE_ID,          /**< Tablespace Cache By ID */
    ELDC_OBJECTCACHE_SPACE_NAME,        /**< Tablespace Cache By Name */

    ELDC_OBJECTCACHE_AUTH_ID,           /**< Authorization Cache By ID */
    ELDC_OBJECTCACHE_AUTH_NAME,         /**< Authorization Cache By Name */

    ELDC_OBJECTCACHE_CATALOG_ID,        /**< Catalog Cache By ID */
    ELDC_OBJECTCACHE_CATALOG_NAME,      /**< Catalog Cache By Name */

    ELDC_OBJECTCACHE_SYNONYM_ID,        /**< Synonym Cache By ID */
    ELDC_OBJECTCACHE_SYNONYM_NAME,      /**< Synonym Cache By Name */

    ELDC_OBJECTCACHE_PUBLIC_SYNONYM_ID, /**< Public Synonym Cache By ID */
    ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME,/**< Public Synonym Cache By Name */

    ELDC_OBJECTCACHE_PROFILE_ID,        /**< Profile Cache By ID */
    ELDC_OBJECTCACHE_PROFILE_NAME,      /**< Profile Cache By Name */
    
    ELDC_OBJECTCACHE_MAX,
} eldcObjectCacheNO;

/**
 * @brief Object Cache 객체 
 */

extern eldcDictDump    gEldcObjectDump[ELDC_OBJECTCACHE_MAX];

/** @} eldcObject */









/**
 * @defgroup eldcPrivilege SQL Privilege Cache
 * @ingroup eldc
 * @{
 * @remarks Privilege 를 위한 Cache 
 * <BR> - Name Cache는 존재하지 않으며, ID Coupled Cache 로 구성된다.
 * <BR> - Object Cache 와 달리 Version 정보를 관리하지 않는다.
 * <BR> - Privilege 는 Real Time 으로 바로 적용되야 하기 떄문이기도 하지만,
 * <BR> - Privilege 검사는 Validation(또는 Prepare-Execution에서의 Execution Validation)
 *        과정에서만 이루어지기 때문이다.
 * <BR> - 권한은 과거에 유효했던 권한이라는 개념이 없으므로, Versioning 대상이 아니다.
 */


/**
 * @brief Privilege Cache 종류
 */

typedef enum eldcPrivCacheNO
{
    ELDC_PRIVCACHE_DATABASE = 0,      /**< Database Privilege Cache */
    ELDC_PRIVCACHE_SPACE,             /**< Tablespace Privilege Cache */
    ELDC_PRIVCACHE_SCHEMA,            /**< Schema Privilege Cache */
    ELDC_PRIVCACHE_TABLE,             /**< Table Privilege Cache */
    ELDC_PRIVCACHE_USAGE,             /**< Usage Privilege Cache */
    ELDC_PRIVCACHE_COLUMN,            /**< Column Privilege Cache */
    
    ELDC_PRIVCACHE_MAX
} eldcPrivCacheNO;

/**
 * @brief Privilege Cache 객체 
 */

extern eldcDictDump    gEldcPrivDump[ELDC_PRIVCACHE_MAX];

/** @} eldcPrivilege */

/** @} eldc */


/**
 * @defgroup eldcNonService Non-Service 단계에서의 SQL-Object Cache
 * @ingroup eldc
 * @{
 */

/** @} eldcNonService */


/** @} elInternal */


#endif /* _ELDC_DEF_H_ */
