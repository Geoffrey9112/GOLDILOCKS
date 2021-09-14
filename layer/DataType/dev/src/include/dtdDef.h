/*******************************************************************************
 * dtdDef.h
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
#ifndef _DTDDEF_H_
#define _DTDDEF_H_ 1

/**
 * @file dtdDef.h
 * @brief DataType Layer 데이타타입들의 내부 정의
 */

/**
 * @defgroup dtdDef Data Type internal Definition 
 * @ingroup dtInternal
 * @{
 */

extern const stlInt8 dtdHexLookup[256];


#define DTL_HEX_CHAR_TABLE_SIZE (17)
extern const stlChar dtdHexTable[DTL_HEX_CHAR_TABLE_SIZE];

#define DTD_BASE64_CHAR_TABLE_SIZE (65)
extern const stlChar dtdBase64[DTD_BASE64_CHAR_TABLE_SIZE];
extern const stlChar dtdBase64Char[257];
extern const stlInt8 dtdBase64lookup[256];

/*******************************************************************************
 *  DTD_ROWID_DTLX_TO_BASE64 는 
 *  dtdRowId.mDigits의 값에 대해 BASE64 문자로 변환.
 *
 *  BASE64 인코딩 : 6bit를 한 문자로 변환 (64개의 ascii 문자 중 하나로 변환)
 *                  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
 *******************************************************************************/

#define DTD_ROWID_OBJECT_ID_TO_BASE64_LENGTH         ( 11 )
#define DTD_ROWID_TABLESPACE_ID_TO_BASE64_LENGTH     ( 3 )
#define DTD_ROWID_PAGE_ID_TO_BASE64_LENGTH           ( 6 )        
#define DTD_ROWID_ROW_NUMBER_TO_BASE64_LENGTH        ( 3 )


/**
 *  DTD_ROWID_OBJECT_ID_TO_BASE64 :                                                  <BR>
 *      dtdRowId.mDigits 중 OBJECT_ID 값에 대해 BASE64 문자로 변환.                  <BR>
 *                                                                                   <BR>
 *  예)                                                                              <BR>
 *      [ (0 0 0 0)(0 0 0 0 ][ 0 0)(0 0 0 0 0 0) ][ (0 0 0 0 1 1)(1 0 ][ 1 1 1 0)(1 1 0 0 ] <BR>
 *        --------  --------------  -----------      -----------  --------------  --------  <BR>
 *           A            A             A                 D             u             w     <BR>
 *                                                                                          <BR>
 *      [ 0 0)(0 0 0 0 0 0) ][ (0 0 0 0 0 0)(0 0 ][ 0 0 0 0)(0 0 0 0 ][ 0 0)(0 0 0 0 0 0) ] <BR>
 *        ---  -----------      -----------  --------------  --------------  -----------    <BR>
 *                 A                A             A                A               A        <BR>
 *                                                                                   <BR>
 *  변환방법) . objectID ( physical ID ) : 16406775070720 이면,                      <BR>
 *              aRowIdType->mDigits[0] = 0x00,                                       <BR>
 *              aRowIdType->mDigits[1] = 0x00,                                       <BR>
 *              aRowIdType->mDigits[2] = 0x0E,                                       <BR>
 *              aRowIdType->mDigits[3] = 0xEC,                                       <BR>
 *              aRowIdType->mDigits[4] = 0x00,                                       <BR>
 *              aRowIdType->mDigits[5] = 0x00,                                       <BR>
 *              aRowIdType->mDigits[6] = 0x00,                                       <BR>
 *              aRowIdType->mDigits[7] = 0x00  이 들어있음.                          <BR>
 *                                                                                   <BR>
 *            1. aRowIdType->mDigits[0] ~ [7]의 8byte를 10진수로 변경.               <BR>
 *            2. 하위에서부터 6bit단위로 계산해 6비트씩 해당하는 문자를 aResultBuffer에 대입.             
 */
