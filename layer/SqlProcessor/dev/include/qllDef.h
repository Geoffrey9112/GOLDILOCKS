/*******************************************************************************
 * qllDef.h
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

#ifndef _QLLDEF_H_
#define _QLLDEF_H_ 1

/**
 * @file qllDef.h
 * @brief SQL Processor Layer Definition
 */

#include <qllRowStatusDef.h>

/*******************************************************************************
 * Error 
 ******************************************************************************/

/**
 * @defgroup qllError Error
 * @ingroup qlExternal
 * @{
 */

/**
 * @brief initialize error
 */
#define QLL_ERRCODE_INITIALIZE                          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 0 )

/**
 * @brief not implemented feature, in a function %s
 */
#define QLL_ERRCODE_NOT_IMPLEMENTED                     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 1 )

/**
 * @brief invalid function sequence
 */
#define QLL_ERRCODE_INVALID_FUNCTION_SEQUENCE           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 2 )

/**
 * @brief identifier is too long
 */
#define QLL_ERRCODE_IDENTIFIER_LENGTH_EXCEEDED          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 3 )


/**
 * @brief invalid login
 */
#define QLL_ERRCODE_INVALID_LOGIN                       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 4 )

/**
 * @brief name '%s.%s' is already used by an existing object
 */
#define QLL_ERRCODE_USED_OBJECT_NAME                    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 5 )

/**
 * @brief schema '%s' does not exists
 */
#define QLL_ERRCODE_SCHEMA_NOT_EXISTS                   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 6 )

/**
 * @brief schema '%s' is not writable
 */
#define QLL_ERRCODE_SCHEMA_NOT_WRITABLE                 \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 7 )

/**
 * @brief tablespace '%s' does not exists
 */
#define QLL_ERRCODE_TABLESPACE_NOT_EXISTS               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 8 )

/**
 * @brief tablespace '%s' is not writable
 */
#define QLL_ERRCODE_TABLESPACE_NOT_WRITABLE             \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 9 )

/**
 * @brief invalid pctfree, pctused value
 */
#define QLL_ERRCODE_INVALID_PCTVALUE                    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 10 )

/**
 * @brief invalid INITRANS option value
 */
#define QLL_ERRCODE_INVALID_INITRANS                   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 11 )

/**
 * @brief duplicate column name
 */
#define QLL_ERRCODE_DUPLICATE_COLUMN_NAME               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 12 )

/**
 * @brief invalid value is prevented in validation step
 */
#define QLL_ERRCODE_VALIDATION_INVALID_VALUE            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 13 )

/**
 * @brief Sequence increment zero
 */
#define QLL_ERRCODE_SEQUENCE_INCREMENT_ZERO             \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 14 )

/**
 * @brief Sequence startwith under minvalue
 */
#define QLL_ERRCODE_SEQUENCE_STARTWITH_UNDER_MINVALUE   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 15 )

/**
 * @brief Sequence startwith over maxvalue
 */
#define QLL_ERRCODE_SEQUENCE_STARTWITH_OVER_MAXVALUE    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 16 )

/**
 * @brief Sequence invalid cache size
 */
#define QLL_ERRCODE_SEQUENCE_CACHE_SIZE                 \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 17 )

/**
 * @brief Sequence invalid cache range
 */
#define QLL_ERRCODE_SEQUENCE_CACHE_RANGE                \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 18 )

/**
 * @brief MINVALUE must be less than MAXVALUE
 */
#define QLL_ERRCODE_SEQUENCE_MINVALUE_RANGE             \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 19 )

/**
 * @brief INCREMENT must be less than MAXVALUE minus MINVALUE
 */
#define QLL_ERRCODE_SEQUENCE_INCREMENT_RANGE            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 20 )

/**
 * @brief descending sequences that CYCLE must specify MINVALUE
 */
#define QLL_ERRCODE_SEQUENCE_DESCENDING_CYCLE_NO_MINVALUE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 21 )

/**
 * @brief ascending sequences that CYCLE must specify MAXVALUE
 */
#define QLL_ERRCODE_SEQUENCE_ASCENDING_CYCLE_NO_MAXVALUE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 22 )

/**
 * @brief duplicate or conflicting CACHE/NO CACHE specifications
 */
#define QLL_ERRCODE_SEQUENCE_CACHE_NOCACHE              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 23 )

/**
 * @brief numeric precision specifier is out of range( %d to %d )
 */
#define QLL_ERRCODE_NUMERIC_PRECISION_INVALID            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 24 )

/**
 * @brief numeric scale specifier is out of range( %d to %d )
 */
#define QLL_ERRCODE_NUMERIC_SCALE_INVALID               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 25 )

/**
 * @brief character length specifier is out of range( 1 to %d )
 */
#define QLL_ERRCODE_CHARACTER_LENGTH_INVALID            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 26 )

/**
 * @brief character varying length specifier is out of range( 1 to %d )
 */
#define QLL_ERRCODE_CHARACTER_VARYING_LENGTH_INVALID    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 27 )

/**
 * @brief binary length specifier is out of range( 1 to %d )
 */
#define QLL_ERRCODE_BINARY_LENGTH_INVALID               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 28 )

/**
 * @brief binary varying length specifier is out of range( 1 to %d )
 */
#define QLL_ERRCODE_BINARY_VARYING_LENGTH_INVALID       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 29 )

/**
 * @brief name already used by an existing constraint
 */
#define QLL_ERRCODE_CONSTRAINT_NAME_EXISTS              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 30 )

/**
 * @brief auto-generated name already used by an existing constraint,
 * specify unique constraint name 
 */
#define QLL_ERRCODE_AUTO_CONSTRAINT_NAME_EXISTS         \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 31 )

/**
 * @brief table can have only one primary key
 */
#define QLL_ERRCODE_MULTIPLE_PRIMARY_KEY                \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 32 )

/**
 * @brief such unique or primary key already exists in the table
 */
#define QLL_ERRCODE_MULTIPLE_UNIQUE_KEY                 \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 33 )

/**
 * @brief name already used by an existing index
 */
#define QLL_ERRCODE_INDEX_NAME_EXISTS                   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 34 )

/**
 * @brief auto-generated name already used by an existing index,
 * specify non-duplicate index name 
 */
#define QLL_ERRCODE_AUTO_INDEX_NAME_EXISTS              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 35 )

/**
 * @brief '%s': invalid identifier
 */
#define QLL_ERRCODE_INVALID_IDENTIFIER                  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 36 )

/**
 * @brief maximum number of columns exceeded
 */
#define QLL_ERRCODE_MAXIMUM_COLUMN_EXCEEDED             \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 37 )

/**
 * @brief integrity constraint violation
 */
#define QLL_ERRCODE_INTEGRITY_CONSTRAINT_VIOLATION      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 38 )

/**
 * @brief savepoint '%s' never established in this session or is invalid
 */
#define QLL_ERRCODE_SAVEPOINT_EXCEPTION                 \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 39 )

/**
 * @brief table or view does not exist
 */
#define QLL_ERRCODE_TABLE_OR_VIEW_NOT_EXIST      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 40 )

/**
 * @brief table '%s' is not writable
 */
#define QLL_ERRCODE_TABLE_NOT_WRITABLE                 \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 41 )

/**
 * @brief unique/primary keys in table referenced by foreign keys
 */
#define QLL_ERRCODE_TABLE_KEY_REFERENCED_BY_FOREIGN_KEY   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 42 )

/**
 * @brief table referenced by views
 */
#define QLL_ERRCODE_TABLE_REFERENCED_BY_VIEW   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 43 )

/**
 * @brief sequence does not exist
 */
#define QLL_ERRCODE_SEQUENCE_NOT_EXIST      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 44 )

/**
 * @brief duplicate options 
 */
#define QLL_ERRCODE_DUPLICATE_OPTIONS      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 45 )

/**
 * @brief such column list already indexed
 */
#define QLL_ERRCODE_SAME_INDEX_KEY_COLUMNS      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 46 )

/**
 * @brief numeric value out of range
 */
#define QLL_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 47 )

/**
 * @brief specified index does not exist
 */
#define QLL_ERRCODE_INDEX_NOT_EXIST      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 48 )

/**
 * @brief index '%s' is not writable
 */
#define QLL_ERRCODE_INDEX_NOT_WRITABLE                 \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 49 )

/**
 * @brief cannot modify index used for enforcement of unique/primary/foreign key
 */
#define QLL_ERRCODE_INDEX_WITH_KEY_CONSTRAINT      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 50 )

/**
 * @brief 수행할 수 없는 연산
 */
#define QLL_ERRCODE_NOT_APPLICABLE                      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 51 )

/**
 * @brief column not allowed here
 */
#define QLL_ERRCODE_COLUMN_NOT_ALLOWED          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 52 )

/**
 * @brief not enough values
 */
#define QLL_ERRCODE_NOT_ENOUGH_VALUES           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 53 )

/**
 * @brief too many values
 */
#define QLL_ERRCODE_TOO_MANY_VALUES             \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 54 )

/**
 * @brief NULL value not allowed 
 */
#define QLL_ERRCODE_NULL_VALUE_NOT_ALLOWED        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 55 )

/**
 * @brief cannot insert NULL into \"%s\".\"%s\".\"%s\"
 */
#define QLL_ERRCODE_CANNOT_INSERT_NULL_INTO       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 56 )

/**
 * @brief unique constraint (%s.%s) violated
 */
#define QLL_ERRCODE_UNIQUE_CONSTRAINT_VIOLATED    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 57 )

/**
 * @brief not applicable hint
 */
#define QLL_ERRCODE_NOT_APPLICABLE_HINT    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 58 )

/**
 * @brief not applicable datatype
 */
#define QLL_ERRCODE_NOT_APPLICABLE_DATATYPE    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 59 )

/**
 * @brief bind variable does not exist
 */
#define QLL_ERRCODE_BIND_VARIABLE_NOT_EXIST     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 60 )

/**
 * @brief cannot update (\"%s\".\"%s\".\"%s\") to NULL
 */
#define QLL_ERRCODE_CANNOT_UPDATE_NULL          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 61 )

/**
 * @brief syntax error
 */
#define QLL_ERRCODE_SYNTAX_ERROR                        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 62 )

/**
 * @brief fractional seconds precision specifier is out of range( %d to %d )
 */
#define QLL_ERRCODE_FRACTIONAL_SECOND_PRECISION_INVALID       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 63 )

/**
 * @brief invalid interval indicator
 */
#define QLL_ERRCODE_INVALID_INTERVAL_INDICATOR    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 64 )

/**
 * @brief interval leading precision specifier is out of range( %d to %d )
 */
#define QLL_ERRCODE_INVALID_INTERVAL_LEADING_PRECISION    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 65 )

/**
 * @brief interval fractional seconds precision specifier is out of range( %d to %d )
 */
#define QLL_ERRCODE_INVALID_INTERVAL_FRACTIONAL_SECONDS_PRECISION    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 66 )

/**
 * @brief invalid INITIAL storage option value
 */
#define QLL_ERRCODE_INVALID_INITIAL_STORAGE_OPTION  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 67 )

/**
 * @brief invalid NEXT storage option value
 */
#define QLL_ERRCODE_INVALID_NEXT_STORAGE_OPTION     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 68 )

/**
 * @brief invalid MINSIZE storage option value
 */
#define QLL_ERRCODE_INVALID_MINSIZE_STORAGE_OPTION   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 69 )

/**
 * @brief invalid MAXSIZE storage option value
 */
#define QLL_ERRCODE_INVALID_MAXSIZE_STORAGE_OPTION   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 70 )

/**
 * @brief storage option MAXSIZE is less than MINSIZE
 */
#define QLL_ERRCODE_INVALID_MINMAX_STORAGE_OPTION   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 71 )

/**
 * @brief duplicate constraint (null/not null)
 */
#define QLL_ERRCODE_DUPLICATE_CONSTRAINTS_NULL_NOT_NULL      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 72 )
 
/**
 * @brief duplicate constraint (unique/primary key)
 */
#define QLL_ERRCODE_DUPLICATE_CONSTRAINTS_UNIQUE_PRIMARY_KEY      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 73 )

/**
 * @brief sequence number not allowed here
 */
#define QLL_ERRCODE_SEQUENCE_NUMBER_NOT_ALLOWED      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 74 )

/**
 * @brief inconsistent datatypes (unsupported internal cast)
 */
#define QLL_ERRCODE_INCONSISTENT_DATATYPES              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 75 )

/**
 * @brief invalid DATAFILE clause for alter of %s %s TABLESPACE
 */
#define QLL_ERRCODE_INVALID_MEMORY_DATAFILE_CLAUSE_FOR_ALTER  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 76 )

/**
 * @brief invalid DATAFILE clause for alter of %s %s TABLESPACE
 */
#define QLL_ERRCODE_INVALID_DATAFILE_FOR_ALTER  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 77 )

/**
 * @brief invalid TEMPFILE clause for alter of %s %s TABLESPACE
 */
#define QLL_ERRCODE_INVALID_TEMPFILE_FOR_ALTER  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 78 )

/**
 * @brief file path is too long ( < %d )
 */
#define QLL_ERRCODE_FILEPATH_TOO_LONG          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 79 )

/**
 * @brief file size is out of range( %ld to %ld )
 */
#define QLL_ERRCODE_FILESIZE_OUT_OF_RANGE          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 80 )

/**
 * @brief system thread is already created
 */
#define QLL_ERRCODE_ALREADY_EXIST_SYSTEM_THREAD       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 81 )

/**
 * @brief invalid MAXTRANS option value
 */
#define QLL_ERRCODE_INVALID_MAXTRANS                   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 82 )

/**
 * @brief INITRANS must be less equal than MAXTRANS
 */
#define QLL_ERRCODE_TRANS_RANGE                        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 83 )

/**
 * @brief result OFFSET must be 0 or positive numeric value
 */
#define QLL_ERRCODE_RESULT_OFFSET_MUST_BE_ZERO_OR_POSITIVE  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 84 )

/**
 * @brief result LIMIT/FETCH must be positive numeric value
 */
#define QLL_ERRCODE_RESULT_LIMIT_MUST_BE_POSITIVE    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 85 )

/**
 * @brief related objects are modified by concurrent DDL execution
 */
#define QLL_ERRCODE_INVALID_OBJECT_BY_CONCURRENT_DDL    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 86 )

/**
 * @brief invalid where clause
 */
#define QLL_ERRCODE_INVALID_WHERE_CLAUSE    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 87 )

/**
 * @brief session cannot be killed now
 */
#define QLL_ERRCODE_SESSION_CANNOT_BE_KILLED_NOW        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 88 )

/**
 * @brief built-in authorization(user or role) '%s' is not modifiable
 */
#define QLL_ERRCODE_BUILT_IN_AUTHORIZATION_IS_NOT_MODIFIABLE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 89 )

/**
 * @brief built-in tablespace '%s' is not modifiable
 */
#define QLL_ERRCODE_BUILT_IN_TABLESPACE_IS_NOT_MODIFIABLE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 90 )

/**
 * @brief built-in schema '%s' is not modifiable
 */
#define QLL_ERRCODE_BUILT_IN_SCHEMA_IS_NOT_MODIFIABLE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 91 )

/**
 * @brief built-in table or view '%s' is not modifiable
 */
#define QLL_ERRCODE_BUILT_IN_TABLE_IS_NOT_MODIFIABLE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 92 )

/**
 * @brief built-in index '%s' is not modifiable
 */
#define QLL_ERRCODE_BUILT_IN_INDEX_IS_NOT_MODIFIABLE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 93 )

/**
 * @brief built-in constraint '%s' is not modifiable
 */
#define QLL_ERRCODE_BUILT_IN_CONSTRAINT_IS_NOT_MODIFIABLE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 94 )

/**
 * @brief built-in sequence '%s' is not modifiable
 */
#define QLL_ERRCODE_BUILT_IN_SEQUENCE_IS_NOT_MODIFIABLE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 95 )

/**
 * @brief authorization(user or role) '%s' does not exist
 */
#define QLL_ERRCODE_AUTHORIZATION_NOT_EXIST               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 96 )

/**
 * @brief constraint '%s' does not exist
 */
#define QLL_ERRCODE_CONSTRAINT_NOT_EXIST               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 97 )

/**
 * @brief bind type mismatch of parameter number (%d)
 */
#define QLL_ERRCODE_BIND_TYPE_MISMATCH               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 98 )

/**
 * @brief the number of elements in the INTO list is mismatch
 */
#define QLL_ERRCODE_NUMBER_OF_ELEMENTS_INTO_LIST_MISMATCH   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 99 )

/**
 * @brief single-row subquery returns more than one row
 */
#define QLL_ERRCODE_SINGLE_ROW_SUBQUERY_RETURNS_MORE_THAN_ONE_ROW   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 100 )

/**
 * @brief parallel factor is out of range( 0 to %d )
 */
#define QLL_ERRCODE_INVALID_PARALLEL_FACTOR    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 101 )

/**
 * @brief invalid MEMORY clause for alter of %s %s TABLESPACE
 */
#define QLL_ERRCODE_INVALID_MEMORY_CLAUSE_FOR_ALTER  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 102 )

/**
 * @brief cursor '%s' is already declared
 */
#define QLL_ERRCODE_CURSOR_ALREADY_DECLARED       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 103 )

/**
 * @brief invalid cursor query: '%s' 
 */
#define QLL_ERRCODE_INVALID_CURSOR_QUERY       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 104 )

/**
 * @brief invalid cursor property 
 */
#define QLL_ERRCODE_INVALID_CURSOR_PROPERTY       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 105 )

/**
 * @brief cursor '%s' does not exist
 */
#define QLL_ERRCODE_CURSOR_NOT_EXIST               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 106 )

/**
 * @brief cursor '%s' is already open
 */
#define QLL_ERRCODE_CURSOR_ALREADY_OPEN       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 107 )

/**
 * @brief cursor '%s' is not open
 */
#define QLL_ERRCODE_CURSOR_IS_NOT_OPEN       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 108 )

/**
 * @brief invalid use: cursor '%s' is not a declared cursor
 */
#define QLL_ERRCODE_CURSOR_NOT_DECLARED_CURSOR       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 109 )

/**
 * @brief invalid use: cursor '%s' is not an ODBC/JDBC cursor
 */
#define QLL_ERRCODE_CURSOR_NOT_ODBC_CURSOR       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 110 )

/**
 * @brief cursor '%s' is not scrollable
 */
#define QLL_ERRCODE_CURSOR_IS_NOT_SCROLLABLE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 111 )

/**
 * @brief query expression is not updatable
 */
#define QLL_ERRCODE_QUERY_IS_NOT_UPDATABLE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 112 )

/**
 * @brief invalid phase(%s): executable phase of '%s' is between %s and %s
 */
#define QLL_ERRCODE_INVALID_EXECUTABLE_PHASE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 113 )

/**
 * @brief system is already mounted
 */
#define QLL_ERRCODE_INVALID_MOUNTABLE_PHASE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 114 )

/**
 * @brief system is already open
 */
#define QLL_ERRCODE_INVALID_OPENABLE_PHASE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 115 )

/**
 * @brief transaction is READ ONLY
 */
#define QLL_ERRCODE_TRANSACTION_IS_READ_ONLY       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 116 )

/**
 * @brief database not mounted
 */
#define QLL_ERRCODE_DATABASE_NOT_MOUNTED          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 117 )

/**
 * @brief supplemental logging attribute primary key exists
 */
#define QLL_ERRCODE_SUPPLEMENTAL_LOGGING_PK_EXISTS        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 118 )

/**
 * @brief cannot drop nonexistent primary key supplemental logging
 */
#define QLL_ERRCODE_CANNOT_DROP_NONEXISTENT_PK_SUPPLEMENTAL_LOGGING   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 119 )

/**
 * @brief column referenced by views
 */
#define QLL_ERRCODE_COLUMN_REFERENCED_BY_VIEW   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 120 )

/**
 * @brief column has a check constraint defined
 */
#define QLL_ERRCODE_COLUMN_HAS_CHECK_CONSTRAINT_DEFINED   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 121 )

/**
 * @brief pseudo column not allowed here
 */
#define QLL_ERRCODE_PSEUDO_COLUMN_NOT_ALLOWED           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 122 )
/**
 * @brief cannot select ROWID from not writable table
 */
#define QLL_ERRCODE_PSEUDO_COLUMN_CANNOT_SELECT_FROM_NOT_WRITABLE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 123 )

/**
 * @brief table can have only one identity column
 */
#define QLL_ERRCODE_MULTIPLE_IDENTITY_COLUMN                \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 124 )

/**
 * @brief column being added already exists in table
 */
#define QLL_ERRCODE_COLUMN_BEING_ADDED_EXISTS_IN_TABLE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 125 )

/**
 * @brief table must be empty to add mandatory (%s) column ('%s')
 */
#define QLL_ERRCODE_TABLE_MUST_EMPTY_TO_ADD_MANDATORY_COLUMN        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 126 )

/**
 * @brief cannot modify INITIAL storage option
 */
#define QLL_ERRCODE_CANNOT_MODIFY_STORAGE_INITIAL        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 127 )

/**
 * @brief name '%s' is already used by an existing tablespace
 */
