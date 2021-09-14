/*******************************************************************************
 * zlcxExecute4Imp.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlcxExecute4Imp.c 12017 2014-04-15 05:44:19Z mae113 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <cml.h>
#include <goldilocks.h>

#include <zlDef.h>
#include <zle.h>

/**
 * @file zlcxExecute4Imp.c
 * @brief ODBC API Internal Execute Routines.
 */

/**
 * @addtogroup zlxExecute
 * @{
 */

stlStatus zlcxExecute4Imp( zlcDbc             * aDbc,
                           zlsStmt            * aStmt,
                           stlUInt16            aParameterCount,
                           dtlValueBlockList ** aParameterData, 
                           stlInt64           * aAffectedRowCount,
                           stlBool            * aSuccessWithInfo,
                           stlErrorStack      * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLE_ERRCODE_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

/** @} */
