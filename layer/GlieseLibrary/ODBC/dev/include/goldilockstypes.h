/*******************************************************************************
 * goldilockstypes.h
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

#ifndef _GOLDILOCKS_TYPES_H_
#define _GOLDILOCKS_TYPES_H_ 1

/**
 * @file goldilockstypes.h
 * @brief Gliese API Type Definition file.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SQL_DRIVER_C_TYPE_BASE
#define SQL_DRIVER_C_TYPE_BASE      0x4000
#endif

#ifndef SQL_DRIVER_SQL_TYPE_BASE
#define SQL_DRIVER_SQL_TYPE_BASE    0x4000
#endif

#ifndef SQL_DRIVER_DESC_FIELD_BASE
#define SQL_DRIVER_DESC_FIELD_BASE  0x4000
#endif

#ifndef SQL_DRIVER_DIAG_FIELD_BASE
#define SQL_DRIVER_DIAG_FIELD_BASE  0x4000
#endif

#ifndef SQL_DRIVER_INFO_TYPE_BASE
#define SQL_DRIVER_INFO_TYPE_BASE   0x4000
#endif

#ifndef SQL_DRIVER_CONN_ATTR_BASE
#define SQL_DRIVER_CONN_ATTR_BASE   0x00004000 
#endif

#ifndef SQL_DRIVER_STMT_ATTR_BASE
#define SQL_DRIVER_STMT_ATTR_BASE   0x00004000
#endif

/****************************
 * Driver SQL* data types. use these as much as possible when using ODBC calls/vars
 ***************************/
typedef unsigned char         SQLBOOLEAN;

/****************************
 * Driver structs
 ***************************/

typedef struct tagLONG_VARIABLE_LENGTH_STRUCT
{
    SQLBIGINT   len;
    SQLCHAR   * arr;
} LONG_VARIABLE_LENGTH_STRUCT;

typedef LONG_VARIABLE_LENGTH_STRUCT SQL_LONG_VARIABLE_LENGTH_STRUCT;

/*
 * If the timezone_hour is negative, the timezone_minute must be negative or zero.
 * If the timezone_hour is positive, the timezone_minute must be positive or zero.
 * If the timezone_hour is zero, the stimezone_minute may have any value in the range -59 through +59.
 */

typedef struct tagTIME_WITH_TIMEZONE_STRUCT
{
   SQLUSMALLINT hour;
   SQLUSMALLINT minute;
   SQLUSMALLINT second;
   SQLUINTEGER  fraction;
   SQLSMALLINT  timezone_hour;
   SQLSMALLINT  timezone_minute;
} TIME_WITH_TIMEZONE_STRUCT;

typedef TIME_WITH_TIMEZONE_STRUCT SQL_TIME_WITH_TIMEZONE_STRUCT;

typedef struct tagTIMESTAMP_WITH_TIMEZONE_STRUCT
{
   SQLSMALLINT  year;
   SQLUSMALLINT month;
   SQLUSMALLINT day;
   SQLUSMALLINT hour;
   SQLUSMALLINT minute;
   SQLUSMALLINT second;
   SQLUINTEGER  fraction;
   SQLSMALLINT  timezone_hour;
   SQLSMALLINT  timezone_minute;
} TIMESTAMP_WITH_TIMEZONE_STRUCT;

typedef TIMESTAMP_WITH_TIMEZONE_STRUCT SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT;

/* SQL data type codes */
#ifndef SQL_BOOLEAN
#define SQL_BOOLEAN                      16
#endif

#ifndef SQL_BLOB
#define SQL_BLOB                         30
#endif

#ifndef SQL_CLOB
#define SQL_CLOB                         40
#endif

#ifndef SQL_TYPE_TIME_WITH_TIMEZONE
#define SQL_TYPE_TIME_WITH_TIMEZONE      94
#endif

#ifndef SQL_TYPE_TIMESTAMP_WITH_TIMEZONE
#define SQL_TYPE_TIMESTAMP_WITH_TIMEZONE 95
#endif
    
