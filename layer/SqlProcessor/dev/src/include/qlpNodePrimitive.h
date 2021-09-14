/*******************************************************************************
 * qlpNodePrimitive.h
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

#ifndef _QLPNODEPRIMITIVE_H_
#define _QLPNODEPRIMITIVE_H_ 1

/**
 * @file qlpNodePrimitive.h
 * @brief SQL Processor Layer Primitive Nodes 
 */

/**
 * @defgroup qlpNodePrimitive Primitive Nodes 
 * @ingroup qlpNode
 * @{
 */


/*******************************************************************************
 * DEFINITIONS 
 ******************************************************************************/

/**
 * @brief table 종류
 */
typedef enum qlpTableScope
{
    QLP_TABLE_SCOPE_INVALID = 0,
    QLP_TABLE_SCOPE_EMPTY,
    QLP_TABLE_SCOPE_GLOBAL_TEMPORARY,
    QLP_TABLE_SCOPE_LOCAL_TEMPORARY
} qlpTableScope;


/**
 * @brief parameter 종류
 */
typedef enum qlpParamType
{
    QLP_PARAM_TYPE_INVALID = 0,
    QLP_PARAM_TYPE_HOST,     /**< sql parameter : ":v1" */
//    QLP_PARAM_TYPE_SQL,      /**< sql parameter : "@xxx" */
    QLP_PARAM_TYPE_DYNAMIC   /**< dynammic parameter : "?" */
} qlpParamType;

/**
 * @brief join 종류
 */
typedef enum qlpJoinType
{
    QLP_JOIN_TYPE_INVALID = 0,
    QLP_JOIN_TYPE_NONE,              /**< comma list join (default, basic) */
    QLP_JOIN_TYPE_CROSS,             /**< cross join */
    QLP_JOIN_TYPE_INNER,             /**< inner join */

    QLP_JOIN_TYPE_LEFT_OUTER,        /**< left outer join */
    QLP_JOIN_TYPE_RIGHT_OUTER,       /**< right outer join */
    QLP_JOIN_TYPE_FULL_OUTER,        /**< full outer join */

    QLP_JOIN_TYPE_LEFT_SEMI,         /**< left semi join */
    QLP_JOIN_TYPE_RIGHT_SEMI,        /**< right semi join */
    
    QLP_JOIN_TYPE_LEFT_ANTI_SEMI,    /**< left anti-semi join */
    QLP_JOIN_TYPE_RIGHT_ANTI_SEMI,   /**< right anti-semi join */
} qlpJoinType;

/**
 * @brief set operation type
 */
typedef enum qlpSetType
{
    QLP_SET_TYPE_INVALID = 0,
    QLP_SET_TYPE_UNION,
    QLP_SET_TYPE_EXCEPT,
    QLP_SET_TYPE_INTERSECT
} qlpSetType;

/**
 * @brief quantifier
 */
typedef enum qlpQuantifier
{
    QLP_QUANTIFIER_INVALID = 0,
    QLP_QUANTIFIER_ALL,
    QLP_QUANTIFIER_ANY
} qlpQuantifier;

/**
 * @brief set quantifier
 */
typedef enum qlpSetQuantifier
{
    QLP_SET_QUANTIFIER_INVALID = 0,
    QLP_SET_QUANTIFIER_ALL,
    QLP_SET_QUANTIFIER_DISTINCT
} qlpSetQuantifier;


/**
 * @brief updatability
 */
typedef enum qlpUpdatableMode
{
    QLP_UPDATABILITY_NA = 0,
    QLP_UPDATABILITY_FOR_READ_ONLY,
    QLP_UPDATABILITY_FOR_UPDATE,
} qlpUpdatableMode;

typedef enum qlpForUpdateLockWaitMode
{
    QLP_FOR_UPDATE_LOCK_NA = 0,
    QLP_FOR_UPDATE_LOCK_WAIT,
    QLP_FOR_UPDATE_LOCK_NOWAIT,
    QLP_FOR_UPDATE_LOCK_SKIP_LOCKED
} qlpForUpdateLockWaitMode;
    

