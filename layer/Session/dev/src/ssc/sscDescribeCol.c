/*******************************************************************************
 * sscDescribeCol.c
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
 * @file sscDescribeCol.c
 * @brief Session Layer DescribeCol Command Internal Routines
 */


#include <qll.h>
#include <sslDef.h>

stlStatus sscDescribeCol( sslStatement  * aStatement,
                          qllTarget    ** aTargetInfo,
                          sslEnv        * aEnv )
{
    if( aStatement->mTargetInfo == NULL )
    {
        STL_TRY( qllGetTargetInfo( &aStatement->mSqlStmt,
                                   &aStatement->mColumnRegionMem,
                                   aTargetInfo,
                                   QLL_ENV( aEnv ) ) == STL_SUCCESS );

        aStatement->mTargetInfo = *aTargetInfo;
    }
    else
    {
        *aTargetInfo = aStatement->mTargetInfo;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
