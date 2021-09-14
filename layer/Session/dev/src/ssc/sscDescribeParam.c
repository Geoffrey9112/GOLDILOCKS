/*******************************************************************************
 * sscDescribeParam.c
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
 * @file sscDescribeParam.c
 * @brief Session Layer DescribeParam Command Internal Routines
 */


#include <qll.h>
#include <sslDef.h>

stlStatus sscDescribeParam( sslStatement * aStatement,
                            stlInt32       aParamIdx,
                            stlInt16     * aInputOutputType,
                            sslEnv       * aEnv )
{
    stlInt32      sParameterCount = 0;
    knlBindType * sParameterType  = NULL;

    qllGetParamInfo( &aStatement->mSqlStmt,
                     &sParameterCount,
                     &sParameterType );

    STL_TRY_THROW( aParamIdx <= sParameterCount, RAMP_ERR_INVALID_DESCRIPTOR_INDEX );
    STL_TRY_THROW( sParameterType != NULL,       RAMP_ERR_INVALID_DESCRIPTOR_INDEX );

    switch( sParameterType[aParamIdx-1] )
    {
        case KNL_BIND_TYPE_IN :
            *aInputOutputType = SSL_PARAM_INPUT;
            break;
        case KNL_BIND_TYPE_OUT :
            *aInputOutputType = SSL_PARAM_OUTPUT;
            break;
        case KNL_BIND_TYPE_INOUT :
            *aInputOutputType = SSL_PARAM_INPUT_OUTPUT;
            break;
        default :
            STL_DASSERT( STL_FALSE );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SSL_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}
