/*******************************************************************************
 * dtmPhysicalComp.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *x3
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file dtmPhysicalComp.c
 * @brief Physical 비교 연산
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtdDataType.h>
#include <dtfNumeric.h>


/**
 * @defgroup dtmPhysicalComp Physical 비교 연산
 * @ingroup dtlOperCompareType
 * @{
 */

/* Type vs Type */
dtlPhysicalCompareFuncPtr dtlPhysicalCompareFuncList[ DTL_TYPE_MAX ][ DTL_TYPE_MAX ] =
{
    /**< BOOLEAN */
    {
        dtlPhysicalCompareBooleanToBoolean,      /**< BOOLEAN */
        NULL,                            /**< SMALLINT */
        NULL,                            /**< INTEGER */
        NULL,                            /**< BIGINT */

        NULL,                            /**< REAL */
        NULL,                            /**< DOUBLE PRECISION */

        NULL,                            /**< FLOAT */        
        NULL,                            /**< NUMBER */
        NULL,                            /**< unsupported feature, DECIMAL */
    
        NULL,                            /**< CHARACTER */
        NULL,                            /**< CHARACTER VARYING */
        NULL,                            /**< CHARACTER LONG VARYING */
        NULL,                            /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                            /**< BINARY */
        NULL,                            /**< BINARY VARYING */
        NULL,                            /**< BINARY LONG VARYING */
        NULL,                            /**< unsupported feature, BINARY LARGE OBJECT */
                            
        NULL,                            /**< DATE */
        NULL,                            /**< TIME WITHOUT TIME ZONE */
        NULL,                            /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                            /**< TIME WITH TIME ZONE */
        NULL,                            /**< TIMESTAMP WITH TIME ZONE */

        NULL,                            /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                            /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                             /**< DTL_TYPE_ROWID */        
    },

    /**< SMALLINT */
    {
        NULL,                              /**< BOOLEAN */
        dtlPhysicalCompareSmallIntToSmallInt,      /**< SMALLINT */
        dtlPhysicalCompareSmallIntToInteger,       /**< INTEGER */
        dtlPhysicalCompareSmallIntToBigInt,        /**< BIGINT */

        dtlPhysicalCompareSmallIntToReal,          /**< REAL */
        dtlPhysicalCompareSmallIntToDouble,        /**< DOUBLE PRECISION */

        dtlPhysicalCompareSmallIntToNumeric,       /**< FLOAT */        
        dtlPhysicalCompareSmallIntToNumeric,       /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },
    /**< INTEGER */
    {
        NULL,                              /**< BOOLEAN */
        dtlPhysicalCompareIntegerToSmallInt,       /**< SMALLINT */
        dtlPhysicalCompareIntegerToInteger,        /**< INTEGER */
        dtlPhysicalCompareIntegerToBigInt,         /**< BIGINT */

        dtlPhysicalCompareIntegerToReal,           /**< REAL */
        dtlPhysicalCompareIntegerToDouble,         /**< DOUBLE PRECISION */

        dtlPhysicalCompareIntegerToNumeric,        /**< FLOAT */        
        dtlPhysicalCompareIntegerToNumeric,        /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },

    /**< BIGINT */
    {
        NULL,                              /**< BOOLEAN */
        dtlPhysicalCompareBigIntToSmallInt,        /**< SMALLINT */
        dtlPhysicalCompareBigIntToInteger,         /**< INTEGER */
        dtlPhysicalCompareBigIntToBigInt,          /**< BIGINT */

        dtlPhysicalCompareBigIntToReal,            /**< REAL */
        dtlPhysicalCompareBigIntToDouble,          /**< DOUBLE PRECISION */

        dtlPhysicalCompareBigIntToNumeric,         /**< FLOAT */        
        dtlPhysicalCompareBigIntToNumeric,         /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },

    /**< REAL */
    {
        NULL,                              /**< BOOLEAN */
        dtlPhysicalCompareRealToSmallInt,          /**< SMALLINT */
        dtlPhysicalCompareRealToInteger,           /**< INTEGER */
        dtlPhysicalCompareRealToBigInt,            /**< BIGINT */

        dtlPhysicalCompareRealToReal,              /**< REAL */
        dtlPhysicalCompareRealToDouble,            /**< DOUBLE PRECISION */

        dtlPhysicalCompareRealToNumeric,           /**< FLOAT */        
        dtlPhysicalCompareRealToNumeric,           /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },

    /**< DOUBLE */
    {
        NULL,                              /**< BOOLEAN */
        dtlPhysicalCompareDoubleToSmallInt,        /**< SMALLINT */
        dtlPhysicalCompareDoubleToInteger,         /**< INTEGER */
        dtlPhysicalCompareDoubleToBigInt,          /**< BIGINT */

        dtlPhysicalCompareDoubleToReal,            /**< REAL */
        dtlPhysicalCompareDoubleToDouble,          /**< DOUBLE PRECISION */

        dtlPhysicalCompareDoubleToNumeric,         /**< FLOAT */        
        dtlPhysicalCompareDoubleToNumeric,         /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },

    /**< FLOAT */
    {
        NULL,                              /**< BOOLEAN */
        dtlPhysicalCompareNumericToSmallInt,       /**< SMALLINT */
        dtlPhysicalCompareNumericToInteger,        /**< INTEGER */
        dtlPhysicalCompareNumericToBigInt,         /**< BIGINT */

        dtlPhysicalCompareNumericToReal,           /**< REAL */
        dtlPhysicalCompareNumericToDouble,         /**< DOUBLE PRECISION */

        dtlPhysicalCompareNumericToNumeric,        /**< FLOAT */        
        dtlPhysicalCompareNumericToNumeric,        /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },
    
    /**< NUMBER */
    {
        NULL,                              /**< BOOLEAN */
        dtlPhysicalCompareNumericToSmallInt,       /**< SMALLINT */
        dtlPhysicalCompareNumericToInteger,        /**< INTEGER */
        dtlPhysicalCompareNumericToBigInt,         /**< BIGINT */

        dtlPhysicalCompareNumericToReal,           /**< REAL */
        dtlPhysicalCompareNumericToDouble,         /**< DOUBLE PRECISION */

        dtlPhysicalCompareNumericToNumeric,        /**< FLOAT */        
        dtlPhysicalCompareNumericToNumeric,        /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },
    
    /**< DECIMAL */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT   */
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },

    /**< CHARACTER */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        dtlPhysicalCompareFixedLengthChar,         /**< CHARACTER */
        dtlPhysicalCompareVariableLengthChar,      /**< CHARACTER VARYING */
        dtlPhysicalCompareVariableLengthChar,      /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */
    },

    /**< CHARACTER VARYING */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        dtlPhysicalCompareVariableLengthChar,      /**< CHARACTER */
        dtlPhysicalCompareVariableLengthChar,      /**< CHARACTER VARYING */
        dtlPhysicalCompareVariableLengthChar,      /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },

    /**< CHARACTER LONG VARYING */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        dtlPhysicalCompareVariableLengthChar,      /**< CHARACTER */
        dtlPhysicalCompareVariableLengthChar,      /**< CHARACTER VARYING */
        dtlPhysicalCompareVariableLengthChar,      /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },

    /**< CHARACTER LARGE OBJECT */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },
       
    /**< BINARY */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        dtlPhysicalCompareFixedLengthBinary,       /**< BINARY */
        dtlPhysicalCompareVariableLengthBinary,    /**< BINARY VARYING */
        dtlPhysicalCompareVariableLengthBinary,    /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },

    /**< BINARY VARYING */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        dtlPhysicalCompareVariableLengthBinary,    /**< BINARY */
        dtlPhysicalCompareVariableLengthBinary,    /**< BINARY VARYING */
        dtlPhysicalCompareVariableLengthBinary,     /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },

    /**< BINARY LONG VARYING */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        dtlPhysicalCompareVariableLengthBinary,    /**< BINARY */
        dtlPhysicalCompareVariableLengthBinary,    /**< BINARY VARYING */
        dtlPhysicalCompareVariableLengthBinary,    /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },

    /**< BINARY LARGE OBJECT */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },
        
    /**< DATE */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        dtlPhysicalCompareDateToDate,      /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        dtlPhysicalCompareDateToTimestamp, /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },

    /**< TIME WITHOUT TIME ZONE */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        dtlPhysicalCompareTimeToTime,              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },
        
    /**< TIMESTAMP WITHOUT TIME ZONE */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        dtlPhysicalCompareTimestampToDate, /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        dtlPhysicalCompareTimestampToTimestamp,    /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },

    /**< TIME WITH TIME ZONE */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        dtlPhysicalCompareTimeTzToTimeTz,          /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },

    /**< TIMESTAMP WITH TIME ZONE */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        dtlPhysicalCompareTimestampTzToTimestampTz, /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },
       
    /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        dtlPhysicalCompareIntervalYearToMonth,     /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },

    /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        dtlPhysicalCompareIntervalDayToSecond,     /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                               /**< DTL_TYPE_ROWID */        
    },

    /**< DTL_TYPE_ROWID */
    {
        NULL,                              /**< BOOLEAN */
        NULL,                              /**< SMALLINT */
        NULL,                              /**< INTEGER */
        NULL,                              /**< BIGINT */

        NULL,                              /**< REAL */
        NULL,                              /**< DOUBLE PRECISION */

        NULL,                              /**< FLOAT */        
        NULL,                              /**< NUMBER */
        NULL,                              /**< unsupported feature, DECIMAL */
    
        NULL,                              /**< CHARACTER */
        NULL,                              /**< CHARACTER VARYING */
        NULL,                              /**< CHARACTER LONG VARYING */
        NULL,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        NULL,                              /**< BINARY */
        NULL,                              /**< BINARY VARYING */
        NULL,                              /**< BINARY LONG VARYING */
        NULL,                              /**< unsupported feature, BINARY LARGE OBJECT */

        NULL,                              /**< DATE */
        NULL,                              /**< TIME WITHOUT TIME ZONE */
        NULL,                              /**< TIMESTAMP WITHOUT TIME ZONE */
        NULL,                              /**< TIME WITH TIME ZONE */
        NULL,                              /**< TIMESTAMP WITH TIME ZONE */

        NULL,                              /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                              /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        dtlPhysicalCompareRowId            /**< DTL_TYPE_ROWID */
    }   
};



/*******************************************************************************
 * TYPE : BINARY INTEGER
 ******************************************************************************/

