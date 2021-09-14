/*******************************************************************************
 * qlpNodeAttrInfo.h
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

#ifndef _QLPNODEATTRINFO_H_
#define _QLPNODEATTRINFO_H_ 1


/**
 * @file qlpNodeAttrInfo.h
 * @brief SQL Processor Layer Parser
 */

/**
 * @defgroup qlpNodeAttrInfo Attribute Nodes
 * @ingroup qlpNode
 * @{
 */

/*******************************************************************************
 * DEFINITIONS 
 ******************************************************************************/

/**
 * @brief Size Unit 종류
 */
typedef enum qlpSizeUnit
{
    QLP_SIZE_UNIT_INVALID = 0,
    QLP_SIZE_UNIT_BYTE, 
    QLP_SIZE_UNIT_KILO, 
    QLP_SIZE_UNIT_MEGA,
    QLP_SIZE_UNIT_GIGA,
    QLP_SIZE_UNIT_TERA
} qlpSizeUnit;


/**
 * @brief tablespace 속성
 */
typedef enum qlpTablespaceAttrType
{
    QLP_TABLESPACE_ATTR_INVALID = 0,
    QLP_TABLESPACE_ATTR_LOGGING,
    QLP_TABLESPACE_ATTR_ONLINE,
    QLP_TABLESPACE_ATTR_EXTSIZE,
    QLP_TABLESPACE_ATTR_MAX
} qlpTablespaceAttrType;

#define QLP_GET_BYTES_FROM_SIZE_UNIT( unit )    \
    (                   (unit) == QLP_SIZE_UNIT_BYTE ? \
      1             : ( (unit) == QLP_SIZE_UNIT_KILO ? \
      1024          : ( (unit) == QLP_SIZE_UNIT_MEGA ? \
      1048576       : ( (unit) == QLP_SIZE_UNIT_GIGA ? \
      1073741824    : ( (unit) == QLP_SIZE_UNIT_TERA ? \
      1099511627776 : 1 ) ) ) ) )

/*******************************************************************************
 * STRUCTURES 
 ******************************************************************************/

    
/**
 * @brief object name 정보 : QLP_OBJECT_NAME_TYPE
 */
struct qlpObjectName   
{
    qllNodeType     mType;           /**< node type */
    stlInt32        mNodePos;        /**< node position */
    
    // stlInt32        mCatalogPos;     /**< catalog name position */
    stlInt32        mSchemaPos;      /**< schema name position */
    stlInt32        mObjectPos;      /**< object name position */

    // stlChar       * mCatalog;        /**< catalog name */
    stlChar       * mSchema;         /**< schema name */
    stlChar       * mObject;         /**< object name */
};

/**
 * @brief position을 갖는 object name 정보 : QLP_ORDERING_TYPE
 */
struct qlpOrdering
{
    qllNodeType     mType;           /**< node type */
    stlInt32        mNodePos;        /**< node position */

    qlpObjectName * mObjectName;     /**< Object Name */

    stlBool         mIsFixedPosition;/**< Is Fixed Position */
    stlBool         mIsLeft;         /**< Is Left */
};

/**
 * @brief object name 정보 : QLP_OBJECT_NAME_TYPE
 */
struct qlpColumnName   
{
    qllNodeType     mType;           /**< node type */
    stlInt32        mNodePos;        /**< node position */
    
    // stlInt32        mCatalogPos;     /**< catalog name position */
    stlInt32        mSchemaPos;      /**< schema name position */
    stlInt32        mTablePos;       /**< table name position */
    stlInt32        mColumnPos;      /**< table name position */

    // stlChar       * mCatalog;        /**< catalog name */
    stlChar       * mSchema;         /**< schema name */
    stlChar       * mTable;          /**< table name */
    stlChar       * mColumn;         /**< column name */
};


/**
 * @brief character length 정보 : QLP_CHAR_LENGTH_TYPE
 */
struct qlpCharLength   
{
    qllNodeType            mType;           /**< node type */
    stlInt32               mNodePos;        /**< node position */

    stlInt32               mLengthUnitPos;  /**< length unit position */

