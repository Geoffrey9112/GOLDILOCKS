/*******************************************************************************
 * dtlDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISIONS
 *        $Id$
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _DTLDEF_H_
#define _DTLDEF_H_ 1

/**
 * @file dtlDef.h
 * @brief DataType Layer Definitions
 */

/**
 * @addtogroup dtExternal
 * @{
 */

/** @} dtExternal */


/**
 * @defgroup dtlGeneral General
 * @ingroup dtExternal
 * @{
 */

/** @} dtlGeneral */



/**
 * @defgroup dtlError Error
 * @ingroup dtExternal
 * @{
 */

/**
 * @brief failed to initialize datatype layer
 */
#define DTL_ERRCODE_INITIALIZE                          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 0 )

/**
 * @brief not implemented feature, in a function %s
 */
#define DTL_ERRCODE_NOT_IMPLEMENTED                     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 1 )

/**********************
 * C character to SQL
 **********************/

/**
 * @brief byte length of data greater than column length
 */
#define DTL_ERRCODE_C_STRING_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 2 )

/**
 * @brief character length of data greater than column length
 */   
#define DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 3 )

/**
 * @brief data converted with truncation of fractional digits
 */
#define DTL_ERRCODE_C_STRING_CONVERTED_TRUNCATION_FRACTIONAL_DIGITS     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 4 )
    
/**
 * @brief conversion of data would result in loss of whole digits
 */
#define DTL_ERRCODE_C_STRING_CONVERSION_DATA_LOSS_WHOLE_DIGITS  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 5 )
    
/**
 * @brief data value is not a numeric literal
 */
#define DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_NUMERIC_LITERAL     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 6 )
    
/**
 * @brief data is outside the range of the data type to which the number is being converted
 */
#define DTL_ERRCODE_C_STRING_DATA_OUTSIDE_RANGE_NUMBER_CONVERTED        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 7 )
    
/**
 * @brief data is greater than 0, less than 2, and not equal to 1
 */
#define DTL_ERRCODE_C_STRING_DATA_GREATER_0_LESS_2_NOT_EQUAL_1  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 8 )
    
/**
 * @brief data is less than 0 or greater than or equal to 2
 */
#define DTL_ERRCODE_C_STRING_DATA_LESS_0_GREATER_OR_EQUAL_2     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 9 )
    
/**
 * @brief (byte length of data)/2 is greater than column byte length
 */
#define DTL_ERRCODE_C_STRING_BYTE_LENGTH_DIVIDE_2_GREATER_COLUMN_BYTE_LENGTH    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 10 )
    
/**
 * @brief data value is not a hexadecimal value
 */
#define DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_HEXADECIMAL_VALUE   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 11 )

/**
 * @brief data value is a valid timestamp literal; time portion is nonzero
 */
#define DTL_ERRCODE_C_STRING_VALID_TIMESTAMP_TIME_NONZERO       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 12 )

/**
 * @brief data value is not a valid date literal or timestamp literal
 */
#define DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIMESTAMP_LITERAL        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 13 )

/**
 * @brief data value is a valid timestamp literal; fractional seconds portion is nonzero
 */
#define DTL_ERRCODE_C_STRING_VALID_TIMESTAMP_FRACTIONAL_SECONDS_NONZERO \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 14 )

/**
 * @brief data value is not a valid time literal or timestamp literal
 */
#define DTL_ERRCODE_C_STRING_NOT_VALID_TIME_OR_TIMESTAMP_LITERAL        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 15 )

/**
 * @brief data value is a valid timestamp literal; fractional seconds portion truncated
 */
#define DTL_ERRCODE_C_STRING_VALID_TIMESTAMP_FRACTIONAL_SECONDS_TRUNCATED       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 16 )

/**
 * @brief data value is not a valid date literal or time literal or timestamp literal
 */
#define DTL_ERRCODE_C_STRING_NOT_VALID_DATE_OR_TIME_OR_TIMESTAMP_LITERAL        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 17 )

/**
 * @brief data value is a valid interval value; the value in one of the fields is truncated
 */
#define DTL_ERRCODE_C_STRING_INTERVAL_FIELD_TRUNCATED   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 18 )
    
/**
 * @brief data value is not a valid interval literal
 */
#define DTL_ERRCODE_C_STRING_NOT_VALID_INTERVAL_LITERAL \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 19 )

/**********************
 * C numeric to SQL
 **********************/

/**
 * @brief number of digits greater than column byte length
 */
#define DTL_ERRCODE_C_NUMERIC_NUMBER_DIGITS_GREATER_COLUMN_BYTE_LENGTH  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 20 )
    
/**
 * @brief number of characters greater than column byte length
 */
#define DTL_ERRCODE_C_NUMERIC_NUMBER_CHARACTERS_GREATER_COLUMN_CHARACTER_LENGTH \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 21 )
 
/**
 * @brief data converted with truncation of whole digits
 */
#define DTL_ERRCODE_C_NUMERIC_DATA_CONVERTED_TRUNCATION_WHOLE_DIGITS    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 22 )
 
/**
 * @brief data is outside the range of the data type to which the number is being converted
 */
#define DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 23 )
    
/**
 * @brief data is greater than 0, less than 2, and not equal to 1
 */
#define DTL_ERRCODE_C_NUMERIC_DATA_GREATER_0_LESS_2_NOT_EQUAL_1 \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 24 )
    
/**
 * @brief data is less than 0 or greater than or equal to 2
 */
#define DTL_ERRCODE_C_NUMERIC_DATA_LESS_0_GREATER_OR_EQUAL_2    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 25 )
    
/**
 * @brief interval data truncated
 */
#define DTL_ERRCODE_C_NUMERIC_INTERVAL_DATA_TRUNCATED   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 26 )
    

/**********************
 * C binary to SQL
 **********************/
    
/**
 * @brief byte length of data greater than column length
 */
#define DTL_ERRCODE_C_BINARY_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 27 )
    
/**
 * @brief character length of data greater than column length
 */
#define DTL_ERRCODE_C_BINARY_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 28 )
    
/**
 * @brief byte length of data not equal to SQL data length
 */
#define DTL_ERRCODE_C_BINARY_BYTE_LENGTH_NOT_EQUAL_SQL_DATA_LENGTH      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 29 )
    

/**********************
 * C date to SQL
 **********************/

/**
 * @brief column byte length is less than %d
 */
#define DTL_ERRCODE_C_DATE_COLUMN_BYTE_LENGTH_LESS_THAN_DATE_DISPLAY_SIZE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 30 )
 
/**
 * @brief data value is not a valid date
 */
#define DTL_ERRCODE_C_DATE_NOT_VALID_DATE               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 31 )
 
/**
 * @brief data value is not a valid date format
 */
#define DTL_ERRCODE_C_DATE_NOT_VALID_DATE_FORMAT        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 32 )
    

/**********************
 * C guid to SQL
 **********************/

/**
 * @brief column byte length is less than %d
 */
#define DTL_ERRCODE_C_GUID_COLUMN_BYTE_LENGTH_LESS_THAN_GUID_DISPLAY_SIZE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 33 )
    
/**
 * @brief data value is not a valid GUID
 */
#define DTL_ERRCODE_C_GUID_NOT_VALID_GUID               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 34 )
    
    
/**********************
 * C time to SQL
 **********************/

/**
 * @brief column byte length is less than %d
 */
#define DTL_ERRCODE_C_TIME_COLUMN_BYTE_LENGTH_LESS_THAN_TIME_DISPLAY_SIZE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 35 )
    
/**
 * @brief data value is not a valid time
 */
#define DTL_ERRCODE_C_TIME_NOT_VALID_TIME               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 36 )
    
/**
 * @brief data value is not a valid time format
 */
#define DTL_ERRCODE_C_TIME_NOT_VALID_TIME_FORMAT        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 37 )
 

/**********************
 * C timestamp to SQL
 **********************/

/**
 * @brief column byte length is between %d and character byte length
 */
#define DTL_ERRCODE_C_TIMESTAMP_COLUMN_BYTE_LENGTH_BETWEEN_TIMESTAMP_DISPLAY_SIZE_AND_CHAR_BYTE_LENGTH \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 38 )
    
/**
 * @brief column byte length is less than %d
 */
#define DTL_ERRCODE_C_TIMESTAMP_COLUMN_BYTE_LENGTH_LESS_TIMESTAMP_DISPLAY_SIZE  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 39 )
    
/**
 * @brief data value is not a valid timestamp
 */
#define DTL_ERRCODE_C_TIMESTAMP_NOT_VALID_TIME_FORMAT   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 40 )
 
/**
 * @brief time fields are nonzero
 */
#define DTL_ERRCODE_C_TIMESTAMP_TIME_FILEDS_NONZERO     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 41 )
    
/**
 * @brief data value does not contain a valid date
 */
#define DTL_ERRCODE_C_TIMESTAMP_NOT_CONTAIN_VALID_DATE  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 42 )
    
/**
 * @brief fractional seconds fields are nonzero
 */
#define DTL_ERRCODE_C_TIMESTAMP_FRACTIONAL_SECOND_NONZERO       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 43 )
    
/**
 * @brief data value does not contain a valid time
 */
#define DTL_ERRCODE_C_TIMESTAMP_NOT_CONTAIN_VALID_TIME  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 44 )
    
/**
 * @brief fractional seconds fields are truncated
 */
#define DTL_ERRCODE_C_TIMESTAMP_FRACTIONAL_SECOND_TRUNCATED     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 45 )
    
/**
 * @brief data value does not contain a valid timestamp
 */
#define DTL_ERRCODE_C_TIMESTAMP_NOT_CONTAIN_VALID_TIMESTAMP     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 46 )
    

/**********************
 * C year_month interval to SQL
 **********************/

/**
 * @brief column byte length is less than character byte length
 */
#define DTL_ERRCODE_C_YM_INTERVAL_COLUMN_BYTE_LENGTH_LESS_CHARACTER_BYTE_LENGTH \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 47 )
    
/**
 * @brief data value is not a valid interval literal
 */
#define DTL_ERRCODE_C_YM_INTERVAL_NOT_VALID_INTERVAL_LITERAL    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 48 )
    
/**
 * @brief conversion resulted in truncation of whole digits
 */
#define DTL_ERRCODE_C_YM_INTERVAL_CONVERSION_TRUNCATION_WHOLE_DIGITS    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 49 )
    
/**
 * @brief one or more fields of data value were truncated during conversion
 */
#define DTL_ERRCODE_C_YM_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 50 )
    

/**********************
 * C day_time interval to SQL
 **********************/

/**
 * @brief column byte length is less than character byte length
 */
#define DTL_ERRCODE_C_DT_INTERVAL_COLUMN_BYTE_LENGTH_LESS_CHARACTER_BYTE_LENGTH \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 51 )
    
/**
 * @brief data value is not a valid interval literal
 */
#define DTL_ERRCODE_C_DT_INTERVAL_NOT_VALID_INTERVAL_LITERAL    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 52 )
    
/**
 * @brief conversion resulted in truncation of whole digits
 */
#define DTL_ERRCODE_C_DT_INTERVAL_CONVERSION_TRUNCATION_WHOLE_DIGITS    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 53 )
    
/**
 * @brief one or more fields of data value were truncated during conversion
 */
#define DTL_ERRCODE_C_DT_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 54 )
    

/**********************
 * SQL string to C
 **********************/

/**
 * @brief byte length of data greater equal than buffer length
 */
#define DTL_ERRCODE_SQL_STRING_BYTE_LENGTH_GREATER_BUFFER_LENGTH        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 55 )
    
/**
 * @brief character length of data greater equal than buffer length
 */
#define DTL_ERRCODE_SQL_STRING_CHARACTER_LENGTH_GREATER_BUFFER_LENGTH   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 56 )
    
/**
 * @brief data converted with truncation of fractional digits
 */
#define DTL_ERRCODE_SQL_STRING_CONVERTED_TRUNCATION_FRACTIONAL_DIGITS   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 57 )
    
/**
 * @brief conversion of data would result in loss of whole digits
 */
#define DTL_ERRCODE_SQL_STRING_CONVERSION_RESULT_LOSS_WHOLE_DIGITS      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 58 )
    
/**
 * @brief data is not numeric literal
 */
#define DTL_ERRCODE_SQL_STRING_NOT_NUMERIC_LITERAL      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 59 )
    
/**
 * @brief data is outside the range of the data type to which the number is being converted
 */
#define DTL_ERRCODE_SQL_STRING_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 60 )
    
/**
 * @brief data is greater than 0, less than 2, and not equal to 1
 */
#define DTL_ERRCODE_SQL_STRING_DATA_GREATER_0_LESS_2_NOT_EQUAL_1        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 61 )
    
/**
 * @brief data is less than 0 or greater than or equal to 2
 */
#define DTL_ERRCODE_SQL_STRING_DATA_LESS_0_GREATER_OR_EQUAL_2   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 62 )
    
/**
 * @brief data value is a valid timestamp-value; time portion is nonzero
 */
#define DTL_ERRCODE_SQL_STRING_VALID_TIMESTAMP_TIME_PORTION_NONZERO     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 63 )
    
/**
 * @brief data value is not a valid date-value or timestamp-value
 */
#define DTL_ERRCODE_SQL_STRING_NOT_VALID_DATE_OR_TIMESTAMP      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 64 )
    
/**
 * @brief data value is a valid timestamp-value; fractional seconds portion is nonzero
 */
#define DTL_ERRCODE_SQL_STRING_VALID_TIMESTAMP_FRACTIONAL_SECONDS_NONZERO       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 65 )
    
/**
 * @brief data value is not a valid time-value or timestamp-value
 */
#define DTL_ERRCODE_SQL_STRING_NOT_VALID_TIME_OR_TIMESTAMP      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 66 )
    
/**
 * @brief data value is not a valid date-value or time-value or timestamp-value
 */
#define DTL_ERRCODE_SQL_STRING_NOT_VALID_DATE_OR_TIME_OR_TIMESTAMP      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 67 )
    
/**
 * @brief data value is a valid interval value;truncation of one or more trailing fields
 */
#define DTL_ERRCODE_SQL_STRING_VALID_INTERVAL_TRUNCATION_SOME_TRAINLING_FIELDS  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 68 )
 
/**
 * @brief data value is a valid interval; leading field significant precision is lost
 */
#define DTL_ERRCODE_SQL_STRING_VALID_INTERVAL_LEADING_FIELD_PRECISION_LOST      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 69 )
    
/**
 * @brief data value is not a valid interval value
 */
#define DTL_ERRCODE_SQL_STRING_NOT_VALID_INTERVAL       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 70 )
    

/**********************
 * SQL numeric to C
 **********************/

/**
 * @brief number of whole digits is less than buffer length
 */
#define DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_LESS_BUFFER_LENGTH  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 71 )
    
/**
 * @brief number of whole digits is greater than buffer length
 */
#define DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_GREATER_BUFFER_LENGTH       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 72 )
    
/**
 * @brief data converted with truncation of fractional digits
 */
#define DTL_ERRCODE_SQL_NUMBER_CONVERTED_TRUNCATION_FRACTIONAL_DIGITS   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 73 )
    
/**
 * @brief conversion of data would result in loss of whole digits
 */
#define DTL_ERRCODE_SQL_NUMBER_CONVERSION_RESULT_LOSS_WHOLE_DIGITS      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 74 )
    
/**
 * @brief data is outside the range of the data type to which the number is being converted
 */
#define DTL_ERRCODE_SQL_NUMBER_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 75 )
    
/**
 * @brief data is greater than 0, less than 2, and not equal to 1
 */
#define DTL_ERRCODE_SQL_NUMBER_DATA_GREATER_0_LESS_2_NOT_EQUAL_1        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 76 )
    
/**
 * @brief data is less than 0 or greater than or equal to 2
 */
#define DTL_ERRCODE_SQL_NUMBER_DATA_LESS_0_GREATER_OR_EQUAL_2   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 77 )
    
/**
 * @brief byte length of data is greater than buffer length
 */
#define DTL_ERRCODE_SQL_NUMBER_BYTE_LENGTH_GREATER_BUFFER_LENGTH        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 78 )
    
/**
 * @brief fractional seconds portion truncated
 */
#define DTL_ERRCODE_SQL_NUMBER_FRACTIONAL_SECONDS_TRUNCATED     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 79 )
    
/**
 * @brief whole part of number truncated
 */
#define DTL_ERRCODE_SQL_NUMBER_WHOLE_NUMBER_TRUNCATED_INTERVAL  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 80 )
    

/**********************
 * SQL bit to C
 **********************/

/**
 * @brief buffer length is less equal than 1
 */
#define DTL_ERRCODE_SQL_BIT_BUFFER_LENGTH_LESS_EQUAL_1  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 81 )
    
/**
 * @brief buffer length is less than 1
 */
#define DTL_ERRCODE_SQL_BIT_BUFFER_LENGTH_LESS_1        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 82 )
    

/**********************
 * SQL binary to C
 **********************/

/**
 * @brief (byte length of data)/2 is greater equal than buffer length
 */
#define DTL_ERRCODE_SQL_BINARY_BYTE_LENGTH_DIVIDE_2_GREATER_EQUAL_BUFFER_LENGTH \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 83 )
    
/**
 * @brief (character length of data)/2 is greater equal than buffer length
 */
#define DTL_ERRCODE_SQL_BINARY_CHARACTER_LENGTH_DIVIDE_2_GREATER_EQUAL_BUFFER_LENGTH    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 84 )
    
/**
 * @brief byte length of data is greater than buffer length
 */
#define DTL_ERRCODE_SQL_BINARY_BYTE_LENGTH_GREATER_BUFFER_LENGTH        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 85 )
    

/**********************
 * SQL date to C
 **********************/
    
/**
 * @brief buffer length is between %d and character byte length
 */
#define DTL_ERRCODE_SQL_DATE_BUFFER_LENGTH_BETWEEN_DATE_DISPLAY_SIZE_CHARACTER_BYTE_LENGTH      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 86 )
    
/**
 * @brief buffer length is less than %d
 */
#define DTL_ERRCODE_SQL_DATE_BUFFER_LENGTH_LESS_DATE_DISPLAY_SIZE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 87 )
    
/**
 * @brief byte length of data is greater than buffer length
 */
#define DTL_ERRCODE_SQL_DATE_BYTE_LENGTH_GREATER_BUFFER_LENGTH  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 88 )
    

/**********************
 * SQL guid to C
 **********************/

/**
 * @brief buffer length is less than %d
 */
#define DTL_ERRCODE_SQL_GUID_BUFFER_LENGTH_LESS_GUID_DISPLAY_SIZE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 89 )
    
/**
 * @brief byte length of data is greater than buffer length
 */
#define DTL_ERRCODE_SQL_GUID_BYTE_LENGTH_GREATER_BUFFER_LENGTH  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 90 )
    
    
/**********************
 * SQL time to C
 **********************/

/**
 * @brief buffer length is between %d and character byte length
 */
#define DTL_ERRCODE_SQL_TIME_BUFFER_LENGTH_BETWEEN_TIME_DISPLAY_SIZE_CHARACTER_BYTE_LENGTH      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 91 )
    
/**
 * @brief buffer length is less than %d
 */
#define DTL_ERRCODE_SQL_TIME_BUFFER_LENGTH_LESS_TIME_DISPLAY_SIZE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 92 )
    
/**
 * @brief byte length of data is greater than buffer length
 */
#define DTL_ERRCODE_SQL_TIME_BYTE_LENGTH_GREATER_BUFFER_LENGTH  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 93 )
    

/**********************
 * SQL timestamp to C
 **********************/

/**
 * @brief buffer length is between %d and character byte length
 */
#define DTL_ERRCODE_SQL_TIMESTAMP_BUFFER_LENGTH_BETWEEN_TIMESTAMP_DISPALY_SIZE_CHARACTER_BYTE_LENGTH \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 94 )
    
/**
 * @brief buffer length is less than %d
 */
#define DTL_ERRCODE_SQL_TIMESTAMP_BUFFER_LENGTH_LESS_TIMESTAMP_DISPLAY_SIZE     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 95 )
    
/**
 * @brief buffer length is between %d and character length
 */
#define DTL_ERRCODE_SQL_TIMESTAMP_BUFFER_LENGTH_BETWEEN_TIMESTAMP_DISPLAY_SIZE_CHARACTER_LENGTH \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 96 ) 
    
/**
 * @brief byte length of data is greater than buffer length
 */
#define DTL_ERRCODE_SQL_TIMESTAMP_BYTE_LENGTH_GREATER_BUFFER_LENGTH     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 97 )
    
/**
 * @brief time portion of timestamp is nonzero
 */
#define DTL_ERRCODE_SQL_TIMESTAMP_TIME_PORTION_NONZERO  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 98 )
    
/**
 * @brief fractional seconds portion of timestamp is nonzero
 */
#define DTL_ERRCODE_SQL_TIMESTAMP_FRACTIONAL_SECONDS_PORTION_NONZERO    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 99 )
    
/**
 * @brief fractional seconds portion of timestamp is truncated
 */
#define DTL_ERRCODE_SQL_TIMESTAMP_FRACTIONAL_SECONDS_PORTION_TRUNCATED  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 100 )
    

/**********************
 * SQL year-month interval to C
 **********************/

/**
 * @brief trailing fields portion truncated
 */
#define DTL_ERRCODE_SQL_YM_INTERVAL_TRAILING_FIELDS_TRUNCATED   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 101 )
    
/**
 * @brief leading precision of target is not big enough to hold data from source
 */
#define DTL_ERRCODE_SQL_YM_INTERVAL_LEADING_PRECISION_NOT_BIG_ENOUGH_TO_HOLD    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 102 )
    
/**
 * @brief interval precision was a single field and truncated whole
 */
#define DTL_ERRCODE_SQL_YM_INTERVAL_SINGLE_FILED_TRUNCATED_WHOLE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 103 )
    
/**
 * @brief interval precision was not a sigle field
 */
#define DTL_ERRCODE_SQL_YM_INTERVAL_PRECISION_NOT_SINGLE_FILED  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 104 )
    
/**
 * @brief byte length of data is greater than buffer length
 */
#define DTL_ERRCODE_SQL_YM_INTERVAL_BYTE_LENGTH_GREATER_BUFFER_LENGTH   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 105 )
    
/**
 * @brief number of whole digits is less than buffer length
 */
#define DTL_ERRCODE_SQL_YM_INTERVAL_WHOLE_DIGITS_LESS_BUFFER_LEGNTH     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 106 )
    
/**
 * @brief number of whole digits is greater equal than buffer length
 */
#define DTL_ERRCODE_SQL_YM_INTERVAL_WHOLE_DIGITS_GREATER_EQUAL_BUFFER_LENGTH    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 107 )
    
    
/**********************
 * SQL day-time interval to C
 **********************/

/**
 * @brief trailing fields portion truncated
 */
#define DTL_ERRCODE_SQL_DT_INTERVAL_TRAILING_FIELDS_TRUNCATED   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 108 )
    
/**
 * @brief leading precision of target is not big enough to hold data from source
 */
#define DTL_ERRCODE_SQL_DT_INTERVAL_LEADING_PRECISION_NOT_BIG_ENOUGH_TO_HOLD    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 109 )
    
/**
 * @brief interval precision was a single field and truncated fractional
 */
#define DTL_ERRCODE_SQL_DT_INTERVAL_SINGLE_FILED_TRUNCATED_FRACTIONAL   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 110 )
    
/**
 * @brief interval precision was a single field and truncated whole
 */
#define DTL_ERRCODE_SQL_DT_INTERVAL_SINGLE_FILED_TRUNCATED_WHOLE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 111 )
    
/**
 * @brief interval precision was not a sigle field
 */
#define DTL_ERRCODE_SQL_DT_INTERVAL_PRECISION_NOT_SINGLE_FILED  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 112 )
    
/**
 * @brief byte length of data is greater than buffer length
 */
#define DTL_ERRCODE_SQL_DT_INTERVAL_BYTE_LENGTH_GREATER_BUFFER_LENGTH   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 113 )
    
/**
 * @brief number of whole digits is less than buffer length
 */
#define DTL_ERRCODE_SQL_DT_INTERVAL_WHOLE_DIGITS_LESS_BUFFER_LEGNTH     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 114 )
    
/**
 * @brief number of whole digits is greater equal than buffer length
 */
#define DTL_ERRCODE_SQL_DT_INTERVAL_WHOLE_DIGITS_GREATER_EQUAL_BUFFER_LENGTH    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 115 )
   
/**********************
 * other error codes
 **********************/

/**
 * @brief invalid time zone displacement value
 */
#define DTL_ERRCODE_INVALID_TIME_ZONE_DISPLACEMENT_VALUE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 116 )

/**
 * @brief invalid number of arguments
 */
#define DTL_ERRCODE_INVALID_NUMBER_OF_ARGUMENTS         \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 117 )

/**
 * @brief inconsistent datatype of function parameters: %s
 */
#define DTL_ERRCODE_INCONSISTENT_DATATYPE_FUNCTION_PARAMETERS   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 118 )

