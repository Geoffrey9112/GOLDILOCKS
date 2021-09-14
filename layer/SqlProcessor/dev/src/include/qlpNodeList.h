/*******************************************************************************
 * qlpNodeList.h
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

#ifndef _QLPNODELIST_H_
#define _QLPNODELIST_H_ 1

/**
 * @file qlpNodeList.h
 * @brief SQL Processor Layer List Nodes 
 */


/**
 * @defgroup qlpNodeList List Nodes
 * @ingroup qlpNode
 * @{
 */


/*******************************************************************************
 * DEFINITIONS 
 ******************************************************************************/



/*******************************************************************************
 * STRUCTURES 
 ******************************************************************************/



/*******************************************************************************
 * FUNCTIONS 
 ******************************************************************************/




/**
 * @brief element of list node
 */
typedef struct qlpListElement
{
    union
    {
        void        * mPtrValue;
        stlInt64      mIntValue;
/*      Oid           mOidValue;  */
    } mData;
    
    struct qlpListElement  * mPrev;
    struct qlpListElement  * mNext;
} qlpListElement;


/**
 * @brief list node
 */
struct qlpList
{
    qllNodeType       mType;     /**< list type : QLL_POINTER_LIST_TYPE, QLL_INTEGER_LIST_TYPE, QLL_OID_LIST_TYPE */
    
    stlInt32          mCount;    /**< list element count */
    qlpListElement  * mHead;     /**< head element */
    qlpListElement  * mTail;     /**< tail element */
};
    

/**
 * @brief get first element of list
 */
#define QLP_LIST_GET_FIRST( aList )    ( (aList)->mHead )


/**
 * @brief get last element of list
 */
#define QLP_LIST_GET_LAST( aList )    ( (aList)->mTail )


/**
 * @brief get element count of list
 */
#define QLP_LIST_GET_COUNT( aList )   ( (aList)->mCount )


/**
 * @brief get pointer value of current element
 */
#define QLP_LIST_GET_POINTER_VALUE( aListElem )   ( (aListElem)->mData.mPtrValue )


/**
 * @brief get integer value of current element
 */
#define QLP_LIST_GET_INTEGER_VALUE( aListElem )   ( (aListElem)->mData.mIntValue )


/**
 * @brief get oid value of current element
 */
/*
#define QLP_LIST_GET_OID_VALUE( aList )   ( (aList)->mData.mOidValue )
*/


/**
 * @brief get next element of list element
 */
#define QLP_LIST_GET_NEXT_ELEM( aListElem )    ( (aListElem)->mNext )


/**
 * @brief get prev element of list element
 */
#define QLP_LIST_GET_PREV_ELEM( aListElem )    ( (aListElem)->mPrev )


/**
 * @brief loop through the list
 */
#define QLP_LIST_FOR_EACH( aList, aElem /* out */ )     \
    for( (aElem) = QLP_LIST_GET_FIRST( aList ) ;        \
         (aElem) != NULL ;                              \
         (aElem) = QLP_LIST_GET_NEXT_ELEM( aElem ) )


/**
 * @brief loop through the list starting from a specified element
 */
#define QLP_LIST_FOR_EACH_ELEM( aInitElem, aElem /* out */ )     \
    for( (aElem) = (aInitElem) ;                                 \
         (aElem) != NULL ;                                       \
         (aElem) = QLP_LIST_GET_NEXT_ELEM( aElem ) )


/**
 * @brief loop reverse through the list
 */
#define QLP_LIST_FOR_EACH_REVERSE( aList, aElem /* out */ )     \
    for( (aElem) = QLP_LIST_GET_LAST( aList ) ;                 \
         (aElem) != NULL ;                                      \
         (aElem) = QLP_LIST_GET_PREV_ELEM( aElem ) )


/**
 * @brief loop reverse through the list starting from a specified element
 */
#define QLP_LIST_FOR_EACH_ELEM_REVERSE( aInitElem, aElem /* out */ )    \
    for( (aElem) = (aInitElem) ;                                        \
         (aElem) != NULL ;                                              \
         (aElem) = QLP_LIST_GET_PREV_ELEM( aElem ) )


/* public functions */
inline qlpList * qlpMakeList( stlParseParam  * aParam,
                              qllNodeType      aListType );

inline void qlpAddListToList( qlpList        * aToList,
                              qlpList        * aAddList );

inline void qlpAddPtrValueToList( stlParseParam  * aParam,
                                  qlpList        * aList,
                                  void           * aValue );

        
/** @} */

#endif /* _QLPNODELIST_H_ */
