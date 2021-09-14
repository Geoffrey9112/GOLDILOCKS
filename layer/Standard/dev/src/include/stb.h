/*******************************************************************************
 * stb.h
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


#ifndef _STB_H_
#define _STB_H_ 1

STL_BEGIN_DECLS

#if STC_HAVE_EXECINFO_H
#include <execinfo.h>
#endif

#if STC_HAVE_UNWIND_H
#include <unwind.h>
#endif
#if STC_HAVE_UWX_H
#include <uwx.h>
#endif

#ifdef STC_TARGET_OS_AIX
/* gcc 사용을 가정하고 있다. */
/* Support for bounded pointers.  */
#ifdef __BOUNDED_POINTERS__
#define BOUNDED_N(PTR, N) 				\
  ({ __typeof (PTR) __bounded _p_;			\
     __ptrvalue _p_ = __ptrlow _p_ = __ptrvalue (PTR);	\
     __ptrhigh _p_ = __ptrvalue _p_ + (N);		\
     _p_; })

#else /* !__BOUNDED_POINTERS__ */
#define __bounded	/* nothing */
#define __unbounded	/* nothing */
#define __ptrvalue	/* nothing */

#define BOUNDED_N(PTR, N)      (PTR)

#endif /* __BOUNDED_POINTERS__ */

#define BOUNDED_1(PTR) BOUNDED_N (PTR, 1)

typedef struct stbLayout
{
  struct stbLayout *__unbounded mNext;
  stlInt64                      mConditionRegister;
  void             *__unbounded mReturnAddress;
} stbLayout;

#endif  /* STC_TARGET_OS_AIX */

stlInt32 stbBacktrace( stlFile   * aFile,
                       void     ** aFrame,
                       stlInt32    aSize,
                       void      * aSigInfo,
                       void      * aContext );
void     stbBacktraceToFile( stlFile   * aFile,
                             void     ** aFrame,
                             stlInt32    aSize,
                             void      * aSigInfo,
                             void      * aContext );
void     stbBacktraceSymbolsToFile( void     ** aFrame,
                                    stlInt32    aSize,
                                    stlFile   * aFile );

void     stbBacktraceSymbolsToStr( void     ** aFrame,
                                   void      * aBuffer,
                                   stlInt32    aFrameSize,
                                   stlInt32    aBufferSize );

STL_END_DECLS

#endif /* _STB_H_ */