/**
 * @brief Boolean vs Boolean의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareBooleanToBoolean( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength )
{
    dtlSmallIntType   sLeft;
    dtlSmallIntType   sRight;

    DTL_GET_BOOLEAN_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_BOOLEAN_FROM_PTR( aRightValue, sRight ); 
    
    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief SmallInt vs SmallInt의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareSmallIntToSmallInt( void      * aLeftValue,
                                                       stlInt64    aLeftLength,
                                                       void      * aRightValue,
                                                       stlInt64    aRightLength )
{
    dtlSmallIntType   sLeft;
    dtlSmallIntType   sRight;

    DTL_GET_NATIVE_SMALLINT_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_SMALLINT_FROM_PTR( aRightValue, sRight ); 

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief SmallInt vs Integer의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareSmallIntToInteger( void      * aLeftValue,
                                                      stlInt64    aLeftLength,
                                                      void      * aRightValue,
                                                      stlInt64    aRightLength )
{
    dtlSmallIntType   sLeft;
    dtlIntegerType    sRight;

    DTL_GET_NATIVE_SMALLINT_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_INTEGER_FROM_PTR( aRightValue, sRight ); 
    
    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief SmallInt vs BigInt의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareSmallIntToBigInt( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength )
{
    dtlSmallIntType   sLeft;
    dtlBigIntType     sRight;

    DTL_GET_NATIVE_SMALLINT_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_BIGINT_FROM_PTR( aRightValue, sRight ); 

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief SmallInt vs Real의 Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareSmallIntToReal( void      * aLeftValue,
                                                   stlInt64    aLeftLength,
                                                   void      * aRightValue,
                                                   stlInt64    aRightLength )
{
    dtlSmallIntType   sLeft;
    dtlRealType       sRight;

    DTL_GET_NATIVE_SMALLINT_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_REAL_FROM_PTR( aRightValue, sRight ); 

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief SmallInt vs Double의 Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareSmallIntToDouble( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength )
{
    dtlSmallIntType   sLeft;
    dtlDoubleType     sRight;
    
    DTL_GET_NATIVE_SMALLINT_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_DOUBLE_FROM_PTR( aRightValue, sRight ); 

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief Integer vs SmallInt의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareIntegerToSmallInt( void      * aLeftValue,
                                                      stlInt64    aLeftLength,
                                                      void      * aRightValue,
                                                      stlInt64    aRightLength )
{
    dtlIntegerType    sLeft;
    dtlSmallIntType   sRight;

    DTL_GET_NATIVE_INTEGER_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_SMALLINT_FROM_PTR( aRightValue, sRight ); 
    
    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief Integer vs Integer의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareIntegerToInteger( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength )
{
    dtlIntegerType    sLeft;
    dtlIntegerType    sRight;

    DTL_GET_NATIVE_INTEGER_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_INTEGER_FROM_PTR( aRightValue, sRight ); 

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief Integer vs BigInt의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareIntegerToBigInt( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength )
{
    dtlIntegerType    sLeft;
    dtlBigIntType     sRight;

    DTL_GET_NATIVE_INTEGER_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_BIGINT_FROM_PTR( aRightValue, sRight ); 
    
    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief Integer vs Real의 Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareIntegerToReal( void      * aLeftValue,
                                                  stlInt64    aLeftLength,
                                                  void      * aRightValue,
                                                  stlInt64    aRightLength )
{
    dtlIntegerType   sLeft;
    dtlRealType      sRight;
    
    DTL_GET_NATIVE_INTEGER_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_REAL_FROM_PTR( aRightValue, sRight ); 

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief Integer vs Double의 Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareIntegerToDouble( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength )
{
    dtlIntegerType   sLeft;
    dtlDoubleType    sRight;

    DTL_GET_NATIVE_INTEGER_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_DOUBLE_FROM_PTR( aRightValue, sRight ); 

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief BigInt vs SmallInt의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareBigIntToSmallInt( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength )
{
    dtlBigIntType     sLeft;
    dtlSmallIntType   sRight;

    DTL_GET_NATIVE_BIGINT_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_SMALLINT_FROM_PTR( aRightValue, sRight ); 
        
    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief BigInt vs Integer의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareBigIntToInteger( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength )
{
    dtlBigIntType    sLeft;
    dtlIntegerType   sRight;

    DTL_GET_NATIVE_BIGINT_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_INTEGER_FROM_PTR( aRightValue, sRight ); 
    
    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief BigInt vs BigInt의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareBigIntToBigInt( void      * aLeftValue,
                                                   stlInt64    aLeftLength,
                                                   void      * aRightValue,
                                                   stlInt64    aRightLength )
{
    dtlBigIntType    sLeft;
    dtlBigIntType    sRight;

    DTL_GET_NATIVE_BIGINT_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_BIGINT_FROM_PTR( aRightValue, sRight ); 
    
    return ( sLeft > sRight ) - ( sLeft < sRight );
}


/**
 * @brief BigInt vs Real의 Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareBigIntToReal( void      * aLeftValue,
                                                 stlInt64    aLeftLength,
                                                 void      * aRightValue,
                                                 stlInt64    aRightLength )
{
    dtlBigIntType   sLeft;
    dtlRealType     sRight;
    
    DTL_GET_NATIVE_BIGINT_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_REAL_FROM_PTR( aRightValue, sRight ); 

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief BigInt vs Double의 Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareBigIntToDouble( void      * aLeftValue,
                                                   stlInt64    aLeftLength,
                                                   void      * aRightValue,
                                                   stlInt64    aRightLength )
{
    dtlBigIntType   sLeft;
    dtlDoubleType   sRight;
    
    DTL_GET_NATIVE_BIGINT_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_DOUBLE_FROM_PTR( aRightValue, sRight ); 

    return ( sLeft > sRight ) - ( sLeft < sRight );
}


/*******************************************************************************
 * TYPE : BINARY REAL
 ******************************************************************************/
/**
 * @brief Real vs Real의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareRealToReal( void      * aLeftValue,
                                               stlInt64    aLeftLength,
                                               void      * aRightValue,
                                               stlInt64    aRightLength )
{
    dtlRealType   sLeft;
    dtlRealType   sRight;

    DTL_GET_NATIVE_REAL_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_REAL_FROM_PTR( aRightValue, sRight );

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief Real vs Double의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareRealToDouble( void      * aLeftValue,
                                                 stlInt64    aLeftLength,
                                                 void      * aRightValue,
                                                 stlInt64    aRightLength )
{
    dtlRealType     sLeft;
    dtlDoubleType   sRight;

    DTL_GET_NATIVE_REAL_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_DOUBLE_FROM_PTR( aRightValue, sRight );
    
    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief Double vs Real의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareDoubleToReal( void      * aLeftValue,
                                                 stlInt64    aLeftLength,
                                                 void      * aRightValue,
                                                 stlInt64    aRightLength )
{
    dtlDoubleType   sLeft;
    dtlRealType     sRight;

    DTL_GET_NATIVE_DOUBLE_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_REAL_FROM_PTR( aRightValue, sRight );
    
    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief Double vs Double의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareDoubleToDouble( void      * aLeftValue,
                                                   stlInt64    aLeftLength,
                                                   void      * aRightValue,
                                                   stlInt64    aRightLength )
{
    dtlDoubleType   sLeft;
    dtlDoubleType   sRight;

    DTL_GET_NATIVE_DOUBLE_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_DOUBLE_FROM_PTR( aRightValue, sRight );
    
    return ( sLeft > sRight ) - ( sLeft < sRight );
}


/**
 * @brief Real vs SmallInt의 Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareRealToSmallInt( void      * aLeftValue,
                                                   stlInt64    aLeftLength,
                                                   void      * aRightValue,
                                                   stlInt64    aRightLength )
{
    dtlRealType        sLeft;
    dtlSmallIntType    sRight;
    
    DTL_GET_NATIVE_REAL_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_SMALLINT_FROM_PTR( aRightValue, sRight ); 

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief Real vs Integer의 Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareRealToInteger( void      * aLeftValue,
                                                  stlInt64    aLeftLength,
                                                  void      * aRightValue,
                                                  stlInt64    aRightLength )
{
    dtlRealType       sLeft;
    dtlIntegerType    sRight;
    
    DTL_GET_NATIVE_REAL_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_INTEGER_FROM_PTR( aRightValue, sRight ); 

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief Real vs BigInt의 Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareRealToBigInt( void      * aLeftValue,
                                                 stlInt64    aLeftLength,
                                                 void      * aRightValue,
                                                 stlInt64    aRightLength )
{
    dtlRealType      sLeft;
    dtlBigIntType    sRight;
    
    DTL_GET_NATIVE_REAL_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_BIGINT_FROM_PTR( aRightValue, sRight ); 

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief Double vs SmallInt의 Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareDoubleToSmallInt( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength )
{
    dtlDoubleType      sLeft;
    dtlSmallIntType    sRight;
    
    DTL_GET_NATIVE_DOUBLE_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_SMALLINT_FROM_PTR( aRightValue, sRight ); 

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief Double vs Integer의 Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareDoubleToInteger( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength )
{
    dtlDoubleType     sLeft;
    dtlIntegerType    sRight;
    
    DTL_GET_NATIVE_DOUBLE_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_INTEGER_FROM_PTR( aRightValue, sRight ); 

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief Double vs BigInt의 Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareDoubleToBigInt( void      * aLeftValue,
                                                   stlInt64    aLeftLength,
                                                   void      * aRightValue,
                                                   stlInt64    aRightLength )
{
    dtlDoubleType    sLeft;
    dtlBigIntType    sRight;
    
    DTL_GET_NATIVE_DOUBLE_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_NATIVE_BIGINT_FROM_PTR( aRightValue, sRight ); 

    return ( sLeft > sRight ) - ( sLeft < sRight );
}


/*******************************************************************************
 * TYPE : NUMERIC 
 ******************************************************************************/

