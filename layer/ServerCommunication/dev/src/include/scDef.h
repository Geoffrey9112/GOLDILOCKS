/*******************************************************************************
 * scDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: scDef.h 13675 2014-10-06 14:17:27Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _SCDEF_H_
#define _SCDEF_H_ 1

/**
 * @file scDef.h
 * @brief Gliese Server Communication Difinitions
 */

/**
 * @defgroup scInternal Gliese Server Communication Routines
 * @{
 */


/**
 * @addtogroup slBoot
 * @{
 */

/**
 * @brief Server Library error code
 */

extern stlErrorRecord       gServerCommunicationErrorTable[];

/** @} */

#define SCC_ALLOC_ELEMENT_TIMEOUT       (STL_INFINITE_TIME)


/* 1byte중 첫번째 bit는 period여부, 나머지 7bit는 sequence 이다 */
#define SCP_WRITE_PERIOD_SEQ( aHandle, aDest, aPeriod )                \
{                                                                      \
    (aDest)[0] = (*(aPeriod) | (aHandle)->mSendSequence);              \
    if( (*(stlUInt8 *)(aPeriod) & CML_PERIOD_MASK) == CML_PERIOD_END ) \
    {                                                                  \
        (aHandle)->mSendSequence = ((aHandle)->mSendSequence + 1) & 0x7f;\
    }                                                                  \
}

#define SCP_READ_PERIOD_SEQ( aHandle, aPeriod, aSrc )                                   \
{                                                                                       \
    STL_DASSERT( (*(stlUInt8 *)(aSrc) & CML_PERIOD_SEQ_MASK) == (aHandle)->mRecvSequence ); \
    CML_GET_PERIOD( *(aPeriod), *(aSrc));                                                 \
    if( (*(stlUInt8 *)(aSrc) & CML_PERIOD_MASK) == CML_PERIOD_END )                     \
    {                                                                                   \
        (aHandle)->mRecvSequence = ((aHandle)->mRecvSequence + 1) & 0x7f;\
    }                                                                                   \
}


/* header는 8byte중 4byte payload size와 1byte period 를 사용한다 */
#define SCP_WRITE_HEADER( aHandle, aPayloadSize, aPeriod )                                      \
    {                                                                                           \
        if( SCP_IS_EQUAL_ENDIAN( aHandle ) == STL_FALSE )                                       \
        {                                                                                       \
            SCP_WRITE_CONVERT_ENDIAN_INT32( (aHandle)->mProtocolSentence->mWriteStartPos, (aPayloadSize) );          \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            STL_WRITE_INT32( (aHandle)->mProtocolSentence->mWriteStartPos, (aPayloadSize) );    \
        }                                                                                       \
        SCP_WRITE_PERIOD_SEQ( aHandle, (aHandle)->mProtocolSentence->mWriteStartPos + 4, (aPeriod) );\
        SCP_HEADER_INIT( (aHandle) );                                                           \
    }



#define SCP_READ_HEADER( aHandle, aPayloadSize, aPeriod )                                       \
    {                                                                                           \
        if( SCP_IS_EQUAL_ENDIAN( aHandle ) == STL_FALSE )                                       \
        {                                                                                       \
            SCP_WRITE_CONVERT_ENDIAN_INT32( (aPayloadSize), (aHandle)->mProtocolSentence->mReadStartPos );           \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            STL_WRITE_INT32( (aPayloadSize), (aHandle)->mProtocolSentence->mReadStartPos );     \
        }                                                                                       \
        SCP_READ_PERIOD_SEQ( aHandle, (aPeriod), (aHandle)->mProtocolSentence->mReadStartPos + 4 ); \
    }



/**
 * @addtogroup scp
 * @{
 */

#define SCP_FETCH_COMMAND_SIZE       ( 2 + 1 + 8 + 2 + 8 + 4 )
#define SCP_FETCH_TRAIL_FIELD_BYTES  ( 1 )

#define SCP_2BYTE_LEN_FLAG           ( 0xFE )
#define SCP_4BYTE_LEN_FLAG           ( 0xFF )
#define SCP_1BYTE_LEN_MAX            ( 0xFD )
#define SCP_2BYTE_LEN_MAX            ( STL_INT16_MAX )

#define SCP_VAR_INT_1BYTE_MIN        ( (stlInt8)0x83 )
#define SCP_VAR_INT_1BYTE_MAX        ( (stlInt8)0x7F )
#define SCP_VAR_INT_2BYTE_FLAG       ( (stlInt8)0x82 )
#define SCP_VAR_INT_4BYTE_FLAG       ( (stlInt8)0x81 )
#define SCP_VAR_INT_8BYTE_FLAG       ( (stlInt8)0x80 )

#define SCP_STATEMENT_ID_1BYTE_MAX   ( 0xFC )
#define SCP_STATEMENT_ID_2BYTE_FLAG  ( 0xFD )
#define SCP_STATEMENT_ID_4BYTE_FLAG  ( 0xFE )
#define SCP_STATEMENT_ID_8BYTE_FLAG  ( 0xFF )

typedef struct scpCursor
{
    stlChar   *mCurPos;
    stlChar   *mEndPos;
    stlInt16   mInt16;
    stlInt32   mInt32;
} scpCursor;

#define SCP_WRITE_CONVERT_ENDIAN_INT32( aDesc, aSrc )   \
    {                                                   \
        ((stlChar*)(aDesc))[0] = ((stlChar*)(aSrc))[3]; \
        ((stlChar*)(aDesc))[1] = ((stlChar*)(aSrc))[2]; \
        ((stlChar*)(aDesc))[2] = ((stlChar*)(aSrc))[1]; \
        ((stlChar*)(aDesc))[3] = ((stlChar*)(aSrc))[0]; \
    }

#define SCP_WRITE_CONVERT_ENDIAN_INT16( aDesc, aSrc )   \
    {                                                   \
        ((stlChar*)(aDesc))[0] = ((stlChar*)(aSrc))[1]; \
        ((stlChar*)(aDesc))[1] = ((stlChar*)(aSrc))[0]; \
    }

