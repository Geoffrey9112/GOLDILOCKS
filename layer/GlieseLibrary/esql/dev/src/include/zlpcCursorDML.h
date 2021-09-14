/*******************************************************************************
 * zlpcCursorDML.h
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

#ifndef _ZLPCCURSORDML_H_
#define _ZLPCCURSORDML_H_ 1

/**
 * @file zlpcCursorDML.h
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
 * positioned Cursor DML
 */
stlStatus  zlpcPositionedCursorDML( zlplSqlContext * aSqlContext,
                                    sqlargs_t      * aSqlArgs );


#endif /* _ZLPCCURSORDML_H_ */
