/*******************************************************************************
 * stlStrings.c
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
 * @file stlStrings.c
 * @brief Standard Layer Strings Routines
 */

/**
 * @addtogroup stlStrings
 * @{
 */

#include <stc.h>
#include <stlDef.h>
#include <stlStrings.h>
#include <str.h>

/**
 * @brief 주어진 문자열의 길이를 반환한다.
 * @param[in] aString 길이를 얻어올 문자열
 * @return 문자열의 길이
 */
stlSize stlStrlen(const stlChar *aString)
{
    return strlen(aString);
}

/**
 * @brief aSrc 문자열을 aDest 문자열에 복사한다.
 * @param[out] aDest 복사될 문자열 버퍼의 위치
 * @param[in] aSrc 복사할 문자열 버퍼의 시작 위치
 * @return 복사된 aDest 문자열의 Pointer
 */
stlChar * stlStrcpy(stlChar *aDest, const stlChar *aSrc)
{
    return strcpy(aDest, aSrc);
}

/**
 * @brief aSrc 문자열을 aDest 문자열에 aLen 길이만큼 복사한다.
 * @param[out] aDest 복사될 문자열 버퍼의 위치
 * @param[in] aSrc 복사할 문자열 버퍼의 시작 위치
 * @param[in] aLen 복사될 문자열의 길이.
 *        @a aDest는 항상 NULL 종료가 된 문자열이다.
 *        만약 @a aSrc 의 길이가 @a aLen 보다 클 경우에 실제 복사되는 문자의 길이는
 *        @a aLen - 1 만큼이다.
 * @return NULL 종료가 된 복사된 aDest 문자열의 Pointer
 * @remark strncpy()와 stlStrncpy()의 차이점.
 * <PRE>
 *   (1) strncpy()는 NULL 종료를 보장하지 않지만 stlStrncpy()는 NULL 종료를 보장한다.
 *   (2) strncpy()는 복사될 문자열에 NULL 패딩을 하지만 stlStrncpy()는 불필요한 NULL 패딩을 하지 않는다.
 * </PRE>
 */
stlChar * stlStrncpy(stlChar *aDest, const stlChar *aSrc, stlSize aLen)
{
    stlChar *sDest;
    stlChar *sEnd;

    sDest = aDest;
    
    STL_TRY_THROW( aLen != 0, EXIT_FUNCTION );

    sEnd = aDest + aLen - 1;

    for( ; sDest < sEnd; sDest++, ++aSrc )
    {
        *sDest = *aSrc;

        STL_TRY_THROW( *sDest != 0, EXIT_FUNCTION );
    }

    *sDest = '\0';

    STL_RAMP(EXIT_FUNCTION);

    return aDest;

}

/**
 * @brief aStr 문자열에서 주어진 문자 aChar가 처음 등장한 위치를 반환한다.
 * @param[in] aStr 문자열
 * @param[in] aChar 찾을 문자
 * @return 찾은 문자가 위치한 Pointer/문자를 찾지 못한 경우에는 NULL을 반환
 */
stlChar * stlStrchr(const stlChar *aStr, stlChar aChar)
{
    return strchr(aStr, (int)aChar);
}

/**
 * @brief aStr 문자열에서 주어진 문자 aChar가 마지막으로 등장한 위치를 반환한다.
 * @param[in] aStr 문자열
 * @param[in] aChar 찾을 문자
 * @return 찾은 문자가 위치한 Pointer/문자를 찾지 못한 경우에는 NULL을 반환
 */
stlChar * stlStrrchr(const stlChar *aStr, stlChar aChar)
{
    return strrchr(aStr, (int)aChar);
}

/**
 * @brief 2개의 문자열 aStr1과 aStr2를 비교한다.
 * @param[in] aStr1 비교 문자열 1
 * @param[in] aStr2 비교 문자열 2
 * @return aStr1이 aStr2보다 작으면 음수, 크면 양수, 같으면 0을 반환한다.
 */
stlInt32 stlStrcmp(const stlChar *aStr1, const stlChar *aStr2)
{
    return strcmp(aStr1, aStr2);
}

/**
 * @brief 2개의 문자열 aStr1과 aStr2를 길이 aLen까지 비교한다.
 * @param[in] aStr1 비교 문자열 1
 * @param[in] aStr2 비교 문자열 2
 * @param[in] aLen 비교할 문자열 길이
 * @return aStr1이 aStr2보다 작으면 음수, 크면 양수, 같으면 0을 반환한다.
 */
stlInt32 stlStrncmp(const stlChar *aStr1, const stlChar *aStr2, stlSize aLen)
{
    return strncmp(aStr1, aStr2, aLen);
}