#ifdef STL_VALGRIND
#define SCP_HEADER_INIT( aHandle )                                  \
        (aHandle)->mProtocolSentence->mWriteStartPos[5] = 0x00;     \
        (aHandle)->mProtocolSentence->mWriteStartPos[6] = 0x00;     \
        (aHandle)->mProtocolSentence->mWriteStartPos[7] = 0x00;
#else
#define SCP_HEADER_INIT( aHandle )
#endif


#define SCP_DASSERT_N_READABLE( aCursor, aBytes )                                 \
    STL_DASSERT( (aCursor)->mEndPos - (aCursor)->mCurPos >= (aBytes) )

#define SCP_CHECK_N_READABLE( aHandle, aCursor, aBytes, aEnv )             \
    {                                                                             \
        if( (aCursor)->mEndPos - (aCursor)->mCurPos < (aBytes) )                  \
        {                                                                         \
            (aHandle)->mProtocolSentence->mReadProtocolPos = (aCursor)->mCurPos;                     \
            STL_TRY( sccRecvPacket( (aHandle), (aEnv) ) == STL_SUCCESS );  \
            (aCursor)->mCurPos = (aHandle)->mProtocolSentence->mReadProtocolPos;                     \
            (aCursor)->mEndPos = (aHandle)->mProtocolSentence->mReadEndPos;                          \
        }                                                                         \
    }

#define SCP_CHECK_N_WRITABLE( aHandle, aCursor, aBytes, aEnv )             \
    {                                                                             \
        if( (aCursor)->mEndPos - (aCursor)->mCurPos < (aBytes) )                  \
        {                                                                         \
            (aHandle)->mProtocolSentence->mWriteProtocolPos = (aCursor)->mCurPos;                    \
            STL_TRY( sccFlushPacket( (aHandle), (aEnv) ) == STL_SUCCESS );  \
            (aCursor)->mCurPos = (aHandle)->mProtocolSentence->mWriteProtocolPos;                    \
            (aCursor)->mEndPos = (aHandle)->mProtocolSentence->mWriteEndPos;                         \
        }                                                                         \
    }

#define SCP_IS_EQUAL_ENDIAN( aHandle )                                                 \
    ( ( (aHandle)->mEndian == STL_PLATFORM_ENDIAN ) ? STL_TRUE : STL_FALSE )     \

#define SCP_CONVERT_ENDIAN_INT16( aDesc, aSrc )         \
    {                                                   \
        ((stlChar*)(aDesc))[0] = ((stlChar*)(aSrc))[1]; \
        ((stlChar*)(aDesc))[1] = ((stlChar*)(aSrc))[0]; \
    }

#define SCP_CONVERT_ENDIAN_INT32( aDesc, aSrc )         \
    {                                                   \
        ((stlChar*)(aDesc))[0] = ((stlChar*)(aSrc))[3]; \
        ((stlChar*)(aDesc))[1] = ((stlChar*)(aSrc))[2]; \
        ((stlChar*)(aDesc))[2] = ((stlChar*)(aSrc))[1]; \
        ((stlChar*)(aDesc))[3] = ((stlChar*)(aSrc))[0]; \
    }

#define SCP_CONVERT_ENDIAN_INT64( aDesc, aSrc )         \
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

#define SCP_CHECK_READ_INT16( aHandle, aCursor, aValue )                \
    {                                                                   \
        if( SCP_IS_EQUAL_ENDIAN( (aHandle) ) == STL_TRUE )              \
        {                                                               \
            STL_WRITE_INT16( (aValue), (aCursor)->mCurPos );            \
        }                                                               \
        else                                                            \
        {                                                               \
            SCP_CONVERT_ENDIAN_INT16( (aValue), (aCursor)->mCurPos );   \
        }                                                               \
    }

#define SCP_CHECK_WRITE_INT16( aHandle, aCursor, aValue )               \
    {                                                                   \
        if( SCP_IS_EQUAL_ENDIAN( (aHandle) ) == STL_TRUE )              \
        {                                                               \
            STL_WRITE_INT16( (aCursor)->mCurPos, (aValue) );            \
        }                                                               \
        else                                                            \
        {                                                               \
            SCP_CONVERT_ENDIAN_INT16( (aCursor)->mCurPos, (aValue) );   \
        }                                                               \
    }

#define SCP_CHECK_READ_INT32( aHandle, aCursor, aValue )                \
    {                                                                   \
        if( SCP_IS_EQUAL_ENDIAN( (aHandle) ) == STL_TRUE )              \
        {                                                               \
            STL_WRITE_INT32( (aValue), (aCursor)->mCurPos );            \
        }                                                               \
        else                                                            \
        {                                                               \
            SCP_CONVERT_ENDIAN_INT32( (aValue), (aCursor)->mCurPos );   \
        }                                                               \
    }

#define SCP_CHECK_WRITE_INT32( aHandle, aCursor, aValue )               \
    {                                                                   \
        if( SCP_IS_EQUAL_ENDIAN( (aHandle) ) == STL_TRUE )              \
        {                                                               \
            STL_WRITE_INT32( (aCursor)->mCurPos, (aValue) );            \
        }                                                               \
        else                                                            \
        {                                                               \
            SCP_CONVERT_ENDIAN_INT32( (aCursor)->mCurPos, (aValue) );   \
        }                                                               \
    }

#define SCP_CHECK_READ_INT64( aHandle, aCursor, aValue )                \
    {                                                                   \
        if( SCP_IS_EQUAL_ENDIAN( (aHandle) ) == STL_TRUE )              \
        {                                                               \
            STL_WRITE_INT64( (aValue), (aCursor)->mCurPos );            \
        }                                                               \
        else                                                            \
        {                                                               \
            SCP_CONVERT_ENDIAN_INT64( (aValue), (aCursor)->mCurPos );   \
        }                                                               \
    }

