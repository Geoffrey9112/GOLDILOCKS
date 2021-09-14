/*******************************************************************************
 * smlAggr.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smlAggr.h 7364 2013-02-22 07:36:43Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMLAGGR_H_
#define _SMLAGGR_H_ 1

/**
 * @file smlAggr.h
 * @brief Storage Manager Aggr Routines
 */

/**
 * @defgroup smlAggr Aggr
 * @ingroup smExternal
 * @{
 */

stlStatus smlFetchAggr( void          * aIterator,
                        smlFetchInfo  * aAggrFetchInfo,
                        smlEnv        * aEnv );

/** @} */

#endif /* _SMLAGGR_H_ */