/**
 * @brief 2개의 문자열 aStr1과 aStr2를 case를 무시하고 비교한다.
 * @param[in] aStr1 비교 문자열 1
 * @param[in] aStr2 비교 문자열 2
 * @return aStr1이 aStr2보다 작으면 음수, 크면 양수, 같으면 0을 반환한다.
 */
stlInt32 stlStrcasecmp(const stlChar *aStr1, const stlChar *aStr2)
{
#if STC_HAVE_STRCASECMP
    return strcasecmp(aStr1, aStr2);
#elif STC_HAVE_STRICMP
    return stricmp(aStr1, aStr2);
#else
    stlInt32         sDiff = 0;
    const stlChar   *sStr1 = aStr1;
    const stlChar   *sStr2 = aStr2;

    for( ; (*sStr1 != '\0') && (*sStr2 != '\0'); sStr1++, sStr2++ )
    {
        sDiff = stlTolower(*sStr1) - stlTolower(*sStr2);
        STL_TRY_THROW( sDiff == 0, EXIT_FUNCTION );
    }

    if( *sStr1 != '\0' )
    {
        sDiff = 1;               /* p was longer than q */
    }
    else if( *sStr2 != '\0' )
    {
        sDiff =  -1;             /* p was shorter than q */
    }
    else
    {
        sDiff = 0;               /* Exact match */
    }

    STL_RAMP(EXIT_FUNCTION);

    return sDiff;
#endif
}

/**
 * @brief 2개의 문자열 aStr1과 aStr2를 길이 aLen까지 case를 무시하고 비교한다.
 * @param[in] aStr1 비교 문자열 1
 * @param[in] aStr2 비교 문자열 2
 * @param[in] aLen 비교할 문자열 길이
 * @return aStr1이 aStr2보다 작으면 음수, 크면 양수, 같으면 0을 반환한다.
 */
stlInt32 stlStrncasecmp(const stlChar *aStr1, const stlChar *aStr2, stlSize aLen)
{
#if STC_HAVE_STRNCASECMP
    return strncasecmp(aStr1, aStr2, aLen);
#elif STC_HAVE_STRNICMP
    return strnicmp(aStr1, aStr2, aLen);
#else
    stlInt32         sDiff = 0;
    const stlChar   *sStr1 = aStr1;
    const stlChar   *sStr2 = aStr2;

    for( ; /* NOTHING */; sStr1++, sStr2++)
    {
        if(sStr1 == aStr1 + aLen)
        {
            sDiff = 0;           /*   Match up to n characters */
            STL_THROW( EXIT_FUNCTION );
        }
        
        if((*sStr1 == '\0') || (*sStr2 == '\0'))
        {
            sDiff = *sStr1 - *sStr2;
            STL_THROW( EXIT_FUNCTION );
        }
            
        sDiff = stlTolower(*sStr1) - stlTolower(*sStr2);
        STL_TRY_THROW( sDiff == 0, EXIT_FUNCTION );
    }

    STL_RAMP(EXIT_FUNCTION);

    return( sDiff );
#endif
}

/**
 * @brief 문자열 aHaystack에서 부분 문자열 aNeedle을 찾아서 그 위치를 반환한다.
 * @param[in] aHaystack 부분 문자열을 검색할 전체 문자열
 * @param[in] aNeedle 찾고자 하는 부분 문자열
 * @return aHaystack에서 aNeedle 문자열을 찾은 시작 위치/
 *         부분 문자열을 찾지 못한 경우에는 NULL을 반환
 */
stlChar * stlStrstr(const stlChar *aHaystack, const stlChar *aNeedle)
{
#if STC_HAVE_STRSTR
    return strstr(aHaystack, aNeedle);
#else
    stlChar  *sHaystack = (stlChar *)aHaystack;
    stlChar  *sNeedle = (stlChar *)aNeedle;
    stlChar  *sP1;
    stlChar  *sP2;

    if (*sNeedle == '\0')
    {
        /* an empty sNeedle */
        STL_THROW(EXIT_FUNCTION);
    }

    sHaystack = stlStrchr(sHaystack, *sNeedle);
    while(sHaystack != NULL)
    {
        /* found first character of sNeedle, see if the rest matches */
        sP1 = sHaystack;
        sP2 = sNeedle;

        ++sP1;
        ++sP2;
        while( *sP1 == *sP2 )
        {
            if( *sP1 == '\0' )
            {
                /* both strings ended together */
                STL_THROW(EXIT_FUNCTION);
            }
            ++sP1;
            ++sP2;
        }
        if( *sP2 == '\0' )
        {
            /* second string ended, a match */
            break;
        }
        /* didn't find a match here, try starting at next character in sHaystack */
        sHaystack++;
        sHaystack = stlStrchr(sHaystack, *sNeedle);
    }

    STL_RAMP(EXIT_FUNCTION);
    return(sHaystack);
#endif
}