#define SCP_CHECK_WRITE_INT64( aHandle, aCursor, aValue )               \
    {                                                                   \
        if( SCP_IS_EQUAL_ENDIAN( (aHandle) ) == STL_TRUE )              \
        {                                                               \
            STL_WRITE_INT64( (aCursor)->mCurPos, (aValue) );            \
        }                                                               \
        else                                                            \
        {                                                               \
            SCP_CONVERT_ENDIAN_INT64( (aCursor)->mCurPos, (aValue) );   \
        }                                                               \
    }

#define SCP_READ_INT8( aHandle, aCursor, aValue, aEnv )          \
    {                                                                   \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 1, aEnv );       \
        STL_WRITE_INT8( (aValue), (aCursor)->mCurPos );                 \
        (aCursor)->mCurPos += 1;                                        \
    }

#define SCP_WRITE_INT8( aHandle, aCursor, aValue, aEnv )         \
    {                                                                   \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 1, aEnv );       \
        STL_WRITE_INT8( (aCursor)->mCurPos, (aValue) ) ;                \
        (aCursor)->mCurPos += 1;                                        \
    }

#define SCP_READ_INT16( aHandle, aCursor, aValue, aEnv )         \
    {                                                                   \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 2, aEnv );       \
        STL_WRITE_INT16( (aValue), (aCursor)->mCurPos );                \
        (aCursor)->mCurPos += 2;                                        \
    }

#define SCP_WRITE_INT16( aHandle, aCursor, aValue, aEnv )        \
    {                                                                   \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 2, aEnv );       \
        STL_WRITE_INT16( (aCursor)->mCurPos, (aValue) );                \
        (aCursor)->mCurPos += 2;                                        \
    }

#define SCP_READ_INT32( aHandle, aCursor, aValue, aEnv )         \
    {                                                                   \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 4, aEnv );       \
        STL_WRITE_INT32( (aValue), (aCursor)->mCurPos );                \
        (aCursor)->mCurPos += 4;                                        \
    }

#define SCP_WRITE_INT32( aHandle, aCursor, aValue, aEnv )        \
    {                                                                   \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 4, aEnv );       \
        STL_WRITE_INT32( (aCursor)->mCurPos, (aValue) );                \
        (aCursor)->mCurPos += 4;                                        \
    }

#define SCP_READ_INT64( aHandle, aCursor, aValue, aEnv )         \
    {                                                                   \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 8, aEnv );       \
        STL_WRITE_INT64( (aValue), (aCursor)->mCurPos );                \
        (aCursor)->mCurPos += 8;                                        \
    }

#define SCP_WRITE_INT64( aHandle, aCursor, aValue, aEnv )        \
    {                                                                   \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 8, aEnv );       \
        STL_WRITE_INT64( (aCursor)->mCurPos, (aValue) );                \
        (aCursor)->mCurPos += 8;                                        \
    }

#define SCP_READ_INT16_ENDIAN( aHandle, aCursor, aValue, aEnv )  \
    {                                                                   \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 2, aEnv );       \
        SCP_CHECK_READ_INT16( aHandle, aCursor, aValue );               \
        (aCursor)->mCurPos += 2;                                        \
    }

#define SCP_WRITE_INT16_ENDIAN( aHandle, aCursor, aValue, aEnv ) \
    {                                                                   \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 2, aEnv );       \
        SCP_CHECK_WRITE_INT16( aHandle, aCursor, aValue );              \
        (aCursor)->mCurPos += 2;                                        \
    }

#define SCP_READ_INT32_ENDIAN( aHandle, aCursor, aValue, aEnv )  \
    {                                                                   \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 4, aEnv );       \
        SCP_CHECK_READ_INT32( aHandle, aCursor, aValue );               \
        (aCursor)->mCurPos += 4;                                        \
    }

#define SCP_WRITE_INT32_ENDIAN( aHandle, aCursor, aValue, aEnv ) \
    {                                                                   \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 4, aEnv );       \
        SCP_CHECK_WRITE_INT32( aHandle, aCursor, aValue );              \
        (aCursor)->mCurPos += 4;                                        \
    }

#define SCP_READ_INT64_ENDIAN( aHandle, aCursor, aValue, aEnv )  \
    {                                                                   \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 8, aEnv );       \
        SCP_CHECK_READ_INT64( aHandle, aCursor, aValue );               \
        (aCursor)->mCurPos += 8;                                        \
    }

#define SCP_WRITE_INT64_ENDIAN( aHandle, aCursor, aValue, aEnv ) \
    {                                                                   \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 8, aEnv );       \
        SCP_CHECK_WRITE_INT64( aHandle, aCursor, aValue );              \
        (aCursor)->mCurPos += 8;                                        \
    }

#define SCP_SKIP_BYTES( aHandle, aCursor, aLength, aEnv )        \
    {                                                                   \
        if( (aCursor)->mEndPos - (aCursor)->mCurPos < (aLength) )       \
        {                                                               \
            STL_TRY( scpSkipNBytes( aHandle, aCursor, aLength, aEnv ) == STL_SUCCESS );    \
        }                                                               \
        else                                                            \
        {                                                               \
            (aCursor)->mCurPos += (aLength);                            \
        }                                                               \
    }

#define SCP_READ_BYTES( aHandle, aCursor, aValue, aLength, aEnv )               \
    {                                                                           \
        if( (aCursor)->mEndPos - (aCursor)->mCurPos < (aLength) )               \
        {                                                                       \
            STL_TRY( scpReadNBytes( aHandle, aCursor, aValue, aLength, aEnv ) == STL_SUCCESS );\
        }                                                                       \
        else                                                                    \
        {                                                                       \
            STL_WRITE_BYTES( aValue, (aCursor)->mCurPos, (aLength) );           \
            (aCursor)->mCurPos += (aLength);                                    \
        }                                                                       \
    }

#define SCP_WRITE_BYTES( aHandle, aCursor, aValue, aLength, aEnv )              \
    {                                                                           \
        if( (aCursor)->mEndPos - (aCursor)->mCurPos < (aLength) )               \
        {                                                                       \
            STL_TRY( scpWriteNBytes( aHandle, aCursor, aValue, aLength, aEnv ) == STL_SUCCESS );\
        }                                                                       \
        else                                                                    \
        {                                                                       \
            STL_WRITE_BYTES( (aCursor)->mCurPos, aValue, (aLength) );           \
            (aCursor)->mCurPos += (aLength);                                    \
        }                                                                       \
    }