/**
 * @brief numeric type과 real type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareNumericToReal( void      * aLeftValue,
                                                  stlInt64    aLeftLength,
                                                  void      * aRightValue,
                                                  stlInt64    aRightLength )
{
    dtlNumericType    * sLeftVal;
    dtlRealType         sRightVal;    

    stlInt16            sNumLength;
    stlUInt8          * sDigit;

    stlInt32            sMaxExponent;
    stlInt32            sNumExponent;
    stlInt32            sRoundExponent;
    stlInt64            sLeftInt;
    stlInt64            sRightInt;    

    DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftValue, sLeftVal );
    DTL_GET_NATIVE_REAL_FROM_PTR( aRightValue, sRightVal );    

    if( DTL_NUMERIC_IS_POSITIVE( sLeftVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sRightVal < 0.0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( sLeftVal, aLeftLength ) )
        {
            return ( sRightVal == 0.0 ? DTL_COMPARISON_EQUAL : DTL_COMPARISON_LESS );
        }

        if( sRightVal == 0.0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* real 범위 체크 */
        if( sRightVal < 1.0 )
        {
            sMaxExponent = stlLog10f( sRightVal ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10f( sRightVal );
        }
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sLeftVal );

        if( sMaxExponent < sNumExponent * 2 )
        {
            return DTL_COMPARISON_GREATER;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return DTL_COMPARISON_LESS;
        }
        
        sRoundExponent = STL_FLT_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = (( ((stlInt64) ( sRightVal * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10 ) * 100;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sRightVal * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength );
        sLeftInt = 0;

        if( sNumLength > 4 )
        {
            sNumLength = 4;
            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }

            if( sLeftInt >= sRightInt )
            {
                return DTL_COMPARISON_GREATER; 
            }
            else
            {
                return DTL_COMPARISON_LESS;
            }
        }
        else
        {
            sNumExponent = 4 - sNumLength;

            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

            if( sLeftInt < sRightInt )
            {
                return DTL_COMPARISON_LESS; 
            }
            else
            {
                return sLeftInt > sRightInt;
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sRightVal >= 0.0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* real 범위 체크 */
        if( sRightVal > -1.0 )
        {
            sMaxExponent = stlLog10f( -sRightVal ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10f( -sRightVal );
        }
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sLeftVal );

        if( sMaxExponent < sNumExponent * 2 )
        {
            return DTL_COMPARISON_LESS;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return DTL_COMPARISON_GREATER;
        }
        
        sRoundExponent = STL_FLT_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = (( ((stlInt64) ( sRightVal * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10 ) * 100;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sRightVal * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength );
        sLeftInt = 0;

        if( sNumLength > 4 )
        {
            sNumLength = 4;
            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }

            if( sLeftInt <= sRightInt )
            {
                return DTL_COMPARISON_LESS; 
            }
            else
            {
                return DTL_COMPARISON_GREATER;
            }
        }
        else
        {
            sNumExponent = 4 - sNumLength;

            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

            if( sLeftInt < sRightInt )
            {
                return DTL_COMPARISON_LESS; 
            }
            else
            {
                return sLeftInt > sRightInt;
            }
        }
    }
}


/**
 * @brief numeric type과 double type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareNumericToDouble( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength )
{
    dtlNumericType    * sLeftVal;
    dtlDoubleType       sRightVal;    

    stlInt16            sNumLength;
    stlUInt8          * sDigit;

    stlInt32            sMaxExponent;
    stlInt32            sNumExponent;
    stlInt32            sRoundExponent;
    stlInt64            sLeftInt;    
    stlInt64            sRightInt;

    DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftValue, sLeftVal );
    DTL_GET_NATIVE_DOUBLE_FROM_PTR( aRightValue, sRightVal );    

    if( DTL_NUMERIC_IS_POSITIVE( sLeftVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sRightVal < 0.0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( sLeftVal, aLeftLength ) )
        {
            return ( sRightVal == 0.0 ? DTL_COMPARISON_EQUAL : DTL_COMPARISON_LESS );
        }

        if( sRightVal == 0.0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* double 범위 체크 */
        if( sRightVal < 1.0 )
        {
            sMaxExponent = stlLog10( sRightVal ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10( sRightVal );
        }
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sLeftVal );

        if( sMaxExponent < sNumExponent * 2 )
        {
            return DTL_COMPARISON_GREATER;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return DTL_COMPARISON_LESS;
        }

        sRoundExponent = STL_DBL_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = ( ((stlInt64) ( sRightVal * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sRightVal * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength );
        sLeftInt = 0;

        if( sNumLength > 8 )
        {
            sNumLength = 8;
            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }

            if( sLeftInt >= sRightInt )
            {
                return DTL_COMPARISON_GREATER; 
            }
            else
            {
                return DTL_COMPARISON_LESS;
            }
        }
        else
        {
            sNumExponent = 8 - sNumLength;

            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

            if( sLeftInt < sRightInt )
            {
                return DTL_COMPARISON_LESS; 
            }
            else
            {
                return sLeftInt > sRightInt;
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sRightVal >= 0.0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* double 범위 체크 */
        if( sRightVal > -1.0 )
        {
            sMaxExponent = stlLog10( -sRightVal ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10( -sRightVal );
        }
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sLeftVal );

        if( sMaxExponent < sNumExponent * 2 )
        {
            return DTL_COMPARISON_LESS;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return DTL_COMPARISON_GREATER;
        }

        sRoundExponent = STL_DBL_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = ( ((stlInt64) ( sRightVal * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sRightVal * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength );
        sLeftInt = 0;

        if( sNumLength > 8 )
        {
            sNumLength = 8;
            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }

            if( sLeftInt <= sRightInt )
            {
                return DTL_COMPARISON_LESS; 
            }
            else
            {
                return DTL_COMPARISON_GREATER;
            }
        }
        else
        {
            sNumExponent = 8 - sNumLength;

            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

            if( sLeftInt < sRightInt )
            {
                return DTL_COMPARISON_LESS; 
            }
            else
            {
                return sLeftInt > sRightInt;
            }
        }
    }
}


/* /\** */
/*  * @brief numeric type과 real type간의 Physical Compare 연산 */
/*  * @param[in]  aLeftValue    left value */
/*  * @param[in]  aLeftLength   left value length */
/*  * @param[in]  aRightValue   right value */
/*  * @param[in]  aRightLength  right value length */
/*  * @return dtlCompareResult */
/*  *\/ */
/* dtlCompareResult dtlPhysicalCompareNumericToReal( void      * aLeftValue, */
/*                                                   stlInt64    aLeftLength, */
/*                                                   void      * aRightValue, */
/*                                                   stlInt64    aRightLength ) */
/* { */
/*     dtlNumericType    * sLeftVal; */
/*     dtlRealType         sRightVal; */

/*     stlInt16            sNumLength; */
/*     stlUInt8          * sDigit; */
/*     stlFloat32          sPow; */
/*     stlFloat32          sTempValue; */

/*     DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftValue, sLeftVal ); */
/*     DTL_GET_NATIVE_REAL_FROM_PTR( aRightValue, sRightVal ); */

/*     if( DTL_NUMERIC_IS_POSITIVE( sLeftVal ) ) */
/*     { */
/*         /\* 부호가 다른지 검사 *\/ */
/*         if( sRightVal < 0.0 ) */
/*         { */
/*             return DTL_COMPARISON_GREATER; */
/*         } */

/*         /\* zero 검사 *\/ */
/*         if( DTL_NUMERIC_IS_ZERO( sLeftVal, aLeftLength ) ) */
/*         { */
/*             return ( sRightVal == 0.0 ? DTL_COMPARISON_EQUAL : DTL_COMPARISON_LESS ); */
/*         } */

/*         if( sRightVal == 0.0 ) */
/*         { */
/*             return DTL_COMPARISON_GREATER; */
/*         } */

/*         /\* real 범위 체크 *\/ */
/*         sPow = gPredefined10PowValue[ DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sLeftVal ) * 2 ]; */
/*         sTempValue = sRightVal / sPow; */
        
/*         if( stlIsinfinite( sTempValue ) != STL_IS_INFINITE_FALSE ) */
/*         { */
/*             /\* overflow: numeric이 real의 최대 표현범위보다 크다. *\/ */
/*             return DTL_COMPARISON_LESS; */
/*         } */
        
/*         if( ( stlIsnan( sTempValue ) != STL_IS_NAN_FALSE ) || */
/*             ( sTempValue == 0.0 ) ) */
/*         { */
/*             /\* underflow: numeric이 0에 가깝다. 즉, real보다 작다. *\/ */
/*             return DTL_COMPARISON_GREATER; */
/*         } */

/*         sDigit = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal ); */

/*         if( sTempValue < 10.0 ) */
/*         { */
/*             sTempValue += 0.000005; */
/*             if( sTempValue <= 1.0 ) */
/*             { */
/*                 return DTL_COMPARISON_GREATER; */
/*             } */
/*         } */
/*         else */
/*         { */
/*             sTempValue += 0.00005; */
/*             if( sTempValue >= 100.0 ) */
/*             { */
/*                 return DTL_COMPARISON_LESS; */
/*             } */
/*         } */

/*         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[0] ) < (stlInt32) sTempValue ) */
/*         { */
/*             return DTL_COMPARISON_LESS; */
/*         } */
        
/*         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[0] ) > (stlInt32) sTempValue ) */
/*         { */
/*             return DTL_COMPARISON_GREATER; */
/*         } */
/*         else */
/*         { */
/*             sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength ); */
/*             switch( sNumLength ) */
/*             { */
/*                 case 3 : */
/*                     { */
/*                         sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[1] ) < (stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_LESS; */
/*                         } */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[1] ) > (stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_GREATER; */
/*                         } */

/*                         sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[2] ) < (stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_LESS; */
/*                         } */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[2] ) > (stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_GREATER; */
/*                         } */

/*                         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[0] ) > 9 ) */
/*                         { */
/*                             return DTL_COMPARISON_EQUAL; */
/*                         } */
/*                         else */
/*                         { */
/*                             return -( (stlInt32)(( sTempValue - (stlInt32) sTempValue ) * 10 ) > 0 ); */
/*                         } */
/*                     } */
/*                 case 2 : */
/*                     { */
/*                         sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[1] ) < (stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_LESS; */
/*                         } */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[1] ) > (stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_GREATER; */
/*                         } */
                            
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[0] ) > 9 ) */
/*                         { */
/*                             return -( (stlInt32)(( sTempValue - (stlInt32) sTempValue ) * 100 ) > 0 ); */
/*                         } */
/*                         else */
/*                         { */
/*                             return -( (stlInt32)(( sTempValue - (stlInt32) sTempValue ) * 1000 ) > 0 ); */
/*                         } */
/*                     } */
/*                 case 1 : */
/*                     { */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[0] ) > 9 ) */
/*                         { */
/*                             return -( (stlInt32)(( sTempValue - (stlInt32) sTempValue ) * 10000 ) > 0 ); */
/*                         } */
/*                         else */
/*                         { */
/*                             return -( (stlInt32)(( sTempValue - (stlInt32) sTempValue ) * 100000 ) > 0 ); */
/*                         } */
/*                     } */
/*                 default : */
/*                     { */
/*                         sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[1] ) < (stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_LESS; */
/*                         } */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[1] ) > (stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_GREATER; */
/*                         } */

/*                         sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[2] ) < (stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_LESS; */
/*                         } */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[2] ) > (stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_GREATER; */
/*                         } */

/*                         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[0] ) > 9 ) */
/*                         { */
/*                             return DTL_COMPARISON_GREATER; */
/*                         } */
/*                         else */
/*                         { */
/*                             sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 10; */
/*                             if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[3] ) < ((stlInt32) sTempValue) * 10 ) */
/*                             { */
/*                                 return DTL_COMPARISON_LESS; */
/*                             } */
/*                             if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[3] ) > ((stlInt32) sTempValue) * 10 ) */
/*                             { */
/*                                 return DTL_COMPARISON_GREATER; */
/*                             } */

/*                             return ( sNumLength > 4 ); */
/*                         } */
/*                     } */
/*             } */
/*         } */
/*     } */
/*     else */
/*     { */
/*         /\* 부호가 다른지 검사 *\/ */
/*         if( sRightVal >= 0.0 ) */
/*         { */
/*             return DTL_COMPARISON_LESS; */
/*         } */

/*         /\* real 범위 체크 *\/ */
/*         sPow = gPredefined10PowValue[ DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sLeftVal ) * 2 ]; */
/*         sTempValue = sRightVal / sPow; */
        
/*         if( stlIsinfinite( sTempValue ) != STL_IS_INFINITE_FALSE ) */
/*         { */
/*             /\* overflow: numeric이 real의 최대 표현범위보다 크다. *\/ */
/*             return DTL_COMPARISON_GREATER; */
/*         } */
        
/*         if( ( stlIsnan( sTempValue ) != STL_IS_NAN_FALSE ) || */
/*             ( sTempValue == 0.0 ) ) */
/*         { */
/*             /\* underflow: numeric이 0에 가깝다. 즉, real보다 작다. *\/ */
/*             return DTL_COMPARISON_LESS; */
/*         } */

/*         sDigit = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal ); */

/*         if( sTempValue > -10.0 ) */
/*         { */
/*             sTempValue -= 0.000005; */
/*             if( sTempValue > -1.0 ) */
/*             { */
/*                 return DTL_COMPARISON_LESS; */
/*             } */
/*         } */
/*         else */
/*         { */
/*             sTempValue -= 0.00005; */
/*             if( sTempValue <= -100.0 ) */
/*             { */
/*                 return DTL_COMPARISON_GREATER; */
/*             } */
/*         } */

/*         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[0] ) > -(stlInt32) sTempValue ) */
/*         { */
/*             return DTL_COMPARISON_LESS; */
/*         } */
        
/*         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[0] ) < -(stlInt32) sTempValue ) */
/*         { */
/*             return DTL_COMPARISON_GREATER; */
/*         } */
/*         else */
/*         { */
/*             sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength ); */
/*             switch( sNumLength ) */
/*             { */
/*                 case 3 : */
/*                     { */
/*                         sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[1] ) > -(stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_LESS; */
/*                         } */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[1] ) < -(stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_GREATER; */
/*                         } */

/*                         sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[2] ) > -(stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_LESS; */
/*                         } */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[2] ) < -(stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_GREATER; */
/*                         } */

/*                         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[0] ) > 9 ) */
/*                         { */
/*                             return DTL_COMPARISON_EQUAL; */
/*                         } */
/*                         else */
/*                         { */
/*                             return ( (stlInt32)(( sTempValue - (stlInt32) sTempValue ) * 10 ) < 0 ); */
/*                         } */
/*                     } */
/*                 case 2 : */
/*                     { */
/*                         sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[1] ) > -(stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_LESS; */
/*                         } */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[1] ) < -(stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_GREATER; */
/*                         } */
                            
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[0] ) > 9 ) */
/*                         { */
/*                             return ( (stlInt32)(( sTempValue - (stlInt32) sTempValue ) * 100 ) < 0 ); */
/*                         } */
/*                         else */
/*                         { */
/*                             return ( (stlInt32)(( sTempValue - (stlInt32) sTempValue ) * 1000 ) < 0 ); */
/*                         } */
/*                     } */
/*                 case 1 : */
/*                     { */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[0] ) > 9 ) */
/*                         { */
/*                             return ( (stlInt32)(( sTempValue - (stlInt32) sTempValue ) * 10000 ) < 0 ); */
/*                         } */
/*                         else */
/*                         { */
/*                             return ( (stlInt32)(( sTempValue - (stlInt32) sTempValue ) * 100000 ) < 0 ); */
/*                         } */
/*                     } */
/*                 default : */
/*                     { */
/*                         sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[1] ) > -(stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_LESS; */
/*                         } */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[1] ) < -(stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_GREATER; */
/*                         } */

/*                         sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[2] ) > -(stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_LESS; */
/*                         } */
/*                         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[2] ) < -(stlInt32) sTempValue ) */
/*                         { */
/*                             return DTL_COMPARISON_GREATER; */
/*                         } */

/*                         if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[0] ) > 9 ) */
/*                         { */
/*                             return DTL_COMPARISON_LESS; */
/*                         } */
/*                         else */
/*                         { */
/*                             sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 10; */
/*                             if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[3] ) > -((stlInt32) sTempValue) * 10 ) */
/*                             { */
/*                                 return DTL_COMPARISON_LESS; */
/*                             } */
/*                             if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[3] ) < -((stlInt32) sTempValue) * 10 ) */
/*                             { */
/*                                 return DTL_COMPARISON_GREATER; */
/*                             } */

/*                             return -( sNumLength > 4 ); */
/*                         } */
/*                     } */
/*             } */
/*         } */
/*     } */
/* } */

/* /\** */
/*  * @brief numeric type과 double type간의 Physical Compare 연산 */
/*  * @param[in]  aLeftValue    left value */
/*  * @param[in]  aLeftLength   left value length */
/*  * @param[in]  aRightValue   right value */
/*  * @param[in]  aRightLength  right value length */
/*  * @return dtlCompareResult */
/*  *\/ */
/* dtlCompareResult dtlPhysicalCompareNumericToDouble( void      * aLeftValue, */
/*                                                     stlInt64    aLeftLength, */
/*                                                     void      * aRightValue, */
/*                                                     stlInt64    aRightLength ) */
/* { */
/*     dtlNumericType    * sLeftVal; */
/*     dtlDoubleType       sRightVal; */

/*     stlInt16            sNumLength; */
/*     stlInt32            sLoop; */
/*     stlUInt8          * sDigit; */
/*     stlFloat64          sTempValue; */

/*     DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftValue, sLeftVal ); */
/*     DTL_GET_NATIVE_DOUBLE_FROM_PTR( aRightValue, sRightVal ); */

/*     if( DTL_NUMERIC_IS_POSITIVE( sLeftVal ) ) */
/*     { */
/*         /\* 부호가 다른지 검사 *\/ */
/*         if( sRightVal < 0.0 ) */
/*         { */
/*             return DTL_COMPARISON_GREATER; */
/*         } */

/*         /\* zero 검사 *\/ */
/*         if( DTL_NUMERIC_IS_ZERO( sLeftVal, aLeftLength ) ) */
/*         { */
/*             return ( sRightVal == 0.0 ? DTL_COMPARISON_EQUAL : DTL_COMPARISON_LESS ); */
/*         } */

/*         if( sRightVal == 0.0 ) */
/*         { */
/*             return DTL_COMPARISON_GREATER; */
/*         } */

/*         /\* double 범위 체크 *\/ */
/*         sTempValue = sRightVal / gPredefined10PowValue[ DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sLeftVal ) * 2 ]; */
/*         if( stlIsfinite( sTempValue ) == STL_IS_FINITE_FALSE ) */
/*         { */
/*             return DTL_COMPARISON_LESS; */
/*         } */

/*         if( sTempValue >= 99.99999999999995 ) */
/*         { */
/*             return DTL_COMPARISON_LESS; */
/*         } */

/*         if( sTempValue < 0.999999999999995 ) */
/*         { */
/*             return DTL_COMPARISON_GREATER; */
/*         } */

/*         sDigit = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal ); */
/*         sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength ); */
        
/*         if( sTempValue < 10.0 ) */
/*         { */
/*             sTempValue += 0.000000000000005; */

/*             if( sNumLength >= 8 ) */
/*             { */
/*                 for( sLoop = 0 ; sLoop < 8 ; sLoop++ ) */
/*                 { */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[ sLoop ] ) > (stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_GREATER; */
/*                     } */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[ sLoop ] ) < (stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_LESS; */
/*                     } */
/*                     sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                 } */

/*                 return ( sNumLength > 8 ); */
/*             } */
/*             else */
/*             { */
/*                 for( sLoop = 0 ; sLoop < sNumLength ; sLoop++ ) */
/*                 { */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[ sLoop ] ) > (stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_GREATER; */
/*                     } */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[ sLoop ] ) < (stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_LESS; */
/*                     } */
/*                     sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                 } */

/*                 return -( (stlInt64) ( sTempValue * gPredefined10PowValue[ ( 7 - sLoop ) * 2 ] ) > 0 ); */
/*             } */
/*         } */
/*         else */
/*         { */
/*             sTempValue += 0.00000000000005; */

/*             if( sNumLength >= 8 ) */
/*             { */
/*                 for( sLoop = 0 ; sLoop < 7 ; sLoop++ ) */
/*                 { */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[ sLoop ] ) > (stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_GREATER; */
/*                     } */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[ sLoop ] ) < (stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_LESS; */
/*                     } */
/*                     sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                 } */

/*                 sTempValue = sTempValue / 10; */
/*                 if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[ 7 ] ) < ( (stlInt32) sTempValue ) * 10 ) */
/*                 { */
/*                     return DTL_COMPARISON_LESS; */
/*                 } */
/*                 if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[ 7 ] ) > ( (stlInt32) sTempValue ) * 10 ) */
/*                 { */
/*                     return DTL_COMPARISON_GREATER; */
/*                 } */

/*                 return ( sNumLength > 8 ); */
/*             } */
/*             else */
/*             { */
/*                 for( sLoop = 0 ; sLoop < sNumLength ; sLoop++ ) */
/*                 { */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[ sLoop ] ) > (stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_GREATER; */
/*                     } */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigit[ sLoop ] ) < (stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_LESS; */
/*                     } */
/*                     sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                 } */
                    
/*                 return -( (stlInt64) ( sTempValue * gPredefined10PowValue[ ( 6 - sLoop ) * 2 + 1 ] ) > 0 ); */
/*             } */
/*         } */
/*     } */
/*     else */
/*     { */
/*         /\* 부호가 다른지 검사 *\/ */
/*         if( sRightVal >= 0.0 ) */
/*         { */
/*             return DTL_COMPARISON_LESS; */
/*         } */

/*         /\* double 범위 체크 *\/ */
/*         sTempValue = sRightVal / gPredefined10PowValue[ DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sLeftVal ) * 2 ]; */
/*         if( stlIsfinite( sTempValue ) == STL_IS_FINITE_FALSE ) */
/*         { */
/*             return DTL_COMPARISON_GREATER; */
/*         } */

/*         if( sTempValue > -0.999999999999995 ) */
/*         { */
/*             return DTL_COMPARISON_LESS; */
/*         } */

/*         if( sTempValue <= -99.99999999999995 ) */
/*         { */
/*             return DTL_COMPARISON_GREATER; */
/*         } */

/*         sDigit = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal ); */
/*         sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength ); */
        
/*         if( sTempValue > -10.0 ) */
/*         { */
/*             sTempValue -= 0.000000000000005; */

/*             if( sNumLength >= 8 ) */
/*             { */
/*                 for( sLoop = 0 ; sLoop < 8 ; sLoop++ ) */
/*                 { */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[ sLoop ] ) > -(stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_LESS; */
/*                     } */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[ sLoop ] ) < -(stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_GREATER; */
/*                     } */
/*                     sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                 } */

/*                 return -( sNumLength > 8 ); */
/*             } */
/*             else */
/*             { */
/*                 for( sLoop = 0 ; sLoop < sNumLength ; sLoop++ ) */
/*                 { */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[ sLoop ] ) > -(stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_LESS; */
/*                     } */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[ sLoop ] ) < -(stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_GREATER; */
/*                     } */
/*                     sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                 } */

/*                 return ( (stlInt64) ( sTempValue * gPredefined10PowValue[ ( 7 - sLoop ) * 2 ] ) < 0 ); */
/*             } */
/*         } */
/*         else */
/*         { */
/*             sTempValue -= 0.00000000000005; */

/*             if( sNumLength >= 8 ) */
/*             { */
/*                 for( sLoop = 0 ; sLoop < 7 ; sLoop++ ) */
/*                 { */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[ sLoop ] ) > -(stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_LESS; */
/*                     } */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[ sLoop ] ) < -(stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_GREATER; */
/*                     } */
/*                     sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                 } */

/*                 sTempValue = sTempValue / 100; */
/*                 if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[ 7 ] ) > */
/*                     ( (stlInt32) ( ( sTempValue - (stlInt32) sTempValue ) * 10 ) ) * -10 ) */
/*                 { */
/*                     return DTL_COMPARISON_LESS; */
/*                 } */
/*                 if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[ 7 ] ) < */
/*                     ( (stlInt32) ( ( sTempValue - (stlInt32) sTempValue ) * 10 ) ) * -10 ) */
/*                 { */
/*                     return DTL_COMPARISON_GREATER; */
/*                 } */

/*                 return -( sNumLength > 8 ); */
/*             } */
/*             else */
/*             { */
/*                 for( sLoop = 0 ; sLoop < sNumLength ; sLoop++ ) */
/*                 { */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[ sLoop ] ) > -(stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_LESS; */
/*                     } */
/*                     if( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigit[ sLoop ] ) < -(stlInt32) sTempValue ) */
/*                     { */
/*                         return DTL_COMPARISON_GREATER; */
/*                     } */
/*                     sTempValue = ( sTempValue - (stlInt32) sTempValue ) * 100; */
/*                 } */

/*                 return ( (stlInt64) ( sTempValue * gPredefined10PowValue[ ( 6 - sLoop ) * 2 + 1 ] ) < 0 ); */
/*             } */
/*         } */
/*     } */
/* } */

/**
 * @brief numeric type과 small int type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareNumericToSmallInt( void      * aLeftValue,
                                                      stlInt64    aLeftLength,
                                                      void      * aRightValue,
                                                      stlInt64    aRightLength )
{
    dtlNumericType    * sLeftVal;
    dtlSmallIntType     sRightVal;
    
    stlInt64            sNumExponent;
    stlInt16            sNumLength;
    stlInt64            sTempValue;
    stlInt64            sDiffExpo;
    stlUInt8          * sDigit;

    DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftValue, sLeftVal );
    DTL_GET_NATIVE_SMALLINT_FROM_PTR( aRightValue, sRightVal );
    
    if( DTL_NUMERIC_IS_POSITIVE( sLeftVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sRightVal < 0 )
        {
            return DTL_COMPARISON_GREATER;
        }
        
        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( sLeftVal, aLeftLength ) )
        {
            return ( sRightVal == 0 ? DTL_COMPARISON_EQUAL : DTL_COMPARISON_LESS );
        }

        if( sRightVal == 0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sLeftVal );
        if( sNumExponent > 2 )
        {
            return DTL_COMPARISON_GREATER;
        }

        if( sNumExponent < 0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }

            return ( sTempValue < sRightVal ) ? DTL_COMPARISON_LESS : ( sTempValue > sRightVal );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( sTempValue < sRightVal ) ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER;
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];

                return ( sTempValue < sRightVal ) ? DTL_COMPARISON_LESS : ( sTempValue > sRightVal );
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sRightVal >= 0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sLeftVal );
        if( sNumExponent > 2 )
        {
            return DTL_COMPARISON_LESS;
        }

        if( sNumExponent < 0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }

            sTempValue = -(sTempValue);
            return ( sTempValue < sRightVal ) ? DTL_COMPARISON_LESS : ( sTempValue > sRightVal );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }

                sTempValue = -(sTempValue);
                return ( sTempValue <= sRightVal ) ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER;
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                sTempValue = -(sTempValue);

                return ( sTempValue < sRightVal ) ? DTL_COMPARISON_LESS : ( sTempValue > sRightVal );
            }
        }
    }
}


/**
 * @brief numeric type과 integer type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareNumericToInteger( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength )
{
#if 0

    /*
     * @todo 
     * 성능 개선을 위해 추가된 코드이나, 검증이 필요함.
     * 추후, 코드 검증 후,
     * native ingeger 계열들에 대해 일괄절으로 반영하기로 한다.
     */
    
    dtlNumericType    * sLeftVal;
    dtlIntegerType      sRightVal;

    stlInt32            sExponent;
    stlInt32            sDigitCount;
    stlInt32            sDigit;
    stlUInt8          * sDigitPtr;
    stlInt64            sInteger = 0;
    stlBool             sScale = STL_FALSE;

    DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftValue, sLeftVal );
    DTL_GET_NATIVE_INTEGER_FROM_PTR( aRightValue, sRightVal );

    if( DTL_NUMERIC_IS_POSITIVE( sLeftVal ) )
    {
        if( DTL_NUMERIC_IS_ZERO( sLeftVal, aLeftLength ) == STL_TRUE )
        {
            if( sRightVal > 0 )
            {
                return DTL_COMPARISON_LESS;
            }
            else if( sRightVal < 0 )
            {
                return DTL_COMPARISON_GREATER;
            }
            else
            {
                return DTL_COMPARISON_EQUAL;
            }
        }

        sExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sLeftVal );
        sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength );
        if( sExponent + 1 < sDigitCount )
        {
            /* 소수점이 있는  수 */
            sScale    = STL_TRUE;
            sDigit    = sExponent + 1;
            sExponent = ( sDigit > 5 ) ? sDigitCount - sExponent : 0;
        }
        else
        {
            sDigit    = sDigitCount;
            sExponent = (sExponent - sDigitCount + 1) * 2;
        }
        
        sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal );
        switch( sDigit )
        {
            case 0:
                sInteger = 0;
                break;
            case 1:
                sInteger =
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[0] );
                break;
            case 2:
                sInteger =
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[0] ) * 100 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[1] );
                break;
            case 3:
                sInteger =
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[0] ) * 10000 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[1] ) * 100 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[2] );
                break;
            case 4:
                sInteger =
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[0] ) * 1000000 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[1] ) * 10000 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[2] ) * 100 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[3] );
                break;
            default:
                sInteger =
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[0] ) * 100000000 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[1] ) * 1000000 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[2] ) * 10000 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[3] ) * 100 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[4] );
                break;
        }

        sInteger *= (stlInt64)gPreDefinedPow[ sExponent ];
    }
    else
    {
        sExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sLeftVal );
        sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength );
        if( sExponent + 1 <= sDigitCount )
        {
            /* 소수점이 있는  수 */
            sScale    = STL_TRUE;
            sDigit    = sExponent + 1;
            sExponent = ( sDigit > 5 ) ? sDigitCount - sExponent : 0;
        }
        else
        {
            sDigit    = sDigitCount;
            sExponent = (sExponent - sDigitCount + 1) * 2;
        }
        
        sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal );
        switch( sDigit )
        {
            case 0:
                sInteger = 0;
                break;
            case 1:
                sInteger =
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[0] );
                break;
            case 2:
                sInteger =
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[0] ) * 100 +
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[1] );
                break;
            case 3:
                sInteger =
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[0] ) * 10000 +
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[1] ) * 100 +
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[2] );
                break;
            case 4:
                sInteger =
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[0] ) * 1000000 +
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[1] ) * 10000 +
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[2] ) * 100 +
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[3] );
                break;
            default:
                sInteger =
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[0] ) * 100000000 +
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[1] ) * 1000000 +
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[2] ) * 10000 +
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[3] ) * 100 +
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[4] );
                break;
        }

        sInteger *= -(stlInt64)gPreDefinedPow[ sExponent ];
    }

    if( sInteger < sRightVal )
    {
        return DTL_COMPARISON_LESS;
    }
    else if( sInteger > sRightVal )
    {
        return DTL_COMPARISON_GREATER;
    }
    else
    {
        if( sScale == STL_TRUE )
        {
            return ( sInteger < 0 ) ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER;
        }
        else
        {
            return DTL_COMPARISON_EQUAL;
        }
    }
