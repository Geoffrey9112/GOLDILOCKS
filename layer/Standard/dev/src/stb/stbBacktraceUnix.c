/*******************************************************************************
 * stbBacktraceUnix.c
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

#include <stc.h>
#include <stlDef.h>
#include <stb.h>
#include <stlMemorys.h>
#include <stlFile.h>
#include <stlStrings.h>

#ifdef STC_TARGET_OS_HPUX

typedef struct stbFrame
{
    stlInt32    mLevel;
    stlInt32    mFrameMaxCount;
    void     ** mFrameAddr;
    stlFile   * mFile;
} stbFrame;
    
_Unwind_Reason_Code stbTrace( struct _Unwind_Context *aContext, void *aArg )
{
    stlChar       * sSymbol = NULL;
    stlChar       * sMod = NULL;
    stlUInt64       sOffset = 0;
    stlUInt64       sInstruction;
    stbFrame      * sFrame;
    stlChar         sOutputString[1024];
    stlSize         sWriteBytes;
    stlSize         sWrittenBytes;
    stlErrorStack   sErrorStack;

    sFrame = (stbFrame*)aArg;
    
    (void)uwx_get_reg( (struct uwx_env *)aContext,
                       UWX_REG_IP,
                       &sInstruction );

    if( sFrame->mLevel < sFrame->mFrameMaxCount )
    {
        sFrame->mFrameAddr[sFrame->mLevel] = (void*)((stlVarInt)sInstruction);
        sFrame->mLevel++;
    }

    if( sFrame->mFile != NULL )
    {
        (void)uwx_get_sym_info( (struct uwx_env *)aContext,
                                &sMod,
                                &sSymbol,
                                &sOffset );

        STL_INIT_ERROR_STACK( &sErrorStack );

        sWriteBytes = stlSnprintf( sOutputString,
                                   1024,
                                   "%s(%s+0x%lx)[0x%lx]\n",
                                   sMod,
                                   sSymbol,
                                   sOffset,
                                   sInstruction );
        stlWriteFile( sFrame->mFile,
                      sOutputString,
                      sWriteBytes,
                      &sWrittenBytes,
                      &sErrorStack );
    }
    
    return _URC_NO_REASON;
}

#elif STC_TARGET_OS_SOLARIS

#include <ucontext.h>

typedef struct stbFrame
{
    stlInt32    mLevel;
    stlInt32    mFrameMaxCount;
    void     ** mFrameAddr;
    stlFile   * mFile;
} stbFrame;

int stbTrace( uintptr_t aInstruction, int aSig, void *aArg )
{
    Dl_info         sDlIp;
    stbFrame      * sFrame;
    stlChar         sOutputString[1024];
    stlSize         sWriteBytes;
    stlSize         sWrittenBytes;
    stlErrorStack   sErrorStack;

    sFrame = (stbFrame*)aArg;
    
    if( dladdr((void *)aInstruction, &sDlIp) )
    {
        if( sFrame->mLevel < sFrame->mFrameMaxCount )
        {
            sFrame->mFrameAddr[sFrame->mLevel] = (void*)aInstruction;
            sFrame->mLevel++;
        }

        if( sFrame->mFile != NULL )
        {
            STL_INIT_ERROR_STACK( &sErrorStack );

            sWriteBytes = stlSnprintf( sOutputString,
                                       1024,
                                       "%s(%s+0x%lx)[0x%lx]\n",
                                       sDlIp.dli_fname,
                                       sDlIp.dli_sname,
                                       ( (stlUInt64)aInstruction - 
                                         (stlUInt64)sDlIp.dli_saddr ),
                                       aInstruction );

            stlWriteFile( sFrame->mFile,
                          sOutputString,
                          sWriteBytes,
                          &sWrittenBytes,
                          &sErrorStack );
        }

        return 0;
    }

    return -1;
}

#endif

inline stlInt32 stbBacktrace( stlFile   * aFile,
                              void     ** aFrame,
                              stlInt32    aSize,
                              void      * aSigInfo,
                              void      * aContext )
{
#ifdef STC_TARGET_OS_AIX

/* This is the stack layout we see with every stack frame.
   Note that every routine is required by the ABI to lay out the stack
   like this.

            +----------------+        +-----------------+
    %r1  -> | %r1 last frame--------> | %r1 last frame--->...  --> NULL
            |                |        |                 |
            | cr save        |        | cr save	  |
            |                |        |                 |
            | (unused)       |        | return address  |
            +----------------+        +-----------------+
*/
    /* gcc 사용을 가정하고 있다. */
    stbLayout  *sCurrent;
    stlInt32    sCount;

    if( aContext != NULL )
    {
        sCurrent = (stbLayout*)(((ucontext_t*)aContext)->uc_mcontext.jmp_context.gpr[1]);
    }
    else
    {
        /* Force gcc to spill LR. */
        asm volatile ("" : "=l"(sCurrent));
        
        /* Get the address on top-of-stack. */
        asm volatile ("ld %0,0(1)" : "=r"(sCurrent));
        sCurrent = BOUNDED_1 (sCurrent);
    }
        
    for( sCount = 0;
         sCurrent != NULL && sCount < aSize;
         sCurrent = BOUNDED_1 (sCurrent->mNext), sCount++ )
    {
#ifdef STL_DEBUG
        aFrame[sCount] = sCurrent->mReturnAddress;
#else
        aFrame[sCount] = sCurrent->mReturnAddress - 0x51;
#endif
    }

    /* It's possible the second-last stack frame can't return
       (that is, it's __libc_start_main), in which case
       the CRT startup code will have set its LR to 'NULL'.  */
    if( sCount > 0 && aFrame[sCount-1] == NULL )
    {
        sCount--;
    }

    return sCount;