#define DTD_ROWID_OBJECT_ID_TO_BASE64( aRowIdType, aResultBuffer )              \
    {                                                                           \
        stlInt64  sTempValue;                                                   \
                                                                                \
        sTempValue = (stlInt64)(((stlUInt64)((aRowIdType)->mDigits[0]) << 56) | \
                                ((stlUInt64)((aRowIdType)->mDigits[1]) << 48) | \
                                ((stlUInt64)((aRowIdType)->mDigits[2]) << 40) | \
                                ((stlUInt64)((aRowIdType)->mDigits[3]) << 32) | \
                                ((stlUInt64)((aRowIdType)->mDigits[4]) << 24) | \
                                ((stlUInt64)((aRowIdType)->mDigits[5]) << 16) | \
                                ((stlUInt64)((aRowIdType)->mDigits[6]) << 8)  | \
                                ((stlUInt64)((aRowIdType)->mDigits[7])));       \
                                                                                \
        aResultBuffer[0] = dtdBase64[ (stlUInt8)((sTempValue >> 60) & 0x3F) ];  \
        aResultBuffer[1] = dtdBase64[ (stlUInt8)((sTempValue >> 54) & 0x3F) ];  \
        aResultBuffer[2] = dtdBase64[ (stlUInt8)((sTempValue >> 48) & 0x3F) ];  \
        aResultBuffer[3] = dtdBase64[ (stlUInt8)((sTempValue >> 42) & 0x3F) ];  \
        aResultBuffer[4] = dtdBase64[ (stlUInt8)((sTempValue >> 36) & 0x3F) ];  \
        aResultBuffer[5] = dtdBase64[ (stlUInt8)((sTempValue >> 30) & 0x3F) ];  \
        aResultBuffer[6] = dtdBase64[ (stlUInt8)((sTempValue >> 24) & 0x3F) ];  \
        aResultBuffer[7] = dtdBase64[ (stlUInt8)((sTempValue >> 18) & 0x3F) ];  \
        aResultBuffer[8] = dtdBase64[ (stlUInt8)((sTempValue >> 12) & 0x3F) ];  \
        aResultBuffer[9] = dtdBase64[ (stlUInt8)((sTempValue >> 6) & 0x3F) ];   \
        aResultBuffer[10] = dtdBase64[ (stlUInt8)(sTempValue & 0x3F) ];         \
    }

/**
 *  DTD_ROWID_TABLESPACE_ID_TO_BASE64 :                                  <BR>
 *      dtdRowId.mDigits 중 TABLESPACE_ID 값에 대해 BASE64 문자로 변환.  <BR>
 *                                                                       <BR>
 *  예)                                                                  <BR> 
 *      [ (0 0 0 0)(0 0 0 0 ][ 0 0)(0 0 0 0 0 6) ]                       <BR>
 *         -------  --------------  ------------                         <BR> 
 *            A           A               G                              <BR>
 *                                                                       <BR>
 *  변환방법) . 테이블스페이스 아이디 : 2 이면,                          <BR>
 *              aRowIdType->mDigits[8] = 0x00,                           <BR>
 *              aRowIdType->mDigits[9] = 0x02 이 들어 있음.              <BR> 
 *                                                                       <BR>
 *            1. aRowIdType->mDigits[8] ~ [9]의 2byte를 10진수로 변경.   <BR>
 *            2. 하위에서부터 6bit단위로 계산해 6bit씩 해당하는 문자를 aResultBuffer에 대입. 
 */
#define DTD_ROWID_TABLESPACE_ID_TO_BASE64( aRowIdType, aResultBuffer )          \
    {                                                                           \
        stlUInt16  sTempValue;                                                  \
                                                                                \
        sTempValue = (stlUInt16)(((stlUInt16)((aRowIdType)->mDigits[8]) << 8) | \
                                 ((stlUInt16)((aRowIdType)->mDigits[9])));      \
                                                                                \
        aResultBuffer[0] = dtdBase64[ (stlUInt8)((sTempValue >> 12) & 0x3F) ];  \
        aResultBuffer[1] = dtdBase64[ (stlUInt8)((sTempValue >> 6) & 0x3F) ];   \
        aResultBuffer[2] = dtdBase64[ (stlUInt8)(sTempValue & 0x3F) ];          \
    }

/**
 *  DTD_ROWID_PAGE_ID_TO_BASE64 :                                                    <BR>
 *      dtdRowId.mDigits 중 PAGE_ID 값에 대해 BASE64 문자로 변환.                    <BR>
 *                                                                                   <BR>
 *  예)                                                                              <BR>
 *      [ (0 0)(0 0 0 0 0 0) ][ (0 0 0 0 0 0)(0 0 ][ 0 0 0 0)(0 0 0 0 ][ 0 0)(1 0 0 0 1 0) ] <BR>
 *         ---  -----------      -----------  --------------  --------------  -----------    <BR>
 *          A        A                A             A               A               i        <BR>
 *                                                                                   <BR>
 *  변환방법) . 페이지 아이디 : 34 이면,                                             <BR>
 *              aRowIdType->mDigits[10] = 0x00,                                      <BR>
 *              aRowIdType->mDigits[11] = 0x00,                                      <BR>
 *              aRowIdType->mDigits[12] = 0x00,                                      <BR> 
 *              aRowIdType->mDigits[13] = 0x22 이 들어 있음.                         <BR>
 *                                                                                   <BR>
 *            1. aRowIdType->mDigits[10] ~ [13]의 2byte를 10진수로 변경.             <BR>
 *            2. 하위에서부터 6bit단위로 계산해 6bit씩 해당하는 문자를 aResultBuffer에 대입.
 */