#define QLL_ERRCODE_USED_TABLESPACE_NAME                 \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 128 )

/**
 * @brief duplicate datafile name
 */
#define QLL_ERRCODE_DUPLICATE_DATAFILE_NAME              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 129 )

/**
 * @brief file is already exist
 */
#define QLL_ERRCODE_ALREADY_EXIST_FILE              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 130 )

/**
 * @brief cannot rename the built-in tablespace
 */
#define QLL_ERRCODE_RENAME_BUILTIN_TABLESPACE              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 131 )

/**
 * @brief cannot rename the OFFLINE tablespace
 */
#define QLL_ERRCODE_RENAME_OFFLINE_TABLESPACE              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 132 )

/**
 * @brief cannot drop the built-in tablespace
 */
#define QLL_ERRCODE_DROP_BUILTIN_TABLESPACE              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 133 )

/**
 * @brief cannot drop constraint - nonexistent constraint
 */
#define QLL_ERRCODE_DROP_CONSTRAINT_NONEXISTENT_CONSTRAINT  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 134 )


/**
 * @brief cannot drop constraint - nonexistent primary key
 */
#define QLL_ERRCODE_DROP_CONSTRAINT_NONEXISTENT_PRIMARY_KEY  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 135 )

/**
 * @brief cannot drop constraint - nonexistent unique key
 */
#define QLL_ERRCODE_DROP_CONSTRAINT_NONEXISTENT_UNIQUE_KEY  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 136 )

/**
 * @brief the CDS database cannot be closed with ABORT option
 */
#define QLL_ERRCODE_CDS_ABORT_CLOSE_OPTION  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 137 )

/**
 * @brief the DS database can be closed with NORMAL option - deprecated
 */
#define QLL_ERRCODE_INVALID_DS_CLOSE_OPTION  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 138 )

/**
 * @brief multiple order by clause
 */
#define QLL_ERRCODE_MULTIPLE_ORDER_BY_CLAUSE    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 139 )

/**
 * @brief multiple offset clause
 */
#define QLL_ERRCODE_MULTIPLE_OFFSET_CLAUSE    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 140 )
 
/**
 * @brief multiple fetch clause
 */
#define QLL_ERRCODE_MULTIPLE_LIMIT_CLAUSE    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 141 )
 
/**
 * @brief column ambiguously defined
 */
#define QLL_ERRCODE_COLUMN_AMBIGUOUS        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 142 )

/**
 * @brief column part of USING clause cannot have qualifier
 */
#define QLL_ERRCODE_COLUMN_PART_OF_USING_CLAUSE   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 143 )

/**
 * @brief invalid OFFSET option value
 */
#define QLL_ERRCODE_INVALID_OFFSET                      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 144 )

/**
 * @brief invalid LIMIT option value
 */
#define QLL_ERRCODE_INVALID_LIMIT                       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 145 )

/**
 * @brief common column name '%s' appears more than once in %s table
 */
#define QLL_ERRCODE_EXIST_SAME_COLUMN_NAME_IN_TABLE    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 146 )

/**
 * @brief file is already used in database
 */
#define QLL_ERRCODE_ALREADY_USED_DATAFILE    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 147 )

/**
 * @brief tablespace not empty, use INCLUDING CONTENTS option
 */
#define QLL_ERRCODE_TABLESPACE_NOT_EMPTY    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 148 )

/**
 * @brief datafile does not exist in tablespace
 */
#define QLL_ERRCODE_DATAFILE_NOT_EXISTS               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 149 )

/**
 * @brief cannot drop datafile of OFFLINE tablespace
 */
#define QLL_ERRCODE_DROP_OFFLINE_DATAFILE               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 150 )

/**
 * @brief cannot drop first datafile of tablespace
 */
#define QLL_ERRCODE_DROP_FIRST_DATAFILE               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 151 )

/**
 * @brief memory is already used in database
 */
#define QLL_ERRCODE_ALREADY_USED_MEMORY    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 152 )

/**
 * @brief memory does not exist in tablespace
 */
#define QLL_ERRCODE_MEMORY_NOT_EXISTS    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 153 )

/**
 * @brief cannot drop first memory of tablespace
 */
#define QLL_ERRCODE_DROP_FIRST_MEMORY               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 154 )

/**
 * @brief tablespace '%s' is not writable for logging index
 */
#define QLL_ERRCODE_TABLESPACE_NOT_WRITABLE_FOR_LOGGING_INDEX        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 155 )

/**
 * @brief tablespace '%s' is not writable for nologging index
 */
#define QLL_ERRCODE_TABLESPACE_NOT_WRITABLE_FOR_NOLOGGING_INDEX        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 156 )

/**
 * @brief ORDER BY item must be the number of a SELECT-list expression
 */
#define QLL_ERRCODE_ORDER_BY_NUMBER_MISMATCH   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 157 )

/**
 * @brief group function is not allowed here
 */
#define QLL_ERRCODE_GROUP_FUNCTION_NOT_ALLOWED   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 158 )

/**
 * @brief invalid number of arguments
 */
#define QLL_ERRCODE_INVALID_NUMBER_OF_ARGUMENTS   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 159 )

/**
 * @brief group function is nested too deeply
 */
#define QLL_ERRCODE_GROUP_FUNCTION_IS_NESTED_TOO_DEEPLY   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 160 )

/**
 * @brief not a single-group group function
 */
#define QLL_ERRCODE_NOT_A_GROUP_FUNCTION   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 161 )

/**
 * @brief mismatched datafile pairs
 */
#define QLL_ERRCODE_MISMATCHED_DATAFILE_PAIRS   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 162 )

/**
 * @brief cannot rename datafile of ONLINE tablespace
 */
#define QLL_ERRCODE_RENAME_ONLINE_DATAFILE   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 163 )

/**
 * @brief file does not exist
 */
#define QLL_ERRCODE_FILE_NOT_EXIST               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 164 )

/**
 * @brief tablespace is not online
 */
#define QLL_ERRCODE_NOT_ONLINE_TABLESPACE                   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 165 )

/**
 * @brief tablespace is not offline
 */
#define QLL_ERRCODE_NOT_OFFLINE_TABLESPACE                  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 166 )

/**
 * @brief cannot offline temporary tablespace
 */
#define QLL_ERRCODE_OFFLINE_TEMPORARY_TABLESPACE            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 167 )

/**
 * @brief cannot offline dictionary tablespace
 */
#define QLL_ERRCODE_OFFLINE_DICTIONARY_TABLESPACE           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 168 )

/**
 * @brief cannot offline undo tablespace
 */
#define QLL_ERRCODE_OFFLINE_UNDO_TABLESPACE            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 169 )

/**
 * @brief invalid WAIT interval
 */
#define QLL_ERRCODE_INVALID_WAIT_INTERVAL            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 170 )

/**
 * @brief column is not updatable
 */
#define QLL_ERRCODE_COLUMN_IS_NOT_UPDATABLE                     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 171 )

/**
 * @brief data is outside the range of the data type to which the number is being converted
 */
#define QLL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 172 )

/**
 * @brief one or more fields of data value were truncated during conversion
 */
#define QLL_ERRCODE_C_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 173 )

/**
 * @brief system session cannot be killed
 */
#define QLL_ERRCODE_SYSTEM_SESSION_CANNOT_BE_KILLED        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 174 )

/**
 * @brief hint not allowed in positioned DELETE
 */
#define QLL_ERRCODE_HINT_NOT_ALLOWED_POSITIONED_DELETE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 175 )

/**
 * @brief hint not allowed in positioned UPDATE
 */
#define QLL_ERRCODE_HINT_NOT_ALLOWED_POSITIONED_UPDATE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 176 )

/**
 * @brief cursor '%s' is not updatable
 */
#define QLL_ERRCODE_CURSOR_IS_NOT_UPDATABLE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 177 )

/**
 * @brief cursor cannot identify the underlying table
 */
#define QLL_ERRCODE_CURSOR_CANNOT_IDENTIFY_UNDERLYING_TABLE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 178 )

/**
 * @brief cursor was not positioned on a certain row
 */
#define QLL_ERRCODE_CURSOR_WAS_NOT_POSITIONED_ON_A_CERTAIN_ROW       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 179 )

/**
 * @brief cursor was positioned on a row that had been deleted
 */
#define QLL_ERRCODE_CURSOR_WAS_POSITIONED_ON_A_ROW_THAT_HAD_BEEN_DELETED       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 180 )

/**
 * @brief the CDS database cannot be closed with OPERATIONAL option
 */
#define QLL_ERRCODE_CDS_OPERATIONAL_CLOSE_OPTION  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 181 )

/**
 * @brief invalid segment identifier
 */
#define QLL_ERRCODE_INVALID_SEGMENT_ID  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 182 )

/**
 * @brief FETCH position must be numeric value
 */
#define QLL_ERRCODE_FETCH_POSITION_MUST_BE_NUMERIC_VALUE  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 183 )

/**
 * @brief cannot drop all columns in a table
 */
#define QLL_ERRCODE_CANNOT_DROP_ALL_COLUMNS_IN_TABLE  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 184 )

/**
 * @brief column already has a NOT NULL constraint
 */
#define QLL_ERRCODE_COLUMN_ALREADY_HAS_NOT_NULL_CONSTRAINT  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 185 )

/**
 * @brief column does not have a NOT NULL constraint
 */
#define QLL_ERRCODE_COLUMN_DOES_NOT_HAVE_NOT_NULL_CONSTRAINT  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 186 )

/**
 * @brief null values found
 */
#define QLL_ERRCODE_NULL_VALUES_FOUND  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 187 )

/**
 * @brief result set does not exist
 */
#define QLL_ERRCODE_RESULT_SET_NOT_EXIST               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 188 )

/**
 * @brief column is not an identity column
 */
#define QLL_ERRCODE_COLUMN_NOT_IDENTITY_COLUMN  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 189 )

/**
 * @brief column cannot be cast to incompatible type
 */
#define QLL_ERRCODE_COLUMN_CANNOT_CAST_IMCOMPATIBLE_TYPE  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 190 )
/**
 * @brief column cannot be cast to decrease precision or scale
 */
#define QLL_ERRCODE_COLUMN_CANNOT_CAST_DECREASE_PRECISION_OR_SCALE  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 191 )

/**
 * @brief column cannot be cast to decrease string length
 */
#define QLL_ERRCODE_COLUMN_CANNOT_CAST_DECREASE_STRING_LENGTH  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 192 )

/**
 * @brief column cannot be cast to incompatible char length unit
 */
#define QLL_ERRCODE_COLUMN_CANNOT_CAST_INCOMPATIBLE_CHAR_LENGTH_UNIT  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 193 )

/**
 * @brief column cannot be cast to incompatible interval indicator
 */
#define QLL_ERRCODE_COLUMN_CANNOT_CAST_INCOMPATIBLE_INTERVAL_INDICATOR  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 194 )

/**
 * @brief float precision specifier is out of range( %d to %d )
 */
#define QLL_ERRCODE_FLOAT_PRECISION_INVALID            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 195 )

/**
 * @brief file is already used in log stream
 */
#define QLL_ERRCODE_ALREADY_USED_LOGFILE    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 196 )

/**
 * @brief duplicate logfile name
 */
#define QLL_ERRCODE_DUPLICATE_LOGFILE_NAME              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 197 )

/**
 * @brief too small log file size
 */
#define QLL_ERRCODE_TOO_SMALL_LOG_FILE_SIZE              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 198 )

/**
 * @brief cannot find log member
 */
#define QLL_ERRCODE_LOG_MEMBER_NOT_FOUND                    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 199 )

/**
 * @brief log file count mismatched
 */
#define QLL_ERRCODE_LOG_FILE_COUNT_MISMATCH               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 200 )
/**
 * @brief cannot find log file
 */
#define QLL_ERRCODE_LOG_FILE_NOT_FOUND                    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 201 )

/**
 * @brief name '%s' is already used by an existing schema
 */
#define QLL_ERRCODE_USED_SCHEMA_NAME                 \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 202 )

/**
 * @brief schema not empty, use CASCADE option
 */
#define QLL_ERRCODE_SCHEMA_NOT_EMPTY    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 203 )

/**
 * @brief another checkpoint is in progress
 */
#define QLL_ERRCODE_INPROGRESS_CHECKPOINT       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 204 )

/**
 * @brief boolean expression expected
 */
#define QLL_ERRCODE_BOOLEAN_EXPRESSION_EXPECTED                 \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 205 )

/**
 * @brief maximum number of arguments exceeded
 */
#define QLL_ERRCODE_MAXIMUM_NUMBER_OF_ARGUMENTS_EXCEEDED  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 206 )
        
/**
 * @brief %s and %s cannot be matched
 */
#define QLL_ERRCODE_TYPE_MISMATCH                        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 207)

/**
 * @brief insufficient privileges
 */
#define QLL_ERRCODE_INSUFFICENT_PRIVILEGES       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 208 )

/**
 * @brief user %s lacks CREATE SESSION privilege; logon denied
 */
#define QLL_ERRCODE_USER_LACKS_CREATE_SESSION_PRIVILEGE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 209 )

/**
 * @brief lacks privilege (%s ON DATABASE)
 */
#define QLL_ERRCODE_LACKS_PRIVILEGE_ON_DATABASE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 210 )

/**
 * @brief lacks privilege (%s ON TABLESPACE \"%s\")
 */
#define QLL_ERRCODE_LACKS_PRIVILEGE_ON_TABLESPACE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 211 )

/**
 * @brief lacks privilege (%s ON SCHEMA \"%s\")
 */
#define QLL_ERRCODE_LACKS_PRIVILEGES_ON_SCHEMA       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 212 )

/**
 * @brief user name '%s' conflicts with another user or role name
 */
#define QLL_ERRCODE_USER_NAME_CONFLICT_WITH_ANOTHER_USER_OR_ROLE_NAME       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 213 )

/**
 * @brief tablespace '%s' is not data tablespace
 */
#define QLL_ERRCODE_TABLESPACE_NOT_DATA_TABLESPACE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 214 )

/**
 * @brief tablespace '%s' is not temporary tablespace
 */
#define QLL_ERRCODE_TABLESPACE_NOT_TEMPORARY_TABLESPACE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 215 )

/**
 * @brief user '%s' does not exists
 */
#define QLL_ERRCODE_USER_NOT_EXISTS                   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 216 )

/**
 * @brief user has own schema: DROP SCHEMA first
 */
#define QLL_ERRCODE_USER_HAS_OWN_SCHEMA                   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 217 )

/**
 * @brief CASCADE must be specified to drop user '%s'
 */
#define QLL_ERRCODE_CASCADE_MUST_BE_SPECIFIED_TO_DROP_USER            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 218 )

/**
 * @brief you may not GRANT/REVOKE privileges to/from yourself
 */
#define QLL_ERRCODE_NOT_GRANT_REVOKE_PRIVILEGES_YOURSELF            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 219 )

/**
 * @brief duplicate authorization name in grantee/revokee list
 */
#define QLL_ERRCODE_DUPLICATE_AUTH_NAME_IN_GRATEE_REVOKEE_LIST            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 220 )

/**
 * @brief user default schema dropped
 */
#define QLL_ERRCODE_USER_DEFAULT_SCHEMA_DROPPED            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 221 )

/**
 * @brief user default data tablespace dropped
 */
#define QLL_ERRCODE_USER_DEFAULT_DATA_TABLESPACE_DROPPED            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 222 )

/**
 * @brief user default temporary tablespace dropped
 */
#define QLL_ERRCODE_USER_DEFAULT_TEMPORARY_TABLESPACE_DROPPED            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 223 )

/**
 * @brief no privileges to GRANT ALL PRIVILEGES
 */
#define QLL_ERRCODE_NO_PRIVILEGES_TO_GRANT_ALL_PRIVILEGES            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 224 )

/**
 * @brief insufficient privilege to GRANT
 */
#define QLL_ERRCODE_INSUFFICIENT_PRIVILEGE_TO_GRANT            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 225 )

/**
 * @brief duplicate privilege listed
 */
#define QLL_ERRCODE_DUPLICATE_PRIVILEGE_LISTED            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 226 )

/**
 * @brief cannot drop a user that is currently connected
 */
#define QLL_ERRCODE_CANNOT_DROP_USER_CURRENTLY_CONNECTED            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 227 )

/**
 * @brief fixed table not grantable/revokable
 */
#define QLL_ERRCODE_FIXED_TABLE_NOT_GRANT_REVOKE            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 228 )

/**
 * @brief invalid old password
 */
#define QLL_ERRCODE_INVALID_OLD_PASSWORD            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 229 )

/**
 * @brief schema element does not match schema identifier
 */
#define QLL_ERRCODE_SCHEMA_ELEMENT_NOT_MATCH_SCHEMA_IDENTIFIER \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 230 )

/**
 * @brief cannot GRANT/REVOKE privileges to/from built-in authorization
 */
#define QLL_ERRCODE_NOT_GRANT_REVOKE_PRIVILEGES_BUILT_IN_AUTHORIZATION    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 231 )

/**
 * @brief cannot REVOKE privileges you did not grant to '%s'
 */
#define QLL_ERRCODE_NOT_REVOKE_PRIVILEGES_YOU_DID_NOT_GRANT    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 232 )

/**
 * @brief noarchivelog mode
 */
#define QLL_ERRCODE_NOARCHIVELOG_MODE                         \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 233 )

/**
 * @brief no logging tablespace
 */
#define QLL_ERRCODE_NO_LOGGING_TABLESPACE                   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 234 )

/**
 * @brief dependent privilege descriptors still exist
 */
#define QLL_ERRCODE_DEPENDENT_PRIVILEGE_DESCRIPTORS_STILL_EXIST              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 235 )

/**
 * @brief not supported query 
 */
#define QLL_ERRCODE_NOT_SUPPORTED_QUERY                     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 236 )

/**
 * @brief a subquery block has incorrect number of result columns
 */
#define QLL_ERRCODE_SUBQUERY_BLOCK_HAS_INCORRECT_NUMBER_RESULT_COLUMNS \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 237 )

/**
 * @brief ORDER BY item must be the number of a SELECT-list expression
 */
#define QLL_ERRCODE_ORDER_BY_ITEM_MUST_BE_NUMBER_OF_SELECT_LIST_EXPRESSION \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 238 )

/**
 * @brief expression must have same datatype as corresponding expression
 */
#define QLL_ERRCODE_EXPRESSION_MUST_HAVE_SAME_DATATYPE_AS_CORRESPONDING_EXPRESSION \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 239 )

/**
 * @brief subquery expressions not allowed here 
 */
#define QLL_ERRCODE_SUB_QUERY_NOT_ALLOWED                       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 240 )


/**
 * @brief invalid number of column names specified
 */
#define QLL_ERRCODE_INVALID_NUMBER_OF_COLUMN_NAMES_SPECIFIED  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 241 )

/**
 * @brief must name this expression with a column alias
 */
#define QLL_ERRCODE_MUST_NAME_THIS_EXPRESSION_WTIH_COLUMN_ALIAS  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 242 )

/**
 * @brief Warning: View definition has compilation errors
 */
#define QLL_ERRCODE_WARNING_VIEW_DEFINITION_HAS_COMPILATION_ERRORS  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 243 )

/**
 * @brief bind variables not allowed for data definition operations
 */
#define QLL_ERRCODE_BIND_VARIABLES_NOT_ALLOWED_FOR_DDL  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 244 )

/**
 * @brief view \"%s.%s\" has errors
 */
#define QLL_ERRCODE_VIEW_HAS_ERRORS  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 245 )

/**
 * @brief illegal use of data type
 */
#define QLL_ERRCODE_ILLEGAL_USE_OF_DATA_TYPE  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 246 )

/**
 * @brief cannot BEGIN BACKUP; noarchivelog mode
 */
#define QLL_ERRCODE_CANNOT_BACKUP_NOARCHIVELOG_MODE      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 247 )

/**
 * @brief cannot RECOVER TABLESPACE; tablespace is in use
 */
#define QLL_ERRCODE_CANNOT_RECOVER_TABLESPACE      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 248 )

/**
 * @brief cannot SHUTDOWN; backup in progress
 */
#define QLL_ERRCODE_BACKUP_IN_PROGRESS      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 249 )

/**
 * @brief RECOVER UNTIL CANCEL can be issued by directly attached session only - deprecated
 */
#define QLL_ERRCODE_CANNOT_RECOVER_UNTIL_CANCEL      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 250 )

/**
 * @brief single-row subquery returns more than one row
 * @remark 100번의 Error Code와 중복
 */
#define QLL_ERRCODE_SINGLE_ROW_SUBQUERY_RETURNS_MULTIPLE_ROWS  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 251 )

/**
 * @brief circular view definition encountered
 */
#define QLL_ERRCODE_CIRCULAR_VIEW_DEFINITION_ENCOUNTERED  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 252 )

/**
 * @brief only base table columns are allowed to GRANT/REVOKE
 */
#define QLL_ERRCODE_ONLY_BASE_TABLE_COLUMNS_ARE_ALLOWED_TO_GRANT_REVOKE    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 253 )

/**
 * @brief lacks privilege (%s ON TABLE \"%s\".\"%s\")
 */