/**
 * @brief comparison is not applicable: %s and %s
 */
#define DTL_ERRCODE_COMPARISON_NOT_APPLICABLE           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 119 )

/**
 * @brief conversion is not applicable: from %s to %s
 */
#define DTL_ERRCODE_CONVERSION_NOT_APPLICABLE           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 120 )

/**
 * @brief invalid character value in characterset repertoire
 */
#define DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 121 )

/**
 * @brief divisor is equal to zero
 */
#define DTL_ERRCODE_NUMERIC_DIVISION_BY_ZERO            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 122 )


/**
 * @todo 에러코드 제자리에 넣기. ( C character to SQL )
 * @brief data is not boolean literal  
 */
#define DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_BOOLEAN_LITERAL     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 123 )

#define DTL_ERRCODE_INVALID_ARGUMENT_FOR_FACTORIAL_FUNCTION     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 124 )

/**
 * @brief trim set should have only one character
 */
#define DTL_ERRCODE_TRIM_SET_SHOULD_ONLY_ONE_CHARACTER  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 125 )

/**
 * @brief datetime field overflow
 */
#define DTL_ERRCODE_DATETIME_FIELD_OVERFLOW             \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 126 )

/**
 * @brief invalid ROWID
 */
#define DTL_ERRCODE_INVALID_ROWID                       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 127)   \

/**
 * @brief invalid escape character
 */
#define DTL_ERRCODE_INVALID_ESCAPE_CHARACTER            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 128)   \

/**
 * @brief invalid character set identifier
 */
#define DTL_ERRCODE_INVALID_CHARACTER_SET_IDENTIFIER    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 129)   \

/**
 * @brief invalid extract field for extract source
 */
#define DTL_ERRCODE_C_INVALID_EXTRACT_FIELD_FOR_EXTRACT_SOURCE  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 130)

/**
 * @brief inconsistent datatypes : expected %s
 */
#define DTL_ERRCODE_INCONSISTENT_DATATYPES              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 131)

/**
 * @brief date format value exceeds maximum length (%d)
 */
#define DTL_ERRCODE_DATE_FORMAT_LENGTH                  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 132)

/**
 * @brief date format not recognized
 */
#define DTL_ERRCODE_DATE_FORMAT_NOT_RECOGNIZED          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 133)

/**
 * @brief format code appears twice
 */
#define DTL_ERRCODE_FORMAT_CODE_APPEARS_TWICE           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 134)

/**
 * @brief %s conflicts with %s
 */
#define DTL_ERRCODE_DATE_FORMAT_CODE_CONFLICT           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 135)

/**
 * @brief literal does not match format string
 */
#define DTL_ERRCODE_DATE_FORMAT_NOT_MATCH_FORMAT_STRING \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 136)

/**
 * @brief input value not long enough for date format
 */
#define DTL_ERRCODE_DATE_FORMAT_NOT_LONG_ENOUGH_INPUT_VALUE     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 137)

/**
 * @brief date format picture ends before converting entire input string
 */
#define DTL_ERRCODE_DATE_FORMAT_NOT_LONG_ENOUGH_FORMAT_PICTURE  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 138)

/**
 * @brief year must be between %s and %s, and not be 0
 */
#define DTL_ERRCODE_DATE_INVALID_YEAR_VALUE             \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 139)

/** 
 * @brief invalid escape character
 */     
#define DTL_ERRCODE_INVALID_LIKE_PATTERN                \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 140)
/**
 * @brief not a valid month
 */
#define DTL_ERRCODE_DATE_INVALID_MONTH_VALUE            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 141)

/**
 * @brief a non-numeric character was found where a numeric was expected
 */
#define DTL_ERRCODE_DATE_NON_NUMERIC_CHARACTER          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 142)

/**
 * @brief day of month must be between 1 and last day of month
 */
#define DTL_ERRCODE_DATE_INVALID_LAST_DAY_OF_MONTH_VALUE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 143)

/**
 * @brief not a valid day of the week
 */
#define DTL_ERRCODE_DATE_INVALID_DAY_OF_WEEK            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 144)

/**
 * @brief day of year must be between 1 and 365 (366 for leap year)
 */
#define DTL_ERRCODE_DATE_INVALID_DAY_OF_YEAR            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 145)

/**
 * @brief format code cannot appear in date input format
 */
#define DTL_ERRCODE_CANNOT_APPEAR_IN_DATE_INPUT_FORMAT  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 146)

/**
 * @brief BC/B.C. or AD/A.D. required
 */
#define DTL_ERRCODE_DATE_REQUIRED_AD_BC_VALUE           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 147)

/**
 * @brief julian date must be between 0 and 5373484
 */
#define DTL_ERRCODE_DATE_INVALID_JULIAN_DATE_VALUE      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 148)

/**
 * @brief %s may only be specified once
 */
#define DTL_ERRCODE_DATE_FORMAT_SPECIFIED_ONCE          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 149)

/**
 * @brief Julian date precludes use of day of year
 */
#define DTL_ERRCODE_DATE_FORMAT_JULIAN_DATE_PRECLUDES_USE_OF_DAY_OF_YEAR        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 150)

/**
 * @brief Not enough format info buffer
 */
#define DTL_ERRCODE_NOT_ENOUGH_FORMAT_INFO_BUFFER       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 151)

/**
 * @brief hour must be between % and %
 */
#define DTL_ERRCODE_TIME_INVALID_HOUR_VALUE             \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 152)

/**
 * @brief minute must be between 0 and 59
 */
#define DTL_ERRCODE_TIME_INVALID_MINUTE_VALUE           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 153)

/**
 * @brief second must be between 0 and 59
 */
#define DTL_ERRCODE_TIME_INVALID_SECOND_VALUE           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 154)

/**
 * @brief the fractional seconds must be between 0 and 999999
 */
#define DTL_ERRCODE_TIME_INVALID_FRACTIONAL_SECOND_VALUE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 155)

/**
 * @brief time zone hour must be between -14 and 14
 */
#define DTL_ERRCODE_TIME_INVALID_TIMEZONE_HOUR_VALUE    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 156)

/**
 * @brief time zone minute must be between 0 and 59
 */
#define DTL_ERRCODE_TIME_INVALID_TIMEZONE_MINUTE_VALUE  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 157)

/**
 * @brief seconds in day must be between 0 and 86399
 */
#define DTL_ERRCODE_TIME_INVALID_SECONDS_IN_DAY_VALUE   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 158)

/**
 * @brief AM/A.M. or PM/P.M. required
 */
#define DTL_ERRCODE_TIME_REQUIRED_AM_PM_VALUE           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 159)

/**
 * @brief HH24 precludes use of meridian indicator
 */
#define DTL_ERRCODE_DATE_FORMAT_HH24_PRECLUDES_USE_OF_MERIDIAN_INDICATOR        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 160)

/**
 * @brief %s conflicts with use of %s
 */
#define DTL_ERRCODE_DATE_FORMAT_CODE_CONFLICT_WITH_USE_OF       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 161)

/**
 * @brief invalid XID string
 */
#define DTL_ERRCODE_INVALID_XID_STRING                  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 162)

/**
 * @brief date not valid for month specified
 */
#define DTL_ERRCODE_DATE_NOT_VALID_FOR_MONTH_SPECIFIED  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 163)

/**
 * @brief signed year precludes use of BC/AD
 */
#define DTL_ERRCODE_DATE_FORMAT_SIGNED_YEAR_PRECLUDES_USE_OF_BC_AD      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 164)

/**
 * @brief number format value exceeds maximum length (%d)
 */
#define DTL_ERRCODE_NUMBER_FORMAT_LENGTH                \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 165)

/**
 * @brief invalid number format model
 */
#define DTL_ERRCODE_INVALID_NUMBER_FORMAT_MODEL         \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 166)

/**
 * @brief cannot use %s twice
 */
#define DTL_ERRCODE_NUMBER_FORMAT_CANNOT_USE_TWICE      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 167)

/**
 * @brief cannot use %s and %s together
 */
#define DTL_ERRCODE_NUMBER_FORMAT_CANNOT_USE_TOGETHER   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 168)

/**
 * @brief string data, right truncation
 */
#define DTL_ERRCODE_CHARACTER_STRING_DATA_RIGHT_TRUNCATION      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 169)

/**
 * @brief argument '%d' is out of range 
 */
#define DTL_ERRCODE_ARGUMENT_OUT_OF_RANGE               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 170)

/**
 * @brief invalid datepart for data type %s.
 */
#define DTL_ERRCODE_INVALID_DATEPART_FOR_DATA_TYPE      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 171)

/**
 * @brief maximum number of arguments exceeded
 */
#define DTL_ERRCODE_MAXIMUM_NUMBER_OF_ARGUMENTS_EXCEEDED        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 172)

/**
 * @brief invalid char length units(%s): use OCTETS or CHARACTERS
 */
#define DTL_ERRCODE_INVALID_CHAR_LENGTH_UNITS           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 173)

/**
 * @brief The argument of %s function is invalid
 */
#define DTL_ERRCODE_INVALID_ARGUMENT_VALUE              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 174)

/**
 * @brief The argument [%d] of %s function is invalid
 */
#define DTL_ERRCODE_INVALID_NTH_ARGUMENT_VALUE          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 175)

/**
 * @brief internal error, in a function %s
 */
#define DTL_ERRCODE_INTERNAL                            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 176)

/**
 * @brief not supported data type ( %s )
 */
#define DTL_ERRCODE_NOT_SUPPORTED_DATA_TYPE             \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 177)

/**
 * @brief data type specifier is out of range( type : %d, range : %d to %d )
 */
#define DTL_ERRCODE_DATA_TYPE_INVALID                   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 178)

/**
 * @brief precision specifier is out of range( type : %s, precision : %d, range : %d to %d )
 */
#define DTL_ERRCODE_PRECISION_INVALID                   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 179)

/**
 * @brief leading precision specifier is out of range( type : %s, precision : %d, range : %d to %d )
 */
#define DTL_ERRCODE_LEADING_PRECISION_INVALID           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 180)

/**
 * @brief scale specifier is out of range( type : %s, scale : %d, range : %d to %d )
 */
#define DTL_ERRCODE_SCALE_INVALID                       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 181)

/**
 * @brief fractional second precision specifier is out of range( type : %s, precision : %d, range : %d to %d )
 */
#define DTL_ERRCODE_FRACTIONAL_SECOND_PRECISION_INVALID \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 182)

/**
 * @brief character length unit specifier is out of range( type : %s, character length unit : %d, range : %d to %d )
 */
#define DTL_ERRCODE_CHARACTER_LENGTH_UNIT_INVALID            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 183)

/**
 * @brief interval indicator specifier is out of range( type : %s, indicator : %d, range : %d to %d )
 */
#define DTL_ERRCODE_INTERVAL_INDICATOR_INVALID          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_DATATYPE, 184)


#define DTL_MAX_ERROR    185

/** @} dtlError */

#ifdef STL_IGNORE_VALIDATION
#define DTL_PARAM_VALIDATE(aExpression, aErrorStack)
#else
#define DTL_PARAM_VALIDATE(aExpression, aErrorStack) STL_PARAM_VALIDATE(aExpression, aErrorStack)
#endif

#if defined( STL_DEBUG ) || defined( DOXYGEN )
#define DTL_IS_VALID( aExpression ) STL_ASSERT( aExpression )
#else
#define DTL_IS_VALID( aExpression ) (aExpression)
#endif

/**
 * @defgroup dtlUpperAttr 상위 Layer Attribute
 * @ingroup dtlGeneral
 * @remarks
 * 상위 layer 에서 연산 처리에 필요한 Attribute 를 설정
 */

typedef struct dtlDataValue dtlDataValue;

/**
 * @brief Unicode Encoding Enumeration ID
 */
typedef enum
{
    DTL_UNICODE_UTF8,
    DTL_UNICODE_UTF16,
    DTL_UNICODE_UTF32,

    DTL_UNICODE_MAX
} dtlUnicodeEncoding;

/**
 * @brief CharacterSet Enumeration ID
 */
typedef enum
{
    DTL_SQL_ASCII = 0, /* 0 */
    DTL_UTF8,          /* 1 */    
    DTL_UHC,           /* 2 */
    DTL_GB18030,       /* 3 */

    DTL_CHARACTERSET_MAX
} dtlCharacterSet;

/**
 * @brief Character Map
 */
typedef struct dtlCharsetMap
{
    const stlChar         * mName;
    const dtlCharacterSet   mCharset;
} dtlCharsetMap;

/**
 * @brief Charset binary-comparable
 */
extern const stlBool dtsCharsetBinaryComparable[ DTL_CHARACTERSET_MAX ];

#define DTL_NOMOUNT_CHARSET_ID   ( DTL_UTF8 )
#define DTL_NOMOUNT_GMT_OFFSET   ( 32400 )
#define DTL_NOMOUNT_DATE_FORMAT                     ( "YYYY-MM-DD" )
#define DTL_NOMOUNT_TIME_FORMAT                     ( "HH24:MI:SS.FF6" )
#define DTL_NOMOUNT_TIME_WITH_TIME_ZONE_FORMAT      ( "HH24:MI:SS.FF6 TZH:TZM" )
#define DTL_NOMOUNT_TIMESTAMP_FORMAT                ( "YYYY-MM-DD HH24:MI:SS.FF6" )
#define DTL_NOMOUNT_TIMESTAMP_WITH_TIME_ZONE_FORMAT ( "YYYY-MM-DD HH24:MI:SS.FF6 TZH:TZM" )

typedef struct dtlDateTimeFormatStrInfo    dtlDateTimeFormatStrInfo;
typedef struct dtlDateTimeFormatInfo       dtlDateTimeFormatInfo;

/**
 * @brief Character Set ID 를 얻는 Function Pointer
 * @note
 * 에러 핸들링을 하지 않으므로,
 * 내부에서 에러가 발생한다면 기본값인 DTL_NOMOUNT_CHARSET_ID 값을 리턴 
 */
typedef dtlCharacterSet (* dtlGetCharSetIDFunc) ( void * aArgs );

/**
 * @brief Time Zone 의 GMT Offset 을 얻는 Function Pointer
 * @note
 * 에러 핸들링을 하지 않으므로,
 * 내부에서 에러가 발생한다면 기본값인 DTL_NOMOUNT_GMT_OFFSET 값을 리턴 
 */
typedef stlInt32 (* dtlGetTimeZoneGMTOffsetFunc) ( void * aArgs );

/**
 * @brief Date Format 을 얻는 Function Pointer
 * @todo
 * 사용할 수 있도록 확장해야 함.
 * @note
 * 에러 핸들링을 하지 않으므로,
 * 내부에서 에러가 발생한다면 기본값인 DTL_NOMOUNT_DATE_FORMAT 값을 리턴 
 * @note
 * 연산시마다 호출되는 함수이므로 프로토타입을 변경하는 것은 위험함.
 * 예를 들어, 동시성 제어를 위해 버퍼 공간을 할당해서 넘기는 방법을 사용하면 안됨.
 */
typedef stlChar *             (* dtlGetDateFormatStringFunc) ( void * aArgs );
typedef dtlDateTimeFormatInfo * (* dtlGetDateFormatInfoFunc) ( void * aArgs );

typedef stlChar *             (* dtlGetTimeFormatStringFunc) ( void * aArgs );
typedef dtlDateTimeFormatInfo * (* dtlGetTimeFormatInfoFunc) ( void * aArgs );

typedef stlChar *             (* dtlGetTimeWithTimeZoneFormatStringFunc) ( void * aArgs );
typedef dtlDateTimeFormatInfo * (* dtlGetTimeWithTimeZoneFormatInfoFunc) ( void * aArgs );

typedef stlChar *             (* dtlGetTimestampFormatStringFunc) ( void * aArgs );
typedef dtlDateTimeFormatInfo * (* dtlGetTimestampFormatInfoFunc) ( void * aArgs );

typedef stlChar *             (* dtlGetTimestampWithTimeZoneFormatStringFunc) ( void * aArgs );
typedef dtlDateTimeFormatInfo * (* dtlGetTimestampWithTimeZoneFormatInfoFunc) ( void * aArgs );


/**
 * @brief Long Varchar/Varbinary를 위한 추가 공간을 할당하는 Function Pointer
 */
typedef stlStatus  (* dtlReallocLongVaryingMemFunc) ( void           * aArgs,
                                                      stlInt32         aAllocSize,
                                                      void          ** aAddr,
                                                      stlErrorStack  * aErrorStack );
typedef stlStatus  (* dtlReallocAndMoveLongVaryingMemFunc) ( void           * aArgs,
                                                             dtlDataValue   * aDataValue,
                                                             stlInt32         aAllocSize,
                                                             stlErrorStack  * aErrorStack );

/**
 * @brief Data Type Layer 의 연산을 위해 Attribute 를 획득하는 Function Vector
 */
typedef struct dtlFuncVector
{
    dtlGetCharSetIDFunc                 mGetCharSetIDFunc;                   /**< Character Set ID function Ptr */
    dtlGetTimeZoneGMTOffsetFunc         mGetGMTOffsetFunc;                   /**< GMT Offset function Ptr */
    dtlReallocLongVaryingMemFunc        mReallocLongVaryingMemFunc;          /**< Realloc Long Varying Memory function Ptr */
    dtlReallocAndMoveLongVaryingMemFunc mReallocAndMoveLongVaryingMemFunc;   /**< Realloc And Move Long Varying
                                                                                  Memory function Ptr */

    /*
     * NLS_DATE_FORMAT 관련 함수
     */
    
    dtlGetDateFormatStringFunc     mGetDateFormatStringFunc;     /**< FORMAT String function Ptr */
    dtlGetDateFormatInfoFunc       mGetDateFormatInfoFunc;       /**< get FORMAT info function ptr */

    /*
     * NLS_TIME_FORMAT 관련 함수
     */
    
    dtlGetTimeFormatStringFunc     mGetTimeFormatStringFunc;     /**< FORMAT String function Ptr */
    dtlGetTimeFormatInfoFunc       mGetTimeFormatInfoFunc;       /**< get FORMAT info function ptr */

    /*
     * NLS_TIME_WITH_TIME_ZONE_FORMAT 관련 함수
     */
    
    dtlGetTimeWithTimeZoneFormatStringFunc     mGetTimeWithTimeZoneFormatStringFunc;     /**< FORMAT String function Ptr */
    dtlGetTimeWithTimeZoneFormatInfoFunc       mGetTimeWithTimeZoneFormatInfoFunc;       /**< get FORMAT info function ptr */
    
    /*
     * NLS_TIMESTAMP_FORMAT 관련 함수
     */
    
    dtlGetTimestampFormatStringFunc     mGetTimestampFormatStringFunc;     /**< FORMAT String function Ptr */
    dtlGetTimestampFormatInfoFunc       mGetTimestampFormatInfoFunc;       /**< get FORMAT info function ptr */

    /*
     * NLS_TIMESTAMP_WITH_TIME_ZONE_FORMAT 관련 함수
     */
    
    dtlGetTimestampWithTimeZoneFormatStringFunc     mGetTimestampWithTimeZoneFormatStringFunc;     /**< FORMAT String function Ptr */
    dtlGetTimestampWithTimeZoneFormatInfoFunc       mGetTimestampWithTimeZoneFormatInfoFunc;       /**< get FORMAT info function ptr */
 
} dtlFuncVector;
    
/** @} dtlUpperAttr */


/**
 * @defgroup dtlDataType  Data Type 
 * @ingroup dtExternal
 * @{
 */

/** @} dtlDataType */


/**
 * @defgroup dtlDataTypeDefinition Data Type 정의
 * @ingroup dtlDataType
 * @remarks Dictionary 관련 정수형 변수는 다음과 같이 type을 가져간다.
 *          <BR> INTEGER : precision/scale, Code Identifier
 *          <BR> BIGINT  : length, Object Identifier
 * @{
 */

/**
 * @brief Data Type 이름의 문자열 최대 길이
 */
#define DTL_DATA_TYPE_NAME_MAX_SIZE   ( 32 )

/**
 * @brief Dump Function에서 사용할 Data Type 이름의 문자열 최대 길이
 */
#define DTL_DATA_TYPE_NAME_FOR_DUMP_FUNC_MAX_SIZE   ( 16 )

/**
 * @brief Dump Function에서 사용할 Data Type 정보 길이 (Byte 출력 제외)
 */
#define DTL_DUMP_FUNC_TYPE_INFO_SIZE   ( 64 )


/**
 * @brief Data Type Enumeration ID
 */
typedef enum
{
    DTL_TYPE_BOOLEAN = 0,      /**< BOOLEAN */
    DTL_TYPE_NATIVE_SMALLINT,  /**< NATIVE_SMALLINT */
    DTL_TYPE_NATIVE_INTEGER,   /**< NATIVE_INTEGER */
    DTL_TYPE_NATIVE_BIGINT,    /**< NATIVE_BIGINT */

    DTL_TYPE_NATIVE_REAL,      /**< NATIVE_REAL */
    DTL_TYPE_NATIVE_DOUBLE,    /**< NATIVE_DOUBLE */

    DTL_TYPE_FLOAT,            /**< FLOAT */
    DTL_TYPE_NUMBER,           /**< NUMBER */
    DTL_TYPE_DECIMAL,          /**< unsupported feature, DECIMAL */
    
    DTL_TYPE_CHAR,          /**< CHARACTER */
    DTL_TYPE_VARCHAR,       /**< CHARACTER VARYING */
    DTL_TYPE_LONGVARCHAR,   /**< CHARACTER LONG VARYING */
    DTL_TYPE_CLOB,          /**< unsupported feature, CHARACTER LARGE OBJECT */
    
    DTL_TYPE_BINARY,        /**< BINARY */
    DTL_TYPE_VARBINARY,     /**< BINARY VARYING */
    DTL_TYPE_LONGVARBINARY, /**< BINARY LONG VARYING */
    DTL_TYPE_BLOB,          /**< unsupported feature, BINARY LARGE OBJECT */

    DTL_TYPE_DATE,                     /**< DATE */
    DTL_TYPE_TIME,                     /**< TIME WITHOUT TIME ZONE */
    DTL_TYPE_TIMESTAMP,                /**< TIMESTAMP WITHOUT TIME ZONE */
    DTL_TYPE_TIME_WITH_TIME_ZONE,      /**< TIME WITH TIME ZONE */
    DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, /**< TIMESTAMP WITH TIME ZONE */

    DTL_TYPE_INTERVAL_YEAR_TO_MONTH,   /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
    DTL_TYPE_INTERVAL_DAY_TO_SECOND,   /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

    DTL_TYPE_ROWID,                    /**< DTL_TYPE_ROWID */

    DTL_TYPE_MAX
} dtlDataType;

/**
 * @brief 유효하지 않은 DB Type
 */
#define DTL_TYPE_NA  (DTL_TYPE_MAX)

#define DTL_STATIC_TYPE_COUNT   ( 7 )

extern const stlChar dtlDataTypeName[ DTL_TYPE_MAX ][ DTL_DATA_TYPE_NAME_MAX_SIZE ];

extern const stlChar dtlDataTypeNameForDumpFunc[ DTL_TYPE_MAX ][ DTL_DATA_TYPE_NAME_FOR_DUMP_FUNC_MAX_SIZE ];

/** boolean : 0과 1로 TRUE, FALSE를 표현 */
typedef stlBool      dtlBooleanType;

/** SmallInt : 부호를 지닌 2바이트(16비트) 정수 */
typedef stlInt16     dtlSmallIntType;

/** Integer : 부호를 지닌 4바이트(32비트) 정수 */
typedef stlInt32     dtlIntegerType;

/** BigInt : 부호를 지닌 8바이트(64비트) 정수 */
typedef stlInt64     dtlBigIntType;

/** real : 4바이트 부동소수점(32비트 실수)     */
typedef stlFloat32   dtlRealType;

/** double precision : 8바이트 부동소수점(64비트 실수) */
typedef stlFloat64   dtlDoubleType;


/**
 * NUMERIC TYPE
 * <BR>
 * <BR> < NUMERIC 구조 >
 * <BR> Exponent : -65 <= exponent <= 62 (100진수 기준)
 * <BR>            Exponent : Digit Sign (1-bit, 최상위 비트) + Exponent (7-bit, 하위 비트)
 * <BR>            Exponent 표현 : Exponent (7-bit)
 * <BR>              1) Digit Sign이 양수(+)일 경우
 * <BR>                            Exponent Value + 65
 * <BR>              1) Digit Sign이 음수(-)일 경우
 * <BR>                            62 - Exponent Value
 * <BR> Digit    : 1 ~ 20 (1 <= precision <= 38 )
 * <BR>            cf. digit 구성이 "01|34|...|20" 과 같이 38자리의 유효숫자가 존재하는 구조도 고려
 * <BR>
 * <BR> < NUMERIC에 대한 비교 연산 >
 * <BR> * memcmp를 이용
 * <BR> * 첫번째 Exponent byte를 포함한 바이트 비교
 */           
