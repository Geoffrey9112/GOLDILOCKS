/*******************************************************************************
 * knfFileMgr.c
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
 * @file knfFileMgr.c
 * @brief Kernel Layer File Manager implementation routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knDef.h>
#include <knl.h>
#include <knfFileMgr.h>

#define KNF_MEM_ALLOC_UNIT_SIZE  1024

extern knsEntryPoint * gKnEntryPoint;

stlStatus knfInitializeFileMgr( knlEnv * aEnv )
{
    STL_RING_INIT_HEADLINK( &gKnEntryPoint->mFileMgr.mFileList,
                            STL_OFFSETOF( knlFileItem, mList ) );

    STL_TRY( knlInitLatch( &gKnEntryPoint->mFileMgr.mFileLatch,
                           STL_FALSE,
                           aEnv ) == STL_SUCCESS );

    STL_TRY( knlCreateDynamicMem( &gKnEntryPoint->mFileMgr.mMemMgr,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_KERNEL_FILE_MANAGER,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  KNF_MEM_ALLOC_UNIT_SIZE * 16,
                                  KNF_MEM_ALLOC_UNIT_SIZE * 16,
                                  NULL,  /* aMemController */
                                  aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus knfFinalizeFileMgr( knlEnv * aEnv )
{
    STL_TRY( knlDestroyDynamicMem( &(gKnEntryPoint->mFileMgr.mMemMgr),
                                   aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus knfAddDbFile( stlChar       * aFileName,
                        knlDbFileType   aFileType,
                        knlEnv        * aEnv )
{
    knlFileItem * sNewFileItem = NULL;
    knlFileItem * sCurFile;
    knlFileItem * sNextFile;
    stlBool       sIsSuccess;
    stlUInt32     sState = 0;

    STL_TRY( knlAcquireLatch( &(gKnEntryPoint->mFileMgr.mFileLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_RING_FOREACH_ENTRY_SAFE( &(gKnEntryPoint->mFileMgr.mFileList),
                                 sCurFile,
                                 sNextFile)
    {
        /* database에서 사용중이면 already exist exception */
        if( stlStrcmp(aFileName, sCurFile->mFileName) == 0 )
        {
            STL_TRY( knlLogMsg( NULL,
                                aEnv,
                                KNL_LOG_LEVEL_INFO,
                                "[DATABASE FILE MANAGER] register database file (%s) - already in use\n",
                                aFileName )
                     == STL_SUCCESS );

            STL_THROW( RAMP_ERR_ALREADY_FILE_EXIST );
        }
    }

    STL_TRY( knlAllocDynamicMem( &(gKnEntryPoint->mFileMgr.mMemMgr),
                                 STL_SIZEOF( knlFileItem ),
                                 (void**)&sNewFileItem,
                                 aEnv ) == STL_SUCCESS );
    sState = 2;

    sNewFileItem->mFileName = NULL;

    STL_TRY( knlAllocDynamicMem( &(gKnEntryPoint->mFileMgr.mMemMgr),
                                 stlStrlen(aFileName) + 1,
                                 (void**)&(sNewFileItem->mFileName),
                                 aEnv ) == STL_SUCCESS );
    sState = 3;

    stlMemset( sNewFileItem->mFileName, 0x00, stlStrlen(aFileName) + 1 );
    stlStrcpy( sNewFileItem->mFileName, aFileName );
    sNewFileItem->mFileType = aFileType;

    STL_RING_ADD_LAST( &(gKnEntryPoint->mFileMgr.mFileList), (void *)sNewFileItem );
    sState = 4;

    STL_TRY( knlLogMsg( NULL,
                        aEnv,
                        KNL_LOG_LEVEL_INFO,
                        "[DATABASE FILE MANAGER] register database file (%s) - success\n",
                        aFileName )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlReleaseLatch( &(gKnEntryPoint->mFileMgr.mFileLatch), aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ALREADY_FILE_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_FILE_ALREADY_IN_USE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      aFileName );
    }

    STL_FINISH;

    switch( sState )
    {
        case 4:
            STL_RING_UNLINK( &(gKnEntryPoint->mFileMgr.mFileList), sNewFileItem );
        case 3:
            (void) knlFreeDynamicMem( &(gKnEntryPoint->mFileMgr.mMemMgr),
                                      sNewFileItem->mFileName,
                                      aEnv );
        case 2:
            (void) knlFreeDynamicMem( &(gKnEntryPoint->mFileMgr.mMemMgr),
                                      sNewFileItem,
                                      aEnv );
        case 1:
            (void) knlReleaseLatch( &(gKnEntryPoint->mFileMgr.mFileLatch), aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus knfRemoveDbFile( stlChar   * aFileName,
                           stlBool   * aIsRemoved,
                           knlEnv    * aEnv )
{
    knlFileItem * sCurFile = NULL;
    knlFileItem * sNextFile;
    stlBool       sFound = STL_FALSE;
    stlBool       sIsSuccess;
    stlInt32      sState = 0;

    STL_TRY( knlAcquireLatch( &(gKnEntryPoint->mFileMgr.mFileLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_RING_FOREACH_ENTRY_SAFE( &(gKnEntryPoint->mFileMgr.mFileList),
                                 sCurFile,
                                 sNextFile)
    {
        if( stlStrcmp(aFileName, sCurFile->mFileName) == 0 )
        {
            sFound = STL_TRUE;
            break;
        }
    }

    *aIsRemoved = sFound;

    if( sFound == STL_TRUE )
    {
        /* remove할 file을 찾았고, 현재 file list를 scan하지 않고 있으면,
         * file manager에서 file item을 즉시 제거한다. */
        STL_RING_UNLINK( &(gKnEntryPoint->mFileMgr.mFileList), sCurFile );

        STL_TRY( knlFreeDynamicMem( &(gKnEntryPoint->mFileMgr.mMemMgr),
                                    sCurFile->mFileName,
                                    aEnv ) == STL_SUCCESS );
        sCurFile->mFileName = NULL;

        STL_TRY( knlFreeDynamicMem( &(gKnEntryPoint->mFileMgr.mMemMgr),
                                    sCurFile,
                                    aEnv ) == STL_SUCCESS );
        sCurFile->mFileName = NULL;

        STL_TRY( knlLogMsg( NULL,
                            aEnv,
                            KNL_LOG_LEVEL_INFO,
                            "[DATABASE FILE MANAGER] remove database file (%s) - success\n",
                            aFileName )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlLogMsg( NULL,
                            aEnv,
                            KNL_LOG_LEVEL_INFO,
                            "[DATABASE FILE MANAGER] remove database file (%s) - file not found\n",
                            aFileName )
                 == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( knlReleaseLatch( &(gKnEntryPoint->mFileMgr.mFileLatch), aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlReleaseLatch( &(gKnEntryPoint->mFileMgr.mFileLatch), aEnv );
    }

    return STL_FAILURE;
}

stlStatus knfInitDbFileIterator( knlFileItem ** aFileItem,
                                 knlEnv       * aEnv )
{
    *aFileItem = NULL;

    return STL_SUCCESS;
}

stlStatus knfFiniDbFileIterator( knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus knfNextDbFile( knlFileItem ** aFileItem,
                         knlEnv       * aEnv )
{
    if( *aFileItem == NULL )
    {
        if( STL_RING_IS_EMPTY( &(gKnEntryPoint->mFileMgr.mFileList) ) != STL_TRUE )
        {
            *aFileItem = STL_RING_GET_FIRST_DATA( &(gKnEntryPoint->mFileMgr.mFileList) );
        }
    }
    else
    {
        *aFileItem = STL_RING_GET_NEXT_DATA( &(gKnEntryPoint->mFileMgr.mFileList), *aFileItem );

        if( STL_RING_IS_HEADLINK( &(gKnEntryPoint->mFileMgr.mFileList),
                                  *aFileItem ) == STL_TRUE )
        {
            *aFileItem = NULL;
        }
    }

    return STL_SUCCESS;
}

stlStatus knfExistDbFile( stlChar  * aFileName,
                          stlBool  * aFileExist,
                          knlEnv   * aEnv )
{
    knlFileItem * sCurFile = NULL;
    knlFileItem * sNextFile;
    stlBool       sIsSuccess;
    stlInt32      sState = 0;

    *aFileExist = STL_FALSE;

    STL_TRY( knlAcquireLatch( &(gKnEntryPoint->mFileMgr.mFileLatch),
                              KNL_LATCH_MODE_SHARED,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_RING_FOREACH_ENTRY_SAFE( &(gKnEntryPoint->mFileMgr.mFileList),
                                 sCurFile,
                                 sNextFile )
    {
        if( stlStrcmp(aFileName, sCurFile->mFileName) == 0 )
        {
            *aFileExist = STL_TRUE;
            break;
        }
    }
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(gKnEntryPoint->mFileMgr.mFileLatch), aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlReleaseLatch( &(gKnEntryPoint->mFileMgr.mFileLatch), aEnv );
    }

    return STL_FAILURE;
}

void knfFileTypeString( knlDbFileType   aFileType,
                        stlChar       * aFileTypeName )
{
    switch( aFileType )
    {
        case KNL_DATABASE_FILE_TYPE_CONTROL:
            stlStrcpy( aFileTypeName, "Control File" );
            break;
        case KNL_DATABASE_FILE_TYPE_CONFIG:
            stlStrcpy( aFileTypeName, "Config File" );
            break;
        case KNL_DATABASE_FILE_TYPE_DATAFILE:
            stlStrcpy( aFileTypeName, "Data File" );
            break;
        case KNL_DATABASE_FILE_TYPE_REDO_LOG:
            stlStrcpy( aFileTypeName, "Redo Log File" );
            break;
        case KNL_DATABASE_FILE_TYPE_TRACE_LOG:
            stlStrcpy( aFileTypeName, "Trace Log File" );
            break;
        case KNL_DATABASE_FILE_TYPE_TEMP:
            stlStrcpy( aFileTypeName, "Temporary File" );
            break;
        default:
            stlStrcpy( aFileTypeName, "UNKNOWN" );
            break;
    }
}

/** @} */
