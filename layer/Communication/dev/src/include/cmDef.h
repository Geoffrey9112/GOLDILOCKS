/*******************************************************************************
 * cmDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmDef.h 6781 2012-12-20 03:09:11Z kyungoh $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _CMDEF_H_
#define _CMDEF_H_ 1

/**
 * @file cmDef.h
 * @brief Communication Layer Internal Definitions
 */

/**
 * @defgroup cmInternal Communication Layer Internal Routines
 * @{
 */

extern stlErrorRecord  gCommunicationErrorTable[CML_MAX_ERROR + 1];


/**
 * @addtogroup cmg
 * @{
 */

#define CMG_PACKET_HEADER_SIZE ( CML_PACKET_HEADER_SIZE )
#define CMG_MTU_SIZE           ( CML_MTU_SIZE )

/* 1byte중 첫번째 bit는 period여부, 나머지 7bit는 sequence 이다 */
#define CMP_WRITE_PERIOD_SEQ( aHandle, aDest, aPeriod )                \
{                                                                      \
    (aDest)[0] = (*(aPeriod) | (aHandle)->mSendSequence);              \
    if( (*(stlUInt8 *)(aPeriod) & CML_PERIOD_MASK) == CML_PERIOD_END ) \
    {                                                                  \
        (aHandle)->mSendSequence = ((aHandle)->mSendSequence + 1) & 0x7f;\
    }                                                                  \
}

#define CMP_READ_PERIOD_SEQ( aHandle, aPeriod, aSrc )                                   \
{                                                                                       \
    STL_DASSERT( (*(stlUInt8 *)(aSrc) & CML_PERIOD_SEQ_MASK) == (aHandle)->mRecvSequence ); \
    CML_GET_PERIOD( *(aPeriod), *(aSrc));                                                 \
    if( (*(stlUInt8 *)(aSrc) & CML_PERIOD_MASK) == CML_PERIOD_END )                     \
    {                                                                                   \
        (aHandle)->mRecvSequence = ((aHandle)->mRecvSequence + 1) & 0x7f;\
    }                                                                                   \
}

/* header는 8byte중 4byte payload size와 1byte period 를 사용한다 */
#define CMG_WRITE_HEADER( aHandle, aPayloadSize, aPeriod )                                      \
    {                                                                                           \
        if( CMP_IS_EQUAL_ENDIAN( (aHandle) ) == STL_FALSE )                                       \
        {                                                                                       \
            CMP_WRITE_CONVERT_ENDIAN_INT32( (aHandle)->mProtocolSentence->mWriteStartPos, (aPayloadSize) );          \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            STL_WRITE_INT32( (aHandle)->mProtocolSentence->mWriteStartPos, (aPayloadSize) );    \
        }                                                                                       \
        CMP_WRITE_PERIOD_SEQ( (aHandle), (aHandle)->mProtocolSentence->mWriteStartPos + 4, (aPeriod) );\
        CMP_HEADER_INIT( (aHandle) );                                                           \
    }



#define CMG_READ_HEADER( aHandle, aPayloadSize, aPeriod )                                       \
    {                                                                                           \
        if( CMP_IS_EQUAL_ENDIAN( (aHandle) ) == STL_FALSE )                                     \
        {                                                                                       \
            CMP_WRITE_CONVERT_ENDIAN_INT32( (aPayloadSize), (aHandle)->mProtocolSentence->mReadStartPos );           \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            STL_WRITE_INT32( (aPayloadSize), (aHandle)->mProtocolSentence->mReadStartPos );     \
        }                                                                                       \
        CMP_READ_PERIOD_SEQ( (aHandle), (aPeriod), (aHandle)->mProtocolSentence->mReadStartPos + 4 ); \
    }


/** @} */

/**
 * @addtogroup cmp
 * @{
 */

#define CMP_FETCH_COMMAND_SIZE       ( 2 + 1 + 8 + 2 + 8 + 4 )
#define CMP_FETCH_TRAIL_FIELD_BYTES  ( 1 )

#define CMP_2BYTE_LEN_FLAG           ( 0xFE )
#define CMP_4BYTE_LEN_FLAG           ( 0xFF )
#define CMP_1BYTE_LEN_MAX            ( 0xFD )
#define CMP_2BYTE_LEN_MAX            ( STL_INT16_MAX )

#define CMP_VAR_INT_1BYTE_MIN        ( -125 ) /* 0x83 */
#define CMP_VAR_INT_1BYTE_MAX        ( 0x7F )
#define CMP_VAR_INT_2BYTE_FLAG       ( -126 ) /* 0x82 */
#define CMP_VAR_INT_4BYTE_FLAG       ( -127 ) /* 0x81 */
#define CMP_VAR_INT_8BYTE_FLAG       ( -128 ) /* 0x80 */

#define CMP_STATEMENT_ID_1BYTE_MAX   ( 0xFC )
#define CMP_STATEMENT_ID_2BYTE_FLAG  ( 0xFD )
#define CMP_STATEMENT_ID_4BYTE_FLAG  ( 0xFE )
#define CMP_STATEMENT_ID_8BYTE_FLAG  ( 0xFF )

typedef struct cmpCursor
{
    stlChar   *mCurPos;
    stlChar   *mEndPos;
    stlInt16   mInt16;
    stlInt32   mInt32;
} cmpCursor;

#define CMP_WRITE_CONVERT_ENDIAN_INT32( aDesc, aSrc )   \
    {                                                   \
        ((stlChar*)(aDesc))[0] = ((stlChar*)(aSrc))[3]; \
        ((stlChar*)(aDesc))[1] = ((stlChar*)(aSrc))[2]; \
        ((stlChar*)(aDesc))[2] = ((stlChar*)(aSrc))[1]; \
        ((stlChar*)(aDesc))[3] = ((stlChar*)(aSrc))[0]; \
    }

#define CMP_WRITE_CONVERT_ENDIAN_INT16( aDesc, aSrc )   \
    {                                                   \
        ((stlChar*)(aDesc))[0] = ((stlChar*)(aSrc))[1]; \
        ((stlChar*)(aDesc))[1] = ((stlChar*)(aSrc))[0]; \
    }

#ifdef STL_VALGRIND
#define CMP_HEADER_INIT( aHandle )                                  \
        (aHandle)->mProtocolSentence->mWriteStartPos[5] = 0x00;     \
        (aHandle)->mProtocolSentence->mWriteStartPos[6] = 0x00;     \
        (aHandle)->mProtocolSentence->mWriteStartPos[7] = 0x00;
#else
#define CMP_HEADER_INIT( aHandle )
#endif


#define CMP_DASSERT_N_READABLE( aCursor, aBytes )                                 \
    STL_DASSERT( (aCursor)->mEndPos - (aCursor)->mCurPos >= (aBytes) )

