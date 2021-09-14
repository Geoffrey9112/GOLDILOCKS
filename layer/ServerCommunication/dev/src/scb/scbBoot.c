/*******************************************************************************
 * scbBoot.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: scbBoot.c 13675 2014-10-06 14:17:27Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file scbBoot.c
 * @brief Server Communication Startup Routines
 */

#include <stl.h>
#include <cml.h>
#include <knl.h>
#include <scb.h>

extern knlFxTableDesc gScbSystemInfoTableDesc;
extern knlFxTableDesc gScbDispatcherTableDesc;
extern knlFxTableDesc gScbSharedServerTableDesc;

stlBool             gScbWarmedUp = STL_FALSE;
stlFatalHandler     gScbOldFatalHandler;

/**
 * @brief Warmup 한다.
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus scbWarmUp( void  * aEnv )
{
    gScbWarmedUp = STL_TRUE;
    
    return STL_SUCCESS;
}

/**
 * @brief CoolDown 한다.
 * @param[in,out] aEnv Environment 포인터
 */
stlStatus scbCoolDown( void * aEnv )
{
    gScbWarmedUp = STL_FALSE;
    
    return STL_SUCCESS;
}

/**
 * @brief Server Communication Layer를 No-Mount 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus scbStartupNoMount( knlStartupInfo * aStartupInfo,
                             void           * aEnv )
{
    STL_ASSERT( aEnv != NULL );

    return STL_SUCCESS;
}

/**
 * @brief Server Communication Layer를 Mount 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus scbStartupMount( knlStartupInfo * aStartupInfo,
                           void           * aEnv )
{
    STL_ASSERT( aEnv != NULL );

    return STL_SUCCESS;
}


/**
 * @brief Server Communication Layer를 Open 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 * @remarks
 */
stlStatus scbStartupOpen( knlStartupInfo * aStartupInfo,
                          void           * aEnv )
{
    STL_ASSERT( aEnv != NULL );
    
    return STL_SUCCESS;
}

/**
 * @brief Server Communication Layer를 INIT 시킨다.
 * @param[in] aShutdownInfo ShutdownInfo
 * @param[in,out] aEnv      Environment 구조체
 * @remarks
 */
stlStatus scbShutdownInit( knlShutdownInfo  * aShutdownInfo,
                           void             * aEnv )
{
    STL_ASSERT( aEnv != NULL );
    
    return STL_SUCCESS;
}

/**
 * @brief Server Communication Layer를 DISMOUNT 시킨다.
 * @param[in] aShutdownInfo ShutdownInfo
 * @param[in,out] aEnv      Environment 구조체
 * @remarks
 */
stlStatus scbShutdownDismount( knlShutdownInfo  * aShutdownInfo,
                               void             * aEnv )
{

    return STL_SUCCESS;
}

/**
 * @brief Server Communication Layer를 CLOSE 시킨다.
 * @param[in] aShutdownInfo ShutdownInfo
 * @param[in,out] aEnv      Environment 구조체
 * @remarks
 */
stlStatus scbShutdownClose( knlShutdownInfo  * aShutdownInfo,
                            void             * aEnv )
{
    STL_ASSERT( aEnv != NULL );

    return STL_SUCCESS;
}


/**
 * @brief Server Communication Layer의 Fatal Handler.
 * @param[in] aCauseString Fatal 유발 원인을 알수 있는 스트링
 * @param[in] aSigInfo Signal Information
 * @param[in] aContext 이전 Context 정보
 */
void scbFatalHandler( const stlChar * aCauseString,
                      void          * aSigInfo,
                      void          * aContext )
{

    /**
     * 하위 Layer의 Fatal Handler를 호출한다.
     */
    
    gScbOldFatalHandler( aCauseString, aSigInfo, aContext );
}


