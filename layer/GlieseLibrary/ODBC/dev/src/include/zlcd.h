/*******************************************************************************
 * zlcd.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlcd.h 7311 2013-02-19 09:56:56Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLCD_H_
#define _ZLCD_H_ 1

/**
 * @file zlcd.h
 * @brief Gliese API Internal Diagnostic/Descriptor Routines.
 */

/**
 * @defgroup zlcd Internal Diagnostic/Descriptor Routines
 * @{
 */

/*
 * Descriptor
 */

stlStatus zlcdDescBuildResult( zlcDbc        * aDbc,
                               zlsStmt       * aStmt,
                               stlBool         aNeedResultCols,
                               stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLCD_H_ */
