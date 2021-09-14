/*******************************************************************************
 * ztuMain.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztuMain.h $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZTU_MAIN_H_
#define _ZTU_MAIN_H_ 1

/**
 * @file ztuMain.h
 * @brief ztuMain Routines
 */
/**
 * @defgroup ztuMain Routines
 * @ingroup ztuMain
 * @{
 */

void ztuUsage();

void ztuInitializeArguments( ztuArguments * aArguments );

stlStatus ztuParseArguments( stlInt32       aArgc,
                             stlChar      * aArgv[],
                             ztuArguments * aArguments,
                             smlEnv       * aEnv );

void ztuGetFileTypeString( ztuArguments * aArguments,
                           stlChar      * aFormatStr );

/** @} */
#endif /* _ZTU_MAIN_H_ */
