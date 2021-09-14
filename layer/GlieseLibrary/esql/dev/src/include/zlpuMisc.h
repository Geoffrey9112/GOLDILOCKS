/*******************************************************************************
 * zlpuMisc.h
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

#ifndef _ZLPUMISC_H_
#define _ZLPUMISC_H_ 1

/**
 * @file zlpuMisc.h
 * @brief Gliese Embedded SQL in C precompiler utility library functions
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>

extern SQLHENV                gEnv;
extern zlplSqlContext        *gDefaultContext;

extern stlSemaphore           gAllocatorSem;
extern stlAllocator           gAllocator;
extern stlDynamicAllocator    gDynMemory;
extern stlErrorStack          gErrorStack;
extern stlThreadOnce          gThreadOnce;
extern stlUInt32              gIsInit;

extern stlSpinLock            gCtxtRingSpinLock;
extern stlRingHead            gCtxtRing;


/************************************************************
 * Misc
 ************************************************************/

void      zlpuInitialize(void);
void      zlpuFinalize(void);
void      zlpuPrintErrorStack(stlErrorStack *aErrorStack);
stlStatus zlpuAllocContextMem(zlplSqlContext **aSqlContext);
stlStatus zlpuFreeContextMem(zlplSqlContext **aSqlContext);
stlStatus zlpuAllocStmt(SQLHANDLE    *aStmtHandle,
                        SQLHANDLE     aDbc,
                        zlplSqlArgs  *aSqlArgs);
stlStatus zlpuFreeStmt(SQLHANDLE  aStmtHandle);

stlInt32  zlpuGetMinArraySize( stlInt32          aIterationValue,
                               stlInt32          aHostVarCount,
                               zlplSqlHostVar   *aHostVar,
                               stlInt32         *aIteration );

#endif /* _ZLPUMISC_H_ */