    dtlStringLengthUnit    mLengthUnit;     /**< length unit */
    qlpValue             * mLength;         /**< length */
};


/**
 * @brief column definition second clause : QLP_COL_DEF_SECOND_TYPE
 */
struct qlpColDefSecond
{
    qllNodeType          mType;           /**< node type */
    stlInt32             mNodePos;        /**< node position */

    stlInt32             mIdentTypePos;   /**< position : type of identity generation */

    /* default clause */
    qllNode            * mRawDefault;     /**< default value : untransformed parse tree */

    /* identity column specification */
    qlpIdentityType      mIdentType;        /**< type of identity generation */
    qlpSequenceOption  * mIdentSeqOptions;  /**< sequence options for identity */

    /* generation clause */
    qllNode            * mGenerationExpr;   /**< expression of generation */
};

/**
 * @brief ALTER COLUMN alter_identity_spec
 */
struct qlpAlterIdentity
{
    qlpIdentityType      mIdentType;        /**< type of identity generation */
    qlpSequenceOption  * mIdentSeqOptions;  /**< sequence options for identity */
};



/**
 * @brief referential triggered action 정보 : QLP_REF_TRIGGER_ACTION_TYPE
 */
struct qlpRefTriggerAction
{
    qllNodeType   mType;              /**< node type */
    stlInt32      mNodePos;           /**< node position */

    qlpValue    * mUpdateAction;      /**< update action : qllValue (integer : qlpFKAction ) */
    qlpValue    * mDeleteAction;      /**< delete action : qllValue (integer : qlpFKAction ) */
};


/**
 * @brief TABLE Hint : QLL_HINT_TYPE
 */
struct qlpHintTable
{
    qllNodeType         mType;          /**< node type */
    stlInt32            mNodePos;       /**< node position */

    qlpHintType         mHintType;      /**< hint type (QLP_HINT_TABLE) */
    stlBool             mIsUsed;        /**< 실제 사용되는지 체크를 위해 사용 */

    qlpTableHintType    mTableHintType; /**< table hint type */
    qlpObjectName     * mObjectName;    /**< object name */
};


/**
 * @brief INDEX Hint : QLL_HINT_TYPE
 */
struct qlpHintIndex
{
    qllNodeType         mType;          /**< node type */
    stlInt32            mNodePos;       /**< node position */

    qlpHintType         mHintType;      /**< hint type (QLP_HINT_INDEX) */
    stlBool             mIsUsed;        /**< 실제 사용되는지 체크를 위해 사용 */

    qlpIndexHintType    mIndexHintType; /**< index hint type */
    qlpObjectName     * mObjectName;    /**< object name */
    qlpList           * mIndexScanList; /**< index scan list */
};


/**
 * @brief JOIN ORDER Hint : QLL_HINT_TYPE
 */
struct qlpHintJoinOrder
{
    qllNodeType             mType;              /**< node type */
    stlInt32                mNodePos;           /**< node position */

    qlpHintType             mHintType;          /**< hint type (QLP_HINT_JOIN_ORDER) */
    stlBool                 mIsUsed;            /**< 실제 사용되는지 체크를 위해 사용 */

    qlpJoinOrderHintType    mJoinOrderHintType; /**< join order hint type */
    qlpList               * mObjectNameList;    /**< object name list */
};


/**
 * @brief JOIN OPERATION Hint : QLL_HINT_TYPE
 */
struct qlpHintJoinOper
{
    qllNodeType             mType;              /**< node type */
    stlInt32                mNodePos;           /**< node position */

    qlpHintType             mHintType;          /**< hint type (QLP_HINT_JOIN_OPER) */
    stlBool                 mIsUsed;            /**< 실제 사용되는지 체크를 위해 사용 */

    qlpJoinOperHintType     mJoinOperHintType;  /**< join operation hint type */
    stlBool                 mIsNoUse;           /**< join operation 사용여부 */
    qlpList               * mObjectNameList;    /**< object name list */
};


/**
 * @brief QUERY TRANSFORMATION Hint : QLL_HINT_TYPE
 */
