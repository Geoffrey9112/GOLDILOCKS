/*******************************************************************************
 * str.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: str.h 177 2011-03-09 07:02:01Z mycomman $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _STR_H_
#define _STR_H_ 1

#include <stl.h>
#include <stc.h>

STL_BEGIN_DECLS

#define STR_NULCHAR  '\0'

typedef stlInt32   strStatus;
/**
 * Structure used by the variable-formatter routines.
 */
typedef struct strVformatterBuffer
{
    /** The start position */
    stlChar * mStartPosition;
    /** The current position */
    stlChar * mCurrentPosition;
    /** The end position of the format string */
    stlChar * mEndPosition;
} strVformatterBuffer;

typedef enum strDirection
{
    STR_LEFT,
    STR_RIGHT
} strDirection;

typedef enum strVarTypeEnum
{
    STR_IS_INT64,
    STR_IS_INT16,
    STR_IS_INT32
} strVarTypeEnum;

stlInt32  strVsnprintf(stlChar       *aString,
                       stlSize        aSize,
                       const stlChar *aFormat,
                       va_list        aVarArgList);

stlStatus strStrToFloat64(const stlChar  *aNumPtr,
                          stlInt64        aLength,
                          stlChar       **aEndPtr,
                          stlFloat64     *aResult,
                          stlErrorStack  *aErrorStack);

stlStatus strStrToFloat32(const stlChar  *aNumPtr,
                          stlInt64        aLength,
                          stlChar       **aEndPtr,
                          stlFloat32     *aResult,
                          stlErrorStack  *aErrorStack);

stlStatus strStrToInt64(const stlChar  *aNumPtr,
                        stlInt64        aLength,
                        stlChar       **aEndPtr,
                        stlInt32        aBase,
                        stlInt64       *aResult,
                        stlErrorStack  *aErrorStack);

stlStatus strStrToUInt64(const stlChar  *aNumPtr,
                         stlInt64        aLength,
                         stlChar       **aEndPtr,
                         stlInt32        aBase,
                         stlUInt64      *aResult,
                         stlErrorStack  *aErrorStack);

stlInt32 strVprintf( const stlChar * aFormat,
                     va_list         aVarArgList );
    
stlInt32 strVformatter( void                (* aFlushFunc)(strVformatterBuffer *),
                        strVformatterBuffer  * aVBuffer,
                        stlChar              * aFormat,
                        va_list                aVarArgList );

stlChar * strGetDefaultCharacterSet();

stlStatus strGetPassword( const stlChar * aPrompt,
                          stlBool         aEcho,
                          stlChar       * aBuffer,
                          stlSize         aBufferLength,
                          stlErrorStack * aErrorStack );


STL_END_DECLS

#endif /* _STR_H_ */