typedef struct dtlNumericType
{
    stlUInt8  mData[0];
} dtlNumericType;


#define DTL_NUMERIC_SIGN_MASK       (0x80)
#define DTL_NUMERIC_SIGN_POSITIVE   (0x80)
#define DTL_NUMERIC_SIGN_NEGATIVE   (0x00)

/**
 * Sign
 */

#define DTL_NUMERIC_SET_POSITIVE_SIGN( aNumeric )               \
    {                                                           \
        (aNumeric)->mData[0] |= DTL_NUMERIC_SIGN_POSITIVE;      \
    }

#define DTL_NUMERIC_SET_NEGATIVE_SIGN( aNumeric )               \
    {                                                           \
        (aNumeric)->mData[0] |= DTL_NUMERIC_SIGN_NEGATIVE;      \
    }

#define DTL_NUMERIC_IS_POSITIVE( aNumeric )     \
    ( (aNumeric)->mData[0] > 127 )

#define DTL_NUMERIC_IS_NEGATIVE( aNumeric )     \
    ( (aNumeric)->mData[0] < 128 )

#define DTL_IS_POSITIVE( aDataValue )                           \
    ( ((dtlNumericType*)(aDataValue)->mValue)->mData[0] > 127 )

#define DTL_IS_NEGATIVE( aDataValue )                           \
    ( ((dtlNumericType*)(aDataValue)->mValue)->mData[0] < 128 )


/**
 * Exponent
 */

#define DTL_NUMERIC_SET_EXPONENT( aNumeric, aIsNegative, aExp ) \
    {                                                           \
        STL_DASSERT( (aIsNegative)                              \
                     ? ((62 - (aExp)) >= 0)                     \
                     : ((65 + (aExp)) >= 0) );                  \
        ( (aIsNegative)                                         \
          ? ( (aNumeric)->mData[0] |= (62 - (aExp)) )           \
          : ( (aNumeric)->mData[0] |= (65 + (aExp)) ) );        \
    }

#define DTL_NUMERIC_GET_EXPONENT( aNumeric )                            \
    ( DTL_NUMERIC_IS_POSITIVE( aNumeric )                               \
      ? ( DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aNumeric ) )    \
      : ( DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aNumeric ) ) )

#define DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aNumeric )        \
    ( (aNumeric)->mData[0] - 193 )

#define DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aNumeric )        \
    ( 62 - (aNumeric)->mData[0] )


/**
 * Sign & Exponent
 */

#define DTL_NUMERIC_SET_SIGN_AND_EXPONENT( aNumeric, aIsPositive, aExp )                \
    {                                                                                   \
        if( (aIsPositive) == STL_TRUE )                                                 \
        {                                                                               \
            STL_DASSERT( ((aExp) - DTL_NUMERIC_MIN_EXPONENT) >= 0 );                    \
            (aNumeric)->mData[0] =                                                      \
                ( DTL_NUMERIC_SIGN_POSITIVE | ((aExp) - DTL_NUMERIC_MIN_EXPONENT) );    \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            STL_DASSERT( (DTL_NUMERIC_MAX_EXPONENT - (aExp)) >= 0 );                    \
            (aNumeric)->mData[0] =                                                      \
                ( DTL_NUMERIC_SIGN_NEGATIVE | (DTL_NUMERIC_MAX_EXPONENT - (aExp)) );    \
        }                                                                               \
    }

#define DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( aNumeric, aExp )                                \
    {                                                                                               \
        STL_DASSERT( ((aExp) - DTL_NUMERIC_MIN_EXPONENT) >= 0 );                                    \
        (aNumeric)->mData[0] = ( DTL_NUMERIC_SIGN_POSITIVE | ((aExp) - DTL_NUMERIC_MIN_EXPONENT) ); \
    }

#define DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( aNumeric, aExp )                                \
    {                                                                                               \
        STL_DASSERT( (DTL_NUMERIC_MAX_EXPONENT - (aExp)) >= 0 );                                    \
        (aNumeric)->mData[0] = ( DTL_NUMERIC_SIGN_NEGATIVE | (DTL_NUMERIC_MAX_EXPONENT - (aExp)) ); \
    }

#define DTL_NUMERIC_SET_REVERSE_SIGN_AND_EXPONENT( aSrcNumeric, aDestNumeric )  \
    {                                                                           \
        (aDestNumeric)->mData[0] = 0xFF - (aSrcNumeric)->mData[0];              \
    }


/**
 * Digits
 */

#define DTL_NUMERIC_DIGIT_POSITIVE_FENCE_VALUE    ( 1 )
#define DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE    ( 101 )
#define DTL_NUMERIC_DIGIT_REVERSE_FENCE_VALUE     ( 102 )

#define DTL_NUMERIC_POSITIVE_DIGIT_ZERO_VALUE     ( 1 )
#define DTL_NUMERIC_POSITIVE_DIGIT_MIN_VALUE      ( 1 )
#define DTL_NUMERIC_POSITIVE_DIGIT_MAX_VALUE      ( 100 )

#define DTL_NUMERIC_NEGATIVE_DIGIT_ZERO_VALUE     ( 101 )
#define DTL_NUMERIC_NEGATIVE_DIGIT_MIN_VALUE      ( 2 )
#define DTL_NUMERIC_NEGATIVE_DIGIT_MAX_VALUE      ( 101 )

#define DTL_NUMERIC_GET_DIGIT_PTR( aNumeric ) ( &((aNumeric)->mData[1]) )

#define DTL_NUMERIC_GET_DIGIT_COUNT( aLength )  \
    ( (aLength) - 1 )


/**
 * Value To Digit & Digit To Value
 */

#define DTL_NUMERIC_GET_DIGIT_FROM_VALUE( aIsPositive, aValue ) \
    ( (aIsPositive) == STL_TRUE                                 \
      ? ( DTL_NUMERIC_DIGIT_POSITIVE_FENCE_VALUE + (aValue) )   \
      : ( DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE - (aValue) ) )

#define DTL_NUMERIC_GET_VALUE_FROM_DIGIT( aIsPositive, aDigit ) \
    ( (aIsPositive) == STL_TRUE                                 \
      ? ( (aDigit) - DTL_NUMERIC_DIGIT_POSITIVE_FENCE_VALUE )   \
      : ( DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE - (aDigit) ) )

#define DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( aValue )     \
    ( DTL_NUMERIC_DIGIT_POSITIVE_FENCE_VALUE + (aValue) )

#define DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( aValue )     \
    ( DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE - (aValue) )

#define DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( aDigit )     \
    ( (aDigit) - DTL_NUMERIC_DIGIT_POSITIVE_FENCE_VALUE )

#define DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( aDigit )     \
    ( DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE - (aDigit) )

#define DTL_NUMERIC_GET_REVERSE_DIGIT_FROM_DIGIT( aDigit )      \
    ( DTL_NUMERIC_DIGIT_REVERSE_FENCE_VALUE - *(aDigit) )


/**
 * ZERO
 */

#define DTL_NUMERIC_ZERO_LENGTH  ( 1 )

#define DTL_NUMERIC_SET_ZERO( aNumeric, aLength )                       \
    {                                                                   \
        ((dtlNumericType*)(aNumeric))->mData[0] = (stlUInt8)0x80;       \
        (aLength) = DTL_NUMERIC_ZERO_LENGTH;                            \
    }

#define DTL_NUMERIC_DATA_SET_ZERO( aNumeric )                           \
    {                                                                   \
        ((dtlNumericType*)(aNumeric))->mData[0] = (stlUInt8)0x80;       \
    }

#define DTL_NUMERIC_IS_ZERO( aNumeric, aLength )                        \
    ( ( (aLength) == DTL_NUMERIC_ZERO_LENGTH ) &&                       \
      ( ((dtlNumericType*)(aNumeric))->mData[0] == (stlUInt8)0x80 )  )
     

/**
 * Reset Header
 */

#define DTL_NUMERIC_RESET_HEADER( aNumeric )    \
    {                                           \
        (aNumeric)->mData[0] = 0;               \
    }


typedef struct dtlNumericCarryInfo
{
    stlInt8    mCarry;
    stlUInt8   mDigit;
} dtlNumericCarryInfo;

extern const dtlNumericCarryInfo  dtdNumericInvalidCarryInfo[1];
extern const dtlNumericCarryInfo  dtdNumericDigitMap[2][2][203];

extern const stlUInt8 dtlNumericZero[2];

#define DTL_DIV_UINT32_BY_100( aNum )                                                           \
    ( ( ( ( ((stlUInt64)(aNum) * (stlUInt64)0x47AE147B) >> 32 ) + (aNum) ) >> 1 ) >> 6 )


/** character(n), char(n) : 고정길이 character type */
typedef stlChar    * dtlCharType;

/** character varying(n), varchar(n) : 가변길이 character type */
typedef dtlCharType  dtlVarcharType;

/** character long varying : 가변길이 character type */
typedef dtlCharType  dtlLongvarcharType;

/** binary(n) : 고정길이 binary type */
typedef stlChar    * dtlBinaryType;

/** binary varying(n), varbinary(n) : 가변길이 binary type  */
typedef dtlBinaryType  dtlVarbinaryType;

/** binary long varying : 가변길이 binary type */
typedef dtlBinaryType  dtlLongvarbinaryType;

/** date : year, month, day, hour, minute, second를 표현       */
typedef stlInt64     dtlDateType;

/** time : hour, minute, second, fractional second를 표현    */
typedef stlInt64     dtlTimeType;

/** time with timeZone : hour, minute, second, fractional second, timeZone를 표현 */
typedef struct dtlTimeTzType
{
    dtlTimeType   mTime;      /**< hour, minute, second */
    stlInt32      mTimeZone;  /**< timeZone Offset      */
} dtlTimeTzType;

/** timestamp : date(year, month, day) + time(hour, minute, second, fractional second)를 표현 */
typedef stlInt64  dtlTimestampType;

/**
 *  timestamp with timeZone :
 *  date(year, month, day) + time(hour, minute, second, fractional second) + timeZone를 표현
 *  예) '1999-01-01 09:00:00 +09:00' 의 데이타는 아래와 같은 데이타가 멤버타입에 맞게 저장된다. 
 *      dtlTimestampTzType.mTimestamp = '1999-01-01 00:00:00'
 *                                      ( 09:00를 뺀 microsecond단위의 UTC time으로 저장됨 )
 *      dtlTimestampTzType.mTimeZone  = '+09:00을 초로 변환한 값'
 */
//typedef stlInt64  dtlTimestampTzType;
typedef struct dtlTimestampTzType
{
    dtlTimestampType mTimestamp; /**< date(year, month, day) + time(hour, minute, second, fractional second) */
    stlInt32         mTimeZone;  /**< timeZone Offset */  
} dtlTimestampTzType;

typedef stlInt64  dtlTimeT;
typedef stlInt64  dtlTimeOffset;
typedef stlInt32  dtlFractionalSecond;

/** interval : time의 기간을 표현      */

#define DTL_INTERVAL_YEAR_TO_MONTH_TYPE_ID    ( DTL_TYPE_INTERVAL_YEAR_TO_MONTH )
#define DTL_INTERVAL_YEAR_TO_MONTH_TYPE_COUNT ( 3 )

#define DTL_INTERVAL_DAY_TO_SECOND_TYPE_ID    ( DTL_TYPE_INTERVAL_DAY_TO_SECOND )
#define DTL_INTERVAL_DAY_TO_SECOND_TYPE_COUNT ( 10 )

/**
 * @brief INTERVAL 타입의 Indicator
 */
typedef enum dtlIntervalIndicator
{
    DTL_INTERVAL_INDICATOR_NA = 0,           /**< Not Available */
    
    DTL_INTERVAL_INDICATOR_YEAR,             /**< YEAR interval */
    DTL_INTERVAL_INDICATOR_MONTH,            /**< MONTH interval */
    DTL_INTERVAL_INDICATOR_DAY,              /**< DAY interval */
    DTL_INTERVAL_INDICATOR_HOUR,             /**< HOUR interval */
    DTL_INTERVAL_INDICATOR_MINUTE,           /**< MINUTE interval */
    DTL_INTERVAL_INDICATOR_SECOND,           /**< SECOND interval */
    DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH,    /**< YEAR TO MONTH interval */
    DTL_INTERVAL_INDICATOR_DAY_TO_HOUR,      /**< DAY TO MONTH interval */
    DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE,    /**< DAY TO MINUTE interval */
    DTL_INTERVAL_INDICATOR_DAY_TO_SECOND,    /**< DAY TO SECOND interval */
    DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE,   /**< HOUR TO MINUTE interval */
    DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND,   /**< HOUR TO SECOND interval */
    DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND, /**< MINUTE TO SECOND interval */
    
    DTL_INTERVAL_INDICATOR_MAX
} dtlIntervalIndicator;

typedef struct dtlIntervalYearToMonthType
{
    stlInt32  mMonth;
    stlInt32  mIndicator;
} dtlIntervalYearToMonthType;

typedef struct dtlIntervalDayToSecondType
{
    dtlTimeOffset   mTime;
    stlInt32        mDay;
    stlInt32        mIndicator;
} dtlIntervalDayToSecondType;

extern stlChar * gDtlIntervalIndicatorString[DTL_INTERVAL_INDICATOR_MAX];

extern stlChar * gDtlIntervalIndicatorStringForDump[DTL_INTERVAL_INDICATOR_MAX];


/**
 * @brief INTERVAL 타입의 Primary Field
 */
typedef enum dtlIntervalPrimaryField
{
    DTL_INTERVAL_PRIMARY_NONE,   /**< 정의 없음 */
    DTL_INTERVAL_PRIMARY_YEAR,   /**< YEAR */
    DTL_INTERVAL_PRIMARY_MONTH,  /**< MONTH */
    DTL_INTERVAL_PRIMARY_DAY,    /**< DAY */
    DTL_INTERVAL_PRIMARY_HOUR,   /**< HOUR */
    DTL_INTERVAL_PRIMARY_MINUTE, /**< MINUTE */
    DTL_INTERVAL_PRIMARY_SECOND    /**< SECOND */
        
} dtlIntervalPrimaryField;


/**
 * @brief DateTime field 
 */
typedef enum dtlDateTimeField
{
    DTL_DATETIME_FIELD_NONE   = DTL_INTERVAL_PRIMARY_NONE,   /**< 정의 없음 */
    DTL_DATETIME_FIELD_YEAR   = DTL_INTERVAL_PRIMARY_YEAR,   /**< YEAR */
    DTL_DATETIME_FIELD_MONTH  = DTL_INTERVAL_PRIMARY_MONTH,  /**< MONTH */
    DTL_DATETIME_FIELD_DAY    = DTL_INTERVAL_PRIMARY_DAY,    /**< DAY */
    DTL_DATETIME_FIELD_HOUR   = DTL_INTERVAL_PRIMARY_HOUR,   /**< HOUR */
    DTL_DATETIME_FIELD_MINUTE = DTL_INTERVAL_PRIMARY_MINUTE, /**< MINUTE */
    DTL_DATETIME_FIELD_SECOND = DTL_INTERVAL_PRIMARY_SECOND,  /**< SECOND */
    DTL_DATETIME_FIELD_TIMEZONE_HOUR,  /**< TIMEZONE_HOUR */
    DTL_DATETIME_FIELD_TIMEZONE_MINUTE  /**< TIMEZONE_MINUTE */
    
} dtlDateTimeField;


/**
 * @brief DateTime part
 */
typedef enum dtlDateTimePart
{
    DTL_DATETIME_PART_NONE   = DTL_INTERVAL_PRIMARY_NONE,   /**< 정의 없음 */    
    DTL_DATETIME_PART_YEAR   = DTL_INTERVAL_PRIMARY_YEAR,   /**< YEAR */
    DTL_DATETIME_PART_MONTH  = DTL_INTERVAL_PRIMARY_MONTH,  /**< MONTH */
    DTL_DATETIME_PART_DAY    = DTL_INTERVAL_PRIMARY_DAY,    /**< DAY */
    DTL_DATETIME_PART_HOUR   = DTL_INTERVAL_PRIMARY_HOUR,   /**< HOUR */
    DTL_DATETIME_PART_MINUTE = DTL_INTERVAL_PRIMARY_MINUTE, /**< MINUTE */
    DTL_DATETIME_PART_SECOND = DTL_INTERVAL_PRIMARY_SECOND, /**< SECOND */

    DTL_DATETIME_PART_QUARTER,                              /**< QUARTER */  
    DTL_DATETIME_PART_DAYOFYEAR,                            /**< DAYOFYEAR */
    DTL_DATETIME_PART_WEEK,                                 /**< WEEK */  
    DTL_DATETIME_PART_WEEKDAY,                              /**< WEEKDAY */
    DTL_DATETIME_PART_MILLISECOND,                          /**< MILLISECOND */ 
    DTL_DATETIME_PART_MICROSECOND,                          /**< MICROSECOND */
    
    DTL_DATETIME_PART_MAX
    
} dtlDateTimePart;


/**
 * ROWID type                                                          <BR>
 *                                                                     <BR>
 * 16byte hexadecimal string                                           <BR>
 *      배열[0]~[7]   : 8 byte objectID ( physical ID )                <BR>
 *      배열[8]~[9]   : 2 byte 테이블스페이스 아이디                   <BR> 
 *      배열[10]~[13] : 4 byte 페이지 아이디                           <BR>
 *      배열[14]~[15] : 2 byte 페이지내 옵셋                           <BR>
 *                                                                     <BR>
 * ex) ObjectID     : 16406775070720                                   <BR>
 *     TablespaceID : 2                                                <BR>
 *     PageID       : 34                                               <BR>
 *     PageOffset   : 6                                                <BR>
 *                                                                     <BR>
 *     < ROWID type에 다음과 같이 저장됨. >                            <BR>
 *                                                                     <BR>
 *     . objectID ( physical ID ) : 16406775070720                     <BR>
 *      ( Table Physical ID : stlInt64 )                               <BR>
 *       mDigits[0] = 0x00                                             <BR>
 *       mDigits[1] = 0x00                                             <BR>
 *       mDigits[2] = 0x0E                                             <BR>
 *       mDigits[3] = 0xEC                                             <BR>
 *       mDigits[4] = 0x00                                             <BR>
 *       mDigits[5] = 0x00                                             <BR>
 *       mDigits[6] = 0x00                                             <BR>
 *       mDigits[7] = 0x00                                             <BR>
 *                                                                     <BR>
 *     . 테이블스페이스 아이디 : 2                                     <BR>
 *      ( Tablespace ID : stlUInt16 )                                  <BR>
 *       mDigits[8] = 0x00                                             <BR>
 *       mDigits[9] = 0x02                                             <BR>
 *                                                                     <BR>
 *     . 페이지 아이디 : 34                                            <BR> 
 *      ( Page ID : stlUInt32 )                                        <BR>
 *       mDigits[10] = 0x00                                            <BR>
 *       mDigits[11] = 0x00                                            <BR>
 *       mDigits[12] = 0x00                                            <BR>
 *       mDigits[13] = 0x22                                            <BR>
 *                                                                     <BR>
 *     . ROW NUMBER (페이지내 옵셋) : 6                                <BR>
 *      ( Page OFFSET : stlInt16 )                                     <BR>
 *       mDigits[14] = 0x00                                            <BR>
 *       mDigits[15] = 0x06                                            <BR>
 *                                                                     <BR>
 *     < SELECT로 검색해서 사용자에게 보여지는 string은 다음과 같음. > <BR>
 *                                                                     <BR>
 *       SELECT ROWID FROM ...;                                        <BR>
 *       ROWID                                                         <BR>
 *       ----------------------                                        <BR>
 *       AAADuwAAAAAAACAAAAAiAAG
 *     
 */

typedef struct dtlRowIdType
{
    stlUInt8   mDigits[16];  /**< hexadecimal arrary */
} dtlRowIdType;


typedef struct dtlExpTime
{
    stlInt32        mSecond;
    stlInt32        mMinute;
    stlInt32        mHour;
    stlInt32        mDay;
    stlInt32        mMonth;
    stlInt32        mYear;
    stlInt32        mWeekDay;
    stlInt32        mYearDay;
    stlInt32        mIsDaylightSaving;
    stlInt64        mTimeGmtOff;
    const stlChar * mTimeZone;
} dtlExpTime;


/**
 * Comparison Result Type Combination
 */
extern const dtlDataType dtlComparisonFuncArgType[DTL_TYPE_MAX][DTL_TYPE_MAX];

extern const dtlDataType dtlComparisonResultTypeCombination[DTL_TYPE_MAX][DTL_TYPE_MAX];

/**
 * Result Type Combination
 */
extern const dtlDataType dtlResultTypeCombination[DTL_TYPE_MAX][DTL_TYPE_MAX];

/**
 * Result Interval Indicator Combination
 */
extern const dtlIntervalIndicator
dtlIntervalIndicatorContainArgIndicator[DTL_INTERVAL_INDICATOR_MAX][DTL_INTERVAL_INDICATOR_MAX];

/** @} dtlDataTypeDefinition */






/**
 * @defgroup dtlDataTypeInformation Data Type 정보   
 * @ingroup dtlDataType
 * @{
 */

/** 최대 최소 범위 값 */
#define DTL_NATIVE_SMALLINT_MIN    ((stlInt64) STL_INT16_MIN)
#define DTL_NATIVE_SMALLINT_MAX    ((stlInt64) STL_INT16_MAX)
#define DTL_NATIVE_INTEGER_MIN     ((stlInt64) STL_INT32_MIN)
#define DTL_NATIVE_INTEGER_MAX     ((stlInt64) STL_INT32_MAX)
#define DTL_NATIVE_BIGINT_MIN      ((stlInt64) STL_INT64_MIN)
#define DTL_NATIVE_BIGINT_MAX      ((stlInt64) STL_INT64_MAX)
#define DTL_DATE_MIN               ((stlInt64) STL_INT32_MIN)
#define DTL_DATE_MAX               ((stlInt64) STL_INT32_MAX)

/** 고정 길이 타입의 길이 */
#define DTL_BOOLEAN_SIZE                 STL_SIZEOF( dtlBooleanType )
#define DTL_NATIVE_SMALLINT_SIZE         STL_SIZEOF( dtlSmallIntType )
#define DTL_NATIVE_INTEGER_SIZE          STL_SIZEOF( dtlIntegerType )
#define DTL_NATIVE_BIGINT_SIZE           STL_SIZEOF( dtlBigIntType )
#define DTL_NATIVE_REAL_SIZE             STL_SIZEOF( dtlRealType )
#define DTL_NATIVE_DOUBLE_SIZE           STL_SIZEOF( dtlDoubleType )
#define DTL_DATE_SIZE                    STL_SIZEOF( dtlDateType )
#define DTL_TIME_SIZE                    STL_SIZEOF( dtlTimeType )
#define DTL_TIMETZ_SIZE                  (STL_SIZEOF(dtlTimeType)+STL_SIZEOF(stlInt32))
#define DTL_TIMESTAMP_SIZE               STL_SIZEOF( dtlTimestampType )
#define DTL_TIMESTAMPTZ_SIZE             (STL_SIZEOF(dtlTimestampType)+STL_SIZEOF(stlInt32))
#define DTL_INTERVAL_YEAR_TO_MONTH_SIZE  STL_SIZEOF( dtlIntervalYearToMonthType )
#define DTL_INTERVAL_DAY_TO_SECOND_SIZE  STL_SIZEOF( dtlIntervalDayToSecondType )
#define DTL_ROWID_SIZE                   STL_SIZEOF( dtlRowIdType )

/** 가변 길이 타입의 길이 범위 */
#define DTL_CHAR_MIN_PRECISION         ( 1 )             /** 1 Byte or Char */
#define DTL_CHAR_MAX_PRECISION         ( 2000 )          /** 2000 Byte or Char */
#define DTL_VARCHAR_MIN_PRECISION      ( 1 )             /** 1 Byte or Char */
#define DTL_VARCHAR_MAX_PRECISION      ( 4000 )          /** 4000 Byte or Char */
#define DTL_LONGVARCHAR_MAX_PRECISION  ( 100*1024*1024 ) /** 10MB */

#define DTL_CHAR_MAX_BUFFER_SIZE       ( DTL_CHAR_MAX_PRECISION * 4 )
#define DTL_VARCHAR_MAX_BUFFER_SIZE    ( DTL_VARCHAR_MAX_PRECISION * 4 )

#define DTL_CHAR_OCTETS_STRING_SIZE           ( DTL_CHAR_MAX_PRECISION + 1 )  
#define DTL_VARCHAR_OCTETS_STRING_SIZE        ( DTL_VARCHAR_MAX_PRECISION + 1 )  

/**
 * @todo db character set 에 따라 달라질 수 있음
 */
#define DTL_CHAR_MAX_CHAR_STRING_SIZE           ( DTL_CHAR_MAX_PRECISION * 4 + 1)  
#define DTL_VARCHAR_MAX_CHAR_STRING_SIZE        ( DTL_VARCHAR_MAX_PRECISION * 4 + 1)  

