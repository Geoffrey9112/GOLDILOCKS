/*******************************************************************************
 * elDef.h
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


#ifndef _ELDEF_H_
#define _ELDEF_H_ 1

/**
 * @file elDef.h
 * @brief Execution Library Layer Interfanl Definitions
 */

/**
 * @defgroup elInternal Execution Library Layer Internal Routines
 * @internal
 * @{
 */

/**
 * @defgroup elg General
 * @ingroup elInternal
 * @{
 */

/**
 * @defgroup elgEvent Event Table 
 * @ingroup elg
 * @{
 */

/**
 * @brief Dictionary Cache를 해제하기 위한 이벤트
 */
#define ELG_EVENT_FREE_DICTIONARY_MEMORY  KNL_MAKE_EVENT_ID( STL_LAYER_EXECUTION_LIBRARY, 0 )

/** @} elgEvent */

/** @} elg */

/**
 * @defgroup elf Function
 * @ingroup elInternal
 * @{
 */

/** @} elf */

/**
 * @defgroup eldDict Dictionary 관리 공통 모듈
 * @ingroup elInternal
 * @{
 */

/**
 * @brief Local cache
 */

typedef struct eldcLocalCache
{
    stlRingEntry   mCacheLink;   /* cache link */
    void         * mObject;      /* Object Pointer */
    stlBool        mDropFlag;    /* drop flag */
    stlUInt64      mModifySeq;   /* object modify sequence */
} eldcLocalCache;

/** @} eldDict */

/**
 * @defgroup eld Dictionary Management
 * @ingroup elInternal
 * @{
 */

/**
 * @defgroup eldDesc Dictionary Descriptor
 * @ingroup eld
 * @{
 */


/** @} eldDesc */

/** @} eld */


/**
 * @defgroup els Session Object
 * @ingroup elInternal
 * @{
 */

/** @} els */


/** @} elInternal */


#endif /* _ELDEF_H_ */
