/*******************************************************************************
 * ztdMain.h
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZTD_MAIN_H_
#define _ZTD_MAIN_H_ 1

/**
 * @file ztdMain.h
 * @brief Main Definition
 */

/**
 * @defgroup ztdMain
 * @ingroup ztd
 * @{
 */

stlStatus ztdCloseDatabase( SQLHENV         aEnvHandle,
                            SQLHDBC         aDbcHandle,
                            stlBool         aIsRollBack,
                            stlErrorStack * aErrorStack);

stlStatus ztdOpenDatabase( SQLHENV           * aEnvHandle,
                           SQLHDBC           * aDbcHandle,
                           ztdInputArguments * aInputArguments,
                           stlChar           * aCharacterSet,
                           stlErrorStack     * aErrorStack );

stlStatus ztdParseControl( ztdFileAndBuffer * aFileAndBuffer,
                           ztdControlInfo   * aControlInfo,
                           stlAllocator     * aAllocator,
                           stlErrorStack    * aErrorstac );

stlStatus ztdInitInputArguments( ztdInputArguments * aInputArguments,
                                 stlErrorStack     * aErrorStack );

stlStatus ztdParseOptions( stlInt32            aArgc,
                           stlChar           * aArgv[],
                           ztdInputArguments * aInputArguments,
                           stlErrorStack     * aErrorStack );

stlStatus ztdExamineOptions( ztdInputArguments  * aInputArguments,
                             stlErrorStack      * aErrorStack );

stlStatus ztdInitManager( stlErrorStack * aErrorStack );

stlStatus ztdFiniManager( stlErrorStack * aErrorStack );

/** @} */
#endif /* _ZTD_MAIN_H_ */
