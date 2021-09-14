/*******************************************************************************
 * sscAuth.c
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
 * @file sscAuth.c
 * @brief Session Authentication Routines
 */

#include <qll.h>
#include <sslDef.h>

stlStatus sscAuthenticateUser( stlChar * aUserName,
                               stlChar * aEncryptedPasswd,
                               stlChar * aRoleName,
                               stlChar * aOldPassword,
                               stlChar * aNewPassword,
                               sslEnv  * aEnv )
{
    STL_TRY( qllLogin( aUserName,
                       aEncryptedPasswd,
                       aRoleName,
                       aOldPassword, /* aPlainOrgPassword */
                       aNewPassword, /* aPlainNewPassword */
                       QLL_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

