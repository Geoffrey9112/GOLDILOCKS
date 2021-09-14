/*******************************************************************************
 * sth.h
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


#ifndef _STH_H_
#define _STH_H_ 1

#include <stlDef.h>

STL_BEGIN_DECLS

stlStatus sthCreate(stlShm        * aShm,
                    stlChar       * aShmName,
                    stlInt32        aShmKey,
                    void          * aShmAddr,
                    stlSize         aReqSize,
                    stlBool         aHugeTlb,
                    stlErrorStack * aErrorStack);

stlStatus sthAttach(stlShm        * aShm,
                    stlChar       * aShmName,
                    stlInt32        aShmKey,
                    void          * aShmAddr,
                    stlErrorStack * aErrorStack);

stlStatus sthDetach(stlShm        * aShm,
                    stlErrorStack * aErrorStack);

stlStatus sthDestroy(stlShm        * aShm,
                     stlErrorStack * aErrorStack);

void * sthGetBaseAddr(stlShm * aShm);

stlSize sthGetSize(stlShm * aShm);
stlBool sthSupportShareAddressShm();

STL_END_DECLS

#endif /* _STA_H_ */