#define SCP_READ_VAR_LEN( aHandle, aCursor, aLength, aEnv )                     \
    {                                                                           \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 1, aEnv );                      \
        if( *((stlUInt8*)((aCursor)->mCurPos)) == SCP_2BYTE_LEN_FLAG )          \
        {                                                                       \
            SCP_DASSERT_N_READABLE( aCursor, 3 );                               \
            STL_WRITE_INT16( &((aCursor)->mInt16), (aCursor)->mCurPos + 1 );    \
            (aLength) = (aCursor)->mInt16;                                      \
            (aCursor)->mCurPos += 3;                                            \
        }                                                                       \
        else if( *((stlUInt8*)((aCursor)->mCurPos)) == SCP_4BYTE_LEN_FLAG )     \
        {                                                                       \
            SCP_DASSERT_N_READABLE( aCursor, 5 );                               \
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

#define SCP_READ_N_VAR( aHandle, aCursor, aValue, aLength, aEnv )        \
    {                                                                    \
        SCP_READ_VAR_LEN( aHandle, aCursor, aLength, aEnv );             \
        SCP_READ_BYTES( aHandle, aCursor, aValue, aLength, aEnv );       \
    }

#define SCP_READ_VAR_LEN_ENDIAN( aHandle, aCursor, aLength, aEnv )                          \
    {                                                                                       \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 1, aEnv );                                  \
        if( *((stlUInt8*)((aCursor)->mCurPos)) == SCP_2BYTE_LEN_FLAG )                      \
        {                                                                                   \
            SCP_DASSERT_N_READABLE( aCursor, 3 );                                           \
            if( SCP_IS_EQUAL_ENDIAN( aHandle ) == STL_TRUE )                                \
            {                                                                               \
                STL_WRITE_INT16( &((aCursor)->mInt16), (aCursor)->mCurPos + 1 );            \
            }                                                                               \
            else                                                                            \
            {                                                                               \
                SCP_CONVERT_ENDIAN_INT16( &((aCursor)->mInt16), (aCursor)->mCurPos + 1 );   \
            }                                                                               \
            (aLength) = (aCursor)->mInt16;                                                  \
            (aCursor)->mCurPos += 3;                                                        \
        }                                                                                   \
        else if( *((stlUInt8*)((aCursor)->mCurPos)) == SCP_4BYTE_LEN_FLAG )                 \
        {                                                                                   \
            SCP_DASSERT_N_READABLE( aCursor, 5 );                                           \
            if( SCP_IS_EQUAL_ENDIAN( aHandle ) == STL_TRUE )                                \
            {                                                                               \
                STL_WRITE_INT32( &((aCursor)->mInt32), (aCursor)->mCurPos + 1 );            \
            }                                                                               \
            else                                                                            \
            {                                                                               \
                SCP_CONVERT_ENDIAN_INT32( &((aCursor)->mInt16), (aCursor)->mCurPos + 1 );   \
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

#define SCP_READ_N_VAR_ENDIAN( aHandle, aCursor, aValue, aLength, aEnv ) \
    {                                                                    \
        SCP_READ_VAR_LEN_ENDIAN( aHandle, aCursor, aLength, aEnv );      \
        SCP_READ_BYTES( aHandle, aCursor, aValue, aLength, aEnv );       \
    }

#define SCP_WRITE_N_VAR( aHandle, aCursor, aValue, aLength, aEnv )                      \
    {                                                                                   \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 5, aEnv );                              \
        if( (aLength) > SCP_2BYTE_LEN_MAX )                                             \
        {                                                                               \
            *((aCursor)->mCurPos) = SCP_4BYTE_LEN_FLAG;                                 \
            (aCursor)->mCurPos += 1;                                                    \
            (aCursor)->mInt32 = (stlInt32)(aLength);                                    \
            SCP_WRITE_INT32( aHandle, aCursor, &((aCursor)->mInt32), aEnv );            \
        }                                                                               \
        else if( (aLength) > SCP_1BYTE_LEN_MAX )                                        \
        {                                                                               \
            *((aCursor)->mCurPos) = SCP_2BYTE_LEN_FLAG;                                 \
            (aCursor)->mCurPos += 1;                                                    \
            (aCursor)->mInt16 = (stlInt16)(aLength);                                    \
            SCP_WRITE_INT16( aHandle, aCursor, &((aCursor)->mInt16), aEnv );            \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            *((aCursor)->mCurPos) = (stlInt8)(aLength);                                 \
            (aCursor)->mCurPos += 1;                                                    \
        }                                                                               \
        SCP_WRITE_BYTES( aHandle, aCursor, aValue, aLength, aEnv );                     \
    }

#define SCP_WRITE_N_VAR16( aHandle, aCursor, aValue, aLength, aEnv )                    \
    {                                                                                   \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 5, aEnv );                              \
        if( (aLength) > SCP_1BYTE_LEN_MAX )                                             \
        {                                                                               \
            *((aCursor)->mCurPos) = SCP_2BYTE_LEN_FLAG;                                 \
            (aCursor)->mCurPos += 1;                                                    \
            (aCursor)->mInt16 = (stlInt16)(aLength);                                    \
            SCP_WRITE_INT16( aHandle, aCursor, &((aCursor)->mInt16), aEnv );            \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            *((aCursor)->mCurPos) = (stlInt8)(aLength);                                 \
            (aCursor)->mCurPos += 1;                                                    \
        }                                                                               \
        SCP_WRITE_BYTES( aHandle, aCursor, aValue, aLength, aEnv );                     \
    }

#define SCP_WRITE_N_VAR_ENDIAN( aHandle, aCursor, aValue, aLength, aEnv )                \
    {                                                                                    \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 5, aEnv );                               \
        if( (aLength) > SCP_2BYTE_LEN_MAX )                                              \
        {                                                                                \
            *((aCursor)->mCurPos) = SCP_4BYTE_LEN_FLAG;                                  \
            (aCursor)->mCurPos += 1;                                                     \
            (aCursor)->mInt32 = (stlInt32)(aLength);                                     \
            SCP_WRITE_INT32_ENDIAN( aHandle, aCursor, &((aCursor)->mInt32), aEnv );      \
        }                                                                                \
        else if( (aLength) > SCP_1BYTE_LEN_MAX )                                         \
        {                                                                                \
            *((aCursor)->mCurPos) = SCP_2BYTE_LEN_FLAG;                                  \
            (aCursor)->mCurPos += 1;                                                     \
            (aCursor)->mInt16 = (stlInt16)(aLength);                                     \
            SCP_WRITE_INT16_ENDIAN( aHandle, aCursor, &((aCursor)->mInt16), aEnv );      \
        }                                                                                \
        else                                                                             \
        {                                                                                \
            *((aCursor)->mCurPos) = (stlInt8)(aLength);                                  \
            (aCursor)->mCurPos += 1;                                                     \
        }                                                                                \
        SCP_WRITE_BYTES( aHandle, aCursor, aValue, aLength, aEnv );                      \
    }