/**
 * @brief grouping type
 */
typedef enum qlpGroupingType
{
    QLP_GROUPING_TYPE_NA = 0,
    QLP_GROUPING_TYPE_ORDINARY,
    QLP_GROUPING_TYPE_GROUPING_SET,
    QLP_GROUPING_TYPE_EMPTY,
    QLP_GROUPING_TYPE_ROLL_UP,
    QLP_GROUPING_TYPE_CUBE
} qlpGroupingType;


/* plsql param 
     QLP_PARAM_TYPE_EXTERN,
     QLP_PARAM_TYPE_EXEC   
*/

/*******************************************************************************
 * STRUCTURES 
 ******************************************************************************/


/**
 * @brief query set node : QLL_QUERY_SET_NODE_TYPE
 */
struct qlpQuerySetNode
{
    qllNodeType           mType;                /**< node type */
    stlInt32              mNodePos;             /**< node position */

    qlpSetType            mSetType;             /**< set operation type */
    qlpSetQuantifier      mSetQuantifier;       /**< set quantifier */
    qllNode             * mLeftQueryNode;       /**< left query node */
    qllNode             * mRightQueryNode;      /**< right query node */
    void                * mCorrespondingSpec;   /**< corresponding specification */

    void                * mWith;                /**< WITH clause (TODO with절 구현시 사용) */
    qlpList             * mOrderBy;             /**< ORDER BY clause */
    qllNode             * mResultSkip;          /**< OFFSET skip_count */
    qllNode             * mResultLimit;         /**< LIMIT fetch_count */
};


/**
 * @brief query specification node : QLL_QUERY_SPEC_NODE_TYPE
 */
struct qlpQuerySpecNode
{
    qllNodeType           mType;            /**< node type */
    stlInt32              mNodePos;         /**< node position */

    qlpList             * mHints;           /**< list : qlpHint */
    qlpSetQuantifier      mSetQuantifier;   /**< Set Quantifier */
    qlpList             * mTargets;         /**< list : qlpTarget */
    qllNode             * mFrom;            /**< FROM clause */
    qllNode             * mWhere;           /**< WHERE qualification */
    qllNode             * mGroupBy;         /**< GROUP BY Clause */
    qllNode             * mHaving;          /**< HAVING Clause */
    qllNode             * mWindow;          /**< WINDOW Clause */

    void                * mWith;            /**< WITH clause (TODO with절 구현시 사용) */
    qlpList             * mOrderBy;         /**< ORDER BY clause */
    qllNode             * mResultSkip;      /**< OFFSET skip_count */
    qllNode             * mResultLimit;     /**< LIMIT fetch_count */
};


/**
 * @brief join type info : QLL_JOIN_TYPE_INFO_TYPE
 */
struct qlpJoinTypeInfo
{
    qllNodeType     mType;           /**< node type */
    stlInt32        mNodePos;        /**< node position */

    qlpJoinType     mJoinType;       /**< join type */
};


/**
 * @brief join specification : QLL_JOIN_SPECIFICATION_TYPE
 */
struct qlpJoinSpecification
{
    qllNodeType     mType;           /**< node type */
    stlInt32        mNodePos;        /**< node position */

    qlpList       * mJoinCondition;  /**< join condition */
    qlpList       * mNamedColumns;   /**< named columns */
};


/**
 * @brief base table node : QLL_BASE_TABLE_NODE_TYPE
 */
struct qlpBaseTableNode
{
    qllNodeType     mType;           /**< node type */
    stlInt32        mNodePos;        /**< node position */

    qlpObjectName * mName;           /**< relation name */
    qlpValue      * mDumpOpt;        /**< Dump option string for Dump Table */
    qlpAlias      * mAlias;          /**< table alias : qlpAlias (primitive) */
};


/**
 * @brief sub table node : QLL_SUB_TABLE_NODE_TYPE
 */
