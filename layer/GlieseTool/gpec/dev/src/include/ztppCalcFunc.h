/*******************************************************************************
 * ztppCalcFunc.h
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


#ifndef _ZTPPCALCFUNC_H_
#define _ZTPPCALCFUNC_H_ 1

/**
 * @file ztppCalcFunc.h
 * @brief C Preprocessor Cacluator Parser for Gliese Embedded SQL
 */

/**
 * @defgroup ztppCalcFunc C Preprocessor  Cacluator Parser
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
#define YY_EXTRA_TYPE ztpCalcParseParam *
#endif

ztpCalcResult *ztppMakeCalcResult( ztpCalcParseParam  *aParam,
                                   stlInt64            aValue );

ztpPPParameterList *ztppCalcParamList( ztpCalcParseParam   *aParam,
                                       ztpPPParameterList  *aParameterList,
                                       stlInt64             aValue );


stlStatus ztppCalcParseIt( stlAllocator  *aAllocator,
                           stlErrorStack *aErrorStack,
                           stlChar       *aExprStr,
                           stlInt32       aExprLen,
                           stlInt64      *aResult );

stlInt32 ztppCalclex( YYSTYPE *, YYLTYPE *, void * );
stlInt32 ztppCalclex_init( void ** );
stlInt32 ztppCalclex_destroy( void * );
void     ztppCalcset_extra( YY_EXTRA_TYPE, void * );
stlInt32 ztppCalcparse( ztpCalcParseParam *, void * );

stlInt32 ztppCalcerror( YYLTYPE             * aLloc,
                        ztpCalcParseParam   * aParam,
                        void                * aScanner,
                        const stlChar       * aMsg );


/** @} */

#endif /* _ZTPPCALCFUNC_H_ */