#define DTL_BINARY_MIN_PRECISION          ( 1 )             /** 2000 Byte */
#define DTL_BINARY_MAX_PRECISION          ( 2000 )          /** 2000 Byte */
#define DTL_VARBINARY_MIN_PRECISION       ( 1 )             /** 4000 Byte */
#define DTL_VARBINARY_MAX_PRECISION       ( 4000 )          /** 4000 Byte */
#define DTL_LONGVARBINARY_MAX_PRECISION   ( 100*1024*1024 ) /** 10MB */

#define DTL_BINARY_STRING_SIZE            ( DTL_BINARY_MAX_PRECISION + 1 )
#define DTL_VARBINARY_STRING_SIZE         ( DTL_VARBINARY_MAX_PRECISION + 1 )

#define DTL_ROWID_TO_STRING_LENGTH        ( 23 )
#define DTL_ROWID_STRING_SIZE             ( DTL_ROWID_TO_STRING_LENGTH + 1 )

#define DTL_ROWID_TO_UTF16_STRING_LENGTH  ( DTL_ROWID_TO_STRING_LENGTH * STL_UINT16_SIZE )
#define DTL_ROWID_TO_UTF32_STRING_LENGTH  ( DTL_ROWID_TO_STRING_LENGTH * STL_UINT32_SIZE )


/** default binary precision */
#define DTL_NATIVE_SMALLINT_DEFAULT_PRECISION  ( 15 )
#define DTL_NATIVE_INTEGER_DEFAULT_PRECISION   ( 31 )
#define DTL_NATIVE_BIGINT_DEFAULT_PRECISION    ( 63 )
#define DTL_NATIVE_REAL_DEFAULT_PRECISION      ( 23 )
#define DTL_NATIVE_DOUBLE_DEFAULT_PRECISION    ( 52 )

#define DTL_DECIMAL_SMALLINT_DEFAULT_PRECISION  ( 5 )
#define DTL_DECIMAL_INTEGER_DEFAULT_PRECISION   ( 10 )
#define DTL_DECIMAL_BIGINT_DEFAULT_PRECISION    ( 19 )
#define DTL_DECIMAL_REAL_DEFAULT_PRECISION      ( 24 )
#define DTL_DECIMAL_DOUBLE_DEFAULT_PRECISION    ( 53 )

#define DTL_DECIMAL_SMALLINT_DEFAULT_PRECISION_STRING  ( "5" )
#define DTL_DECIMAL_INTEGER_DEFAULT_PRECISION_STRING   ( "10" )
#define DTL_DECIMAL_BIGINT_DEFAULT_PRECISION_STRING    ( "19" )
#define DTL_DECIMAL_REAL_DEFAULT_PRECISION_STRING      ( "24" )
#define DTL_DECIMAL_DOUBLE_DEFAULT_PRECISION_STRING    ( "53" )
#define DTL_DECIMAL_NUMERIC_DEFAULT_PRECISION_STRING    ( "38" )
#define DTL_DECIMAL_FLOAT_DEFAULT_PRECISION_STRING    ( "126" )

/** default decimal precision */
#define DTL_FLOAT_DEFAULT_PRECISION   ( 126 )
#define DTL_FLOAT_MAX_PRECISION       ( 126 )
#define DTL_FLOAT_MIN_PRECISION       ( 1 )   

#define DTL_NUMERIC_DEFAULT_PRECISION   ( 38 )
#define DTL_NUMERIC_DEFAULT_SCALE       ( 0 )

/** NUMERIC 길이 범위 */
#define DTL_NUMERIC_MAX_PRECISION       ( 38 )
#define DTL_NUMERIC_MIN_PRECISION       ( 1 )   


#define DTL_NUMERIC_MIN_SCALE           ( -84 )
#define DTL_NUMERIC_MAX_SCALE           ( 127 )

#define DTL_NUMERIC_MIN_EXPONENT        ( -65 )
#define DTL_NUMERIC_MAX_EXPONENT        ( 62 )

#define DTL_NUMBER_MIN_EXPONENT          ( -130 )
#define DTL_NUMBER_MAX_EXPONENT          ( 126 )

#define DTL_NUMBER_MIN_SCALE             ( -88 )
#define DTL_NUMBER_MAX_SCALE             ( 128 )

#define DTL_NUMERIC_MAX_DIGIT_COUNT     ( 20 )

extern const stlUInt16 dtlNumericDigitSize[ DTL_NUMERIC_MAX_PRECISION + 1 ];
extern const stlUInt16 dtlBinaryToDecimalPrecision[ DTL_FLOAT_MAX_PRECISION + 1];
extern const stlUInt16 dtlDecimalToBinaryPrecision[ DTL_NUMERIC_MAX_PRECISION + 1];

/** NUMERIC 크기 */
#define DTL_NUMERIC_EXPONENT_SIZE       ( 1 )
#define DTL_NUMERIC_MIN_SIZE            ( DTL_NUMERIC_EXPONENT_SIZE )
#define DTL_NUMERIC_MAX_SIZE            ( DTL_NUMERIC_EXPONENT_SIZE + DTL_NUMERIC_MAX_DIGIT_COUNT )

#define DTL_NUMERIC_SIZE(precision)     ( DTL_NUMERIC_EXPONENT_SIZE + dtlNumericDigitSize[ precision ] )
#define DTL_FLOAT_SIZE(precision)       ( DTL_NUMERIC_SIZE( dtlBinaryToDecimalPrecision[ precision ] ) )

/** default decimal precision */
#define DTL_INT16_DECIMAL_PRECISION     ( 4 )
#define DTL_INT32_DECIMAL_PRECISION     ( 9 )
#define DTL_INT64_DECIMAL_PRECISION     ( 18 )
#define DTL_UINT16_DECIMAL_PRECISION    ( 4 )
#define DTL_UINT32_DECIMAL_PRECISION    ( 9 )
#define DTL_UINT64_DECIMAL_PRECISION    ( 19 )

/** min/max decimal exponent */
#define DTL_FLOAT32_MIN_EXPONENT        ( -38 )
#define DTL_FLOAT32_MAX_EXPONENT        ( 38 )
#define DTL_FLOAT64_MIN_EXPONENT        ( -323 )
#define DTL_FLOAT64_MAX_EXPONENT        ( 308 )

/** BINARY REAL 관련 정의 */
#define DTL_BINARY_REAL_INFINITY        ( "INFINITY" )
#define DTL_BINARY_REAL_INFINITY_SIZE   stlStrlen(DTL_BINARY_REAL_INFINITY)

/** DATE TIME 관련 정의 */
#define DTL_EPOCH_JDATE                 ( 2451545 ) /** == dtdDate2JulianDate(2000, 1, 1) */
#define DTL_UNIX_EPOCH_JDATE            ( 2440588 ) /** == dtdDate2JulianDate(1970, 1, 1) */

#define DTL_YEARS_PER_CENTURY           ( 100 )
#define DTL_QUARTERS_PER_YEAR           ( 4 )

#define DTL_SECS_PER_DAY                ( 86400 )   /** 60 * 60 * 24 */
#define DTL_SECS_PER_HOUR               ( 3600 )
#define DTL_SECS_PER_MINUTE             ( 60 )

#define DTL_USECS_PER_DAY	        STL_INT64_C(86400000000)
#define DTL_USECS_PER_HOUR	        STL_INT64_C(3600000000)
#define DTL_USECS_PER_MINUTE            STL_INT64_C(60000000)
#define DTL_USECS_PER_SEC	        STL_USEC_PER_SEC
#define DTL_USECS_PER_MILLISEC	        STL_INT64_C(1000)
#define DTL_MINS_PER_HOUR               (60)

#define DTL_MONTHS_PER_YEAR             (12)
#define DTL_MONTHS_PER_QUARTER          (3)
#define DTL_DAYS_PER_MONTH              (30)
#define DTL_DAYS_PER_WEEK               (7)
#define DTL_HOURS_PER_DAY               (24)

/* time type fractional second precision */
#define DTL_TIME_MIN_PRECISION          (0)
#define DTL_TIME_MAX_PRECISION          (6)

/* timeTz type fractional second precision */
#define DTL_TIMETZ_MIN_PRECISION        (0)
#define DTL_TIMETZ_MAX_PRECISION        (6)

/* timestamp type fractional second precision */
#define DTL_TIMESTAMP_MIN_PRECISION     (0)
#define DTL_TIMESTAMP_MAX_PRECISION     (6)

/* timestampTz type fractional second precision */
#define DTL_TIMESTAMPTZ_MIN_PRECISION   (0)
#define DTL_TIMESTAMPTZ_MAX_PRECISION   (6)

/* interval type leading field precision */
#define DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION      (2)
#define DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION      (6)

/* interval type fractional second precision */
#define DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION  (0)
#define DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION  (6)

#define DTL_DATETIME_FIELD_FRACTIONAL_SECOND_EXPONENT  (3)

/**
 * dtlExpTime init
 */
#define DTL_INIT_DTLEXPTIME( aDtlExpTime )      \
    {                                           \
        (aDtlExpTime).mSecond     = 0;          \
        (aDtlExpTime).mMinute     = 0;          \
        (aDtlExpTime).mHour       = 0;          \
        (aDtlExpTime).mDay        = 0;          \
        (aDtlExpTime).mMonth      = 0;          \
        (aDtlExpTime).mYear       = 0;          \
        (aDtlExpTime).mWeekDay    = 0;          \
        (aDtlExpTime).mYearDay    = 0;          \
        (aDtlExpTime).mIsDaylightSaving = 0;    \
        (aDtlExpTime).mTimeGmtOff = 0;          \
        (aDtlExpTime).mTimeZone   = NULL;       \
    }

/**
 * String definitions 
 */

#define DTL_DAGO                        "ago"
#define DTL_DCURRENT                    "current"
#define DTL_EPOCH                       "epoch"
#define DTL_INVALID                     "invalid"
#define DTL_EARLY                       "-infinity"
#define DTL_LATE                        "infinity"
#define DTL_NOW                         "now"
#define DTL_TODAY                       "today"
#define DTL_TOMORROW                    "tomorrow"
#define DTL_YESTERDAY                   "yesterday"
#define DTL_ZULU                        "zulu"

#define DTL_DMICROSEC                   "usecond"
#define DTL_DMILLISEC                   "msecond"
#define DTL_DSECOND                     "second"
#define DTL_DMINUTE                     "minute"
#define DTL_DHOUR                       "hour"
#define DTL_DDAY                        "day"
#define DTL_DWEEK                       "week"
#define DTL_DMONTH                      "month"
#define DTL_DQUARTER                    "quarter"
#define DTL_DYEAR                       "year"
#define DTL_DDECADE                     "decade"
#define DTL_DCENTURY                    "century"
#define DTL_DMILLENNIUM                 "millennium"
#define DTL_DA_D                        "ad"
#define DTL_DB_C                        "bc"
#define DTL_DTIMEZONE                   "timezone"

/**
 * Parsing을 위한 time field definitions
 */

#define DTL_AM		(0)
#define DTL_PM		(1)
#define DTL_HR24	(2)

#define DTL_AD		(0)
#define DTL_BC		(1)

/**
 * time decoding을 위한 Fields
 */

#define DTL_RESERV               (0)
#define DTL_MONTH                (1)
#define DTL_YEAR                 (2)
#define DTL_DAY                  (3)
#define DTL_JULIAN               (4)
#define DTL_TZ                   (5)
#define DTL_DTZ                  (6)
#define DTL_DTZMOD               (7)
#define DTL_IGNORE_DTF           (8)
#define DTL_AMPM                 (9)
#define DTL_HOUR                (10)
#define DTL_MINUTE              (11)
#define DTL_SECOND              (12)
#define DTL_MILLISECOND         (13)
#define DTL_MICROSECOND         (14)
#define DTL_DOY                 (15)
#define DTL_DOW                 (16)
#define DTL_UNITS               (17)
#define DTL_ADBC                (18)
/* these are only for relative dates */
#define DTL_AGO                 (19)
#define DTL_ABS_BEFORE          (20)
#define DTL_ABS_AFTER           (21)
/* generic fields to help with parsing */
#define DTL_ISODATE             (22)
#define DTL_ISOTIME             (23)
/* these are only for parsing intervals */
#define DTL_WEEK                (24)
#define DTL_DECADE              (25)
#define DTL_CENTURY             (26)
#define DTL_MILLENNIUM          (27)
/* reserved for unrecognized string values */
#define DTL_UNKNOWN_FIELD       (31)

/**
 * DATE/TIME parsing token define
 * @todo 제거되어야 함.
 */

#define DTL_DATES_STYLE_ISO      (0)       /* default */
#define DTL_DATES_STYLE_SQL      (1)

#define DTL_INTERVAL_STYLE_ISO         (0) 
#define DTL_INTERVAL_STYLE_SQLSTANDARD (1) /* default */

#define DTL_DATE_ORDER_YMD       (0)       /* default */
#define DTL_DATE_ORDER_DMY       (1)
#define DTL_DATE_ORDER_MDY       (2)

/**
 * @brief time parsing과 decoding을 위한 token field 정의.
 */
#define DTL_DTK_NUMBER           (0) 
#define DTL_DTK_STRING           (1)

#define DTL_DTK_DATE             (2)
#define DTL_DTK_TIME             (3)
#define DTL_DTK_TZ               (4)
#define DTL_DTK_AGO              (5)

#define DTL_DTK_SPECIAL          (6)
#define DTL_DTK_EARLY            (7)
#define DTL_DTK_LATE             (8)
#define DTL_DTK_EPOCH            (9)
#define DTL_DTK_NOW             (10)
#define DTL_DTK_YESTERDAY       (11)
#define DTL_DTK_TODAY           (12)
#define DTL_DTK_TOMORROW        (13)
#define DTL_DTK_ZULU            (14)

#define DTL_DTK_DELTA           (15)
#define DTL_DTK_SECOND          (16)
#define DTL_DTK_MINUTE          (17)
#define DTL_DTK_HOUR            (18)
#define DTL_DTK_DAY             (19)
#define DTL_DTK_WEEK            (20)
#define DTL_DTK_MONTH           (21)
#define DTL_DTK_QUARTER         (22)
#define DTL_DTK_YEAR            (23)
#define DTL_DTK_DECADE          (24)
#define DTL_DTK_CENTURY         (25)
#define DTL_DTK_MILLENNIUM      (26)
#define DTL_DTK_MILLISEC        (27)
#define DTL_DTK_MICROSEC        (28)
#define DTL_DTK_JULIAN          (29)

#define DTL_DTK_DOW             (30)
#define DTL_DTK_DOY             (31)
#define DTL_DTK_TZ_HOUR         (32)
#define DTL_DTK_TZ_MINUTE       (33)
#define DTL_DTK_ISOYEAR         (34)
#define DTL_DTK_ISODOW          (35)


#define DTL_DTK_M(t)    (0x01 << (t))
#define DTL_DTK_ALL_SECS_M (DTL_DTK_M(DTL_SECOND) |             \
                            DTL_DTK_M(DTL_MILLISECOND) |        \
                            DTL_DTK_M(DTL_MICROSECOND))
#define DTL_DTK_DATE_M (DTL_DTK_M(DTL_YEAR) | DTL_DTK_M(DTL_MONTH) | DTL_DTK_M(DTL_DAY))
#define DTL_DTK_TIME_M (DTL_DTK_M(DTL_HOUR) | DTL_DTK_M(DTL_MINUTE) | DTL_DTK_ALL_SECS_M)

#define DTL_MAX_DATE_FIELDS      (25)
#define DTL_DATE_TOKEN_MAX_LEN   (12)

#define DTL_MAX_TIMEZONE_LEN     (10)

/* 예) +999999-11 */
#define DTL_INTERVAL_YEAR_TO_MONTH_MAX_STRING_SIZE  ( 10 )
/* 예) +999999 23:59:59.999999 */
#define DTL_INTERVAL_DAY_TO_SECOND_MAX_STRING_SIZE  ( 23 )

/**
 * 윤년 체크 
 */ 
#define DTL_IS_LEAP(year) ( ((year) % 4 == 0) && ((((year) % 100) != 0) || (((year) % 400) == 0)) )

typedef struct dtlDateToken
{
    stlChar  mToken[DTL_DATE_TOKEN_MAX_LEN];
    stlChar  mType;
    stlChar  mValue;
} dtlDateToken;

extern const dtlDateToken dtlDateTokenTbl[];
extern const stlInt32 dtlDateTokenTblSize;

extern const dtlDateToken * dtlDateTokenCache[];

extern const dtlDateToken dtlDateDeltaTokenTbl[];
extern const stlInt32 dtlDateDeltaTokenTblSize;

extern const dtlDateToken * dtlDateDeltaCache[];

extern const stlInt32 dtlDayTab[2][13];

extern const stlUInt16 dtlSumDayOfYear[2][13];

/**
 * Julian date
 */ 
#define DTL_JULIAN_MINYEAR   (-4713)
#define DTL_JULIAN_MINMONTH  (11)
#define DTL_JULIAN_MINDAY    (24)
#define DTL_JULIAN_MAXYEAR   (9999)
#define DTL_JULIAN_MINJULDAY (0)         /* 4714-11-24 BC */
#define DTL_JULIAN_MAXJULDAY (5373484)   /* 9999-12-31 */

#define DTL_JULIAN_MAX_TOTAL_MONTH  ( 176545 )

#define DTL_IS_VALID_JULIAN(y,m,d)                                              \
    ( (((y) > DTL_JULIAN_MINYEAR)                                               \
       ||                                                                       \
       (((y) == DTL_JULIAN_MINYEAR) &&                                          \
        (((m) > DTL_JULIAN_MINMONTH) ||                                         \
         (((m) == DTL_JULIAN_MINMONTH) && ((d) >= DTL_JULIAN_MINDAY)))))        \
      &&                                                                        \
      ((y) <= DTL_JULIAN_MAXYEAR) )

#define DTL_TIMESTAMP_MODULO( t, q, u )         \
    do {                                        \
        (q) = ((t) / (u));                      \
        if((q) != 0) (t) -= ((q) * (u));        \
    } while(0)

#define DTL_GET_DATE_TIME_FROM_TIMESTAMP( aTime, aDate, aUsec ) \
    do {                                                        \
        DTL_TIMESTAMP_MODULO( (aTime), (aDate), (aUsec) );      \
        if( (aTime) < STL_INT64_C(0) )                          \
        {                                                       \
            (aTime) += DTL_USECS_PER_DAY;                       \
            (aDate) -= 1;                                       \
        }                                                       \
        else                                                    \
        {                                                       \
            /* Do Nothing */                                    \
        }                                                       \
                                                                \
    } while(0)

/** 입력받은 년도로부터 내부저장구조인 dtlExpTime의 year 값을 구한다. */
#define DTL_GET_DTLEXPTIME_YEAR_FROM_YEAR( aYear )      \
    ( (aYear) < 0 ? ((aYear) + 1) : (aYear) )

/**
 * @brief 입력받은 timestamp with time zone value를 timezone이 적용된 local timestamp 값으로 변환한다.
 * @param[in]  aTimestampTz
 * @param[out] aLocalTimestamp
 */
#define DTL_GET_LOCAL_TIMESTAMP_FROM_TIMESTAMPTZ( aTimestampTz, aLocalTimestamp )                   \
    {                                                                                               \
        (aLocalTimestamp) = (aTimestampTz)->mTimestamp - ((aTimestampTz)->mTimeZone * DTL_USECS_PER_SEC); \
    }

/** Not Available Length Value */
#define DTL_LENGTH_NA    (STL_INT64_MIN)

/** Not Available Precision Value */
#define DTL_PRECISION_NA (STL_INT32_MIN)

/** Not Available Scale Value */
#define DTL_SCALE_NA     (STL_INT32_MIN)

#define DTL_SCALE_STRING_NA     ("-2147483648")
#define DTL_NUMERIC_STRING_RADIX_BINARY ("2")

/** BOOLEAN string */
#define DTL_BOOLEAN_STRING_TRUE         ("TRUE")
#define DTL_BOOLEAN_STRING_FALSE        ("FALSE")
#define DTL_BOOLEAN_LOWER_STRING_TRUE   ("true")
#define DTL_BOOLEAN_LOWER_STRING_FALSE  ("false")
#define DTL_BOOLEAN_STRING_TRUE_SIZE    ( 4 )
#define DTL_BOOLEAN_STRING_FALSE_SIZE   ( 5 )
#define DTL_BOOLEAN_STRING_SIZE         ( 6 )

/** C type's maximum digit count */
#define DTL_INT8_MAX_DIGIT_COUNT        ( 3 )
#define DTL_UINT8_MAX_DIGIT_COUNT       ( 3 )
#define DTL_INT16_MAX_DIGIT_COUNT       ( 5 )
#define DTL_UINT16_MAX_DIGIT_COUNT      ( 5 )
#define DTL_INT32_MAX_DIGIT_COUNT       ( 10 )
#define DTL_UINT32_MAX_DIGIT_COUNT      ( 10 )
#define DTL_INT64_MAX_DIGIT_COUNT       ( 19 )
#define DTL_UINT64_MAX_DIGIT_COUNT      ( 20 )

/** C type's float expression : 'E' exponent + sign + digit */
#define DTL_FLOAT32_EXPONENT_SIZE ( 4 )
#define DTL_FLOAT64_EXPONENT_SIZE ( 5 )

/** C type's string length : sign + digit + decimal point + null terminator */
#define DTL_INT8_STRING_SIZE         ( DTL_INT8_MAX_DIGIT_COUNT + 2 )
#define DTL_UINT8_STRING_SIZE        ( DTL_UINT8_MAX_DIGIT_COUNT + 1 )
#define DTL_INT16_STRING_SIZE        ( DTL_INT16_MAX_DIGIT_COUNT + 2 )
#define DTL_UINT16_STRING_SIZE       ( DTL_UINT16_MAX_DIGIT_COUNT + 1 )
#define DTL_INT32_STRING_SIZE        ( DTL_INT32_MAX_DIGIT_COUNT + 2 )
#define DTL_UINT32_STRING_SIZE       ( DTL_UINT32_MAX_DIGIT_COUNT + 1 )
#define DTL_INT64_STRING_SIZE        ( DTL_INT64_MAX_DIGIT_COUNT + 2 )
#define DTL_UINT64_STRING_SIZE       ( DTL_UINT64_MAX_DIGIT_COUNT + 1 )
#define DTL_FLOAT32_STRING_SIZE      ( STL_FLT_DIG + DTL_FLOAT32_EXPONENT_SIZE + 3 )   /* Exponent expression */
#define DTL_FLOAT64_STRING_SIZE      ( STL_DBL_DIG + DTL_FLOAT64_EXPONENT_SIZE + 3 )   /* Exponent expression */

/**
 * DB type's string length : sign + digit + decimal point + null terminator
 */
#define DTL_NATIVE_SMALLINT_STRING_SIZE     ( DTL_INT16_STRING_SIZE )
#define DTL_NATIVE_INTEGER_STRING_SIZE      ( DTL_INT32_STRING_SIZE )
#define DTL_NATIVE_BIGINT_STRING_SIZE       ( DTL_INT64_STRING_SIZE )
#define DTL_NATIVE_REAL_STRING_SIZE         ( DTL_FLOAT32_STRING_SIZE )
#define DTL_NATIVE_DOUBLE_STRING_SIZE       ( DTL_FLOAT64_STRING_SIZE )
#define DTL_NUMERIC_STRING_MAX_SIZE         ( 46 )  /* exponent 'E' + sign + value(0~500) */

/**
 * @todo INIT 크기로 설정해야 함.
 * @note Long Varying Type의 초기 사이즈는 페이지사이즈보다 작을수 없다.
 */
#define DTL_LONGVARYING_INIT_STRING_SIZE    (8192)

#define DTL_CHAR_STRING_COLUMN_SIZE(aPrecision, aVectorFunc, aVectorArg)                \
    ( (aPrecision) * dtlGetMbMaxLength((aVectorFunc)->mGetCharSetIDFunc(aVectorArg)) )
#define DTL_BINARY_STRING_COLUMN_SIZE(aPrecision) ( aPrecision )

/**
 * @brief List function predicate
 * List function에서 사용하는 right / left는 list column의 대표 column number이다.
 * List function에서 사용하는 List column이 증가하면 predicate_function 상위에 적용하면 된다.
 */ 
typedef enum dtlListPredicate
{
    DTL_LIST_PREDICATE_RIGHT = 0,             /**< RIGHT COLUMN */
    DTL_LIST_PREDICATE_LEFT  = 1,             /**< LEFT COLUMN */
    DTL_LIST_PREDICATE_FUNCTION,              /**< LIST_FUNCTION */ 
    
    DTL_LIST_PREDICATE_NA,                    /**< N/A */
    
} dtlListPredicate;

/**
 * @brief String 의 길이 단위
 */
