/*******************************************************************************
 * zlad.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlad.h 7311 2013-02-19 09:56:56Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLAD_H_
#define _ZLAD_H_ 1

/**
 * @file zlad.h
 * @brief Gliese API Internal Diagnostic/Descriptor Routines.
 */

/**
 * @defgroup zlad Internal Diagnostic/Descriptor Routines
 * @{
 */

/*
 * Descriptor
 */

stlStatus zladDescBuildResult( zlcDbc        * aDbc,
                               zlsStmt       * aStmt,
                               stlBool         aNeedResultCols,
                               stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLAD_H_ */