#define QLL_ERRCODE_LACKS_PRIVILEGES_ON_TABLE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 254 )

/**
 * @brief lacks privilege (USAGE ON SEQUENCE \"%s\".\"%s\")
 */
#define QLL_ERRCODE_LACKS_PRIVILEGES_ON_SEQUENCE       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 255 )

/**
 * @brief wrong number of parameters
 */
#define QLL_ERRCODE_WRONG_NUMBER_OF_PARAMETERS                  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 256 )

/**
 * @brief argument of CASE/WHEN must be type boolean
 */
#define QLL_ERRCODE_ARGUMENT_OF_CASE_WHEN_MUST_BE_TYPE_BOOLEAN     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 257 )

/**
 * @brief the XID does not exist
 */
#define QLL_ERRCODE_XA_NOTA     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 258 )

/**
 * @brief the XID string is not valid
 */
#define QLL_ERRCODE_INVALID_XID_STRING     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 259 )

/**
 * @brief the global transaction is in progress
 */
#define QLL_ERRCODE_INVALID_XA_STATE     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 260 )

/**
 * @brief invalid INCREMENTAL BACKUP LEVEL specified : %d
 */
#define QLL_ERRCODE_CANNOT_SUPPORT_INCREMENTAL_BACKUP_LEVEL     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 261 )

/**
 * @brief INCREMENTAL BACKUP LEVEL 0 cannot have any BACKUP options
 */
#define QLL_ERRCODE_CANNOT_SUPPORT_INCREMENTAL_BACKUP_OPTION     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 262 )

/**
 * @brief cannot RESTORE TABLESPACE; tablespace is in use
 */
#define QLL_ERRCODE_CANNOT_RESTORE_TABLESPACE      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 263 )

/**
 * @brief transaction is in progress
 */
#define QLL_ERRCODE_INPROGRESS_TRANSACTION     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 264 )

/**
 * @brief outer join operator (+) cannot be used with ANSI joins
 */
#define QLL_ERRCODE_CANNOT_USE_OUTER_JOIN_OPERATOR_WITH_ANSI_JOINS  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 265 )

/**
 * @brief outer join operator (+) not allowed in %s
 */
#define QLL_ERRCODE_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 266 )

/**
 * @brief an outer join cannot be specified on a correlation column
 */
#define QLL_ERRCODE_OUTER_JOIN_CANNOT_BE_SPECIFIED_ON_CORRELATION_COLUMN    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 267 )

/**
 * @brief a predicate may reference only one outer-joined table
 */
#define QLL_ERRCODE_PREDICATE_MAY_REFERENCED_ONLY_ONE_OUTER_JOIN_TABLE  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 268 )

/**
 * @brief a table may be outer joined to at most one other table
 */
#define QLL_ERRCODE_MAY_BE_OUTER_JOINED_TO_AT_MOST_ONE_OTHER_TABLE  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 269 )

/**
 * @brief two tables cannot be outer-joined to each other
 */
#define QLL_ERRCODE_CANNOT_BE_OUTER_JOINED_TO_EACH_OTHER    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 270 )

/**
 * @brief RESTART value cannot be made to be less than the MINVALUE
 */
#define QLL_ERRCODE_RESTART_VALUE_CANNOT_BE_MADE_TO_LESS_THAN_MINVALUE \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 271 )

/**
 * @brief RESTART value cannot be made to exceed the MAXVALUE
 */
#define QLL_ERRCODE_RESTART_VALUE_CANNOT_BE_MADE_TO_EXCEED_MAXVALUE \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 272 )

/**
 * @brief MINVALUE cannot be made to exceed the current value
 */
#define QLL_ERRCODE_MINVALUE_CANNOT_BE_MADE_TO_EXCEED_THE_CURRENT_VALUE \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 273 )

/**
 * @brief MAXVALUE cannot be made to be less than the current value
 */
#define QLL_ERRCODE_MAXVALUE_CANNOT_BE_MADE_TO_LESS_THAN_THE_CURRENT_VALUE \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 274 )

/**
 * @brief MINVALUE cannot be made to exceed the START WITH value
 */
#define QLL_ERRCODE_MINVALUE_CANNOT_BE_MADE_TO_EXCEED_THE_START_WITH_VALUE \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 275 )

/**
 * @brief MAXVALUE cannot be made to be less than START WITH value
 */
#define QLL_ERRCODE_MAXVALUE_CANNOT_BE_MADE_TO_LESS_THAN_THE_START_WITH_VALUE \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 276 )

/**
 * @brief log group id is already used
 */
#define QLL_ERRCODE_ALREADY_USED_LOGGROUP_ID \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 277 )

/**
 * @brief identity column cannot have a default value
 */
#define QLL_ERRCODE_IDENTITY_COLUMN_CANNOT_HAVE_DEFAULT_VALUE \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 278 )

/**
 * @brief DEFAULT expression has errors
 */
#define QLL_ERRCODE_DEFAULT_EXPRESSION_HAS_ERRORS  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 279 )

/**
 * @brief the statement is disallowed
 */
#define QLL_ERRCODE_DISALLOW_STATEMENT  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 280 )

/**
 * @brief resource manager doing work outside transaction
 */
#define QLL_ERRCODE_XA_OUTSIDE  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 281 )

/**
 * @brief MEMORY tablespace does not support an AUTOEXTEND option
 */
#define QLL_ERRCODE_MEMORY_TABLESPACE_DOES_NOT_SUPPORT_AUTOEXTEND  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 282 )

/**
 * @brief invalid ROWID
 */
#define QLL_ERRCODE_INVALID_ROWID                               \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 283 )

/**
 * @brief unable to execute in CDS mode
 */
#define QLL_ERRCODE_NOT_ALLOW_COMMAND_IN_CDS                 \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 284 )

/**
 * @brief failed to create system threads
 */
#define QLL_ERRCODE_SYSTEM_THREAD_CREATION_FAIL       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 285 )

/**
 * @brief valid incremental backup does not exist
 */
#define QLL_ERRCODE_NOT_EXIST_VALID_INCREMENTAL_BACKUP          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 286 )

/**
 * @brief log group id is not exist
 */
#define QLL_ERRCODE_NOT_EXIST_LOGGROUP_ID                       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 287 )

/**
 * @brief invalid string value specified
 */
#define QLL_ERRCODE_INVALID_STRING_VALUE_SPECIFIED              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 288 )

/**
 * @brief into clause can have only one row
 */
#define QLL_ERRCODE_INTO_CLAUSE_MULTIPLE_ROWS  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 289 )

/**
 * @brief SHUTDOWN must be executed in dedicated session
 */
#define QLL_ERRCODE_SHUTDOWN_MUST_DEDICATED     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 290 )

/** 
 * @brief transaction rollback: integrity constraint violation 
 */ 
#define QLL_ERRCODE_TRANSACTION_ROLLBACK_INTEGRITY_CONSTRAINT_VIOLATION     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 291 )

/**
 * @brief column length specifier is out of range( 1 to %d ) for columnar table
 */
#define QLL_ERRCODE_COLUMNAR_TABLE_INVALID_COLUMN_LENGTH        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 292 )

/**
 * @brief columnar table does not support LONGVARCHAR and LONGVARBINARY
 */
#define QLL_ERRCODE_COLUMNAR_TABLE_NOT_SUPPORT_COLUMN_TYPE      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 293 )

/**
 * @brief cannot defer a constraint that is not deferrable
 */
#define QLL_ERRCODE_CANNOT_DEFER_CONSTRAINT_THAT_IS_NOT_DEFERRABLE     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 294 )
        
/**
 * @brief invalid NOT NULL constraint specified on an identity column
 */
#define QLL_ERRCODE_INVALID_NOT_NULL_CONSTRAINT_SPECIFIED_ON_IDENTITY_COLUMN     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 295 )

/**
 * @brief constraint '%s' is not deferrable
 */
#define QLL_ERRCODE_CONSTRAINT_IS_NOT_DEFERRABLE     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 296 )

/**
 * @brief duplicate constraint name specified
 */
#define QLL_ERRCODE_DUPLICATE_CONSTRAINT_NAME_SPECIFIED     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 297 )

/**
 * @brief cannot make primary key supplemental log data with primary key constraint violation
 */
#define QLL_ERRCODE_CANNOT_MAKE_PRIMARY_KEY_SUPPLEMENTAL_LOG_DATA_WITH_VIOLATION  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 298 )

/**
 * @brief datafile does not exist in database
 */
#define QLL_ERRCODE_DATAFILE_NOT_EXISTS_DATABASE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 299 )
        
/**
 * @brief too many expressions
 */
#define QLL_ERRCODE_TOO_MANY_EXPRESSIONS                        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 300 )

/**
 * @brief name '%s' is already used by an existing public synonym
 */
#define QLL_ERRCODE_USED_PUBLIC_SYNONYM_NAME                   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 301 )

/**
 * @brief private synonym '%s' to be droppped does not exist
 */
#define QLL_ERRCODE_PRIVATE_SYNONYM_NOT_EXIST                  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 302 )

/**
 * @brief public synonym '%s' to be droppped does not exist
 */
#define QLL_ERRCODE_PUBLIC_SYNONYM_NOT_EXIST                   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 303 )

/**
 * @brief looping chain of synonyms
 */
#define QLL_ERRCODE_LOOPING_CHAIN_SYNONYM_NAME                 \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 304 )        

/**
 * @brief synonym translation is no longer valid
 */
#define QLL_ERRCODE_SYNONYM_TRANSLATION_IS_NO_LONGER_VALID      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 305 )    

/**
 * @brief expression is out of iteration time scope
 */
#define QLL_ERRCODE_ITERATION_TIME_SCOPE_INVALID                \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 306 )    

/**
 * @brief constant value expression expected
 */
#define QLL_ERRCODE_CONSTANT_VALUE_EXPRESSION_EXPECTED          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 307 )

/**
 * @brief invalid or redundant resource
 */
#define QLL_ERRCODE_INVALID_PROFILE_PARAMETER                   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 308 )

/**
 * @brief the account is locked
 */
#define QLL_ERRCODE_ACCOUNT_LOCKED                 \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 309 )

/**
 * @brief the password will expire in %ld days
 */
#define QLL_ERRCODE_PASSWORD_WILL_EXPIRE_N_DAYS \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 310 )

/**
 * @brief the password will expire soon
 */
#define QLL_ERRCODE_PASSWORD_WILL_EXPIRE_SOON \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 311 )

/**
 * @brief the password has expired
 */
#define QLL_ERRCODE_PASSWORD_EXPIRED \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 312 )

/**
 * @brief the password cannot be reused
 */
#define QLL_ERRCODE_PASSWORD_CANNOT_REUSED \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 313 )

/**
 * @brief cannot lock \"SYS\" account
 */
#define QLL_ERRCODE_CANNOT_LOCK_SYS_ACCOUNT \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 314 )

/**
 * @brief profile '%s' already exists
 */
#define QLL_ERRCODE_USED_PROFILE_NAME                           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 315 )        
/**
 * @brief function '%s' not found
 */
#define QLL_ERRCODE_PASSWORD_VERIFY_FUNCTION_NOT_FOUND         \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 316 )
/**
 * @brief profile '%s' has users assigned, cannot drop without CASCADE
 */
#define QLL_ERRCODE_CANNOT_DROP_PROFILE_WITHOUT_CASCADE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 317 )

/**
 * @brief profile '%s' does not exists
 */
#define QLL_ERRCODE_PROFILE_NOT_EXISTS        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 318 )

/**
 * @brief cannot assign PROFILE to \"SYS\" account
 */
#define QLL_ERRCODE_CANNOT_ASSIGN_PROFILE_SYS_ACCOUNT \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 319 )

/**
 * @brief cannot assign DEFAULT parameter value to \"DEFAULT\" profile
 */
#define QLL_ERRCODE_CANNOT_ASSIGN_DEFAULT_VALUE_TO_DEFAULT_PROFILE \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 320 )
/**
 * @brief password length less than '%s'
 */
#define QLL_ERRCODE_PASSWORD_LENGTH                            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 321 )

/**
 * @brief password must contain at least '%s' letter(s)
 */
#define QLL_ERRCODE_PASSWORD_LETTER_COUNT                      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 322 )

/**
 * @brief password must contain at least uppercase '%s' character(s)
 */
#define QLL_ERRCODE_PASSWORD_UPPER_CHARACTER_COUNT             \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 323 )

/**
 * @brief password must contain at least lowercase '%s' character(s)
 */
#define QLL_ERRCODE_PASSWORD_LOWER_CHARACTER_COUNT             \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 324 )
        
/**
 * @brief password must contain at least '%s' digit(s)
 */
#define QLL_ERRCODE_PASSWORD_DIGIT_COUNT                       \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 325 )

/**
 * @brief password must contain at least '%s' special character(s)
 */
#define QLL_ERRCODE_PASSWORD_SPECIAL_COUNT                     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 326 )

/**
 * @brief password same as or similar to user name
 */
#define QLL_ERRCODE_PASSWORD_SAME_OR_SIMILAR_TO_USER_NAME      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 327 )

/**
 * @brief password contains the user name reversed
 */
#define QLL_ERRCODE_PASSWORD_CONTAIN_USER_NAME_REVERSED        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 328 )

/**
 * @brief password same as or similar to server name
 */
#define QLL_ERRCODE_PASSWORD_SAME_OR_SIMILAR_TO_DATABASE_NAME  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 329 )

/**
 * @brief password too simple
 */
#define QLL_ERRCODE_PASSWORD_TOO_SIMPLE                        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 330 )

/**
 * @brief Password should differ from the old password by at least '%s' characters
 */
#define QLL_ERRCODE_PASSWORD_DIFFER_FROM_OLD_PASSWORD         \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 331 )

/**
 * @brief constant value does not support LONGVARCHAR and LONGVARBINARY
 */
#define QLL_ERRCODE_CONSTANT_NOT_SUPPORT_DATA_TYPE              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 332 )
        
/**
 * @brief value of parameter number (%d) must be consistent datatype
 */
#define QLL_ERRCODE_BIND_PARAM_INCONSISTENT_DATATYPES           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 333 )

/**
 * @brief invalid value is prevented in parameter number (%d) value number (%d)
 */
#define QLL_ERRCODE_BIND_PARAM_INVALID_VALUE                    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_SQL_PROCESSOR, 334 )

#define QLL_MAX_ERROR      335

/** @} qllError */


/**
 * @defgroup qllDef Definition
 * @ingroup qlExternal
 * @{
 */

/** @} qllDef */


/*******************************************************************************
 * Node Definition
 ******************************************************************************/

/**
 * @defgroup qllNode Node Definition
 * @ingroup qlExternal
 * @{
 */

typedef enum
{
    /* statement */
    QLL_PLAN_NODE_STMT_SELECT_TYPE = 0,
    QLL_PLAN_NODE_STMT_INSERT_TYPE,
    QLL_PLAN_NODE_STMT_UPDATE_TYPE,
    QLL_PLAN_NODE_STMT_DELETE_TYPE,
    QLL_PLAN_NODE_STMT_MERGE_TYPE,

    /* query */
    QLL_PLAN_NODE_QUERY_SPEC_TYPE,
    QLL_PLAN_NODE_INDEX_BUILD_TYPE,
    QLL_PLAN_NODE_FOR_UPDATE_TYPE,
    QLL_PLAN_NODE_MERGE_TYPE,
    QLL_PLAN_NODE_VIEW_TYPE,
    QLL_PLAN_NODE_VIEW_PUSHED_PRED_TYPE,
    QLL_PLAN_NODE_FILTER_TYPE,
    QLL_PLAN_NODE_INLIST_TYPE,

    /* set */
    QLL_PLAN_NODE_QUERY_SET_TYPE,
    QLL_PLAN_NODE_UNION_ALL_TYPE,
    QLL_PLAN_NODE_UNION_DISTINCT_TYPE,
    QLL_PLAN_NODE_EXCEPT_ALL_TYPE,
    QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE,
    QLL_PLAN_NODE_INTERSECT_ALL_TYPE,
    QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE,

    /* sub query */
    QLL_PLAN_NODE_SUB_QUERY_TYPE,
    QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE,
    QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
    QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE,

    /* or */
    QLL_PLAN_NODE_CONCAT_TYPE,

    /* join */
    QLL_PLAN_NODE_NESTED_LOOPS_TYPE,
    QLL_PLAN_NODE_MERGE_JOIN_TYPE,
    QLL_PLAN_NODE_HASH_JOIN_TYPE,
    QLL_PLAN_NODE_INDEX_JOIN_TYPE,
    
    /* order by, group by, windows and etc. */
    QLL_PLAN_NODE_CONNECT_BY_TYPE,
    QLL_PLAN_NODE_PSEUDO_COLUMN_TYPE,
    QLL_PLAN_NODE_COUNT_TYPE,
    QLL_PLAN_NODE_WINDOW_TYPE,
    QLL_PLAN_NODE_HASH_TYPE,
    QLL_PLAN_NODE_HASH_AGGREGATION_TYPE,
    QLL_PLAN_NODE_GROUP_TYPE,

    /* scan */
    QLL_PLAN_NODE_TABLE_ACCESS_TYPE,
    QLL_PLAN_NODE_INDEX_ACCESS_TYPE,
    QLL_PLAN_NODE_ROWID_ACCESS_TYPE,
    QLL_PLAN_NODE_FAST_DUAL_ACCESS_TYPE,
    QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE,
    QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE,
    QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE,
    QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE,
    QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE,
    QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE,
    QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE,

    QLL_PLAN_NODE_TYPE_MAX,
    
} qllPlanNodeType;

        
/**
 * @brief Node Types
 */
