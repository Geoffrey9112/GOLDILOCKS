/*******************************************************************************
 * knpPropertyManager.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *        Property를 관리 관련된 함수
 *
 ******************************************************************************/


/**
 * @file knpPropertyManager.c
 * @brief Kernel Layer Property Internal Routines
 */

#include <knl.h>

#include <knDef.h>
#include <knpDef.h>
#include <knpDataType.h>
#include <knpPropertyManager.h>
#include <knpPropertyFile.h>

/**
 * @addtogroup knpProperty
 * @{
 */

extern knsEntryPoint * gKnEntryPoint;
knpPropertyMgr       * gKnpPropertyMgr;

/**
 * @remarks
 * <BR>
 * <BR>설명
 * <BR>1. Property가 갖을 수 있는 DataType은 Boolean/숫자형(BIGINT)/VARCHAR 이다.
 * <BR>2. Property 로딩 순서
 * <BR> - Default -> 시스템 환경 변수 -> Property Binary File || (Property Text File)
 * <BR>3. Property는 크게 READ-ONLY/WRITABLE 로 나뉘며 
 * <BR> - READ ONLY는 DB CREATE시에만 Text PropertyFile에서 변경 가능하고 그 이후에는 변경이 불가능하다. 
 * <BR> - READ ONLY는 DB create시에는 Property에서 읽어도 되지만 Startup 된 이후에는 DB또는 Control File등의 값에서 읽어야 한다.
 * <BR> - WRITABLE은 ALTER SESSION 가능 여부/ALTER SYSTEM 속성 IMMEDIATE, DEFERRED, FALSE로 나뉜다.
 * <BR> - Read Only Property를 따로 지정하지 않을 경우에는 Default Value로 사용된다.
 * <BR> - PERMANENT File은 삭제하면 안되며, 복구 가능해야 한다. (Permanent File 제거 2014/05/08)
 * <BR> - ex) DataFile 또는 Control File의 한 부분으로 저장하며, Utility를 이용해서 ReadOnly Property File을 만들수 있도록 한다.
 * <BR>4. Varchar DataType을 갖는 Property
 * <BR> - ALTER SESSION 가능/불가능을 설정가능하다.
 * <BR> - ALTER SYSTEM은 DEFERRED, FALSE(File에 반영)로만 가능하다. IMMEDIATE는 설정 불가능하다.
 * <BR>5. 숫자형의 Property는 Range Check 속성을 사용할 경우에 MIN/MAX 값을 설정해 줘야 한다.
 * <BR> - 숫자형 Property가 UNIT(단위)를 갖을 경우에는 TIME/SIZE 지정할 수 있다.
 * <BR>6. Property의 접근은 Property 이름/ Property ID로 가능하다.
 * <BR>7. 기본적으로 추가해야하는 시스템 환경 변수는 다음과 같다. (PREFIX는 GOLDILOCKS_)
 * <BR>  - GOLDILOCKS_DATA
 * <BR>8. Exception
 * <BR> - Text PropertyFile 에서는 연산은 지원되지 않는다. ex) CLIENT_MAX_SIZE = 1024 * 5 (X)
 * <BR>
 * <BR>Property 등록 방법
 * <BR>1. knpPropertyList.c의 gPropertyContentList에 추가하고자 하는 Property를 입력한다. 
 * <BR>2. 입력하는 데이터중 PropertyID는 knlDef.h의 knlPropertyID에 추가한 뒤에 입력한다.
 * <BR>3. Property가 갖을 수 있는 DataType은 Boolean/Bigint/Varchar 이다.
 * <BR>4. ALTER SYSTEM으로 Property 변경시 영향의 범위를 나타내는 knlPropertyModifyType 를 설정해 줘야 한다.
 * <BR>  - KNL_PROPERTY_SYS_TYPE_IMMEDIATE : 현재 접속되어 있는 세션 및 이후 접속되는 세션에 영향을 줌
 * <BR>  - KNL_PROPERTY_SYS_TYPE_DEFERRED : 이후 접소되는 세션부터 변경된 프로퍼티의 영향을 줌
 * <BR>  - KNL_PROPERTY_SYS_TYPE_FALSE : 운용 중에는 변경할 수 없으며, ALTER SYSTEM .. SCOPE=File/Both로만 변경가능하며, RESTART이후에 적용된다.
 * <BR>  - KNL_PROPERTY_SYS_TYPE_NONE : ALTER SYSTEM 불가능 (READ ONLY일 경우 사용됨)
 * <BR>5. 변경 가능한 Startup Phase를 설정해 줘야 한다.
 * <BR> - Startup Phase Domain을 설정하여 이하/이상등의 범위를 설정할 수 있다.
 * <BR>  
 * <BR>Property 변경 방법
 * <BR>1. knlUpdatePropertyValueByID, knlUpdatePropertyValueByName 로 가능하며, 아래의 값을 지정해 줘야 한다.
 * <BR> knlPropertyModifyMode 
 * <BR>  - ALTER SESSION인 경우, KNL_PROPERTY_MODE_SES (이 경우에는 SCOPE를 사용할 수 없다.)
 * <BR>  - ALTER SYSTEM인 경우, KNL_PROPERTY_MODE_SYS
 * <BR> knlPropertyModifyScope : ALTER SYSTEM ...... SCOPE=XXX
 * <BR>  - KNL_PROPERTY_SCOPE_MEMORY : 해당 프로퍼티의 값이 MEMORY에만 반영되며, RESTART이후에는 적용되지 않음
 * <BR>  - KNL_PROPERTY_SCOPE_FILE : 해당 프로퍼티의 변경이 Binary 프로퍼티 파일에만 반영되며, Restart 이후부터 적용
 * <BR>  - KNL_PROPERTY_SCOPE_BOTH : MEMORY/FILE 모두 변경. 현재 및 Restart 이후에도 적용됨.
 * <BR>
 */


/**
 * @brief Property를 정보를 Initialize하고 Default값을 넣는다.
 * @param[in,out] aKnlEnv      Kernel Environment
 * @remarks
 * <BR> Property를 등록하고, 각 소스에서 Property의 값을 읽어온다.
 * <BR> 해당 정보는 No-Mount 단계의 Shared Memory를 구성하기 위한 정보를 포함하기 때문에
 * <BR> Heap에 구성된다. Shared Memory가 구성된 이후에는 해당 정보를 Shared Memory로 옮긴다.
 */

stlStatus knpInitializePropertyInHeap( knlEnv  * aKnlEnv )
{
    stlUInt32 sState = 0;
    
    /**
     * Property 를 등록하기 위한 환경 세팅
     */
    STL_TRY( knpInitializeEnvironment( aKnlEnv ) == STL_SUCCESS );
    
    sState = 1;
    
    /**
     * Default 값으로 Property를 구성한다.
     */
    STL_TRY( knpBuildPropertyWithDefault( aKnlEnv ) == STL_SUCCESS );

    /**
     * 시스템 환경 변수에서 Property를 읽는다.
     */
    STL_TRY( knpSetPropertyFromEnv( aKnlEnv ) == STL_SUCCESS );

    /**
     * Property File에서 Property를 읽는다.
     * @todo 첫번째 인자는 Args로 들어온 값을 받을 수 있도록 해야 함
     */
    STL_TRY( knpSetPropertyFromFile( NULL, STL_FALSE, aKnlEnv ) == STL_SUCCESS );

    /**
     * Property의 Init Value를 등록한다. 
     * Init Value는 Default -> Env_Var -> Property File 을 읽고 난 뒤, 마지막에 처리한다.
     */
    STL_TRY ( knpSetPropertyInitValue( aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void)knpFinalizePropertyInHeap(aKnlEnv);
        default:
            break;
    }    

    return STL_FAILURE;
}


/**
 * @brief Warm-up 단계에 사용될 초기화
 * @param[in,out] aKnlEnv      Kernel Environment
 * @remarks
 * <BR> Shared Memory의 Attach한 Process 들이 Property Manager를 사용할 수 있도록 
 * <BR> Warm-up 단계에서 시작 주소를 Assign 한다.
 */

stlStatus knpInitializePropertyAtWarmup ( knlEnv * aKnlEnv )
{
    gKnpPropertyMgr = (knpPropertyMgr*)gKnEntryPoint->mPropertyMem;
    
    return STL_SUCCESS;
}


/**
 * @brief Heap 영역에 구성된 Property 정보를 Shared Memory로 옮긴다.
 * @param[in,out] aKnlEnv      Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_NOT_EXIST]
 * 입력한 Property 이름과 동일한 Property가 없다.
 * - Property 이름을 확인한다.
 * @endcode
 * @todo 잘못된 Memory 연산을 방지하기 위한 루틴이 들어가야 함!
 * @remarks
 * <BR> Heap에 구성된 정보를 Shared Memory가 구성된 이후에 Shared Memory로 옮긴다.
 * <BR> 다음의 과정을 수행한다.
 * <BR> 1. Size 알아내기
 * <BR> 2. Property를 위한 Fixed Static Area Allocation.
 * <BR> 3. PropertyMgr Copy
 * <BR> 4. PropertyMgr의 Hash 구성
 * <BR> 5. Property의 Latch 구성
 * <BR> 6. Property 구조체 Copy.
 * <BR> 7. 기타 정보 Copy
 * <BR> 8. Hash Insert..
 * <BR> 9. 5,6,7,8 반복
 * <BR> 10. Heap 정보 Finalize.
 * <BR>
 * <BR> Shared Memory 구조
 * <BR> +-------------------++-------------------+----------+-------------+-----------++
 * <BR> | Property Manager  || Property1 구조체  |  Value    |  resetValue | FileValue ||
 * <BR> |-------------------++------+-----------+-----------++------------+-----------++
 * <BR> |  Property2 구조체 | Value | resetValue | fileValue ||   ......                |
 * <BR> |-------------------+-------+-----------+-----------++-------------------------+
 * <BR> | ...                                                                          |
 * <BR> | ...                                                                          |
 * <BR> +------------------------------------------------------------------------------+
 * <BR>
 */

