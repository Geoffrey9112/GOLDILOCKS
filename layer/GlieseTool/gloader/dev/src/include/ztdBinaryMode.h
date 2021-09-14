/*******************************************************************************
 * ztdExecute.h
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

#ifndef _ZTDBINARYMODE_H_
#define _ZTDBINARYMODE_H_ 1

/**
 * @file ztdExecute.h
 * @brief Execute  Definition
 */

/**
 * @defgroup ztdExecute Execute
 * @ingroup ztd
 * @{
 */

/**
 * @addtogroup binary mode
 * @{
 */

stlStatus ztdExportBinaryData( SQLHENV             aEnvHandle,
                               SQLHDBC             aDbcHandle,
                               ztdInputArguments   aInputArguments,
                               ztdControlInfo    * aControlInfo,
                               ztdFileAndBuffer  * aFileAndBuffer,
                               stlErrorStack     * aErrorStack );

stlStatus ztdImportBinaryData( SQLHENV             aEnvHandle,
                               SQLHDBC             aDbcHandle,
                               ztdInputArguments   aInputArguments,
                               ztdControlInfo    * aControlInfo,
                               ztdFileAndBuffer  * aFileAndBuffer,
                               stlErrorStack     * aErrorStack );

dtlDataType ztdGetDtlType( SQLSMALLINT aSQLType );

/** @} */

/** @} */

#endif /* _ZTDBINARYMODE_H_ */
