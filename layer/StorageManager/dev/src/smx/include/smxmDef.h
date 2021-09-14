/*******************************************************************************
 * smxmDef.h
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


#ifndef _SMXMDEF_H_
#define _SMXMDEF_H_ 1

/**
 * @file smxmDef.h
 * @brief Storage Manager Layer Mini-Transaction Internal Definition
 */

/**
 * @defgroup smxmInternal Component Internal Routines
 * @ingroup smxm
 * @{
 */

#define SMXM_MAX_BLOCK_SIZE SMR_MAX_LOGPIECE_SIZE

#define SMXM_INIT_LOG_BLOCK( aLogBlock ) \
{                                        \
    (aLogBlock)->mBlockCount = 0;        \
}

/** @} */
    
#endif /* _SMXMDEF_H_ */
