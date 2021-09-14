/*******************************************************************************
 * smlKey.h
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


#ifndef _SMLKEY_H_
#define _SMLKEY_H_ 1

/**
 * @file smlKey.h
 * @brief Storage Manager Index Key Routines
 */

/**
 * @defgroup smlKey Index Key
 * @ingroup smExternal
 * @{
 */

stlStatus smlInsertKeys( smlStatement            * aStatement,
                         void                    * aRelationHandle,
                         knlValueBlockList       * aColumns,
                         smlRowBlock             * aRowBlock,
                         stlBool                 * aBlockFilter,
                         stlBool                   aIsDeferred,
                         stlInt32                * aViolationCnt,
                         smlUniqueViolationScope * aUniqueViolationScope,
                         smlEnv                  * aEnv );

stlStatus smlDeleteKeys( smlStatement            * aStatement,
                         void                    * aRelationHandle,
                         knlValueBlockList       * aColumns,
                         smlRowBlock             * aRowBlock,
                         stlBool                 * aBlockFilter,
                         stlInt32                * aViolationCnt,
                         smlUniqueViolationScope * aUniqueViolationScope,
                         smlEnv                  * aEnv );
/** @} */

#endif /* _SMLRECORD_H_ */
