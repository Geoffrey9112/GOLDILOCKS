/*******************************************************************************
 * stlStrings.h
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


#ifndef _STLSTRINGS_H_
#define _STLSTRINGS_H_ 1

/**
 * @file stlStrings.h
 * @brief Standard Layer Strings Routines
 */

#include <stlDef.h>


STL_BEGIN_DECLS


/**
 * @defgroup stlStrings Strings Routines
 * @ingroup STL 
 * @{
 */

/**
 * @defgroup stlStringFormat String Format
 * @ingroup stlStrings
 * @par Description
 * parameter, flags, width, precision과 type의 스펙은 printf()와 동일하며,<BR>
 * 다만 length에 대한 스펙이 printf()에 비해서 제한된 부분만 지원한다.<BR>
 * printf는 'hh', 'h', 'l', 'll', 'L', 'z', 'j', 't'를 지원하는 반면 <BR>
 * Standard layer에서는 'h'와 'l'만 지원한다.<BR>
 * 또한, printf의 출력 형식중에 'A', 'a'는 Standard layer에서는 지원하지 않는다.
 * 
 * @par syntax
 * @code
 * %[parameter][flags][width][.precision][length]type
 * @endcode
 * @par length
 * @code
 * 'h' : 16비트 정수형
 * ' ' : 32비트 정수형 (length를 지정하지 않은 경우)
 * 'l' : 64비트 정수형
 * @endcode
 * @remark
 *     플랫폼에 따라서 크기가 바뀌는 타입(stlSize, stlSsize, stlOffset, ... )은 <BR>
 *     64비트 정수형 length('l')를 사용해야 한다.
 * @see printf()
 */

/** @see isalnum */
#define stlIsalnum(c) ((isalnum(((unsigned char)(c))) == 0) ? STL_FALSE : STL_TRUE)
/** @see isalpha */
#define stlIsalpha(c) ((isalpha(((unsigned char)(c))) == 0) ? STL_FALSE : STL_TRUE)
/** @see iscntrl */
#define stlIscntrl(c) ((iscntrl(((unsigned char)(c))) == 0) ? STL_FALSE : STL_TRUE)
/** @see isdigit */
#define stlIsdigit(c) ((isdigit(((unsigned char)(c))) == 0) ? STL_FALSE : STL_TRUE)
/** @see isgraph */
#define stlIsgraph(c) ((isgraph(((unsigned char)(c))) == 0) ? STL_FALSE : STL_TRUE)
/** @see islower*/
#define stlIslower(c) ((islower(((unsigned char)(c))) == 0) ? STL_FALSE : STL_TRUE)
/** @see isascii */
#define stlIsascii(c) (((c) & ~0x7f)==0)
/** @see isprint */
#define stlIsprint(c) ((isprint(((unsigned char)(c))) == 0) ? STL_FALSE : STL_TRUE)
/** @see ispunct */
#define stlIspunct(c) ((ispunct(((unsigned char)(c))) == 0) ? STL_FALSE : STL_TRUE)
/** @see isspace */
#define stlIsspace(c) ((isspace(((unsigned char)(c))) == 0) ? STL_FALSE : STL_TRUE)
/** @see isupper */
#define stlIsupper(c) ((isupper(((unsigned char)(c))) == 0) ? STL_FALSE : STL_TRUE)
/** @see isxdigit */
#define stlIsxdigit(c) ((isxdigit(((unsigned char)(c))) == 0) ? STL_FALSE : STL_TRUE)
/** @see tolower */
#define stlTolower(c) (tolower(((unsigned char)(c))))
/** @see toupper */
#define stlToupper(c) (toupper(((unsigned char)(c))))
/** @see isblank */
#define stlIsblank(c) ((isblank(((unsigned char)(c))) == 0) ? STL_FALSE : STL_TRUE)

#define stlVarStart( aVarArgList, aFormat ) va_start( (aVarArgList), (aFormat) )
#define stlVarEnd( aVarArgList ) va_end( (aVarArgList) )

