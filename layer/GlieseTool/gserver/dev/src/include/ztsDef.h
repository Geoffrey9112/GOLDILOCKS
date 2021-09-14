/*******************************************************************************
 * ztsDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztsDef.h 12157 2014-04-23 03:50:31Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _ZTSDEF_H_
#define _ZTSDEF_H_ 1

/**
 * @file ztsDef.h
 * @brief Gliese Server Definitions
 */

/**
 * @defgroup  Gliese Server Routines
 * @{
 */
extern stlErrorRecord   gZtseErrorTable[];

#define ZTS_POLL_TIMEOUT                STL_SET_SEC_TIME(1)

#define ZTS_RECV_FD_TIMEOUT             STL_SET_SEC_TIME(10)


/** @} */

/** @} */

#endif /* _ZTSDEF_H_ */