typedef enum
{
    QLL_INVALID_TYPE = 0,
    
    /*
     * TYPES FOR EXECUTOR NODES
     */

    // QLL_INDEXINFO_TYPE,
    // QLL_EXPRCONTEXT_TYPE,
    // QLL_PROJECTIONINFO_TYPE,
    // QLL_JUNKFILTER_TYPE,
    // QLL_RESULTRELINFO_TYPE,
    // QLL_ESTATE_TYPE,
    // QLL_TUPLETABLESLOT_TYPE,

    /*
     * TYPES FOR PLAN NODES (???.H)
     */
    QLL_INDEX_SCAN_TYPE,    /* QLL_INDEXSCAN_TYPE */
    
    // QLL_PLAN_TYPE,
    // QLL_RESULT_TYPE,
    // QLL_MODIFYTABLE_TYPE,
    // QLL_APPEND_TYPE,
    // QLL_RECURSIVEUNION_TYPE,
    // QLL_BITMAPAND_TYPE,
    // QLL_BITMAPOR_TYPE,
    // QLL_SCAN_TYPE,
    // QLL_SEQSCAN_TYPE,
    // QLL_BITMAPINDEXSCAN_TYPE,
    // QLL_BITMAPHEAPSCAN_TYPE,
    // QLL_TIDSCAN_TYPE,
    // QLL_SUBQUERYSCAN_TYPE,
    // QLL_FUNCTIONSCAN_TYPE,
    // QLL_VALUESSCAN_TYPE,
    // QLL_CTESCAN_TYPE,
    // QLL_WORKTABLESCAN_TYPE,
    // QLL_JOIN_TYPE,
    // QLL_NESTLOOP_TYPE,
    // QLL_MERGEJOIN_TYPE,
    // QLL_HASHJOIN_TYPE,
    // QLL_MATERIAL_TYPE,
    // QLL_SORT_TYPE,
    // QLL_GROUP_TYPE,
    // QLL_AGG_TYPE,
    // QLL_WINDOWAGG_TYPE,
    // QLL_UNIQUE_TYPE,
    // QLL_HASH_TYPE,
    // QLL_SETOP_TYPE,
    // QLL_LOCKROWS_TYPE,
    // QLL_LIMIT_TYPE,
    // /* THESE AREN'T SUBCLASSES OF PLAN: */
    // QLL_PLANROWMARK_TYPE,
    // QLL_PLANINVALITEM_TYPE,

    /*
     * TYPES FOR PLAN STATE NODES (???.H)
     *
     * THESE SHOULD CORRESPOND ONE-TO-ONE WITH PLAN NODE TYPES.
     */
    // QLL_PLANSTATE_TYPE,
    // QLL_RESULTSTATE_TYPE,
    // QLL_MODIFYTABLESTATE_TYPE,
    // QLL_APPENDSTATE_TYPE,
    // QLL_RECURSIVEUNIONSTATE_TYPE,
    // QLL_BITMAPANDSTATE_TYPE,
    // QLL_BITMAPORSTATE_TYPE,
    // QLL_SCANSTATE_TYPE,
    // QLL_SEQSCANSTATE_TYPE,
    // QLL_INDEXSCANSTATE_TYPE,
    // QLL_BITMAPINDEXSCANSTATE_TYPE,
    // QLL_BITMAPHEAPSCANSTATE_TYPE,
    // QLL_TIDSCANSTATE_TYPE,
    // QLL_SUBQUERYSCANSTATE_TYPE,
    // QLL_FUNCTIONSCANSTATE_TYPE,
    // QLL_VALUESSCANSTATE_TYPE,
    // QLL_CTESCANSTATE_TYPE,
    // QLL_WORKTABLESCANSTATE_TYPE,
    // QLL_JOINSTATE_TYPE,
    // QLL_NESTLOOPSTATE_TYPE,
    // QLL_MERGEJOINSTATE_TYPE,
    // QLL_HASHJOINSTATE_TYPE,
    // QLL_MATERIALSTATE_TYPE,
    // QLL_SORTSTATE_TYPE,
    // QLL_GROUPSTATE_TYPE,
    // QLL_AGGSTATE_TYPE,
    // QLL_WINDOWAGGSTATE_TYPE,
    // QLL_UNIQUESTATE_TYPE,
    // QLL_HASHSTATE_TYPE,
    // QLL_SETOPSTATE_TYPE,
    // QLL_LOCKROWSSTATE_TYPE,
    // QLL_LIMITSTATE_TYPE,

    /*
     * TYPES FOR PRIMITIVE NODES (???.H)
     */
    QLL_QUERY_SET_NODE_TYPE,
    QLL_QUERY_SPEC_NODE_TYPE,
    QLL_JOIN_TYPE_INFO_TYPE,
    QLL_JOIN_SPECIFICATION_TYPE,
    QLL_BASE_TABLE_NODE_TYPE,
    QLL_SUB_TABLE_NODE_TYPE,
    QLL_JOIN_TABLE_NODE_TYPE,

    QLL_REL_INFO_TYPE,      /* QLL_RANGEVAR_TYPE */ 
    QLL_ALIAS_TYPE,         /* QLL_ALIAS_TYPE */
    QLL_PARAM_TYPE,
    QLL_INTO_CLAUSE_TYPE,
    QLL_INSDEL_RETURN_CLAUSE_TYPE, 
    QLL_UPDATE_RETURN_CLAUSE_TYPE, 
    QLL_FUNCTION_TYPE,      /* QLL_FUNCEXPR_TYPE */
    QLL_PSEUDO_COLUMN_TYPE,
    QLL_AGGREGATION_TYPE,
    QLL_DEFAULT_TYPE,
    QLL_FOR_UPDATE_LOCK_WAIT_TYPE,
    QLL_UPDATABILITY_TYPE,
    QLL_COMMENT_OBJECT_TYPE,
    QLL_DROP_CONSTRAINT_OBJECT_TYPE,
    QLL_ORDER_BY_TYPE,
    QLL_GROUP_BY_TYPE,
    QLL_GROUPING_ELEM_TYPE,
    QLL_HAVING_TYPE,
    QLL_CASE_SIMPLE_EXPR,      /* Simple Case */
    QLL_CASE_SEARCHED_EXPR,    /* Searched Case */
    QLL_NULLIF_EXPR,           /* NULLIF */    
    QLL_COALESCE_EXPR,         /* COALESCE */
    QLL_DECODE_EXPR,           /* DECODE */
    QLL_CASE2_EXPR,            /* CASE2 */
    QLL_NVL_EXPR,              /* NVL */
    QLL_NVL2_EXPR,             /* NVL2 */

    QLL_LIST_FUNCTION_TYPE,
    QLL_LIST_COLUMN_TYPE,
    QLL_ROW_EXPR_TYPE,
    
    // QLL_EXPR_TYPE,
    // QLL_VAR_TYPE,
    // QLL_CONST_TYPE,
    // QLL_AGGREF_TYPE,
    // QLL_WINDOWFUNC_TYPE,
    // QLL_ARRAYREF_TYPE,
    // QLL_NAMEDARGEXPR_TYPE,
    // QLL_OPEXPR_TYPE,
    // QLL_DISTINCTEXPR_TYPE,
    // QLL_SCALARARRAYOPEXPR_TYPE,
    // QLL_SUBLINK_TYPE,
    // QLL_SUBPLAN_TYPE,
    // QLL_ALTERNATIVESUBPLAN_TYPE,
    // QLL_FIELDSELECT_TYPE,
    // QLL_FIELDSTORE_TYPE,
    // QLL_RELABELTYPE_TYPE,
    // QLL_COERCEVIAIO_TYPE,
    // QLL_ARRAYCOERCEEXPR_TYPE,
    // QLL_CONVERTROWTYPEEXPR_TYPE,
    // QLL_CASEEXPR_TYPE,
    // QLL_CASEWHEN_TYPE,
    // QLL_CASETESTEXPR_TYPE,
    // QLL_ARRAYEXPR_TYPE,
    // QLL_ROWEXPR_TYPE,
    // QLL_ROWCOMPAREEXPR_TYPE,
    // QLL_COALESCEEXPR_TYPE,
    // QLL_MINMAXEXPR_TYPE,
    // QLL_XMLEXPR_TYPE,
    // QLL_NULLIFEXPR_TYPE,
    // QLL_NULLTEST_TYPE,
    // QLL_COERCETODOMAIN_TYPE,
    // QLL_COERCETODOMAINVALUE_TYPE,
    // QLL_SETTODEFAULT_TYPE,
    // QLL_CURRENTOFEXPR_TYPE,
    // QLL_TARGETENTRY_TYPE,
    // QLL_RANGETBLREF_TYPE,
    // QLL_JOINEXPR_TYPE,
    // QLL_FROMEXPR_TYPE,


    /*
     * TYPES FOR EXPRESSION STATE NODES (???.H)
     *
     * THESE CORRESPOND (NOT ALWAYS ONE-FOR-ONE) TO PRIMITIVE NODES DERIVED
     * FROM EXPR.
     */
    // QLL_EXPRSTATE_TYPE,
    // QLL_GENERICEXPRSTATE_TYPE,
    // QLL_AGGREFEXPRSTATE_TYPE,
    // QLL_WINDOWFUNCEXPRSTATE_TYPE,
    // QLL_ARRAYREFEXPRSTATE_TYPE,
    // QLL_FUNCEXPRSTATE_TYPE,
    // QLL_SCALARARRAYOPEXPRSTATE_TYPE,
    // QLL_BOOLEXPRSTATE_TYPE,
    // QLL_SUBPLANSTATE_TYPE,
    // QLL_ALTERNATIVESUBPLANSTATE_TYPE,
    // QLL_FIELDSELECTSTATE_TYPE,
    // QLL_FIELDSTORESTATE_TYPE,
    // QLL_COERCEVIAIOSTATE_TYPE,
    // QLL_ARRAYCOERCEEXPRSTATE_TYPE,
    // QLL_CONVERTROWTYPEEXPRSTATE_TYPE,
    // QLL_CASEEXPRSTATE_TYPE,
    // QLL_CASEWHENSTATE_TYPE,
    // QLL_ARRAYEXPRSTATE_TYPE,
    // QLL_ROWEXPRSTATE_TYPE,
    // QLL_ROWCOMPAREEXPRSTATE_TYPE,
    // QLL_COALESCEEXPRSTATE_TYPE,
    // QLL_MINMAXEXPRSTATE_TYPE,
    // QLL_XMLEXPRSTATE_TYPE,
    // QLL_NULLTESTSTATE_TYPE,
    // QLL_COERCETODOMAINSTATE_TYPE,
    // QLL_DOMAINCONSTRAINTSTATE_TYPE,

    /*
     * TYPES FOR PLANNER NODES (???.H)
     */
    // QLL_PLANNERINFO_TYPE,
    // QLL_PLANNERGLOBAL_TYPE,
    // QLL_RELOPTINFO_TYPE,
    // QLL_INDEXOPTINFO_TYPE,
    // QLL_PATH_TYPE,
    // QLL_INDEXPATH_TYPE,
    // QLL_BITMAPHEAPPATH_TYPE,
    // QLL_BITMAPANDPATH_TYPE,
    // QLL_BITMAPORPATH_TYPE,
    // QLL_NESTPATH_TYPE,
    // QLL_MERGEPATH_TYPE,
    // QLL_HASHPATH_TYPE,
    // QLL_TIDPATH_TYPE,
    // QLL_APPENDPATH_TYPE,
    // QLL_RESULTPATH_TYPE,
    // QLL_MATERIALPATH_TYPE,
    // QLL_UNIQUEPATH_TYPE,
    // QLL_EQUIVALENCECLASS_TYPE,
    // QLL_EQUIVALENCEMEMBER_TYPE,
    // QLL_PATHKEY_TYPE,
    // QLL_RESTRICTINFO_TYPE,
    // QLL_INNERINDEXSCANINFO_TYPE,
    // QLL_PLACEHOLDERVAR_TYPE,
    // QLL_SPECIALJOININFO_TYPE,
    // QLL_APPENDRELINFO_TYPE,
    // QLL_PLACEHOLDERINFO_TYPE,
    // QLL_PLANNERPARAMITEM_TYPE,

    /*
     * TYPES FOR MEMORY NODES (???.H)
     */
    // QLL_MEMORYCONTEXT_TYPE,
    // QLL_ALLOCSETCONTEXT_TYPE,

    /*
     * TYPES FOR VALUE NODES (???.H)
     */

    /*
     * QLL_BNF_CONSTANT_TYPE : 다음을 포괄
     */
    QLL_BNF_INTPARAM_CONSTANT_TYPE,       /**< parameter integer constant */
    QLL_BNF_NUMBER_CONSTANT_TYPE,         /**< numeric literal constant */
    QLL_BNF_STRING_CONSTANT_TYPE,         /**< string literal constant */
    QLL_BNF_BITSTR_CONSTANT_TYPE,         /**< bit string literal constant */
    QLL_BNF_NULL_CONSTANT_TYPE,           /**< untyped null constant */
    QLL_BNF_DATE_CONSTANT_TYPE,           /**< date literal constant */
    QLL_BNF_TIME_CONSTANT_TYPE,           /**< time literal constant */
    QLL_BNF_TIME_TZ_CONSTANT_TYPE,        /**< time with time zone literal constant */
    QLL_BNF_TIMESTAMP_CONSTANT_TYPE,      /**< timestamp literal constant */
    QLL_BNF_TIMESTAMP_TZ_CONSTANT_TYPE,   /**< timestamp with time zone literal constant */

    /*
     * TYPES FOR LIST NODES (???.H)
     */
    QLL_POINTER_LIST_TYPE,
    QLL_INTEGER_LIST_TYPE,
    QLL_OID_LIST_TYPE,

    /*
     * TYPES FOR STATEMENT NODES (???.H)
     */
    QLL_STMT_NA,
    QLL_STMT_ALTER_DATABASE_ADD_LOGFILE_GROUP_TYPE,     /* ALTER DATABASE ADD LOGFILE GROUP*/
    QLL_STMT_ALTER_DATABASE_ADD_LOGFILE_MEMBER_TYPE,    /* ALTER DATABASE ADD LOGFILE MEMBER */
    QLL_STMT_ALTER_DATABASE_DROP_LOGFILE_GROUP_TYPE,    /* ALTER DATABASE DROP LOGFILE GROUP */
    QLL_STMT_ALTER_DATABASE_DROP_LOGFILE_MEMBER_TYPE,   /* ALTER DATABASE DROP LOGFILE MEMBER */
    QLL_STMT_ALTER_DATABASE_RENAME_LOGFILE_TYPE,        /* ALTER DATABASE RENAME LOGFILE */
    QLL_STMT_ALTER_DATABASE_ARCHIVELOG_MODE_TYPE,       /* ALTER DATABASE ARCHIVELOG */
    QLL_STMT_ALTER_DATABASE_NOARCHIVELOG_MODE_TYPE,     /* ALTER DATABASE NOARCHIVELOG */
    QLL_STMT_ALTER_DATABASE_BACKUP_TYPE,                /* ALTER DATABASE BEGIN | END BACKUP */
    QLL_STMT_ALTER_DATABASE_DELETE_BACKUP_LIST_TYPE,    /* ALTER DATABASE DELETE BACKUP LIST */
    QLL_STMT_ALTER_DATABASE_BACKUP_INCREMENTAL_TYPE,    /* ALTER DATABASE BACKUP INCREMENTAL */
    QLL_STMT_ALTER_DATABASE_BACKUP_CTRLFILE_TYPE,       /* ALTER DATABASE BACKUP CONTROLFILE */
    QLL_STMT_ALTER_DATABASE_IRRECOVERABLE_TYPE,         /* ALTER DATABASE REGISTER IRRECOVERABLE SEGMENT */
    QLL_STMT_ALTER_DATABASE_RECOVER_DATABASE_TYPE,      /* ALTER DATABASE RECOVER */
    QLL_STMT_ALTER_DATABASE_INCOMPLETE_RECOVERY_TYPE,      /* ALTER DATABASE INCOMPLETE RECOVERY */
    QLL_STMT_ALTER_DATABASE_RECOVER_TABLESPACE_TYPE,    /* ALTER DATABASE RECOVER TABLESPACE .. */
    QLL_STMT_ALTER_DATABASE_RECOVER_DATAFILE_TYPE,      /* ALTER DATABASE RECOVER DATAFILE .. */
    QLL_STMT_ALTER_DATABASE_RESTORE_DATABASE_TYPE,      /* ALTER DATABASE RESTORE */
    QLL_STMT_ALTER_DATABASE_RESTORE_TABLESPACE_TYPE,    /* ALTER DATABASE RESTORE TABLESPACE .. */
    QLL_STMT_ALTER_DATABASE_RESTORE_CTRLFILE_TYPE,      /* ALTER DATABASE RESTORE CONTROLFILE FROM */
    QLL_STMT_ALTER_DATABASE_CLEAR_PASSWORD_HISTORY_TYPE, /* ALTER DATABASE CLEAR PASSWORD HISTORY */
    QLL_STMT_ALTER_DOMAIN_TYPE,                  /* ALTER DOMAIN */
    QLL_STMT_ALTER_FUNCTION_TYPE,                /* ALTER FUNCTION */
    QLL_STMT_ALTER_INDEX,                        /* ALTER INDEX */
    QLL_STMT_ALTER_METHOD_TYPE,                  /* ALTER METHOD */
    QLL_STMT_ALTER_PROCEDURE_TYPE,               /* ALTER PROCEDURE */
    QLL_STMT_ALTER_PROFILE_TYPE,                 /* ALTER PROFILE */
    QLL_STMT_ALTER_ROUTINE_TYPE,                 /* ALTER ROUTINE */
    QLL_STMT_ALTER_SEQUENCE_TYPE,                /* ALTER SEQUENCE */
    QLL_STMT_ALTER_SESSION_SET_TYPE,             /* ALTER SESSION SET */
    QLL_STMT_ALTER_SYSTEM_AGER_TYPE,             /* ALTER SYSTEM AGER */
    QLL_STMT_ALTER_SYSTEM_BREAKPOINT_TYPE,       /* ALTER SYSTEM AGER */
    QLL_STMT_ALTER_SYSTEM_SESS_NAME_TYPE,        /* ALTER SYSTEM SESSION NAME */
    QLL_STMT_ALTER_SYSTEM_STARTUP_DATABASE_TYPE, /* ALTER SYSTEM MOUNT/OPEN DATABASE */
    QLL_STMT_ALTER_SYSTEM_SHUTDOWN_DATABASE_ABORT_TYPE, /* ALTER SYSTEM CLOSE DATABASE_ABORT */
    QLL_STMT_ALTER_SYSTEM_SHUTDOWN_DATABASE_TYPE,/* ALTER SYSTEM CLOSE DATABASE */
    QLL_STMT_ALTER_SYSTEM_SWITCH_LOGFILE_TYPE,   /* ALTER SYSTEM SWITCH LOGFILE */
    QLL_STMT_ALTER_SYSTEM_CHECKPOINT_TYPE,       /* ALTER SYSTEM CHECKPOINT */
    QLL_STMT_ALTER_SYSTEM_DISCONNECT_SESSION_TYPE,  /* ALTER SYSTEM DISCONNECT SESSION */
    QLL_STMT_ALTER_SYSTEM_FLUSH_LOG_TYPE,        /* ALTER SYSTEM FLUSH LOG */
    QLL_STMT_ALTER_SYSTEM_CLEANUP_PLAN_TYPE,     /* ALTER SYSTEM CLEANUP PLAN */
    QLL_STMT_ALTER_SYSTEM_CLEANUP_SESSION_TYPE,  /* ALTER SYSTEM CLEANUP SESSION */
    QLL_STMT_ALTER_SYSTEM_KILL_SESSION_TYPE,     /* ALTER SYSTEM KILL SESSION */
    QLL_STMT_ALTER_SYSTEM_RESET_TYPE,            /* ALTER SYSTEM RESET */
    QLL_STMT_ALTER_SYSTEM_SET_TYPE,              /* ALTER SYSTEM SET */
    QLL_STMT_ALTER_SYSTEM_SET_UNDO_MEM_TBS_TYPE, /* ALTER SYSTEM SET UNDO_MEMORY_TBS */
    QLL_STMT_ALTER_TABLE_ADD_COLUMN_TYPE,        /* ALTER TABLE .. ADD COLUMN */
    QLL_STMT_ALTER_TABLE_SET_UNUSED_COLUMN_TYPE, /* ALTER TABLE .. SET UNUSED COLUMN */
    QLL_STMT_ALTER_TABLE_DROP_COLUMN_TYPE,       /* ALTER TABLE .. DROP COLUMN */
    QLL_STMT_ALTER_TABLE_DROP_UNUSED_COLUMNS_TYPE, /* ALTER TABLE .. DROP UNUSED COLUMNS */
    QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_DEFAULT_TYPE,    /* ALTER TABLE .. ALTER COLUMN .. SET DEFAULT */
    QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_DEFAULT_TYPE,   /* ALTER TABLE .. ALTER COLUMN .. DROP DEFAULT */
    QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_NOT_NULL_TYPE,   /* ALTER TABLE .. ALTER COLUMN .. SET NOT NULL */
    QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_NOT_NULL_TYPE,  /* ALTER TABLE .. ALTER COLUMN .. DROP NOT NULL */
    QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_DATA_TYPE_TYPE,  /* ALTER TABLE .. ALTER COLUMN .. SET DATA TYPE */
    QLL_STMT_ALTER_TABLE_ALTER_COLUMN_ALTER_IDENTITY_TYPE, /* ALTER TABLE .. ALTER COLUMN .. AS IDENTITY */
    QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_IDENTITY_TYPE,  /* ALTER TABLE .. ALTER COLUMN .. DROP IDENTITY */
    QLL_STMT_ALTER_TABLE_RENAME_COLUMN_TYPE,     /* ALTER TABLE .. RENAME COLUMN */
    QLL_STMT_ALTER_TABLE_PHYSICAL_ATTR_TYPE,     /* ALTER TABLE .. STORAGE .. */
    QLL_STMT_ALTER_TABLE_ADD_CONSTRAINT_TYPE,    /* ALTER TABLE .. ADD CONSTRAINT */
    QLL_STMT_ALTER_TABLE_ALTER_CONSTRAINT_TYPE,  /* ALTER TABLE .. ALTER CONSTRAINT */
    QLL_STMT_ALTER_TABLE_DROP_CONSTRAINT_TYPE,   /* ALTER TABLE .. DROP CONSTRAINT */
    QLL_STMT_ALTER_TABLE_RENAME_CONSTRAINT_TYPE, /* ALTER TABLE .. RENAME CONSTRAINT */
    QLL_STMT_ALTER_TABLE_RENAME_TABLE_TYPE,      /* ALTER TABLE .. RENAME TO .. */
    QLL_STMT_ALTER_TABLE_TABLESPACE_TYPE,        /* ALTER TABLE .. TABLESPACE */
    QLL_STMT_ALTER_TABLE_ADD_SUPPLOG_TYPE,       /* ALTER TABLE .. ADD SUPPLEMENTAL LOG */
    QLL_STMT_ALTER_TABLE_DROP_SUPPLOG_TYPE,      /* ALTER TABLE .. DROP SUPPLEMENTAL LOG */
    QLL_STMT_ALTER_INDEX_PHYSICAL_ATTR_TYPE,     /* ALTER INDEX .. STORAGE .. */
    QLL_STMT_ALTER_TABLESPACE_ADD_TYPE,          /* ALTER TABLESPACE .. ADD  */
    QLL_STMT_ALTER_TABLESPACE_BACKUP_TYPE,       /* ALTER TABLESPACE .. BACKUP  */
    QLL_STMT_ALTER_TABLESPACE_BACKUP_INCREMENTAL_TYPE,   /* ALTER TABLESPACE .. BACKUP INCREMENTAL */
    QLL_STMT_ALTER_TABLESPACE_DROP_TYPE,         /* ALTER TABLESPACE .. DROP */
    QLL_STMT_ALTER_TABLESPACE_ONLINE_TYPE,       /* ALTER TABLESPACE .. ONLINE */
    QLL_STMT_ALTER_TABLESPACE_OFFLINE_TYPE,      /* ALTER TABLESPACE .. OFFLINE */
    QLL_STMT_ALTER_TABLESPACE_RENAME_TYPE,       /* ALTER TABLESPACE .. RENAME TO */
    QLL_STMT_ALTER_TABLESPACE_RENAME_FILE_TYPE,  /* ALTER TABLESPACE .. RENAME FILE */
    QLL_STMT_ALTER_TABLESPACE_SHRINK_TYPE,       /* ALTER TABLESPACE .. SHRINK */
    QLL_STMT_ALTER_TRANSFORM_FOR_TYPE,           /* ALTER TRANSFORM FOR */
    QLL_STMT_ALTER_TYPE_TYPE,                    /* ALTER TYPE */
    QLL_STMT_ALTER_USER_TYPE,                    /* ALTER USER */
    QLL_STMT_ALTER_VIEW_TYPE,                    /* ALTER VIEW */
    QLL_STMT_CALL_TYPE,                          /* CALL routine*/
    QLL_STMT_CLOSE_CURSOR_TYPE,                  /* CLOSE cursor */
    QLL_STMT_COMMENT_ON_TYPE,                    /* COMMENT ON .. IS */
    QLL_STMT_COMMIT_TYPE,                        /* COMMIT */
    QLL_STMT_COMMIT_FORCE_TYPE,                  /* COMMIT FORCE ... */
    QLL_STMT_CONNECT_TO_TYPE,                    /* CONNECT TO server_name */
    QLL_STMT_COPY_FROM_TYPE,                     /* COPY .. FROM .. */
    QLL_STMT_COPY_TO_TYPE,                       /* COPY .. TO .. */
    QLL_STMT_CREATE_ASSERTION_TYPE,              /* CREATE ASSERTION */
    QLL_STMT_CREATE_CAST_TYPE,                   /* CREATE CAST */
    QLL_STMT_CREATE_CHARACTER_SET_TYPE,          /* CREATE CHARACTER SET */
    QLL_STMT_CREATE_COLLATION_TYPE,              /* CREATE COLLATION */
    QLL_STMT_CREATE_DATABASE_TYPE,               /* CREATE DATABASE */
    QLL_STMT_CREATE_DOMAIN_TYPE,                 /* CREATE DOMAIN */
    QLL_STMT_CREATE_FUNCTION_TYPE,               /* CREATE FUNCTION */
    QLL_STMT_CREATE_INDEX_TYPE,                  /* CREATE INDEX */
    QLL_STMT_CREATE_METHOD_TYPE,                 /* CREATE METHOD */
    QLL_STMT_CREATE_ORDERING_FOR_TYPE,           /* CREATE ORDERING FOR */
    QLL_STMT_CREATE_PROCEDURE_TYPE,              /* CREATE PROCEDURE */
    QLL_STMT_CREATE_PROFILE_TYPE,                /* CREATE PROFILE */
    QLL_STMT_CREATE_ROLE_TYPE,                   /* CREATE ROLE */
    QLL_STMT_CREATE_SCHEMA_TYPE,                 /* CREATE SCHEMA */
    QLL_STMT_CREATE_SEQUENCE_TYPE,               /* CREATE SEQUENCE */
    QLL_STMT_CREATE_SYNONYM_TYPE,                /* CREATE SYNONYM */
    QLL_STMT_CREATE_TABLE_TYPE,                  /* CREATE TABLE */
    QLL_STMT_CREATE_TABLE_AS_SELECT_TYPE,        /* CREATE TABLE .. AS SELECT */
    QLL_STMT_CREATE_TABLESPACE_TYPE,             /* CREATE TABLESPACE */
    QLL_STMT_CREATE_TRANSFORM_FOR_TYPE,          /* CREATE TRANSFORM_FOR */
    QLL_STMT_CREATE_TRANSLATION_TYPE,            /* CREATE TRANSLATION */
    QLL_STMT_CREATE_TRIGGER_TYPE,                /* CREATE TRIGGER */
    QLL_STMT_CREATE_TYPE_TYPE,                   /* CREATE TYPE */
    QLL_STMT_CREATE_USER_TYPE,                   /* CREATE USER */
    QLL_STMT_CREATE_VIEW_TYPE,                   /* CREATE VIEW */
    QLL_STMT_DECLARE_CURSOR_TYPE,                /* DECLARE .. CURSOR */
    QLL_STMT_DECLARE_LOCAL_TEMPORARY_TABLE_TYPE, /* DECLARE LOCAL TEMPORARY TABLE */
    QLL_STMT_DELETE_TYPE,                        /* DELETE FROM */
    QLL_STMT_DELETE_RETURNING_QUERY_TYPE,        /* DELETE FROM .. RETURNING */
    QLL_STMT_DELETE_RETURNING_INTO_TYPE,         /* DELETE FROM .. RETURNING .. INTO */
    QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE,       /* DELETE FROM .. WHERE CURRENT OF cursor*/
    QLL_STMT_DISCONNECT_TYPE,                    /* DISCONNECT */
    QLL_STMT_DROP_ASSERTION_TYPE,                /* DROP ASSERTION */
    QLL_STMT_DROP_CAST_TYPE,                     /* DROP CAST */
    QLL_STMT_DROP_CHARACTER_SET_TYPE,            /* DROP CHARACTER SET */
    QLL_STMT_DROP_COLLATION_TYPE,                /* DROP COLLATION */
    QLL_STMT_DROP_DATABASE_TYPE,                 /* DROP DATABASE */
    QLL_STMT_DROP_DOMAIN_TYPE,                   /* DROP DOMAIN */
    QLL_STMT_DROP_FUNCTION_TYPE,                 /* DROP FUNCTION */
    QLL_STMT_DROP_INDEX_TYPE,                    /* DROP INDEX */
    QLL_STMT_DROP_METHOD_TYPE,                   /* DROP METHOD */
    QLL_STMT_DROP_ORDERING_FOR_TYPE,             /* DROP ORDERING FOR */
    QLL_STMT_DROP_PROCEDURE_TYPE,                /* DROP PROCEDURE */
    QLL_STMT_DROP_PROFILE_TYPE,                  /* DROP PROFILE */
    QLL_STMT_DROP_ROLE_TYPE,                     /* DROP ROLE */
    QLL_STMT_DROP_ROUTINE_TYPE,                  /* DROP ROUTINE */
    QLL_STMT_DROP_SCHEMA_TYPE,                   /* DROP SCHEMA */
    QLL_STMT_DROP_SEQUENCE_TYPE,                 /* DROP SEQUENCE */
    QLL_STMT_DROP_SYNONYM_TYPE,                  /* DROP SYNONYM */
    QLL_STMT_DROP_TABLE_TYPE,                    /* DROP TABLE */
    QLL_STMT_DROP_TABLESPACE_TYPE,               /* DROP TABLESPACE */
    QLL_STMT_DROP_TRANSFORM_FOR_TYPE,            /* DROP TRANSFORM FOR */
    QLL_STMT_DROP_TRANSLATION_TYPE,              /* DROP TRANSLATION */
    QLL_STMT_DROP_TRIGGER_TYPE,                  /* DROP TRIGGER */
    QLL_STMT_DROP_TYPE_TYPE,                     /* DROP TYPE */
    QLL_STMT_DROP_USER_TYPE,                     /* DROP USER */
    QLL_STMT_DROP_VIEW_TYPE,                     /* DROP VIEW */
    QLL_STMT_FETCH_CURSOR_TYPE,                  /* FETCH cursor */
    QLL_STMT_FETCH_LOCATOR_TYPE,                 /* FETCH LOCATOR locator */
    QLL_STMT_GET_DIAGNOSTICS_TYPE,               /* GET DIAGNOSTICS */
    QLL_STMT_GRANT_DATABASE_TYPE,                /* GRANT .. ON DATABASE */
    QLL_STMT_GRANT_TABLESPACE_TYPE,              /* GRANT .. ON TABLESPACE */
    QLL_STMT_GRANT_SCHEMA_TYPE,                  /* GRANT .. ON SCHEMA */
    QLL_STMT_GRANT_TABLE_TYPE,                   /* GRANT .. ON TABLE */
    QLL_STMT_GRANT_USAGE_TYPE,                   /* GRANT USAGE ON .. */
    QLL_STMT_GRANT_ROLE_TYPE,                    /* GRANT role TO */
    QLL_STMT_HOLD_LOCATOR_TYPE,                  /* HOLD LOCATOR */
    QLL_STMT_INSERT_TYPE,                        /* INSERT INTO */
    QLL_STMT_INSERT_RETURNING_QUERY_TYPE,        /* INSERT INTO .. RETURNING */
    QLL_STMT_INSERT_RETURNING_INTO_TYPE,         /* INSERT INTO .. RETURNING .. INTO */
    QLL_STMT_INSERT_SELECT_TYPE,                 /* INSERT .. SELECT */
    QLL_STMT_MERGE_INTO_TYPE,                    /* MERGE INTO */
    QLL_STMT_MODULE_TYPE,                        /* MODULE module */
    QLL_STMT_LOCK_TABLE_TYPE,                    /* LOCK TABLE */
    QLL_STMT_OPEN_CURSOR_TYPE,                   /* OPEN cursor */
    QLL_STMT_PROCEDURE_TYPE,                     /* PROCEDURE procedure */
    QLL_STMT_RELEASE_SAVEPOINT_TYPE,             /* RELEASE SAVEPOINT */
    QLL_STMT_RETURN_TYPE,                        /* RETURN value */
    QLL_STMT_REVOKE_DATABASE_TYPE,               /* REVOKE .. ON DATABASE */
    QLL_STMT_REVOKE_TABLESPACE_TYPE,             /* REVOKE .. ON TABLESPACE */
    QLL_STMT_REVOKE_SCHEMA_TYPE,                 /* REVOKE .. ON SCHEMA */
    QLL_STMT_REVOKE_TABLE_TYPE,                  /* REVOKE .. ON TABLE */
    QLL_STMT_REVOKE_USAGE_TYPE,                  /* REVOKE USAGE ON .. */
    QLL_STMT_REVOKE_ROLE_TYPE,                   /* REVOKE role TO */
    QLL_STMT_ROLLBACK_TYPE,                      /* ROLLBACK */
    QLL_STMT_ROLLBACK_FORCE_TYPE,                /* ROLLBACK FORCE ... */
    QLL_STMT_ROLLBACK_TO_TYPE,                   /* ROLLBACK TO savepoint */
    QLL_STMT_SAVEPOINT_TYPE,                     /* SAVEPOINT savepoint */
    QLL_STMT_SELECT_TYPE,                        /* SELECT STATEMENT */
    QLL_STMT_SELECT_FOR_UPDATE_TYPE,             /* SELECT .. FOR UPDATE */
    QLL_STMT_SELECT_INTO_TYPE,                   /* SELECT .. INTO */
    QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE,        /* SELECT .. INTO .. FOR UPDATE */
    QLL_STMT_SET_CATALOG_TYPE,                   /* SET CATALOG */
    QLL_STMT_SET_COLLATION_TYPE,                 /* SET COLLATION */
    QLL_STMT_SET_CONSTRAINTS_TYPE,               /* SET CONSTRAINTS */
    QLL_STMT_SET_NAMES_TYPE,                     /* SET NAMES */
    QLL_STMT_SET_PATH_TYPE,                      /* SET PATH */
    QLL_STMT_SET_ROLE_TYPE,                      /* SET ROLE */
    QLL_STMT_SET_SCHEMA_TYPE,                    /* SET SCHEMA */
    QLL_STMT_SET_SESSION_AUTHORIZATION_TYPE,     /* SET AUTHORIZATION */
    QLL_STMT_SET_SESSION_CHARACTERISTICS_TYPE,   /* SET SESSION CHARACTERISTICS */
    QLL_STMT_SET_TIMEZONE_TYPE,                  /* SET TIMEZONE */
    QLL_STMT_SET_TRANSACTION_TYPE,               /* SET TRANSACTION */
    QLL_STMT_SET_TRANSFORM_GROUP_FOR_TYPE,       /* SET TRANSFORM GROUP FOR */
    QLL_STMT_START_TRANSACTION_TYPE,             /* START TRANSACTION */
    QLL_STMT_TRUNCATE_TABLE_TYPE,                /* TRUNCATE TABLE */
    QLL_STMT_UPDATE_TYPE,                        /* UPDATE */
    QLL_STMT_UPDATE_RETURNING_QUERY_TYPE,        /* UPDATE .. RETURNING */
    QLL_STMT_UPDATE_RETURNING_INTO_TYPE,         /* UPDATE .. RETURNING .. INTO */
    QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE,       /* UPDATE .. WHERE CURRENT OF cursor */
    
    QLL_STMT_MAX,

    /*
     * TYPES FOR PHRASE 
     */

    QLL_PHRASE_VIEWED_TABLE_TYPE,               /* VIEW v1 ( c1, c2, ... ) */
    QLL_PHRASE_DEFAULT_EXPR_TYPE,               /* DEFAULT 0.123 */
    QLL_PHRASE_CHECK_CONSTRAINT_TYPE,           /* CHECK c1 > 0 */
    
    /*
     * TYPES FOR PARSE TREE NODES (???.H)
     */
    QLL_BNF_CONSTANT_TYPE,     /* QLL_A_CONST_TYPE */
    QLL_COLUMN_DEF_TYPE,       /* QLL_COLUMNDEF_TYPE */
    QLL_CONSTRAINT_TYPE,
    QLL_TYPE_NAME_TYPE,        /*  QLL_TYPENAME_TYPE */
    QLL_TYPECAST_TYPE,
    QLL_DEF_ELEM_TYPE,         /* QLL_DEFELEM_TYPE */
    QLL_TARGET_TYPE,           /* QLL_RESTARGET */
    QLL_ROW_TARGET_TYPE,       /* QLL_ROW_RESTARGET */
    QLL_TARGET_ASTERISK_TYPE,  /* QLL_RESTARGET_WITH_A_STAR */
    QLL_COLUMN_REF_TYPE,       /* QLL_COLUMNREF_TYPE */
    QLL_HINT_TYPE,
    QLL_INDEX_ELEMENT_TYPE,    /* QLL_INDEXELEM_TYPE */
    QLL_QUALIFIER_TYPE,
    QLL_INTERVAL_DESC_TYPE,
    
    // QLL_A_EXPR_TYPE,
    // QLL_PARAMREF_TYPE,
    // QLL_FUNCCALL_TYPE,
    // QLL_A_INDICES_TYPE,
    // QLL_A_INDIRECTION_TYPE,
    // QLL_A_ARRAYEXPR_TYPE,
    // QLL_SORTBY_TYPE,
    // QLL_WINDOWDEF_TYPE,
    // QLL_RANGESUBSELECT_TYPE,
    // QLL_RANGEFUNCTION_TYPE,
    // QLL_RANGETBLENTRY_TYPE,
    // QLL_SORTGROUPCLAUSE_TYPE,
    // QLL_WINDOWCLAUSE_TYPE,
    // QLL_PRIVGRANTEE_TYPE,
    // QLL_FUNCWITHARGS_TYPE,
    // QLL_ACCESSPRIV_TYPE,
    // QLL_CREATEOPCLASSITEM_TYPE,
    // QLL_INHRELATION_TYPE,
    // QLL_FUNCTIONPARAMETER_TYPE,
    // QLL_LOCKINGCLAUSE_TYPE,
    // QLL_ROWMARKCLAUSE_TYPE,
    // QLL_XMLSERIALIZE_TYPE,
    // QLL_WITHCLAUSE_TYPE,
    // QLL_COMMONTABLEEXPR_TYPE,


    /*
     * TYPES FOR RANDOM OTHER STUFF
     *
     * THESE ARE OBJECTS THAT AREN'T PART OF PARSE/PLAN/EXECUTE NODE TREE
     * STRUCTURES, BUT WE GIVE THEM NODETYPES ANYWAY FOR IDENTIFICATION
     * PURPOSES (USUALLY BECAUSE THEY ARE INVOLVED IN APIS WHERE WE WANT TO
     * PASS MULTIPLE OBJECT TYPES THROUGH THE SAME POINTER).
     */
    // QLL_TRIGGERDATA_TYPE,           /* IN ???.H */
    // QLL_RETURNSETINFO_TYPE,         /* IN ???.H */
    // QLL_WINDOWOBJECTDATA_TYPE,      /* PRIVATE IN ???.C */
    // QLL_TIDBITMAP_TYPE,             /* IN ???.H */
    // QLL_INLINECODEBLOCK_TYPE,       /* IN ???.H */

    /*
     * FOR PARSER
     */
    QLL_OBJECT_NAME_TYPE,
    QLL_ORDERING,
    QLL_SEQUENCE_NAME_TYPE,
    QLL_COLUMN_NAME_TYPE,
    QLL_CHAR_LENGTH_TYPE,
    QLL_COL_DEF_SECOND_TYPE,
    QLL_REF_TRIGGER_ACTION_TYPE,
    QLL_DATAFILE_TYPE,
    QLL_TABLESPACE_ATTR_TYPE,
    QLL_AUTO_EXTEND_TYPE,
    QLL_SEQUENCE_OPTION_TYPE,
    QLL_INSERT_SOURCE_TYPE,
    QLL_CONSTRAINT_ATTR__TYPE,
    QLL_VALUE_EXPR_TYPE,
    QLL_SIZE_TYPE,
    QLL_SET_PARAM_TYPE,
    QLL_SET_PARAM_ATTR_TYPE,
    QLL_SET_TRANSACTION_MODE,
    QLL_BREAKPOINT_ACTION_TYPE,
    QLL_BREAKPOINT_OPTION_TYPE,
    QLL_PRIV_ACTION_TYPE,
    QLL_PRIV_OBJECT_TYPE,
    QLL_PRIVILEGE_TYPE,
    QLL_PASSWORD_PARAMETER_TYPE,

    QLL_NODE_TYPE_MAX
} qllNodeType; 



