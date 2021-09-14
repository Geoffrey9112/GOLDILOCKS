/*******************************************************************************
 * qlpNodeParseTree.h
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

#ifndef _QLPNODEPARSETREE_H_
#define _QLPNODEPARSETREE_H_ 1

/**
 * @file qlpNodeParseTree.h
 * @brief SQL Processor Layer Parse-Tree Nodes 
 */


/**
 * @defgroup qlpNodeParseTree Parse-Tree Nodes 
 * @ingroup qlpNode
 * @{
 */


/*******************************************************************************
 * DEFINITIONS 
 ******************************************************************************/

/**
 * @brief types of constraints
 */
typedef enum qlpConstraintType
{
    QLP_CONSTRAINT_INVALID = 0,
    QLP_CONSTRAINT_NULL,
    QLP_CONSTRAINT_NOTNULL,
    QLP_CONSTRAINT_CHECK,
    QLP_CONSTRAINT_PRIMARY,
    QLP_CONSTRAINT_UNIQUE,
    QLP_CONSTRAINT_FOREIGN,
    QLP_CONSTRAINT_EXCLUSION     /* unsupported */
} qlpConstraintType;


/**
 * @brief match types of foreign key 
 */
typedef enum qlpFKMatchType
{
    QLP_FK_MATCH_INVALID = 0,
    QLP_FK_MATCH_FULL,
    QLP_FK_MATCH_PARTIAL,
    QLP_FK_MATCH_SIMPLE
} qlpFKMatchType;


/**
 * @brief actions of foreign key 
 */
typedef enum qlpFKAction
{
    QLP_FK_ACTION_INVALID = 0,
    QLP_FK_ACTION_NO_ACTION,
    QLP_FK_ACTION_RESTRICT,
    QLP_FK_ACTION_CASCADE,
    QLP_FK_ACTION_SET_NULL,
    QLP_FK_ACTION_SET_DEFAULT
} qlpFKAction;


/**
 * @brief sequence options 
 */
typedef enum qlpSequenceOptionType
{
    QLP_SEQUENCE_OPTION_INVALID = 0,

    QLP_SEQUENCE_OPTION_START_WITH,
    QLP_SEQUENCE_OPTION_INCREMENT,
    QLP_SEQUENCE_OPTION_MAXVALUE,
    QLP_SEQUENCE_OPTION_MINVALUE,
    QLP_SEQUENCE_OPTION_CYCLE,
    QLP_SEQUENCE_OPTION_CACHE,
    QLP_SEQUENCE_OPTION_NO_CACHE,

    QLP_SEQUENCE_OPTION_RESTART_WITH,
    
    QLP_SEQUENCE_OPTION_MAX
} qlpSequenceOptionType;


/**
 * @brief type of identity generation 
 */
typedef enum qlpIdentityType
{
    QLP_IDENTITY_INVALID = 0,
    QLP_IDENTITY_ALWAYS,
    QLP_IDENTITY_BY_DEFAULT,
} qlpIdentityType;


/**
 * @brief type of hint 
 */
typedef enum qlpHintType
{
    QLP_HINT_INVALID = 0,

    QLP_HINT_TABLE,
    QLP_HINT_INDEX,
    QLP_HINT_JOIN_ORDER,
    QLP_HINT_JOIN_OPER,
    QLP_HINT_QUERY_TRANSFORM,
    QLP_HINT_OTHER,
} qlpHintType;


/**
 * @brief type of table hint
 */
typedef enum qlpTableHintType
{
    QLP_TABLE_HINT_INVALID = 0,

    QLP_TABLE_HINT_FULL,
    QLP_TABLE_HINT_HASH,
    QLP_TABLE_HINT_ROWID
} qlpTableHintType;


/**
 * @brief type of index hint
 */