#else
    dtlNumericType    * sLeftVal;
    dtlIntegerType      sRightVal;
    
    stlInt64            sNumExponent;
    stlInt16            sNumLength;
    stlInt64            sTempValue;
    stlInt64            sDiffExpo;
    stlUInt8          * sDigit;

    DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftValue, sLeftVal );
    DTL_GET_NATIVE_INTEGER_FROM_PTR( aRightValue, sRightVal );
    
    if( DTL_NUMERIC_IS_POSITIVE( sLeftVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sRightVal < 0 )
        {
            return DTL_COMPARISON_GREATER;
        }
        
        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( sLeftVal, aLeftLength ) )
        {
            return ( sRightVal == 0 ? DTL_COMPARISON_EQUAL : DTL_COMPARISON_LESS );
        }

        if( sRightVal == 0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sLeftVal );
        if( sNumExponent > 4 )
        {
            return DTL_COMPARISON_GREATER;
        }

        if( sNumExponent < 0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }

            return ( sTempValue < sRightVal ) ? DTL_COMPARISON_LESS : ( sTempValue > sRightVal );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( sTempValue < sRightVal ) ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER;
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];

                return ( sTempValue < sRightVal ) ? DTL_COMPARISON_LESS : ( sTempValue > sRightVal );
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sRightVal >= 0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sLeftVal );
        if( sNumExponent > 4 )
        {
            return DTL_COMPARISON_LESS;
        }

        if( sNumExponent < 0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }

            sTempValue = -(sTempValue);
            return ( sTempValue < sRightVal ) ? DTL_COMPARISON_LESS : ( sTempValue > sRightVal );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }

                sTempValue = -(sTempValue);
                return ( sTempValue <= sRightVal ) ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER;
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                sTempValue = -(sTempValue);
                
                return ( sTempValue < sRightVal ) ? DTL_COMPARISON_LESS : ( sTempValue > sRightVal );
            }
        }
    }
