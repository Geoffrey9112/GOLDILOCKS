/*******************************************************************************
 * zlpcDeclareCursor.h
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

#ifndef _ZLPCDECLARECURSOR_H_
#define _ZLPCDECLARECURSOR_H_ 1

/**
 * @file zlpcDeclareCursor.h
 * @brief Gliese Embedded SQL in C precompiler SQL cursor library function header file
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>

/************************************************************
 * Cursor
 ************************************************************/

/*
 * DECLARE cursor
 */

stlStatus zlpcDeclareCursor( zlplSqlContext * aSqlContext,
                             zlplSqlArgs    * aSqlArgs );


#endif /* _ZLPCDECLARECURSOR_H_ */