#define SCP_READ_1_VAR( aHandle, aCursor, aValue, aLength, aEnv )        \
    {                                                                    \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 1, aEnv );               \
        (aLength) = *((stlUInt8*)((aCursor)->mCurPos));                  \
        (aCursor)->mCurPos += 1;                                         \
        STL_DASSERT( (aLength) < 0xFE );                                 \
        SCP_READ_BYTES( aHandle, aCursor, aValue, aLength, aEnv );       \
    }

#define SCP_WRITE_1_VAR( aHandle, aCursor, aValue, aLength, aEnv )       \
    {                                                                    \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 1, aEnv );               \
        *((aCursor)->mCurPos) = (aLength);                               \
        (aCursor)->mCurPos += 1;                                         \
        SCP_WRITE_BYTES( aHandle, aCursor, aValue, aLength, aErrorStaak );\
    }

#define SCP_READ_VAR_STRING( aHandle, aCursor, aValue, aLength, aBufferLength, aEnv )        \
    {                                                                                        \
        SCP_READ_N_VAR( aHandle, aCursor, aValue, aLength, aEnv );                           \
        STL_DASSERT( (aLength) <= aBufferLength );                                           \
        (aValue)[(aLength)] = '\0';                                                          \
    }

#define SCP_READ_VAR_STRING_ENDIAN( aHandle, aCursor, aValue, aLength, aBufferLength, aEnv ) \
    {                                                                                        \
        SCP_READ_N_VAR_ENDIAN( aHandle, aCursor, aValue, aLength, aEnv );                    \
        STL_DASSERT( (aLength) <= aBufferLength );                                           \
        (aValue)[(aLength)] = '\0';                                                          \
    }

#define SCP_WRITE_VAR_INT16( aHandle, aCursor, aValue, aEnv )                       \
    {                                                                               \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 9, aEnv );                          \
        if( ((stlInt64)*(aValue) >= SCP_VAR_INT_1BYTE_MIN) &&                       \
            ((stlInt64)*(aValue) <= SCP_VAR_INT_1BYTE_MAX) )                        \
        {                                                                           \
            *((aCursor)->mCurPos) = *(aValue);                                      \
            (aCursor)->mCurPos += 1;                                                \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            *((aCursor)->mCurPos) = SCP_VAR_INT_2BYTE_FLAG;                         \
            *((aCursor)->mCurPos + 1) = (*(aValue) & 0xFF00) >> 8;                  \
            *((aCursor)->mCurPos + 2) = *(aValue) & 0xFF;                           \
            (aCursor)->mCurPos += 3;                                                \
        }                                                                           \
    }

#define SCP_WRITE_VAR_INT32( aHandle, aCursor, aValue, aEnv )                       \
    {                                                                               \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 9, aEnv );                          \
        if( ((stlInt64)*(aValue) >= SCP_VAR_INT_1BYTE_MIN) &&                       \
            ((stlInt64)*(aValue) <= SCP_VAR_INT_1BYTE_MAX) )                        \
        {                                                                           \
            *((aCursor)->mCurPos) = *(aValue);                                      \
            (aCursor)->mCurPos += 1;                                                \
        }                                                                           \
        else if( ((stlInt64)*(aValue) >= STL_INT16_MIN ) &&                         \
                 ((stlInt64)*(aValue) <= STL_INT16_MAX) )                           \
        {                                                                           \
            *((aCursor)->mCurPos) = SCP_VAR_INT_2BYTE_FLAG;                         \
            *((aCursor)->mCurPos + 1) = (*(aValue) & 0xFF00) >> 8;                  \
            *((aCursor)->mCurPos + 2) = *(aValue) & 0xFF;                           \
            (aCursor)->mCurPos += 3;                                                \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            *((aCursor)->mCurPos) = SCP_VAR_INT_4BYTE_FLAG;                         \
            *((aCursor)->mCurPos + 1) = (*(aValue) & 0xFF000000) >> 24;             \
            *((aCursor)->mCurPos + 2) = (*(aValue) & 0xFF0000) >> 16;               \
            *((aCursor)->mCurPos + 3) = (*(aValue) & 0xFF00) >> 8;                  \
            *((aCursor)->mCurPos + 4) = *(aValue) & 0xFF;                           \
            (aCursor)->mCurPos += 5;                                                \
        }                                                                           \
    }

