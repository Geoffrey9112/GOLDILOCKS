/*******************************************************************************
 * knlFileMgr.h
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


#ifndef _KNLFILEMGR_H_
#define _KNLFILEMGR_H_ 1

/**
 * @file knlFileMgr.h
 * @brief Kernel Layer File Manager Routines
 */

/**
 * @defgroup knlFileMgr Process File Manager
 * @ingroup knExternal
 * @{
 */

stlStatus knlInitializeFileMgr( knlEnv * aEnv );

stlStatus knlFinalizeFileMgr( knlEnv * aEnv );

stlStatus knlAddDbFile( stlChar       * aFileName,
                        knlDbFileType   aFileType,
                        knlEnv        * aEnv );

stlStatus knlRemoveDbFile( stlChar   * aFileName,
                           stlBool   * aIsRemoved,
                           knlEnv    * aEnv );

stlStatus knlExistDbFile( stlChar  * aFileName,
                          stlBool  * aFileExist,
                          knlEnv   * aEnv );

/** @} */
    
#endif /* _KNLFIELMGR_H_ */
