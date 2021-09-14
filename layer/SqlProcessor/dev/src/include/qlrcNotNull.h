/*******************************************************************************
 * qlrcNotNull.h
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


#ifndef _QLRC_NOT_NULL_H_
#define _QLRC_NOT_NULL_H_ 1

/**
 * @file qlrcNotNull.h
 * @brief SQL Processor Layer NOT NULL constraint
 */

void qlrcCheckNotNull4Insert( knlValueBlockList * aNewValueBlockList,
                              stlInt32          * aCollisionCount,
                              stlInt32          * aKeyViolPos );

void qlrcCheckNotNull4Delete( stlBool           * aIsDeleted,
                              knlValueBlockList * aOldValueBlockList,
                              stlInt32          * aCollisionCount );

void qlrcCheckNotNull4Update( stlBool           * aIsUpdated,
                              knlValueBlockList * aOldValueBlockList,
                              knlValueBlockList * aNewValueBlockList,
                              stlInt32          * aCollisionCount,
                              stlInt32          * aKeyViolPos );


#endif /* _QLRC_NOT_NULL_H_ */
