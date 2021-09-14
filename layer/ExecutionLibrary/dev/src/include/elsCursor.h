/*******************************************************************************
 * elsCursor.h
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


#ifndef _ELS_CURSOR_H_
#define _ELS_CURSOR_H_ 1

/**
 * @file elsCursor.h
 * @brief Execution Library Layer Cursor
 */

/**
 * @defgroup elsCursor CURSOR
 * @ingroup els
 * @{
 */

#define ELS_CURSOR_OBJECT_BUCKET_COUNT    (127)

/** @} elsCursor */


/**
 * @defgroup elsCursorDeclDesc CURSOR Declaration Descriptor
 * @ingroup elsCursor
 * @{
 */

/*
 * Hash 비교 함수
 */

stlUInt32  elsCursorKeyGetHashValue( void * aHashElement, stlUInt32 aBucketCount );
stlInt32   elsCursorKeyCompare( void * aSearchKey, void * aBaseKey );

/*
 * Cursor Declaration Descriptor 관리 함수 
 */

void elsInitCursorDeclDesc( ellCursorDeclDesc * aDeclDesc );

stlStatus elsInsertCursorDeclDesc( stlChar                 * aCursorName,
                                   stlInt64                  aProcID,
                                   ellCursorOriginType       aOriginType,
                                   stlChar                 * aCursorOrigin,
                                   ellCursorProperty       * aCursorProperty,
                                   ellCursorDeclDesc      ** aCursorDeclDesc,
                                   ellEnv                  * aEnv );

stlStatus elsFindCursorDeclDesc( stlChar            * aCursorName,
                                 stlInt64             aProcID,
                                 ellCursorDeclDesc ** aCursorDeclDesc,
                                 ellEnv             * aEnv );

stlStatus elsRemoveCursorDeclDesc( ellCursorDeclDesc * aCursorDeclDesc,
                                   ellEnv            * aEnv );


/** @} elsCursorDeclDesc */





/**
 * @defgroup elsCursorInstDesc CURSOR Instance Descriptor
 * @ingroup elsCursor
 * @{
 */

/*
 * Cursor Instance Descriptor 관리 함수 
 */

void elsInitCursorInstDesc( ellCursorInstDesc * aInstDesc );

stlStatus elsInsertCursorInstDesc( ellCursorDeclDesc  * aDeclDesc,
                                   stlInt32             aCursorDBCStmtSize,
                                   stlInt32             aCursorSQLStmtSize,
                                   ellCursorInstDesc ** aInstDesc,
                                   ellEnv             * aEnv );

stlStatus elsFindCursorInstDesc( stlChar            * aCursorName,
                                 stlInt64             aProcID,
                                 ellCursorInstDesc ** aCursorInstDesc,
                                 ellEnv             * aEnv );

stlStatus elsRemoveCursorInstDesc( ellCursorInstDesc * aCursorInstDesc,
                                   ellEnv            * aEnv );


/** @} elsCursorInstDesc */

    
#endif /* _ELS_CURSOR_H_ */


