/*******************************************************************************
 * cmlError.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlError.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>

/**
 * @file sclError.c
 * @brief Server Communication Layer Error Protocol Routines
 */

/**
 * @brief Server Communication  Layer 에러 테이블.
 */

stlErrorRecord gServerCommunicationErrorTable[] =
{
    {   /* SCL_ERRCODE_COMMUNICATION_LINK_FAILURE */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE,
        "Communication link failure"
    },
    {   /* SCL_ERRCODE_INVALID_COMMUNICATION_PROTOCOL */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_NO_SUBCLASS,
        "Invalid communication protocol"
    },
    {   /* SCL_ERRCODE_OUT_OF_CM_UNIT_MEMORY    */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Not enough cm unit memory"
    },
    {
        0,
        NULL
    }
};