#define DTD_ROWID_PAGE_ID_TO_BASE64( aRowIdType, aResultBuffer )                        \
    {                                                                                   \
        stlUInt32  sTempValue;                                                          \
                                                                                        \
        sTempValue = (stlUInt32)(((stlUInt32)((aRowIdType)->mDigits[10]) << 24) |       \
                                 ((stlUInt32)((aRowIdType)->mDigits[11]) << 16) |       \
                                 ((stlUInt32)((aRowIdType)->mDigits[12]) << 8) |        \
                                 ((stlUInt32)((aRowIdType)->mDigits[13])));             \
                                                                                        \
        aResultBuffer[0] = dtdBase64[ (stlUInt8)((sTempValue >> 30) & 0x3F) ];          \
        aResultBuffer[1] = dtdBase64[ (stlUInt8)((sTempValue >> 24) & 0x3F) ];          \
        aResultBuffer[2] = dtdBase64[ (stlUInt8)((sTempValue >> 18) & 0x3F) ];          \
        aResultBuffer[3] = dtdBase64[ (stlUInt8)((sTempValue >> 12) & 0x3F) ];          \
        aResultBuffer[4] = dtdBase64[ (stlUInt8)((sTempValue >> 6) & 0x3F) ];           \
        aResultBuffer[5] = dtdBase64[ (stlUInt8)(sTempValue & 0x3F) ];                  \
    }

/**
 *  DTD_ROWID_ROW_NUMBER_TO_BASE64 :                                             <BR>
 *      dtdRowId.mDigits 중 ROW_NUMBER(PAGEOFFSET) 값에 대해 BASE64 문자로 변환. <BR>
 *                                                                               <BR>
 *  예)                                                                          <BR>
 *      [ (0 0 0 0)(0 0 0 0 ][ 0 0)(0 0 0 0 0 6) ]                               <BR>
 *         -------  --------------- ------------                                 <BR>
 *            A           A              G                                       <BR>
 *                                                                               <BR>
 *  변환방법) . 페이지내 옵셋 : 6 이면,                                          <BR>
 *              aRowIdType->mDigits[14] = 0x00,                                  <BR>
 *              aRowIdType->mDigits[15] = 0x06 이 들어 있음.                     <BR>
 *                                                                               <BR>
 *            1. aRowIdType->mDigits[14] ~ [15]의 2byte를 10진수로 변경.         <BR>
 *            2. 하위에서부터 6bit단위로 계산해 6bit씩 해당하는 문자를 aResultBuffer에 대입.
 */
#define DTD_ROWID_ROW_NUMBER_TO_BASE64( aRowIdType, aResultBuffer )             \
    {                                                                           \
        stlInt16  sTempValue;                                                   \
                                                                                \
        sTempValue = (stlInt16)(((stlUInt16)((aRowIdType)->mDigits[14]) << 8) | \
                                ((stlUInt16)((aRowIdType)->mDigits[15])));      \
                                                                                \
        aResultBuffer[0] = dtdBase64[ (stlUInt8)((sTempValue >> 12) & 0x3F) ];  \
        aResultBuffer[1] = dtdBase64[ (stlUInt8)((sTempValue >> 6) & 0x3F) ];   \
        aResultBuffer[2] = dtdBase64[ (stlUInt8)(sTempValue & 0x3F) ];          \
    }


/*******************************************************************************
 * DTD_DTL_TO_DTL_DIGIT 는
 * OBJECT_ID, TABLESPACE_ID, PAGE_ID, ROW_NUMBER(PAGE_OFFSET)을
 * dtlRowIdType->mDigits의 각 배열에 넣어준다.
 *******************************************************************************/

/*
 * DTD_OBJECT_ID_TO_ROWID_DIGIT :                                   <BR>
 *     objectid 를 dtlRowIdType->mDigits[0] ~ [7]의 8byte에 넣는다. <BR>
 *                                                                  <BR>
 * 예)   objectID ( physical ID ) : 16406775070720                  <BR>
 *                                                                  <BR>
 *       aRowIdType->mDigits[0] = 0x00                              <BR>
 *       aRowIdType->mDigits[1] = 0x00                              <BR>
 *       aRowIdType->mDigits[2] = 0x0E                              <BR>
 *       aRowIdType->mDigits[3] = 0xEC                              <BR>
 *       aRowIdType->mDigits[4] = 0x00                              <BR>
 *       aRowIdType->mDigits[5] = 0x00                              <BR>
 *       aRowIdType->mDigits[6] = 0x00                              <BR>
 *       aRowIdType->mDigits[7] = 0x00 
 */ 
