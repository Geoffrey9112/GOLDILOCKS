/*******************************************************************************
 * elgFixedTable.h
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


#ifndef _ELG_FIXED_TABLE_H_
#define _ELG_FIXED_TABLE_H_ 1

/**
 * @file elgFixedTable.h
 * @brief Fixed Tables for Execution Library Layer
 */

/**
 * @defgroup elgFixedTable Fixed Tables for Execution Library Layer
 * @ingroup elg
 * @internal
 * @{
 */

stlStatus elgRegisterFixedTables( ellEnv * aEnv );

stlStatus elgGetBaseTableHandleDumpCallback( stlInt64   aTransID,
                                             stlInt64   aViewSCN,
                                             stlChar  * aSchemaName,
                                             stlChar  * aTableName,
                                             stlChar  * aNA,
                                             void    ** aTableHandle,
                                             stlBool  * aExist,
                                             knlEnv   * aEnv );

stlStatus elgGetIndexHandleDumpCallback( stlInt64   aTransID,
                                         stlInt64   aViewSCN,
                                         stlChar  * aSchemaName,
                                         stlChar  * aIndexName,
                                         stlChar  * aNA,
                                         void    ** aIndexHandle,
                                         stlBool  * aExist,
                                         knlEnv   * aEnv );

stlStatus elgGetSequenceHandleDumpCallback( stlInt64   aTransID,
                                            stlInt64   aViewSCN,
                                            stlChar  * aSchemaName,
                                            stlChar  * aSeqName,
                                            stlChar  * aNA,
                                            void    ** aSeqHandle,
                                            stlBool  * aExist,
                                            knlEnv   * aEnv );

stlStatus elgGetSpaceHandleDumpCallback( stlInt64   aTransID,
                                         stlInt64   aViewSCN,
                                         stlChar  * aNA,
                                         stlChar  * aSpaceName,
                                         stlChar  * aNA2,
                                         void    ** aSpaceID,
                                         stlBool  * aExist,
                                         knlEnv   * aEnv );


/**********************************************************
 * D$OBJECT_CACHE
 **********************************************************/


/**
 * @defgroup elgD_OBJECT_CACHE D$OBJECT_CACHE Dump 테이블
 * @brief
 * <BR> OBJECT_CACHE 자료구조를 확인한다.
 * <BR> 다음과 같은 항목이 있다.
 * <BR> - D$OBJECT_CACHE( 'COLUMN_ID' )   : COLUMN_ID Object Cache 를 조회 
 * <BR> - D$OBJECT_CACHE( 'COLUMN_NAME' ) : COLUMN_NAME Object Cache 를 조회 
 * <BR> - D$OBJECT_CACHE( 'INDEX_ID' )    : INDEX_ID Object Cache 를 조회 
 * <BR> - D$OBJECT_CACHE( 'INDEX_NAME' )  : INDEX_NAME Object Cache 를 조회 
 * <BR> - D$OBJECT_CACHE( 'CONSTRAINT_ID' )    : CONSTRAINT_ID Object Cache 를 조회 
 * <BR> - D$OBJECT_CACHE( 'CONSTRAINT_NAME' )  : CONSTRAINT_NAME Object Cache 를 조회 
 * <BR> - D$OBJECT_CACHE( 'SCHEMA_ID' )   : SCHEMA_ID Object Cache 를 조회 
 * <BR> - D$OBJECT_CACHE( 'SCHEMA_NAME' ) : SCHEMA_NAME Object Cache 를 조회 
 * <BR> - D$OBJECT_CACHE( 'TABLE_ID' )    : TABLE_ID Object Cache 를 조회 
 * <BR> - D$OBJECT_CACHE( 'TABLE_NAME' )  : TABLE_NAME Object Cache 를 조회 
 * <BR> - D$OBJECT_CACHE( 'SEQUENCE_ID' )    : SEQUENCE_ID Object Cache 를 조회 
 * <BR> - D$OBJECT_CACHE( 'SEQUENCE_NAME' )  : SEQUENCE_NAME Object Cache 를 조회 
 * <BR> - D$OBJECT_CACHE( 'TABLESPACE_ID' )  : TABLESPACE_ID Object Cache 를 조회 
 * <BR> - D$OBJECT_CACHE( 'TABLESPACE_NAME' ): TABLESPACE_NAME Object Cache 를 조회 
 * <BR> - D$OBJECT_CACHE( 'AUTHORIZATION_ID' )  : AUTHORIZATION_ID Object Cache 를 조회 
 * <BR> - D$OBJECT_CACHE( 'AUTHORIZATION_NAME' ): AUTHORIZATION_NAME Object Cache 를 조회 
 * @ingroup elgFixedTable
 * @internal
 * @{
 */