#ifndef SQL_ROWID
#define SQL_ROWID                        (-101)
#endif

/* C datatype to SQL datatype mapping */
#define SQL_C_BOOLEAN                      SQL_BOOLEAN
#define SQL_C_LONGVARCHAR                  SQL_LONGVARCHAR
#define SQL_C_LONGVARBINARY                SQL_LONGVARBINARY
#define SQL_C_TYPE_TIME_WITH_TIMEZONE      SQL_TYPE_TIME_WITH_TIMEZONE
#define SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE SQL_TYPE_TIMESTAMP_WITH_TIMEZONE

/* SQL time with timezone/timestamp with timezone type subcodes */
#define SQL_CODE_TIME_WITH_TIMEZONE      4
#define SQL_CODE_TIMESTAMP_WITH_TIMEZONE 5

/* extended descriptor field */
#define SQL_DESC_CHAR_LENGTH_UNITS              SQL_DRIVER_DESC_FIELD_BASE + 1

/* values for SQL_DESC_CHAR_LENGTH_UNITS */
#define SQL_CLU_NONE       0
#define SQL_CLU_CHARACTERS 1
#define SQL_CLU_OCTETS     2
    
/* connection attributes */
#define SQL_ATTR_CHARACTER_SET                  SQL_DRIVER_CONN_ATTR_BASE + 1
#define SQL_ATTR_DATABASE_CHARACTER_SET         SQL_DRIVER_CONN_ATTR_BASE + 2
#define SQL_ATTR_TIMEZONE                       SQL_DRIVER_CONN_ATTR_BASE + 3
#define SQL_ATTR_DATE_FORMAT                    SQL_DRIVER_CONN_ATTR_BASE + 4
#define SQL_ATTR_TIME_FORMAT                    SQL_DRIVER_CONN_ATTR_BASE + 5
#define SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT      SQL_DRIVER_CONN_ATTR_BASE + 6
#define SQL_ATTR_TIMESTAMP_FORMAT               SQL_DRIVER_CONN_ATTR_BASE + 7
#define SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT SQL_DRIVER_CONN_ATTR_BASE + 8
#define SQL_ATTR_PROTOCOL                       SQL_DRIVER_CONN_ATTR_BASE + 9
#define SQL_ATTR_OLDPWD                         SQL_DRIVER_CONN_ATTR_BASE + 10

/* character set values */
#define SQL_ASCII   0
#define SQL_UTF8    1
#define SQL_UHC     2
#define SQL_GB18030 3
    
/* protocol values */
#define SQL_DA   0
#define SQL_TCP  1
    
/* statement attributes */
#define SQL_ATTR_CURSOR_HOLDABLE          (-3)
    
#define SQL_ATTR_EXPLAIN_PLAN_OPTION      SQL_DRIVER_STMT_ATTR_BASE + 1
#define SQL_ATTR_EXPLAIN_PLAN_TEXT        SQL_DRIVER_STMT_ATTR_BASE + 2
#define SQL_ATTR_ATOMIC_EXECUTION         SQL_DRIVER_STMT_ATTR_BASE + 3
#define SQL_ATTR_PREFETCH_ROWS            SQL_DRIVER_STMT_ATTR_BASE + 4

/* explain plan values */
#define SQL_EXPLAIN_PLAN_OFF                0
#define SQL_EXPLAIN_PLAN_ON                 1
#define SQL_EXPLAIN_PLAN_ON_VERBOSE         2
#define SQL_EXPLAIN_PLAN_ONLY               3

/* atomic execution values */
#define SQL_ATOMIC_EXECUTION_OFF            0
#define SQL_ATOMIC_EXECUTION_ON             1

/* SQL_ATTR_CURSOR_HOLDABLE options */
#define SQL_NONHOLDABLE                     0
#define SQL_HOLDABLE                        1

/* SQL_CURSOR_TYPE options */
#define SQL_CURSOR_ISO                      4UL


#ifdef __cplusplus
}
#endif
    
#endif /* _GOLDILOCKS_TYPES_H_ */
