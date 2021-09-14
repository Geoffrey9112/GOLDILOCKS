/*******************************************************************************
 * zlpxDynExecuteImmediate.h
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

#ifndef _ZLPXDYNEXECUTEIMMEDIATE_H_
#define _ZLPXDYNEXECUTEIMMEDIATE_H_ 1

/**
 * @file zlpxDynExecuteImmediate.h
 * @brief Gliese Embedded SQL in C precompiler library internal header file.
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>


/****************************************************************
 * Dynamic SQL
 ****************************************************************/

stlStatus zlpxDynExecuteImmediate( zlplSqlContext   * aSqlContext,
                                   zlplSqlArgs      * aSqlArgs );

#endif /* _ZLPXDYNEXECUTEIMMEDIATE_H_ */
