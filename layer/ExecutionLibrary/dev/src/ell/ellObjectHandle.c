/*******************************************************************************
 * ellObjectHandle.c
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
 * @file ellObjectHandle.c
 * @brief Execution Layer 의 Object Dictionary Handle 관리 공통 모듈 
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>
#include <elgPendOp.h>

/**
 * @addtogroup ellObjectHandle
 * @{
 */

/*******************************************************************************
 * Object Dictionary Handle
 *******************************************************************************/

/**
 * @brief Dictionary Handle 로부터 Object Descriptor를 얻는다.
 * @param[in]  aDictHandle    Dictionary Handle
 * @warning 제발~~ return 값을 지역변수로만 사용하길...
 * - Prepare-Execute 등에서 해당 Return 값을 저장해서 사용하면 안된다.
 *  - Object Descriptor 는 공유 메모리 상의 정보로서 LOCK 이 없으면 언제든지 대체될 수 있다.
 * - 저장 대상이 되는 것은 Dictionary Handle 이며, Execute 전에 Validation 해야 한다. 
 * @return void pointer
 * - 다음과 같은 Object Descriptor 로 형변환하여 정보에 접근할 수 있다.
 *    - ellColumnDesc
 *    - ellTableConstDesc
 *    - ellTableDesc
 *    - ellIndexDesc
 *    - ellSequenceDesc
 *    - ellSynonymDesc
 *    - ellPublicSynonymDesc
 *    - ellSchemaDesc
 *    - ellAuthDesc
 *    - ellTablespaceDesc
 * @remarks
 */
void * ellGetObjectDesc( ellDictHandle * aDictHandle )
{
    STL_ASSERT( (aDictHandle->mObjectType > ELL_OBJECT_NA) &&
                (aDictHandle->mObjectType < ELL_OBJECT_MAX) );

    /* Handle 을 이용하여 descriptor 접근시 aging 되거나 재사용되지 않은 상태여야 함 */
    STL_DASSERT( aDictHandle->mMemUseCnt
                 == ((eldcHashElement*)aDictHandle->mObjHashElement)->mVersionInfo.mMemUseCnt );
    
    return ((eldcHashElement*)aDictHandle->mObjHashElement)->mObjectDesc;
}

/**
 * @brief Dictionary Handle 을 초기화한다.
 * @param[in]  aDictHandle   Dicitonary Handle
 * @remarks
 */
void  ellInitDictHandle( ellDictHandle * aDictHandle )
{
    stlMemset( aDictHandle, 0x00, STL_SIZEOF(ellDictHandle) );
    
    aDictHandle->mFullyQualifiedName = STL_FALSE;
    
    aDictHandle->mObjectType  = ELL_OBJECT_NA;
    aDictHandle->mObjHashElement = NULL;
    
    aDictHandle->mPrivObject      = ELL_PRIV_NA;
    aDictHandle->mPrivHashElement = NULL;
    
    aDictHandle->mViewSCN         = 0;
    aDictHandle->mTryModifyCnt    = 0;
    aDictHandle->mLocalModifySeq  = 0;
    aDictHandle->mMemUseCnt       = 0;
}

/**
 * @brief Dictionary Handle 정보를 설정한다.
 * @param[in]     aTransID          Transaction Identifier
 * @param[in]     aViewSCN          View SCN
 * @param[in,out] aDictHandle       Dictionary Handle
 * @param[in]     aObjectType       ObjectType
 * @param[in]     aHashElement      Cache Hash Element
 * @param[in]     aRecentCheckable  Recent Checkable
 * @param[in]     aEnv              Environment
 * @remarks
 */
