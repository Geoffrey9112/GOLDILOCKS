/*******************************************************************************
 * strLocaleUnix.c
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

/**
 * @file strLocaleUnix.c
 * @brief Standard Layer Locale Routines
 */

#include <stlDef.h>
#include <stc.h>

#if STC_HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if STC_HAVE_LANGINFO_H
#include <langinfo.h>
#endif
#if STC_HAVE_LOCALE_H
#include <locale.h>
#endif

stlChar * strGetDefaultCharacterSet()
{
    stlChar * sDefaultCharacterSet = "ANSI_X3.4-1968";
    
#if defined(STC_HAVE_SETLOCALE) && defined(STC_HAVE_NL_LANGINFO)

    stlChar * sLocale = NULL;
    stlChar * sLangInfo = NULL;

    sLocale = setlocale( LC_CTYPE, "" );
    sLangInfo = nl_langinfo( CODESET );

    if( sLocale && sLangInfo )
    {
        sDefaultCharacterSet = sLangInfo;
    }
#endif

    return sDefaultCharacterSet;
}