#endif
}


/**
 * @brief numeric type과 big int type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareNumericToBigInt( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength )
{
    dtlNumericType    * sLeftVal;
    dtlBigIntType       sRightVal;
    
    stlInt64            sNumExponent;
    stlInt16            sNumLength;
    stlUInt64           sTempValue;
    stlUInt64           sTempRightValue;    
    stlInt64            sDiffExpo;
    stlUInt8          * sDigit;

    DTL_GET_NUMERIC_PTR_FROM_PTR( aLeftValue, sLeftVal );
    DTL_GET_NATIVE_BIGINT_FROM_PTR( aRightValue, sRightVal );
    
    if( DTL_NUMERIC_IS_POSITIVE( sLeftVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sRightVal < 0 )
        {
            return DTL_COMPARISON_GREATER;
        }
        
        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( sLeftVal, aLeftLength ) )
        {
            return ( sRightVal == 0 ? DTL_COMPARISON_EQUAL : DTL_COMPARISON_LESS );
        }

        if( sRightVal == 0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sLeftVal );
        if( sNumExponent > 9 )
        {
            return DTL_COMPARISON_GREATER;
        }

        if( sNumExponent < 0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }

            return ( sTempValue < sRightVal ) ? DTL_COMPARISON_LESS : ( sTempValue > sRightVal );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( sTempValue < sRightVal ) ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER;
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];

                return ( sTempValue < sRightVal ) ? DTL_COMPARISON_LESS : ( sTempValue > sRightVal );
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sRightVal >= 0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sLeftVal );
        if( sNumExponent > 9 )
        {
            return DTL_COMPARISON_LESS;
        }

        if( sNumExponent < 0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /*
         * sTempValue가 Int64 범위를 넘어갈 수 있으므로, UInt64로 계산한다.
         * 따라서, sRightVal을 양수로 계산해서 비교함.
         * ( 이 단계에서의 sRightVal은 음수값임 )
         */ 
            
        if( sRightVal == DTL_NATIVE_BIGINT_MIN )
        {
            sTempRightValue = (stlUInt64)(-(sRightVal + 1));
            sTempRightValue += 1;
                
            STL_DASSERT( sTempRightValue == ((stlUInt64)DTL_NATIVE_BIGINT_MAX + (stlUInt64)1) );
        }
        else
        {
            sTempRightValue = (stlUInt64)(-(sRightVal));
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLength );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( sLeftVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }

            return ( sTempValue > sTempRightValue ) ? DTL_COMPARISON_LESS : ( sTempValue < sTempRightValue );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( sTempValue >= sTempRightValue ) ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER;
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];

                return ( sTempValue > sTempRightValue ) ? DTL_COMPARISON_LESS : ( sTempValue < sTempRightValue );
            }
        }
    }
}

/**
 * @brief numeric type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */

