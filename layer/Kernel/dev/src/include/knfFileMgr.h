/*******************************************************************************
 * knfFileMgr.h
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


#ifndef _KNFFILEMGR_H
#define _KNFFILEMGR_H_ 1

/**
 * @file knfFileMgr.h
 * @brief Kernel Layer File Manger Internal Routines
 */

/**
 * @defgroup knfFileMgr File Manager
 * @ingroup  knInternal
 * @internal
 * @{
 */

stlStatus knfInitializeFileMgr( knlEnv * aEnv );

stlStatus knfFinalizeFileMgr( knlEnv * aEnv );

stlStatus knfAddDbFile( stlChar       * aFileName,
                        knlDbFileType   aFileType,
                        knlEnv        * aEnv );

stlStatus knfRemoveDbFile( stlChar   * aFileName,
                           stlBool   * aIsRemoved,
                           knlEnv    * aEnv );

stlStatus knfCompactDbFileList( knlEnv * aEnv );

stlStatus knfInitDbFileIterator( knlFileItem ** aFileItem,
                                 knlEnv       * aEnv );

stlStatus knfFiniDbFileIterator( knlEnv * aEnv );

stlStatus knfFirstDbFile( knlFileItem ** aFileItem,
                          knlEnv       * aEnv );

stlStatus knfNextDbFile( knlFileItem ** aFileItem,
                         knlEnv       * aEnv );

void knfFileTypeString( knlDbFileType   aFileType,
                        stlChar       * aFileTypeName );

stlStatus knfExistDbFile( stlChar  * aFileName,
                          stlBool  * aFileExist,
                          knlEnv   * aEnv );

/** @} */
    
#endif /* _KNFFILEMGR_H_ */