struct qlpSubTableNode
{
    qllNodeType     mType;           /**< node type */
    stlInt32        mNodePos;        /**< node position */

    qllNode       * mQueryNode;      /**< query node */
    qlpAlias      * mAlias;          /**< table alias : qlpAlias (primitive) */

    qlpList       * mColAliasList;   /**< column alias list */
};


/**
 * @brief join table node : QLL_JOIN_TABLE_NODE_TYPE
 */
struct qlpJoinTableNode
{
    qllNodeType               mType;            /**< node type */
    stlInt32                  mNodePos;         /**< node position */

    stlBool                   mIsNatural;       /**< is natural join */
    qlpJoinType               mJoinType;        /**< join type */
    qllNode                 * mLeftTableNode;   /**< left table node */
    qllNode                 * mRightTableNode;  /**< left table node */
    qlpJoinSpecification    * mJoinSpec;        /**< join specification */
};


/**
 * @brief relation info : QLL_REL_INFO_TYPE
 */
struct qlpRelInfo
{
    qllNodeType     mType;           /**< node type */
    stlInt32        mNodePos;        /**< node position */

    qlpObjectName * mName;           /**< relation name */
    qlpAlias      * mAlias;          /**< table alias : qlpAlias (primitive) */

    stlBool         mIsUsed;         /**< Rel Info 사용 여부 (Rule Base Hint시에만 사용한다. */
/*
  is_temp_rel
  inherit_option
*/  
};


/**
 * @brief relation or column's alias : QLL_ALIAS_TYPE
 */
struct qlpAlias
{
    qllNodeType     mType;           /**< node type */
    stlInt32        mNodePos;        /**< node position */

    qlpValue      * mAliasName;      /**< alias name */
    qlpList       * mColumnNames;    /**< rename columns : list of qlpValue (value)
                                      * column alias name을 qlpValue node에 string으로 저장
                                      * alias name이 없으면 ""로 표시 => RTE와 연관
                                      */
    qllNode       * mBaseObject;     /**< base object node */
};


/**
 * @brief parameter : QLL_PARAM_TYPE
 */
struct qlpParameter
{
    qllNodeType     mType;           /**< node type */
    stlInt32        mNodePos;        /**< node position */

    stlInt32        mParamTypePos;   /**< parameter type position */

    qlpParamType    mParamType;      /**< parameter type */
    knlBindType     mInOutType;      /**< IN or OUT or INOUT */
    stlInt32        mParamId;        /**< parameter ID */
    qlpValue      * mHostName;       /**< parameter name */
    qlpValue      * mIndicatorName;  /**< indicator name */
};


/**
 * @brief INTO clause : QLL_INTO_CLAUSE_TYPE
 * @remark for SELECT INTO
 */
struct qlpIntoClause
{
    qllNodeType     mType;           /**< node type */
    stlInt32        mNodePos;        /**< node position */

    qlpList       * mParameters;     /**< qlpList : qlpParameter  */
};

/**
 * @brief INSERT 또는 DELETE 구문을 위한 RETURNING
 */
struct qlpInsDelReturn
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    qlpList          * mReturnTarget;   /**< RETURNING : qlpTarget */
    qlpIntoClause    * mIntoTarget;     /**< INTO  */
};

/**
 * @brief UPDATE 구문을 위한 RETURNING
 */
struct qlpUpdateReturn
{
    qllNodeType        mType;           /**< node type */
    stlInt32           mNodePos;        /**< node position */

    stlBool            mIsReturnNew;    /**< RETURNING NEW or RETURNING OLD */
    qlpList          * mReturnTarget;   /**< RETURNING : qlpTarget */
    qlpIntoClause    * mIntoTarget;     /**< INTO  */
};

/**
 * @brief function : QLL_FUNCTION_TYPE
 */
struct qlpFunction
{
    qllNodeType               mType;           /**< node type */
    stlInt32                  mNodePos;        /**< node position */
    stlInt32                  mNodeLen;        /**< node length */

