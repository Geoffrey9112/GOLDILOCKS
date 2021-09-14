/*******************************************************************************
 * ztleError.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztleError.c 8784 2013-06-24 17:24:30Z mycomman $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>

stlErrorRecord gZtieErrorTable[] =
{
    {   /* ZTIE_ERRCODE_ALREADY_START */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Dispatcher is already started."
    },
    {   /* ZTIE_ERRCODE_OVERFLOW_FD */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "overflow fd"
    },
    {
        0,
        NULL
    }
};

