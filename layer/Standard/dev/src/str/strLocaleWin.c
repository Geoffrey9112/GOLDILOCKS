/*******************************************************************************
 * strLocaleWin.c
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
 * @file strLocaleWin.c
 * @brief Standard Layer Locale Routines
 */

#if STC_HAVE_WINDOWS_H
#include <Windows.h>
#endif

#include <stlDef.h>
#include <stlStrings.h>

stlChar gDefaultCharacterSet[64];

stlChar * strGetDefaultCharacterSet()
{
    LCID sLocale = GetThreadLocale();
	
    if (GetLocaleInfo(sLocale, LOCALE_IDEFAULTANSICODEPAGE, gDefaultCharacterSet + 2, 64 - 2) > 0)
    {
        gDefaultCharacterSet[0] = 'C';
        gDefaultCharacterSet[1] = 'P';
    }
    else
    {
        stlSnprintf(gDefaultCharacterSet,
                    STL_SIZEOF(gDefaultCharacterSet),
                    "CP%u",
                    GetACP());
    }

    return gDefaultCharacterSet;
}