typedef enum qlpIndexHintType
{
    QLP_INDEX_HINT_INVALID = 0,

    QLP_INDEX_HINT_INDEX,
    QLP_INDEX_HINT_INDEX_ASC,
    QLP_INDEX_HINT_INDEX_DESC,
    QLP_INDEX_HINT_INDEX_COMBINE,
    QLP_INDEX_HINT_NO_INDEX,
    QLP_INDEX_HINT_IN_KEY_RANGE
} qlpIndexHintType;


/**
 * @brief type of join opder hint
 */
typedef enum qlpJoinOrderHintType
{
    QLP_JOIN_ORDER_HINT_INVALID = 0,

    QLP_JOIN_ORDER_HINT_ORDERED,
    QLP_JOIN_ORDER_HINT_ORDERING,
    QLP_JOIN_ORDER_HINT_LEADING
} qlpJoinOrderHintType;


/**
 * @brief type of join operation hint
 */
typedef enum qlpJoinOperHintType
{
    QLP_JOIN_OPER_HINT_INVALID = 0,

    QLP_JOIN_OPER_HINT_HASH,
    QLP_JOIN_OPER_HINT_INL,
    QLP_JOIN_OPER_HINT_MERGE,
    QLP_JOIN_OPER_HINT_NL
} qlpJoinOperHintType;


/**
 * @brief type of query transformation hint
 */
typedef enum qlpQueryTransformHintType
{
    QLP_QUERY_TRANSFORM_HINT_INVALID = 0,

    QLP_QUERY_TRANSFORM_HINT_UNNEST,
    QLP_QUERY_TRANSFORM_HINT_NO_UNNEST,
    QLP_QUERY_TRANSFORM_HINT_NL_ISJ,
    QLP_QUERY_TRANSFORM_HINT_NL_SJ,
    QLP_QUERY_TRANSFORM_HINT_NL_AJ,
    QLP_QUERY_TRANSFORM_HINT_MERGE_SJ,
    QLP_QUERY_TRANSFORM_HINT_MERGE_AJ,
    QLP_QUERY_TRANSFORM_HINT_HASH_ISJ,
    QLP_QUERY_TRANSFORM_HINT_HASH_SJ,
    QLP_QUERY_TRANSFORM_HINT_HASH_AJ,
    QLP_QUERY_TRANSFORM_HINT_NO_QUERY_TRANSFORMATION
} qlpQueryTransformHintType;


/**
 * @brief type of other hint
 */
typedef enum qlpOtherHintType
{
    QLP_OTHER_HINT_INVALID = 0,

    QLP_OTHER_HINT_PUSH_PRED,
    QLP_OTHER_HINT_PUSH_SUBQ,
    QLP_OTHER_HINT_USE_DISTINCT_HASH,
    QLP_OTHER_HINT_USE_GROUP_HASH
} qlpOtherHintType;


/**
 * @brief Between
 */
typedef enum qlpBetweenType
{
    QLP_BETWEEN_TYPE_INVALID = 0,
    QLP_BETWEEN_TYPE_BETWEEN_SYMMETRIC,
    QLP_BETWEEN_TYPE_BETWEEN_ASYMMETRIC,
    QLP_BETWEEN_TYPE_NOT_BETWEEN_SYMMETRIC,
    QLP_BETWEEN_TYPE_NOT_BETWEEN_ASYMMETRIC,
} qlpBetweenType;

/*******************************************************************************
 * STRUCTURES 
 ******************************************************************************/


/**
 * @brief constant value : QLP_CONSTANT_VALUE_TYPE
 */
struct qlpConstantValue
{
    qllNodeType     mType;           /**< node type */
    stlInt32        mNodePos;        /**< node position */
    
    qlpValue      * mValue;          /**< constant value (value) */
};


/**
 * @brief column definition : QLP_COLUMN_DEF_TYPE
 */
struct qlpColumnDef
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    stlInt32           mColumnNamePos;  /**< column name position */

    stlChar          * mColumnName;     /**< column name */
    qlpTypeName      * mTypeName;       /**< type info in definition (parsrtree) */
    qlpList          * mConstraints;    /**< other constraints : list of qlpConstraint (parsetree) */

    qlpColDefSecond  * mDefSecond;      /**< column definition second information */