/**
 * @brief Node의 type 정보
 * @remark 모든 Node에서 공통 헤더로 사용
 */
typedef struct qllNode 
{
    qllNodeType   mType;        /**< node type */
    stlInt32      mNodePos;     /**< node position : qlpList는 제외 */
} qllNode;


/**
 * @brief Node의 type(qllNodeType) 정보 얻기.
 * Node의 첫 번째 필드는 항상 qllNodeType 정보이다.
 */
#define QLL_NODE_TYPE( aNode )    ( ((qllNode*)(aNode))->mType )


/**
 * @brief Node에 NodeType를 설정
 */
#define QLL_SET_NODE_TYPE( aNode, aType )  ( QLL_NODE_TYPE(aNode) = (aType) )


/**
 * @brief Node에 설정된 TYPE이 주어진 TYPE과 일치하는지 여부 판단
 */
#define QLL_IS_A_TYPE( aNode, aType )  ( QLL_NODE_TYPE( aNode ) == (aType) ? STL_TRUE : STL_FALSE )


/** @} qllNodes */


/*******************************************************************************
 * Common Definition
 ******************************************************************************/

/**
 * @defgroup qllEnvironment Environment
 * @ingroup qlExternal
 */



/**
 * @defgroup qllFixedTable Fixed Tables of SQL layer
 * @ingroup qllEnvironment
 * @{
 */

/** @} qllFixedTable */






/**
 * @defgroup qllProcEnv Process Environment
 * @ingroup qllEnvironment
 * @{
 */

   
/**
 * @brief SQL Processor 의 Process Environment 구조체
 * @warning Execute 단계에서 메모리 할당/해제가 발생하지 않도록 하자.
 * - Parsing Memory        : 비공유, Parsing 과정에서 사용하는 임시 메모리 
 * - Candidate Plan Memory : 비공유, Optimize 과정에서 생성되는 다양한 Plan 
 * - Execution Memory      : 비공유, Atomic Statement 처리를 위한 메모리 
 */
typedef struct qllEnv
{
    ellEnv            mEllEnv;           /**< 하위 Layer 의 Process Environment */

    stlChar           mMessage[STL_MAX_ERROR_MESSAGE];

    /*
     * Heap 영역 Memory
     */
    
    knlRegionMem      mHeapMemParsing;      /**< Paring, Validation 을 위한 메모리 관리자 */
    knlRegionMem      mHeapMemRunning;      /**< Running 을 위한 메모리 관리자 for
                                             * Candidate Plan, Execution Node, DDL Execution
                                             */

    /*
     * for X$SQL_PROC_ENV fixed table
     */

    stlInt64          mParsingCnt;       /**< Parsing Call Count */
    stlInt64          mValidateCnt;      /**< Validate Call Count */
    stlInt64          mBindContextCnt;   /**< Bind Context Call Count */
    stlInt64          mOptCodeCnt;       /**< Code Optimize Call Count */
    stlInt64          mOptDataCnt;       /**< Data Optimize Call Count */
    stlInt64          mRecheckCnt;       /**< Recheck Call Count */
    stlInt64          mExecuteCnt;       /**< Execute Call Count */
    stlInt64          mRecompileCnt;     /**< Recompile Call Count */
    stlInt64          mFetchCnt;         /**< Fetch Call Count */

    stlInt64          mExecStmtCnt[QLL_STMT_MAX - QLL_STMT_NA];
    
} qllEnv;


/**
 * @brief SQL Processor Environment
 */
#define QLL_ENV( aEnv )      ( (qllEnv *)aEnv )

/**
 * @brief SQL Processor Environment에서 ErrorStack을 얻는 매크로
 */
#define QLL_ERROR_STACK( aEnv ) KNL_ERROR_STACK( aEnv )