/**
 * @brief 문자열 aStr을 Token의 조합으로 parsing한다.
 * Token은 aDelimeter에 있는 문자를 포함하지 않은 하나이상의 문자로 된 문자열을 의미한다.
 * 이 함수는 주어진 문자열 aStr을 aDelimeter를 구분자로 하여 토큰을 뽑아낸다.
 * 원본 문자열에서 구분자는 NUL 문자로 겹쳐 쓰여지고, 이에 대한 포인터가 다음 이 함수의 호출을 위해서 저장된다. 이 함수는 stlStrtok()와 동일하게 작동하지만, 정적버퍼를 사용하지 않고 주어진 aSavePtr 포인터를 사용한다. 
 * @param[in] aStr Token으로 분리할 원본 문자열
 * @param[in] aDelimeter Token으로 쪼갤 구분자
 * @param[in,out] aSavePtr stlStrtok_r()에서 Parsing을 진행할때 Token의 위치를 저장. 같은 문자열에 대해서 반복적으로 Parsing을 수행할 때에 같은 값이 주어져야 하며, aStr이 NULL로 주어진 경우 Parsing을 시작할 Pointer
 * @return Token을 가리키는 문자열에 대한 Pointer/<BR>더 이상 가져올 수 있는 Token이 없으면 NULL
 * @remark <BR>
 *   1. 이 함수는 주어진 aStr 인자를 수정한다.<BR>
 *   2. 원본에 NUL 문자를 겹쳐쓰기 때문에, aStr에 상수 문자열에 사용해서는 안된다.<BR>
 */
stlChar * stlStrtok(stlChar *aStr, const stlChar *aDelimeter, stlChar **aSavePtr)
{
    stlChar   *sToken;

    if(aStr == NULL)           /* subsequent call */
    {
        aStr = *aSavePtr;      /* start where we left off */
    }
    else
    {
        /* Do Nothing */
    }

    /* skip characters in aDelimeter (will terminate at \0) */
    while((*aStr != '\0') && (stlStrchr(aDelimeter, *aStr) != NULL))
    {
        ++aStr;
    }

    if(*aStr == '\0')          /* no more tokens */
    {
        sToken = NULL;
        STL_THROW(EXIT_FUNCTION);
    }

    sToken = aStr;

    /* skip valid sToken characters to terminate sToken and
     * prepare for the next call (will terminate at \0) 
     */
    *aSavePtr = sToken + 1;
    while((**aSavePtr != '\0') && (stlStrchr(aDelimeter, **aSavePtr) == NULL) )
    {
        ++*aSavePtr;
    }

    if(**aSavePtr != '\0')
    {
        **aSavePtr = '\0';
        ++*aSavePtr;
    }

    STL_RAMP(EXIT_FUNCTION);

    return sToken;
}

/**
 * @brief 문자열 aSrc를 aDest 뒷부분에 덧붙여서, 하나의 문자열을 만든다.
 * @param[out] aDest 합쳐질 문자열
 * @param[in] aSrc 합칠 문자열
 * @return 하나로 합쳐진 aDest 문자열의 Pointer
 * @remark stlStrcat의 결과인 aDest 문자열은 항상 NUL 문자로 종료한다.
 */
stlChar * stlStrcat(stlChar *aDest, const stlChar *aSrc)
{
    return strcat(aDest, aSrc);
}

/**
 * @brief 문자열 aSrc의 aLen 길이만큼을 aDest 뒷부분에 덧붙여서, 하나의 문자열을 만든다.
 *        만약 aSrc의 길이가 aLen 보다 작은 경우에는, aSrc 전체가 aDest로 결합되지만,
 *        aSrc의 길이가 aLen 보다 큰 경우에는, aSrc 중에서 aLen - 1 길이만큼의 문자열만
 *        aDest에 결합되고, 마지막은 NUL 문자로 종료한다.
 * @param[out] aDest 합쳐질 문자열
 * @param[in]  aSrc  합칠 문자열
 * @param[in]  aLen  합칠 aSrc의 길이
 * @return 하나로 합쳐진 aDest 문자열의 Pointer
 * @remark stlStrncat의 결과인 aDest 문자열은 항상 NUL 문자로 종료한다.
 */
stlChar * stlStrncat(stlChar *aDest, const stlChar *aSrc, stlSize aLen)
{
    stlInt32   sDestLen = stlStrlen(aDest);
    stlInt32   i;

    for(i = 0; (i < (aLen - 1)) && (aSrc[i] != '\0') ; i++)
    {
        aDest[sDestLen+i] = aSrc[i];
    }

    aDest[sDestLen+i] = '\0';

    return aDest;
}

