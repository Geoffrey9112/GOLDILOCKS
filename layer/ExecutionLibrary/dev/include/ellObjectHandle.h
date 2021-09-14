/*******************************************************************************
 * ellObjectHandle.h
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


#ifndef _ELL_OBJECT_HANDLE_H_
#define _ELL_OBJECT_HANDLE_H_ 1

/**
 * @file ellObjectHandle.h
 * @brief Execution Layer 의 Object Dictionary Handle 관리 공통 모듈 
 */

/**
 * @defgroup ellObjectHandle Object Dictionary Handle
 * @ingroup ellSqlObject
 * @{
 */

/**
 * @brief Dictionary Object List
 */
typedef struct ellObjectList
{
    stlInt32    mCount;      /**< Object Count */
    stlRingHead mHeadList;   /**< Object List */
} ellObjectList;


/**
 * @brief Dictionary Object Item
 * @remarks
 */
typedef struct ellObjectItem
{
    ellDictHandle mObjectHandle;  /**< Object Dictionary Handle */
    stlRingEntry  mLink;          /**< Link */
} ellObjectItem;


/*
 * Object Dictionary Handle
 */

void * ellGetObjectDesc( ellDictHandle * aDictHandle );

void ellInitDictHandle( ellDictHandle * aDictHandle );

void ellSetDictHandle( smlTransId        aTransID,
                       smlScn            aViewSCN,
                       ellDictHandle   * aDictHandle,
                       ellObjectType     aObjectType,
                       void            * aHashElement,
                       stlBool           aRecentCheckable,
                       ellEnv          * aEnv );

stlStatus ellIsRecentDictHandle( smlTransId        aTransID,
                                 ellDictHandle   * aDictHandle,
                                 stlBool         * aIsValid,
                                 ellEnv          * aEnv );

ellObjectType ellGetObjectType( ellDictHandle * aDictHandle );

stlInt64 ellGetDictObjectID( ellDictHandle * aDictHandle );

stlBool ellIsFullNameObject( ellDictHandle * aDictHandle );

/*
 * Object Handle List
 */

stlStatus ellInitObjectList( ellObjectList ** aObjectList,
                             knlRegionMem   * aRegionMem,
                             ellEnv         * aEnv );

void ellMergeObjectList( ellObjectList * aIntoObjectList,
                         ellObjectList * aChildObjectList );

stlStatus ellAddNewObjectItem( ellObjectList * aObjectList,
                               ellDictHandle * aObjectHandle,
                               stlBool       * aDuplicate,
                               knlRegionMem  * aRegionMem,
                               ellEnv        * aEnv );

stlStatus ellAddNewObjectItemWithQuantifiedName( ellObjectList * aObjectList,
                                                 ellDictHandle * aObjectHandle,
                                                 stlBool       * aDuplicate,
                                                 knlRegionMem  * aRegionMem,
                                                 ellEnv        * aEnv );

void ellDeleteObjectItem( ellObjectList * aObjectList,
                          ellDictHandle * aObjectHandle,
                          stlBool       * aDeleted );

stlStatus ellAppendObjectItem( ellObjectList * aObjectList,
                               ellDictHandle * aObjectHandle,
                               knlRegionMem  * aRegionMem,
                               ellEnv        * aEnv );

stlStatus ellAppendObjectList( ellObjectList * aDstObjectList,
                               ellObjectList * aSrcObjectList,
                               knlRegionMem  * aRegionMem,
                               ellEnv        * aEnv );

stlStatus ellCopyObjectItem( ellObjectList ** aDstObjectList,
                             ellObjectList  * aSrcObjectList,
                             knlRegionMem   * aRegionMem,
                             ellEnv         * aEnv );

stlBool ellHasObjectItem( ellObjectList * aObjectList );

stlBool ellObjectExistInObjectList( ellObjectType   aObjectType,
                                    stlInt64        aObjectID,
                                    ellObjectList * aObjectList );

stlStatus ellGetObjectList( ellObjectList  * aSrcObjList,
                            ellObjectType    aObjectType,
                            ellObjectList ** aDstObjList,
                            knlRegionMem   * aRegionMem,
                            ellEnv         * aEnv );

ellDictHandle * ellGetNthObjectHandle( ellObjectList * aObjectList,
                                       stlInt32        aObjectIdx );


/** @} ellObjectHandle */



#endif /* _ELL_OBJECT_HANDLE_H_ */