stlStatus knpInitializePropertyInShm( knlEnv  * aKnlEnv )
{
    stlUInt64       sPropertyOffset      = 0;                       /** Shared Memory안의 Property 구분을 위한 Offset */
    stlUInt32       sBucketSeq           = 0;                       /** Hash Scan에 사용 */
    stlUInt64       sOnePropertySize     = 0;                       /** 해당 Property의 크기를 위함 */
    stlUInt32       sValueSize           = 0;                       /** Property Type별 Allocation Size를 위함 */ 
    
    stlUInt64       sPropertySize        = gKnpPropertyMgr->mPropertySize;
    stlUInt32       sPropertyCount       = gKnpPropertyMgr->mPropertyCnt;
    stlUInt32       sPropertyMgrSize     = STL_SIZEOF(knpPropertyMgr);
    stlUInt32       sPropertyContentSize = STL_SIZEOF(knpPropertyContent);
    
    knpPropertyContent   * sRetContent   = NULL;
    knpPropertyContent   * sAddContent   = NULL;
    knpPropertyMgr       * sMgr          = NULL;
    
    /**
     * Property를 위한 Fixed Static Area Allocation.
     * Size = 전체 Property Size + Property Manager Size
     */
    STL_TRY( knlCacheAlignedAllocFixedStaticArea( sPropertySize + sPropertyMgrSize,
                                                  &(gKnEntryPoint->mPropertyMem),
                                                  aKnlEnv) == STL_SUCCESS );
    
    /** Property Manager 정보 세팅 */
    sMgr = (knpPropertyMgr*)gKnEntryPoint->mPropertyMem;

    stlStrncpy( sMgr->mHomeDir,
                gKnpPropertyMgr->mHomeDir,
                stlStrlen( gKnpPropertyMgr->mHomeDir ) + 1 );
    
    stlStrncpy( sMgr->mSID,
                gKnpPropertyMgr->mSID,
                stlStrlen( gKnpPropertyMgr->mSID ) + 1 );
    
    stlStrncpy( sMgr->mPropertyFile,
                gKnpPropertyMgr->mPropertyFile,
                stlStrlen( gKnpPropertyMgr->mPropertyFile ) + 1 );
    
    stlMemcpy( sMgr->mDoCopyToSessionEnv,
               gKnpPropertyMgr->mDoCopyToSessionEnv,
               ( KNP_PROPERTY_MAX_SIZE * STL_SIZEOF(stlBool) ) );
              
    sMgr->mPropertyCnt     = gKnpPropertyMgr->mPropertyCnt;
    sMgr->mVarcharTotalLen = gKnpPropertyMgr->mVarcharTotalLen;
    
    /**
     * Shared Memory에 StaticHash 생성 및 Hash Pointer 얻기
     */
    STL_TRY( knlCreateStaticHash( &(sMgr->mPropertyHash),
                                  sPropertyCount,
                                  STL_OFFSETOF(knpPropertyContent, mLink),
                                  STL_OFFSETOF(knpPropertyContent, mName),
                                  KNL_STATICHASH_MODE_NOLATCH,
                                  STL_TRUE,   /** SHARED MEMORY */
                                  NULL,
                                  aKnlEnv ) == STL_SUCCESS );
    
    /**
     * Heap 영역에 저장된 Property를 Scan해서 Shared Memory에 저장한다.
     * Get First HashItem
     */
    STL_TRY( knlGetFirstStaticHashItem( gKnpPropertyMgr->mPropertyHash, 
                                        &sBucketSeq, 
                                        (void**)&sRetContent, 
                                        aKnlEnv ) == STL_SUCCESS );
    
    /**
     *  Copy to Shared Memory From Heap Memory.
     */
    sPropertyOffset = 0;
            
    while( sRetContent != NULL )
    {
        switch ( sRetContent->mDataType )
        {
            case KNL_PROPERTY_TYPE_BOOL:
            case KNL_PROPERTY_TYPE_BIGINT:
            {
                sValueSize = STL_SIZEOF(stlInt64);
                break;
            }    
            case KNL_PROPERTY_TYPE_VARCHAR:
            {
                sValueSize = sRetContent->mMax;
                break;
            }
            case KNL_PROPERTY_TYPE_MAX:
            default:
            {   
                STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
                break;
            }
        }       
        
        /**
         * Shared Memory 안에서의 해당 Property의 시작 주소
         * mPropertyMem + PropertyManager Size + 축적된 PropertyOffset
         * sPropertyOffset은 전단계에서 사용되었던 Shared Memory의 끝을 가리킨다.
         */
        sAddContent = (void*)gKnEntryPoint->mPropertyMem + 
                      sPropertyMgrSize +
                      sPropertyOffset;
        
        /**
         * 각 Property의 시작 주소를 저장한다.
         * 이는 PropertyID를 사용해 Direct Access 하도록 한다.
         */
        sMgr->mPropertyAddr[sRetContent->mID] = sAddContent;
        
        /**
         * 각각의 PropertyContent의 데이터를 복사한다.
         */
        sAddContent->mFlag                = sRetContent->mFlag;
        sAddContent->mID                  = sRetContent->mID;
        sAddContent->mGroup               = sRetContent->mGroup;
        sAddContent->mDataType            = sRetContent->mDataType;
        sAddContent->mStartupPhase        = sRetContent->mStartupPhase;
        sAddContent->mStartupPhaseDomain  = sRetContent->mStartupPhaseDomain;
        sAddContent->mMin                 = sRetContent->mMin;
        sAddContent->mMax                 = sRetContent->mMax;
        sAddContent->mSource              = sRetContent->mSource;
        sAddContent->mInitSource          = sRetContent->mInitSource;
        sAddContent->mHasFileValue        = sRetContent->mHasFileValue;
        sAddContent->mFileValueLength     = sRetContent->mFileValueLength;
        
        stlStrncpy( sAddContent->mName,
                    sRetContent->mName,
                    stlStrlen( sRetContent->mName ) + 1 );
        
        stlStrncpy( sAddContent->mDesc,
                    sRetContent->mDesc,
                    stlStrlen( sRetContent->mDesc ) + 1);
        
        /**
         * PropertySize = sizeof(knpPropertyContent) ( sPropertyContentSize )
         *                + Value Size
         *                + InitValue Size
         *                + FileValue Size
         */
        sOnePropertySize = ( sPropertyContentSize ) + 
                           ( sValueSize * 3 ) ;
        
        /**
         * 여기에서 sPropertyOffset은 다음 단계에서 사용되는 Shared Memory의 시작을 가리킨다.
         */
        sPropertyOffset += sOnePropertySize;
        
        /**
         * Offset이 할당한 Memory를 넘어서는 안된다.
         * 반드시 할당된 Memory Size와 같거나 작아야 한다.
         * 그러지 않을 경우에는 메모리를 긁을 수 있다.
         */
        STL_TRY( sPropertyOffset <= sPropertySize );
        
        /** 
         * Value Address를 세팅한다.
         * 각각의 PropertyContent의 mValue, mInitValue, mFileValue의 Pointer는 다음과 같다.
         * +--------------------------------------------------------------------------------------------+
         * |   Property  *Value, *InitValue *FileValue |   mValue  |      mInitValue .. |   mFileValue  |
         * +---------------+-----------+----------+----------^--------------------^-----+-------^-------+
         *                 |           |          |          |                    |             |
         *                 +-----------|----------|----------+                    |             |
         *                             +----------|-------------------------------+             |
         *                                        +---------------------------------------------+
         */
        sAddContent->mValue = (void*)sAddContent +       /* StartPoint */
                              sPropertyContentSize;      /* Structure Area */
           
        sAddContent->mInitValue = (void*)sAddContent +      /* StartPoint */
                                  sPropertyContentSize +    /* Structure Area */
                                  sValueSize;               /* Value Offset */
        
        sAddContent->mFileValue = (void*)sAddContent +      /* StartPoint */
                                  sPropertyContentSize +    /* Structure Area */
                                  ( sValueSize * 2 );       /* Value Offset */
        
        /**
         * mFileValue = 0;
         */
        stlMemset( sAddContent->mValue, 0x00, sValueSize );
        stlMemset( sAddContent->mInitValue, 0x00, sValueSize );
        stlMemset( sAddContent->mFileValue, 0x00, sValueSize );
        
        
        /** 
         * Property DataType에 따라서 Value, InitValue 값을 추가 
         */
        stlMemcpy( sAddContent->mValue, sRetContent->mValue, sRetContent->mValueLength );
        sAddContent->mValueLength = sRetContent->mValueLength;
        
        STL_TRY( knpAddPropertyInitValue( sAddContent,
                                          sRetContent->mValue,
                                          aKnlEnv ) == STL_SUCCESS );
        
        /**
         * Shared Memory Hash item Insert
         */ 
        STL_TRY( knlInsertStaticHash( sMgr->mPropertyHash,
                                      knpHashFunction,
                                      sAddContent,
                                      aKnlEnv ) == STL_SUCCESS );
        
        /**
         * Get Next HashItem
         */
        STL_TRY( knlGetNextStaticHashItem( gKnpPropertyMgr->mPropertyHash, 
                                           &sBucketSeq, 
                                           (void**)&sRetContent, 
                                           aKnlEnv ) == STL_SUCCESS );
    }
    
    /**
     * Region Heap 영역 제거
     */    
    STL_TRY( knpFinalizePropertyInHeap( aKnlEnv ) == STL_SUCCESS );
    
    /**
     * Property List를 Shared Memory에서 가지고 오도록 지정
     */
    gKnpPropertyMgr = (knpPropertyMgr*)gKnEntryPoint->mPropertyMem;
    
    STL_TRY( knlInitLatch( &(gKnpPropertyMgr->mLatch),
                           STL_TRUE,               //SHM
                           aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Manager 초기화 및 No-Mount단계에서 Shared-Memory가 구성되기전에 임시로 사용되는 Heap Memory와 StaticHash를 구성한다.
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_HOME_DIR]
 * Home Directory가 정확하지 않습니다.
 * - 시스템 환경 변수에서 Home Directory를 설정해야 합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_SID]
 * SID가 정확하지 않습니다.
 * - 시스템 환경 변수에서 SID를 설정해야 합니다.
 * @endcode
 * @remarks
 * <BR> 해당 함수를 호출하기 전에 시스템 환경변수에 
 * <BR> Home Directory, SID가 설정되어 있어야 한다.
 */

stlStatus knpInitializeEnvironment( knlEnv  * aKnlEnv )
{
    stlUInt32          sBucketSize    = 0;
    stlUInt32          sHomeDirLength = 0;
    stlBool            sFound         = STL_FALSE;
    
    stlChar            sHomeDir[KNL_PROPERTY_STRING_MAX_LENGTH] = {0};
    stlChar            sSID[KNL_PROPERTY_STRING_MAX_LENGTH]     = {0};
    stlChar            sValue[KNL_PROPERTY_STRING_MAX_LENGTH]   = {0};

    stlInt32           sState = 0;
    
    /**
     * 시스템 환경변수에서 HOME DIRECTION를 읽어온다.
     */    
    stlSnprintf( sValue, 
                 STL_SIZEOF(sValue), 
                 "%s",
                 STL_ENV_DB_DATA );
    
    STL_TRY( stlGetEnv( sHomeDir, 
                        STL_SIZEOF(sHomeDir),
                        sValue,
                        &sFound,
                        KNL_ERROR_STACK(aKnlEnv) ) == STL_SUCCESS );

    STL_TRY_THROW( sFound == STL_TRUE , RAMP_ERR_INVALID_HOMEDIR );
    
    STL_TRY_THROW( stlStrlen(sHomeDir) > 0 , RAMP_ERR_INVALID_HOMEDIR );

    stlMemset( sValue, 0x00, STL_SIZEOF( sValue ) );
    
    /**
     * Property Manager의 임시 Property Manager 관련 Heap Memory 구성
     */
    STL_TRY( stlAllocHeap( (void**)&gKnpPropertyMgr,
                           STL_SIZEOF(knpPropertyMgr),
                           KNL_ERROR_STACK(aKnlEnv) ) == STL_SUCCESS );
    sState = 1;
    
    stlMemset( gKnpPropertyMgr, 0x00, STL_SIZEOF(knpPropertyMgr));
    
    /**
     * Property Manager 기본값으로 세팅
     */ 
        
    /**
     * Home Directory가 '/', '\'로 끝날 경우에 제거한다.
     */
    if ( sHomeDir[stlStrlen(sHomeDir)-1] == '/' ||
         sHomeDir[stlStrlen(sHomeDir)-1] == '\\' )
    {
        sHomeDirLength = stlStrlen(sHomeDir)-1;
    }
    else
    {
        sHomeDirLength = stlStrlen(sHomeDir);
    }
    
    stlStrncpy( gKnpPropertyMgr->mHomeDir, 
                sHomeDir, 
                sHomeDirLength + 1 );
    
    stlStrncpy( gKnpPropertyMgr->mSID, 
                sSID, 
                stlStrlen(sSID) + 1 );
    
    stlStrncpy( gKnpPropertyMgr->mPropertyFile, 
                KNP_DEFAULT_PROPERTY_FILE, 
                stlStrlen(KNP_DEFAULT_PROPERTY_FILE) + 1 );
    
    gKnpPropertyMgr->mPropertyCnt     = 0;
    gKnpPropertyMgr->mPropertySize    = 0;
    
    
    /**
     * Heap Memory Create
     * No-Mount 단계에서 Shared Memory가 구성되기전 임시로 사용하는 Region Heap 메모리 생성
     */
    STL_TRY( knlCreateRegionMem( &(gKnpPropertyMgr->mHeapMem),
                                 KNL_ALLOCATOR_KERNEL_PROPERTY_MANAGER,
                                 NULL,  /* aParentMem */
                                 KNL_MEM_STORAGE_TYPE_HEAP,
                                 KNL_DEFAULT_LARGE_REGION_SIZE,
                                 KNL_DEFAULT_LARGE_REGION_SIZE,
                                 aKnlEnv ) == STL_SUCCESS );
    sState = 2;
    
    /**
     * Static Hash 생성
     * @todo Bucket Size 조정해야함
     */
    sBucketSize = KNL_PROPERTY_MAX_COUNT / 30;
    
    STL_TRY( knlCreateStaticHash( &(gKnpPropertyMgr->mPropertyHash),
                                  sBucketSize,
                                  STL_OFFSETOF(knpPropertyContent, mLink),
                                  STL_OFFSETOF(knpPropertyContent, mName),
                                  KNL_STATICHASH_MODE_NOLATCH,
                                  STL_FALSE,        // Heap 
                                  &(gKnpPropertyMgr->mHeapMem),
                                  aKnlEnv ) == STL_SUCCESS );
    
    STL_TRY( knlInitLatch( &(gKnpPropertyMgr->mLatch),
                           STL_FALSE,               //HEAP
                           aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;    

    STL_CATCH( RAMP_ERR_INVALID_HOMEDIR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_HOME_DIR,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    /*
    STL_CATCH( RAMP_ERR_INVALID_SID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_SID,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    */
    STL_FINISH;

    switch( sState )
    {
        case 2 :
            (void)knlDestroyRegionMem( &(gKnpPropertyMgr->mHeapMem),
                                       aKnlEnv );
        case 1 :
            stlFreeHeap( gKnpPropertyMgr );
            gKnpPropertyMgr = NULL;
        default :
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief Heap 메모리에 임시적으로 사용한 Property 관련 메모리를 Finalize 한다.
 * @param[in,out] aKnlEnv              Kernel Environment
 * @remarks
 */

stlStatus knpFinalizePropertyInHeap( knlEnv  * aKnlEnv )
{
    /**
     * Region Memory Destroy
     */
    STL_TRY( knlDestroyRegionMem( &(gKnpPropertyMgr->mHeapMem),
                                  aKnlEnv ) == STL_SUCCESS );
    
    knlFinLatch( &gKnpPropertyMgr->mLatch );
    
    /**
     * 임시로 Heap에 할당된 Property Manager의 Memory를 반환한다.
     */
    (void)stlFreeHeap( gKnpPropertyMgr );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 시스템 환경변수에서 Property Value를 얻어온다.
 * @param[in,out] aKnlEnv              Kernel Environment
 */

stlStatus knpSetPropertyFromEnv( knlEnv  * aKnlEnv )
{
    knpPropertyContent   * sContent        = NULL;
    stlUInt32              sBucketSeq      = 0;
    stlBool                sFound          = STL_FALSE;
    stlUInt32              sNameLen        = KNL_PROPERTY_NAME_MAX_LENGTH + stlStrlen(KNP_PROPERTY_PREFIX);
    stlBool                sModifiable     = STL_FALSE;
    knlPropertyDataType    sType           = KNL_PROPERTY_TYPE_MAX;
    
    stlChar                sName[ sNameLen ];
    stlChar                sValue[ KNL_PROPERTY_STRING_MAX_LENGTH ];
    stlInt64               sUnitValue  = 0;
    
    /**
     * Get First HashItem
     */
    STL_TRY( knlGetFirstStaticHashItem( gKnpPropertyMgr->mPropertyHash, 
                                        &sBucketSeq, 
                                        (void**)&sContent, 
                                        aKnlEnv ) == STL_SUCCESS );

    while( sContent != NULL )
    {
        /**
         * READ ONLY 속성을 갖는 Property인 경우에는 시스템 환경변수에서 읽지 않는다.
         */
        STL_TRY( knpGetPropertyModifiable( sContent, &sModifiable, aKnlEnv ) == STL_SUCCESS );
         
        if ( sModifiable == STL_FALSE )
        {
             sFound = STL_FALSE;
             /**
              * Get Next HashItem
              */
             STL_TRY( knlGetNextStaticHashItem( gKnpPropertyMgr->mPropertyHash, 
                                                &sBucketSeq, 
                                                (void**)&sContent, 
                                                aKnlEnv ) == STL_SUCCESS );
             continue;  
        }
                
                
        stlMemset( sName,  0x00, STL_SIZEOF( sName ) );
        stlMemset( sValue, 0x00, STL_SIZEOF( sValue ) );
        
        stlSnprintf( sName, 
                     STL_SIZEOF( sName ), 
                     "%s%s", 
                     KNP_PROPERTY_PREFIX, 
                     sContent->mName );
        
        /**
         * 시스템 환경변수에서 Property Name과 동일한 값을 읽어낸다.
         */
        STL_TRY( stlGetEnv( sValue, 
                            STL_SIZEOF( sValue ),
                            sName,
                            &sFound,
                            KNL_ERROR_STACK(aKnlEnv) ) == STL_SUCCESS );
        
        if( sFound == STL_TRUE )
        {
            /**
             * Data Convert..
             */ 
            STL_TRY( knpGetPropertyDataType( sContent,
                                             &sType,
                                             aKnlEnv ) == STL_SUCCESS );
            
            if ( sType == KNL_PROPERTY_TYPE_BIGINT )
            {
                STL_TRY( knpConvertUnitToValue( sContent,
                                                sValue,
                                                &sUnitValue,
                                                aKnlEnv ) == STL_SUCCESS );
                    
                stlMemset( sValue, 0x00, STL_SIZEOF( sValue ) );
            
                stlSnprintf( sValue, STL_SIZEOF( sValue ),
                             "%ld", sUnitValue );
            }
            
            STL_TRY( knpUpdatePropertyValueUseStr( sContent,
                                                   sValue,
                                                   KNL_PROPERTY_MODE_SYS,
                                                   STL_FALSE,
                                                   KNL_PROPERTY_SCOPE_MEMORY,
                                                   aKnlEnv ) == STL_SUCCESS );
            sContent->mSource = KNP_SOURCE_ENV;
        }
        
        sFound = STL_FALSE;
        
        /**
         * Get Next HashItem
         */
        STL_TRY( knlGetNextStaticHashItem( gKnpPropertyMgr->mPropertyHash, 
                                           &sBucketSeq, 
                                           (void**)&sContent, 
                                           aKnlEnv ) == STL_SUCCESS );
    }    
    
    return STL_SUCCESS;    
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 지정된 Property File에서 Property Value를 얻어온다.
 * @param[in]     aFileName   PropertyFileName
 * @param[in]     aIsBinary   해당 파일이 Binary File인지 여부
 * @param[in,out] aKnlEnv     Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_FILE_NOT_EXIST]
 * 파일이 존재하지 않습니다.
 * - 파일 경로 및 파일 이름을 확인합니다.
 * @endcode
 * @remarks
 */

stlStatus knpSetPropertyFromFile( stlChar  * aFileName,
                                  stlBool    aIsBinary,
                                  knlEnv   * aKnlEnv )
{
    stlChar   sFileName[KNL_PROPERTY_STRING_MAX_LENGTH]   = {0};
    stlBool   sFileExist = STL_FALSE;
    stlBool   sIsBinary  = STL_FALSE;       /** aIsBinary와 다른 의미로 사용되므로 제거하면 안된다. */
    
    /**
     * args로 FileName을 받을 수 있음..
     * 1.ARgs로 넘어올 경우에는 FileName과 aIsBinary를 본다.
     * 2. FileName이 NULL일 경우에는 Default Binary가 있는지를 보고 있으면 Binary를 읽는다.
     */
    
    if ( aFileName != NULL )
    {
        /**
         * ARGV로 FILE을 지정한 경우이며, Binary인지 Text인지를 구분해줘야 한다. (사용자에게 받아야 한다.)
         */
        stlSnprintf( sFileName, STL_SIZEOF(sFileName), 
                     "%s", aFileName );
        
        STL_TRY( knpGetFileExist( sFileName, 
                                  &sFileExist,
                                  aKnlEnv ) == STL_SUCCESS );
        
        STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
        
        /** FILE이 존재하다면, aIsBinary를 사용한다. */
        sIsBinary = aIsBinary;
    }
    else
    {
        /**
         * 1. Binary File이 있는지 확인
         * 2. Binary File이 있을 경우에는 Binary File을 읽는다.
         * 3. Binary File이 없을 경우에 Text File을 읽는다.
         * -- 반드시 둘중에 하나만 읽으며 우선 순위는 Binary File -> Text File 순이다.
         */
            
        /** Binary File */
        stlSnprintf( sFileName, STL_SIZEOF(sFileName), 
                     "%s"STL_PATH_SEPARATOR"%s", gKnpPropertyMgr->mHomeDir, KNP_DEFAULT_BINARY_PROPERTY_FILE );
        
        STL_TRY( knpGetFileExist( sFileName, 
                                  &sFileExist,
                                  aKnlEnv ) == STL_SUCCESS );
        
        if( sFileExist == STL_FALSE )
        {
            /**
             * Default BinaryFile이 없을 경우에 TextFile이 있는지 확인한다.
             */
            stlSnprintf( sFileName, STL_SIZEOF(sFileName), 
                         "%s"STL_PATH_SEPARATOR"%s", gKnpPropertyMgr->mHomeDir, KNP_DEFAULT_PROPERTY_FILE );    
            
            STL_TRY( knpGetFileExist( sFileName, 
                                      &sFileExist,
                                      aKnlEnv ) == STL_SUCCESS );    
            
            STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
            
            /** TextFile에서 읽어옴 */
            sIsBinary = STL_FALSE;
        }
        else
        {
            /** BinaryFile에서 읽어옴 */
            sIsBinary = STL_TRUE;
        }
    }
    

    if ( sIsBinary == STL_TRUE )
    {
        /** Process Binary Property File */
        STL_TRY( knpProcessBinaryPropertyFile( sFileName,
                                               aKnlEnv ) == STL_SUCCESS );
    }
    else
    {
        /** Process Text Property File */
        STL_TRY( knpProcessTextPropertyFile( sFileName, 
                                             aKnlEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      sFileName );
    }
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Property의 Init Value를 설정한다.
 * @param[in,out] aKnlEnv     Kernel Environment
 */

stlStatus knpSetPropertyInitValue( knlEnv  * aKnlEnv )
{
    knpPropertyContent   * sContent   = NULL;
    stlUInt32              sBucketSeq = 0;
    
    /**
     * Get First HashItem
     */
    STL_TRY( knlGetFirstStaticHashItem( gKnpPropertyMgr->mPropertyHash, 
                                        &sBucketSeq, 
                                        (void**)&sContent, 
                                        aKnlEnv ) == STL_SUCCESS );
    
    while( sContent != NULL )
    {
        /**
         * Init시에 사용될 Value를 추가한다.
         */
        STL_TRY( knpAddPropertyInitValue( sContent,
                                          sContent->mValue,
                                          aKnlEnv ) == STL_SUCCESS );
        
        /**
         * Init Source Setting
         */
        sContent->mInitSource = sContent->mSource;
        
        /**
         * Get Next HashItem
         */
        STL_TRY( knlGetNextStaticHashItem( gKnpPropertyMgr->mPropertyHash, 
                                           &sBucketSeq, 
                                           (void**)&sContent, 
                                           aKnlEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Property를 정보를 등록한다.
 * @param[in]     aDesc        개발자가 정의한 knpPropertyDesc (knpPropertyList.c에서 등록한다.)
 * @param[in,out] aKnlEnv      Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_DATATYPE]
 * 해당 Property의 DataType이 정상적이지 않습니다.
 * - 해당 Property의 DataType을 확인합니다.
 * @endcode
 * @remarks
 * <BR> 프로퍼티 등록시 다음과 같은 제약을 갖는다.
 * <BR> 1. READ ONLY Property 
 * <BR> - Alter Session 으로 변경 불가능 해야한다.
 * <BR> - Alter System 으로 변경 불가능 해야한다.
 * <BR> 2. Writable Property
 * <BR> - Alter System은 반드시 가능해야 한다. (IMMEDIATE/DEFERRED/FALSE 중에 하나의 값을 가져야한다. NONE이 되면 안된다)
 * <BR> 3. VARCHAR DataType의 Property 
 * <BR> - Alter Session으로 변경 불가능 해야한다.
 * <BR> - Startup Phase는 반드시 Mount 이하여야 한다. (동시성 문제로 인한 제약)
 * <BR> - Alter System 변경 속성은 DEFERRED가 아니여아 한다. (DEFERED의 경우에는 각 Environment마다 값을 가지고 있어야 하는데, 이는 설계상 맞지 않다. - Enviroment는 한 Property당 무조건 8Byte 할당 사용한다.)
 * <BR>
 * <BR> . 위의 제약에 걸리 경우에는 ASSERT 처리한다.
 * <BR> 
 */

stlStatus knpRegistProperty( knpPropertyDesc   * aDesc,
                             knlEnv            * aKnlEnv )
{
    knpPropertyContent     * sContent         = NULL;
    stlBool                  sModifiable      = STL_FALSE;
    stlBool                  sPropertySesMod  = STL_FALSE;
    knlPropertyModifyType    sPropertySysMod  = KNL_PROPERTY_SYS_TYPE_MAX;
    stlUInt32                sAllocSize       = 0;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aDesc   != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /**
     * Initialize
     */
    STL_TRY( knlAllocRegionMem( &(gKnpPropertyMgr->mHeapMem),
                                STL_SIZEOF( knpPropertyContent ),
                                (void**)&sContent,
                                aKnlEnv ) == STL_SUCCESS );
    
    STL_RING_INIT_DATALINK( sContent,
                            STL_OFFSETOF( knpPropertyContent, mLink ) ); 
    
    stlStrncpy( sContent->mName, 
                aDesc->mName, 
                stlStrlen(aDesc->mName) + 1 );
    
    stlStrncpy( sContent->mDesc, 
                aDesc->mDesc, 
                stlStrlen(aDesc->mDesc) + 1 );
    
    sContent->mFlag           = aDesc->mFlag;
    sContent->mID             = aDesc->mID;
    sContent->mGroup          = aDesc->mGroup;
    sContent->mSource         = KNP_SOURCE_DEFAULT;
    sContent->mInitSource     = KNP_SOURCE_DEFAULT;
            
    sContent->mStartupPhase       = aDesc->mStartupPhase;
    sContent->mStartupPhaseDomain = aDesc->mStartupPhaseDomain;
    sContent->mMin                = 0;
    sContent->mMax                = 0;
    sContent->mValueLength        = 0;
    sContent->mInitValueLength    = 0;
    sContent->mValue              = NULL;
    sContent->mInitValue          = NULL;
    sContent->mFileValue          = NULL;      //mFileValue는 Shared Memory 에서만 사용되므로 메모리를 할당하지 않는다. 
    sContent->mHasFileValue       = STL_FALSE;
    sContent->mFileValueLength    = 0;
    
    STL_TRY( knpGetPropertySysModType( sContent, 
                                       &sPropertySysMod, 
                                       aKnlEnv ) == STL_SUCCESS );
    
    STL_TRY( knpGetPropertySesModType( sContent,
                                       &sPropertySesMod,
                                       aKnlEnv ) == STL_SUCCESS );
    
    STL_TRY( knpGetPropertyModifiable( sContent,
                                       &sModifiable,
                                       aKnlEnv ) == STL_SUCCESS );
    
    /**
     * Property Attribute Check
     * 1. Read Only
     * - Alter Session 불가능/ Alter System 불가능(NONE)해야 한다.
     * 2. Writable
     * - Alter System은 반드시 가능해야 한다 (NONE이 되면 안된다)
     */
    if( sModifiable == STL_FALSE )
    {
        STL_DASSERT( sPropertySesMod == STL_FALSE );
        STL_DASSERT( sPropertySysMod == KNL_PROPERTY_SYS_TYPE_NONE );
    }
    else
    {
        STL_DASSERT( sPropertySysMod != KNL_PROPERTY_SYS_TYPE_NONE );
    }
    
    /**
     * Property Attribute Check
     * 3. VARCHAR DataType
     * - ALTER System 속성이 IMMEDIATE가 아니여야 한다.
     */
    if( aDesc->mDataType == KNL_PROPERTY_TYPE_VARCHAR )
    {
        STL_DASSERT( sPropertySysMod != KNL_PROPERTY_SYS_TYPE_IMMEDIATE );
    }

    /**
     * Property DataType별로 allocation Size 세팅
     */
    switch ( aDesc->mDataType )
    {
        case KNL_PROPERTY_TYPE_VARCHAR:
        {
            STL_TRY_THROW( STL_ALIGN_DEFAULT( aDesc->mMax ) <= KNL_PROPERTY_STRING_MAX_LENGTH, RAMP_ERR_TOO_LONG_STRING_TYPE );
            
            sContent->mDataType = KNL_PROPERTY_TYPE_VARCHAR;
            sAllocSize          = STL_ALIGN_DEFAULT( aDesc->mMax );
            
            sContent->mMin      = aDesc->mMin;
            sContent->mMax      = STL_ALIGN_DEFAULT( aDesc->mMax );
            
            /**
             * Session에서 Allocate하기 위한 SIZE를 구해놓는다.
             * Property 속성이 KNP_ATTR_MD_SES_TRUE 또는 KNL_PROPERTY_SYS_TYPE_DEFERRED인 경우에 Session ENV에 복사하도록 한다.
             */
            if( sPropertySesMod == STL_TRUE || 
                sPropertySysMod == KNL_PROPERTY_SYS_TYPE_DEFERRED )
            {
                gKnpPropertyMgr->mVarcharTotalLen += sAllocSize;    
            }
            break;
        }
        case KNL_PROPERTY_TYPE_BOOL:
        {
            sContent->mDataType = KNL_PROPERTY_TYPE_BOOL;
            sAllocSize          = STL_SIZEOF(stlInt64);        /** 8 Byte align */
            
            /**
             * Boolean 형은 Min:0, Max:1로 한다.
             */
            sContent->mMin      = 0;
            sContent->mMax      = 1;
            break;
        }
        case KNL_PROPERTY_TYPE_BIGINT:
        {
            sContent->mDataType = KNL_PROPERTY_TYPE_BIGINT;
            sAllocSize          = STL_SIZEOF(stlInt64);
            sContent->mMin      = aDesc->mMin;
            sContent->mMax      = aDesc->mMax;
            break;
        }    
        case KNL_PROPERTY_TYPE_MAX:
        default:
        {   
            STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
            break;
        }
    }

    /** 
     * PropertyManager의 PropertySize 크기 추가
     * - 해당 값은 후에 Shared Memory 공간 할당시 필요하다.
     */
    gKnpPropertyMgr->mPropertySize += STL_SIZEOF( knpPropertyContent );    /* 구조체 크기 */

    gKnpPropertyMgr->mPropertySize += sAllocSize * 3;                      /* 한개의 Value, InitValue, FileValue 크기 */
    
    /**
     * Value의 메모리를 할당한다.
     */
    STL_TRY( knlAllocRegionMem( &(gKnpPropertyMgr->mHeapMem),
                                sAllocSize,
                                (void**)&sContent->mValue,
                                aKnlEnv ) == STL_SUCCESS );
        
    stlMemset( sContent->mValue, 0x00, sAllocSize );
    
    /**
     * Init Value의 메모리를 할당한다.
     */
    STL_TRY( knlAllocRegionMem( &(gKnpPropertyMgr->mHeapMem),
                                sAllocSize,
                                (void**)&sContent->mInitValue,
                                aKnlEnv ) == STL_SUCCESS );
        
    stlMemset( sContent->mInitValue, 0x00, sAllocSize );
    
    /**
     * Default Value 추가
     */   
    STL_TRY( knpUpdatePropertyValueUseStr( sContent,
                                           aDesc->mValue,
                                           KNL_PROPERTY_MODE_NONE,
                                           STL_FALSE,
                                           KNL_PROPERTY_SCOPE_MEMORY,
                                           aKnlEnv ) == STL_SUCCESS );
    
    /**
     * Hash Insert
     */ 
    STL_TRY( knlInsertStaticHash( gKnpPropertyMgr->mPropertyHash,
                                  knpHashFunction,
                                  sContent,
                                  aKnlEnv ) == STL_SUCCESS );

    /**
     * Property Manager 정보 세팅
     * Session Environment 에 저장할지 여부를 결정한다.
     * Varchar Type이 아닐 경우에 모두 Session Environment에 복사한다.
     */
    gKnpPropertyMgr->mDoCopyToSessionEnv[sContent->mID] = STL_TRUE;
        
    /**
     * VARCHAR TYPE의 Property는 ALTER SESSION으로 변경 가능할 경우, 
     * Alter SYSTEM Defered 일 경우SessionEnv에 복사 가능하다.
     */
    if ( sContent->mDataType == KNL_PROPERTY_TYPE_VARCHAR )
    {
        if( sPropertySesMod == STL_TRUE || 
            sPropertySysMod == KNL_PROPERTY_SYS_TYPE_DEFERRED )
        {
            gKnpPropertyMgr->mDoCopyToSessionEnv[sContent->mID] = STL_TRUE;
        }
        else
        {
            gKnpPropertyMgr->mDoCopyToSessionEnv[sContent->mID] = STL_FALSE;
        }
    }

    gKnpPropertyMgr->mPropertyAddr[sContent->mID] = sContent;
    gKnpPropertyMgr->mPropertyCnt++;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERR_TOO_LONG_STRING_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_VALUE, 
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aDesc->mName );
    }
    STL_FINISH;

    return STL_FAILURE;
    
}


/**
 * @brief 개발자가 입력한 Key를 위한 Hash Function.
 * @param[in] aKey              HashKey
 * @param[in] aBucketCount      Bucket Count
 * @remarks
 */

stlUInt32 knpHashFunction( void * aKey, stlUInt32 aBucketCount )
{
    /**
     * @todo 더 좋은 방법으로 수정할 필요가 있음..
     */
    stlUInt32 sLen    = stlStrlen((stlChar*)aKey);
    
    return sLen % aBucketCount;
}


/**
 * @brief HashCompare 함수
 * @param[in] aKeyA              HashKey
 * @param[in] aKeyB              HashKey
 * @remarks
 */

stlInt32 knpHashCompareFunction( void * aKeyA, void * aKeyB )
{
    stlInt32 sRet = 0;
    
    //if ( stlStrcmp( (stlChar*)aKeyA, (stlChar*)aKeyB ) == 0 )
    if( stlStrncasecmp( (stlChar*)aKeyA, (stlChar*)aKeyB, stlStrlen( (stlChar*)aKeyA ) ) == 0 )
    {
        sRet = 0;
    }
    else
    {
        sRet = 1;
    }

    return sRet;
}

/**
 * @brief Property Name으로 Property Handle을 얻어낸다.
 * @param[in]     aPropertyName        PropertyName
 * @param[out]    aPropertyHandle      PropertyHandle
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_NOT_EXIST]
 * 입력한 Property 이름과 동일한 Property가 없다.
 * - Property 이름을 확인한다.
 * @endcode
 * @remarks
 */

stlStatus knpGetPropertyHandleByName( stlChar   * aPropertyName,
                                      void     ** aPropertyHandle, 
                                      knlEnv    * aKnlEnv )
{
    knpPropertyContent   * sContent = NULL;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( *aPropertyHandle == NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE(  aPropertyName   != NULL, KNL_ERROR_STACK(aKnlEnv) );

    *aPropertyHandle = sContent;
    
    /**
     * 프로퍼티 이름을 이용하여 프로퍼티 정보 획득 
     */
    STL_TRY( knlFindStaticHash( gKnpPropertyMgr->mPropertyHash,
                                knpHashFunction,
                                knpHashCompareFunction,
                                (void*)aPropertyName,
                                (void**)&sContent,
                                aKnlEnv ) == STL_SUCCESS );

    STL_TRY_THROW( sContent != NULL, RAMP_ERR_PROPERTY_NOT_EXIST );
    
    *aPropertyHandle = sContent;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_PROPERTY_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aPropertyName );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Property Name으로 Property가 존재하는지 검사한다.
 * @param[in]     aPropertyName        PropertyName
 * @param[out]    aIsExist             Property 존재 여부
 * @param[in,out] aKnlEnv              Kernel Environment
 */

stlStatus knpExistPropertyByName( stlChar   * aPropertyName,
                                  stlBool   * aIsExist,
                                  knlEnv    * aKnlEnv )
{
    knpPropertyContent   * sContent = NULL;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aPropertyName != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aIsExist      != NULL, KNL_ERROR_STACK(aKnlEnv) );

    *aIsExist = STL_TRUE;
    
    /**
     * 프로퍼티 이름을 이용하여 프로퍼티 정보 획득 
     */
    STL_TRY( knlFindStaticHash( gKnpPropertyMgr->mPropertyHash,
                                knpHashFunction,
                                knpHashCompareFunction,
                                (void*)aPropertyName,
                                (void**)&sContent,
                                aKnlEnv ) == STL_SUCCESS );

    if( sContent == NULL )
    {
        *aIsExist = STL_FALSE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Property 변경에 대하여 해당 Property의 속성을 만족하는지를 확인한다.
 * @param[in]     aContent             knpPropertyContent
 * @param[in]     aMode                knlPropertyModifyMode
 * @param[in]     aIsDeferred          DEFERRED 옵션
 * @param[in]     aScope               knlPropertyModifyScope
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code 
 * [KNL_ERRCODE_PROPERTY_INVALID_STARTUP_PHASE]
 * 해당 프로퍼티를 설정할 수 있는 StartupPhase가 아닙니다.
 * - 해당 프로퍼티를 설정할 수 있는 StartupPhase에서 수행해야 합니다.
 * [KNL_ERRCODE_PROPERTY_NOT_MODIFIABLE]
 * 해당 프로퍼티의 속성이 READ-ONLY 입니다.
 * - 해당 프로퍼티는 시스템 운영중에는 변경할 수 없으며, 시스템환경변수 및 프로퍼티 파일에서 변경해야만 적용가능합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_SES_MODIFY]
 * 해당 프로퍼티는 Alter Session으로 변경할 수 없습니다.
 * - 해당 프로퍼티는 Alter System 으로만 변경할 수 있습니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_SCOPE]
 * 해당 프로퍼티의 속성중 SCOPE가 맞지 않습니다.
 * - 해당 프로퍼티의 속성중 SCOPE가 FALSE일 경우에는 Binary 프로퍼티파일에만 적용가능합니다.
 * - Alter Session은 Scope=Memory로만 사용가능합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_MODE]
 * Property 변경 Mode가 정확하지 않습니다.
 * - 변경 Mode를 확인한다.
 * @endcode
 * @remarks
 */

stlStatus knpVerifyPropertyAttribute( knpPropertyContent     * aContent, 
                                      knlPropertyModifyMode    aMode,
                                      stlBool                  aIsDeferred,
                                      knlPropertyModifyScope   aScope,
                                      knlEnv                 * aKnlEnv )
{
    stlBool                 sModifiable     = STL_TRUE;
    stlBool                 sIsInitialied   = STL_FALSE;
    stlBool                 sPropertySesMod = STL_FALSE;
    knlPropertyModifyType   sPropertySysMod = KNL_PROPERTY_SYS_TYPE_MAX;
    
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /**
     * Property를 Heap 메모리에 구성하는 단계에서는 EntryPoint가 구성되어 있지 않다.
     */
    if( gKnEntryPoint != NULL )
    {
       sIsInitialied = STL_TRUE;
    }
    else
    {
       sIsInitialied = STL_FALSE; 
    }
    
    /**
     * KNL_PROPERTY_MODE_NONE일 경우에는 초기화(DEFAULT)의미하므로 
     * READ-ONLY인지를 체크하지 않는다.
     */
    if ( aMode != KNL_PROPERTY_MODE_NONE )
    {
        if( sIsInitialied == STL_TRUE )
        {
            STL_TRY( knpGetPropertyModifiable( aContent,
                                               &sModifiable,
                                               aKnlEnv ) == STL_SUCCESS);
       
            STL_TRY_THROW( sModifiable == STL_TRUE , RAMP_ERR_NOT_MODIFIABLE );
            
            /** 
             * SCOPE가 FILE일 경우에는 StartupPhase를 체크하지 않는다.
             * SCOPE가 File일 경우에는 다음 Startup시에 변경이 될텐데 
             * Property 변경을 특정 Startup Phase까지 올리는 건 불필요하다
             */
            if( aScope != KNL_PROPERTY_SCOPE_FILE )
            {
                /**
                 * Property 변경 가능한 Startup Phase에 따라 가능 여부 확인.
                 * KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW : 설정한 StartupPhase 이하에서만 가능
                 * KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER : 설정한 StartupPhase 이상에서만 가능
                 * KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY  : 설정한 StartupPhase 에서만 가능
                 */
                switch( aContent->mStartupPhaseDomain )
                {
                    case KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW:
                    {
                        STL_TRY_THROW ( gKnEntryPoint->mStartupPhase <= aContent->mStartupPhase,
                                        RAMP_ERR_INVALID_STARTUP_PHASE );
                        break;
                    }
                    case KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER:
                    {
                        STL_TRY_THROW ( gKnEntryPoint->mStartupPhase >= aContent->mStartupPhase,
                                        RAMP_ERR_INVALID_STARTUP_PHASE );
                        break;
                    }
                    case KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY:
                    {
                        STL_TRY_THROW ( gKnEntryPoint->mStartupPhase == aContent->mStartupPhase,
                                        RAMP_ERR_INVALID_STARTUP_PHASE );
                        break;
                    }
                    case KNP_PROPERTY_PHASE_DOMAIN_MAX:
                    default:
                        break;
                }
            }
        }/* end if initialize */
    }
    
    /**
     * 시스템 환경 변수 또는 Property 파일에서 읽은 내용은 Heap에 저장된 Property에 값을 저장하게 된다.
     * 이 경우에는 아래의 체크를 하지 않는다.
     */
    if( sIsInitialied == STL_TRUE )
    {
        /**
         * ALTER SESSION, ALTER SYSTEM, ALTER DATABASE에 따른 Property 변경 가능여부 확인
         */
        switch ( aMode )
        {
            case KNL_PROPERTY_MODE_NONE:
            {
                /**
                 * Nothing To Do.
                 * 초기화이므로 해당 속성을 verify할 필요가 없다.
                 */
                break;
            }   
            case KNL_PROPERTY_MODE_SES:
            {
                /**
                 * ALTER SESSION은 MEMORY만 변경할 수 있음...
                 */
                STL_TRY_THROW( aScope == KNL_PROPERTY_SCOPE_MEMORY, RAMP_ERROR_INVALID_SCOPE );
        
                STL_TRY( knpGetPropertySesModType( aContent,
                                                   &sPropertySesMod,
                                                   aKnlEnv ) == STL_SUCCESS );
        
                STL_TRY_THROW( sPropertySesMod == STL_TRUE, RAMP_ERR_INVALID_SES_MODIFY );
        
                break;
            }
            case KNL_PROPERTY_MODE_SYS:
            {
                /**
                 * ALTER SYSTEM ... SCOPE = MEMORY/FILE/BOTH 로 사용할 수 있으며,
                 * Property 속성이 가질 수 있는 값 3가지 IMMEDIATE/DEFERED/FALSE 중에
                 * 속성이 FALSE일 경우에는 반드시 SCOPE=FILE로만 사용할 수 있다.
                 */
                STL_TRY( knpGetPropertySysModType( aContent,
                                                   &sPropertySysMod,
                                                   aKnlEnv ) == STL_SUCCESS );

                switch( sPropertySysMod )
                {
                    case KNL_PROPERTY_SYS_TYPE_IMMEDIATE:
                        break;
                    case KNL_PROPERTY_SYS_TYPE_DEFERRED:
                        /**
                         * SCOPE = MEMORY/BOTH 인경우는 DEFERRED 속성을 갖는 Property는
                         * 반드시 DEFERRED 옵션을 명시해 주어야 한다.
                         */
                        if( aScope != KNL_PROPERTY_SCOPE_FILE )
                        {
                            STL_TRY_THROW( aIsDeferred == STL_TRUE,
                                           RAMP_ERROR_INVALID_MODE );
                        }
                        break;
                    case KNL_PROPERTY_SYS_TYPE_FALSE:
                        STL_TRY_THROW( aScope == KNL_PROPERTY_SCOPE_FILE ,
                                       RAMP_ERROR_INVALID_SCOPE );
                        break;
                    case KNL_PROPERTY_SYS_TYPE_NONE:
                        /**
                         * Property 속성이 NONE일 경우에는 변경이 불가능하다.
                         */
                        STL_THROW( RAMP_ERR_NOT_MODIFIABLE );
                        break;
                    default:
                        STL_ASSERT( STL_FALSE );
                        break;
                }

                break;
            }
            case KNL_PROPERTY_MODE_DATABASE:
            {
                /**
                 * What To do?
                 * ALTER DATABASE는 속성으로 관리를 하지 않음?
                 */
                break;
            }
            case KNL_PROPERTY_MODE_MAX:
            default:
            {
                STL_THROW( RAMP_ERROR_INVALID_MODE );
                break;
            }
        } /* end switch */
    } /* end if initialize */
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_STARTUP_PHASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_STARTUP_PHASE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aContent->mName );
    }
    STL_CATCH( RAMP_ERR_NOT_MODIFIABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_NOT_MODIFIABLE, 
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aContent->mName );
    }  
    STL_CATCH( RAMP_ERR_INVALID_SES_MODIFY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_SES_MODIFY,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aContent->mName );
    }
    STL_CATCH( RAMP_ERROR_INVALID_SCOPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_SCOPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aContent->mName );
    }
    STL_CATCH( RAMP_ERROR_INVALID_MODE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_MODE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aContent->mName );
    }
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 해당 Property에 저장되어 있는 값을 문자형 데이터를 변형하여 새로운 값으로 등록한다.
 * @param[in]     aContent             knpPropertyContent
 * @param[in]     aValue               입력할 값
 * @param[in]     aMode                해당 프로퍼티 변경 모드(Alter Session, Alter System, Alter Database)
 * @param[in]     aIsDeferred          DEFERRED 옵션
 * @param[in]     aScope               해당 프로퍼티 값의 적용 범위(Memory, File, Both)
 * @param[in,out] aKnlEnv              Kernel Environment
 * @remarks
 * <BR> Heap에 임시로 Memory를 할당하고 Shared Memory 구성시 Destroy하므로 Heap에 Property를 구성할 경우에만 사용할 수 있다. 
 * <BR> 이 함수는 프로퍼티 등록 및 시스템 환경변수와 Text Property File에서 데이터를 읽을 경우에만 사용한다.
 * <BR> Shared Memory에 Property가 구성된 이후에는 사용하면 안된다.
 */

stlStatus knpUpdatePropertyValueUseStr( knpPropertyContent     * aContent,
                                        stlChar                * aValue,
                                        knlPropertyModifyMode    aMode,
                                        stlBool                  aIsDeferred,
                                        knlPropertyModifyScope   aScope,
                                        knlEnv                 * aKnlEnv )
{
    void * sResult  = NULL;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue   != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /**
     * 입력받은 문자열로 부터 각 Property의 DataType에 맞는 형태로 변경한다.
     */
    STL_TRY( knpConvertFromString( aContent,
                                   aValue, 
                                   &sResult,
                                   aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 변경 가능한 Property인지를 판단한다.
     */
    STL_TRY( knpVerifyPropertyAttribute( aContent, 
                                         aMode,
                                         aIsDeferred,
                                         aScope,
                                         aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 입력하고자 하는 값이 정상적인지 판단한다.
     */
    STL_TRY( knpVerifyPropertyValue( aContent,
                                     sResult,
                                     aKnlEnv ) == STL_SUCCESS );
    
    /**
     * SCOPE 처리
     */ 
    if( ( aScope == KNL_PROPERTY_SCOPE_MEMORY ) ||
        ( aScope == KNL_PROPERTY_SCOPE_BOTH ) )
    {
        /**
        * Memory에 해당 Property에 값을 추가한다.
        */
        STL_TRY( knpAddPropertyValue( aContent,
                                      sResult,
                                      aMode,
                                      aIsDeferred,
                                      aKnlEnv ) == STL_SUCCESS );
        /**
         * ENV 반영
         */
        STL_TRY( knpUpdateSessionEnv( aContent,
                                      aMode,
                                      aIsDeferred,
                                      aKnlEnv ) == STL_SUCCESS );
    }
    
    if( ( aScope == KNL_PROPERTY_SCOPE_FILE || aScope == KNL_PROPERTY_SCOPE_BOTH ) 
          && ( aMode == KNL_PROPERTY_MODE_SYS ) )
    {
        /**
         * Alter System .. Scope=FILE일 경우에 해당 값을 mFileValue에 저장한다.
         */
        STL_TRY( knpAddPropertyFileValue( aContent,
                                          sResult,
                                          aMode,
                                          aKnlEnv ) == STL_SUCCESS );
                                           
        /**
         * Binary FILE에 해당 Property에 값을 추가한다.
         */
        STL_TRY( knpAddPropertyValueAtFile( aContent,
                                            sResult,
                                            aKnlEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief 해당 Property에 저장되어 있는 값을 새로운 값으로 등록한다.
 * @param[in]     aContent             knpPropertyContent
 * @param[in]     aValue               입력할 값
 * @param[in]     aMode                해당 프로퍼티 변경 모드(Alter Session, Alter System, Alter Database)
 * @param[in]     aIsDeferred          DEFERRED 옵션
 * @param[in]     aScope               해당 프로퍼티 값의 적용 범위(Memory, File, Both)
 * @param[in,out] aKnlEnv              Kernel Environment
 * @remarks
 * <BR> SCOPE=BOTH일 경우에 Memory 뿐만 아니라, File도 변경되어야 한다. 
 * <BR> 이 경우에 Memory 변경 시킨 후에 File 변경에 실패했다면 문제가 될텐데 ... 좀 더 생각해 봐야할 문제이다.
 */

stlStatus knpUpdatePropertyValue( knpPropertyContent      * aContent,
                                  void                    * aValue,
                                  knlPropertyModifyMode     aMode,
                                  stlBool                   aIsDeferred,
                                  knlPropertyModifyScope    aScope,
                                  knlEnv                  * aKnlEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue   != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /**
     * 변경 가능한 Property인지를 판단한다.
     */
    STL_TRY( knpVerifyPropertyAttribute( aContent, 
                                         aMode,
                                         aIsDeferred,
                                         aScope,
                                         aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 입력하고자 하는 값이 정상적인지 판단한다.
     */   
    STL_TRY( knpVerifyPropertyValue( aContent,
                                     aValue,
                                     aKnlEnv ) == STL_SUCCESS );
    /**
     * SCOPE 처리
     */ 
    if( ( aScope == KNL_PROPERTY_SCOPE_MEMORY ) ||
        ( aScope == KNL_PROPERTY_SCOPE_BOTH ) )
    {
        /**
        * Memory에 해당 Property에 값을 추가한다.
        */
        STL_TRY( knpAddPropertyValue( aContent,
                                      aValue,
                                      aMode,
                                      aIsDeferred,
                                      aKnlEnv ) == STL_SUCCESS );
        /**
         * ENV 반영
         */
        STL_TRY( knpUpdateSessionEnv( aContent,
                                      aMode,
                                      aIsDeferred,
                                      aKnlEnv ) == STL_SUCCESS );
    }
    
    if( ( aScope == KNL_PROPERTY_SCOPE_FILE || aScope == KNL_PROPERTY_SCOPE_BOTH ) 
          && ( aMode == KNL_PROPERTY_MODE_SYS ) )
    {
        /**
         * Alter System .. Scope=FILE일 경우에 해당 값을 mFileValue에 저장한다.
         */
        STL_TRY( knpAddPropertyFileValue( aContent,
                                          aValue,
                                          aMode,
                                          aKnlEnv ) == STL_SUCCESS );
        
        /**
         * Binary FILE에 해당 Property에 값을 추가한다.
         */
        STL_TRY( knpAddPropertyValueAtFile( aContent,
                                            aValue,
                                            aKnlEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 해당 Property가 해당 값을 FILE에 추가/변경하기 전에 Memory에 저장한다.
 * @param[in]     aContent             knpPropertyContent
 * @param[in]     aValue               입력할 값
 * @param[in]     aMode                해당 프로퍼티 변경 모드(Alter System)
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_DATATYPE]
 * 해당 Property의 DataType이 정상적이지 않습니다.
 * - 해당 Property의 DataType을 확인합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_STARTUP_PHASE]
 * 해당 프로퍼티를 설정할 수 있는 StartupPhase가 아닙니다.
 * - 해당 프로퍼티를 설정할 수 있는 StartupPhase에서 수행해야 합니다.
 * @endcode
 * @remarks
 * <BR> Alter System ... Scope=FILE 을 수행할 경우, FILE에 저장된 값을 Memory에 갖고있는다.
 * <BR> Fixed Table에서 사용된다.
 */

stlStatus knpAddPropertyFileValue( knpPropertyContent     * aContent, 
                                   void                   * aValue,
                                   knlPropertyModifyMode    aMode,
                                   knlEnv                 * aKnlEnv )
{
    stlUInt32   sValueLen   = 0;
    stlBool     sLatchState = STL_FALSE;
    stlBool     sIsSuccess  = STL_FALSE;
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue   != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /**
     * 초기화에서는 mFileValue를 넣을 수 없다.
     * NO-MOUNT 이상일 경우에만 FALSE Value를 넣을 수 있다.
     */
    STL_TRY_THROW ( gKnEntryPoint->mStartupPhase >= KNL_STARTUP_PHASE_NO_MOUNT,
                    RAMP_ERR_INVALID_STARTUP_PHASE );

    switch ( aContent->mDataType )
    {
        case KNL_PROPERTY_TYPE_BOOL:
        {
            sValueLen = STL_SIZEOF( stlInt64 );
            *(stlInt64*)aContent->mFileValue = (stlInt64)*(stlBool*)aValue;
            break;
        }
        case KNL_PROPERTY_TYPE_BIGINT:
        {
            sValueLen = STL_SIZEOF( stlInt64 );
            *(stlInt64*)aContent->mFileValue = *(stlInt64*)aValue;
            break;
        }    
        case KNL_PROPERTY_TYPE_VARCHAR:
        {
            sValueLen = stlStrlen( aValue );
            
            STL_TRY( knlAcquireLatch( &gKnpPropertyMgr->mLatch,
                                      KNL_LATCH_MODE_EXCLUSIVE,
                                      KNL_LATCH_PRIORITY_NORMAL,
                                      STL_INFINITE_TIME,
                                      &sIsSuccess,
                                      aKnlEnv ) == STL_SUCCESS );
            sLatchState = STL_TRUE;
            
            stlStrncpy( aContent->mFileValue, 
                        aValue, 
                        sValueLen + 1 );
            
            STL_TRY( knlReleaseLatch( &gKnpPropertyMgr->mLatch,
                                      aKnlEnv ) == STL_SUCCESS );
            sLatchState = STL_FALSE;
            
            break;
        }
        case KNL_PROPERTY_TYPE_MAX:
        default:
        {   
            STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
            break;
        }
    }
    
    aContent->mHasFileValue    = STL_TRUE;
    aContent->mFileValueLength = sValueLen;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERR_INVALID_STARTUP_PHASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_STARTUP_PHASE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aContent->mName );
    }
    STL_FINISH;
    
    if( sLatchState == STL_TRUE )
    {
        (void) knlReleaseLatch( &gKnpPropertyMgr->mLatch,
                                aKnlEnv );
        sLatchState = STL_FALSE;
    }

    return STL_FAILURE;
    
}


/**
 * @brief Property의 DataType별로 처리되며, 해당 Property에 값을 추가한다.
 * @param[in]     aContent             knpPropertyContent
 * @param[in]     aValue               입력할 값
 * @param[in]     aMode                knlPropertyModifyMode
 * @param[in]     aIsDeferred          DEFERRED 옵션
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_DATATYPE]
 * 해당 Property의 DataType이 정상적이지 않습니다.
 * - 해당 Property의 DataType을 확인합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_ID]
 * 해당 Property의 ID가 정상적이지 않습니다.
 * - 해당 Property의 ID를 확인합니다. 
 * @endcode
 * @remarks
 * <BR> 해당 함수를 이용해서 곧바로 Property에 값을 추가해서는 안된다.
 * <BR> 프로퍼티 속성 및 값의 Validation을 확인하지 않으므로, 
 * <BR> 반드시 knpUpdatePropertyValue(...)을 사용해야 한다.
 */

stlStatus knpAddPropertyValue( knpPropertyContent     * aContent, 
                               void                   * aValue,
                               knlPropertyModifyMode    aMode,
                               stlBool                  aIsDeferred,
                               knlEnv                 * aKnlEnv )
{
    stlUInt32        sValueLen    = 0;
    knlPropertyID    sPropertyID  = KNL_PROPERTY_ID_MAX;
    knlSessionEnv  * sSessionEnv  = NULL;
    stlBool          sLatchState  = STL_FALSE;
    stlBool          sIsSuccess   = STL_FALSE;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue   != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sSessionEnv = KNL_SESS_ENV( aKnlEnv );
    STL_PARAM_VALIDATE( sSessionEnv  != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    sPropertyID = aContent->mID;
    
    STL_TRY_THROW( sPropertyID < KNL_PROPERTY_ID_MAX, RAMP_ERROR_INVALID_PROPERTYID );    
    
    if( aMode == KNL_PROPERTY_MODE_SES )
    {
        /**
         * ALTER SESSION일 경우에는 ENV만 바꾼다.
         */
        switch ( aContent->mDataType )
        {
            case KNL_PROPERTY_TYPE_BOOL:
            {
                sSessionEnv->mPropertyInfo.mPropertyValue[sPropertyID] = (stlInt64)*(stlBool*)aValue;
                
                /**
                 * ALTER SESSION으로 변경될 경우에는 해당 Property가 변경되어있음을 알린다.
                 */
                sSessionEnv->mPropertyInfo.mModified[sPropertyID] = STL_TRUE;
                break;
            }
            case KNL_PROPERTY_TYPE_BIGINT:
            {
                sSessionEnv->mPropertyInfo.mPropertyValue[sPropertyID] = *(stlInt64*)aValue;
                
                /**
                 * ALTER SESSION으로 변경될 경우에는 해당 Property가 변경되어있음을 알린다.
                 */
                sSessionEnv->mPropertyInfo.mModified[sPropertyID] = STL_TRUE;
                break;
            }
            case KNL_PROPERTY_TYPE_VARCHAR:
            {
                sValueLen = stlStrlen( aValue );
                
                stlStrncpy( (stlChar*)sSessionEnv->mPropertyInfo.mPropertyValue[sPropertyID],
                            aValue,
                            sValueLen + 1 );
                
                sSessionEnv->mPropertyInfo.mModified[sPropertyID] = STL_TRUE;
                break;
            }
            case KNL_PROPERTY_TYPE_MAX:
            default:
            {   
                STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
                break;
            }
        }
    }
    else
    {
        switch ( aContent->mDataType )
        {
            case KNL_PROPERTY_TYPE_BOOL:
            {
                if ( aIsDeferred == STL_FALSE && 
                     aMode       != KNL_PROPERTY_MODE_NONE )
                {
                    sSessionEnv->mPropertyInfo.mPropertyValue[sPropertyID] = (stlInt64)*(stlBool*)aValue;
                }
                
                sValueLen = STL_SIZEOF( stlInt64 );
                *(stlInt64*)aContent->mValue = (stlInt64)*(stlBool*)aValue;
                break;
            }
            case KNL_PROPERTY_TYPE_BIGINT:
            {
                if ( aIsDeferred == STL_FALSE && 
                     aMode       != KNL_PROPERTY_MODE_NONE )
                {
                    sSessionEnv->mPropertyInfo.mPropertyValue[sPropertyID] = *(stlInt64*)aValue;
                }
                sValueLen = STL_SIZEOF( stlInt64 );
                *(stlInt64*)aContent->mValue = *(stlInt64*)aValue;
                break;
            }    
            case KNL_PROPERTY_TYPE_VARCHAR:
            {
                /**
                 * VARCHAR Type은 IMMEDIATE가 없기 때문에 SessionEnv에 적용하지 않는다.
                 * SHM 값만 변경하도록 한다.
                 */
                STL_TRY( knlAcquireLatch( &gKnpPropertyMgr->mLatch,
                                          KNL_LATCH_MODE_EXCLUSIVE,
                                          KNL_LATCH_PRIORITY_NORMAL,
                                          STL_INFINITE_TIME,
                                          &sIsSuccess,
                                          aKnlEnv ) == STL_SUCCESS );
                sLatchState = STL_TRUE;
                
                sValueLen = stlStrlen( aValue );
                
                stlStrncpy( aContent->mValue, 
                            aValue,
                            sValueLen + 1 );
                
                STL_TRY( knlReleaseLatch( &gKnpPropertyMgr->mLatch,
                                          aKnlEnv ) == STL_SUCCESS );
                sLatchState = STL_FALSE;
                break;
            }
            case KNL_PROPERTY_TYPE_MAX:
            default:
            {   
                STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
                break;
            }
        }
        
        aContent->mValueLength = sValueLen;
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERROR_INVALID_PROPERTYID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_ID,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_FINISH;
    
    if( sLatchState == STL_TRUE )
    {
        (void) knlReleaseLatch( &gKnpPropertyMgr->mLatch,
                                aKnlEnv );
        sLatchState = STL_FALSE;
    }

    return STL_FAILURE;
}


/**
 * @brief Property의 DataType별로 처리되며, 해당 Property에 Init 값을 추가한다.
 * @param[in]     aContent             knpPropertyContent
 * @param[in]     aValue               입력할 값
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_DATATYPE]
 * 해당 Property의 DataType이 정상적이지 않습니다.
 * - 해당 Property의 DataType을 확인합니다.
 * @endcode
 * @remarks
 */

stlStatus knpAddPropertyInitValue( knpPropertyContent  * aContent, 
                                   void                * aValue,
                                   knlEnv              * aKnlEnv )
{
    stlUInt32   sValueLen = 0;
    
    /**
     * Parameter Validation
     */    
    STL_PARAM_VALIDATE( aContent    != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue      != NULL, KNL_ERROR_STACK(aKnlEnv) );
        
    /**
     * Property의 DataType에 맞게 Init값을 추가한다.
     */
    switch ( aContent->mDataType )
    {
        case KNL_PROPERTY_TYPE_BOOL:
        case KNL_PROPERTY_TYPE_BIGINT:
        {
            sValueLen = STL_SIZEOF( stlInt64 );
            *(stlInt64*)aContent->mInitValue = *(stlInt64*)aValue;
            break;
        }    
        case KNL_PROPERTY_TYPE_VARCHAR:
        {
            /**
             * Initialize 하는 과정에 구성되므로 동시성 제어가 필요없음
             */
            sValueLen = stlStrlen( aValue );
            stlStrncpy( aContent->mInitValue, 
                        aValue, 
                        sValueLen + 1 );
            break;
        }
        case KNL_PROPERTY_TYPE_MAX:
        default:
        {   
            STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
            break;
        }
    }
    
    aContent->mInitValueLength = sValueLen;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Property의 DataType별로 처리되며, 정상적인 값인지를 확인한다.
 * @param[in]     aContent             knpPropertyContent
 * @param[in]     aValue               입력할 값
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_DATATYPE]
 * 해당 Property의 DataType이 정상적이지 않습니다.
 * - 해당 Property의 DataType을 확인합니다.
 * @endcode
 * @remarks
 */

stlStatus knpVerifyPropertyValue( knpPropertyContent     * aContent, 
                                  void                   * aValue,
                                  knlEnv                 * aKnlEnv )
{
    /**
     * Parameter Validation
     */     
    STL_PARAM_VALIDATE( aContent    != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue      != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /**
     * Property의 DataType에 맞게 입력된 값이 정상적인지를 확인한다.
     */    
    switch ( aContent->mDataType )
    {
        case KNL_PROPERTY_TYPE_BOOL:
        {
            STL_TRY( knpBooleanVerifyValue( aContent,
                                            aValue,
                                            aKnlEnv ) == STL_SUCCESS );
            break;
        }
        case KNL_PROPERTY_TYPE_BIGINT:
        {
            STL_TRY( knpBigIntVerifyValue( aContent,
                                           aValue,
                                           aKnlEnv ) == STL_SUCCESS );
            break;
        }
        case KNL_PROPERTY_TYPE_VARCHAR:
        {
            STL_TRY( knpVarcharVerifyValue( aContent,
                                            aValue,
                                            aKnlEnv ) == STL_SUCCESS );
            break;
        }
        case KNL_PROPERTY_TYPE_MAX:
        default:
        {   
            STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
            break;
        }
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief Gliese Home Directory를 반환한다.
 * @return Home Directory 경로
 * @remarks
 */

stlChar * knpGetHomeDir()
{
    return gKnpPropertyMgr->mHomeDir;
}


/**
 * @brief 입력 받은 문자열을 해당 Property의 데이터타입으로 변경한다.
 * @param[in]     aContent             knpPropertyContent
 * @param[in]     aValue               입력할 값
 * @param[out]    aResult              변경된 값
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_DATATYPE]
 * 해당 Property의 DataType이 정상적이지 않습니다.
 * - 해당 Property의 DataType을 확인합니다.
 * @endcode
 * @remarks
 */

stlStatus knpConvertFromString ( knpPropertyContent  * aContent, 
                                 void                * aValue,
                                 void               ** aResult,
                                 knlEnv              * aKnlEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue   != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /**
     * 데이터타입에 맞는 형태로 변경한다.
     */
    switch ( aContent->mDataType )
    {
        case KNL_PROPERTY_TYPE_BOOL:
        {
            STL_TRY( knpBooleanConvertFromString( aValue,
                                                  aResult,
                                                  aKnlEnv ) == STL_SUCCESS );
            break;
        }
        case KNL_PROPERTY_TYPE_BIGINT:
        {
            STL_TRY( knpBigIntConvertFromString( aValue,
                                                 aResult,
                                                 aKnlEnv ) == STL_SUCCESS );
            break;
        }    
        case KNL_PROPERTY_TYPE_VARCHAR:
        {
            STL_TRY( knpVarcharConvertFromString( aValue,
                                                  aResult,
                                                  aKnlEnv ) == STL_SUCCESS );
            break;
        }
        case KNL_PROPERTY_TYPE_MAX:
        default:
        {   
            STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
            break;
        }
    }    
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Property 정보를 Session Env에 세팅한다. 
 * @param[in]     aSessionEnv          Kernel Session Environment
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_DATATYPE]
 * 해당 Property의 DataType이 정상적이지 않습니다.
 * - 해당 Property의 DataType을 확인합니다.
 * @endcode
 * @remarks
 * <BR> Session별 정보를 저장하기 위해서 Property Value & Init Value를 ENV에 저장한다.
 * <BR> Boolean, Number 데이터만 저장된다. String Value는 Property에서 직접 읽어와야 한다.
 * <BR> 
 * <BR> 튜닝의 여지가 있음..
 * <BR> Connection이 일어날 경우에 이 함수가 호출될텐데... 여기서 항상 Shared Memory의 값을 읽어서 가져오는 것이 아니라
 * <BR> 미리 배열을 생성해 놓고 Shared Memory에 구성해 놓고 memcpy를 하도록 하면 좀 나을 듯 싶다.
 * <BR> 물론 Alter System을 이용하여 Update가 일어날 경우에 미리 생성해 놓은 배열의 값도 함께 변경해야 한다.
 */

stlStatus knpCopyPropertyValueToSessionEnv ( knlSessionEnv * aSessionEnv,
                                             knlEnv        * aKnlEnv )
{
    stlUInt32            sIdx        = 0;
    stlUInt32            sOffset     = 0;
    knpPropertyContent * sContent    = NULL;
    stlChar            * sValue      = NULL;
    stlBool              sLatchState = STL_FALSE;
    stlBool              sIsSuccess  = STL_FALSE;

    STL_DASSERT( gKnpPropertyMgr->mVarcharTotalLen > 0 );
    
    STL_TRY( knlAllocRegionMem( &aSessionEnv->mPropertyMem,
                                gKnpPropertyMgr->mVarcharTotalLen * 2,      //Value와 InitValue
                                (void**)&aSessionEnv->mPropertyInfo.mVarcharValue,
                                aKnlEnv ) == STL_SUCCESS );
    
    sValue = aSessionEnv->mPropertyInfo.mVarcharValue;
    
    stlMemset( sValue, 0x00, gKnpPropertyMgr->mVarcharTotalLen );
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aSessionEnv  != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    for( sIdx = 0; sIdx < gKnpPropertyMgr->mPropertyCnt ; sIdx++ )
    {
        if ( gKnpPropertyMgr->mDoCopyToSessionEnv[sIdx] == STL_TRUE )
        {
            sContent = gKnpPropertyMgr->mPropertyAddr[sIdx];
            
            switch ( sContent->mDataType )
            {
                case KNL_PROPERTY_TYPE_BOOL:
                case KNL_PROPERTY_TYPE_BIGINT:
                {
                    aSessionEnv->mPropertyInfo.mPropertyValue[sIdx]     = *(stlInt64*)sContent->mValue;
                    aSessionEnv->mPropertyInfo.mPropertyInitValue[sIdx] = *(stlInt64*)sContent->mValue;
                    break;
                }
                case KNL_PROPERTY_TYPE_VARCHAR:
                {
                    /**
                     * INIT VALUE도 함께 저장해야 함!!!
                     */
                    aSessionEnv->mPropertyInfo.mPropertyValue[ sIdx ] = (stlInt64)&sValue[ sOffset ];
                    
                    STL_TRY( knlAcquireLatch( &gKnpPropertyMgr->mLatch,
                                              KNL_LATCH_MODE_SHARED,
                                              KNL_LATCH_PRIORITY_NORMAL,
                                              STL_INFINITE_TIME,
                                              &sIsSuccess,
                                              aKnlEnv ) == STL_SUCCESS );
                    sLatchState = STL_TRUE;
                    
                    stlStrncpy( &sValue[sOffset], sContent->mValue, sContent->mValueLength + 1 );                      // Value
                    stlStrncpy( &sValue[sOffset + sContent->mMax], sContent->mValue, sContent->mValueLength + 1 );     // Init Value
                    
                    STL_TRY( knlReleaseLatch( &gKnpPropertyMgr->mLatch,
                                              aKnlEnv ) == STL_SUCCESS );
                    sLatchState = STL_FALSE;
                    
                    sOffset += ( sContent->mMax * 2 );
                    
                    STL_DASSERT( sOffset <= ( gKnpPropertyMgr->mVarcharTotalLen * 2 ) );
                    break;   
                }
                case KNL_PROPERTY_TYPE_MAX:
                default:
                {   
                    STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
                    break;
                }
            }
            
        }
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_FINISH;
    
    if( sLatchState == STL_TRUE )
    {
        (void) knlReleaseLatch( &gKnpPropertyMgr->mLatch,
                                aKnlEnv );
        sLatchState = STL_FALSE;
    }

    return STL_FAILURE;
}


/**
 * @brief 해당 Property에 저장된 값의 길이를 반환한다.
 * @param[in]     aContent             knpPropertyContent
 * @param[out]    aValue               Property에 저장된 값의 길이
 * @param[in,out] aKnlEnv              Kernel Environment
 * @remarks
 */

stlStatus knpGetPropertyValueLen( knpPropertyContent  * aContent,
                                  stlUInt32           * aValue, 
                                  knlEnv              * aKnlEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue   != NULL, KNL_ERROR_STACK(aKnlEnv) );

    /**
     * Property의 Value Length를 넘겨준다.
     */
    *aValue = aContent->mValueLength;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 해당 Property에 저장된 값의 최대 길이를 반환한다.
 * @param[in]     aContent             knpPropertyContent
 * @param[out]    aValue               Property에 저장된 값의 최대 길이
 * @param[in,out] aKnlEnv              Kernel Environment
 * @remarks
 */

stlStatus knpGetPropertyValueMaxLen( knpPropertyContent  * aContent,
                                     stlUInt32           * aValue, 
                                     knlEnv              * aKnlEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue   != NULL, KNL_ERROR_STACK(aKnlEnv) );

    /**
     * Property의 Value Length를 넘겨준다.
     */
    *aValue = aContent->mMax;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief 해당 Property의 Alter System으로 변경 Mode 속성을 얻어온다.
 * @param[in]     aContent             knpPropertyContent
 * @param[out]    aType                knlPropertyModifyType
 * @param[in,out] aKnlEnv              Kernel Environment
 * @remarks
 */

stlStatus knpGetPropertySysModType( knpPropertyContent     * aContent,
                                    knlPropertyModifyType  * aType,
                                    knlEnv                 * aKnlEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aType    != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /**
     * 속성값을 얻어온다.
     */
    if ( ( KNP_ATTR_MD_SYS_MASK & aContent->mFlag ) == KNP_ATTR_MD_SYS_IMMEDIATE )
    {
        *aType = KNL_PROPERTY_SYS_TYPE_IMMEDIATE;
    }
    else if( ( KNP_ATTR_MD_SYS_MASK & aContent->mFlag ) == KNP_ATTR_MD_SYS_DEFERRED )
    {
        *aType = KNL_PROPERTY_SYS_TYPE_DEFERRED;
    }
    else if( ( KNP_ATTR_MD_SYS_MASK & aContent->mFlag ) == KNP_ATTR_MD_SYS_FALSE )
    {
        *aType = KNL_PROPERTY_SYS_TYPE_FALSE;
    }
    else if( ( KNP_ATTR_MD_SYS_MASK & aContent->mFlag ) == KNP_ATTR_MD_SYS_NONE )
    {
        *aType = KNL_PROPERTY_SYS_TYPE_NONE;
    }
    else
    {
        *aType = KNL_PROPERTY_SYS_TYPE_MAX;
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 해당 Property의 Alter Session으로 변경 가능 여부를 얻어온다.
 * @param[in]     aContent             knpPropertyContent
 * @param[out]    aType                Alter Session 가능 여부
 * @param[in,out] aKnlEnv              Kernel Environment
 * @remarks
 */

stlStatus knpGetPropertySesModType( knpPropertyContent  * aContent,
                                    stlBool             * aType,
                                    knlEnv              * aKnlEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aType    != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /**
     * Alter Session으로 변경가능 여부를 얻어온다.
     */
    if ( ( KNP_ATTR_MD_SES_MASK & aContent->mFlag ) == KNP_ATTR_MD_SES_TRUE )
    {
        *aType = STL_TRUE;
    }
    else
    {
        *aType = STL_FALSE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 해당 Property의 변경 가능 여부를 얻어온다.
 * @param[in]     aContent             knpPropertyContent
 * @param[out]    aType                변경 가능 여부
 * @param[in,out] aKnlEnv              Kernel Environment
 * @remarks
 * <BR> aType이 STL_TRUE일 경우 변경 가능하다.
 */

stlStatus knpGetPropertyModifiable ( knpPropertyContent  * aContent,
                                     stlBool             * aType,
                                     knlEnv              * aKnlEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aType    != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /**
     * 변경 가능 여부 속성을 받아온다.
     */
    if ( ( KNP_ATTR_MD_MASK & aContent->mFlag ) == KNP_ATTR_MD_WRITABLE )
    {
        *aType = STL_TRUE;
    }
    else
    {
        *aType = STL_FALSE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 해당 Property의 Range Check 여부를 얻어온다.
 * @param[in]     aContent             knpPropertyContent
 * @param[out]    aType                Range Check 여부
 * @param[in,out] aKnlEnv              Kernel Environment
 * @remarks
 * <BR> aType이 STL_TRUE일 경우 Range Check를 한다.
 */

stlStatus knpGetPropertyCheckRange ( knpPropertyContent  * aContent,
                                     stlBool             * aType,
                                     knlEnv              * aKnlEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aType    != NULL, KNL_ERROR_STACK(aKnlEnv) );

    /**
     * Range Check 여부를 얻어온다.
     */
    if ( ( KNP_ATTR_RANGE_MASK & aContent->mFlag ) == KNP_ATTR_RANGE_CHECK )
    {
        *aType = STL_TRUE;
    }
    else
    {
        *aType = STL_FALSE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief 해당 Property의 Increase 여부를 얻어온다.
 * @param[in]     aContent             knpPropertyContent
 * @param[out]    aType                Increase Check 여부
 * @param[in,out] aKnlEnv              Kernel Environment
 * @remarks
 * <BR> aType이 STL_TRUE일 경우 Increase Check를 한다.
 */

stlStatus knpGetPropertyCheckIncrease ( knpPropertyContent  * aContent,
                                        stlBool             * aType,
                                        knlEnv              * aKnlEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aType    != NULL, KNL_ERROR_STACK(aKnlEnv) );

    /**
     * Range Check 여부를 얻어온다.
     */
    if ( ( KNP_ATTR_INCREASE_MASK & aContent->mFlag ) == KNP_ATTR_INCREASE_CHECK )
    {
        *aType = STL_TRUE;
    }
    else
    {
        *aType = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 해당 Property가 Internal/External/Hidden 속성을 얻어온다.
 * @param[in]     aContent             knpPropertyContent
 * @param[out]    aType                Domain (External/Internal/Hidden)
 * @param[in,out] aKnlEnv              Kernel Environment
 * @remarks
 * <BR> aType이 STL_TRUE일 경우 Internal 속성을 갖는다.
 */

stlStatus knpGetPropertyDomain ( knpPropertyContent  * aContent,
                                 knlPropertyDomain   * aType,
                                 knlEnv              * aKnlEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aType    != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /**
     * Domain 속성을 얻어온다.
     */
    if ( ( KNP_ATTR_DOMAIN_MASK & aContent->mFlag ) == KNP_ATTR_DOMAIN_EXTERNAL )
    {
        *aType = KNL_PROPERTY_DOMAIN_EXTERNAL;
    }
    else if ( ( KNP_ATTR_DOMAIN_MASK & aContent->mFlag ) == KNP_ATTR_DOMAIN_INTERNAL )
    {
        *aType = KNL_PROPERTY_DOMAIN_INTERNAL;
    }
    else if ( ( KNP_ATTR_DOMAIN_MASK & aContent->mFlag ) == KNP_ATTR_DOMAIN_HIDDEN )
    {
        *aType = KNL_PROPERTY_DOMAIN_HIDDEN;
    }
    else
    {
        *aType = KNL_PROPERTY_DOMAIN_MAX;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
    
}


/**
 * @brief Datatype을 얻어온다.
 * @param[in]       aContent           knpPropertyContent
 * @param[out]      aType              knlPropertyDataType
 * @param[in,out]   aKnlEnv            Kernel Environment
 */
         
stlStatus knpGetPropertyDataType ( knpPropertyContent  * aContent,
                                   knlPropertyDataType * aType,
                                   knlEnv              * aKnlEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aType    != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /**
     * DataType을 얻어온다.
     */
    *aType = aContent->mDataType;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 해당 Property의 타입이 Bigint일 경우에 단위(UNIT)를 얻어온다.
 * @param[in]       aContent           knpPropertyContent
 * @param[out]      aType              knlPropertyBigintUnit
 * @param[in,out]   aKnlEnv            Kernel Environment
 */

stlStatus knpGetPropertyBigIntUnit( knpPropertyContent     * aContent,
                                    knlPropertyBigintUnit  * aType,
                                    knlEnv                 * aKnlEnv )
{
    knlPropertyDataType  sType;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aType    != NULL, KNL_ERROR_STACK(aKnlEnv) );
           
    STL_TRY( knpGetPropertyDataType( aContent, 
                                     &sType,
                                     aKnlEnv ) == STL_SUCCESS );
    
    if ( sType != KNL_PROPERTY_TYPE_BIGINT )
    {
        *aType = KNL_PROPERTY_BIGINT_UNIT_NONE;
    }
    else
    {
        if ( ( KNP_ATTR_UNIT_TIME_SIZE_MASK & aContent->mFlag ) == KNP_ATTR_UNIT_NONE  )
        {
            *aType = KNL_PROPERTY_BIGINT_UNIT_NONE;
        }
        else if( ( KNP_ATTR_UNIT_TIME_SIZE_MASK & aContent->mFlag ) == KNP_ATTR_UNIT_SIZE )
        {
            *aType = KNL_PROPERTY_BIGINT_UNIT_SIZE;
        }
        else if( ( KNP_ATTR_UNIT_TIME_SIZE_MASK & aContent->mFlag ) == KNP_ATTR_UNIT_TIME )
        {
            *aType = KNL_PROPERTY_BIGINT_UNIT_TIME;
        }
        else
        {
            *aType = KNL_PROPERTY_BIGINT_UNIT_MAX;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}


/** 입력한 단위를 변환하여 Value를 리턴한다.
 * @brief 
 * @param[in]       aContent           knpPropertyContent
 * @param[in]       aInStr             stlChar Type의 단위를 포함한 Value
 * @param[out]      aValue             단위를 변환한 결과 값
 * @param[in,out]   aKnlEnv            Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_VALUE]
 * 해당 Property의 값이 정상적이지 않습니다.
 * - 해당 값을 확인합니다.
 * @endcode
 * @remarks
 * <BR> 해당 Property의 타입이 Bigint일 경우에 사용되며, 입력한 단위와 Property의 단위를 계산하여 값을 리턴한다.
 */

stlStatus knpConvertUnitToValue( knpPropertyContent  * aContent,
                                 stlChar             * aInStr,
                                 stlInt64            * aValue,
                                 knlEnv              * aKnlEnv )
{
    stlChar               sUnitValueStr[KNL_PROPERTY_STRING_MAX_LENGTH] = {0};
    knlPropertyBigintUnit sUnitType   = KNL_PROPERTY_BIGINT_UNIT_MAX;
    stlUInt32             sLen        = 0;
    stlInt64              sUnit       = 1;
    stlInt64              sUnitValue  = 0;
    stlChar               sUnitStr[3] = {0};
    stlChar             * sEndPtr;                   //CONVERT에 사용됨
    stlBool               sFound      = STL_FALSE;

    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aInStr   != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue   != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    sLen = stlStrlen( aInStr );
    
    if ( sLen >= 2 )
    {
        STL_TRY( knpGetPropertyBigIntUnit( aContent,
                                           &sUnitType,
                                           aKnlEnv ) == STL_SUCCESS );
        /**
         * SIZE는 Byte로 변환해야 하며, TIME은 Milli-Second로 변환한다.
         */
        switch( sUnitType )
        {
            case KNL_PROPERTY_BIGINT_UNIT_SIZE:
            {
                stlStrncpy( sUnitStr, &aInStr[sLen - 1], 2 );
                stlStrncpy( sUnitValueStr, aInStr, sLen );
        
                if( stlStrncasecmp( sUnitStr, "K", 1 ) == 0 )
                {
                    sUnit = STL_INT64_C(1024);
                    sFound = STL_TRUE;
                }
                else if (stlStrncasecmp( sUnitStr, "M", 1 ) == 0 )
                {
                    sUnit = STL_INT64_C(1024) * STL_INT64_C(1024);
                    sFound = STL_TRUE;
                }
                else if (stlStrncasecmp( sUnitStr, "G", 1 ) == 0 )
                {
                    sUnit = STL_INT64_C(1024) * STL_INT64_C(1024) * STL_INT64_C(1024);
                    sFound = STL_TRUE;
                }
                else if (stlStrncasecmp( sUnitStr, "T", 1 ) == 0 )
                {
                    sUnit = STL_INT64_C(1024) * STL_INT64_C(1024) * STL_INT64_C(1024) * STL_INT64_C(1024);
                    sFound = STL_TRUE;
                }
                else if (stlStrncasecmp( sUnitStr, "P", 1 ) == 0 )
                {
                    sUnit = STL_INT64_C(1024) * STL_INT64_C(1024) * STL_INT64_C(1024) * STL_INT64_C(1024) * STL_INT64_C(1024);
                    sFound = STL_TRUE;
                }
                else
                {
                    sFound = STL_FALSE;
                }
                break;
            }

            case KNL_PROPERTY_BIGINT_UNIT_TIME:
            {
                stlStrncpy( sUnitStr, &aInStr[sLen - 2], 3 );
                stlStrncpy( sUnitValueStr, aInStr, sLen - 1 );
        
                if( stlStrncasecmp( sUnitStr, "SS", 2 ) == 0 )
                {
                    sUnit = STL_TIME_C(1000);
                    sFound = STL_TRUE;
                }
                else if (stlStrncasecmp( sUnitStr, "MI", 2 ) == 0 )
                {
                    sUnit = STL_TIME_C(1000) * STL_TIME_C(60);
                    sFound = STL_TRUE;
                }
                else if (stlStrncasecmp( sUnitStr, "HH", 2 ) == 0 )
                {
                    sUnit = STL_TIME_C(1000) * STL_TIME_C(60) * STL_TIME_C(60);
                    sFound = STL_TRUE;
                }
                else if (stlStrncasecmp( sUnitStr, "DD", 2 ) == 0 )
                {
                    sUnit = STL_TIME_C(1000) * STL_TIME_C(60) * STL_TIME_C(60) * STL_TIME_C(24);
                    sFound = STL_TRUE;
                }
                else
                {
                    sFound = STL_FALSE;
                }
                break;
            }
            case KNL_PROPERTY_BIGINT_UNIT_NONE:
            {
                sFound = STL_FALSE;
                break;
            }
            default:
            {
                /**
                 * Nothing To Do..
                 */
                break;
            }
        } /* end switch */
        
        if ( sFound == STL_TRUE )
        {
            STL_TRY( stlStrToInt64( sUnitValueStr,
                                    STL_NTS,
                                    &sEndPtr,
                                    10,
                                    &sUnitValue, 
                                    KNL_ERROR_STACK(aKnlEnv) ) == STL_SUCCESS );
            
            /** 입력된 String이 정상적으로 변환이 되었을 경우에는 EndPtr는 0 된다. */
            if ( *sEndPtr != 0 )
            {
                STL_THROW( RAMP_ERR_INVALID_VALUE );
            }
            
            *aValue = sUnitValue * sUnit;
        }
        else
        {
            STL_TRY( stlStrToInt64( aInStr,
                                    STL_NTS,
                                    &sEndPtr,
                                    10,
                                    aValue, 
                                    KNL_ERROR_STACK(aKnlEnv) ) == STL_SUCCESS );
            
            /** 입력된 String이 정상적으로 변환이 되었을 경우에는 EndPtr는 0 된다. */
            if ( *sEndPtr != 0 )
            {
                STL_THROW( RAMP_ERR_INVALID_VALUE );
            }
        }
    }
    else
    {
        STL_TRY( stlStrToInt64( aInStr,
                                STL_NTS,
                                &sEndPtr,
                                10,
                                aValue, 
                                KNL_ERROR_STACK(aKnlEnv) ) == STL_SUCCESS );
        
        /** 입력된 String이 정상적으로 변환이 되었을 경우에는 EndPtr는 0 된다. */
        if ( *sEndPtr != 0 )
        {
            STL_THROW( RAMP_ERR_INVALID_VALUE );
        }
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_VALUE, 
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aContent->mName );
    }
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 해당 Property에 저장된 Boolean 값을 반환한다.
 * @param[in] aPropertyID          Property Identifier
 * @param[in] aKnlEnv              Kernel Environment
 * @return
 * Property에 저장된 Boolean 값
 * @remarks
 */
stlBool knpGetPropertyBoolValue( knlPropertyID    aPropertyID,
                                 knlEnv         * aKnlEnv )
{
    knlSessionEnv * sSessionEnv;

    /**
     * Parameter Validation
     */
    
    STL_DASSERT( aKnlEnv != NULL );
    STL_DASSERT( aPropertyID < KNL_PROPERTY_ID_MAX );

    /**
     * Return Value
     */
    
    sSessionEnv = KNL_SESS_ENV( aKnlEnv );
    
    return (stlBool) sSessionEnv->mPropertyInfo.mPropertyValue[aPropertyID];
}

/**
 * @brief 해당 Property에 저장된 Bigint 값을 반환한다.
 * @param[in] aPropertyID          Property Identifier
 * @param[in] aKnlEnv              Kernel Environment
 * @return
 * Property에 저장된 Bigint 값
 * @remarks
 */
stlInt64 knpGetPropertyBigIntValue( knlPropertyID    aPropertyID,
                                    knlEnv         * aKnlEnv )
{
    knlSessionEnv * sSessionEnv;

    /**
     * Parameter Validation
     */
    
    STL_DASSERT( aKnlEnv != NULL );
    STL_DASSERT( aPropertyID < KNL_PROPERTY_ID_MAX );

    /**
     * Return Value
     */
    
    sSessionEnv = KNL_SESS_ENV( aKnlEnv );
    
    return (stlInt64) sSessionEnv->mPropertyInfo.mPropertyValue[aPropertyID];
}


/**
 * @brief 해당 Property에 저장된 String Pointer 값을 반환한다.
 * @param[in] aPropertyID          Property Identifier
 * @param[in] aKnlEnv              Kernel Environment
 * @return
 * Property에 저장된 String Pointer 값
 * @remarks
 */
stlChar * knpGetPropertyStringValue( knlPropertyID    aPropertyID,
                                     knlEnv         * aKnlEnv )
{
    /**
     * @todo String 계열의 Property 를 획득할 수 있는 체계가 필요함.
     */
    
    return NULL;
}


/**
 * @brief 해당 Property에 저장된 Boolean 값을 반환한다.
 * @param[in] aPropertyID          Property Identifier
* @return
 * Property에 저장된 Boolean 값
 * @remarks
 */
stlBool knpGetPropertyBoolValueAtShm( knlPropertyID aPropertyID )
{
    knpPropertyContent   * sContent = NULL;
    
    /**
     * Parameter Validation
     */
    STL_DASSERT( aPropertyID < KNL_PROPERTY_ID_MAX );
    
    sContent = gKnpPropertyMgr->mPropertyAddr[aPropertyID];
    
    return (stlBool)*(stlInt64*)sContent->mValue;
}

/**
 * @brief 해당 Property에 저장된 Bigint 값을 반환한다.
 * @param[in] aPropertyID          Property Identifier
 * @return
 * Property에 저장된 Bigint 값
 * @remarks
 */
stlInt64 knpGetPropertyBigIntValueAtShm( knlPropertyID aPropertyID )
{
    knpPropertyContent   * sContent = NULL;
    
    /**
     * Parameter Validation
     */
    STL_DASSERT( aPropertyID < KNL_PROPERTY_ID_MAX );
    
    sContent = gKnpPropertyMgr->mPropertyAddr[aPropertyID];
    
    return *(stlInt64*)sContent->mValue;
}

/**
 * @brief 해당 Property에 저장된 Boolean 값을 SessionEnv 에서 얻어온다.
 * @param[in] aPropertyID          Property Identifier
 * @param[in] aSessionEnv          knlSessionEnv
* @return
 * SessionEnv에 저장된 Property Boolean 값
 * @remarks
 */
stlBool knpGetPropertyBoolValue4SessEnv( knlPropertyID   aPropertyID,
                                         knlSessionEnv * aSessionEnv )
{
    /**
     * Parameter Validation
     */
    STL_DASSERT( aPropertyID < KNL_PROPERTY_ID_MAX );
    STL_DASSERT( aSessionEnv != NULL );
    
    return (stlBool) aSessionEnv->mPropertyInfo.mPropertyValue[aPropertyID];
}

/**
 * @brief 해당 Property에 저장된 Bigint 값을 SessionEnv 에서 얻어온다.
 * @param[in] aPropertyID          Property Identifier
 * @param[in] aSessionEnv          knlSessionEnv
 * @return
 * SessionEnv에 저장된 Property Bigint 값
 * @remarks
 */
stlInt64 knpGetPropertyBigIntValue4SessEnv( knlPropertyID   aPropertyID,
                                            knlSessionEnv * aSessionEnv )
{
    /**
     * Parameter Validation
     */
    STL_DASSERT( aPropertyID < KNL_PROPERTY_ID_MAX );
    STL_DASSERT( aSessionEnv != NULL );
    
    return (stlInt64) aSessionEnv->mPropertyInfo.mPropertyValue[aPropertyID];
}


/**
 * @brief 해당 Property에 저장된 값을 반환한다.
 * @param[in]     aContent             knpPropertyContent
 * @param[out]    aValue               Property에 저장된 값
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_DATATYPE]
 * 해당 Property의 DataType이 정상적이지 않습니다.
 * - 해당 Property의 DataType을 확인합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_ID]
 * 해당 Property의 ID가 정상적이지 않습니다.
 * - 해당 Property의 ID를 확인합니다.
 * @endcode
 * @remarks
 * <BR> 해당 Property를 가져올 때는 ENV에서 가져올지, Shared Memory에서 가져올지 판단하고 가져오게 된다.
 */
stlStatus knpGetPropertyValue ( knpPropertyContent  * aContent,
                                void                * aValue,
                                knlEnv              * aKnlEnv )
{
    knlPropertyDataType    sType        = KNL_PROPERTY_TYPE_MAX;
    knlPropertyID          sPropertyID  = KNL_PROPERTY_ID_MAX;
    knlSessionEnv        * sSessionEnv  = NULL;
    
    stlBool     sLatchState = STL_FALSE;
    stlBool     sIsSuccess  = STL_FALSE;
    stlUInt32   sValueLen   = 0;
    stlChar     sTmpBuff[KNL_PROPERTY_STRING_MAX_LENGTH] = {0,};
    

    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue   != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sSessionEnv = KNL_SESS_ENV( aKnlEnv );
    STL_PARAM_VALIDATE( sSessionEnv  != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    sPropertyID = aContent->mID;
    
    STL_TRY_THROW( sPropertyID < KNL_PROPERTY_ID_MAX, RAMP_ERROR_INVALID_PROPERTYID );
    
    /**
     * DataType 얻어오기
     */
    STL_TRY( knpGetPropertyDataType( aContent,
                                     &sType,
                                     aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 어디서 가져올지를 선택한다.
     */ 
    if ( gKnpPropertyMgr->mDoCopyToSessionEnv[sPropertyID] == STL_TRUE )
    {
        /**
         * Session ENV 에서 가져온다 
         */
        switch( sType )
        {
            case KNL_PROPERTY_TYPE_BOOL:
            {
                *(stlBool*)aValue = (stlBool)sSessionEnv->mPropertyInfo.mPropertyValue[sPropertyID];
                break;
            }
            case KNL_PROPERTY_TYPE_BIGINT:
            {
                *(stlInt64*)aValue = sSessionEnv->mPropertyInfo.mPropertyValue[sPropertyID];
                break;
            }
            case KNL_PROPERTY_TYPE_VARCHAR:
            {
                knpParseVarcharPath( (stlChar*)sSessionEnv->mPropertyInfo.mPropertyValue[sPropertyID], 
                                     sTmpBuff,
                                     aKnlEnv );
                
                sValueLen = stlStrlen( sTmpBuff );
                
                STL_TRY_THROW( sValueLen < aContent->mMax, RAMP_ERROR_INVALID_VALUE );
                
                stlStrncpy( aValue,
                            sTmpBuff,
                            sValueLen + 1 );
                break;
            }
            case KNL_PROPERTY_TYPE_MAX:
            default:
            {
                STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
            }
        }
    }
    else
    {
        /**
         * Shared Memory || HEAP 에서 가져온다.
         */
        switch( sType )
        {
            case KNL_PROPERTY_TYPE_BOOL:
            {
                *(stlBool*)aValue = (stlBool)*(stlInt64*)aContent->mValue;
                break;
            }
            case KNL_PROPERTY_TYPE_BIGINT:
            {
                *(stlInt64*)aValue = *(stlInt64*)aContent->mValue;
                break;
            }
            case KNL_PROPERTY_TYPE_VARCHAR:
            {
                knpParseVarcharPath( aContent->mValue, 
                                     sTmpBuff,
                                     aKnlEnv );
                
                sValueLen = stlStrlen( sTmpBuff );
                
                STL_TRY_THROW( sValueLen < aContent->mMax, RAMP_ERROR_INVALID_VALUE );
                
                STL_TRY( knlAcquireLatch( &gKnpPropertyMgr->mLatch,
                                          KNL_LATCH_MODE_SHARED,
                                          KNL_LATCH_PRIORITY_NORMAL,
                                          STL_INFINITE_TIME,
                                          &sIsSuccess,
                                          aKnlEnv ) == STL_SUCCESS );
                sLatchState = STL_TRUE;
                
                stlStrncpy( aValue,
                            sTmpBuff,
                            sValueLen  + 1 );
                
                STL_TRY( knlReleaseLatch( &gKnpPropertyMgr->mLatch,
                                          aKnlEnv ) == STL_SUCCESS );
                sLatchState = STL_FALSE;

                break;
            }
            case KNL_PROPERTY_TYPE_MAX:
            default:
            {
                STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
            }
        }
        
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_INVALID_PROPERTYID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_ID,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERROR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_VALUE, 
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aContent->mName );
    }
    STL_FINISH;
    
    if( sLatchState == STL_TRUE )
    {
        (void) knlReleaseLatch( &gKnpPropertyMgr->mLatch,
                                aKnlEnv );
        sLatchState = STL_FALSE;
    }

    return STL_FAILURE;
}


/**
 * @brief Varchar DataType을 갖는 Property일 경우에 PATH를 파싱해야 한다.
 * @param[in,out] aValue               Varchar PropertyValue
 * @param[in,out] aRetValue               return Varchar PropertyValue
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * @endcode
 * @remarks
 * <BR> VARCHAR DataType을 갖는 Property의 경우에는 DB_HOME이 포함되었을 경우,
 * <BR> 환경 변수 값으로 파싱해야 한다.
 */
stlStatus knpParseVarcharPath ( stlChar   * aValue,
                                stlChar   * aRetValue,
                                knlEnv    * aKnlEnv )
{
    stlInt32   sValueLength = 0;
    stlInt32   sLength      = 0;
    stlBool    sFound       = STL_FALSE;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aValue != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    sValueLength = stlStrlen( aValue );
    sLength      = stlStrlen( KNP_HOME_DIRECTORY );
    
    if( sValueLength >= sLength )
    {
        if( stlStrncmp( aValue, KNP_HOME_DIRECTORY, sLength ) == 0 )
        {
            sFound = STL_TRUE;
            
            stlSnprintf( aRetValue, KNL_PROPERTY_STRING_MAX_LENGTH, "%s%s", 
                         gKnpPropertyMgr->mHomeDir, 
                         &aValue[sLength]);
        }
    }
    
    if( sFound == STL_FALSE )
    {
        stlStrncpy( aRetValue,
                    aValue,
                    stlStrlen( aValue ) + 1 );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 해당 Property가 Binary Property에 저장된 값을 반환한다.
 * @param[in]     aContent             knpPropertyContent
 * @param[out]    aValue               Binary Property에 저장된 값
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_DATATYPE]
 * 해당 Property의 DataType이 정상적이지 않습니다.
 * - 해당 Property의 DataType을 확인합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_STARTUP_PHASE]
 * 해당 프로퍼티를 설정할 수 있는 StartupPhase가 아닙니다.
 * - 해당 프로퍼티를 설정할 수 있는 StartupPhase에서 수행해야 합니다.
 * @endcode
 * @remarks
 * <BR> FileValue는 File에 저장된 내용을 읽어서 가져오는 것이 아니라, File에 쓰기전에 저장하는 mFileValue를 가져오는 것이다.
 */

stlStatus knpGetPropertyFileValue ( knpPropertyContent  * aContent,
                                    void                * aValue,
                                    knlEnv              * aKnlEnv )
{
    knlPropertyDataType sDataType  = KNL_PROPERTY_TYPE_MAX;
    
    stlBool     sLatchState = STL_FALSE;
    stlBool     sIsSuccess  = STL_FALSE;
    stlChar     sTmpBuff[KNL_PROPERTY_STRING_MAX_LENGTH] = {0,};
    stlUInt32   sValueLen    = 0;

    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue   != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    STL_TRY_THROW( gKnEntryPoint->mStartupPhase >= KNL_STARTUP_PHASE_NO_MOUNT,
                   RAMP_ERR_INVALID_STARTUP_PHASE );
    
    /** 
     * File에 저장된 내용을 저장하는 mFileValue에 값이 지정되지 않았을 경우에 mHasFileValue는 STL_FALSE가 된다.
     * 즉, 값이 없을 경우에는 해당 함수는 STL_FAILURE를 리턴한다. ('0' 또는 NULL을 리턴하지 않는다.)
     */
    STL_TRY( aContent->mHasFileValue == STL_TRUE );     //에러 처리를 해야하나?
    
    /**
     * DataType 얻어오기
     */
    STL_TRY( knpGetPropertyDataType( aContent,
                                     &sDataType,
                                     aKnlEnv ) == STL_SUCCESS );
    
    switch( sDataType )
    {
        case KNL_PROPERTY_TYPE_BOOL:
        {
            *(stlBool*)aValue = (stlBool)*(stlInt64*)aContent->mFileValue;
            break;
        }
        case KNL_PROPERTY_TYPE_BIGINT:
        {
            *(stlInt64*)aValue = *(stlInt64*)aContent->mFileValue;
            break;   
        }
        case KNL_PROPERTY_TYPE_VARCHAR:
        {
            knpParseVarcharPath( aContent->mFileValue, 
                                 sTmpBuff,
                                 aKnlEnv );
            
            sValueLen = stlStrlen( sTmpBuff );
            
            STL_TRY_THROW( sValueLen < aContent->mMax, RAMP_ERROR_INVALID_VALUE );
            
            STL_TRY( knlAcquireLatch( &gKnpPropertyMgr->mLatch,
                                      KNL_LATCH_MODE_SHARED,
                                      KNL_LATCH_PRIORITY_NORMAL,
                                      STL_INFINITE_TIME,
                                      &sIsSuccess,
                                      aKnlEnv ) == STL_SUCCESS );
            sLatchState = STL_TRUE;
            
            stlStrncpy( aValue,
                        sTmpBuff,
                        sValueLen + 1 );
            
            STL_TRY( knlReleaseLatch( &gKnpPropertyMgr->mLatch,
                                      aKnlEnv ) == STL_SUCCESS );
            sLatchState = STL_FALSE;
            break;
        }
        case KNL_PROPERTY_TYPE_MAX:
        default:
        {
            STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
        }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERR_INVALID_STARTUP_PHASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_STARTUP_PHASE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aContent->mName );
    }
    STL_CATCH( RAMP_ERROR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_VALUE, 
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aContent->mName );
    }
    STL_FINISH;

    if( sLatchState == STL_TRUE )
    {
        (void) knlReleaseLatch( &gKnpPropertyMgr->mLatch,
                                aKnlEnv );
        sLatchState = STL_FALSE;
    }
    
    return STL_FAILURE;
}


/**
 * @brief 해당 Property에 저장된 Init 값을 반환한다.
 * @param[in]     aContent             knpPropertyContent
 * @param[out]    aValue               Property에 저장된 Init 값
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_DATATYPE]
 * 해당 Property의 DataType이 정상적이지 않습니다.
 * - 해당 Property의 DataType을 확인합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_ID]
 * 해당 Property의 ID가 정상적이지 않습니다.
 * - 해당 Property의 ID를 확인합니다.
 * @endcode
 * @remarks
 * <BR> 해당 Property를 가져올 때는 ENV에서 가져올지, Shared Memory에서 가져올지 판단하고 가져오게 된다.
 */

stlStatus knpGetPropertyInitValue ( knpPropertyContent  * aContent,
                                    void                * aValue,
                                    knlEnv              * aKnlEnv )
{
    knlPropertyDataType    sType        = KNL_PROPERTY_TYPE_MAX;
    knlPropertyID          sPropertyID  = KNL_PROPERTY_ID_MAX;
    knlSessionEnv        * sSessionEnv;
    
    stlUInt32   sValueLen = 0;
    stlChar     sTmpBuff[KNL_PROPERTY_STRING_MAX_LENGTH] = {0,};
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue   != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sSessionEnv = KNL_SESS_ENV( aKnlEnv );
    STL_PARAM_VALIDATE( sSessionEnv  != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    sPropertyID = aContent->mID;
    
    STL_TRY_THROW( sPropertyID < KNL_PROPERTY_ID_MAX, RAMP_ERROR_INVALID_PROPERTYID );
    
    /**
     * DataType 얻어오기
     */
    STL_TRY( knpGetPropertyDataType( aContent,
                                     &sType,
                                     aKnlEnv ) == STL_SUCCESS );    
    
    /**
     * 어디서 가져올지를 선택한다.
     */ 
    if ( gKnpPropertyMgr->mDoCopyToSessionEnv[sPropertyID] == STL_TRUE )
    {
        /**
         * ENV 에서 가져온다 : Size = knlEnv .. stlInt64 mPropertyValue
         */
        switch( sType )
        {
            case KNL_PROPERTY_TYPE_BOOL:
            {
                *(stlBool*)aValue = (stlBool)sSessionEnv->mPropertyInfo.mPropertyInitValue[sPropertyID];
                break;
            }
            case KNL_PROPERTY_TYPE_BIGINT:
            {
                *(stlInt64*)aValue = sSessionEnv->mPropertyInfo.mPropertyInitValue[sPropertyID];
                break;
            }
            case KNL_PROPERTY_TYPE_VARCHAR:
            {
                
                knpParseVarcharPath( (stlChar*)sSessionEnv->mPropertyInfo.mPropertyValue[sPropertyID] + aContent->mMax, 
                                     sTmpBuff,
                                     aKnlEnv );
                
                sValueLen = stlStrlen( sTmpBuff );
                
                STL_TRY_THROW( sValueLen < aContent->mMax, RAMP_ERROR_INVALID_VALUE );
                
                stlStrncpy( aValue,
                            sTmpBuff,
                            sValueLen + 1 );
                break;   
            }
            case KNL_PROPERTY_TYPE_MAX:
            default:
            {
                STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
            }
        }
    }
    else
    {
        /**
         * Shared Memory || HEAP 에서 가져온다.
         * Boolean 4Byte
         * Number  8Byte
         */
        switch( sType )
        {
            case KNL_PROPERTY_TYPE_BOOL:
            {
                *(stlBool*)aValue = (stlBool)*(stlInt64*)aContent->mInitValue;
                break;
            }
            case KNL_PROPERTY_TYPE_BIGINT:
            {
                *(stlInt64*)aValue = *(stlInt64*)aContent->mInitValue;
                break;   
            }
            case KNL_PROPERTY_TYPE_VARCHAR:
            {
                knpParseVarcharPath( aContent->mInitValue, 
                                     sTmpBuff,
                                     aKnlEnv );
                
                sValueLen = stlStrlen( sTmpBuff );
                
                /**
                 * Initialize 과정에서 세팅되고 변경되지 않으므로 동시성 제어가 필요가 없다.
                 */
                
                stlStrncpy( aValue,
                            sTmpBuff,
                            sValueLen  + 1 );
                break;
            }
            case KNL_PROPERTY_TYPE_MAX:
            default:
            {
                STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
            }
        }
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_INVALID_PROPERTYID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_ID,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERROR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_VALUE, 
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aContent->mName );
    }
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Property ID로 해당 Property의 이름을 반환한다.
 * @param[in]     aPropertyName        Property Name
 * @param[out]    aPropertyID          Property ID
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_ID]
 * 해당 Property의 ID가 정상적이지 않습니다.
 * - 해당 Property의 ID를 확인합니다.
 * @endcode
 * @remarks
 */

stlStatus knpGetPropertyIDByName( stlChar        * aPropertyName,
                                  knlPropertyID  * aPropertyID, 
                                  knlEnv         * aKnlEnv )
{
    knpPropertyContent     * sContent = NULL;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aPropertyID   != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aPropertyName != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /**
     * knpPropertyContent 얻어오기
     */
    
    STL_TRY( knpGetPropertyHandleByName( aPropertyName,
                                         (void**)&sContent,
                                         aKnlEnv ) == STL_SUCCESS );
    
    STL_TRY_THROW( sContent->mID < KNL_PROPERTY_ID_MAX, RAMP_ERROR_INVALID_PROPERTYID );
    
    *aPropertyID = sContent->mID;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_INVALID_PROPERTYID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_ID,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_FINISH;
    
    *aPropertyID = KNL_PROPERTY_ID_MAX;

    return STL_FAILURE;    
    
}


/**
 * @brief 각 Process별로 사용되고 있는 ENV의 값을 변경한다.
 * @param[in]     aContent             knpPropertyContent
 * @param[in]     aMode                knlPropertyModifyMode
 * @param[in]     aIsDeferred          DEFERRED 옵션
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_DATATYPE]
 * 해당 Property의 DataType이 정상적이지 않습니다.
 * - 해당 Property의 DataType을 확인합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_ID]
 * 해당 Property의 ID가 정상적이지 않습니다.
 * - 해당 Property의 ID를 확인합니다.
 * @endcode
 * @remarks
 * <BR> ALTER SYSTEM ... immediate 속성을 갖는 Property의 경우에는 변경에 대한 영향이 즉시 이루어지므로, 
 * <BR> 각 Process에서 사용되고 있는 ENV에 저장된 PropertyValue를 변경해 줘야 한다.
 * <BR> 이와 함께, SET parameter_name TO DEFAULT 수행시에도 변경된 값으로 초기화 되어야 함으로, ENV의 InitValue도 함께 변경해 줘야한다.
 */

stlStatus knpUpdateSessionEnv( knpPropertyContent     * aContent,
                               knlPropertyModifyMode    aMode,
                               stlBool                  aIsDeferred,
                               knlEnv                 * aKnlEnv )
{
    knlSessionEnv         * sSessionEnv     = NULL;
    knlPropertyID           sPropertyID     = KNL_PROPERTY_ID_MAX;
    knlPropertyModifyType   sPropertySysMod = KNL_PROPERTY_SYS_TYPE_MAX;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent  != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sSessionEnv = KNL_SESS_ENV( aKnlEnv );
    STL_PARAM_VALIDATE( sSessionEnv  != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    sPropertyID = aContent->mID;
    
    STL_TRY_THROW( sPropertyID < KNL_PROPERTY_ID_MAX, RAMP_ERROR_INVALID_PROPERTYID );
    
    STL_TRY( knpGetPropertySysModType( aContent,
                                       &sPropertySysMod,
                                       aKnlEnv ) == STL_SUCCESS );
        
    /**
     * Session Env에 저장되는 값이고, ALTER SYSTEM... IMMEIDATE 속성일 경우에 다른 Session Env를 변경해 줘야 한다.
     * Immediate라 하더라도 DEFERRED 옵션을 주었을 경우에는 다른 Session의 Property값을 변경하지 않는다.
     * KNL_PROPERTY_SYS_TYPE_DEFERRED 속성은 항상 DEFERRED 옵션을 줘야 한다.
     */
    if ( ( gKnpPropertyMgr->mDoCopyToSessionEnv[sPropertyID] == STL_TRUE ) && 
         ( aMode == KNL_PROPERTY_MODE_SYS ) && 
         ( sPropertySysMod == KNL_PROPERTY_SYS_TYPE_IMMEDIATE ) )
    {
        if ( aIsDeferred == STL_FALSE )
        {
            /**
             * 각 Session별 Session Env 돌면서 변경!!    
             */ 
            for( sSessionEnv = (knlSessionEnv*)knlGetFirstSessionEnv();
                 sSessionEnv != NULL;
                 sSessionEnv = (knlSessionEnv*)knlGetNextSessionEnv( (void*)sSessionEnv ) )
            {
                switch ( aContent->mDataType )
                {
                    case KNL_PROPERTY_TYPE_BOOL:
                    case KNL_PROPERTY_TYPE_BIGINT:
                    {
                        sSessionEnv->mPropertyInfo.mPropertyValue[sPropertyID] =  *(stlInt64*)aContent->mValue;
                        
                        /**
                         * Alter System .. immediate 속성일 경우에는 SET parameter_name TO DEFAULT 시에는 최근의 데이터로 변경해야만 한다.
                         * INIT VALUE는 바꾸지 않는다.... InitValue는 Session이 생성될 경우의 값이기 때문에 바꾸지 않는다.
                         */
                        //sSessionEnv->mPropertyInfo.mPropertyInitValue[sPropertyID] =  *(stlInt64*)aContent->mValue;
                        sSessionEnv->mPropertyInfo.mModified[sPropertyID] = STL_FALSE;
                        break;
                    }
                    case KNL_PROPERTY_TYPE_VARCHAR:
                    {
                        /**
                         * VARCHAR PROPERTY는 IMMEIDATE가 될 수 없다.
                         */
                        STL_DASSERT( STL_FALSE );
                        break;
                    }
                    case KNL_PROPERTY_TYPE_MAX:
                    default:
                    {   
                        STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
                    }
                }// end switch
            }// end for .. loop
        }// end if
        
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERROR_INVALID_PROPERTYID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_ID,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;        
}


/**
 * @brief 각 Process별로 사용되고 있는 ENV의 값을 변경한다.
 * @param[in]     aContent             knpPropertyContent
 * @param[in]     aMode                knlPropertyModifyMode
 * @param[in]     aIsDeferred          DEFERRED 옵션
 * @param[in]     aScope               해당 프로퍼티 값의 적용 범위(Memory, File, Both)
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_DATATYPE]
 * 해당 Property의 DataType이 정상적이지 않습니다.
 * - 해당 Property의 DataType을 확인합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_ID]
 * 해당 Property의 ID가 정상적이지 않습니다.
 * - 해당 Property의 ID를 확인합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_MODE]
 * Property 변경 Mode가 정확하지 않습니다.
 * - 변경 Mode를 확인한다.
 * @endcode
 * @remarks
 * <BR> ALTER SYSTEM ... immediate 속성을 갖는 Property의 경우에는 변경에 대한 영향이 즉시 이루어지므로, 
 * <BR> 각 Process에서 사용되고 있는 ENV에 저장된 PropertyValue를 변경해 줘야 한다.
 * <BR> 이와 함께, SET Parameter_name TO DEFAULT 수행시에도 변경된 값으로 초기화 되어야 함으로, ENV의 InitValue도 함께 변경해 줘야한다.
 */

stlStatus knpInitPropertyValue( knpPropertyContent     * aContent,
                                knlPropertyModifyMode    aMode,
                                stlBool                  aIsDeferred,
                                knlPropertyModifyScope   aScope,
                                knlEnv                 * aKnlEnv )
{
    knlPropertyID         sPropertyID = KNL_PROPERTY_ID_MAX;
    knlPropertyDataType   sType       = KNL_PROPERTY_TYPE_MAX;
    knlSessionEnv       * sSessionEnv = NULL;
    
    stlBool     sLatchState = STL_FALSE;
    stlBool     sIsSuccess  = STL_FALSE;

    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent  != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sSessionEnv = KNL_SESS_ENV( aKnlEnv );
    STL_PARAM_VALIDATE( sSessionEnv  != NULL, KNL_ERROR_STACK(aKnlEnv) );
        
    /**
     * Verify Attribute
     */
    STL_TRY( knpVerifyPropertyAttribute( aContent, 
                                         aMode, 
                                         aIsDeferred,
                                         aScope,
                                         aKnlEnv ) == STL_SUCCESS );
    
    sPropertyID = aContent->mID;
    STL_TRY_THROW( sPropertyID < KNL_PROPERTY_ID_MAX, RAMP_ERROR_INVALID_PROPERTYID );

    STL_TRY( knpGetPropertyDataType( aContent,
                                     &sType,
                                     aKnlEnv ) == STL_SUCCESS );
    
    /**
     * SCOPE 처리
     */ 
    if( ( aScope == KNL_PROPERTY_SCOPE_MEMORY ) ||
        ( aScope == KNL_PROPERTY_SCOPE_BOTH ) )
    {
        /**
        * Memory에 해당 Property에 값을 추가한다.
        */
        if ( aMode == KNL_PROPERTY_MODE_SES )
        {
            /**
             * ALTER SESSION SET parameter_name TO DEFAULT 일 경우에는 ENV에 저장된 InitValue를 사용해서 초기화 해야 함.
             * 기존에 ALTER SYSTEM .. immediate 속성을 갖는 Property를 변경할 경우에 ENV의 InitValue도 함께 변경하므로 
             * 무조건 ENV의 InitValue를 사용하면 됨.
             */
            switch ( sType )
            {
                case KNL_PROPERTY_TYPE_BOOL:
                case KNL_PROPERTY_TYPE_BIGINT:
                {
                    sSessionEnv->mPropertyInfo.mPropertyValue[sPropertyID] = sSessionEnv->mPropertyInfo.mPropertyInitValue[sPropertyID];
                
                    //초기화 되었으므로 Modified는 FALSE가 된다.
                    sSessionEnv->mPropertyInfo.mModified[sPropertyID] = STL_FALSE;
                    break;
                }
                case KNL_PROPERTY_TYPE_VARCHAR:      
                {
                    stlStrncpy( (stlChar*)sSessionEnv->mPropertyInfo.mPropertyValue[sPropertyID],
                                (stlChar*)sSessionEnv->mPropertyInfo.mPropertyValue[sPropertyID] + aContent-> mMax,     //Initvalue의 Offset
                                stlStrlen( (stlChar*)sSessionEnv->mPropertyInfo.mPropertyValue[sPropertyID] + aContent-> mMax ) + 1 );
                    
                    //초기화 되었으므로 Modified는 FALSE가 된다.
                    sSessionEnv->mPropertyInfo.mModified[sPropertyID] = STL_FALSE;
                    break;   
                }
                case KNL_PROPERTY_TYPE_MAX:
                default:
                {
                    STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
                    break;
                }
            }
        }
        else if ( aMode == KNL_PROPERTY_MODE_SYS )
        {
            /**
             * ALTER SYSTEM SET parameter_name TO DEFAULT 일 경우에는 Shared-Memory에 저장된 InitValue로 Reset 해야 함.
             * IMMEIDATE가 아니면 자신의 SessionEnv값도 변경하지 않는다.
             */
            switch ( sType )
            {
                case KNL_PROPERTY_TYPE_BOOL:
                case KNL_PROPERTY_TYPE_BIGINT:
                {
                    *(stlInt64*)aContent->mValue = *(stlInt64*)aContent->mInitValue;
                    break;
                }
                case KNL_PROPERTY_TYPE_VARCHAR:
                {
                    /**
                     * VARCHAR Type은 IMMEDIATE가 없기 때문에 SessionEnv는 변경하지 않는다.
                     * BOOL, BIGINT는 knpUpdateSessionEnv 에서 변경한다.
                     */
                    STL_TRY( knlAcquireLatch( &gKnpPropertyMgr->mLatch,
                                              KNL_LATCH_MODE_EXCLUSIVE,
                                              KNL_LATCH_PRIORITY_NORMAL,
                                              STL_INFINITE_TIME,
                                              &sIsSuccess,
                                              aKnlEnv ) == STL_SUCCESS );
                    sLatchState = STL_TRUE;
                    
                    stlStrncpy( aContent->mValue, 
                                aContent->mInitValue,
                                aContent->mInitValueLength + 1 );
                
                    aContent->mValueLength = aContent->mInitValueLength;
                    
                    STL_TRY( knlReleaseLatch( &gKnpPropertyMgr->mLatch,
                                      aKnlEnv ) == STL_SUCCESS );
                    sLatchState = STL_FALSE;
                    break;
                }
                case KNL_PROPERTY_TYPE_MAX:
                default:
                {
                    STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
                    break;
                }
            }
        
            /**
             * ENV 반영이 필요할 경우에는 반영해야한다.
             */
            STL_TRY( knpUpdateSessionEnv( aContent,
                                          aMode,
                                          aIsDeferred,
                                          aKnlEnv ) == STL_SUCCESS );
        }
    }
    
    if( ( ( aScope == KNL_PROPERTY_SCOPE_FILE ) ||
          ( aScope == KNL_PROPERTY_SCOPE_BOTH ) ) &&
          ( aMode == KNL_PROPERTY_MODE_SYS ) )
    {
        STL_TRY( knpAddPropertyFileValue( aContent,
                                          aContent->mInitValue,
                                          aMode,
                                          aKnlEnv ) == STL_SUCCESS );
        
        STL_TRY( knpAddPropertyValueAtFile( aContent,
                                            aContent->mFileValue,
                                            aKnlEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERROR_INVALID_PROPERTYID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_ID,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_FINISH;
    
    if( sLatchState == STL_TRUE )
    {
        (void) knlReleaseLatch( &gKnpPropertyMgr->mLatch,
                                aKnlEnv );
        sLatchState = STL_FALSE;
    }

    return STL_FAILURE;        
    
}


/**
 * @brief Property 이름으로 해당 Property의 ID을 반환한다.
 * @param[in]    aPropertyID          Property ID
 * @param[out]     aPropertyName        Property Name
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_ID]
 * 해당 Property의 ID가 정상적이지 않습니다.
 * - 해당 Property의 ID를 확인합니다.
 * [KNL_ERRCODE_PROPERTY_NOT_EXIST]
 * 입력한 Property ID와 동일한 Property가 없다.
 * - Property ID를 확인한다.
 * @endcode
 * @remarks
 */

stlStatus knpGetPropertyNameByID ( knlPropertyID    aPropertyID,
                                   stlChar        * aPropertyName,
                                   knlEnv         * aKnlEnv )
{
    knpPropertyContent  * sContent = NULL;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aPropertyName != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    STL_TRY_THROW( aPropertyID < KNL_PROPERTY_ID_MAX, RAMP_ERROR_INVALID_PROPERTYID );
    
    sContent = gKnpPropertyMgr->mPropertyAddr[aPropertyID];
    
    STL_TRY_THROW( sContent != NULL, RAMP_ERR_NOT_EXIST );
    
    stlStrncpy( aPropertyName,
                sContent->mName,
                stlStrlen( sContent->mName ) + 1 );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_INVALID_PROPERTYID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_ID,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    } 
    STL_CATCH( RAMP_ERR_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aPropertyName );
    }   
    STL_FINISH;
    
    return STL_FAILURE;     
}


/**
 * @brief Property ID으로 해당 Property의 Handle을 반환한다.
 * @param[in]     aPropertyID          Property ID
 * @param[out]    aPropertyHandle      PropertyHandle
 * @param[in,out] aKnlEnv              Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_ID]
 * 해당 Property의 ID가 정상적이지 않습니다.
 * - 해당 Property의 ID를 확인합니다.
 * [KNL_ERRCODE_PROPERTY_NOT_EXIST]
 * 입력한 Property ID와 동일한 Property가 없다.
 * - Property ID를 확인한다.
 * @endcode
 * @remarks
 */

stlStatus knpGetPropertyHandleByID( knlPropertyID     aPropertyID, 
                                    void           ** aPropertyHandle, 
                                    knlEnv          * aKnlEnv )
{
    knpPropertyContent   * sContent = NULL;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( *aPropertyHandle == NULL, KNL_ERROR_STACK(aKnlEnv) );

    STL_TRY_THROW( aPropertyID < KNL_PROPERTY_ID_MAX, RAMP_ERROR_INVALID_PROPERTYID );

    sContent = gKnpPropertyMgr->mPropertyAddr[aPropertyID];

    STL_TRY_THROW( sContent != NULL, RAMP_ERROR_INVALID_PROPERTYID );
    
    *aPropertyHandle = sContent;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_INVALID_PROPERTYID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_ID,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }   

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * X$PROPERTY Fixed Table 정의
 */

knlFxColumnDesc gPropertyColumnDesc[] =
{
    {
        "PROPERTY_ID",
        "Property id (unique)",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( knlPropertyFxRecord, mID ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "PROPERTY_NAME",
        "Property name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlPropertyFxRecord, mName ),
        KNL_PROPERTY_NAME_MAX_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "DESCRIPTION",
        "Property description",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlPropertyFxRecord, mDesc ),
        KNL_PROPERTY_STRING_MAX_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "GROUP_ID",
        "Property group",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlPropertyFxRecord, mGroup ),
        KNL_PROPERTY_FXTABLE_BUFFER_SIZE,
        STL_FALSE  /* nullable */
    },
    {
        "DATA_TYPE_ID",
        "data type identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knlPropertyFxRecord, mDataTypeID ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "DATA_TYPE",
        "Property Data type",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlPropertyFxRecord, mDataType ),
        KNL_PROPERTY_FXTABLE_BUFFER_SIZE,
        STL_FALSE  /* nullable */
    },
    {
        "STARTUP_PHASE",
        "Property modifiable startup-phase",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlPropertyFxRecord, mStartupPhase ),
        KNL_PROPERTY_FXTABLE_BUFFER_SIZE,
        STL_FALSE  /* nullable */
    },
    {
        "UNIT",
        "Property Unit Type",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlPropertyFxRecord, mUnitType ),
        KNL_PROPERTY_FXTABLE_BUFFER_SIZE,
        STL_TRUE  /* nullable */
    },
    {
        "VALUE",
        "Property Value",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlPropertyFxRecord, mValue ),
        KNL_PROPERTY_STRING_MAX_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "SOURCE",
        "Property Value Source",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlPropertyFxRecord, mSource ),
        KNL_PROPERTY_FXTABLE_BUFFER_SIZE,
        STL_FALSE  /* nullable */
    },
    {
        "INIT_VALUE",
        "Property Initialization Value",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlPropertyFxRecord, mInitValue ),
        KNL_PROPERTY_STRING_MAX_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "INIT_SOURCE",
        "Property Initialization Value Source",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlPropertyFxRecord, mInitSource ),
        KNL_PROPERTY_FXTABLE_BUFFER_SIZE,
        STL_FALSE  /* nullable */
    },
    {
        "SES_MODIFIABLE",
        "ALTER SESSION modifiable",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlPropertyFxRecord, mSesModifiable ),
        KNL_PROPERTY_FXTABLE_BUFFER_SIZE,
        STL_FALSE  /* nullable */
    },
    {
        "SYS_MODIFIABLE",
        "ALTER SESSION modifiable",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlPropertyFxRecord, mSysModifiable ),
        KNL_PROPERTY_FXTABLE_BUFFER_SIZE,
        STL_FALSE  /* nullable */
    },
    {
        "MIN",
        "Property min value",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knlPropertyFxRecord, mMin ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "MAX",
        "Property Max Value",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knlPropertyFxRecord, mMax ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "MODIFIABLE",
        "Property Modifiable",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlPropertyFxRecord, mModifiable ),
        KNL_PROPERTY_FXTABLE_BUFFER_SIZE,
        STL_FALSE  /* nullable */
    },
    {
        "DOMAIN",
        "Internal or External Property",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knlPropertyFxRecord, mInternal ),
        KNL_PROPERTY_FXTABLE_BUFFER_SIZE,
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};


stlStatus openFxPropertyCallback( void   * aStmt,
                                  void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv )
{
    /**
     * PROPERTY ID의 시작을 준다.
     */
    stlInt16 sPropertyID = 0;
    stlMemcpy( aPathCtrl, &sPropertyID, STL_SIZEOF(stlInt16) );

    return STL_SUCCESS;
}

stlStatus closeFxPropertyCallback( void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv )
{
    /**
     * Nothing To Do.
     */
    return STL_SUCCESS;
}

stlStatus buildRecordFxPropertyCallback( void              * aDumpObjHandle,
                                         void              * aPathCtrl,
                                         knlValueBlockList * aValueList,
                                         stlInt32            aBlockIdx,
                                         stlBool           * aTupleExist,
                                         knlEnv            * aKnlEnv )
{
    stlInt16    sPropertyID = 0;
    
    knpPropertyContent     * sContent     = NULL;
    stlBool                  sMode        = STL_FALSE;
    stlBool                  sModifiable  = STL_FALSE;
    knlPropertyDomain        sDomain      = KNL_PROPERTY_DOMAIN_MAX;
    knlPropertyModifyType    sType        = KNL_PROPERTY_SYS_TYPE_MAX;
    knlPropertyDataType      sDataType    = KNL_PROPERTY_TYPE_MAX;
    knlPropertyBigintUnit    sUnitType    = KNL_PROPERTY_BIGINT_UNIT_MAX;
    knlPropertyFxRecord      sPropertyFxRec;
    
    stlBool                  sBoolValue = STL_FALSE;
    stlInt64                 sBigIntValue = 0;
    stlChar                  sStringValue[KNL_PROPERTY_STRING_MAX_LENGTH] = {0};
    knlSessionEnv          * sSessionEnv;

    stlMemcpy( &sPropertyID, aPathCtrl, STL_SIZEOF(sPropertyID) );
    
    sSessionEnv = KNL_SESS_ENV( aKnlEnv );
    
    while( 1 )
    {
        sContent = NULL;
        
        if( sPropertyID == KNL_PROPERTY_ID_MAX )
        {
            *aTupleExist = STL_FALSE;
            break;
        }
        else
        {
            STL_TRY( knpGetPropertyHandleByID ( sPropertyID,
                                                (void**)&sContent,
                                                aKnlEnv ) == STL_SUCCESS );
            
            /**
             * DOMAIN_HIDDEN을 처리하기 위해 Domain을 먼저 검색
             */
            STL_TRY( knpGetPropertyDomain( sContent, 
                                           &sDomain, 
                                           aKnlEnv ) == STL_SUCCESS );
            
            /** INTERNAL/EXTERNAL 
             *  HIDDEN 일 경우에 보여주지 않는다. SKIP하도록 한다.
             */
            STL_TRY( knpGetPropertyDomain( sContent, 
                                           &sDomain, 
                                           aKnlEnv ) == STL_SUCCESS );
        
            if( sDomain == KNL_PROPERTY_DOMAIN_HIDDEN )
            {
                /**
                 * NEXT PROPERTY
                 */ 
                sPropertyID++;
                continue;
            }
            else if ( sDomain == KNL_PROPERTY_DOMAIN_INTERNAL )
            {
                stlSnprintf( sPropertyFxRec.mInternal, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                             "INTERNAL");
            }
            else
            {
                stlSnprintf( sPropertyFxRec.mInternal, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                             "EXTERNAL");
            }
            
            break;
        }
    }

    /**
     * Record 구성
     */
    if ( sContent != NULL )
    {
        /** Get Property Modifiable */
        STL_TRY( knpGetPropertyModifiable( sContent, &sModifiable, aKnlEnv ) == STL_SUCCESS );

        if ( sModifiable == STL_TRUE )
        {
            stlSnprintf( sPropertyFxRec.mModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                         "TRUE");
        }
        else
        {
            stlSnprintf( sPropertyFxRec.mModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                         "FALSE");    
        }
    
        /** Get ALTER SESSION Modifable */
        STL_TRY( knpGetPropertySesModType( sContent, &sMode, aKnlEnv ) == STL_SUCCESS );

        if( sMode == STL_TRUE )
        {
            stlSnprintf( sPropertyFxRec.mSesModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                        "TRUE");
        }
        else
        {
            stlSnprintf( sPropertyFxRec.mSesModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                        "FALSE");
        }
        
        /** Get ALTER SYSTEM Modify Mode */
        STL_TRY( knpGetPropertySysModType( sContent, &sType, aKnlEnv ) == STL_SUCCESS );
        
        /** ALTER SYSTEM Modifable Type */
        switch( sType )
        {
            case KNL_PROPERTY_SYS_TYPE_IMMEDIATE:
            {
                stlSnprintf( sPropertyFxRec.mSysModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "IMMEDIATE");
                break;
            }
            case KNL_PROPERTY_SYS_TYPE_DEFERRED:
            {
                stlSnprintf( sPropertyFxRec.mSysModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "DEFERRED");
                break;
            }
            case KNL_PROPERTY_SYS_TYPE_FALSE:
            {
                stlSnprintf( sPropertyFxRec.mSysModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "FALSE");
                break;
            }
            case KNL_PROPERTY_SYS_TYPE_NONE:
            {
                stlSnprintf( sPropertyFxRec.mSysModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "NONE");
                break;
            }
            default:
                break;
        }
        
        /** Get DataType */
        STL_TRY( knpGetPropertyDataType( sContent, &sDataType, aKnlEnv ) == STL_SUCCESS );

        /** DataType */        
        switch( sDataType )
        {
            case KNL_PROPERTY_TYPE_BOOL:
            {
                /** Boolean DataType */
                sPropertyFxRec.mDataTypeID = DTL_TYPE_BOOLEAN;
                stlSnprintf( sPropertyFxRec.mDataType, 
                             KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "BOOLEAN");
                
                /** Boolean VALUE */
                STL_TRY ( knpGetPropertyValue( sContent, &sBoolValue, aKnlEnv ) == STL_SUCCESS );
                
                if ( sBoolValue == STL_TRUE )
                {
                    stlSnprintf( sPropertyFxRec.mValue, KNL_PROPERTY_STRING_MAX_LENGTH, 
                                 "YES");
                }
                else
                {
                    stlSnprintf( sPropertyFxRec.mValue, KNL_PROPERTY_STRING_MAX_LENGTH, 
                                 "NO");
                }
                
                /** Boolean Init Value */
                STL_TRY ( knpGetPropertyInitValue( sContent, &sBoolValue, aKnlEnv ) == STL_SUCCESS );
                
                if ( sBoolValue == STL_TRUE )
                {
                    stlSnprintf( sPropertyFxRec.mInitValue, KNL_PROPERTY_STRING_MAX_LENGTH, 
                                 "YES");
                }
                else
                {
                    stlSnprintf( sPropertyFxRec.mInitValue, KNL_PROPERTY_STRING_MAX_LENGTH, 
                                 "NO");
                }
                break;
            }
            case KNL_PROPERTY_TYPE_BIGINT:
            {
                /** BIGINT DataType */
                sPropertyFxRec.mDataTypeID = DTL_TYPE_NATIVE_BIGINT;
                stlSnprintf( sPropertyFxRec.mDataType, 
                             KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "BIGINT");
                
                /** BIGINT VALUE */
                STL_TRY ( knpGetPropertyValue( sContent, &sBigIntValue, aKnlEnv ) == STL_SUCCESS );
                
                stlSnprintf( sPropertyFxRec.mValue, KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "%ld", sBigIntValue);
                
                /** BIGINT Init Value */
                STL_TRY ( knpGetPropertyInitValue( sContent, &sBigIntValue, aKnlEnv ) == STL_SUCCESS );
                
                stlSnprintf( sPropertyFxRec.mInitValue, KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "%ld", sBigIntValue);
                break;
            }
            case KNL_PROPERTY_TYPE_VARCHAR:
            {
                /** String DataType */
                sPropertyFxRec.mDataTypeID = DTL_TYPE_VARCHAR;
                stlSnprintf( sPropertyFxRec.mDataType, 
                             KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "VARCHAR");
                
                /** STRING VALUE */
                STL_TRY ( knpGetPropertyValue( sContent, sStringValue, aKnlEnv ) == STL_SUCCESS );
                stlStrncpy( sPropertyFxRec.mValue, sStringValue, stlStrlen( sStringValue ) + 1 );
                
                /** STRING Init Value */
                STL_TRY ( knpGetPropertyInitValue( sContent, &sStringValue, aKnlEnv ) == STL_SUCCESS );
                stlStrncpy( sPropertyFxRec.mInitValue, sStringValue, stlStrlen( sStringValue ) + 1 );
                break;
            }
            default:
                break;        
        }
        
        /** Get UnitType */    
        STL_TRY( knpGetPropertyBigIntUnit( sContent, &sUnitType, aKnlEnv ) == STL_SUCCESS );
        
        switch( sUnitType )
        {
            case KNL_PROPERTY_BIGINT_UNIT_NONE:
            {
                stlSnprintf( sPropertyFxRec.mUnitType, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "NONE");
                break;
            }
            case KNL_PROPERTY_BIGINT_UNIT_SIZE:
            {
                stlSnprintf( sPropertyFxRec.mUnitType, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "BYTE");
                break;
            }
            case KNL_PROPERTY_BIGINT_UNIT_TIME:
            {
                stlSnprintf( sPropertyFxRec.mUnitType, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "MS");
                break;
            }
            case KNL_PROPERTY_BIGINT_UNIT_TIMEZONE_OFFSET:
            {
                stlSnprintf( sPropertyFxRec.mUnitType, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "GMT_OFFSET");
                break;
            }
            default:
            {
                break;
            }
        }
            
        /**
         * Value Source  
         * ALTER SESSION으로 변경됐을 경우에는 Source를 USER로 한다.
         */
        if ( sSessionEnv->mPropertyInfo.mModified[sPropertyID] == STL_TRUE )
        {
            stlSnprintf( sPropertyFxRec.mSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                         "USER");
        }
        else
        {
            switch( sContent->mSource )
            {
                case KNP_SOURCE_DEFAULT:
                {
                    stlSnprintf( sPropertyFxRec.mSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                                "DEFAULT");
                    break;
                }
                case KNP_SOURCE_ENV:
                {
                    stlSnprintf( sPropertyFxRec.mSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                                "ENV_VAR");
                    break;
                }
                case KNP_SOURCE_BINARYFILE:
                {
                    stlSnprintf( sPropertyFxRec.mSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                                "BINARY_FILE");
                    break;
                }
                case KNP_SOURCE_TEXTFILE:
                {
                    stlSnprintf( sPropertyFxRec.mSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                                "FILE");
                    break;
                }
                case KNP_SOURCE_SYSTEM:
                {
                    stlSnprintf( sPropertyFxRec.mSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                                "SYSTEM");
                    break;
                }
                case KNP_SOURCE_USER:
                default:
                    break;
            }
        }
            
        /** Init Value Source */
        switch( sContent->mInitSource )
        {
            case KNP_SOURCE_DEFAULT:
            {
                stlSnprintf( sPropertyFxRec.mInitSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                             "DEFAULT");
                break;
            }
            case KNP_SOURCE_ENV:
            {
                stlSnprintf( sPropertyFxRec.mInitSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                             "ENV_VAR");
                break;
            }
            case KNP_SOURCE_BINARYFILE:
            {
                stlSnprintf( sPropertyFxRec.mInitSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                             "BINARY_FILE");
                break;
            }
            case KNP_SOURCE_TEXTFILE:
            {
                stlSnprintf( sPropertyFxRec.mInitSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                             "FILE");
                break;
            }
            case KNP_SOURCE_SYSTEM:
            {
                stlSnprintf( sPropertyFxRec.mInitSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                             "SYSTEM");
                break;
            }
            case KNP_SOURCE_USER:
            {
                stlSnprintf( sPropertyFxRec.mInitSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                             "USER");
                break;
            }
            default:
                break;
        }
        
        /** MIN/MAX */
        sPropertyFxRec.mMin = sContent->mMin;
        sPropertyFxRec.mMax = sContent->mMax;
                
        /** ETC Value */
        sPropertyFxRec.mID = sContent->mID;
        
        stlStrncpy( sPropertyFxRec.mName, sContent->mName, stlStrlen(sContent->mName) + 1 );
        stlStrncpy( sPropertyFxRec.mDesc, sContent->mDesc, stlStrlen(sContent->mDesc) + 1 );
        
        /** GROUP/STARTUP PHASE STRING */
        stlSnprintf( sPropertyFxRec.mGroup, KNL_PROPERTY_STRING_MAX_LENGTH, 
                     "%s", gPropertyGroupList[sContent->mGroup]);
        
        switch( sContent->mStartupPhaseDomain )
        {
            case KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW:
            {
                stlSnprintf( sPropertyFxRec.mStartupPhase, KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "%s BELOW", gPhaseString[sContent->mStartupPhase]);
                break;
            }
            case KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER:
            {
                stlSnprintf( sPropertyFxRec.mStartupPhase, KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "%s ABOVE", gPhaseString[sContent->mStartupPhase]);
                break;
            }
            case KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY:
            {
                stlSnprintf( sPropertyFxRec.mStartupPhase, KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "%s", gPhaseString[sContent->mStartupPhase]);
                break;
            }
            case KNP_PROPERTY_PHASE_DOMAIN_MAX:
            default:
                break;
        }
        
        /** Build Record */
        STL_TRY( knlBuildFxRecord( gPropertyColumnDesc,
                                   &sPropertyFxRec,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv ) == STL_SUCCESS );
         /**
          * NEXT PROPERTY
          */
        sPropertyID++;
        stlMemcpy( aPathCtrl, &sPropertyID, STL_SIZEOF(sPropertyID) );
        *aTupleExist = STL_TRUE;
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    *aTupleExist = STL_FALSE;

    return STL_FAILURE;
}



stlStatus buildRecordFxSPropertyCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aTupleExist,
                                          knlEnv            * aKnlEnv )
{
    stlInt16    sPropertyID = 0;
    
    knpPropertyContent     * sContent     = NULL;
    stlBool                  sMode        = STL_FALSE;
    stlBool                  sModifiable  = STL_FALSE;
    knlPropertyDomain        sDomain      = KNL_PROPERTY_DOMAIN_MAX;
    knlPropertyModifyType    sType        = KNL_PROPERTY_SYS_TYPE_MAX;
    knlPropertyDataType      sDataType    = KNL_PROPERTY_TYPE_MAX;
    knlPropertyBigintUnit    sUnitType    = KNL_PROPERTY_BIGINT_UNIT_MAX;
    knlPropertyFxRecord      sPropertyFxRec;
    
    stlBool                  sBoolValue = STL_FALSE;
    stlInt64                 sBigIntValue = 0;
    stlChar                  sStringValue[KNL_PROPERTY_STRING_MAX_LENGTH] = {0};

    stlMemcpy( &sPropertyID, aPathCtrl, STL_SIZEOF(sPropertyID) );
    
    if( sPropertyID == KNL_PROPERTY_ID_MAX )
    {
        *aTupleExist = STL_FALSE;
    }
    else
    {
        while( 1 )
        {
            sContent = NULL;
            
            STL_TRY( knpGetPropertyHandleByID ( sPropertyID,
                                                (void**)&sContent,
                                                aKnlEnv ) == STL_SUCCESS );
            
            /**
             * SCOPE = FILE로 변경한 Property만을 보여준다.
             */
            if( sContent->mHasFileValue == STL_TRUE )
            {
                break;
            }
            
            sPropertyID++;
            
            if( sPropertyID == KNL_PROPERTY_ID_MAX )
            {
                sContent     = NULL;
                *aTupleExist = STL_FALSE;
                break;
            }
        }
    }

    /**
     * Record 구성
     */
    if ( sContent != NULL )
    {
        /** Get Property Modifiable */
        STL_TRY( knpGetPropertyModifiable( sContent, &sModifiable, aKnlEnv ) == STL_SUCCESS );

        if ( sModifiable == STL_TRUE )
        {
            stlSnprintf( sPropertyFxRec.mModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                         "TRUE");
        }
        else
        {
            stlSnprintf( sPropertyFxRec.mModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                         "FALSE");    
        }
    
        /** Get ALTER SESSION Modifable */
        STL_TRY( knpGetPropertySesModType( sContent, &sMode, aKnlEnv ) == STL_SUCCESS );

        if( sMode == STL_TRUE )
        {
            stlSnprintf( sPropertyFxRec.mSesModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                        "TRUE");
        }
        else
        {
            stlSnprintf( sPropertyFxRec.mSesModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                        "FALSE");
        }
        
        /** Get ALTER SYSTEM Modify Mode */
        STL_TRY( knpGetPropertySysModType( sContent, &sType, aKnlEnv ) == STL_SUCCESS );
        
        /** ALTER SYSTEM Modifable Type */
        switch( sType )
        {
            case KNL_PROPERTY_SYS_TYPE_IMMEDIATE:
            {
                stlSnprintf( sPropertyFxRec.mSysModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "IMMEDIATE");
                break;
            }
            case KNL_PROPERTY_SYS_TYPE_DEFERRED:
            {
                stlSnprintf( sPropertyFxRec.mSysModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "DEFERRED");
                break;
            }
            case KNL_PROPERTY_SYS_TYPE_FALSE:
            {
                stlSnprintf( sPropertyFxRec.mSysModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "FALSE");
                break;
            }
            case KNL_PROPERTY_SYS_TYPE_NONE:
            {
                stlSnprintf( sPropertyFxRec.mSysModifiable, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "NONE");
                break;
            }
            default:
                break;
        }
        
        /** Get DataType */
        STL_TRY( knpGetPropertyDataType( sContent, &sDataType, aKnlEnv ) == STL_SUCCESS );

        /** DataType */        
        switch( sDataType )
        {
            case KNL_PROPERTY_TYPE_BOOL:
            {
                /** Boolean DataType */
                sPropertyFxRec.mDataTypeID = DTL_TYPE_BOOLEAN;
                stlSnprintf( sPropertyFxRec.mDataType, 
                             KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "BOOLEAN");
                
                /** Boolean VALUE */
                STL_TRY ( knpGetPropertyFileValue( sContent, &sBoolValue, aKnlEnv ) == STL_SUCCESS );
                
                if ( sBoolValue == STL_TRUE )
                {
                    stlSnprintf( sPropertyFxRec.mValue, KNL_PROPERTY_STRING_MAX_LENGTH, 
                                 "YES");
                }
                else
                {
                    stlSnprintf( sPropertyFxRec.mValue, KNL_PROPERTY_STRING_MAX_LENGTH, 
                                 "NO");
                }
                
                /** Boolean Init Value */
                STL_TRY ( knpGetPropertyInitValue( sContent, &sBoolValue, aKnlEnv ) == STL_SUCCESS );
                
                if ( sBoolValue == STL_TRUE )
                {
                    stlSnprintf( sPropertyFxRec.mInitValue, KNL_PROPERTY_STRING_MAX_LENGTH, 
                                 "YES");
                }
                else
                {
                    stlSnprintf( sPropertyFxRec.mInitValue, KNL_PROPERTY_STRING_MAX_LENGTH, 
                                 "NO");
                }
                break;
            }
            case KNL_PROPERTY_TYPE_BIGINT:
            {
                /** BIGINT DataType */
                sPropertyFxRec.mDataTypeID = DTL_TYPE_NATIVE_BIGINT;
                stlSnprintf( sPropertyFxRec.mDataType, 
                             KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "BIGINT");
                
                /** BIGINT VALUE */
                STL_TRY ( knpGetPropertyFileValue( sContent, &sBigIntValue, aKnlEnv ) == STL_SUCCESS );
                
                stlSnprintf( sPropertyFxRec.mValue, KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "%ld", sBigIntValue);
                
                /** BIGINT Init Value */
                STL_TRY ( knpGetPropertyInitValue( sContent, &sBigIntValue, aKnlEnv ) == STL_SUCCESS );
                
                stlSnprintf( sPropertyFxRec.mInitValue, KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "%ld", sBigIntValue);
                break;
            }
            case KNL_PROPERTY_TYPE_VARCHAR:
            {
                /** String DataType */
                sPropertyFxRec.mDataTypeID = DTL_TYPE_VARCHAR;
                stlSnprintf( sPropertyFxRec.mDataType, 
                             KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "VARCHAR");
                
                /** STRING VALUE */
                STL_TRY ( knpGetPropertyFileValue( sContent, sStringValue, aKnlEnv ) == STL_SUCCESS );
                stlStrncpy( sPropertyFxRec.mValue, sStringValue, stlStrlen( sStringValue ) + 1 );
                
                /** STRING Init Value */
                STL_TRY ( knpGetPropertyInitValue( sContent, &sStringValue, aKnlEnv ) == STL_SUCCESS );
                stlStrncpy( sPropertyFxRec.mInitValue, sStringValue, stlStrlen( sStringValue ) + 1 );
                break;
            }
            default:
                break;        
        }
        
        /** Get UnitType */    
        STL_TRY( knpGetPropertyBigIntUnit( sContent, &sUnitType, aKnlEnv ) == STL_SUCCESS );
        
        switch( sUnitType )
        {
            case KNL_PROPERTY_BIGINT_UNIT_NONE:
            {
                stlSnprintf( sPropertyFxRec.mUnitType, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "NONE");
                break;
            }
            case KNL_PROPERTY_BIGINT_UNIT_SIZE:
            {
                stlSnprintf( sPropertyFxRec.mUnitType, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "BYTE");
                break;
            }
            case KNL_PROPERTY_BIGINT_UNIT_TIME:
            {
                stlSnprintf( sPropertyFxRec.mUnitType, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "MS");
                break;
            }
            case KNL_PROPERTY_BIGINT_UNIT_TIMEZONE_OFFSET:
            {
                stlSnprintf( sPropertyFxRec.mUnitType, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                            "GMT_OFFSET");
                break;
            }
            default:
            {
                break;
            }
        }
            
        /**
         * Value Source는 무조건 BINARY_FILE이다. 
         * SCOPE=FILE로 변경된 값만을 보여주기 때문에...
         */
        stlSnprintf( sPropertyFxRec.mSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                                "BINARY_FILE");
            
        /** Init Value Source */
        switch( sContent->mInitSource )
        {
            case KNP_SOURCE_DEFAULT:
            {
                stlSnprintf( sPropertyFxRec.mInitSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                             "DEFAULT");
                break;
            }
            case KNP_SOURCE_ENV:
            {
                stlSnprintf( sPropertyFxRec.mInitSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                             "ENV_VAR");
                break;
            }
            case KNP_SOURCE_BINARYFILE:
            {
                stlSnprintf( sPropertyFxRec.mInitSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                             "BINARY_FILE");
                break;
            }
            case KNP_SOURCE_TEXTFILE:
            {
                stlSnprintf( sPropertyFxRec.mInitSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                             "FILE");
                break;
            }
            case KNP_SOURCE_SYSTEM:
            {
                stlSnprintf( sPropertyFxRec.mInitSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                             "SYSTEM");
                break;
            }
            case KNP_SOURCE_USER:
            {
                stlSnprintf( sPropertyFxRec.mInitSource, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                             "USER");
                break;
            }
            default:
                break;
        }
        
        /** MIN/MAX */
        sPropertyFxRec.mMin = sContent->mMin;
        sPropertyFxRec.mMax = sContent->mMax;
                
        /** ETC Value */
        sPropertyFxRec.mID = sContent->mID;
        
        stlStrncpy( sPropertyFxRec.mName, sContent->mName, stlStrlen(sContent->mName) + 1 );
        stlStrncpy( sPropertyFxRec.mDesc, sContent->mDesc, stlStrlen(sContent->mDesc) + 1 );
        
        /** GROUP/STARTUP PHASE STRING */
        stlSnprintf( sPropertyFxRec.mGroup, KNL_PROPERTY_STRING_MAX_LENGTH, 
                     "%s", gPropertyGroupList[sContent->mGroup]);
        
        switch( sContent->mStartupPhaseDomain )
        {
            case KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW:
            {
                stlSnprintf( sPropertyFxRec.mStartupPhase, KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "%s BELOW", gPhaseString[sContent->mStartupPhase]);
                break;
            }
            case KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER:
            {
                stlSnprintf( sPropertyFxRec.mStartupPhase, KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "%s ABOVE", gPhaseString[sContent->mStartupPhase]);
                break;
            }
            case KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY:
            {
                stlSnprintf( sPropertyFxRec.mStartupPhase, KNL_PROPERTY_STRING_MAX_LENGTH, 
                             "%s", gPhaseString[sContent->mStartupPhase]);
                break;
            }
            case KNP_PROPERTY_PHASE_DOMAIN_MAX:
            default:
                break;
        }
        
        /** INTERNAL/EXTERNAL/HIDDEN */
        STL_TRY( knpGetPropertyDomain( sContent, 
                                       &sDomain, 
                                       aKnlEnv ) == STL_SUCCESS );
        
        if ( sDomain == KNL_PROPERTY_DOMAIN_EXTERNAL )
        {
            stlSnprintf( sPropertyFxRec.mInternal, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                         "EXTERNAL");
        }
        else
        {
            stlSnprintf( sPropertyFxRec.mInternal, KNL_PROPERTY_FXTABLE_BUFFER_SIZE, 
                         "INTERNAL");
        }
        
        /** Build Record */
        STL_TRY( knlBuildFxRecord( gPropertyColumnDesc,
                                   &sPropertyFxRec,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv ) == STL_SUCCESS );
         /**
          * NEXT PROPERTY
          */
        sPropertyID++;
        stlMemcpy( aPathCtrl, &sPropertyID, STL_SIZEOF(sPropertyID) );
        *aTupleExist = STL_TRUE;
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    *aTupleExist = STL_FALSE;

    return STL_FAILURE;
}

knlFxTableDesc gPropertyTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxPropertyCallback,
    closeFxPropertyCallback,
    buildRecordFxPropertyCallback,
    STL_SIZEOF( stlUInt16 ),        //knpPropertyContent.mID
    "X$PROPERTY",
    "Property information fixed table",
    gPropertyColumnDesc
};


knlFxTableDesc gSPropertyTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxPropertyCallback,
    closeFxPropertyCallback,
    buildRecordFxSPropertyCallback,
    STL_SIZEOF( stlUInt16 ),        //knpPropertyContent.mID
    "X$SPROPERTY",
    "Property information fixed table for Server",
    gPropertyColumnDesc
};

/** @} */