/**
 * @brief 문자열 aSrc의 aLen 길이만큼을 aDest 뒷부분에 덧붙여서, 하나의 문자열을 만든다.
 *        만약 aSrc의 길이가 aLen 보다 작은 경우에는, aSrc 전체가 aDest로 결합되지만,
 *        aSrc의 길이가 aLen 보다 큰 경우에는, aSrc 중에서 aLen - 1 길이만큼의 문자열만
 *        aDest에 결합되고, 마지막은 NUL 문자로 종료한다.
 * @param[out] aDest    합쳐질 문자열
 * @param[in]  aSrc     합칠 문자열
 * @param[in]  aDestLen aDest 공간의 최대 크기
 * @param[in]  aSrcLen  합칠 aSrc의 길이
 * @return 하나로 합쳐진 aDest 문자열의 Pointer
 * @remark stlStrnncat의 결과인 aDest 문자열은 항상 NUL 문자로 종료한다.
 */
stlChar * stlStrnncat( stlChar       * aDest,
                       const stlChar * aSrc,
                       stlSize         aDestLen,
                       stlSize         aSrcLen )
{
    stlInt32   sDestLen = stlStrlen( aDest );
    stlInt32   i;

    for(i = 0; (i < (aSrcLen - 1)) && (aSrc[i] != '\0') && (i < (aDestLen - sDestLen - 1)); i++)
    {
        aDest[sDestLen+i] = aSrc[i];
    }

    aDest[sDestLen + i] = '\0';

    return aDest;
}

/**
 * @brief Formatting된 문자열을 stdout으로 출력한다.
 * @param[in] aFormat 출력할 문자열의 format
 * @param[in] ... 출력 인자
 * @see @a aFormat : stlStringFormat
 * @return 출력한 문자열의 길이
 */
stlInt32 stlPrintf(const stlChar *aFormat, ...)
{
    stlInt32  sReturn;
    va_list   sVarArgList;

    va_start(sVarArgList, aFormat);
    sReturn = strVprintf( aFormat, sVarArgList );
    va_end(sVarArgList);

    return sReturn;
}

/**
 * @brief 주어진 Color 속성으로 Formatting된 문자열을 stdout으로 출력한다.
 * @param[in] aAttr   문자열 Color 속성
 * @param[in] aFormat 출력할 문자열의 format
 * @param[in] ... 출력 인자
 * @see @a aFormat : stlStringFormat
 * @return 출력한 문자열의 길이
 */
stlInt32 stlPaintf( stlPaintAttr * aAttr, const stlChar *aFormat, ... )
{
    stlInt32  sReturn;
    va_list   sVarArgList;

    va_start(sVarArgList, aFormat);
    sReturn = stlVpaintf( aAttr, aFormat, sVarArgList );
    va_end(sVarArgList);

    return sReturn;
}

/**
 * @brief Formatting된 문자열을 aString으로 출력한다.
 * @param[out] aString 출력할 문자열 버퍼
 * @param[in] aSize 출력할 문자열의 길이
 * @param[in] aFormat 출력할 문자열의 format
 * @param[in] ... 출력 인자
 * @see @a aFormat : stlStringFormat
 * @return 출력한 문자열의 길이
 */
stlInt32 stlSnprintf(stlChar       *aString,
                     stlSize        aSize,
                     const stlChar *aFormat,
                     ...)
{
    stlInt32  sReturn;
    va_list   sVarArgList;

    STL_TRY(aSize > 0);

    va_start(sVarArgList, aFormat);
    sReturn = strVsnprintf(aString, aSize, aFormat, sVarArgList);
    va_end(sVarArgList);

    return sReturn;

    STL_FINISH;

    return aSize;
}

/**
 * @brief Formatting된 문자열을 stdout으로 출력한다.
 * @param[in] aFormat 출력할 문자열의 format
 * @param[in] aVarArgList 출력할 가변 인자 List
 * @see @a aFormat : stlStringFormat
 * @return 출력한 문자열의 길이
 */
stlInt32 stlVprintf(const stlChar *aFormat, va_list aVarArgList)
{
    return strVprintf( aFormat, aVarArgList );
}

/**
 * @brief 주어진 Color 속성으로 Formatting된 문자열을 stdout으로 출력한다.
 * @param[in] aAttr   문자열 Color 속성
 * @param[in] aFormat 출력할 문자열의 format
 * @param[in] aVarArgList 출력할 가변 인자 List
 * @see @a aFormat : stlStringFormat
 * @return 출력한 문자열의 길이
 */
