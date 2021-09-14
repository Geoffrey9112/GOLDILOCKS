/*******************************************************************************
 * smlStatistics.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smlStatistics.h 7364 2013-02-22 07:36:43Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMLSTATISTICS_H_
#define _SMLSTATISTICS_H_ 1

/**
 * @file smlStatistics.h
 * @brief Storage Manager Statistics Routines
 */

/**
 * @defgroup smlStatistics Statistics
 * @ingroup smExternal
 * @{
 */

stlStatus smlGetUsedPageCount( void     * aRelationHandle,
                               stlInt64 * aUsedPageCount,
                               smlEnv   * aEnv );

/** @} */

#endif /* _SMLSTATISTICS_H_ */
