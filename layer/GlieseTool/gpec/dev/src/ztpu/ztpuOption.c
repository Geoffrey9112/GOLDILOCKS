/*******************************************************************************
 * ztpuOption.c
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

#include <stl.h>
#include <dtl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztpDef.h>
#include <ztpuOption.h>

/**
 * @file ztpuOption.c
 * @brief Gliese Embedded SQL in C precompiler option processing functions
 */


/**
 * @addtogroup ztpuOption
 * @{
 */

stlStatus ztpuAddIncludeDir( stlAllocator  *aAllocator,
                             stlErrorStack *aErrorStack,
                             const stlChar *aDirPath )
{
    ztpIncludePath   *sIncludePath = NULL;
    ztpIncludePath   *sLastIncludePath = NULL;

    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(ztpIncludePath),
                                (void **)&sIncludePath,
                                aErrorStack )
             == STL_SUCCESS );
    stlStrncpy(sIncludePath->mIncludePath, aDirPath, STL_MAX_FILE_PATH_LENGTH + 1);
    sIncludePath->mNext = NULL;

    sLastIncludePath = &gIncludePath;
    while( sLastIncludePath->mNext != NULL )
    {
        sLastIncludePath = sLastIncludePath->mNext;
    }

    sLastIncludePath->mNext = sIncludePath;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
