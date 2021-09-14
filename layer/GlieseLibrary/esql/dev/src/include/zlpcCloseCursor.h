/*******************************************************************************
 * zlpcCloseCursor.h
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

#ifndef _ZLPCCLOSECURSOR_H_
#define _ZLPCCLOSECURSOR_H_ 1

/**
 * @file zlpcCloseCursor.h
 * @brief Gliese Embedded SQL in C precompiler SQL cursor library functions header file
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>

/************************************************************
 * Cursor
 ************************************************************/

/*
 * CLOSE cursor
 */

stlStatus zlpcCloseCursor( zlplSqlContext * aSqlContext,
                           zlplSqlArgs    * aSqlArgs );


#endif /* _ZLPCCLOSECURSOR_H_ */
