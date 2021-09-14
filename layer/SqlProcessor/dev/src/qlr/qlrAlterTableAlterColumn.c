/*******************************************************************************
 * qlrAlterTableAlterColumn.c
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
 * @file qlrAlterTableAlterColumn.c
 * @brief ALTER TABLE .. ALTER COLUMN .. 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlrAlterTableAlterColumn ALTER TABLE .. ALTER COLUMN
 * @ingroup qlrAlterTable
 * @{
 */


/**
 * @brief SET DATA TYPE Compatibility Matrix
 */
const stlBool qlrSetDataTypeCompatMatrix[DTL_TYPE_MAX][DTL_TYPE_MAX] =
{
    /*
     * DTL_TYPE_BOOLEAN
     */
    {
        STL_TRUE,  /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_NATIVE_SMALLINT
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_TRUE,  /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_NATIVE_INTEGER
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_TRUE,  /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_NATIVE_BIGINT
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_TRUE,  /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_NATIVE_REAL
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_TRUE,  /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_NATIVE_DOUBLE
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_TRUE,  /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_FLOAT
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_TRUE,  /* DTL_TYPE_FLOAT */        
        STL_TRUE,  /* DTL_TYPE_NUMBER */
        STL_TRUE,  /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },   
    /*
     * DTL_TYPE_NUMBER
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_TRUE,  /* DTL_TYPE_FLOAT */        
        STL_TRUE,  /* DTL_TYPE_NUMBER */
        STL_TRUE,  /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_DECIMAL
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_TRUE,  /* DTL_TYPE_FLOAT */        
        STL_TRUE,  /* DTL_TYPE_NUMBER */
        STL_TRUE,  /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_CHAR
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR : 실시간 DDL이 아닌 경우 지원하여야 함 */ 
        STL_FALSE, /* DTL_TYPE_VARCHAR : 실시간 DDL이 아닌 경우 지원하여야 함 */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_VARCHAR
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR : 실시간 DDL이 아닌 경우 지원하여야 함 */
        STL_TRUE,  /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_LONGVARCHAR
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_TRUE,  /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_CLOB
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_TRUE,  /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_BINARY
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY : 실시간 DDL이 아닌 경우 지원하여야 함 */
        STL_FALSE, /* DTL_TYPE_VARBINARY : 실시간 DDL이 아닌 경우 지원하여야 함 */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_VARBINARY
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY : 실시간 DDL이 아닌 경우 지원하여야 함 */
        STL_TRUE,  /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_LONGVARBINARY
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_TRUE,  /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_BLOB
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_TRUE,  /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_DATE
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_TRUE,  /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_TIME
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_TRUE,  /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_TIMESTAMP
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_TRUE,  /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_TIME_WITH_TIME_ZONE
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_TRUE,  /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_TRUE,  /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_INTERVAL_YEAR_TO_MONTH
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_TRUE,  /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_INTERVAL_DAY_TO_SECOND
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_TRUE,  /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE  /* DTL_TYPE_ROWID */
    },
    /*
     * DTL_TYPE_ROWID
     */
    {
        STL_FALSE, /* DTL_TYPE_BOOLEAN */
        STL_FALSE, /* DTL_TYPE_NATIVE_SMALLINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_INTEGER */
        STL_FALSE, /* DTL_TYPE_NATIVE_BIGINT */
        STL_FALSE, /* DTL_TYPE_NATIVE_REAL */
        STL_FALSE, /* DTL_TYPE_NATIVE_DOUBLE */
        STL_FALSE, /* DTL_TYPE_FLOAT */        
        STL_FALSE, /* DTL_TYPE_NUMBER */
        STL_FALSE, /* DTL_TYPE_DECIMAL */
        STL_FALSE, /* DTL_TYPE_CHAR */
        STL_FALSE, /* DTL_TYPE_VARCHAR */
        STL_FALSE, /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE, /* DTL_TYPE_CLOB */
        STL_FALSE, /* DTL_TYPE_BINARY */
        STL_FALSE, /* DTL_TYPE_VARBINARY */
        STL_FALSE, /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE, /* DTL_TYPE_BLOB */
        STL_FALSE, /* DTL_TYPE_DATE */
        STL_FALSE, /* DTL_TYPE_TIME */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP */
        STL_FALSE, /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_TRUE   /* DTL_TYPE_ROWID */
    }
};




