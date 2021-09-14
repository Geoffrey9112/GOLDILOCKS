/*******************************************************************************
 * ztdCmdParser.h
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZTD_CMD_PARSER_H_
#define _ZTD_CMD_PARSER_H_ 1

/**
 * @file ztdCmdParser.h
 * @brief CmdParser Definition
 */

/**
 * @defgroup ztdCmdParser
 * @ingroup ztd
 * @{
 */

#ifndef YYSTYPE
#define YYSTYPE     void *
#endif
#ifndef YYLTYPE
#define YYLTYPE     stlInt32
#endif
#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE ztdCmdParseParam *
#endif

stlChar * ztdCmdAllocBuffer( ztdCmdParseParam * aParam,
                             stlSize            aSize );

void ztdCmdCheckQualifier( ztdCmdParseParam * aParam,
                           stlChar          * aStr );

stlInt32 ztdCmderror( YYLTYPE          * aLloc,
                      ztdCmdParseParam * aParam,
                      void             * aScanner,
                      const stlChar    * aMsg );
    
stlStatus ztdCmdParseIt( stlChar        * aBuffer,
                         stlAllocator   * aAllocator,
                         ztdControlInfo * aControlInfo,
                         stlErrorStack  * aErrorStack );

stlInt32 ztdCmdlex( YYSTYPE *, YYLTYPE *, void * );

stlInt32 ztdCmdlex_init( void ** );

stlInt32 ztdCmdlex_destroy( void *);

void     ztdCmdset_extra( YY_EXTRA_TYPE, void * );

stlInt32 ztdCmdparse( ztdCmdParseParam *, void * );

/** @} */
#endif /* _ZTD_CMDPARSER_H_ */