/**
 * @brief D$OBJECT_CACHE 를 구성하기 위한 자료구조
 * @remarks
 */

typedef struct elgDumpObjectCache
{
    /*
     * Bucket 영역
     */
    
    stlInt32  mBucketNO;          /**< Bucket Number */

    /*
     * Elemenet 영역
     */
    
    stlInt32     mElementNO;          /**< Element Number */
    stlInt64     mElementHashValue;   /**< Hash Value */

    /* Hash Element Version Info */
    smlTransId   mElementCreateTransID;     /**< DDL 을 수행한/수행하고 있는 Transcation ID */
    smlScn       mElementCreateCommitSCN;   /**< Transaction 의 Commit SCN */
    smlTransId   mElementDropTransID;       /**< DDL 을 수행한/수행하고 있는 Transcation ID */
    smlScn       mElementDropCommitSCN;     /**< Transaction 의 Commit SCN */
    stlBool      mElementDropFlag;          /**< Drop 여부 */
    stlInt64     mElementModifyingCnt;      /**< table related object를 변경하고 있는 트랜잭션의 개수 */
    stlInt64     mElementTryModifyCnt;      /**< 객체 생성후 table related object 변경 횟수 */
    stlInt64     mElementMemUseCnt;         /**< 홀수: Alloc Memory, 짝수: Free Memory */
    
    stlChar   mBufferHashData[ELDC_DUMP_PRINT_BUFFER_SIZE];    /**< Hash Data 출력 결과 */
    stlChar   mBufferObjectDesc[ELDC_DUMP_PRINT_BUFFER_SIZE];  /**< Object Descritor 출력 */
    
} elgDumpObjectCache;


/**
 * @brief D$OBJECT_CACHE를 위한 Column Definition
 */
extern knlFxColumnDesc gElgDumpObjectCacheColDesc[];




/**
 * @brief D$OBJECT_CACHE 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 */
typedef struct elgDumpObjectCachePathCtrl
{
    stlUInt32           mBucketNO;            /**< Bucket NO */
    stlUInt32           mElementNO;           /**< Element NO */
    eldcHashElement   * mHashElement;         /**< Hash Element */
} elgDumpObjectCachePathCtrl;


stlStatus elgGetHandleDumpObjectCacheCallback( stlInt64   aTransID,
                                               void     * aStmt,
                                               stlChar  * aDumpOption,
                                               void    ** aDumpObjHandle,
                                               knlEnv   * aKnlEnv );

stlStatus elgOpenDumpObjectCacheCallback( void   * aStmt,
                                          void   * aDumpObjHandle,
                                          void   * aPathCtrl,
                                          knlEnv * aKnlEnv );

stlStatus elgCloseDumpObjectCacheCallback( void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aKnlEnv );

stlStatus elgBuildRecordDumpObjectCacheCallback( void              * aDumpObjHandle,
                                                 void              * aPathCtrl,
                                                 knlValueBlockList * aValueList,
                                                 stlInt32            aBlockIdx,
                                                 stlBool           * aIsExist,
                                                 knlEnv            * aKnlEnv );

/**
 * @brief D$OBJECT_CACHE 의 Table Definition
 */

extern knlFxTableDesc gDumpObjectCacheTabDesc;

/** @} elgD_OBJECT_CACHE */



/**********************************************************
 * D$PRIV_CACHE
 **********************************************************/

/**
 * @defgroup elgD_PRIV_CACHE D$PRIV_CACHE Dump 테이블
 * @brief
 * <BR> PRIV_CACHE 자료구조를 확인한다.
 * <BR> 다음과 같은 항목이 있다.
 * <BR> - D$PRIV_CACHE( 'DATABASE' )   : DATABASE Privilege Cache 를 조회 
 * @ingroup elgFixedTable
 * @remarks Object Cache 와 대부분 동일한 자료이다.
 * @internal
 * @{
 */

stlStatus elgGetHandleDumpPrivCacheCallback( stlInt64   aTransID,
                                             void     * aStmt,
                                             stlChar  * aDumpOption,
                                             void    ** aDumpObjHandle,
                                             knlEnv   * aKnlEnv );


