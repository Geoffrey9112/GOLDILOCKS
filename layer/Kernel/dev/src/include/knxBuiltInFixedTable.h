/*******************************************************************************
 * knxBuiltInFixedTable.h
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


#ifndef _KNX_BUILTIN_FIXEDTABLE_H_
#define _KNX_BUILTIN_FIXEDTABLE_H_ 1

/**
 * @file knxBuiltInFixedTable.h
 * @brief Kernel Layer에서 구축되는 기본 Fixed Table 들
 */


/**
 * @defgroup   knxBuiltInFixedTable 기본적으로 구축되는 Fixed Table
 * @ingroup KNX
 * @internal
 * @{
 */
/** @} */

/**
 * @defgroup  X_TABLES X$TABLES 테이블
 * @brief
 * <BR> 모든 Fixed Table들의 테이블 정보를 획득할 수 있다.
 * <BR> knxFxTableDefinition 자료구조에 대한 Fixed Table 정의 방법으로
 * <BR> 비교적 간단한 List 유형의 자료 구조를 Fixed Table로 구성한 Example임.
 * @ingroup knxBuiltInFixedTable
 * @internal
 * @{
 */

/**
 * @brief X$TABLES 를 구성하기 위한 자료구조
 * @remarks
 */

#define KNX_MAX_BUILTIN_NAMESIZE_USAGETYPE    (16)
#define KNX_MAX_BUILTIN_NAMESIZE_STARTUPPHASE (32)

typedef struct knxXTablesStructure {
    stlInt64   mTableID;           /**< Dictionary 와 논리적인 연관성을 유지할 Table ID */
    stlChar  * mTableName;         /**< knxFxTableDefinition::mTableName 에 해당 */
    stlChar  * mTableComment;      /**< knxFxTableDefinition::mTableComment 에 해당 */
    stlInt32   mColumnCount;       /**< knxFxTableDefinition::mColumnCount 에 해당 */
    
    stlInt32   mUsageTypeValue;    /**< knxFxTableDefinition::mUsageType 에 해당*/
    stlChar  * mUsageTypeName;     /**< UsageType을 String으로 변환한 이름 */
    
    stlInt32   mStartupPhaseValue; /**< knxFxTableDefinition::mStartupPhase 에 해당 */
    stlChar  * mStartupPhaseName;  /**< StartupPhase를 String으로 변환한 이름 */
} knxXTablesStructure;


extern knlFxColumnDesc gXTablesColDesc[];

stlStatus knxOpenXTablesCallback( void   * aStmt,
                                  void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aKnlEnv );

stlStatus knxCloseXTablesCallback( void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aKnlEnv );

stlStatus knxBuildRecordXTablesCallback( void              * aDumpObjHandle,
                                         void              * aPathCtrl,
                                         knlValueBlockList * aValueList,
                                         stlInt32            aBlockIdx,
                                         stlBool           * aIsExist,
                                         knlEnv            * aKnlEnv );

/**
 * @brief X$TABLES 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 * <BR> 포인터로 관리가 가능하여 별도의 자료구조를 선언할 필요가 없으나,
 * <BR> 보편화된 형태의 예제가 될 수 있도록 자료구조를 선언함.
 */
typedef struct knxXTablesPathCtrl {
    stlInt32     mTabIdx;    /**< 현재 Fixed Table 의 Idx */
    stlInt64     mTableID;   /**< 음수로 감소하는 Process 내 유일한 Table ID */
} knxXTablesPathCtrl;

extern knlFxTableDesc gXTablesTabDesc;

/** @} */

/**
 * @defgroup  X_COLUMNS X$COLUMNS 테이블
 * @brief
 * <BR> 모든 Fixed Table들의 컬럼 정보를 획득할 수 있다.
 * <BR> 여러가지 자료구조를 참조하고,
 * <BR> 연결관계가 List와 Array가 함께 복합적으로 조합된 자료구조에 대한
 * <BR> Fixed Table 구성 Example이다.
 * @ingroup knxBuiltInFixedTable
 * @internal
 * @{
 */

#define KNX_MAX_BUILTIN_NAMESIZE_DTD_IDENTIFIER    (32)

