/*******************************************************************************
 * cmpCursor.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmpCursor.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <cmlDef.h>
#include <cmDef.h>
#include <cmlGeneral.h>

/**
 * @file cmpCursor.c
 * @brief Communication Layer Cursor Component Routines
 */

/**
 * @addtogroup cmpCursor
 * @{
 */

stlStatus cmpBeginReading( cmlHandle       * aHandle,
                           cmpCursor       * aCursor,
                           stlErrorStack   * aErrorStack )
{
    cmlProtocolSentence * sProtocolSentence = aHandle->mProtocolSentence;

    aCursor->mCurPos = sProtocolSentence->mReadProtocolPos;
    aCursor->mEndPos = sProtocolSentence->mReadEndPos;
    
    return STL_SUCCESS;
}

stlStatus cmpEndReading( cmlHandle       * aHandle,
                         cmpCursor       * aCursor,
                         stlErrorStack   * aErrorStack )
{
    cmlProtocolSentence * sProtocolSentence = aHandle->mProtocolSentence;

    sProtocolSentence->mReadProtocolPos = aCursor->mCurPos;
    
    return STL_SUCCESS;
}

stlStatus cmpBeginWriting( cmlHandle       * aHandle,
                           cmpCursor       * aCursor,
                           stlErrorStack   * aErrorStack )
{
    cmlProtocolSentence * sProtocolSentence = aHandle->mProtocolSentence;

    aCursor->mCurPos = sProtocolSentence->mWriteProtocolPos;
    aCursor->mEndPos = sProtocolSentence->mWriteStartPos + sProtocolSentence->mMemoryMgr->mPacketBufferSize;
    
    return STL_SUCCESS;
}

stlStatus cmpEndWriting( cmlHandle       * aHandle,
                         cmpCursor       * aCursor,
                         stlErrorStack   * aErrorStack )
{
    cmlProtocolSentence * sProtocolSentence = aHandle->mProtocolSentence;

    sProtocolSentence->mWriteProtocolPos = aCursor->mCurPos;

    return STL_SUCCESS;
}

stlStatus cmpReadNBytes( cmlHandle       * aHandle,
                         cmpCursor       * aCursor,
                         stlChar         * aValue,
                         stlInt32          aLength,
                         stlErrorStack   * aErrorStack )
{
    stlInt32 sLen; 

    while( aLength > 0 )
    {
        sLen = STL_MIN( CMP_READABLE_SIZE( aCursor ), aLength );

        STL_WRITE_BYTES( aValue, aCursor->mCurPos, sLen );

        aLength -= sLen;
        aCursor->mCurPos += sLen;
        aValue += sLen;

        if( aLength == 0 )
        {
            break;
        }

        STL_TRY( cmpEndReading( aHandle, aCursor, aErrorStack ) == STL_SUCCESS );
        STL_TRY( cmlRecvPacket( aHandle, aErrorStack ) == STL_SUCCESS );
        STL_TRY( cmpBeginReading( aHandle, aCursor, aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus cmpWriteNBytes( cmlHandle       * aHandle,
                          cmpCursor       * aCursor,
                          stlChar         * aValue,
                          stlInt32          aLength,
                          stlErrorStack   * aErrorStack )
{
    stlInt32 sLen;

    while( aLength > 0 )
    {
        sLen = STL_MIN( CMP_WRITABLE_SIZE( aCursor ), aLength );

        STL_WRITE_BYTES( aCursor->mCurPos, aValue, sLen );

        aLength -= sLen;
        aCursor->mCurPos += sLen;
        aValue += sLen;

        if( aLength == 0 )
        {
            break;
        }

        STL_TRY( cmpEndWriting( aHandle, aCursor, aErrorStack ) == STL_SUCCESS );
        STL_TRY( cmlFlushPacket( aHandle, aErrorStack ) == STL_SUCCESS );
        STL_TRY( cmpBeginWriting( aHandle, aCursor, aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