#elif STC_TARGET_OS_HPUX

    struct stbFrame sFrame;

    sFrame.mLevel = 0;
    sFrame.mFrameAddr = aFrame;
    sFrame.mFile = aFile;
    sFrame.mFrameMaxCount = aSize;
    
    _Unwind_Backtrace( stbTrace, (void*)&sFrame );

    return sFrame.mLevel;

#elif STC_TARGET_OS_SOLARIS

    struct stbFrame sFrame;
    ucontext_t      sContext;

    sFrame.mLevel = 0;
    sFrame.mFrameAddr = aFrame;
    sFrame.mFile = aFile;
    sFrame.mFrameMaxCount = aSize;
    
    if( getcontext( &sContext ) == 0 )
    {
        walkcontext( &sContext, stbTrace, (void*)&sFrame );
    }

    return sFrame.mLevel;
#else

    return backtrace( aFrame, aSize );

#endif
}

inline void stbBacktraceSymbolsToFile( void     ** aFrame,
                                       stlInt32    aSize,
                                       stlFile   * aFile )
{
#if defined( STC_TARGET_OS_AIX ) || ( STC_TARGET_OS_HPUX )

    stlChar         sOutputString[1024];
    stlInt32        sCnt;
    stlSize         sWriteBytes;
    stlSize         sWrittenBytes;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK(&sErrorStack);

    for( sCnt = 0; sCnt < aSize; ++sCnt )
    {
        sWriteBytes = stlSnprintf( sOutputString, 
                                   1024, 
                                   "[CALLSTACK:0x%016lX] \n", 
                                   (stlInt64)(stlVarInt)(aFrame[sCnt]) );
        stlWriteFile( aFile,
                      sOutputString,
                      sWriteBytes,
                      &sWrittenBytes,
                      &sErrorStack );
    }

#elif STC_TARGET_OS_SOLARIS

    stlChar         sOutputString[1024];
    stlInt32        sCnt;
    stlSize         sWriteBytes;
    stlSize         sWrittenBytes;
    stlErrorStack   sErrorStack;
    Dl_info         sDlIp;

    STL_INIT_ERROR_STACK(&sErrorStack);

    for( sCnt = 0; sCnt < aSize; ++sCnt )
    {
        if( dladdr((void *)aFrame[sCnt], &sDlIp) )
	{
            sWriteBytes = stlSnprintf( sOutputString,
                                       1024,
                                       "%s(%s+0x%lx)[0x%lx]\n",
                                       sDlIp.dli_fname,
                                       sDlIp.dli_sname,
                                       ( (stlUInt64)aFrame[sCnt] - 
                                         (stlUInt64)sDlIp.dli_saddr ),
                                       aFrame[sCnt] );

            stlWriteFile( aFile,
                          sOutputString,
                          sWriteBytes,
                          &sWrittenBytes,
                          &sErrorStack );
	}
    }

#else

    backtrace_symbols_fd( aFrame, aSize, aFile->mFileDesc );

#endif
}

