/*******************************************************************************
 * elsSessionObjectMgr.c
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
 * @file elsSessionObjectMgr.c
 * @brief Execution Library Layer Session Object Manager
 */


#include <ell.h>
#include <elsSessionObjectMgr.h>
#include <elsCursor.h>
#include <eldt.h>
#include <eldc.h>

/**
 * @addtogroup elsSessObjMgr
 * @{
 */


/**
 * @brief Session-Object Manager 를 초기화한다.
 * @param[in]  aSessEnv       Session Environment
 * @param[in]  aSessType      Session Type
 * @param[in]  aSessEnvType   Session Env Type
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus  elsInitSessObjectMgr( ellSessionEnv    * aSessEnv,
                                 knlSessionType     aSessType,
                                 knlSessEnvType     aSessEnvType,
                                 ellEnv           * aEnv )
{
    stlInt32   sState = 0;
    stlUInt32  sMemType = 0;
    stlBool    sIsShm = STL_FALSE;
    ellSessObjectMgr * sSessObjMgr = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSessEnv != NULL, ELL_ERROR_STACK(aEnv) );

    sSessObjMgr = & aSessEnv->mSessObjMgr;

    sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
    sIsShm   = STL_FALSE;
    
    switch (aSessEnvType)
    {
        case KNL_SESS_ENV_HEAP:
            {
                sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
                sIsShm = STL_FALSE;
                break;
            }
        case KNL_SESS_ENV_SHARED:
            {
                /**
                 * admin session은 shared memory를 사용하지 않는다.
                 * - static memory가 부족한 상황에서도 세션을 사용할수
                 *   있어야 한다.
                 */
                if( knlIsAdminSession( aSessEnv ) == STL_TRUE )
                {
                    sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
                    sIsShm = STL_FALSE;
                }
                else
                {
                    switch (aSessType)
                    {
                        case KNL_SESSION_TYPE_DEDICATE:
                            sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
                            sIsShm   = STL_FALSE;
                            break;
                        case KNL_SESSION_TYPE_SHARED:
                            sMemType = KNL_MEM_STORAGE_TYPE_SHM;
                            sIsShm   = STL_TRUE;
                            break;
                        default:
                            STL_DASSERT( 0 );
                            break;
                    }
                }
                break;
            }
        default:
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    /**
     * 멤버 초기화
     */

    sSessObjMgr->mLatestDeclCursor = NULL;
    sSessObjMgr->mLatestInstCursor = NULL;
    
    sSessObjMgr->mMaxCursorCount = 0;
    sSessObjMgr->mUsedCursorCount = 0;
    sSessObjMgr->mNextCursorSlot = 0;
    sSessObjMgr->mCursorSlot = NULL;

    STL_RING_INIT_HEADLINK( & sSessObjMgr->mOpenCursorList,
                            STL_OFFSETOF( ellCursorInstDesc, mListLink ) );         
    
    /**
     * Session Object 관리자 초기화 
     */
    
    if ( aSessEnvType == KNL_SESS_ENV_HEAP )
    {
        /**
         * 초기화 과정 중의 Session 임.
         */
        
        sSessObjMgr->mHashDeclCursor = NULL;
        sSessObjMgr->mHashInstCursor = NULL;
    }
    else
    {
        /**
         * Session Object Memory 관리자 초기화
         */

        STL_TRY( knlCreateDynamicMem( & sSessObjMgr->mMemSessObj,
                                      NULL, /* aParentMem */
                                      KNL_ALLOCATOR_EXECUTION_LIBRARY_SESSION_OBJECT,
                                      sMemType,
                                      ELS_SESSION_OBJECT_MEM_INIT_SIZE,
                                      ELS_SESSION_OBJECT_MEM_NEXT_SIZE,
                                      NULL, /* aMemController */
                                      KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sState = 1;

        /**
         * Session Hash Memory 관리자 초기화
         */
        
        STL_TRY( knlCreateRegionMem( & sSessObjMgr->mMemSessHash,
                                     KNL_ALLOCATOR_EXECUTION_LIBRARY_SESSION_HASH,
                                     NULL,  /* aParentMem */
                                     sMemType,
                                     ELS_SESSION_HASH_MEM_INIT_SIZE,
                                     ELS_SESSION_HASH_MEM_NEXT_SIZE,
                                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sState = 2;
        
        
        /**
         * Cursor Declaration Descriptor 를 위한 Hash 초기화
         */
        
        STL_TRY( knlCreateStaticHash( & sSessObjMgr->mHashDeclCursor,
                                      ELS_CURSOR_OBJECT_BUCKET_COUNT,
                                      STL_OFFSETOF( ellCursorDeclDesc, mLink ),
                                      STL_OFFSETOF( ellCursorDeclDesc, mCursorKey ),
                                      KNL_STATICHASH_MODE_NOLATCH,
                                      sIsShm,
                                      & sSessObjMgr->mMemSessHash,
                                      KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sState = 3;
        
        /**
         * Cursor Instance Descriptor 를 위한 Hash 초기화
         */
        
        STL_TRY( knlCreateStaticHash( & sSessObjMgr->mHashInstCursor,
                                      ELS_CURSOR_OBJECT_BUCKET_COUNT,
                                      STL_OFFSETOF( ellCursorInstDesc, mHashLink ),
                                      STL_OFFSETOF( ellCursorInstDesc, mCursorKey ),
                                      KNL_STATICHASH_MODE_NOLATCH,
                                      sIsShm,
                                      & sSessObjMgr->mMemSessHash,
                                      KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sState = 4;
        
        /**
         * Cursor Slot 메모리 관리자 초기화
         */

        if( knlIsAdminSession( aSessEnv ) == STL_TRUE )
        {
            /**
             * admin session은 shared memory를 사용하지 않는다.
             * - static memory가 부족한 상황에서도 세션을 사용할수
             *   있어야 한다.
             */
            sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
        }
        else
        {
            /**
             * fixed table 로 조회하기 위해 Shared Memory 상에 생성되어야 함
             */
            sMemType = KNL_MEM_STORAGE_TYPE_SHM;
        }

        STL_TRY( knlCreateDynamicMem( & sSessObjMgr->mMemCursorSlot,
                                      NULL, /* aParentMem */
                                      KNL_ALLOCATOR_EXECUTION_LIBRARY_CURSOR_SLOT,
                                      sMemType,
                                      ELS_SESSION_CURSOR_SLOT_MEM_INIT_SIZE,
                                      ELS_SESSION_CURSOR_SLOT_MEM_NEXT_SIZE,
                                      NULL, /* aMemController */
                                      KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        sState = 5;

        /**
         * instance cursor slot 초기화
         */
        
        STL_TRY( ellInitNamedCursorSlot( aSessEnv, aEnv ) == STL_SUCCESS );
    }
        
    return STL_SUCCESS;

    STL_FINISH;

    switch ( sState )
    {
        case 5:
            (void) knlDestroyDynamicMem( & sSessObjMgr->mMemCursorSlot,
                                         KNL_ENV(aEnv) );
        case 4:
        case 3:
        case 2:
            (void) knlDestroyRegionMem( & sSessObjMgr->mMemSessHash,
                                        KNL_ENV(aEnv) );
        case 1:
            (void) knlDestroyDynamicMem( & sSessObjMgr->mMemSessObj,
                                         KNL_ENV(aEnv) );
        default:
            break;
    }

    sSessObjMgr->mHashDeclCursor = NULL;
    sSessObjMgr->mHashInstCursor = NULL;
    
    sSessObjMgr->mLatestDeclCursor = NULL;
    sSessObjMgr->mLatestInstCursor = NULL;
    
    sSessObjMgr->mMaxCursorCount = 0;
    sSessObjMgr->mUsedCursorCount = 0;
    sSessObjMgr->mNextCursorSlot = 0;
    sSessObjMgr->mCursorSlot = NULL;
    
    STL_RING_INIT_HEADLINK( & sSessObjMgr->mOpenCursorList,
                            STL_OFFSETOF( ellCursorInstDesc, mListLink ) ); 
    
    return STL_FAILURE;
}

/**
 * @brief Session-Object Manager 를 종료한다.
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus  elsFiniSessObjectMgr( ellEnv * aEnv )
{
    stlInt32   sState = 0;

    ellSessionEnv * sSessEnv = NULL;

    sSessEnv = ELL_SESS_ENV( aEnv );
    
    if ( KNL_IS_SHARED_SESS_ENV( sSessEnv ) == STL_FALSE )
    {
        /**
         * Session 초기화 과정임
         */
        sState = 5;
    }
    else
    {
        /**
         * Instance Cursor Slot 제거 
         */
        
        sState = 1;

        STL_TRY( ellFiniNamedCursorSlot( sSessEnv, aEnv ) == STL_SUCCESS );
        
        /**
         * Dictionary Local Cache 제거
         */
        
        sState = 2;
        eldcFreeLocalCaches( aEnv );

        /**
         * Session Hash Memory 관리자 종료 
         */
        
        sState = 3;
        STL_TRY( knlDestroyRegionMem( & sSessEnv->mSessObjMgr.mMemSessHash,
                                      KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /**
         * Session Object Memory 관리자 종료 
         */

        knlValidateDynamicMem( & sSessEnv->mSessObjMgr.mMemSessObj, KNL_ENV(aEnv) );
        
        sState = 4;
        STL_TRY( knlDestroyDynamicMem( & sSessEnv->mSessObjMgr.mMemSessObj,
                                       KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Cursor Slot Memory 관리자 종료 
         */

        knlValidateDynamicMem( & sSessEnv->mSessObjMgr.mMemSessObj, KNL_ENV(aEnv) );
        
        sState = 5;
        STL_TRY( knlDestroyDynamicMem( & sSessEnv->mSessObjMgr.mMemCursorSlot,
                                       KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    sSessEnv->mSessObjMgr.mHashDeclCursor = NULL;
    sSessEnv->mSessObjMgr.mHashInstCursor = NULL;

    sSessEnv->mSessObjMgr.mLatestDeclCursor = NULL;
    sSessEnv->mSessObjMgr.mLatestInstCursor = NULL;

    sSessEnv->mSessObjMgr.mMaxCursorCount = 0;
    sSessEnv->mSessObjMgr.mNextCursorSlot = 0;
    sSessEnv->mSessObjMgr.mCursorSlot = NULL;
    
    STL_RING_INIT_HEADLINK( & sSessEnv->mSessObjMgr.mOpenCursorList,
                            STL_OFFSETOF( ellCursorInstDesc, mListLink ) );         
    
    return STL_SUCCESS;

    STL_FINISH;

    switch ( sState )
    {
        case 0:
            (void) ellFiniNamedCursorSlot( sSessEnv, aEnv );
        case 1:
            (void) eldcFreeLocalCaches( aEnv );
        case 2:
            (void) knlDestroyRegionMem( & sSessEnv->mSessObjMgr.mMemSessHash,
                                        KNL_ENV(aEnv) );
        case 3:
            (void) knlDestroyDynamicMem( & sSessEnv->mSessObjMgr.mMemSessObj,
                                         KNL_ENV(aEnv) );
        case 4:
            (void) knlDestroyDynamicMem( & sSessEnv->mSessObjMgr.mMemCursorSlot,
                                         KNL_ENV(aEnv) );
        default:
            break;
    }

    sSessEnv->mSessObjMgr.mHashDeclCursor = NULL;
    sSessEnv->mSessObjMgr.mHashInstCursor = NULL;
    
    sSessEnv->mSessObjMgr.mLatestDeclCursor = NULL;
    sSessEnv->mSessObjMgr.mLatestInstCursor = NULL;
    
    sSessEnv->mSessObjMgr.mMaxCursorCount = 0;
    sSessEnv->mSessObjMgr.mNextCursorSlot = 0;
    sSessEnv->mSessObjMgr.mCursorSlot = NULL;
    
    STL_RING_INIT_HEADLINK( & sSessEnv->mSessObjMgr.mOpenCursorList,
                            STL_OFFSETOF( ellCursorInstDesc, mListLink ) ); 
            
    
    return STL_FAILURE;
}

/** @} elsSessObjMgr */


