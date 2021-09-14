/*******************************************************************************
 * knlFileMgr.c
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
 * @file knlFileMgr.c
 * @brief Kernel Layer File Manager wrapper routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knfFileMgr.h>

/**
 * @brief Process내에서 사용되는 file Descriptor를 관리하는 manager를 초기화 한다
 * @param[out] aEnv environment 정보
 */
stlStatus knlInitializeFileMgr( knlEnv * aEnv )
{
    return knfInitializeFileMgr( aEnv );
}

/**
 * @brief file manager를 종료한다
 * @param[out] aEnv environment 정보
 */
stlStatus knlFinalizeFileMgr( knlEnv * aEnv )
{
    return knfFinalizeFileMgr( aEnv );
}

/**
 * @brief Database가 사용하는 file을 추가한다.
 * @param[in] aFileName 추가할 file 이름
 * @param[in] aFileType 추가할 file의 file type
 * @param[in] aEnv environment 정보
 */
stlStatus knlAddDbFile( stlChar       * aFileName,
                        knlDbFileType   aFileType,
                        knlEnv        * aEnv )
{
    return knfAddDbFile( aFileName,
                         aFileType,
                         aEnv );
}

/**
 * @brief Database가 사용하는 file을 제거한다.
 * @param[in] aFileName 제거할 file 이름
 * @param[in] aIsRemoved file이 제거되었는지 여부
 * @param[in] aEnv environment 정보
 */
stlStatus knlRemoveDbFile( stlChar   * aFileName,
                           stlBool   * aIsRemoved,
                           knlEnv    * aEnv )
{
    return knfRemoveDbFile( aFileName,
                            aIsRemoved,
                            aEnv );
}

/**
 * @brief 주어진 file name이 database에서 사용 중인지 체크한다.
 * @param[in] aFileName 제거할 file 이름
 * @param[in] aFileExist file이 제거되었는지 여부
 * @param[in] aEnv environment 정보
 */
stlStatus knlExistDbFile( stlChar   * aFileName,
                          stlBool   * aFileExist,
                          knlEnv    * aEnv )
{
    return knfExistDbFile( aFileName,
                           aFileExist,
                           aEnv );
}

/** @} */