inline void stbBacktraceToFile( stlFile   * aFile,
                                void     ** aFrame,
                                stlInt32    aSize,
                                void      * aSigInfo,
                                void      * aContext )
{
#if defined( STC_TARGET_OS_AIX )
    
    stlChar         sOutputString[1024];
    stlInt32        sCnt;
    stlSize         sWriteBytes;
    stlSize         sWrittenBytes;
    stlErrorStack   sErrorStack;
    stlInt32        sFrameSize;

    sFrameSize = stbBacktrace( NULL,
                               aFrame,
                               aSize,
                               aSigInfo,
                               aContext );

    STL_INIT_ERROR_STACK(&sErrorStack);

    for( sCnt = 0; sCnt < sFrameSize; ++sCnt )
    {
        sWriteBytes = stlSnprintf( sOutputString, 
                                   1024, 
                                   "[CALLSTACK:0x%016lX] \n", 
                                   (stlInt64)(stlVarInt)(aFrame[sCnt]) );
        stlWriteFile( aFile,
                      sOutputString,
                      sWriteBytes,
                      &sWrittenBytes,
                      &sErrorStack );
    }
    
#elif defined( STC_TARGET_OS_HPUX ) || defined( STC_TARGET_OS_SOLARIS )
    
    stbBacktrace( aFile,
                  aFrame,
                  aSize,
                  aSigInfo,
                  aContext );
    
#else
    
    stlInt32  sFrameSize;
    
    sFrameSize = stbBacktrace( NULL,
                               aFrame,
                               aSize,
                               aSigInfo,
                               aContext );

    backtrace_symbols_fd( aFrame,
                          sFrameSize,
                          aFile->mFileDesc );
    
#endif
}

inline void stbBacktraceSymbolsToStr( void     ** aFrame,
                                      void      * aBuffer,
                                      stlInt32    aFrameSize,
                                      stlInt32    aBufferSize )
{
    stlInt32 sCnt = 0;

    if( aBufferSize > 0 )
    {
        ((stlChar*)aBuffer)[0] = '\0';
    }
    
#if defined( STC_TARGET_OS_AIX ) || ( STC_TARGET_OS_HPUX )

    stlChar sSymbol[1024];

    for( sCnt = 0; sCnt < aFrameSize; ++sCnt )
    {
        stlSnprintf( sSymbol, 1024, "[CALLSTACK:0x%016lX]\n", (stlInt64)(stlVarInt)(aFrame[sCnt]) );
        stlStrnncat( aBuffer, sSymbol, aBufferSize, STL_UINT32_MAX );
    }

#elif STC_TARGET_OS_SOLARIS

    Dl_info sDlIp;
    stlChar sOutputString[1024];

    for( sCnt = 0; sCnt < aFrameSize; ++sCnt )
    {
        if( dladdr((void *)aFrame[sCnt], &sDlIp) )
	{
            stlSnprintf( sOutputString,
                         1024,
                         "%s(%s+0x%lx)[0x%lx]\n",
                         sDlIp.dli_fname,
                         sDlIp.dli_sname,
                         ( (stlUInt64)aFrame[sCnt] - 
                           (stlUInt64)sDlIp.dli_saddr ),
                         aFrame[sCnt] );

            stlStrnncat( aBuffer, 
                         sOutputString, 
                         aBufferSize, 
                         STL_UINT32_MAX );
	}
    }

#else

    stlChar ** sSymbol;
    stlChar  * sUnknownSymbol = "UNKNOWN";
    
    while( sCnt < aFrameSize )
    {
        sSymbol = backtrace_symbols( &aFrame[sCnt], 1 );

        if( sSymbol != NULL )
        {
            stlStrnncat( aBuffer, sSymbol[0], aBufferSize, STL_UINT32_MAX );
            stlFreeHeap( sSymbol );
        }
        else
        {
            stlStrnncat( aBuffer, sUnknownSymbol, aBufferSize, STL_UINT32_MAX );
        }
        
        stlStrnncat( aBuffer, "\n", aBufferSize, STL_UINT32_MAX );
        sCnt += 1;
    }

#endif
}
