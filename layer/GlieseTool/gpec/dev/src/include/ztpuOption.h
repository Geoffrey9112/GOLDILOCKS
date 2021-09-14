/*******************************************************************************
 * ztpuOption.h
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


#ifndef _ZTPUOPTION_H_
#define _ZTPUOPTION_H_ 1

#include <dtl.h>

/**
 * @file ztpuOption.h
 * @brief Gliese Embedded SQL in C precompiler option processing functions
 */

/**
 * @defgroup ztpuOption Gliese Embedded SQL in C precompiler option processing functions
 * @ingroup ztp
 * @{
 */

stlStatus ztpuAddIncludeDir( stlAllocator  *aAllocator,
                             stlErrorStack *aErrorStack,
                             const stlChar *aDirPath );


/** @} */

#endif /* _ZTPUOPTION_H_ */
