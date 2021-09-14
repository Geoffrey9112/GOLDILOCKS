/*******************************************************************************
 * dtlError.c
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
 * @file dtlError.c
 * @brief DataType Layer Error Routines
 */

#include <stl.h>

/**
 * @addtogroup dtlError
 * @{
 */

/**
 * @brief DataType Layer 에러 테이블.
 * 내부에러에 대응되는 외부에러와 에러 메세지로 구성됨.
 */
stlErrorRecord gDataTypeErrorTable[] =
{
    {   /* DTL_ERRCODE_INITIALIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "failed to initialize datatype layer"
    },
    {   /* DTL_ERRCODE_NOT_IMPLEMENTED */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
        "not implemented feature, in a function %s"
    },

    /**
     * C character to SQL
     */
    
    {   /* DTL_ERRCODE_C_STRING_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "byte length of data greater than column length"
    },
    {   /* DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "character length of data greater than column length"
    },
    {   /* DTL_ERRCODE_C_STRING_CONVERTED_TRUNCATION_FRACTIONAL_DIGITS */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "data converted with truncation of fractional digits"
    },
    {   /* DTL_ERRCODE_C_STRING_CONVERSION_DATA_LOSS_WHOLE_DIGITS */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "conversion of data would result in loss of whole digits"
    },
    {   /* DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_NUMERIC_LITERAL */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_CHARACTER_VALUE_FOR_CAST,
        "data value is not a numeric literal"
    },
    {   /* DTL_ERRCODE_C_STRING_DATA_OUTSIDE_RANGE_NUMBER_CONVERTED */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "data is outside the range of the data type to which the number is being converted"
    },
    {   /* DTL_ERRCODE_C_STRING_DATA_GREATER_0_LESS_2_NOT_EQUAL_1 */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "data is greater than 0, less than 2, and not equal to 1"
    },
    {   /* DTL_ERRCODE_C_STRING_DATA_LESS_0_GREATER_OR_EQUAL_2 */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "data is less than 0 or greater than or equal to 2"
    },
    {   /* DTL_ERRCODE_C_STRING_BYTE_LENGTH_DIVIDE_2_GREATER_COLUMN_BYTE_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "(byte length of data)/2 is greater than column byte length"
    },
    {   /* DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_HEXADECIMAL_VALUE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "data value is not a hexadecimal value"
    },
    {   /* DTL_ERRCODE_C_STRING_VALID_TIMESTAMP_TIME_NONZERO */
        STL_EXT_ERRCODE_DATA_EXCEPTION_DATETIME_FIELD_OVERFLOW,
        "data value is a valid timestamp literal; time portion is nonzero"
    },
    {   /* DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIMESTAMP_LITERAL */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_CHARACTER_VALUE_FOR_CAST,
        "data value is not a valid date literal or timestamp literal"
    },
    {   /* DTL_ERRCODE_C_STRING_VALID_TIMESTAMP_FRACTIONAL_SECONDS_NONZERO */
        STL_EXT_ERRCODE_DATA_EXCEPTION_DATETIME_FIELD_OVERFLOW,
        "data value is a valid timestamp literal; fractional seconds portion is nonzero"
    },
    {   /* DTL_ERRCODE_C_STRING_NOT_VALID_TIME_OR_TIMESTAMP_LITERAL */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_CHARACTER_VALUE_FOR_CAST,
        "data value is not a valid time literal or timestamp literal"
    },
    {   /* DTL_ERRCODE_C_STRING_VALID_TIMESTAMP_FRACTIONAL_SECONDS_TRUNCATED */
        STL_EXT_ERRCODE_DATA_EXCEPTION_DATETIME_FIELD_OVERFLOW,
        "data value is a valid timestamp literal; fractional seconds portion truncated"
    },
    {   /* DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIME_OR_TIMESTAMP_LITERAL */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_CHARACTER_VALUE_FOR_CAST,
        "data value is not a valid date literal or time literal or timestamp literal"
    },
    {   /* DTL_ERRCODE_C_STRING_INTERVAL_FIELD_TRUNCATED */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INTERVAL_FIELD_OVERFLOW,
        "data value is a valid interval value; the value in one of the fields is truncated"
    },
    {   /* DTL_ERRCODE_C_STRING_NOT_VALID_INTERVAL_LITERAL */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_CHARACTER_VALUE_FOR_CAST,
        "data value is not a valid interval literal"
    },

    /**
     * C numeric to SQL
     */

    {   /* DTL_ERRCODE_C_NUMERIC_NUMBER_DIGITS_GREATER_COLUMN_BYTE_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "number of digits greater than column byte length"
    },
    {   /* DTL_ERRCODE_C_NUMERIC_NUMBER_CHARACTERS_GREATER_COLUMN_CHARACTER_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "number of characters greater than column byte length"
    },
    {   /* DTL_ERRCODE_C_NUMERIC_DATA_CONVERTED_TRUNCATION_WHOLE_DIGITS */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "data converted with truncation of whole digits"
    },
    {   /* DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "data is outside the range of the data type to which the number is being converted"
    },
    {   /* DTL_ERRCODE_C_NUMERIC_DATA_GREATER_0_LESS_2_NOT_EQUAL_1 */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "data is greater than 0, less than 2, and not equal to 1"
    },
    {   /* DTL_ERRCODE_C_NUMERIC_DATA_LESS_0_GREATER_OR_EQUAL_2 */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "data is less than 0 or greater than or equal to 2"
    },
    {   /* DTL_ERRCODE_C_NUMERIC_INTERVAL_DATA_TRUNCATED */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INTERVAL_FIELD_OVERFLOW,
        "interval data truncated"
    },

    /**
     * C binary to SQL
     */
    
    {   /* DTL_ERRCODE_C_BINARY_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "byte length of data greater than column length"
    },
    {   /* DTL_ERRCODE_C_BINARY_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "character length of data greater than column length"
    },
    {   /* DTL_ERRCODE_C_BINARY_BYTE_LENGTH_NOT_EQUAL_SQL_DATA_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "byte length of data not equal to SQL data length"
    },

    /**
     * C date to SQL
     */

    {   /* DTL_ERRCODE_C_DATE_COLUMN_BYTE_LENGTH_LESS_THAN_DATE_DISPLAY_SIZE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "column byte length is less than %d"
    },
    {   /* DTL_ERRCODE_C_DATE_NOT_VALID_DATE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_DATETIME_FIELD_OVERFLOW,
        "data value is not a valid date"
    },
    {   /* DTL_ERRCODE_C_DATE_NOT_VALID_DATE_FORMAT */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_DATETIME_FORMAT,
        "data value is not a valid date format"
    },

    /**
     * C guid to SQL
     */

    {   /* DTL_ERRCODE_C_GUID_COLUMN_BYTE_LENGTH_LESS_THAN_GUID_DISPLAY_SIZE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "column byte length is less than %d"
    },
    {   /* DTL_ERRCODE_C_GUID_NOT_VALID_GUID */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_CHARACTER_VALUE_FOR_CAST,
        "data value is not a valid GUID"
    },
    
    /**
     * C time to SQL
     */

    {   /* DTL_ERRCODE_C_TIME_COLUMN_BYTE_LENGTH_LESS_THAN_TIME_DISPLAY_SIZE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "column byte length is less than %d"
    },
    {   /* DTL_ERRCODE_C_TIME_NOT_VALID_TIME */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "data value is not a valid time"
    },
    {   /* DTL_ERRCODE_C_TIME_NOT_VALID_TIME_FORMAT */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_DATETIME_FORMAT,
        "data value is not a valid time format"
    },

    /**
     * C timestamp to SQL
     */

    {   /* DTL_ERRCODE_C_TIMESTAMP_COLUMN_BYTE_LENGTH_BETWEEN_TIMESTAMP_DISPLAY_SIZE_AND_CHAR_BYTE_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "column byte length is between %d and character byte length"
    },
    {   /* DTL_ERRCODE_C_TIMESTAMP_COLUMN_BYTE_LENGTH_LESS_TIMESTAMP_DISPLAY_SIZE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "column byte length is less than %d"
    },
    {   /* DTL_ERRCODE_C_TIMESTAMP_NOT_VALID_TIME_FORMAT */
        STL_EXT_ERRCODE_DATA_EXCEPTION_DATETIME_FIELD_OVERFLOW,
        "data value is not a valid timestamp"
    },
    {   /* DTL_ERRCODE_C_TIMESTAMP_TIME_FILEDS_NONZERO */
        STL_EXT_ERRCODE_DATA_EXCEPTION_DATETIME_FIELD_OVERFLOW,
        "time fields are nonzero"
    },
    {   /* DTL_ERRCODE_C_TIMESTAMP_NOT_CONTAIN_VALID_DATE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_DATETIME_FORMAT,
        "data value does not contain a valid date"
    },
    {   /* DTL_ERRCODE_C_TIMESTAMP_FRACTIONAL_SECOND_NONZERO */
        STL_EXT_ERRCODE_DATA_EXCEPTION_DATETIME_FIELD_OVERFLOW,
        "fractional seconds fields are nonzero"
    },
    {   /* DTL_ERRCODE_C_TIMESTAMP_NOT_CONTAIN_VALID_TIME */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_DATETIME_FORMAT,
        "data value does not contain a valid time"
    },
    {   /* DTL_ERRCODE_C_TIMESTAMP_FRACTIONAL_SECOND_TRUNCATED */
        STL_EXT_ERRCODE_DATA_EXCEPTION_DATETIME_FIELD_OVERFLOW,
        "fractional seconds fields are truncated"
    },
    {   /* DTL_ERRCODE_C_TIMESTAMP_NOT_CONTAIN_VALID_TIMESTAMP */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_DATETIME_FORMAT,
        "data value does not contain a valid timestamp"
    },

    /**
     * C year_month interval to SQL
     */

    {   /* DTL_ERRCODE_C_YM_INTERVAL_COLUMN_BYTE_LENGTH_LESS_CHARACTER_BYTE_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "column byte length is less than character byte length"
    },
    {   /* DTL_ERRCODE_C_YM_INTERVAL_NOT_VALID_INTERVAL_LITERAL */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INTERVAL_FIELD_OVERFLOW,
        "data value is not a valid interval literal"
    },
    {   /* DTL_ERRCODE_C_YM_INTERVAL_CONVERSION_TRUNCATION_WHOLE_DIGITS */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "conversion resulted in truncation of whole digits"
    },
    {   /* DTL_ERRCODE_C_YM_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INTERVAL_FIELD_OVERFLOW,
        "one or more fields of data value were truncated during conversion"
    },

    /**
     * C day_time interval to SQL
     */

    {   /* DTL_ERRCODE_C_DT_INTERVAL_COLUMN_BYTE_LENGTH_LESS_CHARACTER_BYTE_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "column byte length is less than character byte length"
    },
    {   /* DTL_ERRCODE_C_DT_INTERVAL_NOT_VALID_INTERVAL_LITERAL */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INTERVAL_FIELD_OVERFLOW,
        "data value is not a valid interval literal"
    },
    {   /* DTL_ERRCODE_C_DT_INTERVAL_CONVERSION_TRUNCATION_WHOLE_DIGITS */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "conversion resulted in truncation of whole digits"
    },
    {   /* DTL_ERRCODE_C_DT_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INTERVAL_FIELD_OVERFLOW,
        "one or more fields of data value were truncated during conversion"
    },

    /**
     * SQL string to C
     */

    {   /* DTL_ERRCODE_SQL_STRING_BYTE_LENGTH_GREATER_BUFFER_LENGTH */
        STL_EXT_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATION_WARNING,
        "byte length of data greater equal than buffer length"
    },
    {   /* DTL_ERRCODE_SQL_STRING_CHARACTER_LENGTH_GREATER_BUFFER_LENGTH */
        STL_EXT_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATION_WARNING,
        "character length of data greater equal than buffer length"
    },
    {   /* DTL_ERRCODE_SQL_STRING_CONVERTED_TRUNCATION_FRACTIONAL_DIGITS */
        STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
        "data converted with truncation of fractional digits"
    },
    {   /* DTL_ERRCODE_SQL_STRING_CONVERSION_RESULT_LOSS_WHOLE_DIGITS */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "conversion of data would result in loss of whole digits"
    },
    {   /* DTL_ERRCODE_SQL_STRING_NOT_NUMERIC_LITERAL */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_CHARACTER_VALUE_FOR_CAST,
        "data is not numeric literal"
    },
    {   /* DTL_ERRCODE_SQL_STRING_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "data is outside the range of the data type to which the number is being converted"
    },
    {   /* DTL_ERRCODE_SQL_STRING_DATA_GREATER_0_LESS_2_NOT_EQUAL_1 */
        STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
        "data is greater than 0, less than 2, and not equal to 1"
    },
    {   /* DTL_ERRCODE_SQL_STRING_DATA_LESS_0_GREATER_OR_EQUAL_2 */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "data is less than 0 or greater than or equal to 2"
    },
    {   /* DTL_ERRCODE_SQL_STRING_VALID_TIMESTAMP_TIME_PORTION_NONZERO */
        STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
        "data value is a valid timestamp-value; time portion is nonzero"
    },
    {   /* DTL_ERRCODE_SQL_STRING_NOT_VALID_DATE_OR_TIMESTAMP */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_CHARACTER_VALUE_FOR_CAST,
        "data value is not a valid date-value or timestamp-value"
    },
    {   /* DTL_ERRCODE_SQL_STRING_VALID_TIMESTAMP_FRACTIONAL_SECONDS_NONZERO */
        STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
        "data value is a valid timestamp-value; fractional seconds portion is nonzero"
    },
    {   /* DTL_ERRCODE_SQL_STRING_NOT_VALID_TIME_OR_TIMESTAMP */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_CHARACTER_VALUE_FOR_CAST,
        "data value is not a valid time-value or timestamp-value"
    },
    {   /* DTL_ERRCODE_SQL_STRING_NOT_VALID_DATE_OR_TIME_OR_TIMESTAMP */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_CHARACTER_VALUE_FOR_CAST,
        "data value is not a valid date-value or time-value or timestamp-value"
    },
    {   /* DTL_ERRCODE_SQL_STRING_VALID_INTERVAL_TRUNCATION_SOME_TRAINLING_FIELDS */
        STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
        "data value is a valid interval value;truncation of one or more trailing fields"
    },
    {   /* DTL_ERRCODE_SQL_STRING_VALID_INTERVAL_LEADING_FIELD_PRECISION_LOST */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INTERVAL_FIELD_OVERFLOW,
        "data value is a valid interval; leading field significant precision is lost"
    },
    {   /* DTL_ERRCODE_SQL_STRING_NOT_VALID_INTERVAL */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_CHARACTER_VALUE_FOR_CAST,
        "data value is not a valid interval value"
    },

    /**
     * SQL numeric to C
     */

    {   /* DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_LESS_BUFFER_LENGTH */
        STL_EXT_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATION_WARNING,
        "number of whole digits is less than buffer length"
    },
    {   /* DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_GREATER_BUFFER_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "number of whole digits is greater than buffer length"
    },
    {   /* DTL_ERRCODE_SQL_NUMBER_CONVERTED_TRUNCATION_FRACTIONAL_DIGITS */
        STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
        "data converted with truncation of fractional digits"
    },
    {   /* DTL_ERRCODE_SQL_NUMBER_CONVERSION_RESULT_LOSS_WHOLE_DIGITS */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE ,
        "conversion of data would result in loss of whole digits"
    },
    {   /* DTL_ERRCODE_SQL_NUMBER_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE ,
        "data is outside the range of the data type to which the number is being converted"
    },
    {   /* DTL_ERRCODE_SQL_NUMBER_DATA_GREATER_0_LESS_2_NOT_EQUAL_1 */
        STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
        "data is greater than 0, less than 2, and not equal to 1"
    },
    {   /* DTL_ERRCODE_SQL_NUMBER_DATA_LESS_0_GREATER_OR_EQUAL_2 */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "data is less than 0 or greater than or equal to 2"
    },
    {   /* DTL_ERRCODE_SQL_NUMBER_BYTE_LENGTH_GREATER_BUFFER_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "byte length of data is greater than buffer length"
    },
    {   /* DTL_ERRCODE_SQL_NUMBER_FRACTIONAL_SECONDS_TRUNCATED */
        STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
        "fractional seconds portion truncated"
    },
    {   /* DTL_ERRCODE_SQL_NUMBER_WHOLE_NUMBER_TRUNCATED_INTERVAL */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INTERVAL_FIELD_OVERFLOW,
        "whole part of number truncated"
    },

    /**
     * SQL bit to C
     */

    {   /* DTL_ERRCODE_SQL_BIT_BUFFER_LENGTH_LESS_EQUAL_1 */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "buffer length is less equal than 1"
    },
    {   /* DTL_ERRCODE_SQL_BIT_BUFFER_LENGTH_LESS_1 */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "buffer length is less than 1"
    },

    /**
     * SQL binary to C
     */

    {   /* DTL_ERRCODE_SQL_BINARY_BYTE_LENGTH_DIVIDE_2_GREATER_EQUAL_BUFFER_LENGTH */
        STL_EXT_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATION_WARNING,
        "(byte length of data)/2 is greater equal than buffer length"
    },
    {   /* DTL_ERRCODE_SQL_BINARY_CHARACTER_LENGTH_DIVIDE_2_GREATER_EQUAL_BUFFER_LENGTH */
        STL_EXT_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATION_WARNING,
        "(character length of data)/2 is greater equal than buffer length"
    },
    {   /* DTL_ERRCODE_SQL_BINARY_BYTE_LENGTH_GREATER_BUFFER_LENGTH */
        STL_EXT_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATION_WARNING,
        "byte length of data is greater than buffer length"
    },

    /**
     * SQL date to C
     */
    
    {   /* DTL_ERRCODE_SQL_DATE_BUFFER_LENGTH_BETWEEN_DATE_DISPLAY_SIZE_CHARACTER_BYTE_LENGTH */
        STL_EXT_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATION_WARNING,
        "buffer length is between %d and character byte length"
    },
    {   /* DTL_ERRCODE_SQL_DATE_BUFFER_LENGTH_LESS_DATE_DISPLAY_SIZE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "buffer length is less than %d"
    },
    {   /* DTL_ERRCODE_SQL_DATE_BYTE_LENGTH_GREATER_BUFFER_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "byte length of data is greater than buffer length"
    },

    /**
     * SQL guid to C
     */

    {   /* DTL_ERRCODE_SQL_GUID_BUFFER_LENGTH_LESS_GUID_DISPLAY_SIZE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "buffer length is less than %d"
    },
    {   /* DTL_ERRCODE_SQL_GUID_BYTE_LENGTH_GREATER_BUFFER_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "byte length of data is greater than buffer length"
    },
    
    /**
     * SQL time to C
     */

    {   /* DTL_ERRCODE_SQL_TIME_BUFFER_LENGTH_BETWEEN_TIME_DISPLAY_SIZE_CHARACTER_BYTE_LENGTH */
        STL_EXT_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATION_WARNING,
        "buffer length is between %d and character byte length"
    },
    {   /* DTL_ERRCODE_SQL_TIME_BUFFER_LENGTH_LESS_TIME_DISPLAY_SIZE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "buffer length is less than %d"
    },
    {   /* DTL_ERRCODE_SQL_TIME_BYTE_LENGTH_GREATER_BUFFER_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "byte length of data is greater than buffer length"
    },

    /**
     * SQL timestamp to C
     */

    {   /* DTL_ERRCODE_SQL_TIMESTAMP_BUFFER_LENGTH_BETWEEN_TIMESTAMP_DISPALY_SIZE_CHARACTER_BYTE_LENGTH */
        STL_EXT_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATION_WARNING,
        "buffer length is between %d and character byte length"
    },
    {   /* DTL_ERRCODE_SQL_TIMESTAMP_BUFFER_LENGTH_LESS_TIMESTAMP_DISPLAY_SIZE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "buffer length is less than %d"
    },
    {   /* DTL_ERRCODE_SQL_TIMESTAMP_BUFFER_LENGTH_BETWEEN_TIMESTAMP_DISPLAY_SIZE_CHARACTER_LENGTH */
        STL_EXT_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATION_WARNING,
        "buffer length is between %d and character length"
    },
    {   /* DTL_ERRCODE_SQL_TIMESTAMP_BYTE_LENGTH_GREATER_BUFFER_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "byte length of data is greater than buffer length"
    },
    {   /* DTL_ERRCODE_SQL_TIMESTAMP_TIME_PORTION_NONZERO */
        STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
        "time portion of timestamp is nonzero"
    },
    {   /* DTL_ERRCODE_SQL_TIMESTAMP_FRACTIONAL_SECONDS_PORTION_NONZERO */
        STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
        "fractional seconds portion of timestamp is nonzero"
    },
    {   /* DTL_ERRCODE_SQL_TIMESTAMP_FRACTIONAL_SECONDS_PORTION_TRUNCATED */
        STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
        "fractional seconds portion of timestamp is truncated"
    },

    /**
     * SQL year-month interval to C
     */

    {   /* DTL_ERRCODE_SQL_YM_INTERVAL_TRAILING_FIELDS_TRUNCATED */
        STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
        "trailing fields portion truncated"
    },
    {   /* DTL_ERRCODE_SQL_YM_INTERVAL_LEADING_PRECISION_NOT_BIG_ENOUGH_TO_HOLD */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INTERVAL_FIELD_OVERFLOW,
        "leading precision of target is not big enough to hold data from source"
    },
    {   /* DTL_ERRCODE_SQL_YM_INTERVAL_SINGLE_FILED_TRUNCATED_WHOLE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "interval precision was a single field and truncated whole"
    },
    {   /* DTL_ERRCODE_SQL_YM_INTERVAL_PRECISION_NOT_SINGLE_FILED */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INTERVAL_FIELD_OVERFLOW,
        "interval precision was not a sigle field"
    },
    {   /* DTL_ERRCODE_SQL_YM_INTERVAL_BYTE_LENGTH_GREATER_BUFFER_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "byte length of data is greater than buffer length"
    },
    {   /* DTL_ERRCODE_SQL_YM_INTERVAL_WHOLE_DIGITS_LESS_BUFFER_LEGNTH */
        STL_EXT_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATION_WARNING,
        "number of whole digits is less than buffer length"
    },
    {   /* DTL_ERRCODE_SQL_YM_INTERVAL_WHOLE_DIGITS_GREATER_EQUAL_BUFFER_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "number of whole digits is greater equal than buffer length"
    },
    
    /**
     * SQL day-time interval to C
     */

    {   /* DTL_ERRCODE_SQL_DT_INTERVAL_TRAILING_FIELDS_TRUNCATED */
        STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
        "trailing fields portion truncated"
    },
    {   /* DTL_ERRCODE_SQL_DT_INTERVAL_LEADING_PRECISION_NOT_BIG_ENOUGH_TO_HOLD */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INTERVAL_FIELD_OVERFLOW,
        "leading precision of target is not big enough to hold data from source"
    },
    {   /* DTL_ERRCODE_SQL_DT_INTERVAL_SINGLE_FILED_TRUNCATED_FRACTIONAL */
        STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
        "interval precision was a single field and truncated fractional"
    },
    {   /* DTL_ERRCODE_SQL_DT_INTERVAL_SINGLE_FILED_TRUNCATED_WHOLE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "interval precision was a single field and truncated whole"
    },
    {   /* DTL_ERRCODE_SQL_DT_INTERVAL_PRECISION_NOT_SINGLE_FILED */
        STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION,
        "interval precision was not a sigle field"
    },
    {   /* DTL_ERRCODE_SQL_DT_INTERVAL_BYTE_LENGTH_GREATER_BUFFER_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "byte length of data is greater than buffer length"
    },
    {   /* DTL_ERRCODE_SQL_DT_INTERVAL_WHOLE_DIGITS_LESS_BUFFER_LEGNTH */
        STL_EXT_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATION_WARNING,
        "number of whole digits is less than buffer length"
    },
    {   /* DTL_ERRCODE_SQL_DT_INTERVAL_WHOLE_DIGITS_GREATER_EQUAL_BUFFER_LENGTH */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "number of whole digits is greater equal than buffer length"
    },

    /**
     * Other Error Codes
     */

    {   /* DTL_ERRCODE_INVALID_TIME_ZONE_DISPLACEMENT_VALUE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_TIME_ZONE_DISPLACEMENT_VALUE,
        "invalid time zone displacement value"
    },
    {   /* DTL_ERRCODE_INVALID_NUMBER_OF_ARGUMENTS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid number of arguments"
    },
    {   /* DTL_ERRCODE_INCONSISTENT_DATATYPE_FUNCTION_PARAMETERS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "inconsistent datatype of function parameters: %s"
    },
    {   /* DTL_ERRCODE_COMPARISON_NOT_APPLICABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "comparison is not applicable: %s and %s"
    },
    {   /* DTL_ERRCODE_CONVERSION_NOT_APPLICABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "conversion is not applicable: from %s to %s"
    },
    {   /* DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_CHARACTER_NOT_IN_REPERTOIRE,
        "invalid character value in characterset repertoire"
    },
    {   /* DTL_ERRCODE_NUMERIC_DIVISION_BY_ZERO */
        STL_EXT_ERRCODE_DATA_EXCEPTION_DIVISION_BY_ZERO,
        "divisor is equal to zero"
    },
    {   /* DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_BOOLEAN_LITERAL */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_CHARACTER_VALUE_FOR_CAST,
        "data is not boolean literal"
    },
    {   /* DTL_ERRCODE_INVALID_ARGUMENT_FOR_FACTORIAL_FUNCTION */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid argument for factorial function"
    },
    {   /* DTL_ERRCODE_TRIM_SET_SHOULD_ONLY_ONE_CHARACTER */
        STL_EXT_ERRCODE_DATA_EXCEPTION_TRIM_ERROR,
        "trim set should have only one character"
    },
    {   /* DTL_ERRCODE_DATETIME_FIELD_OVERFLOW */
        STL_EXT_ERRCODE_DATA_EXCEPTION_DATETIME_FIELD_OVERFLOW,
        "datetime field overflow"
    },
    {   /* DTL_ERRCODE_INVALID_ROWID */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid ROWID"
    },
    {   /* DTL_ERRCODE_INVALID_ESCAPE_CHARACTER */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_ESCAPE_CHARACTER,
        "invalid escape character"
    },
    {   /* DTL_ERRCODE_INVALID_CHARACTER_SET_IDENTIFIER */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid character set identifier"
    },
    {   /* DTL_ERRCODE_C_INVALID_EXTRACT_FIELD_FOR_EXTRACT_SOURCE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid extract field for extract source"
    },

    {   /* DTL_ERRCODE_INCONSISTENT_DATATYPES */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "inconsistent datatypes: argument[%d] expected %s"
    },
    
    {
        /* DTL_ERRCODE_DATE_FORMAT_LENGTH */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "date format value exceeds maximum length (%d)"
    },
    
    {
        /* DTL_ERRCODE_DATE_FORMAT_NOT_RECOGNIZED */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "date format not recognized"
    },

    {
        /* DTL_ERRCODE_FORMAT_CODE_APPEARS_TWICE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "format code appears twice"
    },

    {
        /* DTL_ERRCODE_DATE_FORMAT_CODE_CONFLICT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "%s conflicts with %s"
    },

    {
        /* DTL_ERRCODE_DATE_FORMAT_NOT_MATCH_FORMAT_STRING */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "literal does not match format string"
    },

    {
        /* DTL_ERRCODE_DATE_FORMAT_NOT_LONG_ENOUGH_INPUT_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "input value not long enough for date format"
    },

    {
        /* DTL_ERRCODE_DATE_FORMAT_NOT_LONG_ENOUGH_FORMAT_PICTURE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "date format picture ends before converting entire input string"
    },

    {
        /* DTL_ERRCODE_DATE_INVALID_YEAR_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "year must be between %s and %s, and not be 0"
    },            

    {
        /* DTL_ERRCODE_INVALID_LIKE_PATTERN */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_USE_OF_ESCAPE_CHARACTER,
        "missing or illegal character following the escape character"
    },

    {
        /* DTL_ERRCODE_DATE_INVALID_MONTH_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "not a valid month"
    },

    {
        /* DTL_ERRCODE_DATE_NON_NUMERIC_CHARACTER */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "a non-numeric character was found where a numeric was expected"
    },                

    {
        /* DTL_ERRCODE_DATE_INVALID_LAST_DAY_OF_MONTH_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "day of month must be between 1 and last day of month"
    },                

    {
        /* DTL_ERRCODE_DATE_INVALID_DAY_OF_WEEK */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "not a valid day of the week"
    },                

    {
        /* DTL_ERRCODE_DATE_INVALID_DAY_OF_YEAR */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "day of year must be between 1 and 365 (366 for leap year)"
    },

    {
        /* DTL_ERRCODE_CANNOT_APPEAR_IN_DATE_INPUT_FORMAT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "format code cannot appear in date input format"
    },                    

    {
        /* DTL_ERRCODE_DATE_REQUIRED_AD_BC_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "BC/B.C. or AD/A.D. required"
    },

    {
        /* DTL_ERRCODE_DATE_INVALID_JULIAN_DATE_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "julian date must be between 0 and 5373484"
    },                        

    {
        /* DTL_ERRCODE_DATE_FORMAT_SPECIFIED_ONCE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "%s may only be specified once"
    },                        

    {
        /* DTL_ERRCODE_DATE_FORMAT_JULIAN_DATE_PRECLUDES_USE_OF_DAY_OF_YEAR */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Julian date precludes use of day of year"
    },                        

    {
        /* DTL_ERRCODE_NOT_ENOUGH_FORMAT_INFO_BUFFER */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Not enough format info buffer"
    },

    {
        /* DTL_ERRCODE_TIME_INVALID_HOUR_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "hour must be between %d and %d"
    },

    {
        /* DTL_ERRCODE_TIME_INVALID_MINUTE_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "minute must be between 0 and 59"
    },

    {
        /* DTL_ERRCODE_TIME_INVALID_SECOND_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "second must be between 0 and 59"
    },

    {
        /* DTL_ERRCODE_TIME_INVALID_FRACTIONAL_SECOND_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "the fractional seconds must be between 0 and 999999"
    },

    {
        /* DTL_ERRCODE_TIME_INVALID_TIMEZONE_HOUR_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "time zone hour must be between -14 and 14"
    },

    {
        /* DTL_ERRCODE_TIME_INVALID_TIMEZONE_MINUTE_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "time zone minute must be between 0 and 59"
    },

    {
        /* DTL_ERRCODE_TIME_INVALID_SECONDS_IN_DAY_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "seconds in day must be between 0 and 86399"
    },

    {
        /* DTL_ERRCODE_TIME_REQUIRED_AM_PM_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "AM/A.M. or PM/P.M. required"
    },

    {
        /* DTL_ERRCODE_DATE_FORMAT_HH24_PRECLUDES_USE_OF_MERIDIAN_INDICATOR */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "HH24 precludes use of meridian indicator"
    },

    {
        /* DTL_ERRCODE_DATE_FORMAT_CODE_CONFLICT_WITH_USE_OF */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "%s conflicts with use of %s"
    },
    
    {
        /* DTL_ERRCODE_INVALID_XID_STRING */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid XID string"
    },

    {
        /* DTL_ERRCODE_DATE_NOT_VALID_FOR_MONTH_SPECIFIED */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "date not valid for month specified"
    },

    {
        /* DTL_ERRCODE_DATE_FORMAT_SIGNED_YEAR_PRECLUDES_USE_OF_BC_AD */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "signed year precludes use of BC/AD"
    },

    {
        /* DTL_ERRCODE_NUMBER_FORMAT_LENGTH */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "number format value exceeds maximum length (%d)"
    },

    {
        /* DTL_ERRCODE_INVALID_NUMBER_FORMAT_MODEL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid number format model"
    },

    {
        /* DTL_ERRCODE_NUMBER_FORMAT_CANNOT_USE_TWICE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot use %s twice"
    },

    {
        /* DTL_ERRCODE_NUMBER_FORMAT_CANNOT_USE_TOGETHER */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot use %s and %s together"
    },

    {
        /* DTL_ERRCODE_CHARACTER_STRING_DATA_RIGHT_TRUNCATION */
        STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION,
        "string data, right truncation"
    },

    {
        /* DTL_ERRCODE_ARGUMENT_OUT_OF_RANGE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "argument '%d' is out of range"
    },

    {
        /* DTL_ERRCODE_INVALID_DATEPART_FOR_DATA_TYPE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid datepart for data type %s"
    },

    {   /* DTL_ERRCODE_MAXIMUM_NUMBER_OF_ARGUMENTS_EXCEEDED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "maximum number of arguments exceeded"
    },

    {   /* DTL_ERRCODE_INVALID_CHAR_LENGTH_UNITS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid char length units(%s): use OCTETS or CHARACTERS"
    },
    
    {   /* DTL_ERRCODE_INVALID_ARGUMENT_VALUE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "The argument of %s function is invalid"
    },

    {   /* DTL_ERRCODE_INVALID_NTH_ARGUMENT_VALUE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "The argument [%d] of %s function is invalid"
    },
    {   /* DTL_ERRCODE_INTERNAL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "internal error, in a function %s"
    },
    {   /* DTL_ERRCODE_NOT_SUPPORTED_DATA_TYPE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "not supported data type ( %s )"
    },
    {   /* DTL_ERRCODE_DATA_TYPE_INVALID */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "data type specifier is out of range ( type : %d, range : %d to %d )"
    },
    {   /* DTL_ERRCODE_PRECISION_INVALID */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "precision specifier is out of range( type : %s, precision : %d, range : %d to %d )"
    },
    {   /* DTL_ERRCODE_LEADING_PRECISION_INVALID */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "leading precision specifier is out of range( type : %s, precision : %d, range : %d to %d )"
    },
    {   /* DTL_ERRCODE_SCALE_INVALID */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "scale specifier is out of range( type : %s, scale : %d, range : %d to %d )"
    },
    {   /* DTL_ERRCODE_FRACTIONAL_SECOND_PRECISION_INVALID */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fractional second precision specifier is out of range( type : %s, precision : %d, range : %d to %d )"
    },
    {   /* DTL_ERRCODE_CHARACTER_LENGTH_UNIT_INVALID */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "character length unit specifier is out of range( type : %s, character length unit : %d, range : %d to %d )"
    },
    {   /* DTL_ERRCODE_INTERVAL_INDICATOR_INVALID */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "interval indicator specifier is out of range( type : %s, indicator : %d, range : %d to %d )"
    },

    {
        0,
        NULL
    }
};

/** @} */


