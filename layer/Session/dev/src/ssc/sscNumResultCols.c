/*******************************************************************************
 * sscNumResultCols.c
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
 * @file sscNumResultCols.c
 * @brief Session Layer NumResultCols Command Internal Routines
 */


#include <qll.h>
#include <sslDef.h>

stlStatus sscNumResultCols( sslStatement * aStatement,
                            stlInt16     * aColumnCount,
                            sslEnv       * aEnv )
{
    stlInt32 sColumnCount;

    sColumnCount = qllGetTargetCount( &aStatement->mSqlStmt );

    if( aColumnCount != NULL )
    {
        *aColumnCount = (stlInt16)sColumnCount;
    }

    return STL_SUCCESS;
}