#define CMP_CHECK_N_READABLE( aHandle, aCursor, aBytes, aErrorStack )             \
    {                                                                             \
        if( (aCursor)->mEndPos - (aCursor)->mCurPos < (aBytes) )                  \
        {                                                                         \
            (aHandle)->mProtocolSentence->mReadProtocolPos = (aCursor)->mCurPos;                     \
            STL_TRY( cmlRecvPacket( (aHandle), (aErrorStack) ) == STL_SUCCESS );  \
            (aCursor)->mCurPos = (aHandle)->mProtocolSentence->mReadProtocolPos;                     \
            (aCursor)->mEndPos = (aHandle)->mProtocolSentence->mReadEndPos;                          \
        }                                                                         \
    }

#define CMP_CHECK_N_WRITABLE( aHandle, aCursor, aBytes, aErrorStack )             \
    {                                                                             \
        if( (aCursor)->mEndPos - (aCursor)->mCurPos < (aBytes) )                  \
        {                                                                         \
            (aHandle)->mProtocolSentence->mWriteProtocolPos = (aCursor)->mCurPos;                    \
            STL_TRY( cmlFlushPacket( (aHandle), (aErrorStack) ) == STL_SUCCESS );  \
            (aCursor)->mCurPos = (aHandle)->mProtocolSentence->mWriteProtocolPos;                    \
            (aCursor)->mEndPos = (aHandle)->mProtocolSentence->mWriteEndPos;                         \
        }                                                                         \
    }

#define CMP_IS_EQUAL_ENDIAN( aHandle )                                                 \
    ( ( (aHandle)->mEndian == STL_PLATFORM_ENDIAN ) ? STL_TRUE : STL_FALSE )     \

#define CMP_CONVERT_ENDIAN_INT16( aDesc, aSrc )         \
    {                                                   \
        ((stlChar*)(aDesc))[0] = ((stlChar*)(aSrc))[1]; \
        ((stlChar*)(aDesc))[1] = ((stlChar*)(aSrc))[0]; \
    }

#define CMP_CONVERT_ENDIAN_INT32( aDesc, aSrc )         \
    {                                                   \
        ((stlChar*)(aDesc))[0] = ((stlChar*)(aSrc))[3]; \
        ((stlChar*)(aDesc))[1] = ((stlChar*)(aSrc))[2]; \
        ((stlChar*)(aDesc))[2] = ((stlChar*)(aSrc))[1]; \
        ((stlChar*)(aDesc))[3] = ((stlChar*)(aSrc))[0]; \
    }

#define CMP_CONVERT_ENDIAN_INT64( aDesc, aSrc )         \
    {                                                   \
        ((stlChar*)(aDesc))[0] = ((stlChar*)(aSrc))[7]; \
        ((stlChar*)(aDesc))[1] = ((stlChar*)(aSrc))[6]; \
        ((stlChar*)(aDesc))[2] = ((stlChar*)(aSrc))[5]; \
        ((stlChar*)(aDesc))[3] = ((stlChar*)(aSrc))[4]; \
        ((stlChar*)(aDesc))[4] = ((stlChar*)(aSrc))[3]; \
        ((stlChar*)(aDesc))[5] = ((stlChar*)(aSrc))[2]; \
        ((stlChar*)(aDesc))[6] = ((stlChar*)(aSrc))[1]; \
        ((stlChar*)(aDesc))[7] = ((stlChar*)(aSrc))[0]; \
    }

#define CMP_CHECK_READ_INT16( aHandle, aCursor, aValue )                \
    {                                                                   \
        if( CMP_IS_EQUAL_ENDIAN( (aHandle) ) == STL_TRUE )              \
        {                                                               \
            STL_WRITE_INT16( (aValue), (aCursor)->mCurPos );            \
        }                                                               \
        else                                                            \
        {                                                               \
            CMP_CONVERT_ENDIAN_INT16( (aValue), (aCursor)->mCurPos );   \
        }                                                               \
    }

#define CMP_CHECK_WRITE_INT16( aHandle, aCursor, aValue )               \
    {                                                                   \
        if( CMP_IS_EQUAL_ENDIAN( (aHandle) ) == STL_TRUE )              \
        {                                                               \
            STL_WRITE_INT16( (aCursor)->mCurPos, (aValue) );            \
        }                                                               \
        else                                                            \
        {                                                               \
            CMP_CONVERT_ENDIAN_INT16( (aCursor)->mCurPos, (aValue) );   \
        }                                                               \
    }

#define CMP_CHECK_READ_INT32( aHandle, aCursor, aValue )                \
    {                                                                   \
        if( CMP_IS_EQUAL_ENDIAN( (aHandle) ) == STL_TRUE )              \
        {                                                               \
            STL_WRITE_INT32( (aValue), (aCursor)->mCurPos );            \
        }                                                               \
        else                                                            \
        {                                                               \
            CMP_CONVERT_ENDIAN_INT32( (aValue), (aCursor)->mCurPos );   \
        }                                                               \
    }

#define CMP_CHECK_WRITE_INT32( aHandle, aCursor, aValue )               \
    {                                                                   \
        if( CMP_IS_EQUAL_ENDIAN( (aHandle) ) == STL_TRUE )              \
        {                                                               \
            STL_WRITE_INT32( (aCursor)->mCurPos, (aValue) );            \
        }                                                               \
        else                                                            \
        {                                                               \
            CMP_CONVERT_ENDIAN_INT32( (aCursor)->mCurPos, (aValue) );   \
        }                                                               \
    }

#define CMP_CHECK_READ_INT64( aHandle, aCursor, aValue )                \
    {                                                                   \
        if( CMP_IS_EQUAL_ENDIAN( (aHandle) ) == STL_TRUE )              \
        {                                                               \
            STL_WRITE_INT64( (aValue), (aCursor)->mCurPos );            \
        }                                                               \
        else                                                            \
        {                                                               \
            CMP_CONVERT_ENDIAN_INT64( (aValue), (aCursor)->mCurPos );   \
        }                                                               \
    }

#define CMP_CHECK_WRITE_INT64( aHandle, aCursor, aValue )               \
    {                                                                   \
        if( CMP_IS_EQUAL_ENDIAN( (aHandle) ) == STL_TRUE )              \
        {                                                               \
            STL_WRITE_INT64( (aCursor)->mCurPos, (aValue) );            \
        }                                                               \
        else                                                            \
        {                                                               \
            CMP_CONVERT_ENDIAN_INT64( (aCursor)->mCurPos, (aValue) );   \
        }                                                               \
    }

#define CMP_READ_INT8( aHandle, aCursor, aValue, aErrorStack )          \
    {                                                                   \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 1, aErrorStack );       \
        STL_WRITE_INT8( (aValue), (aCursor)->mCurPos );                 \
        (aCursor)->mCurPos += 1;                                        \
    }

