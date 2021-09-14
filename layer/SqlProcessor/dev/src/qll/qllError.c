/*******************************************************************************
 * qllError.c
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
 * @file qllError.c
 * @brief SQL Processor Layer Error Routines
 */

#include <stl.h>


/**
 * @addtogroup qllError
 * @{
 */

/**
 * @brief SQL Processor Library Layer 에러 테이블.
 * 내부에러에 대응되는 외부에러와 에러 메세지로 구성됨.
 */
stlErrorRecord gSqlProcessorLibraryErrorTable[] =
{
    {   /* QLL_ERRCODE_INITIALIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "failed to initialize sql processor layer"
    },
    {   /* QLL_ERRCODE_NOT_IMPLEMENTED */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
        "not implemented feature, in a function %s"
    },
    {   /* QLL_ERRCODE_INVALID_FUNCTION_SEQUENCE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid function sequence of sql processor layer"
    },
    {   /* QLL_ERRCODE_IDENTIFIER_LENGTH_EXCEEDED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "identifier is too long"
    },
    {   /* QLL_ERRCODE_INVALID_LOGIN */
        STL_EXT_ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION_NO_SUBCLASS,
        "invalid username/password; logon denied"
    },
    {   /* QLL_ERRCODE_USED_OBJECT_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "name '%s.%s' is already used by an existing object"
    },
    {   /* QLL_ERRCODE_SCHEMA_NOT_EXISTS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "schema '%s' does not exist"
    },
    {   /* QLL_ERRCODE_SCHEMA_NOT_WRITABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "schema '%s' is not writable"
    },
    {   /* QLL_ERRCODE_TABLESPACE_NOT_EXISTS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "tablespace '%s' does not exist"
    },
    {   /* QLL_ERRCODE_TABLESPACE_NOT_WRITABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "tablespace '%s' is not writable"
    },
    {   /* QLL_ERRCODE_INVALID_PCTVALUE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid value for PCTFREE or PCTUSED"
    },
    {   /* QLL_ERRCODE_INVALID_INITRANS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid INITRANS option value"
    },
    {   /* QLL_ERRCODE_DUPLICATE_COLUMN_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "duplicate column name"
    },
    {   /* QLL_ERRCODE_VALIDATION_INVALID_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid value is prevented in validation step"
    },
    {   /* QLL_ERRCODE_SEQUENCE_INCREMENT_ZERO */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "INCREMENT must be a non-zero integer"
    },
    {   /* QLL_ERRCODE_SEQUENCE_STARTWITH_UNDER_MINVALUE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "START WITH cannot be less than MINVALUE"
    },
    {   /* QLL_ERRCODE_SEQUENCE_STARTWITH_OVER_MAXVALUE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "START WITH cannot be more than MAXVALUE"
    },
    {   /* QLL_ERRCODE_SEQUENCE_CACHE_SIZE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "the number of values to CACHE must be greater than 1"
    },
    {   /* QLL_ERRCODE_SEQUENCE_CACHE_RANGE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "number to CACHE must be less than one cycle"
    },
    {   /* QLL_ERRCODE_SEQUENCE_MINVALUE_RANGE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "MINVALUE must be less than MAXVALUE"
    },
    {   /* QLL_ERRCODE_SEQUENCE_INCREMENT_RANGE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "INCREMENT must be less than MAXVALUE minus MINVALUE"
    },
    {   /* QLL_ERRCODE_SEQUENCE_DESCENDING_CYCLE_NO_MINVALUE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "descending sequences that CYCLE must specify MINVALUE"
    },
    {   /* QLL_ERRCODE_SEQUENCE_ASCENDING_CYCLE_NO_MAXVALUE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "ascending sequences that CYCLE must specify MAXVALUE"
    },
    {   /* QLL_ERRCODE_SEQUENCE_CACHE_NOCACHE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE,
        "duplicate or conflicting CACHE/NO CACHE specifications"
    },
    {   /* QLL_ERRCODE_NUMERIC_PRECISION_INVALID */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "numeric precision specifier is out of range( %d to %d )"
    },
    {   /* QLL_ERRCODE_NUMERIC_SCALE_INVALID */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "numeric scale specifier is out of range( %d to %d )"
    },
    {   /* QLL_ERRCODE_CHARACTER_LENGTH_INVALID */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "character length specifier is out of range( 1 to %d )"
    },
    {   /* QLL_ERRCODE_CHARACTER_VARYING_LENGTH_INVALID */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "character varying length specifier is out of range( 1 to %d )"
    },
    {   /* QLL_ERRCODE_BINARY_LENGTH_INVALID */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "binary length specifier is out of range( 1 to %d )"
    },
    {   /* QLL_ERRCODE_BINARY_VARYING_LENGTH_INVALID */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "binary varying length specifier is out of range( 1 to %d )"
    },
    {   /* QLL_ERRCODE_CONSTRAINT_NAME_EXISTS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "name already used by an existing constraint"
    },
    {   /* QLL_ERRCODE_AUTO_CONSTRAINT_NAME_EXISTS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "auto-generated name already used by an existing constraint, specify unique constraint name"
    },
    {   /* QLL_ERRCODE_MULTIPLE_PRIMARY_KEY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "table can have only one primary key"
    },
    {   /* QLL_ERRCODE_MULTIPLE_UNIQUE_KEY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "such unique or primary key already exists in the table"
    },
    {   /* QLL_ERRCODE_INDEX_NAME_EXISTS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "name already used by an existing index"
    },
    {   /* QLL_ERRCODE_AUTO_INDEX_NAME_EXISTS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "auto-generated name already used by an existing index, specify non-duplicate index name"
    },
    {   /* QLL_ERRCODE_INVALID_IDENTIFIER */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "'%s': invalid identifier"
    },
    {   /* QLL_ERRCODE_MAXIMUM_COLUMN_EXCEEDED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "maximum number of key columns exceeded"
    },
    {   /* QLL_ERRCODE_INTEGRITY_CONSTRAINT_VIOLATION */
        STL_EXT_ERRCODE_INTEGRITY_CONSTRAINT_VIOLATION_NO_SUBCLASS,
        "integrity constraint violation"
    },
    {   /* QLL_ERRCODE_SAVEPOINT_EXCEPTION */
        STL_EXT_ERRCODE_SAVEPOINT_EXCEPTION_NO_SUBCLASS,
        "savepoint '%s' never established in this session or is invalid"
    },
    {   /* QLL_ERRCODE_TABLE_OR_VIEW_NOT_EXIST */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "table or view does not exist"
    },
    {   /* QLL_ERRCODE_TABLE_NOT_WRITABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "table '%s' is not writable"
    },
    {   /* QLL_ERRCODE_TABLE_KEY_REFERENCED_BY_FOREIGN_KEY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "unique/primary keys in table referenced by foreign keys"
    },
    {   /* QLL_ERRCODE_TABLE_REFERENCED_BY_VIEW */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "table referenced by views"
    },
    {   /* QLL_ERRCODE_SEQUENCE_NOT_EXIST */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "sequence does not exist"
    },
    {   /* QLL_ERRCODE_DUPLICATE_OPTIONS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "duplicate options"
    },
    {   /* QLL_ERRCODE_SAME_INDEX_KEY_COLUMNS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "such column list already indexed"
    },
    {   /* QLL_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "numeric value out of range"
    },
    {   /* QLL_ERRCODE_INDEX_NOT_EXIST */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "specified index does not exist"
    },
    {   /* QLL_ERRCODE_INDEX_NOT_WRITABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "index '%s' is not writable"
    },
    {   /* QLL_ERRCODE_INDEX_WITH_KEY_CONSTRAINT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot modify index used for enforcement of unique/primary/foreign key"
    },
    {   /* QLL_ERRCODE_NOT_APPLICABLE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "not applicable"
    },
    {   /* QLL_ERRCODE_COLUMN_NOT_ALLOWED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column not allowed here"
    },
    {   /* QLL_ERRCODE_NOT_ENOUGH_VALUES */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "not enough values"
    },
    {   /* QLL_ERRCODE_TOO_MANY_VALUES */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "too many values"
    },
    {   /* QLL_ERRCODE_NULL_VALUE_NOT_ALLOWED */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NULL_VALUE_NOT_ALLOWED,
        "NULL value not allowed"
    },
    {   /* QLL_ERRCODE_CANNOT_INSERT_NULL_INTO */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NULL_VALUE_NOT_ALLOWED,
        "cannot insert NULL into \"%s\".\"%s\".\"%s\""
    },
    {   /* QLL_ERRCODE_UNIQUE_CONSTRAINT_VIOLATED */
        STL_EXT_ERRCODE_INTEGRITY_CONSTRAINT_VIOLATION_NO_SUBCLASS,
        "unique constraint (%s.%s) violated"
    },
    {   /* QLL_ERRCODE_NOT_APPLICABLE_HINT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "not applicable hint"
    },
    {   /* QLL_ERRCODE_NOT_APPLICABLE_DATATYPE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "not applicable datatype"
    },
    {   /* QLL_ERRCODE_BIND_VARIABLE_NOT_EXIST */
        STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_USING_CLAUSE_DOES_NOT_MATCH_TARGET_SPEC,
        "bind variable does not exist"
    },
    {   /* QLL_ERRCODE_CANNOT_UPDATE_NULL */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NULL_VALUE_NOT_ALLOWED,
        "cannot update (\"%s\".\"%s\".\"%s\") to NULL"
    },
    {   /* QLL_ERRCODE_SYNTAX_ERROR */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "syntax error"
    },
    {   /* QLL_ERRCODE_FRACTIONAL_SECOND_PRECISION_INVALID */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "fractional seconds precision specifier is out of range( %d to %d )"
    },
    {   /* QLL_ERRCODE_INVALID_INTERVAL_INDICATOR */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid interval indicator"
    },
    {   /* QLL_ERRCODE_INVALID_INTERVAL_LEADING_PRECISION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "interval leading precision specifier is out of range( %d to %d )"
    },
    {   /* QLL_ERRCODE_INVALID_INTERVAL_FRACTIONAL_SECONDS_PRECISION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "interval fractional seconds precision specifier is out of range( %d to %d )"
    },
    {   /* QLL_ERRCODE_INVALID_INITIAL_STORAGE_OPTION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid INITIAL storage option value"
    },
    {   /* QLL_ERRCODE_INVALID_NEXT_STORAGE_OPTION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid NEXT storage option value"
    },
    {   /* QLL_ERRCODE_INVALID_MINSIZE_STORAGE_OPTION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid MINSIZE storage option value"
    },
    {   /* QLL_ERRCODE_INVALID_MAXSIZE_STORAGE_OPTION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid MAXSIZE storage option value"
    },
    {   /* QLL_ERRCODE_INVALID_MINMAX_STORAGE_OPTION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "storage option MAXSIZE is less than MINSIZE"
    },
    {   /* QLL_ERRCODE_DUPLICATE_CONSTRAINTS_NULL_NOT_NULL */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "duplicate or conflicting NULL and/or NOT NULL specifications"
    },
    {   /* QLL_ERRCODE_DUPLICATE_CONSTRAINTS_UNIQUE_PRIMARY_KEY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "duplicate UNIQUE/PRIMARY KEY specifications"
    },
    {   /* QLL_ERRCODE_SEQUENCE_NUMBER_NOT_ALLOWED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "sequence number not allowed here"
    },
    {   /* QLL_ERRCODE_INCONSISTENT_DATATYPES */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "inconsistent datatypes : expected %s"
    },
    {   /* QLL_ERRCODE_INVALID_MEMORY_DATAFILE_CLAUSE_FOR_ALTER */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid DATAFILE clause for alter of %s %s TABLESPACE"
    },
    {   /* QLL_ERRCODE_INVALID_DATAFILE_FOR_ALTER */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid DATAFILE clause for alter of %s %s TABLESPACE"
    },
    {   /* QLL_ERRCODE_INVALID_TEMPFILE_FOR_ALTER */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid TEMPFILE clause for alter of %s %s TABLESPACE"
    },
    {   /* QLL_ERRCODE_FILEPATH_TOO_LONG */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "file path is too long ( < %d )"
    },
    {   /* QLL_ERRCODE_FILESIZE_OUT_OF_RANGE */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "file size is out of range( %ld to %ld )"
    },
    {   /* QLL_ERRCODE_ALREADY_EXIST_SYSTEM_THREAD */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "failed to open database : system threads are already exist."
    },
    {   /* QLL_ERRCODE_INVALID_MAXTRANS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid MAXTRANS option value"
    },
    {   /* QLL_ERRCODE_TRANS_RANGE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "INITRANS must be less equal than MAXTRANS"
    },
    {   /* QLL_ERRCODE_RESULT_OFFSET_MUST_BE_ZERO_OR_POSITIVE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "result OFFSET must be 0 or positive numeric value"
    },
    {   /* QLL_ERRCODE_RESULT_LIMIT_MUST_BE_POSITIVE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "result LIMIT/FETCH must be positive numeric value"
    },
    {   /* QLL_ERRCODE_INVALID_OBJECT_BY_CONCURRENT_DDL */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "related objects are modified by concurrent DDL execution"
    },
    {   /* QLL_ERRCODE_INVALID_WHERE_CLAUSE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "boolean expression expected on where clause"
    },
    {   /* QLL_ERRCODE_SESSION_CANNOT_BE_KILLED_NOW */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "session cannot be killed now"
    },
    {   /* QLL_ERRCODE_BUILT_IN_AUTHORIZATION_IS_NOT_MODIFIABLE */
        STL_EXT_ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION_NO_SUBCLASS,
        "built-in authorization(user or role) '%s' is not modifiable"
    },
    {   /* QLL_ERRCODE_BUILT_IN_TABLESPACE_IS_NOT_MODIFIABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "built-in tablespace '%s' is not modifiable"
    },
    {   /* QLL_ERRCODE_BUILT_IN_SCHEMA_IS_NOT_MODIFIABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "built-in schema '%s' is not modifiable"
    },
    {   /* QLL_ERRCODE_BUILT_IN_TABLE_IS_NOT_MODIFIABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "built-in table or view '%s' is not modifiable"
    },
    {   /* QLL_ERRCODE_BUILT_IN_INDEX_IS_NOT_MODIFIABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "built-in index '%s' is not modifiable"
    },
    {   /* QLL_ERRCODE_BUILT_IN_CONSTRAINT_IS_NOT_MODIFIABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "built-in constraint '%s' is not modifiable"
    },
    {   /* QLL_ERRCODE_BUILT_IN_SEQUENCE_IS_NOT_MODIFIABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "built-in sequence '%s' is not modifiable"
    },
    {   /* QLL_ERRCODE_AUTHORIZATION_NOT_EXIST */
        STL_EXT_ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION_NO_SUBCLASS,
        "authorization(user or role) '%s' does not exist"
    },
    {   /* QLL_ERRCODE_CONSTRAINT_NOT_EXIST */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "constraint '%s' does not exist"
    },
    {   /* QLL_ERRCODE_BIND_TYPE_MISMATCH */
        STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION,
        "bind type mismatch of parameter number (%d)"
    },
    {   /* QLL_ERRCODE_NUMBER_OF_ELEMENTS_INTO_LIST_MISMATCH */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "the number of elements in the INTO list is mismatch"
    },
    {   /* QLL_ERRCODE_SINGLE_ROW_SUBQUERY_RETURNS_MORE_THAN_ONE_ROW */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "single-row subquery returns more than one row"
    },
    {   /* QLL_ERRCODE_INVALID_PARALLEL_FACTOR */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "parallel factor is out of range( 0 to %d )"
    },
    {   /* QLL_ERRCODE_INVALID_MEMORY_CLAUSE_FOR_ALTER */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid MEMORY clause for alter of %s %s TABLESPACE"
    },
    {   /* QLL_ERRCODE_CURSOR_ALREADY_DECLARED */
        STL_EXT_ERRCODE_AMBIGUOUS_CURSOR_NAME_NO_SUBCLASS,
        "cursor '%s' is already declared"
    },
    {   /* QLL_ERRCODE_INVALID_CURSOR_QUERY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid cursor query: '%s'"
    },
    {   /* QLL_ERRCODE_INVALID_CURSOR_PROPERTY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid cursor property"
    },
    {   /* QLL_ERRCODE_CURSOR_NOT_EXIST */
        STL_EXT_ERRCODE_INVALID_CURSOR_NAME_NO_SUBCLASS,
        "cursor '%s' does not exist"
    },
    {   /* QLL_ERRCODE_CURSOR_ALREADY_OPEN */
        STL_EXT_ERRCODE_INVALID_CURSOR_NAME_NO_SUBCLASS,
        "cursor '%s' is already open"
    },
    {   /* QLL_ERRCODE_CURSOR_IS_NOT_OPEN */
        STL_EXT_ERRCODE_INVALID_CURSOR_NAME_NO_SUBCLASS,
        "cursor '%s' is not open"
    },
    {   /* QLL_ERRCODE_CURSOR_NOT_DECLARED_CURSOR */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid use: cursor '%s' is not a declared cursor"
    },
    {   /* QLL_ERRCODE_CURSOR_NOT_ODBC_CURSOR */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "iinvalid use: cursor '%s' is not an ODBC/JDBC cursor"
    },
    {   /* QLL_ERRCODE_CURSOR_IS_NOT_SCROLLABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cursor '%s' is not scrollable"
    },
    {   /* QLL_ERRCODE_QUERY_IS_NOT_UPDATABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "query expression is not updatable"
    },
    {   /* QLL_ERRCODE_INVALID_EXECUTABLE_PHASE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid phase(%s): executable phase of '%s' is between %s and %s"
    },
    {   /* QLL_ERRCODE_INVALID_MOUNTABLE_PHASE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "database is already mounted"
    },
    {   /* QLL_ERRCODE_INVALID_OPENABLE_PHASE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "database is already open"
    },
    {   /* QLL_ERRCODE_TRANSACTION_IS_READ_ONLY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "transaction is READ ONLY"
    },
    {   /* QLL_ERRCODE_DATABASE_NOT_MOUNTED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "database not mounted"
    },
    {   /* QLL_ERRCODE_SUPPLEMENTAL_LOGGING_PK_EXISTS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "supplemental logging attribute primary key exists"
    },
    {   /* QLL_ERRCODE_CANNOT_DROP_NONEXISTENT_PK_SUPPLEMENTAL_LOGGING */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot drop nonexistent primary key supplemental logging"
    },
    {   /* QLL_ERRCODE_COLUMN_REFERENCED_BY_VIEW */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column referenced by views"
    },
    {   /* QLL_ERRCODE_COLUMN_HAS_CHECK_CONSTRAINT_DEFINED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column has a check constraint defined"
    },
    {   /* QLL_ERRCODE_PSEUDO_COLUMN_NOT_ALLOWED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "pseudo column not allowed here"
    },
    {   /* QLL_ERRCODE_PSEUDO_COLUMN_CANNOT_SELECT_FROM_NOT_WRITABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot select ROWID from not writable table"
    },  
    {   /* QLL_ERRCODE_MULTIPLE_IDENTITY_COLUMN */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "table can have only one identity column"
    },
    {   /* QLL_ERRCODE_COLUMN_BEING_ADDED_EXISTS_IN_TABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column being added already exists in table"
    },
    {   /* QLL_ERRCODE_TABLE_MUST_EMPTY_TO_ADD_MANDATORY_COLUMN */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "table must be empty to add mandatory (%s) column ('%s')"
    },
    {   /* QLL_ERRCODE_CANNOT_MODIFY_STORAGE_INITIAL */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot modify INITIAL storage option"
    },
    {   /* QLL_ERRCODE_USED_TABLESPACE_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "name '%s' is already used by an existing tablespace"
    },
    {   /* QLL_ERRCODE_DUPLICATE_DATAFILE_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "duplicate datafile name"
    },
    {   /* QLL_ERRCODE_ALREADY_EXIST_FILE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "file is already exist - '%s'"
    },
    {   /* QLL_ERRCODE_RENAME_BUILTIN_TABLESPACE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot rename the built-in tablespace"
    },
    {   /* QLL_ERRCODE_RENAME_OFFLINE_TABLESPACE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot rename the OFFLINE tablespace"
    },
    {   /* QLL_ERRCODE_DROP_BUILTIN_TABLESPACE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot drop the built-in tablespace"
    },
    {   /* QLL_ERRCODE_DROP_CONSTRAINT_NONEXISTENT_CONSTRAINT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot drop constraint - nonexistent constraint"
    },
    {   /* QLL_ERRCODE_DROP_CONSTRAINT_NONEXISTENT_PRIMARY_KEY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot drop constraint - nonexistent primary key"
    },
    {   /* QLL_ERRCODE_DROP_CONSTRAINT_NONEXISTENT_UNIQUE_KEY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot drop constraint - nonexistent unique key"
    },
    {   /* RAMP_ERR_CDS_ABORT_CLOSE_OPTION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "the CDS database cannot be closed with ABORT option"
    },
    {   /* RAMP_ERR_INVALID_DS_CLOSE_OPTION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "the DS database can be closed with NORMAL option - deprecated"
    },
    {   /* QLL_ERRCODE_MULTIPLE_ORDER_BY_CLAUSE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "multiple ORDER BY clauses not allowed"
    },
    {   /* QLL_ERRCODE_MULTIPLE_OFFSET_CLAUSE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "multiple OFFSET clauses not allowed"
    },
    {   /* QLL_ERRCODE_MULTIPLE_LIMIT_CLAUSE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "multiple LIMIT clauses not allowed"
    },
    {   /* QLL_ERRCODE_COLUMN_AMBIGUOUS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column ambiguously defined"
    },
    {   /* QLL_ERRCODE_COLUMN_PART_OF_USING_CLAUSE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column part of USING clause cannot have qualifier"
    },
    {   /* QLL_ERRCODE_INVALID_OFFSET */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid OFFSET option value"
    },
    {   /* QLL_ERRCODE_INVALID_LIMIT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid LIMIT option value"
    },
    {   /* QLL_ERRCODE_EXIST_SAME_COLUMN_NAME_IN_TABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "common column name '%s' appears more than once in %s table"
    },
    {   /* QLL_ERRCODE_ALREADY_USED_DATAFILE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "file is already used in database"
    },
    {   /* QLL_ERRCODE_TABLESPACE_NOT_EMPTY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "tablespace not empty, use INCLUDING CONTENTS option"
    },
    {   /* QLL_ERRCODE_DATAFILE_NOT_EXISTS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "datafile does not exist in tablespace"
    },
    {   /* QLL_ERRCODE_DROP_OFFLINE_DATAFILE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot drop datafile of OFFLINE tablespace"
    },
    {   /* QLL_ERRCODE_DROP_FIRST_DATAFILE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot drop first datafile of tablespace"
    },
    {   /* QLL_ERRCODE_ALREADY_USED_MEMORY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "memory is already used in database"
    },
    {   /* QLL_ERRCODE_MEMORY_NOT_EXISTS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "memory does not exist in tablespace"
    },
    {   /* QLL_ERRCODE_DROP_FIRST_MEMORY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot drop first memory of tablespace"
    },
    {   /* QLL_ERRCODE_TABLESPACE_NOT_WRITABLE_FOR_LOGGING_INDEX */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "tablespace '%s' is not writable for logging index"
    },
    {   /* QLL_ERRCODE_TABLESPACE_NOT_WRITABLE_FOR_NOLOGGING_INDEX */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "tablespace '%s' is not writable for nologging index"
    },
    {   /* QLL_ERRCODE_ORDER_BY_NUMBER_MISMATCH */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "ORDER BY item must be the number of a SELECT-list expression"
    },
    {   /* QLL_ERRCODE_GROUP_FUNCTION_NOT_ALLOWED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "group function is not allowed here"
    },
    {   /* QLL_ERRCODE_INVALID_NUMBER_OF_ARGUMENTS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid number of arguments"
    },
    {   /* QLL_ERRCODE_GROUP_FUNCTION_IS_NESTED_TOO_DEEPLY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "group function is nested too deeply"
    },
    {   /* QLL_ERRCODE_NOT_A_GROUP_FUNCTION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "not a single-group group function"
    },
    {   /* QLL_ERRCODE_MISMATCHED_DATAFILE_PAIRS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "mismatched datafile pairs"
    },
    {   /* QLL_ERRCODE_RENAME_ONLINE_DATAFILE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot rename datafile of ONLINE tablespace"
    },
    {   /* QLL_ERRCODE_FILE_NOT_EXIST */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "file does not exist"
    },
    {   /* QLL_ERRCODE_NOT_ONLINE_TABLESPACE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "tablespace is not online"
    },
    {   /* QLL_ERRCODE_NOT_OFFLINE_TABLESPACE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "tablespace is not offline"
    },
    {   /* QLL_ERRCODE_OFFLINE_TEMPORARY_TABLESPACE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot offline temporary tablespace"
    },
    {   /* QLL_ERRCODE_OFFLINE_DICTIONARY_TABLESPACE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot offline dictionary tablespace"
    },
    {   /* QLL_ERRCODE_OFFLINE_UNDO_TABLESPACE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot offline undo tablespace"
    },
    {   /* QLL_ERRCODE_INVALID_WAIT_INTERVAL */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid WAIT interval"
    },
    {   /* QLL_ERRCODE_COLUMN_IS_NOT_UPDATABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column is not updatable"
    },
    {   /* QLL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "data is outside the range of the data type to which the number is being converted"
    },
    {   /* QLL_ERRCODE_C_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INTERVAL_FIELD_OVERFLOW,
        "one or more fields of data value were truncated during conversion"
    },
    {   /* QLL_ERRCODE_SYSTEM_SESSION_CANNOT_BE_KILLED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "system session cannot be killed"
    },
    {   /* QLL_ERRCODE_HINT_NOT_ALLOWED_POSITIONED_DELETE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "hint not allowed in positioned DELETE"
    },
    {   /* QLL_ERRCODE_HINT_NOT_ALLOWED_POSITIONED_UPDATE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "hint not allowed in positioned UPDATE"
    },
    {   /* QLL_ERRCODE_CURSOR_IS_NOT_UPDATABLE */
        STL_EXT_ERRCODE_INVALID_CURSOR_NAME_NO_SUBCLASS,
        "cursor '%s' is not updatable"
    },
    {   /* QLL_ERRCODE_CURSOR_CANNOT_IDENTIFY_UNDERLYING_TABLE */
        STL_EXT_ERRCODE_INVALID_CURSOR_NAME_NO_SUBCLASS,
        "cursor cannot identify the underlying table"
    },
    {   /* QLL_ERRCODE_CURSOR_WAS_NOT_POSITIONED_ON_A_CERTAIN_ROW */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_CURSOR_POSITION,
        "cursor was not positioned on a certain row"
    },
    {   /* QLL_ERRCODE_CURSOR_WAS_POSITIONED_ON_A_ROW_THAT_HAD_BEEN_DELETED */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_CURSOR_POSITION,
        "cursor was positioned on a row that had been deleted"
    },
    {   /* QLL_ERRCODE_CDS_OPERATIONAL_CLOSE_OPTION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "the CDS database cannot be closed with OPERATIONAL option"
    },
    {   /* QLL_ERRCODE_INVALID_SEGMENT_ID */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid segment identifier"
    },
    {   /* QLL_ERRCODE_FETCH_POSITION_MUST_BE_NUMERIC_VALUE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "FETCH position must be numeric value"
    },
    {   /* QLL_ERRCODE_CANNOT_DROP_ALL_COLUMNS_IN_TABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot drop all columns in a table"
    },
    {   /* QLL_ERRCODE_COLUMN_ALREADY_HAS_NOT_NULL_CONSTRAINT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column already has a NOT NULL constraint"
    },
    {   /* QLL_ERRCODE_COLUMN_DOES_NOT_HAVE_NOT_NULL_CONSTRAINT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column does not have a NOT NULL constraint"
    },
    {   /* QLL_ERRCODE_NULL_VALUES_FOUND */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "null values found"
    },
    {   /* QLL_ERRCODE_RESULT_SET_NOT_EXIST */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "result set does not exist"
    },
    {   /* QLL_ERRCODE_COLUMN_NOT_IDENTITY_COLUMN */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column is not an identity column"
    },
    {   /* QLL_ERRCODE_COLUMN_CANNOT_CAST_IMCOMPATIBLE_TYPE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column cannot be cast to incompatible type"
    },
    {   /* QLL_ERRCODE_COLUMN_CANNOT_CAST_DECREASE_PRECISION_OR_SCALE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column cannot be cast to decrease precision or scale"
    },
    {   /* QLL_ERRCODE_COLUMN_CANNOT_CAST_DECREASE_STRING_LENGTH */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column cannot be cast to decrease string length"
    },
    {   /* QLL_ERRCODE_COLUMN_CANNOT_CAST_INCOMPATIBLE_CHAR_LENGTH_UNIT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column cannot be cast to incompatible char length unit"
    },
    {   /* QLL_ERRCODE_COLUMN_CANNOT_CAST_INCOMPATIBLE_INTERVAL_INDICATOR */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column cannot be cast to incompatible interval indicator"
    },
    {   /* QLL_ERRCODE_FLOAT_PRECISION_INVALID */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "float precision specifier is out of range( %d to %d )"
    },
    {   /* QLL_ERRCODE_ALREADY_USED_LOGFILE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "file is already used in log stream - '%s'"
    },
    {   /* QLL_ERRCODE_DUPLICATE_LOGFILE_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "duplicate logfile name"
    },
    {   /* QLL_ERRCODE_TOO_SMALL_LOG_FILE_SIZE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "size of log file is smaller than minimum size of log file(%ld bytes)."
    },
    {   /* QLL_ERRCODE_LOG_MEMBER_NOT_FOUND */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "can not find specified log member."
    },
    {   /* QLL_ERRCODE_LOG_FILE_COUNT_MISMATCH */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "the number of source logfiles is different from the number of targets"
    },
    {   /* QLL_ERRCODE_LOG_FILE_NOT_FOUND */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "can not find specified log file."
    },
    {   /* QLL_ERRCODE_USED_SCHEMA_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "name '%s' is already used by an existing schema"
    },
    {   /* QLL_ERRCODE_SCHEMA_NOT_EMPTY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "schema not empty, use CASCADE option"
    },
    {   /* QLL_ERRCODE_INPROGRESS_CHECKPOINT */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "another checkpoint is in progress"
    },
    {   /* QLL_ERRCODE_BOOLEAN_EXPRESSION_EXPECTED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "boolean expression expected"
    },
    {   /* QLL_ERRCODE_MAXIMUM_NUMBER_OF_ARGUMENTS_EXCEEDED */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "maximum number of arguments exceeded"
    },
    {   /* QLL_ERRCODE_TYPE_MISMATCH */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "%s and %s cannot be matched"
    },
    {   /* QLL_ERRCODE_INSUFFICENT_PRIVILEGES */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "insufficient privileges"
    },
    {   /* QLL_ERRCODE_USER_LACKS_CREATE_SESSION_PRIVILEGE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "user %s lacks CREATE SESSION privilege; logon denied"
    },
    {   /* QLL_ERRCODE_LACKS_PRIVILEGE_ON_DATABASE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "lacks privilege (%s ON DATABASE)"
    },
    {   /* QLL_ERRCODE_LACKS_PRIVILEGE_ON_TABLESPACE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "lacks privilege (%s ON TABLESPACE \"%s\")"
    },
    {   /* QLL_ERRCODE_LACKS_PRIVILEGES_ON_SCHEMA */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "lacks privilege (%s ON SCHEMA \"%s\")"
    },
    {   /* QLL_ERRCODE_USER_NAME_CONFLICT_WITH_ANOTHER_USER_OR_ROLE_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "user name '%s' conflicts with another user or role name"
    },
    {   /* QLL_ERRCODE_TABLESPACE_NOT_DATA_TABLESPACE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "tablespace '%s' is not data tablespace"
    },
    {   /* QLL_ERRCODE_TABLESPACE_NOT_TEMPORARY_TABLESPACE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "tablespace '%s' is not temporary tablespace"
    },
    {   /* QLL_ERRCODE_USER_NOT_EXISTS */
        STL_EXT_ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION_NO_SUBCLASS,
        "user '%s' does not exist"
    },
    {   /* QLL_ERRCODE_USER_HAS_OWN_SCHEMA */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "user has own schema: DROP SCHEMA first"
    },
    {   /* QLL_ERRCODE_CASCADE_MUST_BE_SPECIFIED_TO_DROP_USER */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "CASCADE must be specified to drop user '%s'"
    },
    {   /* QLL_ERRCODE_NOT_GRANT_REVOKE_PRIVILEGES_YOURSELF */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "you may not GRANT/REVOKE privileges to/from yourself"
    },
    {   /* QLL_ERRCODE_DUPLICATE_AUTH_NAME_IN_GRATEE_REVOKEE_LIST */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "duplicate authorization name in grantee/revokee list"
    },
    {   /* QLL_ERRCODE_USER_DEFAULT_SCHEMA_DROPPED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "user default schema dropped"
    },
    {   /* QLL_ERRCODE_USER_DEFAULT_DATA_TABLESPACE_DROPPED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "user default data tablespace dropped"
    },
    {   /* QLL_ERRCODE_USER_DEFAULT_TEMPORARY_TABLESPACE_DROPPED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "user default temporary tablespace dropped"
    },
    {   /* QLL_ERRCODE_NO_PRIVILEGES_TO_GRANT_ALL_PRIVILEGES */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "no privileges to GRANT ALL PRIVILEGES"
    },
    {   /* QLL_ERRCODE_INSUFFICIENT_PRIVILEGE_TO_GRANT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "insufficient privilege to GRANT"
    },
    {   /* QLL_ERRCODE_DUPLICATE_PRIVILEGE_LISTED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "duplicate privilege listed"
    },
    {   /* QLL_ERRCODE_CANNOT_DROP_USER_CURRENTLY_CONNECTED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot drop a user that is currently connected"
    },
    {   /* QLL_ERRCODE_FIXED_TABLE_NOT_GRANT_REVOKE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "fixed table not grantable/revokable"
    },
    {   /* QLL_ERRCODE_INVALID_OLD_PASSWORD */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid old password"
    },
    {   /* QLL_ERRCODE_SCHEMA_ELEMENT_NOT_MATCH_SCHEMA_IDENTIFIER */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "schema element does not match schema identifier"
    },
    {   /* QLL_ERRCODE_NOT_GRANT_REVOKE_PRIVILEGES_BUILT_IN_AUTHORIZATION */
        STL_EXT_ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION_NO_SUBCLASS,
        "cannot GRANT/REVOKE privileges to/from built-in authorization"
    },
    {   /* QLL_ERRCODE_NOT_REVOKE_PRIVILEGES_YOU_DID_NOT_GRANT */
        STL_EXT_ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION_NO_SUBCLASS,
        "cannot REVOKE privileges you did not grant to '%s'"
    },
    {   /* QLL_ERRCODE_NOARCHIVELOG_MODE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "OFFLINE IMMEDIATE disallowed; noarchivelog mode"
    },
    {   /* QLL_ERRCODE_NO_LOGGING_TABLESPACE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "OFFLINE IMMEDIATE disallowed; no logging tablespace"
    },
    {   /* QLL_ERRCODE_DEPENDENT_PRIVILEGE_DESCRIPTORS_STILL_EXIST */
        STL_EXT_ERRCODE_DEPENDENT_PRIVILEGE_DESCRIPTORS_STILL_EXIST_NO_SUBCLASS,
        "dependent privilege descriptors still exist"
    },
    {   /* QLL_ERRCODE_NOT_SUPPORTED_QUERY */
        STL_EXT_ERRCODE_FEATURE_NOT_SUPPORTED_NO_SUBCLASS,
        "not supported query%s"
    },
    {   /* QLL_ERRCODE_SUBQUERY_BLOCK_HAS_INCORRECT_NUMBER_RESULT_COLUMNS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "a subquery block has incorrect number of result columns"
    },
    {   /* QLL_ERRCODE_ORDER_BY_ITEM_MUST_BE_NUMBER_OF_SELECT_LIST_EXPRESSION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "ORDER BY item must be the number of a SELECT-list expression"
    },
    {   /* QLL_ERRCODE_EXPRESSION_MUST_HAVE_SAME_DATATYPE_AS_CORRESPONDING_EXPRESSION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "expression must have same datatype as corresponding expression"
    },
    {   /* QLL_ERRCODE_SUB_QUERY_NOT_ALLOWED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "subquery expressions not allowed here"
    },
    {   /* QLL_ERRCODE_INVALID_NUMBER_OF_COLUMN_NAMES_SPECIFIED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid number of column names specified"
    },
    {   /* QLL_ERRCODE_MUST_NAME_THIS_EXPRESSION_WTIH_COLUMN_ALIAS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "must name this expression with a column alias"
    },
    {   /* QLL_ERRCODE_WARNING_VIEW_DEFINITION_HAS_COMPILATION_ERRORS */
        STL_EXT_ERRCODE_WARNING_NO_SUBCLASS,
        "Warning: View definition has compilation errors"
    },
    {   /* QLL_ERRCODE_BIND_VARIABLES_NOT_ALLOWED_FOR_DDL */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "bind variables not allowed for data definition operations"
    },
    {   /* QLL_ERRCODE_VIEW_HAS_ERRORS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "view \"%s.%s\" has errors"
    },
    {   /* QLL_ERRCODE_ILLEGAL_USE_OF_DATA_TYPE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "illegal use of %s data type"
    },
    {   /* QLL_ERRCODE_CANNOT_BACKUP_NOARCHIVELOG_MODE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot BACKUP; noarchivelog mode"
    },
    {   /* QLL_ERRCODE_CANNOT_RECOVER_TABLESPACE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot RECOVER TABLESPACE; tablespace is in use"
    },
    {   /* QLL_ERRCODE_BACKUP_IN_PROGRESS */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot SHUTDOWN; backup in progress"
    },
    {   /* QLL_ERRCODE_CANNOT_RECOVER_UNTIL_CANCEL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "RECOVER UNTIL CANCEL can be issued by directly attached session only - deprecated"
    },
    {   /* QLL_ERRCODE_SINGLE_ROW_SUBQUERY_RETURNS_MULTIPLE_ROWS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,  
        "single-row subquery returns more than one row"   /* 100번의 Error Code와 중복 */
    },
    {   /* QLL_ERRCODE_CIRCULAR_VIEW_DEFINITION_ENCOUNTERED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "circular view definition encountered"
    },
    {   /* QLL_ERRCODE_ONLY_BASE_TABLE_COLUMNS_ARE_ALLOWED_TO_GRANT_REVOKE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "only base table columns are allowed to GRANT/REVOKE"
    },
    {   /* QLL_ERRCODE_LACKS_PRIVILEGES_ON_TABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "lacks privilege (%s ON TABLE \"%s\".\"%s\")"
    },
    {   /* QLL_ERRCODE_LACKS_PRIVILEGES_ON_SEQUENCE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "lacks privilege (USAGE ON SEQUENCE \"%s\".\"%s\")"
    },
    {   /* QLL_ERRCODE_WRONG_NUMBER_OF_PARAMETERS */
        STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_USING_CLAUSE_DOES_NOT_MATCH_DYNAMIC_PARAMETER_SPEC,
        "wrong number of parameters"
    },
    {   /* QLL_ERRCODE_ARGUMENT_OF_CASE_WHEN_MUST_BE_TYPE_BOOLEAN */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "argument of CASE/WHEN must be type boolean"
    },
    {   /* QLL_ERRCODE_XA_NOTA */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "the XID does not exist"
    },
    {   /* QLL_ERRCODE_INVALID_XID_STRING */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "the XID string is not valid"
    },
    {   /* QLL_ERRCODE_INVALID_XA_STATE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "the global transaction is in progress"
    },
    {   /* QLL_ERRCODE_CANNOT_SUPPORT_INCREMENTAL_BACKUP_LEVEL */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid INCREMENTAL BACKUP LEVEL specified : %d"
    },
    {   /* QLL_ERRCODE_CANNOT_SUPPORT_INCREMENTAL_BACKUP_OPTION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "INCREMENTAL BACKUP LEVEL 0 cannot have any BACKUP options"
    },
    {   /* QLL_ERRCODE_CANNOT_RESTORE_TABLESPACE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot RESTORE TABLESPACE; tablespace is in use"
    },
    {   /* QLL_ERRCODE_INPROGRESS_TRANSACTION */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "transaction is in progress"
    },
    {   /* QLL_ERRCODE_CANNOT_USE_OUTER_JOIN_OPERATOR_WITH_ANSI_JOINS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "outer join operator (+) cannot be used with ANSI joins"
    },
    {   /* QLL_ERRCODE_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "outer join operator (+) not allowed in %s"
    },
    {   /* QLL_ERRCODE_OUTER_JOIN_CANNOT_BE_SPECIFIED_ON_CORRELATION_COLUMN */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "an outer join cannot be specified on a correlation column"
    },
    {   /* QLL_ERRCODE_PREDICATE_MAY_REFERENCED_ONLY_ONE_OUTER_JOIN_TABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "a predicate may reference only one outer-joined table"
    },
    {   /* QLL_ERRCODE_MAY_BE_OUTER_JOINED_TO_AT_MOST_ONE_OTHER_TABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "a table may be outer joined to at most one other table"
    },
    {   /* QLL_ERRCODE_CANNOT_BE_OUTER_JOINED_TO_EACH_OTHER */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "two tables cannot be outer-joined to each other"
    },
    {   /* QLL_ERRCODE_RESTART_VALUE_CANNOT_BE_MADE_TO_LESS_THAN_MINVALUE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "RESTART value cannot be made to be less than the MINVALUE"
    },
    {   /* QLL_ERRCODE_RESTART_VALUE_CANNOT_BE_MADE_TO_EXCEED_MAXVALUE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "RESTART value cannot be made to exceed the MAXVALUE"
    },
    {   /* QLL_ERRCODE_MINVALUE_CANNOT_BE_MADE_TO_EXCEED_THE_CURRENT_VALUE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "MINVALUE cannot be made to exceed the current value"
    },
    {   /* QLL_ERRCODE_MAXVALUE_CANNOT_BE_MADE_TO_LESS_THAN_THE_CURRENT_VALUE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "MAXVALUE cannot be made to be less than the current value"
    },
    {   /* QLL_ERRCODE_MINVALUE_CANNOT_BE_MADE_TO_EXCEED_THE_START_WITH_VALUE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "MINVALUE cannot be made to exceed the START WITH value"
    },
    {   /* QLL_ERRCODE_MAXVALUE_CANNOT_BE_MADE_TO_LESS_THAN_THE_START_WITH_VALUE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "MAXVALUE cannot be made to be less than START WITH value"
    },
    {   /* QLL_ERRCODE_ALREADY_USED_LOGGROUP_ID */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "log group id is already used"
    },
    {   /* QLL_ERRCODE_IDENTITY_COLUMN_CANNOT_HAVE_DEFAULT_VALUE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "identity column cannot have a default value"
    },
    {   /* QLL_ERRCODE_DEFAULT_EXPRESSION_HAS_ERRORS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "DEFAULT expression has errors"
    },
    {   /* QLL_ERRCODE_DISALLOW_STATEMENT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "the statement is disallowed"
    },
    {   /* QLL_ERRCODE_XA_OUTSIDE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "resource manager doing work outside transaction"
    },
    {   /* QLL_ERRCODE_MEMORY_TABLESPACE_DOES_NOT_SUPPORT_AUTOEXTEND */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "MEMORY tablespace does not support an AUTOEXTEND option"
    },
    {   /* QLL_ERRCODE_INVALID_ROWID */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid ROWID"
    },
    {   /* QLL_ERRCODE_NOT_ALLOW_COMMAND_IN_CDS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "unable to execute in CDS mode"
    },
    {   /* QLL_ERRCODE_SYSTEM_THREAD_CREATION_FAIL */
        STL_EXT_ERRCODE_OPERATING_SYSTEM_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "failed to create system threads (%s)"
    },
    {   /* QLL_ERRCODE_NOT_EXIST_VALID_INCREMENTAL_BACKUP */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "valid incremental backup does not exist"
    },
    {   /* QLL_ERRCODE_NOT_EXIST_LOGGROUP_ID */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "log group id does not exist"
    },
    {   /* QLL_ERRCODE_INVALID_STRING_VALUE_SPECIFIED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid string value specified"
    },
    {   /* QLL_ERRCODE_INTO_CLAUSE_MULTIPLE_ROWS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,  
        "into clause can have only one row"
    },
    {   /* QLL_ERRCODE_SHUTDOWN_MUST_DEDICATED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "SHUTDOWN must be executed in dedicated session"
    },
    {   /* QLL_ERRCODE_TRANSACTION_ROLLBACK_INTEGRITY_CONSTRAINT_VIOLATION */ 
        STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_INTEGRITY_CONSTRAINT_VIOLATION, 
        "transaction rollback: integrity constraint violation" 
    },
    {   /* QLL_ERRCODE_COLUMNAR_TABLE_INVALID_COLUMN_LENGTH */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "column length specifier is out of range( 1 to %d ) for columnar table"
    },
    {   /* QLL_ERRCODE_COLUMNAR_TABLE_NOT_SUPPORT_COLUMN_TYPE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "columnar table does not support LONGVARCHAR and LONGVARBINARY"
    },
    {   /* QLL_ERRCODE_CANNOT_DEFER_CONSTRAINT_THAT_IS_NOT_DEFERRABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot defer a constraint that is not deferrable"
    },
    {   /* QLL_ERRCODE_INVALID_NOT_NULL_CONSTRAINT_SPECIFIED_ON_IDENTITY_COLUMN */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid NOT NULL constraint specified on an identity column"
    },
    {   /* QLL_ERRCODE_CONSTRAINT_IS_NOT_DEFERRABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "constraint '%s' is not deferrable"
    },
    {   /* QLL_ERRCODE_DUPLICATE_CONSTRAINT_NAME_SPECIFIED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "duplicate constraint name specified"
    },
    {   /* QLL_ERRCODE_CANNOT_MAKE_PRIMARY_KEY_SUPPLEMENTAL_LOG_DATA_WITH_VIOLATION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot make primary key supplemental log data with primary key constraint violation"
    },
    {   /* QLL_ERRCODE_DATAFILE_NOT_EXISTS_DATABASE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "datafile does not exist in database"
    },
    {   /* QLL_ERRCODE_TOO_MANY_EXPRESSIONS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "too many expressions"
    },
    {   /* QLL_ERRCODE_USED_PUBLIC_SYNONYM_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "name '%s' is already used by an existing public synonym"
    },
    {   /* QLL_ERRCODE_PRIVATE_SYNONYM_NOT_EXIST */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "private synonym '%s' to be droppped does not exist"
    },
    {   /* QLL_ERRCODE_PUBLIC_SYNONYM_NOT_EXIST */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "public synonym '%s' to be droppped does not exist"
    },
    {   /* QLL_ERRCODE_LOOPING_CHAIN_SYNONYM_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "looping chain of synonyms"
    },
    {   /* QLL_ERRCODE_SYNONYM_TRANSLATION_IS_NO_LONGER_VALID */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "synonym translation is no longer valid"
    },
    {   /* QLL_ERRCODE_ITERATION_TIME_SCOPE_INVALID */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "expression is out of iteration time scope"
    },
    {   /* QLL_ERRCODE_CONSTANT_VALUE_EXPRESSION_EXPECTED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "constant value expression expected"
    },
    {   /* QLL_ERRCODE_INVALID_PROFILE_PARAMETER */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid or redundant resource"
    },
    {   /* QLL_ERRCODE_ACCOUNT_LOCKED */
        STL_EXT_ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION_NO_SUBCLASS,
        "the account is locked"
    },
    {   /* QLL_ERRCODE_PASSWORD_WILL_EXPIRE_N_DAYS */
        STL_EXT_ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION_NO_SUBCLASS,
        "the password will expire in %ld days"
    },
    {   /* QLL_ERRCODE_PASSWORD_WILL_EXPIRE_SOON */
        STL_EXT_ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION_NO_SUBCLASS,
        "the password will expire soon"
    },
    {   /* QLL_ERRCODE_PASSWORD_EXPIRED */
        STL_EXT_ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION_NO_SUBCLASS,
        "the password has expired"
    },
    {   /* QLL_ERRCODE_PASSWORD_CANNOT_REUSED */
        STL_EXT_ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION_NO_SUBCLASS,
        "the password cannot be reused"
    },
    {   /* QLL_ERRCODE_CANNOT_LOCK_SYS_ACCOUNT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot lock \"SYS\" account"
    },
    {   /* QLL_ERRCODE_USED_PROFILE_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "profile '%s' already exists"
    },
    {   /* QLL_ERRCODE_PASSWORD_VERIFY_FUNCTION_NOT_FOUND */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "function '%s' not found"
    },
    {   /* QLL_ERRCODE_CANNOT_DROP_PROFILE_WITHOUT_CASCADE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "profile '%s' has users assigned, cannot drop without CASCADE"
    },
    {   /* QLL_ERRCODE_PROFILE_NOT_EXISTS */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "profile '%s' does not exists"
    },
    {   /* QLL_ERRCODE_CANNOT_ASSIGN_PROFILE_SYS_ACCOUNT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot assign PROFILE to \"SYS\" account"
    },
    {   /* QLL_ERRCODE_CANNOT_ASSIGN_DEFAULT_VALUE_TO_DEFAULT_PROFILE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cannot assign DEFAULT parameter value to \"DEFAULT\" profile"
    },
    {   /* QLL_ERRCODE_PASSWORD_LENGTH */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "password length less than '%d'"
    },
    {   /* QLL_ERRCODE_PASSWORD_LETTER_COUNT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "password must contain at least '%d' letter(s)"
    },
    {   /* QLL_ERRCODE_PASSWORD_UPPER_CHARACTER_COUNT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "password must contain at least uppercase '%d' character(s)"
    },
    {   /* QLL_ERRCODE_PASSWORD_LOWER_CHARACTER_COUNT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "password must contain at least lowercase '%d' character(s)"
    },
    {   /* QLL_ERRCODE_PASSWORD_DIGIT_COUNT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "password must contain at least '%d' digit(s)"
    },
    {   /* QLL_ERRCODE_PASSWORD_SPECIAL_COUNT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "password must contain at least '%d' special character(s)"
    },
    {   /* QLL_ERRCODE_PASSWORD_SAME_OR_SIMILAR_TO_USER_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "password same as or similar to user name"
    },
    {   /* QLL_ERRCODE_PASSWORD_CONTAIN_USER_NAME_REVERSED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "password contains the user name reversed"
    },
    {   /* QLL_ERRCODE_PASSWORD_SAME_OR_SIMILAR_TO_DATABASE_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "password same as or similar to database name"
    },
    {   /* QLL_ERRCODE_PASSWORD_TOO_SIMPLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "password too simple"
    },
    {   /* QLL_ERRCODE_PASSWORD_DIFFER_FROM_OLD_PASSWORD */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "Password should differ from the old password by at least '%d' characters"
    },
    {   /* QLL_ERRCODE_CONSTANT_NOT_SUPPORT_DATA_TYPE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "constant value does not support LONGVARCHAR and LONGVARBINARY"
    },
    {   /* QLL_ERRCODE_BIND_PARAM_INCONSISTENT_DATATYPES */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "value of parameter number (%d) must be consistent datatype"
    },
    {   /* QLL_ERRCODE_BIND_PARAM_INVALID_VALUE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid value is prevented in parameter number (%d)"
    },
    {
        0,
        NULL
    }
};

/** @} qllError */
