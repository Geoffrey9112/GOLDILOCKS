/*******************************************************************************
 * dtDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtDef.h 1472 2011-07-20 03:23:06Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _DTDEF_H_
#define _DTDEF_H_ 1

/**
 * @file dtDef.h
 * @brief DataType Layer Internal Definitions
 */
#include <dtdDef.h>

#include <dtdBigInt.h>
#include <dtdBinary.h>
#include <dtdBoolean.h>
#include <dtdChar.h>
#include <dtdDataType.h>
#include <dtdDate.h>
#include <dtdDouble.h>
#include <dtdInteger.h>
#include <dtdIntervalYearToMonth.h>
#include <dtdIntervalDayToSecond.h>
#include <dtdLongvarbinary.h>
#include <dtdLongvarchar.h>
#include <dtdNumeric.h>
#include <dtdReal.h>
#include <dtdSmallInt.h>
#include <dtdTime.h>
#include <dtdTimeTz.h>
#include <dtdTimestamp.h>
#include <dtdTimestampTz.h>
#include <dtdVarbinary.h>
#include <dtdVarchar.h>
#include <dtdRowId.h>



/**
 * @defgroup dtInternal DataType Layer Internal Definitions
 * @internal
 * @{
 */


/**
 * @defgroup dtd DataType 관련 함수
 * @ingroup dtInternal
 * @{
 */

/** @} dtd */


/**
 * @defgroup dtf Function
 * @ingroup dtInternal
 * @{
 */


/*******************************************************************************
 * MACROS
 ******************************************************************************/