#define CMP_WRITE_INT8( aHandle, aCursor, aValue, aErrorStack )         \
    {                                                                   \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 1, aErrorStack );       \
        STL_WRITE_INT8( (aCursor)->mCurPos, (aValue) ) ;                \
        (aCursor)->mCurPos += 1;                                        \
    }

#define CMP_READ_INT16( aHandle, aCursor, aValue, aErrorStack )         \
    {                                                                   \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 2, aErrorStack );       \
        STL_WRITE_INT16( (aValue), (aCursor)->mCurPos );                \
        (aCursor)->mCurPos += 2;                                        \
    }

#define CMP_WRITE_INT16( aHandle, aCursor, aValue, aErrorStack )        \
    {                                                                   \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 2, aErrorStack );       \
        STL_WRITE_INT16( (aCursor)->mCurPos, (aValue) );                \
        (aCursor)->mCurPos += 2;                                        \
    }

#define CMP_READ_INT32( aHandle, aCursor, aValue, aErrorStack )         \
    {                                                                   \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 4, aErrorStack );       \
        STL_WRITE_INT32( (aValue), (aCursor)->mCurPos );                \
        (aCursor)->mCurPos += 4;                                        \
    }

#define CMP_WRITE_INT32( aHandle, aCursor, aValue, aErrorStack )        \
    {                                                                   \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 4, aErrorStack );       \
        STL_WRITE_INT32( (aCursor)->mCurPos, (aValue) );                \
        (aCursor)->mCurPos += 4;                                        \
    }

#define CMP_READ_INT64( aHandle, aCursor, aValue, aErrorStack )         \
    {                                                                   \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 8, aErrorStack );       \
        STL_WRITE_INT64( (aValue), (aCursor)->mCurPos );                \
        (aCursor)->mCurPos += 8;                                        \
    }

#define CMP_WRITE_INT64( aHandle, aCursor, aValue, aErrorStack )        \
    {                                                                   \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 8, aErrorStack );       \
        STL_WRITE_INT64( (aCursor)->mCurPos, (aValue) );                \
        (aCursor)->mCurPos += 8;                                        \
    }

#define CMP_READ_INT16_ENDIAN( aHandle, aCursor, aValue, aErrorStack )  \
    {                                                                   \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 2, aErrorStack );       \
        CMP_CHECK_READ_INT16( aHandle, aCursor, aValue );               \
        (aCursor)->mCurPos += 2;                                        \
    }

#define CMP_WRITE_INT16_ENDIAN( aHandle, aCursor, aValue, aErrorStack ) \
    {                                                                   \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 2, aErrorStack );       \
        CMP_CHECK_WRITE_INT16( aHandle, aCursor, aValue );              \
        (aCursor)->mCurPos += 2;                                        \
    }

#define CMP_READ_INT32_ENDIAN( aHandle, aCursor, aValue, aErrorStack )  \
    {                                                                   \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 4, aErrorStack );       \
        CMP_CHECK_READ_INT32( aHandle, aCursor, aValue );               \
        (aCursor)->mCurPos += 4;                                        \
    }

#define CMP_WRITE_INT32_ENDIAN( aHandle, aCursor, aValue, aErrorStack ) \
    {                                                                   \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 4, aErrorStack );       \
        CMP_CHECK_WRITE_INT32( aHandle, aCursor, aValue );              \
        (aCursor)->mCurPos += 4;                                        \
    }

#define CMP_READ_INT64_ENDIAN( aHandle, aCursor, aValue, aErrorStack )  \
    {                                                                   \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 8, aErrorStack );       \
        CMP_CHECK_READ_INT64( aHandle, aCursor, aValue );               \
        (aCursor)->mCurPos += 8;                                        \
    }

#define CMP_WRITE_INT64_ENDIAN( aHandle, aCursor, aValue, aErrorStack ) \
    {                                                                   \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 8, aErrorStack );       \
        CMP_CHECK_WRITE_INT64( aHandle, aCursor, aValue );              \
        (aCursor)->mCurPos += 8;                                        \
    }

#define CMP_SKIP_BYTES( aHandle, aCursor, aLength, aErrorStack )        \
    {                                                                   \
        if( (aCursor)->mEndPos - (aCursor)->mCurPos < (aLength) )       \
        {                                                               \
            cmpSkipNBytes( aHandle, aCursor, aLength, aErrorStack );    \
        }                                                               \
        else                                                            \
        {                                                               \
            (aCursor)->mCurPos += (aLength);                            \
        }                                                               \
    }

#define CMP_READ_BYTES( aHandle, aCursor, aValue, aLength, aErrorStack )        \
    {                                                                           \
        if( (aCursor)->mEndPos - (aCursor)->mCurPos < (aLength) )               \
        {                                                                       \
            cmpReadNBytes( aHandle, aCursor, aValue, aLength, aErrorStack );    \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            STL_WRITE_BYTES( aValue, (aCursor)->mCurPos, (aLength) );           \
            (aCursor)->mCurPos += (aLength);                                    \
        }                                                                       \
    }

#define CMP_WRITE_BYTES( aHandle, aCursor, aValue, aLength, aErrorStack )       \
    {                                                                           \
        if( (aCursor)->mEndPos - (aCursor)->mCurPos < (aLength) )               \
        {                                                                       \
            cmpWriteNBytes( aHandle, aCursor, aValue, aLength, aErrorStack );   \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            STL_WRITE_BYTES( (aCursor)->mCurPos, aValue, (aLength) );           \
            (aCursor)->mCurPos += (aLength);                                    \
        }                                                                       \
    }

#define CMP_READ_VAR_LEN( aHandle, aCursor, aLength, aErrorStack )              \
    {                                                                           \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 1, aErrorStack );               \
        if( *((stlUInt8*)((aCursor)->mCurPos)) == CMP_2BYTE_LEN_FLAG )          \
        {                                                                       \
            CMP_DASSERT_N_READABLE( aCursor, 3 );                               \
            STL_WRITE_INT16( &((aCursor)->mInt16), (aCursor)->mCurPos + 1 );    \
            (aLength) = (aCursor)->mInt16;                                      \
            (aCursor)->mCurPos += 3;                                            \
        }                                                                       \
        else if( *((stlUInt8*)((aCursor)->mCurPos)) == CMP_4BYTE_LEN_FLAG )     \
        {                                                                       \
            CMP_DASSERT_N_READABLE( aCursor, 5 );                               \
            STL_WRITE_INT32( &((aCursor)->mInt32), (aCursor)->mCurPos + 1 );    \
            (aLength) = (aCursor)->mInt32;                                      \
            (aCursor)->mCurPos += 5;                                            \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            (aLength) = *((stlUInt8*)((aCursor)->mCurPos));                     \
            (aCursor)->mCurPos += 1;                                            \
        }                                                                       \
    }

