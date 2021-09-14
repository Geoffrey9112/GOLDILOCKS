/*******************************************************************************
 * zlpxDynExecute.h
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

#ifndef _ZLPXDYNEXECUTE_H_
#define _ZLPXDYNEXECUTE_H_ 1

/**
 * @file zlpxDynExecute.h
 * @brief Gliese Embedded SQL in C precompiler library internal header file.
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>


/****************************************************************
 * Dynamic SQL
 ****************************************************************/

stlStatus zlpxDynExecute( zlplSqlContext   * aSqlContext,
                          zlplSqlArgs      * aSqlArgs,
                          stlChar          * aStmtName );

stlStatus zlpxGetDynamicItemCount( zlplSqlContext   * aSqlContext,
                                   sqldynhv_t       * aDynParam,
                                   stlInt32         * aItemCount );

/* @todo descriptor 관련 file 로 옮기자 */
stlStatus zlpxGetDescItemCount( zlplSqlContext   * aSqlContext,
                                sqldynhv_t       * aDynParam,
                                stlInt32         * aItemCount );

stlBool  zlpxCheckParamCacheExist( zlplDynStmtSymbol * aDynStmtSymbol,
                                   stlInt32            aParamCnt,
                                   stlInt32            aColumnCnt );

stlBool  zlpxCheckSameSymbolBindAddr( stlInt32    aAddrCnt,
                                      stlChar  ** aAddrArray,
                                      stlChar  ** aIndArray,
                                      sqlhv_t   * aHostArray );

void  zlpxStoreSymbolBindAddr( stlInt32    aAddrCnt,
                               stlChar  ** aAddrArray,
                               stlChar  ** aIndArray,
                               sqlhv_t   * aHostArray );

stlStatus zlpxAllocDynStmtSymbolMemory( zlplSqlContext    * aSqlContext,
                                        zlplSqlArgs       * aSqlArgs,
                                        zlplDynStmtSymbol * aDynStmtSymbol,
                                        stlInt32            aArraySize,
                                        stlInt32            aParamCnt,
                                        stlInt32            aColumnCnt );

stlStatus zlpxFreeDynStmtSymbolMemory( zlplSqlContext    * aSqlContext,
                                       zlplDynStmtSymbol * aDynStmtSymbol );

#endif /* _ZLPXDYNEXECUTE_H_ */