typedef enum dtlStringLengthUnit
{
    DTL_STRING_LENGTH_UNIT_NA = 0,            /**< N/A */
    DTL_STRING_LENGTH_UNIT_CHARACTERS,        /**< 길이 단위가 CHARACTER */
    DTL_STRING_LENGTH_UNIT_OCTETS,            /**< 길이 단위가 BYTE */
    
    DTL_STRING_LENGTH_UNIT_MAX
} dtlStringLengthUnit;

extern stlChar * gDtlLengthUnitString[DTL_STRING_LENGTH_UNIT_MAX];

/**
 * @brief Length Unit Map
 */
typedef struct dtlLengthUnitMap
{
    const stlChar             * mName;
    const dtlStringLengthUnit   mLengthUnit;
} dtlLengthUnitMap;

/**
 * @brief 숫자형 타입의 Precision 의 Radix 정보
 */
typedef enum dtlNumericPrecRadix
{
    DTL_NUMERIC_PREC_RADIX_NA = 0,             /**< N/A */
    DTL_NUMERIC_PREC_RADIX_BINARY  = 2,        /**< 2진수 */
    DTL_NUMERIC_PREC_RADIX_DECIMAL = 10        /**< 10진수 */
} dtlNumericPrecRadix;

typedef enum dtlSearchable
{
    DTL_SEARCHABLE_PRED_NONE = 0,  /**< unsearchable */
    DTL_SEARCHABLE_PRED_CHAR,      /**< like only */
    DTL_SEARCHABLE_PRED_BASIC,     /**< all except like */
    DTL_SEARCHABLE_PRED_SEARCHABLE /**< all */
} dtlSearchable;

/**
 * @brief Data Type Definition
 */
typedef struct dtlDataTypeDefinition
{
    stlInt32  mEnumeratedDataTypeID;     /**< 내부 Data Type ID */
    stlInt32  mODBCDataTypeID;           /**< ODBC Data Type ID */
    stlInt32  mJDBCDataTypeID;           /**< JDBC Data Type ID */
    
    stlChar * mSqlNormalTypeName;        /**< SQL 표준 Data Type Name */
    
    stlInt64  mMaxStringLength;          /**< String Maximum Length */
    stlInt64  mMinStringLength;          /**< String Minimum Length */
    stlInt64  mDefStringLength;          /**< String Default Length */

    dtlSearchable mSearchable;           /**< 검색 가능 여부 */
    
    dtlNumericPrecRadix mDefaultNumericPrecRadix; /**< Numeric의 기본 Radix */
    
    stlInt32  mMaxNumericPrec;           /**< Numeric Maximum Precision */
    stlInt32  mMinNumericPrec;           /**< Numeric Minimum Precision */
    stlInt32  mDefNumericPrec;           /**< Numeric Default Precision */

    stlInt32  mMaxNumericScale;          /**< Numeric Maximum Scale */
    stlInt32  mMinNumericScale;          /**< Numeric Minimum Scale */
    stlInt32  mDefNumericScale;          /**< Numeric Default Scale */

    stlInt32  mMaxFractionalSecondPrec;  /**< Fractional Second Max Precision */
    stlInt32  mMinFractionalSecondPrec;  /**< Fractional Second Min Precision */
    stlInt32  mDefFractionalSecondPrec;  /**< Fractional Second Default */
    
    stlInt32  mMaxIntervalPrec;          /**< Interval Max Precision */
    stlInt32  mMinIntervalPrec;          /**< Interval Min Precision */
    stlInt32  mDefIntervalPrec;          /**< Non-Second Time Default Precision */
    
    stlBool   mSupportedFeature;         /**< 지원 여부 */
} dtlDataTypeDefinition;

/**
 * @brief Pre-defined Data Type Definition
 */
extern dtlDataTypeDefinition gIntervalYearToMonthDataTypeDefinition[DTL_INTERVAL_YEAR_TO_MONTH_TYPE_COUNT];
extern dtlDataTypeDefinition gIntervalDayToSecondDataTypeDefinition[DTL_INTERVAL_DAY_TO_SECOND_TYPE_COUNT];

/**
 * @brief Pre-defined Data Type Definition
 */
extern dtlDataTypeDefinition gDataTypeDefinition[DTL_TYPE_MAX];


/**
 * @brief 각 data type의 max precision을 고려한 최대 버퍼 크기
 */
extern const stlUInt32 dtlDataTypeMaxBufferSize[ DTL_TYPE_MAX ];


/**
 * data type definition
 */

typedef struct dtlDataTypeDefInfo
{
    stlInt64             mArgNum1;           /**< 첫번째 숫자 (TYPE 마다 용도가 다름) */
    stlInt64             mArgNum2;           /**< 두번째 숫자 (TYPE 마다 용도가 다름) */
    dtlStringLengthUnit  mStringLengthUnit;  /**< only for STRING 계열 Type */
    dtlIntervalIndicator mIntervalIndicator; /**< only for INTERVAL Type */

    stlInt64             mDataValueSize;     /**< value buffer size */
    stlInt64             mStringBufferSize;  /**< string buffer size */

} dtlDataTypeDefInfo;

/*
 * init data type definition
 */

#define DTL_INIT_DATA_TYPE_DEF_INFO( _aDataTypeDefInfo )                        \
    {                                                                           \
        (_aDataTypeDefInfo)->mArgNum1           = DTL_PRECISION_NA;             \
        (_aDataTypeDefInfo)->mArgNum2           = DTL_SCALE_NA;                 \
        (_aDataTypeDefInfo)->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;    \
        (_aDataTypeDefInfo)->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;    \
        (_aDataTypeDefInfo)->mDataValueSize     = DTL_LENGTH_NA;                \
        (_aDataTypeDefInfo)->mStringBufferSize  = DTL_LENGTH_NA;                \
    }                                                                           \
                                                                                \

/**
 * _aDest ( dtlDataTypeDefInfo* )
 * _aSrc  ( dtlDataTypeDefInfo* )
 */
#define DTL_COPY_DATA_TYPE_DEF_INFO( _aDest, _aSrc )                    \
    {                                                                   \
        (_aDest)->mArgNum1           = (_aSrc)->mArgNum1;               \
        (_aDest)->mArgNum2           = (_aSrc)->mArgNum2;               \
        (_aDest)->mStringLengthUnit  = (_aSrc)->mStringLengthUnit;      \
        (_aDest)->mIntervalIndicator = (_aSrc)->mIntervalIndicator;     \
        (_aDest)->mDataValueSize     = (_aSrc)->mDataValueSize;         \
        (_aDest)->mStringBufferSize  = (_aSrc)->mStringBufferSize;      \
    }                                                                   \

/**
 * default data type definition
 */
extern const dtlDataTypeDefInfo gDefaultDataTypeDef[ DTL_TYPE_MAX ];


/**
 * data type definition for function result
 */
extern const dtlDataTypeDefInfo gResultDataTypeDef[ DTL_TYPE_MAX ];


/**
 * data type info domain
 */

typedef struct dtlDataTypeInfoDomain
{
    stlInt64             mMinArgNum1;           /**< 첫번째 숫자 (TYPE 마다 용도가 다름) : 최소값 */
    stlInt64             mMinArgNum2;           /**< 두번째 숫자 (TYPE 마다 용도가 다름) : 최소값 */
    dtlStringLengthUnit  mMinStringLengthUnit;  /**< only for STRING 계열 Type : 최소값 */
    dtlIntervalIndicator mMinIntervalIndicator; /**< only for INTERVAL Type : 최소값 */

    stlInt64             mMaxArgNum1;           /**< 첫번째 숫자 (TYPE 마다 용도가 다름) : 최대값 */
    stlInt64             mMaxArgNum2;           /**< 두번째 숫자 (TYPE 마다 용도가 다름) : 최대값 */
    dtlStringLengthUnit  mMaxStringLengthUnit;  /**< only for STRING 계열 Type : 최대값 */
    dtlIntervalIndicator mMaxIntervalIndicator; /**< only for INTERVAL Type : 최대값 */
} dtlDataTypeInfoDomain;


/**
 * data type info domain definition
 */
extern const dtlDataTypeInfoDomain gDataTypeInfoDomain[ DTL_TYPE_MAX ];


/**
 * data type info
 */
typedef struct dtlDataTypeInfo
{
    dtlDataType          mDataType;
    
    stlInt64             mArgNum1;           /**< 첫번째 숫자 (TYPE 마다 용도가 다름) */
    stlInt64             mArgNum2;           /**< 두번째 숫자 (TYPE 마다 용도가 다름) */
    stlInt64             mArgNum3;           /**< 세번째 숫자 (TYPE 마다 용도가 다름) */
} dtlDataTypeInfo;


/**
 * Offset for Memory Compare
 */
extern const stlInt32 gDtlCompValueIgnoreSize[ DTL_TYPE_MAX ];


/** @} dtlDataTypeInformation */




/**
 * @defgroup dtlODBCColumnSize ODBC Column Size 정의
 * @ingroup dtlDataTypeInformation
 * @{
 */
/* #define DTL_BOOLEAN_ODBC_COLUMN_SIZE                   ( DTL_BOOLEAN_STRING_SIZE ) */

/* #define DTL_NATIVE_SMALLINT_ODBC_COLUMN_SIZE           ( DTL_INT16_MAX_DIGIT_COUNT ) */
/* #define DTL_NATIVE_INTEGER_ODBC_COLUMN_SIZE            ( DTL_INT32_MAX_DIGIT_COUNT ) */
/* #define DTL_NATIVE_BIGINT_ODBC_COLUMN_SIZE             ( DTL_INT64_MAX_DIGIT_COUNT ) */

/* #define DTL_NATIVE_REAL_ODBC_COLUMN_SIZE               ( STL_FLT_DIG )  */
/* #define DTL_NATIVE_DOUBLE_ODBC_COLUMN_SIZE             ( STL_DBL_DIG ) */

/* #define DTL_NUMERIC_ODBC_COLUMN_SIZE(aPrecision)       ( aPrecision ) */

/* #define DTL_CHAR_ODBC_COLUMN_SIZE(aPrecision, aVectorFunc, aVectorArg)          \ */
/*     ( DTL_CHAR_STRING_COLUMN_SIZE(aPrecision, aVectorFunc, aVectorArg) ) */
/* #define DTL_VARCHAR_ODBC_COLUMN_SIZE(aPrecision, aVectorFunc, aVectorArg)       \ */
/*     ( DTL_CHAR_STRING_COLUMN_SIZE(aPrecision, aVectorFunc, aVectorArg) ) */
/* #define DTL_LONGVARCHAR_ODBC_COLUMN_SIZE(aPrecision, aVectorFunc, aVectorArg)   \ */
/*     ( DTL_CHAR_STRING_COLUMN_SIZE(aPrecision, aVectorFunc, aVectorArg) ) */

/* #define DTL_BINARY_ODBC_COLUMN_SIZE(aPrecision)        ( DTL_BINARY_STRING_COLUMN_SIZE(aPrecision) ) */
/* #define DTL_VARBINARY_ODBC_COLUMN_SIZE(aPrecision)     ( DTL_BINARY_STRING_COLUMN_SIZE(aPrecision) ) */
/* #define DTL_LONGVARBINARY_ODBC_COLUMN_SIZE(aPrecision) ( DTL_BINARY_STRING_COLUMN_SIZE(aPrecision) ) */

/*
 * date format 에 따라 odbc column size 가 결정된다.
 */
#define DTL_DATE_ODBC_COLUMN_SIZE( aVectorFunc, aVectorArg ) \
    ( ((aVectorFunc)->mGetDateFormatInfoFunc((aVectorArg)))->mToCharMaxResultLen )

/*
 * time format 에 따라 odbc column size 가 결정된다.
 */
#define DTL_TIME_ODBC_COLUMN_SIZE( aVectorFunc, aVectorArg ) \
    ( (aVectorFunc)->mGetTimeFormatInfoFunc( (aVectorArg) )->mToCharMaxResultLen )

/*
 * timestamp format 에 따라 odbc column size 가 결정된다.
 */ 
#define DTL_TIMESTAMP_ODBC_COLUMN_SIZE( aVectorFunc, aVectorArg ) \
    ( (aVectorFunc)->mGetTimestampFormatInfoFunc( (aVectorArg) )->mToCharMaxResultLen )

/*
 * time with time zone format 에 따라 odbc column size 가 결정된다.
 */
#define DTL_TIME_WITH_TIME_ZONE_ODBC_COLUMN_SIZE( aVectorFunc, aVectorArg )  \
    ( (aVectorFunc)->mGetTimeWithTimeZoneFormatInfoFunc( (aVectorArg) )->mToCharMaxResultLen )

/*
 * timestamp with time zone format 에 따라 odbc column size 가 결정된다.
 */
#define DTL_TIMESTAMP_WITH_TIME_ZONE_ODBC_COLUMN_SIZE( aVectorFunc, aVectorArg )  \
    ( (aVectorFunc)->mGetTimestampWithTimeZoneFormatInfoFunc( (aVectorArg) )->mToCharMaxResultLen )

/*
 * INTERVAL
 * YEAR-MONTH format : [+ |-]YYYYYY-MM
 * DAY TIME format   : [+ |-]DDDDDD HH:MI:SS.ffffff
 */ 

/*
 * YEAR-MONTH format : [+ |-]YYYYYY-MM 
 * ( 4 + (aLeadingPrecision) ) 
 * 4 : sign(1) + -(1) + MM(2)
 */
#define DTL_INTERVAL_YEAR_ODBC_COLUMN_SIZE( aLeadingPrecision )          \
    ( DTL_INTERVAL_YEAR_TO_MONTH_DISPLAY_SIZE( (aLeadingPrecision) ) )

#define DTL_INTERVAL_MONTH_ODBC_COLUMN_SIZE( aLeadingPrecision )         \
    ( DTL_INTERVAL_YEAR_TO_MONTH_DISPLAY_SIZE( (aLeadingPrecision) ) )

#define DTL_INTERVAL_YEAR_TO_MONTH_ODBC_COLUMN_SIZE( aLeadingPrecision ) \
    ( DTL_INTERVAL_YEAR_TO_MONTH_DISPLAY_SIZE( (aLeadingPrecision) ) )


/*
 * DAY TIME format   : [+ |-]DDDDDD HH:MI:SS.ffffff
 * 
 * (9 + (aLeadingPrecision))
 * 9 : sign(1) + space(1) + HH:MI:SS(8)
 * 
 * (11 + (aLeadingPrecision) + (aSecondPrecision))
 * 11 : sign(1) + space(1) + HH:MI:SS.(9)
 */ 

#define DTL_INTERVAL_DAY_ODBC_COLUMN_SIZE( aLeadingPrecision )    \
    ( DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( (aLeadingPrecision), 0, DTL_INTERVAL_INDICATOR_DAY ) )

#define DTL_INTERVAL_HOUR_ODBC_COLUMN_SIZE( aLeadingPrecision )   \
    ( DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( (aLeadingPrecision), 0, DTL_INTERVAL_INDICATOR_HOUR ) )

#define DTL_INTERVAL_MINUTE_ODBC_COLUMN_SIZE( aLeadingPrecision ) \
    ( DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( (aLeadingPrecision), 0, DTL_INTERVAL_INDICATOR_MINUTE ) )

#define DTL_INTERVAL_SECOND_ODBC_COLUMN_SIZE( aLeadingPrecision, aFracSecondPrecision )            \
    ( DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( (aLeadingPrecision), (aFracSecondPrecision), DTL_INTERVAL_INDICATOR_SECOND ) )

#define DTL_INTERVAL_DAY_TO_HOUR_ODBC_COLUMN_SIZE( aLeadingPrecision )    \
    ( DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( (aLeadingPrecision), 0, DTL_INTERVAL_INDICATOR_DAY_TO_HOUR ) )

#define DTL_INTERVAL_DAY_TO_MINUTE_ODBC_COLUMN_SIZE( aLeadingPrecision )  \
    ( DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( (aLeadingPrecision), 0, DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE ) )

#define DTL_INTERVAL_DAY_TO_SECOND_ODBC_COLUMN_SIZE( aLeadingPrecision, aFracSecondPrecision ) \
    ( DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( (aLeadingPrecision), (aFracSecondPrecision), DTL_INTERVAL_INDICATOR_DAY_TO_SECOND ) )

#define DTL_INTERVAL_HOUR_TO_MINUTE_ODBC_COLUMN_SIZE( aLeadingPrecision ) \
    ( DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( (aLeadingPrecision), 0, DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE ) )

#define DTL_INTERVAL_HOUR_TO_SECOND_ODBC_COLUMN_SIZE( aLeadingPrecision, aFracSecondPrecision )    \
    ( DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( (aLeadingPrecision), (aFracSecondPrecision), DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND ) )

#define DTL_INTERVAL_MINUTE_TO_SECOND_ODBC_COLUMN_SIZE( aLeadingPrecision, aFracSecondPrecision )  \
    ( DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( (aLeadingPrecision), (aFracSecondPrecision), DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND ) )

/** @} dtlODBCColumnSize */

/**
 * @defgroup dtlODBCDisplaySize Display Size 정의 
 * @ingroup dtlDataTypeInformation
 * @{
 */
#define DTL_NO_TOTAL_SIZE         ( 0x7FFFFFFF )
#define DTL_BOOLEAN_DISPLAY_SIZE  DTL_BOOLEAN_STRING_SIZE

/* DTL_DTL_ODBC_COLUMN_SIZE + 1 (sign) */
#define DTL_NATIVE_SMALLINT_DISPLAY_SIZE ( DTL_INT16_MAX_DIGIT_COUNT + 1 )
#define DTL_NATIVE_INTEGER_DISPLAY_SIZE  ( DTL_INT32_MAX_DIGIT_COUNT + 1 )
#define DTL_NATIVE_BIGINT_DISPLAY_SIZE   ( DTL_INT64_MAX_DIGIT_COUNT + 1 )

/* real : (a sign, STL_FLT_DIG, a decimal point, the letter E, a sign, and 2 digits) */
#define DTL_NATIVE_REAL_DISPLAY_SIZE   ( STL_FLT_DIG + DTL_FLOAT32_EXPONENT_SIZE + 2 )
/* double : (a sign, STL_DBL_DIG, a decimal point, the letter E, a sign, and 3 digits) */
#define DTL_NATIVE_DOUBLE_DISPLAY_SIZE ( STL_DBL_DIG + DTL_FLOAT64_EXPONENT_SIZE + 2 )

/*
 * Precision, Scale에 따라
 * numeric : aPrecision + sign + decimal point + the letter E + a sign, and 3 digits
 */
#define DTL_NUMERIC_DISPLAY_SIZE(aPrecision) ( aPrecision + 7 )
/* float */
#define DTL_FLOAT_DISPLAY_SIZE(aPrecision) ( dtlBinaryToDecimalPrecision[(aPrecision)] + 7 )

#define DTL_CHAR_DISPLAY_SIZE(aPrecision, aVectorFunc, aVectorArg)      \
    ( DTL_CHAR_STRING_COLUMN_SIZE(aPrecision, aVectorFunc, aVectorArg) )
#define DTL_VARCHAR_DISPLAY_SIZE(aPrecision, aVectorFunc, aVectorArg)   \
    ( DTL_CHAR_STRING_COLUMN_SIZE(aPrecision, aVectorFunc, aVectorArg) )

/* binary column size * 2 (Each binary byte is represented by a 2-digit hexadecimal number.)  */
#define DTL_BINARY_DISPLAY_SIZE(aPrecision)     ( DTL_BINARY_STRING_COLUMN_SIZE(aPrecision) * 2 )
#define DTL_VARBINARY_DISPLAY_SIZE(aPrecision)  ( DTL_BINARY_STRING_COLUMN_SIZE(aPrecision) * 2 )

/* DTL_DATE_DISPLAY_SIZE : date format 형식에 따라 */
#define DTL_DATE_DISPLAY_SIZE( aVectorFunc, aVectorArg ) \
    ( DTL_DATE_ODBC_COLUMN_SIZE( (aVectorFunc), (aVectorArg) ) )

/* DTL_TIME_DISPLAY_SIZE : time format 형식에 따라 */
#define DTL_TIME_DISPLAY_SIZE( aVectorFunc, aVectorArg ) \
    ( DTL_TIME_ODBC_COLUMN_SIZE( (aVectorFunc), (aVectorArg) ) )

/* DTL_TIMESTAMP_DISPLAY_SIZE : timestamp format 형식에 따라 */
#define DTL_TIMESTAMP_DISPLAY_SIZE( aVectorFunc, aVectorArg ) \
    ( DTL_TIMESTAMP_ODBC_COLUMN_SIZE( (aVectorFunc), (aVectorArg) ) )

/*
 * DTL_TIME_WITH_TIME_ZONE_DISPLAY_SIZE : time with time zone format 형식에 따라 */
#define DTL_TIME_WITH_TIME_ZONE_DISPLAY_SIZE( aVectorFunc, aVectorArg ) \
    ( DTL_TIME_WITH_TIME_ZONE_ODBC_COLUMN_SIZE( (aVectorFunc), (aVectorArg) ) )

/*
 * DTL_TIMESTAMP_WITH_TIME_ZONE_DISPLAY_SIZE : timestamp with time zone format 형식에 따라 */
#define DTL_TIMESTAMP_WITH_TIME_ZONE_DISPLAY_SIZE( aVectorFunc, aVectorArg ) \
    ( DTL_TIMESTAMP_WITH_TIME_ZONE_ODBC_COLUMN_SIZE( (aVectorFunc), (aVectorArg) ) )

/*
 * DTL_INTERVAL_YEAR_TO_MONTH_DISPLAY_SIZE : YEAR-MONTH
 * +999999-11 
 * leading precision 에 따른 display size 
 * ((aLeadingPrecision) + 4)
 * 4 = ( 부호(1)  +  -(1)  +  second field(2) ) 
 */
#define DTL_INTERVAL_YEAR_TO_MONTH_DISPLAY_SIZE( aLeadingPrecision ) \
    ( ( ((aLeadingPrecision) == DTL_PRECISION_NA) ? DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION : (aLeadingPrecision) ) + 4 )

/*
 * DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE : DAY HH24:MI:SS.FF6
 * +999999 11:22:33.123456
 * leading precision 과 fractional second precision 에 따른 display size
 */
#define DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( aLeadingPrecision, aFracSecondPrecision, aIntervalIndicator ) \
    ( ( ((aLeadingPrecision) == DTL_PRECISION_NA)                                             \
        ? DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION : (aLeadingPrecision) ) +                  \
      ( ( ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_DAY) ||                             \
          ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||                     \
          ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||                   \
          ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_HOUR) ||                            \
          ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||                  \
          ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_MINUTE) ) ? 0 :                     \
        ( ( (aFracSecondPrecision) == DTL_SCALE_NA ) &&                                       \
          ( ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||                 \
            ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||                \
            ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) ||              \
            ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_SECOND) ) )                       \
         ? DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION : (aFracSecondPrecision) ) +          \
      ( ( ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_DAY) ||                             \
          ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||                     \
          ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||                   \
          ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_HOUR) ||                            \
          ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||                  \
          ((aIntervalIndicator) == DTL_INTERVAL_INDICATOR_MINUTE) ) ? 10 :                    \
        ( (aFracSecondPrecision) == 0 ) ? 10 : 11 ) )

/*
 * DTL_INTERVAL_YEAR_TO_MONTH_MAX_DISPLAY_SIZE : YEAR-MONTH
 * +999999-11
 */
#define DTL_INTERVAL_YEAR_TO_MONTH_MAX_DISPLAY_SIZE    DTL_INTERVAL_YEAR_TO_MONTH_MAX_STRING_SIZE

/* @todo interval에 대한 정리 precision에 따른 odbc, dt format 정리 등... */
/*
 * DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE : DAY HH24:MI:SS.FF6
 * +999999 11:22:33.123456
 */ 
#define DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE    DTL_INTERVAL_DAY_TO_SECOND_MAX_STRING_SIZE

#define DTL_ROWID_DISPLAY_SIZE   ( DTL_ROWID_TO_STRING_LENGTH )

/** @} dtlODBCDisplaySize */

/**
 * @defgroup dtlDataValue  Data Value (DB data type을 갖는 value)
 * @ingroup dtExternal
 * @{
 */

/**
 * @brief Binary Data를 dtlDataType의 mValue로 내용 복사
 * [in/out]  aDstDataValue      copy되는 내용이 들어갈 dtlDataType ( dtlDataValue * )
 * [in]      aSrcValue          copy 대상이 되는 Binary Data
 * [in]      aValueLen          aSrcValue의 바이트 단위 길이
 */