/*
  column_inherit_times
  is_local
  is_from_type
  storage??
  collate
*/  
};


/**
 * @brief constraint info : QLP_CONSTRAINT_TYPE
 */

struct qlpConstraint
{
    qllNodeType           mType;               /**< node type */
    stlInt32              mNodePos;            /**< node position */

    stlInt32              mConstTypePos;       /**< position : type of constraint */
    stlInt32              mFKMatchTypePos;     /**< position : MATCH option of FK */
    stlInt32              mFKUpdateActionPos;  /**< position : UPDATE trigger action */
    stlInt32              mFKDeleteActionPos;  /**< position : DELETE trigger action */

    /* constraint type */
    qlpConstraintType     mConstType;          /**< type of constraint :
                                                * UNIQUE, PK, FK,               => support
                                                * NULL, NOTNULL,                => support
                                                * CHECK, DEFAULT, EXCLUSION     => unsupport
                                                */
    /* constraint characteristics */
    qlpConstraintAttr   * mConstAttr;          /**< init_deferred / deferrable / enforce */
    
    /* common for all constraints */
    qlpObjectName       * mName;               /**< constraint name */

    /* UNIQUE, PRIMARY Key constraints */
    qlpList             * mUniqueFields;       /**< referenced column names : string value (value) */

    /* FOREIGN KEY constraints */
    qlpRelInfo          * mPKTable;            /**< PK table (primitive) */
    qlpList             * mPKFields;           /**< corresponding attributes in PK table
                                                * : string value (value) */
    qlpList             * mFKFields;           /**< attributes of FK : string value (value) */
    qlpFKMatchType        mFKMatchType;        /**< MATCH option of FK */
    qlpFKAction           mFKUpdateAction;     /**< UPDATE trigger action */
    qlpFKAction           mFKDeleteAction;     /**< DELETE trigger action */

    /* CHECK constraint */
    qllNode             * mRawExpr;            /**< expression : untransformed parse tree */

    /**
     * Key Constraint Index Option
     */
    
    qlpValue            * mIndexName;          /**< index name */
    qlpIndexAttr          mIndexAttr;          /**< index attribute */
    qlpObjectName       * mIndexSpace;         /**< tablespace name for index */
    
/*
  expr_for_default_or_check
  field_of_exclusion_constraints
  with_options
  index_for_PK_or_UNIQUE
  skip_validation
*/  
};


/**
 * @brief type name : QLP_TYPE_NAME_TYPE
 */
struct qlpTypeName
{
    qllNodeType            mType;             /**< node type */
    stlInt32               mNodePos;          /**< node position */
    stlInt32               mNodeLen;          /**< node length */

    stlInt32               mDBTypePos;            /**< DB data type position */
    stlInt32               mCharacterSetPos;      /**< character set position */
    stlInt32               mNumericPrecRadixPos;  /**< numeric precision radix position */

    dtlDataType            mDBType;           /**< DB data type */

    /* character */
    qlpCharLength        * mStringLength;     /**< character string length */
    stlChar              * mCharacterSet;     /**< character set */
    
    /* character */
    qlpValue             * mBinaryLength;     /**< bianry string length */

    /* numeric */
    dtlNumericPrecRadix    mNumericPrecRadix; /**< numeric precision radix */
    qlpValue             * mNumericPrecision; /**< numeric precision */
    qlpValue             * mNumericScale;     /**< numeric scale */

    /* date time */
    qlpValue             * mFractionalSecondPrec;  /**< fractional second precision : float */
    
    /* date time & interval */
    qllNode              * mQualifier;        /**< Qualifier */
    
/*
  qualified_name
  set_of
  percentage_type
  type_modifier_expression : precision & scale ...
  array_bound
*/
};


/**
 * @brief Qualifier
 */
