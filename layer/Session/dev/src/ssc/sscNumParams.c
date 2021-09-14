/*******************************************************************************
 * sscNumParams.c
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
 * @file sscNumParams.c
 * @brief Session Layer NumParams Command Internal Routines
 */


#include <qll.h>
#include <sslDef.h>

stlStatus sscNumParams( sslStatement * aStatement,
                        stlInt16     * aParameterCount,
                        sslEnv       * aEnv )
{
    stlInt32      sParamCount;
    knlBindType * sParamIO;

    qllGetParamInfo( & aStatement->mSqlStmt,
                     & sParamCount,
                     & sParamIO );
    
    if( aParameterCount != NULL )
    {
        *aParameterCount = (stlInt16) sParamCount;
    }

    return STL_SUCCESS;
}