#define CMP_READ_N_VAR( aHandle, aCursor, aValue, aLength, aErrorStack )        \
    {                                                                           \
        CMP_READ_VAR_LEN( aHandle, aCursor, aLength, aErrorStack );             \
        CMP_READ_BYTES( aHandle, aCursor, aValue, aLength, aErrorStack );       \
    }

#define CMP_READ_VAR_LEN_ENDIAN( aHandle, aCursor, aLength, aErrorStack )                   \
    {                                                                                       \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 1, aErrorStack );                           \
        if( *((stlUInt8*)((aCursor)->mCurPos)) == CMP_2BYTE_LEN_FLAG )                      \
        {                                                                                   \
            CMP_DASSERT_N_READABLE( aCursor, 3 );                                           \
            if( CMP_IS_EQUAL_ENDIAN( aHandle ) == STL_TRUE )                                \
            {                                                                               \
                STL_WRITE_INT16( &((aCursor)->mInt16), (aCursor)->mCurPos + 1 );            \
            }                                                                               \
            else                                                                            \
            {                                                                               \
                CMP_CONVERT_ENDIAN_INT16( &((aCursor)->mInt16), (aCursor)->mCurPos + 1 );   \
            }                                                                               \
            (aLength) = (aCursor)->mInt16;                                                  \
            (aCursor)->mCurPos += 3;                                                        \
        }                                                                                   \
        else if( *((stlUInt8*)((aCursor)->mCurPos)) == CMP_4BYTE_LEN_FLAG )                 \
        {                                                                                   \
            CMP_DASSERT_N_READABLE( aCursor, 5 );                                           \
            if( CMP_IS_EQUAL_ENDIAN( aHandle ) == STL_TRUE )                                \
            {                                                                               \
                STL_WRITE_INT32( &((aCursor)->mInt32), (aCursor)->mCurPos + 1 );            \
            }                                                                               \
            else                                                                            \
            {                                                                               \
                CMP_CONVERT_ENDIAN_INT32( &((aCursor)->mInt16), (aCursor)->mCurPos + 1 );   \
            }                                                                               \
            (aLength) = (aCursor)->mInt32;                                                  \
            (aCursor)->mCurPos += 5;                                                        \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            (aLength) = *((stlUInt8*)((aCursor)->mCurPos));                                 \
            (aCursor)->mCurPos += 1;                                                        \
        }                                                                                   \
    }

#define CMP_READ_N_VAR_ENDIAN( aHandle, aCursor, aValue, aLength, aErrorStack ) \
    {                                                                           \
        CMP_READ_VAR_LEN_ENDIAN( aHandle, aCursor, aLength, aErrorStack );      \
        CMP_READ_BYTES( aHandle, aCursor, aValue, aLength, aErrorStack );       \
    }

#define CMP_WRITE_N_VAR( aHandle, aCursor, aValue, aLength, aErrorStack )               \
    {                                                                                   \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 5, aErrorStack );                       \
        if( (aLength) > CMP_2BYTE_LEN_MAX )                                             \
        {                                                                               \
            *((aCursor)->mCurPos) = CMP_4BYTE_LEN_FLAG;                                 \
            (aCursor)->mCurPos += 1;                                                    \
            (aCursor)->mInt32 = (stlInt32)(aLength);                                    \
            CMP_WRITE_INT32( aHandle, aCursor, &((aCursor)->mInt32), aErrorStack );     \
        }                                                                               \
        else if( (aLength) > CMP_1BYTE_LEN_MAX )                                        \
        {                                                                               \
            *((aCursor)->mCurPos) = CMP_2BYTE_LEN_FLAG;                                 \
            (aCursor)->mCurPos += 1;                                                    \
            (aCursor)->mInt16 = (stlInt16)(aLength);                                    \
            CMP_WRITE_INT16( aHandle, aCursor, &((aCursor)->mInt16), aErrorStack );     \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            *((aCursor)->mCurPos) = (stlInt8)(aLength);                                 \
            (aCursor)->mCurPos += 1;                                                    \
        }                                                                               \
        CMP_WRITE_BYTES( aHandle, aCursor, aValue, aLength, aErrorStack );              \
    }

#define CMP_WRITE_N_VAR16( aHandle, aCursor, aValue, aLength, aErrorStack )             \
    {                                                                                   \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 5, aErrorStack );                       \
        if( (aLength) > CMP_1BYTE_LEN_MAX )                                             \
        {                                                                               \
            *((aCursor)->mCurPos) = CMP_2BYTE_LEN_FLAG;                                 \
            (aCursor)->mCurPos += 1;                                                    \
            (aCursor)->mInt16 = (stlInt16)(aLength);                                    \
            CMP_WRITE_INT16( aHandle, aCursor, &((aCursor)->mInt16), aErrorStack );     \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            *((aCursor)->mCurPos) = (stlInt8)(aLength);                                 \
            (aCursor)->mCurPos += 1;                                                    \
        }                                                                               \
        CMP_WRITE_BYTES( aHandle, aCursor, aValue, aLength, aErrorStack );              \
    }

#define CMP_WRITE_N_VAR_ENDIAN( aHandle, aCursor, aValue, aLength, aErrorStack )                \
    {                                                                                           \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 5, aErrorStack );                               \
        if( (aLength) > CMP_2BYTE_LEN_MAX )                                                     \
        {                                                                                       \
            *((aCursor)->mCurPos) = CMP_4BYTE_LEN_FLAG;                                         \
            (aCursor)->mCurPos += 1;                                                            \
            (aCursor)->mInt32 = (stlInt32)(aLength);                                            \
            CMP_WRITE_INT32_ENDIAN( aHandle, aCursor, &((aCursor)->mInt32), aErrorStack );      \
        }                                                                                       \
        else if( (aLength) > CMP_1BYTE_LEN_MAX )                                                \
        {                                                                                       \
            *((aCursor)->mCurPos) = CMP_2BYTE_LEN_FLAG;                                         \
            (aCursor)->mCurPos += 1;                                                            \
            (aCursor)->mInt16 = (stlInt16)(aLength);                                            \
            CMP_WRITE_INT16_ENDIAN( aHandle, aCursor, &((aCursor)->mInt16), aErrorStack );      \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            *((aCursor)->mCurPos) = (stlInt8)(aLength);                                         \
            (aCursor)->mCurPos += 1;                                                            \
        }                                                                                       \
        CMP_WRITE_BYTES( aHandle, aCursor, aValue, aLength, aErrorStack );                      \
    }