void ellSetDictHandle( smlTransId        aTransID,
                       smlScn            aViewSCN,
                       ellDictHandle   * aDictHandle,
                       ellObjectType     aObjectType,
                       void            * aHashElement,
                       stlBool           aRecentCheckable,
                       ellEnv          * aEnv )
{
    stlUInt64        sLocalModifySeq = 0;
    eldcHashElement * sHashElement = (eldcHashElement*)aHashElement;
    eldcLocalCache * sLocalCache;
    
    aDictHandle->mFullyQualifiedName = STL_FALSE;
    aDictHandle->mRecentCheckable = aRecentCheckable;
    
    aDictHandle->mObjectType  = aObjectType;
    aDictHandle->mObjHashElement = aHashElement;
    
    aDictHandle->mPrivObject  = ELL_PRIV_NA;
    aDictHandle->mPrivHashElement = NULL;

    if( aRecentCheckable == STL_TRUE )
    {
        /**
         * MY 트랜잭션에 의해서 삽입되었거나 삭제되거나 변경된 경우는
         * local cache 에서 객체의 Modify sequence를 구한다.
         */
        
        if( (sHashElement->mVersionInfo.mCreateTransID == aTransID) ||
            (sHashElement->mVersionInfo.mDropTransID == aTransID) ||
            ( (sHashElement->mVersionInfo.mModifyingCnt > 0) &&
              (ELL_SESS_ENV(aEnv)->mLocalModifySeq > 0) ) )
        {
            eldcSearchLocalCache( aHashElement, &sLocalCache, aEnv );

            if( sLocalCache != NULL )
            {
                sLocalModifySeq = sLocalCache->mModifySeq;
            }
        }
    }
    
    aDictHandle->mTryModifyCnt = sHashElement->mVersionInfo.mTryModifyCnt;
    aDictHandle->mLocalModifySeq = sLocalModifySeq;
    aDictHandle->mViewSCN = aViewSCN;
    aDictHandle->mMemUseCnt = sHashElement->mVersionInfo.mMemUseCnt;
        
    /* free 된 memory 가 아니어야 함 */
    STL_DASSERT( (aDictHandle->mMemUseCnt % 2) == 1 );
}


/**
 * @brief Dictionary Handle 이 유효한지 판단한다.
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aDictHandle  Dictionary Handle
 * @param[out] aIsValid     Valid 여부
 * @param[in]  aEnv         Environment
 * @remarks
 * - Prepared Statement 나 Planned Cache 등에 저장해둔 Dictionary Handle 정보가 유효한지 확인한다.
 * - Execution 전에 검사하여, 유효하지 않을 경우 Recompile 을 시도해야 한다.
 */