struct qlpQualifier
{
    qllNodeType             mType;              /**< node type */
    stlInt32                mNodePos;           /**< node position */

    qlpIntvPriFieldDesc   * mStartFieldDesc;    /**< start field */
    qlpIntvPriFieldDesc   * mEndFieldDesc;      /**< end field */
    qlpIntvPriFieldDesc   * mSingleFieldDesc;   /**< single field */
};


/**
 * @brief Interval Primary datetime field description
 */
struct qlpIntvPriFieldDesc
{
    qllNodeType         mType;                          /**< node type */
    stlInt32            mNodePos;                       /**< node position */

    qlpValue          * mPrimaryFieldType;              /**< primary field type (dtlIntervalPrimaryField 사용) */
    qlpValue          * mLeadingFieldPrecision;         /**< interval leading field precision */
    qlpValue          * mFractionalSecondsPrecision;    /**< interval fractional seconds precision */
};


/**
 * @brief CAST expression : QLP_TYPECAST_TYPE
 */
struct qlpTypeCast
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
    
    qllNode          * mExpr;           /**< the expression being casted */
    qlpTypeName      * mTypeName;       /**< type info in definition (parsrtree) */
};


/**
 * @brief Element Definition : QLP_DEF_ELEM_TYPE
 */

struct qlpDefElem
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    stlInt32           mElementTypePos; /**< node position position */

    stlInt32           mElementType;    /**< kind of definition element */
    qllNode          * mArgument;       /**< qlpValue or qlpTypeName */
};


/**
 * @brief Target for SELECT/INSERT/UPDATE : QLP_TARGET_TYPE
 */
struct qlpTarget
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
    stlInt32           mNodeLen;        /**< node length */

    stlInt32           mNamePos;        /**< position of target name : SELECT(AS), INSERT, UPDATE */

    stlChar          * mName;           /**< target name : SELECT(AS), INSERT, UPDATE */
    qlpList          * mDestSubscripts; /**< destination subscripts (???) : INSERT, UPDATE */
    qlpObjectName    * mRelName;        /**< relation name : SELECT(relation_name.*) */
    qlpList          * mColumnNameList; /**< column name list : SELECT(AS) */
    qllNode          * mExpr;           /**< value expression : SELECT, UPDATE
                                         *   SELECT : target column 
                                         *   UPDATE : set column = VALUE
                                         *                        ^^^^^^^   */    
    qlpColumnRef     * mUpdateColumn;   /**< update column : UPDATE set column = VALUE
                                         *                             ^^^^^^^^         */
};


/**
 * @brief Multiple Set Column for UPDATE : QLP_ROW_TARGET_TYPE
 */
struct qlpRowTarget
{
    qllNodeType       mType;            /**< node type */
    stlInt32          mNodePos;         /**< node position */

    qlpList         * mSetColumnList;   /**< update set column list */
    qllNode         * mValueExpr;       /**< Row SubQuery Value Expression */
};


/**
 * @brief All Columns : QLP_ASTERISK_TYPE
 */
struct qlpAsterisk
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */
};


/**
 * @brief Hint : QLP_HINT_TYPE
 */
struct qlpHint
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    qlpHintType        mHintType;       /**< hint type */
    stlBool            mIsUsed;         /**< 실제 사용되는지 체크를 위해 사용 */
};


/**
 * @brief Column Reference : QLP_COLUMN_REF_TYPE
 */
struct qlpColumnRef
{
    qllNodeType      mType;           /**< node type */
    stlInt32         mNodePos;        /**< node position */

    qlpColumnName  * mName;           /**< column name */
    stlBool          mIsSetOuterMark; /**< Outer Join Operator (+) 설정 여부 */
};


/**
 * @brief index scan : QLP_INDEX_SCAN_TYPE
 */
struct qlpIndexScan
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    qlpObjectName    * mName;           /**< index name : qlpValue (string) */

    /*
     * index info : attr_list, predicate_list, unique, ...
     */
};