#define CMP_READ_1_VAR( aHandle, aCursor, aValue, aLength, aErrorStack )        \
    {                                                                           \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 1, aErrorStack );               \
        (aLength) = *((stlUInt8*)((aCursor)->mCurPos));                         \
        (aCursor)->mCurPos += 1;                                                \
        STL_DASSERT( (aLength) < 0xFE );                                        \
        CMP_READ_BYTES( aHandle, aCursor, aValue, aLength, aErrorStack );       \
    }

#define CMP_WRITE_1_VAR( aHandle, aCursor, aValue, aLength, aErrorStack )       \
    {                                                                           \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 1, aErrorStack );               \
        *((aCursor)->mCurPos) = (aLength);                                      \
        (aCursor)->mCurPos += 1;                                                \
        CMP_WRITE_BYTES( aHandle, aCursor, aValue, aLength, aErrorStaak );      \
    }

#define CMP_READ_VAR_STRING( aHandle, aCursor, aValue, aLength, aBufferLength, aErrorStack )        \
    {                                                                                               \
        CMP_READ_N_VAR( aHandle, aCursor, aValue, aLength, aErrorStack );                           \
        STL_DASSERT( (aLength) <= aBufferLength );                                                  \
        (aValue)[(aLength)] = '\0';                                                                 \
    }

#define CMP_READ_VAR_STRING_ENDIAN( aHandle, aCursor, aValue, aLength, aBufferLength, aErrorStack ) \
    {                                                                                               \
        CMP_READ_N_VAR_ENDIAN( aHandle, aCursor, aValue, aLength, aErrorStack );                    \
        STL_DASSERT( (aLength) <= aBufferLength );                                                  \
        (aValue)[(aLength)] = '\0';                                                                 \
    }

#define CMP_WRITE_VAR_INT16( aHandle, aCursor, aValue, aErrorStack )                       \
    {                                                                                      \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 9, aErrorStack );                          \
        if( ((stlInt64)*(aValue) >= CMP_VAR_INT_1BYTE_MIN) &&                              \
            ((stlInt64)*(aValue) <= CMP_VAR_INT_1BYTE_MAX) )                               \
        {                                                                                  \
            *((aCursor)->mCurPos) = *(aValue);                                             \
            (aCursor)->mCurPos += 1;                                                       \
        }                                                                                  \
        else                                                                               \
        {                                                                                  \
            *((aCursor)->mCurPos) = CMP_VAR_INT_2BYTE_FLAG;                                \
            *((aCursor)->mCurPos + 1) = (*(aValue) & 0xFF00) >> 8;                         \
            *((aCursor)->mCurPos + 2) = *(aValue) & 0xFF;                                  \
            (aCursor)->mCurPos += 3;                                                       \
        }                                                                                  \
    }

#define CMP_WRITE_VAR_INT32( aHandle, aCursor, aValue, aErrorStack )                       \
    {                                                                                      \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 9, aErrorStack );                          \
        if( ((stlInt64)*(aValue) >= CMP_VAR_INT_1BYTE_MIN) &&                              \
            ((stlInt64)*(aValue) <= CMP_VAR_INT_1BYTE_MAX) )                               \
        {                                                                                  \
            *((aCursor)->mCurPos) = *(aValue);                                             \
            (aCursor)->mCurPos += 1;                                                       \
        }                                                                                  \
        else if( ((stlInt64)*(aValue) >= STL_INT16_MIN ) &&                                \
                 ((stlInt64)*(aValue) <= STL_INT16_MAX) )                                  \
        {                                                                                  \
            *((aCursor)->mCurPos) = CMP_VAR_INT_2BYTE_FLAG;                                \
            *((aCursor)->mCurPos + 1) = (*(aValue) & 0xFF00) >> 8;                         \
            *((aCursor)->mCurPos + 2) = *(aValue) & 0xFF;                                  \
            (aCursor)->mCurPos += 3;                                                       \
        }                                                                                  \
        else                                                                               \
        {                                                                                  \
            *((aCursor)->mCurPos) = CMP_VAR_INT_4BYTE_FLAG;                                \
            *((aCursor)->mCurPos + 1) = (*(aValue) & 0xFF000000) >> 24;                    \
            *((aCursor)->mCurPos + 2) = (*(aValue) & 0xFF0000) >> 16;                      \
            *((aCursor)->mCurPos + 3) = (*(aValue) & 0xFF00) >> 8;                         \
            *((aCursor)->mCurPos + 4) = *(aValue) & 0xFF;                                  \
            (aCursor)->mCurPos += 5;                                                       \
        }                                                                                  \
    }

#define CMP_WRITE_VAR_UINT32( aHandle, aCursor, aValue, aErrorStack )                      \
    {                                                                                      \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 9, aErrorStack );                          \
        if( (stlInt64)*(aValue) <= CMP_VAR_INT_1BYTE_MAX )                                 \
        {                                                                                  \
            *((aCursor)->mCurPos) = *(aValue);                                             \
            (aCursor)->mCurPos += 1;                                                       \
        }                                                                                  \
        else if( (stlInt64)*(aValue) <= STL_INT16_MAX )                                    \
        {                                                                                  \
            *((aCursor)->mCurPos) = CMP_VAR_INT_2BYTE_FLAG;                                \
            *((aCursor)->mCurPos + 1) = (*(aValue) & 0xFF00) >> 8;                         \
            *((aCursor)->mCurPos + 2) = *(aValue) & 0xFF;                                  \
            (aCursor)->mCurPos += 3;                                                       \
        }                                                                                  \
        else                                                                               \
        {                                                                                  \
            *((aCursor)->mCurPos) = CMP_VAR_INT_4BYTE_FLAG;                                \
            *((aCursor)->mCurPos + 1) = (*(aValue) & 0xFF000000) >> 24;                    \
            *((aCursor)->mCurPos + 2) = (*(aValue) & 0xFF0000) >> 16;                      \
            *((aCursor)->mCurPos + 3) = (*(aValue) & 0xFF00) >> 8;                         \
            *((aCursor)->mCurPos + 4) = *(aValue) & 0xFF;                                  \
            (aCursor)->mCurPos += 5;                                                       \
        }                                                                                  \
    }