dtlCompareResult dtlPhysicalCompareNumericToNumeric( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength )
{
#ifdef STL_SUPPORT_MISSALIGNED_ADDRESS

    register stlUInt8          * sLeftDigit;
    register stlUInt8          * sRightDigit;
    register stlInt32            sLen;
    register dtlCompareResult    sResult;

    if( aLeftLength == aRightLength )
    {
        sLen = aLeftLength;
        sResult = DTL_COMPARISON_EQUAL;
    }
    else if( aLeftLength < aRightLength )
    {
        sLen = aLeftLength;
        sResult = DTL_COMPARISON_LESS;
    }
    else
    {
        sLen = aRightLength;
        sResult = DTL_COMPARISON_GREATER;
    }

    sLeftDigit  = (stlUInt8*)aLeftValue;
    sRightDigit = (stlUInt8*)aRightValue;

    while( sLen >= 4 )
    {
        if( *((stlUInt32*)sLeftDigit) == *((stlUInt32*)sRightDigit) )
        {
            sLeftDigit += 4;
            sRightDigit += 4;
            sLen -= 4;
            continue;
        }

        if( *((stlUInt16*)sLeftDigit) == *((stlUInt16*)sRightDigit) )
        {
            if( sLeftDigit[2] == sRightDigit[2] )
            {
                if( sLeftDigit[3] < sRightDigit[3] )
                {
                    return DTL_COMPARISON_LESS;
                }
                else
                {
                    return DTL_COMPARISON_GREATER;
                }
            }
            else if( sLeftDigit[2] < sRightDigit[2] )
            {
                return DTL_COMPARISON_LESS;
            }
            else
            {
                return DTL_COMPARISON_GREATER;
            }
        }
        else
        {
            if( sLeftDigit[0] == sRightDigit[0] )
            {
                if( sLeftDigit[1] < sRightDigit[1] )
                {
                    return DTL_COMPARISON_LESS;
                }
                else
                {
                    return DTL_COMPARISON_GREATER;
                }
            }
            else if( sLeftDigit[0] < sRightDigit[0] )
            {
                return DTL_COMPARISON_LESS;
            }
            else
            {
                return DTL_COMPARISON_GREATER;
            }
        }
    }

    switch( sLen )
    {
        case 1:
            if( sLeftDigit[0] < sRightDigit[0] )
            {
                return DTL_COMPARISON_LESS;
            }
            else if( sLeftDigit[0] > sRightDigit[0] )
            {
                return DTL_COMPARISON_GREATER;
            }

            if( ((stlUInt8*)aLeftValue)[0] > 127 )
            {
                return sResult;
            }
            else
            {
                return -sResult;
            }
            break;
        case 2:
            if( sLeftDigit[0] == sRightDigit[0] )
            {
                if( sLeftDigit[1] < sRightDigit[1] )
                {
                    return DTL_COMPARISON_LESS;
                }
                else if( sLeftDigit[1] > sRightDigit[1] )
                {
                    return DTL_COMPARISON_GREATER;
                }
                else
                {
                    if( ((stlUInt8*)aLeftValue)[0] > 127 )
                    {
                        return sResult;
                    }
                    else
                    {
                        return -sResult;
                    }
                }
            }
            else if( sLeftDigit[0] < sRightDigit[0] )
            {
                return DTL_COMPARISON_LESS;
            }
            else
            {
                return DTL_COMPARISON_GREATER;
            }
            break;
        case 3:
            if( *((stlUInt16*)sLeftDigit) == *((stlUInt16*)sRightDigit) )
            {
                if( sLeftDigit[2] < sRightDigit[2] )
                {
                    return DTL_COMPARISON_LESS;
                }
                else if( sLeftDigit[2] > sRightDigit[2] )
                {
                    return DTL_COMPARISON_GREATER;
                }
                else
                {
                    if( ((stlUInt8*)aLeftValue)[0] > 127 )
                    {
                        return sResult;
                    }
                    else
                    {
                        return -sResult;
                    }
                }
            }
            else
            {
                if( sLeftDigit[0] == sRightDigit[0] )
                {
                    if( sLeftDigit[1] < sRightDigit[1] )
                    {
                        return DTL_COMPARISON_LESS;
                    }
                    else
                    {
                        return DTL_COMPARISON_GREATER;
                    }
                }
                else if( sLeftDigit[0] < sRightDigit[0] )
                {
                    return DTL_COMPARISON_LESS;
                }
                else
                {
                    return DTL_COMPARISON_GREATER;
                }
            }
            break;
        default:
            if( ((stlUInt8*)aLeftValue)[0] > 127 )
            {
                return sResult;
            }
            else
            {
                return -sResult;
            }
            break;
    }

    /* 여기 오기전에 이미 return되어야 한다. */
    STL_DASSERT( 0 );

    return sResult;
#else
    register stlUInt8          * sLeftDigit;
    register stlUInt8          * sRightDigit;
    register stlInt8           * sTable;
    register stlInt32            i;

    sLeftDigit  = (stlUInt8*)aLeftValue;
    sRightDigit = (stlUInt8*)aRightValue;

    if( sLeftDigit[0] == sRightDigit[0] )
    {
        sTable = dtlNumericLenCompTable[ aLeftLength ][ aRightLength ];

        if( sTable[1] == 0 )
        {
            if( sLeftDigit[0] > 127 )
            {
                return sTable[0];
            }
            else
            {
                return -sTable[0];
            }
        }

        for( i = 1; sLeftDigit[i] == sRightDigit[i]; i++ )
        {
            if( i == sTable[1] )
            {
                if( sLeftDigit[0] > 127 )
                {
                    return sTable[0];
                }
                else
                {
                    return -sTable[0];
                }
            }
        }

        return dtlNumericCompResult[ sLeftDigit[i] < sRightDigit[i] ];
    }

    return dtlNumericCompResult[ sLeftDigit[0] < sRightDigit[0] ];
#endif
}

/**
 * @brief small int type과 numeric type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareSmallIntToNumeric( void      * aLeftValue,
                                                      stlInt64    aLeftLength,
                                                      void      * aRightValue,
                                                      stlInt64    aRightLength )
{
    dtlSmallIntType     sLeftVal;
    dtlNumericType    * sRightVal;
    
    stlInt64            sNumExponent;
    stlInt16            sNumLength;
    stlInt64            sTempValue;
    stlInt64            sDiffExpo;
    stlUInt8          * sDigit;

    DTL_GET_NATIVE_SMALLINT_FROM_PTR( aLeftValue, sLeftVal );
    DTL_GET_NUMERIC_PTR_FROM_PTR( aRightValue, sRightVal );
    
    if( DTL_NUMERIC_IS_POSITIVE( sRightVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sLeftVal < 0 )
        {
            return DTL_COMPARISON_LESS;
        }
        
        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( sRightVal, aRightLength ) )
        {
            return ( sLeftVal == 0 ? DTL_COMPARISON_EQUAL : DTL_COMPARISON_GREATER );
        }

        if( sLeftVal == 0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sRightVal );
        if( sNumExponent > 2 )
        {
            return DTL_COMPARISON_LESS;
        }

        if( sNumExponent < 0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLength );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( sRightVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }

            return ( sLeftVal < sTempValue ) ? DTL_COMPARISON_LESS : ( sLeftVal > sTempValue );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( sLeftVal <= sTempValue ) ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER;
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];

                return ( sLeftVal < sTempValue ) ? DTL_COMPARISON_LESS : ( sLeftVal > sTempValue );
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sLeftVal >= 0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sRightVal );
        if( sNumExponent > 2 )
        {
            return DTL_COMPARISON_GREATER;
        }

        if( sNumExponent < 0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLength );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( sRightVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }

            sTempValue = -(sTempValue);
            return ( sLeftVal < sTempValue ) ? DTL_COMPARISON_LESS : ( sLeftVal > sTempValue );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                sTempValue = -(sTempValue);
                return ( sLeftVal < sTempValue ) ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER;
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                sTempValue = -(sTempValue);
                return ( sLeftVal < sTempValue ) ? DTL_COMPARISON_LESS : ( sLeftVal > sTempValue );
            }
        }
    }
}

/**
 * @brief integer type과 numeric type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareIntegerToNumeric( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength )
{
    dtlIntegerType      sLeftVal;
    dtlNumericType    * sRightVal;
    
    stlInt64            sNumExponent;
    stlInt16            sNumLength;
    stlInt64            sTempValue;
    stlInt64            sDiffExpo;
    stlUInt8          * sDigit;

    DTL_GET_NATIVE_INTEGER_FROM_PTR( aLeftValue, sLeftVal );
    DTL_GET_NUMERIC_PTR_FROM_PTR( aRightValue, sRightVal );
    
    if( DTL_NUMERIC_IS_POSITIVE( sRightVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sLeftVal < 0 )
        {
            return DTL_COMPARISON_LESS;
        }
        
        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( sRightVal, aRightLength ) )
        {
            return ( sLeftVal == 0 ? DTL_COMPARISON_EQUAL : DTL_COMPARISON_GREATER );
        }

        if( sLeftVal == 0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sRightVal );
        if( sNumExponent > 4 )
        {
            return DTL_COMPARISON_LESS;
        }

        if( sNumExponent < 0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLength );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( sRightVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }

            return ( sLeftVal < sTempValue ) ? DTL_COMPARISON_LESS : ( sLeftVal > sTempValue );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( sLeftVal <= sTempValue ) ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER;
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];

                return ( sLeftVal < sTempValue ) ? DTL_COMPARISON_LESS : ( sLeftVal > sTempValue );
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sLeftVal >= 0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sRightVal );
        if( sNumExponent > 4 )
        {
            return DTL_COMPARISON_GREATER;
        }

        if( sNumExponent < 0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLength );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( sRightVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }

            sTempValue = -(sTempValue);
            return ( sLeftVal < sTempValue ) ? DTL_COMPARISON_LESS : ( sLeftVal > sTempValue );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                sTempValue = -(sTempValue);
                return ( sLeftVal < sTempValue ) ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER;
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                sTempValue = -(sTempValue);
                return ( sLeftVal < sTempValue ) ? DTL_COMPARISON_LESS : ( sLeftVal > sTempValue );
            }
        }
    }
}

/**
 * @brief big int type과 numeric type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareBigIntToNumeric( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength )
{
    dtlBigIntType       sLeftVal;
    dtlNumericType    * sRightVal;
    
    stlInt64            sNumExponent;
    stlInt16            sNumLength;
    stlUInt64           sTempValue;
    stlUInt64           sTempLeftValue;        
    stlInt64            sDiffExpo;
    stlUInt8          * sDigit;

    DTL_GET_NATIVE_BIGINT_FROM_PTR( aLeftValue, sLeftVal );
    DTL_GET_NUMERIC_PTR_FROM_PTR( aRightValue, sRightVal );
    
    if( DTL_NUMERIC_IS_POSITIVE( sRightVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sLeftVal < 0 )
        {
            return DTL_COMPARISON_LESS;
        }
        
        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( sRightVal, aRightLength ) )
        {
            return ( sLeftVal == 0 ? DTL_COMPARISON_EQUAL : DTL_COMPARISON_GREATER );
        }

        if( sLeftVal == 0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sRightVal );
        if( sNumExponent > 9 )
        {
            return DTL_COMPARISON_LESS;
        }

        if( sNumExponent < 0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLength );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( sRightVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }

            return ( sLeftVal < sTempValue ) ? DTL_COMPARISON_LESS : ( sLeftVal > sTempValue );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( sLeftVal <= sTempValue ) ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER;
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];

                return ( sLeftVal < sTempValue ) ? DTL_COMPARISON_LESS : ( sLeftVal > sTempValue );
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sLeftVal >= 0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sRightVal );
        if( sNumExponent > 9 )
        {
            return DTL_COMPARISON_GREATER;
        }

        if( sNumExponent < 0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /*
         * sTempValue가 Int64 범위를 넘어갈 수 있으므로, UInt64로 계산한다.
         * 따라서, sRightVal을 양수로 계산해서 비교함.
         * ( 이 단계에서의 sLeftVal은 음수값임 )
         */ 
            
        if( sLeftVal == DTL_NATIVE_BIGINT_MIN )
        {
            sTempLeftValue = (stlUInt64)(-(sLeftVal + 1));
            sTempLeftValue += 1;
                
            STL_DASSERT( sTempLeftValue == ((stlUInt64)DTL_NATIVE_BIGINT_MAX + (stlUInt64)1) );
        }
        else
        {
            sTempLeftValue = (stlUInt64)(-(sLeftVal));
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLength );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( sRightVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }

            return ( sTempLeftValue > sTempValue ) ? DTL_COMPARISON_LESS : ( sTempLeftValue < sTempValue );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( sTempLeftValue > sTempValue ) ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER;
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];

                return ( sTempLeftValue > sTempValue ) ? DTL_COMPARISON_LESS : ( sTempLeftValue < sTempValue );
            }
        }
    }
}