stlInt32 stlVpaintf( stlPaintAttr  * aAttr,
                     const stlChar * aFormat,
                     va_list         aVarArgList )
{
    stlInt32  sReturn;
    
    if( aAttr->mTextAttr != STL_PTA_OFF )
    {
        stlPrintf( "\x1b[%dm", aAttr->mTextAttr );
    }
    
    if( aAttr->mForeColor != STL_PFC_OFF )
    {
        switch( aAttr->mForeColor )
        {
            case STL_PFC_BLACK:
                stlPrintf( "\x1b[22;30m" );
                break;
            case STL_PFC_GRAY:
                stlPrintf( "\x1b[01;30m" );
                break;
            case STL_PFC_RED:
                stlPrintf( "\x1b[22;31m" );
                break;
            case STL_PFC_LIGHT_RED:
                stlPrintf( "\x1b[01;31m" );
                break;
            case STL_PFC_GREEN:
                stlPrintf( "\x1b[22;32m" );
                break;
            case STL_PFC_LIGHT_GREEN:
                stlPrintf( "\x1b[01;32m" );
                break;
            case STL_PFC_BROWN:
                stlPrintf( "\x1b[22;33m" );
                break;
            case STL_PFC_YELLOW:
                stlPrintf( "\x1b[01;33m" );
                break;
            case STL_PFC_BLUE:
                stlPrintf( "\x1b[22;34m" );
                break;
            case STL_PFC_LIGHT_BLUE:
                stlPrintf( "\x1b[01;34m" );
                break;
            case STL_PFC_MAGENTA:
                stlPrintf( "\x1b[22;35m" );
                break;
            case STL_PFC_LIGHT_MAGENTA:
                stlPrintf( "\x1b[01;35m" );
                break;
            case STL_PFC_CYAN:
                stlPrintf( "\x1b[22;36m" );
                break;
            case STL_PFC_LIGHT_CYAN:
                stlPrintf( "\x1b[22;36m" );
                break;
            case STL_PFC_WHITE:
                stlPrintf( "\x1b[37m" );
                break;
            default:
                break;
        }
    }
    
    if( aAttr->mBackColor != STL_PBC_OFF )
    {
        stlPrintf( "\x1b[%dm", aAttr->mBackColor );
    }
    
    sReturn = strVprintf( aFormat, aVarArgList );
    
    if( (aAttr->mTextAttr  != STL_PTA_OFF) ||
        (aAttr->mForeColor != STL_PFC_OFF) ||
        (aAttr->mBackColor != STL_PBC_OFF) )
    {
        stlPrintf( "\x1b[0m" );
    }
    
    return sReturn;
}

/**
 * @brief Formatting된 문자열을 aString으로 출력한다.
 * @param[out] aString 출력할 문자열 버퍼
 * @param[in] aSize 출력할 문자열의 길이
 * @param[in] aFormat 출력할 문자열의 format
 * @param[in] aVarArgList 출력할 가변 인자 List
 * @see @a aFormat : stlStringFormat
 * @return 출력한 문자열의 길이
 */
stlInt32 stlVsnprintf(stlChar       *aString,
                      stlSize        aSize,
                      const stlChar *aFormat,
                      va_list        aVarArgList)
{
    STL_TRY(aSize > 0);

    return strVsnprintf(aString, aSize, aFormat, aVarArgList);

    STL_FINISH;

    return aSize;
}

/**
 * @brief Formatting된 문자열을 만들었을때 그 출력 문자열의 길이를 반환한다.
 * @param[in] aFormat 출력할 문자열의 format
 * @param[in] ... 출력 인자
 * @see @a aFormat : stlStringFormat
 * @return Formatting된 문자열의 문자열의 길이
 */
stlInt32 stlGetPrintfBufferSize(const stlChar *aFormat,
                                ...)
{
    stlInt32  sReturn;
    va_list   sVarArgList;

    va_start(sVarArgList, aFormat);
    sReturn = strVsnprintf(NULL, 0, aFormat, sVarArgList);
    va_end(sVarArgList);

    return sReturn;
}

/**
 * @brief Formatting된 문자열을 만들었을때 그 출력 문자열의 길이를 반환한다.
 * @param[in] aFormat 출력할 문자열의 format
 * @param[in] aVarArgList 출력할 가변 인자 List
 * @see @a aFormat : stlStringFormat
 * @return Formatting된 문자열의 문자열의 길이
 */
stlInt32 stlGetVprintfBufferSize(const stlChar *aFormat,
                                 va_list        aVarArgList)
{
    return strVsnprintf(NULL, 0, aFormat, aVarArgList);
}