/**
 * @brief index element : QLP_INDEX_ELEMENT_TYPE
 */
struct qlpIndexElement
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    qlpValue         * mIsAsc;          /**< is ascending sort order ? */
    qlpValue         * mIsNullsFirst;   /**< is nulls first ? */
    qlpValue         * mIsNullable;     /**< nullable ? */
    qlpValue         * mColumnName;     /**< index column name */
};


/**
 * @brief sequence definition options : QLP_SEQUENCE_OPTION_TYPE
 */
struct qlpSequenceOption
{
    qllNodeType    mType;           /**< node type */
    stlInt32       mNodePos;        /**< node position */

    qlpValue     * mStartWith;      /**< start with */
    qlpValue     * mIncrement;      /**< increment size */
    qlpValue     * mMaxValue;       /**< max value */
    qlpValue     * mMinValue;       /**< min value */
    qlpValue     * mCache;          /**< cache size */
    qlpValue     * mNoCache;        /**< is no cache ? */
    qlpValue     * mCycle;          /**< is cycle */

    stlInt32       mRestartNodePos; /**< restart node position */
    qlpValue     * mRestartWith;    /**< restart with */
    
    stlBool        mIsSetting[ QLP_SEQUENCE_OPTION_MAX ];
};



/**
 * @brief value expression : QLL_VALUE_EXPR_TYPE
 */
struct qlpValueExpr
{
    qllNodeType    mType;           /**< node type */
    stlInt32       mNodePos;        /**< node position */
    stlInt32       mNodeLen;        /**< node length */

    qllNode      * mExpr;           /**< expression */
};

struct qlpFetchOrientation
{
    qllNodeType    mType;           /**< node type */
    stlInt32       mNodePos;        /**< node position */

    qllFetchOrientation   mFetchOrient;
    qllNode             * mFetchPosition;
}; 

/**
 * @brief stmt expression : QLP_BETWEEN_TYPE
 */
struct qlpBetween
{
    qlpListElement    * mElem1;           /**< expr1 node value */
    qlpListElement    * mElem2;           /**< expr2 node value */

    qlpBetweenType      mType;            /**< between func type */
    knlBuiltInFunc      mFuncID;          /**< between func ID */
};

/*******************************************************************************
 * FUNCTIONS 
 ******************************************************************************/

stlStatus qlpMakeNullValueNode( qlpConstantValue  ** aValueNode,
                                knlRegionMem       * aMemMgr );





// QLP_A_EXPR_TYPE,
// QLP_COLUMNREF_TYPE,
// QLP_PARAMREF_TYPE,
// QLP_FUNCCALL_TYPE,
// QLP_A_INDICES_TYPE,
// QLP_A_INDIRECTION_TYPE,
// QLP_A_ARRAYEXPR_TYPE,
// QLP_SORTBY_TYPE,
// QLP_WINDOWDEF_TYPE,
// QLP_RANGESUBSELECT_TYPE,
// QLP_RANGEFUNCTION_TYPE,
// QLP_TYPENAME_TYPE,
// QLP_RANGETBLENTRY_TYPE,
// QLP_SORTGROUPCLAUSE_TYPE,
// QLP_WINDOWCLAUSE_TYPE,
// QLP_PRIVGRANTEE_TYPE,
// QLP_FUNCWITHARGS_TYPE,
// QLP_ACCESSPRIV_TYPE,
// QLP_CREATEOPCLASSITEM_TYPE,
// QLP_INHRELATION_TYPE,
// QLP_FUNCTIONPARAMETER_TYPE,
// QLP_LOCKINGCLAUSE_TYPE,
// QLP_ROWMARKCLAUSE_TYPE,
// QLP_XMLSERIALIZE_TYPE,
// QLP_WITHCLAUSE_TYPE,
// QLP_COMMONTABLEEXPR_TYPE,


        
/** @} */

#endif /* _QLPNODEPARSETREE_H_ */