/**
 * @brief ALTER COLUMN 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateAlterColumn( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitAlterColumn       * sInitPlan = NULL;
    qlpAlterTableAlterColumn * sParseTree = NULL;

    stlBool         sObjectExist = STL_FALSE;
    void          * sTableHandle = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_DEFAULT_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_DEFAULT_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_NOT_NULL_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_NOT_NULL_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_DATA_TYPE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_ALTER_IDENTITY_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_IDENTITY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseTree->mType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_DEFAULT_TYPE) ||
                        (aParseTree->mType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_DEFAULT_TYPE) ||
                        (aParseTree->mType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_NOT_NULL_TYPE) ||
                        (aParseTree->mType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_NOT_NULL_TYPE) ||
                        (aParseTree->mType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_DATA_TYPE_TYPE) ||
                        (aParseTree->mType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_ALTER_IDENTITY_TYPE) ||
                        (aParseTree->mType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_IDENTITY_TYPE),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sParseTree = (qlpAlterTableAlterColumn *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitAlterColumn),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitAlterColumn) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * Table Name Validation
     **********************************************************/
    
    STL_TRY( qlrGetAlterTableHandle( aTransID,
                                     aDBCStmt,
                                     aSQLStmt,
                                     aSQLString,
                                     sAuthHandle,
                                     sParseTree->mTableName,
                                     & sInitPlan->mSchemaHandle,
                                     & sInitPlan->mTableHandle,
                                     aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Column Name Validation
     **********************************************************/

    STL_TRY( ellGetColumnDictHandle( aTransID,
                                     aSQLStmt->mViewSCN,
                                     & sInitPlan->mTableHandle,
                                     QLP_GET_PTR_VALUE( sParseTree->mColumnName ),
                                     & sInitPlan->mColumnHandle,
                                     & sObjectExist,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_COLUMN_NOT_EXIST );

    /**********************************************************
     * Columnar Table Validation
     **********************************************************/

    sTableHandle = ellGetTableHandle( & sInitPlan->mTableHandle );

    STL_TRY_THROW( smlIsAlterableTable( sTableHandle ) == STL_TRUE,
                   RAMP_ERR_NOT_IMPLEMENTED );

    /**********************************************************
     * Alter Column Action Validation
     **********************************************************/

    sInitPlan->mAlterAction = sParseTree->mAlterColumnAction;

    switch ( sInitPlan->mAlterAction )
    {
        case QLP_ALTER_COLUMN_SET_DEFAULT:
            STL_TRY( qlrValidateSetDefault( aTransID,
                                            aDBCStmt,
                                            aSQLStmt,
                                            aSQLString,
                                            sParseTree,
                                            sInitPlan,
                                            aEnv )
                     == STL_SUCCESS );
            break;
        case QLP_ALTER_COLUMN_DROP_DEFAULT:
            STL_TRY( qlrValidateDropDefault( aTransID,
                                             aSQLString,
                                             sParseTree,
                                             sInitPlan,
                                             aEnv )
                     == STL_SUCCESS );
            break;
        case QLP_ALTER_COLUMN_SET_NOT_NULL:
            STL_TRY( qlrValidateSetNotNull( aTransID,
                                            aDBCStmt,
                                            aSQLStmt,
                                            aSQLString,
                                            sInitPlan,
                                            sParseTree->mNotNullName,
                                            sParseTree->mNotNullConstAttr,
                                            aEnv )
                     == STL_SUCCESS );
            break;
        case QLP_ALTER_COLUMN_DROP_NOT_NULL:
            STL_TRY( qlrValidateDropNotNull( aTransID,
                                             aDBCStmt,
                                             aSQLStmt,
                                             sInitPlan,
                                             aEnv )
                     == STL_SUCCESS );
            break;
        case QLP_ALTER_COLUMN_SET_DATA_TYPE:
            STL_TRY( qlrValidateSetDataType( aTransID,
                                             aDBCStmt,
                                             aSQLStmt,
                                             aSQLString,
                                             sParseTree,
                                             sInitPlan,
                                             aEnv )
                     == STL_SUCCESS );
            break;
        case QLP_ALTER_COLUMN_ALTER_IDENTITY_SPEC:
            STL_TRY( qlrValidateAlterIdentity( aTransID,
                                               aDBCStmt,
                                               aSQLStmt,
                                               aSQLString,
                                               sParseTree,
                                               sInitPlan,
                                               aEnv )
                     == STL_SUCCESS );
            break;
        case QLP_ALTER_COLUMN_DROP_IDENTITY:
            STL_TRY( qlrValidateDropIdentity( aTransID,
                                              aDBCStmt,
                                              aSQLStmt,
                                              sInitPlan,
                                              aEnv )
                     == STL_SUCCESS );
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mTableHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /***********************************************************
     * Init Plan 연결 
     ***********************************************************/
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_IDENTIFIER,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mColumnName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE(sParseTree->mColumnName) );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mAlterActionPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "ALTER TABLE ALTER COLUMN" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief ALTER COLUMN .. SET DEFAULT 구문을 Validation 한다.
 * @param[in] aTransID     Transaction ID
 * @param[in] aDBCStmt     DBC Statement
 * @param[in] aSQLStmt     SQL Statement
 * @param[in] aSQLString   SQL String
 * @param[in] aParseTree   Parse Tree
 * @param[in] aInitPlan    ALTER COLUMN Init Plan
 * @param[in] aEnv         Environment
 * @remarks
 */
stlStatus qlrValidateSetDefault( smlTransId                 aTransID,
                                 qllDBCStmt               * aDBCStmt,
                                 qllStatement             * aSQLStmt,
                                 stlChar                  * aSQLString,
                                 qlpAlterTableAlterColumn * aParseTree,
                                 qlrInitAlterColumn       * aInitPlan,
                                 qllEnv                   * aEnv )
{
    qlpColDefSecond  * sSecondDef = NULL;
    qlpValueExpr     * sValueExpr = NULL;
    
    qlvInitExpression * sInitExpr = NULL;

    qlvStmtInfo            sStmtInfo;
    qlvInitStmtExprList  * sStmtExprList  = NULL;
    qlvInitExprList      * sQueryExprList = NULL;
    
    ellObjectList * sObjList = NULL;
    
    stlInt32  sDefaultLen = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mDefault != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sSecondDef = (qlpColDefSecond *) aParseTree->mDefault;
    sValueExpr = (qlpValueExpr *) sSecondDef->mRawDefault;

    /***************************************************
     * Expression List 초기화 
     ***************************************************/

    /**
     * Statement 단위 Expression List 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitStmtExprList ),
                                (void **) & sStmtExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mPseudoColExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mConstExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    sStmtExprList->mHasSubQuery = STL_FALSE;

    /**
     * Query 단위 Expression List 설정
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitExprList ),
                                (void **) & sQueryExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mPseudoColExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mAggrExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mNestedAggrExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    sQueryExprList->mStmtExprList = sStmtExprList;

    /**
     * Statement Information
     */
    
    stlMemset( & sStmtInfo, 0x00, STL_SIZEOF( qlvStmtInfo ) );
    
    sStmtInfo.mTransID = aTransID;
    sStmtInfo.mDBCStmt = aDBCStmt;
    sStmtInfo.mSQLStmt = aSQLStmt;
    sStmtInfo.mSQLString = aSQLString;
    sStmtInfo.mQueryExprList = sQueryExprList;
    
    /***************************************************
     * Column 이 Identity Column 인지 검사 
     ***************************************************/

    STL_TRY_THROW( ellGetColumnIdentityHandle( & aInitPlan->mColumnHandle ) == NULL,
                   RAMP_ERR_COLUMN_IS_IDENTITY );

    /***************************************************
     * DEFAULT 구문 분석
     ***************************************************/

    if ( sValueExpr->mExpr->mType == QLL_BNF_NULL_CONSTANT_TYPE )
    {
        /**
         * DEFAULT NULL 임
         */

        aInitPlan->mColumnDefault = NULL;
    }
    else
    {
        /**
         * Default Expression String 을 저장
         */
        
        sDefaultLen = sValueExpr->mNodeLen
            + ( sValueExpr->mNodePos - sSecondDef->mNodePos );
        
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    sDefaultLen + 1,
                                    (void **) & aInitPlan->mColumnDefault,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemcpy( aInitPlan->mColumnDefault,
                   aSQLString + sSecondDef->mNodePos,
                   sDefaultLen );
        aInitPlan->mColumnDefault[sDefaultLen] = '\0';

        /**
         * Default Expression 을 Validation
         */
        
        STL_TRY( ellInitObjectList( & sObjList, QLL_INIT_PLAN( aDBCStmt ), ELL_ENV(aEnv) ) == STL_SUCCESS );
        
        STL_TRY( qlvValidateValueExpr( & sStmtInfo,
                                       QLV_EXPR_PHRASE_DEFAULT,
                                       sObjList,
                                       NULL, /* aRefTableList */
                                       STL_FALSE,  /* aIsAtomicInsert */
                                       STL_FALSE,  /* Row Expr */
                                       0, /* Allowed Aggregation Depth */
                                       (qllNode *) sValueExpr,
                                       & sInitExpr,
                                       NULL, /* aHasOuterJoinOperator */
                                       KNL_BUILTIN_FUNC_INVALID,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       QLL_INIT_PLAN( aDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );
        
        /**
         * 호환 가능하지 않은 타입이거나 공간이 부족한 경우,
         * INSERT 나 UPDATE 구문에서 DEFAULT 사용시 에러가 발생한다.
         */
    }
    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_IS_IDENTITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_IDENTITY_COLUMN_CANNOT_HAVE_DEFAULT_VALUE,
                      qlgMakeErrPosString( aSQLString,
                                           sSecondDef->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief ALTER COLUMN .. DROP DEFAULT 구문을 Validation 한다.
 * @param[in] aTransID     Transaction ID
 * @param[in] aSQLString   SQL String
 * @param[in] aParseTree   Parse Tree
 * @param[in] aInitPlan    ALTER COLUMN Init Plan
 * @param[in] aEnv         Environment
 * @remarks
 */
stlStatus qlrValidateDropDefault( smlTransId                 aTransID,
                                  stlChar                  * aSQLString,
                                  qlpAlterTableAlterColumn * aParseTree,
                                  qlrInitAlterColumn       * aInitPlan,
                                  qllEnv                   * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************
     * Column 이 Identity Column 인지 검사 
     ***************************************************/

    STL_TRY_THROW( ellGetColumnIdentityHandle( & aInitPlan->mColumnHandle ) == NULL,
                   RAMP_ERR_COLUMN_IS_IDENTITY );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_IS_IDENTITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_IDENTITY_COLUMN_CANNOT_HAVE_DEFAULT_VALUE,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mAlterActionPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ALTER COLUMN .. SET NOT NULL 구문을 Validation 한다.
 * @param[in] aTransID            Transaction ID
 * @param[in] aDBCStmt            DBC Statement
 * @param[in] aSQLStmt            SQL Statement
 * @param[in] aSQLString          SQL String
 * @param[in] aInitPlan           ALTER COLUMN Init Plan
 * @param[in] aNotNullName        Not Null Constraint Name
 * @param[in] aNotNullConstAttr   Not Null Constraint Attribute (nullable)
 * @param[in] aEnv                Environment
 * @remarks
 */
stlStatus qlrValidateSetNotNull( smlTransId                 aTransID,
                                 qllDBCStmt               * aDBCStmt,
                                 qllStatement             * aSQLStmt,
                                 stlChar                  * aSQLString,
                                 qlrInitAlterColumn       * aInitPlan,
                                 qlpObjectName            * aNotNullName,
                                 qlpConstraintAttr        * aNotNullConstAttr,
                                 qllEnv                   * aEnv )
{
    stlInt32        i = 0;
    
    stlInt32        sNotNullCnt = 0;
    ellDictHandle * sNotNullHandleArray = NULL;
    ellDictHandle * sColumnHandle = NULL;

    ellDictHandle   sConstHandle;
    stlChar         sNameBuffer[STL_MAX_SQL_IDENTIFIER_LENGTH + 1] = {0,};
    stlBool         sAutoGenName = STL_FALSE;
    stlBool         sObjectExist = STL_FALSE;

    stlInt64  sBigintValue = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************
     * Column 에 NOT NULL Constraint 가 존재하는 지 검사
     ***************************************************/

    /**
     * Check Not Null Constraint 정보 획득
     */

    STL_TRY( ellGetTableCheckNotNullDictHandle( aTransID,
                                                aSQLStmt->mViewSCN,
                                                QLL_INIT_PLAN( aDBCStmt ),
                                                & aInitPlan->mTableHandle,
                                                & sNotNullCnt,
                                                & sNotNullHandleArray,
                                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Column 에 NOT NULL Constraint 가 존재하는 지 검사
     */
    
    for( i = 0 ; i < sNotNullCnt ; i++ )
    {
        sColumnHandle = ellGetCheckNotNullColumnHandle( & sNotNullHandleArray[i] );

        STL_TRY_THROW( ellGetColumnID( sColumnHandle ) != ellGetColumnID( & aInitPlan->mColumnHandle ),
                       RAMP_ERR_COLUMN_HAS_NOT_NULL );
    }

    /***************************************************
     * Not Null Constraint Name Validation
     ***************************************************/

    if ( aNotNullName == NULL )
    {
        /**
         * 이름을 정의하지 않은 경우
         */
        
        sAutoGenName = STL_TRUE;

        /**
         * Table 과 동일한 Schema 로 생성
         */
        
        stlMemcpy( & aInitPlan->mNotNullSchemaHandle,
                   & aInitPlan->mSchemaHandle,
                   STL_SIZEOF( ellDictHandle ) );

        /**
         * Constraint Name 을 자동으로 생성
         */

        ellMakeCheckNotNullName( sNameBuffer,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                                 ellGetTableName( & aInitPlan->mTableHandle ),
                                 ellGetColumnName( & aInitPlan->mColumnHandle ) );

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    stlStrlen( sNameBuffer ) + 1,
                                    (void **) & aInitPlan->mNotNullConstName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( aInitPlan->mNotNullConstName, sNameBuffer );
    }
    else
    {
        /**
         * 이름을 정의한 경우 
         */
        
        sAutoGenName = STL_FALSE;

        /**
         * Constraint 의 Schema Hanlde 결정
         */
        
        if ( aNotNullName->mSchema != NULL )
        {
            STL_TRY( ellGetSchemaDictHandle( aTransID,
                                             aSQLStmt->mViewSCN,
                                             aNotNullName->mSchema,
                                             & aInitPlan->mNotNullSchemaHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * 존재하지 않는 Schema 인지 검사
             */

            STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXISTS );
            
            /**
             * Wriable Schema 인지 검사
             */
            
            STL_TRY_THROW( ellGetSchemaWritable( & aInitPlan->mNotNullSchemaHandle )
                           == STL_TRUE, RAMP_ERR_SCHEMA_NOT_WRITABLE );
        }
        else
        {
            /**
             * Table 과 동일한 Schema 를 가져감
             */
            
            stlMemcpy( & aInitPlan->mNotNullSchemaHandle,
                       & aInitPlan->mSchemaHandle,
                       STL_SIZEOF( ellDictHandle ) );
        }

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    stlStrlen( aNotNullName->mObject ) + 1,
                                    (void **) & aInitPlan->mNotNullConstName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( aInitPlan->mNotNullConstName, aNotNullName->mObject );
    }

    /**
     * Constraint Schema 에 대해 ADD CONSTRAINT ON SCHEMA 권한 검사 
     */

    STL_TRY( qlaCheckSchemaPriv( aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 ELL_SCHEMA_PRIV_ACTION_ADD_CONSTRAINT,
                                 & aInitPlan->mNotNullSchemaHandle,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Constraint Schema 에 동일한 Constraint Name 이 존재하는 지 검사
     */

    STL_TRY( ellGetConstraintDictHandleWithSchema( aTransID,
                                                   aSQLStmt->mViewSCN,
                                                   & aInitPlan->mNotNullSchemaHandle,
                                                   aInitPlan->mNotNullConstName,
                                                   & sConstHandle,
                                                   & sObjectExist,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sObjectExist == STL_TRUE )
    {
        if ( sAutoGenName == STL_TRUE )
        {
            STL_THROW( RAMP_ERR_AUTO_NAME_EXISTS );
        }
        else
        {
            STL_THROW( RAMP_ERR_NAME_EXISTS );
        }
    }
    else
    {
        /* 존재하지 않는 Constraint Name 임. */
    }
    
    /***************************************************
     * Not Null Constraint Attribute
     ***************************************************/

    if ( aNotNullConstAttr == NULL )
    {
        aInitPlan->mNotNullDeferrable   = ELL_TABLE_CONSTRAINT_DEFERRABLE_DEFAULT;
        aInitPlan->mNotNullInitDeferred = ELL_TABLE_CONSTRAINT_INITIALLY_DEFERRED_DEFAULT;
        aInitPlan->mNotNullEnforced     = ELL_TABLE_CONSTRAINT_ENFORCED_DEFAULT;
    }
    else
    {
        /**
         * mNotNullInitDeferred
         */

        if ( aNotNullConstAttr->mInitDeferred != NULL )
        {
            sBigintValue = QLP_GET_INT_VALUE( aNotNullConstAttr->mInitDeferred );
            aInitPlan->mNotNullInitDeferred = (stlBool) sBigintValue;
        }
        else
        {
            aInitPlan->mNotNullInitDeferred = ELL_TABLE_CONSTRAINT_INITIALLY_DEFERRED_DEFAULT;
        }

        /**
         * mNotNullDeferrable
         */
        
        if ( aNotNullConstAttr->mDeferrable != NULL )
        {
            sBigintValue = QLP_GET_INT_VALUE( aNotNullConstAttr->mDeferrable );
            aInitPlan->mNotNullDeferrable = (stlBool) sBigintValue;

            if ( (aInitPlan->mNotNullDeferrable == STL_FALSE) && (aInitPlan->mNotNullInitDeferred == STL_TRUE) )
            {
                STL_THROW( RAMP_ERR_CANNOT_DEFER_NOT_DEFERRABLE );
            }
            else
            {
                /* ok */
            }
        }
        else
        {
            /**
             * Deferrable 이 명시되지 않은 경우 INIT DEFERRED/IMMEDIATE 값을 따른다
             */
            if ( aInitPlan->mNotNullInitDeferred == STL_TRUE )
            {
                aInitPlan->mNotNullDeferrable = STL_TRUE;
            }
            else
            {
                aInitPlan->mNotNullDeferrable = STL_FALSE;
            }
        }

        /**
         * mEnforced
         */

        if ( aNotNullConstAttr->mEnforce != NULL )
        {
            sBigintValue = QLP_GET_INT_VALUE( aNotNullConstAttr->mEnforce );
            aInitPlan->mNotNullEnforced = (stlBool) sBigintValue;
            
            /**
             * @todo ENFORCED / NOT ENFORCED constraint 구현해야 함
             */

            STL_TRY_THROW( aInitPlan->mNotNullEnforced == STL_TRUE, RAMP_ERR_NOT_IMPLEMENTED );
        }
        else
        {
            aInitPlan->mNotNullEnforced = ELL_TABLE_CONSTRAINT_ENFORCED_DEFAULT;
        }
        
    }

    /***************************************************
     * identity column 의 NOT NULL 제약조건은
     * NOT DEFERRABLE INITIALLY IMMEDIATE ENFORCED 이어야 한다.
     ***************************************************/

    if ( ellGetColumnIdentityHandle( & aInitPlan->mColumnHandle ) == NULL )
    {
        /* identity column 이 아님 */
    }
    else
    {
        STL_TRY_THROW( (aInitPlan->mNotNullDeferrable == STL_FALSE) &&
                       (aInitPlan->mNotNullInitDeferred == STL_FALSE) &&
                       (aInitPlan->mNotNullEnforced == STL_TRUE),
                       RAMP_ERR_INDENTITY_NOT_NULL_INVALID_CHARACTERISTICS );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_HAS_NOT_NULL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_ALREADY_HAS_NOT_NULL_CONSTRAINT,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aNotNullName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      aNotNullName->mSchema );
    }
    
    STL_CATCH( RAMP_ERR_SCHEMA_NOT_WRITABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_WRITABLE,
                      qlgMakeErrPosString( aSQLString,
                                           aNotNullName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      aNotNullName->mSchema );
    }
    
    STL_CATCH( RAMP_ERR_AUTO_NAME_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_AUTO_CONSTRAINT_NAME_EXISTS,
                      NULL,
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_CATCH( RAMP_ERR_NAME_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CONSTRAINT_NAME_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           aNotNullName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlrValidateSetNotNull()" );
    }

    STL_CATCH( RAMP_ERR_CANNOT_DEFER_NOT_DEFERRABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_DEFER_CONSTRAINT_THAT_IS_NOT_DEFERRABLE,
                      qlgMakeErrPosString( aSQLString,
                                           aNotNullConstAttr->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INDENTITY_NOT_NULL_INVALID_CHARACTERISTICS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_NOT_NULL_CONSTRAINT_SPECIFIED_ON_IDENTITY_COLUMN,
                      qlgMakeErrPosString( aSQLString,
                                           aNotNullConstAttr->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief ALTER COLUMN .. DROP NOT NULL 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aInitPlan  ALTER COLUMN Init Plan
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateDropNotNull( smlTransId                 aTransID,
                                  qllDBCStmt               * aDBCStmt,
                                  qllStatement             * aSQLStmt,
                                  qlrInitAlterColumn       * aInitPlan,
                                  qllEnv                   * aEnv )
{
    stlInt32        i = 0;
    
    stlInt32        sNotNullCnt = 0;
    ellDictHandle * sNotNullHandleArray = NULL;
    ellDictHandle * sColumnHandle = NULL;

    stlInt32        sPrimaryKeyCnt = 0;
    ellDictHandle * sPrimaryKeyHandleArray = NULL;
    
    stlInt32        sKeyColumnCnt = 0;
    ellDictHandle * sKeyColumnHandle = NULL;
    
    stlBool  sFound = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    
    /***************************************************
     * Column 에 NOT NULL Constraint 가 존재하는 지 검사
     ***************************************************/

    /**
     * Check Not Null Constraint 정보 획득
     */

    STL_TRY( ellGetTableCheckNotNullDictHandle( aTransID,
                                                aSQLStmt->mViewSCN,
                                                QLL_INIT_PLAN( aDBCStmt ),
                                                & aInitPlan->mTableHandle,
                                                & sNotNullCnt,
                                                & sNotNullHandleArray,
                                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Column 에 NOT NULL Constraint 가 존재하는 지 검사
     */

    sFound = STL_FALSE;
    for( i = 0 ; i < sNotNullCnt ; i++ )
    {
        sColumnHandle = ellGetCheckNotNullColumnHandle( & sNotNullHandleArray[i] );

        if( ellGetColumnID( sColumnHandle ) == ellGetColumnID( & aInitPlan->mColumnHandle ) )
        {
            /**
             * 컬럼에 대한 NOT NULL Constraint 임
             */

            stlMemcpy( & aInitPlan->mNotNullHandle, & sNotNullHandleArray[i], STL_SIZEOF(ellDictHandle) );
            sFound = STL_TRUE;
            break;
        }
        else
        {
            continue;
        }
    }

    STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_COLUMN_HAS_NO_NOT_NULL );

    /***************************************************
     * Column 이 Primary Key 에 포함되는지 검사 
     ***************************************************/

    aInitPlan->mIsNullable = STL_TRUE;
    
    /**
     * Primary Key Constraint 정보 획득
     */
    
    STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                              aSQLStmt->mViewSCN,
                                              QLL_INIT_PLAN( aDBCStmt ),
                                              & aInitPlan->mTableHandle,
                                              & sPrimaryKeyCnt,
                                              & sPrimaryKeyHandleArray,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Primary Key 에 포함된 컬럼인지 검사 
     */
    
    if ( sPrimaryKeyCnt == 0 )
    {
        /**
         * nothing to do
         */
    }
    else
    {
        sKeyColumnCnt = ellGetPrimaryKeyColumnCount( & sPrimaryKeyHandleArray[0] );
        sKeyColumnHandle = ellGetPrimaryKeyColumnHandle( & sPrimaryKeyHandleArray[0] );

        sFound = STL_FALSE;
        for ( i = 0; i < sKeyColumnCnt; i++ )
        {
            if( ellGetColumnID( sKeyColumnHandle ) == ellGetColumnID( & aInitPlan->mColumnHandle ) )
            {
                /**
                 * Primary Key 에 포함된 컬럼임
                 */
                aInitPlan->mIsNullable = STL_FALSE;
                break;
            }
            else
            {
                continue;
            }
        }
    }

    /***************************************************
     * Column 이 Identity Column 인지 검사 
     ***************************************************/
    
    if ( aInitPlan->mIsNullable == STL_FALSE )
    {
        /**
         * nothing to do
         */
    }
    else
    {
        if ( ellGetColumnIdentityHandle( & aInitPlan->mColumnHandle ) != NULL )
        {
            aInitPlan->mIsNullable = STL_FALSE;
        }
        else
        {
            aInitPlan->mIsNullable = STL_TRUE;
        }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_HAS_NO_NOT_NULL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_DOES_NOT_HAVE_NOT_NULL_CONSTRAINT,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief ALTER COLUMN .. SET DATA TYPE 구문을 Validation 한다.
 * @param[in] aTransID     Transaction ID
 * @param[in] aDBCStmt     DBC Statement
 * @param[in] aSQLStmt     SQL Statement
 * @param[in] aSQLString   SQL String
 * @param[in] aParseTree   Parse Tree
 * @param[in] aInitPlan    ALTER COLUMN Init Plan
 * @param[in] aEnv         Environment
 * @remarks
 */
stlStatus qlrValidateSetDataType( smlTransId                 aTransID,
                                  qllDBCStmt               * aDBCStmt,
                                  qllStatement             * aSQLStmt,
                                  stlChar                  * aSQLString,
                                  qlpAlterTableAlterColumn * aParseTree,
                                  qlrInitAlterColumn       * aInitPlan,
                                  qllEnv                   * aEnv )
{
    qlvStmtInfo    sStmtInfo;
    qlpTypeName  * sNewDataType = NULL;

    dtlDataType    sSrcType = DTL_TYPE_MAX;
    stlInt64       sSrcArg1 = DTL_PRECISION_NA;
    stlInt64       sSrcArg2 = DTL_SCALE_NA;
    dtlStringLengthUnit  sSrcCharUnit = DTL_STRING_LENGTH_UNIT_NA;
    dtlIntervalIndicator sSrcIndicator = DTL_INTERVAL_INDICATOR_NA;
    
    dtlDataType    sDstType = DTL_TYPE_MAX;
    stlInt64       sDstArg1 = DTL_PRECISION_NA;
    stlInt64       sDstArg2 = DTL_SCALE_NA;
    dtlStringLengthUnit  sDstCharUnit = DTL_STRING_LENGTH_UNIT_NA;
    dtlIntervalIndicator sDstIndicator = DTL_INTERVAL_INDICATOR_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mDataType != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    sNewDataType = (qlpTypeName *) aParseTree->mDataType,
    
    /************************************************
     * New Type 정보 설정 
     ************************************************/

    /**
     * mDBType
     */

    aInitPlan->mNewDataType.mDBType = sNewDataType->mDBType;

    /**
     * mUserTypeName
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                sNewDataType->mNodeLen + 1,
                                (void **) & aInitPlan->mNewDataType.mUserTypeName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemcpy( aInitPlan->mNewDataType.mUserTypeName,
               aSQLString + sNewDataType->mNodePos,
               sNewDataType->mNodeLen );
    aInitPlan->mNewDataType.mUserTypeName[sNewDataType->mNodeLen] = '\0';

    /**
     * Type 정보 설정
     * - mArgNum1
     * - mArgNum2
     * - mStringLengthUnit
     * - mIntervalIndicator
     */

    stlMemset( & sStmtInfo, 0x00, STL_SIZEOF(qlvStmtInfo) );
    sStmtInfo.mTransID       = aTransID;
    sStmtInfo.mDBCStmt       = aDBCStmt;
    sStmtInfo.mSQLStmt       = aSQLStmt;
    sStmtInfo.mSQLString     = aSQLString;
    sStmtInfo.mQueryExprList = NULL;
    
    STL_TRY( qlvSetColumnType( & sStmtInfo,
                               & aInitPlan->mNewDataType,
                               sNewDataType,
                               aEnv )
             == STL_SUCCESS );
    

    /************************************************
     * SET DATA TYPE 호환성 검사
     ************************************************/
    
    /**
     * 호환 가능한 Type 인지 검사
     */

    sSrcType = ellGetColumnDBType( & aInitPlan->mColumnHandle );
    sDstType = aInitPlan->mNewDataType.mDBType;

    STL_TRY_THROW( qlrSetDataTypeCompatMatrix[sSrcType][sDstType] == STL_TRUE,
                   RAMP_ERR_INCOMPATIBLE_TYPE );

    /**
     * Type 유형별 속성(precision, scale, length, ...) 검사
     */

    switch (sDstType)
    {
        case DTL_TYPE_BOOLEAN:
            {
                /**
                 * nothing to do
                 */
                
                STL_ASSERT( sSrcType == DTL_TYPE_BOOLEAN );
                break;
            }
        case DTL_TYPE_NATIVE_SMALLINT:
            {
                /**
                 * nothing to do
                 */
                
                STL_ASSERT( sSrcType == DTL_TYPE_NATIVE_SMALLINT );
                break;
            }
        case DTL_TYPE_NATIVE_INTEGER:
            {
                /**
                 * nothing to do
                 */
                
                STL_ASSERT( sSrcType == DTL_TYPE_NATIVE_INTEGER );
                break;
            }
        case DTL_TYPE_NATIVE_BIGINT:
            {
                /**
                 * nothing to do
                 */
                
                STL_ASSERT( sSrcType == DTL_TYPE_NATIVE_BIGINT );
                break;
            }
        case DTL_TYPE_NATIVE_REAL:
            {
                /**
                 * nothing to do
                 */
                
                STL_ASSERT( sSrcType == DTL_TYPE_NATIVE_REAL );
                break;
            }
        case DTL_TYPE_NATIVE_DOUBLE:
            {
                /**
                 * nothing to do
                 */
                
                STL_ASSERT( sSrcType == DTL_TYPE_NATIVE_DOUBLE );
                break;
            }
        case DTL_TYPE_FLOAT:
            {
                /**
                 * Source Type 의 Precision, Scale 정보 획득 (10진 Radix 기준)
                 */
                sSrcArg1  = ellGetColumnPrecision( & aInitPlan->mColumnHandle );

                if ( gDataTypeDefinition[ sSrcType ].mDefaultNumericPrecRadix ==
                     DTL_NUMERIC_PREC_RADIX_BINARY )
                {
                    /**
                     * 2 진 Radix 경우
                     */

                    sSrcArg1 = dtlBinaryToDecimalPrecision[ sSrcArg1 ];
                }
                else
                {
                    /**
                     * 10 진 Radix 경우
                     */

                    /* Do Nothing */
                }

                /**
                 * Destination Type 의 Precision, Scale 정보 획득 (10진 Radix 기준)
                 */
                sDstArg1 = dtlBinaryToDecimalPrecision[ aInitPlan->mNewDataType.mArgNum1 ];
                    
                /**
                 * 다음 조건을 만족해야 함.
                 * - precision(dst) >= precision(src)
                 */

                STL_TRY_THROW( sDstArg1 >= sSrcArg1, RAMP_ERR_INCOMPATIBLE_PREC_SCALE );
                
                break;
            }
        case DTL_TYPE_NUMBER:
            {
                /**
                 * Source Type 의 Precision, Scale 정보 획득 (10진 Radix 기준)
                 */
                sSrcArg1  = ellGetColumnPrecision( & aInitPlan->mColumnHandle );
                sSrcArg2  = ellGetColumnScale( & aInitPlan->mColumnHandle );

                if ( gDataTypeDefinition[ sSrcType ].mDefaultNumericPrecRadix ==
                     DTL_NUMERIC_PREC_RADIX_BINARY )
                {
                    /**
                     * 2 진 Radix 경우
                     */

                    sSrcArg1 = dtlBinaryToDecimalPrecision[ sSrcArg1 ];
                }
                else
                {
                    /**
                     * 10 진 Radix 경우
                     */

                    /* Do Nothing */
                }

                /**
                 * Destination Type 의 Precision, Scale 정보 획득
                 * Parsing 을 통해 얻는 Type 정보는 2 진 또는 10 진일 수 있다.
                 */
                sDstArg1 = aInitPlan->mNewDataType.mArgNum1;
                sDstArg2 = aInitPlan->mNewDataType.mArgNum2;

                /**
                 * Floating => Exact Numeric 으로 변환할 수 없음
                 */

                if( sSrcArg2 == DTL_SCALE_NA )
                {
                    STL_TRY_THROW( sDstArg2 == DTL_SCALE_NA, RAMP_ERR_INCOMPATIBLE_PREC_SCALE );
                }
                else
                {
                    /* go go */
                }
                    
                /**
                 * 다음 조건을 만족해야 함.
                 * - precision(dst) >= precision(src)
                 * - scale(dst) >= scale(src)
                 * - precision(dst) - scale(dst) >= precision(src) - scale(src)
                 */

                if ( sDstArg2 == DTL_SCALE_NA )
                {
                    /**
                     * Scale 이 없는 경우
                     */
                    STL_TRY_THROW( sDstArg1 >= sSrcArg1, RAMP_ERR_INCOMPATIBLE_PREC_SCALE );
                }
                else
                {
                    /**
                     * Scale 이 있는 경우
                     */
                    STL_TRY_THROW( sDstArg1 >= sSrcArg1, RAMP_ERR_INCOMPATIBLE_PREC_SCALE );
                    STL_TRY_THROW( sDstArg2 >= sSrcArg2, RAMP_ERR_INCOMPATIBLE_PREC_SCALE );
                    STL_TRY_THROW( (sDstArg1 - sDstArg2) >= (sSrcArg1 - sSrcArg2),
                                   RAMP_ERR_INCOMPATIBLE_PREC_SCALE );
                }
                
                break;
            }
        case DTL_TYPE_DECIMAL:
            {
                STL_THROW(RAMP_ERR_NOT_IMPLEMENTED);
                break;
            }
        case DTL_TYPE_CHAR:
            {
                /**
                 * Source Type 의 length, char length unit 정보 획득
                 */

                sSrcArg1 = ellGetColumnPrecision( & aInitPlan->mColumnHandle );
                sSrcCharUnit = ellGetColumnStringLengthUnit( & aInitPlan->mColumnHandle );

                /**
                 * Destination Type 의 length, char length unit 정보 획득
                 */

                sDstArg1     = aInitPlan->mNewDataType.mArgNum1;
                sDstCharUnit = aInitPlan->mNewDataType.mStringLengthUnit;
                
                if ( sDstCharUnit == DTL_STRING_LENGTH_UNIT_NA )
                {
                    sDstCharUnit = ellGetDbCharLengthUnit();
                }
                else
                {
                    /* nothing to do */
                }

                /**
                 * length(dst) >= length(src)
                 */

                STL_TRY_THROW( sDstArg1 >= sSrcArg1, RAMP_ERR_INCOMPATIBLE_STRING_LENGTH );
                
                /**
                 * character set unit
                 * (X) CHARACTERS => OCTETS
                 */

                if ( sSrcCharUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS )
                {
                    STL_TRY_THROW( sDstCharUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                   RAMP_ERR_INCOMPATIBLE_CHAR_UNIT );
                }
                else
                {
                    /* go go */
                }

                /**
                 * @todo SET DATA TYPE CHAR(n) 구현해야 함.
                 */
                STL_THROW(RAMP_ERR_NOT_IMPLEMENTED);
                
                break;
            }
        case DTL_TYPE_VARCHAR:
        case DTL_TYPE_LONGVARCHAR:
            {
                /**
                 * Source Type 의 length, char length unit 정보 획득
                 */

                sSrcArg1 = ellGetColumnPrecision( & aInitPlan->mColumnHandle );
                sSrcCharUnit = ellGetColumnStringLengthUnit( & aInitPlan->mColumnHandle );

                /**
                 * Destination Type 의 length, char length unit 정보 획득
                 */

                sDstArg1     = aInitPlan->mNewDataType.mArgNum1;
                sDstCharUnit = aInitPlan->mNewDataType.mStringLengthUnit;
                
                if ( sDstCharUnit == DTL_STRING_LENGTH_UNIT_NA )
                {
                    sDstCharUnit = ellGetDbCharLengthUnit();
                }
                else
                {
                    /* nothing to do */
                }

                /**
                 * length(dst) >= length(src)
                 */

                STL_TRY_THROW( sDstArg1 >= sSrcArg1, RAMP_ERR_INCOMPATIBLE_STRING_LENGTH );
                
                /**
                 * character set unit
                 * (X) CHARACTERS => OCTETS
                 */

                if ( sSrcCharUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS )
                {
                    STL_TRY_THROW( sDstCharUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                   RAMP_ERR_INCOMPATIBLE_CHAR_UNIT );
                }
                else
                {
                    /* go go */
                }
                
                break;
            }
        case DTL_TYPE_CLOB:
            {
                /**
                 * nothing to do
                 */
                
                STL_ASSERT( sSrcType == DTL_TYPE_CLOB );
                break;
            }
        case DTL_TYPE_BINARY:
            {
                /**
                 * Source Type 의 length 정보 획득
                 */

                sSrcArg1 = ellGetColumnPrecision( & aInitPlan->mColumnHandle );

                /**
                 * Destination Type 의 length 정보 획득
                 */

                sDstArg1     = aInitPlan->mNewDataType.mArgNum1;
                
                /**
                 * length(dst) >= length(src)
                 */

                STL_TRY_THROW( sDstArg1 >= sSrcArg1, RAMP_ERR_INCOMPATIBLE_STRING_LENGTH );
                
                /**
                 * @todo SET DATA TYPE BINARY(n) 구현해야 함.
                 */
                
                STL_THROW(RAMP_ERR_NOT_IMPLEMENTED);
                
                break;
            }
        case DTL_TYPE_VARBINARY:
        case DTL_TYPE_LONGVARBINARY:
            {
                /**
                 * Source Type 의 length 정보 획득
                 */

                sSrcArg1 = ellGetColumnPrecision( & aInitPlan->mColumnHandle );

                /**
                 * Destination Type 의 length 정보 획득
                 */

                sDstArg1 = aInitPlan->mNewDataType.mArgNum1;
                
                /**
                 * length(dst) >= length(src)
                 */

                STL_TRY_THROW( sDstArg1 >= sSrcArg1, RAMP_ERR_INCOMPATIBLE_STRING_LENGTH );
                
                break;
            }
        case DTL_TYPE_BLOB:
            {
                /**
                 * nothing to do
                 */
                
                STL_ASSERT( sSrcType == DTL_TYPE_BLOB );
                break;
            }
        case DTL_TYPE_DATE:
            {
                /**
                 * nothing to do
                 */
                
                STL_ASSERT( sSrcType == DTL_TYPE_DATE );
                break;
            }
        case DTL_TYPE_TIME:
        case DTL_TYPE_TIMESTAMP:
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                STL_ASSERT( sSrcType == sDstType );
                
                /**
                 * Source Type 의 fractional second 정보 획득
                 */

                sSrcArg1 = ellGetColumnPrecision( & aInitPlan->mColumnHandle );

                /**
                 * Destination Type 의 fractional second 정보 획득
                 */

                sDstArg1 = aInitPlan->mNewDataType.mArgNum1;

                if ( sDstArg1 == DTL_PRECISION_NA )
                {
                    sDstArg1 = gDataTypeDefinition[sDstType].mDefFractionalSecondPrec;
                }
                else
                {
                    /* nothing to do */
                }

                /**
                 * fractional_second(dst) >= fractional_second(src)
                 */

                STL_TRY_THROW( sDstArg1 >= sSrcArg1, RAMP_ERR_INCOMPATIBLE_PREC_SCALE );
                
                break;
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            {
                STL_ASSERT( sSrcType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH );

                /**
                 * Source Type 의 indicator, leading precision 정보 획득
                 */

                sSrcIndicator = ellGetColumnIntervalIndicator( & aInitPlan->mColumnHandle );
                sSrcArg1 = ellGetColumnPrecision( & aInitPlan->mColumnHandle );

                /**
                 * Destination Type 의 indicator, leading precision 정보 획득
                 */

                sDstIndicator = aInitPlan->mNewDataType.mIntervalIndicator;
                sDstArg1 = aInitPlan->mNewDataType.mArgNum1;

                if ( sDstArg1 == DTL_PRECISION_NA )
                {
                    sDstArg1 = gDataTypeDefinition[sDstType].mDefIntervalPrec;
                }
                else
                {
                    /* nothing to do */
                }
                
                /**
                 * same IntervalIndicator
                 * leading_precision(dst) >= leading_precision(src)
                 */

                STL_TRY_THROW( sSrcIndicator == sDstIndicator, RAMP_ERR_INCOMPATIBLE_INTERVAL_INDICATOR );

                STL_TRY_THROW( sDstArg1 >= sSrcArg1, RAMP_ERR_INCOMPATIBLE_PREC_SCALE );
                
                break;
            }
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                STL_ASSERT( sSrcType == DTL_TYPE_INTERVAL_DAY_TO_SECOND );

                /**
                 * Source Type 의 indicator, leading precision, fractional second 정보 획득
                 */

                sSrcIndicator = ellGetColumnIntervalIndicator( & aInitPlan->mColumnHandle );
                sSrcArg1 = ellGetColumnPrecision( & aInitPlan->mColumnHandle );
                sSrcArg2 = ellGetColumnScale( & aInitPlan->mColumnHandle );

                /**
                 * Destination Type 의 indicator, leading precision, fractional second 정보 획득
                 */

                sDstIndicator = aInitPlan->mNewDataType.mIntervalIndicator;
                sDstArg1 = aInitPlan->mNewDataType.mArgNum1;
                sDstArg2 = aInitPlan->mNewDataType.mArgNum2;

                if ( sDstArg1 == DTL_PRECISION_NA )
                {
                    sDstArg1 = gDataTypeDefinition[sDstType].mDefIntervalPrec;
                }
                else
                {
                    /* nothing to do */
                }

                if ( sDstArg2 == DTL_PRECISION_NA )
                {
                    sDstArg2 = gDataTypeDefinition[sDstType].mDefFractionalSecondPrec;
                }
                else
                {
                    /* nothing to do */
                }
                
                /**
                 * Same Interval Indicator
                 */

                STL_TRY_THROW( sSrcIndicator == sDstIndicator, RAMP_ERR_INCOMPATIBLE_INTERVAL_INDICATOR );

                /**
                 * Indicator 에 따른 precision 검사
                 */
                
                switch ( sDstIndicator )
                {
                    case DTL_INTERVAL_INDICATOR_DAY:
                    case DTL_INTERVAL_INDICATOR_HOUR:
                    case DTL_INTERVAL_INDICATOR_MINUTE:
                    case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR:
                    case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE:
                    case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:
                        {
                            /**
                             * leading_precision(dst) >= leading_precision(src)
                             */
                            
                            STL_TRY_THROW( sDstArg1 >= sSrcArg1, RAMP_ERR_INCOMPATIBLE_PREC_SCALE );
                            break;
                        }
                    case DTL_INTERVAL_INDICATOR_SECOND:
                    case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND:
                    case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:
                    case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
                        {
                            /**
                             * leading_precision(dst) >= leading_precision(src)
                             * fractional_second(dst) >= fractional_second(src)
                             */
                            
                            STL_TRY_THROW( sDstArg1 >= sSrcArg1, RAMP_ERR_INCOMPATIBLE_PREC_SCALE );
                            STL_TRY_THROW( sDstArg2 >= sSrcArg2, RAMP_ERR_INCOMPATIBLE_PREC_SCALE );
                            break;
                        }
                        break;
                    default:
                        STL_ASSERT(0);
                        break;
                }
                    
                break;
            }
        case DTL_TYPE_ROWID:
            {
                /**
                 * nothing to do
                 */
                
                STL_ASSERT( sSrcType == DTL_TYPE_ROWID );
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INCOMPATIBLE_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_CANNOT_CAST_IMCOMPATIBLE_TYPE,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mColumnName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INCOMPATIBLE_PREC_SCALE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_CANNOT_CAST_DECREASE_PRECISION_OR_SCALE,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mColumnName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INCOMPATIBLE_STRING_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_CANNOT_CAST_DECREASE_STRING_LENGTH,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mColumnName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INCOMPATIBLE_CHAR_UNIT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_CANNOT_CAST_INCOMPATIBLE_CHAR_LENGTH_UNIT,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mColumnName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INCOMPATIBLE_INTERVAL_INDICATOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_CANNOT_CAST_INCOMPATIBLE_INTERVAL_INDICATOR,
                      qlgMakeErrPosString( aSQLString,
                                           aParseTree->mColumnName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlrValidateSetDataType()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ALTER COLUMN .. Alter Identity 구문을 Validation 한다.
 * @param[in] aTransID     Transaction ID
 * @param[in] aDBCStmt     DBC Statement
 * @param[in] aSQLStmt     SQL Statement
 * @param[in] aSQLString   SQL String
 * @param[in] aParseTree   Parse Tree
 * @param[in] aInitPlan    ALTER COLUMN Init Plan
 * @param[in] aEnv         Environment
 * @remarks
 */
stlStatus qlrValidateAlterIdentity( smlTransId                 aTransID,
                                    qllDBCStmt               * aDBCStmt,
                                    qllStatement             * aSQLStmt,
                                    stlChar                  * aSQLString,
                                    qlpAlterTableAlterColumn * aParseTree,
                                    qlrInitAlterColumn       * aInitPlan,
                                    qllEnv                   * aEnv )
{
    void * sIdentityHandle = NULL;
    qlpAlterIdentity * sIdentitySpec = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mIdentitySpec != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sIdentitySpec = (qlpAlterIdentity *) aParseTree->mIdentitySpec;
    
    /***************************************************
     * Column 이 Identity Column 인지 검사 
     ***************************************************/

    sIdentityHandle = ellGetColumnIdentityHandle( & aInitPlan->mColumnHandle );
    
    STL_TRY_THROW( sIdentityHandle != NULL, RAMP_ERR_COLUMN_IS_NOT_IDENTITY );

    /**********************************************************
     * Identity Generation 정보 설정
     **********************************************************/

    switch ( sIdentitySpec->mIdentType )
    {
        case QLP_IDENTITY_INVALID:
            /**
             * 기존 Option 을 유지
             */
            aInitPlan->mIdentityGenOption = ellGetColumnIdentityGenOption( & aInitPlan->mColumnHandle );
            break;
        case QLP_IDENTITY_ALWAYS:
            aInitPlan->mIdentityGenOption = ELL_IDENTITY_GENERATION_ALWAYS;
            break;
        case QLP_IDENTITY_BY_DEFAULT:
            aInitPlan->mIdentityGenOption = ELL_IDENTITY_GENERATION_BY_DEFAULT;
            break;
        default:
            STL_ASSERT(0);
            break;
    }
    
    /**********************************************************
     * Identity Attribute 정보 설정
     **********************************************************/

    if ( sIdentitySpec->mIdentSeqOptions == NULL )
    {
        /**
         * 기존 Sequence 의 Attribute 정보를 획득
         * - 모든 Sequence Attribute 는 유효한 값이다.
         */

        aInitPlan->mModifySeqAttr = STL_FALSE;
        STL_TRY( smlGetSequenceAttr( sIdentityHandle,
                                     & aInitPlan->mIdentitySeqAttr,
                                     SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        aInitPlan->mModifySeqAttr = STL_TRUE;
        STL_TRY( qlqValidateAlterSeqOption( aSQLString,
                                            sIdentityHandle,
                                            & aInitPlan->mIdentitySeqAttr,
                                            sIdentitySpec->mIdentSeqOptions,
                                            aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_IS_NOT_IDENTITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_NOT_IDENTITY_COLUMN,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
    
}
                                     

/**
 * @brief ALTER COLUMN .. DROP IDENTITY 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aInitPlan  ALTER COLUMN Init Plan
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateDropIdentity( smlTransId                 aTransID,
                                   qllDBCStmt               * aDBCStmt,
                                   qllStatement             * aSQLStmt,
                                   qlrInitAlterColumn       * aInitPlan,
                                   qllEnv                   * aEnv )
{
    stlInt32        sNotNullCnt = 0;
    ellDictHandle * sNotNullHandleArray = NULL;
    ellDictHandle * sColumnHandle = NULL;

    stlInt32        sPrimaryKeyCnt = 0;
    ellDictHandle * sPrimaryKeyHandleArray = NULL;
    
    stlInt32        sKeyColumnCnt = 0;
    ellDictHandle * sKeyColumnHandle = NULL;

    stlInt32 i = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    
    /***************************************************
     * Column 이 Identity Column 인지 검사 
     ***************************************************/

    STL_TRY_THROW( ellGetColumnIdentityHandle( & aInitPlan->mColumnHandle ) != NULL,
                   RAMP_ERR_COLUMN_IS_NOT_IDENTITY );

    /***************************************************
     * Column 에 NOT NULL Constraint 가 존재하는 지 검사
     ***************************************************/

    /**
     * Check Not Null Constraint 정보 획득
     */

    STL_TRY( ellGetTableCheckNotNullDictHandle( aTransID,
                                                aSQLStmt->mViewSCN,
                                                QLL_INIT_PLAN( aDBCStmt ),
                                                & aInitPlan->mTableHandle,
                                                & sNotNullCnt,
                                                & sNotNullHandleArray,
                                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Column 에 NOT NULL Constraint 가 존재하는 지 검사
     */

    aInitPlan->mIsNullable = STL_TRUE;
    
    for( i = 0 ; i < sNotNullCnt ; i++ )
    {
        sColumnHandle = ellGetCheckNotNullColumnHandle( & sNotNullHandleArray[i] );

        if( ellGetColumnID( sColumnHandle ) == ellGetColumnID( & aInitPlan->mColumnHandle ) )
        {
            aInitPlan->mIsNullable = STL_FALSE;
            break;
        }
        else
        {
            continue;
        }
    }

    /***************************************************
     * Column 이 Primary Key 에 포함되는지 검사 
     ***************************************************/

    if ( aInitPlan->mIsNullable == STL_FALSE )
    {
        /**
         * Identity 속성 제거시 NOT NULL 속성을 제거하면 안됨
         */
    }
    else
    {
        /**
         * Primary Key Constraint 정보 획득
         */
        
        STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  QLL_INIT_PLAN( aDBCStmt ),
                                                  & aInitPlan->mTableHandle,
                                                  & sPrimaryKeyCnt,
                                                  & sPrimaryKeyHandleArray,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /**
         * Primary Key 에 포함된 컬럼인지 검사 
         */
        
        if ( sPrimaryKeyCnt == 0 )
        {
            /**
             * nothing to do
             */
        }
        else
        {
            sKeyColumnCnt = ellGetPrimaryKeyColumnCount( & sPrimaryKeyHandleArray[0] );
            sKeyColumnHandle = ellGetPrimaryKeyColumnHandle( & sPrimaryKeyHandleArray[0] );
            
            for ( i = 0; i < sKeyColumnCnt; i++ )
            {
                if( ellGetColumnID( sKeyColumnHandle ) == ellGetColumnID( & aInitPlan->mColumnHandle ) )
                {
                    /**
                     * Primary Key 에 포함된 컬럼임
                     */
                    
                    aInitPlan->mIsNullable = STL_FALSE;
                    break;
                }
                else
                {
                    continue;
                }
            }
        }

    }
    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_IS_NOT_IDENTITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_NOT_IDENTITY_COLUMN,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ALTER COLUMN 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeAlterColumn( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv )
{
    /**
     * nothing to do
     */

    return STL_SUCCESS;
}

/**
 * @brief ALTER COLUMN 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataAlterColumn( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv )
{
    /**
     * nothing to do
     */
    
    return STL_SUCCESS;
}


/**
 * @brief ALTER COLUMN 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteAlterColumn( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    qlrInitAlterColumn * sInitPlan = NULL;
    ellSuppLogTableDDL   sSuppLog  = ELL_SUPP_LOG_MAX;

    ellObjectList * sAffectedViewList = NULL;
    
    stlTime sTime = 0;
    stlBool   sLocked = STL_TRUE;

    ellDictHandle * sAuthHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_DEFAULT_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_DEFAULT_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_NOT_NULL_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_NOT_NULL_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_DATA_TYPE_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_ALTER_IDENTITY_TYPE) ||
                        (aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_IDENTITY_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * 기본 정보 설정 
     ***************************************************************/
    
    /**
     * DDL 수행 시간 설정
     */

    sTime = stlNow();
    STL_TRY( ellBeginStmtDDL( aStmt, sTime, ELL_ENV(aEnv) ) == STL_SUCCESS );

    /***************************************************************
     *  Run-Time Validation
     ***************************************************************/

    STL_RAMP( RAMP_RETRY );
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * Valid Plan 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );
    
    sInitPlan = (qlrInitAlterColumn *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * DDL Lock 획득 
     */

    switch ( sInitPlan->mAlterAction )
    {
        case QLP_ALTER_COLUMN_SET_DEFAULT:
            {
                STL_TRY( ellLock4AlterColumn( aTransID,
                                              aStmt,
                                              & sInitPlan->mTableHandle,
                                              & sLocked,
                                              ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case QLP_ALTER_COLUMN_DROP_DEFAULT:
            {
                STL_TRY( ellLock4AlterColumn( aTransID,
                                              aStmt,
                                              & sInitPlan->mTableHandle,
                                              & sLocked,
                                              ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case QLP_ALTER_COLUMN_SET_NOT_NULL:
            {
                STL_TRY( ellLock4AddConst( aTransID,
                                           aStmt,
                                           sAuthHandle,
                                           & sInitPlan->mNotNullSchemaHandle,
                                           NULL, /* space handle */
                                           & sInitPlan->mTableHandle,
                                           ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL,
                                           & sLocked,
                                           ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case QLP_ALTER_COLUMN_DROP_NOT_NULL:
            {
                STL_TRY( ellLock4DropConst( aTransID,
                                            aStmt,
                                            & sInitPlan->mTableHandle,
                                            & sInitPlan->mNotNullHandle,
                                            & sLocked,
                                            ELL_ENV(aEnv) )
                         == STL_SUCCESS );
            }
        case QLP_ALTER_COLUMN_SET_DATA_TYPE:
            {
                STL_TRY( ellLock4AlterColumn( aTransID,
                                              aStmt,
                                              & sInitPlan->mTableHandle,
                                              & sLocked,
                                              ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case QLP_ALTER_COLUMN_ALTER_IDENTITY_SPEC:
            {
                STL_TRY( ellLock4AlterColumn( aTransID,
                                              aStmt,
                                              & sInitPlan->mTableHandle,
                                              & sLocked,
                                              ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case QLP_ALTER_COLUMN_DROP_IDENTITY:
            {
                STL_TRY( ellLock4AlterColumn( aTransID,
                                              aStmt,
                                              & sInitPlan->mTableHandle,
                                              & sLocked,
                                              ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_ASSERT(0);
            break;
            }
    }
            
    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );

    /**
     * Valid Plan 을 다시 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );
    
    sInitPlan = (qlrInitAlterColumn *) aSQLStmt->mInitPlan;

    /**********************************************************
     * Alter Column Action 에 따른 DDL 수행 
     **********************************************************/

    switch ( sInitPlan->mAlterAction )
    {
        case QLP_ALTER_COLUMN_SET_DEFAULT:
            STL_TRY( qlrExecuteSetDefault( aTransID,
                                           aStmt,
                                           sInitPlan,
                                           aEnv )
                     == STL_SUCCESS );
            break;
        case QLP_ALTER_COLUMN_DROP_DEFAULT:
            STL_TRY( qlrExecuteDropDefault( aTransID,
                                            aStmt,
                                            sInitPlan,
                                            aEnv )
                     == STL_SUCCESS );
            break;
        case QLP_ALTER_COLUMN_SET_NOT_NULL:
            STL_TRY( qlrExecuteSetNotNull( aTransID,
                                           aStmt,
                                           aDBCStmt,
                                           aSQLStmt,
                                           sInitPlan,
                                           aEnv )
                     == STL_SUCCESS );
            break;
        case QLP_ALTER_COLUMN_DROP_NOT_NULL:
            STL_TRY( qlrExecuteDropNotNull( aTransID,
                                            aStmt,
                                            sInitPlan,
                                            aEnv )
                     == STL_SUCCESS );
            break;
        case QLP_ALTER_COLUMN_SET_DATA_TYPE:
            {
                /**
                 * Affected View List 획득
                 */
                
                STL_TRY( ellGetAffectedViewListByRelation( aTransID,
                                                           aStmt,
                                                           & sInitPlan->mTableHandle,
                                                           & QLL_EXEC_DDL_MEM(aEnv),
                                                           & sAffectedViewList,
                                                           ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                /**
                 * Affected View List 에 대해 X Lock 획득
                 */
                
                STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                                      aStmt,
                                                      SML_LOCK_X,
                                                      sAffectedViewList,
                                                      & sLocked,
                                                      ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
                
                /**
                 * Affected View List 획득
                 */
                
                STL_TRY( ellGetAffectedViewListByRelation( aTransID,
                                                           aStmt,
                                                           & sInitPlan->mTableHandle,
                                                           & QLL_EXEC_DDL_MEM(aEnv),
                                                           & sAffectedViewList,
                                                           ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                
                STL_TRY( qlrExecuteSetDataType( aTransID,
                                                aStmt,
                                                aDBCStmt,
                                                sInitPlan,
                                                sAffectedViewList,
                                                aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLP_ALTER_COLUMN_ALTER_IDENTITY_SPEC:
            STL_TRY( qlrExecuteAlterIdentity( aTransID,
                                              aStmt,
                                              sInitPlan,
                                              aEnv )
                     == STL_SUCCESS );
            break;
        case QLP_ALTER_COLUMN_DROP_IDENTITY:
            STL_TRY( qlrExecuteDropIdentity( aTransID,
                                             aStmt,
                                             sInitPlan,
                                             aEnv )
                     == STL_SUCCESS );
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /***************************************************************
     * Alter Column 공통 Dictionary Record 변경 
     ***************************************************************/

    /**
     * Table 구조가 변경된 시간을 설정 
     */

    STL_TRY( ellSetTime4ModifyTableElement( aTransID,
                                            aStmt,
                                            & sInitPlan->mTableHandle,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************************************
     * Alter Column 공통 Dictionary Cache 재구성 
     ***************************************************************/

    switch ( sInitPlan->mAlterAction )
    {
        case QLP_ALTER_COLUMN_SET_DEFAULT:
            sSuppLog = ELL_SUPP_LOG_ALTER_COLUMN_SET_DEFAULT;
            break;
        case QLP_ALTER_COLUMN_DROP_DEFAULT:
            sSuppLog = ELL_SUPP_LOG_ALTER_COLUMN_DROP_DEFAULT;
            break;
        case QLP_ALTER_COLUMN_SET_NOT_NULL:
            sSuppLog = ELL_SUPP_LOG_ALTER_COLUMN_SET_NOT_NULL;
            break;
        case QLP_ALTER_COLUMN_DROP_NOT_NULL:
            sSuppLog = ELL_SUPP_LOG_ALTER_COLUMN_DROP_NOT_NULL;
            break;
        case QLP_ALTER_COLUMN_SET_DATA_TYPE:
            sSuppLog = ELL_SUPP_LOG_ALTER_COLUMN_SET_DATA_TYPE;
            break;
        case QLP_ALTER_COLUMN_ALTER_IDENTITY_SPEC:
            sSuppLog = ELL_SUPP_LOG_ALTER_COLUMN_ALTER_IDENTITY;
            break;
        case QLP_ALTER_COLUMN_DROP_IDENTITY:
            sSuppLog = ELL_SUPP_LOG_ALTER_COLUMN_DROP_IDENTITY;
            break;
        default:
            STL_ASSERT(0);
            break;
    }
    
    STL_TRY( ellRefineCache4AlterColumn( aTransID,
                                         aStmt,
                                         & sInitPlan->mTableHandle,
                                         sSuppLog,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlrExecuteAlterColumn()" ); */
    /* } */
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ALTER COLUMN .. SET DEFAULT 구문을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aInitPlan Init Plan
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteSetDefault( smlTransId                 aTransID,
                                smlStatement             * aStmt,
                                qlrInitAlterColumn       * aInitPlan,
                                qllEnv                   * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    
    /***************************************************************
     * Dictionary Record 변경 
     ***************************************************************/

    /**
     * Column 의 Default String 정보 변경
     */

    STL_TRY( ellModifyColumnDefault( aTransID,
                                     aStmt,
                                     ellGetColumnID( & aInitPlan->mColumnHandle ),
                                     aInitPlan->mColumnDefault, 
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ALTER COLUMN .. DROP DEFAULT 구문을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aInitPlan Init Plan
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteDropDefault( smlTransId                 aTransID,
                                 smlStatement             * aStmt,
                                 qlrInitAlterColumn       * aInitPlan,
                                 qllEnv                   * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    
    /***************************************************************
     * Dictionary Record 변경 
     ***************************************************************/

    /**
     * Column 의 Default String 정보 변경
     */

    STL_TRY( ellModifyColumnDefault( aTransID,
                                     aStmt,
                                     ellGetColumnID( & aInitPlan->mColumnHandle ),
                                     NULL, /* aDefaultString */
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ALTER COLUMN .. SET NOT NULL 구문을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aInitPlan Init Plan
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteSetNotNull( smlTransId                 aTransID,
                                smlStatement             * aStmt,
                                qllDBCStmt               * aDBCStmt,
                                qllStatement             * aSQLStmt,
                                qlrInitAlterColumn       * aInitPlan,
                                qllEnv                   * aEnv )
{
    stlInt32 sState = 0;
    
    ellDictHandle sConstHandle;
    stlBool       sObjectExist = STL_FALSE;

    knlValueBlockList * sValueBlock = NULL;
    knlPhysicalFilter   sIsNullFilter;
    knlColumnInReadRow  sEmptyColumnValue = { NULL, 0 };

    void                * sIterator = NULL;
    smlIteratorProperty   sIteratorProperty;
    smlFetchInfo          sFetchInfo;

    smlRowBlock         sRowBlock;
    smlRid              sRowRid;
    smlScn              sRowScn;

    ellDictHandle * sAuthHandle = NULL;
    
    stlInt64  sCheckConstID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    
    /***************************************************************
     * Constraint Validation
     ***************************************************************/
    
    /**
     * 동일한 NOT NULL Constraint 가 존재하는 지 검사
     */

    STL_TRY( ellGetConstraintDictHandleWithSchema( aTransID,
                                                   ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                   & aInitPlan->mNotNullSchemaHandle,
                                                   aInitPlan->mNotNullConstName,
                                                   & sConstHandle,
                                                   & sObjectExist,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sObjectExist == STL_FALSE, RAMP_ERR_AUTO_NAME_EXISTS );

    /***************************************************************
     * IS NULL filter 생성 
     ***************************************************************/

    /**
     * Column 공간 확보
     */
    
    STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                & sValueBlock,
                                KNL_NO_BLOCK_READ_CNT,
                                STL_TRUE, /* 컬럼임 */
                                STL_LAYER_SQL_PROCESSOR,
                                ellGetColumnTableID( & aInitPlan->mColumnHandle ),
                                ellGetColumnIdx( & aInitPlan->mColumnHandle ),
                                ellGetColumnDBType( & aInitPlan->mColumnHandle ),
                                ellGetColumnPrecision( & aInitPlan->mColumnHandle ),
                                ellGetColumnScale( & aInitPlan->mColumnHandle ),
                                ellGetColumnStringLengthUnit( & aInitPlan->mColumnHandle ),
                                ellGetColumnIntervalIndicator( & aInitPlan->mColumnHandle ),
                                & QLL_EXEC_DDL_MEM(aEnv),
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * column IS NULL filter 생성
     */

    sIsNullFilter.mFunc     = dtlPhysicalFuncArg1FuncList[ DTL_PHYSICAL_FUNC_IS_NULL - DTL_PHYSICAL_FUNC_IS ];
    sIsNullFilter.mColIdx1  = ellGetColumnIdx( & aInitPlan->mColumnHandle );
    sIsNullFilter.mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT;
    sIsNullFilter.mColVal1  = & sValueBlock->mColumnInRow;
    sIsNullFilter.mColVal2  = & sEmptyColumnValue;
    sIsNullFilter.mConstVal = NULL;
    sIsNullFilter.mNext     = NULL;
    
    /***************************************************************
     * column 에 NULL data 가 존재하는 지 검사
     ***************************************************************/

    if ( ellGetColumnNullable( & aInitPlan->mColumnHandle ) == STL_TRUE )
    {
        /**
         * Row Block 초기화
         */
        
        STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                        &sRowRid,
                                        &sRowScn,
                                        SML_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Iterator Property 초기화
         */
        
        SML_INIT_ITERATOR_PROP( sIteratorProperty );
        /**
         * Iterator 생성
         */

        STL_TRY( smlAllocIterator( aStmt,
                                   ellGetTableHandle( & aInitPlan->mTableHandle ),
                                   SML_TRM_COMMITTED, 
                                   SML_SRM_RECENT, 
                                   & sIteratorProperty,
                                   SML_SCAN_FORWARD,
                                   & sIterator,
                                   SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        sState = 1;
        
        /**
         * Set Fetch Info
         */

        sFetchInfo.mRange           = NULL;
        sFetchInfo.mPhysicalFilter  = & sIsNullFilter;
        sFetchInfo.mKeyCompList     = NULL;
        sFetchInfo.mLogicalFilter   = NULL;

        sFetchInfo.mColList         = sValueBlock;
        sFetchInfo.mRowIdColList    = NULL;
        sFetchInfo.mRowBlock        = & sRowBlock;
        sFetchInfo.mTableLogicalId  = KNL_ANONYMOUS_TABLE_ID;

        sFetchInfo.mSkipCnt         = 0;
        sFetchInfo.mFetchCnt        = sRowBlock.mBlockSize;
        sFetchInfo.mIsEndOfScan     = STL_FALSE;

        sFetchInfo.mFilterEvalInfo  = NULL;
        sFetchInfo.mFetchRecordInfo = NULL;
        sFetchInfo.mAggrFetchInfo   = NULL;
        sFetchInfo.mBlockJoinFetchInfo   = NULL;
        

        /**
         * NULL data fetch
         */
        
        STL_TRY( smlFetchNext( sIterator,
                               & sFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /**
         * Iterator 종료 
         */

        sState = 0;
        STL_TRY( smlFreeIterator( sIterator, SML_ENV(aEnv) ) == STL_SUCCESS );
        sIterator = NULL;

        /**
         * NULL data 가 없어야 함.
         */                        
                                   
        STL_TRY_THROW( (sFetchInfo.mIsEndOfScan == STL_TRUE) &&
                       (KNL_GET_BLOCK_USED_CNT( sValueBlock ) == 0),
                       RAMP_ERR_NULL_DATA_EXIST );
    }
    else
    {
        /**
         * Primary Key 에 의한 Not Nullable Column 임
         * - nothing to do
         */
    }

    
    /***************************************************************
     * Dictionary Record 변경 
     ***************************************************************/

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /**
     * Table Constraint Descriptor 추가
     */
        
    STL_TRY( ellInsertTableConstraintDesc(
                 aTransID,
                 aStmt,
                 ellGetAuthID( sAuthHandle ),
                 ellGetSchemaID( & aInitPlan->mNotNullSchemaHandle ),
                 & sCheckConstID,
                 ellGetColumnOwnerID( & aInitPlan->mColumnHandle ),
                 ellGetColumnSchemaID( & aInitPlan->mColumnHandle ),
                 ellGetColumnTableID( & aInitPlan->mColumnHandle ),
                 aInitPlan->mNotNullConstName,
                 ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL,
                 aInitPlan->mNotNullDeferrable,
                 aInitPlan->mNotNullInitDeferred,
                 aInitPlan->mNotNullEnforced,
                 ELL_DICT_OBJECT_ID_NA, /* 관련 index 가 없음 */
                 NULL,
                 ELL_ENV(aEnv) )
             == STL_SUCCESS );
                     
    /**
     * Check Constraint Descriptor 추가
     * - CHECK NOT NULL 은 구문을 저장하지 않음.
     */
    STL_TRY( ellInsertCheckConstraintDesc(
                 aTransID,
                 aStmt,
                 ellGetAuthID( sAuthHandle ),
                 ellGetSchemaID( & aInitPlan->mNotNullSchemaHandle ),
                 sCheckConstID,
                 NULL,   /* aCheckClause */
                 ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Check Table Usage Descriptor 추가
     */

    STL_TRY( ellInsertCheckTableUsageDesc(
                 aTransID,
                 aStmt,
                 ellGetAuthID( sAuthHandle ),
                 ellGetSchemaID( & aInitPlan->mNotNullSchemaHandle ),
                 sCheckConstID,
                 ellGetColumnOwnerID( & aInitPlan->mColumnHandle ),
                 ellGetColumnSchemaID( & aInitPlan->mColumnHandle ),
                 ellGetColumnTableID( & aInitPlan->mColumnHandle ),
                 ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Check Column Usage Descriptor 추가
     */

    STL_TRY( ellInsertCheckColumnUsageDesc(
                 aTransID,
                 aStmt,
                 ellGetAuthID( sAuthHandle ),
                 ellGetSchemaID( & aInitPlan->mNotNullSchemaHandle ),
                 sCheckConstID,
                 ellGetColumnOwnerID( & aInitPlan->mColumnHandle ),
                 ellGetColumnSchemaID( & aInitPlan->mColumnHandle ),
                 ellGetColumnTableID( & aInitPlan->mColumnHandle ),
                 ellGetColumnID( & aInitPlan->mColumnHandle ),
                 ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Column 을 NOT NULL 로 변경
     */

    STL_TRY( ellModifyColumnNullable( aTransID,
                                      aStmt,
                                      ellGetColumnID( & aInitPlan->mColumnHandle ),
                                      STL_FALSE, /* aIsNullable */
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_AUTO_NAME_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_AUTO_CONSTRAINT_NAME_EXISTS,
                      NULL,
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_NULL_DATA_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NULL_VALUES_FOUND,
                      NULL,
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    switch ( sState )
    {
        case 1:
            (void) smlFreeIterator( sIterator, SML_ENV(aEnv) );
        default:
            break;
    }
    
    return STL_FAILURE;
}




/**
 * @brief ALTER COLUMN .. DROP NOT NULL 구문을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aInitPlan Init Plan
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteDropNotNull( smlTransId                 aTransID,
                                 smlStatement             * aStmt,
                                 qlrInitAlterColumn       * aInitPlan,
                                 qllEnv                   * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    
    /***************************************************************
     * Dictionary Record 변경 
     ***************************************************************/

    /**
     * NOT NULL 의 Dictionary Record 삭제 
     */
    
    STL_TRY( ellDeleteDict4DropConstraint( aTransID,
                                           aStmt,
                                           & aInitPlan->mNotNullHandle,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Column 의 Nullable 정보 변경
     */

    STL_TRY( ellModifyColumnNullable( aTransID,
                                      aStmt,
                                      ellGetColumnID( & aInitPlan->mColumnHandle ),
                                      aInitPlan->mIsNullable, 
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ALTER COLUMN .. SET DATA TYPE 구문을 수행한다
 * @param[in] aTransID           Transaction ID
 * @param[in] aStmt              Statement
 * @param[in] aDBCStmt           DBC Statement
 * @param[in] aInitPlan          Init Plan
 * @param[in] aAffectedViewList  Affected View List
 * @param[in] aEnv               Environment
 */
stlStatus qlrExecuteSetDataType( smlTransId                 aTransID,
                                 smlStatement             * aStmt,
                                 qllDBCStmt               * aDBCStmt,
                                 qlrInitAlterColumn       * aInitPlan,
                                 ellObjectList            * aAffectedViewList,
                                 qllEnv                   * aEnv )
{
    ellObjectList * sIndexList = NULL;

    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    stlInt64          sColumnID = ELL_DICT_OBJECT_ID_NA;
    
    stlInt32          sKeyCnt = 0;
    ellIndexKeyDesc * sKeyDesc = NULL;

    stlInt64             sPrecision = DTL_PRECISION_NA;
    dtlStringLengthUnit  sCharUnit = DTL_STRING_LENGTH_UNIT_NA;
    
    stlInt64          sKeyBufferSize = 0;

    stlInt32   i = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAffectedViewList != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * CHAR(n) BINARY(n) 타입에 대한 Column 의 물리적 변경 
     ***************************************************************/

    if ( (aInitPlan->mNewDataType.mDBType == DTL_TYPE_CHAR) ||
         (aInitPlan->mNewDataType.mDBType == DTL_TYPE_BINARY) )
    {
        /**
         * @todo CHAR(n) BINARY(n) 타입에 대한 Column 의 물리적 변경
         */

        STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
    }
    else
    {
        /**
         * nothing to do
         */
    }
    
    /***************************************************************
     * 컬럼이 포함된 Index 의 key buffer size 조정
     ***************************************************************/

    sColumnID = ellGetColumnID( & aInitPlan->mColumnHandle );
    
    /**
     * 컬럼이 포함된 Index List 획득
     */
    
    STL_TRY( ellGetIndexListByColumn( aTransID,
                                      aStmt,
                                      & aInitPlan->mColumnHandle,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( (aInitPlan->mNewDataType.mDBType == DTL_TYPE_CHAR) ||
         (aInitPlan->mNewDataType.mDBType == DTL_TYPE_BINARY) )
    {
        /**
         * @todo CHAR(n), BINARY(n) 일 경우, Index Rebuild
         */
        
        STL_RING_FOREACH_ENTRY( & sIndexList->mHeadList, sObjectItem )
        {
            sObjectHandle = & sObjectItem->mObjectHandle;

            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
        }
    }
    else
    {
        /**
         * Key Buffer Size 계산
         */
        
        sPrecision = (aInitPlan->mNewDataType.mArgNum1 == DTL_PRECISION_NA) ?
                     gDefaultDataTypeDef[aInitPlan->mNewDataType.mDBType].mArgNum1 : 
                     aInitPlan->mNewDataType.mArgNum1;
        sCharUnit  = aInitPlan->mNewDataType.mStringLengthUnit;
        
        if ( sCharUnit == DTL_STRING_LENGTH_UNIT_NA )
        {
            sCharUnit = ellGetDbCharLengthUnit();
        }
        else
        {
            /* nothing to do */
        }
        
        STL_TRY( dtlGetDataValueBufferSize( aInitPlan->mNewDataType.mDBType,
                                            sPrecision,
                                            sCharUnit,
                                            & sKeyBufferSize,
                                            KNL_DT_VECTOR(aEnv),
                                            aEnv,
                                            KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
                
        /**
         * 모든 Index 의 Key Column 의 Buffer Size 조정
         */
        
        STL_RING_FOREACH_ENTRY( & sIndexList->mHeadList, sObjectItem )
        {
            sObjectHandle = & sObjectItem->mObjectHandle;

            sKeyCnt = ellGetIndexKeyCount( sObjectHandle );
            sKeyDesc = ellGetIndexKeyDesc( sObjectHandle );

            /**
             * Column 과 동일한 Key Column 을 검색
             */

            for ( i = 0; i < sKeyCnt; i++ )
            {
                if ( ellGetColumnID( & sKeyDesc[i].mKeyColumnHandle ) == sColumnID )
                {
                    /**
                     * Key Column 의 Buffer Size 조정
                     */

                    STL_TRY( smlAlterDataType( aStmt,
                                               ellGetIndexHandle( sObjectHandle ),
                                               i,
                                               aInitPlan->mNewDataType.mDBType,
                                               sKeyBufferSize,
                                               SML_ENV(aEnv) )
                             == STL_SUCCESS );

                    break;
                }
                else
                {
                    continue;
                }
            }
        }
    }
    
    /***************************************************************
     * Dictionary Record 변경 
     ***************************************************************/

    /**
     * Column 의 Data Type 정보 변경
     */

    switch ( aInitPlan->mNewDataType.mDBType )
    {
        case DTL_TYPE_BOOLEAN:
            {
                STL_TRY( ellModifyBooleanColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NATIVE_SMALLINT:
            {
                STL_TRY( ellModifySmallintColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NATIVE_INTEGER:
            {
                STL_TRY( ellModifyIntegerColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NATIVE_BIGINT:
            {
                STL_TRY( ellModifyBigintColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NATIVE_REAL:
            {
                STL_TRY( ellModifyRealColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NATIVE_DOUBLE:
            {
                STL_TRY( ellModifyDoubleColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_FLOAT:
            {
                STL_TRY( ellModifyFloatColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             aInitPlan->mNewDataType.mNumericRadix,
                             aInitPlan->mNewDataType.mArgNum1,
                             aInitPlan->mNewDataType.mArgNum2,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_NUMBER:
            {
                STL_TRY( ellModifyNumericColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             aInitPlan->mNewDataType.mNumericRadix,
                             aInitPlan->mNewDataType.mArgNum1,
                             aInitPlan->mNewDataType.mArgNum2,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_DECIMAL:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case DTL_TYPE_CHAR:
            {
                STL_TRY( ellModifyCharColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             aInitPlan->mNewDataType.mStringLengthUnit,
                             aInitPlan->mNewDataType.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_VARCHAR:
            {
                STL_TRY( ellModifyVarcharColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             aInitPlan->mNewDataType.mStringLengthUnit,
                             aInitPlan->mNewDataType.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_LONGVARCHAR:
            {
                STL_TRY( ellModifyLongVarcharColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_CLOB:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case DTL_TYPE_BINARY:
            {
                STL_TRY( ellModifyBinaryColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             aInitPlan->mNewDataType.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_VARBINARY:
            {
                STL_TRY( ellModifyVarBinaryColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             aInitPlan->mNewDataType.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_LONGVARBINARY:
            {
                STL_TRY( ellModifyLongVarBinaryColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_BLOB:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case DTL_TYPE_DATE:
            {
                STL_TRY( ellModifyDateColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_TIME:
            {
                STL_TRY( ellModifyTimeColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             aInitPlan->mNewDataType.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_TIMESTAMP:
            {
                STL_TRY( ellModifyTimestampColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             aInitPlan->mNewDataType.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
            {
                STL_TRY( ellModifyTimeWithZoneColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             aInitPlan->mNewDataType.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                STL_TRY( ellModifyTimestampWithZoneColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             aInitPlan->mNewDataType.mArgNum1,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            {
                STL_TRY( ellModifyIntervalYearToMonthColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             aInitPlan->mNewDataType.mIntervalIndicator,
                             aInitPlan->mNewDataType.mArgNum1,
                             aInitPlan->mNewDataType.mArgNum2,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                STL_TRY( ellModifyIntervalDayToSecondColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             aInitPlan->mNewDataType.mIntervalIndicator,
                             aInitPlan->mNewDataType.mArgNum1,
                             aInitPlan->mNewDataType.mArgNum2,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                break;
            }
        case DTL_TYPE_ROWID:
            {
                STL_TRY( ellModifyRowIdColumnTypeDesc(
                             aTransID,
                             aStmt,
                             sColumnID,
                             aInitPlan->mNewDataType.mUserTypeName,
                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                             
                break;
            }
        default:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
    }

    /**
     * Affected View List 에 대해 IS_AFFECTED 를 TRUE 로 변경
     */

    STL_TRY( ellSetAffectedViewList( aTransID,
                                     aStmt,
                                     aAffectedViewList,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlrExecuteSetDataType()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ALTER COLUMN .. alter identity 구문을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aInitPlan Init Plan
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteAlterIdentity( smlTransId                 aTransID,
                                   smlStatement             * aStmt,
                                   qlrInitAlterColumn       * aInitPlan,
                                   qllEnv                   * aEnv )
{
    void *   sNewIdentityHandle = NULL;
    stlInt64 sNewIdentityPhyID  = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     *  시쿼스 옵션의 물리적 변경 
     ***************************************************************/

    if ( aInitPlan->mModifySeqAttr == STL_TRUE )
    {
        /**
         * 물리적 속성이 변경되는 경우
         */
        STL_TRY( smlAlterSequence( aStmt,
                                   ellGetColumnIdentityHandle( & aInitPlan->mColumnHandle ),
                                   & aInitPlan->mIdentitySeqAttr,           
                                   & sNewIdentityPhyID,  
                                   & sNewIdentityHandle, 
                                   SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * 물리적 속성이 변경되지 않는 경우
         */
        
        sNewIdentityHandle = ellGetColumnIdentityHandle( & aInitPlan->mColumnHandle );
        sNewIdentityPhyID  = ellGetColumnIdentityPhysicalID( & aInitPlan->mColumnHandle );
    }
    
    /***************************************************************
     * Dictionary Record 변경 
     ***************************************************************/

    /**
     * Identity Column Dictionary 갱신
     */
    
    STL_TRY( ellModifyColumnAlterIdentity( aTransID,
                                           aStmt,
                                           ellGetColumnID( & aInitPlan->mColumnHandle ),
                                           & aInitPlan->mIdentityGenOption,
                                           & aInitPlan->mIdentitySeqAttr.mStartWith,
                                           & aInitPlan->mIdentitySeqAttr.mIncrementBy,
                                           & aInitPlan->mIdentitySeqAttr.mMaxValue,
                                           & aInitPlan->mIdentitySeqAttr.mMinValue,
                                           & aInitPlan->mIdentitySeqAttr.mCycle,
                                           & sNewIdentityPhyID, 
                                           & aInitPlan->mIdentitySeqAttr.mCacheSize,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER COLUMN .. DROP IDENTITY 구문을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aInitPlan Init Plan
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteDropIdentity( smlTransId                 aTransID,
                                  smlStatement             * aStmt,
                                  qlrInitAlterColumn       * aInitPlan,
                                  qllEnv                   * aEnv )
{
    void * sIdentityHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * Sequence 객체 제거 
     ***************************************************************/

    sIdentityHandle = ellGetColumnIdentityHandle( & aInitPlan->mColumnHandle );
            
    STL_TRY( smlDropSequence( aStmt,
                              sIdentityHandle,
                              SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    /***************************************************************
     * Dictionary Record 변경 
     ***************************************************************/

    STL_TRY( ellModifyColumnDropIdentity( aTransID,
                                          aStmt,
                                          ellGetColumnID( & aInitPlan->mColumnHandle ),
                                          aInitPlan->mIsNullable, 
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

                               

/** @} qlrAlterTableAlterColumn */
