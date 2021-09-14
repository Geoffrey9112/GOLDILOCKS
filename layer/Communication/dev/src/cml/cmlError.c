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
#include <dtl.h>
#include <cmlDef.h>
#include <cmDef.h>
#include <cmlProtocol.h>
#include <cmlGeneral.h>
#include <cmpCursor.h>
#include <cmpProtocol.h>

/**
 * @file cmlError.c
 * @brief Communication Layer Error Protocol Routines
 */

/**
 * @brief Kernel Layer 에러 테이블.
 * 내부에러에 대응되는 외부에러와 에러 메세지로 구성됨.
 */
stlErrorRecord gCommunicationErrorTable[] =
{
    {/* CML_ERRCODE_COMMUNICATION_LINK_FAILURE */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE,
        "Communication link failure"
    },
    {/* CML_ERRCODE_INVALID_COMMUNICATION_PROTOCOL */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE,
        "Invalid communication protocol"
    },
    {/* CML_ERRCODE_PROPERTY_INVALID_HOME_DIR */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NO_SUBCLASS,
        "Invalid GOLDILOCKS_DATA Directory"
    },
    {/* CML_ERRCODE_NOT_SUPPORT */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NO_SUBCLASS,
        "not supported function"
    },
    {/* CML_ERRCODE_INVALID_INTERPROCESS_PROTOCOL */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NO_SUBCLASS,
        "Invalid interprocess protocol"
    },
    {
        0,
        NULL
    }
};



/**
 * @brief Error Result Protocol을 기록한다.
 * @param[in] aHandle          Communication Handle
 * @param[in] aResultType      Result Type
 * @param[in] aErrorStack      기록될 error stack
 *
 * @par protocol sequence:
 * @code
 *
 * - result (t)ype  : result type         2 bytes
 * - erro(r) level  : error level         1 byte
 * - error (c)ount  : session identifier  1 bytes
 * - (s)ql state    : sql state           4 bytes
 * - n(a)tive error : native error        4 bytes
 * - (m)essage text : message text        variable
 *
 * field sequence : t-r-c-[s-a-m]^c
 *
 * @endcode
 */
stlStatus cmlWriteErrorResult( cmlHandle       * aHandle,
                               cmlCommandType    aResultType,
                               stlErrorStack   * aErrorStack )
{
    cmpCursor      sCursor;
    stlErrorData * sErrorData;
    stlInt8        sErrorCount;
    stlInt8        sErrorLevel;
    stlInt16       sResultType = aResultType;
    stlInt32       sMessageLen;
    stlInt32       sDetailMessageLen;
    stlInt32       i;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS);

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );

    sErrorCount = stlGetErrorStackDepth( aErrorStack );

    sErrorLevel = CML_RESULT_ABORT;

    CMP_WRITE_INT8( aHandle, &sCursor, &sErrorLevel, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &sErrorCount, aErrorStack );

    for( i = 0; i < sErrorCount; i++ )
    {
        sErrorData = &(aErrorStack->mErrorData[i]);
        sMessageLen = stlStrlen( sErrorData->mErrorMessage );
        sDetailMessageLen = stlStrlen( sErrorData->mDetailErrorMessage );

        CMP_WRITE_INT32( aHandle, &sCursor, &sErrorData->mExternalErrorCode, aErrorStack );
        CMP_WRITE_INT32( aHandle, &sCursor, &sErrorData->mErrorCode, aErrorStack );
        CMP_WRITE_N_VAR( aHandle, &sCursor, sErrorData->mErrorMessage, sMessageLen, aErrorStack );
        CMP_WRITE_N_VAR( aHandle, &sCursor, sErrorData->mDetailErrorMessage, sDetailMessageLen, aErrorStack );
    }

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