/** @} qllProcEnv */

/** @} qllEnvironment */


/**
 * @defgroup qllDBCStmt DBC Statement
 * @ingroup qllEnvironment
 * @{
 */

/**
 * @brief DBC Statement 자료 구조
 */
typedef struct qllDBCStmt
{
    /*
     * Heap or Shared 영역 Memory
     */

    /* non-plan 용 memory */
    knlRegionMem      mShareMemSyntaxSQL;   /**< SQL Syntax 저장을 위한 메모리 관리자 */
    knlRegionMem      mShareMemStmt;        /**< Statement life-time 을 위한 메모리 관리자 */

    /* plan 용 memory */
    knlRegionMem      mShareMemInitPlan;    /**< Validation 을 위한 메모리 관리자 */
    knlRegionMem    * mShareMemCodePlan;    /**< Code Plan 을 위한 메모리 관리자 */
    knlRegionMem      mShareMemDataPlan;    /**< Data Plan 을 위한 메모리 관리자 */

    knlRegionMem      mPlanCacheMem;        /**< Init Plan 을 위한 메모리 관리자 */
    knlRegionMem      mNonPlanCacheMem;     /**< Init Plan 을 위한 메모리 관리자 */
    
} qllDBCStmt;


/****************************************************
 * Memory Manager MACRO
 ****************************************************/

#define QLL_PARSE_MEM(aEnv)         ((aEnv)->mHeapMemParsing)
#define QLL_VALIDATE_MEM(aEnv)      ((aEnv)->mHeapMemParsing)
#define QLL_CANDIDATE_MEM(aEnv)     ((aEnv)->mHeapMemRunning)
#define QLL_EXEC_DDL_MEM(aEnv)      ((aEnv)->mHeapMemRunning)
#define QLL_EXEC_DML_MEM(aEnv)      ((aEnv)->mHeapMemRunning)

#define QLL_INIT_PLAN(aDBCStmt)         (&(aDBCStmt)->mShareMemInitPlan)
#define QLL_CODE_PLAN(aDBCStmt)         ((aDBCStmt)->mShareMemCodePlan)
#define QLL_DATA_PLAN(aDBCStmt)         ((aDBCStmt)->mShareMemDataPlan)


/****************************************************
 * Heap Memory 
 ****************************************************/

/**
 * Parsing 을 위한 메모리 크기의 단위
 */
#define QLL_PARSING_MEM_INIT_SIZE       ( 16 * 1024 )
#define QLL_PARSING_MEM_NEXT_SIZE       ( 16 * 1024 )

/**
 * Data Plan 을 위한 메모리 크기의 단위
 */
#define QLL_DATA_PLAN_MEM_INIT_SIZE     ( 32 * 1024 )
#define QLL_DATA_PLAN_MEM_NEXT_SIZE     ( 32 * 1024 )

/**
 * Running 을 위한 메모리 크기의 단위
 */
#define QLL_RUNNING_MEM_INIT_SIZE       ( 16 * 1024 )
#define QLL_RUNNING_MEM_NEXT_SIZE       ( 16 * 1024 )



/****************************************************
 * Shared or Heap Memory 
 ****************************************************/

/**
 * Retry SQL 을 위한 메모리 크기의 단위
 */
#define QLL_RETRY_MEM_INIT_SIZE         ( 4 * 1024 )
#define QLL_RETRY_MEM_NEXT_SIZE         ( 4 * 1024 )

/**
 * Init Plan 을 위한 메모리 크기의 단위
 */
#define QLL_INIT_PLAN_MEM_INIT_SIZE     ( 16 * 1024 )
#define QLL_INIT_PLAN_MEM_NEXT_SIZE     ( 16 * 1024 )

/**
 * Non-Cache Plan 을 위한 메모리 크기의 단위
 */
#define QLL_NON_CACHE_PLAN_MEM_INIT_SIZE     ( 16 * 1024 )
#define QLL_NON_CACHE_PLAN_MEM_NEXT_SIZE     ( 16 * 1024 )

/**
 * Statement를 위한 메모리 크기의 단위
 */
#define QLL_STMT_MEM_INIT_SIZE         ( 4 * 1024 )
#define QLL_STMT_MEM_NEXT_SIZE         ( 4 * 1024 )

/**
 * Plan Cache 를 위한 메모리 크기의 단위
 */
#define QLL_NEXT_PLAN_CACHE_MEM_NEXT_SIZE ( 4 * 1024 )

/****************************************************
 * System Memory
 ****************************************************/

/**
 * XA Context 를 위한 메모리 크기의 단위
 */
#define QLL_XA_CONTEXT_MEM_INIT_SIZE         ( 1 * 1024 )
#define QLL_XA_CONTEXT_MEM_NEXT_SIZE         ( 1 * 1024 )

/****************************************************
 * Session Memory
 ****************************************************/

/**
 * Deferred Constraint 을 위한 메모리 크기의 단위
 */
#define QLL_COLLISION_MEM_INIT_SIZE     ( 1 * 1024 )
#define QLL_COLLISION_MEM_NEXT_SIZE     ( 1 * 1024 )

#define QLL_NLS_MEM_INIT_SIZE           ( 1 * 1024 )
#define QLL_NLS_MEM_NEXT_SIZE           ( 1 * 1024 )

#define QLL_PARENT_DBC_MEM_INIT_SIZE            \
    ( ( QLL_RETRY_MEM_INIT_SIZE + QLL_INIT_PLAN_MEM_INIT_SIZE +                                      \
        QLL_NON_CACHE_PLAN_MEM_INIT_SIZE + QLL_DATA_PLAN_MEM_INIT_SIZE + QLL_STMT_MEM_INIT_SIZE ) * 2 )

#define QLL_PARENT_DBC_MEM_NEXT_SIZE            \
    ( ( QLL_RETRY_MEM_NEXT_SIZE + QLL_INIT_PLAN_MEM_NEXT_SIZE +                                  \
        QLL_NON_CACHE_PLAN_MEM_NEXT_SIZE + QLL_DATA_PLAN_MEM_NEXT_SIZE + QLL_STMT_MEM_NEXT_SIZE ) * 2 )

/** @} qllDBCStmt */



/**
 * @defgroup qllSharedEntry Shared Entry of SQL Layer
 * @ingroup qllEnvironment
 * @{
 */

/**
 * @brief SQL layer 가 공유하는 자료 구조
 */
typedef struct qllSharedEntry
{
    knlDynamicMem     mMemContextXA;       
    
    /*
     * for X$SQL_SYST_ENV fixed table
     */
    
    stlInt64          mParsingCnt;       /**< Parsing Call Count */
    stlInt64          mValidateCnt;      /**< Validate Call Count */
    stlInt64          mBindContextCnt;   /**< Bind Context Call Count */
    stlInt64          mOptCodeCnt;       /**< Code Optimize Call Count */
    stlInt64          mOptDataCnt;       /**< Data Optimize Call Count */
    stlInt64          mRecheckCnt;       /**< Recheck Call Count */
    stlInt64          mExecuteCnt;       /**< Execute Call Count */
    stlInt64          mRecompileCnt;     /**< Recompile Call Count */
    stlInt64          mFetchCnt;         /**< Fetch Call Count */

    /*
     * for X$SQL_SYST_EXEC_STMT fixed table
     */

    stlInt64          mExecStmtCnt[QLL_STMT_MAX - QLL_STMT_NA];
} qllSharedEntry;



/**
 * @brief Shared Memory 상의 SQL layer 를 위한 Memory 시작 공간
 */
extern qllSharedEntry * gQllSharedEntry;

/**
 * @brief Environment 초기화시 사용할 Global Vector 
 */
extern dtlFuncVector * gQllVectorPtr;

/** @} qllSharedEntry */


/**
 * @defgroup qllAPI SQL API List
 * @ingroup qlExternal
 * @{
 */


/**
 * @defgroup qllSession SQL Session
 * @ingroup qllAPI
 * @{
 */

/** @} qllSession */

/**
 * @defgroup qllParsing SQL Parsing
 * @ingroup qllAPI
 * @{
 */

/** @} qllParsing */

/**
 * @defgroup qllValidation SQL Validation
 * @ingroup qllAPI
 * @{
 */

#define QLL_MAX_LIST_COLUMN_ARGS    ( 1024 )    /**< List Column 내의 최대 argument 개수  */
#define QLL_MAX_ROW_EXPR_ARGS       ( 1024 )    /**< Row 내의 최대 argument 개수 */

/** @} qllValidation */


/**
 * @defgroup qllBinding SQL Binding
 * @ingroup qllAPI
 * @{
 */

/** @} qllBinding */


/**
 * @defgroup qllDeferrableConstraint Deferrable Constraint
 * @ingroup qllAPI
 * @{
 */

/**
 * @brief SET SESSION CHARACTERISTICS AS TRANSACTION UNIQUE INTEGERITY IS 유형
 */
typedef enum qllUniqueMode
{
    QLL_UNIQUE_MODE_NONE = 0,

    QLL_UNIQUE_MODE_DEFAULT,  /**< SET SESSION CHARACTERISTICS AS TRANSACTION UNIQUE INTEGERITY IS DEFAULT */
    QLL_UNIQUE_MODE_DEFERRED, /**< SET SESSION CHARACTERISTICS AS TRANSACTION UNIQUE INTEGERITY IS DEFERRED */

    QLL_UNIQUE_MODE_MAX
} qllUniqueMode;
    
/**
 * @brief SET CONSTRAINT 유형
 */
typedef enum qllSetConstraintMode
{
    QLL_SET_CONSTRAINT_MODE_NA = 0,                 /**< N/A */
    
    QLL_SET_CONSTRAINT_MODE_ALL_IMMEDIATE,          /**< SET CONSTARINTS ALL IMMEDIATE */    
    QLL_SET_CONSTRAINT_MODE_ALL_DEFERRED,           /**< SET CONSTARINTS ALL DEFERRED */    
    QLL_SET_CONSTRAINT_MODE_CONSTRAINT_IMMEDIATE,   /**< SET CONSTARINTS ... IMMEDIATE */    
    QLL_SET_CONSTRAINT_MODE_CONSTRAINT_DEFERRED,    /**< SET CONSTARINTS ... DEFERRED */    

    QLL_SET_CONSTRAINT_MODE_SAVEPOINT,              /**< for savepoint management */    
    
    QLL_SET_CONSTRAINT_MODE_MAX
    
} qllSetConstraintMode;


/**
 * @brief SET CONSTRAINT item 정보 
 */
typedef struct qllSetConstraintModeItem qllSetConstraintModeItem;

struct qllSetConstraintModeItem
{
    qllSetConstraintMode mSetMode;       /**< SET CONSTRAINT mode */
    stlInt64             mConstID;       /**< Constraint ID:
                                          * SET CONSTRAINT 이후에 DROP CONSTRAINT 가능하므로 handle 을 사용하면 안됨
                                          */
    
    stlInt64             mSaveTimestamp; /**< only for SAVEPOINT */

    qllSetConstraintModeItem * mNext;
};


/**
 * @brief Collision Item Type
 */

typedef enum
{
    QLL_COLL_ITEM_NA = 0,

    QLL_COLL_ITEM_CONSTRAINT,     /**< Constraint Object */
    QLL_COLL_ITEM_UNIQUE_INDEX,   /**< Unique Index Object for CDC_DEFERRED_UNIQUE */
    
    QLL_COLL_ITEM_SAVEPOINT,      /**< for Savepoint Management */

    QLL_COLL_ITEM_MAX
    
} qllCollItemType;


/**
 * @brief Deferred Constarint 의 Collision Item
 */

typedef struct qllCollisionItem qllCollisionItem;

struct qllCollisionItem
{
    qllCollItemType        mItemType;              /**< collision item type */
    ellDictHandle          mObjectHandle;          /**< object handle */
    stlInt64               mCollisionCount;        /**< collision count */

    stlInt64               mSaveTimestamp;         /**< only for SAVEPOINT */

    qllCollisionItem     * mNext;
};


/**
 * @brief Deferrable Constraint Information
 */
typedef struct qllDeferrableConstraintInfo
{
    /**
     * SET CONSTRAINT
     */
    
    qllSetConstraintModeItem * mSetModeHistory;          /**< SET CONSTRAINT history */

    /**
     * Transaction Collision
     */
    
    qllCollisionItem         * mTransCollisionHistory;   /**< Constraint Violation history */
    qllCollisionItem         * mTransCollisionSummary;   /**< Constraint Violation summary */

    /**
     * Statement Collision
     */
    
    qllCollisionItem         * mStmtCollisionList;    /**< Statement Collision List */
    qllCollisionItem         * mNewCollisionSummary;  /**< Trans 에서 처음 발생한 Collision Summary */
    
} qllDeferrableConstraintInfo;

/** @} qllDeferrableConstraint */

/**
 * @defgroup qllCursor SQL Cursor
 * @ingroup qllAPI
 * @{
 */


/**
 * @brief Cursor Position 유형
 */

typedef enum qllCursorPositionType
{
    QLL_CURSOR_POSITION_NA = 0,                /**< N/A */

    QLL_CURSOR_POSITION_BEFORE_THE_FIRST_ROW,  /**< BEFORE THE FIRST ROW */
    QLL_CURSOR_POSITION_ON_A_CERTAIN_ROW,      /**< ON A CERTAIN ROW */
    QLL_CURSOR_POSITION_AFTER_THE_LAST_ROW,    /**< AFTER THE FIRST ROW */

    QLL_CURSOR_POSITION_MAX

} qllCursorPositionType;


/**
 * @brief Fetch Orientation 유형
 */
typedef enum qllFetchOrientation
{
    QLL_FETCH_NA = 0,                          /**< N/A */

    QLL_FETCH_NEXT,
    QLL_FETCH_PRIOR,
    QLL_FETCH_FIRST,
    QLL_FETCH_LAST,
    QLL_FETCH_CURRENT,
    QLL_FETCH_ABSOLUTE,
    QLL_FETCH_RELATIVE,
    
    QLL_FETCH_MAX
    
} qllFetchOrientation;


/**
 * Cursor Item 유형
 */
typedef enum qllCursorItemType
{
    QLL_CURSOR_ITEM_NA = 0,              /**< N/A */

    QLL_CURSOR_ITEM_SCAN,                /**< Scan Item */ 
    QLL_CURSOR_ITEM_LOCK,                /**< Lock Item */ 

    QLL_CURSOR_ITEM_MAX              
} qllCursorItemType;
    
/**
 * @brief Cursor 가 소유한 Scan 정보 List
 */

typedef struct qllCursorScanItem
{
    smlRowBlock              * mScanRowBlock;       /**< Scan Row Block */
    
    void                     * mInitBaseTable;      /**< Base Table Node */
    
    smlFetchRecordInfo         mRIDFetchInfo;       /**< RID Fetch Record Info */
    
    struct qllCursorScanItem * mNext;       
} qllCursorScanItem;


/**
 * @brief Cursor 가 소유한 Lock Wait Mode
 */
typedef enum qllCursorLockWait
{
    QLL_CURSOR_LOCK_WAIT_NA = 0,

    QLL_CURSOR_LOCK_WAIT_INFINITE,        /**< lock when execution */
    QLL_CURSOR_LOCK_WAIT_INTERVAL,        /**< lock when execution */
    QLL_CURSOR_LOCK_WAIT_NOWAIT,          /**< lock when execution */
    QLL_CURSOR_LOCK_WAIT_SKIP_LOCKED      /**< lock when fetch */
    
} qllCursorLockWait;

/**
 * @brief Cursor 가 소유한 Lock 정보 List
 */
typedef struct qllCursorLockItem
{
    void                     * mTablePhyHandle;     /**< Table Physical Handle */
    smlRowBlock              * mLockRowBlock;       /**< Lock Row Block */
    smlFetchRecordInfo       * mFetchRecordInfo;    /**< NO_RETRY 때만 필요한 정보이다. */
    struct qllCursorLockItem * mNext;
} qllCursorLockItem;


typedef struct qllStatement     qllStatement;

/**
 * @brief Cursor Result Set 정보
 */
typedef struct qllResultSetDesc
{
    /******************************************
     * Result Set 상태 정보 
     ******************************************/
    
    stlBool                mIsOpen;           /**< Result Set OPEN 여부 */
    stlInt64               mStmtTimestamp;    /**< Statement Timestamp */
    
    /*****************************************
     * Compile-Time Information
     *****************************************/
    
    /*
     * Standing Cursor Information
     */

    qllStatement          * mQueryStmt;       /**< Query Statement
                                               * if NULL: FETCH 대상 Query 가 아님 
                                               */
    
    ellCursorInstDesc     * mInstDesc;        /**< Cursor Instance Descriptor
                                               * if NULL: UnNamed Cursor
                                               */
    
    ellCursorProperty       mCursorProperty;   /**< Result Set Cursor Property */
    
    /*****************************************
     * Result Set Target Information
     *****************************************/

    void                  * mTargetExprList;    /**< Target 의 Expression List : qlvRefExprList*/
    knlExprStack          * mTargetCodeStack;   /**< Target 의 Code Stack */
    knlExprContextInfo    * mTargetDataContext; /**< Target 의 Data Context */
    knlValueBlockList     * mTargetBlock;       /**< Target 공간 */

    /*****************************************
     * instant table information
     *****************************************/

    stlBool                 mMaterialized;      /**< Materialized 여부 */
    knlValueBlockList     * mStoreBlockList;    /**< Store Block List */
    
    /*****************************************
     * for cursor SENSITIVITY
     *****************************************/
    
    /*
     * Scan Table Information
     */
    
    qllCursorScanItem      * mScanItemList;    /**< Base Table Item list */

    smlTransReadMode         mRIDFetchTRM;     /**< Fetch RID Transaction Read Mode */
    smlStmtReadMode          mRIDFetchSRM;     /**< Fetch RID Statement Read Mode */
    
    /*****************************************
     * for cursor UPDATABILITY
     *****************************************/

    qllCursorLockWait        mLockWaitMode;      /**< lock wait mode */
    stlInt64                 mLockTimeInterval;  /**< lock wait interval */
    
    qllCursorLockItem      * mLockItemList;      /**< Base Table Item list */
    
    /*****************************************
     * for cursor SCROLLABILITY
     *****************************************/
    
    /*****************************************
     * Execution-Time Information
     *****************************************/

    void                  * mInstantTable;      /**< qlnInstantTable */
    stlBool               * mRowLocked;         /**< Row Lock 성공 여부 */
    smlFetchInfo            mInstantFetchInfo;  /**< qlnInstantTable Iterator 에 대한 Fetch Info */
    
    /*****************************************
     * Fetch-Time Information
     *****************************************/

    /*
     * Total Position Information
     */

    stlBool                 mKnownLastPosition;
    stlInt64                mLastPositionIdx;
    
    /*
     * Row Position Information
     */

    qllCursorPositionType   mPositionType;     /**< Cursor Position Type */
    stlInt64                mPositionRowIdx;   /**< ON A CERTAIN ROW 위치, 1-base */
    stlInt32                mBlockRowIdx;      /**< Block 내에서의 위치 */

    /*
     * Block Position Information
     */

    stlInt32                mBlockStartIdx;    /**< Block 내에서의 시작 위치 */
    stlInt32                mBlockEOFIdx;      /**< Block 내에서의 종료 위치 */
    
    stlInt64                mBlockStartPositionIdx; /**< Block 시작 위치의 절대값 */
    stlInt64                mBlockEOFPositionIdx;   /**< Block 종료 위치의 절대값 */
    
} qllResultSetDesc;


/** @} qllCursor */


/**
 * @defgroup qllOptimization SQL Optimize
 * @ingroup qllAPI
 * @{
 */

#define QLL_INIT_GROUP_ENTRY_NO   ( 1 )
#define QLL_INVALID_NODE_IDX      ( -1 )

/**
 * @brief Code Optimization에서 평가한 Node별 Cost 정보
 */
typedef struct qllEstimateCost
{
    stlFloat64          mCost;
} qllEstimateCost;

/**
 * @brief Code Optimization에서 사용된 Bind Parameter 정보
 */
typedef struct qllBindParamInfo
{
    dtlDataTypeInfo  * mBindDataTypeInfo;  /**< SQL 문에 포함된 Bind 의 Data Type Info
                                            * mDataType : Data Type
                                            * mArgNum1  : Precision 또는 Leading Precision 
                                            * mArgNum2  : dtlStringLengthUnit 또는 dtlIntervalIndicator */
} qllBindParamInfo;

typedef struct qllOptimizationNode   qllOptimizationNode;
typedef struct qllOptNodeItem        qllOptNodeItem;
typedef struct qllOptNodeList        qllOptNodeList;
typedef struct qllExecutionNode      qllExecutionNode;

/**
 * @brief Common Optimization Node
 */
struct qllOptimizationNode
{
    qllPlanNodeType         mType;               /**< Node Type */
    stlInt16                mIdx;                /**< Node Index : Node List 구성시 자동으로 설정 */
    stlBool                 mIsAtomic;           /**< Atomic 여부 */

    void                  * mOptNode;            /**< 각 기능별 optimization node */
    // qllOptimizationNode   * mOptCurQueryNode;    /**< 해당 node가 속한 Query node */

