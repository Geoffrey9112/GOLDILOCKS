/*******************************************************************************
 * ztpfPathControl.h
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


#ifndef _ZTPFPATHCONTROL_H_
#define _ZTPFPATHCONTROL_H_ 1

/**
 * @file ztpfPathControl.h
 * @brief File/Path manipulation Routines
 */

/**
 * @defgroup ztpfPathControl File/Path manipulation Routines
 * @ingroup ztp
 * @{
 */

stlStatus ztpfSearchFile(stlChar         *aSourceFilePath,
                         stlChar         *aFileName,
                         stlChar         *aSearchFileName,
                         stlChar         *aSearchFilePath,
                         stlBool         *aIsFound,
                         stlErrorStack   *aErrorStack);

stlStatus ztpfDirname( stlChar *aPath,
                       stlChar *aDirname );

/** @} */

#endif /* _ZTPFPATHCONTROL_H_ */
