/*******************************************************************************
 * eldtSQL_CONFORMANCE.c
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
 * @file eldtSQL_CONFORMANCE.c
 * @brief SQL_CONFORMANCE dictionary table 
 */

#include <ell.h>

#include <eldt.h>

/**
 * @addtogroup eldtSQL_CONFORMANCE
 * @internal
 * @{
 */

/**
 * @todo IS_SUPPORTED 항목을 채워야 한다. (현재 모두 STL_FALSE로 설정함)
 */
eldtSqlConformanceRecord gEldtSqlConformanceRecord[] =
{
    /*
     * PART
     */
    { "PART", "1", " ", "Framework (SQL/Framework)", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "PART", "2", " ", "Foundation (SQL/Foundation)", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "PART", "3", " ", "Call-Level Interface (SQL/CLI)", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "PART", "4", " ", "Persistent Stored Modules (SQL/PSM)", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "PART", "9", " ", "Management of External Data (SQL/MED)", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "PART", "10", " ", "Object Language Bindings (SQL/OLB)", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "PART", "11", " ", "Information and Definition Schema (SQL/Schemata)", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "PART", "13", " ", "Routines and Types Using the Java Programming Language (SQL/JRT)", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "PART", "14", " ", "XML-Related Specifications (SQL/XML)", " ", STL_FALSE, STL_FALSE, NULL, NULL },

    /*
     * PACKAGE
     */
    
    { "PACKAGE", "PKG001", " ", "Enhanced datetime facilities", " ", STL_FALSE, STL_FALSE, NULL, NULL }, /* F052, F411, F555 */
    { "PACKAGE", "PKG002", " ", "Enhanced integrity management", " ", STL_FALSE, STL_FALSE, NULL, NULL }, /* F191, F521, F701, F491, F671, T201, T211, T212, T191 */
    { "PACKAGE", "PKG004", " ", "PSM", " ", STL_FALSE, STL_FALSE, NULL, NULL }, /* T341, P001, P002, P003 */
    { "PACKAGE", "PKG006", " ", "Basic object support", " ", STL_FALSE, STL_FALSE, NULL, NULL }, /* S023, S041, S051, S151, T041 */
    { "PACKAGE", "PKG007", " ", "Enhanced object support", " ", STL_FALSE, STL_FALSE, NULL, NULL }, /* S024, S043, S071, S081, S111, S161, S211, S231, S241 */
    { "PACKAGE", "PKG008", " ", "Active database", " ", STL_FALSE, STL_FALSE, NULL, NULL }, /* T211 */
    { "PACKAGE", "PKG010", " ", "OLAP", " ", STL_FALSE, STL_FALSE, NULL, NULL }, /* T431, T611 */

    /*
     * FEATURE, SUBFEATURE
     */
    
    { "FEATURE", "B011", " ", "Embedded Ada", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B012", " ", "Embedded C", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B013", " ", "Embedded COBOL", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B014", " ", "Embedded Fortran", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B015", " ", "Embedded MUMPS", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B016", " ", "Embedded Pascal", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B017", " ", "Embedded PL/I", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B021", " ", "Direct SQL", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B031", " ", "Basic dynamic SQL", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B032", " ", "Extended dynamic SQL", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "B032", "01", "Extended dynamic SQL", "<describe input statement>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B033", " ", "Untyped SQL-invoked function arguments", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B034", " ", "Dynamic specification of cursor attributes", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B035", " ", "Non-extended descriptor names", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B041", " ", "Extensions to embedded SQL exception declarations", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B051", " ", "Enhanced execution rights", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B111", " ", "Module language Ada", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B112", " ", "Module language C", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B113", " ", "Module language COBOL", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B114", " ", "Module language Fortran", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B115", " ", "Module language MUMPS", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B116", " ", "Module language Pascal", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B117", " ", "Module language PL/I", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B121", " ", "Routine language Ada", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B122", " ", "Routine language C", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B123", " ", "Routine language COBOL", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B125", " ", "Routine language MUMPS", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B126", " ", "Routine language Pascal", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B127", " ", "Routine language PL/I", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "B128", " ", "Routine language SQL", " ", STL_FALSE, STL_FALSE, NULL, NULL },

    { "FEATURE", "C011", " ", "All facilities defined by this part of ISO/IEC 9075", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "E011", " ", "Numeric data types", " ", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E011", "01", "Numeric data types", "INTEGER and SMALLINT data types", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E011", "02", "Numeric data types", "REAL, DOUBLE PRECISON, and FLOAT data types", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E011", "03", "Numeric data types", "DECIMAL and NUMERIC data types ", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E011", "04", "Numeric data types", "Arithmetic operators", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E011", "05", "Numeric data types", "Numeric comparison", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E011", "06", "Numeric data types", "Implicit casting among the numeric data types", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "E021", " ", "Character string types", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E021", "01", "Character string types", "CHARACTER data type", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E021", "02", "Character string types", "CHARACTER VARYING data type", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E021", "03", "Character string types", "Character literals", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E021", "04", "Character string types", "CHARACTER_LENGTH function", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E021", "05", "Character string types", "OCTET_LENGTH function", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E021", "06", "Character string types", "SUBSTRING function", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E021", "07", "Character string types", "Character concatenation", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E021", "08", "Character string types", "UPPER and LOWER functions", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E021", "09", "Character string types", "TRIM function", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E021", "10", "Character string types", "Implicit casting among the fixed-length and variable-length character string types", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "E021", "11", "Character string types", "POSITION function", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E021", "12", "Character string types", "Character comparison", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "E031", " ", "Identifiers", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E031", "01", "Identifiers", "Delimited identifiers", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E031", "02", "Identifiers", "Lower case identifiers", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E031", "03", "Identifiers", "Trailing underscore", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "E051", " ", "Basic query specification", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E051", "01", "Basic query specification", "SELECT DISTINCT", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E051", "02", "Basic query specification", "GROUP BY clause", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E051", "04", "Basic query specification", "GROUP BY can contain columns not in <select list>", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E051", "05", "Basic query specification", "Select list items can be renamed", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E051", "06", "Basic query specification", "HAVING clause", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E051", "07", "Basic query specification", "Qualified * in select list", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E051", "08", "Basic query specification", "Correlation names in the FROM clause", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E051", "09", "Basic query specification", "Rename columns in the FROM clause", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "E061", " ", "Basic predicates and search conditions", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E061", "01", "Basic predicates and search conditions", "Comparison predicate", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E061", "02", "Basic predicates and search conditions", "BETWEEN predicate", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E061", "03", "Basic predicates and search conditions", "IN predicate with list of values", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E061", "04", "Basic predicates and search conditions", "LIKE predicate", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E061", "05", "Basic predicates and search conditions", "LIKE predicate: ESCAPE clause", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E061", "06", "Basic predicates and search conditions", "NULL predicate ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E061", "07", "Basic predicates and search conditions", "Quantified comparison predicate", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E061", "08", "Basic predicates and search conditions", "EXISTS predicate", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E061", "09", "Basic predicates and search conditions", "Subqueries in comparison predicate", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E061", "11", "Basic predicates and search conditions", "Subqueries in IN predicate", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E061", "12", "Basic predicates and search conditions", "Subqueries in quantified comparison predicate", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E061", "13", "Basic predicates and search conditions", "Correlated subqueries", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E061", "14", "Basic predicates and search conditions", "Search condition", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "E071", " ", "Basic query expressions", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E071", "01", "Basic query expressions", "UNION DISTINCT table operator", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E071", "02", "Basic query expressions", "UNION ALL table operator", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E071", "03", "Basic query expressions", "EXCEPT DISTINCT table operator", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E071", "05", "Basic query expressions", "Columns combined via table operators need not have exactly the same data type", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E071", "06", "Basic query expressions", "Table operators in subqueries", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "E081", " ", "Basic Privileges", " ", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E081", "01", "Basic Privileges", "SELECT privilege at the table level", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E081", "02", "Basic Privileges", "DELETE privilege", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E081", "03", "Basic Privileges", "INSERT privilege at the table level", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E081", "04", "Basic Privileges", "UPDATE privilege at the table level", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E081", "05", "Basic Privileges", "UPDATE privilege at the column level", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E081", "06", "Basic Privileges", "REFERENCES privilege at the table level", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E081", "07", "Basic Privileges", "REFERENCES privilege at the column level", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E081", "08", "Basic Privileges", "WITH GRANT OPTION", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E081", "09", "Basic Privileges", "USAGE privilege", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E081", "10", "Basic Privileges", "EXECUTE privilege", STL_FALSE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "E091", " ", "Set functions", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E091", "01", "Set functions", "AVG", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E091", "02", "Set functions", "COUNT", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E091", "03", "Set functions", "MAX", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E091", "04", "Set functions", "MIN", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E091", "05", "Set functions", "SUM", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E091", "06", "Set functions", "ALL quantifier", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E091", "07", "Set functions", "DISTINCT quantifier", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "E101", " ", "Basic data manipulation", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E101", "01", "Basic data manipulation", "INSERT statement", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E101", "03", "Basic data manipulation", "Searched UPDATE statement", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E101", "04", "Basic data manipulation", "Searched DELETE statement", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "E111", " ", "Single row SELECT statement", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "E121", " ", "Basic cursor support", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E121", "01", "Basic cursor support", "DECLARE CURSOR", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E121", "02", "Basic cursor support", "ORDER BY columns need not be in select list", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E121", "03", "Basic cursor support", "Value expressions in ORDER BY clause", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E121", "04", "Basic cursor support", "OPEN statement", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E121", "06", "Basic cursor support", "Positioned UPDATE statement", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E121", "07", "Basic cursor support", "Positioned DELETE statement", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E121", "08", "Basic cursor support", "CLOSE statement", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E121", "10", "Basic cursor support", "FETCH statement: implicit NEXT", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E121", "17", "Basic cursor support", "WITH HOLD cursors", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "E131", " ", "Null value support (nulls in lieu of values)", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "E141", " ", "Basic integrity constraints", " ", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E141", "01", "Basic integrity constraints", "NOT NULL constraints", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E141", "02", "Basic integrity constraints", "UNIQUE constraints of NOT NULL columns", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E141", "03", "Basic integrity constraints", "PRIMARY KEY constraints", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E141", "04", "Basic integrity constraints", "Basic FOREIGN KEY constraint with the NO ACTION default for both referential delete action and referential update action", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E141", "06", "Basic integrity constraints", "CHECK constraints", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E141", "07", "Basic integrity constraints", "Column defaults", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E141", "08", "Basic integrity constraints", "NOT NULL inferred on PRIMARY KEY", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E141", "10", "Basic integrity constraints", "Names in a foreign key can be specified in any order", STL_FALSE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "E151", " ", "Transaction support", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E151", "01", "Transaction support", "COMMIT statement", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E151", "02", "Transaction support", "ROLLBACK statement", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "E152", " ", "Basic SET TRANSACTION statement", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E152", "01", "Basic SET TRANSACTION statement", "SET TRANSACTION statement: ISOLATION LEVEL SERIALIZABLE clause", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "E152", "02", "Basic SET TRANSACTION statement", "SET TRANSACTION statement: READ ONLY and READ WRITE clauses", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "E153", " ", "Updatable queries with subqueries", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "FEATURE", "E161", " ", "SQL comments using leading double minus", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "FEATURE", "E171", " ", "SQLSTATE support", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "FEATURE", "E182", " ", "Host language binding", " ", STL_TRUE, STL_TRUE, NULL, NULL },

    { "FEATURE", "F021", " ", "Basic information schema", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F021", "01", "Basic information schema", "COLUMNS view", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F021", "02", "Basic information schema", "TABLES view", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F021", "03", "Basic information schema", "VIEWS view", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F021", "04", "Basic information schema", "TABLE_CONSTRAINTS view", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F021", "05", "Basic information schema", "REFERENTIAL_CONSTRAINTS view", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F021", "06", "Basic information schema", "CHECK_CONSTRAINTS view", STL_TRUE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F031", " ", "Basic schema manipulation", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F031", "01", "Basic schema manipulation", "CREATE TABLE statement to create persistent base tables", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F031", "02", "Basic schema manipulation", "CREATE VIEW statement", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F031", "03", "Basic schema manipulation", "GRANT statement", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F031", "04", "Basic schema manipulation", "ALTER TABLE statement: ADD COLUMN clause", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F031", "13", "Basic schema manipulation", "DROP TABLE statement: RESTRICT clause", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F031", "16", "Basic schema manipulation", "DROP VIEW statement: RESTRICT clause", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F031", "19", "Basic schema manipulation", "REVOKE statement: RESTRICT clause", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "F032", " ", "CASCADE drop behavior", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F033", " ", "ALTER TABLE statement: DROP COLUMN clause", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F034", " ", "Extended REVOKE statement", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F034", "01", "Extended REVOKE statement", "REVOKE statement performed by other than the owner of a schema object", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F034", "02", "Extended REVOKE statement", "REVOKE statement: GRANT OPTION FOR clause", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F034", "03", "Extended REVOKE statement", "REVOKE statement to revoke a privilege that the grantee has WITH GRANT OPTION", STL_TRUE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F041", " ", "Basic joined table", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F041", "01", "Basic joined table", "Inner join (but not necessarily the INNER keyword)", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F041", "02", "Basic joined table", "INNER keyword", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F041", "03", "Basic joined table", "LEFT OUTER JOIN", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F041", "04", "Basic joined table", "RIGHT OUTER JOIN", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F041", "05", "Basic joined table", "Outer joins can be nested", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F041", "07", "Basic joined table", "The inner table in a left or right outer join can also be used in an inner join", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F041", "08", "Basic joined table", "All comparison operators are supported (rather than just =)", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "F051", " ", "Basic date and time", " ", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F051", "01", "Basic date and time", "DATE data type (including support of DATE literal)", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F051", "02", "Basic date and time", "TIME data type (including support of TIME literal) with fractional seconds precision of at least 0", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F051", "03", "Basic date and time", "TIMESTAMP data type (including support of TIMESTAMP literal) with fractional seconds precision of at least 0 and 6", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F051", "04", "Basic date and time", "Comparison predicate on DATE, TIME, and TIMESTAMP data types", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F051", "05", "Basic date and time", "Explicit CAST between datetime types and character string types", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F051", "06", "Basic date and time", "CURRENT_DATE", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F051", "07", "Basic date and time", "LOCALTIME", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F051", "08", "Basic date and time", "LOCALTIMESTAMP", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "F052", " ", "Intervals and datetime arithmetic", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F053", " ", "OVERLAPS predicate", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F054", " ", "TIMESTAMP in DATE type precedence list", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F081", " ", "UNION and EXCEPT in views", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "F111", " ", "Isolation levels other than SERIALIZABLE", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F111", "01", "Isolation levels other than SERIALIZABLE", "READ UNCOMMITTED isolation level", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F111", "02", "Isolation levels other than SERIALIZABLE", "READ COMMITTED isolation level", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F111", "03", "Isolation levels other than SERIALIZABLE", "REPEATABLE READ isolation level", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F121", " ", "Basic diagnostics management", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F121", "01", "Basic diagnostics management", "GET DIAGNOSTICS statement", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F121", "02", "Basic diagnostics management", "SET TRANSACTION statement: DIAGNOSTICS SIZE clause", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F122", " ", "Enhanced diagnostics management", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F123", " ", "All diagnostics", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F131", " ", "Grouped operations", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F131", "01", "Grouped operations", "WHERE, GROUP BY, and HAVING clauses supported in queries with grouped views", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F131", "02", "Grouped operations", "Multiple tables supported in queries with grouped views", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F131", "03", "Grouped operations", "Set functions supported in queries with grouped views", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F131", "04", "Grouped operations", "Subqueries with GROUP BY and HAVING clauses and grouped views", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F131", "05", "Grouped operations", "Single row SELECT with GROUP BY and HAVING clauses and grouped views", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "F171", " ", "Multiple schemas per user", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F181", " ", "Multiple module support", " ", STL_FALSE, STL_TRUE, NULL, NULL },
    { "FEATURE", "F191", " ", "Referential delete actions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F200", " ", "TRUNCATE TABLE statement", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F201", " ", "CAST function", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "FEATURE", "F202", " ", "TRUNCATE TABLE: identity column restart option", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F221", " ", "Explicit defaults", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "FEATURE", "F222", " ", "INSERT statement: DEFAULT VALUES clause", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F231", " ", "Privilege tables", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F251", " ", "Domain support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F261", " ", "CASE expression", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F261", "01", "CASE expression", "Simple CASE", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F261", "02", "CASE expression", "Searched CASE", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F261", "03", "CASE expression", "NULLIF", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F261", "04", "CASE expression", "COALESCE", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "F262", " ", "Extended CASE expression", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F263", " ", "Comma-separated predicates in simple CASE expression", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F271", " ", "Compound character literals", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F281", " ", "LIKE enhancements", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F291", " ", "UNIQUE predicate", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F301", " ", "CORRESPONDING in query expressions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F302", " ", "INTERSECT table operator", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F302", "01", "INTERSECT table operator", "INTERSECT DISTINCT table operator", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F302", "02", "INTERSECT table operator", "INTERSECT ALL table operator", STL_TRUE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F304", " ", "EXCEPT ALL table operator", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F311", " ", "Schema definition statement", " ", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F311", "01", "Schema definition statement", "CREATE SCHEMA", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F311", "02", "Schema definition statement", "CREATE TABLE for persistent base tables", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F311", "03", "Schema definition statement", "CREATE VIEW", STL_TRUE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F311", "04", "Schema definition statement", "CREATE VIEW: WITH CHECK OPTION", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "F311", "05", "Schema definition statement", "GRANT statement", STL_TRUE, STL_TRUE, NULL, NULL },
    
    { "FEATURE", "F312", " ", "MERGE statement", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F313", " ", "Enhanced MERGE statement", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F314", " ", "MERGE statement with DELETE branch", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F321", " ", "User authorization", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F341", " ", "Usage tables", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F361", " ", "Subprogram support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F381", " ", "Extended schema manipulation", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F381", "01", "Extended schema manipulation", "ALTER TABLE statement: ALTER COLUMN clause", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F381", "02", "Extended schema manipulation", "ALTER TABLE statement: ADD CONSTRAINT clause", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F381", "03", "Extended schema manipulation", "ALTER TABLE statement: DROP CONSTRAINT clause", STL_TRUE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F382", " ", "Alter column data type", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F383", " ", "Set column not null clause", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F384", " ", "Drop identity property clause", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F385", " ", "Drop column generation expression clause", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F386", " ", "Set identity column generation clause", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F391", " ", "Long identifiers", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F392", " ", "Unicode escapes in identifiers", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F393", " ", "Unicode escapes in literals", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F394", " ", "Optional normal form specification", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F401", " ", "Extended joined table", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F401", "01", "Extended joined table", "NATURAL JOIN", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F401", "02", "Extended joined table", "FULL OUTER JOIN", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F401", "04", "Extended joined table", "CROSS JOIN", STL_TRUE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F402", " ", "Named column joins for LOBs, arrays, and multisets", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F403", " ", "Partitioned join tables", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F411", " ", "Time zone specification", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F421", " ", "National character", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F431", " ", "Read-only scrollable cursors", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F431", "01", "Read-only scrollable cursors", "FETCH with explicit NEXT", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F431", "02", "Read-only scrollable cursors", "FETCH FIRST", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F431", "03", "Read-only scrollable cursors", "FETCH LAST", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F431", "04", "Read-only scrollable cursors", "FETCH PRIOR", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F431", "05", "Read-only scrollable cursors", "FETCH ABSOLUTE", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F431", "06", "Read-only scrollable cursors", "FETCH RELATIVE", STL_TRUE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F441", " ", "Extended set function support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F442", " ", "Mixed column references in set functions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F451", " ", "Character set definition", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F461", " ", "Named character sets", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F471", " ", "Scalar subquery values", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "FEATURE", "F481", " ", "Expanded NULL predicate", " ", STL_TRUE, STL_TRUE, NULL, NULL },
    { "FEATURE", "F491", " ", "Constraint management", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F492", " ", "Optional table constraint enforcement", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F501", " ", "Features and conformance views", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F501", "01", "Features and conformance views", "SQL_FEATURES view", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F501", "02", "Features and conformance views", "SQL_SIZING view ", STL_TRUE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F502", " ", "Enhanced documentation tables", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F521", " ", "Assertions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F531", " ", "Temporary tables", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F555", " ", "Enhanced seconds precision", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F561", " ", "Full value expressions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F571", " ", "Truth value tests", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F591", " ", "Derived tables", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F611", " ", "Indicator data types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F641", " ", "Row and table constructors", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F651", " ", "Catalog name qualifiers", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F661", " ", "Simple tables", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F671", " ", "Subqueries in CHECK constraints", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F672", " ", "Retrospective check constraints", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F690", " ", "Collation support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F692", " ", "Enhanced collation support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F693", " ", "SQL-session and client module collations", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F695", " ", "Translation support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F696", " ", "Additional translation documentation", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F701", " ", "Referential update actions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F711", " ", "ALTER domain", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F721", " ", "Deferrable constraints", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F731", " ", "INSERT column privileges", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F741", " ", "Referential MATCH types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F751", " ", "View CHECK enhancements", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F761", " ", "Session management", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F762", " ", "CURRENT_CATALOG", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F763", " ", "CURRENT_SCHEMA", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F771", " ", "Connection management", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F781", " ", "Self-referencing operations", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F791", " ", "Insensitive cursors", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F801", " ", "Full set function", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F812", " ", "Basic flagging", " ", STL_FALSE, STL_TRUE, NULL, NULL },
    { "FEATURE", "F813", " ", "Extended flagging", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F821", " ", "Local table references", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F831", " ", "Full cursor update", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F831", "01", "Full cursor update", "Updateable scrollable cursors", STL_TRUE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "F831", "02", "Full cursor update", "Updateable ordered cursors", STL_TRUE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "F841", " ", "LIKE_REGEX predicate", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F842", " ", "OCCURRENCES_REGEX function", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F843", " ", "POSITION_REGEX function", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F844", " ", "SUBSTRING_REGEX", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F845", " ", "TRANSLATE_REGEX", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F846", " ", "Octet support in regular expression operators", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F847", " ", "Nonconstant regular expressions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F850", " ", "Top-level <order by clause> in <query expression>", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F851", " ", "<order by clause> in subqueries", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F852", " ", "Top-level <order by clause> in views", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F855", " ", "Nested <order by clause> in <query expression>", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F856", " ", "Nested <fetch first clause> in <query expression>", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F857", " ", "Top-level <fetch first clause> in <query expression>", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F858", " ", "<fetch first clause> in subqueries", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F859", " ", "Top-level <fetch first clause> in views", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F860", " ", "dynamic <fetch first row count> in <fetch first clause>", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F861", " ", "Top-level <result offset clause> in <query expression>", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F862", " ", "<result offset clause> in subqueries", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F863", " ", "Nested <result offset clause> in <query expression>", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F864", " ", "Top-level <result offset clause> in views", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "F865", " ", "dynamic <offset row count> in <result offset clause>", " ", STL_TRUE, STL_FALSE, NULL, NULL },

    { "FEATURE", "J001", " ", "Embedded Java", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J002", " ", "ResultSetIterator access to JDBC ResultSet", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J003", " ", "Execution control", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J004", " ", "Batch update", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J005", " ", "Call statement", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J006", " ", "Assignment Function statement", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J007", " ", "Compound statement", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J008", " ", "Datalinks via SQL language", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J009", " ", "Multiple Open ResultSets", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J010", " ", "XML via SQL language", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J511", " ", "Commands", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J521", " ", "JDBC data types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J531", " ", "Deployment", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J541", " ", "SERIALIZABLE", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J551", " ", "SQLDATA", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J561", " ", "JAR privileges", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J571", " ", "NEW operator", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J581", " ", "Output parameters", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J591", " ", "Overloading", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J601", " ", "SQL-Java paths", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J611", " ", "References", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J621", " ", "external Java routines", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J622", " ", "external Java types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J631", " ", "Java signatures", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J641", " ", "Static fields", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J651", " ", "SQL/JRT Information Schema", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "J652", " ", "SQL/JRT Usage tables", " ", STL_FALSE, STL_FALSE, NULL, NULL },

    { "FEATURE", "M001", " ", "Datalinks", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M002", " ", "Datalinks via SQL/CLI", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M003", " ", "Datalinks via Embedded SQL", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M004", " ", "Foreign data support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M005", " ", "Foreign schema support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M006", " ", "GetSQLString routine", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M007", " ", "TransmitRequest", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M009", " ", "GetOpts and GetStatistics routines", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M010", " ", "Foreign data wrapper support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M011", " ", "Datalinks via Ada", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M012", " ", "Datalinks via C", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M013", " ", "Datalinks via COBOL", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M014", " ", "Datalinks via Fortran", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M015", " ", "Datalinks via M", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M016", " ", "Datalinks via Pascal", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M017", " ", "Datalinks via PL/I", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M018", " ", "Foreign data wrapper interface routines in Ada", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M019", " ", "Foreign data wrapper interface routines in C", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M020", " ", "Foreign data wrapper interface routines in COBOL", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M021", " ", "Foreign data wrapper interface routines in Fortran", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M022", " ", "Foreign data wrapper interface routines in MUMPS", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M023", " ", "Foreign data wrapper interface routines in Pascal", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M024", " ", "Foreign data wrapper interface routines in PL/I", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M030", " ", "SQL-server foreign data support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "M031", " ", "Foreign data wrapper general routines", " ", STL_FALSE, STL_FALSE, NULL, NULL },

    { "FEATURE", "P001", " ", "Stored modules", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P001", "01", "Stored modules", "<SQL-server module definition>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P001", "02", "Stored modules", "<drop module statement>", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "P002", " ", "Computational completeness", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "01", "Computational completeness", "<compound statement>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "02", "Computational completeness", "<handler declaration>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "03", "Computational completeness", "<condition declaration>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "04", "Computational completeness", "<SQL variable declaration>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "05", "Computational completeness", "<assignment statement>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "06", "Computational completeness", "<case statement>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "07", "Computational completeness", "<if statement>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "08", "Computational completeness", "<iterate statement>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "09", "Computational completeness", "<leave statement>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "10", "Computational completeness", "<loop statement>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "11", "Computational completeness", "<repeat statement>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "12", "Computational completeness", "<while statement>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "13", "Computational completeness", "<for statement>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "14", "Computational completeness", "<signal statement>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "15", "Computational completeness", "<resignal statement>", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P002", "16", "Computational completeness", "<SQL control statement>s as the SQL-statement of an externally-invoked procedure", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "P003", " ", "Information Schema views", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P003", "01", "Information Schema views", "MODULES view", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P003", "02", "Information Schema views", "MODULE_TABLE_USAGE view", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P003", "03", "Information Schema views", "MODULE_COLUMN_USAGE view", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "P003", "04", "Information Schema views", "MODULE_PRIVILEGES view", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "P004", " ", "Extended CASE statement", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "P005", " ", "Qualified SQL variable references", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "P006", " ", "Multiple assignment", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "P007", " ", "Enhanced diagnostics management", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "P008", " ", "Comma-separated predicates in a CASE statement Extended CASE", " ", STL_FALSE, STL_FALSE, NULL, NULL },

    { "FEATURE", "S011", " ", "Distinct data types", " ", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "S011", "01", "Distinct data types", "USER_DEFINED_TYPES view", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "S023", " ", "Basic structured types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S024", " ", "Enhanced structured types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S025", " ", "Final structured types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S026", " ", "Self-referencing structured types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S027", " ", "Create method by specific method name", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S028", " ", "Permutable UDT options list", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S041", " ", "Basic reference types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S043", " ", "Enhanced reference types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S051", " ", "Create table of type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S071", " ", "SQL paths in function and type name resolution", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S081", " ", "Subtables", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "S091", " ", "Basic array support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "S091", "01", "Basic array support", "Arrays of built-in data types", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "S091", "02", "Basic array support", "Arrays of distinct types", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "S091", "03", "Basic array support", "Arrays expressions", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "S092", " ", "Arrays of user-defined types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S094", " ", "Arrays of reference types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S095", " ", "Array constructors by query", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S096", " ", "Optional array bounds", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S097", " ", "Array element assignment", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S098", " ", "ARRAY_AGG", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S111", " ", "ONLY in query expressions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S151", " ", "Type predicate", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S161", " ", "Subtype treatment", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S162", " ", "Subtype treatment for references", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "S201", " ", "SQL-invoked routines on arrays", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "S201", "01", "SQL-invoked routines on arrays", "Array parameters", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "S201", "02", "SQL-invoked routines on arrays", "Array as result type of functions", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "S202", " ", "SQL-invoked routines on multisets", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S211", " ", "User-defined cast functions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S231", " ", "Structured type locators", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S232", " ", "Array locators", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S233", " ", "Multiset locators", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S241", " ", "Transform functions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S242", " ", "Alter transform statement", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S251", " ", "User-defined orderings", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S261", " ", "Specific type method", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S271", " ", "Basic multiset support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S272", " ", "Multisets of user-defined types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S274", " ", "Multisets of reference types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S275", " ", "Advanced multiset support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S281", " ", "Nested collection types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S291", " ", "Unique constraint on entire row", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S301", " ", "Enhanced UNNEST", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S401", " ", "Distinct types based on array types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S402", " ", "Distinct types based on distinct types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S403", " ", "MAX_CARDINALITY", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "S404", " ", "TRIM_ARRAY", " ", STL_FALSE, STL_FALSE, NULL, NULL },

    { "FEATURE", "T011", " ", "Timestamp in Information Schema", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T021", " ", "BINARY and VARBINARY data types", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T022", " ", "Advanced support for BINARY and VARBINARY data types", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T023", " ", "Compound binary literals", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T024", " ", "Spaces in binary literals", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T031", " ", "BOOLEAN data type", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "T041", " ", "Basic LOB data type support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "T041", "01", "Basic LOB data type support", "BLOB data type", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "T041", "02", "Basic LOB data type support", "CLOB data type", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "T041", "03", "Basic LOB data type support", "POSITION, LENGTH, LOWER, TRIM, UPPER, and SUBSTRING functions for LOB data types", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "T041", "04", "Basic LOB data type support", "Concatenation of LOB data types", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "T041", "05", "Basic LOB data type support", "LOB locator: non-holdable", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "T042", " ", "Extended LOB data type support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T043", " ", "Multiplier T", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T044", " ", "Multiplier P", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T051", " ", "Row types", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T053", " ", "Explicit aliases for all-fields reference", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T061", " ", "UCS support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T071", " ", "BIGINT data type", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T101", " ", "Enhanced nullability determination", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T111", " ", "Updatable joins, unions, and columns", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T121", " ", "WITH (excluding RECURSIVE) in query expression", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T122", " ", "WITH (excluding RECURSIVE) in subquery", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T131", " ", "Recursive query", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T132", " ", "Recursive query in subquery", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T141", " ", "SIMILAR predicate", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T151", " ", "DISTINCT predicate", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T152", " ", "DISTINCT predicate with negation", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T171", " ", "LIKE clause in table definition", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T172", " ", "AS subquery clause in table definition", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T173", " ", "Extended LIKE clause in table definition", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T174", " ", "Identity columns", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T175", " ", "Generated columns", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T176", " ", "Sequence generator support", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T177", " ", "Sequence generator support: simple restart option", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T178", " ", "Identity columns: simple restart option", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T180", " ", "System-versioned tables", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T191", " ", "Referential action RESTRICT", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T201", " ", "Comparable data types for referential constraints", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "T211", " ", "Basic trigger capability", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "T211", "01", "Basic trigger capability", "Triggers activated on UPDATE, INSERT, or DELETE of one base table", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "T211", "02", "Basic trigger capability", "BEFORE triggers", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "T211", "03", "Basic trigger capability", "AFTER triggers", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "T211", "04", "Basic trigger capability", "FOR EACH ROW triggers", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "T211", "05", "Basic trigger capability", "Ability to specify a search condition that shall be True before the trigger is invoked", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "T211", "06", "Basic trigger capability", "Support for run-time rules for the interaction of triggers and constraints", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "T211", "07", "Basic trigger capability", "TRIGGER privilege", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "T211", "08", "Basic trigger capability", "Multiple triggers for the same event are executed in the order in which they were created in the catalog", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "T212", " ", "Enhanced trigger capability", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T213", " ", "INSTEAD OF triggers", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T231", " ", "Sensitive cursors", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T241", " ", "START TRANSACTION statement", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T251", " ", "SET TRANSACTION statement: LOCAL option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T261", " ", "Chained transactions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T271", " ", "Savepoints", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T272", " ", "Enhanced savepoint management", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T281", " ", "SELECT privilege with column granularity", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T285", " ", "Enhanced derived column names", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T301", " ", "Functional dependencies", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T312", " ", "OVERLAY function", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "T321", " ", "Basic SQL-invoked routines", " ", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "T321", "01", "Basic SQL-invoked routines", "User-defined functions with no overloading", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "T321", "02", "Basic SQL-invoked routines", "User-defined stored procedures with no overloading", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "T321", "03", "Basic SQL-invoked routines", "Function invocation", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "T321", "04", "Basic SQL-invoked routines", "CALL statement", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "T321", "05", "Basic SQL-invoked routines", "RETURN statement", STL_FALSE, STL_TRUE, NULL, NULL },
    { "SUBFEATURE", "T321", "06", "Basic SQL-invoked routines", "ROUTINES view", STL_FALSE, STL_FALSE, NULL, NULL },
    { "SUBFEATURE", "T321", "07", "Basic SQL-invoked routines", "PARAMETERS view", STL_FALSE, STL_FALSE, NULL, NULL },
    
    { "FEATURE", "T322", " ", "Declared data type attributes", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T323", " ", "Explicit security for external routines", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T324", " ", "Explicit security for SQL routines", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T325", " ", "Qualified SQL parameter references", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T326", " ", "Table functions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T331", " ", "Basic roles", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T332", " ", "Extended roles", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T341", " ", "Overloading of SQL-invoked functions and SQL-invoked procedures", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T351", " ", "Bracketed comments", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T431", " ", "Extended grouping capabilities", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T432", " ", "Nested and concatenated GROUPING SETS", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T433", " ", "Multiargument GROUPING function", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T434", " ", "GROUP BY DISINCT", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T441", " ", "ABS and MOD functions", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T461", " ", "Symmetric BETWEEN predicate", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T471", " ", "Result sets return value", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T491", " ", "LATERAL derived table", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T495", " ", "Combined data change and retrieval", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T501", " ", "Enhanced EXISTS predicate", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T511", " ", "Transaction counts", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T521", " ", "Named arguments in CALL statement", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T522", " ", "Default values for IN parameters of SQL-invoked procedures", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T551", " ", "Optional key words for default syntax", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T561", " ", "Holdable locators", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T571", " ", "Array-returning external SQL-invoked functions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T572", " ", "Multiset-returning external SQL-invoked functions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T581", " ", "Regular expression substring function", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T591", " ", "UNIQUE constraints of possibly null columns", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T601", " ", "Local cursor references", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T611", " ", "Elementary OLAP operations", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T612", " ", "Advanced OLAP operations", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T613", " ", "Sampling", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T614", " ", "NTILE function", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T615", " ", "LEAD and LAG functions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T616", " ", "Null treatment option for LEAD and LAG functions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T617", " ", "FIRST_VALUE and LAST_VALUE functions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T618", " ", "NTH_VALUE function", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T619", " ", "Nested window functions", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T620", " ", "WINDOW clause: GROUPS option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T621", " ", "Enhanced numeric functions", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T631", " ", "IN predicate with one list element", " ", STL_FALSE, STL_TRUE, NULL, NULL },
    { "FEATURE", "T641", " ", "Multiple column assignment", " ", STL_TRUE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T651", " ", "SQL-schema statements in SQL routines", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T652", " ", "SQL-dynamic statements in SQL routines", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T653", " ", "SQL-schema statements in external routines", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T654", " ", "SQL-dynamic statements in external routines", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "T655", " ", "Cyclically dependent routines", " ", STL_FALSE, STL_FALSE, NULL, NULL },

    { "FEATURE", "X010", " ", "XML type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X011", " ", "Arrays of XML type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X012", " ", "XMultisets of XML type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X013", " ", "Distinct types of XML type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X014", " ", "Attributes of XML type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X015", " ", "Fields of XML type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X016", " ", "Persistent XML values", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X020", " ", "XMLConcat", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X025", " ", "XMLCast", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X030", " ", "XMLDocument", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X031", " ", "XMLElement", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X032", " ", "XMLForest", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X033", " ", "XMLAgg", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X034", " ", "XMLAgg: ORDER BY option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X035", " ", "XMLComment", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X036", " ", "XMLPI", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X037", " ", "XMLText", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X038", " ", "Basic table mapping", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X040", " ", "XMLConcat", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X041", " ", "Basic table mapping: null absent", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X042", " ", "Basic table mapping: null as nil", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X043", " ", "Basic table mapping: table as forest", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X044", " ", "Basic table mapping: table as element", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X045", " ", "Basic table mapping: with target namespace", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X046", " ", "Basic table mapping: data mapping", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X047", " ", "Basic table mapping: metadata mapping", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X048", " ", "XBasic table mapping: base64 encoding of binary strings", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X049", " ", "Basic table mapping: hex encoding of binary strings", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X050", " ", "Advanced table mapping", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X051", " ", "Advanced table mapping: null absent", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X052", " ", "Advanced table mapping: null as nil", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X053", " ", "Advanced table mapping: table as forest", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X054", " ", "Advanced table mapping: table as element", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X055", " ", "Advanced table mapping: with target namespace", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X056", " ", "Advanced table mapping: data mapping", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X057", " ", "Advanced table mapping: metadata mapping", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X058", " ", "Advanced table mapping: base64 encoding of binary strings", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X059", " ", "Advanced table mapping: hex encoding of binary strings", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X060", " ", "XMLParse: Character string input and CONTENT option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X061", " ", "XMLParse: Character string input and DOCUMENT option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X065", " ", "XMLParse: BLOB input and CONTENT option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X066", " ", "XMLParse: BLOB input and DOCUMENT option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X068", " ", "XMLSerialize: BOM", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X069", " ", "XMLSerialize: INDENT", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X070", " ", "XMLSerialize: Character string serialization and CONTENT option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X071", " ", "Advanced table mapping: with target namespace", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X072", " ", "XMLSerialize: Character string serialization", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X073", " ", "XMLSerialize: BLOB serialization and CONTENT option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X074", " ", "XMLSerialize: BLOB serialization and DOCUMENT option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X075", " ", "XMLSerialize: BLOB serialization", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X076", " ", "XMLSerialize: VERSION", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X077", " ", "XMLSerialize: explicit ENCODING option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X078", " ", "XMLSerialize: explicit XML declaration", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X080", " ", "Namespaces in XML publishing", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X081", " ", "Query-level XML namespace declarations", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X082", " ", "XML namespace declarations in DML", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X083", " ", "XML namespace declarations in DDL", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X084", " ", "XML namespace declarations in compound statements", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X085", " ", "Predefined namespace prefixes", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X086", " ", "XML namespace declarations in XMLTable", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X090", " ", "XML document predicate", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X091", " ", "XML content predicate", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X096", " ", "XMLExists", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X100", " ", "Host language support for XML: CONTENT option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X101", " ", "Host language support for XML: DOCUMENT option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X110", " ", "Host language support for XML: VARCHAR mapping", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X111", " ", "Host language support for XML: CLOB mapping", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X112", " ", "Host language support for XML: BLOB mapping", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X113", " ", "Host language support for XML: STRIP WHITESPACE option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X114", " ", "Host language support for XML: PRESERVE WHITESPACE option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X120", " ", "XML parameters in SQL routines", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X121", " ", "XML parameters in external routines", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X131", " ", "Query-level XMLBINARY clause", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X132", " ", "XMLBINARY clause in DML", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X133", " ", "XMLBINARY clause in DDL", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X134", " ", "XMLBINARY clause in compound statements", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X135", " ", "XMLBINARY clause in subqueries", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X141", " ", "IS VALID predicate: data-driven case", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X142", " ", "IS VALID predicate: ACCORDING TO clause", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X143", " ", "IS VALID predicate: ELEMENT clause", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X144", " ", "IS VALID predicate: schema location", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X145", " ", "IS VALID predicate outside check constraints", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X151", " ", "IS VALID predicate with DOCUMENT option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X152", " ", "IS VALID predicate with CONTENT option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X153", " ", "IS VALID predicate with SEQUENCE option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X155", " ", "IS VALID predicate: NAMESPACE without ELEMENT clause", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X157", " ", "IS VALID predicate: NO NAMESPACE with ELEMENT clause", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X160", " ", "Basic Information Schema for registered XML Schemas", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X161", " ", "Advanced Information Schema for registered XML Schemas", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X170", " ", "XML null handling options", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X171", " ", "NIL ON NO CONTENT option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X181", " ", "XML(DOCUMENT(UNTYPED)) type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X182", " ", "XML(DOCUMENT(ANY)) type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X190", " ", "XML(SEQUENCE) type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X191", " ", "XML(DOCUMENT(XMLSCHEMA)) type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X192", " ", "XML(CONTENT(XMLSCHEMA)) type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X200", " ", "XMLQuery", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X201", " ", "XMLQuery: RETURNING CONTENT", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X202", " ", "XMLQuery: RETURNING SEQUENCE", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X203", " ", "XMLQuery: passing a context item", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X204", " ", "XMLQuery: initializing an XQuery variable", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X205", " ", "XMLQuery: EMPTY ON EMPTY option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X206", " ", "XMLQuery: NULL ON EMPTY option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X211", " ", "XML 1.1 support", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X221", " ", "XML passing mechanism BY VALUE", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X222", " ", "XML passing mechanism BY REF", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X231", " ", "XML(CONTENT(UNTYPED)) type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X232", " ", "XML(CONTENT(ANY)) type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X241", " ", "RETURNING CONTENT in XML publishing", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X242", " ", "RETURNING SEQUENCE in XML publishing", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X251", " ", "Persistent XML values of XML(DOCUMENT(UNTYPED)) type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X252", " ", "Persistent XML values of XML(DOCUMENT(ANY)) type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X253", " ", "Persistent XML values of XML(CONTENT(UNTYPED)) type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X254", " ", "Persistent XML values of XML(CONTENT(ANY)) type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X255", " ", "Persistent XML values of XML(SEQUENCE) type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X256", " ", "Persistent XML values of XML(DOCUMENT(XMLSCHEMA)) type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X257", " ", "Persistent XML values of XML(CONTENT(XMLSCHEMA)) type", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X260", " ", "XML type: ELEMENT clause", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X261", " ", "XML type: NAMESPACE without ELEMENT clause", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X263", " ", "XML type: NO NAMESPACE with ELEMENT clause", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X264", " ", "XML type: schema location", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X271", " ", "XMLValidate: data-driven case", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X272", " ", "XMLValidate: ACCORDING TO clause", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X273", " ", "XMLValidate: ELEMENT clause", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X274", " ", "XMLValidate: schema location", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X281", " ", "XMLValidate: with DOCUMENT option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X282", " ", "XMLValidate with CONTENT option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X283", " ", "XMLValidate with SEQUENCE option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X284", " ", "XMLValidate NAMESPACE without ELEMENT clause", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X286", " ", "XMLValidate: NO NAMESPACE with ELEMENT clause", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X300", " ", "XMLTable", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X301", " ", "XMLTable: derived column list option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X302", " ", "XMLTable: ordinality column option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X303", " ", "XMLTable: column default option", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X304", " ", "XMLTable: passing a context item", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X305", " ", "XMLTable: initializing an XQuery variable", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X400", " ", "Name and identifier mapping", " ", STL_FALSE, STL_FALSE, NULL, NULL },
    { "FEATURE", "X410", " ", "Alter column data type: XML type", " ", STL_FALSE, STL_FALSE, NULL, NULL },

    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        STL_FALSE,
        STL_FALSE,
        NULL,
        NULL
    }
};

/**
 * @brief DEFINITION_SCHEMA.SQL_CONFORMANCE 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescSQL_CONFORMANCE[ELDT_SqlConformance_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_SQL_CONFORMANCE,        /**< Table ID */
        "TYPE",                               /**< 컬럼의 이름  */
        ELDT_SqlConformance_ColumnOrder_TYPE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,               /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,            /**< 컬럼의 Nullable 여부 */
        "SQL part, package, feature and subfeature defined in ISO/IEC 9075"
    },
    {
        ELDT_TABLE_ID_SQL_CONFORMANCE,      /**< Table ID */
        "ID",                               /**< 컬럼의 이름  */
        ELDT_SqlConformance_ColumnOrder_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "identifier string of the conformance element"        
    },
    {
        ELDT_TABLE_ID_SQL_CONFORMANCE,          /**< Table ID */
        "SUB_ID",                               /**< 컬럼의 이름  */
        ELDT_SqlConformance_ColumnOrder_SUB_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "identifier string of the subfeature, or a single space if not a subfeature"
    },
    {
        ELDT_TABLE_ID_SQL_CONFORMANCE,        /**< Table ID */
        "NAME",                               /**< 컬럼의 이름  */
        ELDT_SqlConformance_ColumnOrder_NAME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,        /**< 컬럼의 Nullable 여부 */
        "descriptive name of the conformance element"
    },
    {
        ELDT_TABLE_ID_SQL_CONFORMANCE,            /**< Table ID */
        "SUB_NAME",                               /**< 컬럼의 이름  */
        ELDT_SqlConformance_ColumnOrder_SUB_NAME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,            /**< 컬럼의 Nullable 여부 */
        "descriptive name of the subfeature, or a single space if not a subfeature"
    },
    {
        ELDT_TABLE_ID_SQL_CONFORMANCE,                /**< Table ID */
        "IS_SUPPORTED",                               /**< 컬럼의 이름  */
        ELDT_SqlConformance_ColumnOrder_IS_SUPPORTED, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                /**< 컬럼의 Nullable 여부 */
        "YES if an SQL-implementation fully supports that conformance element described "
        "when SQL-data in the identified catalog is accessed through that implementation, NO if not"
    },
    {
        ELDT_TABLE_ID_SQL_CONFORMANCE,                /**< Table ID */
        "IS_MANDATORY",                               /**< 컬럼의 이름  */
        ELDT_SqlConformance_ColumnOrder_IS_MANDATORY, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                /**< 컬럼의 Nullable 여부 */
        "YES if SQL Conformance mandatory feature "
    },
    {
        ELDT_TABLE_ID_SQL_CONFORMANCE,                  /**< Table ID */
        "IS_VERIFIED_BY",                               /**< 컬럼의 이름  */
        ELDT_SqlConformance_ColumnOrder_IS_VERIFIED_BY, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                          /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                              /**< 컬럼의 Nullable 여부 */
        "If full support for the conformance element described has been verified by testing, "
        "then the IS_VERIFIED_BY column shall contain information identifying "
        "the conformance test used to verify the conformance claim; "
        "otherwise, IS_VERIFIED_BY shall be the null value"
    },
    {
        ELDT_TABLE_ID_SQL_CONFORMANCE,            /**< Table ID */
        "COMMENTS",                               /**< 컬럼의 이름  */
        ELDT_SqlConformance_ColumnOrder_COMMENTS, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                        /**< 컬럼의 Nullable 여부 */
        "possibly a comment pertaining to the conformance element"
    }
};


/**
 * @brief DEFINITION_SCHEMA.SQL_CONFORMANCE 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescSQL_CONFORMANCE =
{
    ELDT_TABLE_ID_SQL_CONFORMANCE, /**< Table ID */
    "SQL_CONFORMANCE",             /**< 테이블의 이름  */
    "The SQL_CONFORMANCE base table has one row for each conformance element "
    "(part, package, feature, and subfeature) identified by ISO/IEC 9075."
};

/**
 * @brief DEFINITION_SCHEMA.SQL_CONFORMANCE 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescSQL_CONFORMANCE[ELDT_SqlConformance_Const_MAX] =
{
    {
        ELDT_TABLE_ID_SQL_CONFORMANCE,         /**< Table ID */
        ELDT_SqlConformance_Const_PRIMARY_KEY, /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY, /**< Table Constraint 유형 */
        3,                                     /**< 키를 구성하는 컬럼의 개수 */
        {                                  /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_SqlConformance_ColumnOrder_TYPE,
            ELDT_SqlConformance_ColumnOrder_ID,
            ELDT_SqlConformance_ColumnOrder_SUB_ID,
        },
        ELL_DICT_OBJECT_ID_NA,                  /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                  /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};


/**
 * @brief SQL-implementation information item 명세를 추가한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aStmt              Statement
 * @param[in]  aEnv               Execution Library Environment
 * @remarks
 */
stlStatus eldtInsertSqlConformanceDesc( smlTransId     aTransID,
                                        smlStatement * aStmt,
                                        ellEnv       * aEnv )
{
    eldtSqlConformanceRecord * sRecord = NULL;
    stlInt32                   i = -1;

    while( STL_TRUE )
    {
        i++;

        sRecord = &gEldtSqlConformanceRecord[i];
        
        if( sRecord->mType == NULL )
        {
            break;
        }

        STL_TRY( ellInsertSqlConformanceDesc( aTransID,
                                              aStmt,
                                              sRecord->mType,
                                              sRecord->mID,
                                              sRecord->mSubID,
                                              sRecord->mName,
                                              sRecord->mSubName,
                                              sRecord->mIsSupported,
                                              sRecord->mIsMandatory,
                                              sRecord->mIsVerifiedBy,
                                              sRecord->mComments,
                                              aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} eldtSQL_CONFORMANCE */
