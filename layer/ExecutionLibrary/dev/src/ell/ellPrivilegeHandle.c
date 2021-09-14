/*******************************************************************************
 * ellPrivilegeHandle.c
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
 * @file ellPrivilegeHandle.c
 * @brief Execution Layer 의 Privilege Dictionary Handle 관리 공통 모듈 
 */

#include <ell.h>
#include <elDef.h>
#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>
#include <elgPendOp.h>

/**
 * @addtogroup ellPrivHandle
 * @{
 */


/*******************************************************************************
 * Privilege Dictionary Handle
 *******************************************************************************/

/**
 * @brief Dictionary Handle 로부터 Object Descriptor를 얻는다.
 * @param[in]  aDictHandle    Dictionary Handle
 * @warning 제발~~ return 값을 지역변수로만 사용하길...
 * - Prepare-Execute 등에서 해당 Return 값을 저장해서 사용하면 안된다.
 *  - Privilege Descriptor 는 공유 메모리 상의 정보로서 LOCK 이 없으면 언제든지 대체될 수 있다.
 * - 저장 대상이 되는 것은 Dictionary Handle 이며, Execute 전에 Validation 해야 한다. 
 * @return privilege descriptor pointer
 * @remarks
 */
ellPrivDesc * ellGetPrivDesc( ellDictHandle * aDictHandle )
{
    eldcHashElement * sHashElement = (eldcHashElement *) aDictHandle->mPrivHashElement;
    
    STL_ASSERT( (aDictHandle->mPrivObject > ELL_PRIV_NA) &&
                (aDictHandle->mPrivObject < ELL_PRIV_OWNER) );
    
    return (ellPrivDesc *) sHashElement->mHashData;
}

/**
 * @brief Privilege Dictionary Handle 정보를 설정한다.
 * @param[in]     aTransID          Transaction Identifier
 * @param[in]     aViewSCN          View SCN
 * @param[in,out] aDictHandle       Dictionary Handle
 * @param[in]     aPrivObject       Privilege Object
 * @param[in]     aPrivHashElement  Privilege Hash Element
 * @param[in]     aEnv              Environment
 * @remarks
 */
void ellSetPrivHandle( smlTransId       aTransID,
                       smlScn           aViewSCN,
                       ellDictHandle  * aDictHandle,
                       ellPrivObject    aPrivObject,
                       void *           aPrivHashElement,
                       ellEnv         * aEnv )
{
    eldcHashElement * sHashElement = (eldcHashElement *) aPrivHashElement;
    stlInt64          sLocalModifySeq = 0;
    eldcLocalCache  * sLocalCache;

    aDictHandle->mRecentCheckable = STL_TRUE;
    
    aDictHandle->mObjectType  = ELL_OBJECT_NA;
    aDictHandle->mObjHashElement = NULL;

    aDictHandle->mPrivObject      = aPrivObject;
    aDictHandle->mPrivHashElement = aPrivHashElement;

    /**
     * MY 트랜잭션에 의해서 삽입되었거나 삭제되거나 변경된 경우는
     * local cache 에서 객체의 Modify sequence를 구한다.
     */
        
    if( (sHashElement->mVersionInfo.mCreateTransID == aTransID) ||
        (sHashElement->mVersionInfo.mDropTransID == aTransID) ||
        ( (sHashElement->mVersionInfo.mModifyingCnt > 0) &&
          (ELL_SESS_ENV(aEnv)->mLocalModifySeq > 0) ) )
    {
    
        eldcSearchLocalCache( aPrivHashElement, &sLocalCache, aEnv );

        if( sLocalCache != NULL )
        {
            sLocalModifySeq = sLocalCache->mModifySeq;
        }
    }
    
    aDictHandle->mLocalModifySeq = sLocalModifySeq;
    aDictHandle->mTryModifyCnt = sHashElement->mVersionInfo.mTryModifyCnt;
    aDictHandle->mViewSCN = aViewSCN;
    aDictHandle->mMemUseCnt = sHashElement->mVersionInfo.mMemUseCnt;
        
    /* free 된 memory 가 아니어야 함 */
    STL_DASSERT( (aDictHandle->mMemUseCnt % 2) == 1 );
}


/**
 * @brief Privilege Dictionary Handle 이 유효한지 판단한다.
 * @param[in]  aTransID    Transaction ID
 * @param[in]  aPrivHandle Privilege Handle
 * @param[out] aIsValid    Valid 여부
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus ellIsRecentPrivHandle( smlTransId      aTransID,
                                 ellDictHandle * aPrivHandle,
                                 stlBool       * aIsValid,
                                 ellEnv        * aEnv )
{
    stlBool sValid = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( (aPrivHandle->mPrivObject > ELL_PRIV_NA) &&
                        (aPrivHandle->mPrivObject < ELL_PRIV_OWNER),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIsValid != NULL, ELL_ERROR_STACK(aEnv) );
    

    STL_TRY( eldcIsRecentHandle( aTransID,
                                 aPrivHandle,
                                 & sValid,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aIsValid = sValid;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Session User 의 Granted Privilege Handle 을 획득한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aPrivObject        Privilege Object
 * @param[in]  aObjectID          Object ID
 * @param[in]  aPrivAction        Privilege Action
 * @param[in]  aWithGrant         WITH GRANT OPTION 필요 여부 
 * @param[out] aPrivHandle        Privilege Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment
 * @remarks
 * - Applicable Privilege : aWithGrant(FALSE)
 * - Grantable  Privilege : aWithGrant(TRUE)
 * 용어 정의
 * - Granted Privilege    : 소유한 권한
 * - Applicable Privilege : 사용할 수 있는 권한
 *                        : 부여받지 않았으나, ROLE, PUBLIC 등으로부터 획득할 수 있는 권한
 * - Grantable Privilege  : 소유한 권한 중 다시 부여할 수 있는 권한
 */