#define DTL_COPY_VALUE( aDstDataValue, aSrcValue, aValueLen )                   \
    {                                                                           \
        stlInt64    tmpValueLen = 0;                                            \
        stlChar   * tmpDstValue = (stlChar*)((aDstDataValue)->mValue);          \
        stlChar   * tmpSrcValue = (stlChar*)(aSrcValue);                        \
                                                                                \
        STL_DASSERT( (aDstDataValue)->mValue != NULL );                         \
        STL_DASSERT( (aDstDataValue)->mBufferSize >= aValueLen );               \
                                                                                \
        if( (aDstDataValue)->mBufferSize & 0x000000007 )                        \
        {                                                                       \
            while( tmpValueLen < (aValueLen) )                                  \
            {                                                                   \
                ((stlChar*)(tmpDstValue))[0] = ((stlChar*)(tmpSrcValue))[0];    \
                tmpDstValue++;                                                  \
                tmpSrcValue++;                                                  \
                tmpValueLen++;                                                  \
            }                                                                   \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            while( tmpValueLen < (aValueLen) )                                  \
            {                                                                   \
                STL_WRITE_INT64( tmpDstValue, tmpSrcValue );                    \
                tmpDstValue += 8;                                               \
                tmpSrcValue += 8;                                               \
                tmpValueLen += 8;                                               \
            }                                                                   \
        }                                                                       \
    }

/**
 * @brief dtlDataType의 내용 복사
 * [in]      aAvailableDstSize  destination dtlDataType의 사용할 수 있는 mValue 메모리 공간의 크기 ( stlInt64 )
 * [in]      aSrcValue          copy 대상이 되는 dtlDataType ( dtlDataValue * )
 * [in/out]  aDstValue          copy되는 내용이 들어갈 dtlDataType ( dtlDataValue * )
 * @remarks Long Varying Type은 사용해서는 안된다.
 */
#define DTL_COPY_DATA_VALUE( aAvailableDstSize, aSrcValue, aDstValue )                          \
    {                                                                                           \
        STL_DASSERT( ( (aSrcValue)->mType >= 0 ) &&                                             \
                     ( (aSrcValue)->mType < DTL_TYPE_MAX ) );                                   \
        STL_DASSERT( ( (aDstValue)->mType >= 0 ) &&                                             \
                     ( (aDstValue)->mType < DTL_TYPE_MAX ) );                                   \
        STL_DASSERT( ( (aSrcValue)->mType != DTL_TYPE_LONGVARCHAR ) &&                          \
                     ( (aSrcValue)->mType != DTL_TYPE_LONGVARBINARY ) );                        \
        STL_DASSERT( ( (aDstValue)->mType != DTL_TYPE_LONGVARCHAR ) &&                          \
                     ( (aDstValue)->mType != DTL_TYPE_LONGVARBINARY ) );                        \
                                                                                                \
        if( (aSrcValue) == (aDstValue) )                                                        \
        {                                                                                       \
            /* Do Nothing */                                                                    \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            STL_DASSERT( (aAvailableDstSize) >= (aSrcValue)->mLength );                         \
            STL_DASSERT( ((aSrcValue)->mLength == 0) ||                                         \
                         ((aSrcValue)->mValue != NULL) );                                       \
            STL_DASSERT( ((aDstValue)->mValue != NULL) );                                       \
                                                                                                \
            STL_DASSERT( ( (aDstValue)->mType == (aSrcValue)->mType ) ||                        \
                         ( ( dtlDataTypeGroup[(aDstValue)->mType] == DTL_GROUP_NUMBER ) &&      \
                           ( dtlDataTypeGroup[(aSrcValue)->mType] == DTL_GROUP_NUMBER ) ) );    \
            (aDstValue)->mLength = (aSrcValue)->mLength;                                        \
                                                                                                \
            if( (aDstValue)->mValue == (aSrcValue)->mValue )                                    \
            {                                                                                   \
                /* Do Nothing */                                                                \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                STL_DASSERT( (aDstValue)->mBufferSize >= (aSrcValue)->mLength );                \
                DTL_COPY_VALUE( (aDstValue),                                                    \
                                (aSrcValue)->mValue,                                            \
                                (aSrcValue)->mLength );                                         \
            }                                                                                   \
        }                                                                                       \
    }

/**
 * @brief dtlRowId value정보로 rid 각 구성요소 정보 추출
 * [in]  aRowIdValue   dtlRowIdType value
 * [out] aTbsId        tablespace id
 * [out] aPageId       page id
 * [out] aOffset       row offset( row number )
 */
#define DTL_GET_RID_INFO( aRowIdValue, aTbsId, aPageId, aOffset )                               \
    {                                                                                           \
        (aTbsId) =                                                                              \
            (stlUInt16)( (((stlUInt16)(((dtlRowIdType *)(aRowIdValue))->mDigits[8])) << 8)  |   \
                         ((stlUInt16)(((dtlRowIdType *)(aRowIdValue))->mDigits[9])) );          \
                                                                                                \
        (aPageId) =                                                                             \
            (stlUInt32)( (((stlUInt32)(((dtlRowIdType *)(aRowIdValue))->mDigits[10])) << 24) |  \
                         (((stlUInt32)(((dtlRowIdType *)(aRowIdValue))->mDigits[11])) << 16) |  \
                         (((stlUInt32)(((dtlRowIdType *)(aRowIdValue))->mDigits[12])) << 8)  |  \
                         ((stlUInt32)(((dtlRowIdType *)(aRowIdValue))->mDigits[13])) );         \
                                                                                                \
        (aOffset) =                                                                             \
            (stlInt16)( (((stlUInt16)(((dtlRowIdType *)(aRowIdValue))->mDigits[14])) << 8)  |   \
                        ((stlUInt16)(((dtlRowIdType *)(aRowIdValue))->mDigits[15])) );          \
    }

/**
 * @brief dtlRowId value정보로 Table의 ID를 얻는다.
 * [in]  aRowIdValue        dtlRowIdType value
 * [out] aTablePhysicalID   Table ID
 */
#define DTL_GET_TABLE_ID( aRowIdValue, aTableID )                                               \
    {                                                                                           \
        (aTableID) =                                                                            \
            (stlInt64)( (((stlUInt64)(((dtlRowIdType *)(aRowIdValue))->mDigits[0])) << 56) |    \
                        (((stlUInt64)(((dtlRowIdType *)(aRowIdValue))->mDigits[1])) << 48) |    \
                        (((stlUInt64)(((dtlRowIdType *)(aRowIdValue))->mDigits[2])) << 40) |    \
                        (((stlUInt64)(((dtlRowIdType *)(aRowIdValue))->mDigits[3])) << 32) |    \
                        (((stlUInt64)(((dtlRowIdType *)(aRowIdValue))->mDigits[4])) << 24) |    \
                        (((stlUInt64)(((dtlRowIdType *)(aRowIdValue))->mDigits[5])) << 16) |    \
                        (((stlUInt64)(((dtlRowIdType *)(aRowIdValue))->mDigits[6])) << 8)  |    \
                        ((stlUInt64)(((dtlRowIdType *)(aRowIdValue))->mDigits[7])) );           \
    }

/** @} dtlDataValue */


/**
 * @defgroup dtlDataValueDefinition Data Value 정의
 * @ingroup dtlDataValue
 * @{
 */

#define DTL_DATAVALUE_SIZE          STL_SIZEOF( dtlDataValue )

/**
 * @brief DB data value 자료 구조
 * @remarks data type layer의 모든 DB data는 dtlDataValue를 사용한다.
 */
struct dtlDataValue
{
    dtlDataType    mType;       /**< DB data type ID */
    stlInt32       mBufferSize; /**< dtlDataValue::mValue의 버퍼 공간 크기 */
    stlInt64       mLength;     /**< dtlDataValue::mValue의 길이, 0 은 NULL로 약속하고 있음 */
    void         * mValue;      /**< data type 에 해당하는 실제 Value */
};


/**
 * @brief DB Type 을 갖는 Data Value 정의
 */

typedef dtlDataValue   dtlBoolean;        /**< BOOLEAN value */
typedef dtlDataValue   dtlSmallInt;       /**< NATIVE_SMALLINT value */
typedef dtlDataValue   dtlInteger;        /**< NATIVE_INTEGER value */
typedef dtlDataValue   dtlBigInt;         /**< NATIVE_BIGINT value */

typedef dtlDataValue   dtlBinary;         /**< BINARY value */
typedef dtlDataValue   dtlVarbinary;      /**< BINARY VARYING value */
typedef dtlDataValue   dtlLongvarbinary;  /**< BINARY LONG VARYING value */
typedef dtlDataValue   dtlChar;           /**< CHARACTER value */
typedef dtlDataValue   dtlVarchar;        /**< CHARACTER VARYING value */
typedef dtlDataValue   dtlLongvarchar;    /**< CHARACTER LONG VARYING value */

typedef dtlDataValue   dtlReal;           /**< NATIVE_REAL value */
typedef dtlDataValue   dtlDouble;         /**< NATIVE_DOUBLE value */
typedef dtlDataValue   dtlNumeric;        /**< NUMERIC value */

typedef dtlDataValue   dtlDate;           /**< DATE value */
typedef dtlDataValue   dtlTime;           /**< TIME value */
typedef dtlDataValue   dtlTimeTz;         /**< TIME WITH TIME ZONE value */
typedef dtlDataValue   dtlTimestamp;      /**< TIMESTAMP value */
typedef dtlDataValue   dtlTimestampTz;    /**< TIMESTAMP WITH TIME ZONE value */
typedef dtlDataValue   dtlIntervalYearToMonth; /**< YEAR TO MONTH INTERVAL value */
typedef dtlDataValue   dtlIntervalDayToSecond; /**< DAY TO SECOND INTERVAL value */

typedef dtlDataValue   dtlRowId;          /**< ROWID value */


#define DTL_BOOLEAN( aDataValue )              ( *( (dtlBooleanType *) (((dtlBoolean*)(aDataValue))->mValue) ) )
#define DTL_BOOLEAN_IS_TRUE( aDataValue )      ( (aDataValue)->mLength == 0 ? STL_FALSE : DTL_BOOLEAN( aDataValue ) )
#define DTL_BOOLEAN_IS_FALSE( aDataValue )     ( (aDataValue)->mLength == 0 ? STL_FALSE : !DTL_BOOLEAN( aDataValue ) )
#define DTL_SET_BOOLEAN_LENGTH( aDataValue )   ( ((dtlBoolean*)(aDataValue))->mLength = 1 )

extern dtlBoolean dtlBooleanTrue[1];
extern dtlBoolean dtlBooleanFalse[1];
extern dtlBoolean dtlBooleanNull[1];

/**
 * @brief Value Block 의 Header 정보로
 * Table Read, Filter, Index Key 등의 처리를 위해 공유 대상이 되는 자료구조임. 
 */
typedef struct dtlValueBlock
{
    stlBool               mIsSepBuff;         /**< Value 마다 메모리 공간을 확보하는 지 여부 */
    stlLayerClass         mAllocLayer;        /**< Block을 할당한 Layer */
    
    stlInt32              mAllocBlockCnt;     /**< 할당받은 dtlDataValue 의 개수 */
    stlInt32              mUsedBlockCnt;      /**< 실제 Data 가 저장된 개수 */
    stlInt32              mSkipBlockCnt;      /**< Skip 할 Value 개수 */
    stlInt64              mArgNum1;           /**< 첫번째 숫자 (TYPE 마다 용도가 다름)
                                               * 1) time, timetz, timestamp, timestamptz 타입은
                                               *    fractional second precision 정보
                                               * 2) interval 타입은 leading precision 정보
                                               * 3) 그외 타입은 precision 정보
                                               */
    stlInt64              mArgNum2;           /**< 두번째 숫자 (TYPE 마다 용도가 다름)
                                               * 1) interval 타입은 fractional second precision 정보
                                               * 2) 그외 타입은 scale 정보 
                                               */
    dtlStringLengthUnit   mStringLengthUnit;  /**< only for STRING 계열 Type */
    dtlIntervalIndicator  mIntervalIndicator; /**< only for INTERVAL Type */
    stlInt64              mAllocBufferSize;   /**< Data Buffer 공간의 크기 */
    dtlDataValue        * mDataValue;         /**< mAllocBlockCnt 만큼 할당된 공간 */
} dtlValueBlock;

/**
 * @brief SCAN하고 있는 현재 row에 대한 column value 정보 (align이 고려되지 않은 value)
 * @remarks SM에서 SCAN시 사용하기 위한 구조체
 */
typedef struct dtlColumnInReadRow
{
    void      * mValue;
    stlInt64    mLength;
} dtlColumnInReadRow;

typedef struct dtlValueBlockList dtlValueBlockList;

/**
 * @brief Value Block 의 연결관계를 관리하기 위한 정보
 * @remarks
 * column value 의 경우, block 개수만큼의 data buffer 공간을 확보하고,
 * constant value 의 경우, block 개수와 관계 없이 data buffer 공간은 하나만 할당한다.
 */
struct dtlValueBlockList
{
    stlInt64              mTableID;     /**< Table ID */
    stlInt32              mColumnOrder; /**< Table 내 컬럼 순서로 List 구성과 무관하게 고유한 값 */
    dtlValueBlock       * mValueBlock;  /**< Value Block 정보 */
    dtlColumnInReadRow    mColumnInRow; /**< SM에서 SCAN하고 있는 현재 row에 대한 column value 정보 */
    
    dtlValueBlockList   * mNext;
};

/**
 * @brief Value Block List를 연결
 * [in/out]  aCurrBlockList    ( dtlValueBlockList * )
 * [in]      aNextBlockList    ( dtlValueBlockList * )
 *
 * return : 없음
 */
#define DTL_LINK_BLOCK_LIST( aCurrBlockList, aNextBlockList )   \
    ( (aCurrBlockList)->mNext = (aNextBlockList)  )

/**
 * @brief Value Block에서 Value 마다 메모리 공간을 확보하는 지 여부를 반환
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 *
 * return : Value 마다 메모리 공간을 확보하는 지 여부 (stlBool)
 */
#define DTL_GET_BLOCK_IS_SEP_BUFF( aValueBlockList )    \
    ( (aValueBlockList)->mValueBlock->mIsSepBuff )

/**
 * @brief Value Block List 이 속한 Table의 Table ID
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 *
 * return : Table ID (stlInt64)
 *   Table이 아닌 경우는 의미 없는 값이며, 0 을 리턴함.
 */
#define DTL_GET_BLOCK_TABLE_ID( aValueBlockList )       \
    ( (aValueBlockList)->mTableID )

/**
 * @brief Value Block List 의 Table ID를 설정한다.
 * [in/out]  aValueBlockList     ( dtlValueBlockList * )
 * [in]      aTableID            ( stlInt64 )
 *
 * return : 없음
 */
#define DTL_SET_BLOCK_TABLE_ID( aValueBlockList, aTableID )     \
    ( (aValueBlockList)->mTableID = (aTableID) )

/**
 * @brief Value Block List 의 Table 내 Column Order
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 *
 * return : Column Order (stlInt32)
 *   컬럼이 아닌 경우는 의미 없는 값이며, 0 을 리턴함.
 */
#define DTL_GET_BLOCK_COLUMN_ORDER( aValueBlockList )   \
    ( (aValueBlockList)->mColumnOrder )

/**
 * @brief Value Block 의 Column Order 를 설정한다.
 * [in/out]  aValueBlockList     ( dtlValueBlockList * )
 * [in]      aOrder              ( stlInt32 )
 *
 * return : 없음 
 */
#define DTL_SET_BLOCK_COLUMN_ORDER( aValueBlockList, aOrder )   \
    ( (aValueBlockList)->mColumnOrder = (aOrder) )

/**
 * @brief Value Block List 의 할당된 공간의 개수를 획득 
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 *
 * return : 할당된 공간의 개수  ( stlInt32 )
 */
#define DTL_GET_BLOCK_ALLOC_CNT( aValueBlockList )      \
    ( (aValueBlockList)->mValueBlock->mAllocBlockCnt )

/**
 * @brief Value Block List 의 Data 가 존재하는 공간의 개수를 획득 
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 *
 * return : Data가 존재하는 공간의 개수  ( stlInt32 )
 */
#define DTL_GET_BLOCK_USED_CNT( aValueBlockList )       \
    ( (aValueBlockList)->mValueBlock->mUsedBlockCnt )
    
/**
 * @brief 하나의 Value Block List 에 Data 가 존재하는 개수를 설정
 * [in/out]  aValueBlockList     ( dtlValueBlockList * )
 * [in]      aUsedBlockCnt       ( stlInt32 )
 *
 * return : 없음
 */
#define DTL_SET_ONE_BLOCK_USED_CNT( aValueBlockList, aUsedBlockCnt )    \
    ( (aValueBlockList)->mValueBlock->mUsedBlockCnt = (aUsedBlockCnt) )

/**
 * @brief List 에 연결된 모든 Value Block List 에 Data 가 존재하는 개수를 설정
 * [in/out]  aValueBlockList     ( dtlValueBlockList * )
 * [in]      aUsedBlockCnt       ( stlInt32 )
 *
 * return : 없음
 * 
 * remark : List 에 연결된 모든 Value Block List 의 값을 변경해야 함.
 */
#define DTL_SET_ALL_BLOCK_USED_CNT( aValueBlockList, aUsedBlockCnt )    \
    {                                                                   \
        dtlValueBlockList * sBL = NULL;                                 \
        for ( sBL = (aValueBlockList);                                  \
              sBL != NULL;                                              \
              sBL = sBL->mNext )                                        \
        {                                                               \
            sBL->mValueBlock->mUsedBlockCnt = (aUsedBlockCnt);          \
        }                                                               \
    }

/**
 * @brief Value Block List 에서 Skip 할 공간의 개수를 획득 
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 * [in]  aUsedBlockCnt       ( stlInt32 )
 *
 * return : Skip 할 공간의 개수  ( stlInt32 )
 */
#define DTL_GET_BLOCK_SKIP_CNT( aValueBlockList )       \
    ( (aValueBlockList)->mValueBlock->mSkipBlockCnt )

/**
 * @brief 하나의 Value Block List 에 Skip 할 개수를 설정
 * [in/out]  aValueBlockList     ( dtlValueBlockList * )
 * [in]      aSkipBlockCnt       ( stlInt32 )
 *
 * return : 없음
 */
#define DTL_SET_ONE_BLOCK_SKIP_CNT( aValueBlockList, aSkipBlockCnt )    \
    ( (aValueBlockList)->mValueBlock->mSkipBlockCnt = (aSkipBlockCnt) )


/**
 * @brief List 에 연결된 모든 Value Block List 에 Skip 개수를 설정
 * [in/out]  aValueBlockList     ( dtlValueBlockList * )
 * [in]      aSkipBlockCnt       ( stlInt32 )
 *
 * return : 없음
 *
 * remark : List 에 연결된 모든 Value Block List 의 값을 변경해야 함.
 */
#define DTL_SET_ALL_BLOCK_SKIP_CNT( aValueBlockList, aSkipBlockCnt )    \
    {                                                                   \
        dtlValueBlockList * sBL = NULL;                                 \
                                                                        \
        for ( sBL = (aValueBlockList);                                  \
              sBL != NULL;                                              \
              sBL = sBL->mNext )                                        \
        {                                                               \
            sBL->mValueBlock->mSkipBlockCnt = (aSkipBlockCnt);          \
        }                                                               \
    }

/**
 * @brief List 에 연결된 모든 Value Block List 에 Skip과 데이터가 존재하는 개수를 설정
 * [in/out]  aValueBlockList     ( dtlValueBlockList * )
 * [in]      aSkipBlockCnt       ( stlInt32 )
 * [in]      aUsedBlockCnt       ( stlInt32 )
 *
 * return : 없음
 *
 * remark : List 에 연결된 모든 Value Block List 의 값을 변경해야 함.
 */
#define DTL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aValueBlockList, aSkipBlockCnt, aUsedBlockCnt )    \
    {                                                                                           \
        dtlValueBlockList * sBL = NULL;                                                         \
                                                                                                \
        for ( sBL = (aValueBlockList);                                                          \
              sBL != NULL;                                                                      \
              sBL = sBL->mNext )                                                                \
        {                                                                                       \
            sBL->mValueBlock->mSkipBlockCnt = (aSkipBlockCnt);                                  \
            sBL->mValueBlock->mUsedBlockCnt = (aUsedBlockCnt);                                  \
        }                                                                                       \
    }

/**
 * @brief Value Block List 의 DB Data Type
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 *
 * return : Value Block List 의 DB Data Type  ( dtlDataType )
 *   컬럼이 아닌 경우는 의미없는 값이며, 0 을 리턴함.
 */
#define DTL_GET_BLOCK_DB_TYPE( aValueBlockList )                \
    ( (aValueBlockList)->mValueBlock->mDataValue->mType )

/**
 * @brief Value Block List 의 DB Precision
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 *
 * return : dtlValueBlockList.mArgNum1 정보를 리턴   ( stlInt64 )
 */
#define DTL_GET_BLOCK_ARG_NUM1( aValueBlockList )       \
    ( (aValueBlockList)->mValueBlock->mArgNum1 )

/**
 * @brief Value Block List를 할당한 Layer를 반환한다.
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 *
 * return : dtlValueBlockList.mAllocLayer 정보를 리턴   ( stlLayerClass )
 */
#define DTL_GET_BLOCK_ALLOC_LAYER( aValueBlockList )    \
    ( (aValueBlockList)->mValueBlock->mAllocLayer )

/**
 * @brief Value Block List 의 DB Scale
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 *
 * return : dtlValueBlockList.mArgNum2 정보를 리턴   ( stlInt64 )
 */
#define DTL_GET_BLOCK_ARG_NUM2( aValueBlockList )       \
    ( (aValueBlockList)->mValueBlock->mArgNum2 )

/**
 * @brief Value Block List 의 DB StringLengthUnit
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 *
 * return : dtlValueBlockList.mStringLengthUnit 정보를 리턴   ( dtlStringLengthUnit )
 */
#define DTL_GET_BLOCK_STRING_LENGTH_UNIT( aValueBlockList )     \
    ( (aValueBlockList)->mValueBlock->mStringLengthUnit )

/**
 * @brief Value Block List 의 DB IntervalIndicator
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 *
 * return : dtlValueBlockList.mIntervalIndicator 정보를 리턴   ( dtlIntervalIndicator )
 */
#define DTL_GET_BLOCK_INTERVAL_INDICATOR( aValueBlockList )     \
    ( (aValueBlockList)->mValueBlock->mIntervalIndicator )

/**
 * @brief Value Block List 의 Data Buffer 의 할당된 공간의 크기
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 *
 * return : Data Buffer 의 할당된 공간의 크기   ( stlInt64 )
 */
#define DTL_GET_BLOCK_DATA_BUFFER_SIZE( aValueBlockList )       \
    ( (aValueBlockList)->mValueBlock->mAllocBufferSize )

/**
 * @brief Value Block List 의 첫번째 Data Value 를 획득
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 *
 * return : Data Value Pointer   ( dtlDataValue * )
 */
#define DTL_GET_BLOCK_FIRST_DATA_VALUE( aValueBlockList )       \
    ( (aValueBlockList)->mValueBlock->mDataValue )

/**
 * @brief Value Block List 의 N 번째 Data 의 실제 길이를 획득  
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 * [in]  aBlockIdx           ( stlInt32 )
 *
 * return : Data 의 실제 길이   ( stlInt64 )
 *
 * remark : aBlockIdx는 Block Idx (0-base)
 */
#define DTL_GET_BLOCK_DATA_LENGTH( aValueBlockList, aBlockIdx )                 \
    ( ( (aValueBlockList)->mValueBlock->mIsSepBuff == STL_TRUE )                \
      ? (aValueBlockList)->mValueBlock->mDataValue[ (aBlockIdx) ].mLength       \
      : (aValueBlockList)->mValueBlock->mDataValue[0].mLength )                 \

/**
 * @brief Value Block List 의 N 번째 Data 의 실제 길이를 설정 
 * [in/out]  aValueBlockList     ( dtlValueBlockList * )
 * [in]      aBlockIdx           ( stlInt32 )
 * [in]      aDataLength         ( stlInt32 )
 *
 * return : Data 의 실제 길이   ( stlInt64 )
 *
 * remark : aBlockIdx는 Block Idx (0-base)
 *          aDataLength는 Data 의 실제 길이 
 */
#define DTL_SET_BLOCK_DATA_LENGTH( aValueBlockList, aBlockIdx, aDataLength )                    \
    {                                                                                           \
        if ( (aValueBlockList)->mValueBlock->mIsSepBuff == STL_TRUE )                           \
        {                                                                                       \
            STL_DASSERT(                                                                        \
                (aValueBlockList)->mValueBlock->mDataValue[ (aBlockIdx) ].mBufferSize >=        \
                (aDataLength) );                                                                \
            (aValueBlockList)->mValueBlock->mDataValue[ (aBlockIdx) ].mLength = (aDataLength);  \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            STL_DASSERT(                                                                        \
                (aValueBlockList)->mValueBlock->mDataValue[0].mBufferSize >=                    \
                (aDataLength) );                                                                \
            (aValueBlockList)->mValueBlock->mDataValue[0].mLength = (aDataLength);              \
        }                                                                                       \
    }

/**
 * @brief Value Block List 의 N 번째 Data Value 를 획득
 * [in]  aValueBlockList     ( dtlValueBlockList * )
 * [in]  aBlockIdx           ( stlInt32 )
 *
 * return : Data Value Pointer   ( dtlDataValue * )
 *
 * remark : 범위를 벗어난 Idx 인 경우, NULL
 */