stlSize   stlStrlen(const stlChar *s);
stlChar  *stlStrcpy(stlChar *aDest, const stlChar *aSrc);
stlChar  *stlStrncpy(stlChar *aDest, const stlChar *aSrc, stlSize aLen);
stlChar  *stlStrchr(const stlChar *aStr, stlChar aChar);
stlChar  *stlStrrchr(const stlChar *aStr, stlChar aChar);
stlInt32  stlStrcmp(const stlChar *aStr1, const stlChar *aStr2);
stlInt32  stlStrncmp(const stlChar *aStr1, const stlChar *aStr2, stlSize aLen);
stlInt32  stlStrcasecmp(const stlChar *aStr1, const stlChar *aStr2);
stlInt32  stlStrncasecmp(const stlChar *aStr1, const stlChar *aStr2, stlSize aLen);
stlChar  *stlStrstr(const stlChar *aHaystack, const stlChar *aNeedle);

stlChar  *stlStrtok(stlChar *aStr, const stlChar *aDelimeter, stlChar **aSavePtr);

stlChar  *stlStrcat(stlChar *aDest, const stlChar *aSrc);
stlChar  *stlStrncat(stlChar *aDest, const stlChar *aSrc, stlSize aLen);
stlChar  *stlStrnncat( stlChar * aDest, const stlChar * aSrc, stlSize aDestLen, stlSize aSrcLen );

stlInt32  stlPrintf(const stlChar *aFormat, ...);
stlInt32  stlPaintf(stlPaintAttr * aAttr, const stlChar *aFormat, ...);
stlInt32  stlSnprintf(stlChar       *aString,
                      stlSize        aSize,
                      const stlChar *aFormat,
                      ...);
stlInt32  stlVprintf(const stlChar *aFormat, va_list aVarArgList);
stlInt32  stlVpaintf( stlPaintAttr  * aAttr, const stlChar * aFormat, va_list aVarArgList );
stlInt32  stlVsnprintf(stlChar       *aString,
                       stlSize        aSize,
                       const stlChar *aFormat,
                       va_list        aVarArgList);

stlInt32  stlGetPrintfBufferSize(const stlChar *aFormat,
                                 ...);
stlInt32  stlGetVprintfBufferSize(const stlChar *aFormat,
                                  va_list        aVarArgList);

stlStatus stlStrToFloat64(const stlChar  *aNumPtr,
                          stlInt64        aLength,
                          stlChar       **aEndPtr,
                          stlFloat64     *aResult,
                          stlErrorStack  *aErrorStack);

stlStatus stlStrToFloat32(const stlChar  *aNumPtr,
                          stlInt64        aLength,
                          stlChar       **aEndPtr,
                          stlFloat32     *aResult,
                          stlErrorStack  *aErrorStack);

stlStatus stlStrToInt64(const stlChar  *aNumPtr,
                        stlInt64        aLength,
                        stlChar       **aEndPtr,
                        stlInt32        aBase,
                        stlInt64       *aResult,
                        stlErrorStack  *aErrorStack);

stlStatus stlStrToUInt64(const stlChar  *aNumPtr,
                         stlInt64        aLength,
                         stlChar       **aEndPtr,
                         stlInt32        aBase,
                         stlUInt64      *aResult,
                         stlErrorStack  *aErrorStack);

stlStatus stlBinary2HexString( stlUInt8      * aBinary,
                               stlInt32        aBinarySize,
                               stlChar       * aHexString,
                               stlInt32        aHexStringSize,
                               stlErrorStack * aErrorStack );

stlStatus stlBinary2CharString( stlUInt8      * aBinary,
                                stlInt32        aBinarySize,
                                stlChar       * aCharString,
                                stlInt32        aCharStringSize,
                                stlErrorStack * aErrorStack );

stlSize stlStrspn( const stlChar * aString,
                   const stlChar * aAccept );

void stlToupperString( stlChar * aSrcString,
                       stlChar * aDestString );

void stlTolowerString( stlChar * aSrcString,
                       stlChar * aDestString );

void stlTrimString( stlChar * aString );

stlChar * stlGetDefaultCharacterSet();

stlStatus stlGetPassword( const stlChar * aPrompt,
                          stlBool         aEcho,
                          stlChar       * aBuffer,
                          stlSize         aBufferLength,
                          stlErrorStack * aErrorStack );

/** @} */
    
STL_END_DECLS

#endif /* _STLSTRINGS_H_ */
