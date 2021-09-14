/*******************************************************************************
 * ztbeError.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztbeError.c 8784 2013-06-24 17:24:30Z mycomman $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>

stlErrorRecord gZtbeErrorTable[] =
{
    {   /* ZTBE_ERRCODE_OVERFLOW_FD */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "overflow fd"
    },
    {
        0,
        NULL
    }
};