#ifdef STL_DEBUG
#define DTL_GET_BLOCK_DATA_VALUE( aValueBlockList, aBlockIdx )                  \
    ( ( (aValueBlockList)->mValueBlock->mIsSepBuff == STL_TRUE )                \
      ? ( ( ( (aBlockIdx) >= 0) &&                                              \
            ( (aBlockIdx) < DTL_GET_BLOCK_ALLOC_CNT( (aValueBlockList) )) )     \
          ? & (aValueBlockList)->mValueBlock->mDataValue[ (aBlockIdx) ]         \
          : NULL )                                                              \
      : & (aValueBlockList)->mValueBlock->mDataValue[0] )
#else
#define DTL_GET_BLOCK_DATA_VALUE( aValueBlockList, aBlockIdx )          \
    ( ( (aValueBlockList)->mValueBlock->mIsSepBuff == STL_TRUE )        \
      ? & (aValueBlockList)->mValueBlock->mDataValue[ (aBlockIdx) ]     \
      : & (aValueBlockList)->mValueBlock->mDataValue[0] )
#endif

/**
 * @brief mIsSepBuff가 True인 Value Block List 의 N 번째 Data Value 를 획득
 * [in]  aValueBlockList     ( knlValueBlockList * )
 * [in]  aBlockIdx           ( stlInt32 )
 *
 * return : Data Value Pointer   ( dtlDataValue * )
 */
#ifdef STL_DEBUG
#define DTL_GET_BLOCK_SEP_DATA_VALUE( aValueBlockList, aBlockIdx )      \
    ( ( ( (aBlockIdx) >= 0) &&                                          \
        ( (aBlockIdx) < DTL_GET_BLOCK_ALLOC_CNT( (aValueBlockList) )) ) \
      ? & (aValueBlockList)->mValueBlock->mDataValue[ (aBlockIdx) ]     \
      : NULL )
#else
#define DTL_GET_BLOCK_SEP_DATA_VALUE( aValueBlockList, aBlockIdx )      \
    ( & (aValueBlockList)->mValueBlock->mDataValue[ (aBlockIdx) ] )
#endif

/**
 * @brief Value Block의 N 번째 Data Value 를 획득
 * [in]  aValueBlock     ( dtlValueBlock * )
 * [in]  aBlockIdx       ( stlInt32 )
 *
 * return : Data Value Pointer   ( dtlDataValue * )
 *
 * remark : 범위를 벗어난 Idx 인 경우, NULL
 *   Kernel Layer 에서만 사용함.
 */
#ifdef STL_DEBUG
#define DTL_GET_VALUE_BLOCK_DATA_VALUE( aValueBlock, aBlockIdx )                        \
    ( ( (aValueBlock)->mIsSepBuff == STL_TRUE )                                         \
      ? ( ( ( (aBlockIdx) >= 0 ) && ( (aBlockIdx) < (aValueBlock)->mAllocBlockCnt) )    \
          ? & (aValueBlock)->mDataValue[ (aBlockIdx) ]                                  \
          : NULL )                                                                      \
      : & (aValueBlock)->mDataValue[0] )
#else
#define DTL_GET_VALUE_BLOCK_DATA_VALUE( aValueBlock, aBlockIdx )        \
    ( ( (aValueBlock)->mIsSepBuff == STL_TRUE )                         \
      ? & (aValueBlock)->mDataValue[ (aBlockIdx) ]                      \
      : & (aValueBlock)->mDataValue[0] )
#endif

/**
 * @brief Value Block을 할당한 Layer를 반환한다.
 * [in]  aValueBlock     ( dtlValueBlock * )
 */
#define DTL_GET_VALUE_BLOCK_ALLOC_LAYER( aValueBlock )  \
    ( (aValueBlock)->mAllocLayer )

/**
 * @brief Value Block List의 N 번째 Data 의 Buffer Pointer
 * [in]  aValueBlockList ( dtlValueBlockList * )
 * [in]  aBlockIdx       ( stlInt32 )
 *
 * return : Data Value 의 mValue Buffer Pointer   ( void * )
 */
#define DTL_GET_BLOCK_DATA_PTR( aValueBlockList, aBlockIdx )                    \
    ( ( (aValueBlockList)->mValueBlock->mIsSepBuff == STL_TRUE )                \
      ? (aValueBlockList)->mValueBlock->mDataValue[ (aBlockIdx) ].mValue        \
      : (aValueBlockList)->mValueBlock->mDataValue[ 0 ].mValue )

/**
 * @brief Value Block List의 N 번째 Data Buffer 공간을 변경한다.
 * [in]  aValueBlockList ( dtlValueBlockList * )
 * [in]  aBlockIdx       ( stlInt32 )
 * [in]  aBuffer         ( void * )
 * [in]  aBufferSize     ( stlInt32 )
 *
 * return : 없음
 *
 * remark : aBlockIdx는 Block Idx (0-base)
 */
#define DTL_SET_BLOCK_DATA_PTR( aValueBlockList, aBlockIdx, aBuffer, aBufferSize )                  \
    {                                                                                               \
        if ( (aValueBlockList)->mValueBlock->mIsSepBuff == STL_TRUE )                               \
        {                                                                                           \
            (aValueBlockList)->mValueBlock->mDataValue[ (aBlockIdx) ].mValue = (aBuffer);           \
            (aValueBlockList)->mValueBlock->mDataValue[ (aBlockIdx) ].mBufferSize = (aBufferSize);  \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            (aValueBlockList)->mValueBlock->mDataValue[0].mValue = (aBuffer);                       \
            (aValueBlockList)->mValueBlock->mDataValue[0].mBufferSize = (aBufferSize);              \
        }                                                                                           \
    }

/**
 * @brief (SCAN 시) 현재 읽고 있는 row내 해당 column value 정보
 * [in]  aValueBlockList ( dtlValueBlockList * )
 *
 * return : 현재 읽고 있는 row내 column value 정보 ( dtlColumnInReadRow * )
 */
#define DTL_GET_BLOCK_COLUMN_IN_ROW( aValueBlockList )  \
    ( &((aValueBlockList)->mColumnInRow) )

/** @} dtlDataValueDefinition */


/**
 * @defgroup dtlDataValueControl Data Value 제어
 * @ingroup dtlDataValue
 * @{
 */


/**
 * @brief dtlDataType을 NULL로 설정
 * [in/out]  aValueBlockList     ( dtlDataValue * )
 *
 * return : 없음
 *
 * remark : aValue는 NULL로 설정할 dtlDataValue
 */
#define DTL_SET_NULL( aValue )   ( (aValue)->mLength = 0 )

/**
 * @brief dtlDataType의 NULL 여부 판정
 * [in/out]  aValueBlockList     ( dtlDataValue * )
 *
 * return : NULL 여부  (stlBool)
 *
 * remark : aValue는 NULL인지 판단할 dtlDataType
 */
#define DTL_IS_NULL( aValue )    ( (aValue)->mLength == 0 )


#define DTL_SWAP( value1, value2, type )        \
    {                                           \
        type temp;                              \
        temp = (value1);                        \
        (value1) = (value2);                    \
        (value2) = temp;                        \
    }

#ifdef STL_IGNORE_VALIDATION
#define DTL_CHECK_TYPE( type, typePtr, errorStack )
#else
#define DTL_CHECK_TYPE( type, typePtr, errorStack )                     \
    {                                                                   \
        DTL_PARAM_VALIDATE( (typePtr) != NULL, (errorStack) );          \
        DTL_PARAM_VALIDATE( (typePtr)->mValue != NULL, (errorStack) );  \
        DTL_PARAM_VALIDATE( (typePtr)->mType == (type), (errorStack) ); \
    }
#endif

#ifdef STL_IGNORE_VALIDATION
#define DTL_CHECK_TYPE2( type1, type2, typePtr, errorStack )
#else
#define DTL_CHECK_TYPE2( type1, type2, typePtr, errorStack )                    \
    {                                                                           \
        DTL_PARAM_VALIDATE( (typePtr) != NULL, (errorStack) );                  \
        DTL_PARAM_VALIDATE( (typePtr)->mValue != NULL, (errorStack) );          \
        DTL_PARAM_VALIDATE( (typePtr)->mType == (type1) ||                      \
                            (typePtr)->mType == (type2) , (errorStack) );       \
    }
#endif

/**
 * @brief real, double 형 overflow 체크
 * return : STL_FALSE : overflow 인 상태
 *          STL_TRUE  : valid한 value인 상태.
 */ 
#define DTL_CHECK_FLOAT_VALUE( value, isInFinite )                                                  \
    ( ((stlIsinfinite(value) != STL_IS_INFINITE_FALSE) && ((isInFinite) == STL_IS_INFINITE_FALSE))  \
      ? STL_FALSE : STL_TRUE )

/**
 * @brief real, double 형 underflow 체크
 * return : STL_FALSE : underflow 인 상태
 *          STL_TRUE  : valid한 value인 상태.
 */ 
#define DTL_CHECK_FLOAT_VALUE_UNDERFLOW( value, zeroIsValid )           \
    ( ((value) == 0.0) && (!(zeroIsValid) ) ? STL_FALSE : STL_TRUE )

/**
 * @brief real, double 형의 overflow, underflow를 체크한다.
 * return : STL_FALSE : overflow 또는 underflow 인 상태
 *          STL_TRUE  : valid한 value인 상태.
 */
#define DTL_CHECK_FLOAT_VALUE_OVER_AND_UNDER_FLOW( value, isInFinite, zeroIsValid )     \
    (                                                                                   \
        ( ( DTL_CHECK_FLOAT_VALUE( (value), (isInFinite) ) == STL_TRUE ) &&             \
          ( DTL_CHECK_FLOAT_VALUE_UNDERFLOW( (value), (zeroIsValid) ) == STL_TRUE ) )   \
        ? STL_TRUE : STL_FALSE )


/** @} dtlDataValueControl */



/**
 * @defgroup dtlOperation Data Value 간 연산
 * @ingroup dtExternal
 * @{
 */

/** @} dtlOperation */



/**
 * @defgroup dtlOperCast Casting 연산
 * @ingroup dtlOperation
 * @{
 */

#define DTL_GET_NUMBER_DECIMAL_PRECISION( aDataType, aPrecision )       \
    ( (aDataType) == DTL_TYPE_FLOAT                                     \
      ? dtlBinaryToDecimalPrecision[ (aPrecision) ]                     \
      : (aPrecision) )                                                  \


/** @} dtlOperCast */



/**
 * @defgroup dtlOperCompare Comparison 연산 
 * @ingroup dtlOperation
 * @{
 */

/* DATA TYPE GROUP */
typedef enum
{
    DTL_GROUP_BOOLEAN = 0,
    DTL_GROUP_BINARY_INTEGER,
    DTL_GROUP_NUMBER,
    DTL_GROUP_BINARY_REAL,
    DTL_GROUP_CHAR_STRING,
    DTL_GROUP_LONGVARCHAR_STRING,
    DTL_GROUP_BINARY_STRING,
    DTL_GROUP_LONGVARBINARY_STRING,
    DTL_GROUP_DATE,
    DTL_GROUP_TIME,
    DTL_GROUP_TIMESTAMP,
    DTL_GROUP_TIME_WITH_TIME_ZONE,
    DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE,
    DTL_GROUP_INTERVAL_YEAR_TO_MONTH,     
    DTL_GROUP_INTERVAL_DAY_TO_SECOND,
    DTL_GROUP_ROWID,
    DTL_GROUP_MAX
} dtlGroup;


#define DTL_GROUP_NOT_APPLICABLE_COMPARISON         (DTL_GROUP_MAX)

/**
 * @brief DataType Group의 대표 타입
 */
extern const dtlDataType dtlGroupRepresentType[ DTL_GROUP_MAX ];


/**
 * @brief Datatype 간의 cast & compare 함수를 구성하기 위해 그룹화 함수 
 */
extern const dtlGroup dtlDataTypeGroup[DTL_TYPE_MAX];

/**
 * @brief 각 data type group간의 conversion 정보 
 */
extern const dtlGroup dtlConversionGroup[DTL_GROUP_MAX][DTL_GROUP_MAX];

/**
 * @brief 두 value에 대한 비교 결과
 * @remarks 두 value에 대한 비교는 comparison operation을 수행하는데 사용된다. 
 */
typedef enum
{
    DTL_COMPARISON_LESS     = -1,
    DTL_COMPARISON_EQUAL    = 0,
    DTL_COMPARISON_GREATER  = 1
} dtlCompareResult;


typedef struct dtlCompareType
{
    dtlDataType   mLeftType;
    dtlDataType   mRightType;
} dtlCompareType;


extern const dtlCompareResult dtlNumericCompResult[2];


/** @} dtlOperCompare */

/**
 * @defgroup dtlFilterDefinition Filter & KeyRange 정의
 * @ingroup dtExternal
 * @{
 */

/**
 * @brief INDEX Column의 최대 개수
 */
#define DTL_INDEX_COLUMN_MAX_COUNT   ( 32 )

/**
 * @brief Operation 이름의 문자열 최대 길이
 */
#define DTL_OPER_NAME_MAX_SIZE       ( 32 )

/**
 * @brief index key column의 flag를 설정하는데 사용한다.
 * @remarks index key column이 아닌 경우 DTL_KEYCOLUMN_EMPTY로 설정한다.
 *     <BR> sort order : ASC / DESC
 *     <BR> nullable   : NOT NULL / NULLABLE
 *     <BR> null order : NULLS FIRST / NULLS LAST
 */
#define DTL_SET_INDEX_COLUMN_FLAG( flag, sortOrder, nullable, nullOrder )       \
    ( (flag) = (sortOrder) | (nullable) | (nullOrder) )


/**
 * @brief key flag로 부터 sort order 속성 얻기 : ASC / DESC
 */
#define DTL_GET_KEY_FLAG_SORT_ORDER( flag )  ( (flag) & DTL_KEYCOLUMN_INDEX_ORDER_MASK )

/**
 * @brief key flag로 부터 nullable 속성 얻기 : NOT NULL / NULLABLE
 */
#define DTL_GET_KEY_FLAG_NULLABLE( flag )    ( (flag) & DTL_KEYCOLUMN_INDEX_NULLABLE_MASK )

/**
 * @brief key flag로 부터 null order 속성 얻기 : NULLS FIRST / NULLS LAST
 */
#define DTL_GET_KEY_FLAG_NULL_ORDER( flag )  ( (flag) & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK )

/**
 * @brief index key column의 flag가 설정되었는지 체크한다.
 * @remarks key column flag는 아래의 flag들이 모두 설정되어 있어야 한다.
 *     <BR> sort order : ASC / DESC
 *     <BR> nullable   : NOT NULL / NULLABLE
 *     <BR> null order : NULLS FIRST / NULLS LAST
 */
#define DTL_CHECK_KEY_FLAG( flag, errorStack )                          \
    {                                                                   \
        DTL_PARAM_VALIDATE( DTL_GET_KEY_FLAG_SORT_ORDER( flag ) != 0,   \
                            errorStack );                               \
        DTL_PARAM_VALIDATE( DTL_GET_KEY_FLAG_NULLABLE( flag ) != 0,     \
                            errorStack );                               \
        DTL_PARAM_VALIDATE( DTL_GET_KEY_FLAG_NULL_ORDER( flag ) != 0,   \
                            errorStack );                               \
    }

/**
 * @brief index key column flag 종류
 *   <BR> 자료 구조 : stlUInt8
 */
typedef enum
{
    DTL_KEYCOLUMN_EMPTY                   = 0x00,   /**< index key column이 아닌 경우 */

    /* keycolumn flags for index key column order */
    DTL_KEYCOLUMN_INDEX_ORDER_ASC         = 0x01,   /**< ASCEND ORDER    (0000 0001) */
    DTL_KEYCOLUMN_INDEX_ORDER_DESC        = 0x02,   /**< DESCEND ORDER   (0000 0010) */
    DTL_KEYCOLUMN_INDEX_ORDER_MASK        = 0x03,   /**< ORDER MASK      (0000 0011) */

    DTL_KEYCOLUMN_INDEX_ORDER_DEFAULT     = 0x01,   /**< DEFAULT = ASC */

    /* keycolumn flags for nullable */
    DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE     = 0x04,   /**< NULLABLE        (0000 0100) */
    DTL_KEYCOLUMN_INDEX_NULLABLE_FALSE    = 0x08,   /**< NOT NULL        (0000 1000) */
    DTL_KEYCOLUMN_INDEX_NULLABLE_MASK     = 0x0C,   /**< NULLABLE MASK   (0000 1100) */

    DTL_KEYCOLUMN_INDEX_NULLABLE_DEFAULT  = 0x04,   /**< DEFAULT = NULLABLE */

    /* keycolumn flags for index null order */
    DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST  = 0x10,   /**< NULLS FIRST     (0001 0000) */
    DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST   = 0x20,   /**< NULLS LAST      (0010 0000) */
    DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK   = 0x30,   /**< NULL ORDER MASK (0011 0000) */

    DTL_KEYCOLUMN_INDEX_NULL_ORDER_DEFAULT= 0x20,   /**< DEFAULT = NULLS LAST */
} dtlKeycolumnFlag;


/**
 * @brief Comparison Operator의 종류
 */
typedef enum 
{
    DTL_COMP_OPER_IS_EQUAL        = 0,   /**< == 연산자 */
    DTL_COMP_OPER_IS_NOT_EQUAL,          /**< != 연산자 */    
    DTL_COMP_OPER_IS_LESS_THAN,          /**< < 연산자 */    
    DTL_COMP_OPER_IS_LESS_THAN_EQUAL,    /**< <= 연산자 */
    DTL_COMP_OPER_IS_GREATER_THAN,       /**< > 연산자 */
    DTL_COMP_OPER_IS_GREATER_THAN_EQUAL, /**< >= 연산자 */
    DTL_COMP_OPER_IS_NULL,               /**< IS NULL 연산자 */
    DTL_COMP_OPER_IS_NOT_NULL,           /**< IS NOT NULL 연산자 */
    DTL_COMP_OPER_MAX
} dtlComparisonOper;

/**
 * @brief Physical Operator의 종류
 */
typedef enum 
{
    DTL_PHYSICAL_FUNC_IS_EQUAL        = 0,   /**< == 연산자 */
    DTL_PHYSICAL_FUNC_IS_NOT_EQUAL,          /**< != 연산자 */    
    DTL_PHYSICAL_FUNC_IS_LESS_THAN,          /**< < 연산자 */    
    DTL_PHYSICAL_FUNC_IS_LESS_THAN_EQUAL,    /**< <= 연산자 */
    DTL_PHYSICAL_FUNC_IS_GREATER_THAN,       /**< > 연산자 */
    DTL_PHYSICAL_FUNC_IS_GREATER_THAN_EQUAL, /**< >= 연산자 */
    DTL_PHYSICAL_FUNC_IS,                    /**< IS 연산자 */
    DTL_PHYSICAL_FUNC_IS_NOT,                /**< IS NOT 연산자 */
    DTL_PHYSICAL_FUNC_IS_NULL,               /**< IS NULL 연산자 */
    DTL_PHYSICAL_FUNC_IS_NOT_NULL,           /**< IS NOT NULL 연산자 */
    DTL_PHYSICAL_FUNC_NOT,                   /**< NOT 연산자 */
    DTL_PHYSICAL_FUNC_MAX
} dtlPhysicalFunctions;

#define DTL_IN_PHYSICAL_FILTER_CNT (16)

/**
 * @brief Function 수행 결과의 종류
 *   <BR> 자료 구조 : stlUInt8
 */
typedef enum dtlFuncReturnTypeClass
{
    DTL_FUNC_RETURN_TYPE_CLASS_INVALID = 0,
    DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,         /**< 1-tuple */
    // DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_ARRAY,         /**< 1-array */
    // DTL_FUNC_RETURN_TYPE_CLASS_MULTISET_VALUE,       /**< n-tuples */
    // DTL_FUNC_RETURN_TYPE_CLASS_MULTISET_ARRAY,       /**< n-array */
    // DTL_FUNC_RETURN_TYPE_CLASS_RELATION,             /**< n-tuples & n-rows */

    DTL_FUNC_RETURN_TYPE_CLASS_MAX
} dtlFuncReturnTypeClass;

extern const stlChar * const gDtlFuncReturnClassString[DTL_FUNC_RETURN_TYPE_CLASS_MAX];


/**
 * @brief Expression Type
 *   <BR> 자료 구조 : stlUInt8
 */
typedef enum
{
    DTL_EXPR_TYPE_EMPTY                 = 0,   /**< Type이 설정되지 않음 */
    DTL_EXPR_TYPE_VALUE,                       /**< dtlDataValue */
    DTL_EXPR_TYPE_FUNCTION,                    /**< Comparison Operator와 Logical Operator를 제외한 Function */
/*    DTL_OPER_ARGUMENT_TYPE_ARRAY, */     /* spec out */
    DTL_EXPR_TYPE_MAX
} dtlExprType;

/**
 * @brief Expression Type
 *   <BR> 자료 구조 : stlUInt8
 */
typedef enum
{
    DTL_VALUE_TYPE_EMPTY                 = 0,   /**< Type이 설정되지 않음 */
    DTL_VALUE_TYPE_VALUE,                       /**< dtlDataValue */
/*    DTL_VALUE_TYPE_ARRAY, */     /* spec out */
    DTL_VALUE_TYPE_TYPE_MAX
} dtlValueType;

/**
 * @brief Expression Entry의 Action type
 *   <BR> 자료 구조 : stlUInt8
 */
typedef enum
{
    DTL_ACTION_TYPE_NORMAL                       = 0,      /**< No Action */
    DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,                /**< jump when dtlDataValue is NULL */
    DTL_ACTION_TYPE_CONDITION_JUMP_IS_NOT_NULL,            /**< jump when dtlDataValue is NOT NULL */
    DTL_ACTION_TYPE_CONDITION_JUMP_IS_TRUE,                /**< jump when dtlBooleanType is TRUE */
    DTL_ACTION_TYPE_CONDITION_JUMP_IS_FALSE,               /**< jump when dtlBooleanType is FALSE */
    DTL_ACTION_TYPE_CONDITION_JUMP_IS_NOT_TRUE,            /**< jump when dtlBooleanType is not TRUE */
    DTL_ACTION_TYPE_CONDITION_JUMP_IS_NOT_FALSE,           /**< jump when dtlBooleanType is not FALSE */
    DTL_ACTION_TYPE_CONDITION_JUMP_IS_FALSE_THEN_IS_NULL,  /**< jump when dtlBooleanType is FALSE & then is NULL ( for AND ) */
    DTL_ACTION_TYPE_CONDITION_JUMP_IS_TRUE_THEN_IS_NULL,   /**< jump when dtlBooleanType is TRUE & then is NULL ( for OR )*/
    DTL_ACTION_TYPE_UNCONDITION_JUMP,                      /**< jump */
    DTL_ACTION_TYPE_MAX
} dtlActionType;

extern const stlChar * const gDtlActionTypeString[DTL_ACTION_TYPE_MAX];

/**
 * @brief Expression Entry의 Action Return Value
 *   <BR> expression result stack의 entry에 대한 Action 수행 후 반환하는 결과 값
 *   <BR> 
 *   <BR> 자료 구조 : stlUInt8
 */
typedef enum
{
    DTL_ACTION_RETURN_PASS       = 0,   /**< 해당 entry의 값을 expression result stack에 push */
    DTL_ACTION_RETURN_NULL,             /**< NULL 값을 expression result stack에 push */
    DTL_ACTION_RETURN_TRUE,             /**< TRUE 값을 expression result stack에 push */
    DTL_ACTION_RETURN_FALSE,            /**< FALSE 값을 expression result stack에 push */
    DTL_ACTION_RETURN_FALSE_THEN_NULL,  /**< FALSE or NULL 값을 expression result stack에 push */
    DTL_ACTION_RETURN_TRUE_THEN_NULL,   /**< TRUE or NULL 값을 expression result stack에 push */

    DTL_ACTION_RETURN_EMPTY,            /**< expression result stack에 아무것도 push 하지 않음 */
    
    DTL_ACTION_RETURN_MAX
} dtlActionReturnValue;

extern const stlChar * const gDtlActionReturnString[DTL_ACTION_RETURN_MAX];

/**
 * @brief Value Entry Value의 구조
 */
typedef union dtlValueEntryValue
{
    dtlDataValue       * mDataValue;   /**< constant value : dtlDataValue */
    /* array   mArray */   /* spec out */
} dtlValueEntryValue;


/**
 * @brief Function Input Argument(value)의 기본 구조
 * 
 * @remark SCAN/FILTER 수행시 argument 또는 수행 결과를 dtlValueEntry로 저장
 */
