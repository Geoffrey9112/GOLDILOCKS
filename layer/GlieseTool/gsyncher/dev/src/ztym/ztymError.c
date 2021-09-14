/*******************************************************************************
 * ztymError.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>

stlErrorRecord gZtyErrorTable[] =
{
    {   /* ZTY_ERRCODE_INVALID_PHASE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid phase(%s): executable phase is %s"
    },
    {   /* ZTY_ERRCODE_INVALID_HOME */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "GOLDILOCKS_DATA system environment is invalid"
    },
    {   /* ZTY_ERRCODE_GMASTER_ACTIVE  */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "gmaster is active"
    },
    {
        0,
        NULL
    }
};