#define CMP_WRITE_VAR_INT64( aHandle, aCursor, aValue, aErrorStack )                            \
    {                                                                                           \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 9, aErrorStack );                               \
        if( ((stlInt64)*(aValue) >= CMP_VAR_INT_1BYTE_MIN) &&                                   \
            ((stlInt64)*(aValue) <= CMP_VAR_INT_1BYTE_MAX) )                                    \
        {                                                                                       \
            *((aCursor)->mCurPos) = *(aValue);                                                  \
            (aCursor)->mCurPos += 1;                                                            \
        }                                                                                       \
        else if( ((stlInt64)*(aValue) >= STL_INT16_MIN ) &&                                     \
                 ((stlInt64)*(aValue) <= STL_INT16_MAX) )                                       \
        {                                                                                       \
            *((aCursor)->mCurPos) = CMP_VAR_INT_2BYTE_FLAG;                                     \
            *((aCursor)->mCurPos + 1) = (*(aValue) & 0xFF00) >> 8;                              \
            *((aCursor)->mCurPos + 2) = *(aValue) & 0xFF;                                       \
            (aCursor)->mCurPos += 3;                                                            \
        }                                                                                       \
        else if( ((stlInt64)*(aValue) >= STL_INT32_MIN) &&                                      \
                 ((stlInt64)*(aValue) <= STL_INT32_MAX) )                                       \
        {                                                                                       \
            *((aCursor)->mCurPos) = CMP_VAR_INT_4BYTE_FLAG;                                     \
            *((aCursor)->mCurPos + 1) = (*(aValue) & 0xFF000000) >> 24;                         \
            *((aCursor)->mCurPos + 2) = (*(aValue) & 0xFF0000) >> 16;                           \
            *((aCursor)->mCurPos + 3) = (*(aValue) & 0xFF00) >> 8;                              \
            *((aCursor)->mCurPos + 4) = *(aValue) & 0xFF;                                       \
            (aCursor)->mCurPos += 5;                                                            \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            *((aCursor)->mCurPos) = CMP_VAR_INT_8BYTE_FLAG;                                     \
            *((aCursor)->mCurPos + 1) = (*(aValue) & STL_INT64_C(0xFF00000000000000)) >> 56;    \
            *((aCursor)->mCurPos + 2) = (*(aValue) & STL_INT64_C(0xFF000000000000)) >> 48;      \
            *((aCursor)->mCurPos + 3) = (*(aValue) & STL_INT64_C(0xFF0000000000)) >> 40;        \
            *((aCursor)->mCurPos + 4) = (*(aValue) & STL_INT64_C(0xFF00000000)) >> 32;          \
            *((aCursor)->mCurPos + 5) = (*(aValue) & 0xFF000000) >> 24;                         \
            *((aCursor)->mCurPos + 6) = (*(aValue) & 0xFF0000) >> 16;                           \
            *((aCursor)->mCurPos + 7) = (*(aValue) & 0xFF00) >> 8;                              \
            *((aCursor)->mCurPos + 8) = *(aValue) & 0xFF;                                       \
            (aCursor)->mCurPos += 9;                                                            \
        }                                                                                       \
    }

#define CMP_READ_VAR_INT16( aHandle, aCursor, aValue, aErrorStack )         \
    {                                                                       \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 1, aErrorStack );           \
        if( *((stlInt8*)((aCursor)->mCurPos)) >= CMP_VAR_INT_1BYTE_MIN )    \
        {                                                                   \
            *(aValue) = *((stlInt8*)((aCursor)->mCurPos));                  \
            (aCursor)->mCurPos += 1;                                        \
        }                                                                   \
        else if( *((stlInt8*)((aCursor)->mCurPos)) == CMP_VAR_INT_2BYTE_FLAG ) \
        {                                                                   \
            CMP_DASSERT_N_READABLE( aCursor, 3 );                           \
            *(aValue) = (((*((stlUInt8*)((aCursor)->mCurPos + 1))) << 8) |  \
                         (*((stlUInt8*)((aCursor)->mCurPos + 2))));         \
            (aCursor)->mCurPos += 3;                                        \
        }                                                                   \
        else                                                                \
        {                                                                   \
            *(aValue) = 0;                                                  \
            STL_ASSERT(0);                                                  \
        }                                                                   \
    }

#define CMP_READ_VAR_INT32( aHandle, aCursor, aValue, aErrorStack )         \
    {                                                                       \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 1, aErrorStack );           \
        if( *((stlInt8*)((aCursor)->mCurPos)) >= CMP_VAR_INT_1BYTE_MIN )    \
        {                                                                   \
            *(aValue) = *((stlInt8*)((aCursor)->mCurPos));                  \
            (aCursor)->mCurPos += 1;                                        \
        }                                                                   \
        else if( *((stlInt8*)((aCursor)->mCurPos)) == CMP_VAR_INT_2BYTE_FLAG ) \
        {                                                                   \
            CMP_DASSERT_N_READABLE( aCursor, 3 );                           \
            *(aValue) = (((*((stlUInt8*)((aCursor)->mCurPos + 1))) << 8) |  \
                         (*((stlUInt8*)((aCursor)->mCurPos + 2))));         \
            (aCursor)->mCurPos += 3;                                        \
        }                                                                   \
        else if( *((stlInt8*)((aCursor)->mCurPos)) == CMP_VAR_INT_4BYTE_FLAG ) \
        {                                                                   \
            CMP_DASSERT_N_READABLE( aCursor, 5 );                           \
            *(aValue) = (((*((stlUInt8*)((aCursor)->mCurPos + 1))) << 24) | \
                         ((*((stlUInt8*)((aCursor)->mCurPos + 2))) << 16) | \
                         ((*((stlUInt8*)((aCursor)->mCurPos + 3))) << 8)  | \
                         (*((stlUInt8*)((aCursor)->mCurPos + 4))));         \
            (aCursor)->mCurPos += 5;                                        \
        }                                                                   \
        else                                                                \
        {                                                                   \
            *(aValue) = 0;                                                  \
            STL_ASSERT(0);                                                  \
        }                                                                   \
    }