    knlBuiltInFunc            mFuncId;         /**< function ID */
    qlpList                 * mArgs;           /**< function argument list */
};


/**
 * @brief pseudo column : QLL_PSEUDO_COLUMN_TYPE
 */
struct qlpPseudoCol
{
    qllNodeType               mType;           /**< node type */
    stlInt32                  mNodePos;        /**< node position */
    stlInt32                  mNodeLen;        /**< node length */

    knlPseudoCol              mPseudoId;       /**< pseudo column ID */
    qlpList                 * mArgs;           /**< function argument list */
};


/**
 * @brief aggregation : QLL_AGGREGATION_TYPE
 */
struct qlpAggregation
{
    qllNodeType               mType;           /**< node type */
    stlInt32                  mNodePos;        /**< node position */
    stlInt32                  mNodeLen;        /**< node length */

    knlBuiltInAggrFunc        mAggrId;         /**< aggregation ID */
    qlpSetQuantifier          mSetQuantifier;  /**< set quantifier */
    qlpList                 * mArgs;           /**< aggregation argument list */
    qllNode                 * mFilter;         /**< filter */
};


/**
 * @brief default node : QLL_DEFAULT_TYPE
 */
struct qlpDefault
{
    qllNodeType           mType;           /**< node type */
    stlInt32              mNodePos;        /**< node position */

    stlInt32              mIsRowValuePos;  /**< is row value */

    stlBool               mIsRowValue;     /**< is row value */
};


/**
 * @brief FOR UPDATE lock wait
 */
struct qlpForUpdateLockWait
{
    qllNodeType                mType;              /**< node type */
    stlInt32                   mNodePos;           /**< node position */
    
    qlpForUpdateLockWaitMode   mLockWaitMode;      /**< lock wait mode */
    qlpValue                 * mWaitSecond;        /**< WAIT second */
};
    
/**
 * @brief FOR UPDATE or FOR READ ONLY
 */
struct qlpUpdatableClause
{
    qllNodeType                mType;           /**< node type */
    stlInt32                   mNodePos;        /**< node position */

    qlpUpdatableMode           mUpdatableMode;     /**< Updatability */
    qlpList                  * mUpdateColumnList;  /**< FOR UPDATE OF column list */

    qlpForUpdateLockWait     * mLockWait;
};


/**
 * @brief comment object : QLL_COMMENT_OBJECT_TYPE
 */
struct qlpCommentObject
{
    qllNodeType           mType;           /**< node type */
    stlInt32              mNodePos;        /**< node position */

    ellObjectType         mObjectType;     /**< object type */

    qlpValue            * mNonSchemaObjectName;  /**< non-schema object */
    qlpObjectName       * mSchemaObjectName;     /**< schema object */
    qlpColumnName       * mColumnObjectName;     /**< column object */
};


/**
 * @brief Index Attribute
 */
struct qlpIndexAttr
{
    /* segment attribute values */
    qlpSize       * mSegInitialSize;
    qlpSize       * mSegNextSize;
    qlpSize       * mSegMinSize;
    qlpSize       * mSegMaxSize;

    /* physical attribute values */
    qlpValue      * mIniTrans;
    qlpValue      * mMaxTrans;
    qlpValue      * mPctFree;

    /* index attribute values */
    qlpValue      * mLogging;
    qlpValue      * mParallel;
};


/**
 * @brief DROP constraint object 유형
 */
typedef enum qlpDropConstType
{
    QLP_DROP_CONST_NA = 0,
    
    QLP_DROP_CONST_NAME,          /**< CONSTRAINT name */
    QLP_DROP_CONST_PRIMARY_KEY,   /**< PRIAMRY KEY */
    QLP_DROP_CONST_UNIQUE,        /**< UNIQUE (columns) */

    QLP_DROP_CONST_MAX
} qlpDropConstType;


/**
 * @brief DROP constraint object
 */
