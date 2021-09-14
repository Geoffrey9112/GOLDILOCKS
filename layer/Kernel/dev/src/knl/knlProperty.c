/*******************************************************************************
 * knlProperty.c
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
 * @file knlProperty.c
 * @brief Kernel Layer Property Routines
 */
#include <knl.h>

#include <knDef.h>
#include <knpDef.h>
#include <knpPropertyManager.h>
#include <knpPropertyFile.h>


/**
 * @addtogroup knlProperty
 * @{
 */


/**
 * @brief Heap Memory 영역에 Property를 초기화한다.
 * @param[in,out] aKnlEnv       Kernel Environment
 * @remarks
 * <BR> No-Mount 수행중 Shared Memory가 구성되지 않은 단계에서 필요한 Property를 사용하기 위해서
 * <BR> Heap Memory 영역을 생성하고 구성한다.
 */

stlStatus knlInitializePropertyInHeap( knlEnv * aKnlEnv )
{
    return knpInitializePropertyInHeap( aKnlEnv );
}


/**
 * @brief Heap Memory 영역에 Property를 제거한다.
 * @param[in,out] aKnlEnv       Kernel Environment
 * @remarks
 * <BR> Heap Memory 영역에 구성된 메모리를 제거한다.
 */

stlStatus knlFinalizePropertyInHeap( knlEnv * aKnlEnv )
{
    return knpFinalizePropertyInHeap( aKnlEnv );
}


/**
 * @brief Shared Memory를 구성하고, Heap Memory 영역에 구성된 Property를 복사한다.
 * @param[in,out] aKnlEnv       Kernel Environment
 * @remarks
 * <BR> No-Mount 수행중 Shared Memory가 구성된 이후에 수행되며, Shared Memory's Static Area의 Fixed Area에 존재한다.
 * <BR> Heap Memory 영역에 구성된 메모리는 복사한 뒤에 제거한다.
 */

stlStatus knlInitializePropertyInShm( knlEnv * aKnlEnv )
{
    return knpInitializePropertyInShm( aKnlEnv );
}


/**
 * @brief Warm-up 단계에 사용될 초기화
 * @param[in,out] aKnlEnv      Kernel Environment
 * @remarks
 * <BR> Shared Memory의 Attach한 Process 들이 Property Manager를 사용할 수 있도록 한다.
 * <BR> Warm up 단계에서 호출되어야 한다.
 */

stlStatus knlInitializePropertyAtWarmup ( knlEnv * aKnlEnv )
{
    return knpInitializePropertyAtWarmup( aKnlEnv );
}


/**
 * @brief Property 이름으로 ID을 얻어온다.
 * @param[in]       aPropertyName        검색할 프로퍼티 이름
 * @param[out]      aPropertyID          knlPropertyID
 * @param[in,out]   aKnlEnv              Kernel Environment
 */

stlStatus knlGetPropertyIDByName( stlChar        * aPropertyName,
                                  knlPropertyID  * aPropertyID, 
                                  knlEnv         * aKnlEnv )
{
    return knpGetPropertyIDByName( aPropertyName,
                                   aPropertyID,
                                   aKnlEnv );
}


/**
 * @brief Property ID로 이름을 얻어온다.
 * @param[in]       aPropertyID          knlPropertyID
 * @param[out]      aPropertyName        프로퍼티 이름
 * @param[in,out]   aKnlEnv              Kernel Environment
 */

stlStatus knlGetPropertyNameByID ( knlPropertyID    aPropertyID,
                                   stlChar        * aPropertyName,
                                   knlEnv         * aKnlEnv )
{
    return  knpGetPropertyNameByID( aPropertyID,
                                    aPropertyName,
                                    aKnlEnv ); 
}


/**
 * @brief Property Name으로 Property가 존재하는지 검사한다.
 * @param[in]     aPropertyName        PropertyName
 * @param[out]    aIsExist             Property 존재 여부
 * @param[in,out] aKnlEnv              Kernel Environment
 */

