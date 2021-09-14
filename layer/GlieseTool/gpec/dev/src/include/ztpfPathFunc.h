/*******************************************************************************
 * ztpfPathFunc.h
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


#ifndef _ZTPFPATHFUNC_H_
#define _ZTPFPATHFUNC_H_ 1

/**
 * @file ztpfPathFunc.h
 * @brief Embedded SQL path parser
 */

/**
 * @defgroup ztpfPathFunc Embedded SQL path parser
 * @ingroup ztp
 * @{
 */

#ifndef YYSTYPE
#define YYSTYPE       void *
#endif
#ifndef YYLTYPE
#define YYLTYPE       stlInt32
#endif

#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE ztpPathParseParam *
#endif

/*
 * Path list를 parsing
 * EXEC SQL OPTION ( INCLUDE = ... )
 */
stlStatus ztpfPathParseIt( stlAllocator     *aAllocator,
                           stlErrorStack    *aErrorStack,
                           stlChar          *aPathStr,
                           stlInt32         *aPathLen,
                           ztpIncludePath  **aPathDesc );

stlInt32 ztpfPathlex( YYSTYPE *, YYLTYPE *, void * );
stlInt32 ztpfPathlex_init( void ** );
stlInt32 ztpfPathlex_destroy( void * );
void     ztpfPathset_extra( YY_EXTRA_TYPE, void * );
stlInt32 ztpfPathparse( ztpPathParseParam *, void * );

stlInt32 ztpfPatherror( YYLTYPE                 * aLloc,
                        ztpPathParseParam       * aParam,
                        void                    * aScanner,
                        const stlChar           * aMsg );

stlStatus ztpfMakeIncludePath( ztpPathParseParam  *aParam,
                               stlChar            *aDirName );

/** @} */

#endif /* _ZTPFPATHFUNC_H_ */