/**
 * @brief real type과 numeric type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareRealToNumeric( void      * aLeftValue,
                                                  stlInt64    aLeftLength,
                                                  void      * aRightValue,
                                                  stlInt64    aRightLength )
{
    dtlRealType         sLeftVal;
    dtlNumericType    * sRightVal;

    stlInt16            sNumLength;
    stlUInt8          * sDigit;

    stlInt32            sMaxExponent;
    stlInt32            sNumExponent;
    stlInt32            sRoundExponent;
    stlInt64            sLeftInt;
    stlInt64            sRightInt;

    DTL_GET_NATIVE_REAL_FROM_PTR( aLeftValue, sLeftVal );
    DTL_GET_NUMERIC_PTR_FROM_PTR( aRightValue, sRightVal );

    if( DTL_NUMERIC_IS_POSITIVE( sRightVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sLeftVal < 0.0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( sRightVal, aRightLength ) )
        {
            return ( sLeftVal == 0.0 ? DTL_COMPARISON_EQUAL : DTL_COMPARISON_GREATER );
        }

        if( sLeftVal == 0.0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* real 범위 체크 */
        if( sLeftVal < 1.0 )
        {
            sMaxExponent = stlLog10f( sLeftVal ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10f( sLeftVal );
        }
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sRightVal );

        if( sMaxExponent < sNumExponent * 2 )
        {
            return DTL_COMPARISON_LESS;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return DTL_COMPARISON_GREATER;
        }

        sRoundExponent = STL_FLT_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sLeftInt = (( ((stlInt64) ( sLeftVal * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10 ) * 100;
        }
        else
        {
            sLeftInt = (( ((stlInt64) ( sLeftVal * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( sRightVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLength );
        sRightInt = 0;

        if( sNumLength > 4 )
        {
            sNumLength = 4;
            while( sNumLength )
            {
                sRightInt *= 100;
                sRightInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }

            if( sLeftInt <= sRightInt )
            {
                return DTL_COMPARISON_LESS; 
            }
            else
            {
                return DTL_COMPARISON_GREATER;
            }
        }
        else
        {
            sNumExponent = 4 - sNumLength;

            while( sNumLength )
            {
                sRightInt *= 100;
                sRightInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sRightInt *= gPreDefinedPow[ sNumExponent * 2 ];

            if( sLeftInt < sRightInt )
            {
                return DTL_COMPARISON_LESS; 
            }
            else
            {
                return sLeftInt > sRightInt;
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sLeftVal >= 0.0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* real 범위 체크 */
        if( sLeftVal > -1.0 )
        {
            sMaxExponent = stlLog10f( -sLeftVal ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10f( -sLeftVal );
        }
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sRightVal );

        if( sMaxExponent < sNumExponent * 2 )
        {
            return DTL_COMPARISON_GREATER;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return DTL_COMPARISON_LESS;
        }

        sRoundExponent = STL_FLT_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sLeftInt = (( ((stlInt64) ( sLeftVal * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10 ) * 100;
        }
        else
        {
            sLeftInt = (( ((stlInt64) ( sLeftVal * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( sRightVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLength );
        sRightInt = 0;

        if( sNumLength > 4 )
        {
            sNumLength = 4;
            while( sNumLength )
            {
                sRightInt *= 100;
                sRightInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }

            if( sLeftInt < sRightInt )
            {
                return DTL_COMPARISON_LESS; 
            }
            else
            {
                return DTL_COMPARISON_GREATER;
            }
        }
        else
        {
            sNumExponent = 4 - sNumLength;

            while( sNumLength )
            {
                sRightInt *= 100;
                sRightInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sRightInt *= gPreDefinedPow[ sNumExponent * 2 ];

            if( sLeftInt < sRightInt )
            {
                return DTL_COMPARISON_LESS; 
            }
            else
            {
                return sLeftInt > sRightInt;
            }
        }
    }
}

/**
 * @brief double type과 numeric type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareDoubleToNumeric( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength )
{
    dtlDoubleType       sLeftVal;
    dtlNumericType    * sRightVal;

    stlInt16            sNumLength;
    stlUInt8          * sDigit;

    stlInt32            sMaxExponent;
    stlInt32            sNumExponent;
    stlInt32            sRoundExponent;
    stlInt64            sLeftInt;
    stlInt64            sRightInt;

    DTL_GET_NATIVE_DOUBLE_FROM_PTR( aLeftValue, sLeftVal );
    DTL_GET_NUMERIC_PTR_FROM_PTR( aRightValue, sRightVal );

    if( DTL_NUMERIC_IS_POSITIVE( sRightVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sLeftVal < 0.0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( sRightVal, aRightLength ) )
        {
            return ( sLeftVal == 0.0 ? DTL_COMPARISON_EQUAL : DTL_COMPARISON_GREATER );
        }

        if( sLeftVal == 0.0 )
        {
            return DTL_COMPARISON_LESS;
        }

        /* double 범위 체크 */
        if( sLeftVal < 1.0 )
        {
            sMaxExponent = stlLog10( sLeftVal ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10( sLeftVal );
        }
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sRightVal );

        if( sMaxExponent < sNumExponent * 2 )
        {
            return DTL_COMPARISON_LESS;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return DTL_COMPARISON_GREATER;
        }

        sRoundExponent = STL_DBL_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sLeftInt = ( ((stlInt64) ( sLeftVal * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10;
        }
        else
        {
            sLeftInt = (( ((stlInt64) ( sLeftVal * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( sRightVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLength );
        sRightInt = 0;

        if( sNumLength > 8 )
        {
            sNumLength = 8;
            while( sNumLength )
            {
                sRightInt *= 100;
                sRightInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }

            if( sLeftInt <= sRightInt )
            {
                return DTL_COMPARISON_LESS; 
            }
            else
            {
                return DTL_COMPARISON_GREATER;
            }
        }
        else
        {
            sNumExponent = 8 - sNumLength;

            while( sNumLength )
            {
                sRightInt *= 100;
                sRightInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sRightInt *= gPreDefinedPow[ sNumExponent * 2 ];

            if( sLeftInt < sRightInt )
            {
                return DTL_COMPARISON_LESS; 
            }
            else
            {
                return sLeftInt > sRightInt;
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sLeftVal >= 0.0 )
        {
            return DTL_COMPARISON_GREATER;
        }

        /* double 범위 체크 */
        if( sLeftVal > -1.0 )
        {
            sMaxExponent = stlLog10( -sLeftVal ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10( -sLeftVal );
        }
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sRightVal );

        if( sMaxExponent < sNumExponent * 2 )
        {
            return DTL_COMPARISON_GREATER;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return DTL_COMPARISON_LESS;
        }

        sRoundExponent = STL_DBL_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sLeftInt = ( ((stlInt64) ( sLeftVal * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10;
        }
        else
        {
            sLeftInt = (( ((stlInt64) ( sLeftVal * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( sRightVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLength );
        sRightInt = 0;

        if( sNumLength > 8 )
        {
            sNumLength = 8;
            while( sNumLength )
            {
                sRightInt *= 100;
                sRightInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }

            if( sLeftInt < sRightInt )
            {
                return DTL_COMPARISON_LESS; 
            }
            else
            {
                return DTL_COMPARISON_GREATER;
            }
        }
        else
        {
            sNumExponent = 8 - sNumLength;

            while( sNumLength )
            {
                sRightInt *= 100;
                sRightInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sRightInt *= gPreDefinedPow[ sNumExponent * 2 ];

            if( sLeftInt < sRightInt )
            {
                return DTL_COMPARISON_LESS; 
            }
            else
            {
                return sLeftInt > sRightInt;
            }
        }
    }
}


/*******************************************************************************
 * TYPE : CHARACTER STRING
 ******************************************************************************/

/**
 * @brief 고정 길이 character string type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 *
 * @remark 평가 방법 : 앞에서부터 space padding이 나오기전까지 비교
 *    <BR> character string의 Compare에서 Char/Char의 경우만 해당
 */
dtlCompareResult dtlPhysicalCompareFixedLengthChar( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength )
{
    stlInt64         sCompLen;
    stlInt64         sDiffLen;
    stlUInt8       * sLeftValue  = (stlUInt8 *) aLeftValue;
    stlUInt8       * sRightValue = (stlUInt8 *) aRightValue;

    
    if( (sDiffLen = (aLeftLength - aRightLength)) > 0 )
    {
        sCompLen = aRightLength;
    }
    else
    {
        sCompLen = aLeftLength;
    }

    while( sCompLen > 0 )
    {
        if( *sLeftValue == *sRightValue )
        {
            /* Do Nothing */
        }
        else
        {
            if( *sLeftValue < *sRightValue )
            {
                return DTL_COMPARISON_LESS;
            }
            else
            {
                return DTL_COMPARISON_GREATER;
            }
        }

        ++sLeftValue;
        ++sRightValue;
        --sCompLen;
    }

    if( sDiffLen == 0 )
    {
        return DTL_COMPARISON_EQUAL;
    }
    else
    {
        if( sDiffLen > 0 )
        {
            while( (sDiffLen > 0) & (*sLeftValue == ' ') )
            {
                ++sLeftValue;
                --sDiffLen;
            }

            if( sDiffLen == 0 )
            {
                return DTL_COMPARISON_EQUAL;
            }
            else
            {
                return ( *sLeftValue < ' ' ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER );
            }
        }
        else
        {
            while( (sDiffLen < 0) & (*sRightValue == ' ') )
            {
                ++sRightValue;
                ++sDiffLen;
            }

            if( sDiffLen == 0 )
            {
                return DTL_COMPARISON_EQUAL;
            }
            else
            {
                return ( *sRightValue > ' ' ? DTL_COMPARISON_LESS : DTL_COMPARISON_GREATER );
            }
        }
    }
}

/**
 * @brief 가변 길이 character string type을 포함한 Compare 연산 
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 *
 * @remark 평가 방법 : value 비교
 *    <BR> character string의 Compare에서 Char/Char 제외한 모든 경우에 해당
 */
dtlCompareResult dtlPhysicalCompareVariableLengthChar( void      * aLeftValue,
                                                       stlInt64    aLeftLength,
                                                       void      * aRightValue,
                                                       stlInt64    aRightLength )
{
    stlInt32         sCompResult;
    
    /**
     * @todo locale 지원여부가 ... 
     */ 

    sCompResult = stlMemcmp( aLeftValue,
                             aRightValue,
                             ( aLeftLength < aRightLength ) ? aLeftLength : aRightLength );

    if( sCompResult == 0 )
    {
        sCompResult = aLeftLength - aRightLength;
    }

    if( sCompResult < 0 )
    {
        return DTL_COMPARISON_LESS;
    }
    else
    {
        return sCompResult > 0;
    }
}


/*******************************************************************************
 * TYPE : COMPARE
 ******************************************************************************/
/**
 * @brief 고정 길이 binary string type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareFixedLengthBinary( void      * aLeftValue,
                                                      stlInt64    aLeftLength,
                                                      void      * aRightValue,
                                                      stlInt64    aRightLength )
{
    stlInt32         sCompResult;
    stlInt64         sCompLength;
    stlChar        * sValue;
    stlUInt64        sLength;

    sCompLength = ( aLeftLength < aRightLength ) ? aLeftLength : aRightLength;
    sCompResult = stlMemcmp( aLeftValue,
                             aRightValue,
                             sCompLength );

    if( ( sCompResult == 0 ) &&
        ( aLeftLength != aRightLength ) )
    {
        /* case 1) 처리 */
        if( aLeftLength > aRightLength )
        {
            sValue = ((stlChar*)aLeftValue) + sCompLength;
            sLength = aLeftLength - sCompLength;
            
            while( (sLength > 0) && (*sValue == 0x00) )
            {
                ++sValue;
                --sLength;
            }
            
            return (sLength == 0) == STL_FALSE;
        }
        else
        {
            sValue = ((stlChar*)aRightValue) + sCompLength;
            sLength = aRightLength - sCompLength;
            
            while( (sLength > 0) && (*sValue == 0x00) )
            {
                ++sValue;
                --sLength;
            }
            
            sCompResult = ( (sLength == 0) == STL_FALSE );
            
            return ( -sCompResult );
        }
    }
    else
    {
        if( sCompResult < 0 )
        {
            return DTL_COMPARISON_LESS;
        }
        else
        {
            return ( sCompResult > 0 );
        }
    }

    return sCompResult;
}

/**
 * @brief 가변 길이 binary string type을 포함한 Compare 연산 
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 *
 * @remark 평가 방법 : value 비교
 *    <BR> binary string의 Compare에서 Binary/Binary 제외한 모든 경우에 해당
 */
dtlCompareResult dtlPhysicalCompareVariableLengthBinary( void      * aLeftValue,
                                                         stlInt64    aLeftLength,
                                                         void      * aRightValue,
                                                         stlInt64    aRightLength )
{
    stlInt32         sCompResult;

    sCompResult = stlMemcmp( aLeftValue,
                             aRightValue,
                             ( aLeftLength < aRightLength ) ? aLeftLength : aRightLength );

    if( sCompResult == 0 )
    {
        sCompResult = ( aLeftLength - aRightLength );
    }

    if( sCompResult < 0 )
    {
        return DTL_COMPARISON_LESS;
    }
    else
    {
        return ( sCompResult > 0 );
    }
}


/*******************************************************************************
 * TYPE : DATE
 ******************************************************************************/
/**
 * @brief Date type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareDateToDate( void      * aLeftValue,
                                               stlInt64    aLeftLength,
                                               void      * aRightValue,
                                               stlInt64    aRightLength )
{
    dtlDateType sLeft;
    dtlDateType sRight;

    DTL_GET_DATE_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_DATE_FROM_PTR( aRightValue, sRight );
    
    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief Date vs Timestamp type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareDateToTimestamp( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength )
{
    dtlDateType      sLeft;
    dtlTimestampType sRight;
    
    DTL_GET_DATE_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_TIMESTAMP_FROM_PTR( aRightValue, sRight );
    
    return ( sLeft > sRight ) - ( sLeft < sRight );
}


/*******************************************************************************
 * TYPE : TIME
 ******************************************************************************/
/**
 * @brief Time type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareTimeToTime( void      * aLeftValue,
                                               stlInt64    aLeftLength,
                                               void      * aRightValue,
                                               stlInt64    aRightLength )
{
    dtlTimeType sLeft;
    dtlTimeType sRight;

    DTL_GET_TIME_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_TIME_FROM_PTR( aRightValue, sRight );
    
    return ( sLeft > sRight ) - ( sLeft < sRight );
}


/*******************************************************************************
 * TYPE : COMPARE
 ******************************************************************************/
/**
 * @brief Timestamp vs Date type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareTimestampToDate( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength )
{
    dtlTimestampType sLeft;
    dtlDateType      sRight;
    
    DTL_GET_TIMESTAMP_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_DATE_FROM_PTR( aRightValue, sRight );

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/**
 * @brief Timestamp type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareTimestampToTimestamp( void      * aLeftValue,
                                                         stlInt64    aLeftLength,
                                                         void      * aRightValue,
                                                         stlInt64    aRightLength )
{
    dtlTimestampType sLeft;
    dtlTimestampType sRight;

    DTL_GET_TIMESTAMP_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_TIMESTAMP_FROM_PTR( aRightValue, sRight );

    return ( sLeft > sRight ) - ( sLeft < sRight );
}

/*******************************************************************************
 * TYPE : COMPARE
 ******************************************************************************/
/**
 * @brief TimeTz type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareTimeTzToTimeTz( void      * aLeftValue,
                                                   stlInt64    aLeftLength,
                                                   void      * aRightValue,
                                                   stlInt64    aRightLength )
{
    dtlTimeTzType  sLeft;
    dtlTimeTzType  sRight;

    DTL_GET_TIME_TZ_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_TIME_TZ_FROM_PTR( aRightValue, sRight );

    return ( sLeft.mTime > sRight.mTime ) - ( sLeft.mTime < sRight.mTime );    
}


/*******************************************************************************
 * TYPE : COMPARE
 ******************************************************************************/
/**
 * @brief TimestampTz type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareTimestampTzToTimestampTz( void      * aLeftValue,
                                                             stlInt64    aLeftLength,
                                                             void      * aRightValue,
                                                             stlInt64    aRightLength )
{
    dtlTimestampTzType sLeft;
    dtlTimestampTzType sRight;
    
    DTL_GET_TIMESTAMP_TZ_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_TIMESTAMP_TZ_FROM_PTR( aRightValue, sRight );

    return ( sLeft.mTimestamp > sRight.mTimestamp ) - ( sLeft.mTimestamp < sRight.mTimestamp );
}


/*******************************************************************************
 * TYPE : COMPARE
 ******************************************************************************/
/**
 * @brief dtlIntervalYearToMonth type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareIntervalYearToMonth( void      * aLeftValue,
                                                        stlInt64    aLeftLength,
                                                        void      * aRightValue,
                                                        stlInt64    aRightLength )
{
    dtlIntervalYearToMonthType   sLeft;
    dtlIntervalYearToMonthType   sRight;

    DTL_GET_INTERVAL_YEAR_TO_MONTH_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_INTERVAL_YEAR_TO_MONTH_FROM_PTR( aRightValue, sRight );
        
    return ( sLeft.mMonth > sRight.mMonth ) - ( sLeft.mMonth < sRight.mMonth );
}


/*******************************************************************************
 * TYPE : COMPARE
 ******************************************************************************/
/**
 * @brief dtlIntervalDayToSecond type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareIntervalDayToSecond( void      * aLeftValue,
                                                        stlInt64    aLeftLength,
                                                        void      * aRightValue,
                                                        stlInt64    aRightLength )
{
    dtlIntervalDayToSecondType  sLeft;
    dtlIntervalDayToSecondType  sRight;
    
    DTL_GET_INTERVAL_DAY_TO_SECOND_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_INTERVAL_DAY_TO_SECOND_FROM_PTR( aRightValue, sRight );
    
    return ( ( DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE( & sLeft ) > DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE( & sRight ) ) -
             ( DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE( & sLeft ) < DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE( & sRight ) ) );
}


/*******************************************************************************
 * TYPE : COMPARE
 ******************************************************************************/
/**
 * @brief dtlRowId type간의 Physical Compare 연산
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 */
dtlCompareResult dtlPhysicalCompareRowId( void      * aLeftValue,
                                          stlInt64    aLeftLength,
                                          void      * aRightValue,
                                          stlInt64    aRightLength )
{
    dtlRowIdType  * sLeft;
    dtlRowIdType  * sRight;
    stlInt32        sCompResult;
    
    DTL_GET_ROWID_FROM_PTR( aLeftValue, sLeft );
    DTL_GET_ROWID_FROM_PTR( aRightValue, sRight );
    
    sCompResult = stlMemcmp( sLeft->mDigits,
                             sRight->mDigits,
                             DTL_ROWID_SIZE );

    if( sCompResult == 0 )
    {
        return DTL_COMPARISON_EQUAL;
    }
    else
    {
        if( sCompResult < 0 )
        {
            return DTL_COMPARISON_LESS;
        }
        else
        {
            return DTL_COMPARISON_GREATER;
        }
    }
}


/*******************************************************************************
 * TYPE : LIKE Function
 ******************************************************************************/

/**
 * @brief Like Function을 위한 Compare 연산 
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 *
 * @remark Like Pattern은 _나 %를 포함한 뒷 문자열이 제거된 문자열
 */
dtlCompareResult dtlLikePhysicalCompareChar( void      * aLeftValue,
                                             stlInt64    aLeftLength,
                                             void      * aRightValue,
                                             stlInt64    aRightLength )
{
    stlInt32         sCompResult;
    
    STL_DASSERT( aRightValue != NULL );
    STL_DASSERT( aRightLength > 0 );

    /**
     * @todo locale 지원여부가 ... 
     */

    if( aLeftLength < aRightLength )
    {
        sCompResult = stlMemcmp( aLeftValue,
                                 aRightValue,
                                 aLeftLength );

        if( sCompResult > 0 )
        {
            return DTL_COMPARISON_GREATER;
        }
        else
        {
            return DTL_COMPARISON_LESS;
        }
    }
    else
    {
        sCompResult = stlMemcmp( aLeftValue,
                                 aRightValue,
                                 aRightLength );
        
        if( sCompResult < 0 )
        {
            return DTL_COMPARISON_LESS;
        }
        else
        {
            return sCompResult > 0;
        }
    }
}


/**
 * @brief Like All Function을 위한 Compare 연산 
 * @param[in]  aLeftValue    left value
 * @param[in]  aLeftLength   left value length
 * @param[in]  aRightValue   right value
 * @param[in]  aRightLength  right value length
 * @return dtlCompareResult
 *
 * @remark 모든 문자열이 Like 연산을 만족한다.
 */
dtlCompareResult dtlLikeAllPhysicalCompareChar( void      * aLeftValue,
                                                stlInt64    aLeftLength,
                                                void      * aRightValue,
                                                stlInt64    aRightLength )
{
    return DTL_COMPARISON_EQUAL;
}


/** @} dtmPhysicalComp */
 
