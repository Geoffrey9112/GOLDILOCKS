/*******************************************************************************
 * zlp.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlp.h 9950 2013-10-18 06:00:12Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLP_H_
#define _ZLP_H_ 1

/**
 * @file zlp.h
 * @brief Gliese API Internal Prepare Routines.
 */

/**
 * @defgroup zlp Internal Prepare Routines
 * @{
 */

stlStatus zlpPrepare( zlsStmt       * aStmt,
                      stlChar       * aSql,
                      stlBool       * aSuccessWithInfo,
                      stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLP_H_ */