/**
 * @brief D$PRIV_CACHE 의 Table Definition
 */

extern knlFxTableDesc gDumpPrivCacheTabDesc;

/** @} elgD_PRIV_CACHE */


/**********************************************************
 * D$PRODUCT
 **********************************************************/

/**
 * @defgroup elgD_PRODUCT Dump 테이블
 * @internal
 * @{
 */

#define ELG_PRODUCT_NAME_LEN      (128)
#define ELG_PRODUCT_COMMENTS_LEN  (1024)

/**
 * @brief D$PRODUCT 를 구성하기 위한 자료구조
 * @remarks
 */
typedef struct elgDumpProduct
{
    stlChar   * mName;
    stlChar   * mComment;
} elgDumpProduct;


/**
 * @brief D$PRODUCT 를 위한 Column Definition
 */
extern knlFxColumnDesc gElgDumpProductColDesc[];

/**
 * @brief D$PRODUCT 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 */
typedef struct elgDumpProductPathCtrl
{
    stlInt32    mNO;
} elgDumpProductPathCtrl;


stlStatus elgGetHandleDumpProductCallback( stlInt64   aTransID,
                                           void     * aStmt,
                                           stlChar  * aDumpOption,
                                           void    ** aDumpObjHandle,
                                           knlEnv   * aKnlEnv );

stlStatus elgOpenDumpProductCallback( void   * aStmt,
                                      void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aKnlEnv );

stlStatus elgCloseDumpProductCallback( void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aKnlEnv );

stlStatus elgBuildRecordProductCacheCallback( void              * aDumpObjHandle,
                                              void              * aPathCtrl,
                                              knlValueBlockList * aValueList,
                                              stlInt32            aBlockIdx,
                                              stlBool           * aIsExist,
                                              knlEnv            * aKnlEnv );

/**
 * @brief D$PRODUCT 의 Table Definition
 */

extern knlFxTableDesc gDumpProductTabDesc;

/** @} elgD_PRODUCT */



/**********************************************************
 * X$BUILTIN_FUNCTION
 **********************************************************/


/**
 * @defgroup elgX_BuiltInFunction X$BUILTIN_FUNCTION fixed 테이블
 * @internal
 * @{
 */

/**
 * @brief X$BUILTIN_FUNCTION 을 구성하기 위한 자료구조
 * @remarks
 */
typedef struct elgFixedBuiltInFunction
{
    stlInt32    mFuncID;             /**< Function ID */
    stlChar   * mFuncName;           /**< Function Name */
    stlInt32    mMinArgCnt;          /**< Minimum Argument Count */
    stlInt32    mMaxArgCnt;          /**< Maximum Argument Count */
    stlChar   * mReturnValueClass;   /**< Return Value Class */
    stlChar   * mIterationTime;      /**< Iteration Time */
    stlBool     mNeedCastInGroup;    /**< Need Casting in Same Group */
    stlChar   * mExceptionAction;    /**< Exception Action  */
    stlChar   * mExceptionResult;    /**< Exception Result Value */
} elgFixedBuiltInFunction;


/**
 * @brief X$BUILTIN_FUNCTION 를 위한 Column Definition
 */
extern knlFxColumnDesc gElgFixedBuiltInFunctionColDesc[];

/**
 * @brief X$BUILTIN_FUNCTION 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 */
typedef struct elgFixedBuiltInFunctionPathCtrl
{
    stlInt32    mNO;
} elgFixedBuiltInFunctionPathCtrl;


stlStatus elgOpenFixedBuiltInFunctionCallback( void   * aStmt,
                                               void   * aDumpObjHandle,
                                               void   * aPathCtrl,
                                               knlEnv * aKnlEnv );

stlStatus elgCloseFixedBuiltInFunctionCallback( void   * aDumpObjHandle,
                                                void   * aPathCtrl,
                                                knlEnv * aKnlEnv );

stlStatus elgBuildRecordBuiltInFunctionCallback( void              * aDumpObjHandle,
                                                 void              * aPathCtrl,
                                                 knlValueBlockList * aValueList,
                                                 stlInt32            aBlockIdx,
                                                 stlBool           * aIsExist,
                                                 knlEnv            * aKnlEnv );

/**
 * @brief X$BUILTIN_FUNCTION 의 Table Definition
 */

