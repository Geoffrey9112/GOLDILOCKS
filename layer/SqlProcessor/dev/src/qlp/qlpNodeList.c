/*******************************************************************************
 * qlpNodeList.c
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

/**
 * @file qlpNodeList.c
 * @brief SQL Processor Layer List Definition
 */

#include <qlDef.h>


/**
 * @addtogroup qlpNodeList
 * @{
 */


/**
 * @brief list 생성
 * @param[in]     aParam       Parse Parameter
 * @param[in]     aListType    List Node Type
 */
inline qlpList * qlpMakeList( stlParseParam   * aParam,
                              qllNodeType       aListType )
{
    qlpList  * sList = NULL;

    /**
     * List & Head Element 공간 할당
     */
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpList ),
                            (void**) & sList,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sList = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        /**
         * List 초기화
         */

        sList->mType  = aListType;
        sList->mCount = 0;
        sList->mHead  = NULL;
        sList->mTail  = NULL;
    }
    
    return sList;
}


/**
 * @brief list 에 list 를 추가
 * @param[out]    aToList      List Node
 * @param[in]     aAddList     추가할 list
 */
inline void qlpAddListToList( qlpList        * aToList,
                              qlpList        * aAddList )
{
    qlpListElement * sTail = NULL;

    sTail = aToList->mTail;
    sTail->mNext = aAddList->mHead;
    aToList->mTail = aAddList->mTail;
    aToList->mCount += aAddList->mCount;
}


/**
 * @brief list에 pointer value element 추가
 * @param[in]     aParam       Parse Parameter
 * @param[out]    aList        List Node
 * @param[in]     aValue       Pointer Value
 */
inline void qlpAddPtrValueToList( stlParseParam  * aParam,
                                  qlpList        * aList,
                                  void           * aValue )
{
    qlpListElement  * sListElem = NULL;

    /**
     * List Element 공간 할당
     */

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpListElement ),
                            (void**) & sListElem,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        aList = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        /**
         * List Element Value 설정
         */

        sListElem->mData.mPtrValue = aValue;
        sListElem->mPrev = NULL;
        sListElem->mNext = NULL;
    
    
        /**
         * List 설정
         */

        if( aList->mCount == 0 )
        {
            aList->mHead = aList->mTail = sListElem;
        }
        else 
        {
            aList->mTail->mNext = sListElem;
            sListElem->mPrev    = aList->mTail;
            aList->mTail        = sListElem;
        }
    
        aList->mCount++;
    }
    
    return;
}


/** @} qlpNodeList */