#define CMP_READ_VAR_INT64( aHandle, aCursor, aValue, aErrorStack )         \
    {                                                                       \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 1, aErrorStack );           \
        if( *((stlInt8*)((aCursor)->mCurPos)) >= CMP_VAR_INT_1BYTE_MIN )    \
        {                                                                   \
            *(aValue) = *((stlInt8*)((aCursor)->mCurPos));                  \
            (aCursor)->mCurPos += 1;                                        \
        }                                                                   \
        else if( *((stlInt8*)((aCursor)->mCurPos)) == CMP_VAR_INT_2BYTE_FLAG ) \
        {                                                                   \
            CMP_DASSERT_N_READABLE( aCursor, 3 );                           \
            *(aValue) = (((*((stlUInt8*)((aCursor)->mCurPos + 1))) << 8) |  \
                         (*((stlUInt8*)((aCursor)->mCurPos + 2))));         \
            (aCursor)->mCurPos += 3;                                        \
        }                                                                   \
        else if( *((stlInt8*)((aCursor)->mCurPos)) == CMP_VAR_INT_4BYTE_FLAG ) \
        {                                                                   \
            CMP_DASSERT_N_READABLE( aCursor, 5 );                           \
            *(aValue) = (((*((stlUInt8*)((aCursor)->mCurPos + 1))) << 24) | \
                         ((*((stlUInt8*)((aCursor)->mCurPos + 2))) << 16) | \
                         ((*((stlUInt8*)((aCursor)->mCurPos + 3))) << 8) |  \
                         (*((stlUInt8*)((aCursor)->mCurPos + 4))));         \
            (aCursor)->mCurPos += 5;                                        \
        }                                                                   \
        else                                                                \
        {                                                                   \
            CMP_DASSERT_N_READABLE( aCursor, 9 );                           \
            *(aValue) = ((((stlInt64)*((aCursor)->mCurPos + 1)) << 56) |    \
                         (((stlInt64)*((aCursor)->mCurPos + 2)) << 48) |    \
                         (((stlInt64)*((aCursor)->mCurPos + 3)) << 40) |    \
                         (((stlInt64)*((aCursor)->mCurPos + 4)) << 32) |    \
                         (((stlInt64)*((aCursor)->mCurPos + 5)) << 24) |    \
                         (((stlInt64)*((aCursor)->mCurPos + 6)) << 16) |    \
                         (((stlInt64)*((aCursor)->mCurPos + 7)) << 8)  |    \
                         (((stlInt64)*((aCursor)->mCurPos + 8)) & 0xFF));   \
            (aCursor)->mCurPos += 9;                                        \
        }                                                                   \
    }

/**
 * statement id로 사용되는 상수들
 *  - undefined: 아직 할당되지 않은 statement의 id로 새로 할당하라는 의미
 *  - predefined: 프로토콜 패킷의 앞쪽 프로토콜에서 사용된 statement id를 재사용하라는 의미
 */
#define CMP_STATEMENT_ID_UNDEFINED          (CMP_STATEMENT_ID_8BYTE_FLAG)
#define CMP_STATEMENT_ID_PREDEFINED         (CMP_STATEMENT_ID_4BYTE_FLAG)

#define CMP_WRITE_STATEMENT_ID_BODY( aHandle, aCursor, aId, aErrorStack )                           \
    {                                                                                               \
        if( ((*((stlUInt64*)(aId)) & STL_UINT64_C(0xFFFF000000000000)) >> 48) <= CMP_STATEMENT_ID_1BYTE_MAX ) \
        {                                                                                           \
            *((aCursor)->mCurPos) = ((*((stlUInt64*)(aId)) & STL_UINT64_C(0xFFFF000000000000)) >> 48); \
            (aCursor)->mCurPos += 1;                                                                \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            *((aCursor)->mCurPos) = CMP_STATEMENT_ID_2BYTE_FLAG;                                    \
            *((aCursor)->mCurPos + 1) = (*((stlUInt64*)(aId)) & STL_UINT64_C(0xFF00000000000000)) >> 56; \
            *((aCursor)->mCurPos + 2) = (*(aId) & STL_INT64_C(0x00FF000000000000)) >> 48;           \
            (aCursor)->mCurPos += 3;                                                                \
        }                                                                                           \
        if( (*(aId) & STL_INT64_C(0x0000FFFFFFFFFFFF)) <= CMP_STATEMENT_ID_1BYTE_MAX )              \
        {                                                                                           \
            *((aCursor)->mCurPos) = *(aId) & 0xFF;                                                  \
            (aCursor)->mCurPos += 1;                                                                \
        }                                                                                           \
        else if( (*(aId) & STL_INT64_C(0x0000FFFFFFFFFFFF)) <= STL_UINT16_MAX )                     \
        {                                                                                           \
            *((aCursor)->mCurPos) = CMP_STATEMENT_ID_2BYTE_FLAG;                                    \
            *((aCursor)->mCurPos + 1) = (*(aId) & 0xFF00) >> 8;                                     \
            *((aCursor)->mCurPos + 2) = (*(aId) & 0x00FF);                                          \
            (aCursor)->mCurPos += 3;                                                                \
        }                                                                                           \
        else if( (*(aId) & STL_INT64_C(0x0000FFFFFFFFFFFF)) <= STL_UINT32_MAX )                     \
        {                                                                                           \
            *((aCursor)->mCurPos) = CMP_STATEMENT_ID_4BYTE_FLAG;                                    \
            *((aCursor)->mCurPos + 1) = (*(aId) & 0xFF000000) >> 24;                                \
            *((aCursor)->mCurPos + 2) = (*(aId) & 0x00FF0000) >> 16;                                \
            *((aCursor)->mCurPos + 3) = (*(aId) & 0x0000FF00) >> 8;                                 \
            *((aCursor)->mCurPos + 4) = (*(aId) & 0x000000FF);                                      \
            (aCursor)->mCurPos += 5;                                                                \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            *((aCursor)->mCurPos) = CMP_STATEMENT_ID_8BYTE_FLAG;                                    \
            *((aCursor)->mCurPos + 1) = (*(aId) & STL_INT64_C(0xFF0000000000)) >> 40;               \
            *((aCursor)->mCurPos + 2) = (*(aId) & STL_INT64_C(0xFF00000000)) >> 32;                 \
            *((aCursor)->mCurPos + 3) = (*(aId) & 0xFF000000) >> 24;                                \
            *((aCursor)->mCurPos + 4) = (*(aId) & 0xFF0000) >> 16;                                  \
            *((aCursor)->mCurPos + 5) = (*(aId) & 0xFF00) >> 8;                                     \
            *((aCursor)->mCurPos + 6) = (*(aId) & 0xFF);                                            \
            (aCursor)->mCurPos += 7;                                                                \
        }                                                                                           \
    }

#define CMP_WRITE_STATEMENT_ID( aHandle, aCursor, aId, aErrorStack )                                 \
    {                                                                                                \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 10, aErrorStack );                                   \
        STL_DASSERT( *(aId) != CML_STATEMENT_ID_UNDEFINED );                                         \
        CMP_WRITE_STATEMENT_ID_BODY( aHandle, aCursor, aId, aErrorStack );                           \
    }

#define CMP_WRITE_STATEMENT_ID_ENDIAN( aHandle, aCursor, aId, aErrorStack )                          \
    {                                                                                                \
        CMP_CHECK_N_WRITABLE( aHandle, aCursor, 10, aErrorStack );                                   \
        if( *(aId) == CML_STATEMENT_ID_UNDEFINED )                                                   \
        {                                                                                            \
            *((aCursor)->mCurPos) = CMP_STATEMENT_ID_UNDEFINED;                                      \
            (aCursor)->mCurPos += 1;                                                                 \
        }                                                                                            \
        else if( *(aId) == (aHandle)->mProtocolSentence->mCachedStatementId )                               \
        {                                                                                            \
            *((aCursor)->mCurPos) = CMP_STATEMENT_ID_PREDEFINED;                                     \
            (aCursor)->mCurPos += 1;                                                                 \
        }                                                                                            \
        else                                                                                         \
        {                                                                                            \
            CMP_WRITE_STATEMENT_ID_BODY( aHandle, aCursor, aId, aErrorStack );                       \
            (aHandle)->mProtocolSentence->mCachedStatementId = *(aId);                                      \
        }                                                                                            \
    }

