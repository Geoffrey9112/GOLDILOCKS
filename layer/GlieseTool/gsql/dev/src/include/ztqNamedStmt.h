/*******************************************************************************
 * ztqNamedStmt.h
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


#ifndef _ZTQ_NAMED_STMT_H_
#define _ZTQ_NAMED_STMT_H_ 1

/**
 * @file ztqNamedStmt.h
 * @brief Named Statement Definition
 */

/**
 * @defgroup ztqNamedStmt Named Statement
 * @ingroup ztq
 * @{
 */

void ztqInitNamedStmt( ztqNamedStmt * aNamedStmt );

stlStatus ztqDropNamedStmt( ztqNamedStmt  * aNamedStmt,
                            stlErrorStack * aErrorStack );

stlStatus ztqResetNamedStmt( ztqNamedStmt  * aNamedStmt,
                             stlErrorStack * aErrorStack );

stlStatus ztqAllocNamedStmt( stlChar       * aStmtName,
                             stlErrorStack * aErrorStack );

stlStatus ztqFreeNamedStmt( stlChar       * aStmtName,
                            stlErrorStack * aErrorStack );


stlStatus ztqFindNamedStmt( stlChar        * aStmtName,
                            ztqNamedStmt  ** aNamedStmt,
                            stlErrorStack  * aErrorStack );

void ztqDestAllNamedStmt( stlErrorStack  * aErrorStack );


/** @} */

#endif /* _ZTQ_NAMED_STMT_H_ */
