/*******************************************************************************
 * ztqNamedCursor.h
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


#ifndef _ZTQ_NAMED_CURSOR_H_
#define _ZTQ_NAMED_CURSOR_H_ 1

/**
 * @file ztqNamedCursor.h
 * @brief Named Cursor Management
 */

/**
 * @defgroup ztqNamedCursor Named Cursor
 * @ingroup ztq
 * @{
 */

stlStatus ztqAddNamedCursor( ztqSqlStmt    * aZtqSqlStmt,
                             stlErrorStack * aErrorStack );

void ztqFindNamedCursor( stlChar         * aCursorName,
                         ztqNamedCursor ** aNamedCursor );

void ztqDestAllNamedCursor( stlErrorStack  * aErrorStack );


/** @} */

#endif /* _ZTQ_NAMED_CURSOR_H_ */