struct qlpDropConstObject
{
    qllNodeType             mType;              /**< node type */
    stlInt32                mNodePos;           /**< node position */
    
    qlpDropConstType        mDropConstType;     /**< Drop Constraint Type */
    qlpObjectName         * mConstraintName;    /**< CONSTRAINT name */
    qlpList               * mUniqueColumnList;  /**< UNIQUE( column list ) */
};


/**
 * @brief Order By Node
 */
struct qlpOrderBy
{
    qllNodeType             mType;          /**< node type */
    stlInt32                mNodePos;       /**< node position */

    qllNode               * mSortKey;       /**< Sort Key */
    qlpValue              * mSortOrder;     /**< Sort Ordering specification */
    qlpValue              * mNullOrder;     /**< Null Ordering */
};


/**
 * @brief Group By Node
 */
struct qlpGroupBy
{
    qllNodeType             mType;             /**< node type */
    stlInt32                mNodePos;          /**< node position */

    qlpList               * mGroupingElemList; /**< Grouping Element List */
};


/**
 * @brief Grouping Set Node
 */
struct qlpGroupingElem
{
    qllNodeType             mType;            /**< node type */
    stlInt32                mNodePos;         /**< node position */

    qlpGroupingType         mGroupingType;    /**< Grouping Type */
    qlpList               * mGroupingElem;    /**< Grouping Element */
};


/**
 * @brief Having Node
 */
struct qlpHaving
{
    qllNodeType             mType;          /**< node type */
    stlInt32                mNodePos;       /**< node position */

    qllNode               * mCondition;     /**< haviing condition */
};


/**
 * @brief List Function Node
 */
struct qlpListFunc
{
    qllNodeType             mType;          /**< node type */
    stlInt32                mNodePos;       /**< node position */
    stlInt32                mNodeLen;       /**< node length */

    stlInt32                mOperatorPos;   /**< operator position */
    knlBuiltInFunc          mFuncId;        /**< node function */
    stlBool                 mWithSubQuery;  /**< rigth operand 가 multiple row subquery 인지 여부 */
    qlpList               * mArgs;
};
    

/**
 * @brief List Column Node
 */
struct qlpListCol
{
    qllNodeType             mType;          /**< node type */
    
    stlInt32                mNodePos;       /**< node position */
    knlBuiltInFunc          mFuncId;        /**< node function */
    dtlListPredicate        mPredicate;     /**< List Function, Left column, Right column predicate */
    stlUInt16               mArgCount;
    qlpList               * mArgs;          /**< List Column Row  */
};

/**
 * @brief Row Expr Node
 */
struct qlpRowExpr
{
    qllNodeType          mType;          /**< node type */

    stlInt32             mNodePos;       /**< node position */
    stlUInt16            mArgCount;
    qlpList            * mArgs;          /**< value list  */
};
    
/**
 * @brief Case Expression
 *
 *   <BR> CASE specification ( simple case ) : 
 *
 *   <BR> 예1) CASE i1 WHEN 1 THEN 'one'
 *   <BR>             -----------------     
 *   <BR>             WHEN 2 THEN 'two'
 *   <BR>             -----------------
 *   <BR>             ELSE NULL
 *   <BR>     END
 *
 *   <BR> 예2) CASE (i1, i2, i3) WHEN (1, 2, 3) THEN 'one'
 *   <BR>                        -------------------------     
 *   <BR>                        WHEN (2, 3, 4) THEN 'two'
 *   <BR>                        -------------------------
 *   <BR>                        ELSE NULL
 *   <BR>     END
 *
 *
 *   <BR> CASE specification ( searched case ) :
 *   
 *   <BR> 예1) CASE WHEN i1 = 1 THEN 'one'
 *   <BR>           ----------------------
 *   <BR>           WHEN i1 = 2 THEN 'two'
 *   <BR>           ----------------------
 *   <BR>           ELSE NULL
 *   <BR>     END
 *
 *   <BR> 예2) CASE WHEN (i1, i2) = (1, 2) THEN 'one'
 *   <BR>           ---------------------------------
 *   <BR>           WHEN (i1, i2) = (2, 3) THEN 'two'
 *   <BR>           ---------------------------------
 *   <BR>           ELSE NULL
 *   <BR>     END
 *   
 */