#define SCP_WRITE_VAR_UINT32( aHandle, aCursor, aValue, aEnv )                      \
    {                                                                               \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 9, aEnv );                          \
        if( (stlInt64)*(aValue) <= SCP_VAR_INT_1BYTE_MAX )                                 \
        {                                                                                  \
            *((aCursor)->mCurPos) = *(aValue);                                             \
            (aCursor)->mCurPos += 1;                                                       \
        }                                                                                  \
        else if( (stlInt64)*(aValue) <= STL_INT16_MAX )                                    \
        {                                                                                  \
            *((aCursor)->mCurPos) = SCP_VAR_INT_2BYTE_FLAG;                                \
            *((aCursor)->mCurPos + 1) = (*(aValue) & 0xFF00) >> 8;                         \
            *((aCursor)->mCurPos + 2) = *(aValue) & 0xFF;                                  \
            (aCursor)->mCurPos += 3;                                                       \
        }                                                                                  \
        else                                                                               \
        {                                                                                  \
            *((aCursor)->mCurPos) = SCP_VAR_INT_4BYTE_FLAG;                                \
            *((aCursor)->mCurPos + 1) = (*(aValue) & 0xFF000000) >> 24;                    \
            *((aCursor)->mCurPos + 2) = (*(aValue) & 0xFF0000) >> 16;                      \
            *((aCursor)->mCurPos + 3) = (*(aValue) & 0xFF00) >> 8;                         \
            *((aCursor)->mCurPos + 4) = *(aValue) & 0xFF;                                  \
            (aCursor)->mCurPos += 5;                                                       \
        }                                                                                  \
    }