    // qllEstimateCost       * mEstimateCost;       /**< Code Optimization 시 평가한 수행 Cost */
    qllBindParamInfo      * mBindParamInfo;   // ???
};

/**
 * @brief Optimization Node List를 구성 단위
 */
struct qllOptNodeItem
{
    qllOptimizationNode  * mNode;
    qllOptNodeItem       * mPrev;
    qllOptNodeItem       * mNext;
};

/**
 * @brief Optimization Node List
 */
struct qllOptNodeList
{
    stlInt16          mCount;
    qllOptNodeItem  * mHead;
    qllOptNodeItem  * mTail;
};


#define QLL_GET_OPT_NODE_IDX( aOptNode )  ( (aOptNode)->mIdx )

#define QLL_GET_CACHED_INIT_PLAN( aOptNode )                                            \
    ( ((qlnoDmlStmt *)((qllOptimizationNode*)(aOptNode))->mOptNode)->mCachedInitNode)

typedef struct qloColumnBlockItem         qloColumnBlockItem;
typedef struct qloColumnBlockList         qloColumnBlockList;


/**
 * @brief Data Optimization 수행 결과로 생성되는 Data Area
 * @remark Execution시 필요한 Statement 단위의 정보를 관리하고 있다.
 */
typedef struct qllDataArea
{
    stlInt32              mBlockAllocCnt;        /**< Block Allocation Count */
    stlBool               mIsFirstExecution;     /**< 첫번째 Exeuction 여부 */
    stlBool               mIsEvaluateCost;       /**< Exeuction시에 Cost 평가 여부 */

    smlTransId            mTransID;              /**< transaction ID */    

    stlInt16              mExecNodeCnt;          /**< Execution Node Count */
    qllExecutionNode    * mExecNodeList;         /**< Execution Node List : array */

    qloColumnBlockList  * mReadColumnBlock;      /**< 읽을 Column 공간 */
    qloColumnBlockList  * mWriteColumnBlock;     /**< 쓸 Column 공간 */
    qloColumnBlockList  * mRowIdColumnBlock;     /**< RowId Pseudo Column 공간 */    
    knlValueBlockList   * mPredResultBlock;      /**< Predicate 평가 결과 저장을 위한 공간 */
    knlValueBlockList   * mConstResultBlock;     /**< Constant Expression 평가 결과 저장을 위한 공간 */
    knlValueBlockList   * mPseudoColBlock;       /**< Pseudo Column 공간 */
    knlValueBlockList   * mStmtFuncBlock;        /**< Statement 단위 Function Block 공간 */
    knlValueBlockList   * mTargetBlock;          /**< Fetch할 Target Block 공간 */
    knlValueBlockList   * mIntoBlock;            /**< INTO clause를 위한 Block 공간 */
    knlValueBlockList   * mSysTimestampTzBlock;  /**< SYS_DATETIME을 위한 Timestamp with time zone Block 공간 */
    knlValueBlockList   * mStmtTimestampTzBlock; /**< STATEMENT_DATETIME을 위한 Timestamp with time zone Block 공간 */
    knlValueBlockList   * mTransactionTimestampTzBlock; /**< TRANSACTION_DATETIME을 위한 Timestamp with time zone Block 공간 */    
    knlCastFuncInfo     * mIntoAssignInfo;       /**< INTO Assign Information */

    knlExprContextInfo  * mExprDataContext;      /**< expression data context */

    stlInt64              mWriteRowCnt;          /**< Write Row Count */
    stlInt64              mFetchRowCnt;          /**< Fetch Row Count */
} qllDataArea;

#define QLL_GET_EXECUTION_NODE( aDataArea, aNodeIdx )  (  &(aDataArea)->mExecNodeList[ (aNodeIdx) ] )

/** @} qllOptimization */


/**
 * @defgroup qllExecution SQL Execution
 * @ingroup qllAPI
 * @{
 */


/**
 * @brief Execution Cost 정보
 */
typedef struct qllExecutionCost
{
    stlFloat32        mCpuCost;       /**< CPU Cost */
    stlFloat32        mIoCost;        /**< IO Cost */
    stlInt64          mFetchRowCnt;   /**< Fetch Row Count */
    stlInt64          mCopyValueCnt;  /**< Copy Value Count */
    stlInt64          mRecursiveCnt;  /**< Recursize Call Node Count */
} qllExecutionCost;


/**
 * @brief Common Execution Node 
 */
struct qllExecutionNode
{
    qllPlanNodeType         mNodeType;         /**< Node Type */
    qllOptimizationNode   * mOptNode;          /**< 관련 Optimization Node */
    void                  * mExecNode;         /**< Execution Node : qloxXXXXNode */

    qllExecutionCost      * mExecutionCost;    /**< Execution Cost : 
                                                * qllDataArea의 mIsEvalateCost가
                                                * STL_TRUE인 경우만 공간 할당 받음
                                                */
};

#define QLL_GET_EXEC_NODE_IDX( aNode )  ( (aNode)->mNodeIdx )

#define QLL_ROWID_COLUMN_IDX    (-1)       /**< RowId Column에 대한 Column Index */


/** @} qllExecution */


/**
 * @defgroup qllExplainPlan SQL Explain Plan
 * @ingroup qllAPI
 * @{
 */

#define QLL_EXPLAIN_PLAN_DEFAULT_LINE_SIZE  512 /**< Explain Plan에 대한 Default Line Size */

typedef struct qllExplainText
{
    stlInt32                      mMaxSize;         /**< Max Size */
    stlInt32                      mCurSize;         /**< Current Size */
    stlChar                     * mText;            /**< Text Buffer */
    stlChar                    ** mLineText;        /**< Line Text Pointer */
} qllExplainText;

typedef struct qllExplainPredText
{
    qllExplainText                mLineBuffer;      /**< plan 정보 한 줄을 위한 Buffer */
    stlChar                     * mLineText;        /**< plan 정보 한 줄 */
    struct qllExplainPredText   * mNext;            /**< 다음 line text */ 
} qllExplainPredText;

typedef struct qllExplainNodeText
{
    stlUInt32                     mIdx;             /**< Node의 Index Number */
    stlUInt32                     mDepth;           /**< Node의 Depth */
    stlInt64                      mFetchRowCnt;     /**< Fetch Row Count */
    stlTime                       mBuildTime;
    stlTime                       mFetchTime;
    stlFloat64                    mFetchRecordAvgSize;
    stlInt32                      mFetchCallCount;
    qllExplainText                mDescBuffer;      /**< Node Description 정보의 Buffer */
    qllExplainText                mLineBuffer;      /**< plan 정보 한 줄을 위한 Buffer */
    stlChar                     * mLineText;        /**< plan 정보 한 줄 */
    qllExplainPredText          * mPredText;        /**< predicate 정보 리스트 */
    qllExplainPredText          * mLastPredText;    /**< 마지막 predicate 정보 리스트 */
    struct qllExplainNodeText   * mNext;            /**< 다음 line text */ 
} qllExplainNodeText;

/** @} qllExplainPlan */


/**
 * @defgroup qllStatement SQL Statement
 * @ingroup qllAPI
 * @{
 */

typedef struct qllDCLContext    qllDCLContext;


/**
 * @brief SQL 구문 유형
 */

/**
 * @defgroup qllStatementAttr SQL Statement Attribute
 * @ingroup qllStatement
 * @{
 */

#define QLL_STMT_ATTR_DB_ACCESS_NONE         (0x00000000)    /**< Database에 접근하지 않음 */
#define QLL_STMT_ATTR_DB_ACCESS_WRITE        (0x00000001)    /**< Database를 갱신/검색함 */
#define QLL_STMT_ATTR_DB_ACCESS_READ         (0x00000002)    /**< Database를 검색함 */
#define QLL_STMT_ATTR_DB_ACCESS_UNKNOWN      (0x00000004)    /**< Database를 접근 여부를 알 수 없는 경우, Ex) OPEN cursor, EXEC proc  */
#define QLL_STMT_ATTR_DB_ACCESS_MASK         (0x0000000F)    /**< Database Access Mask */

#define QLL_STMT_ATTR_DDL_NO                 (0x00000000)    /**< DDL 이 아님  */
#define QLL_STMT_ATTR_DDL_YES                (0x00000010)    /**< DDL 임  */
#define QLL_STMT_ATTR_DDL_MASK               (0x00000010)    /**< DDL 여부  */

#define QLL_STMT_ATTR_LOCKABLE_NO            (0x00000000)    /**< Lockless statement */
#define QLL_STMT_ATTR_LOCKABLE_YES           (0x00000020)    /**< Lockable statement */
#define QLL_STMT_ATTR_LOCKABLE_MASK          (0x00000020)    /**< Lockable Mask */

#define QLL_STMT_ATTR_AUTOCOMMIT_NO          (0x00000000)    /**< Statement 종료후 Commit 필요함 */
#define QLL_STMT_ATTR_AUTOCOMMIT_YES         (0x00000040)    /**< Statement 종료후 Commit 필요없음 */
#define QLL_STMT_ATTR_AUTOCOMMIT_MASK        (0x00000040)    /**< Retry 재컴파일 Mask */

#define QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO   (0x00000000)    /**< Snapshot Iterator가 필요없음 */
#define QLL_STMT_ATTR_SNAPSHOT_ITERATOR_YES  (0x00000080)    /**< Snapshot Iterator가 필요함 */
#define QLL_STMT_ATTR_SNAPSHOT_ITERATOR_MASK (0x00000080)    /**< Snapshot Iterator Mask */

#define QLL_STMT_ATTR_FETCH_NO               (0x00000000)    /**< Statement 수행후 레코드 Fetch가 필요없음 */
#define QLL_STMT_ATTR_FETCH_YES              (0x00000100)    /**< Statement 수행후 레코드 Fetch가 필요함 */
#define QLL_STMT_ATTR_FETCH_MASK             (0x00000100)    /**< Fetch Mask */

#define QLL_STMT_ATTR_STATE_TRANS_NO         (0x00000000)    /**< Statement 수행후 Session의 상태전이가 필요없음 */
#define QLL_STMT_ATTR_STATE_TRANS_YES        (0x00000200)    /**< Statement 수행후 Session의 상태전이가 필요함 */
#define QLL_STMT_ATTR_STATE_TRANS_MASK       (0x00000200)    /**< Session의 상태전이 Mask */

#define QLL_STMT_ATTR_SUPPORT_GLOBAL_NO      (0x00000000)    /**< Global Transaction 지원 안함 */
#define QLL_STMT_ATTR_SUPPORT_GLOBAL_YES     (0x00000800)    /**< Global Transaction 지원 */
#define QLL_STMT_ATTR_SUPPORT_GLOBAL_MASK    (0x00000800)    /**< Global Transaction Mask */

#define QLL_STMT_ATTR_PLAN_CACHE_NO          (0x00000000)    /**< Plan Cache 사용 안함 */
#define QLL_STMT_ATTR_PLAN_CACHE_YES         (0x00001000)    /**< Plan Cache 사용함 */
#define QLL_STMT_ATTR_PLAN_CACHE_MASK        (0x00001000)    /**< Plan Cache Mask */

#define QLL_STMT_ATTR_SUPPORT_CDS_NO         (0x00000000)    /**< CDS Database에서 수행 안됨 */
#define QLL_STMT_ATTR_SUPPORT_CDS_YES        (0x00002000)    /**< CDS Database에서 수행될수 있음 */
#define QLL_STMT_ATTR_SUPPORT_CDS_MASK       (0x00002000)    /**< CDS Database Mask */

#define QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO   (0x00000000)    /**< SYNONYM 사용할 수 없는 구문임 */
#define QLL_STMT_ATTR_CAN_REFER_SYNONYM_YES  (0x00004000)    /**< SYNONYM 사용할 수 있는 구문임 */
#define QLL_STMT_ATTR_CAN_REFER_SYNONYM_MASK (0x00004000)    /**< SYNONYM Mask */

/**
 * @todo 제거되어야 합니다.
 */
#define QLL_STMT_ATTR_ATOMIC_EXECUTION_NO   (0x00000000)    /**< Atomic Execution이 가능하지 않음. */
#define QLL_STMT_ATTR_ATOMIC_EXECUTION_YES  (0x01000000)    /**< Atomic Execution이 불가능함. */
#define QLL_STMT_ATTR_ATOMIC_EXECUTION_MASK (0x0F000000)    /**< Atomic Execution Mask */

/**
 * @brief statement의 access 속성
 */
typedef enum
{
    QLL_ACCESS_MODE_NONE = 0,      /**< statement가 필요 없음 */
    QLL_ACCESS_MODE_READ_WRITE,    /**< writeable statement 사용 */
    QLL_ACCESS_MODE_READ_ONLY,     /**< read only statement 사용 */
    QLL_ACCESS_MODE_UNKNOWN,       /**< OPEN cursor, EXEC proc 과 같이
                                    * read/write 여부를 알 수 없으나, transaction begin 은 필요함
                                    */
} qllAccessMode;

/** @} qllStatementAttr */

/**
 * @brief 사용 불가능한 Statement 속성
 */
#define QLL_STMT_ATTR_NA       ( 0xFFFFFFFF )
/**
 * @brief Statement Nothing 속성
 * <BR> (1) 데이터베이스에 접근하지 않는다.
 * <BR> (2) Lock Timeout 설정이 필요하지 않다.
 * <BR> (3) 수행후 레코드 결과가 존재하지 않는다.
 * <BR> (4) 수행후 Session의 상태가 전이될 필요없다.
 */
#define QLL_STMT_ATTR_NOTHING  ( QLL_STMT_ATTR_DB_ACCESS_NONE       |   \
                                 QLL_STMT_ATTR_DDL_NO               |   \
                                 QLL_STMT_ATTR_AUTOCOMMIT_NO        |   \
                                 QLL_STMT_ATTR_SNAPSHOT_ITERATOR_NO |   \
                                 QLL_STMT_ATTR_LOCKABLE_NO          |   \
                                 QLL_STMT_ATTR_FETCH_NO             |   \
                                 QLL_STMT_ATTR_ATOMIC_EXECUTION_NO  |   \
                                 QLL_STMT_ATTR_SUPPORT_GLOBAL_NO    |   \
                                 QLL_STMT_ATTR_STATE_TRANS_NO )


#define QLL_SELECT_BLOCK_COUNT   ( 1 )

/**
 * @brief Query Processing Phase
 */
typedef enum
{
    QLL_PHASE_NA = 0,       /**< N/A */

    QLL_PHASE_PARSING,
    QLL_PHASE_VALIDATION,
    QLL_PHASE_CODE_OPTIMIZE,
    QLL_PHASE_DATA_OPTIMIZE,
    QLL_PHASE_RECHECK_PRIVILEGE,
    QLL_PHASE_EXECUTE,

    QLL_PHASE_MAX
} qllQueryPhase;

    
/**
 * @brief Validation Function Pointer
 * @param[in] aTransID    Transaction ID
 * @param[in] aSQLStmt    SQL Statement
 * @param[in] aSQLString  SQL String
 * @param[in] aParseTree  Parse Tree
 * @param[in] aEnv        Environment
 * @remarks
 */
typedef stlStatus (* qllValidateFunc) ( smlTransId     aTransID,
                                        qllDBCStmt   * aDBCStmt,
                                        qllStatement * aSQLStmt,
                                        stlChar      * aSQLString,
                                        qllNode      * aParseTree,
                                        qllEnv       * aEnv );

/**
 * @brief Code Area Optimize Function Pointer
 * @param[in] aTransID    Transaction ID
 * @param[in] aSQLStmt    SQL Statement
 * @param[in] aEnv        Environment
 * @remarks
 */
typedef stlStatus (* qllCodeOptFunc) ( smlTransId     aTransID,
                                       qllDBCStmt   * aDBCStmt,
                                       qllStatement * aSQLStmt,
                                       qllEnv       * aEnv );

/**
 * @brief Data Area Optimize Function Pointer
 * @param[in] aTransID    Transaction ID
 * @param[in] aSQLStmt    SQL Statement
 * @param[in] aEnv        Environment
 * @remarks
 */
typedef stlStatus (* qllDataOptFunc) ( smlTransId     aTransID,
                                       qllDBCStmt   * aDBCStmt,
                                       qllStatement * aSQLStmt,
                                       qllEnv       * aEnv );

/**
 * @brief Execution Function Pointer
 * @param[in] aTransID    Transaction ID
 * @param[in] aStmt       Statement
 * @param[in] aSQLStmt    SQL Statement
 * @param[in] aEnv        Environment
 * @remarks
 */
typedef stlStatus (* qllExecuteFunc) ( smlTransId     aTransID,
                                       smlStatement * aStmt,
                                       qllDBCStmt   * aDBCStmt,
                                       qllStatement * aSQLStmt,
                                       qllEnv       * aEnv );



/**
 * @brief Data Control Language Type
 */
typedef enum
{
    QLL_DCL_TYPE_NONE = 0,
    
    /*
     * COMMIT / ROLLBACK
     */
    QLL_DCL_TYPE_END_TRANSACTION,

    /*
     * SET TRANSACTION
     */
    QLL_DCL_TYPE_SET_TRANSACTION_ACCESS_MODE,
    QLL_DCL_TYPE_SET_TRANSACTION_ISOLATION_LEVEL,
    QLL_DCL_TYPE_SET_TRANSACTION_UNIQUE_INTEGRITY,

    /*
     * SET SESSION CHARACTERISTICS
     */
    QLL_DCL_TYPE_SET_SESSION_CHARACTERISTICS_ACCESS_MODE,
    QLL_DCL_TYPE_SET_SESSION_CHARACTERISTICS_ISOLATION_LEVEL,
    QLL_DCL_TYPE_SET_SESSION_CHARACTERISTICS_UNIQUE_INTEGRITY,

    /*
     * SET TIME ZONE
     */
    QLL_DCL_TYPE_SET_TIME_ZONE,

    /*
     * START TRANSACTION
     */
    QLL_DCL_TYPE_START_TRANSACTION,
    
    /*
     * OPEN DATABASE
     */
    QLL_DCL_TYPE_OPEN_DATABASE,
    
    /*
     * CLOSE DATABASE
     */
    QLL_DCL_TYPE_CLOSE_DATABASE
} qllDCLType;


/**
 * @brief Data Control Language Context
 */
struct qllDCLContext
{
    qllDCLType        mType;
    stlInt16          mAccessMode;
    stlInt16          mGMTOffset;
    smlIsolationLevel mIsolationLevel;
    qllUniqueMode     mUniqueMode;
};

typedef struct qllStmtCursorProperty
{
    stlBool                mIsDbcCursor;    /**< ODBC/JDBC Cursor 인지  여부 */
    ellCursorProperty      mProperty;       /**< Statement Cursor Property */
} qllStmtCursorProperty;

#define QLL_OPT_SQL_LAYER_ID_FOR_TRACE  ( STL_ERROR_MODULE_SQL_PROCESSOR + 1 )

#define QLL_OPT_ADMIN_LAYER_ID  ( 1400 + QLL_OPT_SQL_LAYER_ID_FOR_TRACE )

#define QLL_OPT_GET_TRACE_SQL_PLAN_LEVEL( _aEnv )                               \
    ( ( ( ( knlGetPropertyBigIntValueByID( KNL_PROPERTY_TRACE_LOG_ID,           \
                                           KNL_ENV(_aEnv) ) / 10000 ) % 100 )   \
        == QLL_OPT_SQL_LAYER_ID_FOR_TRACE )                                     \
      ? ( ( knlGetPropertyBigIntValueByID( KNL_PROPERTY_TRACE_LOG_ID,           \
                                           KNL_ENV(_aEnv) ) / 100 ) % 100 )     \
      : 0 )

#define QLL_OPT_GET_TRACE_CODE_PLAN_LEVEL( _aEnv )                  \
    ( ( ( knlGetPropertyBigIntValueByID( KNL_PROPERTY_TRACE_LOG_ID, \
                                         KNL_ENV(_aEnv) ) / 10000 ) \
        == QLL_OPT_ADMIN_LAYER_ID )                                 \
      ? ( knlGetPropertyBigIntValueByID( KNL_PROPERTY_TRACE_LOG_ID, \
                                         KNL_ENV(_aEnv) ) % 100 )   \
      : 0 )


#define QLL_OPT_TRACE_SUCCESS_SQL_PLAN_ON( _aEnv )                  \
    ( ( ( (QLL_OPT_GET_TRACE_SQL_PLAN_LEVEL(_aEnv) / 10) == 0 ) &&  \
        ( (QLL_OPT_GET_TRACE_SQL_PLAN_LEVEL(_aEnv) % 10) > 0 ) &&   \
        ( (QLL_OPT_GET_TRACE_SQL_PLAN_LEVEL(_aEnv) % 10) <= 4 ) )   \
      ? STL_TRUE : STL_FALSE )