stlStatus ellIsRecentDictHandle( smlTransId        aTransID,
                                 ellDictHandle   * aDictHandle,
                                 stlBool         * aIsValid,
                                 ellEnv          * aEnv )
{
    stlBool sResult = STL_FALSE;
    
    /**
     * Privilege Dictionary Handle 을 인자로 넘기면 안됨.
     * ID-based Object Dictionary Handle 인 경우, Object Descriptor 가 반드시 존재해야 함
     * Name-based Object Dictionary Handle 이 외부에서 사용되어서는 안됨.
     */
    
    STL_PARAM_VALIDATE( (aDictHandle->mObjectType > ELL_OBJECT_NA) &&
                        (aDictHandle->mObjectType < ELL_OBJECT_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDictHandle->mObjHashElement != NULL, ELL_ERROR_STACK(aEnv) );

    STL_TRY( eldcIsRecentHandle( aTransID,
                                 aDictHandle,
                                 & sResult,
                                 aEnv )
             == STL_SUCCESS );
    
    /**
     * Output 설정
     */

    *aIsValid = sResult;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Dictionary Handle 의 Object Type 을 획득한다.
 * @param[in]  aDictHandle Dictionary Handle
 * @return
 * ellObjectType
 * @remarks
 */
ellObjectType ellGetObjectType( ellDictHandle   * aDictHandle )
{
    return aDictHandle->mObjectType;
}

/**
 * @brief Dictionary Handle 의 Object ID 를 획득한다.
 * @param[in]  aDictHandle Dictionary Handle
 * @return stlInt64 Object ID
 * @remarks
 */
stlInt64 ellGetDictObjectID( ellDictHandle   * aDictHandle )
{
    typedef stlInt64 (* eldGetIDFunc) ( ellDictHandle * aObjectHandle );
    eldGetIDFunc      sGetIDFunc[ELL_OBJECT_MAX] = {
        NULL,
        ellGetColumnID,
        ellGetTableID,
        ellGetConstraintID,
        ellGetIndexID,
        ellGetSequenceID,
        ellGetSchemaID,
        ellGetTablespaceID,
        ellGetAuthID,
        ellGetDbCatalogID,
        ellGetSynonymID,
        ellGetPublicSynonymID,
        ellGetProfileID
        
    };

    return sGetIDFunc[aDictHandle->mObjectType]( aDictHandle );
}


/**
 * @brief Object Handle 이 구성시점에 Schema Name 을 포함해서 얻은 정보인지 검사
 */
stlBool ellIsFullNameObject( ellDictHandle * aDictHandle )
{
    return aDictHandle->mFullyQualifiedName;
}

/*******************************************************************************
 * Object Handle List
 *******************************************************************************/

/**
 * @brief SQL Object 목록을 관리하기 위한 초기화 
 * @param[out] aObjectList       SQL Object List
 * @param[in]  aRegionMem        Region Memory 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellInitObjectList( ellObjectList ** aObjectList,
                             knlRegionMem   * aRegionMem,
                             ellEnv         * aEnv )
{
    ellObjectList * sObjList = NULL;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aObjectList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Object List 초기화
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(ellObjectList),
                                (void **) & sObjList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sObjList, 0x00, STL_SIZEOF(ellObjectList) );

    STL_RING_INIT_HEADLINK( & sObjList->mHeadList,
                            STL_OFFSETOF( ellObjectItem, mLink ) );

    /**
     * Output 설정
     */

    *aObjectList = sObjList;
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Child Object List 를 Into Object List 에 Merge 한다.
 * @param[in] aIntoObjectList  Into Object List
 * @param[in] aChildObjectList Child Object List
 * @remarks
 */
void ellMergeObjectList( ellObjectList * aIntoObjectList,
                         ellObjectList * aChildObjectList )
{
    stlRingHead  * sIntoHeadPtr = NULL;
    stlRingHead  * sChildHeadPtr = NULL;
    stlRingEntry * sChildFirstDataPtr = NULL;
    stlRingEntry * sChildLastDataPtr = NULL;
    
    STL_DASSERT( aIntoObjectList != NULL );
    STL_DASSERT( aChildObjectList != NULL );

    sIntoHeadPtr  = & aIntoObjectList->mHeadList;
    sChildHeadPtr = & aChildObjectList->mHeadList;

    aIntoObjectList->mCount += aChildObjectList->mCount;
    
    if ( STL_RING_IS_EMPTY(sChildHeadPtr) == STL_TRUE )
    {
        /**
         * child 가 empty 인 경우
         */
    }
    else
    {
        sChildFirstDataPtr = STL_RING_GET_FIRST_DATA(sChildHeadPtr);
        sChildLastDataPtr  = STL_RING_GET_LAST_DATA(sChildHeadPtr);
        
        STL_RING_SPLICE_LAST(sIntoHeadPtr, sChildFirstDataPtr, sChildLastDataPtr);
    }
}

/**
 * @brief Object Handle 을 비교한다.
 * @param[in] aLeftObjectHandle     Left Object Dictionary Handle
 * @param[in] aRightObjectHandle    Right Object Dictionary Handle
 * @param[in] aWithQuantifyName     with Quantified Name
 * @remarks
 */
static inline stlInt32 ellCompareObjectHandle( ellDictHandle    * aLeftObjectHandle,
                                               ellDictHandle    * aRightObjectHandle,
                                               stlBool            aWithQuantifyName )
{
    stlInt64          sLeftID;
    stlInt64          sRightID;

    if( aLeftObjectHandle->mObjectType == aRightObjectHandle->mObjectType )
    {
        sLeftID     = ellGetDictObjectID( aLeftObjectHandle );
        sRightID    = ellGetDictObjectID( aRightObjectHandle );

        if( sLeftID < sRightID )
        {
            return -1;
        }
        else if( sLeftID == sRightID )
        {
            if ( aWithQuantifyName == STL_TRUE )
            {
                if ( aLeftObjectHandle->mFullyQualifiedName == aRightObjectHandle->mFullyQualifiedName )
                {
                    return 0;
                }
                else
                {
                    if ( aLeftObjectHandle->mFullyQualifiedName == STL_TRUE )
                    {
                        return -1;
                    }
                    else
                    {
                        return 1;
                    }
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 1;
        }
    }
    else if( aLeftObjectHandle->mObjectType < aRightObjectHandle->mObjectType )
    {
        return -1;
    }
    else
    {
        return 1;
    }

    return 0;
}

/**
 * @brief SQL Object List 에 중복없이 Object Handle 을 추가한다.
 * @param[in]  aObjectList       SQL Object List
 * @param[in]  aObjectHandle     Object Dictionary Handle
 * @param[out] aDuplicate        중복 여부 (nullable)
 * @param[in]  aRegionMem        Region Memory 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellAddNewObjectItem( ellObjectList * aObjectList,
                               ellDictHandle * aObjectHandle,
                               stlBool       * aDuplicate,
                               knlRegionMem  * aRegionMem,
                               ellEnv        * aEnv )
{
    ellObjectItem    * sObjectItem = NULL;
    ellObjectItem    * sFoundObjectItem = NULL;

    stlInt32  sResult = 0;
    stlBool   sDuplicate = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aObjectList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * DDL Object List에 이미 존재하는지 검사
     */

    STL_RING_FOREACH_ENTRY( & aObjectList->mHeadList, sFoundObjectItem )
    {   
        sResult = ellCompareObjectHandle( aObjectHandle,
                                          &sFoundObjectItem->mObjectHandle,
                                          STL_FALSE /* aWithQuantifiedName */ );
        if ( sResult <= 0 )
        {   
            break;
        }
    }


    if ( STL_RING_IS_HEADLINK( & aObjectList->mHeadList, sFoundObjectItem ) == STL_TRUE )
    {
        /**
         * DDL Object List 의 마지막에 연결 
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF(ellObjectItem),
                                    (void **) & sObjectItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sObjectItem, 0x00, STL_SIZEOF(ellObjectItem) );

        STL_RING_INIT_DATALINK( sObjectItem, STL_OFFSETOF(ellObjectItem, mLink) );

        stlMemcpy( & sObjectItem->mObjectHandle, aObjectHandle, STL_SIZEOF(ellDictHandle) );

        aObjectList->mCount++;
        STL_RING_ADD_LAST( & aObjectList->mHeadList, sObjectItem );

        sDuplicate = STL_FALSE;
    }
    else
    {
        if ( sResult == 0 )
        {
            /**
             * 동일한 테이블 핸들인 경우 추가하지 않는다.
             */
            
            sDuplicate = STL_TRUE;
        }
        else
        {
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF(ellObjectItem),
                                        (void **) & sObjectItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemset( sObjectItem, 0x00, STL_SIZEOF(ellObjectItem) );

            STL_RING_INIT_DATALINK( sObjectItem, STL_OFFSETOF(ellObjectItem, mLink) );

            stlMemcpy( & sObjectItem->mObjectHandle, aObjectHandle, STL_SIZEOF(ellDictHandle) );

            /**
             * Data Pointer 앞에 추가
             */

            aObjectList->mCount++;
            STL_RING_INSERT_BEFORE( & aObjectList->mHeadList, sFoundObjectItem, sObjectItem );

            sDuplicate = STL_FALSE;
        }
    }

    /**
     * Output 설정
     */

    if ( aDuplicate != NULL )
    {
        *aDuplicate = sDuplicate;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief SQL Object List 에 Quantified 여부를 고려하여 중복없이 Object Handle 을 추가한다.
 * @param[in]  aObjectList       SQL Object List
 * @param[in]  aObjectHandle     Object Dictionary Handle
 * @param[out] aDuplicate        중복 여부 (nullable)
 * @param[in]  aRegionMem        Region Memory 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellAddNewObjectItemWithQuantifiedName( ellObjectList * aObjectList,
                                                 ellDictHandle * aObjectHandle,
                                                 stlBool       * aDuplicate,
                                                 knlRegionMem  * aRegionMem,
                                                 ellEnv        * aEnv )
{
    ellObjectItem    * sObjectItem = NULL;
    ellObjectItem    * sFoundObjectItem = NULL;

    stlInt32  sResult = 0;
    stlBool   sDuplicate = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aObjectList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * DDL Object List에 이미 존재하는지 검사
     */

    STL_RING_FOREACH_ENTRY( & aObjectList->mHeadList, sFoundObjectItem )
    {   
        sResult = ellCompareObjectHandle( aObjectHandle,
                                          & sFoundObjectItem->mObjectHandle,
                                          STL_TRUE /* aWithQuantifiedName */ );
        if ( sResult <= 0 )
        {   
            break;
        }
    }


    if ( STL_RING_IS_HEADLINK( & aObjectList->mHeadList, sFoundObjectItem ) == STL_TRUE )
    {
        /**
         * DDL Object List 의 마지막에 연결 
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF(ellObjectItem),
                                    (void **) & sObjectItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sObjectItem, 0x00, STL_SIZEOF(ellObjectItem) );

        STL_RING_INIT_DATALINK( sObjectItem, STL_OFFSETOF(ellObjectItem, mLink) );

        stlMemcpy( & sObjectItem->mObjectHandle, aObjectHandle, STL_SIZEOF(ellDictHandle) );

        aObjectList->mCount++;
        STL_RING_ADD_LAST( & aObjectList->mHeadList, sObjectItem );

        sDuplicate = STL_FALSE;
    }
    else
    {
        if ( sResult == 0 )
        {
            /**
             * 동일한 테이블 핸들인 경우 추가하지 않는다.
             */
            
            sDuplicate = STL_TRUE;
        }
        else
        {
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF(ellObjectItem),
                                        (void **) & sObjectItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemset( sObjectItem, 0x00, STL_SIZEOF(ellObjectItem) );

            STL_RING_INIT_DATALINK( sObjectItem, STL_OFFSETOF(ellObjectItem, mLink) );

            stlMemcpy( & sObjectItem->mObjectHandle, aObjectHandle, STL_SIZEOF(ellDictHandle) );

            /**
             * Data Pointer 앞에 추가
             */

            aObjectList->mCount++;
            STL_RING_INSERT_BEFORE( & aObjectList->mHeadList, sFoundObjectItem, sObjectItem );

            sDuplicate = STL_FALSE;
        }
    }

    /**
     * Output 설정
     */

    if ( aDuplicate != NULL )
    {
        *aDuplicate = sDuplicate;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SQL Object List 에서 해당 Object Handle 을 찾아 제거한다.
 * @param[in]  aObjectList       SQL Object List
 * @param[in]  aObjectHandle     Object Dictionary Handle
 * @param[out] aDeleted          삭제 여부
 * @remarks
 */
void ellDeleteObjectItem( ellObjectList * aObjectList,
                          ellDictHandle * aObjectHandle,
                          stlBool       * aDeleted )
{
    ellObjectItem    * sObjectItem = NULL;

    stlInt32  sResult = 0;
    stlBool   sFound = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_DASSERT( aObjectList != NULL );
    STL_DASSERT( aObjectHandle != NULL );
    STL_DASSERT( aDeleted != NULL );

    /**
     * Object List 에서 검색
     */

    sFound = STL_FALSE;
    
    STL_RING_FOREACH_ENTRY( & aObjectList->mHeadList, sObjectItem )
    {   
        sResult = ellCompareObjectHandle( aObjectHandle,
                                          &sObjectItem->mObjectHandle,
                                          STL_FALSE /* aWithQuantifiedName */ );
        if ( sResult == 0 )
        {
            sFound = STL_TRUE;
            break;
        }
        else
        {
            /* continue */
        }
    }

    /**
     * 해당 Object Handle 을 삭제
     */
    
    if ( sFound == STL_TRUE )
    {
        aObjectList->mCount--;
        *aDeleted = STL_TRUE;
        STL_RING_UNLINK( & aObjectList->mHeadList, sObjectItem );
    }
    else
    {
        *aDeleted = STL_FALSE;
    }
}


/**
 * @brief SQL Object List 에 중복을 허용하여 Object Handle 을 추가한다.
 * @param[in] aObjectList       SQL Object List
 * @param[in] aObjectHandle     Object Dictionary Handle
 * @param[in] aRegionMem        Region Memory 
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus ellAppendObjectItem( ellObjectList * aObjectList,
                               ellDictHandle * aObjectHandle,
                               knlRegionMem  * aRegionMem,
                               ellEnv        * aEnv )
{
    ellObjectItem    * sObjectItem = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aObjectList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Object Item 공간을 확보
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(ellObjectItem),
                                (void **) & sObjectItem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sObjectItem, 0x00, STL_SIZEOF(ellObjectItem) );

    STL_RING_INIT_DATALINK( sObjectItem,
                            STL_OFFSETOF(ellObjectItem, mLink) );

    stlMemcpy( & sObjectItem->mObjectHandle,
               aObjectHandle,
               STL_SIZEOF(ellDictHandle) );

    /**
     * Object List 에 추가
     */
    
    aObjectList->mCount++;
    STL_RING_ADD_LAST( & aObjectList->mHeadList, sObjectItem );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Destin Object List 에 Source Object List 를 추가한다.
 * @param[in] aDstObjectList    Destine SQL Object List
 * @param[in] aSrcObjectList    Source SQL Object List
 * @param[in] aRegionMem        Region Memory 
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus ellAppendObjectList( ellObjectList * aDstObjectList,
                               ellObjectList * aSrcObjectList,
                               knlRegionMem  * aRegionMem,
                               ellEnv        * aEnv )
{
    ellObjectItem  * sObjectItem    = NULL;
    ellDictHandle  * sObjectHandle  = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDstObjectList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSrcObjectList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Source Object List 순회
     */
        
    STL_RING_FOREACH_ENTRY( & aSrcObjectList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        /**
         * Destine Object List 에 추가
         */

        STL_TRY( ellAppendObjectItem( aDstObjectList,
                                      sObjectHandle,
                                      aRegionMem,
                                      aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SQL Object List 을 복사한다.
 * @param[out] aDstObjectList    Dest SQL Object List
 * @param[in]  aSrcObjectList    Source  SQL Object List
 * @param[in]  aRegionMem        Region Memory 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellCopyObjectItem( ellObjectList ** aDstObjectList,
                             ellObjectList  * aSrcObjectList,
                             knlRegionMem   * aRegionMem,
                             ellEnv         * aEnv )
{
    ellObjectList  * sNewObjectList = NULL;

    ellObjectItem  * sNewObjectItem = NULL;
    ellObjectItem  * sObjectItem    = NULL;
    ellDictHandle  * sObjectHandle  = NULL;

    
    /**
     * Parameter Validation
     */

    STL_ASSERT( aDstObjectList != NULL );
    STL_ASSERT( aSrcObjectList != NULL );


    /**
     * Object List 초기화
     */

    STL_TRY( ellInitObjectList( & sNewObjectList,
                                aRegionMem,
                                aEnv )
             == STL_SUCCESS );
                                
    
    /**
     * Source Object Item 순회
     */
        
    STL_RING_FOREACH_ENTRY( & aSrcObjectList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        
        /**
         * Object Item 공간을 확보
         */
    
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF(ellObjectItem),
                                    (void **) & sNewObjectItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sNewObjectItem, 0x00, STL_SIZEOF(ellObjectItem) );

        STL_RING_INIT_DATALINK( sNewObjectItem,
                                STL_OFFSETOF(ellObjectItem, mLink) );

        stlMemcpy( & sNewObjectItem->mObjectHandle,
                   sObjectHandle,
                   STL_SIZEOF(ellDictHandle) );
        
        
        /**
         * Object List 에 추가
         */

        sNewObjectList->mCount++;
        STL_RING_ADD_LAST( & sNewObjectList->mHeadList, sNewObjectItem );
    }


    /**
     * OUTPUT 설정
     */
    
    *aDstObjectList = sNewObjectList;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Dictionary Object List 가 Object Item 을 가지고 있는지 검사
 * @param[in] aObjectList  Dictionary Object List
 * @return
 * - STL_TRUE : Object Item 이 있음
 * - STL_FALSE: Object Item 이 없음
 * @remarks
 */
stlBool ellHasObjectItem( ellObjectList * aObjectList )
{
    stlRingHead  * sHeadPtr =  & aObjectList->mHeadList;
    
    if ( STL_RING_IS_EMPTY(sHeadPtr) == STL_TRUE )
    {
        return STL_FALSE;
    }
    else
    {
        return STL_TRUE;
    }
}


/**
 * @brief Object ID 가 Object List 에 존재하는지 여부
 * @param[in] aObjectType  Object Type
 * @param[in] aObjectID    Object ID
 * @param[in] aObjectList  Dictionary Object List
 * @return
 * - STL_TRUE : Object 가 있음
 * - STL_FALSE: Object 가 없음
 * @remarks
 */
stlBool ellObjectExistInObjectList( ellObjectType   aObjectType,
                                    stlInt64        aObjectID,
                                    ellObjectList * aObjectList )
{
    stlBool   sExist = STL_FALSE;

    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    stlInt64          sObjID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Parameter Validation
     */

    STL_ASSERT( (aObjectType > ELL_OBJECT_NA) && (aObjectType < ELL_OBJECT_MAX) );
    STL_ASSERT( aObjectList != NULL );
    
    /**
     * Object List 를 순회하며 Object ID 와 동일한 Object 가 있는지 확인
     */
    
    STL_RING_FOREACH_ENTRY( & aObjectList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_ASSERT( sObjectHandle->mObjectType == aObjectType );
        
        sObjID = ellGetDictObjectID( sObjectHandle );

        if ( sObjID == aObjectID )
        {
            sExist = STL_TRUE;
            break;
        }
        else
        {
            continue;
        }
    }
    
    return sExist;
}


/**
 * @brief Source Object List 에서 Object Type 에 해당하는 Object List 를 추출한다
 * @param[in]  aSrcObjList   Source Object List
 * @param[in]  aObjectType   Object Type
 * @param[out] aDstObjList   Destine Object List
 * @param[in]  aRegionMem    Region Memory
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus ellGetObjectList( ellObjectList  * aSrcObjList,
                            ellObjectType    aObjectType,
                            ellObjectList ** aDstObjList,
                            knlRegionMem   * aRegionMem,
                            ellEnv         * aEnv )
{
    ellObjectList * sObjList = NULL;

    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    stlBool           sMatched = STL_FALSE;
    stlBool           sDuplicate = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSrcObjList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aObjectType > ELL_OBJECT_NA) && (aObjectType < ELL_OBJECT_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDstObjList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Object List 초기화
     */

    STL_TRY( ellInitObjectList( & sObjList, aRegionMem, aEnv ) == STL_SUCCESS );

    
    /**
     * Source Object List 를 순회하며 Object Type 에 해당하는 객체를 추가 
     */
    
    STL_RING_FOREACH_ENTRY( & aSrcObjList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        sMatched = STL_FALSE;
        if( sObjectHandle->mObjectType == aObjectType )
        {
            /**
             * 원하는 Object Type 임
             */
            
            sMatched = STL_TRUE;
        }
        else
        {
            sMatched = STL_FALSE;
        }

        if ( sMatched == STL_TRUE )
        {
            STL_TRY( ellAddNewObjectItem( sObjList,
                                          sObjectHandle,
                                          & sDuplicate,
                                          aRegionMem,
                                          aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * 관련 없는 Object Type 임
             */
        }
    }
                                
    /**
     * Output 설정
     */

    *aDstObjList = sObjList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Dictionary Object List 에서 N 번째 Dictionary Handle 을 얻는다.
 * @param[in]  aObjectList      Dictionary Object List
 * @param[in]  aObjectIdx       N 번째 Object (0-base)
 * @return
 * Dictionary Object Handle
 * - 존재하지 않을 경우, null 값
 * @remarks
 */
ellDictHandle * ellGetNthObjectHandle( ellObjectList * aObjectList,
                                       stlInt32        aObjectIdx )
{
    stlInt32          i = 0;
    
    ellObjectItem * sObjectItem = NULL;
    ellDictHandle * sObjectHandle = NULL;

    ellObjectItem * sFoundObjectItem = NULL;
    
    /**
     * N 번째 Object Dictionary Handle 검색 
     */

    if ( STL_RING_IS_EMPTY(& aObjectList->mHeadList ) == STL_TRUE )
    {
        sObjectHandle = NULL;
    }
    else
    {
        sFoundObjectItem = NULL;
        STL_RING_FOREACH_ENTRY( & aObjectList->mHeadList, sObjectItem )
        {
            if ( i == aObjectIdx )
            {
                sFoundObjectItem = sObjectItem;
                break;
            }
            else
            {
                i++;
            }
        }

        if ( sFoundObjectItem != NULL )
        {
            sObjectHandle = & sFoundObjectItem->mObjectHandle;
        }
        else
        {
            /**
             * aObjectIdx 가 범위를 벗어난 경우
             */
            
            sObjectHandle = NULL;
        }
    }
    
    return sObjectHandle;
}


/** @} ellObjectHandle */