#define SCP_WRITE_VAR_INT64( aHandle, aCursor, aValue, aEnv )                            \
    {                                                                                    \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 9, aEnv );                               \
        if( ((stlInt64)*(aValue) >= SCP_VAR_INT_1BYTE_MIN) &&                                   \
            ((stlInt64)*(aValue) <= SCP_VAR_INT_1BYTE_MAX) )                                    \
        {                                                                                       \
            *((aCursor)->mCurPos) = *(aValue);                                                  \
            (aCursor)->mCurPos += 1;                                                            \
        }                                                                                       \
        else if( ((stlInt64)*(aValue) >= STL_INT16_MIN ) &&                                     \
                 ((stlInt64)*(aValue) <= STL_INT16_MAX) )                                       \
        {                                                                                       \
            *((aCursor)->mCurPos) = SCP_VAR_INT_2BYTE_FLAG;                                     \
            *((aCursor)->mCurPos + 1) = (*(aValue) & 0xFF00) >> 8;                              \
            *((aCursor)->mCurPos + 2) = *(aValue) & 0xFF;                                       \
            (aCursor)->mCurPos += 3;                                                            \
        }                                                                                       \
        else if( ((stlInt64)*(aValue) >= STL_INT32_MIN) &&                                      \
                 ((stlInt64)*(aValue) <= STL_INT32_MAX) )                                       \
        {                                                                                       \
            *((aCursor)->mCurPos) = SCP_VAR_INT_4BYTE_FLAG;                                     \
            *((aCursor)->mCurPos + 1) = (*(aValue) & 0xFF000000) >> 24;                         \
            *((aCursor)->mCurPos + 2) = (*(aValue) & 0xFF0000) >> 16;                           \
            *((aCursor)->mCurPos + 3) = (*(aValue) & 0xFF00) >> 8;                              \
            *((aCursor)->mCurPos + 4) = *(aValue) & 0xFF;                                       \
            (aCursor)->mCurPos += 5;                                                            \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            *((aCursor)->mCurPos) = SCP_VAR_INT_8BYTE_FLAG;                                     \
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

#define SCP_READ_VAR_INT16( aHandle, aCursor, aValue, aEnv )                \
    {                                                                       \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 1, aEnv );                  \
        if( *((stlInt8*)((aCursor)->mCurPos)) >= SCP_VAR_INT_1BYTE_MIN )    \
        {                                                                   \
            *(aValue) = *((stlInt8*)((aCursor)->mCurPos));                  \
            (aCursor)->mCurPos += 1;                                        \
        }                                                                   \
        else if( *((stlInt8*)((aCursor)->mCurPos)) == SCP_VAR_INT_2BYTE_FLAG ) \
        {                                                                   \
            SCP_DASSERT_N_READABLE( aCursor, 3 );                           \
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

#define SCP_READ_VAR_INT32( aHandle, aCursor, aValue, aEnv )                \
    {                                                                       \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 1, aEnv );                  \
        if( *((stlInt8*)((aCursor)->mCurPos)) >= SCP_VAR_INT_1BYTE_MIN )    \
        {                                                                   \
            *(aValue) = *((stlInt8*)((aCursor)->mCurPos));                  \
            (aCursor)->mCurPos += 1;                                        \
        }                                                                   \
        else if( *((stlInt8*)((aCursor)->mCurPos)) == SCP_VAR_INT_2BYTE_FLAG ) \
        {                                                                   \
            SCP_DASSERT_N_READABLE( aCursor, 3 );                           \
            *(aValue) = (((*((stlUInt8*)((aCursor)->mCurPos + 1))) << 8) |  \
                         (*((stlUInt8*)((aCursor)->mCurPos + 2))));         \
            (aCursor)->mCurPos += 3;                                        \
        }                                                                   \
        else if( *((stlInt8*)((aCursor)->mCurPos)) == SCP_VAR_INT_4BYTE_FLAG ) \
        {                                                                   \
            SCP_DASSERT_N_READABLE( aCursor, 5 );                           \
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

#define SCP_READ_VAR_INT64( aHandle, aCursor, aValue, aEnv )                \
    {                                                                       \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 1, aEnv );                  \
        if( *((stlInt8*)((aCursor)->mCurPos)) >= SCP_VAR_INT_1BYTE_MIN )    \
        {                                                                   \
            *(aValue) = *((stlInt8*)((aCursor)->mCurPos));                  \
            (aCursor)->mCurPos += 1;                                        \
        }                                                                   \
        else if( *((stlInt8*)((aCursor)->mCurPos)) == SCP_VAR_INT_2BYTE_FLAG ) \
        {                                                                   \
            SCP_DASSERT_N_READABLE( aCursor, 3 );                           \
            *(aValue) = (((*((stlUInt8*)((aCursor)->mCurPos + 1))) << 8) |  \
                         (*((stlUInt8*)((aCursor)->mCurPos + 2))));         \
            (aCursor)->mCurPos += 3;                                        \
        }                                                                   \
        else if( *((stlInt8*)((aCursor)->mCurPos)) == SCP_VAR_INT_4BYTE_FLAG ) \
        {                                                                   \
            SCP_DASSERT_N_READABLE( aCursor, 5 );                           \
            *(aValue) = (((*((stlUInt8*)((aCursor)->mCurPos + 1))) << 24) | \
                         ((*((stlUInt8*)((aCursor)->mCurPos + 2))) << 16) | \
                         ((*((stlUInt8*)((aCursor)->mCurPos + 3))) << 8) |  \
                         (*((stlUInt8*)((aCursor)->mCurPos + 4))));         \
            (aCursor)->mCurPos += 5;                                        \
        }                                                                   \
        else                                                                \
        {                                                                   \
            SCP_DASSERT_N_READABLE( aCursor, 9 );                           \
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
#define SCP_STATEMENT_ID_UNDEFINED          (SCP_STATEMENT_ID_8BYTE_FLAG)
#define SCP_STATEMENT_ID_PREDEFINED         (SCP_STATEMENT_ID_4BYTE_FLAG)

#define SCP_WRITE_STATEMENT_ID_BODY( aHandle, aCursor, aId, aEnv )                           \
    {                                                                                               \
        if( ((*((stlUInt64*)(aId)) & STL_UINT64_C(0xFFFF000000000000)) >> 48) <= SCP_STATEMENT_ID_1BYTE_MAX ) \
        {                                                                                           \
            *((aCursor)->mCurPos) = ((*((stlUInt64*)(aId)) & STL_UINT64_C(0xFFFF000000000000)) >> 48); \
            (aCursor)->mCurPos += 1;                                                                \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            *((aCursor)->mCurPos) = SCP_STATEMENT_ID_2BYTE_FLAG;                                    \
            *((aCursor)->mCurPos + 1) = (*((stlUInt64*)(aId)) & STL_UINT64_C(0xFF00000000000000)) >> 56; \
            *((aCursor)->mCurPos + 2) = (*(aId) & STL_INT64_C(0x00FF000000000000)) >> 48;           \
            (aCursor)->mCurPos += 3;                                                                \
        }                                                                                           \
        if( (*(aId) & STL_INT64_C(0x0000FFFFFFFFFFFF)) <= SCP_STATEMENT_ID_1BYTE_MAX )              \
        {                                                                                           \
            *((aCursor)->mCurPos) = *(aId) & 0xFF;                                                  \
            (aCursor)->mCurPos += 1;                                                                \
        }                                                                                           \
        else if( (*(aId) & STL_INT64_C(0x0000FFFFFFFFFFFF)) <= STL_UINT16_MAX )                     \
        {                                                                                           \
            *((aCursor)->mCurPos) = SCP_STATEMENT_ID_2BYTE_FLAG;                                    \
            *((aCursor)->mCurPos + 1) = (*(aId) & 0xFF00) >> 8;                                     \
            *((aCursor)->mCurPos + 2) = (*(aId) & 0x00FF);                                          \
            (aCursor)->mCurPos += 3;                                                                \
        }                                                                                           \
        else if( (*(aId) & STL_INT64_C(0x0000FFFFFFFFFFFF)) <= STL_UINT32_MAX )                     \
        {                                                                                           \
            *((aCursor)->mCurPos) = SCP_STATEMENT_ID_4BYTE_FLAG;                                    \
            *((aCursor)->mCurPos + 1) = (*(aId) & 0xFF000000) >> 24;                                \
            *((aCursor)->mCurPos + 2) = (*(aId) & 0x00FF0000) >> 16;                                \
            *((aCursor)->mCurPos + 3) = (*(aId) & 0x0000FF00) >> 8;                                 \
            *((aCursor)->mCurPos + 4) = (*(aId) & 0x000000FF);                                      \
            (aCursor)->mCurPos += 5;                                                                \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            *((aCursor)->mCurPos) = SCP_STATEMENT_ID_8BYTE_FLAG;                                    \
            *((aCursor)->mCurPos + 1) = (*(aId) & STL_INT64_C(0xFF0000000000)) >> 40;               \
            *((aCursor)->mCurPos + 2) = (*(aId) & STL_INT64_C(0xFF00000000)) >> 32;                 \
            *((aCursor)->mCurPos + 3) = (*(aId) & 0xFF000000) >> 24;                                \
            *((aCursor)->mCurPos + 4) = (*(aId) & 0xFF0000) >> 16;                                  \
            *((aCursor)->mCurPos + 5) = (*(aId) & 0xFF00) >> 8;                                     \
            *((aCursor)->mCurPos + 6) = (*(aId) & 0xFF);                                            \
            (aCursor)->mCurPos += 7;                                                                \
        }                                                                                           \
    }

#define SCP_WRITE_STATEMENT_ID( aHandle, aCursor, aId, aEnv )                                 \
    {                                                                                                \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 10, aEnv );                                   \
        STL_DASSERT( *(aId) != CML_STATEMENT_ID_UNDEFINED );                                         \
        SCP_WRITE_STATEMENT_ID_BODY( aHandle, aCursor, aId, aEnv );                           \
    }

#define SCP_WRITE_STATEMENT_ID_ENDIAN( aHandle, aCursor, aId, aEnv )                          \
    {                                                                                                \
        SCP_CHECK_N_WRITABLE( aHandle, aCursor, 10, aEnv );                                   \
        if( *(aId) == CML_STATEMENT_ID_UNDEFINED )                                                   \
        {                                                                                            \
            *((aCursor)->mCurPos) = SCP_STATEMENT_ID_UNDEFINED;                                      \
            (aCursor)->mCurPos += 1;                                                                 \
        }                                                                                            \
        else if( *(aId) == (aHandle)->mProtocolSentence->mCachedStatementId )                               \
        {                                                                                            \
            *((aCursor)->mCurPos) = SCP_STATEMENT_ID_PREDEFINED;                                     \
            (aCursor)->mCurPos += 1;                                                                 \
        }                                                                                            \
        else                                                                                         \
        {                                                                                            \
            SCP_WRITE_STATEMENT_ID_BODY( aHandle, aCursor, aId, aEnv );                       \
            (aHandle)->mProtocolSentence->mCachedStatementId = *(aId);                                      \
        }                                                                                            \
    }

#define SCP_READ_STATEMENT_ID_BODY( aHandle, aCursor, aId, aEnv )              \
    {                                                                                 \
        if( *((stlUInt8*)((aCursor)->mCurPos)) <= SCP_STATEMENT_ID_1BYTE_MAX )        \
        {                                                                             \
            *(aId) = *((stlUInt8*)((aCursor)->mCurPos));                              \
            (aCursor)->mCurPos += 1;                                                  \
        }                                                                             \
        else if( *((stlUInt8*)((aCursor)->mCurPos)) == SCP_STATEMENT_ID_2BYTE_FLAG )  \
        {                                                                             \
            SCP_CHECK_N_READABLE( aHandle, aCursor, 3, aEnv );                 \
            *(aId) = (*((stlUInt8*)((aCursor)->mCurPos + 1)) << 8) +                  \
                     (*((stlUInt8*)((aCursor)->mCurPos + 2)));                        \
            (aCursor)->mCurPos += 3;                                                  \
        }                                                                             \
        *(aId) = *(aId) << 48;                                                        \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 1, aEnv );                     \
        if( *((stlUInt8*)((aCursor)->mCurPos)) <= SCP_STATEMENT_ID_1BYTE_MAX )        \
        {                                                                             \
            *(aId) += *((stlUInt8*)((aCursor)->mCurPos));                             \
            (aCursor)->mCurPos += 1;                                                  \
        }                                                                             \
        else if( (stlUInt8)*((aCursor)->mCurPos) == SCP_STATEMENT_ID_2BYTE_FLAG )     \
        {                                                                             \
            SCP_CHECK_N_READABLE( aHandle, aCursor, 3, aEnv );                 \
            *(aId) += (*((stlUInt8*)((aCursor)->mCurPos + 1)) << 8) +                 \
                      (*((stlUInt8*)((aCursor)->mCurPos + 2)));                       \
            (aCursor)->mCurPos += 3;                                                  \
        }                                                                             \
        else if( (stlUInt8)*((aCursor)->mCurPos) == SCP_STATEMENT_ID_4BYTE_FLAG )     \
        {                                                                             \
            SCP_CHECK_N_READABLE( aHandle, aCursor, 5, aEnv );                 \
            *(aId) += (((stlUInt32)(*((stlUInt8*)((aCursor)->mCurPos + 1)))) << 24) + \
                      (((stlUInt32)(*((stlUInt8*)((aCursor)->mCurPos + 2)))) << 16) + \
                      (((stlUInt32)(*((stlUInt8*)((aCursor)->mCurPos + 3)))) << 8) +  \
                      (((stlUInt32)(*((stlUInt8*)((aCursor)->mCurPos + 4)))));        \
            (aCursor)->mCurPos += 5;                                                  \
        }                                                                             \
        else                                                                          \
        {                                                                             \
            SCP_CHECK_N_READABLE( aHandle, aCursor, 7, aEnv );                 \
            *(aId) += (((stlUInt64)(*((stlUInt8*)((aCursor)->mCurPos + 1)))) << 40) + \
                      (((stlUInt64)(*((stlUInt8*)((aCursor)->mCurPos + 2)))) << 32) + \
                      (((stlUInt64)(*((stlUInt8*)((aCursor)->mCurPos + 3)))) << 24) + \
                      (((stlUInt64)(*((stlUInt8*)((aCursor)->mCurPos + 4)))) << 16) + \
                      (((stlUInt64)(*((stlUInt8*)((aCursor)->mCurPos + 5)))) << 8) +  \
                      (((stlUInt64)(*((stlUInt8*)((aCursor)->mCurPos + 6)))));        \
            (aCursor)->mCurPos += 7;                                                  \
        }                                                                             \
    }

#define SCP_READ_STATEMENT_ID( aHandle, aCursor, aId, aEnv )                   \
    {                                                                                 \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 1, aEnv );                     \
        if( (stlUInt8)*((aCursor)->mCurPos) == SCP_STATEMENT_ID_UNDEFINED )           \
        {                                                                             \
            *(aId) = CML_STATEMENT_ID_UNDEFINED;                                      \
            (aCursor)->mCurPos += 1;                                                  \
        }                                                                             \
        else if( (stlUInt8)*((aCursor)->mCurPos) == SCP_STATEMENT_ID_PREDEFINED )     \
        {                                                                             \
            *(aId) = (aHandle)->mProtocolSentence->mCachedStatementId;                       \
            (aCursor)->mCurPos += 1;                                                  \
        }                                                                             \
        else                                                                          \
        {                                                                             \
            SCP_READ_STATEMENT_ID_BODY( aHandle, aCursor, aId, aEnv );         \
            (aHandle)->mProtocolSentence->mCachedStatementId = *(aId);                       \
        }                                                                             \
    }

#define SCP_READ_STATEMENT_ID_ENDIAN( aHandle, aCursor, aId, aEnv )            \
    {                                                                                 \
        SCP_CHECK_N_READABLE( aHandle, aCursor, 1, aEnv );                     \
        STL_DASSERT( (stlUInt8)*((aCursor)->mCurPos) != SCP_STATEMENT_ID_UNDEFINED ); \
        STL_DASSERT( (stlUInt8)*((aCursor)->mCurPos) != SCP_STATEMENT_ID_PREDEFINED );\
        SCP_READ_STATEMENT_ID_BODY( aHandle, aCursor, aId, aEnv );             \
    }



#define SCP_WRITABLE_SIZE( aCursor )   ( (aCursor)->mEndPos - (aCursor)->mCurPos )
#define SCP_READABLE_SIZE( aCursor )   ( (aCursor)->mEndPos - (aCursor)->mCurPos )

#define SCP_GET_CURRENT_POS( aCursor ) ( (aCursor)->mCurPos )

#define SCP_BIND_DATA_TYPE_FOR_OUT_BIND   ( (stlInt8)-1 ) /* data type에 없는 값을 사용해야 함 */

/** @} */


/** @} */

/** @} */

#endif /* _SCDEF_H_ */
