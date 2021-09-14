/*******************************************************************************
 * ellGeneral.c
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
 * @file ellGeneral.c
 * @brief Execution Library Startup Routines
 */

#include <ell.h>
#include <elDef.h>

#include <elgStartup.h>
#include <elsSessionObjectMgr.h>

/**
 * @addtogroup ellProcEnv
 * @{
 */

/*
 * Process가 공유하는 Execution Library의 Shared Memory 위치
 */
ellSharedEntry * gEllSharedEntry = NULL;

/*
 * Dictionary Cache Build 시 사용되는 Cache Entry
 */
ellCacheEntry  * gEllCacheBuildEntry = NULL;  

extern stlErrorRecord gExecutionLibraryErrorTable[ELL_MAX_ERROR + 1];
extern stlFatalHandler gElgOldFatalHandler;

knlStartupFunc gStartupEL[KNL_STARTUP_PHASE_MAX] =
{
    NULL,
    elgStartupNoMount,
    elgStartupMount,
    elgStartupPreOpen,
    NULL
};

knlWarmupFunc gWarmupEL = elgWarmup;

knlShutdownFunc gShutdownEL[KNL_SHUTDOWN_PHASE_MAX] =
{
    NULL,
    elgShutdownInit,
    elgShutdownDismount,
    elgShutdownPostClose,
    NULL
};

knlCooldownFunc    gCooldownEL = elgCooldown;

dtlDataValue       gNullNumber;          
stlChar            gNullNumberBuffer[DTL_NUMERIC_MAX_SIZE];

/**
 * @brief Execution Library layer를 초기화한다.
 * @remark 프로세스당 한번 호출되어야 한다.
 */