typedef struct dtlValueEntry
{
    dtlValueEntryValue mValue;             /**< Expression Type에 따른 Expression Record */

    /** mValue을 구분하는 정보 */
    dtlValueType  mValueType;              /**< value type : dtlValueType */

    /** Operation Group 분석을 위한 정보 */
    stlUInt16     mOperGroupEntryNo;       /**< 해당 entry가 속한 Operation Group의 시작 entry 번호
                                            * SCAN/FILTER evaluate에서 설정되는 부분
                                            */
    stlChar       mPadding[2];
} dtlValueEntry;


#define DTL_VALUE_ENTRY_SIZE        STL_SIZEOF( dtlValueEntry )

/** dtlValueEntry 초기화 */
#define DTL_INIT_VALUE_ENTRY( entryPtr )                        \
    {                                                           \
        (entryPtr)->mValueType         = DTL_VALUE_TYPE_EMPTY;  \
        (entryPtr)->mOperGroupEntryNo  = 0;                     \
    }

/** @} dtlFilterDefinition */



/**
 * @defgroup dtlFunction Function 정의 및 제어
 * @ingroup dtlOperation
 * @{
 */

/**
 * @brief builtin function의 함수 원형
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      결과
 *             <BR> result결과가 arrary인 경우의 확장을 위해 void *를 output 인자로 받는다.
 *             <BR>
 *             <BR> 결과값의 갯수 차이에 따라 사용상의 차이가 발생하는데, 
 *             <BR> (1) 결과값이 하나인 함수들은 dtlDataValue로 casting해서 결과값을 만든다.
 *             <BR> (2) 결과값이 여러개인 함수들에 대한 고려는 이후에 ...
 *             <BR>
 * @param[in,out]  aInfo  function 수행에 필요한 부가 정보
 *             <BR> 예) to_char 함수
 *             <BR>     to_char( '2001-01-01', 'yyyy-mm-dd' ) 처리시,
 *             <BR>     'yyyy-mm-dd' 의 parsing(???) 정보
 * @param[in]  aVectorFunc
 * @param[in]  aVectorArg
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
typedef stlStatus (*dtlBuiltInFuncPtr) ( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

/**
 * expression 평가 반복 수행
 */
typedef enum dtlIterationTime
{
    DTL_ITERATION_TIME_FOR_EACH_AGGREGATION = 0,
    /**< 각각의 Aggregation에 대해 매번 평가함
     * : sum, count, min, max, avg */

    DTL_ITERATION_TIME_FOR_EACH_EXPR,
    /**< 각각의 Expression에 대해 매번 평가함
     * : column, function with variable args, random */

    DTL_ITERATION_TIME_FOR_EACH_QUERY,
    /**< 각각의 Query에 대해 한번 평가함
     * : query constant value, outer column */

    DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
    /**< 각각의 Statement에 대해 한번 평가함
     * : statement function, bind param,
     *   function with bind param & constant args */

    DTL_ITERATION_TIME_NONE,
    /**< 미리 평가된 값을 사용
     * : constant value, function with constant agrs */

    DTL_ITERATION_TIME_MAX
} dtlIterationTime;

#define DTL_ITERATION_TIME_INVALID    ( DTL_ITERATION_TIME_MAX )

extern const stlChar * const gDtlIterationTimeString[ DTL_ITERATION_TIME_MAX ];



/**
 * 수행 결과로 Nullable 여부를 반환
 */
typedef enum dtlReturnNullable
{
    DTL_RETURN_NULLABLE_ALWAYS = 0,
    /**< 항상 Nullable 를 반환 */

    DTL_RETURN_NULLABLE_NEVER,
    /**< 항상 Not Nullable 를 반환 */

    DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
    /**< 입력 인자가 하나라도 Nullable 이면 Nullable 를 반환 */

    DTL_RETURN_NULLABLE_FROM_ALL_NULLABLE_INPUT,
    /**< 모든 입력 인자가 Nullable 이면 Nullable 를 반환 */

    DTL_RETURN_NULLABLE_CHECK,
    /**< 모든 입력 인자의 Nullable 을 체크하여 Nullable 를 결정 */

    DTL_RETURN_NULLABLE_MAX
} dtlReturnNullable;


#define DTL_RETURN_NULLABLE_INVALID    ( DTL_RETURN_NULLABLE_MAX )


/**
 * @brief function의 argument의 최대 개수
 */
#define DTL_FUNC_INPUTARG_VARIABLE_CNT    ( 8 )


/**
 * @brief builtIn function의 세부적인 정보를 얻을 수 있는 함수.
 *   <BR> 이 함수에서 얻을수 있는 정보는
 *   <BR>   (1) function point 얻기 위한 정보
 *   <BR>   (2) function의 argument type 정보
 *   <BR>   (3) function의 return type 정보
 *   
 * @param[in]  aDataTypeArrayCount     aDataTypeArrary의 갯수   
 * @param[in]  aIsConstantData         constant value인지 여부
 * @param[in]  aDataTypeArray          function을 수행할 data type
 *                                     <BR> 예) 1(SmallInt) + 5(BigInt) 인 경우,
 *                                     <BR>     (SmallInt와 BigInt)
 * @param[in]  aDataTypeDefInfoArray   aDataTypeArray에 대응되는 dtlDataTypeDefInfo정보
 * @param[in]  aFuncArgDataTypeArrayCount    function의 input argument count
 * @param[out] aFuncArgDataTypeArray   function 수행시 수행 argument type
 *                                     <BR> 예) 1(SmallInt) + 5(BigInt) 인 경우,
 *                                     <BR> 실제 dtlBigIntPlus함수를 수행하게 될것이고,
 *                                     <BR> dtlBigIntPlus는 input argument type로 BigInt를 받아서
 *                                     <BR> BigInt + BigInt = BigInt를 수행하게 됨.
 *                                     <BR> out으로 (BigInt, BigInt) 정보를 보냄.
 * @param[out] aFuncArgDataTypeDefInfoArray aFuncArgDataTypeArray에 대응되는 dtlDataTypeDefInfo정보
 * @param[out] aFuncResultDataType     function 수행시 return type
 * @param[out] aFuncResultDataTypeDefInfo aFuncResultDataType에 대응하는 dtlDataTypeDefInfo정보 
 * @param[out] aFuncPtrIdx             함수 수행을 위한 function pointer 얻기 위한 식별 정보
 * @param[in]  aVectorFunc             Function Vector
 * @param[in]  aVectorArg              Vector Argument
 * @param[out] aErrorStack             에러스택정보 
 */ 
typedef stlStatus (*dtlGetBuiltInFuncDetailInfo) (
    stlUInt16               aDataTypeArrayCount,
    stlBool               * aIsConstantData,
    dtlDataType           * aDataTypeArray,
    dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
    stlUInt16               aFuncArgDataTypeArrayCount,
    dtlDataType           * aFuncArgDataTypeArray,
    dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
    dtlDataType           * aFuncResultDataType,
    dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
    stlUInt32             * aFuncPtrIdx,
    dtlFuncVector         * aVectorFunc,
    void                  * aVectorArg,     
    stlErrorStack         * aErrorStack );

/**
 * @brief builtIn function의 function pointer를 얻는 함수
 * @param[in]  aFuncPtrIdx   function pointer를 얻기 위한 식별 정보 
 * @param[out] aFuncPtr      function pointer
 * @param[out] aErrorStack   에러스택정보 
 */ 
typedef stlStatus (*dtlGetBuiltInFuncPtr) ( stlUInt32             aFuncPtrIdx,
                                            dtlBuiltInFuncPtr   * aFuncPtr,
                                            stlErrorStack       * aErrorStack );



/**
 * @brief function에 대한 정보 관리하는 구조체
 */
typedef struct dtlBuiltInFuncInfo
{
    stlInt32                     mID;                  /**< execution layer에서 정의한 function 배열id
                                                        * ( ellBuiltInFunction ) */
    const stlChar                mName[64];            /**< 함수의 대표명 */
    
    stlUInt16                    mArgumentCntMin;      /**< Function의 input Argument 갯수 */
    stlUInt16                    mArgumentCntMax;      /**< Function의 input Argument 갯수 */    
    dtlFuncReturnTypeClass       mReturnTypeClass;     /**< Function의 return type에 대한 class */
    
    dtlIterationTime             mIterationTime;       /**< Function의 IterationTime */
    stlBool                      mIsNeedCastInGroup;   /**< Function 수행시 동일 GROUP에 속한 
                                                        * DataType간의 연산 수행을 위해
                                                        * type casting이 필요한지 여부 */

    stlBool                      mIsPhysicalFilter;    /**< Physical Filter로 전환이 가능한지 여부 */

    dtlReturnNullable            mReturnNullable;      /**< 수행 결과로 Nullable 여부를 반환 */
    
    dtlActionType                mExceptionType;       /**< Argument 값에 따른 exception 상황 */
    dtlActionReturnValue         mExceptionResult;     /**< exception 상황 발생시 return value */
    stlBool                      mIsExprCompositionComplex;   /**< Function의 expr composition complex 여부 */

    dtlGetBuiltInFuncDetailInfo  mGetFuncDetailInfo;   /**< 실제 수행시 필요한 부가정보를 얻을 수 있는
                                                        * Function pointer */

    dtlGetBuiltInFuncPtr         mGetFuncPointer;      /**< Function point 얻기 */
  
} dtlBuiltInFuncInfo;



/** @} dtlFunction */


/**
 * @defgroup dtlBind Bind Paramater & Fetch Parameter
 * @ingroup dtExternal
 * @{
 */

/**
 * @brief C Data Type Enumeration ID
 */
typedef enum
{
    /** C types */
    DTL_C_TYPE_INT8,      /**< stlBool, stlChar, stlInt8 */
    DTL_C_TYPE_UINT8,     /**< stlUInt8 */

    DTL_C_TYPE_INT16,     /**< stlInt16 */
    DTL_C_TYPE_UINT16,    /**< stlUInt16 */
    
    DTL_C_TYPE_INT32,     /**< stlInt32 */
    DTL_C_TYPE_UINT32,    /**< stlUInt32, stlShortInterval */

    DTL_C_TYPE_INT64,     /**< stlInt64 */
    DTL_C_TYPE_UINT64,    /**< stlUInt64, stlTime, stlInterval */

    DTL_C_TYPE_FLOAT32,   /**< stlFloat32 */
    
    DTL_C_TYPE_FLOAT64,   /**< stlFloat64 */

    DTL_C_TYPE_CHAR_PTR,  /**< stlChar* */
    DTL_C_TYPE_WCHAR_PTR, /**< stlWChar* */

    DTL_C_TYPE_MAX
} dtlCType;

/** @} dtlBind */


/**
 * @defgroup dtlPhysicalFunction Physical Function 정의
 * @ingroup dtlDataTypeInformation
 * @{
 *
 * @todo Type 확장하기 (long varchar, long varbinary, clob, blob ... )
 */

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlBooleanType 얻기
 */
#define DTL_GET_BOOLEAN_FROM_PTR( ptr, value )          \
    ( (value) = (dtlBooleanType)((stlChar*)(ptr))[0] ) 

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlSmallIntType 얻기
 */
#define DTL_GET_NATIVE_SMALLINT_FROM_PTR( ptr, value )    STL_WRITE_INT16( &(value), (ptr) )

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlIntegerType 얻기 
 */
#define DTL_GET_NATIVE_INTEGER_FROM_PTR( ptr, value )    STL_WRITE_INT32( &(value), (ptr) )

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlBigIntType 얻기
 */
#define DTL_GET_NATIVE_BIGINT_FROM_PTR( ptr, value )    STL_WRITE_INT64( &(value), (ptr) )

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlRealType 얻기
 */
#define DTL_GET_NATIVE_REAL_FROM_PTR( ptr, value )     STL_WRITE_FLOAT32( &(value), (ptr) )

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlDoubleType 얻기
 */
#define DTL_GET_NATIVE_DOUBLE_FROM_PTR( ptr, value )   STL_WRITE_FLOAT64( &(value), (ptr) )

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlNumericType 얻기 
 */
#define DTL_GET_NUMERIC_PTR_FROM_PTR( ptr, value ) ( (value) = (dtlNumericType*)(ptr) )

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlCharType 얻기
 */
#define DTL_GET_CHAR_FROM_PTR( ptr, value )      ( (value) = (dtlCharType)(ptr) )

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlVarcharType 얻기 
 */
#define DTL_GET_VARCHAR_FROM_PTR( ptr, value )   ( (value) = (dtlVarcharType)(ptr) )

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlBinaryType 얻기
 */
#define DTL_GET_BINARY_FROM_PTR( ptr, value )      (value) = (dtlBinaryType)(ptr)

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlVarbinaryType 얻기
 */
#define DTL_GET_VARBINARY_FROM_PTR( ptr, value )   (value) = (dtlVarbinaryType)(ptr)

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlDateType 얻기
 */
#define DTL_GET_DATE_FROM_PTR( ptr, value )      STL_WRITE_INT64( &(value), (ptr) )
/**
 * @brief align이 보장되지 않은 pointer로부터 dtlTimeType 얻기
 */
#define DTL_GET_TIME_FROM_PTR( ptr, value )      STL_WRITE_INT64( &(value), (ptr) )

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlTimeTzType 얻기
 */
#define DTL_GET_TIME_TZ_FROM_PTR( ptr, value )                          \
    {                                                                   \
        STL_WRITE_INT64( &(value.mTime), (ptr) );                       \
        STL_WRITE_INT32( &(value.mTimeZone), & ((stlChar*)(ptr))[8] );  \
    }                                                                           

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlTimestampType 얻기 
 */
#define DTL_GET_TIMESTAMP_FROM_PTR( ptr, value )     STL_WRITE_INT64( &(value), (ptr) )

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlTimestampTzType 얻기
 */
#define DTL_GET_TIMESTAMP_TZ_FROM_PTR( ptr, value )                     \
    {                                                                   \
        STL_WRITE_INT64( &(value.mTimestamp), (ptr) );                  \
        STL_WRITE_INT32( &(value.mTimeZone), & ((stlChar*)(ptr))[8]);   \
    }

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlIntervalYearToMonthType 얻기
 */
#define DTL_GET_INTERVAL_YEAR_TO_MONTH_FROM_PTR( ptr, value )           \
    {                                                                   \
        STL_WRITE_INT32( &(value.mMonth), (ptr) );                      \
        STL_WRITE_INT32( &(value.mIndicator), & ((stlChar*)(ptr))[4]);  \
    }                                                                   \
        
/**
 * @brief align이 보장되지 않은 pointer로부터 dtlIntervalDayToSecondType 얻기
 */
#define DTL_GET_INTERVAL_DAY_TO_SECOND_FROM_PTR( ptr, value )           \
    {                                                                   \
        STL_WRITE_INT64( &(value.mTime), (ptr) );                       \
        STL_WRITE_INT32( &(value.mDay), & ((stlChar*)(ptr))[8]);        \
        STL_WRITE_INT32( &(value.mIndicator), & ((stlChar*)(ptr))[12]); \
    }

/**
 * @brief align이 보장되지 않은 pointer로부터 dtlRowIdType 얻기 
 */
#define DTL_GET_ROWID_FROM_PTR( ptr, value )   ( (value) = (dtlRowIdType *)(ptr) )


/**
 * @brief Physical Function Pointer (align이 보장되지 않은 값의 비교)
 */
typedef stlBool (*dtlPhysicalFunc) ( void       * aValue1,
                                     stlInt64     aLength1,
                                     void       * aValue2,
                                     stlInt64     aLength2 );

typedef stlStatus (*dtlNumericSumFunc)( dtlDataValue  * aSumValue,
                                        dtlDataValue  * aSrcValue,
                                        stlErrorStack * aErrorStack );

typedef stlStatus (*dtlAllocFunc)( void      * aMemory,
                                   stlInt32    aSize,
                                   void     ** aAddr,
                                   void      * aEnv );

typedef struct dtlPatternInfo dtlPatternInfo;

typedef stlStatus (*dtlLikeMatchFunc)( dtlPatternInfo *aInfo,
                                       dtlVarcharType  aString,
                                       stlInt32        aStringLen,
                                       dtlCharacterSet aCharSetId,
                                       stlErrorStack  *aErrorStack,
                                       stlBool        *aResult );

/** @} dtlPhysicalFunction */

/**
 * @defgroup dtlFormatting Formatting
 * @ingroup dtExternal
 * @{
 */

// format string 한자에 대한 max length 7 ??? ( J 일때 7 자 )
// format string max length 128 ???

#define DTL_NLS_DATETIME_FORMAT_STRING_MAX_SIZE ( 80 )
#define DTL_NLS_DATETIME_FORMAT_NULL_TERMINATED_STRING_MAX_SIZE ( DTL_NLS_DATETIME_FORMAT_STRING_MAX_SIZE + 1 )

#define DTL_DATETIME_FORMAT_MAX_SIZE   DTL_NLS_DATETIME_FORMAT_STRING_MAX_SIZE

/**
 * TO_CHAR()로 변환했을때 그 결과를 수용할 수 있는 최대 버퍼 사이즈
 * format string 한자에 대한 max length 7임을 감안해서 ( J일때 7자 )
 */
#define DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE ( DTL_NLS_DATETIME_FORMAT_STRING_MAX_SIZE * 7 )
#define DTL_DATETIME_TO_CHAR_RESULT_NULL_TERMINATED_STRING_MAX_SIZE ( DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE + 1 )

// #define DTL_MAX_DATE_LEN             (DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE)

#define DTL_DATE_STRING_SIZE         ( DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE )
#define DTL_TIME_STRING_SIZE         ( DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE )
#define DTL_TIMETZ_STRING_SIZE       ( DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE )
#define DTL_TIMESTAMP_STRING_SIZE    ( DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE )
#define DTL_TIMESTAMPTZ_STRING_SIZE  ( DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE )
// #define DTL_INTERVAL_YEAR_TO_MONTH_MAX_STRING_SIZE ( DTL_INTERVAL_YEAR_TO_MONTH_MAX_STRING_SIZE ) /* 10 */
// #define DTL_INTERVAL_DAY_TO_SECOND_MAX_STRING_SIZE ( DTL_INTERVAL_DAY_TO_SECOND_MAX_STRING_SIZE ) /* 23 */


typedef enum dtlFromCharDateMode
{
    DTL_FORMAT_DATE_NONE = 0,    
    DTL_FORMAT_DATE_GREGORIAN,      /**< Gregorian date ( day, month, year ) */
    DTL_FORMAT_DATE_ISOWEEK         /**< ISO 8601 week date */
} dtlFromCharDateMode;

typedef enum dtlFormatTypeMode
{
    DTL_FORMAT_TYPE_NONE = 0,
    DTL_FORMAT_TYPE_DATE,           /**< DATE */
    DTL_FORMAT_TYPE_TIME,           /**< TIME */
    DTL_FORMAT_TYPE_WITH_TIME_ZONE  /**< WITH TIME ZONE */
} dtlFormatTypeMode;

/**
 * @brief dtlToDateTimeFormatElementSetInfo 구조체
 */
typedef struct dtlToDateTimeFormatElementSetInfo
{
    stlBool        mFractionalSecond;
    stlBool        mSecond;
    stlBool        mMinute;
    stlBool        mHour;
    stlBool        mDay;
    stlBool        mMonth;
    stlBool        mYear;
    stlBool        mWeekDay;
    stlBool        mYearDay;
    stlBool        mSssss;
    stlBool        mJulianDay;
    stlBool        mBc;
    stlBool        mAmPm;
    stlBool        mTimeZoneHour;
    stlBool        mTimeZoneMinute;
} dtlToDateTimeFormatElementSetInfo;

/**
 * @brief datetime format keyword 구조체
 */
typedef struct dtlDateTimeFormatKeyWord
{
    const stlChar      * mName;
    stlUInt8             mLen;
    stlUInt8             mValueMaxLen;  /* string <-> format element간 변환시 string의 최대길이 */
    stlInt32             mId;
    stlBool              mIsDigit;
    dtlFromCharDateMode  mDateMode;
    dtlFormatTypeMode    mTypeMode;
    stlBool              mUsableToDateTimeFunc;    
} dtlDateTimeFormatKeyWord;


/**
 * @brief format keyword를 분석한 node 
 */
typedef struct dtlDateTimeFormatNode
{
    stlInt32                           mType;      /**< node type */
    const dtlDateTimeFormatKeyWord   * mKey;
    stlInt8                            mPos;       /**< dtfFormatStrInof.mStr에서
                                                    * 해당 format element의 시작위치 */
    stlInt8                            mPosLen;    /**< dtfFormatStrInof.mStr에서
                                                    * 해당 format element의 길이 */
} dtlDateTimeFormatNode;

/**
 * @brief format string 전체를 분석한 정보
 */

struct dtlDateTimeFormatStrInfo
{
    stlChar                 * mStr;
    stlInt16                  mStrLen; /* '\0'을 제외한 mStr length */
    dtlDateTimeFormatNode   * mFormat;    
};

/**
 * @brief to_char( datetime )  to_datetime 함수의 format info 자료구조.
 */ 
struct dtlDateTimeFormatInfo
{
    dtlDateTimeFormatStrInfo          * mFormatStrInfo;

    /*
     * 아래 3개의 변수는 to_datetime 에서만 사용됨.
     */
    dtlToDateTimeFormatElementSetInfo   mFormatElementSetInfo;
    stlBool                             mNeedCurrentDate;
    stlBool                             mNeedCurrentTimeZoneOffset;

    /*
     * to_char의 result 길이
     */
    stlUInt16                           mToCharMaxResultLen;

    /*
     * dtlDateTimeFormatInfo 크기.
     */ 
    stlInt32                            mSize;
};

#define DTL_DATETIME_FORMAT_STR_INFO_SIZE            ( STL_SIZEOF(dtlDateTimeFormatStrInfo) )
#define DTL_DATETIME_FORMAT_STR_INFO_MAX_STRING_SIZE ( DTL_DATETIME_FORMAT_MAX_SIZE + 1 )
#define DTL_DATETIME_FORMAT_STR_INFO_MAX_FORMAT_NODE_SIZE                       \
    ( STL_SIZEOF(dtlDateTimeFormatNode) * (DTL_DATETIME_FORMAT_MAX_SIZE + 1) )

#define DTL_DATETIME_FORMAT_STR_INFO_MAX_BUFFER_SIZE                                                \
    ( DTL_DATETIME_FORMAT_STR_INFO_SIZE + DTL_DATETIME_FORMAT_STR_INFO_MAX_STRING_SIZE + DTL_DATETIME_FORMAT_STR_INFO_MAX_FORMAT_NODE_SIZE )

#define DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE                                                    \
    ( STL_ALIGN_DEFAULT(STL_SIZEOF(dtlDateTimeFormatInfo) + DTL_DATETIME_FORMAT_STR_INFO_MAX_BUFFER_SIZE) )

/** @} dtlFormatting */

/**
 * @brief like pattern의 %를 제외한 pattern
 * abc_def_xxx라는 패턴은 3개의 atomPattern을 가진다. abc, def, xxx
 * 각각의 atomPattern은 pos와 length를 가진다.
 */
typedef struct dtlPattern
{
    stlChar        **mAtomPatterns;
    stlInt32        *mUnderscoreCount;/**< 현재 atomPattern 앞에 underscore 개수 */
    stlInt32        *mAtomPatternLen; /**< 현재 atomPattern의 길이 */
    stlInt32         mAtomPatternCount;
    stlInt32         mPatternLen;
} dtlPattern;

/**
 * @brief like pattern을 분석한 정보
 */
struct dtlPatternInfo
{
    dtlLikeMatchFunc mLikeFunc;
    stlChar         *mPatternString;
    dtlPattern      *mLeadingPattern;
    dtlPattern      *mFollowingPattern;
    dtlPattern     **mPatterns;
    stlInt32         mPatternCount;
    stlBool          mIsConstPattern;
    stlBool          mIsBinComparable;
};


/**
 * @brief dtf 함수에서 메모리 할당이 필요할 때 사용되는 info
 */
typedef struct dtlAllocInfo
{
    stlChar      *mBufBegin;
    stlChar      *mPos;
    stlInt32      mBufSize;
} dtlAllocInfo;

#define DTL_ALLOC_INFO_INIT( info, bufBegin, size )     \
    (info)->mBufBegin = (bufBegin);                     \
    (info)->mPos = (bufBegin);                          \
    (info)->mBufSize = (size);

#define DTL_ALLOC_INFO_USED_SIZE( info )        \
    ( (info)->mPos - (info)->mBufBegin )

#define DTL_ALLOC_INFO_SET_BUF_SIZE( info, size )    \
    ( (info)->mBufSize = (size) )

stlStatus dtlAllocMem( dtlAllocInfo * aInfo,
                       stlInt32       aSize,
                       void        ** aResultPtr );


/**
 * @defgroup dtlHash32 32 bit hash functions
 * @ingroup dtExternal
 * @{
 */

typedef stlUInt32 (*dtlHash32Func) ( register const stlUInt8 * aKeyValue,
                                     register stlInt64         aKeyLength );

extern dtlHash32Func gDtlHash32[ DTL_TYPE_MAX ];

/** @} dtlHash32 */





#endif /* _DTLDEF_H_ */


