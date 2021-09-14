/*******************************************************************************
 * ztpfIncludeFileFunc.h
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


#ifndef _ZTPFINCLUDEFILEFUNC_H_
#define _ZTPFINCLUDEFILEFUNC_H_ 1

/**
 * @file ztpfIncludeFileFunc.h
 * @brief Embedded SQL path parser
 */

/**
 * @defgroup ztpfIncludeFileFunc Embedded SQL include file path parser
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
#define YY_EXTRA_TYPE ztpIncludeFileParseParam *
#endif

/*
 * File path를 parsing
 * EXEC SQL INCLUDE ...
 */
stlStatus ztpfIncludeFileParseIt( stlAllocator     *aAllocator,
                                  stlErrorStack    *aErrorStack,
                                  stlChar          *aPathStr,
                                  stlInt32         *aPathLen,
                                  stlChar         **aPathDesc );

stlInt32 ztpfIncludeFilelex( YYSTYPE *, YYLTYPE *, void * );
stlInt32 ztpfIncludeFilelex_init( void ** );
stlInt32 ztpfIncludeFilelex_destroy( void * );
void     ztpfIncludeFileset_extra( YY_EXTRA_TYPE, void * );
stlInt32 ztpfIncludeFileparse( YY_EXTRA_TYPE, void * );

stlInt32 ztpfIncludeFileerror( YYLTYPE                    * aLloc,
                               ztpIncludeFileParseParam   * aParam,
                               void                       * aScanner,
                               const stlChar              * aMsg );


/** @} */

#endif /* _ZTPFINCLUDEFILEFUNC_H_ */
