/*******************************************************************************
 * ztss.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztss.h 12157 2014-04-23 03:50:31Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _ZTSS_H_
#define _ZTSS_H_ 1

/**
 * @file ztss.h
 * @brief Gliese Server shared-server routine
 */

stlStatus ztssProcessShared( stlInt64  * aMsgKey,
                             stlInt32    aIndex,
                             sllEnv    * aEnv );



#endif /* _ZTSS_H_ */