/**
 * @brief 인자로 받은 문자열 aNumPtr을 해석할 수 있는 곳 까지 해석해서 stlFloat64형으로 표현한 값을 리턴한다. 
 * @param[in] aNumPtr 부동소수점 값으로 변환할 문자열의 포인터
 * @param[in] aLength aNumPtr의 길이. 만약 @a aNumPtr 가 널 문자로 종료되어있으면 음수값을 입력한다.
 * @param[out] aEndPtr aNumPtr에서 해석 가능한 마지막 문자에 대한 포인터. 이 값을 참조하여 어디까지 해석했는지에 대한 정보를 파악할 수 있다.
 * @param[out] aResult 변환된 stlFloat64형 값을 저장
 * @param[out] aErrorStack 에러 스택
 * @return 성공시 STL_SUCCESS/<BR>실패시 STL_FAILURE
 * @code
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     변환 과정을 수행하지 못할 때
 * [STL_ERRCODE_OUT_OF_RANGE]
 *     변환하고자 하는값이 Float64의 표현범위를 넘거나(overflow) 못미치는(underflow) 경우
 * [STL_ERRCODE_STRING_IS_NOT_NUMBER]
 *     변환하고자 하는 문자열이 숫자가 아닐때
 * @endcode
 */
stlStatus stlStrToFloat64(const stlChar  *aNumPtr,
                          stlInt64        aLength,
                          stlChar       **aEndPtr,
                          stlFloat64     *aResult,
                          stlErrorStack  *aErrorStack)
{
    return strStrToFloat64(aNumPtr, aLength, aEndPtr, aResult, aErrorStack);
}

/**
 * @brief 인자로 받은 문자열 aNumPtr을 해석할 수 있는 곳 까지 해석해서 stlFloat32형으로 표현한 값을 리턴한다. 
 * @param[in] aNumPtr 부동소수점 값으로 변환할 문자열의 포인터
 * @param[in] aLength aNumPtr의 길이. 만약 @a aNumPtr 가 널 문자로 종료되어있으면 음수값을 입력한다.
 * @param[out] aEndPtr aNumPtr에서 해석 가능한 마지막 문자에 대한 포인터. 이 값을 참조하여 어디까지 해석했는지에 대한 정보를 파악할 수 있다.
 * @param[out] aResult 변환된 stlFloat32형 값을 저장
 * @param[out] aErrorStack 에러 스택
 * @return 성공시 STL_SUCCESS/<BR>실패시 STL_FAILURE
 * @code
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     변환 과정을 수행하지 못할 때
 * [STL_ERRCODE_OUT_OF_RANGE]
 *     변환하고자 하는값이 Float64의 표현범위를 넘거나(overflow) 못미치는(underflow) 경우
 * [STL_ERRCODE_STRING_IS_NOT_NUMBER]
 *     변환하고자 하는 문자열이 숫자가 아닐때
 * @endcode
 */
stlStatus stlStrToFloat32(const stlChar  *aNumPtr,
                          stlInt64        aLength,
                          stlChar       **aEndPtr,
                          stlFloat32     *aResult,
                          stlErrorStack  *aErrorStack)
{
    return strStrToFloat32(aNumPtr, aLength, aEndPtr, aResult, aErrorStack);
}

/**
 * @brief 인자로 받은 문자열 aNumPtr을 해석할 수 있는 곳 까지 해석해서 stlInt64형으로 표현한 값을 리턴한다. 
 * @param[in] aNumPtr 정수 값으로 변환할 문자열의 포인터
 * @param[in] aLength aNumPtr의 길이. 만약 @a aNumPtr 가 널 문자로 종료되어있으면 음수값을 입력한다.
 * @param[out] aEndPtr aNumPtr에서 어디까지를 변환했는지에 대한 포인터. 이 값을 참조하여 어디까지 해석했는지에 대한 정보를 파악할 수 있다.
 * @param[in] aBase 문자열의 숫자를 몇진수로 인식할 것인지에 대한 표현. 8, 10, 16이 쓰일 수 있으며, 0이 주어질 경우에는 문자열을 해석하여 0x 로 시작하면 16, 0 으로 시작하면 8, 그 외에는 10으로 간주한다.
 * @param[out] aResult 변환된 stlInt64형 값을 저장
 * @param[out] aErrorStack 에러 스택
 * @return 성공시 STL_SUCCESS/<BR>실패시 STL_FAILURE
 * @code
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     주어진 Base가 허용되는 Base가 아닐때
 * [STL_ERRCODE_OUT_OF_RANGE]
 *     변환하고자 하는값이 Int64의 표현범위를 넘는(overflow) 경우
 * [STL_ERRCODE_STRING_IS_NOT_NUMBER]
 *     변환하고자 하는 문자열이 숫자가 아닐때
 * @endcode
 */
stlStatus stlStrToInt64(const stlChar  *aNumPtr,
                        stlInt64        aLength,
                        stlChar       **aEndPtr,
                        stlInt32        aBase,
                        stlInt64       *aResult,
                        stlErrorStack  *aErrorStack)
{
    return strStrToInt64(aNumPtr, aLength, aEndPtr, aBase, aResult, aErrorStack);
}