stlStatus ellInitialize()
{
    if( stlGetProcessInitialized( STL_LAYER_EXECUTION_LIBRARY ) == STL_FALSE )
    {
        /**
         * 하위 Layer 초기화
         */
    
        STL_TRY( smlInitialize() == STL_SUCCESS );

        /**
         * FATAL Handler 등록
         */

        STL_TRY( stlHookFatalHandler( elgFatalHandler,
                                      & gElgOldFatalHandler )
                 == STL_SUCCESS );
    
        /**
         * 에러 테이블 등록
         */
    
        stlRegisterErrorTable( STL_ERROR_MODULE_EXECUTION_LIBRARY,
                               gExecutionLibraryErrorTable);
    
        stlSetProcessInitialized( STL_LAYER_EXECUTION_LIBRARY );

        /**
         * Null Number 설정
         */

        gNullNumber.mType       = DTL_TYPE_NUMBER;
        gNullNumber.mBufferSize = DTL_NUMERIC_MAX_SIZE;
        gNullNumber.mValue      = gNullNumberBuffer;
        DTL_SET_NULL( & gNullNumber );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Execution Library 의 Process Envrionment를 초기화 한다.
 * @param[out] aEnv     초기화할 Process Environment 포인터
 * @param[in]  aEnvType Env 종류
 * @remarks
 */
stlStatus ellInitializeEnv( ellEnv      * aEnv,
                            knlEnvType    aEnvType )
{
    stlInt32 sState = 0;
    
    /**
     * 메모리를 Reset 하면 안된다.
     * Environment 를 Environment Manager로부터 할당받은 경우,
     * 각 layer 별로 필요한 정보가 Environment에 모두 설정되어 있다.
     */
    STL_TRY( smlInitializeEnv( (smlEnv*) aEnv, aEnvType ) == STL_SUCCESS );
    sState = 1;

    /**
     * Long Type Block 초기화
     */
    
    aEnv->mLongTypeBlock = NULL;
    
    /**
     * Dictionary Operation을 위한 메모리 관리자를 초기화
     */

    STL_TRY( knlCreateRegionMem( & aEnv->mMemDictOP,
                                 KNL_ALLOCATOR_EXECUTION_LIBRARY_DICT_OPERATION,
                                 NULL,  /* aParentMem */
                                 KNL_MEM_STORAGE_TYPE_HEAP,
                                 ELL_DICT_OP_MEM_INIT_SIZE,
                                 ELL_DICT_OP_MEM_NEXT_SIZE,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;
                                  
    /**
     * Environment에 Top Layer 설정
     */
    knlSetTopLayer( STL_LAYER_EXECUTION_LIBRARY, KNL_ENV( aEnv ) );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) knlDestroyRegionMem( & aEnv->mMemDictOP, KNL_ENV(aEnv) );
        case 1:
            (void) smlFinalizeEnv( SML_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Exeuction Library 의 Process Envrionment를 종료한다.
 * @param[in] aEnv 종료할 Process Environment 포인터
 * @remarks
 */
stlStatus ellFinalizeEnv( ellEnv * aEnv )
{

    /**
     * Dictionary Operation을 위한 메모리 관리자를 종료
     */
    
    STL_TRY( knlDestroyRegionMem( & aEnv->mMemDictOP,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * 하위 layer 의 Process Environment 를 종료
     */
    
    STL_TRY( smlFinalizeEnv( (smlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} ellProcEnv */




/**
 * @addtogroup ellSessEnv
 * @{
 */

/**
 * @brief Execution Library 의 Session Environment 를 초기화한다.
 * @param[in] aSessEnv    Execution Library 의 Session Environment
 * @param[in] aConnectionType Session Connection Type
 * @param[in] aSessType   Session Type
 * @param[in] aSessEnvType Session Envrionment Type
 * @param[in] aEnv        Execution Library 의 Process Environment
 * @remarks
 */
stlStatus ellInitSessionEnv( ellSessionEnv     * aSessEnv,
                             knlConnectionType   aConnectionType,
                             knlSessionType      aSessType,
                             knlSessEnvType      aSessEnvType,
                             ellEnv            * aEnv )
{
    stlInt32    sState = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSessEnv != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 하위 Layer 의 Session Env 를 초기화
     */
    
    STL_TRY( smlInitializeSessionEnv( (smlSessionEnv *) aSessEnv,
                                      aConnectionType,
                                      aSessType,
                                      aSessEnvType,
                                      SML_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Session Object Manager 초기화
     */

    
    STL_TRY( elsInitSessObjectMgr( aSessEnv,
                                   aSessType,
                                   aSessEnvType,
                                   aEnv )
             == STL_SUCCESS );
    sState = 2;

    /**
     * 멤버 변수를 초기화
     */

    ellInitDictHandle( & aSessEnv->mLoginUser );
    ellInitDictHandle( & aSessEnv->mSessionUser );
    ellInitDictHandle( & aSessEnv->mCurrentUser );

    aSessEnv->mIsSetRole = STL_FALSE;
    ellInitDictHandle( & aSessEnv->mSessionRole );

    aSessEnv->mLoginAuthID = ELL_DICT_OBJECT_ID_NA;
    aSessEnv->mSessionAuthID = ELL_DICT_OBJECT_ID_NA;
    
    aSessEnv->mTimeDDL = 0;
    aSessEnv->mStmtRollbackPendOpList = NULL;
    aSessEnv->mStmtSuccessPendOpList = NULL;
    aSessEnv->mStmtPreRollbackActionList = NULL;

    STL_RING_INIT_HEADLINK( & aSessEnv->mLocalCache,
                            STL_OFFSETOF( eldcLocalCache, mCacheLink ) );
    
    aSessEnv->mIncludeDDL = STL_FALSE;
    aSessEnv->mLocalModifySeq = 0;
    aSessEnv->mTxPreRollbackActionList = NULL;
    aSessEnv->mTxRollbackPendOpList = NULL;
    aSessEnv->mTxCommitPendOpList = NULL;
    
    knlSetSessionTopLayer( (knlSessionEnv*)aSessEnv, STL_LAYER_EXECUTION_LIBRARY );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 2:
            (void) elsFiniSessObjectMgr( aEnv );
            
        case 1:
            (void) smlFinalizeSessionEnv( (smlSessionEnv *) aSessEnv, SML_ENV(aEnv) );
        default:
            break;
    }
    return STL_FAILURE;
}

/**
 * @brief Execution Library 의 Session Environment 를 종료한다.
 * @param[in] aSessEnv    Execution Library 의 Session Environment
 * @param[in] aEnv        Execution Library 의 Process Environment
 * @remarks
 */
stlStatus ellFiniSessionEnv( ellSessionEnv * aSessEnv,
                             ellEnv        * aEnv )
{
    stlInt32   sState = 0;
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSessEnv != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSessEnv->mIncludeDDL == STL_FALSE, ELL_ERROR_STACK(aEnv) );

    /**
     * Session Object 관리자 종료
     */

    sState = 1;
    STL_TRY( elsFiniSessObjectMgr( aEnv ) == STL_SUCCESS );
    
    /**
     * 멤버 변수를 초기화
     */

    ellInitDictHandle( & aSessEnv->mLoginUser );
    ellInitDictHandle( & aSessEnv->mSessionUser );
    ellInitDictHandle( & aSessEnv->mCurrentUser );

    aSessEnv->mIsSetRole = STL_FALSE;
    ellInitDictHandle( & aSessEnv->mSessionRole );

    aSessEnv->mLoginAuthID = ELL_DICT_OBJECT_ID_NA;
    aSessEnv->mSessionAuthID = ELL_DICT_OBJECT_ID_NA;
    
    aSessEnv->mTimeDDL = 0;
    aSessEnv->mStmtRollbackPendOpList = NULL;
    aSessEnv->mStmtSuccessPendOpList = NULL;
    aSessEnv->mStmtPreRollbackActionList = NULL;
    
    aSessEnv->mIncludeDDL = STL_FALSE;
    aSessEnv->mLocalModifySeq = 0;
    aSessEnv->mTxPreRollbackActionList = NULL;
    aSessEnv->mTxRollbackPendOpList = NULL;
    aSessEnv->mTxCommitPendOpList = NULL;

    /**
     * 하위 Layer 의 Session Env 를 종료
     */

    sState = 2;
    STL_TRY( smlFinalizeSessionEnv( (smlSessionEnv*) aSessEnv, SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 0:
            (void) elsFiniSessObjectMgr( aEnv );
        case 1:
            (void) smlFinalizeSessionEnv( (smlSessionEnv*) aSessEnv, SML_ENV(aEnv) );
        case 2:
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Ager Cleanup 연산을 위해서 Session Envrionment를 정리한다.
 * @param[in]     aSessEnv  Session Environment 포인터
 * @param[in,out] aEnv      Environment 포인터
 */
stlStatus ellCleanupSessionEnv( ellSessionEnv * aSessEnv,
                                ellEnv        * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSessEnv != NULL, ELL_ERROR_STACK( aEnv ) );

    /**
     * Shared Memory 상에 생성된 Cursor Slot Memory 관리자를 제거
     */
    
    if( KNL_DYNAMIC_MEMORY_TYPE( &aSessEnv->mSessObjMgr.mMemCursorSlot ) == KNL_MEM_STORAGE_TYPE_SHM )
    {
        STL_TRY( knlDestroyDynamicMem( & aSessEnv->mSessObjMgr.mMemCursorSlot,
                                       KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    
    if( KNL_DYNAMIC_MEMORY_TYPE( &aSessEnv->mSessObjMgr.mMemSessObj ) == KNL_MEM_STORAGE_TYPE_SHM )
    {
        STL_TRY( knlDestroyDynamicMem( & aSessEnv->mSessObjMgr.mMemSessObj,
                                       KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    if( KNL_REGION_MEMORY_TYPE( &aSessEnv->mSessObjMgr.mMemSessHash ) == KNL_MEM_STORAGE_TYPE_SHM )
    {
        STL_TRY( knlDestroyRegionMem( & aSessEnv->mSessObjMgr.mMemSessHash,
                                      KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}



/**
 * @brief Session Authorization Handle 이 유효한지 검사한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aViewSCN   View SCN
 * @param[in] aModified  변경 여부 
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus ellValidateSessionAuth( smlTransId    aTransID,
                                  smlScn        aViewSCN,
                                  stlBool     * aModified,
                                  ellEnv      * aEnv )
{
    stlBool  sModify = STL_FALSE;
    stlBool  sIsValid = STL_FALSE;
    
    ellDictHandle   sLoginAuthHandle;
    ellDictHandle   sSessionAuthHandle;
    stlBool         sObjectExist = STL_FALSE;
    
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aModified != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Login User 와 Session User 가 유효한지 검사 
     */

    STL_TRY( ellIsRecentDictHandle( aTransID,
                                    & sSessEnv->mLoginUser,
                                    & sIsValid,
                                    aEnv )
             == STL_SUCCESS );
    
    if ( sIsValid == STL_TRUE )
    {
        if ( sSessEnv->mLoginAuthID == sSessEnv->mSessionAuthID )
        {
            /**
             * 변경되지 않음
             */

            sModify = STL_FALSE;
        }
        else
        {
            /**
             * Login User 와 Session User 가 다른 경우
             */

            STL_TRY( ellIsRecentDictHandle( aTransID,
                                            & sSessEnv->mSessionUser,
                                            & sIsValid,
                                            aEnv )
                     == STL_SUCCESS );
            
            if ( sIsValid == STL_TRUE )
            {
                /**
                 * 변경되지 않음
                 */
                
                sModify = STL_FALSE;
            }
            else
            {
                /**
                 * 변경됨
                 */
                
                sModify = STL_TRUE;
            }
        }
    }
    else
    {
        /**
         * 변경됨
         */

        sModify = STL_TRUE;
    }

    if ( sModify == STL_TRUE )
    {
        /**
         * Login Authorization Handle 이 다시 획득 
         */

        STL_TRY( ellGetAuthDictHandleByID( aTransID,
                                           SML_MAXIMUM_STABLE_SCN,
                                           sSessEnv->mLoginAuthID,
                                           & sLoginAuthHandle,
                                           & sObjectExist,
                                           aEnv )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_USER_DROPPED );

        /**
         * Session Authorization Handle 이 다시 획득 
         */

        STL_TRY( ellGetAuthDictHandleByID( aTransID,
                                           SML_MAXIMUM_STABLE_SCN,
                                           sSessEnv->mSessionAuthID,
                                           & sSessionAuthHandle,
                                           & sObjectExist,
                                           aEnv )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_USER_DROPPED );

        /**
         * Session Authorization 정보 설정
         */
        
        ellSetLoginUser( & sLoginAuthHandle, aEnv );
        ellSetSessionUser( & sSessionAuthHandle, aEnv );
    }
    else
    {
        /* nothing to do */
    }

    /**
     * Output 설정
     */

    *aModified = sModify;
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_CATCH( RAMP_USER_DROPPED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_USER_DROPPED_BY_OTHER_SESSION,
                      NULL,
                      ELL_ERROR_STACK(aEnv) );
    }
    
    return STL_FAILURE;
}

/**
 * @brief SQL Session 에 Login User Handle 을 설정한다.
 * @param[in] aAuthHandle     Authorization Dict Handle
 * @param[in] aEnv            Environment
 * @remarks
 */
void ellSetLoginUser( ellDictHandle * aAuthHandle, ellEnv * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    stlMemcpy( & sSessEnv->mLoginUser, aAuthHandle, STL_SIZEOF( ellDictHandle ) );
    stlMemcpy( & sSessEnv->mSessionUser, aAuthHandle, STL_SIZEOF( ellDictHandle ) );
    stlMemcpy( & sSessEnv->mCurrentUser, aAuthHandle, STL_SIZEOF( ellDictHandle ) );
    
    sSessEnv->mLoginAuthID   = ellGetAuthID( aAuthHandle );
    sSessEnv->mSessionAuthID = ellGetAuthID( aAuthHandle );
}



/**
 * @brief SQL Session 에 Session User Handle 을 설정한다.
 * @param[in] aAuthHandle     Authorization Dict Handle
 * @param[in] aEnv            Environment
 * @remarks
 */
void ellSetSessionUser( ellDictHandle * aAuthHandle, ellEnv * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    stlMemcpy( & sSessEnv->mSessionUser, aAuthHandle, STL_SIZEOF( ellDictHandle ) );
    stlMemcpy( & sSessEnv->mCurrentUser, aAuthHandle, STL_SIZEOF( ellDictHandle ) );

    sSessEnv->mSessionAuthID = ellGetAuthID( aAuthHandle );
}


/**
 * @brief SQL Session 에 Current User Handle 을 설정한다.
 * @param[in] aAuthHandle     Authorization Dict Handle
 * @param[in] aEnv            Environment
 * @remarks
 */
void ellSetCurrentUser( ellDictHandle * aAuthHandle, ellEnv * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    stlMemcpy( & sSessEnv->mCurrentUser, aAuthHandle, STL_SIZEOF( ellDictHandle ) );
}

/**
 * @brief Error 발생시 Current User 를 Session User 로 복원한다.
 * @param[in] aEnv   Environment
 */
void ellRollbackCurrentUser( ellEnv * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    stlMemcpy( & sSessEnv->mCurrentUser, & sSessEnv->mSessionUser, STL_SIZEOF( ellDictHandle ) );
}

/**
 * @brief SQL Session 에 Session Role Handle 을 설정한다.
 * @param[in] aAuthHandle     Authorization Dict Handle
 * @param[in] aEnv            Environment
 * @remarks
 */
void ellSetSessionRole( ellDictHandle * aAuthHandle, ellEnv * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    sSessEnv->mIsSetRole = STL_TRUE;
    stlMemcpy( & sSessEnv->mSessionRole, aAuthHandle, STL_SIZEOF( ellDictHandle ) );
}



/**
 * @brief SQL Session 의 Current User Handle 을 얻는다.
 * @param[in]  aEnv  Environment
 * @return
 * Current User Handle
 * @remarks
 */
ellDictHandle * ellGetCurrentUserHandle( ellEnv * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    return & sSessEnv->mCurrentUser;
}

/**
 * @brief SQL Session 의 Session User Handle 을 얻는다.
 * @param[in]  aEnv  Environment
 * @return
 * Session User Handle
 * @remarks
 */
ellDictHandle * ellGetSessionUserHandle( ellEnv * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    return & sSessEnv->mSessionUser;
}

stlInt64 ellGetSessionAuthID( ellEnv * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    return sSessEnv->mSessionAuthID;
}

/**
 * @brief SQL Session 의 Login User Handle 을 얻는다.
 * @param[in]  aEnv  Environment
 * @return
 * Login User Handle
 * @remarks
 */
ellDictHandle * ellGetLoginUserHandle( ellEnv * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    return & sSessEnv->mLoginUser;
}


/**
 * @brief SQL Session 의 Session Role Handle 을 얻는다.
 * @param[in]  aEnv  Environment
 * @return
 * Current Role Handle (nullable)
 * @remarks
 */
ellDictHandle * ellGetSessionRoleHandle( ellEnv * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    if ( sSessEnv->mIsSetRole == STL_TRUE )
    {
        return & sSessEnv->mSessionRole;
    }
    else
    {
        return NULL;
    }
}


/**
 * @brief SQL Session 의 Current User 의 Default Temporary Tablespace Handle 를 얻는다.
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aViewSCN     View SCN
 * @param[out] aSpaceHandle Temporary Tablespace Handle
 * @param[in]  aEnv         Environment
 * @remarks
 */
stlStatus ellGetCurrentTempSpaceHandle( smlTransId       aTransID,
                                        smlScn           aViewSCN,
                                        ellDictHandle  * aSpaceHandle,
                                        ellEnv         * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    return ellGetAuthTempSpaceHandle( aTransID,
                                      aViewSCN,
                                      & sSessEnv->mCurrentUser,
                                      aSpaceHandle,
                                      aEnv );
}

/**
 * @brief NULL Number 에 해당하는 data value 를 얻는다.
 * @return dtlDataValue pointer
 */
dtlDataValue * ellGetNullNumber()
{
    return & gNullNumber;
}


/** @} ellSessEnv */