#define DTD_OBJECT_ID_TO_ROWID_DIGIT( aObjectId, aRowIdType )                   \
    {                                                                           \
        (sRowIdType)->mDigits[0] = (((stlUInt64)(aObjectId)) >> 56) & 0xFF;     \
        (sRowIdType)->mDigits[1] = (((stlUInt64)(aObjectId)) >> 48) & 0xFF;     \
        (sRowIdType)->mDigits[2] = (((stlUInt64)(aObjectId)) >> 40) & 0xFF;     \
        (sRowIdType)->mDigits[3] = (((stlUInt64)(aObjectId)) >> 32) & 0xFF;     \
        (sRowIdType)->mDigits[4] = (((stlUInt64)(aObjectId)) >> 24) & 0xFF;     \
        (sRowIdType)->mDigits[5] = (((stlUInt64)(aObjectId)) >> 16) & 0xFF;     \
        (sRowIdType)->mDigits[6] = (((stlUInt64)(aObjectId)) >> 8) & 0xFF;      \
        (sRowIdType)->mDigits[7] = ((stlUInt64)(aObjectId)) & 0xFF;             \
    }                                                        

/*
 * DTD_TABLESPACE_ID_TO_ROWID_DIGIT :                               <BR>
 *     objectid 를 dtlRowIdType->mDigits[8] ~ [9]의 8byte에 넣는다. <BR>
 *                                                                  <BR>
 * 예)   테이블스페이스 아이디 : 2                                  <BR>
 *       mDigits[8] = 0x00                                          <BR>
 *       mDigits[9] = 0x02 
 */ 
#define DTD_TABLESPACE_ID_TO_ROWID_DIGIT( aTablespaceId, aRowIdType )           \
    {                                                                           \
        (aRowIdType)->mDigits[8] = (((stlUInt16)(aTablespaceId)) >> 8) & 0xFF;  \
        (aRowIdType)->mDigits[9] = ((stlUInt16)(aTablespaceId)) & 0xFF;         \
    }                                                        

/*
 * DTD_PAGE_ID_TO_ROWID_DIGIT :                                       <BR>
 *     objectid 를 dtlRowIdType->mDigits[10] ~ [13]의 8byte에 넣는다. <BR> 
 *                                                                    <BR>
 * 예)   페이지 아이디 : 34                                           <BR>
 *       mDigits[10] = 0x00                                           <BR>
 *       mDigits[11] = 0x00                                           <BR>
 *       mDigits[12] = 0x00                                           <BR>
 *       mDigits[13] = 0x22
 */ 
#define DTD_PAGE_ID_TO_ROWID_DIGIT( aPageId, aRowIdType )                       \
    {                                                                           \
        (aRowIdType)->mDigits[10] = (((stlUInt32)(aPageId)) >> 24) & 0xFF;      \
        (aRowIdType)->mDigits[11] = (((stlUInt32)(aPageId)) >> 16) & 0xFF;      \
        (aRowIdType)->mDigits[12] = (((stlUInt32)(aPageId)) >> 8) & 0xFF;       \
        (aRowIdType)->mDigits[13] = ((stlUInt32)(aPageId)) & 0xFF;              \
    }

/*
 * DTD_ROW_NUMBER_TO_ROWID_DIGIT :                                    <BR>
 *     objectid 를 dtlRowIdType->mDigits[14] ~ [15]의 8byte에 넣는다. <BR>
 *                                                                    <BR>
 * 예)   페이지내 옵셋 : 6                                            <BR>
 *       mDigits[14] = 0x00                                           <BR>
 *       mDigits[15] = 0x06
 */ 
#define DTD_ROW_NUMBER_TO_ROWID_DIGIT( aRowNumber, aRowIdType )                 \
    {                                                                           \
        (aRowIdType)->mDigits[14] = (((stlUInt16)(aRowNumber)) >> 8) & 0xFF;    \
        (aRowIdType)->mDigits[15] = ((stlUInt16)(aRowNumber)) & 0xFF;           \
    }


/*******************************************************************************
 * Date/Time을 위한 정의
 ******************************************************************************/
