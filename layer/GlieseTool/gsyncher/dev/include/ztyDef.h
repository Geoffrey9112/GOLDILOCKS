/*******************************************************************************
 * ztyDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ZTYDEF_H_
#define _ZTYDEF_H_ 1

/**
 * @file ztyDef.h
 * @brief gsyncher Definitions
 */

/**
 * @defgroup ztyError Error
 * @{
 */

#define ZTY_ERRCODE_INVALID_PHASE     STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSYNCHER, 0 )
#define ZTY_ERRCODE_INVALID_HOME      STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSYNCHER, 1 )
#define ZTY_ERRCODE_GMASTER_ACTIVE    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GSYNCHER, 2 )

#define ZTY_MAX_ERROR   3

/** @} */

#define ZTY_MAX_LOGGER_SIZE   ( 10 * 1024 * 1024 )  /**< 10M bytes */

#endif /* _ZTYDEF_H_ */
