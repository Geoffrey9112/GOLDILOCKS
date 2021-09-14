/*******************************************************************************
 * qlgStmtInfo.h
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


#ifndef _QLG_STMT_INFO_H_
#define _QLG_STMT_INFO_H_ 1

/**
 * @file qlgStmtInfo.h
 * @brief SQL Processor Layer Statement Information Internal Routines
 */

stlInt32           qlgGetStmtTypeRelativeID( qllNodeType aType );
stlChar          * qlgGetStmtTypeKeyString( stlInt32 aRelStmtID );

stlInt32           qlgGetStatementAttr( qllNodeType aType );

stlBool  qlgIsSameStmtCursorProperty( qllStatement           * aSQLStmt,
                                      qllStmtCursorProperty  * aStmtProperty );

stlBool  qlgHasUncommittedObject( qllStatement * aSQLStmt,
                                  qllEnv       * aEnv );

#endif /* _QLG_STMT_INFO_H_ */