#define CMP_READ_STATEMENT_ID_BODY( aHandle, aCursor, aId, aErrorStack )              \
    {                                                                                 \
        if( *((stlUInt8*)((aCursor)->mCurPos)) <= CMP_STATEMENT_ID_1BYTE_MAX )        \
        {                                                                             \
            *(aId) = *((stlUInt8*)((aCursor)->mCurPos));                              \
            (aCursor)->mCurPos += 1;                                                  \
        }                                                                             \
        else if( *((stlUInt8*)((aCursor)->mCurPos)) == CMP_STATEMENT_ID_2BYTE_FLAG )  \
        {                                                                             \
            CMP_CHECK_N_READABLE( aHandle, aCursor, 3, aErrorStack );                 \
            *(aId) = (*((stlUInt8*)((aCursor)->mCurPos + 1)) << 8) +                  \
                     (*((stlUInt8*)((aCursor)->mCurPos + 2)));                        \
            (aCursor)->mCurPos += 3;                                                  \
        }                                                                             \
        *(aId) = *(aId) << 48;                                                        \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 1, aErrorStack );                     \
        if( *((stlUInt8*)((aCursor)->mCurPos)) <= CMP_STATEMENT_ID_1BYTE_MAX )        \
        {                                                                             \
            *(aId) += *((stlUInt8*)((aCursor)->mCurPos));                             \
            (aCursor)->mCurPos += 1;                                                  \
        }                                                                             \
        else if( (stlUInt8)*((aCursor)->mCurPos) == CMP_STATEMENT_ID_2BYTE_FLAG )     \
        {                                                                             \
            CMP_CHECK_N_READABLE( aHandle, aCursor, 3, aErrorStack );                 \
            *(aId) += (*((stlUInt8*)((aCursor)->mCurPos + 1)) << 8) +                 \
                      (*((stlUInt8*)((aCursor)->mCurPos + 2)));                       \
            (aCursor)->mCurPos += 3;                                                  \
        }                                                                             \
        else if( (stlUInt8)*((aCursor)->mCurPos) == CMP_STATEMENT_ID_4BYTE_FLAG )     \
        {                                                                             \
            CMP_CHECK_N_READABLE( aHandle, aCursor, 5, aErrorStack );                 \
            *(aId) += (((stlUInt32)(*((stlUInt8*)((aCursor)->mCurPos + 1)))) << 24) + \
                      (((stlUInt32)(*((stlUInt8*)((aCursor)->mCurPos + 2)))) << 16) + \
                      (((stlUInt32)(*((stlUInt8*)((aCursor)->mCurPos + 3)))) << 8) +  \
                      (((stlUInt32)(*((stlUInt8*)((aCursor)->mCurPos + 4)))));        \
            (aCursor)->mCurPos += 5;                                                  \
        }                                                                             \
        else                                                                          \
        {                                                                             \
            CMP_CHECK_N_READABLE( aHandle, aCursor, 7, aErrorStack );                 \
            *(aId) += (((stlUInt64)(*((stlUInt8*)((aCursor)->mCurPos + 1)))) << 40) + \
                      (((stlUInt64)(*((stlUInt8*)((aCursor)->mCurPos + 2)))) << 32) + \
                      (((stlUInt64)(*((stlUInt8*)((aCursor)->mCurPos + 3)))) << 24) + \
                      (((stlUInt64)(*((stlUInt8*)((aCursor)->mCurPos + 4)))) << 16) + \
                      (((stlUInt64)(*((stlUInt8*)((aCursor)->mCurPos + 5)))) << 8) +  \
                      (((stlUInt64)(*((stlUInt8*)((aCursor)->mCurPos + 6)))));        \
            (aCursor)->mCurPos += 7;                                                  \
        }                                                                             \
    }

#define CMP_READ_STATEMENT_ID( aHandle, aCursor, aId, aErrorStack )                   \
    {                                                                                 \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 1, aErrorStack );                     \
        if( (stlUInt8)*((aCursor)->mCurPos) == CMP_STATEMENT_ID_UNDEFINED )           \
        {                                                                             \
            *(aId) = CML_STATEMENT_ID_UNDEFINED;                                      \
            (aCursor)->mCurPos += 1;                                                  \
        }                                                                             \
        else if( (stlUInt8)*((aCursor)->mCurPos) == CMP_STATEMENT_ID_PREDEFINED )     \
        {                                                                             \
            *(aId) = (aHandle)->mProtocolSentence->mCachedStatementId;                       \
            (aCursor)->mCurPos += 1;                                                  \
        }                                                                             \
        else                                                                          \
        {                                                                             \
            CMP_READ_STATEMENT_ID_BODY( aHandle, aCursor, aId, aErrorStack );         \
            (aHandle)->mProtocolSentence->mCachedStatementId = *(aId);                       \
        }                                                                             \
    }

#define CMP_READ_STATEMENT_ID_ENDIAN( aHandle, aCursor, aId, aErrorStack )            \
    {                                                                                 \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 1, aErrorStack );                     \
        STL_DASSERT( (stlUInt8)*((aCursor)->mCurPos) != CMP_STATEMENT_ID_UNDEFINED ); \
        STL_DASSERT( (stlUInt8)*((aCursor)->mCurPos) != CMP_STATEMENT_ID_PREDEFINED );\
        CMP_READ_STATEMENT_ID_BODY( aHandle, aCursor, aId, aErrorStack );             \
    }

#define CMP_READ_PERIOD( aHandle, aCursor, aValue, aErrorStack )          \
    {                                                                   \
        CMP_CHECK_N_READABLE( aHandle, aCursor, 1, aErrorStack );       \
        CMP_READ_PERIOD_SEQ( aHandle, (aValue), (aCursor)->mCurPos );                 \
        (aCursor)->mCurPos += 1;                                        \
    }

#define CMP_WRITABLE_SIZE( aCursor )   ( (aCursor)->mEndPos - (aCursor)->mCurPos )
#define CMP_READABLE_SIZE( aCursor )   ( (aCursor)->mEndPos - (aCursor)->mCurPos )

#define CMP_GET_CURRENT_POS( aCursor ) ( (aCursor)->mCurPos )

#define CMP_BIND_DATA_TYPE_FOR_OUT_BIND   ( (stlInt8)-1 ) /* data type에 없는 값을 사용해야 함 */

/** @} */

/** @} */

#endif /* _CMDEF_H_ */