stlStatus ellGetGrantedPrivHandle( smlTransId           aTransID,
                                   smlScn               aViewSCN,
                                   ellPrivObject        aPrivObject,
                                   stlInt64             aObjectID,
                                   stlInt32             aPrivAction,
                                   stlBool              aWithGrant,
                                   ellDictHandle      * aPrivHandle,
                                   stlBool            * aExist,
                                   ellEnv             * aEnv )
{
    ellDictHandle * sSessionUser = NULL;
    ellDictHandle * sSessionRole = NULL;

    ellDictHandle   sPublicAuth;
    stlBool         sObjectExist = STL_FALSE;
    
    stlInt64  sGranteeID = ELL_DICT_OBJECT_ID_NA;
    stlBool   sPrivExist = STL_FALSE;

    stlInt32  i = 0;
    ellAuthDesc * sAuthDesc = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( (aPrivObject > ELL_PRIV_NA) && (aPrivObject < ELL_PRIV_OWNER),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivAction > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Session 사용자 정보 획득
     */

    sSessionUser = ellGetSessionUserHandle( aEnv );
    sSessionRole = ellGetSessionRoleHandle( aEnv );

    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( sSessionUser );
        
    /**
     * Applicable Privilege 검색
     */
    
    while ( 1 )
    {
        /**
         * Current User 의 Granted Privilege 검색
         */
        
        sGranteeID = ellGetAuthID( sSessionUser );

        STL_TRY( eldcGetGrantPrivHandle( aTransID,
                                         aViewSCN,
                                         aPrivObject,
                                         sGranteeID,
                                         aObjectID,
                                         aPrivAction,
                                         aWithGrant,
                                         aPrivHandle,
                                         & sPrivExist,
                                         aEnv )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /**
         * Role 정보로부터 Applicable Privilege 검색
         */
        
        if ( sSessionRole == NULL )
        {
            /**
             * Granted Role 에서 Applicable Privilege 검색
             */
            
            for ( i = 0; i < sAuthDesc->mGrantedRoleCnt; i++ )
            {
                sGranteeID = sAuthDesc->mGrantedRoleIDArray[i];

                STL_TRY( eldcGetGrantPrivHandle( aTransID,
                                                 aViewSCN,
                                                 aPrivObject,
                                                 sGranteeID,
                                                 aObjectID,
                                                 aPrivAction,
                                                 aWithGrant,
                                                 aPrivHandle,
                                                 & sPrivExist,
                                                 aEnv )
                         == STL_SUCCESS );

                if ( sPrivExist == STL_TRUE )
                {
                    break;
                }
            }

            if ( sPrivExist == STL_TRUE )
            {
                break;
            }

            /**
             * @todo Applicable Role 의 Hierachy 를 고려하여 Applicable Privilege 를 검색해야 함
             */
        }
        else
        {
            /**
             * Session Role 정보로부터 Applicable Privilege 검색
             * - 명시적으로 ROLE 을 부여한 경우로 Applicable Role 을 검색하지 않는다.
             *  - ex) SET ROLE role_name
             *  - ex) --as role_name
             */
        
            sGranteeID = ellGetAuthID( sSessionRole );

            STL_TRY( eldcGetGrantPrivHandle( aTransID,
                                             aViewSCN,
                                             aPrivObject,
                                             sGranteeID,
                                             aObjectID,
                                             aPrivAction,
                                             aWithGrant,
                                             aPrivHandle,
                                             & sPrivExist,
                                             aEnv )
                     == STL_SUCCESS );

            if ( sPrivExist == STL_TRUE )
            {
                break;
            }
        }

        /**
         * PUBLIC authorization 의 Granted Privilege 검색
         */

        STL_TRY( ellGetAuthDictHandleByID( aTransID,
                                           aViewSCN,
                                           ellGetAuthIdPUBLIC(),
                                           & sPublicAuth,
                                           & sObjectExist,
                                           aEnv )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_OBJECT_MODIFIED );
        
        sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( & sPublicAuth );
        sGranteeID = ellGetAuthID( & sPublicAuth );

        STL_TRY( eldcGetGrantPrivHandle( aTransID,
                                         aViewSCN,
                                         aPrivObject,
                                         sGranteeID,
                                         aObjectID,
                                         aPrivAction,
                                         aWithGrant,
                                         aPrivHandle,
                                         & sPrivExist,
                                         aEnv )
                 == STL_SUCCESS );

        if ( sPrivExist == STL_TRUE )
        {
            break;
        }

        /**
         * Role 정보로부터 Applicable Privilege 검색
         */
        
        if ( sSessionRole == NULL )
        {
            /**
             * PUBLIC 에 부여된 Granted Role 에서 Applicable Privilege 검색
             */
            
            for ( i = 0; i < sAuthDesc->mGrantedRoleCnt; i++ )
            {
                sGranteeID = sAuthDesc->mGrantedRoleIDArray[i];

                STL_TRY( eldcGetGrantPrivHandle( aTransID,
                                                 aViewSCN,
                                                 aPrivObject,
                                                 sGranteeID,
                                                 aObjectID,
                                                 aPrivAction,
                                                 aWithGrant,
                                                 aPrivHandle,
                                                 & sPrivExist,
                                                 aEnv )
                         == STL_SUCCESS );

                if ( sPrivExist == STL_TRUE )
                {
                    break;
                }
            }

            if ( sPrivExist == STL_TRUE )
            {
                break;
            }

            /**
             * @todo Applicable Role 의 Hierachy 를 고려하여 Applicable Privilege 를 검색해야 함
             */
        }
        else
        {
            /**
             * Nothing To Do
             */
        }
        
        break;
    }
    
    /**
     * Output 설정
     */

    *aExist = sPrivExist;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OBJECT_MODIFIED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INVALID_OBJECT,
                      NULL,
                      ELL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Privilege Descriptor 와 동일한 Privilege Handle 을 획득한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aPrivDesc          Privilege Descriptor
 * @param[out] aPrivHandle        Privilege Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment
 * @remarks
 */
stlStatus ellGetPrivDescHandle( smlTransId           aTransID,
                                smlScn               aViewSCN,
                                ellPrivDesc        * aPrivDesc,
                                ellDictHandle      * aPrivHandle,
                                stlBool            * aExist,
                                ellEnv             * aEnv )
{
    stlBool   sPrivExist = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aPrivDesc != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Privilege Descriptor 와 동일한 Privilege Handle 을 검색
     */
    
    STL_TRY( eldcGetRevokePrivHandle( aTransID,
                                      aViewSCN,
                                      aPrivDesc->mPrivObject,
                                      aPrivDesc->mGrantorID,
                                      aPrivDesc->mGranteeID,
                                      aPrivDesc->mObjectID,
                                      aPrivDesc->mPrivAction.mAction,
                                      aPrivHandle,
                                      & sPrivExist,
                                      aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aExist = sPrivExist;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * Privilege Handle List
 *******************************************************************************/

/**
 * @brief Privilege 목록을 관리하기 위한 초기화 
 * @param[out] aPrivList         Privilege List
 * @param[in]  aForRevoke        GRANT or REVOKE
 * @param[in]  aRegionMem        Region Memory 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellInitPrivList( ellPrivList   ** aPrivList,
                           stlBool          aForRevoke,
                           knlRegionMem   * aRegionMem,
                           ellEnv         * aEnv )
{
    ellPrivList * sPrivList = NULL;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPrivList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Privilege List 초기화
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(ellPrivList),
                                (void **) & sPrivList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sPrivList, 0x00, STL_SIZEOF(ellPrivList) );

    sPrivList->mCount    = 0;
    sPrivList->mForRevoke = aForRevoke;

    STL_RING_INIT_HEADLINK( & sPrivList->mHeadList,
                            STL_OFFSETOF( ellPrivItem, mLink ) );

    
    /**
     * Output 설정
     */

    *aPrivList = sPrivList;
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Privilege Descriptor 를 비교한다.
 * @param[in] aForRevoke           for GRANT or REVOKE
 * @param[in] aLeftPrivDesc       Left Privilege Descriptor
 * @param[in] aRightPrivDesc      Right Privilege Descriptor
 * @remarks
 */
static inline stlInt64 ellComparePrivDesc( stlBool        aForRevoke,
                                           ellPrivDesc  * aLeftPrivDesc,
                                           ellPrivDesc  * aRightPrivDesc )
{
    if ( aLeftPrivDesc->mPrivObject == aRightPrivDesc->mPrivObject )
    {
        if ( aLeftPrivDesc->mObjectID == aRightPrivDesc->mObjectID )
        {
            if ( aLeftPrivDesc->mPrivAction.mAction == aRightPrivDesc->mPrivAction.mAction )
            {
                if ( aForRevoke == STL_TRUE )
                {
                    /**
                     * REVOKE 용인 경우
                     * - Grantor 와 Grantee 도 동일해야 함.
                     */

                    if ( aLeftPrivDesc->mGrantorID == aRightPrivDesc->mGrantorID )
                    {
                        if ( aLeftPrivDesc->mGranteeID == aRightPrivDesc->mGranteeID )
                        {
                            return 0;
                        }
                        else if ( aLeftPrivDesc->mGranteeID < aRightPrivDesc->mGranteeID )
                        {
                            return -1;
                        }
                        else
                        {
                            return 1;
                        }
                    }
                    else if ( aLeftPrivDesc->mGrantorID < aRightPrivDesc->mGrantorID )
                    {
                        return -1;
                    }
                    else
                    {
                        return 1;
                    }
                }
                else
                {
                    /**
                     * GRANT 용인 경우
                     * - 동일한 객체(PrivObject, ObjectID) 이며 동일한 Privilege Action 임.
                     */
                    return 0;
                }
            }
            else if ( aLeftPrivDesc->mPrivAction.mAction < aRightPrivDesc->mPrivAction.mAction )
            {
                return -1;
            }
            else
            {
                return 1;
            }
        }
        else if ( aLeftPrivDesc->mObjectID < aRightPrivDesc->mObjectID )
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }
    else if ( aLeftPrivDesc->mPrivObject < aRightPrivDesc->mPrivObject )
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
 * @brief Privilege List 에 중복없이 Privilege Handle 을 추가한다.
 * @param[in] aPrivList         Privilege List
 * @param[in] aPrivObject       Privilege Object 유형 
 * @param[in] aPrivHandle       Privilege Handle or Authorization Handle(Object Owner) (nullable)
 * @param[in] aPrivDesc         Privilege Descriptor (nullable)
 * @param[in] aDuplicate        중복 여부 
 * @param[in] aRegionMem        Region Memory 
 * @param[in] aEnv              Environment
 * @remarks
 * 
 * aPrivObject, aPrivHandle, aPrivDesc 관계
 * -------------------------------------------------------------
 *                 |  aPrivObject |  aPrivHandle   |  aPrivDesc
 * -------------------------------------------------------------
 * normal priv     |  PRIV OBJECT |  priv handle   |  null
 * owner           |  OWNER       |  auth handle   |  null
 * access control  |  PRIV OBJECT |  null          |  something
 * -------------------------------------------------------------
 * 
 */
stlStatus ellAddNewPrivItem( ellPrivList   * aPrivList,
                             ellPrivObject   aPrivObject,
                             ellDictHandle * aPrivHandle,
                             ellPrivDesc   * aPrivDesc,
                             stlBool       * aDuplicate,
                             knlRegionMem  * aRegionMem,
                             ellEnv        * aEnv )
{
    ellDictHandle   sEmptyHandle;
    ellDictHandle * sPrivHandle = NULL;
    ellPrivItem   * sPrivItem = NULL;
    ellPrivItem   * sFoundPrivItem = NULL;

    ellPrivDesc     sLeftDesc;
    stlBool         sResult = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aPrivList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aPrivObject > ELL_PRIV_NA) && (aPrivObject < ELL_PRIV_MAX), ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aPrivHandle != NULL) || (aPrivDesc != NULL), ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDuplicate != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Left Privilege Descriptor 구성
     */

    if ( aPrivObject == ELL_PRIV_OWNER )
    {
        /**
         * DROP object 계열의 구문이 수행될 때 발생한다.
         * Authorization Handle 정보임
         */
        
        stlMemset( & sLeftDesc, 0x00, STL_SIZEOF(ellPrivDesc) );
        sLeftDesc.mGrantorID          = ellGetAuthIdSYSTEM();     /* N/A */
        sLeftDesc.mGranteeID          = ellGetAuthID( aPrivHandle );
        sLeftDesc.mObjectID           = 0;    /* N/A */
        sLeftDesc.mPrivObject         = ELL_PRIV_OWNER;
        sLeftDesc.mPrivAction.mAction = 0;    /* N/A */
        sLeftDesc.mWithGrant          = STL_FALSE;  /* N/A */

        sPrivHandle = aPrivHandle;
    }
    else
    {
        if ( aPrivHandle != NULL )
        {
            stlMemcpy( & sLeftDesc, ellGetPrivDesc( aPrivHandle ), STL_SIZEOF(ellPrivDesc) );
            sPrivHandle = aPrivHandle;
        }
        else
        {
            stlMemcpy( & sLeftDesc, aPrivDesc, STL_SIZEOF(ellPrivDesc) );

            ellInitDictHandle( & sEmptyHandle );
            sPrivHandle = & sEmptyHandle;
        }
    }
    
    /**
     * Privilege List에 이미 존재하는지 검사
     */

    STL_RING_FOREACH_ENTRY( & aPrivList->mHeadList, sFoundPrivItem )
    {   
        sResult = ellComparePrivDesc( aPrivList->mForRevoke, & sLeftDesc, & sFoundPrivItem->mPrivDesc );
        if ( sResult <= 0 )
        {   
            break;
        }
    }


    if ( STL_RING_IS_HEADLINK( & aPrivList->mHeadList, sFoundPrivItem ) == STL_TRUE )
    {
        /**
         * Privilege List 의 마지막에 연결 
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF(ellPrivItem),
                                    (void **) & sPrivItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sPrivItem, 0x00, STL_SIZEOF(ellPrivItem) );

        STL_RING_INIT_DATALINK( sPrivItem, STL_OFFSETOF(ellPrivItem, mLink) );

        stlMemcpy( & sPrivItem->mPrivDesc, & sLeftDesc, STL_SIZEOF( ellPrivDesc ) );
        stlMemcpy( & sPrivItem->mPrivHandle, sPrivHandle, STL_SIZEOF(ellDictHandle) );

        aPrivList->mCount++;
        STL_RING_ADD_LAST( & aPrivList->mHeadList, sPrivItem );

        *aDuplicate = STL_FALSE;
    }
    else
    {
        if ( sResult == 0 )
        {
            /**
             * 동일한 Privilege 인 경우 추가하지 않는다.
             */

            *aDuplicate = STL_TRUE;
        }
        else
        {
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF(ellPrivItem),
                                        (void **) & sPrivItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemset( sPrivItem, 0x00, STL_SIZEOF(ellPrivItem) );

            STL_RING_INIT_DATALINK( sPrivItem, STL_OFFSETOF(ellPrivItem, mLink) );
            
            stlMemcpy( & sPrivItem->mPrivDesc, & sLeftDesc, STL_SIZEOF( ellPrivDesc ) );
            stlMemcpy( & sPrivItem->mPrivHandle, sPrivHandle, STL_SIZEOF(ellDictHandle) );

            /**
             * Data Pointer 앞에 추가
             */

            aPrivList->mCount++;
            STL_RING_INSERT_BEFORE( & aPrivList->mHeadList, sFoundPrivItem, sPrivItem );

            *aDuplicate = STL_FALSE;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Privilege List 를 Privilege List 로 재구성
 * @param[in]  aPrivList      Privilege List
 * @param[out] aPrivCnt       Privilege Count
 * @param[out] aPrivArray     Privilege Array
 * @param[in]  aRegionMem     Region Memory
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus ellMakePrivList2Array( ellPrivList   * aPrivList,
                                 stlInt32      * aPrivCnt,
                                 ellPrivItem  ** aPrivArray,
                                 knlRegionMem  * aRegionMem,
                                 ellEnv        * aEnv )
{
    stlInt32      i = 0;
    
    stlInt32      sCount = 0;
    ellPrivItem * sItemArray = NULL;
    ellPrivItem * sPrivItem  = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aPrivList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivCnt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivArray != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 공간 확보
     */

    sCount = aPrivList->mCount;

    if ( sCount == 0 )
    {
        sItemArray = NULL;
    }
    else
    {
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF(ellPrivItem) * sCount,
                                    (void **) & sItemArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        i = 0;
        STL_RING_FOREACH_ENTRY( & aPrivList->mHeadList, sPrivItem )
        {
            stlMemcpy( & sItemArray[i], sPrivItem, STL_SIZEOF(ellPrivItem) );

            /* link 를 제거하여 link 를 사용하는 경우가 없도록 함 */
            stlMemset( & sItemArray[i].mLink, 0x00, STL_SIZEOF(stlRingEntry) );

            i++;
        }
    }

    /**
     * Output 설정
     */

    *aPrivCnt   = sCount;
    *aPrivArray = sItemArray;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Privilege Array 를 구성한 Privilege Handle 이 유효한지 검사
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aPrivCnt     Privilege Item Count
 * @param[in]  aPrivArray   Privilege Array
 * @param[out] aIsValid     Valid 여부
 * @param[in]  aEnv         Environment
 * @remarks
 */
stlStatus ellIsRecentPrivArray( smlTransId     aTransID,
                                stlInt32       aPrivCnt,
                                ellPrivItem  * aPrivArray,
                                stlBool      * aIsValid,
                                ellEnv       * aEnv )
{
    stlBool sResult;
    
    stlInt32   i = 0;
    
    ellPrivItem  * sPrivItem = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aIsValid != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Privilege 개수만큼 유효성 검사
     */
    
    sResult = STL_TRUE;
    
    for ( i = 0; i < aPrivCnt; i++ )
    {
        sPrivItem = & aPrivArray[i];
        
        if ( sPrivItem->mPrivDesc.mPrivObject == ELL_PRIV_OWNER )
        {
            STL_TRY( ellIsRecentDictHandle( aTransID,
                                            & sPrivItem->mPrivHandle,
                                            & sResult,
                                            aEnv )
                     == STL_SUCCESS );

            if ( sResult == STL_FALSE )
            {
                break;
            }
        }
        else
        {
            STL_TRY( ellIsRecentPrivHandle( aTransID,
                                            & sPrivItem->mPrivHandle,
                                            & sResult,
                                            aEnv )
                     == STL_SUCCESS );
            
            if ( sResult == STL_FALSE )
            {
                break;
            }
        }
    }

    /**
     * Output 설정
     */

    *aIsValid = sResult;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Privilege List 가 비어있는지 여부
 * @param[in] aPrivList   Privilege List
 * @remarks
 */
stlBool ellHasPrivItem( ellPrivList   * aPrivList )
{
    if ( aPrivList->mCount == 0 )
    {
        return STL_FALSE;
    }
    else
    {
        return STL_TRUE;
    }
}


/**
 * @brief ACCESS CONTROL 권한을 통해 revokable privilege action 을 구한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aPrivObject              Privilege Object
 * @param[in]  aObjectHandle            Object Handle
 * @param[in]  aRevokeeID               Revokee ID
 * @param[in]  aPrivAction              space privilege action
 * @param[out] aPrivExist               Privilege 존재 여부
 * @param[out] aDuplicate               Duplicate Privilege 존재 여부 
 * @param[in,out] aPrivList             Revokable Privilege List
 * @param[in]  aRegionMem               Region Memory
 * @param[in]  aEnv                     Execution Library Environment
 */
stlStatus ellGetRevokablePrivByAccessControl( smlTransId           aTransID,
                                              smlStatement       * aStmt,
                                              ellPrivObject        aPrivObject,
                                              ellDictHandle      * aObjectHandle,
                                              stlInt64             aRevokeeID,
                                              stlInt32             aPrivAction,
                                              stlBool            * aPrivExist,
                                              stlBool            * aDuplicate,
                                              ellPrivList        * aPrivList,
                                              knlRegionMem       * aRegionMem,
                                              ellEnv             * aEnv )
{
    ellPrivList  * sTotalPrivList = NULL;
    
    ellPrivItem   * sPrivItem = NULL;

    stlBool   sPrivExist = STL_FALSE;
    stlBool   sDuplicate = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aPrivObject > ELL_PRIV_NA) && (aPrivObject < ELL_PRIV_OWNER), ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRevokeeID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivAction > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivExist != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDuplicate != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 객체 유형에 따른 All Revokable Privilege List 획득
     */

    switch (aPrivObject)
    {
        case ELL_PRIV_DATABASE:
            {
                STL_TRY( ellGetRevokableAllDatabasePriv( aTransID,
                                                         aStmt,
                                                         aRevokeeID,
                                                         STL_TRUE, /* aAccessControl */
                                                         STL_FALSE, /* aGrantOption */
                                                         & sTotalPrivList,
                                                         aRegionMem,
                                                         aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_PRIV_SPACE:
            {
                STL_TRY( ellGetRevokableAllSpacePriv( aTransID,
                                                      aStmt,
                                                      aObjectHandle,
                                                      aRevokeeID,
                                                      STL_TRUE, /* aAccessControl */
                                                      STL_FALSE, /* aGrantOption */
                                                      & sTotalPrivList,
                                                      aRegionMem,
                                                      aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_PRIV_SCHEMA:
            {
                STL_TRY( ellGetRevokableAllSchemaPriv( aTransID,
                                                       aStmt,
                                                       aObjectHandle,
                                                       aRevokeeID,
                                                       STL_TRUE, /* aAccessControl */
                                                       STL_FALSE, /* aGrantOption */
                                                       & sTotalPrivList,
                                                       aRegionMem,
                                                       aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_PRIV_TABLE:
            {
                STL_TRY( ellGetRevokableAllTablePriv( aTransID,
                                                      aStmt,
                                                      aObjectHandle,
                                                      aRevokeeID,
                                                      STL_TRUE, /* aAccessControl */
                                                      STL_FALSE, /* aGrantOption */
                                                      & sTotalPrivList,
                                                      aRegionMem,
                                                      aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_PRIV_USAGE:
            {
                STL_TRY( ellGetRevokableAllSeqPriv( aTransID,
                                                    aStmt,
                                                    aObjectHandle,
                                                    aRevokeeID,
                                                    STL_TRUE, /* aAccessControl */
                                                    STL_FALSE, /* aGrantOption */
                                                    & sTotalPrivList,
                                                    aRegionMem,
                                                    aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_PRIV_COLUMN:
            {
                STL_ASSERT(0);
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /**
     * Privilege Action 과 동일한 Privilege 추출
     */

    sPrivExist = STL_FALSE;

    STL_RING_FOREACH_ENTRY( & sTotalPrivList->mHeadList, sPrivItem )
    {
        if ( (sPrivItem->mPrivDesc.mPrivObject == aPrivObject) &&
             (sPrivItem->mPrivDesc.mPrivAction.mAction == aPrivAction) )
        {
            /**
             * 동일한 Privilege Action 임
             */
                
            STL_TRY( ellAddNewPrivItem( aPrivList,
                                        aPrivObject,
                                        & sPrivItem->mPrivHandle,
                                        NULL,   /* aPrivDesc */
                                        & sDuplicate,
                                        aRegionMem,
                                        aEnv )
                     == STL_SUCCESS );

            sPrivExist = STL_TRUE;
            
            /**
             * 중복이 존재함
             */

            if ( sDuplicate == STL_TRUE )
            {
                *aDuplicate = sDuplicate;
            }
            else
            {
                /* 중복 여부를 누적함 */
            }
        }
        else
        {
            /**
             * 다른 privilege action 임
             */
        }
    }

    /**
     * Output 설정
     */
    
    *aPrivExist = sPrivExist;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Revoked Privilege Descriptor 와 관련하여 REVOKE 할 후손 Privilege List 를 획득한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aStmt              Statement
 * @param[in]  aGrantOption       GRANT OPTION FOR
 * @param[in]  aRevokePriv        Revoked Privilege Descriptor
 * @param[out] aDependList        Dependent Privilege List 
 * @param[in]  aRegionMem         Region Memory
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus ellGetRevokeDependPrivList( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      stlBool              aGrantOption,
                                      ellPrivDesc        * aRevokePriv,
                                      ellPrivList       ** aDependList,
                                      knlRegionMem       * aRegionMem,
                                      ellEnv             * aEnv )
{
    ellPrivList * sDescendentList = NULL;
    ellPrivList * sCycleList = NULL;
    
    stlBool sDuplicate = STL_FALSE;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRevokePriv != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDependList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Revoked Privilege Descriptor 의 후손 Privilege List 를 획득
     */

    STL_TRY( ellInitPrivList( & sCycleList,
                              STL_TRUE,  /* aForRevoke */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );

    /**
     * Cycle Detect List 에 자신을 추가
     */
    
    STL_TRY( ellAddNewPrivItem( sCycleList,
                                aRevokePriv->mPrivObject,
                                NULL, /* aPrivHandle */
                                aRevokePriv,
                                & sDuplicate,
                                aRegionMem,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * Revoke Privilege 를 기준으로 Descendent List 를 획득
     */

    STL_TRY( ellGetDescendentPrivList( aTransID,
                                       aStmt,
                                       aGrantOption,
                                       aRevokePriv,
                                       sCycleList,
                                       & sDescendentList,
                                       aRegionMem,
                                       aEnv )
             == STL_SUCCESS );
    
    /**
     * Output 설정
     */

    *aDependList = sDescendentList;
        
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}





/**
 * @brief Parent Privilege Descriptor 와 관련된 후손 Privilege List 를 획득한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aStmt              Statement
 * @param[in]  aGrantOption       GRANT OPTION FOR
 * @param[in]  aParentPriv        Parent Privilege Descriptor
 * @param[in]  aCycleDetectList   Cycle Detect List
 * @param[out] aDecendentList     후손 Privilege List
 * @param[in]  aRegionMem         Region Memory
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus ellGetDescendentPrivList( smlTransId           aTransID,
                                    smlStatement       * aStmt,
                                    stlBool              aGrantOption,
                                    ellPrivDesc        * aParentPriv,
                                    ellPrivList        * aCycleDetectList,
                                    ellPrivList       ** aDecendentList,
                                    knlRegionMem       * aRegionMem,
                                    ellEnv             * aEnv )
{
    ellPrivList  * sMyDescentList = NULL;
    ellPrivList  * sChildPrivList = NULL;
    ellPrivList  * sRivalList = NULL;

    ellPrivItem  * sPrivItem = NULL;

    ellPrivList  * sChildDescentList = NULL;
    ellPrivItem  * sDescentPrivItem = NULL;
    
    stlBool   sDuplicate = STL_FALSE;
    stlBool   sHasDescendent = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParentPriv != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCycleDetectList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDecendentList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * My Descendent List 초기화
     */
    
    STL_TRY( ellInitPrivList( & sMyDescentList,
                              STL_TRUE,  /* aForRevoke */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );
    
    /**
     * Privilege 유형에 따른 Child Privilege List 획득
     */

    STL_TRY( ellGetChildPrivList( aTransID,
                                  aStmt,
                                  aGrantOption,
                                  aParentPriv,
                                  & sChildPrivList,
                                  aRegionMem,
                                  aEnv )
             == STL_SUCCESS );

    if ( ellHasPrivItem( sChildPrivList ) == STL_TRUE )
    {
        sHasDescendent = STL_TRUE;

        /**
         * Child List 를 My Decendent List 와 Cycle List 에 추가
         */

        STL_RING_FOREACH_ENTRY( & sChildPrivList->mHeadList, sPrivItem )
        {
            /**
             * Cycle Detect List 에 추가
             */

            STL_TRY( ellAddNewPrivItem( aCycleDetectList,
                                        sPrivItem->mPrivDesc.mPrivObject,
                                        NULL, /* aPrivHandle */
                                        & sPrivItem->mPrivDesc,
                                        & sDuplicate,
                                        aRegionMem,
                                        aEnv )
                     == STL_SUCCESS );
            
            if ( sDuplicate == STL_TRUE )
            {
                /**
                 * Cycle 이 존재함, Child Decendent 를 검사할 필요가 없음
                 */
            }
            else
            {
                /**
                 * My Decendent List 에 추가
                 */
                
                STL_TRY( ellAddNewPrivItem( sMyDescentList,
                                            sPrivItem->mPrivDesc.mPrivObject,
                                            NULL, /* aPrivHandle */
                                            & sPrivItem->mPrivDesc,
                                            & sDuplicate,
                                            aRegionMem,
                                            aEnv )
                         == STL_SUCCESS );
                
                STL_DASSERT( sDuplicate == STL_FALSE );
                
                /**
                 * Child 의 Descendent List 를 획득 
                 */
                
                STL_TRY( ellGetDescendentPrivList( aTransID,
                                                   aStmt,
                                                   aGrantOption,
                                                   & sPrivItem->mPrivDesc,
                                                   aCycleDetectList,
                                                   & sChildDescentList,
                                                   aRegionMem,
                                                   aEnv )
                         == STL_SUCCESS );

                if ( sChildDescentList == NULL )
                {
                    /**
                     * Child 의 Descent 가 없음
                     */
                }
                else
                {
                    /**
                     * Child 의 Descendent List 를 내 Descendent List 에 추가
                     */
                    
                    STL_RING_FOREACH_ENTRY( & sChildDescentList->mHeadList, sDescentPrivItem )
                    {
                        STL_TRY( ellAddNewPrivItem( sMyDescentList,
                                                    sDescentPrivItem->mPrivDesc.mPrivObject,
                                                    NULL, /* aPrivHandle */
                                                    & sDescentPrivItem->mPrivDesc,
                                                    & sDuplicate,
                                                    aRegionMem,
                                                    aEnv )
                                 == STL_SUCCESS );
                        
                        STL_DASSERT( sDuplicate == STL_FALSE );
                    }
                }
            }
        }
            
        /**
         * Parent Privilege 의 Rival List 획득
         */

        STL_TRY( ellGetRivalPrivList( aTransID,
                                      aStmt,
                                      aGrantOption,
                                      aParentPriv,
                                      & sRivalList,
                                      aRegionMem,
                                      aEnv )
                 == STL_SUCCESS );
        
        /**
         * Parent 가 없는 Descendent List 인지 검사
         */
        
        STL_RING_FOREACH_ENTRY( & sRivalList->mHeadList, sPrivItem )
        {
            /**
             * Rival 을 Descendent List 에 추가를 시도
             */

            sDuplicate = STL_FALSE;
            STL_TRY( ellAddNewPrivItem( sMyDescentList,
                                        sPrivItem->mPrivDesc.mPrivObject,
                                        NULL, /* aPrivHandle */
                                        & sPrivItem->mPrivDesc,
                                        & sDuplicate,
                                        aRegionMem,
                                        aEnv )
                     == STL_SUCCESS );

            if ( sDuplicate == STL_TRUE )
            {
                /**
                 * Rival 역시 Descendent 임
                 */
            }
            else
            {
                /**
                 * Revoked Privilege 뿐 아니라 또다른 Parent 가 있음.
                 */

                sHasDescendent = STL_FALSE;
                break;
            }
        }
    }
    else
    {
        /**
         * 후손이 없음
         */
        
        sHasDescendent = STL_FALSE;
    }
    
    /**
     * Output 설정
     */

    if ( sHasDescendent == STL_TRUE )
    {
        *aDecendentList = sMyDescentList;
    }
    else
    {
        *aDecendentList = NULL;
    }
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Revoked Privilege 와 관련된 Rival 이 될 수 있는 후보 Privilege List 를 획득한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aStmt              Statement
 * @param[in]  aGrantOption       GRANT OPTION FOR
 * @param[in]  aRevokePriv        Revoked Privilege Descriptor
 * @param[out] aRivalList         Rival Privilege List
 * @param[in]  aRegionMem         Region Memory
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus ellGetRivalPrivList( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               stlBool              aGrantOption,
                               ellPrivDesc        * aRevokePriv,
                               ellPrivList       ** aRivalList,
                               knlRegionMem       * aRegionMem,
                               ellEnv             * aEnv )
{
    ellPrivList * sAllPrivList = NULL;
    ellPrivList * sPrivList = NULL;

    ellPrivItem   * sPrivItem = NULL;

    stlInt32 sGranteeColumnNO = 0;
    
    stlBool   sIsRival = STL_FALSE;
    stlBool   sDuplicate = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRevokePriv != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRivalList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Revoke Privilege 와 동일한 Privilege List 추출
     */

    STL_TRY( ellInitPrivList( & sAllPrivList,
                              STL_TRUE,  /* aForRevoke, 모든 Privilege 목록을 얻기 위함 */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );

    switch (aRevokePriv->mPrivObject)
    {
        case ELL_PRIV_DATABASE:
            sGranteeColumnNO = ELDT_DBPriv_ColumnOrder_GRANTEE_ID;
            break;
        case ELL_PRIV_SPACE:
            sGranteeColumnNO = ELDT_SpacePriv_ColumnOrder_GRANTEE_ID;
            break;
        case ELL_PRIV_SCHEMA:
            sGranteeColumnNO = ELDT_SchemaPriv_ColumnOrder_GRANTEE_ID;
            break;
        case ELL_PRIV_TABLE:
            sGranteeColumnNO = ELDT_TablePriv_ColumnOrder_GRANTEE_ID;
            break;
        case ELL_PRIV_USAGE:
            sGranteeColumnNO = ELDT_UsagePriv_ColumnOrder_GRANTEE_ID;
            break;
        case ELL_PRIV_COLUMN:
            sGranteeColumnNO = ELDT_ColumnPriv_ColumnOrder_GRANTEE_ID;
            break;
        default:
            STL_ASSERT(0);
            break;
    }
    
    STL_TRY( eldGetPrivList( aTransID,
                             aStmt,
                             aRevokePriv->mPrivObject,
                             sGranteeColumnNO,
                             aRevokePriv->mGranteeID,
                             aRegionMem,
                             sAllPrivList,
                             aEnv )
             == STL_SUCCESS );

    /**
     * Privilege Object 와 Privilege Action 이 동일한 Privilege 를 추출
     */

    STL_TRY( ellInitPrivList( & sPrivList,
                              STL_TRUE,  /* aForRevoke */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & sAllPrivList->mHeadList, sPrivItem )
    {
        if ( (sPrivItem->mPrivDesc.mPrivObject == aRevokePriv->mPrivObject) &&
             (sPrivItem->mPrivDesc.mObjectID == aRevokePriv->mObjectID) &&
             (sPrivItem->mPrivDesc.mPrivAction.mAction == aRevokePriv->mPrivAction.mAction) )
        {
            /**
             * 동일한  Privilege 임
             */

            if ( (sPrivItem->mPrivDesc.mGrantorID == aRevokePriv->mGrantorID) &&
                 (sPrivItem->mPrivDesc.mGranteeID == aRevokePriv->mGranteeID) )
            {
                /**
                 * 자기 자신임
                 */
                
                sIsRival = STL_FALSE;
            }
            else
            {
                /**
                 * GRANT OPTION FOR 에 따른 Rival Privilege 대상인지 확인
                 */
                
                sIsRival = STL_TRUE;
                
                if ( aGrantOption == STL_TRUE )
                {
                    if ( sPrivItem->mPrivDesc.mWithGrant == STL_TRUE )
                    {
                        /**
                         * Rival Privilege 임
                         */
                        
                        sIsRival = STL_TRUE;
                    }
                    else
                    {
                        /**
                         * Rival Privilege 가 아님
                         */
                        
                        sIsRival = STL_FALSE;
                    }
                }
                else
                {
                    /**
                     * Rival Privilege 임 
                     */
                    
                    sIsRival = STL_TRUE;
                }
            }

            /**
             * Rival List 에 추가
             */
            
            if ( sIsRival == STL_TRUE )
            {
                STL_TRY( ellAddNewPrivItem( sPrivList,
                                            sPrivItem->mPrivDesc.mPrivObject,
                                            NULL, /* aPrivHandle */
                                            & sPrivItem->mPrivDesc,
                                            & sDuplicate,
                                            aRegionMem,
                                            aEnv )
                         == STL_SUCCESS );

                STL_ASSERT( sDuplicate == STL_FALSE );
            }
            else
            {
                /**
                 * Rival Privilege 가 아님
                 */
            }
        }
        else
        {
            /**
             * 다른 Privilege 임
             */
        }
    }

    
    /**
     * Output 설정
     */

    *aRivalList = sPrivList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
    
}


/**
 * @brief Parent Privilege Descriptor 의 Child Privilege List 를 획득한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aStmt              Statement
 * @param[in]  aGrantOption       GRANT OPTION FOR
 * @param[in]  aParentPriv        Parent Privilege Descriptor
 * @param[out] aChildList         Child Privilege List
 * @param[in]  aRegionMem         Region Memory
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus ellGetChildPrivList( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               stlBool              aGrantOption,
                               ellPrivDesc        * aParentPriv,
                               ellPrivList       ** aChildList,
                               knlRegionMem       * aRegionMem,
                               ellEnv             * aEnv )
{
    ellPrivList  * sAllPrivList = NULL;
    ellPrivList  * sChildPrivList = NULL;

    ellPrivItem   * sPrivItem = NULL;

    stlBool  sIsChild = STL_FALSE;
    
    stlInt32 sGrantorColumnNO = 0;

    stlBool   sDuplicate = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParentPriv != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Privilege 유형에 따른 Child Privilege List 획득
     */

    STL_TRY( ellInitPrivList( & sAllPrivList,
                              STL_TRUE,  /* aForRevoke, 모든 Privilege 목록을 얻기 위함 */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );
    
    switch (aParentPriv->mPrivObject)
    {
        case ELL_PRIV_DATABASE:
            sGrantorColumnNO = ELDT_DBPriv_ColumnOrder_GRANTOR_ID;
            break;
        case ELL_PRIV_SPACE:
            sGrantorColumnNO = ELDT_SpacePriv_ColumnOrder_GRANTOR_ID;
            break;
        case ELL_PRIV_SCHEMA:
            sGrantorColumnNO = ELDT_SchemaPriv_ColumnOrder_GRANTOR_ID;
            break;
        case ELL_PRIV_TABLE:
            sGrantorColumnNO = ELDT_TablePriv_ColumnOrder_GRANTOR_ID;
            break;
        case ELL_PRIV_USAGE:
            sGrantorColumnNO = ELDT_UsagePriv_ColumnOrder_GRANTOR_ID;
            break;
        case ELL_PRIV_COLUMN:
            sGrantorColumnNO = ELDT_ColumnPriv_ColumnOrder_GRANTOR_ID;
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    STL_TRY( eldGetPrivList( aTransID,
                             aStmt,
                             aParentPriv->mPrivObject,
                             sGrantorColumnNO,
                             aParentPriv->mGranteeID,
                             aRegionMem,
                             sAllPrivList,
                             aEnv )
             == STL_SUCCESS );
    
    /**
     * Privilege Object 와 Privilege Action 이 동일한 Privilege 를 추출
     */

    STL_TRY( ellInitPrivList( & sChildPrivList,
                              STL_TRUE,  /* aForRevoke */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & sAllPrivList->mHeadList, sPrivItem )
    {
        if ( (sPrivItem->mPrivDesc.mPrivObject == aParentPriv->mPrivObject) &&
             (sPrivItem->mPrivDesc.mObjectID == aParentPriv->mObjectID) &&
             (sPrivItem->mPrivDesc.mPrivAction.mAction == aParentPriv->mPrivAction.mAction) )
        {
            /**
             * GRANT OPTION FOR 에 따른 Child Privilege 대상인지 확인
             */
            
            sIsChild = STL_TRUE;
            
            if ( aGrantOption == STL_TRUE )
            {
                if ( sPrivItem->mPrivDesc.mWithGrant == STL_TRUE )
                {
                    /**
                     * Child Privilege 임
                     */

                    sIsChild = STL_TRUE;
                }
                else
                {
                    /**
                     * Child Privilege 가 아님
                     */
                    
                    sIsChild = STL_FALSE;
                }
            }
            else
            {
                /**
                 * Child Privilege 임 
                 */
                
                sIsChild = STL_TRUE;
            }


            if ( sIsChild == STL_TRUE )
            {
                /**
                 * Child Privilege 임 
                 */
                
                STL_TRY( ellAddNewPrivItem( sChildPrivList,
                                            sPrivItem->mPrivDesc.mPrivObject,
                                            NULL, /* aPrivHandle */
                                            & sPrivItem->mPrivDesc,
                                            & sDuplicate,
                                            aRegionMem,
                                            aEnv )
                         == STL_SUCCESS );
                
                STL_DASSERT( sDuplicate == STL_FALSE );
            }
            else
            {
                /**
                 * Child 대상이 아님 
                 */
            }
        }
        else
        {
            /**
             * 다른 privilege action 임
             */
        }
    }

    /**
     * Output 설정
     */

    *aChildList = sChildPrivList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief Auth ID 가 Grantee 인 모든 Privilege 를 REVOKE 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aAuthID    Authorization ID
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus ellRevokeAuthGranteeAllPrivilege( smlTransId     aTransID,
                                            smlStatement * aStmt,
                                            stlInt64       aAuthID,
                                            ellEnv       * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );

    /**
     * Auth 가 소유한 Database Privilege 를 제거
     */

    STL_TRY( eldDeletePrivRowsAndCache( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_DATABASE_PRIVILEGES,
                                        ELDT_DBPriv_ColumnOrder_GRANTEE_ID,
                                        aAuthID,
                                        aEnv )
             == STL_SUCCESS );

    /**
     * Auth 가 소유한 Space Privilege 를 제거
     */

    STL_TRY( eldDeletePrivRowsAndCache( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_TABLESPACE_PRIVILEGES,
                                        ELDT_SpacePriv_ColumnOrder_GRANTEE_ID,
                                        aAuthID,
                                        aEnv )
             == STL_SUCCESS );

    /**
     * Auth 가 소유한 Schema Privilege 를 제거
     */

    STL_TRY( eldDeletePrivRowsAndCache( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_SCHEMA_PRIVILEGES,
                                        ELDT_SchemaPriv_ColumnOrder_GRANTEE_ID,
                                        aAuthID,
                                        aEnv )
             == STL_SUCCESS );

    /**
     * Auth 가 소유한 Usage Privilege 를 제거
     */

    STL_TRY( eldDeletePrivRowsAndCache( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_USAGE_PRIVILEGES,
                                        ELDT_UsagePriv_ColumnOrder_GRANTEE_ID,
                                        aAuthID,
                                        aEnv )
             == STL_SUCCESS );
    
    /**
     * Auth 가 소유한 Column Privilege 를 제거
     */

    STL_TRY( eldDeletePrivRowsAndCache( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_COLUMN_PRIVILEGES,
                                        ELDT_ColumnPriv_ColumnOrder_GRANTEE_ID,
                                        aAuthID,
                                        aEnv )
             == STL_SUCCESS );
    
    /**
     * Auth 가 소유한 Table Privilege 를 제거
     */

    STL_TRY( eldDeletePrivRowsAndCache( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_TABLE_PRIVILEGES,
                                        ELDT_TablePriv_ColumnOrder_GRANTEE_ID,
                                        aAuthID,
                                        aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Auth ID 가 Grantee 인 모든 Privilege 목록을 획득
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aAuthID        Authorization ID
 * @param[out] aUserPrivList  User Grantor Privilege List
 * @param[in]  aRegionMem     Region Memory
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus ellGetAuthGrantorPrivList( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     stlInt64        aAuthID,
                                     ellPrivList  ** aUserPrivList,
                                     knlRegionMem  * aRegionMem,
                                     ellEnv        * aEnv )
{
    ellPrivList * sAllPrivList = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserPrivList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 초기화
     */
    
    STL_TRY( ellInitPrivList( & sAllPrivList,
                              STL_TRUE,  /* aForRevoke, 모든 Privilege 목록을 얻기 위함 */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );
    
    /**
     * Database Privilege List 획득 
     */

    STL_TRY( eldGetPrivList( aTransID,
                             aStmt,
                             ELL_PRIV_DATABASE,
                             ELDT_DBPriv_ColumnOrder_GRANTOR_ID,
                             aAuthID,
                             aRegionMem,
                             sAllPrivList,
                             aEnv )
             == STL_SUCCESS );

    /**
     * Tablespace Privilege List 획득 
     */

    STL_TRY( eldGetPrivList( aTransID,
                             aStmt,
                             ELL_PRIV_SPACE,
                             ELDT_SpacePriv_ColumnOrder_GRANTOR_ID,
                             aAuthID,
                             aRegionMem,
                             sAllPrivList,
                             aEnv )
             == STL_SUCCESS );

    /**
     * Schema Privilege List 획득 
     */

    STL_TRY( eldGetPrivList( aTransID,
                             aStmt,
                             ELL_PRIV_SCHEMA,
                             ELDT_SchemaPriv_ColumnOrder_GRANTOR_ID,
                             aAuthID,
                             aRegionMem,
                             sAllPrivList,
                             aEnv )
             == STL_SUCCESS );

    /**
     * Table Privilege List 획득 
     */

    STL_TRY( eldGetPrivList( aTransID,
                             aStmt,
                             ELL_PRIV_TABLE,
                             ELDT_TablePriv_ColumnOrder_GRANTOR_ID,
                             aAuthID,
                             aRegionMem,
                             sAllPrivList,
                             aEnv )
             == STL_SUCCESS );

    /**
     * Column Privilege List 획득 
     */
    
    STL_TRY( eldGetPrivList( aTransID,
                             aStmt,
                             ELL_PRIV_COLUMN,
                             ELDT_ColumnPriv_ColumnOrder_GRANTOR_ID,
                             aAuthID,
                             aRegionMem,
                             sAllPrivList,
                             aEnv )
             == STL_SUCCESS );
    
    /**
     * Usage Privilege List 획득 
     */

    STL_TRY( eldGetPrivList( aTransID,
                             aStmt,
                             ELL_PRIV_USAGE,
                             ELDT_UsagePriv_ColumnOrder_GRANTOR_ID,
                             aAuthID,
                             aRegionMem,
                             sAllPrivList,
                             aEnv )
             == STL_SUCCESS );
    
    /**
     * Output 설정
     */

    *aUserPrivList = sAllPrivList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * Privilege Dictionary Descriptor 조작 
 *******************************************************************************/

/**
 * @brief GRANT PRIVILEGE 를 위한 privilege descriptor 를 dictionary 와 cache 에 추가한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aPrivDesc  Privilege Descriptor
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus ellGrantPrivDescAndCache( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    ellPrivDesc  * aPrivDesc,
                                    ellEnv       * aEnv )
{
    ellDictHandle   sPrivHandle;
    ellPrivDesc   * sFoundDesc = NULL;
    stlBool         sPrivExist;

    ellDictHandle   sObjectHandle;
    stlBool         sObjectExist;
    
    stlBool    sNeedInsert = STL_FALSE;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Privilege Handle 이 이미 존재하는 지 확인
     */ 

    STL_TRY( ellGetPrivDescHandle( aTransID,
                                   ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                   aPrivDesc,
                                   & sPrivHandle,
                                   & sPrivExist,
                                   aEnv )
             == STL_SUCCESS );

    if ( sPrivExist == STL_FALSE )
    {
        /**
         * 존재하지 않는 경우
         */

        sNeedInsert = STL_TRUE;
        
    }
    else
    {
        /**
         * 동일한 Privilege Descriptor 를 가진 Privilege Handle 을 존재하는 경우
         */

        sFoundDesc = ellGetPrivDesc( & sPrivHandle );

        if ( sFoundDesc->mWithGrant == STL_TRUE )
        {
            /**
             * 이미 WITH GRANT OPTION 이 있는 경우
             * - 변경할 필요가 없음 
             */

            sNeedInsert = STL_FALSE;            
        }
        else
        {
            /**
             * WITH GRANT OPTION 이 없는 경우
             */
            
            if ( sFoundDesc->mWithGrant == aPrivDesc->mWithGrant )
            {
                /**
                 * 동일하므로 변경할 필요 없음 
                 */

                sNeedInsert = STL_FALSE;                
            }
            else
            {
                /**
                 * 기존 Privilege 를 제거하고 WITH GRANT OPTION 을 추가적으로 부여함
                 */

                sNeedInsert = STL_TRUE;

                /**
                 * WITH GRANT OPTION 만 추가될때, Privilege Descripor 에 해당하는 Row 를 삭제
                 */

                STL_TRY( ellRevokePrivDescAndCache( aTransID,
                                                    aStmt,
                                                    STL_FALSE, /* aGrantOption */
                                                    aPrivDesc,
                                                    aEnv )
                         == STL_SUCCESS );
            }
        }
    }

    /**
     * Descriptor 를 추가할 필요가 있다면 추가함
     */
    
    if ( sNeedInsert == STL_TRUE )
    {
        switch ( aPrivDesc->mPrivObject )
        {
            case ELL_PRIV_DATABASE:
                {
                    STL_TRY( ellInsertDatabasePrivDesc( aTransID,
                                                        aStmt,
                                                        aPrivDesc->mGrantorID,
                                                        aPrivDesc->mGranteeID,
                                                        aPrivDesc->mPrivAction.mDatabaseAction,
                                                        aPrivDesc->mWithGrant,
                                                        aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_PRIV_SPACE:
                {
                    STL_TRY( ellInsertTablespacePrivDesc( aTransID,
                                                          aStmt,
                                                          aPrivDesc->mGrantorID,
                                                          aPrivDesc->mGranteeID,
                                                          aPrivDesc->mObjectID,
                                                          aPrivDesc->mPrivAction.mSpaceAction,
                                                          aPrivDesc->mWithGrant,
                                                          aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_PRIV_SCHEMA:
                {
                    STL_TRY( ellInsertSchemaPrivDesc( aTransID,
                                                      aStmt,
                                                      aPrivDesc->mGrantorID,
                                                      aPrivDesc->mGranteeID,
                                                      aPrivDesc->mObjectID,
                                                      aPrivDesc->mPrivAction.mSchemaAction,
                                                      aPrivDesc->mWithGrant,
                                                      aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_PRIV_TABLE:
                {
                    STL_TRY( ellGetTableDictHandleByID( aTransID,
                                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                        aPrivDesc->mObjectID,
                                                        & sObjectHandle,
                                                        & sObjectExist,
                                                        aEnv )
                             == STL_SUCCESS );
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_OBJECT_NOT_EXIST );
                    
                    STL_TRY( ellInsertTablePrivDesc( aTransID,
                                                     aStmt,
                                                     aPrivDesc->mGrantorID,
                                                     aPrivDesc->mGranteeID,
                                                     ellGetTableSchemaID( & sObjectHandle ),
                                                     aPrivDesc->mObjectID,
                                                     aPrivDesc->mPrivAction.mTableAction,
                                                     aPrivDesc->mWithGrant,
                                                     STL_FALSE,  /* aWithHierarchy */
                                                     aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_PRIV_USAGE:
                {
                    STL_TRY( ellGetSequenceDictHandleByID( aTransID,
                                                           ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                           aPrivDesc->mObjectID,
                                                           & sObjectHandle,
                                                           & sObjectExist,
                                                           aEnv )
                             == STL_SUCCESS );
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_OBJECT_NOT_EXIST );
                    
                    STL_TRY( ellInsertUsagePrivDesc( aTransID,
                                                     aStmt,
                                                     aPrivDesc->mGrantorID,
                                                     aPrivDesc->mGranteeID,
                                                     ellGetSequenceSchemaID( & sObjectHandle ),
                                                     aPrivDesc->mObjectID,
                                                     aPrivDesc->mPrivAction.mObjectType,
                                                     aPrivDesc->mWithGrant,
                                                     aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_PRIV_COLUMN:
                {
                    STL_TRY( ellGetColumnDictHandleByID( aTransID,
                                                         ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                         aPrivDesc->mObjectID,
                                                         & sObjectHandle,
                                                         & sObjectExist,
                                                         aEnv )
                             == STL_SUCCESS );
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_OBJECT_NOT_EXIST );
                    
                    STL_TRY( ellInsertColumnPrivDesc( aTransID,
                                                      aStmt,
                                                      aPrivDesc->mGrantorID,
                                                      aPrivDesc->mGranteeID,
                                                      ellGetColumnSchemaID( & sObjectHandle ),
                                                      ellGetColumnTableID( & sObjectHandle ),
                                                      aPrivDesc->mObjectID,
                                                      aPrivDesc->mPrivAction.mColumnAction,
                                                      aPrivDesc->mWithGrant,
                                                      aEnv )
                             == STL_SUCCESS );
                    break;
                }
            default:
                {
                    STL_ASSERT(0);
                    break;
                }
        }
    }
    else
    {
        /**
         * nothing to do
         */
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OBJECT_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INVALID_OBJECT,
                      NULL,
                      ELL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief REVOKE PRIVILEGE 를 위한 privilege descriptor 를 dictionary 와 cache  추가한다.
 * @param[in] aTransID      Transaction ID
 * @param[in] aStmt         Statement
 * @param[in] aGrantOption  GRANT OPTION
 * @param[in] aPrivDesc     Privilege Descriptor
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus ellRevokePrivDescAndCache( smlTransId     aTransID,
                                     smlStatement * aStmt,
                                     stlBool        aGrantOption,
                                     ellPrivDesc  * aPrivDesc,
                                     ellEnv       * aEnv )
{
    ellDictHandle   sPrivHandle;
    ellPrivDesc   * sHandleDesc;
    stlBool         sPrivExist = STL_FALSE;

    ellDictHandle   sObjectHandle;
    stlBool         sObjectExist;
    
    stlBool         sNeedDelete = STL_FALSE;
    stlBool         sNeedInsert = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Privilege Handle 이 존재하는 지 확인
     */ 

    STL_TRY( ellGetPrivDescHandle( aTransID,
                                   ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                   aPrivDesc,
                                   & sPrivHandle,
                                   & sPrivExist,
                                   aEnv )
             == STL_SUCCESS );

    /**
     * 동시에 수행된 REVOKE 에 의해 이미 제거되었는지 검사
     */
    
    STL_TRY_THROW( sPrivExist == STL_TRUE, RAMP_FINISH );

    /**
     * 삽입, 삭제가 필요한 지 확인
     */

    sNeedDelete = STL_FALSE;

    sHandleDesc = ellGetPrivDesc( & sPrivHandle );
    
    if ( aGrantOption == STL_TRUE )
    {
        if ( sHandleDesc->mWithGrant == STL_TRUE )
        {
            sNeedDelete = STL_TRUE;
            sNeedInsert = STL_TRUE;
        }
        else
        {
            sNeedDelete = STL_FALSE;
            sNeedInsert = STL_FALSE;
        }
    }
    else
    {
        sNeedDelete = STL_TRUE;
        sNeedInsert = STL_FALSE;
    }

    /**
     * 삭제가 필요한 경우
     */

    if ( sNeedDelete == STL_TRUE )
    {
        STL_TRY( eldDeletePrivRowAndCacheByPrivDesc( aTransID,
                                                     aStmt,
                                                     aPrivDesc,
                                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* nothing to do */
    }
    
    /**
     * 삽입이 필요한 경우
     */

    if ( sNeedInsert == STL_TRUE )
    {
        switch ( aPrivDesc->mPrivObject )
        {
            case ELL_PRIV_DATABASE:
                {
                    STL_TRY( ellInsertDatabasePrivDesc( aTransID,
                                                        aStmt,
                                                        aPrivDesc->mGrantorID,
                                                        aPrivDesc->mGranteeID,
                                                        aPrivDesc->mPrivAction.mDatabaseAction,
                                                        STL_FALSE,  /* aIsGrantable */
                                                        aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_PRIV_SPACE:
                {
                    STL_TRY( ellInsertTablespacePrivDesc( aTransID,
                                                          aStmt,
                                                          aPrivDesc->mGrantorID,
                                                          aPrivDesc->mGranteeID,
                                                          aPrivDesc->mObjectID,
                                                          aPrivDesc->mPrivAction.mSpaceAction,
                                                          STL_FALSE,  /* aIsGrantable */
                                                          aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_PRIV_SCHEMA:
                {
                    STL_TRY( ellInsertSchemaPrivDesc( aTransID,
                                                      aStmt,
                                                      aPrivDesc->mGrantorID,
                                                      aPrivDesc->mGranteeID,
                                                      aPrivDesc->mObjectID,
                                                      aPrivDesc->mPrivAction.mSchemaAction,
                                                      STL_FALSE,  /* aIsGrantable */
                                                      aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_PRIV_TABLE:
                {
                    STL_TRY( ellGetTableDictHandleByID( aTransID,
                                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                        aPrivDesc->mObjectID,
                                                        & sObjectHandle,
                                                        & sObjectExist,
                                                        aEnv )
                             == STL_SUCCESS );
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_OBJECT_NOT_EXIST );
                    
                    STL_TRY( ellInsertTablePrivDesc( aTransID,
                                                     aStmt,
                                                     aPrivDesc->mGrantorID,
                                                     aPrivDesc->mGranteeID,
                                                     ellGetTableSchemaID( & sObjectHandle ),
                                                     aPrivDesc->mObjectID,
                                                     aPrivDesc->mPrivAction.mTableAction,
                                                     STL_FALSE,  /* aIsGrantable */
                                                     STL_FALSE,  /* aWithHierarchy */
                                                     aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_PRIV_USAGE:
                {
                    STL_TRY( ellGetSequenceDictHandleByID( aTransID,
                                                           ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                           aPrivDesc->mObjectID,
                                                           & sObjectHandle,
                                                           & sObjectExist,
                                                           aEnv )
                             == STL_SUCCESS );
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_OBJECT_NOT_EXIST );
                    
                    STL_TRY( ellInsertUsagePrivDesc( aTransID,
                                                     aStmt,
                                                     aPrivDesc->mGrantorID,
                                                     aPrivDesc->mGranteeID,
                                                     ellGetSequenceSchemaID( & sObjectHandle ),
                                                     aPrivDesc->mObjectID,
                                                     aPrivDesc->mPrivAction.mObjectType,
                                                     STL_FALSE,  /* aIsGrantable */
                                                     aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case ELL_PRIV_COLUMN:
                {
                    STL_TRY( ellGetColumnDictHandleByID( aTransID,
                                                         ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                         aPrivDesc->mObjectID,
                                                         & sObjectHandle,
                                                         & sObjectExist,
                                                         aEnv )
                             == STL_SUCCESS );
                    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_OBJECT_NOT_EXIST );
                    
                    STL_TRY( ellInsertColumnPrivDesc( aTransID,
                                                      aStmt,
                                                      aPrivDesc->mGrantorID,
                                                      aPrivDesc->mGranteeID,
                                                      ellGetColumnSchemaID( & sObjectHandle ),
                                                      ellGetColumnTableID( & sObjectHandle ),
                                                      aPrivDesc->mObjectID,
                                                      aPrivDesc->mPrivAction.mColumnAction,
                                                      STL_FALSE,  /* aIsGrantable */
                                                      aEnv )
                             == STL_SUCCESS );
                    break;
                }
            default:
                {
                    STL_ASSERT(0);
                    break;
                }
        }
    }
    else
    {
        /**
         * nothing to do
         */
    }

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OBJECT_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INVALID_OBJECT,
                      NULL,
                      ELL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/** @} ellSqlObject */