stlStatus knlExistPropertyByName ( stlChar        * aPropertyName,
                                   stlBool        * aIsExist,
                                   knlEnv         * aKnlEnv )
{
    return  knpExistPropertyByName( aPropertyName,
                                    aIsExist,
                                    aKnlEnv ); 
}


/**
 * @brief 프로퍼티 이름으로 해당 Property를 검색하고 프로퍼티에 저장되어있는 값을 얻어온다.
 * @param[in]       aPropertyName      프로퍼티 이름
 * @param[out]      aValue             프로퍼티에 저장된 값
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knlGetPropertyValueByName ( stlChar    * aPropertyName,
                                      void       * aValue,
                                      knlEnv     * aKnlEnv )
{
    void *sPropertyHandle = NULL;
    
    /**
     * 프로퍼티 이름으로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByName( aPropertyName,
                                         &sPropertyHandle,
                                         aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 프로퍼티에 저장된 값을 얻어온다.
     */
    STL_TRY( knpGetPropertyValue( sPropertyHandle,
                                  aValue,
                                  aKnlEnv ) == STL_SUCCESS );
     
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief 프로퍼티 ID로 프로퍼티에 저장되어있는 값을 얻어온다.
 * @param[in]       aPropertyID        knlPropertyID
 * @param[out]      aValue             프로퍼티에 저장된 값
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knlGetPropertyValueByID ( knlPropertyID    aPropertyID,
                                    void           * aValue,
                                    knlEnv         * aKnlEnv )
{
    void *sPropertyHandle = NULL;
    
    /**
     * 프로퍼티 ID로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByID( aPropertyID, 
                                       &sPropertyHandle, 
                                       aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 프로퍼티에 저장된 값을 얻어온다.
     */
    STL_TRY( knpGetPropertyValue( sPropertyHandle,
                                  aValue,
                                  aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;  
}


/**
 * @brief 프로퍼티 ID로 프로퍼티에 저장되어있는 Boolean 값을 얻어온다.
 * @param[in] aPropertyID        knlPropertyID
 * @param[in] aKnlEnv            Kernel Environment
 * @remarks
 */
stlBool knlGetPropertyBoolValueByID( knlPropertyID    aPropertyID,
                                     knlEnv         * aKnlEnv )
{
    /**
     * 프로퍼티에 저장된 값을 얻어온다.
     */
    
    return knpGetPropertyBoolValue( aPropertyID, aKnlEnv );
}


/**
 * @brief 프로퍼티 ID로 프로퍼티에 저장되어있는 Bigint 값을 얻어온다.
 * @param[in] aPropertyID        knlPropertyID
 * @param[in] aKnlEnv            Kernel Environment
 * @remarks
 */
stlInt64 knlGetPropertyBigIntValueByID ( knlPropertyID    aPropertyID,
                                         knlEnv         * aKnlEnv )
{
    /**
     * 프로퍼티에 저장된 값을 얻어온다.
     */
    
    return knpGetPropertyBigIntValue( aPropertyID, aKnlEnv );
}


/**
 * @brief 프로퍼티 ID로 프로퍼티에 저장되어있는 String Pointer 값을 얻어온다.
 * @param[in] aPropertyID        knlPropertyID
 * @param[in] aKnlEnv            Kernel Environment
 * @remarks
 */
stlChar * knlGetPropertyStringValueByID ( knlPropertyID    aPropertyID,
                                          knlEnv         * aKnlEnv )
{
    /**
     * 프로퍼티에 저장된 값을 얻어온다.
     */

    /**
     * @todo String 계열의 Property 를 획득할 수 있는 체계가 필요함.
     */

    return knpGetPropertyStringValue( aPropertyID, aKnlEnv );
}


/**
 * @brief 프로퍼티 ID로 프로퍼티에 저장되어있는 Boolean 값을 Shared Memory에서 얻어온다.
 * @param[in] aPropertyID        knlPropertyID
 * @remarks
 * <BR> SessionEnv에서 얻어오기 위해서는 knlGetPropertyBoolValueByID를 사용해야 하며,
 * <BR> Shared Memory에서 얻어와야 한다면 이 함수를 사용해야 한다.
 */
stlBool knlGetPropertyBoolValueAtShmByID( knlPropertyID aPropertyID )
{
    return knpGetPropertyBoolValueAtShm( aPropertyID );   
}


/**
 * @brief 프로퍼티 ID로 프로퍼티에 저장되어있는 Boolean 값을 Session Environment에서 얻어온다.
 * @param[in] aPropertyID        knlPropertyID
 * @param[in] aSessionEnv        knlSessionEnv
 * @remarks
 * <BR> SessionEnv에서 직접얻어오기 위한 함수이다.
 */
stlBool knlGetPropertyBoolValue4SessEnvById ( knlPropertyID   aPropertyID,
                                              knlSessionEnv * aSessionEnv )
{
    return knpGetPropertyBoolValue4SessEnv( aPropertyID, aSessionEnv );
}

/**
 * @brief 프로퍼티 ID로 프로퍼티에 저장되어있는 BigInt값을 Shared Memory에서 얻어온다.
 * @param[in] aPropertyID        knlPropertyID
 * @remarks
 * <BR> SessionEnv에서 얻어오기 위해서는 knlGetPropertyBigIntValueByID를 사용해야 하며,
 * <BR> Shared Memory에서 얻어와야 한다면 이 함수를 사용해야 한다.
 */
stlInt64 knlGetPropertyBigIntValueAtShmByID ( knlPropertyID aPropertyID )
{
    return knpGetPropertyBigIntValueAtShm( aPropertyID );
}


/**
 * @brief 프로퍼티 ID로 프로퍼티에 저장되어있는 BigInt값을 Session Environment에서 얻어온다.
 * @param[in] aPropertyID        knlPropertyID
 * @param[in] aSessionEnv        knlSessionEnv
 * @remarks
 * <BR> SessionEnv에서 직접얻어오기 위한 함수이다.
 */
stlInt64 knlGetPropertyBigIntValue4SessEnvById ( knlPropertyID   aPropertyID,
                                                 knlSessionEnv * aSessionEnv )
{
    return knpGetPropertyBigIntValue4SessEnv( aPropertyID, aSessionEnv );
}

/**
 * @brief 프로퍼티 이름으로 해당 Property를 검색하고 프로퍼티에 저장되어있는 값의 길이를 얻어온다.
 * @param[in]       aPropertyName      프로퍼티 이름
 * @param[out]      aValue             얻어올 프로퍼티에 저장된 값의 길이
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knlGetPropertyValueMaxLenByName( stlChar    * aPropertyName,
                                           stlUInt32  * aValue, 
                                           knlEnv     * aKnlEnv )
{
    void *sPropertyHandle = NULL;
    
    /**
     * 프로퍼티 이름으로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByName( aPropertyName,
                                         &sPropertyHandle,
                                         aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 프로퍼티에 저장된 값의 길이를 얻어온다.
     */
    STL_TRY( knpGetPropertyValueMaxLen( sPropertyHandle,
                                        aValue,
                                        aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;        
}


/**
 * @brief 프로퍼티 ID로 해당 Property를 검색하고 프로퍼티에 저장되어있는 값의 길이를 얻어온다.
 * @param[in]       aPropertyID        knlPropertyID
 * @param[out]      aValue             얻어올 프로퍼티에 저장된 값의 길이
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knlGetPropertyValueMaxLenByID ( knlPropertyID    aPropertyID,
                                          stlUInt32      * aValue, 
                                          knlEnv         * aKnlEnv )
{
    void *sPropertyHandle = NULL;
    
    /**
     * 프로퍼티 ID로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByID( aPropertyID, 
                                       &sPropertyHandle, 
                                       aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 프로퍼티에 저장된 값의 길이를 얻어온다.
     */
    STL_TRY( knpGetPropertyValueMaxLen( sPropertyHandle,
                                        aValue,
                                        aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;        
}


/**
 * @brief 프로퍼티 이름으로 해당 Property를 검색하고 문자값을 변경한다.
 * @param[in]       aPropertyName      프로퍼티 이름
 * @param[in]       aValue             변경할 값
 * @param[in]       aMode              knlPropertyModifyMode
 * @param[in]       aIsDeferred        DEFERRED 옵션
 * @param[in]       aScope             knlPropertyModifyScope
 * @param[in,out]   aKnlEnv            Kernel Environment
 * @remarks
 * <BR> 변경할 값은 해당 데이터타입의 값이여야 한다.
 */

stlStatus knlUpdatePropertyValueByName( stlChar                * aPropertyName,
                                        void                   * aValue, 
                                        knlPropertyModifyMode    aMode,
                                        stlBool                  aIsDeferred,
                                        knlPropertyModifyScope   aScope,
                                        knlEnv                 * aKnlEnv )
{
    void *sPropertyHandle = NULL;

    /**
     * 프로퍼티 이름으로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByName( aPropertyName,
                                         &sPropertyHandle,
                                         aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 새로운 프로퍼티 Value를 업데이트 한다.
     */ 
    STL_TRY( knpUpdatePropertyValue( sPropertyHandle,
                                     aValue,
                                     aMode,
                                     aIsDeferred,
                                     aScope,
                                     aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;      
}


/**
 * @brief 프로퍼티 ID로 해당 Property를 검색하고 문자값을 변경한다.
 * @param[in]       aPropertyID        knlPropertyID
 * @param[in]       aValue             변경할 값
 * @param[in]       aMode              knlPropertyModifyMode
 * @param[in]       aIsDeferred        DEFERRED 옵션
 * @param[in]       aScope             knlPropertyModifyScope
 * @param[in,out]   aKnlEnv            Kernel Environment
 * @remarks
 * <BR> 변경할 값은 해당 데이터타입의 값이여야 한다.
 */

stlStatus knlUpdatePropertyValueByID( knlPropertyID            aPropertyID,
                                      void                   * aValue, 
                                      knlPropertyModifyMode    aMode,
                                      stlBool                  aIsDeferred,
                                      knlPropertyModifyScope   aScope,
                                      knlEnv                 * aKnlEnv )
{
    void *sPropertyHandle = NULL;
    
    /**
     * 프로퍼티 ID로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByID( aPropertyID, 
                                       &sPropertyHandle, 
                                       aKnlEnv ) == STL_SUCCESS );
        
    /**
     * 새로운 프로퍼티 Value를 업데이트 한다.
     */ 
    STL_TRY( knpUpdatePropertyValue( sPropertyHandle,
                                     aValue,
                                     aMode,
                                     aIsDeferred,
                                     aScope,
                                     aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;      
}


/**
 * @brief 프로퍼티 이름으로 해당 Property를 검색하고 프로퍼티의 Modify Type을 얻어온다.
 * @param[in]       aPropertyName      프로퍼티 이름
 * @param[out]      aType              knlPropertyModifyType
 * @param[in,out]   aKnlEnv            Kernel Environment
 * @remarks
 * <BR> ModifyType은 ALTER SYSTEM 질의 시에 사용할 수 있으며 다음과 같다.
 * <BR>
 * <BR> 1. KNL_PROPERTY_SYS_TYPE_IMMEDIATE : 현재 접속되어 있는 세션 및 이후 접속되는 세션에 영향을 받는다.
 * <BR> 2. KNL_PROPERTY_SYS_TYPE_DEFERRED : 이후 접속되는 세션부터 변경된 프로퍼티의 영향을 받는다.
 * <BR> 3. KNL_PROPERTY_SYS_TYPE_FALSE : 운용중에는 변경할 수 없으며, ALTER SYSTEM SCOPE=FALSE를 사용해서만 변경 가능하며 Restart가 필요하다.
 * <BR> 4. KNL_PROPERTY_SYS_TYPE_NONE : ALTER SYSTEMS으로 변경 불가능
 */

stlStatus knlGetPropertySysModTypeByName ( stlChar                * aPropertyName,
                                           knlPropertyModifyType  * aType,
                                           knlEnv                 * aKnlEnv )
{
    void *sPropertyHandle = NULL;
    
    /**
     * 프로퍼티 이름으로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByName( aPropertyName,
                                         &sPropertyHandle,
                                         aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 해당 프로퍼티의 Modify Type을 얻어온다.
     */
    STL_TRY( knpGetPropertySysModType( sPropertyHandle,
                                       aType,
                                       aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;     
}


/**
 * @brief 프로퍼티 ID로 해당 Property를 검색하고 프로퍼티의 Modify Type을 얻어온다.
 * @param[in]       aPropertyID        knlPropertyID
 * @param[out]      aType              knlPropertyModifyType
 * @param[in,out]   aKnlEnv            Kernel Environment
 * @remarks
 * <BR> ModifyType은 ALTER SYSTEM 질의 시에 사용할 수 있으며 다음과 같다.
 * <BR>
 * <BR> 1. KNL_PROPERTY_SYS_TYPE_IMMEDIATE : 현재 접속되어 있는 세션 및 이후 접속되는 세션에 영향을 받는다.
 * <BR> 2. KNL_PROPERTY_SYS_TYPE_DEFERRED : 이후 접속되는 세션부터 변경된 프로퍼티의 영향을 받는다.
 * <BR> 3. KNL_PROPERTY_SYS_TYPE_FALSE : 운용중에는 변경할 수 없으며, ALTER SYSTEM SCOPE=FALSE를 사용해서만 변경 가능하며 Restart가 필요하다.
 * <BR> 4. KNL_PROPERTY_SYS_TYPE_NONE : ALTER SYSTEMS으로 변경 불가능
 */

stlStatus knlGetPropertySysModTypeByID ( knlPropertyID            aPropertyID,
                                         knlPropertyModifyType  * aType,
                                         knlEnv                 * aKnlEnv )
{
    void *sPropertyHandle = NULL;
    
    /**
     * 프로퍼티 ID로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByID( aPropertyID, 
                                       &sPropertyHandle, 
                                       aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 해당 프로퍼티의 Modify Type을 얻어온다.
     */
    STL_TRY( knpGetPropertySysModType( sPropertyHandle,
                                       aType,
                                       aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;     
}

/**
 * @brief 프로퍼티 이름으로 해당 Property를 검색하고 ALTER SESSION 질의로 프로퍼티를 변경가능여부를 얻어온다.
 * @param[in]       aPropertyName      프로퍼티 이름
 * @param[out]      aType              ALTER SESSION 가능 여부
 * @param[in,out]   aKnlEnv            Kernel Environment
 * @remarks
 * <BR> 얻어진 aType이 STL_TRUE일 경우에 ALTER SESSION 질의로 변경 가능하다.
 */

stlStatus knlGetPropertySesModTypeByName ( stlChar  * aPropertyName,
                                           stlBool  * aType,
                                           knlEnv   * aKnlEnv )
{
    void *sPropertyHandle = NULL;
    
    /**
     * 프로퍼티 이름으로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByName( aPropertyName,
                                         &sPropertyHandle,
                                         aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 프로퍼티의 Alter Session 가능 여부를 얻어온다.
     */
    STL_TRY( knpGetPropertySesModType( sPropertyHandle,
                                       aType,
                                       aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;     
}


/**
 * @brief 프로퍼티 ID으로 해당 Property를 검색하고 ALTER SESSION 질의로 프로퍼티를 변경가능여부를 얻어온다.
 * @param[in]       aPropertyID        knlPropertyID    
 * @param[out]      aType              ALTER SESSION 가능 여부
 * @param[in,out]   aKnlEnv            Kernel Environment
 * @remarks
 * <BR> 얻어진 aType이 STL_TRUE일 경우에 ALTER SESSION 질의로 변경 가능하다.
 */

stlStatus knlGetPropertySesModTypeByID ( knlPropertyID    aPropertyID,
                                         stlBool        * aType,
                                         knlEnv         * aKnlEnv )
{
    void *sPropertyHandle = NULL;
    
    /**
     * 프로퍼티 ID로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByID( aPropertyID, 
                                       &sPropertyHandle, 
                                       aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 프로퍼티의 Alter Session 가능 여부를 얻어온다.
     */
    STL_TRY( knpGetPropertySesModType( sPropertyHandle,
                                       aType,
                                       aKnlEnv ) == STL_SUCCESS );
        
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE; 
    
}


/**
 * @brief 프로퍼티 이름으로 해당 Property를 검색하고 프로퍼티를 변경가능여부를 얻어온다.
 * @param[in]       aPropertyName      프로퍼티 이름
 * @param[out]      aType              변경 가능 여부
 * @param[in,out]   aKnlEnv            Kernel Environment
 * @remarks
 * <BR> 얻어진 aType이 STL_TRUE일 경우에 ALTER SESSION 질의로 변경 가능하다.
 */

stlStatus knlGetPropertyModifiableByName ( stlChar  * aPropertyName,
                                           stlBool  * aType,
                                           knlEnv   * aKnlEnv )
{
    void *sPropertyHandle = NULL;

    /**
     * 프로퍼티 이름으로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByName( aPropertyName,
                                         &sPropertyHandle,
                                         aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 프로퍼티의 변경가능 여부를 얻어온다.
     */
    STL_TRY( knpGetPropertyModifiable( sPropertyHandle,
                                       aType,
                                       aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 프로퍼티 ID으로 해당 Property를 검색하고 프로퍼티를 변경가능여부를 얻어온다.
 * @param[in]       aPropertyID        knlPropertyID    
 * @param[out]      aType              변경 가능 여부
 * @param[in,out]   aKnlEnv            Kernel Environment
 * @remarks
 * <BR> 얻어진 aType이 STL_TRUE일 경우에 ALTER SESSION 질의로 변경 가능하다.
 */

stlStatus knlGetPropertyModifiableByID ( knlPropertyID    aPropertyID,
                                         stlBool        * aType,
                                         knlEnv         * aKnlEnv )
{
    void *sPropertyHandle = NULL;
    
    /**
     * 프로퍼티 ID로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByID( aPropertyID, 
                                       &sPropertyHandle, 
                                       aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 프로퍼티의 변경가능 여부를 얻어온다.
     */
    STL_TRY( knpGetPropertyModifiable( sPropertyHandle,
                                       aType,
                                       aKnlEnv ) == STL_SUCCESS );    
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE; 
    
}


/**
 * @brief 프로퍼티 이름으로 내부 Datatype을 얻어온다.
 * @param[in]       aPropertyName      프로퍼티 이름
 * @param[out]      aType              knlPropertyDataType
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knlGetPropertyDataTypeByName ( stlChar             * aPropertyName,
                                         knlPropertyDataType * aType,
                                         knlEnv              * aKnlEnv )
{
    void *sPropertyHandle = NULL;

    /**
     * 프로퍼티 이름으로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByName( aPropertyName,
                                         &sPropertyHandle,
                                         aKnlEnv ) == STL_SUCCESS );    
    
    /**
     * 프로퍼티의 데이터 타입을 얻어온다.
     */
    STL_TRY( knpGetPropertyDataType( sPropertyHandle,
                                     aType,
                                     aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief 프로퍼티 ID로 내부 Datatype을 얻어온다.
 * @param[in]       aPropertyID        knlPropertyID
 * @param[out]      aType              knlPropertyDataType
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knlGetPropertyDataTypeByID ( knlPropertyID         aPropertyID,
                                       knlPropertyDataType * aType,
                                       knlEnv              * aKnlEnv )
{
    void *sPropertyHandle = NULL;
    
    /**
     * 프로퍼티 ID로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByID( aPropertyID, 
                                       &sPropertyHandle, 
                                       aKnlEnv ) == STL_SUCCESS ); 
    
    /**
     * 프로퍼티의 데이터 타입을 얻어온다.
     */
    STL_TRY( knpGetPropertyDataType( sPropertyHandle,
                                     aType,
                                     aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief 프로퍼티 ID 로 Input String 을 Bigint 값으로 변경한다.
 * @param[in]       aPropertyID        knlPropertyID
 * @param[in]       aInStr             stlChar Type의 단위를 포함한 Value
 * @param[out]      aValue             단위를 변환한 결과 값
 * @param[in,out]   aKnlEnv            Kernel Environment
 */
stlStatus knlConvertUnitToValue( knlPropertyID         aPropertyID,
                                 stlChar             * aInStr,
                                 stlInt64            * aValue,
                                 knlEnv              * aKnlEnv )
{
    void *sPropertyHandle = NULL;
    
    /**
     * 프로퍼티 ID로 Handle을 얻어온다.
     */
    
    STL_TRY( knpGetPropertyHandleByID( aPropertyID, 
                                       &sPropertyHandle, 
                                       aKnlEnv ) == STL_SUCCESS ); 

    /**
     * Input String 을 Bigint Value 로 변환한다.
     */

    STL_TRY( knpConvertUnitToValue( sPropertyHandle,
                                    aInStr,
                                    aValue,
                                    aKnlEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief 프로퍼티 이름으로 Property Value를 초기화한다.
 * @param[in]       aPropertyName      프로퍼티 이름
 * @param[in]       aMode              knlPropertyModifyMode
 * @param[in]       aIsDeferred        DEFERRED 옵션
 * @param[in]       aScope             knlPropertyModifyScope
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knlInitPropertyValueByName( stlChar                * aPropertyName,
                                      knlPropertyModifyMode    aMode,
                                      stlBool                  aIsDeferred,
                                      knlPropertyModifyScope   aScope,
                                      knlEnv                 * aKnlEnv )
{
    void *sPropertyHandle = NULL;

    /**
     * 프로퍼티 이름으로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByName( aPropertyName,
                                         &sPropertyHandle,
                                         aKnlEnv ) == STL_SUCCESS );    
    
    /**
     * 프로퍼티 Value를 초기화 한다.
     */ 
    STL_TRY( knpInitPropertyValue( sPropertyHandle,
                                   aMode,
                                   aIsDeferred,
                                   aScope,
                                   aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 프로퍼티 ID로 Property Value를 초기화한다.
 * @param[in]       aPropertyID        knlPropertyID
 * @param[out]      aMode              knlPropertyModifyMode
 * @param[in]       aIsDeferred        DEFERRED 옵션
 * @param[in]       aScope             knlPropertyModifyScope
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knlInitPropertyValueByID( knlPropertyID            aPropertyID,
                                    knlPropertyModifyMode    aMode,
                                    stlBool                  aIsDeferred,
                                    knlPropertyModifyScope   aScope,
                                    knlEnv                 * aKnlEnv )
{
    void *sPropertyHandle = NULL;
    
    /**
     * 프로퍼티 ID로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByID( aPropertyID, 
                                       &sPropertyHandle, 
                                       aKnlEnv ) == STL_SUCCESS ); 
    
    /**
     * 프로퍼티 Value를 초기화 한다.
     */ 
    STL_TRY( knpInitPropertyValue( sPropertyHandle,
                                   aMode,
                                   aIsDeferred,
                                   aScope,
                                   aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief 프로퍼티 파일에서 프로퍼티 이름으로 Property Value를 삭제한다.
 * @param[in]       aPropertyName      프로퍼티 이름
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knlRemovePropertyValueByName( stlChar * aPropertyName,
                                        knlEnv  * aKnlEnv )
{
    void *sPropertyHandle = NULL;

    /**
     * 프로퍼티 이름으로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByName( aPropertyName,
                                         &sPropertyHandle,
                                         aKnlEnv ) == STL_SUCCESS );    
    
    /**
     * 프로퍼티 파일에서 프로퍼티를 삭제한다.
     */
    STL_TRY( knpRemovePropertyValueAtFile( sPropertyHandle,
                                           aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 프로퍼티 파일에서 프로퍼티 ID로 Property Value를 삭제한다.
 * @param[in]       aPropertyID        knlPropertyID
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knlRemovePropertyValueByID( knlPropertyID   aPropertyID,
                                      knlEnv        * aKnlEnv )
{
    void *sPropertyHandle = NULL;
    
    /**
     * 프로퍼티 ID로 Handle을 얻어온다.
     */
    STL_TRY( knpGetPropertyHandleByID( aPropertyID, 
                                       &sPropertyHandle, 
                                       aKnlEnv ) == STL_SUCCESS ); 
    
    /**
     * 프로퍼티 파일에서 프로퍼티를 삭제한다.
     */
    STL_TRY( knpRemovePropertyValueAtFile( sPropertyHandle,
                                           aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
            
    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief Text 프로퍼티 파일을 Binary 프로퍼티 파일로 변환한다.
 * @param[in]       aTFileName         Text 프로퍼티 파일 이름
 * @param[in]       aBFileName         Binary 프로퍼티 파일 이름
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knlConvertPropertyTextToBinaryFile( stlChar  * aTFileName,  
                                              stlChar  * aBFileName,  
                                              knlEnv   * aKnlEnv )
{
    return knpConvertPropertyTextToBinaryFile( aTFileName,  
                                               aBFileName,  
                                               aKnlEnv );
}


/**
 * @brief Default Text 프로퍼티 파일을 Default Binary 프로퍼티 파일로 변환한다.
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knlConvertPropertyTextToBinaryFileByDefault( knlEnv  * aKnlEnv )
{
    return knpConvertPropertyTextToBinaryFileByDefault( aKnlEnv );
}


/**
 * @brief Binary 프로퍼티 파일을 Text 프로퍼티 파일로 변환한다.
 * @param[in]       aBFileName         Binary 프로퍼티 파일 이름
 * @param[in]       aTFileName         Text 프로퍼티 파일 이름
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knlConvertPropertyBinaryToTextFile( stlChar  * aBFileName,  
                                              stlChar  * aTFileName,  
                                              knlEnv   * aKnlEnv )
{
    return knpConvertPropertyBinaryToTextFile( aBFileName,  
                                               aTFileName,  
                                               aKnlEnv );
}


/**
 * @brief Default Binary 프로퍼티 파일을 Default Text 프로퍼티 파일로 변환한다.
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knlConvertPropertyBinaryToTextFileByDefault( knlEnv  * aKnlEnv )
{
    return knpConvertPropertyBinaryToTextFileByDefault( aKnlEnv );
}

/**
 * @brief Binary 프로퍼티 파일을 Dump한다.
 * @param[in]       aFileName          Binary 프로퍼티 파일이름
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knlDumpBinaryPropertyFile( stlChar * aFileName,
                                     knlEnv  * aKnlEnv )
{
    return knpDumpBinaryPropertyFile( aFileName,
                                      aKnlEnv );
}



/**
 * @brief Kernel Session Environment에 Property를 저장한다.
 * @param[out]      aSessionEnv        Session Environment
 * @param[in,out]   aKnlEnv            Kernel Environment
 * @remarks
 * <BR> Session에서 조작 가능한 Property Value 를 Kernel Envirment에 저장한다.
 */

stlStatus knlCopyPropertyValueToSessionEnv ( knlSessionEnv * aSessionEnv,
                                             knlEnv        * aKnlEnv )
{
    return knpCopyPropertyValueToSessionEnv( aSessionEnv, aKnlEnv );
}


/** @} */