/**
 * @brief 인자로 받은 문자열 aNumPtr을 해석할 수 있는 곳 까지 해석해서 stlUInt64형으로 표현한 값을 리턴한다. 
 * @param[in] aNumPtr 정수 값으로 변환할 문자열의 포인터
 * @param[in] aLength aNumPtr의 길이. 만약 @a aNumPtr 가 널 문자로 종료되어있으면 음수값을 입력한다.
 * @param[out] aEndPtr aNumPtr에서 어디까지를 변환했는지에 대한 포인터. 이 값을 참조하여 어디까지 해석했는지에 대한 정보를 파악할 수 있다.
 * @param[in] aBase 문자열의 숫자를 몇진수로 인식할 것인지에 대한 표현. 8, 10, 16이 쓰일 수 있으며, 0이 주어질 경우에는 문자열을 해석하여 0x 로 시작하면 16, 0 으로 시작하면 8, 그 외에는 10으로 간주한다.
 * @param[out] aResult 변환된 stlInt64형 값을 저장
 * @param[out] aErrorStack 에러 스택
 * @return 성공시 STL_SUCCESS/<BR>실패시 STL_FAILURE
 * @code
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     주어진 Base가 허용되는 Base가 아닐때
 * [STL_ERRCODE_OUT_OF_RANGE]
 *     변환하고자 하는값이 Int64의 표현범위를 넘는(overflow) 경우
 * [STL_ERRCODE_STRING_IS_NOT_NUMBER]
 *     변환하고자 하는 문자열이 숫자가 아닐때
 * @endcode
 */
stlStatus stlStrToUInt64(const stlChar  *aNumPtr,
                         stlInt64        aLength,
                         stlChar       **aEndPtr,
                         stlInt32        aBase,
                         stlUInt64       *aResult,
                         stlErrorStack  *aErrorStack)
{
    return strStrToUInt64(aNumPtr, aLength, aEndPtr, aBase, aResult, aErrorStack);
}

/**
 * @brief 이진 형태의 aBinary를 Hexa String으로 변환한다.
 * @param[in] aBinary 대상 Binary
 * @param[in] aBinarySize 대상 Binary의 바이트단위 크기
 * @param[out] aHexString 변환된 Hexa 형태의 문자열 버퍼
 * @param[in] aHexStringSize 변환될 aHexSting의 버퍼 크기
 * @param[out] aErrorStack 에러 스택
 * @return 성공시 STL_SUCCESS/<BR>실패시 STL_FAILURE
 * @note aBinary를 aHexString으로 변환시 aHexStringSize를 초과하는 경우는
 * <BR> aHexString이 truncate될수 있다.
 * <BR> aHexString은 항상 마지막은 Null 문자로 채워진다.
 */
stlStatus stlBinary2HexString( stlUInt8      * aBinary,
                               stlInt32        aBinarySize,
                               stlChar       * aHexString,
                               stlInt32        aHexStringSize,
                               stlErrorStack * aErrorStack )
{
    stlUInt8 sHexCode[] = "0123456789ABCDEF";
    stlInt32 i;

    stlMemset( aHexString, 0x00, aHexStringSize );
    
    for( i = 0; i < aBinarySize; i++ )
    {
        if( (aHexStringSize-1) > (i*2) )
        {
            aHexString[(i*2)] = sHexCode[aBinary[i] >> 4];
        }
        else
        {
            aHexString[i*2] = 0;
            break;
        }

        if( (aHexStringSize-1) > (i*2)+1 )
        {
            aHexString[(i*2)+1] = sHexCode[aBinary[i] & 0xF];
        }
        else
        {
            aHexString[(i*2)+1] = 0;
            break;
        }
    }
    
    return STL_SUCCESS;
}

/**
 * @brief 이진 형태의 aBinary를 Character String으로 변환한다.
 * @param[in] aBinary 대상 Binary
 * @param[in] aBinarySize 대상 Binary의 바이트단위 크기
 * @param[out] aCharString 변환된 Character 문자열 버퍼
 * @param[in] aCharStringSize 변환될 aCharSting의 버퍼 크기
 * @param[out] aErrorStack 에러 스택
 * @return 성공시 STL_SUCCESS/<BR>실패시 STL_FAILURE
 * @note aBinary를 aCharString으로 변환시 aCharStringSize를 초과하는 경우는
 * <BR> aCharString이 truncate될수 있다.
 * <BR> aCharString은 항상 마지막은 Null 문자로 채워진다.
 * <BR> Binary값을 Character로 변환시키지 못할 경우에는 '.'으로 출력된다.
 */
