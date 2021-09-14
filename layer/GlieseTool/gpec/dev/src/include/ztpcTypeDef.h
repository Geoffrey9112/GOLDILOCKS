/*******************************************************************************
 * ztpcTypeDef.h
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


#ifndef _ZTPCTYPEDEF_H_
#define _ZTPCTYPEDEF_H_ 1

#include <dtl.h>
#include <zlplDef.h>

/**
 * @file ztpcTypeDef.h
 * @brief Gliese Embedded SQL in C precompiler common functions on Type Definitions
 */

/**
 * @defgroup ztpcTypeDef Gliese Embedded SQL in C precompiler common functions on Type Definitions
 * @ingroup ztp
 * @{
 */

/************************************************************************
 * Type Definitions
 ************************************************************************/

stlStatus ztpcAddTypeDefToSymTab(ztpSymTabHeader   *aSymTab,
                                 ztpCVariable      *aTypeDefSymbolList);

stlStatus ztpcFindTypeDefInSymTab(stlChar          *aSymName,
                                  ztpCVariable    **aTypeDefSymbol);


/** @} */

#endif /* _ZTPCTYPEDEF_H_ */
