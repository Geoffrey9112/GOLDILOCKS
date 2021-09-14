/*******************************************************************************
 * cmpProtocol.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmpProtocol.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <cmlDef.h>
#include <cmDef.h>
#include <cmpCursor.h>
#include <cmlGeneral.h>

/**
 * @file cmpProtocol.c
 * @brief Communication Layer Protocol Component Routines
 */

/**
 * @addtogroup cmpProtocol
 * @{
 */

stlStatus cmpReadErrorResult( cmlHandle       * aHandle,
                              stlBool         * aIgnored,
                              cmpCursor       * aCursor,
                              stlErrorStack   * aErrorStack )
{
    stlErrorData sErrorData;
    stlInt8      sErrorCount;
    stlInt32     sMessageLen;
    stlInt32     sDetailMessageLen;
    stlInt8      sErrorLevel;
    stlInt32     i;

    if( aIgnored != NULL )
    {
        *aIgnored = STL_FALSE;
    }
    
    CMP_READ_INT8( aHandle, aCursor, &sErrorLevel, aErrorStack );

    if( sErrorLevel > CML_RESULT_SUCCESS )
    {
        if( sErrorLevel == CML_RESULT_IGNORE )
        {
            STL_DASSERT( aIgnored != NULL );
            *aIgnored = STL_TRUE;
            STL_THROW( RAMP_FINISH );
        }
        
        CMP_READ_INT8( aHandle, aCursor, &sErrorCount, aErrorStack );

        for( i = 0; i < sErrorCount; i++ )
        {
            sErrorData.mErrorLevel = STL_ERROR_LEVEL_ABORT;
            sErrorData.mSystemErrorCode = 0;
            sErrorData.mDetailErrorMessage[0] = '\0';

            CMP_READ_INT32_ENDIAN( aHandle, aCursor, &sErrorData.mExternalErrorCode, aErrorStack );
            CMP_READ_INT32_ENDIAN( aHandle, aCursor, &sErrorData.mErrorCode, aErrorStack );
            CMP_READ_VAR_STRING_ENDIAN( aHandle,
                                        aCursor,
                                        sErrorData.mErrorMessage,
                                        sMessageLen,
                                        STL_MAX_ERROR_MESSAGE,
                                        aErrorStack );
            CMP_READ_VAR_STRING_ENDIAN( aHandle,
                                        aCursor,
                                        sErrorData.mDetailErrorMessage,
                                        sDetailMessageLen,
                                        STL_MAX_ERROR_MESSAGE,
                                        aErrorStack );

            stlPushErrorData( &sErrorData, aErrorStack );
        }

        STL_TRY( sErrorLevel <= CML_RESULT_WARNING );
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    (void)cmpEndReading( aHandle,
                         aCursor,
                         aErrorStack );

    return STL_FAILURE;
}

stlStatus cmpReadXaErrorResult( cmlHandle       * aHandle,
                                stlBool         * aIgnored,
                                cmpCursor       * aCursor,
                                stlInt32        * aXaError,
                                stlErrorStack   * aErrorStack )
{
    stlErrorData sErrorData;
    stlInt8      sErrorCount;
    stlInt32     sMessageLen;
    stlInt32     sDetailMessageLen;
    stlInt8      sErrorLevel;
    stlInt32     i;

    if( aIgnored != NULL )
    {
        *aIgnored = STL_FALSE;
    }
    
    CMP_READ_INT8( aHandle, aCursor, &sErrorLevel, aErrorStack );
    CMP_READ_INT32( aHandle, aCursor, aXaError, aErrorStack );

    if( sErrorLevel > CML_RESULT_SUCCESS )
    {
        if( sErrorLevel == CML_RESULT_IGNORE )
        {
            STL_DASSERT( aIgnored != NULL );
            *aIgnored = STL_TRUE;
            STL_THROW( RAMP_FINISH );
        }
        
        CMP_READ_INT8( aHandle, aCursor, &sErrorCount, aErrorStack );

        for( i = 0; i < sErrorCount; i++ )
        {
            sErrorData.mErrorLevel = STL_ERROR_LEVEL_ABORT;
            sErrorData.mSystemErrorCode = 0;
            sErrorData.mDetailErrorMessage[0] = '\0';

            CMP_READ_INT32_ENDIAN( aHandle, aCursor, &sErrorData.mExternalErrorCode, aErrorStack );
            CMP_READ_INT32_ENDIAN( aHandle, aCursor, &sErrorData.mErrorCode, aErrorStack );
            CMP_READ_VAR_STRING_ENDIAN( aHandle,
                                        aCursor,
                                        sErrorData.mErrorMessage,
                                        sMessageLen,
                                        STL_MAX_ERROR_MESSAGE,
                                        aErrorStack );
            CMP_READ_VAR_STRING_ENDIAN( aHandle,
                                        aCursor,
                                        sErrorData.mDetailErrorMessage,
                                        sDetailMessageLen,
                                        STL_MAX_ERROR_MESSAGE,
                                        aErrorStack );

            stlPushErrorData( &sErrorData, aErrorStack );
        }

        STL_TRY( sErrorLevel <= CML_RESULT_WARNING );
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    (void)cmpEndReading( aHandle,
                         aCursor,
                         aErrorStack );

    return STL_FAILURE;
}

/**
 * @brief 이 함수는 preHandshake에서도 불릴수 있음으로
 *        endian 고려해야 해서 CMP_WRITE_xxx_ENDIAN 사용함.
 */
stlStatus cmpWriteErrorResult( cmlHandle       * aHandle,
                               cmpCursor       * aCursor,
                               stlErrorStack   * aErrorStack )
{
    stlErrorData * sErrorData;
    stlInt8        sErrorCount;
    stlInt8        sErrorLevel = CML_RESULT_SUCCESS;
    stlInt16       sErrorIdx = -1;
    stlInt32       sMessageLen;
    stlInt32       sDetailMessageLen;

    if( STL_HAS_ERROR( aErrorStack ) == STL_FALSE )
    {
        CMP_WRITE_INT8( aHandle, aCursor, &sErrorLevel, aErrorStack );
    }
    else
    {
        sErrorCount = stlGetErrorStackDepth( aErrorStack );
        sErrorData  = stlGetLastErrorData( aErrorStack );

        sErrorLevel = CML_RESULT_WARNING;
        STL_DASSERT( sErrorData->mErrorLevel == STL_ERROR_LEVEL_WARNING );
        
        CMP_WRITE_INT8( aHandle, aCursor, &sErrorLevel, aErrorStack );
        CMP_WRITE_INT8( aHandle, aCursor, &sErrorCount, aErrorStack );

        sErrorData = stlGetFirstErrorData( aErrorStack, &sErrorIdx );

        while( sErrorData != NULL )
        {
            sMessageLen = stlStrlen( sErrorData->mErrorMessage );
            sDetailMessageLen = stlStrlen( sErrorData->mDetailErrorMessage );
            
            CMP_WRITE_INT32_ENDIAN( aHandle, aCursor, &sErrorData->mExternalErrorCode, aErrorStack );
            CMP_WRITE_INT32_ENDIAN( aHandle, aCursor, &sErrorData->mErrorCode, aErrorStack );
            CMP_WRITE_N_VAR_ENDIAN( aHandle, aCursor, sErrorData->mErrorMessage, sMessageLen, aErrorStack );
            CMP_WRITE_N_VAR_ENDIAN( aHandle, aCursor, sErrorData->mDetailErrorMessage, sDetailMessageLen, aErrorStack );

            sErrorData = stlGetNextErrorData( aErrorStack, &sErrorIdx );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