#define DTD_ABS_SIGNBIT     ( (stlUInt8) 0200 )
#define DTD_VAL_MASK        ( (stlUInt8) 0177 )
#define DTD_POS(n)          (n)
#define DTD_NEG(n)          ((n)|DTD_ABS_SIGNBIT)
#define DTD_SIGNED_CHAR(c)  ( (c) & DTD_ABS_SIGNBIT ? -((c) & DTD_VAL_MASK) : (c) )
#define DTD_FROM_VAL(tp)    ( -DTD_SIGNED_CHAR((tp)->mValue) * 15 )
#define DTD_TO_VAL(tp, v)   ((tp)->mValue = ((v) < 0 ? DTD_NEG((-(v))/15) : DTD_POS(v)/15))

#define DTD_APPEND_CHAR( buffer, end, newchar )                         \
    do                                                                  \
    {   STL_TRY_THROW( ((buffer) + 1) < (end), RAMP_ERROR_BAD_FORMAT ); \
        *(buffer) = (newchar);                                          \
        (buffer)++;                                                     \
    } while(0)

#define DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE( aIntervalDayToSecond )                       \
    ( (aIntervalDayToSecond)->mTime + ((aIntervalDayToSecond)->mDay * DTL_USECS_PER_DAY) )


#define DTD_GET_DATETIME_SYEAR( aYear, aIsInterval ) ( (aIsInterval) == STL_TRUE ? (aYear) : (aYear) <= 0 ? ((aYear) - 1) : (aYear) )


/**
 * microsecond로 표현한 aTime(dtlTimeType, dtlTimeTzType->mTime)과
 * time zone을 인자로 받아
 * microsecond 단위로 계산한다.
 * 예) DTD_GET_TIMEOFFSET_FROM_TIME_AND_TIMEZONE( dtlTimeTzType->mTime, dtlTimeTzType->mTimeZone );
 *     DTD_GET_TIMEOFFSET_FROM_TIME_AND_TIMEZONE( dtlTimeType, session time zone );
 */
#define DTD_GET_TIMEOFFSET_FROM_TIME_AND_TIMEZONE( aTime, aTimeZone )   \
    ( (aTime) + ((aTimeZone) * DTL_USECS_PER_SEC) )

/**
 * aTimestamp(dtlTimestamp type)와  time zone을 인자로 받아
 * microsecond 단위로 계산한다.
 * 예) DTD_GET_TIMESTAMP_OFFSET_FROM_TIMESTAMP_AND_TIMEZONE( dtlTimestampType, session time zone );
 */
#define DTD_GET_TIMESTAMP_OFFSET_FROM_TIMESTAMP_AND_TIMEZONE( aTimestamp, aTimeZone )   \
    ( (aTimestamp) + ((aTimeZone) * DTL_USECS_PER_SEC) )

#define DTD_DATE_TO_TIMESTAMP( aDateTypeValue ) \
    ( (aDateTypeValue) * DTL_USECS_PER_DAY )    \

#define DTD_DATE_TO_TIMESTAMP_WITH_TIMEZONE( aDateTypeValue, aTimeZone )                \
    ( ((aDateTypeValue) * DTL_USECS_PER_DAY) + ((aTimeZone) * DTL_USECS_PER_SEC) )


#define DTD_INTERVAL_MAX_PRECISION_MIN_VALUE  ( -999999 )
#define DTD_INTERVAL_MAX_PRECISION_MAX_VALUE  ( 999999 )

typedef struct dtlNumericDigitString
{
    stlChar  mString[3];
} dtlNumericDigitString;

typedef struct dtlExponentString
{
    stlInt16   mStringCnt;
    stlChar    mString[6];
} dtlExponentString;


extern const dtlNumericDigitString    dtdNumericDigitStringArr[2][102];
extern const dtlNumericDigitString  * dtdNumericPosDigitString;
extern const dtlNumericDigitString  * dtdNumericNegDigitString;

extern const dtlExponentString    dtdNumberExponentStringArr[650];
extern const dtlExponentString  * dtdNumberExponentString;

#define DTD_NUMERIC_GET_DIGIT_STRING( isPos, digit )   ( (stlChar*) dtdNumericDigitStringArr[ (isPos) ][ (digit) ].mString )
#define DTD_NUMERIC_GET_POS_DIGIT_STRING( digit )      ( dtdNumericPosDigitString[ (digit) ].mString )
#define DTD_NUMERIC_GET_NEG_DIGIT_STRING( digit )      ( dtdNumericNegDigitString[ (digit) ].mString )

/** @} */

#endif /* _DTDDEF_H_ */

