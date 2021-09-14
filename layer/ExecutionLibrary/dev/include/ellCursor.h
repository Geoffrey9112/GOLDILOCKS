/*******************************************************************************
 * ellCursor.h
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


#ifndef _ELL_CURSOR_H_
#define _ELL_CURSOR_H_ 1

/**
 * @file ellCursor.h
 * @brief Standing CURSOR Management
 */


/**
 * @defgroup ellCursorDeclDesc CURSOR Declaration Descriptor
 * @ingroup ellCursor
 * @{
 */

stlStatus ellInitNamedCursorSlot( ellSessionEnv * aSessEnv, ellEnv * aEnv );
stlStatus ellFiniNamedCursorSlot( ellSessionEnv * aSessEnv, ellEnv * aEnv );

stlStatus ellInsertStandingCursorDeclDesc( stlChar                 * aCursorName,
                                           stlInt64                  aProcID,
                                           stlChar                 * aCursorOrigin,
                                           ellCursorProperty       * aCursorProperty,
                                           ellCursorDeclDesc      ** aCursorDeclDesc,
                                           ellEnv                  * aEnv );

stlStatus ellInsertDBCCursorDeclDesc( stlChar                 * aCursorName,
                                      ellCursorDeclDesc      ** aCursorDeclDesc,
                                      ellEnv                  * aEnv );

stlStatus ellFindCursorDeclDesc( stlChar            * aCursorName,
                                 stlInt64             aProcID,
                                 ellCursorDeclDesc ** aCursorDeclDesc,
                                 ellEnv             * aEnv );

stlStatus ellRemoveCursorDeclDesc( ellCursorDeclDesc * aCursorDeclDesc,
                                   ellEnv            * aEnv );


/** @} ellCursorDeclDesc */





/**
 * @defgroup ellCursorInstDesc CURSOR Instance Descriptor
 * @ingroup ellCursor
 * @{
 */

stlStatus ellInsertCursorInstDesc( ellCursorDeclDesc  * aDeclDesc,
                                   stlInt32             aCursorDBCStmtSize,
                                   stlInt32             aCursorSQLStmtSize,
                                   ellCursorInstDesc ** aInstDesc,
                                   ellEnv             * aEnv );

stlStatus ellFindCursorInstDesc( stlChar            * aCursorName,
                                 stlInt64             aProcID,
                                 ellCursorInstDesc ** aCursorInstDesc,
                                 ellEnv             * aEnv );

stlStatus ellRemoveCursorInstDesc( ellCursorInstDesc * aCursorInstDesc,
                                   ellEnv            * aEnv );

stlRingHead * ellGetCursorOpenList( ellEnv * aEnv );

stlStatus ellGetFirstSQLCursor( stlUInt32           * aFirstBucketSeq,
                                ellCursorInstDesc  ** aCursorInstDesc,
                                ellEnv              * aEnv );

stlStatus ellGetNextSQLCursor( stlUInt32           * aBucketSeq,
                               ellCursorInstDesc  ** aCursorInstDesc,
                               ellEnv              * aEnv );

void ellCursorSetOpen( ellCursorInstDesc      * aCursorInstDesc,
                       smlStatement           * aCursorStmt,
                       void                   * aCursorDBCStmt,
                       void                   * aCursorSQLStmt,
                       ellCursorProperty      * aResultProperty,
                       stlChar                * aCursorQuery,
                       ellEnv                 * aEnv );

void ellCursorSetClosed( ellCursorInstDesc * aCursorInstDesc,
                         ellEnv            * aEnv );

stlBool ellCursorIsOpen( ellCursorInstDesc * aCursorInstDesc );

stlBool ellCursorHasOrigin( ellCursorInstDesc * aCursorInstDesc );




/** @} elsCursorInstDesc */



#endif /* _ELL_CURSOR_H_ */