/**
 * @brief X$COLUMNS 를 구성하기 위한 자료구조
 * @remarks
 * <BR> X$COLUMNS 의 레코드를 구성하기 위한 정보는 여러곳에 흩어져 있다.
 * <BR> 이를 하나의 자료구조에서 참조할 수 있도록 다음 자료구조를 선언한다.
 */


typedef struct knxXColumnsStructure {
    stlInt64    mTableID;         /**< Dictionary 와 논리적인 연관성을 유지할 Table ID */
    stlInt64    mColumnID;        /**< Dictionary 와 논리적인 연관성을 유지할 Column ID */
    stlChar   * mTableName;       /**< knxFxTableDefinition::mTableName 에 해당하는 정보 */
    stlChar   * mColumnName;      /**< knlFxColumnDesc::mColumnName 에 해당하는 정보 */
    stlInt32    mOrdinalPosition; /**< Column 순서 */
    stlInt32    mDataTypeID;      /**< Dictionary 와 논리적인 연관성을 유지할 DataType ID */
    stlChar   * mDataTypeName;    /**< knlFxColumnDesc::mDataType 에 해당하는 정보 */
    stlInt64    mLength;          /**< knlFxColumnDesc::mLength 에 해당하는 정보 */
    stlBool     mNullable;        /**<  knlFxColumnDesc::mNullable 에 해당하는 정보 */
    stlChar   * mColumnComment;   /**< knlFxColumnDesc::mColumnComment 에 해당하는 정보 */
} knxXColumnsStructure;

extern knlFxColumnDesc gXColumnsColDesc[];

stlStatus knxOpenXColumnsCallback( void   * aStmt,
                                   void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aKnlEnv );

stlStatus knxCloseXColumnsCallback( void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aKnlEnv );

stlStatus knxBuildRecordXColumnsCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aIsExist,
                                          knlEnv            * aKnlEnv );

/**
 * @brief X$COLUMNS 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 * 테이블들의 정보는 next 관계로 설정되어 있고
 * 컬럼들의 정보는 array 관계로 설정되어 있다.
 * 이를 관리하기 위한 Path Controller 로 테이블 Pointer와 Column Idx를 유지한다.
 */

typedef struct knxXColumnsPathCtrl {
    stlInt32               mTabIdx;      /**< 현재 Fixed Table Idx */
    stlInt32               mColIdx;      /**< knxFxTableHeapDesc.mColumnDesc[sIdx]
                                          * 정보를 관리 */
    stlInt64               mTableID;     /**< 음수로 감소하는 Process 내 유일한 Table ID */
    stlInt64               mColumnID;    /**< 음수로 감소하는 Process 내 유일한 Column ID */
} knxXColumnsPathCtrl;

extern knlFxTableDesc gXColumnsTabDesc;

/** @} */




/**
 * @defgroup  X_ERROR_CODE X$ERROR_CODE 테이블
 * @brief
 * @ingroup knxBuiltInFixedTable
 * @internal
 * @{
 */

/**
 * @brief X$ERROR_CODE 를 구성하기 위한 자료구조
 * @remarks
 */
typedef struct knxXErrCodeStructure {
    stlInt32    mErrorCode;                           /**< Internal Error Code */
    stlChar     mSQLState[STL_SQLSTATE_LENGTH + 1];   /**< SQL state code */
    stlChar   * mMessage;                             /**< Error Message */
} knxXErrCodeStructure;

extern knlFxColumnDesc gXErrCodeColDesc[];


stlStatus knxOpenXErrCodeCallback( void   * aStmt,
                                   void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aKnlEnv );

stlStatus knxCloseXErrCodeCallback( void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aKnlEnv );

stlStatus knxBuildRecordXErrCodeCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aIsExist,
                                          knlEnv            * aKnlEnv );

/**
 * @brief X$ERROR_CODE 의 레코드를 구성하기 위한 경로 관리 정보
 * @remarks
 */

typedef struct knxXErrCodePathCtrl
{
    stlInt32  mLayerNO;     /**< Layer ID */
    stlInt32  mErrorNO;     /**< Error # in Layer */
} knxXErrCodePathCtrl;

extern knlFxTableDesc gXErrCodeTabDesc;

/** @} X_ERROR_CODE */




#endif /* _KNX_BUILTIN_FIXEDTABLE_H_ */