#ifdef STL_IGNORE_VALIDATION
#define DTF_CHECK_DTL_VALUE( aValue, aDtlDataType, aErrorStack )
#define DTF_CHECK_DTL_RESULT( aValue, aDtlDataType, aErrorStack )
#else
/** 함수의 피연산자인 구조체를 체크한다. */
#define DTF_CHECK_DTL_VALUE( aValue, aDtlDataType, aErrorStack )                        \
    do                                                                                  \
    {                                                                                   \
        stlInt64 sLen = 0;                                                              \
        DTL_PARAM_VALIDATE( (aValue) != NULL, (aErrorStack) );                          \
        DTL_PARAM_VALIDATE( (aValue)->mType == (aDtlDataType), (aErrorStack) );         \
        sLen = (aValue)->mLength;                                                       \
        switch( (aValue)->mType )                                                       \
        {                                                                               \
            case DTL_TYPE_BOOLEAN:                                                      \
                DTL_PARAM_VALIDATE( sLen == DTL_BOOLEAN_SIZE,                           \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_NATIVE_SMALLINT:                                              \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_SMALLINT_SIZE,                   \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_NATIVE_INTEGER:                                               \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_INTEGER_SIZE,                    \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_NATIVE_BIGINT:                                                \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_BIGINT_SIZE,                     \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_NATIVE_REAL:                                                  \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_REAL_SIZE,                       \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_NATIVE_DOUBLE:                                                \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_DOUBLE_SIZE,                     \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_FLOAT:                                                        \
            case DTL_TYPE_NUMBER:                                                       \
                DTL_PARAM_VALIDATE( ( sLen >= DTL_NUMERIC_MIN_SIZE ) &&                 \
                                    ( sLen <= DTL_NUMERIC_MAX_SIZE ),                   \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_DECIMAL:                                                      \
                /* 구현 필요함 */                                                       \
                break;                                                                  \
            case DTL_TYPE_CHAR:                                                         \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_VARCHAR_MAX_CHAR_STRING_SIZE ),       \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_CLOB:                                                         \
                /* 구현 필요함 */                                                       \
                break;                                                                  \
            case DTL_TYPE_BINARY:                                                       \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_VARBINARY_STRING_SIZE ),              \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_BLOB:                                                         \
                /* 구현 필요함 */                                                       \
                break;                                                                  \
            case DTL_TYPE_DATE:                                                         \
                DTL_PARAM_VALIDATE( sLen == DTL_DATE_SIZE,                              \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_TIME:                                                         \
                DTL_PARAM_VALIDATE( sLen == DTL_TIME_SIZE,                              \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_TIMESTAMP:                                                    \
                DTL_PARAM_VALIDATE( sLen == DTL_TIMESTAMP_SIZE,                         \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_TIME_WITH_TIME_ZONE:                                          \
                DTL_PARAM_VALIDATE( sLen == DTL_TIMETZ_SIZE,                            \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:                                     \
                DTL_PARAM_VALIDATE( sLen == DTL_TIMESTAMPTZ_SIZE,                       \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:                                       \
                DTL_PARAM_VALIDATE( sLen == DTL_INTERVAL_YEAR_TO_MONTH_SIZE,            \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_INTERVAL_DAY_TO_SECOND:                                       \
                DTL_PARAM_VALIDATE( sLen == DTL_INTERVAL_DAY_TO_SECOND_SIZE,            \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_VARCHAR:                                                      \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_VARCHAR_MAX_CHAR_STRING_SIZE ),       \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_LONGVARCHAR:                                                  \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_LONGVARCHAR_MAX_PRECISION ),          \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_VARBINARY:                                                    \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_VARBINARY_STRING_SIZE ),              \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_LONGVARBINARY:                                                \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_LONGVARBINARY_MAX_PRECISION ),        \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_ROWID:                                                        \
                DTL_PARAM_VALIDATE( sLen == DTL_ROWID_SIZE,                             \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            default:                                                                    \
                STL_DASSERT( STL_FALSE );                                               \
                break;                                                                  \
        }                                                                               \
    } while( 0 )

/** dtfArithmetic 함수의 결과가 저장될 구조체를 체크한다. */
#define DTF_CHECK_DTL_RESULT( aValue, aDtlDataType, aErrorStack )               \
    do                                                                          \
    {                                                                           \
        DTL_PARAM_VALIDATE( (aValue) != NULL, (aErrorStack) );                  \
        DTL_PARAM_VALIDATE( (aValue)->mValue != NULL, (aErrorStack) );          \
        DTL_PARAM_VALIDATE( (aValue)->mType == (aDtlDataType), (aErrorStack) ); \
    } while( 0 )
#endif


#ifdef STL_IGNORE_VALIDATION
#define DTF_CHECK_DTL_VALUE2( aValue, aDtlDataType1, aDtlDataType2, aErrorStack )
#define DTF_CHECK_DTL_RESULT2( aValue, aDtlDataType1, aDtlDataType2, aErrorStack )
#define DTF_CHECK_DTL_RESULT3( aValue, aDtlDataType1, aDtlDataType2, aDtlDataType3, aErrorStack )
#else
/** 함수의 피연산자인 구조체를 체크한다. */
#define DTF_CHECK_DTL_VALUE2( aValue, aDtlDataType1, aDtlDataType2, aErrorStack )       \
    do                                                                                  \
    {                                                                                   \
        stlInt64 sLen = 0;                                                              \
        DTL_PARAM_VALIDATE( (aValue) != NULL, (aErrorStack) );                          \
        DTL_PARAM_VALIDATE( (aValue)->mType == (aDtlDataType1) ||                       \
                            (aValue)->mType == (aDtlDataType2), (aErrorStack) );        \
        sLen = (aValue)->mLength;                                                       \
        switch( (aValue)->mType )                                                       \
        {                                                                               \
            case DTL_TYPE_BOOLEAN:                                                      \
                DTL_PARAM_VALIDATE( sLen == DTL_BOOLEAN_SIZE,                           \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_NATIVE_SMALLINT:                                              \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_SMALLINT_SIZE,                   \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_NATIVE_INTEGER:                                               \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_INTEGER_SIZE,                    \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_NATIVE_BIGINT:                                                \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_BIGINT_SIZE,                     \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_NATIVE_REAL:                                                  \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_REAL_SIZE,                       \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_NATIVE_DOUBLE:                                                \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_DOUBLE_SIZE,                     \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_FLOAT:                                                        \
            case DTL_TYPE_NUMBER:                                                       \
                DTL_PARAM_VALIDATE( ( sLen >= DTL_NUMERIC_MIN_SIZE ) &&                 \
                                    ( sLen <= DTL_NUMERIC_MAX_SIZE ),                   \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_DECIMAL:                                                      \
                /* 구현 필요함 */                                                       \
                break;                                                                  \
            case DTL_TYPE_CHAR:                                                         \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_VARCHAR_MAX_CHAR_STRING_SIZE ),       \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_CLOB:                                                         \
                /* 구현 필요함 */                                                       \
                break;                                                                  \
            case DTL_TYPE_BINARY:                                                       \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_VARBINARY_STRING_SIZE ),              \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_BLOB:                                                         \
                /* 구현 필요함 */                                                       \
                break;                                                                  \
            case DTL_TYPE_DATE:                                                         \
                DTL_PARAM_VALIDATE( sLen == DTL_DATE_SIZE,                              \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_TIME:                                                         \
                DTL_PARAM_VALIDATE( sLen == DTL_TIME_SIZE,                              \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_TIMESTAMP:                                                    \
                DTL_PARAM_VALIDATE( sLen == DTL_TIMESTAMP_SIZE,                         \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_TIME_WITH_TIME_ZONE:                                          \
                DTL_PARAM_VALIDATE( sLen == DTL_TIMETZ_SIZE,                            \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:                                     \
                DTL_PARAM_VALIDATE( sLen == DTL_TIMESTAMPTZ_SIZE,                       \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:                                       \
                DTL_PARAM_VALIDATE( sLen == DTL_INTERVAL_YEAR_TO_MONTH_SIZE,            \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_INTERVAL_DAY_TO_SECOND:                                       \
                DTL_PARAM_VALIDATE( sLen == DTL_INTERVAL_DAY_TO_SECOND_SIZE,            \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_VARCHAR:                                                      \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_VARCHAR_MAX_CHAR_STRING_SIZE ),       \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_LONGVARCHAR:                                                  \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_LONGVARCHAR_MAX_PRECISION ),          \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_VARBINARY:                                                    \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_VARBINARY_STRING_SIZE ),              \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_LONGVARBINARY:                                                \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_LONGVARBINARY_MAX_PRECISION ),        \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_ROWID:                                                        \
                DTL_PARAM_VALIDATE( sLen == DTL_ROWID_SIZE,                             \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            default:                                                                    \
                STL_DASSERT( STL_FALSE );                                               \
                break;                                                                  \
        }                                                                               \
    } while( 0 )
#define DTF_CHECK_DTL_RESULT2( aValue, aDtlDataType1, aDtlDataType2, aErrorStack )  \
    do                                                                              \
    {                                                                               \
        DTL_PARAM_VALIDATE( (aValue) != NULL, (aErrorStack) );                      \
        DTL_PARAM_VALIDATE( (aValue)->mValue != NULL, (aErrorStack) );              \
        DTL_PARAM_VALIDATE( (aValue)->mType == (aDtlDataType1) ||                   \
                            (aValue)->mType == (aDtlDataType2), (aErrorStack) );    \
    } while( 0 )
#define DTF_CHECK_DTL_RESULT3( aValue, aDtlDataType1, aDtlDataType2, aDtlDataType3, aErrorStack ) \
    do                                                                              \
    {                                                                               \
        DTL_PARAM_VALIDATE( (aValue) != NULL, (aErrorStack) );                      \
        DTL_PARAM_VALIDATE( (aValue)->mValue != NULL, (aErrorStack) );              \
        DTL_PARAM_VALIDATE( (aValue)->mType == (aDtlDataType1) ||                   \
                            (aValue)->mType == (aDtlDataType2) ||                   \
                            (aValue)->mType == (aDtlDataType3), (aErrorStack) );    \
    } while( 0 )        
#endif

#ifdef STL_IGNORE_VALIDATION
#define DTF_CHECK_DTL_GROUP( aValue, aDtlGroup, aErrorStack )
#else
/** 함수의 피연산자인 구조체를 체크한다. */
#define DTF_CHECK_DTL_GROUP( aValue, aDtlGroup, aErrorStack )                           \
    do                                                                                  \
    {                                                                                   \
        stlInt64 sLen = 0;                                                              \
        DTL_PARAM_VALIDATE( (aValue) != NULL, (aErrorStack) );                          \
        DTL_PARAM_VALIDATE( dtlDataTypeGroup((aValue)->mType) == (aDtlGroup),           \
                            (aErrorStack) );                                            \
        sLen = (aValue)->mLength;                                                       \
        switch( (aValue)->mType )                                                       \
        {                                                                               \
            case DTL_TYPE_BOOLEAN:                                                      \
                DTL_PARAM_VALIDATE( sLen == DTL_BOOLEAN_SIZE,                           \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_NATIVE_SMALLINT:                                              \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_SMALLINT_SIZE,                   \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_NATIVE_INTEGER:                                               \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_INTEGER_SIZE,                    \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_NATIVE_BIGINT:                                                \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_BIGINT_SIZE,                     \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_NATIVE_REAL:                                                  \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_REAL_SIZE,                       \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_NATIVE_DOUBLE:                                                \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_DOUBLE_SIZE,                     \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_FLOAT:                                                        \
            case DTL_TYPE_NUMBER:                                                       \
                DTL_PARAM_VALIDATE( ( sLen >= DTL_NUMERIC_MIN_SIZE ) &&                 \
                                    ( sLen <= DTL_NUMERIC_MAX_SIZE ),                   \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_DECIMAL:                                                      \
                /* 구현 필요함 */                                                       \
                break;                                                                  \
            case DTL_TYPE_CHAR:                                                         \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_VARCHAR_MAX_CHAR_STRING_SIZE ),       \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_CLOB:                                                         \
                /* 구현 필요함 */                                                       \
                break;                                                                  \
            case DTL_TYPE_BINARY:                                                       \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_VARBINARY_STRING_SIZE ),              \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_BLOB:                                                         \
                /* 구현 필요함 */                                                       \
                break;                                                                  \
            case DTL_TYPE_DATE:                                                         \
                DTL_PARAM_VALIDATE( sLen == DTL_DATE_SIZE,                              \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_TIME:                                                         \
                DTL_PARAM_VALIDATE( sLen == DTL_TIME_SIZE,                              \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_TIMESTAMP:                                                    \
                DTL_PARAM_VALIDATE( sLen == DTL_TIMESTAMP_SIZE,                         \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_TIME_WITH_TIME_ZONE:                                          \
                DTL_PARAM_VALIDATE( sLen == DTL_TIMETZ_SIZE,                            \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:                                     \
                DTL_PARAM_VALIDATE( sLen == DTL_TIMESTAMPTZ_SIZE,                       \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:                                       \
                DTL_PARAM_VALIDATE( sLen == DTL_INTERVAL_YEAR_TO_MONTH_SIZE,            \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_INTERVAL_DAY_TO_SECOND:                                       \
                DTL_PARAM_VALIDATE( sLen == DTL_INTERVAL_DAY_TO_SECOND_SIZE,            \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_VARCHAR:                                                      \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_VARCHAR_MAX_CHAR_STRING_SIZE ),       \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_LONGVARCHAR:                                                  \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_LONGVARCHAR_MAX_PRECISION ),          \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_VARBINARY:                                                    \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_VARBINARY_STRING_SIZE ),              \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_LONGVARBINARY:                                                \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                    \
                                    ( sLen <= DTL_LONGVARBINARY_MAX_PRECISION ),        \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            case DTL_TYPE_ROWID:                                                        \
                DTL_PARAM_VALIDATE( sLen == DTL_ROWID_SIZE,                             \
                                    (aErrorStack) );                                    \
                break;                                                                  \
            default:                                                                    \
                STL_DASSERT( STL_FALSE );                                               \
                break;                                                                  \
        }                                                                               \
    } while( 0 )
#endif


#ifdef STL_IGNORE_VALIDATION
#define DTF_CHECK_DTL_GROUP2( aValue, aDtlGroup1, aDtlGroup2, aErrorStack )
#else
/** 함수의 피연산자인 구조체를 체크한다. */
#define DTF_CHECK_DTL_GROUP2( aValue, aDtlGroup1, aDtlGroup2, aErrorStack )         \
    do                                                                              \
    {                                                                               \
        stlInt64 sLen = 0;                                                          \
        DTL_PARAM_VALIDATE( (aValue) != NULL, (aErrorStack) );                      \
        DTL_PARAM_VALIDATE( dtlDataTypeGroup[(aValue)->mType] == (aDtlGroup1)  ||   \
                            dtlDataTypeGroup[(aValue)->mType] == (aDtlGroup2),      \
                            (aErrorStack) );                                        \
        sLen = (aValue)->mLength;                                                   \
        switch( (aValue)->mType )                                                   \
        {                                                                           \
            case DTL_TYPE_BOOLEAN:                                                  \
                DTL_PARAM_VALIDATE( sLen == DTL_BOOLEAN_SIZE,                       \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_NATIVE_SMALLINT:                                          \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_SMALLINT_SIZE,               \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_NATIVE_INTEGER:                                           \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_INTEGER_SIZE,                \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_NATIVE_BIGINT:                                            \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_BIGINT_SIZE,                 \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_NATIVE_REAL:                                              \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_REAL_SIZE,                   \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_NATIVE_DOUBLE:                                            \
                DTL_PARAM_VALIDATE( sLen == DTL_NATIVE_DOUBLE_SIZE,                 \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_FLOAT:                                                    \
            case DTL_TYPE_NUMBER:                                                   \
                DTL_PARAM_VALIDATE( ( sLen >= DTL_NUMERIC_MIN_SIZE ) &&             \
                                    ( sLen <= DTL_NUMERIC_MAX_SIZE ),               \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_DECIMAL:                                                  \
                /* 구현 필요함 */                                                   \
                break;                                                              \
            case DTL_TYPE_CHAR:                                                     \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                \
                                    ( sLen <= DTL_VARCHAR_MAX_CHAR_STRING_SIZE ),   \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_CLOB:                                                     \
                /* 구현 필요함 */                                                   \
                break;                                                              \
            case DTL_TYPE_BINARY:                                                   \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                \
                                    ( sLen <= DTL_VARBINARY_STRING_SIZE ),          \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_BLOB:                                                     \
                /* 구현 필요함 */                                                   \
                break;                                                              \
            case DTL_TYPE_DATE:                                                     \
                DTL_PARAM_VALIDATE( sLen == DTL_DATE_SIZE,                          \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_TIME:                                                     \
                DTL_PARAM_VALIDATE( sLen == DTL_TIME_SIZE,                          \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_TIMESTAMP:                                                \
                DTL_PARAM_VALIDATE( sLen == DTL_TIMESTAMP_SIZE,                     \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_TIME_WITH_TIME_ZONE:                                      \
                DTL_PARAM_VALIDATE( sLen == DTL_TIMETZ_SIZE,                        \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:                                 \
                DTL_PARAM_VALIDATE( sLen == DTL_TIMESTAMPTZ_SIZE,                   \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:                                   \
                DTL_PARAM_VALIDATE( sLen == DTL_INTERVAL_YEAR_TO_MONTH_SIZE,        \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_INTERVAL_DAY_TO_SECOND:                                   \
                DTL_PARAM_VALIDATE( sLen == DTL_INTERVAL_DAY_TO_SECOND_SIZE,        \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_VARCHAR:                                                  \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                \
                                    ( sLen <= DTL_VARCHAR_MAX_CHAR_STRING_SIZE ),   \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_LONGVARCHAR:                                              \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                \
                                    ( sLen <= DTL_LONGVARCHAR_MAX_PRECISION ),      \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_VARBINARY:                                                \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                \
                                    ( sLen <= DTL_VARBINARY_STRING_SIZE ),          \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_LONGVARBINARY:                                            \
                DTL_PARAM_VALIDATE( ( sLen >= 0 ) &&                                \
                                    ( sLen <= DTL_LONGVARBINARY_MAX_PRECISION ),    \
                                    (aErrorStack) );                                \
                break;                                                              \
            case DTL_TYPE_ROWID:                                                    \
                DTL_PARAM_VALIDATE( sLen == DTL_ROWID_SIZE,                         \
                                    (aErrorStack) );                                \
                break;                                                              \
            default:                                                                \
                STL_DASSERT( STL_FALSE );                                           \
                break;                                                              \
        }                                                                           \
    } while( 0 )
#endif

/* dtlDataValue 에서 dtlBooleanType 으로 변환한다. */
#define DTF_BOOLEAN( aValue )       ( *(dtlBooleanType *)(aValue)->mValue )
/* dtlDataValue 에서 dtlSmallIntType 으로 변환한다. */
#define DTF_SMALLINT( aValue )      ( *(dtlSmallIntType *)(aValue)->mValue )
/* dtlDataValue 에서 dtlIntegerType 으로 변환한다. */
#define DTF_INTEGER( aValue )       ( *(dtlIntegerType *)(aValue)->mValue )
/* dtlDataValue 에서 dtlBigIntType 으로 변환한다. */
#define DTF_BIGINT( aValue )        ( *(dtlBigIntType *)(aValue)->mValue )
/* dtlDataValue 에서 dtlDoubleType 으로 변환한다. */
#define DTF_REAL( aValue )          ( *(dtlRealType *)(aValue)->mValue )
/* dtlDataValue 에서 dtlDoubleType 으로 변환한다. */
#define DTF_DOUBLE( aValue )        ( *(dtlDoubleType *)(aValue)->mValue )
/* dtlDataValue 에서 dtlDateType 으로 변환한다. */
#define DTF_DATE( aValue )          ( *(dtlDateType *)(aValue)->mValue )
/* dtlDataValue 에서 dtlTimeType 으로 변환한다. */
#define DTF_TIME( aValue )          ( *(dtlTimeType *)(aValue)->mValue )
/* dtlDataValue 에서 dtlTimestampType 으로 변환한다. */
#define DTF_TIMESTAMP( aValue )     ( *(dtlTimestampType *)(aValue)->mValue )
/* dtlDataValue 에서 dtlNumericType pointer 로 변환한다. */
#define DTF_NUMERIC( aValue )       ( (dtlNumericType *)(aValue)->mValue )
/* dtlDataValue 에서 dtlCharType pointer 로 변환한다. */
#define DTF_CHAR( aValue )          ( (dtlCharType)(aValue)->mValue )
/* dtlDataValue 에서 dtlVarcharType pointer 로 변환한다. */
#define DTF_VARCHAR( aValue )       ( (dtlVarcharType)(aValue)->mValue )
/* dtlDataValue 에서 dtlBinaryType pointer 로 변환한다. */
#define DTF_BINARY( aValue )        ( (dtlBinaryType)(aValue)->mValue )
/* dtlDataValue 에서 dtlVarbinaryType pointer 로 변환한다. */
#define DTF_VARBINARY( aValue )     ( (dtlVarbinaryType)(aValue)->mValue )
/* dtlDataValue 에서 dtlRowIdType pointer 로 변환한다. */
#define DTF_ROWID( aValue )         ( (dtlRowIdType *)(aValue)->mValue )

/* 두개의 파라미터가 같은 부호인지 확인한다. */
#define DTF_SAMESIGN( aA, aB )    ( ( ( ( aA ) < 0 ) == ( ( aB ) < 0 ) ) ? STL_TRUE : STL_FALSE )
/* 두개의 파라미터가 다른 부호인지 확인한다. */
#define DTF_DIFFSIGN( aA, aB )    ( ( ( ( aA ) < 0 ) != ( ( aB ) < 0 ) ) ? STL_TRUE : STL_FALSE )

/*
 * 값이 32bit 범위내에 있는지 체크한다.
 * dtlBigIntType 변수를 32bit로 casting 후
 * 다시 64bit로 casting 한 값과 동일한지 확인한다.
 * 이것은 하위 32bit만 사용하는지와는 별개이다.
 * (음수의 경우 -1이 전체 bit를 사용함으로..)
 * 사용처: BigInt 의 * 연산 (32bit * 32bit는 64bit를 overflow 하지 않는다.)
 */
#define DTF_CHECK_VALUE_INT32( aA )                             \
    ( ( ( aA ) == (dtlBigIntType)( (dtlIntegerType)(aA) ) ) ?   \
      STL_TRUE : STL_FALSE )


#define DTF_MAX_FACTORIAL_BIGINT  ( 20 )
#define DTF_MAX_FACTORIAL_NUMERIC ( 83 )


typedef struct dtfPredefinedNumeric
{
    stlInt64          mLength;
    dtlNumericType  * mNumeric;
} dtfPredefinedNumeric;

extern const stlInt64 dtfFactorialBigInt[ DTF_MAX_FACTORIAL_BIGINT + 1 ];
extern const dtfPredefinedNumeric dtfFactorialNumeric[ DTF_MAX_FACTORIAL_NUMERIC + 1 ];

extern const dtfPredefinedNumeric dtfPINumeric[ 1 ];


extern dtlBuiltInFuncPtr  gDtfAbsFunc[ DTL_TYPE_MAX ];
extern dtlBuiltInFuncPtr  gDtfNegativeFunc[ DTL_TYPE_MAX ];
extern dtlBuiltInFuncPtr  gDtfPositiveFunc[ DTL_TYPE_MAX ];
extern dtlBuiltInFuncPtr  gDtfBitwiseNotFunc[ DTL_TYPE_MAX ];
extern dtlBuiltInFuncPtr  gDtfBitwiseShiftLeftFunc[ DTL_TYPE_MAX ];
extern dtlBuiltInFuncPtr  gDtfBitwiseShiftRightFunc[ DTL_TYPE_MAX ];

/*
 * Like function Pattern info
 */
typedef struct dtfPatternInfo
{
    stlBool   mIsEscape;
}dtfPatternInfo;

#define DTF_CHECK_ESCAPE_PATTERN_MATCH( aPattern, aEscape, aLen, aResult )  \
    {                                                                       \
        stlInt8 sCount = ( aLen );                                          \
        (aResult) = STL_TRUE;                                               \
        while( ( sCount ) > 0 )                                             \
        {                                                                   \
            ( sCount )--;                                                   \
            if( (aPattern[(sCount)]) == (sEscape[(sCount)]) )               \
            {                                                               \
                /* Do Nothing */                                            \
            }                                                               \
            else                                                            \
            {                                                               \
                (aResult) = STL_FALSE;                                      \
                break;                                                      \
            }                                                               \
        }                                                                   \
    }                                                                                       

#define DTF_CHECK_ESCAPE_CHARACTER( aPattern, aEscape, aPLen, aELen, aResult )                  \
    {                                                                                           \
        if( ( (*aPattern) == '%' ) || ( (*aPattern) == '_' ) )                                  \
        {                                                                                       \
            (aResult) = STL_TRUE;                                                               \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            if( (aPLen) == (aELen) )                                                            \
            {                                                                                   \
                DTF_CHECK_ESCAPE_PATTERN_MATCH( (aPattern), (aEscape), (aPLen), (aResult) );    \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                (aResult) = STL_FALSE;                                                          \
            }                                                                                   \
        }                                                                                       \
    }


/**
 * date + integer (month)  계산
 */

#define DTF_DATE_ADD_MONTHS( aDtlExpTime, aMonths )  \
    {                                               \
        stlBool sGetLastDay = STL_FALSE;                                                                     \
                                                                                                             \
        if( (aDtlExpTime).mDay == dtlDayTab[DTL_IS_LEAP((aDtlExpTime).mYear)][(aDtlExpTime).mMonth - 1] )    \
        {                                                                                                    \
            sGetLastDay = STL_TRUE;                                                                          \
        }                                                                                                    \
        else                                                                                                 \
        {                                                                                                    \
            sGetLastDay = STL_FALSE;                                                                         \
        }                                                                                                    \
                                                                                                             \
        (aDtlExpTime).mMonth = (aDtlExpTime).mMonth + (aMonths);                                             \
        if( (aDtlExpTime).mMonth > DTL_MONTHS_PER_YEAR )                                                     \
        {                                                                                                    \
            (aDtlExpTime).mYear += ((aDtlExpTime).mMonth - 1) / DTL_MONTHS_PER_YEAR;                         \
            (aDtlExpTime).mMonth = (((aDtlExpTime).mMonth - 1) % DTL_MONTHS_PER_YEAR) + 1;                   \
        }                                                                                                    \
        else if( (aDtlExpTime).mMonth < 1 )                                                                  \
        {                                                                                                    \
            (aDtlExpTime).mYear += ((aDtlExpTime).mMonth / DTL_MONTHS_PER_YEAR) - 1;                         \
            (aDtlExpTime).mMonth = ((aDtlExpTime).mMonth % DTL_MONTHS_PER_YEAR) + DTL_MONTHS_PER_YEAR;       \
        }                                                                                                    \
        else                                                                                                 \
        {                                                                                                    \
            /* Do Nothing */                                                                                 \
        }                                                                                                    \
                                                                                                             \
        if( sGetLastDay == STL_TRUE )                                                                        \
        {                                                                                                    \
            (aDtlExpTime).mDay = dtlDayTab[DTL_IS_LEAP((aDtlExpTime).mYear)][(aDtlExpTime).mMonth - 1];      \
        }                                                                                                    \
        else                                                                                                 \
        {                                                                                                    \
            if( (aDtlExpTime).mDay > dtlDayTab[DTL_IS_LEAP((aDtlExpTime).mYear)][(aDtlExpTime).mMonth - 1] ) \
            {                                                                                                \
                (aDtlExpTime).mDay = dtlDayTab[DTL_IS_LEAP((aDtlExpTime).mYear)][(aDtlExpTime).mMonth - 1];  \
            }                                                                                                \
            else                                                                                             \
            {                                                                                                \
                /* Do Nothing */                                                                             \
            }                                                                                                \
        }                                                                                                    \
    }


/**
 * @brief date type 또는 timestamp type 의 value에 대한 validate
 * [in]  aDateTime    ( date or timestamp value : microsecond 단위의 value임 )
 * [out] aResult      valid : STL_TRUE, invalid : STL_FALSE
 */        

#define DTF_DATETIME_VALUE_IS_VALID( aDateTime, aIsValid )                                          \
    {                                                                                               \
        dtlTimestampType sDateTime = aDateTime;                                                     \
        stlInt32         sJulianDay = 0;                                                            \
        DTL_GET_DATE_TIME_FROM_TIMESTAMP( (sDateTime), sJulianDay, DTL_USECS_PER_DAY );             \
        sJulianDay += DTL_EPOCH_JDATE;                                                              \
        (aIsValid) =                                                                                \
            ( ( ( sJulianDay >= 0 ) && ( sJulianDay <= DTL_JULIAN_MAXJULDAY ) ) == STL_TRUE )       \
            ? STL_TRUE : STL_FALSE;                                                                 \
    }


/**
 * @brief character set별 function list 검사.
 *   <BR> string 함수의 많은 부분은 성능을 위하여 character set 별로 구현되어 있는데,
 *   <BR> character set이 추가될때 누락되는 부분이 없도록 하기 위한 검사 코드
 *   <BR> aFuncList의 배열의 갯수는 DTL_CHARACTERSET_MAX + 1 이고,
 *   <BR> 배열 마지막이 NULL 이어야 한다.
 */
#ifdef STL_IGNORE_VALIDATION
#define DTF_CHECK_CHAR_SET_FUNC_LIST( aFuncList )
#else
#define DTF_CHECK_CHAR_SET_FUNC_LIST( aFuncList )       \
    {                                                   \
        stlInt32 sCnt = 0;                              \
        while( (aFuncList)[sCnt] != NULL )              \
        {                                               \
            sCnt++;                                     \
        }                                               \
        STL_DASSERT( sCnt == DTL_CHARACTERSET_MAX );    \
    }
#endif


#define DTF_IS_DIGIT( _aStr ) ( ((_aStr)>='0') && ((_aStr)<='9') )
    
/**
 * @brief builtin function의 함수 원형 (for internal)
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      결과
 *             <BR> result결과가 arrary인 경우의 확장을 위해 void *를 output 인자로 받는다.
 *             <BR>
 *             <BR> 결과값의 갯수 차이에 따라 사용상의 차이가 발생하는데, 
 *             <BR> (1) 결과값이 하나인 함수들은 dtlDataValue로 casting해서 결과값을 만든다.
 *             <BR> (2) 결과값이 여러개인 함수들에 대한 고려는 이후에 ...
 *             <BR>
 * @param[out] aEnv              environment
 *             <BR> result결과가 arrary인 경우의 확장을 위해
 *             <BR> Environment를 void *의 output 인자로 받는다.
 *             <BR>
 *             <BR> DataType layer와 Execution layer에서 사용상의 차이가 발생하는데,
 *             <BR> result결과가 여러개인 함수는 execution library에 구현하기로 논의됨.
 *             <BR> (1) DataType layer : stlErrorStack으로 casting해서 사용.
 *             <BR>     env가 없고, DT layer의 함수들은 result결과가 하나이며,
 *             <BR>     메모리할당/해제도 하지 않으므로
 *             <BR>     stlErrorStack으로 casting해서 errorstack만 사용.
 *             <BR> (2) Execution Library : ellEnv로 casting해서 사용
 *             <BR>     EL layer의 함수들중에는 result결과가 여러개일 수 있고,
 *             <BR>     이 경우 메모리를 사용해야 할 것으로 보이며,
 *             <BR>     ellEnv로 casting해서 필요한 environment를 사용한다. 
 */
typedef stlStatus (*dtfBuiltInFuncPtr) ( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv );


/** @} dtf */

/** @} */

#endif /* _DTDEF_H_ */