extern knlFxTableDesc gElgFixedBuiltInFunctionTabDesc;

/** @} elgX_BuiltIn_Function */



/**********************************************************
 * X$PSEUDO_FUNCTION
 **********************************************************/


/**
 * @defgroup elgX_PseudoFunction X$PSEUDO_FUNCTION fixed 테이블
 * @internal
 * @{
 */

/**
 * @brief X$PSEUDO_FUNCTION 을 구성하기 위한 자료구조
 * @remarks
 */
typedef struct elgFixedPseudoFunction
{
    stlInt32    mFuncID;             /**< Function ID */
    stlChar   * mFuncName;           /**< Function Name */
    stlChar   * mIterationTime;      /**< Iteration Time */
} elgFixedPseudoFunction;


/**
 * @brief X$PSEUDO_FUNCTION 를 위한 Column Definition
 */
extern knlFxColumnDesc gElgFixedPseudoFunctionColDesc[];

/**
 * @brief X$PSEUDO_FUNCTION 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 */
typedef struct elgFixedPseudoFunctionPathCtrl
{
    stlInt32    mNO;
} elgFixedPseudoFunctionPathCtrl;


stlStatus elgOpenFixedPseudoFunctionCallback( void   * aStmt,
                                              void   * aDumpObjHandle,
                                              void   * aPathCtrl,
                                              knlEnv * aKnlEnv );

stlStatus elgCloseFixedPseudoFunctionCallback( void   * aDumpObjHandle,
                                               void   * aPathCtrl,
                                               knlEnv * aKnlEnv );

stlStatus elgBuildRecordPseudoFunctionCallback( void              * aDumpObjHandle,
                                                void              * aPathCtrl,
                                                knlValueBlockList * aValueList,
                                                stlInt32            aBlockIdx,
                                                stlBool           * aIsExist,
                                                knlEnv            * aKnlEnv );

/**
 * @brief X$PSEUDO_FUNCTION 의 Table Definition
 */

extern knlFxTableDesc gElgFixedPseudoFunctionTabDesc;

/** @} elgX_Pseudo_Function */



/**********************************************************
 * X$EXE_SYSTEM_INFO
 **********************************************************/


/**
 * @defgroup elgX_EXE_SYSTEM_INFO X$EXE_SYSTEM_INFO fixed 테이블
 * @internal
 * @{
 */

#define ELG_EXE_SYSTEM_INFO_ROW_COUNT  (10)

/**
 * @brief X$EXE_SYSTEM_INFO 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 */
typedef struct elgFxExeSystemInfoPathCtrl
{
    stlInt32      mIterator;
    stlInt64      mValues[ELG_EXE_SYSTEM_INFO_ROW_COUNT];
} elgFxExeSystemInfoPathCtrl;


stlStatus elgOpenFxExeSystemInfoCallback( void   * aStmt,
                                          void   * aDumpObjHandle,
                                          void   * aPathCtrl,
                                          knlEnv * aKnlEnv );

stlStatus elgCloseFxExeSystemInfoCallback( void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aKnlEnv );

stlStatus elgBuildRecordFxExeSystemInfoCallback( void              * aDumpObjHandle,
                                                 void              * aPathCtrl,
                                                 knlValueBlockList * aValueList,
                                                 stlInt32            aBlockIdx,
                                                 stlBool           * aIsExist,
                                                 knlEnv            * aKnlEnv );

/**
 * @brief X$EXE_SYSTEM_INFO 의 Table Definition
 */

extern knlFxTableDesc gElgFxExeSystemInfoTabDesc;

/** @} elgX_EXE_SYSTEM_INFO */


/**********************************************************
 * X$EXE_PROC_ENV
 **********************************************************/


/**
 * @defgroup elgX_EXE_PROC_ENV X$EXE_PROC_ENV fixed 테이블
 * @internal
 * @{
 */

#define ELG_EXE_PROC_ENV_ROW_COUNT  (2)

/**
 * @brief X$EXE_PROC_ENV 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 */
typedef struct elgFxExeProcEnvPathCtrl
{
    void        * mCurrEnv;
    stlUInt32     mProcId;
    stlInt32      mIterator;
    stlInt64      mValues[ELG_EXE_PROC_ENV_ROW_COUNT];
} elgFxExeProcEnvPathCtrl;


stlStatus elgOpenFxExeProcEnvCallback( void   * aStmt,
                                       void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aKnlEnv );

