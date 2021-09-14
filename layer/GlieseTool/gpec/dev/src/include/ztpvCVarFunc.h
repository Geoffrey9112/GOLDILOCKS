/*******************************************************************************
 * ztpvCVarFunc.h
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


#ifndef _ZTPVCVARFUNC_H_
#define _ZTPVCVARFUNC_H_ 1

/**
 * @file ztpvCVarFunc.h
 * @brief C Preprocessor Cacluator Parser for Gliese Embedded SQL
 */

/**
 * @defgroup ztpvCVarFunc C Preprocessor  Cacluator Parser
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
#define YY_EXTRA_TYPE ztpCHostVarParseParam *
#endif

ztpHostVariable *ztpvMakeHostVarNode( ztpCHostVarParseParam  *aParam,
                                      ztpHostVarDesc          aHostVarDesc,
                                      stlChar                *aValue,
                                      ztpHostVariable        *aHostVariable );

stlChar *ztpvMakeString( ztpCHostVarParseParam *aParam,
                         stlInt32               aStartPos,
                         stlInt32               aEndPos );

ztpHostVariable *ztpvMakeHostVariable( stlAllocator  *aAllocator,
                                       stlErrorStack *aErrorStack,
                                       stlChar       *aBuffer,
                                       stlInt32       aStartPos,
                                       stlInt32      *aEndPos );

stlStatus ztpvCVarParseIt( stlAllocator     *aAllocator,
                           stlErrorStack    *aErrorStack,
                           stlChar          *aHostVarStr,
                           stlInt32         *aHostVarLen,
                           ztpHostVariable **aHostVariable );

stlInt32 ztpvCVarlex( YYSTYPE *, YYLTYPE *, void * );
stlInt32 ztpvCVarlex_init( void ** );
stlInt32 ztpvCVarlex_destroy( void * );
void     ztpvCVarset_extra( YY_EXTRA_TYPE, void * );
stlInt32 ztpvCVarparse( ztpCHostVarParseParam *, void * );

stlInt32 ztpvCVarerror( YYLTYPE                 * aLloc,
                        ztpCHostVarParseParam   * aParam,
                        void                    * aScanner,
                        const stlChar           * aMsg );


/** @} */

#endif /* _ZTPVCVARFUNC_H_ */
