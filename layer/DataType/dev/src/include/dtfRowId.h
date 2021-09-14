/*******************************************************************************
 * dtfRowId.h
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


#ifndef _DTF_ROWID_H_
#define _DTF_ROWID_H_ 1

/**
 * @file dtfRowId.h
 * @brief RowId for DataType Layer
 */

/**
 * @addtogroup dtfRowId RowId
 * @ingroup dtf
 * @{
 */

stlStatus dtfRowIdObjectId( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aEnv );

stlStatus dtfRowIdPageId( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfRowIdRowNumber( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv );

stlStatus dtfRowIdTablespaceId( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aEnv );

/** @} */

#endif /* _DTF_ROWID_H_ */