stlStatus elgCloseFxExeProcEnvCallback( void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aKnlEnv );

stlStatus elgBuildRecordFxExeProcEnvCallback( void              * aDumpObjHandle,
                                              void              * aPathCtrl,
                                              knlValueBlockList * aValueList,
                                              stlInt32            aBlockIdx,
                                              stlBool           * aIsExist,
                                              knlEnv            * aKnlEnv );

/**
 * @brief X$EXE_PROC_ENV 의 Table Definition
 */

extern knlFxTableDesc gElgFxExeProcEnvTabDesc;

/** @} elgX_EXE_PROC_ENV */



/**********************************************************
 * X$EXE_SESS_ENV
 **********************************************************/

/**
 * @defgroup elgX_EXE_SESS_ENV X$EXE_SESS_ENV fixed 테이블
 * @internal
 * @{
 */

#define ELG_EXE_SESS_ENV_ROW_COUNT  (9)


/**
 * @brief X$EXE_SESS_ENV 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 */
typedef struct elgFxExeSessEnvPathCtrl
{
    knlSessionEnv * mCurEnv;
    stlUInt32       mSessId;
    stlInt32        mIterator;
    stlInt64        mValues[ELG_EXE_SESS_ENV_ROW_COUNT];
} elgFxExeSessEnvPathCtrl;


stlStatus elgOpenFxExeSessEnvCallback( void   * aStmt,
                                       void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aKnlEnv );

stlStatus elgCloseFxExeSessEnvCallback( void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aKnlEnv );

stlStatus elgBuildRecordFxExeSessEnvCallback( void              * aDumpObjHandle,
                                              void              * aPathCtrl,
                                              knlValueBlockList * aValueList,
                                              stlInt32            aBlockIdx,
                                              stlBool           * aIsExist,
                                              knlEnv            * aKnlEnv );

/**
 * @brief X$EXE_SESS_ENV 의 Table Definition
 */

extern knlFxTableDesc gElgFxExeSessEnvTabDesc;

/** @} elgX_EXE_SESS_ENV */




/**********************************************************
 * X$NAMED_CURSOR
 **********************************************************/

/**
 * @defgroup elgX_NAMED_CURSOR X$NAMED_CURSOR fixed 테이블
 * @internal
 * @{
 */


/**
 * @brief X$NAMED_CURSOR 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 */
typedef struct elgNameCursorPathCtrl
{
    knlSessionEnv * mCurEnv;
    stlInt32        mCurSlot;
} elgNameCursorPathCtrl;


/**
 * @brief X$NAMED_CURSOR 의 레코드
 * @remarks
 */
typedef struct elgNameCursorFxRecord
{
    stlInt32     mSlotID;
    stlUInt32    mSessID;
    stlChar      mCursorName[STL_MAX_SQL_IDENTIFIER_LENGTH];       
    stlBool      mIsOpen;
    stlTime      mOpenTime;
    stlInt64     mViewScn;
    stlBool      mIsSensitive;
    stlBool      mIsScrollable;
    stlBool      mIsHoldable;
    stlBool      mIsUpdatable;
    stlChar      mCursorQuery[ELL_MAX_NAMED_CURSOR_QUERY_SIZE];    /**< Cursor Query */
} elgNameCursorFxRecord;
    
stlStatus elgOpenFxNamedCursorCallback( void   * aStmt,
                                        void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aKnlEnv );

stlStatus elgCloseFxNamedCursorCallback( void   * aDumpObjHandle,
                                         void   * aPathCtrl,
                                         knlEnv * aKnlEnv );

stlStatus elgBuildRecordFxNamedCursorCallback( void              * aDumpObjHandle,
                                               void              * aPathCtrl,
                                               knlValueBlockList * aValueList,
                                               stlInt32            aBlockIdx,
                                               stlBool           * aIsExist,
                                               knlEnv            * aKnlEnv );


/**
 * @brief X$NAMED_CURSOR 를 위한 Column Definition
 */
extern knlFxColumnDesc gElgNamedCursorColDesc[];


/**
 * @brief X$NAMED_CURSOR 의 Table Definition
 */
extern knlFxTableDesc gElgNamedCursorTabDesc;

/** @} elgX_NAMED_CURSOR */




/** @} elgFixedTable */
    
#endif /* _ELG_FIXED_TABLE_H_ */