#define QLL_OPT_TRACE_FAILURE_SQL_PLAN_ON( _aEnv )                      \
    ( ( ( ( (QLL_OPT_GET_TRACE_SQL_PLAN_LEVEL(_aEnv) / 10) == 0 ) ||    \
          ( (QLL_OPT_GET_TRACE_SQL_PLAN_LEVEL(_aEnv) / 10) == 9 ) ) &&  \
        ( (QLL_OPT_GET_TRACE_SQL_PLAN_LEVEL(_aEnv) % 10) > 0 ) &&       \
        ( (QLL_OPT_GET_TRACE_SQL_PLAN_LEVEL(_aEnv) % 10) <= 4 ) )       \
      ? STL_TRUE : STL_FALSE )

#define QLL_OPT_TRACE_COST_PLAN_ON( _aEnv )                 \
    ( ( ( QLL_OPT_GET_TRACE_CODE_PLAN_LEVEL(_aEnv) > 0 ) && \
        ( QLL_OPT_GET_TRACE_CODE_PLAN_LEVEL(_aEnv) <= 2 ) ) \
      ? STL_TRUE : STL_FALSE )

#define QLL_OPT_TRACE_CODE_PLAN_ON( _aEnv )             \
    ( ( QLL_OPT_GET_TRACE_CODE_PLAN_LEVEL(_aEnv) == 2 ) \
      ? STL_TRUE : STL_FALSE )


#define QLL_OPT_DECLARE( _aDecl ) \
    _aDecl

#define QLL_OPT_CHECK_TIME( _aTimeValue )                               \
{                                                                       \
    if( knlGetPropertyBoolValueByID( KNL_PROPERTY_TRACE_LOG_TIME_DETAIL,\
                                     KNL_ENV(aEnv) ) == STL_FALSE )     \
    {                                                                   \
        (_aTimeValue) = knlGetSystemTime();                             \
    }                                                                   \
    else                                                                \
    {                                                                   \
        (_aTimeValue) = stlNow();                                       \
    }                                                                   \
}

#define QLL_OPT_ADD_ELAPSED_TIME( _aDestTimeValue, _aBeginTime, _aEndTime ) \
{                                                                           \
    (_aDestTimeValue) += ((_aEndTime) - (_aBeginTime));                     \
}

#define QLL_OPT_ADD_TIME( _aDestTimeValue, _aAddTime )  \
{                                                       \
    (_aDestTimeValue) += (_aAddTime);                   \
}

#define QLL_OPT_ADD_COUNT( _aDestValue, _aCount )   \
    (_aDestValue) += (_aCount);

#define QLL_OPT_SET_VALUE( _aDestValue, _aValue )   \
    (_aDestValue) = (_aValue);

#define QLL_OPT_ADD_RECORD_STAT_INFO( _aRecordStat, _aValueBlockList, _aFetchRecordCount, _aSqlEnv )\
{                                                                                                   \
    stlInt32              _sIdx;                                                                    \
    stlInt64              _sRecordSize;                                                             \
    knlValueBlockList   * _sValueBlockList  = NULL;                                                 \
                                                                                                    \
    if( QLL_OPT_GET_TRACE_SQL_PLAN_LEVEL( _aSqlEnv ) % 10 >= 4 )                                    \
    {                                                                                               \
        for( _sIdx = 0; _sIdx < (_aFetchRecordCount); _sIdx++ )                                     \
        {                                                                                           \
            _sRecordSize = 0;                                                                       \
            _sValueBlockList  = (_aValueBlockList);                                                 \
            while( _sValueBlockList != NULL )                                                       \
            {                                                                                       \
                if( _sValueBlockList->mValueBlock->mIsSepBuff == STL_TRUE )                         \
                {                                                                                   \
                    _sRecordSize += _sValueBlockList->mValueBlock->mDataValue[_sIdx].mLength;       \
                }                                                                                   \
                else                                                                                \
                {                                                                                   \
                    _sRecordSize += _sValueBlockList->mValueBlock->mDataValue[0].mLength;           \
                }                                                                                   \
                _sValueBlockList = _sValueBlockList->mNext;                                         \
            }                                                                                       \
                                                                                                    \
            if( _sRecordSize > (_aRecordStat).mMaxSize )                                            \
            {                                                                                       \
                (_aRecordStat).mMaxSize = _sRecordSize;                                             \
            }                                                                                       \
                                                                                                    \
            if( _sRecordSize < (_aRecordStat).mMinSize )                                            \
            {                                                                                       \
                (_aRecordStat).mMinSize = _sRecordSize;                                             \
            }                                                                                       \
                                                                                                    \
            (_aRecordStat).mTotalSize += _sRecordSize;                                              \
            (_aRecordStat).mTotalCount++;                                                           \
        }                                                                                           \
    }                                                                                               \
}

/**
 * @brief qllOptInfo
 */
typedef struct qllOptInfo
{
    /* Basic Info */
    stlBool               mIsExecDirect;
    stlBool               mIsExplainPlanOnly;

    /* Time Info */
    stlTime               mParseTime;
    stlTime               mValidateTime;
    stlTime               mCodeOptTime;
    stlTime               mOptimizerTime;
    stlTime               mDataOptTime;
    stlTime               mExecuteTime;
    stlTime               mFetchTime;

    /* Call Count Info */
    stlInt32              mParseCallCount;
    stlInt32              mValidateCallCount;
    stlInt32              mCodeOptCallCount;
    stlInt32              mOptimizerCallCount;
    stlInt32              mDataOptCallCount;
    stlInt32              mExecuteCallCount;
    stlInt32              mFetchCallCount;
} qllOptInfo;


/**
 * @brief qllLongTypeValueItem
 */
typedef struct qllLongTypeValueItem
{
    stlInt32                        mArrSize;
    dtlDataValue                  * mDataValues;
    struct qllLongTypeValueItem   * mNext;
} qllLongTypeValueItem;


/**
 * @brief qllLongTypeValueList
 */
typedef struct qllLongTypeValueList
{
    stlInt32                 mCount;
    qllLongTypeValueItem   * mHead;
    qllLongTypeValueItem   * mTail;
} qllLongTypeValueList;


/**
 * @brief Maximum Statement SQL size
 */
#define QLL_MAX_STMT_SQL_SIZE  ( 1024 )

/**
 * @brief QP Statement
 */
struct qllStatement
{
    /********************************
     * SQL 구문 정보 
     ********************************/
      
    stlChar   * mRetrySQL;      /**< Retry 가 필요한 SQL 구문은 저장 */
    stlChar     mStmtSQL[QLL_MAX_STMT_SQL_SIZE]; /**< X$STATEMENT를 위한 SQL 문장 */
    stlTime     mStartTime;     /**< qllStatement가 시작한 시간 */
    stlBool     mIsRecompile;   /**< Recompile 발생 여부 */
    stlBool     mKeepParseTree; /**< Parse Tree 유지 ex) CREATE SCHEMA */

    stlInt32    mStmtAttr;      /**< SQL Statement 속성 */
    qllNodeType mStmtType;      /**< SQL Statement Type */

    qllQueryPhase  mLastSuccessPhase;    /**< 완료된 Query 수행 단계 */

    smlScn         mViewSCN;
    smlStatement * mStmt;
    
    /********************************
     * Layer 간 Context
     ********************************/

    stlBool          mIsAtomic;      /**< Atomic Statement 인지 여부 */
    
    stlInt32         mBindCount;     /**< SQL 문에 포함된 Bind 개수 */
    
    knlBindContext * mBindContext;   /**< Client 에서 설정한 Bind Context */
    
    qllDCLContext    mDCLContext;    /**< DCL 정보 */

    /********************************
     * Plan
     ********************************/

    ellPrivList     * mGrantedPrivList;      /**< Granted Privilege List, Validation 과정에서 목록을 구축하기 위해 사용 */

    /* granted privilege list 를 linear 형태로 재구성하여 관리 */
    stlInt32      mGrantedPrivCount;         /**< Granted Privilege Count */
    ellPrivItem * mGrantedPrivArray;         /**< Granted Privilege Array */
    
    void      * mInitPlan;          /**< Init Plan, qlvInitXXXX */
    void      * mCodePlan;          /**< Code Plan, qloCodeXXXX */
    void      * mDataPlan;          /**< Data Plan, qloDataXXXX */
    void      * mSqlHandle;         /**< plan cache handle */
    
    qllExplainNodeText  * mExplainPlanText;  /**< Explain Plan 결과 */

    /********************************
     * Cursor Result Set Descriptor 
     ********************************/

    qllStmtCursorProperty   mStmtCursorProperty;     /**< Statement Cursor Property */

    qllResultSetDesc      * mResultSetDesc;        /**< Statement Result Set, Data Optimization 이후에 생성 */

    /********************************
     * Cycle Detector
     ********************************/

    ellObjectList        * mViewCycleList;       /**< View Cycle Detector */
    
    /********************************
     * Long Type Data Value List
     ********************************/
    
    qllLongTypeValueList  mLongTypeValueList;    /**< Long Varchar/Varbinary Data Value List */

    /********************************
     * Trace Log를 위한 정보
     ********************************/

    qllOptInfo            mOptInfo;
};


/** @} qllStatement */



/**
 * @defgroup qllParameter SQL BindParameter
 * @ingroup qllAPI
 * @{
 */

typedef struct qllParameter
{
    /*
     * Parameter Name 관련 정보
     */
    
    stlChar  * mTableName;         /**< SQL_DESC_TABLE_NAME */

    stlBool    mIsAliasUnnamed;    /**< SQL_DESC_UNNAMED */
    stlChar  * mColumnAlias;       /**< SQL_DESC_NAME */

    /*
     * Parameter 의 관계모델 속성
     */
    
    stlBool    mIsNullable;        /**< SQL_DESC_NULLABLE */
    stlBool    mIsRowVersion;      /**< SQL_DESC_ROWVER */

    /*
     * Parameter 의 Type 정보
     */

    dtlDataType   mDBType;               /**< SQL_DESC_TYPE */
    dtlDataType   mConciseType;          /**< SQL_DESC_CONCISE_TYPE */
    dtlIntervalIndicator mIntervalCode;  /**< SQL_DESC_DATETIME_INTERVAL_CODE */
    
    stlChar     * mTypeName;             /**< SQL_DESC_TYPE_NAME */
    stlChar     * mLocalTypeName;        /**< SQL_DESC_LOCAL_TYPE_NAME */
    
    /*
     * Parameter Type 의 Date/Time/Interval Precision 관련 정보
     * - Time/Timestamp : fractional second precision
     * - Interval : leading precision (second precision : mPrecision 값)
     */

    stlInt64      mDateTimeIntervalPrec;   /**< SQL_DESC_DATETIME_INTERVAL_PRECISION */
    
    /*
     * Parameter Type 의 숫자 관련 정보
     */
    
    dtlNumericPrecRadix  mNumPrecRadix;   /**< SQL_DESC_NUM_PREC_RADIX */
    stlBool    mUnsigned;          /**< SQL_DESC_UNSIGNED */
    stlBool    mIsExactNumeric;    /**< SQL_DESC_FIXED_PREC_SCALE */
    stlInt64   mPrecision;         /**< SQL_DESC_PRECISION */
    stlInt64   mScale;             /**< SQL_DESC_SCALE */

    /*
     * Parameter Type 의 문자 관련 Type 정보
     */
    
    stlBool    mIsCaseSensitive;   /**< SQL_DESC_CASE_SENSITIVE */
    
    stlInt64   mCharacterLength;   /**< SQL_DESC_LENGTH */
    stlInt64   mOctetLength;       /**< SQL_DESC_OCTET_LENGTH */
    
    struct qllParameter * mNext;
} qllParameter;
    
/** @} qllParameter */


/**
 * @defgroup qllTargetInfo Query Target Information
 * @ingroup qllAPI
 * @{
 */

typedef struct qllTarget
{
    /*
     * Target Name 관련 정보
     */
    
    stlChar  * mCatalogName;       /**< SQL_DESC_CATALOG_NAME */
    stlChar  * mSchemaName;        /**< SQL_DESC_SCHEMA_NAME */
    
    stlChar  * mTableName;         /**< SQL_DESC_TABLE_NAME */
    stlChar  * mBaseTableName;     /**< SQL_DESC_BASE_TABLE_NAME */

    /*
     * ---------------------------------------------
     * SELECT    C1,     C2 + 1,    C3 AS NEW_COLUMN
     * ---------------------------------------------
     * UNNAMED  FALSE     TRUE         FALSE
     * Alias     C1       (null)       NEW_COLUMN
     * Label     C1       C2 + 1       NEW_COLUMN
     * Base      C1       (null)       C3
     * ---------------------------------------------
     */
    
    stlBool    mIsAliasUnnamed;    /**< SQL_DESC_UNNAMED */
    stlChar  * mColumnAlias;       /**< SQL_DESC_NAME */
    stlChar  * mColumnLabel;       /**< SQL_DESC_LABEL */
    stlChar  * mBaseColumnName;    /**< SQL_DESC_BASE_COLUMN_NAME */

    /*
     * Target 의 관계모델 속성
     */
    
    stlBool    mIsNullable;        /**< SQL_DESC_NULLABLE */
    stlBool    mIsUpdatable;       /**< SQL_DESC_UPDATABLE */
    stlBool    mIsAutoUnique;      /**< SQL_DESC_AUTO_UNIQUE_VALUE */
    stlBool    mIsRowVersion;      /**< SQL_DESC_ROWVER */
    stlBool    mIsAbleLike;        /**< SQL_DESC_SEARCHABLE */

    /*
     * Target 의 출력 공간
     */
    
    stlInt64   mDisplaySize;       /**< SQL_DESC_DISPLAY_SIZE */
    
    /*
     * Target 의 Type 정보
     */

    dtlDataType   mDBType;               /**< SQL_DESC_TYPE */
    dtlDataType   mConciseType;          /**< SQL_DESC_CONCISE_TYPE */
    dtlIntervalIndicator mIntervalCode;  /**< SQL_DESC_DATETIME_INTERVAL_CODE */
    
    stlChar     * mTypeName;             /**< SQL_DESC_TYPE_NAME */
    stlChar     * mLocalTypeName;        /**< SQL_DESC_LOCAL_TYPE_NAME */
    
    /*
     * Target Type 의 Date/Time/Interval Precision 관련 정보
     * - Time/Timestamp : fractional second precision
     * - Interval : leading precision (second precision : mPrecision 값)
     */

    stlInt64      mDateTimeIntervalPrec;   /**< SQL_DESC_DATETIME_INTERVAL_PRECISION */
    
    /*
     * Target Type 의 숫자 관련 정보
     */
    
    dtlNumericPrecRadix  mNumPrecRadix;   /**< SQL_DESC_NUM_PREC_RADIX */
    stlBool    mUnsigned;          /**< SQL_DESC_UNSIGNED */
    stlBool    mIsExactNumeric;    /**< SQL_DESC_FIXED_PREC_SCALE */
    stlInt64   mPrecision;         /**< SQL_DESC_PRECISION */
    stlInt64   mScale;             /**< SQL_DESC_SCALE */

    /*
     * Target Type 의 문자 관련 Type 정보
     */
    
    stlBool    mIsCaseSensitive;   /**< SQL_DESC_CASE_SENSITIVE */

    dtlStringLengthUnit   mStringLengthUnit;  /**< SQL_DESC_CHAR_LENGTH_UNITS */
    stlInt64              mCharacterLength;   /**< SQL_DESC_LENGTH */
    stlInt64              mOctetLength;       /**< SQL_DESC_OCTET_LENGTH */
    
    stlChar  * mLiteralPrefix;     /**< SQL_DESC_LITERAL_PREFIX */
    stlChar  * mLiteralSuffix;     /**< SQL_DESC_LITERAL_SUFFIX */

    /*
     * Base Column 관련 정보
     */

    ellDictHandle       * mBaseColumnHandle;   /**< Base Column Handle */
    
    struct qllTarget * mNext;
} qllTarget;
 

/**
 * @brief Fetch Prev 및 Fetch Next를 위한 최대 Fetch Count
 */
#define QLL_FETCH_COUNT_MAX        ( SML_FETCH_COUNT_MAX )


/** @} qllTargetInfo */

/** @} qllAPI */


/**
 * @defgroup qllSessEnv Session Environment
 * @ingroup qllEnvironment
 * @{
 */


/**
 * @brief Session NLS Information
 */
typedef struct qllSessNLS
{
    /*
     * Non Property
     */
    
    stlInt32        mTimeZoneOffset;     /**< Time Zone Offset */
    stlInt32        mTimeZoneOffsetOrg;  /**< Original Time Zone Offset */

    dtlCharacterSet mCharacterSet;       /** Character Set */
    

    /*
     * from Property
     */


    /*
     * NLS_DATE_FORMAT
     */
    
    stlChar                 * mDATE_FORMAT;    
    dtlDateTimeFormatInfo   * mDateFormatInfo;    /* TO_CHAR() TO_DATE() 용 */

    /*
     * NLS_TIME_FORMAT
     */
    
    stlChar                 * mTIME_FORMAT;    
    dtlDateTimeFormatInfo   * mTimeFormatInfo;    /* TO_CHAR() TO_TIME() 용  */

    /*
     * NLS_TIME_WITH_TIME_ZONE_FORMAT
     */
    
    stlChar                 * mTIME_WITH_TIME_ZONE_FORMAT;    
    dtlDateTimeFormatInfo   * mTimeWithTimeZoneFormatInfo;    /* TO_CHAR() TO_TIME_WITH_TIME_ZONE() 용 */
    
    /*
     * NLS_TIMESTAMP_FORMAT
     */
    
    stlChar                 * mTIMESTAMP_FORMAT;    
    dtlDateTimeFormatInfo   * mTimestampFormatInfo;    /* TO_CHAR() TO_TIMESTAMP() 용  */

    /*
     * NLS_TIMESTAMP_WITH_TIME_ZONE_FORMAT
     */
    
    stlChar                 * mTIMESTAMP_WITH_TIME_ZONE_FORMAT;    
    dtlDateTimeFormatInfo   * mTimestampWithTimeZoneFormatInfo;    /* TO_CHAR() TO_TIMESTAMP_WITH_TIME_ZONE() 용  */
    
} qllSessNLS;


/**
 * @brief SQL Processor 의 Session Environment 구조체
 * - Retry SQL Memory : Retry SQL for Recompile, Multiplexing 에 의해 공유
 * - Code Plan Memory : Plan Cache, Multiplexing 에 의해 공유
 * - Data Plan Memory : Multiplexing 에 의해 공유
 */
typedef struct qllSessionEnv
{
    ellSessionEnv    mEllSessionEnv;     /**< 하위 Layer 의 Session Environment */

    /*
     * Session Level Memory 관리자 
     */

    knlDynamicMem    mMemCollision;      /**< Collision Memory 관리자 */
    knlDynamicMem    mMemNLS;            /**< NLS Memory 관리자 */
    knlDynamicMem    mMemParentDBC;      /**< Parent DBC Memory 관리자 */

    /*
     * Session Level 정보
     */

    qllAccessMode             mAccessMode;      /**< Access Mode */

    qllUniqueMode               mUniqueMode;      /**< SET SESSION .. UNIQUE INTEGRITY IS DEFERRED/DEFAULT */
    qllDeferrableConstraintInfo mDeferConstInfo;  /**< Deferrable Constraint Information */

    qllSessNLS                mSessNLS;         /**< Session NLS information */
    
    /*
     * Trace Log
     */

    knlTraceLogger    mTraceLogger;         /**< Trace Logger */
    stlBool           mTraceLoggerInited;   /**< Trace Logger 초기화 여부 */
    stlBool           mTraceLoggerEnabled;  /**< Log Trace Enable 여부 */

    /*
     * for X$SQL_SESS_ENV fixed table
     */

    stlInt64          mParsingCnt;       /**< Parsing Call Count */
    stlInt64          mValidateCnt;      /**< Validate Call Count */
    stlInt64          mBindContextCnt;   /**< Bind Context Call Count */
    stlInt64          mOptCodeCnt;       /**< Code Optimize Call Count */
    stlInt64          mOptDataCnt;       /**< Data Optimize Call Count */
    stlInt64          mRecheckCnt;       /**< Recheck Call Count */
    stlInt64          mExecuteCnt;       /**< Execute Call Count */
    stlInt64          mRecompileCnt;     /**< Recompile Call Count */
    stlInt64          mFetchCnt;         /**< Fetch Call Count */

    /*
     * for X$SQL_SESS_EXEC_STMT fixed table
     */

    stlInt64          mExecStmtCnt[QLL_STMT_MAX - QLL_STMT_NA];
    
} qllSessionEnv;

/**
 * @brief Sql Processor 의 Session Environment 를 얻는다.
 * @param[in]  aEnv Process Environment 포인터
 */
#define QLL_SESS_ENV(aEnv)    ((qllSessionEnv*)(KNL_ENV(aEnv)->mSessionEnv))

#define QLL_COLLISION_MEM(aEnv)  (QLL_SESS_ENV(aEnv)->mMemCollision)


/** @} qllSessEnv */


#include <qllTrace.h>

/**
 * @todo Role 관련 코드가 작성되면 아래 코드는 삭제되어야 한다.
 */
#define QLL_ROLE_STRING_ADMIN  "ADMIN"
#define QLL_ROLE_STRING_SYSDBA "SYSDBA"

#endif /* _QLLDEF_H_ */

