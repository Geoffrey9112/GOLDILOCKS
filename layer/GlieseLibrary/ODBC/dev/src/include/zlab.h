/*******************************************************************************
 * zlab.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlab.h 7308 2013-02-19 05:52:47Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLAB_H_
#define _ZLAB_H_ 1

/**
 * @file zlab.h
 * @brief ODBC API Internal Boot Routines.
 */

/**
 * @defgroup zlb Internal Boot Routines
 * @{
 */

stlStatus zlabWarmup( stlErrorStack * aErrorStack );
stlStatus zlabCoolDown( stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLAB_H_ */