struct qlpCaseExpr
{
    qllNodeType       mType;           /**< node type : QLL_CASE_SIMPLE_EXPR
                                        *               QLL_CASE_SEARCHED_EXPR
                                        *               QLL_NULLIF_EXPR
                                        *               QLL_COALESCE_EXPR
                                        *               QLL_DECODE_EXPR
                                        *               QLL_CASE2_EXPR
                                        *               QLL_NVL_EXPR
                                        *               QLL_NVL2_EXPR */
    stlInt32          mNodePos;        /**< node position */
    stlInt32          mNodeLen;        /**< node length */
    
    knlBuiltInFunc    mFuncId;         /**< function ID
                                        *   CASE, NULLIF, COALESCE, DECODE, CASE2, NVL, NVL2 */

    qlpList         * mExpr;           /**< simple case   : NULL
                                        *   searched case : NULL
                                        *   nullif, coalesce, nvl, nvl2, decode, decode2 : expr list */        
    
    qlpList         * mWhenClause;     /**< When ... Then */
    qllNode         * mDefResult;      /**< ELSE  */
};

struct qlpCaseWhenClause
{
    qllNode       * mWhen;           /**< WHEN  */
    qllNode       * mThen;           /**< THEN  */
};



/*******************************************************************************
 * FUNCTIONS 
 ******************************************************************************/



    


    // QLL_EXPR_TYPE,    // QLL_VAR_TYPE,
    // QLL_CONST_TYPE,
    // QLL_AGGREF_TYPE,
    // QLL_WINDOWFUNC_TYPE,
    // QLL_ARRAYREF_TYPE,
    // QLL_FUNCEXPR_TYPE,
    // QLL_NAMEDARGEXPR_TYPE,
    // QLL_OPEXPR_TYPE,
    // QLL_DISTINCTEXPR_TYPE,
    // QLL_SCALARARRAYOPEXPR_TYPE,
    // QLL_BOOLEXPR_TYPE,
    // QLL_SUBLINK_TYPE,
    // QLL_SUBPLAN_TYPE,
    // QLL_ALTERNATIVESUBPLAN_TYPE,
    // QLL_FIELDSELECT_TYPE,
    // QLL_FIELDSTORE_TYPE,
    // QLL_RELABELTYPE_TYPE,
    // QLL_COERCEVIAIO_TYPE,
    // QLL_ARRAYCOERCEEXPR_TYPE,
    // QLL_CONVERTROWTYPEEXPR_TYPE,
    // QLL_CASEEXPR_TYPE,
    // QLL_CASEWHEN_TYPE,
    // QLL_CASETESTEXPR_TYPE,
    // QLL_ARRAYEXPR_TYPE,
    // QLL_ROWEXPR_TYPE,
    // QLL_ROWCOMPAREEXPR_TYPE,
    // QLL_COALESCEEXPR_TYPE,
    // QLL_MINMAXEXPR_TYPE,
    // QLL_XMLEXPR_TYPE,
    // QLL_NULLIFEXPR_TYPE,
    // QLL_NULLTEST_TYPE,
    // QLL_COERCETODOMAIN_TYPE,
    // QLL_COERCETODOMAINVALUE_TYPE,
    // QLL_SETTODEFAULT_TYPE,
    // QLL_CURRENTOFEXPR_TYPE,
    // QLL_TARGETENTRY_TYPE,
    // QLL_RANGETBLREF_TYPE,
    // QLL_JOINEXPR_TYPE,
    // QLL_FROMEXPR_TYPE,
    // QLL_INTOCLAUSE_TYPE,

        
/** @} */

#endif /* _QLPNODEPRIMITIVE_H_ */
