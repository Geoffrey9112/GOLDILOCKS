/*******************************************************************************
 * ztqEditCommand.h
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


#ifndef _ZTQEDITCOMMAND_H_
#define _ZTQEDITCOMMAND_H_ 1

/**
 * @file ztqEditCommand.h
 * @brief Edit Command Definition
 */

/**
 * @addtogroup ztqCommand
 * @{
 */

stlStatus ztqEditCommand( stlChar       *aEditorName,
                          stlChar       *aSqlFilePath,
                          stlInt64       aHistoryNum,
                          stlAllocator  *aAllocator,
                          stlErrorStack *aErrorStack );

stlStatus ztqMakeEditBuferFile( stlChar       *aSqlFilePath,
                                stlChar       *aCommand,
                                stlErrorStack *aErrorStack );

stlStatus ztqRemoveTrailingSpace( stlChar     *aSqlBuffer );

stlStatus ztqGetEditedSql( stlChar       *aSqlFilePath,
                           stlChar      **aSqlBuffer,
                           stlAllocator  *aAllocator,
                           stlErrorStack *aErrorStack );

void ztqReleaseEditedSql( stlChar  *aSqlBuffer );


/** @} */

#endif /* _ZTQEDITCOMMAND_H_ */