stlStatus stlBinary2CharString( stlUInt8      * aBinary,
                                stlInt32        aBinarySize,
                                stlChar       * aCharString,
                                stlInt32        aCharStringSize,
                                stlErrorStack * aErrorStack )
{
    stlUInt8 sChar;
    stlUInt8 sHex;
    stlInt32 i;

    stlMemset( aCharString, 0x00, aCharStringSize );
    
    for( i = 0; i < aBinarySize; i++ )
    {
        sHex = aBinary[i];

        /**
         * hpux에서 버그로 인하여 Hexa code의 범위를 검사해야 한다.
         */
        
        if( (stlIsprint( sHex ) == STL_TRUE) && (sHex <= STL_INT8_MAX) )
        {
            sChar = sHex;
        }
        else
        {
            sChar = '.';
        }
        
        if( (aCharStringSize-1) > i )
        {
            aCharString[i] = sChar;
        }
        else
        {
            aCharString[i] = 0;
            break;
        }
    }

    aCharString[aCharStringSize-1] = 0;
    
    return STL_SUCCESS;
}

/**
 * @brief 문자열에서 지정된 문자들로 구성된 초기 문자열의 길이를 반환.
 * @param[in] aString 검색 대상 문자열
 * @param[in] aAccept 검색에 사용되는 문자들의 모임
 * @return 지정된 문자들(aAccept)로 구성된 초기문자열의 길이 반환.
 *    <BR> 예) aString = '321ab', aAccept = '12346', return = 3 (321ab 에서 321)
 *    <BR> 예) aString = 'ab321', aAccept = '12346', return = 0 
 */
stlSize stlStrspn( const stlChar * aString,
                   const stlChar * aAccept )
{
    return strspn( aString,
                   aAccept );
}

/**
 * @brief 주어진 문자열을 대문자 문자열로 변환한다.
 * @param[in]  aSrcString  대상 문자열
 * @param[out] aDestString 변환된 문자열
 */
void stlToupperString( stlChar * aSrcString,
                       stlChar * aDestString )
{
    stlInt32 i = 0;

    while( aSrcString[i] != 0 )
    {
        aDestString[i] = stlToupper( aSrcString[i] );
        i++;
    }

    aDestString[i] = '\0';
}

/**
 * @brief 주어진 문자열을 소문자 문자열로 변환한다.
 * @param[in]  aSrcString  대상 문자열
 * @param[out] aDestString 변환된 문자열
 */
void stlTolowerString( stlChar * aSrcString,
                       stlChar * aDestString )
{
    stlInt32 i = 0;

    while( aSrcString[i] != 0 )
    {
        aDestString[i] = stlTolower( aSrcString[i] );
        i++;
    }

    aDestString[i] = '\0';
}

/**
 * @brief 주어진 문자열의 선행 공백과 후행 공백을 제거한다.
 * @param[in,out]  aString  대상 문자열
 */
void stlTrimString( stlChar * aString )
{
    stlChar  * sString;
    stlInt32   sLength;
    
    sString = aString;
    sLength = stlStrlen( aString );

    if( sLength > 0 )
    {
        while( stlIsspace( sString[sLength - 1] ) == STL_TRUE )
        {
            sLength--;
            sString[sLength] = '\0';

            STL_TRY_THROW( sLength > 0 , RAMP_FINISH );
        }

        while( (*sString) && (stlIsspace( *sString ) == STL_TRUE) )
        {
            sString++;
            sLength--;
        }

        stlMemmove( aString, sString, sLength + 1 );
    }
    else
    {
        /* do nothing */
    }

    STL_RAMP( RAMP_FINISH );
}

/**
 * @brief OS의 기본 CharacterSet을 반환한다.
 * @return UNIX의 경우 locale -m 명령어를 통해 반환 가능한 문자열을 알 수 있다.<BR>
 *         WINDOWS의 경우 CPXXXX 형식의 코드페이지를 반환한다.
 */
stlChar * stlGetDefaultCharacterSet()
{
    return strGetDefaultCharacterSet();
}

/**
 * @brief 패스워드를 입력 받는다.
 * @param[in]  aPrompt       패스워드 입력 시 프롬프트
 * @param[in]  aEcho         패스워드 입력 시 * 문자 출력 여부
 * @param[out] aBuffer       입력 받은 패스워드가 저장될 버퍼
 * @param[in]  aBufferLength 버퍼의 크기
 * @param[out] aErrorStack 에러 스택
 */
stlStatus stlGetPassword( const stlChar * aPrompt,
                          stlBool         aEcho,
                          stlChar       * aBuffer,
                          stlSize         aBufferLength,
                          stlErrorStack * aErrorStack )
{
    return strGetPassword( aPrompt, aEcho, aBuffer, aBufferLength, aErrorStack );
}

/** @} */