struct qlpHintQueryTransform
{
    qllNodeType                 mType;                      /**< node type */
    stlInt32                    mNodePos;                   /**< node position */

    qlpHintType                 mHintType;                  /**< hint type (QLP_HINT_QUERY_TRANSFORMATION) */
    stlBool                     mIsUsed;                    /**< 실제 사용되는지 체크를 위해 사용 */

    qlpQueryTransformHintType   mQueryTransformHintType;    /**< query transformation hint type */
};


/**
 * @brief OTHER Hint : QLL_HINT_TYPE
 */
struct qlpHintOther
{
    qllNodeType         mType;          /**< node type */
    stlInt32            mNodePos;       /**< node position */

    qlpHintType         mHintType;      /**< hint type */
    stlBool             mIsUsed;        /**< 실제 사용되는지 체크를 위해 사용 */

    stlBool             mIsNoUse;       /**< push pred 사용여부 */
    qlpObjectName     * mObjectName;    /**< object name */

    qlpOtherHintType    mOtherHintType; /**< other hint type */
};


/**
 * @brief datafile attributes : QLP_DATAFILE_TYPE
 */
struct qlpDatafile
{
    qllNodeType      mType;         /**< node type */
    stlInt32         mNodePos;      /**< node position */

    qlpValue       * mFilePath;     /**< file path : string */
    qlpSize        * mInitSize;     /**< file size : float */
    qlpValue       * mReuse;        /**< reuse : boolean */
    qlpAutoExtend  * mAutoExtend;   /**< autoextend attributes */ 
};


/**
 * @brief datafile autoextend attributes : QLP_AUTO_EXTEND_TYPE
 */
struct qlpAutoExtend
{
    qllNodeType    mType;         /**< node type */
    stlInt32       mNodePos;      /**< node position */

    qlpValue     * mIsAutoOn;     /**< auto extend : ON / OFF */
    qlpSize      * mNextSize;     /**< auto extend : next size */
    qlpSize      * mMaxSize;      /**< auto extend : max size */ 
};


/**
 * @brief tablespace attributes : QLP_TABLESPACE_ATTR_TYPE
 */
struct qlpTablespaceAttr
{
    qllNodeType        mType;             /**< node type */
    stlInt32           mNodePos;          /**< node position */

    qlpValue         * mIsLogging;        /**< LOGGING or NO LOGGING */
    qlpValue         * mIsOnline;         /**< ONLINE or OFFLINE */
    qlpSize          * mExtSize;          /**< EXTENT SIZE */
    stlBool            mIsSetting[ QLP_TABLESPACE_ATTR_MAX ];
};

/**
 * @brief insert source : QLP_INSERT_SOURCE_TYPE
 */
struct qlpInsertSource
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    stlBool            mIsAllColumn;    /**< is all column */
    stlBool            mIsDefault;      /**< is default values */ 

    qlpList          * mColumns;        /**< target columns : value (if all columns, Column list is NULL) */
    qlpList          * mValues;         /**< source : value (if row default, Values list is NULL) */
    qllNode          * mSubquery;       /**< SELECT Subquery */
};


/**
 * @brief insert source : QLL_CONSTRAINT_ATTR__TYPE
 */
struct qlpConstraintAttr
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    qlpValue         * mInitDeferred;       /**< initially deferred : integer(boolean) */
    qlpValue         * mDeferrable;         /**< deferrable : integer(boolean) */
    qlpValue         * mEnforce;            /**< enforce : integer(boolean) */
};


/**
 * @brief size : QLL_SIZE_TYPE
 */
struct qlpSize
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    stlBool            mIsUnlimited;    /**< is unlimited ? */
    qlpValue         * mSizeUnit;       /**< size unit : qlpSizeUnit */
    qlpValue         * mSize;           /**< size : float */
};


/******************************************************************************* 
 * STRUCTURES FOR PARSE : parse 과정에서만 사용할 자료 구조
 ******************************************************************************/

/** @} */

#endif /* _QLPNODEATTRINFO_H_ */
